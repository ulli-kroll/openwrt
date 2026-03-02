#ifndef QUPAC_COMMONIDS_H
#define QUPAC_COMMONIDS_H

/*===========================================================================
         Copyright (c) 2016-2018 by QUALCOMM Technologies, Incorporated.  
              All Rights Reserved.
            QUALCOMM Confidential & Proprietary
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE


  when       who     what, where, why
  --------   ---     ------------------------------------------------------------
  06/13/18   dpk     Moved protocol enums to this file since they has to be exposed.
  05/28/15   rc      Inlined peripheral ID ENUM index with the honeybadger target
  12/18/15   dpk     Added subsystem ID macros
  09/15/15   dpk     Created
  =============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

#define BLSP_QUP_1_DEV_ACCESS "/dev/buses/qup/blsp_qup_1"
#define BLSP_QUP_2_DEV_ACCESS "/dev/buses/qup/blsp_qup_2"
#define BLSP_QUP_3_DEV_ACCESS "/dev/buses/qup/blsp_qup_3"
#define BLSP_QUP_4_DEV_ACCESS "/dev/buses/qup/blsp_qup_4"
#define BLSP_QUP_5_DEV_ACCESS "/dev/buses/qup/blsp_qup_5"
#define BLSP_QUP_6_DEV_ACCESS "/dev/buses/qup/blsp_qup_6"
#define BLSP_QUP_7_DEV_ACCESS "/dev/buses/qup/blsp_qup_7"
#define BLSP_QUP_8_DEV_ACCESS "/dev/buses/qup/blsp_qup_8"
#define BLSP_QUP_9_DEV_ACCESS "/dev/buses/qup/blsp_qup_9"
#define BLSP_QUP_10_DEV_ACCESS "/dev/buses/qup/blsp_qup_10"
#define BLSP_QUP_11_DEV_ACCESS "/dev/buses/qup/blsp_qup_11"
#define BLSP_QUP_12_DEV_ACCESS "/dev/buses/qup/blsp_qup_12"

#define BLSP_UART_1_DEV_ACCESS "/dev/buses/uart/blsp_uart_1"
#define BLSP_UART_2_DEV_ACCESS "/dev/buses/uart/blsp_uart_2"
#define BLSP_UART_3_DEV_ACCESS "/dev/buses/uart/blsp_uart_3"
#define BLSP_UART_4_DEV_ACCESS "/dev/buses/uart/blsp_uart_4"
#define BLSP_UART_5_DEV_ACCESS "/dev/buses/uart/blsp_uart_5"
#define BLSP_UART_6_DEV_ACCESS "/dev/buses/uart/blsp_uart_6"
#define BLSP_UART_7_DEV_ACCESS "/dev/buses/uart/blsp_uart_7"
#define BLSP_UART_8_DEV_ACCESS "/dev/buses/uart/blsp_uart_8"
#define BLSP_UART_9_DEV_ACCESS "/dev/buses/uart/blsp_uart_9"
#define BLSP_UART_10_DEV_ACCESS "/dev/buses/uart/blsp_uart_10"
#define BLSP_UART_11_DEV_ACCESS "/dev/buses/uart/blsp_uart_11"
#define BLSP_UART_12_DEV_ACCESS "/dev/buses/uart/blsp_uart_12"



#define AC_NONE  0
#define AC_TZ  1
#define AC_RPM  2
#define AC_HLOS  3
#define AC_HYP  4
#define AC_SSC_Q6_ELF 5
#define AC_ADSP_Q6_ELF 6 // Single 
#define AC_SSC_HLOS 7   // ??, may be we combine this with other SSC one //
#define AC_CP_TOUCH 8
#define AC_CP_BITSTREAM 9
#define AC_CP_PIXEL 10
#define AC_CP_NON_PIXEL 11
#define AC_VIDEO_FW 12
#define AC_CP_CAMERA 13
#define AC_HLOS_UNMAPPED 14
#define AC_MSS_MSA 15
#define AC_MSS_NONMSA 16
#define AC_UNMAPPED 17
#define BLSP_AC_LAST 18
#define AC_DEFAULT 0xFF


typedef enum TzBspBlspPeripheralId TzBspBlspPeripheralId;
enum TzBspBlspPeripheralId
{
   BLSP_QUP_1,
   BLSP_QUP_2,
   BLSP_QUP_3,
   BLSP_QUP_4,
   BLSP_QUP_5,
   BLSP_QUP_6,
   BLSP_QUP_7,
   BLSP_QUP_8,
   BLSP_QUP_9,
   BLSP_QUP_10,
   BLSP_QUP_11,
   BLSP_QUP_12,
   BLSP_QUP_END,
   BLSP_UART_START=15,
   BLSP_UART_1 = BLSP_UART_START,
   BLSP_UART_2,
   BLSP_UART_3,
   BLSP_UART_4,
   BLSP_UART_5,
   BLSP_UART_6,
   BLSP_UART_7,
   BLSP_UART_8,
   BLSP_UART_9,
   BLSP_UART_10,
   BLSP_UART_11,
   BLSP_UART_12,
   BLSP_INVALID_ID = 0xFFFFFFFF,
};

typedef enum TzBspBlspProtocol TzBspBlspProtocol;
enum TzBspBlspProtocol
{
   PROTOCOL_SPI,
   PROTOCOL_I2C,
   PROTOCOL_UART_2_LINE,   /* Without HW Flow control*/
   PROTOCOL_UART_4_LINE    /* With HW Flow control */
};

#endif

