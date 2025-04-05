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
float d_sqrtf( float number )
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
float d_lengthOfVec2f( const dVec2_t vec )
{
  return d_sqrt( vec.x * vec.x + vec.y * vec.y );
}

/*
  Find the distance between two vector 2fs

  Parameter:
    a: vector2f struct
    b: vector2f struct
  
  Returns:
    Distance between the two points
*/
float d_distanceVec2f( const dVec2_t a, const dVec2_t b )
{
  dVec2_t temp = { 0 };
  temp.x = a.x - b.x;
  temp.y = a.y - b.y;
  return d_sqrt( temp.x * temp.x + temp.y * temp.y );
}

/*
  Find the Dot product between two vector 2fs

  Parameters:
    a: vector2f struct
    b: vector2f struct
  
  Returns:
    Dot product between them
*/
float d_dotProductVec2f( const dVec2_t a, const dVec2_t b )
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
float d_crossProductVec2f( const dVec2_t a, const dVec2_t b )
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
void d_addTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b )
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
void d_subTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b )
{
  output->x = ( a.x - b.x );
  output->y = ( a.y - b.y );
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
void d_getVec2fFromAnglef( dVec2_t *output, float angle )
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
void d_scaleMultiplyVec2f( dVec2_t *output, const dVec2_t vec, const float value )
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
void d_scaleDivideVec2f( dVec2_t *output, const dVec2_t vec, const float value )
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
void d_limitVec2f( dVec2_t *output, const dVec2_t a, const float value )
{
  if ( d_sqrt( ( a.x * a.x ) + ( a.y * a.y ) ) > value )
  {
    float temp; //Normalize vector
    temp = d_sqrt( a.y * a.y + a.y * a.y );
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
void d_normalizeVec2f( dVec2_t *output, const dVec2_t vec )
{
  float temp;
  temp = d_sqrt( vec.x * vec.x + vec.y * vec.y );
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
void d_createNormalVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b )
{
  float temp;
  output->x = a.x - b.x;
  output->y = a.y - b.y;
  temp = d_sqrt( output->x * output->x + output->y * output->y );
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
void d_findIntersectionVec2f( dVec2_t *output, const dVec2_t lineA0, const dVec2_t lineA1, const dVec2_t lineB0, const dVec2_t lineB1 )
{
  float temp = ( ( lineA0.x - lineA1.x ) * ( lineB0.y - lineB1.y ) ) - ( ( lineA0.y - lineA1.y ) * ( lineB0.x - lineB1.x ) );

  output->x = ( ( ( lineA0.x * lineA1.y ) - ( lineA0.y * lineA1.x ) ) * ( lineB0.x - lineB1.x ) ) - 
  ( ( lineA0.x - lineA1.x ) * ( ( lineB0.x * lineB1.y ) - ( lineB0.y * lineB1.x ) ) );
  
  output->x /= temp;

  output->y = ( ( ( lineA0.x * lineA1.y ) - ( lineA0.y * lineA1.x ) ) * ( lineB0.y - lineB1.y ) ) - 
  ( ( lineA0.y - lineA1.y ) * ( ( lineB0.x * lineB1.y ) - ( lineB0.y * lineB1.x ) ) );

  output->y /= temp;
}

/*---------------- Vector 3 float Math ----------------*/

/*
  Length of a vector 3f

  Parameter:
    vec: vector3f struct
  
  Returns:
    float of length 
*/
float d_lengthOfVec3f( const dVec3_t vec )
{
  return d_sqrt( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );
}

/*
  Distance between two vector 3f

  Parameter:
    a: vector3f struct
    b: vector3f struct
  
  Returns:
    float distance between
*/
float d_distanceVec3f( const dVec3_t a, const dVec3_t b )
{
  dVec3_t output;
  output.x = a.x - b.x;
  output.y = a.y - b.y;
  output.z = a.z - b.z;
  return d_sqrt( output.x * output.x + output.y * output.y + output.z * output.z );
}

/*
  Dot product of two vector 3f

  Parameter:
    a: vector3f struct
    b: vector3f struct
  
  Returns:
    returns dot product
*/
float d_dotProductVec3f( const dVec3_t a, const dVec3_t b )
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
void d_crossProductVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b )
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
void d_addTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b )
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
void d_subTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b )
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
void d_scaleMultiplyVec3f( dVec3_t *output, const dVec3_t vec, const float value )
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
void d_scaleDivideVec3f( dVec3_t *output, const dVec3_t vec, const float value )
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
void d_limitVec3f( dVec3_t *output, const dVec3_t a, const float value )
{
  if ( d_sqrt( ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ) ) > value )
  {
    float temp; //Normalize vector
    temp = d_sqrt( ( a.x * a.x ) + ( a.y * a.y ) + ( a.z * a.z ) );
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
void d_normalizeVec3f( dVec3_t *output, const dVec3_t vec )
{
  float temp;
  temp = d_sqrt( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z );
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
void d_createNormalVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b, const dVec3_t c )
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
  
  temp = d_sqrt( output->x * output->x + output->y * output->y + output->z * output->z );
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
void d_normalizeVec4f( dVec4_t *output, const dVec4_t vec )
{
  float temp;
  temp = d_sqrt( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w );
  output->x = vec.x / temp;
  output->y = vec.y / temp;
  output->z = vec.z / temp;
  output->w = vec.w / temp;
}

