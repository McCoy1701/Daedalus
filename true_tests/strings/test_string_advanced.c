// File: true_tests/strings/test_string_advanced.c - Tests for advanced string functions
#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper functions for creating detailed log messages with dString_t
void log_test_start(const char* test_name)
{
    dString_t* log_msg = d_InitString();
    d_FormatString(log_msg, "Starting test: %s", test_name);
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);
}

void log_test_result(const char* test_name, bool passed, const char* details)
{
    dString_t* log_msg = d_InitString();
    d_FormatString(log_msg, "Test %s: %s", test_name, passed ? "PASSED" : "FAILED");
    if (details) {
        d_AppendString(log_msg, " - ", 0);
        d_AppendString(log_msg, details, 0);
    }
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);
}

void log_string_comparison(const char* expected, const char* actual)
{
    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "String comparison:\n", 0);
    d_FormatString(log_msg, "  Expected: '%s'\n", expected);
    d_FormatString(log_msg, "  Actual:   '%s'", actual);
    printf("%s\n", d_PeekString(log_msg));
    d_DestroyString(log_msg);
}

void log_progress_bar_test(int current, int max, int width, const char* result)
{
    dString_t* log_msg = d_InitString();
    d_FormatString(log_msg, "Progress bar test: %d/%d (width=%d) -> %s",
                   current, max, width, result);
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);
}

// =============================================================================
// d_FormatString Tests
// =============================================================================

int test_string_format_basic(void)
{
    log_test_start("string_format_basic");

    dString_t* sb = d_InitString();
    TEST_ASSERT(sb != NULL, "Failed to create string builder");

    d_FormatString(sb, "Hello %s!", "World");
    const char* expected = "Hello World!";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("string_format_basic", true, "Basic string formatting works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_string_format_integers(void)
{
    log_test_start("string_format_integers");

    dString_t* sb = d_InitString();
    d_FormatString(sb, "Level %d character with %d health", 42, 100);

    const char* expected = "Level 42 character with 100 health";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("string_format_integers", true, "Integer formatting works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_string_format_append(void)
{
    log_test_start("string_format_append");

    dString_t* sb = d_InitString();
    d_AppendString(sb, "Start: ", 0);
    d_FormatString(sb, "Value=%d", 123);
    d_AppendString(sb, " End", 0);

    const char* expected = "Start: Value=123 End";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("string_format_append", true, "Format append works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_string_format_null_safety(void)
{
    log_test_start("string_format_null_safety");

    // Should not crash with NULL parameters
    d_FormatString(NULL, "test");
    LOG("NULL string builder handled safely");

    dString_t* sb = d_InitString();
    size_t initial_len = d_GetStringLength(sb);
    d_FormatString(sb, NULL);

    TEST_ASSERT(d_GetStringLength(sb) == initial_len, "Null format should not modify string");

    log_test_result("string_format_null_safety", true, "NULL parameters handled safely");
    d_DestroyString(sb);
    return 1;
}
int test_string_append_float(void)
{
    log_test_start("string_append_float");

    dString_t* sb = d_InitString();

    // Test with 2 decimal places
    d_AppendFloat(sb, 3.14159f, 2);
    const char* expected_2dp = "3.14";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected_2dp) != 0) {
        log_string_comparison(expected_2dp, actual);
        d_DestroyString(sb);
        return 0;
    }

    // Test with different decimal places
    d_ClearString(sb);
    d_AppendString(sb, "Pi is approximately: ", 0);
    d_AppendFloat(sb, 3.14159f, 4);
    const char* expected_pi = "Pi is approximately: 3.1416";
    actual = d_PeekString(sb);

    if (strcmp(actual, expected_pi) != 0) {
        log_string_comparison(expected_pi, actual);
        d_DestroyString(sb);
        return 0;
    }

    // Test with 0 decimal places (integer-like)
    d_ClearString(sb);
    d_AppendFloat(sb, 42.7f, 0);
    const char* expected_int = "43";
    actual = d_PeekString(sb);

    if (strcmp(actual, expected_int) != 0) {
        log_string_comparison(expected_int, actual);
        d_DestroyString(sb);
        return 0;
    }

    // Test with negative decimals (should use default)
    d_ClearString(sb);
    d_AppendFloat(sb, 1.23456789f, -1);
    actual = d_PeekString(sb);

    dString_t* log_msg = d_InitString();
    d_FormatString(log_msg, "Float with negative decimals (-1): %s", actual);
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);

    // Test with excessive decimals (should be clamped to 10)
    d_ClearString(sb);
    d_AppendFloat(sb, 1.23456789012345f, 20);
    actual = d_PeekString(sb);

    log_msg = d_InitString();
    d_FormatString(log_msg, "Float with excessive decimals (20): %s", actual);
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);

    // Test with NULL safety
    d_AppendFloat(NULL, 3.14f, 2);
    LOG("NULL string builder handled safely");

    log_test_result("string_append_float", true, "Float appending works correctly with various precision settings");
    d_DestroyString(sb);
    return 1;
}
// =============================================================================
// d_AppendProgressBar Tests
// =============================================================================

int test_progress_bar_basic(void)
{
    log_test_start("progress_bar_basic");

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 50, 100, 10, '#', '-');

    const char* expected = "[#####-----]";
    const char* actual = d_PeekString(sb);

    log_progress_bar_test(50, 100, 10, actual);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("progress_bar_basic", true, "Basic progress bar rendered correctly");
    d_DestroyString(sb);
    return 1;
}

int test_progress_bar_full(void)
{
    log_test_start("progress_bar_full");

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 100, 100, 8, '=', '.');

    const char* expected = "[========]";
    const char* actual = d_PeekString(sb);

    log_progress_bar_test(100, 100, 8, actual);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("progress_bar_full", true, "Full progress bar rendered correctly");
    d_DestroyString(sb);
    return 1;
}

int test_progress_bar_empty(void)
{
    log_test_start("progress_bar_empty");

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 0, 100, 6, '*', ' ');

    const char* expected = "[      ]";
    const char* actual = d_PeekString(sb);

    log_progress_bar_test(0, 100, 6, actual);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("progress_bar_empty", true, "Empty progress bar rendered correctly");
    d_DestroyString(sb);
    return 1;
}

int test_progress_bar_overflow(void)
{
    log_test_start("progress_bar_overflow");

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 150, 100, 5, '+', '-');

    const char* expected = "[+++++]";
    const char* actual = d_PeekString(sb);

    log_progress_bar_test(150, 100, 5, actual);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("progress_bar_overflow", true, "Overflow progress bar handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_progress_bar_partial(void)
{
    log_test_start("progress_bar_partial");

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 33, 100, 12, '#', '-');

    // 33% of 12 = 3.96, should round down to 3
    const char* expected = "[###---------]";
    const char* actual = d_PeekString(sb);

    log_progress_bar_test(33, 100, 12, actual);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("progress_bar_partial", true, "Partial progress bar calculated correctly");
    d_DestroyString(sb);
    return 1;
}

int test_progress_bar_null_safety(void)
{
    log_test_start("progress_bar_null_safety");

    // Should not crash with NULL
    d_AppendProgressBar(NULL, 50, 100, 10, '#', '-');
    LOG("NULL string builder handled safely");

    dString_t* sb = d_InitString();
    size_t original_len = d_GetStringLength(sb);

    // Invalid parameters should not modify string
    d_AppendProgressBar(sb, 50, 0, 10, '#', '-');  // max = 0
    d_AppendProgressBar(sb, 50, 100, 0, '#', '-'); // width = 0
    d_AppendProgressBar(sb, 50, 100, -5, '#', '-'); // negative width

    dString_t* log_msg = d_InitString();
    d_FormatString(log_msg, "String length before: %zu, after: %zu",
                   original_len, d_GetStringLength(sb));
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);

    TEST_ASSERT(d_GetStringLength(sb) == original_len, "Invalid parameters should not modify string");

    log_test_result("progress_bar_null_safety", true, "NULL and invalid parameters handled safely");
    d_DestroyString(sb);
    return 1;
}

// =============================================================================
// d_TemplateString Tests
// =============================================================================

int test_template_basic(void)
{
    log_test_start("template_basic");

    dString_t* sb = d_InitString();
    const char* keys[] = {"name", "level"};
    const char* values[] = {"Alice", "10"};

    d_TemplateString(sb, "Hello {name}, you are level {level}!", keys, values, 2);

    const char* expected = "Hello Alice, you are level 10!";
    const char* actual = d_PeekString(sb);

    dString_t* log_msg = d_InitString();
    d_FormatString(log_msg, "Template with %d replacements: %s", 2, actual);
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("template_basic", true, "Basic template replacement works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_template_missing_keys(void)
{
    log_test_start("template_missing_keys");

    dString_t* sb = d_InitString();
    const char* keys[] = {"name"};
    const char* values[] = {"Bob"};

    d_TemplateString(sb, "Hello {name}, you have {gold} gold!", keys, values, 1);

    const char* expected = "Hello Bob, you have {gold} gold!";
    const char* actual = d_PeekString(sb);

    LOG("Testing template with missing key - {gold} should remain unchanged");

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("template_missing_keys", true, "Missing keys left unchanged correctly");
    d_DestroyString(sb);
    return 1;
}

int test_template_no_placeholders(void)
{
    log_test_start("template_no_placeholders");

    dString_t* sb = d_InitString();
    const char* keys[] = {"unused"};
    const char* values[] = {"value"};

    const char* template_str = "No placeholders here!";
    d_TemplateString(sb, template_str, keys, values, 1);

    const char* actual = d_PeekString(sb);

    LOG("Testing template with no placeholders - should remain unchanged");

    if (strcmp(actual, template_str) != 0) {
        log_string_comparison(template_str, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("template_no_placeholders", true, "Template without placeholders handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_template_empty_keys(void)
{
    log_test_start("template_empty_keys");

    dString_t* sb = d_InitString();
    const char* keys[] = {""};
    const char* values[] = {"empty"};

    d_TemplateString(sb, "Test {} placeholder", keys, values, 1);

    const char* expected = "Test empty placeholder";
    const char* actual = d_PeekString(sb);

    LOG("Testing template with empty key - {} should be replaced");

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("template_empty_keys", true, "Empty key replacement works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_template_multiple_same_key(void)
{
    log_test_start("template_multiple_same_key");

    dString_t* sb = d_InitString();
    const char* keys[] = {"item"};
    const char* values[] = {"sword"};

    d_TemplateString(sb, "You have a {item}. The {item} is sharp!", keys, values, 1);

    const char* expected = "You have a sword. The sword is sharp!";
    const char* actual = d_PeekString(sb);

    LOG("Testing template with same key appearing multiple times");

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("template_multiple_same_key", true, "Multiple occurrences of same key replaced correctly");
    d_DestroyString(sb);
    return 1;
}

int test_template_nested_braces(void)
{
    log_test_start("template_nested_braces");

    dString_t* sb = d_InitString();
    const char* keys[] = {"name"};
    const char* values[] = {"Alice"};

    d_TemplateString(sb, "Hello {name}! { This is not a placeholder }", keys, values, 1);

    const char* expected = "Hello Alice! { This is not a placeholder }";
    const char* actual = d_PeekString(sb);

    LOG("Testing template with nested braces - only {name} should be replaced");

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("template_nested_braces", true, "Nested braces handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_template_rpg_scenario(void)
{
    log_test_start("template_rpg_scenario");

    dString_t* sb = d_InitString();
    const char* keys[] = {"player", "enemy", "damage", "weapon", "critical"};
    const char* values[] = {"Warrior", "Goblin", "25", "Iron Sword", "CRITICAL"};

    d_TemplateString(sb,
        "{player} attacks {enemy} with {weapon} for {critical} {damage} damage!",
        keys, values, 5);

    const char* expected = "Warrior attacks Goblin with Iron Sword for CRITICAL 25 damage!";
    const char* actual = d_PeekString(sb);

    dString_t* log_msg = d_InitString();
    d_FormatString(log_msg, "RPG Combat Scenario: %s", actual);
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("template_rpg_scenario", true, "Complex RPG scenario template works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_template_faction_dialogue(void)
{
    log_test_start("template_faction_dialogue");

    dString_t* sb = d_InitString();
    const char* keys[] = {"faction", "player", "reputation", "quest"};
    const char* values[] = {"Royal Loyalists", "Hero", "trusted ally", "retrieve the crown"};

    d_TemplateString(sb,
        "The {faction} representative nods.\n"
        "\"Greetings, {player}. As a {reputation}, we need you to {quest}.\"",
        keys, values, 4);

    const char* expected =
        "The Royal Loyalists representative nods.\n"
        "\"Greetings, Hero. As a trusted ally, we need you to retrieve the crown.\"";
    const char* actual = d_PeekString(sb);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Faction Dialogue:\n", 0);
    d_AppendString(log_msg, actual, 0);
    printf("%s\n", d_PeekString(log_msg));
    d_DestroyString(log_msg);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("template_faction_dialogue", true, "Multi-line faction dialogue template works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_template_null_safety(void)
{
    log_test_start("template_null_safety");

    // Should not crash with NULL parameters
    d_TemplateString(NULL, "test {key}", NULL, NULL, 0);
    LOG("NULL string builder handled safely");

    dString_t* sb = d_InitString();
    size_t initial_len = d_GetStringLength(sb);
    d_TemplateString(sb, NULL, NULL, NULL, 0);
    TEST_ASSERT(d_GetStringLength(sb) == initial_len, "Null template should not modify string");

    // Test with valid sb but null keys/values
    d_TemplateString(sb, "test {key}", NULL, NULL, 1);
    const char* expected = "test {key}";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("template_null_safety", true, "NULL parameters handled safely");
    d_DestroyString(sb);
    return 1;
}

// =============================================================================
// Integration Tests
// =============================================================================

int test_integration_rpg_character_sheet(void)
{
    log_test_start("integration_rpg_character_sheet");

    dString_t* sheet = d_InitString();

    // Character name and title
    const char* keys[] = {"name", "title", "level", "health", "mana", "faction"};
    const char* values[] = {"Sir Galahad", "Knight of the Round Table", "15", "180", "50", "Royal Loyalists"};

    // Build character sheet using all string functions
    d_TemplateString(sheet, "=== {name} ===\n{title}\n\n", keys, values, 6);

    // Stats with progress bars
    d_AppendString(sheet, "Level: ", 0);
    d_FormatString(sheet, "%s\n", values[2]);

    d_AppendString(sheet, "Health: ", 0);
    d_AppendProgressBar(sheet, 180, 200, 20, '=', '-');
    d_FormatString(sheet, " %s/200\n", values[3]);

    d_AppendString(sheet, "Mana:   ", 0);
    d_AppendProgressBar(sheet, 50, 100, 20, '*', '-');
    d_FormatString(sheet, " %s/100\n", values[4]);

    d_TemplateString(sheet, "\nFaction: {faction}\n", keys, values, 6);

    // Log the complete character sheet
    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Generated Character Sheet:\n", 0);
    d_AppendString(log_msg, d_PeekString(sheet), 0);
    printf("%s\n", d_PeekString(log_msg));
    d_DestroyString(log_msg);

    // Verify the complete character sheet components
    const char* expected_start = "=== Sir Galahad ===\nKnight of the Round Table\n\nLevel: 15\n";
    const char* full_result = d_PeekString(sheet);

    TEST_ASSERT(strncmp(full_result, expected_start, strlen(expected_start)) == 0,
                "Character sheet header failed");

    // Check that it contains the progress bars
    TEST_ASSERT(strstr(full_result, "[==================--]") != NULL, "Health bar not found");
    TEST_ASSERT(strstr(full_result, "[**********----------]") != NULL, "Mana bar not found");
    TEST_ASSERT(strstr(full_result, "Faction: Royal Loyalists") != NULL, "Faction not found");

    log_test_result("integration_rpg_character_sheet", true,
                   "Complete character sheet integration successful");
    d_DestroyString(sheet);
    return 1;
}

// =============================================================================
// Advanced Edge Case Tests
// =============================================================================

int test_format_string_edge_cases(void)
{
    log_test_start("format_string_edge_cases");

    dString_t* sb = d_InitString();

    // Test with empty format string
    d_FormatString(sb, "");
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Empty format should result in empty string");

    // Test with format specifiers but no arguments (undefined behavior, but shouldn't crash)
    d_ClearString(sb);
    d_FormatString(sb, "Value: %d");
    LOG("Format with missing arguments handled (result may be undefined)");

    // Test with very long format strings
    d_ClearString(sb);
    char long_format[1000];
    for (int i = 0; i < 999; i++) {
        long_format[i] = (i % 26) + 'a';
    }
    long_format[999] = '\0';
    d_FormatString(sb, "%s", long_format);
    TEST_ASSERT(d_GetStringLength(sb) == 999, "Long format string should be handled correctly");

    log_test_result("format_string_edge_cases", true, "Format string edge cases handled");
    d_DestroyString(sb);
    return 1;
}

int test_progress_bar_edge_cases(void)
{
    log_test_start("progress_bar_edge_cases");

    dString_t* sb = d_InitString();

    // Test with width of 1
    d_AppendProgressBar(sb, 50, 100, 1, '#', '-');
    TEST_ASSERT(strcmp(d_PeekString(sb), "[-]") == 0 || strcmp(d_PeekString(sb), "[#]") == 0,
                "Width 1 progress bar should work");

    // Test with very large width
    d_ClearString(sb);
    d_AppendProgressBar(sb, 50, 100, 100, '=', '-');
    size_t result_len = d_GetStringLength(sb);
    TEST_ASSERT(result_len == 102, "Large width progress bar should have correct length"); // [100 chars]

    // Test with floating point precision edge cases
    d_ClearString(sb);
    d_AppendProgressBar(sb, 1, 3, 10, '#', '-'); // 33.33%
    log_progress_bar_test(1, 3, 10, d_PeekString(sb));

    log_test_result("progress_bar_edge_cases", true, "Progress bar edge cases handled");
    d_DestroyString(sb);
    return 1;
}

int test_template_edge_cases(void)
{
    log_test_start("template_edge_cases");

    dString_t* sb = d_InitString();

    // Test with empty template
    const char* keys[] = {"key"};
    const char* values[] = {"value"};
    d_TemplateString(sb, "", keys, values, 1);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Empty template should result in empty string");

    // Test with template containing only braces
    d_ClearString(sb);
    d_TemplateString(sb, "{}", keys, values, 0);
    TEST_ASSERT(strcmp(d_PeekString(sb), "{}") == 0, "Template with only braces should remain unchanged");

    // Test with unmatched braces
    d_ClearString(sb);
    d_TemplateString(sb, "Hello {name world", keys, values, 1);
    LOG("Unmatched braces handled (result may vary)");

    // Test with very long key names
    d_ClearString(sb);
    char long_key[100];
    for (int i = 0; i < 99; i++) {
        long_key[i] = 'a';
    }
    long_key[99] = '\0';
    const char* long_keys[] = {long_key};
    const char* long_values[] = {"replaced"};

    dString_t* template_with_long_key = d_InitString();
    d_FormatString(template_with_long_key, "Test {%s} end", long_key);
    d_TemplateString(sb, d_PeekString(template_with_long_key), long_keys, long_values, 1);
    TEST_ASSERT(strstr(d_PeekString(sb), "replaced") != NULL, "Long key names should work");
    d_DestroyString(template_with_long_key);

    log_test_result("template_edge_cases", true, "Template edge cases handled");
    d_DestroyString(sb);
    return 1;
}

// =============================================================================
// Performance and Stress Tests
// =============================================================================

int test_string_functions_performance(void)
{
    log_test_start("string_functions_performance");

    // Test with many operations
    dString_t* sb = d_InitString();

    // Many format operations
    for (int i = 0; i < 100; i++) {
        d_FormatString(sb, "Number %d ", i);
    }

    dString_t* log_msg = d_InitString();
    d_FormatString(log_msg, "After 100 format operations, string length: %zu", d_GetStringLength(sb));
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);

    // Many progress bars
    d_ClearString(sb);
    for (int i = 0; i < 50; i++) {
        d_AppendProgressBar(sb, i, 50, 10, '=', '-');
        d_AppendString(sb, "\n", 0);
    }

    log_msg = d_InitString();
    d_FormatString(log_msg, "After 50 progress bars, string length: %zu", d_GetStringLength(sb));
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);

    // Many template operations
    d_ClearString(sb);
    const char* keys[] = {"num"};
    for (int i = 0; i < 100; i++) {
        char value_str[20];
        snprintf(value_str, sizeof(value_str), "%d", i);
        const char* values[] = {value_str};
        d_TemplateString(sb, "Item {num} ", keys, values, 1);
    }

    log_msg = d_InitString();
    d_FormatString(log_msg, "After 100 template operations, string length: %zu", d_GetStringLength(sb));
    LOG(d_PeekString(log_msg));
    d_DestroyString(log_msg);

    log_test_result("string_functions_performance", true, "Performance test completed successfully");
    d_DestroyString(sb);
    return 1;
}

// =============================================================================
// Main Test Runner
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Advanced String Function Tests");

    LOG("Starting comprehensive advanced string function tests");

    RUN_TEST(test_string_format_basic);
    RUN_TEST(test_string_format_integers);
    RUN_TEST(test_string_format_append);
    RUN_TEST(test_string_format_null_safety);

    RUN_TEST(test_progress_bar_basic);
    RUN_TEST(test_progress_bar_full);
    RUN_TEST(test_progress_bar_empty);
    RUN_TEST(test_progress_bar_overflow);
    RUN_TEST(test_progress_bar_partial);
    RUN_TEST(test_progress_bar_null_safety);

    RUN_TEST(test_template_basic);
    RUN_TEST(test_template_missing_keys);
    RUN_TEST(test_template_no_placeholders);
    RUN_TEST(test_template_empty_keys);
    RUN_TEST(test_template_multiple_same_key);
    RUN_TEST(test_template_nested_braces);
    RUN_TEST(test_template_rpg_scenario);
    RUN_TEST(test_template_faction_dialogue);
    RUN_TEST(test_template_null_safety);

    // float
    RUN_TEST(test_string_append_float);

    // Integration tests

    RUN_TEST(test_integration_rpg_character_sheet);

    // Edge case tests

    RUN_TEST(test_format_string_edge_cases);
    RUN_TEST(test_progress_bar_edge_cases);
    RUN_TEST(test_template_edge_cases);

    // Performance tests

    RUN_TEST(test_string_functions_performance);

    TEST_SUITE_END();
}
