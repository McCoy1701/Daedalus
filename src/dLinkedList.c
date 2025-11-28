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
