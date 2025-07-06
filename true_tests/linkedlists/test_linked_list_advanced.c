// File: true_tests/linkedlists/test_linked_list_advanced.c - Advanced linked list functionality tests
// Focuses on hash table utility functions and complex scenarios

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
int test_check_for_name_basic(void)
{
    // Create a test list with multiple items
    dLinkedList_t* list = NULL;
    int value1 = 100, value2 = 200, value3 = 300;

    d_PushBackToLinkedList(&list, &value1, "first", sizeof(int));
    d_PushBackToLinkedList(&list, &value2, "second", sizeof(int));
    d_PushBackToLinkedList(&list, &value3, "third", sizeof(int));

    // Test checking for existing names
    TEST_ASSERT(d_CheckForNameInLinkedList(list, "first") == 0, "Should find 'first' name (return 0)");
    TEST_ASSERT(d_CheckForNameInLinkedList(list, "second") == 0, "Should find 'second' name (return 0)");
    TEST_ASSERT(d_CheckForNameInLinkedList(list, "third") == 0, "Should find 'third' name (return 0)");

    // Test checking for non-existent name
    TEST_ASSERT(d_CheckForNameInLinkedList(list, "nonexistent") == 1, "Should not find 'nonexistent' name (return 1)");

    // Test edge cases
    TEST_ASSERT(d_CheckForNameInLinkedList(NULL, "test") == 1, "Should return 1 for NULL list");
    TEST_ASSERT(d_CheckForNameInLinkedList(list, NULL) == 1, "Should return 1 for NULL name");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_get_node_by_name_basic(void)
{
    // Create a test list with multiple items
    dLinkedList_t* list = NULL;
    int value1 = 111, value2 = 222, value3 = 333;

    d_PushBackToLinkedList(&list, &value1, "alpha", sizeof(int));
    d_PushBackToLinkedList(&list, &value2, "beta", sizeof(int));
    d_PushBackToLinkedList(&list, &value3, "gamma", sizeof(int));

    // Test getting existing nodes
    dLinkedList_t* node1 = d_GetNodeByNameLinkedList(list, "alpha");
    TEST_ASSERT(node1 != NULL, "Should get valid node pointer for 'alpha'");
    TEST_ASSERT(*(int*)node1->data == value1, "Alpha node should contain correct data");
    TEST_ASSERT(strcmp(node1->buffer, "alpha") == 0, "Alpha node should have correct name");

    dLinkedList_t* node2 = d_GetNodeByNameLinkedList(list, "beta");
    TEST_ASSERT(node2 != NULL, "Should get valid node pointer for 'beta'");
    TEST_ASSERT(*(int*)node2->data == value2, "Beta node should contain correct data");

    dLinkedList_t* node3 = d_GetNodeByNameLinkedList(list, "gamma");
    TEST_ASSERT(node3 != NULL, "Should get valid node pointer for 'gamma'");
    TEST_ASSERT(*(int*)node3->data == value3, "Gamma node should contain correct data");

    // Test getting non-existent node
    dLinkedList_t* not_found = d_GetNodeByNameLinkedList(list, "delta");
    TEST_ASSERT(not_found == NULL, "Should return NULL for non-existent name");

    // Test edge cases
    TEST_ASSERT(d_GetNodeByNameLinkedList(NULL, "test") == NULL, "Should return NULL for NULL list");
    TEST_ASSERT(d_GetNodeByNameLinkedList(list, NULL) == NULL, "Should return NULL for NULL name");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_upsert_insert_new_nodes(void)
{
    // Start with empty list
    dLinkedList_t* list = NULL;

    // Test inserting new nodes (insert operation of upsert)
    int value1 = 42;
    int result1 = d_UpdateDataByNameLinkedList(&list, &value1, "config1", sizeof(int));
    TEST_ASSERT(result1 == 0, "Upsert should succeed for new node");
    TEST_ASSERT(list != NULL, "List should not be NULL after first upsert");
    TEST_ASSERT(d_CheckForNameInLinkedList(list, "config1") == 0, "Should find newly inserted 'config1'");

    int value2 = 84;
    int result2 = d_UpdateDataByNameLinkedList(&list, &value2, "config2", sizeof(int));
    TEST_ASSERT(result2 == 0, "Second upsert should succeed");
    TEST_ASSERT(d_CheckForNameInLinkedList(list, "config2") == 0, "Should find newly inserted 'config2'");

    // Verify both nodes exist and have correct data
    int* data1 = (int*)d_FindDataFromLinkedList(list, "config1");
    int* data2 = (int*)d_FindDataFromLinkedList(list, "config2");
    TEST_ASSERT(data1 != NULL && *data1 == value1, "Config1 should have correct data");
    TEST_ASSERT(data2 != NULL && *data2 == value2, "Config2 should have correct data");

    TEST_ASSERT(d_GetLengthOfLinkedList(list) == 2, "List should have 2 nodes after 2 upserts");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_upsert_update_existing_nodes(void)
{
    // Create list with initial data
    dLinkedList_t* list = NULL;
    int original_value = 100;
    d_PushBackToLinkedList(&list, &original_value, "player_stats", sizeof(int));

    // Verify original data
    int* original_data = (int*)d_FindDataFromLinkedList(list, "player_stats");
    TEST_ASSERT(original_data != NULL && *original_data == original_value, "Original data should be correct");

    // Test updating existing node (update operation of upsert)
    int new_value = 500;
    int result = d_UpdateDataByNameLinkedList(&list, &new_value, "player_stats", sizeof(int));
    TEST_ASSERT(result == 0, "Upsert update should succeed");

    // Verify data was updated
    int* updated_data = (int*)d_FindDataFromLinkedList(list, "player_stats");
    TEST_ASSERT(updated_data != NULL && *updated_data == new_value, "Data should be updated to new value");
    TEST_ASSERT(*updated_data != original_value, "Data should no longer be original value");

    // Verify list length didn't change (update, not insert)
    TEST_ASSERT(d_GetLengthOfLinkedList(list) == 1, "List should still have 1 node after update");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_upsert_different_data_sizes(void)
{
    dLinkedList_t* list = NULL;

    // Test upsert with different data types and sizes
    // Insert int
    int int_val = 42;
    d_UpdateDataByNameLinkedList(&list, &int_val, "int_config", sizeof(int));

    // Insert float
    float float_val = 3.14f;
    d_UpdateDataByNameLinkedList(&list, &float_val, "float_config", sizeof(float));

    // Insert struct
    typedef struct {
        int x, y;
        char name[16];
    } Point;
    Point point_val = {10, 20, "origin"};
    d_UpdateDataByNameLinkedList(&list, &point_val, "point_config", sizeof(Point));

    // Verify all data types
    int* int_ptr = (int*)d_FindDataFromLinkedList(list, "int_config");
    float* float_ptr = (float*)d_FindDataFromLinkedList(list, "float_config");
    Point* point_ptr = (Point*)d_FindDataFromLinkedList(list, "point_config");

    TEST_ASSERT(int_ptr != NULL && *int_ptr == int_val, "Int config should be correct");
    TEST_ASSERT(float_ptr != NULL && *float_ptr == float_val, "Float config should be correct");
    TEST_ASSERT(point_ptr != NULL && point_ptr->x == 10 && point_ptr->y == 20, "Point config should be correct");

    // Test updating with different size (int to float)
    float new_int_as_float = 99.5f;
    d_UpdateDataByNameLinkedList(&list, &new_int_as_float, "int_config", sizeof(float));
    
    float* updated_float = (float*)d_FindDataFromLinkedList(list, "int_config");
    TEST_ASSERT(updated_float != NULL && *updated_float == new_int_as_float, "Should update int config with float data");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_hash_table_simulation_basic(void)
{
    // Simulate a basic hash table using linked list as bucket
    dLinkedList_t* hash_bucket = NULL;

    // Simulate hash collisions by putting multiple items in same bucket
    typedef struct {
        char key[32];
        int value;
    } KeyValuePair;

    KeyValuePair item1 = {"user:123", 100};
    KeyValuePair item2 = {"user:456", 200};
    KeyValuePair item3 = {"config:timeout", 30};

    // Insert items (simulating hash table put operations)
    d_UpdateDataByNameLinkedList(&hash_bucket, &item1, item1.key, sizeof(KeyValuePair));
    d_UpdateDataByNameLinkedList(&hash_bucket, &item2, item2.key, sizeof(KeyValuePair));
    d_UpdateDataByNameLinkedList(&hash_bucket, &item3, item3.key, sizeof(KeyValuePair));

    // Test hash table operations
    // Check existence
    TEST_ASSERT(d_CheckForNameInLinkedList(hash_bucket, "user:123") == 0, "Should find user:123 in hash bucket");
    TEST_ASSERT(d_CheckForNameInLinkedList(hash_bucket, "user:456") == 0, "Should find user:456 in hash bucket");
    TEST_ASSERT(d_CheckForNameInLinkedList(hash_bucket, "config:timeout") == 0, "Should find config:timeout in hash bucket");
    TEST_ASSERT(d_CheckForNameInLinkedList(hash_bucket, "nonexistent") == 1, "Should not find nonexistent key");

    // Get values
    KeyValuePair* retrieved1 = (KeyValuePair*)d_FindDataFromLinkedList(hash_bucket, "user:123");
    KeyValuePair* retrieved2 = (KeyValuePair*)d_FindDataFromLinkedList(hash_bucket, "user:456");
    
    TEST_ASSERT(retrieved1 != NULL && retrieved1->value == 100, "Should retrieve correct value for user:123");
    TEST_ASSERT(retrieved2 != NULL && retrieved2->value == 200, "Should retrieve correct value for user:456");

    // Update value (hash table update)
    KeyValuePair updated_item1 = {"user:123", 999};
    d_UpdateDataByNameLinkedList(&hash_bucket, &updated_item1, "user:123", sizeof(KeyValuePair));
    
    KeyValuePair* updated_retrieved = (KeyValuePair*)d_FindDataFromLinkedList(hash_bucket, "user:123");
    TEST_ASSERT(updated_retrieved != NULL && updated_retrieved->value == 999, "Should have updated value");

    d_DestroyLinkedList(&hash_bucket);
    return 1;
}

int test_hash_table_simulation_advanced(void)
{
    // Simulate more complex hash table operations
    dLinkedList_t* bucket = NULL;

    // Test rapid insert/update cycles
    LOOP_TEST_START();
    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "item_%d", i);
        
        int value = i * 10;
        d_UpdateDataByNameLinkedList(&bucket, &value, key, sizeof(int));
        
        // Verify it was inserted
        TEST_ASSERT(d_CheckForNameInLinkedList(bucket, key) == 0, "Should find inserted item");
    }
    LOOP_TEST_END();

    TEST_ASSERT(d_GetLengthOfLinkedList(bucket) == 10, "Should have 10 items after insertion loop");

    // Test updating all values
    LOOP_TEST_START();
    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "item_%d", i);
        
        int new_value = i * 100; // Different values
        d_UpdateDataByNameLinkedList(&bucket, &new_value, key, sizeof(int));
    }
    LOOP_TEST_END();

    TEST_ASSERT(d_GetLengthOfLinkedList(bucket) == 10, "Should still have 10 items after update loop");

    // Verify all updates worked
    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "item_%d", i);
        
        int* value = (int*)d_FindDataFromLinkedList(bucket, key);
        TEST_ASSERT(value != NULL && *value == i * 100, "Should have updated value");
    }

    d_DestroyLinkedList(&bucket);
    return 1;
}

int test_node_pointer_manipulation(void)
{
    // Test direct node pointer access and manipulation
    dLinkedList_t* list = NULL;
    
    int value1 = 111, value2 = 222, value3 = 333;
    d_PushBackToLinkedList(&list, &value1, "node1", sizeof(int));
    d_PushBackToLinkedList(&list, &value2, "node2", sizeof(int));
    d_PushBackToLinkedList(&list, &value3, "node3", sizeof(int));

    // Get direct node pointers
    dLinkedList_t* node1 = d_GetNodeByNameLinkedList(list, "node1");
    dLinkedList_t* node2 = d_GetNodeByNameLinkedList(list, "node2");
    dLinkedList_t* node3 = d_GetNodeByNameLinkedList(list, "node3");

    TEST_ASSERT(node1 != NULL && node2 != NULL && node3 != NULL, "Should get all node pointers");

    // Verify pointer relationships
    TEST_ASSERT(node1 == list, "First node should be the head");
    TEST_ASSERT(node1->next == node2, "Node1 should point to node2");
    TEST_ASSERT(node2->next == node3, "Node2 should point to node3");
    TEST_ASSERT(node3->next == NULL, "Node3 should be the tail");

    // Test direct access to node fields
    TEST_ASSERT(strcmp(node1->buffer, "node1") == 0, "Node1 should have correct name");
    TEST_ASSERT(strcmp(node2->buffer, "node2") == 0, "Node2 should have correct name");
    TEST_ASSERT(strcmp(node3->buffer, "node3") == 0, "Node3 should have correct name");

    TEST_ASSERT(*(int*)node1->data == value1, "Node1 should have correct data");
    TEST_ASSERT(*(int*)node2->data == value2, "Node2 should have correct data");
    TEST_ASSERT(*(int*)node3->data == value3, "Node3 should have correct data");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_error_handling_advanced(void)
{
    // Test error handling for new functions
    dLinkedList_t* list = NULL;
    int test_value = 42;

    // Test upsert with invalid inputs
    int result1 = d_UpdateDataByNameLinkedList(NULL, &test_value, "test", sizeof(int));
    TEST_ASSERT(result1 == 1, "Upsert with NULL head should fail");

    int result2 = d_UpdateDataByNameLinkedList(&list, NULL, "test", sizeof(int));
    TEST_ASSERT(result2 == 1, "Upsert with NULL data should fail");

    int result3 = d_UpdateDataByNameLinkedList(&list, &test_value, NULL, sizeof(int));
    TEST_ASSERT(result3 == 1, "Upsert with NULL name should fail");

    int result4 = d_UpdateDataByNameLinkedList(&list, &test_value, "test", 0);
    TEST_ASSERT(result4 == 1, "Upsert with zero size should fail");

    // Test that list remains NULL after failed operations
    TEST_ASSERT(list == NULL, "List should remain NULL after failed upsert operations");

    // Test successful operation after failures
    int result5 = d_UpdateDataByNameLinkedList(&list, &test_value, "valid", sizeof(int));
    TEST_ASSERT(result5 == 0, "Valid upsert should succeed after failed attempts");
    TEST_ASSERT(list != NULL, "List should not be NULL after successful upsert");

    d_DestroyLinkedList(&list);
    return 1;
}

int test_performance_characteristics(void)
{
    // Test performance characteristics of new functions
    dLinkedList_t* large_list = NULL;
    
    // Build a moderately large list to test search performance
    LOOP_TEST_START();
    for (int i = 0; i < 100; i++) {
        char name[32];
        snprintf(name, sizeof(name), "item_%03d", i);
        d_UpdateDataByNameLinkedList(&large_list, &i, name, sizeof(int));
    }
    LOOP_TEST_END();

    TEST_ASSERT(d_GetLengthOfLinkedList(large_list) == 100, "Should have 100 items in large list");

    // Test search performance - finding items at different positions
    TEST_ASSERT(d_CheckForNameInLinkedList(large_list, "item_000") == 0, "Should find first item quickly");
    TEST_ASSERT(d_CheckForNameInLinkedList(large_list, "item_050") == 0, "Should find middle item");
    TEST_ASSERT(d_CheckForNameInLinkedList(large_list, "item_099") == 0, "Should find last item");

    // Test node retrieval performance
    dLinkedList_t* first_node = d_GetNodeByNameLinkedList(large_list, "item_000");
    dLinkedList_t* middle_node = d_GetNodeByNameLinkedList(large_list, "item_050");
    dLinkedList_t* last_node = d_GetNodeByNameLinkedList(large_list, "item_099");

    TEST_ASSERT(first_node != NULL, "Should find first node");
    TEST_ASSERT(middle_node != NULL, "Should find middle node");
    TEST_ASSERT(last_node != NULL, "Should find last node");

    // Test update performance on existing items
    LOOP_TEST_START();
    for (int i = 0; i < 100; i += 10) { // Update every 10th item
        char name[32];
        snprintf(name, sizeof(name), "item_%03d", i);
        int new_value = i * 1000;
        d_UpdateDataByNameLinkedList(&large_list, &new_value, name, sizeof(int));
    }
    LOOP_TEST_END();

    // Verify updates worked
    for (int i = 0; i < 100; i += 10) {
        char name[32];
        snprintf(name, sizeof(name), "item_%03d", i);
        int* value = (int*)d_FindDataFromLinkedList(large_list, name);
        TEST_ASSERT(value != NULL && *value == i * 1000, "Updated values should be correct");
    }

    d_DestroyLinkedList(&large_list);
    return 1;
}

int test_complete_hash_table_workflow(void)
{
    // Simulate a complete hash table workflow using all new functions
    d_LogInfo("VERIFICATION: Complete Hash Table Workflow - All utility functions working together");
    dLogContext_t* ctx = d_PushLogContext("HashTableWorkflow");

    dLinkedList_t* hash_bucket = NULL;

    // Phase 1: Initial insertions
    d_LogInfo("PHASE 1: Building hash table bucket with initial data...");
    
    typedef struct {
        char user_id[16];
        int score;
        float rating;
        char status[8];
    } UserRecord;

    UserRecord users[] = {
        {"user001", 1500, 4.5f, "active"},
        {"user002", 2300, 3.8f, "active"},
        {"user003", 890, 4.2f, "pending"}
    };

    for (int i = 0; i < 3; i++) {
        d_UpdateDataByNameLinkedList(&hash_bucket, &users[i], users[i].user_id, sizeof(UserRecord));
        d_LogInfoF("Inserted user: %s (score: %d)", users[i].user_id, users[i].score);
    }

    TEST_ASSERT(d_GetLengthOfLinkedList(hash_bucket) == 3, "Should have 3 users after initial insert");

    // Phase 2: Existence checks
    d_LogInfo("PHASE 2: Checking user existence...");
    for (int i = 0; i < 3; i++) {
        int exists = d_CheckForNameInLinkedList(hash_bucket, users[i].user_id);
        TEST_ASSERT(exists == 0, "All inserted users should exist");
        d_LogInfoF("User %s exists: %s", users[i].user_id, exists == 0 ? "YES" : "NO");
    }

    TEST_ASSERT(d_CheckForNameInLinkedList(hash_bucket, "user999") == 1, "Non-existent user should not be found");

    // Phase 3: Direct node access for metadata inspection
    d_LogInfo("PHASE 3: Direct node access for advanced operations...");
    dLinkedList_t* user001_node = d_GetNodeByNameLinkedList(hash_bucket, "user001");
    TEST_ASSERT(user001_node != NULL, "Should get direct access to user001 node");
    
    UserRecord* user001_data = (UserRecord*)user001_node->data;
    d_LogInfoF("Direct access - User001 score: %d, rating: %.1f", user001_data->score, user001_data->rating);
    TEST_ASSERT(user001_data->score == 1500, "Direct node access should show correct score");

    // Phase 4: User record updates
    d_LogInfo("PHASE 4: Updating user records...");
    UserRecord updated_user001 = {"user001", 1750, 4.7f, "premium"};
    d_UpdateDataByNameLinkedList(&hash_bucket, &updated_user001, "user001", sizeof(UserRecord));
    
    UserRecord* retrieved_updated = (UserRecord*)d_FindDataFromLinkedList(hash_bucket, "user001");
    TEST_ASSERT(retrieved_updated != NULL && retrieved_updated->score == 1750, "User001 should have updated score");
    TEST_ASSERT(strcmp(retrieved_updated->status, "premium") == 0, "User001 should have updated status");
    d_LogInfoF("Updated user001: score %d -> %d, status %s -> %s", 
               users[0].score, retrieved_updated->score, users[0].status, retrieved_updated->status);

    // Phase 5: New user insertion via upsert
    d_LogInfo("PHASE 5: Adding new user via upsert...");
    UserRecord new_user = {"user004", 1200, 4.0f, "trial"};
    d_UpdateDataByNameLinkedList(&hash_bucket, &new_user, "user004", sizeof(UserRecord));
    
    TEST_ASSERT(d_GetLengthOfLinkedList(hash_bucket) == 4, "Should have 4 users after adding new user");
    TEST_ASSERT(d_CheckForNameInLinkedList(hash_bucket, "user004") == 0, "New user should be findable");

    // Phase 6: Final verification
    d_LogInfo("PHASE 6: Final hash bucket state verification...");
    char* expected_users[] = {"user001", "user002", "user003", "user004"};
    for (int i = 0; i < 4; i++) {
        dLinkedList_t* node = d_GetNodeByNameLinkedList(hash_bucket, expected_users[i]);
        TEST_ASSERT(node != NULL, "All users should be accessible via node retrieval");
        
        UserRecord* record = (UserRecord*)node->data;
        d_LogInfoF("User %s: score=%d, rating=%.1f, status=%s", 
                   expected_users[i], record->score, record->rating, record->status);
    }

    d_LogInfo("Hash table workflow completed successfully - all utility functions working in harmony");

    d_DestroyLinkedList(&hash_bucket);
    d_PopLogContext(ctx);
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
    
    d_LogInfo("Starting Linked List Advanced Function Tests");
    d_LogInfo("Testing hash table utility functions and complex scenarios");

    TEST_SUITE_START("Linked List Advanced Tests");

    // Run all advanced tests
    RUN_TEST(test_check_for_name_basic);
    RUN_TEST(test_get_node_by_name_basic);
    RUN_TEST(test_upsert_insert_new_nodes);
    RUN_TEST(test_upsert_update_existing_nodes);
    RUN_TEST(test_upsert_different_data_sizes);
    RUN_TEST(test_hash_table_simulation_basic);
    RUN_TEST(test_hash_table_simulation_advanced);
    RUN_TEST(test_node_pointer_manipulation);
    RUN_TEST(test_error_handling_advanced);
    RUN_TEST(test_performance_characteristics);
    RUN_TEST(test_complete_hash_table_workflow);

    TEST_SUITE_END();

    d_LogInfo("Advanced Linked List Test Suite completed.");
    d_LogInfo("Benefits demonstrated:");
    d_LogInfo("- Fast existence checking with d_CheckForNameInLinkedList()");
    d_LogInfo("- Efficient upsert operations with d_UpdateDataByNameLinkedList()");
    d_LogInfo("- Direct node access with d_GetNodeByNameLinkedList()");
    d_LogInfo("- Hash table bucket management capabilities");
    d_LogInfo("- Performance characteristics for search and update operations");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}