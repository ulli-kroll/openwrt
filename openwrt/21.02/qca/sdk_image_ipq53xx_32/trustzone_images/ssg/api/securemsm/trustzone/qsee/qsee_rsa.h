#ifndef QSEE_RSA_H
#define QSEE_RSA_H

/*===========================================================================
   Copyright (c) 2012, 2017,2020,2022 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
03/22/22   as      Support for FR69399
08/14/20   as      FR_63264 Changes
11/06/12   ah      Initial version.
05/25/16   yl      Add QSEE_RSA_OAEP_PAD_HASH_IDX and MD5 hash support

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
/**
  @addtogroup qtee_rsa
  @{
*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
typedef uint32_t QSEE_BLONG;
#define QSEE_MAX_KEY_SIZE          4128                                              ///< Maximum key size in bits
#define QSEE_BLONG_SIZE            sizeof(QSEE_BLONG)                                ///< Bytes per digit
#define QSEE_BN_BITS_PER_WORD      (QSEE_BLONG_SIZE * 8)                             ///< Bit per digit
#define QSEE_BLONGS_PER_KEY        (QSEE_MAX_KEY_SIZE + 8*QSEE_BLONG_SIZE - 1)/(8*QSEE_BLONG_SIZE)  ///< Digits per key

#define QSEE_MAX_PUB_EXP_SIZE      5
#define RSA_ENC_DEC_MAX_LENGTH     256

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  QSEE_BigInt type is a QTEE representation of a large multi-precision
  integer.
*/
typedef struct
{
  /** Array of QSEE_BLONG representing a large multi-precision integer.*/
  QSEE_BLONG a[QSEE_BLONGS_PER_KEY];
  /** Length of a in units of QSEE_BLONG_SIZE. */
  int n;
} QSEE_BigInt;

/**
  QSEE_S_BIGINT type is a QTEE representation of a large multi-precision
  signed integer.
*/
typedef struct /** @cond */ QSEE_S_BIGINT_ /** @endcond */
{
  /** QSEE_BigInt type representing a large multi-precision integer. */
  QSEE_BigInt bi;
  /** 0 for positive number representation, 1 for negative. */
  int sign;
} QSEE_S_BIGINT;

/**
  QSEE_RSA_KEY is a QTEE representation of an RSA PKCS key.
*/
typedef struct /** @cond */ QSEE_RSA_KEY /** @endcond */
{
  /**
    Type of key:
    - QSEE_RSA_KEY_PUBLIC for encryption.
    - QSEE_RSA_KEY_PRIVATE for decryption.
    - QSEE_RSA_KEY_PRIVATE_CRT for decryption.
    - QSEE_RSA_KEY_PRIVATE_PUBLIC
    - QSEE_RSA_KEY_PRIVATE_CRT_PUBLIC for key generation.
  */
  int type;
  /** RSA key bit length. */
  int bitLength;
  /** Public exponent */
  QSEE_S_BIGINT *e;
  /** Private exponent. */
  QSEE_S_BIGINT *d;
  /** Modulus. */
  QSEE_S_BIGINT *N;
  /** p factor of N. */
  QSEE_S_BIGINT *p;
  /** q factor of N. */
  QSEE_S_BIGINT *q;
  /** 1/q mod p CRT param. */
  QSEE_S_BIGINT *qP;
  /** d mod (p - 1) CRT param. */
  QSEE_S_BIGINT *dP;
  /** d mod (q - 1) CRT param, */
  QSEE_S_BIGINT *dQ;
} QSEE_RSA_KEY;

/**
  Types used for ce_rsa_encrypt, ce_rsa_decrypt, and other related functions.
*/
typedef enum
{
  /** Public key.  */
  QSEE_RSA_KEY_PUBLIC  = 0,
  /** Private key in non-CRT representation. */
  QSEE_RSA_KEY_PRIVATE = 1,
  /** Private key in CRT representation. */
  QSEE_RSA_KEY_PRIVATE_CRT = 2,
  /** Used only for ce_rsa_key_gen in private/public key pair generation. */
  QSEE_RSA_KEY_PRIVATE_PUBLIC = 3 ,
  /** Private CRT/public key pair. */
  QSEE_RSA_KEY_PRIVATE_CRT_PUBLIC = 4,
  /** Invalid RSA key type. */
  QSEE_RSA_KEY_INVALID = 0x7FFFFFFF,
}QSEE_RSA_KEY_TYPE;

/** QSEE RSA Key Size Macro. */
#define QSEE_RSA_KEY_SIZE(key) ((key)->bi.n * QSEE_BLONG_SIZE)

/**
  Identifies the supported private key algorithms in pkcs8.
*/
typedef enum
{
  /** Private key is type RSA. */
  QSEE_PKCS8_RSA_ENCRYPTION = 0,
  /** Private key is type ECC. */
  QSEE_PKCS8_ECC_ENCRYPTION,
  /** Maximum supported key type value. */
  QSEE_PKCS8_ALGO_MAX,
  /** Invalid PKCS key type. */
  QSEE_PKCS8_INVALID = 0x7FFFFFFF,

} QSEE_pkcs8_algo_type;

/**
  Structure representing an RSA private key (defined in
  RFC2313).
*/
typedef struct
{
  /** Version number used for compatibility with future versions
      of RFC2313. */
  uint8_t   version;
  uint16_t  mod_len;        /**< modulus length (in bytes)                        */
  uint8_t*  mod_data;       /**< modulus:                           n             */
  uint16_t  pub_exp_len;    /**< public exponent length (in bytes)                */
  uint8_t*  pub_exp_data;   /**< public exponent:                   e             */
  uint16_t  priv_exp_len;   /**< private exponent length (in bytes)               */
  uint8_t*  priv_exp_data;  /**< private exponent:                  d             */
  uint16_t  prime1_len;     /**< prime1 length (in bytes)                         */
  uint8_t*  prime1_data;    /**< prime1 is the prime factor p of n: p             */
  uint16_t  prime2_len;     /**< prime2 length (in bytes)                         */
  uint8_t*  prime2_data;    /**< prime2 is the prime factor q of n: q             */
  uint16_t  exp1_len;       /**< exponent1 length (in bytes)                      */
  uint8_t*  exp1_data;      /**< exponent1:                         d mod (p-1)   */
  uint16_t  exp2_len;       /**< exponent2 length (in bytes)                      */
  uint8_t*  exp2_data;      /**< exponent2:                         d mod (q-1)   */
  uint16_t  coef_len ;      /**< coefficient length (in bytes)                    */
  uint8_t*  coef_data;      /**< Chinese remainder
                                 Theorem coefficient:               q**{-1} mod p */

} QSEE_pkcs8_rsa_privkey_type;

/**
  Type representing an ECC private key (defined in RFC5915).
*/
typedef struct
{
  /**
    Specifies syntax version number of elliptic curve
    private key structure.
  */
  uint8_t   version;
  /**
    Specifies private key length in bytes.
  */
  uint16_t  privkey_len;
  /**
    Pointer to private key.
  */
  uint8_t*  privkey_data;
  uint16_t  pubkey_x_len;       /**< Public key x coordinate length (in bytes) */
  uint8_t*  pubkey_x_data;      /**< x coordinate */
  uint16_t  pubkey_y_len;       /**< Public key y coordinate length (in bytes) */
  uint8_t*  pubkey_y_data;      /**< y coordinate */
  /**
    Indicates if key is compressed or uncompressed. Only the
    following are supported:
    - 0x02: compressed
    - 0x03: compressed
    - 0x04: uncompressed
  */
  uint8_t   compressed_pubkey;
  uint8_t   pubkey_y_flag;       /**< RFU */
  /**
    Identifies one of these well known curves:
    - secp192r1
    - secp192r2
    - secp192r3
    - secp239r1
    - secp239r2
    - secp239r3
    - secp256r1
  */
  uint16_t  namedCurve;

} QSEE_pkcs8_ecc_privkey_type;


/**
  DSA private key parameters.
*/
typedef struct
{
  uint16_t  dummy_len;      /**< RFU */
  uint8_t*  dummy;          /**< RFU */

} QSEE_pkcs8_dsa_privkey_type;

/**
  Type representing DH private key parameters.
*/
typedef struct
{
  uint16_t  mod_len;        /**< Modulus length (in bytes).  */
  uint8_t*  mod_data;       /**< Modulus                     */
  uint16_t  exp_len;        /**< Exponent length (in bytes). */
  uint8_t*  exp_data;       /**< Exponent                    */

} QSEE_pkcs8_dh_privkey_type;


/**
  Type representing different private key types to enable generic APIs. Supported key types are:
    - rsa
    - dsa
    - dh
    - ecc
*/
typedef struct
{
  uint8_t                 ver;    /**< Key version */
  QSEE_pkcs8_algo_type    algo;   /**< Algo type */
  /**
    Union of all the supported private key types
  */
  union
  {
    QSEE_pkcs8_rsa_privkey_type  rsa;   /**< RSA key type */
    QSEE_pkcs8_dsa_privkey_type  dsa;   /**< DSA key type */
    QSEE_pkcs8_dh_privkey_type   dh;    /**< DH key type  */
    QSEE_pkcs8_ecc_privkey_type  ecc;   /**< ECC key type */

  } key;

} QSEE_pkcs8_privkey_type;

/**
  - RSA padding type
  - PKCS #1 v2.1 RFC3447
*/
typedef enum
{
  QSEE_RSA_PAD_PKCS1_V1_5_SIG = 1,                /**< PKCS1 v1.5 signature.     */
  QSEE_RSA_PAD_PKCS1_V1_5_ENC = 2,                /**< PKCS1 v1.5 encryption.    */
  QSEE_RSA_PAD_PKCS1_OAEP = 3,                    /**< OAEP encryption.          */
  QSEE_RSA_PAD_PKCS1_PSS = 4,                     /**< PSS signature.            */
  QSEE_RSA_NO_PAD = 5,                            /**< No padding.               */
  QSEE_RSA_PAD_PKCS1_PSS_AUTORECOVER_SALTLEN = 6, /**< PSS with unknown saltlen. */
  QSEE_RSA_PAD_INVALID = 0x7FFFFFFF,              /**< Invalid padding scheme.   */

} QSEE_RSA_PADDING_TYPE;

/**
  - RSA OAEP padding type
  - PKCS #1 v2.1 RFC3447
*/
typedef struct /** @cond */ QSEE_RSA_OAEP_PAD_INFO /** @endcond */
{
   /** Index of Hash and Mask generation function desired. */
   int hashidx;
   /** Label to add to message. */
   unsigned char *label;
   /** Label length. */
   int labellen;
} QSEE_RSA_OAEP_PAD_INFO;

/**
  - RSA PSS padding type
  - PKCS #1 v2.1 RFC3447
*/
typedef struct /** @cond */ QSEE_RSA_PSS_PAD_INFO /** @endcond */
{
   /** Index of Hash and Mask generation function desired. */
   int hashidx;
   /** salt length */
   int saltlen;
} QSEE_RSA_PSS_PAD_INFO;


/**
  Defines the hash function for RSA signature generation.
*/
typedef enum
{
   QSEE_HASH_IDX_NULL = 1,      /**< Initial value. */
   QSEE_HASH_IDX_SHA1,          /**< SHA1.          */
   QSEE_HASH_IDX_SHA256,        /**< SHA256.        */
   QSEE_HASH_IDX_SHA224,        /**< SHA224.        */
   QSEE_HASH_IDX_SHA384,        /**< SHA384.        */
   QSEE_HASH_IDX_SHA512,        /**< SHA512.        */
   QSEE_HASH_IDX_MD5,           /**< MD5.           */
   QSEE_HASH_IDX_MAX,           /**< Enum end limit.*/
   QSEE_HASH_IDX_INVALID = 0x7FFFFFFF,
}QSEE_HASH_IDX;

/**
   Hash algorithm index for RSA OAEP padding.
   The indices below are kept for backward compatbility
   only.

   @note1hang To use SHA1 for Mask Generator Function (MGF)
   hash function correctly, use QSEE_RSA_OAEP_HASH_IDX_XXX_SHA1.

*/
typedef enum
{
   QSEE_RSA_OAEP_HASH_IDX_NULL = 1,   /**< Initial value. */
   QSEE_RSA_OAEP_HASH_IDX_SHA1,       /**< SHA1.          */
   QSEE_RSA_OAEP_HASH_IDX_SHA256,     /**< SHA256.        */
   QSEE_RSA_OAEP_HASH_IDX_SHA224,     /**< SHA224.        */
   QSEE_RSA_OAEP_HASH_IDX_SHA384,     /**< SHA384.        */
   QSEE_RSA_OAEP_HASH_IDX_SHA512,     /**< SHA512.        */
   QSEE_RSA_OAEP_HASH_IDX_MD5,        /**< MD5.           */
   QSEE_RSA_OAEP_HASH_IDX_SHA256_SHA1,/**< SHA256_SHA1.   */
   QSEE_RSA_OAEP_HASH_IDX_SHA224_SHA1,/**< SHA224_SHA1.   */
   QSEE_RSA_OAEP_HASH_IDX_SHA384_SHA1,/**< SHA384_SHA1.   */
   QSEE_RSA_OAEP_HASH_IDX_SHA512_SHA1,/**< SHA512_SHA1.   */
   QSEE_RSA_OAEP_HASH_IDX_MAX,        /**< Enum end limit.*/
   QSEE_RSA_OAEP_HASH_IDX_INVALID = 0x7FFFFFFF,
} QSEE_RSA_OAEP_PAD_HASH_IDX;

typedef struct {
  uint8_t         *msg;
  uint32_t        msg_len;
  uint8_t         *label;
  uint32_t        label_len;
  uint8_t         *emsg;
  uint32_t        emsg_len;
  int           padding_version;
  QSEE_HASH_IDX hashidx;
  uint8_t         *out_msg;
  uint8_t        *out_msg_len;
} rsa_message_vector_type;

typedef struct
{
   uint64_t nbits;   ///< Number of bits in modulus
   uint64_t n;  ///< Modulus
   uint64_t e;
   uint64_t d;
} d_test_rsa_enc_key_t;

typedef struct {
  uint64_t ip_msg;
  uint64_t ip_msg_len;
  uint64_t label;
  uint64_t label_len;
  uint64_t op_msg;
  uint64_t op_msg_len;
  uint64_t pad_type;
  uint64_t hashidx;
} d_rsa_message_vector_type;

typedef struct
{
   uint64_t nbits;   ///< Number of bits in modulus
   uint64_t n;  ///< Modulus
   uint64_t nlen;
   uint64_t e;
   uint64_t elen;
   uint64_t d;
   uint64_t dlen;
} d_test_rsa_key_t;

typedef struct {
  uint64_t msg;
  uint64_t msg_len;
  uint64_t salt;
  uint64_t salt_len;
  uint64_t sig;
  uint64_t sig_len;
  uint64_t pad_ver;
  uint64_t hashidx;
} d_pss_message_vector_type;

struct rsa_sign_test_req{
  uint64_t key_list;
  uint64_t test_vec_list;
  uint64_t operation;
};

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  This function will generate an RSA private/public key pair as per
  FIPS 186-4

  @param[out] key          The public/private RSA key
  @param[in]  keylen       RSA key length (in Bytes)
  @param[in]  pub_exp      Public exponent array
  @param[in]  pub_exp_len  Public exponent array length

  @return
  CE_SUCCESS                  -- Function executes successfully. \n
  -CE_ERROR_FAILURE           -- Generic Error. \n
  -CE_ERROR_NOT_SUPPORTED     -- the feature is not supported. \n
  -CE_ERROR_INVALID_ARG       -- Generic invalid argument. \n
  -CE_ERROR_BUFFER_OVERFLOW   -- Not enough space for output. \n
  -CE_ERROR_NO_MEMORY         -- Out of memory. \n
  -CE_ERROR_INVALID_SIGNATURE -- Invalid signature. \n

*/
int qsee_rsa_key_gen(QSEE_RSA_KEY *key,
                     int keylen,
                     unsigned char *pub_exp,
                     int pub_exp_len);

/**
  Parses a private key in PKCS#8 format. The private key contains multiple
  items pointing to the memory where raw PKCS#8 data is held.

  @param[in] data_ptr      Pointer to raw PKCS#8 data.
  @param[in] data_len      Length of PKCS#8 data.
  @param[out] privkey      Pointer to private key extracted from raw data.

  @return
  E_SUCCESS                -- Private key parsed properly. \n
  -E_DATA_INVALID          -- Key cannot be parsed properly. \n
  -E_NOT_SUPPORTED         -- An algorithm or version is not supported. \n
  -E_INVALID_ARG           -- A pointer argument is NULL. \n
  -E_FAILURE               -- The input data length does not match parsed length. \n

*/
int qsee_secpkcs8_parse(uint8_t* data_ptr,
                        uint16_t data_len,
                        QSEE_pkcs8_privkey_type* privkey);

/**
  This function hashes the input plaintext, performs PKCS #1 padding then signs the signature.
  This is the offical FIPS certifiable RSA signing API.

  @param[in] key           The private RSA key to use
  @param[in] padding_type  Type of padding
  @param[in] padding_info  PSS padding parameters
  @param[in] hashidx       The index of the hash desired
  @param[in] msg           The msg to sign (octets)
  @param[in] msglen        The length of the msg to sign
  @param[out] signature    The signature
  @param[in,out] siglen    The max size and resulting size of
                           the signature

  @return
  CE_SUCCESS               -- Function executes successfully. \n
  -CE_ERROR_FAILURE        -- Generic Error. \n
  -CE_ERROR_NOT_SUPPORTED  -- The feature is not supported. \n
  -CE_ERROR_INVALID_ARG    -- Generic invalid argument. \n
  -CE_ERROR_BUFFER_OVERFLOW -- Not enough space for output. \n
  -CE_ERROR_NO_MEMORY      -- Out of memory. \n
  -CE_ERROR_NOP            -- SW crypto self test failed. \n
*/
int qsee_rsa_sign_ex
(
  QSEE_RSA_KEY              *key,
  QSEE_RSA_PADDING_TYPE     padding_type,
  void                      *padding_info,
  QSEE_HASH_IDX             hashidx,
  const unsigned char       *msg,
  int                       msglen,
  unsigned char             *signature,
  int                       *siglen
);

/**
  Performs PKCS #1 padding, then signs the signature.

  @param[in] key           Private RSA key to use.
  @param[in] padding_type  Padding type.
  @param[in] padding_info  PSS padding parameters.
  @param[in] hashidx       Hash index desired.
  @param[in] hash          Hash to sign (octets).
  @param[in] hashlen       Hash length to sign.
  @param[out] signature    Signature.
  @param[in,out] siglen    Max size and resulting size of signature.

  @return
  CE_SUCCESS               -- Function executes successfully. \n
  -CE_ERROR_FAILURE        -- Generic error. \n
  -CE_ERROR_NOT_SUPPORTED  -- Feature not supported. \n
  -CE_ERROR_INVALID_ARG    -- Generic invalid argument. \n
  -CE_ERROR_BUFFER_OVERFLOW -- Not enough space for output. \n
  -CE_ERROR_NO_MEMORY      -- Out of memory. \n
  -CE_ERROR_NOP            -- Software crypto self test failed. \n
*/
int qsee_rsa_sign_hash
(
  QSEE_RSA_KEY              *key,
  QSEE_RSA_PADDING_TYPE     padding_type,
  void                      *padding_info,
  QSEE_HASH_IDX             hashidx,
  const unsigned char       *hash,
  int                       hashlen,
  unsigned char             *signature,
  int                       *siglen
);

/**
  This function hashes the plaintext, performs PKCS #1 padding and then verifies
  the signature.
  This is the offical FIPS certifiable RSA signing API.

  @param[in] key           The private RSA key to use
  @param[in] padding_type  Type of padding
  @param[in] padding_info  PSS padding parameters
  @param[in] hashidx       The index of the hash desired
  @param[in] msg           The msg to verify (octets)
  @param[in] msglen        The length of the msg to verify
  @param[in] sig           The signature
  @param[in] siglen        The max size and resulting size of
                           the signature

  @return
  CE_SUCCESS               -- Function executes successfully. \n
  -CE_ERROR_FAILURE        -- Generic Error. \n
  -CE_ERROR_NOT_SUPPORTED  -- The feature is not supported. \n
  -CE_ERROR_INVALID_ARG    -- Generic invalid argument. \n
  -CE_ERROR_BUFFER_OVERFLOW -- Not enough space for output. \n
  -CE_ERROR_NO_MEMORY      -- Out of memory. \n
  -CE_ERROR_INVALID_SIGNATURE -- Invalid signature. \n
  -CE_ERROR_NOP            -- SW crypto self test failed. \n

*/
int qsee_rsa_verify_signature_ex
(
  QSEE_RSA_KEY              *key,
  QSEE_RSA_PADDING_TYPE     padding_type,
  void                      *padding_info,
  QSEE_HASH_IDX             hashidx,
  unsigned char             *msg,
  int                       msglen,
  unsigned char             *sig,
  int                       siglen
);


/**
  Performs PKCS #1 padding, then verifies the signature.

  @param[in] key           Private RSA key to use.
  @param[in] padding_type  Padding type.
  @param[in] padding_info  PSS padding parameters.
  @param[in] hashidx       Hash index desired.
  @param[in] hash          Hash to sign (octets).
  @param[in] hashlen       Hash length to sign.
  @param[in] sig           Signature.
  @param[in] siglen        Max size and resulting size of signature.

  @return
  CE_SUCCESS               -- Function executes successfully. \n
  -CE_ERROR_FAILURE        -- Generic Error. \n
  -CE_ERROR_NOT_SUPPORTED  -- Feature not supported. \n
  -CE_ERROR_INVALID_ARG    -- Generic invalid argument. \n
  -CE_ERROR_BUFFER_OVERFLOW -- Not enough space for output. \n
  -CE_ERROR_NO_MEMORY      -- Out of memory. \n
  -CE_ERROR_INVALID_SIGNATURE -- Invalid signature. \n
  -CE_ERROR_NOP            -- Software crypto self test failed. \n

*/
int qsee_rsa_verify_signature
(
  QSEE_RSA_KEY              *key,
  QSEE_RSA_PADDING_TYPE     padding_type,
  void                      *padding_info,
  QSEE_HASH_IDX             hashidx,
  unsigned char             *hash,
  int                       hashlen,
  unsigned char             *sig,
  int                       siglen
);

/**
 * @brief  This function decrypt the signature then v1.5 depad.
 *
 * @param[in] key          The private RSA key to use
 * @param[in] padding_type Type of padding
 * @param[in] padding_info PSS padding parameters
 * @param[in] hashidx      The index of the hash desired
 * @param[in] sig          The signature
 * @param[in] siglen       The max size and resulting size of
 *                         the signature
 * @param[out] msg          Unsigned msg
 * @param[out] msglen       The length of the msg

 *
 * @return 0 on success, negative on failure
*/
int qsee_rsa_decode_signature
(
   QSEE_RSA_KEY              *key,
   QSEE_RSA_PADDING_TYPE     padding_type,
   void                      *padding_info,
   QSEE_HASH_IDX             hashidx,
   unsigned char             *sig,
   int                       siglen,
   unsigned char             *dec_msg,
   int                       *dec_msg_len

);

/**
 Performs PKCS #1 v1.5 padding, followed by encryption.

  @param[in] key           RSA key to encrypt to.
  @param[in] padding_type  Padding type.
  @param[in] padding_info  OAEP padding parameters.
  @param[in] msg           Plaintext.
  @param[in] msglen        Plaintext length (octets).
  @param[out] cipher       Ciphertext.
  @param[in,out] cipherlen Max size and resulting size of ciphertext.

  @return
  CE_SUCCESS               -- Function executes successfully. \n
  -CE_ERROR_NOT_SUPPORTED  -- Feature not supported. \n
  -CE_ERROR_INVALID_PACKET -- Invalid packet. \n
  -CE_ERROR_BUFFER_OVERFLOW -- Not enough space for output. \n
  -CE_ERROR_NOP            -- Software crypto self test failed.

*/
int qsee_rsa_encrypt
(
  QSEE_RSA_KEY            *key,
  QSEE_RSA_PADDING_TYPE   padding_type,
  void                    *padding_info,
  const unsigned char     *msg,
  int                     msglen,
  unsigned char           *cipher,
  int                     *cipherlen
);

/**
  Performs PKCS #1 decryption, followed by v1.5 depad.

  @param[in] key           Corresponding private RSA key.
  @param[in] padding_type  Padding type.
  @param[in] padding_info  OAEP padding parameters.
  @param[in] cipher        Ciphertext.
  @param[in] cipherlen     Ciphertext length (octets).
  @param[out] msg          Plaintext.
  @param[in,out] msglen    Max size and resulting size of plaintext.

  @return
  CE_SUCCESS               -- Function executes successfully.\n
  -CE_ERROR_NOT_SUPPORTED  -- Feature not supported. \n
  -CE_ERROR_INVALID_PACKET -- Invalid packet. \n
  -CE_ERROR_BUFFER_OVERFLOW -- Not enough space for output. \n
  -CE_ERROR_NOP            -- Software crypto self test failed.

*/
int qsee_rsa_decrypt
(
  QSEE_RSA_KEY            *key,
  QSEE_RSA_PADDING_TYPE   padding_type,
  void                    *padding_info,
  unsigned char           *cipher,
  int                     cipherlen,
  unsigned char           *msg,
  int                     *msglen
);

/**
  Reads an unsigned buffer of bytes into a big integer.

  @param[out] a            Pointer to big integer.
  @param[in]  buf          Pointer to array of bytes.
  @param[in]  len          Array length.

  @return
  E_SECMATH_SUCCESS          -- Function executes successfully. \n
  -E_SECMATH_INVALID_ARG     -- Generic invalid argument. \n
  -E_SECMATH_BUFFER_OVERFLOW -- Not enough space for output.

*/
int qsee_BIGINT_read_unsigned_bin
(
  QSEE_BigInt * a,
  const uint8_t * buf,
  uint32_t len
);

/**
  Allocates and initializes S_BIGINT data.

  @param[in]  a            S_BIGINT data.

  @return
  CE_SUCCESS               -- Function executes successfully. \n
  -CE_ERROR_INVALID_ARG    -- Generic invalid argument. \n
  -CE_ERROR_NO_MEMORY      -- Out of memory. \n

*/
int qsee_util_init_s_bigint
(
  QSEE_S_BIGINT **a
);

/**
  Frees S_BIGINT data.

  @param[in]  a            S_BIGINT data.

*/
void qsee_util_free_s_bigint
(
  QSEE_S_BIGINT *a
);

/**
  Reads a zero terminated string into a big integer.

  @param[out] a            Pointer to QSEE_BigInt structure.
  @param[in]  str          Pointer to zero terminated string.
  @param[in]  radix        Radix.

  @return
  E_SECMATH_SUCCESS        -- Function executes successfully. \n
  -E_SECMATH_INVALID_ARG   -- Generic invalid argument. \n
  -E_SECMATH_NOT_SUPPORTED -- Operation not supported.
*/
int qsee_BIGINT_read_radix
(
  QSEE_BigInt * a,
  const char *str,
  uint32_t radix
);

/**
  Computes an RSA modular exponentiation.

  @param[in]     key     RSA key to use.
  @param[in]     in      Input data to send into RSA.
  @param[in]     inlen   Input length (octets).
  @param[out]    out     Destination.
  @param[in,out] outlen  Max size and resulting size of output.
  @param[in]     which   Which exponent to use, e.g., PRIVATE or PUBLIC.

  @return
  CE_SUCCESS -- Function executes successfully. \n
  -CE_ERROR_NOT_SUPPORTED -- Feature not supported. \n
  -CE_ERROR_INVALID_PACKET -- Invalid packet. \n
  -CE_ERROR_BUFFER_OVERFLOW -- Not enough space for output. \n

  @par<text>Dependencies<text>
  None.

*/
int qsee_rsa_exptmod
(
  QSEE_RSA_KEY            *key,
  const unsigned char     *in,
  int                     inlen,
  unsigned char           *out,
  int                     *outlen,
  int                     which
);

/**
  Counts the total number of bytes in a QSEE_S_BIGINT BLONG array.

  @param[in]  s          QSEE_S_BIGINT structure.

  @return
  Total number of bytes.

  @dependencies
  None.
*/
int qsee_util_count_bytes
(
  QSEE_S_BIGINT *s
);

/** @} */

#endif /*QSEE_RSA_H*/

