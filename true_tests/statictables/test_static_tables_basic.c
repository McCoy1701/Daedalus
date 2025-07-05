// File: true_tests/statictables/test_static_tables_basic.c - Basic static hash table functionality tests

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

int test_static_table_init_destroy(void)
{
    // Test basic initialization with fixed keys
    int keys[] = {1, 2, 3, 4, 5};
    int values[] = {10, 20, 30, 40, 50};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3], &keys[4]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3], &values[4]};
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 5);

    TEST_ASSERT(table != NULL, "Should create static hash table successfully");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(table) == 5, "New table should have 5 keys");

    // Test destroy (should not crash)
    int result = d_DestroyStaticTable(&table);
    TEST_ASSERT(result == 0, "Destroy should succeed");
    TEST_ASSERT(table == NULL, "Table pointer should be NULL after destroy");

    return 1;
}

int test_static_table_init_invalid_params(void)
{
    int keys[] = {1, 2, 3};
    int values[] = {10, 20, 30};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2]};

    // Test initialization with invalid parameters
    dStaticTable_t* table1 = d_InitStaticTable(0, sizeof(int), int_hash_func, int_compare_func, 8, key_ptrs, value_ptrs, 3);
    TEST_ASSERT(table1 == NULL, "Should fail with zero key size");

    dStaticTable_t* table2 = d_InitStaticTable(sizeof(int), 0, int_hash_func, int_compare_func, 8, key_ptrs, value_ptrs, 3);
    TEST_ASSERT(table2 == NULL, "Should fail with zero value size");

    dStaticTable_t* table3 = d_InitStaticTable(sizeof(int), sizeof(int), NULL, int_compare_func, 8, key_ptrs, value_ptrs, 3);
    TEST_ASSERT(table3 == NULL, "Should fail with NULL hash function");

    dStaticTable_t* table4 = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, NULL, 8, key_ptrs, value_ptrs, 3);
    TEST_ASSERT(table4 == NULL, "Should fail with NULL compare function");

    dStaticTable_t* table5 = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, int_compare_func, 0, key_ptrs, value_ptrs, 3);
    TEST_ASSERT(table5 == NULL, "Should fail with zero buckets");

    dStaticTable_t* table6 = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, int_compare_func, 8, NULL, value_ptrs, 3);
    TEST_ASSERT(table6 == NULL, "Should fail with NULL keys array");

    dStaticTable_t* table7 = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, int_compare_func, 8, key_ptrs, NULL, 3);
    TEST_ASSERT(table7 == NULL, "Should fail with NULL values array");

    dStaticTable_t* table8 = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, int_compare_func, 8, key_ptrs, value_ptrs, 0);
    TEST_ASSERT(table8 == NULL, "Should fail with zero num_keys");

    return 1;
}

int test_static_table_duplicate_keys_rejection(void)
{
    // Test with duplicate keys (should fail)
    int keys[] = {1, 2, 2, 3}; // Duplicate key "2"
    int values[] = {10, 20, 30, 40};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 4);

    TEST_ASSERT(table == NULL, "Should fail with duplicate keys during initialization");

    return 1;
}

int test_static_table_get_values_basic(void)
{
    // Create static table with known keys and values
    int keys[] = {42, 84, 126};
    int values[] = {100, 200, 300};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 3);

    TEST_ASSERT(table != NULL, "Table should be created successfully");

    // Test getting existing values
    int* retrieved1 = (int*)d_GetValueInStaticTable(table, &keys[0]);
    int* retrieved2 = (int*)d_GetValueInStaticTable(table, &keys[1]);
    int* retrieved3 = (int*)d_GetValueInStaticTable(table, &keys[2]);

    TEST_ASSERT(retrieved1 != NULL && *retrieved1 == values[0], "Should retrieve correct value for key 42");
    TEST_ASSERT(retrieved2 != NULL && *retrieved2 == values[1], "Should retrieve correct value for key 84");
    TEST_ASSERT(retrieved3 != NULL && *retrieved3 == values[2], "Should retrieve correct value for key 126");

    // Test getting non-existent key
    int nonexistent_key = 999;
    void* result = d_GetValueInStaticTable(table, &nonexistent_key);
    TEST_ASSERT(result == NULL, "Should return NULL for non-existent key");

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_set_values_existing_keys(void)
{
    // Create static table
    int keys[] = {1, 2, 3};
    int original_values[] = {10, 20, 30};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&original_values[0], &original_values[1], &original_values[2]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 3);

    TEST_ASSERT(table != NULL, "Table should be created successfully");

    // Update existing values
    int new_values[] = {100, 200, 300};
    TEST_ASSERT(d_SetValueInStaticTable(table, &keys[0], &new_values[0]) == 0, "Should update existing key 1");
    TEST_ASSERT(d_SetValueInStaticTable(table, &keys[1], &new_values[1]) == 0, "Should update existing key 2");
    TEST_ASSERT(d_SetValueInStaticTable(table, &keys[2], &new_values[2]) == 0, "Should update existing key 3");

    // Verify updated values
    int* retrieved1 = (int*)d_GetValueInStaticTable(table, &keys[0]);
    int* retrieved2 = (int*)d_GetValueInStaticTable(table, &keys[1]);
    int* retrieved3 = (int*)d_GetValueInStaticTable(table, &keys[2]);

    TEST_ASSERT(retrieved1 != NULL && *retrieved1 == new_values[0], "Should have updated value for key 1");
    TEST_ASSERT(retrieved2 != NULL && *retrieved2 == new_values[1], "Should have updated value for key 2");
    TEST_ASSERT(retrieved3 != NULL && *retrieved3 == new_values[2], "Should have updated value for key 3");

    // Verify key count remains the same
    TEST_ASSERT(d_GetKeyCountOfStaticTable(table) == 3, "Key count should remain unchanged after value updates");

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_set_values_nonexistent_keys(void)
{
    // Create static table
    int keys[] = {1, 2, 3};
    int values[] = {10, 20, 30};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 3);

    TEST_ASSERT(table != NULL, "Table should be created successfully");

    // Try to set value for non-existent key (should fail)
    int nonexistent_key = 999;
    int new_value = 777;
    TEST_ASSERT(d_SetValueInStaticTable(table, &nonexistent_key, &new_value) == 1, "Should fail to set value for non-existent key");

    // Verify table remains unchanged
    TEST_ASSERT(d_GetKeyCountOfStaticTable(table) == 3, "Key count should remain unchanged after failed set");

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_check_key_existence(void)
{
    // Create static table
    int keys[] = {10, 20, 30, 40};
    int values[] = {100, 200, 300, 400};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 4);

    TEST_ASSERT(table != NULL, "Table should be created successfully");

    // Test existence of known keys
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, &keys[0]) == 0, "Key 10 should exist");
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, &keys[1]) == 0, "Key 20 should exist");
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, &keys[2]) == 0, "Key 30 should exist");
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, &keys[3]) == 0, "Key 40 should exist");

    // Test non-existent key
    int nonexistent_key = 999;
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, &nonexistent_key) == 1, "Non-existent key should not be found");

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_get_all_keys(void)
{
    // Create static table
    int keys[] = {5, 15, 25, 35, 45};
    int values[] = {50, 150, 250, 350, 450};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3], &keys[4]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3], &values[4]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 5);

    TEST_ASSERT(table != NULL, "Table should be created successfully");

    // Get all keys
    dArray_t* all_keys = d_GetAllStaticTableKeys(table);
    TEST_ASSERT(all_keys != NULL, "Should return valid keys array");
    TEST_ASSERT(all_keys->count == 5, "Should have 5 keys in array");

    // Verify all expected keys are present (order may vary)
    for (int i = 0; i < 5; i++) {
        bool found = false;
        for (size_t j = 0; j < all_keys->count; j++) {
            int* key_in_array = (int*)d_IndexDataFromArray(all_keys, j);
            if (key_in_array && *key_in_array == keys[i]) {
                found = true;
                break;
            }
        }
        TEST_ASSERT(found, "Should find all original keys in returned array");
    }

    d_DestroyArray(all_keys);
    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_get_all_values(void)
{
    // Create static table
    int keys[] = {7, 14, 21, 28};
    int values[] = {77, 144, 211, 288};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 4);

    TEST_ASSERT(table != NULL, "Table should be created successfully");

    // Get all values
    dArray_t* all_values = d_GetAllStaticTableValues(table);
    TEST_ASSERT(all_values != NULL, "Should return valid values array");
    TEST_ASSERT(all_values->count == 4, "Should have 4 values in array");

    // Verify all expected values are present (order may vary)
    for (int i = 0; i < 4; i++) {
        bool found = false;
        for (size_t j = 0; j < all_values->count; j++) {
            int* value_in_array = (int*)d_IndexDataFromArray(all_values, j);
            if (value_in_array && *value_in_array == values[i]) {
                found = true;
                break;
            }
        }
        TEST_ASSERT(found, "Should find all original values in returned array");
    }

    d_DestroyArray(all_values);
    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_string_keys(void)
{
    // Create static table with string keys
    char* keys[] = {"apple", "banana", "cherry"};
    int values[] = {1, 2, 3};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(char*), sizeof(int), string_hash_func, 
                                              string_compare_func, 8, key_ptrs, value_ptrs, 3);

    TEST_ASSERT(table != NULL, "Table should be created successfully with string keys");

    // Test getting values with string keys
    int* retrieved1 = (int*)d_GetValueInStaticTable(table, &keys[0]);
    int* retrieved2 = (int*)d_GetValueInStaticTable(table, &keys[1]);
    int* retrieved3 = (int*)d_GetValueInStaticTable(table, &keys[2]);

    TEST_ASSERT(retrieved1 != NULL && *retrieved1 == values[0], "Should retrieve correct value for 'apple'");
    TEST_ASSERT(retrieved2 != NULL && *retrieved2 == values[1], "Should retrieve correct value for 'banana'");
    TEST_ASSERT(retrieved3 != NULL && *retrieved3 == values[2], "Should retrieve correct value for 'cherry'");

    // Test key existence with string keys
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, &keys[0]) == 0, "Should find 'apple'");
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, &keys[1]) == 0, "Should find 'banana'");
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, &keys[2]) == 0, "Should find 'cherry'");

    char* nonexistent = "grape";
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, &nonexistent) == 1, "Should not find 'grape'");

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_collision_handling(void)
{
    // Create a small table to force collisions
    int keys[] = {1, 3, 5, 7, 9, 11, 13, 15}; // Many keys for only 2 buckets
    int values[] = {10, 30, 50, 70, 90, 110, 130, 150};
    const void* key_ptrs[8];
    const void* value_ptrs[8];
    
    for (int i = 0; i < 8; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 2, key_ptrs, value_ptrs, 8); // Only 2 buckets

    TEST_ASSERT(table != NULL, "Table should be created successfully despite collisions");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(table) == 8, "Should have all 8 entries despite collisions");

    // Verify all values can be retrieved despite collisions
    for (int i = 0; i < 8; i++) {
        int* retrieved = (int*)d_GetValueInStaticTable(table, &keys[i]);
        TEST_ASSERT(retrieved != NULL && *retrieved == values[i], "Should retrieve correct value despite collisions");
    }

    // Verify all keys can be found despite collisions
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT(d_CheckForKeyInStaticTable(table, &keys[i]) == 0, "Should find key despite collisions");
    }

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_error_handling(void)
{
    // Create a valid table for testing error conditions
    int keys[] = {1, 2, 3};
    int values[] = {10, 20, 30};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 3);

    int key = 1, value = 100;

    // Test operations with NULL table
    TEST_ASSERT(d_SetValueInStaticTable(NULL, &key, &value) == 1, "Set with NULL table should fail");
    TEST_ASSERT(d_GetValueInStaticTable(NULL, &key) == NULL, "Get with NULL table should return NULL");
    TEST_ASSERT(d_CheckForKeyInStaticTable(NULL, &key) == 1, "Check with NULL table should return not found");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(NULL) == 0, "Count with NULL table should return 0");
    TEST_ASSERT(d_GetAllStaticTableKeys(NULL) == NULL, "Get keys with NULL table should return NULL");
    TEST_ASSERT(d_GetAllStaticTableValues(NULL) == NULL, "Get values with NULL table should return NULL");

    // Test operations with NULL key/value
    TEST_ASSERT(d_SetValueInStaticTable(table, NULL, &value) == 1, "Set with NULL key should fail");
    TEST_ASSERT(d_SetValueInStaticTable(table, &key, NULL) == 1, "Set with NULL value should fail");
    TEST_ASSERT(d_GetValueInStaticTable(table, NULL) == NULL, "Get with NULL key should return NULL");
    TEST_ASSERT(d_CheckForKeyInStaticTable(table, NULL) == 1, "Check with NULL key should return not found");

    // Test destroy with NULL pointer
    TEST_ASSERT(d_DestroyStaticTable(NULL) == 1, "Destroy with NULL should fail");

    d_DestroyStaticTable(&table);
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
    
    d_LogInfo("Starting Static Hash Table Basic Function Tests");

    TEST_SUITE_START("Static Hash Table Basic Tests");

    // Run all 12 tests
    RUN_TEST(test_static_table_init_destroy);
    RUN_TEST(test_static_table_init_invalid_params);
    RUN_TEST(test_static_table_duplicate_keys_rejection);
    RUN_TEST(test_static_table_get_values_basic);
    RUN_TEST(test_static_table_set_values_existing_keys);
    RUN_TEST(test_static_table_set_values_nonexistent_keys);
    RUN_TEST(test_static_table_check_key_existence);
    RUN_TEST(test_static_table_get_all_keys);
    RUN_TEST(test_static_table_get_all_values);
    RUN_TEST(test_static_table_string_keys);
    RUN_TEST(test_static_table_collision_handling);
    RUN_TEST(test_static_table_error_handling);

    TEST_SUITE_END();

    d_LogInfo("Static Hash Table Basic Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}