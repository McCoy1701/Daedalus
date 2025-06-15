// File: src/dStrings.c - String utilities for Daedalus project

#include "../include/Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
// dString_t
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
static void d_string_builder_ensure_space(dString_t* sb, size_t add_len)
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
 * `dString_t*` - Pointer to new string builder, or NULL on allocation failure
 *
 * -- Must be destroyed with d_StringDestroy() to free memory
 * -- Initial capacity is 32 bytes but will grow automatically
 */
dString_t* d_StringCreate(void)
{
    dString_t* sb;

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
void d_StringDestroy(dString_t* sb)
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
void d_StringAddStr(dString_t* sb, const char* str, size_t len)
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
void d_StringAddChar(dString_t* sb, char c)
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
void d_StringAddInt(dString_t* sb, int val)
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
void d_StringClear(dString_t* sb)
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
void d_StringTruncate(dString_t* sb, size_t len)
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
void d_StringDrop(dString_t* sb, size_t len)
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
size_t d_StringLen(const dString_t* sb)
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
const char* d_StringPeek(const dString_t* sb)
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
char* d_StringDump(const dString_t* sb, size_t* len)
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
void d_StringFormat(dString_t* sb, const char* format, ...) {
    if (sb == NULL || format == NULL) return;

    va_list args;
    va_start(args, format);

    // Calculate required size
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed < 0) {
        va_end(args);
        return;
    }

    // Ensure space and format
    d_string_builder_ensure_space(sb, needed);
    vsnprintf(sb->str + sb->len, needed + 1, format, args);
    sb->len += needed;

    va_end(args);
}
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
void d_StringRepeat(dString_t* sb, char character, int count) {
    if (sb == NULL || count <= 0) return;

    d_string_builder_ensure_space(sb, count);
    for (int i = 0; i < count; i++) {
        sb->str[sb->len + i] = character;
    }
    sb->len += count;
    sb->str[sb->len] = '\0';
}

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
void d_StringProgressBar(dString_t* sb, int current, int max, int width, char fill_char, char empty_char) {
    if (sb == NULL || width <= 0 || max <= 0) return;

    int filled = (current * width) / max;
    if (filled > width) filled = width;

    d_StringAddChar(sb, '[');
    d_StringRepeat(sb, fill_char, filled);
    d_StringRepeat(sb, empty_char, width - filled);
    d_StringAddChar(sb, ']');
}
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
 void d_StringTemplate(dString_t* sb, const char* tmplt, const char** keys, const char** values, int count) {
     if (sb == NULL || tmplt == NULL) return;

     const char* pos = tmplt;
     while (*pos) {
         if (*pos == '{') {
             const char* end = strchr(pos + 1, '}');
             if (end) {
                 // Extract key
                 int key_len = end - pos - 1;
                 char key[256];
                 if (key_len < 256 && key_len >= 0) {
                     strncpy(key, pos + 1, key_len);
                     key[key_len] = '\0';

                     // Find matching value - but only if keys and values are not NULL
                     int found = 0;
                     if (keys != NULL && values != NULL) {
                         for (int i = 0; i < count; i++) {
                             if (keys[i] != NULL && strcmp(keys[i], key) == 0) {
                                 if (values[i] != NULL) {
                                     d_StringAddStr(sb, values[i], 0);
                                 }
                                 found = 1;
                                 break;
                             }
                         }
                     }

                     if (!found) {
                         // Keep original placeholder if no match
                         d_StringAddStr(sb, pos, end - pos + 1);
                     }

                     pos = end + 1;
                 } else {
                     d_StringAddChar(sb, *pos++);
                     continue;
                 }
             } else {
                 d_StringAddChar(sb, *pos++);
             }
         } else {
             d_StringAddChar(sb, *pos++);
         }
     }
 }
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
  * -- Example: d_StringPadLeft(sb, "Hi", 5, '.') produces "...Hi"
  * -- Commonly used for right-aligned text in tables and menus
  */
 void d_StringPadLeft(dString_t* sb, const char* text, int width, char pad_char) {
     if (sb == NULL || text == NULL || width <= 0) return;

     int text_len = strlen(text);
     int pad_needed = width - text_len;

     if (pad_needed > 0) {
         d_StringRepeat(sb, pad_char, pad_needed);
     }
     d_StringAddStr(sb, text, 0);
 }

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
  * -- Example: d_StringPadRight(sb, "Hi", 5, '.') produces "Hi..."
  * -- Commonly used for left-aligned text in tables and menus
  */
 void d_StringPadRight(dString_t* sb, const char* text, int width, char pad_char) {
     if (sb == NULL || text == NULL || width <= 0) return;

     int text_len = strlen(text);
     int pad_needed = width - text_len;

     d_StringAddStr(sb, text, 0);
     if (pad_needed > 0) {
         d_StringRepeat(sb, pad_char, pad_needed);
     }
 }

 /*
  * Add text centered with specified padding character to reach target width
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
  * -- Example: d_StringPadCenter(sb, "Hi", 6, '.') produces "..Hi.."
  * -- Example: d_StringPadCenter(sb, "Hi", 7, '.') produces "..Hi..."
  * -- Commonly used for centered headers and titles in ASCII interfaces
  */
 void d_StringPadCenter(dString_t* sb, const char* text, int width, char pad_char) {
     if (sb == NULL || text == NULL || width <= 0) return;

     int text_len = strlen(text);
     int pad_needed = width - text_len;

     if (pad_needed <= 0) {
         d_StringAddStr(sb, text, 0);
         return;
     }

     int left_pad = pad_needed / 2;
     int right_pad = pad_needed - left_pad;

     d_StringRepeat(sb, pad_char, left_pad);
     d_StringAddStr(sb, text, 0);
     d_StringRepeat(sb, pad_char, right_pad);
 }
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
  * -- Example: d_StringJoin(sb, {"a", "b", "c"}, 3, ", ") produces "a, b, c"
  * -- Commonly used for creating comma-separated lists, file paths, etc.
  */
 void d_StringJoin(dString_t* sb, const char** strings, int count, const char* separator) {
     if (sb == NULL || strings == NULL || count <= 0) return;

     for (int i = 0; i < count; i++) {
         if (strings[i] != NULL) {
             d_StringAddStr(sb, strings[i], 0);
         }

         // Add separator between elements (but not after the last one)
         if (i < count - 1 && separator != NULL) {
             d_StringAddStr(sb, separator, 0);
         }
     }
 }
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
  * -- Caller must free the result using d_StringFreeSplit()
  * -- Each string in the result array is individually allocated
  * -- Empty strings between delimiters are included in the result
  * -- If delimiter is not found, returns array with single copy of original string
  * -- *count is set to the number of strings in the returned array
  */
 char** d_StringSplit(const char* text, const char* delimiter, int* count) {
     if (text == NULL || delimiter == NULL || count == NULL) {
         if (count) *count = 0;
         return NULL;
     }

     *count = 0;
     int delimiter_len = strlen(delimiter);
     if (delimiter_len == 0) {
         if (count) *count = 0;
         return NULL;
     }

     // First pass: count the number of parts
     const char* pos = text;
     int parts = 1; // At least one part
     while ((pos = strstr(pos, delimiter)) != NULL) {
         parts++;
         pos += delimiter_len;
     }

     // Allocate array for string pointers
     char** result = malloc(parts * sizeof(char*));
     if (result == NULL) {
         *count = 0;
         return NULL;
     }

     // Second pass: extract the parts
     pos = text;
     int current_part = 0;

     while (current_part < parts) {
         const char* next_delim = strstr(pos, delimiter);

         if (next_delim == NULL) {
             // Last part - copy rest of string
             int len = strlen(pos);
             result[current_part] = malloc(len + 1);
             if (result[current_part] == NULL) {
                 // Clean up on allocation failure
                 for (int i = 0; i < current_part; i++) {
                     free(result[i]);
                 }
                 free(result);
                 *count = 0;
                 return NULL;
             }
             strcpy(result[current_part], pos);
             current_part++;
             break;
         } else {
             // Copy part before delimiter
             int len = next_delim - pos;
             result[current_part] = malloc(len + 1);
             if (result[current_part] == NULL) {
                 // Clean up on allocation failure
                 for (int i = 0; i < current_part; i++) {
                     free(result[i]);
                 }
                 free(result);
                 *count = 0;
                 return NULL;
             }
             strncpy(result[current_part], pos, len);
             result[current_part][len] = '\0';
             current_part++;
             pos = next_delim + delimiter_len;
         }
     }

     *count = parts;
     return result;
 }
 /*
  * Free memory allocated by d_StringSplit()
  *
  * `result` - Array of strings returned by d_StringSplit()
  * `count` - Number of strings in the array
  *
  * -- Safe to call with NULL result pointer
  * -- Frees each individual string and then the array itself
  * -- Must be called for every successful d_StringSplit() call to prevent memory leaks
  */
 void d_StringFreeSplit(char** result, int count) {
     if (result == NULL) return;

     for (int i = 0; i < count; i++) {
         free(result[i]);
     }
     free(result);
 }
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
  * -- Example: d_StringSlice(sb, "Hello", 1, 4) produces "ell"
  * -- Example: d_StringSlice(sb, "Hello", -3, -1) produces "ll"
  * -- Example: d_StringSlice(sb, "Hello", 0, -1) produces "Hello"
  */
  void d_StringSlice(dString_t* sb, const char* text, int start, int end) {
      if (sb == NULL || text == NULL) return;

      int text_len = strlen(text);

      // Handle negative indices
      if (start < 0) {
          start = text_len + start;
      }
      if (end == -1) {
          end = text_len;  // Special case: -1 means "to the end"
      } else if (end < 0) {
          end = text_len + end;
      }

     // Clamp indices to valid range
     if (start < 0) start = 0;
     if (start > text_len) start = text_len;
     if (end < 0) end = 0;
     if (end > text_len) end = text_len;

     // Check if slice is valid
     if (start >= end) return;

     // Add the slice to string builder
     int slice_len = end - start;
     d_string_builder_ensure_space(sb, slice_len);
     strncpy(sb->str + sb->len, text + start, slice_len);
     sb->len += slice_len;
     sb->str[sb->len] = '\0';
 }
