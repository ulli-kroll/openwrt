/*==============================================================================

FILE:      NOC_error_OEM_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.tzw/1.2/settings/systemdrivers/icb/config/90xx/NOC_error_OEM_data.c#2 $ 
$DateTime: 2021/05/25 03:19:29 $
$Author: pwbldsvc $
$Change: 30887020 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2021/05/23  sg     [90xx-Emulation] NOC Config Setup
2019/07/16  as     Enabled MEMNOC IRQ
2019/06/24  as     Disabled error fatal for NOC errors
2015/09/15  ddk    Disabling error fatal for bringup.
2015/05/07  tb     Port to 8952
2014/12/09  tb     Created
 
        Copyright (c) 2014-2020 Qualcomm Technologies Incorporated.
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
  [0] = {
    .name        = "PCNOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .obs_mask    = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .to_reg_vals = NULL,
  },


  [1] = {
    .name        = "SNOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .obs_mask    = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .to_reg_vals = NULL,
  },

 [2] = {
    .name        = "MEMNOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .obs_mask    = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .to_reg_vals = NULL,
  },

 [3] = {
    .name        = "AGGNOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .obs_mask    = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
      },
    .to_reg_vals = NULL,
  },

 [4] = {
    .name        = "NSSNOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1 },
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
          .family          = CHIPINFO_FAMILY_IPQ90XX,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),

          .len            = sizeof(NOCERR_cfgdata_oem)/sizeof(NOCERR_info_type_oem),
          .NOCInfoOEM     = NOCERR_cfgdata_oem,
          .clock_reg_vals = clock_reg_vals,
        },
    },
};
