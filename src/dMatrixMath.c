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
void d_matrixClearf( dMat4x4_t* matrix )
{
  matrix->m[0]  = 1.0f;
  matrix->m[1]  = 0.0f;
  matrix->m[2]  = 0.0f;
  matrix->m[3]  = 0.0f;

  matrix->m[4]  = 0.0f;
  matrix->m[5]  = 1.0f;
  matrix->m[6]  = 0.0f;
  matrix->m[7]  = 0.0f;

  matrix->m[8]  = 0.0f;
  matrix->m[9]  = 0.0f;
  matrix->m[10] = 1.0f;
  matrix->m[11] = 0.0f;

  matrix->m[12] = 0.0f;
  matrix->m[13] = 0.0f;
  matrix->m[14] = 0.0f;
  matrix->m[15] = 1.0f;
}

/*
  Rotate a Matrix about the X Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void d_matrixRotateXf( dMat4x4_t *matrix, const float angle_rad )
{
  matrix->m[0]  =  1.0f;
  matrix->m[1]  =  0.0f;
  matrix->m[2]  =  0.0f;
  matrix->m[3]  =  0.0f;

  matrix->m[4]  =  0.0f;
  matrix->m[5]  =  cosf( angle_rad );
  matrix->m[6]  =  sinf( angle_rad );
  matrix->m[7]  =  0.0f;
  
  matrix->m[8]  =  0.0f;
  matrix->m[9]  = -sinf( angle_rad );
  matrix->m[10] =  cosf( angle_rad );
  matrix->m[11] =  0.0f;
  
  matrix->m[12] =  0.0f;
  matrix->m[13] =  0.0f;
  matrix->m[14] =  0.0f;
  matrix->m[15] =  1.0f;
}

/*
  Rotate a Matrix about the Y Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void d_matrixRotateYf( dMat4x4_t *matrix, const float angle_rad )
{
  matrix->m[0]  =  cosf( angle_rad );
  matrix->m[1]  =  0.0f;
  matrix->m[2]  =  sinf( angle_rad );
  matrix->m[3]  =  0.0f;
  
  matrix->m[4]  =  0.0f;
  matrix->m[5]  =  1.0f;
  matrix->m[6]  =  0.0f;
  matrix->m[7]  =  0.0f;

  matrix->m[8]  = -sinf( angle_rad );
  matrix->m[9]  =  0.0f;
  matrix->m[10] =  cosf( angle_rad );
  matrix->m[11] =  0.0f;
  
  matrix->m[12] =  0.0f;
  matrix->m[13] =  0.0f;
  matrix->m[14] =  0.0f;
  matrix->m[15] =  1.0f;
}

/*
  Rotate a Matrix about the Z Axis

  Parameters:
    matrix:   pointer to matrix
    angleRad: angle to rotate in radians
  
  Returns:
    void
*/
void d_matrixRotateZf( dMat4x4_t *matrix, const float angle_rad )
{
  matrix->m[0]  =  cosf( angle_rad );
  matrix->m[1]  =  sinf( angle_rad );
  matrix->m[2]  =  0.0f;
  matrix->m[3]  =  0.0f;

  matrix->m[4]  = -sinf( angle_rad );
  matrix->m[5]  =  cosf( angle_rad );
  matrix->m[6]  =  0.0f;
  matrix->m[7]  =  0.0f;

  matrix->m[8]  =  0.0f;
  matrix->m[9]  =  0.0f;
  matrix->m[10] =  1.0f;
  matrix->m[11] =  0.0f;

  matrix->m[12] =  0.0f;
  matrix->m[13] =  0.0f;
  matrix->m[14] =  0.0f;
  matrix->m[15] =  1.0f;
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
void d_matrixCreateProjectionf( dMat4x4_t *matrix, const float aspect_ratio, const float fov, const float near, const float far )
{
  float fFovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
  
  matrix->m[0]  = aspect_ratio * fFovRad;
  matrix->m[1]  = 0.0f;
  matrix->m[2]  = 0.0f;
  matrix->m[3]  = 0.0f;

  matrix->m[4]  = 0.0f;
  matrix->m[5]  = fFovRad;
  matrix->m[6]  = 0.0f;
  matrix->m[7]  = 0.0f;

  matrix->m[8]  = 0.0f;
  matrix->m[9]  = 0.0f;
  matrix->m[10] = far / far - near;
  matrix->m[11] = 1.0f;

  matrix->m[12] = 0.0f;
  matrix->m[13] = 0.0f;
  matrix->m[14] = (-far * near) / far - near;
  matrix->m[15] = 0.0f;
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
void d_matrixMultiplyf( dMat4x4_t* output, const dMat4x4_t a, const dMat4x4_t b )
{ 
	output->m[0]  = (b.m[0]  * a.m[0]) + (b.m[1]  * a.m[4]) + (b.m[2]  * a.m[8])  + (b.m[3]  * a.m[12]);
	output->m[4]  = (b.m[4]  * a.m[0]) + (b.m[5]  * a.m[4]) + (b.m[6]  * a.m[8])  + (b.m[7]  * a.m[12]);
	output->m[8]  = (b.m[8]  * a.m[0]) + (b.m[9]  * a.m[4]) + (b.m[10] * a.m[8])  + (b.m[11] * a.m[12]);
	output->m[12] = (b.m[12] * a.m[0]) + (b.m[13] * a.m[4]) + (b.m[14] * a.m[8])  + (b.m[15] * a.m[12]);

	output->m[1]  = (b.m[0]  * a.m[1]) + (b.m[1]  * a.m[5]) + (b.m[2]  * a.m[9])  + (b.m[3]  * a.m[13]);
	output->m[5]  = (b.m[4]  * a.m[1]) + (b.m[5]  * a.m[5]) + (b.m[6]  * a.m[9])  + (b.m[7]  * a.m[13]);
	output->m[9]  = (b.m[8]  * a.m[1]) + (b.m[9]  * a.m[5]) + (b.m[10] * a.m[9])  + (b.m[11] * a.m[13]);
	output->m[13] = (b.m[12] * a.m[1]) + (b.m[13] * a.m[5]) + (b.m[14] * a.m[9])  + (b.m[15] * a.m[13]);

	output->m[2]  = (b.m[0]  * a.m[2]) + (b.m[1]  * a.m[6]) + (b.m[2]  * a.m[10]) + (b.m[3]  * a.m[14]);
	output->m[6]  = (b.m[4]  * a.m[2]) + (b.m[5]  * a.m[6]) + (b.m[6]  * a.m[10]) + (b.m[7]  * a.m[14]);
	output->m[10] = (b.m[8]  * a.m[2]) + (b.m[9]  * a.m[6]) + (b.m[10] * a.m[10]) + (b.m[11] * a.m[14]);
	output->m[14] = (b.m[12] * a.m[2]) + (b.m[13] * a.m[6]) + (b.m[14] * a.m[10]) + (b.m[15] * a.m[14]);

	output->m[3]  = (b.m[0]  * a.m[3]) + (b.m[1]  * a.m[7]) + (b.m[2]  * a.m[11]) + (b.m[3]  * a.m[15]);
	output->m[7]  = (b.m[4]  * a.m[3]) + (b.m[5]  * a.m[7]) + (b.m[6]  * a.m[11]) + (b.m[7]  * a.m[15]);
	output->m[11] = (b.m[8]  * a.m[3]) + (b.m[9]  * a.m[7]) + (b.m[10] * a.m[11]) + (b.m[11] * a.m[15]);
	output->m[15] = (b.m[12] * a.m[3]) + (b.m[13] * a.m[7]) + (b.m[14] * a.m[11]) + (b.m[15] * a.m[15]);
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
void d_matrixTranslateVec3f( dMat4x4_t *matrix, const dVec3_t vec )
{
  matrix->m[0]  = 1.0f;
  matrix->m[1]  = 0.0f;
  matrix->m[2]  = 0.0f;
  matrix->m[3]  = 0.0f;

  matrix->m[4]  = 0.0f;
  matrix->m[5]  = 1.0f;
  matrix->m[6]  = 0.0f;
  matrix->m[7]  = 0.0f;

  matrix->m[8]  = 0.0f;
  matrix->m[9]  = 0.0f;
  matrix->m[10] = 1.0f;
  matrix->m[11] = 0.0f;

  matrix->m[12] = vec.x;
  matrix->m[13] = vec.y;
  matrix->m[14] = vec.z;
  matrix->m[15] = 1.0f;
}

/*
  Translate a Matrix by a 3D Point

  Parameters:
    matrix: pointer to matrix
    vec:    vector4f struct 
  
  Returns:
    void
*/

void d_matrixTranslateVec4f( dMat4x4_t *matrix, const dVec4_t vec )
{
  matrix->m[0]  = 1.0f;
  matrix->m[1]  = 0.0f;
  matrix->m[2]  = 0.0f;
  matrix->m[3]  = 0.0f;

  matrix->m[4]  = 0.0f;
  matrix->m[5]  = 1.0f;
  matrix->m[6]  = 0.0f;
  matrix->m[7]  = 0.0f;

  matrix->m[8]  = 0.0f;
  matrix->m[9]  = 0.0f;
  matrix->m[10] = 1.0f;
  matrix->m[11] = 0.0f;

  matrix->m[12] = vec.x;
  matrix->m[13] = vec.y;
  matrix->m[14] = vec.z;
  matrix->m[15] = vec.w;
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
void d_matrixMultiplyVec3f( dVec3_t *output, dMat4x4_t matrix, const dVec3_t vec )
{
  output->x = vec.x * matrix.m[0] + vec.y * matrix.m[4] + vec.z * matrix.m[8]  + matrix.m[12];
  output->y = vec.x * matrix.m[1] + vec.y * matrix.m[5] + vec.z * matrix.m[9]  + matrix.m[13];
  output->z = vec.x * matrix.m[2] + vec.y * matrix.m[6] + vec.z * matrix.m[10] + matrix.m[14];
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
void d_matrixInverseTransformVec3f( dVec3_t *output, const dMat4x4_t matrix, dVec3_t vec )
{
  vec.x -= matrix.m[8];
  vec.y -= matrix.m[9];
  vec.z -= matrix.m[10];
  output->x = ( matrix.m[0] * vec.x ) + ( matrix.m[1] * vec.y ) + ( matrix.m[2]  * vec.z );
  output->y = ( matrix.m[4] * vec.x ) + ( matrix.m[5] * vec.y ) + ( matrix.m[6]  * vec.z );
  output->z = ( matrix.m[8] * vec.x ) + ( matrix.m[9] * vec.y ) + ( matrix.m[10] * vec.z );
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

void d_matrixMultiplyVec4f( dVec4_t *output, const dMat4x4_t matrix, const dVec4_t vec4 )
{
  output->x = ( matrix.m[0] * vec4.x ) + ( matrix.m[4] * vec4.y ) + ( matrix.m[8]  * vec4.z ) + ( matrix.m[12] * vec4.w );
  output->y = ( matrix.m[1] * vec4.x ) + ( matrix.m[5] * vec4.y ) + ( matrix.m[9]  * vec4.z ) + ( matrix.m[13] * vec4.w );
  output->z = ( matrix.m[2] * vec4.x ) + ( matrix.m[6] * vec4.y ) + ( matrix.m[10] * vec4.z ) + ( matrix.m[14] * vec4.w );
  output->w = ( matrix.m[3] * vec4.x ) + ( matrix.m[7] * vec4.y ) + ( matrix.m[11] * vec4.z ) + ( matrix.m[15] * vec4.w );
}

