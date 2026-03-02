#ifndef SFS_OEM_UTILS_H
#define SFS_OEM_UTILS_H

/**
@file qsee_oem_utils.h
@brief Provides API's for OEM utility functions
 */

/*=============================================================================
              Copyright (c) 2012-2018 QUALCOMM Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary

=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_oem_utils.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/12   yh      Initial version
=============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "qsee_heap.h"

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_oem_utils
  @{
*/

/**
  Retrieves derivation key for SFS secret key. \n

  The key_len value shall not be less than 16; otherwise, QTEE will still use the
  hardware-based key as the SFS KDF derivation key.
  If key_len value exceeds 32, QTEE will only take the first 32 bytes as the
  SFS KDF derivation key.

  @details When not using the OEM key, the SFS secret key is different depending
  on whether the JTAG_DISABLE eFuse is blown.
  Before the fuse is blown, SFS KDF uses the dummy hardware derived key as the KDF derivation key.
  After the fuse is blown, SFS KDF uses the hardware derived key as the KDF derivation key.

  If the key is set by OEM, SFS KDF always uses the OEM key as the KDF derivation key
  to create the SFS secret key.
  JTAG_DISABLE eFuse does not affect the SFS secret key value.

  @return size_t length of the OEM SFS derivation key.

 */
size_t qsee_oem_set_kdf_derive_key(void);

/**
  Controls SFS file segment size.

  @details SFS creates several sub-files of equal size (file segments, or simply segments for short)
  which are then encrypted and integrity-protected before being written to the underlying file system.
  This function lets the OEM decide the segment size.\n
  For example, OEM have 2000 bytes plain text to save to SFS. The default SFS segment size is 1024 bytes,
  so SFS creates two segments (1024 bytes, 976 bytes + 48 bytes padding) on the REE file system.
  If OEM changes the segment size to 2048 bytes, SFS creates one segment (2000 bytes + 48 bytes padding) on the
  REE file system.

  OEM must be very cautious when changing segment size.

  @return Segment size.

  @par<text>Side effects<text> After the segment size change, SFS files saved with the old segment are useless.
  Old SFS files on the REE file system must be cleaned manually.
 */

uint32_t qsee_sfs_get_segment_size(void);

/**
  Controls SFS sanity mode.

  @details When qsee_sfs_open is called, SFS checks the sanity of each segment.
  This process is expensive and includes data segment authentication and decryption.
  OEM can skip this process to improve performance by setting the safe mode to FALSE.

  @return Safe or fast mode.
  @par<text>Side effects<text> qsee_sfs_open does not return an error if the data segment is corrupt.
  However, qsee_sfs_ API call (read/write) will fail afterwards.
*/
bool qsee_sfs_set_safe_mode(void);

/**
  Sets SFS and TZ counter file root path.

  @details By default, SFS files are saved under the /persist/data/sfs/ folder.
  TZ counter files are saved under the /persist/data/tz/ folder with the
  default root path is /persist/data/ .\n
  This function lets OEM set the root folder other than /persist/data/ .\n
  For example, if OEM set the root path to /OEM/subfolder/
  SFS and TZ counter files are generated under /OEM/subfolder/sfs/ and
  /OEM/subfolder/tz/ ,
  the path must:
  - start with '/',
  - end with '/', and
  - path length be less than 128 bytes.

  @return OEM defined root path string.

  @par<text>Side effects<text> The value is defined at compile time, and the path must not
  change for a production build.
 */
const char * qsee_sfs_set_root_path(void);

/** @} */

#endif /* SFS_OEM_UTILS_H */
