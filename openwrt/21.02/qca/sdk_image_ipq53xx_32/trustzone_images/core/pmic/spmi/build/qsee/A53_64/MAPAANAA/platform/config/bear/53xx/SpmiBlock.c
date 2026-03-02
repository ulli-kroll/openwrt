/**
 * @file:  SpmiBlock.c
 * 
 * Copyright (c) 2013-2018 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2022/05/05 03:56:08 $
 * $Header: //components/dev/core.tzw/1.2/mpiyaraj.core.tzw.1.2.mimai_core_branching/pmic/spmi/src/platform/config/bear/90xx/SpmiBlock.c#1 $
 * $Change: 36989313 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "SpmiInfo.h"
#include "SpmiTypes.h"
#include "SpmiHal.h"
#include "SpmiGenericConsts.h"
#include "SpmiUtils.h"
#include "SpmiOsTarget.h"
#include "SpmiBusCfg.h"

#define SPMI_PMIC_ARB_PHYS_ADDRESS 0x02000000
#define SPMI_BARE_OWNER_NUMBER     3

#define SPMI_MAX_CHANNELS_SUPPORTED   (HWIO_PMIC_ARB_REG_CHNLn_MAXn + 1)
#define SPMI_MAX_BUSES_SUPPORTED      (1)
#define SPMI_MAX_OWNER_IDS            (HWIO_PMIC_ARBq_CHNLn_CMD_MAXq + 1)

static uint16 periphMap[SPMI_MAX_PERIPH_ID + 1];
static ChannelMapEntry channelMap[SPMI_MAX_CHANNELS_SUPPORTED];
static SpmiDeviceGenerics generics = INIT_SPMI_DEV_GENERICS;
static uint16 ownerMaskMap[SPMI_MAX_OWNER_IDS] = {
   SPMI_OWNER_APPS,    // ee id 0
   SPMI_OWNER_TZ,      // ee id 1
   SPMI_OWNER_MSS,     // ee id 2
   SPMI_OWNER_LPASS,   // ee id 3
   SPMI_OWNER_AOP,     // ee id 4
   SPMI_OWNER_SSC,     // ee id 5
   SPMI_OWNER_SECPROC, // ee id 6
};

SpmiTargetConfig spmiTgt =
{
    .hwioAddrStr    = "PMIC_ARB",
    .interruptVec   = 0, // not used in tz
};

SpmiDevInfo spmiDevices[] = {
   {
      .baseAddrs          = (uint8*)SPMI_PMIC_ARB_PHYS_ADDRESS,
      .uDevIdx            = 0,
      .reservedChan       = SPMI_MAX_CHANNELS_SUPPORTED - 1,
      .nextChanIdx        = 0,
      .reservedChanEn     = TRUE,   
      .dynamicChannelMode = FALSE,   
      .periphMap          = periphMap,
      .channelMap         = channelMap,
      .ownerMaskMap       = ownerMaskMap,
      .pGenerics          = &generics,
      .target             = (void*) &spmiTgt,    
   },
};

SpmiInfo spmiInfo = {
  .owner              = SPMI_BARE_OWNER_NUMBER,
  .rgOwnerMask        = SPMI_OWNER_TZ|SPMI_OWNER_MSS,
  .uNumDevices        = SPMI_MAX_BUSES_SUPPORTED,
  .devices            = spmiDevices,
};

SpmiInfo * pSpmiBlockInfo = &spmiInfo;

