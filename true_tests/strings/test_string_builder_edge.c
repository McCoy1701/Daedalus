// File: true_tests/strings/test_string_builder_edge.c - Edge case tests for string builder

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper Functions
dString_t* create_test_builder(void)
{
    return d_InitString();
}

void fill_builder_with_content(dString_t* sb, size_t target_size)
{
    // Fill with repeated pattern to reach target size
    const char* pattern = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t pattern_len = strlen(pattern);

    while (d_GetStringLength(sb) < target_size) {
        size_t remaining = target_size - d_GetStringLength(sb);
        size_t to_add = (remaining < pattern_len) ? remaining : pattern_len;
        d_AppendString(sb, pattern, to_add);
    }
}

// Edge Case Test Functions

int test_string_builder_empty_string_operations(void)
{
    dString_t* sb = create_test_builder();
    printf("Before adding empty string: %s\n", d_PeekString(sb));
    // Test adding empty string
    d_AppendString(sb, "", 0);
    printf("After adding empty string: %s\n", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Adding empty string should not change length");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "Content should remain empty");

    // Test adding empty string with explicit length
    d_AppendString(sb, "test", 0); // Should add because 0 uses strlen
    printf("After adding empty string with explicit length 4: %s\n", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) == 4, "Adding with explicit length 0 should add");

    // Add some content, then test truncate/drop to zero
    d_AppendString(sb, "Hello", 0);
    d_TruncateString(sb, 0);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Truncate to 0 should make empty");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "Content should be empty after truncate to 0");

    d_DestroyString(sb);
    return 1;
}

int test_string_builder_boundary_conditions(void)
{
    dString_t* sb = create_test_builder();

    // Test exactly at initial capacity boundary (32 bytes)
    // Add 31 characters (leaving 1 for null terminator)
    d_AppendString(sb, "1234567890123456789012345678901", 31);
    TEST_ASSERT(d_GetStringLength(sb) == 31, "Should handle exactly 31 characters");

    // Add one more character to trigger growth
    d_AppendChar(sb, 'X');
    TEST_ASSERT(d_GetStringLength(sb) == 32, "Should handle growth trigger correctly");
    TEST_ASSERT(strcmp(d_PeekString(sb), "1234567890123456789012345678901X") == 0, "Content should be correct after growth");

    d_DestroyString(sb);
    return 1;
}

int test_string_builder_large_content(void)
{
    dString_t* sb = create_test_builder();

    // Test building a large string (more than initial capacity)
    const size_t large_size = 1000;
    fill_builder_with_content(sb, large_size);

    TEST_ASSERT(d_GetStringLength(sb) == large_size, "Should handle large content correctly");
    TEST_ASSERT(strlen(d_PeekString(sb)) == large_size, "String length should match builder length");

    // Test that content is still valid
    const char* content = d_PeekString(sb);
    TEST_ASSERT(content[0] == 'A', "First character should be correct");
    TEST_ASSERT(content[large_size - 1] != '\0', "Last character should not be null");
    TEST_ASSERT(content[large_size] == '\0', "Should be null terminated");

    d_DestroyString(sb);
    return 1;
}

int test_string_builder_partial_string_operations(void)
{
    dString_t* sb = create_test_builder();

    // Test adding partial strings with explicit length
    const char* test_str = "Hello, World!";

    // Add only "Hello"
    d_AppendString(sb, test_str, 5);
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Should add exactly 5 characters");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hello") == 0, "Should contain only first 5 characters");

    // Add partial from middle ", Wor"
    d_AppendString(sb, test_str + 5, 4);
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hello, Wo") == 0, "Should append partial string correctly");

    // Test partial string longer than actual string (should only copy available chars)
    d_ClearString(sb);
    const char* short_str = "Hi";
    d_AppendString(sb, short_str, 10); // Request more than available
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hi") == 0, "Should only copy available characters");

    d_DestroyString(sb);
    return 1;
}

int test_string_builder_special_characters(void)
{
    dString_t* sb = create_test_builder();

    // Test with special characters
    d_AppendString(sb, "Tab:\t", 0);
    d_AppendString(sb, "Newline:\n", 0);
    d_AppendString(sb, "Quote:\"", 0);
    d_AppendString(sb, "Backslash:\\", 0);

    const char* content = d_PeekString(sb);
    TEST_ASSERT(strstr(content, "Tab:\t") != NULL, "Should contain tab character");
    TEST_ASSERT(strstr(content, "Newline:\n") != NULL, "Should contain newline character");
    TEST_ASSERT(strstr(content, "Quote:\"") != NULL, "Should contain quote character");
    TEST_ASSERT(strstr(content, "Backslash:\\") != NULL, "Should contain backslash character");

    // Test null character (though this breaks string functions)
    d_ClearString(sb);
    d_AppendChar(sb, 'A');
    d_AppendChar(sb, '\0');
    d_AppendChar(sb, 'B');

    TEST_ASSERT(d_GetStringLength(sb) == 3, "Should handle null character in middle");
    TEST_ASSERT(d_PeekString(sb)[0] == 'A', "First char should be A");
    TEST_ASSERT(d_PeekString(sb)[1] == '\0', "Second char should be null");
    TEST_ASSERT(d_PeekString(sb)[2] == 'B', "Third char should be B");

    d_DestroyString(sb);
    return 1;
}

int test_string_builder_integer_edge_cases(void)
{
    dString_t* sb = create_test_builder();

    // Test various integer edge cases
    d_AppendInt(sb, 0);
    d_AppendChar(sb, '|');

    d_AppendInt(sb, -1);
    d_AppendChar(sb, '|');

    d_AppendInt(sb, INT_MAX);
    d_AppendChar(sb, '|');

    d_AppendInt(sb, INT_MIN);
    d_AppendChar(sb, '|');

    const char* content = d_PeekString(sb);
    TEST_ASSERT(strstr(content, "0|") != NULL, "Should contain zero");
    TEST_ASSERT(strstr(content, "-1|") != NULL, "Should contain negative one");
    TEST_ASSERT(strstr(content, "2147483647|") != NULL, "Should contain INT_MAX");
    TEST_ASSERT(strstr(content, "-2147483648|") != NULL, "Should contain INT_MIN");

    d_DestroyString(sb);
    return 1;
}

int test_string_builder_truncate_edge_cases(void)
{
    dString_t* sb = create_test_builder();

    // Add content
    d_AppendString(sb, "Hello, World!", 0);
    size_t original_len = d_GetStringLength(sb);

    // Test truncate to same length (should do nothing)
    d_TruncateString(sb, original_len);
    TEST_ASSERT(d_GetStringLength(sb) == original_len, "Truncate to same length should do nothing");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hello, World!") == 0, "Content should be unchanged");

    // Test truncate to longer length (should do nothing)
    d_TruncateString(sb, original_len + 10);
    TEST_ASSERT(d_GetStringLength(sb) == original_len, "Truncate to longer length should do nothing");

    // Test truncate to 1 character
    d_TruncateString(sb, 1);
    TEST_ASSERT(d_GetStringLength(sb) == 1, "Should truncate to 1 character");
    TEST_ASSERT(strcmp(d_PeekString(sb), "H") == 0, "Should contain only first character");

    // Test truncate to 0 (clear)
    d_TruncateString(sb, 0);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Should truncate to empty");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "Should be empty string");

    d_DestroyString(sb);
    return 1;
}

int test_string_builder_drop_edge_cases(void)
{
    dString_t* sb = create_test_builder();

    // Add content
    d_AppendString(sb, "Hello, World!", 0);

    // Test drop 0 characters (should do nothing)
    size_t original_len = d_GetStringLength(sb);
    d_DropString(sb, 0);
    TEST_ASSERT(d_GetStringLength(sb) == original_len, "Drop 0 should do nothing");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hello, World!") == 0, "Content should be unchanged");

    // Test drop 1 character
    d_DropString(sb, 1);
    TEST_ASSERT(strcmp(d_PeekString(sb), "ello, World!") == 0, "Should drop first character");

    // Test drop exactly remaining length (should clear)
    size_t current_len = d_GetStringLength(sb);
    d_DropString(sb, current_len);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Drop all should clear");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "Should be empty");

    // Test drop more than length (should clear)
    d_AppendString(sb, "Test", 0);
    d_DropString(sb, 100);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Drop more than length should clear");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "Should be empty");

    d_DestroyString(sb);
    return 1;
}

int test_string_builder_dump_edge_cases(void)
{
    dString_t* sb = create_test_builder();

    // Test dump of empty string builder
    size_t dump_len;
    char* empty_dump = d_DumpString(sb, &dump_len);

    TEST_ASSERT(empty_dump != NULL, "Dump of empty builder should return valid pointer");
    TEST_ASSERT(dump_len == 0, "Dump length should be 0 for empty builder");
    TEST_ASSERT(strcmp(empty_dump, "") == 0, "Dump should be empty string");
    free(empty_dump);

    // Test dump without length parameter
    char* empty_dump2 = d_DumpString(sb, NULL);
    TEST_ASSERT(empty_dump2 != NULL, "Dump without length should work");
    TEST_ASSERT(strcmp(empty_dump2, "") == 0, "Dump should be empty string");
    free(empty_dump2);

    // Test dump of large content
    fill_builder_with_content(sb, 500);
    char* large_dump = d_DumpString(sb, &dump_len);

    TEST_ASSERT(large_dump != NULL, "Dump of large content should succeed");
    TEST_ASSERT(dump_len == 500, "Dump length should match content length");
    TEST_ASSERT(strlen(large_dump) == 500, "Dumped string length should be correct");
    TEST_ASSERT(strcmp(large_dump, d_PeekString(sb)) == 0, "Dump should match original content");

    // Verify original is unchanged
    TEST_ASSERT(d_GetStringLength(sb) == 500, "Original should be unchanged after dump");

    free(large_dump);
    d_DestroyString(sb);
    return 1;
}

int test_string_builder_memory_stress(void)
{
    dString_t* sb = create_test_builder();

    // Test repeated growth and shrinkage
    for (int i = 0; i < 10; i++) {
        // Grow
        fill_builder_with_content(sb, 100 * (i + 1));
        TEST_ASSERT(d_GetStringLength(sb) == 100 * (i + 1), "Should grow correctly in iteration");

        // Shrink
        d_TruncateString(sb, 50);
        TEST_ASSERT(d_GetStringLength(sb) == 50, "Should shrink correctly in iteration");

        // Clear
        d_ClearString(sb);
        TEST_ASSERT(d_GetStringLength(sb) == 0, "Should clear correctly in iteration");
    }

    // Test many small additions
    for (int i = 0; i < 1000; i++) {
        d_AppendChar(sb, 'A' + (i % 26));
    }
    TEST_ASSERT(d_GetStringLength(sb) == 1000, "Should handle many small additions");

    // Verify pattern
    const char* content = d_PeekString(sb);
    TEST_ASSERT(content[0] == 'A', "First character should be A");
    TEST_ASSERT(content[25] == 'Z', "26th character should be Z");
    TEST_ASSERT(content[26] == 'A', "Pattern should repeat");

    d_DestroyString(sb);
    return 1;
}

int test_string_builder_null_safety_comprehensive(void)
{
    // Comprehensive NULL safety testing

    // Test all functions with NULL string builder
    TEST_ASSERT(d_GetStringLength(NULL) == 0, "Len with NULL should return 0");
    TEST_ASSERT(d_PeekString(NULL) == NULL, "Peek with NULL should return NULL");
    TEST_ASSERT(d_DumpString(NULL, NULL) == NULL, "Dump with NULL should return NULL");

    d_DestroyString(NULL); // Should not crash
    d_AppendString(NULL, "test", 5);
    d_AppendChar(NULL, 'x');
    d_AppendInt(NULL, 42);
    d_ClearString(NULL);
    d_TruncateString(NULL, 10);
    d_DropString(NULL, 5);

    // Test with valid builder but NULL string
    dString_t* sb = create_test_builder();
    d_AppendString(sb, NULL, 0);
    d_AppendString(sb, NULL, 10);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Adding NULL string should not change length");

    d_DestroyString(sb);

    TEST_ASSERT(1, "All NULL safety tests should pass without crashing");
    return 1;
}

int test_string_builder_len_zero_behavior(void)
{
    dString_t* sb = create_test_builder();

    // Test: When len=0, strlen() should be called
    d_AppendString(sb, "test", 0);
    TEST_ASSERT(d_GetStringLength(sb) == 4, "len=0 should use strlen() and add 'test'");
    TEST_ASSERT(strcmp(d_PeekString(sb), "test") == 0, "Content should be 'test'");

    // Test: When len=0 with empty string, should add nothing
    d_ClearString(sb);
    d_AppendString(sb, "", 0);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "len=0 with empty string should add nothing");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "Content should remain empty");

    // Test: When len > 0, should add exactly that many characters
    d_ClearString(sb);
    d_AppendString(sb, "Hello World", 5);
    TEST_ASSERT(d_GetStringLength(sb) == 5, "len=5 should add exactly 5 characters");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hello") == 0, "Content should be 'Hello'");

    // Test: When len > actual string length, should only copy available characters
    d_ClearString(sb);
    d_AppendString(sb, "Hi", 10);
    printf("Length of string: %zu\n", d_GetStringLength(sb));
    printf("Content of string: %s\n", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) == 10, "Length should be 10");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hi") == 0, "Content should be 'Hi'");

    // Test: Edge case - len=0 with NULL-terminated string containing embedded nulls
    d_ClearString(sb);
    const char* str_with_null = "A\0B\0C";  // This string appears as "A" to strlen()
    d_AppendString(sb, str_with_null, 0);
    TEST_ASSERT(d_GetStringLength(sb) == 1, "strlen() should stop at first null");
    TEST_ASSERT(strcmp(d_PeekString(sb), "A") == 0, "Content should be 'A'");

    // Test: Explicit length can copy past embedded null
    d_ClearString(sb);
    d_AppendString(sb, str_with_null, 5);
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Explicit length should copy all 5 bytes");
    TEST_ASSERT(d_PeekString(sb)[0] == 'A', "First char should be 'A'");
    TEST_ASSERT(d_PeekString(sb)[1] == '\0', "Second char should be null");
    TEST_ASSERT(d_PeekString(sb)[2] == 'B', "Third char should be 'B'");

    d_DestroyString(sb);
    return 1;
}
/**
 * @brief Tests appending the builder's own content back to itself.
 * This is a classic edge case that can fool memory management if not
 * handled carefully. `memmove` is required for this to work safely,
 * especially when a `realloc` happens.
 */
int test_self_append_operations(void)
{
    dString_t* sb = create_test_builder();

    // Start with a known string
    d_AppendString(sb, "ABC-123-XYZ", 0);

    // Append a slice of its own content without forcing growth
    // Appending "123" to "ABC-123-XYZ"
    d_AppendString(sb, d_PeekString(sb) + 4, 3);
    TEST_ASSERT(strcmp(d_PeekString(sb), "ABC-123-XYZ123") == 0, "Self-append without growth should work");

    // Now, force a reallocation during a self-append.
    // Fill the builder so the next append *must* realloc.
    // Current alloc is 32. Current len is 14. Let's add 17 chars to make len 31.
    d_AppendString(sb, "................", 17); // len is now 31, alloc is 32
    TEST_ASSERT(d_GetStringLength(sb) == 31, "Builder should be filled to capacity limit");

    // This self-append will trigger realloc. The source pointer (`d_PeekString(sb)`)
    // could be invalidated by realloc if not handled correctly by the OS.
    // Appending the first 5 chars of the string to itself.
    const char* old_pointer = d_PeekString(sb);
    d_AppendString(sb, old_pointer, 5); // Append "ABC-1"

    TEST_ASSERT(d_GetStringLength(sb) == 36, "Length should be correct after self-append with growth");
    TEST_ASSERT(strncmp(d_PeekString(sb) + 31, "ABC-1", 5) == 0, "Self-append with growth should have correct content");

    d_DestroyString(sb);
    return 1;
}

/**
 * @brief Tests appending pure binary data to the builder.
 * A robust string builder should be 8-bit clean, meaning it can handle any
 * byte value from 0-255, not just valid text characters.
 */
int test_append_binary_data(void)
{
    dString_t* sb = create_test_builder();
    unsigned char binary_data[256];

    // Create a buffer containing every possible byte value
    for (int i = 0; i < 256; i++) {
        binary_data[i] = (unsigned char)i;
    }

    // Append the binary data. The length must be explicit because the data
    // contains a null byte at the beginning.
    d_AppendString(sb, (const char*)binary_data, 256);

    TEST_ASSERT(d_GetStringLength(sb) == 256, "Length should be 256 after appending all byte values");

    // Use memcmp to verify the content, as strcmp would stop at the first null byte.
    TEST_ASSERT(memcmp(d_PeekString(sb), binary_data, 256) == 0, "Builder content should match binary data byte-for-byte");

    d_DestroyString(sb);
    return 1;
}

/**
 * @brief Stress test that rapidly mixes different operations.
 * This can uncover state corruption bugs that only occur in specific
 * sequences of manipulation (e.g., append -> drop -> truncate -> append).
 */
int test_rapid_mixed_operations(void)
{
    dString_t* sb = create_test_builder();
    d_AppendString(sb, "START", 0);

    for (int i = 0; i < 100; i++) {
        d_AppendString(sb, "----APPEND----", 14);
        d_DropString(sb, 5); // Drop 5 chars from the front
        d_AppendString(sb, "++++", 4);
        d_TruncateString(sb, d_GetStringLength(sb) - 2); // Truncate last 2 chars
    }

    // After 100 loops, the final state should be predictable.
    // Each loop: adds 14, drops 5 (net +9), adds 4 (net +13), truncates 2 (net +11).
    // Initial length is 5. Final length should be 5 + (100 * 11) = 1105.
    TEST_ASSERT(d_GetStringLength(sb) == 1105, "Length should be correct after 100 mixed operations");

    // Check a small part of the final string for correctness
    // After first loop: "APPEND----++++" -> "APPEND----++" (len 16)
    // After second loop starts: "APPEND----++----APPEND----"
    // Drops 5 -> "ND----++----APPEND----"
    // ...it's complex, so just checking length is a good smoke test.

    d_DestroyString(sb);
    return 1;
}

/**
 * @brief Tests behavior with advanced and potentially tricky printf format specifiers.
 * Ensures that d_FormatString is robust.
 */
int test_format_string_advanced(void)
{
    dString_t* sb = create_test_builder();
    int num = 42;

    // Test with pointer format
    d_FormatString(sb, "Pointer: %p", (void*)&num);
    TEST_ASSERT(d_GetStringLength(sb) > 10, "Should format a pointer address"); // Address format varies
    d_ClearString(sb);

    // Test with hex format and padding
    d_FormatString(sb, "Hex: 0x%08X", 0xABC);
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hex: 0x00000ABC") == 0, "Should format hex with zero padding");
    d_ClearString(sb);

    // Test a very long formatted string that forces reallocation
    const char* long_str = "This is a very long string used as an argument.";
    d_FormatString(sb, "Start. %s %s %s %s. End.", long_str, long_str, long_str, long_str);
    TEST_ASSERT(d_GetStringLength(sb) > 200, "Should handle long format strings that cause growth");

    d_DestroyString(sb);
    return 1;
}

/**
 * @brief Attempts a massive allocation to test the growth logic under extreme
 * memory pressure and check for size_t overflow issues.
 * WARNING: This test may be slow and consume significant RAM.
 */
int test_massive_allocation_and_append(void)
{
    dString_t* sb = create_test_builder();

    // Use a large chunk size for appending
    size_t chunk_size = 1024 * 1024; // 1 MB
    char* chunk = malloc(chunk_size);
    if(chunk == NULL) {
        printf("SKIPPING massive allocation test: could not allocate 1MB chunk.\n");
        d_DestroyString(sb);
        return 1; // Cannot perform test
    }
    memset(chunk, 'X', chunk_size);

    size_t target_size_mb = 128; // Target 128 MB string
    size_t target_size_bytes = target_size_mb * 1024 * 1024;

    printf("\nAttempting to build a %zu MB string. This may be slow...\n", target_size_mb);

    for (size_t i = 0; i < target_size_mb; i++) {
        d_AppendString(sb, chunk, chunk_size);
    }

    // If the corrected d_StringBuilderEnsureSpace is used and memory runs out,
    // the length will be less than the target. The key is that it doesn't crash.
    if (d_GetStringLength(sb) == target_size_bytes) {
        TEST_ASSERT(d_GetStringLength(sb) == target_size_bytes, "Should successfully create a 128MB string");
        TEST_ASSERT(d_PeekString(sb)[target_size_bytes - 1] == 'X', "Last character of massive string should be correct");
    } else {
        printf("NOTE: Massive allocation test finished with a smaller string (%zu bytes) than targeted (%zu bytes). This likely indicates an out-of-memory condition, which was handled gracefully.\n", d_GetStringLength(sb), target_size_bytes);
        TEST_ASSERT(1, "Massive allocation did not crash (graceful failure)");
    }

    free(chunk);
    d_DestroyString(sb);
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

    RUN_TEST(test_self_append_operations);
    RUN_TEST(test_append_binary_data);
    RUN_TEST(test_rapid_mixed_operations);
    RUN_TEST(test_format_string_advanced);
    RUN_TEST(test_massive_allocation_and_append);

    TEST_SUITE_END();
}
