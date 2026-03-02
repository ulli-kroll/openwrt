#ifndef TZBSP_SYSCALL_PRIV_H
#define TZBSP_SYSCALL_PRIV_H

/**
   @file tzbsp_syscall_priv.h
   @brief Provide the private SYSCALL API infrastructure
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The xxx_mainpage.dox file contains all descriptions that are displayed
      in the output PDF generated using Doxygen and LaTeX. To edit or update
      any of the file/group text in the PDF, edit the xxx_mainpage.dox
      file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
   Copyright (c) 2010-2012, 2016,2019 by Qualcomm Technologies, Inc.
   All rights reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/securemsm/trustzone/qsee/include/tzbsp_syscall_priv.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
11/15/19   gp       Removed LMh support
03/21/12   pre      Initial version.
04/25/14   dp       Removed CPU DCVS and updated GFX DCVS
02/18/16   dk       Make GFX DCVS Context Aware

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "tz_syscall_pub.h"

/** DCVS service. */
#define TZ_SVC_DCVS               13

#ifdef TZBSP_WITH_TEST_SVC
/**
   @ingroup tzos_load_tztestexec_image

   Loads tztestexec image into memory.

   @smc_id
     0x32000300

   @param_id
     0x00000003

   @sys_call_params{tz_os_load_external_image_req_s}
     @table{weak__tz__os__load__external__image__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_LOAD_TZTESTEXEC_IMAGE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_EXTERNAL, 0x00)

#define TZ_OS_LOAD_TZTESTEXEC_IMAGE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )
#endif

/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZBSP_GFX_DCVS_RESET_OLD_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x03)

#define TZBSP_GFX_DCVS_UPDATE_OLD_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x04)

#define TZBSP_DCVS_CREATE_GROUP_DEPRECATED_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x1)

#define TZBSP_DCVS_REGISTER_CORE_DEPRECATED_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x2)

#define TZBSP_DCVS_SET_ALG_PARAMS_DEPRECATED_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x3)

#define TZBSP_DCVS_SYSTEM_EVENT_DEPRECATED_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x4)

#define TZBSP_DCVS_INIT_DEPRECATED_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x5)

#define TZBSP_GFX_DCVS_INIT_OLD_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x6)

/**
   @ingroup gfx_dcvs_reset

   API for the graphics team to control the proprietary dynamic clock
   and voltage scaling routine.

   @note1hang Implementation takes place as a TZ kernel service

   @owner_id
     0x00000002

   @command_id
     0x00000D03

   @param_id
     0x00000000

   @return
     \c 0                Always successful.
*/
#define TZBSP_GFX_DCVS_RESET_ID                           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x07)

#define TZBSP_GFX_DCVS_RESET_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup gfx_dcvs_update

   API for the graphics team to control the proprietary dynamic clock
   and voltage scaling routine.

   @owner_id
     0x00000002

   @command_id
     0x00000D04

   @param_id
     0x00000001

   @param[in] data: tzbsp_gfx_dcvs_update_data_t structure containing
                    power level, total value, and busy value.
   @param[out] decision_buf: Pointer to an int.
     \c  0               Continue in last performance level.
     \c  1               Increase the performance level.
     \c -1               Decrease the performance level.
   @param[in] decision_buf_size: Size of result buffer.

   @note1hang Implementation takes place as a TZ kernel service

   @return
     \c  E_SUCCESS       If successful.
     \c -E_BAD_ADDRESS   If structure pointer/memory is invalid.
     \c -E_OUT_OF_RANGE  If power levels are out of range
                         or buffer size is too small.
     \c -E_FAILURE       If number of power levels is zero.


*/
#define TZBSP_GFX_DCVS_UPDATE_ID                          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x08)

#define TZBSP_GFX_DCVS_UPDATE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup gfx_dcvs_init

   Used to initialize the Graphics DCVS.

   @param[in] data: tzbsp_gfx_dcvs_init_data_t struct containing number
                    of power levels and array of frequency values.
   @param[out] version_buf: A buffer of size unsigned int containing
                    versioning of GFX DCVS TZ implementation.
   @param[in] version_buf_size: Size of version buffer.

   @note1hang Implementation takes place as a TZ kernel service

   @return
     \c 0                If successful.
     \c -E_BAD_ADDRESS   If structure pointer/memory is invalid, or if
                         buffer is not in NS memory.
     \c -E_OUT_OF_RANGE  If power levels are out of range, or if
                         buffer size is too small.
     \c -E_FAILURE       If number of power levels is zero.

   @command_id
     0x00003406

   @com_datatypes
     tz_syscall_req_s \n
     tz_syscall_rsp_s
 */
#define TZBSP_GFX_DCVS_INIT_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x09)

#define TZBSP_GFX_DCVS_INIT_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup gfx_dcvs_update

   API for the graphics team to control the proprietary dynamic clock
   and voltage scaling routine.

   @owner_id
     0x00000002

   @command_id
     0x00000D04

   @param_id
     0x00000001

   @param[in] data: tzbsp_gfx_dcvs_update_data_t structure containing
                    power level, total value, and busy value.
   @param[out] decision_buf: Pointer to an int.
     \c  0               Continue in last performance level.
     \c  1               Increase the performance level.
     \c -1               Decrease the performance level.
   @param[in] decision_buf_size: Size of result buffer.

   @note1hang Implementation takes place as a TZ kernel service

   @return
     \c  E_SUCCESS       If successful.
     \c -E_BAD_ADDRESS   If structure pointer/memory is invalid.
     \c -E_OUT_OF_RANGE  If power levels are out of range
                         or buffer size is too small.
     \c -E_FAILURE       If number of power levels is zero.


*/
#define TZBSP_GFX_DCVS_UPDATE_FLATTENED_ID                          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x0A)

#define TZBSP_GFX_DCVS_UPDATE_FLATTENED_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup gfx_dcvs_init

   Used to initialize the Graphics DCVS.

   @param[in] data: tzbsp_gfx_dcvs_init_data_t struct containing number
                    of power levels and array of frequency values.
   @param[out] version_buf: A buffer of size unsigned int containing
                    versioning of GFX DCVS TZ implementation.
   @param[in] version_buf_size: Size of version buffer.

   @note1hang Implementation takes place as a TZ kernel service

   @return
     \c 0                If successful.
     \c -E_BAD_ADDRESS   If structure pointer/memory is invalid, or if
                         buffer is not in NS memory.
     \c -E_OUT_OF_RANGE  If power levels are out of range, or if
                         buffer size is too small.
     \c -E_FAILURE       If number of power levels is zero.

   @command_id
     0x00003406

   @com_datatypes
     tz_syscall_req_s \n
     tz_syscall_rsp_s
 */
#define TZBSP_GFX_DCVS_INIT_WITH_SIZE_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x0B)

#define TZBSP_GFX_DCVS_INIT_WITH_SIZE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup gfx_dcvs_ca_init

   Used to initialize the Graphics DCVS context aware jump.

   @param[in] data: tzbsp_gfx_dcvs_init_ca_data_t struct containing target
                    power levels and busy penality for context aware jump.

   @note1hang Implementation takes place as a TZ kernel service

   @return
     \c 0                If successful.

   @command_id
     0x00003406

   @com_datatypes
     tz_syscall_req_s \n
     tz_syscall_rsp_s
 */
#define TZBSP_GFX_DCVS_INIT_CA_WITH_SIZE_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x0C)

#define TZBSP_GFX_DCVS_INIT_CA_WITH_SIZE_ID_PARAM_ID \
	TZ_SYSCALL_CREATE_PARAM_ID_2( \
		TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup gfx_dcvs_update

   API for the graphics team to control the proprietary dynamic clock
   and voltage scaling routine.

   @owner_id
     0x00000002

   @command_id
     0x00000D04

   @param_id
     0x00000001

   @param[in] data: tzbsp_gfx_dcvs_update_data_t structure containing
                    power level, total value, busy value and context count.
   @param[out] decision_buf: Pointer to an int.
     \c  0               Continue in last performance level.
     \c  1               Increase the performance level.
     \c -1               Decrease the performance level.
   @param[in] decision_buf_size: Size of result buffer.

   @note1hang Implementation takes place as a TZ kernel service

   @return
     \c  E_SUCCESS       If successful.
     \c -E_BAD_ADDRESS   If structure pointer/memory is invalid.
     \c -E_OUT_OF_RANGE  If power levels are out of range
                         or buffer size is too small.
     \c -E_FAILURE       If number of power levels is zero.


*/
#define TZBSP_GFX_DCVS_UPDATE_FLATTENED_WITH_CA_ID                          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DCVS, 0x0D)

#define TZBSP_GFX_DCVS_UPDATE_FLATTENED_WITH_CA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup get_ablfv_entry_point

   Get the entry point of the Apps Boot Loader Firmware Volume
   (ABL.FV) image for UEFI. Can only be called once per cold boot.

   @smc_id
     0x02000113

   @param_id
     0x00000000

   @sys_call_params{tz_get_image_entry_point_rsp_s}
     @table{weak__tz__get__image__entry__point__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_GET_IMAGE_ENTRY_POINT_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x13)

#define TZ_GET_IMAGE_ENTRY_POINT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/*----------------------------------------------------------------------------
 * Graphics DCVS structures
 * -------------------------------------------------------------------------*/

#define TZBSP_GPU_DCVS_NUMPWRLEVELS                 10

/**
   @ingroup gfx_dcvs_update

   Request message structure corresponding to the message ID
   TZBSP_GFX_DCVS_UPDATE_ID.
*/
typedef struct tzbsp_gfx_dcvs_update_data_s
{
  uint32 active_pwrlevel;               /**< Power level index */
  uint32 total;                         /**< Total vector index */
  uint32 inBusy;                        /**< Busy vector index */
  uint32 ctxt_aware_ctxt_count;         /**< Currently active contexts count*/
} __attribute__ ((packed)) tzbsp_gfx_dcvs_update_data_t;

typedef struct tzbsp_gfx_dcvs_update_req_s
{
  tzbsp_gfx_dcvs_update_data_t* data;     /**< Data structure for update */
} __attribute__ ((packed)) tzbsp_gfx_dcvs_update_req_t;

typedef struct tzbsp_gfx_dcvs_update_rsp_s
{
  int32 decision;                       /**< Result of power level either
                                             increased, decreased, or stayed
                                             the same */
} __attribute__ ((packed)) tzbsp_gfx_dcvs_update_rsp_t;

typedef struct tzbsp_gfx_dcvs_init_data_s
{
  uint32 nlevels;                       /**< Number of levels provided in
                                             freq array */
  uint32 freq[TZBSP_GPU_DCVS_NUMPWRLEVELS]; /**< Array of GPU frequencies */
} __attribute__ ((packed)) tzbsp_gfx_dcvs_init_data_t;

typedef struct tzbsp_gfx_dcvs_init_rsp_s
{
  uint32 version;                       /**< Version of TZ GFX DCVS implementation */
} __attribute__ ((packed)) tzbsp_gfx_dcvs_init_rsp_t;

/**
   @ingroup gfx_dcvs_init

   Request message structure corresponding to the message ID
   TZBSP_GFX_DCVS_INIT_ID.
*/
typedef struct tzbsp_gfx_dcvs_init_req_s
{
  tzbsp_gfx_dcvs_init_data_t* data;     /**< Data structure for init */
} __attribute__ ((packed)) tzbsp_gfx_dcvs_init_req_t;

typedef struct tzbsp_gfx_dcvs_init_ca_data_s
{
  uint32 ctxt_aware_target_pwrlevel;     /**< Target pwrlevel index for Context aware jump */
  uint32 ctxt_aware_busy_penalty;        /**< Busy penality for context aware jump*/
} __attribute__ ((packed)) tzbsp_gfx_dcvs_init_ca_data_t;

/**
   @ingroup gfx_dcvs_init_ca

   Request message structure corresponding to the message ID
   TZBSP_GFX_DCVS_INIT_CA_ID.
*/
typedef struct tzbsp_gfx_dcvs_init_ca_req_s
{
  tzbsp_gfx_dcvs_init_ca_data_t* data;     /**< Data structure for init */
} __attribute__ ((packed)) tzbsp_gfx_dcvs_init_ca_req_t;

/**
   @weakgroup weak_tz_get_ablfv_entry_point_rsp_s
@{
*/

typedef struct tz_get_ablfv_entry_point_rsp_s
{
  uint64 e_entry;  /**< Image entry point. */

} __attribute__ ((packed)) tz_get_ablfv_entry_point_rsp_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * LMH Private Driver Structures
 * -------------------------------------------------------------------------*/

#define LMH_PACKET_NODE_COUNT 10
#define LMH_DEBUG_DATA_COUNT 5

/**
   @weakgroup weak_tz_lmh_node_info_s
@{
*/
typedef struct tz_lmh_node_info_s
{
  uint32                node;               /**< Node type (4-character string). */
  uint32                node_id;            /**< Node ID number. */
  uint32                intensity;          /**< Current throttling intensity . */
  uint32                max_intensity;      /**< Maximum throttling intensity. */
  uint32                type;               /**< Sensor type. */
} __attribute__ ((packed)) tz_lmh_node_info_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_lmh_node_packet_s
@{
*/
typedef struct tz_lmh_node_packet_s
{
  uint32                count;              /**< Number of nodes in a packet. */
  tz_lmh_node_info_t    nodes[LMH_PACKET_NODE_COUNT];  /**< Array of nodes. */
} __attribute__ ((packed)) tz_lmh_node_packet_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_lmh_debug_header_s
@{
*/
typedef struct tz_lmh_debug_header_s
{
  uint32                node;               /**< Node type (4-character string). */
  uint32                node_id;            /**< Node ID number. */
  uint32                data_type;          /**< Identifier for data being returned. */
  uint64                timestamp;          /**< Timestamp for data collection. */
} __attribute__ ((packed)) tz_lmh_debug_header_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_lmh_debug_packet_s
@{
*/
typedef struct tz_lmh_debug_packet_s
{
  tz_lmh_debug_header_t header;             /**< Identifier for packet data. */
  uint32                data[LMH_DEBUG_DATA_COUNT];  /**< Array of debug data in 32-bit words. */
} __attribute__ ((packed)) tz_lmh_debug_packet_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tzbsp_lmh_change_profile_req_s
@{
*/
typedef struct tzbsp_lmh_change_profile_req_s
{
  uint32                profile;            /**< Profile ID to which to switch. */
} __attribute__ ((packed)) tzbsp_lmh_change_profile_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tzbsp_lmh_get_profiles_req_s
@{
*/
typedef struct tzbsp_lmh_get_profiles_req_s
{
  uint32                profile_list_addr;  /**< Array to which to return profile IDs. */
  uint32                size;               /**< Size of profile_list array in bytes; must be 10* the size of (uint32). */
  uint32                start;              /**< Number of profile IDs to skip before beginning the list. */
} __attribute__ ((packed)) tzbsp_lmh_get_profiles_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tzbsp_lmh_enable_qpmda_req_s
@{
*/
typedef struct tzbsp_tzbsp_lmh_enable_qpmda_s
{
  uint32                enable;             /**< Enable bitmask. */
  uint32                rate;               /**< Timestamp request rate in XO ticks. */
} __attribute__ ((packed)) tzbsp_lmh_enable_qpmda_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tzbsp_lmh_set_current_limit_req_s
@{
*/
typedef struct tzbsp_lmh_set_current_limit_req_s
{
  uint32                    limit;          /**< Value to set. */
  uint32                    limit_type;     /**< Value type. */
  uint32                    limit_id;       /**< Limit profile to update. */
} __attribute__ ((packed)) tzbsp_lmh_set_current_limit_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tzbsp_lmh_intensity_req_s
@{
*/
typedef struct tzbsp_lmh_intensity_req_s
{
  tz_lmh_node_packet_t*     packet;         /**< Packet buffer. */
  uint32                    size;           /**< Size of the packet buffer in bytes. */
} __attribute__ ((packed)) tzbsp_lmh_intensity_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tzbsp_lmh_get_sensor_list_req_s
@{
*/
typedef struct tzbsp_lmh_get_sensor_list_req_s
{
  tz_lmh_node_packet_t*     packet;         /**< Packet buffer. */
  uint32                    size;           /**< Size of the packet buffer in bytes. */
} __attribute__ ((packed)) tzbsp_lmh_get_sensor_list_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak__tzbsp__lmh__debug__set__req__s
@{
*/
typedef struct tzbsp_lmh_debug_set_req_s
{
  uint32                debug_param_addr;   /**< Array to provide debug parameters. */
  uint32                size;               /**< Size of debug_param array in bytes, must be a multiple of size of uint32. */
  uint32                node;               /**< Node type (4-character string). */
  uint32                node_id;            /**< Node ID number. */
  uint32                debug_type;         /**< Identifier for debug action. */
} __attribute__ ((packed)) tzbsp_lmh_debug_set_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak__tzbsp__lmh__debug__config__read__req__s
@{
*/
typedef struct tzbsp_lmh_debug_config_read_req_s
{
  uint32                read_param_addr;    /**< Array to provide read config parameters. */
  uint32                size;               /**< Size of debug_param array in bytes, must be a multiple of size of uint32. */
  uint32                node;               /**< Node type (4-character string). */
  uint32                node_id;            /**< Node ID number. */
  uint32                read_type;          /**< Identifier for read action. */
} __attribute__ ((packed)) tzbsp_lmh_debug_config_read_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak__tzbsp__lmh__debug__read__req__s
@{
*/
typedef struct tzbsp_lmh_debug_read_req_s
{
  tz_lmh_debug_packet_t* debug_packets;     /**< Array of debug packets. */
  uint32                size;               /**< Size of debug_packets array in bytes. */
} __attribute__ ((packed)) tzbsp_lmh_debug_read_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak__tzbsp__lmh__debug__get__commands__req__s
@{
*/
typedef struct tzbsp_lmh_debug_get_commands_req_s
{
  uint32                command_list_addr;  /**< Array to return command ids to. */
  uint32                size;               /**< Size of command_list array in bytes, must be 10 * size of uint32. */
  uint32                command_type;       /**< Identifier for command types to return. */
  uint32                start;              /**< Number of command ids to skip before beginning list. */
} __attribute__ ((packed)) tzbsp_lmh_debug_get_commands_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak__tzbsp__lmh__analytics__set__req__s
@{
*/
typedef struct tzbsp_lmh_analytics_set_req_s
{
  uint32                node;               /**< Node type (4-character string). */
  uint32                node_id;            /**< Node ID number. */
  uint32                analytics_type;     /**< Identifier for analytics action. */
  uint32                enable;             /**< Switch to enable or disable specified analytics actions. */
} __attribute__ ((packed)) tzbsp_lmh_analytics_set_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak__tzbsp__lmh__analytics__read__req__s
@{
*/
typedef struct tzbsp_lmh_analytics_read_req_s
{
  uint32                node;               /**< Node type (4-character string). */
  uint32                node_id;            /**< Node ID number. */
} __attribute__ ((packed)) tzbsp_lmh_analytics_read_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak__tzbsp__lmh__analytics__get__types__req__s
@{
*/
typedef struct tzbsp_lmh_analytics_get_types_req_s
{
  uint32                analytics_list_addr;/**< Array to return analytics ids to. */
  uint32                size;               /**< Size of analytics_list array in bytes, must be 10 * size of uint32. */
  uint32                start;              /**< Number of analytics ids to skip before beginning list. */
} __attribute__ ((packed)) tzbsp_lmh_analytics_get_types_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tzbsp_lmh_config_limit_req_s
@{
*/
typedef struct tzbsp_lmh_config_limit_req_s
{
  uint32                    node;           /**< Node type (4-character string). */
  uint32                    node_id;        /**< Node ID number. */
  uint32                    limit_type;     /**< Identifier for limit type. */
  uint32                    limit;          /**< Value to set. */
  uint32                    option;         /**< Setting option. */
} __attribute__ ((packed)) tzbsp_lmh_config_limit_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tzbsp_lmh_dcvsh_config_req_s
@{
*/
typedef struct tzbsp_lmh_dcvsh_config_req_s
{
  uint32                    data_addr;      /**< Configuration data array. */
  uint32                    data_len;       /**< Length of configuration data array. */
  uint32                    node;           /**< Node type (4-character string). */
  uint32                    node_id;        /**< Node ID number. */
  uint32                    version;        /**< Identifier for configuration version. */
} __attribute__ ((packed)) tzbsp_lmh_dcvsh_config_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tzbsp_lmh_sensor_init_req_s
@{
*/
typedef struct tzbsp_lmh_sensor_init_req_s
{
  uint64                    sensor_id;      /**< Sensor id (8-character string). */
  uint64                    cmd_id;         /**< Sensor init cmd. */
  uint64                    arg0;           /**< Value to set. */
  uint64                    arg1;           /**< Value to set. */
  uint64                    arg2;           /**< Value to set. */
  uint64                    arg3;           /**< Value to set. */
} __attribute__ ((packed)) tzbsp_lmh_sensor_init_req_t;

/** @} */ /* end_weakgroup */
#endif /* TZBSP_SYSCALL_PRIV_H */
