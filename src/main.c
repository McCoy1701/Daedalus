#include <stdio.h>
#include <stdlib.h>

#include "daedalus.h"

int main( int argc, char* argv[] )
{
  int data = 10;
  dLinkedList *testList = dCreateLinkedList( &data, sizeof( int ) );
  
  for ( int i = 11; i < 20; i++ )
  {
    printf("%d\n", i);
    dPushFront( &testList, &i, sizeof( int ) );
  }

  dPrintLinkedList( testList );

  for ( int i = 0; i < 10; i++ )
  {
    int *temp = dPopFront( &testList );
    printf( "Should be: %d | Is: %d\n", i + 10, *temp );
  }
  
  //dPrintLinkedList( testList );
}
