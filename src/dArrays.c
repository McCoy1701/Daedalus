// File: src/dDynamicArray.c - Dynamic array utilities for Daedalus project

#define LOG(msg) printf("[LOG] %s:%d - %s\n", __FILE__, __LINE__, msg)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "Daedalus.h"

/*
 * Create a new dynamic array with specified capacity and element size
 *
 * `capacity` - Initial number of elements the array can hold
 * `element_size` - Size in bytes of each element (e.g., sizeof(int))
 *
 * `dArray_t*` - Pointer to new dynamic array, or NULL on allocation failure
 *
 * -- Must be destroyed with d_DestroyArray() to free memory
 * -- Initial count is 0 even though capacity may be larger
 * -- Elements can be any type as long as element_size is correct
 * -- Capacity of 0 is allowed but array cannot store elements until resized
 */
dArray_t* d_InitArray( size_t capacity, size_t element_size )
{
  dArray_t* new_array = ( dArray_t* )malloc( sizeof( dArray_t ) );
  // Overflow Check
  if (capacity > 0 && element_size > 0) {
      // Check for overflow: capacity * element_size
      if (capacity > SIZE_MAX / element_size) {
          printf("Array size would overflow\n");
          free(new_array);
          return NULL;
      }
  }
  if ( new_array == NULL )
  {
    printf( "Failed to allocate memory for array\n" );
    return NULL;
  }

  new_array->data = ( void* )malloc( capacity * element_size );
  if ( new_array->data == NULL && capacity > 0 )
  {
    printf( "Failed to allocate memory for array data\n" );
    free( new_array );
    return NULL;
  }

  new_array->capacity = capacity;
  new_array->element_size = element_size;
  new_array->count = 0;

  return new_array;
}

/*
 * Append an element to the end of the dynamic array
 *
 * `array` - Pointer to dynamic array
 * `data` - Pointer to element data to copy into the array
 *
 * -- Does nothing if array or data is NULL
 * -- Copies element_size bytes from data into the array
 * -- Automatically grows array capacity when full (doubles capacity)
 * -- Elements are stored contiguously in memory for cache efficiency
 * -- Growth strategy: start with capacity 1 if 0, otherwise double capacity
 */
void d_AppendArray( dArray_t* array, void* data )
{
  if ( array == NULL || data == NULL )
  {
    return;
  }

  // Auto-grow array if at capacity
  if ( array->count >= array->capacity )
  {
    size_t new_capacity = array->capacity == 0 ? 1 : array->capacity * 2;
    size_t new_size = new_capacity * array->element_size;

    // Check for overflow
    if (array->element_size > 0 && new_capacity > SIZE_MAX / array->element_size) {
      printf("Array growth would overflow\n");
      return;
    }

    if ( d_ResizeArray( array, new_size ) != 0 )
    {
      printf( "Failed to grow array from capacity %zd to %zd\n", array->capacity, new_capacity );
      return;
    }
  }

  // Append the element
  void* dest = ( char* )array->data + ( array->count * array->element_size );
  memcpy( dest, data, array->element_size );
  array->count++;
}

/*
 * Get a pointer to an element at the specified index
 *
 * `array` - Pointer to dynamic array
 * `index` - Zero-based index of element to retrieve
 *
 * `void*` - Pointer to element data, or NULL if index is out of bounds
 *
 * -- Returns NULL if array is NULL or index >= count
 * -- Returned pointer is valid until the array is modified or destroyed
 * -- Caller can read/write through the returned pointer safely
 * -- Use appropriate casting: int* ptr = (int*)d_GetDataFromArrayByIndex(array, 0)
 * -- Index must be less than count, not capacity (only counts appended elements)
 */
void* d_GetDataFromArrayByIndex( dArray_t* array, size_t index )
{
  if ( array == NULL )
  {
    return NULL;
  }

  if ( index < array->count && index < array->capacity )
  {
    return ( char* )array->data + ( index * array->element_size );
  }
  else
  {
    printf( "Index out of bounds %zd / %zd\n", index, array->count );
    return NULL;
  }
}

/*
 * Remove and return a pointer to the last element in the array (LIFO - Last In, First Out)
 *
 * `array` - Pointer to dynamic array
 *
 * `void*` - Pointer to the last element's data, or NULL if array is empty
 *
 * -- Returns NULL if array is NULL or empty (count == 0)
 * -- Decrements count but does not free memory (element data remains in buffer)
 * -- Returned pointer becomes invalid after next append or array modification
 * -- Implements stack-like behavior for dynamic arrays
 * -- Memory is not reallocated, only the count is decremented for efficiency
 */
void* d_PopDataFromArray( dArray_t* array )
{
  if ( array == NULL || array->count == 0 )
  {
    return NULL;
  }

  // Decrement count first, then return pointer to what was the last element
  array->count--;
  void* last_item = ( char* )array->data + ( array->count * array->element_size );
  return last_item;
}

/*
 * Resize the dynamic array's data buffer to accommodate more or fewer elements
 *
 * `array` - Pointer to dynamic array
 * `new_capacity` - New capacity in bytes (not elements)
 *
 * `int` - 0 on success, 1 on failure
 *
 * -- Returns 1 (error) if array is NULL or memory allocation fails
 * -- new_capacity is in bytes, not element count
 * -- To resize to N elements: d_ResizeArray(array, N * array->element_size)
 * -- Existing data is preserved up to the smaller of old/new sizes
 * -- Count is not automatically adjusted when shrinking (may exceed new capacity)
 * -- Use realloc() internally which may move data to new memory location
 * -- Array capacity is updated to new_capacity on success
 */
 int d_ResizeArray( dArray_t* array, size_t new_capacity )
 {
   if ( array == NULL )
   {
    LOG("d_ResizeArray: array is NULL");
     return 1;
   }

   void* new_data = realloc( array->data, new_capacity );
   if ( new_data == NULL && new_capacity > 0 )
   {
    LOG("d_ResizeArray: failed to allocate new memory for array");
     return 1;
   }

   array->data = new_data;

   // Handle division by zero when element_size is 0
   if ( array->element_size == 0 )
   {
     // For zero element size, capacity in terms of "number of elements" is undefined
     // Set capacity to 0 to avoid division by zero
     array->capacity = 0;
   }
   else
   {
     array->capacity = new_capacity / array->element_size;
   }

   return 0;
 }


/*
 * Grow the dynamic array by adding additional capacity to current capacity
 *
 * `array` - Pointer to dynamic array
 * `additional_capacity` - Number of bytes to add to current capacity
 *
 * `int` - 0 on success, 1 on failure
 *
 * -- Convenience function that calls d_ResizeArray() internally
 * -- New total capacity = current capacity + additional_capacity
 * -- additional_capacity is in bytes, not element count
 * -- To grow by N elements: d_GrowArray(array, N * array->element_size)
 * -- Useful for incrementally expanding arrays without calculating new total size
 * -- Returns same error codes as d_ResizeArray()
 */
int d_GrowArray( dArray_t* array, size_t additional_capacity )
{
  if ( array == NULL )
  {
    return 1;
  }
  return d_ResizeArray( array, (array->capacity * array->element_size) + additional_capacity );
}

/*
 * Destroy a dynamic array and free all associated memory
 *
 * `array` - Pointer to dynamic array to destroy
 *
 * -- After calling this function, the pointer is invalid and should not be used
 * -- Frees both the data buffer and the array structure itself
 * -- Safe to call with NULL pointer (does nothing)
 * -- Does not call destructors for complex element types (caller responsibility)
 * -- For arrays of pointers, caller must free pointed-to objects before destroying array
 */
void d_DestroyArray( dArray_t* array )
{
  if ( array != NULL )
  {
    free( array->data );
    free( array );
  }
}
