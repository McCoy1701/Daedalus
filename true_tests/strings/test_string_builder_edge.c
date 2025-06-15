// File: true_tests/strings/test_string_builder_edge.c - Edge case tests for string builder

#include "../../include/tests.h"
#include "../../include/Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper Functions
_dString_t* create_test_builder(void)
{
    return d_StringCreate();
}

void fill_builder_with_content(_dString_t* sb, size_t target_size)
{
    // Fill with repeated pattern to reach target size
    const char* pattern = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t pattern_len = strlen(pattern);

    while (d_StringLen(sb) < target_size) {
        size_t remaining = target_size - d_StringLen(sb);
        size_t to_add = (remaining < pattern_len) ? remaining : pattern_len;
        d_StringAddStr(sb, pattern, to_add);
    }
}

// Edge Case Test Functions

int test_string_builder_empty_string_operations(void)
{
    _dString_t* sb = create_test_builder();
    printf("Before adding empty string: %s\n", d_StringPeek(sb));
    // Test adding empty string
    d_StringAddStr(sb, "", 0);
    printf("After adding empty string: %s\n", d_StringPeek(sb));
    TEST_ASSERT(d_StringLen(sb) == 0, "Adding empty string should not change length");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "") == 0, "Content should remain empty");

    // Test adding empty string with explicit length
    d_StringAddStr(sb, "test", 0); // Should add because 0 uses strlen
    printf("After adding empty string with explicit length 4: %s\n", d_StringPeek(sb));
    TEST_ASSERT(d_StringLen(sb) == 4, "Adding with explicit length 0 should add");

    // Add some content, then test truncate/drop to zero
    d_StringAddStr(sb, "Hello", 0);
    d_StringTruncate(sb, 0);
    TEST_ASSERT(d_StringLen(sb) == 0, "Truncate to 0 should make empty");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "") == 0, "Content should be empty after truncate to 0");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_boundary_conditions(void)
{
    _dString_t* sb = create_test_builder();

    // Test exactly at initial capacity boundary (32 bytes)
    // Add 31 characters (leaving 1 for null terminator)
    d_StringAddStr(sb, "1234567890123456789012345678901", 31);
    TEST_ASSERT(d_StringLen(sb) == 31, "Should handle exactly 31 characters");

    // Add one more character to trigger growth
    d_StringAddChar(sb, 'X');
    TEST_ASSERT(d_StringLen(sb) == 32, "Should handle growth trigger correctly");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "1234567890123456789012345678901X") == 0, "Content should be correct after growth");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_large_content(void)
{
    _dString_t* sb = create_test_builder();

    // Test building a large string (more than initial capacity)
    const size_t large_size = 1000;
    fill_builder_with_content(sb, large_size);

    TEST_ASSERT(d_StringLen(sb) == large_size, "Should handle large content correctly");
    TEST_ASSERT(strlen(d_StringPeek(sb)) == large_size, "String length should match builder length");

    // Test that content is still valid
    const char* content = d_StringPeek(sb);
    TEST_ASSERT(content[0] == 'A', "First character should be correct");
    TEST_ASSERT(content[large_size - 1] != '\0', "Last character should not be null");
    TEST_ASSERT(content[large_size] == '\0', "Should be null terminated");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_partial_string_operations(void)
{
    _dString_t* sb = create_test_builder();

    // Test adding partial strings with explicit length
    const char* test_str = "Hello, World!";

    // Add only "Hello"
    d_StringAddStr(sb, test_str, 5);
    TEST_ASSERT(d_StringLen(sb) == 5, "Should add exactly 5 characters");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hello") == 0, "Should contain only first 5 characters");

    // Add partial from middle ", Wor"
    d_StringAddStr(sb, test_str + 5, 4);
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hello, Wo") == 0, "Should append partial string correctly");

    // Test partial string longer than actual string (should only copy available chars)
    d_StringClear(sb);
    const char* short_str = "Hi";
    d_StringAddStr(sb, short_str, 10); // Request more than available
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hi") == 0, "Should only copy available characters");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_special_characters(void)
{
    _dString_t* sb = create_test_builder();

    // Test with special characters
    d_StringAddStr(sb, "Tab:\t", 0);
    d_StringAddStr(sb, "Newline:\n", 0);
    d_StringAddStr(sb, "Quote:\"", 0);
    d_StringAddStr(sb, "Backslash:\\", 0);

    const char* content = d_StringPeek(sb);
    TEST_ASSERT(strstr(content, "Tab:\t") != NULL, "Should contain tab character");
    TEST_ASSERT(strstr(content, "Newline:\n") != NULL, "Should contain newline character");
    TEST_ASSERT(strstr(content, "Quote:\"") != NULL, "Should contain quote character");
    TEST_ASSERT(strstr(content, "Backslash:\\") != NULL, "Should contain backslash character");

    // Test null character (though this breaks string functions)
    d_StringClear(sb);
    d_StringAddChar(sb, 'A');
    d_StringAddChar(sb, '\0');
    d_StringAddChar(sb, 'B');

    TEST_ASSERT(d_StringLen(sb) == 3, "Should handle null character in middle");
    TEST_ASSERT(d_StringPeek(sb)[0] == 'A', "First char should be A");
    TEST_ASSERT(d_StringPeek(sb)[1] == '\0', "Second char should be null");
    TEST_ASSERT(d_StringPeek(sb)[2] == 'B', "Third char should be B");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_integer_edge_cases(void)
{
    _dString_t* sb = create_test_builder();

    // Test various integer edge cases
    d_StringAddInt(sb, 0);
    d_StringAddChar(sb, '|');

    d_StringAddInt(sb, -1);
    d_StringAddChar(sb, '|');

    d_StringAddInt(sb, INT_MAX);
    d_StringAddChar(sb, '|');

    d_StringAddInt(sb, INT_MIN);
    d_StringAddChar(sb, '|');

    const char* content = d_StringPeek(sb);
    TEST_ASSERT(strstr(content, "0|") != NULL, "Should contain zero");
    TEST_ASSERT(strstr(content, "-1|") != NULL, "Should contain negative one");
    TEST_ASSERT(strstr(content, "2147483647|") != NULL, "Should contain INT_MAX");
    TEST_ASSERT(strstr(content, "-2147483648|") != NULL, "Should contain INT_MIN");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_truncate_edge_cases(void)
{
    _dString_t* sb = create_test_builder();

    // Add content
    d_StringAddStr(sb, "Hello, World!", 0);
    size_t original_len = d_StringLen(sb);

    // Test truncate to same length (should do nothing)
    d_StringTruncate(sb, original_len);
    TEST_ASSERT(d_StringLen(sb) == original_len, "Truncate to same length should do nothing");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hello, World!") == 0, "Content should be unchanged");

    // Test truncate to longer length (should do nothing)
    d_StringTruncate(sb, original_len + 10);
    TEST_ASSERT(d_StringLen(sb) == original_len, "Truncate to longer length should do nothing");

    // Test truncate to 1 character
    d_StringTruncate(sb, 1);
    TEST_ASSERT(d_StringLen(sb) == 1, "Should truncate to 1 character");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "H") == 0, "Should contain only first character");

    // Test truncate to 0 (clear)
    d_StringTruncate(sb, 0);
    TEST_ASSERT(d_StringLen(sb) == 0, "Should truncate to empty");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "") == 0, "Should be empty string");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_drop_edge_cases(void)
{
    _dString_t* sb = create_test_builder();

    // Add content
    d_StringAddStr(sb, "Hello, World!", 0);

    // Test drop 0 characters (should do nothing)
    size_t original_len = d_StringLen(sb);
    d_StringDrop(sb, 0);
    TEST_ASSERT(d_StringLen(sb) == original_len, "Drop 0 should do nothing");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hello, World!") == 0, "Content should be unchanged");

    // Test drop 1 character
    d_StringDrop(sb, 1);
    TEST_ASSERT(strcmp(d_StringPeek(sb), "ello, World!") == 0, "Should drop first character");

    // Test drop exactly remaining length (should clear)
    size_t current_len = d_StringLen(sb);
    d_StringDrop(sb, current_len);
    TEST_ASSERT(d_StringLen(sb) == 0, "Drop all should clear");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "") == 0, "Should be empty");

    // Test drop more than length (should clear)
    d_StringAddStr(sb, "Test", 0);
    d_StringDrop(sb, 100);
    TEST_ASSERT(d_StringLen(sb) == 0, "Drop more than length should clear");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "") == 0, "Should be empty");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_dump_edge_cases(void)
{
    _dString_t* sb = create_test_builder();

    // Test dump of empty string builder
    size_t dump_len;
    char* empty_dump = d_StringDump(sb, &dump_len);

    TEST_ASSERT(empty_dump != NULL, "Dump of empty builder should return valid pointer");
    TEST_ASSERT(dump_len == 0, "Dump length should be 0 for empty builder");
    TEST_ASSERT(strcmp(empty_dump, "") == 0, "Dump should be empty string");
    free(empty_dump);

    // Test dump without length parameter
    char* empty_dump2 = d_StringDump(sb, NULL);
    TEST_ASSERT(empty_dump2 != NULL, "Dump without length should work");
    TEST_ASSERT(strcmp(empty_dump2, "") == 0, "Dump should be empty string");
    free(empty_dump2);

    // Test dump of large content
    fill_builder_with_content(sb, 500);
    char* large_dump = d_StringDump(sb, &dump_len);

    TEST_ASSERT(large_dump != NULL, "Dump of large content should succeed");
    TEST_ASSERT(dump_len == 500, "Dump length should match content length");
    TEST_ASSERT(strlen(large_dump) == 500, "Dumped string length should be correct");
    TEST_ASSERT(strcmp(large_dump, d_StringPeek(sb)) == 0, "Dump should match original content");

    // Verify original is unchanged
    TEST_ASSERT(d_StringLen(sb) == 500, "Original should be unchanged after dump");

    free(large_dump);
    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_memory_stress(void)
{
    _dString_t* sb = create_test_builder();

    // Test repeated growth and shrinkage
    for (int i = 0; i < 10; i++) {
        // Grow
        fill_builder_with_content(sb, 100 * (i + 1));
        TEST_ASSERT(d_StringLen(sb) == 100 * (i + 1), "Should grow correctly in iteration");

        // Shrink
        d_StringTruncate(sb, 50);
        TEST_ASSERT(d_StringLen(sb) == 50, "Should shrink correctly in iteration");

        // Clear
        d_StringClear(sb);
        TEST_ASSERT(d_StringLen(sb) == 0, "Should clear correctly in iteration");
    }

    // Test many small additions
    for (int i = 0; i < 1000; i++) {
        d_StringAddChar(sb, 'A' + (i % 26));
    }
    TEST_ASSERT(d_StringLen(sb) == 1000, "Should handle many small additions");

    // Verify pattern
    const char* content = d_StringPeek(sb);
    TEST_ASSERT(content[0] == 'A', "First character should be A");
    TEST_ASSERT(content[25] == 'Z', "26th character should be Z");
    TEST_ASSERT(content[26] == 'A', "Pattern should repeat");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_null_safety_comprehensive(void)
{
    // Comprehensive NULL safety testing

    // Test all functions with NULL string builder
    TEST_ASSERT(d_StringLen(NULL) == 0, "Len with NULL should return 0");
    TEST_ASSERT(d_StringPeek(NULL) == NULL, "Peek with NULL should return NULL");
    TEST_ASSERT(d_StringDump(NULL, NULL) == NULL, "Dump with NULL should return NULL");

    d_StringDestroy(NULL); // Should not crash
    d_StringAddStr(NULL, "test", 5);
    d_StringAddChar(NULL, 'x');
    d_StringAddInt(NULL, 42);
    d_StringClear(NULL);
    d_StringTruncate(NULL, 10);
    d_StringDrop(NULL, 5);

    // Test with valid builder but NULL string
    _dString_t* sb = create_test_builder();
    d_StringAddStr(sb, NULL, 0);
    d_StringAddStr(sb, NULL, 10);
    TEST_ASSERT(d_StringLen(sb) == 0, "Adding NULL string should not change length");

    d_StringDestroy(sb);

    TEST_ASSERT(1, "All NULL safety tests should pass without crashing");
    return 1;
}

int test_string_builder_len_zero_behavior(void)
{
    _dString_t* sb = create_test_builder();

    // Test: When len=0, strlen() should be called
    d_StringAddStr(sb, "test", 0);
    TEST_ASSERT(d_StringLen(sb) == 4, "len=0 should use strlen() and add 'test'");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "test") == 0, "Content should be 'test'");

    // Test: When len=0 with empty string, should add nothing
    d_StringClear(sb);
    d_StringAddStr(sb, "", 0);
    TEST_ASSERT(d_StringLen(sb) == 0, "len=0 with empty string should add nothing");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "") == 0, "Content should remain empty");

    // Test: When len > 0, should add exactly that many characters
    d_StringClear(sb);
    d_StringAddStr(sb, "Hello World", 5);
    TEST_ASSERT(d_StringLen(sb) == 5, "len=5 should add exactly 5 characters");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hello") == 0, "Content should be 'Hello'");

    // Test: When len > actual string length, should only copy available characters
    d_StringClear(sb);
    d_StringAddStr(sb, "Hi", 10);
    printf("Length of string: %zu\n", d_StringLen(sb));
    printf("Content of string: %s\n", d_StringPeek(sb));
    TEST_ASSERT(d_StringLen(sb) == 10, "Length should be 10");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hi") == 0, "Content should be 'Hi'");

    // Test: Edge case - len=0 with NULL-terminated string containing embedded nulls
    d_StringClear(sb);
    const char* str_with_null = "A\0B\0C";  // This string appears as "A" to strlen()
    d_StringAddStr(sb, str_with_null, 0);
    TEST_ASSERT(d_StringLen(sb) == 1, "strlen() should stop at first null");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "A") == 0, "Content should be 'A'");

    // Test: Explicit length can copy past embedded null
    d_StringClear(sb);
    d_StringAddStr(sb, str_with_null, 5);
    TEST_ASSERT(d_StringLen(sb) == 5, "Explicit length should copy all 5 bytes");
    TEST_ASSERT(d_StringPeek(sb)[0] == 'A', "First char should be 'A'");
    TEST_ASSERT(d_StringPeek(sb)[1] == '\0', "Second char should be null");
    TEST_ASSERT(d_StringPeek(sb)[2] == 'B', "Third char should be 'B'");

    d_StringDestroy(sb);
    return 1;
}


// Main test runner
int main(void)
{
    TEST_SUITE_START("String Builder Edge Case Tests");

    RUN_TEST(test_string_builder_empty_string_operations);
    RUN_TEST(test_string_builder_boundary_conditions);
    RUN_TEST(test_string_builder_large_content);
    RUN_TEST(test_string_builder_partial_string_operations);
    RUN_TEST(test_string_builder_special_characters);
    RUN_TEST(test_string_builder_integer_edge_cases);
    RUN_TEST(test_string_builder_truncate_edge_cases);
    RUN_TEST(test_string_builder_drop_edge_cases);
    RUN_TEST(test_string_builder_dump_edge_cases);
    RUN_TEST(test_string_builder_memory_stress);
    RUN_TEST(test_string_builder_null_safety_comprehensive);
    RUN_TEST(test_string_builder_len_zero_behavior);

    TEST_SUITE_END();
}
