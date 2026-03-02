#ifndef PBKDF2_H
#define PBKDF2_H
/*===========================================================================

                               pbkdf2.h 

GENERAL DESCRIPTION 
   This file defines PBKDF2 API.         

Copyright (c) 2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/securemsm/uclib/usr/legacy/unifiedcrypto/shared/pbkdf2/inc/pbkdf2.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
07/26/17   shl     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uc_comdef.h"
#include "sha_shared.h"

/**
 * @brief This functions derives key based on password and salt
 *
 * @param hmacAlgo      [in] HMAC algorithm
 * @param password      [in] password pointer
 * @param passwordLen   [in] password length
 * @param salt          [in] salt pointer
 * @param saltLen       [in] salt length
 * @param iterations    [in] iterations 
 * @param derivedKey    [in/out] derivedKey pointer
 * @param derivedKeyLen [in] derivedKey length
 *
 * @return sw_crypto_errno_enum_type 
 *
 */ 
sw_crypto_errno_enum_type pbkdf2
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

#endif /* PBKDF2_H */
