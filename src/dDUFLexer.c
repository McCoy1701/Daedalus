/* dDUFLexer.c - DUF lexical analysis (tokenization) */

#include "Daedalus.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// =============================================================================
// Token Types
// =============================================================================

typedef enum {
    TOK_EOF,           // End of file
    TOK_AT,            // @
    TOK_LBRACE,        // {
    TOK_RBRACE,        // }
    TOK_LBRACKET,      // [
    TOK_RBRACKET,      // ]
    TOK_COLON,         // :
    TOK_COMMA,         // ,
    TOK_IDENTIFIER,    // name, key
    TOK_STRING,        // "string" or """multiline"""
    TOK_NUMBER,        // 123 or 1.5
    TOK_BOOL,          // true or false
    TOK_ERROR          // Lexical error
} TokenType_t;

typedef struct {
    TokenType_t type;
    dString_t* value;
    int line;
    int column;
} Token_t;

// =============================================================================
// Lexer State
// =============================================================================

typedef struct {
    const char* input;
    size_t pos;
    size_t len;
    int line;
    int column;
    dArray_t* tokens;     // Array of Token_t*
} Lexer_t;

// =============================================================================
// Helper Functions
// =============================================================================

static char lexer_peek(Lexer_t* lex)
{
    if (lex->pos >= lex->len) {
        return '\0';
    }
    return lex->input[lex->pos];
}

static char lexer_advance(Lexer_t* lex)
{
    if (lex->pos >= lex->len) {
        return '\0';
    }

    char c = lex->input[lex->pos++];

    if (c == '\n') {
        lex->line++;
        lex->column = 1;
    } else {
        lex->column++;
    }

    return c;
}

static void lexer_skip_whitespace(Lexer_t* lex)
{
    while (isspace(lexer_peek(lex))) {
        lexer_advance(lex);
    }
}

static void lexer_skip_comment(Lexer_t* lex)
{
    if (lexer_peek(lex) == '#') {
        // Skip until end of line
        while (lexer_peek(lex) != '\n' && lexer_peek(lex) != '\0') {
            lexer_advance(lex);
        }
    }
}

static Token_t* token_create(TokenType_t type, const char* value, int line, int column)
{
    Token_t* tok = (Token_t*)calloc(1, sizeof(Token_t));
    if (tok == NULL) {
        return NULL;
    }

    tok->type = type;
    tok->line = line;
    tok->column = column;
    tok->value = d_StringInit();

    if (tok->value == NULL) {
        free(tok);
        return NULL;
    }

    if (value != NULL) {
        d_StringAppend(tok->value, value, 0);
    }

    return tok;
}

static void token_destroy(Token_t* tok)
{
    if (tok == NULL) {
        return;
    }

    if (tok->value != NULL) {
        d_StringDestroy(tok->value);
    }

    free(tok);
}

// =============================================================================
// Tokenization Functions
// =============================================================================

static Token_t* lexer_read_string(Lexer_t* lex)
{
    int start_line = lex->line;
    int start_column = lex->column;
    dString_t* str = d_StringInit();

    if (str == NULL) {
        return token_create(TOK_ERROR, "Memory allocation failed", start_line, start_column);
    }

    // Check for multi-line string (""")
    bool is_multiline = false;
    if (lexer_peek(lex) == '"') {
        lexer_advance(lex); // consume first "

        if (lexer_peek(lex) == '"') {
            lexer_advance(lex); // consume second "
            if (lexer_peek(lex) == '"') {
                lexer_advance(lex); // consume third "
                is_multiline = true;
            } else {
                // Just an empty string ""
                Token_t* tok = token_create(TOK_STRING, "", start_line, start_column);
                d_StringDestroy(str);
                return tok;
            }
        }
    }

    // Read string content
    if (is_multiline) {
        // Multi-line string: read until """
        while (lexer_peek(lex) != '\0') {
            if (lexer_peek(lex) == '"') {
                size_t saved_pos = lex->pos;
                int saved_line = lex->line;
                int saved_col = lex->column;

                lexer_advance(lex);
                if (lexer_peek(lex) == '"') {
                    lexer_advance(lex);
                    if (lexer_peek(lex) == '"') {
                        lexer_advance(lex);
                        // Found closing """
                        break;
                    }
                }

                // Not the closing """, restore and continue
                lex->pos = saved_pos;
                lex->line = saved_line;
                lex->column = saved_col;
            }

            char c = lexer_advance(lex);
            d_StringAppendChar(str, c);
        }
    } else {
        // Single-line string: read until closing "
        while (lexer_peek(lex) != '"' && lexer_peek(lex) != '\n' && lexer_peek(lex) != '\0') {
            char c = lexer_advance(lex);

            // Handle escape sequences
            if (c == '\\') {
                char next = lexer_peek(lex);
                if (next == 'n') {
                    lexer_advance(lex);
                    d_StringAppendChar(str, '\n');
                } else if (next == 't') {
                    lexer_advance(lex);
                    d_StringAppendChar(str, '\t');
                } else if (next == '"') {
                    lexer_advance(lex);
                    d_StringAppendChar(str, '"');
                } else if (next == '\\') {
                    lexer_advance(lex);
                    d_StringAppendChar(str, '\\');
                } else {
                    d_StringAppendChar(str, c);
                }
            } else {
                d_StringAppendChar(str, c);
            }
        }

        // Consume closing "
        if (lexer_peek(lex) == '"') {
            lexer_advance(lex);
        }
    }

    Token_t* tok = token_create(TOK_STRING, d_StringPeek(str), start_line, start_column);
    d_StringDestroy(str);
    return tok;
}

static Token_t* lexer_read_number(Lexer_t* lex)
{
    int start_line = lex->line;
    int start_column = lex->column;
    dString_t* num = d_StringInit();

    if (num == NULL) {
        return token_create(TOK_ERROR, "Memory allocation failed", start_line, start_column);
    }

    // Handle negative numbers
    if (lexer_peek(lex) == '-') {
        d_StringAppendChar(num, lexer_advance(lex));
    }

    // Read digits before decimal point
    while (isdigit(lexer_peek(lex))) {
        d_StringAppendChar(num, lexer_advance(lex));
    }

    // Check for decimal point
    if (lexer_peek(lex) == '.') {
        d_StringAppendChar(num, lexer_advance(lex));

        // Read digits after decimal point
        while (isdigit(lexer_peek(lex))) {
            d_StringAppendChar(num, lexer_advance(lex));
        }
    }

    Token_t* tok = token_create(TOK_NUMBER, d_StringPeek(num), start_line, start_column);
    d_StringDestroy(num);
    return tok;
}

static Token_t* lexer_read_identifier(Lexer_t* lex)
{
    int start_line = lex->line;
    int start_column = lex->column;
    dString_t* id = d_StringInit();

    if (id == NULL) {
        return token_create(TOK_ERROR, "Memory allocation failed", start_line, start_column);
    }

    // Read alphanumeric and underscore
    while (isalnum(lexer_peek(lex)) || lexer_peek(lex) == '_') {
        d_StringAppendChar(id, lexer_advance(lex));
    }

    const char* id_str = d_StringPeek(id);
    TokenType_t type = TOK_IDENTIFIER;

    // Check for boolean keywords
    if (strcmp(id_str, "true") == 0 || strcmp(id_str, "false") == 0) {
        type = TOK_BOOL;
    }

    Token_t* tok = token_create(type, id_str, start_line, start_column);
    d_StringDestroy(id);
    return tok;
}

// =============================================================================
// Main Lexer Function
// =============================================================================

dArray_t* d_DUFLex(const char* input)
{
    if (input == NULL) {
        return NULL;
    }

    Lexer_t lex = {0};
    lex.input = input;
    lex.len = strlen(input);
    lex.pos = 0;
    lex.line = 1;
    lex.column = 1;
    lex.tokens = d_ArrayInit(32, sizeof(Token_t*));

    if (lex.tokens == NULL) {
        return NULL;
    }

    while (lex.pos < lex.len) {
        lexer_skip_whitespace(&lex);
        lexer_skip_comment(&lex);
        lexer_skip_whitespace(&lex); // Skip whitespace after comment (including newline)

        if (lex.pos >= lex.len) {
            break;
        }

        char c = lexer_peek(&lex);
        Token_t* tok = NULL;

        switch (c) {
            case '@':
                tok = token_create(TOK_AT, "@", lex.line, lex.column);
                lexer_advance(&lex);
                break;

            case '{':
                tok = token_create(TOK_LBRACE, "{", lex.line, lex.column);
                lexer_advance(&lex);
                break;

            case '}':
                tok = token_create(TOK_RBRACE, "}", lex.line, lex.column);
                lexer_advance(&lex);
                break;

            case '[':
                tok = token_create(TOK_LBRACKET, "[", lex.line, lex.column);
                lexer_advance(&lex);
                break;

            case ']':
                tok = token_create(TOK_RBRACKET, "]", lex.line, lex.column);
                lexer_advance(&lex);
                break;

            case ':':
                tok = token_create(TOK_COLON, ":", lex.line, lex.column);
                lexer_advance(&lex);
                break;

            case ',':
                tok = token_create(TOK_COMMA, ",", lex.line, lex.column);
                lexer_advance(&lex);
                break;

            case '"':
                tok = lexer_read_string(&lex);
                break;

            case '-':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                tok = lexer_read_number(&lex);
                break;

            default:
                if (isalpha(c) || c == '_') {
                    tok = lexer_read_identifier(&lex);
                } else {
                    // Unknown character
                    dString_t* err = d_StringInit();
                    d_StringFormat(err, "Unexpected character '%c'", c);
                    tok = token_create(TOK_ERROR, d_StringPeek(err), lex.line, lex.column);
                    d_StringDestroy(err);
                    lexer_advance(&lex);
                }
                break;
        }

        if (tok != NULL) {
            d_ArrayAppend(lex.tokens, &tok);
        }
    }

    // Add EOF token
    Token_t* eof = token_create(TOK_EOF, "", lex.line, lex.column);
    if (eof != NULL) {
        d_ArrayAppend(lex.tokens, &eof);
    }

    return lex.tokens;
}

// Free token array
void d_DUFLexFree(dArray_t* tokens)
{
    if (tokens == NULL) {
        return;
    }

    for (size_t i = 0; i < tokens->count; i++) {
        Token_t** tok_ptr = (Token_t**)d_ArrayGet(tokens, i);
        if (tok_ptr != NULL && *tok_ptr != NULL) {
            token_destroy(*tok_ptr);
        }
    }

    d_ArrayDestroy(tokens);
}
