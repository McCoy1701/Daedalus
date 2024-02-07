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

extern float dLength2f( float *vec );
extern float dLength3f( float *vec );
extern float dDistance2f( float *a, float *b );
extern float dDistance3f( float *a, float *b );
extern float dDot2f( float *a, float *b );
extern float dDot3f( float *a, float *b );
extern void dCross2f( float *output, float *a, float *b );
extern void dCross3f( float *output, float *a, float *b );
extern void dNormalize2f( float *vec );
extern void dNormalize3f( float *vec );
extern void dNormalize4f( float *vec );
extern void dNormal2f( float *output, float *a, float *b );
extern void dNormal3f( float *output, float *a, float *b, float *c );
extern void dIntersect2f( float *output, float *lineA0, float *lineA1, float *lineB0, float *lineB1 );

/* Vector Math Double */
extern double dLength2d( double *vec );
extern double dLength3d( double *vec );
extern double dDistance2d( double *a, double *b );
extern double dDistance3d( double *a, double *b );
extern double dDot2d( double *a, double *b );
extern double dDot3d( double *a, double *b );
extern void dCross2d( double *output, double *a, double *b );
extern void dCross3d( double *output, double *a, double *b );
extern void dNormalize2d( double *vec );
extern void dNormalize3d( double *vec );
extern void dNormalize4d( double *vec );
extern void dNormal2d( double *output, double *a, double *b );
extern void dNormal3d( double *output, double *a, double *b, double *c );
extern void dIntersect2d( double *output, double *lineA0, double *lineA1, double *lineB0, double *lineB1 );

/* Vector Integer Math */
extern long dSqrti( int number );
extern boolean dNormalize2di( int *point, int multiplyer );
extern boolean dNormalize3di( int *point, int multiplyer );
extern void dIntersect2di( int *output, int *lineA0, int *lineA1,int *lineB0, int *lineB1 );

/* Matrix Operations */
extern void dMatrixClearf( float *matrix );
extern void dMatrixCleard( float *matrix );
extern void dMatrixTransform3f( float *output, const float *matrix, const float x, const float y, const float z );
extern void dMatrixTransform3d( double *output, const double *matrix, const double x, const double y, const double z );
extern void dMatrixTransform4f( float *output, const float *matrix, const float x, const float y, const float z, const float w );
extern void dMatrixTransform4d( double *output, const double *matrix, const double x, const double y, const double z, const double w );
extern void dMatrixMultiplyf( float *output, const float *a, const float *b );
extern void dMatrixMultiplyd( double *output, const double *a, const double *b );
extern void dMatrixInversef( float *output, const float *matrix );
extern void dMatrixInversed( double* output, const double *matrix );

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
