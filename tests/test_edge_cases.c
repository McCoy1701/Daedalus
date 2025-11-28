/* test_edge_cases.c - Comprehensive edge case tests for DUF parser */

#define _POSIX_C_SOURCE 200809L

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

// Helper function to navigate nested paths and array indices
// Example: "edge_cases.complex.level1[2]" would navigate: edge_cases -> complex -> level1 -> child[2]
static dDUFValue_t* navigate_path(dDUFValue_t* root, const char* path)
{
    if (root == NULL || path == NULL || path[0] == '\0') {
        return NULL;
    }

    dDUFValue_t* current = root;
    char buffer[256];
    const char* p = path;

    while (*p != '\0' && current != NULL) {
        // Skip dots
        if (*p == '.') {
            p++;
            continue;
        }

        // Extract key name
        int i = 0;
        while (*p != '\0' && *p != '.' && *p != '[' && i < 255) {
            buffer[i++] = *p++;
        }
        buffer[i] = '\0';

        // Navigate to the key
        if (i > 0) {
            current = d_DUFGetObjectItem(current, buffer);
            if (current == NULL) {
                return NULL;
            }
        }

        // Handle array indexing
        if (*p == '[') {
            p++; // Skip '['
            int index = 0;
            while (*p >= '0' && *p <= '9') {
                index = index * 10 + (*p - '0');
                p++;
            }
            if (*p == ']') {
                p++; // Skip ']'
            }

            // Navigate to array index
            dDUFValue_t* elem = current->child;
            for (int j = 0; j < index && elem != NULL; j++) {
                elem = elem->next;
            }
            current = elem;
        }
    }

    return current;
}

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

    dDUFValue_t* edge_cases = d_DUFGetObjectItem(g_data, "edge_cases");
    assert(edge_cases != NULL);

    // INT64_MAX
    dDUFValue_t* max_int_node = d_DUFGetObjectItem(edge_cases, "max_int");
    assert(max_int_node != NULL);
    if (max_int_node->value_int != INT64_MAX) {
        TEST_FAIL("INT64_MAX", "expected %lld, got %lld", (long long)INT64_MAX, (long long)max_int_node->value_int);
    }
    TEST_PASS("INT64_MAX");

    // INT64_MIN
    dDUFValue_t* min_int_node = d_DUFGetObjectItem(edge_cases, "min_int");
    assert(min_int_node != NULL);
    if (min_int_node->value_int != INT64_MIN) {
        TEST_FAIL("INT64_MIN", "expected %lld, got %lld", (long long)INT64_MIN, (long long)min_int_node->value_int);
    }
    TEST_PASS("INT64_MIN");

    // Zero
    dDUFValue_t* zero_node = d_DUFGetObjectItem(edge_cases, "zero");
    assert(zero_node != NULL && zero_node->value_int == 0);
    TEST_PASS("zero integer");

    // Negative
    dDUFValue_t* neg_node = d_DUFGetObjectItem(edge_cases, "negative");
    assert(neg_node != NULL && neg_node->value_int == -42);
    TEST_PASS("negative integer");

    // Float zero
    dDUFValue_t* fzero_node = d_DUFGetObjectItem(edge_cases, "float_zero");
    assert(fzero_node != NULL && fabs(fzero_node->value_double - 0.0) < 0.0001);
    TEST_PASS("float zero");

    // Small float
    dDUFValue_t* small_node = d_DUFGetObjectItem(edge_cases, "small_float");
    assert(small_node != NULL && fabs(small_node->value_double - 0.000001) < 0.0000001);
    TEST_PASS("small float");

    // Large float
    dDUFValue_t* large_node = d_DUFGetObjectItem(edge_cases, "large_float");
    assert(large_node != NULL && fabs(large_node->value_double - 999999.999999) < 0.01);
    TEST_PASS("large float");
}

void test_string_edge_cases(void)
{
    TEST_START("string edge cases");

    dDUFValue_t* edge_cases = d_DUFGetObjectItem(g_data, "edge_cases");
    assert(edge_cases != NULL);

    // Empty string
    dDUFValue_t* empty_node = d_DUFGetObjectItem(edge_cases, "empty_string");
    if (empty_node == NULL || empty_node->value_string == NULL || strlen(empty_node->value_string) != 0) {
        TEST_FAIL("empty string", "expected \"\", got %s", empty_node ? (empty_node->value_string ? empty_node->value_string : "NULL") : "NODE NULL");
    }
    TEST_PASS("empty string");

    // Escaped characters
    dDUFValue_t* escaped_node = d_DUFGetObjectItem(edge_cases, "escaped");
    assert(escaped_node != NULL && escaped_node->value_string != NULL);
    if (strstr(escaped_node->value_string, "\"Hello\"") == NULL) {
        TEST_FAIL("escaped quotes", "string doesn't contain escaped quotes");
    }
    if (strstr(escaped_node->value_string, "\\") == NULL) {
        TEST_FAIL("escaped backslash", "string doesn't contain backslash");
    }
    TEST_PASS("escaped characters");

    // Single character
    dDUFValue_t* single_node = d_DUFGetObjectItem(edge_cases, "single_char");
    assert(single_node != NULL && single_node->value_string != NULL);
    assert(strcmp(single_node->value_string, "x") == 0);
    assert(strlen(single_node->value_string) == 1);
    TEST_PASS("single character");
}

void test_empty_collections(void)
{
    TEST_START("empty collections");

    dDUFValue_t* edge_cases = d_DUFGetObjectItem(g_data, "edge_cases");
    assert(edge_cases != NULL);

    // Empty array
    dDUFValue_t* empty_arr = d_DUFGetObjectItem(edge_cases, "empty_array");
    if (empty_arr == NULL || d_DUFGetType(empty_arr) != D_DUF_ARRAY) {
        TEST_FAIL("empty array", "not found or wrong type");
    }
    // Check child is NULL for empty array
    assert(empty_arr->child == NULL);
    TEST_PASS("empty array");

    // Empty table
    dDUFValue_t* empty_tbl = d_DUFGetObjectItem(edge_cases, "empty_table");
    if (empty_tbl == NULL || d_DUFGetType(empty_tbl) != D_DUF_TABLE) {
        TEST_FAIL("empty table", "not found or wrong type");
    }
    // Check child is NULL for empty table
    assert(empty_tbl->child == NULL);
    TEST_PASS("empty table");
}

void test_deep_nesting(void)
{
    TEST_START("deep nesting");

    // 4-level deep path
    dDUFValue_t* deep_node = navigate_path(g_data, "edge_cases.complex.level1.level2.level3.deep_value");
    assert(deep_node != NULL && deep_node->value_string != NULL);
    if (strcmp(deep_node->value_string, "nested") != 0) {
        TEST_FAIL("4-level nesting", "expected 'nested', got '%s'", deep_node->value_string);
    }
    TEST_PASS("4-level nested table");

    // Deep array access
    dDUFValue_t* deep_arr_node = navigate_path(g_data, "edge_cases.complex.level1.level2.level3.deep_array[1]");
    assert(deep_arr_node != NULL && deep_arr_node->value_int == 2);
    TEST_PASS("deep nested array access");
}

void test_array_stress(void)
{
    TEST_START("array stress tests");

    // Array of tables
    dDUFValue_t* config0_name = navigate_path(g_data, "array_stress.configs[0].name");
    assert(config0_name != NULL && strcmp(config0_name->value_string, "config1") == 0);
    dDUFValue_t* config1_val = navigate_path(g_data, "array_stress.configs[1].value");
    assert(config1_val != NULL && config1_val->value_int == 20);
    TEST_PASS("array of tables");

    // Array of arrays (matrix)
    dDUFValue_t* matrix_val = navigate_path(g_data, "array_stress.matrix[1][1]");
    assert(matrix_val != NULL && matrix_val->value_int == 5); // Middle of matrix
    TEST_PASS("array of arrays (matrix)");

    // Large array - count children
    dDUFValue_t* range = navigate_path(g_data, "array_stress.range");
    assert(range != NULL);
    int count = 0;
    dDUFValue_t* elem = range->child;
    while (elem != NULL) {
        count++;
        elem = elem->next;
    }
    assert(count == 21); // 0-20 inclusive
    dDUFValue_t* last_elem = navigate_path(g_data, "array_stress.range[20]");
    assert(last_elem != NULL && last_elem->value_int == 20);
    TEST_PASS("large array (21 elements)");

    // Single element array
    dDUFValue_t* singleton = navigate_path(g_data, "array_stress.singleton");
    assert(singleton != NULL);
    assert(singleton->child != NULL && singleton->child->next == NULL); // Only 1 child
    dDUFValue_t* single_val = navigate_path(g_data, "array_stress.singleton[0]");
    assert(single_val != NULL && single_val->value_int == 42);
    TEST_PASS("singleton array");
}

void test_multiline_strings(void)
{
    TEST_START("multi-line strings");

    // Simple multi-line
    dDUFValue_t* simple_node = navigate_path(g_data, "multiline_strings.simple");
    assert(simple_node != NULL && simple_node->value_string != NULL);
    if (strstr(simple_node->value_string, "multi-line") == NULL || strstr(simple_node->value_string, "\n") == NULL) {
        TEST_FAIL("simple multi-line", "doesn't contain expected text or newlines");
    }
    TEST_PASS("simple multi-line");

    // Multi-line with quotes (no escaping needed)
    dDUFValue_t* quotes_node = navigate_path(g_data, "multiline_strings.with_quotes");
    assert(quotes_node != NULL && quotes_node->value_string != NULL);
    if (strstr(quotes_node->value_string, "\"Quoted text\"") == NULL) {
        TEST_FAIL("multi-line with quotes", "doesn't contain unescaped quotes");
    }
    TEST_PASS("multi-line with quotes");

    // Code snippet (with braces)
    dDUFValue_t* code_node = navigate_path(g_data, "multiline_strings.code_snippet");
    assert(code_node != NULL && code_node->value_string != NULL);
    if (strstr(code_node->value_string, "function") == NULL || strstr(code_node->value_string, "{") == NULL) {
        TEST_FAIL("code snippet", "doesn't contain expected code syntax");
    }
    TEST_PASS("code snippet preservation");

    // Formatted with blank lines
    dDUFValue_t* formatted_node = navigate_path(g_data, "multiline_strings.formatted");
    assert(formatted_node != NULL && formatted_node->value_string != NULL);
    if (strstr(formatted_node->value_string, "===") == NULL || strstr(formatted_node->value_string, "Bullet point") == NULL) {
        TEST_FAIL("formatted text", "doesn't contain expected formatting");
    }
    TEST_PASS("formatted text with blank lines");
}

void test_special_keys(void)
{
    TEST_START("special identifier names");

    // Underscore prefix
    dDUFValue_t* underscore_node = navigate_path(g_data, "special_keys._underscore");
    assert(underscore_node != NULL && strcmp(underscore_node->value_string, "starts with underscore") == 0);
    TEST_PASS("underscore prefix");

    // All caps
    dDUFValue_t* caps_node = navigate_path(g_data, "special_keys.CAPS_KEY");
    assert(caps_node != NULL && strcmp(caps_node->value_string, "all caps") == 0);
    TEST_PASS("all caps key");

    // Mixed case
    dDUFValue_t* mixed_node = navigate_path(g_data, "special_keys.mixedCase");
    assert(mixed_node != NULL && strcmp(mixed_node->value_string, "camelCase") == 0);
    TEST_PASS("camelCase key");

    // Ends with numbers
    dDUFValue_t* nums_node = navigate_path(g_data, "special_keys.key_123");
    assert(nums_node != NULL && strcmp(nums_node->value_string, "ends with numbers") == 0);
    TEST_PASS("key ending with numbers");

    // Mixed alphanumeric
    dDUFValue_t* alphanum_node = navigate_path(g_data, "special_keys.key123abc");
    assert(alphanum_node != NULL && strcmp(alphanum_node->value_string, "mixed alphanumeric") == 0);
    TEST_PASS("mixed alphanumeric key");
}

void test_realistic_structure(void)
{
    TEST_START("realistic complex enemy structure");

    // Basic fields
    dDUFValue_t* id_node = navigate_path(g_data, "realistic_enemy.id");
    assert(id_node != NULL && id_node->value_int == 999);
    dDUFValue_t* name_node = navigate_path(g_data, "realistic_enemy.name");
    assert(name_node != NULL && strcmp(name_node->value_string, "Glitch Hound") == 0);
    TEST_PASS("basic enemy fields");

    // Nested stats table
    dDUFValue_t* str_node = navigate_path(g_data, "realistic_enemy.stats.str");
    assert(str_node != NULL && str_node->value_int == 12);
    dDUFValue_t* dex_node = navigate_path(g_data, "realistic_enemy.stats.dex");
    assert(dex_node != NULL && dex_node->value_int == 18);
    TEST_PASS("nested stats table");

    // Resistances (floats)
    dDUFValue_t* fire_res_node = navigate_path(g_data, "realistic_enemy.resistances.fire");
    assert(fire_res_node != NULL && fabs(fire_res_node->value_double - 1.2) < 0.01);
    TEST_PASS("resistance floats");

    // Abilities array
    dDUFValue_t* abilities = navigate_path(g_data, "realistic_enemy.abilities");
    assert(abilities != NULL);
    int ability_count = 0;
    dDUFValue_t* ability = abilities->child;
    while (ability != NULL) {
        ability_count++;
        ability = ability->next;
    }
    assert(ability_count == 3);
    dDUFValue_t* ability1_node = navigate_path(g_data, "realistic_enemy.abilities[1]");
    assert(ability1_node != NULL && strcmp(ability1_node->value_string, "data_corruption") == 0);
    TEST_PASS("abilities array");

    // Loot table (array of tables)
    dDUFValue_t* loot_item_node = navigate_path(g_data, "realistic_enemy.loot_table[0].item");
    assert(loot_item_node != NULL && strcmp(loot_item_node->value_string, "scrap_metal") == 0);
    dDUFValue_t* loot_weight_node = navigate_path(g_data, "realistic_enemy.loot_table[0].weight");
    assert(loot_weight_node != NULL && loot_weight_node->value_int == 50);
    TEST_PASS("loot table (array of tables)");

    // AI behavior nested table
    dDUFValue_t* aggro_node = navigate_path(g_data, "realistic_enemy.ai_behavior.aggro_range");
    assert(aggro_node != NULL && fabs(aggro_node->value_double - 15.5) < 0.01);
    dDUFValue_t* tactical_node = navigate_path(g_data, "realistic_enemy.ai_behavior.tactical");
    assert(tactical_node != NULL && tactical_node->value_int == 0); // false = 0
    TEST_PASS("AI behavior table");

    // Spawn conditions with array
    dDUFValue_t* zones = navigate_path(g_data, "realistic_enemy.spawn_conditions.zones");
    assert(zones != NULL);
    int zone_count = 0;
    dDUFValue_t* zone_elem = zones->child;
    while (zone_elem != NULL) {
        zone_count++;
        zone_elem = zone_elem->next;
    }
    assert(zone_count == 3);
    dDUFValue_t* zone_node = navigate_path(g_data, "realistic_enemy.spawn_conditions.zones[1]");
    assert(zone_node != NULL && strcmp(zone_node->value_string, "corrupted_sector") == 0);
    TEST_PASS("spawn conditions with array");

    // Flavor text (multi-line)
    dDUFValue_t* flavor_node = navigate_path(g_data, "realistic_enemy.flavor_text");
    assert(flavor_node != NULL && flavor_node->value_string != NULL);
    if (strstr(flavor_node->value_string, "Feral programs") == NULL || strstr(flavor_node->value_string, "\n") == NULL) {
        TEST_FAIL("flavor text", "multi-line text not preserved");
    }
    TEST_PASS("multi-line flavor text");
}

void test_type_coercion(void)
{
    TEST_START("type coercion");

    // With AUF-style API, we access the actual type directly
    // Int stored in value_int field
    dDUFValue_t* zero_node = navigate_path(g_data, "edge_cases.zero");
    assert(zero_node != NULL && zero_node->type == D_DUF_INT);
    assert(zero_node->value_int == 0);
    TEST_PASS("int type stored correctly");

    // Float stored in value_double field
    dDUFValue_t* float_zero = navigate_path(g_data, "edge_cases.float_zero");
    assert(float_zero != NULL && float_zero->type == D_DUF_FLOAT);
    assert(fabs(float_zero->value_double - 0.0) < 0.0001);
    TEST_PASS("float type stored correctly");
}

void test_invalid_paths(void)
{
    TEST_START("invalid path handling");

    // Missing key - AUF-style returns NULL
    dDUFValue_t* missing = navigate_path(g_data, "nonexistent.key.path");
    assert(missing == NULL);
    TEST_PASS("missing key returns NULL");

    // Out of bounds array
    dDUFValue_t* oob = navigate_path(g_data, "array_stress.range[999]");
    assert(oob == NULL);
    TEST_PASS("out of bounds array returns NULL");

    // Wrong type access - with AUF style, user checks type themselves
    dDUFValue_t* max_int_node = navigate_path(g_data, "edge_cases.max_int");
    assert(max_int_node != NULL && max_int_node->type == D_DUF_INT);
    // User would check type before accessing wrong field
    TEST_PASS("can check type before access");

    // Null path
    dDUFValue_t* null_path = navigate_path(g_data, NULL);
    assert(null_path == NULL);
    TEST_PASS("NULL path returns NULL");

    // Empty path
    dDUFValue_t* empty_path = navigate_path(g_data, "");
    assert(empty_path == NULL);
    TEST_PASS("empty path returns NULL");
}

void test_boolean_arrays(void)
{
    TEST_START("boolean arrays");

    // Booleans stored as value_int (1=true, 0=false)
    dDUFValue_t* bool1 = navigate_path(g_data, "edge_cases.bool_array[0]");
    dDUFValue_t* bool2 = navigate_path(g_data, "edge_cases.bool_array[1]");
    dDUFValue_t* bool3 = navigate_path(g_data, "edge_cases.bool_array[2]");
    dDUFValue_t* bool4 = navigate_path(g_data, "edge_cases.bool_array[3]");

    assert(bool1 != NULL && bool1->value_int == 1); // true
    assert(bool2 != NULL && bool2->value_int == 0); // false
    assert(bool3 != NULL && bool3->value_int == 1); // true
    assert(bool4 != NULL && bool4->value_int == 0); // false

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
