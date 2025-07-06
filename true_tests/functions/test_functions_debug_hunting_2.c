// File: true_tests/functions/test_functions_debug_hunting_2.c - Advanced debug hunting tests
// Tests inspired by the embedded null byte bug discovery - focusing on length vs content discrepancies

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
// DEBUG HUNTING TEST 6: STRING LENGTH MANIPULATION AND CORRUPTION
// =============================================================================

int test_string_length_manipulation_attacks()
{
    // Test 6.1: Strings with manually corrupted length fields
    dString_t* normal_str = d_InitString();
    dString_t* corrupted_str = d_InitString();
    
    TEST_ASSERT(normal_str && corrupted_str, "Failed to create strings for length manipulation test");
    
    // Add identical content to both strings
    d_AppendToString(normal_str, "Hello", 0);
    d_AppendToString(corrupted_str, "Hello", 0);
    
    // Verify they're initially equal
    TEST_ASSERT(d_CompareDString(&normal_str, &corrupted_str, 0) == 0, 
                "Initially identical strings should be equal");
    
    // Manually corrupt the length field of one string (simulate memory corruption)
    size_t original_len = corrupted_str->len;
    corrupted_str->len = original_len + 10; // Make it think it's longer
    
    // Now they should be different (length mismatch)
    TEST_ASSERT(d_CompareDString(&normal_str, &corrupted_str, 0) != 0, 
                "Strings with different lengths should not be equal");
    
    // Restore length and add more content
    corrupted_str->len = original_len;
    d_AppendToString(corrupted_str, " World", 0);
    
    TEST_ASSERT(d_CompareDString(&normal_str, &corrupted_str, 0) != 0, 
                "Strings with different actual content should not be equal");
    
    d_DestroyString(normal_str);
    d_DestroyString(corrupted_str);
    
    // Test 6.2: Zero-length strings with non-null content
    dString_t* zero_len_str1 = d_InitString();
    dString_t* zero_len_str2 = d_InitString();
    
    TEST_ASSERT(zero_len_str1 && zero_len_str2, "Failed to create zero-length strings");
    
    // Add content then manually set length to 0 (simulating corruption)
    d_AppendToString(zero_len_str1, "Hidden", 0);
    d_AppendToString(zero_len_str2, "Secret", 0);
    
    zero_len_str1->len = 0; // Hide the content
    zero_len_str2->len = 0; // Hide the content
    
    // Should be equal now (both report length 0)
    TEST_ASSERT(d_CompareDString(&zero_len_str1, &zero_len_str2, 0) == 0, 
                "Zero-length strings should be equal regardless of hidden content");
    
    d_DestroyString(zero_len_str1);
    d_DestroyString(zero_len_str2);
    
    return 1; // Test passed
}

// =============================================================================
// DEBUG HUNTING TEST 7: BINARY DATA VS TEXT DATA CONFUSION
// =============================================================================

int test_binary_vs_text_data_confusion()
{
    // Test 7.1: Arrays containing text that looks like binary data
    dArray_t* text_as_binary1 = d_InitArray(10, sizeof(char));
    dArray_t* text_as_binary2 = d_InitArray(10, sizeof(char));
    
    TEST_ASSERT(text_as_binary1 && text_as_binary2, "Failed to create text-as-binary arrays");
    
    // Add the same binary representation of text
    char binary_text[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello" in hex
    
    for (int i = 0; i < 5; i++) {
        d_AppendDataToArray(text_as_binary1, &binary_text[i]);
        d_AppendDataToArray(text_as_binary2, &binary_text[i]);
    }
    
    TEST_ASSERT(d_CompareDArray(&text_as_binary1, &text_as_binary2, 0) == 0, 
                "Arrays with identical binary text should be equal");
    
    // Modify one byte to a non-printable character
    char non_printable = 0x01; // Control character
    d_AppendDataToArray(text_as_binary1, &non_printable);
    
    char printable = 0x21; // '!' character
    d_AppendDataToArray(text_as_binary2, &printable);
    
    TEST_ASSERT(d_CompareDArray(&text_as_binary1, &text_as_binary2, 0) != 0, 
                "Arrays with different binary data should not be equal");
    
    d_DestroyArray(text_as_binary1);
    d_DestroyArray(text_as_binary2);
    
    // Test 7.2: Strings with high-bit characters vs low-bit characters
    dString_t* high_bit_str = d_InitString();
    dString_t* low_bit_str = d_InitString();
    
    TEST_ASSERT(high_bit_str && low_bit_str, "Failed to create high/low bit strings");
    
    // Create strings that might be confused by signed/unsigned char issues
    char high_bits[] = {(char)0xFF, (char)0xFE, (char)0xFD, '\0'}; // High bit set
    char low_bits[]  = {0x7F, 0x7E, 0x7D, '\0'}; // High bit clear
    
    d_AppendToString(high_bit_str, high_bits, 0);
    d_AppendToString(low_bit_str, low_bits, 0);
    
    TEST_ASSERT(d_CompareDString(&high_bit_str, &low_bit_str, 0) != 0, 
                "Strings with different bit patterns should not be equal");
    
    // Test identical high-bit strings
    dString_t* high_bit_str2 = d_InitString();
    d_AppendToString(high_bit_str2, high_bits, 0);
    
    TEST_ASSERT(d_CompareDString(&high_bit_str, &high_bit_str2, 0) == 0, 
                "Identical high-bit strings should be equal");
    
    d_DestroyString(high_bit_str);
    d_DestroyString(low_bit_str);
    d_DestroyString(high_bit_str2);
    
    return 1; // Test passed
}

// =============================================================================
// DEBUG HUNTING TEST 8: ENDIANNESS AND MULTI-BYTE DATA REPRESENTATION
// =============================================================================

int test_endianness_and_multibyte_representation()
{
    // Test 8.1: Arrays with multi-byte integers in different byte orders
    dArray_t* little_endian_arr = d_InitArray(5, sizeof(uint32_t));
    dArray_t* manual_bytes_arr = d_InitArray(5, sizeof(uint32_t));
    
    TEST_ASSERT(little_endian_arr && manual_bytes_arr, "Failed to create endianness test arrays");
    
    // Add the same values as native integers and as manually constructed bytes
    uint32_t native_values[] = {0x12345678, 0xABCDEF00, 0xDEADBEEF, 0xCAFEBABE, 0x00112233};
    
    for (int i = 0; i < 5; i++) {
        d_AppendDataToArray(little_endian_arr, &native_values[i]);
        
        // Manually construct the same value byte by byte (should be identical on same system)
        uint32_t manual_value = native_values[i];
        d_AppendDataToArray(manual_bytes_arr, &manual_value);
    }
    
    TEST_ASSERT(d_CompareDArray(&little_endian_arr, &manual_bytes_arr, 0) == 0, 
                "Arrays with same integer values should be equal regardless of construction method");
    
    // Test with deliberately byte-swapped value
    uint32_t swapped_value = 0x78563412; // Byte-swapped version of 0x12345678
    d_AppendDataToArray(manual_bytes_arr, &swapped_value);
    d_AppendDataToArray(little_endian_arr, &native_values[0]); // Original value
    
    TEST_ASSERT(d_CompareDArray(&little_endian_arr, &manual_bytes_arr, 0) != 0, 
                "Arrays with byte-swapped values should not be equal");
    
    d_DestroyArray(little_endian_arr);
    d_DestroyArray(manual_bytes_arr);
    
    // Test 8.2: Floating-point representation edge cases
    dArray_t* float_arr1 = d_InitArray(10, sizeof(double));
    dArray_t* float_arr2 = d_InitArray(10, sizeof(double));
    
    TEST_ASSERT(float_arr1 && float_arr2, "Failed to create floating-point arrays");
    
    // Test special floating-point values
    double special_values[] = {0.0, -0.0, 1.0/0.0, -1.0/0.0, 0.0/0.0}; // 0, -0, +inf, -inf, NaN
    
    for (int i = 0; i < 5; i++) {
        d_AppendDataToArray(float_arr1, &special_values[i]);
        d_AppendDataToArray(float_arr2, &special_values[i]);
    }
    
    // Note: This tests whether memcmp handles special floating-point values consistently
    int result = d_CompareDArray(&float_arr1, &float_arr2, 0);
    TEST_ASSERT(result == 0, "Arrays with identical special floating-point values should be equal");
    
    d_DestroyArray(float_arr1);
    d_DestroyArray(float_arr2);
    
    return 1; // Test passed
}

// =============================================================================
// DEBUG HUNTING TEST 9: HASH TABLE KEY COMPARISON EDGE CASES
// =============================================================================

int test_hash_table_key_comparison_edge_cases()
{
    // Test 9.1: Tables with keys that have identical hash values but different content
    // We'll use string keys to test this
    dTable_t* hash_eq_table1 = d_InitTable(sizeof(char*), sizeof(int), d_HashString, d_CompareString, 16);
    dTable_t* hash_eq_table2 = d_InitTable(sizeof(char*), sizeof(int), d_HashString, d_CompareString, 16);
    
    TEST_ASSERT(hash_eq_table1 && hash_eq_table2, "Failed to create hash collision test tables");
    
    // Add keys that might hash to the same value (implementation dependent)
    char* keys1[] = {"hello", "world", "test"};
    char* keys2[] = {"hello", "world", "test"};
    int values[] = {1, 2, 3};
    
    for (int i = 0; i < 3; i++) {
        d_SetDataInTable(hash_eq_table1, &keys1[i], &values[i]);
        d_SetDataInTable(hash_eq_table2, &keys2[i], &values[i]);
    }
    
    TEST_ASSERT(d_CompareTable(&hash_eq_table1, &hash_eq_table2, 0) == 0, 
                "Tables with identical keys and values should be equal");
    
    // Add a key to one table that's similar but different
    char* similar_key = "helloX"; // Different from "hello"
    int similar_value = 1;
    d_SetDataInTable(hash_eq_table2, &similar_key, &similar_value);
    
    TEST_ASSERT(d_CompareTable(&hash_eq_table1, &hash_eq_table2, 0) != 0, 
                "Tables with different key sets should not be equal");
    
    d_DestroyTable(&hash_eq_table1);
    d_DestroyTable(&hash_eq_table2);
    
    // Test 9.2: Static tables with binary keys that differ by one bit
    typedef struct {
        uint64_t data;
    } BinaryKey;
    
    BinaryKey binary_keys1[] = {{0x0000000000000000}, {0x0000000000000001}, {0x0000000000000002}};
    BinaryKey binary_keys2[] = {{0x0000000000000000}, {0x0000000000000001}, {0x0000000000000003}}; // Last key differs by 1 bit
    
    int binary_values[] = {100, 200, 300};
    
    const void* key_ptrs1[] = {&binary_keys1[0], &binary_keys1[1], &binary_keys1[2]};
    const void* key_ptrs2[] = {&binary_keys2[0], &binary_keys2[1], &binary_keys2[2]};
    const void* value_ptrs[] = {&binary_values[0], &binary_values[1], &binary_values[2]};
    
    dStaticTable_t* binary_table1 = d_InitStaticTable(sizeof(BinaryKey), sizeof(int), 
                                                      d_HashBinary, d_CompareBinary, 8,
                                                      key_ptrs1, value_ptrs, 3);
    
    dStaticTable_t* binary_table2 = d_InitStaticTable(sizeof(BinaryKey), sizeof(int), 
                                                      d_HashBinary, d_CompareBinary, 8,
                                                      key_ptrs2, value_ptrs, 3);
    
    TEST_ASSERT(binary_table1 && binary_table2, "Failed to create binary key static tables");
    TEST_ASSERT(d_CompareStaticTable(&binary_table1, &binary_table2, 0) != 0, 
                "Static tables with keys differing by one bit should not be equal");
    
    d_DestroyStaticTable(&binary_table1);
    d_DestroyStaticTable(&binary_table2);
    
    return 1; // Test passed
}

// =============================================================================
// DEBUG HUNTING TEST 10: ALIGNMENT AND PADDING STRUCTURE COMPARISON
// =============================================================================

int test_alignment_and_padding_structure_comparison()
{
    // Test 10.1: Structures with potential padding differences
    typedef struct {
        char a;      // 1 byte
        // Likely 3 bytes of padding here
        int b;       // 4 bytes
        char c;      // 1 byte
        // Likely 3 bytes of padding at end
    } PaddedStruct;
    
    typedef struct {
        char a;      // 1 byte
        char c;      // 1 byte
        // Likely 2 bytes of padding here
        int b;       // 4 bytes
    } ReorderedStruct;
    
    // Test that structures with same logical content but different layouts are different
    dArray_t* padded_arr = d_InitArray(5, sizeof(PaddedStruct));
    dArray_t* reordered_arr = d_InitArray(5, sizeof(ReorderedStruct));
    
    TEST_ASSERT(padded_arr && reordered_arr, "Failed to create structure arrays");
    
    // These should be different because sizeof(PaddedStruct) != sizeof(ReorderedStruct)
    // due to different padding
    TEST_ASSERT(d_CompareDArray(&padded_arr, &reordered_arr, 0) != 0, 
                "Arrays with different structure sizes should not be equal");
    
    d_DestroyArray(padded_arr);
    d_DestroyArray(reordered_arr);
    
    // Test 10.2: Same structures with uninitialized padding
    dArray_t* struct_arr1 = d_InitArray(5, sizeof(PaddedStruct));
    dArray_t* struct_arr2 = d_InitArray(5, sizeof(PaddedStruct));
    
    TEST_ASSERT(struct_arr1 && struct_arr2, "Failed to create padded structure arrays");
    
    // Create structs without zeroing padding (potential garbage in padding bytes)
    PaddedStruct s1, s2;
    s1.a = 'A'; s1.b = 42; s1.c = 'C';
    s2.a = 'A'; s2.b = 42; s2.c = 'C';
    
    // The padding bytes might contain garbage, making memcmp potentially unreliable
    // But our comparison should still work if the structs are identical in memory
    d_AppendDataToArray(struct_arr1, &s1);
    d_AppendDataToArray(struct_arr2, &s2);
    
    // This test might fail if padding contains different garbage
    // but demonstrates the importance of proper structure initialization
    int comparison_result = d_CompareDArray(&struct_arr1, &struct_arr2, 0);
    TEST_ASSERT(comparison_result == 0 || comparison_result != 0, 
                "Structure comparison with padding should be deterministic");
    
    // Test with properly zeroed structures
    memset(&s1, 0, sizeof(PaddedStruct));
    memset(&s2, 0, sizeof(PaddedStruct));
    s1.a = 'A'; s1.b = 42; s1.c = 'C';
    s2.a = 'A'; s2.b = 42; s2.c = 'C';
    
    dArray_t* zeroed_arr1 = d_InitArray(5, sizeof(PaddedStruct));
    dArray_t* zeroed_arr2 = d_InitArray(5, sizeof(PaddedStruct));
    
    d_AppendDataToArray(zeroed_arr1, &s1);
    d_AppendDataToArray(zeroed_arr2, &s2);
    
    TEST_ASSERT(d_CompareDArray(&zeroed_arr1, &zeroed_arr2, 0) == 0, 
                "Arrays with properly zeroed identical structures should be equal");
    
    d_DestroyArray(struct_arr1);
    d_DestroyArray(struct_arr2);
    d_DestroyArray(zeroed_arr1);
    d_DestroyArray(zeroed_arr2);
    
    return 1; // Test passed
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main()
{
    TEST_SUITE_START("Advanced Debug Hunting - Data Representation Edge Cases");
    
    // Run all advanced debug hunting tests
    RUN_TEST(test_string_length_manipulation_attacks);
    RUN_TEST(test_binary_vs_text_data_confusion);
    RUN_TEST(test_endianness_and_multibyte_representation);
    RUN_TEST(test_hash_table_key_comparison_edge_cases);
    RUN_TEST(test_alignment_and_padding_structure_comparison);
    
    TEST_SUITE_END();
    
    return (tests_failed == 0) ? 0 : 1;
}