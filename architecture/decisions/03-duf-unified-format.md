# ADR 03: DUF (Daedalus Unified Format) Design

**Status:** Active

## Context

The Daedalus library needed a data serialization format for game configuration files (enemies, items, levels, etc.). The format needed to be:

1. **Human-readable and writable** - Artists and designers should be able to edit files directly
2. **Supports nested structures** - Game data often has hierarchical relationships
3. **Multiple data types** - int64, float64, bool, string, arrays, tables
4. **Comments** - Configuration files benefit from inline documentation
5. **Multi-line strings** - Descriptions and dialogue need formatting
6. **Simple to implement** - No external dependencies, parse in pure C

## Decision

**Create a custom format called DUF (Daedalus Unified Format)** instead of using JSON, TOML, YAML, or other existing formats.

### Syntax Design

DUF uses a clean, minimal syntax:

```duf
# This is a comment

@entity_name {
    key: value
    nested: {
        inner_key: 123
        flag: true
    }
    list: [1, 2, 3]
    description: """
    Multi-line string
    with formatting preserved
    """
}
```

**Key syntax features:**
- `@name { }` - Top-level entries (sections)
- `key: value` - Key-value pairs (no commas needed between entries)
- `{ }` - Nested tables
- `[ ]` - Arrays (comma-separated)
- `"""..."""` - Multi-line strings
- `"..."` - Single-line strings with escape sequences (\n, \t, \", \\)
- `#` - Line comments
- Types: integers, floats (with `.`), booleans (`true`/`false`), strings

### Implementation Architecture

**Three-phase processing:**

1. **Lexer** ([dDUFLexer.c](../../src/dDUFLexer.c:299)) - Tokenization
   - State machine that converts characters into tokens
   - Handles: `@`, `{`, `}`, `[`, `]`, `:`, `,`, identifiers, strings, numbers, booleans
   - Multi-line string detection (`"""`)
   - Comment stripping (`#`)

2. **Parser** ([dDUFParser.c](../../src/dDUFParser.c:302)) - Recursive descent parsing
   - `parse_document()` → `parse_entry()` → `parse_table()` / `parse_array()` → `parse_value()`
   - Builds AST using dDUFValue_t nodes
   - Error tracking with line/column numbers
   - Predictable single-pass parsing

3. **Value System** ([dDUFValue.c](../../src/dDUFValue.c:13)) - AST nodes and memory management
   - Union-based value type: `dDUFValue_t`
   - Type enum: `D_DUF_TABLE`, `D_DUF_ARRAY`, `D_DUF_INT`, `D_DUF_FLOAT`, `D_DUF_BOOL`, `D_DUF_STRING`, `D_DUF_NULL`
   - Creation functions: `d_DUFCreate{Table,Array,Int,Float,Bool,String}()`
   - Recursive cleanup: `d_DUFFree()` walks the entire tree

**API Components:**

- **Parsing** ([dDUFParser.c](../../src/dDUFParser.c:364)): `d_DUFParseFile()`, `d_DUFParseString()`
- **Querying** ([dDUFQuery.c](../../src/dDUFQuery.c:125)): Path-based access with dot notation and array indices
  - `d_DUFGet(root, "enemy.stats.hp")` - Generic access
  - `d_DUFGetInt()`, `d_DUFGetFloat()`, `d_DUFGetBool()`, `d_DUFGetString()` - Typed access with fallbacks
  - Path syntax: `"player.items[0].name"` → `["player", 0, "name"]`
- **Serialization** ([dDUFIO.c](../../src/dDUFIO.c:180)): `d_DUFToString()`, `d_DUFWriteFile()`
  - Round-trip capable: Parse → Modify → Serialize → Parse again
  - Automatic multi-line string detection (if contains `\n`)
  - 4-space indentation for readability
- **Iteration** ([dDUFValue.c](../../src/dDUFValue.c:213)): `d_DUFTableForEach()`, `d_DUFArrayGet()`

## Alternatives Considered

### Option 1: JSON
**Pros:**
- Universal standard
- Many parsers available
- Wide tooling support

**Cons:**
- No comments (requires workarounds)
- Verbose syntax (quotes everywhere, commas required)
- No multi-line strings (need `\n` escaping)
- Not optimized for human editing
- Would need external dependency or large parser implementation

### Option 2: TOML
**Pros:**
- Human-friendly
- Comments supported
- Clear structure

**Cons:**
- Complex specification (harder to implement)
- Dotted keys can be confusing
- Array of tables syntax is verbose
- Would need external dependency

### Option 3: YAML
**Pros:**
- Very human-friendly
- Minimal syntax
- Comments supported

**Cons:**
- Whitespace-significant (error-prone)
- Complex specification (very hard to implement correctly)
- Many edge cases and gotchas
- Massive security attack surface (parsing complexity)
- Would need external dependency

### Option 4: Custom format (DUF) - **CHOSEN**
**Pros:**
- **Zero dependencies** - Pure C implementation
- **Simple to implement** - ~1000 lines total (lexer + parser + value + query + I/O)
- **Exactly what we need** - Tailored for game configuration use case
- **Complete control** - Can evolve format as needs change
- **Fast** - Single-pass parsing, minimal allocations
- **Predictable** - No hidden complexity or gotchas
- **Human-friendly** - Clean syntax, comments, multi-line strings

**Cons:**
- Not a standard format (no external tooling)
- Team needs to learn new syntax (though it's very simple)
- Responsibility for maintenance

## Consequences

### Positive

1. **No external dependencies** - Daedalus remains self-contained
2. **Small code footprint** - Entire DUF implementation is ~1000 lines
3. **Perfect fit for game configs** - Syntax designed specifically for this use case
4. **Easy to debug** - Simple implementation, clear error messages with line/column
5. **Fast parsing** - Single-pass recursive descent, no backtracking
6. **Complete control** - Can add features as needed (e.g., includes, references)

### Negative

1. **Learning curve** - Developers need to learn DUF syntax (mitigated by simplicity)
2. **No external tooling** - No syntax highlighting, linters, validators (yet)
3. **Maintenance burden** - We own the parser implementation
4. **Not standard** - Can't use generic tools or libraries

### Neutral

1. **Format evolution** - Can add features, but must maintain backward compatibility
2. **Documentation** - Need to document syntax clearly for users
3. **Testing** - Need comprehensive test suite (currently have edge_cases.duf and enemies.duf)

## Implementation Details

### Type System

```c
typedef enum {
    D_DUF_NULL,
    D_DUF_BOOL,
    D_DUF_INT,      // int64_t
    D_DUF_FLOAT,    // double
    D_DUF_STRING,   // dString_t*
    D_DUF_ARRAY,    // dArray_t* of dDUFValue_t*
    D_DUF_TABLE     // dTable_t with string keys, dDUFValue_t* values
} dDUFType_t;

typedef struct dDUFValue_t {
    dDUFType_t type;
    union {
        bool bool_val;
        int64_t int_val;
        double float_val;
        dString_t* string_val;
        dArray_t* array_val;
        dTable_t* table_val;
    };
} dDUFValue_t;
```

### Error Handling

```c
typedef struct {
    int line;
    int column;
    dString_t* message;
} dDUFError_t;
```

Parsing returns `NULL` error on success, or `dDUFError_t*` with location and message on failure.

### Query Path Syntax

- Simple key: `"player"`
- Nested key: `"player.health"`
- Array index: `"enemies[0]"`
- Complex path: `"level.enemies[2].drops[0].item_id"`

### Example Usage

```c
// Parse file
dDUFValue_t* data = NULL;
dDUFError_t* err = d_DUFParseFile("config/enemies.duf", &data);
if (err != NULL) {
    printf("Parse error at %d:%d - %s\n",
           err->line, err->column, d_StringPeek(err->message));
    d_DUFErrorFree(err);
    return;
}

// Query with fallback values
int64_t hp = d_DUFGetInt(data, "didact.hp", 100);
bool is_boss = d_DUFGetBool(data, "didact.is_boss", false);
const char* name = d_DUFGetString(data, "didact.name", "Unknown");

// Iterate over table
void print_enemy(const char* key, dDUFValue_t* val, void* ctx) {
    printf("Enemy: %s\n", key);
}
d_DUFTableForEach(data, print_enemy, NULL);

// Cleanup
d_DUFFree(data);
```

## Future Extensions

Potential additions to DUF (not yet implemented):

1. **File includes** - `@include "common.duf"` for shared configs
2. **References** - `ref: @other_entity` for cross-references
3. **Inheritance** - `@entity : base_entity { }` for config reuse
4. **Type hints** - Optional type annotations for validation
5. **Macros/variables** - `$MAX_HP` for reusable values

## Related Decisions

- **ADR 00**: Core Data Structure Design - DUF uses void* pattern (dTable_t, dArray_t)
- **ADR 01**: Dynamic vs Static Variants - DUF uses dynamic containers for flexibility

## References

- Implementation: [src/dDUF*.c](../../src/)
- Tests: [tests/test_duf.c](../../tests/test_duf.c), [tests/test_data/enemies.duf](../../tests/test_data/enemies.duf)
- API: [include/Daedalus.h](../../include/Daedalus.h) (search for `d_DUF`)
