#ifndef QSEE_CIPHER_H
#define QSEE_CIPHER_H

/*
@file qsee_cipher.h
@brief Provide cipher API wrappers
*/

/*===========================================================================
   Copyright (c) 2000-2017,2022,2023 by QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_cipher.h#5 $
  $DateTime: 2023/07/28 01:15:31 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
07/17/23   hk       Added new macros for ECDSA FR
03/07/23   abs      Support for AES using SW keys in ipq53xx
03/22/22   as       Support for FR69399
03/04/22   as       Added Crypto key distribution param set
03/12/18   jp       Added QSEE_CIPHER_BAM_HLOS_HLOS for using API to get pipe pair
05/16/17   shl      Added support for SMMU virtual address cipher
12/8/10    cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include <stdint.h>
/** @cond */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QSEE_AES128_IV_SIZE         16
#define QSEE_AES128_KEY_SIZE        16
#define QSEE_AES256_IV_SIZE         16
#define QSEE_AES256_KEY_SIZE        32
#define QSEE_TRIPLE_DES_KEY_SIZE    24
#define QSEE_TRIPLE_DES_IV_SIZE     8

#define AES_BLOCK_SZ (0x10)
#define DES3_BLOCK_SZ (0x8)

#define AES_CIPHER_ENCRYPT     1
#define AES_CIPHER_DECRYPT     2

// Macros for choosing the curve mode in ecdsa new API
#define ECC_192_BIT_CURVE           1
#define ECC_256_BIT_CURVE           2
#define ECC_384_BIT_CURVE           3

 /** @endcond */
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/**
@addtogroup qtee_cipher
@{
  Within Cipher API, the following return values are defined by
  qsee_shim.h:
  - SUCCESS -- 0
  - FAILURE -- -1
*/

#ifndef QSEE_CIPHER_CTX
#define QSEE_CIPHER_CTX
typedef void qsee_cipher_ctx;
#endif

/**
  QSEE Cipher supports these algorithms:
 */
typedef enum
{
   QSEE_CIPHER_ALGO_AES_128    = 0,
   /**< AES-128 Cipher Algorithm. */
   QSEE_CIPHER_ALGO_AES_256,
   /**< AES-256 Cipher Algorithm. */
   QSEE_CIPHER_ALGO_TRIPLE_DES,
   /**< Triple DES Cipher Algorithm. */
   QSEE_CIPHER_ALGO_INVALID     =0x7FFFFFFF,
   /**< Default or invalid algorithm value. */
} QSEE_CIPHER_ALGO_ET;

/**
  QSEE Cipher parameter types:
 */
typedef enum
{
  QSEE_CIPHER_PARAM_KEY        = 0, /**< Key value parameter. */
  QSEE_CIPHER_PARAM_IV,             /**< Initialization Vector (IV) parameter. */
  QSEE_CIPHER_PARAM_MODE,
  /**< Cipher mode parameter. See QSEE_CIPHER_MODE_ET for valid values. */
  QSEE_CIPHER_PARAM_PAD,
  /**< Cipher padding scheme parameter. See QSEE_CIPHER_PAD_ET for valid values. */
  QSEE_CIPHER_PARAM_NONCE,
  /**< Nonce value parameter. Used in AES-CCM Mode only. */
  QSEE_CIPHER_PARAM_XTS_KEY,
  /**< AES-XTS mode secondary key value parameter. */
  QSEE_CIPHER_PARAM_XTS_DU_SIZE,
  /**< AES-XTS data unit size parameter. */
  QSEE_CIPHER_PARAM_CCM_PAYLOAD_LEN,
  /**< AES-CCM Payload length parameter. */
  QSEE_CIPHER_PARAM_CCM_MAC_LEN,
  /**< AES-CCM MAC length parameter. */
  QSEE_CIPHER_PARAM_CCM_HDR_LEN,
  /**< AES-CCM Header Data length parameter. */
  QSEE_CIPHER_PARAM_BAM_PIPE,
  /**< BAM Pipe parameter. */
  QSEE_CIPHER_PARAM_VA_IN,
  /**< Virtual address cipher input parameter. */
  QSEE_CIPHER_PARAM_VA_IN_LEN,
  /**< Virtual address cipher input parameter length. */
  QSEE_CIPHER_PARAM_VA_OUT,
  /**< Virtual address cipher output parameter. */
  QSEE_CIPHER_PARAM_VA_OUT_LEN,
  /**< Virtual address cipher output parameter length. */
  QSEE_CIPHER_PARAM_COPY,
  /* Cipher operation using HW key */
  QSEE_CIPHER_PARAM_HW_KEY_TYPE,
  /** Distribute HW key to crypto */
  QSEE_CIPHER_PARAM_DIS_HW_KEY,
  /* Cipher operation using PROD_SEED Key*/
  QSEE_CIPHER_PARAM_PROD_SEED_KEY,
  /*Cipher CRBK Key*/
  QSEE_CIPHER_PARAM_CRBK_KEY,
  /**< Boolean indicating if copying of cipher is allowed. */
  QSEE_CIPHER_PARAM_INVALID = 0x7FFFFFFF,
  /**< Unknown or invalid parameter. */
} QSEE_CIPHER_PARAM_ET;

/**
  Supported modes of operation:
 */
typedef enum
{
   QSEE_CIPHER_MODE_ECB        = 0,       /**< Electronic Codebook mode. */
   QSEE_CIPHER_MODE_CBC,                  /**< Cipher block chaining mode. */
   QSEE_CIPHER_MODE_CTR,                  /**< Counter mode. */
   QSEE_CIPHER_MODE_XTS,
   /**< XEX-based tweaked codebook mode with ciphertext stealing. */
   QSEE_CIPHER_MODE_CCM,                  /**< Counter mode with CBC-MAC. */
   QSEE_CIPHER_MODE_CTS,                  /**< Cipher text stealing mode. */
   QSEE_CIPHER_MODE_INVALID = 0x7FFFFFFF, /**< Unknown or invalid mode. */
} QSEE_CIPHER_MODE_ET;

/**
  QSEE Cipher padding algorithms:
 */
typedef enum
{
  QSEE_CIPHER_PAD_ISO10126,             /**< Padding mode defined by ISO 10126. */
  QSEE_CIPHER_PAD_PKCS7,
  /**< PKCS7 padding mode as described in Internet Engineering
       Task Force's RFC5652. */
  QSEE_CIPHER_PAD_NO_PAD,               /**< No padding added. */
  QSEE_CIPHER_PAD_INVALID = 0x7FFFFFFF, /**< Unknown or invalid padding mode. */
} QSEE_CIPHER_PAD_ET;

/**
  QSEE Cipher Bit Accurate Model (BAM) Pipe:
  */
typedef enum
{
  QSEE_CIPHER_BAM_GENERIC        = 0x00,  /**< Generic TZ use case */
  QSEE_CIPHER_BAM_CPB_CPB        = 0x01,
  /** Pattern use case and other use cases where scratch memory is used in
      the app */
  QSEE_CIPHER_BAM_HLOS_CPB       = 0x02,  /**< Video DRM Decrypt. */
  QSEE_CIPHER_BAM_CPB_HLOS       = 0x03,  /**< HDCP Encryption. */
  QSEE_CIPHER_BAM_HLOS_HLOS      = 0x04,  /**< DRM Audio use case. */
  QSEE_CIPHER_BAM_INVALID                 /**< Unknown or invalid pipe option. */
} QSEE_CIPHER_BAM_PIPE_ET;

typedef enum
{
  QSEE_CIPHER_HW_KEY_OEM_PROD_SEED = 1,
  QSEE_CIPHER_HW_KEY_CRBK,
  QSEE_CIPHER_HW_KEY_INVALID = 0x7FFFFFF,
} QSEE_CIPHER_HW_KEY_TYPE;

typedef struct {
  uint32_t derived_key_handle;
  uint32_t derived_key_input_src;
} Crypto_HWKey_distribution_param;

typedef struct {
  uint32_t operations;
  uint32_t algorithm;
} HWKey_operationPolicy;

typedef struct {
  HWKey_operationPolicy opPolicy;
  uint32_t kdfDepth;
  uint32_t permissions;
  uint32_t keyType;
  uint32_t destination;
} HWKey_policy;

typedef struct {
  uint32_t bindings;
  uint32_t contextLen;
  uint8_t context[64];
} HWKey_bindings;

typedef struct crypto_key_derive_cmd
{  HWKey_policy policy;
   HWKey_bindings bindings;
   uint32_t source;
   uint64_t mixkey;
   uint64_t keyhandle;
} qsee_key_derive_cmd_t;

typedef struct qsee_crypto_aes_cmd
{   uint64_t keyhandle;
    uint64_t type;
    uint64_t mode;
    uint8_t* req_buf;
    uint64_t reqlen;
    uint8_t* iv_buf;
    uint64_t ivlen;
    uint64_t resp_buf;
    uint64_t resplen;
} qsee_crypto_aes_cmd_t; 

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
 /**
   Intializes a cipher context for encrypt or decrypt operation.

   @caution1hang The QSEE_CIPHER_ALGO_TRIPLE_DES cipher algorithm is not supported
   by the QTEE off-target environment.

   @param[in] alg         Algorithm standard to use.
   @param[out] cipher_ctx Double pointer to cipher context.

   @return
   SUCCESS -- Function executes successfully. \n
   FAILURE -- Any error encountered during cipher initialization.
 */
int qsee_cipher_init(QSEE_CIPHER_ALGO_ET alg,
                     qsee_cipher_ctx  **cipher_ctx);

/**
  Releases all resources with a given cipher context.

  @param[in] cipher_ctx Pointer to the cipher context to be deleted.

  @return
  SUCCESS -- Function executes successfully. \n
  FAILURE -- Any error encountered during context free.
 */
int qsee_cipher_free_ctx(qsee_cipher_ctx *cipher_ctx);


/**
  Resets the cipher context; does not reset the key.

  @param[in,out] cipher_ctx Pointer to cipher context.

  @return
  SUCCESS -- Function executes successfully. \n
  FAILURE -- Any error encountered during cipher reset.
 */
int qsee_cipher_reset(qsee_cipher_ctx *cipher_ctx);

/**
  Modifies the parameters for a given cipher operation.

  @caution1hang The QSEE_CIPHER_MODE_CTS and QSEE_CIPHER_MODE_XTS cipher modes
  are not supported by the QTEE off-target environment.

  @param[in,out] cipher_ctx  Pointer to cipher context.
  @param[in] param_id        Parameter to modify.
  @param[in] param           Pointer to parameter value to set.
  @param[in] param_len       Parameter length (in bytes).

  @return
  SUCCESS -- Function executes successfully. \n
  FAILURE -- Any error encountered during parameter setting.
 */
int qsee_cipher_set_param(qsee_cipher_ctx *cipher_ctx,
                          QSEE_CIPHER_PARAM_ET param_id,
                          const void *param,
                          uint32_t param_len);
/**
  Retrieves the parameters for a given cipher context.

  @param[in] cipher_ctx      Pointer to cipher context.
  @param[in] param_id        Parameter to retrieve.
  @param[out] param          Pointer to memory location to store the parameter.
  @param[in,out] param_len   Pointer to param length (in bytes). \n
                             @note1 Length value is modified to the actual
                             length of param.

  @return
  SUCCESS -- Function executes successfully. \n
  FAILURE -- Any error encountered during parameter retrieval.
 */
int qsee_cipher_get_param(const qsee_cipher_ctx *cipher_ctx,
                          QSEE_CIPHER_PARAM_ET param_id,
                          void *param,
                          uint32_t *param_len);

/**
  Encrypts the passed plaintext message using the specified algorithm.

  @param[in] cipher_ctx         Pointer to cipher context.
  @param[in] pt                 Pointer to input plaintext buffer.
  @param[in] pt_len             Input plaintext buffer length (in bytes).
  @param[out] ct                Pointer to output ciphertext buffer.
  @param[in,out] ct_len         Pointer to output ciphertext buffer length. \n
                                @note1 Length value is modified to the actual
                                ciphertext bytes written.

  @detdesc
  The memory allocated for the ciphertext must be large enough to hold the
  plaintext equivalent. If a padding scheme is selected, the ciphertext buffer
  length may need to be up to one block size larger than the plaintext length.
  If the output buffer is not large enough to hold the encrypted results,
  an error is returned.

  @return
  SUCCESS -- Function executes successfully. \n
  FAILURE -- Any error encountered during encryption.
 */
int qsee_cipher_encrypt(const qsee_cipher_ctx *cipher_ctx,
                        const uint8_t *pt,
                        uint32_t pt_len,
                        uint8_t *ct,
                        uint32_t *ct_len);

/**
  Decrypts the passed ciphertext message using the specified algorithm.

  @param[in] cipher_ctx         Pointer to cipher context.
  @param[in] ct                 Pointer to input ciphertext buffer.
  @param[in] ct_len             Input ciphertext buffer length (in bytes).
  @param[out] pt                Pointer to output plaintext buffer.
  @param[in,out] pt_len         Pointer to output plaintext buffer length. \n
                                @note1 Value is modified to the actual plaintext
                                bytes written.

  @detdesc
  The memory allocated for the plaintext must be large enough to hold the
  ciphertext equivalent. If a padding scheme is selected, the plaintext output
  length may be up to one block size smaller than the ciphertext length.
  If the output buffer is not large enough to hold the decrypted results,
  an error is returned.

  @return
  SUCCESS -- Function executes successfully. \n
  FAILURE -- Any error encountered during decryption.
 */
int qsee_cipher_decrypt(const qsee_cipher_ctx *cipher_ctx,
                        const uint8_t *ct,
                        uint32_t ct_len,
                        uint8_t* pt,
                        uint32_t *pt_len);

/** @} */

#endif /*QSEE_CIPHER_H*/

