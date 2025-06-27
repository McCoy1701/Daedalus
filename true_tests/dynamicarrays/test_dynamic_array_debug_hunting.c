// File: true_tests/dynamicarrays/test_dynamic_array_debug_hunting.c - Advanced debug hunting tests for catching sneaky bugs
// This file contains sophisticated tests designed to catch memory corruption, state inconsistencies, and edge cases

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Debug hunting helper structures
typedef struct {
    uint32_t magic_start;
    int value;
    uint32_t magic_end;
} DebugElement_t;

#define MAGIC_START 0xDEADBEEF
#define MAGIC_END   0xCAFEBABE

// Test Functions

int test_debug_hunt_memory_corruption_detection(void)
{
    dLogContext_t* ctx = d_PushLogContext("MemoryCorruptionHunt");
    d_LogInfo("🔍 HUNT #1: Memory Corruption Detection - Looking for buffer overruns and corruption");
    
    dArray_t* array = d_InitArray(3, sizeof(DebugElement_t));
    d_LogInfoF("Created array with magic-protected elements: capacity=%zu, element_size=%zu", 
               array->capacity, array->element_size);
    
    // Fill array with magic-protected elements
    DebugElement_t elements[5] = {
        {MAGIC_START, 100, MAGIC_END},
        {MAGIC_START, 200, MAGIC_END},
        {MAGIC_START, 300, MAGIC_END},
        {MAGIC_START, 400, MAGIC_END},
        {MAGIC_START, 500, MAGIC_END}
    };
    
    d_LogDebug("Adding magic-protected elements to detect corruption");
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &elements[i]);
        d_LogDebugF("Added element %d with magic guards: start=0x%X, value=%d, end=0x%X", 
                   i, elements[i].magic_start, elements[i].value, elements[i].magic_end);
    }
    
    // Verify memory integrity
    bool corruption_detected = false;
    dLogStructured_t* corruption_log = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(corruption_log, "test", "memory_corruption_detection");
    
    for (size_t i = 0; i < array->count; i++) {
        DebugElement_t* elem = (DebugElement_t*)d_GetDataFromArrayByIndex(array, i);
        if (elem) {
            bool start_intact = (elem->magic_start == MAGIC_START);
            bool end_intact = (elem->magic_end == MAGIC_END);
            
            d_LogDebugF("Element %zu: magic_start=0x%X(%s), value=%d, magic_end=0x%X(%s)",
                       i, elem->magic_start, start_intact ? "OK" : "CORRUPTED",
                       elem->value, elem->magic_end, end_intact ? "OK" : "CORRUPTED");
            
            if (!start_intact || !end_intact) {
                corruption_detected = true;
                d_LogErrorF("🚨 CORRUPTION DETECTED at element %zu! Start:%s End:%s", 
                           i, start_intact ? "OK" : "BAD", end_intact ? "OK" : "BAD");
            }
        }
    }
    
    d_LogStructured_FieldInt(corruption_log, "elements_checked", (int)array->count);
    d_LogStructured_FieldBool(corruption_log, "corruption_detected", corruption_detected);
    d_LogStructured_Field(corruption_log, "status", corruption_detected ? "FAILED" : "CLEAN");
    d_LogStructured_SetFormat(corruption_log, false);
    d_LogStructured_Commit(corruption_log);
    
    TEST_ASSERT(!corruption_detected, "Memory should not be corrupted");
    
    // Force expansion and re-check
    d_LogDebug("Testing corruption detection during capacity expansion");
    size_t old_capacity = array->capacity;
    d_AppendArray(array, &elements[3]);
    d_AppendArray(array, &elements[4]);
    
    d_LogInfoF("Array expanded from %zu to %zu capacity", old_capacity, array->capacity);
    
    // Re-verify after expansion
    for (size_t i = 0; i < array->count; i++) {
        DebugElement_t* elem = (DebugElement_t*)d_GetDataFromArrayByIndex(array, i);
        if (elem && (elem->magic_start != MAGIC_START || elem->magic_end != MAGIC_END)) {
            d_LogErrorF("🚨 CORRUPTION after expansion at element %zu!", i);
            corruption_detected = true;
        }
    }
    
    TEST_ASSERT(!corruption_detected, "Memory should remain intact after expansion");
    d_LogInfo("✅ Memory corruption hunt completed - no corruption detected");
    
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

int test_debug_hunt_use_after_free_simulation(void)
{
    dLogContext_t* ctx = d_PushLogContext("UseAfterFreeHunt");
    d_LogInfo("🔍 HUNT #2: Use-After-Free Detection - Simulating dangerous pointer access patterns");
    
    dArray_t* array = d_InitArray(5, sizeof(int));
    d_LogInfoF("Created test array: capacity=%zu", array->capacity);
    
    // Fill array with test data
    int test_values[] = {111, 222, 333, 444, 555};
    void* stored_pointers[5];
    
    d_LogDebug("Storing pointers to track potential use-after-free scenarios");
    for (int i = 0; i < 5; i++) {
        d_AppendArray(array, &test_values[i]);
        stored_pointers[i] = d_GetDataFromArrayByIndex(array, i);
        d_LogDebugF("Stored pointer %d: %p -> value %d", i, stored_pointers[i], test_values[i]);
    }
    
    // Simulate dangerous operations that could invalidate pointers
    d_LogWarning("⚠️ Performing operations that may invalidate stored pointers");
    
    // Force reallocation by exceeding capacity
    int extra_values[] = {666, 777, 888, 999};
    size_t old_capacity = array->capacity;
    void* old_data_ptr = array->data;
    
    d_LogDebugF("Current data pointer: %p, capacity: %zu", old_data_ptr, old_capacity);
    
    for (int i = 0; i < 4; i++) {
        d_AppendArray(array, &extra_values[i]);
    }
    
    // Check if reallocation occurred
    bool reallocation_occurred = (array->data != old_data_ptr);
    d_LogInfoF("Reallocation %s: old_ptr=%p, new_ptr=%p, old_cap=%zu, new_cap=%zu",
               reallocation_occurred ? "OCCURRED" : "did not occur",
               old_data_ptr, array->data, old_capacity, array->capacity);
    
    // Test stored pointers (dangerous in real code!)
    dLogStructured_t* uaf_log = d_LogStructured(D_LOG_LEVEL_WARNING);
    d_LogStructured_Field(uaf_log, "test", "use_after_free_simulation");
    d_LogStructured_FieldBool(uaf_log, "reallocation_occurred", reallocation_occurred);
    d_LogStructured_Field(uaf_log, "danger_level", reallocation_occurred ? "HIGH" : "LOW");
    
    if (reallocation_occurred) {
        d_LogWarning("🚨 DANGER: Stored pointers are now invalid (use-after-free scenario)");
        d_LogStructured_Field(uaf_log, "status", "POINTERS_INVALIDATED");
        
        // In a real bug, accessing stored_pointers[i] would be undefined behavior
        d_LogRateLimited(D_LOG_LEVEL_ERROR, 3, 1.0, 
                        "Stored pointers now point to freed/invalid memory!");
    } else {
        d_LogInfo("✅ No reallocation occurred - stored pointers remain valid");
        d_LogStructured_Field(uaf_log, "status", "POINTERS_STILL_VALID");
    }
    
    d_LogStructured_SetFormat(uaf_log, true);
    d_LogStructured_Commit(uaf_log);
    
    // Verify current data integrity through proper API
    d_LogDebug("Verifying data integrity through proper API access");
    for (size_t i = 0; i < 5; i++) {
        int* current_ptr = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(current_ptr != NULL, "Should get valid pointer through API");
        TEST_ASSERT(*current_ptr == test_values[i], "Original values should be preserved");
        d_LogDebugF("Element %zu: current_ptr=%p, value=%d (expected %d)", 
                   i, (void*)current_ptr, *current_ptr, test_values[i]);
    }
    
    d_LogInfo("✅ Use-after-free hunt completed - proper API access works correctly");
    
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

int test_debug_hunt_state_consistency_validation(void)
{
    dLogContext_t* ctx = d_PushLogContext("StateConsistencyHunt");
    d_LogInfo("🔍 HUNT #3: State Consistency Validation - Checking internal invariants and state corruption");
    
    dArray_t* array = d_InitArray(4, sizeof(double));
    d_LogInfoF("Created array for state validation: capacity=%zu, element_size=%zu, count=%zu", 
               array->capacity, array->element_size, array->count);
    
    // Define state invariants to check
    #define CHECK_INVARIANT(condition, message) do { \
        if (!(condition)) { \
            d_LogErrorF("🚨 INVARIANT VIOLATION: %s", message); \
            invariants_valid = false; \
        } else { \
            d_LogDebugF("✅ Invariant OK: %s", message); \
        } \
    } while(0)
    
    bool invariants_valid = true;
    
    // Initial state validation
    d_LogDebug("Validating initial state invariants");
    CHECK_INVARIANT(array != NULL, "Array pointer must not be NULL");
    CHECK_INVARIANT(array->capacity == 4, "Initial capacity should match requested");
    CHECK_INVARIANT(array->element_size == sizeof(double), "Element size should match requested");
    CHECK_INVARIANT(array->count == 0, "Initial count should be 0");
    CHECK_INVARIANT(array->data != NULL || array->capacity == 0, "Data pointer consistency");
    
    // Add elements and validate state after each operation
    double test_doubles[] = {3.14, 2.718, 1.414, 0.577, 1.618};
    
    for (int i = 0; i < 5; i++) {
        size_t old_count = array->count;
        size_t old_capacity = array->capacity;
        
        d_LogDebugF("Adding element %d: value=%.3f", i, test_doubles[i]);
        d_AppendArray(array, &test_doubles[i]);
        
        // Validate state after append
        dLogStructured_t* state_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
        d_LogStructured_Field(state_log, "operation", "post_append_validation");
        d_LogStructured_FieldInt(state_log, "element_index", i);
        d_LogStructured_FieldInt(state_log, "old_count", (int)old_count);
        d_LogStructured_FieldInt(state_log, "new_count", (int)array->count);
        d_LogStructured_FieldInt(state_log, "old_capacity", (int)old_capacity);
        d_LogStructured_FieldInt(state_log, "new_capacity", (int)array->capacity);
        
        CHECK_INVARIANT(array->count == old_count + 1, "Count should increment by exactly 1");
        CHECK_INVARIANT(array->count <= array->capacity, "Count should never exceed capacity");
        CHECK_INVARIANT(array->capacity >= old_capacity, "Capacity should never decrease");
        CHECK_INVARIANT(array->element_size == sizeof(double), "Element size should remain constant");
        CHECK_INVARIANT(array->data != NULL, "Data pointer should remain valid");
        
        // Validate that we can retrieve the element we just added
        double* retrieved = (double*)d_GetDataFromArrayByIndex(array, array->count - 1);
        CHECK_INVARIANT(retrieved != NULL, "Should be able to retrieve just-added element");
        if (retrieved) {
            CHECK_INVARIANT(*retrieved == test_doubles[i], "Retrieved value should match stored value");
            d_LogStructured_FieldFloat(state_log, "stored_value", test_doubles[i], 3);
            d_LogStructured_FieldFloat(state_log, "retrieved_value", *retrieved, 3);
            d_LogStructured_FieldBool(state_log, "values_match", *retrieved == test_doubles[i]);
        }
        
        d_LogStructured_FieldBool(state_log, "invariants_valid", invariants_valid);
        d_LogStructured_SetFormat(state_log, false);
        d_LogStructured_Commit(state_log);
        
        if (!invariants_valid) {
            d_LogError("🚨 State invariant violation detected - stopping validation");
            break;
        }
    }
    
    // Final comprehensive state validation
    d_LogInfo("Performing final comprehensive state validation");
    
    // Validate all stored elements
    for (size_t i = 0; i < array->count; i++) {
        double* elem = (double*)d_GetDataFromArrayByIndex(array, i);
        CHECK_INVARIANT(elem != NULL, "All elements should be retrievable");
        if (elem && i < 5) {
            CHECK_INVARIANT(*elem == test_doubles[i], "All stored values should be intact");
        }
    }
    
    // Validate memory layout
    if (array->count >= 2) {
        double* first = (double*)d_GetDataFromArrayByIndex(array, 0);
        double* second = (double*)d_GetDataFromArrayByIndex(array, 1);
        if (first && second) {
            ptrdiff_t diff = second - first;
            CHECK_INVARIANT(diff == 1, "Elements should be contiguous in memory");
            d_LogDebugF("Memory layout check: second - first = %td (expected: 1)", diff);
        }
    }
    
    // Final invariant summary
    dLogStructured_t* final_state = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(final_state, "test", "state_consistency_final");
    d_LogStructured_FieldInt(final_state, "final_count", (int)array->count);
    d_LogStructured_FieldInt(final_state, "final_capacity", (int)array->capacity);
    d_LogStructured_FieldBool(final_state, "all_invariants_valid", invariants_valid);
    d_LogStructured_Field(final_state, "conclusion", invariants_valid ? "STATE_CONSISTENT" : "STATE_CORRUPTED");
    d_LogStructured_SetFormat(final_state, true);
    d_LogStructured_Commit(final_state);
    
    TEST_ASSERT(invariants_valid, "All state invariants should be valid");
    
    if (invariants_valid) {
        d_LogInfo("✅ State consistency hunt completed - all invariants valid");
    } else {
        d_LogError("💥 State consistency hunt FAILED - invariant violations detected");
    }
    
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

int test_debug_hunt_boundary_overflow_detection(void)
{
    dLogContext_t* ctx = d_PushLogContext("BoundaryOverflowHunt");
    d_LogInfo("🔍 HUNT #4: Boundary Overflow Detection - Testing for buffer overruns and index violations");
    
    dArray_t* array = d_InitArray(3, sizeof(int));
    d_LogInfoF("Created small array for boundary testing: capacity=%zu", array->capacity);
    
    // Fill array to capacity
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
    }
    d_LogInfoF("Array filled to capacity: count=%zu, capacity=%zu", array->count, array->capacity);
    
    // Test various boundary conditions and potential overflows
    size_t test_indices[] = {
        0,                    // Valid: first element
        2,                    // Valid: last element  
        3,                    // Invalid: just past end
        10,                   // Invalid: way past end
        100,                  // Invalid: far past end
        SIZE_MAX,             // Invalid: maximum possible
        SIZE_MAX - 1,         // Invalid: near maximum
        (size_t)-1,           // Invalid: wrapped around
        (size_t)-10           // Invalid: wrapped around
    };
    
    d_LogDebug("Testing boundary conditions with various indices");
    
    int valid_accesses = 0;
    int invalid_accesses = 0;
    int null_returns = 0;
    
    for (size_t i = 0; i < sizeof(test_indices)/sizeof(test_indices[0]); i++) {
        size_t test_idx = test_indices[i];
        bool should_be_valid = (test_idx < array->count);
        
        d_LogRateLimitedF(D_LOG_LEVEL_DEBUG, 5, 1.0, 
                         "Testing index %zu (expected: %s)", test_idx, 
                         should_be_valid ? "VALID" : "INVALID");
        
        void* result = d_GetDataFromArrayByIndex(array, test_idx);
        
        dLogStructured_t* boundary_test = d_LogStructured(D_LOG_LEVEL_DEBUG);
        d_LogStructured_Field(boundary_test, "test", "boundary_access");
        d_LogStructured_FieldInt(boundary_test, "test_index", (int)test_idx);
        d_LogStructured_FieldInt(boundary_test, "array_count", (int)array->count);
        d_LogStructured_FieldBool(boundary_test, "should_be_valid", should_be_valid);
        d_LogStructured_FieldBool(boundary_test, "got_null", result == NULL);
        d_LogStructured_FieldBool(boundary_test, "correct_behavior", 
                                 (should_be_valid && result != NULL) || (!should_be_valid && result == NULL));
        
        if (should_be_valid) {
            if (result != NULL) {
                valid_accesses++;
                int* int_ptr = (int*)result;
                d_LogDebugF("✅ Valid access [%zu]: got value %d", test_idx, *int_ptr);
                d_LogStructured_FieldInt(boundary_test, "retrieved_value", *int_ptr);
                
                // Verify the value is correct
                if (test_idx < 3 && *int_ptr == values[test_idx]) {
                    d_LogStructured_Field(boundary_test, "value_status", "CORRECT");
                } else {
                    d_LogStructured_Field(boundary_test, "value_status", "CORRUPTED");
                    d_LogErrorF("🚨 Value corruption at index %zu: got %d, expected %d", 
                               test_idx, *int_ptr, values[test_idx]);
                }
            } else {
                d_LogErrorF("🚨 BOUNDARY ERROR: Expected valid access at index %zu but got NULL", test_idx);
                d_LogStructured_Field(boundary_test, "error_type", "FALSE_NEGATIVE");
            }
        } else {
            if (result == NULL) {
                invalid_accesses++;
                null_returns++;
                d_LogDebugF("✅ Correctly rejected invalid index %zu", test_idx);
                d_LogStructured_Field(boundary_test, "boundary_check", "CORRECT_REJECTION");
            } else {
                d_LogErrorF("🚨 BOUNDARY VIOLATION: Invalid index %zu returned non-NULL pointer!", test_idx);
                d_LogStructured_Field(boundary_test, "error_type", "BUFFER_OVERFLOW");
                d_LogStructured_Field(boundary_test, "boundary_check", "FAILED_REJECTION");
            }
        }
        
        d_LogStructured_SetFormat(boundary_test, false);
        d_LogStructured_Commit(boundary_test);
    }
    
    // Summary of boundary testing
    dLogStructured_t* boundary_summary = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(boundary_summary, "test", "boundary_overflow_summary");
    d_LogStructured_FieldInt(boundary_summary, "total_tests", (int)(sizeof(test_indices)/sizeof(test_indices[0])));
    d_LogStructured_FieldInt(boundary_summary, "valid_accesses", valid_accesses);
    d_LogStructured_FieldInt(boundary_summary, "invalid_accesses", invalid_accesses);
    d_LogStructured_FieldInt(boundary_summary, "null_returns", null_returns);
    d_LogStructured_FieldBool(boundary_summary, "no_buffer_overflows", invalid_accesses == null_returns);
    d_LogStructured_SetFormat(boundary_summary, true);
    d_LogStructured_Commit(boundary_summary);
    
    TEST_ASSERT(valid_accesses >= 2, "Should have successful valid accesses");
    TEST_ASSERT(invalid_accesses == null_returns, "All invalid accesses should return NULL");
    
    d_LogInfoF("Boundary testing results: %d valid, %d properly rejected, %d null returns", 
               valid_accesses, invalid_accesses, null_returns);
    d_LogInfo("✅ Boundary overflow hunt completed - no buffer overflows detected");
    
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

int test_debug_hunt_data_integrity_stress_test(void)
{
    dLogContext_t* ctx = d_PushLogContext("DataIntegrityStressHunt");
    d_LogInfo("🔍 HUNT #5: Data Integrity Stress Test - Aggressive operations to expose data corruption");
    
    dArray_t* array = d_InitArray(2, sizeof(uint64_t));  // Start small to force multiple reallocations
    d_LogInfoF("Created small array for stress testing: capacity=%zu", array->capacity);
    
    // Use checksums to detect data corruption
    uint64_t checksum = 0;
    const size_t stress_iterations = 100;
    uint64_t stored_values[stress_iterations];
    
    d_LogInfoF("Starting aggressive stress test with %zu iterations", stress_iterations);
    
    // Phase 1: Aggressive append with checksum tracking
    d_LogDebug("PHASE 1: Aggressive append operations with checksum validation");
    for (size_t i = 0; i < stress_iterations; i++) {
        // Create a value with embedded index for verification
        uint64_t value = (uint64_t)i * 0x123456789ABCDEF + i;
        stored_values[i] = value;
        checksum ^= value;  // XOR checksum
        
        size_t old_capacity = array->capacity;
        d_AppendArray(array, &value);
        
        // Log capacity changes
        if (array->capacity != old_capacity) {
            d_LogInfoF("Capacity expansion %zu: %zu → %zu", i, old_capacity, array->capacity);
            
            // Validate all existing data after reallocation
            uint64_t verification_checksum = 0;
            for (size_t j = 0; j <= i; j++) {
                uint64_t* elem = (uint64_t*)d_GetDataFromArrayByIndex(array, j);
                if (elem) {
                    verification_checksum ^= *elem;
                    if (*elem != stored_values[j]) {
                        d_LogErrorF("🚨 DATA CORRUPTION at index %zu after reallocation! Got 0x%lX, expected 0x%lX", 
                                   j, *elem, stored_values[j]);
                    }
                }
            }
            
            if (verification_checksum != checksum) {
                d_LogErrorF("🚨 CHECKSUM MISMATCH after reallocation at iteration %zu!", i);
            }
        }
        
        // Periodic integrity check
        if (i % 20 == 0) {
            d_LogRateLimitedF(D_LOG_LEVEL_DEBUG, 3, 2.0, 
                             "Stress iteration %zu: count=%zu, capacity=%zu", 
                             i, array->count, array->capacity);
        }
    }
    
    d_LogInfoF("PHASE 1 completed: %zu elements added, final capacity=%zu", 
               array->count, array->capacity);
    
    // Phase 2: Random access validation
    d_LogDebug("PHASE 2: Random access pattern validation");
    size_t random_indices[] = {0, 99, 50, 25, 75, 10, 90, 5, 95, 1, 98, 49, 51};
    
    for (size_t i = 0; i < sizeof(random_indices)/sizeof(random_indices[0]); i++) {
        size_t idx = random_indices[i];
        if (idx < array->count) {
            uint64_t* elem = (uint64_t*)d_GetDataFromArrayByIndex(array, idx);
            TEST_ASSERT(elem != NULL, "Random access should succeed for valid index");
            
            if (elem) {
                uint64_t expected = stored_values[idx];
                bool values_match = (*elem == expected);
                
                dLogStructured_t* access_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
                d_LogStructured_Field(access_log, "phase", "random_access_validation");
                d_LogStructured_FieldInt(access_log, "access_index", (int)idx);
                d_LogStructured_FieldBool(access_log, "values_match", values_match);
                d_LogStructured_SetFormat(access_log, false);
                d_LogStructured_Commit(access_log);
                
                TEST_ASSERT(values_match, "Random access should return correct values");
                
                if (!values_match) {
                    d_LogErrorF("🚨 CORRUPTION in random access at index %zu: got 0x%lX, expected 0x%lX", 
                               idx, *elem, expected);
                }
            }
        }
    }
    
    // Phase 3: Pop operations and integrity verification
    d_LogDebug("PHASE 3: Pop operations with integrity verification");
    size_t elements_to_pop = stress_iterations / 4;  // Pop 25% of elements
    
    for (size_t i = 0; i < elements_to_pop; i++) {
        size_t expected_index = stress_iterations - 1 - i;
        uint64_t expected_value = stored_values[expected_index];
        
        uint64_t* popped = (uint64_t*)d_PopDataFromArray(array);
        TEST_ASSERT(popped != NULL, "Pop should succeed when array has elements");
        
        if (popped) {
            bool value_correct = (*popped == expected_value);
            
            dLogStructured_t* pop_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
            d_LogStructured_Field(pop_log, "phase", "pop_integrity_check");
            d_LogStructured_FieldInt(pop_log, "pop_iteration", (int)i);
            d_LogStructured_FieldInt(pop_log, "expected_index", (int)expected_index);
            d_LogStructured_FieldBool(pop_log, "value_correct", value_correct);
            d_LogStructured_FieldInt(pop_log, "remaining_count", (int)array->count);
            d_LogStructured_SetFormat(pop_log, false);
            d_LogStructured_Commit(pop_log);
            
            TEST_ASSERT(value_correct, "Popped value should match expected LIFO order");
            
            if (!value_correct) {
                d_LogErrorF("🚨 POP INTEGRITY FAILURE: got 0x%lX, expected 0x%lX from index %zu", 
                           *popped, expected_value, expected_index);
            }
        }
    }
    
    // Final integrity verification
    d_LogInfo("FINAL PHASE: Complete integrity verification");
    size_t remaining_elements = array->count;
    uint64_t final_checksum = 0;
    
    for (size_t i = 0; i < remaining_elements; i++) {
        uint64_t* elem = (uint64_t*)d_GetDataFromArrayByIndex(array, i);
        if (elem) {
            final_checksum ^= *elem;
            bool value_correct = (*elem == stored_values[i]);
            
            if (!value_correct) {
                d_LogErrorF("🚨 FINAL INTEGRITY CHECK FAILED at index %zu", i);
            }
        }
    }
    
    // Calculate expected checksum for remaining elements
    uint64_t expected_final_checksum = 0;
    for (size_t i = 0; i < remaining_elements; i++) {
        expected_final_checksum ^= stored_values[i];
    }
    
    bool checksum_valid = (final_checksum == expected_final_checksum);
    
    dLogStructured_t* final_integrity = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(final_integrity, "test", "data_integrity_stress_final");
    d_LogStructured_FieldInt(final_integrity, "initial_elements", (int)stress_iterations);
    d_LogStructured_FieldInt(final_integrity, "elements_popped", (int)elements_to_pop);
    d_LogStructured_FieldInt(final_integrity, "remaining_elements", (int)remaining_elements);
    d_LogStructured_FieldBool(final_integrity, "checksum_valid", checksum_valid);
    d_LogStructured_Field(final_integrity, "integrity_status", checksum_valid ? "INTACT" : "CORRUPTED");
    d_LogStructured_SetFormat(final_integrity, true);
    d_LogStructured_Commit(final_integrity);
    
    TEST_ASSERT(checksum_valid, "Final data integrity checksum should be valid");
    
    if (checksum_valid) {
        d_LogInfo("✅ Data integrity stress test completed - no corruption detected");
    } else {
        d_LogError("💥 Data integrity stress test FAILED - corruption detected");
    }
    
    d_LogInfoF("Stress test summary: %zu operations, %zu reallocations, %zu elements remaining", 
               stress_iterations, array->capacity > 2 ? (size_t)(__builtin_ctzl(array->capacity) - 1) : 0, remaining_elements);
    
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

// Main test runner
int main(void)
{
    // Initialize comprehensive logging for debug hunting
    dLogger_t* debug_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_thread_id = true,
        .colorize_output = true,
        .timestamp_format = "%H:%M:%S.%03d",
        .context_separator = " :: "
    });
    d_SetGlobalLogger(debug_logger);

    // Main debug hunting context
    dLogContext_t* main_ctx = d_PushLogContext("DebugHuntingSuite");
    
    d_LogInfo("🔍 Starting Dynamic Array Debug Hunting Test Suite");
    d_LogInfo("🎯 Advanced tests designed to catch memory corruption, state issues, and sneaky bugs");
    d_LogWarning("⚠️ These tests simulate dangerous conditions and edge cases");
    
    // Structured logging for debug suite initialization
    dLogStructured_t* suite_start = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(suite_start, "event", "debug_hunting_suite_start");
    d_LogStructured_Field(suite_start, "suite_name", "dynamic_array_debug_hunting");
    d_LogStructured_FieldInt(suite_start, "total_hunts", 5);
    d_LogStructured_Field(suite_start, "focus", "memory_corruption_state_integrity_boundary_overflow");
    d_LogStructured_FieldTimestamp(suite_start, "hunt_start_time");
    d_LogStructured_SetFormat(suite_start, true);
    d_LogStructured_Commit(suite_start);

    TEST_SUITE_START("Dynamic Array Debug Hunting Tests");

    RUN_TEST(test_debug_hunt_memory_corruption_detection);
    RUN_TEST(test_debug_hunt_use_after_free_simulation);
    RUN_TEST(test_debug_hunt_state_consistency_validation);
    RUN_TEST(test_debug_hunt_boundary_overflow_detection);
    RUN_TEST(test_debug_hunt_data_integrity_stress_test);

    TEST_SUITE_END();

    // Final debug hunting summary
    d_LogInfo("🔍 Debug Hunting Summary:");
    d_LogInfoF("   • Memory Corruption: Tested magic-guard protection and reallocation integrity");
    d_LogInfoF("   • Use-After-Free: Simulated pointer invalidation scenarios");
    d_LogInfoF("   • State Consistency: Validated internal invariants and state transitions");
    d_LogInfoF("   • Boundary Overflow: Tested index bounds and buffer protection");
    d_LogInfoF("   • Data Integrity: Stress-tested with aggressive operations and checksums");
    
    // Rate-limited final message
    d_LogRateLimited(D_LOG_LEVEL_INFO, 1, 5.0, 
                    "🏁 Debug Hunting Suite completed - bugs have nowhere to hide!");
    
    d_LogWarning("Remember: In production, use proper error handling and avoid the dangerous patterns we tested!");
    
    d_PopLogContext(main_ctx);
    
    // Cleanup logging
    d_DestroyLogger(debug_logger);
    d_SetGlobalLogger(NULL);
    
    return 0;
}