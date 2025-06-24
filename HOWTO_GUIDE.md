## Game Development with Daedalus String Builder

The Daedalus string builder is particularly well-suited for RPG development, where dynamic text generation is crucial for creating immersive narratives, dialogue systems, and procedural content. Here are practical examples for common RPG scenarios:

### Character Description Generator

```c
#include <Daedalus.h>

void generate_character_description(const char* name, const char* class, int level, int health) {
    dString_t* desc = d_InitString();

    d_AppendString(desc, "The legendary ", 0);
    d_AppendString(desc, name, 0);
    d_AppendString(desc, " stands before you - a level ", 0);
    d_AppendInt(desc, level);
    d_AppendChar(desc, ' ');
    d_AppendString(desc, class, 0);
    d_AppendString(desc, " with ", 0);
    d_AppendInt(desc, health);
    d_AppendString(desc, " health points remaining.", 0);

    printf("%s\n", d_PeekString(desc));
    d_DestroyString(desc);
}
```

### Combat Log Generator

```c
#include <Daedalus.h>

void generate_combat_log(const char* attacker, const char* target,
                        const char* skill, int damage, int critical) {
    dString_t* log = d_InitString();

    d_AppendString(log, "[COMBAT] ", 0);
    d_AppendString(log, attacker, 0);
    d_AppendString(log, " uses ", 0);
    d_AppendString(log, skill, 0);
    d_AppendString(log, " on ", 0);
    d_AppendString(log, target, 0);

    if (critical) {
        d_AppendString(log, " for a CRITICAL ", 0);
    } else {
        d_AppendString(log, " for ", 0);
    }

    d_AppendInt(log, damage);
    d_AppendString(log, " damage!", 0);

    printf("%s\n", d_PeekString(log));
    d_DestroyString(log);
}
```

### Dynamic Dialogue System with Templates

```c
#include <Daedalus.h>

typedef struct {
    char* speaker_name;
    int relationship_level;
    int quest_progress;
    char* faction;
} NPCData;

void generate_dynamic_dialogue(NPCData* npc, const char* player_name) {
    dString_t* dialogue = d_InitString();

    // Use template system for dynamic dialogue
    const char* keys[] = {"speaker", "player", "faction", "relationship", "progress"};
    const char* values[5];
    values[0] = npc->speaker_name;
    values[1] = player_name;
    values[2] = npc->faction;

    // Dynamic relationship text
    if (npc->relationship_level > 80) {
        values[3] = "trusted ally";
    } else if (npc->relationship_level > 40) {
        values[3] = "respected friend";
    } else {
        values[3] = "unknown stranger";
    }

    // Quest progress as string
    dString_t* progress_str = d_InitString();
    d_AppendInt(progress_str, npc->quest_progress);
    values[4] = d_PeekString(progress_str);

    d_TemplateString(dialogue,
        "The {faction} {speaker} nods respectfully.\n"
        "\"Greetings, {player}. As a {relationship}, I must inform you that "
        "our shared quest is {progress}% complete. Will you continue to aid us?\"",
        keys, values, 5);

    printf("%s\n", d_PeekString(dialogue));

    d_DestroyString(progress_str);
    d_DestroyString(dialogue);
}
```

## Game Development with Daedalus Dynamic Arrays

The Daedalus dynamic array system provides efficient, type-safe collections perfect for managing game entities, inventory systems, and real-time data.

Here are practical examples for common game development scenarios:

### Player Inventory Management

```c
#include <Daedalus.h>

typedef struct {
    int item_id;
    char name[64];
    int quantity;
    float weight;
    int rarity;
} InventoryItem;

typedef struct {
    dArray_t* items;
    float max_weight;
    int max_slots;
} PlayerInventory;

PlayerInventory* create_inventory(int max_slots, float max_weight) {
    PlayerInventory* inv = malloc(sizeof(PlayerInventory));
    inv->items = d_InitArray(max_slots, sizeof(InventoryItem));
    inv->max_weight = max_weight;
    inv->max_slots = max_slots;
    return inv;
}

int add_item_to_inventory(PlayerInventory* inv, int item_id, const char* name,
                         int quantity, float weight, int rarity) {
    if (inv->items->count >= inv->max_slots) {
        printf("Inventory full! Cannot add %s\n", name);
        return 0;
    }

    InventoryItem new_item = {
        .item_id = item_id,
        .quantity = quantity,
        .weight = weight,
        .rarity = rarity
    };
    strncpy(new_item.name, name, sizeof(new_item.name) - 1);

    d_AppendArray(inv->items, &new_item);
    printf("Added %d x %s to inventory\n", quantity, name);
    return 1;
}

void print_inventory(PlayerInventory* inv) {
    printf("\n=== INVENTORY ===\n");
    printf("Items: %zu/%d\n", inv->items->count, inv->max_slots);

    for (size_t i = 0; i < inv->items->count; i++) {
        InventoryItem* item = (InventoryItem*)d_GetDataFromArrayByIndex(inv->items, i);
        printf("[%zu] %s (x%d) - Weight: %.1f, Rarity: %d\n",
               i, item->name, item->quantity, item->weight, item->rarity);
    }
}

void destroy_inventory(PlayerInventory* inv) {
    d_DestroyArray(inv->items);
    free(inv);
}
```

### Real-time Entity Management System

```c
#include <Daedalus.h>

typedef struct {
    int entity_id;
    float x, y, z;
    float velocity_x, velocity_y, velocity_z;
    float health;
    int entity_type;  // 0=player, 1=enemy, 2=projectile, 3=pickup
    int active;
} GameEntity;

typedef struct {
    dArray_t* entities;
    dArray_t* inactive_entities;  // Entity pool for reuse
    int next_entity_id;
} EntityManager;

EntityManager* create_entity_manager(int max_entities) {
    EntityManager* mgr = malloc(sizeof(EntityManager));
    mgr->entities = d_InitArray(max_entities, sizeof(GameEntity));
    mgr->inactive_entities = d_InitArray(max_entities, sizeof(GameEntity));
    mgr->next_entity_id = 1;
    return mgr;
}

int spawn_entity(EntityManager* mgr, float x, float y, float z, int type) {
    GameEntity new_entity = {
        .entity_id = mgr->next_entity_id++,
        .x = x, .y = y, .z = z,
        .velocity_x = 0, .velocity_y = 0, .velocity_z = 0,
        .health = 100.0f,
        .entity_type = type,
        .active = 1
    };

    d_AppendArray(mgr->entities, &new_entity);
    printf("Spawned entity ID %d at (%.1f, %.1f, %.1f)\n",
           new_entity.entity_id, x, y, z);
    return new_entity.entity_id;
}

void update_entities(EntityManager* mgr, float delta_time) {
    for (size_t i = 0; i < mgr->entities->count; i++) {
        GameEntity* entity = (GameEntity*)d_GetDataFromArrayByIndex(mgr->entities, i);

        if (!entity->active) continue;

        // Update position based on velocity
        entity->x += entity->velocity_x * delta_time;
        entity->y += entity->velocity_y * delta_time;
        entity->z += entity->velocity_z * delta_time;

        // Remove entities that fall below the world
        if (entity->y < -100.0f) {
            entity->active = 0;
        }
    }
}

void cleanup_inactive_entities(EntityManager* mgr) {
    // Remove inactive entities using the stack-like pop operation
    for (int i = mgr->entities->count - 1; i >= 0; i--) {
        GameEntity* entity = (GameEntity*)d_GetDataFromArrayByIndex(mgr->entities, i);
        if (!entity->active) {
            // Move to inactive pool for reuse
            d_AppendArray(mgr->inactive_entities, entity);

            // Remove from active entities (this is simplified - normally you'd use a swap-remove)
            // For demonstration, we'll just mark as handled
            printf("Cleaned up entity ID %d\n", entity->entity_id);
        }
    }
}

void destroy_entity_manager(EntityManager* mgr) {
    d_DestroyArray(mgr->entities);
    d_DestroyArray(mgr->inactive_entities);
    free(mgr);
}
```

### Particle System for Visual Effects

```c
#include <Daedalus.h>

typedef struct {
    float x, y, z;
    float velocity_x, velocity_y, velocity_z;
    float life_remaining;
    float max_life;
    int color_r, color_g, color_b, alpha;
    float size;
} Particle;

typedef struct {
    dArray_t* active_particles;
    dArray_t* particle_pool;  // Reusable particles
    int max_particles;
} ParticleSystem;

ParticleSystem* create_particle_system(int max_particles) {
    ParticleSystem* system = malloc(sizeof(ParticleSystem));
    system->active_particles = d_InitArray(max_particles, sizeof(Particle));
    system->particle_pool = d_InitArray(max_particles, sizeof(Particle));
    system->max_particles = max_particles;

    // Pre-populate particle pool
    for (int i = 0; i < max_particles; i++) {
        Particle p = {0};
        d_AppendArray(system->particle_pool, &p);
    }

    return system;
}

void emit_particle(ParticleSystem* system, float x, float y, float z,
                  float vel_x, float vel_y, float vel_z, float life) {
    // Try to get a particle from the pool first
    Particle* particle = NULL;

    if (system->particle_pool->count > 0) {
        particle = (Particle*)d_PopDataFromArray(system->particle_pool);
    } else if (system->active_particles->count < system->max_particles) {
        // Pool is empty, create new if under limit
        Particle new_particle = {0};
        d_AppendArray(system->active_particles, &new_particle);
        particle = (Particle*)d_GetDataFromArrayByIndex(system->active_particles,
                                                        system->active_particles->count - 1);
    }

    if (particle) {
        particle->x = x;
        particle->y = y;
        particle->z = z;
        particle->velocity_x = vel_x;
        particle->velocity_y = vel_y;
        particle->velocity_z = vel_z;
        particle->life_remaining = life;
        particle->max_life = life;
        particle->color_r = 255;
        particle->color_g = 100;
        particle->color_b = 50;
        particle->alpha = 255;
        particle->size = 1.0f;
    }
}

void update_particle_system(ParticleSystem* system, float delta_time) {
    // Update all active particles
    for (int i = system->active_particles->count - 1; i >= 0; i--) {
        Particle* p = (Particle*)d_GetDataFromArrayByIndex(system->active_particles, i);

        // Update physics
        p->x += p->velocity_x * delta_time;
        p->y += p->velocity_y * delta_time;
        p->z += p->velocity_z * delta_time;

        // Apply gravity
        p->velocity_y -= 9.8f * delta_time;

        // Update life
        p->life_remaining -= delta_time;

        // Fade out based on remaining life
        float life_ratio = p->life_remaining / p->max_life;
        p->alpha = (int)(255 * life_ratio);

        // Remove dead particles and return to pool
        if (p->life_remaining <= 0) {
            d_AppendArray(system->particle_pool, p);
            // Note: This simplified example doesn't actually remove from active array
            // In a real implementation, you'd use swap-remove or similar
        }
    }
}

void destroy_particle_system(ParticleSystem* system) {
    d_DestroyArray(system->active_particles);
    d_DestroyArray(system->particle_pool);
    free(system);
}
```

## Game Development with Daedalus Logger

The Daedalus logging system provides a robust, efficient way to track game events, debug issues, and monitor performance in real-time. With support for multiple log levels, formatted output, and easy integration, it's perfect for game development workflows.

### Basic Game Event Logging:

```c
#include <Daedalus.h>

typedef struct { // Example struct for player data
    char name[32];
    int level;
    int health;
    int max_health;
    float x, y;
} Player;

typedef struct { // Example struct for enemy data
    char name[32];
    int health;
    int damage;
    float x, y;
} Enemy;

void setup_game_logging() {
    // Create logger with appropriate settings for game development
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,  // Show all logs during development
        .include_timestamp = true,           // Track when events occur
        .include_file_info = false,          // Keep output clean for gameplay
        .colorize_output = true              // Easy visual distinction
    };

    dLogger_t* game_logger = d_CreateLogger(config);
    d_SetGlobalLogger(game_logger);

    d_LogInfo("Game logging system initialized");
}

void log_player_actions(Player* player) {
    // Log player state changes
    d_LogInfoF("Player '%s' moved to position (%.2f, %.2f)",
               player->name, player->x, player->y);

    // Log critical health situations
    float health_percent = (float)player->health / player->max_health * 100.0f;
    if (health_percent < 25.0f) {
        d_LogWarningF("Player '%s' health critical: %d/%d (%.1f%%)",
                      player->name, player->health, player->max_health, health_percent);
    }

    // Log level progression
    d_LogInfoF("Player '%s' is now level %d", player->name, player->level);
}

void log_combat_events(Player* player, Enemy* enemy, int damage_dealt, int damage_received) {
    // Log combat start
    d_LogInfoF("Combat initiated: %s vs %s", player->name, enemy->name);

    // Log damage dealt
    if (damage_dealt > 0) {
        d_LogInfoF("%s attacks %s for %d damage",
                   player->name, enemy->name, damage_dealt);

        if (damage_dealt > enemy->health) {
            d_LogInfoF("%s defeated! Overkill: %d damage",
                       enemy->name, damage_dealt - enemy->health);
        }
    }

    // Log damage received
    if (damage_received > 0) {
        d_LogWarningF("%s takes %d damage from %s",
                      player->name, damage_received, enemy->name);
    }

    // Log combat end
    if (enemy->health <= 0) {
        d_LogInfoF("Combat victory: %s defeated %s", player->name, enemy->name);
    } else if (player->health <= 0) {
        d_LogErrorF("Combat defeat: %s was defeated by %s", player->name, enemy->name);
    }
}

void log_game_systems(float delta_time, int fps, int active_enemies, int items_collected) {
    // Performance logging (less frequent)
    static float log_timer = 0.0f;
    log_timer += delta_time;

    if (log_timer >= 5.0f) {  // Log every 5 seconds
        d_LogDebugF("Performance: FPS=%d, Delta=%.3fs, Enemies=%d, Items=%d",
                    fps, delta_time, active_enemies, items_collected);
        log_timer = 0.0f;
    }

    // Error conditions
    if (fps < 30) {
        d_LogWarningF("Low FPS detected: %d (target: 60)", fps);
    }

    if (active_enemies > 100) {
        d_LogWarningF("High enemy count may impact performance: %d active", active_enemies);
    }
}

void log_game_state_changes(const char* old_state, const char* new_state, const char* reason) {
    d_LogInfoF("Game state transition: %s → %s (reason: %s)",
               old_state, new_state, reason);
}

void log_resource_loading(const char* resource_type, const char* filename, bool success, float load_time) {
    if (success) {
        d_LogInfoF("Loaded %s: %s (%.3fs)", resource_type, filename, load_time);
    } else {
        d_LogErrorF("Failed to load %s: %s", resource_type, filename);
    }
}

// Example usage in main game loop
int main() {
    setup_game_logging();

    // Initialize game objects
    Player player = {"Hero", 1, 100, 100, 0.0f, 0.0f};
    Enemy goblin = {"Goblin", 30, 15, 5.0f, 3.0f};

    d_LogInfo("=== Game Started ===");
    log_game_state_changes("MENU", "PLAYING", "player_start");

    // Simulate some game events
    player.x = 10.5f;
    player.y = 7.2f;
    log_player_actions(&player);

    // Simulate combat
    log_combat_events(&player, &goblin, 25, 10);
    player.health -= 10;
    goblin.health -= 25;

    // Simulate resource loading
    log_resource_loading("texture", "player_sprite.png", true, 0.045f);
    log_resource_loading("audio", "missing_sound.wav", false, 0.0f);

    // Simulate performance monitoring
    log_game_systems(0.016f, 60, 5, 12);

    // Player levels up
    player.level = 2;
    player.max_health = 120;
    player.health = 120;
    d_LogInfoF("Player leveled up! %s is now level %d with %d max health",
               player.name, player.level, player.max_health);

    d_LogInfo("=== Game Session Ended ===");

    // Cleanup
    dLogger_t* logger = d_GetGlobalLogger();
    if (logger) {
        d_DestroyLogger(logger);
    }

    return 0;
}
```

### Advanced RPG Combat & Analytics System

```c
#include <Daedalus.h>
#include <time.h>

// Advanced game structures
typedef struct {
    int player_id;
    char name[32];
    char class_name[16];
    int level;
    int health, max_health;
    int mana, max_mana;
    float x, y, z;
    int experience;
    int session_id;
} RPGPlayer;

typedef struct {
    int enemy_id;
    char name[32];
    char type[16];
    int health, max_health;
    int damage_rating;
    float x, y, z;
    int threat_level;
} RPGEnemy;

typedef struct {
    dLogger_t* gameplay_logger;
    dLogger_t* analytics_logger;
    dLogger_t* performance_logger;
    dString_t* session_analytics;
    FILE* combat_log_file;
    int current_session_id;
} AdvancedGameLogger;

// Initialize sophisticated multi-logger system
AdvancedGameLogger* setup_advanced_game_logging() {
    AdvancedGameLogger* system = malloc(sizeof(AdvancedGameLogger));

    // Create specialized loggers for different purposes
    dLogConfig_t gameplay_config = {
        .default_level = D_LOG_LEVEL_INFO,
        .include_timestamp = true,
        .include_file_info = false,
        .colorize_output = true
    };

    dLogConfig_t analytics_config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_file_info = true,
        .colorize_output = false
    };

    dLogConfig_t performance_config = {
        .default_level = D_LOG_LEVEL_WARNING,
        .include_timestamp = true,
        .include_file_info = true,
        .colorize_output = true
    };

    system->gameplay_logger = d_CreateLogger(gameplay_config);
    system->analytics_logger = d_CreateLogger(analytics_config);
    system->performance_logger = d_CreateLogger(performance_config);

    // Set up multiple handlers for different purposes
    system->session_analytics = d_InitString();
    system->combat_log_file = fopen("combat_session.log", "w");
    system->current_session_id = (int)time(NULL);

    // Gameplay logger: Console + Analytics buffer
    d_AddLogHandler(system->gameplay_logger, d_ConsoleLogHandler, NULL);
    d_AddLogHandler(system->gameplay_logger, d_StringLogHandler, system->session_analytics);

    // Analytics logger: File + Analytics buffer
    if (system->combat_log_file) {
        d_AddLogHandler(system->analytics_logger, d_FileLogHandler, system->combat_log_file);
    }
    d_AddLogHandler(system->analytics_logger, d_StringLogHandler, system->session_analytics);

    // Performance logger: Console only (for immediate alerts)
    d_AddLogHandler(system->performance_logger, d_ConsoleLogHandler, NULL);

    // Set up advanced filtering for performance logger
    dLogFilterBuilder_t* perf_filter = d_CreateFilterBuilder();
    d_FilterBuilder_AddPrefix(perf_filter, "PERF_", D_LOG_LEVEL_INFO);
    d_FilterBuilder_AddSuffix(perf_filter, "_CRITICAL", D_LOG_LEVEL_ERROR);
    d_FilterBuilder_Apply(system->performance_logger, perf_filter);
    d_DestroyFilterBuilder(perf_filter);

    // Set gameplay as global for convenience
    d_SetGlobalLogger(system->gameplay_logger);

    d_LogInfo("=== Advanced RPG Logging System Initialized ===");

    return system;
}

// Advanced structured logging for player actions
void log_player_spawn(AdvancedGameLogger* system, RPGPlayer* player) {
    d_SetGlobalLogger(system->analytics_logger);

    dLogStructured_t* spawn_log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (spawn_log) {
        d_LogStructured_SetFormat(spawn_log, true); // JSON format for analytics
        d_LogStructured_Field(spawn_log, "event_type", "player_spawn");
        d_LogStructured_Field(spawn_log, "player_name", player->name);
        d_LogStructured_Field(spawn_log, "player_class", player->class_name);
        d_LogStructured_FieldInt(spawn_log, "player_id", player->player_id);
        d_LogStructured_FieldInt(spawn_log, "level", player->level);
        d_LogStructured_FieldInt(spawn_log, "session_id", system->current_session_id);
        d_LogStructured_FieldFloat(spawn_log, "spawn_x", player->x, 2);
        d_LogStructured_FieldFloat(spawn_log, "spawn_y", player->y, 2);
        d_LogStructured_FieldFloat(spawn_log, "spawn_z", player->z, 2);
        d_LogStructured_Commit(spawn_log);
    }

    d_SetGlobalLogger(system->gameplay_logger);
    d_LogInfoF("Player %s (Level %d %s) has entered the realm at (%.2f, %.2f, %.2f)",
               player->name, player->level, player->class_name, player->x, player->y, player->z);
}

// Advanced combat logging with detailed analytics
void log_advanced_combat_encounter(AdvancedGameLogger* system, RPGPlayer* player,
                                  RPGEnemy* enemy, int damage_dealt, int damage_received,
                                  const char* skill_used, bool critical_hit, float combat_duration) {

    // Analytics logging (structured data)
    d_SetGlobalLogger(system->analytics_logger);

    dLogStructured_t* combat_log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (combat_log) {
        d_LogStructured_SetFormat(combat_log, true);
        d_LogStructured_Field(combat_log, "event_type", "combat_action");
        d_LogStructured_Field(combat_log, "player_name", player->name);
        d_LogStructured_Field(combat_log, "enemy_name", enemy->name);
        d_LogStructured_Field(combat_log, "enemy_type", enemy->type);
        d_LogStructured_Field(combat_log, "skill_used", skill_used);
        d_LogStructured_FieldInt(combat_log, "damage_dealt", damage_dealt);
        d_LogStructured_FieldInt(combat_log, "damage_received", damage_received);
        d_LogStructured_FieldInt(combat_log, "player_health_remaining", player->health);
        d_LogStructured_FieldInt(combat_log, "enemy_health_remaining", enemy->health);
        d_LogStructured_FieldInt(combat_log, "critical_hit", critical_hit ? 1 : 0);
        d_LogStructured_FieldFloat(combat_log, "combat_duration", combat_duration, 3);
        d_LogStructured_FieldInt(combat_log, "threat_level", enemy->threat_level);
        d_LogStructured_FieldInt(combat_log, "session_id", system->current_session_id);
        d_LogStructured_Commit(combat_log);
    }

    // User-friendly gameplay logging
    d_SetGlobalLogger(system->gameplay_logger);

    if (critical_hit) {
        d_LogInfoF("💥 CRITICAL HIT! %s uses %s on %s for %d damage!",
                   player->name, skill_used, enemy->name, damage_dealt);
    } else {
        d_LogInfoF("⚔️  %s attacks %s with %s for %d damage",
                   player->name, enemy->name, skill_used, damage_dealt);
    }

    if (damage_received > 0) {
        d_LogWarningF("🩸 %s takes %d damage from %s (Health: %d/%d)",
                      player->name, damage_received, enemy->name, player->health, player->max_health);
    }

    // Check for combat end conditions
    if (enemy->health <= 0) {
        int exp_gained = enemy->threat_level * 10;
        player->experience += exp_gained;

        d_LogInfoF("🏆 Victory! %s defeated %s and gained %d experience",
                   player->name, enemy->name, exp_gained);

        // Log victory analytics
        d_SetGlobalLogger(system->analytics_logger);
        dLogStructured_t* victory_log = d_LogStructured(D_LOG_LEVEL_INFO);
        if (victory_log) {
            d_LogStructured_SetFormat(victory_log, false); // Key-value for quick parsing
            d_LogStructured_Field(victory_log, "event_type", "combat_victory");
            d_LogStructured_Field(victory_log, "winner", player->name);
            d_LogStructured_Field(victory_log, "defeated", enemy->name);
            d_LogStructured_FieldInt(victory_log, "exp_gained", exp_gained);
            d_LogStructured_FieldFloat(victory_log, "total_combat_time", combat_duration, 2);
            d_LogStructured_Commit(victory_log);
        }
    }
}

// Performance monitoring with advanced filtering
void log_performance_metrics(AdvancedGameLogger* system, float frame_time, int fps,
                           int active_entities, float memory_usage_mb, int draw_calls) {
    d_SetGlobalLogger(system->performance_logger);

    // Critical performance warnings
    if (frame_time > 0.033f) { // > 33ms (below 30 FPS)
        d_LogErrorF("PERF_CRITICAL: Frame time excessive: %.3fs (FPS: %d)", frame_time, fps);
    }

    if (memory_usage_mb > 512.0f) {
        d_LogWarningF("PERF_MEMORY_HIGH: Memory usage: %.1f MB", memory_usage_mb);
    }

    if (draw_calls > 1000) {
        d_LogWarningF("PERF_DRAW_CALLS_HIGH: Draw calls: %d", draw_calls);
    }

    // Regular performance logging (structured for analysis)
    static float perf_log_timer = 0.0f;
    perf_log_timer += frame_time;

    if (perf_log_timer >= 10.0f) { // Every 10 seconds
        dLogStructured_t* perf_log = d_LogStructured(D_LOG_LEVEL_DEBUG);
        if (perf_log) {
            d_LogStructured_SetFormat(perf_log, false);
            d_LogStructured_Field(perf_log, "metric_type", "performance_snapshot");
            d_LogStructured_FieldFloat(perf_log, "avg_frame_time", frame_time, 4);
            d_LogStructured_FieldInt(perf_log, "fps", fps);
            d_LogStructured_FieldInt(perf_log, "active_entities", active_entities);
            d_LogStructured_FieldFloat(perf_log, "memory_mb", memory_usage_mb, 1);
            d_LogStructured_FieldInt(perf_log, "draw_calls", draw_calls);
            d_LogStructured_Commit(perf_log);
        }
        perf_log_timer = 0.0f;
    }
}

// Session analytics with statistical reporting
void generate_session_analytics_report(AdvancedGameLogger* system) {
    d_SetGlobalLogger(system->analytics_logger);

    // Get analytics data from captured logs
    const char* session_data = d_PeekString(system->session_analytics);
    size_t total_log_size = strlen(session_data);

    // Count different event types (simplified parsing)
    int combat_events = 0;
    int spawn_events = 0;
    int victory_events = 0;

    const char* ptr = session_data;
    while ((ptr = strstr(ptr, "\"event_type\":")) != NULL) {
        if (strstr(ptr, "combat_action")) combat_events++;
        else if (strstr(ptr, "player_spawn")) spawn_events++;
        else if (strstr(ptr, "combat_victory")) victory_events++;
        ptr += 13; // Move past "event_type":
    }

    d_LogInfo("=== SESSION ANALYTICS REPORT ===");

    dLogStructured_t* report = d_LogStructured(D_LOG_LEVEL_INFO);
    if (report) {
        d_LogStructured_SetFormat(report, true);
        d_LogStructured_Field(report, "report_type", "session_summary");
        d_LogStructured_FieldInt(report, "session_id", system->current_session_id);
        d_LogStructured_FieldInt(report, "total_combat_actions", combat_events);
        d_LogStructured_FieldInt(report, "player_spawns", spawn_events);
        d_LogStructured_FieldInt(report, "combat_victories", victory_events);
        d_LogStructured_FieldInt(report, "total_log_bytes", (int)total_log_size);

        // Calculate derived metrics
        float victory_rate = spawn_events > 0 ? (float)victory_events / spawn_events * 100.0f : 0.0f;
        d_LogStructured_FieldFloat(report, "victory_rate_percent", victory_rate, 1);

        float actions_per_spawn = spawn_events > 0 ? (float)combat_events / spawn_events : 0.0f;
        d_LogStructured_FieldFloat(report, "avg_actions_per_spawn", actions_per_spawn, 2);

        d_LogStructured_Commit(report);
    }

    // Get final performance statistics
    dLogStats_t stats;
    d_GetLogStats(system->analytics_logger, &stats);

    d_LogInfoF("Total logging time: %.4f seconds", stats.total_log_time);
    d_LogInfoF("Logs suppressed: %llu", (unsigned long long)stats.logs_suppressed);
    d_LogInfoF("Handler errors: %u", stats.handler_errors);
}

// Cleanup the advanced logging system
void cleanup_advanced_game_logging(AdvancedGameLogger* system) {
    if (!system) return;

    d_LogInfo("=== Shutting Down Advanced Logging System ===");

    // Generate final analytics
    generate_session_analytics_report(system);

    // Reset statistics for clean shutdown logging
    d_ResetLogStats(system->gameplay_logger);
    d_ResetLogStats(system->analytics_logger);
    d_ResetLogStats(system->performance_logger);

    // Cleanup resources
    if (system->gameplay_logger) d_DestroyLogger(system->gameplay_logger);
    if (system->analytics_logger) d_DestroyLogger(system->analytics_logger);
    if (system->performance_logger) d_DestroyLogger(system->performance_logger);

    if (system->session_analytics) d_DestroyString(system->session_analytics);
    if (system->combat_log_file) fclose(system->combat_log_file);

    free(system);
}

// Advanced usage example
int main() {
    // Initialize the advanced logging system
    AdvancedGameLogger* log_system = setup_advanced_game_logging();

    // Create game entities
    RPGPlayer player = {
        .player_id = 12345,
        .name = "Shadowblade",
        .class_name = "Rogue",
        .level = 15,
        .health = 120, .max_health = 120,
        .mana = 80, .max_mana = 80,
        .x = 100.5f, .y = 50.2f, .z = 10.0f,
        .experience = 2450,
        .session_id = log_system->current_session_id
    };

    RPGEnemy dragon = {
        .enemy_id = 9001,
        .name = "Ancient Red Dragon",
        .type = "Boss",
        .health = 500, .max_health = 500,
        .damage_rating = 85,
        .x = 95.0f, .y = 45.0f, .z = 15.0f,
        .threat_level = 8
    };

    // Log player spawn
    log_player_spawn(log_system, &player);

    // Simulate an epic boss fight with multiple combat rounds
    d_LogInfo("🐉 BOSS ENCOUNTER: Ancient Red Dragon appears!");

    float combat_start_time = 0.0f;
    int round = 1;

    while (player.health > 0 && dragon.health > 0 && round <= 5) {
        d_LogInfoF("=== Combat Round %d ===", round);

        // Player attacks
        int player_damage = 45 + (rand() % 20); // 45-64 damage
        bool critical = (rand() % 100) < 15; // 15% crit chance
        if (critical) player_damage *= 2;

        dragon.health -= player_damage;
        if (dragon.health < 0) dragon.health = 0;

        // Dragon counter-attacks (if alive)
        int dragon_damage = 0;
        if (dragon.health > 0) {
            dragon_damage = 30 + (rand() % 25); // 30-54 damage
            player.health -= dragon_damage;
            if (player.health < 0) player.health = 0;
        }

        float round_duration = 2.5f + (rand() % 100) / 100.0f; // 2.5-3.5 seconds
        combat_start_time += round_duration;

        log_advanced_combat_encounter(log_system, &player, &dragon,
                                    player_damage, dragon_damage,
                                    critical ? "Shadow Strike" : "Backstab",
                                    critical, round_duration);

        // Simulate performance monitoring
        float frame_time = 0.016f + (rand() % 10) / 10000.0f; // 16-17ms
        int fps = (int)(1.0f / frame_time);
        log_performance_metrics(log_system, frame_time, fps,
                               50 + round * 5, 245.5f + round * 2.3f,
                               800 + round * 50);

        round++;
    }

    // Final session analytics
    d_LogInfo("=== Combat Concluded ===");
    generate_session_analytics_report(log_system);

    // Cleanup
    cleanup_advanced_game_logging(log_system);

    return 0;
}
```

### Advanced Logging System - Complete Feature Showcase

#### Builder Pattern Logging

```c
void demonstrate_builder_pattern() {
    // Fluent interface for complex log construction
    dLogBuilder_t* builder = d_LogBegin(D_LOG_LEVEL_INFO);
    if (builder) {
        builder = d_LogBuilder_Append(builder, "Game state: ");
        builder = d_LogBuilder_AppendInt(builder, current_level);
        builder = d_LogBuilder_Append(builder, ", Player health: ");
        builder = d_LogBuilder_AppendFloat(builder, player_health, 2);
        builder = d_LogBuilder_AppendF(builder, ", Score: %d", player_score);
        d_LogBuilder_End(builder);
    }
}
```

#### Structured Logging (JSON & Key-Value)

```c
void demonstrate_structured_logging() {
    // JSON format structured logging
    dLogStructured_t* json_log = d_LogStructured(D_LOG_LEVEL_INFO);
    if (json_log) {
        d_LogStructured_Field(json_log, "event", "player_death");
        d_LogStructured_FieldInt(json_log, "player_id", 12345);
        d_LogStructured_FieldFloat(json_log, "damage_taken", 127.5f, 1);
        d_LogStructured_Field(json_log, "cause", "dragon_fire");
        d_LogStructured_SetFormat(json_log, true);  // Enable JSON
        d_LogStructured_Commit(json_log);
    }
    // Output: {"event":"player_death","player_id":"12345","damage_taken":"127.5","cause":"dragon_fire"}

    // Key-value format
    dLogStructured_t* kv_log = d_LogStructured(D_LOG_LEVEL_WARNING);
    if (kv_log) {
        d_LogStructured_Field(kv_log, "system", "inventory");
        d_LogStructured_FieldInt(kv_log, "items_count", 99);
        d_LogStructured_Field(kv_log, "status", "near_full");
        d_LogStructured_SetFormat(kv_log, false);  // Key-value format
        d_LogStructured_Commit(kv_log);
    }
    // Output: system=inventory items_count=99 status=near_full
}
```

#### Rate-Limited Logging

```c
void demonstrate_rate_limiting() {
    // Prevent log spam in loops or frequent events
    for (int i = 0; i < 1000; i++) {
        // Only logs 5 times per 2-second window
        d_LogRateLimited(D_LOG_LEVEL_WARNING, 5, 2.0, "High frequency event detected");

        // Rate limited with formatting
        d_LogRateLimitedF(D_LOG_LEVEL_ERROR, 3, 1.0, "Error in iteration %d", i);

        simulate_game_tick();
    }
}
```

#### Conditional Logging

```c
void demonstrate_conditional_logging() {
    bool debug_mode = is_debug_enabled();
    int player_health = get_player_health();

    // Only log if condition is true
    d_LogIf(debug_mode, D_LOG_LEVEL_DEBUG, "Debug mode active");
    d_LogIf(player_health < 20, D_LOG_LEVEL_WARNING, "Player health critically low!");

    // Conditional with formatting
    d_LogIfF(player_health <= 0, D_LOG_LEVEL_FATAL,
             "Player died! Final score: %d", get_player_score());
}
```

#### Context-Aware Logging

```c
void demonstrate_context_logging() {
    // Push context for grouped operations
    dLogContext_t* battle_ctx = d_PushLogContext("BattleSystem");

    d_LogInfo("Battle initiated");

    // Nested context
    dLogContext_t* spell_ctx = d_PushLogContext("SpellCasting");
    d_LogDebug("Casting fireball spell");
    d_LogInfo("Spell cast successful");

    // Pop contexts in reverse order
    d_PopLogContext(spell_ctx);
    d_LogInfo("Battle concluded");
    d_PopLogContext(battle_ctx);
}
```

#### Advanced Logger Configuration

```c
void setup_advanced_logging() {
    // Create custom logger with full configuration
    dLogger_t* game_logger = d_CreateLogger((dLogConfig_t){
        .default_level = D_LOG_LEVEL_DEBUG,
        .include_timestamp = true,
        .include_file_info = true,
        .include_function = true,
        .include_thread_id = true,
        .colorize_output = true,
        .timestamp_format = "%H:%M:%S.%03d",
        .context_separator = " >> "
    });

    // Add multiple handlers
    d_AddLogHandler(game_logger, d_ConsoleLogHandler, NULL);

    FILE* log_file = fopen("game.log", "a");
    d_AddLogHandler(game_logger, d_FileLogHandler, log_file);

    dString_t* log_buffer = d_InitString();
    d_AddLogHandler(game_logger, d_StringLogHandler, log_buffer);

    d_SetGlobalLogger(game_logger);
}
```

#### Filter System for Selective Logging

```c
void demonstrate_log_filtering() {
    dLogger_t* logger = d_GetGlobalLogger();

    // Create filter builder
    dLogFilterBuilder_t* filter = d_CreateFilterBuilder();

    // Filter by directory/module
    d_FilterBuilder_AddDirectory(filter, "engine/", D_LOG_LEVEL_WARNING);
    d_FilterBuilder_AddDirectory(filter, "gameplay/", D_LOG_LEVEL_DEBUG);

    // Filter by prefix
    d_FilterBuilder_AddPrefix(filter, "PERF_", D_LOG_LEVEL_INFO);
    d_FilterBuilder_AddPrefix(filter, "NET_", D_LOG_LEVEL_ERROR);

    // Apply filters
    d_FilterBuilder_Apply(logger, filter);
    d_DestroyFilterBuilder(filter);

    // Or use string-based configuration
    d_FilterBuilder_FromString(logger, "src/*:DEBUG,tests/*:INFO,vendor/*:ERROR");
}
```

#### Performance Monitoring & Statistics

```c
void demonstrate_log_statistics() {
    dLogStats_t stats;
    d_GetLogStats(NULL, &stats);  // Get global stats

    printf("Logging Performance Report:\n");
    printf("- Logs suppressed: %llu\n", (unsigned long long)stats.logs_suppressed);
    printf("- Rate limited: %llu\n", (unsigned long long)stats.logs_rate_limited);
    printf("- Total processing time: %.6f seconds\n", stats.total_log_time);
    printf("- Handler errors: %u\n", stats.handler_errors);

    // Reset statistics
    d_ResetLogStats(NULL);
}
```
