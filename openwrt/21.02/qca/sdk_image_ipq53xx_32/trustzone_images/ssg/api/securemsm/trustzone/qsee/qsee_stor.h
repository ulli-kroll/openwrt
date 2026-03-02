#ifndef QSEE_STOR_H
#define QSEE_STOR_H

/* @file qsee_stor.h

  This file contains storage definitions for the secure apps

  Copyright (c) 2012-2015, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   -----   -----------------------------------------------------------
2018-10-04   jt      Update error codes
2018-04-24   jt      Add device ID for spinor
2015-12-09   jt      Add remove client API
2015-09-14   jt      Secure write protect support
2015-06-04   jt      Add access violation error code
2015-04-24   jt      Add autoprovisioning disabled error code
2013-03-04   jt      Adjusted error codes
2012-12-12   jt      Initial version

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
/** @cond */

/* Storage Device Return Error Codes */
#define QSEE_STOR_SUCCESS                     0  /* Success */
#define QSEE_STOR_ERROR                      -1  /* Generic Failure */
#define QSEE_STOR_INVALID_PARAM_ERROR        -2  /* Invalid arguments passed to the API */
#define QSEE_STOR_NOT_FOUND_ERROR            -3  /* Device Not found Error */
#define QSEE_STOR_PARTI_NOT_FOUND_ERROR      -4  /* Partition Not found Error */
#define QSEE_STOR_VERSION_MISMATCH           -5  /* Listener version Version mismatch */
#define QSEE_STOR_OUT_OF_RESOURCES           -6  /* Out of memory/other resources*/
#define QSEE_STOR_NOT_SUPPORTED              -7  /* Operation Not supported */
#define QSEE_STOR_RPMB_UNKNOWN_ERROR         -8  /* Unknown error during RPMB access */
#define QSEE_STOR_RPMB_MAC_ERROR             -9  /* MAC autentication error during RPMB access */
#define QSEE_STOR_RPMB_WRITE_COUNTER_ERROR   -10 /* Write counter mismatch during RPMB access */
#define QSEE_STOR_RPMB_ADDR_ERROR            -11 /* RPMB address failure */
#define QSEE_STOR_RPMB_WRITE_ERROR           -12 /* Failure during RPMB Write */
#define QSEE_STOR_RPMB_READ_ERROR            -13 /* Failure during RPMB Read */
#define QSEE_STOR_RPMB_NOT_PROVISIONED_ERROR -14 /* RPMB Not provisioned */
#define QSEE_STOR_RPMB_MAC_GENERATION_ERROR  -15 /* Failure during MAC generation */
#define QSEE_STOR_RPMB_RNG_GENERATION_ERROR  -16 /* Failure during RNG generation */
#define QSEE_STOR_RPMB_NONCE_ERROR           -17 /* Nonce authentication error */
#define QSEE_STOR_RPMB_PROVISIONED_ERROR     -18 /* RPMB already provisioned */
#define QSEE_STOR_PARTITION_FOUND            -19 /* Partition Found */
#define QSEE_STOR_APP_ID_ERROR               -20 /* Failed to create/get application id */
#define QSEE_STOR_RPMB_AUTOPROV_DISABLED     -21 /* RPMB autoprovisioning has been disabled */
#define QSEE_STOR_ACCESS_VIOLATION_ERROR     -22 /* Access violation error */
#define QSEE_STOR_WP_CONFIG_ACCESS_ERROR     -23 /* WP config read/write failure */
#define QSEE_STOR_WP_CONFIG_INV_PARAM_ERROR  -24 /* WP config invalid param */
#define QSEE_STOR_WP_NOT_APPLICABLE_ERROR    -25 /* WP config not allowed */
#define QSEE_STOR_RPMB_MAJOR_VERSION_ERROR   -26 /* RPMB partition major version break */
#define QSEE_STOR_RPMB_MAX_COUNTER           -27 /* RPMB write counter has reached its max value */
#define QSEE_STOR_DTPM_COUNTER_CREATE_ERROR  -28 /* dTPM counter creation failure */
#define QSEE_STOR_DTPM_COUNTER_READ_ERROR    -29 /* dTPM counter read failure (if created and password good) */
#define QSEE_STOR_DTPM_COUNTER_WRITE_ERROR   -30 /* dTPM counter write failure */

/* Storage Device Types */
typedef enum
{
  QSEE_STOR_EMMC_USER = 0,            /* User Partition in eMMC */
  QSEE_STOR_EMMC_BOOT0,               /* Boot0 Partition in eMMC */
  QSEE_STOR_EMMC_BOOT1,               /* Boot1 Partition in eMMC */
  QSEE_STOR_EMMC_RPMB,                /* RPMB Partition in eMMC */
  QSEE_STOR_EMMC_GPP1,                /* GPP1 Partition in eMMC */
  QSEE_STOR_EMMC_GPP2,                /* GPP2 Partition in eMMC */
  QSEE_STOR_EMMC_GPP3,                /* GPP3 Partition in eMMC */
  QSEE_STOR_EMMC_GPP4,                /* GPP4 Partition in eMMC */
  QSEE_STOR_EMMC_ALL,                 /* Entire eMMC device */
  QSEE_STOR_SPINOR_ALL = 0xb,         /* Entire SPINOR device */
  QSEE_STOR_ID_RESERVED = 0x7FFFFFFF  /* Reserved: Device ID Max */
} qsee_stor_device_id_type;

/* Device Information structure */
typedef struct
{
  qsee_stor_device_id_type dev_id;               /* Device ID (Physical partition number) */
  uint8_t                    partition_guid[16]; /* GUID for the partition in the device (GPT only) */
  uint32_t                   bytes_per_sector;   /* Bytes per Sector */
  uint32_t                   total_sectors;      /* Total size in sectors */
  uint32_t                   available_sectors;  /* Total available sectors for new partitions */
} __attribute__ ((packed)) qsee_stor_device_info_t;


/* Client Information structure */
typedef struct
{
  qsee_stor_device_id_type    dev_id;             /* Device ID */
  uint8_t                     partition_guid[16]; /* GUID for the partition in the device (GPT only) */
  uint32_t                    bytes_per_sector;   /* Bytes per Sector */
  uint32_t                    total_sectors;      /* Total size available in sectors */
} __attribute__ ((packed)) qsee_stor_client_info_t;

/* Secure Write Protect Info Entry structure */
typedef struct
{
   uint8_t  wp_enable;    /* UFS: WPF (Write Protect Flag), eMMC: SECURE_WP_MODE_ENABLE */
   uint8_t  wp_type_mask; /* UFS: WPT (Write Protect Type), eMMC: SECURE_WP_MODE_CONFIG */
   uint64_t addr;         /* UFS: LBA, eMMC: 0x1/0x2 (address of the device config register) */
   uint32_t num_blocks;   /* UFS: Num LBA, eMMC: Set to 0 */
} __attribute__ ((packed)) qsee_stor_secure_wp_info_entry_t;

/* Secure Write Protect Info structure */
typedef struct
{
   uint8_t                          lun_number;    /* UFS: LUN #, eMMC: Set to 0 */
   uint8_t                          num_entries;   /* Number of Secure wp entries */
   qsee_stor_secure_wp_info_entry_t wp_entries[4]; /* Max 4 entries total */
} __attribute__ ((packed)) qsee_stor_secure_wp_info_t;

typedef uint64_t qsee_stor_device_handle_t;
typedef uint64_t qsee_stor_client_handle_t;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_stor
  @{
*/

/**
  Initialize device indicated by device_id and partition_guid.

  @param[in]  device_id         Device partition number.
  @param[in]  partition_guid    Partition GUID (applies only for GPT partitions).
  @param[out] device_handle     Pointer to a device handle.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_stor_device_init(qsee_stor_device_id_type device_id, uint8_t *partition_guid,
                          qsee_stor_device_handle_t *device_handle);

/**
  Open a logical partition.

  @param[in]  device_handle  Pointer to a device handle obtained from
                             qsee_stor_device_init().
  @param[in]  partition_id   Logical partition ID.
  @param[out] client_handle  Pointer to a client handle.

  @return QSEE_STOR_SUCCESS if no errors; otherwise error code.

*/
int qsee_stor_open_partition(qsee_stor_device_handle_t *device_handle, uint32_t partition_id,
                             qsee_stor_client_handle_t *client_handle);

/**
  Returns device info.

  @param[in]  device_handle   Pointer to a device handle from qsee_stor_device_init().
  @param[out] device_info     Pointer to a device info structure.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_stor_device_get_info(qsee_stor_device_handle_t *device_handle,
                              qsee_stor_device_info_t *device_info);

/**
  Returns client info.

  @param[in]  client_handle   Pointer to the client handle from qsee_stor_open_partition().
  @param[in]  client_info     Pointer to a client info structure.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_stor_client_get_info(qsee_stor_client_handle_t *client_handle,
                              qsee_stor_client_info_t *client_info);

/**
  Adds a new logical partition.

  @param[in] device_handle    Pointer to device handle from qsee_stor_device_init().
  @param[in] partition_id     Logical Partition ID.
  @param[in] num_sectors      Number of sectors of the new logical partition.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_stor_add_partition(qsee_stor_device_handle_t *device_handle,
                            uint32_t partition_id, uint16_t num_sectors);

/**
  Read num_sectors of data from start_sector to data_buffer.

  @param[in]  client_handle  Pointer to a client handle from
                             qsee_stor_open_partition().
  @param[in]  start_sector   Starting sector to read from.
  @param[in]  num_sectors    Number of sectors to read.
  @param[out] data_buffer    Pointer to buffer containing read data.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_stor_read_sectors(qsee_stor_client_handle_t *client_handle, uint32_t start_sector,
                           uint32_t num_sectors, uint8_t *data_buffer);

/**
  Write num_sectors of data from data_buffer to start_sector.

  @param[in] client_handle   Pointer to a client handle from
                             qsee_stor_open_partition().
  @param[in] start_sector    Starting sector to write to.
  @param[in] num_sectors     Number of sectors to write.
  @param[in] data_buffer     Pointer to buffer containing data to be written.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_stor_write_sectors(qsee_stor_client_handle_t *client_handle, uint32_t start_sector,
                            uint32_t num_sectors, uint8_t *data_buffer);

/**
  Read the Secure Write Protect Configuration Block.

  @param[in,out] wp_info     Pointer to buffer containing information about
                             Secure Write Protect Configuration.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_stor_read_wp_config(qsee_stor_secure_wp_info_t *wp_info);

/**
  Write to the Secure Write Protect Configuration Block.

  @param[in] wp_info         Pointer to buffer containing information about
                             Secure Write Protect Configuration.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_stor_write_wp_config(qsee_stor_secure_wp_info_t *wp_info);

/**
  Closes a logical partition within RPMB. If the partition is
  the last in the partition table, it will reclaim space. Otherwise,
  it removes the client.

  @param[in] client_handle    Pointer to a client handle.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_stor_remove_client(qsee_stor_client_handle_t *client_handle);

/**
  Queries whether the OEM has configured RPMB enablement on the device.

  @param[out] b               Pointer to boolean query response.

  @return QSEE_STOR_SUCCESS if no errors; otherwise, error code.

*/
int qsee_query_rpmb_enablement(bool* b);

/** @} */

#endif /* QSEE_STOR_H */
