#ifndef QSEE_TIMER_H
#define QSEE_TIMER_H

/**
@file qsee_timer.h
@brief Provide API wrappers for timer functions
*/

/*============================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_timer.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     --------------------------------------------------------
08/04/11    rv     Initial Revision

=========================================================================== */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_timer
  @{
*/

/**
 * @brief Gets up time from bootup in ms.
 *
 * @return Up time in ms from system bootup.
 *
 */
unsigned long long qsee_get_uptime(void);

/** @} */

#endif /*QSEE_TIMER_H*/

