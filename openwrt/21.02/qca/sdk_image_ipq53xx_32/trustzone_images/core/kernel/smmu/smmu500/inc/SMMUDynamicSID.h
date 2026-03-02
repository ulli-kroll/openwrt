#ifndef SMMUDYNAMICSID_H
#define SMMUDYNAMICSID_H

/*=============================================================================

                       SMMU Dynamic SID Header

FILE:      SMMUDynamicSID.h

GENERAL DESCRIPTION  

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2013 - 2019
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tzw/1.2/kernel/smmu/smmu500/inc/SMMUDynamicSID.h#1 $
$DateTime: 2020/11/02 23:11:51 $
$Author: pwbldsvc $
$Change: 27014929 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/10/19   gp      Initial version; created from ipq807x
=============================================================================*/
#include "SMMUTypes.h"
#include "IxErrno.h"


typedef enum
{
  SMMU_INSTANCE_APPS	   = 0,
  SMMU_INSTANCE_GPU 	   = 1,
  SMMU_INSTANCE_COUNT	   = 2,
  SMMU_INSTANCE_MAX        = 0x7FFFFFFF,
} SMMU_Instance_e_type;


#endif
