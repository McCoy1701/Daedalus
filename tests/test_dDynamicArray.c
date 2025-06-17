#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Daedalus.h"
#include "test.h"

int test_d_initArray( void )
{
  size_t cap = 10;
  size_t int_size = sizeof(int);

  dArray_t* arr = d_initArray( cap, int_size );

  assert( arr != NULL );
  assert( arr->data != NULL );

  assert( arr->capacity == cap );
  assert( arr->element_size == int_size );
  assert( arr->capacity == 0 );

  int* data = (int*)arr->data;
  for ( size_t i = 0; i < arr->capacity; i++ )
  {
    data[i] = i;
  }

  for ( size_t i = 0; i < arr->capacity; i++ )
  {
    assert( data[i] = i );
  }

  printf( "d_initArray passed\n" );

  free( arr->data );
  free( arr );

  return 0;
}

int test_d_appendArray( void )
{

  return 0;
}

int test_d_getDataFromArrayByIndex( void )
{

  return 0;
}

int test_d_popDataFromArray( void )
{

  return 0;
}

int test_d_resizeArray( void )
{

  return 0;
}

int test_d_destroyArray( void )
{

  return 0;
}
