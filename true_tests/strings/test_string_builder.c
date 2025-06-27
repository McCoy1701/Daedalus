// File: true_tests/strings/test_string_builder.c - Divine String Builder Tests
// Enhanced with the full power of Daedalus Logging and Metis's Wisdom

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// HELPER FUNCTIONS WITH DIVINE LOGGING
// =============================================================================

dString_t* create_test_builder(void)
{
    dString_t* sb = d_InitString();
    d_LogIfF(sb == NULL, D_LOG_LEVEL_ERROR, "Failed to create test string builder");
    return sb;
}

// Helper for safe string comparison with logging
static bool safe_string_compare(const char* actual, const char* expected, const char* context)
{
    if (actual == NULL && expected == NULL) {
        d_LogDebugF("String comparison [%s]: Both strings are NULL (valid)", context);
        return true;
    }
    if (actual == NULL || expected == NULL) {
        d_LogWarningF("String comparison [%s]: One string is NULL (actual=%p, expected=%p)",
                      context, (void*)actual, (void*)expected);
        return false;
    }

    bool result = strcmp(actual, expected) == 0;
    d_LogIfF(!result, D_LOG_LEVEL_WARNING,
             "String mismatch [%s]: Expected '%s', got '%s'", context, expected, actual);
    return result;
}

// Helper for float comparison with tolerance
static bool float_equals(float a, float b, float tolerance)
{
    bool result = fabs(a - b) < tolerance;
    d_LogIfF(!result, D_LOG_LEVEL_DEBUG,
             "Float comparison failed: %.6f vs %.6f (diff=%.6f, tolerance=%.6f)",
             a, b, fabs(a - b), tolerance);
    return result;
}

// =============================================================================
// ENHANCED TEST FUNCTIONS WITH COMPREHENSIVE LOGGING
// =============================================================================

int test_string_builder_create_destroy(void)
{
    d_LogInfo("VERIFICATION: String builder creation and destruction integrity.");
    dLogContext_t* ctx = d_PushLogContext("CreateDestroy");

    d_LogDebug("Creating new string builder...");
    dString_t* sb = d_InitString();
    TEST_ASSERT(sb != NULL, "Should create string builder successfully");

    if (sb) {
        d_LogDebugF("String builder created: length=%zu, content='%s'",
                    d_GetStringLength(sb), d_PeekString(sb) ? d_PeekString(sb) : "NULL");
        TEST_ASSERT(d_GetStringLength(sb) == 0, "New string builder should have length 0");
        TEST_ASSERT(d_PeekString(sb) != NULL, "Peek should return valid pointer");
        TEST_ASSERT(safe_string_compare(d_PeekString(sb), "", "initial content"),
                   "New string builder should be empty string");
    }

    d_LogDebug("Testing normal destroy operation...");
    d_DestroyString(sb);
    TEST_ASSERT(1, "Destroy should not crash");

    d_LogDebug("Testing NULL safety for destroy operation...");
    d_DestroyString(NULL);
    TEST_ASSERT(1, "Destroy with NULL should not crash");

    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_add_string(void)
{
    d_LogInfo("VERIFICATION: String appending functionality and length calculations.");
    dLogContext_t* ctx = d_PushLogContext("AddString");

    dString_t* sb = create_test_builder();
    const char* test_str = "Hello, World!";

    d_LogDebugF("Appending string: '%s' (auto-length)", test_str);
    d_AppendString(sb, test_str, 0);
    TEST_ASSERT(d_GetStringLength(sb) == strlen(test_str), "Length should match added string");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), test_str, "first append"),
               "Content should match added string");

    d_LogDebug("Appending string with explicit length...");
    d_AppendString(sb, " Test", 5);
    TEST_ASSERT(d_GetStringLength(sb) == strlen(test_str) + 5, "Length should include second string");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hello, World! Test", "explicit length append"),
               "Content should be concatenated");

    d_LogDebug("Testing partial string append...");
    d_AppendString(sb, "12345", 3);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hello, World! Test123", "partial append"),
               "Should add only specified length");

    d_LogDebugF("Final string: '%s' (length=%zu)", d_PeekString(sb), d_GetStringLength(sb));
    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_add_string_n(void)
{
    d_LogInfo("VERIFICATION: Limited string appending with d_AppendStringN function.");
    dLogContext_t* ctx = d_PushLogContext("AddStringN");

    dString_t* sb = create_test_builder();
    const char* test_str = "Hello, World!";

    d_LogDebugF("Testing d_AppendStringN with limit less than string length...");
    d_AppendStringN(sb, test_str, 5);
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Length should be limited to max_len");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hello", "limited append"),
               "Content should be truncated to max_len");

    d_LogDebug("Testing d_AppendStringN with limit greater than string length...");
    d_ClearString(sb);
    d_AppendStringN(sb, "Hi", 10);
    TEST_ASSERT(d_GetStringLength(sb) == 2, "Length should be actual string length");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hi", "over-limit append"),
               "Content should be full string when limit exceeds length");

    d_LogDebug("Testing d_AppendStringN with zero limit...");
    size_t len_before = d_GetStringLength(sb);
    d_AppendStringN(sb, "test", 0);
    TEST_ASSERT(d_GetStringLength(sb) == len_before, "Zero limit should not change length");

    d_LogDebug("Testing d_AppendStringN with NULL string...");
    d_AppendStringN(sb, NULL, 5);
    TEST_ASSERT(d_GetStringLength(sb) == len_before, "NULL string should not change length");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_add_char(void)
{
    d_LogInfo("VERIFICATION: Single character append operations.");
    dLogContext_t* ctx = d_PushLogContext("AddChar");

    dString_t* sb = create_test_builder();

    d_LogDebug("Adding single character 'A'...");
    d_AppendChar(sb, 'A');
    TEST_ASSERT(d_GetStringLength(sb) == 1, "Length should be 1 after adding char");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "A", "single char"),
               "Content should be the added character");

    d_LogDebug("Adding multiple characters in sequence...");
    d_AppendChar(sb, 'B');
    d_AppendChar(sb, 'C');
    TEST_ASSERT(d_GetStringLength(sb) == 3, "Length should be 3 after adding 3 chars");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "ABC", "multiple chars"),
               "Content should be concatenated characters");

    d_LogDebug("Testing special characters and edge cases...");
    d_AppendChar(sb, '\0');  // Null character
    d_AppendChar(sb, '\n');  // Newline
    d_AppendChar(sb, '\t');  // Tab
    TEST_ASSERT(d_GetStringLength(sb) == 6, "Length should include special characters");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_add_int(void)
{
    d_LogInfo("VERIFICATION: Integer to string conversion functionality.");
    dLogContext_t* ctx = d_PushLogContext("AddInt");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing positive integer append...");
    d_AppendInt(sb, 123);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "123", "positive int"),
               "Should add positive integer correctly");

    d_LogDebug("Testing negative integer append...");
    d_AppendInt(sb, -456);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "123-456", "negative int"),
               "Should add negative integer correctly");

    d_LogDebug("Testing zero integer...");
    d_ClearString(sb);
    d_AppendInt(sb, 0);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "0", "zero int"),
               "Should add zero correctly");

    d_LogDebug("Testing extreme integer values...");
    d_ClearString(sb);
    d_AppendInt(sb, INT32_MAX);
    d_LogDebugF("INT32_MAX converted to: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Should handle maximum integer");

    d_ClearString(sb);
    d_AppendInt(sb, INT32_MIN);
    d_LogDebugF("INT32_MIN converted to: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Should handle minimum integer");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_add_float(void)
{
    d_LogInfo("VERIFICATION: Floating-point number formatting and precision control.");
    dLogContext_t* ctx = d_PushLogContext("AddFloat");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing default precision (-1)...");
    d_AppendFloat(sb, 3.14159f, -1);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "3.141590", "default precision"),
               "Should add float with default precision");
    d_ClearString(sb);

    d_LogDebug("Testing specified precision with rounding...");
    d_AppendFloat(sb, 123.4567f, 2);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "123.46", "2 decimal places"),
               "Should add float with 2 decimal places (and round)");
    d_ClearString(sb);

    d_LogDebug("Testing zero precision with rounding...");
    d_AppendFloat(sb, 99.9f, 0);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "100", "0 decimal places"),
               "Should add float with 0 decimal places (and round)");
    d_ClearString(sb);

    d_LogDebug("Testing negative float values...");
    d_AppendFloat(sb, -42.789f, 2);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "-42.79", "negative float"),
               "Should handle negative floats correctly");
    d_ClearString(sb);

    d_LogDebug("Testing extreme precision values...");
    d_AppendFloat(sb, 1.23456789f, 10);  // Maximum precision
    d_LogDebugF("Maximum precision result: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Should handle maximum precision");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_clear(void)
{
    d_LogInfo("VERIFICATION: String builder content clearing functionality.");
    dLogContext_t* ctx = d_PushLogContext("Clear");

    dString_t* sb = create_test_builder();

    d_LogDebug("Adding content before clear test...");
    d_AppendString(sb, "Test content", 0);
    size_t len_before = d_GetStringLength(sb);
    TEST_ASSERT(len_before > 0, "Should have content before clear");
    d_LogDebugF("Content before clear: '%s' (length=%zu)", d_PeekString(sb), len_before);

    d_LogDebug("Clearing string builder...");
    d_ClearString(sb);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Length should be 0 after clear");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "", "after clear"),
               "Content should be empty after clear");

    d_LogDebug("Testing clear on already empty string...");
    d_ClearString(sb);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Should remain empty after clearing empty string");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_truncate(void)
{
    d_LogInfo("VERIFICATION: String truncation to specific lengths.");
    dLogContext_t* ctx = d_PushLogContext("Truncate");

    dString_t* sb = create_test_builder();

    d_LogDebug("Setting up content for truncation tests...");
    d_AppendString(sb, "Hello, World!", 0);
    d_LogDebugF("Initial content: '%s' (length=%zu)", d_PeekString(sb), d_GetStringLength(sb));

    d_LogDebug("Truncating to shorter length (5)...");
    d_TruncateString(sb, 5);
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Length should be 5 after truncate");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hello", "truncated content"),
               "Content should be truncated");

    d_LogDebug("Truncating to same length (no-op)...");
    d_TruncateString(sb, 5);
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Length should remain 5");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hello", "unchanged content"),
               "Content should remain same");

    d_LogDebug("Attempting to truncate to longer length (should be no-op)...");
    d_TruncateString(sb, 10);
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Length should remain 5 when truncating to longer");

    d_LogDebug("Truncating to zero length...");
    d_TruncateString(sb, 0);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Should be empty after truncating to 0");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "", "empty after truncate"),
               "Content should be empty");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_drop(void)
{
    d_LogInfo("VERIFICATION: String prefix removal functionality.");
    dLogContext_t* ctx = d_PushLogContext("Drop");

    dString_t* sb = create_test_builder();

    d_LogDebug("Setting up content for drop tests...");
    d_AppendString(sb, "Hello, World!", 0);
    d_LogDebugF("Initial content: '%s' (length=%zu)", d_PeekString(sb), d_GetStringLength(sb));

    d_LogDebug("Dropping 7 characters from beginning...");
    d_DropString(sb, 7); // Drop "Hello, "
    TEST_ASSERT(d_GetStringLength(sb) == 6, "Length should be 6 after dropping 7 chars");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "World!", "after drop"),
               "Content should have beginning dropped");

    d_LogDebug("Dropping more than remaining length...");
    d_DropString(sb, 10);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Should be empty after dropping more than length");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "", "empty after over-drop"),
               "Content should be empty");

    d_LogDebug("Testing drop on empty string...");
    d_DropString(sb, 5);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Should remain empty");

    d_LogDebug("Testing drop with zero length...");
    d_AppendString(sb, "Test", 0);
    size_t len_before = d_GetStringLength(sb);
    d_DropString(sb, 0);
    TEST_ASSERT(d_GetStringLength(sb) == len_before, "Zero drop should not change string");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_dump(void)
{
    d_LogInfo("VERIFICATION: String builder content extraction and copying.");
    dLogContext_t* ctx = d_PushLogContext("Dump");

    dString_t* sb = create_test_builder();
    const char* test_content = "Test dump content";

    d_LogDebugF("Adding test content: '%s'", test_content);
    d_AppendString(sb, test_content, 0);

    d_LogDebug("Testing dump with length parameter...");
    size_t dump_len;
    char* dumped = d_DumpString(sb, &dump_len);

    TEST_ASSERT(dumped != NULL, "Dump should return valid pointer");
    TEST_ASSERT(dump_len == strlen(test_content), "Dump length should match content length");
    TEST_ASSERT(safe_string_compare(dumped, test_content, "dumped content"),
               "Dumped content should match original");

    d_LogDebug("Verifying original string is unchanged...");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), test_content, "original after dump"),
               "Original should be unchanged after dump");

    free(dumped);

    d_LogDebug("Testing dump without length parameter...");
    char* dumped2 = d_DumpString(sb, NULL);
    TEST_ASSERT(dumped2 != NULL, "Dump without length should work");
    TEST_ASSERT(safe_string_compare(dumped2, test_content, "dumped without length"),
               "Dumped content should match without length param");

    free(dumped2);

    d_LogDebug("Testing dump with empty string...");
    d_ClearString(sb);
    char* empty_dump = d_DumpString(sb, NULL);
    TEST_ASSERT(empty_dump != NULL, "Should dump empty string");
    TEST_ASSERT(safe_string_compare(empty_dump, "", "empty dump"),
               "Empty dump should be empty string");
    free(empty_dump);

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_growth(void)
{
    d_LogInfo("VERIFICATION: Dynamic buffer growth and capacity management.");
    dLogContext_t* ctx = d_PushLogContext("Growth");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing growth with long string exceeding initial capacity...");
    const char* long_string = "This is a very long string that should exceed the initial capacity of 32 bytes and force the string builder to grow its internal buffer to accommodate the additional content";

    d_LogDebugF("Appending string of length %zu (exceeds 32 byte initial capacity)", strlen(long_string));
    d_AppendString(sb, long_string, 0);
    TEST_ASSERT(d_GetStringLength(sb) == strlen(long_string), "Length should match long string");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), long_string, "long string"),
               "Content should match long string");

    d_LogDebug("Testing multiple growth iterations...");
    size_t initial_len = d_GetStringLength(sb);
    for (int i = 0; i < 10; i++) {
        d_AppendString(sb, " Additional", 0);
        // Rate limit progress reporting
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 2.0, "Growth iteration %d completed, length now %zu", i + 1, d_GetStringLength(sb));
    }

    TEST_ASSERT(d_GetStringLength(sb) == initial_len + (10 * 11), "Length should include all additions");
    d_LogDebugF("Final length: %zu characters", d_GetStringLength(sb));

    d_LogDebug("Testing massive growth scenario...");
    d_ClearString(sb);
    for (int i = 0; i < 100; i++) {
        d_AppendString(sb, "Growth test string with moderate length to force multiple reallocations", 0);
    }
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Should handle massive growth");
    d_LogDebugF("Massive growth test completed: %zu characters", d_GetStringLength(sb));

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_null_safety(void)
{
    d_LogInfo("VERIFICATION: NULL pointer safety across all string builder functions.");
    dLogContext_t* ctx = d_PushLogContext("NullSafety");

    d_LogDebug("Testing all functions with NULL string builder parameter...");
    // These should not crash
    d_DestroyString(NULL);
    d_AppendString(NULL, "test", 0);
    d_AppendStringN(NULL, "test", 4);
    d_AppendChar(NULL, 'a');
    d_AppendInt(NULL, 123);
    d_AppendFloat(NULL, 3.14f, 2);
    d_ClearString(NULL);
    d_TruncateString(NULL, 5);
    d_DropString(NULL, 5);
    d_FormatString(NULL, "test %d", 42);
    d_RepeatString(NULL, 'x', 5);
    d_AppendProgressBar(NULL, 50, 100, 10, '#', '-');

    TEST_ASSERT(d_GetStringLength(NULL) == 0, "GetStringLength with NULL should return 0");
    TEST_ASSERT(d_PeekString(NULL) == NULL, "PeekString with NULL should return NULL");
    TEST_ASSERT(d_DumpString(NULL, NULL) == NULL, "DumpString with NULL should return NULL");

    d_LogDebug("Testing functions with NULL string parameters...");
    dString_t* sb = create_test_builder();
    size_t len_before = d_GetStringLength(sb);

    d_AppendString(sb, NULL, 0);
    TEST_ASSERT(d_GetStringLength(sb) == len_before, "Adding NULL string should not change length");

    d_AppendStringN(sb, NULL, 5);
    TEST_ASSERT(d_GetStringLength(sb) == len_before, "Adding NULL string with N should not change length");

    d_FormatString(sb, NULL);
    TEST_ASSERT(d_GetStringLength(sb) == len_before, "Format with NULL format should not change length");

    d_LogDebug("Testing template functions with NULL arrays...");
    d_TemplateString(sb, "Hello {name}", NULL, NULL, 1);
    TEST_ASSERT(d_GetStringLength(sb) >= len_before, "Template with NULL arrays should be safe");

    d_JoinStrings(sb, NULL, 5, ", ");
    TEST_ASSERT(d_GetStringLength(sb) >= len_before, "Join with NULL array should be safe");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// ADVANCED EDGE CASE TESTS
// =============================================================================

int test_string_builder_self_append(void)
{
    d_LogWarning("BUG HUNT: Testing self-append scenarios that could cause memory corruption.");
    dLogContext_t* ctx = d_PushLogContext("SelfAppend");

    dString_t* sb = create_test_builder();
    d_AppendString(sb, "Hello", 0);

    d_LogDebug("Testing self-append of entire string...");
    const char* self_ptr = d_PeekString(sb);
    d_AppendString(sb, self_ptr, 0);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "HelloHello", "self append"),
               "Self-append should work correctly");

    d_LogDebug("Testing self-append of substring...");
    d_ClearString(sb);
    d_AppendString(sb, "ABCDEF", 0);
    const char* substr_ptr = d_PeekString(sb) + 2; // Points to "CDEF"
    d_AppendString(sb, substr_ptr, 0);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "ABCDEFCDEF", "self substring append"),
               "Self-substring append should work correctly");

    d_LogDebug("Testing self-append with length limit...");
    d_ClearString(sb);
    d_AppendString(sb, "Test", 0);
    d_AppendString(sb, d_PeekString(sb), 2); // Append first 2 chars
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "TestTe", "self append with limit"),
               "Self-append with length should work correctly");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_memory_stress(void)
{
    d_LogWarning("BUG HUNT: Memory allocation stress testing.");
    dLogContext_t* ctx = d_PushLogContext("MemoryStress");

    d_LogDebug("Creating multiple string builders simultaneously...");
    dString_t* builders[10];
    for (int i = 0; i < 10; i++) {
        builders[i] = create_test_builder();
        TEST_ASSERT(builders[i] != NULL, "Should create multiple builders");
        if (builders[i]) {
            d_FormatString(builders[i], "Builder %d with content", i);
        }
    }

    d_LogDebug("Testing rapid allocation/deallocation cycles...");
    for (int cycle = 0; cycle < 50; cycle++) {
        dString_t* temp = create_test_builder();
        if (temp) {
            d_AppendString(temp, "Temporary string for stress testing", 0);
            d_DestroyString(temp);
        }

        // Rate limited progress reporting
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 3.0, "Stress cycle %d completed", cycle);
    }

    d_LogDebug("Cleaning up stress test builders...");
    for (int i = 0; i < 10; i++) {
        if (builders[i]) {
            TEST_ASSERT(d_GetStringLength(builders[i]) > 0, "Builders should retain content");
            d_DestroyString(builders[i]);
        }
    }

    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_format_advanced(void)
{
    d_LogInfo("VERIFICATION: Advanced printf-style formatting scenarios.");
    dLogContext_t* ctx = d_PushLogContext("FormatAdvanced");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing complex format specifiers...");
    d_FormatString(sb, "Int: %d, Hex: %x, Float: %.2f, String: %s", 42, 255, 3.14159f, "test");
    d_LogDebugF("Complex format result: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Complex format should produce output");

    d_LogDebug("Testing format with escaped percent signs...");
    d_ClearString(sb);
    d_FormatString(sb, "Progress: 50%% complete (that's %d out of %d)", 5, 10);
    TEST_ASSERT(strstr(d_PeekString(sb), "50% complete") != NULL, "Should handle escaped percent");

    d_LogDebug("Testing format with very long output...");
    d_ClearString(sb);
    d_FormatString(sb, "Long format test: %s %s %s",
                   "This is a very long string that will be repeated multiple times",
                   "to test the formatting system's ability to handle large outputs",
                   "and ensure proper memory allocation during the formatting process");
    TEST_ASSERT(d_GetStringLength(sb) > 100, "Long format should produce substantial output");

    d_LogDebug("Testing format appending to existing content...");
    d_ClearString(sb);
    d_AppendString(sb, "Prefix: ", 0);
    d_FormatString(sb, "Value = %d", 123);
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Prefix: Value = 123", "format append"),
               "Format should append to existing content");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_template_advanced(void)
{
    d_LogInfo("VERIFICATION: Advanced template substitution scenarios.");
    dLogContext_t* ctx = d_PushLogContext("TemplateAdvanced");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing template with missing keys...");
    const char* keys[] = {"name", "version"};
    const char* values[] = {"Daedalus", "2.0"};
    d_TemplateString(sb, "System: {name} v{version}, Status: {status}, Mode: {mode}",
                     keys, values, 2);

    const char* result = d_PeekString(sb);
    TEST_ASSERT(strstr(result, "Daedalus") != NULL, "Should substitute existing keys");
    TEST_ASSERT(strstr(result, "2.0") != NULL, "Should substitute version");
    TEST_ASSERT(strstr(result, "{status}") != NULL, "Should leave missing keys unchanged");
    d_LogDebugF("Template result: '%s'", result);

    d_LogDebug("Testing template with empty keys/values...");
    d_ClearString(sb);
    const char* empty_keys[] = {"", "test"};
    const char* empty_values[] = {"empty", "value"};
    d_TemplateString(sb, "Empty: {}, Test: {test}, Missing: {missing}",
                     empty_keys, empty_values, 2);
    d_LogDebugF("Empty key template: '%s'", d_PeekString(sb));

    d_LogDebug("Testing template with nested braces...");
    d_ClearString(sb);
    d_TemplateString(sb, "Code: {code}, JSON: {{\"key\": \"{value}\"}}",
                     (const char*[]){"code", "value"},
                     (const char*[]){"200", "success"}, 2);
    d_LogDebugF("Nested braces result: '%s'", d_PeekString(sb));

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_pythonic_advanced(void)
{
    d_LogInfo("VERIFICATION: Advanced Pythonic string operations.");
    dLogContext_t* ctx = d_PushLogContext("PythonicAdvanced");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing join with empty strings and NULL elements...");
    const char* mixed_items[] = {"apple", "", "banana", NULL, "cherry"};
    d_JoinStrings(sb, mixed_items, 5, " | ");
    d_LogDebugF("Mixed join result: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Should handle mixed array");

    d_LogDebug("Testing slice with complex boundary conditions...");
    d_ClearString(sb);
    const char* test_str = "Hello, World!";

    // Test various slice scenarios
    d_SliceString(sb, test_str, 0, 5);   // "Hello"
    d_AppendString(sb, " ", 0);
    d_SliceString(sb, test_str, 7, -1);  // "World!"
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hello World!", "complex slice"),
               "Complex slicing should work correctly");

    d_LogDebug("Testing slice with out-of-bounds indices...");
    d_ClearString(sb);
    d_SliceString(sb, "Test", -100, 100);  // Should clamp to full string
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Test", "clamped slice"),
               "Out-of-bounds slice should be clamped");

    d_LogDebug("Testing slice with invalid ranges...");
    d_ClearString(sb);
    d_SliceString(sb, "Test", 5, 2);  // Invalid range
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Invalid slice range should produce empty string");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_padding_advanced(void)
{
    d_LogInfo("VERIFICATION: Advanced text padding and alignment operations.");
    dLogContext_t* ctx = d_PushLogContext("PaddingAdvanced");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing padding with various characters...");
    d_PadLeftString(sb, "Test", 10, '.');
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "......Test", "left pad"),
               "Left padding should work correctly");

    d_ClearString(sb);
    d_PadRightString(sb, "Test", 10, '-');
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Test------", "right pad"),
               "Right padding should work correctly");

    d_ClearString(sb);
    d_PadCenterString(sb, "Hi", 8, '*');
    d_LogDebugF("Center padding result: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) == 8, "Center padding should produce correct length");

    d_LogDebug("Testing padding with text longer than width...");
    d_ClearString(sb);
    d_PadLeftString(sb, "VeryLongText", 5, '.');
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "VeryLongText", "no pad when too long"),
               "Should not pad when text is already longer than width");

    d_LogDebug("Testing progress bar generation...");
    d_ClearString(sb);
    d_AppendProgressBar(sb, 7, 10, 20, '#', '-');
    d_LogDebugF("Progress bar: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) == 22, "Progress bar should be correct length (width + 2 brackets)");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_append_n_truncation_basic(void)
{
    d_LogInfo("VERIFICATION: d_AppendStringN basic truncation scenarios.");
    dLogContext_t* ctx = d_PushLogContext("AppendN_BasicTrunc");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing truncation of long string to exact limit...");
    const char* long_text = "This is a very long string that will be truncated";
    d_AppendStringN(sb, long_text, 10);
    TEST_ASSERT(d_GetStringLength(sb) == 10, "Length should be exactly 10");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "This is a ", "truncated to 10 chars"),
               "Should truncate to exactly 10 characters");

    d_LogDebug("Testing sequential truncated appends...");
    d_AppendStringN(sb, "ABCDEFGHIJK", 3); // Only append "ABC"
    TEST_ASSERT(d_GetStringLength(sb) == 13, "Length should be 13 after second append");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "This is a ABC", "sequential truncation"),
               "Should append truncated portion to existing content");

    d_LogDebug("Testing truncation with exact string length...");
    d_ClearString(sb);
    d_AppendStringN(sb, "Hello", 5); // Exact length
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Length should be 5 for exact match");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hello", "exact length"),
               "Should append entire string when limit equals length");

    d_LogDebug("Testing single character truncation...");
    d_ClearString(sb);
    d_AppendStringN(sb, "ABCDEFG", 1);
    TEST_ASSERT(d_GetStringLength(sb) == 1, "Length should be 1 for single char");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "A", "single char truncation"),
               "Should truncate to single character");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_builder_append_n_truncation_edge_cases(void)
{
    d_LogWarning("BUG HUNT: d_AppendStringN edge cases and boundary conditions.");
    dLogContext_t* ctx = d_PushLogContext("AppendN_EdgeCases");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing truncation with strings containing null bytes...");
    const char embedded_null[] = "Hello\0World"; // Contains embedded null
    d_AppendStringN(sb, embedded_null, 10); // Try to append 10 chars
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Should stop at first null byte");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hello", "null byte truncation"),
               "Should truncate at embedded null byte, not at limit");

    d_LogDebug("Testing truncation with empty string...");
    d_ClearString(sb);
    d_AppendStringN(sb, "", 5); // Try to append 5 chars from empty string
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Empty string should add nothing");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "", "empty string truncation"),
               "Should remain empty when truncating empty string");

    d_LogDebug("Testing truncation with very large limit...");
    d_ClearString(sb);
    const char* short_text = "Hi";
    d_AppendStringN(sb, short_text, 1000000); // Massive limit
    TEST_ASSERT(d_GetStringLength(sb) == 2, "Should use actual string length");
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "Hi", "large limit truncation"),
               "Should append entire string when limit exceeds length");

    d_LogDebug("Testing truncation with buffer growth scenario...");
    d_ClearString(sb);
    // First, fill buffer close to capacity
    d_AppendString(sb, "Initial content to fill buffer", 0);

    // Now append with truncation that might trigger reallocation
    const char* growth_text = "This is additional content that might trigger buffer growth during truncation";
    d_AppendStringN(sb, growth_text, 25); // Truncate to 25 chars

    const char* final_content = d_PeekString(sb);
    TEST_ASSERT(strstr(final_content, "Initial content to fill buffer") != NULL,
               "Should preserve original content");
    TEST_ASSERT(strstr(final_content, "This is additional conten") != NULL,
               "Should append exactly 25 truncated characters");
    TEST_ASSERT(d_GetStringLength(sb) == 30 + 25, "Total length should be original + 25");

    d_LogDebug("Testing self-truncation scenario...");
    d_ClearString(sb);
    d_AppendString(sb, "SelfTest", 0);
    const char* self_ptr = d_PeekString(sb) + 4; // Points to "Test"
    d_AppendStringN(sb, self_ptr, 2); // Append "Te" (truncated "Test")
    TEST_ASSERT(safe_string_compare(d_PeekString(sb), "SelfTestTe", "self truncation"),
               "Should handle self-truncation correctly");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER WITH COMPREHENSIVE LOGGING SETUP
// =============================================================================

int main(void)
{
    // =========================================================================
    // DAEDALUS LOGGER INITIALIZATION - DIVINE CONFIGURATION
    // =========================================================================
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .colorize_output = true,
        .include_timestamp = false,
        .include_file_info = true,   // Enable for debugging
        .include_function = true     // Enable for detailed context
    };

    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);
    d_AddLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler, NULL);

    d_LogInfo("Initializing MIDAS-Enhanced String Builder Test Suite");
    d_LogDebugF("Daedalus Logging System: %s", "ACTIVE");
    // =========================================================================

    TEST_SUITE_START("MIDAS Enhanced String Builder Tests");

    // Core functionality tests
    RUN_TEST(test_string_builder_create_destroy);
    RUN_TEST(test_string_builder_add_string);
    RUN_TEST(test_string_builder_add_string_n);
    RUN_TEST(test_string_builder_add_char);
    RUN_TEST(test_string_builder_add_int);
    RUN_TEST(test_string_builder_add_float);
    RUN_TEST(test_string_builder_clear);
    RUN_TEST(test_string_builder_truncate);
    RUN_TEST(test_string_builder_drop);
    RUN_TEST(test_string_builder_dump);
    RUN_TEST(test_string_builder_growth);
    RUN_TEST(test_string_builder_null_safety);

    // Advanced functionality tests
    RUN_TEST(test_string_builder_self_append);
    RUN_TEST(test_string_builder_memory_stress);
    RUN_TEST(test_string_builder_format_advanced);
    RUN_TEST(test_string_builder_template_advanced);
    RUN_TEST(test_string_builder_pythonic_advanced);
    RUN_TEST(test_string_builder_padding_advanced);

    RUN_TEST(test_string_builder_append_n_truncation_edge_cases);
    RUN_TEST(test_string_builder_append_n_truncation_basic);

    // =========================================================================
    // DAEDALUS LOGGER CLEANUP
    // =========================================================================
    d_LogInfo("String Builder Test Suite completed");
    d_DestroyLogger(d_GetGlobalLogger());
    // =========================================================================

    TEST_SUITE_END();
}
