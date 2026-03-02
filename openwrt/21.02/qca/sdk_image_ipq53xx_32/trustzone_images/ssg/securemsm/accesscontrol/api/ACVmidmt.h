#ifndef TZBSP_VMIDMT_H
#define TZBSP_VMIDMT_H

/*===========================================================================
  Copyright (c) 2012, 2019, 2021 - 2023
  Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/ssg.tzw/1.2/securemsm/accesscontrol/api/ACVmidmt.h#6 $
  $DateTime: 2023/01/06 11:04:28 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  01/04/23   sp      Added QDSS VMID for 53xx.
  12/08/22   sp      Updated QDSP VMID for 53xx.
  08/08/22   sp      Added VMIDs for 53xx as required.
  07/17/21   sg      [90xx-Emulation] Added VMID for TME
  05/21/21   as      Updated VMID for PCIE
  06/10/19   gp      Update VMIDs for USB and LPASS.
  02/26/19   gp      Configure SMMU Stage 2.
  01/10/19   gp      Configure for Stage 2 bypass
  05/18/12   sg      First version.
  ============================================================================*/

#include <HALvmidmt.h>
#include <stdbool.h>
#include "ACCommon.h"
#include "tz_syscall_pub.h"

/* VMID can't be more than 16bits because of lack of support in S2VMID conversion */
#define TZBSP_VMID_NOACCESS     0
#define TZBSP_VMID_VMID_0       0
#define TZBSP_VMID_RPM          1
#define TZBSP_VMID_TZ           2
#define TZBSP_VMID_AP           3
#define TZBSP_VMID_TME          4
/* FIXME_53xx, FIXME_90xx:
 * Review the below VMID assignments and clean up/remove unused VMIDs
 * VMID values need to match with VMIDs defined in ssg/securemsm/accesscontrol/api/ACCommon.h
 */
#ifndef CHIPSET_53xx
#define TZBSP_VMID_USB3_0  	4
#else
#define TZBSP_VMID_USB3_0       5
#endif
#define TZBSP_VMID_USB2_0       5
#define TZBSP_VMID_PCIE0        6
#define TZBSP_VMID_LPASS        7

#ifndef CHIPSET_53xx
#define TZBSP_VMID_QDSP	        8
#define TZBSP_VMID_NSS          TZBSP_VMID_AP
#else
/* VMID value is fixed as 0 at H/W level for QDSP MSA in 53xx */
#define TZBSP_VMID_QDSP		TZBSP_VMID_VMID_0
#define TZBSP_VMID_NSS		9
#endif

#define TZBSP_VMID_CRYPTO       10
#define TZBSP_VMID_MSS          4
#define TZBSP_VMID_WLAN_CE      4
#define TZBSP_VMID_CP           6
#define TZBSP_VMID_VIDEO        7
#define TZBSP_VMID_ZAP_SHADER   8
#define TZBSP_VMID_CDSP         8
#define TZBSP_VMID_MDSS         9
#define TZBSP_VMID_WLAN_DSP     10
#define TZBSP_VMID_SSC          11
#define TZBSP_VMID_QPIC         12

#ifndef CHIPSET_53xx
#define TZBSP_VMID_BLSP         TZBSP_VMID_AP
#else
#define TZBSP_VMID_BLSP         13
#endif

#define TZBSP_VMID_SDCC         14

/* These values were obtained by the IPA team, and are documented in the
 ** 'IPAv2 Pipes - Istari SMMU VMID configuration' spreadsheet
 ** these shouldn't be changed as these are used by IPA to generate SID */
#define TZBSP_VMID_IPA_AP       (0b1100) /* 12 */
#define TZBSP_VMID_IPA_UC_PIPE  (0b1101) /* 13 */
#define TZBSP_VMID_IPA_WIFI     TZBSP_VMID_IPA_UC_PIPE
#define TZBSP_VMID_IPA_UC       (0b1110) /* 14*/

/* 9x35, 9x45 IPA VMID need special care as per below description from IPA h/w team.
 **
 ** IPA uC handles power collapse save and restore producer. Without XPU and VMIDMT 
 ** retention capability in Elessar IPA uC restores their state. 
 ** This means that uC firmware could use any VMID value (by reprogramming the VMIDMT).
 ** Thus security team required that IPA HW always sets VMID bit 4 as high so that it will
 ** not be able to use �regular� system VMIDs. (same IPA impl was carried over to 9x45)
 */
#define TZBSP_VMID_IPA_AP_MDM       0x13 /* BIT 4 SET | TZBSP_VMID_AP */
#define TZBSP_VMID_IPA_UC_PIPE_MDM  0x14 /* BIT 4 SET | TZBSP_VMID_MSS */
#define TZBSP_VMID_IPA_UC_MDM       0x15 /* BIT 4 SET & new VMID */

#define TZBSP_VMID_WLAN         15
#define TZBSP_VMID_WCSS         TZBSP_VMID_WLAN
#define TZBSP_VMID_PCIE1        16
#define TZBSP_VMID_PCIE2        17
#define TZBSP_VMID_PCIE3        18
#define TZBSP_VMID_WCSS_EDPD    20
#define TZBSP_VMID_WCSS_CE      21

/* For Napali QUPv3, there is a GSI mode for HLOS and a normal mode
 * The GSI mode has a different SID
 * Also on Napali v1 and possibly v2 we have TZ VMID programmed in HW
 * as 0x0 and it cannot be configured, hence we will have to handle it specially with the below
 * new VMID
 */
#define TZBSP_VMID_AP_GSI       22
#define TZBSP_VMID_QDSS         23
#define TZBSP_VMID_QUP_TZ       0x0

#define TZBSP_VMID_NOACCESS_BIT     (1<<TZBSP_VMID_NOACCESS)
#define TZBSP_VMID_VMID_0_BIT       (1<<TZBSP_VMID_VMID_0)
#define TZBSP_VMID_TZ_BIT           (1<<TZBSP_VMID_TZ)
#define TZBSP_VMID_TME_BIT          (1<<TZBSP_VMID_TME)
#define TZBSP_VMID_RPM_BIT          (1<<TZBSP_VMID_RPM)
#define TZBSP_VMID_LPASS_BIT        (1<<TZBSP_VMID_LPASS)
#define TZBSP_VMID_MSS_BIT          (1<<TZBSP_VMID_MSS)
#define TZBSP_VMID_WLAN_CE_BIT      (1<<TZBSP_VMID_WLAN_CE)
#define TZBSP_VMID_AP_BIT           (1<<TZBSP_VMID_AP)
#define TZBSP_VMID_USB3_0_BIT       (1<<TZBSP_VMID_USB3_0)
#define TZBSP_VMID_NSS_BIT          (1<<TZBSP_VMID_NSS)
#define TZBSP_VMID_CP_BIT           (1<<TZBSP_VMID_CP)
#define TZBSP_VMID_VIDEO_BIT        (1<<TZBSP_VMID_VIDEO)
#define TZBSP_VMID_CDSP_BIT         (1<<TZBSP_VMID_CDSP)
#define TZBSP_VMID_MDSS_BIT         (1<<TZBSP_VMID_MDSS)
#define TZBSP_VMID_SSC_BIT          (1<<TZBSP_VMID_SSC)
#define TZBSP_VMID_PCIE0_BIT        (1<<TZBSP_VMID_PCIE0)
#define TZBSP_VMID_PCIE1_BIT        (1<<TZBSP_VMID_PCIE1)
#define TZBSP_VMID_PCIE2_BIT        (1<<TZBSP_VMID_PCIE2)
#define TZBSP_VMID_WCSS_EDPD_BIT    (1<<TZBSP_VMID_WCSS_EDPD)
#define TZBSP_VMID_WCSS_CE_BIT      (1<<TZBSP_VMID_WCSS_CE)

#define TZBSP_VMID_IPA_AP_BIT       (1 << TZBSP_VMID_IPA_AP)
#define TZBSP_VMID_IPA_UC_PIPE_BIT  (1 << TZBSP_VMID_IPA_UC_PIPE)
#define TZBSP_VMID_IPA_UC_BIT       (1 << TZBSP_VMID_IPA_UC)

#define TZBSP_VMID_IPA_AP_MDM_BIT       (1 << TZBSP_VMID_IPA_AP_MDM)
#define TZBSP_VMID_IPA_UC_PIPE_MDM_BIT  (1 << TZBSP_VMID_IPA_UC_PIPE_MDM)
#define TZBSP_VMID_IPA_UC_MDM_BIT       (1 << TZBSP_VMID_IPA_UC_MDM)

#define TZBSP_VMID_WLAN_BIT         (1<<TZBSP_VMID_WLAN)
#define TZBSP_VMID_ZAP_SHADER_BIT   (1<<TZBSP_VMID_ZAP_SHADER)
#define TZBSP_VMID_IPA_WIFI_BIT     (1<<TZBSP_VMID_IPA_WIFI)
#define TZBSP_VMID_WLAN_DSP_BIT      (1<<TZBSP_VMID_WLAN_DSP)

#define TZBSP_VMID_QDSP_BIT         (1<<TZBSP_VMID_QDSP)
#define TZBSP_VMID_QPIC_BIT         (1<<TZBSP_VMID_QPIC)
#define TZBSP_VMID_BLSP_BIT         (1<<TZBSP_VMID_BLSP)
#define TZBSP_VMID_SDCC_BIT         (1<<TZBSP_VMID_SDCC)
#define TZBSP_VMID_QDSS_BIT         (1<<TZBSP_VMID_QDSS)
#define TZBSP_ALL_VMID              ((~TZBSP_VMID_NOACCESS) & (~TZBSP_DOMAIN_MASK))


/* Domain definitions. Start downwards from 31 so it doesn't conflict with VMIDs */
#define TZBSP_DOMAIN_MASK 0xFF000000 /* Reserve 8 bits for domains */
#define TZBSP_DOMAIN_MSA   31
#define TZBSP_DOMAIN_HYP   30
#define TZBSP_DOMAIN_SP    29
#define TZBSP_DOMAIN_TZ    28


/* Domain definitions */
#define TZBSP_DOMAIN_MSA_BIT   (1U << TZBSP_DOMAIN_MSA)
#define TZBSP_DOMAIN_HYP_BIT   (1U << TZBSP_DOMAIN_HYP)
#define TZBSP_DOMAIN_SP_BIT    (1U << TZBSP_DOMAIN_SP)
#define TZBSP_DOMAIN_TZ_BIT    (1U << TZBSP_DOMAIN_TZ)


/**
 * API to map a vmid. Allows only specific instances to be remapped
 *
 * @param [in] master VMIDMT instance
 * @param [in] index  Index for which VMID has to be mapped
 * @param [in] vmid   VMID to be mapped
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
int tzbsp_vmidmt_map_vmid(HAL_vmidmt_InstanceType master,
        uint8 index, uint32 vmid, bool secure);

/**
 * API to map a vmid. Allows only specific instances to be remapped
 *
 * @param [in] eVmidmtInstance  VMIDMT instance
 * @param [in] pInternalSidList List of internal SIDs to be mapped 
 * @param [in] vmid   VMID to be mapped to 
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
AC_ERROR_CODE ACMapSidToVmidmt(HAL_vmidmt_InstanceType eVmidmtInstance,
        uint32 *pInternalSidList,
        uint32 uNumInternalSids,
        ACVirtualMachineId  uDestinationVm);

#endif /* TZBSP_VMIDMT_H */
