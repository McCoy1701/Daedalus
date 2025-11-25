# ADR 00: Core Data Structure Design Philosophy

**Status:** Active

## Context

Daedalus is a foundational C library providing data structures, math utilities, and game development tools. The library must establish a consistent design philosophy for memory management, type safety, and API ergonomics across all data structures (arrays, hash tables, strings, linked lists). C's lack of generics and RAII requires explicit design decisions about ownership, initialization patterns, and error handling.

**Options Considered:**

1. **Macro-based generics** - Use C macros to generate type-specific implementations (like C++ templates)
2. **void* with manual size tracking** - Store opaque data with `element_size` parameter, user casts on retrieval
3. **Typed wrapper structs** - Generate concrete types for common use cases (IntArray, FloatArray, etc.)
4. **Header-only template approach** - Single header file with inline implementations using preprocessor

## Decision

Use **void* with manual size tracking** (Option 2) with a consistent initialization/destruction pattern.

All data structures follow these principles:

**Memory Management:**
- Heap allocation required (`_Init()` functions return pointers, never stack objects)
- Explicit destruction required (`_Destroy()` functions, never automatic)
- Caller owns data until transferred (arrays/tables copy or take ownership explicitly)
- Clear ownership semantics documented per-function

**API Pattern:**
```c
dArray_t* d_ArrayInit(size_t capacity, size_t element_size);
void* d_ArrayGet(dArray_t* array, size_t index);
void d_ArrayAppend(dArray_t* array, void* element);
void d_ArrayDestroy(dArray_t* array);
```

**Type Safety:**
- `element_size` stored in structure, checked on operations
- Getters return `void*`, requiring explicit cast by caller
- No type checking at compile time (impossible without generics)
- Runtime size validation prevents buffer overruns

**Prefix Convention:**
- All symbols prefixed with `d_` (namespace simulation)
- Types suffixed with `_t` (dArray_t, dTable_t, dVec3_t)
- Prevents collisions with user code and other libraries

**Justification**:

1. **Maximum flexibility** - Works with any data type (primitives, structs, pointers) without code generation
2. **Predictable memory layout** - `element_size * count` gives exact memory usage, crucial for embedded/game dev
3. **Cross-platform compatibility** - No macro magic that breaks on different compilers (works with GCC, Emscripten)
4. **Performance** - Contiguous memory layout for cache-friendly iteration, no vtable overhead
5. **Debugger-friendly** - Real struct definitions visible in GDB/LLDB, not macro-generated symbols

## Consequences

**Positive:**
- Single implementation supports all types (reduces code size)
- Consistent API across all data structures (learn once, apply everywhere)
- Explicit memory management matches C idioms (no "magic" behavior)
- Works with Emscripten/WebAssembly without modifications
- Easy to reason about memory ownership at API boundaries
- Library users familiar with C patterns can use immediately

**Negative:**
- No compile-time type safety - wrong cast = undefined behavior
- Requires manual element_size calculation (use `sizeof()`)
- More verbose than C++ containers (`std::vector<int>` vs `d_ArrayInit(10, sizeof(int))`)
- Users must remember to call `_Destroy()` functions (no RAII)
- Double-pointer patterns for pointer storage can confuse beginners

**Accepted Trade-offs:**
- Type safety loss acceptable in C context (idiomatic for C libraries)
- Verbosity trade-off for portability and predictability (works everywhere C works)
- Manual memory management is explicit design goal (users need deterministic control for games/embedded)
- Library documentation and examples compensate for learning curve
- Python-like helper functions (string manipulation, formatters) provide ergonomic sugar layer

## Implementation Examples

**Storing Primitives:**
```c
dArray_t* scores = d_ArrayInit(10, sizeof(int));
int value = 42;
d_ArrayAppend(scores, &value);
int* retrieved = (int*)d_ArrayGet(scores, 0);
d_ArrayDestroy(scores);
```

**Storing Structs:**
```c
typedef struct { int hp; float speed; } Enemy;
dArray_t* enemies = d_ArrayInit(10, sizeof(Enemy));
Enemy goblin = {.hp = 50, .speed = 2.5};
d_ArrayAppend(enemies, &goblin);
Enemy* ptr = (Enemy*)d_ArrayGet(enemies, 0);
d_ArrayDestroy(enemies);
```

**Storing Pointers (Double-Pointer Pattern):**
```c
dArray_t* strings = d_ArrayInit(10, sizeof(dString_t*));
dString_t* str = d_StringInit();
d_StringAppend(str, "Hello", 0);
d_ArrayAppend(strings, &str);  // Store the pointer itself
dString_t** str_ptr = (dString_t**)d_ArrayGet(strings, 0);
d_StringDestroy(*str_ptr);
d_ArrayDestroy(strings);
```

## Related Decisions

- ADR 01: Dynamic vs Static Array Variants (capacity management)
- ADR 02: Hash Table Implementation Strategy (collision resolution, hashing)
- ADR 03: DUF Data Format Design (builds on tables/arrays/strings)
