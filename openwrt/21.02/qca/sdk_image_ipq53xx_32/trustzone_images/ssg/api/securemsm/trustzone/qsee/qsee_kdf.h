#ifndef _QSEE_KDF_H
#define _QSEE_KDF_H

/**
@file qsee_kdf.h
@brief This is an implementation of the key derivation algorithm.
The counter-based algorithm from NIST SP 800-108 using CMAC where the block
cipher is AES-128.
All sensitive data is zeroized before return.
*/



/*=============================================================================
              Copyright (c) 2000-2012 Qualcomm Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary

=============================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCVPath: $
  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_kdf.h#1 $
  $DateTime: 2020/11/02 23:12:26 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/11   yh      initial version

===========================================================================*/
#include <stdint.h>
/**
  @addtogroup qtee_kdf
  @{
*/

#define QSEE_KDF_SUCCESS   0
#define QSEE_KDF_INVALID  -1
#define QSEE_KDF_FAILURE  -3

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
  KDF key derivation algorithm.

  @caution1hang NULL keys are not supported by QTEE off-target environment.

  @param[in] key           Pointer to key derivation key.
  @param[in] key_len       Key derivation key length in bytes.
  @param[in] label         Pointer to key derivation label.
  @param[in] label_len     Key derivation label length in bytes.
  @param[in] context       Pointer to key derivation context.
  @param[in] context_len   Key derivation context length in byte.
  @param[out] output       Pointer to derived key.
  @param[in] output_len    Derived key length in bytes.

  @detdesc
  The software is a three-level stack:
  - First level: AES
  - Second level: CMAC algorithm from NIST SP 800-38B
  - Third level: Counter-based algorithm from NIST SP 800-108 (named KDF in implementation)
  @par
  The inputs are:
  - Key derivation key (key, key_len)
  - Label (label, label_len)
  - Context (context, context_len)

  The output will be output_len bytes long stored at output. All sensitive data is set
  to zero before return.
  @par
  When a key is set to NULL, key_len will be ignored.
  QTEE sets a 32 bytes key (not accessible from outside Trustzone) as the input key.

  @return
  QSEE_KDF_SUCCESS -- Function executes successfully. \n
  QSEE_KDF_INVALID -- Invalid parameters. \n
  QSEE_KDF_FAILURE -- All other errors during key derivation. \n
*/
int qsee_kdf(const void *key, unsigned int key_len, const void *label, unsigned int label_len,
             const void *context, unsigned int context_len, void* output, unsigned int output_len);

/** @} */

#endif /* _QSEE_KDF_H */
