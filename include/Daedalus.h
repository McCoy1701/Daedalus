#include <stdlib.h>
#include <time.h>

#ifndef __DAEDALUS_H__
#define __DAEDALUS_H__

#define MAX_LINE_LENGTH     1024
#define MAX_FILENAME_LENGTH 256

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define STRNCPY(dest, src, n) strncpy(dest, src, n); dest[n - 1] = '\0'

#define RANDF(lower, upper) (((float)rand() / (float)(RAND_MAX)) * (upper - lower)) + lower

#define MAP( value, start0, start1, end0, end1 ) ( ( value - start0 ) * ( ( end1 - end0 ) / ( start1 - start0 ) ) + end0 )

#define PI 3.14159265

typedef struct _dVec2_t
{
  float x, y;
} dVec2_t;

typedef struct _dVec3_t
{
  float x, y, z;
} dVec3_t;

typedef struct _dVec4_t
{
  float x;
  float y;
  float z;
  float w;
} dVec4_t;

typedef struct _dMat4x4_t
{
  float m[16];
} dMat4x4_t;

typedef struct _dKinematicBody
{
  dVec3_t position;
  dVec3_t velocity;
  dVec3_t acceleration;
  dVec3_t force;
  float mass;
} dKinematicBody_t;

typedef struct
{
  dVec3_t points[3];
} dTriangle_t;

typedef struct
{
  dTriangle_t *triangles;
  int numberOfTriangles;
} dMesh_t;

typedef struct _dLinkedList_t
{
  void *data;
  char buffer[MAX_FILENAME_LENGTH];
  struct _dLinkedList_t *next;
} dLinkedList_t;

typedef struct _dQuadTree_t
{
  float rect[4];
  int capacity;
  dLinkedList_t *objects;
} dQuadTree_t;

typedef struct
{
  size_t capacity;
  size_t count;
  size_t element_size;
  void* data;
} DynamicArray_t;

typedef struct
{
  char* str;
  size_t size;
  size_t len;
} dSTR_t;

/* Vector Math Float */
extern float d_Sqrtf( float number ); //Quake fast inverse square root

extern float d_LengthOfVec2f( const dVec2_t vec ); //Length of a vector 2f
extern float d_DistanceVec2f( const dVec2_t a, const dVec2_t b ); //Distance between two vector 2fs
extern float d_DotProductVec2f( const dVec2_t a, const dVec2_t b ); //Dot product between two vector 2fs
extern float d_CrossProductVec2f( const dVec2_t a, const dVec2_t b ); //Cross product of two vector 2fs
extern void  d_AddTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Add two vector 2fs
extern void  d_SubTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Subtract two vector 2fs
extern void  d_GetVec2fFromAnglef( dVec2_t *output, float angle );
extern void  d_ScaleMultiplyVec2f( dVec2_t *output, const dVec2_t vec, const float value ); //Multiply a vector 2f by a value
extern void  d_ScaleDivideVec2f( dVec2_t *output, const dVec2_t vec, const float value ); //Divide a vector 2f by a value
extern void  d_LimitVec2f( dVec2_t *output, const dVec2_t a, const float value ); //Limit a vector 2f within a range
extern void  d_NormalizeVec2f( dVec2_t *output, const dVec2_t vec ); //Normalize a vector 2f
extern void  d_CreateNormalVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Create a normal vector from two vector 2fs
extern void  d_FindIntersectionVec2f( dVec2_t *output, const dVec2_t lineA0, const dVec2_t lineA1, const dVec2_t lineB0, const dVec2_t lineB1 ); //Find the intersection between two vector 2fs

extern float d_LengthOfVec3f( const dVec3_t vec ); //Length of a vector 3f
extern float d_DistanceVec3f( const dVec3_t a, const dVec3_t b ); //Distance between two vector 3fs
extern float d_DotProductVec3f( const dVec3_t a, const dVec3_t b ); //Dot product between two vector 3fs
extern void  d_CrossProductVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Cross product of two vector 3fs
extern void  d_AddTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Add two vector 3fs
extern void  d_SubTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Subtract two vector 3fs
extern void  d_ScaleMultiplyVec3f( dVec3_t *output, const dVec3_t vec, const float value ); //Multiply a vector 3f by a value
extern void  d_ScaleDivideVec3f( dVec3_t *output, const dVec3_t vec, const float value ); //Divide a vector 3f by a value
extern void  d_LimitVec3f( dVec3_t *output, const dVec3_t a, const float value ); //Limit a vector 3f within a range
extern void  d_NormalizeVec3f( dVec3_t *output, const dVec3_t vec ); //Normalize a vector 3f
extern void  d_CreateNormalVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b, const dVec3_t c ); //Create a normal vector from two vector 3fs
extern void  d_NormalizeVec4f( dVec4_t *output, const dVec4_t vec );

/* Matrix Operations */
extern void d_MatrixClearf( dMat4x4_t *matrix ); //Clear a 4x4 matrix to an identity matrix
extern void d_MatrixRotateXf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the x axis
extern void d_MatrixRotateYf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the y axis
extern void d_MatrixRotateZf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the z axis
extern void d_MatrixCreateProjectionf( dMat4x4_t *matrix, const float aspect_ratio, const float fov, const float near, const float far ); //Create a projection matrix 
extern void d_MatrixMultiplyf( dMat4x4_t *output, const dMat4x4_t a, const dMat4x4_t b ); //Multiply two 4x4 Matrices together
extern void d_MatrixTranslateVec3f( dMat4x4_t *matrix, const dVec3_t vec ); //Translate a matrix by a 3D point
extern void d_MatrixTranslateVec4f( dMat4x4_t *matrix, const dVec4_t vec ); //Translate a matrix by a 4D point
extern void d_MatrixMultiplyVec3f( dVec3_t *output, dMat4x4_t matrix, const dVec3_t vec ); //Transform a 3D point into a 4x4 matrix 
extern void d_MatrixInverseTransformVec3f( dVec3_t *output, const dMat4x4_t matrix, dVec3_t vec ); //Inverse transform 3D point into a 4x4 matrix
extern void d_MatrixMultiplyVec4f( dVec4_t *output, const dMat4x4_t matrix, const dVec4_t vec4 ); //Transform a 4D point into a 4x4 matrix

/* Matrix Creation */
extern void d_MatrixXYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_MatrixXZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_MatrixYXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_MatrixYZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_MatrixZXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_MatrixZYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );

/* Linked List */
extern dLinkedList_t *d_CreateLinkedList( void *data, char *name, size_t size );
extern void *d_GetDataInLinkedListByIndex( dLinkedList_t *head, int index );
extern void *d_GetDataInLinkedListByName( dLinkedList_t *head, char *name );
extern void d_RemoveNodeInLinkedListByIndex( dLinkedList_t *head, int index );
extern void d_RemoveNodeInLinkedListByName( dLinkedList_t *head, char *name );
extern void d_ClearLinkedList( dLinkedList_t *head );
extern void d_PushBack( dLinkedList_t *head, void *data, char *name, size_t size );
extern void d_PushFront( dLinkedList_t **head, void *data, char *name, size_t size );
extern void *d_PopBack( dLinkedList_t *head );
extern void *d_PopFront( dLinkedList_t **head );
extern void d_PrintLinkedList( dLinkedList_t *head );
extern int  d_GetLengthOfLinkedList( dLinkedList_t *head );

/* Quad Tree */
extern dQuadTree_t *d_CreateQuadtree( float *rect, int capacity );
extern void d_InsertObjectInQuadtree( dQuadTree_t *tree, void *object );
extern void d_SubdivideQuadtree( dQuadTree_t *tree );

/* Kinematic Body 2D  */
extern void d_CreateKinmaticBody( dKinematicBody_t *output, const dVec2_t position, const dVec2_t velocity, const dVec2_t acceleration, const float mass );

/* Strings */
extern char* d_CreateStringFromFile(const char *filename);

/* Dynamic Arrays */
extern DynamicArray_t* d_InitArray( size_t capacity, size_t element_size );
extern void d_AppendArray( DynamicArray_t* array, void* data );
extern void* d_GetDataFromArrayByIndex( DynamicArray_t* array, size_t count );
extern void* d_PopDataFromArray( DynamicArray_t* array );
extern int d_ResizeArray( DynamicArray_t* array, size_t new_capacity );
extern void d_DestroyArray( DynamicArray_t* array );

#endif
