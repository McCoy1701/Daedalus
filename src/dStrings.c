/* dStrings.c - The heart of the Daedalus string manipulation library, a testament to the power of dynamic, safe, and expressive string handling in C. */
// Wisdom: A string is not just a sequence of characters; it is a story waiting to be told, a message to be delivered, a command to be executed. Handle it with the respect it deserves.


#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )

#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

static const size_t d_string_builder_min_size = 32;

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

/**
 * @brief Ensure the string builder has enough space for additional content
 *
 * @param sb The string builder to ensure space for
 * @param add_len The number of bytes to add (not including null terminator)
 */
static void d_StringBuilderEnsureSpace(dString_t* sb, size_t add_len)
{
    if (sb == NULL || add_len == 0) {
           LOG("d_StringBuilderEnsureSpace: sb is NULL or add_len is 0");
           return;
       }

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
 */
 dString_t* d_InitString(void)
 {
     dString_t* sb;

     sb = calloc(1, sizeof(*sb));
     if (sb == NULL) {
         LOG("d_InitString: Failed to allocate memory for string builder");
         return NULL;
     }

     sb->str = malloc(d_string_builder_min_size);
     if (sb->str == NULL) {
         LOG("d_InitString: Failed to allocate memory for string builder");
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
 */
void d_DestroyString(dString_t* sb)
{
    if (sb == NULL) {
        LOG("d_DestroyString: sb is NULL");
        return;
    }
    free(sb->str);
    free(sb);
}
/*
 * Add a string to the string builder
 */
 void d_AppendToString(dString_t* sb, const char* str, size_t len)
  {
      // Basic validation: must have a valid string builder and source string.
      if (sb == NULL || str == NULL) {
          return;
      }

      // If len is 0, we treat `str` as a normal C-string.
      // We can safely check for an empty string and use strlen().
      if (len == 0) {
          if (*str == '\0') {
              return; // Nothing to append.
          }
          len = strlen(str);
      }
      // If len > 0, we proceed and copy EXACTLY len bytes, including null bytes.

     // Handle the self-append edge case.
     ptrdiff_t offset = -1;
     if (str >= sb->str && str < sb->str + sb->alloced) {
         // The source `str` is inside our own buffer.
         // Save the offset, not the pointer, as the pointer may be invalidated by realloc.
         offset = str - sb->str;
     }

     // Ensure there is enough space for the new content. This might call realloc.
     d_StringBuilderEnsureSpace(sb, len);

     const char* source_ptr = str;
     if (offset != -1) {
         // If it was a self-append, the buffer might have moved.
         // Recalculate the source pointer using the (potentially new) buffer start and the saved offset.
         source_ptr = sb->str + offset;
     }

     // Use memmove, as it's safe for overlapping memory regions.
     // This is now safe from dangling pointers because source_ptr is recalculated.
     memmove(sb->str + sb->len, source_ptr, len);
     sb->len += len;
     sb->str[sb->len] = '\0'; // Ensure null-termination.
 }

/*
 * Set the content of an existing dString_t to a new value
 */
int d_SetString(dString_t* string, const char* content, int flags)
{
    // Check your inputs.
    if (!string)
    {
        return -1; // Error: null dString_t pointer
    }

    // No content? Clear the string.
    if (!content)
    {
        d_ClearString(string);
        return 0;
    }

    size_t content_len = strlen(content);

    // If the new content is the same as the old, do nothing.
    if (string->str && strcmp(string->str, content) == 0)
    {
        return 0; // Success: no change needed
    }

    // Ensure there's enough space. This might reallocate.
    d_StringBuilderEnsureSpace(string, content_len);
    if (!string->str)
    {
        return -1; // Error: memory allocation failed
    }

    // Copy the new content. Use memmove for safety
    memmove(string->str, content, content_len);
    string->len = content_len;
    string->str[string->len] = '\0'; // Null-terminate.

    return 0; // Success
}

/**
 * @brief Create a new string builder with the same content as another string.
 *
 * @param source The string to copy content from.
 *
 * @return A new string builder with the same content as the source, or NULL on error.
 */
dString_t* d_CloneString(const dString_t* source)
{
    // Check input
    if (!source) {
        return NULL; // Error: null source pointer
    }

    // Create new string builder
    dString_t* clone = d_InitString();
    if (!clone) {
        return NULL; // Error: failed to create new string
    }

    // If source has content, copy it
    if (source->str && source->len > 0) {
        // Set the content using existing function
        if (d_SetString(clone, source->str, 0) != 0) {
            // Failed to set content, cleanup and return error
            d_DestroyString(clone);
            return NULL;
        }
    }

    return clone; // Success
}

 /*
  * Add a limited portion of a string to the string builder
  */
 void d_AppendToStringN(dString_t* sb, const char* str, size_t max_len)
 {
     if (sb == NULL || str == NULL || max_len == 0) {
         return;
     }

     // Find the actual length to append (minimum of max_len and string length)
     size_t actual_len = 0;
     while (actual_len < max_len && str[actual_len] != '\0') {
         actual_len++;
     }

     if (actual_len == 0) {
         return; // Nothing to append
     }

     // Handle the self-append edge case (similar to d_AppendToString)
     ptrdiff_t offset = -1;
     if (str >= sb->str && str < sb->str + sb->alloced) {
         offset = str - sb->str;
     }

     // Ensure space and handle potential realloc
     d_StringBuilderEnsureSpace(sb, actual_len);

     const char* source_ptr = str;
     if (offset != -1) {
         // Recalculate pointer if buffer was reallocated
         source_ptr = sb->str + offset;
     }

     // Safe append with exact length control
     memmove(sb->str + sb->len, source_ptr, actual_len);
     sb->len += actual_len;
     sb->str[sb->len] = '\0'; // Ensure null termination
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
void d_AppendCharToString(dString_t* sb, char c)
{
    if (sb == NULL)
        {
        LOG("d_AppendCharToString: sb is NULL");
        return;
    }

    d_StringBuilderEnsureSpace(sb, 1);
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
void d_AppendIntToString(dString_t* sb, int val)
{
    char str[12]; // Enough for 32-bit int plus sign and null terminator

    if (sb == NULL)
        {
        LOG("d_AppendIntToString: sb is NULL");
        return;
        }
    snprintf(str, sizeof(str), "%d", val);
    d_AppendToString(sb, str, 0);
}
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
void d_AppendFloatToString(dString_t* sb, float val, int decimals)
{
    char str[32]; // Enough for float with up to 10 decimal places
    char format[8]; // Format string like "%.2f"

    if (sb == NULL)
        {
        LOG("d_AppendFloatToString: sb is NULL");
        return;
        }
    // Clamp decimals to reasonable range
    if (decimals < 0)
        decimals = 6; // Default precision
    if (decimals > 10)
        decimals = 10; // Maximum precision

    // Build format string
    snprintf(format, sizeof(format), "%%.%df", decimals);

    // Format the float
    snprintf(str, sizeof(str), format, val);
    d_AppendToString(sb, str, 0);
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
void d_ClearString(dString_t* sb)
{
    if (sb == NULL) {
        LOG("d_ClearString: sb is NULL");
        return;
    }
    d_TruncateString(sb, 0);
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
void d_TruncateString(dString_t* sb, size_t len)
{
    if (sb == NULL || len > sb->len){
        LOG("d_TruncateString: sb is NULL or len > current length");
        return;
    }

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
void d_DropString(dString_t* sb, size_t len)
{
    if (sb == NULL || len == 0){
        LOG("d_DropString: sb is NULL or len is 0");
        return;
    }
    if (len >= sb->len) {
        LOG("d_DropString: len >= current length");
        d_ClearString(sb);
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
size_t d_GetLengthOfString(const dString_t* sb)
{
    if (sb == NULL) {
        LOG("d_GetLengthOfString: sb is NULL");
        return 0;
    }
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
const char* d_PeekString(const dString_t* sb)
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
char* d_DumpString(const dString_t* sb, size_t* len)
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
void d_FormatString(dString_t* sb, const char* format, ...) {
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
    d_StringBuilderEnsureSpace(sb, needed);
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
void d_RepeatString(dString_t* sb, char character, int count) {
    if (sb == NULL || count <= 0) return;

    d_StringBuilderEnsureSpace(sb, count);
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
void d_AppendProgressBarToString(dString_t* sb, int current, int max, int width, char fill_char, char empty_char) {
    if (sb == NULL || width <= 0 || max <= 0) return;

    int filled = (current * width) / max;
    if (filled > width) filled = width;

    d_AppendCharToString(sb, '[');
    d_RepeatString(sb, fill_char, filled);
    d_RepeatString(sb, empty_char, width - filled);
    d_AppendCharToString(sb, ']');
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
 void d_ApplyTemplateToString(dString_t* sb, const char* tmplt, const char** keys, const char** values, int count) {
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
                                     d_AppendToString(sb, values[i], 0);
                                 }
                                 found = 1;
                                 break;
                             }
                         }
                     }

                     if (!found) {
                         // Keep original placeholder if no match
                         d_AppendToString(sb, pos, end - pos + 1);
                     }

                     pos = end + 1;
                 } else {
                     d_AppendCharToString(sb, *pos++);
                     continue;
                 }
             } else {
                 d_AppendCharToString(sb, *pos++);
             }
         } else {
             d_AppendCharToString(sb, *pos++);
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
  * -- Example: d_PadLeftString(sb, "Hi", 5, '.') produces "...Hi"
  * -- Commonly used for right-aligned text in tables and menus
  */
 void d_PadLeftString(dString_t* sb, const char* text, int width, char pad_char) {
     if (sb == NULL || text == NULL || width <= 0) return;

     int text_len = strlen(text);
     int pad_needed = width - text_len;

     if (pad_needed > 0) {
         d_RepeatString(sb, pad_char, pad_needed);
     }
     d_AppendToString(sb, text, 0);
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
  * -- Example: d_PadRightString(sb, "Hi", 5, '.') produces "Hi..."
  * -- Commonly used for left-aligned text in tables and menus
  */
 void d_PadRightString(dString_t* sb, const char* text, int width, char pad_char) {
     if (sb == NULL || text == NULL || width <= 0) return;

     int text_len = strlen(text);
     int pad_needed = width - text_len;

     d_AppendToString(sb, text, 0);
     if (pad_needed > 0) {
         d_RepeatString(sb, pad_char, pad_needed);
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
  * -- Example: d_PadCenterString(sb, "Hi", 6, '.') produces "..Hi.."
  * -- Example: d_PadCenterString(sb, "Hi", 7, '.') produces "..Hi..."
  * -- Commonly used for centered headers and titles in ASCII interfaces
  */
 void d_PadCenterString(dString_t* sb, const char* text, int width, char pad_char) {
     if (sb == NULL || text == NULL || width <= 0) return;

     int text_len = strlen(text);
     int pad_needed = width - text_len;

     if (pad_needed <= 0) {
         d_AppendToString(sb, text, 0);
         return;
     }

     int left_pad = pad_needed / 2;
     int right_pad = pad_needed - left_pad;

     d_RepeatString(sb, pad_char, left_pad);
     d_AppendToString(sb, text, 0);
     d_RepeatString(sb, pad_char, right_pad);
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
  * -- Example: d_JoinStrings(sb, {"a", "b", "c"}, 3, ", ") produces "a, b, c"
  * -- Commonly used for creating comma-separated lists, file paths, etc.
  */
 void d_JoinStrings(dString_t* sb, const char** strings, int count, const char* separator) {
     if (sb == NULL || strings == NULL || count <= 0) return;

     for (int i = 0; i < count; i++) {
         if (strings[i] != NULL) {
             d_AppendToString(sb, strings[i], 0);
         }

         // Add separator between elements (but not after the last one)
         if (i < count - 1 && separator != NULL) {
             d_AppendToString(sb, separator, 0);
         }
     }
 }
 
 /*
  * Extract a substring using Python-style slice notation (like str[start:end])
  *
  * `sb` - Pointer to string builder
  * `text` - Source string to slice (must be null-terminated)
  * `start` - Starting index (inclusive, negative values count from end)
  * `end` - Ending index (exclusive, negative values count from end)
  *
  * -- Does nothing if sb or text is NULL
  * -- Negative indices count from the end: -1 is last character, -2 is second-to-last, etc.
  * -- SPECIAL CASE: An 'end' value of -1 is treated as the end of the string.
  * -- If start >= end (after resolving negative indices), no text is added
  * -- Indices are clamped to valid range [0, string_length]
  * -- Example: d_SliceString(sb, "Hello", 1, 4) produces "ell"
  * -- Example: d_SliceString(sb, "Hello", -3, -1) produces "llo"
  */
  void d_SliceString(dString_t* sb, const char* text, int start, int end) {
      if (sb == NULL || text == NULL) return;

      int text_len = (int)strlen(text);

      // Handle negative indices for start
      if (start < 0) {
          start = text_len + start;
      }

      // Handle 'end' index, with a special case for -1 to mean "to the end"
      if (end == -1) {
         end = text_len;
      } else if (end < 0) {
          end = text_len + end;
      }

      // Clamp indices to a valid range to prevent out-of-bounds access
      if (start < 0) start = 0;
      if (start > text_len) start = text_len;
      if (end < 0) end = 0;
      if (end > text_len) end = text_len;

      // After clamping, if the slice is invalid or empty, do nothing
      if (start >= end) {
          return;
      }

      // Add the slice to string builder
      int slice_len = end - start;
      d_StringBuilderEnsureSpace(sb, slice_len);

      // Use memcpy as we know the exact length; it's safer than strncpy here.
      memcpy(sb->str + sb->len, text + start, slice_len);
      sb->len += slice_len;
      sb->str[sb->len] = '\0'; // Ensure null termination
 }

/*
 * Compare two dString_t objects lexicographically
 */
int d_CompareStrings(const dString_t* str1, const dString_t* str2)
{
    // If both are the same kind of invalid, consider them equal.
    if (d_IsStringInvalid(str1) && d_IsStringInvalid(str2)) {
        return 0;
    }
    // If one is invalid and the other is not, they are not equal.
    if (d_IsStringInvalid(str1)) {
        return -1; // Treat invalid str1 as "less than" valid str2.
    }
    if (d_IsStringInvalid(str2)) {
        return 1;  // Treat valid str1 as "greater than" invalid str2.
    }

    // If both are valid, perform length-aware binary comparison
    // First check if lengths are different
    if (str1->len != str2->len) {
        return (str1->len < str2->len) ? -1 : 1;
    }
    
    // If lengths are equal, compare the actual data using memcmp
    // This handles embedded null bytes correctly
    return memcmp(str1->str, str2->str, str1->len);
}

/*
 * Compare a dString_t with a standard C-string
 */
int d_CompareStringToCString(const dString_t* d_str, const char* c_str)
{
    // An invalid dString and a NULL/empty C-string can be considered "equal".
    if (d_IsStringInvalid(d_str) && (c_str == NULL || *c_str == '\0')) {
        return 0;
    }

    // If one is invalid/NULL and the other is not, they are not equal.
    if (d_IsStringInvalid(d_str)) {
        return -1; // Treat invalid d_str as "less than" a valid c_str.
    }
    if (c_str == NULL) {
        return 1;  // Treat valid d_str as "greater than" a NULL c_str.
    }

    // If both are valid, perform length-aware comparison
    // First get the C string length
    size_t c_str_len = strlen(c_str);
    
    // If lengths are different, they're not equal
    if (d_str->len != c_str_len) {
        return (d_str->len < c_str_len) ? -1 : 1;
    }
    
    // If lengths are equal, compare the actual data using memcmp
    // This handles embedded null bytes correctly
    return memcmp(d_str->str, c_str, d_str->len);
}
