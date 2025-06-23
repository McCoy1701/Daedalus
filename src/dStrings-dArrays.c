// File: src/dStrings-dArrays.c - Utilities that turn dStrings into dArrays

#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )

#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void d_FreeSplitStringArray(dArray_t* string_array) {
    if (string_array == NULL) {
        LOG("d_FreeSplitStringArray: NULL array");
        return;
    }

    // Iterate through the array and free each string builder.
    for (size_t i = 0; i < string_array->count; i++) {
        // Get the location of the dString_t* stored in the array's data.
        dString_t** sb_ptr_location = (dString_t**)d_GetDataFromArrayByIndex(string_array, i);
        if (sb_ptr_location != NULL && *sb_ptr_location != NULL) {
            // Dereference the location to get the actual dString_t* and destroy it.
            d_DestroyString(*sb_ptr_location);
        }
    }

    // Finally, destroy the array structure itself.
    d_DestroyArray(string_array);
}

dArray_t* d_SplitString(const char* text, const char* delimiter) {
    if (text == NULL || delimiter == NULL) {
        LOG("d_SplitString: NULL text or delimiter");
        return NULL;
    }

    size_t delimiter_len = strlen(delimiter);
    if (delimiter_len == 0) {
        LOG("d_SplitString: Empty delimiter");
        // Splitting by an empty delimiter is not supported.
        return NULL;
    }

    // Initialize a dynamic array to hold the dString_t* pointers.
    // The element size is the size of a pointer to a dString_t.
    dArray_t* result_array = d_InitArray(8, sizeof(dString_t*));
    if (result_array == NULL) {
        LOG("d_SplitString: Failed to create array");
        return NULL; // Failed to create the array.
    }

    const char* current_pos = text;
    const char* next_delim;

    // Loop through the string, finding each occurrence of the delimiter.
    while ((next_delim = strstr(current_pos, delimiter)) != NULL) {
        size_t segment_len = next_delim - current_pos;
        dString_t* segment_sb = d_InitString();
        if (segment_sb == NULL) {
            d_FreeSplitStringArray(result_array); // Clean up on failure.
            return NULL;
        }

        d_AppendString(segment_sb, current_pos, segment_len);

        // Check if array is full and grow it if needed
        if (result_array->count >= result_array->capacity) {
            if (d_GrowArray(result_array, 8 * result_array->element_size) != 0) {
                d_DestroyString(segment_sb);
                d_FreeSplitStringArray(result_array);
                LOG("d_SplitString: Failed to grow array");
                return NULL;
            }
        }

        d_AppendArray(result_array, &segment_sb); // Append the pointer to the new string builder

        // Move the position past the delimiter for the next search.
        current_pos = next_delim + delimiter_len;
    }

    // Append the final segment (the remainder of the string after the last delimiter).
    dString_t* final_segment_sb = d_InitString();
    if (final_segment_sb == NULL) {
        d_FreeSplitStringArray(result_array); // Clean up on failure.
        return NULL;
    }
    d_AppendString(final_segment_sb, current_pos, 0); // Use strlen via len=0

    // Check if array is full and grow it if needed for final segment
    if (result_array->count >= result_array->capacity) {
        if (d_GrowArray(result_array, 8 * result_array->element_size) != 0) {
            d_DestroyString(final_segment_sb);
            d_FreeSplitStringArray(result_array);
            LOG("d_SplitString: Failed to grow array");
            return NULL;
        }
    }

    d_AppendArray(result_array, &final_segment_sb);

    return result_array;
}
