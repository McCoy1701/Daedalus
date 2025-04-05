#include <stdio.h>
#include <stdlib.h>

#include "Daedalus.h"
#include "test.h"

int test_d_sqrtf( void )
{
  float test0 = d_sqrtf( 25 );
  float test1 = d_sqrtf( 144 );
  float test2 = d_sqrtf( 169 );

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
int test_d_lengthOfVec2f( void )
{
  dVec2_t vec0 = { 1.0f, 1.0f };
  float test0 = d_lengthOfVec2f( vec0 );

  if ( test0 != 1.0f )
  {
    printf( "Test Failed: test_dLength2f | Expexted: 1 | Recevied: %f\n", test0 );
    return -1;
  }

  printf( "Test Succeeded: test_dLength2f | Expexted: 1 | Recevied: %f\n", test0 );
  return 1;
}

/* Vector Math Float */

int test_d_distanceVec2f( void )
{

  return 0;
}

int test_d_dotProductVec2f( void )
{

  return 0;
}

int test_d_crossProductVec2f( void )
{

  return 0;
}

int test_d_addTwoVec2f( void )
{

  return 0;
}

int test_d_subTwoVec2f( void )
{

  return 0;
}

int test_d_getVec2fFromAnglef( void )
{

  return 0;
}

int test_d_scaleMultiplyVec2f( void )
{

  return 0;
}

int test_d_scaleDivideVec2f( void )
{

  return 0;
}

int test_d_limitVec2f( void )
{

  return 0;
}

int test_d_normalizeVec2f( void )
{

  return 0;
}

int test_d_createNormalVec2f( void )
{

  return 0;
}

int test_d_findIntersectionVec2f( void )
{

  return 0;
}

int test_d_lengthOfVec3f( void )
{

  return 0;
}

int test_d_distanceVec3f( void )
{

  return 0;
}

int test_d_dotProductVec3f( void )
{

  return 0;
}

int test_d_crossProductVec3f( void )
{

  return 0;
}

int test_d_addTwoVec3f( void )
{

  return 0;
}

int test_d_subTwoVec3f( void )
{

  return 0;
}

int test_d_scaleMultiplyVec3f( void )
{

  return 0;
}

int test_d_scaleDivideVec3f( void )
{

  return 0;
}

int test_d_limitVec3f( void )
{

  return 0;
}

int test_d_normalizeVec3f( void )
{

  return 0;
}

int test_d_createNormalVec3f( void )
{

  return 0;
}

int test_d_normalizeVec4f( void )
{

  return 0;
}

