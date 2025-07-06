// File: true_tests/tables/test_tables_basic.c - Basic hash table functionality tests

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// HELPER FUNCTIONS FOR TESTING
// =============================================================================

// Simple integer hash function for testing
static size_t int_hash_func(const void* key, size_t key_size) {
    (void)key_size; // Unused parameter
    int* int_key = (int*)key;
    return (size_t)(*int_key * 2654435761U); // Knuth's multiplicative hash
}

// Simple integer comparison function for testing
static int int_compare_func(const void* key1, const void* key2, size_t key_size) {
    (void)key_size; // Unused parameter
    int* k1 = (int*)key1;
    int* k2 = (int*)key2;
    return (*k1 == *k2) ? 0 : 1;
}

// String hash function for testing (simple djb2)
static size_t string_hash_func(const void* key, size_t key_size) {
    (void)key_size; // Unused parameter
    const char* str = *(const char**)key;
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// String comparison function for testing
static int string_compare_func(const void* key1, const void* key2, size_t key_size) {
    (void)key_size; // Unused parameter
    const char* str1 = *(const char**)key1;
    const char* str2 = *(const char**)key2;
    return strcmp(str1, str2);
}

// =============================================================================
// TEST FUNCTIONS
// =============================================================================

int test_table_init_destroy(void)
{
    // Test basic initialization
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    TEST_ASSERT(table != NULL, "Should create hash table successfully");
    TEST_ASSERT(d_GetCountInTable(table) == 0, "New table should have count 0");

    // Test destroy (should not crash)
    int result = d_DestroyTable(&table);
    TEST_ASSERT(result == 0, "Destroy should succeed");
    TEST_ASSERT(table == NULL, "Table pointer should be NULL after destroy");

    return 1;
}

int test_table_init_invalid_params(void)
{
    // Test initialization with invalid parameters
    dTable_t* table1 = d_InitTable(0, sizeof(int), int_hash_func, int_compare_func, 8);
    TEST_ASSERT(table1 == NULL, "Should fail with zero key size");

    dTable_t* table2 = d_InitTable(sizeof(int), 0, int_hash_func, int_compare_func, 8);
    TEST_ASSERT(table2 == NULL, "Should fail with zero value size");

    dTable_t* table3 = d_InitTable(sizeof(int), sizeof(int), NULL, int_compare_func, 8);
    TEST_ASSERT(table3 == NULL, "Should fail with NULL hash function");

    dTable_t* table4 = d_InitTable(sizeof(int), sizeof(int), int_hash_func, NULL, 8);
    TEST_ASSERT(table4 == NULL, "Should fail with NULL compare function");

    dTable_t* table5 = d_InitTable(sizeof(int), sizeof(int), int_hash_func, int_compare_func, 0);
    TEST_ASSERT(table5 == NULL, "Should fail with zero buckets");

    return 1;
}

int test_table_set_get_basic(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    // Test setting and getting basic key-value pairs
    int key1 = 42, value1 = 100;
    int key2 = 84, value2 = 200;
    int key3 = 126, value3 = 300;

    // Set values
    TEST_ASSERT(d_SetDataInTable(table, &key1, &value1) == 0, "Should set first key-value pair");
    TEST_ASSERT(d_SetDataInTable(table, &key2, &value2) == 0, "Should set second key-value pair");
    TEST_ASSERT(d_SetDataInTable(table, &key3, &value3) == 0, "Should set third key-value pair");

    TEST_ASSERT(d_GetCountInTable(table) == 3, "Table should have 3 entries");

    // Get values
    int* retrieved1 = (int*)d_GetDataFromTable(table, &key1);
    int* retrieved2 = (int*)d_GetDataFromTable(table, &key2);
    int* retrieved3 = (int*)d_GetDataFromTable(table, &key3);

    TEST_ASSERT(retrieved1 != NULL && *retrieved1 == value1, "Should retrieve correct value for key1");
    TEST_ASSERT(retrieved2 != NULL && *retrieved2 == value2, "Should retrieve correct value for key2");
    TEST_ASSERT(retrieved3 != NULL && *retrieved3 == value3, "Should retrieve correct value for key3");

    d_DestroyTable(&table);
    return 1;
}

int test_table_update_existing_key(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    // Set initial value
    int key = 42, original_value = 100;
    d_SetDataInTable(table, &key, &original_value);
    TEST_ASSERT(d_GetCountInTable(table) == 1, "Should have 1 entry initially");

    // Update the value
    int new_value = 999;
    d_SetDataInTable(table, &key, &new_value);
    TEST_ASSERT(d_GetCountInTable(table) == 1, "Should still have 1 entry after update");

    // Verify the value was updated
    int* retrieved = (int*)d_GetDataFromTable(table, &key);
    TEST_ASSERT(retrieved != NULL && *retrieved == new_value, "Should retrieve updated value");
    TEST_ASSERT(*retrieved != original_value, "Should not have original value");

    d_DestroyTable(&table);
    return 1;
}

int test_table_check_key_existence(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    int key1 = 42, value1 = 100;
    int key2 = 84, value2 = 200;
    int nonexistent_key = 999;

    // Test existence on empty table
    TEST_ASSERT(d_CheckForKeyInTable(table, &key1) == 1, "Key should not exist in empty table");

    // Add keys
    d_SetDataInTable(table, &key1, &value1);
    d_SetDataInTable(table, &key2, &value2);

    // Test existence
    TEST_ASSERT(d_CheckForKeyInTable(table, &key1) == 0, "Key1 should exist");
    TEST_ASSERT(d_CheckForKeyInTable(table, &key2) == 0, "Key2 should exist");
    TEST_ASSERT(d_CheckForKeyInTable(table, &nonexistent_key) == 1, "Nonexistent key should not be found");

    d_DestroyTable(&table);
    return 1;
}

int test_table_remove_data(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    int key1 = 42, value1 = 100;
    int key2 = 84, value2 = 200;
    int key3 = 126, value3 = 300;

    // Add entries
    d_SetDataInTable(table, &key1, &value1);
    d_SetDataInTable(table, &key2, &value2);
    d_SetDataInTable(table, &key3, &value3);
    TEST_ASSERT(d_GetCountInTable(table) == 3, "Should have 3 entries initially");

    // Remove middle entry
    TEST_ASSERT(d_RemoveDataFromTable(table, &key2) == 0, "Should successfully remove key2");
    TEST_ASSERT(d_GetCountInTable(table) == 2, "Should have 2 entries after removal");

    // Verify key2 is gone but others remain
    TEST_ASSERT(d_CheckForKeyInTable(table, &key1) == 0, "Key1 should still exist");
    TEST_ASSERT(d_CheckForKeyInTable(table, &key2) == 1, "Key2 should no longer exist");
    TEST_ASSERT(d_CheckForKeyInTable(table, &key3) == 0, "Key3 should still exist");

    // Test removing nonexistent key
    int nonexistent_key = 999;
    TEST_ASSERT(d_RemoveDataFromTable(table, &nonexistent_key) == 1, "Should fail to remove nonexistent key");
    TEST_ASSERT(d_GetCountInTable(table) == 2, "Count should remain unchanged after failed removal");

    d_DestroyTable(&table);
    return 1;
}

int test_table_clear(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    // Add multiple entries
    for (int i = 0; i < 10; i++) {
        int key = i, value = i * 10;
        d_SetDataInTable(table, &key, &value);
    }
    TEST_ASSERT(d_GetCountInTable(table) == 10, "Should have 10 entries");

    // Clear the table
    TEST_ASSERT(d_ClearTable(table) == 0, "Clear operation should succeed");
    TEST_ASSERT(d_GetCountInTable(table) == 0, "Table should be empty after clear");

    // Verify all keys are gone
    for (int i = 0; i < 10; i++) {
        int key = i;
        TEST_ASSERT(d_CheckForKeyInTable(table, &key) == 1, "All keys should be gone after clear");
    }

    // Verify table can be reused
    int new_key = 999, new_value = 888;
    TEST_ASSERT(d_SetDataInTable(table, &new_key, &new_value) == 0, "Should be able to add new data after clear");
    TEST_ASSERT(d_GetCountInTable(table) == 1, "Should have 1 entry after reuse");

    d_DestroyTable(&table);
    return 1;
}

int test_table_string_keys(void)
{
    dTable_t* table = d_InitTable(sizeof(char*), sizeof(int), string_hash_func, 
                                  string_compare_func, 8);

    // Test with string keys
    char* key1 = "hello";
    char* key2 = "world";
    char* key3 = "testing";
    int value1 = 100, value2 = 200, value3 = 300;

    // Set values
    TEST_ASSERT(d_SetDataInTable(table, &key1, &value1) == 0, "Should set string key1");
    TEST_ASSERT(d_SetDataInTable(table, &key2, &value2) == 0, "Should set string key2");
    TEST_ASSERT(d_SetDataInTable(table, &key3, &value3) == 0, "Should set string key3");

    // Get values
    int* retrieved1 = (int*)d_GetDataFromTable(table, &key1);
    int* retrieved2 = (int*)d_GetDataFromTable(table, &key2);
    int* retrieved3 = (int*)d_GetDataFromTable(table, &key3);

    TEST_ASSERT(retrieved1 != NULL && *retrieved1 == value1, "Should retrieve correct value for 'hello'");
    TEST_ASSERT(retrieved2 != NULL && *retrieved2 == value2, "Should retrieve correct value for 'world'");
    TEST_ASSERT(retrieved3 != NULL && *retrieved3 == value3, "Should retrieve correct value for 'testing'");

    // Test existence
    TEST_ASSERT(d_CheckForKeyInTable(table, &key1) == 0, "Should find 'hello'");
    TEST_ASSERT(d_CheckForKeyInTable(table, &key2) == 0, "Should find 'world'");

    char* nonexistent = "nonexistent";
    TEST_ASSERT(d_CheckForKeyInTable(table, &nonexistent) == 1, "Should not find 'nonexistent'");

    d_DestroyTable(&table);
    return 1;
}

int test_table_struct_values(void)
{
    // Test with struct values
    typedef struct {
        int x, y;
        char name[16];
    } Point;

    dTable_t* table = d_InitTable(sizeof(int), sizeof(Point), int_hash_func, 
                                  int_compare_func, 8);

    int key1 = 1, key2 = 2;
    Point point1 = {10, 20, "origin"};
    Point point2 = {30, 40, "corner"};

    // Set struct values
    TEST_ASSERT(d_SetDataInTable(table, &key1, &point1) == 0, "Should set struct value1");
    TEST_ASSERT(d_SetDataInTable(table, &key2, &point2) == 0, "Should set struct value2");

    // Get struct values
    Point* retrieved1 = (Point*)d_GetDataFromTable(table, &key1);
    Point* retrieved2 = (Point*)d_GetDataFromTable(table, &key2);

    TEST_ASSERT(retrieved1 != NULL, "Should retrieve struct pointer1");
    TEST_ASSERT(retrieved1->x == 10 && retrieved1->y == 20, "Should have correct coordinates for point1");
    TEST_ASSERT(strcmp(retrieved1->name, "origin") == 0, "Should have correct name for point1");

    TEST_ASSERT(retrieved2 != NULL, "Should retrieve struct pointer2");
    TEST_ASSERT(retrieved2->x == 30 && retrieved2->y == 40, "Should have correct coordinates for point2");
    TEST_ASSERT(strcmp(retrieved2->name, "corner") == 0, "Should have correct name for point2");

    d_DestroyTable(&table);
    return 1;
}

int test_table_collision_handling(void)
{
    // Create a small table to force collisions
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 2); // Only 2 buckets

    // Add many keys that will likely collide
    int keys[] = {1, 3, 5, 7, 9, 11, 13, 15};
    int values[] = {10, 30, 50, 70, 90, 110, 130, 150};
    int num_pairs = 8;

    // Set all key-value pairs
    for (int i = 0; i < num_pairs; i++) {
        TEST_ASSERT(d_SetDataInTable(table, &keys[i], &values[i]) == 0, "Should set key-value pair despite collisions");
    }

    TEST_ASSERT(d_GetCountInTable(table) == num_pairs, "Should have all entries despite collisions");

    // Verify all can be retrieved
    for (int i = 0; i < num_pairs; i++) {
        int* retrieved = (int*)d_GetDataFromTable(table, &keys[i]);
        TEST_ASSERT(retrieved != NULL && *retrieved == values[i], "Should retrieve correct value despite collisions");
    }

    // Verify all can be found
    for (int i = 0; i < num_pairs; i++) {
        TEST_ASSERT(d_CheckForKeyInTable(table, &keys[i]) == 0, "Should find key despite collisions");
    }

    // Remove some entries and verify others remain
    TEST_ASSERT(d_RemoveDataFromTable(table, &keys[2]) == 0, "Should remove key despite collisions");
    TEST_ASSERT(d_RemoveDataFromTable(table, &keys[5]) == 0, "Should remove another key despite collisions");

    TEST_ASSERT(d_GetCountInTable(table) == num_pairs - 2, "Should have correct count after removals");
    TEST_ASSERT(d_CheckForKeyInTable(table, &keys[2]) == 1, "Removed key should not be found");
    TEST_ASSERT(d_CheckForKeyInTable(table, &keys[5]) == 1, "Another removed key should not be found");
    TEST_ASSERT(d_CheckForKeyInTable(table, &keys[0]) == 0, "Non-removed key should still be found");

    d_DestroyTable(&table);
    return 1;
}

int test_table_error_handling(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    int key = 42, value = 100;

    // Test operations with NULL table
    TEST_ASSERT(d_SetDataInTable(NULL, &key, &value) == 1, "Set with NULL table should fail");
    TEST_ASSERT(d_GetDataFromTable(NULL, &key) == NULL, "Get with NULL table should return NULL");
    TEST_ASSERT(d_RemoveDataFromTable(NULL, &key) == 1, "Remove with NULL table should fail");
    TEST_ASSERT(d_CheckForKeyInTable(NULL, &key) == 1, "Check with NULL table should return not found");
    TEST_ASSERT(d_GetCountInTable(NULL) == 0, "Count with NULL table should return 0");
    TEST_ASSERT(d_ClearTable(NULL) == 1, "Clear with NULL table should fail");

    // Test operations with NULL key/value
    TEST_ASSERT(d_SetDataInTable(table, NULL, &value) == 1, "Set with NULL key should fail");
    TEST_ASSERT(d_SetDataInTable(table, &key, NULL) == 1, "Set with NULL value should fail");
    TEST_ASSERT(d_GetDataFromTable(table, NULL) == NULL, "Get with NULL key should return NULL");
    TEST_ASSERT(d_RemoveDataFromTable(table, NULL) == 1, "Remove with NULL key should fail");
    TEST_ASSERT(d_CheckForKeyInTable(table, NULL) == 1, "Check with NULL key should return not found");

    // Test destroy with NULL pointer
    TEST_ASSERT(d_DestroyTable(NULL) == 1, "Destroy with NULL should fail");

    d_DestroyTable(&table);
    return 1;
}

int test_table_large_dataset(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 16);

    const int num_entries = 100;

    // Add large dataset
    LOOP_TEST_START();
    for (int i = 0; i < num_entries; i++) {
        int key = i, value = i * 10;
        TEST_ASSERT(d_SetDataInTable(table, &key, &value) == 0, "Should set entry in large dataset");
    }
    LOOP_TEST_END();

    TEST_ASSERT(d_GetCountInTable(table) == num_entries, "Should have all entries in large dataset");

    // Verify all entries
    for (int i = 0; i < num_entries; i++) {
        int key = i;
        TEST_ASSERT(d_CheckForKeyInTable(table, &key) == 0, "Should find all keys in large dataset");
        
        int* value = (int*)d_GetDataFromTable(table, &key);
        TEST_ASSERT(value != NULL && *value == i * 10, "Should have correct values in large dataset");
    }

    // Remove every other entry
    LOOP_TEST_START();
    for (int i = 0; i < num_entries; i += 2) {
        int key = i;
        TEST_ASSERT(d_RemoveDataFromTable(table, &key) == 0, "Should remove entries from large dataset");
    }
    LOOP_TEST_END();

    TEST_ASSERT(d_GetCountInTable(table) == num_entries / 2, "Should have half entries after bulk removal");

    // Verify removal pattern
    for (int i = 0; i < num_entries; i++) {
        int key = i;
        if (i % 2 == 0) {
            TEST_ASSERT(d_CheckForKeyInTable(table, &key) == 1, "Even keys should be removed");
        } else {
            TEST_ASSERT(d_CheckForKeyInTable(table, &key) == 0, "Odd keys should remain");
        }
    }

    d_DestroyTable(&table);
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
    
    d_LogInfo("Starting Hash Table Basic Function Tests");

    TEST_SUITE_START("Hash Table Basic Tests");

    // Run all tests
    RUN_TEST(test_table_init_destroy);
    RUN_TEST(test_table_init_invalid_params);
    RUN_TEST(test_table_set_get_basic);
    RUN_TEST(test_table_update_existing_key);
    RUN_TEST(test_table_check_key_existence);
    RUN_TEST(test_table_remove_data);
    RUN_TEST(test_table_clear);
    RUN_TEST(test_table_string_keys);
    RUN_TEST(test_table_struct_values);
    RUN_TEST(test_table_collision_handling);
    RUN_TEST(test_table_error_handling);
    RUN_TEST(test_table_large_dataset);

    TEST_SUITE_END();

    d_LogInfo("Hash Table Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}