#include <stdio.h>
#include <stdlib.h>

#include "daedalus.h"

int main( int argc, char* argv[] )
{
  float a[2] = { 0.0f };
  float b[2] = { 0.0f };
  b[0] = 1.0f; b[1] = 1.0f;
  float test = dDistance2f( a, b );
  printf("Test: %f\n", test);
  float test1 = dLength2f( b );
  printf("Test1: %f\n", test1);
}
