#ifndef QSEE_I2C_H
#define QSEE_I2C_H

/**
@file qsee_i2c.h
@brief Provide I2C API wrappers
*/

/*===========================================================================
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
09/20/17   unr      Added new device ids
03/12/13   cb       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
/** @cond */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* Definition of Default wait time */
#define QSEE_I2C_DEFAULT_WAIT_TIME -1

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/** @brief Devices supported by the interface. These device IDs map on to the
 *         DALDEVICEID_I2CPD_DEVICE_x IDs */
typedef enum
{
  QSEE_I2CPD_DEVICE_1,
  QSEE_I2CPD_DEVICE_2,
  QSEE_I2CPD_DEVICE_3,
  QSEE_I2CPD_DEVICE_4,
  QSEE_I2CPD_DEVICE_5,
  QSEE_I2CPD_DEVICE_6,
  QSEE_I2CPD_DEVICE_7,
  QSEE_I2CPD_DEVICE_8,
  QSEE_I2CPD_DEVICE_9,
  QSEE_I2CPD_DEVICE_10,
  QSEE_I2CPD_DEVICE_11,
  QSEE_I2CPD_DEVICE_12,
  QSEE_I2CPD_DEVICE_13,
  QSEE_I2CPD_DEVICE_14,
  QSEE_I2CPD_DEVICE_15,
  QSEE_I2CPD_DEVICE_16,
  QSEE_I2CPD_DEVICE_17,
  QSEE_I2CPD_DEVICE_18,
  QSEE_I2CPD_DEVICE_19,
  QSEE_I2CPD_DEVICE_20,
  QSEE_I2CPD_DEVICE_LIMIT
  /**< Used to check the bounds of this enumeration. */

} qsee_i2cpd_device_id_t;

/** @brief Interfaces supported by the I2C driver. */
typedef enum
{
  QSEE_I2C_STANDARD_INTERFACE,
  /**< Standard set of I/O pins is used. */

  QSEE_I2C_AUXILIARY_INTERFACE,
  /**< Alternate auxiliary set of I/O pins is used; not all targets support #
       the auxiliary interface. */

  QSEE_I2C_INTERFACE_COUNT,
  /**< Used to check the bounds of this enumeration. */

  /* Force enum to 32 bit */
  PLACEHOLDER_qsee_i2c_interface = 0x7fffffff

} qsee_i2c_interface_t;

/** @brief Slave address mode to differentiate between 7-bit and 10-bit
           addressing. */
typedef enum
{
  QSEE_I2C_7_BIT_SLAVE_ADDRESS,
  /**< Slave is a 7-bit address device. */

  QSEE_I2C_10_BIT_SLAVE_ADDRESS,
  /**< Slave is a 10-bit address device; currently, is not supported. */

  QSEE_I2C_SLAVE_ADDR_OPTIONS_COUNT,
  /**< Used to check the bounds of this enumeration. */

  /* Force enum to 32 bit */
  PLACEHOLDER_qsee_i2c_slaveaddr_options = 0x7fffffff

} qsee_i2c_slaveaddr_options_t;

/** @brief Slave device type. */
typedef enum
{
  QSEE_I2C_DEFAULT_ADDRESS_DEVICE,
   /**< No memory or register address is written to the slave device before
        any reads or writes from/to the slave device. Message format is:
   @verbatim
   <SlaveAddr><R/W bit><ACK/NACK><R/W data>
   @endverbatim
   */

  QSEE_I2C_MEMORY_ADDRESS_DEVICE,
  /**< Two bytes of address are written to the slave device before starting
       any reads or writes from/to the slave device. Message format is:
  @verbatim
  Write: <SlaveAddr><W bit><ACK/NACK><2 Byte Mem Addr><ACK/NACK>
         <W data>
  Read:  <SlaveAddr><W bit><ACK/NACK><2 Byte Mem Addr><ACK/NACK>
         <SlaveAddr><R bit><ACK/NACK><R data>
  @endverbatim
  */

  QSEE_I2C_REGISTER_ADDRESS_DEVICE,
  /**< One byte of address is written to the slave device before starting
       any reads or writes from/to the slave device. Message format is:
  @verbatim
  Write: <SlaveAddr><W bit><ACK/NACK><1 Byte Mem Addr><ACK/NACK>
         <W data>
  Read:  <SlaveAddr><W bit><ACK/NACK><1 Byte Mem Addr><ACK/NACK>
         <SlaveAddr><R bit><ACK/NACK><R data>
  @endverbatim
  */

  QSEE_I2C_SLAVE_DEVICE_COUNT,
  /**< Used to check the bounds of this enumeration. */

  /* Force enum to 32 bit */
  PLACEHOLDER_qsee_i2c_slave_device = 0x7fffffff

} qsee_i2c_slave_device_t;

/** @brief Slave device's read option type. This is used only if the slave
           device is not an I2C_DEFAULT_ADDRESS_DEVICE type. */
typedef enum
{
  QSEE_I2C_READ_OPTION_DEFAULT,
  /**< Default option where only a START condition is generated
       between writing the address bytes and reading from the slave
       device. */

  QSEE_I2C_GEN_START_BEFORE_READ,
  /**< Only a START condition is generated between writing the address
       bytes and reading from the slave device. */

  QSEE_I2C_GEN_STOP_START_BEFORE_READ,
  /**< STOP and START condition is generated between writing the
       address bytes and reading from the slave device. */

  QSEE_I2C_READ_OPTIONS_COUNT,
  /**< Used to check the bounds of this enumeration. */

  /* Force enum to 32 bit */
  PLACEHOLDER_qsee_i2c_read_options = 0x7fffffff

} qsee_i2c_read_options_t;

/** @brief Used for bus configuration settings. */
typedef struct
{
  uint32_t                        fs_bus_freq_in_khz;
  /**< Bus frequency in Fast-Speed mode. */

  uint32_t                        hs_bus_freq_in_khz;
  /**< Bus frequency in High-Speed mode; currently is not supported. */

  qsee_i2c_interface_t            i2c_interface;
  /**< Interface to use. */

  int32_t                         max_bus_acquire_wait_time_msec;
  /**< Bus acquisition timeout time in microseconds. */

} qsee_i2c_bus_config_t;

/** @brief Used for slave device configuration settings. */
typedef struct
{
  uint32_t                        slave_address;
  /**< I2C slave device address. */

  qsee_i2c_slaveaddr_options_t    slave_address_option;
  /**< I2C slave device address options. */

  qsee_i2c_slave_device_t         slave_device_type;
  /**< I2C slave device type. */

  qsee_i2c_read_options_t         read_option;
  /**< I2C slave device read option. */

  /* Set to I2C_DEFAULT_WAIT_TIME for default value */
  int32_t                         byte_transfer_wait_time_usec;
  /**< Byte transfer timeout time in microseconds. Clock stretching for
       a slower device can be achieved by increasing this timeout value.
       Use the I2C_DEFAULT_WAIT_TIME constant for the default timeout time
       preset by the driver. */

} qsee_i2c_slave_device_config_t;

/** @brief Used for configuration settings. */
typedef struct
{
  /* set this to NULL, if want to use default Bus Config */
  qsee_i2c_bus_config_t          *p_bus_config;
  /**< Pointer to the bus configuration; set this pointer to NULL
       if the default bus configuration is preferred. */

  qsee_i2c_slave_device_config_t *p_slave_config;
  /**< Pointer to the slave device configuration. */

} qsee_i2c_config_t;

/** @brief Structure that is used for the transaction information type. */
typedef struct
{
  uint32_t                        start_addr;
  /**< Memory/register address to be written to the slave device
       before the slave device starts transmitting/receiving bytes.
       If the slave device is a I2C_DEFAULT_ADDRESS_DEVICE type, this address
       is ignored and the I2C controller directly starts reading/writing
       from/to the slave device without any address byte writes. */

  unsigned char*                  p_buf;
  /**< Read/write buffer. */

  uint32_t                        buf_len;
  /**< Number of bytes to read/write. */

  uint32_t                        total_bytes;
  /**< Number of bytes actually read/written. */

} qsee_i2c_transaction_info_t;
/** @endcond */


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_i2c
  @{
*/

/**
 * Transfers access to the I2C bus to the calling application.
 *
 * This function returns success only if the following operations are successful:
 *
 *   -# Obtain a handle to the requested device, then open device.
 *   -# Lock I2C bus for exclusive access.
 *   -# Register the I2C interrupt in QTEE, to transfer
 *      the interrupt from REE to TEE.
 *   -# Disable the I2C interrupt.
 *   -# Protect the control block for the device.
 *   -# Protect the QUP block for the device.
 *
 * @param[in] device_id I2C device ID to open.
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- Negative
 */
int qsee_i2c_open(qsee_i2cpd_device_id_t device_id);

/**
 * Reads data from a slave device.
 *
 * <b>Prerequisite:</b> This function requires a successful call to qsee_i2c_open().
 *
 * The read aborts if the slave device does not acknowledge control/address
 * bytes written to it (before the data read starts). A read operation always
 * terminates with a STOP condition.
 *
 * An error is generated if the bus is in an inconsistent state,
 * i.e., uninitialized or busy.
 *
 * The following diagram shows the bus activity during a read. The second
 * START shown is to be interpreted as a repeated START and may be replaced
 * by a STOP and START by some protocols.

   @verbatim
               |<------optional------>|
   ------------------------------------------------------------
   |S|       | |        | |S|       | |                   |N|S|
   |T|Control|A|Address |A|T|Control|A|      Data         |O|T|
   |A| Byte  |C|[Offset |C|A| Byte  |C|                   |A|O|
   |R|       |K| or Reg]|K|R|       |K|                   |C|P|
   |T|       | |        | |T|       | |                   |K| |
   ------------------------------------------------------------
   @endverbatim
   @vertspace{12}

 * The address may be 0, 1, or 2 bytes depending on the slave. Every address
 * byte written to the slave device must be acknowledged.
 *
 * This function is blocking. It will return when either data
 * has been read, or an error has occurred.
 *
 * @param[in] device_id       I2C device ID being read from.
 * @param[in] p_config        Pointer to bus and slave configuration
 *                            for this read transaction.
 * @param[in,out] p_read_info Pointer to read information, i.e.
 *                            buffer, length, etc.
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- Negative
 */
int qsee_i2c_read(
  qsee_i2cpd_device_id_t        device_id,
  const qsee_i2c_config_t*      p_config,
  qsee_i2c_transaction_info_t*  p_read_info
);

/**
 * Writes data to a slave device.
 *
 * <b>Prerequisite:</b> This function requires a successful call to qsee_i2c_open().
 *
 * The write aborts if the slave device does not acknowledge
 * control/address/data bytes written to it. The write operation always
 * terminates with a STOP condition.
 *
 * If there is a write attempt with a count of zero, the slave device is selected
 * and returns success if the slave device acknowledges it. Otherwise, a
 * failure is returned. This is useful if you want to test if a slave device
 * is addressable, as in NVRAM devices that may be busy when performing
 * internal cache writes.
 *
 * The following diagram shows the bus activity during a write operation.

   @verbatim
               |<--opt.-->|
   ------------------------------------------------
   |S|       | |        | |                   | |S|
   |T|Control|A|Address |A|      Data         |A|T|
   |A| Byte  |C|[Offset |C|                   |C|O|
   |R|       |K| or Reg]|K|                   |K|P|
   |T|       | |        | |                   | | |
   ------------------------------------------------
   @endverbatim
   @vertspace{12}

 * The address may be 0, 1, or 2 bytes depending on the slave. Every address
 * byte written to the slave device must be acknowledged.
 *
 * @param[in] device_id        I2C device ID being written to.
 * @param[in] p_config         Pointer to bus and slave configuration
 *                             for this write transaction.
 * @param[in,out] p_write_info Pointer to write information, i.e.
 *                             buffer, length, etc.
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- Negative
 */
int qsee_i2c_write(
  qsee_i2cpd_device_id_t       device_id,
  const qsee_i2c_config_t*     p_config,
  qsee_i2c_transaction_info_t* p_write_info
);

/**
 * Transfers access of I2C QUP back to REE.
 *
 * <b>Prerequisite:</b> This function requires a successful call to qsee_i2c_open().
 *
 * This function returns success only if the following operations are succesful:
 *
 *   -# Remove exclusive access lock to the I2C bus, then close device.
 *   -# Remove QUP block protection from GSBI3.
 *   -# Remove control block protection for GSBI3.
 *   -# Reenable the I2C interrupt.
 *   -# Deregister the I2C interrupt. The I2C interrupt transfers back to REE.
 *
 * @param[in] device_id I2C Device ID to close.
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- Negative
 */
int qsee_i2c_close(qsee_i2cpd_device_id_t device_id);

/** @} */

#endif /*QSEE_I2C_H*/

