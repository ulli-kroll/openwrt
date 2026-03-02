#ifndef QSEE_UF_DES_H
#define QSEE_UF_DES_H
/*===========================================================================

                             Unified Crypto

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) {2013, 2019} by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/**
@addtogroup qtee_uf_des
@{
    Within Unified DES, the following return values are defined by 
    the Unified Crypto Environment:
   - UC_E_SUCCESS            =  0 
   - UC_E_FAILURE            =  1
   - UC_E_NOT_ALLOWED        =  2
   - UC_E_NO_MEMORY          = 15
   - UC_E_INVALID_ARG        = 16
*/

/** @cond */

/**
 * DES Key and IV sizes
 */
#define SW_DES128_KEY_SIZE        16   /**< DES-128 key size. */
#define SW_DES_IV_SIZE            8    /**< DES initialization vector size. */
#define SW_DES_BLOCK_BYTE_LEN     8    /**< Single DES cipher block length. */
#define SW_3DES_BLOCK_BYTE_LEN    8    /**< Triple DES cipher block length. */
#define SW_DES192_KEY_SIZE        24   /**< DES-192 key size. */
#define SW_DES256_KEY_SIZE        32   /**< DES-256 key size. */
#define SW_DES_MAX_KEY_SIZE       60   /**< Maximum DES key size. */

#define SW_3DES_KEY_BYTE_LEN      24   /**< Triple DES cipher key length. */
#define SW_DES_KEY_BYTE_LEN       8    /**< Single DES cipher key length. */

#ifndef SW_CRYPTO_TYPEDEF
#define SW_CRYPTO_TYPEDEF
typedef uint32_t sw_crypto_errno_enum_type;
#endif

#ifndef IOVECDEF
#define IOVECDEF
typedef  struct
{
  void    *pvBase;
  uint32_t  dwLen;
} __attribute__((__packed__)) IovecType;

typedef  struct
{
  IovecType    *iov;
  uint32_t       size;
} __attribute__((__packed__)) IovecListType;
#endif

#ifndef _DEFINED_CRYPTOTOCNTXHANDLE
#define _DEFINED_CRYPTOTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

/** @endcond */

/**
 * Cipher DES modes.
 */
typedef enum
{
  SW_CIPHER_DES_MODE_ECB          = 0x0,   /**< ECB Mode. */
  SW_CIPHER_DES_MODE_CBC          = 0x1    /**< CBC Mode. */
} SW_CipherDESModeType;

/**
 * Cipher DES direction.
 */
typedef enum
{
  SW_CIPHER_DES_ENCRYPT           = 0x00,      /**< Cipher direction encrypt. */
  SW_CIPHER_DES_DECRYPT           = 0x01,      /**< Cipher direction decrypt. */
  SW_CIPHER_DES_MAX               = 0x7FFFFFFF /**< Invalid direction value. */
} SW_CipherDESEncryptDir;

/**
 * Cipher DES parameters.
 */
typedef enum
{
  SW_CIPHER_DES_PARAM_DIRECTION   = 0x01,      
  /**< Cipher direction (encrypt or decrypt). 
       See SW_CipherDESEncryptDir for valid values. */
  SW_CIPHER_DES_PARAM_KEY         = 0x02,      /**< Key value parameter. */
  SW_CIPHER_DES_PARAM_IV          = 0x03,      /**< Initalization vector (IV). */
  SW_CIPHER_DES_PARAM_MODE        = 0x04,      
  /**< Cipher mode. See SW_CipherDESModeType for valid values. */
  SW_CIPHER_DES_PARAM_BLOCKSIZE   = 0x05,      /**< Cipher block size. */
  SW_CIPHER_DES_PARAM_KEY_SIZE    = 0x06,      
  /**< Key size parameter. See SW_Cipher_DES_Key_Size for valid values. */
  SW_CIPHER_DES_PARAM_IV_SIZE     = 0x07,      /**< IV size parameter. */
  SW_CIPHER_DES_PARAM_COPY        = 0x08,      /**< Create a copy of the DES cipher handle passed as argument. */
  SW_CIPHER_DES_PARAM_MAX         = 0x7FFFFFFF /**< Unknown parameter. */
} SW_CipherDESParam;

/**
 * Cipher DES algorithm type.
 */
typedef enum {
  SW_CIPHER_ALG_DES               = 0x0,       /**< DES Algorithm. */
  SW_CIPHER_ALG_DES3              = 0x1,       /**< Triple DES algorithm. */
  SW_CIPHER_ALG_DES_MAX           = 0x7FFFFFFF /**< Unknown algorithm type. */
} SW_Cipher_DES_Alg_Type;

/**
 * Cipher key size.
 */
typedef enum
{
  SW_CIPHER_KEY_SIZE_DES          = 0x0,       /**< Key size for DES. */
  SW_CIPHER_KEY_SIZE_3DES         = 0x1,       /**< Key size for triple DES. */
  SW_CIPHER_KEY_SIZE_DES_MAX      = 0x7FFFFFFF /**< Unknown key size. */
} SW_Cipher_DES_Key_Size;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  Intializes a cipher context.
 
  @param[out] handle       Double pointer to cipher context handle.
  @param[in] pAlgo         Cipher algorithm type.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_NOT_ALLOWED -- Operation currently not allowed.  \n
  UC_E_NO_MEMORY -- Allocation from a memory pool failed. \n
 */

sw_crypto_errno_enum_type qsee_SW_Cipher_DES_Init(CryptoCntxHandle **handle, SW_Cipher_DES_Alg_Type pAlgo);

/**
  Deintializes cipher context.

  @param[in] handle         Double pointer to cipher context handle.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
 */

sw_crypto_errno_enum_type qsee_SW_Cipher_DES_DeInit(CryptoCntxHandle  **handle);

/**
  Encrypts or decrypts the passed message using the specified algorithm.
 
  @param[in] handle            Pointer to cipher context handle.
  @param[in] ioVecIn           Cipher input vector.
  @param[out] ioVecOut         Pointer to cipher output vector.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */

sw_crypto_errno_enum_type qsee_SW_CipherDESData(CryptoCntxHandle  *handle,
                                                IovecListType      ioVecIn,
                                                IovecListType     *ioVecOut);

/**
  Sets cipher parameters used by qsee_SW_CipherDESData.

  @param[in,out] handle     Pointer to cipher context handle.
  @param[in] nParamID       Cipher parameter ID to set.
  @param[in] pParam         Pointer to parameter data.
  @param[in] cParam         Size of parameter data in bytes.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Cipher_DES_SetParam(CryptoCntxHandle    *handle,
                                                      SW_CipherDESParam    nParamID,
                                                      const void          *pParam,
                                                      uint32_t             cParam );

/** @} */

#endif /* QSEE_UF_DES_H */

