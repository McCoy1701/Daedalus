#include <stdio.h>
#include <stdlib.h>

#include "daedalus.h"

int main( int argc, char* argv[] )
{
  int data = 10;
  dLinkedList *testList = dCreateLinkedList( &data, "first", sizeof( int ) );
  
  char buffer[MAX_FILENAME_LENGTH];

  for ( int i = 11; i < 20; i++ )
  {
    printf("%d\n", i);
    
    sprintf( buffer, "test%d", i );

    dPushFront( &testList, &i, buffer, sizeof( int ) );
  }

  dPrintLinkedList( testList );

  for ( int i = 0; i < 9; i++ )
  {
    int *temp = dPopFront( &testList );
    printf( "Should be: %d | Is: %d\n", i + 10, *temp );
  }
  
  int *temp = dGetDataInLinkedListByName( testList, "test11" );
  if ( temp == NULL )
  {
    exit(0);
  }
  printf( "Is: %d\n", *temp );

  //dPrintLinkedList( testList );
}
