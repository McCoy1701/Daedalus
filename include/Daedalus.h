#ifndef __DAEDALUS_H__
#define __DAEDALUS_H__

#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LENGTH     1024
#define MAX_FILENAME_LENGTH 256

typedef struct _dDeltaTime_t
{
  clock_t lastTime;
  clock_t currentTime;
} dDeltaTime_t;

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
  float x, y, z, w;
} dVec4_t;

typedef struct _dMat4x4_t
{
  float matrix [16];
} dMat4x4_t;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define STRNCPY(dest, src, n) strncpy(dest, src, n); dest[n - 1] = '\0'

#define RANDF(lower, upper) (((float)rand() / (float)(RAND_MAX)) * (upper - lower)) + lower

#define MAP( value, start0, start1, end0, end1 ) ( ( value - start0 ) * ( ( end1 - end0 ) / ( start1 - start0 ) ) + end0 )

#define PI 3.14159265

#define d_sqrt sqrtf

/* Vector Math Float */
extern float d_sqrt( float number ); //Quake fast inverse square root

extern float d_length_of_dVec2f( const dVec2_t vec ); //Lenght of a vector 2f
extern float d_distance_dVec2f( const dVec2_t a, const dVec2_t b ); //Distance between two vector 2fs
extern float d_dot_product_dVec2f( const dVec2_t a, const dVec2_t b ); //Dot product between two vector 2fs
extern float d_cross_product_dVec2f( const dVec2_t a, const dVec2_t b ); //Cross product of two vector 2fs
extern void  d_add_two_dVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Add two vector 2fs
extern void  d_sub_two_dVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Subtract two vector 2fs
extern void  d_get_dVec2f_from_anglef( dVec2_t *output, float angle );
extern void  d_scale_multiply_dVec2f( dVec2_t *output, const float value ); //Multiply a vector 2f by a value
extern void  d_scale_divide_dVec2f( dVec2_t *output, const float value ); //Divide a vector 2f by a value
extern void  d_limit_dVec2f( dVec2_t *a, const float value ); //Limit a vector 2f within a range
extern void  d_normalize_dVec2f( dVec2_t *vec ); //Normalize a vector 2f
extern void  d_create_normal_dVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Create a normal vector from two vector 2fs
extern void  d_find_intersection_dVec2f( dVec2_t *output, const dVec2_t lineA0, const dVec2_t lineA1, const dVec2_t lineB0, const dVec2_t lineB1 ); //Find the intersection between two vector 2fs

extern float d_length_of_dVec3f( const dVec3_t vec ); //Lenght of a vector 3f
extern float d_distance_dVec3f( const dVec3_t a, const dVec3_t b ); //Distance between two vector 3fs
extern float d_dot_product_dVec3f( const dVec3_t a, const dVec3_t b ); //Dot product between two vector 3fs
extern void  d_cross_product_dVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Cross product of two vector 3fs
extern void  d_add_two_dVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Add two vector 3fs
extern void  d_sub_two_dVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Subtract two vector 3fs
extern void  d_scale_multiply_dVec3f( dVec3_t *output, const float value ); //Multiply a vector 3f by a value
extern void  d_scale_divide_dVec3f( dVec3_t *output, const float value ); //Divide a vector 3f by a value
extern void  d_limit_dVec3f( dVec3_t *a, const float value ); //Limit a vector 3f within a range
extern void  d_normalize_dVec3f( dVec3_t *vec ); //Normalize a vector 3f
extern void  d_create_normal_dVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b, const dVec3_t c ); //Create a normal vector from two vector 3fs
extern void  d_normalize_dVec4f( dVec4_t *vec ); //Normalize a vector 4f

/* Matrix Operations */
extern void d_matrix_clearf( dMat4x4_t *matrix ); //Clear a 4x4 matrix to an identity matrix
extern void d_matrix_rotate_xf( dMat4x4_t *matrix, const float angleRad ); //Rotate matrix by angle in radians about the x axis
extern void d_matrix_rotate_yf( dMat4x4_t *matrix, const float angleRad ); //Rotate matrix by angle in radians about the y axis
extern void d_matrix_rotate_zf( dMat4x4_t *matrix, const float angleRad ); //Rotate matrix by angle in radians about the z axis
extern void d_matrix_create_projectionf( dMat4x4_t *matrix, const float aspectRatio, const float fov, const float near, const float far ); //Create a projection matrix 
extern void d_matrix_multiplyf( dMat4x4_t *output, const dMat4x4_t a, const dMat4x4_t b ); //Multiply two 4x4 Matrices together
extern void d_matrix_translate_dVec3f( dMat4x4_t *matrix, const dVec3_t vec ); //Translate a matrix by a 3D point
extern void d_matrix_translate_dVec4f( dMat4x4_t *matrix, const dVec4_t vec ); //Translate a matrix by a 4D point
extern void d_matrix_multiply_dVec3f( dVec3_t *output, const dMat4x4_t matrix, const dVec3_t vec ); //Transform a 3D point into a 4x4 matrix 
extern void d_matrix_inverse_transform_dVec3f( dVec3_t *output, const dMat4x4_t matrix, dVec3_t vec ); //Inverse transform 3D point into a 4x4 matrix
extern void d_matrix_multiply_dVec4f( dVec4_t *output, const dMat4x4_t matrix, const dVec4_t vec4 ); //Transform a 4D point into a 4x4 matrix

/* Matrix Creation */
extern void d_matrix_XYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrix_XZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrix_YXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrix_YZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrix_ZXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
extern void d_matrix_ZYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );

/* Delta Time */
extern double d_get_delta_time( dDeltaTime_t *time );

/* Linked List */

extern dLinkedList_t *d_create_linked_list( void *data, char *name, size_t size );
extern void *d_get_data_in_linked_list_by_index( dLinkedList_t *head, int index );
extern void *d_get_data_in_linked_list_by_name( dLinkedList_t *head, char *name );
extern void d_remove_node_in_linked_list_by_index( dLinkedList_t *head, int index );
extern void d_remove_node_in_linked_list_by_name( dLinkedList_t *head, char *name );
extern void d_clear_linked_list( dLinkedList_t *head );
extern void d_push_back( dLinkedList_t *head, void *data, char *name, size_t size );
extern void d_push_front( dLinkedList_t **head, void *data, char *name, size_t size );
extern void *d_pop_back( dLinkedList_t *head );
extern void *d_pop_front( dLinkedList_t **head );
extern void d_print_linked_list( dLinkedList_t *head );
extern int  d_get_length_of_linked_list( dLinkedList_t *head );

/* Quad Tree */

extern dQuadTree_t *d_create_quadtree( float *rect, int capacity );
extern void d_insert_object_in_quadtree( dQuadTree_t *tree, void *object );
extern void d_subdivide_quadtree( dQuadTree_t *tree );

#endif
