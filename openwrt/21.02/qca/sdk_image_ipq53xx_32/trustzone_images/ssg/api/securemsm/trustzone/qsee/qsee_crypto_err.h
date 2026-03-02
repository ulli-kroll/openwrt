#ifndef QSEE_CRYPTO_ERR_H
#define QSEE_CRYPTO_ERR_H

/**
@file qsee_crypto_err.h
@brief Defines crypto-engine error values
*/

/*===========================================================================
Copyright (c) {2012, 2018} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/**
@addtogroup qtee_pkey
@{

*/

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
 ===========================================================================*/

/*
 * Error Modules
 */
#define QSEE_UCM_CMN                0 /* Common errors */
#define QSEE_UCM_ENV                1 /* Environment module errors */
#define QSEE_UCM_CEHW               2 /* Crypto Engine HWIO HAL Driver moduele errors */
#define QSEE_UCM_CEBAM              3 /* Crypto Engine BAM HAL Driver module errors */
#define QSEE_UCM_HASH               4 /* Hash module errors */
#define QSEE_UCM_CIPHER             5 /* Cipher module errors */
#define QSEE_UCM_AES                6 /* AES module errors */
#define QSEE_UCM_DES                7 /* DES module errors */
#define QSEE_UCM_PRNG               8 /* PRNG module errors */
#define QSEE_UCM_PKEY               9 /* Public-Key module errors */
#define QSEE_UCM_BIGINT            10 /* Crypto Big-Integer module errors */
#define QSEE_UCM_MAC               11 /* MAC module errors */
#define QSEE_UCM_KDF               12 /* KDF module errors */

/*
 Macro to create error code.
 31*******************20**********************************0
 +--------------------+-----------------------------------+
 |    12 bits for     |          20 bits
 |    Module Name     |        error code
 +--------------------+-----------------------------------+
*/
#define QSEE_CRYPTO_GET_EC(mod, ec)    ((((mod) & 0xFFF) << 20) + ((ec) & 0xFFFFF))
#define QSEE_CRYPTO_ERR(mod, ec)       ((int)QSEE_CRYPTO_GET_EC(mod, ec))

/**
 * Error return values
 */
enum qsee_crypto_err_enum
{
  /** Common QSEE Crypto Error Codes */
  QSEE_CRYPTO_SUCCESS                  = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 0),         /**< Operation successful                 */
  QSEE_CRYPTO_ERR_FAILURE              = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 1),         /**< Operation failed due to unknown err  */
  QSEE_CRYPTO_ERR_INVALID_ARG          = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 2),         /**< Arg is not recognized                */
  QSEE_CRYPTO_ERR_OUT_OF_RANGE         = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 3),         /**< Arg value is out of range            */
  QSEE_CRYPTO_ERR_VERSION_MISMATCH     = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 4),         /**< Unexpected software or protocol ver. */
  QSEE_CRYPTO_ERR_NOT_SUPPORTED        = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 5),         /**< Operation not yet implemented        */
  QSEE_CRYPTO_ERR_INVALID_CONTEXT      = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 6),         /**< Invalid context                      */
  QSEE_CRYPTO_ERR_OUT_OF_MEMORY        = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 7),         /**< Out of memory                        */
  QSEE_CRYPTO_ERR_INV_DGST_SIZE        = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 9),         /**< Invalid digest size                  */
  QSEE_CRYPTO_ERR_INV_DGST_CONTEXT     = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 10),        /**< Invalid digest context               */
  QSEE_CRYPTO_ERR_ADD_OVERFLOW         = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 11),        /**< Addition Overflow                    */
  QSEE_CRYPTO_ERR_HEAP_LITE_FAILED     = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 12),        /**< Heap API failed                      */
  QSEE_CRYPTO_ERR_SELF_TEST_FAILED     = QSEE_CRYPTO_ERR(QSEE_UCM_CMN, 13),        /**< Self test failed                     */

  /** Environment Error Codes */
  QSEE_CRYPTO_ERR_SET_CLK_BW           = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 1),         /**< Set Clock BW failed                  */
  QSEE_CRYPTO_ERR_VTOP_FAILED          = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 2),         /**< VA to PA failed                      */
  QSEE_CRYPTO_ERR_MUTEX_INIT           = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 3),         /**< Mutex Init API failed                */
  QSEE_CRYPTO_ERR_MUTEX_LOCK           = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 4),         /**< Mutex Lock API failed                */
  QSEE_CRYPTO_ERR_MUTEX_RELEASE        = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 5),         /**< Mutex Release API failed             */
  QSEE_CRYPTO_ERR_HEAP_NOT_SUPPORTED   = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 6),         /**< Heap API not supported               */
  QSEE_CRYPTO_ERR_HEAP_INIT_FAILED     = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 7),         /**< Heap init. failed                    */
  QSEE_CRYPTO_ERR_INV_BAM_CTX_SIZE     = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 8),         /**< Invalid BAM Driver CTX size          */
  QSEE_CRYPTO_ERR_INV_BAM_CTX          = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 9),         /**< Invalid BAM Driver CTX               */
  QSEE_CRYPTO_ERR_PRNG_CLK_FAILED      = QSEE_CRYPTO_ERR(QSEE_UCM_ENV, 10),        /**< PRNG Clock Vote API failed           */

  /** CE HWIO HAL Driver Error Codes */
  QSEE_CRYPTO_ERR_INV_AUTH_XFER_SZ     = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 1),       /**< Invalid Auth Transfer size            */
  QSEE_CRYPTO_ERR_INV_CIPHER_XFER_SZ   = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 2),       /**< Invalid Cipher Transfer size          */
  QSEE_CRYPTO_ERR_CE_HAL_CTX_INVALID   = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 3),       /**< Invalid CE HAL Context                */
  QSEE_CRYPTO_ERR_HAL_MAC_FAILED       = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 4),       /**<                                       */
  QSEE_CRYPTO_ERR_HAL_ACCESS_VIOL      = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 5),       /**<                                       */
  QSEE_CRYPTO_ERR_HAL_PIPE_ACTIVE_ERR  = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 6),       /**<                                       */
  QSEE_CRYPTO_ERR_HAL_CFG_CHNG_ERR     = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 7),       /**<                                       */
  QSEE_CRYPTO_ERR_HAL_DOUT_ERR         = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 8),       /**<                                       */
  QSEE_CRYPTO_ERR_HAL_DIN_ERR          = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 9),       /**<                                       */
  QSEE_CRYPTO_ERR_HAL_AXI_ERR          = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 10),      /**<                                       */
  QSEE_CRYPTO_ERR_HAL_CE_BUSY_ERR      = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 11),      /**<                                       */
  QSEE_CRYPTO_ERR_HAL_SET_IV_FAILED    = QSEE_CRYPTO_ERR(QSEE_UCM_CEHW, 12),      /**<                                       */

  /** CE BAM HAL Driver Error Codes */
  QSEE_CRYPTO_ERR_BAM_INIT_FAILED      = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 1),      /**< BAM driver init. failed               */
  QSEE_CRYPTO_ERR_BAM_INIT_TXPIPE      = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 2),      /**< BAM Tx pipe init. failed              */
  QSEE_CRYPTO_ERR_BAM_INIT_RXPIPE      = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 3),      /**< BAM Rx pipe init. failed              */
  QSEE_CRYPTO_ERR_BAM_TX_IRQ_MODE      = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 4),      /**< BAM Tx IRQ Mode set failed            */
  QSEE_CRYPTO_ERR_BAM_RX_IRQ_MODE      = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 5),      /**< BAM Rx IRQ Mode set failed            */
  QSEE_CRYPTO_ERR_BAM_XFR_CMD_DSC      = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 6),      /**< BAM Command Descriptor Xfer failed    */
  QSEE_CRYPTO_ERR_BAM_XFR_DAT_DSC      = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 7),      /**< BAM Data Descriptor Xfer failed       */
  QSEE_CRYPTO_ERR_BAM_XFR_RSLT_DSC     = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 8),      /**< BAM Result Dump Xfer failed           */
  QSEE_CRYPTO_ERR_BAM_BUF_PA_XLATE     = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 9),      /**< Data buffer VTOP failed               */
  QSEE_CRYPTO_ERR_BAM_PIPE_UNLCK       = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 10),     /**< BAM pipe unlock request failed        */
  QSEE_CRYPTO_ERR_BAM_TX_PIPE_POLL     = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 11),     /**< Tx pipe poll API failed               */
  QSEE_CRYPTO_ERR_BAM_RX_PIPE_POLL     = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 12),     /**< Rx pipe poll API failed               */
  QSEE_CRYPTO_ERR_BAM_PIPE_BUSY        = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 13),     /**< Tx/Rx pipe is still busy              */
  QSEE_CRYPTO_ERR_BAM_NO_ENG_EN        = QSEE_CRYPTO_ERR(QSEE_UCM_CEBAM, 14),     /**<               */

  /** CE Cipher Error Codes */
  QSEE_CRYPTO_ERR_CIPHER_INV_KEY_PTR      = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 1),     /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_KEY_SZ       = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 2),     /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_IV_PTR       = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 3),     /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_IV_SZ        = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 4),     /**<                */
  QSEE_CRYPTO_ERR_CIPHER_NO_PADDING       = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 5),     /**<                */
  QSEE_CRYPTO_ERR_CIPHER_SMALL_OUT_BUF    = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 6),     /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_IBUF_SZ      = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 7),     /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_KEY          = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 8),     /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_IV           = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 9),     /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_NONCE_PTR    = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 10),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_NONCE_SZ     = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 11),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_NONCE_VAL    = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 12),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_TAG_SZ       = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 13),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_ODD_TAG_SZ       = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 14),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_AAD_UPDATE   = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 15),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_DATA_UPDATE  = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 16),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_FINAL_CALL   = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 17),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_AUTH_FLD_LEN = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 18),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_AUTH_SIZE    = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 19),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_MSG_SIZE     = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 20),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_MAC          = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 21),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_TAG_PTR      = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 22),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_PADDING_FAILED   = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 23),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_PLD_SIZE     = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 24),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_HDR_SIZE     = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 25),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_VA_PARAM     = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 26),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_DIR          = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 27),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_BAM_PIPE     = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 28),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_FINAL_DATA   = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 29),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_XTS_KEY_PTR  = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 30),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_XTS_KEY_SZ   = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 31),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_XTS_KEY      = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 32),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_XTS_DU_SIZE  = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 33),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_CTR_SIZE     = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 34),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_HASH_ALG     = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 35),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_HASH_ORDER   = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 36),    /**<                */
  QSEE_CRYPTO_ERR_CIPHER_INV_CTR_BUF_SZ   = QSEE_CRYPTO_ERR(QSEE_UCM_CIPHER, 37),    /**<                */

  /** CE Cipher - AES Error Codes */
  QSEE_CRYPTO_ERR_AES_SET_ENC_KEY      = QSEE_CRYPTO_ERR(QSEE_UCM_AES, 1),        /**<                */
  QSEE_CRYPTO_ERR_AES_SET_DEC_KEY      = QSEE_CRYPTO_ERR(QSEE_UCM_AES, 2),        /**<                */
  QSEE_CRYPTO_ERR_AES_ENC              = QSEE_CRYPTO_ERR(QSEE_UCM_AES, 3),        /**<                */
  QSEE_CRYPTO_ERR_AES_DEC              = QSEE_CRYPTO_ERR(QSEE_UCM_AES, 4),        /**<                */

  /** CE Cipher - DES Error Codes */
  QSEE_CRYPTO_ERR_DES_SET_ENC_KEY      = QSEE_CRYPTO_ERR(QSEE_UCM_DES, 1),        /**<                */
  QSEE_CRYPTO_ERR_DES_SET_DEC_KEY      = QSEE_CRYPTO_ERR(QSEE_UCM_DES, 2),        /**<                */
  QSEE_CRYPTO_ERR_DES_ENC              = QSEE_CRYPTO_ERR(QSEE_UCM_DES, 3),        /**<                */
  QSEE_CRYPTO_ERR_DES_DEC              = QSEE_CRYPTO_ERR(QSEE_UCM_DES, 4),        /**<                */

  /** PRNG - Error Codes */
  QSEE_CRYPTO_ERR_PRNG_ACCESS_ENABLE   = QSEE_CRYPTO_ERR(QSEE_UCM_PRNG, 1),       /**<                */
  QSEE_CRYPTO_ERR_PRNG_ACCESS_DISABLE  = QSEE_CRYPTO_ERR(QSEE_UCM_PRNG, 2),       /**<                */

  /** CE Public-Key Error Codes */
  QSEE_CRYPTO_ERR_PKEY_KEY_SIZE_TOO_SMALL       = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 1),   /**< Key size too small to be secure                           */
  QSEE_CRYPTO_ERR_PKEY_KEY_SIZE_TOO_LARGE       = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 2),   /**< Key size exceeds BigInteger size                          */
  QSEE_CRYPTO_ERR_PKEY_MSG_SIZE_TOO_LARGE       = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 3),   /**< Message size too large for padding scheme                 */
  QSEE_CRYPTO_ERR_DIGEST_ALG_UNSUPPORTED        = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 4),   /**< The digest algorithm is not supported                     */
  QSEE_CRYPTO_ERR_MSG_SIZE_DIGEST_MISMATCH      = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 5),   /**< Message size does not match digest size                   */
  QSEE_CRYPTO_ERR_PKEY_INVALID_SIGNATURE        = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 6),   /**< Invalid signature                                         */
  QSEE_CRYPTO_ERR_PKEY_DECRYPT_KEY_SIZE_INVALID = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 7),   /**< Invalid key size for decryption                           */
  QSEE_CRYPTO_ERR_PKEY_PADDED_PACKET_INVALID    = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 8),   /**< Invalid padding detected                                  */
  QSEE_CRYPTO_ERR_PKEY_DECRYPT_BUF_TOO_SMALL    = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 9),   /**< Buffer provided for decrypted message is not large enough */
  QSEE_CRYPTO_ERR_PKEY_PADDING_PRNG_FAILED      = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 10),  /**< Failed to generate required number of bytes for padding   */
  QSEE_CRYPTO_ERR_PKEY_POINT_AT_INFINITY        = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 11),  /**< Point at infinity                                         */
  QSEE_CRYPTO_ERR_PKEY_CONV_TO_BIGVAL_FAILED    = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 12),  /**< Conversion from byte buffer to big-integer failed         */
  QSEE_CRYPTO_ERR_PKEY_CONV_FROM_BIGVAL_FAILED  = QSEE_CRYPTO_ERR(QSEE_UCM_PKEY, 13),  /**< Conversion from big-integer to byte-buffer failed         */

  /** CE BigInteger Error Codes */
  QSEE_CRYPTO_ERR_GENPRIME_BITS_INVALID  = QSEE_CRYPTO_ERR(QSEE_UCM_BIGINT, 1),  /**< Invalid number of bits provided for prime-generation           */
  QSEE_CRYPTO_ERR_GENPRIME_PRNG_FAILED   = QSEE_CRYPTO_ERR(QSEE_UCM_BIGINT, 2),  /**< PRNG failed                                                    */
  QSEE_CRYPTO_ERR_BIGINT_OVERFLOW        = QSEE_CRYPTO_ERR(QSEE_UCM_BIGINT, 3),  /**< Operation would exceed maximum number of bits in BigInteger    */
  QSEE_CRYPTO_ERR_BIGINT_NO_INVERSE      = QSEE_CRYPTO_ERR(QSEE_UCM_BIGINT, 4),  /**< No inverse present                                             */
  QSEE_CRYPTO_ERR_BIGINT_INVALID_LENGTH  = QSEE_CRYPTO_ERR(QSEE_UCM_BIGINT, 5),  /**< Length field of BigInteger exceeds length of BigInteger array  */
  QSEE_CRYPTO_ERR_BIGINT_IS_COMPOSITE    = QSEE_CRYPTO_ERR(QSEE_UCM_BIGINT, 6),  /**< BigInteger is trivially composite                              */

  /** MAC Error Codes */
  QSEE_CRYPTO_ERR_MAC_ALG_NOT_SET         = QSEE_CRYPTO_ERR(QSEE_UCM_MAC, 1),  /**< Underlying Hash/Cipher algorithm was not set */
  QSEE_CRYPTO_ERR_MAC_KEY_NOT_SET         = QSEE_CRYPTO_ERR(QSEE_UCM_MAC, 2),  /**< MAC key was not set */

  /** KDF Error Codes */
  QSEE_CRYPTO_ERR_KDF_MAC_ALG_NOT_SET          = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 1),  /**< pseudorandom number function not set */
  QSEE_CRYPTO_ERR_KDF_MAC_ALG_NOT_MATCH        = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 2),  /**< pseudorandom number function not match */
  QSEE_CRYPTO_ERR_KDF_R_LEN_NOT_SET            = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 3),  /**< r length not set */
  QSEE_CRYPTO_ERR_KDF_CTR_LOC_NOT_SET          = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 4),  /**< counter location not set */
  QSEE_CRYPTO_ERR_KDF_CIPHER_ALG_NOT_SET       = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 5),  /**< cipher algorithm not set */
  QSEE_CRYPTO_ERR_KDF_CIPHER_KEY_LEN_NOT_MATCH = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 6),  /**< cipher key length not match */
  QSEE_CRYPTO_ERR_KDF_HASH_ALG_NOT_SET         = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 7),  /**< hash algorithm not match */
  QSEE_CRYPTO_ERR_KDF_PASSWORD_NOT_SET         = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 8),  /**< pbkdf password not set */
  QSEE_CRYPTO_ERR_KDF_SALT_NOT_SET             = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 9),  /**< pbkdf salt not set */
  QSEE_CRYPTO_ERR_KDF_ITERATION_NOT_SET        = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 10), /**< pbkdf iteration not set */
  QSEE_CRYPTO_ERR_KDF_KO_LEN_NOT_SET           = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 11), /**< pbkdf output key length not set */
  QSEE_CRYPTO_ERR_KDF_KO_LEN_TOO_LONG          = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 12), /**< pbkdf output key length too long */
  QSEE_CRYPTO_ERR_KDF_KEY_NOT_SET              = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 13), /**< kbkdf key not set */
  QSEE_CRYPTO_ERR_KDF_IV_NOT_SET               = QSEE_CRYPTO_ERR(QSEE_UCM_KDF, 14), /**< kbkdf iv not set */
};

/** @} */

#endif
