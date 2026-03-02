#ifndef QSEE_PRNG_H
#define QSEE_PRNG_H

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_prng.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/05/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/**
  @addtogroup qtee_prng
  @{
*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QSEE_MAX_PRNG    512  /* Maximum number of PRNG bytes read */

/**
 * @brief Generates a random number of a specified length.
 *
 * @param[out] out     Output data buffer.
 * @param[in]  out_len Output data length.\n 
 *                     out_len must be at most QSEE_MAX_PRNG bytes.
 *
 * @return
 * Number of bytes read.
 *
 */
uint32_t qsee_prng_getdata(uint8_t *out, uint32_t out_len);

/** @} */

#endif /*QSEE_PRNG_H*/

