#ifndef UC_IMAGE_ENV_H
#define UC_IMAGE_ENV_H
/*===========================================================================

                             Unified Crypto 

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/ssg.tzw/1.2/securemsm/uclib/usr/legacy/unifiedcrypto/environment/usr/inc/uc_image_env.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
 10/10/12   yk     Initial version
 ===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "qsee_heap.h"
#include "qsee_prng.h"

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/

#define uc_internal_malloc(ptr, ptrLen)                   \
  do {                                                    \
    *ptr = qsee_malloc(ptrLen);                           \
    } while(0)

#define uc_internal_free(ptr)                             \
  do {                                                    \
    qsee_free(ptr);                                       \
  } while(0)

#define uc_internal_prng_getdata(returnvar, ptr, ptrLen)  \
  do {                                                    \
    returnvar = qsee_prng_getdata(ptr, ptrLen);           \
  } while(0)

#endif //UC_IMAGE_ENV_H
