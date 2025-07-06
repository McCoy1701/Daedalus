// File: true_tests/statictables/test_static_tables_advanced.c - Advanced static hash table functionality tests

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

int test_static_table_clear_functionality(void)
{
    // Create static table with some data
    int keys[] = {1, 2, 3, 4, 5};
    int values[] = {10, 20, 30, 40, 50};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3], &keys[4]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3], &values[4]};
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 5);

    TEST_ASSERT(table != NULL, "Should create static table successfully");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(table) == 5, "Should have 5 keys initially");

    // Clear the table
    TEST_ASSERT(d_ClearStaticTable(table) == 0, "Clear operation should succeed");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(table) == 0, "Should have 0 keys after clear");

    // Verify all keys are gone
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(d_CheckForKeyInStaticTable(table, &keys[i]) == 1, "All keys should be gone after clear");
    }

    // Test getting arrays from cleared table
    dArray_t* keys_array = d_GetAllKeysFromStaticTable(table);
    dArray_t* values_array = d_GetAllValuesFromStaticTable(table);
    TEST_ASSERT(keys_array == NULL, "Should return NULL for keys from uninitialized table");
    TEST_ASSERT(values_array == NULL, "Should return NULL for values from uninitialized table");

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_clear_null_parameter(void)
{
    // Test with NULL table
    TEST_ASSERT(d_ClearStaticTable(NULL) == 1, "Should fail with NULL table parameter");

    return 1;
}

int test_static_table_stats_basic(void)
{
    // Create static table with controlled distribution
    int keys[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int values[] = {10, 20, 30, 40, 50, 60, 70, 80};
    const void* key_ptrs[8];
    const void* value_ptrs[8];
    
    for (int i = 0; i < 8; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 4, key_ptrs, value_ptrs, 8); // 4 buckets for 8 keys

    TEST_ASSERT(table != NULL, "Should create static table successfully");

    // Get statistics
    size_t min_entries, max_entries, empty_buckets;
    float avg_entries;
    
    TEST_ASSERT(d_GetStaticTableStats(table, &min_entries, &max_entries, &avg_entries, &empty_buckets) == 0, 
                "Should get stats successfully");

    // Verify basic constraints
    TEST_ASSERT(min_entries <= max_entries, "Min should be <= max");
    TEST_ASSERT(avg_entries > 0.0f, "Average should be positive");
    TEST_ASSERT(empty_buckets <= 4, "Empty buckets should be <= total buckets");
    
    // With 8 entries and 4 buckets, average should be 2.0
    TEST_ASSERT(avg_entries == 2.0f, "Average should be exactly 2.0 with 8 entries in 4 buckets");

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_stats_null_parameters(void)
{
    // Create a valid table
    int keys[] = {1, 2, 3};
    int values[] = {10, 20, 30};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2]};
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 3);

    // Test with all NULL output parameters (should not crash)
    TEST_ASSERT(d_GetStaticTableStats(table, NULL, NULL, NULL, NULL) == 0, 
                "Should handle NULL output parameters gracefully");

    // Test with NULL table
    size_t dummy;
    float dummy_float;
    TEST_ASSERT(d_GetStaticTableStats(NULL, &dummy, &dummy, &dummy_float, &dummy) == 1, 
                "Should fail with NULL table");

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_rebucket_basic(void)
{
    // Create static table with known data
    int keys[] = {10, 20, 30, 40, 50};
    int values[] = {100, 200, 300, 400, 500};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3], &keys[4]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3], &values[4]};
    
    dStaticTable_t* original = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                                  int_compare_func, 4, key_ptrs, value_ptrs, 5);

    TEST_ASSERT(original != NULL, "Should create original table successfully");

    // Rebucket to different size
    dStaticTable_t* rebucketed = d_RebucketStaticTable(original, 16);
    TEST_ASSERT(rebucketed != NULL, "Should create rebucketed table successfully");

    // Verify both tables have same key count
    TEST_ASSERT(d_GetKeyCountOfStaticTable(original) == d_GetKeyCountOfStaticTable(rebucketed), 
                "Both tables should have same key count");

    // Verify all data is preserved in rebucketed table
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(d_CheckForKeyInStaticTable(rebucketed, &keys[i]) == 0, "All keys should exist in rebucketed table");
        
        int* value = (int*)d_GetValueInStaticTable(rebucketed, &keys[i]);
        TEST_ASSERT(value != NULL && *value == values[i], "All values should be preserved in rebucketed table");
    }

    // Verify original table is unchanged
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_GetValueInStaticTable(original, &keys[i]);
        TEST_ASSERT(value != NULL && *value == values[i], "Original table should be unchanged");
    }

    d_DestroyStaticTable(&original);
    d_DestroyStaticTable(&rebucketed);
    return 1;
}

int test_static_table_rebucket_invalid_parameters(void)
{
    // Create a valid table
    int keys[] = {1, 2, 3};
    int values[] = {10, 20, 30};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2]};
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 3);

    // Test with NULL table
    dStaticTable_t* result1 = d_RebucketStaticTable(NULL, 16);
    TEST_ASSERT(result1 == NULL, "Should return NULL for NULL table parameter");

    // Test with zero buckets
    dStaticTable_t* result2 = d_RebucketStaticTable(table, 0);
    TEST_ASSERT(result2 == NULL, "Should return NULL for zero buckets");

    d_DestroyStaticTable(&table);
    return 1;
}

int test_static_table_string_keys_advanced_operations(void)
{
    // Create static table with string keys
    char* keys[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int values[] = {1, 2, 3, 4, 5};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3], &keys[4]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3], &values[4]};

    dStaticTable_t* table = d_InitStaticTable(sizeof(char*), sizeof(int), string_hash_func, 
                                              string_compare_func, 4, key_ptrs, value_ptrs, 5);

    TEST_ASSERT(table != NULL, "Should create static table with string keys successfully");

    // Test statistics with string keys
    size_t min_entries, max_entries, empty_buckets;
    float avg_entries;
    
    TEST_ASSERT(d_GetStaticTableStats(table, &min_entries, &max_entries, &avg_entries, &empty_buckets) == 0, 
                "Should get stats for string-keyed table");

    // Test rebucketing with string keys
    dStaticTable_t* rebucketed = d_RebucketStaticTable(table, 16);
    TEST_ASSERT(rebucketed != NULL, "Should successfully rebucket table with string keys");

    // Verify string keys still work after rebucketing
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_GetValueInStaticTable(rebucketed, &keys[i]);
        TEST_ASSERT(value != NULL && *value == values[i], "Should retrieve string-keyed values after rebucketing");
    }

    // Test getting all string keys from rebucketed table
    dArray_t* all_keys = d_GetAllKeysFromStaticTable(rebucketed);
    TEST_ASSERT(all_keys != NULL && all_keys->count == 5, "Should get all string keys from rebucketed table");

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
        TEST_ASSERT(found, "Should find all original string keys in rebucketed table");
    }

    d_DestroyArray(all_keys);
    d_DestroyStaticTable(&table);
    d_DestroyStaticTable(&rebucketed);
    return 1;
}

int test_static_table_large_dataset_advanced_operations(void)
{
    const size_t num_entries = 50;
    
    // Create large dataset
    int* keys = malloc(num_entries * sizeof(int));
    int* values = malloc(num_entries * sizeof(int));
    const void** key_ptrs = malloc(num_entries * sizeof(void*));
    const void** value_ptrs = malloc(num_entries * sizeof(void*));
    
    for (size_t i = 0; i < num_entries; i++) {
        keys[i] = (int)i;
        values[i] = (int)i * 2;
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, num_entries);

    TEST_ASSERT(table != NULL, "Should create large static table successfully");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(table) == num_entries, "Should have correct number of keys");

    // Test statistics on large dataset
    size_t min_entries, max_entries, empty_buckets;
    float avg_entries;
    
    TEST_ASSERT(d_GetStaticTableStats(table, &min_entries, &max_entries, &avg_entries, &empty_buckets) == 0, 
                "Should get stats for large dataset");
    
    TEST_ASSERT(avg_entries > 0.0f, "Average should be positive for large dataset");

    // Test rebucketing large dataset
    dStaticTable_t* rebucketed = d_RebucketStaticTable(table, 128);
    TEST_ASSERT(rebucketed != NULL, "Should successfully rebucket large dataset");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(rebucketed) == num_entries, "Should maintain key count after rebucketing");

    // Get all keys and values to verify integrity
    dArray_t* all_keys = d_GetAllKeysFromStaticTable(rebucketed);
    dArray_t* all_values = d_GetAllValuesFromStaticTable(rebucketed);
    
    TEST_ASSERT(all_keys != NULL, "Should get keys array from large rebucketed dataset");
    TEST_ASSERT(all_keys->count == num_entries, "Should have correct number of keys");
    TEST_ASSERT(all_values != NULL, "Should get values array from large rebucketed dataset");
    TEST_ASSERT(all_values->count == num_entries, "Should have correct number of values");

    // Verify data integrity after rebucketing
    for (size_t i = 0; i < num_entries; i++) {
        int key = (int)i;
        int* value = (int*)d_GetValueInStaticTable(rebucketed, &key);
        TEST_ASSERT(value != NULL && *value == (int)i * 2, "Should maintain data integrity in large rebucketed dataset");
    }

    // Cleanup
    d_DestroyArray(all_keys);
    d_DestroyArray(all_values);
    d_DestroyStaticTable(&table);
    d_DestroyStaticTable(&rebucketed);
    free(keys);
    free(values);
    free(key_ptrs);
    free(value_ptrs);
    
    return 1;
}

int test_static_table_mixed_operations_stress_test(void)
{
    // Create initial table
    int keys[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int values[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    const void* key_ptrs[10];
    const void* value_ptrs[10];
    
    for (int i = 0; i < 10; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }

    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 4, key_ptrs, value_ptrs, 10);

    TEST_ASSERT(table != NULL, "Should create initial table successfully");

    // Phase 1: Get initial statistics
    size_t min1, max1, empty1;
    float avg1;
    TEST_ASSERT(d_GetStaticTableStats(table, &min1, &max1, &avg1, &empty1) == 0, 
                "Should get initial stats");

    // Phase 2: Update some values
    for (int i = 0; i < 10; i += 2) {
        int new_value = values[i] * 10;
        TEST_ASSERT(d_SetValueInStaticTable(table, &keys[i], &new_value) == 0, 
                    "Should update values successfully");
    }

    // Phase 3: Rebucket to larger size
    dStaticTable_t* rebucketed = d_RebucketStaticTable(table, 32);
    TEST_ASSERT(rebucketed != NULL, "Should rebucket successfully in stress test");

    // Phase 4: Get statistics from rebucketed table
    size_t min2, max2, empty2;
    float avg2;
    TEST_ASSERT(d_GetStaticTableStats(rebucketed, &min2, &max2, &avg2, &empty2) == 0, 
                "Should get rebucketed stats");

    // Phase 5: Verify all keys and updated values
    dArray_t* final_keys = d_GetAllKeysFromStaticTable(rebucketed);
    dArray_t* final_values = d_GetAllValuesFromStaticTable(rebucketed);
    
    TEST_ASSERT(final_keys != NULL && final_keys->count == 10, "Should get all keys in stress test");
    TEST_ASSERT(final_values != NULL && final_values->count == 10, "Should get all values in stress test");

    // Phase 6: Verify updated values are correct
    for (int i = 0; i < 10; i++) {
        int* value = (int*)d_GetValueInStaticTable(rebucketed, &keys[i]);
        if (i % 2 == 0) {
            // Even indices were updated
            TEST_ASSERT(value != NULL && *value == values[i] * 10, "Should have updated values for even keys");
        } else {
            // Odd indices should have original values
            TEST_ASSERT(value != NULL && *value == values[i], "Should have original values for odd keys");
        }
    }

    // Phase 7: Clear and verify empty state
    TEST_ASSERT(d_ClearStaticTable(rebucketed) == 0, "Should clear table successfully");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(rebucketed) == 0, "Should have no keys after clear");

    // Cleanup
    d_DestroyArray(final_keys);
    d_DestroyArray(final_values);
    d_DestroyStaticTable(&table);
    d_DestroyStaticTable(&rebucketed);
    
    return 1;
}

int test_static_table_performance_comparison(void)
{
    // Create two identical tables with different bucket counts
    const size_t num_keys = 20;
    int* keys = malloc(num_keys * sizeof(int));
    int* values = malloc(num_keys * sizeof(int));
    const void** key_ptrs = malloc(num_keys * sizeof(void*));
    const void** value_ptrs = malloc(num_keys * sizeof(void*));
    
    for (size_t i = 0; i < num_keys; i++) {
        keys[i] = (int)i;
        values[i] = (int)i * 5;
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }

    // Create table with poor bucket ratio (many collisions expected)
    dStaticTable_t* crowded = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                                 int_compare_func, 2, key_ptrs, value_ptrs, num_keys);

    // Create table with good bucket ratio (fewer collisions expected)
    dStaticTable_t* spacious = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                                  int_compare_func, 32, key_ptrs, value_ptrs, num_keys);

    TEST_ASSERT(crowded != NULL && spacious != NULL, "Should create both tables successfully");

    // Get statistics for both tables
    size_t min_crowded, max_crowded, empty_crowded;
    size_t min_spacious, max_spacious, empty_spacious;
    float avg_crowded, avg_spacious;
    
    TEST_ASSERT(d_GetStaticTableStats(crowded, &min_crowded, &max_crowded, &avg_crowded, &empty_crowded) == 0, 
                "Should get crowded table stats");
    TEST_ASSERT(d_GetStaticTableStats(spacious, &min_spacious, &max_spacious, &avg_spacious, &empty_spacious) == 0, 
                "Should get spacious table stats");

    // Spacious table should have more empty buckets and better distribution
    TEST_ASSERT(empty_spacious > empty_crowded, "Spacious table should have more empty buckets");
    TEST_ASSERT(max_spacious <= max_crowded, "Spacious table should have better max distribution");

    // Both tables should contain the same data
    for (size_t i = 0; i < num_keys; i++) {
        int* val_crowded = (int*)d_GetValueInStaticTable(crowded, &keys[i]);
        int* val_spacious = (int*)d_GetValueInStaticTable(spacious, &keys[i]);
        
        TEST_ASSERT(val_crowded != NULL && *val_crowded == values[i], "Crowded table should have correct values");
        TEST_ASSERT(val_spacious != NULL && *val_spacious == values[i], "Spacious table should have correct values");
        TEST_ASSERT(*val_crowded == *val_spacious, "Both tables should have identical values");
    }

    // Cleanup
    d_DestroyStaticTable(&crowded);
    d_DestroyStaticTable(&spacious);
    free(keys);
    free(values);
    free(key_ptrs);
    free(value_ptrs);
    
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
    
    d_LogInfo("Starting Static Hash Table Advanced Function Tests");

    TEST_SUITE_START("Static Hash Table Advanced Tests");

    // Run all advanced tests
    RUN_TEST(test_static_table_clear_functionality);
    RUN_TEST(test_static_table_clear_null_parameter);
    RUN_TEST(test_static_table_stats_basic);
    RUN_TEST(test_static_table_stats_null_parameters);
    RUN_TEST(test_static_table_rebucket_basic);
    RUN_TEST(test_static_table_rebucket_invalid_parameters);
    RUN_TEST(test_static_table_string_keys_advanced_operations);
    RUN_TEST(test_static_table_large_dataset_advanced_operations);
    RUN_TEST(test_static_table_mixed_operations_stress_test);
    RUN_TEST(test_static_table_performance_comparison);

    TEST_SUITE_END();

    d_LogInfo("Static Hash Table Advanced Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}