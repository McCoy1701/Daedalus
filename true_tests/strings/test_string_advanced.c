// File: true_tests/strings/test_string_advanced.c - Tests for advanced string functions

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
// d_StringFormat Tests
// ============================================================================

void test_string_format_basic() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    TEST_ASSERT(sb != NULL, "Failed to create string builder");

    d_StringFormat(sb, "Hello %s!", "World");
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Hello World!", "Basic string formatting failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_string_format_integers() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringFormat(sb, "Level %d character with %d health", 42, 100);
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Level 42 character with 100 health", "Integer formatting failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_string_format_append() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringAddStr(sb, "Start: ", 0);
    d_StringFormat(sb, "Value=%d", 123);
    d_StringAddStr(sb, " End", 0);
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Start: Value=123 End", "Format append failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_string_format_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_StringFormat(NULL, "test");

    dString_t* sb = d_StringCreate();
    d_StringFormat(sb, NULL);
    TEST_ASSERT(d_StringLen(sb) == 0, "Null format should not modify string");

    d_StringDestroy(sb);
    TEST_PASS();
}

// ============================================================================
// d_StringProgressBar Tests
// ============================================================================

void test_progress_bar_basic() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringProgressBar(sb, 50, 100, 10, '#', '-');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "[#####-----]", "Basic progress bar failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_progress_bar_full() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringProgressBar(sb, 100, 100, 8, '=', '.');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "[========]", "Full progress bar failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_progress_bar_empty() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringProgressBar(sb, 0, 100, 6, '*', ' ');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "[      ]", "Empty progress bar failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_progress_bar_overflow() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringProgressBar(sb, 150, 100, 5, '+', '-');
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "[+++++]", "Overflow progress bar failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_progress_bar_partial() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    d_StringProgressBar(sb, 33, 100, 12, '#', '-');
    // 33% of 12 = 3.96, should round down to 3
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "[###---------]", "Partial progress bar failed");

    d_StringDestroy(sb);
    TEST_PASS();
}


void test_progress_bar_null_safety() {
    TEST_START();

    // Should not crash with NULL
    d_StringProgressBar(NULL, 50, 100, 10, '#', '-');

    dString_t* sb = d_StringCreate();
    size_t original_len = d_StringLen(sb);

    // Invalid parameters should not modify string
    d_StringProgressBar(sb, 50, 0, 10, '#', '-');  // max = 0
    d_StringProgressBar(sb, 50, 100, 0, '#', '-'); // width = 0
    d_StringProgressBar(sb, 50, 100, -5, '#', '-'); // negative width

    TEST_ASSERT(d_StringLen(sb) == original_len, "Invalid parameters should not modify string");

    d_StringDestroy(sb);
    TEST_PASS();
}

// ============================================================================
// d_StringTemplate Tests
// ============================================================================

void test_template_basic() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    const char* keys[] = {"name", "level"};
    const char* values[] = {"Alice", "10"};

    d_StringTemplate(sb, "Hello {name}, you are level {level}!", keys, values, 2);
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Hello Alice, you are level 10!", "Basic template failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_template_missing_keys() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    const char* keys[] = {"name"};
    const char* values[] = {"Bob"};

    d_StringTemplate(sb, "Hello {name}, you have {gold} gold!", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Hello Bob, you have {gold} gold!", "Missing key handling failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_template_no_placeholders() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    const char* keys[] = {"unused"};
    const char* values[] = {"value"};

    d_StringTemplate(sb, "No placeholders here!", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "No placeholders here!", "No placeholders failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_template_empty_keys() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    const char* keys[] = {""};
    const char* values[] = {"empty"};

    d_StringTemplate(sb, "Test {} placeholder", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Test empty placeholder", "Empty key failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_template_multiple_same_key() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    const char* keys[] = {"item"};
    const char* values[] = {"sword"};

    d_StringTemplate(sb, "You have a {item}. The {item} is sharp!", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "You have a sword. The sword is sharp!", "Multiple same key failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_template_nested_braces() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    const char* keys[] = {"name"};
    const char* values[] = {"Alice"};

    d_StringTemplate(sb, "Hello {name}! { This is not a placeholder }", keys, values, 1);
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "Hello Alice! { This is not a placeholder }", "Nested braces failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_template_rpg_scenario() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    const char* keys[] = {"player", "enemy", "damage", "weapon", "critical"};
    const char* values[] = {"Warrior", "Goblin", "25", "Iron Sword", "CRITICAL"};

    d_StringTemplate(sb,
        "{player} attacks {enemy} with {weapon} for {critical} {damage} damage!",
        keys, values, 5);
    printf("RPG Combat Scenario:\n%s\n", d_StringPeek(sb));

    TEST_ASSERT_STR_EQ(d_StringPeek(sb),
        "Warrior attacks Goblin with Iron Sword for CRITICAL 25 damage!",
        "RPG scenario template failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_template_faction_dialogue() {
    TEST_START();

    dString_t* sb = d_StringCreate();
    const char* keys[] = {"faction", "player", "reputation", "quest"};
    const char* values[] = {"Royal Loyalists", "Hero", "trusted ally", "retrieve the crown"};

    d_StringTemplate(sb,
        "The {faction} representative nods.\n"
        "\"Greetings, {player}. As a {reputation}, we need you to {quest}.\"",
        keys, values, 4);
    printf("Faction Dialogue:\n%s\n", d_StringPeek(sb));
    const char* expected =
        "The Royal Loyalists representative nods.\n"
        "\"Greetings, Hero. As a trusted ally, we need you to retrieve the crown.\"";

    TEST_ASSERT_STR_EQ(d_StringPeek(sb), expected, "Faction dialogue template failed");

    d_StringDestroy(sb);
    TEST_PASS();
}

void test_template_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_StringTemplate(NULL, "test {key}", NULL, NULL, 0);

    dString_t* sb = d_StringCreate();
    d_StringTemplate(sb, NULL, NULL, NULL, 0);
    TEST_ASSERT(d_StringLen(sb) == 0, "Null template should not modify string");

    // Test with valid sb but null keys/values
    d_StringTemplate(sb, "test {key}", NULL, NULL, 1);
    TEST_ASSERT_STR_EQ(d_StringPeek(sb), "test {key}", "Null keys should leave placeholders unchanged");

    d_StringDestroy(sb);
    TEST_PASS();
}

// ============================================================================
// Integration Tests
// ============================================================================

void test_integration_rpg_character_sheet() {
    TEST_START();

    dString_t* sheet = d_StringCreate();

    // Character name and title
    const char* keys[] = {"name", "title", "level", "health", "mana", "faction"};
    const char* values[] = {"Sir Galahad", "Knight of the Round Table", "15", "180", "50", "Royal Loyalists"};

    d_StringTemplate(sheet, "=== {name} ===\n{title}\n\n", keys, values, 6);

    // Stats with progress bars
    d_StringAddStr(sheet, "Level: ", 0);
    d_StringFormat(sheet, "%s\n", values[2]);

    d_StringAddStr(sheet, "Health: ", 0);
    d_StringProgressBar(sheet, 180, 200, 20, '=', '-');
    d_StringFormat(sheet, " %s/200\n", values[3]);

    d_StringAddStr(sheet, "Mana:   ", 0);
    d_StringProgressBar(sheet, 50, 100, 20, '*', '-');
    d_StringFormat(sheet, " %s/100\n", values[4]);

    d_StringTemplate(sheet, "\nFaction: {faction}\n", keys, values, 6);
    printf("Character Sheet:\n%s\n", d_StringPeek(sheet));

    // Verify the complete character sheet
    const char* expected_start = "=== Sir Galahad ===\nKnight of the Round Table\n\nLevel: 15\n";
    TEST_ASSERT(strncmp(d_StringPeek(sheet), expected_start, strlen(expected_start)) == 0,
                "Character sheet header failed");

    // Check that it contains the progress bars (using ASCII characters now)
    const char* full_result = d_StringPeek(sheet);
    TEST_ASSERT(strstr(full_result, "[==================--]") != NULL, "Health bar not found");
    TEST_ASSERT(strstr(full_result, "[**********----------]") != NULL, "Mana bar not found");
    TEST_ASSERT(strstr(full_result, "Faction: Royal Loyalists") != NULL, "Faction not found");

    d_StringDestroy(sheet);
    TEST_PASS();
}


// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    printf("Running Advanced String Function Tests...\n");
    printf("=========================================\n");

    // Format tests
    printf("\n--- d_StringFormat Tests ---\n");
    test_string_format_basic();
    test_string_format_integers();
    test_string_format_append();
    test_string_format_null_safety();

    // Progress bar tests
    printf("\n--- d_StringProgressBar Tests ---\n");
    test_progress_bar_basic();
    test_progress_bar_full();
    test_progress_bar_empty();
    test_progress_bar_overflow();
    test_progress_bar_partial();
    test_progress_bar_null_safety();

    // Template tests
    printf("\n--- d_StringTemplate Tests ---\n");
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
