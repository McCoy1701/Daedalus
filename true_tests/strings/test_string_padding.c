// File: true_tests/strings/test_string_padding.c - Tests for string padding functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../include/Daedalus.h"

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
// d_StringPadLeft Tests
// ============================================================================

void test_pad_left_basic() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadLeft(sb, "Hi", 5, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "...Hi", "Basic left padding failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_left_no_padding_needed() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadLeft(sb, "Hello", 5, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Hello", "No padding needed case failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_left_text_longer_than_width() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadLeft(sb, "VeryLongText", 5, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "VeryLongText", "Text longer than width failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_left_zero_width() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadLeft(sb, "Test", 0, '.');
    TEST_ASSERT(d_StringLen(sb) == 0, "Zero width should not add anything");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_left_different_characters() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadLeft(sb, "XP", 6, '*');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "****XP", "Different pad character failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_left_single_character() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadLeft(sb, "A", 10, '-');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "---------A", "Single character padding failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_left_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_StringPadLeft(NULL, "test", 5, '.');

    dString_t* sb = d_StringCreate();
    d_StringPadLeft(sb, NULL, 5, '.');
    TEST_ASSERT(d_StringLen(sb) == 0, "NULL text should not modify string");

    d_StringDestroy(sb);
    TEST_PASS();
}

// ============================================================================
// d_StringPadRight Tests
// ============================================================================

void test_pad_right_basic() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadRight(sb, "Hi", 5, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Hi...", "Basic right padding failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_right_no_padding_needed() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadRight(sb, "Hello", 5, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Hello", "No padding needed case failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_right_text_longer_than_width() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadRight(sb, "VeryLongText", 5, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "VeryLongText", "Text longer than width failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_right_spaces() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadRight(sb, "Name", 12, ' ');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Name        ", "Space padding failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_right_append_to_existing() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringAddStr(sb, "Start: ", 0);
    d_StringPadRight(sb, "Item", 8, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Start: Item....", "Append to existing failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_right_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_StringPadRight(NULL, "test", 5, '.');

    dString_t* sb = d_StringCreate();
    d_StringPadRight(sb, NULL, 5, '.');
    TEST_ASSERT(d_StringLen(sb) == 0, "NULL text should not modify string");

    d_StringDestroy(sb);
    TEST_PASS();
}

// ============================================================================
// d_StringPadCenter Tests
// ============================================================================

void test_pad_center_even_padding() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadCenter(sb, "Hi", 6, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "..Hi..", "Even center padding failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_center_odd_padding() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadCenter(sb, "Hi", 7, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "..Hi...", "Odd center padding failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_center_no_padding_needed() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadCenter(sb, "Hello", 5, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Hello", "No padding needed case failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_center_text_longer_than_width() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadCenter(sb, "VeryLongText", 5, '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "VeryLongText", "Text longer than width failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_center_single_character_text() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadCenter(sb, "X", 9, '-');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "----X----", "Single character centering failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_center_single_character_text_odd() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadCenter(sb, "X", 8, '-');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "---X----", "Single character odd centering failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_center_empty_string() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringPadCenter(sb, "", 5, '*');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "*****", "Empty string centering failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_pad_center_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_StringPadCenter(NULL, "test", 5, '.');

    dString_t* sb = d_StringCreate();
    d_StringPadCenter(sb, NULL, 5, '.');
    TEST_ASSERT(d_StringLen(sb) == 0, "NULL text should not modify string");

    d_StringDestroy(sb);
    TEST_PASS();
}

// ============================================================================
// RPG-Specific Integration Tests
// ============================================================================

void test_rpg_table_formatting() {
    TEST_START();

    dString_t* table = d_StringCreate();

    // Create a character stats table
    d_StringPadCenter(table, "CHARACTER STATS", 40, '=');
    d_StringAddChar(table, '\n');

    // Column headers
    d_StringPadLeft(table, "Attribute", 15, ' ');
    d_StringAddStr(table, " | ", 0);
    d_StringPadRight(table, "Value", 10, ' ');
    d_StringAddStr(table, " | ", 0);
    d_StringPadCenter(table, "Status", 10, ' ');
    d_StringAddStr(table, "\n", 0);

    // Separator line
    d_StringRepeat(table, '-', 40);
    d_StringAddChar(table, '\n');

    // Data rows
    d_StringPadLeft(table, "Strength", 15, ' ');
    d_StringAddStr(table, " | ", 0);
    d_StringPadRight(table, "18", 10, ' ');
    d_StringAddStr(table, " | ", 0);
    d_StringPadCenter(table, "Strong", 10, ' ');
    d_StringAddStr(table, "\n", 0);

    d_StringPadLeft(table, "Dexterity", 15, ' ');
    d_StringAddStr(table, " | ", 0);
    d_StringPadRight(table, "14", 10, ' ');
    d_StringAddStr(table, " | ", 0);
    d_StringPadCenter(table, "Good", 10, ' ');
    d_StringAddStr(table, "\n", 0);

    const char* result = d_StringPeek(table);
    printf("Table After Padding:\n%s\n", result);

    // Verify the table structure
    TEST_ASSERT(strstr(result, "=======CHARACTER STATS========") != NULL, "Header centering failed");
    TEST_ASSERT(strstr(result, "     Strength |") != NULL, "Left padding failed");
    TEST_ASSERT(strstr(result, "18         |") != NULL, "Right padding failed");
    TEST_ASSERT(strstr(result, "  Strong  ") != NULL, "Center padding failed");

    d_StringDestroy(table);
    TEST_PASS();
}

void test_rpg_menu_system() {
    TEST_START();

    dString_t* menu = d_StringCreate();

    // Menu title
    d_StringPadCenter(menu, "MAIN MENU", 30, '*');
    d_StringAddStr(menu, "\n\n", 0);

    // Menu options
    const char* options[] = {"New Game", "Load Game", "Settings", "Quit"};
    for (int i = 0; i < 4; i++) {
        d_StringAddStr(menu, "[", 0);
        d_StringAddInt(menu, i + 1);
        d_StringAddStr(menu, "] ", 0);
        d_StringPadRight(menu, options[i], 20, '.');
        d_StringAddStr(menu, "\n", 0);
    }

    const char* result = d_StringPeek(menu);
    printf("Menu After Padding:\n%s\n", result);

    // Verify menu formatting
    TEST_ASSERT(strstr(result, "**********MAIN MENU***********") != NULL, "Menu title failed");
    TEST_ASSERT(strstr(result, "[1] New Game............") != NULL, "Menu option 1 failed");
    TEST_ASSERT(strstr(result, "[4] Quit................") != NULL, "Menu option 4 failed");

    d_StringDestroy(menu);
    TEST_PASS();
}

void test_rpg_dialogue_box() {
    TEST_START();

    dString_t* dialogue = d_StringCreate();

    // Create dialogue box border
    d_StringRepeat(dialogue, '+', 50);
    d_StringAddChar(dialogue, '\n');

    // Speaker name centered
    d_StringAddChar(dialogue, '|');
    d_StringPadCenter(dialogue, "Royal Guard Captain", 48, ' ');
    d_StringAddStr(dialogue, "|\n", 0);

    // Separator
    d_StringAddChar(dialogue, '|');
    d_StringRepeat(dialogue, '-', 48);
    d_StringAddStr(dialogue, "|\n", 0);

    // Dialogue text (left-aligned)
    d_StringAddChar(dialogue, '|');
    d_StringAddChar(dialogue, ' ');
    d_StringPadRight(dialogue, "\"Halt! None shall pass without", 47, ' ');
    d_StringAddStr(dialogue, "|\n", 0);

    d_StringAddChar(dialogue, '|');
    d_StringAddChar(dialogue, ' ');
    d_StringPadRight(dialogue, "the King's seal!\"", 47, ' ');
    d_StringAddStr(dialogue, "|\n", 0);

    // Bottom border
    d_StringRepeat(dialogue, '+', 50);
    d_StringAddChar(dialogue, '\n');
    printf("Dialogue After Padding:\n%s\n", d_StringPeek(dialogue));

    const char* result = d_StringPeek(dialogue);

    // Verify dialogue box structure
    TEST_ASSERT(strstr(result, "|              Royal Guard Captain               |") != NULL, "Speaker name centering failed");
    TEST_ASSERT(strstr(result, "| \"Halt! None shall pass without                 |") != NULL, "Dialogue text alignment failed");

    d_StringDestroy(dialogue);
    TEST_PASS();
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    printf("Running String Padding Function Tests...\n");
    printf("========================================\n");

    // Pad Left tests
    printf("\n--- d_StringPadLeft Tests ---\n");
    test_pad_left_basic();
    test_pad_left_no_padding_needed();
    test_pad_left_text_longer_than_width();
    test_pad_left_zero_width();
    test_pad_left_different_characters();
    test_pad_left_single_character();
    test_pad_left_null_safety();

    // Pad Right tests
    printf("\n--- d_StringPadRight Tests ---\n");
    test_pad_right_basic();
    test_pad_right_no_padding_needed();
    test_pad_right_text_longer_than_width();
    test_pad_right_spaces();
    test_pad_right_append_to_existing();
    test_pad_right_null_safety();

    // Pad Center tests
    printf("\n--- d_StringPadCenter Tests ---\n");
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
