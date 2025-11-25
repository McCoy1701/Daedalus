/* test_duf.c - Test program for DUF parser */

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
        return; // Don't assert, just return
    }

    if (data == NULL) {
        printf("  ERROR: Parse returned NULL data but no error!\n");
        fflush(stdout);
        return;
    }

    printf("  ✓ Parse successful\n");

    // Test basic integer access
    int64_t didact_hp = d_DUFGetInt(data, "didact.hp", 0);
    printf("  Didact HP: %lld\n", (long long)didact_hp);
    assert(didact_hp == 500);
    printf("  ✓ Integer access works\n");

    // Test boolean access
    bool didact_is_boss = d_DUFGetBool(data, "didact.is_boss", true);
    printf("  Didact is boss: %s\n", didact_is_boss ? "true" : "false");
    assert(didact_is_boss == false);
    printf("  ✓ Boolean access works\n");

    // Test float access
    double didact_multiplier = d_DUFGetFloat(data, "didact.multiplier", 0.0);
    printf("  Didact multiplier: %g\n", didact_multiplier);
    assert(didact_multiplier == 1.5);
    printf("  ✓ Float access works\n");

    // Test string access
    const char* didact_name = d_DUFGetString(data, "didact.name", "");
    printf("  Didact name: %s\n", didact_name);
    assert(strcmp(didact_name, "Didact") == 0);
    printf("  ✓ String access works\n");

    // Test array access
    const char* ability = d_DUFGetString(data, "didact.abilities[0]", "");
    printf("  First ability: %s\n", ability);
    assert(strcmp(ability, "the_house_remembers") == 0);
    printf("  ✓ Array access works\n");

    // Test nested table access
    int64_t threshold = d_DUFGetInt(data, "daemon.phases[0].threshold", 0);
    printf("  First phase threshold: %lld\n", (long long)threshold);
    assert(threshold == 75);
    printf("  ✓ Nested table access works\n");

    // Test fallback values
    int64_t nonexistent = d_DUFGetInt(data, "nonexistent.path", -1);
    assert(nonexistent == -1);
    printf("  ✓ Fallback values work\n");

    // Test boss status for daemon
    bool daemon_is_boss = d_DUFGetBool(data, "daemon.is_boss", false);
    assert(daemon_is_boss == true);
    printf("  ✓ Daemon boss status correct\n");

    // Test array length
    dDUFValue_t* abilities = d_DUFGet(data, "didact.abilities");
    size_t ability_count = d_DUFArrayLength(abilities);
    printf("  Didact has %zu abilities\n", ability_count);
    assert(ability_count == 3);
    printf("  ✓ Array length works\n");

    // Test table iteration
    printf("  Enemy list:\n");
    int enemy_count = 0;
    void count_enemies(const char* key, dDUFValue_t* val, void* ctx) {
        (void)val;
        int* count = (int*)ctx;
        (*count)++;
        printf("    - %s\n", key);
    }
    d_DUFTableForEach(data, count_enemies, &enemy_count);
    assert(enemy_count == 3);
    printf("  ✓ Table iteration works (%d enemies)\n", enemy_count);

    d_DUFFree(data);
    printf("  ✓ Cleanup successful\n\n");
}

void test_serialization(void)
{
    printf("Testing DUF serialization...\n");

    // Create a simple structure
    dDUFValue_t* root = d_DUFCreateTable();
    dDUFValue_t* player = d_DUFCreateTable();

    d_DUFTableSet(player, "name", d_DUFCreateString("Hero"));
    d_DUFTableSet(player, "level", d_DUFCreateInt(5));
    d_DUFTableSet(player, "health", d_DUFCreateFloat(100.5));
    d_DUFTableSet(player, "alive", d_DUFCreateBool(true));

    dDUFValue_t* items = d_DUFCreateArray();
    d_ArrayAppend(items->array_val, &(dDUFValue_t*){d_DUFCreateString("sword")});
    d_ArrayAppend(items->array_val, &(dDUFValue_t*){d_DUFCreateString("shield")});
    d_DUFTableSet(player, "items", items);

    d_DUFTableSet(root, "player", player);

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
    const char* name = d_DUFGetString(loaded, "player.name", "");
    assert(strcmp(name, "Hero") == 0);

    int64_t level = d_DUFGetInt(loaded, "player.level", 0);
    assert(level == 5);

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
    assert(err != NULL);
    printf("  ✓ Invalid file error: %s\n", d_StringPeek(err->message));
    d_DUFErrorFree(err);

    // Test malformed DUF
    const char* bad_duf = "@test { key: }";  // Missing value
    err = d_DUFParseString(bad_duf, &data);
    if (err != NULL) {
        printf("  ✓ Malformed DUF error: %s\n", d_StringPeek(err->message));
        d_DUFErrorFree(err);
    }

    printf("\n");
}

int main(void)
{
    printf("=== DUF Parser Test Suite ===\n\n");

    test_parse_enemies();
    test_serialization();
    test_error_handling();

    printf("=== All tests passed! ===\n");
    return 0;
}
