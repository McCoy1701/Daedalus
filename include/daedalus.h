#ifndef __DAEDALUS_H__
#define __DAEDALUS_H__

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef signed char    int8;
typedef unsigned char  uint8;
typedef signed short   int16;
typedef unsigned short uint16;
typedef signed int     int32;
typedef unsigned int   uint32;
typedef float          real32;
typedef double         real64;
typedef unsigned char  boolean;

#define PI 3.14159265

#define dSqrt sqrt

/* Vector Math Float */
extern float dSqrt( float number );

extern void dAddVec2f( float *output, const float *a, const float *b );
extern void dSubVec2f( float *output, const float *a, const float *b );
extern void dMinVec2f( float *output, const float *a, const float *b );
extern void dMaxVec2f( float *output, const float *a, const float *b );
extern void dMultiplyVec2f( float *output, const float *a, const float value );
extern void dDivideVec2f( float *output, const float *a, const float value );
extern void dLimitVec2f( float *a, const float value );
extern float dLength2f( const float *vec );
extern float dDistance2f( const float *a, const float *b );
extern float dDot2f( const float *a, const float *b );
extern void dCross2f( float *output, const float *a, const float *b );
extern void dNormalize2f( float *vec );
extern void dNormal2f( float *output, const float *a, const float *b );
extern void dIntersect2f( float *output, const float *lineA0, const float *lineA1, const float *lineB0, const float *lineB1 );

extern void dAddVec3f( float *output, const float *a, const float *b );
extern void dSubVec3f( float *output, const float *a, const float *b );
extern void dMinVec3f( float *output, const float *a, const float *b );
extern void dMaxVec3f( float *output, const float *a, const float *b );
extern void dMultiplyVec3f( float *output, const float *a, const float value );
extern void dDivideVec3f( float *output, const float *a, const float value );
extern void dLimitVec3f( float *a, const float value );
extern float dLength3f( const float *vec );
extern float dDistance3f( const float *a, const float *b );
extern float dDot3f( const float *a, const float *b );
extern void dCross3f( float *output, const float *a, const float *b );
extern void dNormalize3f( float *vec );
extern void dNormal3f( float *output, const float *a, const float *b, const float *c );
extern void dNormalize4f( float *vec );

/* Vector Math Double */
extern void dAddVec2d( double *output, const double *a, const double *b );
extern void dSubVec2d( double *output, const double *a, const double *b );
extern void dMinVec2d( double *output, const double *a, const double *b );
extern void dMaxVec2d( double *output, const double *a, const double *b );
extern void dMultiplyVec2d( double *output, const double *a, const double value );
extern void dDivideVec2d( double *output, const double *a, const double value );
extern void dLimitVec2d( double *a, const double value );
extern double dLength2d( const double *vec );
extern double dDistance2d( const double *a, const double *b );
extern double dDot2d( const double *a, const double *b );
extern void dCross2d( double *output, const double *a, const double *b );
extern void dNormalize2d( double *vec );
extern void dNormal2d( double *output, const double *a, const double *b );
extern void dIntersect2d( const double *output, const double *lineA0, const double *lineA1, const double *lineB0, const double *lineB1 );

extern void dAddVec3d( double *output, const double *a, const double *b );
extern void dSubVec3d( double *output, const double *a, const double *b );
extern void dMinVec3d( double *output, const double *a, const double *b );
extern void dMaxVec3d( double *output, const double *a, const double *b );
extern void dMultiplyVec3d( double *output, const double *a, const double value );
extern void dDivideVec3d( double *output, const double *a, const double value );
extern void dLimitVec3d( double *a, const double value );
extern double dLength3d( const double *vec );
extern double dDistance3d( const double *a, const double *b );
extern double dDot3d( const double *a, const double *b );
extern void dCross3d( double *output, const double *a, const double *b );
extern void dNormalize3d( double *vec );
extern void dNormal3d( double *output, const double *a, const double *b, const double *c );
extern void dNormalize4d( double *vec );

/* Vector Integer Math */
extern long dSqrti( int number );
extern unsigned char dNormalize2di( int *point, int multiplyer );
extern unsigned char dNormalize3di( int *point, int multiplyer );
extern void dIntersect2di( int *output, int *lineA0, int *lineA1,int *lineB0, int *lineB1 );

/* Matrix Operations */
extern void dMatrixClearf( float *matrix );
extern void dMatrixTransform3f( float *output, const float *matrix, const float x, const float y, const float z );
extern void dMatrixInverseTransform3f( float *output, const float *matrix, float x, float y, float z );
extern void dMatrixTransform4f( float *output, const float *matrix, const float x, const float y, const float z, const float w );
extern void dMatrixMultiplyf( float *output, const float *a, const float *b );
extern void dMatrixInversef( float *output, const float *matrix );
extern void dMatrixRotateXf( float *matrix, const float angleRad );
extern void dMatrixRotateYf( float *matrix, const float angleRad );
extern void dMatrixRotateZf( float *matrix, const float angleRad );
extern void dMatrixTranslation3f( float *matrix, const float x, const float y, const float z );

extern void dMatrixCleard( float *matrix );
extern void dMatrixTransform3d( double *output, const double *matrix, const double x, const double y, const double z );
extern void dMatrixInverseTransform3d( double *output, const double *matrix, double x, double y, double z );
extern void dMatrixTransform4d( double *output, const double *matrix, const double x, const double y, const double z, const double w );
extern void dMatrixMultiplyd( double *output, const double *a, const double *b );
extern void dMatrixInversed( double* output, const double *matrix );
extern void dMatrixRotateYd( double *matrix, const double angleRad );
extern void dMatrixRotateXd( double *matrix, const double angleRad );
extern void dMatrixRotateZd( double *matrix, const double angleRad );
extern void dMatrixTranslation3d( double *matrix, const double x, const double y, const double z );

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

#endif
