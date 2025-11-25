/* dDUFValue.c - DUF value creation, management, and cleanup */

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
    // Create table with string keys and dDUFValue_t* values
    val->table_val = d_TableInit(
        sizeof(char*),           // key_size (pointer to string)
        sizeof(dDUFValue_t*),    // value_size (pointer to value)
        d_HashString,            // hash function for strings
        d_CompareString,         // comparison function for strings
        16                       // initial bucket count
    );

    if (val->table_val == NULL) {
        free(val);
        return NULL;
    }

    return val;
}

dDUFValue_t* d_DUFCreateArray(void)
{
    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_ARRAY;
    val->array_val = d_ArrayInit(8, sizeof(dDUFValue_t*));

    if (val->array_val == NULL) {
        free(val);
        return NULL;
    }

    return val;
}

dDUFValue_t* d_DUFCreateInt(int64_t int_val)
{
    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_INT;
    val->int_val = int_val;
    return val;
}

dDUFValue_t* d_DUFCreateFloat(double float_val)
{
    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_FLOAT;
    val->float_val = float_val;
    return val;
}

dDUFValue_t* d_DUFCreateBool(bool bool_val)
{
    dDUFValue_t* val = (dDUFValue_t*)calloc(1, sizeof(dDUFValue_t));
    if (val == NULL) {
        return NULL;
    }

    val->type = D_DUF_BOOL;
    val->bool_val = bool_val;
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
    val->string_val = d_StringInit();

    if (val->string_val == NULL) {
        free(val);
        return NULL;
    }

    d_StringAppend(val->string_val, str, 0);
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
    return val->type;
}

// =============================================================================
// Direct Table/Array Access
// =============================================================================

dDUFValue_t* d_DUFTableGet(dDUFValue_t* table, const char* key)
{
    if (table == NULL || table->type != D_DUF_TABLE || key == NULL) {
        return NULL;
    }

    // Get from table
    dDUFValue_t** val_ptr = (dDUFValue_t**)d_TableGet(table->table_val, &key);
    if (val_ptr == NULL) {
        return NULL;
    }

    return *val_ptr;
}

void d_DUFTableSet(dDUFValue_t* table, const char* key, dDUFValue_t* val)
{
    if (table == NULL || table->type != D_DUF_TABLE || key == NULL || val == NULL) {
        return;
    }

    // Duplicate the key string for storage
    char* key_copy = strdup(key);
    if (key_copy == NULL) {
        return;
    }

    // Store pointer to value
    d_TableSet(table->table_val, &key_copy, &val);
}

size_t d_DUFArrayLength(dDUFValue_t* array)
{
    if (array == NULL || array->type != D_DUF_ARRAY) {
        return 0;
    }
    return array->array_val->count;
}

dDUFValue_t* d_DUFArrayGet(dDUFValue_t* array, size_t index)
{
    if (array == NULL || array->type != D_DUF_ARRAY) {
        return NULL;
    }

    if (index >= array->array_val->count) {
        return NULL;
    }

    dDUFValue_t** val_ptr = (dDUFValue_t**)d_ArrayGet(array->array_val, index);
    if (val_ptr == NULL) {
        return NULL;
    }

    return *val_ptr;
}

// =============================================================================
// Iteration
// =============================================================================

// Helper structure for table iteration
typedef struct {
    dDUFIteratorFunc callback;
    void* user_data;
} DUFTableIterContext_t;

// Internal callback adapter for table iteration
static void d_DUFTableIterAdapter(const void* key, size_t key_size,
                                   const void* value, size_t value_size,
                                   void* user_data)
{
    (void)key_size;    // Unused
    (void)value_size;  // Unused

    DUFTableIterContext_t* ctx = (DUFTableIterContext_t*)user_data;
    const char** key_ptr = (const char**)key;
    dDUFValue_t** val_ptr = (dDUFValue_t**)value;

    ctx->callback(*key_ptr, *val_ptr, ctx->user_data);
}

void d_DUFTableForEach(dDUFValue_t* table, dDUFIteratorFunc callback, void* user_data)
{
    if (table == NULL || table->type != D_DUF_TABLE || callback == NULL) {
        return;
    }

    DUFTableIterContext_t ctx = { callback, user_data };
    d_TableForEach(table->table_val, d_DUFTableIterAdapter, &ctx);
}

// =============================================================================
// Cleanup Functions
// =============================================================================

void d_DUFFree(dDUFValue_t* val)
{
    if (val == NULL) {
        return;
    }

    switch (val->type) {
        case D_DUF_STRING:
            if (val->string_val != NULL) {
                d_StringDestroy(val->string_val);
            }
            break;

        case D_DUF_ARRAY:
            if (val->array_val != NULL) {
                // Free each element in the array
                for (size_t i = 0; i < val->array_val->count; i++) {
                    dDUFValue_t** elem_ptr = (dDUFValue_t**)d_ArrayGet(val->array_val, i);
                    if (elem_ptr != NULL && *elem_ptr != NULL) {
                        d_DUFFree(*elem_ptr);
                    }
                }
                d_ArrayDestroy(val->array_val);
            }
            break;

        case D_DUF_TABLE:
            if (val->table_val != NULL) {
                // Free each value in the table
                // We need to iterate and free all values first
                size_t num_buckets = val->table_val->num_buckets;
                dArray_t* buckets = val->table_val->buckets;

                for (size_t i = 0; i < num_buckets; i++) {
                    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(buckets, i);
                    if (bucket_ptr == NULL || *bucket_ptr == NULL) {
                        continue;
                    }

                    dLinkedList_t* node = *bucket_ptr;
                    while (node != NULL) {
                        dTableEntry_t* entry = (dTableEntry_t*)node->data;
                        if (entry != NULL) {
                            // Free the key string
                            char** key_ptr = (char**)entry->key_data;
                            if (key_ptr != NULL && *key_ptr != NULL) {
                                free(*key_ptr);
                            }

                            // Free the value
                            dDUFValue_t** val_ptr = (dDUFValue_t**)entry->value_data;
                            if (val_ptr != NULL && *val_ptr != NULL) {
                                d_DUFFree(*val_ptr);
                            }
                        }
                        node = node->next;
                    }
                }

                d_TableDestroy(&val->table_val);
            }
            break;

        case D_DUF_NULL:
        case D_DUF_BOOL:
        case D_DUF_INT:
        case D_DUF_FLOAT:
            // Primitive types, nothing to free
            break;
    }

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
