#ifndef __TEST_H__
#define __TEST_H__

extern int test_dSqrt( void );

extern int test_dLength2f( void );
extern int test_dLength3f( void );
extern int test_dDistance2f( void );
extern int test_dDistance3f( void );
extern int test_dDot2f( void );
extern int test_dDot3f( void );
extern int test_dCross2f( void );
extern int test_dCross3f( void );
extern int test_dNormalize2f( void );
extern int test_dNormalize3f( void );
extern int test_dNormalize4f( void );
extern int test_dNormal2f( void );
extern int test_dNormal3f( void );
extern int test_dIntersect2f( void );

/* Vector Math Double */
extern int test_dLength2d( void );
extern int test_dLength3d( void );
extern int test_dDistance2d( void );
extern int test_dDistance3d( void );
extern int test_dDot2d( void );
extern int test_dDot3d( void );
extern int test_dCross2d( void );
extern int test_dCross3d( void );
extern int test_dNormalize2d( void );
extern int test_dNormalize3d( void );
extern int test_dNormalize4d( void );
extern int test_dNormal2d( void );
extern int test_dNormal3d( void );
extern int test_dIntersect2d( void );

/* Vector Integer Math */
extern int test_dSqrti( void );
extern int test_dNormalize2di( void );
extern int test_dNormalize3di( void );
extern int test_dIntersect2di( void );

/* Matrix Operations */
extern int test_dMatrixClearf( void );
extern int test_dMatrixCleard( void );
extern int test_dMatrixTransform3f( void );
extern int test_dMatrixTransform3d( void );
extern int test_dMatrixTransform4f( void );
extern int test_dMatrixTransform4d( void );
extern int test_dMatrixMultiplyf( void );
extern int test_dMatrixMultiplyd( void );
extern int test_dMatrixInversef( void );
extern int test_dMatrixInversed( void );

/* Matrix Creation */
extern int test_dMatrixXYf( void );
extern int test_dMatrixXZf( void );
extern int test_dMatrixYXf( void );
extern int test_dMatrixYZf( void );
extern int test_dMatrixZXf( void );
extern int test_dMatrixZYf( void );
extern int test_dMatrixXYd( void );
extern int test_dMatrixXZd( void );
extern int test_dMatrixYXd( void );
extern int test_dMatrixYZd( void );
extern int test_dMatrixZXd( void );
extern int test_dMatrixZYd( void );

#endif
