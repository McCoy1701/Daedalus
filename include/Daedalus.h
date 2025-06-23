#include <stdlib.h>
#include <time.h>

#ifndef __DAEDALUS_H__
#define __DAEDALUS_H__

#define MAX_LINE_LENGTH        1024
#define MAX_FILENAME_LENGTH    256
#define MAX_NAME_LENGTH        32
#define MAX_DESCRIPTION_LENGTH 256
#define MAX_INPUT_LENGTH       16

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define STRCPY( dest, src ) strncpy( dest, src, sizeof( dest ) ); dest[sizeof(dest) - 1] = '\0'
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
} dArray_t;

typedef struct {    // dString_t
    char* str;      // The actual string buffer
    size_t alloced; // Total allocated memory
    size_t len;     // Current string length
} dString_t;

/* Vector Math Float */
float d_Sqrtf( float number ); //Quake fast inverse square root

float d_LengthOfVec2f( const dVec2_t vec ); //Length of a vector 2f
float d_DistanceVec2f( const dVec2_t a, const dVec2_t b ); //Distance between two vector 2fs
float d_DotProductVec2f( const dVec2_t a, const dVec2_t b ); //Dot product between two vector 2fs
float d_CrossProductVec2f( const dVec2_t a, const dVec2_t b ); //Cross product of two vector 2fs
void  d_AddTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Add two vector 2fs
void  d_SubTwoVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Subtract two vector 2fs
void  d_GetVec2fFromAnglef( dVec2_t *output, float angle );
void  d_ScaleMultiplyVec2f( dVec2_t *output, const dVec2_t vec, const float value ); //Multiply a vector 2f by a value
void  d_ScaleDivideVec2f( dVec2_t *output, const dVec2_t vec, const float value ); //Divide a vector 2f by a value
void  d_LimitVec2f( dVec2_t *output, const dVec2_t a, const float value ); //Limit a vector 2f within a range
void  d_NormalizeVec2f( dVec2_t *output, const dVec2_t vec ); //Normalize a vector 2f
void  d_CreateNormalVec2f( dVec2_t *output, const dVec2_t a, const dVec2_t b ); //Create a normal vector from two vector 2fs
void  d_FindIntersectionVec2f( dVec2_t *output, const dVec2_t lineA0, const dVec2_t lineA1, const dVec2_t lineB0, const dVec2_t lineB1 ); //Find the intersection between two vector 2fs

float d_LengthOfVec3f( const dVec3_t vec ); //Length of a vector 3f
float d_DistanceVec3f( const dVec3_t a, const dVec3_t b ); //Distance between two vector 3fs
float d_DotProductVec3f( const dVec3_t a, const dVec3_t b ); //Dot product between two vector 3fs
void  d_CrossProductVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Cross product of two vector 3fs
void  d_AddTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Add two vector 3fs
void  d_SubTwoVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b ); //Subtract two vector 3fs
void  d_ScaleMultiplyVec3f( dVec3_t *output, const dVec3_t vec, const float value ); //Multiply a vector 3f by a value
void  d_ScaleDivideVec3f( dVec3_t *output, const dVec3_t vec, const float value ); //Divide a vector 3f by a value
void  d_LimitVec3f( dVec3_t *output, const dVec3_t a, const float value ); //Limit a vector 3f within a range
void  d_NormalizeVec3f( dVec3_t *output, const dVec3_t vec ); //Normalize a vector 3f
void  d_CreateNormalVec3f( dVec3_t *output, const dVec3_t a, const dVec3_t b, const dVec3_t c ); //Create a normal vector from two vector 3fs
void  d_NormalizeVec4f( dVec4_t *output, const dVec4_t vec );

/* Matrix Operations */
void d_MatrixClearf( dMat4x4_t *matrix ); //Clear a 4x4 matrix to an identity matrix
void d_MatrixRotateXf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the x axis
void d_MatrixRotateYf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the y axis
void d_MatrixRotateZf( dMat4x4_t *matrix, const float angle_rad ); //Rotate matrix by angle in radians about the z axis
void d_MatrixCreateProjectionf( dMat4x4_t *matrix, const float aspect_ratio, const float fov, const float near, const float far ); //Create a projection matrix
void d_MatrixMultiplyf( dMat4x4_t *output, const dMat4x4_t a, const dMat4x4_t b ); //Multiply two 4x4 Matrices together
void d_MatrixTranslateVec3f( dMat4x4_t *matrix, const dVec3_t vec ); //Translate a matrix by a 3D point
void d_MatrixTranslateVec4f( dMat4x4_t *matrix, const dVec4_t vec ); //Translate a matrix by a 4D point
void d_MatrixMultiplyVec3f( dVec3_t *output, dMat4x4_t matrix, const dVec3_t vec ); //Transform a 3D point into a 4x4 matrix

void d_MatrixInverseTransformVec3f( dVec3_t *output, const dMat4x4_t matrix, dVec3_t vec ); //Inverse transform 3D point into a 4x4 matrix
void d_MatrixMultiplyVec4f( dVec4_t *output, const dMat4x4_t matrix, const dVec4_t vec4 ); //Transform a 4D point into a 4x4 matrix

/* Matrix Creation */
void d_MatrixXYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixXZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixYXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixYZf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixZXf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );
void d_MatrixZYf( dMat4x4_t output, const dVec3_t origin, const dVec3_t point0, const dVec3_t point1 );

/* Linked List */
dLinkedList_t *d_CreateLinkedList( void *data, char *name, size_t size );
void *d_GetDataInLinkedListByIndex( dLinkedList_t *head, int index );
void *d_GetDataInLinkedListByName( dLinkedList_t *head, char *name );
void d_RemoveNodeInLinkedListByIndex( dLinkedList_t *head, int index );
void d_RemoveNodeInLinkedListByName( dLinkedList_t *head, char *name );
void d_ClearLinkedList( dLinkedList_t *head );
void d_PushBack( dLinkedList_t *head, void *data, char *name, size_t size );
void d_PushFront( dLinkedList_t **head, void *data, char *name, size_t size );
void *d_PopBack( dLinkedList_t *head );
void *d_PopFront( dLinkedList_t **head );
void d_PrintLinkedList( dLinkedList_t *head );
int  d_GetLengthOfLinkedList( dLinkedList_t *head );

/* Quad Tree */
dQuadTree_t *d_CreateQuadtree( float *rect, int capacity );
void d_InsertObjectInQuadtree( dQuadTree_t *tree, void *object );
void d_SubdivideQuadtree( dQuadTree_t *tree );

/* Kinematic Body 2D  */
void d_CreateKinmaticBody( dKinematicBody_t *output, const dVec2_t position, const dVec2_t velocity, const dVec2_t acceleration, const float mass );

/* Strings */
char* d_CreateStringFromFile(const char *filename);
/*
 * Create a new string builder
 *
 * `dString_t*` - Pointer to new string builder, or NULL on allocation failure
 *
 * -- Must be destroyed with d_DestroyString() to free memory
 * -- Initial capacity is 32 bytes but will grow automatically
 */
dString_t *d_InitString(void);
/*
 * Destroy a string builder and free its memory
 *
 * `sb` - Pointer to string builder to destroy
 *
 * -- After calling this function, the pointer is invalid and should not be used
 * -- Calling with NULL is safe and does nothing
 */
void d_DestroyString(dString_t* sb);
/*
 * Add a string to the string builder
 *
 * `sb` - Pointer to string builder
 * `str` - String to append (must be null-terminated if len is 0)
 * `len` - Length of string to append, or 0 to use strlen()
 *
 * -- If len is 0, strlen() is called to determine the length
 * -- If len > 0, exactly len characters are copied (partial strings allowed)
 * -- Does nothing if sb or str is NULL, or if str is empty
 */
void d_AppendString(dString_t* sb, const char* str, size_t len);
/*
 * Add a single character to the string builder
 *
 * `sb` - Pointer to string builder
 * `c` - Character to append
 *
 * -- Does nothing if sb is NULL
 * -- Can append any character including '\0' (though this may confuse string functions)
 */
void d_AppendChar(dString_t* sb, char c);
/*
 * Add an integer to the string builder as a decimal string
 *
 * `sb` - Pointer to string builder
 * `val` - Integer value to append
 *
 * -- Does nothing if sb is NULL
 * -- Uses snprintf internally for safe conversion
 * -- Supports full 32-bit integer range including negative values
 */
void d_AppendInt(dString_t* sb, int val);
/*
 * Append a floating-point number to the string builder
 *
 * `sb` - Pointer to string builder
 * `val` - Float value to append
 * `decimals` - Number of decimal places to show (0-10)
 *
 * -- Does nothing if sb is NULL
 * -- If decimals is negative, uses 6 decimal places (default)
 * -- If decimals > 10, caps at 10 decimal places
 * -- Uses standard printf formatting for floating-point representation
 */
void d_AppendFloat(dString_t* sb, float val, int decimals);
/*
 * Clear the string builder content
 *
 * `sb` - Pointer to string builder
 *
 * -- Does nothing if sb is NULL
 * -- Memory is not freed, only the length is reset to 0
 * -- Buffer capacity remains unchanged for efficient reuse
 */
void d_ClearString(dString_t* sb);
/*
 * Truncate the string builder to a specific length
 *
 * `sb` - Pointer to string builder
 * `len` - New length (must be <= current length)
 *
 * -- Does nothing if sb is NULL or len >= current length
 * -- Memory is not freed, only the length is reduced
 * -- The string remains null-terminated at the new length
 */
void d_TruncateString(dString_t* sb, size_t len);
/*
 * Remove characters from the beginning of the string builder
 *
 * `sb` - Pointer to string builder
 * `len` - Number of characters to remove from the beginning
 *
 * -- Does nothing if sb is NULL or len is 0
 * -- If len >= current length, the string builder is cleared completely
 * -- Uses memmove for safe overlapping memory operation
 */
void d_DropString(dString_t* sb, size_t len);
/*
 * Get the current length of the string builder content
 *
 * `sb` - Pointer to string builder
 *
 * `size_t` - Current length in characters, or 0 if sb is NULL
 *
 * -- Return value does not include the null terminator
 * -- Safe to call with NULL pointer (returns 0)
 */
size_t d_GetStringLength(const dString_t* sb);
/*
 * Get a read-only pointer to the string builder's content
 *
 * `sb` - Pointer to string builder
 *
 * `const char*` - Pointer to internal string, or NULL if sb is NULL
 *
 * -- Do not modify the returned string or free the pointer
 * -- The pointer becomes invalid after any modification to the string builder
 * -- The returned string is always null-terminated
 * -- Safe to call with NULL pointer (returns NULL)
 */
const char* d_PeekString(const dString_t* sb);
/*
 * Create a copy of the string builder's content
 *
 * `sb` - Pointer to string builder
 * `len` - Optional pointer to receive the length of the returned string
 *
 * `char*` - Newly allocated copy of the string, or NULL on error
 *
 * -- The caller is responsible for freeing the returned pointer
 * -- If len is not NULL, it receives the length of the string (excluding null terminator)
 * -- Returns NULL if sb is NULL or memory allocation fails
 * -- The returned string is always null-terminated
 */
char *d_DumpString(const dString_t* sb, size_t* len);
/*
 * Add formatted text to the string builder using printf-style formatting
 *
 * `sb` - Pointer to string builder
 * `format` - Printf-style format string
 * `...` - Variable arguments corresponding to format specifiers
 *
 * -- Does nothing if sb or format is NULL
 * -- Uses vsnprintf internally for safe formatting
 * -- Supports all standard printf format specifiers (%d, %s, %f, etc.)
 * -- Automatically calculates required space and grows buffer as needed
 * -- Appends formatted text to existing content (does not replace)
 */
void d_FormatString(dString_t* sb, const char* format, ...);
/*
 * Add text with template substitution to the string builder
 *
 * `sb` - Pointer to string builder
 * `tmplt` - Template string with placeholders in {key} format
 * `keys` - Array of key strings to match against placeholders
 * `values` - Array of replacement values corresponding to keys
 * `count` - Number of key-value pairs
 *
 * -- Does nothing if sb or tmplt is NULL
 * -- Placeholders must be in format {keyname} with no spaces
 * -- Keys are matched exactly (case-sensitive)
 * -- If a placeholder has no matching key, it is left unchanged
 * -- Keys longer than 255 characters are treated as literal text
 * -- Supports nested braces by treating unmatched { as literal characters
 */
void d_TemplateString(dString_t* sb, const char* tmplt, const char** keys, const char** values, int count);
/*
 * Add an ASCII progress bar to the string builder
 *
 * `sb` - Pointer to string builder
 * `current` - Current progress value
 * `max` - Maximum progress value
 * `width` - Width of the progress bar (excluding brackets)
 * `fill_char` - Character to use for filled portions
 * `empty_char` - Character to use for empty portions
 *
 * -- Does nothing if sb is NULL, width <= 0, or max <= 0
 * -- Progress bar format: [████████----] where █ is fill_char and - is empty_char
 * -- If current > max, the bar is filled completely
 * -- If current < 0, the bar is empty
 * -- Total visual width is width + 2 (for brackets)
 */
void d_AppendProgressBar(dString_t* sb, int current, int max, int width, char fill_char, char empty_char);
/*
  * Add text padded to the center with specified character to reach target width
  *
  * `sb` - Pointer to string builder
  * `text` - Text to center (must be null-terminated)
  * `width` - Target total width including padding
  * `pad_char` - Character to use for padding
  *
  * -- Does nothing if sb or text is NULL
  * -- If text length >= width, adds text without padding
  * -- Text is centered with padding distributed evenly on both sides
  * -- If padding cannot be evenly distributed, left side gets one less character
  * -- Example: d_PadCenterString(sb, "Hi", 6, '.') produces "..Hi.."
  * -- Example: d_PadCenterString(sb, "Hi", 7, '.') produces "..Hi..."
  * -- Commonly used for centered headers and titles in ASCII interfaces
  */
void d_PadCenterString(dString_t* sb, const char* text, int width, char pad_char);
/*
  * Add text padded to the left with specified character to reach target width
  *
  * `sb` - Pointer to string builder
  * `text` - Text to pad (must be null-terminated)
  * `width` - Target total width including padding
  * `pad_char` - Character to use for padding
  *
  * -- Does nothing if sb or text is NULL
  * -- If text length >= width, adds text without padding
  * -- Padding is added to the left side of the text
  * -- Example: d_PadLeftString(sb, "Hi", 5, '.') produces "...Hi"
  * -- Commonly used for right-aligned text in tables and menus
  */
void d_PadLeftString(dString_t* sb, const char* text, int width, char pad_char);
/*
  * Add text padded to the right with specified character to reach target width
  *
  * `sb` - Pointer to string builder
  * `text` - Text to pad (must be null-terminated)
  * `width` - Target total width including padding
  * `pad_char` - Character to use for padding
  *
  * -- Does nothing if sb or text is NULL
  * -- If text length >= width, adds text without padding
  * -- Padding is added to the right side of the text
  * -- Example: d_PadRightString(sb, "Hi", 5, '.') produces "Hi..."
  * -- Commonly used for left-aligned text in tables and menus
  */
void d_PadRightString(dString_t* sb, const char* text, int width, char pad_char);
/*
 * Add repeated characters to the string builder
 *
 * `sb` - Pointer to string builder
 * `character` - Character to repeat
 * `count` - Number of times to repeat the character
 *
 * -- Does nothing if sb is NULL or count <= 0
 * -- Efficiently adds multiple copies of the same character
 * -- Used internally by progress bar and padding functions
 */
void d_RepeatString(dString_t* sb, char character, int count);

// Pythonic String Utils
/*
  * Join an array of strings with a separator (like Python's str.join())
  *
  * `sb` - Pointer to string builder
  * `strings` - Array of string pointers to join
  * `count` - Number of strings in the array
  * `separator` - String to insert between each element
  *
  * -- Does nothing if sb is NULL or count <= 0
  * -- If strings array is NULL, does nothing
  * -- NULL strings in the array are treated as empty strings
  * -- If separator is NULL, strings are joined without separation
  * -- Example: d_JoinStrings(sb, {"a", "b", "c"}, 3, ", ") produces "a, b, c"
  * -- Commonly used for creating comma-separated lists, file paths, etc.
  */
void d_JoinStrings(dString_t* sb, const char** strings, int count, const char* separator);
/*
  * Split a string by delimiter into an array of strings (like Python's str.split())
  *
  * `text` - String to split (must be null-terminated)
  * `delimiter` - Delimiter string to split by
  * `count` - Pointer to receive the number of resulting strings
  *
  * `char**` - Array of newly allocated strings, or NULL on error
  *
  * -- Returns NULL if text or delimiter is NULL, or if allocation fails
  * -- Caller must free the result using d_FreeSplitString()
  * -- Each string in the result array is individually allocated
  * -- Empty strings between delimiters are included in the result
  * -- If delimiter is not found, returns array with single copy of original string
  * -- *count is set to the number of strings in the returned array
  */
char** d_SplitString(const char* text, const char* delimiter, int* count);
/*
  * Free memory allocated by d_SplitString()
  *
  * `result` - Array of strings returned by d_SplitString()
  * `count` - Number of strings in the array
  *
  * -- Safe to call with NULL result pointer
  * -- Frees each individual string and then the array itself
  * -- Must be called for every successful d_SplitString() call to prevent memory leaks
  */
void d_FreeSplitString(char** result, int count);
/*
  * Extract a substring using Python-style slice notation (like str[start:end])
  *
  * `sb` - Pointer to string builder
  * `text` - Source string to slice (must be null-terminated)
  * `start` - Starting index (inclusive, negative values count from end)
  * `end` - Ending index (exclusive, negative values count from end, -1 means end of string)
  *
  * -- Does nothing if sb or text is NULL
  * -- Negative indices count from the end: -1 is last character, -2 is second-to-last, etc.
  * -- If start >= end (after resolving negative indices), no text is added
  * -- Indices are clamped to valid range [0, string_length]
  * -- Example: d_SliceString(sb, "Hello", 1, 4) produces "ell"
  * -- Example: d_SliceString(sb, "Hello", -3, -1) produces "llo"
  * -- Example: d_SliceString(sb, "Hello", 0, -1) produces "Hello"
  */
void d_SliceString(dString_t* sb, const char* text, int start, int end);
/* Dynamic Arrays */
dArray_t* d_InitArray( size_t capacity, size_t element_size );
void d_AppendArray( dArray_t* array, void* data );
void* d_GetDataFromArrayByIndex( dArray_t* array, size_t count );
void* d_PopDataFromArray( dArray_t* array );
int d_ResizeArray( dArray_t* array, size_t new_capacity );
void d_DestroyArray( dArray_t* array );

#endif
