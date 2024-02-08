#include <stdio.h>
#include <stdlib.h>
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

void dAddVec2f( float *output, const float *a, const float *b )
{

}

void dSubVec2f( float *output, const float *a, const float *b )
{

}

void dMinVec2f( float *output, const float *a, const float *b )
{

}

void dMaxVec2f( float *output, const float *a, const float *b )
{

}

void dMultiplyVec2f( float *output, const float *a, const float value )
{

}

void dDivideVec2f( float *output, const float *a, const float value )
{

}

void dLimitVec2f( float *a, const float value )
{

}

/*
  Find the length of a vector 2

  Parameters:
    vec: pointer to vec 2
  
  Returns:
    Length of vector
*/
float dLength2f( float *vec )
{
  return dSqrt( vec[0] * vec[0] + vec[1] * vec[1] );
}

/*
  Find the distance between two vector 2s

  Parameter:
    a: pointer to first vector 2
    b: pointer to second vector 2
  
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
  Find the Dot product between two vector 2s

  Parameters:
    a: pointer to first vector 2
    b: pointer to second vector 2
  
  Returns:
    Dot product between them
*/
float dDot2f( float *a, float *b )
{
  return ( a[0] * b[0] + a[1] * b[1] );
}

/*
  Find the Cross Product between two vector 2s

  Parameters:
    output: contains the return value of the cross product
    a:      pointer to first vector 2
    b:      pointer to second vector 2
  
  Returns:
    void
*/
void dCross2f( float *output, float *a, float *b )
{
  output[0] = a[1];
  output[1] = -a[0];
}

/*
  Normalize a vector 2

  Parameters:
    vec: pointer to vector 2
  
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
  Creates a normal vector from two vector 2s

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

void dAddVec3f( float *output, const float *a, const float *b )
{

}

void dSubVec3f( float *output, const float *a, const float *b )
{

}

void dMinVec3f( float *output, const float *a, const float *b )
{

}

void dMaxVec3f( float *output, const float *a, const float *b )
{

}

void dMultiplyVec3f( float *output, const float *a, const float value )
{

}

void dDivideVec3f( float *output, const float *a, const float value )
{

}

void dLimitVec3f( float *a, const float value )
{

}

float dLength3f( float *vec )
{
  return dSqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
}

float dDistance3f( float *a, float *b )
{
  float output[3];
  output[0] = a[0] - b[0];
  output[1] = a[1] - b[1];
  output[2] = a[2] - b[2];
  return dSqrt( output[0] * output[0] + output[1] * output[1] + output[2] * output[2] );
}

float dDot3f( float *a, float *b )
{
  return ( a[0] * b[0] + a[1] * b[1] + a[2] * b[2] );
}

void dCross3f( float *output, float *a, float *b )
{
  output[0] = ( a[1] * b[2] - a[2] * b[1] );
  output[1] = ( a[2] * b[0] - a[0] * b[2] );
  output[2] = ( a[0] * b[1] - a[1] * b[0] );
}

void dNormalize3f( float *vec )
{
  float temp;
  temp = dSqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
  vec[0] /= temp;
  vec[1] /= temp;
  vec[2] /= temp;
}

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

void dAddVec2d( double *output, const double *a, const double *b )
{

}

void dSubVec2d( double *output, const double *a, const double *b )
{

}

void dMinVec2d( double *output, const double *a, const double *b )
{

}

void dMaxVec2d( double *output, const double *a, const double *b )
{

}

void dMultiplyVec2d( double *output, const double *a, const double value )
{

}

void dDivideVec2d( double *output, const double *a, const double value )
{

}

void dLimitVec2d( double *a, const double value )
{

}

double dLength2d( double *vec )
{
  return sqrt( vec[0] * vec[0] + vec[1] * vec[1] );
}

double dDistance2d( double *a, double *b )
{
  double temp[2] = { 0 };
  temp[0] = a[0] - b[0];
  temp[1] = a[1] - b[1];
  return sqrt( temp[0] * temp[0] + temp[1] * temp[1] );
}

double dDot2d( double *a, double *b )
{
  return ( a[0] * b[0] + a[1] * b[1] );
}

void dCross2d( double *output, double *a, double *b )
{
  output[0] = a[1];
  output[1] = -a[0];
}

void dNormalize2d( double *vec )
{
  double temp;
  temp = sqrt( vec[0] * vec[0] + vec[1] * vec[1] );
  vec[0] /= temp;
  vec[1] /= temp;
}

void dNormal2d( double *output, double *a, double *b )
{
  float temp;
  output[0] = a[0] - b[0];
  output[1] = a[1] - b[1];
  temp = dSqrt( output[0] * output[0] + output[1] * output[1] );
  output[0] /= temp;
  output[1] /= temp;
}

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

void dAddVec3d( double *output, const double *a, const double *b )
{

}

void dSubVec3d( double *output, const double *a, const double *b )
{

}

void dMinVec3d( double *output, const double *a, const double *b )
{

}

void dMaxVec3d( double *output, const double *a, const double *b )
{

}

void dMultiplyVec3d( double *output, const double *a, const double value )
{

}

void dDivideVec3d( double *output, const double *a, const double value )
{

}

void dLimitVec3d( double *a, const double value )
{

}

double dLength3d( double *vec )
{
  return sqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
}

double dDistance3d( double *a, double *b )
{
  double output[3];
  output[0] = a[0] - b[0];
  output[1] = a[1] - b[1];
  output[2] = a[2] - b[2];
  return sqrt( output[0] * output[0] + output[1] * output[1] + output[2] * output[2] );
}

double dDot3d( double *a, double *b )
{
  return ( a[0] * b[0] + a[1] * b[1] + a[2] * b[2] );
}

void dCross3d( double *output, double *a, double *b )
{
  output[0] = ( a[1] * b[2] - a[2] * b[1] );
  output[1] = ( a[2] * b[0] - a[0] * b[2] );
  output[2] = ( a[0] * b[1] - a[1] * b[0] );
}
void dNormalize3d( double *vec )
{
  double temp;
  temp = sqrt( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
  vec[0] /= temp;
  vec[1] /= temp;
  vec[2] /= temp;
}

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

long dSqrti( int number )
{

}

void dIntersect2di( int *output, int *lineA0, int *lineA1,int *lineB0, int *lineB1 )
{
  output[0] = ( ( ( lineA0[0] * lineA1[1] ) - ( lineA0[1] * lineA1[0] ) ) * ( lineB0[0] - lineB1[0] ) ) - 
  ( ( lineA0[0] - lineA1[0] ) * ( ( lineB0[0] * lineB1[1] ) - ( lineB0[1] * lineB1[0] ) ) );
  
  output[0] /= ( ( lineA0[0] - lineA1[0] ) * ( lineB0[1] - lineB1[1] ) ) - ( ( lineA0[1] - lineA1[1] ) * ( lineB0[0] - lineB1[0] ) );

  output[1] = ( ( ( lineA0[0] * lineA1[1] ) - ( lineA0[1] * lineA1[0] ) ) * ( lineB0[1] - lineB1[1] ) ) - 
  ( ( lineA0[1] - lineA1[1] ) * ( ( lineB0[0] * lineB1[1] ) - ( lineB0[1] * lineB1[0] ) ) );

  output[1] /= ( ( lineA0[0] - lineA1[0] ) * ( lineB0[1] - lineB1[1] ) ) - ( ( lineA0[1] - lineA1[1] ) * ( lineB0[0] - lineB1[0] ) );
}

unsigned char dNormalize2di( int *point, int multiplyer )
{

}

unsigned char dNormalize3di( int *point, int multiplyer )
{

}
