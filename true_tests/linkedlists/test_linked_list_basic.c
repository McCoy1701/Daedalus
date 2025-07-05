// File: true_tests/linkedlists/test_linked_list_basic.c - Basic linked list functionality tests

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Test Functions
int test_linked_list_init_destroy(void)
{
    // Test basic initialization
    int value = 42;
    dLinkedList_t* list = d_InitLinkedList(&value, "first_node", sizeof(int));

    TEST_ASSERT(list != NULL, "Should create linked list successfully");
    TEST_ASSERT(list->data != NULL, "Data pointer should be allocated");
    TEST_ASSERT(*(int*)list->data == value, "Data should be copied correctly");
    TEST_ASSERT(strcmp(list->buffer, "first_node") == 0, "Name should be copied correctly");
    TEST_ASSERT(list->next == NULL, "Single node should have next as NULL");

    // Test destroy (should not crash)
    d_DestroyLinkedList(&list);
    TEST_ASSERT(list == NULL, "List pointer should be NULL after destroy");

    return 1;
}

int test_linked_list_push_back(void)
{
    // Start with empty list
    dLinkedList_t* list = NULL;
    
    // Test pushing to empty list
    int value1 = 10;
    int result = d_PushBackToLinkedList(&list, &value1, "item1", sizeof(int));
    TEST_ASSERT(result == 0, "Push back to empty list should succeed");
    TEST_ASSERT(list != NULL, "List should not be NULL after first push");
    TEST_ASSERT(*(int*)list->data == value1, "First item data should be correct");
    TEST_ASSERT(strcmp(list->buffer, "item1") == 0, "First item name should be correct");

    // Test pushing second item
    int value2 = 20;
    result = d_PushBackToLinkedList(&list, &value2, "item2", sizeof(int));
    TEST_ASSERT(result == 0, "Push back second item should succeed");
    TEST_ASSERT(list->next != NULL, "Second node should exist");
    TEST_ASSERT(*(int*)list->next->data == value2, "Second item data should be correct");
    TEST_ASSERT(strcmp(list->next->buffer, "item2") == 0, "Second item name should be correct");

    // Test pushing third item
    int value3 = 30;
    result = d_PushBackToLinkedList(&list, &value3, "item3", sizeof(int));
    TEST_ASSERT(result == 0, "Push back third item should succeed");
    TEST_ASSERT(list->next->next != NULL, "Third node should exist");
    TEST_ASSERT(*(int*)list->next->next->data == value3, "Third item data should be correct");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_linked_list_push_front(void)
{
    // Create initial list
    int value1 = 10;
    dLinkedList_t* list = d_InitLinkedList(&value1, "original", sizeof(int));

    // Test pushing to front
    int value2 = 20;
    int result = d_PushFrontToLinkedList(&list, &value2, "new_front", sizeof(int));
    TEST_ASSERT(result == 0, "Push front should succeed");
    TEST_ASSERT(*(int*)list->data == value2, "New front data should be correct");
    TEST_ASSERT(strcmp(list->buffer, "new_front") == 0, "New front name should be correct");
    TEST_ASSERT(list->next != NULL, "Original node should still exist");
    TEST_ASSERT(*(int*)list->next->data == value1, "Original node data should be preserved");

    // Test pushing another to front
    int value3 = 30;
    result = d_PushFrontToLinkedList(&list, &value3, "newest_front", sizeof(int));
    TEST_ASSERT(result == 0, "Second push front should succeed");
    TEST_ASSERT(*(int*)list->data == value3, "Newest front data should be correct");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_linked_list_index_access(void)
{
    // Create list with multiple items
    dLinkedList_t* list = NULL;
    int values[] = {100, 200, 300, 400};
    char names[][20] = {"first", "second", "third", "fourth"};

    for (int i = 0; i < 4; i++) {
        d_PushBackToLinkedList(&list, &values[i], names[i], sizeof(int));
    }

    // Test valid indices
    for (int i = 0; i < 4; i++) {
        int* retrieved = (int*)d_IndexDataFromLinkedList(list, i);
        TEST_ASSERT(retrieved != NULL, "Should get valid pointer for valid index");
        TEST_ASSERT(*retrieved == values[i], "Retrieved value should match stored value");
    }

    // Test invalid indices
    void* invalid = d_IndexDataFromLinkedList(list, 10);
    TEST_ASSERT(invalid == NULL, "Should return NULL for out of bounds index");

    void* negative = d_IndexDataFromLinkedList(list, -1);
    TEST_ASSERT(negative == NULL, "Should return NULL for negative index");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_linked_list_find_by_name(void)
{
    // Create list with multiple items
    dLinkedList_t* list = NULL;
    int value1 = 111, value2 = 222, value3 = 333;

    d_PushBackToLinkedList(&list, &value1, "alpha", sizeof(int));
    d_PushBackToLinkedList(&list, &value2, "beta", sizeof(int));
    d_PushBackToLinkedList(&list, &value3, "gamma", sizeof(int));

    // Test finding existing names
    int* found1 = (int*)d_FindDataFromLinkedList(list, "alpha");
    TEST_ASSERT(found1 != NULL, "Should find 'alpha' node");
    TEST_ASSERT(*found1 == value1, "Found 'alpha' should have correct value");

    int* found2 = (int*)d_FindDataFromLinkedList(list, "beta");
    TEST_ASSERT(found2 != NULL, "Should find 'beta' node");
    TEST_ASSERT(*found2 == value2, "Found 'beta' should have correct value");

    int* found3 = (int*)d_FindDataFromLinkedList(list, "gamma");
    TEST_ASSERT(found3 != NULL, "Should find 'gamma' node");
    TEST_ASSERT(*found3 == value3, "Found 'gamma' should have correct value");

    // Test finding non-existent name
    void* not_found = d_FindDataFromLinkedList(list, "delta");
    TEST_ASSERT(not_found == NULL, "Should return NULL for non-existent name");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_linked_list_remove_by_index(void)
{
    // Create list with multiple items
    dLinkedList_t* list = NULL;
    int values[] = {10, 20, 30, 40, 50};
    char names[][20] = {"a", "b", "c", "d", "e"};

    for (int i = 0; i < 5; i++) {
        d_PushBackToLinkedList(&list, &values[i], names[i], sizeof(int));
    }

    // Test removing middle element (index 2)
    int result = d_RemoveIndexFromLinkedList(&list, 2);
    TEST_ASSERT(result == 0, "Remove by index should succeed");
    
    // Verify the list structure after removal
    TEST_ASSERT(*(int*)d_IndexDataFromLinkedList(list, 0) == 10, "Index 0 should still be 10");
    TEST_ASSERT(*(int*)d_IndexDataFromLinkedList(list, 1) == 20, "Index 1 should still be 20");
    TEST_ASSERT(*(int*)d_IndexDataFromLinkedList(list, 2) == 40, "Index 2 should now be 40 (was index 3)");
    TEST_ASSERT(*(int*)d_IndexDataFromLinkedList(list, 3) == 50, "Index 3 should now be 50 (was index 4)");
    TEST_ASSERT(d_IndexDataFromLinkedList(list, 4) == NULL, "Index 4 should now be out of bounds");

    // Test removing head (index 0)
    result = d_RemoveIndexFromLinkedList(&list, 0);
    TEST_ASSERT(result == 0, "Remove head should succeed");
    TEST_ASSERT(*(int*)list->data == 20, "New head should be 20");

    // Test removing invalid index
    result = d_RemoveIndexFromLinkedList(&list, 10);
    TEST_ASSERT(result == 1, "Remove invalid index should fail");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_linked_list_remove_by_name(void)
{
    // Create list with multiple items
    dLinkedList_t* list = NULL;
    int value1 = 100, value2 = 200, value3 = 300;

    d_PushBackToLinkedList(&list, &value1, "first", sizeof(int));
    d_PushBackToLinkedList(&list, &value2, "second", sizeof(int));
    d_PushBackToLinkedList(&list, &value3, "third", sizeof(int));

    // Test removing middle element by name
    int result = d_RemoveDataFromLinkedList(&list, "second");
    TEST_ASSERT(result == 0, "Remove by name should succeed");
    
    // Verify the element is gone
    void* not_found = d_FindDataFromLinkedList(list, "second");
    TEST_ASSERT(not_found == NULL, "Removed element should not be found");
    
    // Verify remaining elements
    int* first = (int*)d_FindDataFromLinkedList(list, "first");
    int* third = (int*)d_FindDataFromLinkedList(list, "third");
    TEST_ASSERT(first != NULL && *first == 100, "First element should remain");
    TEST_ASSERT(third != NULL && *third == 300, "Third element should remain");

    // Test removing head by name
    result = d_RemoveDataFromLinkedList(&list, "first");
    TEST_ASSERT(result == 0, "Remove head by name should succeed");
    TEST_ASSERT(*(int*)list->data == 300, "New head should be third element");

    // Test removing non-existent name
    result = d_RemoveDataFromLinkedList(&list, "nonexistent");
    TEST_ASSERT(result == 1, "Remove non-existent name should fail");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_linked_list_pop_back(void)
{
    // Create list with multiple items
    dLinkedList_t* list = NULL;
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; i++) {
        d_PushBackToLinkedList(&list, &values[i], "item", sizeof(int));
    }

    // Test popping from back
    int* popped = (int*)d_PopBackFromLinkedList(&list);
    TEST_ASSERT(popped != NULL, "Pop back should return valid pointer");
    TEST_ASSERT(*popped == 30, "Popped value should be the last one added");
    free(popped); // Caller is responsible for freeing returned data

    // Verify list structure after pop
    TEST_ASSERT(d_GetLengthOfLinkedList(list) == 2, "List should have 2 elements after pop");
    TEST_ASSERT(d_IndexDataFromLinkedList(list, 2) == NULL, "Index 2 should now be out of bounds");

    // Pop until only one element remains
    popped = (int*)d_PopBackFromLinkedList(&list);
    TEST_ASSERT(popped != NULL && *popped == 20, "Second pop should return 20");
    free(popped);

    // Pop the last element
    popped = (int*)d_PopBackFromLinkedList(&list);
    TEST_ASSERT(popped != NULL && *popped == 10, "Last pop should return 10");
    TEST_ASSERT(list == NULL, "List should be NULL after popping all elements");
    free(popped);

    // Test popping from empty list
    void* empty_pop = d_PopBackFromLinkedList(&list);
    TEST_ASSERT(empty_pop == NULL, "Pop from empty list should return NULL");

    return 1;
}

int test_linked_list_pop_front(void)
{
    // Create list with multiple items
    dLinkedList_t* list = NULL;
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; i++) {
        d_PushBackToLinkedList(&list, &values[i], "item", sizeof(int));
    }

    // Test popping from front
    int* popped = (int*)d_PopFrontFromLinkedList(&list);
    TEST_ASSERT(popped != NULL, "Pop front should return valid pointer");
    TEST_ASSERT(*popped == 10, "Popped value should be the first one");
    free(popped);

    // Verify list structure after pop
    TEST_ASSERT(*(int*)list->data == 20, "New head should be 20");
    TEST_ASSERT(d_GetLengthOfLinkedList(list) == 2, "List should have 2 elements after pop");

    // Pop another from front
    popped = (int*)d_PopFrontFromLinkedList(&list);
    TEST_ASSERT(popped != NULL && *popped == 20, "Second pop should return 20");
    free(popped);

    // Pop the last element
    popped = (int*)d_PopFrontFromLinkedList(&list);
    TEST_ASSERT(popped != NULL && *popped == 30, "Last pop should return 30");
    TEST_ASSERT(list == NULL, "List should be NULL after popping all elements");
    free(popped);

    return 1;
}

int test_linked_list_length(void)
{
    // Test empty list length
    dLinkedList_t* list = NULL;
    TEST_ASSERT(d_GetLengthOfLinkedList(list) == 0, "Empty list should have length 0");

    // Test single element list
    int value = 42;
    list = d_InitLinkedList(&value, "single", sizeof(int));
    TEST_ASSERT(d_GetLengthOfLinkedList(list) == 1, "Single element list should have length 1");

    // Test growing list
    for (int i = 0; i < 5; i++) {
        d_PushBackToLinkedList(&list, &value, "item", sizeof(int));
        TEST_ASSERT(d_GetLengthOfLinkedList(list) == (size_t)(i + 2), "Length should increase with each push");
    }

    TEST_ASSERT(d_GetLengthOfLinkedList(list) == 6, "Final list should have length 6");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_linked_list_different_types(void)
{
    // Test with float data
    dLinkedList_t* float_list = NULL;
    float float_vals[] = {1.5f, 2.7f, 3.9f};

    for (int i = 0; i < 3; i++) {
        d_PushBackToLinkedList(&float_list, &float_vals[i], "float_item", sizeof(float));
    }

    for (int i = 0; i < 3; i++) {
        float* retrieved = (float*)d_IndexDataFromLinkedList(float_list, i);
        TEST_ASSERT(retrieved != NULL, "Should get valid float pointer");
        TEST_ASSERT(*retrieved == float_vals[i], "Float values should match");
    }

    d_DestroyLinkedList(&float_list);

    // Test with struct data
    typedef struct {
        int x, y;
        char name[16];
    } Point;

    dLinkedList_t* point_list = NULL;
    Point points[] = {{10, 20, "point1"}, {30, 40, "point2"}};

    for (int i = 0; i < 2; i++) {
        d_PushBackToLinkedList(&point_list, &points[i], points[i].name, sizeof(Point));
    }

    for (int i = 0; i < 2; i++) {
        Point* retrieved = (Point*)d_IndexDataFromLinkedList(point_list, i);
        TEST_ASSERT(retrieved != NULL, "Should get valid Point pointer");
        TEST_ASSERT(retrieved->x == points[i].x, "Point x values should match");
        TEST_ASSERT(retrieved->y == points[i].y, "Point y values should match");
        TEST_ASSERT(strcmp(retrieved->name, points[i].name) == 0, "Point names should match");
    }

    d_DestroyLinkedList(&point_list);
    return 1;
}

int test_linked_list_error_handling(void)
{
    // Test NULL pointer handling
    dLinkedList_t* list = NULL;
    
    // Test operations on NULL list
    void* result = d_IndexDataFromLinkedList(NULL, 0);
    TEST_ASSERT(result == NULL, "Index on NULL list should return NULL");

    result = d_FindDataFromLinkedList(NULL, "test");
    TEST_ASSERT(result == NULL, "Find on NULL list should return NULL");

    int remove_result = d_RemoveIndexFromLinkedList(NULL, 0);
    TEST_ASSERT(remove_result == 1, "Remove from NULL list should fail");

    remove_result = d_RemoveDataFromLinkedList(NULL, "test");
    TEST_ASSERT(remove_result == 1, "Remove by name from NULL list should fail");

    // Test invalid inputs to push functions
    int value = 42;
    int push_result = d_PushBackToLinkedList(NULL, &value, "test", sizeof(int));
    TEST_ASSERT(push_result == 1, "Push back with NULL head pointer should fail");

    push_result = d_PushBackToLinkedList(&list, NULL, "test", sizeof(int));
    TEST_ASSERT(push_result == 1, "Push back with NULL data should fail");

    push_result = d_PushBackToLinkedList(&list, &value, NULL, sizeof(int));
    TEST_ASSERT(push_result == 1, "Push back with NULL name should fail");

    push_result = d_PushBackToLinkedList(&list, &value, "test", 0);
    TEST_ASSERT(push_result == 1, "Push back with zero size should fail");

    return 1;
}

// Main test runner
int main(void)
{
    // Initialize Daedalus logging system
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = true,
        .include_file_info = false,
        .include_function = false,
        .include_thread_id = false,
        .colorize_output = true,
        .timestamp_format = NULL,
        .context_separator = "::"
    };
    
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);
    
    d_LogInfo("Starting Linked List Basic Function Tests");

    TEST_SUITE_START("Linked List Basic Tests");

    // Run all tests
    RUN_TEST(test_linked_list_init_destroy);
    RUN_TEST(test_linked_list_push_back);
    RUN_TEST(test_linked_list_push_front);
    RUN_TEST(test_linked_list_index_access);
    RUN_TEST(test_linked_list_find_by_name);
    RUN_TEST(test_linked_list_remove_by_index);
    RUN_TEST(test_linked_list_remove_by_name);
    RUN_TEST(test_linked_list_pop_back);
    RUN_TEST(test_linked_list_pop_front);
    RUN_TEST(test_linked_list_length);
    RUN_TEST(test_linked_list_different_types);
    RUN_TEST(test_linked_list_error_handling);

    TEST_SUITE_END();

    d_LogInfo("Linked List Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}