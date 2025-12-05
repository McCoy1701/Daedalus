/* dDUFIO.c - DUF serialization and file I/O (AUF-style) */

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

    // Iterate over children
    dDUFValue_t* child = array->child;
    while (child != NULL) {
        serialize_value(child, out, indent_level);

        if (child->next != NULL) {
            d_StringAppend(out, ", ", 0);
        }

        child = child->next;
    }

    d_StringAppendChar(out, ']');
}

static void serialize_table(dDUFValue_t* table, dString_t* out, int indent_level)
{
    d_StringAppend(out, "{\n", 0);

    // Iterate over children
    dDUFValue_t* child = table->child;
    while (child != NULL) {
        write_indent(out, indent_level + 1);

        // Write key
        if (child->key != NULL) {
            d_StringAppend(out, child->key, 0);
            d_StringAppend(out, ": ", 0);
        }

        // Write value
        serialize_value(child, out, indent_level + 1);
        d_StringAppendChar(out, '\n');

        child = child->next;
    }

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
            d_StringAppend(out, val->value_int ? "true" : "false", 0);
            break;

        case D_DUF_INT:
            d_StringFormat(out, "%lld", (long long)val->value_int);
            break;

        case D_DUF_FLOAT:
            d_StringFormat(out, "%g", val->value_double);
            break;

        case D_DUF_STRING:
            if (val->value_string != NULL) {
                serialize_string(val->value_string, out);
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

    // If root is a table, serialize each child with @ prefix
    if (root->type == D_DUF_TABLE) {
        dDUFValue_t* child = root->child;
        while (child != NULL) {
            d_StringAppendChar(out, '@');
            if (child->key != NULL) {
                d_StringAppend(out, child->key, 0);
            }
            d_StringAppend(out, " ", 0);

            // Serialize the table value
            if (child->type == D_DUF_TABLE) {
                serialize_table(child, out, 0);
            }

            d_StringAppend(out, "\n\n", 0);

            child = child->next;
        }
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
