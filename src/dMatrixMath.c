#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Daedalus.h"

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
void dMatrixTransform3f( float *output, const float *matrix, const float *vec )
{
  output[0] = ( matrix[0] * vec[0] ) + ( matrix[4] * vec[1] ) + ( matrix[8]  * vec[2] ) + matrix[12];
  output[1] = ( matrix[1] * vec[0] ) + ( matrix[5] * vec[1] ) + ( matrix[9]  * vec[2] ) + matrix[13];
  output[2] = ( matrix[2] * vec[0] ) + ( matrix[6] * vec[1] ) + ( matrix[10] * vec[2] ) + matrix[14];
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
void dMatrixInverseTransform3f( float *output, const float *matrix, float *vec )
{
  vec[0] -= matrix[12];
  vec[1] -= matrix[13];
  vec[2] -= matrix[14];
  output[0] = ( matrix[0] * vec[0] ) + ( matrix[1] * vec[1] ) + ( matrix[2]  * vec[2] );
  output[1] = ( matrix[4] * vec[0] ) + ( matrix[5] * vec[1] ) + ( matrix[6]  * vec[2] );
  output[2] = ( matrix[8] * vec[0] ) + ( matrix[9] * vec[1] ) + ( matrix[10] * vec[2] );
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
void dMatrixTransform4f( float *output, const float *matrix, const float *vec4 )
{
  output[0] = ( matrix[0] * vec4[0] ) + ( matrix[4] * vec4[1] ) + ( matrix[8]  * vec4[2] ) + ( matrix[12] * vec4[3] );
  output[1] = ( matrix[1] * vec4[0] ) + ( matrix[5] * vec4[1] ) + ( matrix[9]  * vec4[2] ) + ( matrix[13] * vec4[3] );
  output[2] = ( matrix[2] * vec4[0] ) + ( matrix[6] * vec4[1] ) + ( matrix[10] * vec4[2] ) + ( matrix[14] * vec4[3] );
  output[3] = ( matrix[3] * vec4[0] ) + ( matrix[7] * vec4[1] ) + ( matrix[11] * vec4[2] ) + ( matrix[15] * vec4[3] );
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

/* void dMatrixInversef( float *output, const float *matrix )
{

} */

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
void dMatrixTranslation3f( float *matrix, const float *vec )
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

  matrix[12] = vec[0];
  matrix[13] = vec[1];
  matrix[14] = vec[2];
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
void dMatrixProjectionf( float *matrix, const float aspectRatio, const float fov, const float near, const float far )
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
void dMatrixTransform3d( double *output, const double *matrix, const double *vec )
{
  output[0] = ( matrix[0] * vec[0] ) + ( matrix[4] * vec[1] ) + ( matrix[8]  * vec[2] ) + matrix[12];
  output[1] = ( matrix[1] * vec[0] ) + ( matrix[5] * vec[1] ) + ( matrix[9]  * vec[2] ) + matrix[13];
  output[2] = ( matrix[2] * vec[0] ) + ( matrix[6] * vec[1] ) + ( matrix[10] * vec[2] ) + matrix[14];
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
void dMatrixInverseTransform3d( double *output, const double *matrix, double *vec )
{
  vec[0] -= matrix[12];
  vec[1] -= matrix[13];
  vec[2] -= matrix[14];
  output[0] = ( matrix[0] * vec[0] ) + ( matrix[1] * vec[1] ) + ( matrix[2]  * vec[2] );
  output[1] = ( matrix[4] * vec[0] ) + ( matrix[5] * vec[1] ) + ( matrix[6]  * vec[2] );
  output[2] = ( matrix[8] * vec[0] ) + ( matrix[9] * vec[1] ) + ( matrix[10] * vec[2] );
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
void dMatrixTransform4d( double *output, const double *matrix, const double *vec4 )
{
  output[0] = ( matrix[0] * vec4[0] ) + ( matrix[4] * vec4[1] ) + ( matrix[8]  * vec4[2] ) + ( matrix[12] * vec4[3] );
  output[1] = ( matrix[1] * vec4[0] ) + ( matrix[5] * vec4[1] ) + ( matrix[9]  * vec4[2] ) + ( matrix[13] * vec4[3] );
  output[2] = ( matrix[2] * vec4[0] ) + ( matrix[6] * vec4[1] ) + ( matrix[10] * vec4[2] ) + ( matrix[14] * vec4[3] );
  output[3] = ( matrix[3] * vec4[0] ) + ( matrix[7] * vec4[1] ) + ( matrix[11] * vec4[2] ) + ( matrix[15] * vec4[3] );
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

/* void dMatrixInversed( double* output, const double *matrix )
{

} */

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
void dMatrixTranslation3d( double *matrix, const double *vec )
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

  matrix[12] = vec[0];
  matrix[13] = vec[1];
  matrix[14] = vec[2];
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
void dMatrixProjectiond( double *matrix, const double aspectRatio, const double fov, const double near, const double far )
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

/* ----------------------------Shit-------------------------------- */

void dMatrixTransformVec3f( dVec4 output, const float *matrix, const dVec4 vec )
{
  output.x = ( matrix[0] * vec.x ) + ( matrix[4] * vec.y ) + ( matrix[8]  * vec.z ) + matrix[12];
  output.y = ( matrix[1] * vec.x ) + ( matrix[5] * vec.y ) + ( matrix[9]  * vec.z ) + matrix[13];
  output.z = ( matrix[2] * vec.x ) + ( matrix[6] * vec.y ) + ( matrix[10] * vec.z ) + matrix[14];
}

void dMatrixTranslateVec4f( float *matrix, const dVec4 vec )
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

  matrix[12] = vec.x;
  matrix[13] = vec.y;
  matrix[14] = vec.z;
  matrix[15] = vec.w;
}

void dMatrixInverseTransformVec3f( dVec4 output, const float *matrix, dVec4 vec )
{
  vec.x -= matrix[12];
  vec.y -= matrix[13];
  vec.z -= matrix[14];
  output.x = ( matrix[0] * vec.x ) + ( matrix[1] * vec.y ) + ( matrix[2]  * vec.z );
  output.y = ( matrix[4] * vec.x ) + ( matrix[5] * vec.y ) + ( matrix[6]  * vec.z );
  output.z = ( matrix[8] * vec.x ) + ( matrix[9] * vec.y ) + ( matrix[10] * vec.z );
}

void dMatrixTransformVec4f( dVec4 output, const float *matrix, const dVec4 vec4 )
{
  output.x = ( matrix[0] * vec4.x ) + ( matrix[4] * vec4.y ) + ( matrix[8]  * vec4.z ) + ( matrix[12] * vec4.w );
  output.y = ( matrix[1] * vec4.x ) + ( matrix[5] * vec4.y ) + ( matrix[9]  * vec4.z ) + ( matrix[13] * vec4.w );
  output.z = ( matrix[2] * vec4.x ) + ( matrix[6] * vec4.y ) + ( matrix[10] * vec4.z ) + ( matrix[14] * vec4.w );
  output.w = ( matrix[3] * vec4.x ) + ( matrix[7] * vec4.y ) + ( matrix[11] * vec4.z ) + ( matrix[15] * vec4.w );
}
