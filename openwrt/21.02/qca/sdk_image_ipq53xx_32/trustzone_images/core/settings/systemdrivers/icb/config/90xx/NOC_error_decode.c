/*==============================================================================
Copyright (c) 2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================
FILE:      NOC_error_decode.c

DESCRIPTION: This file implements NOC Error Decoding.
==============================================================================*/
#include "NOC_error_decode.h"

NOC_decode_data_type NOC_decode_info;

const char* const NOC_Initiator[5][50] = {
    {   /*PCNOC*/
        "qhm0",
        "pcnoc_cfg",
        "rpm",
        "tic",
        "crypto",
        "lpass",
        "snoc",
        "qdss_dap",
        "sdcc",
    },
    {   /*SNOC*/
        "pcie_ep",
        "qdss_bam",
        "snoc_cfg_0",
        "tme",
        "aggrnoc0",
        "aggrnoc1",
        "memnoc",
        "pcnoc",
        "qdss_etr",
    },
    {   /*MEMNOC*/
        "memnoc_cfg",
        "nss",
        "sys0",
        "sys1",
        "wcssq6",
        "app0",
        "atcu0",
    },
    {   /*NSSNOC*/
        "nssnoc_cfg",
        "nssnoc_snoc",
        "ce",
        "eip197b",
        "haq",
        "ppe",
        "ubi32_c0",
        "ubi32_c1",
        "ubi32_c2",
        "ubi32_c3",
        "ubi32_nc0",
        "ubi32_nc1",
        "ubi32_nc2",
        "ubi32_nc3",
    },
    {   /*AGGRNOC*/
        "aggrnoc_cfg",
        "wcss",
        "pcie1_lane0",
        "pcie1_lane1",
        "pcie2_lane0",
        "pcie2_lane1",
        "usb",
    },
};

const char* const NOC_Target[5][50] = {
    {   /*PCNOC*/
        "qhs0",
        "qhs10",
        "qhs1",
        "qhs2",
        "qhs3",
        "qhs4",
        "qhs5",
        "qhs6",
        "qhs7",
        "qhs8",
        "qhs9",
        "dcc_cfg",
        "snoc",
        "pcnoc",
        "tcu",
    },
    {   /*SNOC*/
        "apss_cfg",
        "group0",
        "lpass_cfg",
        "tme_cfg",
        "usb_cfg",
        "wcss_cfg",
        "memnoc0",
        "memnoc1",
        "pcnoc",
        "imem",
        "nssnoc",
        "srvc_snoc",
        "pcie1_lane0",
        "pcie1_lane1",
        "pcie2_lane0",
        "pcie2_lane1",
        "pcie_ep_cfg",
        "qdss_stm",
    },
    {   /*MEMNOC*/
        "dbhn_sch0",
        "dtb_reg",
        "memnoc_mpu_sch0_cfg",
        "memnoc_xpu_cfg",
        "snoc",
        "srvc_memnoc",
    },
    {   /*NSSNOC*/
        "ce_cfg",
        "haq_cfg",
        "imem_cfg",
        "nss_csr",
        "nsscc_cfg",
        "ppe_cfg",
        "ubi32_cfg0",
        "ubi32_cfg1",
        "ubi32_cfg2",
        "ubi32_cfg3",
        "ubi32_int0",
        "ubi32_int1",
        "ubi32_int2",
        "ubi32_int3",
        "clc",
        "imem",
        "nss_memnoc",
        "ubi32_tcm0",
        "ubi32_tcm1",
        "ubi32_tcm2",
        "ubi32_tcm3",
        "srvc_nssnoc"
        "eip197b_cfg",
    },
    {   /*AGGRNOC*/
        "snoc0",
        "snoc2",
        "srvc_aggrnoc",
    },
};

const char* const NOC_ErrCode[] = {
    "Slave error",
    "Decode error",
    "Unsupported access error",
    "Disconnect error",
    "Security error",
    "Hidden security error",
    "Time-out error"
};

void NOC_decode_target_initiator(uint32_t ErrLog1_Low, char *NOC_Type)
{
    if(!strcmp(NOC_Type,"PCNOC"))
    {
    NOC_decode_info.Target_Index = ((ErrLog1_Low) & (0xF)); /* 0-3 bits*/
    NOC_decode_info.Initiator_Index = (((ErrLog1_Low) >> 4) & (0xF)); /* 4-7 bits*/
    tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : Initiator = 0x%x - %s, Target = 0x%x - %s", NOC_Type, NOC_decode_info.Initiator_Index, NOC_Initiator[0][NOC_decode_info.Initiator_Index], NOC_decode_info.Target_Index, NOC_Target[0][NOC_decode_info.Target_Index]);
    }

    if(!strcmp(NOC_Type,"SNOC"))
    {
    NOC_decode_info.Target_Index = ((ErrLog1_Low) & (0x1F)); /* 0-4 bits*/
    NOC_decode_info.Initiator_Index = (((ErrLog1_Low) >> 5) & (0xF)); /* 5-8 bits*/
    tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : Initiator = 0x%x - %s, Target = 0x%x - %s", NOC_Type, NOC_decode_info.Initiator_Index, NOC_Initiator[1][NOC_decode_info.Initiator_Index], NOC_decode_info.Target_Index, NOC_Target[1][NOC_decode_info.Target_Index]);
    }

    if(!strcmp(NOC_Type,"MEMNOC"))
    {
    NOC_decode_info.Target_Index = ((ErrLog1_Low) & (0x7)); /* 0-2 bits*/
    NOC_decode_info.Initiator_Index = (((ErrLog1_Low) >> 3) & (0x7)); /* 3-5 bits*/
    tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : Initiator = 0x%x - %s, Target = 0x%x - %s", NOC_Type, NOC_decode_info.Initiator_Index, NOC_Initiator[2][NOC_decode_info.Initiator_Index], NOC_decode_info.Target_Index, NOC_Target[2][NOC_decode_info.Target_Index]);
    }

    if(!strcmp(NOC_Type,"NSSNOC"))
    {
    NOC_decode_info.Target_Index = ((ErrLog1_Low) & (0x1F)); /* 0-4 bits*/
    NOC_decode_info.Initiator_Index = (((ErrLog1_Low) >> 5) & (0xF)); /* 5-8 bits*/
    tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : Initiator = 0x%x - %s, Target = 0x%x - %s", NOC_Type, NOC_decode_info.Initiator_Index, NOC_Initiator[3][NOC_decode_info.Initiator_Index], NOC_decode_info.Target_Index, NOC_Target[3][NOC_decode_info.Target_Index]);
    }

    if(!strcmp(NOC_Type,"AGGNOC"))
    {
    NOC_decode_info.Target_Index = ((ErrLog1_Low) & (0x3)); /* 0-1 bits*/
    NOC_decode_info.Initiator_Index = (((ErrLog1_Low) >> 2) & (0x7)); /* 2-4 bits*/
    tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : Initiator = 0x%x - %s, Target = 0x%x - %s", NOC_Type, NOC_decode_info.Initiator_Index, NOC_Initiator[4][NOC_decode_info.Initiator_Index], NOC_decode_info.Target_Index, NOC_Target[4][NOC_decode_info.Target_Index]);
    }
}

void NOC_decode_violation_addr(uint32_t ErrLog2_Low, char *NOC_Type)
{
    tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : Address of Violation = 0x%x", NOC_Type, ErrLog2_Low);
}

void NOC_decode_id(uint32_t ErrLog1_High, char *NOC_Type)
{
    NOC_decode_info.bid = (((ErrLog1_High) >> 13) & (0x7)); /* 13-15 bits*/
    NOC_decode_info.pid = (((ErrLog1_High) >> 8) & (0x1F)); /* 8-12 bits*/
    NOC_decode_info.mid = ((ErrLog1_High) & (0xFF));    /* 0-7 bits*/

    tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : BID = 0x%x, PID = 0x%x, MID = 0x%x", NOC_Type, NOC_decode_info.bid, NOC_decode_info.pid, NOC_decode_info.mid);
}

void NOC_decode_errcode(uint32_t ErrLog0_Low, char *NOC_Type)
{
    NOC_decode_info.errcode = (((ErrLog0_Low) >> 8) & (0x7));   /* 8-10 bits */
    NOC_decode_info.log_info_vld = ((ErrLog0_Low) & (0x1));     /* bit 0 */
    if (!NOC_decode_info.log_info_vld)
        tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : Target Type = Minimal Debug Target", NOC_Type);
    else
        {
            tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : ErrCode = %s", NOC_Type, NOC_ErrCode[NOC_decode_info.errcode]);
        }
}

void NOC_error_decode(NOCERR_info_type *NOCERRinfo)
{
    if (NOCERRinfo == NULL)
        return;
    memset(&NOC_decode_info, 0, sizeof(NOC_decode_data_type));
    NOC_decode_id(NOCERRinfo->syndrome.ERRLOG1_HIGH, NOCERRinfo->name);
    NOC_decode_target_initiator(NOCERRinfo->syndrome.ERRLOG1_LOW, NOCERRinfo->name);
    NOC_decode_errcode(NOCERRinfo->syndrome.ERRLOG0_LOW, NOCERRinfo->name);
    NOC_decode_violation_addr(NOCERRinfo->syndrome.ERRLOG2_LOW, NOCERRinfo->name);
}

