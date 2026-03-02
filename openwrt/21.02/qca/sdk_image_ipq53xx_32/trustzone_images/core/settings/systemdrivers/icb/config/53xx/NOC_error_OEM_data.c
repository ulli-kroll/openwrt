/*==============================================================================

FILE:      NOC_error_OEM_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/dev/core.tzw/1.2/mpiyaraj.core.tzw.1.2.mimai_core_branching/settings/systemdrivers/icb/config/90xx/NOC_error_OEM_data.c#1 $ 
$DateTime: 2022/05/05 03:56:08 $
$Author: mpiyaraj $
$Change: 36989313 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
02/10/23    sp     53xx: Update NOC Timeout value to 1.37s.
01/02/23    sp     Increase timeout values to 43 s which is the POR value.
12/22/22    sp     Update PCIE Slaves NOC Timeout value to 86 ms from 43 ms.
09/26/22    sp     Update Miami NOC Timeout Value for all slaves
09/02/22    sp     Update/Enable NOC configs for ipq53xx.
5/17/22     mp           Miami branching
2021/05/23  sg     [90xx-Emulation] NOC Config Setup
2019/07/16  as     Enabled MEMNOC IRQ
2019/06/24  as     Disabled error fatal for NOC errors
2015/09/15  ddk    Disabling error fatal for bringup.
2015/05/07  tb     Port to 8952
2014/12/09  tb     Created
 
        Copyright (c) 2014-2020,2022,2023 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "NOC_error.h"
#include "NOC_error_HWIO.h"

/*============================================================================
                           TARGET SPECIFIC DATA
============================================================================*/

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* OEM NOC Configuration Data*/
NOCERR_info_type_oem NOCERR_cfgdata_oem[] = 
{ 
    /*
     * Note:
     * Previously, we tried to use 43 ms (Ratio = 0x10a) which gave
     * lots of Timeout issues in Heavy traffic scenarios. We also tried
     * 43 s (Ratio = 0x114), during when Secure WDOG got hit
     * before NOC Timeout. 
     *
     * Currently, we have configured to use 1.37 s as NOC Timeout value
     * (Ratio = 0x10f), which we need to stress test, evaluate and update
     * as and when required if in case of any issues (NOC Timeout's).
     *
     * Since > 1 s is a huge time for a single transaction, the
     * current assumption is that this should be sufficient and we
     * should not face any timeout issues and we also won't miss any
     * NOC Timeouts since its lesser than Secure WDOG Timeout.
     */
  [0] = {
    .name        = "PCNOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0xffffffff,
		.faultin_en0_high = 0xffffffff },
      },
    .obs_mask    = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1,
		.faultin_en0_high = 0x0 },
      },
    .to_reg_vals = (uint32_t []){ 0x0,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x1, },
  },


  [1] = {
    .name        = "SNOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x3ffff },
      },
    .obs_mask    = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .to_reg_vals = (uint32_t []){ 0x0,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x10f,
				  0x1, },
  },

 [2] = {
    .name        = "MEMNOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x7f },
      },
    .obs_mask    = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .to_reg_vals = (uint32_t []){ 0x0,
				  0x10f,
				  0x1, },
  },

 [3] = {
    .name        = "NSSNOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0xffff },
      },
    .obs_mask    = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .to_reg_vals = NULL,
  },
};

uint32_t clock_reg_vals[] =
{
  [0] = 0x1,
  [1] = 0x1, 
};

NOCERR_config_info_type_oem NOCERR_propdata_oem =
{
  .num_configs = 1,
  .configs = (NOCERR_propdata_type_oem [])
    {
      /* Target info: SDM670 v1.0 */
      [0] = 
        {
          .family          = CHIPINFO_FAMILY_IPQ53XX,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),

          .len            = sizeof(NOCERR_cfgdata_oem)/sizeof(NOCERR_info_type_oem),
          .NOCInfoOEM     = NOCERR_cfgdata_oem,
          .clock_reg_vals = clock_reg_vals,
        },
    },
};
