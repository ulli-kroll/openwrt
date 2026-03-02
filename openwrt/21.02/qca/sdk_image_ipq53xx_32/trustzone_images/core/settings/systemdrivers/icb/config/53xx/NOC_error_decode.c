/*==============================================================================
Copyright (c) 2021, 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================
FILE:      NOC_error_decode.c

DESCRIPTION: This file implements NOC Error Decoding.
==============================================================================*/
#include "NOC_error_decode.h"

NOC_decode_data_type NOC_decode_info;

const char* const NOC_Initiator[4][50] = {
    {   /*PCNOC*/
        "qhm0",
        "pcnoc_cfg",
        "tic",
	"snoc",
        "lpass",
        "qdss_dap",
        "sdcc",
	"RESERVED",
    },
    {   /*SNOC*/
        "pcie_ep",
        "qdss_bam",
        "snoc_cfg_0",
        "tme",
        "memnoc",
	"nssnoc",
        "pcnoc",
	"wcss",
	"edpd",
	"ce",
	"pcie_g3x1_0",
	"pcie_g3x1_1",
	"pcie_g3x2",
        "qdss_etr",
	"usb",
	"RESERVED",
    },
    {   /*MEMNOC*/
        "memnoc_cfg",
        "sys0",
        "sys1",
        "wcssq6",
        "app0",
	"RESERVED",
	"RESERVED",
	"RESERVED",
    },
    {   /*NSSNOC*/
        "nssnoc_cfg",
        "nssnoc_snoc",
        "ce",
        "eip196a",
	"ppe",
	"RESERVED",
	"RESERVED",
	"RESERVED",
    },
};

const char* const NOC_Target[4][50] = {
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
        "snoc",
	"edpd",
        "pcnoc",
        "RESERVED",
	"RESERVED",
    },
    {   /*SNOC*/
        "apss_cfg",
        "group0",
        "lpass_cfg",
	"q6_mst_xpu_cfg",
        "tme_cfg",
        "usb_cfg",
        "wcss_cfg",
        "memnoc0",
        "memnoc1",
        "nssnoc",
        "pcnoc",
	"wcss",
        "imem",
	"q6",
        "srvc_snoc",
        "pcie_ep_cfg",
        "pcie_g3x1_0",
        "pcie_g3x1_1",
        "pcie_g3x2",
        "qdss_stm",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
	"RESERVED",
    },
    {   /*MEMNOC*/
        "dbhn_sch0",
        "dtb_reg",
        "memnoc_mpu_sch0_cfg",
        "memnoc_xpu_cfg",
        "qxs_snoc",
        "srvc_memnoc",
	"RESERVED",
	"RESERVED",
    },
    {   /*NSSNOC*/
        "ce_cfg",
        "nss_csr",
        "nsscc_cfg",
	"ppe_cfg",
	"nssnoc_snoc",
	"srvc_nssnoc",
	"eip196a_cfg",
	"RESERVED",
    },
};

const char* const NOC_ErrCode[] = {
    "Slave error",
    "Decode error",
    "Unsupported access error",
    "Disconnect error",
    "Security error",
    "Hidden security error",
    "Time-out error",
    "Tag match operation failed",
};

void NOC_decode_target_initiator(uint32_t ErrLog1_Low, char *NOC_Type)
{
    if(!strcmp(NOC_Type,"PCNOC"))
    {
    NOC_decode_info.Target_Index = ((ErrLog1_Low) & (0xF)); /* 0-3 bits*/
    NOC_decode_info.Initiator_Index = (((ErrLog1_Low) >> 4) & (0x7)); /* 4-6 bits*/
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
    NOC_decode_info.Target_Index = ((ErrLog1_Low) & (0x7)); /* 0-2 bits*/
    NOC_decode_info.Initiator_Index = (((ErrLog1_Low) >> 3) & (0x7)); /* 3-5 bits*/
    tzbsp_log(TZBSP_MSG_ERROR ,"%s ERROR Decode : Initiator = 0x%x - %s, Target = 0x%x - %s", NOC_Type, NOC_decode_info.Initiator_Index, NOC_Initiator[3][NOC_decode_info.Initiator_Index], NOC_decode_info.Target_Index, NOC_Target[3][NOC_decode_info.Target_Index]);
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

