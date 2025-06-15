# Daedalus

A high-performance C library providing linear algebra operations, data structures, and utility functions for game development, scientific computing, and real-time applications.

## Overview

Daedalus is a comprehensive math and utility library written in C that offers efficient implementations of vector mathematics, matrix operations, spatial data structures, and common programming utilities. The library is designed with performance in mind, featuring optimized algorithms like Quake's fast square root and providing both 2D and 3D mathematical operations.

## Features

### String Utilities
- **String Builder**: Efficient string construction and manipulation
- **File Operations**: Read entire files into strings
- **Safe String Operations**: Secure string handling with bounds checking

## RPG Development with Daedalus String Builder

The Daedalus string builder is particularly well-suited for RPG development, where dynamic text generation is crucial for creating immersive narratives, dialogue systems, and procedural content. Here are practical examples for common RPG scenarios:

### Character Description Generator

```c
#include <Daedalus.h>

void generate_character_description(const char* name, const char* class, int level, int health) {
    dString_t* desc = d_StringCreate();

    d_StringAddStr(desc, "The legendary ", 0);
    d_StringAddStr(desc, name, 0);
    d_StringAddStr(desc, " stands before you - a level ", 0);
    d_StringAddInt(desc, level);
    d_StringAddChar(desc, ' ');
    d_StringAddStr(desc, class, 0);
    d_StringAddStr(desc, " with ", 0);
    d_StringAddInt(desc, health);
    d_StringAddStr(desc, " health points remaining.", 0);

    printf("%s\n", d_StringPeek(desc));
    d_StringDestroy(desc);
}
```

### Combat Log Generator

```c
#include <Daedalus.h>

void generate_combat_log(const char* attacker, const char* target,
                        const char* skill, int damage, int critical) {
    dString_t* log = d_StringCreate();

    d_StringAddStr(log, "[COMBAT] ", 0);
    d_StringAddStr(log, attacker, 0);
    d_StringAddStr(log, " uses ", 0);
    d_StringAddStr(log, skill, 0);
    d_StringAddStr(log, " on ", 0);
    d_StringAddStr(log, target, 0);

    if (critical) {
        d_StringAddStr(log, " for a CRITICAL ", 0);
    } else {
        d_StringAddStr(log, " for ", 0);
    }

    d_StringAddInt(log, damage);
    d_StringAddStr(log, " damage!", 0);

    printf("%s\n", d_StringPeek(log));
    d_StringDestroy(log);
}
```
