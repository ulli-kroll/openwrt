#ifndef QSEE_ESE_SERVICE_H
#define QSEE_ESE_SERVICE_H

/**
 *
 *  @file qsee_ese_service.h
 *
 *  @brief Provide Embedded Secure Element (eSE) Services API wrappers. The eSE QSEE service
 *         resides in the QSEE kernel.
 *         The has the following responsibilities:
 *         - Management of multiple sessions and open channels.
 *         - Sending Command Application Protocol Data Units (C-APDUs) to the eSE driver
 *         - Receiving Response Application Protocol Data Units (R-APDUs) from the eSE driver.
 *
 */

/*===========================================================================
  Copyright (c) 2015 - 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

  when            who         what, where, why
  --------        ---         -----------------------------------
  11-Oct-2016     dc          Add QSEE_ESE_BASIC_CHANNEL_NUMBER
  15-Dec-2015     dc          Add support for SPI Chip Select slave identifier
  20-Apr-2015     dc          Initial Version

  ===========================================================================*/

#include <stdbool.h>
#include <stdint.h>

#include "qsee_heap.h"
#include "qsee_log.h"
#include "qsee_spi.h"
#include "qsee_utils.h"

#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
/** @cond */

// clang-format off
#define QSEE_ESE_APDU_CLA_CHANNEL_MASK         0x03
#define QSEE_ESE_APDU_CLA_SZ                   1
#define QSEE_ESE_APDU_INS_SZ                   1
#define QSEE_ESE_APDU_P1_SZ                    1
#define QSEE_ESE_APDU_P2_SZ                    1
#define QSEE_ESE_APDU_LC_SZ                    1
#define QSEE_ESE_APDU_LE_SZ                    1
#define QSEE_ESE_APDU_EXT_LC_SZ                3
#define QSEE_ESE_APDU_EXT_LE_SZ_WITH_LC        2
#define QSEE_ESE_APDU_HDR_SZ                   (QSEE_ESE_APDU_CLA_SZ + QSEE_ESE_APDU_INS_SZ + QSEE_ESE_APDU_P2_SZ + QSEE_ESE_APDU_P2_SZ)
#define QSEE_ESE_CPDU_PAYLOAD_SZ               0xFFFF  // 7816-4 Maximum extended CAPDU
#define QSEE_ESE_RPDU_PAYLOAD_SZ               0x10000 // 7816-4 Maximum extended RAPDU
#define QSEE_ESE_RPDU_STATUS_SZ                0x02
#define QSEE_ESE_CPDU_MAX_SZ                   (QSEE_ESE_APDU_HDR_SZ + QSEE_ESE_APDU_EXT_LC_SZ + QSEE_ESE_CPDU_PAYLOAD_SZ + QSEE_ESE_APDU_EXT_LE_SZ_WITH_LC)
#define QSEE_ESE_RPDU_MAX_SZ                   (QSEE_ESE_RPDU_PAYLOAD_SZ + QSEE_ESE_RPDU_STATUS_SZ)
#define QSEE_ESE_APDU_SCP_OVERHEAD             100
#define QSEE_ESE_MAX_NUM_OF_SESSIONS           4
#define QSEE_ESE_MAX_NUMBER_OF_CHANNELS        20
#define QSEE_ESE_BASIC_CHANNEL_NUMBER          0x00
#define QSEE_ESE_T1_MAX_FRAME_SZ               258
#define QSEE_ESE_AID_MIN_LEN                   5
#define QSEE_ESE_AID_MAX_LEN                   16
#define QSEE_ESE_T1_MAX_BWT_MILLISECONDS       1624
#define QSEE_ESE_T1_POLL_TIMEOUT_MS            50
#define QSEE_ESE_T1_POLL_DELAY_SCALER          1
#define QSEE_ESE_T1_R_BLOCK_ERROR_MASK         0x83
#define QSEE_ESE_T1_R_BLOCK_ERROR_OTHER        0x82
#define QSEE_ESE_T1_R_BLOCK_ERROR_RC_OR_PARITY 0x81
#define QSEE_ESE_ARR_SZ(a)                     (sizeof((a)) / sizeof(0[(a)]))
#define QSEE_ESE_MALLOC                        qsee_zalloc
#define QSEE_ESE_REALLOC                       qsee_realloc
#define QSEE_ESE_FREE                          QSEE_FREE_PTR

#define QSEE_ESE_LOG_INF(xx_fmt, ...)          qsee_log(QSEE_LOG_MSG_DEBUG, " %s " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
#define QSEE_ESE_LOG_ERR(xx_fmt, ...)          qsee_log(QSEE_LOG_MSG_ERROR, " %s " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
#define QSEE_ESE_ENTER(xx_fmt, ...)            qsee_log(QSEE_LOG_MSG_DEBUG, "+%s " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
#define QSEE_ESE_EXIT(xx_fmt, ...)             qsee_log(QSEE_LOG_MSG_DEBUG, "-%s " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
// clang-format on

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*
 * Feature mask enabling/disabling several ESE Service functionality
 */
typedef enum
{
  QSEE_ESE_OEM_SEAC                = (1 << 0),
  QSEE_ESE_OEM_SEAC_APDU_FILTERING = (1 << 1),
  QSEE_ESE_OEM_TRANSMIT_SELECT_CMD = (1 << 2),
  QSEE_ESE_OEM_DEPRECATED_BIT_3    = (1 << 3),
  QSEE_ESE_OEM_DEPRECATED_BIT_4    = (1 << 4),
  QSEE_ESE_OEM_DENY_BASIC_CHANNEL  = (1 << 5),
  QSEE_ESE_OEM_DENY_DEFAULT_APPLET = (1 << 6),
  QSEE_ESE_OEM_DEPRECATED_BIT_7    = (1 << 7),
  QSEE_ESE_OEM_ESESERVICE_TA_LOGS  = (1 << 8),
  QSEE_ESE_OEM_FEATURE_MAX         = (1 << 30),
  QSEE_ESE_OEM_FEATURE_MASK        = 0x7FFFFFFF,
  QSEE_ESE_OEM_BWT_SHIFT           = 0,
  QSEE_ESE_OEM_POLL_TIMEOUT_SHIFT  = 12,
  QSEE_ESE_OEM_POLL_DELAY_SHIFT    = 20,
  QSEE_ESE_OEM_POLL_DELAY_MI_SHIFT = 24,
  QSEE_ESE_OEM_RESET_DELAY_SHIFT   = 28,
  QSEE_ESE_OEM_BWT_MASK            = (0x7FF << QSEE_ESE_OEM_BWT_SHIFT),
  QSEE_ESE_OEM_POLL_TIMEOUT_MASK   = (0xFF << QSEE_ESE_OEM_POLL_TIMEOUT_SHIFT),
  QSEE_ESE_OEM_POLL_DELAY_MASK     = (0xF << QSEE_ESE_OEM_POLL_DELAY_SHIFT),
  QSEE_ESE_OEM_POLL_DELAY_MI_MASK  = (0xF << QSEE_ESE_OEM_POLL_DELAY_MI_SHIFT),
  QSEE_ESE_OEM_RESET_DELAY_MASK    = (0xF << QSEE_ESE_OEM_RESET_DELAY_SHIFT),
} teOemEseMask;

/*
 * Unique identifier for OEM Property
 */
typedef enum
{
  QSEE_ESE_OEM_PROP_SPI_DEV_ID,
  QSEE_ESE_OEM_PROP_SPI_CS_ID,
  QSEE_ESE_OEM_PROP_SPI_MAX_FREQ,
  QSEE_ESE_OEM_PROP_SPI_BITS_PER_WORD,
  QSEE_ESE_OEM_PROP_SPI_NAD_CMD,
  QSEE_ESE_OEM_PROP_SPI_NAD_RSP,
  QSEE_ESE_OEM_PROP_FEATURE_MASK,
  QSEE_ESE_OEM_PROP_TIMERS,
  QSEE_ESE_OEM_PROP_POLL_TIMEOUT,
  QSEE_ESE_OEM_PROP_BWT,
  QSEE_ESE_OEM_PROP_LRC_OFFSET,
  QSEE_ESE_OEM_PROP_IFSC,
  QSEE_ESE_OEM_PROP_IFSD,
  QSEE_ESE_OEM_PROP_CMD_GETATR,
  QSEE_ESE_OEM_PROP_CMD_HARD_RESET,
  QSEE_ESE_OEM_PROP_CMD_INTERFACE_RESET,
  QSEE_ESE_OEM_PROP_CMD_END_PDU_SESSION,
  QSEE_ESE_OEM_PROP_CMD_IFS,
  QSEE_ESE_OEM_PROP_WTX_MAX_COUNT,
  QSEE_ESE_OEM_PROP_NAK_MAX_COUNT,
  QSEE_ESE_OEM_PROP_RETRANSMISSIONS_MAX_COUNT,
  QSEE_ESE_OEM_PROP_MAX = 0x7FFFFFFF,
} teOemEseProperty;

/**
 * Type of timeout variable
 */
typedef uint32_t qsee_ese_service_timeout_t;

/**
 * @brief C-APDU
 */
typedef struct
{
  uint32_t size_write;                     /**< number of bytes to write */
  uint8_t  buff[QSEE_ESE_T1_MAX_FRAME_SZ]; /**< address for write data */
} qsee_ese_service_capdu_t;

/**
 * @brief R-APDU
 */
typedef struct
{
  uint32_t size_read; /**< number of bytes read which is initialised to zero  */
  uint8_t  buff[QSEE_ESE_T1_MAX_FRAME_SZ]; /**< address for read data */
} qsee_ese_service_rapdu_t;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_ese
  @{
*/

/**
 * Checks if SEAC is enabled in QSEE.
 *
 * @param [in] pContext ESE pContext opaque pointer.
 *
 * @return True if enabled, otherwise disabled.
 */
bool qsee_ese_service_seac_enabled(void *pContext);

/**
 * Checks if SEAC APDU filtering is enabled in QSEE.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return True if enabled, otherwise disabled.
 */
bool qsee_ese_service_seac_apdu_filtering_enabled(void *pContext);

/**
 * Checks if the secure element GP API is allowed to send the SELECT command using
 * the TEE_SEChannelTransmit API. 
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return True if enabled, otherwise disabled.
 */
bool qsee_ese_service_transmit_select_cmd_enabled(void *pContext);

/**
 * Checks if the ESE Service can send a SPI soft reset to the ESE. This is to
 * reset SPI T=1 protocol parameters, SPI interface, and JCOP context for the
 * SPI interface.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return True if enabled, otherwise disabled.
 */
bool qsee_ese_service_spi_soft_reset_enabled(void *pContext);

/**
 * Checks if ESE Service can send end of APDU session message to indicate the
 * end of an active APDU session over SPI interface. JCOP may put the chip to
 * low power mode if no APDU session is active over all the supported interfaces
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return true if enabled, otherwise disable
 */
bool qsee_ese_service_spi_end_apdu_session_enabled(void *pContext);

/**
 * Checks if the TEE SE API library is configured to block access to the basic channel. 
 
 * Access to the basic channel is allowed by default. It must be explicitly blocked
 * via devcfg oem_config.xml
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return True if blocked, otherwise allowed.
 */
bool qsee_ese_service_block_access_basic_channel(void *pContext);

/**
 * Checks if the TEE SE API library is configured to block access to default applet
 * selection if no AID is specified.
 *
 * Access to the default applet is allowed by default. It must be explicitly blocked
 * via devcfg oem_config.xml.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return True if blocked, otherwise allowed.
 */
bool qsee_ese_service_block_access_default_applet(void *pContext);

/**
 * Gets the feature mask that enables and disables several ESE service functionalities.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return A value of the ESE feature mask from the OEM configuration file.
 */
uint32_t qsee_ese_service_feature_mask(void *pContext);

/**
 * Gets the timer mask for the ESE timers.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return A value of the ESE timer mask from the OEM configuration file.
 *
 */
uint32_t qsee_ese_service_get_timer(void *pContext);

/**
 * Gets the secure element maximum block waiting (BWT).
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return A BWT value.
 *
 */
uint32_t qsee_ese_service_get_bwt(void *pContext);

/**
 * Gets the SPI polling time for the secure element response message.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return The polling timer value.
 *
 */
uint32_t qsee_ese_service_get_poll_timeout(void *pContext);

/**
 * Gets the T=1 NAD used for the C-APDU that is sent to the secure element.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return The NAD byte used in commands sent to the secure element.
 */
uint8_t qsee_ese_service_get_cmd_nad(void *pContext);

/**
 * Gets the T=1 NAD used for the R-APDU that is expected from the secure element.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return The NAD byte used in response from secure element.
 */
uint8_t qsee_ese_service_get_rsp_nad(void *pContext);

/**
 * Retrieves the SPI device identifier used by the secure element from devcfg.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return A valid SPI identifier.
 */
qsee_spi_device_id_t qsee_ese_service_oem_get_spi_id(void *pContext);

/**
 * Retrieves the chip select identifier used by secure element from devcfg.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return A valid value for the chip select identifier used by secure element.
 */
uint8_t qsee_ese_service_oem_get_chip_select_id(void *pContext);

/**
 * Retrieves the maximum SPI frequency used by the secure element from devcfg.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return A valid frequency; defaults to 9600000 if it cannot be retrieved from devcfg.
 */
uint32_t qsee_ese_service_oem_get_spi_max_frequency(void *pContext);

/**
 * Retrieves the SPI number of bits per work used by secure element from devcfg.
 *
 * @param[in] pContext ESE pContext opaque pointer.
 *
 * @return A valid number of SPI bits per work; defaults to 8 if it cannot retrieve this number from devcfg.
 */
uint8_t qsee_ese_service_oem_get_spi_bits_per_word(void *pContext);

/**
 * Waits for the specified number of milliseconds.
 *
 * @param[in]  milliseconds: Number of milliseconds to wait.
 *
 * @return Object_OK on success, otherwise failure.
 */
uint32_t qsee_ese_service_wait(qsee_ese_service_timeout_t milliseconds);

/**
 * Opens a logical connection using the handle to communicate over SPI.
 *
 * @param[in,out] ppContext ESE pContext opaque pointer.
 *
 * @return Object_OK on success, otherwise failure.
 *
 * @see qsee_ese_service_close
 *
 */
int qsee_ese_service_open(void **ppContext);

/**
 * Closes a logical connection using the handle.
 *
 * @param[in,out] pContext ESE pContext opaque pointer.
 *
 * @return QSEE_ESE_SERVICE_STATUS_SUCCESS on success, otherwise failure.
 *
 * @see qsee_ese_service_open
 *
 */
int qsee_ese_service_close(void *pContext);

/**
 * Performs secure element access control (SEAC) based on the application identifier (AID) and/or
 * application protocol data unit (APDU).
 *
 * @param[in]     pContext  ESE pContext opaque pointer.
 * @param[in]     aid       Applet Application Identifier
 *                          NULL means AID filtering should not be applied.
 * @param[in]     aid_size  AID length (in bytes).
 * @param[in]     apdu      APDU in which SEAC filtering should be applied;
 *                          NULL means no APDU filtering should be applied.
 * @param[in]     apdu_size APDU length in bytes.
 *
 * @return Object_OK on success, otherwise access denied.
 */
int qsee_ese_service_seac(void *      pContext,
                          const void *aid,
                          size_t      aid_size,
                          const void *apdu,
                          size_t      apdu_size);

/**
 * Writes to slave data on the logical connection handle.
 *
 * @param[in]     pContext  ESE pContext opaque pointer.
 * @param[in]     c_apdu    Writes buffer information.
 *
 * @return Object_OK on success, otherwise failure.
 */
int qsee_ese_service_write(void *pContext, qsee_ese_service_capdu_t *c_apdu);

/**
 * Reads from slave data on the logical connection handle.
 *
 * @param[in]      pContext       ESE pContext opaque pointer.
 * @param[in,out]  r_apdu         Reads buffer information.
 *
 * @return Object_OK on success, otherwise failure.
 *
 */
int qsee_ese_service_read(void *pContext, qsee_ese_service_rapdu_t *r_apdu);

/**
 * Checks if T=1 Protocol Control Byte has error indications.
 *
 * @param[in]      pcb            Protocol control byte.
 *
 * @return True if error bits are set, otherwise false.
 */
bool qsee_ese_service_rsp_pcb_error(uint8_t pcb);

/** @} */

#endif // QSEE_ESE_SERVICE_H
