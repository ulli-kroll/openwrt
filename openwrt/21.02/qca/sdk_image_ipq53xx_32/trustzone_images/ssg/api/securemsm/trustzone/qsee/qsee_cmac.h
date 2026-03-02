#ifndef QSEE_CMAC_H
#define QSEE_CMAC_H

/**
@file qsee_cmac.h
@brief Provide hmac API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_cmac.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/29/13   ejt      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
/**
@addtogroup qtee_cmac
@{
*/
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QSEE_CMAC_DIGEST_SIZE      16 /**< CMAC digest size. */

#define QSEE_CMAC_SUCCESS          0  /**< Function executes successfully. */
#define QSEE_CMAC_FAILURE          -1 /**<  Any error encountered. */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/**
  Supported CMAC algorithms.
 */
typedef enum
{
   QSEE_CMAC_ALGO_AES_128          = 1, /**< AES-128 Cipher Algorithm. */
   QSEE_CMAC_ALGO_AES_256          = 2  /**< AES-256 Cipher Algorithm. */
}  QSEE_CMAC_ALGO_ET;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
  Creates a cipher MAC per FIPS publication 198 using the specified hash algorithm.
  
  @caution1hang qsee_cmac with a NULL key parameter is not supported by the QTEE
  off-target environment.

  @param[in] alg             CMAC algorithm to use.
  @param[in] msg             Pointer to message to be authenticated.
  @param[in] msg_len         Message length (in bytes).
  @param[in] key             Pointer to input key to CMAC algorithm.
  @param[in] key_len         Input key length (in bytes).
  @param[out] cmac_digest    Pointer to CMAC digest (memory provided by caller).
  @param[in] cmac_len        CMAC digest length (in bytes). Must be at least 
                             QSEE_CMAC_DIGEST_SIZE.


  @return
  QSEE_CMAC_SUCCESS -- Function executes successfully. \n
  QSEE_CMAC_FAILURE -- Any error encountered during CMAC creation.
*/
int qsee_cmac(QSEE_CMAC_ALGO_ET alg, const uint8_t *msg, uint32_t msg_len,
              const uint8_t *key, uint32_t key_len, uint8_t *cmac_digest,
              uint32_t cmac_len);

/** @} */

#endif /*QSEE_CMAC_H*/
