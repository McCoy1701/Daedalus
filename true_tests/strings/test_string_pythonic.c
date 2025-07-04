// File: true_tests/strings/test_string_pythonic.c - Tests for Python-inspired string functions

#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )
#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

// Helper function to create test strings
dString_t* create_test_string(const char* initial_content)
{
    dString_t* str = d_InitString();
    if (initial_content) {
        d_AppendString(str, initial_content, 0);
    }
    return str;
}

// Helper function to log string contents
void log_string_content(const char* label, const dString_t* str)
{
    dString_t* log_message = d_InitString();
    d_AppendString(log_message, label, 0);
    d_AppendString(log_message, ": ", 0);
    if (str && d_PeekString(str)) {
        d_AppendString(log_message, "'", 0);
        d_AppendString(log_message, d_PeekString(str), 0);
        d_AppendString(log_message, "'", 0);
    } else {
        d_AppendString(log_message, "[NULL or EMPTY]", 0);
    }
    LOG(log_message->str);
    d_DestroyString(log_message);
}

// Helper function to log array contents
void log_string_array(const char* label, dArray_t* array)
{
    dString_t* log_message = d_InitString();
    d_AppendString(log_message, label, 0);
    if (array) {
        d_AppendString(log_message, " (count: ", 0);
        d_AppendInt(log_message, (int)array->count);
        d_AppendString(log_message, "): [", 0);

        for (size_t i = 0; i < array->count; i++) {
            if (i > 0) d_AppendString(log_message, ", ", 0);
            d_AppendString(log_message, "'", 0);

            dString_t** str_ptr = (dString_t**)d_IndexDataFromArray(array, i);
            if (str_ptr && *str_ptr) {
                d_AppendString(log_message, d_PeekString(*str_ptr), 0);
            } else {
                d_AppendString(log_message, "NULL", 0);
            }
            d_AppendString(log_message, "'", 0);
        }
        d_AppendString(log_message, "]", 0);
    } else {
        d_AppendString(log_message, ": [NULL]", 0);
    }
    LOG(log_message->str);
    d_DestroyString(log_message);
}

// =============================================================================
// d_JoinStrings TESTS
// =============================================================================

int test_join_basic(void)
{
    dString_t* sb = create_test_string(NULL);
    const char* items[] = {"apple", "banana", "cherry"};

    d_JoinStrings(sb, items, 3, ", ");
    log_string_content("Join result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "apple, banana, cherry") == 0, "Basic join failed");
    TEST_ASSERT(d_GetStringLength(sb) > 0, "Joined string should have content");

    d_DestroyString(sb);
    return 1;
}

int test_join_single_item(void)
{
    dString_t* sb = create_test_string(NULL);
    const char* items[] = {"lonely"};

    d_JoinStrings(sb, items, 1, ", ");
    log_string_content("Single item join result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "lonely") == 0, "Single item join failed");
    TEST_ASSERT(d_GetStringLength(sb) == 6, "Single item should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_join_empty_separator(void)
{
    dString_t* sb = create_test_string(NULL);
    const char* items[] = {"a", "b", "c"};

    d_JoinStrings(sb, items, 3, "");
    log_string_content("Empty separator join result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "abc") == 0, "Empty separator join failed");
    TEST_ASSERT(d_GetStringLength(sb) == 3, "Result should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_join_null_separator(void)
{
    dString_t* sb = create_test_string(NULL);
    const char* items[] = {"hello", "world"};

    d_JoinStrings(sb, items, 2, NULL);
    log_string_content("NULL separator join result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "helloworld") == 0, "NULL separator join failed");
    TEST_ASSERT(d_GetStringLength(sb) == 10, "Result should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_join_with_nulls(void)
{
    dString_t* sb = create_test_string(NULL);
    const char* items[] = {"start", NULL, "end"};

    d_JoinStrings(sb, items, 3, "-");
    log_string_content("Join with NULLs result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "start--end") == 0, "Join with NULL strings failed");
    TEST_ASSERT(d_GetStringLength(sb) == 10, "Result should handle NULL items correctly");

    d_DestroyString(sb);
    return 1;
}

int test_join_path_separator(void)
{
    dString_t* sb = create_test_string(NULL);
    const char* path_parts[] = {"home", "user", "documents", "file.txt"};

    d_JoinStrings(sb, path_parts, 4, "/");
    log_string_content("Path join result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "home/user/documents/file.txt") == 0, "Path join failed");
    TEST_ASSERT(strstr(d_PeekString(sb), "home/user") != NULL, "Path should contain proper segments");

    d_DestroyString(sb);
    return 1;
}

int test_join_null_safety(void)
{
    // Test with NULL string builder - should not crash
    d_JoinStrings(NULL, NULL, 0, ",");
    TEST_ASSERT(1, "NULL string builder should not crash");

    dString_t* sb = create_test_string(NULL);
    d_JoinStrings(sb, NULL, 3, ",");
    log_string_content("Join with NULL array result", sb);

    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL array should not modify string");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "String should remain empty with NULL array");

    d_DestroyString(sb);
    return 1;
}

// =============================================================================
// d_SliceString TESTS
// =============================================================================

int test_slice_basic(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello World", 6, 11);
    log_string_content("Basic slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "World") == 0, "Basic slice failed");
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Sliced string should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_beginning(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello World", 0, 5);
    log_string_content("Beginning slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hello") == 0, "Beginning slice failed");
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Sliced string should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_middle(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello World", 2, 9);
    log_string_content("Middle slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "llo Wor") == 0, "Middle slice failed");
    TEST_ASSERT(d_GetStringLength(sb) == 7, "Sliced string should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_negative_indices(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello", -3, -1);
    log_string_content("Negative indices slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "llo") == 0, "Negative indices slice failed");
    TEST_ASSERT(d_GetStringLength(sb) == 3, "Sliced string should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_negative_to_end(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello World", -5, 11);
    log_string_content("Negative start to end slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "World") == 0, "Negative start to end slice failed");
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Sliced string should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_whole_string(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello", 0, 5);
    log_string_content("Whole string slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hello") == 0, "Whole string slice failed");
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Sliced string should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_empty_result(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello", 3, 3);  // start == end
    log_string_content("Empty slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "Empty slice failed");
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Empty slice should have zero length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_reverse_indices(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello", 4, 2);  // start > end
    log_string_content("Reverse indices slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "Reverse indices should produce empty string");
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Reverse indices should have zero length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_out_of_bounds(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello", 10, 20);  // start beyond string
    log_string_content("Out of bounds slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "Out of bounds slice should be empty");
    TEST_ASSERT(d_GetStringLength(sb) == 0, "Out of bounds slice should have zero length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_negative_extreme(void)
{
    dString_t* sb = create_test_string(NULL);

    d_SliceString(sb, "Hello", -100, -1);
    log_string_content("Extreme negative slice result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Hello") == 0, "Extreme negative start should be clamped");
    TEST_ASSERT(d_GetStringLength(sb) == 5, "Clamped slice should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_append_to_existing(void)
{
    dString_t* sb = create_test_string("Start: ");

    d_SliceString(sb, "Hello World", 6, 11);
    log_string_content("Slice append result", sb);

    TEST_ASSERT(sb != NULL, "String builder should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(sb), "Start: World") == 0, "Slice append failed");
    TEST_ASSERT(d_GetStringLength(sb) == 12, "Appended slice should have correct length");

    d_DestroyString(sb);
    return 1;
}

int test_slice_null_safety(void)
{
    // Test with NULL string builder - should not crash
    d_SliceString(NULL, "test", 0, 4);
    TEST_ASSERT(1, "NULL string builder should not crash");

    dString_t* sb = create_test_string(NULL);
    d_SliceString(sb, NULL, 0, 4);
    log_string_content("Slice with NULL text result", sb);

    TEST_ASSERT(d_GetStringLength(sb) == 0, "NULL text should not modify string");
    TEST_ASSERT(strcmp(d_PeekString(sb), "") == 0, "String should remain empty with NULL text");

    d_DestroyString(sb);
    return 1;
}

// =============================================================================
// RPG INTEGRATION TESTS
// =============================================================================

int test_rpg_inventory_management(void)
{
    const char* inventory[] = {"Iron Sword", "Health Potion", "Magic Ring", "Gold Coins"};
    dString_t* list = create_test_string("Inventory: ");

    d_JoinStrings(list, inventory, 4, ", ");
    log_string_content("Inventory list", list);

    const char* result = d_PeekString(list);
    TEST_ASSERT(result != NULL, "Inventory result should not be NULL");
    TEST_ASSERT(strstr(result, "Iron Sword, Health Potion, Magic Ring, Gold Coins") != NULL,
                "Inventory list creation failed");
    TEST_ASSERT(strstr(result, "Inventory:") != NULL, "Should contain inventory prefix");

    d_DestroyString(list);
    return 1;
}

int test_rpg_command_parsing(void)
{
    dArray_t* parts = d_SplitString("attack goblin with sword", " ");

    TEST_ASSERT(parts != NULL, "Command parts should not be NULL");
    TEST_ASSERT(parts->count == 4, "Command should have 4 parts");

    dString_t** action_ptr = (dString_t**)d_IndexDataFromArray(parts, 0);
    dString_t** target_ptr = (dString_t**)d_IndexDataFromArray(parts, 1);
    dString_t** prep_ptr = (dString_t**)d_IndexDataFromArray(parts, 2);
    dString_t** weapon_ptr = (dString_t**)d_IndexDataFromArray(parts, 3);

    TEST_ASSERT(action_ptr != NULL && *action_ptr != NULL, "Action element should exist");
    TEST_ASSERT(target_ptr != NULL && *target_ptr != NULL, "Target element should exist");
    TEST_ASSERT(prep_ptr != NULL && *prep_ptr != NULL, "Preposition element should exist");
    TEST_ASSERT(weapon_ptr != NULL && *weapon_ptr != NULL, "Weapon element should exist");

    TEST_ASSERT(strcmp(d_PeekString(*action_ptr), "attack") == 0, "Action incorrect");
    TEST_ASSERT(strcmp(d_PeekString(*target_ptr), "goblin") == 0, "Target incorrect");
    TEST_ASSERT(strcmp(d_PeekString(*prep_ptr), "with") == 0, "Preposition incorrect");
    TEST_ASSERT(strcmp(d_PeekString(*weapon_ptr), "sword") == 0, "Weapon incorrect");

    d_FreeSplitStringArray(parts);
    return 1;
}

int test_rpg_name_generation(void)
{
    dString_t* name = create_test_string(NULL);

    // Create a character name by slicing and joining parts
    d_SliceString(name, "Alexander", 0, 4);  // "Alex"
    d_AppendString(name, " ", 0);
    d_SliceString(name, "Thunderstrike", 0, 7);  // "Thunder"
    d_AppendString(name, "born", 0);

    log_string_content("Generated name", name);

    TEST_ASSERT(name != NULL, "Name string should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(name), "Alex Thunderborn") == 0, "Name generation failed");
    TEST_ASSERT(strstr(d_PeekString(name), "Alex") != NULL, "Should contain first name");
    TEST_ASSERT(strstr(d_PeekString(name), "Thunder") != NULL, "Should contain thunder element");

    d_DestroyString(name);
    return 1;
}

int test_rpg_dialogue_word_wrapping(void)
{
    const char* long_dialogue = "The ancient wizard speaks: 'Young adventurer, your quest leads to dangerous lands beyond the misty mountains.'";

    // Split into words for wrapping
    dArray_t* words = d_SplitString(long_dialogue, " ");

    dString_t* log_message = d_InitString();
    for (int i = 0; i < words->count; i++) {
        dString_t** word_ptr = (dString_t**)d_IndexDataFromArray(words, i);
        dString_t* word = *word_ptr;
        d_AppendString(log_message, word->str, 0);
        d_AppendChar(log_message, '\t');
    }
    LOG(log_message->str);
    d_DestroyString(log_message);

    TEST_ASSERT(words != NULL, "Words array should not be NULL");
    TEST_ASSERT(words->count > 10, "Should have many words");

    dString_t** first_word_ptr = (dString_t**)d_IndexDataFromArray(words, 0);
    dString_t** last_word_ptr = (dString_t**)d_IndexDataFromArray(words, words->count - 1);

    TEST_ASSERT(first_word_ptr != NULL && *first_word_ptr != NULL, "First word should exist");
    TEST_ASSERT(last_word_ptr != NULL && *last_word_ptr != NULL, "Last word should exist");
    TEST_ASSERT(strcmp((*first_word_ptr)->str, "The") == 0, "First word should be 'The'");
    TEST_ASSERT(strcmp(d_PeekString(*last_word_ptr), "mountains.'") == 0, "Last word should be 'mountains.'");

    // Build array of first 5 words for joining
    const char* first_five[5];
    for (int i = 0; i < 5; i++) {
        dString_t** word_ptr = (dString_t**)d_IndexDataFromArray(words, i);
        first_five[i] = d_PeekString(*word_ptr);
    }

    dString_t* short_line = create_test_string(NULL);
    d_JoinStrings(short_line, first_five, 5, " ");

    log_string_content("Short line", short_line);

    TEST_ASSERT(strcmp(d_PeekString(short_line), "The ancient wizard speaks: 'Young") == 0,
                "Word wrapping should be successful");

    d_FreeSplitStringArray(words);
    d_DestroyString(short_line);
    return 1;
}

int test_rpg_file_path_handling(void)
{
    const char* path_parts[] = {"saves", "characters", "warrior_level_42.dat"};

    dString_t* path = create_test_string(NULL);
    d_JoinStrings(path, path_parts, 3, "/");

    log_string_content("File path", path);

    TEST_ASSERT(path != NULL, "Path string should not be NULL");
    TEST_ASSERT(strcmp(d_PeekString(path), "saves/characters/warrior_level_42.dat") == 0,
                "Path building failed");
    TEST_ASSERT(strstr(d_PeekString(path), "saves/") != NULL, "Should contain saves directory");
    TEST_ASSERT(strstr(d_PeekString(path), ".dat") != NULL, "Should have proper file extension");

    d_DestroyString(path);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Python-inspired String Functions Tests");

    // d_JoinStrings tests
    RUN_TEST(test_join_basic);
    RUN_TEST(test_join_single_item);
    RUN_TEST(test_join_empty_separator);
    RUN_TEST(test_join_null_separator);
    RUN_TEST(test_join_with_nulls);
    RUN_TEST(test_join_path_separator);
    RUN_TEST(test_join_null_safety);

    // d_SliceString tests
    RUN_TEST(test_slice_basic);
    RUN_TEST(test_slice_beginning);
    RUN_TEST(test_slice_middle);
    RUN_TEST(test_slice_negative_indices);
    RUN_TEST(test_slice_negative_to_end);
    RUN_TEST(test_slice_whole_string);
    RUN_TEST(test_slice_empty_result);
    RUN_TEST(test_slice_reverse_indices);
    RUN_TEST(test_slice_out_of_bounds);
    RUN_TEST(test_slice_negative_extreme);
    RUN_TEST(test_slice_append_to_existing);
    RUN_TEST(test_slice_null_safety);

    // RPG Integration tests
    RUN_TEST(test_rpg_inventory_management);
    RUN_TEST(test_rpg_command_parsing);
    RUN_TEST(test_rpg_name_generation);
    RUN_TEST(test_rpg_dialogue_word_wrapping);
    RUN_TEST(test_rpg_file_path_handling);

    TEST_SUITE_END();
}
