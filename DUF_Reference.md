# DUF (Daedalus Unified Format) Reference

DUF is a simple, human-readable data format for game configuration and assets. It's designed to be easy to write by hand and straightforward to parse.

## Basic Syntax

### Values

DUF supports five basic types:

```duf
# Integers (64-bit signed)
health: 100
damage: -50

# Floats (double precision)
speed: 2.5
multiplier: 0.001

# Booleans
is_boss: true
active: false

# Strings (double quotes)
name: "Glitch Hound"
description: "A corrupted entity"

# Multi-line strings (triple quotes)
flavor: """
This is a multi-line string.
No need to escape "quotes" inside.
Great for descriptions and dialogue.
"""
```

### Tables

Tables group related data using the `@name { }` syntax. Each `@` defines a top-level entry:

```duf
# File: enemies.duf
@daemon {
    hp: 500
    damage: 25
    boss: false
}

@didact {
    hp: 1000
    damage: 50
    boss: true
}
```

### Arrays

Arrays hold ordered lists of values using `[ ]`:

```duf
@config {
    # Array of strings
    weapons: ["sword", "bow", "staff"]

    # Array of numbers
    damage_values: [10, 25, 50]

    # Array of booleans
    flags: [true, false, true]
}
```

### Nested Structures

Tables can contain other tables and arrays:

```duf
# File: enemies.duf
@glitch_hound {
    id: 999
    name: "Glitch Hound"

    # Nested table
    stats: {
        str: 12
        dex: 18
        int: 8
    }

    # Array of strings
    abilities: ["bite", "data_corruption", "phase_shift"]

    # Nested array of tables
    spawn_conditions: {
        min_level: 10
        zones: ["corrupted_sector", "data_vaults", "lost_archives"]
    }
}
```

## API Usage

DUF uses an AUF-style API where you get node pointers and access fields directly.

### Parsing Files

```c
dDUFValue_t* root = NULL;
dDUFError_t* err = d_DUFParseFile("config.duf", &root);

if (err != NULL) {
    printf("Parse error at %d:%d - %s\n",
           err->line, err->column, d_StringPeek(err->message));
    d_DUFErrorFree(err);
    return;
}
```

### Accessing Values

The core API is simple: get a node, check if it's NULL, then access its fields.

```c
// Load enemies.duf
dDUFValue_t* root = NULL;
d_DUFParseFile("enemies.duf", &root);

// Get the daemon enemy
dDUFValue_t* daemon = d_DUFGetObjectItem(root, "daemon");
if (daemon == NULL) {
    // Key doesn't exist
    return;
}

// Get daemon's HP
dDUFValue_t* hp = d_DUFGetObjectItem(daemon, "hp");
if (hp != NULL) {
    printf("Daemon HP: %lld\n", (long long)hp->value_int);
}

// Get daemon's boss status
dDUFValue_t* boss = d_DUFGetObjectItem(daemon, "boss");
if (boss != NULL) {
    printf("Is boss: %s\n", boss->value_int ? "true" : "false");
}
```

### Value Fields

Each `dDUFValue_t` node has these fields you can access directly:

- `int type` - The value type (D_DUF_INT, D_DUF_FLOAT, D_DUF_BOOL, D_DUF_STRING, D_DUF_TABLE, D_DUF_ARRAY)
- `char* string` - The key name (for table entries)
- `char* value_string` - String value
- `int64_t value_int` - Integer value (also used for booleans: 1=true, 0=false)
- `double value_double` - Float value

### Iterating Arrays

Arrays use a linked list structure. Iterate through children:

```c
// Get glitch_hound's abilities
dDUFValue_t* hound = d_DUFGetObjectItem(root, "glitch_hound");
dDUFValue_t* abilities = d_DUFGetObjectItem(hound, "abilities");

if (abilities != NULL && abilities->type == D_DUF_ARRAY) {
    dDUFValue_t* item = abilities->child;
    while (item != NULL) {
        printf("Ability: %s\n", item->value_string);
        item = item->next;
    }
}
```

### Iterating Tables

Tables work the same way - iterate through children and check the `string` field for keys:

```c
// Iterate through all enemies in the file
if (root != NULL && root->type == D_DUF_TABLE) {
    dDUFValue_t* enemy = root->child;
    while (enemy != NULL) {
        printf("Enemy: %s\n", enemy->string);  // enemy->string is the key (daemon, didact, etc)

        // Get HP for this enemy
        dDUFValue_t* hp = d_DUFGetObjectItem(enemy, "hp");
        if (hp != NULL) {
            printf("  HP: %lld\n", (long long)hp->value_int);
        }

        enemy = enemy->next;
    }
}
```

### Type Checking

Always check types before accessing fields:

```c
dDUFValue_t* didact = d_DUFGetObjectItem(root, "didact");
dDUFValue_t* damage = d_DUFGetObjectItem(didact, "damage");

if (damage != NULL) {
    if (damage->type == D_DUF_INT) {
        printf("Damage: %lld\n", (long long)damage->value_int);
    } else if (damage->type == D_DUF_FLOAT) {
        printf("Damage: %.2f\n", damage->value_double);
    }
}
```

### Cleanup

```c
d_DUFFree(root);
```

## Creating DUF Data

You can build DUF structures programmatically:

```c
// Create a table
dDUFValue_t* player = d_DUFCreateTable();

// Create values
dDUFValue_t* name = d_DUFCreateString("Hero");
name->string = strdup("name");

dDUFValue_t* level = d_DUFCreateInt(5);
level->string = strdup("level");

// Add to table (manual linked list building)
player->child = name;
name->next = level;
level->prev = name;

// Serialize to string
dString_t* output = d_DUFToString(player);
printf("%s\n", d_StringPeek(output));

// Write to file
d_DUFWriteFile(player, "player.duf");

// Cleanup
d_StringDestroy(output);
d_DUFFree(player);
```

## Comments

DUF supports hash-style comments:

```duf
# File: enemies.duf
# This file defines all enemy types in the game

@daemon {
    hp: 500  # Health points
    damage: 25
}
```

## Design Philosophy

DUF is intentionally simple:

- **Human-readable**: Easy to write and edit by hand
- **Predictable**: Simple rules, no magic behavior
- **Direct access**: No fallback functions - you handle NULL yourself
- **Explicit**: Type checking is your responsibility
- **Minimal**: Only what you need for game data

This keeps the parser small, the API straightforward, and gives you full control over how to handle missing or invalid data.
