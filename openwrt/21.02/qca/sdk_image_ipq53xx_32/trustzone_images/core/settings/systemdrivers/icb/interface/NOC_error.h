#ifndef __NOC_ERROR_H__
#define __NOC_ERROR_H__
/*============================================================================

FILE:      NOC_Error.h

DESCRIPTION: APIs and data structure declarations 
             for NOC Error Handler

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.tzw/1.2/settings/systemdrivers/icb/interface/NOC_error.h#2 $
$DateTime: 2022/09/09 14:09:31 $
$Author: pwbldsvc $
$Change: 39605750 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2022/09/08  sp     Added SBM register offsets for QNOC4_7 which uses silicon debug 
2018/03/30  sds    Add definition for Gladiator NoC errors
2018/01/11  sds    Expand SBM definition
2017/10/02  sds    Add support for chip versioned configs
2017/03/01  sds    Add timeout enable register support
2017/02/06  sds    Rewrite for QNoC 4.x
2015/11/23  pm     Added Sideband Manager support
2014/12/09  tb     Split OEM modifiable data from internal data
2013/05/10  pm     Added NOC_ERRLOG6 macro to support FlexNoc 2.8
2012/10/03  av     Created

              Copyright (c) 2014 - 2018, 2022 Qualcomm Technologies, Inc.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/
#include "com_dtypes.h"
#include "stdint.h"
#include "stdbool.h"
#include "ChipInfo.h"


/*============================================================================
                          DEFINEs/MACROs
============================================================================*/
/* Reserved value of offset that indicates that the register does not apply to
 * this version of hardware
 */
#define REGISTER_NOT_APPLICABLE 0xFFFF 

/*============================================================================
                          TYPE DEFINITION
============================================================================*/
/**
 * NOC Register Interface
 */
typedef struct
{
   uint16_t swid_low;      
   uint16_t swid_high;   
   uint16_t main_ctl_low;
   uint16_t err_valid_low;
   uint16_t err_clear_low;    
   uint16_t errlog0_low;
   uint16_t errlog0_high;   
   uint16_t errlog1_low;
   uint16_t errlog1_high;   
   uint16_t errlog2_low;
   uint16_t errlog2_high;   
   uint16_t errlog3_low;
   uint16_t errlog3_high;
   /* Additional fields for gladiator NoC errors. */
   uint16_t errlog2_1_low;
   uint16_t errlog2_1_high;
   uint16_t errlog4_3_low;
   uint16_t errlog4_3_high;
   uint16_t errlog6_5_low;
   uint16_t errlog6_5_high;
   uint16_t errlog8_high;
/*QCNOC2 registers*/
   uint16_t errlog4_low;
   uint16_t errlog5_low;
   uint16_t errlog6_low;
   uint16_t stallen;
   uint16_t faulten;
   /* New fields for new NoC versions go here */
} NOC_hw_type;

/**
 * NOC SidebandManager Register Interface
 */
typedef struct
{
   uint16_t swid_low;
   uint16_t swid_high;
   uint16_t faultin_en0_low;
   uint16_t faultin_en0_high;
   uint16_t faultin_status0_low;
   uint16_t faultin_status0_high;
   uint16_t faultin_en1_low;
   uint16_t faultin_en1_high;
   uint16_t faultin_status1_low;
   uint16_t faultin_status1_high;
   /*QNOC2*/
   uint16_t faulten;
   uint16_t faultstatus;
   uint16_t sensein0;
   uint16_t sensein1;
   uint16_t sensein2;
   /* New fields for new NoC SidebandManager versions go here */
} NOC_sideband_hw_type;

/**
 * NOC Error Syndrome Register Data Types
 */
typedef struct
{
  uint32_t FAULTINSTATUS0_LOW;
  uint32_t FAULTINSTATUS0_HIGH;
  uint32_t FAULTINSTATUS1_LOW;
  uint32_t FAULTINSTATUS1_HIGH;
  /*QNOC2*/
  uint32_t FAULTSTATUS;
  uint32_t SENSEIN0;
  uint32_t SENSEIN1;
  uint32_t SENSEIN2;
} NOCERR_sbm_syndrome_type;

typedef struct
{
  uint32_t ERRLOG0_LOW;
  uint32_t ERRLOG0_HIGH;
  uint32_t ERRLOG1_LOW;
  uint32_t ERRLOG1_HIGH;
  uint32_t ERRLOG2_LOW;
  uint32_t ERRLOG2_HIGH;
  uint32_t ERRLOG3_LOW;
  uint32_t ERRLOG3_HIGH;
  uint32_t ERRLOG2_1_LOW;
  uint32_t ERRLOG2_1_HIGH;
  uint32_t ERRLOG4_3_LOW;
  uint32_t ERRLOG4_3_HIGH;
  uint32_t ERRLOG6_5_LOW;
  uint32_t ERRLOG6_5_HIGH;
  uint32_t ERRLOG8_HIGH;
  uint32_t ERRLOG4_LOW;
  uint32_t ERRLOG5_LOW;
  uint32_t ERRLOG6_LOW;
  NOCERR_sbm_syndrome_type *sbms;
} NOCERR_syndrome_type;


/**
 *  Check info type <offs, mask, value>
 */

typedef struct 
{
    uint32_t offs;                /**< Offset within the NOCERR_syndrome_type structure*/
    uint32_t mask;                /**< Mask of that need to be applied on value */
    uint32_t val;                 /**< Value that need to be compared with */
}CHECK_info_type;

/**
 * Filter_info: It will have array of checks, which need to be passed before this filer is applied
 */

typedef struct 
{
    uint32_t len;                 /**< Lenght of the CHECK_info_type array*/
    CHECK_info_type *checks;    /**< Array of the check for this filter */
}FILTER_info_type;

/**
 * Internal NOC error Info data type
 */
typedef struct
{
    char* name;                     /**< NOC name */
  NOC_hw_type*           hw;              /**< NOC register offsets */
    void* base_addr;                /**< NOC base address */
    uintptr_t intr_vector;          /**< NOC interrupt vector */
  uint32_t               num_sbms;        /**< Number of NoC sideband managers */
  NOC_sideband_hw_type **sb_hw;           /**< NOC sideband register offsets */
  void**                 sb_base_addrs;   /**< NOC sideband base addresses */
  uint32_t               num_tos;         /**< Number of timeout enable registers */
  void**                 to_addrs;        /**< NOC timeout enable registers */
  NOCERR_syndrome_type   syndrome;        /**< Error syndrome */
}NOCERR_info_type;

/**
 * OEM NOC error Info data type
 */
typedef struct
{
  uint32_t faultin_en0_low;
  uint32_t faultin_en0_high;
  uint32_t faultin_en1_low;
  uint32_t faultin_en1_high;
} NOCERR_sbm_info_type_oem;

typedef struct
{
  char*                     name;         /**< NOC name */
  bool                      intr_enable;  /**< NOC errorlogger interrupt enable */
  bool                      error_fatal;  /**< NOC interrupt error fatal */ 
  NOCERR_sbm_info_type_oem *sbms;         /**< NOC sideband manager cfgs */
  NOCERR_sbm_info_type_oem *obs_mask;     /**< NOC sideband manager for obs cfg */
  uint32_t *                to_reg_vals;  /**< NoC timeout enable register values */
  uint32_t                  filter_len;   /**< Length of NOC FILTER  info array */
  FILTER_info_type*         filter_info;  /**< NOC regsiters filter info  */
}NOCERR_info_type_oem;


typedef struct
{
  /* Chip version information for this device data. */
  ChipInfoFamilyType   family;
  bool                 match;
  ChipInfoVersionType  version;

  /* Associated target data */
  uint32_t          len;             /**< Length of cfgdata array */
  NOCERR_info_type* NOCInfo;         /**< Pointer to internal cfgdata array*/
  uint32_t          num_clock_regs;  /**< Number of external timeout clock regs */
  void**            clock_reg_addrs; /**< External timeout clock regs */
    const char *const *BIMC_reg_names;
    const uint64 *BIMC_reg_addr;
    uint8 BIMC_num_reg;
    const uint64 *clock_reg_addr;
    uint8 clock_num_reg;
}NOCERR_propdata_type;

typedef struct
{
  uint32_t              num_configs;
  NOCERR_propdata_type *configs;
} NOCERR_config_info_type;

/**
 * NOC Error OEM Property Data type
 */
typedef struct
{
  /* Chip version information for this device data. */
  ChipInfoFamilyType   family;
  bool                 match;
  ChipInfoVersionType  version;

  /* Associated target data */
  uint32_t              len;            /**< Length of cfgdata array */
  NOCERR_info_type_oem* NOCInfoOEM;     /**< Pointer to OEM cfgdata array*/
  uint32_t *            clock_reg_vals; /**< NOC timeout external register values */
}NOCERR_propdata_type_oem;

typedef struct
{
  uint32_t                  num_configs;
  NOCERR_propdata_type_oem *configs;
} NOCERR_config_info_type_oem;

/*============================================================================
                        NOC HW Register DEFINITIONS 
============================================================================*/
/* NOC HW Register Offsets Version 4.0 */
/* Use ERRLOG_SWID_LOW as base address. */
static NOC_hw_type QNOC_4_0 = {
  .swid_low       = 0x0,
  .swid_high      = 0x4,
  .main_ctl_low   = 0x8,
  .err_valid_low  = 0x10,
  .err_clear_low  = 0x18,
  .errlog0_low    = 0x20,
  .errlog0_high   = 0x24,
  .errlog1_low    = 0x28,
  .errlog1_high   = 0x2C,
  .errlog2_low    = 0x30,
  .errlog2_high   = 0x34,
  .errlog3_low    = 0x38,
  .errlog3_high   = 0x3C,
  .errlog2_1_low  = REGISTER_NOT_APPLICABLE,
  .errlog2_1_high = REGISTER_NOT_APPLICABLE,
  .errlog4_3_low  = REGISTER_NOT_APPLICABLE,
  .errlog4_3_high = REGISTER_NOT_APPLICABLE,
  .errlog6_5_low  = REGISTER_NOT_APPLICABLE,
  .errlog6_5_high = REGISTER_NOT_APPLICABLE,
  .errlog8_high   = REGISTER_NOT_APPLICABLE,
  .errlog4_low    = REGISTER_NOT_APPLICABLE,
  .errlog5_low    = REGISTER_NOT_APPLICABLE,
  .errlog6_low    = REGISTER_NOT_APPLICABLE,
  .stallen        = REGISTER_NOT_APPLICABLE,
  .faulten        = REGISTER_NOT_APPLICABLE,

};


static NOC_hw_type QNOC_2_1 = {
  .swid_low       = 0x0,
  .swid_high      = 0x4,
  .main_ctl_low   = REGISTER_NOT_APPLICABLE,
  .err_valid_low  = 0xC,
  .err_clear_low  = 0x10,
  .errlog0_low    = 0x14,
  .errlog0_high   = REGISTER_NOT_APPLICABLE,
  .errlog1_low    = 0x18,
  .errlog1_high   = REGISTER_NOT_APPLICABLE,
  .errlog2_low    = 0x1C,
  .errlog2_high   = REGISTER_NOT_APPLICABLE,
  .errlog3_low    = 0x20,
  .errlog3_high   = REGISTER_NOT_APPLICABLE,
  .errlog2_1_low  = REGISTER_NOT_APPLICABLE,
  .errlog2_1_high = REGISTER_NOT_APPLICABLE,
  .errlog4_3_low  = REGISTER_NOT_APPLICABLE,
  .errlog4_3_high = REGISTER_NOT_APPLICABLE,
  .errlog6_5_low  = REGISTER_NOT_APPLICABLE,
  .errlog6_5_high = REGISTER_NOT_APPLICABLE,
  .errlog8_high   = REGISTER_NOT_APPLICABLE,
  .errlog4_low    = 0x24,
  .errlog5_low    = 0x28,
  .errlog6_low    = REGISTER_NOT_APPLICABLE,
  .stallen        = 0x38,
  .faulten        = 0x8,

};

static NOC_hw_type QNOC_2_0 = {
  .swid_low       = 0x0,
  .swid_high      = 0x4,
  .main_ctl_low   = REGISTER_NOT_APPLICABLE,
  .err_valid_low  = 0xC,
  .err_clear_low  = 0x10,
  .errlog0_low    = 0x14,
  .errlog0_high   = REGISTER_NOT_APPLICABLE,
  .errlog1_low    = 0x18,
  .errlog1_high   = REGISTER_NOT_APPLICABLE,
  .errlog2_low    = REGISTER_NOT_APPLICABLE,
  .errlog2_high   = REGISTER_NOT_APPLICABLE,
  .errlog3_low    = 0x20,
  .errlog3_high   = REGISTER_NOT_APPLICABLE,
  .errlog2_1_low  = REGISTER_NOT_APPLICABLE,
  .errlog2_1_high = REGISTER_NOT_APPLICABLE,
  .errlog4_3_low  = REGISTER_NOT_APPLICABLE,
  .errlog4_3_high = REGISTER_NOT_APPLICABLE,
  .errlog6_5_low  = REGISTER_NOT_APPLICABLE,
  .errlog6_5_high = REGISTER_NOT_APPLICABLE,
  .errlog8_high   = REGISTER_NOT_APPLICABLE,
  .errlog4_low    = 0x24,
  .errlog5_low    = 0x28,
  .errlog6_low    = REGISTER_NOT_APPLICABLE,
  .stallen        = 0x38,
  .faulten        = 0x8,

};


/* NOC HW Register Offsets Gladiator Version */
/* Use ERRORVLD_LOW as base address. */
static NOC_hw_type GLADIATOR_0_0 = {
  .swid_low       = REGISTER_NOT_APPLICABLE,
  .swid_high      = REGISTER_NOT_APPLICABLE,
  .main_ctl_low   = 0x0,  /**< gladiator errvld_low */
  .err_valid_low  = 0x4,  /**< gladiator errvld_high */
  .err_clear_low  = 0x8,  /**< gladiator errlog0_low */
  .errlog0_low    = REGISTER_NOT_APPLICABLE,
  .errlog0_high   = 0xC,
  .errlog1_low    = REGISTER_NOT_APPLICABLE,
  .errlog1_high   = REGISTER_NOT_APPLICABLE,
  .errlog2_low    = REGISTER_NOT_APPLICABLE,
  .errlog2_high   = REGISTER_NOT_APPLICABLE,
  .errlog3_low    = REGISTER_NOT_APPLICABLE,
  .errlog3_high   = REGISTER_NOT_APPLICABLE,
  .errlog2_1_low  = 0x10,
  .errlog2_1_high = 0x14,
  .errlog4_3_low  = 0x18,
  .errlog4_3_high = 0x1C,
  .errlog6_5_low  = 0x20,
  .errlog6_5_high = 0x24,
  .errlog8_high   = 0x2C,
  .errlog4_low    = REGISTER_NOT_APPLICABLE,
  .errlog5_low    = REGISTER_NOT_APPLICABLE,
  .errlog6_low    = REGISTER_NOT_APPLICABLE,
  .stallen        = REGISTER_NOT_APPLICABLE,
  .faulten        = REGISTER_NOT_APPLICABLE,
};

/* NOC SidebandManager HW Register Offsets */
static NOC_sideband_hw_type QNOC_SB_4_0_L = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = REGISTER_NOT_APPLICABLE,
   .faultin_en1_low      = REGISTER_NOT_APPLICABLE,
   .faultin_en1_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status1_low  = REGISTER_NOT_APPLICABLE,
   .faultin_status1_high = REGISTER_NOT_APPLICABLE,
   .faulten              = REGISTER_NOT_APPLICABLE,
   .faultstatus          = REGISTER_NOT_APPLICABLE,
   .sensein0             = REGISTER_NOT_APPLICABLE,
   .sensein1             = REGISTER_NOT_APPLICABLE,
   .sensein2             = REGISTER_NOT_APPLICABLE,
};

/* Below struct is added for the NSS NOC in ipq53xx which uses QNOC_4.7-11 */
/* NSS NOC uses silicon debug whereas other NOCs in ipq53xx uses debug chain */
static NOC_sideband_hw_type QNOC_SB_4_7 = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = REGISTER_NOT_APPLICABLE,
   .faultin_en1_low      = REGISTER_NOT_APPLICABLE,
   .faultin_en1_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status1_low  = REGISTER_NOT_APPLICABLE,
   .faultin_status1_high = REGISTER_NOT_APPLICABLE,
   .faulten              = REGISTER_NOT_APPLICABLE,
   .faultstatus          = REGISTER_NOT_APPLICABLE,
   .sensein0             = 0x100,
   .sensein1             = 0x104,
   .sensein2             = 0x108,
};

static NOC_sideband_hw_type QNOC_SB_4_0 = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = 0x44,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = 0x4C,
   .faultin_en1_low      = REGISTER_NOT_APPLICABLE,
   .faultin_en1_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status1_low  = REGISTER_NOT_APPLICABLE,
   .faultin_status1_high = REGISTER_NOT_APPLICABLE,
   .faulten              = REGISTER_NOT_APPLICABLE,
   .faultstatus          = REGISTER_NOT_APPLICABLE,
   .sensein0             = REGISTER_NOT_APPLICABLE,
   .sensein1             = REGISTER_NOT_APPLICABLE,
   .sensein2             = REGISTER_NOT_APPLICABLE,
};

static NOC_sideband_hw_type QNOC_SB_4_0_2L = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = 0x44,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = 0x4C,
   .faultin_en1_low      = 0x50,
   .faultin_en1_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status1_low  = 0x58,
   .faultin_status1_high = REGISTER_NOT_APPLICABLE,
   .faulten              = REGISTER_NOT_APPLICABLE,
   .faultstatus          = REGISTER_NOT_APPLICABLE,
   .sensein0             = REGISTER_NOT_APPLICABLE,
   .sensein1             = REGISTER_NOT_APPLICABLE,
   .sensein2             = REGISTER_NOT_APPLICABLE,
};

static NOC_sideband_hw_type QNOC_SB_4_0_2 = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = 0x44,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = 0x4C,
   .faultin_en1_low      = 0x50,
   .faultin_en1_high     = 0x54,
   .faultin_status1_low  = 0x58,
   .faultin_status1_high = 0x5C,
   .faulten              = REGISTER_NOT_APPLICABLE,
   .faultstatus          = REGISTER_NOT_APPLICABLE,
   .sensein0             = REGISTER_NOT_APPLICABLE,
   .sensein1             = REGISTER_NOT_APPLICABLE,
   .sensein2             = REGISTER_NOT_APPLICABLE,

};

static NOC_sideband_hw_type QNOC_SB_2_0 = {
   .swid_low             = 0x0,
   .swid_high            = REGISTER_NOT_APPLICABLE,
   .faultin_en0_low      = REGISTER_NOT_APPLICABLE,
   .faultin_en0_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status0_low  = REGISTER_NOT_APPLICABLE,
   .faultin_status0_high = REGISTER_NOT_APPLICABLE,
   .faultin_en1_low      = REGISTER_NOT_APPLICABLE,
   .faultin_en1_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status1_low  = REGISTER_NOT_APPLICABLE,
   .faultin_status1_high = REGISTER_NOT_APPLICABLE,
   .faulten              = REGISTER_NOT_APPLICABLE,
   .faultstatus          = REGISTER_NOT_APPLICABLE,
   .sensein0             = 0xB0,
   .sensein1             = REGISTER_NOT_APPLICABLE,
   .sensein2             = REGISTER_NOT_APPLICABLE,
};


static NOC_sideband_hw_type QNOC_SB_2_12 = {
   .swid_low             = 0x0,
   .swid_high            = REGISTER_NOT_APPLICABLE,
   .faultin_en0_low      = 0x10,
   .faultin_en0_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status0_low  = 0x14,
   .faultin_status0_high = REGISTER_NOT_APPLICABLE,
   .faultin_en1_low      = REGISTER_NOT_APPLICABLE,
   .faultin_en1_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status1_low  = REGISTER_NOT_APPLICABLE,
   .faultin_status1_high = REGISTER_NOT_APPLICABLE,
   .faulten              = 0x8,
   .faultstatus          = 0xC,
   .sensein0             = 0xB0,
   .sensein1             = 0xB4,
   .sensein2             = 0xB8,
};

/*============================================================================
                                 FUNCTIONS
============================================================================*/

//*============================================================================*/
/**
@brief 
      Performs initialization for NOC error handler.
      It enables interrupts required to handle NOC errors.
 
@param[in]  None.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void NOC_Error_Init(void);

#endif /* __NOC_ERROR_H__ */
