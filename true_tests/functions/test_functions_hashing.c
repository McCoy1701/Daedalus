// File: true_tests/functions/test_functions_hashing.c - Tests for built-in Daedalus hash and comparison functions

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// TEST DATA STRUCTURES AND HELPERS
// =============================================================================

// Test structure for binary hashing
typedef struct {
    int x;
    int y;
    char name[16];
} TestStruct_t;

// Helper to check hash distribution quality
static void check_hash_distribution(size_t* hashes, size_t count, const char* hash_name) {
    if (count == 0) return;
    
    // Count unique hashes
    size_t unique_count = 0;
    for (size_t i = 0; i < count; i++) {
        bool is_unique = true;
        for (size_t j = 0; j < i; j++) {
            if (hashes[i] == hashes[j]) {
                is_unique = false;
                break;
            }
        }
        if (is_unique) unique_count++;
    }
    
    float uniqueness_ratio = (float)unique_count / (float)count;
    d_LogInfoF("📊 %s Hash Distribution: %zu/%zu unique (%.1f%% uniqueness)", 
               hash_name, unique_count, count, uniqueness_ratio * 100.0f);
    
    // Good hash functions should have high uniqueness for diverse inputs
    if (uniqueness_ratio >= 0.9f) {
        d_LogInfo("✅ EXCELLENT hash distribution");
    } else if (uniqueness_ratio >= 0.7f) {
        d_LogInfo("✅ GOOD hash distribution");
    } else {
        d_LogWarning("⚠️  POOR hash distribution - consider different hash function");
    }
}

// =============================================================================
// INTEGER HASH FUNCTION TESTS
// =============================================================================

int test_hash_int_basic_functionality(void)
{
    // Test basic integer hashing
    int keys[] = {0, 1, -1, 42, -42, 1000, -1000, INT_MAX, INT_MIN};
    size_t num_keys = sizeof(keys) / sizeof(keys[0]);
    size_t hashes[9];
    
    d_LogInfo("\n=== 🔢 INTEGER HASH FUNCTION TEST ===");
    
    // Generate hashes
    for (size_t i = 0; i < num_keys; i++) {
        hashes[i] = d_HashInt(&keys[i], sizeof(int));
        d_LogInfoF("d_HashInt(%d) = %zu", keys[i], hashes[i]);
    }
    
    // Test determinism
    for (size_t i = 0; i < num_keys; i++) {
        size_t hash2 = d_HashInt(&keys[i], sizeof(int));
        TEST_ASSERT(hashes[i] == hash2, "Hash function should be deterministic");
    }
    
    // Test that different inputs generally produce different outputs
    check_hash_distribution(hashes, num_keys, "INTEGER");
    
    // Test NULL safety
    size_t null_hash = d_HashInt(NULL, sizeof(int));
    TEST_ASSERT(null_hash == 0, "NULL key should hash to 0");
    
    return 1;
}

int test_hash_int_with_hash_table(void)
{
    // Create hash table using built-in integer functions
    dTable_t* table = d_InitTable(sizeof(int), sizeof(char*), d_HashInt, d_CompareInt, 16);
    TEST_ASSERT(table != NULL, "Should create table with built-in int functions");
    
    // Test data
    int keys[] = {1, 10, 100, 1000, 10000};
    char* values[] = {"one", "ten", "hundred", "thousand", "ten thousand"};
    size_t num_entries = sizeof(keys) / sizeof(keys[0]);
    
    d_LogInfo("\n=== 🗂️  INTEGER HASH TABLE INTEGRATION TEST ===");
    
    // Insert all key-value pairs
    for (size_t i = 0; i < num_entries; i++) {
        TEST_ASSERT(d_SetDataInTable(table, &keys[i], &values[i]) == 0, 
                   "Should insert integer key successfully");
    }
    
    // Verify all lookups work
    for (size_t i = 0; i < num_entries; i++) {
        char** found_value = (char**)d_GetDataFromTable(table, &keys[i]);
        TEST_ASSERT(found_value != NULL, "Should find integer key");
        TEST_ASSERT(strcmp(*found_value, values[i]) == 0, "Should retrieve correct value");
        d_LogInfoF("✅ Key %d -> Value '%s'", keys[i], *found_value);
    }
    
    // Test key existence checks
    for (size_t i = 0; i < num_entries; i++) {
        TEST_ASSERT(d_CheckForKeyInTable(table, &keys[i]) == 0, "Should confirm key exists");
    }
    
    // Test non-existent key
    int missing_key = 99999;
    TEST_ASSERT(d_CheckForKeyInTable(table, &missing_key) == 1, "Should confirm key doesn't exist");
    
    d_LogInfoF("📊 Table contains %zu entries", d_GetCountInTable(table));
    
    d_DestroyTable(&table);
    return 1;
}

// =============================================================================
// STRING HASH FUNCTION TESTS
// =============================================================================

int test_hash_string_basic_functionality(void)
{
    // Test string hashing
    char* strings[] = {"hello", "world", "test", "", "a", "very long string for testing"};
    char** string_ptrs[] = {&strings[0], &strings[1], &strings[2], &strings[3], &strings[4], &strings[5]};
    size_t num_strings = sizeof(strings) / sizeof(strings[0]);
    size_t hashes[6];
    
    d_LogInfo("\n=== 📝 STRING HASH FUNCTION TEST ===");
    
    // Generate hashes
    for (size_t i = 0; i < num_strings; i++) {
        hashes[i] = d_HashString(&string_ptrs[i], 0);
        d_LogInfoF("d_HashString(\"%s\") = %zu", strings[i], hashes[i]);
    }
    
    // Test determinism
    for (size_t i = 0; i < num_strings; i++) {
        size_t hash2 = d_HashString(&string_ptrs[i], 0);
        TEST_ASSERT(hashes[i] == hash2, "String hash should be deterministic");
    }
    
    check_hash_distribution(hashes, num_strings, "STRING");
    
    // Test NULL safety
    char* null_str = NULL;
    char** null_ptr = &null_str;
    size_t null_hash = d_HashString(null_ptr, 0);
    TEST_ASSERT(null_hash == 0, "NULL string should hash to 0");
    
    size_t null_key_hash = d_HashString(NULL, 0);
    TEST_ASSERT(null_key_hash == 0, "NULL key should hash to 0");
    
    return 1;
}

int test_hash_string_literal_functionality(void)
{
    // Test string literal hashing (direct char* keys)
    char* literals[] = {"apple", "banana", "cherry", "date", "elderberry"};
    size_t num_literals = sizeof(literals) / sizeof(literals[0]);
    size_t hashes[5];
    
    d_LogInfo("\n=== 📝 STRING LITERAL HASH FUNCTION TEST ===");
    
    // Generate hashes
    for (size_t i = 0; i < num_literals; i++) {
        hashes[i] = d_HashStringLiteral(literals[i], 0);
        d_LogInfoF("d_HashStringLiteral(\"%s\") = %zu", literals[i], hashes[i]);
    }
    
    // Test with specified length
    char* test_str = "testing123";
    size_t full_hash = d_HashStringLiteral(test_str, 0);
    size_t partial_hash = d_HashStringLiteral(test_str, 4); // Just "test"
    d_LogInfoF("Full string hash: %zu, Partial (4 chars): %zu", full_hash, partial_hash);
    TEST_ASSERT(full_hash != partial_hash, "Full and partial hashes should differ");
    
    check_hash_distribution(hashes, num_literals, "STRING LITERAL");
    
    return 1;
}

int test_hash_string_case_insensitive(void)
{
    d_LogInfo("\n=== 🔤 CASE-INSENSITIVE STRING HASH TEST ===");
    
    // Test case-insensitive hashing
    char* mixed_case[] = {"Hello", "HELLO", "hello", "HeLLo"};
    char** ptrs[] = {&mixed_case[0], &mixed_case[1], &mixed_case[2], &mixed_case[3]};
    
    size_t hash1 = d_HashStringCaseInsensitive(ptrs[0], 0);
    size_t hash2 = d_HashStringCaseInsensitive(ptrs[1], 0);
    size_t hash3 = d_HashStringCaseInsensitive(ptrs[2], 0);
    size_t hash4 = d_HashStringCaseInsensitive(ptrs[3], 0);
    
    d_LogInfoF("\"Hello\" -> %zu", hash1);
    d_LogInfoF("\"HELLO\" -> %zu", hash2);
    d_LogInfoF("\"hello\" -> %zu", hash3);
    d_LogInfoF("\"HeLLo\" -> %zu", hash4);
    
    TEST_ASSERT(hash1 == hash2 && hash2 == hash3 && hash3 == hash4, 
               "Case-insensitive hash should be same for all variations");
    
    // Test comparison function too
    TEST_ASSERT(d_CompareStringCaseInsensitive(ptrs[0], ptrs[1], 0) == 0, 
               "Case-insensitive compare should match different cases");
    
    return 1;
}

int test_hash_string_with_hash_table(void)
{
    // Create hash table using built-in string functions
    dTable_t* table = d_InitTable(sizeof(char*), sizeof(int), d_HashString, d_CompareString, 16);
    TEST_ASSERT(table != NULL, "Should create table with built-in string functions");
    
    // Test data
    char* keys[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int values[] = {1, 2, 3, 4, 5};
    size_t num_entries = sizeof(keys) / sizeof(keys[0]);
    
    d_LogInfo("\n=== 🗂️  STRING HASH TABLE INTEGRATION TEST ===");
    
    // Insert all key-value pairs
    for (size_t i = 0; i < num_entries; i++) {
        TEST_ASSERT(d_SetDataInTable(table, &keys[i], &values[i]) == 0, 
                   "Should insert string key successfully");
    }
    
    // Verify all lookups work
    for (size_t i = 0; i < num_entries; i++) {
        int* found_value = (int*)d_GetDataFromTable(table, &keys[i]);
        TEST_ASSERT(found_value != NULL, "Should find string key");
        TEST_ASSERT(*found_value == values[i], "Should retrieve correct value");
        d_LogInfoF("✅ Key \"%s\" -> Value %d", keys[i], *found_value);
    }
    
    d_DestroyTable(&table);
    return 1;
}

// =============================================================================
// FLOATING-POINT HASH FUNCTION TESTS
// =============================================================================

int test_hash_float_functionality(void)
{
    float test_floats[] = {0.0f, 1.0f, -1.0f, 3.14159f, -2.71828f, 
                          1000.5f, -1000.5f, 0.00001f, 999999.9f};
    size_t num_floats = sizeof(test_floats) / sizeof(test_floats[0]);
    size_t hashes[9];
    
    d_LogInfo("\n=== 🔢 FLOAT HASH FUNCTION TEST ===");
    
    // Generate hashes
    for (size_t i = 0; i < num_floats; i++) {
        hashes[i] = d_HashFloat(&test_floats[i], sizeof(float));
        d_LogInfoF("d_HashFloat(%.5f) = %zu", test_floats[i], hashes[i]);
    }
    
    // Test determinism
    for (size_t i = 0; i < num_floats; i++) {
        size_t hash2 = d_HashFloat(&test_floats[i], sizeof(float));
        TEST_ASSERT(hashes[i] == hash2, "Float hash should be deterministic");
    }
    
    // Test special case: +0.0 and -0.0 should hash the same
    float pos_zero = 0.0f;
    float neg_zero = -0.0f;
    size_t pos_hash = d_HashFloat(&pos_zero, sizeof(float));
    size_t neg_hash = d_HashFloat(&neg_zero, sizeof(float));
    TEST_ASSERT(pos_hash == neg_hash, "+0.0 and -0.0 should hash the same");
    
    check_hash_distribution(hashes, num_floats, "FLOAT");
    
    return 1;
}

int test_hash_double_functionality(void)
{
    double test_doubles[] = {0.0, 1.0, -1.0, 3.141592653589793, -2.718281828459045, 
                            1000.123456789, -1000.123456789, 1e-10, 1e10};
    size_t num_doubles = sizeof(test_doubles) / sizeof(test_doubles[0]);
    size_t hashes[9];
    
    d_LogInfo("\n=== 🔢 DOUBLE HASH FUNCTION TEST ===");
    
    // Generate hashes
    for (size_t i = 0; i < num_doubles; i++) {
        hashes[i] = d_HashDouble(&test_doubles[i], sizeof(double));
        d_LogInfoF("d_HashDouble(%.10f) = %zu", test_doubles[i], hashes[i]);
    }
    
    // Test determinism
    for (size_t i = 0; i < num_doubles; i++) {
        size_t hash2 = d_HashDouble(&test_doubles[i], sizeof(double));
        TEST_ASSERT(hashes[i] == hash2, "Double hash should be deterministic");
    }
    
    check_hash_distribution(hashes, num_doubles, "DOUBLE");
    
    return 1;
}

// =============================================================================
// BINARY DATA HASH FUNCTION TESTS
// =============================================================================

int test_hash_binary_functionality(void)
{
    d_LogInfo("\n=== 🔢 BINARY DATA HASH FUNCTION TEST ===");
    
    // Test with structured data
    TestStruct_t structs[] = {
        {1, 2, "test1"},
        {10, 20, "test2"},
        {100, 200, "test3"},
        {-1, -2, "test4"}
    };
    size_t num_structs = sizeof(structs) / sizeof(structs[0]);
    size_t hashes[4];
    
    // Generate hashes
    for (size_t i = 0; i < num_structs; i++) {
        hashes[i] = d_HashBinary(&structs[i], sizeof(TestStruct_t));
        d_LogInfoF("d_HashBinary(struct{%d, %d, \"%s\"}) = %zu", 
                   structs[i].x, structs[i].y, structs[i].name, hashes[i]);
    }
    
    // Test determinism
    for (size_t i = 0; i < num_structs; i++) {
        size_t hash2 = d_HashBinary(&structs[i], sizeof(TestStruct_t));
        TEST_ASSERT(hashes[i] == hash2, "Binary hash should be deterministic");
    }
    
    check_hash_distribution(hashes, num_structs, "BINARY");
    
    // Test with raw byte arrays
    unsigned char data1[] = {0x00, 0x11, 0x22, 0x33, 0x44};
    unsigned char data2[] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB};
    
    size_t hash1 = d_HashBinary(data1, sizeof(data1));
    size_t hash2 = d_HashBinary(data2, sizeof(data2));
    d_LogInfoF("Raw bytes [00,11,22,33,44] -> %zu", hash1);
    d_LogInfoF("Raw bytes [FF,EE,DD,CC,BB] -> %zu", hash2);
    TEST_ASSERT(hash1 != hash2, "Different byte arrays should hash differently");
    
    return 1;
}

int test_hash_binary_with_hash_table(void)
{
    // Create hash table for struct keys
    dTable_t* table = d_InitTable(sizeof(TestStruct_t), sizeof(char*), 
                                  d_HashBinary, d_CompareBinary, 16);
    TEST_ASSERT(table != NULL, "Should create table with binary hash functions");
    
    d_LogInfo("\n=== 🗂️  BINARY DATA HASH TABLE INTEGRATION TEST ===");
    
    // Test data
    TestStruct_t keys[] = {
        {1, 1, "first"},
        {2, 4, "second"},
        {3, 9, "third"}
    };
    char* values[] = {"value1", "value2", "value3"};
    size_t num_entries = sizeof(keys) / sizeof(keys[0]);
    
    // Insert all key-value pairs
    for (size_t i = 0; i < num_entries; i++) {
        TEST_ASSERT(d_SetDataInTable(table, &keys[i], &values[i]) == 0, 
                   "Should insert struct key successfully");
    }
    
    // Verify all lookups work
    for (size_t i = 0; i < num_entries; i++) {
        char** found_value = (char**)d_GetDataFromTable(table, &keys[i]);
        TEST_ASSERT(found_value != NULL, "Should find struct key");
        TEST_ASSERT(strcmp(*found_value, values[i]) == 0, "Should retrieve correct value");
        d_LogInfoF("✅ Struct{%d, %d, \"%s\"} -> \"%s\"", 
                   keys[i].x, keys[i].y, keys[i].name, *found_value);
    }
    
    d_DestroyTable(&table);
    return 1;
}

// =============================================================================
// POINTER HASH FUNCTION TESTS
// =============================================================================

int test_hash_pointer_functionality(void)
{
    d_LogInfo("\n=== 🔢 POINTER HASH FUNCTION TEST ===");
    
    // Create some variables to get different pointers
    int var1 = 10, var2 = 20, var3 = 30;
    char str1[] = "test1", str2[] = "test2";
    
    void* pointers[] = {&var1, &var2, &var3, str1, str2, NULL};
    size_t num_ptrs = sizeof(pointers) / sizeof(pointers[0]);
    size_t hashes[6];
    
    // Generate hashes
    for (size_t i = 0; i < num_ptrs; i++) {
        hashes[i] = d_HashPointer(&pointers[i], sizeof(void*));
        d_LogInfoF("d_HashPointer(%p) = %zu", pointers[i], hashes[i]);
    }
    
    // Test determinism
    for (size_t i = 0; i < num_ptrs; i++) {
        size_t hash2 = d_HashPointer(&pointers[i], sizeof(void*));
        TEST_ASSERT(hashes[i] == hash2, "Pointer hash should be deterministic");
    }
    
    // Different pointers should generally produce different hashes
    check_hash_distribution(hashes, num_ptrs - 1, "POINTER"); // Exclude NULL from distribution check
    
    return 1;
}

// =============================================================================
// SPECIALIZED HASH FUNCTION TESTS
// =============================================================================

int test_hash_small_int_optimization(void)
{
    d_LogInfo("\n=== 🔢 SMALL INTEGER HASH OPTIMIZATION TEST ===");
    
    // Test small integer range (0-999)
    int small_ints[] = {0, 1, 2, 5, 10, 50, 100, 500, 999};
    size_t num_small = sizeof(small_ints) / sizeof(small_ints[0]);
    
    // Compare regular vs optimized hash
    for (size_t i = 0; i < num_small; i++) {
        size_t regular_hash = d_HashInt(&small_ints[i], sizeof(int));
        size_t optimized_hash = d_HashSmallInt(&small_ints[i], sizeof(int));
        
        d_LogInfoF("Value %d: Regular=%zu, Optimized=%zu", 
                   small_ints[i], regular_hash, optimized_hash);
    }
    
    // Both should be deterministic
    for (size_t i = 0; i < num_small; i++) {
        size_t hash1 = d_HashSmallInt(&small_ints[i], sizeof(int));
        size_t hash2 = d_HashSmallInt(&small_ints[i], sizeof(int));
        TEST_ASSERT(hash1 == hash2, "Optimized hash should be deterministic");
    }
    
    return 1;
}

// =============================================================================
// COMPARISON FUNCTION TESTS
// =============================================================================

int test_comparison_functions_correctness(void)
{
    d_LogInfo("\n=== ⚖️  COMPARISON FUNCTIONS TEST ===");
    
    // Test integer comparison
    int int1 = 42, int2 = 42, int3 = 43;
    TEST_ASSERT(d_CompareInt(&int1, &int2, sizeof(int)) == 0, "Equal integers should compare as equal");
    TEST_ASSERT(d_CompareInt(&int1, &int3, sizeof(int)) == 1, "Different integers should compare as different");
    d_LogInfo("✅ Integer comparison works correctly");
    
    // Test string comparison
    char* str1 = "hello";
    char* str2 = "hello";
    char* str3 = "world";
    char** ptr1 = &str1, **ptr2 = &str2, **ptr3 = &str3;
    TEST_ASSERT(d_CompareString(&ptr1, &ptr2, 0) == 0, "Equal strings should compare as equal");
    TEST_ASSERT(d_CompareString(&ptr1, &ptr3, 0) == 1, "Different strings should compare as different");
    d_LogInfo("✅ String comparison works correctly");
    
    // Test string literal comparison
    char literal1[] = "test";
    char literal2[] = "test";
    char literal3[] = "different";
    TEST_ASSERT(d_CompareStringLiteral(literal1, literal2, 0) == 0, "Equal string literals should compare as equal");
    TEST_ASSERT(d_CompareStringLiteral(literal1, literal3, 0) == 1, "Different string literals should compare as different");
    d_LogInfo("✅ String literal comparison works correctly");
    
    // Test float comparison
    float f1 = 3.14159f, f2 = 3.14159f, f3 = 2.71828f;
    TEST_ASSERT(d_CompareFloat(&f1, &f2, sizeof(float)) == 0, "Equal floats should compare as equal");
    TEST_ASSERT(d_CompareFloat(&f1, &f3, sizeof(float)) == 1, "Different floats should compare as different");
    d_LogInfo("✅ Float comparison works correctly");
    
    // Test binary comparison
    TestStruct_t s1 = {1, 2, "test"};
    TestStruct_t s2 = {1, 2, "test"};
    TestStruct_t s3 = {3, 4, "different"};
    TEST_ASSERT(d_CompareBinary(&s1, &s2, sizeof(TestStruct_t)) == 0, "Equal structs should compare as equal");
    TEST_ASSERT(d_CompareBinary(&s1, &s3, sizeof(TestStruct_t)) == 1, "Different structs should compare as different");
    d_LogInfo("✅ Binary comparison works correctly");
    
    return 1;
}

// =============================================================================
// EDGE CASE AND ERROR HANDLING TESTS
// =============================================================================

int test_null_and_edge_cases(void)
{
    d_LogInfo("\n=== ⚠️  NULL AND EDGE CASE HANDLING TEST ===");
    
    // Test NULL key handling for all hash functions
    TEST_ASSERT(d_HashInt(NULL, sizeof(int)) == 0, "NULL int key should hash to 0");
    TEST_ASSERT(d_HashString(NULL, 0) == 0, "NULL string key should hash to 0");
    TEST_ASSERT(d_HashFloat(NULL, sizeof(float)) == 0, "NULL float key should hash to 0");
    TEST_ASSERT(d_HashBinary(NULL, 10) == 0, "NULL binary key should hash to 0");
    TEST_ASSERT(d_HashPointer(NULL, sizeof(void*)) == 0, "NULL pointer key should hash to 0");
    d_LogInfo("✅ All hash functions handle NULL keys safely");
    
    // Test NULL handling for comparison functions
    int test_int = 42;
    TEST_ASSERT(d_CompareInt(NULL, &test_int, sizeof(int)) == 1, "NULL vs non-NULL should be different");
    TEST_ASSERT(d_CompareInt(&test_int, NULL, sizeof(int)) == 1, "Non-NULL vs NULL should be different");
    TEST_ASSERT(d_CompareString(NULL, NULL, 0) == 1, "NULL vs NULL should be handled safely");
    d_LogInfo("✅ All comparison functions handle NULL safely");
    
    // Test zero-size binary data
    char dummy_data = 'x';
    TEST_ASSERT(d_HashBinary(&dummy_data, 0) == 0, "Zero-size binary data should hash to 0");
    TEST_ASSERT(d_CompareBinary(&dummy_data, &dummy_data, 0) == 0, "Zero-size binary data should compare as equal");
    d_LogInfo("✅ Zero-size binary data handled correctly");
    
    // Test empty string
    char* empty_str = "";
    char** empty_ptr = &empty_str;
    size_t empty_hash = d_HashString(&empty_ptr, 0);
    d_LogInfoF("Empty string hash: %zu", empty_hash);
    TEST_ASSERT(empty_hash != 0, "Empty string should have non-zero hash"); // FNV-1a gives non-zero for empty
    
    return 1;
}

// =============================================================================
// PERFORMANCE AND STRESS TESTS
// =============================================================================

int test_hash_performance_comparison(void)
{
    d_LogInfo("\n=== 🚀 HASH FUNCTION PERFORMANCE COMPARISON ===");
    
    const size_t num_ops = 1000;
    
    // Generate test data
    int* test_ints = malloc(num_ops * sizeof(int));
    for (size_t i = 0; i < num_ops; i++) {
        test_ints[i] = (int)(i * 1327 + 999); // Some pattern to avoid sequential
    }
    
    d_LogInfoF("Performing %zu hash operations...", num_ops);
    
    // Test integer hashing speed (basic measurement)
    size_t total_hash = 0;
    for (size_t i = 0; i < num_ops; i++) {
        total_hash += d_HashInt(&test_ints[i], sizeof(int));
    }
    
    d_LogInfoF("Integer hash total: %zu (sanity check - should be non-zero)", total_hash);
    TEST_ASSERT(total_hash > 0, "Hash operations should produce non-zero total");
    
    // Test with actual hash table to measure real-world performance
    dTable_t* perf_table = d_InitTable(sizeof(int), sizeof(size_t), d_HashInt, d_CompareInt, 64);
    TEST_ASSERT(perf_table != NULL, "Should create performance test table");
    
    // Insert many entries
    LOOP_TEST_START();
    for (size_t i = 0; i < num_ops; i++) {
        size_t value = i;
        TEST_ASSERT(d_SetDataInTable(perf_table, &test_ints[i], &value) == 0, 
                   "Should insert entry in performance test");
    }
    
    // Lookup all entries
    size_t successful_lookups = 0;
    for (size_t i = 0; i < num_ops; i++) {
        size_t* found = (size_t*)d_GetDataFromTable(perf_table, &test_ints[i]);
        if (found && *found == i) {
            successful_lookups++;
        }
    }
    LOOP_TEST_END();
    
    TEST_ASSERT(successful_lookups == num_ops, "All lookups should succeed in performance test");
    d_LogInfoF("✅ Successfully performed %zu hash table operations", successful_lookups);
    
    // Check table statistics
    d_LogInfoF("📊 Table contains %zu entries", d_GetCountInTable(perf_table));
    
    free(test_ints);
    d_DestroyTable(&perf_table);
    return 1;
}

// =============================================================================
// COMPREHENSIVE INTEGRATION TESTS
// =============================================================================

int test_mixed_data_types_hash_table(void)
{
    d_LogInfo("\n=== 🔀 MIXED DATA TYPES INTEGRATION TEST ===");
    
    // Test multiple hash tables with different built-in functions
    dTable_t* int_table = d_InitTable(sizeof(int), sizeof(char*), d_HashInt, d_CompareInt, 8);
    dTable_t* str_table = d_InitTable(sizeof(char*), sizeof(int), d_HashString, d_CompareString, 8);
    dTable_t* float_table = d_InitTable(sizeof(float), sizeof(char*), d_HashFloat, d_CompareFloat, 8);
    
    TEST_ASSERT(int_table != NULL && str_table != NULL && float_table != NULL, 
               "Should create all mixed type tables");
    
    // Integer table
    int int_keys[] = {1, 2, 3};
    char* int_values[] = {"one", "two", "three"};
    for (int i = 0; i < 3; i++) {
        d_SetDataInTable(int_table, &int_keys[i], &int_values[i]);
    }
    
    // String table  
    char* str_keys[] = {"apple", "banana", "cherry"};
    int str_values[] = {100, 200, 300};
    for (int i = 0; i < 3; i++) {
        d_SetDataInTable(str_table, &str_keys[i], &str_values[i]);
    }
    
    // Float table
    float float_keys[] = {1.1f, 2.2f, 3.3f};
    char* float_values[] = {"1.1", "2.2", "3.3"};
    for (int i = 0; i < 3; i++) {
        d_SetDataInTable(float_table, &float_keys[i], &float_values[i]);
    }
    
    // Verify all tables work correctly
    char** int_result = (char**)d_GetDataFromTable(int_table, &int_keys[1]);
    int* str_result = (int*)d_GetDataFromTable(str_table, &str_keys[1]);
    char** float_result = (char**)d_GetDataFromTable(float_table, &float_keys[1]);
    
    TEST_ASSERT(int_result != NULL && strcmp(*int_result, "two") == 0, "Integer table should work");
    TEST_ASSERT(str_result != NULL && *str_result == 200, "String table should work");
    TEST_ASSERT(float_result != NULL && strcmp(*float_result, "2.2") == 0, "Float table should work");
    
    d_LogInfoF("✅ Integer table: key 2 -> value \"%s\"", *int_result);
    d_LogInfoF("✅ String table: key \"%s\" -> value %d", str_keys[1], *str_result);
    d_LogInfoF("✅ Float table: key %.1f -> value \"%s\"", float_keys[1], *float_result);
    
    d_DestroyTable(&int_table);
    d_DestroyTable(&str_table);
    d_DestroyTable(&float_table);
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
    
    d_LogInfo("Starting Built-in Hash and Comparison Functions Tests");

    TEST_SUITE_START("Built-in Hash and Comparison Functions Tests");

    // Integer hash function tests
    RUN_TEST(test_hash_int_basic_functionality);
    RUN_TEST(test_hash_int_with_hash_table);

    // String hash function tests
    RUN_TEST(test_hash_string_basic_functionality);
    RUN_TEST(test_hash_string_literal_functionality);
    RUN_TEST(test_hash_string_case_insensitive);
    RUN_TEST(test_hash_string_with_hash_table);

    // Floating-point hash function tests
    RUN_TEST(test_hash_float_functionality);
    RUN_TEST(test_hash_double_functionality);

    // Binary data hash function tests
    RUN_TEST(test_hash_binary_functionality);
    RUN_TEST(test_hash_binary_with_hash_table);

    // Pointer hash function tests
    RUN_TEST(test_hash_pointer_functionality);

    // Specialized hash function tests
    RUN_TEST(test_hash_small_int_optimization);

    // Comparison function tests
    RUN_TEST(test_comparison_functions_correctness);

    // Edge case and error handling tests
    RUN_TEST(test_null_and_edge_cases);

    // Performance and stress tests
    RUN_TEST(test_hash_performance_comparison);

    // Integration tests
    RUN_TEST(test_mixed_data_types_hash_table);

    TEST_SUITE_END();

    d_LogInfo("Built-in Hash and Comparison Functions Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}