#ifndef QSEE_ECC_H
#define QSEE_ECC_H

/**
@file qsee_ecc.h
@brief Provide ECC API wrappers
*/

/*===========================================================================
   Copyright (c) 2012-2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "qsee_rsa.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*
 * Define either ECC_P256 or ECC_P192 to control which polynomial is used.
 * Of course, the binary must have been compiled with the same .h file
 * contents as are used by applications.
 *
 * Arguably, to avoid branching in version control systems, this
 * should be supplied by a -DP256 or -DP192 at compile time. It would be
 * good to include a run-time check to make sure the
 * library was compiled with the same modulus as the include file
 * specifies, but it seems require that the main "functions" be
 * macros, and I'm not sure I want to do that.
 */
#define ECC_P256

/**
@addtogroup qtee_ecc
@{
  Within QTEE ECC API the following return values are defined by
  the Unified Crypto Environment:
  - UC_E_SUCCESS            =  0
  - UC_E_FAILURE            =  1
  - UC_E_NOT_ALLOWED        =  2
  - UC_E_INVALID_ARG        = 16
  - UC_E_OUT_OF_RANGE       = 17
  - UC_E_DATA_INVALID       = 21
*/

#ifdef ECC_P256
/** QSEE_bigval data length for ECC P256. */
#define QSEE_BIGLEN 9
#endif

#ifdef ECC_P192
/** QSEE_bigval data length for ECC P192. */
#define QSEE_BIGLEN 7
#endif
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 - QTEE representation of a large multiprecision
  integer.

  - For ECC P256, QSEE_bigval_t types hold 288-bit 2's complement numbers
  (9 * 32-bit words).

  - For ECC P192 they hold 224-bit 2's complement numbers
  (7 * 32-bit words).

  - Representation is little endian by word and native endian
  within each word.
*/
typedef struct {
  uint32_t data[QSEE_BIGLEN];
  /**< Array of uint32_t representing a large multi-precision integer. */
} QSEE_bigval_t;

/**
  - QTEE representation of an affine point.
  - Used for ECC public keys.
  - For the point at infinity, the boolean value is true and x and y values are
  ignored.
*/
typedef struct {
  QSEE_bigval_t x; /**< Affine point X coordinate. */
  QSEE_bigval_t y; /**< Affine point Y coordinate. */
  bool infinity;   /**< Boolean value representing infinity point. */
} QSEE_affine_point_t;

/**
  QTEE representation for the ECDSA signature.
 */
typedef struct {
  QSEE_bigval_t r;   /**< ECDSA digital signature 'r' value. */
  QSEE_bigval_t s;   /**< ECDSA digital signature 's' value. */
} QSEE_ECDSA_sig_t;

/* Generic ECC parameters type */

#define QSEE_ECC_MAX_MODULUS_BITS (521 + 8)
#define QSEE_ECC_BIGNUM_WORDS ((QSEE_ECC_MAX_MODULUS_BITS + 24 + 31) / 32)

/**
  QTEE representation of Qualcomm Reduced Leakage Big Number (QRLBN) modulus data.
 */
typedef struct {
  unsigned flags;          /**< Flags to control reduction. */
  size_t allocated_words;  /**< Number of words allocated for QRLBN. */
  size_t num_words;        /**< Number of words used by QRLBN. */
  size_t modulus_bits;     /**< Size of modulus in bits. */
  uint32_t *modulus;       /**< Pointer to modulus value. */
  uint32_t *montgomery_R;  /**< Pointer to Montgomery R value. */
  uint32_t *montgomery_R2; /**< Pointer to Montgomery R^2 value. */
  uint32_t m_inv32;
  /**< Helper value used in computing Qi for Montgomery multiplication */
  int quo_num_shift;
  /**< Number of bits to shift so that the binary point is just left of bit 7. */
  uint64_t m_recip;
  /**< Reciprocal value. */
} QSEE_qrlbn_modulus_data_t;

/**
  QTEE representation of QRLBN.
 */
typedef struct {
  uint32_t data[QSEE_ECC_BIGNUM_WORDS];
  /**< Array of uint32_t representing a large multi-precision integer. */
} QSEE_qrlbn_ecc_bigval_t;

/**
  - QTEE representation of an ECC point in Jacobian coordinates with X, Y, and Z
  montgomery encoded.
  - If QRLBN_IS_POINT_AT_INFINITY flag is set, the X, Y,
  and Z values must have values whose absolute value is less than a few times
  the modulus, but the actual values do not otherwise matter.
*/
typedef struct {
  QSEE_qrlbn_ecc_bigval_t X; /**< X coordinate value. */
  QSEE_qrlbn_ecc_bigval_t Y; /**< Y coordinate value. */
  QSEE_qrlbn_ecc_bigval_t Z; /**< Z coordinate value. */
  int flags;                 /**< ECC point flags. */
} QSEE_qrlbn_ecc_point_t;

/**
  QTEE representation of ECC modulus data.
*/
typedef struct {
  QSEE_qrlbn_modulus_data_t md;                  /**< Modulus data. */
  QSEE_qrlbn_ecc_bigval_t modulus_storage;       /**< QRLBN Modulus value storage. */
  QSEE_qrlbn_ecc_bigval_t montgomery_R_storage;  /**< QRLBN Montgomery R value storage. */
  QSEE_qrlbn_ecc_bigval_t montgomery_R2_storage; /**< QRLBN Montgomery R^2 value storage. */
} QSEE_qrlbn_ecc_modulus_data_t;

/**
  - QTEE representation of an ECC affine point.
  - For the point at infinity, the
  QRLBN_POINT_AT_INFINITY flag is set and X and Y values are ignored.
*/
typedef struct {
  QSEE_qrlbn_ecc_bigval_t x; /**< X coordinate value. */
  QSEE_qrlbn_ecc_bigval_t y; /**< Y coordinate value. */
  int flags;                 /**< Affine point flags. */
} QSEE_qrlbn_ecc_affine_point_t;

/**
  QTEE representation of ECC domain parameters.
*/
typedef struct {
  int version;                                    /**< Data structure version number. */
  QSEE_qrlbn_ecc_modulus_data_t modulus;          /**< QRLBN type modulus data. */
  QSEE_qrlbn_ecc_modulus_data_t base_point_order; /**< Order of base point. */
  QSEE_qrlbn_ecc_bigval_t a;                      /**< Elliptic curve parameter a. */
  QSEE_qrlbn_ecc_bigval_t b;                      /**< Elliptic curve parameter b. */
  QSEE_qrlbn_ecc_bigval_t a_mont;                 /**< Montgomery conversion of a. */
  QSEE_qrlbn_ecc_bigval_t b_mont;                 /**< Montgomery conversion of b. */
  QSEE_qrlbn_ecc_affine_point_t affine_base_point;
  /**< Base point on curve. */
  QSEE_qrlbn_ecc_point_t base_point;
  /**< Montgomery conversion of affine_base_point. */
  QSEE_qrlbn_ecc_bigval_t ts_z;
  /**< Montgomery value for Tonelli Shanks square root. */
  uint64_t cofactor;                             /**< Cofactor. */
} QSEE_qrlbn_ecc_domain_t;

/**
  QTEE representation of ECDSA signature in QRLBN values.
*/
typedef struct {
  QSEE_qrlbn_ecc_bigval_t r;  /**< ECDSA digital signature 'r' value. */
  QSEE_qrlbn_ecc_bigval_t s;  /**< ECDSA digital signature 's' value. */
} QSEE_qrlbn_ECDSA_sig_t;

/**
  Tags used for specifying fields in certain operations.
*/
typedef enum {
  QSEE_qrlbn_tag_m,
  /**< ECC domain parameter modulus.modulus_storage. Limited to 32 * mod_words - 24 bits. */
  QSEE_qrlbn_tag_X,
  /**< ECC domain parameter affine_base_point.x. Limited to 32 * mod_words - 24 bits. */
  QSEE_qrlbn_tag_Y,
  /**< ECC domain parameter affine_base_point.y. Limited to 32 * mod_words - 24 bits. */
  QSEE_qrlbn_tag_a,
  /**< ECC domain parameter a. Limited to 32 * mod_words - 24 bits. */
  QSEE_qrlbn_tag_b,
  /**< ECC domain parameter b. Limited to 32 * mod_words - 24 bits. */
  QSEE_qrnbn_tag_n,
  /**< ECC domain parameter base_point_order.modulus_storage. Limited to 32 * mod_words - 24 bits. */
  QSEE_qrlbn_tag_privkey,
  /**< Private key paramater. 32 * base_point_order_num_words - 24 bits */
  QSEE_qrlbn_tag_hash,
  /**< Hash value. No limit */
  QSEE_qrlbn_tag_r,
  /**< ECDSA signature parameter r. Limited to 32 * base_point_order_num_words - 24 bits. */
  QSEE_qrlbn_tag_s,
  /**< ECDSA signature parameter s. Limited to 32 * base_point_order_num_words - 24 bits. */
  QSEE_qrlbn_tag_p,
  /**< ECC point P parameter. */
  QSEE_qrlbn_tag_q,
  /**< ECC point Q parameter. */
} QSEE_qrlbn_field_tag_t;

/**
  ECIES block cipher algorithms.
 */
typedef enum {
  QSEE_ECIES_AES_128 = 0, /**< ECIES AES 128 algorithm. */
  QSEE_ECIES_AES_192 = 1, /**< ECIES AES 192 algorithm. */
  QSEE_ECIES_AES_256 = 2, /**< ECIES AES 256 algorithm. */
} QSEE_ecies_balg_t;

/**
  Possible key (or pair) purposes.
 */
typedef enum {
  QSEE_ECIES_PURPOSE_ENCRYPT = 0,
  /**< For encrypt purposes. Usable with RSA, EC, and AES keys. */
  QSEE_ECIES_PURPOSE_DECRYPT = 1,
  /**< For decrypt purposes. Usable with RSA, EC, and AES keys. */
} QSEE_ecies_purpose_t;

/**
  Supported EC curves; used in ECDSA/ECIES.
 */
typedef enum {
  QSEE_ECIES_CURVE_P_224 = 0, /**< ECC P224 curve. */
  QSEE_ECIES_CURVE_P_256 = 1, /**< ECC P256 curve. */
  QSEE_ECIES_CURVE_P_384 = 2, /**< ECC P384 curve. */
  QSEE_ECIES_CURVE_P_521 = 3, /**< ECC P521 curve. */
} QSEE_ecies_curve_t;

/**
 * Digests provided by keymaster implementations.
 */
typedef enum {
  QSEE_ECIES_DIGEST_NONE = 0,
  /**< Digest optional. May not be implemented in hardware; handled in software, if needed. */
  QSEE_ECIES_DIGEST_MD5 = 1,       /**< MD5 Digest. */
  QSEE_ECIES_DIGEST_SHA1 = 2,      /**< SHA1 Digest. */
  QSEE_ECIES_DIGEST_SHA_2_224 = 3, /**< SHA224 Digest. */
  QSEE_ECIES_DIGEST_SHA_2_256 = 4, /**< SHA256 Digest. */
  QSEE_ECIES_DIGEST_SHA_2_384 = 5, /**< SHA384 Digest. */
  QSEE_ECIES_DIGEST_SHA_2_512 = 6, /**< SHA512 Digest. */
} QSEE_ecies_digest_t;

/**
  Key derivation functions; mostly used in ECIES.
 */
typedef enum {
  QSEE_ECIES_KDF_NONE = 0,
  /** Do not apply a key derivation function; use the raw agreed key. */
  QSEE_ECIES_KDF_RFC5869_SHA256 = 1,
  /**< HKDF defined in RFC 5869 with SHA256. */
  QSEE_ECIES_KDF_ISO18033_2_KDF1_SHA1 = 2,
  /**< KDF1 defined in ISO 18033-2 with SHA1. */
  QSEE_ECIES_KDF_ISO18033_2_KDF1_SHA256 = 3,
  /**< KDF1 defined in ISO 18033-2 with SHA256. */
  QSEE_ECIES_KDF_ISO18033_2_KDF2_SHA1 = 4,
  /**< KDF2 defined in ISO 18033-2 with SHA1. */
  QSEE_ECIES_KDF_ISO18033_2_KDF2_SHA256 = 5,
  /**< KDF2 defined in ISO 18033-2 with SHA256. */
} QSEE_ecies_kdf_t;

/**
  Structure representing an ECIES public or private key value depending on the
  purpose value passed to update.
 */
typedef struct /** @cond */_QSEE_ecies_key /** @endcond */
{
  /**
    Union that represents either public or private ECIES key.
  */
  union {
   QSEE_qrlbn_ecc_affine_point_t * recipient_public_key;
              /**< Public key. */
   QSEE_qrlbn_ecc_bigval_t * private_key;
              /**< Private key. */
  } enc_dec;
} QSEE_ecies_key_t;

/**
  Structure to hold elliptic curve integrated encryption scheme (ECIES) parameter values.
 */
typedef struct  /** @cond */ _QSEE_ecies_params /** @endcond */
{
  QSEE_ecies_curve_t ecies_curve;    /**< ECC curve. */
  QSEE_ecies_kdf_t kdf;              /**< Key derivation function. */
  QSEE_ecies_digest_t digest;        /**< Digest hash function. */
  QSEE_ecies_balg_t balg;            /**< Symmetric algorithm (AES) scheme. */
} QSEE_ecies_params_t;

/**
  Structure representing an ECIES context handle.
 */
typedef struct {
  QSEE_qrlbn_ecc_domain_t domain;    /**< ECC domain. */
  QSEE_ecies_params_t params;        /**< ECIES parameter values. */
} QSEE_ecies_ctx_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
  - Generates public and private keys of ECC.
  - Same keys are used for ECDH and ECDSA.

  @param[out] pubkey        Pointer to ECC public key.
  @param[out] privkey       Pointer to ECC private key.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_ECC_PubPrivate_Key_generate(QSEE_affine_point_t *pubkey,
                                        QSEE_bigval_t *privkey);

/**
  Generates a shared key from Alice's public key and Bob's private key.

  @param[out] shared_key   Pointer to shared key between Alice and Bob.
  @param[in]  pubkey       Pointer to ECC public key.
  @param[in]  privkey      Pointer to ECC private key.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_ECDH_Shared_Key_Derive(QSEE_affine_point_t *shared_key,
                                   QSEE_bigval_t *privkey,
                                   QSEE_affine_point_t *pubkey);

/**
  Signs data with the ECC private key.

  @param[in]  msgdgst   Pointer to message digest.
  @param[in]  privkey   Pointer to private key for signing.
  @param[out] sig       Pointer to message signature.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_ECDSA_Sign(QSEE_bigval_t const *msgdgst,
                       QSEE_bigval_t const *privkey,
                       QSEE_ECDSA_sig_t *sig);

/**
  Verifies data with an ECC public key.

  @param[in] msgdgst    Pointer to message digest.
  @param[in] pubkey     Pointer to public key for signing.
  @param[in] sig        Pointer to message signature.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_ECDSA_Verify(QSEE_bigval_t const *msgdgst,
                         QSEE_affine_point_t const *pubkey,
                         QSEE_ECDSA_sig_t const *sig);

/**
  Converts a hash value to bigval_t.

  @param[in] tgt         Pointer to destination buffer.
  @param[in] hashp       Pointer to hash buffer.
  @param[in] hashlen     Hash buffer size.

  @return
  None.
 */
void qsee_ECC_hash_to_bigval(QSEE_bigval_t *tgt,
                             void const *hashp,
                             unsigned int hashlen);

/** @cond */
/**
  Inject error for testing

  @param[in] buf         Data to inject error.
  @param[in] len         Length of data.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_toggle_random_bit(void *buf,
                           size_t len);
/** @endcond */

/**
  Software-based random bytes generator.

  @param[in,out] buf     Pointer to random bytes buffer.
  @param[in] len         Buffer size.

  @return
  SUCCESS -- 0 on success \n
  FAILURE -- Negative
 */
int qsee_get_random_bytes(void *buf,
                          int len);

/**
  Calculate whether point P lies in the elliptic curve.

  @param[in] P          Pointer to affine point type variable.

  @return
  TRUE -- P is in curve. \n
  FALSE -- P is not in curve.
 */
bool qsee_in_curveP(QSEE_affine_point_t const *P);

/**
  Initializes a domain from curve hex strings and the cofactor.

  @param[out] dp            Pointer to ECC domain context
  @param[in]  modulus       Pointer to modulus.
  @param[in]  a             Pointer to a.
  @param[in]  b             Pointer to b.
  @param[in]  x             Pointer to x.
  @param[in]  y             Pointer to y.
  @param[in]  n             Pointer to n.
  @param[in]  cofactor      Cofactor.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECC_init(QSEE_qrlbn_ecc_domain_t *dp,
                             char *modulus,
                             char *a,
                             char* b,
                             char *x,
                             char *y,
                             char *n,
                             unsigned cofactor);

/**
  - Hashes input data and sign with an ECC private key.
  - Official FIPS certifiable ECDSA signing API.

  @param[in]  hash_alg          Hashing algorithm to sign the message.
  @param[in]  msg               Pointer to message that must be signed.
  @param[in]  msg_len           Message length (in bytes).
  @param[in]  privkey           Pointer to private key for signing.
  @param[out] sig               Pointer to message signature.
  @param[in]  dp                Pointer to ECC domain context.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECDSA_sign_ex(QSEE_HASH_IDX hash_alg,
                                  uint8_t *msg,
                                  uint32_t msg_len,
                                  QSEE_qrlbn_ecc_bigval_t *privkey,
                                  QSEE_qrlbn_ECDSA_sig_t *sig,
                                  QSEE_qrlbn_ecc_domain_t *dp);

/**
  Signs data with an ECC private key.

  @param[in]  msgdgst           Pointer to message that must be signed.
  @param[in]  msgdgst_len       Message length (in bytes).
  @param[in]  privkey           Pointer to private key for signing.
  @param[out] sig               Pointer to message signature.
  @param[in]  dp                Pointer to ECC domain context.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECDSA_sign(uint8_t *msgdgst,
                               uint32_t msgdgst_len,
                               QSEE_qrlbn_ecc_bigval_t *privkey,
                               QSEE_qrlbn_ECDSA_sig_t *sig,
                               QSEE_qrlbn_ecc_domain_t *dp);

/**
  - Hash input data and verify with an ECC private key.
  - Official FIPS certifiable ECDSA verification API.

  @param[in]  hash_alg          Hashing algorithm to sign the message.
  @param[in]  msg               Pointer to message that must be signed.
  @param[in]  msg_len           Message length (in bytes).
  @param[in]  pubkey            Pointer to public key for verification.
  @param[in]  sig               Pointer to signed message to be verfied.
  @param[in]  dp                Pointer to ECC domain context.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECDSA_verify_ex(QSEE_HASH_IDX hash_alg,
                                    uint8_t *msg,
                                    uint32_t msg_len,
                                    QSEE_qrlbn_ecc_affine_point_t *pubkey,
                                    QSEE_qrlbn_ECDSA_sig_t *sig,
                                    QSEE_qrlbn_ecc_domain_t *dp);

/**
  Verifies data with an ECC public key.

  @param[in]  msgdgst           Pointer to message that must be signed.
  @param[in]  msgdgst_len       Message length (in bytes).
  @param[in]  pubkey            Pointer to public key for verification.
  @param[in]  sig               Pointer to signed message to be verfied.
  @param[in]  dp                Pointer to ECC domain context.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECDSA_verify(uint8_t *msgdgst,
                                 uint32_t msgdgst_len,
                                 QSEE_qrlbn_ecc_affine_point_t *pubkey,
                                 QSEE_qrlbn_ECDSA_sig_t *sig,
                                 QSEE_qrlbn_ecc_domain_t *dp);

/**
  Generates a shared key from Alice's public key and Bob's private key.

  @param[out] shared_key   Pointer to shared key between Alice and Bob.
  @param[in]  privkey      Pointer to ECC private key.
  @param[in]  pubkey       Pointer to ECC public key.
  @param[in]  dp           Pointer to ECC domain context.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */

int qsee_SW_GENERIC_ECDH_shared_key_derive(QSEE_qrlbn_ecc_bigval_t *shared_key,
                                           QSEE_qrlbn_ecc_bigval_t *privkey,
                                           QSEE_qrlbn_ecc_affine_point_t *pubkey,
                                           QSEE_qrlbn_ecc_domain_t *dp);

/**
  Generates an ECC public and private key pair.

  @param[out] privkey              Pointer to private key.
  @param[out] pubkey               Pointer to public key.
  @param[in]  dp                   Pointer to ECC domain context.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECC_keypair_generate(QSEE_qrlbn_ecc_bigval_t *privkey,
                                         QSEE_qrlbn_ecc_affine_point_t  *pubkey,
                                         QSEE_qrlbn_ecc_domain_t *dp);

/**
  Generates an ECC public key for a given private key.

  @param[in] privkey               Pointer to private key.
  @param[out] pubkey               Pointer to public key.
  @param[in]  dp                   Pointer to ECC domain context.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECC_pubkey_generate(QSEE_qrlbn_ecc_bigval_t const *privkey,
                                        QSEE_qrlbn_ecc_affine_point_t  *pubkey,
                                        QSEE_qrlbn_ecc_domain_t const *dp);

/**
  Converts data from binary to bigval.

  @param[out] dst                  Pointer to destination data.
  @param[in]  src                  Pointer to source data.
  @param[in]  srclen               Source data length (in bytes).
  @param[in]  dp                   Pointer to ECC domain context.
  @param[in]  tag                  Data type tag.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECC_binary_to_bigval(QSEE_qrlbn_ecc_bigval_t *dst,
                                         const void *src,
                                         size_t srclen,
                                         const QSEE_qrlbn_ecc_domain_t *dp,
                                         QSEE_qrlbn_field_tag_t tag);

/**
  Converts data from bigval to binary.

  @param[out] dst                  Pointer to destination data.
  @param[in]  dstlen               Destination data length (in bytes).
  @param[in]  src                  Pointer to source data.
  @param[in]  dp                   Pointer to ECC domain context.
  @param[in]  tag                  Data type tag.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECC_bigval_to_binary(uint8_t *dst,
                                         size_t dstlen,
                                         const QSEE_qrlbn_ecc_bigval_t *src,
                                         const QSEE_qrlbn_ecc_domain_t *dp,
                                         QSEE_qrlbn_field_tag_t tag);

/** @cond */
/**
  Compare memory. It is not constant time comparison and intended for
  only testing purpose.

  @param[in] a                     Pointer to data.
  @param[in] b                     Pointer to data.
  @param[in] mdp                   Pointer to ECC modulus data.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECC_compare(const uint32_t *a,
                                const uint32_t *b,
                                const QSEE_qrlbn_modulus_data_t *mdp);
/** @endcond */

/**
  - Converts input to bigval.
  - Input can be network order binary, ASCII
  hex, or ASCII decimal, as indicated by radix (256, 16, 10,
  respectively).
  - When mode is 256 (binary), srclen indicates the
  length.  Otherwise, srclen is ignored and the input must be null
  terminated.
  - If data cannot be represented, an error occurs.

  @param[out] dst                  Pointer to destination.
  @param[in]  src                  Pointer to source.
  @param[in]  srclen               Source length (in bytes).
  @param[in]  base                 Base for conversion mode.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECC_convert_input_to_bigval(QSEE_qrlbn_ecc_bigval_t *dst,
                                                const void* src,
                                                size_t srclen,
                                                int base);


/**
  Initializes an ECIES instance.

  @param[out] ctx      Pointer to ECIES context.
  @param[in] params    Pointer to parameters of ECIES context intialization.

  @return
  UC_E_SUCCESS -- Function executes successfully \n
  UC_E_FAILURE -- Operation failed due to unknown error \n
  UC_E_NOT_ALLOWED -- Operation currently not allowed  \n
  UC_E_INVALID_ARG -- Unrecognized argument
 */
int qsee_SW_ECIES_init(QSEE_ecies_ctx_t *ctx,
                       QSEE_ecies_params_t *params);

/**
  Updates an ECIES instance.

  @param[in,out] ctx      Pointer to ECIES context.
  @param[in] key          Pointer of the key for encryption or decryption.
  @param[in] purpose      Defines update encryption or decryption.
  @param[in] msg          Message for encryption or decryption.
  @param[in] msg_len      Message length.
  @param[in,out] ppAD     Pointer to the AD message. In encryption,
                          this parameter does NOT change. In decryption, the *ppAD
                           changes to point to AD data.
  @param[in,out] pAD_len  AD message length.
  @param[out] out         Encryption or decryption output message.
  @param[in,out] out_len  Output message length. The caller needs must ensure
                           enough memory is allocated to contain the output message.

  @return
  UC_E_SUCCESS -- Function executes successfully \n
  UC_E_FAILURE -- Operation failed due to unknown error \n
  UC_E_NOT_ALLOWED -- Operation currently not allowed  \n
  UC_E_INVALID_ARG -- Unrecognized argument \n
  UC_E_OUT_OF_RANGE -- Value out of range \n
  UC_E_DATA_INVALID -- Data is correct but contents are invalid
 */
int qsee_SW_ECIES_update(QSEE_ecies_ctx_t *ctx,
                         QSEE_ecies_key_t *key,
                         QSEE_ecies_purpose_t purpose,
                         const uint8_t *msg,
                         const uint32_t msg_len,
                         uint8_t **ppAD,
                         uint32_t *pAD_len,
                         uint8_t *out,
                         uint32_t *out_len);

/**
  De-initializes and resets content of the ECIES instance.

  @param[in] ctx      Pointer to ECIES context.

  @return
  UC_E_SUCCESS -- Function executes successfully \n
  UC_E_INVALID_ARG -- Unrecognized argument
 */
int qsee_SW_ECIES_finish(QSEE_ecies_ctx_t *ctx);

/**
  Calculates if the ECC affine point is on a curve.

  @param[in] Q                     Pointer to affine point type variable.
  @param[in] dp                    Pointer to the domain curve.

  @return
  TRUE -- Point Q is on curve dp \n
  FALSE -- Point Q is not on curve dp
 */
bool qsee_SW_GENERIC_ECC_affine_point_on_curve(
                                        QSEE_qrlbn_ecc_affine_point_t const *Q,
                                        QSEE_qrlbn_ecc_domain_t *dp);

/** @} */

#endif /*QSEE_ECC_H*/
