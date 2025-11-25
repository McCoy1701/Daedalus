/* dDUFQuery.c - DUF path resolution and query functions */

#define _POSIX_C_SOURCE 200809L

#include "Daedalus.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// =============================================================================
// Path Tokenization
// =============================================================================

typedef enum {
    PATH_TOKEN_KEY,     // table key (e.g., "player")
    PATH_TOKEN_INDEX    // array index (e.g., "[0]")
} PathTokenType_t;

typedef struct {
    PathTokenType_t type;
    union {
        char* key;          // For PATH_TOKEN_KEY
        size_t index;       // For PATH_TOKEN_INDEX
    };
} PathToken_t;

// Parse path into tokens
// Examples: "player.health" -> ["player", "health"]
//          "enemies[0].name" -> ["enemies", 0, "name"]
static dArray_t* parse_path(const char* path)
{
    if (path == NULL || *path == '\0') {
        return NULL;
    }

    dArray_t* tokens = d_ArrayInit(4, sizeof(PathToken_t));
    if (tokens == NULL) {
        return NULL;
    }

    const char* p = path;
    dString_t* current = d_StringInit();

    while (*p != '\0') {
        if (*p == '.') {
            // End of key token
            if (d_StringGetLength(current) > 0) {
                PathToken_t tok;
                tok.type = PATH_TOKEN_KEY;
                tok.key = strdup(d_StringPeek(current));
                d_ArrayAppend(tokens, &tok);
                d_StringClear(current);
            }
            p++;
        } else if (*p == '[') {
            // End of key, start of index
            if (d_StringGetLength(current) > 0) {
                PathToken_t tok;
                tok.type = PATH_TOKEN_KEY;
                tok.key = strdup(d_StringPeek(current));
                d_ArrayAppend(tokens, &tok);
                d_StringClear(current);
            }

            // Parse index
            p++; // skip '['
            size_t index = 0;
            while (*p != '\0' && isdigit(*p)) {
                index = index * 10 + (*p - '0');
                p++;
            }

            if (*p != ']') {
                // Invalid index
                d_StringDestroy(current);
                d_ArrayDestroy(tokens);
                return NULL;
            }

            PathToken_t tok;
            tok.type = PATH_TOKEN_INDEX;
            tok.index = index;
            d_ArrayAppend(tokens, &tok);

            p++; // skip ']'
        } else {
            // Regular character
            d_StringAppendChar(current, *p);
            p++;
        }
    }

    // Add final key if any
    if (d_StringGetLength(current) > 0) {
        PathToken_t tok;
        tok.type = PATH_TOKEN_KEY;
        tok.key = strdup(d_StringPeek(current));
        d_ArrayAppend(tokens, &tok);
    }

    d_StringDestroy(current);
    return tokens;
}

static void free_path_tokens(dArray_t* tokens)
{
    if (tokens == NULL) {
        return;
    }

    for (size_t i = 0; i < tokens->count; i++) {
        PathToken_t* tok = (PathToken_t*)d_ArrayGet(tokens, i);
        if (tok != NULL && tok->type == PATH_TOKEN_KEY && tok->key != NULL) {
            free(tok->key);
        }
    }

    d_ArrayDestroy(tokens);
}

// =============================================================================
// Path Resolution
// =============================================================================

dDUFValue_t* d_DUFGet(dDUFValue_t* root, const char* path)
{
    if (root == NULL || path == NULL) {
        return NULL;
    }

    dArray_t* tokens = parse_path(path);
    if (tokens == NULL) {
        return NULL;
    }

    dDUFValue_t* current = root;

    for (size_t i = 0; i < tokens->count; i++) {
        PathToken_t* tok = (PathToken_t*)d_ArrayGet(tokens, i);
        if (tok == NULL || current == NULL) {
            free_path_tokens(tokens);
            return NULL;
        }

        if (tok->type == PATH_TOKEN_KEY) {
            // Navigate into table
            if (current->type != D_DUF_TABLE) {
                free_path_tokens(tokens);
                return NULL;
            }

            current = d_DUFTableGet(current, tok->key);
        } else if (tok->type == PATH_TOKEN_INDEX) {
            // Navigate into array
            if (current->type != D_DUF_ARRAY) {
                free_path_tokens(tokens);
                return NULL;
            }

            current = d_DUFArrayGet(current, tok->index);
        }
    }

    free_path_tokens(tokens);
    return current;
}

// =============================================================================
// Typed Getters
// =============================================================================

int64_t d_DUFGetInt(dDUFValue_t* root, const char* path, int64_t fallback)
{
    dDUFValue_t* val = d_DUFGet(root, path);

    if (val == NULL || val->type != D_DUF_INT) {
        return fallback;
    }

    return val->int_val;
}

double d_DUFGetFloat(dDUFValue_t* root, const char* path, double fallback)
{
    dDUFValue_t* val = d_DUFGet(root, path);

    if (val == NULL) {
        return fallback;
    }

    if (val->type == D_DUF_FLOAT) {
        return val->float_val;
    }

    // Allow implicit int->float conversion
    if (val->type == D_DUF_INT) {
        return (double)val->int_val;
    }

    return fallback;
}

bool d_DUFGetBool(dDUFValue_t* root, const char* path, bool fallback)
{
    dDUFValue_t* val = d_DUFGet(root, path);

    if (val == NULL || val->type != D_DUF_BOOL) {
        return fallback;
    }

    return val->bool_val;
}

const char* d_DUFGetString(dDUFValue_t* root, const char* path, const char* fallback)
{
    dDUFValue_t* val = d_DUFGet(root, path);

    if (val == NULL || val->type != D_DUF_STRING) {
        return fallback;
    }

    if (val->string_val == NULL) {
        return fallback;
    }

    return d_StringPeek(val->string_val);
}
