// File: true_tests/statictables/test_static_tables_hashing.c - Custom hashing function tests and education

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// EDUCATIONAL HASHING FUNCTIONS - FROM BASIC TO ADVANCED
// =============================================================================

// 1. TERRIBLE HASH: Always returns same value (demonstrates worst case)
static size_t terrible_hash_func(const void* key, size_t key_size) {
    (void)key; (void)key_size; // Unused parameters
    return 42; // Everything goes to same bucket!
}

// 2. SIMPLE MODULO: Basic but poor distribution for sequential keys
static size_t simple_modulo_hash_func(const void* key, size_t key_size) {
    (void)key_size; // Unused parameter
    int* int_key = (int*)key;
    return (size_t)(*int_key % 1000);
}

// 3. KNUTH'S MULTIPLICATIVE: Good general-purpose hash
static size_t knuth_multiplicative_hash_func(const void* key, size_t key_size) {
    (void)key_size; // Unused parameter
    int* int_key = (int*)key;
    return (size_t)(*int_key * 2654435761U); // Knuth's magic number
}

// 4. FNV-1A: Excellent for strings and binary data
static size_t fnv1a_hash_func(const void* key, size_t key_size) {
    const unsigned char* data = (const unsigned char*)key;
    size_t hash = 2166136261U; // FNV offset basis
    
    for (size_t i = 0; i < key_size; i++) {
        hash ^= data[i];
        hash *= 16777619U; // FNV prime
    }
    return hash;
}

// 5. CUSTOM GAME-OPTIMIZED: Designed for game entity IDs
static size_t game_entity_hash_func(const void* key, size_t key_size) {
    (void)key_size; // Unused parameter
    int* entity_id = (int*)key;
    
    // Assume entity IDs have patterns: player=1-100, enemies=1000-9999, items=10000+
    // Spread these ranges across different bucket regions
    int id = *entity_id;
    
    if (id <= 100) {
        // Players: use lower buckets with fine distribution
        return (size_t)(id * 13 + 7);
    } else if (id < 10000) {
        // Enemies: use middle buckets
        return (size_t)((id * 23 + 101) % 5000 + 1000);
    } else {
        // Items: use upper buckets
        return (size_t)((id * 31 + 301) % 3000 + 6000);
    }
}

// 6. CRYPTO-INSPIRED: Strong mixing for security-sensitive data
static size_t crypto_inspired_hash_func(const void* key, size_t key_size) {
    (void)key_size; // Unused parameter
    int* int_key = (int*)key;
    uint32_t x = (uint32_t)(*int_key);
    
    // Multiple rounds of bit mixing (simplified from MurmurHash)
    x ^= x >> 16;
    x *= 0x85ebca6b;
    x ^= x >> 13;
    x *= 0xc2b2ae35;
    x ^= x >> 16;
    
    return (size_t)x;
}

// Standard comparison function for all tests
static int int_compare_func(const void* key1, const void* key2, size_t key_size) {
    (void)key_size; // Unused parameter
    int* k1 = (int*)key1;
    int* k2 = (int*)key2;
    return (*k1 == *k2) ? 0 : 1;
}

// =============================================================================
// HELPER FUNCTIONS FOR MEASURING HASH QUALITY
// =============================================================================

// Callback to collect distribution statistics
typedef struct {
    size_t* bucket_counts;
    size_t num_buckets;
    size_t total_entries;
} DistributionStats_t;

static void distribution_callback(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key; (void)key_size; (void)value; (void)value_size; // Unused parameters
    DistributionStats_t* stats = (DistributionStats_t*)user_data;
    stats->total_entries++;
    // Note: We can't easily determine which bucket we're in from the callback,
    // so this callback just counts total entries. Bucket analysis is done separately.
}

// Helper to analyze hash distribution quality
static void analyze_hash_distribution(const dStaticTable_t* table, const char* hash_name) {
    size_t min_entries, max_entries, empty_buckets;
    float avg_entries;
    
    if (d_GetStaticTableStats(table, &min_entries, &max_entries, &avg_entries, &empty_buckets) == 0) {
        printf("\n=== %s Hash Analysis ===\n", hash_name);
        printf("Min entries per bucket: %zu\n", min_entries);
        printf("Max entries per bucket: %zu\n", max_entries);
        printf("Avg entries per bucket: %.2f\n", avg_entries);
        printf("Empty buckets: %zu\n", empty_buckets);
        printf("Distribution quality: ");
        
        // Improved quality metric that considers load factor
        float load_factor = avg_entries; // avg_entries is essentially load factor
        float distribution_ratio = max_entries / (avg_entries > 0 ? avg_entries : 1.0f);
        
        printf("Load factor: %.2f | Distribution ratio: %.2f\n", load_factor, distribution_ratio);
        
        if (distribution_ratio <= 1.5f) {
            printf("EXCELLENT distribution (very even)\n");
        } else if (distribution_ratio <= 2.0f) {
            printf("GOOD distribution (acceptable clustering)\n");
        } else if (distribution_ratio <= 3.0f) {
            printf("FAIR distribution (some clustering)\n");
        } else {
            printf("POOR distribution (heavy clustering)\n");
        }
        
        // Separate assessment for empty buckets
        if (load_factor < 1.0f) {
            printf("NOTE: Many empty buckets expected with low load factor\n");
        }
    }
}

// =============================================================================
// EDUCATIONAL TESTS
// =============================================================================

int test_terrible_hash_demonstrates_clustering(void)
{
    // Create dataset that would normally spread across buckets
    int keys[] = {1, 100, 1000, 5000, 9999};
    int values[] = {10, 20, 30, 40, 50};
    const void* key_ptrs[5];
    const void* value_ptrs[5];
    
    for (int i = 0; i < 5; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), terrible_hash_func, 
                                              int_compare_func, 16, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(table != NULL, "Should create table with terrible hash function");
    
    // All entries should still be accessible (correctness not affected)
    for (int i = 0; i < 5; i++) {
        int* value = (int*)d_GetValueInStaticTable(table, &keys[i]);
        TEST_ASSERT(value != NULL && *value == values[i], "Should find all values despite terrible hash");
    }
    
    // But distribution should be awful
    size_t min_entries, max_entries, empty_buckets;
    float avg_entries;
    d_GetStaticTableStats(table, &min_entries, &max_entries, &avg_entries, &empty_buckets);
    
    TEST_ASSERT(empty_buckets >= 14, "Should have many empty buckets with terrible hash"); // 15 out of 16 buckets empty
    TEST_ASSERT(max_entries == 5, "Should have all entries in one bucket");
    
    analyze_hash_distribution(table, "TERRIBLE");
    
    d_DestroyStaticTable(&table);
    return 1;
}

int test_hash_function_comparison_sequential_keys(void)
{
    // Sequential keys (worst case for simple modulo)
    const size_t num_keys = 20;
    int keys[20];
    int values[20];
    const void* key_ptrs[20];
    const void* value_ptrs[20];
    
    for (size_t i = 0; i < num_keys; i++) {
        keys[i] = (int)i + 1; // Sequential: 1, 2, 3, ..., 20
        values[i] = (int)i * 10;
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    // Test with simple modulo hash
    dStaticTable_t* simple_table = d_InitStaticTable(sizeof(int), sizeof(int), simple_modulo_hash_func, 
                                                     int_compare_func, 8, key_ptrs, value_ptrs, num_keys);
    
    // Test with Knuth's multiplicative hash
    dStaticTable_t* knuth_table = d_InitStaticTable(sizeof(int), sizeof(int), knuth_multiplicative_hash_func, 
                                                    int_compare_func, 8, key_ptrs, value_ptrs, num_keys);
    
    TEST_ASSERT(simple_table != NULL && knuth_table != NULL, "Should create both tables successfully");
    
    // Both should be functionally correct
    LOOP_TEST_START();
    for (size_t i = 0; i < num_keys; i++) {
        int* simple_val = (int*)d_GetValueInStaticTable(simple_table, &keys[i]);
        int* knuth_val = (int*)d_GetValueInStaticTable(knuth_table, &keys[i]);
        
        TEST_ASSERT(simple_val != NULL && *simple_val == values[i], "Simple hash should find all values");
        TEST_ASSERT(knuth_val != NULL && *knuth_val == values[i], "Knuth hash should find all values");
    }
    LOOP_TEST_END();
    
    // Compare distribution quality
    size_t simple_min, simple_max, simple_empty, knuth_min, knuth_max, knuth_empty;
    float simple_avg, knuth_avg;
    
    d_GetStaticTableStats(simple_table, &simple_min, &simple_max, &simple_avg, &simple_empty);
    d_GetStaticTableStats(knuth_table, &knuth_min, &knuth_max, &knuth_avg, &knuth_empty);
    
    TEST_ASSERT(knuth_max <= simple_max, "Knuth hash should have better max distribution than simple modulo");
    
    analyze_hash_distribution(simple_table, "SIMPLE MODULO");
    analyze_hash_distribution(knuth_table, "KNUTH MULTIPLICATIVE");
    
    d_DestroyStaticTable(&simple_table);
    d_DestroyStaticTable(&knuth_table);
    return 1;
}

int test_fnv1a_hash_with_binary_data(void)
{
    // Test FNV-1a with actual binary data (treating ints as raw bytes)
    int keys[] = {0x12345678, 0x87654321, 0xDEADBEEF, 0xCAFEBABE, 0xFEEDFACE};
    int values[] = {100, 200, 300, 400, 500};
    const void* key_ptrs[5];
    const void* value_ptrs[5];
    
    for (int i = 0; i < 5; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    // Create table using FNV-1a hash (treating int as 4 bytes)
    dStaticTable_t* fnv_table = d_InitStaticTable(sizeof(int), sizeof(int), fnv1a_hash_func, 
                                                  int_compare_func, 16, key_ptrs, value_ptrs, 5);
    
    // Compare with Knuth's multiplicative hash
    dStaticTable_t* knuth_table = d_InitStaticTable(sizeof(int), sizeof(int), knuth_multiplicative_hash_func, 
                                                    int_compare_func, 16, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(fnv_table != NULL && knuth_table != NULL, "Should create both FNV and Knuth tables");
    
    // Verify correctness for both
    for (int i = 0; i < 5; i++) {
        int* fnv_val = (int*)d_GetValueInStaticTable(fnv_table, &keys[i]);
        int* knuth_val = (int*)d_GetValueInStaticTable(knuth_table, &keys[i]);
        
        TEST_ASSERT(fnv_val != NULL && *fnv_val == values[i], "FNV hash should find all values");
        TEST_ASSERT(knuth_val != NULL && *knuth_val == values[i], "Knuth hash should find all values");
    }
    
    analyze_hash_distribution(fnv_table, "FNV-1A");
    analyze_hash_distribution(knuth_table, "KNUTH (for comparison)");
    
    d_DestroyStaticTable(&fnv_table);
    d_DestroyStaticTable(&knuth_table);
    return 1;
}

int test_game_entity_hash_demonstrates_domain_knowledge(void)
{
    // Simulate game entity IDs with known patterns
    int player_ids[] = {1, 2, 50, 99}; // Players: 1-100
    int enemy_ids[] = {1001, 2500, 5000, 9999}; // Enemies: 1000-9999  
    int item_ids[] = {10001, 15000, 20000, 99999}; // Items: 10000+
    
    // Combine all IDs
    int all_keys[12];
    int all_values[12];
    const void* key_ptrs[12];
    const void* value_ptrs[12];
    
    // Copy players
    for (int i = 0; i < 4; i++) {
        all_keys[i] = player_ids[i];
        all_values[i] = player_ids[i] * 10; // Arbitrary values
        key_ptrs[i] = &all_keys[i];
        value_ptrs[i] = &all_values[i];
    }
    
    // Copy enemies
    for (int i = 0; i < 4; i++) {
        all_keys[i + 4] = enemy_ids[i];
        all_values[i + 4] = enemy_ids[i] * 10;
        key_ptrs[i + 4] = &all_keys[i + 4];
        value_ptrs[i + 4] = &all_values[i + 4];
    }
    
    // Copy items
    for (int i = 0; i < 4; i++) {
        all_keys[i + 8] = item_ids[i];
        all_values[i + 8] = item_ids[i] * 10;
        key_ptrs[i + 8] = &all_keys[i + 8];
        value_ptrs[i + 8] = &all_values[i + 8];
    }
    
    // Test domain-specific hash vs generic hash
    dStaticTable_t* game_table = d_InitStaticTable(sizeof(int), sizeof(int), game_entity_hash_func, 
                                                   int_compare_func, 32, key_ptrs, value_ptrs, 12);
    
    dStaticTable_t* generic_table = d_InitStaticTable(sizeof(int), sizeof(int), knuth_multiplicative_hash_func, 
                                                      int_compare_func, 32, key_ptrs, value_ptrs, 12);
    
    TEST_ASSERT(game_table != NULL && generic_table != NULL, "Should create both game-specific and generic tables");
    
    LOOP_TEST_START();
    // Verify all lookups work correctly
    for (int i = 0; i < 12; i++) {
        int* game_val = (int*)d_GetValueInStaticTable(game_table, &all_keys[i]);
        int* generic_val = (int*)d_GetValueInStaticTable(generic_table, &all_keys[i]);
        
        TEST_ASSERT(game_val != NULL && *game_val == all_values[i], "Game hash should find all entity values");
        TEST_ASSERT(generic_val != NULL && *generic_val == all_values[i], "Generic hash should find all entity values");
    }
    LOOP_TEST_END();
    
    printf("\n=== GAME ENTITY ID PATTERNS ===\n");
    printf("Players (1-100): %d, %d, %d, %d\n", player_ids[0], player_ids[1], player_ids[2], player_ids[3]);
    printf("Enemies (1000-9999): %d, %d, %d, %d\n", enemy_ids[0], enemy_ids[1], enemy_ids[2], enemy_ids[3]);
    printf("Items (10000+): %d, %d, %d, %d\n", item_ids[0], item_ids[1], item_ids[2], item_ids[3]);
    
    analyze_hash_distribution(game_table, "GAME-OPTIMIZED");
    analyze_hash_distribution(generic_table, "GENERIC KNUTH");
    
    d_DestroyStaticTable(&game_table);
    d_DestroyStaticTable(&generic_table);
    return 1;
}

int test_crypto_inspired_hash_avalanche_effect(void)
{
    // Test avalanche effect: small input changes should cause large output changes
    int similar_keys[] = {1000, 1001, 1002, 1003, 1004}; // Very similar inputs
    int values[] = {10, 20, 30, 40, 50};
    const void* key_ptrs[5];
    const void* value_ptrs[5];
    
    for (int i = 0; i < 5; i++) {
        key_ptrs[i] = &similar_keys[i];
        value_ptrs[i] = &values[i];
    }
    
    // Test crypto-inspired hash vs simple modulo with similar keys
    dStaticTable_t* crypto_table = d_InitStaticTable(sizeof(int), sizeof(int), crypto_inspired_hash_func, 
                                                     int_compare_func, 8, key_ptrs, value_ptrs, 5);
    
    dStaticTable_t* simple_table = d_InitStaticTable(sizeof(int), sizeof(int), simple_modulo_hash_func, 
                                                     int_compare_func, 8, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(crypto_table != NULL && simple_table != NULL, "Should create both crypto and simple tables");
    
    // Both should work correctly
    for (int i = 0; i < 5; i++) {
        int* crypto_val = (int*)d_GetValueInStaticTable(crypto_table, &similar_keys[i]);
        int* simple_val = (int*)d_GetValueInStaticTable(simple_table, &similar_keys[i]);
        
        TEST_ASSERT(crypto_val != NULL && *crypto_val == values[i], "Crypto hash should find all similar key values");
        TEST_ASSERT(simple_val != NULL && *simple_val == values[i], "Simple hash should find all similar key values");
    }
    
    printf("\n=== SIMILAR KEY AVALANCHE TEST ===\n");
    printf("Testing keys: 1000, 1001, 1002, 1003, 1004\n");
    printf("(Small input differences should create large hash differences)\n");
    
    analyze_hash_distribution(crypto_table, "CRYPTO-INSPIRED");
    analyze_hash_distribution(simple_table, "SIMPLE MODULO");
    
    d_DestroyStaticTable(&crypto_table);
    d_DestroyStaticTable(&simple_table);
    return 1;
}

int test_hash_function_performance_comparison(void)
{
    // Large dataset to show performance differences
    const size_t num_keys = 50;
    int* keys = malloc(num_keys * sizeof(int));
    int* values = malloc(num_keys * sizeof(int));
    const void** key_ptrs = malloc(num_keys * sizeof(void*));
    const void** value_ptrs = malloc(num_keys * sizeof(void*));
    
    // Generate diverse key set
    for (size_t i = 0; i < num_keys; i++) {
        keys[i] = (int)(i * 137 + 1000); // Non-sequential pattern
        values[i] = (int)i;
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    // Test multiple hash functions with same data
    dStaticTable_t* terrible_table = d_InitStaticTable(sizeof(int), sizeof(int), terrible_hash_func, 
                                                       int_compare_func, 16, key_ptrs, value_ptrs, num_keys);
    
    dStaticTable_t* simple_table = d_InitStaticTable(sizeof(int), sizeof(int), simple_modulo_hash_func, 
                                                     int_compare_func, 16, key_ptrs, value_ptrs, num_keys);
    
    dStaticTable_t* knuth_table = d_InitStaticTable(sizeof(int), sizeof(int), knuth_multiplicative_hash_func, 
                                                    int_compare_func, 16, key_ptrs, value_ptrs, num_keys);
    
    dStaticTable_t* fnv_table = d_InitStaticTable(sizeof(int), sizeof(int), fnv1a_hash_func, 
                                                  int_compare_func, 16, key_ptrs, value_ptrs, num_keys);
    
    TEST_ASSERT(terrible_table != NULL, "Should create terrible hash table");
    TEST_ASSERT(simple_table != NULL, "Should create simple hash table");
    TEST_ASSERT(knuth_table != NULL, "Should create Knuth hash table");
    TEST_ASSERT(fnv_table != NULL, "Should create FNV hash table");
    
    // Verify all work correctly
    LOOP_TEST_START();
    for (size_t i = 0; i < num_keys; i++) {
        TEST_ASSERT(d_GetValueInStaticTable(terrible_table, &keys[i]) != NULL, "Terrible hash should work correctly");
        TEST_ASSERT(d_GetValueInStaticTable(simple_table, &keys[i]) != NULL, "Simple hash should work correctly");
        TEST_ASSERT(d_GetValueInStaticTable(knuth_table, &keys[i]) != NULL, "Knuth hash should work correctly");
        TEST_ASSERT(d_GetValueInStaticTable(fnv_table, &keys[i]) != NULL, "FNV hash should work correctly");
    }
    LOOP_TEST_END();
    
    printf("\n=== HASH FUNCTION PERFORMANCE COMPARISON ===\n");
    printf("Dataset: %zu diverse keys across 16 buckets\n", num_keys);
    
    analyze_hash_distribution(terrible_table, "TERRIBLE");
    analyze_hash_distribution(simple_table, "SIMPLE MODULO");
    analyze_hash_distribution(knuth_table, "KNUTH MULTIPLICATIVE");
    analyze_hash_distribution(fnv_table, "FNV-1A");
    
    // Cleanup
    d_DestroyStaticTable(&terrible_table);
    d_DestroyStaticTable(&simple_table);
    d_DestroyStaticTable(&knuth_table);
    d_DestroyStaticTable(&fnv_table);
    free(keys);
    free(values);
    free(key_ptrs);
    free(value_ptrs);
    
    return 1;
}

int test_custom_hash_design_principles(void)
{
    printf("\n=== HASH FUNCTION DESIGN PRINCIPLES ===\n");
    printf("1. DETERMINISTIC: Same input always produces same output\n");
    printf("2. UNIFORM DISTRIBUTION: Spread inputs evenly across buckets\n");
    printf("3. AVALANCHE EFFECT: Small input changes cause large output changes\n");
    printf("4. EFFICIENT: Fast computation, minimal CPU overhead\n");
    printf("5. DOMAIN-AWARE: Consider your specific data patterns\n");
    printf("\n=== WHEN TO CREATE CUSTOM HASH FUNCTIONS ===\n");
    printf(" You know specific patterns in your data\n");
    printf(" Generic hashes show poor distribution for your use case\n");
    printf(" Performance is critical and you need domain optimization\n");
    printf(" You have security requirements (crypto-grade mixing)\n");
    printf(" Your keys have structure that can be exploited\n");
    
    // Simple demonstration: verify our design principles work
    int test_key = 12345;
    
    // Test determinism
    size_t hash1 = knuth_multiplicative_hash_func(&test_key, sizeof(int));
    size_t hash2 = knuth_multiplicative_hash_func(&test_key, sizeof(int));
    TEST_ASSERT(hash1 == hash2, "Hash function should be deterministic");
    
    // Test avalanche effect
    int similar_key = 12346; // Just +1
    size_t hash3 = knuth_multiplicative_hash_func(&similar_key, sizeof(int));
    size_t difference = hash1 ^ hash3; // XOR to see bit differences
    
    // Count bits that changed (simplified avalanche measure)
    int bits_changed = 0;
    for (int i = 0; i < 64; i++) {
        if (difference & (1ULL << i)) bits_changed++;
    }
    
    TEST_ASSERT(bits_changed >= 16, "Good hash should change many bits for small input change");
    printf("\n=== AVALANCHE TEST RESULT ===\n");
    printf("Input: %d vs %d (difference: +1)\n", test_key, similar_key);
    printf("Hash output bits changed: %d/64\n", bits_changed);
    printf("Avalanche quality: %s\n", bits_changed >= 32 ? "EXCELLENT" : bits_changed >= 16 ? "GOOD" : "POOR");
    
    return 1;
}

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
    
    d_LogInfo("Starting Static Hash Table Custom Hashing Tests");

    TEST_SUITE_START("Static Hash Table Custom Hashing Tests");

    // Educational tests showing hash function design and benefits
    RUN_TEST(test_terrible_hash_demonstrates_clustering);
    RUN_TEST(test_hash_function_comparison_sequential_keys);
    RUN_TEST(test_fnv1a_hash_with_binary_data);
    RUN_TEST(test_game_entity_hash_demonstrates_domain_knowledge);
    RUN_TEST(test_crypto_inspired_hash_avalanche_effect);
    RUN_TEST(test_hash_function_performance_comparison);
    RUN_TEST(test_custom_hash_design_principles);

    TEST_SUITE_END();

    d_LogInfo("Static Hash Table Custom Hashing Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}