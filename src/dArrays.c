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

/**
 * @brief Initialize a Dynamic Array.
 * 
 * @param capacity The initial capacity of the array in elements.
 * @param element_size The size of each element in bytes.
 * 
 * @return A pointer to the new array, or NULL on error.
 * 
 * -- Must be destroyed with d_DestroyArray() to free memory
 * -- Initial count is 0 even though capacity may be larger
 * -- Elements can be any type as long as element_size is correct
 * -- Capacity of 0 is allowed but array cannot store elements until resized
 * 
 * Example: `dArray_t* array = d_InitArray(10, sizeof(int));`
 * This creates a new array with a capacity of 10 elements, each of size 4 bytes.
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

/**
 * @brief Destroy a dynamic array.
 * 
 * @param array The array to destroy.
 * 
 * @return: 0 on success, 1 on failure.
 * 
 * -- Frees both the data buffer and the array structure itself
 * -- After calling this function, the pointer is invalid and should not be used
 * 
 * Example: `d_DestroyArray(array);`
 * This destroys the dynamic array and frees its memory.
 */
int d_DestroyArray(dArray_t* array) {
    if (!array) return 1;
    if (array->data) free(array->data);
    free(array);
    return 0;
}

// =============================================================================
// DYNAMIC ARRAY MEMORY MANAGEMENT
// =============================================================================

/**
 * @brief Resize the internal data buffer of a dynamic array.
 *
 * This function adjusts the allocated memory for the array's elements.
 *
 * @param array A pointer to the dynamic array whose internal buffer is to be resized.
 * @param new_size_in_bytes The desired new total size of the internal data buffer in bytes.
 * This will determine the new capacity in elements.
 *
 * @return 0 on success, 1 on failure.
 *
 * @note If `new_size_in_bytes` is 0, the internal data buffer will be freed, and the array's
 * capacity and count will be reset to 0.
 * @note If `new_size_in_bytes` is larger than the current allocated size, the data buffer
 * is reallocated, potentially moving to a new memory location. Existing data (up to
 * the old capacity) is preserved.
 * @note If `new_size_in_bytes` is smaller than the current allocated size, the data buffer
 * is truncated. If the current `count` of elements exceeds the new capacity, `count`
 * will be adjusted down to match the new capacity, effectively truncating the array's
 * contents.
 * @warning This function only affects the internal data buffer (`array->data`). The `dArray_t* array`
 * pointer itself remains valid and points to the same `dArray_t` structure.
 * Pointers obtained previously via `d_IndexDataFromArray` (or similar direct access)
 * will become invalid if the underlying `array->data` buffer is reallocated and moved.
 *
 * Example: `d_ResizeArray(myArray, 10 * sizeof(int));`
 * This resizes the internal buffer of `myArray` to accommodate 10 integer elements.
 * If `myArray` previously held more than 10 elements, its `count` will be truncated.
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

/**
 * @brief Grow the array by a number of additional bytes.
 * 
 * @param array The array to grow.
 * @param additional_bytes The number of bytes to add.
 * 
 * @return 0 on success, 1 on failure.
 * 
 * -- Convenience function that calls d_ResizeArray() internally
 * 
 * Example: `d_GrowArray(array, 10 * sizeof(int));`
 * This grows the dynamic array by 10 elements, each of size 4 bytes.
 */
int d_GrowArray(dArray_t* array, size_t additional_bytes) {
    if (!array) return 1;
    size_t current_bytes = array->capacity * array->element_size;
    return d_ResizeArray(array, current_bytes + additional_bytes);
}

// =============================================================================
// DYNAMIC ARRAY ELEMENT MANAGEMENT (to) and (from) ARRAY
// =============================================================================

/**
 * @brief Append an element to the end of the dynamic array.
 *
 * This function adds a new element to the array. If the array's current capacity
 * is insufficient, it will automatically grow to accommodate the new element,
 * typically by doubling its capacity.
 *
 * @param array A pointer to the dynamic array to append to.
 * @param data A pointer to the element data to copy into the array.
 * The data pointed to will be copied by `element_size` bytes.
 *
 * @return 0 on success, 1 on failure.
 *
 * @note Fails if `array` or `data` is NULL.
 * @note If the array's capacity is reached, it will attempt to grow its
 * internal buffer. If reallocation fails, the append operation will fail.
 * @note Copies `element_size` bytes from the `data` pointer into the array.
 * @note Increments the array's `count` on successful append.
 *
 * Example: `int my_value = 123; int result = d_AppendDataToArray(myArray, &my_value);`
 * This appends an integer value to the end of `myArray`.
 */
 int d_AppendDataToArray( dArray_t* array, void* data )
 {
     if ( array == NULL || data == NULL )
     {
         return 1;
     }

     if ( array->count >= array->capacity )
     {
         size_t new_capacity = array->capacity == 0 ? 1 : array->capacity * 2;
         size_t new_size = new_capacity * array->element_size;

         if ( d_ResizeArray( array, new_size ) != 0 )
         {
             return 1;
         }
     }

     void* dest = ( char* )array->data + ( array->count * array->element_size );
     memcpy( dest, data, array->element_size );

     array->count++;
     return 0;
 }

/**
 * @brief Get a pointer to the data at a specific index.
 * 
 * @param array The array to get data from.
 * @param index The index of the element to get.
 * 
 * @return A pointer to the element data, or NULL if index is out of bounds.
 * 
 * -- Returns NULL if array is NULL or index >= count
 * -- Returned pointer is valid until the array is modified or destroyed
 * -- Caller can read/write through the returned pointer safely
 * -- Use appropriate casting: int* ptr = (int*)d_IndexDataFromArray(array, 0)
 * -- Index must be less than count, not capacity (only counts appended elements)
 * 
 * Example: `void* data = d_IndexDataFromArray(array, 0);`
 * This retrieves the first element from the dynamic array.
 */
void* d_IndexDataFromArray(dArray_t* array, size_t index) {
    if (!array || index >= array->count) {
        return NULL;
    }
    return (char*)array->data + (index * array->element_size);
}

/**
 * @brief Remove and return the last element from the array.
 * 
 * @param array The array to pop from.
 * 
 * @return A pointer to the last element's data, or NULL if array is empty.
 * 
 * -- Decrements count but does not free memory (element data remains in buffer)
 * -- Returned pointer becomes invalid after next append or array modification
 * -- Implements stack-like behavior for dynamic arrays
 * -- Memory is not reallocated, only the count is decremented for efficiency
 * 
 * Example: `void* data = d_PopDataFromArray(array);`
 * This removes and returns the last element from the dynamic array.
 */
void* d_PopDataFromArray(dArray_t* array) {
    if (!array || array->count == 0) {
        return NULL;
    }
    array->count--;
    return (char*)array->data + (array->count * array->element_size);
}

// =============================================================================
// DYNAMIC ARRAY CAPACITY MANAGEMENT
// =============================================================================

/**
 * @brief Optimize memory usage by shrinking the array's capacity to match its count
 * 
 * @param array The array to trim
 * 
 * @return 0 on success, 1 on failure
 * 
 * -- Reduces allocated memory to exactly match the number of elements
 * -- If array is empty, frees the data buffer
 * -- Does nothing if array is already optimally sized
 * -- Useful after bulk removal operations to reclaim memory
 * 
 * Example: `d_TrimCapacityOfArray(array);`
 * This trims the array's capacity to match its count, freeing memory if necessary.
 */
int d_TrimCapacityOfArray(dArray_t* array) {
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

    if (d_ResizeArray(array, required_bytes) != 0) {
        d_LogError("Failed to reallocate memory for trimming dynamic array.");
        return 1;
    }

    return 0;
}

/**
 * @brief Ensure the array has enough capacity for at least min_capacity elements
 * 
 * @param array The array to ensure capacity for
 * @param min_capacity Minimum number of elements the array should accommodate
 * 
 * @return 0 on success, 1 on failure
 * 
 * -- Grows the array if current capacity is less than min_capacity
 * -- Uses exponential growth strategy to minimize reallocations
 * -- Never shrinks the array - use d_TrimCapacityOfArray for that
 * -- Useful for pre-allocating space before bulk operations
 * 
 * Example: `d_EnsureCapacityOfArray(array, 100);`
 * This ensures the array has at least 100 elements allocated, growing the array if needed.
 */
int d_EnsureCapacityOfArray(dArray_t* array, size_t min_capacity) {
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

    if (d_ResizeArray(array, new_size_in_bytes) != 0) {
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

/**
 * @brief Insert data at a specific index in the array
 * 
 * @param array The array to insert into
 * @param data Pointer to the data to insert
 * @param index The index where to insert the data
 * 
 * @return 0 on success, 1 on failure
 * 
 * -- Shifts existing elements to the right to make space
 * -- Grows the array capacity if needed
 * -- index must be <= array->count (can insert at end)
 * -- Uses memmove for safe overlapping memory operations
 * 
 * Example: `d_InsertDataIntoArray(array, &value, 2);`
 * This inserts a value at index 2, shifting existing elements to the right.
 */
int d_InsertDataIntoArray(dArray_t* array, void* data, size_t index) {
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

        if (d_ResizeArray(array, new_size_in_bytes) != 0) {
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

/**
 * @brief Remove data at a specific index from the array
 * 
 * @param array The array to remove from
 * @param index The index of the element to remove
 * 
 * @return 0 on success, 1 on failure
 * 
 * -- Shifts existing elements to the left to fill the gap
 * -- Does not resize the array capacity (use d_TrimCapacityOfArray for that)
 * -- index must be < array->count
 * -- Uses memmove for safe overlapping memory operations
 * 
 * Example: `d_RemoveDataFromArray(array, 2);`
 * This removes the element at index 2, shifting remaining elements to the left.
 */
int d_RemoveDataFromArray(dArray_t* array, size_t index) {
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

/**
 * @brief Clear all elements from the array without deallocating memory
 *
 * @param array The array to clear
 *
 * @return 0 on success, 1 on failure
 *
 * -- Sets count to 0, preserving capacity for efficient reuse
 * -- O(1) operation - no memory operations performed
 * -- Does not zero memory or shrink capacity
 * -- Ideal for clearing collections that will be reused (hands, queues, temp buffers)
 *
 * Example: `d_ClearArray(array);`
 * This clears all elements from the array, resetting count to 0 while keeping capacity.
 */
int d_ClearArray(dArray_t* array) {
    if (!array) {
        d_LogError("Attempted to clear a NULL dynamic array.");
        return 1;
    }

    array->count = 0;
    return 0;
}