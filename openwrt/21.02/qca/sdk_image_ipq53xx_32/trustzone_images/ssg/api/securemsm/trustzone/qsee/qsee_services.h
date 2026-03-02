#ifndef QSEE_SERVICES_H
#define QSEE_SERVICES_H

#include <stdint.h>

/**
@file qsee_services.h
@brief Provide API wrappers for services
*/

/*============================================================================
  Copyright (c) 2012, 2017, 2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  =========================================================================== */

#define QSEE_SERVICE_ERROR_BUFFER_TOO_LARGE (-2)

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_services
  @{
*/

/**
  Requests a service from a REE listener.

  The requested REE listener must be available and running. The supplied buffer
  lengths must match the requested REE listener instance.

  @param[in] listener_id Requested listener ID.
  @param[in] req         Pointer to request buffer.
  @param[in] req_len     Request buffer length.
  @param[out] rsp        Pointer to response buffer.
  @param[in] rsp_len     Response buffer length.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
  QSEE_SERVICE_ERROR_BUFFER_TOO_LARGE -- passed request/responses are too large
*/
int qsee_request_service(unsigned int listener_id, void *req, unsigned int req_len, void *rsp, unsigned int rsp_len);


/**
  Registers a shared buffer with QTEE.

  @param[in] address   Pointer to shared buffer.
  @param[in] size      Shared buffer size.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_register_shared_buffer(void *address, unsigned int size);


/**
  Deregisters the shared buffer previously registered with QTEE.

  @param[in] address   Pointer to shared buffer.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_deregister_shared_buffer(void *address);


/**
  Prepares a shared buffer before sending it across to the REE.

  @note1hang Flushes cachelines for shared memory in anticipation of
  non-secure access.

  @param[in] address    Pointer to shared buffer.
  @param[in] size       Size of region. \n
                        @note1 Deprecated and unused.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_prepare_shared_buf_for_nosecure_read(void * address, unsigned int size);


/**
  Prepares the shared buffer sent by the REE before the
  trusted side reads it.

  @note1hang Invalidates the cachelines for the shared memory,
  this must be done prior to first time secure access.

  @param[in] address          Pointer to shared buffer.
  @param[in] size             Size of region. \n
                              @note1 Deprecated and unused.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_prepare_shared_buf_for_secure_read(void *address, unsigned int size);

/**
  Registers and Memory maps the Allocated OEM Imem buffer with QTEE for secure app.

  @param[in] address   Pointer to shared buffer.
  @param[in] size      Shared buffer size.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_register_imem_buffer(void *address, unsigned int *size);


/**
  Deregisters and Unmaps the OEM buffer previously registered with QTEE for secure app.

  @param[in] address   Pointer to shared buffer.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_deregister_imem_buffer(void *address);


/** @} */

/**
 * 64 bytes alignment required on start and size of the buffer
 */
#define QSEE_SHARED_BUFFER_ALIGNMENT 0x40

#endif /*QSEE_SERVICES_H*/
