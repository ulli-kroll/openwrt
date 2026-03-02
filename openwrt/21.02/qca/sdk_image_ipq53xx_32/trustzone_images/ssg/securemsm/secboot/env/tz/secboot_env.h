#ifndef SECBOOT_ENV_H
#define SECBOOT_ENV_H

/**
@file secboot_env.h
@brief Define functions or parameters for particular environment
*/

/*===========================================================================
   Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/securemsm/secboot/env/tz/secboot_env.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
02/16/17   hw       Init Version 

===========================================================================*/
#include "com_dtypes.h" // type definition with std library
#include <string.h>
#include "secboot_chipset.h"

/*----------------------------------------------------------------------------
 * Environment Supporting Preprocessor Definition
 * -------------------------------------------------------------------------*/
#ifndef PACKED_STRUCT
  #define PACKED_STRUCT struct __attribute__((__packed__)) 
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions for Environment Constants
 * -------------------------------------------------------------------------*/
#define SECBOOT_HASH_DIGEST_BYTE_SIZE SECBOOT_OTP_ROOT_OF_TRUST_BYTE_SIZE
                                             /**< The MAX Digest Size that secboot supports (in bytes). 
                                                     This impacts the size of image hash and certificate hash,
                                                     and the size of root of trust hash provided by caller to
                                                     verify the root certificate. the OTP root of trust is not
                                                     impacted because that is fixed for chipset. */
#define SECBOOT_MAX_CERT_SIZE_IN_BYTE (2048) /**< buffer for certificate */

#define SECBOOT_MAX_KEY_SIZE_IN_BITS (4096) /**< Max public key size supported by secboot (in bits) */
#define SECBOOT_MIN_KEY_SIZE_IN_BITS (2048) /**< minimum cert size to support */

#define SECBOOT_MIN_NUM_CERTS        (2) /**< Minimum number of cert levels in a cert chain*/
#define SECBOOT_MAX_CERT_CHAIN_LEVEL (3) /**< Maximum number of cert levels in a cert chain */
#define SECBOOT_MAX_NUM_ROOT_CERTS   (4) /**< Maximum number of Root Certs for APPS/MODEM */

/** Total number of certs including max root certs */
#define SECBOOT_TOTAL_MAX_CERTS      (SECBOOT_MAX_NUM_ROOT_CERTS + SECBOOT_MAX_CERT_CHAIN_LEVEL - 1)

#define SECBOOT_HANDLE_WORD_SIZE_ENV (0x500/WORD_LEN) /**< The word size of context handle required to run
                                                           secboot. 0x468 is the size in byte for TZ env. */

/*----------------------------------------------------------------------------
 * Define Data Structures
 * -------------------------------------------------------------------------*/
/**
 * Data Type for crypto run-time buffer. Caller MUST provide run-time buffer.
 */
typedef struct crypto_ctx_type {
  uint8    *ctx_imem;  /**< pointer to crypto internal memory, assigned by caller for crypto operation */
  uint32   ctx_imem_size;   /**< Crypto driver to provide the size of crypto run-time buffer */
} crypto_ctx_type;

#endif /* SECBOOT_ENV_H */
