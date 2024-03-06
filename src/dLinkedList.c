#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Daedalus.h"

dLinkedList_t* dCreateLinkedList( void *data, char *name, size_t size )
{
  dLinkedList_t* newList = ( dLinkedList_t* )malloc( sizeof( dLinkedList_t ) );
  
  if ( newList == NULL )
  {
    printf("Failed to create list\n");
    exit(1);
  }

  newList->data = malloc( size );
  if ( newList->data == NULL )
  {
    printf("Failed to allocate memory for data\n");
    free( newList );
    exit(1);
  }
  
  memcpy( newList->data, data, size );
  STRNCPY( newList->buffer, name, MAX_FILENAME_LENGTH );
  newList->next = NULL;
  
  return newList;
}

void dPushFront( dLinkedList_t **head, void *data, char *name, size_t size )
{
  dLinkedList_t *temp = dCreateLinkedList( data, name, size );
  if ( *head != NULL )
  {
    temp->next = *head;
    *head = temp;
  }

  else
  {
    *head = temp;
  }
}

void dPushBack( dLinkedList_t *head, void *data, char *name, size_t size )
{
  dLinkedList_t *newList = dCreateLinkedList( data, name, size );
  
  if ( head != NULL )
  {
    dLinkedList_t *current = head;

    while ( current->next != NULL )
    {
      current = current->next;
    }

    current->next = newList;
  }

  else
  {
    head = newList;
  }
}

void* dPopFront( dLinkedList_t **head )
{
  dLinkedList_t *temp = NULL;
  void *data;

  if ( head == NULL )
  {
    exit(1);
  }

  temp = (*head)->next;
  data = (*head)->data;
  free( *head );
  *head = temp;

  return data;
}

void* dPopBack( dLinkedList_t *head )
{
  dLinkedList_t *current = head;
  void *temp;

  if ( current->next == NULL )
  {
    temp = current->data;
    free( head );
    return temp;
  }

  while( current->next->next != NULL )
  {
    current = current->next;
  }

  temp = current->next->data;
  free( current->next );
  current->next = NULL;

  return temp;
}

void* dGetDataInLinkedListByIndex( dLinkedList_t *head, int index )
{
  dLinkedList_t *current = head;

  for ( int i = 0; i < index; i ++ )
  {
    current = current->next;
  }
  
  return current->data;
}

void* dGetDataInLinkedListByName( dLinkedList_t *head, char *name )
{
  dLinkedList_t *current;

  for ( current = head; current != NULL; current = current->next )
  { 
    if ( strcmp( current->buffer, name ) == 0 )
    {
      return current->data;
    }
  }
  
  return NULL;
}

void dRemoveNodeInLinkedListByIndex( dLinkedList_t *head, int index )
{
  dLinkedList_t *current = head;
  dLinkedList_t *temp = NULL;

  for ( int i = 0; i < index; i ++ )
  {
    current = current->next;
  }

  if ( current->next == NULL )
  {
    exit(1);
  }

  temp = current->next;
  current->next = temp->next;
  free( temp );
}

void dRemoveNodeInLinkedListByName( dLinkedList_t *head, char *name )
{
  dLinkedList_t *current;
  dLinkedList_t *temp = NULL;

  for ( current = head; current->next != NULL; current = current->next )
  {
    if ( strcmp( current->buffer, name ) == 0 )
    { 
      temp = current->next;
      current->next = temp->next;
      free( temp );
    }
  }
}

void dClearLinkedList( dLinkedList_t *head )
{
  dLinkedList_t *current = head;
  dLinkedList_t *next = NULL;

  while ( current != NULL )
  {
    next = current->next;
    free( current );
    current = next;
  }
  
  head = NULL;
}

void dPrintLinkedList( dLinkedList_t *head )
{
  dLinkedList_t *current = head;

  while( current != NULL )
  {
    printf( "Data: %s\n", current->buffer );
    current = current->next;
  }
}

int dGetLengthOfLinkedList( dLinkedList_t *head )
{
  dLinkedList_t *current = head;
  int count = 0;

  while( current != NULL )
  {
    current = current->next;
    ++count;
  }
  return count;
}
