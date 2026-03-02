#ifndef QSEE_HASH_H
#define QSEE_HASH_H

/**
@file qsee_hash.h
@brief Provide hash API wrappers
*/

/*===========================================================================
   Copyright (c) 2010-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
11/22/17   jp       Added SHA384 support
08/20/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/**
@addtogroup qtee_hash
@{
  qsee_shim.h defines the following return values:
  - SUCCESS --  0
  - FAILURE -- -1
*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QSEE_SHA1_HASH_SZ    20
#define QSEE_SHA224_HASH_SZ  28
#define QSEE_SHA256_HASH_SZ  32
#define QSEE_SHA384_HASH_SZ  48
#define QSEE_SHA512_HASH_SZ  64

#define QSEE_HASH_SUCCESS 0
#define QSEE_HASH_FAILURE 1

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
 /** @cond */
typedef void qsee_hash_ctx;
#ifndef QSEE_CIPHER_CTX
#define QSEE_CIPHER_CTX
typedef void qsee_cipher_ctx;
#endif

/** @endcond */

/**
  Hashing algorithm.
 */
typedef enum
{
  QSEE_HASH_NULL         = 1,      /**< Do not perform any hashing. */
  QSEE_HASH_SHA1         = 2,      /**< SHA1 Hash algorithm. */
  QSEE_HASH_SHA256       = 3,      /**< SHA256 Hash algorithm. */
  QSEE_HASH_SHA384       = 4,      /**< SHA384 Hash algorithm. */
  QSEE_HASH_SHA512       = 5,      /**< SHA512 Hash algorithm. */
  QSEE_HASH_SHA224       = 6,      /**< SHA224 Hash algorithm. */
  QSEE_HASH_INVALID = 0x7FFFFFFF,  /**< Unknown or invalid hash algorithm. */
} QSEE_HASH_ALGO_ET;

/**
  Parameters for hash.
 */
typedef enum
{
  QSEE_HASH_PARAM_MODE     = 0,          /**< Hashing mode parameter. */
  QSEE_HASH_PARAM_HMAC_KEY = 1,          /**< HMAC key value parameter. */
  QSEE_HASH_PARAM_SEQ      = 2,          /**< Hashing sequence parameter. */
  QSEE_HASH_PARAM_INVALID  = 0x7FFFFFFF, /**< Unknown or invalid parameter. */
} QSEE_HASH_PARAM_ET;

/**
  Hashing modes.
 */
typedef enum
{
  QSEE_HASH_MODE_HASH    = 0,          /**< Plain SHA. */
  QSEE_HASH_MODE_HMAC    = 1,          /**< HMAC SHA. */
  QSEE_HASH_MODE_INVALID = 0x7FFFFFFF, /**< Unknown or invalid hash mode. */
} QSEE_HASH_MODE_ET;

/**
  Hashing sequences.
 */
typedef enum
{
  QSEE_HASH_MODE_FIRST   = 0,           /**< First block position. */
  QSEE_HASH_MODE_LAST    = 1,           /**< Last block position. */
  QSEE_HASH_MODE_INVALID2 = 0x7FFFFFFF,
  /**< Unknown or invalid hashing sequence position. */
} QSEE_HASH_SEQ_ET;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  Creates a message digest hash using the specified algorithm.

  @param[in] alg          Hash algorithm.
  @param[in] msg          Pointer to message to hash.
  @param[in] msg_len      Message length.
  @param[out] digest      Pointer to digest to store.
  @param[in] digest_len   Length of the output digest buffer (in bytes).
                          Must be at least equal to hash size for requested hash algorithm.

  @return
  QSEE_HASH_SUCCESS -- Function executes successfully \n
  QSEE_HASH_FAILURE -- Any error encountered during hash creation
 */
int qsee_hash(QSEE_HASH_ALGO_ET alg,
              const uint8_t    *msg,
              uint32_t          msg_len,
              uint8_t          *digest,
              uint32_t          digest_len);

/**
  Intializes a hash context for update and final functions.

  @param[in]  alg       Algorithm standard to use.
  @param[out] hash_ctx  Double pointer to hash context.

  @return
  QSEE_HASH_SUCCESS -- Function executes successfully \n
  QSEE_HASH_FAILURE -- Any error encountered during hash initialization
 */
int qsee_hash_init(QSEE_HASH_ALGO_ET alg,
                   qsee_hash_ctx **hash_ctx);


/**
  Resets hash context; does not reset keys.

  @param[in,out] hash_ctx  Pointer to hash context.

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error encountered during hash reset
 */
int qsee_hash_reset(qsee_hash_ctx *hash_ctx);

/**
  Hashes some data into the hash context structure (must be initialized by qsee_hash_init()).

  @param[in,out] hash_ctx    Pointer to hash context.
  @param[in] msg             Pointer to data to hash.
  @param[in] msg_len         Data length to hash.

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error encountered during hash update

  @dependencies
  qsee_hash_init
 */
int qsee_hash_update(const qsee_hash_ctx    *hash_ctx,
                     const uint8_t          *msg,
                     uint32_t                msg_len);

/**
  Computes the digest hash value.

  @param[in] hash_ctx     Pointer to hash context.
  @param[out] digest      Pointer to output message digest hash.
  @param[in] digest_len   Length of the output digest buffer (in bytes).
                          Must be at least equal to hash size for requested hash algorithm.

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error encountered during final hash calculation

  @dependencies
  qsee_hash_init
 */
int qsee_hash_final(const qsee_hash_ctx  *hash_ctx,
                    uint8_t                 *digest,
                    uint32_t                digest_len);

/**
  Releases all resources with a given hash context.

  @param[in] hash_ctx Pointer to hash context for deletion.

  @return
  QSEE_HASH_SUCCESS -- Function executes successfully \n
  QSEE_HASH_FAILURE -- Any error encountered while freeing hash context
 */
int qsee_hash_free_ctx(qsee_hash_ctx *hash_ctx);

/**
  Performs Hash and Cipher Encrypt operation simultaneously.

  The memory allocated for the output cipher text buffer must be large enough to
  hold the plaintext equivalent. If a padding scheme is selected, the ciphertext
  buffer length may be up to one block size larger than the plaintext length.
  If the output buffer is not large enough to hold the encrypted results,
  an error is returned.

  @param[in] cipher_ctx   Pointer to cipher context.
  @param[in] hash_ctx     Pointer to hash context.
  @param[in] pt           Pointer to input plaintext buffer.
  @param[in] pt_len       Input plaintext buffer length.
  @param[out] ct          Pointer to output ciphertext buffer.
  @param[in] ct_len       Output ciphertext buffer length.
  @param[out] digest      Pointer to digest to store.
  @param[in] digest_len   Length of the output digest buffer (in bytes).
                          Must be at least equal to hash size for requested hash algorithm.

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error encountered during encryption
 */
int qsee_hashcipher_encrypt(const qsee_cipher_ctx *cipher_ctx,
                            const qsee_hash_ctx   *hash_ctx,
                            uint8_t                 *pt,
                            uint32_t                 pt_len,
                            uint8_t                 *ct,
                            uint32_t                 ct_len,
                            uint8_t                 *digest,
                            uint32_t                 digest_len);


/**
  Performs Hash and Cipher Decrypt operation simultaneously.

  The memory allocated for the output plaintext buffer must be large enough to
  hold the ciphertext equivalent. If a padding scheme is selected, the plaintext
  output length may be up to one block size smaller than the ciphertext length.
  If the output buffer is not large enough to hold the decrypted results,
  an error is returned.

  @param[in] cipher_ctx   Pointer to cipher context.
  @param[in] hash_ctx     Pointer to the hash context.
  @param[in] ct           Pointer to input ciphertext buffer.
  @param[in] ct_len       Input ciphertext buffer length.
  @param[out] pt          Pointer to output plaintext buffer.
  @param[in] pt_len       Output plaintext buffer length.
  @param[out] digest      Pointer to digest to store.
  @param[in] digest_len   Length of the output digest buffer (in bytes).
                          Must be at least equal to hash size for requested hash algorithm.

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error encountered during decryption
 */
int qsee_hashcipher_decrypt(const qsee_cipher_ctx   *cipher_ctx,
                            const qsee_hash_ctx     *hash_ctx,
                            uint8_t                 *ct,
                            uint32_t                 ct_len,
                            uint8_t                 *pt,
                            uint32_t                 pt_len,
                            uint8_t                 *digest,
                            uint32_t                 digest_len);

/**
  Modifies parameters for a given hash operation.

  @caution1hang  These parameters are not supported by the QTEE
  off-target environment:
  - QSEE_HASH_PARAM_HMAC_KEY
  - QSEE_HASH_PARAM_SEQ

  @caution1hang QSEE_CIPHER_MODE_CCM mode is not supported by the QTEE
  off-target environment.

  @param[in] hash_ctx   Pointer to hash context.
  @param[in] param_id   Parameter to modify.
  @param[in] param      Pointer to parameter value to set.
  @param[in] param_len  Param length (in bytes).

  @return
  SUCCESS -- Function executes successfully \n
  FAILURE -- Any error encountered during parameter setting
 */
int qsee_hash_set_param(const qsee_hash_ctx *hash_ctx,
                        QSEE_HASH_PARAM_ET   param_id,
                        const void          *param,
                        uint32_t             param_len);

/** @} */

#endif /*QSEE_HASH_H*/

