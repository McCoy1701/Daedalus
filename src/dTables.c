// File: src/dTables.c - Generic Hash Table Implementation for Daedalus Library
// Uses dLinkedList_t for collision resolution via chaining

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Daedalus.h"

// =============================================================================
// INTERNAL HELPER FUNCTIONS
// =============================================================================

/**
 * @brief Internal helper: Create a new hash table entry.
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
static dTableEntry_t* _d_CreateTableEntry(const void* key, size_t key_size, 
                                           const void* value, size_t value_size)
{
    if (!key || !value || key_size == 0 || value_size == 0) {
        d_LogError("Invalid parameters for creating table entry.");
        return NULL;
    }

    dTableEntry_t* entry = (dTableEntry_t*)malloc(sizeof(dTableEntry_t));
    if (!entry) {
        d_LogError("Failed to allocate memory for table entry structure.");
        return NULL;
    }

    // Allocate and copy key data
    entry->key_data = malloc(key_size);
    if (!entry->key_data) {
        d_LogError("Failed to allocate memory for table entry key data.");
        free(entry);
        return NULL;
    }
    memcpy(entry->key_data, key, key_size);

    // Allocate and copy value data
    entry->value_data = malloc(value_size);
    if (!entry->value_data) {
        d_LogError("Failed to allocate memory for table entry value data.");
        free(entry->key_data);
        free(entry);
        return NULL;
    }
    memcpy(entry->value_data, value, value_size);

    return entry;
}

/**
 * @brief Internal helper: Destroy a hash table entry.
 *
 * Frees all memory associated with a dTableEntry_t including the key data,
 * value data, and the entry structure itself.
 *
 * @param entry Pointer to the entry to destroy
 */
static void _d_DestroyTableEntry(dTableEntry_t* entry)
{
    if (!entry) return;

    if (entry->key_data) free(entry->key_data);
    if (entry->value_data) free(entry->value_data);
    free(entry);
}

/**
 * @brief Internal helper: Find an entry in a bucket by key.
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
static dTableEntry_t* _d_FindEntryInBucket(dLinkedList_t* bucket_head, const void* key, 
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
static void _d_GenerateEntryName(const dTableEntry_t* entry, char* name_buffer, size_t buffer_size)
{
    snprintf(name_buffer, buffer_size, "entry_%p", (void*)entry);
}

// =============================================================================
// HASH TABLE CREATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Initialize a new hash table structure.
 *
 * Allocates memory for the dTable_t structure, initializes its internal fields,
 * and allocates the initial array of dLinkedList_t pointers (buckets). Each
 * bucket is initially set to NULL.
 *
 * @param key_size The size in bytes of the keys that will be stored
 * @param value_size The size in bytes of the values that will be stored
 * @param hash_func A pointer to the user-provided hashing function
 * @param compare_func A pointer to the user-provided key comparison function
 * @param initial_num_buckets The initial number of buckets for the table
 * @param load_factor_threshold The threshold at which the table will rehash
 *
 * @return A pointer to the newly initialized dTable_t instance, or NULL on failure
 *
 * Example:
 * `dTable_t* table = d_InitTable(sizeof(int), sizeof(char*), my_hash, my_compare, 16, 0.75f);`
 */
dTable_t* d_InitTable(size_t key_size, size_t value_size, dTableHashFunc hash_func,
                      dTableCompareFunc compare_func, size_t initial_num_buckets,
                      float load_factor_threshold)
{
    if (key_size == 0 || value_size == 0 || !hash_func || !compare_func || 
        initial_num_buckets == 0 || load_factor_threshold <= 0.0f) {
        d_LogError("Invalid parameters for hash table initialization.");
        return NULL;
    }

    dTable_t* table = (dTable_t*)malloc(sizeof(dTable_t));
    if (!table) {
        d_LogError("Failed to allocate memory for hash table structure.");
        return NULL;
    }

    // Allocate buckets array using dArray_t as per header definition
    table->buckets = d_InitArray(initial_num_buckets, sizeof(dLinkedList_t*));
    if (!table->buckets) {
        d_LogError("Failed to allocate memory for hash table buckets array.");
        free(table);
        return NULL;
    }

    // Initialize all bucket pointers to NULL
    for (size_t i = 0; i < initial_num_buckets; i++) {
        dLinkedList_t* null_ptr = NULL;
        d_AppendDataToArray(table->buckets, &null_ptr);
    }

    // Initialize table fields
    table->num_buckets = initial_num_buckets;
    table->count = 0;
    table->key_size = key_size;
    table->value_size = value_size;
    table->hash_func = hash_func;
    table->compare_func = compare_func;
    table->load_factor_threshold = load_factor_threshold;

    d_LogDebugF("Initialized hash table with %zu buckets, load factor threshold: %.2f",
                initial_num_buckets, load_factor_threshold);

    return table;
}

/**
 * @brief Destroy a hash table and free all associated memory.
 *
 * Deallocates all memory associated with a dTable_t instance. This includes
 * iterating through all buckets, destroying each dLinkedList_t (which frees
 * the dTableEntry_ts and their internal key and value data), then freeing
 * the buckets array, and finally the dTable_t structure itself.
 *
 * @param table A pointer to the pointer of the hash table to destroy
 *
 * @return 0 on success, 1 on failure
 *
 * Example:
 * `d_DestroyTable(&my_table); // my_table will be NULL after this`
 */
int d_DestroyTable(dTable_t** table)
{
    if (!table || !*table) {
        d_LogError("Attempted to destroy NULL or invalid hash table.");
        return 1;
    }

    dTable_t* t = *table;

    // Destroy all buckets and their entries
    for (size_t i = 0; i < t->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(t->buckets, i);
        if (bucket_ptr && *bucket_ptr) {
            // Manually destroy entries first, then the linked list structure
            dLinkedList_t* current = *bucket_ptr;
            while (current) {
                dTableEntry_t* entry = (dTableEntry_t*)current->data;
                if (entry) {
                    _d_DestroyTableEntry(entry);
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

    d_LogDebug("Hash table destroyed successfully.");
    return 0;
}

// =============================================================================
// HASH TABLE DATA MANAGEMENT
// =============================================================================

/**
 * @brief Insert or update a key-value pair in the hash table (upsert operation).
 *
 * Inserts a new key-value pair into the hash table, or updates the value if the
 * key already exists. Computes the hash, identifies the correct bucket, and either
 * updates an existing dTableEntry_t or creates a new one within that bucket's
 * linked list. Manages the table's count and triggers rehashing if needed.
 *
 * @param table A pointer to the hash table
 * @param key A pointer to the key data to set
 * @param value A pointer to the value data to set
 *
 * @return 0 on success, 1 on failure
 *
 * Example:
 * `int key = 42; char* value = "hello"; d_SetDataToTable(table, &key, &value);`
 */
int d_SetDataToTable(dTable_t* table, const void* key, const void* value)
{
    if (!table || !key || !value) {
        d_LogError("Invalid parameters for setting data to hash table.");
        return 1;
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, bucket_index);
    if (!bucket_ptr) {
        d_LogError("Failed to access bucket in hash table.");
        return 1;
    }

    // Check if key already exists in this bucket
    dTableEntry_t* existing_entry = _d_FindEntryInBucket(*bucket_ptr, key, table->key_size, table->compare_func);

    if (existing_entry) {
        // Update existing entry's value
        d_LogDebugF("Updating existing key in hash table (bucket %zu).", bucket_index);
        
        // Free old value data
        if (existing_entry->value_data) {
            free(existing_entry->value_data);
        }
        
        // Allocate and copy new value data
        existing_entry->value_data = malloc(table->value_size);
        if (!existing_entry->value_data) {
            d_LogError("Failed to allocate memory for updated value data.");
            return 1;
        }
        memcpy(existing_entry->value_data, value, table->value_size);
        
        return 0; // Success - updated existing entry
    }

    // Create new entry
    dTableEntry_t* new_entry = _d_CreateTableEntry(key, table->key_size, value, table->value_size);
    if (!new_entry) {
        d_LogError("Failed to create new table entry.");
        return 1;
    }

    // Generate unique name for linked list storage
    char entry_name[64];
    _d_GenerateEntryName(new_entry, entry_name, sizeof(entry_name));

    // Add to bucket's linked list
    int result = d_PushBackToLinkedList(bucket_ptr, new_entry, 
                                       entry_name, sizeof(dTableEntry_t));
    if (result != 0) {
        d_LogErrorF("Failed to add entry to bucket %zu linked list.", bucket_index);
        _d_DestroyTableEntry(new_entry);
        return 1;
    }

    // Increment count
    table->count++;
    
    d_LogDebugF("Added new key-value pair to hash table (bucket %zu, total count: %zu).",
                bucket_index, table->count);

    // Check if rehashing is needed
    float current_load_factor = (float)table->count / (float)table->num_buckets;
    if (current_load_factor > table->load_factor_threshold) {
        d_LogInfoF("Load factor (%.2f) exceeds threshold (%.2f). Rehashing recommended.",
                   current_load_factor, table->load_factor_threshold);
        // TODO: Implement rehashing in future version
    }

    return 0;
}

/**
 * @brief Retrieve a pointer to the value associated with a given key.
 *
 * Retrieves a pointer to the value associated with a given key from the hash table.
 * Computes the hash, finds the relevant bucket, and searches the linked list in that
 * bucket using the compare function to locate the entry with the matching key.
 *
 * @param table A pointer to the hash table
 * @param key A pointer to the key data to search for
 *
 * @return A void* pointer to the internally stored value data if found, or NULL if not found
 *
 * Example:
 * `int key = 42; char** value = (char**)d_GetDataFromTable(table, &key);`
 */
void* d_GetDataFromTable(dTable_t* table, const void* key)
{
    if (!table || !key) {
        d_LogError("Invalid parameters for getting data from hash table.");
        return NULL;
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, bucket_index);
    if (!bucket_ptr) {
        d_LogError("Failed to access bucket in hash table.");
        return NULL;
    }

    // Find entry in bucket
    dTableEntry_t* entry = _d_FindEntryInBucket(*bucket_ptr, key, table->key_size, table->compare_func);

    if (entry) {
        d_LogDebugF("Found key in hash table (bucket %zu).", bucket_index);
        return entry->value_data;
    }

    d_LogDebugF("Key not found in hash table (bucket %zu).", bucket_index);
    return NULL;
}

/**
 * @brief Remove a key-value pair from the hash table.
 *
 * Removes a key-value pair from the hash table based on the provided key.
 * Computes the hash, locates the correct bucket, and removes the entry with
 * the matching key from the linked list. Frees the key data, value data,
 * and the entry itself. Manages the table's count.
 *
 * @param table A pointer to the hash table
 * @param key A pointer to the key data to remove
 *
 * @return 0 on success, 1 on failure/key not found
 *
 * Example:
 * `int key = 42; d_RemoveDataFromTable(table, &key);`
 */
int d_RemoveDataFromTable(dTable_t* table, const void* key)
{
    if (!table || !key) {
        d_LogError("Invalid parameters for removing data from hash table.");
        return 1;
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, bucket_index);
    if (!bucket_ptr) {
        d_LogError("Failed to access bucket in hash table.");
        return 1;
    }

    dLinkedList_t* bucket = *bucket_ptr;
    if (!bucket) {
        d_LogDebugF("Key not found - bucket %zu is empty.", bucket_index);
        return 1; // Key not found
    }

    // Find and remove the entry
    dLinkedList_t* current = bucket;
    dLinkedList_t* previous = NULL;

    while (current) {
        dTableEntry_t* entry = (dTableEntry_t*)current->data;
        if (entry && table->compare_func(entry->key_data, key, table->key_size) == 0) {
            
            // Found the entry to remove
            d_LogDebugF("Removing key from hash table (bucket %zu).", bucket_index);
            
            // Remove from linked list
            if (previous) {
                previous->next = current->next;
            } else {
                // Removing the head
                *bucket_ptr = current->next;
            }
            
            // Free the entry and node
            _d_DestroyTableEntry(entry);
            free(current);
            
            // Decrement count
            table->count--;
            
            d_LogDebugF("Removed key from hash table (total count: %zu).", table->count);
            return 0; // Success
        }
        
        previous = current;
        current = current->next;
    }

    d_LogDebugF("Key not found in hash table (bucket %zu).", bucket_index);
    return 1; // Key not found
}

// =============================================================================
// HASH TABLE UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Check if a specific key exists in the hash table.
 *
 * Determines if a specific key exists within the hash table. Calculates the
 * bucket and uses the compare function to check for the key's presence in
 * the corresponding linked list.
 *
 * @param table A pointer to the hash table
 * @param key A pointer to the key data to check for
 *
 * @return 0 if the key exists, 1 if not found or error occurred
 *
 * Example:
 * `int key = 42; if (d_CheckForKeyInTable(table, &key) == 0) { // key exists }`
 */
int d_CheckForKeyInTable(const dTable_t* table, const void* key)
{
    if (!table || !key) {
        d_LogError("Invalid parameters for checking key existence in hash table.");
        return 1; // Not found / error
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, bucket_index);
    if (!bucket_ptr) {
        d_LogError("Failed to access bucket in hash table.");
        return 1;
    }

    // Find entry in bucket
    dTableEntry_t* entry = _d_FindEntryInBucket(*bucket_ptr, key, table->key_size, table->compare_func);

    if (entry) {
        d_LogDebugF("Key found in hash table (bucket %zu).", bucket_index);
        return 0; // Found
    }

    d_LogDebugF("Key not found in hash table (bucket %zu).", bucket_index);
    return 1; // Not found
}

/**
 * @brief Get the current number of key-value pairs in the hash table.
 *
 * Returns the current number of key-value pairs (entries) stored in the hash table.
 *
 * @param table A pointer to the hash table
 *
 * @return The total count of entries, or 0 if table is NULL
 *
 * Example:
 * `size_t count = d_GetCountInTable(table);`
 */
size_t d_GetCountInTable(const dTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to get count from NULL hash table.");
        return 0;
    }

    return table->count;
}

/**
 * @brief Clear all entries from the hash table but keep the structure intact.
 *
 * Empties the hash table, removing all key-value pairs and freeing their
 * associated memory, but keeps the underlying dTable_t structure (including
 * its buckets array and allocated num_buckets) intact for reuse.
 *
 * @param table A pointer to the hash table to clear
 *
 * @return 0 on success, 1 on failure
 *
 * Example:
 * `d_ClearTable(table); // Table is now empty but ready for reuse`
 */
int d_ClearTable(dTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to clear NULL hash table.");
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
                    _d_DestroyTableEntry(entry);
                    current->data = NULL; // Prevent double free by linked list
                }
                current = current->next;
            }
            // Destroy the linked list
            d_DestroyLinkedList(bucket_ptr);
        }
    }

    // Reset count
    table->count = 0;

    d_LogDebugF("Cleared hash table, reset count to 0 (%zu buckets preserved).", table->num_buckets);
    return 0;
}

// =============================================================================
// ADVANCED HASH TABLE UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Rehash the table with a new number of buckets to optimize performance.
 *
 * This function creates a new bucket array with the specified size, recalculates
 * the hash for all existing entries, and redistributes them across the new buckets.
 * This is essential for maintaining O(1) performance as the table grows.
 *
 * @param table A pointer to the hash table to rehash
 * @param new_num_buckets The new number of buckets (0 for auto-sizing to double current)
 *
 * @return 0 on success, 1 on failure
 *
 * @note If new_num_buckets is 0, the function will automatically double the current bucket count
 * @note The function will fail if new_num_buckets is not greater than the current bucket count
 * @note All existing entries are preserved and redistributed based on their recalculated hashes
 *
 * Example:
 * `d_RehashTable(table, 32); // Rehash to 32 buckets`
 * `d_RehashTable(table, 0);  // Auto-resize to double current bucket count`
 */
int d_RehashTable(dTable_t* table, size_t new_num_buckets)
{
    if (!table) {
        d_LogError("Attempted to rehash NULL hash table.");
        return 1;
    }

    // Auto-size if new_num_buckets is 0
    size_t actual_new_num_buckets = new_num_buckets;
    if (actual_new_num_buckets == 0) {
        actual_new_num_buckets = table->num_buckets == 0 ? 16 : table->num_buckets * 2;
        d_LogDebugF("Auto-sizing rehash from %zu to %zu buckets.", table->num_buckets, actual_new_num_buckets);
    }

    // Validate that we're actually growing
    if (actual_new_num_buckets <= table->num_buckets) {
        d_LogErrorF("Cannot rehash to %zu buckets (current: %zu). New size must be larger.", 
                    actual_new_num_buckets, table->num_buckets);
        return 1;
    }

    // Allocate new buckets array
    dArray_t* new_buckets_array = d_InitArray(actual_new_num_buckets, sizeof(dLinkedList_t*));
    if (!new_buckets_array) {
        d_LogError("Failed to allocate new buckets array for rehashing.");
        return 1;
    }

    // Initialize all new bucket pointers to NULL
    for (size_t i = 0; i < actual_new_num_buckets; i++) {
        dLinkedList_t* null_ptr = NULL;
        d_AppendDataToArray(new_buckets_array, &null_ptr);
    }

    // Preserve old table state
    dArray_t* old_buckets_array = table->buckets;
    size_t old_num_buckets = table->num_buckets;
    size_t old_count = table->count;

    // Update table with new buckets (reset count as we'll rebuild it)
    table->buckets = new_buckets_array;
    table->num_buckets = actual_new_num_buckets;
    table->count = 0;

    // Re-insert all existing entries into new buckets
    for (size_t i = 0; i < old_num_buckets; i++) {
        dLinkedList_t** old_bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(old_buckets_array, i);
        if (!old_bucket_ptr || !*old_bucket_ptr) {
            continue; // Empty bucket
        }

        dLinkedList_t* current_node = *old_bucket_ptr;
        while (current_node != NULL) {
            dLinkedList_t* next_node = current_node->next; // Store next before modification

            dTableEntry_t* entry = (dTableEntry_t*)current_node->data;
            if (entry) {
                // Calculate new hash and bucket index
                size_t new_hash = table->hash_func(entry->key_data, table->key_size);
                size_t new_bucket_index = new_hash % table->num_buckets;

                // Get pointer to new bucket's head
                dLinkedList_t** new_bucket_head_ptr = (dLinkedList_t**)d_IndexDataFromArray(table->buckets, new_bucket_index);
                if (new_bucket_head_ptr) {
                    // Generate unique name for linked list storage
                    char temp_name[64];
                    _d_GenerateEntryName(entry, temp_name, sizeof(temp_name));

                    // Re-add entry to new bucket
                    if (d_PushBackToLinkedList(new_bucket_head_ptr, entry, temp_name, sizeof(dTableEntry_t)) == 0) {
                        table->count++;
                    } else {
                        d_LogErrorF("Failed to re-insert entry during rehashing at bucket %zu.", new_bucket_index);
                    }
                }
            }

            current_node = next_node;
        }
    }

    // Cleanup old buckets - free linked list nodes without freeing entry data
    for (size_t i = 0; i < old_num_buckets; i++) {
        dLinkedList_t** old_bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(old_buckets_array, i);
        if (old_bucket_ptr && *old_bucket_ptr) {
            dLinkedList_t* current_old_node = *old_bucket_ptr;
            while (current_old_node != NULL) {
                dLinkedList_t* temp_free = current_old_node;
                current_old_node = current_old_node->next;
                
                // CRITICAL: Prevent double-free by nulling data before freeing node
                temp_free->data = NULL;
                free(temp_free);
            }
        }
    }

    // Destroy old buckets array
    d_DestroyArray(old_buckets_array);

    d_LogInfoF("Rehashed table from %zu to %zu buckets. Entries: %zu (expected: %zu).",
               old_num_buckets, actual_new_num_buckets, table->count, old_count);

    return 0;
}

/**
 * @brief Get an array containing copies of all keys currently stored in the hash table.
 *
 * This function iterates through all buckets and collects all keys, returning them
 * in a newly allocated dArray_t. The keys are copied, so the returned array can
 * be modified independently of the hash table.
 *
 * @param table A pointer to the hash table
 *
 * @return A newly allocated dArray_t containing all keys, or NULL on failure
 *
 * @note The caller is responsible for destroying the returned array with d_DestroyArray
 * @note The keys are copied into the array, not referenced
 * @note If the table is empty, returns an empty array (not NULL)
 *
 * Example:
 * `dArray_t* keys = d_GetAllKeysFromTable(table);`
 * `// Use keys array...`
 * `d_DestroyArray(keys);`
 */
dArray_t* d_GetAllKeysFromTable(const dTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to get keys from NULL hash table.");
        return NULL;
    }

    // Initialize result array with estimated capacity
    size_t initial_capacity = table->count > 0 ? table->count : 16;
    dArray_t* all_keys_array = d_InitArray(initial_capacity, table->key_size);
    if (!all_keys_array) {
        d_LogError("Failed to allocate array for collecting hash table keys.");
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

    d_LogDebugF("Collected %zu keys from hash table (expected: %zu).", keys_collected, table->count);
    return all_keys_array;
}

/**
 * @brief Get an array containing copies of all values currently stored in the hash table.
 *
 * This function iterates through all buckets and collects all values, returning them
 * in a newly allocated dArray_t. The values are copied, so the returned array can
 * be modified independently of the hash table.
 *
 * @param table A pointer to the hash table
 *
 * @return A newly allocated dArray_t containing all values, or NULL on failure
 *
 * @note The caller is responsible for destroying the returned array with d_DestroyArray
 * @note The values are copied into the array, not referenced
 * @note If the table is empty, returns an empty array (not NULL)
 *
 * Example:
 * `dArray_t* values = d_GetAllValuesFromTable(table);`
 * `// Use values array...`
 * `d_DestroyArray(values);`
 */
dArray_t* d_GetAllValuesFromTable(const dTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to get values from NULL hash table.");
        return NULL;
    }

    // Initialize result array with estimated capacity
    size_t initial_capacity = table->count > 0 ? table->count : 16;
    dArray_t* all_values_array = d_InitArray(initial_capacity, table->value_size);
    if (!all_values_array) {
        d_LogError("Failed to allocate array for collecting hash table values.");
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

    d_LogDebugF("Collected %zu values from hash table (expected: %zu).", values_collected, table->count);
    return all_values_array;
}

/**
 * @brief Create a deep copy of a dynamic hash table.
 *
 * This function creates a new dTable_t and copies all key-value pairs
 * from the source table into the new table. All keys and values are
 * deep-copied, ensuring the new table is independent of the original.
 *
 * @param source_table A pointer to the dTable_t to clone.
 *
 * @return A pointer to the newly created dTable_t, or NULL on failure.
 *
 * @note The caller is responsible for destroying the returned table with d_DestroyTable().
 * @note The new table will have the same configuration (key_size, value_size,
 * hash_func, compare_func, initial num_buckets, load_factor_threshold)
 * as the source table.
 *
 * Example:
 * `dTable_t* cloned_table = d_CloneTable(original_table);`
 * `// Use cloned_table...`
 * `d_DestroyTable(&cloned_table);`
 */
dTable_t* d_CloneTable(const dTable_t* source_table)
{
    if (!source_table) {
        d_LogError("Attempted to clone a NULL source hash table.");
        return NULL;
    }

    // 1. Initialize a new dTable_t with the same configuration
    // We use the source table's current number of buckets and load factor threshold
    // for the initial allocation, as this is typically what you want for a copy.
    dTable_t* new_table = d_InitTable(source_table->key_size,
                                      source_table->value_size,
                                      source_table->hash_func,
                                      source_table->compare_func,
                                      source_table->num_buckets, // Use current buckets, not necessarily initial
                                      source_table->load_factor_threshold);
    if (!new_table) {
        d_LogError("Failed to initialize new table for cloning.");
        return NULL;
    }

    // 2. Iterate through all entries in the source table and insert into new table
    size_t entries_cloned = 0;
    for (size_t i = 0; i < source_table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_IndexDataFromArray(source_table->buckets, i);
        if (!bucket_ptr || !*bucket_ptr) {
            continue; // Empty bucket
        }

        dLinkedList_t* current_node = *bucket_ptr;
        while (current_node != NULL) {
            dTableEntry_t* entry = (dTableEntry_t*)current_node->data;
            if (entry && entry->key_data && entry->value_data) {
                // Insert key-value pair into the new table
                if (d_SetDataToTable(new_table, entry->key_data, entry->value_data) != 0) {
                    d_LogErrorF("Failed to clone entry during table cloning (key_size: %zu, val_size: %zu).",
                                entry->key_data, entry->value_data);
                    // Clean up partially cloned table
                    d_DestroyTable(&new_table);
                    return NULL;
                }
                entries_cloned++;
            }
            current_node = current_node->next;
        }
    }

    d_LogInfoF("Successfully cloned dynamic table: %zu entries copied to new table with %zu buckets.",
               entries_cloned, new_table->num_buckets);
    
    // Ensure the count of the new table matches the source table,
    // in case d_SetDataToTable's internal rehash logic changed behavior.
    // However, since we're just copying, `new_table->count` should already be `source_table->count`.
    if (new_table->count != source_table->count) {
        d_LogWarningF("Cloned table count (%zu) does not match source table count (%zu). This indicates an issue with cloning logic.",
                      new_table->count, source_table->count);
        // This is a warning, not a fatal error for a clone unless we want exact behavior.
        // It's more of a self-check.
    }

    return new_table;
}