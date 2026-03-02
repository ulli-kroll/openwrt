#ifndef __SECBOOT_OEM_H__
#define __SECBOOT_OEM_H__
/*
============================================================================

                 D A L T L M M   S T A T E   S T R U C T U R E
                             H E A D E R   F I L E

FILE:         DALTLMMDefines.h

DESCRIPTION:
              This file contains definitions for use solely with the
              DALTLMM Properties XML file.  It's use is not recommended by
              any drivers and must not be used in any DALTLMM APIs.

============================================================================

                             Edit History

$Header: //components/rel/ssg.tzw/1.2/securemsm/secboot/auth/src/secboot_oem.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/30/14   dcf     Created the initial revision.

============================================================================
             Copyright (c) 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
============================================================================
*/

#include "DALStdDef.h"
#include "CeML.h"
#include "secboot_env.h"

/*==========================================================================

                     DEFINITIONS FOR THIS MODULE

==========================================================================*/

/*
 * SecbootRootOfTrustType
 *
 * Set of roots-of-trust.
 *
 *  root_of_trust:  hash of the root certificate
 */

typedef struct {
  uint8 root_of_trust[SECBOOT_HASH_DIGEST_BYTE_SIZE];
} SecbootRootOfTrustType;

#endif

