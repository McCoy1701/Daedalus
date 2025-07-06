// File: src/dFunctions.c - Built-in utility functions for Daedalus Library
// Provides default hashing and comparison functions for common data types

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "Daedalus.h"

// =============================================================================
// BUILT-IN HASH FUNCTIONS
// =============================================================================

/**
 * @brief Hash function for 32-bit integers using Knuth's multiplicative method.
 *
 * Uses Knuth's magic constant for excellent distribution with integer keys.
 * Suitable for most general-purpose integer hashing needs.
 *
 * @param key Pointer to integer key
 * @param key_size Size of the key (should be sizeof(int))
 * @return Hash value
 *
 * Example:
 * ```c
 * dTable_t* table = d_InitTable(sizeof(int), sizeof(char*), 
 *                               d_HashInt, d_CompareInt, 16);
 * ```
 */
size_t d_HashInt(const void* key, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key) return 0;
    
    int* int_key = (int*)key;
    return (size_t)(*int_key * 2654435761U); // Knuth's multiplicative constant
}

/**
 * @brief Hash function for null-terminated strings using FNV-1a algorithm.
 *
 * FNV-1a provides excellent distribution for string data with good performance.
 * Works with any null-terminated string regardless of the key_size parameter.
 *
 * @param key Pointer to null-terminated string
 * @param key_size Size parameter (can be 0 for null-terminated strings)
 * @return Hash value
 *
 * Example:
 * ```c
 * dTable_t* table = d_InitTable(sizeof(char*), sizeof(int), 
 *                               d_HashString, d_CompareString, 16);
 * ```
 */
size_t d_HashString(const void* key, size_t key_size)
{
    if (!key) return 0;
    
    const char* str = *(const char**)key;
    if (!str) return 0;
    
    const unsigned char* data = (const unsigned char*)str;
    size_t hash = 2166136261U; // FNV-1a offset basis
    
    while (*data) {
        hash ^= *data++;
        hash *= 16777619U; // FNV-1a prime
    }
    
    (void)key_size; // Unused for null-terminated strings
    return hash;
}

/**
 * @brief Hash function for string literals (const char*).
 *
 * Similar to d_HashString but expects the key to be a char* directly,
 * not a pointer to a char*. Useful for string literal keys.
 *
 * @param key Pointer to string (char*)
 * @param key_size Length of the string (can be 0 for null-terminated)
 * @return Hash value
 */
size_t d_HashStringLiteral(const void* key, size_t key_size)
{
    if (!key) return 0;
    
    const char* str = (const char*)key;
    const unsigned char* data = (const unsigned char*)str;
    size_t hash = 2166136261U; // FNV-1a offset basis
    
    if (key_size > 0) {
        // Use specified length
        for (size_t i = 0; i < key_size && data[i]; i++) {
            hash ^= data[i];
            hash *= 16777619U; // FNV-1a prime
        }
    } else {
        // Use null termination
        while (*data) {
            hash ^= *data++;
            hash *= 16777619U; // FNV-1a prime
        }
    }
    
    return hash;
}

/**
 * @brief Hash function for floating-point numbers.
 *
 * Converts float to integer representation for hashing.
 * Note: Be careful with floating-point precision when using as keys.
 *
 * @param key Pointer to float value
 * @param key_size Size of the key (should be sizeof(float))
 * @return Hash value
 *
 * Example:
 * ```c
 * dTable_t* table = d_InitTable(sizeof(float), sizeof(int), 
 *                               d_HashFloat, d_CompareFloat, 16);
 * ```
 */
size_t d_HashFloat(const void* key, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key) return 0;
    
    float* float_key = (float*)key;
    
    // Handle special cases
    if (*float_key == 0.0f) return 0; // +0.0 and -0.0 should hash the same
    
    // Convert to integer representation for hashing
    union { float f; uint32_t i; } converter;
    converter.f = *float_key;
    
    // Apply Knuth's multiplicative hash to the bit pattern
    return (size_t)(converter.i * 2654435761U);
}

/**
 * @brief Hash function for double-precision floating-point numbers.
 *
 * Converts double to integer representation for hashing.
 * Note: Be careful with floating-point precision when using as keys.
 *
 * @param key Pointer to double value
 * @param key_size Size of the key (should be sizeof(double))
 * @return Hash value
 */
size_t d_HashDouble(const void* key, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key) return 0;
    
    double* double_key = (double*)key;
    
    // Handle special cases
    if (*double_key == 0.0) return 0; // +0.0 and -0.0 should hash the same
    
    // Convert to integer representation for hashing
    union { double d; uint64_t i; } converter;
    converter.d = *double_key;
    
    // Mix the high and low 32 bits
    uint32_t high = (uint32_t)(converter.i >> 32);
    uint32_t low = (uint32_t)(converter.i & 0xFFFFFFFF);
    
    return (size_t)((high ^ low) * 2654435761U);
}

/**
 * @brief General-purpose hash function for binary data using FNV-1a.
 *
 * Can hash any binary data of specified length. Useful for structs,
 * arrays, or any fixed-size binary data.
 *
 * @param key Pointer to binary data
 * @param key_size Size of the data in bytes
 * @return Hash value
 *
 * Example:
 * ```c
 * typedef struct { int x, y; } Point;
 * dTable_t* table = d_InitTable(sizeof(Point), sizeof(int), 
 *                               d_HashBinary, d_CompareBinary, 16);
 * ```
 */
size_t d_HashBinary(const void* key, size_t key_size)
{
    if (!key || key_size == 0) return 0;
    
    const unsigned char* data = (const unsigned char*)key;
    size_t hash = 2166136261U; // FNV-1a offset basis
    
    for (size_t i = 0; i < key_size; i++) {
        hash ^= data[i];
        hash *= 16777619U; // FNV-1a prime
    }
    
    return hash;
}

/**
 * @brief Hash function for pointers (memory addresses).
 *
 * Hashes the pointer value itself, not the data it points to.
 * Useful when you want to use memory addresses as keys.
 *
 * @param key Pointer to pointer value
 * @param key_size Size of pointer (should be sizeof(void*))
 * @return Hash value
 */
size_t d_HashPointer(const void* key, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key) return 0;
    
    void** ptr_key = (void**)key;
    uintptr_t addr = (uintptr_t)(*ptr_key);
    
    // Mix the pointer bits
    if (sizeof(void*) == 8) {
        // 64-bit pointers
        uint64_t hash_val = (uint64_t)addr;
        hash_val ^= hash_val >> 33;
        hash_val *= 0xff51afd7ed558ccdULL;
        hash_val ^= hash_val >> 33;
        return (size_t)hash_val;
    } else {
        // 32-bit pointers
        return (size_t)(addr * 2654435761U);
    }
}

// =============================================================================
// BUILT-IN COMPARISON FUNCTIONS
// =============================================================================

/**
 * @brief Comparison function for 32-bit integers.
 *
 * @param key1 Pointer to first integer
 * @param key2 Pointer to second integer
 * @param key_size Size of keys (should be sizeof(int))
 * @return 0 if equal, non-zero if different
 */
int d_CompareInt(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return 1; // Different if either is NULL
    
    int* k1 = (int*)key1;
    int* k2 = (int*)key2;
    return (*k1 == *k2) ? 0 : 1;
}

/**
 * @brief Comparison function for null-terminated strings.
 *
 * Compares strings pointed to by char** keys using strcmp.
 *
 * @param key1 Pointer to first string pointer (char**)
 * @param key2 Pointer to second string pointer (char**)
 * @param key_size Size parameter (unused for null-terminated strings)
 * @return 0 if equal, non-zero if different
 */
int d_CompareString(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return 1; // Different if either is NULL
    
    const char* str1 = *(const char**)key1;
    const char* str2 = *(const char**)key2;
    
    if (!str1 || !str2) return (str1 == str2) ? 0 : 1; // Handle NULL strings
    
    return (strcmp(str1, str2) == 0) ? 0 : 1;
}

/**
 * @brief Comparison function for string literals (const char*).
 *
 * Compares strings directly, expecting keys to be char* not char**.
 *
 * @param key1 Pointer to first string (char*)
 * @param key2 Pointer to second string (char*)
 * @param key_size Length to compare (0 for full null-terminated comparison)
 * @return 0 if equal, non-zero if different
 */
int d_CompareStringLiteral(const void* key1, const void* key2, size_t key_size)
{
    if (!key1 || !key2) return (key1 == key2) ? 0 : 1;
    
    const char* str1 = (const char*)key1;
    const char* str2 = (const char*)key2;
    
    if (key_size > 0) {
        return (strncmp(str1, str2, key_size) == 0) ? 0 : 1;
    } else {
        return (strcmp(str1, str2) == 0) ? 0 : 1;
    }
}

/**
 * @brief Comparison function for floating-point numbers.
 *
 * Compares floats with exact equality. Be aware that floating-point
 * precision issues may cause problems with hash table lookups.
 *
 * @param key1 Pointer to first float
 * @param key2 Pointer to second float
 * @param key_size Size of keys (should be sizeof(float))
 * @return 0 if equal, non-zero if different
 */
int d_CompareFloat(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return 1; // Different if either is NULL
    
    float* f1 = (float*)key1;
    float* f2 = (float*)key2;
    return (*f1 == *f2) ? 0 : 1;
}

/**
 * @brief Comparison function for double-precision floating-point numbers.
 *
 * Compares doubles with exact equality. Be aware that floating-point
 * precision issues may cause problems with hash table lookups.
 *
 * @param key1 Pointer to first double
 * @param key2 Pointer to second double
 * @param key_size Size of keys (should be sizeof(double))
 * @return 0 if equal, non-zero if different
 */
int d_CompareDouble(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return 1; // Different if either is NULL
    
    double* d1 = (double*)key1;
    double* d2 = (double*)key2;
    return (*d1 == *d2) ? 0 : 1;
}

/**
 * @brief Comparison function for binary data using memcmp.
 *
 * Performs byte-by-byte comparison of binary data.
 *
 * @param key1 Pointer to first data block
 * @param key2 Pointer to second data block
 * @param key_size Size of data blocks in bytes
 * @return 0 if equal, non-zero if different
 */
int d_CompareBinary(const void* key1, const void* key2, size_t key_size)
{
    if (!key1 || !key2) return (key1 == key2) ? 0 : 1;
    if (key_size == 0) return 0; // Zero-size data is always equal
    
    return (memcmp(key1, key2, key_size) == 0) ? 0 : 1;
}

/**
 * @brief Comparison function for pointers (memory addresses).
 *
 * Compares pointer values, not the data they point to.
 *
 * @param key1 Pointer to first pointer value
 * @param key2 Pointer to second pointer value
 * @param key_size Size of pointer (should be sizeof(void*))
 * @return 0 if equal, non-zero if different
 */
int d_ComparePointer(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return 1; // Different if either is NULL
    
    void** p1 = (void**)key1;
    void** p2 = (void**)key2;
    return (*p1 == *p2) ? 0 : 1;
}

// =============================================================================
// SPECIALIZED HASH FUNCTIONS FOR COMMON PATTERNS
// =============================================================================

/**
 * @brief Hash function optimized for small positive integers (0-999).
 *
 * Uses simple multiplication with good distribution for small integer ranges.
 * More efficient than general-purpose hashing for known small ranges.
 *
 * @param key Pointer to integer key
 * @param key_size Size of the key (should be sizeof(int))
 * @return Hash value
 */
size_t d_HashSmallInt(const void* key, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key) return 0;
    
    int* int_key = (int*)key;
    return (size_t)(*int_key * 31); // Simple but effective for small ranges
}

/**
 * @brief Hash function for case-insensitive strings.
 *
 * Converts characters to lowercase before hashing for case-insensitive keys.
 *
 * @param key Pointer to string pointer (char**)
 * @param key_size Size parameter (unused for null-terminated strings)
 * @return Hash value
 */
size_t d_HashStringCaseInsensitive(const void* key, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key) return 0;
    
    const char* str = *(const char**)key;
    if (!str) return 0;
    
    size_t hash = 2166136261U; // FNV-1a offset basis
    
    while (*str) {
        // Convert to lowercase before hashing
        char c = *str;
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }
        hash ^= (unsigned char)c;
        hash *= 16777619U; // FNV-1a prime
        str++;
    }
    
    return hash;
}

/**
 * @brief Comparison function for case-insensitive strings.
 *
 * Performs case-insensitive string comparison.
 *
 * @param key1 Pointer to first string pointer (char**)
 * @param key2 Pointer to second string pointer (char**)
 * @param key_size Size parameter (unused for null-terminated strings)
 * @return 0 if equal, non-zero if different
 */
int d_CompareStringCaseInsensitive(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return 1; // Different if either is NULL
    
    const char* str1 = *(const char**)key1;
    const char* str2 = *(const char**)key2;
    
    if (!str1 || !str2) return (str1 == str2) ? 0 : 1; // Handle NULL strings
    
    // Manual case-insensitive comparison (strcasecmp not available everywhere)
    while (*str1 && *str2) {
        char c1 = *str1;
        char c2 = *str2;
        
        // Convert to lowercase
        if (c1 >= 'A' && c1 <= 'Z') c1 = c1 - 'A' + 'a';
        if (c2 >= 'A' && c2 <= 'Z') c2 = c2 - 'A' + 'a';
        
        if (c1 != c2) return 1; // Different
        
        str1++;
        str2++;
    }
    
    return (*str1 == *str2) ? 0 : 1; // Check if both reached end
}

// =============================================================================
// DSTRING HASH AND COMPARISON FUNCTIONS
// =============================================================================

/**
 * @brief Hash function for dString_t objects.
 *
 * Hashes the string content of a dString_t using FNV-1a algorithm.
 * Perfect for using dString_t objects as hash table keys.
 *
 * @param key Pointer to dString_t* (pointer to dString_t pointer)
 * @param key_size Size parameter (unused for dString_t)
 * @return Hash value
 *
 * Example:
 * ```c
 * dTable_t* table = d_InitTable(sizeof(dString_t*), sizeof(int), 
 *                               d_HashDString, d_CompareDString, 16);
 * ```
 */
size_t d_HashDString(const void* key, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key) return 0;
    
    const dString_t* const* dstr_ptr = (const dString_t* const*)key;
    const dString_t* dstr = *dstr_ptr;
    
    if (!dstr || !dstr->str || dstr->len == 0) return 0;
    
    const unsigned char* data = (const unsigned char*)dstr->str;
    size_t hash = 2166136261U; // FNV-1a offset basis
    
    // Hash exactly the length stored in dString_t for consistency
    for (size_t i = 0; i < dstr->len; i++) {
        hash ^= data[i];
        hash *= 16777619U; // FNV-1a prime
    }
    
    return hash;
}

/**
 * @brief Comparison function for dString_t objects.
 *
 * Compares the string content of two dString_t objects.
 * Uses the existing d_CompareStrings function internally.
 *
 * @param key1 Pointer to first dString_t* (pointer to dString_t pointer)
 * @param key2 Pointer to second dString_t* (pointer to dString_t pointer)
 * @param key_size Size parameter (unused for dString_t)
 * @return 0 if equal, non-zero if different
 */
int d_CompareDString(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return 1; // Different if either is NULL
    
    const dString_t* const* dstr1_ptr = (const dString_t* const*)key1;
    const dString_t* const* dstr2_ptr = (const dString_t* const*)key2;
    
    const dString_t* dstr1 = *dstr1_ptr;
    const dString_t* dstr2 = *dstr2_ptr;
    
    // Use existing dString comparison function
    return (d_CompareStrings(dstr1, dstr2) == 0) ? 0 : 1;
}

// =============================================================================
// DARRAY AND DSTATICARRAY COMPARISON FUNCTIONS
// =============================================================================

/**
 * @brief Comparison function for dArray_t objects.
 *
 * Compares two dArray_t objects by checking their structural properties
 * (element_size, count) and then performing element-by-element comparison
 * using memcmp for the raw data.
 *
 * @param key1 Pointer to first dArray_t* (pointer to dArray_t pointer)
 * @param key2 Pointer to second dArray_t* (pointer to dArray_t pointer)
 * @param key_size Size parameter (unused for dArray_t)
 * @return 0 if equal, non-zero if different
 *
 * Example:
 * ```c
 * dTable_t* table = d_InitTable(sizeof(dArray_t*), sizeof(int), 
 *                               d_HashBinary, d_CompareDArray, 16);
 * ```
 */
int d_CompareDArray(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return (key1 == key2) ? 0 : 1; // Handle NULL pointers
    
    const dArray_t* const* arr1_ptr = (const dArray_t* const*)key1;
    const dArray_t* const* arr2_ptr = (const dArray_t* const*)key2;
    
    const dArray_t* arr1 = *arr1_ptr;
    const dArray_t* arr2 = *arr2_ptr;
    
    if (!arr1 || !arr2) return (arr1 == arr2) ? 0 : 1; // Handle NULL arrays
    
    // Check structural compatibility first
    if (arr1->element_size != arr2->element_size) return 1;
    if (arr1->count != arr2->count) return 1;
    
    // Empty arrays are equal if they have the same element_size
    if (arr1->count == 0) return 0;
    
    // Compare the raw data using memcmp
    size_t total_size = arr1->count * arr1->element_size;
    return (memcmp(arr1->data, arr2->data, total_size) == 0) ? 0 : 1;
}

/**
 * @brief Comparison function for dStaticArray_t objects.
 *
 * Compares two dStaticArray_t objects by checking their structural properties
 * (element_size, count) and then performing element-by-element comparison
 * using memcmp for the raw data.
 *
 * @param key1 Pointer to first dStaticArray_t* (pointer to dStaticArray_t pointer)
 * @param key2 Pointer to second dStaticArray_t* (pointer to dStaticArray_t pointer)
 * @param key_size Size parameter (unused for dStaticArray_t)
 * @return 0 if equal, non-zero if different
 */
int d_CompareStaticArray(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return (key1 == key2) ? 0 : 1; // Handle NULL pointers
    
    const dStaticArray_t* const* arr1_ptr = (const dStaticArray_t* const*)key1;
    const dStaticArray_t* const* arr2_ptr = (const dStaticArray_t* const*)key2;
    
    const dStaticArray_t* arr1 = *arr1_ptr;
    const dStaticArray_t* arr2 = *arr2_ptr;
    
    if (!arr1 || !arr2) return (arr1 == arr2) ? 0 : 1; // Handle NULL arrays
    
    // Check structural compatibility first
    if (arr1->element_size != arr2->element_size) return 1;
    if (arr1->count != arr2->count) return 1;
    
    // Empty arrays are equal if they have the same element_size
    if (arr1->count == 0) return 0;
    
    // Compare the raw data using memcmp
    size_t total_size = arr1->count * arr1->element_size;
    return (memcmp(arr1->data, arr2->data, total_size) == 0) ? 0 : 1;
}

// =============================================================================
// DTABLE AND DSTATICTABLE COMPARISON FUNCTIONS
// =============================================================================

/**
 * @brief Comparison function for dTable_t objects.
 *
 * Compares two dTable_t objects by checking their structural properties
 * (key_size, value_size, count) and then comparing all key-value pairs.
 * Since hash tables are unordered, this performs a comprehensive comparison
 * by extracting all keys and values and comparing them systematically.
 *
 * @param key1 Pointer to first dTable_t* (pointer to dTable_t pointer)
 * @param key2 Pointer to second dTable_t* (pointer to dTable_t pointer)
 * @param key_size Size parameter (unused for dTable_t)
 * @return 0 if equal, non-zero if different
 */
int d_CompareTable(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return (key1 == key2) ? 0 : 1; // Handle NULL pointers
    
    const dTable_t* const* table1_ptr = (const dTable_t* const*)key1;
    const dTable_t* const* table2_ptr = (const dTable_t* const*)key2;
    
    dTable_t* table1 = *((dTable_t**)table1_ptr);
    dTable_t* table2 = *((dTable_t**)table2_ptr);
    
    if (!table1 || !table2) return (table1 == table2) ? 0 : 1; // Handle NULL tables
    
    // Check structural compatibility first
    if (table1->key_size != table2->key_size) return 1;
    if (table1->value_size != table2->value_size) return 1;
    if (table1->count != table2->count) return 1;
    
    // Empty tables are equal if they have the same key_size and value_size
    if (table1->count == 0) return 0;
    
    // Get all keys and values from table1
    dArray_t* keys1 = d_GetAllKeysFromTable(table1);
    dArray_t* values1 = d_GetAllValuesFromTable(table1);
    
    if (!keys1 || !values1) {
        if (keys1) d_DestroyArray(keys1);
        if (values1) d_DestroyArray(values1);
        return 1; // Error occurred
    }
    
    // For each key-value pair in table1, check if it exists in table2 with the same value
    for (size_t i = 0; i < table1->count; i++) {
        void* key_data = d_IndexDataFromArray(keys1, i);
        void* value_data = d_IndexDataFromArray(values1, i);
        void* corresponding_value = d_GetDataFromTable(table2, key_data);
        if (!corresponding_value) {
            // Key not found in table2
            d_DestroyArray(keys1);
            d_DestroyArray(values1);
            return 1;
        }
        
        // Compare the values
        if (memcmp(value_data, corresponding_value, table1->value_size) != 0) {
            d_DestroyArray(keys1);
            d_DestroyArray(values1);
            return 1;
        }
    }
    
    d_DestroyArray(keys1);
    d_DestroyArray(values1);
    return 0; // All key-value pairs match
}

/**
 * @brief Comparison function for dStaticTable_t objects.
 *
 * Compares two dStaticTable_t objects by checking their structural properties
 * (key_size, value_size, count) and then comparing all key-value pairs.
 * Since hash tables are unordered, this performs a comprehensive comparison
 * by extracting all keys and values and comparing them systematically.
 *
 * @param key1 Pointer to first dStaticTable_t* (pointer to dStaticTable_t pointer)
 * @param key2 Pointer to second dStaticTable_t* (pointer to dStaticTable_t pointer)
 * @param key_size Size parameter (unused for dStaticTable_t)
 * @return 0 if equal, non-zero if different
 */
int d_CompareStaticTable(const void* key1, const void* key2, size_t key_size)
{
    (void)key_size; // Unused parameter
    if (!key1 || !key2) return (key1 == key2) ? 0 : 1; // Handle NULL pointers
    
    const dStaticTable_t* const* table1_ptr = (const dStaticTable_t* const*)key1;
    const dStaticTable_t* const* table2_ptr = (const dStaticTable_t* const*)key2;
    
    dStaticTable_t* table1 = *((dStaticTable_t**)table1_ptr);
    dStaticTable_t* table2 = *((dStaticTable_t**)table2_ptr);
    
    if (!table1 || !table2) return (table1 == table2) ? 0 : 1; // Handle NULL tables
    
    // Check structural compatibility first
    if (table1->key_size != table2->key_size) return 1;
    if (table1->value_size != table2->value_size) return 1;
    if (table1->num_keys != table2->num_keys) return 1;
    
    // Empty tables are equal if they have the same key_size and value_size
    if (table1->num_keys == 0) return 0;
    
    // Get all keys and values from table1
    dArray_t* keys1 = d_GetAllKeysFromStaticTable(table1);
    dArray_t* values1 = d_GetAllValuesFromStaticTable(table1);
    
    if (!keys1 || !values1) {
        if (keys1) d_DestroyArray(keys1);
        if (values1) d_DestroyArray(values1);
        return 1; // Error occurred
    }
    
    // For each key-value pair in table1, check if it exists in table2 with the same value
    for (size_t i = 0; i < table1->num_keys; i++) {
        void* key_data = d_IndexDataFromArray(keys1, i);
        void* value_data = d_IndexDataFromArray(values1, i);
        void* corresponding_value = d_GetValueInStaticTable(table2, key_data);
        if (!corresponding_value) {
            // Key not found in table2
            d_DestroyArray(keys1);
            d_DestroyArray(values1);
            return 1;
        }
        
        // Compare the values
        if (memcmp(value_data, corresponding_value, table1->value_size) != 0) {
            d_DestroyArray(keys1);
            d_DestroyArray(values1);
            return 1;
        }
    }
    
    d_DestroyArray(keys1);
    d_DestroyArray(values1);
    return 0; // All key-value pairs match
}

