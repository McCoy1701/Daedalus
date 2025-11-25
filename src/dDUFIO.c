/* dDUFIO.c - DUF serialization and file I/O */

#include "Daedalus.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// =============================================================================
// Serialization Helpers
// =============================================================================

static void serialize_value(dDUFValue_t* val, dString_t* out, int indent_level);

static void write_indent(dString_t* out, int level)
{
    for (int i = 0; i < level * 4; i++) {
        d_StringAppendChar(out, ' ');
    }
}

static void serialize_string(const char* str, dString_t* out)
{
    // Check if string needs multi-line format
    bool has_newline = (strchr(str, '\n') != NULL);

    if (has_newline) {
        // Multi-line string
        d_StringAppend(out, "\"\"\"", 0);
        d_StringAppend(out, str, 0);
        d_StringAppend(out, "\"\"\"", 0);
    } else {
        // Single-line string with escaping
        d_StringAppendChar(out, '"');

        const char* p = str;
        while (*p != '\0') {
            switch (*p) {
                case '"':
                    d_StringAppend(out, "\\\"", 0);
                    break;
                case '\\':
                    d_StringAppend(out, "\\\\", 0);
                    break;
                case '\t':
                    d_StringAppend(out, "\\t", 0);
                    break;
                case '\n':
                    d_StringAppend(out, "\\n", 0);
                    break;
                default:
                    d_StringAppendChar(out, *p);
                    break;
            }
            p++;
        }

        d_StringAppendChar(out, '"');
    }
}

static void serialize_array(dDUFValue_t* array, dString_t* out, int indent_level)
{
    d_StringAppendChar(out, '[');

    size_t len = d_DUFArrayLength(array);
    for (size_t i = 0; i < len; i++) {
        dDUFValue_t* elem = d_DUFArrayGet(array, i);
        if (elem != NULL) {
            serialize_value(elem, out, indent_level);

            if (i < len - 1) {
                d_StringAppend(out, ", ", 0);
            }
        }
    }

    d_StringAppendChar(out, ']');
}

// Helper context for table serialization
typedef struct {
    dString_t* out;
    int indent_level;
    bool first;
} SerializeTableContext_t;

static void serialize_table_entry(const char* key, dDUFValue_t* val, void* user_data)
{
    SerializeTableContext_t* ctx = (SerializeTableContext_t*)user_data;

    write_indent(ctx->out, ctx->indent_level);
    d_StringAppend(ctx->out, key, 0);
    d_StringAppend(ctx->out, ": ", 0);
    serialize_value(val, ctx->out, ctx->indent_level);
    d_StringAppendChar(ctx->out, '\n');
}

static void serialize_table(dDUFValue_t* table, dString_t* out, int indent_level)
{
    d_StringAppend(out, "{\n", 0);

    SerializeTableContext_t ctx;
    ctx.out = out;
    ctx.indent_level = indent_level + 1;
    ctx.first = true;

    d_DUFTableForEach(table, serialize_table_entry, &ctx);

    write_indent(out, indent_level);
    d_StringAppendChar(out, '}');
}

static void serialize_value(dDUFValue_t* val, dString_t* out, int indent_level)
{
    if (val == NULL) {
        d_StringAppend(out, "null", 0);
        return;
    }

    switch (val->type) {
        case D_DUF_NULL:
            d_StringAppend(out, "null", 0);
            break;

        case D_DUF_BOOL:
            d_StringAppend(out, val->bool_val ? "true" : "false", 0);
            break;

        case D_DUF_INT:
            d_StringFormat(out, "%lld", (long long)val->int_val);
            break;

        case D_DUF_FLOAT:
            d_StringFormat(out, "%g", val->float_val);
            break;

        case D_DUF_STRING:
            if (val->string_val != NULL) {
                serialize_string(d_StringPeek(val->string_val), out);
            } else {
                d_StringAppend(out, "\"\"", 0);
            }
            break;

        case D_DUF_ARRAY:
            serialize_array(val, out, indent_level);
            break;

        case D_DUF_TABLE:
            serialize_table(val, out, indent_level);
            break;
    }
}

// Helper context for root table serialization (entries with @)
typedef struct {
    dString_t* out;
} SerializeRootContext_t;

static void serialize_root_entry(const char* key, dDUFValue_t* val, void* user_data)
{
    SerializeRootContext_t* ctx = (SerializeRootContext_t*)user_data;

    d_StringAppendChar(ctx->out, '@');
    d_StringAppend(ctx->out, key, 0);
    d_StringAppend(ctx->out, " ", 0);

    // Serialize the table value
    if (val != NULL && val->type == D_DUF_TABLE) {
        serialize_table(val, ctx->out, 0);
    }

    d_StringAppend(ctx->out, "\n\n", 0);
}

// =============================================================================
// Public API
// =============================================================================

dString_t* d_DUFToString(dDUFValue_t* root)
{
    if (root == NULL) {
        return NULL;
    }

    dString_t* out = d_StringInit();
    if (out == NULL) {
        return NULL;
    }

    // If root is a table, serialize each entry with @ prefix
    if (root->type == D_DUF_TABLE) {
        SerializeRootContext_t ctx;
        ctx.out = out;
        d_DUFTableForEach(root, serialize_root_entry, &ctx);
    } else {
        // Single value (not typical for DUF files)
        serialize_value(root, out, 0);
    }

    return out;
}

int d_DUFWriteFile(dDUFValue_t* root, const char* filename)
{
    if (root == NULL || filename == NULL) {
        return -1;
    }

    dString_t* content = d_DUFToString(root);
    if (content == NULL) {
        return -1;
    }

    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        d_StringDestroy(content);
        return -1;
    }

    size_t written = fwrite(d_StringPeek(content), 1, d_StringGetLength(content), f);
    fclose(f);

    d_StringDestroy(content);

    if (written != d_StringGetLength(content)) {
        return -1;
    }

    return 0;
}
