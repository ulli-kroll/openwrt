/**
@file qsee_bulletin_board.h
@brief Provides simple message posting mechanism between QTEE applications
*/
/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

when         who      what, where, why
--------     ---      ------------------------------------
Jun/01/2015  el       Initial version.
===========================================================================*/
#ifndef QSEE_BULLETIN_BOARD_H
#define QSEE_BULLETIN_BOARD_H


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
/** @cond */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define MAX_CATEGORY_NAME_LEN 8
#define MAX_TA_NAME_LEN       128               /* Distinguished name. */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct qsee_bulletin_board_note_
{
  uint8_t* data; /**< The input data buffer. */
  uint32_t size; /**< Size of the data buffer. */

  char category[MAX_CATEGORY_NAME_LEN +
                1]; /**< Name of the category under which the note will be posted. */

} qsee_bulletin_board_note;

typedef struct qsee_ta_name_
{
  char name[MAX_TA_NAME_LEN + 1]; /**< Name of the TA which posted this note. */
} qsee_ta_name;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @addtogroup qtee_bb
 * @{
 */

/**
 * Posts a note to the QTEE bulletin board, under a specific category.
 *
 * Each category of the QTEE bulletin board only holds the most recent note that was posted to that
 * category, overwriting the previous one. Notes cannot be deleted from
 * the bulletin board and are available until they are overwritten by a
 * newer note or the device reboots.
 *
 * @note1hang
 * No permission model is applied on the bulletin board.
 * Every QTEE application can post notes under any category.
 *
 * @param[in] note        Pointer to a structure that holds the note to post:
 *                        - note->data shall point to a buffer that holds the
 *                          data to post.
 *                        - note->size of the data buffer.
 *                        - note->category shall be a non-empty, NULL terminated
 *                          ASCII string that holds the name of the category under
 *                          which the note is posted.
 *                          @tablebulletend
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- Negative
 */
int qsee_bulletin_board_post(const qsee_bulletin_board_note* note);


/**
 * Reads a note, if available, from the QTEE bulletin board from a specific category.
 *
 * @note1hang
 * No permission model is applied on the bulletin board.
 * Every QTEE application can read all available notes from all categories.
 *
 * @param[in,out] note          Pointer to a structure that holds the read note:
 *                              - note->data shall point to a buffer that has enough space
 *                                to hold the note from the desired category.
 *                              - note->size is a bi-directional parameter. When used as input,
 *                                it indicates buffer size.
 *                                When the function returns, it holds the the bytes
 *                                that were actually copied into the user-provided buffer.
 *                              - note->category shall hold the category name of the category whose
 *                                note is desired.
 *                                @tablebulletend
 *
 * @param[in,out] sender_name   Pointer to a structure that holds the TA name
 *                              that posted the returned note.
 *
 * @return
 * SUCCESS -- 0 \n
 * FAILURE -- Negative
 */
int qsee_bulletin_board_read(qsee_bulletin_board_note* note, qsee_ta_name* sender_name);

/** @} */

#endif // QSEE_BULLETIN_BOARD_H
