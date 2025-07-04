// File: true_tests/staticarrays/test_static_array_basic.c - Divine Static Array Tests
// Enhanced with the full power of Daedalus Logging and Metis's Wisdom

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// HELPER FUNCTIONS WITH DIVINE LOGGING
// =============================================================================

dStaticArray_t* create_test_static_array(size_t capacity, size_t element_size)
{
    dStaticArray_t* arr = d_InitStaticArray(capacity, element_size);
    d_LogIfF(arr == NULL, D_LOG_LEVEL_ERROR, "Failed to create test static array");
    return arr;
}

// Helper for safe element comparison
static bool safe_element_compare(const void* actual, const void* expected, size_t size, const char* context)
{
    if (actual == NULL && expected == NULL) {
        d_LogDebugF("Element comparison [%s]: Both elements are NULL (valid)", context);
        return true;
    }
    if (actual == NULL || expected == NULL) {
        d_LogWarningF("Element comparison [%s]: One element is NULL (actual=%p, expected=%p)",
                      context, actual, expected);
        return false;
    }

    bool result = memcmp(actual, expected, size) == 0;
    d_LogIfF(!result, D_LOG_LEVEL_WARNING,
             "Element mismatch [%s]: Memory comparison failed", context);
    return result;
}

// =============================================================================
// BASIC FUNCTIONALITY TESTS
// =============================================================================

int test_static_array_create_destroy(void)
{
    d_LogInfo("VERIFICATION: Static array creation and destruction integrity.");
    dLogContext_t* ctx = d_PushLogContext("CreateDestroy");

    d_LogDebug("Creating new static array for integers...");
    dStaticArray_t* arr = d_InitStaticArray(10, sizeof(int));
    TEST_ASSERT(arr != NULL, "Should create static array successfully");

    if (arr) {
        d_LogDebugF("Static array created: capacity=%zu, count=%zu, element_size=%zu",
                    arr->capacity, arr->count, arr->element_size);
        TEST_ASSERT(arr->capacity == 10, "Capacity should be 10");
        TEST_ASSERT(arr->count == 0, "Count should start at 0");
        TEST_ASSERT(arr->element_size == sizeof(int), "Element size should match int size");
        TEST_ASSERT(arr->data != NULL, "Data buffer should be allocated");
    }

    d_LogDebug("Testing normal destroy operation...");
    int result = d_DestroyStaticArray(arr);
    TEST_ASSERT(result == 0, "Destroy should return success");

    d_LogDebug("Testing NULL safety for destroy operation...");
    result = d_DestroyStaticArray(NULL);
    TEST_ASSERT(result == 1, "Destroy with NULL should return failure");

    d_PopLogContext(ctx);
    return 1;
}

int test_static_array_append_basic(void)
{
    d_LogInfo("VERIFICATION: Basic element appending functionality.");
    dLogContext_t* ctx = d_PushLogContext("AppendBasic");

    dStaticArray_t* arr = create_test_static_array(5, sizeof(int));
    
    d_LogDebug("Appending single integer element...");
    int value1 = 42;
    int result = d_AppendDataToStaticArray(arr, &value1);
    TEST_ASSERT(result == 0, "Append should return success");
    TEST_ASSERT(arr->count == 1, "Count should be 1 after first append");

    d_LogDebug("Retrieving and verifying appended element...");
    int* retrieved = (int*)d_IndexDataFromStaticArray(arr, 0);
    TEST_ASSERT(retrieved != NULL, "Should retrieve valid pointer");
    TEST_ASSERT(*retrieved == 42, "Retrieved value should match appended value");

    d_LogDebug("Appending multiple elements...");
    int value2 = 100, value3 = -15;
    d_AppendDataToStaticArray(arr, &value2);
    d_AppendDataToStaticArray(arr, &value3);
    TEST_ASSERT(arr->count == 3, "Count should be 3 after three appends");

    // Verify all elements
    int* elem0 = (int*)d_IndexDataFromStaticArray(arr, 0);
    int* elem1 = (int*)d_IndexDataFromStaticArray(arr, 1);
    int* elem2 = (int*)d_IndexDataFromStaticArray(arr, 2);
    
    TEST_ASSERT(*elem0 == 42, "First element should be 42");
    TEST_ASSERT(*elem1 == 100, "Second element should be 100");
    TEST_ASSERT(*elem2 == -15, "Third element should be -15");

    d_DestroyStaticArray(arr);
    d_PopLogContext(ctx);
    return 1;
}

int test_static_array_capacity_limits(void)
{
    d_LogInfo("VERIFICATION: Static array capacity enforcement and overflow handling.");
    dLogContext_t* ctx = d_PushLogContext("CapacityLimits");

    dStaticArray_t* arr = create_test_static_array(3, sizeof(int));
    
    d_LogDebug("Filling array to capacity...");
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        int result = d_AppendDataToStaticArray(arr, &values[i]);
        TEST_ASSERT(result == 0, "Should successfully append within capacity");
    }
    TEST_ASSERT(arr->count == 3, "Array should be at capacity");

    d_LogDebug("Testing overflow attempt...");
    int overflow_value = 40;
    int result = d_AppendDataToStaticArray(arr, &overflow_value);
    TEST_ASSERT(result == 1, "Append beyond capacity should fail");
    TEST_ASSERT(arr->count == 3, "Count should remain at capacity");

    d_LogDebug("Verifying existing elements are unchanged...");
    for (int i = 0; i < 3; i++) {
        int* elem = (int*)d_IndexDataFromStaticArray(arr, i);
        TEST_ASSERT(*elem == values[i], "Existing elements should be preserved");
    }

    d_DestroyStaticArray(arr);
    d_PopLogContext(ctx);
    return 1;
}

int test_static_array_indexing(void)
{
    d_LogInfo("VERIFICATION: Array indexing and bounds checking.");
    dLogContext_t* ctx = d_PushLogContext("Indexing");

    dStaticArray_t* arr = create_test_static_array(5, sizeof(float));
    
    d_LogDebug("Populating array with float values...");
    float values[] = {1.1f, 2.2f, 3.3f};
    for (int i = 0; i < 3; i++) {
        d_AppendDataToStaticArray(arr, &values[i]);
    }

    d_LogDebug("Testing valid indexing...");
    for (int i = 0; i < 3; i++) {
        float* elem = (float*)d_IndexDataFromStaticArray(arr, i);
        TEST_ASSERT(elem != NULL, "Valid index should return valid pointer");
        TEST_ASSERT(fabs(*elem - values[i]) < 0.001f, "Value should match expected");
    }

    d_LogDebug("Testing out-of-bounds indexing...");
    float* out_of_bounds = (float*)d_IndexDataFromStaticArray(arr, 3);
    TEST_ASSERT(out_of_bounds == NULL, "Index beyond count should return NULL");

    out_of_bounds = (float*)d_IndexDataFromStaticArray(arr, 10);
    TEST_ASSERT(out_of_bounds == NULL, "Index beyond capacity should return NULL");

    d_LogDebug("Testing indexing with NULL array...");
    float* null_result = (float*)d_IndexDataFromStaticArray(NULL, 0);
    TEST_ASSERT(null_result == NULL, "NULL array should return NULL");

    d_DestroyStaticArray(arr);
    d_PopLogContext(ctx);
    return 1;
}

int test_static_array_pop_lifo(void)
{
    d_LogInfo("VERIFICATION: LIFO (Last In, First Out) pop behavior.");
    dLogContext_t* ctx = d_PushLogContext("PopLIFO");

    dStaticArray_t* arr = create_test_static_array(4, sizeof(char));
    
    d_LogDebug("Building stack with characters...");
    char stack_chars[] = {'A', 'B', 'C'};
    
    LOOP_TEST_START();
    for (int i = 0; i < 3; i++) {
        d_AppendDataToStaticArray(arr, &stack_chars[i]);
        d_LogDebugF("Added character '%c' to position %d", stack_chars[i], i);
    }
    LOOP_TEST_END();
    
    TEST_ASSERT(arr->count == 3, "Array should have 3 elements");

    d_LogDebug("Testing LIFO pop behavior...");
    // Pop 'C' (last added)
    char* popped = (char*)d_PopDataFromStaticArray(arr);
    TEST_ASSERT(popped != NULL, "Pop should return valid pointer");
    TEST_ASSERT(*popped == 'C', "Should pop last added element 'C'");
    TEST_ASSERT(arr->count == 2, "Count should decrease after pop");

    // Pop 'B'
    popped = (char*)d_PopDataFromStaticArray(arr);
    TEST_ASSERT(*popped == 'B', "Should pop second-to-last element 'B'");
    TEST_ASSERT(arr->count == 1, "Count should be 1 after second pop");

    // Pop 'A'
    popped = (char*)d_PopDataFromStaticArray(arr);
    TEST_ASSERT(*popped == 'A', "Should pop first added element 'A'");
    TEST_ASSERT(arr->count == 0, "Array should be empty after all pops");

    d_LogDebug("Testing pop from empty array...");
    LOOP_TEST_START();
    for (int i = 0; i < 5; i++) {
        popped = (char*)d_PopDataFromStaticArray(arr);
        TEST_ASSERT(popped == NULL, "Pop from empty array should return NULL");
        TEST_ASSERT(arr->count == 0, "Count should remain 0");
        d_LogDebugF("Empty pop attempt %d: correctly returned NULL", i + 1);
    }
    LOOP_TEST_END();

    d_DestroyStaticArray(arr);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// EDGE CASES AND NULL SAFETY TESTS
// =============================================================================

int test_static_array_null_safety(void)
{
    d_LogInfo("VERIFICATION: NULL pointer safety across all static array functions.");
    dLogContext_t* ctx = d_PushLogContext("NullSafety");

    d_LogDebug("Testing all functions with NULL array parameter...");
    int dummy_value = 42;
    
    // These should not crash and return appropriate error codes
    TEST_ASSERT(d_DestroyStaticArray(NULL) == 1, "Destroy NULL should return error");
    TEST_ASSERT(d_AppendDataToStaticArray(NULL, &dummy_value) == 1, "Append to NULL should return error");
    TEST_ASSERT(d_IndexDataFromStaticArray(NULL, 0) == NULL, "Index NULL should return NULL");
    TEST_ASSERT(d_PopDataFromStaticArray(NULL) == NULL, "Pop NULL should return NULL");

    d_LogDebug("Testing functions with NULL data parameter...");
    dStaticArray_t* arr = create_test_static_array(5, sizeof(int));
    
    TEST_ASSERT(d_AppendDataToStaticArray(arr, NULL) == 1, "Append NULL data should return error");
    TEST_ASSERT(arr->count == 0, "Count should remain 0 after failed append");

    d_DestroyStaticArray(arr);
    d_PopLogContext(ctx);
    return 1;
}

int test_static_array_zero_capacity(void)
{
    d_LogWarning("BUG HUNT: Testing static array creation with invalid parameters.");
    dLogContext_t* ctx = d_PushLogContext("ZeroCapacity");

    d_LogDebug("Testing zero capacity...");
    dStaticArray_t* arr = d_InitStaticArray(0, sizeof(int));
    TEST_ASSERT(arr == NULL, "Zero capacity should return NULL");

    d_LogDebug("Testing zero element size...");
    arr = d_InitStaticArray(10, 0);
    TEST_ASSERT(arr == NULL, "Zero element size should return NULL");

    d_LogDebug("Testing both zero...");
    arr = d_InitStaticArray(0, 0);
    TEST_ASSERT(arr == NULL, "Both zero should return NULL");

    d_PopLogContext(ctx);
    return 1;
}

int test_static_array_large_elements(void)
{
    d_LogInfo("VERIFICATION: Static array handling of large element sizes.");
    dLogContext_t* ctx = d_PushLogContext("LargeElements");

    // Test with a large structure
    typedef struct {
        char name[64];
        int values[16];
        double data[8];
    } large_element_t;

    dStaticArray_t* arr = create_test_static_array(3, sizeof(large_element_t));
    TEST_ASSERT(arr != NULL, "Should create array for large elements");

    d_LogDebug("Creating and appending large element...");
    large_element_t elem1;
    strcpy(elem1.name, "TestElement1");
    for (int i = 0; i < 16; i++) elem1.values[i] = i * i;
    for (int i = 0; i < 8; i++) elem1.data[i] = i * 3.14;

    int result = d_AppendDataToStaticArray(arr, &elem1);
    TEST_ASSERT(result == 0, "Should append large element successfully");

    d_LogDebug("Retrieving and verifying large element...");
    large_element_t* retrieved = (large_element_t*)d_IndexDataFromStaticArray(arr, 0);
    TEST_ASSERT(retrieved != NULL, "Should retrieve large element");
    TEST_ASSERT(strcmp(retrieved->name, "TestElement1") == 0, "Name should match");
    TEST_ASSERT(retrieved->values[5] == 25, "Values should match");
    TEST_ASSERT(fabs(retrieved->data[2] - 6.28) < 0.01, "Data should match");

    d_DestroyStaticArray(arr);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// STRESS AND PERFORMANCE TESTS
// =============================================================================

int test_static_array_stress_cycling(void)
{
    d_LogWarning("BUG HUNT: Stress testing with rapid append/pop cycles.");
    dLogContext_t* ctx = d_PushLogContext("StressCycling");

    dStaticArray_t* arr = create_test_static_array(100, sizeof(int));
    
    d_LogDebug("Testing rapid fill and empty cycles...");
    LOOP_TEST_START();
    for (int cycle = 0; cycle < 10; cycle++) {
        // Fill to capacity
        for (int i = 0; i < 100; i++) {
            int value = cycle * 100 + i;
            int result = d_AppendDataToStaticArray(arr, &value);
            TEST_ASSERT(result == 0, "Append should succeed during fill");
        }
        TEST_ASSERT(arr->count == 100, "Array should be full after fill");

        // Empty completely
        for (int i = 0; i < 100; i++) {
            int* popped = (int*)d_PopDataFromStaticArray(arr);
            TEST_ASSERT(popped != NULL, "Pop should succeed during empty");
            // Verify LIFO order
            int expected = cycle * 100 + (99 - i);
            TEST_ASSERT(*popped == expected, "Popped value should match LIFO order");
        }
        TEST_ASSERT(arr->count == 0, "Array should be empty after complete pop");

        // Rate limited progress reporting
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 2.0, "Stress cycle %d completed", cycle + 1);
    }
    LOOP_TEST_END();

    d_DestroyStaticArray(arr);
    d_PopLogContext(ctx);
    return 1;
}

int test_static_array_memory_patterns(void)
{
    d_LogWarning("BUG HUNT: Memory allocation patterns and leak detection.");
    dLogContext_t* ctx = d_PushLogContext("MemoryPatterns");

    d_LogDebug("Creating and destroying multiple arrays rapidly...");
    LOOP_TEST_START();
    for (int i = 0; i < 50; i++) {
        dStaticArray_t* temp_arr = d_InitStaticArray(10 + i, sizeof(double));
        TEST_ASSERT(temp_arr != NULL, "Array creation should not fail");

        // Use the array
        for (int j = 0; j < 5; j++) {
            double value = i * 10.0 + j;
            d_AppendDataToStaticArray(temp_arr, &value);
        }

        TEST_ASSERT(temp_arr->count == 5, "Array should have expected count");
        
        int result = d_DestroyStaticArray(temp_arr);
        TEST_ASSERT(result == 0, "Destroy should succeed");

        // Rate limited progress reporting
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 3.0, "Memory pattern test %d completed", i + 1);
    }
    LOOP_TEST_END();

    d_LogDebug("Testing concurrent array operations...");
    dStaticArray_t* arrays[5];
    LOOP_TEST_START();
    for (int i = 0; i < 5; i++) {
        arrays[i] = create_test_static_array(20, sizeof(int));
        TEST_ASSERT(arrays[i] != NULL, "Concurrent array creation should succeed");
    }
    LOOP_TEST_END();

    // Use all arrays
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 15; j++) {
            int value = i * 100 + j;
            d_AppendDataToStaticArray(arrays[i], &value);
        }
    }

    // Verify and cleanup
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(arrays[i]->count == 15, "Each array should have 15 elements");
        d_DestroyStaticArray(arrays[i]);
    }

    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// REAL-WORLD USAGE SIMULATION TESTS
// =============================================================================

int test_static_array_game_inventory(void)
{
    d_LogInfo("VERIFICATION: Real-world game inventory system simulation.");
    dLogContext_t* ctx = d_PushLogContext("GameInventory");

    // Simulate a game inventory with limited slots
    typedef struct {
        int item_id;
        char name[32];
        int quantity;
        float weight;
    } inventory_item_t;

    dStaticArray_t* inventory = create_test_static_array(10, sizeof(inventory_item_t));
    
    d_LogDebug("Adding items to inventory...");
    inventory_item_t items[] = {
        {1, "Health Potion", 5, 0.5f},
        {2, "Iron Sword", 1, 3.2f},
        {3, "Magic Ring", 1, 0.1f},
        {4, "Bread", 10, 0.2f}
    };

    for (int i = 0; i < 4; i++) {
        int result = d_AppendDataToStaticArray(inventory, &items[i]);
        TEST_ASSERT(result == 0, "Should add item to inventory");
    }

    d_LogDebug("Searching inventory for specific item...");
    bool found_sword = false;
    LOOP_TEST_START();
    for (size_t i = 0; i < inventory->count; i++) {
        inventory_item_t* item = (inventory_item_t*)d_IndexDataFromStaticArray(inventory, i);
        if (item && strcmp(item->name, "Iron Sword") == 0) {
            found_sword = true;
            TEST_ASSERT(item->item_id == 2, "Sword should have correct ID");
            TEST_ASSERT(item->quantity == 1, "Sword quantity should be 1");
            break;
        }
    }
    LOOP_TEST_END();
    TEST_ASSERT(found_sword, "Should find Iron Sword in inventory");

    d_LogDebug("Using (removing) last added item...");
    inventory_item_t* used_item = (inventory_item_t*)d_PopDataFromStaticArray(inventory);
    TEST_ASSERT(used_item != NULL, "Should get last item");
    TEST_ASSERT(strcmp(used_item->name, "Bread") == 0, "Last item should be Bread");
    TEST_ASSERT(inventory->count == 3, "Inventory should have 3 items after use");

    d_LogDebug("Calculating total inventory weight...");
    float total_weight = 0.0f;
    for (size_t i = 0; i < inventory->count; i++) {
        inventory_item_t* item = (inventory_item_t*)d_IndexDataFromStaticArray(inventory, i);
        total_weight += item->weight * item->quantity;
    }
    d_LogDebugF("Total inventory weight: %.2f", total_weight);
    TEST_ASSERT(total_weight > 0.0f, "Inventory should have weight");

    d_DestroyStaticArray(inventory);
    d_PopLogContext(ctx);
    return 1;
}

int test_static_array_command_buffer(void)
{
    d_LogInfo("VERIFICATION: Command buffer system with fixed-size queue.");
    dLogContext_t* ctx = d_PushLogContext("CommandBuffer");

    typedef struct {
        int command_id;
        char command[16];
        float timestamp;
    } game_command_t;

    dStaticArray_t* cmd_buffer = create_test_static_array(5, sizeof(game_command_t));
    
    d_LogDebug("Filling command buffer...");
    const char* commands[] = {"MOVE_UP", "ATTACK", "USE_ITEM", "CAST_SPELL", "DEFEND"};
    
    for (int i = 0; i < 5; i++) {
        game_command_t cmd;
        cmd.command_id = 100 + i;
        strcpy(cmd.command, commands[i]);
        cmd.timestamp = i * 0.1f;
        
        int result = d_AppendDataToStaticArray(cmd_buffer, &cmd);
        TEST_ASSERT(result == 0, "Should add command to buffer");
    }

    d_LogDebug("Testing buffer overflow behavior...");
    game_command_t overflow_cmd = {200, "OVERFLOW", 1.0f};
    int result = d_AppendDataToStaticArray(cmd_buffer, &overflow_cmd);
    TEST_ASSERT(result == 1, "Buffer overflow should be rejected");
    TEST_ASSERT(cmd_buffer->count == 5, "Buffer should remain at capacity");

    d_LogDebug("Processing commands in LIFO order...");
    for (int i = 4; i >= 0; i--) {
        game_command_t* cmd = (game_command_t*)d_PopDataFromStaticArray(cmd_buffer);
        TEST_ASSERT(cmd != NULL, "Should get command from buffer");
        TEST_ASSERT(strcmp(cmd->command, commands[i]) == 0, "Command should match LIFO order");
        TEST_ASSERT(cmd->command_id == 100 + i, "Command ID should match");
    }

    TEST_ASSERT(cmd_buffer->count == 0, "Buffer should be empty after processing");

    d_DestroyStaticArray(cmd_buffer);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER WITH COMPREHENSIVE LOGGING SETUP
// =============================================================================

int main(void)
{
    // =========================================================================
    // DAEDALUS LOGGER INITIALIZATION - DIVINE CONFIGURATION
    // =========================================================================
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .colorize_output = true,
        .include_timestamp = false,
        .include_file_info = true,   // Enable for debugging
        .include_function = true     // Enable for detailed context
    };

    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);

    d_LogInfo("Initializing MIDAS-Enhanced Static Array Test Suite");
    d_LogDebugF("Daedalus Logging System: %s", "ACTIVE");
    // =========================================================================

    TEST_SUITE_START("MIDAS Enhanced Static Array Tests");

    // Core functionality tests
    RUN_TEST(test_static_array_create_destroy);
    RUN_TEST(test_static_array_append_basic);
    RUN_TEST(test_static_array_capacity_limits);
    RUN_TEST(test_static_array_indexing);
    RUN_TEST(test_static_array_pop_lifo);

    // Edge cases and safety tests
    RUN_TEST(test_static_array_null_safety);
    RUN_TEST(test_static_array_zero_capacity);
    RUN_TEST(test_static_array_large_elements);

    // Stress and performance tests
    RUN_TEST(test_static_array_stress_cycling);
    RUN_TEST(test_static_array_memory_patterns);

    // Real-world simulation tests
    RUN_TEST(test_static_array_game_inventory);
    RUN_TEST(test_static_array_command_buffer);

    // =========================================================================
    // DAEDALUS LOGGER CLEANUP
    // =========================================================================
    d_LogInfo("Static Array Test Suite completed");
    d_DestroyLogger(d_GetGlobalLogger());
    // =========================================================================

    TEST_SUITE_END();
}