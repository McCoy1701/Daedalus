#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Daedalus.h"

dQuadTree_t *dCreateQuadTree( float *rect, int capacity )
{
  dQuadTree_t *newTree = ( dQuadTree_t* )malloc( sizeof( dQuadTree_t ) );
  if (newTree == NULL )
  {
    printf( "Failed to allocate memory for quad tree");
    return NULL;
  }

  memcpy( newTree->rect, rect, sizeof( rect ) );
  newTree->capacity = capacity;
  newTree->objects = NULL;

  return newTree;
}

void dInsertObjectInQuadTree( dQuadTree_t *tree, void *object )
{
  if ( tree->objects == NULL || dGetLengthOfLinkedList( tree->objects ) < tree->capacity )
  {
    dPushBack(tree->objects, object, "null", sizeof( object ) );
  }

}

void dSubdivideQuadTree( dQuadTree_t *tree )
{

}
