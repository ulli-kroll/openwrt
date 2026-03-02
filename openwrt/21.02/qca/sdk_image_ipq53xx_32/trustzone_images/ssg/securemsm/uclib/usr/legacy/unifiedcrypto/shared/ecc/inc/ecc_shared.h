#ifndef _ECC_SHARED_H
#define _ECC_SHARED_H

/*===========================================================================

                             Unified Crypto 

GENERAL DESCRIPTION
Provide ECDH and ECDSA cryptography. Currently support only curve P256 which
is 128 bits security level, which is recommended by NIST

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/securemsm/uclib/usr/legacy/unifiedcrypto/shared/ecc/inc/ecc_shared.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/12   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ecc_core.h"
#include "uc_env.h"
#include "ecc_core.h"
#include "uc_comdef.h"


/**
 * @brief generate public key and private key of ECC
 *        same key is used for ECDH and ECDSA
 *
 * @param pubkey       [out] pointer to ECC public key
 * @param privkey      [out] pointer to ECC private key
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type 
SW_ECC_PubPrivate_Key_generate(affine_point_t *pubkey, bigval_t *privkey);

/**
 * @brief generate shared key out of Alice' public key and Bob's private key
 *
 * @param shared_key   [out] shared key bewteen Alice and Bob
 * @param pubkey       [in] pointer to ECC public key
 * @param privkey      [in] pointer to ECC private key
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type 
SW_ECDH_Shared_Key_Derive(affine_point_t *shared_key, bigval_t *privkey,affine_point_t *pubkey);

/**
 * @brief sign data with ECC private key
 *
 * @param msgdgst   [in] msg that needs to be signed
 * @param privkey   [in] pointer to private key to be used for signing
 * @param sig       [out] pointer to the buff that the signed message will be stored
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type
SW_ECDSA_Sign(bigval_t const *msgdgst, bigval_t const *privkey, ECDSA_sig_t *sig);


/**
 * @brief verify data with ECC public key
 *
 * @param msgdgst   [in] msg that needs to be authenticated
 * @param pubvkey   [in] pointer to public key to be used for signing
 * @param sig       [in] pointer to the buff that the signed message will be stored
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type
SW_ECDSA_Verify(bigval_t const *msgdgst, affine_point_t const *pubkey, ECDSA_sig_t const *sig);


#endif /* _ECC_SHARED_H */
