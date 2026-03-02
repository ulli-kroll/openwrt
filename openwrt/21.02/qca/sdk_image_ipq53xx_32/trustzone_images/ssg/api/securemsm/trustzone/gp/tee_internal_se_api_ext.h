#ifndef TEE_INTERNAL_SE_API_EXT_H
#define TEE_INTERNAL_SE_API_EXT_H
/*
 * Contains QCOM Specific extensions to the GP TEE SE APIs. 
 */

#include <stdint.h>
#include <stdbool.h>
#include "tee_internal_api.h"
#include "tee_internal_se_api.h"

#define QC_TEE_SE_WTX 0x00000006 /* QCOM: Waiting for R-APDU after sending WTX Response */

/**
 *
 * The TEE_SEChannelTransmit_ext function transmits an C-APDU to the secure
 * element and receives generated R-APDU from the secure. The API performs
 * similar function to TEE_SEChannelTransmit with the added capability of
 * allowing SELECT by DF Name command to be successfully transmitted which is
 * not allowed by the standard API.
 *
 * @param[in]  seChannelHandle: Reference to seChannel hand
 * @param[in]  command        : Input Buffer containing the command to send as a byte array
 * @param[in]  commandLen     : Size for the command buffer which needs to be big enough to
 *                              allow secure messaging wrapping inside the provided buffer
 * @param[in,out] response    : Output buffer containing the response as a byte array
 * @param[in,out] responseLen : Size of the response buffer which needs to be big enough to
 *                              allow secure messaging unwrapping inside the provided buffer
 * @param[in]  allowSelectByDF: Allow sending SELECT by DF name
 *
 * @return   TEE_SUCCESS      : In case operation is successful. Otherwise @see TEE_SEChannelTransmit
 *
 **/
TEE_Result TEE_SEChannelTransmit_ext(TEE_SEChannelHandle seChannelHandle,
                                     void *              command,
                                     uint32_t            commandLen,
                                     void *              response,
                                     uint32_t *          responseLen,
                                     bool                allowSelectByDF);

/**
 *
 * Get the current number of open sessions
 *
 * @param    [in]  seReaderHandle      : Reference to seReader handle
 * @param    [out] numOfSessions       : Reference to write the number of sessions of success
 *
 * @return   TEE_SUCCESS               : In case operation is successful.
 * @return   TEE_ERROR_BAD_STATE       : If the seSession is closed
 * @return   TEE_ERROR_BAD_PARAMETERS  : If invalid input channel information
 **/
TEE_Result TEE_SEReaderGetSessionCount_ext(TEE_SEReaderHandle seReaderHandle,
                                           uint32_t *         numOfSessions);
/*
 * Returns with status word resulting from the message exchange with Secure Element
 *
 * @param   [in]     seReaderHandle : Reference to Reader handle
 * @param   [out]    response       : Response buffer pointer to return SELECT response.
 * @param   [in,out] responseLen    : Select response buffer length
 *
 * @return  TEE_SUCCESS             : In case of success.
 * @return  TEE_ERROR_COMMUNICATION : If the SE is not present or if there is an I/O error
 * @return  TEE_ERROR_BAD_STATE     : If the seSession is closed
 */
TEE_Result TEE_SEReaderGetStatus_ext(TEE_SEReaderHandle seReaderHandle,
                                     void *             response,
                                     uint32_t *         responseLen);

/**
 *
 * Function returns the channel number given the channel handle
 *
 * @param    [in] seChannelHandle : Reference to seChannel handle
 * @param    [out] channelNumber  : pointer to store the channel number
 *
 * @return   TEE_SUCCESS               : In case operation is successful.
 * @return   TEE_ERROR_BAD_STATE       : If the seSession for the channel handle is closed
 * @return   TEE_ERROR_BAD_PARAMETERS  : If invalid input channel information
 **/
TEE_Result TEE_SEChannelGetNumber_ext(TEE_SEChannelHandle seChannelHandle, uint32_t *channelNumber);

/**
 *
 * Function returns the session handle to the particular channel handle
 *
 * @param    [in] seChannelHandle  : Reference to seChannel handle
 * @param    [out] seSessionHandle : reference to write the session handle into
 *
 * @return   TEE_SUCCESS               : In case operation is successful.
 * @return   TEE_ERROR_BAD_STATE       : If the seSession for the channel handle is closed
 * @return   TEE_ERROR_BAD_PARAMETERS  : If invalid input channel information
 **/
TEE_Result TEE_SEChannelGetSession_ext(TEE_SEChannelHandle  seChannelHandle,
                                       TEE_SESessionHandle *seSessionHandle);

/**
 *
 * Function secures an open channel by attaching to a shareable SCP session
 *
 * @param    [in] seChannelHandle  : Reference to seChannel handle
 * @param    [in] scType           : One of the TEE_SC_TYPE_* values
 * @param    [in] securityLevel    : One of the TEE_SC_* values
 *
 * @return   TEE_SUCCESS               : If operation is successful
 * @return   TEE_ERROR_BAD_STATE       : If the seSession is closed
 * @return   TEE_ERROR_ACCESS_CONFLICT : If the channel handle has already been secured
 * @return   TEE_ERROR_BAD_PARAMETERS  : If the SCP type does not allow shared security
 * @return   TEE_ERROR_NOT_SUPPORTED   : If scType was not SCP11a or SCP11a
 * @return   TEE_ERROR_OUT_OF_MEMORY   : If insufficient memory was available
 * @return   TEE_ERROR_ITEM_NOT_FOUND  : If the Crypto TA could not be located
 * @return   TEE_ERROR_ACCESS_DENIED   : If the Crypto TA refused access
 * @return   TEE_ERROR_TARGET_DEAD     : If the Crypto TA panicked
 * @return   TEE_ERROR_GENERIC         : If the operation failed for other reasons
 **/
TEE_Result TEE_SESecureChannelAttachShared_ext(TEE_SEChannelHandle  seChannelHandle,
                                               uint8_t              scType,
                                               TEE_SC_SecurityLevel scSecurityLevel);

/**
 *
 * Function prefetches a eSE certificate and saves it for use
 * during SCP session establishment.
 *
 * @param    [in] seChannelHandle  : Open channel handle to Security Domain
 * @param    [in] aidTarget        : The target AID
 * @param    [in] scType           : One of the TEE_SC_* values
 * @param    [in] keyVersion       : Key version Number
 * @param    [in] pPublicKey       : Public key (if applicable)
 * @param    [in] publicKeyLength  : Public key size
 *
 * @return   TEE_SUCCESS               : If operation is successful
 * @return   TEE_ERROR_BAD_STATE       : If the seSession is closed
 * @return   TEE_ERROR_ACCESS_CONFLICT : If the channel handle has been secured
 * @return   TEE_ERROR_BAD_PARAMETERS  : If invalid input channel information
 * @return   TEE_ERROR_NOT_SUPPORTED   : If scType was not SCP11a or SCP11a
 * @return   TEE_ERROR_OUT_OF_MEMORY   : If insufficient memory was available
 * @return   TEE_ERROR_ITEM_NOT_FOUND  : If the Crypto TA could not be located
 * @return   TEE_ERROR_ACCESS_DENIED   : If the Crypto TA refused access
 * @return   TEE_ERROR_TARGET_DEAD     : If the Crypto TA panicked
 * @return   TEE_ERROR_GENERIC         : If the operation failed for other reasons
 **/
TEE_Result TEE_SESecureChannelGetSdCertificate_ext(TEE_SEChannelHandle seChannelHandle,
                                                   const TEE_SEAID *   pAIDTarget,
                                                   uint8_t             scType,
                                                   uint8_t             keyVersion,
                                                   const uint8_t *     pPublicKey,
                                                   size_t              publicKeySize);
/**
 *
 * Function sends an S-Block to perform an SPI Protocol reset on the Secure Element.
 * Secure Element responds with an ATR.
 *
 * @param    [in]  seSessionHandle    : Reference to the active SE session
 * @param    [out] response           : Output buffer containing the response as a byte array
 * @param    [in,out] responseLen     : Size of the output response buffer

 * @return   TEE_SUCCESS              : If operation is successful
 * @return   TEE_ERROR_BAD_STATE      : If the seSession is closed/invalid
 * @return   TEE_ERROR_COMMUNICATION  : If the Response is not available
 * @return   TEE_ERROR_SHORT_BUFFER   : If response buffer is too small to hold ATR
 * @return   TEE_ERROR_TARGET_DEAD    : If bad/invalid parameters
 **/
TEE_Result TEE_SESessionInterfaceReset_ext(TEE_SESessionHandle seSessionHandle,
                                           void *              response,
                                           uint32_t *          responseLen);

/**
 *
 * Function sends an S-Block to perform a warm boot of the Secure Element
 *
 * @param    [in]  seSessionHandle    : Reference to the active SE session
 * @param    [out] response           : Output buffer containing the response as a byte array
 * @param    [in,out] responseLen     : Size of the output response buffer

 * @return   TEE_SUCCESS              : If operation is successful
 * @return   TEE_ERROR_BAD_STATE      : If the seSession is closed/invalid
 * @return   TEE_ERROR_COMMUNICATION  : If the Response is not available
 * @return   TEE_ERROR_SHORT_BUFFER   : If response buffer is too small to hold Response
 * @return   TEE_ERROR_TARGET_DEAD    : If bad/invalid parameters
 **/
TEE_Result TEE_SESessionHardReset_ext(TEE_SESessionHandle seSessionHandle,
                                      void *              response,
                                      uint32_t *          responseLen);

/**
 *
 * Function sends an S-Block to update the Secure Element with the IFSD.
 * IFSC cannot be changed
 *
 * @param    [in]  seSessionHandle    : Reference to the active SE session
 * @param    [out] response           : Output buffer containing the response as a byte array
 * @param    [in,out] responseLen     : Size of the output response buffer

 * @return   TEE_SUCCESS              : If operation is successful
 * @return   TEE_ERROR_BAD_STATE      : If the seSession is closed/invalid
 * @return   TEE_ERROR_COMMUNICATION  : If the IFS Response is not available
 * @return   TEE_ERROR_SHORT_BUFFER   : If response buffer is too small to hold IFS Response
 * @return   TEE_ERROR_TARGET_DEAD    : If bad/invalid parameters
 **/
TEE_Result TEE_SESessionIfsReq_ext(TEE_SESessionHandle seSessionHandle,
                                   void *              response,
                                   uint32_t *          responseLen);

#endif /* ifndef TEE_INTERNAL_SE_API_EXT_H */
