// File: true_tests/strings/test_string_advanced.c - Tests for advanced string functions

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
// d_FormatString Tests
// ============================================================================

void test_string_format_basic() {
    TEST_START();

    dString_t* sb = d_InitString();
    TEST_ASSERT(sb != NULL, "Failed to create string builder");

    d_FormatString(sb, "Hello %s!", "World");
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello World!", "Basic string formatting failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_string_format_integers() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_FormatString(sb, "Level %d character with %d health", 42, 100);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Level 42 character with 100 health", "Integer formatting failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_string_format_append() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_AppendString(sb, "Start: ", 0);
    d_FormatString(sb, "Value=%d", 123);
    d_AppendString(sb, " End", 0);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Start: Value=123 End", "Format append failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_string_format_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_FormatString(NULL, "test");

    dString_t* sb = d_InitString();
    d_FormatString(sb, NULL);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Null format should not modify string");

    d_DestroyString(sb);
    TEST_PASS();
}

// ============================================================================
// d_AppendProgressBar Tests
// ============================================================================

void test_progress_bar_basic() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 50, 100, 10, '#', '-');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "[#####-----]", "Basic progress bar failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_progress_bar_full() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 100, 100, 8, '=', '.');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "[========]", "Full progress bar failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_progress_bar_empty() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 0, 100, 6, '*', ' ');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "[      ]", "Empty progress bar failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_progress_bar_overflow() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 150, 100, 5, '+', '-');
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "[+++++]", "Overflow progress bar failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_progress_bar_partial() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_AppendProgressBar(sb, 33, 100, 12, '#', '-');
    // 33% of 12 = 3.96, should round down to 3
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "[###---------]", "Partial progress bar failed");

    d_DestroyString(sb);
    TEST_PASS();
}


void test_progress_bar_null_safety() {
    TEST_START();

    // Should not crash with NULL
    d_AppendProgressBar(NULL, 50, 100, 10, '#', '-');

    dString_t* sb = d_InitString();
    size_t original_len = d_GetStringLength(sb);

    // Invalid parameters should not modify string
    d_AppendProgressBar(sb, 50, 0, 10, '#', '-');  // max = 0
    d_AppendProgressBar(sb, 50, 100, 0, '#', '-'); // width = 0
    d_AppendProgressBar(sb, 50, 100, -5, '#', '-'); // negative width

    TEST_ASSERT(d_GetStringLength(sb) == original_len, "Invalid parameters should not modify string");

    d_DestroyString(sb);
    TEST_PASS();
}

// ============================================================================
// d_TemplateString Tests
// ============================================================================

void test_template_basic() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* keys[] = {"name", "level"};
    const char* values[] = {"Alice", "10"};

    d_TemplateString(sb, "Hello {name}, you are level {level}!", keys, values, 2);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello Alice, you are level 10!", "Basic template failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_template_missing_keys() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* keys[] = {"name"};
    const char* values[] = {"Bob"};

    d_TemplateString(sb, "Hello {name}, you have {gold} gold!", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello Bob, you have {gold} gold!", "Missing key handling failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_template_no_placeholders() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* keys[] = {"unused"};
    const char* values[] = {"value"};

    d_TemplateString(sb, "No placeholders here!", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "No placeholders here!", "No placeholders failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_template_empty_keys() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* keys[] = {""};
    const char* values[] = {"empty"};

    d_TemplateString(sb, "Test {} placeholder", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Test empty placeholder", "Empty key failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_template_multiple_same_key() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* keys[] = {"item"};
    const char* values[] = {"sword"};

    d_TemplateString(sb, "You have a {item}. The {item} is sharp!", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "You have a sword. The sword is sharp!", "Multiple same key failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_template_nested_braces() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* keys[] = {"name"};
    const char* values[] = {"Alice"};

    d_TemplateString(sb, "Hello {name}! { This is not a placeholder }", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello Alice! { This is not a placeholder }", "Nested braces failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_template_rpg_scenario() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* keys[] = {"player", "enemy", "damage", "weapon", "critical"};
    const char* values[] = {"Warrior", "Goblin", "25", "Iron Sword", "CRITICAL"};

    d_TemplateString(sb,
        "{player} attacks {enemy} with {weapon} for {critical} {damage} damage!",
        keys, values, 5);
    printf("RPG Combat Scenario:\n%s\n", d_PeekString(sb));

    TEST_ASSERT_STR_EQ(d_PeekString(sb),
        "Warrior attacks Goblin with Iron Sword for CRITICAL 25 damage!",
        "RPG scenario template failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_template_faction_dialogue() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* keys[] = {"faction", "player", "reputation", "quest"};
    const char* values[] = {"Royal Loyalists", "Hero", "trusted ally", "retrieve the crown"};

    d_TemplateString(sb,
        "The {faction} representative nods.\n"
        "\"Greetings, {player}. As a {reputation}, we need you to {quest}.\"",
        keys, values, 4);
    printf("Faction Dialogue:\n%s\n", d_PeekString(sb));
    const char* expected =
        "The Royal Loyalists representative nods.\n"
        "\"Greetings, Hero. As a trusted ally, we need you to retrieve the crown.\"";

    TEST_ASSERT_STR_EQ(d_PeekString(sb), expected, "Faction dialogue template failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_template_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_TemplateString(NULL, "test {key}", NULL, NULL, 0);

    dString_t* sb = d_InitString();
    d_TemplateString(sb, NULL, NULL, NULL, 0);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Null template should not modify string");

    // Test with valid sb but null keys/values
    d_TemplateString(sb, "test {key}", NULL, NULL, 1);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "test {key}", "Null keys should leave placeholders unchanged");

    d_DestroyString(sb);
    TEST_PASS();
}

// ============================================================================
// Integration Tests
// ============================================================================

void test_integration_rpg_character_sheet() {
    TEST_START();

    dString_t* sheet = d_InitString();

    // Character name and title
    const char* keys[] = {"name", "title", "level", "health", "mana", "faction"};
    const char* values[] = {"Sir Galahad", "Knight of the Round Table", "15", "180", "50", "Royal Loyalists"};

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
    printf("Character Sheet:\n%s\n", d_PeekString(sheet));

    // Verify the complete character sheet
    const char* expected_start = "=== Sir Galahad ===\nKnight of the Round Table\n\nLevel: 15\n";
    TEST_ASSERT(strncmp(d_PeekString(sheet), expected_start, strlen(expected_start)) == 0,
                "Character sheet header failed");

    // Check that it contains the progress bars (using ASCII characters now)
    const char* full_result = d_PeekString(sheet);
    TEST_ASSERT(strstr(full_result, "[==================--]") != NULL, "Health bar not found");
    TEST_ASSERT(strstr(full_result, "[**********----------]") != NULL, "Mana bar not found");
    TEST_ASSERT(strstr(full_result, "Faction: Royal Loyalists") != NULL, "Faction not found");

    d_DestroyString(sheet);
    TEST_PASS();
}


// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    printf("Running Advanced String Function Tests...\n");
    printf("=========================================\n");

    // Format tests
    printf("\n--- d_FormatString Tests ---\n");
    test_string_format_basic();
    test_string_format_integers();
    test_string_format_append();
    test_string_format_null_safety();

    // Progress bar tests
    printf("\n--- d_AppendProgressBar Tests ---\n");
    test_progress_bar_basic();
    test_progress_bar_full();
    test_progress_bar_empty();
    test_progress_bar_overflow();
    test_progress_bar_partial();
    test_progress_bar_null_safety();

    // Template tests
    printf("\n--- d_TemplateString Tests ---\n");
    test_template_basic();
    test_template_missing_keys();
    test_template_no_placeholders();
    test_template_empty_keys();
    test_template_multiple_same_key();
    test_template_nested_braces();
    test_template_rpg_scenario();
    test_template_faction_dialogue();
    test_template_null_safety();

    // Integration tests
    printf("\n--- Integration Tests ---\n");
    test_integration_rpg_character_sheet();

    printf("\n=========================================\n");
    printf("Tests completed: %d/%d passed\n", tests_passed, tests_run);

    if (tests_passed == tests_run) {
        printf("All tests PASSED! ✓\n");
        return 0;
    } else {
        printf("Some tests FAILED! ✗\n");
        return 1;
    }
}
