#ifndef QFPROM_H
#define QFPROM_H

#define QFPROM_H_MAJOR  02
#define QFPROM_H_MINOR  01

/**
 @file qfprom.h 
 @brief
 Contains common definitions and APIs to be used to read and write QFPROM rows.
*/ 

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The qfprom_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and LaTeX. To edit or update 
      any of the file/group text in the PDF, edit the qfprom_mainpage.dox file or 
      contact Tech Pubs.

      The above description for this file is part of the "qfprom" group 
      description in the qfprom_mainpage.dox file. 
===========================================================================*/

/*==============================================================================
                  Copyright 2012 - 2016,2019,2022 Qualcomm Technologies, Inc.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
================================================================================
                              EDIT HISTORY

$Header: //components/rel/core.tzw/1.2/api/boot/qfprom/qfprom.h#2 $
$DateTime: 2022/03/31 23:21:13 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/01/22   as      Added support for RSA encrypt/decrypt, Fuse write in TA
07/20/16   lm      Expose qfprom_row_has_fec_bits
05/31/13   jz      Expose qfprom_write_multiple_rows
07/10/12   dh      Change major verion to 02 for badger family
03/08/11   ds      (Tech Pubs) Edited/added Doxygen comments and markup.
11/17/10   dxiang  Doxygen grouping changes for API auto-generation
10/26/10   dxiang  Doxygen markup standards changes 
05/10/10   ddk     Initial version.

==============================================================================*/

#include "comdef.h"
#include "qsee_fuse.h"

/** @addtogroup qfprom_API
@{ */

/* ============================================================================
**  Function : qfprom_read_row
** ============================================================================
*/
/**
 Reads the row data of the specified QFPROM row address.

 @param[in] row_address     Row address in the QFPROM region from which the row
                            data is read.
 @param[in] addr_type       Raw or corrected address.
 @param[out] row_data       Pointer to the data to be read.

 @return
 Any errors while reading data from the specified QFPROM row address.

 @dependencies
 None.
 */
uint32 qfprom_read_row
(
    uint32   row_address,
    QFPROM_ADDR_SPACE addr_type,
    uint32*  row_data
);

/* ============================================================================
**  Function : qfprom_write_row
** ============================================================================
*/
/**
 Writes the row data to the specified QFPROM raw row address.

 @param[in] raw_row_address     Row address in the QFPROM region to which the
                                row data is to be written.
 @param[in] row_data            Pointer to the data to write into the QFPROM
                                region.
 @param[in] bus_clk_khz         Frequency in kHz of the bus clock connected to
                                the QFPROM. \n
                                @note1hang
                                When the QFPROM driver runs in AMSS/L4 context,
                                this is ignored. 

 @return
 Any errors while writing data to the specified QFPROM raw row address.

 @dependencies
 None.
 */
uint32 qfprom_write_row
(
    uint32   raw_row_address,
    uint32   *row_data,
    uint32   bus_clk_khz
);

/*===========================================================================

FUNCTION  qfprom_write_multiple_rows

DESCRIPTION
This function shall write multiple rows of data to the specified QFPROM Raw Row
addresses.

DEPENDENCIES
  None.

PARAMETERS
    uint32*  row_data_address  - Pointer to the first entry in the array to be
                                 written in  the qfprom raw region.
    uint32   bus_clk_khz       - Frequency in khz of bus connected to the QFPROM .

RETURN VALUE
  uint32  Any errors while writing the specified item.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 qfprom_write_multiple_rows
(
    void   *row_data_address,
    uint32   bus_clk_khz
);

/*===========================================================================

FUNCTION  qfprom_row_has_fec_bits

DESCRIPTION
This function shall check whether the FEC bits are present or not in the specified QFPROM Row address.

DEPENDENCIES
  None.
 
PARAMETERS
    uint32   *row_address  - Pointer to the row address to check the 
                             FEC bits in the qfprom region.
    QFPROM_ADDR_SPACE addr_type - Raw or Corrected address.
    uint8*  fec_bits       - Pointer to the data.
 
RETURN VALUE
  uint32  Any errors while checking the specified item.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 qfprom_row_has_fec_bits
(
    uint32   row_address,
    QFPROM_ADDR_SPACE addr_type,
    uint8*  fec_bits
);

/*===========================================================================

FUNCTION  qfprom_perm_check

DESCRIPTION
This function checks if Access is Permitted to QFPROM Row.

DEPENDENCIES
  None.

PARAMETERS
    uint32   *row_address  - Pointer to the row address to be read from
                                      the qfprom region.
    QFPROM_ADDR_SPACE addr_type - Raw or Corrected address.
    uint32   operation     - Indicates the type of operation read/write to the fuse.

RETURN VALUE
  uint32  Any errors while Checking Permission.

SIDE EFFECTS
  None.

===========================================================================*/

uint32 qfprom_perm_check
(
    uint32   row_address,
    QFPROM_ADDR_SPACE addr_type,
    uint32   operation
);


/** @} */ /* end_addtogroup qfprom_API */

#endif /* QFPROM_H */

