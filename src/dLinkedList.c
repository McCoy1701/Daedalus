#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "daedalus.h"

dLinkedList* dCreateLinkedList( void *data, size_t size )
{
  dLinkedList* newList = ( dLinkedList* )malloc( sizeof( dLinkedList ) );
  
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
  newList->next = NULL;
  
  return newList;
}

void dPush( dLinkedList **head, void *data, size_t size )
{
  dLinkedList *current = *head;
  dLinkedList *newList = dCreateLinkedList( data, size );

  while ( current->next != NULL )
  {
    current = current->next;
  }

  current->next = newList;
}

void* dPop( dLinkedList *head )
{
  dLinkedList *current = head;
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

void* dGetDataInLinkedListByIndex( dLinkedList *head, int index )
{
  dLinkedList *current = head;

  for ( int i = 0; i < index; i ++ )
  {
    current = current->next;
  }
  
  return current->data;
}

void dRemoveNodeInLinkedListByIndex( dLinkedList *head, int index )
{
  dLinkedList *current = head;
  dLinkedList *temp = NULL;

  for ( int i = 0; i < index; i ++ )
  {
    current = current->next;
  }

  if ( current->next == NULL )
  {
    return;
  }

  temp = current->next;
  current->next = temp->next;
  free( temp );
}

void dClearLinkedList( dLinkedList *head )
{
  dLinkedList *current = head;
  dLinkedList *next = NULL;

  while ( current != NULL )
  {
    next = current->next;
    free( current );
    current = next;
  }
  
  head = NULL;
}

void dPrintLinkedList( dLinkedList *head )
{
  dLinkedList *current = head;

  while( current != NULL )
  {
    printf( "Data: %d\n", *( int * )current->data );
    current = current->next;
  }
}
