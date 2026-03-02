#ifndef __ABTIMEOUT_HWIO_H__
#define __ABTIMEOUT_HWIO_H__
/*
===========================================================================
*/
/**
  @file ABTimeout_HWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    PCNOC_0_BUS_TIMEOUT
    PCNOC_1_BUS_TIMEOUT
    PCNOC_2_BUS_TIMEOUT
    PCNOC_3_BUS_TIMEOUT
    PCNOC_4_BUS_TIMEOUT
    PCNOC_5_BUS_TIMEOUT
    PCNOC_6_BUS_TIMEOUT
    PCNOC_7_BUS_TIMEOUT
    PCNOC_8_BUS_TIMEOUT
    PCNOC_9_BUS_TIMEOUT
    SNOC_0_BUS_TIMEOUT
    SNOC_2_BUS_TIMEOUT
    TCSR_TCSR_REGS
    SNOC_1_BUS_TIMEOUT

  'Include' filters applied: .*TCSR_TIMEOUT.* 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.tzw/1.2/settings/systemdrivers/icb/config/90xx/ABTimeout_HWIO.h#1 $
  $DateTime: 2020/12/21 07:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                        (CORE_TOP_CSR_BASE      + 0x00037000)
#define TCSR_TCSR_REGS_REG_BASE_OFFS                                                                   0x00037000

#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00005000)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_OFFS                                                            (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00005000)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_RMSK                                                                   0x1
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR, HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_RMSK)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR, m)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR,v)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR,m,v,HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_IN)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_TIMEOUT_SLAVE_GLB_EN_BMSK                                              0x1
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_TIMEOUT_SLAVE_GLB_EN_SHFT                                              0x0

#define HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00016020)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_OFFS                                                             (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016020)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RMSK                                                                0x1e3ff
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR, HWIO_TCSR_TIMEOUT_INTR_STATUS_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S3_TIMEOUT_IRQ_BMSK                                            0x10000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S3_TIMEOUT_IRQ_SHFT                                               0x10
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S2_TIMEOUT_IRQ_BMSK                                             0x8000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S2_TIMEOUT_IRQ_SHFT                                                0xf
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S1_TIMEOUT_IRQ_BMSK                                             0x4000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S1_TIMEOUT_IRQ_SHFT                                                0xe
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S0_TIMEOUT_IRQ_BMSK                                             0x2000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S0_TIMEOUT_IRQ_SHFT                                                0xd
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S9_TIMEOUT_IRQ_BMSK                                             0x200
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S9_TIMEOUT_IRQ_SHFT                                               0x9
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S8_TIMEOUT_IRQ_BMSK                                             0x100
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S8_TIMEOUT_IRQ_SHFT                                               0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S7_TIMEOUT_IRQ_BMSK                                              0x80
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S7_TIMEOUT_IRQ_SHFT                                               0x7
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S6_TIMEOUT_IRQ_BMSK                                              0x40
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S6_TIMEOUT_IRQ_SHFT                                               0x6
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S5_TIMEOUT_IRQ_BMSK                                              0x20
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S5_TIMEOUT_IRQ_SHFT                                               0x5
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S4_TIMEOUT_IRQ_BMSK                                              0x10
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S4_TIMEOUT_IRQ_SHFT                                               0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S3_TIMEOUT_IRQ_BMSK                                               0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S3_TIMEOUT_IRQ_SHFT                                               0x3
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S2_TIMEOUT_IRQ_BMSK                                               0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S2_TIMEOUT_IRQ_SHFT                                               0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S1_TIMEOUT_IRQ_BMSK                                               0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S1_TIMEOUT_IRQ_SHFT                                               0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S0_TIMEOUT_IRQ_BMSK                                               0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S0_TIMEOUT_IRQ_SHFT                                               0x0

#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR                                                         (TCSR_TCSR_REGS_REG_BASE      + 0x00016030)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_OFFS                                                         (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016030)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_RMSK                                                            0x1e3ff
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S3_TIMEOUT_IRQ_RPM_ENABLE_BMSK                             0x10000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S3_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x10
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S2_TIMEOUT_IRQ_RPM_ENABLE_BMSK                              0x8000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S2_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                 0xf
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_BMSK                              0x4000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                 0xe
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_BMSK                              0x2000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                 0xd
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S9_TIMEOUT_IRQ_RPM_ENABLE_BMSK                              0x200
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S9_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x9
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S8_TIMEOUT_IRQ_RPM_ENABLE_BMSK                              0x100
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S8_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x8
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S7_TIMEOUT_IRQ_RPM_ENABLE_BMSK                               0x80
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S7_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x7
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S6_TIMEOUT_IRQ_RPM_ENABLE_BMSK                               0x40
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S6_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x6
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S5_TIMEOUT_IRQ_RPM_ENABLE_BMSK                               0x20
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S5_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x5
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S4_TIMEOUT_IRQ_RPM_ENABLE_BMSK                               0x10
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S4_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x4
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S3_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                0x8
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S3_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x3
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S2_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                0x4
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S2_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x2
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                0x2
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x1
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                0x1
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                0x0

#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR                                                        (TCSR_TCSR_REGS_REG_BASE      + 0x00016040)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_OFFS                                                        (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016040)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_RMSK                                                           0x1e3ff
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S3_TIMEOUT_IRQ_APSS_ENABLE_BMSK                           0x10000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S3_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x10
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S2_TIMEOUT_IRQ_APSS_ENABLE_BMSK                            0x8000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S2_TIMEOUT_IRQ_APSS_ENABLE_SHFT                               0xf
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_BMSK                            0x4000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_SHFT                               0xe
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_BMSK                            0x2000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_SHFT                               0xd
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_APSS_ENABLE_BMSK                            0x200
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x9
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_APSS_ENABLE_BMSK                            0x100
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x8
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_APSS_ENABLE_BMSK                             0x80
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x7
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_APSS_ENABLE_BMSK                             0x40
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x6
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_APSS_ENABLE_BMSK                             0x20
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x5
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_APSS_ENABLE_BMSK                             0x10
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x4
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_APSS_ENABLE_BMSK                              0x8
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x3
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_APSS_ENABLE_BMSK                              0x4
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x2
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_BMSK                              0x2
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x1
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_BMSK                              0x1
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_SHFT                              0x0

#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_ADDR                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x00016050)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_OFFS                                                       (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016050)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_RMSK                                                          0x1e3ff
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_SNOC_S3_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                         0x10000
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_SNOC_S3_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x10
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_SNOC_S2_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                          0x8000
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_SNOC_S2_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                             0xf
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_SNOC_S1_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                          0x4000
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_SNOC_S1_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                             0xe
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_SNOC_S0_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                          0x2000
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_SNOC_S0_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                             0xd
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                          0x200
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x9
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                          0x100
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x8
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                           0x80
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x7
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                           0x40
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x6
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                           0x20
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x5
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                           0x10
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x4
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                            0x8
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x3
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                            0x4
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x2
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                            0x2
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x1
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_LPASS_ENABLE_BMSK                            0x1
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_LPASS_ENABLE_SHFT                            0x0

#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR                                                         (TCSR_TCSR_REGS_REG_BASE      + 0x00016060)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_OFFS                                                         (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016060)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_RMSK                                                            0x1e3ff
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S3_TIMEOUT_IRQ_MSS_ENABLE_BMSK                             0x10000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S3_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x10
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S2_TIMEOUT_IRQ_MSS_ENABLE_BMSK                              0x8000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S2_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                 0xf
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_BMSK                              0x4000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                 0xe
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_BMSK                              0x2000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                 0xd
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_MSS_ENABLE_BMSK                              0x200
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x9
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_MSS_ENABLE_BMSK                              0x100
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x8
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_MSS_ENABLE_BMSK                               0x80
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x7
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_MSS_ENABLE_BMSK                               0x40
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x6
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_MSS_ENABLE_BMSK                               0x20
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x5
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_MSS_ENABLE_BMSK                               0x10
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x4
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                0x8
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x3
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                0x4
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x2
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                0x2
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x1
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                0x1
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                0x0

#define HWIO_TCSR_TIMEOUT_INTR_1_STATUS_ADDR                                                           (TCSR_TCSR_REGS_REG_BASE      + 0x00016024)
#define HWIO_TCSR_TIMEOUT_INTR_1_STATUS_OFFS                                                           (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016024)
#define HWIO_TCSR_TIMEOUT_INTR_1_STATUS_RMSK                                                           0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_1_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_1_STATUS_ADDR, HWIO_TCSR_TIMEOUT_INTR_1_STATUS_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_1_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_1_STATUS_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_1_STATUS_WCSS_TIMEOUT_INTR_STATUS_BMSK                                  0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_1_STATUS_WCSS_TIMEOUT_INTR_STATUS_SHFT                                         0x0

#define HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_ADDR                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x00016034)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_OFFS                                                       (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016034)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_RMSK                                                       0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_WCSS_TIMEOUT_INTR_RPM_ENABLE_BMSK                          0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_RPM_1_ENABLE_WCSS_TIMEOUT_INTR_RPM_ENABLE_SHFT                                 0x0

#define HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_ADDR                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00016044)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_OFFS                                                      (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016044)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_RMSK                                                      0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_WCSS_TIMEOUT_INTR_APSS_ENABLE_BMSK                        0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_APSS_1_ENABLE_WCSS_TIMEOUT_INTR_APSS_ENABLE_SHFT                               0x0

#define HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_ADDR                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x00016054)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_OFFS                                                     (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016054)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_RMSK                                                     0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_WCSS_TIMEOUT_INTR_LPASS_ENABLE_BMSK                      0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_1_ENABLE_WCSS_TIMEOUT_INTR_LPASS_ENABLE_SHFT                             0x0

#define HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_ADDR                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x00016064)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_OFFS                                                       (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00016064)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_RMSK                                                       0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_WCSS_TIMEOUT_INTR_MSS_ENABLE_BMSK                          0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_MSS_1_ENABLE_WCSS_TIMEOUT_INTR_MSS_ENABLE_SHFT                                 0x0

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_0_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_0_BUS_TIMEOUT_REG_BASE                          (PCNOC_0_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_0_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_1_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_1_BUS_TIMEOUT_REG_BASE                          (PCNOC_1_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_1_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_2_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_2_BUS_TIMEOUT_REG_BASE                          (PCNOC_2_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_2_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_3_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_3_BUS_TIMEOUT_REG_BASE                          (PCNOC_3_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_3_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_4_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_4_BUS_TIMEOUT_REG_BASE                          (PCNOC_4_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_4_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_5_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_5_BUS_TIMEOUT_REG_BASE                          (PCNOC_5_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_5_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_6_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_6_BUS_TIMEOUT_REG_BASE                          (PCNOC_6_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_6_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_7_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_7_BUS_TIMEOUT_REG_BASE                          (PCNOC_7_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_7_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_8_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_8_BUS_TIMEOUT_REG_BASE                          (PCNOC_8_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_8_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_9_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_9_BUS_TIMEOUT_REG_BASE                          (PCNOC_9_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_9_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: SNOC_0_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define SNOC_0_BUS_TIMEOUT_REG_BASE                          (SNOC_0_BUS_TIMEOUT_BASE      + 0x00000000)
#define SNOC_0_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: SNOC_1_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define SNOC_1_BUS_TIMEOUT_REG_BASE                          (SNOC_1_BUS_TIMEOUT_BASE      + 0x00000000)
#define SNOC_1_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000

/*----------------------------------------------------------------------------
 * MODULE: SNOC_2_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define SNOC_2_BUS_TIMEOUT_REG_BASE                          (SNOC_2_BUS_TIMEOUT_BASE      + 0x00000000)
#define SNOC_2_BUS_TIMEOUT_REG_BASE_OFFS                     0x00000000


#endif /* __ABTIMEOUT_HWIO_H__ */
