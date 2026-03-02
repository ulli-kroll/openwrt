/*==============================================================================

FILE:      NOC_error_OEM_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.tzw/1.2/settings/systemdrivers/icb/config/90xx/BIMC_error_OEM_data.c#1 $ 
$DateTime: 2020/12/21 07:48:18 $
$Author: pwbldsvc $
$Change: 27890791 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2015/09/15  ddk   Disabling error fatal for bringup
2015/05/07  tb     Created
 
        Copyright (c) 2018 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "BIMC_error.h"
#include "BIMC_error_HWIO.h"

/*============================================================================
                           TARGET SPECIFIC DATA
============================================================================*/

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* NOC Configuration Data*/
const static BIMC_BRIC_info_type_oem BRIC_info_oem[] = 
{ 
//  {name, turn_on_intr, err_fatal}
    {"BRIC INTERRUPT 1", TRUE, TRUE}
};

const static BIMC_DEFAULT_info_type_oem DEFAULT_info_oem[] =
{
//  {name, turn_on_intr, err_fatal}
    {"DEFAULT SWAY INTERRUPT", TRUE, TRUE}
};

const static BIMC_DDR_SCMO_info_type_oem DDR_SCMO_info_oem[] =
{ 
//  {name, turn_on_intr, err_fatal}
    {"DDR0 SCMO INTERRUPT", TRUE, TRUE}
};

const BIMC_propdata_type_oem BIMC_propdata_oem =
{
    sizeof(BRIC_info_oem)/sizeof(BIMC_BRIC_info_type_oem), 
    BRIC_info_oem,
    sizeof(DEFAULT_info_oem)/sizeof(BIMC_DEFAULT_info_type_oem), 
    DEFAULT_info_oem,
    sizeof(DDR_SCMO_info_oem)/sizeof(BIMC_DDR_SCMO_info_type_oem), 
    DDR_SCMO_info_oem,
};
