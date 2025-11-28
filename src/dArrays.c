/**
 * @file dArrays.c
 *
 * Dynamic Arrays that grow as needed.
 * 
 */

#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// =============================================================================
// DYNAMIC ARRAY INITIALIZATION AND DESTRUCTION
// =============================================================================

dArray_t* d_ArrayInit(size_t capacity, size_t element_size) {
    if (element_size == 0) return NULL;

    dArray_t* array = (dArray_t*)malloc(sizeof(dArray_t));
    if (!array) return NULL;

    array->capacity = capacity;
    array->count = 0;
    array->element_size = element_size;

    // Only allocate memory if the initial capacity is greater than zero.
    if (capacity > 0) {
        array->data = malloc(array->capacity * array->element_size);
        if (!array->data) {
            free(array);
            return NULL;
        }
    } else {
        array->data = NULL;
    }
    return array;
}

int d_ArrayDestroy(dArray_t* array) {
    if (!array) return 1;
    if (array->data) free(array->data);
    free(array);
    return 0;
}

// =============================================================================
// DYNAMIC ARRAY MEMORY MANAGEMENT
// =============================================================================

int d_ArrayResize(dArray_t* array, size_t new_size_in_bytes) {
    if (!array) return 1;

    // If new size is 0, free the data and reset.
    if (new_size_in_bytes == 0) {
        if(array->data) free(array->data);
        array->data = NULL;
        array->capacity = 0;
        array->count = 0;
        return 0;
    }

    void* new_data = realloc(array->data, new_size_in_bytes);
    if (!new_data) return 1;

    array->data = new_data;
    // Correctly calculate capacity in elements from bytes.
    array->capacity = new_size_in_bytes / array->element_size;

    if (array->count > array->capacity) {
        array->count = array->capacity;
    }

    return 0; // Success
}

int d_ArrayGrow(dArray_t* array, size_t additional_bytes) {
    if (!array) return 1;
    size_t current_bytes = array->capacity * array->element_size;
    return d_ArrayResize(array, current_bytes + additional_bytes);
}

// =============================================================================
// DYNAMIC ARRAY ELEMENT MANAGEMENT (to) and (from) ARRAY
// =============================================================================

 int d_ArrayAppend( dArray_t* array, void* data )
 {
     if ( array == NULL || data == NULL )
     {
         return 1;
     }

     if ( array->count >= array->capacity )
     {
         size_t new_capacity = array->capacity == 0 ? 1 : array->capacity * 2;
         size_t new_size = new_capacity * array->element_size;

         if ( d_ArrayResize( array, new_size ) != 0 )
         {
             return 1;
         }
     }

     void* dest = ( char* )array->data + ( array->count * array->element_size );
     memcpy( dest, data, array->element_size );

     array->count++;
     return 0;
 }

void* d_ArrayGet(dArray_t* array, size_t index) {
    if (!array || index >= array->count) {
        return NULL;
    }
    return (char*)array->data + (index * array->element_size);
}

void* d_ArrayPop(dArray_t* array) {
    if (!array || array->count == 0) {
        return NULL;
    }
    array->count--;
    return (char*)array->data + (array->count * array->element_size);
}

// =============================================================================
// DYNAMIC ARRAY CAPACITY MANAGEMENT
// =============================================================================

int d_ArrayTrimCapacity(dArray_t* array) {
    if (!array) {
        d_LogError("Attempted to trim capacity of a NULL dynamic array.");
        return 1;
    }

    size_t required_bytes = array->count * array->element_size;
    size_t current_bytes = array->capacity * array->element_size;

    if (required_bytes == current_bytes) {
        d_LogDebug("Array already optimally sized, no trimming needed.");
        return 0;
    }

    if (d_ArrayResize(array, required_bytes) != 0) {
        d_LogError("Failed to reallocate memory for trimming dynamic array.");
        return 1;
    }

    return 0;
}

int d_ArrayEnsureCapacity(dArray_t* array, size_t min_capacity) {
    if (!array) {
        d_LogError("Attempted to ensure capacity of a NULL dynamic array.");
        return 1;
    }

    if (array->element_size == 0) {
        d_LogError("Cannot ensure capacity for array with zero element size.");
        return 1;
    }

    if (array->capacity >= min_capacity) {
        d_LogDebugF("Array already has sufficient capacity (%zu) for requested minimum (%zu).", 
                    array->capacity, min_capacity);
        return 0;
    }

    size_t old_capacity = array->capacity;
    size_t new_capacity = min_capacity;

    size_t new_size_in_bytes = new_capacity * array->element_size;

    if (d_ArrayResize(array, new_size_in_bytes) != 0) {
        d_LogErrorF("Failed to resize array to ensure minimum capacity of %zu.", min_capacity);
        return 1;
    }

    d_LogInfoF("Array capacity increased from %zu to %zu to meet minimum %zu.", 
               old_capacity, array->capacity, min_capacity);
    return 0;
}

// =============================================================================
// DYNAMIC ARRAY INSERTION AND REMOVAL
// =============================================================================

int d_ArrayInsert(dArray_t* array, void* data, size_t index) {
    if (!array || !data) {
        d_LogError("Invalid input: array or data is NULL for insert operation.");
        return 1;
    }

    if (index > array->count) {
        d_LogErrorF("Attempted to insert data at index %zu, which is beyond current array count %zu.", 
                    index, array->count);
        return 1;
    }

    if (array->count >= array->capacity) {
        size_t new_capacity = array->capacity == 0 ? 1 : array->capacity * 2;
        size_t new_size_in_bytes = new_capacity * array->element_size;

        if (d_ArrayResize(array, new_size_in_bytes) != 0) {
            d_LogErrorF("Failed to grow array for insert operation at index %zu.", index);
            return 1;
        }
    }

    if (index < array->count) {
        char* destination_start = (char*)array->data + ((index + 1) * array->element_size);
        char* source_start = (char*)array->data + (index * array->element_size);
        size_t num_bytes_to_shift = (array->count - index) * array->element_size;

        memmove(destination_start, source_start, num_bytes_to_shift);
    }

    char* insert_address = (char*)array->data + (index * array->element_size);
    memcpy(insert_address, data, array->element_size);

    array->count++;
    return 0;
}

int d_ArrayRemove(dArray_t* array, size_t index) {
    if (!array) {
        d_LogError("Invalid input: array is NULL for remove operation.");
        return 1;
    }

    if (index >= array->count) {
        d_LogErrorF("Attempted to remove data at index %zu, which is beyond current array count %zu.", 
                    index, array->count);
        return 1;
    }

    if (index < array->count - 1) {
        char* destination_start = (char*)array->data + (index * array->element_size);
        char* source_start = (char*)array->data + ((index + 1) * array->element_size);
        size_t num_bytes_to_shift = (array->count - index - 1) * array->element_size;

        memmove(destination_start, source_start, num_bytes_to_shift);
    }

    array->count--;
    return 0;
}

int d_ArrayClear(dArray_t* array) {
    if (!array) {
        d_LogError("Attempted to clear a NULL dynamic array.");
        return 1;
    }

    array->count = 0;
    return 0;
}