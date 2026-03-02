#ifndef QSEE_MESSAGE_H
#define QSEE_MESSAGE_H

/**
@file qsee_message.h
@brief Provide APIs for sending messages between secure applications.
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
03/02/13   nbeier      Initial version.

===========================================================================*/
#include <stdint.h>

/*---------------------------------------------------------------------------
 *  Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**These functions are for a trusted application to prepare a message that
   can be sent to another trusted application.
  @addtogroup qtee_message
  @{
*/

/**
  Prepares a message for another trusted application.

  This function writes the AES128-CTR encrypted message in to the supplied
  output buffer, prepended with a header and appended with HMAC-SHA256.
  This output buffer can be given to the receiving application and then passed
  into qsee_decapsulate_inter_app_message() for authentication and decryption.
  The actual data exchange (passing the encrypted buffer)
  between the trusted applications can be done between their
  clients running in the REE.

  @param[in] dest_app_name Pointer to destination application name (maximum length 128 bytes).
  @param[in] in_buf        Pointer to an input buffer containing the message.
  @param[in] in_len        Exact input buffer length.
  @param[out] out_buf      Pointer to output buffer.
  @param[in, out] out_len  Exact output buffer length (must be greater than in_len by 144
                           bytes to allow room for header and MAC).\n
                           out_len modified to reflect the exact length of the data written
                           into out_buf.

  @return SUCCESS -- 0\n
          All other values indicate failure and correspond to a specific error code.
*/
int qsee_encapsulate_inter_app_message(char* dest_app_name, uint8_t* in_buf,
  uint32_t in_len, uint8_t* out_buf, uint32_t* out_len);

/**
  Decapsulates a message sent to this application.

  This function authenticates the message, decrypts the input buffer, and
  writes the plaintext message into the supplied output buffer. The input
  buffer must be prepared by qsee_encapsulate_inter_app_message(), containing
  a header and MAC.

  @param[out] source_app_name Pointer to the sending application identity (maximum length is 128 bytes).
  @param[in] in_buf           Pointer to input buffer.
  @param[in] in_len           Exact size of in_buf.
  @param[out] out_buf         Pointer to output buffer.
  @param[in, out] out_len     Pointer to exact size of output buffer (must be
                              greater than or equal to in_len).

  @return SUCCESS -- 0\n
          All other values indicate failure and correspond to a specific error code.
*/
int qsee_decapsulate_inter_app_message(char* source_app_name, uint8_t* in_buf,
  uint32_t in_len, uint8_t* out_buf, uint32_t* out_len);


/** @} */

/** @cond */
#define QSEE_MESSAGE_APP_NAME_MAX_LEN 128 /**< Maximum application name length within the messaging API. */


/*----------------------------------------------------------------------------
 * Error codes
 * -------------------------------------------------------------------------*/
#define QSEE_MESSAGE_SUCCESS                     0                    //*< Success. */
#define QSEE_MESSAGE_ERROR                       0xFF000FFF           //*< Generic Error. */
#define QSEE_MESSAGE_ERROR_ZERO_BUFFER_LEN       0xFF000FFE           //*< Zero buffer length. */
#define QSEE_MESSAGE_ERROR_INSUFFCIENT_OUT_LEN   0xFF000FFD           //*< Insufficient output buffer length. */
#define QSEE_MESSAGE_ERROR_BAD_DEST_APPNAME      0xFF000FFC           //*< Deprecated. */
#define QSEE_MESSAGE_ERROR_NO_DEST_APP_THREAD    0xFF000FFB           //*< Deprecated. */
#define QSEE_MESSAGE_ERROR_NO_DEST_APP_KEYS      0xFF000FFA           //*< No destination application keys supplied. */
#define QSEE_MESSAGE_ERROR_NO_SPACE_IN_TABLE     0xFF000FF9           //*< No space in message table. */
#define QSEE_MESSAGE_ERROR_CIPHER_INIT           0xFF000FF8           //*< Error during initialization of the cipher. */
#define QSEE_MESSAGE_ERROR_CIPHER_SET_PARAM      0xFF000FF7           //*< Error during parameter setting for the cipher context. */
#define QSEE_MESSAGE_ERROR_CIPHER_ENCRYPT        0xFF000FF6           //*< Error during encryption. */
#define QSEE_MESSAGE_ERROR_HMAC_FAIL             0xFF000FF5           //*< HMAC (Hash Message Authentication Code) failed. */
#define QSEE_MESSAGE_ERROR_PRNG_FAIL             0xFF000FF4           //*< PRNG (Pseudo Random Number Generator) failed. */
#define QSEE_MESSAGE_ERROR_BAD_SOURCE_NAME       0xFF000FF3           //*< Deprecated. */
#define QSEE_MESSAGE_ERROR_MESSAGE_NOT_FOR_YOU   0xFF000FF2           //*< Message not meant for this application. */
#define QSEE_MESSAGE_ERROR_NO_COUNTER_VALS       0xFF000FF1           //*< No counter values for this application found. */
#define QSEE_MESSAGE_ERROR_MAC_MISMATCH          0xFF000FF0           //*< MAC (Message Authentication Code) mismatch. */
#define QSEE_MESSAGE_ERROR_BAD_COUNTER_VALS      0xFF000FEF           //*< Counter value verification failed. */
#define QSEE_MESSAGE_ERROR_MAXED_OUT_COUNTER_VAL 0xFF000FEE           //*< Counter value would overflow. */
#define QSEE_MESSAGE_ERROR_MALLOC_INTERNAL_BUF   0xFF000FED           //*< Couldn't allocate internal buffer. */
#define QSEE_MESSAGE_ERROR_INTEGER_OVERFLOW      0xFF000FEC           //*< Supplied buffer size would create an overflow. */
#define QSEE_MESSAGE_ERROR_INTEGER_UNDERFLOW     0xFF000FEB           //*< Supplied buffer size would create an underflow. */
#define QSEE_MESSAGE_ERROR_MALLOC_HMAC_BUF       0xFF000FEA           //*< Couldn't allocate buffer for HMAC (Hash Message Authentication Code). */
#define QSEE_MESSAGE_ERROR_STRLCPY_FAIL          0xFF000FE9           //*< Deprecated. */
#define QSEE_MESSAGE_ERROR_CIPHER_SET_PARAM_KEY  0xFF000FE8           //*< Setting cipher key failed. */
#define QSEE_MESSAGE_ERROR_CIPHER_SET_PARAM_MODE 0xFF000FE7           //*< Setting cipher parameters failed. */
#define QSEE_MESSAGE_ERROR_CIPHER_SET_PARAM_IV   0xFF000FE6           //*< Setting cipher input vector failed. */
#define QSEE_MESSAGE_ERROR_SRC_NOT_LEGACY        0xFF000FE5           //*< Deprecated. */
#define QSEE_MESSAGE_ERROR_DST_NOT_LEGACY        0xFF000FE4           //*< Deprecated. */
#define QSEE_MESSAGE_ERROR_TZ_HEAP_MALLOC        0xFF000FE3           //*< Error due to tz heap malloc failure */
#define QSEE_MESSAGE_ERROR_APP_REGION_MALLOC     0xFF000FE2           //*< Error due to ta app region malloc failure */
#define QSEE_MESSAGE_SUCESS                      QSEE_MESSAGE_SUCCESS //*< Success. */
/** @endcond */

#endif // QSEE_MESSAGE_H
