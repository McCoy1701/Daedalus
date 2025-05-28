#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Daedalus.h"

DynamicArray_t* d_InitArray( size_t capacity, size_t element_size )
{
  DynamicArray_t* new_array = ( DynamicArray_t* )malloc( sizeof( DynamicArray_t ) );
  if ( new_array == NULL )
  {
    printf( "Failed to allocate memory for array\n" );
    return NULL;
  }

  new_array->data = ( void* )malloc( capacity * element_size );

  new_array->capacity = capacity;
  new_array->element_size = element_size;
  new_array->count = 0;

  return new_array;
}

void d_AppendArray( DynamicArray_t* array, void* data )
{
  if ( array->count < array->capacity )
  {
    void* dest = ( char* )array->data + ( array->count * array->element_size );
    memcpy( dest, data, array->element_size );
    array->count++; 
  }
  
  else
  {
    printf( "Failed to add data to array too many items! count: %zd, capacity %zd", array->count, array->capacity );
  }
}

void* d_GetDataFromArrayByIndex( DynamicArray_t* array, size_t count )
{
  if ( count < array->capacity )
  {
    return ( char* )array->data + ( count * array->element_size );
  }

  else
  {
    printf( "Index out of bounds %zd / %zd\n", count, array->capacity );
    return NULL;
  }
}

void* d_PopDataFromArray( DynamicArray_t* array )
{
  if ( array->count == 0 )
  {
    return NULL;

  }
  
  else
  {
    void* last_item = ( char* )array->data + ( array->count * array->element_size );
    array->count--;
    return last_item;
  }
}

int d_GrowArray( DynamicArray_t* array, size_t new_capacity )
{
  return da_resizeArray( array, array->capacity + new_capacity );
}

int d_ResizeArray( DynamicArray_t* array, size_t new_capacity )
{
  void* new_array = realloc( array->data, new_capacity );
  if ( new_array == NULL )
  {
    printf( "Failed to allocate new memory for array\n" );
    return 1;
  }

  array->data = new_array;
  array->capacity = new_capacity;
  return 0;
}

void d_DestroyArray( DynamicArray_t* array )
{
  free( array->data );
  free( array );
}

