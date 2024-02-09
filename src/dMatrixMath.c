#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __DAEDALUS_H__
#define PI 3.14159265
#endif

/* Matrix Operations */

/*
  Clears a 4x4 Matrix with to an Identity Matrix

  Parameters:
    matrix: pointer to matrix
  
  Returns:
    void
*/
void dMatrixClearf( float *matrix )
{
  matrix[0]  = 1.0f;
  matrix[1]  = 0.0f;
  matrix[2]  = 0.0f;
  matrix[3]  = 0.0f;
  matrix[4]  = 0.0f;
  matrix[5]  = 1.0f;
  matrix[6]  = 0.0f;
  matrix[7]  = 0.0f;
  matrix[8]  = 0.0f;
  matrix[9]  = 0.0f;
  matrix[10] = 1.0f;
  matrix[11] = 0.0f;
  matrix[12] = 0.0f;
  matrix[13] = 0.0f;
  matrix[14] = 0.0f;
  matrix[15] = 1.0f;
}

/*
  Transform a 3D Point into a 4x4 Matrix

  Parameters:
    output: pointer to trasnformed matrix
    matrix: pointer to matrix
    x:      x coord
    y:      y coord
    z:      z coord
  
  Returns:
    void
*/
void dMatrixTransform3f( float *output, const float *matrix, const float x, const float y, const float z )
{
  output[0] = ( matrix[0] * x ) + ( matrix[4] * y ) + ( matrix[8]  * z ) + matrix[12];
  output[1] = ( matrix[1] * x ) + ( matrix[5] * y ) + ( matrix[9]  * z ) + matrix[13];
  output[2] = ( matrix[2] * x ) + ( matrix[6] * y ) + ( matrix[10] * z ) + matrix[14];
}

/*
  Computes the Inverse Transformation of a 3D Point into a 4x4 Matrix

  Parameters:
    output: pointer to inverse of trasnformed matrix
    matrix: pointer to matrix
    x:      x coord
    y:      y coord
    z:      z coord
  
  Returns:
    void
*/
void dMatrixInverseTransform3f( float *output, const float *matrix, float x, float y, float z )
{
  x -= matrix[12];
  y -= matrix[13];
  z -= matrix[14];
  output[0] = ( matrix[0] * x ) + ( matrix[1] * y ) + ( matrix[2]  * z );
  output[1] = ( matrix[4] * x ) + ( matrix[5] * y ) + ( matrix[6]  * z );
  output[2] = ( matrix[8] * x ) + ( matrix[9] * y ) + ( matrix[10] * z );
}

/*
  Transform a 4D Point to a 4x4 Matrix

  Parameters:
    output: pointer to inverse of trasnformed matrix
    matrix: pointer to matrix
    x:      x coord
    y:      y coord
    z:      z coord
    w:      w coord
  
  Returns:
    void
*/
void dMatrixTransform4f( float *output, const float *matrix, const float x, const float y, const float z, const float w )
{
  output[0] = ( matrix[0] * x ) + ( matrix[4] * y ) + ( matrix[8]  * z ) + ( matrix[12] * w );
  output[1] = ( matrix[1] * x ) + ( matrix[5] * y ) + ( matrix[9]  * z ) + ( matrix[13] * w );
  output[2] = ( matrix[2] * x ) + ( matrix[6] * y ) + ( matrix[10] * z ) + ( matrix[14] * w );
  output[3] = ( matrix[3] * x ) + ( matrix[7] * y ) + ( matrix[11] * z ) + ( matrix[15] * w );
}

/*
  Multiply Two Matrices Together

  Parameters:
    output: pointer to finished computation
    a:      pointer to first matrix
    b:      pointer to second matrix
  
  Returns:
    void
*/
void dMatrixMultiplyf( float *output, const float *a, const float *b )
{
  output[0]  = ( b[0]  * a[0] ) + ( b[1]  * a[4] ) + ( b[2]  * a[8] ) + ( b[3]  * a[12] );
  output[1]  = ( b[4]  * a[0] ) + ( b[5]  * a[4] ) + ( b[6]  * a[8] ) + ( b[7]  * a[12] );
  output[2]  = ( b[8]  * a[0] ) + ( b[9]  * a[4] ) + ( b[10] * a[8] ) + ( b[11] * a[12] );
  output[3]  = ( b[12] * a[0] ) + ( b[13] * a[4] ) + ( b[14] * a[8] ) + ( b[15] * a[12] );

  output[4]  = ( b[0]  * a[1] ) + ( b[1]  * a[5] ) + ( b[2]  * a[9] ) + ( b[3]  * a[13] );
  output[5]  = ( b[4]  * a[1] ) + ( b[5]  * a[5] ) + ( b[6]  * a[9] ) + ( b[7]  * a[13] );
  output[6]  = ( b[8]  * a[1] ) + ( b[9]  * a[5] ) + ( b[10] * a[9] ) + ( b[11] * a[13] );
  output[7]  = ( b[12] * a[1] ) + ( b[13] * a[5] ) + ( b[14] * a[9] ) + ( b[15] * a[13] );

  output[8]  = ( b[0]  * a[2] ) + ( b[1]  * a[6] ) + ( b[2]  * a[10] ) + ( b[3]  * a[14] );
  output[9]  = ( b[4]  * a[2] ) + ( b[5]  * a[6] ) + ( b[6]  * a[10] ) + ( b[7]  * a[14] );
  output[10] = ( b[8]  * a[2] ) + ( b[9]  * a[6] ) + ( b[10] * a[10] ) + ( b[11] * a[14] );
  output[11] = ( b[12] * a[2] ) + ( b[13] * a[6] ) + ( b[14] * a[10] ) + ( b[15] * a[14] );

  output[12] = ( b[0]  * a[3] ) + ( b[1]  * a[7] ) + ( b[2]  * a[11] ) + ( b[3]  * a[15] );
  output[13] = ( b[4]  * a[3] ) + ( b[5]  * a[7] ) + ( b[6]  * a[11] ) + ( b[7]  * a[15] );
  output[14] = ( b[8]  * a[3] ) + ( b[9]  * a[7] ) + ( b[10] * a[11] ) + ( b[11] * a[15] );
  output[15] = ( b[12] * a[3] ) + ( b[13] * a[7] ) + ( b[14] * a[11] ) + ( b[15] * a[15] );
}

void dMatrixInversef( float *output, const float *matrix )
{

}

/*
  Rotate a Matrix about the X Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void dMatrixRotateXf( float *matrix, const float angleRad )
{
  matrix[0]  = 1.0f;
  matrix[1]  = 0.0f;
  matrix[2]  = 0.0f;
  matrix[3]  = 0.0f;
  
  matrix[4]  = 0.0f;
  matrix[5]  = cosf( angleRad );
  matrix[6]  = sinf( angleRad );
  matrix[7]  = 0.0f;

  matrix[8]  = 0.0f;
  matrix[9]  = -sinf( angleRad );
  matrix[10] = cosf( angleRad );
  matrix[11] = 0.0f;

  matrix[12] = 0.0f;
  matrix[13] = 0.0f;
  matrix[14] = 0.0f;
  matrix[15] = 1.0f;
}

/*
  Rotate a Matrix about the Y Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void dMatrixRotateYf( float *matrix, const float angleRad )
{
  matrix[0]  = cosf( angleRad );
  matrix[1]  = 0.0f;
  matrix[2]  = sinf( angleRad );
  matrix[3]  = 0.0f;
  
  matrix[4]  = 0.0f;
  matrix[5]  = 1.0f;
  matrix[6]  = 0.0f;
  matrix[7]  = 0.0f;

  matrix[8]  = -sinf( angleRad );
  matrix[9]  = 0.0f;
  matrix[10] = cosf( angleRad );
  matrix[11] = 0.0f;

  matrix[12] = 0.0f;
  matrix[13] = 0.0f;
  matrix[14] = 0.0f;
  matrix[15] = 1.0f;
}

/*
  Rotate a Matrix about the Z Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void dMatrixRotateZf( float *matrix, const float angleRad )
{
  matrix[0]  = cosf( angleRad );
  matrix[1]  = sinf( angleRad );
  matrix[2]  = 0.0f;
  matrix[3]  = 0.0f;
  
  matrix[4]  = -sinf( angleRad );
  matrix[5]  = cosf( angleRad );
  matrix[6]  = 0.0f;
  matrix[7]  = 0.0f;

  matrix[8]  = 0.0f;
  matrix[9]  = 0.0f;
  matrix[10] = 1.0f;
  matrix[11] = 0.0f;

  matrix[12] = 0.0f;
  matrix[13] = 0.0f;
  matrix[14] = 0.0f;
  matrix[15] = 1.0f;
}

/*
  Translate a Matrix by a 3D Point

  Parameters:
    matrix: pointer to matrix
    x:      x coord
    y:      y coord
    z:      z coord  
  
  Returns:
    void
*/
void dMatrixTranslation3f( float *matrix, const float x, const float y, const float z )
{
  matrix[0]  = 1.0f;
  matrix[1]  = 0.0f;
  matrix[2]  = 0.0f;
  matrix[3]  = 0.0f;
  
  matrix[4]  = 0.0f;
  matrix[5]  = 1.0f;
  matrix[6]  = 0.0f;
  matrix[7]  = 0.0f;

  matrix[8]  = 0.0f;
  matrix[9]  = 0.0f;
  matrix[10] = 1.0f;
  matrix[11] = 0.0f;

  matrix[12] = x;
  matrix[13] = y;
  matrix[14] = z;
  matrix[15] = 1.0f;
}

/*
  Create a 4x4 Projection Matrix

  Parameters:
    matrix: pointer to matrix
    aspectRatio:
    fov:
    far:
    near:
  
  Returns:
    void
*/
void dMatrixProjectionf( float *matrix, const float aspectRatio, const float fov, const float far, const float near )
{
  float fovRad = 1.0f / tanf( fov * 0.5f / 180.0f * PI );
  matrix[0]  = aspectRatio * fovRad;
  matrix[1]  = 0.0f;
  matrix[2]  = 0.0f;
  matrix[3]  = 0.0f;
  
  matrix[4]  = 0.0f;
  matrix[5]  = fovRad;
  matrix[6]  = 0.0f;
  matrix[7]  = 0.0f;
  
  matrix[8]  = 0.0f;
  matrix[9]  = 0.0f;
  matrix[10] = far / far - near;
  matrix[11] = 0.0f;
  
  matrix[12] = 0.0f;
  matrix[13] = 0.0f;
  matrix[14] = ( -far * near ) / far - near;
  matrix[15] = 0.0f;
}

/*---------------- Matrix Double Math ----------------*/

/*
  Clears a 4x4 Matrix with to an Identity Matrix

  Parameters:
    matrix: pointer to matrix
  
  Returns:
    void
*/
void dMatrixCleard( double *matrix )
{
  matrix[0]  = 1.0;
  matrix[1]  = 0.0;
  matrix[2]  = 0.0;
  matrix[3]  = 0.0;
  matrix[4]  = 0.0;
  matrix[5]  = 1.0;
  matrix[6]  = 0.0;
  matrix[7]  = 0.0;
  matrix[8]  = 0.0;
  matrix[9]  = 0.0;
  matrix[10] = 1.0;
  matrix[11] = 0.0;
  matrix[12] = 0.0;
  matrix[13] = 0.0;
  matrix[14] = 0.0;
  matrix[15] = 1.0;
}

/*
  Transform a 3D Point into a 4x4 Matrix

  Parameters:
    output: pointer to trasnformed matrix
    matrix: pointer to matrix
    x:      x coord
    y:      y coord
    z:      z coord
  
  Returns:
    void
*/
void dMatrixTransform3d( double *output, const double *matrix, const double x, const double y, const double z )
{
  output[0] = ( matrix[0] * x ) + ( matrix[4] * y ) + ( matrix[8]  * z ) + matrix[12];
  output[1] = ( matrix[1] * x ) + ( matrix[5] * y ) + ( matrix[9]  * z ) + matrix[13];
  output[2] = ( matrix[2] * x ) + ( matrix[6] * y ) + ( matrix[10] * z ) + matrix[14];
}

/*
  Computes the Inverse Transformation of a 3D Point into a 4x4 Matrix

  Parameters:
    output: pointer to inverse of trasnformed matrix
    matrix: pointer to matrix
    x:      x coord
    y:      y coord
    z:      z coord
  
  Returns:
    void
*/
void dMatrixInverseTransform3d( double *output, const double *matrix, double x, double y, double z )
{
  x -= matrix[12];
  y -= matrix[13];
  z -= matrix[14];
  output[0] = ( matrix[0] * x ) + ( matrix[1] * y ) + ( matrix[2]  * z );
  output[1] = ( matrix[4] * x ) + ( matrix[5] * y ) + ( matrix[6]  * z );
  output[2] = ( matrix[8] * x ) + ( matrix[9] * y ) + ( matrix[10] * z );
}

/*
  Transform a 4D Point to a 4x4 Matrix

  Parameters:
    output: pointer to inverse of trasnformed matrix
    matrix: pointer to matrix
    x:      x coord
    y:      y coord
    z:      z coord
    w:      w coord
  
  Returns:
    void
*/
void dMatrixTransform4d( double *output, const double *matrix, const double x, const double y, const double z, const double w )
{
  output[0] = ( matrix[0] * x ) + ( matrix[4] * y ) + ( matrix[8]  * z ) + ( matrix[12] * w );
  output[1] = ( matrix[1] * x ) + ( matrix[5] * y ) + ( matrix[9]  * z ) + ( matrix[13] * w );
  output[2] = ( matrix[2] * x ) + ( matrix[6] * y ) + ( matrix[10] * z ) + ( matrix[14] * w );
  output[3] = ( matrix[3] * x ) + ( matrix[7] * y ) + ( matrix[11] * z ) + ( matrix[15] * w );
}

/*
  Multiply Two Matrices Together

  Parameters:
    output: pointer to finished computation
    a:      pointer to first matrix
    b:      pointer to second matrix
  
  Returns:
    void
*/
void dMatrixMultiplyd( double *output, const double *a, const double *b )
{
  output[0]  = ( b[0]  * a[0] ) + ( b[1]  * a[4] ) + ( b[2]  * a[8] ) + ( b[3]  * a[12] );
  output[1]  = ( b[4]  * a[0] ) + ( b[5]  * a[4] ) + ( b[6]  * a[8] ) + ( b[7]  * a[12] );
  output[2]  = ( b[8]  * a[0] ) + ( b[9]  * a[4] ) + ( b[10] * a[8] ) + ( b[11] * a[12] );
  output[3]  = ( b[12] * a[0] ) + ( b[13] * a[4] ) + ( b[14] * a[8] ) + ( b[15] * a[12] );

  output[4]  = ( b[0]  * a[1] ) + ( b[1]  * a[5] ) + ( b[2]  * a[9] ) + ( b[3]  * a[13] );
  output[5]  = ( b[4]  * a[1] ) + ( b[5]  * a[5] ) + ( b[6]  * a[9] ) + ( b[7]  * a[13] );
  output[6]  = ( b[8]  * a[1] ) + ( b[9]  * a[5] ) + ( b[10] * a[9] ) + ( b[11] * a[13] );
  output[7]  = ( b[12] * a[1] ) + ( b[13] * a[5] ) + ( b[14] * a[9] ) + ( b[15] * a[13] );

  output[8]  = ( b[0]  * a[2] ) + ( b[1]  * a[6] ) + ( b[2]  * a[10] ) + ( b[3]  * a[14] );
  output[9]  = ( b[4]  * a[2] ) + ( b[5]  * a[6] ) + ( b[6]  * a[10] ) + ( b[7]  * a[14] );
  output[10] = ( b[8]  * a[2] ) + ( b[9]  * a[6] ) + ( b[10] * a[10] ) + ( b[11] * a[14] );
  output[11] = ( b[12] * a[2] ) + ( b[13] * a[6] ) + ( b[14] * a[10] ) + ( b[15] * a[14] );

  output[12] = ( b[0]  * a[3] ) + ( b[1]  * a[7] ) + ( b[2]  * a[11] ) + ( b[3]  * a[15] );
  output[13] = ( b[4]  * a[3] ) + ( b[5]  * a[7] ) + ( b[6]  * a[11] ) + ( b[7]  * a[15] );
  output[14] = ( b[8]  * a[3] ) + ( b[9]  * a[7] ) + ( b[10] * a[11] ) + ( b[11] * a[15] );
  output[15] = ( b[12] * a[3] ) + ( b[13] * a[7] ) + ( b[14] * a[11] ) + ( b[15] * a[15] );
}

void dMatrixInversed( double* output, const double *matrix )
{

}

/*
  Rotate a Matrix about the X Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void dMatrixRotateXd( double *matrix, const double angleRad )
{
  matrix[0]  = 1;
  matrix[1]  = 0;
  matrix[2]  = 0;
  matrix[3]  = 0;
  
  matrix[4]  = 0;
  matrix[5]  = cos( angleRad );
  matrix[6]  = sin( angleRad );
  matrix[7]  = 0;

  matrix[8]  = 0;
  matrix[9]  = sin( angleRad );
  matrix[10] = cos( angleRad );
  matrix[11] = 0;

  matrix[12] = 0;
  matrix[13] = 0;
  matrix[14] = 0;
  matrix[15] = 1;
}

/*
  Rotate a Matrix about the Y Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void dMatrixRotateYd( double *matrix, const double angleRad )
{
  matrix[0]  = cos( angleRad );
  matrix[1]  = 0;
  matrix[2]  = sin( angleRad );
  matrix[3]  = 0;
  
  matrix[4]  = 0;
  matrix[5]  = 1;
  matrix[6]  = 0;
  matrix[7]  = 0;

  matrix[8]  = -sin( angleRad );
  matrix[9]  = 0;
  matrix[10] = cos( angleRad );
  matrix[11] = 0;

  matrix[12] = 0;
  matrix[13] = 0;
  matrix[14] = 0;
  matrix[15] = 1;
}

/*
  Rotate a Matrix about the Z Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void dMatrixRotateZd( double *matrix, const double angleRad )
{
  matrix[0]  = cos( angleRad );
  matrix[1]  = sin( angleRad );
  matrix[2]  = 0;
  matrix[3]  = 0;
  
  matrix[4]  = -sin( angleRad );
  matrix[5]  = cos( angleRad );
  matrix[6]  = 0;
  matrix[7]  = 0;

  matrix[8]  = 0;
  matrix[9]  = 0;
  matrix[10] = 1;
  matrix[11] = 0;

  matrix[12] = 0;
  matrix[13] = 0;
  matrix[14] = 0;
  matrix[15] = 1;
}

/*
  Translate a Matrix by a 3D Point

  Parameters:
    matrix: pointer to matrix
    x:      x coord
    y:      y coord
    z:      z coord  
  
  Returns:
    void
*/
void dMatrixTranslation3d( double *matrix, const double x, const double y, const double z )
{
  matrix[0]  = 1;
  matrix[1]  = 0;
  matrix[2]  = 0;
  matrix[3]  = 0;
  
  matrix[4]  = 0;
  matrix[5]  = 1;
  matrix[6]  = 0;
  matrix[7]  = 0;

  matrix[8]  = 0;
  matrix[9]  = 0;
  matrix[10] = 1;
  matrix[11] = 0;

  matrix[12] = x;
  matrix[13] = y;
  matrix[14] = z;
  matrix[15] = 1;
}

/*
  Create a 4x4 Projection Matrix

  Parameters:
    matrix: pointer to matrix
    aspectRatio:
    fov:
    far:
    near:
  
  Returns:
    void
*/
void dMatrixProjectionf( double *matrix, const double aspectRatio, const double fov, const double far, const double near )
{
  double fovRad = 1 / tan( fov * 0.5 / 180 * PI );
  matrix[0]  = aspectRatio * fovRad;
  matrix[1]  = 0;
  matrix[2]  = 0;
  matrix[3]  = 0;
  
  matrix[4]  = 0;
  matrix[5]  = fovRad;
  matrix[6]  = 0;
  matrix[7]  = 0;
  
  matrix[8]  = 0;
  matrix[9]  = 0;
  matrix[10] = far / far - near;
  matrix[11] = 0;
  
  matrix[12] = 0;
  matrix[13] = 0;
  matrix[14] = ( -far * near ) / far - near;
  matrix[15] = 0;
}
