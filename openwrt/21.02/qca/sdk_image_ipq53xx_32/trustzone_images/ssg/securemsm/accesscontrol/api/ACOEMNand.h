#ifndef __AC_OEM_NAND_H__
#define __AC_OEM_NAND_H__

/*=========================================================================
FILE: ACOEMNand.h
OVERVIEW: Public interface for OEM nand permission configurations.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Technologies Inc. Confidential and Proprietary
=========================================================================*/

/*==============================================================================

  EDIT HISTORY FOR FILE

  $Header: 
  $DateTime: 
  $Author: 

  when       who      what, where, why
  --------   ---      ------------------------------------
  10/10/18   dn       Created

  ==============================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define NAND_PERM_NONE       (1 << 0)
#define NAND_PERM_APPS       (1 << 1)
#define NAND_PERM_MSS        (1 << 2)
#define NAND_PERM_APPS_MSS   (1 << 3)
#define NAND_PERM_ALL        (1 << 4)


/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
  NAND_MPU_RG0 = 0,
  NAND_MPU_RG1,
  NAND_MPU_RG2,
  NAND_MPU_RG3,
  NAND_MPU_RG4,
  NAND_MPU_RG5,
  NAND_MPU_RG6,
  NAND_MPU_RG7,
  NAND_MPU_RG_NUM
} nand_mpu_rg;

typedef struct
{
  const char *name;
  nand_mpu_rg group;
} ACOEMNandPartition;

typedef struct
{
    uint32 read_perm;
    uint32 write_perm;
} ACOEMNandPermission;

/*-------------------------------------------------------------------------
 * Global Data Definitions
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Static Variable Definitions
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

#endif // __AC_OEM_NAND_H__
