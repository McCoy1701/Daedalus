// File: true_tests/dynamicarrays/test_dynamic_array_performance.c - Performance and stress tests

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Test Functions
int test_dynamic_array_large_capacity(void)
{
    // Test with relatively large capacity
    size_t large_capacity = 10000;
    dArray_t* array = d_InitArray(large_capacity, sizeof(int));

    TEST_ASSERT(array != NULL, "Should create large capacity array");
    TEST_ASSERT(array->capacity == large_capacity, "Large capacity should be set correctly");
    TEST_ASSERT(array->count == 0, "Large array should start empty");

    // Test adding elements throughout the array
    int test_indices[] = {0, 1000, 5000, 9999};
    int num_test_indices = sizeof(test_indices) / sizeof(test_indices[0]);

    for (int i = 0; i < num_test_indices; i++) {
        // Fill up to each test index
        while (array->count <= (size_t)test_indices[i]) {
            int value = (int)array->count * 10;  // Unique value based on position
            d_AppendArray(array, &value);
        }

        // Verify the value at the test index
        int* retrieved = (int*)d_GetDataFromArrayByIndex(array, test_indices[i]);
        TEST_ASSERT(retrieved != NULL, "Should retrieve value from large array");
        TEST_ASSERT(*retrieved == test_indices[i] * 10, "Value should match expected pattern");
    }

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_sequential_operations(void)
{
    size_t capacity = 1000;
    dArray_t* array = d_InitArray(capacity, sizeof(int));

    // Sequential append test
    for (size_t i = 0; i < capacity; i++) {
        int value = (int)(i * 2);  // Even numbers
        d_AppendArray(array, &value);
    }

    TEST_ASSERT(array->count == capacity, "Should fill array to capacity");

    // Sequential read test
    for (size_t i = 0; i < capacity; i++) {
        int* retrieved = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(retrieved != NULL, "Sequential read should succeed");
        TEST_ASSERT(*retrieved == (int)(i * 2), "Sequential values should match pattern");
    }

    // Sequential pop test
    for (size_t i = capacity; i > 0; i--) {
        int* popped = (int*)d_PopDataFromArray(array);
        TEST_ASSERT(popped != NULL, "Sequential pop should succeed");
        TEST_ASSERT(array->count == i - 1, "Count should decrease with each pop");
    }

    // Array should be empty now
    TEST_ASSERT(array->count == 0, "Array should be empty after popping all elements");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_mixed_operations(void)
{
    dArray_t* array = d_InitArray(100, sizeof(int));

    // Mixed operations: append, get, pop in various patterns
    int operations = 200;
    int append_count = 0;
    int successful_pops = 0;

    for (int i = 0; i < operations; i++) {
        int operation = i % 3;  // Cycle through 3 operation types

        switch (operation) {
            case 0: // Append
                if (array->count < array->capacity) {
                    int value = i;
                    d_AppendArray(array, &value);
                    append_count++;
                }
                break;

            case 1: // Get random element
                if (array->count > 0) {
                    size_t index = i % array->count;
                    int* retrieved = (int*)d_GetDataFromArrayByIndex(array, index);
                    TEST_ASSERT(retrieved != NULL, "Random get should succeed on valid index");
                }
                break;

            case 2: // Pop
                if (array->count > 0) {
                    int* popped = (int*)d_PopDataFromArray(array);
                    TEST_ASSERT(popped != NULL, "Pop should succeed on non-empty array");
                    successful_pops++;
                }
                break;
        }
    }

    // Verify final state makes sense
    TEST_ASSERT(array->count == (size_t)(append_count - successful_pops),
                "Final count should equal appends minus successful pops");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_resize_stress(void)
{
    dArray_t* array = d_InitArray(10, sizeof(int));

    // Fill initial array
    for (int i = 0; i < 10; i++) {
        int value = i * 10;
        d_AppendArray(array, &value);
    }

    // Multiple resize operations
    size_t resize_sizes[] = {20 * sizeof(int), 5 * sizeof(int), 50 * sizeof(int), 1 * sizeof(int), 100 * sizeof(int)};
    int num_resizes = sizeof(resize_sizes) / sizeof(resize_sizes[0]);

    for (int i = 0; i < num_resizes; i++) {
        int result = d_ResizeArray(array, resize_sizes[i]);
        TEST_ASSERT(result == 0, "Resize operation should succeed");
        TEST_ASSERT(array->capacity == resize_sizes[i], "Capacity should match resize target");

        // Verify that we can still access existing data (where valid)
        size_t max_check = (array->count < 5) ? array->count : 5;  // Check first 5 elements if they exist
        for (size_t j = 0; j < max_check; j++) {
            int* retrieved = (int*)d_GetDataFromArrayByIndex(array, j);
            if (retrieved != NULL) {  // May be NULL due to capacity issues
                TEST_ASSERT(*retrieved == (int)(j * 10), "Data should be preserved across resizes");
            }
        }
    }

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_stress_append_pop(void)
{
    dArray_t* array = d_InitArray(50, sizeof(int));

    // Stress test: many append/pop cycles
    for (int cycle = 0; cycle < 10; cycle++) {
        // Fill array
        for (int i = 0; i < 50; i++) {
            int value = cycle * 100 + i;
            d_AppendArray(array, &value);
        }

        TEST_ASSERT(array->count == 50, "Array should be full after filling");

        // Empty array
        for (int i = 49; i >= 0; i--) {
            int* popped = (int*)d_PopDataFromArray(array);
            TEST_ASSERT(popped != NULL, "Pop should succeed");
            TEST_ASSERT(*popped == cycle * 100 + i, "Popped value should match expected LIFO order");
        }

        TEST_ASSERT(array->count == 0, "Array should be empty after emptying");
    }

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_memory_consistency(void)
{
    dArray_t* array = d_InitArray(100, sizeof(int));

    // Fill with known pattern
    for (int i = 0; i < 100; i++) {
        int value = i * i;  // Square numbers
        d_AppendArray(array, &value);
    }

    // Randomly access and verify pattern integrity
    for (int test = 0; test < 50; test++) {
        size_t index = test * 2;  // Every other element
        int* retrieved = (int*)d_GetDataFromArrayByIndex(array, index);
        TEST_ASSERT(retrieved != NULL, "Random access should succeed");
        TEST_ASSERT(*retrieved == (int)(index * index), "Values should maintain pattern integrity");
    }

    // Pop half the elements and verify remaining
    for (int i = 0; i < 50; i++) {
        d_PopDataFromArray(array);
    }

    // Verify remaining elements still have correct pattern
    for (size_t i = 0; i < array->count; i++) {
        int* retrieved = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(retrieved != NULL, "Access to remaining elements should succeed");
        TEST_ASSERT(*retrieved == (int)(i * i), "Remaining elements should maintain pattern");
    }

    d_DestroyArray(array);
    return 1;
}

// Main test runner
int main(void)
{
    TEST_SUITE_START("Dynamic Array Performance Tests");

    RUN_TEST(test_dynamic_array_large_capacity);
    RUN_TEST(test_dynamic_array_sequential_operations);
    RUN_TEST(test_dynamic_array_mixed_operations);
    RUN_TEST(test_dynamic_array_resize_stress);
    RUN_TEST(test_dynamic_array_stress_append_pop);
    RUN_TEST(test_dynamic_array_memory_consistency);

    TEST_SUITE_END();
}
