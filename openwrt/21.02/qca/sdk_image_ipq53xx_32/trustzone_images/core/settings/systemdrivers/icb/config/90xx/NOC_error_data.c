/*==============================================================================

FILE:      NOC_error_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.tzw/1.2/settings/systemdrivers/icb/config/90xx/NOC_error_data.c#3 $ 
$DateTime: 2021/05/25 03:19:29 $
$Author: pwbldsvc $
$Change: 30887020 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2021/05/23  sg     [90xx-Emulation] NOC Config Setup
2021/05/05  as     Hwio update for Alder
2019/08/26  as     Removed BIMC registers 
2019/07/16  as     Enabled MEMNOC cfg data
2015/05/07  tb     Port to 8952
2013/10/30  pm     Port to 8916
2012/10/03  av     Created
 
        Copyright (c) 2012-2021 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "NOC_error.h"
#include "NOC_error_HWIO.h"
#include "HALhwio.h"

#define GCC_DDRSS_ATB_CBCR   0x1819004
#define GCC_DDRSS_AHB_CBCR   0x1819008
/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* Internal NOC Configuration Data*/
static NOCERR_info_type NOCERR_cfgdata[] = 
{ 
  [0] = {
    .name            = "PCNOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(PC_NOC_ERL_SWID_LOW),
    .intr_vector     = 226,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_PC_NOC_SBM_SWID_LOW_ADDR },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [1] = {
    .name            = "SNOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(SYSTEM_NOC_ERL_SWID_LOW),
    .intr_vector     = 225,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0 },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_SYSTEM_NOC_SBM_SWID_LOW_ADDR },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [2] = {
    .name            = "MEMNOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(MEM_NOC_MEM_NOC_ERL_SWID_LOW),
    .intr_vector     = 113,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_MEM_NOC_MEM_NOC_MEM_NOC_FAULT_SBM_SWID_LOW_ADDR },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [3] = {
    .name            = "AGGNOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(AGGR_NOC_AGGR_NOC_ERL_SWID_LOW),
    .intr_vector     = 142,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_AGGR_NOC_AGGR_NOC_SBM_SWID_LOW_ADDR },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [4] = {
    .name            = "NSSNOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(NSS_NOC_ERL_SWID_LOW),
    .intr_vector     = 437,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0 },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_NSS_NOC_NSSNOC_SBM_SBM_SWID_LOW_ADDR },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
};

const char *const BIMC_reg_names[] = {
};

const uint64 BIMC_reg_addr[] = {
};

static void *clock_reg_addrs[] =
{
  (uint64 *) GCC_DDRSS_ATB_CBCR, (uint64 *) GCC_DDRSS_AHB_CBCR,
};

NOCERR_config_info_type NOCERR_propdata =
{
  .num_configs = 1,
  .configs = (NOCERR_propdata_type [])
  {
      /* Target info: QCS405 v1.0 and later */
      [0] = 

{
          .family          = CHIPINFO_FAMILY_IPQ90XX,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),
          /* NoC info. */
          .len             = sizeof(NOCERR_cfgdata)/sizeof(NOCERR_info_type),
          .NOCInfo         = NOCERR_cfgdata,
          .BIMC_num_reg    = sizeof(BIMC_reg_addr)/sizeof(BIMC_reg_addr[0]),
          .BIMC_reg_names  = BIMC_reg_names,
          .BIMC_reg_addr   = BIMC_reg_addr,
          .num_clock_regs  = sizeof(clock_reg_addrs)/sizeof(clock_reg_addrs[0]),
          .clock_reg_addrs = clock_reg_addrs,
        },
    },
};
