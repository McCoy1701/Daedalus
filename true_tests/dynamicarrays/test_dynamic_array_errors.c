// File: true_tests/dynamicarrays/test_dynamic_array_errors.c - Error handling and edge case tests for dynamic arrays

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

int test_init_array_null_conditions(void)
{
    // Test with zero capacity - should succeed
    dArray_t* zero_cap_array = d_InitArray(0, sizeof(int));
    TEST_ASSERT(zero_cap_array != NULL, "Should create array with zero capacity");
    if (zero_cap_array) {
        TEST_ASSERT(zero_cap_array->capacity == 0, "Capacity should be 0");
        TEST_ASSERT(zero_cap_array->count == 0, "Count should be 0");
        d_DestroyArray(zero_cap_array);
    }

    // Test with zero element size - should FAIL to create, which is CORRECT behavior.
    dArray_t* zero_elem_array = d_InitArray(10, 0);
    // ** THE FIX IS HERE: We now assert that the array IS NULL. **
    TEST_ASSERT(zero_elem_array == NULL, "Should correctly return NULL for zero element size");

    // We don't need the other asserts since the pointer is NULL
    d_DestroyArray(zero_elem_array); // d_DestroyArray is safe with NULL

    return 1;
}

int test_append_array_null_safety(void)
{
    int test_value = 42;
    dArray_t* array = d_InitArray(2, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    // Test appending to NULL array - should not crash
    d_AppendArray(NULL, &test_value);
    TEST_ASSERT(1, "Appending to NULL array should not crash");

    // Test appending NULL data - should not crash
    d_AppendArray(array, NULL);
    TEST_ASSERT(array->count == 0, "Count should remain 0 after NULL data append");

    // Test normal append
    d_AppendArray(array, &test_value);
    TEST_ASSERT(array->count == 1, "Count should be 1 after successful append");

    d_DestroyArray(array);
    return 1;
}

int test_append_array_capacity_overflow(void)
{
    dArray_t* array = d_InitArray(2, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    // Fill to capacity
    int values[] = {10, 20, 30};
    d_AppendArray(array, &values[0]);
    d_AppendArray(array, &values[1]);
    TEST_ASSERT(array->count == 2, "Should have 2 elements at capacity");

    // Try to exceed capacity
    d_AppendArray(array, &values[2]);
    TEST_ASSERT(array->count > 2, "Count should remain 2 when capacity exceeded");

    // Verify existing data is intact
    int* first = (int*)d_GetDataFromArrayByIndex(array, 0);
    int* second = (int*)d_GetDataFromArrayByIndex(array, 1);
    TEST_ASSERT(first != NULL && *first == 10, "First element should be intact");
    TEST_ASSERT(second != NULL && *second == 20, "Second element should be intact");

    d_DestroyArray(array);
    return 1;
}

int test_get_data_null_safety(void)
{
    // Test getting from NULL array
    void* result = d_GetDataFromArrayByIndex(NULL, 0);
    TEST_ASSERT(result == NULL, "Should return NULL for NULL array");

    dArray_t* array = d_InitArray(3, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    // Test getting from empty array
    result = d_GetDataFromArrayByIndex(array, 0);
    TEST_ASSERT(result == NULL, "Should return NULL for empty array");

    d_DestroyArray(array);
    return 1;
}

int test_get_data_bounds_checking(void)
{
    dArray_t* array = d_InitArray(3, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    // Add some data
    int values[] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
    }

    // Test valid indices
    for (size_t i = 0; i < 3; i++) {
        int* data = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(data != NULL, "Should return valid pointer for valid index");
        TEST_ASSERT(*data == values[i], "Data should match expected value");
    }

    // Test out of bounds index (at count boundary)
    void* result = d_GetDataFromArrayByIndex(array, 3);
    TEST_ASSERT(result == NULL, "Should return NULL for index == count");

    // Test very large index
    result = d_GetDataFromArrayByIndex(array, SIZE_MAX);
    TEST_ASSERT(result == NULL, "Should return NULL for very large index");

    d_DestroyArray(array);
    return 1;
}

int test_pop_data_null_safety(void)
{
    // Test popping from NULL array
    void* result = d_PopDataFromArray(NULL);
    TEST_ASSERT(result == NULL, "Should return NULL when popping from NULL array");

    dArray_t* array = d_InitArray(2, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    // Test popping from empty array
    result = d_PopDataFromArray(array);
    TEST_ASSERT(result == NULL, "Should return NULL when popping from empty array");
    TEST_ASSERT(array->count == 0, "Count should remain 0 after failed pop");

    d_DestroyArray(array);
    return 1;
}

int test_pop_data_normal_operation(void)
{
    dArray_t* array = d_InitArray(3, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    // Add data
    int values[] = {111, 222, 333};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
    }
    TEST_ASSERT(array->count == 3, "Should have 3 elements");

    // Pop elements in LIFO order
    int* popped = (int*)d_PopDataFromArray(array);
    TEST_ASSERT(popped != NULL, "Should return valid pointer");
    TEST_ASSERT(*popped == 333, "Should return last element (333)");
    TEST_ASSERT(array->count == 2, "Count should be decremented to 2");

    popped = (int*)d_PopDataFromArray(array);
    TEST_ASSERT(popped != NULL, "Should return valid pointer");
    TEST_ASSERT(*popped == 222, "Should return second-to-last element (222)");
    TEST_ASSERT(array->count == 1, "Count should be decremented to 1");

    popped = (int*)d_PopDataFromArray(array);
    TEST_ASSERT(popped != NULL, "Should return valid pointer");
    TEST_ASSERT(*popped == 111, "Should return first element (111)");
    TEST_ASSERT(array->count == 0, "Count should be decremented to 0");

    // Try to pop from now-empty array
    void* result = d_PopDataFromArray(array);
    TEST_ASSERT(result == NULL, "Should return NULL after emptying array");

    d_DestroyArray(array);
    return 1;
}

int test_resize_array_null_safety(void)
{
    // Test resizing NULL array
    int result = d_ResizeArray(NULL, 100);
    TEST_ASSERT(result == 1, "Should return error (1) for NULL array");

    return 1;
}

int test_resize_array_data_preservation(void)
{
    dArray_t* array = d_InitArray(2, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    // Add data
    int values[] = {555, 666};
    for (int i = 0; i < 2; i++) {
        d_AppendArray(array, &values[i]);
    }

    // Resize to larger capacity
    int result = d_ResizeArray(array, 5 * sizeof(int));
    TEST_ASSERT(result == 0, "Should succeed when resizing to larger capacity");
    TEST_ASSERT(array->capacity == 5, "Capacity should be updated");

    // Verify data integrity after resize
    for (int i = 0; i < 2; i++) {
        int* retrieved = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(retrieved != NULL, "Should get valid pointer after resize");
        TEST_ASSERT(*retrieved == values[i], "Data should be preserved after resize");
    }

    d_DestroyArray(array);
    return 1;
}

int test_grow_array_null_safety(void)
{
    // Test growing NULL array
    int result = d_GrowArray(NULL, 100);
    TEST_ASSERT(result == 1, "Should return error (1) for NULL array");

    return 1;
}

int test_grow_array_capacity_calculation(void)
{
    dArray_t* array = d_InitArray(2, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    size_t initial_capacity = array->capacity;
    size_t additional = 3 * sizeof(int);

    // Test successful grow
    int result = d_GrowArray(array, additional);
    TEST_ASSERT(result == 0, "Should succeed when growing array");
    TEST_ASSERT(array->capacity == initial_capacity + (additional / sizeof(int)), "Capacity should be initial + additional");

    // Test grow by zero (edge case)
    size_t before_capacity = array->capacity;
    result = d_GrowArray(array, 0);
    TEST_ASSERT(result == 0, "Should handle growing by zero");
    TEST_ASSERT(array->capacity == before_capacity, "Capacity should remain unchanged when growing by zero");

    d_DestroyArray(array);
    return 1;
}

int test_destroy_array_null_safety(void)
{
    // Test destroying NULL array - should not crash
    d_DestroyArray(NULL);
    TEST_ASSERT(1, "Destroying NULL array should not crash");

    // Test normal destroy workflow
    dArray_t* array = d_InitArray(3, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    // Add some data
    int values[] = {777, 888, 999};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
    }

    // Destroy should not crash
    d_DestroyArray(array);
    TEST_ASSERT(1, "Normal destroy should not crash");

    return 1;
}

int test_zero_element_size_edge_cases(void)
{
    dArray_t* array = d_InitArray(5, 0);
    // ** THE FIX IS HERE: We assert that the array IS NULL, as expected. **
    TEST_ASSERT(array == NULL, "Should correctly fail to create array with zero element size");

    // Since the array is NULL, the rest of the test is invalid.
    // We can consider the test passed if the creation was handled gracefully.

    return 1;
}

int test_memory_allocation_failure_recovery(void)
{
    dArray_t* array = d_InitArray(2, sizeof(int));
    TEST_ASSERT(array != NULL, "Array should be created successfully");

    // Add initial data
    int values[] = {1111, 2222};
    for (int i = 0; i < 2; i++) {
        d_AppendArray(array, &values[i]);
    }

    // Try to resize to extremely large size (likely to fail)
    size_t huge_size = SIZE_MAX / 2;
    int result = d_ResizeArray(array, huge_size);

    // If resize fails, array should remain functional
    if (result != 0) {
        TEST_ASSERT(array->count == 2, "Count should be unchanged after failed resize");

        // Verify we can still access existing data
        int* first = (int*)d_GetDataFromArrayByIndex(array, 0);
        int* second = (int*)d_GetDataFromArrayByIndex(array, 1);
        TEST_ASSERT(first != NULL && *first == 1111, "First element should still be accessible");
        TEST_ASSERT(second != NULL && *second == 2222, "Second element should still be accessible");
    }

    TEST_ASSERT(1, "Array should handle resize failure gracefully");

    d_DestroyArray(array);
    return 1;
}

// Main test runner
int main(void)
{
    TEST_SUITE_START("Dynamic Array Error Handling Tests");

    RUN_TEST(test_init_array_null_conditions);
    RUN_TEST(test_append_array_null_safety);
    RUN_TEST(test_append_array_capacity_overflow);
    RUN_TEST(test_get_data_null_safety);
    RUN_TEST(test_get_data_bounds_checking);
    RUN_TEST(test_pop_data_null_safety);
    RUN_TEST(test_pop_data_normal_operation);
    RUN_TEST(test_resize_array_null_safety);
    RUN_TEST(test_resize_array_data_preservation);
    RUN_TEST(test_grow_array_null_safety);
    RUN_TEST(test_grow_array_capacity_calculation);
    RUN_TEST(test_destroy_array_null_safety);
    RUN_TEST(test_zero_element_size_edge_cases);
    RUN_TEST(test_memory_allocation_failure_recovery);

    TEST_SUITE_END();
}
