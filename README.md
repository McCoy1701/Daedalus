# 🏗️ Daedalus Library - Math, Data Structures, and Game Dev Utils
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![Greek Mythology](https://img.shields.io/badge/Powered%20by-Labyrinthine%20Logic-blue.svg)](https://en.wikipedia.org/wiki/Daedalus)
[![Performance](https://img.shields.io/badge/Fast%20Math-Carmack%20Approved-red.svg)](#)
[![MIDAS Toolchain](https://img.shields.io/badge/MIDAS%20Toolchain-Master%20Crafted-gold.svg)](#)

*High-Performance Linear Algebra and Mathematical Foundations for Divine Applications*

> *"I am the architect of impossibilities, the craftsman of precision. My tools transcend mortal limitations."* - Daedalus, Master of the Labyrinth

## What is This Madness?

**The Daedalus Library** is a master-crafted collection of high-performance mathematical tools and data structures that form the foundation of the **MIDAS Toolchain**. Born from the forge of divine geometry and tempered in the fires of real-time computation, these are the building blocks that outlast empires.

This isn't just another math library. This is **precision incarnate**—where John Carmack's legendary fast inverse square root meets the geometric perfection of ancient Greek mathematics, where every function is crafted with the obsessive attention to detail that built the original Labyrinth.

## Installation

### 🔧 Native Development (System-Wide)

```sh
git clone https://github.com/your-repo/daedalus.git
cd daedalus
make shared          # Build shared library
sudo make install    # Install to /usr/lib/ and /usr/include/
```

**Then, in your C projects:**

```c
#include <Daedalus.h>  // System-wide header
// Link with: gcc your_code.c -lDaedalus
```

### 🌐 Emscripten Web Projects

```sh
make EM              # Creates bin/libDaedalus.a static library
```

**Copy these files to your Emscripten project:**

`bin/libDaedalus.a` → your lib/ directory
`include/Daedalus.h` → your include/ directory

**Then compile with:**

```sh
emcc your_code.c lib/libDaedalus.a -I include/ -o output.html
```

### 🧹 Cleanup & Maintenance

```sh
make clean           # Remove all build artifacts
sudo make uninstall  # Remove system installation
```

### 🎮 Sample Transformations

**Before Daedalus (Dangerous C)**


```c
char* name = malloc(256);           // No error checking
strcpy(name, "Player");             // Buffer overflow risk  
strcat(name, " Character");         // More overflow risk
printf("Created: %s\n", name);      // No structured logging
free(name);                         // Manual cleanup required
```

**After Daedalus (Master-Crafted)**

```c
dString_t* name = d_InitString();
d_AppendString(name, "Player", 0);
d_AppendString(name, " Character", 0);  
d_LogInfoF("Created: %s", d_PeekString(name));
d_DestroyString(name);              // Automatic, safe cleanup
```

**The Result:** Code that doesn't just work—code that serves with the wisdom of the master architect himself.

### 📦 Array Choices and Utilities

Daedalus provides robust and efficient array structures for various needs:

`dStaticArray_t` **(Fixed-Size, High Performance)**

Ideal for fixed-capacity data, like game inventories, audio buffers, or render framebuffers. Offers predictable memory use and extremely fast operations due to direct memory access.

```c
// Game Inventory Management
dStaticArray_t* inventory = d_InitStaticArray(10, sizeof(int)); // 10 integer slots
int sword_id = 101;
d_AppendDataToStaticArray(inventory, &sword_id);

size_t free_slots = d_GetFreeSpaceInStaticArray(inventory); // O(1) fast check!
d_LogInfoF("Inventory free slots: %zu", free_slots);

// Efficiently clear a pixel buffer
typedef struct { uint8_t r, g, b, a; } Pixel;
dStaticArray_t* framebuffer = d_InitStaticArray(1920 * 1080, sizeof(Pixel));
Pixel black = {0,0,0,255};
d_FillDataInStaticArray(framebuffer, &black, framebuffer->capacity); // Fast bulk fill

// Direct memory access for graphics/serialization
void* raw_pixel_data = d_PeekRawMemoryOfStaticArray(framebuffer);
// Now, pass 'raw_pixel_data' directly to OpenGL/Vulkan/File I/O for max performance

d_DestroyStaticArray(inventory);
d_DestroyStaticArray(framebuffer);
```

`dDynamicArray_t` **(Resizable, Flexible)**

For collections where size changes frequently, such as lists of entities, event queues, or variable-length records. Automatically handles memory reallocations, balancing flexibility with performance.

```c
// List of active enemies
dDynamicArray_t* enemies = d_InitDynamicArray(sizeof(Enemy_t), 4); // Initial capacity 4
Enemy_t goblin = {.health = 50, .type = GOBLIN};
d_AppendDataToDynamicArray(enemies, &goblin);

// Remove defeated enemies
d_RemoveDataFromDynamicArray(enemies, 0); // Remove first enemy

// Convenient iteration
for (size_t i = 0; i < enemies->count; ++i) {
    Enemy_t* current_enemy = (Enemy_t*)d_IndexDataFromDynamicArray(enemies, i);
    // Process enemy...
}
d_DestroyDynamicArray(enemies);
```

These array utilities embody the Daedalus philosophy: providing powerful, performant, and reliable building blocks for your digital creations.

### 📦 Table Choices and Utilities

Daedalus provides lightning-fast hash table implementations optimized for different use cases:

`dStaticTable_t` **(Fixed-Size, Ultra Performance)**

Perfect for game configuration systems, asset lookups, or any scenario where you know the data set size in advance. Zero memory fragmentation and cache-friendly access patterns make this ideal for real-time applications.

```c
// Game Entity Stats Lookup System
typedef struct { int hp, mp, strength, dexterity; } EntityStats_t;

// Hash function for integer entity IDs
size_t entity_hash(const void* key, size_t key_size) {
    (void)key_size;
    int* entity_id = (int*)key;
    return (size_t)(*entity_id * 2654435761U); // Knuth's multiplicative hash
}

// Comparison function for integer keys
int int_compare(const void* key1, const void* key2, size_t key_size) {
    (void)key_size;
    return (*(int*)key1 == *(int*)key2) ? 0 : 1;
}

// Pre-populate entity data
int entity_ids[] = {101, 102, 103, 104}; // Player, Warrior, Mage, Rogue
EntityStats_t stats[] = {
    {120, 50, 18, 12},  // Player stats
    {100, 30, 20, 10},  // Warrior stats  
    {60, 100, 8, 16},   // Mage stats
    {80, 40, 12, 20}    // Rogue stats
};

const void* keys[4] = {&entity_ids[0], &entity_ids[1], &entity_ids[2], &entity_ids[3]};
const void* values[4] = {&stats[0], &stats[1], &stats[2], &stats[3]};

// Create ultra-fast lookup table
dStaticTable_t* entity_lookup = d_InitStaticTable(
    sizeof(int), sizeof(EntityStats_t), 
    entity_hash, int_compare, 
    8, keys, values, 4  // 8 buckets, 4 entries
);

// Lightning-fast O(1) lookups
int player_id = 101;
EntityStats_t* player_stats = (EntityStats_t*)d_GetValueInStaticTable(entity_lookup, &player_id);
d_LogInfoF("Player HP: %d, MP: %d", player_stats->hp, player_stats->mp);

// Iterate over all entities for game updates
typedef struct { int damage_applied; } BattleContext_t;

void apply_poison_damage(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
    (void)key_size; (void)value_size;
    BattleContext_t* battle = (BattleContext_t*)user_data;
    int entity_id = *(int*)key;
    EntityStats_t* stats = (EntityStats_t*)value;
    
    stats->hp -= 5; // Poison damage
    battle->damage_applied += 5;
    d_LogDebugF("🧪 Entity %d takes poison damage, HP now: %d", entity_id, stats->hp);
}

BattleContext_t battle = {0};
d_IterateStaticTable(entity_lookup, apply_poison_damage, &battle);
d_LogInfoF("⚔️  Total poison damage applied: %d", battle.damage_applied);

d_DestroyStaticTable(&entity_lookup);
```

`dTable_t` **(Resizable, Adaptive)**

For hash tables that need to grow and shrink during runtime, such as player inventories, quest systems, or dynamic configuration. Automatically manages load factors and rehashing for optimal performance.

```c
// Dynamic Quest Management System
typedef struct { 
    int progress, max_progress; 
    bool completed; 
    char description[64]; 
} Quest_t;

dTable_t* active_quests = d_InitTable(
    sizeof(int), sizeof(Quest_t), 
    entity_hash, int_compare, 4  // Start small, grow as needed
);

// Add quests dynamically
Quest_t dragon_quest = {0, 10, false, "Slay the Ancient Dragon"};
Quest_t collect_quest = {5, 20, false, "Collect 20 Magic Crystals"};

int quest_id_1 = 5001, quest_id_2 = 5002;
d_SetValueInTable(active_quests, &quest_id_1, &dragon_quest);
d_SetValueInTable(active_quests, &quest_id_2, &collect_quest);

// Update quest progress
Quest_t* quest = (Quest_t*)d_GetValueInTable(active_quests, &quest_id_2);
if (quest) {
    quest->progress++;
    if (quest->progress >= quest->max_progress) {
        quest->completed = true;
        d_LogInfoF("🎉 Quest completed: %s", quest->description);
    }
}

// Remove completed quests
if (quest && quest->completed) {
    d_RemoveValueFromTable(active_quests, &quest_id_2);
    d_LogInfo("🗑️  Removed completed quest from active list");
}

d_DestroyTable(&active_quests);
```

Both table types support **high-performance iteration** with custom callbacks, making them perfect for game loops, batch processing, and data analysis operations.

### 💾 Saving and Loading from Static Arrays/Tables

Daedalus provides robust binary serialization for persistent storage of your data structures. Perfect for game saves, configuration files, or any scenario requiring fast, reliable data persistence.

**Static Array Persistence**

Static arrays can be seamlessly saved to and loaded from binary files with full metadata preservation:

```c
// Create and populate a weapon inventory
typedef struct { 
    int weapon_id; 
    char name[32]; 
    int damage; 
    float durability; 
} Weapon_t;

dStaticArray_t* arsenal = d_InitStaticArray(10, sizeof(Weapon_t));

// Add some legendary weapons
Weapon_t excalibur = {101, "Excalibur", 120, 100.0f};
Weapon_t mjolnir = {102, "Mjolnir", 150, 100.0f};
Weapon_t gungnir = {103, "Gungnir", 140, 95.0f};

d_AppendDataToStaticArray(arsenal, &excalibur);
d_AppendDataToStaticArray(arsenal, &mjolnir);
d_AppendDataToStaticArray(arsenal, &gungnir);

// Save entire arsenal to binary file
if (d_SaveStaticArrayToFile("player_weapons.dat", arsenal) == 0) {
    d_LogInfo("⚔️  Arsenal saved successfully!");
} else {
    d_LogError("❌ Failed to save arsenal!");
}

d_DestroyStaticArray(arsenal);

// Later... load the arsenal back
dStaticArray_t* loaded_arsenal = d_LoadStaticArrayFromFile("player_weapons.dat");
if (loaded_arsenal) {
    d_LogInfoF("🎮 Loaded arsenal: %zu weapons, capacity %zu", 
               loaded_arsenal->count, loaded_arsenal->capacity);
    
    // Iterate through loaded weapons
    for (size_t i = 0; i < loaded_arsenal->count; i++) {
        Weapon_t* weapon = (Weapon_t*)d_IndexDataFromStaticArray(loaded_arsenal, i);
        d_LogInfoF("  Weapon %d: %s (Damage: %d, Durability: %.1f%%)", 
                   weapon->weapon_id, weapon->name, weapon->damage, weapon->durability);
    }
    
    d_DestroyStaticArray(loaded_arsenal);
} else {
    d_LogError("❌ Failed to load arsenal!");
}
```

**Static Table Persistence**

Static tables can also be saved and loaded, preserving both keys and values along with hash table structure:

```c
// Create entity stats table
dStaticTable_t* entity_stats = /* ... initialize with data ... */;

// Save the entire lookup table
if (d_SaveStaticTableToFile("entity_database.dat", entity_stats) == 0) {
    d_LogInfo("🏰 Entity database saved successfully!");
}

// Load it back later
dStaticTable_t* loaded_stats = d_LoadStaticTableFromFile("entity_database.dat", entity_hash, int_compare);
if (loaded_stats) {
    d_LogInfo("📊 Entity database restored!");
    
    // Verify data integrity with iterator
    typedef struct { int entities_verified; } VerifyContext_t;
    
    void verify_entity(const void* key, size_t key_size, const void* value, size_t value_size, void* user_data) {
        (void)key_size; (void)value_size;
        VerifyContext_t* ctx = (VerifyContext_t*)user_data;
        int entity_id = *(int*)key;
        EntityStats_t* stats = (EntityStats_t*)value;
        
        if (stats->hp > 0 && stats->mp >= 0) {
            ctx->entities_verified++;
            d_LogDebugF("✅ Entity %d verified: HP=%d, MP=%d", entity_id, stats->hp, stats->mp);
        }
    }
    
    VerifyContext_t verify = {0};
    d_IterateStaticTable(loaded_stats, verify_entity, &verify);
    d_LogInfoF("🔍 Verified %d entities from saved database", verify.entities_verified);
    
    d_DestroyStaticTable(&loaded_stats);
}
```

**Binary Format Details**

The binary file format is optimized for speed and includes validation:

- **Magic Number**: Identifies file type (0xDAEDDDCA for arrays, 0xDAEDDDCT for tables)
- **Version**: Format version for future compatibility
- **Metadata**: Capacity, count, element sizes
- **Data**: Raw binary data with full type preservation

**Advanced Persistence Features**

```c
// High-performance iterator for batch operations after loading
typedef struct { 
    int items_processed; 
    float total_value; 
} ProcessingContext_t;

void calculate_inventory_value(size_t index, const void* element, size_t element_size, void* user_data) {
    (void)element_size;
    ProcessingContext_t* ctx = (ProcessingContext_t*)user_data;
    const Weapon_t* weapon = (const Weapon_t*)element;
    
    // Calculate value based on damage and durability
    float weapon_value = weapon->damage * (weapon->durability / 100.0f) * 10.0f;
    ctx->total_value += weapon_value;
    ctx->items_processed++;
    
    d_LogDebugF("💰 Weapon %zu: %s worth %.2f gold", index, weapon->name, weapon_value);
}

// Process loaded data immediately
ProcessingContext_t processing = {0, 0.0f};
d_IterateStaticArray(loaded_arsenal, calculate_inventory_value, &processing);
d_LogInfoF("💎 Total arsenal value: %.2f gold (%d weapons)", 
           processing.total_value, processing.items_processed);
```

This persistence system ensures your game saves, configuration data, and runtime state can be reliably stored and restored with **zero data loss** and **maximum performance**.

## 🏛️ The Philosophy: Divine Craftsmanship

In the words of the master himself: *"Tools that work without their creator truly serve."* Every function in Daedalus is designed to operate with mechanical precision, requiring no intervention once deployed. These are not merely utilities—they are **architectural elements** for building digital worlds.

The Daedalus approach emphasizes:
- **🎯 Geometric Perfection**: Every calculation optimized for precision and performance
- **🔧 Master-Crafted APIs**: Interfaces designed by someone who understands the weight of abstraction
- **⚡ Lightning Performance**: Using techniques that would make ancient gods jealous
- **🏗️ Foundational Stability**: Building blocks that support entire digital civilizations
- **🌐 Modern Deployment**: C code that runs beautifully on the web through Emscripten

## 🎯 The MIDAS Connection

Daedalus serves as the mathematical foundation of the **MIDAS Collective**:

- **🧠 Metis**: Provides wisdom through scattered consciousness in the linter
- **⚡ Ixion**: Guards against memory corruption with eternal vigilance  
- **🏗️ Daedalus**: Crafts the mathematical tools that power everything
- **📐 Archimedes**: Builds user interfaces with geometric precision
- **🪨 Sisyphus**: Tests everything with persistent, joyful iteration

*Together, these tools transform the eternal struggle of programming into acts of digital divinity.*

## 🏗️ Built for the Modern Age

While honoring the classical principles of mathematical precision, Daedalus embraces the modern reality of web deployment. Every function is designed to compile efficiently to WebAssembly, bringing desktop-class mathematical performance to browser applications.

---

"From the forge of necessity comes tools of transcendence. These functions will outlive their creator, serving digital architects for generations yet to come." - The Testament of Daedalus
