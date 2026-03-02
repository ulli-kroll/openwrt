
#ifndef QSEE_SECDISP_H
#define QSEE_SECDISP_H

/**
@file qsee_secdisp.h
@brief Provide Secure Display functionality
*/

/*===========================================================================
   Copyright (c) 2015 by QUALCOMM, Technology Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_secdisp.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdbool.h>

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_secdisp
  @{
*/

/**
 * Gets the current secure display session ID.
 *
 * @param[out] sessionID Current secure display session ID.
 *
 * @return \c
 * SUCCESS -- 0 \n
 * FAILURE -- -1
 */
int qsee_sd_get_session(uint32_t *sessionID);

/**
 * Allows or blocks REE from stopping Secure Display through the sd_ctrl syscall.
 *
 * @param[in] allow TRUE to allow, FALSE to block.
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- -1
 */
int qsee_sd_set_stop_allowed(
  bool allow
);

/** @} */

#endif /* QSEE_SECDISP_H */

