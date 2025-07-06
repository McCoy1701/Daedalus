// File: true_tests/tables/test_tables_advanced.c - Advanced hash table functionality tests

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

int test_rehash_basic_functionality(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 4);

    // Add some entries to a small table
    for (int i = 0; i < 8; i++) {
        int key = i, value = i * 10;
        d_SetDataInTable(table, &key, &value);
    }

    TEST_ASSERT(d_GetCountInTable(table) == 8, "Should have 8 entries before rehash");
    
    // Rehash to larger size
    TEST_ASSERT(d_RehashTable(table, 0) == 0, "Rehash should succeed");
    TEST_ASSERT(d_GetCountInTable(table) == 8, "Should still have 8 entries after rehash");

    // Verify all entries are still accessible
    for (int i = 0; i < 8; i++) {
        int key = i;
        int* value = (int*)d_GetDataFromTable(table, &key);
        TEST_ASSERT(value != NULL && *value == i * 10, "Should retrieve correct value after rehash");
    }

    d_DestroyTable(&table);
    return 1;
}

int test_rehash_auto_sizing(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    // Add entries
    for (int i = 0; i < 5; i++) {
        int key = i, value = i * 100;
        d_SetDataInTable(table, &key, &value);
    }

    // Auto-resize (pass 0 for new_num_buckets)
    TEST_ASSERT(d_RehashTable(table, 0) == 0, "Auto-resize should succeed");
    TEST_ASSERT(d_GetCountInTable(table) == 5, "Should maintain entry count after auto-resize");

    // Verify entries still work
    for (int i = 0; i < 5; i++) {
        int key = i;
        int* value = (int*)d_GetDataFromTable(table, &key);
        TEST_ASSERT(value != NULL && *value == i * 100, "Should retrieve correct value after auto-resize");
    }

    d_DestroyTable(&table);
    return 1;
}

int test_rehash_invalid_parameters(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    // Test with NULL table
    TEST_ASSERT(d_RehashTable(NULL, 16) == 1, "Should fail with NULL table");

    // Test with same size (should fail)
    TEST_ASSERT(d_RehashTable(table, 8) == 1, "Should fail when not growing");

    // Test with smaller size (should fail)
    TEST_ASSERT(d_RehashTable(table, 4) == 1, "Should fail when shrinking");

    d_DestroyTable(&table);
    return 1;
}

int test_get_all_keys_basic(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    // Add some entries
    int keys_to_add[] = {10, 20, 30, 40, 50};
    int values[] = {100, 200, 300, 400, 500};
    
    for (int i = 0; i < 5; i++) {
        d_SetDataInTable(table, &keys_to_add[i], &values[i]);
    }

    // Get all keys
    dArray_t* all_keys = d_GetAllKeysFromTable(table);
    TEST_ASSERT(all_keys != NULL, "Should return valid keys array");
    TEST_ASSERT(all_keys->count == 5, "Should have 5 keys in array");

    // Verify all expected keys are present (order may vary)
    for (int i = 0; i < 5; i++) {
        bool found = false;
        for (size_t j = 0; j < all_keys->count; j++) {
            int* key_in_array = (int*)d_IndexDataFromArray(all_keys, j);
            if (key_in_array && *key_in_array == keys_to_add[i]) {
                found = true;
                break;
            }
        }
        TEST_ASSERT(found, "Should find all original keys in returned array");
    }

    d_DestroyArray(all_keys);
    d_DestroyTable(&table);
    return 1;
}

int test_get_all_keys_empty_table(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    // Get keys from empty table
    dArray_t* all_keys = d_GetAllKeysFromTable(table);
    TEST_ASSERT(all_keys != NULL, "Should return valid array even for empty table");
    TEST_ASSERT(all_keys->count == 0, "Should have 0 keys for empty table");

    d_DestroyArray(all_keys);
    d_DestroyTable(&table);
    return 1;
}

int test_get_all_keys_null_parameter(void)
{
    // Test with NULL table
    dArray_t* result = d_GetAllKeysFromTable(NULL);
    TEST_ASSERT(result == NULL, "Should return NULL for NULL table parameter");

    return 1;
}

int test_get_all_values_basic(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    // Add some entries
    int keys[] = {1, 2, 3, 4};
    int values_to_add[] = {101, 202, 303, 404};
    
    for (int i = 0; i < 4; i++) {
        d_SetDataInTable(table, &keys[i], &values_to_add[i]);
    }

    // Get all values
    dArray_t* all_values = d_GetAllValuesFromTable(table);
    TEST_ASSERT(all_values != NULL, "Should return valid values array");
    TEST_ASSERT(all_values->count == 4, "Should have 4 values in array");

    // Verify all expected values are present (order may vary)
    for (int i = 0; i < 4; i++) {
        bool found = false;
        for (size_t j = 0; j < all_values->count; j++) {
            int* value_in_array = (int*)d_IndexDataFromArray(all_values, j);
            if (value_in_array && *value_in_array == values_to_add[i]) {
                found = true;
                break;
            }
        }
        TEST_ASSERT(found, "Should find all original values in returned array");
    }

    d_DestroyArray(all_values);
    d_DestroyTable(&table);
    return 1;
}

int test_get_all_values_empty_table(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    // Get values from empty table
    dArray_t* all_values = d_GetAllValuesFromTable(table);
    TEST_ASSERT(all_values != NULL, "Should return valid array even for empty table");
    TEST_ASSERT(all_values->count == 0, "Should have 0 values for empty table");

    d_DestroyArray(all_values);
    d_DestroyTable(&table);
    return 1;
}

int test_get_all_values_null_parameter(void)
{
    // Test with NULL table
    dArray_t* result = d_GetAllValuesFromTable(NULL);
    TEST_ASSERT(result == NULL, "Should return NULL for NULL table parameter");

    return 1;
}

int test_string_keys_advanced_operations(void)
{
    dTable_t* table = d_InitTable(sizeof(char*), sizeof(int), string_hash_func, 
                                  string_compare_func, 4);

    // Add string entries
    char* keys[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int values[] = {1, 2, 3, 4, 5};
    
    for (int i = 0; i < 5; i++) {
        d_SetDataInTable(table, &keys[i], &values[i]);
    }

    // Test rehashing with string keys
    TEST_ASSERT(d_RehashTable(table, 16) == 0, "Should successfully rehash table with string keys");
    TEST_ASSERT(d_GetCountInTable(table) == 5, "Should maintain count after rehash");

    // Verify string keys still work after rehash
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_GetDataFromTable(table, &keys[i]);
        TEST_ASSERT(value != NULL && *value == values[i], "Should retrieve string-keyed values after rehash");
    }

    // Test getting all string keys
    dArray_t* all_keys = d_GetAllKeysFromTable(table);
    TEST_ASSERT(all_keys != NULL && all_keys->count == 5, "Should get all string keys");

    // Verify we can find all original keys
    for (int i = 0; i < 5; i++) {
        bool found = false;
        for (size_t j = 0; j < all_keys->count; j++) {
            char** key_ptr = (char**)d_IndexDataFromArray(all_keys, j);
            if (key_ptr && strcmp(*key_ptr, keys[i]) == 0) {
                found = true;
                break;
            }
        }
        TEST_ASSERT(found, "Should find all original string keys");
    }

    d_DestroyArray(all_keys);
    d_DestroyTable(&table);
    return 1;
}

int test_large_dataset_advanced_operations(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 8);

    const size_t num_entries = 50;

    // Add large dataset
    LOOP_TEST_START();
    for (size_t i = 0; i < num_entries; i++) {
        int key = (int)i, value = (int)i * 2;
        TEST_ASSERT(d_SetDataInTable(table, &key, &value) == 0, "Should add entry to large dataset");
    }
    LOOP_TEST_END();

    // Test rehashing large dataset
    TEST_ASSERT(d_RehashTable(table, 0) == 0, "Should successfully rehash large dataset");
    TEST_ASSERT(d_GetCountInTable(table) == num_entries, "Should maintain count after large rehash");

    // Get all keys and verify count
    dArray_t* all_keys = d_GetAllKeysFromTable(table);
    TEST_ASSERT(all_keys != NULL, "Should get keys array from large dataset");
    TEST_ASSERT(all_keys->count == num_entries, "Should have correct number of keys");

    // Get all values and verify count  
    dArray_t* all_values = d_GetAllValuesFromTable(table);
    TEST_ASSERT(all_values != NULL, "Should get values array from large dataset");
    TEST_ASSERT(all_values->count == num_entries, "Should have correct number of values");

    // Verify data integrity after all operations
    for (size_t i = 0; i < num_entries; i++) {
        int key = (int)i;
        int* value = (int*)d_GetDataFromTable(table, &key);
        TEST_ASSERT(value != NULL && *value == (int)i * 2, "Should maintain data integrity in large dataset");
    }

    d_DestroyArray(all_keys);
    d_DestroyArray(all_values);
    d_DestroyTable(&table);
    return 1;
}

int test_mixed_operations_stress_test(void)
{
    dTable_t* table = d_InitTable(sizeof(int), sizeof(int), int_hash_func, 
                                  int_compare_func, 4);

    // Phase 1: Add initial data
    for (int i = 0; i < 10; i++) {
        int key = i, value = i * 10;
        d_SetDataInTable(table, &key, &value);
    }

    // Phase 2: Rehash
    TEST_ASSERT(d_RehashTable(table, 32) == 0, "Should rehash successfully in stress test");

    // Phase 3: Add more data after rehash
    for (int i = 10; i < 20; i++) {
        int key = i, value = i * 10;
        d_SetDataInTable(table, &key, &value);
    }

    // Phase 4: Get all keys and values
    dArray_t* keys = d_GetAllKeysFromTable(table);
    dArray_t* values = d_GetAllValuesFromTable(table);
    
    TEST_ASSERT(keys != NULL && keys->count == 20, "Should get all keys in stress test");
    TEST_ASSERT(values != NULL && values->count == 20, "Should get all values in stress test");

    // Phase 5: Remove some entries
    for (int i = 0; i < 20; i += 2) {
        int key = i;
        d_RemoveDataFromTable(table, &key);
    }

    // Phase 6: Final verification
    TEST_ASSERT(d_GetCountInTable(table) == 10, "Should have 10 entries after removals");
    
    // Verify remaining entries
    for (int i = 1; i < 20; i += 2) {
        int key = i;
        int* value = (int*)d_GetDataFromTable(table, &key);
        TEST_ASSERT(value != NULL && *value == i * 10, "Should maintain odd-numbered entries");
    }

    d_DestroyArray(keys);
    d_DestroyArray(values);
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
    
    d_LogInfo("Starting Hash Table Advanced Function Tests");

    TEST_SUITE_START("Hash Table Advanced Tests");

    // Run all tests
    RUN_TEST(test_rehash_basic_functionality);
    RUN_TEST(test_rehash_auto_sizing);
    RUN_TEST(test_rehash_invalid_parameters);
    RUN_TEST(test_get_all_keys_basic);
    RUN_TEST(test_get_all_keys_empty_table);
    RUN_TEST(test_get_all_keys_null_parameter);
    RUN_TEST(test_get_all_values_basic);
    RUN_TEST(test_get_all_values_empty_table);
    RUN_TEST(test_get_all_values_null_parameter);
    RUN_TEST(test_string_keys_advanced_operations);
    RUN_TEST(test_large_dataset_advanced_operations);
    RUN_TEST(test_mixed_operations_stress_test);

    TEST_SUITE_END();

    d_LogInfo("Hash Table Advanced Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}