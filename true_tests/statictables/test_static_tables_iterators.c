// File: true_tests/statictables/test_static_tables_iterators.c - Game-focused iterator tests for static tables

#include "tests.h"
#include "Daedalus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// GAME DATA STRUCTURES
// =============================================================================

// RPG Entity Stats
typedef struct {
    int hp;
    int mp;
    int strength;
    int dexterity;
    int intelligence;
} EntityStats_t;

// Item data for inventory
typedef struct {
    int item_id;
    int quantity;
    float weight;
    int rarity; // 1=common, 5=legendary
} InventoryItem_t;

// Quest data
typedef struct {
    int quest_id;
    int progress;
    int max_progress;
    bool completed;
} QuestData_t;

// Spell data
typedef struct {
    int spell_id;
    int mana_cost;
    int damage;
    float cooldown;
} SpellData_t;

// =============================================================================
// HASH FUNCTIONS FOR GAME DATA
// =============================================================================

static size_t entity_id_hash(const void* key, size_t key_size) {
    (void)key_size;
    int* entity_id = (int*)key;
    return (size_t)(*entity_id * 2654435761U);
}

static int int_compare_func(const void* key1, const void* key2, size_t key_size) {
    (void)key_size;
    int* k1 = (int*)key1;
    int* k2 = (int*)key2;
    return (*k1 == *k2) ? 0 : 1;
}

// =============================================================================
// STAT BOOST ITERATORS
// =============================================================================

// Stat boost context
typedef struct {
    int stat_type; // 0=hp, 1=mp, 2=str, 3=dex, 4=int
    int boost_amount;
    int entities_boosted;
    int total_boost_applied;
} StatBoostContext_t;

static void stat_boost_iterator(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    StatBoostContext_t* ctx = (StatBoostContext_t*)user_data;
    int entity_id = *(int*)key;
    EntityStats_t* stats = (EntityStats_t*)value;
    
    printf("🎮 Boosting Entity %d: ", entity_id);
    
    switch (ctx->stat_type) {
        case 0: // HP
            stats->hp += ctx->boost_amount;
            printf("HP %d → %d", stats->hp - ctx->boost_amount, stats->hp);
            break;
        case 1: // MP
            stats->mp += ctx->boost_amount;
            printf("MP %d → %d", stats->mp - ctx->boost_amount, stats->mp);
            break;
        case 2: // Strength
            stats->strength += ctx->boost_amount;
            printf("STR %d → %d", stats->strength - ctx->boost_amount, stats->strength);
            break;
        case 3: // Dexterity
            stats->dexterity += ctx->boost_amount;
            printf("DEX %d → %d", stats->dexterity - ctx->boost_amount, stats->dexterity);
            break;
        case 4: // Intelligence
            stats->intelligence += ctx->boost_amount;
            printf("INT %d → %d", stats->intelligence - ctx->boost_amount, stats->intelligence);
            break;
    }
    
    printf(" ✨\n");
    ctx->entities_boosted++;
    ctx->total_boost_applied += ctx->boost_amount;
}

// Level up iterator (boosts all stats)
typedef struct {
    int level_gain;
    int entities_leveled;
    int stat_points_distributed;
} LevelUpContext_t;

static void level_up_iterator(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    LevelUpContext_t* ctx = (LevelUpContext_t*)user_data;
    int entity_id = *(int*)key;
    EntityStats_t* stats = (EntityStats_t*)value;
    
    // Each level gives +5 HP, +3 MP, +2 to primary stats
    int hp_gain = ctx->level_gain * 5;
    int mp_gain = ctx->level_gain * 3;
    int stat_gain = ctx->level_gain * 2;
    
    printf("🆙 Entity %d LEVEL UP x%d!\n", entity_id, ctx->level_gain);
    printf("   HP: %d → %d (+%d)\n", stats->hp, stats->hp + hp_gain, hp_gain);
    printf("   MP: %d → %d (+%d)\n", stats->mp, stats->mp + mp_gain, mp_gain);
    printf("   STR: %d → %d (+%d)\n", stats->strength, stats->strength + stat_gain, stat_gain);
    printf("   DEX: %d → %d (+%d)\n", stats->dexterity, stats->dexterity + stat_gain, stat_gain);
    printf("   INT: %d → %d (+%d)\n", stats->intelligence, stats->intelligence + stat_gain, stat_gain);
    
    stats->hp += hp_gain;
    stats->mp += mp_gain;
    stats->strength += stat_gain;
    stats->dexterity += stat_gain;
    stats->intelligence += stat_gain;
    
    ctx->entities_leveled++;
    ctx->stat_points_distributed += hp_gain + mp_gain + (stat_gain * 3);
}

// =============================================================================
// INVENTORY MANAGEMENT ITERATORS
// =============================================================================

// Inventory value calculator
typedef struct {
    float total_value;
    float total_weight;
    int items_counted;
    int legendary_items;
} InventoryValueContext_t;

static void inventory_value_iterator(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    InventoryValueContext_t* ctx = (InventoryValueContext_t*)user_data;
    int item_id = *(int*)key;
    InventoryItem_t* item = (InventoryItem_t*)value;
    
    // Calculate item value based on rarity
    float base_value = item->rarity * item->rarity * 100.0f; // Exponential rarity value
    float total_item_value = base_value * item->quantity;
    float total_item_weight = item->weight * item->quantity;
    
    printf("📦 Item %d: Qty=%d, Rarity=%d, Value=%.2f, Weight=%.2f\n", 
           item_id, item->quantity, item->rarity, total_item_value, total_item_weight);
    
    ctx->total_value += total_item_value;
    ctx->total_weight += total_item_weight;
    ctx->items_counted++;
    
    if (item->rarity >= 5) {
        ctx->legendary_items++;
    }
}

// Item rarity filter iterator
typedef struct {
    int min_rarity;
    int items_found;
    float filtered_value;
} RarityFilterContext_t;

static void rarity_filter_iterator(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    RarityFilterContext_t* ctx = (RarityFilterContext_t*)user_data;
    int item_id = *(int*)key;
    InventoryItem_t* item = (InventoryItem_t*)value;
    
    if (item->rarity >= ctx->min_rarity) {
        float item_value = item->rarity * item->rarity * 100.0f * item->quantity;
        printf("💎 RARE ITEM %d: Rarity=%d, Qty=%d, Value=%.2f\n", 
               item_id, item->rarity, item->quantity, item_value);
        
        ctx->items_found++;
        ctx->filtered_value += item_value;
    }
}

// =============================================================================
// QUEST SYSTEM ITERATORS
// =============================================================================

// Quest progress tracker
typedef struct {
    int completed_quests;
    int in_progress_quests;
    int total_progress_points;
    float completion_percentage;
} QuestProgressContext_t;

static void quest_progress_iterator(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    QuestProgressContext_t* ctx = (QuestProgressContext_t*)user_data;
    int quest_id = *(int*)key;
    QuestData_t* quest = (QuestData_t*)value;
    
    float quest_completion = (float)quest->progress / (float)quest->max_progress * 100.0f;
    
    if (quest->completed) {
        printf("✅ Quest %d: COMPLETED!\n", quest_id);
        ctx->completed_quests++;
    } else {
        printf("📋 Quest %d: %d/%d (%.1f%%)\n", 
               quest_id, quest->progress, quest->max_progress, quest_completion);
        ctx->in_progress_quests++;
    }
    
    ctx->total_progress_points += quest->progress;
    ctx->completion_percentage += quest_completion;
}

// Quest completion booster (adds progress to unfinished quests)
typedef struct {
    int progress_boost;
    int quests_boosted;
    int quests_completed;
} QuestBoostContext_t;

static void quest_boost_iterator(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    QuestBoostContext_t* ctx = (QuestBoostContext_t*)user_data;
    int quest_id = *(int*)key;
    QuestData_t* quest = (QuestData_t*)value;
    
    if (!quest->completed && quest->progress < quest->max_progress) {
        int old_progress = quest->progress;
        quest->progress += ctx->progress_boost;
        
        if (quest->progress >= quest->max_progress) {
            quest->progress = quest->max_progress;
            quest->completed = true;
            printf("🎉 Quest %d: %d → %d COMPLETED!\n", quest_id, old_progress, quest->progress);
            ctx->quests_completed++;
        } else {
            printf("⚡ Quest %d: %d → %d (+%d progress)\n", 
                   quest_id, old_progress, quest->progress, ctx->progress_boost);
        }
        
        ctx->quests_boosted++;
    }
}

// =============================================================================
// SPELL SYSTEM ITERATORS
// =============================================================================

// Mana cost analyzer
typedef struct {
    int total_mana_cost;
    int spell_count;
    int high_cost_spells; // >50 mana
    float average_damage;
} ManaAnalysisContext_t;

static void mana_analysis_iterator(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    ManaAnalysisContext_t* ctx = (ManaAnalysisContext_t*)user_data;
    int spell_id = *(int*)key;
    SpellData_t* spell = (SpellData_t*)value;
    
    float efficiency = (float)spell->damage / (float)spell->mana_cost;
    
    printf("🔮 Spell %d: Cost=%d, Damage=%d, Cooldown=%.1fs, Efficiency=%.2f\n", 
           spell_id, spell->mana_cost, spell->damage, spell->cooldown, efficiency);
    
    ctx->total_mana_cost += spell->mana_cost;
    ctx->average_damage += spell->damage;
    ctx->spell_count++;
    
    if (spell->mana_cost > 50) {
        ctx->high_cost_spells++;
    }
}

// Spell rebalancing iterator (adjusts costs and damage)
typedef struct {
    float cost_multiplier;
    float damage_multiplier;
    int spells_rebalanced;
    float total_power_change;
} SpellRebalanceContext_t;

static void spell_rebalance_iterator(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    SpellRebalanceContext_t* ctx = (SpellRebalanceContext_t*)user_data;
    int spell_id = *(int*)key;
    SpellData_t* spell = (SpellData_t*)value;
    
    int old_cost = spell->mana_cost;
    int old_damage = spell->damage;
    
    spell->mana_cost = (int)(spell->mana_cost * ctx->cost_multiplier);
    spell->damage = (int)(spell->damage * ctx->damage_multiplier);
    
    float old_power = (float)old_damage / (float)old_cost;
    float new_power = (float)spell->damage / (float)spell->mana_cost;
    
    printf("⚖️  Spell %d: Cost %d→%d, Damage %d→%d, Power %.2f→%.2f\n", 
           spell_id, old_cost, spell->mana_cost, old_damage, spell->damage, old_power, new_power);
    
    ctx->spells_rebalanced++;
    ctx->total_power_change += (new_power - old_power);
}

// =============================================================================
// DEBUG-HUNTING ITERATORS
// =============================================================================

// Memory corruption detector iterator
typedef struct {
    size_t entries_checked;
    size_t corruption_found;
    size_t null_key_count;
    size_t null_value_count;
    size_t boundary_violations;
} MemoryCorruptionContext_t;

static void memory_corruption_detector(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    MemoryCorruptionContext_t* ctx = (MemoryCorruptionContext_t*)user_data;
    ctx->entries_checked++;
    
    // Check for NULL pointers
    if (!key) {
        ctx->null_key_count++;
        printf("🚨 DEBUG: NULL key detected at entry %zu\n", ctx->entries_checked);
        return;
    }
    
    if (!value) {
        ctx->null_value_count++;
        printf("🚨 DEBUG: NULL value detected at entry %zu\n", ctx->entries_checked);
        return;
    }
    
    // Check for reasonable key/value sizes
    if (key_size == 0 || key_size > 1024) {
        ctx->boundary_violations++;
        printf("🚨 DEBUG: Suspicious key size %zu at entry %zu\n", key_size, ctx->entries_checked);
    }
    
    if (value_size == 0 || value_size > 1024) {
        ctx->boundary_violations++;
        printf("🚨 DEBUG: Suspicious value size %zu at entry %zu\n", value_size, ctx->entries_checked);
    }
    
    // Test memory accessibility (basic check)
    int test_key = *(int*)key;
    int test_value = *(int*)value;
    
    // Look for patterns that might indicate corruption
    if (test_key == 0xDEADBEEF || test_value == 0xDEADBEEF) {
        ctx->corruption_found++;
        printf("🚨 DEBUG: Suspicious pattern 0xDEADBEEF found at entry %zu\n", ctx->entries_checked);
    }
    
    printf("🔍 Entry %zu: Key=%d, Value=%d (sizes: %zu/%zu)\n", 
           ctx->entries_checked, test_key, test_value, key_size, value_size);
}

// Edge case key pattern detector
typedef struct {
    int min_key;
    int max_key;
    int sequential_count;
    int duplicate_count;
    int negative_count;
    int zero_count;
    int* previous_key;
} KeyPatternContext_t;

static void key_pattern_detector(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value; (void)value_size;
    KeyPatternContext_t* ctx = (KeyPatternContext_t*)user_data;
    int current_key = *(int*)key;
    
    // Track min/max
    if (ctx->previous_key == NULL) {
        ctx->min_key = ctx->max_key = current_key;
    } else {
        if (current_key < ctx->min_key) ctx->min_key = current_key;
        if (current_key > ctx->max_key) ctx->max_key = current_key;
        
        // Check for sequential pattern
        if (current_key == *ctx->previous_key + 1) {
            ctx->sequential_count++;
        }
        
        // Check for duplicates (shouldn't happen in static table!)
        if (current_key == *ctx->previous_key) {
            ctx->duplicate_count++;
            printf("🚨 DEBUG: DUPLICATE KEY DETECTED: %d\n", current_key);
        }
    }
    
    // Check for edge case values
    if (current_key < 0) ctx->negative_count++;
    if (current_key == 0) ctx->zero_count++;
    
    printf("🔑 Key Analysis: %d (neg:%d, zero:%d, seq:%d, dup:%d)\n", 
           current_key, ctx->negative_count, ctx->zero_count, 
           ctx->sequential_count, ctx->duplicate_count);
    
    // Store current key for next iteration
    static int stored_key;
    stored_key = current_key;
    ctx->previous_key = &stored_key;
}

// Value integrity checker
typedef struct {
    size_t total_entries;
    size_t checksum_mismatches;
    size_t value_overflow_count;
    size_t negative_value_count;
    uint32_t running_checksum;
} ValueIntegrityContext_t;

static void value_integrity_checker(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    ValueIntegrityContext_t* ctx = (ValueIntegrityContext_t*)user_data;
    int key_val = *(int*)key;
    int value_val = *(int*)value;
    
    ctx->total_entries++;
    
    // Create expected checksum (simple: key * 10)
    int expected_value = key_val * 10;
    if (value_val != expected_value) {
        ctx->checksum_mismatches++;
        printf("🚨 DEBUG: Value mismatch! Key=%d, Expected=%d, Got=%d\n", 
               key_val, expected_value, value_val);
    }
    
    // Check for overflow conditions
    if (value_val > 100000) {
        ctx->value_overflow_count++;
        printf("🚨 DEBUG: Possible overflow! Value=%d\n", value_val);
    }
    
    // Check for negative values in what should be positive data
    if (value_val < 0) {
        ctx->negative_value_count++;
        printf("🚨 DEBUG: Negative value detected: %d\n", value_val);
    }
    
    // Update running checksum
    ctx->running_checksum ^= (uint32_t)(key_val + value_val);
    
    printf("✅ Integrity: Key=%d, Value=%d, Checksum=0x%X\n", 
           key_val, value_val, ctx->running_checksum);
}

// Performance stress iterator
typedef struct {
    size_t iterations;
    size_t memory_accesses;
    size_t computation_cycles;
    clock_t start_time;
    double max_iteration_time;
    double total_time;
} PerformanceStressContext_t;

static void performance_stress_iterator(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    PerformanceStressContext_t* ctx = (PerformanceStressContext_t*)user_data;
    
    clock_t iter_start = clock();
    
    int key_val = *(int*)key;
    int value_val = *(int*)value;
    
    // Simulate intensive operations that might reveal performance bugs
    ctx->memory_accesses += 2; // Key and value access
    
    // Intensive computation to stress the system
    volatile int computation_result = 0;
    for (int i = 0; i < 1000; i++) {
        computation_result += (key_val * i) ^ (value_val * i);
        ctx->computation_cycles++;
    }
    
    // Simulate memory pressure
    char* temp_buffer = malloc(1024);
    if (temp_buffer) {
        memset(temp_buffer, (char)(key_val % 256), 1024);
        ctx->memory_accesses += 1024;
        free(temp_buffer);
    }
    
    clock_t iter_end = clock();
    double iteration_time = ((double)(iter_end - iter_start)) / CLOCKS_PER_SEC;
    
    if (iteration_time > ctx->max_iteration_time) {
        ctx->max_iteration_time = iteration_time;
        printf("🚨 DEBUG: Slow iteration detected! Time=%.6fs for key=%d\n", 
               iteration_time, key_val);
    }
    
    ctx->total_time += iteration_time;
    ctx->iterations++;
    
    printf("⚡ Perf: Iter=%zu, Key=%d, Time=%.6fs, MemAccess=%zu\n", 
           ctx->iterations, key_val, iteration_time, ctx->memory_accesses);
}

// State corruption detector
typedef struct {
    int previous_key;
    int previous_value;
    bool first_iteration;
    size_t state_changes;
    size_t unexpected_transitions;
    size_t callback_count;
} StateCorruptionContext_t;

static void state_corruption_detector(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    StateCorruptionContext_t* ctx = (StateCorruptionContext_t*)user_data;
    
    int current_key = *(int*)key;
    int current_value = *(int*)value;
    
    ctx->callback_count++;
    
    if (!ctx->first_iteration) {
        // Check for unexpected state changes
        if (current_key < ctx->previous_key) {
            ctx->unexpected_transitions++;
            printf("🚨 DEBUG: Key decreased! %d -> %d (iteration %zu)\n", 
                   ctx->previous_key, current_key, ctx->callback_count);
        }
        
        // Check for impossible value transitions
        int value_diff = abs(current_value - ctx->previous_value);
        if (value_diff > 1000) {
            ctx->unexpected_transitions++;
            printf("🚨 DEBUG: Large value jump! %d -> %d (diff=%d)\n", 
                   ctx->previous_value, current_value, value_diff);
        }
        
        ctx->state_changes++;
    } else {
        ctx->first_iteration = false;
    }
    
    // Modify context state to test for corruption
    ctx->previous_key = current_key;
    ctx->previous_value = current_value;
    
    // Test context pointer stability
    static void* last_context = NULL;
    if (last_context != NULL && last_context != user_data) {
        printf("🚨 DEBUG: Context pointer changed! %p -> %p\n", last_context, user_data);
    }
    last_context = user_data;
    
    printf("🔄 State: Iter=%zu, Key=%d->%d, Value=%d->%d, Changes=%zu\n", 
           ctx->callback_count, ctx->previous_key, current_key, 
           ctx->previous_value, current_value, ctx->state_changes);
}

// =============================================================================
// TESTS
// =============================================================================

int test_entity_stat_boost_iterator(void)
{
    // Create entity stats table
    int entity_ids[] = {101, 102, 103, 104}; // Player, NPC1, NPC2, Boss
    EntityStats_t stats[] = {
        {100, 50, 15, 12, 10}, // Player
        {80, 30, 12, 15, 8},   // NPC1  
        {90, 40, 18, 10, 14},  // NPC2
        {200, 100, 25, 20, 22} // Boss
    };
    
    const void* key_ptrs[4];
    const void* value_ptrs[4];
    
    for (int i = 0; i < 4; i++) {
        key_ptrs[i] = &entity_ids[i];
        value_ptrs[i] = &stats[i];
    }
    
    dStaticTable_t* entity_table = d_InitStaticTable(sizeof(int), sizeof(EntityStats_t), entity_id_hash, 
                                                     int_compare_func, 8, key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(entity_table != NULL, "Should create entity stats table");
    
    printf("\n=== 💪 STRENGTH BOOST EVENT ===\n");
    
    // Boost strength for all entities
    StatBoostContext_t str_boost = {2, 10, 0, 0}; // stat_type=2 (strength), boost=10
    TEST_ASSERT(d_IterateStaticTable(entity_table, stat_boost_iterator, &str_boost) == 0, "Should boost strength");
    TEST_ASSERT(str_boost.entities_boosted == 4, "Should boost all 4 entities");
    TEST_ASSERT(str_boost.total_boost_applied == 40, "Should apply 40 total boost points");
    
    // Verify boosts were applied
    EntityStats_t* player_stats = (EntityStats_t*)d_GetValueInStaticTable(entity_table, &entity_ids[0]);
    TEST_ASSERT(player_stats->strength == 25, "Player strength should be boosted to 25");
    
    printf("\n=== 🧠 INTELLIGENCE BOOST EVENT ===\n");
    
    // Boost intelligence
    StatBoostContext_t int_boost = {4, 5, 0, 0}; // stat_type=4 (intelligence), boost=5
    TEST_ASSERT(d_IterateStaticTable(entity_table, stat_boost_iterator, &int_boost) == 0, "Should boost intelligence");
    TEST_ASSERT(int_boost.entities_boosted == 4, "Should boost all 4 entities");
    
    d_DestroyStaticTable(&entity_table);
    return 1;
}

int test_level_up_iterator(void)
{
    // Create party of adventurers
    int party_ids[] = {201, 202, 203}; // Warrior, Mage, Rogue
    EntityStats_t party[] = {
        {120, 20, 20, 10, 8},  // Warrior
        {60, 80, 8, 12, 25},   // Mage
        {80, 40, 12, 22, 15}   // Rogue
    };
    
    const void* key_ptrs[3];
    const void* value_ptrs[3];
    
    for (int i = 0; i < 3; i++) {
        key_ptrs[i] = &party_ids[i];
        value_ptrs[i] = &party[i];
    }
    
    dStaticTable_t* party_table = d_InitStaticTable(sizeof(int), sizeof(EntityStats_t), entity_id_hash, 
                                                    int_compare_func, 8, key_ptrs, value_ptrs, 3);
    
    TEST_ASSERT(party_table != NULL, "Should create party stats table");
    
    printf("\n=== 🎊 PARTY LEVEL UP EVENT! ===\n");
    
    // Level up entire party by 2 levels
    LevelUpContext_t level_up = {2, 0, 0};
    TEST_ASSERT(d_IterateStaticTable(party_table, level_up_iterator, &level_up) == 0, "Should level up party");
    TEST_ASSERT(level_up.entities_leveled == 3, "Should level up all 3 party members");
    TEST_ASSERT(level_up.stat_points_distributed > 0, "Should distribute stat points");
    
    // Verify level ups
    EntityStats_t* warrior = (EntityStats_t*)d_GetValueInStaticTable(party_table, &party_ids[0]);
    TEST_ASSERT(warrior->hp == 130, "Warrior HP should increase by 10 (2 levels × 5)");
    TEST_ASSERT(warrior->strength == 24, "Warrior strength should increase by 4 (2 levels × 2)");
    
    printf("\n📊 Total stat points distributed: %d\n", level_up.stat_points_distributed);
    
    d_DestroyStaticTable(&party_table);
    return 1;
}

int test_inventory_management_iterators(void)
{
    // Create inventory with various items
    int item_ids[] = {1001, 1002, 1003, 1004, 1005};
    InventoryItem_t items[] = {
        {1001, 10, 0.5f, 1},  // Common healing potions
        {1002, 1, 15.0f, 5},  // Legendary sword
        {1003, 5, 2.0f, 3},   // Rare gems
        {1004, 20, 0.1f, 1},  // Common arrows
        {1005, 1, 25.0f, 4}   // Epic armor
    };
    
    const void* key_ptrs[5];
    const void* value_ptrs[5];
    
    for (int i = 0; i < 5; i++) {
        key_ptrs[i] = &item_ids[i];
        value_ptrs[i] = &items[i];
    }
    
    dStaticTable_t* inventory_table = d_InitStaticTable(sizeof(int), sizeof(InventoryItem_t), entity_id_hash, 
                                                        int_compare_func, 8, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(inventory_table != NULL, "Should create inventory table");
    
    printf("\n=== 💰 INVENTORY VALUE ANALYSIS ===\n");
    
    // Calculate total inventory value
    InventoryValueContext_t value_ctx = {0.0f, 0.0f, 0, 0};
    TEST_ASSERT(d_IterateStaticTable(inventory_table, inventory_value_iterator, &value_ctx) == 0, "Should analyze inventory value");
    TEST_ASSERT(value_ctx.items_counted == 5, "Should count all 5 items");
    TEST_ASSERT(value_ctx.legendary_items == 1, "Should find 1 legendary item");
    
    printf("\n💎 INVENTORY SUMMARY:\n");
    printf("Total Value: %.2f gold\n", value_ctx.total_value);
    printf("Total Weight: %.2f kg\n", value_ctx.total_weight);
    printf("Legendary Items: %d\n", value_ctx.legendary_items);
    
    printf("\n=== 🔍 RARE ITEM FILTER (Rarity ≥ 4) ===\n");
    
    // Filter for high-rarity items
    RarityFilterContext_t filter_ctx = {4, 0, 0.0f};
    TEST_ASSERT(d_IterateStaticTable(inventory_table, rarity_filter_iterator, &filter_ctx) == 0, "Should filter rare items");
    TEST_ASSERT(filter_ctx.items_found >= 1, "Should find at least 1 rare item");
    
    printf("\n🎯 Found %d rare items worth %.2f gold!\n", filter_ctx.items_found, filter_ctx.filtered_value);
    
    d_DestroyStaticTable(&inventory_table);
    return 1;
}

int test_quest_system_iterators(void)
{
    // Create quest log
    int quest_ids[] = {5001, 5002, 5003, 5004};
    QuestData_t quests[] = {
        {5001, 10, 10, true},   // Completed quest
        {5002, 7, 15, false},   // In progress
        {5003, 3, 8, false},    // In progress
        {5004, 20, 20, true}    // Completed quest
    };
    
    const void* key_ptrs[4];
    const void* value_ptrs[4];
    
    for (int i = 0; i < 4; i++) {
        key_ptrs[i] = &quest_ids[i];
        value_ptrs[i] = &quests[i];
    }
    
    dStaticTable_t* quest_table = d_InitStaticTable(sizeof(int), sizeof(QuestData_t), entity_id_hash, 
                                                    int_compare_func, 8, key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(quest_table != NULL, "Should create quest table");
    
    printf("\n=== 📜 QUEST LOG ANALYSIS ===\n");
    
    // Analyze quest progress
    QuestProgressContext_t progress_ctx = {0, 0, 0, 0.0f};
    TEST_ASSERT(d_IterateStaticTable(quest_table, quest_progress_iterator, &progress_ctx) == 0, "Should analyze quest progress");
    TEST_ASSERT(progress_ctx.completed_quests == 2, "Should find 2 completed quests");
    TEST_ASSERT(progress_ctx.in_progress_quests == 2, "Should find 2 in-progress quests");
    
    printf("\n📊 QUEST SUMMARY:\n");
    printf("Completed: %d | In Progress: %d\n", progress_ctx.completed_quests, progress_ctx.in_progress_quests);
    printf("Total Progress Points: %d\n", progress_ctx.total_progress_points);
    
    printf("\n=== ⚡ QUEST BOOST EVENT ===\n");
    
    // Boost quest progress
    QuestBoostContext_t boost_ctx = {5, 0, 0}; // +5 progress to all incomplete quests
    TEST_ASSERT(d_IterateStaticTable(quest_table, quest_boost_iterator, &boost_ctx) == 0, "Should boost quest progress");
    TEST_ASSERT(boost_ctx.quests_boosted >= 1, "Should boost at least 1 quest");
    
    printf("\n🎯 Boosted %d quests, completed %d quests!\n", boost_ctx.quests_boosted, boost_ctx.quests_completed);
    
    d_DestroyStaticTable(&quest_table);
    return 1;
}

int test_spell_system_iterators(void)
{
    // Create spell book
    int spell_ids[] = {9001, 9002, 9003, 9004};
    SpellData_t spells[] = {
        {9001, 20, 50, 2.0f},   // Fireball
        {9002, 80, 200, 10.0f}, // Meteor
        {9003, 15, 30, 1.5f},   // Magic Missile
        {9004, 100, 300, 15.0f} // Apocalypse
    };
    
    const void* key_ptrs[4];
    const void* value_ptrs[4];
    
    for (int i = 0; i < 4; i++) {
        key_ptrs[i] = &spell_ids[i];
        value_ptrs[i] = &spells[i];
    }
    
    dStaticTable_t* spell_table = d_InitStaticTable(sizeof(int), sizeof(SpellData_t), entity_id_hash, 
                                                    int_compare_func, 8, key_ptrs, value_ptrs, 4);
    
    TEST_ASSERT(spell_table != NULL, "Should create spell table");
    
    printf("\n=== 🔮 SPELL ANALYSIS ===\n");
    
    // Analyze mana costs and efficiency
    ManaAnalysisContext_t mana_ctx = {0, 0, 0, 0.0f};
    TEST_ASSERT(d_IterateStaticTable(spell_table, mana_analysis_iterator, &mana_ctx) == 0, "Should analyze spells");
    TEST_ASSERT(mana_ctx.spell_count == 4, "Should analyze all 4 spells");
    TEST_ASSERT(mana_ctx.high_cost_spells >= 2, "Should find high-cost spells");
    
    printf("\n📊 SPELL SUMMARY:\n");
    printf("Total Mana Cost: %d\n", mana_ctx.total_mana_cost);
    printf("Average Damage: %.1f\n", mana_ctx.average_damage / mana_ctx.spell_count);
    printf("High-Cost Spells: %d\n", mana_ctx.high_cost_spells);
    
    printf("\n=== ⚖️  SPELL REBALANCING PATCH ===\n");
    
    // Rebalance spells (reduce costs, increase damage)
    SpellRebalanceContext_t rebalance_ctx = {0.8f, 1.2f, 0, 0.0f}; // -20% cost, +20% damage
    TEST_ASSERT(d_IterateStaticTable(spell_table, spell_rebalance_iterator, &rebalance_ctx) == 0, "Should rebalance spells");
    TEST_ASSERT(rebalance_ctx.spells_rebalanced == 4, "Should rebalance all 4 spells");
    
    printf("\n🎯 Rebalanced %d spells with %.2f total power change!\n", 
           rebalance_ctx.spells_rebalanced, rebalance_ctx.total_power_change);
    
    d_DestroyStaticTable(&spell_table);
    return 1;
}

// =============================================================================
// DEBUG-HUNTING TESTS
// =============================================================================

int test_memory_corruption_detection(void)
{
    // Create test data that might reveal memory issues
    int keys[] = {1, 2, 3, 4, 5};
    int values[] = {10, 20, 30, 40, 50};
    const void* key_ptrs[5];
    const void* value_ptrs[5];
    
    for (int i = 0; i < 5; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), entity_id_hash, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(table != NULL, "Should create table for memory corruption test");
    
    printf("\n=== 🔍 MEMORY CORRUPTION DETECTION TEST ===\n");
    
    MemoryCorruptionContext_t corruption_ctx = {0, 0, 0, 0, 0};
    TEST_ASSERT(d_IterateStaticTable(table, memory_corruption_detector, &corruption_ctx) == 0, 
               "Should complete memory corruption detection");
    
    TEST_ASSERT(corruption_ctx.entries_checked == 5, "Should check all 5 entries");
    TEST_ASSERT(corruption_ctx.null_key_count == 0, "Should find no NULL keys");
    TEST_ASSERT(corruption_ctx.null_value_count == 0, "Should find no NULL values");
    TEST_ASSERT(corruption_ctx.corruption_found == 0, "Should find no corruption patterns");
    
    printf("\n📊 CORRUPTION SUMMARY:\n");
    printf("Entries checked: %zu\n", corruption_ctx.entries_checked);
    printf("NULL keys: %zu\n", corruption_ctx.null_key_count);
    printf("NULL values: %zu\n", corruption_ctx.null_value_count);
    printf("Boundary violations: %zu\n", corruption_ctx.boundary_violations);
    printf("Corruption patterns: %zu\n", corruption_ctx.corruption_found);
    
    d_DestroyStaticTable(&table);
    return 1;
}

int test_key_pattern_analysis(void)
{
    // Create test data with various key patterns
    int keys[] = {-5, 0, 1, 2, 3, 100, 1000};
    int values[] = {-50, 0, 10, 20, 30, 1000, 10000};
    const void* key_ptrs[7];
    const void* value_ptrs[7];
    
    for (int i = 0; i < 7; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), entity_id_hash, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 7);
    
    TEST_ASSERT(table != NULL, "Should create table for key pattern analysis");
    
    printf("\n=== 🔑 KEY PATTERN ANALYSIS TEST ===\n");
    
    KeyPatternContext_t pattern_ctx = {0, 0, 0, 0, 0, 0, NULL};
    TEST_ASSERT(d_IterateStaticTable(table, key_pattern_detector, &pattern_ctx) == 0, 
               "Should complete key pattern analysis");
    
    TEST_ASSERT(pattern_ctx.negative_count >= 1, "Should detect negative keys");
    TEST_ASSERT(pattern_ctx.zero_count >= 1, "Should detect zero key");
    TEST_ASSERT(pattern_ctx.duplicate_count == 0, "Should find no duplicates in static table");
    
    printf("\n📊 KEY PATTERN SUMMARY:\n");
    printf("Min key: %d\n", pattern_ctx.min_key);
    printf("Max key: %d\n", pattern_ctx.max_key);
    printf("Sequential pairs: %d\n", pattern_ctx.sequential_count);
    printf("Negative keys: %d\n", pattern_ctx.negative_count);
    printf("Zero keys: %d\n", pattern_ctx.zero_count);
    printf("Duplicate keys: %d\n", pattern_ctx.duplicate_count);
    
    d_DestroyStaticTable(&table);
    return 1;
}

int test_value_integrity_validation(void)
{
    // Create test data where values should follow key * 10 pattern
    int keys[] = {1, 2, 3, 4, 5};
    int correct_values[] = {10, 20, 30, 40, 50}; // key * 10
    const void* key_ptrs[5];
    const void* value_ptrs[5];
    
    for (int i = 0; i < 5; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &correct_values[i];
    }
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), entity_id_hash, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(table != NULL, "Should create table for value integrity test");
    
    printf("\n=== ✅ VALUE INTEGRITY VALIDATION TEST ===\n");
    
    ValueIntegrityContext_t integrity_ctx = {0, 0, 0, 0, 0};
    TEST_ASSERT(d_IterateStaticTable(table, value_integrity_checker, &integrity_ctx) == 0, 
               "Should complete value integrity check");
    
    TEST_ASSERT(integrity_ctx.total_entries == 5, "Should process all 5 entries");
    TEST_ASSERT(integrity_ctx.checksum_mismatches == 0, "Should find no checksum mismatches");
    TEST_ASSERT(integrity_ctx.negative_value_count == 0, "Should find no negative values");
    
    printf("\n📊 INTEGRITY SUMMARY:\n");
    printf("Total entries: %zu\n", integrity_ctx.total_entries);
    printf("Checksum mismatches: %zu\n", integrity_ctx.checksum_mismatches);
    printf("Value overflows: %zu\n", integrity_ctx.value_overflow_count);
    printf("Negative values: %zu\n", integrity_ctx.negative_value_count);
    printf("Final checksum: 0x%X\n", integrity_ctx.running_checksum);
    
    // Now test with corrupted data
    printf("\n=== 🚨 TESTING WITH CORRUPTED VALUES ===\n");
    
    int bad_values[] = {10, 999, 30, -100, 50}; // Corrupted: 999 instead of 20, -100 instead of 40
    const void* bad_value_ptrs[5];
    for (int i = 0; i < 5; i++) {
        bad_value_ptrs[i] = &bad_values[i];
    }
    
    dStaticTable_t* bad_table = d_InitStaticTable(sizeof(int), sizeof(int), entity_id_hash, 
                                                  int_compare_func, 8, key_ptrs, bad_value_ptrs, 5);
    
    ValueIntegrityContext_t bad_integrity_ctx = {0, 0, 0, 0, 0};
    d_IterateStaticTable(bad_table, value_integrity_checker, &bad_integrity_ctx);
    
    TEST_ASSERT(bad_integrity_ctx.checksum_mismatches >= 2, "Should detect checksum mismatches");
    TEST_ASSERT(bad_integrity_ctx.negative_value_count >= 1, "Should detect negative values");
    
    d_DestroyStaticTable(&table);
    d_DestroyStaticTable(&bad_table);
    return 1;
}

int test_performance_stress_analysis(void)
{
    // Create larger dataset for performance testing
    const int num_entries = 10;
    int* keys = malloc(num_entries * sizeof(int));
    int* values = malloc(num_entries * sizeof(int));
    const void** key_ptrs = malloc(num_entries * sizeof(void*));
    const void** value_ptrs = malloc(num_entries * sizeof(void*));
    
    for (int i = 0; i < num_entries; i++) {
        keys[i] = i + 1;
        values[i] = (i + 1) * 10;
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), entity_id_hash, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, num_entries);
    
    TEST_ASSERT(table != NULL, "Should create table for performance stress test");
    
    printf("\n=== ⚡ PERFORMANCE STRESS ANALYSIS TEST ===\n");
    
    PerformanceStressContext_t perf_ctx = {0, 0, 0, 0, 0.0, 0.0};
    perf_ctx.start_time = clock();
    
    TEST_ASSERT(d_IterateStaticTable(table, performance_stress_iterator, &perf_ctx) == 0, 
               "Should complete performance stress test");
    
    TEST_ASSERT(perf_ctx.iterations == num_entries, "Should process all entries");
    TEST_ASSERT(perf_ctx.memory_accesses > 0, "Should record memory accesses");
    TEST_ASSERT(perf_ctx.computation_cycles > 0, "Should record computation cycles");
    
    printf("\n📊 PERFORMANCE SUMMARY:\n");
    printf("Total iterations: %zu\n", perf_ctx.iterations);
    printf("Memory accesses: %zu\n", perf_ctx.memory_accesses);
    printf("Computation cycles: %zu\n", perf_ctx.computation_cycles);
    printf("Max iteration time: %.6fs\n", perf_ctx.max_iteration_time);
    printf("Total time: %.6fs\n", perf_ctx.total_time);
    printf("Average time per entry: %.6fs\n", perf_ctx.total_time / perf_ctx.iterations);
    
    free(keys);
    free(values);
    free(key_ptrs);
    free(value_ptrs);
    d_DestroyStaticTable(&table);
    return 1;
}

int test_state_corruption_detection(void)
{
    // Create test data for state corruption detection
    int keys[] = {10, 5, 15, 1, 20};
    int values[] = {100, 50, 150, 10, 200};
    const void* key_ptrs[5];
    const void* value_ptrs[5];
    
    for (int i = 0; i < 5; i++) {
        key_ptrs[i] = &keys[i];
        value_ptrs[i] = &values[i];
    }
    
    dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(int), entity_id_hash, 
                                              int_compare_func, 8, key_ptrs, value_ptrs, 5);
    
    TEST_ASSERT(table != NULL, "Should create table for state corruption test");
    
    printf("\n=== 🔄 STATE CORRUPTION DETECTION TEST ===\n");
    
    StateCorruptionContext_t state_ctx = {0, 0, true, 0, 0, 0};
    TEST_ASSERT(d_IterateStaticTable(table, state_corruption_detector, &state_ctx) == 0, 
               "Should complete state corruption detection");
    
    TEST_ASSERT(state_ctx.callback_count == 5, "Should call callback for all entries");
    TEST_ASSERT(state_ctx.state_changes >= 0, "Should track state changes");
    TEST_ASSERT(!state_ctx.first_iteration, "Should have completed first iteration");
    
    printf("\n📊 STATE CORRUPTION SUMMARY:\n");
    printf("Total callbacks: %zu\n", state_ctx.callback_count);
    printf("State changes: %zu\n", state_ctx.state_changes);
    printf("Unexpected transitions: %zu\n", state_ctx.unexpected_transitions);
    printf("Final key: %d\n", state_ctx.previous_key);
    printf("Final value: %d\n", state_ctx.previous_value);
    
    d_DestroyStaticTable(&table);
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
    
    d_LogInfo("Starting Static Hash Table Game Iterator Tests");

    TEST_SUITE_START("Static Hash Table Game Iterator Tests");

    // Run game-focused iterator tests
    RUN_TEST(test_entity_stat_boost_iterator);
    RUN_TEST(test_level_up_iterator);
    RUN_TEST(test_inventory_management_iterators);
    RUN_TEST(test_quest_system_iterators);
    RUN_TEST(test_spell_system_iterators);

    // Run debug-hunting iterator tests
    RUN_TEST(test_memory_corruption_detection);
    RUN_TEST(test_key_pattern_analysis);
    RUN_TEST(test_value_integrity_validation);
    RUN_TEST(test_performance_stress_analysis);
    RUN_TEST(test_state_corruption_detection);

    TEST_SUITE_END();

    d_LogInfo("Static Hash Table Game Iterator Test Suite completed.");

    d_DestroyLogger(logger);
    return (total_tests == tests_passed) ? 0 : 1;
}