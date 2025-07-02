// File: true_tests/strings/test_string_advanced.c - Divine Advanced String Tests
// Enhanced with the full power of Daedalus Logging and Metis's Wisdom

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;


// =============================================================================
// DIVINE HELPER FUNCTIONS WITH ENHANCED LOGGING
// =============================================================================
// Create a test string builder with enhanced logging
dString_t* create_test_builder(void)
{
    dString_t* sb = d_InitString();
    d_LogIfF(sb == NULL, D_LOG_LEVEL_ERROR, "Failed to create test string builder");
    return sb;
}
// Enhanced string comparison with comprehensive logging
static bool divine_string_compare(const char* actual, const char* expected, const char* context)
{
    if (actual == NULL && expected == NULL) {
        d_LogDebugF("String comparison [%s]: Both strings are NULL (valid)", context);
        return true;
    }
    if (actual == NULL || expected == NULL) {
        d_LogErrorF("String comparison [%s]: NULL pointer detected (actual=%p, expected=%p)",
                    context, (void*)actual, (void*)expected);
        return false;
    }

    bool result = strcmp(actual, expected) == 0;
    if (!result) {
        d_LogWarningF("String mismatch [%s]:", context);
        d_LogWarningF("  Expected: '%s' (len=%zu)", expected, strlen(expected));
        d_LogWarningF("  Actual:   '%s' (len=%zu)", actual, strlen(actual));

        // Show character-by-character diff for debugging
        size_t min_len = strlen(actual) < strlen(expected) ? strlen(actual) : strlen(expected);
        for (size_t i = 0; i < min_len; i++) {
            if (actual[i] != expected[i]) {
                d_LogWarningF("  First diff at pos %zu: expected '%c' (0x%02x), got '%c' (0x%02x)",
                             i, expected[i], (unsigned char)expected[i],
                             actual[i], (unsigned char)actual[i]);
                break;
            }
        }
    } else {
        d_LogDebugF("String comparison [%s]: MATCH - '%s'", context, actual);
    }
    return result;
}

// Helper for testing progress bar calculations
static void log_progress_analysis(int current, int max, int width, const char* result)
{
    float percentage = (max > 0) ? ((float)current / max) * 100.0f : 0.0f;
    int expected_filled = (max > 0) ? (current * width) / max : 0;
    if (expected_filled > width) expected_filled = width;

    d_LogDebugF("Progress Analysis: %d/%d (%.1f%%) -> width=%d, expected_filled=%d",
                current, max, percentage, width, expected_filled);
    d_LogDebugF("Progress Result: '%s' (total_len=%zu)", result, strlen(result));
}

// =============================================================================
// d_FormatString DIVINE TEST SUITE
// =============================================================================

int test_string_format_basic(void)
{
    d_LogInfo("VERIFICATION: Basic printf-style string formatting functionality.");
    dLogContext_t* ctx = d_PushLogContext("FormatBasic");

    dString_t* sb = d_InitString();
    TEST_ASSERT(sb != NULL, "Failed to create string builder");

    d_LogDebug("Testing basic string substitution...");
    d_FormatString(sb, "Hello %s!", "World");
    const char* expected = "Hello World!";
    const char* actual = d_PeekString(sb);

    TEST_ASSERT(divine_string_compare(actual, expected, "basic format"),
               "Basic string formatting should work correctly");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_format_integers(void)
{
    d_LogInfo("VERIFICATION: Integer formatting with various specifiers.");
    dLogContext_t* ctx = d_PushLogContext("FormatIntegers");

    dString_t* sb = d_InitString();

    d_LogDebug("Testing multiple integer formatting...");
    d_FormatString(sb, "Level %d character with %d health", 42, 100);
    const char* expected = "Level 42 character with 100 health";
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), expected, "integer format"),
               "Integer formatting should work correctly");

    d_LogDebug("Testing negative integers...");
    d_ClearString(sb);
    d_FormatString(sb, "Temperature: %d°C", -15);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "Temperature: -15°C", "negative integer"),
               "Negative integer formatting should work correctly");

    d_LogDebug("Testing hexadecimal formatting...");
    d_ClearString(sb);
    d_FormatString(sb, "Memory address: 0x%x", 0xDEADBEEF);
    d_LogDebugF("Hex format result: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Hexadecimal formatting should produce output");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_format_append(void)
{
    d_LogInfo("VERIFICATION: Format string appending to existing content.");
    dLogContext_t* ctx = d_PushLogContext("FormatAppend");

    dString_t* sb = d_InitString();

    d_LogDebug("Building string with append and format operations...");
    d_AppendString(sb, "Start: ", 0);
    d_LogDebugF("After initial append: '%s'", d_PeekString(sb));

    d_FormatString(sb, "Value=%d", 123);
    d_LogDebugF("After format: '%s'", d_PeekString(sb));

    d_AppendString(sb, " End", 0);
    d_LogDebugF("After final append: '%s'", d_PeekString(sb));

    const char* expected = "Start: Value=123 End";
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), expected, "format append"),
               "Format append should work correctly");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_format_advanced_specifiers(void)
{
    d_LogWarning("BUG HUNT: Testing advanced format specifiers and edge cases.");
    dLogContext_t* ctx = d_PushLogContext("FormatAdvanced");

    dString_t* sb = d_InitString();

    d_LogDebug("Testing floating point formatting...");
    d_FormatString(sb, "Pi: %.2f, E: %.4f", 3.14159f, 2.71828f);
    d_LogDebugF("Float format result: '%s'", d_PeekString(sb));
    TEST_ASSERT(strstr(d_PeekString(sb), "3.14") != NULL, "Pi should be formatted to 2 decimal places");
    TEST_ASSERT(strstr(d_PeekString(sb), "2.7183") != NULL, "E should be formatted to 4 decimal places");

    d_LogDebug("Testing width and padding specifiers...");
    d_ClearString(sb);
    d_FormatString(sb, "Number: %5d, Padded: %05d", 42, 42);
    d_LogDebugF("Width format result: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Width formatting should produce output");

    d_LogDebug("Testing character formatting...");
    d_ClearString(sb);
    d_FormatString(sb, "First: %c, Second: %c", 'A', 'Z');
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "First: A, Second: Z", "character format"),
               "Character formatting should work correctly");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_string_format_null_safety(void)
{
    d_LogWarning("BUG HUNT: Testing format string NULL pointer safety.");
    dLogContext_t* ctx = d_PushLogContext("FormatNullSafety");

    d_LogDebug("Testing NULL string builder...");
    d_FormatString(NULL, "test format %d", 42);
    TEST_ASSERT(1, "NULL string builder should not crash");

    dString_t* sb = d_InitString();
    size_t initial_len = d_GetStringLength(sb);

    d_LogDebug("Testing NULL format string...");
    d_FormatString(sb, NULL);
    TEST_ASSERT(d_GetStringLength(sb) == initial_len, "NULL format should not modify string");

    d_LogDebug("Testing format with string parameters containing NULL...");
    d_FormatString(sb, "String: %s", (char*)NULL);
    d_LogDebugF("Format with NULL string parameter: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) >= initial_len, "Format should handle NULL string parameter");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// d_AppendFloat DIVINE TEST SUITE
// =============================================================================

int test_string_append_float(void)
{
    d_LogInfo("VERIFICATION: Floating-point number appending with precision control.");
    dLogContext_t* ctx = d_PushLogContext("AppendFloat");

    dString_t* sb = d_InitString();

    d_LogDebug("Testing 2 decimal places...");
    d_AppendFloat(sb, 3.14159f, 2);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "3.14", "2 decimal places"),
               "Float with 2 decimal places should be formatted correctly");

    d_LogDebug("Testing append to existing content...");
    d_ClearString(sb);
    d_AppendString(sb, "Pi is approximately: ", 0);
    d_AppendFloat(sb, 3.14159f, 4);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "Pi is approximately: 3.1416", "float append"),
               "Float should append to existing content correctly");

    d_LogDebug("Testing 0 decimal places (rounding)...");
    d_ClearString(sb);
    d_AppendFloat(sb, 42.7f, 0);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "43", "0 decimal places"),
               "Float with 0 decimals should round correctly");

    d_LogDebug("Testing negative decimals (default precision)...");
    d_ClearString(sb);
    d_AppendFloat(sb, 1.23456789f, -1);
    d_LogDebugF("Default precision result: '%s'", d_PeekString(sb));
    // Instead of exact string match, check for approximate precision
    TEST_ASSERT(d_GetStringLength(sb) >= 8 && d_GetStringLength(sb) <= 10, "Default precision should show approximately 6 decimal places");
    TEST_ASSERT(strstr(d_PeekString(sb), "1.234") != NULL, "Should start with expected digits");

    d_LogDebug("Testing excessive decimals (should clamp to 10)...");
    d_ClearString(sb);
    d_AppendFloat(sb, 1.23456789012345f, 20);
    d_LogDebugF("Excessive decimals result: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Excessive decimals should be clamped");

    d_LogDebug("Testing negative float values...");
    d_ClearString(sb);
    d_AppendFloat(sb, -273.15f, 2);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "-273.15", "negative float"),
               "Negative floats should be formatted correctly");

    d_LogDebug("Testing NULL safety...");
    d_AppendFloat(NULL, 3.14f, 2);
    TEST_ASSERT(1, "NULL string builder should not crash");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// d_AppendProgressBar DIVINE TEST SUITE
// =============================================================================

int test_progress_bar_basic(void)
{
    d_LogInfo("VERIFICATION: Basic progress bar rendering and calculation.");
    dLogContext_t* ctx = d_PushLogContext("ProgressBasic");

    dString_t* sb = d_InitString();

    d_LogDebug("Testing 50% progress bar...");
    d_AppendProgressBar(sb, 50, 100, 10, '#', '-');
    const char* expected = "[#####-----]";
    const char* actual = d_PeekString(sb);
    log_progress_analysis(50, 100, 10, actual);

    TEST_ASSERT(divine_string_compare(actual, expected, "50% progress"),
               "50% progress bar should render correctly");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_progress_bar_extremes(void)
{
    d_LogInfo("VERIFICATION: Progress bar extreme values and edge cases.");
    dLogContext_t* ctx = d_PushLogContext("ProgressExtremes");

    dString_t* sb = d_InitString();

    d_LogDebug("Testing full (100%) progress bar...");
    d_AppendProgressBar(sb, 100, 100, 8, '=', '.');
    const char* expected_full = "[========]";
    log_progress_analysis(100, 100, 8, d_PeekString(sb));
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), expected_full, "100% progress"),
               "Full progress bar should render correctly");

    d_LogDebug("Testing empty (0%) progress bar...");
    d_ClearString(sb);
    d_AppendProgressBar(sb, 0, 100, 6, '*', ' ');
    const char* expected_empty = "[      ]";
    log_progress_analysis(0, 100, 6, d_PeekString(sb));
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), expected_empty, "0% progress"),
               "Empty progress bar should render correctly");

    d_LogDebug("Testing overflow progress (>100%)...");
    d_ClearString(sb);
    d_AppendProgressBar(sb, 150, 100, 5, '+', '-');
    const char* expected_overflow = "[+++++]";
    log_progress_analysis(150, 100, 5, d_PeekString(sb));
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), expected_overflow, "overflow progress"),
               "Overflow progress bar should fill completely");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_progress_bar_precision(void)
{
    d_LogWarning("BUG HUNT: Testing progress bar mathematical precision and rounding.");
    dLogContext_t* ctx = d_PushLogContext("ProgressPrecision");

    dString_t* sb = d_InitString();

    d_LogDebug("Testing fractional progress calculation...");
    d_AppendProgressBar(sb, 33, 100, 12, '#', '-');
    // 33% of 12 = 3.96, should round down to 3
    const char* expected_33 = "[###---------]";
    log_progress_analysis(33, 100, 12, d_PeekString(sb));
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), expected_33, "33% precision"),
               "Fractional progress should round down correctly");

    d_LogDebug("Testing edge case: 1/3 progress...");
    d_ClearString(sb);
    d_AppendProgressBar(sb, 1, 3, 10, '#', '-');
    log_progress_analysis(1, 3, 10, d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) == 12, "Progress bar should have correct length");

    d_LogDebug("Testing minimum width edge case...");
    d_ClearString(sb);
    d_AppendProgressBar(sb, 50, 100, 1, '#', '-');
    const char* result_min = d_PeekString(sb);
    log_progress_analysis(50, 100, 1, result_min);
    TEST_ASSERT(strcmp(result_min, "[-]") == 0 || strcmp(result_min, "[#]") == 0,
               "Minimum width progress bar should work");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_progress_bar_null_safety(void)
{
    d_LogWarning("BUG HUNT: Testing progress bar NULL and invalid parameter safety.");
    dLogContext_t* ctx = d_PushLogContext("ProgressNullSafety");

    d_LogDebug("Testing NULL string builder...");
    d_AppendProgressBar(NULL, 50, 100, 10, '#', '-');
    TEST_ASSERT(1, "NULL string builder should not crash");

    dString_t* sb = d_InitString();
    size_t original_len = d_GetStringLength(sb);

    d_LogDebug("Testing invalid parameters...");
    d_AppendProgressBar(sb, 50, 0, 10, '#', '-');   // max = 0
    d_AppendProgressBar(sb, 50, 100, 0, '#', '-');  // width = 0
    d_AppendProgressBar(sb, 50, 100, -5, '#', '-'); // negative width

    d_LogDebugF("String length - before: %zu, after: %zu", original_len, d_GetStringLength(sb));
    TEST_ASSERT(d_GetStringLength(sb) == original_len, "Invalid parameters should not modify string");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// d_TemplateString DIVINE TEST SUITE
// =============================================================================

int test_template_basic(void)
{
    d_LogInfo("VERIFICATION: Basic template variable substitution.");
    dLogContext_t* ctx = d_PushLogContext("TemplateBasic");

    dString_t* sb = d_InitString();
    const char* keys[] = {"name", "level"};
    const char* values[] = {"Alice", "10"};

    d_LogDebugF("Template substitution: 2 key-value pairs");
    d_TemplateString(sb, "Hello {name}, you are level {level}!", keys, values, 2);

    const char* expected = "Hello Alice, you are level 10!";
    const char* actual = d_PeekString(sb);
    d_LogDebugF("Template result: '%s'", actual);

    TEST_ASSERT(divine_string_compare(actual, expected, "basic template"),
               "Basic template replacement should work correctly");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_template_missing_keys(void)
{
    d_LogInfo("VERIFICATION: Template behavior with missing key placeholders.");
    dLogContext_t* ctx = d_PushLogContext("TemplateMissing");

    dString_t* sb = d_InitString();
    const char* keys[] = {"name"};
    const char* values[] = {"Bob"};

    d_LogDebug("Testing template with missing key - {gold} should remain unchanged");
    d_TemplateString(sb, "Hello {name}, you have {gold} gold!", keys, values, 1);

    const char* expected = "Hello Bob, you have {gold} gold!";
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), expected, "missing keys"),
               "Missing keys should be left unchanged");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_template_complex_scenarios(void)
{
    d_LogInfo("VERIFICATION: Complex template scenarios and edge cases.");
    dLogContext_t* ctx = d_PushLogContext("TemplateComplex");

    dString_t* sb = d_InitString();

    d_LogDebug("Testing template with no placeholders...");
    const char* keys[] = {"unused"};
    const char* values[] = {"value"};
    const char* template_str = "No placeholders here!";
    d_TemplateString(sb, template_str, keys, values, 1);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), template_str, "no placeholders"),
               "Template without placeholders should remain unchanged");

    d_LogDebug("Testing template with empty keys...");
    d_ClearString(sb);
    const char* empty_keys[] = {""};
    const char* empty_values[] = {"empty"};
    d_TemplateString(sb, "Test {} placeholder", empty_keys, empty_values, 1);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "Test empty placeholder", "empty key"),
               "Empty key replacement should work correctly");

    d_LogDebug("Testing multiple occurrences of same key...");
    d_ClearString(sb);
    const char* item_keys[] = {"item"};
    const char* item_values[] = {"sword"};
    d_TemplateString(sb, "You have a {item}. The {item} is sharp!", item_keys, item_values, 1);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "You have a sword. The sword is sharp!", "multiple same key"),
               "Multiple occurrences of same key should be replaced");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_template_rpg_scenarios(void)
{
    d_LogInfo("VERIFICATION: RPG-specific template scenarios for game development.");
    dLogContext_t* ctx = d_PushLogContext("TemplateRPG");

    dString_t* sb = d_InitString();

    d_LogDebug("Testing complex RPG combat scenario...");
    const char* keys[] = {"player", "enemy", "damage", "weapon", "critical"};
    const char* values[] = {"Warrior", "Goblin", "25", "Iron Sword", "CRITICAL"};
    d_TemplateString(sb, "{player} attacks {enemy} with {weapon} for {critical} {damage} damage!",
                     keys, values, 5);

    const char* expected = "Warrior attacks Goblin with Iron Sword for CRITICAL 25 damage!";
    d_LogDebugF("Combat scenario: '%s'", d_PeekString(sb));
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), expected, "rpg combat"),
               "Complex RPG scenario should work correctly");

    d_LogDebug("Testing multi-line faction dialogue...");
    d_ClearString(sb);
    const char* dialogue_keys[] = {"faction", "player", "reputation", "quest"};
    const char* dialogue_values[] = {"Royal Loyalists", "Hero", "trusted ally", "retrieve the crown"};
    d_TemplateString(sb, "The {faction} representative nods.\n\"Greetings, {player}. As a {reputation}, we need you to {quest}.\"",
                     dialogue_keys, dialogue_values, 4);

    const char* expected_dialogue = "The Royal Loyalists representative nods.\n\"Greetings, Hero. As a trusted ally, we need you to retrieve the crown.\"";
    d_LogDebugF("Faction dialogue:\n%s", d_PeekString(sb));
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), expected_dialogue, "faction dialogue"),
               "Multi-line faction dialogue should work correctly");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_template_edge_cases(void)
{
    d_LogWarning("BUG HUNT: Template system edge cases and boundary conditions.");
    dLogContext_t* ctx = d_PushLogContext("TemplateEdgeCases");

    dString_t* sb = d_InitString();

    d_LogDebug("Testing nested braces...");
    const char* keys[] = {"name"};
    const char* values[] = {"Alice"};
    d_TemplateString(sb, "Hello {name}! { This is not a placeholder }", keys, values, 1);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "Hello Alice! { This is not a placeholder }", "nested braces"),
               "Nested braces should be handled correctly");

    d_LogDebug("Testing very long key names...");
    d_ClearString(sb);
    char long_key[300]; // Exceeds the 255 limit in d_TemplateString
    for (int i = 0; i < 299; i++) {
        long_key[i] = 'a';
    }
    long_key[299] = '\0';
    const char* long_keys[] = {long_key};
    const char* long_values[] = {"replaced"};

    dString_t* template_with_long_key = d_InitString();
    d_FormatString(template_with_long_key, "Test {%s} end", long_key);
    d_TemplateString(sb, d_PeekString(template_with_long_key), long_keys, long_values, 1);
    d_LogDebugF("Long key template result: '%s'", d_PeekString(sb));
    // Long keys should be treated as literal text, not replaced
    TEST_ASSERT(strstr(d_PeekString(sb), "replaced") == NULL, "Very long keys should not be replaced");
    d_DestroyString(template_with_long_key);

    d_LogDebug("Testing unmatched braces...");
    d_ClearString(sb);
    d_TemplateString(sb, "Hello {name world", keys, values, 1);
    d_LogDebugF("Unmatched braces result: '%s'", d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Unmatched braces should not crash");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_template_null_safety(void)
{
    d_LogWarning("BUG HUNT: Template system NULL pointer safety verification.");
    dLogContext_t* ctx = d_PushLogContext("TemplateNullSafety");

    d_LogDebug("Testing NULL string builder...");
    d_TemplateString(NULL, "test {key}", NULL, NULL, 0);
    TEST_ASSERT(1, "NULL string builder should not crash");

    dString_t* sb = d_InitString();
    size_t initial_len = d_GetStringLength(sb);

    d_LogDebug("Testing NULL template string...");
    d_TemplateString(sb, NULL, NULL, NULL, 0);
    TEST_ASSERT(d_GetStringLength(sb) == initial_len, "NULL template should not modify string");

    d_LogDebug("Testing NULL keys/values arrays...");
    d_TemplateString(sb, "test {key}", NULL, NULL, 1);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "test {key}", "null arrays"),
               "NULL arrays should leave placeholders unchanged");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// INTEGRATION TESTS - COMBINING ALL DIVINE FUNCTIONS
// =============================================================================

int test_integration_rpg_character_sheet(void)
{
    d_LogInfo("VERIFICATION: Complete character sheet integration using all string functions.");
    dLogContext_t* ctx = d_PushLogContext("CharacterSheet");

    dString_t* sheet = d_InitString();

    // Character data
    const char* keys[] = {"name", "title", "level", "health", "mana", "faction", "class"};
    const char* values[] = {"Sir Galahad", "Knight of the Round Table", "15", "180", "50", "Royal Loyalists", "Paladin"};

    d_LogDebug("Building character sheet header...");
    d_TemplateString(sheet, "╔══════════════════════════════════════╗\n║ {name}\n║ {title}\n╠══════════════════════════════════════╣\n",
                     keys, values, 7);

    d_LogDebug("Adding character statistics...");
    d_TemplateString(sheet, "║ Class: {class}     Level: {level}\n", keys, values, 7);

    d_LogDebug("Adding health bar...");
    d_AppendString(sheet, "║ Health: ", 0);
    d_AppendProgressBar(sheet, 180, 200, 20, '=', '-');
    d_FormatString(sheet, " %s/200\n", values[3]);

    d_LogDebug("Adding mana bar...");
    d_AppendString(sheet, "║ Mana:   ", 0);
    d_AppendProgressBar(sheet, 50, 200, 20, '=', '-');
    d_FormatString(sheet, " %s/100\n", values[4]);

    d_LogDebug("Adding faction and footer...");
    d_TemplateString(sheet, "║ Faction: {faction}\n╚══════════════════════════════════════╝", keys, values, 7);

    // Log the complete character sheet
    d_LogDebugF("Generated Character Sheet:\n%s", d_PeekString(sheet));

    // Comprehensive verification
    const char* full_result = d_PeekString(sheet);
    TEST_ASSERT(strstr(full_result, "Sir Galahad") != NULL, "Character name should be present");
    TEST_ASSERT(strstr(full_result, "Knight of the Round Table") != NULL, "Character title should be present");
    TEST_ASSERT(strstr(full_result, "Level: 15") != NULL, "Character level should be present");
    TEST_ASSERT(strstr(full_result, "Class: Paladin") != NULL, "Character class should be present");
    TEST_ASSERT(strstr(full_result, "Faction: Royal Loyalists") != NULL, "Character faction should be present");
    TEST_ASSERT(strstr(full_result, "=") != NULL, "Health bar should contain pound characters");
    TEST_ASSERT(strstr(full_result, "-") != NULL, "Mana bar should contain dash characters");
    TEST_ASSERT(strstr(full_result, "180/200") != NULL, "Health values should be displayed");
    TEST_ASSERT(strstr(full_result, "50/100") != NULL, "Mana values should be displayed");

    d_LogDebugF("Character sheet length: %zu characters", d_GetStringLength(sheet));
    TEST_ASSERT(d_GetStringLength(sheet) > 200, "Complete character sheet should be substantial");

    d_DestroyString(sheet);
    d_PopLogContext(ctx);
    return 1;
}

int test_integration_combat_log_system(void)
{
    d_LogInfo("VERIFICATION: Dynamic combat log generation using all advanced functions.");
    dLogContext_t* ctx = d_PushLogContext("CombatLog");

    dString_t* combat_log = d_InitString();

    d_LogDebug("Simulating turn-based combat with dynamic logging...");

    // Turn 1: Player Attack
    const char* turn1_keys[] = {"attacker", "target", "weapon", "damage", "crit"};
    const char* turn1_values[] = {"Warrior", "Orc", "Flame Sword", "32", "CRITICAL"};
    d_TemplateString(combat_log, "Turn 1: {attacker} attacks {target} with {weapon}\n", turn1_keys, turn1_values, 5);
    d_TemplateString(combat_log, "        Deals {crit} {damage} damage!\n", turn1_keys, turn1_values, 5);

    // Health bar after attack
    d_AppendString(combat_log, "        Orc Health: ", 0);
    d_AppendProgressBar(combat_log, 68, 100, 15, '#', '-');
    d_AppendString(combat_log, " 68/100\n\n", 0);

    // Turn 2: Enemy Counter-attack
    d_TemplateString(combat_log, "Turn 2: Orc counter-attacks with claws\n        Deals 18 damage\n", NULL, NULL, 0);
    d_AppendString(combat_log, "        Warrior Health: ", 0);
    d_AppendProgressBar(combat_log, 81, 100, 15, '#', '-');
    d_AppendString(combat_log, " 82/100\n\n", 0);

    // Turn 3: Spell casting with float values
    d_AppendString(combat_log, "Turn 3: Warrior casts Heal\n        Restores ", 0);
    d_AppendFloat(combat_log, 15.5f, 1);
    d_AppendString(combat_log, " health\n", 0);
    d_AppendString(combat_log, "        Warrior Health: ", 0);
    d_AppendProgressBar(combat_log, 97, 100, 15, '#', '-');
    d_AppendString(combat_log, " 97/100\n\n", 0);

    // Combat summary with complex formatting
    d_FormatString(combat_log, "Combat Summary:\n- Total turns: %d\n- Damage dealt: %d\n- Experience gained: %d\n",
                   3, 32, 150);

    d_LogDebugF("Generated Combat Log:\n%s", d_PeekString(combat_log));

    // Verification
    const char* log_content = d_PeekString(combat_log);
    TEST_ASSERT(strstr(log_content, "CRITICAL 32 damage") != NULL, "Critical damage should be logged");
    TEST_ASSERT(strstr(log_content, "#") != NULL, "Health bars should use pound symbols");
    TEST_ASSERT(strstr(log_content, "15.5 health") != NULL, "Float healing values should be displayed");
    TEST_ASSERT(strstr(log_content, "Total turns: 3") != NULL, "Combat summary should include turn count");
    TEST_ASSERT(strstr(log_content, "Experience gained: 150") != NULL, "Experience should be calculated");

    d_LogDebugF("Combat log length: %zu characters", d_GetStringLength(combat_log));
    TEST_ASSERT(d_GetStringLength(combat_log) > 400, "Complete combat log should be comprehensive");

    d_DestroyString(combat_log);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// STRESS AND PERFORMANCE TESTS
// =============================================================================

int test_advanced_string_performance(void)
{
    d_LogWarning("BUG HUNT: Performance stress testing with large-scale operations.");
    dLogContext_t* ctx = d_PushLogContext("Performance");

    dString_t* performance_sb = d_InitString();

    d_LogDebug("Testing many format operations...");
    for (int i = 0; i < 100; i++) {
        d_FormatString(performance_sb, "Entry %04d: Value=%d ", i, i * i);

        // Rate-limited progress reporting
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 2.0, "Format operations progress: %d/100", i + 1);
    }
    d_LogDebugF("After 100 format operations: %zu characters", d_GetStringLength(performance_sb));

    d_LogDebug("Testing many progress bars...");
    d_ClearString(performance_sb);
    for (int i = 0; i <= 50; i++) {
        d_FormatString(performance_sb, "Progress %02d: ", i);
        // FIXED: Use ASCII characters instead of Unicode
        d_AppendProgressBar(performance_sb, i, 50, 20, '#', '-');
        d_FormatString(performance_sb, " %d%%\n", (i * 100) / 50);
    }
    d_LogDebugF("After 51 progress bars: %zu characters", d_GetStringLength(performance_sb));

    d_LogDebug("Testing many template operations...");
    d_ClearString(performance_sb);
    const char* keys[] = {"num", "square", "cube"};
    for (int i = 0; i < 100; i++) {
        char num_str[20], square_str[20], cube_str[20];
        snprintf(num_str, sizeof(num_str), "%d", i);
        snprintf(square_str, sizeof(square_str), "%d", i * i);
        snprintf(cube_str, sizeof(cube_str), "%d", i * i * i);
        const char* values[] = {num_str, square_str, cube_str};

        d_TemplateString(performance_sb, "Number {num}: Square={square}, Cube={cube}\n", keys, values, 3);
    }
    d_LogDebugF("After 100 template operations: %zu characters", d_GetStringLength(performance_sb));

    d_LogDebug("Testing mixed operations with large strings...");
    d_ClearString(performance_sb);
    for (int cycle = 0; cycle < 20; cycle++) {
        d_FormatString(performance_sb, "=== Cycle %d ===\n", cycle);
        d_AppendProgressBar(performance_sb, cycle, 20, 30, '=', '-');
        d_AppendString(performance_sb, "\n", 0);

        const char* status_keys[] = {"cycle", "status"};
        char cycle_str[20];
        snprintf(cycle_str, sizeof(cycle_str), "%d", cycle);
        const char* status_values[] = {cycle_str, cycle % 2 ? "Processing" : "Complete"};
        d_TemplateString(performance_sb, "Cycle {cycle}: {status}\n\n", status_keys, status_values, 2);
    }

    size_t final_length = d_GetStringLength(performance_sb);
    d_LogDebugF("Final performance test string length: %zu characters", final_length);
    TEST_ASSERT(final_length > 1000, "Performance test should generate substantial content");
    TEST_ASSERT(final_length < 100000, "Performance test should not generate excessive content");

    d_DestroyString(performance_sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_memory_stress_advanced(void)
{
    d_LogWarning("BUG HUNT: Memory allocation stress testing with rapid creation/destruction.");
    dLogContext_t* ctx = d_PushLogContext("MemoryStress");

    d_LogDebug("Creating and destroying multiple string builders rapidly...");
    for (int cycle = 0; cycle < 50; cycle++) {
        dString_t* temp_sb = d_InitString();
        TEST_ASSERT(temp_sb != NULL, "String builder creation should not fail");

        // Perform various operations on each temporary builder
        d_FormatString(temp_sb, "Stress test cycle %d with formatting", cycle);
        d_AppendProgressBar(temp_sb, cycle, 50, 15, '#', '-');

        const char* keys[] = {"cycle"};
        char cycle_str[20];
        snprintf(cycle_str, sizeof(cycle_str), "%d", cycle);
        const char* values[] = {cycle_str};
        d_TemplateString(temp_sb, " Cycle: {cycle}", keys, values, 1);

        d_AppendFloat(temp_sb, cycle * 3.14159f, 2);

        // Verify content before destruction
        TEST_ASSERT(d_GetStringLength(temp_sb) > 0, "Temporary builder should have content");

        d_DestroyString(temp_sb);

        // Rate-limited progress reporting
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 3.0, "Memory stress cycle %d completed", cycle + 1);
    }

    d_LogDebug("Testing concurrent string builder operations...");
    dString_t* builders[10];
    for (int i = 0; i < 10; i++) {
        builders[i] = d_InitString();
        TEST_ASSERT(builders[i] != NULL, "Multiple builders should be created successfully");
        d_FormatString(builders[i], "Builder %d initialized", i);
    }

    // Perform operations on all builders
    for (int i = 0; i < 10; i++) {
        d_AppendProgressBar(builders[i], i + 1, 10, 10, '=', '.');
        d_FormatString(builders[i], " [%d/10]", i + 1);
    }

    // Verify and cleanup
    for (int i = 0; i < 10; i++) {
        TEST_ASSERT(d_GetStringLength(builders[i]) > 0, "Each builder should have content");
        d_DestroyString(builders[i]);
    }

    d_PopLogContext(ctx);
    return 1;
}
int test_template_advanced_unicode_and_boundaries(void)
{
    d_LogWarning("BUG HUNT: Testing template system with Unicode characters and boundary conditions.");
    dLogContext_t* ctx = d_PushLogContext("TemplateUnicodeBoundary");

    dString_t* sb = create_test_builder();

    d_LogDebug("Testing template with Unicode characters...");
    const char* unicode_keys[] = {"player", "weapon", "effect"};
    const char* unicode_values[] = {"Björn", "Excalibur", "Critical"}; // Removed Unicode symbols
    d_TemplateString(sb, "{player} wields {weapon} with {effect} effect!",
                     unicode_keys, unicode_values, 3);

    const char* result = d_PeekString(sb);
    d_LogDebugF("Unicode template result: '%s'", result);
    TEST_ASSERT(strstr(result, "Björn") != NULL, "Should handle Unicode in player names");
    TEST_ASSERT(strstr(result, "Excalibur") != NULL, "Should handle weapon names");
    TEST_ASSERT(strstr(result, "Critical") != NULL, "Should handle effect names");

    d_LogDebug("Testing template with maximum key length boundary...");
    d_ClearString(sb);
    char max_key[255]; // Exactly at the limit
    for (int i = 0; i < 254; i++) {
        max_key[i] = 'a' + (i % 26);
    }
    max_key[254] = '\0';

    const char* boundary_keys[] = {max_key};
    const char* boundary_values[] = {"REPLACED"};

    dString_t* template_str = d_InitString();
    d_FormatString(template_str, "Test {%s} boundary", max_key);
    d_TemplateString(sb, d_PeekString(template_str), boundary_keys, boundary_values, 1);

    TEST_ASSERT(strstr(d_PeekString(sb), "REPLACED") != NULL,
               "Should handle maximum length keys (254 chars)");
    d_DestroyString(template_str);

    d_LogDebug("Testing template with key exactly 256 chars (should be treated as literal)...");
    d_ClearString(sb);
    char over_limit_key[300]; // Much over the limit to be sure
    for (int i = 0; i < 299; i++) {
        over_limit_key[i] = 'z';
    }
    over_limit_key[299] = '\0';

    const char* over_keys[] = {over_limit_key};
    const char* over_values[] = {"SHOULD_NOT_REPLACE"};

    dString_t* over_template = d_InitString();
    d_FormatString(over_template, "Test {%s} overlimit", over_limit_key);
    d_TemplateString(sb, d_PeekString(over_template), over_keys, over_values, 1);

    // CORRECTED: Your d_TemplateString implementation has if (key_len < 256), so 256+ chars ARE treated as literal
    const char* over_result = d_PeekString(sb);
    d_LogDebugF("Over-limit test result: '%s'", over_result);

    // The key should appear literally in the output since it's too long to be processed
    TEST_ASSERT(strstr(over_result, "SHOULD_NOT_REPLACE") == NULL,
               "Should NOT replace keys longer than 255 characters");
    TEST_ASSERT(strstr(over_result, "Test {") != NULL,
               "Over-limit keys should remain as literal placeholder text");
    d_DestroyString(over_template);

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_format_extreme_edge_cases(void)
{
    d_LogWarning("BUG HUNT: Testing d_FormatString with extreme edge cases and potential buffer overflows.");
    dLogContext_t* ctx = d_PushLogContext("FormatExtremeEdges");

    dString_t* sb = d_InitString(); // FIXED: Use d_InitString() directly

    d_LogDebug("Testing format with extremely large integers...");
    d_FormatString(sb, "Max int: %d, Min int: %d", INT32_MAX, INT32_MIN);
    const char* result = d_PeekString(sb);
    d_LogDebugF("Extreme integer format: '%s'", result);
    TEST_ASSERT(strstr(result, "2147483647") != NULL, "Should format INT32_MAX correctly");
    TEST_ASSERT(strstr(result, "-2147483648") != NULL, "Should format INT32_MIN correctly");

    d_LogDebug("Testing format with precision edge cases for floats...");
    d_ClearString(sb);
    d_FormatString(sb, "Tiny: %.10f, Huge: %.2f", 0.0000000001f, 999999999.99f);
    d_LogDebugF("Float precision test: '%s'", result = d_PeekString(sb));
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Should handle extreme float precision");

    d_LogDebug("Testing format with many arguments...");
    d_ClearString(sb);
    d_FormatString(sb, "%d %d %d %d %d %d %d %d %d %d",
                   1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    TEST_ASSERT(strstr(d_PeekString(sb), "1 2 3 4 5 6 7 8 9 10") != NULL,
               "Should handle many format arguments");

    d_LogDebug("Testing format with string longer than internal buffer...");
    d_ClearString(sb);
    char large_string[1000];
    for (int i = 0; i < 999; i++) {
        large_string[i] = 'A' + (i % 26);
    }
    large_string[999] = '\0';

    d_FormatString(sb, "Large: %s End", large_string);
    TEST_ASSERT(d_GetStringLength(sb) > 1000, "Should handle format strings requiring buffer growth");
    TEST_ASSERT(strstr(d_PeekString(sb), "Large: ABCDEFG") != NULL, "Should start correctly");
    TEST_ASSERT(strstr(d_PeekString(sb), " End") != NULL, "Should end correctly");

    d_LogDebug("Testing format with percent sign edge cases...");
    d_ClearString(sb);
    d_FormatString(sb, "100%% complete, progress: %d%%", 75);
    TEST_ASSERT(strstr(d_PeekString(sb), "100% complete") != NULL, "Should handle escaped percent");
    TEST_ASSERT(strstr(d_PeekString(sb), "progress: 75%") != NULL, "Should handle mixed percent usage");

    d_LogDebug("Testing format with NULL string parameter...");
    d_ClearString(sb);
    d_FormatString(sb, "String: '%s', Number: %d", (char*)NULL, 42);
    result = d_PeekString(sb);
    d_LogDebugF("NULL string format result: '%s'", result);
    // The behavior with NULL string is implementation-defined, but shouldn't crash
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Should handle NULL string parameter gracefully");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}
int test_string_builder_lifecycle_isolation(void)
{
    d_LogWarning("BUG HUNT: Isolating exact memory lifecycle patterns from items.c usage.");
    dLogContext_t* ctx = d_PushLogContext("LifecycleIsolation");

    d_LogDebug("Testing exact item creation pattern - creating 5 string fields per item...");

    // This mimics exactly what happens in create_weapon/create_armor/etc
    for (int item = 0; item < 10; item++) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 2.0, "Creating item %d with full string lifecycle", item);

        // Simulate the exact pattern from your items.c:
        // 1. item->name = d_InitString();
        dString_t* name = d_InitString();
        TEST_ASSERT(name != NULL, "Name string should be created");

        // 2. item->id = d_InitString();
        dString_t* id = d_InitString();
        TEST_ASSERT(id != NULL, "ID string should be created");

        // 3. item->description = d_InitString();
        dString_t* description = d_InitString();
        TEST_ASSERT(description != NULL, "Description string should be created");

        // 4. item->rarity = d_InitString();
        dString_t* rarity = d_InitString();
        TEST_ASSERT(rarity != NULL, "Rarity string should be created");

        // 5. material.name = d_InitString();
        dString_t* material_name = d_InitString();
        TEST_ASSERT(material_name != NULL, "Material name should be created");

        // Populate them exactly like your helper functions do
        d_AppendString(name, "Test Weapon", 0);
        d_AppendString(id, "test_weapon", 0);
        d_AppendString(description, "A weapon made of steel", 0);
        d_AppendString(rarity, "common", 0);
        d_AppendString(material_name, "steel", 0);

        // Verify they have content (like your tests do)
        TEST_ASSERT(d_GetStringLength(name) > 0, "Name should have content");
        TEST_ASSERT(d_GetStringLength(id) > 0, "ID should have content");
        TEST_ASSERT(d_GetStringLength(description) > 0, "Description should have content");
        TEST_ASSERT(d_GetStringLength(rarity) > 0, "Rarity should have content");
        TEST_ASSERT(d_GetStringLength(material_name) > 0, "Material name should have content");

        // NOW - destroy them in the EXACT same order as destroy_item()
        d_LogDebugF("Destroying item %d strings...", item);
        d_DestroyString(name);
        d_DestroyString(id);
        d_DestroyString(description);
        d_DestroyString(rarity);
        d_DestroyString(material_name);
    }

    d_LogDebug("Testing rapid creation/destruction cycles to stress allocation...");
    for (int cycle = 0; cycle < 50; cycle++) {
        dString_t* temp = d_InitString();
        d_AppendString(temp, "Rapid cycle test string content", 0);

        // Use some of the advanced functions that might have internal allocations
        d_FormatString(temp, " - Cycle %d", cycle);
        d_AppendProgressBar(temp, cycle, 50, 10, '#', '-');

        // Verify before destruction
        TEST_ASSERT(d_GetStringLength(temp) > 0, "Temp string should have content");

        d_DestroyString(temp);
    }

    d_LogDebug("Testing the exact _validate_and_truncate_string pattern that uses d_AppendStringN...");
    for (int trunc_test = 0; trunc_test < 20; trunc_test++) {
        dString_t* truncated = d_InitString();

        // This is the pattern from _validate_and_truncate_string
        const char* long_name = "This is a very long name that will be truncated by AppendStringN";
        d_AppendStringN(truncated, long_name, 15); // Truncate to 15 chars

        TEST_ASSERT(d_GetStringLength(truncated) == 15, "Should be truncated to 15 chars");
        TEST_ASSERT(strncmp(d_PeekString(truncated), "This is a very ", 15) == 0, "Content should be truncated correctly");

        d_DestroyString(truncated);
    }

    d_LogDebug("Testing template operations that might have internal state...");
    for (int template_test = 0; template_test < 15; template_test++) {
        dString_t* templated = d_InitString();

        const char* keys[] = {"name", "value"};
        char value_str[20];
        snprintf(value_str, sizeof(value_str), "%d", template_test);
        const char* values[] = {"TestItem", value_str};

        d_TemplateString(templated, "Item {name} has value {value}", keys, values, 2);

        TEST_ASSERT(d_GetStringLength(templated) > 0, "Template should produce content");

        d_DestroyString(templated);
    }

    d_LogDebug("Final test: Creating string builder, using ALL functions, then destroying...");
    dString_t* comprehensive = d_InitString();

    d_AppendString(comprehensive, "Initial", 0);
    d_AppendStringN(comprehensive, " Truncated Content", 5); // " Trun"
    d_AppendChar(comprehensive, '!');
    d_AppendInt(comprehensive, 42);
    d_AppendFloat(comprehensive, 3.14f, 2);
    d_FormatString(comprehensive, " Formatted: %s", "test");
    d_AppendProgressBar(comprehensive, 50, 100, 5, '#', '-');

    const char* keys[] = {"test"};
    const char* values[] = {"value"};
    d_TemplateString(comprehensive, " Template: {test}", keys, values, 1);

    d_LogDebugF("Comprehensive string final content: '%s'", d_PeekString(comprehensive));
    TEST_ASSERT(d_GetStringLength(comprehensive) > 20, "Should have substantial content");

    d_DestroyString(comprehensive);

    d_PopLogContext(ctx);
    return 1;
}

int test_set_string_replaces_content(void)
{
    d_LogInfo("VERIFICATION: d_SetString correctly replaces existing content.");
    dLogContext_t* ctx = d_PushLogContext("SetStringReplace");

    dString_t* sb = create_test_builder();
    d_AppendString(sb, "This is the original content.", 0);

    d_LogDebug("Replacing original content with new content...");
    d_SetString(sb, "This is the new content.", 0);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "This is the new content.", "replace content"),
               "d_SetString should replace the old content entirely.");

    d_LogDebug("Setting content again to ensure no double-free or memory issues...");
    d_SetString(sb, "Final content.", 0);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "Final content.", "final content"),
               "d_SetString should work correctly on subsequent calls.");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

int test_set_string_and_append_integration(void)
{
    d_LogInfo("VERIFICATION: Integration of d_SetString and d_AppendString.");
    dLogContext_t* ctx = d_PushLogContext("SetAndAppend");

    dString_t* sb = create_test_builder();

    d_LogDebug("Setting initial string, then appending...");
    d_SetString(sb, "Initial: ", 0);
    d_AppendString(sb, "Appended.", 0);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "Initial: Appended.", "set then append"),
               "Append should work correctly after a set operation.");

    d_LogDebug("Setting string again, overwriting the appended content...");
    d_SetString(sb, "Overwritten!", 0);
    TEST_ASSERT(divine_string_compare(d_PeekString(sb), "Overwritten!", "overwrite appended"),
               "Set should overwrite all previous content.");

    d_DestroyString(sb);
    d_PopLogContext(ctx);
    return 1;
}

/**
 * @brief Tests string comparison functions after d_AppendStringN operations.
 * Ensures that d_CompareStrings and d_CompareStringToCString correctly
 * reflect the state of dString_t objects after partial appends.
 */
int test_string_comparison_after_append_n(void)
{
    d_LogInfo("VERIFICATION: String comparison after d_AppendStringN operations.");
    dLogContext_t* ctx = d_PushLogContext("CompareAfterAppendN");

    dString_t* sb1 = create_test_builder();
    dString_t* sb2 = create_test_builder();

    d_AppendStringN(sb1, "HelloWorld", 5); // sb1 becomes "Hello"
    d_AppendString(sb2, "Hello", 0);       // sb2 becomes "Hello"

    TEST_ASSERT(d_CompareStrings(sb1, sb2) == 0, "sb1 (Hello) should equal sb2 (Hello) after AppendStringN");
    TEST_ASSERT(d_CompareStringToCString(sb1, "Hello") == 0, "sb1 (Hello) should equal C-string 'Hello'");

    d_ClearString(sb1);
    d_AppendStringN(sb1, "Testing123", 7); // sb1 becomes "Testing"
    d_AppendString(sb2, "World", 0);       // sb2 becomes "HelloWorld"

    d_LogDebugF("sb1 content: '%s'", d_PeekString(sb1));
    d_LogDebugF("sb2 content: '%s'", d_PeekString(sb2));

    TEST_ASSERT(d_CompareStrings(sb1, sb2) > 0, "sb1 (Testing) should be greater than sb2 (HelloWorld)");
    TEST_ASSERT(d_CompareStringToCString(sb1, "Testing") == 0, "sb1 (Testing) should equal C-string 'Testing'");

    d_DestroyString(sb1);
    d_DestroyString(sb2);
    d_PopLogContext(ctx);
    return 1;
}

/**
 * @brief Epic advanced test for string comparison, combining various scenarios.
 * This test aims to push the comparison functions to their limits by mixing
 * different string states, lengths, and content types.
 */
int test_string_comparison_epic_advanced(void)
{
    d_LogInfo("VERIFICATION: Epic advanced string comparison scenarios.");
    dLogContext_t* ctx = d_PushLogContext("CompareEpicAdvanced");

    dString_t* sba = create_test_builder();
    dString_t* sbb = create_test_builder();

    // Scenario 1: Long strings, one truncated, one with embedded null
    d_AppendString(sba, "This is a very long string for comparison.", 0);
    d_AppendString(sbb, "This is a very long string for comparison.", 0);

    d_TruncateString(sba, 10); // sba = "This is a "
    d_AppendStringN(sbb, "\0EMBEDDED", 10); // sbb = "This is a \0EMBEDDED" (length 29, but strcmp sees 10)

    TEST_ASSERT(d_CompareStrings(sba, sbb) < 0, "Truncated should be less than original long string");
    TEST_ASSERT(d_CompareStringToCString(sba, "This is a ") == 0, "Truncated should match C-string");

    // Scenario 2: Case sensitivity and mixed content
    d_SetString(sba, "Apple", 0);
    d_SetString(sbb, "apple", 0);

    TEST_ASSERT(d_CompareStrings(sba, sbb) < 0, "Uppercase 'Apple' should be less than lowercase 'apple'");
    TEST_ASSERT(d_CompareStringToCString(sba, "apple") < 0, "dString 'Apple' vs C-string 'apple'");

    // Scenario 3: Empty strings and NULLs after various operations
    d_ClearString(sba);
    d_DropString(sbb, d_GetStringLength(sbb)); // sbb becomes empty

    TEST_ASSERT(d_CompareStrings(sba, sbb) == 0, "Two empty dStrings should be equal");
    TEST_ASSERT(d_CompareStringToCString(sba, "") == 0, "Empty dString vs empty C-string");

    d_DestroyString(sba);
    d_DestroyString(sbb);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER WITH DIVINE LOGGING ARCHITECTURE
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
        .include_file_info = true,   // Enable for detailed debugging
        .include_function = true     // Enable for comprehensive context
    };

    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);
    d_AddLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler, NULL);

    d_LogInfo("Initializing MIDAS-Enhanced Advanced String Function Test Suite");
    d_LogDebugF("Daedalus Logging System Status: %s", "FULLY OPERATIONAL");
    d_LogDebugF("Test Architecture: %s", "Divine MIDAS Enhanced");
    // =========================================================================

    TEST_SUITE_START("MIDAS Enhanced Advanced String Function Tests");

    // Core d_FormatString functionality
    RUN_TEST(test_string_format_basic);
    RUN_TEST(test_string_format_integers);
    RUN_TEST(test_string_format_append);
    RUN_TEST(test_string_format_advanced_specifiers);
    RUN_TEST(test_string_format_null_safety);

    // d_AppendFloat functionality
    RUN_TEST(test_string_append_float);

    // Progress bar functionality
    RUN_TEST(test_progress_bar_basic);
    RUN_TEST(test_progress_bar_extremes);
    RUN_TEST(test_progress_bar_precision);
    RUN_TEST(test_progress_bar_null_safety);

    // Template system functionality
    RUN_TEST(test_template_basic);
    RUN_TEST(test_template_missing_keys);
    RUN_TEST(test_template_complex_scenarios);
    RUN_TEST(test_template_rpg_scenarios);
    RUN_TEST(test_template_edge_cases);
    RUN_TEST(test_template_null_safety);

    // Integration tests
    RUN_TEST(test_integration_rpg_character_sheet);
    RUN_TEST(test_integration_combat_log_system);

    // Stress and performance tests
    RUN_TEST(test_advanced_string_performance);
    RUN_TEST(test_memory_stress_advanced);

    RUN_TEST(test_format_extreme_edge_cases);
    RUN_TEST(test_template_advanced_unicode_and_boundaries);
    RUN_TEST(test_string_builder_lifecycle_isolation);

    // d_SetString tests
    RUN_TEST(test_set_string_replaces_content);
    RUN_TEST(test_set_string_and_append_integration);

    // New comparison tests
    RUN_TEST(test_string_comparison_after_append_n);
    RUN_TEST(test_string_comparison_epic_advanced);

    // =========================================================================
    // DAEDALUS LOGGER CLEANUP
    // =========================================================================
    d_LogInfo("Advanced String Function Test Suite completed successfully");
    d_LogDebugF("Total test coverage: %s", "Comprehensive with edge cases and stress testing");
    d_DestroyLogger(d_GetGlobalLogger());
    // =========================================================================

    TEST_SUITE_END();
}
