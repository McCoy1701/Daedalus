#ifndef __DAEDALUS_H__
#define __DAEDALUS_H__

//#include <stdlib.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_LINE_LENGTH     1024
#define MAX_FILENAME_LENGTH 256

//#define NULL ( ( void * ) 0 )

typedef signed char    int8;
typedef unsigned char  uint8;
typedef signed short   int16;
typedef unsigned short uint16;
typedef signed int     int32;
typedef unsigned int   uint32;
typedef float          real32;
typedef double         real64;
typedef unsigned char  boolean;
typedef long unsigned int size_t;

typedef struct _deltaTime {
  uint32 lastTime;
  uint32 currentTime;
} DeltaTime;

typedef struct _dLinkedList {
  void *data;
  char buffer[MAX_FILENAME_LENGTH];
  struct _dLinkedList *next;
} dLinkedList;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define STRNCPY(dest, src, n) strncpy(dest, src, n); dest[n - 1] = '\0'

#define RANDF(upper, lower) (((float)rand() / (float)(RAND_MAX)) * (upper - lower)) + lower

#define PI 3.14159265

#define dSqrt sqrtf

/* Vector Math Float */
extern float dSqrt( float number ); //Quake fast inverse square root

extern float dLength2f( const float *vec ); //Lenght of a vector 2f
extern float dDistance2f( const float *a, const float *b ); //Distance between two vector 2fs
extern float dDot2f( const float *a, const float *b ); //Dot product between two vector 2fs
extern float dCross2f( const float *a, const float *b ); //Cross product of two vector 2fs
extern void dAddVec2f( float *output, const float *a, const float *b ); //Add two vector 2fs
extern void dSubVec2f( float *output, const float *a, const float *b ); //Subtract two vector 2fs
extern void dMinVec2f( float *output, const float *a, const float *b ); //Return the smaller of two vector 2fs
extern void dMaxVec2f( float *output, const float *a, const float *b ); //Return the larger of two vector 2fs
extern void dMultiplyVec2f( float *output, const float *a, const float value ); //Multiply a vector 2f by a value
extern void dDivideVec2f( float *output, const float *a, const float value ); //Divide a vector 2f by a value
extern void dLimitVec2f( float *a, const float value ); //Limit a vector 2f within a range
extern void dNormalize2f( float *vec ); //Normalize a vector 2f
extern void dNormal2f( float *output, const float *a, const float *b ); //Create a normal vector from two vector 2fs
extern void dIntersect2f( float *output, const float *lineA0, const float *lineA1, const float *lineB0, const float *lineB1 ); //Find the intersection between two vector 2fs

extern float dLength3f( const float *vec ); //Lenght of a vector 3f
extern float dDistance3f( const float *a, const float *b ); //Distance between two vector 3fs
extern float dDot3f( const float *a, const float *b ); //Dot product between two vector 3fs
extern void dAddVec3f( float *output, const float *a, const float *b ); //Add two vector 3fs
extern void dSubVec3f( float *output, const float *a, const float *b ); //Subtract two vector 3fs
extern void dMinVec3f( float *output, const float *a, const float *b ); //Return the smaller of two vector 3fs
extern void dMaxVec3f( float *output, const float *a, const float *b ); //Return the larger of two vector 3fs
extern void dMultiplyVec3f( float *output, const float *a, const float value ); //Multiply a vector 3f by a value
extern void dDivideVec3f( float *output, const float *a, const float value ); //Divide a vector 3f by a value
extern void dLimitVec3f( float *a, const float value ); //Limit a vector 3f within a range
extern void dCross3f( float *output, const float *a, const float *b ); //Cross product of two vector 3fs
extern void dNormalize3f( float *vec ); //Normalize a vector 3f
extern void dNormal3f( float *output, const float *a, const float *b, const float *c ); //Create a normal vector from two vector 3fs
extern void dNormalize4f( float *vec ); //Normalize a vector 4f

/* Vector Math Double */
extern double dLength2d( const double *vec ); //Lenght of a vector 2d
extern double dDistance2d( const double *a, const double *b ); //Distance between two vector 2ds
extern double dDot2d( const double *a, const double *b ); //Dot product between two vector 2ds
extern double dCross2d( const double *a, const double *b ); //Cross product of two vector 2ds
extern void dAddVec2d( double *output, const double *a, const double *b ); //Add two vector 2ds
extern void dSubVec2d( double *output, const double *a, const double *b ); //Subtract two vector 2ds
extern void dMinVec2d( double *output, const double *a, const double *b ); //Return the smaller of two vector 2ds
extern void dMaxVec2d( double *output, const double *a, const double *b ); //Return the larger of two vector 2ds
extern void dMultiplyVec2d( double *output, const double *a, const double value ); //Multiply a vector 2d by a value
extern void dDivideVec2d( double *output, const double *a, const double value ); //Divide a vector 2d by a value
extern void dLimitVec2d( double *a, const double value ); //Limit a vector 2d within a range
extern void dNormalize2d( double *vec ); //Normalize a vector 2d
extern void dNormal2d( double *output, const double *a, const double *b ); //Create a normal vector from two vector 2ds
extern void dIntersect2d( const double *output, const double *lineA0, const double *lineA1, const double *lineB0, const double *lineB1 ); //Find the intersection between two vector 2ds

extern double dLength3d( const double *vec ); //Lenght of a vector 3d
extern double dDistance3d( const double *a, const double *b ); //Distance between two vector 3ds
extern double dDot3d( const double *a, const double *b ); //Dot product between two vector 3ds
extern void dAddVec3d( double *output, const double *a, const double *b ); //Add two vector 3ds
extern void dSubVec3d( double *output, const double *a, const double *b ); //Subtract two vector 3ds
extern void dMinVec3d( double *output, const double *a, const double *b ); //Return the smaller of two vector 3ds
extern void dMaxVec3d( double *output, const double *a, const double *b ); //Return the larger of two vector 3ds
extern void dMultiplyVec3d( double *output, const double *a, const double value ); //Multiply a vector 3d by a value
extern void dDivideVec3d( double *output, const double *a, const double value ); //Divide a vector 3d by a value
extern void dLimitVec3d( double *a, const double value ); //Limit a vector 3d within a range
extern void dCross3d( double *output, const double *a, const double *b ); //Cross product of two vector 3ds
extern void dNormalize3d( double *vec ); //Normalize a vector 3d
extern void dNormal3d( double *output, const double *a, const double *b, const double *c ); //Create a normal vector from two vector 3ds
extern void dNormalize4d( double *vec ); //Normalize a vector 4d

/* Vector Integer Math */
extern long dSqrti( int number );
extern unsigned char dNormalize2di( int *point, int multiplyer );
extern unsigned char dNormalize3di( int *point, int multiplyer );
extern void dIntersect2di( int *output, int *lineA0, int *lineA1,int *lineB0, int *lineB1 );

/* Matrix Operations */
extern void dMatrixClearf( float *matrix ); //Clear a 4x4 matrix to an identity matrix
extern void dMatrixRotateXf( float *matrix, const float angleRad ); //Rotate matrix by angle in radians about the x axis
extern void dMatrixRotateYf( float *matrix, const float angleRad ); //Rotate matrix by angle in radians about the y axis
extern void dMatrixRotateZf( float *matrix, const float angleRad ); //Rotate matrix by angle in radians about the z axis
extern void dMatrixTransform3f( float *output, const float *matrix, const float x, const float y, const float z ); //Transform a 3D point into a 4x4 matrix 
extern void dMatrixProjectionf( float *matrix, const float aspectRatio, const float fov, const float far, const float near ); //Create a projection matrix 
extern void dMatrixInverseTransform3f( float *output, const float *matrix, const float x, const float y, const float z ); //Inverse transform 3D point into a 4x4 matrix
extern void dMatrixMultiplyf( float *output, const float *a, const float *b ); //Multiply two 4x4 Matrices together
extern void dMatrixInversef( float *output, const float *matrix ); //Inverse of current matrix
extern void dMatrixTranslation3f( float *matrix, const float x, const float y, const float z ); //Translate a matrix by a 3D point
extern void dMatrixTransform4f( float *output, const float *matrix, const float x, const float y, const float z, const float w ); //Transform a 4D point into a 4x4 matrix

extern void dMatrixCleard( float *matrix ); //Clear a 4x4 matrix to an identity matrix
extern void dMatrixRotateYd( double *matrix, const double angleRad ); //Rotate matrix by angle in radians about the x axis
extern void dMatrixRotateXd( double *matrix, const double angleRad ); //Rotate matrix by angle in radians about the y axis
extern void dMatrixRotateZd( double *matrix, const double angleRad ); //Rotate matrix by angle in radians about the z axis
extern void dMatrixTransform3d( double *output, const double *matrix, const double x, const double y, const double z ); //Transform a 3D point into a 4x4 matrix 
extern void dMatrixProjectiond( double *matrix, const double aspectRatio, const double fov, const double far, const double near ); //Create a projection matrix 
extern void dMatrixInverseTransform3d( double *output, const double *matrix, double x, double y, double z ); //Inverse transform 3D point into a 4x4 matrix
extern void dMatrixMultiplyd( double *output, const double *a, const double *b ); //Multiply two 4x4 Matrices together
extern void dMatrixInversed( double* output, const double *matrix ); //Inverse of current matrix
extern void dMatrixTranslation3d( double *matrix, const double x, const double y, const double z ); //Translate a matrix by a 3D point
extern void dMatrixTransform4d( double *output, const double *matrix, const double x, const double y, const double z, const double w ); //Transform a 4D point into a 4x4 matrix

/* Matrix Creation */
extern void dMatrixXYf( float *output, const float *origin, const float *point0, const float *point1 );
extern void dMatrixXZf( float *output, const float *origin, const float *point0, const float *point1 );
extern void dMatrixYXf( float *output, const float *origin, const float *point0, const float *point1 );
extern void dMatrixYZf( float *output, const float *origin, const float *point0, const float *point1 );
extern void dMatrixZXf( float *output, const float *origin, const float *point0, const float *point1 );
extern void dMatrixZYf( float *output, const float *origin, const float *point0, const float *point1 );
extern void dMatrixXYd( double *output, const double *origin, const double *point0, const double *point1 );
extern void dMatrixXZd( double *output, const double *origin, const double *point0, const double *point1 );
extern void dMatrixYXd( double *output, const double *origin, const double *point0, const double *point1 );
extern void dMatrixYZd( double *output, const double *origin, const double *point0, const double *point1 );
extern void dMatrixZXd( double *output, const double *origin, const double *point0, const double *point1 );
extern void dMatrixZYd( double *output, const double *origin, const double *point0, const double *point1 );

/* Delta Time */
extern float getDeltaTime( unsigned int *currentTime, unsigned int *lastTime );

/* Linked List */

extern dLinkedList* dCreateLinkedList( void *data, char *name, size_t size );
extern void* dGetDataInLinkedListByIndex( dLinkedList *head, int index );
extern void* dGetDataInLinkedListByName( dLinkedList *head, char *name );
extern void dRemoveNodeInLinkedListByIndex( dLinkedList *head, int index );
extern void dRemoveNodeInLinkedListByName( dLinkedList *head, char *name );
extern void dClearLinkedList( dLinkedList *head );
extern void dPushBack( dLinkedList *head, void *data, char *name, size_t size );
extern void* dPopBack( dLinkedList *head );
extern void dPushFront( dLinkedList **head, void *data, char *name, size_t size );
extern void* dPopFront( dLinkedList **head );
extern void dPrintLinkedList( dLinkedList *head );

#endif
