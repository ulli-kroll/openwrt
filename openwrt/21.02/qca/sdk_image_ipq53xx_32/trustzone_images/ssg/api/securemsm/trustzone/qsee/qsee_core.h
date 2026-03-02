#ifndef QSEE_CORE_H
#define QSEE_CORE_H

/**
@file qsee_core.h
@brief Provide Core functionality
*/

/*===========================================================================
   Copyright (c) 2014 by QUALCOMM, Technology Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_core.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/25/14    hw      add new api qsee_get_secure_state
08/09/11    rv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
/** @cond */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  uint32_t value[2];
} qsee_secctrl_secure_status_t;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_core
  @{
 */

/**
  Reads the JTAG ID.

  @return
  JTAG ID value.

*/
int qsee_read_jtag_id(void);

/**
  Reads the serial number from the PTE chain.

  @return
  Serial number.

*/
int qsee_read_serial_num(void);

/**
  Tests if the range [\c start, \c start + \c len] is in
  nonsecure memory. This is a convenience function to access \c
  tzbsp_is_ns_area. \c NULL is a valid value for \c start, because
  physical addressing is used.

  @param[in] start Start of the memory range, physical address,
                   included in the range.

  @param[in] len   Memory range length (in bytes).

  @return
  TRUE  -- If entire area is in nonsecure memory. \n
  FALSE -- If area contains secure memory.

*/
bool qsee_is_ns_range(const void* start, uint32_t len);

/**
  Tests if the range [\c start, \c end] is tagged for the particular virtual
  machine ID (VMID), specific to the CPZ use case.

  @param[in] vmid  VMID defined in access
                   control layer (enum ACVirtualMachineId).

  @param[in] start Start of the memory range, physical address,
                   included in the range.

  @param[in] end   End of the memory range, physical address,
                   included in the range.

  @return
  TRUE  -- If entire area is tagged for the specified VMID \n
  FALSE -- If entire area is not tagged for the specified VMID

*/
bool qsee_is_s_tag_area
(
  uint32_t                    vmid,
  uint64_t                    start,
  uint64_t                    end
);

/**
  Tags all memory in the range [\c start, \c end] with the specified VMID.

  @param[in] vmid   VMID defined in access
                    control layer (enum ACVirtualMachineId)

  @param[in] start  Start of the memory range, physical address,
                    included in the range.

  @param[in] end    End of the memory range, physical address,
                    included in the range.

  @return
  CALL SUCCESS -- 0

*/
int qsee_tag_mem(uint32_t vmid, uint64_t start, uint64_t end);

/**
  Counts the number of 4 KB memory chunks in the specified virtual machine (VM).

  @param[in] vmid VMID defined in access
                  control layer (enum ACVirtualMachineId).

  @return
  The number of 4 KB chunks in the VM.

*/
uint32_t qsee_vm_mem_count(
  uint32_t    vmid
);


/**
  Reads HDMI link and hardware HDCP status.

  @param[out] hdmi_enable  HDMI output enabled.
  @param[out] hdmi_sense HDMI sense.
  @param[out] hdcp_auth HDCP authentication success.

  @return
  CALL SUCCESS -- 0

*/
int qsee_hdmi_status_read
(
  uint32_t* hdmi_enable,
  uint32_t* hdmi_sense,
  uint32_t* hdcp_auth
);


/**
  Checks device security status.

  @param[out] status    Pointer to security status (struct qsee_secctrl_secure_status_t)
                        with the following bit field definitions:
                        - Bit 0: secboot enabling check failed
                        - Bit 1: Sec hardware key not programmed
                        - Bit 2: Debug disable check failed
                        - Bit 3: Anti-rollback check failed
                        - Bit 4: Fuse config check failed
                        - Bit 5: RPMB provision check failed
                        - Bit 6: Debug check in image certificate failed \n \n
                        Debug Bits \n
                        - Bit 7: RSVD
                        - Bit 8: TZ secure debug fuse check failed
                        - Bit 9: MSS secure debug fuse check failed
                        - Bit 10: CP secure debug fuse check failed
                        - Bit 11: Nonsecure secure debug fuse check failed

  @return
  CALL SUCCESS -- 0

*/
int qsee_get_secure_state(qsee_secctrl_secure_status_t* status);


/**
  Returns the application distinguished ID stored in the QSEE app certificate.

  @param[out] tz_app_id  Pointer to buffer populated with the application
                         distinguished ID.
  @param[in]  id_buf_len Output buffer length (in bytes).

  @return
  CALL SUCCESS -- 0

  @note1hang The output buffer must be at least the size of distID (32 bytes).

*/
int qsee_get_tz_app_id(uint8_t * tz_app_id, uint32_t id_buf_len);


/**
  Provides the UUID for the device.

  @param[out]     uuid_ptr Pointer to a buffer filled with a struct
                           based an IETF UUID (GP compatible).
  @param[in,out]  uuid_len Pointer to uuid buffer size.

  @return
  CALL SUCCESS -- 0

  @note1hang The output buffer must be at least the size of an IETF UUID (32 bytes).
             uuid_len pointer is updated to indicate actual UUID size.

*/
int qsee_get_device_uuid(uint8_t *uuid_ptr, size_t *uuid_len);


/**
  Details the TEE implementation version number.

  @param[out]    version_ptr Pointer to a buffer filled with the QTEE
                             version number.
  @param[in,out] version_len Pointer to version buffer size.

  @return
  CALL SUCCESS -- 0

  @note1hang The version number is a printable ASCII string, but is not
             NULL-terminated. The maximum buffer size is 128 bytes.
             version_len is updated to indicate the actual length of the
             version string.

*/
int qsee_get_trusted_os_component_version(uint8_t *version_ptr, size_t *version_len);


/**
  The detailed firmware version number that supports the trusted OS
  implementation includes all privileged software involved in TEE
  secure booting and support, apart from the secure OS and trusted
  applications.

  @param[out]    version_ptr Pointer to a buffer filled with the
                             version number.
  @param[in,out] version_len Pointer to version buffer size.

  @return
  CALL SUCCESS -- 0

  @note1hang The version number is a printable ASCII string, but is not
             NULL-terminated. The maximum buffer size is 128 bytes. The
             version_len pointer is updated to indicate the actual version
             string length.

*/
int qsee_get_fw_component_version(uint8_t *version_ptr, size_t *version_len);

/** @} */

#endif /* QSEE_CORE_H */

