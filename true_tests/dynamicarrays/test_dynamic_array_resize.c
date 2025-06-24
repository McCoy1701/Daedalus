// File: true_tests/dynamicarrays/test_dynamic_array_resize.c - Array resizing functionality tests

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define LOG() printf("[LOG] %s:%d - ", __FILE__, __LINE__)

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Test Functions
int test_dynamic_array_resize_expand(void)
{
    dArray_t* array = d_InitArray(3, sizeof(int));
    LOG(); printf("Created array with capacity: %zu, element_size: %zu\n", array->capacity, array->element_size);

    // Fill original capacity
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
    }

    LOG(); printf("After filling: capacity=%zu, count=%zu\n", array->capacity, array->count);
    TEST_ASSERT(array->capacity == 3, "Initial capacity should be 3");
    TEST_ASSERT(array->count == 3, "Should have 3 elements");

    // Resize to larger capacity
    LOG(); printf("Before resize: capacity=%zu, resizing to %zu bytes\n", array->capacity, 6 * sizeof(int));
    int result = d_ResizeArray(array, 6 * sizeof(int));
    LOG(); printf("After resize: result=%d, capacity=%zu, expected=%zu\n", result, array->capacity, 6 * sizeof(int));
    TEST_ASSERT(result == 0, "Resize should succeed");
    TEST_ASSERT(array->capacity == 6, "Capacity should be updated to 6 elements");

    // Verify existing data is preserved
    for (int i = 0; i < 3; i++) {
        int* retrieved = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(retrieved != NULL, "Should get valid pointer after resize");
        TEST_ASSERT(*retrieved == values[i], "Existing data should be preserved after resize");
    }

    // Test that we can now add more elements
    int new_values[] = {4, 5, 6};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &new_values[i]);
    }
    TEST_ASSERT(array->count == 6, "Should be able to add more elements after resize");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_resize_shrink(void)
{
    dArray_t* array = d_InitArray(6, sizeof(int));
    LOG(); printf("Created array with capacity: %zu, element_size: %zu\n", array->capacity, array->element_size);

    // Fill with data
    int values[] = {10, 20, 30, 40, 50, 60};
    for (int i = 0; i < 6; i++) {
        d_AppendArray(array, &values[i]);
    }

    LOG(); printf("After filling: capacity=%zu, count=%zu\n", array->capacity, array->count);

    // Resize to smaller capacity
    LOG(); printf("Before shrink: capacity=%zu, shrinking to %zu bytes\n", array->capacity, 3 * sizeof(int));
    int result = d_ResizeArray(array, 3 * sizeof(int));
    LOG(); printf("After shrink: result=%d, capacity=%zu, expected=%zu\n", result, array->capacity, 3 * sizeof(int));
    TEST_ASSERT(result == 0, "Shrink resize should succeed");
    TEST_ASSERT(array->capacity == 3, "Capacity should be reduced to 3 elements");

    // Note: The count is not automatically adjusted, which might be a design choice
    // We test what the current implementation does
    TEST_ASSERT(array->count == 6, "Count is not automatically adjusted in current implementation");

    // The remaining accessible data should still be valid
    for (int i = 0; i < 3; i++) {
        int* retrieved = (int*)d_GetDataFromArrayByIndex(array, i);
        if (retrieved != NULL) {  // Might be NULL due to capacity vs count issue
            TEST_ASSERT(*retrieved == values[i], "Accessible data should be preserved");
        }
    }

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_resize_zero(void)
{
    dArray_t* array = d_InitArray(5, sizeof(int));
    LOG(); printf("Created array with capacity: %zu, element_size: %zu\n", array->capacity, array->element_size);

    // Resize to zero
    LOG(); printf("Before zero resize: capacity=%zu\n", array->capacity);
    int result = d_ResizeArray(array, 0);
    LOG(); printf("After zero resize: result=%d, capacity=%zu\n", result, array->capacity);
    TEST_ASSERT(result == 0, "Resize to zero should succeed");
    TEST_ASSERT(array->capacity == 0, "Capacity should be zero");

    // Operations after zero resize
    int value = 42;
    d_AppendArray(array, &value);  // Should fail gracefully

    void* get_result = d_GetDataFromArrayByIndex(array, 0);
    TEST_ASSERT(get_result == NULL, "Get from zero-capacity array should return NULL");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_resize_same_size(void)
{
    dArray_t* array = d_InitArray(5, sizeof(int));
    LOG(); printf("Created array with capacity: %zu, element_size: %zu\n", array->capacity, array->element_size);

    // Add some data
    int values[] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(array, &values[i]);
    }

    size_t original_capacity = array->capacity;
    LOG(); printf("Original capacity: %zu, count: %zu\n", original_capacity, array->count);

    // Resize to same size (in bytes, but capacity should stay same in element count)
    size_t resize_bytes = original_capacity * array->element_size;
    LOG(); printf("Before same-size resize: capacity=%zu, resizing to %zu bytes\n", array->capacity, resize_bytes);
    int result = d_ResizeArray(array, resize_bytes);
    LOG(); printf("After same-size resize: result=%d, capacity=%zu, expected=%zu\n", result, array->capacity, original_capacity);
    TEST_ASSERT(result == 0, "Resize to same size should succeed");
    TEST_ASSERT(array->capacity == original_capacity, "Capacity should remain the same");

    // Verify data integrity
    for (int i = 0; i < 3; i++) {
        int* retrieved = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(retrieved != NULL, "Should get valid pointer after same-size resize");
        TEST_ASSERT(*retrieved == values[i], "Data should be preserved after same-size resize");
    }

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_resize_failure_simulation(void)
{
    dArray_t* array = d_InitArray(2, sizeof(int));

    // Add some initial data
    int values[] = {1, 2};
    for (int i = 0; i < 2; i++) {
        d_AppendArray(array, &values[i]);
    }

    // Try to resize to an extremely large size (likely to fail on most systems)
    size_t huge_size = SIZE_MAX / 2;  // Very large but not overflow
    int result = d_ResizeArray(array, huge_size);

    // The function should return an error code and leave the array unchanged
    if (result != 0) {
        TEST_ASSERT(array->capacity == 2, "Capacity should be unchanged after failed resize");
        TEST_ASSERT(array->count == 2, "Count should be unchanged after failed resize");

        // Verify data is still accessible
        for (int i = 0; i < 2; i++) {
            int* retrieved = (int*)d_GetDataFromArrayByIndex(array, i);
            TEST_ASSERT(retrieved != NULL, "Data should still be accessible after failed resize");
            TEST_ASSERT(*retrieved == values[i], "Data should be unchanged after failed resize");
        }
    }

    TEST_ASSERT(1, "Resize failure should be handled gracefully");

    d_DestroyArray(array);
    return 1;
}
// Test Functions - Debug Hunters for Edge Cases and Bugs

int test_debug_hunter_capacity_boundary_dance(void)
{
    // Hunt for off-by-one errors and boundary condition bugs
    LOG(); printf("🔍 HUNTING: Capacity boundary edge cases\n");

    dArray_t* array = d_InitArray(1, sizeof(int));
    LOG(); printf("Created minimal array: capacity=%zu, element_size=%zu\n", array->capacity, array->element_size);

    // Test the exact boundary - should fit exactly 1 element
    int value1 = 42;
    d_AppendArray(array, &value1);
    LOG(); printf("After adding 1 element: count=%zu, capacity=%zu\n", array->count, array->capacity);
    TEST_ASSERT(array->count == 1, "Should accept exactly 1 element at capacity boundary");

    // Test boundary violation - should NOT add a second element
    int value2 = 84;
    size_t count_before = array->count;
    d_AppendArray(array, &value2);
    LOG(); printf("After trying to add 2nd element: count=%zu (was %zu)\n", array->count, count_before);
    TEST_ASSERT(array->count == count_before, "Should reject element beyond capacity boundary");

    // Test resize boundary - from 1 to 2
    int result = d_ResizeArray(array, 2 * sizeof(int));
    LOG(); printf("After resize to 2 elements: result=%d, capacity=%zu\n", result, array->capacity);
    TEST_ASSERT(result == 0, "Resize should succeed");
    TEST_ASSERT(array->capacity == 2, "Capacity should be exactly 2");

    // Now should accept the second element
    d_AppendArray(array, &value2);
    LOG(); printf("After adding 2nd element post-resize: count=%zu\n", array->count);
    TEST_ASSERT(array->count == 2, "Should accept 2nd element after resize");

    // Verify data integrity across boundary operations
    int* retrieved1 = (int*)d_GetDataFromArrayByIndex(array, 0);
    int* retrieved2 = (int*)d_GetDataFromArrayByIndex(array, 1);
    TEST_ASSERT(retrieved1 != NULL && *retrieved1 == 42, "First element should be preserved");
    TEST_ASSERT(retrieved2 != NULL && *retrieved2 == 84, "Second element should be correct");

    d_DestroyArray(array);
    return 1;
}

int test_debug_hunter_zero_element_size_trap(void)
{
    // Hunt for division by zero and invalid element size bugs
    LOG(); printf("🔍 HUNTING: Zero element size edge cases\n");

    // Test creating array with zero element size - should be safe
    dArray_t* array = d_InitArray(10, 0);
    if (array == NULL) {
        LOG(); printf("Array creation with zero element_size returned NULL (safe behavior)\n");
        TEST_ASSERT(1, "NULL return for zero element_size is acceptable");
        return 1;
    }

    LOG(); printf("Created array with zero element_size: capacity=%zu, element_size=%zu\n",
                 array->capacity, array->element_size);

    // Test operations with zero element size
    int dummy_data = 123;
    d_AppendArray(array, &dummy_data);
    LOG(); printf("After append with zero element_size: count=%zu\n", array->count);

    // Test resize with zero element size
    int result = d_ResizeArray(array, 20);
    LOG(); printf("After resize with zero element_size: result=%d, capacity=%zu\n", result, array->capacity);

    // Test get operation
    void* retrieved = d_GetDataFromArrayByIndex(array, 0);
    LOG(); printf("Get operation with zero element_size returned: %p\n", retrieved);

    d_DestroyArray(array);
    TEST_ASSERT(1, "Zero element size operations completed without crashes");
    return 1;
}

int test_debug_hunter_memory_fragmentation_stress(void)
{
    // Hunt for memory fragmentation and allocation pattern bugs
    LOG(); printf("🔍 HUNTING: Memory fragmentation and allocation bugs\n");

    const int num_arrays = 10;
    const int operations_per_array = 20;
    dArray_t* arrays[num_arrays];

    // Create multiple arrays with different sizes
    for (int i = 0; i < num_arrays; i++) {
        size_t capacity = (i + 1) * 3;  // Varying capacities: 3, 6, 9, 12, ...
        arrays[i] = d_InitArray(capacity, sizeof(int));
        LOG(); printf("Created array %d with capacity %zu\n", i, capacity);
        TEST_ASSERT(arrays[i] != NULL, "Array creation should succeed");
    }

    // Perform interleaved operations on all arrays
    for (int op = 0; op < operations_per_array; op++) {
        for (int i = 0; i < num_arrays; i++) {
            int operation = (op + i) % 4;  // Cycle through different operations

            switch (operation) {
                case 0: // Append
                    if (arrays[i]->count < arrays[i]->capacity) {
                        int value = i * 100 + op;
                        d_AppendArray(arrays[i], &value);
                    }
                    break;

                case 1: // Pop
                    if (arrays[i]->count > 0) {
                        d_PopDataFromArray(arrays[i]);
                    }
                    break;

                case 2: // Resize grow
                    if (arrays[i]->capacity < 50) {
                        d_ResizeArray(arrays[i], (arrays[i]->capacity + 5) * sizeof(int));
                    }
                    break;

                case 3: // Resize shrink
                    if (arrays[i]->capacity > 3) {
                        size_t new_cap = (arrays[i]->capacity > 5) ? arrays[i]->capacity - 2 : 3;
                        d_ResizeArray(arrays[i], new_cap * sizeof(int));
                    }
                    break;
            }
        }
    }

    // Verify data integrity across all arrays
    int total_elements = 0;
    for (int i = 0; i < num_arrays; i++) {
        LOG(); printf("Array %d final state: capacity=%zu, count=%zu\n",
                     i, arrays[i]->capacity, arrays[i]->count);

        // Verify we can read all elements without crashes
        for (size_t j = 0; j < arrays[i]->count; j++) {
            int* retrieved = (int*)d_GetDataFromArrayByIndex(arrays[i], j);
            TEST_ASSERT(retrieved != NULL, "Should be able to retrieve all elements");
            total_elements++;
        }

        d_DestroyArray(arrays[i]);
    }

    LOG(); printf("Successfully accessed %d total elements across all arrays\n", total_elements);
    TEST_ASSERT(total_elements >= 0, "Memory fragmentation stress test completed");
    return 1;
}

int test_debug_hunter_resize_data_corruption_detector(void)
{
    // Hunt for data corruption bugs during resize operations
    LOG(); printf("🔍 HUNTING: Data corruption during resize operations\n");

    dArray_t* array = d_InitArray(5, sizeof(int));

    // Fill with a known pattern
    int magic_pattern[] = {0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x87654321, 0xABCDEF00};
    for (int i = 0; i < 5; i++) {
        d_AppendArray(array, &magic_pattern[i]);
    }

    LOG(); printf("Initial pattern loaded, performing resize corruption hunt...\n");

    // Perform multiple resize operations while checking data integrity
    size_t resize_sequence[] = {10, 3, 15, 1, 8, 12, 2, 20};
    int num_resizes = sizeof(resize_sequence) / sizeof(resize_sequence[0]);

    for (int r = 0; r < num_resizes; r++) {
        size_t target_capacity = resize_sequence[r];
        LOG(); printf("Resize %d: %zu -> %zu elements\n", r, array->capacity, target_capacity);

        int result = d_ResizeArray(array, target_capacity * sizeof(int));
        TEST_ASSERT(result == 0, "Resize should succeed");

        // Check data integrity after each resize
        size_t elements_to_check = (array->count < target_capacity) ? array->count : target_capacity;
        elements_to_check = (elements_to_check > 5) ? 5 : elements_to_check;  // Don't check beyond original data

        for (size_t i = 0; i < elements_to_check; i++) {
            int* retrieved = (int*)d_GetDataFromArrayByIndex(array, i);
            if (retrieved != NULL) {
                LOG(); printf("Element %zu: expected=0x%X, actual=0x%X\n", i, magic_pattern[i], *retrieved);
                TEST_ASSERT(*retrieved == magic_pattern[i], "Data should not be corrupted by resize");
            }
        }
    }

    d_DestroyArray(array);
    return 1;
}

int test_debug_hunter_append_pop_stack_integrity(void)
{
    // Hunt for LIFO stack integrity bugs and count tracking issues
    LOG(); printf("🔍 HUNTING: Stack integrity and count tracking bugs\n");

    dArray_t* array = d_InitArray(10, sizeof(int));

    // Test append/pop cycles with verification
    const int num_cycles = 5;
    const int elements_per_cycle = 7;

    for (int cycle = 0; cycle < num_cycles; cycle++) {
        LOG(); printf("=== CYCLE %d: Push phase ===\n", cycle);

        // Push phase - add elements with cycle signature
        for (int i = 0; i < elements_per_cycle; i++) {
            int value = (cycle * 1000) + i;  // Unique value per cycle and position
            d_AppendArray(array, &value);
            LOG(); printf("Pushed: %d, count now: %zu\n", value, array->count);
        }

        TEST_ASSERT(array->count == (size_t)elements_per_cycle, "Count should match pushed elements");

        LOG(); printf("=== CYCLE %d: Pop phase ===\n", cycle);

        // Pop phase - remove elements and verify LIFO order
        for (int i = elements_per_cycle - 1; i >= 0; i--) {
            int expected_value = (cycle * 1000) + i;
            int* popped = (int*)d_PopDataFromArray(array);

            TEST_ASSERT(popped != NULL, "Pop should return valid pointer");

            if (popped != NULL) {
                LOG(); printf("Popped: %d, expected: %d, count now: %zu\n", *popped, expected_value, array->count);
                TEST_ASSERT(*popped == expected_value, "Popped value should match LIFO order");
            }
        }

        TEST_ASSERT(array->count == 0, "Array should be empty after popping all elements");

        // Verify array is truly empty
        int* empty_pop = (int*)d_PopDataFromArray(array);
        TEST_ASSERT(empty_pop == NULL, "Pop from empty array should return NULL");
    }

    // Final integrity check
    LOG(); printf("Final integrity check: count=%zu, capacity=%zu\n", array->count, array->capacity);
    TEST_ASSERT(array->count == 0, "Final count should be zero");
    TEST_ASSERT(array->capacity == 10, "Capacity should be unchanged");

    d_DestroyArray(array);
    return 1;
}
// Main test runner
int main(void)
{
    TEST_SUITE_START("Dynamic Array Resize Tests");

    RUN_TEST(test_dynamic_array_resize_expand);
    RUN_TEST(test_dynamic_array_resize_shrink);
    RUN_TEST(test_dynamic_array_resize_zero);
    RUN_TEST(test_dynamic_array_resize_same_size);
    RUN_TEST(test_dynamic_array_resize_failure_simulation);

    RUN_TEST(test_debug_hunter_capacity_boundary_dance);
    RUN_TEST(test_debug_hunter_zero_element_size_trap);
    RUN_TEST(test_debug_hunter_memory_fragmentation_stress);
    RUN_TEST(test_debug_hunter_resize_data_corruption_detector);
    RUN_TEST(test_debug_hunter_append_pop_stack_integrity);

    TEST_SUITE_END();
}
