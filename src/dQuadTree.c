#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Daedalus.h"

dQuadTree_t *d_CreateQuadtree( float *rect, int capacity )
{
  dQuadTree_t *newTree = ( dQuadTree_t* )malloc( sizeof( dQuadTree_t ) );
  if (newTree == NULL )
  {
    printf( "Failed to allocate memory for quad tree");
    return NULL;
  }

  memcpy( newTree->rect, rect, ( sizeof( float ) * 4 ) );
  newTree->capacity = capacity;
  newTree->objects = NULL;

  return newTree;
}

void d_InsertObjectInQuadtree( dQuadTree_t *tree, void *object )
{
  if ( tree->objects == NULL || d_getLengthOfLinkedList( tree->objects ) < tree->capacity )
  {
    d_pushBack(tree->objects, object, "null", sizeof( object ) );
  }

}

void d_SubdivideQuadtree( dQuadTree_t *tree )
{

}
