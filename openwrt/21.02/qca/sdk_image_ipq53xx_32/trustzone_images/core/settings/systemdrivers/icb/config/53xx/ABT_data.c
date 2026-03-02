/*==============================================================================

FILE:      ABT_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/dev/core.tzw/1.2/mpiyaraj.core.tzw.1.2.mimai_core_branching/settings/systemdrivers/icb/config/90xx/ABT_data.c#1 $ 
$DateTime: 2022/05/05 03:56:08 $
$Author: mpiyaraj $
$Change: 36989313 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2022/07/05  sp     [53xx-Emulation] Update ABT for Miami. PCNOC not supported.
2021/04/19  sg     [90xx-Emulation] ABT for Alder
2015/09/14  ddk    Port for 8937
2015/05/07  tb     Port for 8952
2014/12/09  tb     Split OEM modifiable data from internal data
2013/11/14  tb     Added support for multiple enable/status registers
2013/10/30  pm     Port to MSM8916 
2013/04/16  pm     Added slot for interrupt priority  
2012/10/04  av     Support for disabling ABT 
2012/05/31  av     Created
 
        Copyright (c) 2012-2018, 2021, 2022 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "ABTimeout.h"
#include "ABTimeout_HWIO.h"

/*============================================================================
                      TARGET AND PLATFORM SPECIFIC DATA
============================================================================*/

/* Timeout Interrupt Register Address */
#define ABT_TIMEOUT_INTR_KPSS_ENABLE   HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR
#define ABT_TIMEOUT_INTR_STATUS        HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR 
#define ABT_TIMEOUT_SLAVE_GLB_EN       HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR

/* TCSR Summary Interrupt Vectors */
#define ABT_TCSR_KPSS_INTR_VECTOR      229

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* Internal ABT Configuration Data*/
const static ABT_slave_info_type ABT_cfgdata[] = 
{
//ABT_SLAVE_INFO(  name,    local, shared)
};

/* ABT interrupt enable array */
const static void *const intrEnable[] =
{
  (void*)ABT_TIMEOUT_INTR_KPSS_ENABLE,
};

/* ABT interrupt status array */
const static void *const intrStatus[] =
{
  (void*)ABT_TIMEOUT_INTR_STATUS,
};

/* ABT Platform Data type */
const static ABT_platform_info_type ABT_platform_info =
{
    "TZ",                                     /* Image name */
    intrEnable,                               /* INTR Enable array */
    intrStatus,                               /* INTR Status Register array */
    (void*)ABT_TIMEOUT_SLAVE_GLB_EN,          /* ABT Slave global en address */
    ABT_TCSR_KPSS_INTR_VECTOR,                /* KPSS Summary Interrupt Vector */
    NULL,                                     /* No interrupt priority needed on TZ */
    sizeof(intrEnable)/sizeof(intrEnable[0]), /* Number of interrupt enable/status registers */
    (void*)ABT_NO_LCL_GLB_EN,                 /* ABT Slave local global en address */
    ABT_NO_LCL_GLB_EN,                        /* ABT Slave local global bit mask */
};


/* Internal ABT Configuration Property Data*/
const ABT_propdata_type ABT_propdata =  
{
    /* Length of the internal config data array */
    sizeof(ABT_cfgdata)/sizeof(ABT_slave_info_type), 
    /* Pointer to internal config data array */ 
    ABT_cfgdata,
    /* Pointer to platform info data */ 
    &ABT_platform_info                                    
};

