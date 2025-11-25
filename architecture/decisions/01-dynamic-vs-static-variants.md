# ADR 01: Dynamic vs Static Array Variants

**Status:** Active

## Context

Daedalus provides data structures for game development and systems programming where performance and memory predictability are critical. Arrays are fundamental containers, but different use cases have different requirements:

- **Game loops**: Need predictable frame times (no surprise allocations during gameplay)
- **Resource loading**: Can tolerate dynamic growth (loading phase is separate from gameplay)
- **Embedded systems**: May have strict memory constraints (no dynamic allocation allowed)
- **Web/WASM**: Heap allocations can be expensive (prefer pre-allocated buffers)

**Options Considered:**

1. **Dynamic arrays only** - Auto-growing like C++ `std::vector`, always heap-allocated
2. **Static arrays only** - Fixed capacity, fail when full (like C arrays with bounds checking)
3. **Both variants** - Separate types for dynamic (`dArray_t`) and static (`dStaticArray_t`)
4. **Configurable growth policy** - Single type with compile-time or runtime growth strategies

## Decision

Use **both variants** (Option 3) with clear naming to indicate behavior.

**Dynamic Arrays (`dArray_t`):**
- Automatically grow when capacity is exceeded
- Use `realloc()` to resize (may move memory)
- Trade-off: Unpredictable allocation timing for convenience
- Best for: Loading, preprocessing, editor tools

**Static Arrays (`dStaticArray_t`):**
- Fixed capacity set at initialization
- Return error when attempting to exceed capacity
- Trade-off: Must know upper bound for predictability
- Best for: Gameplay loops, real-time systems, embedded

**API Pattern:**
```c
// Dynamic - grows automatically
dArray_t* enemies = d_ArrayInit(10, sizeof(Enemy));
d_ArrayAppend(enemies, &enemy);  // Always succeeds (or OOM)

// Static - fixed capacity
dStaticArray_t* bullets = d_InitStaticArray(100, sizeof(Bullet));
int result = d_StaticArrayAppend(bullets, &bullet);
if (result != 0) {
    // Handle capacity exceeded
}
```

**Justification**:

1. **Explicit trade-off selection** - Name makes behavior obvious at call site
2. **Zero-cost when not needed** - Static variant has no growth overhead
3. **Predictable performance** - Static variant never triggers realloc during critical sections
4. **Memory debugging** - Easy to detect which arrays might cause fragmentation
5. **Compatibility** - Dynamic variant matches std::vector behavior (familiar to C++ developers)

## Consequences

**Positive:**
- Clear names document performance characteristics
- Developers choose appropriate type for use case
- Static variant enables real-time guarantees
- Both share common patterns (Init, Append, Get, Destroy)
- Testing can verify static arrays never allocate after init

**Negative:**
- Code duplication (two implementations of similar functionality)
- Learning curve (must understand difference between variants)
- Migration cost (changing dArray_t to dStaticArray_t requires error handling)
- Both types in codebase increases cognitive load

**Accepted Trade-offs:**
- Code duplication acceptable for performance clarity
- Two types better than runtime configuration (compile-time is cheaper)
- Migration cost is one-time, performance benefit is ongoing
- Naming convention (`dArray_t` vs `dStaticArray_t`) makes distinction clear

## Implementation Details

**Shared Characteristics:**
- Both use `element_size` for type flexibility
- Both use `count` for current elements
- Both use `capacity` for maximum elements (static's never changes)
- Both return `void*` from Get operations

**Key Differences:**

| Feature | Dynamic (`dArray_t`) | Static (`dStaticArray_t`) |
|---------|---------------------|--------------------------|
| Capacity | Grows (typically 2x) | Fixed at init |
| Append behavior | Always succeeds* | Can fail if full |
| Realloc during runtime | Yes | Never |
| File persistence | Count/capacity stored | Count/capacity/data stored |
| Use case | Loading, preprocessing | Gameplay, real-time |

*Except out-of-memory

## Related Decisions

- ADR 00: Core Data Structure Design (establishes void* pattern)
- ADR 02: Hash Table Implementation (similar dynamic/static split for tables)
