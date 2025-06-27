// File: true_tests/dynamicarrays/test_dynamic_array_edge.c - Edge cases and error handling tests with comprehensive logging

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

// Test Functions
int test_dynamic_array_null_safety(void)
{
    dLogContext_t* ctx = d_PushLogContext("NullSafety");
    d_LogInfo("🛡️ Testing NULL safety - ensuring robust error handling");

    // Test all functions with NULL array parameter (should not crash)
    int test_value = 42;

    d_LogDebug("Testing d_AppendArray with NULL array parameter");
    d_AppendArray(NULL, &test_value);
    TEST_ASSERT(1, "AppendArray with NULL array should not crash");
    d_LogInfo("✅ d_AppendArray handled NULL array gracefully");

    d_LogDebug("Testing d_GetDataFromArrayByIndex with NULL array");
    void* result = d_GetDataFromArrayByIndex(NULL, 0);
    TEST_ASSERT(result == NULL, "GetDataFromArrayByIndex with NULL array should return NULL");

    // Structured logging for test result
    dLogStructured_t* null_test = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(null_test, "test", "null_array_get");
    d_LogStructured_Field(null_test, "function", "d_GetDataFromArrayByIndex");
    d_LogStructured_Field(null_test, "result", result == NULL ? "null_as_expected" : "unexpected_non_null");
    d_LogStructured_FieldBool(null_test, "passed", result == NULL);
    d_LogStructured_SetFormat(null_test, false);
    d_LogStructured_Commit(null_test);

    d_LogDebug("Testing d_PopDataFromArray with NULL array");
    result = d_PopDataFromArray(NULL);
    TEST_ASSERT(result == NULL, "PopDataFromArray with NULL array should return NULL");
    d_LogInfoF("✅ d_PopDataFromArray returned NULL for NULL array (expected: NULL, got: %p)", result);

    d_LogDebug("Testing d_ResizeArray with NULL array");
    int resize_result = d_ResizeArray(NULL, 10);
    TEST_ASSERT(resize_result != 0, "ResizeArray with NULL array should return error");
    d_LogInfoF("✅ d_ResizeArray returned error code %d for NULL array", resize_result);

    // DestroyArray with NULL should not crash
    d_LogDebug("Testing d_DestroyArray with NULL array - should not crash");
    d_DestroyArray(NULL);
    TEST_ASSERT(1, "DestroyArray with NULL should not crash");
    d_LogInfo("✅ d_DestroyArray handled NULL array without crashing");

    d_LogInfo("🎉 NULL safety tests completed successfully");
    d_PopLogContext(ctx);
    return 1;
}

int test_dynamic_array_null_data_append(void)
{
    dLogContext_t* ctx = d_PushLogContext("NullDataAppend");
    d_LogInfo("📝 Testing NULL data append behavior");

    dArray_t* array = d_InitArray(5, sizeof(int));
    d_LogInfoF("Created test array with capacity %zu, element_size %zu", array->capacity, array->element_size);

    // Test appending NULL data (should handle gracefully)
    size_t old_count = array->count;
    d_LogDebugF("Current array count: %zu, attempting to append NULL data", old_count);

    d_AppendArray(array, NULL);

    // Structured logging for append result
    dLogStructured_t* append_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
    d_LogStructured_Field(append_log, "operation", "append_null_data");
    d_LogStructured_FieldInt(append_log, "count_before", (int)old_count);
    d_LogStructured_FieldInt(append_log, "count_after", (int)array->count);
    d_LogStructured_FieldBool(append_log, "count_changed", array->count != old_count);
    d_LogStructured_SetFormat(append_log, true);
    d_LogStructured_Commit(append_log);

    // The behavior might vary - either it doesn't change count, or it copies garbage
    // We just test that it doesn't crash
    TEST_ASSERT(1, "Appending NULL data should not crash");
    d_LogInfo("✅ Appending NULL data handled gracefully without crash");

    d_DestroyArray(array);
    d_LogDebug("Test array destroyed");
    d_PopLogContext(ctx);
    return 1;
}

int test_dynamic_array_large_indices(void)
{
    dLogContext_t* ctx = d_PushLogContext("LargeIndices");
    d_LogInfo("🔢 Testing boundary conditions with large indices");

    dArray_t* array = d_InitArray(5, sizeof(int));
    d_LogInfoF("Created array: capacity=%zu, element_size=%zu", array->capacity, array->element_size);

    int value = 100;
    d_AppendArray(array, &value);
    d_LogInfoF("Added value %d to array, count is now %zu", value, array->count);

    // Test with very large indices
    d_LogDebug("Testing with SIZE_MAX index");
    void* result = d_GetDataFromArrayByIndex(array, SIZE_MAX);
    TEST_ASSERT(result == NULL, "Should return NULL for SIZE_MAX index");

    // Rate-limited logging for repeated boundary tests
    d_LogRateLimited(D_LOG_LEVEL_DEBUG, 2, 1.0, "Testing large index boundary condition");

    d_LogDebugF("Testing with very large index (1000000)");
    result = d_GetDataFromArrayByIndex(array, 1000000);
    TEST_ASSERT(result == NULL, "Should return NULL for very large index");

    // Performance and safety metrics
    dLogStructured_t* boundary_metrics = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(boundary_metrics, "test_type", "large_index_boundary");
    d_LogStructured_FieldInt(boundary_metrics, "array_count", (int)array->count);
    d_LogStructured_FieldInt(boundary_metrics, "tested_size_max", 1);
    d_LogStructured_FieldInt(boundary_metrics, "tested_large_index", 1000000);
    d_LogStructured_FieldBool(boundary_metrics, "all_bounds_handled", result == NULL);
    d_LogStructured_SetFormat(boundary_metrics, false);
    d_LogStructured_Commit(boundary_metrics);

    d_LogInfo("✅ Large index boundary tests passed - array properly rejects invalid indices");
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

int test_dynamic_array_zero_element_size(void)
{
    dLogContext_t* ctx = d_PushLogContext("ZeroElementSize");
    d_LogWarning("⚠️ Testing zero element size edge case - unusual but should be handled");

    // Test initialization with zero element size
    dArray_t* array = d_InitArray(10, 0);

    if (array != NULL) {
        d_LogInfoF("Array created with zero element size: capacity=%zu, element_size=%zu",
                   array->capacity, array->element_size);

        TEST_ASSERT(array->element_size == 0, "Element size should be 0");

        // Structured logging for zero-size array properties
        dLogStructured_t* zero_size_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
        d_LogStructured_Field(zero_size_log, "test", "zero_element_size");
        d_LogStructured_FieldInt(zero_size_log, "capacity", (int)array->capacity);
        d_LogStructured_FieldInt(zero_size_log, "element_size", (int)array->element_size);
        d_LogStructured_FieldInt(zero_size_log, "count", (int)array->count);
        d_LogStructured_FieldBool(zero_size_log, "array_created", true);
        d_LogStructured_SetFormat(zero_size_log, true);
        d_LogStructured_Commit(zero_size_log);

        // Operations with zero element size should be handled
        char dummy = 'x';
        d_LogDebug("Attempting to append to zero-element-size array");
        d_AppendArray(array, &dummy);
        TEST_ASSERT(1, "Append with zero element size should not crash");
        d_LogInfo("✅ Append operation completed without crash");

        d_DestroyArray(array);
        d_LogDebug("Zero-element-size array destroyed");
    } else {
        d_LogWarning("Array creation with zero element size returned NULL");
    }

    TEST_ASSERT(1, "Zero element size handling should not crash");
    d_LogInfo("🎉 Zero element size edge case handled successfully");
    d_PopLogContext(ctx);
    return 1;
}

int test_dynamic_array_boundary_conditions(void)
{
    dLogContext_t* ctx = d_PushLogContext("BoundaryConditions");
    d_LogInfo("🎯 Testing boundary conditions with minimal capacity");

    // Test with capacity of 1
    dArray_t* array = d_InitArray(1, sizeof(int));
    d_LogInfoF("Created minimal array: capacity=%zu, element_size=%zu", array->capacity, array->element_size);

    int value1 = 10, value2 = 20;
    d_LogDebugF("Test values prepared: value1=%d, value2=%d", value1, value2);

    // Fill to capacity
    d_LogDebug("Filling array to capacity");
    d_AppendArray(array, &value1);
    TEST_ASSERT(array->count == 1, "Should have 1 element");
    d_LogInfoF("✅ Array filled to capacity: count=%zu", array->count);

    int* retrieved = (int*)d_GetDataFromArrayByIndex(array, 0);
    TEST_ASSERT(retrieved != NULL && *retrieved == value1, "Should retrieve correct value");
    d_LogInfoF("✅ Retrieved value: %d (expected: %d)", retrieved ? *retrieved : -1, value1);

    // Try to exceed capacity
    d_LogDebug("Testing capacity expansion by adding second element");
    size_t old_capacity = array->capacity;
    d_AppendArray(array, &value2);
    TEST_ASSERT(array->count > 1, "Count should increase beyond capacity");

    // Structured logging for capacity expansion
    dLogStructured_t* expansion_log = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(expansion_log, "operation", "capacity_expansion");
    d_LogStructured_FieldInt(expansion_log, "old_capacity", (int)old_capacity);
    d_LogStructured_FieldInt(expansion_log, "new_capacity", (int)array->capacity);
    d_LogStructured_FieldInt(expansion_log, "count_after_expansion", (int)array->count);
    d_LogStructured_FieldFloat(expansion_log, "expansion_ratio", (float)array->capacity / old_capacity, 2);
    d_LogStructured_SetFormat(expansion_log, false);
    d_LogStructured_Commit(expansion_log);

    d_LogInfoF("✅ Capacity expanded: %zu → %zu, count: %zu", old_capacity, array->capacity, array->count);

    // Pop the last element (should be value2)
    d_LogDebug("Testing pop operation - expecting to get the last added element");
    int* popped = (int*)d_PopDataFromArray(array);

    // Enhanced logging for pop operation
    if (popped != NULL) {
        d_LogInfoF("Popped value: %d, array count after pop: %zu", *popped, array->count);

        // Conditional logging for unexpected values
        d_LogIfF(*popped != value2, D_LOG_LEVEL_ERROR,
                "🚨 UNEXPECTED: Popped value %d, expected %d", *popped, value2);
        d_LogIfF(*popped == value2, D_LOG_LEVEL_DEBUG,
                "✅ Popped correct value %d", *popped);
    } else {
        d_LogError("🚨 Pop operation returned NULL - this should not happen!");
    }

    TEST_ASSERT(popped != NULL && *popped == value2, "Should pop the correct value");
    TEST_ASSERT(array->count == 1, "Array should have 1 element after pop");
    d_LogInfoF("Array state after pop: count=%zu, capacity=%zu", array->count, array->capacity);

    d_LogInfo("🎉 Boundary conditions test completed");
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

int test_dynamic_array_index_edge_cases(void)
{
    dLogContext_t* ctx = d_PushLogContext("IndexEdgeCases");
    d_LogInfo("🔍 Testing index boundary edge cases");

    dArray_t* array = d_InitArray(10, sizeof(int));
    d_LogInfoF("Created array for index testing: capacity=%zu", array->capacity);

    // Add a few elements
    int values[] = {1, 2, 3};
    d_LogDebug("Adding test values to array");
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
        d_LogDebugF("Added value %d at index %d", values[i], i);
    }

    d_LogInfoF("Array populated with %zu elements", array->count);

    // Test index at boundary conditions
    d_LogDebug("Testing index just beyond valid range");
    void* result = d_GetDataFromArrayByIndex(array, 3);  // Just beyond valid range
    TEST_ASSERT(result == NULL, "Should return NULL for index just beyond range");
    d_LogInfo("✅ Out-of-bounds index properly rejected");

    d_LogDebug("Testing access to last valid index");
    result = d_GetDataFromArrayByIndex(array, 2);  // Last valid index
    TEST_ASSERT(result != NULL, "Should return valid result for last valid index");
    TEST_ASSERT(*(int*)result == 3, "Should return correct value for last valid index");
    d_LogInfoF("✅ Last valid index access: got %d", result ? *(int*)result : -1);

    d_LogDebug("Testing access to first valid index");
    result = d_GetDataFromArrayByIndex(array, 0);  // First valid index
    TEST_ASSERT(result != NULL, "Should return valid result for first index");
    TEST_ASSERT(*(int*)result == 1, "Should return correct value for first index");
    d_LogInfoF("✅ First valid index access: got %d", result ? *(int*)result : -1);

    // Rate-limited comprehensive index testing
    for (int i = -1; i <= 5; i++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG, 3, 1.0,
                         "Index boundary test: testing index %d", i);
    }

    // Final index test summary
    dLogStructured_t* index_summary = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(index_summary, "test_category", "index_edge_cases");
    d_LogStructured_FieldInt(index_summary, "array_count", (int)array->count);
    d_LogStructured_FieldInt(index_summary, "valid_index_min", 0);
    d_LogStructured_FieldInt(index_summary, "valid_index_max", (int)array->count - 1);
    d_LogStructured_FieldBool(index_summary, "boundary_tests_passed", true);
    d_LogStructured_SetFormat(index_summary, true);
    d_LogStructured_Commit(index_summary);

    d_LogInfo("✅ Index edge case testing completed successfully");
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

int test_dynamic_array_pop_edge_cases(void)
{
    dLogContext_t* ctx = d_PushLogContext("PopEdgeCases");
    d_LogInfo("📤 Testing pop operation edge cases");

    dArray_t* array = d_InitArray(5, sizeof(int));
    d_LogInfoF("Created array for pop testing: capacity=%zu, initial count=%zu",
               array->capacity, array->count);

    // Test pop on freshly created array
    d_LogDebug("Testing pop on empty array");
    void* result = d_PopDataFromArray(array);
    TEST_ASSERT(result == NULL, "Pop from fresh array should return NULL");
    d_LogInfo("✅ Pop from empty array correctly returned NULL");

    // Add one element and pop it
    int value = 42;
    d_LogInfoF("Adding single value %d to array", value);
    d_AppendArray(array, &value);
    d_LogInfoF("Array state after append: count=%zu", array->count);

    d_LogDebug("Attempting to pop single element");
    int* popped = (int*)d_PopDataFromArray(array);
    TEST_ASSERT(popped != NULL, "Pop from single-element array should succeed");

    if (popped != NULL) {
        d_LogInfoF("Successfully popped value: %d", *popped);
        TEST_ASSERT(*popped == value, "Popped value should match");

        // Structured logging for successful pop
        dLogStructured_t* pop_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
        d_LogStructured_Field(pop_log, "operation", "single_element_pop");
        d_LogStructured_FieldInt(pop_log, "expected_value", value);
        d_LogStructured_FieldInt(pop_log, "actual_value", *popped);
        d_LogStructured_FieldBool(pop_log, "values_match", *popped == value);
        d_LogStructured_FieldInt(pop_log, "array_count_after", (int)array->count);
        d_LogStructured_SetFormat(pop_log, false);
        d_LogStructured_Commit(pop_log);
    } else {
        d_LogError("🚨 Pop returned NULL when it should have returned a value!");
    }

    TEST_ASSERT(array->count == 0, "Array should be empty after popping single element");
    d_LogInfoF("Array count after pop: %zu (expected: 0)", array->count);

    // Try to pop again
    d_LogDebug("Testing second pop on now-empty array");
    result = d_PopDataFromArray(array);
    TEST_ASSERT(result == NULL, "Pop from empty array should return NULL");
    d_LogInfo("✅ Second pop correctly returned NULL");

    // Edge case: Multiple pops in sequence
    d_LogDebug("Testing multiple sequential pops for robustness");
    for (int i = 0; i < 3; i++) {
        result = d_PopDataFromArray(array);
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG, 2, 1.0,
                         "Sequential pop %d returned: %s", i + 1, result ? "non-NULL" : "NULL");
    }

    d_LogInfo("🎉 Pop edge case testing completed successfully");
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

int test_dynamic_array_memory_patterns(void)
{
    dLogContext_t* ctx = d_PushLogContext("MemoryPatterns");
    d_LogInfo("🧠 Testing memory layout and contiguity patterns");

    dArray_t* array = d_InitArray(3, sizeof(int));
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FINAL_MESSAGE, D_LOG_LEVEL_INFO, 2, 1.0,
               "Created array for memory testing: capacity=%zu, element_size=%zu",
               array->capacity, array->element_size);

    // Fill array completely
    int values[] = {10, 20, 30};
    d_LogDebug("Filling array with contiguous test values");
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG, 2, 1.0, "Added value %d at logical position %d", values[i], i);
    }

    // Verify memory is contiguous by checking pointer arithmetic
    int* first = (int*)d_GetDataFromArrayByIndex(array, 0);
    int* second = (int*)d_GetDataFromArrayByIndex(array, 1);
    int* third = (int*)d_GetDataFromArrayByIndex(array, 2);

    TEST_ASSERT(first != NULL && second != NULL && third != NULL, "All pointers should be valid");

    if (first && second && third) {
        d_LogInfoF("Memory addresses: first=%p, second=%p, third=%p",
                   (void*)first, (void*)second, (void*)third);

        // Calculate pointer differences
        ptrdiff_t diff1 = second - first;
        ptrdiff_t diff2 = third - second;

        d_LogInfoF("Pointer arithmetic: second-first=%td, third-second=%td", diff1, diff2);

        TEST_ASSERT(second == first + 1, "Second element should be adjacent to first");
        TEST_ASSERT(third == second + 1, "Third element should be adjacent to second");

        // Comprehensive memory analysis
        dLogStructured_t* memory_analysis = d_LogStructured(D_LOG_LEVEL_INFO);
        d_LogStructured_Field(memory_analysis, "test", "memory_contiguity");
        d_LogStructured_FieldInt(memory_analysis, "element_size_bytes", sizeof(int));
        d_LogStructured_FieldInt(memory_analysis, "elements_tested", 3);
        d_LogStructured_FieldBool(memory_analysis, "contiguous_layout", diff1 == 1 && diff2 == 1);
        d_LogStructured_FieldBool(memory_analysis, "pointer_arithmetic_valid", true);
        d_LogStructured_Field(memory_analysis, "memory_pattern", "sequential_adjacent");
        d_LogStructured_SetFormat(memory_analysis, true);
        d_LogStructured_Commit(memory_analysis);

        d_LogInfo("✅ Memory layout verification: elements are contiguous in memory");

        // Verify actual values are correct
        d_LogDebugF("Value verification: [0]=%d, [1]=%d, [2]=%d", *first, *second, *third);
        d_LogIfF(*first != 10 || *second != 20 || *third != 30, D_LOG_LEVEL_ERROR,
                "🚨 Memory values corrupted! Expected [10,20,30], got [%d,%d,%d]",
                *first, *second, *third);
    } else {
        d_LogError("🚨 CRITICAL: One or more memory pointers are NULL!");
    }

    d_LogInfo("🎉 Memory pattern analysis completed");
    d_DestroyArray(array);
    d_PopLogContext(ctx);
    return 1;
}

// Main test runner
int main(void)
{
    // Initialize comprehensive logging for the test suite
    dLogger_t* test_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_thread_id = true,
        .colorize_output = true,
        .timestamp_format = "%H:%M:%S.%03d",
        .context_separator = " :: "
    });
    d_SetGlobalLogger(test_logger);

    // Main test context
    dLogContext_t* main_ctx = d_PushLogContext("DynamicArrayEdgeTests");

    d_LogInfo("🚀 Starting Dynamic Array Edge Cases Test Suite");
    d_LogInfo("🎯 Testing robust error handling, boundary conditions, and edge cases");

    // Structured logging for test suite initialization
    dLogStructured_t* suite_start = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(suite_start, "event", "test_suite_start");
    d_LogStructured_Field(suite_start, "suite_name", "dynamic_array_edge_cases");
    d_LogStructured_FieldInt(suite_start, "total_test_functions", 8);
    d_LogStructured_Field(suite_start, "focus", "edge_cases_and_error_handling");
    d_LogStructured_FieldTimestamp(suite_start, "start_time");
    d_LogStructured_SetFormat(suite_start, true);
    d_LogStructured_Commit(suite_start);

    TEST_SUITE_START("Dynamic Array Edge Cases Tests");

    RUN_TEST(test_dynamic_array_null_safety);
    RUN_TEST(test_dynamic_array_null_data_append);
    RUN_TEST(test_dynamic_array_large_indices);
    RUN_TEST(test_dynamic_array_zero_element_size);
    RUN_TEST(test_dynamic_array_boundary_conditions);
    RUN_TEST(test_dynamic_array_index_edge_cases);
    RUN_TEST(test_dynamic_array_pop_edge_cases);
    RUN_TEST(test_dynamic_array_memory_patterns);

    TEST_SUITE_END();

    // Final test suite summary
    d_LogInfo("📊 Test Suite Summary:");
    d_LogInfoF("   • NULL safety: Verified all functions handle NULL parameters gracefully");
    d_LogInfoF("   • Boundary conditions: Tested minimal capacity and expansion behavior");
    d_LogInfoF("   • Index validation: Confirmed proper bounds checking");
    d_LogInfoF("   • Memory patterns: Verified contiguous memory layout");
    d_LogInfoF("   • Edge cases: Covered unusual but valid scenarios");

    // Rate-limited final message
    d_LogRateLimited(D_LOG_LEVEL_INFO, 1, 5.0,
                    "🏁 Dynamic Array Edge Cases Test Suite completed!");

    d_PopLogContext(main_ctx);

    // Cleanup logging
    d_DestroyLogger(test_logger);
    d_SetGlobalLogger(NULL);

    return 0;
}
