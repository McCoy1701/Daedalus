// File: true_tests/statictables/test_static_tables_save_load.c - Static table save/load functionality tests

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// HELPER FUNCTIONS FOR TESTING
// =============================================================================

// Simple integer hash function for testing
static size_t int_hash_func(const void* key, size_t key_size) {
    (void)key_size; // Unused parameter
    int* int_key = (int*)key;
    return (size_t)(*int_key * 2654435761U); // Knuth's multiplicative hash
}

// Simple integer comparison function for testing
static int int_compare_func(const void* key1, const void* key2, size_t key_size) {
    (void)key_size; // Unused parameter
    int* k1 = (int*)key1;
    int* k2 = (int*)key2;
    return (*k1 == *k2) ? 0 : 1;
}

// String hash function for testing (simple djb2)
static size_t string_hash_func(const void* key, size_t key_size) {
    (void)key_size; // Unused parameter
    const char* str = *(const char**)key;
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// String comparison function for testing
static int string_compare_func(const void* key1, const void* key2, size_t key_size) {
    (void)key_size; // Unused parameter
    const char* str1 = *(const char**)key1;
    const char* str2 = *(const char**)key2;
    return strcmp(str1, str2);
}

// Test iterator callback to count entries
static void count_entries_callback(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key; (void)key_size; (void)value; (void)value_size; // Unused parameters
    int* count = (int*)user_data;
    (*count)++;
}

// Test iterator callback to verify data integrity
typedef struct {
    int* expected_keys;
    int* expected_values;
    int num_expected;
    int matches_found;
} VerifyData_t;

static void verify_data_callback(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size; // Unused parameters
    VerifyData_t* verify = (VerifyData_t*)user_data;
    
    // Skip verification if expected_keys is NULL (used for simple counting)
    if (!verify->expected_keys) {
        verify->matches_found++;
        return;
    }
    
    int k = *(int*)key;
    int v = *(int*)value;
    
    for (int i = 0; i < verify->num_expected; i++) {
        if (verify->expected_keys[i] == k && verify->expected_values[i] == v) {
            verify->matches_found++;
            break;
        }
    }
}

// Helper to clean up test files
static void cleanup_test_file(const char* filename) {
    if (access(filename, F_OK) == 0) {
        remove(filename);
    }
}

// =============================================================================
// SUCCESS TESTS
// =============================================================================

int test_save_load_basic_success(void)
{
    const char* test_file = "test_static_table_basic.dat";
    cleanup_test_file(test_file);
    
    // Create static table with known data
    int keys[] = {10, 20, 30, 40, 50};
    int values[] = {100, 200, 300, 400, 500};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3], &keys[4]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3], &values[4]};
    
    dStaticTable_t* original = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                                 int_compare_func, 8, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(original != NULL, "Should create original table successfully");
    
    // Save to file
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, original) == 0, "Should save table to file successfully");
    
    // Load from file
    dStaticTable_t* loaded = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded != NULL, "Should load table from file successfully");
    
    // Verify loaded table has same properties
    TEST_ASSERT(d_GetKeyCountOfStaticTable(loaded) == 5, "Loaded table should have 5 keys");
    
    // Verify all original data is present
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(d_CheckForKeyInStaticTable(loaded, &keys[i]) == 0, "All original keys should exist in loaded table");
        
        int* value = (int*)d_GetValueInStaticTable(loaded, &keys[i]);
        TEST_ASSERT(value != NULL && *value == values[i], "All original values should be preserved in loaded table");
    }
    
    // Test iterator on loaded table
    int entry_count = 0;
    TEST_ASSERT(d_IterateStaticTable(loaded, count_entries_callback, &entry_count) == 0, "Should iterate over loaded table successfully");
    TEST_ASSERT(entry_count == 5, "Should iterate over exactly 5 entries");
    
    d_DestroyStaticTable(&original);
    d_DestroyStaticTable(&loaded);
    cleanup_test_file(test_file);
    return 1;
}

int test_save_load_string_keys_success(void)
{
    const char* test_file = "test_static_table_strings.dat";
    cleanup_test_file(test_file);
    
    // Create static table with string keys
    char* keys[] = {"apple", "banana", "cherry", "date"};
    int values[] = {1, 2, 3, 4};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2], &values[3]};
    
    dStaticTable_t* original = d_InitStaticTable(sizeof(char*), sizeof(int), string_hash_func, 
                                                 string_compare_func, 16, key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(original != NULL, "Should create original string table successfully");
    
    // Save to file
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, original) == 0, "Should save string table to file successfully");
    
    // Load from file
    dStaticTable_t* loaded = d_LoadStaticTableFromFile(test_file, string_hash_func, string_compare_func);
    TEST_ASSERT(loaded != NULL, "Should load string table from file successfully");
    
    // Verify loaded table has same properties
    TEST_ASSERT(d_GetKeyCountOfStaticTable(loaded) == 4, "Loaded string table should have 4 keys");
    
    // Verify all original string data is present
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT(d_CheckForKeyInStaticTable(loaded, &keys[i]) == 0, "All original string keys should exist in loaded table");
        
        int* value = (int*)d_GetValueInStaticTable(loaded, &keys[i]);
        TEST_ASSERT(value != NULL && *value == values[i], "All original string values should be preserved in loaded table");
    }
    
    // Test iterator with data verification (just count entries for string test)
    VerifyData_t verify_data = {NULL, values, 4, 0};
    TEST_ASSERT(d_IterateStaticTable(loaded, verify_data_callback, &verify_data) == 0, "Should iterate over loaded string table successfully");
    TEST_ASSERT(verify_data.matches_found == 4, "Should find all 4 entries in string table");
    
    d_DestroyStaticTable(&original);
    d_DestroyStaticTable(&loaded);
    cleanup_test_file(test_file);
    return 1;
}

// =============================================================================
// BAD PATH TESTS
// =============================================================================

int test_save_load_invalid_parameters(void)
{
    const char* test_file = "test_static_table_invalid.dat";
    cleanup_test_file(test_file);
    
    // Create a valid table for testing
    int keys[] = {1, 2, 3};
    int values[] = {10, 20, 30};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2]};
    const void* value_ptrs[] = {&values[0], &values[1], &values[2]};
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 3);
    
    // Test save with invalid parameters
    TEST_ASSERT(d_SaveStaticTableToFile(NULL, table) == 1, "Save should fail with NULL filename");
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, NULL) == 1, "Save should fail with NULL table");
    TEST_ASSERT(d_SaveStaticTableToFile("", table) == 1, "Save should fail with empty filename");
    
    // Test load with invalid parameters
    TEST_ASSERT(d_LoadStaticTableFromFile(NULL, int_hash_func, int_compare_func) == NULL, "Load should fail with NULL filename");
    TEST_ASSERT(d_LoadStaticTableFromFile(test_file, NULL, int_compare_func) == NULL, "Load should fail with NULL hash function");
    TEST_ASSERT(d_LoadStaticTableFromFile(test_file, int_hash_func, NULL) == NULL, "Load should fail with NULL compare function");
    TEST_ASSERT(d_LoadStaticTableFromFile("nonexistent_file.dat", int_hash_func, int_compare_func) == NULL, "Load should fail with nonexistent file");
    
    // Test iterator with invalid parameters
    TEST_ASSERT(d_IterateStaticTable(NULL, count_entries_callback, NULL) == 1, "Iterate should fail with NULL table");
    TEST_ASSERT(d_IterateStaticTable(table, NULL, NULL) == 1, "Iterate should fail with NULL callback");
    
    d_DestroyStaticTable(&table);
    cleanup_test_file(test_file);
    return 1;
}

int test_save_load_corrupted_file(void)
{
    const char* test_file = "test_static_table_corrupted.dat";
    cleanup_test_file(test_file);
    
    // Create a file with invalid content
    FILE* corrupt_file = fopen(test_file, "wb");
    TEST_ASSERT(corrupt_file != NULL, "Should create corrupt test file");
    
    // Write wrong magic number
    uint32_t wrong_magic = 0xDEADBEEF;
    fwrite(&wrong_magic, sizeof(uint32_t), 1, corrupt_file);
    fclose(corrupt_file);
    
    // Try to load corrupted file
    dStaticTable_t* loaded1 = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded1 == NULL, "Should fail to load file with wrong magic number");
    
    // Create file with correct magic but wrong version
    corrupt_file = fopen(test_file, "wb");
    uint32_t correct_magic = 0xDAEDDDCD;
    uint32_t wrong_version = 999;
    fwrite(&correct_magic, sizeof(uint32_t), 1, corrupt_file);
    fwrite(&wrong_version, sizeof(uint32_t), 1, corrupt_file);
    fclose(corrupt_file);
    
    dStaticTable_t* loaded2 = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded2 == NULL, "Should fail to load file with wrong version");
    
    // Create file with truncated data
    corrupt_file = fopen(test_file, "wb");
    uint32_t correct_version = 1;
    fwrite(&correct_magic, sizeof(uint32_t), 1, corrupt_file);
    fwrite(&correct_version, sizeof(uint32_t), 1, corrupt_file);
    // Don't write complete metadata - file will be truncated
    fclose(corrupt_file);
    
    dStaticTable_t* loaded3 = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded3 == NULL, "Should fail to load truncated file");
    
    cleanup_test_file(test_file);
    return 1;
}

// =============================================================================
// MULTIPLE SAVE/LOAD TESTS WITH DATA ALTERATIONS
// =============================================================================

int test_save_load_multiple_cycles_with_updates(void)
{
    const char* test_file = "test_static_table_cycles.dat";
    cleanup_test_file(test_file);
    
    // Create initial table
    int keys[] = {1, 2, 3, 4, 5};
    int original_values[] = {10, 20, 30, 40, 50};
    const void* key_ptrs[] = {&keys[0], &keys[1], &keys[2], &keys[3], &keys[4]};
    const void* value_ptrs[] = {&original_values[0], &original_values[1], &original_values[2], &original_values[3], &original_values[4]};
    
    dStaticTable_t* table1 = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                               int_compare_func, 8, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(table1 != NULL, "Should create initial table successfully");
    
    // === CYCLE 1: Save original data ===
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, table1) == 0, "Should save original table successfully");
    
    dStaticTable_t* loaded1 = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded1 != NULL, "Should load table from cycle 1 successfully");
    
    // Update values in loaded table
    int updated_values_cycle1[] = {100, 200, 300, 400, 500};
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(d_SetValueInStaticTable(loaded1, &keys[i], &updated_values_cycle1[i]) == 0, "Should update values in cycle 1");
    }
    
    // === CYCLE 2: Save updated data ===
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, loaded1) == 0, "Should save updated table from cycle 1");
    
    dStaticTable_t* loaded2 = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded2 != NULL, "Should load table from cycle 2 successfully");
    
    // Verify cycle 1 updates are preserved
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_GetValueInStaticTable(loaded2, &keys[i]);
        TEST_ASSERT(value != NULL && *value == updated_values_cycle1[i], "Should preserve cycle 1 updates");
    }
    
    // Update values again
    int updated_values_cycle2[] = {1000, 2000, 3000, 4000, 5000};
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(d_SetValueInStaticTable(loaded2, &keys[i], &updated_values_cycle2[i]) == 0, "Should update values in cycle 2");
    }
    
    // === CYCLE 3: Save and verify final data ===
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, loaded2) == 0, "Should save updated table from cycle 2");
    
    dStaticTable_t* loaded3 = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded3 != NULL, "Should load table from cycle 3 successfully");
    
    // Verify final updates are preserved
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_GetValueInStaticTable(loaded3, &keys[i]);
        TEST_ASSERT(value != NULL && *value == updated_values_cycle2[i], "Should preserve cycle 2 updates");
    }
    
    // Test iterator on final table
    VerifyData_t verify_final = {keys, updated_values_cycle2, 5, 0};
    TEST_ASSERT(d_IterateStaticTable(loaded3, verify_data_callback, &verify_final) == 0, "Should iterate over final table successfully");
    TEST_ASSERT(verify_final.matches_found == 5, "Should find all expected key-value pairs in final table");
    
    d_DestroyStaticTable(&table1);
    d_DestroyStaticTable(&loaded1);
    d_DestroyStaticTable(&loaded2);
    d_DestroyStaticTable(&loaded3);
    cleanup_test_file(test_file);
    return 1;
}

int test_save_load_large_dataset_consistency(void)
{
    const char* test_file = "test_static_table_large.dat";
    cleanup_test_file(test_file);
    
    const size_t num_entries = 100;
    
    // Create large dataset
    int* keys = malloc(num_entries * sizeof(int));
    int* original_values = malloc(num_entries * sizeof(int));
    const void** key_ptrs = malloc(num_entries * sizeof(void*));
    const void** value_ptrs = malloc(num_entries * sizeof(void*));
    
    for (size_t i = 0; i < num_entries; i++) {
        keys[i] = (int)i;
        original_values[i] = (int)i * 3; // Original pattern
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &original_values[i];
    }
    
    dStaticTable_t* large_table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                                    int_compare_func, 32, key_ptrs, value_ptrs, num_entries);
    
    TEST_ASSERT(large_table != NULL, "Should create large table successfully");
    
    // === PHASE 1: Save and load original data ===
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, large_table) == 0, "Should save large table successfully");
    
    dStaticTable_t* loaded_large = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded_large != NULL, "Should load large table successfully");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(loaded_large) == num_entries, "Should load correct number of entries");
    
    // Verify all original data
    for (size_t i = 0; i < num_entries; i++) {
        int* value = (int*)d_GetValueInStaticTable(loaded_large, &keys[i]);
        TEST_ASSERT(value != NULL && *value == original_values[i], "Should preserve all original values in large dataset");
    }
    
    // === PHASE 2: Update every other value ===
    int* updated_values = malloc(num_entries * sizeof(int));
    for (size_t i = 0; i < num_entries; i++) {
        if (i % 2 == 0) {
            updated_values[i] = original_values[i] * 10; // Update even indices
            TEST_ASSERT(d_SetValueInStaticTable(loaded_large, &keys[i], &updated_values[i]) == 0, "Should update even-indexed values");
        } else {
            updated_values[i] = original_values[i]; // Keep odd indices unchanged
        }
    }
    
    // === PHASE 3: Save and load updated data ===
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, loaded_large) == 0, "Should save updated large table successfully");
    
    dStaticTable_t* final_large = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(final_large != NULL, "Should load final large table successfully");
    
    // Verify selective updates
    for (size_t i = 0; i < num_entries; i++) {
        int* value = (int*)d_GetValueInStaticTable(final_large, &keys[i]);
        TEST_ASSERT(value != NULL && *value == updated_values[i], "Should preserve selective updates in large dataset");
    }
    
    // === PHASE 4: Test iterator on large dataset ===
    int large_entry_count = 0;
    TEST_ASSERT(d_IterateStaticTable(final_large, count_entries_callback, &large_entry_count) == 0, "Should iterate over large dataset successfully");
    TEST_ASSERT(large_entry_count == (int)num_entries, "Should iterate over all entries in large dataset");
    
    // Cleanup
    d_DestroyStaticTable(&large_table);
    d_DestroyStaticTable(&loaded_large);
    d_DestroyStaticTable(&final_large);
    free(keys);
    free(original_values);
    free(updated_values);
    free(key_ptrs);
    free(value_ptrs);
    cleanup_test_file(test_file);
    return 1;
}

// =============================================================================
// DEBUG TESTS - ADVANCED INTEGRATION AND CALLBACK EXPLORATION
// =============================================================================

// Advanced callback that logs all entries and calculates statistics
typedef struct {
    int total_entries;
    int sum_keys;
    int sum_values;
    int min_key;
    int max_key;
    int min_value;
    int max_value;
    bool first_entry;
} StatsData_t;

static void stats_callback(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size; // Unused parameters
    StatsData_t* stats = (StatsData_t*)user_data;
    int k = *(int*)key;
    int v = *(int*)value;
    
    stats->total_entries++;
    stats->sum_keys += k;
    stats->sum_values += v;
    
    if (stats->first_entry) {
        stats->min_key = stats->max_key = k;
        stats->min_value = stats->max_value = v;
        stats->first_entry = false;
    } else {
        if (k < stats->min_key) stats->min_key = k;
        if (k > stats->max_key) stats->max_key = k;
        if (v < stats->min_value) stats->min_value = v;
        if (v > stats->max_value) stats->max_value = v;
    }
}

// Callback that collects all keys into a dynamically allocated array
typedef struct {
    int* collected_keys;
    int capacity;
    int count;
} KeyCollector_t;

static void key_collector_callback(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value; (void)value_size; // Unused parameters
    KeyCollector_t* collector = (KeyCollector_t*)user_data;
    int k = *(int*)key;
    
    if (collector->count >= collector->capacity) {
        // Expand array if needed
        collector->capacity *= 2;
        collector->collected_keys = realloc(collector->collected_keys, collector->capacity * sizeof(int));
    }
    
    collector->collected_keys[collector->count++] = k;
}

// Callback that searches for specific key-value pairs
typedef struct {
    int target_key;
    int target_value;
    bool found_match;
    int search_count;
} SearchData_t;

static void search_callback(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size; // Unused parameters
    SearchData_t* search = (SearchData_t*)user_data;
    int k = *(int*)key;
    int v = *(int*)value;
    
    search->search_count++;
    if (k == search->target_key && v == search->target_value) {
        search->found_match = true;
    }
}

// Callback that builds a string representation of all entries (for debugging)
typedef struct {
    char* debug_string;
    size_t capacity;
    size_t length;
} DebugString_t;

static void debug_string_callback(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size; // Unused parameters
    DebugString_t* debug = (DebugString_t*)user_data;
    int k = *(int*)key;
    int v = *(int*)value;
    
    char entry_str[64];
    int entry_len = snprintf(entry_str, sizeof(entry_str), "[%d:%d] ", k, v);
    
    // Ensure we have enough space
    while (debug->length + entry_len + 1 >= debug->capacity) {
        debug->capacity *= 2;
        debug->debug_string = realloc(debug->debug_string, debug->capacity);
    }
    
    strcat(debug->debug_string + debug->length, entry_str);
    debug->length += entry_len;
}

int test_debug_stats_with_save_load_clone(void)
{
    const char* test_file = "test_debug_stats.dat";
    cleanup_test_file(test_file);
    
    // Create table with known statistical properties
    int keys[] = {5, 15, 25, 35, 45, 55, 65, 75};
    int values[] = {10, 30, 50, 70, 90, 110, 130, 150};
    const void* key_ptrs[8];
    const void* value_ptrs[8];
    
    for (int i = 0; i < 8; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    dStaticTable_t* original = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                                 int_compare_func, 4, key_ptrs, value_ptrs, 8);
    
    TEST_ASSERT(original != NULL, "Should create original table for stats test");
    
    // Get original statistics
    StatsData_t original_stats = {0, 0, 0, 0, 0, 0, 0, true};
    TEST_ASSERT(d_IterateStaticTable(original, stats_callback, &original_stats) == 0, "Should collect original stats");
    TEST_ASSERT(original_stats.total_entries == 8, "Should count 8 entries in original");
    TEST_ASSERT(original_stats.min_key == 5 && original_stats.max_key == 75, "Should find correct key range");
    TEST_ASSERT(original_stats.min_value == 10 && original_stats.max_value == 150, "Should find correct value range");
    
    // Clone the table
    dStaticTable_t* cloned = d_CloneStaticTable(original);
    TEST_ASSERT(cloned != NULL, "Should clone table successfully");
    
    // Save cloned table
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, cloned) == 0, "Should save cloned table");
    
    // Load and verify statistics are preserved
    dStaticTable_t* loaded = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded != NULL, "Should load cloned table");
    
    StatsData_t loaded_stats = {0, 0, 0, 0, 0, 0, 0, true};
    TEST_ASSERT(d_IterateStaticTable(loaded, stats_callback, &loaded_stats) == 0, "Should collect loaded stats");
    TEST_ASSERT(loaded_stats.total_entries == original_stats.total_entries, "Should preserve entry count through clone/save/load");
    TEST_ASSERT(loaded_stats.sum_keys == original_stats.sum_keys, "Should preserve key sum through clone/save/load");
    TEST_ASSERT(loaded_stats.sum_values == original_stats.sum_values, "Should preserve value sum through clone/save/load");
    
    d_DestroyStaticTable(&original);
    d_DestroyStaticTable(&cloned);
    d_DestroyStaticTable(&loaded);
    cleanup_test_file(test_file);
    return 1;
}

int test_debug_key_collection_with_rebucket_save_load(void)
{
    const char* test_file = "test_debug_keys.dat";
    cleanup_test_file(test_file);
    
    // Create table with specific keys for collection testing
    int keys[] = {100, 200, 300, 400, 500, 600};
    int values[] = {1, 2, 3, 4, 5, 6};
    const void* key_ptrs[6];
    const void* value_ptrs[6];
    
    for (int i = 0; i < 6; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    dStaticTable_t* original = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                                 int_compare_func, 2, key_ptrs, value_ptrs, 6);
    
    TEST_ASSERT(original != NULL, "Should create original table for key collection test");
    
    // Collect original keys using callback
    KeyCollector_t original_collector = {malloc(10 * sizeof(int)), 10, 0};
    TEST_ASSERT(d_IterateStaticTable(original, key_collector_callback, &original_collector) == 0, "Should collect original keys");
    TEST_ASSERT(original_collector.count == 6, "Should collect all 6 keys");
    
    // Rebucket to different size
    dStaticTable_t* rebucketed = d_RebucketStaticTable(original, 16);
    TEST_ASSERT(rebucketed != NULL, "Should rebucket table successfully");
    
    // Save rebucketed table
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, rebucketed) == 0, "Should save rebucketed table");
    
    // Load and collect keys again
    dStaticTable_t* loaded = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded != NULL, "Should load rebucketed table");
    
    KeyCollector_t loaded_collector = {malloc(10 * sizeof(int)), 10, 0};
    TEST_ASSERT(d_IterateStaticTable(loaded, key_collector_callback, &loaded_collector) == 0, "Should collect loaded keys");
    TEST_ASSERT(loaded_collector.count == 6, "Should collect all 6 keys after rebucket/save/load");
    
    // Verify all original keys are present (order may vary)
    for (int i = 0; i < 6; i++) {
        bool found = false;
        for (int j = 0; j < loaded_collector.count; j++) {
            if (loaded_collector.collected_keys[j] == keys[i]) {
                found = true;
                break;
            }
        }
        TEST_ASSERT(found, "Should find all original keys after rebucket/save/load");
    }
    
    free(original_collector.collected_keys);
    free(loaded_collector.collected_keys);
    d_DestroyStaticTable(&original);
    d_DestroyStaticTable(&rebucketed);
    d_DestroyStaticTable(&loaded);
    cleanup_test_file(test_file);
    return 1;
}

int test_debug_search_callback_with_updates_and_save_load(void)
{
    const char* test_file = "test_debug_search.dat";
    cleanup_test_file(test_file);
    
    // Create table for search testing
    int keys[] = {10, 20, 30, 40, 50};
    int values[] = {100, 200, 300, 400, 500};
    const void* key_ptrs[5];
    const void* value_ptrs[5];
    
    for (int i = 0; i < 5; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(table != NULL, "Should create table for search test");
    
    // Search for existing key-value pair
    SearchData_t search1 = {30, 300, false, 0};
    TEST_ASSERT(d_IterateStaticTable(table, search_callback, &search1) == 0, "Should search in original table");
    TEST_ASSERT(search1.found_match == true, "Should find existing key-value pair");
    TEST_ASSERT(search1.search_count == 5, "Should check all entries during search");
    
    // Update the value we just found
    int new_value = 3000;
    TEST_ASSERT(d_SetValueInStaticTable(table, &keys[2], &new_value) == 0, "Should update value successfully");
    
    // Search for old value (should not find)
    SearchData_t search2 = {30, 300, false, 0};
    TEST_ASSERT(d_IterateStaticTable(table, search_callback, &search2) == 0, "Should search after update");
    TEST_ASSERT(search2.found_match == false, "Should not find old value after update");
    
    // Search for new value (should find)
    SearchData_t search3 = {30, 3000, false, 0};
    TEST_ASSERT(d_IterateStaticTable(table, search_callback, &search3) == 0, "Should search for new value");
    TEST_ASSERT(search3.found_match == true, "Should find new value after update");
    
    // Save updated table
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, table) == 0, "Should save updated table");
    
    // Load and verify search still works
    dStaticTable_t* loaded = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded != NULL, "Should load updated table");
    
    SearchData_t search4 = {30, 3000, false, 0};
    TEST_ASSERT(d_IterateStaticTable(loaded, search_callback, &search4) == 0, "Should search in loaded table");
    TEST_ASSERT(search4.found_match == true, "Should find updated value in loaded table");
    
    // Search for non-existent pair
    SearchData_t search5 = {999, 999, false, 0};
    TEST_ASSERT(d_IterateStaticTable(loaded, search_callback, &search5) == 0, "Should search for non-existent pair");
    TEST_ASSERT(search5.found_match == false, "Should not find non-existent pair");
    
    d_DestroyStaticTable(&table);
    d_DestroyStaticTable(&loaded);
    cleanup_test_file(test_file);
    return 1;
}

int test_debug_string_builder_with_clear_and_save_load(void)
{
    const char* test_file = "test_debug_string.dat";
    cleanup_test_file(test_file);
    
    // Create table for string building test
    int keys[] = {1, 2, 3};
    int values[] = {10, 20, 30};
    const void* key_ptrs[3];
    const void* value_ptrs[3];
    
    for (int i = 0; i < 3; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), int_hash_func, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 3);
    
    TEST_ASSERT(table != NULL, "Should create table for string builder test");
    
    // Build debug string from original table
    DebugString_t debug1 = {malloc(256), 256, 0};
    debug1.debug_string[0] = '\0';
    TEST_ASSERT(d_IterateStaticTable(table, debug_string_callback, &debug1) == 0, "Should build debug string");
    TEST_ASSERT(strlen(debug1.debug_string) > 0, "Should generate non-empty debug string");
    TEST_ASSERT(strstr(debug1.debug_string, "[1:10]") != NULL, "Should contain first key-value pair");
    TEST_ASSERT(strstr(debug1.debug_string, "[2:20]") != NULL, "Should contain second key-value pair");
    TEST_ASSERT(strstr(debug1.debug_string, "[3:30]") != NULL, "Should contain third key-value pair");
    
    // Save table with debug info
    TEST_ASSERT(d_SaveStaticTableToFile(test_file, table) == 0, "Should save table");
    
    // Clear table and verify it's empty
    TEST_ASSERT(d_ClearStaticTable(table) == 0, "Should clear table");
    TEST_ASSERT(d_GetKeyCountOfStaticTable(table) == 0, "Should have 0 keys after clear");
    
    // Try to iterate over cleared table (should fail gracefully)
    DebugString_t debug2 = {malloc(256), 256, 0};
    debug2.debug_string[0] = '\0';
    TEST_ASSERT(d_IterateStaticTable(table, debug_string_callback, &debug2) == 1, "Should fail to iterate on cleared table");
    TEST_ASSERT(strlen(debug2.debug_string) == 0, "Should have empty debug string when iteration fails");
    
    // Load saved table and build debug string again
    dStaticTable_t* loaded = d_LoadStaticTableFromFile(test_file, int_hash_func, int_compare_func);
    TEST_ASSERT(loaded != NULL, "Should load table after clear");
    
    DebugString_t debug3 = {malloc(256), 256, 0};
    debug3.debug_string[0] = '\0';
    TEST_ASSERT(d_IterateStaticTable(loaded, debug_string_callback, &debug3) == 0, "Should build debug string from loaded table");
    TEST_ASSERT(strlen(debug3.debug_string) > 0, "Should generate non-empty debug string from loaded table");
    
    // Verify loaded table has same debug representation as original
    TEST_ASSERT(strstr(debug3.debug_string, "[1:10]") != NULL, "Loaded table should contain first key-value pair");
    TEST_ASSERT(strstr(debug3.debug_string, "[2:20]") != NULL, "Loaded table should contain second key-value pair");
    TEST_ASSERT(strstr(debug3.debug_string, "[3:30]") != NULL, "Loaded table should contain third key-value pair");
    
    free(debug1.debug_string);
    free(debug2.debug_string);
    free(debug3.debug_string);
    d_DestroyStaticTable(&table);
    d_DestroyStaticTable(&loaded);
    cleanup_test_file(test_file);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

// Main test runner
int main(void)
{
    // Initialize Daedalus logging system
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = true,
        .include_file_info = false,
        .include_function = false,
        .include_thread_id = false,
        .colorize_output = true,
        .timestamp_format = NULL,
        .context_separator = "::"
    };
    
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);
    
    d_LogInfo("Starting Static Hash Table Save/Load Tests");

    TEST_SUITE_START("Static Hash Table Save/Load Tests");

    // Run success tests
    RUN_TEST(test_save_load_basic_success);
    RUN_TEST(test_save_load_string_keys_success);
    
    // Run bad path tests
    RUN_TEST(test_save_load_invalid_parameters);
    RUN_TEST(test_save_load_corrupted_file);
    
    // Run multiple cycle tests
    RUN_TEST(test_save_load_multiple_cycles_with_updates);
    RUN_TEST(test_save_load_large_dataset_consistency);
    
    // Run debug tests
    RUN_TEST(test_debug_stats_with_save_load_clone);
    RUN_TEST(test_debug_key_collection_with_rebucket_save_load);
    RUN_TEST(test_debug_search_callback_with_updates_and_save_load);
    RUN_TEST(test_debug_string_builder_with_clear_and_save_load);

    TEST_SUITE_END();

    d_LogInfo("Static Hash Table Save/Load Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}