#ifndef QSEE_DCACHE_H
#define QSEE_DCACHE_H

/**
@file qsee_dcache.h
@brief Provide DCache functionality
*/

/*===========================================================================
   Copyright (c) 2014 by QUALCOMM, Technology Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_dcache.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when      who       what, where, why
--------  --------  ------------------------------------
03/04/15  cmihalik  Initial revision

===========================================================================*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_dcache
  @{
*/

/**
 * Invalidates a memory region in the cache. Note that the data
 * in the cache is not written back to the main memory. Any further access 
 * to data in this region results in a cache-miss.
 *
 * @param[in] addr   Memory region start address. 
 * @param[in] length Memory region length.
 */

void qsee_dcache_inval_region(void *addr, size_t length);

/**
 * Cleans and invalidates a memory region in the cache. Note
 * that the data in the cache is written back to the main memory if it is
 * dirty, and the region is invalidated. Any further access to the data 
 * results in a cache-miss.
 *
 * @param[in] addr   Memory region start address.
 * @param[in] length Memory region length.
 */

void qsee_dcache_flush_region(void *addr, size_t length);

/**
 * Cleans a memory region in the cache. Note that this 
 * writes back any data that is dirty, but it does not invalidate the
 * cache region.  Any further access to data in this region 
 * results in a cache-hit.
 *
 * @param[in] addr   Memory region start address.
 * @param[in] length Memory region length.
 */

void qsee_dcache_clean_region(void *addr, size_t length);

/** @} */

#endif

