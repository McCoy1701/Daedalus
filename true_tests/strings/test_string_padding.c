// File: true_tests/strings/test_string_padding.c - Tests for string padding functions
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

// ============================================================================
// d_PadLeftString Tests
// ============================================================================

int test_pad_left_basic(void)
{
    log_test_start("pad_left_basic");

    dString_t* sb = d_InitString();
    TEST_ASSERT(sb != NULL, "Failed to create string builder");

    d_PadLeftString(sb, "Hi", 5, '.');
    const char* expected = "...Hi";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_left_basic", true, "Basic left padding works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_left_no_padding_needed(void)
{
    log_test_start("pad_left_no_padding_needed");

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "Hello", 5, '.');
    const char* expected = "Hello";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_left_no_padding_needed", true, "No padding needed case handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_left_text_longer_than_width(void)
{
    log_test_start("pad_left_text_longer_than_width");

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "VeryLongText", 5, '.');
    const char* expected = "VeryLongText";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_left_text_longer_than_width", true, "Text longer than width handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_left_zero_width(void)
{
    log_test_start("pad_left_zero_width");

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "Test", 0, '.');
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Zero width should not add anything");

    log_test_result("pad_left_zero_width", true, "Zero width handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_left_different_characters(void)
{
    log_test_start("pad_left_different_characters");

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "XP", 6, '*');
    const char* expected = "****XP";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_left_different_characters", true, "Different pad characters work correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_left_single_character(void)
{
    log_test_start("pad_left_single_character");

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "A", 10, '-');
    const char* expected = "---------A";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_left_single_character", true, "Single character padding works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_left_null_safety(void)
{
    log_test_start("pad_left_null_safety");

    // Should not crash with NULL parameters
    d_PadLeftString(NULL, "test", 5, '.');
    LOG("NULL string builder handled safely");

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, NULL, 5, '.');
    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL text should not modify string");

    log_test_result("pad_left_null_safety", true, "NULL parameters handled safely");
    d_DestroyString(sb);
    return 1;
}

// ============================================================================
// d_PadRightString Tests
// ============================================================================

int test_pad_right_basic(void)
{
    log_test_start("pad_right_basic");

    dString_t* sb = d_InitString();
    d_PadRightString(sb, "Hi", 5, '.');
    const char* expected = "Hi...";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_right_basic", true, "Basic right padding works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_right_no_padding_needed(void)
{
    log_test_start("pad_right_no_padding_needed");

    dString_t* sb = d_InitString();
    d_PadRightString(sb, "Hello", 5, '.');
    const char* expected = "Hello";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_right_no_padding_needed", true, "No padding needed case handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_right_text_longer_than_width(void)
{
    log_test_start("pad_right_text_longer_than_width");

    dString_t* sb = d_InitString();
    d_PadRightString(sb, "VeryLongText", 5, '.');
    const char* expected = "VeryLongText";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_right_text_longer_than_width", true, "Text longer than width handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_right_spaces(void)
{
    log_test_start("pad_right_spaces");

    dString_t* sb = d_InitString();
    d_PadRightString(sb, "Name", 12, ' ');
    const char* expected = "Name        ";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_right_spaces", true, "Space padding works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_right_append_to_existing(void)
{
    log_test_start("pad_right_append_to_existing");

    dString_t* sb = d_InitString();
    d_AppendString(sb, "Start: ", 0);
    d_PadRightString(sb, "Item", 8, '.');
    const char* expected = "Start: Item....";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_right_append_to_existing", true, "Append to existing string works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_right_null_safety(void)
{
    log_test_start("pad_right_null_safety");

    // Should not crash with NULL parameters
    d_PadRightString(NULL, "test", 5, '.');
    LOG("NULL string builder handled safely");

    dString_t* sb = d_InitString();
    d_PadRightString(sb, NULL, 5, '.');
    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL text should not modify string");

    log_test_result("pad_right_null_safety", true, "NULL parameters handled safely");
    d_DestroyString(sb);
    return 1;
}

// ============================================================================
// d_PadCenterString Tests
// ============================================================================

int test_pad_center_even_padding(void)
{
    log_test_start("pad_center_even_padding");

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "Hi", 6, '.');
    const char* expected = "..Hi..";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_center_even_padding", true, "Even center padding works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_center_odd_padding(void)
{
    log_test_start("pad_center_odd_padding");

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "Hi", 7, '.');
    const char* expected = "..Hi...";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_center_odd_padding", true, "Odd center padding works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_center_no_padding_needed(void)
{
    log_test_start("pad_center_no_padding_needed");

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "Hello", 5, '.');
    const char* expected = "Hello";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_center_no_padding_needed", true, "No padding needed case handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_center_text_longer_than_width(void)
{
    log_test_start("pad_center_text_longer_than_width");

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "VeryLongText", 5, '.');
    const char* expected = "VeryLongText";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_center_text_longer_than_width", true, "Text longer than width handled correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_center_single_character_text(void)
{
    log_test_start("pad_center_single_character_text");

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "X", 9, '-');
    const char* expected = "----X----";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_center_single_character_text", true, "Single character centering works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_center_single_character_text_odd(void)
{
    log_test_start("pad_center_single_character_text_odd");

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "X", 8, '-');
    const char* expected = "---X----";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_center_single_character_text_odd", true, "Single character odd centering works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_center_empty_string(void)
{
    log_test_start("pad_center_empty_string");

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "", 5, '*');
    const char* expected = "*****";
    const char* actual = d_PeekString(sb);

    if (strcmp(actual, expected) != 0) {
        log_string_comparison(expected, actual);
        d_DestroyString(sb);
        return 0;
    }

    log_test_result("pad_center_empty_string", true, "Empty string centering works correctly");
    d_DestroyString(sb);
    return 1;
}

int test_pad_center_null_safety(void)
{
    log_test_start("pad_center_null_safety");

    // Should not crash with NULL parameters
    d_PadCenterString(NULL, "test", 5, '.');
    LOG("NULL string builder handled safely");

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, NULL, 5, '.');
    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL text should not modify string");

    log_test_result("pad_center_null_safety", true, "NULL parameters handled safely");
    d_DestroyString(sb);
    return 1;
}

// ============================================================================
// RPG-Specific Integration Tests
// ============================================================================

int test_rpg_table_formatting(void)
{
    log_test_start("rpg_table_formatting");

    dString_t* table = d_InitString();

    // Create a character stats table
    d_PadCenterString(table, "CHARACTER STATS", 40, '=');
    d_AppendChar(table, '\n');

    // Column headers
    d_PadLeftString(table, "Attribute", 15, ' ');
    d_AppendString(table, " | ", 0);
    d_PadRightString(table, "Value", 10, ' ');
    d_AppendString(table, " | ", 0);
    d_PadCenterString(table, "Status", 10, ' ');
    d_AppendString(table, "\n", 0);

    // Separator line
    d_RepeatString(table, '-', 40);
    d_AppendChar(table, '\n');

    // Data rows
    d_PadLeftString(table, "Strength", 15, ' ');
    d_AppendString(table, " | ", 0);
    d_PadRightString(table, "18", 10, ' ');
    d_AppendString(table, " | ", 0);
    d_PadCenterString(table, "Strong", 10, ' ');
    d_AppendString(table, "\n", 0);

    d_PadLeftString(table, "Dexterity", 15, ' ');
    d_AppendString(table, " | ", 0);
    d_PadRightString(table, "14", 10, ' ');
    d_AppendString(table, " | ", 0);
    d_PadCenterString(table, "Good", 10, ' ');
    d_AppendString(table, "\n", 0);

    const char* result = d_PeekString(table);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Table After Padding:\n", 0);
    d_AppendString(log_msg, result, 0);
    printf("%s\n", d_PeekString(log_msg));
    d_DestroyString(log_msg);

    // Verify the table structure
    TEST_ASSERT(strstr(result, "=======CHARACTER STATS========") != NULL, "Header centering works correctly");
    TEST_ASSERT(strstr(result, "     Strength |") != NULL, "Left padding works correctly");
    TEST_ASSERT(strstr(result, "18         |") != NULL, "Right padding works correctly");
    TEST_ASSERT(strstr(result, "  Strong  ") != NULL, "Center padding works correctly");

    log_test_result("rpg_table_formatting", true, "RPG table formatting works correctly");
    d_DestroyString(table);
    return 1;
}

int test_rpg_menu_system(void)
{
    log_test_start("rpg_menu_system");

    dString_t* menu = d_InitString();

    // Menu title
    d_PadCenterString(menu, "MAIN MENU", 30, '*');
    d_AppendString(menu, "\n\n", 0);

    // Menu options
    const char* options[] = {"New Game", "Load Game", "Settings", "Quit"};
    for (int i = 0; i < 4; i++) {
        d_AppendString(menu, "[", 0);
        d_AppendInt(menu, i + 1);
        d_AppendString(menu, "] ", 0);
        d_PadRightString(menu, options[i], 20, '.');
        d_AppendString(menu, "\n", 0);
    }

    const char* result = d_PeekString(menu);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Menu After Padding:\n", 0);
    d_AppendString(log_msg, result, 0);
    printf("%s\n", d_PeekString(log_msg));
    d_DestroyString(log_msg);

    // Verify menu formatting
    TEST_ASSERT(strstr(result, "**********MAIN MENU***********") != NULL, "Menu title should be centered");
    TEST_ASSERT(strstr(result, "[1] New Game............") != NULL, "Menu option 1 should be [1] New Game............");
    TEST_ASSERT(strstr(result, "[4] Quit................") != NULL, "Menu option 4 should be [4] Quit................");

    log_test_result("rpg_menu_system", true, "RPG menu system works correctly");
    d_DestroyString(menu);
    return 1;
}

int test_rpg_dialogue_box(void)
{
    log_test_start("rpg_dialogue_box");

    dString_t* dialogue = d_InitString();

    // Create dialogue box border
    d_RepeatString(dialogue, '+', 50);
    d_AppendChar(dialogue, '\n');

    // Speaker name centered
    d_AppendChar(dialogue, '|');
    d_PadCenterString(dialogue, "Royal Guard Captain", 48, ' ');
    d_AppendString(dialogue, "|\n", 0);

    // Separator
    d_AppendChar(dialogue, '|');
    d_RepeatString(dialogue, '-', 48);
    d_AppendString(dialogue, "|\n", 0);

    // Dialogue text (left-aligned)
    d_AppendChar(dialogue, '|');
    d_AppendChar(dialogue, ' ');
    d_PadRightString(dialogue, "\"Halt! None shall pass without", 47, ' ');
    d_AppendString(dialogue, "|\n", 0);

    d_AppendChar(dialogue, '|');
    d_AppendChar(dialogue, ' ');
    d_PadRightString(dialogue, "the King's seal!\"", 47, ' ');
    d_AppendString(dialogue, "|\n", 0);

    // Bottom border
    d_RepeatString(dialogue, '+', 50);
    d_AppendChar(dialogue, '\n');

    const char* result = d_PeekString(dialogue);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Dialogue After Padding:\n", 0);
    d_AppendString(log_msg, result, 0);
    printf("%s\n", d_PeekString(log_msg));
    d_DestroyString(log_msg);

    // Verify dialogue box structure
    TEST_ASSERT(strstr(result, "|              Royal Guard Captain               |") != NULL, "Speaker name centering ");
    TEST_ASSERT(strstr(result, "| \"Halt! None shall pass without                 |") != NULL, "Dialogue text alignment ");

    log_test_result("rpg_dialogue_box", true, "RPG dialogue box works correctly");
    d_DestroyString(dialogue);
    return 1;
}

// ============================================================================
// Float Padding Integration Tests
// ============================================================================

int test_float_padding_integration(void)
{
    log_test_start("float_padding_integration");

    dString_t* stats = d_InitString();

    // Create a stats display with float values and padding
    d_PadCenterString(stats, "PLAYER STATISTICS", 30, '=');
    d_AppendString(stats, "\n\n", 0);

    // Health percentage with padding
    d_PadLeftString(stats, "Health", 12, ' ');
    d_AppendString(stats, ": ", 0);
    d_AppendFloat(stats, 85.75f, 1);
    d_AppendString(stats, "%\n", 0);

    // Damage with padding
    d_PadLeftString(stats, "Damage", 12, ' ');
    d_AppendString(stats, ": ", 0);
    d_AppendFloat(stats, 47.333f, 2);
    d_AppendString(stats, "\n", 0);

    // Experience with no decimals
    d_PadLeftString(stats, "Experience", 12, ' ');
    d_AppendString(stats, ": ", 0);
    d_AppendFloat(stats, 1024.89f, 0);
    d_AppendString(stats, " XP\n", 0);

    const char* result = d_PeekString(stats);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Float Padding Stats:\n", 0);
    d_AppendString(log_msg, result, 0);
    printf("%s\n", d_PeekString(log_msg));
    d_DestroyString(log_msg);

    // Verify float formatting with padding
    TEST_ASSERT(strstr(result, "      Health: 85.8%") != NULL, "Float health Should be 85.8%");
    TEST_ASSERT(strstr(result, "      Damage: 47.33") != NULL, "Float damage Should be 47.33");
    TEST_ASSERT(strstr(result, "  Experience: 1025 XP") != NULL, "Float experience Should be 1025 XP");

    log_test_result("float_padding_integration", true, "Float padding integration works correctly");
    d_DestroyString(stats);
    return 1;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main(void)
{
    TEST_SUITE_START("String Padding Function Tests");

    LOG("Starting comprehensive string padding function tests");

    RUN_TEST(test_pad_left_basic);
    RUN_TEST(test_pad_left_no_padding_needed);
    RUN_TEST(test_pad_left_text_longer_than_width);
    RUN_TEST(test_pad_left_zero_width);
    RUN_TEST(test_pad_left_different_characters);
    RUN_TEST(test_pad_left_single_character);
    RUN_TEST(test_pad_left_null_safety);

    RUN_TEST(test_pad_right_basic);
    RUN_TEST(test_pad_right_no_padding_needed);
    RUN_TEST(test_pad_right_text_longer_than_width);
    RUN_TEST(test_pad_right_spaces);
    RUN_TEST(test_pad_right_append_to_existing);
    RUN_TEST(test_pad_right_null_safety);

    RUN_TEST(test_pad_center_even_padding);
    RUN_TEST(test_pad_center_odd_padding);
    RUN_TEST(test_pad_center_no_padding_needed);
    RUN_TEST(test_pad_center_text_longer_than_width);
    RUN_TEST(test_pad_center_single_character_text);
    RUN_TEST(test_pad_center_single_character_text_odd);
    RUN_TEST(test_pad_center_empty_string);
    RUN_TEST(test_pad_center_null_safety);

    RUN_TEST(test_rpg_table_formatting);
    RUN_TEST(test_rpg_menu_system);
    RUN_TEST(test_rpg_dialogue_box);

    RUN_TEST(test_float_padding_integration);

    TEST_SUITE_END();
}
