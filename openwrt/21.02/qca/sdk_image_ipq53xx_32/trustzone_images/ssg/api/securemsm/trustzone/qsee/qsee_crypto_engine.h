#ifndef QSEE_CRYPTO_ENGINE_H
#define QSEE_CRYPTO_ENGINE_H

/**
@file qsee_crypto_engine.h
@brief Defines crypto-engine types
*/

/*===========================================================================
Copyright (c) {2012, 2018} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/**
@addtogroup qtee_pkey
@{

*/

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
 ===========================================================================*/
/**
 * Crypto engine/implementation types
 */
typedef enum {
  QSEE_CE_ENGINE_INV = 0,
  QSEE_CE_SW,     /**< SW Crypto Engine */
  QSEE_CE_ARMV8,  /**< Crypto Engine using ARMv8 CE extention */
  QSEE_CE_HWIO,   /**< HW Crypto Engine using HWIO Access */
  QSEE_CE_BAM,    /**< HW Crypto Engine using BAM Access */

  QSEE_CE_ENGINE_MAX_COUNT
} QSEE_CE_ENGINE_TYPE;

/** @} */

#endif
