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
  matrix->m[0][0] = 1.0f;
  matrix->m[0][1] = 0.0f;
  matrix->m[0][2] = 0.0f;
  matrix->m[0][3] = 0.0f;

  matrix->m[1][0] = 0.0f;
  matrix->m[1][1] = 1.0f;
  matrix->m[1][2] = 0.0f;
  matrix->m[1][3] = 0.0f;

  matrix->m[2][0] = 0.0f;
  matrix->m[2][1] = 0.0f;
  matrix->m[2][2] = 1.0f;
  matrix->m[2][3] = 0.0f;

  matrix->m[3][0] = 0.0f;
  matrix->m[3][1] = 0.0f;
  matrix->m[3][2] = 0.0f;
  matrix->m[3][3] = 1.0f;
}

/*
  Rotate a Matrix about the X Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void d_matrix_rotate_xf( dMat4x4_t *matrix, const float fAngleRad )
{
  matrix->m[0][0] =  1.0f;
  matrix->m[0][1] =  0.0f;
  matrix->m[0][2] =  0.0f;
  matrix->m[0][3] =  0.0f;

  matrix->m[1][0] =  0.0f;
  matrix->m[1][1] =  cosf(fAngleRad);
  matrix->m[1][2] =  sinf(fAngleRad);
  matrix->m[1][3] =  0.0f;
  
  matrix->m[2][0] =  0.0f;
  matrix->m[2][1] = -sinf(fAngleRad);
  matrix->m[2][2] =  cosf(fAngleRad);
  matrix->m[2][3] =  0.0f;
  
  matrix->m[3][0] =  0.0f;
  matrix->m[3][1] =  0.0f;
  matrix->m[3][2] =  0.0f;
  matrix->m[3][3] =  1.0f;
}

/*
  Rotate a Matrix about the Y Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void d_matrix_rotate_yf( dMat4x4_t *matrix, const float fAngleRad )
{
  matrix->m[0][0] =  cosf(fAngleRad);
  matrix->m[0][1] =  0.0f;
  matrix->m[0][2] =  sinf(fAngleRad);
  matrix->m[0][3] =  0.0f;
  
  matrix->m[1][0] =  0.0f;
  matrix->m[1][1] =  1.0f;
  matrix->m[1][2] =  0.0f;
  matrix->m[1][3] =  0.0f;

  matrix->m[2][0] = -sinf(fAngleRad);
  matrix->m[2][1] =  0.0f;
  matrix->m[2][2] =  cosf(fAngleRad);
  matrix->m[2][3] =  0.0f;
  
  matrix->m[3][0] =  0.0f;
  matrix->m[3][1] =  0.0f;
  matrix->m[3][2] =  0.0f;
  matrix->m[3][3] =  1.0f;
}

/*
  Rotate a Matrix about the Z Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void d_matrix_rotate_zf( dMat4x4_t *matrix, const float fAngleRad )
{
  matrix->m[0][0] =  cosf(fAngleRad);
  matrix->m[0][1] =  sinf(fAngleRad);
  matrix->m[0][2] =  0.0f;
  matrix->m[0][3] =  0.0f;

  matrix->m[1][0] = -sinf(fAngleRad);
  matrix->m[1][1] =  cosf(fAngleRad);
  matrix->m[1][2] =  0.0f;
  matrix->m[1][3] =  0.0f;

  matrix->m[2][0] =  0.0f;
  matrix->m[2][1] =  0.0f;
  matrix->m[2][2] =  1.0f;
  matrix->m[2][3] =  0.0f;

  matrix->m[3][0] =  0.0f;
  matrix->m[3][1] =  0.0f;
  matrix->m[3][2] =  0.0f;
  matrix->m[3][3] =  1.0f;
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
  float fFovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
  
  matrix->m[0][0] = aspectRatio * fFovRad;
  matrix->m[0][1] = 0.0f;
  matrix->m[0][2] = 0.0f;
  matrix->m[0][3] = 0.0f;

  matrix->m[1][0] = 0.0f;
  matrix->m[1][1] = fFovRad;
  matrix->m[1][2] = 0.0f;
  matrix->m[1][3] = 0.0f;

  matrix->m[2][0] = 0.0f;
  matrix->m[2][1] = 0.0f;
  matrix->m[2][2] = far / far - near;
  matrix->m[2][3] = 1.0f;

  matrix->m[3][0] = 0.0f;
  matrix->m[3][1] = 0.0f;
  matrix->m[3][2] = (-far * near) / far - near;
  matrix->m[3][3] = 0.0f;
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
void d_matrix_multiplyf( dMat4x4_t *output, const dMat4x4_t a, const dMat4x4_t b )
{ 
  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 4; r++) {
      output->m[r][c] = a.m[r][0] * b.m[0][c] + a.m[r][1] * b.m[1][c] + a.m[r][2] * b.m[2][c] + a.m[r][3] * b.m[3][c];
    }
  }
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
  matrix->m[0][0] = 1.0f;
  matrix->m[0][1] = 0.0f;
  matrix->m[0][2] = 0.0f;
  matrix->m[0][3] = 0.0f;

  matrix->m[1][0] = 0.0f;
  matrix->m[1][1] = 1.0f;
  matrix->m[1][2] = 0.0f;
  matrix->m[1][3] = 0.0f;

  matrix->m[2][0] = 0.0f;
  matrix->m[2][1] = 0.0f;
  matrix->m[2][2] = 1.0f;
  matrix->m[2][3] = 0.0f;

  matrix->m[3][0] = vec.x;
  matrix->m[3][1] = vec.y;
  matrix->m[3][2] = vec.z;
  matrix->m[3][3] = 1.0f;
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
  matrix->m[0][0] = 1.0f;
  matrix->m[0][1] = 0.0f;
  matrix->m[0][2] = 0.0f;
  matrix->m[0][3] = 0.0f;

  matrix->m[1][0] = 0.0f;
  matrix->m[1][1] = 1.0f;
  matrix->m[1][2] = 0.0f;
  matrix->m[1][3] = 0.0f;

  matrix->m[2][0] = 0.0f;
  matrix->m[2][1] = 0.0f;
  matrix->m[2][2] = 1.0f;
  matrix->m[2][3] = 0.0f;

  matrix->m[3][0] = vec.x;
  matrix->m[3][1] = vec.y;
  matrix->m[3][2] = vec.z;
  matrix->m[3][3] = vec.w;
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
void d_matrix_multiply_dVec3f( dVec3_t *output, dMat4x4_t matrix, const dVec3_t vec )
{
  output->x = vec.x * matrix.m[0][0] + vec.y * matrix.m[1][0] + vec.z * matrix.m[2][0] + vec.w * matrix.m[3][0];
  output->y = vec.x * matrix.m[0][1] + vec.y * matrix.m[1][1] + vec.z * matrix.m[2][1] + vec.w * matrix.m[3][1];
  output->z = vec.x * matrix.m[0][2] + vec.y * matrix.m[1][2] + vec.z * matrix.m[2][2] + vec.w * matrix.m[3][2];
  output->w = vec.x * matrix.m[0][3] + vec.y * matrix.m[1][3] + vec.z * matrix.m[2][3] + vec.w * matrix.m[3][3];
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
  vec.x -= matrix.m[3][0];
  vec.y -= matrix.m[3][1];
  vec.z -= matrix.m[3][2];
  output->x = ( matrix.m[0][0] * vec.x ) + ( matrix.m[1][1] * vec.y ) + ( matrix.m[2][2]  * vec.z );
  output->y = ( matrix.m[0][4] * vec.x ) + ( matrix.m[1][5] * vec.y ) + ( matrix.m[2][6]  * vec.z );
  output->z = ( matrix.m[0][8] * vec.x ) + ( matrix.m[1][9] * vec.y ) + ( matrix.m[2][10] * vec.z );
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
  output->x = ( matrix.m[0][0] * vec4.x ) + ( matrix.m[1][4] * vec4.y ) + ( matrix.m[2][8]  * vec4.z ) + ( matrix.m[3][12] * vec4.w );
  output->y = ( matrix.m[0][1] * vec4.x ) + ( matrix.m[1][5] * vec4.y ) + ( matrix.m[2][9]  * vec4.z ) + ( matrix.m[3][13] * vec4.w );
  output->z = ( matrix.m[0][2] * vec4.x ) + ( matrix.m[1][6] * vec4.y ) + ( matrix.m[2][10] * vec4.z ) + ( matrix.m[3][14] * vec4.w );
  output->w = ( matrix.m[0][3] * vec4.x ) + ( matrix.m[1][7] * vec4.y ) + ( matrix.m[2][11] * vec4.z ) + ( matrix.m[3][15] * vec4.w );
}
