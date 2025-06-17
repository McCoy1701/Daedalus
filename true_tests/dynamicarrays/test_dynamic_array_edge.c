// File: true_tests/dynamicarrays/test_dynamic_array_edge.c - Edge cases and error handling tests

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Test Functions
int test_dynamic_array_null_safety(void)
{
    // Test all functions with NULL array parameter (should not crash)
    int test_value = 42;

    d_AppendArray(NULL, &test_value);
    TEST_ASSERT(1, "AppendArray with NULL array should not crash");

    void* result = d_GetDataFromArrayByIndex(NULL, 0);
    TEST_ASSERT(result == NULL, "GetDataFromArrayByIndex with NULL array should return NULL");

    result = d_PopDataFromArray(NULL);
    TEST_ASSERT(result == NULL, "PopDataFromArray with NULL array should return NULL");

    int resize_result = d_ResizeArray(NULL, 10);
    TEST_ASSERT(resize_result != 0, "ResizeArray with NULL array should return error");

    // DestroyArray with NULL should not crash
    d_DestroyArray(NULL);
    TEST_ASSERT(1, "DestroyArray with NULL should not crash");

    return 1;
}

int test_dynamic_array_null_data_append(void)
{
    dArray_t* array = d_InitArray(5, sizeof(int));

    // Test appending NULL data (should handle gracefully)
    size_t old_count = array->count;
    d_AppendArray(array, NULL);

    // The behavior might vary - either it doesn't change count, or it copies garbage
    // We just test that it doesn't crash
    TEST_ASSERT(1, "Appending NULL data should not crash");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_large_indices(void)
{
    dArray_t* array = d_InitArray(5, sizeof(int));

    int value = 100;
    d_AppendArray(array, &value);

    // Test with very large indices
    void* result = d_GetDataFromArrayByIndex(array, SIZE_MAX);
    TEST_ASSERT(result == NULL, "Should return NULL for SIZE_MAX index");

    result = d_GetDataFromArrayByIndex(array, 1000000);
    TEST_ASSERT(result == NULL, "Should return NULL for very large index");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_zero_element_size(void)
{
    // Test initialization with zero element size
    dArray_t* array = d_InitArray(10, 0);

    if (array != NULL) {
        TEST_ASSERT(array->element_size == 0, "Element size should be 0");

        // Operations with zero element size should be handled
        char dummy = 'x';
        d_AppendArray(array, &dummy);
        TEST_ASSERT(1, "Append with zero element size should not crash");

        d_DestroyArray(array);
    }

    TEST_ASSERT(1, "Zero element size handling should not crash");
    return 1;
}

int test_dynamic_array_boundary_conditions(void)
{
    // Test with capacity of 1
    dArray_t* array = d_InitArray(1, sizeof(int));

    int value1 = 10, value2 = 20;

    // Fill to capacity
    d_AppendArray(array, &value1);
    TEST_ASSERT(array->count == 1, "Should have 1 element");

    int* retrieved = (int*)d_GetDataFromArrayByIndex(array, 0);
    TEST_ASSERT(retrieved != NULL && *retrieved == value1, "Should retrieve correct value");

    // Try to exceed capacity
    d_AppendArray(array, &value2);
    TEST_ASSERT(array->count == 1, "Count should not increase beyond capacity");

    // Pop the only element
    int* popped = (int*)d_PopDataFromArray(array);
    TEST_ASSERT(popped != NULL && *popped == value1, "Should pop the correct value");
    TEST_ASSERT(array->count == 0, "Array should be empty after pop");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_index_edge_cases(void)
{
    dArray_t* array = d_InitArray(10, sizeof(int));

    // Add a few elements
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
    }

    // Test index at boundary conditions
    void* result = d_GetDataFromArrayByIndex(array, 3);  // Just beyond valid range
    TEST_ASSERT(result == NULL, "Should return NULL for index just beyond range");

    result = d_GetDataFromArrayByIndex(array, 2);  // Last valid index
    TEST_ASSERT(result != NULL, "Should return valid result for last valid index");
    TEST_ASSERT(*(int*)result == 3, "Should return correct value for last valid index");

    result = d_GetDataFromArrayByIndex(array, 0);  // First valid index
    TEST_ASSERT(result != NULL, "Should return valid result for first index");
    TEST_ASSERT(*(int*)result == 1, "Should return correct value for first index");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_pop_edge_cases(void)
{
    dArray_t* array = d_InitArray(5, sizeof(int));

    // Test pop on freshly created array
    void* result = d_PopDataFromArray(array);
    TEST_ASSERT(result == NULL, "Pop from fresh array should return NULL");

    // Add one element and pop it
    int value = 42;
    d_AppendArray(array, &value);

    int* popped = (int*)d_PopDataFromArray(array);
    TEST_ASSERT(popped != NULL, "Pop from single-element array should succeed");
    TEST_ASSERT(*popped == value, "Popped value should match");
    TEST_ASSERT(array->count == 0, "Array should be empty after popping single element");

    // Try to pop again
    result = d_PopDataFromArray(array);
    TEST_ASSERT(result == NULL, "Pop from empty array should return NULL");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_memory_patterns(void)
{
    dArray_t* array = d_InitArray(3, sizeof(int));

    // Fill array completely
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
    }

    // Verify memory is contiguous by checking pointer arithmetic
    int* first = (int*)d_GetDataFromArrayByIndex(array, 0);
    int* second = (int*)d_GetDataFromArrayByIndex(array, 1);
    int* third = (int*)d_GetDataFromArrayByIndex(array, 2);

    TEST_ASSERT(first != NULL && second != NULL && third != NULL, "All pointers should be valid");
    TEST_ASSERT(second == first + 1, "Second element should be adjacent to first");
    TEST_ASSERT(third == second + 1, "Third element should be adjacent to second");

    d_DestroyArray(array);
    return 1;
}

// Main test runner
int main(void)
{
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
}
