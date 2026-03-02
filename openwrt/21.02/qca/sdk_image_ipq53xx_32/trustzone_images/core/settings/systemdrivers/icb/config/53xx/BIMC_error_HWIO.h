#ifndef __BIMC_ERROR_HWIO_H__
#define __BIMC_ERROR_HWIO_H__
/*
===========================================================================
*/
/**
  @file BIMC_error_HWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    QCS6200 (Vipertooth) [vipertooth_prj_1.0_p2q0r57]
 
  This file contains HWIO register definitions for the following modules:
    BIMC_GLOBAL2
    BIMC_S_DEFAULT_SWAY
    BIMC_S_DDR0_SCMO


  Generation parameters: 
  { u'filename': u'BIMC_error_HWIO.h',
    u'header': u'#include "msmhwiobase.h"',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'BIMC_GLOBAL2', u'BIMC_S_DEFAULT_SWAY', u'BIMC_S_DDR0_SCMO']}
*/
/*
  ===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

  ===========================================================================

  $Header: //components/dev/core.tzw/1.2/mpiyaraj.core.tzw.1.2.mimai_core_branching/settings/systemdrivers/icb/config/90xx/BIMC_error_HWIO.h#1 $
  $DateTime: 2022/05/05 03:56:08 $
  $Author: mpiyaraj $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: BIMC_GLOBAL2
 *--------------------------------------------------------------------------*/

#define BIMC_GLOBAL2_REG_BASE                                              (BIMC_BASE      + 0x00002000)

#define HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_ADDR                         (BIMC_GLOBAL2_REG_BASE      + 0x00000000)
#define HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_RMSK                             0xffff
#define HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_ADDR, HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_RMSK)
#define HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_ADDR, m)
#define HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_SUB_TYPE_BMSK                    0xff00
#define HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_SUB_TYPE_SHFT                       0x8
#define HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_TYPE_BMSK                          0xff
#define HWIO_BIMC_BRIC_GLOBAL2_COMPONENT_INFO_TYPE_SHFT                           0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_0_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x00000100)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_0_RMSK                                 0x7f
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_0_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_0_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_0_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_0_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_0_MPORT_BMSK                           0x7f
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_0_MPORT_SHFT                            0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_0_ADDR                            (BIMC_GLOBAL2_REG_BASE      + 0x00000108)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_0_RMSK                                  0x7f
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_0_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_0_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_0_MPORT_BMSK                            0x7f
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_0_MPORT_SHFT                             0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x0000010c)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_RMSK                                 0x7f
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_ADDR,m,v,HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_IN)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_MPORT_BMSK                           0x7f
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_0_MPORT_SHFT                            0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_1_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x00000110)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_1_RMSK                                  0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_1_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_1_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_1_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_1_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_1_SWAY_0_BMSK                           0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_1_SWAY_0_SHFT                           0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_1_ADDR                            (BIMC_GLOBAL2_REG_BASE      + 0x00000118)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_1_RMSK                                   0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_1_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_1_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_1_SWAY_0_BMSK                            0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_1_SWAY_0_SHFT                            0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x0000011c)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_RMSK                                  0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_ADDR,m,v,HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_IN)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_SWAY_0_BMSK                           0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_1_SWAY_0_SHFT                           0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_2_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x00000120)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_2_RMSK                                  0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_2_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_2_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_2_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_2_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_2_SWAY_1_BMSK                           0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_2_SWAY_1_SHFT                           0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_2_ADDR                            (BIMC_GLOBAL2_REG_BASE      + 0x00000128)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_2_RMSK                                   0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_2_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_2_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_2_SWAY_1_BMSK                            0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_2_SWAY_1_SHFT                            0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x0000012c)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_RMSK                                  0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_ADDR,m,v,HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_IN)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_SWAY_1_BMSK                           0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_2_SWAY_1_SHFT                           0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_3_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x00000130)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_3_RMSK                                  0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_3_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_3_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_3_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_3_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_3_SWAY_2_BMSK                           0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_3_SWAY_2_SHFT                           0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_3_ADDR                            (BIMC_GLOBAL2_REG_BASE      + 0x00000138)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_3_RMSK                                   0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_3_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_3_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_3_SWAY_2_BMSK                            0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_3_SWAY_2_SHFT                            0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x0000013c)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_RMSK                                  0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_ADDR,m,v,HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_IN)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_SWAY_2_BMSK                           0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_3_SWAY_2_SHFT                           0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_4_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x00000140)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_4_RMSK                                  0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_4_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_4_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_4_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_4_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_4_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_4_ARB_BMSK                              0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_STATUS_4_ARB_SHFT                              0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_4_ADDR                            (BIMC_GLOBAL2_REG_BASE      + 0x00000148)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_4_RMSK                                   0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_4_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_4_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_4_ARB_BMSK                               0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_CLEAR_4_ARB_SHFT                               0x0

#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_ADDR                           (BIMC_GLOBAL2_REG_BASE      + 0x0000014c)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_RMSK                                  0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_ADDR, HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_RMSK)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_ADDR, m)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_ADDR,v)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_ADDR,m,v,HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_IN)
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_ARB_BMSK                              0xf
#define HWIO_BIMC_BRIC_INTERRUPT_1_ENABLE_4_ARB_SHFT                              0x0

#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_ADDR                                (BIMC_GLOBAL2_REG_BASE      + 0x000001f0)
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_RMSK                                0x800f000f
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_IN          \
        in_dword_masked(HWIO_BIMC_BRIC_DEFAULT_SEGMENT_ADDR, HWIO_BIMC_BRIC_DEFAULT_SEGMENT_RMSK)
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_INM(m)      \
        in_dword_masked(HWIO_BIMC_BRIC_DEFAULT_SEGMENT_ADDR, m)
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_OUT(v)      \
        out_dword(HWIO_BIMC_BRIC_DEFAULT_SEGMENT_ADDR,v)
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_DEFAULT_SEGMENT_ADDR,m,v,HWIO_BIMC_BRIC_DEFAULT_SEGMENT_IN)
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_REDIRECT_EN_BMSK                    0x80000000
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_REDIRECT_EN_SHFT                          0x1f
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_REDIRECT_BMSK                          0xf0000
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_REDIRECT_SHFT                             0x10
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_DEFAULT_BMSK                               0xf
#define HWIO_BIMC_BRIC_DEFAULT_SEGMENT_DEFAULT_SHFT                               0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000200 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_RMSK                     0xfff0040f
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_BASE_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_BASE_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_BASE_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_BASE_10_SHFT                    0xa
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_RFU_BMSK                        0xc
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_RFU_SHFT                        0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_TYPE_BMSK                       0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_TYPE_SHFT                       0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_ENABLE_BMSK                     0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_LOWER_ENABLE_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000204 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_RMSK                            0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_UNUSED_BMSK                     0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_A_UPPER_UNUSED_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000208 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_RMSK                     0xfff00400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_MASK_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_MASK_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_MASK_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_LOWER_MASK_10_SHFT                    0xa

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x0000020c + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_RMSK                            0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_UNUSED_BMSK                     0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_A_UPPER_UNUSED_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000210 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_RMSK                     0xfff0040f
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_BASE_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_BASE_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_BASE_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_BASE_10_SHFT                    0xa
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_RFU_BMSK                        0xc
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_RFU_SHFT                        0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_TYPE_BMSK                       0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_TYPE_SHFT                       0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_ENABLE_BMSK                     0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_LOWER_ENABLE_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000214 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_RMSK                            0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_UNUSED_BMSK                     0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_B_UPPER_UNUSED_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000218 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_RMSK                     0xfff00400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_MASK_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_MASK_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_MASK_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_LOWER_MASK_10_SHFT                    0xa

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x0000021c + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_RMSK                            0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_UNUSED_BMSK                     0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_B_UPPER_UNUSED_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000220 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RMSK                     0xfff0040f
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_10_SHFT                    0xa
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RFU_BMSK                        0xc
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RFU_SHFT                        0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_TYPE_BMSK                       0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_TYPE_SHFT                       0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ENABLE_BMSK                     0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ENABLE_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000224 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_RMSK                            0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_UNUSED_BMSK                     0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_UPPER_UNUSED_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000228 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_RMSK                     0xfff00400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_10_SHFT                    0xa

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x0000022c + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_RMSK                            0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_MAXn                              3
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_UNUSED_BMSK                     0xf
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_UPPER_UNUSED_SHFT                     0x0

#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_ADDR(n)                      (BIMC_GLOBAL2_REG_BASE      + 0x00000a00 + 0x40 * (n))
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_RMSK                         0xf000040f
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_MAXn                                  3
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_ADDR(n), HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_RMSK)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_INI(n))
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_BASE_31_28_BMSK              0xf0000000
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_BASE_31_28_SHFT                    0x1c
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_BASE_10_BMSK                      0x400
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_BASE_10_SHFT                        0xa
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_RFU_BMSK                            0xe
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_RFU_SHFT                            0x1
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_ENABLE_BMSK                         0x1
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_LOWER_ENABLE_SHFT                         0x0

#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_ADDR(n)                      (BIMC_GLOBAL2_REG_BASE      + 0x00000a04 + 0x40 * (n))
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_RMSK                                0xf
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_MAXn                                  3
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_ADDR(n), HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_RMSK)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_UNUSED_BMSK                         0xf
#define HWIO_BIMC_BRIC_REMAPn_ADDR_BASE_UPPER_UNUSED_SHFT                         0x0

#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_ADDR(n)                      (BIMC_GLOBAL2_REG_BASE      + 0x00000a08 + 0x40 * (n))
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_RMSK                         0xf0000400
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_MAXn                                  3
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_ADDR(n), HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_RMSK)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_INI(n))
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_MASK_31_28_BMSK              0xf0000000
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_MASK_31_28_SHFT                    0x1c
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_MASK_10_BMSK                      0x400
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_LOWER_MASK_10_SHFT                        0xa

#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_ADDR(n)                      (BIMC_GLOBAL2_REG_BASE      + 0x00000a0c + 0x40 * (n))
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_RMSK                                0xf
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_MAXn                                  3
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_ADDR(n), HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_RMSK)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_UNUSED_BMSK                         0xf
#define HWIO_BIMC_BRIC_REMAPn_ADDR_MASK_UPPER_UNUSED_SHFT                         0x0

#define HWIO_BIMC_BRIC_REMAPn_OP0_ADDR(n)                                  (BIMC_GLOBAL2_REG_BASE      + 0x00000a20 + 0x40 * (n))
#define HWIO_BIMC_BRIC_REMAPn_OP0_RMSK                                       0xff0011
#define HWIO_BIMC_BRIC_REMAPn_OP0_MAXn                                              3
#define HWIO_BIMC_BRIC_REMAPn_OP0_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_OP0_ADDR(n), HWIO_BIMC_BRIC_REMAPn_OP0_RMSK)
#define HWIO_BIMC_BRIC_REMAPn_OP0_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_OP0_ADDR(n), mask)
#define HWIO_BIMC_BRIC_REMAPn_OP0_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_REMAPn_OP0_ADDR(n),val)
#define HWIO_BIMC_BRIC_REMAPn_OP0_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_REMAPn_OP0_ADDR(n),mask,val,HWIO_BIMC_BRIC_REMAPn_OP0_INI(n))
#define HWIO_BIMC_BRIC_REMAPn_OP0_OPERAND_BMSK                               0xff0000
#define HWIO_BIMC_BRIC_REMAPn_OP0_OPERAND_SHFT                                   0x10
#define HWIO_BIMC_BRIC_REMAPn_OP0_OPCODE_BMSK                                    0x10
#define HWIO_BIMC_BRIC_REMAPn_OP0_OPCODE_SHFT                                     0x4
#define HWIO_BIMC_BRIC_REMAPn_OP0_OPCODE_ENABLE_BMSK                              0x1
#define HWIO_BIMC_BRIC_REMAPn_OP0_OPCODE_ENABLE_SHFT                              0x0

#define HWIO_BIMC_BRIC_REMAPn_OP1_ADDR(n)                                  (BIMC_GLOBAL2_REG_BASE      + 0x00000a24 + 0x40 * (n))
#define HWIO_BIMC_BRIC_REMAPn_OP1_RMSK                                       0xff0011
#define HWIO_BIMC_BRIC_REMAPn_OP1_MAXn                                              3
#define HWIO_BIMC_BRIC_REMAPn_OP1_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_OP1_ADDR(n), HWIO_BIMC_BRIC_REMAPn_OP1_RMSK)
#define HWIO_BIMC_BRIC_REMAPn_OP1_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_REMAPn_OP1_ADDR(n), mask)
#define HWIO_BIMC_BRIC_REMAPn_OP1_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_REMAPn_OP1_ADDR(n),val)
#define HWIO_BIMC_BRIC_REMAPn_OP1_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_REMAPn_OP1_ADDR(n),mask,val,HWIO_BIMC_BRIC_REMAPn_OP1_INI(n))
#define HWIO_BIMC_BRIC_REMAPn_OP1_OPERAND_BMSK                               0xff0000
#define HWIO_BIMC_BRIC_REMAPn_OP1_OPERAND_SHFT                                   0x10
#define HWIO_BIMC_BRIC_REMAPn_OP1_OPCODE_BMSK                                    0x10
#define HWIO_BIMC_BRIC_REMAPn_OP1_OPCODE_SHFT                                     0x4
#define HWIO_BIMC_BRIC_REMAPn_OP1_OPCODE_ENABLE_BMSK                              0x1
#define HWIO_BIMC_BRIC_REMAPn_OP1_OPCODE_ENABLE_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_DEFAULT_SWAY
 *--------------------------------------------------------------------------*/

#define BIMC_S_DEFAULT_SWAY_REG_BASE                                                   (BIMC_BASE      + 0x00060000)

#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_ADDR                                   (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000000)
#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_RMSK                                     0xffffff
#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_IN          \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_ADDR, HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_RMSK)
#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_ADDR, m)
#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_INSTANCE_BMSK                            0xff0000
#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_INSTANCE_SHFT                                0x10
#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_SUB_TYPE_BMSK                              0xff00
#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_SUB_TYPE_SHFT                                 0x8
#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_TYPE_BMSK                                    0xff
#define HWIO_BIMC_S_DEFAULT_SWAY_COMPONENT_INFO_TYPE_SHFT                                     0x0

#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_0_ADDR                             (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000020)
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_0_RMSK                                   0xff
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_0_ADDR, HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_0_RMSK)
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_0_ADDR, m)
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_0_FUNCTIONALITY_BMSK                     0xff
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_0_FUNCTIONALITY_SHFT                      0x0

#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_1_ADDR                             (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000030)
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_1_RMSK                             0xffffffff
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_1_ADDR, HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_1_RMSK)
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_1_ADDR, m)
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_BMSK          0xffffffff
#define HWIO_BIMC_S_DEFAULT_SWAY_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_SHFT                 0x0

#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_ADDR                                 (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000100)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_RMSK                                        0x3
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_ADDR, HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_RMSK)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_ADDR, m)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_RFU_BMSK                                    0x2
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_RFU_SHFT                                    0x1
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_DECODE_ERROR_BMSK                           0x1
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_DECODE_ERROR_SHFT                           0x0

#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_CLEAR_ADDR                                  (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000108)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_CLEAR_RMSK                                         0x3
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_CLEAR_OUT(v)      \
        out_dword(HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_CLEAR_ADDR,v)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_CLEAR_RFU_BMSK                                     0x2
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_CLEAR_RFU_SHFT                                     0x1
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_CLEAR_DECODE_ERROR_BMSK                            0x1
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_CLEAR_DECODE_ERROR_SHFT                            0x0

#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_ADDR                                 (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x0000010c)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_RMSK                                        0x3
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_ADDR, HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_RMSK)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_ADDR, m)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_ADDR,v)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_ADDR,m,v,HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_IN)
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_RFU_BMSK                                    0x2
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_RFU_SHFT                                    0x1
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_DECODE_ERROR_BMSK                           0x1
#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_DECODE_ERROR_SHFT                           0x0

#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_ADDR                           (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000400)
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_RMSK                           0xffffffff
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_ADDR, HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_RMSK)
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_ADDR, m)
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_ADDR_LOWER_BMSK                0xffffffff
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_ADDR_LOWER_SHFT                       0x0

#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_ADDR                           (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000410)
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_RMSK                           0x8000000f
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_ADDR, HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_RMSK)
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_ADDR, m)
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_WRITE_BMSK                     0x80000000
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_WRITE_SHFT                           0x1f
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_ADDR_UPPER_BMSK                       0xf
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_ADDR_UPPER_SHFT                       0x0

#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_ADDR                           (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000420)
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_RMSK                           0xffffffff
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_ADDR, HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_RMSK)
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_ADDR, m)
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_TID_BMSK                       0xffff0000
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_TID_SHFT                             0x10
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_MID_BMSK                           0xffff
#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_MID_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_DDR0_SCMO
 *--------------------------------------------------------------------------*/

#define BIMC_S_DDR0_SCMO_REG_BASE                                                    (BIMC_BASE      + 0x00048000)

#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000000)
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_RMSK                                      0xffffff
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_ADDR, HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_INSTANCE_BMSK                             0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_INSTANCE_SHFT                                 0x10
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_SUB_TYPE_BMSK                               0xff00
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_SUB_TYPE_SHFT                                  0x8
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_TYPE_BMSK                                     0xff
#define HWIO_BIMC_S_DDR0_SCMO_COMPONENT_INFO_TYPE_SHFT                                      0x0

#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ADDR                                           (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000010)
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_RMSK                                           0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ADDR, HWIO_BIMC_S_DDR0_SCMO_HW_INFO_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_MAJOR_BMSK                                     0xff000000
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_MAJOR_SHFT                                           0x18
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_BRANCH_BMSK                                      0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_BRANCH_SHFT                                          0x10
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_MINOR_BMSK                                         0xff00
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_MINOR_SHFT                                            0x8
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ECO_BMSK                                             0xff
#define HWIO_BIMC_S_DDR0_SCMO_HW_INFO_ECO_SHFT                                              0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000020)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_RMSK                              0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_DATA_WIDTH_BMSK                   0xffff0000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_DATA_WIDTH_SHFT                         0x10
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_TID_WIDTH_BMSK                        0xff00
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_TID_WIDTH_SHFT                           0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_MID_WIDTH_BMSK                          0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_0_MID_WIDTH_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000030)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_RMSK                              0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_BMSK           0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_SHFT                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000040)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_RMSK                                0xff00ff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_NUM_GLOBAL_MONS_BMSK                0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_NUM_GLOBAL_MONS_SHFT                    0x10
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_VMID_WIDTH_BMSK                         0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_2_VMID_WIDTH_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000050)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RMSK                              0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RCH0_CTRL_DEPTH_BMSK              0xff000000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RCH0_CTRL_DEPTH_SHFT                    0x18
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RCH0_DEPTH_BMSK                     0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_RCH0_DEPTH_SHFT                         0x10
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_BCH_DEPTH_BMSK                        0xff00
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_BCH_DEPTH_SHFT                           0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_WCH_DEPTH_BMSK                          0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_3_WCH_DEPTH_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000060)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RMSK                                  0xffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RCH1_CTRL_DEPTH_BMSK                  0xff00
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RCH1_CTRL_DEPTH_SHFT                     0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RCH1_DEPTH_BMSK                         0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_4_RCH1_DEPTH_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000070)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_RMSK                                  0xffff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_DPE_CQ_DEPTH_BMSK                     0xff00
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_DPE_CQ_DEPTH_SHFT                        0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_DDR_BUS_WIDTH_BMSK                      0xff
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_5_DDR_BUS_WIDTH_SHFT                       0x0

#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR                              (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000080)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_RMSK                                  0x1111
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR, HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_WBUFC_PIPE_BMSK                       0x1000
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_WBUFC_PIPE_SHFT                          0xc
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_RDOPT_PIPE_BMSK                        0x100
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_RDOPT_PIPE_SHFT                          0x8
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ACHAN_INTF_PIPE_BMSK                    0x10
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ACHAN_INTF_PIPE_SHFT                     0x4
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR_DECODE_HT_BMSK                      0x1
#define HWIO_BIMC_S_DDR0_SCMO_CONFIGURATION_INFO_6_ADDR_DECODE_HT_SHFT                      0x0

#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ADDR                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000100)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_RMSK                                         0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ERR_OCCURRED_BMSK                            0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ERR_OCCURRED_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_ADDR                                   (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000108)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_RMSK                                          0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_IRQ_CLR_BMSK                                  0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_IRQ_CLR_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x0000010c)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_RMSK                                         0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR, HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_IN)
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_IRQ_EN_BMSK                                  0x1
#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_IRQ_EN_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ADDR                                         (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000120)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_RMSK                                         0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ADDR, HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ERR_ADDR_L_BMSK                              0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ERR_ADDR_L_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000128)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_RMSK                                    0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR, HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_ATID_BMSK                           0xffff0000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_ATID_SHFT                                 0x10
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_AMID_BMSK                               0xffff
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ERR_AMID_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x0000012c)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_RMSK                                     0x10ff117
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ADDR, HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_CODE_BMSK                            0x1000000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_CODE_SHFT                                 0x18
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ALEN_BMSK                              0xf0000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ALEN_SHFT                                 0x10
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ASIZE_BMSK                              0xe000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ASIZE_SHFT                                 0xd
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ABURST_BMSK                             0x1000
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_ABURST_SHFT                                0xc
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AEXCLUSIVE_BMSK                          0x100
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AEXCLUSIVE_SHFT                            0x8
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_APROTNS_BMSK                              0x10
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_APROTNS_SHFT                               0x4
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AOOORD_BMSK                                0x4
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AOOORD_SHFT                                0x2
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AOOOWR_BMSK                                0x2
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AOOOWR_SHFT                                0x1
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AWRITE_BMSK                                0x1
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ERR_AWRITE_SHFT                                0x0

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000130)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_RMSK                                          0x1f
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ADDR, HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ERR_AVMID_BMSK                                0x1f
#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ERR_AVMID_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000200)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RMSK                                        0xfff11111
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR, HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RFU_31_21_BMSK                              0xffe00000
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RFU_31_21_SHFT                                    0x15
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_CFG_WR_CORE_CG_EN_BMSK                        0x100000
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_CFG_WR_CORE_CG_EN_SHFT                            0x14
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_PEN_CMD_CG_EN_BMSK                             0x10000
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_PEN_CMD_CG_EN_SHFT                                0x10
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RCH_CG_EN_BMSK                                  0x1000
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_RCH_CG_EN_SHFT                                     0xc
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_FLUSH_CG_EN_BMSK                                 0x100
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_FLUSH_CG_EN_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_WCH_CG_EN_BMSK                                    0x10
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_WCH_CG_EN_SHFT                                     0x4
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ACH_CG_EN_BMSK                                     0x1
#define HWIO_BIMC_S_DDR0_SCMO_CLOCK_CTRL_ACH_CG_EN_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR                                (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000400)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RMSK                                     0x3ff
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RANK_INTERLEAVE_BMSK                     0x300
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RANK_INTERLEAVE_SHFT                       0x8
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RFU_7_5_BMSK                              0xe0
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RFU_7_5_SHFT                               0x5
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS1_BMSK                       0x10
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS1_SHFT                        0x4
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RFU_3_1_BMSK                               0xe
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_RFU_3_1_SHFT                               0x1
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS0_BMSK                        0x1
#define HWIO_BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS0_SHFT                        0x0

#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n)                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000410 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RMSK                                         0xffff
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_MAXn                                              1
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n), HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_INI(n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_15_10_BMSK                               0xfc00
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_15_10_SHFT                                  0xa
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR_BASE_BMSK                                0x3fc
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_ADDR_BASE_SHFT                                  0x2
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_1_0_BMSK                                    0x3
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_BASE_CSn_RFU_1_0_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n)                                   (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000420 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RMSK                                          0xffff
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_MAXn                                               1
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n), HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_INI(n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RANK_EN_BMSK                                  0x8000
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RANK_EN_SHFT                                     0xf
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_14_13_BMSK                                0x6000
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_14_13_SHFT                                   0xd
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR_MODE_BMSK                                0x1000
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ADDR_MODE_SHFT                                   0xc
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_11_9_BMSK                                  0xe00
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_11_9_SHFT                                    0x9
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_BANK_SIZE_BMSK                                 0x100
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_BANK_SIZE_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_7_6_BMSK                                    0xc0
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_7_6_SHFT                                     0x6
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ROW_SIZE_BMSK                                   0x30
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_ROW_SIZE_SHFT                                    0x4
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_3_2_BMSK                                     0xc
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_RFU_3_2_SHFT                                     0x2
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_COL_SIZE_BMSK                                    0x3
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MAP_CSn_COL_SIZE_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n)                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000430 + 0x4 * (n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RMSK                                         0xffff
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_MAXn                                              1
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_INI(n)        \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n), HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n), mask)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_OUTI(n,val)    \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n),val)
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR(n),mask,val,HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_INI(n))
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_15_10_BMSK                               0xfc00
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_15_10_SHFT                                  0xa
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR_MASK_BMSK                                0x3fc
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_ADDR_MASK_SHFT                                  0x2
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_1_0_BMSK                                    0x3
#define HWIO_BIMC_S_DDR0_SCMO_ADDR_MASK_CSn_RFU_1_0_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000450)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_RMSK                                         0x7ffff31
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_PEN_CLREXMON_EVENT_BMSK                      0x4000000
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_PEN_CLREXMON_EVENT_SHFT                           0x1a
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_CLREXMONACK_BMSK                             0x2000000
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_CLREXMONACK_SHFT                                  0x19
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_CLREXMONREQ_BMSK                             0x1000000
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_CLREXMONREQ_SHFT                                  0x18
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_GLOBAL_MONS_IN_USE_BMSK                       0xffff00
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_GLOBAL_MONS_IN_USE_SHFT                            0x8
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_RANK_IDLE_BMSK                                    0x30
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_RANK_IDLE_SHFT                                     0x4
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_SLAVE_IDLE_BMSK                                    0x1
#define HWIO_BIMC_S_DDR0_SCMO_SLV_STATUS_SLAVE_IDLE_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000460)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_RMSK                                          0x7f
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_EXCMD_ADDR_GRAN_BMSK                          0x70
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_EXCMD_ADDR_GRAN_SHFT                           0x4
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_RFU_3_2_BMSK                                   0xc
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_RFU_3_2_SHFT                                   0x2
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_CLR_EXMON_EN_BMSK                              0x2
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_CLR_EXMON_EN_SHFT                              0x1
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_EXRD_RESP_BMSK                                 0x1
#define HWIO_BIMC_S_DDR0_SCMO_GLOBAL_MON_CFG_EXRD_RESP_SHFT                                 0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR                                       (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000500)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RMSK                                            0xf1f
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RFU_11_10_BMSK                                  0xc00
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RFU_11_10_SHFT                                    0xa
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_CMD_ORDERING_BMSK                               0x300
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_CMD_ORDERING_SHFT                                 0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_HP_CMD_AREQPRIO_MAP_BMSK                         0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_HP_CMD_AREQPRIO_MAP_SHFT                          0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RFU_3_BMSK                                        0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_RFU_3_SHFT                                        0x3
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_HP_CMD_Q_DEPTH_BMSK                               0x7
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_CFG_HP_CMD_Q_DEPTH_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000520)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_RMSK                                          0xff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_HP_CMD_BUF_ENTRIES_IN_USE_BMSK                0xf0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_HP_CMD_BUF_ENTRIES_IN_USE_SHFT                 0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_LP_CMD_BUF_ENTRIES_IN_USE_BMSK                 0xf
#define HWIO_BIMC_S_DDR0_SCMO_CMD_BUF_STATUS_LP_CMD_BUF_ENTRIES_IN_USE_SHFT                 0x0

#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000540)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_RMSK                                        0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR, HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_IN)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_DYNAMIC_RCH_SELECT_BMSK                     0xffff0000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_DYNAMIC_RCH_SELECT_SHFT                           0x10
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_RCH_PORTS_BMSK                                  0xffff
#define HWIO_BIMC_S_DDR0_SCMO_RCH_SELECT_RCH_PORTS_SHFT                                     0x0

#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000544)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_HI_TH_BMSK                 0xff000000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_HI_TH_SHFT                       0x18
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_LO_TH_BMSK                   0xff0000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_LO_TH_SHFT                       0x10
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_HI_TH_BMSK                     0xff00
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_HI_TH_SHFT                        0x8
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_LO_TH_BMSK                       0xff
#define HWIO_BIMC_S_DDR0_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_LO_TH_SHFT                        0x0

#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000560)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RMSK                                           0x33333
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_PRQ_FIFO_FULL_BMSK                             0x20000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_PRQ_FIFO_FULL_SHFT                                0x11
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_PRQ_FIFO_EMPTY_BMSK                            0x10000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_PRQ_FIFO_EMPTY_SHFT                               0x10
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_FULL_BMSK                        0x2000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_FULL_SHFT                           0xd
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_EMPTY_BMSK                       0x1000
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_EMPTY_SHFT                          0xc
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_DATA_FIFO_FULL_BMSK                         0x200
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_DATA_FIFO_FULL_SHFT                           0x9
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_DATA_FIFO_EMPTY_BMSK                        0x100
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH1_DATA_FIFO_EMPTY_SHFT                          0x8
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_FULL_BMSK                          0x20
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_FULL_SHFT                           0x5
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_EMPTY_BMSK                         0x10
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_EMPTY_SHFT                          0x4
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_DATA_FIFO_FULL_BMSK                           0x2
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_DATA_FIFO_FULL_SHFT                           0x1
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_DATA_FIFO_EMPTY_BMSK                          0x1
#define HWIO_BIMC_S_DDR0_SCMO_RCH_STATUS_RCH0_DATA_FIFO_EMPTY_SHFT                          0x0

#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR                                       (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000580)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RMSK                                             0xff
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RFU_7_5_BMSK                                     0xe0
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RFU_7_5_SHFT                                      0x5
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_WRITE_BLOCK_READ_BMSK                            0x10
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_WRITE_BLOCK_READ_SHFT                             0x4
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RFU_3_1_BMSK                                      0xe
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_RFU_3_1_SHFT                                      0x1
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_COALESCE_EN_BMSK                                  0x1
#define HWIO_BIMC_S_DDR0_SCMO_WCH_BUF_CFG_COALESCE_EN_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x000005a0)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_RMSK                                             0x333
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_BRESP_FIFO_FULL_BMSK                             0x200
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_BRESP_FIFO_FULL_SHFT                               0x9
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_BRESP_FIFO_EMPTY_BMSK                            0x100
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_BRESP_FIFO_EMPTY_SHFT                              0x8
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WDATA_FIFO_FULL_BMSK                              0x20
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WDATA_FIFO_FULL_SHFT                               0x5
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WDATA_FIFO_EMPTY_BMSK                             0x10
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WDATA_FIFO_EMPTY_SHFT                              0x4
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WBUF_FULL_BMSK                                     0x2
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WBUF_FULL_SHFT                                     0x1
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WBUF_EMPTY_BMSK                                    0x1
#define HWIO_BIMC_S_DDR0_SCMO_WCH_STATUS_WBUF_EMPTY_SHFT                                    0x0

#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR                                         (BIMC_S_DDR0_SCMO_REG_BASE      + 0x000005c0)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RMSK                                         0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR, HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_IN)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_BL16_ALIGN_FLUSH_EN_BMSK                     0x80000000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_BL16_ALIGN_FLUSH_EN_SHFT                           0x1f
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_30_29_BMSK                               0x60000000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_30_29_SHFT                                     0x1d
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_IN_ORDER_BMSK                          0x10000000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_IN_ORDER_SHFT                                0x1c
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_27_26_BMSK                                0xc000000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_27_26_SHFT                                     0x1a
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_IDLE_DELAY_BMSK                         0x3ff0000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_IDLE_DELAY_SHFT                              0x10
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_15_12_BMSK                                   0xf000
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_15_12_SHFT                                      0xc
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_UPPER_LIMIT_BMSK                            0xf00
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_UPPER_LIMIT_SHFT                              0x8
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_7_4_BMSK                                       0xf0
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_RFU_7_4_SHFT                                        0x4
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_LOWER_LIMIT_BMSK                              0xf
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CFG_FLUSH_LOWER_LIMIT_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR                                         (BIMC_S_DDR0_SCMO_REG_BASE      + 0x000005c4)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_RMSK                                                0xf
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR, HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_IN)
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_RFU_3_2_BMSK                                        0xc
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_RFU_3_2_SHFT                                        0x2
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_FLUSH_ALL_BUF_BMSK                                  0x3
#define HWIO_BIMC_S_DDR0_SCMO_FLUSH_CMD_FLUSH_ALL_BUF_SHFT                                  0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000700)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_DYN_HPCQ_DEPTH_BMSK                   0xe0000000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_DYN_HPCQ_DEPTH_SHFT                         0x1d
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_DYN_CQ_DEPTH_EN_BMSK                  0x10000000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_DYN_CQ_DEPTH_EN_SHFT                        0x1c
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_DYN_CQ_DEPTH_BMSK                      0xf000000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_DYN_CQ_DEPTH_SHFT                           0x18
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_23_21_BMSK                              0xe00000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_23_21_SHFT                                  0x15
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_IGNORE_BANK_UNAVL_BMSK                      0x100000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_IGNORE_BANK_UNAVL_SHFT                          0x14
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_19_18_BMSK                               0xc0000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_19_18_SHFT                                  0x12
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_MASK_CMDOUT_PRI_BMSK                         0x30000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_MASK_CMDOUT_PRI_SHFT                            0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_15_13_BMSK                                0xe000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_15_13_SHFT                                   0xd
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_CMD_REORDERING_BMSK                       0x1000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_DPE_CMD_REORDERING_SHFT                          0xc
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_11_9_BMSK                                  0xe00
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_11_9_SHFT                                    0x9
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_WR_OPT_EN_BMSK                                 0x100
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_WR_OPT_EN_SHFT                                   0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_7_5_BMSK                                    0xe0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_7_5_SHFT                                     0x5
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RD_OPT_EN_BMSK                                  0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RD_OPT_EN_SHFT                                   0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_3_1_BMSK                                     0xe
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_RFU_3_1_SHFT                                     0x1
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_PAGE_MGMT_POLICY_BMSK                            0x1
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG0_PAGE_MGMT_POLICY_SHFT                            0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000704)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HSTP_CMD_TIMEOUT_BYPASS_BMSK              0x80000000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HSTP_CMD_TIMEOUT_BYPASS_SHFT                    0x1f
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_30_29_BMSK                            0x60000000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_30_29_SHFT                                  0x1d
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HSTP_CMD_TIMEOUT_BMSK                     0x1f000000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HSTP_CMD_TIMEOUT_SHFT                           0x18
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_23_21_BMSK                              0xe00000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_23_21_SHFT                                  0x15
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HP_CMD_TIMEOUT_BMSK                         0x1f0000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_HP_CMD_TIMEOUT_SHFT                             0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_15_13_BMSK                                0xe000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_15_13_SHFT                                   0xd
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_MP_CMD_TIMEOUT_BMSK                           0x1f00
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_MP_CMD_TIMEOUT_SHFT                              0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_7_5_BMSK                                    0xe0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_RFU_7_5_SHFT                                     0x5
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_LP_CMD_TIMEOUT_BMSK                             0x1f
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG1_LP_CMD_TIMEOUT_SHFT                              0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000708)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RMSK                                          0xffff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RFU_15_12_BMSK                                0xf000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RFU_15_12_SHFT                                   0xc
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_WR_CMD_TIMEOUT_BMSK                      0xf00
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_WR_CMD_TIMEOUT_SHFT                        0x8
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_RD_CMD_TIMEOUT_BYPASS_BMSK                0x80
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_RD_CMD_TIMEOUT_BYPASS_SHFT                 0x7
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RFU_6_4_BMSK                                    0x70
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RFU_6_4_SHFT                                     0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_RD_CMD_TIMEOUT_BMSK                        0xf
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG2_RWOPT_RD_CMD_TIMEOUT_SHFT                        0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x0000070c)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RMSK                                      0xffffffff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_31_21_BMSK                            0xffe00000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_31_21_SHFT                                  0x15
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_WROPT_CMD_TIMEOUT_BMSK                      0x1f0000
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_WROPT_CMD_TIMEOUT_SHFT                          0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_15_5_BMSK                                 0xffe0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_RFU_15_5_SHFT                                    0x5
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_FLUSH_CMD_TIMEOUT_BMSK                          0x1f
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG3_FLUSH_CMD_TIMEOUT_SHFT                           0x0

#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR                                      (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000710)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RMSK                                            0xff
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR, HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_IN)
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RFU_7_5_BMSK                                    0xe0
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RFU_7_5_SHFT                                     0x5
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_MASK_WRQ_FULL_BMSK                              0x10
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_MASK_WRQ_FULL_SHFT                               0x4
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RFU_3_1_BMSK                                     0xe
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_RFU_3_1_SHFT                                     0x1
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_MASK_RDQ_FULL_BMSK                               0x1
#define HWIO_BIMC_S_DDR0_SCMO_CMD_OPT_CFG4_MASK_RDQ_FULL_SHFT                               0x0

#define HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_ADDR                                     (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000720)
#define HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_RMSK                                           0xff
#define HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_DPE_HP_CQ_ENTRIES_IN_USE_BMSK                  0xf0
#define HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_DPE_HP_CQ_ENTRIES_IN_USE_SHFT                   0x4
#define HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_DPE_LP_CQ_ENTRIES_IN_USE_BMSK                   0xf
#define HWIO_BIMC_S_DDR0_SCMO_DPE_CQ_STATUS_DPE_LP_CQ_ENTRIES_IN_USE_SHFT                   0x0

#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_ADDR                                        (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000a40)
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_RMSK                                               0x7
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_IN          \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_ADDR, HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_RMSK)
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_INM(m)      \
        in_dword_masked(HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_ADDR, m)
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_OUT(v)      \
        out_dword(HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_ADDR,v)
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_ADDR,m,v,HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_IN)
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_FREQ_SET_POINT_REG_IN_USE_BMSK                     0x7
#define HWIO_BIMC_S_DDR0_SCMO_FSP_STATUS_FREQ_SET_POINT_REG_IN_USE_SHFT                     0x0


#endif /* __BIMC_ERROR_HWIO_H__ */
