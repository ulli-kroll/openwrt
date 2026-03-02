/*===========================================================================
   Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#pragma once

/* error codes */
typedef enum
{
  E_SECMATH_SUCCESS = 0,         ///< Result OK
  E_SECMATH_FAILURE,             ///< Generic Error */
  E_SECMATH_NOP,                 ///< Not a failure but no operation was performed */
  E_SECMATH_FAIL_TESTVECTOR,     ///< Algorithm failed test vectors */
  E_SECMATH_BUFFER_OVERFLOW,     ///< Not enough space for output */
  E_SECMATH_MEM,                 ///< Out of memory */
  E_SECMATH_INVALID_ARG,         ///< Generic invalid argument */
  E_SECMATH_INVALID_PRIME_SIZE,  ///< Invalid size of prime requested */
  E_SECMATH_NOT_SUPPORTED        ///< Operation not supported
} SECMATH_ERRNO_ET;
