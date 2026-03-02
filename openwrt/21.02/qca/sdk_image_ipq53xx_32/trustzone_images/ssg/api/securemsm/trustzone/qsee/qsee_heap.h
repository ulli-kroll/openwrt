#ifndef QSEEHEAP_H
#define QSEEHEAP_H

/*===========================================================================

DESCRIPTION
  Wrapper for secure malloc and free calls for secure heap.

Copyright (c) 2011-2019  by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ------------------------------------------------------------------------
** Structures
** ------------------------------------------------------------------------ */

typedef struct heap_info_s
{
  uint32_t total_size;               /** Total size of heap */
  uint32_t used_size;                /** Amount of heap currently allocated */
  uint32_t free_size;                /** Amount of free space */
  uint32_t overhead_size;            /** Amount of bytes consumed as overhead */
  uint32_t wasted_size;              /** Amount of bytes not available for allocation */
  uint32_t largest_free_block_size;  /** Size of largest free block */
} heap_info_t;

/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

/**
   @addtogroup qtee_heap
   @{
*/

#ifdef __cplusplus
   extern "C"
   {
#endif

/**
  @brief   Allocates a block of size bytes from the heap.  If size is 0, returns NULL.

  @param[in] size Number of bytes to allocate from the heap.

  @return
  Returns a pointer to the newly allocated block, or NULL if the block could not be allocated.

*/
void* qsee_malloc(size_t size);

/**
  @brief   Resizes a block of memory previously allocated using qsee_malloc() or qsee_realloc().  If size is zero, returns NULL.

  @param[in] ptr Pointer to previously allocated block. If NULL is passed, this function is akin to qsee_malloc().

  @param[in] size New block size.

  @return
  Returns a pointer to the newly allocated block, or NULL if the block could not be allocated.

*/
void* qsee_realloc(void* ptr, size_t size);

/**
  @brief  Deallocates the ptr block of memory.

  @param[in] ptr Pointer to block to be freed.

*/
void qsee_free(void *ptr);

/**
  @brief  Provides the heap information for a TA.

  @param[out] heap_info_ptr pointer to TA heap information.

  @return 0 for success, else -1.

*/
int qsee_query_heap_info(heap_info_t *heap_info_ptr);

/**
  @brief   Allocates and zero-intializes a block of size bytes.

  @param[in] size Number of bytes to allocate from the heap.

  @return
  Returns a pointer to the newly allocated block, or NULL if the block could not be allocated.

*/
static inline void *qsee_zalloc(size_t size)
{
  void *p = qsee_malloc(size);
  if (p != NULL) {
    memset(p, 0, size);
  }
  return p;
}

/**
  @brief Allocates and zero-initializes a block large enough to hold 'num' items of size 'size'.

  @param[in] num  Number of items to allocate from the heap.
  @param[in] size Item size in bytes.

  @return
  Returns a pointer to the newly allocated block, or NULL if the block could not be allocated.
*/
static inline void *qsee_calloc(size_t num, size_t size)
{
  if (num > SIZE_MAX/size) {
    return NULL;
  }
  return qsee_zalloc(num * size);
}



/**
  @brief Allocate and zero-initialize a block large enough to hold an
  object of the given type.
*/
#define QSEE_ZALLOC_TYPE(type)     ((type*) qsee_zalloc(sizeof(type)))

/**
  @brief Allocate and zero-initialize a block large enough to hold an
  array of n items of the given type.
*/
#define QSEE_ZALLOC_ARRAY(type, n) ((type *) qsee_calloc((n), sizeof(type)))

#define QSEE_FREE_PTR(var)         ((void) (qsee_free(var), (var) = 0))

#ifdef __cplusplus
   }
#endif

/** @} */

#endif /* QSEEHEAP_H */


