#ifndef QSEE_UF_PBKDF2_H
#define QSEE_UF_PBKDF2_H

/**
@file qsee_uf_pbkdf2.h
@brief Provide Software PBKDF2 API
*/

/*===========================================================================
   Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_uf_pbkdf2.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
08/05/17   shl      Initial version.

===========================================================================*/
#include <qsee_uf_sha.h>

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_uf_pbkdf2
  @{
*/

/**
 * @brief This functions derives key based on password and salt
 *
 * @param[in] hmacAlgo           HMAC algorithm
 * @param[in] password           password pointer
 * @param[in] passwordLen        password length
 * @param[in] salt               salt pointer
 * @param[in] saltLen            salt length
 * @param[in] iterations         iterations
 * @param[in,out] derivedKey     derivedKey pointer
 * @param[in] derivedKeyLen      derivedKey length
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type qsee_SW_pbkdf2
(
  SW_Auth_Alg_Type   hmacAlgo,
  const uint8_t     *password,
  size_t             passwordLen,
  const uint8_t     *salt,
  size_t             saltLen,
  size_t             iterations,
  uint8_t           *derivedKey,
  size_t             derivedKeyLen
);

/** @} */

#endif /* QSEE_UF_PBKDF2_H */
