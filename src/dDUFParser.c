/* dDUFParser.c - DUF recursive descent parser (AUF-style) */

#define _POSIX_C_SOURCE 200809L

#include "Daedalus.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// =============================================================================
// Internal Error Creation (with C source location for debugging)
// =============================================================================

/**
 * @brief Create an internal error with C source location for debugging
 *
 * Used for errors that occur in the parser infrastructure (not DUF parse errors).
 * Includes __FILE__ and __LINE__ to help debug library issues.
 */
static dDUFError_t* create_internal_error(const char* message, const char* file, int line)
{
    dDUFError_t* err = (dDUFError_t*)calloc(1, sizeof(dDUFError_t));
    if (err == NULL) {
        return NULL;
    }

    err->line = 0;      // Internal errors don't have DUF source location
    err->column = 0;
    err->message = d_StringInit();
    if (err->message != NULL) {
        d_StringFormat(err->message, "[Internal] %s (at %s:%d)", message, file, line);
    }

    return err;
}

#define DUF_INTERNAL_ERROR(msg) create_internal_error(msg, __FILE__, __LINE__)

// =============================================================================
// Parser State
// =============================================================================

typedef struct {
    dArray_t* tokens;
    size_t pos;
    Token_t* current;
} Parser_t;

// =============================================================================
// Error Creation
// =============================================================================

static dDUFError_t* parser_error(Parser_t* p, const char* message)
{
    dDUFError_t* err = (dDUFError_t*)calloc(1, sizeof(dDUFError_t));
    if (err == NULL) {
        return NULL;
    }

    if (p->current != NULL) {
        err->line = p->current->line;
        err->column = p->current->column;
    } else {
        err->line = 0;
        err->column = 0;
    }

    err->message = d_StringInit();
    if (err->message != NULL) {
        d_StringAppend(err->message, message, 0);
    }

    return err;
}

// =============================================================================
// Token Navigation
// =============================================================================

static Token_t* parser_peek(Parser_t* p)
{
    if (p->pos >= p->tokens->count) {
        return NULL;
    }

    Token_t** tok_ptr = (Token_t**)d_ArrayGet(p->tokens, p->pos);
    if (tok_ptr == NULL) {
        return NULL;
    }

    return *tok_ptr;
}

static Token_t* parser_advance(Parser_t* p)
{
    p->current = parser_peek(p);
    if (p->current != NULL) {
        p->pos++;
    }
    return p->current;
}

static bool parser_match(Parser_t* p, TokenType_t type)
{
    Token_t* tok = parser_peek(p);
    return (tok != NULL && tok->type == type);
}

static Token_t* parser_expect(Parser_t* p, TokenType_t type, dDUFError_t** err)
{
    Token_t* tok = parser_peek(p);

    if (tok == NULL || tok->type != type) {
        dString_t* msg = d_StringInit();
        d_StringFormat(msg, "Expected token type %d but got %d", type, tok ? (int)tok->type : -1);
        *err = parser_error(p, d_StringPeek(msg));
        d_StringDestroy(msg);
        return NULL;
    }

    return parser_advance(p);
}

// =============================================================================
// Helper - Add Child to Parent Node (AUF-style)
// =============================================================================

static void add_child(dDUFValue_t* parent, dDUFValue_t* child)
{
    if (parent == NULL || child == NULL) {
        return;
    }

    child->next = NULL;
    child->prev = NULL;

    if (parent->child == NULL) {
        // First child
        parent->child = child;
    } else {
        // Find last child and append
        dDUFValue_t* last = parent->child;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = child;
        child->prev = last;
    }
}

// =============================================================================
// Parsing Functions
// =============================================================================

// Forward declarations
static dDUFValue_t* parse_value(Parser_t* p, dDUFError_t** err);
static dDUFValue_t* parse_table(Parser_t* p, dDUFError_t** err);
static dDUFValue_t* parse_array(Parser_t* p, dDUFError_t** err);

static dDUFValue_t* parse_value(Parser_t* p, dDUFError_t** err)
{
    Token_t* tok = parser_peek(p);

    if (tok == NULL) {
        *err = parser_error(p, "Unexpected end of input");
        return NULL;
    }

    switch (tok->type) {
        case TOK_STRING:
            parser_advance(p);
            return d_DUFCreateString(d_StringPeek(tok->value));

        case TOK_NUMBER: {
            parser_advance(p);
            const char* num_str = d_StringPeek(tok->value);

            // Check if it's a float (contains '.')
            if (strchr(num_str, '.') != NULL) {
                char* endptr;
                double val = strtod(num_str, &endptr);
                if (endptr == num_str) {
                    *err = parser_error(p, "Invalid float literal");
                    return NULL;
                }
                return d_DUFCreateFloat(val);
            } else {
                char* endptr;
                long long val = strtoll(num_str, &endptr, 10);
                if (endptr == num_str) {
                    *err = parser_error(p, "Invalid integer literal");
                    return NULL;
                }
                return d_DUFCreateInt((int64_t)val);
            }
        }

        case TOK_BOOL: {
            parser_advance(p);
            const char* bool_str = d_StringPeek(tok->value);
            bool val = (strcmp(bool_str, "true") == 0);
            return d_DUFCreateBool(val);
        }

        case TOK_LBRACE:
            return parse_table(p, err);

        case TOK_LBRACKET:
            return parse_array(p, err);

        default:
            *err = parser_error(p, "Unexpected token in value position");
            return NULL;
    }
}

static dDUFValue_t* parse_array(Parser_t* p, dDUFError_t** err)
{
    // Consume '['
    if (parser_expect(p, TOK_LBRACKET, err) == NULL) {
        return NULL;
    }

    dDUFValue_t* array = d_DUFCreateArray();
    if (array == NULL) {
        *err = parser_error(p, "Failed to create array");
        return NULL;
    }

    // Parse elements
    while (!parser_match(p, TOK_RBRACKET) && !parser_match(p, TOK_EOF)) {
        dDUFValue_t* elem = parse_value(p, err);
        if (*err != NULL) {
            d_DUFFree(array);
            return NULL;
        }

        // Add element as child
        add_child(array, elem);

        // Check for comma
        if (parser_match(p, TOK_COMMA)) {
            parser_advance(p);
        } else if (!parser_match(p, TOK_RBRACKET)) {
            *err = parser_error(p, "Expected ',' or ']' in array");
            d_DUFFree(array);
            return NULL;
        }
    }

    // Consume ']'
    if (parser_expect(p, TOK_RBRACKET, err) == NULL) {
        d_DUFFree(array);
        return NULL;
    }

    return array;
}

static dDUFValue_t* parse_table(Parser_t* p, dDUFError_t** err)
{
    // Consume '{'
    if (parser_expect(p, TOK_LBRACE, err) == NULL) {
        return NULL;
    }

    dDUFValue_t* table = d_DUFCreateTable();
    if (table == NULL) {
        *err = parser_error(p, "Failed to create table");
        return NULL;
    }

    // Parse key-value pairs
    while (!parser_match(p, TOK_RBRACE) && !parser_match(p, TOK_EOF)) {
        // Parse key (identifier)
        Token_t* key_tok = parser_expect(p, TOK_IDENTIFIER, err);
        if (*err != NULL) {
            d_DUFFree(table);
            return NULL;
        }

        const char* key = d_StringPeek(key_tok->value);

        // Expect ':'
        if (parser_expect(p, TOK_COLON, err) == NULL) {
            d_DUFFree(table);
            return NULL;
        }

        // Parse value
        dDUFValue_t* value = parse_value(p, err);
        if (*err != NULL) {
            d_DUFFree(table);
            return NULL;
        }

        // Set the key name on the value node
        value->string = strdup(key);
        if (value->string == NULL) {
            d_DUFFree(value);
            d_DUFFree(table);
            *err = parser_error(p, "Memory allocation failed");
            return NULL;
        }

        // Add value as child
        add_child(table, value);

        // No comma needed between entries in DUF
    }

    // Consume '}'
    if (parser_expect(p, TOK_RBRACE, err) == NULL) {
        d_DUFFree(table);
        return NULL;
    }

    return table;
}

static dDUFValue_t* parse_entry(Parser_t* p, dDUFError_t** err)
{
    // Expect '@'
    if (parser_expect(p, TOK_AT, err) == NULL) {
        return NULL;
    }

    // Expect entry name
    Token_t* name_tok = parser_expect(p, TOK_IDENTIFIER, err);
    if (*err != NULL) {
        return NULL;
    }

    const char* entry_name = d_StringPeek(name_tok->value);

    // Parse the table body
    dDUFValue_t* table = parse_table(p, err);
    if (*err != NULL) {
        return NULL;
    }

    // Set the entry name on the table
    table->string = strdup(entry_name);
    if (table->string == NULL) {
        d_DUFFree(table);
        *err = parser_error(p, "Memory allocation failed");
        return NULL;
    }

    return table;
}

static dDUFValue_t* parse_document(Parser_t* p, dDUFError_t** err)
{
    dDUFValue_t* root = d_DUFCreateTable();
    if (root == NULL) {
        *err = parser_error(p, "Failed to create root table");
        return NULL;
    }

    // Parse all @entry blocks
    while (!parser_match(p, TOK_EOF)) {
        if (parser_match(p, TOK_AT)) {
            dDUFValue_t* entry = parse_entry(p, err);
            if (*err != NULL) {
                d_DUFFree(root);
                return NULL;
            }

            // Add entry as child of root
            add_child(root, entry);
        } else {
            *err = parser_error(p, "Expected '@' at start of entry");
            d_DUFFree(root);
            return NULL;
        }
    }

    return root;
}

// =============================================================================
// Public API
// =============================================================================

dDUFError_t* d_DUFParseString(const char* content, dDUFValue_t** out_value)
{
    *out_value = NULL;

    if (content == NULL) {
        return DUF_INTERNAL_ERROR("NULL content provided");
    }

    // Tokenize
    dArray_t* tokens = d_DUFLex(content);
    if (tokens == NULL) {
        return DUF_INTERNAL_ERROR("Lexer failed");
    }

    // Check for lexer errors
    for (size_t i = 0; i < tokens->count; i++) {
        Token_t** tok_ptr = (Token_t**)d_ArrayGet(tokens, i);
        if (tok_ptr != NULL && *tok_ptr != NULL) {
            Token_t* tok = *tok_ptr;
            if (tok->type == TOK_ERROR) {
                dDUFError_t* err = (dDUFError_t*)calloc(1, sizeof(dDUFError_t));
                err->line = tok->line;
                err->column = tok->column;
                err->message = d_StringInit();
                d_StringAppend(err->message, d_StringPeek(tok->value), 0);
                d_DUFLexFree(tokens);
                return err;
            }
        }
    }

    // Parse
    Parser_t parser = {0};
    parser.tokens = tokens;
    parser.pos = 0;
    parser.current = NULL;

    dDUFError_t* err = NULL;
    *out_value = parse_document(&parser, &err);

    d_DUFLexFree(tokens);
    return err;
}

dDUFError_t* d_DUFParseFile(const char* filename, dDUFValue_t** out_value)
{
    *out_value = NULL;

    if (filename == NULL) {
        return DUF_INTERNAL_ERROR("NULL filename provided");
    }

    // Read file
    dString_t* content = d_StringCreateFromFile(filename);
    if (content == NULL) {
        dDUFError_t* err = create_internal_error("Failed to read file", __FILE__, __LINE__);
        if (err != NULL && err->message != NULL) {
            // Append filename to the error message
            d_StringAppend(err->message, ": ", 0);
            d_StringAppend(err->message, filename, 0);
        }
        return err;
    }

    // Parse content
    dDUFError_t* parse_err = d_DUFParseString(d_StringPeek(content), out_value);
    d_StringDestroy(content);

    return parse_err;
}
