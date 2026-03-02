#ifndef QSEE_INTMASK_H
#define QSEE_INTMASK_H

/**
   @file qsee_intmask.h
   @brief Provide APIs to mask and unmap various interrupt sources
*/

/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_intmask.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
09/22/15   pre      Initial version.

===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
/** @cond */

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/** Interrupt mask request succeeded */
#define QSEE_INTMASK_SUCCESS                          0
/** MINK IPC Transport Error */
#define QSEE_INTMASK_ERROR                           -1
/** Returned when a value other than those defined by the
    qsee_intsrc_t enum is passed as a paramter.  Also returned if
    the caller runs in AArch32 and QSEE_DEBUG bit is set. */
#define QSEE_INTMASK_ERROR_UNSUPPORTED_INTERRUPT_BIT -2
/** Returned when a request to unmask IRQs may not be honored due to
    it resulting in system instability due to QSEE currently handling
    an atomic service request. */
#define QSEE_INTMASK_ERROR_MAY_NOT_UNMASK_IRQ        -3

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/**
 * Bitmask definition for supported interrupt sources.
 */
typedef enum
{
  QSEE_FIQ    = 1, /**< Secure interrupts. */
  QSEE_IRQ    = 2, /**< Non-secure interrupts. */
  QSEE_SERROR = 4, /**< SError for AA64 and external aborts for AA32. */
  QSEE_DEBUG  = 8, /**< Debug Exceptions (AA64 only). */
  QSEE_INTSRC_ENUM = 0x1FFFF,
} qsee_intsrc_t;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_intmask
  @{
*/

/**
 * Disables all supported interrupt sources.
 *
 * @param[out] restore_mask Pointer to a bit mask of masked interrupt sources
 *                          prior to calling this function. \n
 *                          Can restore the interrupt mask state
 *                          after calling this function by passing the
 *                          value to qsee_set_intmask().
 *
 * @warning The interrupt state is not preserved when a syscall is made to REE.
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- Negative
 */
int qsee_disable_all_interrupts(qsee_intsrc_t* restore_mask);

/**
 * Masks and unmasks interrupt sources as directed by the input.
 *
 * @param[in] int_mask A bit mask of interrupts to enable and
 *                     disable. \n 
 *                     Value of 1 indicates the interrupt source
 *                     should be masked. \n 
 *                     Value of zero indicates
 *                     the interrupt source should be unmasked.
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- Negative
 */
int qsee_set_intmask(qsee_intsrc_t int_mask);

/**
 * Returns the masking status of all supported interrupt sources.
 *
 * @param[out] int_mask Pointer to a bit mask of currently masked interrupt
 *                      sources. \n 
 *                      A value of 1 indicates the
 *                      interrupt source is currently masked.
 *
 * @warning The interrupt state is not preserved when a syscall is made to REE.
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- Negative
 */
int qsee_get_intmask(qsee_intsrc_t* int_mask);

/** @} */

#endif /* QSEE_INTMASK_H */
