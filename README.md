# Daedalus

A high-performance C library providing linear algebra operations, data structures, and utility functions for game development, scientific computing, and real-time applications.

## Overview

Daedalus is a comprehensive math and utility library written in C that offers efficient implementations of vector mathematics, matrix operations, spatial data structures, and common programming utilities. The library is designed with performance in mind, featuring optimized algorithms like Quake's fast square root and providing both 2D and 3D mathematical operations.

## Features

### String Utilities
- **String Builder**: Efficient string construction and manipulation
- **File Operations**: Read entire files into strings
- **Safe String Operations**: Secure string handling with bounds checking

### Dynamic Arrays
- **Type-Safe Collections**: Generic dynamic arrays that can store any data type
- **Automatic Memory Management**: Handles resizing and memory allocation internally
- **Performance Optimized**: Contiguous memory layout for cache efficiency
- **Stack Operations**: LIFO push/pop functionality for game state management

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
