#ifndef QSEE_SEC_CAMERA_H
#define QSEE_SEC_CAMERA_H

/**
@file qsee_sec_camera.h
@brief Provide Secure Display functionality
*/

/*===========================================================================
   Copyright (c) 2016-2018 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_sec_camera.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
07/16/17   dr       Add mink callback object registration
03/20/17   dr       Add Haven license support
10/30/16   dr       Add 8998 support and ISPIF/CSID regions
05/12/16   ak       Add streaming status control functions
04/25/16   ak       Add CCI address space access functions
04/18/16   sn       Initial version
===========================================================================*/

#include <stdint.h>
#include <stddef.h>
#include "object.h"
/** @cond */

typedef enum {
  QSEE_SEC_CAM_REG_MMSS_A_CCI    = 0,
  QSEE_SEC_CAM_REG_MMSS_A_CSID_0 = 1,
  QSEE_SEC_CAM_REG_MMSS_A_CSID_1 = 2,
  QSEE_SEC_CAM_REG_MMSS_A_CSID_2 = 3,
  QSEE_SEC_CAM_REG_MMSS_A_CSID_3 = 4,
  QSEE_SEC_CAM_REG_MMSS_A_ISPIF   = 5,
} qsee_sec_cam_register_region_t;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_sec_camera
  @{
*/

/**
  Gets the current secure camera session ID.

  @param[out] sessionID   Current secure camera session ID; 0 if
                          no session is active.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1

*/
int qsee_sec_camera_get_session(uint32_t *sessionID);

/**
  - Blocks camera from being released out of secure mode.
  - Must be called by the client to ensure the secure session cannot
    be terminated by REE or other TAs.

  @param[in] sessionID    Current secure camera session ID.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1

*/
int qsee_sec_camera_acquire_camera(uint32_t sessionID);

/**
  - Allows camera to exit secure mode.
  - Must be called when the client allows REE to switch camera to non-secure
    mode.

  @param[in] sessionID    Current secure camera session ID.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1

*/
int qsee_sec_camera_release_camera(uint32_t sessionID);

/**
  Writes 32-bit data to the specified camera register region address space.

  @param[in] registerRegionId  Register region identifier, e.g., MMSS_A_CCI.
  @param[in] offset            Offset at register region.
  @param[in] data              Data to write.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1

*/
int qsee_sec_camera_register_write(qsee_sec_cam_register_region_t registerRegionId,
                                   uint32_t offset, uint32_t data);

/**
  Bulk writes 32-bit data to the specified camera register region address space.

  @param[in] registerRegionId  Register region identifier, e.g., MMSS_A_CCI.
  @param[in] offset            Array of offsets at the register region.
  @param[in] offset_len        Number of items in the offset array.
  @param[in] data              Array of data values to write (corresponding to the offsets).
  @param[in] data_len          Number of items in data array.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1

*/
int qsee_sec_camera_register_bulk_write(qsee_sec_cam_register_region_t registerRegionId,
                                        uint32_t offset[], size_t offset_len,
                                        uint32_t data[], size_t data_len);

/**
  Reads 32-bit data to the specified register address space.

  @param[in]  registerRegionId  Register region identifier, e.g., MMSS_A_CCI.
  @param[in]  offset            Offset at the register region.
  @param[out] data_ptr          Returned data.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1

*/
int qsee_sec_camera_register_read(qsee_sec_cam_register_region_t registerRegionId,
                                  uint32_t offset, uint32_t *data_ptr);

/**
  Sets and validates Haven license certificate for Secure Camera feature.

  @param[in]  licenseCert_ptr   Pointer to buffer containing the certificate in DER format.
  @param[in]  licenseCert_len   Certificate data length (in bytes).
  @param[out] havenError_ptr    Haven license validation result.\n
                                0 means success.\n
                                See IHavenTokenApp.h for more details on the error values.

  @detdesc
  This method must be called at least once after the device reboots to enable the
  secure camera feature.  Invoking secure camera without calling this method first, or if
  the provided certificate is not valid, results in a disabled secure camera feature. \n
  @note Non-production devices bypass the license check to support debug. In that case,
  it is still recommended to check havenError_ptr value to ensure that license validation
  succeeds on the production device as well.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1

*/
int qsee_sec_camera_set_haven_license(const uint8_t* licenseCert_ptr, size_t licenseCert_len,
                                      int32_t* havenError_ptr);

/**
  Registers the TA's callback object of type ISecureCameraClientEvent to TZ secure camera
  module.

  This function should be called by TA to receive notification about secure mode change.
  The object provided should be a MINK object implementing ISecureCameraClientEvent interface
  which is defined in ISecureCameraClientEvent.h.\n
  When the camera protect or unprotect event takes place, the TZ kernel camera module calls
  the provided callback object.
  For TA to receive this notification successfully, the SecureCameraClientEvent must be
  added to the supported TA services list.

  @param [in]  callback_object   MINK object of type ISecureCameraClientEvent.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1

*/
int qsee_sec_camera_register_callback(Object callback_object);

/** @} */

#endif /* QSEE_SEC_CAMERA_H */

