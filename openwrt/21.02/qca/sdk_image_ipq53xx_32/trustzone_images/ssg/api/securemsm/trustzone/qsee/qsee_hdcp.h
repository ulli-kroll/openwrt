/*===========================================================================
 *  Copyright (c) 2017 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *=========================================================================*/

#ifndef __QSEE_HDCP_H
#define __QSEE_HDCP_H

#include <stdbool.h>
#include <stdint.h>

/**
  Function used to reference count the enforcement of HDMI HDCP encryption

  @note1hang
  This API is deprecated. Use IOPS idl instead.

  @param[in] enforce Enforcement vote (0 or 1)
*/
int qsee_enforce_hdmi_hdcp_encryption(unsigned int enforce);

/**
  Function used to set the enablement of CPI assert

  @param[in] cpi_assert_enable TRUE for enable; FALSE for disable
*/
int qsee_set_hdcp_cpi_assert(bool cpi_assert_enable);

/**
  Function used to acquire the status of the HDMI link and hardware HDCP

  @param[out] hdmi_enable HDMI output enabled status return location
  @param[out] hdmi_sense  Panel Hot Plug Detect connect status return location
  @param[out] hdcp_auth   HDCP authentication status return location
*/
int qsee_hdmi_status_read(uint32_t *hdmi_enable, uint32_t *hdmi_sense,
                          uint32_t *hdcp_auth);

#endif //__QSEE_HDCP_H
