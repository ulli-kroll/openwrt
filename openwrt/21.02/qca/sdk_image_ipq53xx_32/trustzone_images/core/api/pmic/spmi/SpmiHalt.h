#ifndef __SPMIHALT_H__
#define	__SPMIHALT_H__

/*! \file 
 *  \n
 *  \brief  SpmiHalt.h 
 *  \details A standalone API for disabling the SPMI bus
 *
 *  \n &copy; Copyright (c) 2004-2017,2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
$DateTime: 2020/11/02 23:11:51 $
$Header: //components/rel/core.tzw/1.2/api/pmic/spmi/SpmiHalt.h#1 $
$Change: 27014929 $ 

                           Edit History

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
 01/20/15           Multiple bus support
 10/1/13            Initial Version

 ===========================================================================*/


#include "SpmiTypes.h"

/**
 * @brief Gracefully halts the SPMI Bus
 * 
 * This function disables all SPMI controllers so no future bus
 * requests are processed.  If this function returns successfully,
 * all buses will have completed any in progress transaction and will be
 * disabled.
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
*/
Spmi_Result SpmiHalt_DisableAllBuses(void);

/**
 * @brief Gracefully halts the SPMI Bus
 * 
 * This function disables the SPMI controller so no future bus
 * requests are processed.  If this function returns successfully,
 * the bus will have completed any in progress transaction and will be
 * disabled.
 * 
 * @param[in] busId  The bus to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
*/
Spmi_Result SpmiHalt_DisableBus(uint8 busId);

#endif
