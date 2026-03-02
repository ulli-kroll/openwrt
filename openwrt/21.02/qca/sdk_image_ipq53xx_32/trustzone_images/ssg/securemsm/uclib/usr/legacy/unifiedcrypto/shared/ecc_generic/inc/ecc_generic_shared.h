#ifndef _GEN_ECC_SHARED_H
#define _GEN_ECC_SHARED_H

/*===========================================================================

                             Unified Crypto

GENERAL DESCRIPTION
Provide ECDSA cryptography. It supports flexible key size with different curves.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012-2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uc_env.h"
#include "qrlbn_ecc_types.h"
#include "qrlbn_ecc.h"
#include "qrlbn_arith.h"
#include "sha_shared.h"

 /**
 * Enum for ECC curve type
 */
typedef enum
{
  SW_ECC_EX_CURVE_P384          = 0x1,
  SW_ECC_EX_CURVE_P521          = 0x2,
  SW_ECC_EX_CURVE_MAX           = 0x7FFFFFFF
} SW_ECC_curveType;

 /**
 * @Initializes a domain from curve hex strings and the cofactor.
 *
 * @param dp             [in]  pointer to pointer to ECC domain context
 * @param curtype        [in]  ECC curve type
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_init_ex(qrlbn_ecc_domain_t **dp, SW_ECC_curveType curType);

 /**
 * @Deinitializes a domain from curve hex strings and the cofactor.
 *
 * @param dp             [in] pointer to pointer to ECC domain context
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_deinit_ex(qrlbn_ecc_domain_t **dp);

 /**
 * @Initializes a domain from curve hex strings and the cofactor.
 *
 * @param dp             [out] pointer to ECC domain context
 * @param modulus        [in]  pointer to modulus
 * @param a              [in]  pointer to a
 * @param b              [in]  pointer to b
 * @param x              [in]  pointer to x
 * @param y              [in]  pointer to y
 * @param n              [in]  pointer to n
 * @param cofactor       [in] cofactor
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_init(qrlbn_ecc_domain_t * dp, char *modulus, char *a,
                    char* b, char *x, char *y, char *n, unsigned cofactor);

/**
 * @brief hashes and signs data with ECC private key
 *        This is the official FIPS certifiable ECDSA signing API.
 *
 * @param hash_alg          [in]  hashing algorithm used to hash the message
 * @param msg               [in]  pointer to msg that needs to be signed
 * @param msg_len           [in]  msg length in Bytes
 * @param privkey           [in]  pointer to private key for signing
 * @param sig               [out] pointer to signed msg, which is signature
 * @param sig_len           [out] signature size in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECDSA_sign_ex(SW_Auth_Alg_Type hash_alg,
                         const uint8_t *msg,
                         uint32_t msg_len,
                         const qrlbn_ecc_bigval_t *privkey,
                         qrlbn_ECDSA_sig_t *  sig,
                         const qrlbn_ecc_domain_t *dp);

/**
 * @brief sign data with ECC private key
 *
 * @param msgdgst           [in]  pointer to msg that needs to be signed
 * @param msgdgst_len       [in]  msg length in Bytes
 * @param privkey           [in]  pointer to private key for signing
 * @param sig               [out] pointer to signed msg, which is signature
 * @param sig_len           [out] signature size in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECDSA_sign(const uint8_t *msgdgst,
                      uint32_t msgdgst_len,
                      const qrlbn_ecc_bigval_t *privkey,
                      qrlbn_ECDSA_sig_t *  sig,
                      const qrlbn_ecc_domain_t *dp);

/**
 * @brief hashes and verifies data with ECC public key
 *        This is the official FIPS certifiable ECDSA signing API.
 *
 * @param hash_alg          [in]  hashing algorithm used to hash the message
 * @param msg               [in]  pointer to msg that needs to be signed
 * @param msg_len           [in]  msg length in Bytes
 * @param pubkey            [in]  pointer to public key for verification
 * @param sig               [out] pointer to signed msg to be verfied
 * @param sig_len           [out] sig_len in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECDSA_verify_ex(SW_Auth_Alg_Type hash_alg,
                           const uint8_t *msg,
                           uint32_t msg_len,
                           const qrlbn_ecc_affine_point_t *pubkey,
                           const qrlbn_ECDSA_sig_t *sig,
                           const qrlbn_ecc_domain_t *dp);

/**
 * @brief verify data with ECC public key
 *
 * @param msgdgst           [in]  pointer to msg that needs to be signed
 * @param msgdgst_len       [in]  msg length in Bytes
 * @param pubkey            [in]  pointer to public key for verification
 * @param sig               [out] pointer to signed msg to be verfied
 * @param sig_len           [out] sig_len in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECDSA_verify(const uint8_t *msgdgst,
                        uint32_t msgdgst_len,
                        const qrlbn_ecc_affine_point_t *pubkey,
                        const qrlbn_ECDSA_sig_t *sig,
                        const qrlbn_ecc_domain_t *dp);

/**
 * @brief generate shared key out of Alice' public key and Bob's private key
 *
 * @param shared_key   [out] shared key bewteen Alice and Bob
 * @param pubkey       [in]  pointer to ECC public key
 * @param privkey      [in]  pointer to ECC private key
 * @param dp           [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */

sw_crypto_errno_enum_type
SW_GENERIC_ECDH_shared_key_derive(qrlbn_ecc_bigval_t *shared_key,
                                  const qrlbn_ecc_bigval_t *privkey,
                                  const qrlbn_ecc_affine_point_t *pubkey,
                                  const qrlbn_ecc_domain_t *dp);

/**
 * @brief Performs a pairwise consistency test to ensure that the
 *        generated keypair can perform signing and verification
 *
 * @param privkey            [in] pointer to private key
 * @param pubkey             [in] pointer to public key
 * @param dp                 [in] ECC domain context
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_pairwise_consistency_test(const qrlbn_ecc_bigval_t *privkey,
                                         const qrlbn_ecc_affine_point_t *pubkey,
                                         const qrlbn_ecc_domain_t *dp);

/**
 * @brief generate public key and private key of ECC
 *
 * @param privkey              [out] pointer to private key
 * @param pubkey               [out] pointer to public key
 * @param dp                   [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_typek
 *
 * @see
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_keypair_generate(qrlbn_ecc_bigval_t *privkey,
                                qrlbn_ecc_affine_point_t  *pubkey,
                                const qrlbn_ecc_domain_t *dp);

/**
 * @brief generate ECC public key for a given private key
 *
 * @param privkey              [in] pointer to private key
 * @param pubkey               [out] pointer to public key
 * @param dp                   [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_typek
 *
 * @see
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_pubkey_generate(const qrlbn_ecc_bigval_t *privkey,
                               qrlbn_ecc_affine_point_t  *pubkey,
                               const qrlbn_ecc_domain_t *dp);

 /**
 * @brief Convert data from Binary to Bigval
 *
 * @param dst              [out] pointer to destination data
 * @param src              [in]  pointer to source data
 * @param srclen           [in]  length of source data in bytes
 * @param dp               [in]  ECC domain context
 * @param tag              [in]  Data type tag
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_binary_to_bigval(qrlbn_ecc_bigval_t *dst,
                                const void *src,
                                size_t srclen,
                                const qrlbn_ecc_domain_t *dp,
                                qrlbn_field_tag_t tag);

/**
 * @brief Convert data from Bigval to Binary
 *
 * @param dst              [out] pointer to destination data
 * @param dstlen           [in]  length of destination data in bytes
 * @param src              [in]  pointer to source data
 * @param dp               [in]  ECC domain context
 * @param tag              [in]  Data type tag
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_bigval_to_binary(uint8_t *dst,
                                size_t dstlen,
                                const qrlbn_ecc_bigval_t *src,
                                const qrlbn_ecc_domain_t *dp,
                                qrlbn_field_tag_t tag);


/**
 * @brief Compare mem. It is not constant time comparison and intended for only testing purpose
 *
 * @param a              [in]  pointer to data
 * @param b              [in]  pointer to data
 * @param mdp            [in]  ECC modulus data
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_compare(const uint32_t *a,
                       const uint32_t *b,
                       const qrlbn_modulus_data_t *mdp);

/*
 * @brief Converts input to bigval.  Input can be network order binary, ascii
 * hex, or ascii decimal, as indicated by radix (256, 16, 10,
 * respectively).  When mode is 256 (binary), srclen indicates the
 * length.  Otherwise, srclen is ignored and the input must be null
 * terminated.  Returns 0 on success, -1 on error.  If the data cannot
 * be represented, an error is indicated.
 *
 * @param dst           [out]  pointer to destination
 * @param src           [in]   pointer to source
 * @param srclen        [in]   length of source in bytes
 * @param base          [in]   base for conversion mode
 *
 * @return sw_crypto_errno_enum_type
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_convert_input_to_bigval(qrlbn_ecc_bigval_t *dst,
                                       const void* src, size_t srclen,
                                       int base);

/*
 * @brief Test affine point on ECC curve.
 * For montgomery encoded affine curves, test if the affine point is
 * on the curve.
 * It is very close to constant time, but we don't care, since
 * for most uses, the point comes from the attacker or is available
 * to the attacker, so he could do the same computation.
 *
 *
 * @param Q           [in]  ECC affine point
 * @param dp          [in]  ECC domain
 *
 * @return B_TRUE if the point is on the curve, B_FALSE otherwise.
 */
boolean_t
SW_GENERIC_ECC_affine_point_on_curve(const qrlbn_ecc_affine_point_t *Q,
                                     const qrlbn_ecc_domain_t *dp);
#endif /* _GEN_ECC_SHARED_H */
