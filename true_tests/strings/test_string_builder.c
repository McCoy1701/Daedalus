// File: true_tests/strings/test_string_builder.c - Tests for string builder functionality

#include "../../include/tests.h"
#include "../../include/Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper Functions
_dString_t* create_test_builder(void)
{
    return d_StringCreate();
}

// Test Functions
int test_string_builder_create_destroy(void)
{
    _dString_t* sb = d_StringCreate();

    TEST_ASSERT(sb != NULL, "Should create string builder successfully");
    TEST_ASSERT(d_StringLen(sb) == 0, "New string builder should have length 0");
    TEST_ASSERT(d_StringPeek(sb) != NULL, "Peek should return valid pointer");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "") == 0, "New string builder should be empty string");

    // Test destroy (should not crash)
    d_StringDestroy(sb);
    TEST_ASSERT(1, "Destroy should not crash");

    // Test destroy with NULL (should not crash)
    d_StringDestroy(NULL);
    TEST_ASSERT(1, "Destroy with NULL should not crash");

    return 1;
}

int test_string_builder_add_string(void)
{
    _dString_t* sb = create_test_builder();
    const char* test_str = "Hello, World!";

    // Test adding string with auto-length
    d_StringAddStr(sb, test_str, 0);
    TEST_ASSERT(d_StringLen(sb) == strlen(test_str), "Length should match added string");
    TEST_ASSERT(strcmp(d_StringPeek(sb), test_str) == 0, "Content should match added string");

    // Test adding string with explicit length
    d_StringAddStr(sb, " Test", 5);
    TEST_ASSERT(d_StringLen(sb) == strlen(test_str) + 5, "Length should include second string");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hello, World! Test") == 0, "Content should be concatenated");

    // Test adding partial string
    d_StringAddStr(sb, "12345", 3);
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hello, World! Test123") == 0, "Should add only specified length");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_add_char(void)
{
    _dString_t* sb = create_test_builder();

    // Test adding single character
    d_StringAddChar(sb, 'A');
    TEST_ASSERT(d_StringLen(sb) == 1, "Length should be 1 after adding char");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "A") == 0, "Content should be the added character");

    // Test adding multiple characters
    d_StringAddChar(sb, 'B');
    d_StringAddChar(sb, 'C');
    TEST_ASSERT(d_StringLen(sb) == 3, "Length should be 3 after adding 3 chars");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "ABC") == 0, "Content should be concatenated characters");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_add_int(void)
{
    _dString_t* sb = create_test_builder();

    // Test adding positive integer
    d_StringAddInt(sb, 123);
    TEST_ASSERT(strcmp(d_StringPeek(sb), "123") == 0, "Should add positive integer correctly");

    // Test adding negative integer
    d_StringAddInt(sb, -456);
    TEST_ASSERT(strcmp(d_StringPeek(sb), "123-456") == 0, "Should add negative integer correctly");

    // Test adding zero
    d_StringClear(sb);
    d_StringAddInt(sb, 0);
    TEST_ASSERT(strcmp(d_StringPeek(sb), "0") == 0, "Should add zero correctly");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_clear(void)
{
    _dString_t* sb = create_test_builder();

    // Add some content
    d_StringAddStr(sb, "Test content", 0);
    TEST_ASSERT(d_StringLen(sb) > 0, "Should have content before clear");

    // Clear and test
    d_StringClear(sb);
    TEST_ASSERT(d_StringLen(sb) == 0, "Length should be 0 after clear");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "") == 0, "Content should be empty after clear");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_truncate(void)
{
    _dString_t* sb = create_test_builder();

    // Add content
    d_StringAddStr(sb, "Hello, World!", 0);

    // Test truncate to shorter length
    d_StringTruncate(sb, 5);
    TEST_ASSERT(d_StringLen(sb) == 5, "Length should be 5 after truncate");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hello") == 0, "Content should be truncated");

    // Test truncate to same length (should do nothing)
    d_StringTruncate(sb, 5);
    TEST_ASSERT(d_StringLen(sb) == 5, "Length should remain 5");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "Hello") == 0, "Content should remain same");

    // Test truncate to longer length (should do nothing)
    d_StringTruncate(sb, 10);
    TEST_ASSERT(d_StringLen(sb) == 5, "Length should remain 5 when truncating to longer");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_drop(void)
{
    _dString_t* sb = create_test_builder();

    // Add content
    d_StringAddStr(sb, "Hello, World!", 0);

    // Test drop from beginning
    d_StringDrop(sb, 7); // Drop "Hello, "
    TEST_ASSERT(d_StringLen(sb) == 6, "Length should be 6 after dropping 7 chars");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "World!") == 0, "Content should have beginning dropped");

    // Test drop more than length (should clear)
    d_StringDrop(sb, 10);
    TEST_ASSERT(d_StringLen(sb) == 0, "Should be empty after dropping more than length");
    TEST_ASSERT(strcmp(d_StringPeek(sb), "") == 0, "Content should be empty");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_dump(void)
{
    _dString_t* sb = create_test_builder();
    const char* test_content = "Test dump content";

    // Add content
    d_StringAddStr(sb, test_content, 0);

    // Test dump with length
    size_t dump_len;
    char* dumped = d_StringDump(sb, &dump_len);

    TEST_ASSERT(dumped != NULL, "Dump should return valid pointer");
    TEST_ASSERT(dump_len == strlen(test_content), "Dump length should match content length");
    TEST_ASSERT(strcmp(dumped, test_content) == 0, "Dumped content should match original");

    // Verify original is unchanged
    TEST_ASSERT(strcmp(d_StringPeek(sb), test_content) == 0, "Original should be unchanged after dump");

    free(dumped);

    // Test dump without length parameter
    char* dumped2 = d_StringDump(sb, NULL);
    TEST_ASSERT(dumped2 != NULL, "Dump without length should work");
    TEST_ASSERT(strcmp(dumped2, test_content) == 0, "Dumped content should match without length param");

    free(dumped2);
    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_growth(void)
{
    _dString_t* sb = create_test_builder();

    // Add content that will definitely exceed initial capacity (32 bytes)
    const char* long_string = "This is a very long string that should exceed the initial capacity of 32 bytes and force the string builder to grow its internal buffer";

    d_StringAddStr(sb, long_string, 0);
    TEST_ASSERT(d_StringLen(sb) == strlen(long_string), "Length should match long string");
    TEST_ASSERT(strcmp(d_StringPeek(sb), long_string) == 0, "Content should match long string");

    // Add more content to test multiple growths
    for (int i = 0; i < 10; i++) {
        d_StringAddStr(sb, " Additional", 0);
    }

    // Verify it still works after multiple growths
    TEST_ASSERT(d_StringLen(sb) == strlen(long_string) + (10 * 11), "Length should include all additions");

    d_StringDestroy(sb);
    return 1;
}

int test_string_builder_null_safety(void)
{
    // Test all functions with NULL parameters (should not crash)
    d_StringDestroy(NULL);
    d_StringAddStr(NULL, "test", 0);
    d_StringAddChar(NULL, 'a');
    d_StringAddInt(NULL, 123);
    d_StringClear(NULL);
    d_StringTruncate(NULL, 5);
    d_StringDrop(NULL, 5);

    TEST_ASSERT(d_StringLen(NULL) == 0, "Len with NULL should return 0");
    TEST_ASSERT(d_StringPeek(NULL) == NULL, "Peek with NULL should return NULL");
    TEST_ASSERT(d_StringDump(NULL, NULL) == NULL, "Dump with NULL should return NULL");

    // Test adding NULL string
    _dString_t* sb = create_test_builder();
    d_StringAddStr(sb, NULL, 0);
    TEST_ASSERT(d_StringLen(sb) == 0, "Adding NULL string should not change length");

    d_StringDestroy(sb);
    return 1;
}

// Main test runner
int main(void)
{
    TEST_SUITE_START("String Builder Tests");

    RUN_TEST(test_string_builder_create_destroy);
    RUN_TEST(test_string_builder_add_string);
    RUN_TEST(test_string_builder_add_char);
    RUN_TEST(test_string_builder_add_int);
    RUN_TEST(test_string_builder_clear);
    RUN_TEST(test_string_builder_truncate);
    RUN_TEST(test_string_builder_drop);
    RUN_TEST(test_string_builder_dump);
    RUN_TEST(test_string_builder_growth);
    RUN_TEST(test_string_builder_null_safety);

    TEST_SUITE_END();
}
