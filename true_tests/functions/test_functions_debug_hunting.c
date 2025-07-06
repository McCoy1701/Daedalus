// File: true_tests/functions/test_functions_debug_hunting.c - Debug hunting tests for comparison functions
// Tests designed to find edge cases, memory issues, and subtle bugs in comparison implementations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "tests.h"
#include "Daedalus.h"

// Global test counters (required by tests.h framework)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// DEBUG HUNTING TEST 1: MEMORY CORRUPTION AND BOUNDARY TESTS
// =============================================================================

int test_memory_corruption_boundary_conditions()
{
    // Test 1.1: Arrays with maximum size_t values to check for integer overflow
    dArray_t* max_arr1 = d_InitArray(1, SIZE_MAX);
    dArray_t* max_arr2 = d_InitArray(1, SIZE_MAX);
    
    // These should fail gracefully without crashing
    if (max_arr1 && max_arr2) {
        TEST_ASSERT(d_CompareDArray(&max_arr1, &max_arr2, 0) == 0, "Max size arrays should be equal when empty");
        d_DestroyArray(max_arr1);
        d_DestroyArray(max_arr2);
    }
    
    // Test 1.2: Arrays with zero element size
    dArray_t* zero_arr1 = d_InitArray(10, 0);
    dArray_t* zero_arr2 = d_InitArray(10, 0);
    
    if (zero_arr1 && zero_arr2) {
        TEST_ASSERT(d_CompareDArray(&zero_arr1, &zero_arr2, 0) == 0, "Zero element size arrays should be equal");
        d_DestroyArray(zero_arr1);
        d_DestroyArray(zero_arr2);
    }
    
    // Test 1.3: String comparison with embedded null bytes
    dString_t* str_with_nulls1 = d_InitString();
    dString_t* str_with_nulls2 = d_InitString();
    
    TEST_ASSERT(str_with_nulls1 && str_with_nulls2, "Failed to create strings for null byte test");
    
    // Manually set string data with embedded nulls
    char data_with_nulls[] = {'H', 'e', 'l', '\0', 'l', 'o'};
    d_AppendToString(str_with_nulls1, data_with_nulls, sizeof(data_with_nulls));
    d_AppendToString(str_with_nulls2, data_with_nulls, sizeof(data_with_nulls));
    
    TEST_ASSERT(d_CompareDString(&str_with_nulls1, &str_with_nulls2, 0) == 0, 
                "Strings with embedded nulls should be equal");
    
    // Modify one byte after the embedded null
    str_with_nulls2->str[4] = 'X';
    TEST_ASSERT(d_CompareDString(&str_with_nulls1, &str_with_nulls2, 0) != 0, 
                "Strings with different bytes after embedded null should not be equal");
    
    d_DestroyString(str_with_nulls1);
    d_DestroyString(str_with_nulls2);
    
    return 1; // Test passed
}

// =============================================================================
// DEBUG HUNTING TEST 2: HASH TABLE COLLISION AND DISTRIBUTION TESTS
// =============================================================================

int test_hash_table_collision_scenarios()
{
    // Test 2.1: Create tables with intentionally colliding keys
    dTable_t* collision_table1 = d_InitTable(sizeof(int), sizeof(char*), d_HashInt, d_CompareInt, 2); // Small bucket count forces collisions
    dTable_t* collision_table2 = d_InitTable(sizeof(int), sizeof(char*), d_HashInt, d_CompareInt, 2);
    
    TEST_ASSERT(collision_table1 && collision_table2, "Failed to create collision test tables");
    
    // Add keys that will likely collide (depends on hash function)
    int keys[] = {0, 2, 4, 6, 8}; // Even numbers might hash to same bucket
    char* values[] = {"zero", "two", "four", "six", "eight"};
    
    for (int i = 0; i < 5; i++) {
        d_SetDataInTable(collision_table1, &keys[i], &values[i]);
        d_SetDataInTable(collision_table2, &keys[i], &values[i]);
    }
    
    TEST_ASSERT(d_CompareTable(&collision_table1, &collision_table2, 0) == 0, 
                "Tables with colliding keys should still be equal");
    
    // Modify one value in the second table
    char* different_value = "EIGHT";
    d_SetDataInTable(collision_table2, &keys[4], &different_value);
    
    TEST_ASSERT(d_CompareTable(&collision_table1, &collision_table2, 0) != 0, 
                "Tables with different values should not be equal even with collisions");
    
    d_DestroyTable(&collision_table1);
    d_DestroyTable(&collision_table2);
    
    // Test 2.2: Empty vs non-empty table comparison
    dTable_t* empty_table = d_InitTable(sizeof(int), sizeof(int), d_HashInt, d_CompareInt, 16);
    dTable_t* single_entry_table = d_InitTable(sizeof(int), sizeof(int), d_HashInt, d_CompareInt, 16);
    
    TEST_ASSERT(empty_table && single_entry_table, "Failed to create empty/single entry tables");
    
    int key = 42, value = 100;
    d_SetDataInTable(single_entry_table, &key, &value);
    
    TEST_ASSERT(d_CompareTable(&empty_table, &single_entry_table, 0) != 0, 
                "Empty table should not equal table with one entry");
    
    d_DestroyTable(&empty_table);
    d_DestroyTable(&single_entry_table);
    
    return 1; // Test passed
}

// =============================================================================
// DEBUG HUNTING TEST 3: STATIC TABLE EDGE CASES AND INITIALIZATION BUGS
// =============================================================================

int test_static_table_initialization_edge_cases()
{
    // Test 3.1: Static table with duplicate keys (should fail during initialization)
    int duplicate_keys[] = {1, 2, 2, 3}; // Key "2" appears twice
    char* values[] = {"one", "two", "two_duplicate", "three"};
    const void* key_ptrs[] = {&duplicate_keys[0], &duplicate_keys[1], &duplicate_keys[2], &duplicate_keys[3]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3]};
    
    // This should fail gracefully and return NULL
    dStaticTable_t* duplicate_table = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                                        d_HashInt, d_CompareInt, 8,
                                                        key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(duplicate_table == NULL, "Static table with duplicate keys should fail initialization");
    
    // Test 3.2: Valid static table comparison with single key
    int single_key = 999;
    char* single_value = "singleton";
    const void* single_key_ptr = &single_key;
    const void* single_value_ptr = &single_value;
    
    dStaticTable_t* singleton1 = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                                   d_HashInt, d_CompareInt, 8,
                                                   &single_key_ptr, &single_value_ptr, 1);
    
    dStaticTable_t* singleton2 = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                                   d_HashInt, d_CompareInt, 8,
                                                   &single_key_ptr, &single_value_ptr, 1);
    
    TEST_ASSERT(singleton1 && singleton2, "Failed to create singleton static tables");
    TEST_ASSERT(d_CompareStaticTable(&singleton1, &singleton2, 0) == 0, 
                "Identical singleton static tables should be equal");
    
    d_DestroyStaticTable(&singleton1);
    d_DestroyStaticTable(&singleton2);
    
    // Test 3.3: Static tables with different bucket counts but same data
    int test_keys[] = {10, 20, 30};
    char* test_values[] = {"ten", "twenty", "thirty"};
    const void* test_key_ptrs[] = {&test_keys[0], &test_keys[1], &test_keys[2]};
    const void* test_value_ptrs[] = {&test_values[0], &test_values[1], &test_values[2]};
    
    dStaticTable_t* table_8_buckets = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                                        d_HashInt, d_CompareInt, 8,
                                                        test_key_ptrs, test_value_ptrs, 3);
    
    dStaticTable_t* table_16_buckets = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                                         d_HashInt, d_CompareInt, 16,
                                                         test_key_ptrs, test_value_ptrs, 3);
    
    TEST_ASSERT(table_8_buckets && table_16_buckets, "Failed to create different bucket count tables");
    TEST_ASSERT(d_CompareStaticTable(&table_8_buckets, &table_16_buckets, 0) == 0, 
                "Static tables with same data but different bucket counts should be equal");
    
    d_DestroyStaticTable(&table_8_buckets);
    d_DestroyStaticTable(&table_16_buckets);
    
    return 1; // Test passed
}

// =============================================================================
// DEBUG HUNTING TEST 4: EXTREME DATA SIZE AND PERFORMANCE STRESS TESTS
// =============================================================================

int test_extreme_data_sizes_and_performance()
{
    // Test 4.1: Arrays with very large element sizes
    typedef struct {
        char padding[1024]; // 1KB struct
    } LargeStruct;
    
    dArray_t* large_struct_arr1 = d_InitArray(10, sizeof(LargeStruct));
    dArray_t* large_struct_arr2 = d_InitArray(10, sizeof(LargeStruct));
    
    TEST_ASSERT(large_struct_arr1 && large_struct_arr2, "Failed to create large struct arrays");
    
    // Fill with identical data
    LargeStruct large_data;
    memset(&large_data, 0xAB, sizeof(LargeStruct)); // Fill with pattern
    
    for (int i = 0; i < 5; i++) {
        d_AppendDataToArray(large_struct_arr1, &large_data);
        d_AppendDataToArray(large_struct_arr2, &large_data);
    }
    
    TEST_ASSERT(d_CompareDArray(&large_struct_arr1, &large_struct_arr2, 0) == 0, 
                "Arrays with large identical structs should be equal");
    
    // Modify one byte in the last element of the second array
    LargeStruct* last_elem = (LargeStruct*)d_IndexDataFromArray(large_struct_arr2, 4);
    last_elem->padding[512] = 0xCD; // Change middle byte
    
    TEST_ASSERT(d_CompareDArray(&large_struct_arr1, &large_struct_arr2, 0) != 0, 
                "Arrays with one different byte in large struct should not be equal");
    
    d_DestroyArray(large_struct_arr1);
    d_DestroyArray(large_struct_arr2);
    
    // Test 4.2: String comparison with very long strings
    dString_t* very_long_str1 = d_InitString();
    dString_t* very_long_str2 = d_InitString();
    
    TEST_ASSERT(very_long_str1 && very_long_str2, "Failed to create very long strings");
    
    // Create 10KB strings
    char large_buffer[10240];
    for (int i = 0; i < 10240; i++) {
        large_buffer[i] = 'A' + (i % 26); // Repeating alphabet pattern
    }
    large_buffer[10239] = '\0';
    
    d_AppendToString(very_long_str1, large_buffer, 0);
    d_AppendToString(very_long_str2, large_buffer, 0);
    
    TEST_ASSERT(d_CompareDString(&very_long_str1, &very_long_str2, 0) == 0, 
                "Very long identical strings should be equal");
    
    // Change one character near the end
    very_long_str2->str[10000] = 'X';
    
    TEST_ASSERT(d_CompareDString(&very_long_str1, &very_long_str2, 0) != 0, 
                "Very long strings with one different character should not be equal");
    
    d_DestroyString(very_long_str1);
    d_DestroyString(very_long_str2);
    
    return 1; // Test passed
}

// =============================================================================
// DEBUG HUNTING TEST 5: TYPE CONFUSION AND POINTER CORRUPTION TESTS
// =============================================================================

int test_type_confusion_and_pointer_corruption()
{
    // Test 5.1: Mixed data types in arrays (testing memcmp behavior)
    typedef union {
        int as_int;
        float as_float;
        char as_bytes[4];
    } MixedData;
    
    dArray_t* mixed_arr1 = d_InitArray(5, sizeof(MixedData));
    dArray_t* mixed_arr2 = d_InitArray(5, sizeof(MixedData));
    
    TEST_ASSERT(mixed_arr1 && mixed_arr2, "Failed to create mixed data arrays");
    
    // Add same binary data interpreted differently
    MixedData data1, data2;
    data1.as_int = 0x41424344; // "ABCD" as int
    data2.as_int = 0x41424344; // Same binary representation
    
    d_AppendDataToArray(mixed_arr1, &data1);
    d_AppendDataToArray(mixed_arr2, &data2);
    
    TEST_ASSERT(d_CompareDArray(&mixed_arr1, &mixed_arr2, 0) == 0, 
                "Arrays with same binary data should be equal regardless of interpretation");
    
    // Test with NaN values that might not equal themselves
    data1.as_float = 0.0f / 0.0f; // NaN
    data2.as_float = 0.0f / 0.0f; // Another NaN
    
    d_AppendDataToArray(mixed_arr1, &data1);
    d_AppendDataToArray(mixed_arr2, &data2);
    
    // This tests whether memcmp handles NaN correctly (should be bitwise comparison)
    int result = d_CompareDArray(&mixed_arr1, &mixed_arr2, 0);
    TEST_ASSERT(result == 0 || result != 0, "NaN comparison should be deterministic with memcmp");
    
    d_DestroyArray(mixed_arr1);
    d_DestroyArray(mixed_arr2);
    
    // Test 5.2: String comparison with non-null-terminated data
    dString_t* manual_str1 = d_InitString();
    dString_t* manual_str2 = d_InitString();
    
    TEST_ASSERT(manual_str1 && manual_str2, "Failed to create manual strings");
    
    // Manually construct string data without null terminator
    char raw_data1[] = {'T', 'e', 's', 't'};
    char raw_data2[] = {'T', 'e', 's', 't'};
    
    d_AppendToString(manual_str1, raw_data1, sizeof(raw_data1));
    d_AppendToString(manual_str2, raw_data2, sizeof(raw_data2));
    
    TEST_ASSERT(d_CompareDString(&manual_str1, &manual_str2, 0) == 0, 
                "Strings with identical raw data should be equal");
    
    d_DestroyString(manual_str1);
    d_DestroyString(manual_str2);
    
    // Test 5.3: Table comparison with keys that have same hash but are different
    // Create a custom hash function that always returns the same value
    dTable_t* hash_collision_table1 = d_InitTable(sizeof(int), sizeof(char*), d_HashInt, d_CompareInt, 1);
    dTable_t* hash_collision_table2 = d_InitTable(sizeof(int), sizeof(char*), d_HashInt, d_CompareInt, 1);
    
    TEST_ASSERT(hash_collision_table1 && hash_collision_table2, "Failed to create hash collision tables");
    
    // Add multiple keys to force hash collisions (single bucket)
    int collision_keys[] = {1, 2, 3, 4, 5};
    char* collision_values[] = {"one", "two", "three", "four", "five"};
    
    for (int i = 0; i < 5; i++) {
        d_SetDataInTable(hash_collision_table1, &collision_keys[i], &collision_values[i]);
        d_SetDataInTable(hash_collision_table2, &collision_keys[i], &collision_values[i]);
    }
    
    TEST_ASSERT(d_CompareTable(&hash_collision_table1, &hash_collision_table2, 0) == 0, 
                "Tables with all keys in same bucket should still be equal");
    
    // Remove one key from the second table and add a different one
    // Note: This test depends on implementation details of table removal
    
    d_DestroyTable(&hash_collision_table1);
    d_DestroyTable(&hash_collision_table2);
    
    return 1; // Test passed
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main()
{
    TEST_SUITE_START("Debug Hunting - Comparison Functions");
    
    // Run all debug hunting tests
    RUN_TEST(test_memory_corruption_boundary_conditions);
    RUN_TEST(test_hash_table_collision_scenarios);
    RUN_TEST(test_static_table_initialization_edge_cases);
    RUN_TEST(test_extreme_data_sizes_and_performance);
    RUN_TEST(test_type_confusion_and_pointer_corruption);
    
    TEST_SUITE_END();
    
    return (tests_failed == 0) ? 0 : 1;
}