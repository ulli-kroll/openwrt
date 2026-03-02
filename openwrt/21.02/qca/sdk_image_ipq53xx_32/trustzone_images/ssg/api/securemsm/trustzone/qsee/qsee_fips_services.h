/*===========================================================================
 *  Copyright (c) 2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *=========================================================================*/

#ifndef __QSEE_FIPS_SERVICES_H
#define __QSEE_FIPS_SERVICES_H

/**
@file qsee_fips_services.h
@brief Definitions for services provided for an OEM to prove FIPS compliance.
*/

#include <stdint.h>
#include <stddef.h>

/**
@addtogroup qtee_fips_services
@{
*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define QSEE_FIPS_SUCCESS 0
#define QSEE_FIPS_FAILURE 1
#define QSEE_FIPS_INVALID_PARAM 2

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  These values are returned when qsee_get_fips_info is called
  with QSEE_FIPS_FUSE_STATUS.
 */
typedef enum
{
    QSEE_FIPS_FUSE_NOT_BLOWN = 0,
    /**< The FIPS enablement fuse has been not been blown. */
    QSEE_FIPS_FUSE_BLOWN = 1,
    /**< The FIPS enablement fuse has been blown. */
} QSEE_FIPS_FUSE_STATUS_TYPE;

/**
  These values are returned when qsee_get_fips_info is called
  with QSEE_FIPS_SELFTEST_STATUS.
 */
typedef enum
{
    QSEE_CRYPTO_SELFTEST_NOT_RUN = 0,
    /**< The crypto selftest has not been run.
         FIPS is not enabled and no TA requiring a selftest has been loaded. */
    QSEE_CRYPTO_SELFTEST_PASSED = 1,
    /**< The crypto selftest has passed. */
} QSEE_FIPS_SELFTEST_STATUS_TYPE;

/**
  These values are returned when qsee_get_fips_info is called with the
  QSEE_FIPS_ENABLEMENT option. \n \n
  This is supported to make testing FIPS features easier by allowing a tester
  to check whether the FIPS features are enabled. \n \n

  CAUTION: The module is ONLY considered FIPS certified if the FIPS enablement
  fuse is blown. \n
  To check whether the module is offically in FIPS certified mode, check the
  QSEE_FIPS_FUSE_STATUS option. \n
 */
typedef enum
{
    QSEE_FIPS_DISABLED = 0,
    /**< FIPS is not enabled */
    QSEE_FIPS_ENABLED = 1,
    /**< FIPS is enabled */
} QSEE_FIPS_ENABLEMENT_TYPE;

/**
  These options can be used for the info_type parameter in
  qsee_get_fips_info().
 */
typedef enum
{
    QSEE_FIPS_MODULE_HMAC = 0,
    /**< Pass a 32 byte buffer with this option
         to get the crypto module HMAC. \n
         Returns the HMAC of the 32bit module when called by a 32bit TA. \n
         Returns the HMAC of the 64bit module when called by a 64bit TA. \n */
    QSEE_FIPS_FUSE_STATUS = 1,
    /**< Pass a 4 byte buffer with this option
         to check if FIPS is enabled by fuse */
    QSEE_FIPS_SELFTEST_STATUS = 2,
    /**< Pass a 4 byte buffer with this option
         to check whether the selftest has run */
    QSEE_FIPS_ENABLEMENT = 3,
    /**< Pass a 4 byte buffer with this option
         to check whether FIPS features are enabled */
} QSEE_FIPS_INFO_TYPE;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief - Provides FIPS versioning information so an OEM seeking FIPS
           certification can prove that they are using a FIPS certified version
           of cmnlib with FIPS enablement. \n
         - Requires an HMAC of the crypto module and a FIPS fuse status to 
           determine a version. \n

  @param [in]  info_type        Type of information requested.
  @param [out] buffer           Buffer to write requested information.
  @param [in]  buffer_len       Buffer length. \n
                                If an info type of QSEE_FIPS_MODULE_HMAC is
                                passed, this should be at least 32 bytes. \n
                                Other information types should be 4 bytes. \n
  @param [out] bytes_written    Number of bytes written to buffer.

  @return QSEE_FIPS_SUCCESS on success. \n
          QSEE_FIPS_FAILURE on error.
 */
int qsee_get_fips_info
(
    QSEE_FIPS_INFO_TYPE info_type,
    void* buffer,
    size_t buffer_len,
    size_t* bytes_written
);

/** @} */

#endif //__QSEE_FIPS_SERVICES_H
