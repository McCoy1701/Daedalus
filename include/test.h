#ifndef __TEST_H__
#define __TEST_H__

extern int test_dynamicArray( void );
extern int test_kinematicBody( void );
extern int test_linkedList( void );
extern int test_matrixCreation( void );
extern int test_matrixMath( void );
extern int test_quadTree( void );
extern int test_strings( void );
extern int test_vectorMath( void );

/* Vector Math Float */
extern int test_d_sqrtf( void );

extern int test_d_lengthOfVec2f( void );
extern int test_d_distanceVec2f( void );
extern int test_d_dotProductVec2f( void );
extern int test_d_crossProductVec2f( void );
extern int test_d_addTwoVec2f( void );
extern int test_d_subTwoVec2f( void );
extern int test_d_getVec2fFromAnglef( void );
extern int test_d_scaleMultiplyVec2f( void );
extern int test_d_scaleDivideVec2f( void );
extern int test_d_limitVec2f( void );
extern int test_d_normalizeVec2f( void );
extern int test_d_createNormalVec2f( void );
extern int test_d_findIntersectionVec2f( void );

extern int test_d_lengthOfVec3f( void );
extern int test_d_distanceVec3f( void );
extern int test_d_dotProductVec3f( void );
extern int test_d_crossProductVec3f( void );
extern int test_d_addTwoVec3f( void );
extern int test_d_subTwoVec3f( void );
extern int test_d_scaleMultiplyVec3f( void );
extern int test_d_scaleDivideVec3f( void );
extern int test_d_limitVec3f( void );
extern int test_d_normalizeVec3f( void );
extern int test_d_createNormalVec3f( void );
extern int test_d_normalizeVec4f( void );

/* Matrix Operations */
extern int test_d_matrixClearf( void );
extern int test_d_matrixRotateXf( void );
extern int test_d_matrixRotateYf( void );
extern int test_d_matrixRotateZf( void );
extern int test_d_matrixCreateProjectionf( void );
extern int test_d_matrixMultiplyf( void );
extern int test_d_matrixTranslateVec3f( void );
extern int test_d_matrixTranslateVec4f( void );
extern int test_d_matrixMultiplyVec3f( void );
extern int test_d_matrixInverseTransformVec3f( void );
extern int test_d_matrixMultiplyVec4f( void );

/* Matrix Creation */
extern int test_d_matrixXYf( void );
extern int test_d_matrixXZf( void );
extern int test_d_matrixYXf( void );
extern int test_d_matrixYZf( void );
extern int test_d_matrixZXf( void );
extern int test_d_matrixZYf( void );

/* Linked List */
extern int test_d_createLinkedList( void );
extern int test_d_getDataInLinkedListByIndex( void );
extern int test_d_getDataInLinkedListByName( void );
extern int test_d_removeNodeInLinkedListByIndex( void );
extern int test_d_removeNodeInLinkedListByName( void );
extern int test_d_clearLinkedList( void );
extern int test_d_pushBack( void );
extern int test_d_pushFront( void );
extern int test_d_popBack( void );
extern int test_d_popFront( void );
extern int test_d_printLinkedList( void );
extern int test_d_getLengthOfLinkedList( void );

/* Quad Tree */
extern int test_d_createQuadtree( void );
extern int test_d_insertObjectInQuadtree( void );
extern int test_d_subdivideQuadtree( void );

/* Kinematic Body 2D  */
extern int test_d_createKinmaticBody( void );

/* Strings */
extern int test_d_createStringFromFile( void );

/* Dynamic Arrays */
extern int test_d_initArray( void );
extern int test_d_appendArray( void );
extern int test_d_getDataFromArrayByIndex( void );
extern int test_d_popDataFromArray( void );
extern int test_d_resizeArray( void );
extern int test_d_destroyArray( void );

#endif

