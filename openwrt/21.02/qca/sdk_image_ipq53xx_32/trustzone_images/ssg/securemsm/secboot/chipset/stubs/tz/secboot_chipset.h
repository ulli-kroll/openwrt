#ifndef SECBOOT_CHIPSET_H
#define SECBOOT_CHIPSET_H

/**
@file secboot_chipset.h
@brief Define functions or parameters for particular chipset
*/

/*===========================================================================
   Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/securemsm/secboot/chipset/stubs/tz/secboot_chipset.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/13/17   hw       Init Version 

===========================================================================*/
#include "CeML.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SECBOOT_OTP_ROOT_OF_TRUST_BYTE_SIZE CEML_HASH_DIGEST_SIZE_SHA384
#define SECBOOT_ROOT_OF_TRUST_ALGO          CEML_HASH_ALGO_SHA384

#define BITS_IN_BYTE (8) // the number of bits in one byte
#define WORD_LEN (sizeof(uint32)) // byte size of one word

/*----------------------------------------------------------------------------
 * Preprocessor Definitions for Feature Enablement
 * -------------------------------------------------------------------------*/
#define SECBOOT_SUPPORT_METADATA_FROM_MBN_HEADER (TRUE)
#endif /* SECBOOT_CHIPSET_H */
