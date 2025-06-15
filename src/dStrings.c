// File: src/dStrings.c - String utilities for Daedalus project

#include "../include/Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* d_CreateStringFromFile(const char *filename)
{
  long fileSize;
  char *fileString;
  FILE *file;

  file = fopen( filename, "r" );
  if ( file == NULL )
  {
    printf( "Error loading file: %s\n", filename );
    return NULL;
  }

  fseek( file, 0, SEEK_END );
  fileSize = ftell( file );
  rewind( file );

  fileString= ( char* )malloc( fileSize + 1 );
  if ( fileString == NULL )
  {
    printf( "Error allocating memory for file string: %s\n", filename );
    fclose( file );
    return NULL;
  }

  if ( fread( fileString, fileSize, 1, file ) != 1 )
  {
    printf("Failed to read vertex file: %s\n", filename);
    fclose(file);
    free(fileString);
    return NULL;
  }

  fclose(file);

  fileString[fileSize] = '\0';

  return fileString;
}

// =====================
// _dString_t
// =====================

static const size_t d_string_builder_min_size = 32;
/*
 * Ensure the string builder has enough space for additional data
 *
 * `sb` - Pointer to string builder
 * `add_len` - Number of bytes to add (not including null terminator)
 *
 * -- Internal function that grows the buffer using doubling strategy
 * -- Buffer is always kept null-terminated
 */
static void d_string_builder_ensure_space(_dString_t* sb, size_t add_len)
{
    if (sb == NULL || add_len == 0)
        return;

    if (sb->alloced >= sb->len + add_len + 1)
        return;

    while (sb->alloced < sb->len + add_len + 1) {
        /* Doubling growth strategy. */
        sb->alloced <<= 1;
        if (sb->alloced == 0) {
            /* Left shift of max bits will go to 0. An unsigned type set to
             * -1 will return the maximum possible size. However, we should
             * have run out of memory well before we need to do this. Since
             * this is the theoretical maximum total system memory we don't
             * have a flag saying we can't grow any more because it should
             * be impossible to get to this point. */
            sb->alloced--;
        }
    }
    sb->str = realloc(sb->str, sb->alloced);
}
/*
 * Create a new string builder
 *
 * `_dString_t*` - Pointer to new string builder, or NULL on allocation failure
 *
 * -- Must be destroyed with d_StringDestroy() to free memory
 * -- Initial capacity is 32 bytes but will grow automatically
 */
_dString_t* d_StringCreate(void)
{
    _dString_t* sb;

    sb = calloc(1, sizeof(*sb));
    if (sb == NULL)
        return NULL;

    sb->str = malloc(d_string_builder_min_size);
    if (sb->str == NULL) {
        free(sb);
        return NULL;
    }

    *sb->str = '\0';
    sb->alloced = d_string_builder_min_size;
    sb->len = 0;

    return sb;
}
/*
 * Destroy a string builder and free its memory
 *
 * `sb` - Pointer to string builder to destroy
 *
 * -- After calling this function, the pointer is invalid and should not be used
 * -- Calling with NULL is safe and does nothing
 */
void d_StringDestroy(_dString_t* sb)
{
    if (sb == NULL)
        return;
    free(sb->str);
    free(sb);
}
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
void d_StringAddStr(_dString_t* sb, const char* str, size_t len)
{
    if (sb == NULL || str == NULL || *str == '\0')
        return;

    if (len == 0)
        len = strlen(str);

    d_string_builder_ensure_space(sb, len);
    memmove(sb->str + sb->len, str, len);
    sb->len += len;
    sb->str[sb->len] = '\0';
}
/*
 * Add a single character to the string builder
 *
 * `sb` - Pointer to string builder
 * `c` - Character to append
 *
 * -- Does nothing if sb is NULL
 * -- Can append any character including '\0' (though this may confuse string functions)
 */
void d_StringAddChar(_dString_t* sb, char c)
{
    if (sb == NULL)
        return;
    d_string_builder_ensure_space(sb, 1);
    sb->str[sb->len] = c;
    sb->len++;
    sb->str[sb->len] = '\0';
}
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
void d_StringAddInt(_dString_t* sb, int val)
{
    char str[12]; // Enough for 32-bit int plus sign and null terminator

    if (sb == NULL)
        return;

    snprintf(str, sizeof(str), "%d", val);
    d_StringAddStr(sb, str, 0);
}
/*
 * Clear the string builder content
 *
 * `sb` - Pointer to string builder
 *
 * -- Does nothing if sb is NULL
 * -- Memory is not freed, only the length is reset to 0
 * -- Buffer capacity remains unchanged for efficient reuse
 */
void d_StringClear(_dString_t* sb)
{
    if (sb == NULL)
        return;
    d_StringTruncate(sb, 0);
}
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
void d_StringTruncate(_dString_t* sb, size_t len)
{
    if (sb == NULL || len >= sb->len)
        return;

    sb->len = len;
    sb->str[sb->len] = '\0';
}
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
void d_StringDrop(_dString_t* sb, size_t len)
{
    if (sb == NULL || len == 0)
        return;

    if (len >= sb->len) {
        d_StringClear(sb);
        return;
    }

    sb->len -= len;
    /* +1 to move the NULL terminator. */
    memmove(sb->str, sb->str + len, sb->len + 1);
}
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
size_t d_StringLen(const _dString_t* sb)
{
    if (sb == NULL)
        return 0;
    return sb->len;
}
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
const char* d_StringPeek(const _dString_t* sb)
{
    if (sb == NULL)
        return NULL;
    return sb->str;
}
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
char* d_StringDump(const _dString_t* sb, size_t* len)
{
    char* out;

    if (sb == NULL)
        return NULL;

    if (len != NULL)
        *len = sb->len;
    out = malloc(sb->len + 1);
    if (out == NULL)
        return NULL;
    memcpy(out, sb->str, sb->len + 1);
    return out;
}
