// File: src/dDynamicArray.c
// Arrays that grow dynamically.

#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * Create a new dynamic array.
 */
dArray_t* d_InitArray(size_t capacity, size_t element_size) {
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

/*
 * Destroy a dynamic array.
 */
void d_DestroyArray(dArray_t* array) {
    if (!array) return;
    if (array->data) free(array->data);
    free(array);
}

/*
 * Resize a dynamic array. The contract is it takes a new capacity in BYTES.
 */
int d_ResizeArray(dArray_t* array, size_t new_size_in_bytes) {
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

/*
 * Append an element to the array.
 */
 void d_AppendArray( dArray_t* array, void* data )
 {
     if ( array == NULL || data == NULL )
     {
         return;
     }

     if ( array->count >= array->capacity )
     {
         size_t new_capacity = array->capacity == 0 ? 1 : array->capacity * 2;
         size_t new_size = new_capacity * array->element_size;

         if ( d_ResizeArray( array, new_size ) != 0 )
         {
             return;
         }
     }

     void* dest = ( char* )array->data + ( array->count * array->element_size );
     memcpy( dest, data, array->element_size );

     array->count++;
 }
/*
 * Get a pointer to the data at a specific index.
 */
void* d_GetDataFromArrayByIndex(dArray_t* array, size_t index) {
    if (!array || index >= array->count) {
        return NULL;
    }
    return (char*)array->data + (index * array->element_size);
}

/*
 * Remove and return the last element from the array.
 */
void* d_PopDataFromArray(dArray_t* array) {
    if (!array || array->count == 0) {
        return NULL;
    }
    array->count--;
    return (char*)array->data + (array->count * array->element_size);
}

/*
 * Grow the array by a number of additional bytes.
 */
int d_GrowArray(dArray_t* array, size_t additional_bytes) {
    if (!array) return 1;
    size_t current_bytes = array->capacity * array->element_size;
    return d_ResizeArray(array, current_bytes + additional_bytes);
}
