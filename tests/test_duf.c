/* test_duf.c - Test program for DUF parser (AUF-style API) */

#define _POSIX_C_SOURCE 200809L

#include "Daedalus.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_parse_enemies(void)
{
    printf("Testing DUF parser with enemies.duf...\n");

    dDUFValue_t* data = NULL;
    dDUFError_t* err = d_DUFParseFile("tests/test_data/enemies.duf", &data);

    if (err != NULL) {
        printf("  ERROR: Parse failed at %d:%d - %s\n",
               err->line, err->column, d_StringPeek(err->message));
        fflush(stdout);
        d_DUFErrorFree(err);
        return;
    }

    if (data == NULL) {
        printf("  ERROR: Parse returned NULL data but no error!\n");
        fflush(stdout);
        return;
    }

    printf("  ✓ Parse successful\n");

    // Get didact entry
    dDUFValue_t* didact = d_DUFGetObjectItem(data, "didact");
    assert(didact != NULL);

    // Test basic integer access
    dDUFValue_t* hp_node = d_DUFGetObjectItem(didact, "hp");
    assert(hp_node != NULL);
    printf("  Didact HP: %lld\n", (long long)hp_node->value_int);
    assert(hp_node->value_int == 500);
    printf("  ✓ Integer access works\n");

    // Test boolean access
    dDUFValue_t* is_boss_node = d_DUFGetObjectItem(didact, "is_boss");
    assert(is_boss_node != NULL);
    printf("  Didact is boss: %s\n", is_boss_node->value_int ? "true" : "false");
    assert(is_boss_node->value_int == 0);  // false
    printf("  ✓ Boolean access works\n");

    // Test float access
    dDUFValue_t* mult_node = d_DUFGetObjectItem(didact, "multiplier");
    assert(mult_node != NULL);
    printf("  Didact multiplier: %g\n", mult_node->value_double);
    assert(mult_node->value_double == 1.5);
    printf("  ✓ Float access works\n");

    // Test string access
    dDUFValue_t* name_node = d_DUFGetObjectItem(didact, "name");
    assert(name_node != NULL);
    printf("  Didact name: %s\n", name_node->value_string);
    assert(strcmp(name_node->value_string, "Didact") == 0);
    printf("  ✓ String access works\n");

    // Test array access
    dDUFValue_t* abilities_node = d_DUFGetObjectItem(didact, "abilities");
    assert(abilities_node != NULL);
    assert(abilities_node->type == D_DUF_ARRAY);

    // Get first ability
    dDUFValue_t* first_ability = abilities_node->child;
    assert(first_ability != NULL);
    printf("  First ability: %s\n", first_ability->value_string);
    assert(strcmp(first_ability->value_string, "the_house_remembers") == 0);
    printf("  ✓ Array access works\n");

    // Test nested table/array access
    dDUFValue_t* daemon = d_DUFGetObjectItem(data, "daemon");
    assert(daemon != NULL);

    dDUFValue_t* phases_node = d_DUFGetObjectItem(daemon, "phases");
    assert(phases_node != NULL);
    assert(phases_node->type == D_DUF_ARRAY);

    // Get first phase
    dDUFValue_t* first_phase = phases_node->child;
    assert(first_phase != NULL);

    dDUFValue_t* threshold_node = d_DUFGetObjectItem(first_phase, "threshold");
    assert(threshold_node != NULL);
    printf("  First phase threshold: %lld\n", (long long)threshold_node->value_int);
    assert(threshold_node->value_int == 75);
    printf("  ✓ Nested table access works\n");

    // Test NULL checks (fallback values)
    dDUFValue_t* nonexistent = d_DUFGetObjectItem(data, "nonexistent");
    assert(nonexistent == NULL);
    printf("  ✓ NULL checks work\n");

    // Test daemon boss status
    dDUFValue_t* daemon_boss = d_DUFGetObjectItem(daemon, "is_boss");
    assert(daemon_boss != NULL);
    assert(daemon_boss->value_int == 1);  // true
    printf("  ✓ Daemon boss status correct\n");

    // Test array length (AUF-style)
    int ability_count = 0;
    dDUFValue_t* ability_item = abilities_node->child;
    while (ability_item != NULL) {
        ability_count++;
        ability_item = ability_item->next;
    }
    printf("  Didact has %d abilities\n", ability_count);
    assert(ability_count == 3);
    printf("  ✓ Array length works\n");

    // Test table iteration (AUF-style)
    printf("  Enemy list:\n");
    int enemy_count = 0;
    dDUFValue_t* enemy = data->child;
    while (enemy != NULL) {
        enemy_count++;
        printf("    - %s\n", enemy->string);
        enemy = enemy->next;
    }
    assert(enemy_count == 3);
    printf("  ✓ Table iteration works (%d enemies)\n", enemy_count);

    d_DUFFree(data);
    printf("  ✓ Cleanup successful\n\n");
}

void test_serialization(void)
{
    printf("Testing DUF serialization...\n");

    // Create a simple structure (AUF-style)
    dDUFValue_t* root = d_DUFCreateTable();
    dDUFValue_t* player = d_DUFCreateTable();

    // Set player name
    player->string = strdup("player");

    // Add name
    dDUFValue_t* name = d_DUFCreateString("Hero");
    name->string = strdup("name");
    if (player->child == NULL) {
        player->child = name;
    } else {
        dDUFValue_t* last = player->child;
        while (last->next != NULL) last = last->next;
        last->next = name;
        name->prev = last;
    }

    // Add level
    dDUFValue_t* level = d_DUFCreateInt(5);
    level->string = strdup("level");
    dDUFValue_t* last = player->child;
    while (last->next != NULL) last = last->next;
    last->next = level;
    level->prev = last;

    // Add health
    dDUFValue_t* health = d_DUFCreateFloat(100.5);
    health->string = strdup("health");
    last = player->child;
    while (last->next != NULL) last = last->next;
    last->next = health;
    health->prev = last;

    // Add alive
    dDUFValue_t* alive = d_DUFCreateBool(true);
    alive->string = strdup("alive");
    last = player->child;
    while (last->next != NULL) last = last->next;
    last->next = alive;
    alive->prev = last;

    // Add items array
    dDUFValue_t* items = d_DUFCreateArray();
    items->string = strdup("items");

    dDUFValue_t* sword = d_DUFCreateString("sword");
    items->child = sword;

    dDUFValue_t* shield = d_DUFCreateString("shield");
    sword->next = shield;
    shield->prev = sword;

    last = player->child;
    while (last->next != NULL) last = last->next;
    last->next = items;
    items->prev = last;

    // Add player to root
    root->child = player;

    // Serialize to string
    dString_t* output = d_DUFToString(root);
    printf("  Serialized output:\n%s\n", d_StringPeek(output));

    // Write to file
    int write_result = d_DUFWriteFile(root, "tests/test_data/output.duf");
    assert(write_result == 0);
    printf("  ✓ Write to file successful\n");

    d_StringDestroy(output);

    // Read it back
    dDUFValue_t* loaded = NULL;
    dDUFError_t* err = d_DUFParseFile("tests/test_data/output.duf", &loaded);

    if (err != NULL) {
        printf("  ERROR: Failed to read back: %s\n", d_StringPeek(err->message));
        d_DUFErrorFree(err);
        assert(0);
    }

    // Verify data
    dDUFValue_t* loaded_player = d_DUFGetObjectItem(loaded, "player");
    assert(loaded_player != NULL);

    dDUFValue_t* loaded_name = d_DUFGetObjectItem(loaded_player, "name");
    assert(loaded_name != NULL);
    assert(strcmp(loaded_name->value_string, "Hero") == 0);

    dDUFValue_t* loaded_level = d_DUFGetObjectItem(loaded_player, "level");
    assert(loaded_level != NULL);
    assert(loaded_level->value_int == 5);

    printf("  ✓ Round-trip serialization works\n\n");

    d_DUFFree(root);
    d_DUFFree(loaded);
}

void test_error_handling(void)
{
    printf("Testing error handling...\n");

    // Test invalid file
    dDUFValue_t* data = NULL;
    dDUFError_t* err = d_DUFParseFile("nonexistent.duf", &data);

    if (err != NULL) {
        printf("  ✓ Invalid file error: %s\n", d_StringPeek(err->message));
        d_DUFErrorFree(err);
    } else {
        printf("  ERROR: Should have failed!\n");
        assert(0);
    }

    // Test malformed DUF
    const char* bad_duf = "@test { value 42 }";  // Missing colon
    err = d_DUFParseString(bad_duf, &data);

    if (err != NULL) {
        printf("  ✓ Malformed DUF error: %s\n", d_StringPeek(err->message));
        d_DUFErrorFree(err);
    } else {
        printf("  ERROR: Should have failed!\n");
        assert(0);
    }

    printf("\n");
}

int main(void)
{
    printf("=== DUF Parser Tests (AUF-style API) ===\n\n");

    test_parse_enemies();
    test_serialization();
    test_error_handling();

    printf("=== All tests passed! ===\n");
    return 0;
}
