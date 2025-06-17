// File: true_tests/dynamicarrays/test_dynamic_array_advanced.c - Advanced dynamic array functionality tests

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

// Helper structures for complex testing
typedef struct {
    int id;
    char name[32];
    float score;
    int active;
} GameEntity;

typedef struct {
    dArray_t* children;
    int value;
    char tag[16];
} TreeNode;

// Test Functions
int test_dynamic_array_complex_structures(void)
{
    dArray_t* entities = d_InitArray(10, sizeof(GameEntity));

    // Create test entities
    GameEntity test_entities[] = {
        {1, "Player", 100.5f, 1},
        {2, "Enemy1", 75.0f, 1},
        {3, "PowerUp", 0.0f, 0},
        {4, "Boss", 500.0f, 1}
    };

    // Add entities to array
    for (int i = 0; i < 4; i++) {
        d_AppendArray(entities, &test_entities[i]);
    }

    TEST_ASSERT(entities->count == 4, "Should have 4 entities");

    // Verify entity data integrity
    for (size_t i = 0; i < entities->count; i++) {
        GameEntity* entity = (GameEntity*)d_GetDataFromArrayByIndex(entities, i);
        TEST_ASSERT(entity != NULL, "Should get valid entity pointer");
        TEST_ASSERT(entity->id == test_entities[i].id, "Entity ID should match");
        TEST_ASSERT(strcmp(entity->name, test_entities[i].name) == 0, "Entity name should match");
        TEST_ASSERT(entity->score == test_entities[i].score, "Entity score should match");
        TEST_ASSERT(entity->active == test_entities[i].active, "Entity active status should match");
    }

    // Test filtering active entities
    int active_count = 0;
    for (size_t i = 0; i < entities->count; i++) {
        GameEntity* entity = (GameEntity*)d_GetDataFromArrayByIndex(entities, i);
        if (entity && entity->active) {
            active_count++;
        }
    }
    TEST_ASSERT(active_count == 3, "Should have 3 active entities");

    d_DestroyArray(entities);
    return 1;
}

int test_dynamic_array_nested_arrays(void)
{
    // Create a tree-like structure with nested arrays
    dArray_t* tree_nodes = d_InitArray(5, sizeof(TreeNode));

    // Create root node with children
    TreeNode root = {NULL, 1, "root"};
    root.children = d_InitArray(3, sizeof(int));  // Simple int children for this test

    int child_values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        d_AppendArray(root.children, &child_values[i]);
    }

    d_AppendArray(tree_nodes, &root);

    // Create leaf nodes
    TreeNode leaf1 = {NULL, 2, "leaf1"};
    TreeNode leaf2 = {NULL, 3, "leaf2"};
    d_AppendArray(tree_nodes, &leaf1);
    d_AppendArray(tree_nodes, &leaf2);

    TEST_ASSERT(tree_nodes->count == 3, "Should have 3 tree nodes");

    // Verify root node and its children
    TreeNode* root_node = (TreeNode*)d_GetDataFromArrayByIndex(tree_nodes, 0);
    TEST_ASSERT(root_node != NULL, "Should get valid root node");
    TEST_ASSERT(root_node->value == 1, "Root value should be 1");
    TEST_ASSERT(strcmp(root_node->tag, "root") == 0, "Root tag should match");
    TEST_ASSERT(root_node->children != NULL, "Root should have children array");
    TEST_ASSERT(root_node->children->count == 3, "Root should have 3 children");

    // Verify children values
    for (int i = 0; i < 3; i++) {
        int* child_val = (int*)d_GetDataFromArrayByIndex(root_node->children, i);
        TEST_ASSERT(child_val != NULL, "Should get valid child value");
        TEST_ASSERT(*child_val == child_values[i], "Child value should match");
    }

    // Cleanup nested arrays manually
    TreeNode* node = (TreeNode*)d_GetDataFromArrayByIndex(tree_nodes, 0);
    if (node && node->children) {
        d_DestroyArray(node->children);
    }
    d_DestroyArray(tree_nodes);
    return 1;
}

int test_dynamic_array_memory_reuse_patterns(void)
{
    dArray_t* array = d_InitArray(100, sizeof(int));

    // Fill array completely
    for (int i = 0; i < 100; i++) {
        int value = i;
        d_AppendArray(array, &value);
    }

    TEST_ASSERT(array->count == 100, "Array should be full");

    // Pop half the elements
    for (int i = 0; i < 50; i++) {
        int* popped = (int*)d_PopDataFromArray(array);
        TEST_ASSERT(popped != NULL, "Pop should succeed");
    }

    TEST_ASSERT(array->count == 50, "Should have 50 elements remaining");

    // Verify remaining elements are still correct
    for (int i = 0; i < 50; i++) {
        int* value = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(value != NULL, "Should get valid pointer");
        TEST_ASSERT(*value == i, "Value should match original");
    }

    // Refill the array (testing memory reuse)
    for (int i = 0; i < 50; i++) {
        int value = 1000 + i;  // Different values to test overwriting
        d_AppendArray(array, &value);
    }

    TEST_ASSERT(array->count == 100, "Array should be full again");

    // Verify the new section
    for (int i = 50; i < 100; i++) {
        int* value = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(value != NULL, "Should get valid pointer for new values");
        TEST_ASSERT(*value == 1000 + (i - 50), "New values should be correct");
    }

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_large_element_sizes(void)
{
    // Test with large structs
    typedef struct {
        char data[1024];  // 1KB struct
        int id;
        double timestamp;
    } LargeStruct;

    dArray_t* large_array = d_InitArray(10, sizeof(LargeStruct));

    LargeStruct large_items[3];
    for (int i = 0; i < 3; i++) {
        memset(large_items[i].data, 'A' + i, sizeof(large_items[i].data) - 1);
        large_items[i].data[sizeof(large_items[i].data) - 1] = '\0';
        large_items[i].id = i + 100;
        large_items[i].timestamp = 1000.0 + i;
    }

    // Add large structures
    for (int i = 0; i < 3; i++) {
        d_AppendArray(large_array, &large_items[i]);
    }

    TEST_ASSERT(large_array->count == 3, "Should have 3 large items");

    // Verify large structure integrity
    for (int i = 0; i < 3; i++) {
        LargeStruct* retrieved = (LargeStruct*)d_GetDataFromArrayByIndex(large_array, i);
        TEST_ASSERT(retrieved != NULL, "Should get valid large struct pointer");
        TEST_ASSERT(retrieved->id == large_items[i].id, "Large struct ID should match");
        TEST_ASSERT(retrieved->timestamp == large_items[i].timestamp, "Large struct timestamp should match");

        // Check data integrity (first few bytes)
        char expected_char = 'A' + i;
        TEST_ASSERT(retrieved->data[0] == expected_char, "Large struct data should be preserved");
        TEST_ASSERT(retrieved->data[100] == expected_char, "Large struct data integrity check");
    }

    d_DestroyArray(large_array);
    return 1;
}

int test_dynamic_array_sorting_simulation(void)
{
    dArray_t* numbers = d_InitArray(20, sizeof(int));

    // Add unsorted numbers
    int unsorted[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42, 30, 5, 77, 55, 28, 35, 70, 15, 8};
    for (int i = 0; i < 20; i++) {
        d_AppendArray(numbers, &unsorted[i]);
    }

    // Simulate bubble sort using array operations
    for (size_t i = 0; i < numbers->count - 1; i++) {
        for (size_t j = 0; j < numbers->count - i - 1; j++) {
            int* current = (int*)d_GetDataFromArrayByIndex(numbers, j);
            int* next = (int*)d_GetDataFromArrayByIndex(numbers, j + 1);

            if (current && next && *current > *next) {
                // Swap values
                int temp = *current;
                *current = *next;
                *next = temp;
            }
        }
    }

    // Verify array is sorted
    for (size_t i = 0; i < numbers->count - 1; i++) {
        int* current = (int*)d_GetDataFromArrayByIndex(numbers, i);
        int* next = (int*)d_GetDataFromArrayByIndex(numbers, i + 1);
        TEST_ASSERT(current != NULL && next != NULL, "Should get valid pointers");
        TEST_ASSERT(*current <= *next, "Array should be sorted in ascending order");
    }

    d_DestroyArray(numbers);
    return 1;
}

int test_dynamic_array_capacity_management(void)
{
    dArray_t* array = d_InitArray(5, sizeof(int));
    size_t original_capacity = array->capacity;

    // Fill to capacity
    for (int i = 0; i < 5; i++) {
        int value = i;
        d_AppendArray(array, &value);
    }

    TEST_ASSERT(array->count == 5, "Should be at capacity");
    TEST_ASSERT(array->capacity == original_capacity, "Capacity should be unchanged");

    // Try to exceed capacity (should fail with current implementation)
    int extra_value = 999;
    size_t count_before = array->count;
    d_AppendArray(array, &extra_value);
    TEST_ASSERT(array->count == count_before, "Count should not increase when at capacity");

    // Test manual capacity expansion
    int resize_result = d_ResizeArray(array, 10 * sizeof(int));
    TEST_ASSERT(resize_result == 0, "Resize should succeed");
    TEST_ASSERT(array->capacity == 10 * sizeof(int), "Capacity should be increased");

    // Now adding should work
    d_AppendArray(array, &extra_value);
    TEST_ASSERT(array->count == 6, "Should be able to add after resize");

    // Verify data integrity after resize
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_GetDataFromArrayByIndex(array, i);
        TEST_ASSERT(value != NULL && *value == i, "Original data should be preserved");
    }

    int* last_value = (int*)d_GetDataFromArrayByIndex(array, 5);
    TEST_ASSERT(last_value != NULL && *last_value == extra_value, "New value should be correct");

    d_DestroyArray(array);
    return 1;
}

int test_dynamic_array_iterator_pattern(void)
{
    dArray_t* strings = d_InitArray(5, sizeof(char*));

    // Note: This is a simplified test - in real use you'd need to manage string memory
    char* test_strings[] = {"hello", "world", "test", "array", "iterator"};

    for (int i = 0; i < 5; i++) {
        d_AppendArray(strings, &test_strings[i]);
    }

    // Test iterator-like access pattern
    size_t word_count = 0;
    size_t total_length = 0;

    for (size_t i = 0; i < strings->count; i++) {
        char** str_ptr = (char**)d_GetDataFromArrayByIndex(strings, i);
        if (str_ptr && *str_ptr) {
            word_count++;
            total_length += strlen(*str_ptr);
        }
    }

    TEST_ASSERT(word_count == 5, "Should count all words");
    TEST_ASSERT(total_length == 27, "Total length should be sum of all strings"); // hello(5) + world(5) + test(4) + array(5) + iterator(8) = 27, wait let me recount: 5+5+4+5+8=27, hmm let me check: "hello"=5, "world"=5, "test"=4, "array"=5, "iterator"=8, total=27

    // Let me fix the expected value
    TEST_ASSERT(total_length == 27, "Total length should be sum of all strings");

    d_DestroyArray(strings);
    return 1;
}

int test_dynamic_array_stack_behavior(void)
{
    dArray_t* stack = d_InitArray(10, sizeof(int));

    // Test LIFO stack behavior
    int push_values[] = {10, 20, 30, 40, 50};

    // Push values
    for (int i = 0; i < 5; i++) {
        d_AppendArray(stack, &push_values[i]);
    }

    TEST_ASSERT(stack->count == 5, "Stack should have 5 elements");

    // Pop values and verify LIFO order
    for (int i = 4; i >= 0; i--) {  // Reverse order
        int* popped = (int*)d_PopDataFromArray(stack);
        TEST_ASSERT(popped != NULL, "Pop should return valid pointer");
        TEST_ASSERT(*popped == push_values[i], "Should pop in LIFO order");
        TEST_ASSERT(stack->count == (size_t)i, "Count should decrease");
    }

    TEST_ASSERT(stack->count == 0, "Stack should be empty");

    // Test pop from empty stack
    int* empty_pop = (int*)d_PopDataFromArray(stack);
    TEST_ASSERT(empty_pop == NULL, "Pop from empty stack should return NULL");

    d_DestroyArray(stack);
    return 1;
}

// Main test runner
int main(void)
{
    TEST_SUITE_START("Dynamic Array Advanced Tests");

    RUN_TEST(test_dynamic_array_complex_structures);
    RUN_TEST(test_dynamic_array_nested_arrays);
    RUN_TEST(test_dynamic_array_memory_reuse_patterns);
    RUN_TEST(test_dynamic_array_large_element_sizes);
    RUN_TEST(test_dynamic_array_sorting_simulation);
    RUN_TEST(test_dynamic_array_capacity_management);
    RUN_TEST(test_dynamic_array_iterator_pattern);
    RUN_TEST(test_dynamic_array_stack_behavior);

    TEST_SUITE_END();
}
