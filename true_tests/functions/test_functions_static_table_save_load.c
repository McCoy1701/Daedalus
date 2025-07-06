// File: true_tests/functions/test_functions_static_table_save_load.c - Tests for built-in Daedalus static table functions with save/load
// Tests the interaction between built-in functions (hash, compare) and static table save/load operations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include "tests.h"
#include "Daedalus.h"

// Global test counters (required by tests.h framework)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

// Helper to clean up test files
static void cleanup_test_file(const char* filename) {
    if (access(filename, F_OK) == 0) {
        remove(filename);
    }
}

// =============================================================================
// TEST 1: BUILT-IN INTEGER FUNCTIONS WITH SAVE/LOAD
// =============================================================================

int test_builtin_int_functions_save_load()
{
    const char* test_file = "test_builtin_int_functions.dat";
    cleanup_test_file(test_file);
    
    // Test 1.1: Create static table using built-in integer functions
    int keys[] = {100, 200, 300, 400, 500};
    char* values[] = {"hundred", "two hundred", "three hundred", "four hundred", "five hundred"};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3], &keys[4]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3], &values[4]};
    
    dStaticTable_t* int_table = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                                  d_HashInt, d_CompareInt, 8,
                                                  key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(int_table != NULL, "Should create table with built-in int functions");
    
    // Test comparison functions work before save
    dStaticTable_t* int_table_copy = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                                       d_HashInt, d_CompareInt, 8,
                                                       key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(d_CompareStaticTable(&int_table, &int_table_copy, 0) == 0, 
                "Identical tables with int functions should be equal");
    
    // Test 1.2: Save table using built-in functions
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, int_table) == 0, 
                "Should save table with built-in int functions");
    
    // Test 1.3: Load and verify functions still work
    dStaticTable_t* loaded_int_table = d_LoadStaticTableFromFile(test_file, d_HashInt, d_CompareInt);
    TEST_ASSERT(loaded_int_table != NULL, "Should load table with built-in int functions");
    
    // Test comparison after load
    TEST_ASSERT(d_CompareStaticTable(&int_table, &loaded_int_table, 0) == 0, 
                "Original and loaded int tables should be equal");
    
    // Test hash functions still work by checking lookups
    for (int i = 0; i < 5; i++) {
        char** value = (char**)d_GetValueInStaticTable(loaded_int_table, &keys[i]);
        TEST_ASSERT(value != NULL && strcmp(*value, values[i]) == 0, 
                    "Built-in hash function should work after load");
    }
    
    d_DestroyStaticTable(&int_table);
    d_DestroyStaticTable(&int_table_copy);
    d_DestroyStaticTable(&loaded_int_table);
    cleanup_test_file(test_file);
    
    return 1; // Test passed
}

// =============================================================================
// TEST 2: BUILT-IN STRING FUNCTIONS WITH SAVE/LOAD
// =============================================================================

int test_builtin_string_functions_save_load()
{
    const char* test_file = "test_builtin_string_functions.dat";
    cleanup_test_file(test_file);
    
    // Test 2.1: Create static table using built-in string functions
    char* keys[] = {"alpha", "beta", "gamma", "delta", "epsilon"};
    int values[] = {1, 2, 3, 4, 5};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3], &keys[4]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3], &values[4]};
    
    dStaticTable_t* string_table = d_InitStaticTable(sizeof(char*), sizeof(int), 
                                                     d_HashString, d_CompareString, 16,
                                                     key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(string_table != NULL, "Should create table with built-in string functions");
    
    // Test string functions work before save
    char* test_key = "gamma";
    int* found_value = (int*)d_GetValueInStaticTable(string_table, &test_key);
    TEST_ASSERT(found_value != NULL && *found_value == 3, 
                "Built-in string hash should find correct value");
    
    // Test 2.2: Save and load
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, string_table) == 0, 
                "Should save table with built-in string functions");
    
    dStaticTable_t* loaded_string_table = d_LoadStaticTableFromFile(test_file, d_HashString, d_CompareString);
    TEST_ASSERT(loaded_string_table != NULL, "Should load table with built-in string functions");
    
    // Test 2.3: Verify string functions after load
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_GetValueInStaticTable(loaded_string_table, &keys[i]);
        TEST_ASSERT(value != NULL && *value == values[i], 
                    "Built-in string functions should work after load");
    }
    
    // Test comparison of tables with string keys
    TEST_ASSERT(d_CompareStaticTable(&string_table, &loaded_string_table, 0) == 0, 
                "Original and loaded string tables should be equal");
    
    d_DestroyStaticTable(&string_table);
    d_DestroyStaticTable(&loaded_string_table);
    cleanup_test_file(test_file);
    
    return 1; // Test passed
}

// =============================================================================
// TEST 3: BUILT-IN BINARY FUNCTIONS WITH COMPLEX DATA
// =============================================================================

typedef struct {
    uint32_t id;
    float score;
    char tag;
} ComplexKey;

int test_builtin_binary_functions_save_load()
{
    const char* test_file = "test_builtin_binary_functions.dat";
    cleanup_test_file(test_file);
    
    // Test 3.1: Create table with binary keys using built-in binary functions
    ComplexKey keys[] = {
        {1001, 95.5f, 'A'},
        {1002, 87.2f, 'B'},
        {1003, 92.1f, 'C'},
        {1004, 88.9f, 'D'}
    };
    
    char* values[] = {"excellent", "good", "very good", "good+"};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3]};
    
    dStaticTable_t* binary_table = d_InitStaticTable(sizeof(ComplexKey), sizeof(char*), 
                                                     d_HashBinary, d_CompareBinary, 8,
                                                     key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(binary_table != NULL, "Should create table with built-in binary functions");
    
    // Test 3.2: Verify binary comparison works before save
    ComplexKey test_key = {1002, 87.2f, 'B'};
    char** found_value = (char**)d_GetValueInStaticTable(binary_table, &test_key);
    TEST_ASSERT(found_value != NULL && strcmp(*found_value, "good") == 0, 
                "Built-in binary hash should find correct value");
    
    // Test 3.3: Save and load with binary functions
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, binary_table) == 0, 
                "Should save table with built-in binary functions");
    
    dStaticTable_t* loaded_binary_table = d_LoadStaticTableFromFile(test_file, d_HashBinary, d_CompareBinary);
    TEST_ASSERT(loaded_binary_table != NULL, "Should load table with built-in binary functions");
    
    // Test 3.4: Verify binary functions work after load
    for (int i = 0; i < 4; i++) {
        char** value = (char**)d_GetValueInStaticTable(loaded_binary_table, &keys[i]);
        TEST_ASSERT(value != NULL && strcmp(*value, values[i]) == 0, 
                    "Built-in binary functions should work after load");
    }
    
    // Test binary comparison of complex structures
    TEST_ASSERT(d_CompareStaticTable(&binary_table, &loaded_binary_table, 0) == 0, 
                "Original and loaded binary tables should be equal");
    
    d_DestroyStaticTable(&binary_table);
    d_DestroyStaticTable(&loaded_binary_table);
    cleanup_test_file(test_file);
    
    return 1; // Test passed
}

// =============================================================================
// TEST 4: NESTED TABLES WITH COMPARISON FUNCTIONS
// =============================================================================

int test_nested_tables_with_functions_save_load()
{
    const char* inner_file = "test_inner_table.dat";
    const char* outer_file = "test_outer_table.dat";
    cleanup_test_file(inner_file);
    cleanup_test_file(outer_file);
    
    // Test 4.1: Create inner tables using built-in functions
    int inner_keys1[] = {10, 20, 30};
    char* inner_values1[] = {"ten", "twenty", "thirty"};
    const void* inner_key_ptrs1[] = {&inner_keys1[0], &inner_keys1[1], &inner_keys1[2]};
    const void* inner_value_ptrs1[] = {&inner_values1[0], &inner_values1[1], &inner_values1[2]};
    
    dStaticTable_t* inner_table1 = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                                     d_HashInt, d_CompareInt, 4,
                                                     inner_key_ptrs1, inner_value_ptrs1, 3);
    
    int inner_keys2[] = {100, 200};
    char* inner_values2[] = {"hundred", "two hundred"};
    const void* inner_key_ptrs2[] = {&inner_keys2[0], &inner_keys2[1]};
    const void* inner_value_ptrs2[] = {&inner_values2[0], &inner_values2[1]};
    
    dStaticTable_t* inner_table2 = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                                     d_HashInt, d_CompareInt, 4,
                                                     inner_key_ptrs2, inner_value_ptrs2, 2);
    
    TEST_ASSERT(inner_table1 && inner_table2, "Should create inner tables");
    
    // Test 4.2: Save inner tables
    TEST_ASSERT(d_SaveStaticTableToFile(inner_file, inner_table1) == 0, "Should save inner table 1");
    
    // Test 4.3: Create outer table that contains table pointers
    char* outer_keys[] = {"table1", "table2"};
    dStaticTable_t* table_values[] = {inner_table1, inner_table2};
    const void* outer_key_ptrs[] = {&outer_keys[0], &outer_keys[1]};
    const void* outer_value_ptrs[] = {&table_values[0], &table_values[1]};
    
    dStaticTable_t* outer_table = d_InitStaticTable(sizeof(char*), sizeof(dStaticTable_t*), 
                                                    d_HashString, d_CompareString, 4,
                                                    outer_key_ptrs, outer_value_ptrs, 2);
    
    TEST_ASSERT(outer_table != NULL, "Should create outer table containing table pointers");
    
    // Test 4.4: Verify nested access works
    char* lookup_key = "table1";
    dStaticTable_t** found_table = (dStaticTable_t**)d_GetValueInStaticTable(outer_table, &lookup_key);
    TEST_ASSERT(found_table != NULL && *found_table == inner_table1, 
                "Should find correct inner table");
    
    int inner_lookup_key = 20;
    char** inner_value = (char**)d_GetValueInStaticTable(*found_table, &inner_lookup_key);
    TEST_ASSERT(inner_value != NULL && strcmp(*inner_value, "twenty") == 0, 
                "Should access nested table value");
    
    // Test 4.5: Save outer table
    TEST_ASSERT(d_SaveStaticTableToFile(outer_file, outer_table) == 0, "Should save outer table");
    
    // Test 4.6: Load and verify structure is preserved
    dStaticTable_t* loaded_outer = d_LoadStaticTableFromFile(outer_file, d_HashString, d_CompareString);
    TEST_ASSERT(loaded_outer != NULL, "Should load outer table");
    
    // Note: Table pointers won't be valid after load, but we can verify the structure
    TEST_ASSERT(d_GetKeyCountOfStaticTable(loaded_outer) == 2, 
                "Loaded outer table should have correct key count");
    
    d_DestroyStaticTable(&inner_table1);
    d_DestroyStaticTable(&inner_table2);
    d_DestroyStaticTable(&outer_table);
    d_DestroyStaticTable(&loaded_outer);
    cleanup_test_file(inner_file);
    cleanup_test_file(outer_file);
    
    return 1; // Test passed
}

// =============================================================================
// TEST 5: MULTIPLE SAVE/LOAD CYCLES WITH FUNCTION INTEGRITY
// =============================================================================

int test_multiple_save_load_cycles_function_integrity()
{
    const char* test_file = "test_multiple_cycles.dat";
    cleanup_test_file(test_file);
    
    // Test 5.1: Create initial table with mixed data types as values
    typedef struct {
        int int_val;
        float float_val;
        char char_val;
    } MixedValue;
    
    char* keys[] = {"first", "second", "third", "fourth"};
    MixedValue values[] = {
        {100, 1.5f, 'A'},
        {200, 2.7f, 'B'},
        {300, 3.14f, 'C'},
        {400, 4.0f, 'D'}
    };
    
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3]};
    
    dStaticTable_t* cycle_table = d_InitStaticTable(sizeof(char*), sizeof(MixedValue), 
                                                    d_HashString, d_CompareString, 8,
                                                    key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(cycle_table != NULL, "Should create initial cycle table");
    
    // Test 5.2: Multiple save/load cycles
    for (int cycle = 0; cycle < 3; cycle++) {
        // Save current state
        TEST_ASSERT(d_SaveStaticTableToFile(test_file, cycle_table) == 0, 
                    "Should save in cycle iteration");
        
        // Load and verify
        dStaticTable_t* loaded_cycle = d_LoadStaticTableFromFile(test_file, d_HashString, d_CompareString);
        TEST_ASSERT(loaded_cycle != NULL, "Should load in cycle iteration");
        
        // Test that comparison functions still work correctly
        TEST_ASSERT(d_CompareStaticTable(&cycle_table, &loaded_cycle, 0) == 0, 
                    "Tables should be equal after cycle");
        
        // Test that hash functions still work for lookups
        for (int i = 0; i < 4; i++) {
            MixedValue* found_value = (MixedValue*)d_GetValueInStaticTable(loaded_cycle, &keys[i]);
            TEST_ASSERT(found_value != NULL && 
                       found_value->int_val == values[i].int_val &&
                       found_value->float_val == values[i].float_val &&
                       found_value->char_val == values[i].char_val, 
                       "Hash function should work correctly after cycle");
        }
        
        // Update values for next cycle
        for (int i = 0; i < 4; i++) {
            MixedValue* value_ptr = (MixedValue*)d_GetValueInStaticTable(loaded_cycle, &keys[i]);
            if (value_ptr) {
                value_ptr->int_val += (cycle + 1) * 100;
                value_ptr->float_val += (cycle + 1) * 1.0f;
                value_ptr->char_val += 1;
                values[i] = *value_ptr; // Update expected values
            }
        }
        
        d_DestroyStaticTable(&cycle_table);
        cycle_table = loaded_cycle; // Use loaded table for next iteration
    }
    
    d_DestroyStaticTable(&cycle_table);
    cleanup_test_file(test_file);
    
    return 1; // Test passed
}

// =============================================================================
// TEST 6: CASE-INSENSITIVE STRING FUNCTIONS WITH SAVE/LOAD
// =============================================================================

int test_case_insensitive_functions_save_load()
{
    const char* test_file = "test_case_insensitive.dat";
    cleanup_test_file(test_file);
    
    // Test 6.1: Create table using case-insensitive string functions
    char* keys[] = {"Apple", "BANANA", "CheRRy", "date"};
    int values[] = {1, 2, 3, 4};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3]};
    
    dStaticTable_t* case_table = d_InitStaticTable(sizeof(char*), sizeof(int), 
                                                   d_HashStringCaseInsensitive, 
                                                   d_CompareStringCaseInsensitive, 8,
                                                   key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(case_table != NULL, "Should create table with case-insensitive functions");
    
    // Test 6.2: Verify case-insensitive lookup works before save
    char* test_key_lower = "apple";
    int* found_value = (int*)d_GetValueInStaticTable(case_table, &test_key_lower);
    TEST_ASSERT(found_value != NULL && *found_value == 1, 
                "Case-insensitive hash should find 'Apple' with 'apple'");
    
    char* test_key_upper = "CHERRY";
    found_value = (int*)d_GetValueInStaticTable(case_table, &test_key_upper);
    TEST_ASSERT(found_value != NULL && *found_value == 3, 
                "Case-insensitive hash should find 'CheRRy' with 'CHERRY'");
    
    // Test 6.3: Save and load
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, case_table) == 0, 
                "Should save table with case-insensitive functions");
    
    dStaticTable_t* loaded_case_table = d_LoadStaticTableFromFile(test_file, 
                                                                  d_HashStringCaseInsensitive, 
                                                                  d_CompareStringCaseInsensitive);
    TEST_ASSERT(loaded_case_table != NULL, "Should load table with case-insensitive functions");
    
    // Test 6.4: Verify case-insensitive functions work after load
    char* mixed_case_keys[] = {"aPPle", "banana", "CHERRY", "DaTe"};
    for (int i = 0; i < 4; i++) {
        int* value = (int*)d_GetValueInStaticTable(loaded_case_table, &mixed_case_keys[i]);
        TEST_ASSERT(value != NULL && *value == values[i], 
                    "Case-insensitive functions should work after load");
    }
    
    d_DestroyStaticTable(&case_table);
    d_DestroyStaticTable(&loaded_case_table);
    cleanup_test_file(test_file);
    
    return 1; // Test passed
}

// =============================================================================
// TEST 7: EDGE CASES WITH FUNCTION COMBINATIONS
// =============================================================================

int test_edge_cases_function_combinations()
{
    const char* test_file = "test_edge_cases.dat";
    cleanup_test_file(test_file);
    
    // Test 7.1: Table with zero values (but valid keys)
    int keys[] = {0, -1, INT_MAX, INT_MIN};
    int values[] = {0, 0, 0, 0}; // All zeros
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3]};
    
    dStaticTable_t* zero_table = d_InitStaticTable(sizeof(int), sizeof(int), 
                                                   d_HashInt, d_CompareInt, 8,
                                                   key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(zero_table != NULL, "Should create table with zero values");
    
    // Test comparison with zeros
    dStaticTable_t* zero_table_copy = d_InitStaticTable(sizeof(int), sizeof(int), 
                                                        d_HashInt, d_CompareInt, 8,
                                                        key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(d_CompareStaticTable(&zero_table, &zero_table_copy, 0) == 0, 
                "Tables with zero values should compare equal");
    
    // Save and load
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, zero_table) == 0, 
                "Should save table with zero values");
    
    dStaticTable_t* loaded_zero = d_LoadStaticTableFromFile(test_file, d_HashInt, d_CompareInt);
    TEST_ASSERT(loaded_zero != NULL, "Should load table with zero values");
    
    // Verify zero values are preserved
    TEST_ASSERT(d_CompareStaticTable(&zero_table, &loaded_zero, 0) == 0, 
                "Zero value tables should be equal after save/load");
    
    // Test 7.2: Empty string keys
    char* empty_keys[] = {"", "a", "", "b"};
    int string_values[] = {100, 200, 300, 400};
    const void* empty_key_ptrs[] = {&empty_keys[0], &empty_keys[1], &empty_keys[2], &empty_keys[3]};
    const void* string_value_ptrs[] = {&string_values[0], &string_values[1], &string_values[2], &string_values[3]};
    
    // This should fail due to duplicate empty string keys
    dStaticTable_t* empty_string_table = d_InitStaticTable(sizeof(char*), sizeof(int), 
                                                           d_HashString, d_CompareString, 8,
                                                           empty_key_ptrs, string_value_ptrs, 4);
    
    TEST_ASSERT(empty_string_table == NULL, "Should fail to create table with duplicate empty string keys");
    
    d_DestroyStaticTable(&zero_table);
    d_DestroyStaticTable(&zero_table_copy);
    d_DestroyStaticTable(&loaded_zero);
    cleanup_test_file(test_file);
    
    return 1; // Test passed
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main()
{
    TEST_SUITE_START("Built-in Functions with Static Table Save/Load");
    
    // Run all integration tests
    RUN_TEST(test_builtin_int_functions_save_load);
    RUN_TEST(test_builtin_string_functions_save_load);
    RUN_TEST(test_builtin_binary_functions_save_load);
    RUN_TEST(test_nested_tables_with_functions_save_load);
    RUN_TEST(test_multiple_save_load_cycles_function_integrity);
    RUN_TEST(test_case_insensitive_functions_save_load);
    RUN_TEST(test_edge_cases_function_combinations);
    
    TEST_SUITE_END();
    
    return (tests_failed == 0) ? 0 : 1;
}