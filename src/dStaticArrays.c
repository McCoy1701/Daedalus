// File: src/dStaticArrays.c - Static Array Implementation for Daedalus Library
// Fixed-size array data structure with no dynamic resizing capabilities

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Daedalus.h"

// =============================================================================
// STATIC ARRAY INITIALIZATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Initialize a new static array with fixed capacity
 *
 * @param capacity: Maximum number of elements the array can hold
 * @param element_size: Size of each element in bytes
 * 
 * @return: Pointer to new static array, or NULL on allocation failure
 *
 * -- Allocates memory for both the structure and the data buffer
 * -- Data buffer size = capacity * element_size
 * -- count is initialized to 0
 * -- capacity and element_size are fixed for the lifetime of the array
 * 
 * Example: `dStaticArray_t* array = d_InitStaticArray(10, sizeof(int));`
 * This creates a new static array with a capacity of 10 elements, each of size 4 bytes.
 */
dStaticArray_t* d_InitStaticArray(size_t capacity, size_t element_size)
{
    // Validate input parameters
    if (capacity == 0 || element_size == 0) {
        return NULL;
    }

    // Allocate memory for the array structure
    dStaticArray_t* array = (dStaticArray_t*)malloc(sizeof(dStaticArray_t));
    if (!array) {
        return NULL; // Structure allocation failed
    }

    // Calculate total data buffer size
    size_t data_size = capacity * element_size;
    
    // Allocate memory for the data buffer
    array->data = malloc(data_size);
    if (!array->data) {
        // Data allocation failed, cleanup structure
        free(array);
        return NULL;
    }

    // Initialize array structure
    array->capacity = capacity;
    array->count = 0;
    array->element_size = element_size;

    // Initialize data buffer to zero for predictable behavior
    memset(array->data, 0, data_size);

    return array;
}

/**
 * @brief: Destroy a static array and free all associated memory
 *
 * @param array: Pointer to static array to destroy
 * @return: 0 on success, 1 on failure
 *
 * -- Safe to call with NULL pointer
 * -- Frees data buffer first, then structure
 * -- After calling, the pointer becomes invalid
 * 
 * Example: `d_DestroyStaticArray(array);`
 * This destroys the static array and frees its memory.
 */
int d_DestroyStaticArray(dStaticArray_t* array)
{
    if (!array) {
        return 1; // NULL pointer - considered failure
    }

    // Free data buffer if it exists
    if (array->data) {
        free(array->data);
        array->data = NULL;
    }

    // Free the array structure itself
    free(array);

    return 0; // Success
}

// =============================================================================
// STATIC ARRAY ELEMENT MANAGEMENT (to) and (from) ARRAY
// =============================================================================

/** 
 * @brief Append an element to the end of the static array.
 * *
 * @param array: Pointer to static array
 * @param data: Pointer to element data to copy
 * @return: 0 on success, 1 on failure
 *
 * -- Fails if array is at maximum capacity
 * -- Fails if array or data is NULL
 * -- Copies element_size bytes from data
 * -- Increments count on successful append
 * 
 * Example: `d_AppendDataToStaticArray(array, &data);`
 * This appends a pointer to a data element to the end of the static array.
 */
int d_AppendDataToStaticArray(dStaticArray_t* array, void* data)
{
    // Validate input parameters
    if (!array || !data) {
        return 1; // Invalid parameters
    }

    // Check if array is at capacity
    if (array->count >= array->capacity) {
        return 1; // Array is full - cannot append
    }

    // Calculate destination address for new element
    char* dest = (char*)array->data + (array->count * array->element_size);
    
    // Copy element data to the array
    memcpy(dest, data, array->element_size);
    
    // Increment count
    array->count++;
    
    return 0; // Success
}

/**
 * @brief: Get a pointer to an element at the specified index
 *
 * @param array: Pointer to static array
 * @param index: Zero-based index of element to retrieve
 * @return: Pointer to element data, or NULL if out of bounds
 *
 * -- Returns NULL if array is NULL or index >= count
 * -- Performs bounds checking for safety
 * -- Returned pointer is valid until array is modified or destroyed
 * 
 * Example: `void* data = d_IndexDataFromStaticArray(array, 0);`
 * This retrieves the first element from the static array.
 */
void* d_IndexDataFromStaticArray(dStaticArray_t* array, size_t index)
{
    // Validate input parameters
    if (!array || !array->data) {
        return NULL; // Invalid array
    }

    // Check bounds - index must be less than count (not capacity)
    if (index >= array->count) {
        return NULL; // Index out of bounds
    }

    // Calculate and return pointer to element
    char* element_ptr = (char*)array->data + (index * array->element_size);
    return (void*)element_ptr;
}

/**
 * @brief: Remove and return a pointer to the last element (LIFO behavior)
 *
 * @param array: Pointer to static array
 * @return: Pointer to last element data, or NULL if empty
 *
 * -- Returns NULL if array is NULL or empty
 * -- Decrements count but doesn't free memory
 * -- Element data remains in buffer but is no longer active
 * -- Returned pointer becomes invalid after next append
 * 
 * Example: `void* data = d_PopDataFromStaticArray(array);`
 * This removes and returns the last element from the static array.
 */
void* d_PopDataFromStaticArray(dStaticArray_t* array)
{
    // Validate input parameters
    if (!array || !array->data) {
        return NULL; // Invalid array
    }

    // Check if array is empty
    if (array->count == 0) {
        return NULL; // No elements to pop
    }

    // Decrement count (removing last element from active array)
    array->count--;
    
    // Calculate and return pointer to the element that was just "popped"
    char* element_ptr = (char*)array->data + (array->count * array->element_size);
    return (void*)element_ptr;
}

// =============================================================================
// STATIC ARRAY UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Get the number of free slots remaining in the static array
 *
 * @param array: Pointer to static array
 * @return: Number of elements that can still be added, or 0 if array is NULL/full
 *
 * -- Returns 0 if array is NULL (no space available)
 * -- Returns capacity - count for valid arrays
 * -- Useful for checking if array can accept more elements before attempting append
 * 
 * Example: `size_t free_slots = d_GetFreeSpaceInStaticArray(array);`
 * This returns how many more elements can be added to the array.
 */
size_t d_GetFreeSpaceInStaticArray(dStaticArray_t* array)
{
    // Input validation
    if (!array) {
        d_LogError("Attempted to get free space from a NULL static array.");
        return 0; // No space available
    }

    // Calculate free space
    if (array->capacity > array->count) {
        return array->capacity - array->count;
    } else {
        // Array is full or over-counted (should not happen with proper usage)
        return 0;
    }
}

/**
 * @brief Fill the static array with multiple copies of a given value
 *
 * @param array: Pointer to static array
 * @param value: Pointer to the value to copy into each element
 * @param num_elements: Number of elements to fill
 * @return: 0 on success, 1 on failure
 *
 * -- Fails if array or value is NULL
 * -- Fails if num_elements exceeds array capacity
 * -- Copies element_size bytes from value to each position
 * -- Sets array count to num_elements after successful fill
 * -- Overwrites existing data in the array
 * 
 * Example: `int value = 42; d_FillDataInStaticArray(array, &value, 5);`
 * This fills the first 5 positions of the array with the value 42.
 */
int d_FillDataInStaticArray(dStaticArray_t* array, const void* value, size_t num_elements)
{
    // Input validation
    if (!array || !value) {
        d_LogError("Invalid input: array or value is NULL for fill operation.");
        return 1; // Failure
    }

    if (num_elements == 0) {
        d_LogDebug("Fill operation requested for zero elements. Doing nothing.");
        return 0; // Success, nothing to do
    }

    if (num_elements > array->capacity) {
        d_LogError("Attempted to fill more elements than static array's capacity.");
        return 1; // Failure, would cause overflow
    }

    // Fill the array with copies of the value
    for (size_t i = 0; i < num_elements; i++) {
        char* destination_address = (char*)array->data + (i * array->element_size);
        memcpy(destination_address, value, array->element_size);
    }

    // Update count to reflect the number of filled elements
    array->count = num_elements;

    return 0; // Success
}

/**
 * @brief Get direct access to the raw memory buffer of the static array
 *
 * @param array: Pointer to static array
 * @return: Pointer to the start of the raw data buffer, or NULL on error
 *
 * -- Returns NULL if array is NULL or data buffer is NULL
 * -- Provides access to entire allocated buffer regardless of current count
 * -- Returned pointer is valid until array is destroyed
 * -- Use with caution - direct memory access bypasses safety checks
 * -- Primarily intended for advanced operations or serialization
 * 
 * Example: `void* raw_buffer = d_PeekRawMemoryOfStaticArray(array);`
 * This returns a pointer to the entire data buffer of the array.
 */
void* d_PeekRawMemoryOfStaticArray(dStaticArray_t* array)
{
    // Input validation
    if (!array) {
        d_LogError("Attempted to peek raw memory of a NULL static array.");
        return NULL;
    }

    if (!array->data) {
        d_LogError("Static array data buffer is NULL. Likely uninitialized or destroyed.");
        return NULL;
    }

    // Return pointer to the raw buffer
    return array->data;
}