#ifndef QSEE_HMAC_H
#define QSEE_HMAC_H

/**
@file qsee_hmac.h
@brief Provide hmac API wrappers
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
   ===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_hmac.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
12/09/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/**
@addtogroup qtee_hmac
@{
  qsee_shim.h defines the following return values:
  - SUCCESS --  0
  - FAILURE -- -1
*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/**
  Supported HMAC algorithms.
  */
typedef enum
  {
    QSEE_HMAC_SHA1          = 1,    /**< SHA1 algorithm. */
    QSEE_HMAC_SHA256        = 2,    /**< SHA256 algorithm. */
    QSEE_HMAC_SHA384        = 3,    /**< SHA384 algorithm. */
    QSEE_HMAC_SHA512        = 4,    /**< SHA512 algorithm. */
    QSEE_HMAC_INVALID = 0x7FFFFFFF, /**< Unknown or invalid alogorithm. */
  } QSEE_HMAC_ALGO_ET;

/**
  HMAC parameters.
  */
typedef enum
  {
    QSEE_HMAC_PARAM_KEY  = 1,             /**< Key value parameter. */
    QSEE_HMAC_PARAM_INVALID = 0x7FFFFFFF, /**< Unknown or invalid parameter. */
  } QSEE_HMAC_PARAM_ET;

typedef void qsee_hmac_ctx;
/*---------------------------------------------------------------------------
 * Digest Sizes
 *--------------------------------------------------------------------------*/
#define QSEE_HMAC_DIGEST_SIZE_SHA1    20
#define QSEE_HMAC_DIGEST_SIZE_SHA256  32
#define QSEE_HMAC_DIGEST_SIZE_SHA384  48
#define QSEE_HMAC_DIGEST_SIZE_SHA512  64

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  Creates HMAC per FIPS pub 198 using the specified hash algorithm.

  @caution1hang HMAC is not supported by the QTEE off-target environment.

  @param[in] alg               HMAC algorithm to use.
  @param[in] msg               Pointer to message to authenticate.
  @param[in] msg_len           Length of message in bytes.
  @param[in] key               Pointer to input key to HMAC algorithm.
  @param[in] key_len           Input key length (in bytes).
  @param[out] msg_digest       Pointer to message digest (memory provided by caller).

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error encountered during HMAC creation
 */
int qsee_hmac(QSEE_HMAC_ALGO_ET alg, const uint8_t *msg, uint32_t msg_len,
              const uint8_t *key, uint16_t key_len, uint8_t *msg_digest);

/**
  Initializes HMAC per FIPS pub 198 using the specified hash algorithm.

  @param[in]  alg           Algorithm for HMAC.
  @param[out] ctx           Double pointer to HMAC context.

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Unable to initialize HMAC
 */
int qsee_hmac_init(QSEE_HMAC_ALGO_ET alg, qsee_hmac_ctx **ctx);

/**
  Updates HMAC per FIPS pub 198 using the specified hash algorithm.

  @param[in,out] ctx           Pointer to HMAC context.
  @param[in] msg               Pointer to message to authenticate.
  @param[in] msg_len           Message length (in bytes).

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error during HMAC update
 */
int qsee_hmac_update(qsee_hmac_ctx *ctx, const uint8_t *msg, uint32_t msg_len);

/**
  Final operation for HMAC per FIPS pub 198 using the specified hash
  algorithm.

  @param[in] ctx                  Pointer to HMAC context.
  @param[out] msg_digest          Pointer to message digest
                                  (memory provided by caller).
  @param[in] msg_digest_length    Message length (in bytes).

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error during final operation
*/
int qsee_hmac_final(qsee_hmac_ctx *ctx, uint8_t *msg_digest, uint32_t msg_digest_length);

/**
  Releases all resources with given HMAC context.

  @param[in] hmac_ctx   Pointer to HMAC context to delete.

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Context is invalid
 */
int qsee_hmac_free_ctx(qsee_hmac_ctx *hmac_ctx);

/**
  Modifies parameters for a given HMAC operation.

  @param[in,out] hmac_ctx  Pointer to HMAC context.
  @param[in] param_id      Parameter to modify.
  @param[in] param         Pointer to parameter value to set.
  @param[in] param_len     Param length (in bytes).

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error during parameter setting
 */
int qsee_hmac_set_param(qsee_hmac_ctx *hmac_ctx,
                        QSEE_HMAC_PARAM_ET param_id,
                        const void *param,
                        uint32_t param_len);

/** @} */

#endif /*QSEE_HMAC_H*/
