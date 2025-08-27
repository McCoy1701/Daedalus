#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Daedalus.h"

/*
  Quake 3 fast inverse square root algorithm, but now it's square root algorithm 

  Parameter:
    number: number to find square root
  
  Returns:
    float of square root
*/
float d_Sqrtf( float number )
{
  if ( number == 1.0f ) return number;

  int i;
  float x, y;
  x = number * 0.5;
  y  = number;
  i  = * (int * ) &y;
  i  = 0x5f3759df - (i >> 1);
  y  = * ( float * ) &i;
  y  = y * (1.5 - (x * y * y));
  y  = y * (1.5 - (x * y * y));
  return number * y;
}

int d_Clampi( int value, int min, int max )
{
  if ( value < min ) return min;
  if( value > max ) return max;
  return value;
}

float d_Clampf( float value, float min, float max )
{
  if ( value < min ) return min;
  if( value > max ) return max;
  return value;
}

/*---------------------------------------------*/
/*---------------- Vector Math ----------------*/
/*---------------------------------------------*/


/*---------------- Vector 2 float Math ----------------*/

/*
  Find the length of a vector 2f

  Parameters:
    vec: vector2f struct
  
  Returns:
    Length of vector
*/
float d_LengthOfVec2f( const dVec2_t vec )
{
  return d_Sqrtf( vec.x * vec.x + vec.y * vec.y );
}

/*
  Find the distance between two vector 2fs

  Parameter:
    a: vector2f struct
    b: vector2f struct
  
  Returns:
    Distance between the two points
*/
float d_DistanceVec2f( const dVec2_t a, const dVec2_t b )
{
  dVec2_t temp = { 0 };
  temp.x = a.x - b.x;
  temp.y = a.y - b.y;
  return d_Sqrtf( temp.x * temp.x + temp.y * temp.y );
}

/*
  Find the Dot product between two vector 2fs

  Parameters:
    a: vector2f struct
    b: vector2f struct
  
  Returns:
    Dot product between them
*/
float d_DotProductVec2f( const dVec2_t a, const dVec2_t b )
{
  return ( a.x * b.x + a.y * b.y );
}

/*
  Find the Cross Product between two vector 2fs

  Parameters:
    output: contains the return value of the cross product
    a:      vector2f struct
    b:      vector2f struct
  
  Returns:
    void
*/
float d_CrossProductVec2f( const dVec2_t a, const dVec2_t b )
{
  return (a.x * b.y - a.y * b.x);
}

/*
  Add two vector 2f together

  Parameters:
    output: pointer to the finished computation
    a:      vector2f struct
    b:      vector2f struct
  
  Returns:
    void
*/
void d_AddTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b )
{
  output->x = ( a.x + b.x );
  output->y = ( a.y + b.y );
}

/*
  Subtract two vector 2f together

  Parameters:
    output: pointer to the finished computation
    a:      vector2f struct
    b:      vector2f struct
  
  Returns:
    void
*/
void d_SubTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b )
{
  output->x = ( b.x - a.x );
  output->y = ( b.y - a.y );
}

/*
  Convert angle to polar coordinates 
  
  note: still need to multiply by the radius

  Parameters:
    output: pointer to the finished computation
    a:      vector2f struct
    b:      vector2f struct
  
  Returns:
    void
*/
void d_GetVec2fFromAnglef( dVec2_t *output, float angle )
{
  output->x = cosf(angle);
  output->y = sinf(angle);
}

/*
  Scale the vector 2f by a value

  Parameters:
    output: pointer to the finished computation
    a:      vector2f struct
    value:  scale factor
  
  Returns:
    void
*/
void d_ScaleMultiplyVec2f( dVec2_t *output, const dVec2_t vec, const float value )
{
  output->x = vec.x * value;
  output->y = vec.y * value;
}

/*
  Divide the vector 2f by a value

  Parameters:
    output: pointer to the finished computation
    a:      vector2f struct
    value:  scale factor
  
  Returns:
    void
*/
void d_ScaleDivideVec2f( dVec2_t *output, const dVec2_t vec, const float value )
{
  output->x = vec.x / value;
  output->y = vec.y / value;
}

/*
  Limit a vector 2f to a value

  Parameters:
    a:     vector2f struct
    value: scale factor
  
  Returns:
    void
*/
void d_LimitVec2f( dVec2_t *output, const dVec2_t a, const float value )
{
  if ( d_Sqrtf( ( a.x * a.x ) + ( a.y * a.y ) ) > value )
  {
    float temp; //Normalize vector
    temp = d_Sqrtf( a.y * a.y + a.y * a.y );
    output->x = a.x / temp;
    output->y = a.y / temp;

    output->x *= value; //scale to value
    output->y *= value;
  }
}

/*
  Normalize a vector 2f

  Parameters:
    vec: vector2f struct
  
  Returns:
    void
*/
void d_NormalizeVec2f( dVec2_t *output, const dVec2_t vec )
{
  float temp;
  temp = d_Sqrtf( vec.x * vec.x + vec.y * vec.y );
  output->x = vec.x / temp;
  output->y = vec.y / temp;
}

/*
  Creates a normal vector from two vector 2fs

  Parameters:
    output: contains the normal vector
    a:      vector2f struct
    b:      vector2f struct
  
  Returns:
    void
*/
void d_CreateNormalVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b )
{
  float temp;
  output->x = a.x - b.x;
  output->y = a.y - b.y;
  temp = d_Sqrtf( output->x * output->x + output->y * output->y );
  output->x /= temp;
  output->y /= temp;
}

/*
  Finds the intersection of two lines

  Parameters:
    output: pointer to the finished computation
    lineA0: pointer to vector of the first point of the first line
    lineA1: pointer to vector of the second point of the first line
    lineB0: pointer to vector of the first point of the second line
    lineB1: pointer to vector of the second point of the second line
  
  Returns:
    void
*/
void d_FindIntersectionVec2f( dVec2_t *output, const dVec2_t lineA0, const dVec2_t lineA1, const dVec2_t lineB0, const dVec2_t lineB1 )
{
  float temp = ( ( lineA0.x - lineA1.x ) * ( lineB0.y - lineB1.y ) ) - ( ( lineA0.y - lineA1.y ) * ( lineB0.x - lineB1.x ) );

  output->x = ( ( ( lineA0.x * lineA1.y ) - ( lineA0.y * lineA1.x ) ) * ( lineB0.x - lineB1.x ) ) - 
  ( ( lineA0.x - lineA1.x ) * ( ( lineB0.x * lineB1.y ) - ( lineB0.y * lineB1.x ) ) );
  
  output->x /= temp;

  output->y = ( ( ( lineA0.x * lineA1.y ) - ( lineA0.y * lineA1.x ) ) * ( lineB0.y - lineB1.y ) ) - 
  ( ( lineA0.y - lineA1.y ) * ( ( lineB0.x * lineB1.y ) - ( lineB0.y * lineB1.x ) ) );

  output->y /= temp;
}

dVec2_t d_Perpendicular( dVec2_t vec )
{
  return (dVec2_t){ .x = vec.y, .y = -vec.x };
}

int d_PointOnRightSideOfLine( dVec2_t a, dVec2_t b, dVec2_t p )
{
  dVec2_t ap;
  dVec2_t ab;
  d_SubTwoVec2f( &ap, a, p );
  d_SubTwoVec2f( &ab, a, b );
  dVec2_t ab_perp = d_Perpendicular( ab );
  return d_DotProductVec2f( ap, ab_perp ) >= 0;
}

int d_PointInsideOfTriangle( dVec2_t a, dVec2_t b, dVec2_t c, dVec2_t p )
{
  int AB = d_PointOnRightSideOfLine( a, b, p );
  int BC = d_PointOnRightSideOfLine( b, c, p );
  int CA = d_PointOnRightSideOfLine( c, a, p );
  return AB == BC && BC == CA;
}

/*---------------- Vector 3 float Math ----------------*/

/*
  Length of a vector 3f

  Parameter:
    vec: vector3f struct
  
  Returns:
    float of length 
*/
float d_LengthOfVec3f( const dVec3_t vec )
{
  return d_Sqrtf( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );
}

/*
  Distance between two vector 3f

  Parameter:
    a: vector3f struct
    b: vector3f struct
  
  Returns:
    float distance between
*/
float d_DistanceVec3f( const dVec3_t a, const dVec3_t b )
{
  dVec3_t output;
  output.x = a.x - b.x;
  output.y = a.y - b.y;
  output.z = a.z - b.z;
  return d_Sqrtf( output.x * output.x + output.y * output.y + output.z * output.z );
}

/*
  Dot product of two vector 3f

  Parameter:
    a: vector3f struct
    b: vector3f struct
  
  Returns:
    returns dot product
*/
float d_DotProductVec3f( const dVec3_t a, const dVec3_t b )
{
  return ( a.x * b.x + a.y * b.y + a.z * b.z );
}

/*
  Cross product of two vector 3f

  Parameter:
    output: pointer to finished computation  
    a:      vector3f struct
    b:      vector3f struct
  
  Returns:
    void
*/
void d_CrossProductVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b )
{
  output->x = ( a.y * b.z - a.z * b.y );
  output->y = ( a.z * b.x - a.x * b.z );
  output->z = ( a.x * b.y - a.y * b.x );
}

/*
  Add two vector 3fs together

  Parameter:
    output: pointer to the finished computation
    a:      vector3f struct
    b:      vector3f struct
  
  Returns:
    void
*/
void d_AddTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b )
{
  output->x = ( a.x + b.x );
  output->y = ( a.y + b.y );
  output->z = ( a.z + b.z );
}

/*
  Subtracts two vector 3fs together

  Parameter:
    output: pointer to the finished computation
    a:      vector3f struct
    b:      vector3f struct
  
  Returns:
    void
*/
void d_SubTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b )
{
  output->x = ( a.x - b.x );
  output->y = ( a.y - b.y );
  output->z = ( a.z - b.z );
}

/*
  scale a vector 3f to a value

  Parameter:
    output: pointer to the scaled vector 3f
    value:  scale factor
  
  Returns:
    void
*/
void d_ScaleMultiplyVec3f( dVec3_t *output, const dVec3_t vec, const float value )
{
  output->x = vec.x * value;
  output->y = vec.y * value;
  output->z = vec.z * value;
}

/*
  Divide a vector 3f by a value

  Parameter:
    output: pointer to finished computation
    value:  divide factor
  
  Returns:
    void
*/
void d_ScaleDivideVec3f( dVec3_t *output, const dVec3_t vec, const float value )
{
  output->x = vec.x / value;
  output->y = vec.y / value;
  output->z = vec.z / value;
}

/*
  Limit a vector 3f to within a value

  Parameter:
    a:     pointer to first vector 3f
    value: limit factor
  
  Returns:
    void
*/
void d_LimitVec3f( dVec3_t *output, const dVec3_t a, const float value )
{
  if ( d_Sqrtf( ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ) ) > value )
  {
    float temp; //Normalize vector
    temp = d_Sqrtf( ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ) );
    output->x = a.x / temp;
    output->y = a.y / temp;
    output->z = a.z / temp;

    output->x *= value; //scale to value
    output->y *= value;
    output->z *= value;
  }
}

/*
  Normalize a vector 3f

  Parameter:
    vec: vector3f struct
  
  Returns:
    void
*/
void d_NormalizeVec3f( dVec3_t *output, const dVec3_t vec )
{
  float temp;
  temp = d_Sqrtf( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );
  output->x /= temp;
  output->y /= temp;
  output->z /= temp;
}

/*
  Create a normal from three vector 3fs

  Parameter:
    output: pointer to finished computation
    a:      vector3f struct
    b:      vector3f struct
    c:      vector3f struct
  
  Returns:
    void
*/
void d_CreateNormalVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b, const dVec3_t c )
{
  dVec3_t tempA, tempB;
  float temp;
  tempA.x = b.x - a.x;
  tempA.y = b.y - a.y;
  tempA.z = b.z - a.z;

  tempB.x = c.x - a.x;
  tempB.y = c.y - a.y;
  tempB.z = c.z - a.z;

  output->x = tempA.y * tempB.z - tempA.z * tempB.y;
  output->y = tempA.z * tempB.x - tempA.x * tempB.z;
  output->z = tempA.x * tempB.y - tempA.y * tempB.x;
  
  temp = d_Sqrtf( output->x * output->x + output->y * output->y + output->z * output->z );
  output->x /= temp;
  output->y /= temp;
  output->z /= temp;
}

/*
  Normalize a vector 4f

  Parameter:
    vec: vector4f struct
  
  Returns:
    void
*/
void d_NormalizeVec4f( dVec4_t *output, const dVec4_t vec )
{
  float temp;
  temp = d_Sqrtf( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w );
  output->x = vec.x / temp;
  output->y = vec.y / temp;
  output->z = vec.z / temp;
  output->w = vec.w / temp;
}

