#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Daedalus.h"

/**
 * @brief Internal helper function: Creates and initializes a single new linked list node.
 *
 * This function handles the memory allocation and data copying for a single
 * `dLinkedList_t` node. It is intended for internal use by other linked list
 * functions (like push operations) that need to create new nodes.
 *
 * @param data A pointer to the data to be stored in the new node. The data
 * will be copied by `size` bytes.
 * @param name A C-string (char array) representing the name for the new node.
 * This name will be copied into the node's internal buffer.
 * @param size The size in bytes of the data to be copied from `data`.
 *
 * @return A pointer to the newly created `dLinkedList_t` node, or `NULL` if
 * memory allocation fails.
 *
 * @note This function performs necessary memory allocations and error checks.
 * @warning This is an internal helper; direct use from outside the linked list
 * implementation file is generally discouraged.
 */
static dLinkedList_t* _d_CreateLinkedListNodeInternal( void *data, char *name, size_t size )
{
  dLinkedList_t* newNode = ( dLinkedList_t* )malloc( sizeof( dLinkedList_t ) );

  if ( newNode == NULL )
  {
    d_LogError("Failed to allocate memory for internal linked list node.");
    return NULL;
  }

  newNode->data = malloc( size );
  if ( newNode->data == NULL )
  {
    d_LogError("Failed to allocate memory for data in internal linked list node.");
    free( newNode );
    return NULL;
  }

  memcpy( newNode->data, data, size );
  STRNCPY( newNode->buffer, name, MAX_FILENAME_LENGTH );
  newNode->next = NULL;

  return newNode;
}

// =============================================================================
// LINKED LIST INITIALIZATION AND DESTRUCTION
// =============================================================================

/**
 * @brief Initializes a new linked list with its first node.
 *
 * This function allocates memory for the head node of a new `dLinkedList_t` and copies
 * the provided data and name into it. This newly created node serves as the starting
 * point of the linked list.
 *
 * @param data A pointer to the data to be stored in the first node. The data
 * will be copied by `size` bytes.
 * @param name A C-string (char array) representing the name to be associated
 * with this initial node. This name will be copied into the node's internal buffer.
 * @param size The size in bytes of the data to be copied from `data`.
 *
 * @return A pointer to the head of the new `dLinkedList_t`, or `NULL` if
 * memory allocation fails.
 *
 * @note The `data` and `name` are copied into the node; the original pointers
 * do not need to remain valid after the function returns.
 * @note To add more elements, use functions like `d_PushBackToLinkedList` or `d_PushFrontToLinkedList`.
 * @note The returned list must be eventually freed using `d_ClearLinkedList` to prevent memory leaks.
 *
 * Example:
 * `int my_int = 42;`
 * `dLinkedList_t* myList = d_InitLinkedList(&my_int, "first_item", sizeof(int));`
 * `if (myList == NULL) { d_LogError("Failed to initialize linked list!"); }`
 */
dLinkedList_t* d_InitLinkedList( void *data, char *name, size_t size )
{
  dLinkedList_t* newList = ( dLinkedList_t* )malloc( sizeof( dLinkedList_t ) );

  if ( newList == NULL )
  {
    d_LogError("Failed to allocate memory for new linked list head node."); // Use Daedalus logging
    return NULL; // Return NULL on failure instead of exiting
  }

  newList->data = malloc( size );
  if ( newList->data == NULL )
  {
    d_LogError("Failed to allocate memory for data in linked list head node."); // Use Daedalus logging
    free( newList ); // Clean up the node itself
    return NULL; // Return NULL on failure
  }

  memcpy( newList->data, data, size );
  STRNCPY( newList->buffer, name, MAX_FILENAME_LENGTH ); // Assuming STRNCPY is defined
  newList->next = NULL;

  return newList;
}

/**
 * @brief Destroys a linked list and frees all its associated memory.
 *
 * This function iterates through the entire linked list, freeing the `data`
 * pointer within each node and then freeing the node itself.
 * The `head` pointer passed by the caller will be set to `NULL` to prevent
 * dangling pointers after destruction.
 *
 * @param head A pointer to the pointer of the head of the linked list (`dLinkedList_t**`).
 * This allows the function to set the caller's head pointer to `NULL`.
 *
 * @return 0 on success, 1 on failure (e.g., if head pointer is NULL).
 *
 * @note After this function returns, the `dLinkedList_t*` pointer held by the caller
 * that was passed as `*head` will be `NULL`.
 * @note This function handles freeing the `data` stored inside each node,
 * assuming the data was allocated via `malloc` and is owned by the node.
 * If `data` points to external, unowned memory, it should not be freed here.
 *
 * Example:
 * `dLinkedList_t* myList = d_InitLinkedList(...);`
 * `// ... use myList ...`
 * `d_DestroyLinkedList(&myList);`
 * `// myList is now NULL`
 */
int d_DestroyLinkedList( dLinkedList_t **head )
{
    if ( !head || !*head )
    {
        d_LogError("Attempted to destroy a NULL or uninitialized linked list.");
        return 1;
    }

    dLinkedList_t *current = *head;
    dLinkedList_t *next_node = NULL;

    while ( current != NULL )
    {
        next_node = current->next;
        // Free the data held by the node if it was dynamically allocated
        if ( current->data )
        {
            free( current->data );
        }
        free( current ); // Free the node itself
        current = next_node;
    }

    *head = NULL; // Set the caller's pointer to NULL
    return 0;
}


// =============================================================================
// LINKED LIST ELEMENT ACCESS AND MODIFICATION
// =============================================================================

/**
 * @brief Retrieves a pointer to the data stored in a linked list node at a specific index.
 *
 * This function traverses the linked list from the head and returns a pointer
 * to the data of the node found at the specified zero-based index.
 *
 * @param head A pointer to the head node of the linked list.
 * @param index The zero-based index of the node whose data is to be retrieved.
 *
 * @return A pointer to the data of the element at the specified index,
 * or `NULL` if the list is empty or the index is out of bounds.
 *
 * @note The returned pointer points directly to the data owned by the linked list node.
 * It remains valid as long as the node itself is not removed or the list is not destroyed.
 * @note The `index` must be less than the total number of nodes in the list.
 *
 * Example:
 * `int* value_ptr = (int*)d_IndexDataFromLinkedList(myList, 0);`
 * `if (value_ptr) { int first_value = *value_ptr; }`
 */
void* d_IndexDataFromLinkedList( dLinkedList_t *head, int index )
{
  if ( head == NULL )
  {
    d_LogError("Attempted to get data from a NULL linked list head.");
    return NULL;
  }
  if ( index < 0 )
  {
    d_LogErrorF("Attempted to get data with negative index (%d) from linked list.", index);
    return NULL;
  }

  dLinkedList_t *current = head;
  int current_index = 0;

  while ( current != NULL && current_index < index )
  {
    current = current->next;
    current_index++;
  }

  if ( current == NULL ) // Index was out of bounds
  {
    d_LogWarningF("Index %d out of bounds for linked list (length less than %d).", index, index + 1);
    return NULL;
  }

  return current->data;
}

/**
 * @brief Finds and retrieves a pointer to the data stored in a linked list node by its name.
 *
 * This function traverses the linked list from the head and returns a pointer
 * to the data of the first node found with the matching name.
 *
 * @param head A pointer to the head node of the linked list.
 * @param name A C-string (char array) representing the name of the node whose data is to be retrieved.
 *
 * @return A pointer to the data of the element with the matching name,
 * or `NULL` if the list is empty, the `name` is NULL, or no node with the given name is found.
 *
 * @note The comparison is case-sensitive.
 * @note The returned pointer points directly to the data owned by the linked list node.
 * It remains valid as long as the node itself is not removed or the list is not destroyed.
 *
 * Example:
 * `MyStruct* s_ptr = (MyStruct*)d_FindDataFromLinkedList(myList, "player_data");`
 * `if (s_ptr) { // Use s_ptr }`
 */
void* d_FindDataFromLinkedList( dLinkedList_t *head, char *name )
{
  if ( head == NULL )
  {
    d_LogError("Attempted to find data in a NULL linked list head.");
    return NULL;
  }
  if ( name == NULL )
  {
    d_LogError("Attempted to find data with a NULL name in linked list.");
    return NULL;
  }

  dLinkedList_t *current;

  for ( current = head; current != NULL; current = current->next )
  {
    // Check if the current node's buffer matches the target name
    if ( strcmp( current->buffer, name ) == 0 )
    {
      return current->data;
    }
  }

  d_LogDebugF("No node found with name '%s' in linked list.", name);
  return NULL; // Name not found in the list
}

/**
 * @brief Removes the node at a specific index from the linked list.
 *
 * This function locates the node at the given zero-based index, frees its
 * internal data and the node structure itself, and re-links the list.
 *
 * @param head A pointer to the pointer of the head of the linked list (`dLinkedList_t**`).
 * This allows the function to update the caller's head pointer if the
 * first node is removed.
 * @param index The zero-based index of the node to remove.
 *
 * @return 0 on success, 1 on failure (e.g., NULL list, invalid index).
 *
 * @note If the head node is removed (index 0), the `*head` pointer will be updated.
 * @note This function frees the `data` stored within the node. If the `data`
 * points to external memory that should *not* be freed by the list,
 * the caller must handle that memory's lifecycle externally, or a
 * more advanced design with a custom `free_data_func` would be needed.
 *
 * Example:
 * `int result = d_RemoveIndexFromLinkedList(&myList, 0); // Removes the first element`
 * `if (result != 0) { d_LogError("Failed to remove node at index."); }`
 */
int d_RemoveIndexFromLinkedList( dLinkedList_t **head, int index )
{
    if ( !head || !*head )
    {
        d_LogError("Attempted to remove node from a NULL or uninitialized linked list.");
        return 1;
    }
    if ( index < 0 )
    {
        d_LogErrorF("Attempted to remove node with negative index (%d) from linked list.", index);
        return 1;
    }

    dLinkedList_t *current = *head;
    dLinkedList_t *prev = NULL;
    int current_index = 0;

    // Handle removal of the head node
    if ( index == 0 )
    {
        *head = current->next; // Move head to the next node
        if ( current->data ) free( current->data ); // Free data
        free( current ); // Free the node itself
        return 0;
    }

    // Traverse to the node before the target node
    while ( current != NULL && current_index < index )
    {
        prev = current;
        current = current->next;
        current_index++;
    }

    // If current is NULL, index was out of bounds
    if ( current == NULL )
    {
        d_LogErrorF("Index %d is out of bounds for linked list. Node not found for removal.", index);
        return 1;
    }

    // Node found: re-link the previous node to skip the current node
    prev->next = current->next;

    if ( current->data ) free( current->data ); // Free data
    free( current ); // Free the node itself

    return 0; // Success
}

/**
 * @brief Removes the first node from the linked list that matches the specified name.
 *
 * This function traverses the linked list to find the first node with a matching
 * name, frees its internal data and the node structure itself, and re-links the list.
 *
 * @param head A pointer to the pointer of the head of the linked list (`dLinkedList_t**`).
 * This allows the function to update the caller's head pointer if the
 * first node is removed.
 * @param name A C-string (char array) representing the name of the node to remove.
 *
 * @return 0 on success, 1 on failure (e.g., NULL list, NULL name, name not found).
 *
 * @note The comparison for the name is case-sensitive.
 * @note If the head node matches the name, the `*head` pointer will be updated.
 * @note This function frees the `data` stored within the node. If the `data`
 * points to external memory that should *not* be freed by the list,
 * the caller must handle that memory's lifecycle externally.
 *
 * Example:
 * `int result = d_RemoveDataFromLinkedList(&myList, "old_item");`
 * `if (result != 0) { d_LogError("Failed to remove node by name."); }`
 */
int d_RemoveDataFromLinkedList( dLinkedList_t **head, char *name )
{
    if ( !head || !*head )
    {
        d_LogError("Attempted to remove data from a NULL or uninitialized linked list.");
        return 1;
    }
    if ( name == NULL )
    {
        d_LogError("Attempted to remove data with a NULL name from linked list.");
        return 1;
    }

    dLinkedList_t *current = *head;
    dLinkedList_t *prev = NULL;

    // Handle removal of the head node
    if ( strcmp( current->buffer, name ) == 0 )
    {
        *head = current->next; // Move head to the next node
        if ( current->data ) free( current->data ); // Free data
        free( current ); // Free the node itself
        return 0;
    }

    // Traverse to find the node, keeping track of the previous node
    while ( current != NULL && strcmp( current->buffer, name ) != 0 )
    {
        prev = current;
        current = current->next;
    }

    // If current is NULL, the name was not found
    if ( current == NULL )
    {
        d_LogWarningF("Node with name '%s' not found in linked list. No node removed.", name);
        return 1; // Node not found
    }

    // Node found: re-link the previous node to skip the current node
    prev->next = current->next;

    if ( current->data ) free( current->data ); // Free data
    free( current ); // Free the node itself

    return 0; // Success
}

/**
 * @brief Adds a new node to the end of the linked list.
 *
 * This function creates a new node with the provided data and name, and
 * appends it to the tail of the linked list. If the list is initially empty,
 * the new node becomes the head of the list.
 *
 * @param head A pointer to the pointer of the head of the linked list (`dLinkedList_t**`).
 * This allows the function to update the caller's head pointer if the
 * list is empty.
 * @param data A pointer to the data to be copied into the new node.
 * @param name A C-string representing the name for the new node.
 * @param size The size in bytes of the data to be copied.
 *
 * @return 0 on success, 1 on failure (e.g., if memory allocation fails).
 *
 * @note The `data` and `name` are copied into the new node; their original
 * pointers do not need to remain valid after the function returns.
 * @note If `*head` is `NULL` (empty list), the new node becomes the head.
 *
 * Example:
 * `dLinkedList_t* myList = NULL; // Start with an empty list`
 * `int value1 = 10;`
 * `d_PushBackToLinkedList(&myList, &value1, "item1", sizeof(int));`
 * `int value2 = 20;`
 * `d_PushBackToLinkedList(&myList, &value2, "item2", sizeof(int));`
 */
int d_PushBackToLinkedList( dLinkedList_t **head, void *data, char *name, size_t size )
{
  if ( !head || data == NULL || name == NULL || size == 0)
  {
    d_LogError("Invalid input for d_PushBackToLinkedList: head pointer, data, name, or size is NULL/zero.");
    return 1;
  }

  dLinkedList_t *newNode = _d_CreateLinkedListNodeInternal( data, name, size );
  if ( newNode == NULL )
  {
    return 1; // _d_CreateLinkedListNodeInternal already logged the error
  }

  // If the list is empty, the new node becomes the head
  if ( *head == NULL )
  {
    *head = newNode;
  }
  else
  {
    // Traverse to the end of the list
    dLinkedList_t *current = *head;
    while ( current->next != NULL )
    {
      current = current->next;
    }
    // Append the new node
    current->next = newNode;
  }
  return 0; // Success
}

/**
 * @brief Adds a new node to the beginning of the linked list.
 *
 * This function creates a new node with the provided data and name, and
 * inserts it at the front of the linked list, making it the new head.
 *
 * @param head A pointer to the pointer of the head of the linked list (`dLinkedList_t**`).
 * This allows the function to update the caller's head pointer to the new node.
 * @param data A pointer to the data to be copied into the new node.
 * @param name A C-string representing the name for the new node.
 * @param size The size in bytes of the data to be copied.
 *
 * @return 0 on success, 1 on failure (e.g., if memory allocation fails).
 *
 * @note The `data` and `name` are copied into the new node; their original
 * pointers do not need to remain valid after the function returns.
 * @note The `*head` pointer will always be updated to point to the newly added node.
 *
 * Example:
 * `dLinkedList_t* myList = d_InitLinkedList(...); // Assume list is already initialized`
 * `int new_value = 5;`
 * `d_PushFrontToLinkedList(&myList, &new_value, "new_item", sizeof(int));`
 * `// new_item is now the first element in myList`
 */
int d_PushFrontToLinkedList( dLinkedList_t **head, void *data, char *name, size_t size )
{
  if ( !head || data == NULL || name == NULL || size == 0)
  {
    d_LogError("Invalid input for d_PushFrontToLinkedList: head pointer, data, name, or size is NULL/zero.");
    return 1;
  }

  dLinkedList_t *newNode = _d_CreateLinkedListNodeInternal( data, name, size );
  if ( newNode == NULL )
  {
    return 1; // _d_CreateLinkedListNodeInternal already logged the error
  }

  // Set the new node's next pointer to the current head
  newNode->next = *head;

  // Update the head pointer to the new node
  *head = newNode;

  return 0; // Success
}

/**
 * @brief Removes and returns the data from the last node of the linked list.
 *
 * This function removes the tail node from the linked list, frees its memory,
 * and returns a pointer to the data that was stored in that node.
 *
 * @param head A pointer to the pointer of the head of the linked list (`dLinkedList_t**`).
 * This is necessary to correctly handle the case where the last (and only)
 * node is popped, making the list empty.
 *
 * @return A pointer to the data from the removed tail node, or `NULL` if
 * the list is empty or an error occurs.
 *
 * @note The memory for the removed node itself (and its internal `data` buffer)
 * is freed by this function. The caller is responsible for managing the returned
 * `void* data` if they need it after this function call.
 *
 * Example:
 * `int* last_int_ptr = (int*)d_PopBackFromLinkedList(&myList);`
 * `if (last_int_ptr) { printf("Popped: %d\n", *last_int_ptr); free(last_int_ptr); }`
 * `else { d_LogWarning("List was empty or pop failed."); }`
 */
void* d_PopBackFromLinkedList( dLinkedList_t **head )
{
    if ( !head || *head == NULL )
    {
        d_LogError("Attempted to pop from the back of a NULL or empty linked list.");
        return NULL;
    }

    void *popped_data = NULL;
    dLinkedList_t *current = *head;
    dLinkedList_t *prev = NULL;

    // Case 1: Only one node in the list
    if ( current->next == NULL )
    {
        popped_data = current->data; // Get data from the head
        free( current );             // Free the head node
        *head = NULL;                // Set the caller's head to NULL (list is now empty)
        return popped_data;
    }

    // Case 2: Multiple nodes in the list
    // Traverse to the second-to-last node
    while ( current->next != NULL )
    {
        prev = current;
        current = current->next;
    }

    // 'current' is now the last node, 'prev' is the second-to-last
    popped_data = current->data; // Get data from the last node
    free( current );             // Free the last node
    prev->next = NULL;           // Terminate the list at the new last node

    return popped_data;
}

/**
 * @brief Removes and returns the data from the first node (head) of the linked list.
 *
 * This function removes the head node from the linked list, frees its memory,
 * updates the list's head to the next node, and returns a pointer to the data
 * that was stored in the original head node.
 *
 * @param head A pointer to the pointer of the head of the linked list (`dLinkedList_t**`).
 * This is necessary to correctly update the list's head pointer after removal.
 *
 * @return A pointer to the data from the removed head node, or `NULL` if
 * the list is empty or an error occurs.
 *
 * @note The memory for the removed node itself (and its internal `data` buffer)
 * is freed by this function. The caller is responsible for managing the returned
 * `void* data` if they need it after this function call.
 *
 * Example:
 * `char* first_name_ptr = (char*)d_PopFrontFromLinkedList(&myList);`
 * `if (first_name_ptr) { printf("Popped: %s\n", first_name_ptr); free(first_name_ptr); }`
 * `else { d_LogWarning("List was empty or pop failed."); }`
 */
void* d_PopFrontFromLinkedList( dLinkedList_t **head )
{
    if ( !head || *head == NULL )
    {
        d_LogError("Attempted to pop from the front of a NULL or empty linked list.");
        return NULL;
    }

    dLinkedList_t *old_head = *head; // Store the current head
    void *popped_data = old_head->data; // Get data from the current head

    *head = old_head->next; // Update the list's head to the next node

    free( old_head ); // Free the original head node
    // Note: old_head->data is not freed here, as it's returned to the caller.
    // If the data was *not* intended to be returned, it would be free(old_head->data); here.

    return popped_data;
}

// =============================================================================
// LINKED LIST UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Prints the names of all nodes in the linked list to standard output.
 *
 * This function traverses the linked list from the head to the tail and
 * prints the `name` (stored in `current->buffer`) of each node.
 * It's primarily intended for debugging and visualization purposes.
 *
 * @param head A pointer to the head node of the linked list.
 *
 * @note This function uses `printf` to output to `stdout`.
 * @note If the list is empty, it will print a message indicating so.
 * @note Only the `name` (buffer) of each node is printed, not the generic `data` itself.
 *
 * Example:
 * `d_PrintLinkedList(myList);`
 * `// Possible output:`
 * `// Linked List Contents:`
 * `//   - Node: item1`
 * `//   - Node: item2`
 */
void d_PrintLinkedList( dLinkedList_t *head )
{
  dLinkedList_t *current = head;
  d_LogInfo("Linked List Contents:"); // Using Daedalus logging for consistency

  if ( head == NULL )
  {
    d_LogInfo("  (List is empty)");
    return;
  }

  while( current != NULL )
  {
    // Using d_LogInfoF for consistent logging within the Daedalus framework
    d_LogInfoF("  - Node: %s", current->buffer );
    current = current->next;
  }
}

/**
 * @brief Calculates and returns the number of nodes (length) in the linked list.
 *
 * This function traverses the entire linked list from head to tail, counting
 * each node encountered.
 *
 * @param head A pointer to the head node of the linked list.
 *
 * @return The total number of nodes in the linked list. Returns 0 if the list is empty.
 *
 * @note The return type is `size_t` for consistency with array counts and to
 * support potentially very large lists.
 *
 * Example:
 * `size_t list_length = d_GetLengthOfLinkedList(myList);`
 * `d_LogInfoF("Linked List Length: %zu", list_length);`
 */
size_t d_GetLengthOfLinkedList( dLinkedList_t *head )
{
  dLinkedList_t *current = head;
  size_t count = 0; // Changed to size_t for consistency with array counts

  while( current != NULL )
  {
    current = current->next;
    ++count;
  }
  return count;
}

// =============================================================================
// LINKED LIST HASH TABLE UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Check if a linked list contains a node with the specified name.
 *
 * This function traverses the linked list to determine if any node's name
 * (stored in the buffer field) matches the given target_name. This is useful
 * for hash table implementations where you need to quickly verify existence
 * without retrieving the actual data.
 *
 * @param head A pointer to the head node of the linked list.
 * @param target_name A C-string representing the name to search for.
 *
 * @return 0 if the name is found, 1 if not found or on error.
 *
 * @note The comparison is case-sensitive using strcmp().
 * @note Returns 1 (not found) if head or target_name is NULL.
 * @note This function is optimized for existence checking and returns
 * immediately upon finding the first match.
 *
 * Example:
 * `int exists = d_CheckForNameInLinkedList(myList, "user_config");`
 * `if (exists == 0) { // Name found in list }`
 */
int d_CheckForNameInLinkedList(dLinkedList_t *head, char *target_name)
{
    if (!head || !target_name) {
        d_LogError("Attempted to check for name in NULL linked list or with NULL target name.");
        return 1; // Not found / error
    }

    dLinkedList_t *current = head;

    while (current != NULL) {
        if (strcmp(current->buffer, target_name) == 0) {
            d_LogDebugF("Found node with name '%s' in linked list.", target_name);
            return 0; // Found
        }
        current = current->next;
    }

    d_LogDebugF("Node with name '%s' not found in linked list.", target_name);
    return 1; // Not found
}

/**
 * @brief Update existing data by name or insert new node if name not found (upsert operation).
 *
 * This function implements an "upsert" (update or insert) operation for linked lists.
 * It searches for a node with the specified name:
 * - If found: Updates the node's data with new_data, freeing the old data first
 * - If not found: Creates a new node and appends it to the end of the list
 *
 * @param head A pointer to the pointer of the head of the linked list (dLinkedList_t**).
 * This allows the function to update the head pointer if the list is initially empty.
 * @param new_data A pointer to the new data to store or update.
 * @param target_name A C-string representing the name of the node to update or create.
 * @param new_size The size in bytes of the new data.
 *
 * @return 0 on success (either update or insert), 1 on failure.
 *
 * @note The new_data is copied into the node; the original pointer does not
 * need to remain valid after the function returns.
 * @note If updating an existing node, the old data memory is freed before
 * allocating and copying the new data.
 * @note If the list is empty and a new node is created, it becomes the head.
 *
 * Example:
 * `PlayerStats stats = {level: 5, health: 100};`
 * `int result = d_UpdateDataByNameLinkedList(&gameData, &stats, "player1", sizeof(PlayerStats));`
 * `// Updates existing player1 data or creates new player1 node`
 */
int d_UpdateDataByNameLinkedList(dLinkedList_t **head, void *new_data, char *target_name, size_t new_size)
{
    if (!head || !new_data || !target_name || new_size == 0) {
        d_LogError("Invalid input for d_UpdateDataByNameLinkedList: head, data, name, or size is NULL/zero.");
        return 1; // Failure
    }

    // Try to find existing node with the target name
    dLinkedList_t *current = *head;

    while (current != NULL) {
        if (strcmp(current->buffer, target_name) == 0) {
            // Found existing node - update its data
            d_LogDebugF("Updating existing node '%s' with new data.", target_name);
            
            // Free the old data
            if (current->data) {
                free(current->data);
            }
            
            // Allocate new data memory
            current->data = malloc(new_size);
            if (!current->data) {
                d_LogErrorF("Failed to allocate memory for updated data in node '%s'.", target_name);
                return 1; // Failure
            }
            
            // Copy new data
            memcpy(current->data, new_data, new_size);
            
            d_LogDebugF("Successfully updated node '%s' with new data.", target_name);
            return 0; // Success - updated existing node
        }
        current = current->next;
    }

    // Node not found - insert new node at the end
    d_LogDebugF("Node '%s' not found. Creating new node and appending to list.", target_name);
    
    // Use existing push back functionality to append new node
    int result = d_PushBackToLinkedList(head, new_data, target_name, new_size);
    if (result == 0) {
        d_LogDebugF("Successfully created and appended new node '%s'.", target_name);
    } else {
        d_LogErrorF("Failed to create and append new node '%s'.", target_name);
    }
    
    return result; // Return result from push back operation
}

/**
 * @brief Get a direct pointer to the node (not just data) with the specified name.
 *
 * This function traverses the linked list and returns a pointer to the actual
 * dLinkedList_t node structure that matches the given target_name. This is
 * different from d_FindDataFromLinkedList which returns the void* data pointer.
 * Getting the node pointer allows for direct manipulation of the node structure
 * itself, which is useful for advanced operations like hash table chaining.
 *
 * @param head A pointer to the head node of the linked list.
 * @param target_name A C-string representing the name of the node to retrieve.
 *
 * @return A pointer to the dLinkedList_t node that matches the target_name,
 * or NULL if not found or on error.
 *
 * @note The comparison is case-sensitive using strcmp().
 * @note Returns NULL if head or target_name is NULL.
 * @note The returned node pointer remains valid until the node is removed
 * or the list is destroyed.
 * @note Use this function when you need access to the entire node structure,
 * not just the data it contains.
 *
 * Example:
 * `dLinkedList_t* node = d_GetNodeByNameLinkedList(myList, "config_node");`
 * `if (node) { // Direct access to node->data, node->buffer, node->next }`
 */
dLinkedList_t* d_GetNodeByNameLinkedList(dLinkedList_t *head, char *target_name)
{
    if (!head || !target_name) {
        d_LogError("Attempted to get node from NULL linked list or with NULL target name.");
        return NULL;
    }

    dLinkedList_t *current = head;

    while (current != NULL) {
        if (strcmp(current->buffer, target_name) == 0) {
            d_LogDebugF("Found and returning node pointer for name '%s'.", target_name);
            return current; // Return pointer to the node itself
        }
        current = current->next;
    }

    d_LogDebugF("Node with name '%s' not found in linked list.", target_name);
    return NULL; // Not found
}
