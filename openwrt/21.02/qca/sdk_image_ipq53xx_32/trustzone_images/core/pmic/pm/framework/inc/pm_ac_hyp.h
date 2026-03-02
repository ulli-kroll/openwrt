#ifndef __PM_AC_DEF_H__
#define __PM_AC_DEF_H__

/*! \ file pm_ac_def.h
*
 * \brief   <Brief Comment Block/>
 * \details <Detailed Comment Block/>
 * \n &copy; Copyright 2015 - 2017 QUALCOMM Technolgoies Incorporated, All Rights Reserved
 */

/*===================================================================
                    EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tzw/1.2/pmic/pm/framework/inc/pm_ac_hyp.h#1 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
01/13/15   vtw     Created.
====================================================================*/

#include "SpmiCfg.h"
#include "AccessControlHyp.h"

typedef struct {
  ACVirtualMachineId  vm_id;
  ACMapAttributesType ac_attr;
} pm_hyp_map_type;


#endif // PM_AC_DEF_H
