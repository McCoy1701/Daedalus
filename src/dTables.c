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

dTable_t* d_TableInit(size_t key_size, size_t value_size, dTableHashFunc hash_func,
                      dTableCompareFunc compare_func, size_t initial_num_buckets
                    )
{
    if (key_size == 0 || value_size == 0 || !hash_func || !compare_func || 
        initial_num_buckets == 0) {
        d_LogError("Invalid parameters for hash table initialization.");
        return NULL;
    }

    dTable_t* table = (dTable_t*)malloc(sizeof(dTable_t));
    if (!table) {
        d_LogError("Failed to allocate memory for hash table structure.");
        return NULL;
    }

    // Allocate buckets array using dArray_t as per header definition
    table->buckets = d_ArrayInit(initial_num_buckets, sizeof(dLinkedList_t*));
    if (!table->buckets) {
        d_LogError("Failed to allocate memory for hash table buckets array.");
        free(table);
        return NULL;
    }

    // Initialize all bucket pointers to NULL
    for (size_t i = 0; i < initial_num_buckets; i++) {
        dLinkedList_t* null_ptr = NULL;
        d_ArrayAppend(table->buckets, &null_ptr);
    }

    // Initialize table fields
    table->num_buckets = initial_num_buckets;
    table->count = 0;
    table->key_size = key_size;
    table->value_size = value_size;
    table->hash_func = hash_func;
    table->compare_func = compare_func;
    table->load_factor_threshold = 0.75f;

    d_LogDebugF("Initialized hash table with %zu buckets, load factor threshold: %.2f",
                initial_num_buckets, 0.75f);

    return table;
}

int _d_TableDestroy_impl(dTable_t** table, const char* file, int line, const char* func)
{
    D_ASSERT(table != NULL, "d_TableDestroy: NULL pointer to table pointer", file, line, func);
    D_ASSERT(*table != NULL, "d_TableDestroy: table already NULL (double-free?)", file, line, func);

    dTable_t* t = *table;
    
    // Sanity check - catch obvious corruption
    D_ASSERT(t->buckets != NULL, "d_TableDestroy: buckets is NULL (corruption?)", file, line, func);

    // Destroy all buckets and their entries
    for (size_t i = 0; i < t->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(t->buckets, i);
        if (bucket_ptr && *bucket_ptr) {
            dLinkedList_t* current = *bucket_ptr;
            while (current) {
                dTableEntry_t* entry = (dTableEntry_t*)current->data;
                if (entry) {
                    _d_DestroyTableEntry(entry);
                    current->data = NULL;
                }
                current = current->next;
            }
            d_DestroyLinkedList(bucket_ptr);
        }
    }

    d_ArrayDestroy(t->buckets);
    free(t);
    *table = NULL;

    d_LogDebug("Hash table destroyed successfully.");
    return 0;
}

// =============================================================================
// HASH TABLE DATA MANAGEMENT
// =============================================================================

int d_TableSet(dTable_t* table, const void* key, const void* value)
{
    if (!table || !key || !value) {
        d_LogError("Invalid parameters for setting data to hash table.");
        return 1;
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(table->buckets, bucket_index);
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

    d_LogDebugF("Load Factor for the Table is %.2f", table->load_factor_threshold);
    // Check if rehashing is needed
    float current_load_factor = (float)table->count / (float)table->num_buckets;
    if (current_load_factor > table->load_factor_threshold) {
        d_LogInfoF("Load factor (%.2f) exceeds threshold (%.2f). Triggering auto-rehash.",
                   current_load_factor, table->load_factor_threshold);
        
        // Auto-resize by doubling the number of buckets.
        // d_TableRehash with 0 for the new size handles this automatically.
        if (d_TableRehash(table, 0) != 0) {
            d_LogError("Rehashing failed. Table performance may be degraded.");
            // The insertion was successful, but the table maintenance failed.
            // Return 1 to signal that a non-fatal error occurred.
            return 1;
        }
    }

    return 0;
}

void* d_TableGet(dTable_t* table, const void* key)
{
    if (!table || !key) {
        d_LogError("Invalid parameters for getting data from hash table.");
        return NULL;
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(table->buckets, bucket_index);
    if (!bucket_ptr) {
        d_LogError("Failed to access bucket in hash table.");
        return NULL;
    }

    // Find entry in bucket
    dTableEntry_t* entry = _d_FindEntryInBucket(*bucket_ptr, key, table->key_size, table->compare_func);

    if (entry) {
        // d_LogDebugF("Found key in hash table (bucket %zu).", bucket_index);
        return entry->value_data;
    }

    d_LogDebugF("Key not found in hash table (bucket %zu).", bucket_index);
    return NULL;
}

int _d_TableRemove_impl(dTable_t* table, const void* key, const char* file, int line, const char* func) {
    
    D_ASSERT(table != NULL, "d_TableRemove: NULL table", file, line, func);
    D_ASSERT(key != NULL, "d_TableRemove: NULL key", file, line, func);

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(table->buckets, bucket_index);
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

int d_TableHasKey(const dTable_t* table, const void* key)
{
    if (!table || !key) {
        d_LogError("Invalid parameters for checking key existence in hash table.");
        return 1; // Not found / error
    }

    // Compute hash and bucket index
    size_t hash = table->hash_func(key, table->key_size);
    size_t bucket_index = hash % table->num_buckets;

    // Get bucket pointer
    dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(table->buckets, bucket_index);
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

size_t d_TableGetCount(const dTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to get count from NULL hash table.");
        return 0;
    }

    return table->count;
}

int d_TableClear(dTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to clear NULL hash table.");
        return 1;
    }

    // Clear all buckets
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(table->buckets, i);
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

int d_TableRehash(dTable_t* table, size_t new_num_buckets)
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
    dArray_t* new_buckets_array = d_ArrayInit(actual_new_num_buckets, sizeof(dLinkedList_t*));
    if (!new_buckets_array) {
        d_LogError("Failed to allocate new buckets array for rehashing.");
        return 1;
    }

    // Initialize all new bucket pointers to NULL
    for (size_t i = 0; i < actual_new_num_buckets; i++) {
        dLinkedList_t* null_ptr = NULL;
        d_ArrayAppend(new_buckets_array, &null_ptr);
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
        dLinkedList_t** old_bucket_ptr = (dLinkedList_t**)d_ArrayGet(old_buckets_array, i);
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
                dLinkedList_t** new_bucket_head_ptr = (dLinkedList_t**)d_ArrayGet(table->buckets, new_bucket_index);
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
        dLinkedList_t** old_bucket_ptr = (dLinkedList_t**)d_ArrayGet(old_buckets_array, i);
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
    d_ArrayDestroy(old_buckets_array);

    d_LogInfoF("Rehashed table from %zu to %zu buckets. Entries: %zu (expected: %zu).",
               old_num_buckets, actual_new_num_buckets, table->count, old_count);

    return 0;
}

dArray_t* d_TableGetAllKeys(const dTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to get keys from NULL hash table.");
        return NULL;
    }

    // Initialize result array with estimated capacity
    size_t initial_capacity = table->count > 0 ? table->count : 16;
    dArray_t* all_keys_array = d_ArrayInit(initial_capacity, table->key_size);
    if (!all_keys_array) {
        d_LogError("Failed to allocate array for collecting hash table keys.");
        return NULL;
    }

    size_t keys_collected = 0;

    // Iterate through all buckets
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(table->buckets, i);
        if (!bucket_ptr || !*bucket_ptr) {
            continue; // Empty bucket
        }

        dLinkedList_t* current_node = *bucket_ptr;
        while (current_node != NULL) {
            dTableEntry_t* entry = (dTableEntry_t*)current_node->data;
            if (entry && entry->key_data) {
                // Append key data to result array
                if (d_ArrayAppend(all_keys_array, entry->key_data) != 0) {
                    d_LogErrorF("Failed to append key to result array at bucket %zu.", i);
                    d_ArrayDestroy(all_keys_array);
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

dArray_t* d_TableGetAllValues(const dTable_t* table)
{
    if (!table) {
        d_LogError("Attempted to get values from NULL hash table.");
        return NULL;
    }

    // Initialize result array with estimated capacity
    size_t initial_capacity = table->count > 0 ? table->count : 16;
    dArray_t* all_values_array = d_ArrayInit(initial_capacity, table->value_size);
    if (!all_values_array) {
        d_LogError("Failed to allocate array for collecting hash table values.");
        return NULL;
    }

    size_t values_collected = 0;

    // Iterate through all buckets
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(table->buckets, i);
        if (!bucket_ptr || !*bucket_ptr) {
            continue; // Empty bucket
        }

        dLinkedList_t* current_node = *bucket_ptr;
        while (current_node != NULL) {
            dTableEntry_t* entry = (dTableEntry_t*)current_node->data;
            if (entry && entry->value_data) {
                // Append value data to result array
                if (d_ArrayAppend(all_values_array, entry->value_data) != 0) {
                    d_LogErrorF("Failed to append value to result array at bucket %zu.", i);
                    d_ArrayDestroy(all_values_array);
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
 * @brief Iterate over all entries in the hash table
 *
 * Calls the provided callback function for each key-value pair in the table.
 * The iteration order is not guaranteed (depends on hash distribution).
 *
 * @param table The hash table to iterate
 * @param callback Function to call for each entry
 * @param user_data User-provided context pointer passed to callback
 *
 * Example:
 * @code
 * void print_entry(const void* key, size_t key_size,
 *                  const void* value, size_t value_size, void* ctx) {
 *     int* k = *(int**)key;
 *     char** v = *(char***)value;
 *     printf("Key: %d, Value: %s\n", *k, *v);
 * }
 * d_TableForEach(table, print_entry, NULL);
 * @endcode
 */
void d_TableForEach(dTable_t* table, dTableIteratorFunc callback, void* user_data)
{
    if (table == NULL || callback == NULL) {
        d_LogWarning("Cannot iterate: table or callback is NULL.");
        return;
    }

    if (table->buckets == NULL) {
        d_LogWarning("Cannot iterate: table buckets are NULL.");
        return;
    }

    d_LogDebugF("Starting table iteration over %zu buckets.", table->num_buckets);

    size_t entries_visited = 0;

    // Iterate through each bucket
    for (size_t i = 0; i < table->num_buckets; i++) {
        dLinkedList_t** bucket_ptr = (dLinkedList_t**)d_ArrayGet(table->buckets, i);

        if (bucket_ptr == NULL || *bucket_ptr == NULL) {
            continue; // Empty bucket
        }

        dLinkedList_t* current_node = *bucket_ptr;

        // Walk the linked list in this bucket
        while (current_node != NULL) {
            dTableEntry_t* entry = (dTableEntry_t*)current_node->data;

            if (entry != NULL && entry->key_data != NULL && entry->value_data != NULL) {
                // Call the callback with key and value pointers
                callback(entry->key_data, table->key_size,
                        entry->value_data, table->value_size,
                        user_data);
                entries_visited++;
            }

            current_node = current_node->next;
        }
    }

    d_LogDebugF("Table iteration complete. Visited %zu entries (expected: %zu).",
                entries_visited, table->count);
}

