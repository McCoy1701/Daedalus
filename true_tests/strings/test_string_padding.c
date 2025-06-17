// File: true_tests/strings/test_string_padding.c - Tests for string padding functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Daedalus.h"

// Test result counters
static int tests_run = 0;
static int tests_passed = 0;

// Test helper macros
#define TEST_START() tests_run++
#define TEST_PASS() tests_passed++
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s - %s\n", __func__, message); \
            return; \
        } \
    } while(0)

#define TEST_ASSERT_STR_EQ(actual, expected, message) \
    do { \
        if (strcmp(actual, expected) != 0) { \
            printf("FAIL: %s - %s\n", __func__, message); \
            printf("  Expected: '%s'\n", expected); \
            printf("  Actual:   '%s'\n", actual); \
            return; \
        } \
    } while(0)

// ============================================================================
// d_PadLeftString Tests
// ============================================================================

void test_pad_left_basic() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "Hi", 5, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "...Hi", "Basic left padding failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_left_no_padding_needed() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "Hello", 5, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello", "No padding needed case failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_left_text_longer_than_width() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "VeryLongText", 5, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "VeryLongText", "Text longer than width failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_left_zero_width() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "Test", 0, '.');
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Zero width should not add anything");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_left_different_characters() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "XP", 6, '*');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "****XP", "Different pad character failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_left_single_character() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, "A", 10, '-');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "---------A", "Single character padding failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_left_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_PadLeftString(NULL, "test", 5, '.');

    dString_t* sb = d_InitString();
    d_PadLeftString(sb, NULL, 5, '.');
    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL text should not modify string");

    d_DestroyString(sb);
    TEST_PASS();
}

// ============================================================================
// d_PadRightString Tests
// ============================================================================

void test_pad_right_basic() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadRightString(sb, "Hi", 5, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hi...", "Basic right padding failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_right_no_padding_needed() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadRightString(sb, "Hello", 5, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello", "No padding needed case failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_right_text_longer_than_width() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadRightString(sb, "VeryLongText", 5, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "VeryLongText", "Text longer than width failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_right_spaces() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadRightString(sb, "Name", 12, ' ');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Name        ", "Space padding failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_right_append_to_existing() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_AppendString(sb, "Start: ", 0);
    d_PadRightString(sb, "Item", 8, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Start: Item....", "Append to existing failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_right_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_PadRightString(NULL, "test", 5, '.');

    dString_t* sb = d_InitString();
    d_PadRightString(sb, NULL, 5, '.');
    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL text should not modify string");

    d_DestroyString(sb);
    TEST_PASS();
}

// ============================================================================
// d_PadCenterString Tests
// ============================================================================

void test_pad_center_even_padding() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "Hi", 6, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "..Hi..", "Even center padding failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_center_odd_padding() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "Hi", 7, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "..Hi...", "Odd center padding failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_center_no_padding_needed() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "Hello", 5, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello", "No padding needed case failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_center_text_longer_than_width() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "VeryLongText", 5, '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "VeryLongText", "Text longer than width failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_center_single_character_text() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "X", 9, '-');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "----X----", "Single character centering failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_center_single_character_text_odd() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "X", 8, '-');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "---X----", "Single character odd centering failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_center_empty_string() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, "", 5, '*');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "*****", "Empty string centering failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_pad_center_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_PadCenterString(NULL, "test", 5, '.');

    dString_t* sb = d_InitString();
    d_PadCenterString(sb, NULL, 5, '.');
    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL text should not modify string");

    d_DestroyString(sb);
    TEST_PASS();
}

// ============================================================================
// RPG-Specific Integration Tests
// ============================================================================

void test_rpg_table_formatting() {
    TEST_START();

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
    printf("Table After Padding:\n%s\n", result);

    // Verify the table structure
    TEST_ASSERT(strstr(result, "=======CHARACTER STATS========") != NULL, "Header centering failed");
    TEST_ASSERT(strstr(result, "     Strength |") != NULL, "Left padding failed");
    TEST_ASSERT(strstr(result, "18         |") != NULL, "Right padding failed");
    TEST_ASSERT(strstr(result, "  Strong  ") != NULL, "Center padding failed");

    d_DestroyString(table);
    TEST_PASS();
}

void test_rpg_menu_system() {
    TEST_START();

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
    printf("Menu After Padding:\n%s\n", result);

    // Verify menu formatting
    TEST_ASSERT(strstr(result, "**********MAIN MENU***********") != NULL, "Menu title failed");
    TEST_ASSERT(strstr(result, "[1] New Game............") != NULL, "Menu option 1 failed");
    TEST_ASSERT(strstr(result, "[4] Quit................") != NULL, "Menu option 4 failed");

    d_DestroyString(menu);
    TEST_PASS();
}

void test_rpg_dialogue_box() {
    TEST_START();

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
    printf("Dialogue After Padding:\n%s\n", d_PeekString(dialogue));

    const char* result = d_PeekString(dialogue);

    // Verify dialogue box structure
    TEST_ASSERT(strstr(result, "|              Royal Guard Captain               |") != NULL, "Speaker name centering failed");
    TEST_ASSERT(strstr(result, "| \"Halt! None shall pass without                 |") != NULL, "Dialogue text alignment failed");

    d_DestroyString(dialogue);
    TEST_PASS();
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    printf("Running String Padding Function Tests...\n");
    printf("========================================\n");

    // Pad Left tests
    printf("\n--- d_PadLeftString Tests ---\n");
    test_pad_left_basic();
    test_pad_left_no_padding_needed();
    test_pad_left_text_longer_than_width();
    test_pad_left_zero_width();
    test_pad_left_different_characters();
    test_pad_left_single_character();
    test_pad_left_null_safety();

    // Pad Right tests
    printf("\n--- d_PadRightString Tests ---\n");
    test_pad_right_basic();
    test_pad_right_no_padding_needed();
    test_pad_right_text_longer_than_width();
    test_pad_right_spaces();
    test_pad_right_append_to_existing();
    test_pad_right_null_safety();

    // Pad Center tests
    printf("\n--- d_PadCenterString Tests ---\n");
    test_pad_center_even_padding();
    test_pad_center_odd_padding();
    test_pad_center_no_padding_needed();
    test_pad_center_text_longer_than_width();
    test_pad_center_single_character_text();
    test_pad_center_single_character_text_odd();
    test_pad_center_empty_string();
    test_pad_center_null_safety();

    // RPG Integration tests
    printf("\n--- RPG Integration Tests ---\n");
    test_rpg_table_formatting();
    test_rpg_menu_system();
    test_rpg_dialogue_box();

    printf("\n========================================\n");
    printf("Tests completed: %d/%d passed\n", tests_passed, tests_run);

    if (tests_passed == tests_run) {
        printf("All tests PASSED! ✓\n");
        return 0;
    } else {
        printf("Some tests FAILED! ✗\n");
        return 1;
    }
}
