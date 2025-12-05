/* dDUFValue.c - DUF value creation, management, and cleanup (AUF-style) */

#define _POSIX_C_SOURCE 200809L

#include "Daedalus.h"
#include <stdlib.h>
#include <string.h>

// =============================================================================
// Value Creation Functions
// =============================================================================

dDUFValue_t* d_DUFCreateTable(void)
{
    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_TABLE;
    return val;
}

dDUFValue_t* d_DUFCreateArray(void)
{
    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_ARRAY;
    return val;
}

dDUFValue_t* d_DUFCreateInt(int64_t int_val)
{
    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_INT;
    val->value_int = int_val;
    return val;
}

dDUFValue_t* d_DUFCreateFloat(double float_val)
{
    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_FLOAT;
    val->value_double = float_val;
    return val;
}

dDUFValue_t* d_DUFCreateBool(bool bool_val)
{
    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_BOOL;
    val->value_int = bool_val ? 1 : 0;  // Store bool as int
    return val;
}

dDUFValue_t* d_DUFCreateString(const char* str)
{
    if (str == NULL) {
        return NULL;
    }

    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_STRING;
    val->value_string = strdup(str);
    if (val->value_string == NULL) {
        free(val);
        return NULL;
    }

    return val;
}

// =============================================================================
// Type Inspection
// =============================================================================

dDUFType_t d_DUFGetType(dDUFValue_t* val)
{
    if (val == NULL) {
        return D_DUF_NULL;
    }
    return (dDUFType_t)val->type;
}

// =============================================================================
// Cleanup
// =============================================================================

void d_DUFFree(dDUFValue_t* val)
{
    if (val == NULL) {
        return;
    }

    // Free all children recursively
    dDUFValue_t* child = val->child;
    while (child != NULL) {
        dDUFValue_t* next_child = child->next;
        d_DUFFree(child);
        child = next_child;
    }

    // Free string fields
    if (val->key != NULL) {
        free(val->key);
    }
    if (val->value_string != NULL) {
        free(val->value_string);
    }

    // Free the node itself
    free(val);
}

void d_DUFErrorFree(dDUFError_t* err)
{
    if (err == NULL) {
        return;
    }

    if (err->message != NULL) {
        d_StringDestroy(err->message);
    }

    free(err);
}
