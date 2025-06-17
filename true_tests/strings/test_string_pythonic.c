// File: true_tests/strings/test_string_pythonic.c - Tests for Python-inspired string functions

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
// d_JoinStrings Tests
// ============================================================================

void test_join_basic() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* items[] = {"apple", "banana", "cherry"};
    d_JoinStrings(sb, items, 3, ", ");
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "apple, banana, cherry", "Basic join failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_join_single_item() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* items[] = {"lonely"};
    d_JoinStrings(sb, items, 1, ", ");
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "lonely", "Single item join failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_join_empty_separator() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* items[] = {"a", "b", "c"};
    d_JoinStrings(sb, items, 3, "");
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "abc", "Empty separator join failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_join_null_separator() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* items[] = {"hello", "world"};
    d_JoinStrings(sb, items, 2, NULL);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "helloworld", "NULL separator join failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_join_with_nulls() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* items[] = {"start", NULL, "end"};
    d_JoinStrings(sb, items, 3, "-");
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "start--end", "Join with NULL strings failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_join_different_separators() {
    TEST_START();

    dString_t* sb = d_InitString();
    const char* path_parts[] = {"home", "user", "documents", "file.txt"};
    d_JoinStrings(sb, path_parts, 4, "/");
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "home/user/documents/file.txt", "Path join failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_join_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_JoinStrings(NULL, NULL, 0, ",");

    dString_t* sb = d_InitString();
    d_JoinStrings(sb, NULL, 3, ",");
    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL array should not modify string");

    d_DestroyString(sb);
    TEST_PASS();
}

// ============================================================================
// d_SplitString Tests
// ============================================================================

void test_split_basic() {
    TEST_START();

    int count;
    char** result = d_SplitString("apple,banana,cherry", ",", &count);

    TEST_ASSERT(result != NULL, "Split result should not be NULL");
    TEST_ASSERT(count == 3, "Split should produce 3 parts");
    TEST_ASSERT_STR_EQ(result[0], "apple", "First part incorrect");
    TEST_ASSERT_STR_EQ(result[1], "banana", "Second part incorrect");
    TEST_ASSERT_STR_EQ(result[2], "cherry", "Third part incorrect");

    d_FreeSplitString(result, count);
    TEST_PASS();
}

void test_split_no_delimiter() {
    TEST_START();

    int count;
    char** result = d_SplitString("no delimiter here", ",", &count);

    TEST_ASSERT(result != NULL, "Split result should not be NULL");
    TEST_ASSERT(count == 1, "Split should produce 1 part");
    TEST_ASSERT_STR_EQ(result[0], "no delimiter here", "Single part incorrect");

    d_FreeSplitString(result, count);
    TEST_PASS();
}

void test_split_empty_parts() {
    TEST_START();

    int count;
    char** result = d_SplitString("a,,c", ",", &count);

    TEST_ASSERT(result != NULL, "Split result should not be NULL");
    TEST_ASSERT(count == 3, "Split should produce 3 parts");
    TEST_ASSERT_STR_EQ(result[0], "a", "First part incorrect");
    TEST_ASSERT_STR_EQ(result[1], "", "Second part should be empty");
    TEST_ASSERT_STR_EQ(result[2], "c", "Third part incorrect");

    d_FreeSplitString(result, count);
    TEST_PASS();
}

void test_split_multiple_character_delimiter() {
    TEST_START();

    int count;
    char** result = d_SplitString("one::two::three", "::", &count);

    TEST_ASSERT(result != NULL, "Split result should not be NULL");
    TEST_ASSERT(count == 3, "Split should produce 3 parts");
    TEST_ASSERT_STR_EQ(result[0], "one", "First part incorrect");
    TEST_ASSERT_STR_EQ(result[1], "two", "Second part incorrect");
    TEST_ASSERT_STR_EQ(result[2], "three", "Third part incorrect");

    d_FreeSplitString(result, count);
    TEST_PASS();
}

void test_split_starts_with_delimiter() {
    TEST_START();

    int count;
    char** result = d_SplitString(",start,end", ",", &count);

    TEST_ASSERT(result != NULL, "Split result should not be NULL");
    TEST_ASSERT(count == 3, "Split should produce 3 parts");
    TEST_ASSERT_STR_EQ(result[0], "", "First part should be empty");
    TEST_ASSERT_STR_EQ(result[1], "start", "Second part incorrect");
    TEST_ASSERT_STR_EQ(result[2], "end", "Third part incorrect");

    d_FreeSplitString(result, count);
    TEST_PASS();
}

void test_split_ends_with_delimiter() {
    TEST_START();

    int count;
    char** result = d_SplitString("start,end,", ",", &count);

    TEST_ASSERT(result != NULL, "Split result should not be NULL");
    TEST_ASSERT(count == 3, "Split should produce 3 parts");
    TEST_ASSERT_STR_EQ(result[0], "start", "First part incorrect");
    TEST_ASSERT_STR_EQ(result[1], "end", "Second part incorrect");
    TEST_ASSERT_STR_EQ(result[2], "", "Third part should be empty");

    d_FreeSplitString(result, count);
    TEST_PASS();
}

void test_split_null_safety() {
    TEST_START();

    int count;
    char** result;

    // Test NULL text
    result = d_SplitString(NULL, ",", &count);
    TEST_ASSERT(result == NULL, "NULL text should return NULL");
    TEST_ASSERT(count == 0, "NULL text should set count to 0");

    // Test NULL delimiter
    result = d_SplitString("test", NULL, &count);
    TEST_ASSERT(result == NULL, "NULL delimiter should return NULL");
    TEST_ASSERT(count == 0, "NULL delimiter should set count to 0");

    // Test NULL count pointer
    result = d_SplitString("test", ",", NULL);
    TEST_ASSERT(result == NULL, "NULL count should return NULL");

    TEST_PASS();
}

// ============================================================================
// d_SliceString Tests
// ============================================================================

void test_slice_basic() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello World", 6, 11);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "World", "Basic slice failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_beginning() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello World", 0, 5);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello", "Beginning slice failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_middle() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello World", 2, 9);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "llo Wor", "Middle slice failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_negative_indices() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello", -3, -1);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "llo", "Negative indices slice failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_negative_to_end() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello World", -5, 11);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "World", "Negative start to end slice failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_whole_string() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello", 0, 5);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello", "Whole string slice failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_empty_result() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello", 3, 3);  // start == end
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "", "Empty slice failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_reverse_indices() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello", 4, 2);  // start > end
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "", "Reverse indices should produce empty string");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_out_of_bounds() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello", 10, 20);  // start beyond string
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "", "Out of bounds slice should be empty");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_negative_extreme() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_SliceString(sb, "Hello", -100, -1);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Hello", "Extreme negative start should be clamped");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_append_to_existing() {
    TEST_START();

    dString_t* sb = d_InitString();
    d_AppendString(sb, "Start: ", 0);
    d_SliceString(sb, "Hello World", 6, 11);
    TEST_ASSERT_STR_EQ(d_PeekString(sb), "Start: World", "Slice append failed");

    d_DestroyString(sb);
    TEST_PASS();
}

void test_slice_null_safety() {
    TEST_START();

    // Should not crash with NULL parameters
    d_SliceString(NULL, "test", 0, 4);

    dString_t* sb = d_InitString();
    d_SliceString(sb, NULL, 0, 4);
    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL text should not modify string");

    d_DestroyString(sb);
    TEST_PASS();
}

// ============================================================================
// RPG Integration Tests
// ============================================================================

void test_rpg_inventory_management() {
    TEST_START();

    // Simulate inventory items
    const char* inventory[] = {"Iron Sword", "Health Potion", "Magic Ring", "Gold Coins"};
    dString_t* list = d_InitString();
    d_AppendString(list, "Inventory: ", 0);
    d_JoinStrings(list, inventory, 4, ", ");
    printf("List After Join: \n%s\n", d_PeekString(list));

    const char* result = d_PeekString(list);
    TEST_ASSERT(strstr(result, "Iron Sword, Health Potion, Magic Ring, Gold Coins") != NULL,
                "Inventory list creation failed");

    d_DestroyString(list);
    TEST_PASS();
}

void test_rpg_command_parsing() {
    TEST_START();

    // Parse a game command
    int count;
    char** parts = d_SplitString("attack goblin with sword", " ", &count);
    printf("Command Parts: \n");
    for (int i = 0; i < count; i++) {
        printf("-%s\t", parts[i]);
    }
    printf("\n");

    TEST_ASSERT(count == 4, "Command should have 4 parts");
    TEST_ASSERT_STR_EQ(parts[0], "attack", "Action incorrect");
    TEST_ASSERT_STR_EQ(parts[1], "goblin", "Target incorrect");
    TEST_ASSERT_STR_EQ(parts[2], "with", "Preposition incorrect");
    TEST_ASSERT_STR_EQ(parts[3], "sword", "Weapon incorrect");

    d_FreeSplitString(parts, count);
    // Ensure memory is freed
    TEST_PASS();
}

void test_rpg_name_generation() {
    TEST_START();

    dString_t* name = d_InitString();

    // Create a character name by slicing and joining parts
    d_SliceString(name, "Alexander", 0, 4);  // "Alex"
    d_AppendString(name, " ", 0);
    d_SliceString(name, "Thunderstrike", 0, 7);  // "Thunder"
    d_AppendString(name, "born", 0);
    printf("Name after Slicing: \n%s\n", d_PeekString(name));

    TEST_ASSERT_STR_EQ(d_PeekString(name), "Alex Thunderborn", "Name generation failed");

    d_DestroyString(name);
    TEST_PASS();
}

void test_rpg_dialogue_word_wrapping() {
    TEST_START();

    const char* long_dialogue = "The ancient wizard speaks: 'Young adventurer, your quest leads to dangerous lands beyond the misty mountains.'";

    // Split into words for wrapping
    int word_count;
    char** words = d_SplitString(long_dialogue, " ", &word_count);

    TEST_ASSERT(word_count > 10, "Should have many words");
    TEST_ASSERT_STR_EQ(words[0], "The", "First word incorrect");
    TEST_ASSERT_STR_EQ(words[word_count-1], "mountains.'", "Last word incorrect");

    // Rejoin first 5 words as a shorter line
    dString_t* short_line = d_InitString();
    d_JoinStrings(short_line, (const char**)words, 5, " ");

    TEST_ASSERT_STR_EQ(d_PeekString(short_line), "The ancient wizard speaks: 'Young",
                       "Word wrapping failed");

    d_FreeSplitString(words, word_count);
    d_DestroyString(short_line);
    TEST_PASS();
}

void test_rpg_file_path_handling() {
    TEST_START();

    // Build a save file path
    const char* path_parts[] = {"saves", "characters", "warrior_level_42.dat"};

    dString_t* path = d_InitString();
    d_JoinStrings(path, path_parts, 3, "/");
    printf("Path after joining: \n%s\n", d_PeekString(path));

    TEST_ASSERT_STR_EQ(d_PeekString(path), "saves/characters/warrior_level_42.dat",
                       "Path building failed");

    d_DestroyString(path);
    TEST_PASS();
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    printf("Running Python-inspired String Function Tests...\n");
    printf("===============================================\n");

    // Join tests
    printf("\n--- d_JoinStrings Tests ---\n");
    test_join_basic();
    test_join_single_item();
    test_join_empty_separator();
    test_join_null_separator();
    test_join_with_nulls();
    test_join_different_separators();
    test_join_null_safety();

    // Split tests
    printf("\n--- d_SplitString Tests ---\n");
    test_split_basic();
    test_split_no_delimiter();
    test_split_empty_parts();
    test_split_multiple_character_delimiter();
    test_split_starts_with_delimiter();
    test_split_ends_with_delimiter();
    test_split_null_safety();

    // Slice tests
    printf("\n--- d_SliceString Tests ---\n");
    test_slice_basic();
    test_slice_beginning();
    test_slice_middle();
    test_slice_negative_indices();
    test_slice_negative_to_end();
    test_slice_whole_string();
    test_slice_empty_result();
    test_slice_reverse_indices();
    test_slice_out_of_bounds();
    test_slice_negative_extreme();
    test_slice_append_to_existing();
    test_slice_null_safety();

    // RPG Integration tests
    printf("\n--- RPG Integration Tests ---\n");
    test_rpg_inventory_management();
    test_rpg_command_parsing();
    test_rpg_name_generation();
    test_rpg_dialogue_word_wrapping();
    test_rpg_file_path_handling();

    printf("\n===============================================\n");
    printf("Tests completed: %d/%d passed\n", tests_passed, tests_run);

    if (tests_passed == tests_run) {
        printf("All tests PASSED! ✓\n");
        return 0;
    } else {
        printf("Some tests FAILED! ✗\n");
        return 1;
    }
}
