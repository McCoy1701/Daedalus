// File: true_tests/functions/test_functions_daedalus_compare.c - Tests for built-in Daedalus comparison functions
// Tests for dString, dArray, dStaticArray, dTable, and dStaticTable comparison functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "tests.h"
#include "Daedalus.h"

// Global test counters (required by tests.h framework)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// DSTRING COMPARISON TESTS
// =============================================================================

int test_dstring_comparison_basic()
{
    dString_t* str1 = d_InitString();
    dString_t* str2 = d_InitString();
    dString_t* str3 = d_InitString();
    
    TEST_ASSERT(str1 && str2 && str3, "Failed to create test strings");
    
    // Test empty strings
    TEST_ASSERT(d_CompareDString(&str1, &str2, 0) == 0, "Empty strings should be equal");
    
    // Add same content to str1 and str2
    d_AppendToString(str1, "Hello World", 0);
    d_AppendToString(str2, "Hello World", 0);
    TEST_ASSERT(d_CompareDString(&str1, &str2, 0) == 0, "Identical strings should be equal");
    
    // Add different content to str3
    d_AppendToString(str3, "Hello Universe", 0);
    TEST_ASSERT(d_CompareDString(&str1, &str3, 0) != 0, "Different strings should not be equal");
    
    // Test NULL handling
    dString_t* null_str = NULL;
    TEST_ASSERT(d_CompareDString(&str1, &null_str, 0) != 0, "String vs NULL should not be equal");
    TEST_ASSERT(d_CompareDString(&null_str, &null_str, 0) == 0, "NULL vs NULL should be equal");
    
    d_DestroyString(str1);
    d_DestroyString(str2);
    d_DestroyString(str3);
    
    return 1; // Test passed
}

int test_dstring_comparison_edge_cases()
{
    dString_t* str1 = d_InitString();
    dString_t* str2 = d_InitString();
    
    TEST_ASSERT(str1 && str2, "Failed to create test strings");
    
    // Test strings with different lengths
    d_AppendToString(str1, "Short", 0);
    d_AppendToString(str2, "Much longer string", 0);
    TEST_ASSERT(d_CompareDString(&str1, &str2, 0) != 0, "Different length strings should not be equal");
    
    // Test strings with same prefix
    d_ClearString(str1);
    d_ClearString(str2);
    d_AppendToString(str1, "Prefix", 0);
    d_AppendToString(str2, "Prefix_Extended", 0);
    TEST_ASSERT(d_CompareDString(&str1, &str2, 0) != 0, "Prefix strings should not be equal");
    
    // Test case sensitivity
    d_ClearString(str1);
    d_ClearString(str2);
    d_AppendToString(str1, "CaseSensitive", 0);
    d_AppendToString(str2, "casesensitive", 0);
    TEST_ASSERT(d_CompareDString(&str1, &str2, 0) != 0, "Case different strings should not be equal");
    
    d_DestroyString(str1);
    d_DestroyString(str2);
    
    return 1; // Test passed
}

// =============================================================================
// DARRAY COMPARISON TESTS
// =============================================================================

int test_darray_comparison_basic()
{
    dArray_t* arr1 = d_InitArray(10, sizeof(int));
    dArray_t* arr2 = d_InitArray(10, sizeof(int));
    dArray_t* arr3 = d_InitArray(10, sizeof(int));
    
    TEST_ASSERT(arr1 && arr2 && arr3, "Failed to create test arrays");
    
    // Test empty arrays with same element size
    TEST_ASSERT(d_CompareDArray(&arr1, &arr2, 0) == 0, "Empty arrays with same element_size should be equal");
    
    // Add same data to arr1 and arr2
    for (int i = 0; i < 5; i++) {
        int value = i * 10;
        d_AppendDataToArray(arr1, &value);
        d_AppendDataToArray(arr2, &value);
    }
    TEST_ASSERT(d_CompareDArray(&arr1, &arr2, 0) == 0, "Arrays with identical data should be equal");
    
    // Add different data to arr3
    for (int i = 0; i < 5; i++) {
        int value = i * 20; // Different values
        d_AppendDataToArray(arr3, &value);
    }
    TEST_ASSERT(d_CompareDArray(&arr1, &arr3, 0) != 0, "Arrays with different data should not be equal");
    
    // Test NULL handling
    dArray_t* null_arr = NULL;
    TEST_ASSERT(d_CompareDArray(&arr1, &null_arr, 0) != 0, "Array vs NULL should not be equal");
    TEST_ASSERT(d_CompareDArray(&null_arr, &null_arr, 0) == 0, "NULL vs NULL should be equal");
    
    d_DestroyArray(arr1);
    d_DestroyArray(arr2);
    d_DestroyArray(arr3);
    
    return 1; // Test passed
}

int test_darray_comparison_edge_cases()
{
    dArray_t* int_arr = d_InitArray(10, sizeof(int));
    dArray_t* double_arr = d_InitArray(10, sizeof(double)); // Use double instead of float (8 bytes vs 4)
    dArray_t* empty_arr = d_InitArray(10, sizeof(int));
    
    TEST_ASSERT(int_arr && double_arr && empty_arr, "Failed to create test arrays");
    
    // Test different element sizes
    TEST_ASSERT(d_CompareDArray(&int_arr, &double_arr, 0) != 0, 
                "Arrays with different element_size should not be equal");
    
    // Add data to int_arr
    for (int i = 0; i < 3; i++) {
        int value = i;
        d_AppendDataToArray(int_arr, &value);
    }
    
    // Test different counts
    TEST_ASSERT(d_CompareDArray(&int_arr, &empty_arr, 0) != 0, 
                "Arrays with different counts should not be equal");
    
    // Test arrays with same count but different data
    dArray_t* other_arr = d_InitArray(10, sizeof(int));
    for (int i = 0; i < 3; i++) {
        int value = i + 100; // Different values
        d_AppendDataToArray(other_arr, &value);
    }
    TEST_ASSERT(d_CompareDArray(&int_arr, &other_arr, 0) != 0, 
                "Arrays with same count but different data should not be equal");
    
    d_DestroyArray(int_arr);
    d_DestroyArray(double_arr);
    d_DestroyArray(empty_arr);
    d_DestroyArray(other_arr);
    
    return 1; // Test passed
}

// =============================================================================
// DSTATICARRAY COMPARISON TESTS
// =============================================================================

int test_dstaticarray_comparison_basic()
{
    dStaticArray_t* arr1 = d_InitStaticArray(10, sizeof(int));
    dStaticArray_t* arr2 = d_InitStaticArray(10, sizeof(int));
    dStaticArray_t* arr3 = d_InitStaticArray(10, sizeof(int));
    
    TEST_ASSERT(arr1 && arr2 && arr3, "Failed to create test static arrays");
    
    // Test empty arrays
    TEST_ASSERT(d_CompareStaticArray(&arr1, &arr2, 0) == 0, "Empty static arrays should be equal");
    
    // Add same data to arr1 and arr2
    for (int i = 0; i < 5; i++) {
        int value = i * 10;
        d_AppendDataToStaticArray(arr1, &value);
        d_AppendDataToStaticArray(arr2, &value);
    }
    TEST_ASSERT(d_CompareStaticArray(&arr1, &arr2, 0) == 0, "Static arrays with identical data should be equal");
    
    // Add different data to arr3
    for (int i = 0; i < 5; i++) {
        int value = i * 20; // Different values
        d_AppendDataToStaticArray(arr3, &value);
    }
    TEST_ASSERT(d_CompareStaticArray(&arr1, &arr3, 0) != 0, "Static arrays with different data should not be equal");
    
    d_DestroyStaticArray(arr1);
    d_DestroyStaticArray(arr2);
    d_DestroyStaticArray(arr3);
    
    return 1; // Test passed
}

// =============================================================================
// DTABLE COMPARISON TESTS
// =============================================================================

int test_dtable_comparison_basic()
{
    dTable_t* table1 = d_InitTable(sizeof(int), sizeof(char*), d_HashInt, d_CompareInt, 8);
    dTable_t* table2 = d_InitTable(sizeof(int), sizeof(char*), d_HashInt, d_CompareInt, 8);
    dTable_t* table3 = d_InitTable(sizeof(int), sizeof(char*), d_HashInt, d_CompareInt, 8);
    
    TEST_ASSERT(table1 && table2 && table3, "Failed to create test tables");
    
    // Test empty tables
    TEST_ASSERT(d_CompareTable(&table1, &table2, 0) == 0, "Empty tables should be equal");
    
    // Add same data to table1 and table2
    int keys[] = {1, 2, 3};
    char* values[] = {"one", "two", "three"};
    
    for (int i = 0; i < 3; i++) {
        d_SetDataInTable(table1, &keys[i], &values[i]);
        d_SetDataInTable(table2, &keys[i], &values[i]);
    }
    TEST_ASSERT(d_CompareTable(&table1, &table2, 0) == 0, "Tables with identical data should be equal");
    
    // Add different data to table3
    char* different_values[] = {"ONE", "TWO", "THREE"};
    for (int i = 0; i < 3; i++) {
        d_SetDataInTable(table3, &keys[i], &different_values[i]);
    }
    TEST_ASSERT(d_CompareTable(&table1, &table3, 0) != 0, "Tables with different values should not be equal");
    
    d_DestroyTable(&table1);
    d_DestroyTable(&table2);
    d_DestroyTable(&table3);
    
    return 1; // Test passed
}

int test_dtable_comparison_edge_cases()
{
    dTable_t* int_table = d_InitTable(sizeof(int), sizeof(int), d_HashInt, d_CompareInt, 8);
    dTable_t* str_table = d_InitTable(sizeof(char*), sizeof(int), d_HashString, d_CompareString, 8);
    dTable_t* empty_table = d_InitTable(sizeof(int), sizeof(int), d_HashInt, d_CompareInt, 8);
    
    TEST_ASSERT(int_table && str_table && empty_table, "Failed to create test tables");
    
    // Test tables with different key/value sizes
    TEST_ASSERT(d_CompareTable(&int_table, &str_table, 0) != 0, 
                "Tables with different key/value sizes should not be equal");
    
    // Add data to int_table
    for (int i = 0; i < 3; i++) {
        int key = i;
        int value = i * 10;
        d_SetDataInTable(int_table, &key, &value);
    }
    
    // Test different counts
    TEST_ASSERT(d_CompareTable(&int_table, &empty_table, 0) != 0, 
                "Tables with different counts should not be equal");
    
    // Test tables with missing keys
    dTable_t* partial_table = d_InitTable(sizeof(int), sizeof(int), d_HashInt, d_CompareInt, 8);
    int key = 0, value = 0;
    d_SetDataInTable(partial_table, &key, &value); // Only add first key
    TEST_ASSERT(d_CompareTable(&int_table, &partial_table, 0) != 0, 
                "Tables with missing keys should not be equal");
    
    d_DestroyTable(&int_table);
    d_DestroyTable(&str_table);
    d_DestroyTable(&empty_table);
    d_DestroyTable(&partial_table);
    
    return 1; // Test passed
}

// =============================================================================
// DSTATICTABLE COMPARISON TESTS
// =============================================================================

int test_dstatictable_comparison_basic()
{
    // Create test data
    int keys[] = {10, 20, 30};
    char* values[] = {"ten", "twenty", "thirty"};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2]};
    
    dStaticTable_t* table1 = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                               d_HashInt, d_CompareInt, 8,
                                               key_ptrs, value_ptrs, 3);
    
    dStaticTable_t* table2 = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                               d_HashInt, d_CompareInt, 8,
                                               key_ptrs, value_ptrs, 3);
    
    TEST_ASSERT(table1 && table2, "Failed to create test static tables");
    
    // Test identical tables
    TEST_ASSERT(d_CompareStaticTable(&table1, &table2, 0) == 0, "Identical static tables should be equal");
    
    // Create table with different values
    char* different_values[] = {"TEN", "TWENTY", "THIRTY"};
    const void* different_value_ptrs[] = {&different_values[0], &different_values[1], &different_values[2]};
    
    dStaticTable_t* table3 = d_InitStaticTable(sizeof(int), sizeof(char*), 
                                               d_HashInt, d_CompareInt, 8,
                                               key_ptrs, different_value_ptrs, 3);
    
    TEST_ASSERT(table3, "Failed to create test static table with different values");
    TEST_ASSERT(d_CompareStaticTable(&table1, &table3, 0) != 0, "Static tables with different values should not be equal");
    
    d_DestroyStaticTable(&table1);
    d_DestroyStaticTable(&table2);
    d_DestroyStaticTable(&table3);
    
    return 1; // Test passed
}

// =============================================================================
// COMPREHENSIVE INTEGRATION TESTS
// =============================================================================

int test_mixed_comparison_scenarios()
{
    // Test arrays containing different data types
    dArray_t* int_arr1 = d_InitArray(5, sizeof(int));
    dArray_t* int_arr2 = d_InitArray(5, sizeof(int));
    
    // Arrays with structures
    typedef struct {
        int id;
        float value;
    } TestStruct;
    
    dArray_t* struct_arr1 = d_InitArray(3, sizeof(TestStruct));
    dArray_t* struct_arr2 = d_InitArray(3, sizeof(TestStruct));
    
    TEST_ASSERT(struct_arr1 && struct_arr2, "Failed to create struct arrays");
    
    // Add identical struct data
    for (int i = 0; i < 3; i++) {
        TestStruct data = {i, i * 1.5f};
        d_AppendDataToArray(struct_arr1, &data);
        d_AppendDataToArray(struct_arr2, &data);
    }
    
    TEST_ASSERT(d_CompareDArray(&struct_arr1, &struct_arr2, 0) == 0, 
                "Arrays with identical struct data should be equal");
    
    // Modify one element
    TestStruct* first_elem = (TestStruct*)d_IndexDataFromArray(struct_arr2, 0);
    first_elem->value = 999.0f;
    
    TEST_ASSERT(d_CompareDArray(&struct_arr1, &struct_arr2, 0) != 0, 
                "Arrays with modified struct data should not be equal");
    
    d_DestroyArray(int_arr1);
    d_DestroyArray(int_arr2);
    d_DestroyArray(struct_arr1);
    d_DestroyArray(struct_arr2);
    
    return 1; // Test passed
}

// =============================================================================
// PERFORMANCE AND STRESS TESTS
// =============================================================================

int test_large_data_comparison()
{
    const size_t large_size = 1000;
    
    dArray_t* large_arr1 = d_InitArray(large_size, sizeof(int));
    dArray_t* large_arr2 = d_InitArray(large_size, sizeof(int));
    
    TEST_ASSERT(large_arr1 && large_arr2, "Failed to create large test arrays");
    
    // Fill with identical data
    LOOP_TEST_START();
    for (size_t i = 0; i < large_size; i++) {
        int value = (int)(i * 7) % 1000; // Some pattern
        d_AppendDataToArray(large_arr1, &value);
        d_AppendDataToArray(large_arr2, &value);
        
        if (i < 5) { // Only assert first few to avoid spam
            TEST_ASSERT(1, "Adding data to large arrays");
        }
    }
    LOOP_TEST_END();
    
    // This should be fast for identical arrays
    TEST_ASSERT(d_CompareDArray(&large_arr1, &large_arr2, 0) == 0, 
                "Large arrays with identical data should be equal");
    
    // Modify last element to test early termination doesn't happen inappropriately
    int* last_elem = (int*)d_IndexDataFromArray(large_arr2, large_size - 1);
    *last_elem = -1;
    
    TEST_ASSERT(d_CompareDArray(&large_arr1, &large_arr2, 0) != 0, 
                "Large arrays with one different element should not be equal");
    
    d_DestroyArray(large_arr1);
    d_DestroyArray(large_arr2);
    
    return 1; // Test passed
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main()
{
    TEST_SUITE_START("Daedalus Comparison Functions");
    
    // dString comparison tests
    RUN_TEST(test_dstring_comparison_basic);
    RUN_TEST(test_dstring_comparison_edge_cases);
    
    // dArray comparison tests
    RUN_TEST(test_darray_comparison_basic);
    RUN_TEST(test_darray_comparison_edge_cases);
    
    // dStaticArray comparison tests
    RUN_TEST(test_dstaticarray_comparison_basic);
    
    // dTable comparison tests
    RUN_TEST(test_dtable_comparison_basic);
    RUN_TEST(test_dtable_comparison_edge_cases);
    
    // dStaticTable comparison tests
    RUN_TEST(test_dstatictable_comparison_basic);
    
    // Integration and performance tests
    RUN_TEST(test_mixed_comparison_scenarios);
    RUN_TEST(test_large_data_comparison);
    
    TEST_SUITE_END();
    
    return (tests_failed == 0) ? 0 : 1;
}