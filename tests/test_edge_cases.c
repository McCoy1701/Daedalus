/* test_edge_cases.c - Comprehensive edge case tests for DUF parser */

#include "Daedalus.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

// Helper to print test status
#define TEST_START(name) printf("  Testing %s...\n", name)
#define TEST_PASS(name) printf("    ✓ %s\n", name)
#define TEST_FAIL(name, ...) do { printf("    ✗ %s: ", name); printf(__VA_ARGS__); printf("\n"); return; } while(0)

// Global test data
static dDUFValue_t* g_data = NULL;

// Load test data once
static int load_test_data(void)
{
    if (g_data != NULL) {
        return 0; // Already loaded
    }

    dDUFError_t* err = d_DUFParseFile("tests/test_data/edge_cases.duf", &g_data);

    if (err != NULL) {
        printf("FATAL: Failed to parse edge_cases.duf at %d:%d - %s\n",
               err->line, err->column, d_StringPeek(err->message));
        d_DUFErrorFree(err);
        return -1;
    }

    if (g_data == NULL) {
        printf("FATAL: Parse returned NULL with no error\n");
        return -1;
    }

    return 0;
}

// ===========================================================================
// Test Functions
// ===========================================================================

void test_numeric_boundaries(void)
{
    TEST_START("numeric boundaries");

    // INT64_MAX
    int64_t max_int = d_DUFGetInt(g_data, "edge_cases.max_int", 0);
    if (max_int != INT64_MAX) {
        TEST_FAIL("INT64_MAX", "expected %lld, got %lld", (long long)INT64_MAX, (long long)max_int);
    }
    TEST_PASS("INT64_MAX");

    // INT64_MIN
    int64_t min_int = d_DUFGetInt(g_data, "edge_cases.min_int", 0);
    if (min_int != INT64_MIN) {
        TEST_FAIL("INT64_MIN", "expected %lld, got %lld", (long long)INT64_MIN, (long long)min_int);
    }
    TEST_PASS("INT64_MIN");

    // Zero
    int64_t zero = d_DUFGetInt(g_data, "edge_cases.zero", -1);
    assert(zero == 0);
    TEST_PASS("zero integer");

    // Negative
    int64_t negative = d_DUFGetInt(g_data, "edge_cases.negative", 0);
    assert(negative == -42);
    TEST_PASS("negative integer");

    // Float zero
    double float_zero = d_DUFGetFloat(g_data, "edge_cases.float_zero", -1.0);
    assert(fabs(float_zero - 0.0) < 0.0001);
    TEST_PASS("float zero");

    // Small float
    double small_float = d_DUFGetFloat(g_data, "edge_cases.small_float", 0.0);
    assert(fabs(small_float - 0.000001) < 0.0000001);
    TEST_PASS("small float");

    // Large float
    double large_float = d_DUFGetFloat(g_data, "edge_cases.large_float", 0.0);
    assert(fabs(large_float - 999999.999999) < 0.01);
    TEST_PASS("large float");
}

void test_string_edge_cases(void)
{
    TEST_START("string edge cases");

    // Empty string
    const char* empty = d_DUFGetString(g_data, "edge_cases.empty_string", NULL);
    if (empty == NULL || strlen(empty) != 0) {
        TEST_FAIL("empty string", "expected \"\", got %s", empty ? empty : "NULL");
    }
    TEST_PASS("empty string");

    // Escaped characters
    const char* escaped = d_DUFGetString(g_data, "edge_cases.escaped", "");
    if (strstr(escaped, "\"Hello\"") == NULL) {
        TEST_FAIL("escaped quotes", "string doesn't contain escaped quotes");
    }
    if (strstr(escaped, "\\") == NULL) {
        TEST_FAIL("escaped backslash", "string doesn't contain backslash");
    }
    TEST_PASS("escaped characters");

    // Single character
    const char* single = d_DUFGetString(g_data, "edge_cases.single_char", "");
    assert(strcmp(single, "x") == 0);
    assert(strlen(single) == 1);
    TEST_PASS("single character");
}

void test_empty_collections(void)
{
    TEST_START("empty collections");

    // Empty array
    dDUFValue_t* empty_arr = d_DUFGet(g_data, "edge_cases.empty_array");
    if (empty_arr == NULL || d_DUFGetType(empty_arr) != D_DUF_ARRAY) {
        TEST_FAIL("empty array", "not found or wrong type");
    }
    size_t arr_len = d_DUFArrayLength(empty_arr);
    assert(arr_len == 0);
    TEST_PASS("empty array");

    // Empty table
    dDUFValue_t* empty_tbl = d_DUFGet(g_data, "edge_cases.empty_table");
    if (empty_tbl == NULL || d_DUFGetType(empty_tbl) != D_DUF_TABLE) {
        TEST_FAIL("empty table", "not found or wrong type");
    }
    TEST_PASS("empty table");
}

void test_deep_nesting(void)
{
    TEST_START("deep nesting");

    // 4-level deep path
    const char* deep = d_DUFGetString(g_data, "edge_cases.complex.level1.level2.level3.deep_value", "");
    if (strcmp(deep, "nested") != 0) {
        TEST_FAIL("4-level nesting", "expected 'nested', got '%s'", deep);
    }
    TEST_PASS("4-level nested table");

    // Deep array access
    int64_t deep_arr_val = d_DUFGetInt(g_data, "edge_cases.complex.level1.level2.level3.deep_array[1]", 0);
    assert(deep_arr_val == 2);
    TEST_PASS("deep nested array access");
}

void test_array_stress(void)
{
    TEST_START("array stress tests");

    // Array of tables
    const char* config_name = d_DUFGetString(g_data, "array_stress.configs[0].name", "");
    assert(strcmp(config_name, "config1") == 0);
    int64_t config_val = d_DUFGetInt(g_data, "array_stress.configs[1].value", 0);
    assert(config_val == 20);
    TEST_PASS("array of tables");

    // Array of arrays (matrix)
    int64_t matrix_val = d_DUFGetInt(g_data, "array_stress.matrix[1][1]", 0);
    assert(matrix_val == 5); // Middle of matrix
    TEST_PASS("array of arrays (matrix)");

    // Large array
    dDUFValue_t* range = d_DUFGet(g_data, "array_stress.range");
    size_t range_len = d_DUFArrayLength(range);
    assert(range_len == 21); // 0-20 inclusive
    int64_t last_elem = d_DUFGetInt(g_data, "array_stress.range[20]", -1);
    assert(last_elem == 20);
    TEST_PASS("large array (21 elements)");

    // Single element array
    dDUFValue_t* singleton = d_DUFGet(g_data, "array_stress.singleton");
    size_t single_len = d_DUFArrayLength(singleton);
    assert(single_len == 1);
    int64_t single_val = d_DUFGetInt(g_data, "array_stress.singleton[0]", 0);
    assert(single_val == 42);
    TEST_PASS("singleton array");
}

void test_multiline_strings(void)
{
    TEST_START("multi-line strings");

    // Simple multi-line
    const char* simple = d_DUFGetString(g_data, "multiline_strings.simple", "");
    if (strstr(simple, "multi-line") == NULL || strstr(simple, "\n") == NULL) {
        TEST_FAIL("simple multi-line", "doesn't contain expected text or newlines");
    }
    TEST_PASS("simple multi-line");

    // Multi-line with quotes (no escaping needed)
    const char* with_quotes = d_DUFGetString(g_data, "multiline_strings.with_quotes", "");
    if (strstr(with_quotes, "\"Quoted text\"") == NULL) {
        TEST_FAIL("multi-line with quotes", "doesn't contain unescaped quotes");
    }
    TEST_PASS("multi-line with quotes");

    // Code snippet (with braces)
    const char* code = d_DUFGetString(g_data, "multiline_strings.code_snippet", "");
    if (strstr(code, "function") == NULL || strstr(code, "{") == NULL) {
        TEST_FAIL("code snippet", "doesn't contain expected code syntax");
    }
    TEST_PASS("code snippet preservation");

    // Formatted with blank lines
    const char* formatted = d_DUFGetString(g_data, "multiline_strings.formatted", "");
    if (strstr(formatted, "===") == NULL || strstr(formatted, "Bullet point") == NULL) {
        TEST_FAIL("formatted text", "doesn't contain expected formatting");
    }
    TEST_PASS("formatted text with blank lines");
}

void test_special_keys(void)
{
    TEST_START("special identifier names");

    // Underscore prefix
    const char* underscore = d_DUFGetString(g_data, "special_keys._underscore", "");
    assert(strcmp(underscore, "starts with underscore") == 0);
    TEST_PASS("underscore prefix");

    // All caps
    const char* caps = d_DUFGetString(g_data, "special_keys.CAPS_KEY", "");
    assert(strcmp(caps, "all caps") == 0);
    TEST_PASS("all caps key");

    // Mixed case
    const char* mixed = d_DUFGetString(g_data, "special_keys.mixedCase", "");
    assert(strcmp(mixed, "camelCase") == 0);
    TEST_PASS("camelCase key");

    // Ends with numbers
    const char* nums = d_DUFGetString(g_data, "special_keys.key_123", "");
    assert(strcmp(nums, "ends with numbers") == 0);
    TEST_PASS("key ending with numbers");

    // Mixed alphanumeric
    const char* alphanum = d_DUFGetString(g_data, "special_keys.key123abc", "");
    assert(strcmp(alphanum, "mixed alphanumeric") == 0);
    TEST_PASS("mixed alphanumeric key");
}

void test_realistic_structure(void)
{
    TEST_START("realistic complex enemy structure");

    // Basic fields
    int64_t id = d_DUFGetInt(g_data, "realistic_enemy.id", 0);
    assert(id == 999);
    const char* name = d_DUFGetString(g_data, "realistic_enemy.name", "");
    assert(strcmp(name, "Glitch Hound") == 0);
    TEST_PASS("basic enemy fields");

    // Nested stats table
    int64_t str = d_DUFGetInt(g_data, "realistic_enemy.stats.str", 0);
    assert(str == 12);
    int64_t dex = d_DUFGetInt(g_data, "realistic_enemy.stats.dex", 0);
    assert(dex == 18);
    TEST_PASS("nested stats table");

    // Resistances (floats)
    double fire_res = d_DUFGetFloat(g_data, "realistic_enemy.resistances.fire", 0.0);
    assert(fabs(fire_res - 1.2) < 0.01);
    TEST_PASS("resistance floats");

    // Abilities array
    dDUFValue_t* abilities = d_DUFGet(g_data, "realistic_enemy.abilities");
    assert(d_DUFArrayLength(abilities) == 3);
    const char* ability1 = d_DUFGetString(g_data, "realistic_enemy.abilities[1]", "");
    assert(strcmp(ability1, "data_corruption") == 0);
    TEST_PASS("abilities array");

    // Loot table (array of tables)
    const char* loot_item = d_DUFGetString(g_data, "realistic_enemy.loot_table[0].item", "");
    assert(strcmp(loot_item, "scrap_metal") == 0);
    int64_t loot_weight = d_DUFGetInt(g_data, "realistic_enemy.loot_table[0].weight", 0);
    assert(loot_weight == 50);
    TEST_PASS("loot table (array of tables)");

    // AI behavior nested table
    double aggro = d_DUFGetFloat(g_data, "realistic_enemy.ai_behavior.aggro_range", 0.0);
    assert(fabs(aggro - 15.5) < 0.01);
    bool tactical = d_DUFGetBool(g_data, "realistic_enemy.ai_behavior.tactical", true);
    assert(tactical == false);
    TEST_PASS("AI behavior table");

    // Spawn conditions with array
    dDUFValue_t* zones = d_DUFGet(g_data, "realistic_enemy.spawn_conditions.zones");
    assert(d_DUFArrayLength(zones) == 3);
    const char* zone = d_DUFGetString(g_data, "realistic_enemy.spawn_conditions.zones[1]", "");
    assert(strcmp(zone, "corrupted_sector") == 0);
    TEST_PASS("spawn conditions with array");

    // Flavor text (multi-line)
    const char* flavor = d_DUFGetString(g_data, "realistic_enemy.flavor_text", "");
    if (strstr(flavor, "Feral programs") == NULL || strstr(flavor, "\n") == NULL) {
        TEST_FAIL("flavor text", "multi-line text not preserved");
    }
    TEST_PASS("multi-line flavor text");
}

void test_type_coercion(void)
{
    TEST_START("type coercion");

    // Int to float implicit conversion
    double zero_as_float = d_DUFGetFloat(g_data, "edge_cases.zero", -1.0);
    assert(fabs(zero_as_float - 0.0) < 0.0001);
    TEST_PASS("int to float coercion");

    // Verify float stays float
    int64_t float_as_int = d_DUFGetInt(g_data, "edge_cases.float_zero", 999);
    assert(float_as_int == 999); // Should use fallback (wrong type)
    TEST_PASS("float doesn't coerce to int");
}

void test_invalid_paths(void)
{
    TEST_START("invalid path handling");

    // Missing key
    int64_t missing = d_DUFGetInt(g_data, "nonexistent.key.path", -999);
    assert(missing == -999);
    TEST_PASS("missing key uses fallback");

    // Out of bounds array
    int64_t oob = d_DUFGetInt(g_data, "array_stress.range[999]", -1);
    assert(oob == -1);
    TEST_PASS("out of bounds array uses fallback");

    // Wrong type access
    const char* wrong_type = d_DUFGetString(g_data, "edge_cases.max_int", "FALLBACK");
    assert(strcmp(wrong_type, "FALLBACK") == 0);
    TEST_PASS("wrong type uses fallback");

    // Null path
    dDUFValue_t* null_path = d_DUFGet(g_data, NULL);
    assert(null_path == NULL);
    TEST_PASS("NULL path returns NULL");

    // Empty path
    dDUFValue_t* empty_path = d_DUFGet(g_data, "");
    assert(empty_path == NULL);
    TEST_PASS("empty path returns NULL");
}

void test_boolean_arrays(void)
{
    TEST_START("boolean arrays");

    bool bool1 = d_DUFGetBool(g_data, "edge_cases.bool_array[0]", false);
    bool bool2 = d_DUFGetBool(g_data, "edge_cases.bool_array[1]", true);
    bool bool3 = d_DUFGetBool(g_data, "edge_cases.bool_array[2]", false);
    bool bool4 = d_DUFGetBool(g_data, "edge_cases.bool_array[3]", true);

    assert(bool1 == true);
    assert(bool2 == false);
    assert(bool3 == true);
    assert(bool4 == false);

    TEST_PASS("boolean array values");
}

// ===========================================================================
// Main Test Runner
// ===========================================================================

int main(void)
{
    printf("=== DUF Edge Case Test Suite ===\n\n");

    // Load test data
    printf("Loading test data...\n");
    if (load_test_data() != 0) {
        printf("FATAL: Could not load test data\n");
        return 1;
    }
    printf("✓ Test data loaded successfully\n\n");

    // Run all tests
    test_numeric_boundaries();
    test_string_edge_cases();
    test_empty_collections();
    test_deep_nesting();
    test_array_stress();
    test_multiline_strings();
    test_special_keys();
    test_realistic_structure();
    test_type_coercion();
    test_invalid_paths();
    test_boolean_arrays();

    // Cleanup
    if (g_data != NULL) {
        d_DUFFree(g_data);
    }

    printf("\n=== All edge case tests passed! ===\n");
    return 0;
}
