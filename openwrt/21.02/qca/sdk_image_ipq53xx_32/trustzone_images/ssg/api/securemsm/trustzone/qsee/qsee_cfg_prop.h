#ifndef QSEE_CFG_PROP_H
#define QSEE_CFG_PROP_H

/*
 * Copyright 2015-18 QUALCOMM Technologies, Inc. 
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 */

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

  when       who      what, where, why
  --------   ---      ------------------------------------
  02/02/15   cz       Initial version

===========================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
/** @cond */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum
{
    QSEE_CFG_SUCCESS                   = 0x00,
    QSEE_CFG_INVALID_INPUT             = 0x01,
    QSEE_CFG_BUFFER_TOO_SMALL          = 0x02,
    QSEE_CFG_ERROR_GET_PROP_HDLSTR     = 0x03,
    QSEE_CFG_ERROR_GET_PROP_VALUE      = 0x04,
    QSEE_CFG_ERROR_MEMSCPY             = 0x05,
    QSEE_CFG_UNKNOWN_TYPE              = 0x06,
    
    QSEE_CFG_MAX_ERROR_CODE     = 0x7FFFFFFF
} qsee_cfg_error;

typedef struct
{
    uint32_t dwType; /* DALProp attribute types are defined in DALPropDef.h */
    uint32_t dwLen;  /* in bytes */
    uint8_t  val[1]; /* If dwLen > 1, the left variables followed by val[0] */
}  qsee_cfg_propvar_t;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
  @addtogroup qtee_cfg_prop
  @{
*/

/**
  Fetches a configuration value from the TZ kernel via a SYS CALL.
 
  @param[in]    PropName     Pointer to property name (a string).
  @param[in]    PropNameLen  PropName length, including '\0' (e.g. strnlen()+1).
  @param[in]    PropId       Property ID.
  @param[out]   pPropBuf     Pointer to output buffer that is populated with the DAL configuration value.
  @param[in]    PropBufSz    Output buffer size (in bytes).
  @param[out]   PropBufSzRet Pointer to the actual populated buffer size (in bytes). If property type is a string, output size does NOT include '\0'.
 
  @return
  SUCCESS -- 0 \n
  FAILURE -- Nonzero
*/
qsee_cfg_error qsee_cfg_getpropval
(
    const char               *PropName,
    uint32_t                  PropNameLen,
    uint32_t                  PropId,
    qsee_cfg_propvar_t       *pPropBuf,
    uint32_t                  PropBufSz,
    uint32_t                 *PropBufSzRet
);

/** @} */

#endif //QSEE_CFG_PROP_H

