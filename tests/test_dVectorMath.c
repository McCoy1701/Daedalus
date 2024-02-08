#include <stdio.h>
#include <stdlib.h>

#include "daedalus.h"


int test_dSqrt( void )
{
  float test0 = dSqrt( 25 );
  float test1 = dSqrt( 144 );
  float test2 = dSqrt( 169 );

  if ( test0 != 5 )
  {
    printf( "Test 1 failed: test_dSqrt | Expected: 5 | Received: %f\n", test0 );
    return -1;
  }
  
  if ( test1 != 12 )
  {
    printf( "Test 2 failed: test_dSqrt | Expected: 12 | Received: %f\n", test1 );
    return -1;
  }
  
  if ( test2 != 13 )
  {
    printf( "Test 3 Failed: test_dSqrt | Expected: 13 | Received: %f\n", test2 );
    return -1;
  }

  printf( "Test Succeeded: test_dSqrt | Expected 5, 12, 13 | Received: %f, %f, %f\n", test0, test1, test2 );
  return 0;
}


/* Vector Math Float */
int test_dLength2f( void )
{
  float vec0[2] = { 1.0f, 1.0f };
  float test0 = dLength2f( vec0 );

  if ( test0 != 1.0f )
  {
    printf( "Test Failed: test_dLength2f | Expexted: 1 | Recevied: %f\n", test0 );
    return -1;
  }

  printf( "Test Succeeded: test_dLength2f | Expexted: 1 | Recevied: %f\n", test0 );
  return 1;
}

int test_dDistance2f( void )
{

}

int test_dDot2f( void )
{

}

int test_dCross2f( void )
{

}

int test_dNormalize2f( void )
{

}

int test_dNormal2f( void )
{

}

int test_dIntersect2f( void )
{

}


int test_dLength3f( void )
{

}

int test_dDistance3f( void )
{

}

int test_dDot3f( void )
{

}

int test_dCross3f( void )
{

}

int test_dNormalize3f( void )
{

}

int test_dNormal3f( void )
{

}

int test_dNormalize4f( void )
{

}

/* Vector Math Double */
int test_dLength2d( void )
{

}

int test_dDistance2d( void )
{

}

int test_dDot2d( void )
{

}

int test_dCross2d( void )
{

}

int test_dNormalize2d( void )
{

}

int test_dNormal2d( void )
{

}

int test_dIntersect2d( void )
{

}
int test_dLength3d( void )
{

}

int test_dDistance3d( void )
{

}

int test_dDot3d( void )
{

}

int test_dCross3d( void )
{

}

int test_dNormalize3d( void )
{

}

int test_dNormal3d( void )
{

}

int test_dNormalize4d( void )
{

}

/* Vector Integer Math */
int test_dSqrti( void )
{

}

int test_dNormalize2di( void )
{

}

int test_dNormalize3di( void )
{

}

int test_dIntersect2di( void )
{

}
