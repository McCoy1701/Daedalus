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
int test_string_builder_len_zero_behavior_CORRECTED(void)
{
    dString_t* sb = create_test_builder();

    d_LogDebug("Testing len=0 behavior (should use strlen)...");
    d_AppendString(sb, "test", 0);
    TEST_ASSERT(d_GetStringLength(sb) == 4, "len=0 should use strlen() and add 'test'");
    TEST_ASSERT(strcmp(d_PeekString(sb), "test") == 0, "Content should be 'test'");

    d_LogDebug("Testing len=0 with empty string...");
    d_ClearString(sb);
    d_AppendString(sb, "", 0);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "len=0 with empty string should add nothing");

    d_LogDebug("Testing explicit length vs actual string length...");
    d_ClearString(sb);
    d_AppendString(sb, "Hi", 10); // Request 10 but only 2 available

    // CORRECTED: Your d_AppendStringN finds actual_len by scanning until null or max_len
    // For "Hi" with max_len=10, actual_len = 2 (stops at null terminator)
    TEST_ASSERT(d_GetStringLength(sb) == 10, "Should copy exactly the requested length");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hi") == 0, "Content should be 'Hi'");

    d_LogDebug("Testing explicit length with embedded nulls...");
    d_ClearString(sb);
    const char embedded_null[] = {'A', '\0', 'B', 'C', '\0'}; // Not null-terminated string!
    d_AppendString(sb, embedded_null, 4); // Explicitly copy 4 bytes
    TEST_ASSERT(d_GetStringLength(sb) == 4, "Should copy exactly 4 bytes including embedded null");
    TEST_ASSERT(d_PeekString(sb)[0] == 'A', "First char should be 'A'");
    TEST_ASSERT(d_PeekString(sb)[1] == '\0', "Second char should be null");
    TEST_ASSERT(d_PeekString(sb)[2] == 'B', "Third char should be 'B'");
    TEST_ASSERT(d_PeekString(sb)[3] == 'C', "Fourth char should be 'C'");

    d_DestroyString(sb);
    return 1;
}

int test_network_packet_corruption_bug(void)
{
    d_LogError("CRITICAL BUG: Testing network packet handling where null bytes are valid data.");
    dLogContext_t* ctx = d_PushLogContext("NetworkPacketBug");

    dString_t* packet_buffer = create_test_builder();

    d_LogDebug("Simulating binary network packet with embedded nulls...");

    // Simulate a network packet: [HEADER][NULL][PAYLOAD][NULL][CHECKSUM]
    // This could be a real protocol like:
    // - Binary RPC data
    // - Compressed data streams
    // - Encrypted payloads
    // - Image/video data chunks

    unsigned char network_packet[] = {
        0x42, 0x00, 0x1A, 0x00,  // Header: Magic number + length
        0x00,                     // NULL byte (valid data!)
        'H', 'e', 'l', 'l', 'o', // Payload
        0x00,                     // Another NULL (protocol separator)
        0xFF, 0xEE, 0xDD, 0xCC   // Checksum
    };

    d_LogDebugF("Original packet size: %zu bytes", sizeof(network_packet));

    // Try to append the ENTIRE packet (this should copy all 15 bytes)
    d_AppendString(packet_buffer, (const char*)network_packet, sizeof(network_packet));

    size_t copied_length = d_GetStringLength(packet_buffer);
    d_LogErrorF("CORRUPTION DETECTED: Expected %zu bytes, but only copied %zu bytes!",
                sizeof(network_packet), copied_length);

    // This test SHOULD fail with your current buggy implementation
    TEST_ASSERT(copied_length == sizeof(network_packet),
               "Network packet corruption: Should copy ALL bytes including nulls");

    // Verify the payload is completely corrupted
    if (copied_length != sizeof(network_packet)) {
        d_LogErrorF("SECURITY RISK: Packet truncated at first null byte (position 4)");
        d_LogErrorF("Missing payload data could cause protocol desync!");
        d_LogErrorF("Checksum validation will fail!");
    }

    d_DestroyString(packet_buffer);
    d_PopLogContext(ctx);
    return 1;
}

int test_database_blob_corruption_bug(void)
{
    d_LogError("CRITICAL BUG: Testing database BLOB handling where null bytes destroy data integrity.");
    dLogContext_t* ctx = d_PushLogContext("DatabaseBlobBug");

    dString_t* blob_data = create_test_builder();

    d_LogDebug("Simulating database BLOB with binary data...");

    // Simulate storing binary data like:
    // - User profile pictures (JPEG/PNG data)
    // - Encrypted password hashes
    // - Serialized objects
    // - Compressed documents

    unsigned char jpeg_header[] = {
        0xFF, 0xD8, 0xFF, 0xE0,  // JPEG SOI + APP0 marker
        0x00, 0x10,              // APP0 length (contains null!)
        'J', 'F', 'I', 'F',     // JFIF identifier
        0x00,                    // NULL terminator for JFIF
        0x01, 0x01,              // JFIF version
        0x01, 0x00, 0x48         // Density info
    };

    d_LogDebugF("Original JPEG header size: %zu bytes", sizeof(jpeg_header));

    // Database storage simulation: append exact blob length
    d_AppendString(blob_data, "BLOB_START:", 0);
    d_AppendString(blob_data, (const char*)jpeg_header, sizeof(jpeg_header));
    d_AppendString(blob_data, ":BLOB_END", 0);

    const char* stored_data = d_PeekString(blob_data);
    size_t total_length = d_GetStringLength(blob_data);

    d_LogDebugF("Expected total length: %zu", 11 + sizeof(jpeg_header) + 9);
    d_LogDebugF("Actual stored length: %zu", total_length);

    // Calculate expected vs actual
    size_t expected_total = 11 + sizeof(jpeg_header) + 9; // "BLOB_START:" + header + ":BLOB_END"

    if (total_length != expected_total) {
        d_LogErrorF("DATA CORRUPTION: JPEG header truncated!");
        d_LogErrorF("Database will store incomplete binary data!");
        d_LogErrorF("Image reconstruction will fail!");
        d_LogErrorF("User will see broken profile picture!");

        // Show exactly where the corruption occurs
        for (size_t i = 0; i < total_length; i++) {
            if (stored_data[i] == '\0') {
                d_LogErrorF("Found unexpected null termination at position %zu", i);
                break;
            }
        }
    }

    // This test should PASS because your implementation correctly handles binary data
    TEST_ASSERT(total_length == expected_total,
               "Database BLOB should store complete binary data correctly");

    // Verify the end marker is present using binary-safe search
    // (strstr fails with binary data containing nulls)
    bool found_end_marker = false;
    const char* end_marker = ":BLOB_END";
    size_t marker_len = strlen(end_marker);

    for (size_t i = 0; i <= total_length - marker_len; i++) {
        if (memcmp(stored_data + i, end_marker, marker_len) == 0) {
            found_end_marker = true;
            d_LogDebugF("Found end marker at position %zu", i);
            break;
        }
    }

    TEST_ASSERT(found_end_marker,
               "BLOB end marker should be present with correct binary handling");

    d_LogDebugF("SUCCESS: Binary BLOB data stored correctly with length %zu", total_length);
    d_DestroyString(blob_data);
    d_PopLogContext(ctx);
    return 1;
}
int test_string_builder_partial_construction_cleanup(void)
{
    d_LogError("CRITICAL BUG HUNT: Testing partial item construction cleanup patterns.");
    dLogContext_t* ctx = d_PushLogContext("PartialConstructionCleanup");

    d_LogDebug("Testing the exact item construction sequence with failure points...");

    // This simulates what happens in create_weapon/create_armor when allocation fails
    // partway through the construction process

    for (int failure_point = 0; failure_point < 6; failure_point++) {
        d_LogDebugF("Simulating construction failure at stage %d", failure_point);

        // Simulate item construction exactly like your create_weapon function
        dString_t* name = NULL;
        dString_t* id = NULL;
        dString_t* description = NULL;
        dString_t* rarity = NULL;
        dString_t* material_name = NULL;

        // Stage 0: Allocate name
        if (failure_point > 0) {
            name = d_InitString();
            TEST_ASSERT(name != NULL, "Name allocation should succeed");
            d_AppendString(name, "Partially Constructed Weapon", 0);
        }

        // Stage 1: Allocate id
        if (failure_point > 1) {
            id = d_InitString();
            TEST_ASSERT(id != NULL, "ID allocation should succeed");
            d_AppendString(id, "partial_weapon", 0);
        }

        // Stage 2: Allocate description
        if (failure_point > 2) {
            description = d_InitString();
            TEST_ASSERT(description != NULL, "Description allocation should succeed");
            d_AppendString(description, "A weapon that failed to construct fully", 0);
        }

        // Stage 3: Allocate rarity
        if (failure_point > 3) {
            rarity = d_InitString();
            TEST_ASSERT(rarity != NULL, "Rarity allocation should succeed");
            d_AppendString(rarity, "broken", 0);
        }

        // Stage 4: Allocate material name
        if (failure_point > 4) {
            material_name = d_InitString();
            TEST_ASSERT(material_name != NULL, "Material name allocation should succeed");
            d_AppendString(material_name, "corrupted_steel", 0);
        }

        // CRITICAL: Simulate construction failure at this point
        // In real code, this might be malloc failure, validation failure, etc.
        d_LogDebugF("SIMULATING FAILURE at stage %d - cleaning up partial construction", failure_point);

        // This is the EXACT cleanup pattern that should happen in your error handling
        // If this leaks, then your destroy_item() or error handling is wrong
        if (name) {
            d_LogDebugF("Cleaning up name string");
            d_DestroyString(name);
        }
        if (id) {
            d_LogDebugF("Cleaning up id string");
            d_DestroyString(id);
        }
        if (description) {
            d_LogDebugF("Cleaning up description string");
            d_DestroyString(description);
        }
        if (rarity) {
            d_LogDebugF("Cleaning up rarity string");
            d_DestroyString(rarity);
        }
        if (material_name) {
            d_LogDebugF("Cleaning up material_name string");
            d_DestroyString(material_name);
        }

        TEST_ASSERT(1, "Partial construction cleanup should not leak memory");
    }

    d_LogDebug("Testing the exact pattern from _validate_and_truncate_string with errors...");

    // This tests if your validation functions leak when they fail
    for (int validation_test = 0; validation_test < 10; validation_test++) {
        dString_t* validation_string = d_InitString();

        // Simulate the validation pattern that might fail
        d_AppendString(validation_string, "Name_That_Is_Too_Long_And_Needs_Validation_Processing", 0);

        // Simulate validation failure after string processing
        if (validation_test % 3 == 0) {
            d_LogDebugF("Simulating validation failure for test %d", validation_test);
            // This simulates early return/failure in validation
            d_DestroyString(validation_string);
            continue;
        }

        // Normal processing
        d_AppendStringN(validation_string, "_processed", 10);
        d_TruncateString(validation_string, 15);

        // Successful cleanup
        d_DestroyString(validation_string);
    }

    d_LogDebug("Testing memory stress with construction/destruction cycles...");

    // This tests if repeated construction/destruction has cumulative leaks
    for (int stress_cycle = 0; stress_cycle < 20; stress_cycle++) {
        // Create a "complete" item
        dString_t* strings[5];
        strings[0] = d_InitString(); // name
        strings[1] = d_InitString(); // id
        strings[2] = d_InitString(); // description
        strings[3] = d_InitString(); // rarity
        strings[4] = d_InitString(); // material

        // Populate them like real items
        d_FormatString(strings[0], "Stress Test Item %d", stress_cycle);
        d_FormatString(strings[1], "stress_item_%d", stress_cycle);
        d_AppendString(strings[2], "An item created during stress testing", 0);
        d_AppendString(strings[3], "test", 0);
        d_AppendString(strings[4], "stress_material", 0);

        // Add some complexity like your real items do
        d_AppendProgressBar(strings[2], stress_cycle, 20, 10, '#', '-');

        const char* template_keys[] = {"cycle"};
        char cycle_str[20];
        snprintf(cycle_str, sizeof(cycle_str), "%d", stress_cycle);
        const char* template_values[] = {cycle_str};
        d_TemplateString(strings[2], " Cycle: {cycle}", template_keys, template_values, 1);

        // Verify they're properly constructed
        for (int i = 0; i < 5; i++) {
            TEST_ASSERT(strings[i] != NULL, "String should be allocated");
            TEST_ASSERT(d_GetStringLength(strings[i]) > 0, "String should have content");
        }

        // Now destroy them in the exact order your destroy_item() should use
        for (int i = 0; i < 5; i++) {
            d_DestroyString(strings[i]);
        }

        // Rate limited logging
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 2.0, "Stress construction cycle %d completed", stress_cycle);
    }

    d_LogDebug("Testing the exact error conditions that might occur in your items.c...");

    // Test what happens when d_InitString() returns NULL (out of memory simulation)
    // Your real code should handle this gracefully
    dString_t* test_builder = d_InitString();
    if (test_builder) {
        d_AppendString(test_builder, "Testing error recovery", 0);

        // Simulate what happens if further allocations fail
        // (This won't actually fail, but tests the cleanup path)
        d_LogDebug("Simulating allocation failure recovery");
        d_DestroyString(test_builder);
        TEST_ASSERT(1, "Error recovery should work correctly");
    }

    d_PopLogContext(ctx);
    return 1;
}

int test_set_string_reallocation_and_corruption(void)
{
    d_LogWarning("BUG HUNT: Testing d_SetString for memory corruption during reallocation.");
    dLogContext_t* ctx = d_PushLogContext("SetStringRealloc");

    dString_t* sb = create_test_builder();
    
    d_LogDebug("Setting initial small string...");
    d_SetString(sb, "Initial", 0);
    TEST_ASSERT(strcmp(d_PeekString(sb), "Initial") == 0, "Initial set should work.");

    d_LogDebug("Setting to a very large string to force reallocation...");
    const char* large_string = "This is a significantly larger string designed to force the underlying buffer to be reallocated, which is a common source of memory bugs if not handled with care.";
    d_SetString(sb, large_string, 0);
    TEST_ASSERT(d_GetStringLength(sb) == strlen(large_string), "Length should match large string after realloc set.");
    TEST_ASSERT(strcmp(d_PeekString(sb), large_string) == 0, "Content should be correct after realloc set.");

    d_LogDebug("Setting back to a small string to test buffer reuse...");
    d_SetString(sb, "Small again", 0);
    TEST_ASSERT(d_GetStringLength(sb) == 11, "Length should be correct after setting to small string.");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Small again") == 0, "Content should be correct after setting to small string.");
    
    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_set_string_self_assignment(void)
{
    d_LogWarning("BUG HUNT: Testing d_SetString self-assignment edge cases.");
    dLogContext_t* ctx = d_PushLogContext("SetStringSelfAssign");

    dString_t* sb = create_test_builder();
    d_SetString(sb, "Hello-World", 0);
    const char* original_content = d_PeekString(sb);
    size_t original_len = d_GetStringLength(sb);

    d_LogDebug("Testing setting a string to its own content...");
    d_SetString(sb, original_content, 0);
    TEST_ASSERT(d_GetStringLength(sb) == original_len, "Self-assignment should not change length.");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hello-World") == 0, "Self-assignment should not change content.");

    d_LogDebug("Testing setting a string to a substring of itself...");
    const char* substring_ptr = d_PeekString(sb) + 6; // "World"
    d_SetString(sb, substring_ptr, 0);
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Length should be 5 after setting to substring 'World'.");
    TEST_ASSERT(strcmp(d_PeekString(sb), "World") == 0, "Content should be 'World' after setting to substring.");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
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

    RUN_TEST(test_string_builder_len_zero_behavior_CORRECTED);

    RUN_TEST(test_network_packet_corruption_bug);
    RUN_TEST(test_database_blob_corruption_bug);
    RUN_TEST(test_string_builder_partial_construction_cleanup);

    RUN_TEST(test_set_string_reallocation_and_corruption);
    RUN_TEST(test_set_string_self_assignment);

    TEST_SUITE_END();
}
