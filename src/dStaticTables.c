// File: src/dStaticTables.c - Fixed-Structure Hash Table Implementation for Daedalus Library
// Hash table with immutable key set and optimized for known key structures

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "Daedalus.h"

// =============================================================================
// INTERNAL HELPER FUNCTIONS (reuse from dTables.c)
// =============================================================================

/**
 * @brief Internal helper: Create a new hash table entry for static table.
 *
 * Allocates memory for a dTableEntry_t and copies the provided key and value
 * data into it. This entry will be stored in a linked list bucket.
 *
 * @param key Pointer to the key data to copy
 * @param key_size Size of the key data in bytes
 * @param value Pointer to the value data to copy
 * @param value_size Size of the value data in bytes
 *
 * @return Pointer to new dTableEntry_t, or NULL on failure
 */
static dTableEntry_t* _d_CreateStaticTableEntry(const void* key, size_t key_size, 
                                                 const void* value, size_t value_size)
{
    if (!key || !value || key_size == 0 || value_size == 0) {
        d_LogError("Invalid parameters for creating static table entry.");
        return NULL;
    }

    dTableEntry_t* entry = (dTableEntry_t*)malloc(sizeof(dTableEntry_t));
    if (!entry) {
        d_LogError("Failed to allocate memory for static table entry structure.");
        return NULL;
    }

    // Allocate and copy key data
    entry->key_data = malloc(key_size);
    if (!entry->key_data) {
        d_LogError("Failed to allocate memory for static table entry key data.");
        free(entry);
        return NULL;
    }
    memcpy(entry->key_data, key, key_size);

    // Allocate and copy value data
    entry->value_data = malloc(value_size);
    if (!entry->value_data) {
        d_LogError("Failed to allocate memory for static table entry value data.");
        free(entry->key_data);
        free(entry);
        return NULL;
    }
    memcpy(entry->value_data, value, value_size);

    return entry;
}

/**
 * @brief Internal helper: Destroy a static hash table entry.
 *
 * Frees all memory associated with a dTableEntry_t including the key data,
 * value data, and the entry structure itself.
 *
 * @param entry Pointer to the entry to destroy
 */
static void _d_DestroyStaticTableEntry(dTableEntry_t* entry)
{
    if (!entry) return;

    if (entry->key_data) free(entry->key_data);
    if (entry->value_data) free(entry->value_data);
    free(entry);
}

/**
 * @brief Internal helper: Find an entry in a bucket by key for static table.
 *
 * Searches through a linked list bucket to find an entry with a matching key
 * using the provided comparison function.
 *
 * @param bucket_head Head of the linked list bucket
 * @param key Key to search for
 * @param key_size Size of the key
 * @param compare_func Function to compare keys
 *
 * @return Pointer to the matching dTableEntry_t, or NULL if not found
 */
static dTableEntry_t* _d_FindEntryInStaticBucket(dLinkedList_t* bucket_head, const void* key, 
                                                  size_t key_size, dTableCompareFunc compare_func)
{
    if (!bucket_head || !key || !compare_func) return NULL;

    dLinkedList_t* current = bucket_head;
    while (current != NULL) {
        dTableEntry_t* entry = (dTableEntry_t*)current->data;
        if (entry && compare_func(entry->key_data, key, key_size) == 0) {
            return entry;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * @brief Internal helper: Generate a unique name for linked list storage.
 *
 * Creates a unique string name for storing entries in linked lists based on
 * memory address to ensure uniqueness.
 *
 * @param entry Pointer to the entry
 * @param name_buffer Buffer to store the generated name
 * @param buffer_size Size of the name buffer
 */
static void _d_GenerateStaticEntryName(const dTableEntry_t* entry, char* name_buffer, size_t buffer_size)
{
    snprintf(name_buffer, buffer_size, "static_entry_%p", (void*)entry);
}

// =============================================================================
// STATIC HASH TABLE CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Initialize a new static hash table with fixed key structure and initial data.
 *
 * Creates and fully initializes a dStaticTable_t structure with the specified parameters
 * and populates it with the provided keys and initial values. After this function succeeds,
 * the table is ready for use with only value update operations allowed.
 *
 * @param key_size The size in bytes of the keys that will be stored
 * @param value_size The size in bytes of the values that will be stored
 * @param hash_func A pointer to the user-provided hashing function
 * @param compare_func A pointer to the user-provided key comparison function
 * @param num_buckets The number of buckets for the table (should be optimized for expected key count)
 * @param keys Array of key data to initialize with (must contain exactly num_keys elements)
 * @param initial_values Array of initial value data (must contain exactly num_keys elements, parallel to keys)
 * @param num_keys Number of elements in both keys and initial_values arrays
 *
 * @return A pointer to the newly initialized dStaticTable_t instance, or NULL on failure
 *
 * @warning CRITICAL: The keys and initial_values arrays MUST contain exactly num_keys elements.
 *          C cannot validate array sizes at runtime - it is the caller's responsibility to ensure
 *          both arrays have the correct number of elements. Mismatched sizes will cause undefined behavior.
 *
 * @note The num_keys parameter is required because C arrays decay to pointers when passed as parameters,
 *       making automatic size detection impossible.
 *
 * Example:
 * ```c
 * int keys[] = {1, 2, 3}; 
 * char* values[] = {"a", "b", "c"};
 * size_t count = sizeof(keys)/sizeof(keys[0]); // Calculate array size safely
 * dStaticTable_t* table = d_InitStaticTable(sizeof(int), sizeof(char*), my_hash, my_compare, 16, 
 *                                           (const void**)keys, (const void**)values, count);
 * ```
 */
dStaticTable_t* d_InitStaticTable(size_t key_size, size_t value_size, dTableHashFunc hash_func,
                                  dTableCompareFunc compare_func, size_t num_buckets,
                                  const void** keys, const void** initial_values, size_t num_keys)
{
    if (key_size == 0 || value_size == 0 || !hash_func || !compare_func || num_buckets == 0 || !keys || !initial_values || num_keys == 0) {
        d_LogError("Invalid parameters for static hash table initialization.");
        return NULL;
    }

    dStaticTable_t* table = (dStaticTable_t*)malloc(sizeof(dStaticTable_t));
    if (!table) {
        d_LogError("Failed to allocate memory for static hash table structure.");
        return NULL;
    }

    // Allocate buckets array using dArray_t as per header definition
    table->buckets = d_InitArray(num_buckets, sizeof(dLinkedList_t*));
    if (!table->buckets) {
        d_LogError("Failed to allocate memory for static hash table buckets array.");
        free(table);
        return NULL;
    }

    // Initialize all bucket pointers to NULL
    for (size_t i = 0; i < num_buckets; i++) {
        dLinkedList_t* null_ptr = NULL;
        d_AppendDataToArray(table->buckets, &null_ptr);
    }

    // Initialize table fields
    table->num_buckets = num_buckets;
    table->num_keys = 0;          // Will be set during population
    table->key_size = key_size;
    table->value_size = value_size;
    table->hash_func = hash_func;
    table->compare_func = compare_func;
    table->is_initialized = false; // Will be set after population

    // Populate table with fixed key set
    for (size_t i = 0; i < num_keys; i++) {
        if (!keys[i] || !initial_values[i]) {
            d_LogErrorF("NULL key or value at index %zu during static table initialization.", i);
            // Cleanup and return failure
            d_DestroyStaticTable(&table);
            return NULL;
        }

        // Calculate hash and bucket index
        size_t hash = table->hash_func(keys[i], table->key_size);
        size_t bucket_index = hash % table->num_buckets;

        // Get bucket pointer
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, bucket_index);
        if (!bucket_ptr) {
            d_LogError("Failed to access bucket during static table initialization.");
            d_DestroyStaticTable(&table);
            return NULL;
        }

        // Check for duplicate keys
        if (_d_FindEntryInStaticBucket(*bucket_ptr, keys[i], table->key_size, table->compare_func)) {
            d_LogErrorF("Duplicate key detected at index %zu during static table initialization.", i);
            d_DestroyStaticTable(&table);
            return NULL;
        }

        // Create new entry
        dTableEntry_t* new_entry = _d_CreateStaticTableEntry(keys[i], table->key_size, 
                                                              initial_values[i], table->value_size);
        if (!new_entry) {
            d_LogError("Failed to create entry during static table initialization.");
            d_DestroyStaticTable(&table);
            return NULL;
        }

        // Generate unique name and add to bucket
        char entry_name[64];
        _d_GenerateStaticEntryName(new_entry, entry_name, sizeof(entry_name));

        if (d_PushBackToLinkedList(bucket_ptr, new_entry, entry_name, sizeof(dTableEntry_t)) != 0) {
            d_LogErrorF("Failed to add entry to bucket %zu during static table initialization.", bucket_index);
            _d_DestroyStaticTableEntry(new_entry);
            d_DestroyStaticTable(&table);
            return NULL;
        }
    }

    // Mark table as fully initialized
    table->num_keys = num_keys;
    table->is_initialized = true;

    d_LogInfoF("Static hash table initialized with %zu fixed keys across %zu buckets.", 
               num_keys, table->num_buckets);

    return table;
}

/**
 * @brief Destroy a static hash table and free all associated memory.
 *
 * Deallocates all memory associated with a dStaticTable_t instance, including
 * all entries, buckets, and the structure itself.
 *
 * @param table A pointer to the pointer of the static hash table to destroy
 *
 * @return 0 on success, 1 on failure
 *
 * Example:
 * `d_DestroyStaticTable(&my_table); // my_table will be NULL after this`
 */
int d_DestroyStaticTable(dStaticTable_t** table)
{
    if (!table || !*table) {
        d_LogError("Attempted to destroy NULL or invalid static hash table.");
        return 1;
    }

    dStaticTable_t* t = *table;

    // Destroy all buckets and their entries
    for (size_t i = 0; i < t->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(t->buckets, i);
        if (bucket_ptr && *bucket_ptr) {
            // Manually destroy entries first, then the linked list structure
            dLinkedList_t* current = *bucket_ptr;
            while (current) {
                dTableEntry_t* entry = (dTableEntry_t*)current->data;
                if (entry) {
                    _d_DestroyStaticTableEntry(entry);
                    current->data = NULL; // Prevent double free by linked list
                }
                current = current->next;
            }
            // Now destroy the linked list structure itself
            d_DestroyLinkedList(bucket_ptr);
        }
    }

    // Free buckets array
    d_DestroyArray(t->buckets);

    // Free table structure
    free(t);

    // Set caller's pointer to NULL
    *table = NULL;

    d_LogDebug("Static hash table destroyed successfully.");
    return 0;
}

// =============================================================================
// STATIC HASH TABLE VALUE MANAGEMENT
// =============================================================================

/**
 * @brief Update the value associated with a key in the static table.
 *
 * Updates the value for an existing key in the static table. Since the key set
 * is immutable, this function only allows value updates, not key insertion.
 *
 * @param table A pointer to the static hash table
 * @param key A pointer to the key data to update
 * @param new_value A pointer to the new value data
 *
 * @return 0 on success, 1 on failure/key not found
 *
 * Example:
 * `int key = 42; char* new_value = "updated"; d_SetStaticTableValue(table, &key, &new_value);`
 */
int d_SetValueInStaticTable(dStaticTable_t* table, const void* key, const void* new_value)
{
    if (!table || !key || !new_value) {
        d_LogError("Invalid parameters for setting static table value.");
        return 1;
    }

    if (!table->is_initialized) {
        d_LogError("Attempted to set value in uninitialized static table.");
        return 1;
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, bucket_index);
    if (!bucket_ptr) {
        d_LogError("Failed to access bucket in static hash table.");
        return 1;
    }

    // Find existing entry
    dTableEntry_t* existing_entry = _d_FindEntryInStaticBucket(*bucket_ptr, key, table->key_size, table->compare_func);

    if (!existing_entry) {
        d_LogDebugF("Key not found in static table (bucket %zu). Cannot add new keys to static table.", bucket_index);
        return 1; // Key not found - cannot add new keys to static table
    }

    // Update existing entry's value
    d_LogDebugF("Updating existing key value in static hash table (bucket %zu).", bucket_index);
    
    // Free old value data
    if (existing_entry->value_data) {
        free(existing_entry->value_data);
    }
    
    // Allocate and copy new value data
    existing_entry->value_data = malloc(table->value_size);
    if (!existing_entry->value_data) {
        d_LogError("Failed to allocate memory for updated static table value data.");
        return 1;
    }
    memcpy(existing_entry->value_data, new_value, table->value_size);
    
    return 0; // Success
}

/**
 * @brief Retrieve a pointer to the value associated with a given key.
 *
 * Retrieves a pointer to the value associated with a given key from the static hash table.
 *
 * @param table A pointer to the static hash table
 * @param key A pointer to the key data to search for
 *
 * @return A void* pointer to the internally stored value data if found, or NULL if not found
 *
 * Example:
 * `int key = 42; char** value = (char**)d_GetValueInStaticTable(table, &key);`
 */
void* d_GetValueInStaticTable(const dStaticTable_t* table, const void* key)
{
    if (!table || !key) {
        d_LogError("Invalid parameters for getting static table value.");
        return NULL;
    }

    if (!table->is_initialized) {
        d_LogError("Attempted to get value from uninitialized static table.");
        return NULL;
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, bucket_index);
    if (!bucket_ptr) {
        d_LogError("Failed to access bucket in static hash table.");
        return NULL;
    }

    // Find entry in bucket
    dTableEntry_t* entry = _d_FindEntryInStaticBucket(*bucket_ptr, key, table->key_size, table->compare_func);

    if (entry) {
        // d_LogDebugF("Found key in static hash table (bucket %zu).", bucket_index);
        return entry->value_data;
    }

    d_LogDebugF("Key not found in static hash table (bucket %zu).", bucket_index);
    return NULL;
}

// =============================================================================
// STATIC HASH TABLE UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Check if a specific key exists in the static hash table.
 *
 * @param table A pointer to the static hash table
 * @param key A pointer to the key data to check for
 *
 * @return 0 if the key exists, 1 if not found or error occurred
 *
 * Example:
 * `int key = 42; if (d_CheckForKeyInStaticTable(table, &key) == 0) { // key exists }`
 */
int d_CheckForKeyInStaticTable(const dStaticTable_t* table, const void* key)
{
    if (!table || !key) {
        d_LogError("Invalid parameters for checking key existence in static hash table.");
        return 1; // Not found / error
    }

    if (!table->is_initialized) {
        d_LogError("Attempted to check key in uninitialized static table.");
        return 1;
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, bucket_index);
    if (!bucket_ptr) {
        d_LogError("Failed to access bucket in static hash table.");
        return 1;
    }

    // Find entry in bucket
    dTableEntry_t* entry = _d_FindEntryInStaticBucket(*bucket_ptr, key, table->key_size, table->compare_func);

    if (entry) {
        d_LogDebugF("Key found in static hash table (bucket %zu).", bucket_index);
        return 0; // Found
    }

    d_LogDebugF("Key not found in static hash table (bucket %zu).", bucket_index);
    return 1; // Not found
}

/**
 * @brief Get the fixed number of keys in the static hash table.
 *
 * @param table A pointer to the static hash table
 *
 * @return The total count of keys, or 0 if table is NULL or uninitialized
 *
 * Example:
 * `size_t key_count = d_GetKeyCountOfStaticTable(table);`
 */
size_t d_GetKeyCountOfStaticTable(const dStaticTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to get key count from NULL static hash table.");
        return 0;
    }

    if (!table->is_initialized) {
        d_LogError("Attempted to get key count from uninitialized static table.");
        return 0;
    }

    return table->num_keys;
}

/**
 * @brief Get an array containing all keys from the static hash table.
 *
 * @param table A pointer to the static hash table
 *
 * @return A newly allocated dArray_t containing all keys, or NULL on failure
 *
 * @note The caller is responsible for destroying the returned array with d_DestroyArray
 * @note The keys are copied into the array, not referenced
 *
 * Example:
 * `dArray_t* keys = d_GetAllKeysFromStaticTable(table);`
 * `d_DestroyArray(keys);`
 */
dArray_t* d_GetAllKeysFromStaticTable(const dStaticTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to get keys from NULL static hash table.");
        return NULL;
    }

    if (!table->is_initialized) {
        d_LogError("Attempted to get keys from uninitialized static table.");
        return NULL;
    }

    // Initialize result array with exact capacity
    dArray_t* all_keys_array = d_InitArray(table->num_keys, table->key_size);
    if (!all_keys_array) {
        d_LogError("Failed to allocate array for collecting static table keys.");
        return NULL;
    }

    size_t keys_collected = 0;

    // Iterate through all buckets
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, i);
        if (!bucket_ptr || !*bucket_ptr) {
            continue; // Empty bucket
        }

        dLinkedList_t* current_node = *bucket_ptr;
        while (current_node != NULL) {
            dTableEntry_t* entry = (dTableEntry_t*)current_node->data;
            if (entry && entry->key_data) {
                // Append key data to result array
                if (d_AppendDataToArray(all_keys_array, entry->key_data) != 0) {
                    d_LogErrorF("Failed to append key to result array at bucket %zu.", i);
                    d_DestroyArray(all_keys_array);
                    return NULL;
                }
                keys_collected++;
            }
            current_node = current_node->next;
        }
    }

    d_LogDebugF("Collected %zu keys from static table (expected: %zu).", keys_collected, table->num_keys);
    return all_keys_array;
}

/**
 * @brief Get an array containing all values from the static hash table.
 *
 * @param table A pointer to the static hash table
 *
 * @return A newly allocated dArray_t containing all values, or NULL on failure
 *
 * @note The caller is responsible for destroying the returned array with d_DestroyArray
 * @note The values are copied into the array, not referenced
 *
 * Example:
 * `dArray_t* values = d_GetAllValuesFromStaticTable(table);`
 * `d_DestroyArray(values);`
 */
dArray_t* d_GetAllValuesFromStaticTable(const dStaticTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to get values from NULL static hash table.");
        return NULL;
    }

    if (!table->is_initialized) {
        d_LogError("Attempted to get values from uninitialized static table.");
        return NULL;
    }

    // Initialize result array with exact capacity
    dArray_t* all_values_array = d_InitArray(table->num_keys, table->value_size);
    if (!all_values_array) {
        d_LogError("Failed to allocate array for collecting static table values.");
        return NULL;
    }

    size_t values_collected = 0;

    // Iterate through all buckets
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, i);
        if (!bucket_ptr || !*bucket_ptr) {
            continue; // Empty bucket
        }

        dLinkedList_t* current_node = *bucket_ptr;
        while (current_node != NULL) {
            dTableEntry_t* entry = (dTableEntry_t*)current_node->data;
            if (entry && entry->value_data) {
                // Append value data to result array
                if (d_AppendDataToArray(all_values_array, entry->value_data) != 0) {
                    d_LogErrorF("Failed to append value to result array at bucket %zu.", i);
                    d_DestroyArray(all_values_array);
                    return NULL;
                }
                values_collected++;
            }
            current_node = current_node->next;
        }
    }

    d_LogDebugF("Collected %zu values from static table (expected: %zu).", values_collected, table->num_keys);
    return all_values_array;
}

// =============================================================================
// ADVANCED STATIC HASH TABLE UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Clear all entries from the static hash table but keep the structure intact.
 *
 * Empties the static table, removing all key-value pairs and resetting it to
 * an uninitialized state. The table structure is preserved for reuse with
 * d_InitKeysInStaticTable() or by creating a new table with d_InitStaticTable().
 *
 * @param table A pointer to the static hash table to clear
 *
 * @return 0 on success, 1 on failure
 *
 * @note After this operation, the table will be in an uninitialized state
 * @note The bucket structure is preserved, only the entries are removed
 * @note To reuse the table, you must reinitialize it with keys
 *
 * Example:
 * `d_ClearStaticTable(table); // Table is now empty and uninitialized`
 */
int d_ClearStaticTable(dStaticTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to clear NULL static hash table.");
        return 1;
    }

    // Clear all buckets
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, i);
        if (bucket_ptr && *bucket_ptr) {
            // Manually destroy entries first, then the linked list structure
            dLinkedList_t* current = *bucket_ptr;
            while (current) {
                dTableEntry_t* entry = (dTableEntry_t*)current->data;
                if (entry) {
                    _d_DestroyStaticTableEntry(entry);
                    current->data = NULL; // Prevent double free by linked list
                }
                current = current->next;
            }
            // Destroy the linked list
            d_DestroyLinkedList(bucket_ptr);
        }
    }

    // Reset table state to uninitialized
    table->num_keys = 0;
    table->is_initialized = false;

    d_LogDebugF("Cleared static hash table, reset to uninitialized state (%zu buckets preserved).", 
                table->num_buckets);
    return 0;
}

/**
 * @brief Get bucket distribution statistics for the static hash table.
 *
 * This function analyzes the distribution of entries across buckets to help
 * evaluate hash function effectiveness and table performance. For static tables,
 * this is particularly useful for choosing optimal bucket count during initialization.
 *
 * @param table A pointer to the static hash table
 * @param min_entries Pointer to store minimum entries in any bucket
 * @param max_entries Pointer to store maximum entries in any bucket
 * @param avg_entries Pointer to store average entries per bucket
 * @param empty_buckets Pointer to store count of empty buckets
 *
 * @return 0 on success, 1 on failure
 *
 * @note All output parameters are optional (can be NULL)
 * @note Average is calculated as a floating-point value
 * @note Useful for performance analysis and optimization
 *
 * Example:
 * `size_t min, max, empty; float avg;`
 * `d_GetStaticTableStats(table, &min, &max, &avg, &empty);`
 */
int d_GetStaticTableStats(const dStaticTable_t* table, size_t* min_entries, 
                          size_t* max_entries, float* avg_entries, size_t* empty_buckets)
{
    if (!table) {
        d_LogError("Attempted to get stats from NULL static hash table.");
        return 1;
    }

    if (!table->is_initialized) {
        d_LogError("Attempted to get stats from uninitialized static table.");
        return 1;
    }

    size_t min_count = SIZE_MAX;
    size_t max_count = 0;
    size_t total_entries = 0;
    size_t empty_count = 0;

    // Analyze each bucket
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, i);
        size_t bucket_count = 0;

        if (bucket_ptr && *bucket_ptr) {
            // Count entries in this bucket
            dLinkedList_t* current = *bucket_ptr;
            while (current) {
                bucket_count++;
                current = current->next;
            }
        }

        if (bucket_count == 0) {
            empty_count++;
        }

        min_count = (bucket_count < min_count) ? bucket_count : min_count;
        max_count = (bucket_count > max_count) ? bucket_count : max_count;
        total_entries += bucket_count;
    }

    // Handle edge case of all empty buckets
    if (total_entries == 0) {
        min_count = 0;
    }

    // Set output parameters if provided
    if (min_entries) *min_entries = min_count;
    if (max_entries) *max_entries = max_count;
    if (avg_entries) *avg_entries = table->num_buckets > 0 ? (float)total_entries / (float)table->num_buckets : 0.0f;
    if (empty_buckets) *empty_buckets = empty_count;

    d_LogDebugF("Static table stats: min=%zu, max=%zu, avg=%.2f, empty=%zu/%zu buckets", 
                min_count, max_count, 
                table->num_buckets > 0 ? (float)total_entries / (float)table->num_buckets : 0.0f,
                empty_count, table->num_buckets);

    return 0;
}

/**
 * @brief Create a copy of the static hash table with a different bucket count.
 *
 * Creates a new static table with the same keys and values but redistributed
 * across a different number of buckets. This is useful for optimizing performance
 * when the ideal bucket count is determined after initial creation.
 *
 * @param source_table A pointer to the source static hash table
 * @param new_num_buckets The number of buckets for the new table
 *
 * @return A pointer to the newly created static table, or NULL on failure
 *
 * @note The source table remains unchanged
 * @note All keys and values are copied to the new table
 * @note The caller is responsible for destroying both tables
 * @note Useful for performance optimization without modifying original table
 *
 * Example:
 * `dStaticTable_t* optimized = d_RebucketStaticTable(original, 64);`
 * `// Use optimized table, destroy when done`
 * `d_DestroyStaticTable(&optimized);`
 */
dStaticTable_t* d_RebucketStaticTable(const dStaticTable_t* source_table, size_t new_num_buckets)
{
    if (!source_table) {
        d_LogError("Attempted to rebucket NULL static hash table.");
        return NULL;
    }

    if (!source_table->is_initialized) {
        d_LogError("Attempted to rebucket uninitialized static table.");
        return NULL;
    }

    if (new_num_buckets == 0) {
        d_LogError("Cannot rebucket to zero buckets.");
        return NULL;
    }

    // Collect all current keys and values
    dArray_t* all_keys = d_GetAllKeysFromStaticTable(source_table);
    if (!all_keys) {
        d_LogError("Failed to collect keys for rebucketing.");
        return NULL;
    }

    dArray_t* all_values = d_GetAllValuesFromStaticTable(source_table);
    if (!all_values) {
        d_LogError("Failed to collect values for rebucketing.");
        d_DestroyArray(all_keys);
        return NULL;
    }

    // Create arrays of pointers for the new table initialization
    const void** key_ptrs = malloc(source_table->num_keys * sizeof(void*));
    const void** value_ptrs = malloc(source_table->num_keys * sizeof(void*));
    
    if (!key_ptrs || !value_ptrs) {
        d_LogError("Failed to allocate pointer arrays for rebucketing.");
        free(key_ptrs);
        free(value_ptrs);
        d_DestroyArray(all_keys);
        d_DestroyArray(all_values);
        return NULL;
    }

    // Set up pointer arrays - get pointers to the data in the arrays
    for (size_t i = 0; i < source_table->num_keys; i++) {
        void* key_data = d_IndexDataFromArray(all_keys, i);
        void* value_data = d_IndexDataFromArray(all_values, i);
        
        if (!key_data || !value_data) {
            d_LogError("Failed to access array data during rebucketing.");
            free(key_ptrs);
            free(value_ptrs);
            d_DestroyArray(all_keys);
            d_DestroyArray(all_values);
            return NULL;
        }
        
        key_ptrs[i] = key_data;
        value_ptrs[i] = value_data;
    }

    // Create new table with same parameters but different bucket count
    dStaticTable_t* new_table = d_InitStaticTable(
        source_table->key_size,
        source_table->value_size,
        source_table->hash_func,
        source_table->compare_func,
        new_num_buckets,
        key_ptrs,
        value_ptrs,
        source_table->num_keys
    );

    // Cleanup temporary arrays
    free(key_ptrs);
    free(value_ptrs);
    d_DestroyArray(all_keys);
    d_DestroyArray(all_values);

    if (new_table) {
        d_LogInfoF("Successfully rebucketed static table from %zu to %zu buckets with %zu keys.",
                   source_table->num_buckets, new_num_buckets, source_table->num_keys);
    } else {
        d_LogError("Failed to create rebucketed static table.");
    }

    return new_table;
}

/**
 * @brief Create a deep copy of a static hash table.
 *
 * This function creates a new dStaticTable_t and copies all fixed key-value pairs
 * from the source table into the new table. All keys and values are
 * deep-copied, ensuring the new table is independent of the original.
 *
 * @param source_table A pointer to the dStaticTable_t to clone.
 *
 * @return A pointer to the newly created dStaticTable_t, or NULL on failure.
 *
 * @note The caller is responsible for destroying the new table with d_DestroyStaticTable.
 * @note The source table remains unchanged.
 *
 * Example:
 * `dStaticTable_t* new_table = d_CloneStaticTable(original_table);`
 * `if (new_table) {`
 * `    // ... use the new table`
 * `    d_DestroyStaticTable(&new_table);`
 * `}`
 */
dStaticTable_t* d_CloneStaticTable(const dStaticTable_t* source_table)
{
    if (!source_table) {
        d_LogError("Attempted to clone a NULL static hash table.");
        return NULL;
    }

    if (!source_table->is_initialized) {
        d_LogError("Attempted to clone an uninitialized static hash table.");
        return NULL;
    }

    // Use existing functions to extract all keys and values
    dArray_t* all_keys = d_GetAllKeysFromStaticTable(source_table);
    if (!all_keys) {
        d_LogError("Failed to collect keys for cloning.");
        return NULL;
    }

    dArray_t* all_values = d_GetAllValuesFromStaticTable(source_table);
    if (!all_values) {
        d_LogError("Failed to collect values for cloning.");
        d_DestroyArray(all_keys);
        return NULL;
    }

    // d_InitStaticTable requires an array of pointers to the data.
    // We create temporary arrays of pointers to the data within our collected arrays.
    const void** key_ptrs = (const void**)malloc(source_table->num_keys * sizeof(void*));
    const void** value_ptrs = (const void**)malloc(source_table->num_keys * sizeof(void*));

    if (!key_ptrs || !value_ptrs) {
        d_LogError("Failed to allocate pointer arrays for cloning operation.");
        free(key_ptrs); // It's safe to free NULL
        free(value_ptrs);
        d_DestroyArray(all_keys);
        d_DestroyArray(all_values);
        return NULL;
    }
    
    // Populate the pointer arrays
    for (size_t i = 0; i < source_table->num_keys; i++) {
        key_ptrs[i] = d_IndexDataFromArray(all_keys, i);
        value_ptrs[i] = d_IndexDataFromArray(all_values, i);
        if (!key_ptrs[i] || !value_ptrs[i]) {
             d_LogError("Failed to retrieve data from temporary arrays during cloning.");
             free(key_ptrs);
             free(value_ptrs);
             d_DestroyArray(all_keys);
             d_DestroyArray(all_values);
             return NULL;
        }
    }

    // Create the new table using the collected data and original parameters
    dStaticTable_t* new_table = d_InitStaticTable(
        source_table->key_size,
        source_table->value_size,
        source_table->hash_func,
        source_table->compare_func,
        source_table->num_buckets,
        key_ptrs,
        value_ptrs,
        source_table->num_keys
    );

    // Clean up all temporary allocations
    free(key_ptrs);
    free(value_ptrs);
    d_DestroyArray(all_keys);
    d_DestroyArray(all_values);

    if (new_table) {
        d_LogInfoF("Successfully cloned static table with %zu keys.", source_table->num_keys);
    } else {
        d_LogError("Failed to create new table during clone operation.");
    }
    
    return new_table;
}

// =============================================================================
// STATIC HASH TABLE FILE I/O FUNCTIONS
// =============================================================================

// Magic number for file format validation
#define D_STATIC_TABLE_MAGIC 0xDAEDDDCD
#define D_STATIC_TABLE_VERSION 1

/**
 * @brief Save a static hash table to a binary file.
 *
 * Writes the complete static table structure and all key-value pairs to a file
 * in a binary format that can be read back by d_LoadStaticTableFromFile().
 *
 * @param filename Path to the output file
 * @param table Pointer to the static table to save
 * @return 0 on success, 1 on failure
 *
 * @note The hash and compare functions cannot be saved and must be provided when loading
 */
int d_SaveStaticTableToFile(const char* filename, const dStaticTable_t* table)
{
    if (!filename || !table) {
        d_LogError("Invalid parameters for saving static table to file.");
        return 1;
    }

    if (!table->is_initialized) {
        d_LogError("Attempted to save uninitialized static table to file.");
        return 1;
    }

    FILE* file = fopen(filename, "wb");
    if (!file) {
        d_LogErrorF("Failed to open file '%s' for writing static table.", filename);
        return 1;
    }

    // Write file header
    uint32_t magic = D_STATIC_TABLE_MAGIC;
    uint32_t version = D_STATIC_TABLE_VERSION;
    
    if (fwrite(&magic, sizeof(uint32_t), 1, file) != 1) {
        d_LogError("Failed to write magic number to static table file.");
        fclose(file);
        return 1;
    }
    
    if (fwrite(&version, sizeof(uint32_t), 1, file) != 1) {
        d_LogError("Failed to write version to static table file.");
        fclose(file);
        return 1;
    }

    // Write table metadata
    if (fwrite(&table->key_size, sizeof(size_t), 1, file) != 1 ||
        fwrite(&table->value_size, sizeof(size_t), 1, file) != 1 ||
        fwrite(&table->num_buckets, sizeof(size_t), 1, file) != 1 ||
        fwrite(&table->num_keys, sizeof(size_t), 1, file) != 1) {
        d_LogError("Failed to write table metadata to static table file.");
        fclose(file);
        return 1;
    }

    // Write all key-value pairs
    size_t pairs_written = 0;
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, i);
        if (!bucket_ptr || !*bucket_ptr) {
            continue; // Empty bucket
        }

        dLinkedList_t* current = *bucket_ptr;
        while (current) {
            dTableEntry_t* entry = (dTableEntry_t*)current->data;
            if (entry && entry->key_data && entry->value_data) {
                // Write key data
                if (fwrite(entry->key_data, 1, table->key_size, file) != table->key_size) {
                    d_LogErrorF("Failed to write key data at pair %zu to static table file.", pairs_written);
                    fclose(file);
                    return 1;
                }
                
                // Write value data
                if (fwrite(entry->value_data, 1, table->value_size, file) != table->value_size) {
                    d_LogErrorF("Failed to write value data at pair %zu to static table file.", pairs_written);
                    fclose(file);
                    return 1;
                }
                
                pairs_written++;
            }
            current = current->next;
        }
    }

    fclose(file);

    if (pairs_written != table->num_keys) {
        d_LogErrorF("Warning: Expected to write %zu key-value pairs but wrote %zu.", table->num_keys, pairs_written);
        return 1;
    }

    d_LogInfoF("Successfully saved static table with %zu key-value pairs to file '%s'.", pairs_written, filename);
    return 0;
}

/**
 * @brief Load a static hash table from a binary file.
 *
 * Reads a file created by d_SaveStaticTableToFile() and reconstructs the static table.
 * The hash and compare functions must be provided as they cannot be stored in the file.
 *
 * @param filename Path to the input file
 * @param hash_func Hash function to use (must match the original table)
 * @param compare_func Compare function to use (must match the original table)
 * @return Pointer to the loaded static table, or NULL on failure
 *
 * @note The caller is responsible for destroying the returned table
 */
dStaticTable_t* d_LoadStaticTableFromFile(const char* filename, dTableHashFunc hash_func, dTableCompareFunc compare_func)
{
    if (!filename || !hash_func || !compare_func) {
        d_LogError("Invalid parameters for loading static table from file.");
        return NULL;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        d_LogErrorF("Failed to open file '%s' for reading static table.", filename);
        return NULL;
    }

    // Read and validate file header
    uint32_t magic, version;
    if (fread(&magic, sizeof(uint32_t), 1, file) != 1) {
        d_LogError("Failed to read magic number from static table file.");
        fclose(file);
        return NULL;
    }

    if (magic != D_STATIC_TABLE_MAGIC) {
        d_LogErrorF("Invalid magic number in static table file. Expected 0x%08X, got 0x%08X.", D_STATIC_TABLE_MAGIC, magic);
        fclose(file);
        return NULL;
    }

    if (fread(&version, sizeof(uint32_t), 1, file) != 1) {
        d_LogError("Failed to read version from static table file.");
        fclose(file);
        return NULL;
    }

    if (version != D_STATIC_TABLE_VERSION) {
        d_LogErrorF("Unsupported static table file version. Expected %u, got %u.", D_STATIC_TABLE_VERSION, version);
        fclose(file);
        return NULL;
    }

    // Read table metadata
    size_t key_size, value_size, num_buckets, num_keys;
    if (fread(&key_size, sizeof(size_t), 1, file) != 1 ||
        fread(&value_size, sizeof(size_t), 1, file) != 1 ||
        fread(&num_buckets, sizeof(size_t), 1, file) != 1 ||
        fread(&num_keys, sizeof(size_t), 1, file) != 1) {
        d_LogError("Failed to read table metadata from static table file.");
        fclose(file);
        return NULL;
    }

    // Validate metadata
    if (key_size == 0 || value_size == 0 || num_buckets == 0 || num_keys == 0) {
        d_LogError("Invalid table metadata in static table file.");
        fclose(file);
        return NULL;
    }

    // Allocate arrays for loaded keys and values
    void** loaded_keys = malloc(num_keys * sizeof(void*));
    void** loaded_values = malloc(num_keys * sizeof(void*));
    
    if (!loaded_keys || !loaded_values) {
        d_LogError("Failed to allocate memory for loading static table keys/values.");
        free(loaded_keys);
        free(loaded_values);
        fclose(file);
        return NULL;
    }

    // Initialize all pointers to NULL for cleanup safety
    for (size_t i = 0; i < num_keys; i++) {
        loaded_keys[i] = NULL;
        loaded_values[i] = NULL;
    }

    // Read all key-value pairs
    for (size_t i = 0; i < num_keys; i++) {
        // Allocate memory for this key and value
        loaded_keys[i] = malloc(key_size);
        loaded_values[i] = malloc(value_size);
        
        if (!loaded_keys[i] || !loaded_values[i]) {
            d_LogErrorF("Failed to allocate memory for key-value pair %zu during loading.", i);
            goto cleanup_and_fail;
        }

        // Read key data
        if (fread(loaded_keys[i], 1, key_size, file) != key_size) {
            d_LogErrorF("Failed to read key data for pair %zu from static table file.", i);
            goto cleanup_and_fail;
        }

        // Read value data
        if (fread(loaded_values[i], 1, value_size, file) != value_size) {
            d_LogErrorF("Failed to read value data for pair %zu from static table file.", i);
            goto cleanup_and_fail;
        }
    }

    fclose(file);

    // Create new static table using loaded data
    dStaticTable_t* new_table = d_InitStaticTable(
        key_size,
        value_size,
        hash_func,
        compare_func,
        num_buckets,
        (const void**)loaded_keys,
        (const void**)loaded_values,
        num_keys
    );

    // Clean up temporary arrays
    for (size_t i = 0; i < num_keys; i++) {
        if (loaded_keys[i]) free(loaded_keys[i]);
        if (loaded_values[i]) free(loaded_values[i]);
    }
    free(loaded_keys);
    free(loaded_values);

    if (new_table) {
        d_LogInfoF("Successfully loaded static table with %zu key-value pairs from file '%s'.", num_keys, filename);
    } else {
        d_LogError("Failed to create static table from loaded data.");
    }

    return new_table;

cleanup_and_fail:
    // Clean up allocated memory on failure
    for (size_t i = 0; i < num_keys; i++) {
        if (loaded_keys[i]) free(loaded_keys[i]);
        if (loaded_values[i]) free(loaded_values[i]);
    }
    free(loaded_keys);
    free(loaded_values);
    fclose(file);
    return NULL;
}

// =============================================================================
// STATIC HASH TABLE ITERATION FUNCTIONS
// =============================================================================

/**
 * @brief Iterate over all key-value pairs in a static hash table.
 *
 * Calls the provided callback function for each key-value pair in the table.
 * This provides a memory-efficient way to process all entries without copying them.
 *
 * @param table Pointer to the static table to iterate over
 * @param callback Function to call for each key-value pair
 * @param user_data Generic pointer passed to the callback for context
 * @return 0 on success, 1 on failure
 */
int d_IterateStaticTable(const dStaticTable_t* table, dTableIteratorFunc callback, void* user_data)
{
    if (!table || !callback) {
        d_LogError("Invalid parameters for iterating static table.");
        return 1;
    }

    if (!table->is_initialized) {
        d_LogError("Attempted to iterate over uninitialized static table.");
        return 1;
    }

    size_t pairs_processed = 0;

    // Iterate through all buckets
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, i);
        if (!bucket_ptr || !*bucket_ptr) {
            continue; // Empty bucket
        }

        // Iterate through all entries in this bucket
        dLinkedList_t* current_node = *bucket_ptr;
        while (current_node != NULL) {
            dTableEntry_t* entry = (dTableEntry_t*)current_node->data;
            if (entry && entry->key_data && entry->value_data) {
                // Call the callback function with the entry data
                callback(entry->key_data, table->key_size, entry->value_data, table->value_size, user_data);
                pairs_processed++;
            }
            current_node = current_node->next;
        }
    }

    d_LogDebugF("Iterated over %zu key-value pairs in static table (expected: %zu).", pairs_processed, table->num_keys);
    
    if (pairs_processed != table->num_keys) {
        d_LogWarningF("Processed %zu pairs but expected %zu in static table.", pairs_processed, table->num_keys);
    }

    return 0;
}