#ifndef QSEE_UF_AES_H
#define QSEE_UF_AES_H

/**
@file qsee_uf_aes.h
@brief Provide Software crypto Cipher API wrappers
*/

/*===========================================================================
Copyright (c) {2012, 2017, 2019} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/**
@addtogroup qtee_uf_aes
@{
  Within Unified AES, the following return values are defined by 
  the Unified Crypto Environment:
  - UC_E_SUCCESS            =  0 
  - UC_E_FAILURE            =  1
  - UC_E_NOT_ALLOWED        =  2
  - UC_E_NOT_AVAILABLE      =  3
  - UC_E_NOT_SUPPORTED      =  4
  - UC_E_NO_MEMORY          = 15
  - UC_E_INVALID_ARG        = 16
*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define SW_AES128_KEY_SIZE         16  /**< Fixed Key Size for AES128. */
#define SW_AES192_KEY_SIZE         24  /**< Fixed Key Size for AES192. */
#define SW_AES256_KEY_SIZE         32  /**< Fixed Key Size for AES256. */

/** Maximum buffer size for AES encryption */
#define SW_AES_MAX_KEY_SIZE        60

 /** Fixed AES IV Size for cipher modes other than CCM and GCM. */
#define SW_AES_IV_SIZE             16

 /** Maximum IV Length for CCM and GCM cipher modes. */
#define SW_AES_MAX_IV_SIZE         60
#define SW_AES_BLOCK_BYTE_LEN      16  /**< Fixed AES Block Byte Length */
#define SW_AES_CCM_MIN_NONCE_SIZE  7   /**< Minimum nonce length for AES CCM */
#define SW_AES_CCM_MAX_NONCE_SIZE  13  /**< Maximum nonce length for AES CCM */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/** @cond */
#ifndef SW_CRYPTO_TYPEDEF
#define SW_CRYPTO_TYPEDEF
typedef uint32_t sw_crypto_errno_enum_type;
#endif

#ifndef IOVECDEF
#define IOVECDEF
typedef struct
{
  void                              *pvBase;
  uint32_t                            dwLen;
} __attribute__((__packed__)) IovecType;

typedef struct
{
  IovecType                     *iov;
  uint32_t                        size;
} __attribute__((__packed__)) IovecListType;
#endif

#ifndef _DEFINED_CRYPTOTOCNTXHANDLE
#define _DEFINED_CRYPTOTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

/** @endcond */

/** 
  Cipher algorithm types.
 */
typedef enum {
  SW_CIPHER_ALG_AES128            = 0x0,       /**< AES-128 cipher algorithm. */
  SW_CIPHER_ALG_AES256            = 0x1,       /**< AES-256 cipher algorithm. */
  SW_CIPHER_ALG_AES192            = 0x2,       /**< AES-192 cipher algorithm. */
  SW_CIPHER_ALG_MAX               = 0x7FFFFFFF /**< Unknown or invalid algorithm. */
} SW_Cipher_Alg_Type;

/**
  Cipher parameters.
 */
typedef enum
{
  SW_CIPHER_PARAM_DIRECTION   = 0x01,       
  /**< Cipher direction. See SW_CipherEncryptDir. */
  SW_CIPHER_PARAM_KEY         = 0x02,       /**< Key value parameter. */
  SW_CIPHER_PARAM_IV          = 0x03,       
  /**< Initialization vector parameter. */
  SW_CIPHER_PARAM_MODE        = 0x04,       /**< Cipher mode parameter. */
  SW_CIPHER_PARAM_BLOCKSIZE   = 0x05,       /**< Cipher block size. */
  SW_CIPHER_PARAM_KEY_SIZE    = 0x06,       /**< Key size parameter. */
  SW_CIPHER_PARAM_IV_SIZE     = 0x07,       
  /**< Initialization vector parameter. */
  SW_CIPHER_PARAM_AAD         = 0x08,
  /**< Additional plaintext data for AES-GCM */
  SW_CIPHER_PARAM_TAG         = 0x09,
  /**< Calculated TAG by AES-GCM */
  SW_CIPHER_PARAM_NONCE       = 0x10,       /**< Nonce value parameter. */
  SW_CIPHER_PARAM_PAYLOAD_LEN = 0x11,       
  /**< AES-CCM Payload length parameter. */
  SW_CIPHER_PARAM_XTS_DU_SIZE = 0x12,       
  /**< AES-XTS data unit size parameter. */
  SW_CIPHER_PARAM_XTS_KEY     = 0x13,       
  /**< AES-XTS mode secondary key value parameter. */
  SW_CIPHER_PARAM_XTS_TWK_SET = 0x14,       
  /**< AES-XTS mode tweak set boolean parameter. */
  SW_CIPHER_PARAM_COPY        = 0x15,       
  /**< Set current cipher context to be a copy of the context passed as a parameter. */
  SW_CIPHER_PARAM_MAX         = 0x7FFFFFFF  /**< Unknown or invalid parameter. */
} SW_CipherParam;

/**
  Cipher direction (encrypt or decrypt).
 */
typedef enum
{
  SW_CIPHER_ENCRYPT           = 0x00,      /**< Direction encrypt. */
  SW_CIPHER_DECRYPT           = 0x01,      /**< Direction decrypt. */
  SW_CIPHER_MAX               = 0x7FFFFFFF /**< Invalid value. */
} SW_CipherEncryptDir;

/**
  Cipher key sizes.
 */
typedef enum
{
  SW_CIPHER_KEY_SIZE_AES128      = 0x0,       /**< Key size for AES-128. */
  SW_CIPHER_KEY_SIZE_AES256      = 0x1,       /**< Key size for AES-256. */
  SW_CIPHER_KEY_SIZE_AES192      = 0x2,       /**< Key size for AES-192. */
  SW_CIPHER_KEY_SIZE_MAX         = 0x7FFFFFFF /**< Unknown key size. */
} SW_Cipher_Key_Size;

/**
  Cipher modes.
 */
typedef enum
{
  SW_CIPHER_MODE_ECB          = 0x0,       /**< Electronic Codebook mode. */
  SW_CIPHER_MODE_CBC          = 0x1,       /**< Cipher block chaining mode. */
  SW_CIPHER_MODE_CTR          = 0x2,       /**< Counter mode. */
  SW_CIPHER_MODE_GCM          = 0x3,       /**< Galois Counter mode. */
  SW_CIPHER_MODE_CCM          = 0x4,       /**< Counter mode with CBC-MAC. */
  SW_CIPHER_MODE_CTS          = 0x5,       /**< Cipher text stealing mode. */
  SW_CIPHER_MODE_XTS          = 0x6,       
  /**< XEX-based tweaked codebook mode with ciphertext stealing. */
  SW_CIPHER_MODE_INVALID      = 0x7FFFFFFF /**< Invalid cipher mode. */
} SW_CipherModeType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  Initialize a cipher context
 
  @param[out] handle     Double pointer to cipher context handle.
  @param[in] pAlgo       Cipher algorithm type.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_NOT_ALLOWED -- Operation currently not allowed.  \n
  UC_E_NOT_SUPPORTED -- Operation not yet implemented  . \n
  UC_E_NO_MEMORY -- Allocation from a memory pool failed. \n
 */
sw_crypto_errno_enum_type qsee_SW_Cipher_Init(CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);

/**
  Resets a cipher context; except keys.
 
  @param[in,out] handle      Pointer to cipher context handle.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_INVALID_ARG -- Invalid context handle. \n
 */
sw_crypto_errno_enum_type qsee_SW_Cipher_Reset(CryptoCntxHandle *handle);

/**
  Encrypts or decrypts passed message using the specified algorithm.
 
  @param[in] handle        Pointer to cipher context handle.
  @param[in] ioVecIn       Cipher input vector.
  @param[out] ioVecOut     Pointer to cipher output vector.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_CipherData (CryptoCntxHandle *handle, IovecListType ioVecIn,
                                              IovecListType *ioVecOut);

/**
  Sets cipher parameters.
 
  @param[in,out] handle Pointer to cipher context handle.
  @param[in] nParamID   Cipher parameter ID to set.
  @param[in] pParam     Pointer to parameter data.
  @param[in] cParam     Parameter data size (in bytes).
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Cipher_SetParam (CryptoCntxHandle *handle, SW_CipherParam nParamID,
                                                   const void *pParam, uint32_t cParam );


/**
  Gets cipher parameters.
 
  @param[in] handle     Pointer to cipher context handle.
  @param[in] nParamID   Cipher parameter ID to get.
  @param[out] pParam    Pointer to parameter data.
  @param[in] cParam     Parameter data size (in bytes).
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Cipher_GetParam (CryptoCntxHandle *handle, SW_CipherParam nParamID,
                                                   void *pParam, uint32_t cParam );

/**
  Deintializes a cipher context.

  @param[in] handle       Double pointer to cipher context handle.
  @param[in] pAlgo        Cipher algorithm type.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Cipher_DeInit(CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);


/**
  Updates the AE operation with the passed message 
  using the specified algorithm.
 
  @param[in] handle     Pointer to AE context handle.
  @param[in] ioVecIn    AE input vector.
  @param[out] ioVecOut  Pointer to AE output vector.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_AE_UpdateData(CryptoCntxHandle *handle, IovecListType ioVecIn,
                                                IovecListType *ioVecOut);


/**
  Finalizes the Authenticated Encryption (AE) operation with the passed message 
  using the specified algorithm, and computes the tag obtained 
  by calling SW_Cipher_GetParam.
 
  @param[in] handle     Pointer to AE context handle.
  @param[in] ioVecIn    AE input vector.
  @param[out] ioVecOut  Pointer to AE output vector.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_AE_FinalData(CryptoCntxHandle *handle, IovecListType ioVecIn,
                                                IovecListType *ioVecOut);

/**
  Updates the Additional Authenticated Data (AAD) using the specified algorithm.
 
  @param[in] handle     Pointer to AE context handle.
  @param[in,out] aad    Pointer to AAD.
  @param[in] aadlen     Length of AAD.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_AE_UpdateAAD(CryptoCntxHandle *handle, void *aad,
                                               uint32_t aadlen);

/** @} */

#endif /* QSEE_UF_AES_H */
