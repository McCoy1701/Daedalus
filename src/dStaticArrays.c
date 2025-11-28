#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Daedalus.h"

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

int d_StaticArrayDestroy(dStaticArray_t* array)
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

int d_StaticArrayAppend(dStaticArray_t* array, void* data)
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

void* d_StaticArrayGet(dStaticArray_t* array, size_t index)
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

void* d_StaticArrayPop(dStaticArray_t* array)
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
 * Example: `size_t free_slots = d_StaticArrayGetFreeSpace(array);`
 * This returns how many more elements can be added to the array.
 */
size_t d_StaticArrayGetFreeSpace(dStaticArray_t* array)
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
 * Example: `int value = 42; d_StaticArrayFill(array, &value, 5);`
 * This fills the first 5 positions of the array with the value 42.
 */
int d_StaticArrayFill(dStaticArray_t* array, const void* value, size_t num_elements)
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
 * Example: `void* raw_buffer = d_StaticArrayPeekRawMemory(array);`
 * This returns a pointer to the entire data buffer of the array.
 */
void* d_StaticArrayPeekRawMemory(dStaticArray_t* array)
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

// =============================================================================
// STATIC ARRAY PERSISTENCE AND ITERATION
// =============================================================================

/**
 * @brief Save a static array to a binary file
 *
 * Binary file format:
 * - Magic number (4 bytes): 0xDAEDARRA (Daedalus Array)
 * - Version (4 bytes): File format version (currently 1)
 * - Capacity (8 bytes): Array capacity
 * - Count (8 bytes): Current number of elements
 * - Element size (8 bytes): Size of each element in bytes
 * - Data (capacity * element_size bytes): Raw array data
 *
 * @param filename Path to the file where the array should be saved
 * @param array Pointer to the static array to save
 * @return 0 on success, 1 on failure
 */
int d_StaticArraySaveToFile(const char* filename, const dStaticArray_t* array)
{
    // Input validation
    if (!filename || !array) {
        d_LogError("Invalid parameters for saving static array to file.");
        return 1;
    }

    if (!array->data) {
        d_LogError("Static array data buffer is NULL - cannot save uninitialized array.");
        return 1;
    }

    // Open file for binary writing
    FILE* file = fopen(filename, "wb");
    if (!file) {
        d_LogErrorF("Failed to open file for writing: %s", filename);
        return 1;
    }

    // Write file header
    const uint32_t magic_number = 0xDAEDDDCA; // Daedalus Array magic
    const uint32_t version = 1;

    if (fwrite(&magic_number, sizeof(uint32_t), 1, file) != 1) {
        d_LogError("Failed to write magic number to file.");
        fclose(file);
        return 1;
    }

    if (fwrite(&version, sizeof(uint32_t), 1, file) != 1) {
        d_LogError("Failed to write version to file.");
        fclose(file);
        return 1;
    }

    // Write array metadata
    if (fwrite(&array->capacity, sizeof(size_t), 1, file) != 1) {
        d_LogError("Failed to write array capacity to file.");
        fclose(file);
        return 1;
    }

    if (fwrite(&array->count, sizeof(size_t), 1, file) != 1) {
        d_LogError("Failed to write array count to file.");
        fclose(file);
        return 1;
    }

    if (fwrite(&array->element_size, sizeof(size_t), 1, file) != 1) {
        d_LogError("Failed to write element size to file.");
        fclose(file);
        return 1;
    }

    // Write array data (entire capacity, not just count)
    size_t total_data_size = array->capacity * array->element_size;
    if (fwrite(array->data, 1, total_data_size, file) != total_data_size) {
        d_LogError("Failed to write array data to file.");
        fclose(file);
        return 1;
    }

    fclose(file);
    d_LogInfo("Successfully saved static array to file.");
    return 0;
}

/**
 * @brief Load a static array from a binary file
 *
 * @param filename Path to the file containing the saved array
 * @return Pointer to new static array, or NULL on failure
 */
dStaticArray_t* d_LoadStaticArrayFromFile(const char* filename)
{
    // Input validation
    if (!filename) {
        d_LogError("Invalid filename for loading static array from file.");
        return NULL;
    }

    // Open file for binary reading
    FILE* file = fopen(filename, "rb");
    if (!file) {
        d_LogErrorF("Failed to open file for reading: %s", filename);
        return NULL;
    }

    // Read and validate file header
    uint32_t magic_number, version;

    if (fread(&magic_number, sizeof(uint32_t), 1, file) != 1) {
        d_LogErrorF("Failed to read magic number from file: %s", filename);
        fclose(file);
        return NULL;
    }

    if (magic_number != 0xDAEDDDCA) {
        d_LogErrorF("Invalid magic number in file: %s (expected Daedalus Array format)", filename);
        fclose(file);
        return NULL;
    }

    if (fread(&version, sizeof(uint32_t), 1, file) != 1) {
        d_LogErrorF("Failed to read version from file: %s", filename);
        fclose(file);
        return NULL;
    }

    if (version != 1) {
        d_LogErrorF("Unsupported file version: %u (expected 1)", version);
        fclose(file);
        return NULL;
    }

    // Read array metadata
    size_t capacity, count, element_size;

    if (fread(&capacity, sizeof(size_t), 1, file) != 1) {
        d_LogErrorF("Failed to read array capacity from file: %s", filename);
        fclose(file);
        return NULL;
    }

    if (fread(&count, sizeof(size_t), 1, file) != 1) {
        d_LogErrorF("Failed to read array count from file: %s", filename);
        fclose(file);
        return NULL;
    }

    if (fread(&element_size, sizeof(size_t), 1, file) != 1) {
        d_LogErrorF("Failed to read element size from file: %s", filename);
        fclose(file);
        return NULL;
    }

    // Validate metadata
    if (capacity == 0 || element_size == 0) {
        d_LogErrorF("Invalid array metadata in file: %s (capacity=%zu, element_size=%zu)", 
                   filename, capacity, element_size);
        fclose(file);
        return NULL;
    }

    if (count > capacity) {
        d_LogErrorF("Invalid array metadata: count (%zu) exceeds capacity (%zu)", count, capacity);
        fclose(file);
        return NULL;
    }

    // Create new array with loaded metadata
    dStaticArray_t* array = d_InitStaticArray(capacity, element_size);
    if (!array) {
        d_LogError("Failed to allocate memory for loaded static array");
        fclose(file);
        return NULL;
    }

    // Read array data
    size_t total_data_size = capacity * element_size;
    if (fread(array->data, 1, total_data_size, file) != total_data_size) {
        d_LogErrorF("Failed to read array data from file: %s", filename);
        d_StaticArrayDestroy(array);
        fclose(file);
        return NULL;
    }

    // Set the count to the loaded value
    array->count = count;

    fclose(file);
    d_LogInfoF("Successfully loaded static array from file: %s (capacity=%zu, count=%zu, element_size=%zu)", 
              filename, capacity, count, element_size);
    return array;
}

/**
 * @brief Iterate over all elements in a static array
 *
 * @param array Pointer to the static array to iterate
 * @param callback Function to call for each element
 * @param user_data Optional data to pass to the callback
 * @return 0 on success, 1 on failure
 */
int d_StaticArrayIterate(const dStaticArray_t* array, dStaticArrayIteratorFunc callback, void* user_data)
{
    // Input validation
    if (!array || !callback) {
        d_LogError("Invalid parameters for static array iteration.");
        return 1;
    }

    if (!array->data) {
        d_LogError("Static array data buffer is NULL - cannot iterate uninitialized array.");
        return 1;
    }

    // Iterate through all elements up to count
    for (size_t i = 0; i < array->count; i++) {
        // Calculate element pointer
        const char* element_ptr = (const char*)array->data + (i * array->element_size);
        
        // Call user callback with element information
        callback(i, element_ptr, array->element_size, user_data);
    }

    d_LogDebugF("Successfully iterated over %zu elements in static array", array->count);
    return 0;
}