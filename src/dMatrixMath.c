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
void d_matrix_clearf( dMat4x4_t *matrix )
{
  matrix->matrix[0]  = 1.0f;
  matrix->matrix[1]  = 0.0f;
  matrix->matrix[2]  = 0.0f;
  matrix->matrix[3]  = 0.0f;

  matrix->matrix[4]  = 0.0f;
  matrix->matrix[5]  = 1.0f;
  matrix->matrix[6]  = 0.0f;
  matrix->matrix[7]  = 0.0f;

  matrix->matrix[8]  = 0.0f;
  matrix->matrix[9]  = 0.0f;
  matrix->matrix[10] = 1.0f;
  matrix->matrix[11] = 0.0f;
  
  matrix->matrix[12] = 0.0f;
  matrix->matrix[13] = 0.0f;
  matrix->matrix[14] = 0.0f;
  matrix->matrix[15] = 1.0f;
}

/*
  Rotate a Matrix about the X Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void d_matrix_rotate_xf( dMat4x4_t *matrix, const float angleRad )
{
  matrix->matrix[0]  = 1.0f;
  matrix->matrix[1]  = 0.0f;
  matrix->matrix[2]  = 0.0f;
  matrix->matrix[3]  = 0.0f;
  
  matrix->matrix[4]  = 0.0f;
  matrix->matrix[5]  = cosf( angleRad );
  matrix->matrix[6]  = sinf( angleRad );
  matrix->matrix[7]  = 0.0f;

  matrix->matrix[8]  = 0.0f;
  matrix->matrix[9]  = -sinf( angleRad );
  matrix->matrix[10] = cosf( angleRad );
  matrix->matrix[11] = 0.0f;

  matrix->matrix[12] = 0.0f;
  matrix->matrix[13] = 0.0f;
  matrix->matrix[14] = 0.0f;
  matrix->matrix[15] = 1.0f;
}

/*
  Rotate a Matrix about the Y Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void d_matrix_rotate_yf( dMat4x4_t *matrix, const float angleRad )
{
  matrix->matrix[0]  = cosf( angleRad );
  matrix->matrix[1]  = 0.0f;
  matrix->matrix[2]  = sinf( angleRad );
  matrix->matrix[3]  = 0.0f;
  
  matrix->matrix[4]  = 0.0f;
  matrix->matrix[5]  = 1.0f;
  matrix->matrix[6]  = 0.0f;
  matrix->matrix[7]  = 0.0f;

  matrix->matrix[8]  = -sinf( angleRad );
  matrix->matrix[9]  = 0.0f;
  matrix->matrix[10] = cosf( angleRad );
  matrix->matrix[11] = 0.0f;

  matrix->matrix[12] = 0.0f;
  matrix->matrix[13] = 0.0f;
  matrix->matrix[14] = 0.0f;
  matrix->matrix[15] = 1.0f;
}

/*
  Rotate a Matrix about the Z Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void d_matrix_rotate_zf( dMat4x4_t *matrix, const float angleRad )
{
  matrix->matrix[0]  = cosf( angleRad );
  matrix->matrix[1]  = sinf( angleRad );
  matrix->matrix[2]  = 0.0f;
  matrix->matrix[3]  = 0.0f;
  
  matrix->matrix[4]  = -sinf( angleRad );
  matrix->matrix[5]  = cosf( angleRad );
  matrix->matrix[6]  = 0.0f;
  matrix->matrix[7]  = 0.0f;

  matrix->matrix[8]  = 0.0f;
  matrix->matrix[9]  = 0.0f;
  matrix->matrix[10] = 1.0f;
  matrix->matrix[11] = 0.0f;

  matrix->matrix[12] = 0.0f;
  matrix->matrix[13] = 0.0f;
  matrix->matrix[14] = 0.0f;
  matrix->matrix[15] = 1.0f;
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
void d_matrix_create_projectionf( dMat4x4_t *matrix, const float aspectRatio, const float fov, const float near, const float far )
{
  float fovRad = 1.0f / tanf( fov * 0.5f / 180.0f * PI );
  matrix->matrix[0]  = aspectRatio * fovRad;
  matrix->matrix[1]  = 0.0f;
  matrix->matrix[2]  = 0.0f;
  matrix->matrix[3]  = 0.0f;
  
  matrix->matrix[4]  = 0.0f;
  matrix->matrix[5]  = fovRad;
  matrix->matrix[6]  = 0.0f;
  matrix->matrix[7]  = 0.0f;
  
  matrix->matrix[8]  = 0.0f;
  matrix->matrix[9]  = 0.0f;
  matrix->matrix[10] = far / far - near;
  matrix->matrix[11] = 1.0f;
  
  matrix->matrix[12] = 0.0f;
  matrix->matrix[13] = 0.0f;
  matrix->matrix[14] = ( -far * near ) / far - near;
  matrix->matrix[15] = 0.0f;
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
void d_matrix_multiplyf( dMat4x4_t *output, const float *a, const float *b )
{
  output->matrix[0]  = ( b[0]  * a[0] ) + ( b[1]  * a[4] ) + ( b[2]  * a[8] ) + ( b[3]  * a[12] );
  output->matrix[4]  = ( b[4]  * a[0] ) + ( b[5]  * a[4] ) + ( b[6]  * a[8] ) + ( b[7]  * a[12] );
  output->matrix[8]  = ( b[8]  * a[0] ) + ( b[9]  * a[4] ) + ( b[10] * a[8] ) + ( b[9]  * a[12] );
  output->matrix[12] = ( b[12] * a[0] ) + ( b[13] * a[4] ) + ( b[14] * a[8] ) + ( b[12] * a[12] );

  output->matrix[1]  = ( b[0]  * a[1] ) + ( b[1]  * a[5] ) + ( b[2]  * a[9] ) + ( b[3]  * a[13] );
  output->matrix[5]  = ( b[4]  * a[1] ) + ( b[5]  * a[5] ) + ( b[6]  * a[9] ) + ( b[7]  * a[13] );
  output->matrix[9]  = ( b[8]  * a[1] ) + ( b[9]  * a[5] ) + ( b[10] * a[9] ) + ( b[9]  * a[13] );
  output->matrix[13] = ( b[12] * a[1] ) + ( b[13] * a[5] ) + ( b[14] * a[9] ) + ( b[12] * a[13] );

  output->matrix[2]  = ( b[0]  * a[2] ) + ( b[1]  * a[6] ) + ( b[2]  * a[10] ) + ( b[3]  * a[14] );
  output->matrix[6]  = ( b[4]  * a[2] ) + ( b[5]  * a[6] ) + ( b[6]  * a[10] ) + ( b[7]  * a[14] );
  output->matrix[10] = ( b[8]  * a[2] ) + ( b[9]  * a[6] ) + ( b[10] * a[10] ) + ( b[9]  * a[14] );
  output->matrix[14] = ( b[12] * a[2] ) + ( b[13] * a[6] ) + ( b[14] * a[10] ) + ( b[12] * a[14] );

  output->matrix[3]  = ( b[0]  * a[3] ) + ( b[1]  * a[7] ) + ( b[2]  * a[11] ) + ( b[3]  * a[15] );
  output->matrix[7]  = ( b[4]  * a[3] ) + ( b[5]  * a[7] ) + ( b[6]  * a[11] ) + ( b[7]  * a[15] );
  output->matrix[11] = ( b[8]  * a[3] ) + ( b[9]  * a[7] ) + ( b[10] * a[11] ) + ( b[9]  * a[15] );
  output->matrix[15] = ( b[12] * a[3] ) + ( b[13] * a[7] ) + ( b[14] * a[11] ) + ( b[12] * a[15] );
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
void d_matrix_translate_dVec3f( dMat4x4_t *matrix, const dVec3_t vec )
{
  matrix->matrix[0]  = 1.0f;
  matrix->matrix[1]  = 0.0f;
  matrix->matrix[2]  = 0.0f;
  matrix->matrix[3]  = 0.0f;
  
  matrix->matrix[4]  = 0.0f;
  matrix->matrix[5]  = 1.0f;
  matrix->matrix[6]  = 0.0f;
  matrix->matrix[7]  = 0.0f;

  matrix->matrix[8]  = 0.0f;
  matrix->matrix[9]  = 0.0f;
  matrix->matrix[10] = 1.0f;
  matrix->matrix[11] = 0.0f;

  matrix->matrix[12] = vec.x;
  matrix->matrix[13] = vec.y;
  matrix->matrix[14] = vec.z;
  matrix->matrix[15] = 1.0f;
}

/*
  Translate a Matrix by a 3D Point

  Parameters:
    matrix: pointer to matrix
    vec:    vector4f struct 
  
  Returns:
    void
*/

void d_matrix_translate_dVec4f( dMat4x4_t *matrix, const dVec4_t vec )
{
  matrix->matrix[0]  = 1.0f;
  matrix->matrix[1]  = 0.0f;
  matrix->matrix[2]  = 0.0f;
  matrix->matrix[3]  = 0.0f;

  matrix->matrix[4]  = 0.0f;
  matrix->matrix[5]  = 1.0f;
  matrix->matrix[6]  = 0.0f;
  matrix->matrix[7]  = 0.0f;

  matrix->matrix[8]  = 0.0f;
  matrix->matrix[9]  = 0.0f;
  matrix->matrix[10] = 1.0f;
  matrix->matrix[11] = 0.0f;

  matrix->matrix[12] = vec.x;
  matrix->matrix[13] = vec.y;
  matrix->matrix[14] = vec.z;
  matrix->matrix[15] = vec.w;
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
void d_matrix_multiply_dVec3f( dVec3_t *output, const dMat4x4_t matrix, const dVec3_t vec )
{
  output->x = ( matrix.matrix[0] * vec.x ) + ( matrix.matrix[4] * vec.y ) + ( matrix.matrix[8]  * vec.z ) + matrix.matrix[12];
  output->y = ( matrix.matrix[1] * vec.x ) + ( matrix.matrix[5] * vec.y ) + ( matrix.matrix[9]  * vec.z ) + matrix.matrix[13];
  output->z = ( matrix.matrix[2] * vec.x ) + ( matrix.matrix[6] * vec.y ) + ( matrix.matrix[10] * vec.z ) + matrix.matrix[14];
  float w   = ( matrix.matrix[3] * vec.x ) + ( matrix.matrix[7] * vec.y ) + ( matrix.matrix[11] * vec.z ) + matrix.matrix[15];

  if ( w != 0.0f )
  {
    output->x /= w;
    output->y /= w;
    output->z /= w;
  }
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
void d_matrix_inverse_transform_dVec3f( dVec3_t *output, const dMat4x4_t matrix, dVec3_t vec )
{
  vec.x -= matrix.matrix[12];
  vec.y -= matrix.matrix[13];
  vec.z -= matrix.matrix[14];
  output->x = ( matrix.matrix[0] * vec.x ) + ( matrix.matrix[1] * vec.y ) + ( matrix.matrix[2]  * vec.z );
  output->y = ( matrix.matrix[4] * vec.x ) + ( matrix.matrix[5] * vec.y ) + ( matrix.matrix[6]  * vec.z );
  output->z = ( matrix.matrix[8] * vec.x ) + ( matrix.matrix[9] * vec.y ) + ( matrix.matrix[10] * vec.z );
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

void d_matrix_multiply_dVec4f( dVec4_t *output, const dMat4x4_t matrix, const dVec4_t vec4 )
{
  output->x = ( matrix.matrix[0] * vec4.x ) + ( matrix.matrix[4] * vec4.y ) + ( matrix.matrix[8]  * vec4.z ) + ( matrix.matrix[12] * vec4.w );
  output->y = ( matrix.matrix[1] * vec4.x ) + ( matrix.matrix[5] * vec4.y ) + ( matrix.matrix[9]  * vec4.z ) + ( matrix.matrix[13] * vec4.w );
  output->z = ( matrix.matrix[2] * vec4.x ) + ( matrix.matrix[6] * vec4.y ) + ( matrix.matrix[10] * vec4.z ) + ( matrix.matrix[14] * vec4.w );
  output->w = ( matrix.matrix[3] * vec4.x ) + ( matrix.matrix[7] * vec4.y ) + ( matrix.matrix[11] * vec4.z ) + ( matrix.matrix[15] * vec4.w );
}
