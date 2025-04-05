#ifndef __DAEDALUS_H__
#define __DAEDALUS_H__

#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LENGTH     1024
#define MAX_FILENAME_LENGTH 256

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

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define STRNCPY(dest, src, n) strncpy(dest, src, n); dest[n - 1] = '\0'

#define RANDF(lower, upper) (((float)rand() / (float)(RAND_MAX)) * (upper - lower)) + lower

#define MAP( value, start0, start1, end0, end1 ) ( ( value - start0 ) * ( ( end1 - end0 ) / ( start1 - start0 ) ) + end0 )

#define PI 3.14159265

#define d_sqrt sqrtf

/* Vector Math Float */
extern float d_sqrt( float number ); //Quake fast inverse square root

extern float d_lengthOfVec2f( const dVec2_t vec ); //Length of a vector 2f
extern float d_distanceVec2f( const dVec2_t a, const dVec2_t b ); //Distance between two vector 2fs
extern float d_dotProductVec2f( const dVec2_t a, const dVec2_t b ); //Dot product between two vector 2fs
extern float d_crossProductVec2f( const dVec2_t a, const dVec2_t b ); //Cross product of two vector 2fs
extern void  d_addTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Add two vector 2fs
extern void  d_subTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Subtract two vector 2fs
extern void  d_getVec2fFromAnglef( dVec2_t *output, float angle );
extern void  d_scaleMultiplyVec2f( dVec2_t *output, const dVec2_t vec, const float value ); //Multiply a vector 2f by a value
extern void  d_scaleDivideVec2f( dVec2_t *output, const dVec2_t vec, const float value ); //Divide a vector 2f by a value
extern void  d_limitVec2f( dVec2_t *output, const dVec2_t a, const float value ); //Limit a vector 2f within a range
extern void  d_normalizeVec2f( dVec2_t *output, const dVec2_t vec ); //Normalize a vector 2f
extern void  d_createNormalVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Create a normal vector from two vector 2fs
extern void  d_findIntersectionVec2f( dVec2_t *output, const dVec2_t lineA0, const dVec2_t lineA1, const dVec2_t lineB0, const dVec2_t lineB1 ); //Find the intersection between two vector 2fs

extern float d_lengthOfVec3f( const dVec3_t vec ); //Length of a vector 3f
extern float d_distanceVec3f( const dVec3_t a, const dVec3_t b ); //Distance between two vector 3fs
extern float d_dotProductVec3f( const dVec3_t a, const dVec3_t b ); //Dot product between two vector 3fs
extern void  d_crossProductVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Cross product of two vector 3fs
extern void  d_addTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Add two vector 3fs
extern void  d_subTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Subtract two vector 3fs
extern void  d_scaleMultiplyVec3f( dVec3_t *output, const dVec3_t vec, const float value ); //Multiply a vector 3f by a value
extern void  d_scaleDivideVec3f( dVec3_t *output, const dVec3_t vec, const float value ); //Divide a vector 3f by a value
extern void  d_limitVec3f( dVec3_t *output, const dVec3_t a, const float value ); //Limit a vector 3f within a range
extern void  d_normalizeVec3f( dVec3_t *output, const dVec3_t vec ); //Normalize a vector 3f
extern void  d_createNormalVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b, const dVec3_t c ); //Create a normal vector from two vector 3fs
extern void  d_normalizeVec4f( dVec4_t *output, const dVec4_t vec );

/* Matrix Operations */
extern void d_matrixClearf( dMat4x4_t *matrix ); //Clear a 4x4 matrix to an identity matrix
extern void d_matrixRotateXf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the x axis
extern void d_matrixRotateYf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the y axis
extern void d_matrixRotateZf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the z axis
extern void d_matrixCreateProjectionf( dMat4x4_t *matrix, const float aspect_ratio, const float fov, const float near, const float far ); //Create a projection matrix 
extern void d_matrixMultiplyf( dMat4x4_t *output, const dMat4x4_t a, const dMat4x4_t b ); //Multiply two 4x4 Matrices together
extern void d_matrixTranslateVec3f( dMat4x4_t *matrix, const dVec3_t vec ); //Translate a matrix by a 3D point
extern void d_matrixTranslateVec4f( dMat4x4_t *matrix, const dVec4_t vec ); //Translate a matrix by a 4D point
extern void d_matrixMultiplyVec3f( dVec3_t *output, dMat4x4_t matrix, const dVec3_t vec ); //Transform a 3D point into a 4x4 matrix 
extern void d_matrixInverseTransformVec3f( dVec3_t *output, const dMat4x4_t matrix, dVec3_t vec ); //Inverse transform 3D point into a 4x4 matrix
extern void d_matrixMultiplyVec4f( dVec4_t *output, const dMat4x4_t matrix, const dVec4_t vec4 ); //Transform a 4D point into a 4x4 matrix

/* Matrix Creation */
extern void d_matrixXYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrixXZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrixYXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrixYZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrixZXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrixZYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );

/* Linked List */
extern dLinkedList_t *d_createLinkedList( void *data, char *name, size_t size );
extern void *d_getDataInLinkedListByIndex( dLinkedList_t *head, int index );
extern void *d_getDataInLinkedListByName( dLinkedList_t *head, char *name );
extern void d_removeNodeInLinkedListByIndex( dLinkedList_t *head, int index );
extern void d_removeNodeInLinkedListByName( dLinkedList_t *head, char *name );
extern void d_clearLinkedList( dLinkedList_t *head );
extern void d_pushBack( dLinkedList_t *head, void *data, char *name, size_t size );
extern void d_pushFront( dLinkedList_t **head, void *data, char *name, size_t size );
extern void *d_popBack( dLinkedList_t *head );
extern void *d_popFront( dLinkedList_t **head );
extern void d_printLinkedList( dLinkedList_t *head );
extern int  d_getLengthOfLinkedList( dLinkedList_t *head );

/* Quad Tree */
extern dQuadTree_t *d_createQuadtree( float *rect, int capacity );
extern void d_insertObjectInQuadtree( dQuadTree_t *tree, void *object );
extern void d_subdivideQuadtree( dQuadTree_t *tree );

/* Kinematic Body 2D  */
extern void d_createKinmaticBody( dKinematicBody_t *output, const dVec2_t position, const dVec2_t velocity, const dVec2_t acceleration, const float mass );

/* Strings */
extern char* d_createStringFromFile(const char *filename);

/* Dynamic Arrays */
extern DynamicArray_t* d_initArray( size_t capacity, size_t element_size );
extern void d_appendArray( DynamicArray_t* array, void* data );
extern void* d_getDataFromArrayByIndex( DynamicArray_t* array, size_t count );
extern void* d_popDataFromArray( DynamicArray_t* array );
extern int d_resizeArray( DynamicArray_t* array, size_t new_capacity );
extern void d_destroyArray( DynamicArray_t* array );

#endif
