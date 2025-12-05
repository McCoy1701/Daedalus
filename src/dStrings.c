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

dString_t* d_StringCreateFromFile(const char *filename)
{
  long fileSize;
  char *fileBuffer;
  FILE *file;
  dString_t* result;

  if (filename == NULL) {
    printf("Error: NULL filename provided\n");
    return NULL;
  }

  file = fopen(filename, "rb");  // Binary mode for cross-platform compatibility
  if (file == NULL) {
    printf("Error loading file: %s\n", filename);
    return NULL;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  fileSize = ftell(file);
  rewind(file);

  // Check for empty file or error
  if (fileSize < 0) {
    printf("Error getting file size: %s\n", filename);
    fclose(file);
    return NULL;
  }

  // Allocate temporary buffer
  fileBuffer = (char*)malloc(fileSize + 1);
  if (fileBuffer == NULL) {
    printf("Error allocating memory for file string: %s\n", filename);
    fclose(file);
    return NULL;
  }

  // Read file contents
  if (fileSize > 0 && fread(fileBuffer, fileSize, 1, file) != 1) {
    printf("Failed to read file: %s\n", filename);
    fclose(file);
    free(fileBuffer);
    return NULL;
  }

  fclose(file);

  // Null-terminate the buffer
  fileBuffer[fileSize] = '\0';

  // Create dString and set its content
  result = d_StringInit();
  if (result == NULL) {
    printf("Failed to initialize dString for file: %s\n", filename);
    free(fileBuffer);
    return NULL;
  }

  // Use d_StringAppend to add the file contents
  d_StringAppend(result, fileBuffer, fileSize);

  // Free temporary buffer
  free(fileBuffer);

  return result;
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
 dString_t* d_StringInit(void)
 {
     dString_t* sb;

     sb = calloc(1, sizeof(*sb));
     if (sb == NULL) {
         LOG("d_StringInit: Failed to allocate memory for string builder");
         return NULL;
     }

     sb->str = malloc(d_string_builder_min_size);
     if (sb->str == NULL) {
         LOG("d_StringInit: Failed to allocate memory for string builder");
         free(sb);
         return NULL;
     }

     *sb->str = '\0';
     sb->alloced = d_string_builder_min_size;
     sb->len = 0;

     return sb;
 }
void _d_StringDestroy_impl(dString_t* sb, const char* file, int line, const char* func)
{
    D_ASSERT(sb != NULL, "d_StringDestroy: sb is NULL", file, line, func);
    D_ASSERT(sb->str != NULL, "d_StringDestroy: sb->str is NULL (double-free or corruption?)", file, line, func);
    D_ASSERT(sb->alloced >= 32, "d_StringDestroy: sb->alloced is impossibly small (corruption?)", file, line, func);
    
    free(sb->str);
    free(sb);
}
/*
 * Add a string to the string builder
 */
 void d_StringAppend(dString_t* sb, const char* str, size_t len)
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
int d_StringSet(dString_t* string, const char* content)
{
    // Check your inputs.
    if (!string)
    {
        return -1; // Error: null dString_t pointer
    }

    // No content? Clear the string.
    if (!content)
    {
        d_StringClear(string);
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


dString_t* d_StringClone(const dString_t* source)
{
    // Check input
    if (!source) {
        return NULL; // Error: null source pointer
    }

    // Create new string builder
    dString_t* clone = d_StringInit();
    if (!clone) {
        return NULL; // Error: failed to create new string
    }

    // If source has content, copy it
    if (source->str && source->len > 0) {
        // Set the content using existing function
        if (d_StringSet(clone, source->str) != 0) {
            // Failed to set content, cleanup and return error
            d_StringDestroy(clone);
            return NULL;
        }
    }

    return clone; // Success
}

 void d_StringAppendN(dString_t* sb, const char* str, size_t max_len)
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

     // Handle the self-append edge case (similar to d_StringAppend)
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

void d_StringAppendChar(dString_t* sb, char c)
{
    if (sb == NULL)
        {
        LOG("d_StringAppendChar: sb is NULL");
        return;
    }

    d_StringBuilderEnsureSpace(sb, 1);
    sb->str[sb->len] = c;
    sb->len++;
    sb->str[sb->len] = '\0';
}

void d_StringAppendInt(dString_t* sb, int val)
{
    char str[12]; // Enough for 32-bit int plus sign and null terminator

    if (sb == NULL)
        {
        LOG("d_StringAppendInt: sb is NULL");
        return;
        }
    snprintf(str, sizeof(str), "%d", val);
    d_StringAppend(sb, str, 0);
}

void d_StringAppendFloat(dString_t* sb, float val, int decimals)
{
    char str[32]; // Enough for float with up to 10 decimal places
    char format[8]; // Format string like "%.2f"

    if (sb == NULL)
        {
        LOG("d_StringAppendFloat: sb is NULL");
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
    d_StringAppend(sb, str, 0);
}

void d_StringClear(dString_t* sb)
{
    if (sb == NULL) {
        LOG("d_StringClear: sb is NULL");
        return;
    }
    d_StringTruncate(sb, 0);
}

void d_StringTruncate(dString_t* sb, size_t len)
{
    if (sb == NULL || len > sb->len){
        LOG("d_StringTruncate: sb is NULL or len > current length");
        return;
    }

    sb->len = len;
    sb->str[sb->len] = '\0';
}

void d_StringDrop(dString_t* sb, size_t len)
{
    if (sb == NULL || len == 0){
        LOG("d_StringDrop: sb is NULL or len is 0");
        return;
    }
    if (len >= sb->len) {
        LOG("d_StringDrop: len >= current length");
        d_StringClear(sb);
        return;
    }

    sb->len -= len;
    /* +1 to move the NULL terminator. */
    memmove(sb->str, sb->str + len, sb->len + 1);
}

size_t d_StringGetLength(const dString_t* sb)
{
    if (sb == NULL) {
        LOG("d_StringGetLength: sb is NULL");
        return 0;
    }
    return sb->len;
}

const char* _d_StringPeek_impl(const dString_t* sb, const char* file, int line, const char* func)
{
    D_ASSERT(sb != NULL, "d_StringPeek: sb is NULL", file, line, func);
    return sb->str;
}

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
    d_StringBuilderEnsureSpace(sb, needed);
    vsnprintf(sb->str + sb->len, needed + 1, format, args);
    sb->len += needed;

    va_end(args);
}


void d_StringRepeat(dString_t* sb, char character, int count) {
    if (sb == NULL || count <= 0) return;

    d_StringBuilderEnsureSpace(sb, count);
    for (int i = 0; i < count; i++) {
        sb->str[sb->len + i] = character;
    }
    sb->len += count;
    sb->str[sb->len] = '\0';
}


void d_StringAppendProgressBar(dString_t* sb, int current, int max, int width, char fill_char, char empty_char) {
    if (sb == NULL || width <= 0 || max <= 0) return;

    int filled = (current * width) / max;
    if (filled > width) filled = width;

    d_StringAppendChar(sb, '[');
    d_StringRepeat(sb, fill_char, filled);
    d_StringRepeat(sb, empty_char, width - filled);
    d_StringAppendChar(sb, ']');
}

 void d_StringTemplate(dString_t* sb, const char* tmplt, const char* keys[], const char* values[], int count) {
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
                                     d_StringAppend(sb, values[i], 0);
                                 }
                                 found = 1;
                                 break;
                             }
                         }
                     }

                     if (!found) {
                         // Keep original placeholder if no match
                         d_StringAppend(sb, pos, end - pos + 1);
                     }

                     pos = end + 1;
                 } else {
                     d_StringAppendChar(sb, *pos++);
                     continue;
                 }
             } else {
                 d_StringAppendChar(sb, *pos++);
             }
         } else {
             d_StringAppendChar(sb, *pos++);
         }
     }
 }


 void d_StringPadLeft(dString_t* sb, const char* text, int width, char pad_char) {
     if (sb == NULL || text == NULL || width <= 0) return;

     int text_len = strlen(text);
     int pad_needed = width - text_len;

     if (pad_needed > 0) {
         d_StringRepeat(sb, pad_char, pad_needed);
     }
     d_StringAppend(sb, text, 0);
 }

 void d_StringPadRight(dString_t* sb, const char* text, int width, char pad_char) {
     if (sb == NULL || text == NULL || width <= 0) return;

     int text_len = strlen(text);
     int pad_needed = width - text_len;

     d_StringAppend(sb, text, 0);
     if (pad_needed > 0) {
         d_StringRepeat(sb, pad_char, pad_needed);
     }
 }

 void d_StringPadCenter(dString_t* sb, const char* text, int width, char pad_char) {
     if (sb == NULL || text == NULL || width <= 0) return;

     int text_len = strlen(text);
     int pad_needed = width - text_len;

     if (pad_needed <= 0) {
         d_StringAppend(sb, text, 0);
         return;
     }

     int left_pad = pad_needed / 2;
     int right_pad = pad_needed - left_pad;

     d_StringRepeat(sb, pad_char, left_pad);
     d_StringAppend(sb, text, 0);
     d_StringRepeat(sb, pad_char, right_pad);
 }

 void d_StringJoin(dString_t* sb, const char** strings, int count, const char* separator) {
     if (sb == NULL || strings == NULL || count <= 0) return;

     for (int i = 0; i < count; i++) {
         if (strings[i] != NULL) {
             d_StringAppend(sb, strings[i], 0);
         }

         // Add separator between elements (but not after the last one)
         if (i < count - 1 && separator != NULL) {
             d_StringAppend(sb, separator, 0);
         }
     }
 }

void d_StringSlice(dString_t* sb, const char* text, int start, int end) {
    if (sb == NULL || text == NULL) return;

    int text_len = (int)strlen(text);

    // Handle negative indices (Python-style)
    if (start < 0) {
        start = text_len + start;
    }
    if (end < 0) {
        end = text_len + end;
    }

    // Clamp indices to valid range
    if (start < 0) start = 0;
    if (start > text_len) start = text_len;
    if (end < 0) end = 0;
    if (end > text_len) end = text_len;

    // If slice is invalid or empty, do nothing
    if (start >= end) {
        return;
    }

    // Add the slice to string builder
    int slice_len = end - start;
    d_StringBuilderEnsureSpace(sb, slice_len);
    memcpy(sb->str + sb->len, text + start, slice_len);
    sb->len += slice_len;
    sb->str[sb->len] = '\0';
}


int d_StringCompare(const dString_t* str1, const dString_t* str2)
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


int d_StringCompareToCString(const dString_t* d_str, const char* c_str)
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
