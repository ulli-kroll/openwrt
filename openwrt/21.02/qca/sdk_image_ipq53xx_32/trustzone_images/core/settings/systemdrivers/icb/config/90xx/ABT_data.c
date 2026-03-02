/*==============================================================================

FILE:      ABT_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.tzw/1.2/settings/systemdrivers/icb/config/90xx/ABT_data.c#2 $ 
$DateTime: 2021/04/20 19:15:11 $
$Author: pwbldsvc $
$Change: 30221436 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2021/04/19  sg     [90xx-Emulation] ABT for Alder
2015/09/14  ddk    Port for 8937
2015/05/07  tb     Port for 8952
2014/12/09  tb     Split OEM modifiable data from internal data
2013/11/14  tb     Added support for multiple enable/status registers
2013/10/30  pm     Port to MSM8916 
2013/04/16  pm     Added slot for interrupt priority  
2012/10/04  av     Support for disabling ABT 
2012/05/31  av     Created
 
        Copyright (c) 2012-2018, 2021 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "ABTimeout.h"
#include "ABTimeout_HWIO.h"

/*============================================================================
                      TARGET AND PLATFORM SPECIFIC DATA
============================================================================*/

/* Base address for devices */
#define ABT_PCNOC_0_BASE_ADDR   PCNOC_0_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_1_BASE_ADDR   PCNOC_1_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_2_BASE_ADDR   PCNOC_2_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_3_BASE_ADDR   PCNOC_3_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_4_BASE_ADDR   PCNOC_4_BUS_TIMEOUT_REG_BASE
#define ABT_PCNOC_5_BASE_ADDR   PCNOC_5_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_6_BASE_ADDR   PCNOC_6_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_7_BASE_ADDR   PCNOC_7_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_8_BASE_ADDR   PCNOC_8_BUS_TIMEOUT_REG_BASE 
#define ABT_PCNOC_9_BASE_ADDR   PCNOC_9_BUS_TIMEOUT_REG_BASE 

/* Bit Mask for ABT Slaves */
#define ABT_PCNOC_0_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S0_TIMEOUT_IRQ_SHFT
#define ABT_PCNOC_1_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S1_TIMEOUT_IRQ_SHFT
#define ABT_PCNOC_2_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S2_TIMEOUT_IRQ_SHFT
#define ABT_PCNOC_3_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S3_TIMEOUT_IRQ_SHFT
#define ABT_PCNOC_4_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S4_TIMEOUT_IRQ_SHFT
#define ABT_PCNOC_5_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S5_TIMEOUT_IRQ_SHFT
#define ABT_PCNOC_6_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S6_TIMEOUT_IRQ_SHFT
#define ABT_PCNOC_7_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S7_TIMEOUT_IRQ_SHFT
#define ABT_PCNOC_8_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S8_TIMEOUT_IRQ_SHFT
#define ABT_PCNOC_9_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S9_TIMEOUT_IRQ_SHFT

/* ABT Slave CLK Name */
/* NOTE: Clock names are not required on TZ platform, Clock API
   enables all the required clocks during ABT driver initialization */
#define ABT_PCNOC_0_CLK    NULL //"gcc_pcnoc_bus_timeout0_ahb_clk"
#define ABT_PCNOC_1_CLK    NULL //"gcc_pcnoc_bus_timeout1_ahb_clk"
#define ABT_PCNOC_2_CLK    NULL //"gcc_pcnoc_bus_timeout2_ahb_clk"
#define ABT_PCNOC_3_CLK    NULL //"gcc_pcnoc_bus_timeout3_ahb_clk"
#define ABT_PCNOC_4_CLK    NULL //"gcc_pcnoc_bus_timeout4_ahb_clk"
#define ABT_PCNOC_5_CLK    NULL //"gcc_pcnoc_bus_timeout5_ahb_clk"
#define ABT_PCNOC_6_CLK    NULL //"gcc_pcnoc_bus_timeout6_ahb_clk"
#define ABT_PCNOC_7_CLK    NULL //"gcc_pcnoc_bus_timeout7_ahb_clk"
#define ABT_PCNOC_8_CLK    NULL //"gcc_pcnoc_bus_timeout8_ahb_clk"
#define ABT_PCNOC_9_CLK    NULL //"gcc_pcnoc_bus_timeout9_ahb_clk"

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
  ABT_SLAVE_INFO(PCNOC_0,  TRUE,  FALSE),
  ABT_SLAVE_INFO(PCNOC_1,  TRUE,  FALSE),
  ABT_SLAVE_INFO(PCNOC_2,  TRUE,  FALSE),
  ABT_SLAVE_INFO(PCNOC_3,  TRUE,  FALSE),
  ABT_SLAVE_INFO(PCNOC_4,  TRUE,  FALSE),
  ABT_SLAVE_INFO(PCNOC_5,  TRUE,  FALSE),
  ABT_SLAVE_INFO(PCNOC_6,  TRUE,  FALSE),
  ABT_SLAVE_INFO(PCNOC_7,  TRUE,  FALSE),
  ABT_SLAVE_INFO(PCNOC_8,  TRUE,  FALSE),
  ABT_SLAVE_INFO(PCNOC_9,  TRUE,  FALSE),
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

