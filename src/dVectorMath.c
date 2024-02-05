#include <stdio.h>
#include <stdlib.h>

#include "daedalus.h"

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

float dLength2f( float *vec )
{
  return dSqrt( vec[0] * vec[0] + vec[1] * vec[1] );
}

float dDistance2f( float *a, float *b )
{
  float temp[2] = { 0 };
  temp[0] = a[0] - b[0];
  temp[1] = a[1] - b[1];
  return dSqrt( temp[0] * temp[0] + temp[1] * temp[1] );
}

float dDot2f( float *a, float *b )
{
  return ( a[0] * b[0] + a[1] * b[1] );
}

void dCross2f( float *output, float *a, float *b )
{
  output[0] = a[1];
  output[1] = -a[0];
}

void dNormalize2f( float *vec )
{
  float temp;
  temp = dSqrt( vec[0] * vec[0] + vec[1] * vec[1] );
  vec[0] /= temp;
  vec[1] /= temp;
}

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

}

/*-------------------------------------------------------------*/

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

/* Vector Math Double */
double dLength2d( double *vec )
{

}

double dDistance2d( double *a, double *b )
{

}

double dDot2d( double *a, double *b )
{

}

void dCross2d( double *output, double *a, double *b )
{

}

void dNormalize2d( double *vec )
{

}

void dNormal2d( double *output, double *a, double *b )
{

}

void dIntersect2d( double *output, double *lineA0, double *lineA1, double *lineB0, double *lineB1 )
{

}

/*-------------------------------------------------------------*/

double dLength3d( double *vec )
{

}

double dDistance3d( double *a, double *b )
{

}

double dDot3d( double *a, double *b )
{

}

void dCross3d( double *output, double *a, double *b )
{

}
void dNormalize3d( double *vec )
{

}

void dNormal3d( double *output, double *a, double *b )
{

}

void dNormalize4d( double *vec )
{

}


/* Vector Integer Math */
long dSqrti( int number )
{

}

void dIntersect2di( int *output, int *lineA0, int *lineA1,int *lineB0, int *lineB1 )
{

}

boolean dNormalize2di( int *point, int multiplyer )
{

}

boolean dNormalize3di( int *point, int multiplyer )
{

}
