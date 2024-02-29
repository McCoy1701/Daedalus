/* #include <stdio.h>
#include <stdlib.h> */
#include <math.h>

/*
  Quake 3 fast inverse square root algorithm, but now it's square root algorithm 

  Parameter:
    number: number to find square root
  
  Returns:
    float of square root
*/
float dSqrt( float number )
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
  Add two vector 2f together

  Parameters:
    output: pointer to the finished computation
    a:      pointer to the first vector 2f
    b:      pointer to the second vector 2f
  
  Returns:
    void
*/
void dAddVec2f( float *output, const float *a, const float *b )
{
  output[0] = ( a[0] + b[0] );
  output[1] = ( a[1] + b[1] );
}

/*
  Subtract two vector 2f together

  Parameters:
    output: pointer to the finished computation
    a:      pointer to the first vector 2f
    b:      pointer to the second vector 2f
  
  Returns:
    void
*/
void dSubVec2f( float *output, const float *a, const float *b )
{
  output[0] = ( a[0] - b[0] );
  output[1] = ( a[1] - b[1] );
}

void dVec2FromAnglef( float *output, float angle )
{
  output[0] = cosf(angle);
  output[1] = sinf(angle);
}

/*
  Returns the smaller of the two vector 2fs

  Parameters:
    output: pointer to the smaller vector 2f
    a:      pointer to the first vector 2f
    b:      pointer to the second vector 2f
  
  Returns:
    void
*/
void dMinVec2f( float *output, const float *a, const float *b )
{
  output[0] = ( a[0] < b[0] ? a[0] : b[0] );
  output[1] = ( a[1] < b[1] ? a[1] : b[1] );
}

/*
  Returns the larger of two vector 2fs

  Parameters:
    output: pointer to the larger vector 2f
    a:      pointer to the first vector 2f
    b:      pointer to the second vector 2f
  
  Returns:
    void
*/
void dMaxVec2f( float *output, const float *a, const float *b )
{
  output[0] = ( a[0] > b[0] ? a[0] : b[0] );
  output[1] = ( a[1] > b[1] ? a[1] : b[1] );
}

/*
  Scale the vector 2f by a value

  Parameters:
    output: pointer to the finished computation
    a:      pointer to the first vector 2f
    value:  scale factor
  
  Returns:
    void
*/
void dMultiplyVec2f( float *output, const float value )
{
  output[0] *= value;
  output[1] *= value;
}

/*
  Divide the vector 2f by a value

  Parameters:
    output: pointer to the finished computation
    a:      pointer to the first vector 2f
    value:  scale factor
  
  Returns:
    void
*/
void dDivideVec2f( float *output, const float value )
{
  output[0] /= value;
  output[1] /= value;
}

/*
  Limit a vector 2f to a value

  Parameters:
    a:      pointer to the first vector 2f
    value:  scale factor
  
  Returns:
    void
*/
void dLimitVec2f( float *a, const float value )
{
  if ( dSqrt( ( a[0] * a[0] ) + ( a[1] * a[1] ) ) > value )
  {
    float temp; //Normalize vector
    temp = dSqrt( a[0] * a[0] + a[1] * a[1] );
    a[0] /= temp;
    a[1] /= temp;

    a[0] *= value; //scale to value
    a[1] *= value;
  }
}

/*
  Find the length of a vector 2f

  Parameters:
    vec: pointer to vec 2f
  
  Returns:
    Length of vector
*/
float dLength2f( float *vec )
{
  return dSqrt( vec[0] * vec[0] + vec[1] * vec[1] );
}

/*
  Find the distance between two vector 2fs

  Parameter:
    a: pointer to first vector 2f
    b: pointer to second vector 2f
  
  Returns:
    Distance between the two points
*/
float dDistance2f( float *a, float *b )
{
  float temp[2] = { 0 };
  temp[0] = a[0] - b[0];
  temp[1] = a[1] - b[1];
  return dSqrt( temp[0] * temp[0] + temp[1] * temp[1] );
}

/*
  Find the Dot product between two vector 2fs

  Parameters:
    a: pointer to first vector 2f
    b: pointer to second vector 2f
  
  Returns:
    Dot product between them
*/
float dDot2f( float *a, float *b )
{
  return ( a[0] * b[0] + a[1] * b[1] );
}

/*
  Find the Cross Product between two vector 2fs

  Parameters:
    output: contains the return value of the cross product
    a:      pointer to first vector 2f
    b:      pointer to second vector 2f
  
  Returns:
    void
*/
float dCross2f( float *a, float *b )
{
  return (a[0] * b[1] - a[1] * b[0]);
}

/*
  Normalize a vector 2f

  Parameters:
    vec: pointer to vector 2f
  
  Returns:
    void
*/
void dNormalize2f( float *vec )
{
  float temp;
  temp = dSqrt( vec[0] * vec[0] + vec[1] * vec[1] );
  vec[0] /= temp;
  vec[1] /= temp;
}

/*
  Creates a normal vector from two vector 2fs

  Parameters:
    output: contains the normal vector
    a:      pointer to first vector 2
    b:      pointer to second vector 2
  
  Returns:
    void
*/
void dNormal2f( float *output, float *a, float *b )
{
  float temp;
  output[0] = a[0] - b[0];
  output[1] = a[1] - b[1];
  temp = dSqrt( output[0] * output[0] + output[1] * output[1] );
  output[0] /= temp;
  output[1] /= temp;
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
void dIntersect2f( float *output, float *lineA0, float *lineA1, float *lineB0, float *lineB1 )
{
  float temp = ( ( lineA0[0] - lineA1[0] ) * ( lineB0[1] - lineB1[1] ) ) - ( ( lineA0[1] - lineA1[1] ) * ( lineB0[0] - lineB1[0] ) );

  output[0] = ( ( ( lineA0[0] * lineA1[1] ) - ( lineA0[1] * lineA1[0] ) ) * ( lineB0[0] - lineB1[0] ) ) - 
  ( ( lineA0[0] - lineA1[0] ) * ( ( lineB0[0] * lineB1[1] ) - ( lineB0[1] * lineB1[0] ) ) );
  
  output[0] /= temp;

  output[1] = ( ( ( lineA0[0] * lineA1[1] ) - ( lineA0[1] * lineA1[0] ) ) * ( lineB0[1] - lineB1[1] ) ) - 
  ( ( lineA0[1] - lineA1[1] ) * ( ( lineB0[0] * lineB1[1] ) - ( lineB0[1] * lineB1[0] ) ) );

  output[1] /= temp;
}

/*---------------- Vector 3 float Math ----------------*/

/*
  Add two vector 3fs together

  Parameter:
    output: pointer to the finished computation
    a:      pointer to first vector 3f
    b:      pointer to second vector 3f
  
  Returns:
    void
*/
void dAddVec3f( float *output, const float *a, const float *b )
{
  output[0] = ( a[0] + b[0] );
  output[1] = ( a[1] + b[1] );
  output[2] = ( a[2] + b[2] );
}

/*
  Subtracts two vector 3fs together

  Parameter:
    output: pointer to the finished computation
    a:      pointer to first vector 3f
    b:      pointer to second vector 3f
  
  Returns:
    void
*/
void dSubVec3f( float *output, const float *a, const float *b )
{
  output[0] = ( a[0] - b[0] );
  output[1] = ( a[1] - b[1] );
  output[2] = ( a[2] - b[2] );
}

/*
  Returns the smaller of two vector 3fs

  Parameter:
    output: pointer to the smaller vector 3f
    a:      pointer to first vector 3f
    b:      pointer to second vector 3f
  
  Returns:
    void
*/
void dMinVec3f( float *output, const float *a, const float *b )
{
  output[0] = ( a[0] < b[0] ? a[0] : b[0] );
  output[1] = ( a[1] < b[1] ? a[1] : b[1] );
  output[2] = ( a[2] < b[2] ? a[2] : b[2] );
}

/*
  Returns the larger of two vector 3fs

  Parameter:
    output: pointer to the larger vector 3f
    a:      pointer to first vector 3f
    b:      pointer to second vector 3f
  
  Returns:
    void
*/
void dMaxVec3f( float *output, const float *a, const float *b )
{
  output[0] = ( a[0] > b[0] ? a[0] : b[0] );
  output[1] = ( a[1] > b[1] ? a[1] : b[1] );
  output[2] = ( a[2] > b[2] ? a[2] : b[2] );
}

/*
  scale a vector 3f to a value

  Parameter:
    output: pointer to the scaled vector 3f
    a:      pointer to vector 3f
    value:  scale factor
  
  Returns:
    void
*/
void dMultiplyVec3f( float *output, const float value )
{
  output[0] *= value;
  output[1] *= value;
  output[2] *= value;
}

/*
  Divide a vector 3f by a value

  Parameter:
    output: pointer to finished computation
    a:      pointer to vector 3f
    value:  divide factor
  
  Returns:
    void
*/
void dDivideVec3f( float *output, const float value )
{
  output[0] /= value;
  output[1] /= value;
  output[2] /= value;
}

/*
  Limit a vector 3f to within a value

  Parameter:
    a:     pointer to first vector 3f
    value: limit factor
  
  Returns:
    void
*/
void dLimitVec3f( float *a, const float value )
{
  if ( dSqrt( ( a[0] * a[0] ) + ( a[1] * a[1] ) + ( a[2] * a[2] ) ) > value )
  {
    float temp; //Normalize vector
    temp = dSqrt( a[0] * a[0] + a[1] * a[1] + a[2] * a[2] );
    a[0] /= temp;
    a[1] /= temp;
    a[2] /= temp;

    a[0] *= value; //scale to value
    a[1] *= value;
    a[2] *= value;
  }
}

/*
  Length of a vector 3f

  Parameter:
    vec: pointer to vector 3f
  
  Returns:
    float of length 
*/
float dLength3f( float *vec )
{
  return dSqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
}

/*
  Distance between two vector 3f

  Parameter:
    a:      pointer to first vector 3f
    b:      pointer to second vector 3f
  
  Returns:
    float distance between
*/
float dDistance3f( float *a, float *b )
{
  float output[3];
  output[0] = a[0] - b[0];
  output[1] = a[1] - b[1];
  output[2] = a[2] - b[2];
  return dSqrt( output[0] * output[0] + output[1] * output[1] + output[2] * output[2] );
}

/*
  Dot product of two vector 3f

  Parameter:
    a: pointer to first vector 3f
    b: pointer to second vector 3f
  
  Returns:
    returns dot product
*/
float dDot3f( float *a, float *b )
{
  return ( a[0] * b[0] + a[1] * b[1] + a[2] * b[2] );
}

/*
  Cross product of two vector 3f

  Parameter:
    output: pointer to finished computation  
    a:      pointer to first vector 3f
    b:      pointer to second vector 3f
  
  Returns:
    void
*/
void dCross3f( float *output, float *a, float *b )
{
  output[0] = ( a[1] * b[2] - a[2] * b[1] );
  output[1] = ( a[2] * b[0] - a[0] * b[2] );
  output[2] = ( a[0] * b[1] - a[1] * b[0] );
}

/*
  Normalize a vector 3f

  Parameter:
    vec: pointer to vector 3f
  
  Returns:
    void
*/
void dNormalize3f( float *vec )
{
  float temp;
  temp = dSqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
  vec[0] /= temp;
  vec[1] /= temp;
  vec[2] /= temp;
}

/*
  Create a normal from three vector 3fs

  Parameter:
    output: pointer to finished computation
    a:      pointer to first vector 3f
    b:      pointer to second vector 3f
    c:      pointer to third vector 3f
  
  Returns:
    void
*/
void dNormal3f( float *output, float *a, float *b, float *c )
{
  float tempA[3], tempB[3], temp;
  tempA[0] = a[0] - c[0];
  tempA[1] = a[1] - c[1];
  tempA[2] = a[2] - c[2];
  tempB[0] = b[0] - c[0];
  tempB[1] = b[1] - c[1];
  tempB[2] = b[2] - c[2];
  output[0] = tempA[1] * tempB[2] + tempA[2] * tempB[1];
  output[1] = tempA[2] * tempB[0] + tempA[0] * tempB[2];
  output[2] = tempA[0] * tempB[1] + tempA[1] * tempB[0];
  temp = dSqrt( output[0] * output[0] + output[1] * output[1] + output[2] * output[2] );
  output[0] /= temp;
  output[1] /= temp;
  output[2] /= temp;
}

/*
  Normalize a vector 4f

  Parameter:
    vec: pointer to vector 4f
  
  Returns:
    void
*/
void dNormalize4f( float *vec )
{
  float temp;
  temp = dSqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3] );
  vec[0] /= temp;
  vec[1] /= temp;
  vec[2] /= temp;
  vec[3] /= temp;
}

/*----------------------------------------------------*/
/*---------------- Vector Math Double ----------------*/
/*----------------------------------------------------*/

/*---------------- Vector 2 double Math ----------------*/

/*
  Add two vector 2d together

  Parameters:
    output: pointer to the finished computation
    a:      pointer to the first vector 2d
    b:      pointer to the second vector 2d
  
  Returns:
    void
*/
void dAddVec2d( double *output, const double *a, const double *b )
{
  output[0] = ( a[0] + b[0] );
  output[1] = ( a[1] + b[1] );
}

/*
  Subtract two vector 2d together

  Parameters:
    output: pointer to the finished computation
    a:      pointer to the first vector 2d
    b:      pointer to the second vector 2d
  
  Returns:
    void
*/
void dSubVec2d( double *output, const double *a, const double *b )
{
  output[0] = ( a[0] - b[0] );
  output[1] = ( a[1] - b[1] );
}

void dVec2FromAngled( double *output, double angle )
{
  output[0] = cosf(angle);
  output[1] = sinf(angle);
}

/*
  Returns the smaller of the two vector 2ds

  Parameters:
    output: pointer to the smaller vector 2d
    a:      pointer to the first vector 2d
    b:      pointer to the second vector 2d
  
  Returns:
    void
*/
void dMinVec2d( double *output, const double *a, const double *b )
{
  output[0] = ( a[0] < b[0] ? a[0] : b[0] );
  output[1] = ( a[1] < b[1] ? a[1] : b[1] );
}

/*
  Returns the larger of two vector 2ds

  Parameters:
    output: pointer to the larger vector 2d
    a:      pointer to the first vector 2d
    b:      pointer to the second vector 2d
  
  Returns:
    void
*/
void dMaxVec2d( double *output, const double *a, const double *b )
{
  output[0] = ( a[0] > b[0] ? a[0] : b[0] );
  output[1] = ( a[1] > b[1] ? a[1] : b[1] );
}

/*
  Scale the vector 2d by a value

  Parameters:
    output: pointer to the finished computation
    a:      pointer to the first vector 2d
    value:  scale factor
  
  Returns:
    void
*/
void dMultiplyVec2d( double *output, const double value )
{
  output[0] *= value;
  output[1] *= value;
}

/*
  Divide the vector 2d by a value

  Parameters:
    output: pointer to the finished computation
    a:      pointer to the first vector 2d
    value:  scale factor
  
  Returns:
    void
*/
void dDivideVec2d( double *output, const double value )
{
  output[0] /= value;
  output[1] /= value;
}

/*
  Limit a vector 2d to a value

  Parameters:
    a:      pointer to the first vector 2d
    value:  scale factor
  
  Returns:
    void
*/
void dLimitVec2d( double *a, const double value )
{
  if ( dSqrt( ( a[0] * a[0] ) + ( a[1] * a[1] ) ) > value )
  {
    float temp; //Normalize vector
    temp = dSqrt( a[0] * a[0] + a[1] * a[1] );
    a[0] /= temp;
    a[1] /= temp;

    a[0] *= value; //scale to value
    a[0] *= value;
  }
}

/*
  Find the length of a vector 2d

  Parameters:
    vec: pointer to vec 2d
  
  Returns:
    Length of vector
*/
double dLength2d( double *vec )
{
  return sqrt( vec[0] * vec[0] + vec[1] * vec[1] );
}

/*
  Find the distance between two vector 2ds

  Parameter:
    a: pointer to first vector 2d
    b: pointer to second vector 2d
  
  Returns:
    Distance between the two points
*/
double dDistance2d( double *a, double *b )
{
  double temp[2] = { 0 };
  temp[0] = a[0] - b[0];
  temp[1] = a[1] - b[1];
  return sqrt( temp[0] * temp[0] + temp[1] * temp[1] );
}

/*
  Find the Dot product between two vector 2ds

  Parameters:
    a: pointer to first vector 2d
    b: pointer to second vector 2d
  
  Returns:
    Dot product between them
*/
double dDot2d( double *a, double *b )
{
  return ( a[0] * b[0] + a[1] * b[1] );
}

/*
  Find the Cross Product between two vector 2ds

  Parameters:
    output: contains the return value of the cross product
    a:      pointer to first vector 2d
    b:      pointer to second vector 2d
  
  Returns:
    void
*/
double dCross2d( double *a, double *b )
{
  return (a[0] * b[1] - a[1] * b[0]);
}

/*
  Normalize a vector 2d

  Parameters:
    vec: pointer to vector 2d
  
  Returns:
    void
*/
void dNormalize2d( double *vec )
{
  double temp;
  temp = sqrt( vec[0] * vec[0] + vec[1] * vec[1] );
  vec[0] /= temp;
  vec[1] /= temp;
}

/*
  Creates a normal vector from two vector 2ds

  Parameters:
    output: contains the normal vector
    a:      pointer to first vector 2d
    b:      pointer to second vector 2d
  
  Returns:
    void
*/
void dNormal2d( double *output, double *a, double *b )
{
  float temp;
  output[0] = a[0] - b[0];
  output[1] = a[1] - b[1];
  temp = dSqrt( output[0] * output[0] + output[1] * output[1] );
  output[0] /= temp;
  output[1] /= temp;
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
void dIntersect2d( double *output, double *lineA0, double *lineA1, double *lineB0, double *lineB1 )
{
  double temp = ( ( lineA0[0] - lineA1[0] ) * ( lineB0[1] - lineB1[1] ) ) - ( ( lineA0[1] - lineA1[1] ) * ( lineB0[0] - lineB1[0] ) );

  output[0] = ( ( ( lineA0[0] * lineA1[1] ) - ( lineA0[1] * lineA1[0] ) ) * ( lineB0[0] - lineB1[0] ) ) - 
  ( ( lineA0[0] - lineA1[0] ) * ( ( lineB0[0] * lineB1[1] ) - ( lineB0[1] * lineB1[0] ) ) );
  
  output[0] /= temp;

  output[1] = ( ( ( lineA0[0] * lineA1[1] ) - ( lineA0[1] * lineA1[0] ) ) * ( lineB0[1] - lineB1[1] ) ) - 
  ( ( lineA0[1] - lineA1[1] ) * ( ( lineB0[0] * lineB1[1] ) - ( lineB0[1] * lineB1[0] ) ) );

  output[1] /= temp;
}

/*---------------- Vector 3 double Math ----------------*/

/*
  Add two vector 3ds together

  Parameter:
    output: pointer to the finished computation
    a:      pointer to first vector 3d
    b:      pointer to second vector 3d
  
  Returns:
    void
*/
void dAddVec3d( double *output, const double *a, const double *b )
{
  output[0] = ( a[0] + b[0] );
  output[1] = ( a[1] + b[1] );
  output[2] = ( a[2] + b[2] );
}

/*
  Subtracts two vector 3ds together

  Parameter:
    output: pointer to the finished computation
    a:      pointer to first vector 3d
    b:      pointer to second vector 3d
  
  Returns:
    void
*/
void dSubVec3d( double *output, const double *a, const double *b )
{
  output[0] = ( a[0] - b[0] );
  output[1] = ( a[1] - b[1] );
  output[2] = ( a[2] - b[2] );
}

/*
  Returns the smaller of two vector 3ds

  Parameter:
    output: pointer to the smaller vector 3d
    a:      pointer to first vector 3d
    b:      pointer to second vector 3d
  
  Returns:
    void
*/
void dMinVec3d( double *output, const double *a, const double *b )
{
  output[0] = ( a[0] < b[0] ? a[0] : b[0] );
  output[1] = ( a[1] < b[1] ? a[1] : b[1] );
  output[2] = ( a[2] < b[2] ? a[2] : b[2] );
}

/*
  Returns the larger of two vector 3ds

  Parameter:
    output: pointer to the larger vector 3d
    a:      pointer to first vector 3d
    b:      pointer to second vector 3d
  
  Returns:
    void
*/
void dMaxVec3d( double *output, const double *a, const double *b )
{
  output[0] = ( a[0] > b[0] ? a[0] : b[0] );
  output[1] = ( a[1] > b[1] ? a[1] : b[1] );
  output[2] = ( a[2] > b[2] ? a[2] : b[2] );
}

/*
  scale a vector 3d to a value

  Parameter:
    output: pointer to the scaled vector 3d
    a:      pointer to vector 3d
    value:  scale factor
  
  Returns:
    void
*/
void dMultiplyVec3d( double *output, const double value )
{
  output[0] *= value;
  output[1] *= value;
  output[2] *= value;
}

/*
  Divide a vector 3d by a value

  Parameter:
    output: pointer to finished computation
    a:      pointer to vector 3d
    value:  divide factor
  
  Returns:
    void
*/
void dDivideVec3d( double *output, const double value )
{
  output[0] /= value;
  output[1] /= value;
  output[2] /= value;
}

/*
  Limit a vector 3d to within a value

  Parameter:
    a:     pointer to first vector 3d
    value: limit factor
  
  Returns:
    void
*/
void dLimitVec3d( double *a, const double value )
{
  if ( dSqrt( ( a[0] * a[0] ) + ( a[1] * a[1] ) + ( a[2] * a[2] ) ) > value )
  {
    float temp; //Normalize vector
    temp = dSqrt( a[0] * a[0] + a[1] * a[1] + a[2] * a[2] );
    a[0] /= temp;
    a[1] /= temp;
    a[2] /= temp;

    a[0] *= value; //scale to value
    a[1] *= value;
    a[2] *= value;
  }
}

/*
  Length of a vector 3d

  Parameter:
    vec: pointer to vector 3d
  
  Returns:
    float of length 
*/
double dLength3d( double *vec )
{
  return sqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
}

/*
  Distance between two vector 3d

  Parameter:
    a:      pointer to first vector 3d
    b:      pointer to second vector 3d
  
  Returns:
    float distance between
*/
double dDistance3d( double *a, double *b )
{
  double output[3];
  output[0] = a[0] - b[0];
  output[1] = a[1] - b[1];
  output[2] = a[2] - b[2];
  return sqrt( output[0] * output[0] + output[1] * output[1] + output[2] * output[2] );
}

/*
  Dot product of two vector 3d

  Parameter:
    a: pointer to first vector 3d
    b: pointer to second vector 3d
  
  Returns:
    returns dot product
*/
double dDot3d( double *a, double *b )
{
  return ( a[0] * b[0] + a[1] * b[1] + a[2] * b[2] );
}

/*
  Cross product of two vector 3d

  Parameter:
    output: pointer to finished computation  
    a:      pointer to first vector 3d
    b:      pointer to second vector 3d
  
  Returns:
    void
*/
void dCross3d( double *output, double *a, double *b )
{
  output[0] = ( a[1] * b[2] - a[2] * b[1] );
  output[1] = ( a[2] * b[0] - a[0] * b[2] );
  output[2] = ( a[0] * b[1] - a[1] * b[0] );
}

/*
  Normalize a vector 3d

  Parameter:
    vec: pointer to vector 3d
  
  Returns:
    void
*/
void dNormalize3d( double *vec )
{
  double temp;
  temp = sqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
  vec[0] /= temp;
  vec[1] /= temp;
  vec[2] /= temp;
}

/*
  Create a normal from three vector 3ds

  Parameter:
    output: pointer to finished computation
    a:      pointer to first vector 3d
    b:      pointer to second vector 3d
    c:      pointer to third vector 3d
  
  Returns:
    void
*/
void dNormal3d( double *output, double *a, double *b, double *c )
{
  double tempA[3], tempB[3], temp;
  tempA[0] = a[0] - c[0];
  tempA[1] = a[1] - c[1];
  tempA[2] = a[2] - c[2];
  tempB[0] = b[0] - c[0];
  tempB[1] = b[1] - c[1];
  tempB[2] = b[2] - c[2];
  output[0] = tempA[1] * tempB[2] + tempA[2] * tempB[1];
  output[1] = tempA[2] * tempB[0] + tempA[0] * tempB[2];
  output[2] = tempA[0] * tempB[1] + tempA[1] * tempB[0];
  temp = sqrt( output[0] * output[0] + output[1] * output[1] + output[2] * output[2] );
  output[0] /= temp;
  output[1] /= temp;
  output[2] /= temp;
}

/*
  Normalize a vector 4d

  Parameter:
    vec: pointer to vector 4d
  
  Returns:
    void
*/
void dNormalize4d( double *vec )
{
  double temp;
  temp = sqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3] );
  vec[0] /= temp;
  vec[1] /= temp;
  vec[2] /= temp;
  vec[3] /= temp;
}


/*---------------- Vector Integer Math ----------------*/

/* long dSqrti( int number )
{

} */

void dIntersect2di( int *output, int *lineA0, int *lineA1,int *lineB0, int *lineB1 )
{
  output[0] = ( ( ( lineA0[0] * lineA1[1] ) - ( lineA0[1] * lineA1[0] ) ) * ( lineB0[0] - lineB1[0] ) ) - 
  ( ( lineA0[0] - lineA1[0] ) * ( ( lineB0[0] * lineB1[1] ) - ( lineB0[1] * lineB1[0] ) ) );
  
  output[0] /= ( ( lineA0[0] - lineA1[0] ) * ( lineB0[1] - lineB1[1] ) ) - ( ( lineA0[1] - lineA1[1] ) * ( lineB0[0] - lineB1[0] ) );

  output[1] = ( ( ( lineA0[0] * lineA1[1] ) - ( lineA0[1] * lineA1[0] ) ) * ( lineB0[1] - lineB1[1] ) ) - 
  ( ( lineA0[1] - lineA1[1] ) * ( ( lineB0[0] * lineB1[1] ) - ( lineB0[1] * lineB1[0] ) ) );

  output[1] /= ( ( lineA0[0] - lineA1[0] ) * ( lineB0[1] - lineB1[1] ) ) - ( ( lineA0[1] - lineA1[1] ) * ( lineB0[0] - lineB1[0] ) );
}

/* unsigned char dNormalize2di( int *point, int multiplyer )
{

}

unsigned char dNormalize3di( int *point, int multiplyer )
{

} */
