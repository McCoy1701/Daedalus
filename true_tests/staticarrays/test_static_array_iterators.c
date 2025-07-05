// File: true_tests/staticarrays/test_static_array_iterators.c - Advanced array functions and iterators

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// GAME-FOCUSED DATA STRUCTURES FOR TESTING
// =============================================================================

// Player data structure for RPG-style testing
typedef struct {
    int id;
    char name[32];
    int level;
    int health;
    int mana;
    float x, y, z; // Position
} Player_t;

// Weapon data structure for inventory testing
typedef struct {
    int weapon_id;
    char weapon_name[64];
    int damage;
    float durability;
    int rarity; // 1=common, 2=rare, 3=epic, 4=legendary
} Weapon_t;

// Enemy data structure for combat testing
typedef struct {
    int enemy_id;
    char type[32];
    int health;
    int attack_power;
    float aggro_range;
    bool is_boss;
} Enemy_t;

// =============================================================================
// ITERATOR CALLBACK FUNCTIONS FOR TESTING
// =============================================================================

// Count total elements callback
static void count_elements_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)index; (void)element; (void)element_size; // Unused parameters
    int* count = (int*)user_data;
    (*count)++;
}

// Find players by level range callback
typedef struct {
    int min_level;
    int max_level;
    int found_count;
    Player_t* found_players;
    int max_results;
} PlayerLevelSearch_t;

static void find_players_by_level_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)element_size; // Unused parameter
    PlayerLevelSearch_t* search = (PlayerLevelSearch_t*)user_data;
    const Player_t* player = (const Player_t*)element;
    
    d_LogDebugF("🔍 Examining player at index %zu: %s (Level %d)", index, player->name, player->level);
    
    if (player->level >= search->min_level && player->level <= search->max_level) {
        if (search->found_count < search->max_results) {
            search->found_players[search->found_count] = *player;
            search->found_count++;
            d_LogInfoF("✅ Found qualifying player: %s (Level %d, HP:%d, MP:%d) at position (%.1f,%.1f,%.1f)", 
                       player->name, player->level, player->health, player->mana, player->x, player->y, player->z);
        } else {
            d_LogWarningF("⚠️  Player %s matches criteria but search limit reached (%d)", player->name, search->max_results);
        }
    } else {
        d_LogRateLimited(D_LOG_LEVEL_DEBUG, 5, 2.0, "❌ Player doesn't match level criteria");
    }
}

// Calculate total damage of all weapons callback
static void calculate_total_damage_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)element_size; // Unused parameter
    int* total_damage = (int*)user_data;
    const Weapon_t* weapon = (const Weapon_t*)element;
    
    int old_total = *total_damage;
    *total_damage += weapon->damage;
    
    d_LogDebugF("⚔️  Weapon %d at index %zu: %s (Damage: %d, Durability: %.1f%%, Rarity: %d)", 
                weapon->weapon_id, index, weapon->weapon_name, weapon->damage, weapon->durability, weapon->rarity);
    d_LogDebugF("💥 Running total: %d → %d (+%d)", old_total, *total_damage, weapon->damage);
    
    if (weapon->damage > 100) {
        d_LogWarningF("🚨 High-damage weapon detected: %s (%d damage)", weapon->weapon_name, weapon->damage);
    }
}

// Find legendary weapons callback
typedef struct {
    Weapon_t* legendary_weapons;
    int found_count;
    int max_results;
} LegendaryWeaponSearch_t;

static void find_legendary_weapons_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)index; (void)element_size; // Unused parameters
    LegendaryWeaponSearch_t* search = (LegendaryWeaponSearch_t*)user_data;
    const Weapon_t* weapon = (const Weapon_t*)element;
    
    if (weapon->rarity == 4) { // Legendary rarity
        if (search->found_count < search->max_results) {
            search->legendary_weapons[search->found_count] = *weapon;
            search->found_count++;
        }
    }
}

// Calculate average position callback
typedef struct {
    float total_x, total_y, total_z;
    int count;
} PositionStats_t;

static void calculate_average_position_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)index; (void)element_size; // Unused parameters
    PositionStats_t* stats = (PositionStats_t*)user_data;
    const Player_t* player = (const Player_t*)element;
    
    stats->total_x += player->x;
    stats->total_y += player->y;
    stats->total_z += player->z;
    stats->count++;
}

// Find enemies within range callback
typedef struct {
    float player_x, player_y, player_z;
    float detection_range;
    Enemy_t* nearby_enemies;
    int found_count;
    int max_results;
} EnemyProximitySearch_t;

static void find_enemies_in_range_callback(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)index; (void)element_size; // Unused parameters
    EnemyProximitySearch_t* search = (EnemyProximitySearch_t*)user_data;
    const Enemy_t* enemy = (const Enemy_t*)element;
    
    // Note: Simplified 2D distance for this example (ignoring z-coordinate)
    float dx = search->player_x - 0.0f; // Assume enemy at origin for simplicity
    float dy = search->player_y - 0.0f;
    float distance = d_Sqrtf(dx * dx + dy * dy);
    
    if (distance <= search->detection_range && search->found_count < search->max_results) {
        search->nearby_enemies[search->found_count] = *enemy;
        search->found_count++;
    }
}

// =============================================================================
// HELPER FUNCTIONS FOR SETTING UP TEST DATA
// =============================================================================

static void setup_test_players(dStaticArray_t* array) {
    Player_t players[] = {
        {1, "Aragorn", 15, 120, 50, 10.5f, 20.3f, 0.0f},
        {2, "Legolas", 12, 95, 80, 15.2f, 18.7f, 5.1f},
        {3, "Gimli", 14, 140, 30, 8.9f, 22.1f, 0.0f},
        {4, "Gandalf", 50, 200, 150, 12.0f, 25.0f, 2.0f},
        {5, "Boromir", 13, 110, 40, 9.5f, 19.8f, 0.0f}
    };
    
    for (size_t i = 0; i < sizeof(players) / sizeof(Player_t); i++) {
        d_AppendDataToStaticArray(array, &players[i]);
    }
}

static void setup_test_weapons(dStaticArray_t* array) {
    Weapon_t weapons[] = {
        {101, "Iron Sword", 25, 100.0f, 1},
        {102, "Steel Blade", 35, 95.0f, 2},
        {103, "Elven Bow", 40, 98.0f, 2},
        {104, "Dragon Slayer", 80, 85.0f, 3},
        {105, "Excalibur", 120, 100.0f, 4},
        {106, "Mjolnir", 150, 100.0f, 4},
        {107, "Rusty Dagger", 10, 50.0f, 1}
    };
    
    for (size_t i = 0; i < sizeof(weapons) / sizeof(Weapon_t); i++) {
        d_AppendDataToStaticArray(array, &weapons[i]);
    }
}

static void setup_test_enemies(dStaticArray_t* array) {
    Enemy_t enemies[] = {
        {201, "Goblin", 30, 15, 5.0f, false},
        {202, "Orc", 50, 25, 8.0f, false},
        {203, "Troll", 120, 40, 10.0f, false},
        {204, "Dragon", 500, 100, 25.0f, true},
        {205, "Skeleton", 20, 12, 4.0f, false}
    };
    
    for (size_t i = 0; i < sizeof(enemies) / sizeof(Enemy_t); i++) {
        d_AppendDataToStaticArray(array, &enemies[i]);
    }
}

// =============================================================================
// ITERATOR TEST FUNCTIONS
// =============================================================================

int test_basic_element_counting(void)
{
    dLogContext_t* ctx = d_PushLogContext("BasicElementCounting");
    d_LogInfo("🧮 Starting basic element counting iterator test");
    
    // Create static array for integers
    dStaticArray_t* array = d_InitStaticArray(10, sizeof(int));
    TEST_ASSERT(array != NULL, "Should create array successfully");
    d_LogDebugF("Created static array with capacity %zu, element_size %zu", 
                array ? array->capacity : 0, array ? array->element_size : 0);
    
    // Add some test data
    int values[] = {10, 20, 30, 40, 50};
    d_LogInfoF("Adding %d test values to array", (int)(sizeof(values)/sizeof(values[0])));
    
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(d_AppendDataToStaticArray(array, &values[i]) == 0, "Should append values successfully");
        d_LogDebugF("Appended value %d at index %d", values[i], i);
    }
    
    dLogStructured_t* log_entry = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(log_entry, "test_phase", "pre_iteration");
    d_LogStructured_FieldInt(log_entry, "array_count", array->count);
    d_LogStructured_FieldInt(log_entry, "array_capacity", array->capacity);
    d_LogStructured_Commit(log_entry);
    
    // Test iterator counting
    int element_count = 0;
    d_LogInfo("🔄 Starting iteration to count elements");
    TEST_ASSERT(d_IterateStaticArray(array, count_elements_callback, &element_count) == 0, "Should iterate successfully");
    TEST_ASSERT(element_count == 5, "Should count exactly 5 elements");
    
    d_LogInfoF("✅ Element counting completed: found %d elements", element_count);
    
    d_DestroyStaticArray(array);
    d_LogDebug("Destroyed test array");
    d_PopLogContext(ctx);
    return 1; // Success
}

int test_rpg_player_level_search(void)
{
    dLogContext_t* ctx = d_PushLogContext("RPGPlayerLevelSearch");
    d_LogInfo("🎮 Starting RPG player level search iterator test");
    
    // Create player array
    dStaticArray_t* player_array = d_InitStaticArray(20, sizeof(Player_t));
    TEST_ASSERT(player_array != NULL, "Should create player array successfully");
    d_LogDebugF("Created player array with capacity %zu for Player_t structures", player_array->capacity);
    
    // Setup test data
    d_LogInfo("🏰 Setting up RPG party data");
    setup_test_players(player_array);
    d_LogInfoF("Party assembled! %zu players ready for adventure", player_array->count);
    
    // Search for players in level range 12-15
    PlayerLevelSearch_t search = {0};
    search.min_level = 12;
    search.max_level = 15;
    search.max_results = 10;
    search.found_players = malloc(10 * sizeof(Player_t));
    
    dLogStructured_t* search_log = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(search_log, "search_type", "level_range");
    d_LogStructured_FieldInt(search_log, "min_level", search.min_level);
    d_LogStructured_FieldInt(search_log, "max_level", search.max_level);
    d_LogStructured_FieldInt(search_log, "max_results", search.max_results);
    d_LogStructured_Commit(search_log);
    
    d_LogInfoF("🔍 Searching for players in level range %d-%d", search.min_level, search.max_level);
    TEST_ASSERT(d_IterateStaticArray(player_array, find_players_by_level_callback, &search) == 0, "Should iterate and search successfully");
    TEST_ASSERT(search.found_count == 4, "Should find 4 players in level range 12-15");
    
    d_LogInfoF("🎯 Found %d players matching level criteria", search.found_count);
    
    // Verify found players
    bool found_aragorn = false, found_legolas = false, found_gimli = false, found_boromir = false;
    d_LogInfo("✅ Verifying found players:");
    for (int i = 0; i < search.found_count; i++) {
        Player_t* player = &search.found_players[i];
        d_LogInfoF("  Player %d: %s (Level %d, HP:%d, MP:%d)", 
                   player->id, player->name, player->level, player->health, player->mana);
        
        if (strcmp(player->name, "Aragorn") == 0) found_aragorn = true;
        if (strcmp(player->name, "Legolas") == 0) found_legolas = true;
        if (strcmp(player->name, "Gimli") == 0) found_gimli = true;
        if (strcmp(player->name, "Boromir") == 0) found_boromir = true;
    }
    
    TEST_ASSERT(found_aragorn && found_legolas && found_gimli && found_boromir, "Should find all expected players in level range");
    
    d_LogIf(found_aragorn && found_legolas && found_gimli && found_boromir, 
            D_LOG_LEVEL_INFO, "🎉 All expected Fellowship members found in level range!");
    
    free(search.found_players);
    d_DestroyStaticArray(player_array);
    d_LogDebug("Cleaned up player search test resources");
    d_PopLogContext(ctx);
    return 1; // Success
}

int test_weapon_damage_calculation(void)
{
    dLogContext_t* ctx = d_PushLogContext("WeaponDamageCalculation");
    d_LogInfo("⚔️  Starting weapon damage calculation iterator test");
    
    // Create weapon array
    dStaticArray_t* weapon_array = d_InitStaticArray(15, sizeof(Weapon_t));
    TEST_ASSERT(weapon_array != NULL, "Should create weapon array successfully");
    d_LogDebugF("Created weapon array with capacity %zu for Weapon_t structures", weapon_array->capacity);
    
    // Setup test data
    d_LogInfo("🗡️  Setting up weapon inventory");
    setup_test_weapons(weapon_array);
    d_LogInfoF("Arsenal loaded! %zu weapons ready for battle", weapon_array->count);
    
    // Calculate total damage of all weapons
    int total_damage = 0;
    d_LogInfo("🧮 Calculating total damage output of all weapons");
    
    double calculation_start = d_GetTimestamp();
    TEST_ASSERT(d_IterateStaticArray(weapon_array, calculate_total_damage_callback, &total_damage) == 0, "Should iterate and calculate damage successfully");
    double calculation_time = d_GetTimestamp() - calculation_start;
    
    dLogStructured_t* calc_log = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(calc_log, "calculation_type", "total_damage");
    d_LogStructured_FieldInt(calc_log, "total_damage", total_damage);
    d_LogStructured_FieldInt(calc_log, "weapon_count", weapon_array->count);
    d_LogStructured_FieldFloat(calc_log, "calculation_time_ms", calculation_time * 1000.0f, 3);
    d_LogStructured_Commit(calc_log);
    
    // Expected total: 25+35+40+80+120+150+10 = 460
    TEST_ASSERT(total_damage == 460, "Should calculate correct total damage");
    
    d_LogInfoF("💥 Total arsenal damage output: %d points", total_damage);
    d_LogRateLimited(D_LOG_LEVEL_DEBUG, 3, 1.0, "⚡ High-performance damage calculation completed");
    
    if (total_damage > 400) {
        d_LogWarning("🚨 High damage arsenal detected - potential balancing needed!");
    }
    
    d_DestroyStaticArray(weapon_array);
    d_LogDebug("Cleaned up weapon calculation test resources");
    d_PopLogContext(ctx);
    return 1; // Success
}

int test_legendary_weapon_filtering(void)
{
    // Create weapon array
    dStaticArray_t* weapon_array = d_InitStaticArray(15, sizeof(Weapon_t));
    TEST_ASSERT(weapon_array != NULL, "Should create weapon array successfully");
    
    // Setup test data
    setup_test_weapons(weapon_array);
    
    // Search for legendary weapons (rarity = 4)
    LegendaryWeaponSearch_t search = {0};
    search.max_results = 5;
    search.legendary_weapons = malloc(5 * sizeof(Weapon_t));
    
    TEST_ASSERT(d_IterateStaticArray(weapon_array, find_legendary_weapons_callback, &search) == 0, "Should iterate and filter successfully");
    TEST_ASSERT(search.found_count == 2, "Should find exactly 2 legendary weapons");
    
    // Verify found weapons are Excalibur and Mjolnir
    bool found_excalibur = false, found_mjolnir = false;
    for (int i = 0; i < search.found_count; i++) {
        if (strcmp(search.legendary_weapons[i].weapon_name, "Excalibur") == 0) found_excalibur = true;
        if (strcmp(search.legendary_weapons[i].weapon_name, "Mjolnir") == 0) found_mjolnir = true;
    }
    
    TEST_ASSERT(found_excalibur && found_mjolnir, "Should find both legendary weapons");
    
    free(search.legendary_weapons);
    d_DestroyStaticArray(weapon_array);
    return 1; // Success
}

int test_player_position_analytics(void)
{
    // Create player array
    dStaticArray_t* player_array = d_InitStaticArray(20, sizeof(Player_t));
    TEST_ASSERT(player_array != NULL, "Should create player array successfully");
    
    // Setup test data
    setup_test_players(player_array);
    
    // Calculate average position
    PositionStats_t stats = {0};
    TEST_ASSERT(d_IterateStaticArray(player_array, calculate_average_position_callback, &stats) == 0, "Should iterate and calculate stats successfully");
    
    TEST_ASSERT(stats.count == 5, "Should process all 5 players");
    
    float avg_x = stats.total_x / stats.count;
    float avg_y = stats.total_y / stats.count;
    float avg_z = stats.total_z / stats.count;
    
    // Expected averages: x=(10.5+15.2+8.9+12.0+9.5)/5=11.22, y=(20.3+18.7+22.1+25.0+19.8)/5=21.18, z=(0+5.1+0+2.0+0)/5=1.42
    TEST_ASSERT(fabsf(avg_x - 11.22f) < 0.1f, "Should calculate correct average X position");
    TEST_ASSERT(fabsf(avg_y - 21.18f) < 0.1f, "Should calculate correct average Y position");
    TEST_ASSERT(fabsf(avg_z - 1.42f) < 0.1f, "Should calculate correct average Z position");
    
    d_DestroyStaticArray(player_array);
    return 1; // Success
}

int test_enemy_proximity_detection(void)
{
    // Create enemy array
    dStaticArray_t* enemy_array = d_InitStaticArray(20, sizeof(Enemy_t));
    TEST_ASSERT(enemy_array != NULL, "Should create enemy array successfully");
    
    // Setup test data
    setup_test_enemies(enemy_array);
    
    // Search for enemies within detection range
    EnemyProximitySearch_t search = {0};
    search.player_x = 0.0f;
    search.player_y = 0.0f;
    search.player_z = 0.0f;
    search.detection_range = 10.0f;
    search.max_results = 10;
    search.nearby_enemies = malloc(10 * sizeof(Enemy_t));
    
    TEST_ASSERT(d_IterateStaticArray(enemy_array, find_enemies_in_range_callback, &search) == 0, "Should iterate and detect enemies successfully");
    
    // All enemies should be found since they're all at origin (0,0,0) and within range
    TEST_ASSERT(search.found_count == 5, "Should find all 5 enemies within range");
    
    free(search.nearby_enemies);
    d_DestroyStaticArray(enemy_array);
    return 1; // Success
}

int test_empty_array_iteration(void)
{
    // Create empty array
    dStaticArray_t* array = d_InitStaticArray(10, sizeof(int));
    TEST_ASSERT(array != NULL, "Should create empty array successfully");
    
    // Test iterator on empty array
    int element_count = 0;
    TEST_ASSERT(d_IterateStaticArray(array, count_elements_callback, &element_count) == 0, "Should iterate empty array successfully");
    TEST_ASSERT(element_count == 0, "Should count zero elements in empty array");
    
    d_DestroyStaticArray(array);
    return 1; // Success
}

int test_iterator_error_handling(void)
{
    dLogContext_t* ctx = d_PushLogContext("IteratorErrorHandling");
    d_LogInfo("🚨 Starting iterator error handling test");
    
    dStaticArray_t* array = d_InitStaticArray(5, sizeof(int));
    TEST_ASSERT(array != NULL, "Should create array successfully");
    d_LogDebug("Created array for error handling validation");
    
    // Test with NULL array
    int count = 0;
    d_LogInfo("Testing NULL array parameter handling");
    TEST_ASSERT(d_IterateStaticArray(NULL, count_elements_callback, &count) == 1, "Should fail with NULL array");
    d_LogDebug("✅ NULL array parameter correctly rejected");
    
    // Test with NULL callback
    d_LogInfo("Testing NULL callback parameter handling");
    TEST_ASSERT(d_IterateStaticArray(array, NULL, &count) == 1, "Should fail with NULL callback");
    d_LogDebug("✅ NULL callback parameter correctly rejected");
    
    dLogStructured_t* error_log = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(error_log, "test_type", "error_handling");
    d_LogStructured_Field(error_log, "null_array_test", "passed");
    d_LogStructured_Field(error_log, "null_callback_test", "passed");
    d_LogStructured_Field(error_log, "validation_status", "complete");
    d_LogStructured_Commit(error_log);
    
    d_DestroyStaticArray(array);
    d_LogInfo("🎯 Iterator error handling validation completed successfully");
    d_PopLogContext(ctx);
    return 1; // Success
}

// =============================================================================
// MAIN TEST EXECUTION
// =============================================================================

int main(void)
{
    // Initialize Daedalus logging system with enhanced configuration
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
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
    
    dLogContext_t* main_ctx = d_PushLogContext("StaticArrayIteratorTests");
    d_LogInfo("🚀 Starting Static Array Iterator Tests Suite");
    d_LogInfo("=====================================");
    
    double suite_start_time = d_GetTimestamp();
    TEST_SUITE_START("Static Array Iterator Tests");
    
    // Basic functionality tests
    d_LogInfo("📋 Running basic functionality tests");
    RUN_TEST(test_basic_element_counting);
    RUN_TEST(test_empty_array_iteration);
    RUN_TEST(test_iterator_error_handling);
    
    // Game-focused iterator tests
    d_LogInfo("\n🎮 Running game-focused iterator tests");
    RUN_TEST(test_rpg_player_level_search);
    RUN_TEST(test_weapon_damage_calculation);
    RUN_TEST(test_legendary_weapon_filtering);
    RUN_TEST(test_player_position_analytics);
    RUN_TEST(test_enemy_proximity_detection);
    
    double suite_end_time = d_GetTimestamp();
    double total_test_time = suite_end_time - suite_start_time;
    TEST_SUITE_END();
    
    // Final results with comprehensive logging
    d_LogInfo("\n=====================================");
    d_LogInfo("📊 Static Array Iterator Test Results:");
    
    dLogStructured_t* results_log = d_LogStructured(D_LOG_LEVEL_INFO);
    d_LogStructured_Field(results_log, "test_suite", "static_array_iterators");
    d_LogStructured_FieldInt(results_log, "total_tests", total_tests);
    d_LogStructured_FieldInt(results_log, "tests_passed", tests_passed);
    d_LogStructured_FieldInt(results_log, "tests_failed", tests_failed);
    d_LogStructured_FieldFloat(results_log, "success_rate", (float)tests_passed / total_tests * 100.0f, 2);
    d_LogStructured_FieldFloat(results_log, "total_time_seconds", total_test_time, 6);
    d_LogStructured_FieldFloat(results_log, "avg_time_per_test", total_test_time / total_tests, 6);
    d_LogStructured_Commit(results_log);
    
    d_LogInfoF("Total tests: %d", total_tests);
    d_LogInfoF("Passed: %d", tests_passed);
    d_LogInfoF("Failed: %d", tests_failed);
    d_LogInfoF("Success rate: %.2f%%", (float)tests_passed / total_tests * 100.0f);
    d_LogInfoF("Total execution time: %.6f seconds", total_test_time);
    
    if (tests_failed == 0) {
        d_LogInfo("🎉 All tests passed! Static array iterators are functioning perfectly.");
    } else {
        d_LogErrorF("❌ %d test(s) failed! Review the logs above for details.", tests_failed);
    }
    
    // Get and log performance stats
    dLogStats_t stats = {0};
    d_GetLogStats(logger, &stats);
    d_LogInfoF("📈 Logging stats: %zu debug, %zu info, %zu warnings, %zu errors", 
               stats.logs_by_level[D_LOG_LEVEL_DEBUG], 
               stats.logs_by_level[D_LOG_LEVEL_INFO],
               stats.logs_by_level[D_LOG_LEVEL_WARNING], 
               stats.logs_by_level[D_LOG_LEVEL_ERROR]);
    
    d_PopLogContext(main_ctx);
    d_DestroyLogger(logger);
    
    return (tests_failed == 0) ? 0 : 1;
}