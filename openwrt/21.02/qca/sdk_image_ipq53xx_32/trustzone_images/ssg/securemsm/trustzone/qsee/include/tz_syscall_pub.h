#ifndef TZ_SYSCALL_PUB_H
#define TZ_SYSCALL_PUB_H

/**
   @file tz_syscall_pub.h
   @brief Provide the SYSCALL API infrastructure
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The TZ_SysCall_API_mainpage.dox file contains all descriptions that
      are displayed in the output PDF generated using Doxygen and LaTeX. To
      edit or update any of the file/group text in the PDF, edit the
      TZ_SysCall_API_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
   Copyright (c) 2010-2023 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*
Copyright (c) 2021,2022 The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*===========================================================================

                            EDIT HISTORY FOR FILE


when       who      what, where, why
--------   ---      ------------------------------------
08/29/23   as       Added support for image segment decompression feature
08/17/23   mp       Added support for key derivation for HLOS
07/19/23   as       Added support for Rootfs enc/dec using EMMC ICE.
                    Syscall to derive and distribute oem product seed
                    based key for EMMC ICE.
05/17/23   vd       Support for TME fuse read multiple syscall
05/05/23   sp       Update tzbsp_mode_switch description
03/31/23   vd       Changes for pebble code sharing
02/14/23   jpr      Support for QWES Bindigs check IPC call
02/03/23   mp       Added support for rootfs metadata auth
01/31/23   hk       Support for enc/dec syscall and key clear syscall for SEC NAND FR
01/12/23   abs      Support for wcss syscall in Miami
11/03/22   vd       Support for QWES DP IPC call
09/26/22   vd       Support for QWES DA IPC calls
09/14/22   vd       Added support for DPR authentication in IPQ53xx
06/28/22   as       Implemented SMCCC_ARCH_WORKAROUND_3 on Alder
08/02/22   jv	    Updated TME auth SCM call parameters
07/04/22   as       Key Clear syscall support
03/24/22   as       Adding tz diag encryption support
03/02/22   as       AES Enc/DEC using the OEM Prod seed/CRBK derived keys
02/23/22   abisrs   Secure boot disable auto increment failure
11/30/21   as       Enabled DPR ipc support
09/08/21   as       ARB version support
04/07/21   mp       Added support for bootconfig params
03/02/20   sg       [90xx-Emulation] TMEL FW Auth
06/01/20   as       Support  for TLS Hardening
05/14/20   mp       Copyright update
11/21/19   gp       Added smc id for syscall to get SMMU Status
07/11/19   sure     Added smc syscall for secure nand storage
07/03/19   sure     Added smc syscall for software version read.(Cypress)
07/02/19   as       Scm call support for ICE h/w key ctrl config
06/21/19   gp       syscalls for Hyp Diag Region
06/17/19   as       Scm call support for Q6 wcss and lpass debug
05/06/19   mp       syscalls for HLOS secboot for cypress
03/26/19   gp       New smc handler for blowing fuses using secdat
01/29/19   dpk      Added support for SCm call from HYP QUP AC driver to TZ QUP AC driver. 
09/05/17   nbg      Adding PSCI Memory Protect APIs (part of spec version 1.1)
07/10/17   dr       Add secure camera syscall IDs
07/24/17   skare    Added paralell hashing and ECC verify syscalls
04/24/17   ai       Add UEFI FV Proc ID
10/06/16   ab       Added bandwidth profiling support.
04/18/16   dks      Updated QDSS generic Syscall.
04/04/16   fm       Fix param IDs for nfdbg APIs and is_retail_unlock_enable
03/22/16   nbg      Added TZ_PSCI_SET_SUSPEND_MODE syscall.
12/08/15   jt       Added command to check RPMB provisioning status
11/06/15   dks      Added QDSS tracer filter and query status commands.
06/24/15   sk       Added ICE syscalls for context-cfg and disable
05/18/15   dp       Propagate edited Doxygen comments and markup from Tech Pubs.
03/03/15   jc       Added POWER_BUS_BW syscall.
12/26/14   da       Added LMh driver syscalls.
09/01/14   ab       Added MDTP syscall.
01/25/14   hw       add new tz api to get device's security state
05/08/13   leo      Edited Doxygen comments and markup for Family B updates.
03/14/13   leo      (Tech Pubs) Edited/added doxygen comments and markup.
05/05/13   as       Added Enterprise APIs.
12/12/12   rkk      Added lock/map/umap v2 APIs.
05/29/12   rkk      Added syscall for lock/map AMT chunks, page table init.
05/18/12   sg       Added syscall to restore sec cfg for a device
03/08/12   leo      (Tech Pubs) Edited/added Doxygen comments and markup.
09/12/11   kpa      Added services to support Version rollback prevention
08/20/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <stdint.h>
#include <stddef.h>

/*----------------------------------------------------------------------------
 * Documentation
 * -------------------------------------------------------------------------*/

/*   ARMV8 SMC ID bit layout
 ___________________________________________________
|    |     |        |          |          |         |
| 31 |  30 |29----24|23------16|15-------8|7-------0|
|IRQ |AARCH|  Owner | Reserved |Service ID|  Func # |
|____|_____|________|__________|__________|_________|
                               \_________  _________/
                                         \/
                                     Command ID
*/

/*   Parameter ID bit layout
 ___________________________________________________________________________
|          |     |     |     |     |     |     |     |    |    |    |       |
|31------24|23-22|21-20|19-18|17-16|15-14|13-12|11-10|9--8|7--6|5--4|3-----0|
| Reserved | p10 | p9  | p8  | p7  | p6  | p5  | p4  | p3 | p2 | p1 | nargs |
|          |type |type |type |type |type |type |type |type|type|type|       |
|__________|_____|_____|_____|_____|_____|_____|_____|____|____|____|_______|

*/

/** @addtogroup tz_macros_constants
@{ */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/** @name Helper Macros
@{ */
/** General helper macro to create a bitmask from bits low to high. */
#define TZ_MASK_BITS(h,l)     ((0xffffffff >> (32 - ((h - l) + 1))) << l)

/** Helper macro to extract the IRQ settings from the SMC ID. */
#define TZ_SYSCALL_IRQ(r0)        ((r0 & TZ_MASK_BITS(31,31)) >> 31)
/** Helper macro to extract the AArch used from the SMC ID. */
#define TZ_SYSCALL_AARCH(r0)      ((r0 & TZ_MASK_BITS(30,30)) >> 30)
/** Helper macro to extract the owning entity from the SMC ID. */
#define TZ_SYSCALL_OWNER_ID(r0)   ((r0 & TZ_MASK_BITS(29,24)) >> 24)
/** Helper macro to extract the reserved bits from the SMC ID. */
#define TZ_SYSCALL_RESERVED(r0)   ((r0 & TZ_MASK_BITS(23,16)) >> 16)
/** Helper macro to extract the function ID from the SMC ID. */
#define TZ_SYSCALL_FUNC_ID(r0)    (r0 & TZ_MASK_BITS(15,0))
/** Helper macro to extract the application ID from the SMC ID. */
#define TZ_SYSCALL_APP_ID(r0)     ((r0 & TZ_MASK_BITS(15,8)) >> 8)
/** Helper macro to extract the service ID from the function ID in the SMC ID. */
#define TZ_SYSCALL_SVC_ID(r0)     ((r0 & TZ_MASK_BITS(15,8)) >> 8)

/** Helper macro to extract the number of arguments from the parameter ID. */
#define TZ_SYSCALL_NUM_ARGS(r1)   (r1 & TZ_MASK_BITS(3,0))
/** Helper macro to extract an argument type from the parameter ID; maximum 10 arguments. */
#define TZ_SYSCALL_ARG_TYPE(r1, n) \
  ((n<10)?((r1 & TZ_MASK_BITS(((n*2)+5),((n*2)+4))) >> ((n * 2) + 4)):0xFFFFFFFF)

/** Helper macro for checking whether an owning entity is of type trusted OS. */
static inline boolean IS_OWNER_TRUSTED_OS(uint32_t owner_id)
{
  return ((owner_id >= 50) && (owner_id <= 63));
}

/** Helper macro for checking whether an owning entity is of type TZ APPS. */
#define IS_OWNER_TZ_APPS(owner_id) \
  (((owner_id >= 48) && (owner_id <= 49))?TRUE:FALSE)
/** Helper macro for checking whether an owning entity is of type TEE. */
#define IS_OWNER_TRUSTED_TEE(owner_id) \
  (((owner_id == 52))?TRUE:FALSE)

/**
   Macro used to define an SMC ID based on the owner ID,
   service ID, and function number.
*/
#define TZ_SYSCALL_CREATE_SMC_ID(o, s, f) \
  ((uint32)((((o & 0x3f) << 24 ) | (s & 0xff) << 8) | (f & 0xff)))

/** Parameter ID nargs bitmask. */
#define TZ_SYSCALL_PARAM_NARGS_MASK  TZ_MASK_BITS(3,0)
/** Parameter ID paramameter type bitmask. */
#define TZ_SYSCALL_PARAM_TYPE_MASK   TZ_MASK_BITS(1,0)

/** Internal helper macro for __TZ_SYSCALL_CREATE_PARAM_ID. */
#define _TZ_SYSCALL_CREATE_PARAM_ID(nargs, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, ...) \
  (((nargs)&TZ_SYSCALL_PARAM_NARGS_MASK)+ \
  (((p1)&TZ_SYSCALL_PARAM_TYPE_MASK)<<4)+ \
  (((p2)&TZ_SYSCALL_PARAM_TYPE_MASK)<<6)+ \
  (((p3)&TZ_SYSCALL_PARAM_TYPE_MASK)<<8)+ \
  (((p4)&TZ_SYSCALL_PARAM_TYPE_MASK)<<10)+ \
  (((p5)&TZ_SYSCALL_PARAM_TYPE_MASK)<<12)+ \
  (((p6)&TZ_SYSCALL_PARAM_TYPE_MASK)<<14)+ \
  (((p7)&TZ_SYSCALL_PARAM_TYPE_MASK)<<16)+ \
  (((p8)&TZ_SYSCALL_PARAM_TYPE_MASK)<<18)+ \
  (((p9)&TZ_SYSCALL_PARAM_TYPE_MASK)<<20)+ \
  (((p10)&TZ_SYSCALL_PARAM_TYPE_MASK)<<22))

/** Internal helper macro for TZ_SYSCALL_CREATE_PARAM_ID_X */
#define __TZ_SYSCALL_CREATE_PARAM_ID(...) \
  _TZ_SYSCALL_CREATE_PARAM_ID(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

/** Macro used to create a parameter ID with no arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_0 \
  __TZ_SYSCALL_CREATE_PARAM_ID(0)
/** Macro used to create a parameter ID with one argument. */
#define TZ_SYSCALL_CREATE_PARAM_ID_1(p1) \
  __TZ_SYSCALL_CREATE_PARAM_ID(1, p1)
/** Macro used to create a parameter ID with two arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_2(p1, p2) \
  __TZ_SYSCALL_CREATE_PARAM_ID(2, p1, p2)
/** Macro used to create a parameter ID with three arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_3(p1, p2, p3) \
  __TZ_SYSCALL_CREATE_PARAM_ID(3, p1, p2, p3)
/** Macro used to create a parameter ID with four arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_4(p1, p2, p3, p4) \
  __TZ_SYSCALL_CREATE_PARAM_ID(4, p1, p2, p3, p4)
/** Macro used to create a parameter ID with five arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_5(p1, p2, p3, p4, p5) \
  __TZ_SYSCALL_CREATE_PARAM_ID(5, p1, p2, p3, p4, p5)
/** Macro used to create a parameter ID with six arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_6(p1, p2, p3, p4, p5, p6) \
  __TZ_SYSCALL_CREATE_PARAM_ID(6, p1, p2, p3, p4, p5, p6)
/** Macro used to create a parameter ID with seven arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_7(p1, p2, p3, p4, p5, p6, p7) \
  __TZ_SYSCALL_CREATE_PARAM_ID(7, p1, p2, p3, p4, p5, p6, p7)
/** Macro used to create a parameter ID with eight arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_8(p1, p2, p3, p4, p5, p6, p7, p8) \
  __TZ_SYSCALL_CREATE_PARAM_ID(8, p1, p2, p3, p4, p5, p6, p7, p8)
/** Macro used to create a parameter ID with nine arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_9(p1, p2, p3, p4, p5, p6, p7, p8, p9) \
  __TZ_SYSCALL_CREATE_PARAM_ID(9, p1, p2, p3, p4, p5, p6, p7, p8, p9)
/** Macro used to create a parameter ID with ten arguments. */
#define TZ_SYSCALL_CREATE_PARAM_ID_10(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) \
  __TZ_SYSCALL_CREATE_PARAM_ID(10, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/** @} */ /* end_namegroup Macros to Create the Parameter ID */

/*----------------------------------------------------------------------------
 * Parameter ID values
 * -------------------------------------------------------------------------*/
/** @name Parameter ID Values
@{ */

/** A parameter of type value. */
#define TZ_SYSCALL_PARAM_TYPE_VAL              0x0
/** A parameter of type buffer read only. */
#define TZ_SYSCALL_PARAM_TYPE_BUF_RO           0x1
/** A parameter of type buffer read-write. */
#define TZ_SYSCALL_PARAM_TYPE_BUF_RW           0x2

/** @} */ /* end_namegroup Parameter ID values */

/*----------------------------------------------------------------------------
 * Owning Entity IDs (defined by ARM SMC doc)
 * -------------------------------------------------------------------------*/

/** @name Owning Entity IDs
 @{ */

/** ARM architecture call ID. */
#define TZ_OWNER_ARM                     0
/** CPU service call ID. */
#define TZ_OWNER_CPU                     1
/** SIP service call ID. */
#define TZ_OWNER_SIP                     2
/** OEM service call ID. */
#define TZ_OWNER_OEM                     3
/** Standard service call ID. */
#define TZ_OWNER_STD                     4

/* Values 5-47 are reserved for future use */

/* Trusted Application call IDs */
#define TZ_OWNER_TZ_APPS                 48
#define TZ_OWNER_TZ_APPS_RESERVED        49
/* Trusted OS Call IDs */
#define TZ_OWNER_QSEE_OS                 50
#define TZ_OWNER_TEE_HLOS                51     /**< For calls to the TEE from the HLOS. */
#define TZ_OWNER_TEE                     52     /**< For calls to/from the TEE/QSEE.     */
#define TZ_OWNER_OS_RESERVED_4           53
#define TZ_OWNER_OS_RESERVED_5           54
#define TZ_OWNER_OS_RESERVED_6           55
#define TZ_OWNER_OS_RESERVED_7           56
#define TZ_OWNER_OS_RESERVED_8           57
#define TZ_OWNER_OS_RESERVED_9           58
#define TZ_OWNER_OS_RESERVED_10          59
#define TZ_OWNER_OS_RESERVED_11          60
#define TZ_OWNER_OS_RESERVED_12          61
#define TZ_OWNER_OS_RESERVED_13          62
#define TZ_OWNER_OS_RESERVED_14          63

/** @} */ /* end_namegroup Owning Entity IDs */

/*----------------------------------------------------------------------------
 * Service IDs
 * -------------------------------------------------------------------------*/

 /** @name Service IDs
 @{ */

/** Information call groups. */
#define TZ_SVC_INFO_TYPE_COUNT    0       /**< System call count information.    */
#define TZ_SVC_INFO_TYPE_UID      1       /**< System call UID information.      */
#define TZ_SVC_INFO_TYPE_VER      3       /**< System call revision information. */
#define TZ_SVC_INFO_CALL_ID       255

/** ARM Workaround call groups */
#define TZ_SVC_ARM_WA_1             128
#define TZ_SVC_ARM_WA_2             127
#define TZ_SVC_ARM_WA_3             63

/* ARM syscall info   */
/** ARM architecture call groups. */
#define TZ_SVC_ARM_INFO           0

/* CPU syscall info   */
/** CPU service call groups. */
#define TZ_SVC_CPU_INFO           TZ_SVC_INFO_CALL_ID

/* SIP service call groups */
#define TZ_SVC_BOOT               1       /**< Boot (cold boot/warm boot).    */
#define TZ_SVC_PIL                2       /**< Peripheral image loading.      */
#define TZ_SVC_DUMP               3       /**< Memory dumping.                */
#define TZ_SVC_RES_LOCKING        4       /**< Resource locking.              */
#define TZ_SVC_IO_ACCESS          5       /**< I/O protection.                */
#define TZ_SVC_INFO               6       /**< Miscellaneous information services. */
#define TZ_SVC_SSD                7       /**< SSD services.                  */
#define TZ_SVC_FUSE               8       /**< Fuse services.                 */
#define TZ_SVC_POWER              9       /**< Power related services.        */
#define TZ_SVC_CRYPTO             10      /**< Crypto services.               */
#define TZ_SVC_SC_KEY_EXCHANGE    11      /**< Secure channel key exchange.   */
#define TZ_SVC_MEMORY_PROTECTION  12      /**< Memory protection service.     */
#define TZ_SVC_RESERVED_0         13      /**< Reserved service ID.           */
#define TZ_SVC_NS_DEBUG_FIQ       14      /**< Nonsecure debug FIQ.           */
#define TZ_SVC_OCMEM              15      /**< OCMEM service.                 */
#define TZ_SVC_ES                 16      /**< Enterprise security service    */
#define TZ_SVC_HDCP               17      /**< HDCP service.                  */
#define TZ_SVC_MDTP               18      /**< MDTP service.                  */
#define TZ_SVC_RESERVED_1         19      /**< Reserved service ID.           */
#define TZ_SVC_READ_DBG_POLICY    20      /**< DBG policy content retrieval.  */
#define TZ_SVC_SMMU_PROGRAM       21      /**< SMMU services.                 */
#define TZ_SVC_QDSS               22      /**< QDSS services.                 */
#define TZ_SVC_ICE                23      /**< Inline crypto engine operations. */
#define TZ_SVC_CAMERASS           24      /**< Camera Subsytem services.      */
#define TZ_SVC_RTIC               25      /**< RTIC Service.                  */
#define TZ_SVC_STORAGE            26      /**< Storage Service .              */
#define TZ_SVC_PCIE               27      /**< PCIE service.                  */
//#define TZ_SVC_KDF                28      /**< SIP Service name changed to TZ_SVC_HYP. */
#define TZ_SVC_HYP                28      /**< TZ provide this service only to Hyp. */
#define TZ_SVC_GIC                29      /**< GIC                            */
#define TZ_SVC_QWES               30      /**< QWES service.                  */

#define TZ_SVC_MON_SAT            252     /**< Monitor SAT test calls.  */

#define TZ_SVC_TEST_1             253     /**< TZ test calls (continued).     */
#define TZ_SVC_TEST_0             254     /**< TZ test calls.                 */
#define TZ_SVC_SIP_INFO           TZ_SVC_INFO_CALL_ID /**< SIP syscall information. */

/* OEM service call groups */
#define TZ_SVC_OEM_INFO           TZ_SVC_INFO_CALL_ID /**< OEM service call information. */

/* Standards call groups */
#define TZ_SVC_PSCI               0
#define TZ_SVC_STANDARD_INFO      TZ_SVC_INFO_CALL_ID /**< Service standard information. */

/* Trusted OS: QSEE call groups */
#define TZ_SVC_APP_MGR            1       /**< Application service manager.   */
#define TZ_SVC_LISTENER           2       /**< Listener service manager.      */
#define TZ_SVC_EXTERNAL           3       /**< External image loading.        */
#define TZ_SVC_RPMB               4       /**< Replay protected memory block. */
#define TZ_SVC_KEYSTORE           5       /**< Keystore management.           */
#define TZ_SVC_SMC_INVOKE         6       /**< SMC Invoke. */
#define TZ_SVC_QSEE_OS_INFO       TZ_SVC_INFO_CALL_ID /**< Trusted OS information. */

/* Trusted OS: Third party call groups */
#define TZ_SVC_EXECUTIVE_EXT      			250     /**< HLOS group.           */
#define TZ_PVC_EXECUTIVE_TEE      			251     /**< TEE group.            */
#define TZ_RESERVED_FIELD_ARM7_TRUST_OS     255
#define TZ_RESERVED_FIELD_PVC_TEE           126

/* Trusted Application call groups */
#define TZ_SVC_APP_ID_PLACEHOLDER 0     /**< Service bits will be empty.        */

/** @} */ /* end_namegroup Service IDs */

/** @} */ /* end_addtogroup tz_macros_constants */

/** @addtogroup tz_datatypes
@{ */

 /** @name SMC Interface Error Codes Enumaration
 @{ */
typedef enum
{
  SMC_INTERRUPTED                   =  1,   /**< SMC call was interrupted.                */
  SMC_SUCCESS                       =  0,   /**< Success, requested syscall was called.   */
  SMC_ERR_UNKNOWN_SMC_ID            = -1L,  /**< ARM-defined error of 0xFFFFFFFF.         */
  SMC_ERR_SYSCALL_FAILED            = -2L,  /**< Syscall function returned failure.       */
  SMC_ERR_SYSCALL_NOT_AVAILABLE     = -3L,  /**< No function associated with the syscall definition. */
  SMC_ERR_RESERVED_FIELD_MBZ        = -4L,  /**< SMC ID error: reserved field must be zero. */
  SMC_ERR_NUM_ARGS_MISMATCH         = -5L,  /**< Number of arguments does not match the
                                                 function definition.                     */
  SMC_ERR_INDIRECT_PARAM_PTR_NOT_NS = -6L,  /**< Indirect parameter pointer does not
                                                 point to NS memory.                      */
  SMC_ERR_BUF_LEN_NOT_FOUND         = -7L,  /**< No buffer length following the buffer pointer. */
  SMC_ERR_BUF_PTR_NOT_NS            = -8L,  /**< Buffer pointer does not point to NS memory. */
  SMC_ERR_NO_MEMORY                 = -9L,  /**< Heap is out of space.                    */
  SMC_ERR_PARAM_ID_MISMATCH         = -10L, /**< Incorrect parameter ID from the NS world. */
  SMC_ERR_BUF_PTR_INVALID           = -11L, /**< Buffer pointer is in the first 4k (invalid) page. */
  SMC_ERR_BUSY                      = -12L, /**< TZ software is busy waiting for a listener response. */
  SMC_ERR_SESSION_ID_MISMATCH       = -13L, /**< Trusted OS session ID mismatch error.    */
  SMC_ERR_NOT_FOUND_IN_HYP          = -14L, /**< SMC call is not served in HYP.           */
  SMC_ERR_TEE_SERVICE_NOT_FOUND     = -15L, /**< SMC call is from a TEE, but the format is bad. */
  SMC_ERR_STANDARD_CALL_NOT_ALLOWED = -16L, /**< Pre-emptible calls are not allowed.      */

  /* ARMv8 SMC spec: Must return 0xFFFFFFFF for these errors */
  SMC_ERR_SYSCALL_NOT_SUPPORTED     = SMC_ERR_UNKNOWN_SMC_ID, /**< Syscall is not supported. */
  SMC_ERR_AARCH64_NOT_SUPPORTED     = SMC_ERR_UNKNOWN_SMC_ID, /**< SMC64 is not supported.   */

  SMC_ERR_FAILURE                   = -INT32_MAX   /**< Internal failure.                    */
} SmcErrnoType;

/** @} */ /* end_namegroup SMC Interface Error Codes Enumaration */

/** @} */ /* end_addtogroup tz_datatypes */

/*----------------------------------------------------------------------------
 * Command IDs
 * -------------------------------------------------------------------------*/

 #define TZ_MON_TEST_SAT_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_MON_SAT, 0x0)

 #define TZ_MON_TEST_SAT_ID_PARAM_ID  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/*----------------------------------------------------------------------------
                ARM Info calls
 * -------------------------------------------------------------------------*/
/**
   @ingroup arm_info

   Returns the version of SMCCC that is implemented.

   @smc_id
     0x80000000

   @return
     Version information: \n
       Bits [31:16] Major Version \n
       Bits [15:0] Minor Version
*/
 #define TZ_SMCCC_VERSION_SMC_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_ARM, TZ_SVC_ARM_INFO , 0x0)
  
 #define TZ_SMCCC_VERSION_PARAM_ID                  \
  TZ_SYSCALL_CREATE_PARAM_ID_0
 

/**
   @ingroup arm_info

   Returns whether a specific Arm Architecture Service function is implemented.
   i.e smccc_version (0x80000000) , smccc_arch_workaround_1 (0x80008000)

   @smc_id
     0x80000001

   @return
    SUCCESS 
    NOT_SUPPORTED
*/
 #define TZ_SMCCC_ARCH_FEATURES_SMC_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_ARM, TZ_SVC_ARM_INFO , 0x1)
  
 #define TZ_SMCCC_ARCH_FEATURES_PARAM_ID                  \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL) 

/**
   @ingroup arm_info

   Execute the mitigation for CVE-2017-5715 on the calling PE
   

   @smc_id
     0x8000 8000

   @return
    void
*/

 #define TZ_SMCCC_ARCH_WORKAROUND_1_SMC_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_ARM, TZ_SVC_ARM_WA_1 , 0x0) 

 #define TZ_SMCCC_ARCH_WORKAROUND_1_PARAM_ID                  \
  TZ_SYSCALL_CREATE_PARAM_ID_0
  
/**
   @ingroup arm_info

   Execute the mitigation for CVE-2018-3639 on the calling PE
   

   @smc_id
     0x8000 7FFF

   @return
    void
*/

 #define TZ_SMCCC_ARCH_WORKAROUND_2_SMC_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_ARM, TZ_SVC_ARM_WA_2 , 0xFF) 

 #define TZ_SMCCC_ARCH_WORKAROUND_2_PARAM_ID                  \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)   

/**
   @ingroup arm_info

   Execute SMCCC_ARCH_WORKAROUND_3 on the calling PE


   @smc_id
     0x8000 3FFF

   @return
    void
*/

 #define TZ_SMCCC_ARCH_WORKAROUND_3_SMC_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_ARM, TZ_SVC_ARM_WA_3 , 0x0)

 #define TZ_SMCCC_ARCH_WORKAROUND_3_PARAM_ID                  \
  TZ_SYSCALL_CREATE_PARAM_ID_0
 
/*----------------------------------------------------------------------------
                PSCI Command IDs: PSCI v1.0 specific syscall.
 * -------------------------------------------------------------------------*/
/**
   @ingroup psci_version_info

     Returns the version of PSCI that is implemented.

   @smc_id
     0xC4000000

   @return
     Version information: \n
       Bits [31:16] Major Version \n
       Bits [15:0] Minor Version
*/
#define TZ_PSCI_VERSION_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x0)

/**
   @ingroup psci_cpu_suspend

     Suspends execution on a core or higher level topology node.
     This call is intended for use in idle subsystems where the core
     is expected to return to execution through a wakeup event.

   @smc_id
     0xC4000001

   @return
     SUCCESS \n
     INVALID_PARAMETERS	\n
     INVALID_ADDRESS \n
     DENIED  Only in OS Initiated mode.
*/
#define TZ_CPU_SUSPEND_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x1)

/**
   @ingroup psci_cpu_off

     Powers down the calling core. This call is intended for use in hotplug.
     A core that is powered down by CPU_OFF can only be powered up
     again in response to a CPU_ON.

   @smc_id
     0xC4000002

   @return
     The call does not return when successful.
     Otherwise, it returns DENIED.
*/
#define TZ_CPU_OFF_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x2)

/**
   @ingroup psci_cpu_on

     Powers up a core. This call is used to power up cores that either: \n
     - Have not yet been booted into the calling supervisory software, or \n
     - Have been previously powered down with a CPU_OFF call.

   @smc_id
     0xC4000003

   @return
     SUCCESS \n
     INVALID_PARAMETERS	\n
     INVALID_ADDRESS \n
     ALREADY_ON	\n
     ON_PENDING
     INTERNAL_FAILURE
*/
#define TZ_CPU_ON_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x3)

/**
   @ingroup psci_affinity_info

     Enable the caller to request status of an affinity instance.

   @smc_id
     0xC4000004

   @return
     2 -- ON_PENDING: The affinity instance is transitioning to an ON state \n
     1 -- OFF \n
     0 -- ON: At least one core in the affinity instance is ON \n
     INVALID_PARAMETERS	\n
     DISABLED
*/
#define TZ_AFFINITY_INFO_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x4)

/**
   @ingroup psci_migrate_to_cpu

     Used to ask a uniprocessor Trusted OS to migrate its
     context to a specific core.

   @smc_id
     0xC4000005

   @return
     SUCCESS \n
     NOT_SUPPORTED \n
     INVALID_PARAMETERS \n
     DENIED	\n
     INTERNAL_FAILURE \n
     NOT_PRESENT
*/
#define TZ_MIGRATE_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x5)

/**
   @ingroup psci_migrate_info

     Allows a caller to identify the level of multicore
     support present in the Trusted OS.

   @smc_id
     0xC4000006

   @return
     0 -- Uniprocessor is a migrate-capable Trusted OS.	\n
     1 -- Uniprocessor is not a migrate-capable Trusted OS. \n
     2 -- Trusted OS is either not present or does not require migration. \n
     NOT_SUPPORTED
*/
#define TZ_MIGRATE_INFO_TYPE_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x6)

/**
   @ingroup psci_migrate_info_up_cpu

     For a uniprocessor Trusted OS, this function returns
     the current resident core.

   @smc_id
     0xC4000007

   @return
     UNDEFINED if MIGRATE_INFO_TYPE returns 2 or NOT_SUPPORTED.	\n
     MPIDR-based value of the core where the Trusted OS
     resides if the return value of MIGRATE_INFO_TYPE is 0 or 1.
*/
#define TZ_MIGRATE_INFO_UP_CPU_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x7)

/**
   @ingroup psci_system_off

     Shuts down the system.

   @smc_id
     0xC4000008

   @return
     Does not return.
*/
#define TZ_SYSTEM_OFF_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x8)

/**
   @ingroup psci_system_reset

     Resets the system.

   @smc_id
     0xC4000009

   @return
     Does not return.
*/
#define TZ_SYSTEM_RESET_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x9)

/**
   @ingroup psci_features_id

     Queries whether a specific PSCI function
     is implemented and determines its features.

   @smc_id
     0xC400000A

   @return
     NOT_SUPPORTED if the function is not implemented
     or is an invalid function ID. \n
     If function ID is implemented, returns Bit[31] as zero and
     Bits[0:30] represent the feature flags.
*/
#define TZ_PSCI_FEATURES_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0xA)

/**
   @ingroup psci_cpu_freeze_id

     Places the core into an IMPLEMENTATION DEFINED low-power state.
     Unlike CPU_OFF, it is still valid for interrupts to be targeted to the core.
     However, the core must remain in the low-power state until
     a CPU_ON command is issued for it.

   @smc_id
     0xC400000B

   @return
     This call does not return when successful.	\n
     Otherwise, the return values are: \n
     NOT_SUPPORTED \n
     DENIED
*/
#define TZ_CPU_FREEZE_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0xB)

/**
   @ingroup psci_cpu_default_suspend_id

     Places a core into an IMPLEMENTATION DEFINED low-power state.
     Unlike CPU_SUSPEND, the caller need not specify a power_state parameter.

   @smc_id
     0xC400000C

   @return
     SUCCESS \n
     INVALID_ADDRESS
*/
#define TZ_CPU_DEFAULT_SUSPEND_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0xC)

/**
  @ingroup psci_set_suspend_mode_id

    PSCI function to switch low power mode handling between platform
    coordinated and OS initiated scheme. The implementation follows
    the PSCI spec as to when this mode switching is allowed so if the
    required conditions are not met, the function could return error.

  @smc_id
    0x8400000F

  @param_id
    0x00000001

  @return
    SUCCESS
    INVALID_PARAMS
    DENIED
*/
#define TZ_PSCI_SET_SUSPEND_MODE_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0xF)

/**
  @ingroup psci_mem_protect

    PSCI function to enable or disable non secure volatile memory
    protection against reboot attacks.

  @smc_id
    0x84000013

  @return
    On success returns the previous state of memory protection.
      1 - Enabled (mem protection was previously enabled).
      0 - Disabled (mem protection was previously disabled).
      NOT_SUPPORTED - if PSCI version does not support this function.

*/
#define TZ_PSCI_MEM_PROTECT                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x13)

/**
  @ingroup psci_mem_protect_check_range

    PSCI function to check whether a memory range will be protected if
    MEM_PROTECT is enabled

  @smc_id
    0xC4000014

  @return
    SUCCESS
    DENIED
    NOT_SUPPORTED

*/
#define TZ_PSCI_MEM_PROTECT_CHECK_RANGE          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x14)


/**
   @ingroup psci_reserved_wa

     This is a reserved workaround API used to leverage a standard services
     framework requirement. Currently, only the MSM8996 is using this.

   @smc_id
     0xC4000020

   @return
     SUCCESS \n
     NOT_SUPPORTED
*/
#define TZ_PSCI_RESERVED_WA_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x20)

/**
   @ingroup tz_psci_hyp_term_power_id

     This is an ID used to execute Fast PC low-power mode from Hypervisor
     outside the PSCI framework to achieve better latency without
     incurring overhead.

   @smc_id
     0xC4000021

   @return
     SUCCESS
*/
#define TZ_PSCI_HYP_TERM_POWER_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_STD, TZ_SVC_PSCI, 0x21)


/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/

/** @ingroup set_boot_addr_dep

 This command is deprecated. Use #TZ_SET_BOOT_ADDR_ID.
*/
#define TZ_SET_BOOT_ADDR_OLD_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x01)

/**
   @ingroup terminate_pwr_collapse

   Called to terminate power collapse. The QSEE configures the
   processor, then issues the Wait for Interrupt (WFI) instruction.

   @note1hang Implementation takes place entirely in Monitor mode.

   @smc_id
     0x02000102

   @param_id
    0x00000001

   @return
     None.
*/
#define TZ_POWER_COLLAPSE_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x02)

#define TZ_POWER_COLLAPSE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )



/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
/** @ingroup tz_boot_dep

 Deprecated: Use of this command is no longer supported. @newpage */
#define TZ_BOOT_DEPRECATED_0                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x03)

/**
   @ingroup krait_errata_21_workaround_enable

   Monitor call to enable the Krait errata #21 workaround. This
   workaround sets BPCR[FBHTD]=1 and does the required branch
   predictor maintenance.

   @note1hang Implementation takes place entirely in Monitor mode.

   @smc_id
     0x02000104

   @param_id
     0x00000000

   @return
     E_SUCCESS (always).
*/
#define TZ_KRAIT_ERRATA_21_WORKAROUND_ENABLE_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x04)

#define TZ_KRAIT_ERRATA_21_WORKAROUND_ENABLE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup cpu_config

   Cold and warm boot time CPU configuration service.

   @smc_id
     0x02000105

   @param_id
     0x00000004

   @sys_call_params{tz_cpu_config_req_s}
     @table{weak__tz__cpu__config__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_CPU_CONFIG_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x05)

#define TZ_CPU_CONFIG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup cpu_config_query

   Service for querying CPU configuration.

   @smc_id
     0x02000106

   @param_id
     0x00002004

   @sys_call_req_params{tz_cpu_config_query_req_s}
     @table{weak__tz__cpu__config__query__req__s}

   @sys_call_rsp_params{tz_cpu_config_query_rsp_s}
     @table{weak__tz__cpu__config__query__rsp__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_CPU_CONFIG_QUERY_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x06)

#define TZ_CPU_CONFIG_QUERY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup secure_wdog_disable

   Subroutine used by the HLOS to disable or enable the secure watchdog.
   This call is a NOP when debug functionality is disabled.

   @smc_id
     0x02000107

   @param_id
     0x00000001

   @sys_call_params{tz_secure_wdog_disable_req_s}
     @table{weak__tz__secure__wdog__disable__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SECURE_WDOG_DISABLE_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x07)

#define TZ_SECURE_WDOG_DISABLE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup secure_wdog_trigger

   Subroutine used by the HLOS to trigger a secure watchdog and reset
   the device. This call is a NOP when debug functionality is
   disabled.

   @smc_id
     0x02000108

   @param_id
     0x00000001

   @sys_call_params{tz_secure_wdog_trigger_req_s}
     @table{weak__tz__secure__wdog__trigger__req__s}

   @return
     An error code if parameters are invalid or watchdog
     reset is not permitted; does not return otherwise.
*/
#define TZ_SECURE_WDOG_TRIGGER_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x08)

#define TZ_SECURE_WDOG_TRIGGER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup config_hw_for_offline_ram_dump

   Subroutine used by the HLOS to configure hardware registers
   WDOG_DEBUG_EN to disable the watchdog.

   @smc_id
     0x02000109

   @param_id
     0x00000002

   @sys_call_params{tz_config_hw_for_ram_dump_req_s}
     @table{weak__tz__config__hw__for__ram__dump__req__s}

   @return
     Zero on success; an error code if parameters are invalid or if xPU
     reconfiguration is not permitted.
*/
#define TZ_CONFIG_HW_FOR_RAM_DUMP_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x09)

#define TZ_CONFIG_HW_FOR_RAM_DUMP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup video_cpu_set_state

   Controls changing the state of the subsystem (for example, suspend/resume),
   without full subsystem restarts

   @smc_id
     0x0200010a

   @param_id
     0x00000002

   @sys_call_params{tz_video_set_state_req_s}
     @table{weak__tz__video__set__state__req__s}

   @sys_call_params{tz_video_set_state_rsp_s}
     @table{weak__tz__video__set__state__rsp__s}

   @return
     Zero on success; negative value on failure.
*/
#define TZ_SUBSYS_SET_STATE_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0A)

#define TZ_SUBSYS_SET_STATE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup lock_xpu_peripheral

   Subroutine used by external bootloaders to request the TZ software to lock a
   peripheral image area.

   @smc_id
     0x0200010b

   @param_id
     0x00000003

   @sys_call_params{tz_lock_xpu_s}
     @table{weak__tz__lock__xpu__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_XPU_LOCK_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0b)

/* TODO Syscall does not exist */
#define TZ_XPU_LOCK_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
    TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup spin_with_irqs_fiqs_disabled

   If secure watchdog debug is allowed, calling this syscall masks IRQs and
   FIQs, starts spinning in the TZ software, and does not return. If secure watchdog debug is
   not allowed, the call returns without side effects.

   @smc_id
     0x0200010D

   @param_id
     0x00000001

   @sys_call_params{tz_spin_with_irqs_fiqs_disabled_s}
     @table{weak__tz__spin__with__irqs__fiqs__disabled__s}

   @return
     Zero on success; an error code if parameters are invalid or if xPU
     reconfiguration is not permitted.
*/

#define TZ_SPIN_WITH_IRQS_FIQS_DISABLED_ID          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0D)

#define TZ_SPIN_WITH_IRQS_FIQS_DISABLED_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup set_boot_milestone

   Subroutine used by external bootloaders to indicate the end
   of the bootloading stage, which is the beginning of HLOS booting.

   @smc_id
     0x0200010E

   @param_id
     0x00000003

   @sys_call_params{tz_set_milestone_s}
     @table{weak__tz__set__milestone__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MILESTONE_SET_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0E)

#define TZ_MILESTONE_SET_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup do_mode_switch

   Switches Execution mode to A64 (used by the 32bit application bootloader).
   Or
   Switches Execution mode to A32 (used by the 64bit application bootloader).

   @smc_id
     0x0000010F

   @param_id
     0x00000012

   @sys_call_params{tz_do_modeswitch_s}
     @table{weak__tz__do__modeswitch__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_DO_MODE_SWITCH                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0F)

#define TZ_DO_MODE_SWITCH_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RO, TZ_SYSCALL_PARAM_TYPE_VAL )

/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
/* Functionality moved to Secure IO syscall for atomicity */

/** @ingroup force_dload_dep

  Deprecated: Use of this command is no longer supported.
*/
#define TZ_FORCE_DLOAD_DEPRECATED_ID                              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x10)

/**
   @ingroup set_boot_addr

   Sets the execution address for the nonsecure execution
   environment after a CPU is brought out of reset.

   @smc_id
     0x02000111

   @param_id
     0x00000006

   @sys_call_params{tz_set_boot_addr_s}
     @table{weak__tz__set__boot__addr__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SET_BOOT_ADDR_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x11)

#define TZ_SET_BOOT_ADDR_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tz_config_cpu_errata_wa

   Generic Monitor SMC call to configure CPU/hardware errata at runtime.

   @smc_id
     0x02000112

   @param_id
     0x00000001

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_CONFIG_CPU_ERRATA_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x12)

#define TZ_CONFIG_CPU_ERRATA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( \
      TZ_SYSCALL_PARAM_TYPE_VAL)

/*
   @ingroup Reserved

   Reserved, do not use.

   @smc_id
     0x02000113
*/
#define TZ_RESERVED_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x13)

/**
   @ingroup xbl_sec_set_boot_milestone

   Subroutine used by external bootloaders to indicate the end
   of the XBL SEC stage, which is the beginning of TZ booting.

   @smc_id
     0x02000114

   @param_id
     0x00000022

   @sys_call_params{tz_set_milestone_s}
     @table{weak__tz__set__milestone__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define XBL_SEC_MILESTONE_SET_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x14)

#define XBL_SEC_MILESTONE_SET_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup xbl_sec_set_bootconfig

   Subroutine used by external bootloaders to share the bootconfig parameters

   @smc_id
     0x02000121

   @param_id
     0x00000012

   @sys_call_params{tz_set_bootconfig_s}
     @table{weak__tz__set_bootconfig__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define XBL_SEC_BOOTCONFIG_SET_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x21)

#define XBL_SEC_BOOTCONFIG_SET_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RO, TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup xbl_sec_tz_mem_init

   Subroutine used by external bootloaders to request to
   unlock tz and debug policy regions after xbl_sec
   cold boot to load TZ and debug policy images and the
   initialization of a pIMEM region.

   @smc_id
     0x02000115

   @param_id
     0x00000000

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define XBL_SEC_TZ_MEM_INIT_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x15)

#define XBL_SEC_TZ_MEM_INIT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup xbl_sec_pimem_get_status

   Subroutine used by external bootloaders to request the
   status and size of the pIMEM region.

   @smc_id
     0x02000116

   @param_id
     0x00000000

   @sys_call_params{xbl_sec_pimem_get_status_s}
     @table{weak__xbl__sec__pimem__get__status__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define XBL_SEC_PIMEM_GET_STATUS_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x16)

#define XBL_SEC_PIMEM_GET_STATUS_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup xbl_sec_xpu_violation

   Subroutine used by external bootloaders to query if an
   xPU violation has occurred.

   @smc_id
     0x02000117

   @param_id
     0x00000000

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define XBL_SEC_QUERY_XPU_VIOLATION_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x17)

#define XBL_SEC_QUERY_XPU_VIOLATION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup xbl_sec_clear_unlock_section

   Subroutine used by external bootloaders to clear a memory section
   and unlock the corresponding xPU

   @smc_id
     0x02000118

   @param_id
     0x00000001

   @return
     E_SUCCESS if xPUs were unlocked and memory was cleared; an error code otherwise.
*/
#define XBL_SEC_CLEAR_UNLOCK_SECTION_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x18)

#define XBL_SEC_CLEAR_UNLOCK_SECTION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( \
  TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup xbl_sec_is_auth_enabled_section

   Subroutine used by external bootloaders to check is_auth_enabled in PBL

   @smc_id
     0x02000119

   @param_id
     0x00000001

   @return
     TRUE if auth is enabled, FALSE otherwise
*/
#define XBL_SEC_IS_AUTH_ENABLED_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x19)

#define XBL_SEC_IS_AUTH_ENABLED_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( \
  TZ_SYSCALL_PARAM_TYPE_VAL )
/**
   @ingroup xbl_sec_set_reset_mode_section

   Subroutine used by external bootloaders to set reset mode

   @smc_id
     0x0200011A

   @param_id
     0x00000001

   @return
     Always return E-SUCCESS
*/
#define XBL_SEC_SET_RESET_MODE_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x1A)

#define XBL_SEC_SET_RESET_MODE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( \
  TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup xbl_sec_set_reset_wdt

   Subroutine used by external bootloaders to set reset mode

   @smc_id
     0x0200011B

   @param_id
     0x00000001

   @return
     Return E-SUCCESS is success if not Failure/not supported
*/
#define XBL_SEC_SET_WDT_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x1B)

#define XBL_SEC_SET_WDT_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( \
  TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup secure_dcc_enable

   Subroutine used by the Hypervisor to enable secure transactions
   for the linked list controlled by HLOS. 

   @smc_id
     0x0200011D

   @param_id
     0x00000002

   @return
     Return E-SUCCESS if sucess / failure. 
*/
#define TZ_SECURE_DCC_TRIGGER_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x1D)

#define TZ_SECURE_DCC_TRIGGER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 @ingroup kernel authentication

 Service for authenticating the HLOS image.

 @smc_id
   0x0200011e

 @param_id
   0x00000001

 @com_struct
   tzbsp_syscall_req_s

 @sys_call_params{kernel_load_addr}

 @return
   Zero on success; negative value on failure.
*/
#define TZ_CPU_KERNEL_AUTH_ID                      \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x1e)

#define TZ_CPU_KERNEL_AUTH_PARAM_ID \
TZ_SYSCALL_CREATE_PARAM_ID_1( \
    TZ_SYSCALL_PARAM_TYPE_VAL \
    )

/**
 @ingroup signed image authentication

 Validate any signed elf/mbn image w.r.t RoT of the fused chip

 @smc_id
   0x0200011f

 @param_id
   0x00000003

 @com_struct
   tzbsp_syscall_req_s

 @sys_call_params{sw_type, img_size, img_addr}

 @return
   Zero on success; negative value on failure.
*/
#define TZ_SEC_IMG_AUTH_ID                      \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x1f)

#define TZ_SEC_IMG_AUTH_PARAM_ID \
TZ_SYSCALL_CREATE_PARAM_ID_3( \
    TZ_SYSCALL_PARAM_TYPE_VAL, \
    TZ_SYSCALL_PARAM_TYPE_VAL, \
    TZ_SYSCALL_PARAM_TYPE_BUF_RW \
    )

/**
 @ingroup Get SMMU status

 Subroutine used by the HLOS to get the SMMU status

 @smc_id
   0x02000120

 @param_id
   0x00000000

 @return
   Zero on success

   Response 1: SMMU Status
     0: SMMU Enabled
     1: SMMU S2 Disabled/Bypassed
     2: SMMU Disabled Completely
*/
#define TZ_SMMU_STATUS_ID                      \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x20)

#define TZ_SMMU_STATUS_PARAM_ID \
    TZ_SYSCALL_CREATE_PARAM_ID_0

/**
 @ingroup kernel ARB version fuse bank update

 Service for updating the HLOS image fuse version bank

 @smc_id
   0x02000122

 @param_id
   0x00000001

 @com_struct
   tzbsp_syscall_req_s

 @sys_call_params{kernel_boot_cfg}

 @return
   Zero on success; negative value on failure.
*/
#define TZ_KERNEL_BOOT_CFG_SET_ID                      \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x22)

#define TZ_KERNEL_BOOT_CFG_SET_PARAM_ID \
TZ_SYSCALL_CREATE_PARAM_ID_1( \
    TZ_SYSCALL_PARAM_TYPE_VAL \
    )

/**
 @ingroup signed image authentication

 Validate the rootfs metadata during sysupgrade. rootfs hash is passed from HLOS
 and that is compared against the value in the hash segment.

 @smc_id
   0x02000123

 @param_id
   0x00000885

 @com_struct
   tzbsp_syscall_req_s

 @sys_call_params{sw_type, img_addr, img_size, hash_addr, hash_size}

 @return
   Zero on success; negative value on failure.
*/

#define TZ_SEC_IMG_AUTH_FS_ID                      \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x23)

#define TZ_SEC_IMG_AUTH_FS_PARAM_ID \
TZ_SYSCALL_CREATE_PARAM_ID_5( \
    TZ_SYSCALL_PARAM_TYPE_VAL, \
    TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
    TZ_SYSCALL_PARAM_TYPE_VAL, \
    TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
    TZ_SYSCALL_PARAM_TYPE_VAL \
    )


/**
   @ingroup init_image_syscall

   Verifies whether the memory at the specified Executable and Linkable
   Format (ELF) header pointer is a valid ELF file. Once that has been
   verified, the QSEE copies the ELF header, program header, and hash
   segment into secure memory.

   @smc_id
     0x02000201

   @param_id
     0x00000082

   @sys_call_params{tz_pil_init_req_s}
     @table{weak__tz__pil__init__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_INIT_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x01)

#define TZ_PIL_INIT_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup get_pil_mem_usage

   Confirms whether the memory region used to load the specified
   peripheral image matches the static region that the QSEE supports
   for the specified region, and that the region is appropriate for the
   ELF header specified in the secure Peripheral Image Loader (PIL)
   initialization call.

   @smc_id
     0x02000202

   @param_id
     0x00000003

   @sys_call_params{tz_pil_mem_req_s}
     @table{weak__tz__pil__mem__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_MEM_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x02)

#define TZ_PIL_MEM_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )


/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/

/** @ingroup tz_pil_dep0

  Deprecated: Use of this command is no longer supported.
*/
#define TZ_PIL_DEPRECATED_0 TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x03)

/** @ingroup tz_pil_dep1

  Deprecated: Use of this command is no longer supported.
*/
#define TZ_PIL_DEPRECATED_1 TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x04)


/**
   @ingroup auth_reset

   Authenticates and releases the specified peripheral from reset.
   The QSEE also protects the memory region for the specified
   peripheral image with a Memory Protection Unit (MPU).

   @smc_id
     0x02000205

   @param_id
     0x00000001

   @sys_call_params{tz_pil_auth_reset_req_s}
     @table{weak__tz__pil__auth__reset__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_AUTH_RESET_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x05)

#define TZ_PIL_AUTH_RESET_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup unlock_xpu

   Stops the specified peripheral, then clears the peripheral image
   location and unlocks the memory area occupied by that region.

   @smc_id
     0x02000206

   @param_id
     0x00000001

   @sys_call_params{tz_pil_unlock_req_s}
     @table{weak__tz__pil__unlock__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_UNLOCK_XPU_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x06)

#define TZ_PIL_UNLOCK_XPU_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup is_subsys_supported

   Queries whether the QSEE supports secure PIL for a specified
   subsystem.

   @smc_id
     0x02000207

   @param_id
     0x00000001

   @sys_call_req_params{tz_pil_subsystem_query_req_s}
     @table{weak__tz__pil__subsystem__query__req__s}

   @sys_call_rsp_params{tz_pil_subsystem_query_rsp_s}
     @table{weak__tz__pil__subsystem__query__rsp__s}

   @return
     E_SUCCESS if the request was successful; an error code otherwise.
*/
#define TZ_PIL_IS_SUBSYS_SUPPORTED_ID            \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x07)

#define TZ_PIL_IS_SUBSYS_SUPPORTED_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup is_subsys_mandated

   Queries whether QSEE security settings make it secure to the point
   where a PIL is required for the specified subsystem.

   @smc_id
     0x02000208

   @param_id
     0x00000001

   @sys_call_params
     Same request and response parameters as for
     #TZ_PIL_IS_SUBSYS_SUPPORTED_ID.

   @return
     E_SUCCESS if the request was successful; an error code otherwise.
*/
#define TZ_PIL_IS_SUBSYS_MANDATED_ID             \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x08)

#define TZ_PIL_IS_SUBSYS_MANDATED_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup get_mem_area

   Queries the start and end addresses in which subsystems are allowed
   to be loaded.

   @smc_id
     0x02000209

   @param_id
     0x00000001

   @sys_call_req_params{tz_pil_mem_area_query_req_s}
     @table{weak__tz__pil__mem__area__query__req__s}

   @sys_call_rsp_params{tz_pil_mem_area_query_rsp_s}
     @table{weak__tz__pil__mem__area__query__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_GET_MEM_AREA_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x09)

#define TZ_PIL_GET_MEM_AREA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup pil_modem_restart

   This command is deprecated.

 */
#define TZ_PIL_MSS_RESTART_DEPRECATED_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL, 0x0A)


/**
   @ingroup share_memory

   Configures xPUs to share/unshare HLOS memory with a specified subsystem.

   @smc_id
     0x0200020B

   @param_id
     0x00000004

   @sys_call_params{tz_pil_share_memory_s}
     @table{weak__tz__pil__share__memory__s}

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_PIL_SHARE_MEMORY_ID              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x0B)

#define TZ_PIL_SHARE_MEMORY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup pil_protect_memory

   Syscall to protect a given address range on the basis of a subsystem (such as dynamic heap).

   @smc_id
     0x0200020C

   @param_id
     0x00000004

   @sys_call_params{tz_pil_protect_memory_s}
     @table{weak__tz__pil__protect__memory__s}

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_PIL_PROTECT_MEM_SUBSYS_ID              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x0C)

#define TZ_PIL_PROTECT_MEM_SUBSYS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup pil_clear_protect_memory

   Syscall to remove protection from the PIL dynamic heap area.

   @smc_id
     0x0200020D

   @param_id
     0x00000004

   @sys_call_params{tz_pil_clear_protect_memory_s}
     @table{weak__tz__pil__clear__protect__memory__s}

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_PIL_CLEAR_PROTECT_MEM_SUBSYS_ID              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x0D)

#define TZ_PIL_CLEAR_PROTECT_MEM_SUBSYS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup get_reset_mem_area

   Queries the start and end addresses of the subsystem as stored across a
   reset. This information is used to determine the memory regions that need
   to be cleared out.

   @smc_id
     0x0200020E

   @param_id
     0x00000001

   @sys_call_req_params{tz_pil_mem_area_query_req_s}
     @table{weak__tz__pil__mem__area__query__req__s}

   @sys_call_rsp_params{tz_pil_mem_area_query_rsp_s}
     @table{weak__tz__pil__mem__area__query__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_GET_RESET_MEM_AREA_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x0E)

#define TZ_PIL_GET_RESET_MEM_AREA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup get_rollback_version

   Queries the version, as stored in the anti-rollback fuses, for a given
   subsystem.

   @smc_id
     0x0200020F

   @param_id
     0x00000001

   @sys_call_req_params{tz_pil_get_rollback_version_req_s}
     @table{weak__tz__pil__get__rollback__version__req__s}

   @sys_call_rsp_params{tz_pil_get_rollback_version_rsp_s}
     @table{weak__tz__pil__get__rollback__version__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_GET_ROLLBACK_VERSION_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x0F)

#define TZ_PIL_GET_ROLLBACK_VERSION_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup sec_img_auth_verify_metadata

   Validates the integrity of an ELF header; checks that the header is well
   formed and verifies the signature.

   @smc_id
     0x02000210

   @param_id
     0x00000944

   @sys_call_req_params{tz_pil_sec_img_verify_metadata_req_s}
     @table{weak__tz__pil__sec__img__verify__metadata__req__s}

   @sys_call_rsp_params{tz_pil_sec_img_verify_metadata_rsp_s}
     @table{weak__tz__pil__sec__img__verify__metadata__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_SEC_IMG_VERIFY_METADATA_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x10)

#define TZ_PIL_SEC_IMG_VERIFY_METADATA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_8( TZ_SYSCALL_PARAM_TYPE_VAL,    \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO, \
                                TZ_SYSCALL_PARAM_TYPE_VAL,    \
                                TZ_SYSCALL_PARAM_TYPE_VAL,    \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO, \
                                TZ_SYSCALL_PARAM_TYPE_VAL,    \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
                                TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup sec_img_auth_hash_elf_segments

   Validates that the hash of each ELF segment matches what is present in the
   ELF header.

   @smc_id
     0x02000211

   @param_id
     0x00000082

   @sys_call_req_params{tz_pil_sec_img_hash_segments_req_s}
     @table{weak__tz__pil__sec__img__hash__segments__req__s}

   @sys_call_rsp_params{tz_pil_sec_img_hash_segments_rsp_s}
     @table{weak__tz__pil__sec__img__hash__segments__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_SEC_IMG_HASH_SEGMENTS_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x11)

#define TZ_PIL_SEC_IMG_HASH_SEGMENTS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( TZ_SYSCALL_PARAM_TYPE_VAL,    \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
                                TZ_SYSCALL_PARAM_TYPE_VAL,    \
                                TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup sec_img_auth_validate_elf

   Validates an ELF header.

   @smc_id
     0x02000212

   @param_id
     0x00000012

   @sys_call_req_params{tz_pil_sec_img_validate_elf_req_s}
     @table{weak__tz__pil__sec__img__validate__elf__req__s}

   @sys_call_rsp_params{tz_pil_sec_img_validate_elf_rsp_s}
     @table{weak__tz__pil__sec__img__validate__elf__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_SEC_IMG_VALIDATE_ELF_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x12)

#define TZ_PIL_SEC_IMG_VALIDATE_ELF_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( TZ_SYSCALL_PARAM_TYPE_VAL,    \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO, \
                                TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup sec_img_auth_is_valid_segment

   Checks whether an ELF segment is valid.

   @smc_id
     0x02000213

   @param_id
     0x00000042

   @sys_call_req_params{tz_pil_sec_img_valid_seg_req_s}
     @table{weak__tz__pil__sec__img__valid__seg__req__s}

   @sys_call_rsp_params{tz_pil_sec_img_valid_seg_rsp_s}
     @table{weak__tz__pil__sec__img__valid__seg__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_SEC_IMG_VALID_SEG_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x13)

#define TZ_PIL_SEC_IMG_VALID_SEG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( TZ_SYSCALL_PARAM_TYPE_VAL,    \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO, \
                                TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup secboot_verify_sig

   Checks whether the signature on a blob of data is valid

   @smc_id
     0x02000214

   @param_id
     0x00008017

   @sys_call_req_params{tz_pil_secboot_verify_sig_req_s}
     @table{weak__tz__pil__secboot__verify__sig__req__s}

   @sys_call_rsp_params{tz_pil_secboot_verify_sig_rsp_s}
     @table{weak__tz__pil__secboot__verify__sig__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_SECBOOT_VERIFY_SIG_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x14)

#define TZ_PIL_SECBOOT_VERIFY_SIG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_7( TZ_SYSCALL_PARAM_TYPE_BUF_RO,    \
                                TZ_SYSCALL_PARAM_TYPE_VAL, \
                                TZ_SYSCALL_PARAM_TYPE_VAL, \
                                TZ_SYSCALL_PARAM_TYPE_VAL, \
                                TZ_SYSCALL_PARAM_TYPE_VAL, \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
                                TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup sec_img_auth_verify_hash

   Validates the hash of a given data buffer, allowing for the hash compuation
   to occur in parts.

   @smc_id
     0x02000215

   @param_id
     0x00000446

   @sys_call_req_params{tz_pil_sec_img_auth_verify_hash_req_s}
     @table{weak__tz__pil__sec__img__auth__verify__hash__req__s}

   @sys_call_rsp_params{tz_pil_sec_img_auth_verify_hash_rsp_s}
     @table{weak__tz__pil__sec__img__auth__verify__hash__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_SEC_IMG_AUTH_VERIFY_HASH_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x15)

#define TZ_PIL_SEC_IMG_AUTH_VERIFY_HASH_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( TZ_SYSCALL_PARAM_TYPE_VAL, \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO,    \
                                TZ_SYSCALL_PARAM_TYPE_VAL, \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO,    \
                                TZ_SYSCALL_PARAM_TYPE_VAL, \
                                TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup sec_img_auth_verify_hash_in_parallel

   Validates the hash of a given data buffer in parallel, allowing for the hash compuation
   to occur in parallel with image loading.

   @smc_id
     0x02000216

   @param_id
     0x00000105

   @sys_call_req_params{tz_pil_sec_img_auth_verify_hash_in_parallel_req_s}
     @table{weak__tz__pil__sec__img__auth__verify__hash__in_parallel_req__s}

   @sys_call_rsp_params{tz_pil_sec_img_auth_verify_hash_in_parallel_rsp_s}
     @table{weak__tz__pil__sec__img__auth__verify__hash__in_parallel_rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_SEC_IMG_AUTH_VERIFY_HASH_IN_PARALLEL_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x16)

#define TZ_PIL_SEC_IMG_AUTH_VERIFY_HASH_IN_PARALLEL_ID_PARAM_ID  \
  TZ_SYSCALL_CREATE_PARAM_ID_5( TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO,   \
                                TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_VAL)
/**
   @ingroup sec_img_auth_verify_ecc_sig

   Verifies the ECC signature

   @smc_id
     0x02000217

   @param_id
     0x00084449

   @sys_call_req_params{tz_pil_sec_img_auth_verify_ecc_sig_req_s}
     @table{weak__tz__pil__sec__img__auth__verify__ecc_sig__req__s}

   @sys_call_rsp_params{tz_pil_sec_img_auth_verify_ecc_sig_rsp_s}
     @table{weak__tz__pil__sec__img__auth__verify__ecc_sig__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_SEC_IMG_AUTH_VERIFY_ECC_SIG_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x17)

#define TZ_PIL_SEC_IMG_AUTH_VERIFY_ECC_SIG_ID_PARAM_ID          \
  TZ_SYSCALL_CREATE_PARAM_ID_9( TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO,   \
                                TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO,   \
                                TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO,   \
                                TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RW,   \
                                TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup wcss_break_start

   Function to enable debug during Q6 init
   @smc_id
     0x02000218

   @param_id
     0x00000001

   @sys_call_params{tz_pil_auth_reset_req_s}
     @table{weak__tz__pil__auth__reset__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_PROC_BREAK_DEBUG                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x18)

#define TZ_PIL_PROC_BREAK_DEBUG_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup tmel_fw_auth

   @smc_id
     0x02000219

   @param_id
     0x00000002

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define XBL_SEC_TMEL_FW_AUTH_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL, 0x19)

/**
     @ingroup tmel_fw_auth

     @smc_id
        0x0200021A
 
    @param_id
        0x00000002
 
    @return
        E_SUCCESS on success; an error code otherwise.
*/
#ifdef CHIPSET_53xx
#define TZ_PIL_INITV2_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x1A)

#define TZ_PIL_INITV2_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )
#endif

#ifndef CHIPSET_53xx 
#define XBL_SEC_TMEL_FW_AUTH_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)
#else
#define XBL_SEC_TMEL_FW_AUTH_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2(TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL)
#endif

/** @ingroup dump_write_lpass_qdsp6_nmi

   Deprecated: Use of this command is no longer supported.
*/
#define TZ_DUMP_WRITE_LPASS_QDSP6_NMI_ID         \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x01)


/**
   @ingroup irong_reg_read_syscall

   system call for wcss register read

   @smc_id
     0x02000222

   @param_id
    0x00000001

   @sys_call_params{tz_pil_init_req_s}
     @table{weak__tz__pil__init__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_IRONG_RFA_READ_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x22)

#define TZ_IRONG_RFA_READ_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup irong_write_reg_syscall

   system call for wcss register write

   @smc_id
     0x02000223

   @param_id
    0x00000002

   @sys_call_params{tz_pil_init_req_s}
     @table{weak__tz__pil__init__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_IRONG_RFA_WRITE_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x23)

#define TZ_IRONG_RFA_WRITE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( TZ_SYSCALL_PARAM_TYPE_VAL, \
                                TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup set_wdt_buff_addr

   System call for the nonsecure side to set the Watchdog Timer (WDT)
   reset context dump buffer address.

   @smc_id
     0x02000302

   @param_id
     0x00000022

   @sys_call_params{tz_dump_set_cpu_ctx_buf_req_s}
     @table{weak__tz__dump__set__cpu__ctx__buf__req__s}

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_CPU_CTX_BUF_ID               \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x02)

#define TZ_DUMP_SET_CPU_CTX_BUF_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/** @ingroup dump_write_mss_qdsp6_nmi

   Deprecated: Use of this command is no longer supported.
*/
#define TZ_DUMP_WRITE_MSS_QDSP6_NMI_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x03)

/**
   @ingroup set_l1_dump_buffer

   System call for the nonsecure side to set the L1 dump buffer address.

   @smc_id
     0x02000304

   @param_id
     0x00000022

   @sys_call_params{tz_dump_set_cache_buf_req_s}
     @table{weak__tz__dump__set__cache__buf__req__s}

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_L1_DUMP_BUF_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x04)

#define TZ_DUMP_SET_L1_DUMP_BUF_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup cache_dump_request

   Monitor call to request L0/L1/L2 cache dumping. Cache
   dumping is ignored if fuse settings indicate that secure boot
   is enabled or debug is disabled.

   A request to dump L0/L1 is assumed to indicate a fatal error condition
   in the nonsecure side. D-caches are not cleared before resetting the
   device. Entry to Download mode is controlled by the Download mode
   magic numbers that are controlled by the HLOS.

   @note1hang Implementation takes place entirely in Monitor mode. \n
   @note1hang This command uses a register-based QSEE interface.

   @smc_id
     0x02000305

   @param_id
     0x00000001

   @param[in] dump_request    0 -- Handle an L1 dump request similar to a WDT bark,
                                   resulting in an L0/L1 cache dump, CPU context dumps
                                   and a device reset. \n
                              1 -- Dump the L1 cache and return to the HLOS only after
                                   the dump is done. \n
                              2 -- Only dump the L2 cache and return to HLOS after the
                                   dump is done. \n
                              Any other value for r2 returns -4 (E_NOT_SUPPORTED).


   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_REQUEST_CACHE_DUMP_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x05)

#define TZ_DUMP_REQUEST_CACHE_DUMP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup query_l1_buffer_size

   System call for the nonsecure side to query the L1 dump buffer size.

   @smc_id
     0x02000306

   @param_id
     0x00000083

   @sys_call_req_params{tz_dump_query_cache_buf_size_req_s}
     @table{weak__tz__dump__query__cache__buf__size__req__s}

   @sys_call_rsp_params{tz_dump_query_cache_buf_size_rsp_s}
     @table{weak__tz__dump__query__cache__buf__size__rsp__s}

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_QUERY_L1_DUMP_BUF_SIZE_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x06)

#define TZ_DUMP_QUERY_L1_DUMP_BUF_SIZE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup set_l2_dump_buffer

   System call for for the nonsecure side to set the L2 dump buffer address.

   @smc_id
     0x02000307

   @param_id
     0x00000002

   @sys_call_params{tz_dump_set_cache_buf_req_s}
     @table{weak__tz__dump__set__cache__buf__req__s}

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_L2_DUMP_BUF_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x07)

#define TZ_DUMP_SET_L2_DUMP_BUF_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup query_l2_buffer_size

   System call for the nonsecure side to query the L2 dump buffer size.

   @smc_id
     0x02000308

   @param_id
     0x00000083

   @sys_call_req_params{tz_dump_query_cache_buf_size_req_s}
     @table{weak__tz__dump__query__cache__buf__size__req__s}

   @sys_call_rsp_params{tz_dump_query_cache_buf_size_rsp_s}
     @table{weak__tz__dump__query__cache__buf__size__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_QUERY_L2_DUMP_BUF_SIZE_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x08)

#define TZ_DUMP_QUERY_L2_DUMP_BUF_SIZE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup set_ocmem_dump_buf

   System call for the nonsecure side to set the On-Chip Memory (OCMEM) dump buffer address.

   @smc_id
     0x02000309

   @param_id
     0x00000022

   @sys_call_req_params{tz_dump_set_ocmem_buf_req_s}
     @table{weak__tz__dump__set__ocmem__buf__req__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_OCMEM_DUMP_BUF_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x09)

#define TZ_DUMP_SET_OCMEM_DUMP_BUF_ID_PARAM_ID \
    TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup query_ocmem_dump_buf_size

   System call for the nonsecure side to query the OCMEM dump buffer size.

   @smc_id
     0x0200030A

   @param_id
     0x00000083

   @sys_call_req_params{tz_dump_query_ocmem_buf_size_req_s}
     @table{weak__tz__dump__query__ocmem__buf__size__req__s}

   @sys_call_rsp_params{tz_dump_query_ocmem_buf_size_rsp_s}
     @table{weak__tz__dump__query__ocmem__buf__size__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_QUERY_OCMEM_DUMP_BUF_SIZE_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0A)

#define TZ_DUMP_QUERY_OCMEM_DUMP_BUF_SIZE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup security_allows_mem_dump_legacy

   Legacy system call for the SBL to determine whether memory dumping
   (for example, DLOAD mode) is allowed.

   @smc_id
     0x0200030b

   @param_id
     0x00000022

   @sys_call_req_params{tz_security_allows_mem_dump_rsp_s}
     @table{weak__tz__security__allows__mem__dump__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.

*/
#define TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP_LEGACY_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0B)

#define TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP_LEGACY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup smmu_dump_fault_syndrome

   System call for dumping the System Memory Management Unit (SMMU) fault syndrome registers.

   @note1hang
     SMMU clocks must be ON.

   @smc_id
     0x0200030c

   @param_id
     0x00000204

   @sys_call_req_params{tz_smmu_fault_regs_dump_req_s}
     @table{weak__tz__smmu__fault__regs__dump__req__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SMMU_FAULT_REGS_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0C)

#define TZ_DUMP_SMMU_FAULT_REGS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup security_is_retail_unlock

   System call for the SBL to determine whether retail unlock is enabled.

   @smc_id
     0x0200030d

   @param_id
     0x00000022

   @sys_call_req_params{tz_security_is_retail_unlock_req_s}
     @table{weak__tz__security__is__retail__unlock__req__s}

   @sys_call_rsp_params{tz_security_is_retail_unlock_rsp_s}
     @table{weak__tz__security__is__retail__unlock__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_IS_RETAIL_UNLOCK_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0D)

#define TZ_DUMP_IS_RETAIL_UNLOCK_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup dump_tz_diag_to_hlos

   System call for the HLOS to provide the address for and enable the dumping of the TZ diagnostics
   area at an HLOS-provided address.

   @smc_id
     0x0200030e

   @param_id
     0x00000002

   @sys_call_req_params{tz_set_address_to_dump_tz_diag_for_uefi_req_s}
     @table{weak__tz__set__address__to__dump__tz__diag__for__uefi__req__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
 */
#define TZ_DUMP_SET_ADDRESS_TO_DUMP_TZ_DIAG_FOR_HLOS \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0E)

#define TZ_DUMP_SET_ADDRESS_TO_DUMP_TZ_DIAG_FOR_HLOS_PARAM_ID \
        TZ_SYSCALL_CREATE_PARAM_ID_2( \
        TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup  rpm_online_dump

   Notification to the RPM to make its memory available to be read by the HLOS
   without resetting the device.

   @smc_id
     0x02000c0f

   @param_id
     0x00000001

   @sys_call_req_params{tz_rpm_online_dump_req_s}
     @table{weak__tz__rpm__online__dump__req__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
  */
#define TZ_DUMP_RPM_ONLINE_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0F)

#define TZ_DUMP_RPM_ONLINE_PARAM_ID \
        TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup security_allows_mem_dump

   System call for the SBL to determine whether memory dumping
   (for example, DLOAD mode) is allowed.

   @smc_id
     0x02000310

   @param_id
     0x00000000

   @sys_call_rsp_params{tz_security_allows_mem_dump_rsp_s}
     @table{weak__tz__security__allows__mem__dump__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.

   Response 1: Dumps Allowed
     0: Dumps not Allowed
     1: SEC_DMP_ALLOWED
     2: NSEC_DMP_ALLOWED

   Response 2: Is DLOAD Cookie set (TRUE/FALSE)

   Response 3: APDP flags status
     Bit 0: SEC_DBG_ENABLE_ONLINE_CRASH_DUMPS
     Bit 1: SEC_DBG_ENABLE_OFFLINE_CRASH_DUMPS
     Bit 2: SEC_DBG_ENABLE_JTAG
     Bit 3: SEC_DBG_ENABLE_LOGGING
     Bit 4: SEC_DBG_ENABLE_NONSECURE_CRASH_DUMPS
     Bit 5: SEC_DBG_ENABLE_APPS_ENCRYPTED_MINI_DUMPS
     Bit 6: SEC_DBG_ENABLE_MPSS_ENCRYPTED_MINI_DUMPS
     Bit 7: SEC_DBG_ENABLE_LPASS_ENCRYPTED_MINI_DUMPS
     Bit 8: SEC_DBG_ENABLE_CSS_ENCRYPTED_MINI_DUMPS
     Bit 9: SEC_DBG_ENABLE_ADSP_ENCRYPTED_MINI_DUMPS
     Bit 10: SEC_DBG_ENABLE_CDSP_ENCRYPTED_MINI_DUMPS
     Bit 11: SEC_DBG_ENABLE_MODEM_INV_DEBUG
     Bit 12: SEC_DBG_ENABLE_MODEM_NINV_DEBUG
     Bit 13: SEC_DBG_EENABLE_APPS_DEBUG_INV
     Bit 14: SEC_DBG_ENABLE_DEBUG_LEVEL_BIT0
     Bit 15: SEC_DBG_ENABLE_DEBUG_LEVEL_BIT1
*/
#define TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x10)

#define TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup security_allows_smmu_tlb_dump

   System call for HLOS to request dumping of SMMU TLB contents

   @smc_id
     0x02000311

   @param_id
     0x00000001

   @param[in] Device ID of SMMU

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SMMU_FAULT_TLBS_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x11)

#define TZ_DUMP_SMMU_FAULT_TLBS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3 \
  ( TZ_SYSCALL_PARAM_TYPE_VAL, \
    TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup PMIC_PBS_RAM_DATASET_CFG

   System call for HLOS to request PBS RAM dataset config

   @smc_id
     0x0200090A

   @param_id
     0x00000000

   @param[in] Device ID of PMIC

   @return
	 SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_PMIC_PBS_RAM_DATASET_CFG_ID \
     TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_POWER, 0x09)

#define TZ_PMIC_PBS_RAM_DATASET_CFG_PARAM_ID \
	    TZ_SYSCALL_CREATE_PARAM_ID_4 \
	     ( TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
	       TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup ddr_self_refresh_return

   System call for bootloader code running in NS EL1 to return to TZ from
   DDR self-refresh enter/exit APIs.

   @smc_id
     0x02000312

   @param_id
     0x00000001

   @sys_call_req_params{tz_ddr_self_refresh_return_req_s}
     @table{weak__tz__ddr__self__refresh__return__req__s}

   @return
     None
*/
#define TZ_DUMP_DDR_SELF_REFRESH_RETURN_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x12)

#define TZ_DUMP_DDR_SELF_REFRESH_RETURN_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1 (TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup lock_shared_resource

   Locks a shared hardware resource.

   @smc_id
     0x02000401

   @param_id
     0x00000000

   @sys_call_req_params{tz_lock_resource_req_s}
     @table{weak__tz__lock__resource__req__s}

   @sys_call_rsp_params{tz_lock_resource_rsp_s}
     @table{weak__tz__lock__resource__rsp__s}

   @return
    Zero if the return code status is filled. \n
    E_BAD_ADDRESS if the return parameter address is bad or wrong. \n
    E_NOT_SUPPORTED if the wrong resource type is passed. \n
    E_INVALID_ARG if the lock is not a value of 0 or 1.
*/
#define TZ_RESOURCE_LOCK_ID                              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RES_LOCKING, 0x01)

/* TODO Syscall does not exist */
#define TZ_RESOURCE_LOCK_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup change_resource_config

   System call to change resource configuration dynamically.

   @smc_id
     0x02000402

   @param_id
     0x00000002

   @sys_call_params{tz_config_resource_req_s}
     @table{weak__tz__config__resource__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_RESOURCE_CONFIG_ID                            \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RES_LOCKING, 0x02)

#define TZ_RESOURCE_CONFIG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
  @ingroup blsp_modify_ownership

  Modify Bus Access Manager (BAM) Low-Speed Peripheral (BLSP) ownership.

  @smc_id
    0x02000403

  @param_id
    0x00000002

  @sys_call_req_params{tz_modify_ownership_req_s}
     @table{weak__tz__modify__ownership__req__s}

  @return
    E_SUCCESS if success; else error code.
*/
#define TZ_BLSP_MODIFY_OWNERSHIP_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RES_LOCKING, 0x03)

#define TZ_BLSP_MODIFY_OWNERSHIP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_ce_pipe_key_select_hw

   Set the context config for ICE_LUT_r_CONTROL on EMMC ICE

   @smc_id
     0x02001701

   @param_id
     0x00000002


   @sys_call_req_params{ice_set_key_config_context_req_s}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_ICE_SET_HW_KEY_CTX_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_ICE, 0x1)

#define TZ_ICE_SET_HW_KEY_CTX_PARAM_ID TZ_SYSCALL_CREATE_PARAM_ID_2( \
           TZ_SYSCALL_PARAM_TYPE_BUF_RO, TZ_SYSCALL_PARAM_TYPE_VAL)

/**
  @ingroup tz_os_ice_disable_sequence

  Disables the SALT/DATA keys from a key LUT index of UFS ICE or EMMC ICE.
  The SALT/DATA keys will still be stored in ICE, but will be disabled.

  @smc_id
    0x32001702

  @param_id
    0x00000002

  @sys_call_req_params{tz_os_ice_disable_sequence_req_s}
     @table{weak__tz__os__ice__disable__sequence__req__s}

  @return
    None
*/
#define TZ_OS_ICE_DISABLE_SEQUENCE_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_ICE, 0x02)

#define TZ_OS_ICE_DISABLE_SEQUENCE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup 

   Set the key on EMMC ICE

   @smc_id
     0x02001703

   @param_id
     0x00004407


   @sys_call_req_params{}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_ICE_GEN_HW_KEY_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_ICE, 0x3)

#define TZ_ICE_GEN_HW_KEY_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_7( \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RO, \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RO, \
      TZ_SYSCALL_PARAM_TYPE_VAL )


/**
  @ingroup tz_qdss_swtrace_control

  Handle QDSS TZ SW Trace commands

  @smc_id
    0x02002201

  @param_id
    0x00000002

  @sys_call_req_params{tz_qdss_swtrace_control_req_s}
     @table{weak__tz__qdss__swtrace__control__req__s}

  @return
    None
*/

#define TZ_QDSS_SWTRACE_CONTROL_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_QDSS, 0x01)

#define TZ_QDSS_SWTRACE_CONTROL_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RO, TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup unlock_debug_register

   System call to unlock a debug register for QDSS.

   @note1hang The TZ software keeps a persistent vote on QDSS clocks after calling this API.

   @smc_id
     0x02001004

   @param_id
     0x00000000

   @return
     E_SUCCESS on success.
*/
#define TZ_UNLOCK_DBG_REG                                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RES_LOCKING, 0x04)

#define TZ_UNLOCK_DBG_REG_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0


/**
   @ingroup Handle QUP ADSP/SLPI SSR.

   Handle the GPII reset during ADSP/SLPI SSR.

   @smc_id
     0x02001005

   @param_id
     0x00000001

   @sys_call_req_params{tz_handle_ssr_req_s}
   @table{weak__tz__handle__ssr__req__s}

   @return
     None
*/
#define TZ_BLSP_HANDLE_SSR_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RES_LOCKING,  0x05)

#define TZ_BLSP_HANDLE_SSR_ID_PARAM_ID  \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup io_access_read

   Allows reading of memory areas that fall under Memory/Area/Register
   Protection Unit (xPU) protection but do not need to be secured.

   @note1hang Implementation takes place entirely in Monitor mode. \n

   @smc_id
     0x02000501

   @param_id
     0x00000001

   @param[in] addr Address to be read.
   @param[out] val Value at that address.

   @return
     Zero on success, negative value otherwise.
*/
#define TZ_IO_ACCESS_READ_ID                           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x01)

#define TZ_IO_ACCESS_READ_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup io_access_write

   Allows writing to memory areas that fall under xPU protection
   but do not need to be secured.

   @note1hang Implementation takes place entirely in Monitor mode. \n

   @smc_id
     0x02000502

   @param_id
     0x00000002

   @param[in] addr Address to be written.
   @param[in] val Values to write to address.

   @return
     E_INVALID_ARG on failure; zero otherwise.
*/
#define TZ_IO_ACCESS_WRITE_ID                          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x02)

#define TZ_IO_ACCESS_WRITE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/*
 * Reserved ID.
 */
#define TZ_IO_ACCESS_RESERVED_3_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x03)

/*
 * Reserved ID.
 */
#define TZ_IO_ACCESS_RESERVED_4_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x04)

/**
   @ingroup is_svc_avail

   Checks if a system call is available.

   @smc_id
     0x02000601

   @param_id
     0x00000001

   @sys_call_req_params{tz_info_is_svc_available_req_s}
     @table{weak__tz__info__is__svc__available__req__s}

   @sys_call_rsp_params{tz_info_is_svc_available_rsp_s}
     @table{weak__tz__info__is__svc__available__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_INFO_IS_SVC_AVAILABLE_ID              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x01)

#define TZ_INFO_IS_SVC_AVAILABLE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup get_diag_info

   Copies the QSEE software diagnostic region into the specified buffer.
   Note that this syscall is deprecated and will return 
   SMC_ERR_SYSCALL_NOT_SUPPORTED if invoked.

   @smc_id
     0x02000602

   @param_id
     0x00000022

   @sys_call_params{tz_info_get_diag_req_s}
     @table{weak__tz__info__get__diag__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_INFO_GET_DIAG_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x02)

#define TZ_INFO_GET_DIAG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup get_feature_ver

   Returns the current version of a specified QSEE feature.

   @smc_id
     0x02000603

   @param_id
     0x00000001

   @sys_call_req_params{tz_feature_version_req_s}
     @table{weak__tz__feature__version__req__s}

   @sys_call_rsp_params{tz_feature_version_rsp_s}
     @table{weak__tz__feature__version__rsp__s}

   @returns
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_INFO_GET_FEATURE_VERSION_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x03)

#define TZ_INFO_GET_FEATURE_VERSION_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup get_secure_state

   System call to check the security status and APDP flags of the system.
  
   @smc_id
     0x02000604

   @param_id
     0x00000000

   @sys_call_params{tz_get_secure_state_rsp_s}
     @table{weak__tz__get__secure__state__rsp__s}

   @return
     E_SUCCESS on success; a negative value on failure.

   Response 1: security status
     Bit 0: secboot enabling check failed
     Bit 1: Sec HW key is not programmed
     Bit 2: debug disable check failed
     Bit 3: Anti-rollback check failed
     Bit 4: fuse config check failed
     Bit 5: RPMB provisioned check failed
     Bit 6: debug check in image certificate failed (debug ou field missing from image cert)
  
     Debug Rearch Bits
     Bit 7: RSVD
     Bit 8: tz secure debug fuse check failed
     Bit 9: mss secure debug fuse check failed
     Bit 10: cp secure debug fuse check failed
     Bit 11: non-secure secure debug fuse check failed
*/
#define TZ_INFO_GET_SECURE_STATE                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x4)

#define TZ_INFO_GET_SECURE_STATE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/* TODO: Shifted to SVC_PIL. Remove this ID once APPS changes are promoted.
  Changed ID from TZ_MSS_RESTART to TZBSP_MSS_RESTART to avoid compiler error. */

/**
   @ingroup modem_restart

  System call for modem start and stop. This command is MSM8916-specific.

	@smc_id
		0x02000605

	@param_id
	  0x00000002

	@return
	E_SUCCESS on success; a negative value on failure.
*/
#define TZBSP_MSS_RESTART_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x5)

#define TZBSP_MSS_RESTART_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup get_secure_state_legacy

   System call to check the security status of the system.

   @smc_id
     0x02000606

   @param_id
     0x00000022

   @sys_call_params{tz_get_secure_state_legacy_req_s}
     @table{weak__tz__get__secure__state__legacy__req__s}

   @return
     E_SUCCESS on success; a negative value on failure.
 */
#define TZ_INFO_GET_SECURE_STATE_LEGACY                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x6)

#define TZ_INFO_GET_SECURE_STATE_LEGACY_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup tz_info_bw_prof

   Execute BW profiling command.

   @smc_id
     0x02000607

   @param_id
     0x00000214

   @sys_call_req_params{tz_bw_prof_req_s}
     @table{weak__tz__bw__prof__req__s}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_INFO_BW_PROF_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x07)

#define TZ_INFO_BW_PROF_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup tz_info_get_sec_halt_state

   Get Secure Halt state.

   @smc_id
     0x02000608

   @param_id
     0x00000214

   @sys_call_rsp_params{tz_get_sec_halt_status_rsp_s}
     @table{tz_get_sec_halt_status_rsp_s}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_INFO_GET_SEC_HALT_STATUS_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x08)

#define TZ_INFO_GET_SEC_HALT_STATUS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup hyp_info_get_bootinfo_address

   Syscall trapped in hypervisor and return an address where boot info stored.
   If syscall reaches to TZ, TZ will return syscall not found.
   NOTE: Syscall ID reserved for a third party Hypervisor usecase.
         QSEE/QHEE shouldn't register any service for this syscall.

   @smc_id
     0x02000609

   @param_id
     0x00000000

   @return
     Return E-SUCCESS & bootinfo address; negative value on failure.
*/
#define HYP_INFO_GET_BOOTINFO_ADDRESS_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x09)

#define HYP_INFO_GET_BOOTINFO_ADDRESS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup get_hyp_diag_info

   Copies the HYP software diagnostic region into the specified buffer.

   @smc_id
     0x0200060A

   @param_id
     0x00000022

   @sys_call_params{tz_info_get_hyp_diag_req_s}
     @table{weak__tz__info__get__hyp_diag__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_INFO_GET_HYP_DIAG_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x0A)

#define TZ_INFO_GET_HYP_DIAG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup decrypt_image

   Parses the XML in the encrypted bootloader image and
   decrypts the data in place.

   @smc_id
     0x02000701

   @param_id
     0x000000A2

   @sys_call_params{tz_ssd_decrypt_img_req_s}
     @table{weak__tz__ssd__decrypt__img__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_DECRYPT_IMG_ID                    \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x01)

#define TZ_SSD_DECRYPT_IMG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_BUF_RW)

/**
   @ingroup encrypt_keystore

   Encrypts the keystore with a key derived from the Crypto Engine (CE)
   hardware key.

   @smc_id
     0x02000702

   @param_id
     0x000000A2

   @sys_call_params{tz_ssd_encrypt_keystore_req_s}
     @table{weak__tz__ssd__encrypt__keystore__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_ENCRYPT_KEY_STORE_ID              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x02)

#define TZ_SSD_ENCRYPT_KEY_STORE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_BUF_RW)

/*----------------------------------------------------------------------------
                Deprecated Command ID: Do not reuse
 * -------------------------------------------------------------------------*/

/** @ingroup ssd_deprecated_0

  Deprecated: Use of this command is no longer supported.
*/
#define TZ_SSD_DEPRECATED_0                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x03)

/*----------------------------------------------------------------------------
                Deprecated Command ID: Do not reuse
 * -------------------------------------------------------------------------*/

/** @ingroup ssd_deprecated_1

  Deprecated: Use of this command is no longer supported.
*/
#define TZ_SSD_DEPRECATED_1                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x04)

/**
   @ingroup protect_keystore

   Loads the Secure Software Download (SSD) keystore into the QSEE
   software for future use.

   @smc_id
     0x02000705

   @param_id
     0x00000022

   @sys_call_req_params{tz_ssd_protect_keystore_req_s}
     @table{weak__tz__ssd__protect__keystore__req__s}

   @sys_call_rsp_params{tz_ssd_protect_keystore_rsp_s}
     @table{weak__tz__ssd__protect__keystore__rsp__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_PROTECT_KEYSTORE_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x05)

#define TZ_SSD_PROTECT_KEYSTORE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup parse_md

   Parses the SSD XML header in a given encrypted image and stores the
   resulting metadata to use for subsequent calls to decrypt image
   fragments.

   @smc_id
     0x02000706

   @param_id
     0x00000082

   @sys_call_req_params{tz_ssd_parse_md_req_s}
     @table{weak__tz__ssd__parse__md__req__s}

   @par Parse MD response structure (tz_ssd_parse_md_rsp_info_s)
     @table{weak__tz__ssd__parse__md__rsp__info__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_PARSE_MD_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x06)

#define TZ_SSD_PARSE_MD_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup decrypt_img_frag

   Decrypts the data in place for a specified fragment of an encrypted image.

   @smc_id
     0x02000707

   @param_id
     0x00000804

   @sys_call_req_params{tz_ssd_decrypt_img_frag_req_s}
     @table{weak__tz__ssd__decrypt__img__frag__req__s}

   @sys_call_rsp_params{tz_ssd_decrypt_frag_rsp_s}
     @table{weak__tz__ssd__decrypt__frag__rsp__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_DECRYPT_IMG_FRAG_ID               \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x07)

#define TZ_SSD_DECRYPT_IMG_FRAG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup decrypt_elf_seg_frag

   Decrypts the data in place for a specified ELF segment from an
   encrypted image.

   @smc_id
     0x02000708

   @param_id
     0x00008006

   @sys_call_req_params{tz_ssd_decrypt_elf_seg_frag_req_s}
     @table{weak__tz__ssd__decrypt__elf__seg__frag__req__s}

   @sys_call_rsp_params{tz_ssd_decrypt_frag_rsp_s}
     @table{weak__tz__ssd__decrypt__frag__rsp__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_DECRYPT_ELF_SEG_FRAG_ID               \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x08)

#define TZ_SSD_DECRYPT_ELF_SEG_FRAG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup blow_sw_fuse

   Changes the state of the software fuse.

   @smc_id
     0x02000801

   @param_id
     0x00000001

   @sys_call_params{tz_sw_fuse_req_s}
     @table{weak__tz__sw__fuse__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_BLOW_SW_FUSE_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x01)

#define TZ_BLOW_SW_FUSE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup is_sw_fuse_blown

   Queries whether the specified software fuse has been blown.

   @caution1hang This call is not thread safe.

   @smc_id
     0x02000802

   @param_id
     0x00000001

   @sys_call_params{tz_sw_fuse_rsp_s}
     @table{weak__tz__sw__fuse__rsp__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_IS_SW_FUSE_BLOWN_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x02)

#define TZ_IS_SW_FUSE_BLOWN_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup write_qfprom_fuse

   Writes a Qualcomm Field Programmable Read Only Memory (QFPROM) fuse row.

   @smc_id
     0x02000803

   @param_id
     0x00000884

   @sys_call_params{tz_qfprom_write_row_s}
     @table{weak__tz__qfprom__write__row__s}

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_QFPROM_WRITE_ROW_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x03)

#define TZ_QFPROM_WRITE_ROW_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup write_mult_qfprom_rows

   Writes multiple rows of QFPROM fuses.

   @note1hang This call is only available prior to a
              #TZ_MILESTONE_SET_ID call.

   @smc_id
     0x02000804

   @param_id
     0x00000824

   @sys_call_params{tz_qfprom_write_multiple_rows_s}
     @table{weak__tz__qfprom__write__multiple__rows__s}

   @return
     E_SUCCESS on success; a negative value on failure.
 */
#define TZ_QFPROM_WRITE_MULTIPLE_ROWS_ID         \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x04)

#define TZ_QFPROM_WRITE_MULTIPLE_ROWS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup read_qfprom_fuse

   Reads QFPROM fuses.

   @smc_id
     0x02000805

   @param_id
     0x00000A04

   @sys_call_params{tz_qfprom_read_row_s}
     @table{weak__tz__qfprom__read__row__s}

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_QFPROM_READ_ROW_ID                    \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x05)

#define TZ_QFPROM_READ_ROW_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup write_rollback_qfprom_fuse

   Write rollback-specific QFPROM fuses.

   @note1hang This call is only available before a
              #TZ_MILESTONE_SET_ID call.

   @smc_id
     0x02000806

   @param_id
     0x00000884

   @sys_call_params{tz_qfprom_rollback_write_row_s}
     @table{weak__tz__qfprom__rollback__write__row__s}

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_QFPROM_ROLLBACK_WRITE_ROW_ID          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x06)

#define TZ_QFPROM_ROLLBACK_WRITE_ROW_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup Secure Boot Register Read

   Reads Authentication Bit in Secure Boot Register

   @command_id
     0x02000807
   @param_id
     0x00000012
   @sys_call_params{secure_boot_val, secure_boot_val_size}

   @return
     E_SUCCESS on success; negative value on failure
*/

#define TZ_SECURE_BOOT_CHECK_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x07)

#define TZ_SECURE_BOOT_PARAM_ID TZ_SYSCALL_CREATE_PARAM_ID_2(\
  TZ_SYSCALL_PARAM_TYPE_BUF_RO, TZ_SYSCALL_PARAM_TYPE_VAL)




/**
   @ingroup Version Register Read

   Reads value from Version Register

   @smc_id
     0x2000808
   @param_id
     0x00000885

   @sys_call_params{sw_id, version,version_size, qfprom_status, qfprom_status_ptr_size}

   @return
     E_SUCCESS on success; negative value on failure
*/



#define TZ_VERSION_REGISTER_READ_ID             \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x08)


#define TZ_VERSION_REGISTER_READ_PARAM_ID       TZ_SYSCALL_CREATE_PARAM_ID_5(\
TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)

/**
@ingroup version register write



Increase anti-rollback fuse version by one and udpate in fuse for requested sw_type



@smc_id
0x2000809



@param_id
0x00000000



@com_struct
tzbsp_syscall_req_s



@sys_call_params{sw_type}



@return
Zero on success; negative value on failure.
*/
#define TZ_VERSION_REGISTER_WRITE_ID \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x09)



#define TZ_VERSION_REGISTER_WRITE_PARAM_ID \
TZ_SYSCALL_CREATE_PARAM_ID_0


/**
   @ingroup blow_fuse_secdat

   Blow fuses specified in the sec.dat file

   @smc_id
     0x02000820

   @param_id
     0x00000011

   @sys_call_params{tz_blow_fuse_secdat_s}
     @table{weak__tz__blow__fuse__secdat__s}

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_BLOW_FUSE_SECDAT_ID          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x20)

#define TZ_BLOW_FUSE_SECDAT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_BUF_RO )


/**
   @ingroup Send_dpr_load_address_tmel via IPC

   Changes the state of the software fuse.

   @smc_id
     0x02000821

   @param_id
     0x00000001

   @sys_call_params{}
     @table{}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_DPR_SEND_LOAD_ADDRESS_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x21)

#ifndef CHIPSET_53xx
#define TZ_DPR_SEND_LOAD_ADDRESS_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )
#else
/* DPR authentication smc call will need 2 arguments for Miami 
   (i.e.) address where the DPR binary is loaded and size*/
#define TZ_DPR_SEND_LOAD_ADDRESS_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )
#endif

#ifdef CHIPSET_53xx
/**
   @ingroup Send Fuse read multiple rows IPC call to TME-L

   Reads the QFPROM fuse row values by sending an IPC call to TME-L.

   @smc_id
     0x02000822

   @param_id
     0x0000022

   @sys_call_params{}
     @table{}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_FUSE_READ_MULTIPLE_ROWS_TMEL_IPC_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x22)

#define TZ_FUSE_READ_MULTIPLE_ROWS_TMEL_IPC_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2(TZ_SYSCALL_PARAM_TYPE_BUF_RW,TZ_SYSCALL_PARAM_TYPE_VAL)
#endif


/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/

/** @ingroup pwr_spmi_disable_bus_dep

  This command is deprecated. Use #TZ_POWER_SPMI_DISABLE_BUS_ID.
*/
#define TZ_POWER_SPMI_DISABLE_BUS_DEPRECATED_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_POWER, 0x01)

/**
   @ingroup spmi_disable_bus

   Disables the SPMI bus to allow clean shutdowns, then deasserts PS_HOLD.

   @note1hang Implementation takes place entirely in Monitor mode. \n
   @note1hang This command uses a register-based QSEE interface.

   @smc_id
     0x02000902

   @param_id
     0x00000001

   @param[in] Spare Reserved/spare; set to zero.

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_POWER_SPMI_DISABLE_BUS_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_POWER, 0x02)

#define TZ_POWER_SPMI_DISABLE_BUS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tz_power_bus_bw

   Returns an estimate of bus bandwidth requirements.

   @smc_id
     0x02000904

   @param_id
     0x00000000

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_POWER_BUS_BW_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_POWER, 0x04)

#define TZ_POWER_BUS_BW_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup gpu_power_down_notif

   Indicates whether the GPU is powering down.

   @smc_id
     0x02000908

   @param_id
     0x00000000

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define HYP_GPU_POWER_DOWN_NOTIFICATION_ID             \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_POWER, 0x08)

#define HYP_GPU_POWER_DOWN_NOTIFICATION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup get_prng_data

   Retrieves random data from the Pseudo-Random Number Generator (PRNG)
   hardware.

   @smc_id
     0x02000A01

   @param_id
     0x00000022

   @sys_call_params{tz_get_prng_data_s}
     @table{weak__tz__get__prng__data__s}

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_PRNG_GETDATA_ID                         \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x01)

#define TZ_PRNG_GETDATA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup hyp_get_prng_data

   Retrieves random data from the PRNG
   hardware. This version of the call should only be made from Hypervisor.

   @smc_id
     0x02000A02

   @param_id
     0x00000022

   @sys_call_params{hyp_get_prng_data_s}
     @table{weak__hyp__get__prng__data__s}

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define HYP_PRNG_GETDATA_ID                         \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x02)

#define HYP_PRNG_GETDATA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup kdf_generate_key

   Generate Key (SW_KDF(HW_KDF(SHDK)) and lock Crypto SW Key register
   @smc_id
     0x02000A03

   @param_id
     0x00000011


   @return
     E_SUCCESS on success; a negative value on failure.
*/

#define TZ_KDF_KEY_GENERATE_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x03)

#define TZ_KDF_KEY_GENERATE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RO )



/**
   @ingroup unlock SW_CRYPTO register

   @smc_id
     0x02000A04

   @param_id
     0x0

   @return
     E_SUCCESS on success; a negative value on failure.
*/


#define TZ_KDF_KEY_REG_UNLOCK_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x04)

#define TZ_KDF_KEY_REG_UNLOCK_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup
   Support of  generation of key/sign/verify adn export keys
    for Asymmetric crypto algos(RSA)
   @command_id
     0x00002A05

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_CRYPTO_ASYM_SERVICE_ID                         \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x05)
#define TZ_CRYPTO_ASYM_SERVICE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL,\
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup

   Support of  generation/seal/unseal of data for symmetirc
   crypto algo(AES_CCM)

   @command_id
     0x02000A06

   @sys_call_params{tz_get_prng_data_s}
     @table{weak__tz__get__prng__data__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_CRYPTO_SERVICE_SYM_ID                         \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x06)
#define TZ_CRYPTO_SERVICE_SYM_PARAM_ID \
 TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL,\
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup
   Perform AES Encryption using the OEM Prod seed/CRBK derived keys

   @smc_id
     0x02000A07

   @param_id
     0x00000022

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_CRYPTO_SERVICE_AES_ENC_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x07)
#define TZ_CRYPTO_SERVICE_AES_ENC_PARAM_ID \
 TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup
   Perform AES Decryption using OEM Prod seed/CRBK derived keys

   @smc_id
     0x02000A08

   @param_id
     0x00000022

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_CRYPTO_SERVICE_AES_DEC_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x08)
#define TZ_CRYPTO_SERVICE_AES_DEC_PARAM_ID \
 TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup
   Perform Hardware Key derivation 

   @smc_id
     0x02000A09

   @param_id
     0x00000022

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_HW_KEY_DERIVATION_SERVICE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x09)
#define TZ_HW_KEY_DERIVATION_PARAM_ID \
 TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup
   Performs Key Clear in TMEL internal sw key table

   @smc_id
     0x02000A0A

   @param_id
     0x00000001

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_TMEL_KEY_CLEAR_SERVICE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x0A)
#define TZ_TMEL_KEY_CLEAR_PARAM_ID \
 TZ_SYSCALL_CREATE_PARAM_ID_1( \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup
   Perform AES Encryption using the sec nand key 

   @smc_id
     0x02000A0B

   @param_id
     0x00000022

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_CRYPTO_SERVICE_SEC_NAND_AES_ENC_DEC_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x0B)
#define TZ_CRYPTO_SERVICE_SEC_NAND_AES_ENC_DEC_PARAM_ID \
 TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup kdf_generate_key

   Clear the key used for Sec NAND encryption and decryption
   @smc_id
     0x02000A0C

   @param_id
     0x00000000


   @return
     E_SUCCESS on success; a negative value on failure.
*/

#define TZ_SEC_NAND_CLEAR_KEY_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x0C)

#define TZ_SEC_NAND_CLEAR_KEY_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup 

   Get dervided key from TME-L based on OEM PROD SEED

   @smc_id
     0x02000A0D

   @param_id
     0x00000845


   @sys_call_req_params{}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_HLOS_HW_KDF_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0xD)

#define TZ_HLOS_HW_KDF_PARAM_ID\
  TZ_SYSCALL_CREATE_PARAM_ID_5(\
      TZ_SYSCALL_PARAM_TYPE_VAL,\
      TZ_SYSCALL_PARAM_TYPE_BUF_RO,\
      TZ_SYSCALL_PARAM_TYPE_VAL,\
      TZ_SYSCALL_PARAM_TYPE_BUF_RW,\
      TZ_SYSCALL_PARAM_TYPE_VAL)



/**
   @ingroup init_secure_channel

   Writes the key material to the specified address.

   This function returns E_ALREADY_DONE if the keys are already written.

   @smc_id
     0x02000B01

   @param_id
     0x00000000

   @sys_call_params{tz_sc_init_s}
     @table{weak__tz__sc__init__s}

   @return
     E_SUCCESS      -- Function was successful. \n
     E_FAILURE      -- Secure channel could not be initialized. \n
     E_ALREADY_DONE -- Secure channel is already established.
*/
#define TZ_SECURE_CHANNEL_INIT_ID                            \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SC_KEY_EXCHANGE, 0x01)

/* TODO Syscall does not exist */
#define TZ_SECURE_CHANNEL_INIT_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup deinit_secure_channel

   Clears the keys and resets the channel state.

   @smc_id
     0x02000B02

   @param_id
     0x00000000

   @sys_call_params{tz_sc_deinit_s}
     @table{weak__tz__sc__deinit__s}

   @return
     E_SUCCESS on success; E_FAILURE otherwise.
*/
#define TZ_SECURE_CHANNEL_DEINIT_ID                          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SC_KEY_EXCHANGE, 0x02)

/* TODO Syscall does not exist */
#define TZ_SECURE_CHANNEL_DEINIT_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

 /*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
/*
   @ingroup protect_memory

   Deprecated: Use of this command is no longer supported.
   This Syscall is deprecated. MPU memory range protection is not exposed to the NS side.
*/
#define TZ_PROTECT_MEMORY_DEPRECATED_0  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x01)

/**
   @ingroup install_nonsecure_debug_fiq

   Installs a nonsecure debug FIQ handler.

   @smc_id
     0x02000E01

   @param_id
     0x00000256

   @sys_call_params{tz_config_ns_debug_fiq_req_s}
     @table{weak__tz__config__ns__debug__fiq__req__s}

   @return
      E_SUCCESS if the nonsecure debug FIQ handler and context were
      installed successfully; error code otherwise.
*/
#define TZ_NS_DEBUG_FIQ_CONFIG_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_NS_DEBUG_FIQ, 0x01)

#define TZ_NS_DEBUG_FIQ_CONFIG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RO, TZ_SYSCALL_PARAM_TYPE_BUF_RO, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup query_nonsecure_debug_fiq_ctx_buf

   Queries the minimum size of a nonsecure debug FIQ context buffer. A pointer
   to the context buffer is passed to the nonsecure handler in register r0.

   @smc_id
     0x02000E02

   @param_id
     0x00000022

   @sys_call_req_params{tz_config_ns_debug_fiq_ctx_size_req_s}
     @table{weak__tz__config__ns__debug__fiq__ctx__size__req__s}

   @sys_call_rsp_params{tz_config_ns_debug_fiq_ctx_size_rsp_s}
     @table{weak__tz__config__ns__debug__fiq__ctx__size__rsp__s}

   @return
     E_SUCCESS if the context buffer size query was successful; error
     code otherwise.
*/
#define TZ_NS_DEBUG_FIQ_CTX_SIZE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_NS_DEBUG_FIQ, 0x02)

#define TZ_NS_DEBUG_FIQ_CTX_SIZE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup query_nonsecure_debug_fiq_int

   Queries whether an interrupt is allowed to be configured as a nonsecure
   debug FIQ. The QSEE only allows a limited set of chipset-specific interrupts
   to be used for this purpose.

   @smc_id
     0x02000E03

   @param_id
     0x00000083

   @sys_call_params{tz_config_ns_debug_fiq_int_ok_req_s}
     @table{weak__tz__config__ns__debug__fiq__int__ok__req__s}

   @sys_call_params{tz_config_ns_debug_fiq_int_ok_rsp_s}
     @table{weak__tz__config__ns__debug__fiq__int__ok__rsp__s}

   @return
     E_SUCCESS if the context buffer size query was successfull; error
     code otherwise.
*/
#define TZ_NS_DEBUG_FIQ_INT_OK_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_NS_DEBUG_FIQ, 0x03)

#define TZ_NS_DEBUG_FIQ_INT_OK_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup restore_sec_cfg

   Restores VMIDMT/xPU/SMMU configuration of a device's System Memory
   Management Unit (SMMU) global space.

   @note1hang SMMU clocks must be ON.

   @smc_id
     0x02000C02

   @param_id
     0x00000002

   @sys_call_params{tz_restore_sec_cfg_req_s}
     @table{weak__tz__restore__sec__cfg__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_RESTORE_SEC_CFG  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x02)

#define TZ_RESTORE_SEC_CFG_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_get_ptbl_mem_size

   Queries the memory size requirement for multimedia SMMU page
   tables.

   @smc_id
     0x02000C03

   @param_id
     0x00000001

   @sys_call_req_params{tz_memprot_ptbl_mem_size_req_s}
     @table{weak__tz__memprot__ptbl__mem__size__req__s}

   @sys_call_rsp_params{tz_memprot_ptbl_mem_size_rsp_s}
     @table{weak__tz__memprot__ptbl__mem__size__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_GET_PTBL_MEM_SIZE  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x03)

#define TZ_MEM_PROTECT_GET_PTBL_MEM_SIZE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_ptbl_mem_init

   Provides memory used for page tables by multimedia SMMU
   devices involved in content protection.

   @smc_id
     0x02000C04

   @param_id
     0x00000023

   @sys_call_params{tz_memprot_ptbl_mem_init_req_s}
     @table{weak__tz__memprot__ptbl__mem__init__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_PTBL_MEM_INIT  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x04)

#define TZ_MEM_PROTECT_PTBL_MEM_INIT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup mem_protect_set_cp_pool_size

   Provide the CP pool size to calculate translation table
   requirement

   @smc_id
     0x02000C05

   @param_id
     0x00000002

   @sys_call_params{tz_memprot_set_cp_pool_size_req_s}
     @table{weak__tz__memprot__set__cp__pool__size__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_SET_CP_POOL_SIZE TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x05)

#define TZ_MEM_PROTECT_SET_CP_POOL_SIZE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 @ingroup tzbsp_mpu_lock_audio_buffer

   Locks the Audio buffer in dynamically allocated partition

   @command_id
     0x02000C06
	 
   @param_id
     0x00000002

   @common_structs
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_MPU_LOCK_AUDIO_BUFFER  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x06)
#define TZ_MPU_LOCK_AUDIO_BUFFER_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
  TZ_SYSCALL_PARAM_TYPE_VAL , TZ_SYSCALL_PARAM_TYPE_VAL)


/**
@ingroup sk_hibernate

From TrEE to SK to tell SKExtension to save state and unload clients during hibernate

@smc_id
0x0200090A

@param_id
0x00000001

@sys_call_params{sk_hibernate_req_s}
@table{weak__sk__hibernate__req__s}

@return
E_SUCCESS on success; an error code otherwise.
*/
#define SK_HIBERNATE   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_POWER, 0x0A)   

#define SK_HIBERNATE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_video_var

   Sets secure Virtual Address Ranges (VARs) in the video device's
   virtual address map.

   @smc_id
     0x02000C08

   @param_id
     0x00000004

   @sys_call_params{tz_memprot_video_var_req_s}
     @table{weak__tz__memprot__video__var__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_VIDEO_VAR TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x08)

#define TZ_MEM_PROTECT_VIDEO_VAR_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup vmidmt_set_memtype

   Sets the memtype attribute for a Virtual Machine ID Mapping Table (VMIDMT)
   associated with a given device.

   @smc_id
     0x02000C09

   @param_id
     0x00000003

   @sys_call_params{tz_set_vmidmt_memtype_req_s}
     @table{weak__tz__set__vmidmt__memtype__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_VMIDMT_SET_MEMTYPE  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x09)

#define TZ_VMIDMT_SET_MEMTYPE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup disable_mmss_sec

   Disable the QRIB security for the Multimedia Subsystems (MMSS).

   @smc_id
     0x02000C20

   @param_id
     0x00000000

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_DISABLE_MMSS_SEC  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x20)

#define TZ_DISABLE_MMSS_SEC_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup tzbsp_xpu_enable_toggle

   Enables/disables an XPU using the base address of the XPU block as the input.

   @owner_id
     0x00000002

   @command_id
     0x00000C23

   @param_id
     0x00000001

   @common_structs
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_ENABLE_TOGGLE_XPU  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x23)

#define TZ_ENABLE_TOGGLE_XPU_PARAM_ID                           \
  TZ_SYSCALL_CREATE_PARAM_ID_2(TZ_SYSCALL_PARAM_TYPE_VAL, \
                               TZ_SYSCALL_PARAM_TYPE_VAL)

/**
 	@ingroup tzbsp_mpu_lock_hlos_memory
 	Locks the HLOS region(code region) in dynamically allocated partition
 	@owner_id
 	0x00000002
 	@command_id
 	0x00000C24
 	@param_id
 	0x00000001
 	@common_structs
 	tz_syscall_req_s \n
 	tz_syscall_rsp_s

 	@return
 	E_SUCCESS on success; an error code otherwise.
 */
 #define TZ_MPU_LOCK_HLOS_REGION  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x24)

 
#define TZ_MPU_LOCK_HLOS_REGION_PARAM_ID          \
  TZ_SYSCALL_CREATE_PARAM_ID_2(TZ_SYSCALL_PARAM_TYPE_VAL, \
                               TZ_SYSCALL_PARAM_TYPE_VAL)
							   
/**
 	@ingroup tzbsp_mpu_lock_ns_memory
 	Locks the non secure region(code region) in dynamically allocated partition. Checks for 
	overlapping with already locked address and non-secure memory range before locking the region. 
 	@owner_id
 	0x00000002
 	@command_id
 	0x00000C25
 	@param_id
 	0x00000104
 	@sys_call_params{hyp_mem_lock_ns_region_t}
 	tz_syscall_req_s \n
 	tz_syscall_rsp_s

 	@return
 	E_SUCCESS on success; an error code otherwise.
 */
  #define TZ_MPU_LOCK_NS_REGION  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x25)


#define TZ_MPU_LOCK_NS_REGION_PARAM_ID          \
  TZ_SYSCALL_CREATE_PARAM_ID_4(TZ_SYSCALL_PARAM_TYPE_VAL, \
                               TZ_SYSCALL_PARAM_TYPE_VAL, \
                               TZ_SYSCALL_PARAM_TYPE_BUF_RO, \
                               TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_lock2_legacy

  Protects or removes protection from a list of memory chunks (legacy).

   @smc_id
     0x02000C0A

   @param_id
     0x00000024

   @sys_call_params{tz_memprot_lock2_legacy_req_s}
     @table{weak__tz__memprot__lock2__legacy__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_LOCK2   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0A)

#define TZ_MEM_PROTECT_LOCK2_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_map2_legacy

   Maps a list of memory chunks (sets up VA to PA mapping) on a
   QSEE-managed context bank (legacy).

   @smc_id
     0x02000C0B

   @param_id
     0x000000A3

   @sys_call_params{tz_memprot_map2_legacy_req_s}
     @table{weak__tz__memprot__map2__legacy__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_MAP2    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0B)

#define TZ_MEM_PROTECT_MAP2_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_unmap2_legacy

   Removes VA to PA mapping on a QSEE-managed context bank (legacy).

   @smc_id
     0x02000C0C

   @param_id
     0x00000022

   @sys_call_params{tz_memprot_unmap2_legacy_req_s}
     @table{weak__tz__memprot__unmap2__legacy__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_UNMAP2  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0C)

#define TZ_MEM_PROTECT_UNMAP2_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup tz_change_pagetable_format

	Enables/disables the 64-bit address translation for Stage 1 CBs.

  The purpose of this function is to enable/disable the setting responsbile
  for 64-bit address translation.

   @smc_id
     0x02000F01

   @param_id
     0x00000003

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_CHANGE_PAGETABLE_FORMAT    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SMMU_PROGRAM, 0x1)

#define TZ_CHANGE_PAGETABLE_FORMAT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup smmu_tz_boost_tcu_and_close_s1_gate

	Does Boosting of the TCU and TLB inval for the given VM.

  The purpose of this function is to do safe tlb inval for the said VMID.

   @smc_id
     0x02000F02

   @param_id
     0x00000002

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SMMU_BOOST_CLOSE_S1GATE TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SMMU_PROGRAM, 0x2)

#define TZ_SMMU_BOOST_CLOSE_S1GATE_PARAM_ID  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup smmu_tz_config_safe_eco

	Enables/Disables Safe Eco for IFE/MDP

  The purpose of this function is to enable the Safe [one pop on safe] for IFE and MDP clients.

   @smc_id
     0x02000F03

   @param_id
     0x00000002

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SMMU_CONFIG_SAFE_ECO TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SMMU_PROGRAM, 0x3)

#define TZ_SMMU_CONFIG_SAFE_ECO_PARAM_ID  TZ_SYSCALL_CREATE_PARAM_ID_2(TZ_SYSCALL_PARAM_TYPE_VAL,TZ_SYSCALL_PARAM_TYPE_VAL)

/**
@ingroup tz_smmu_transfer_ns_fault_int_id

Transfers the S2 SMMU global and sb fault interrupts to EL2 when interrupt virtualization is enabled.

The purpose of this function is to transfer the s2 global and sb interrupts from secure to
 to NS EL2 when interrupt virtualization is enabled (Hypervisor can handle interrupts).

@smc_id
0x02000F04

@param_id
0x00000000

@return
E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SMMU_TRANSFER_NS_FAULT_INT_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP,TZ_SVC_SMMU_PROGRAM,0x4)

#define TZ_SMMU_TRANSFER_NS_FAULT_INT_PARAM_ID TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup mem_protect_tlbinval

   Invalidates Translation Lookaside Buffer (TLB) entries associated with a
   QSEE-managed context bank.

   @smc_id
     0x02000C0D

   @param_id
     0x00000004

   @sys_call_params{tz_memprot_tlbinval_req_s}
     @table{weak__tz__memprot__tlbinval__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_TLBINVALIDATE  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0D)

#define TZ_MEM_PROTECT_TLBINVALIDATE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup config_xpu_violation_err_fatal

   Subroutine used by the HLOS to enable/disable xPU violations becoming fatal
   errors. This can also be used to check whether xPU violations are currently
   fatal errors by sending TZ_XPU_VIOLATION_ERR_FATAL_NOOP as the parameter.

   The TZ software may choose to force enable/disable xPU violations as fatal
   errors by default. It may also choose to disallow the HLOS from overriding
   this configuration. So, the HLOS must always check the output parameter
   (enabled) to see the current behavior and the return code to see if it was
   allowed to carry out the operation (E_NOT_SUPPORTED is returned if the HLOS
   is not allowed to change the behavior).

   @smc_id
     0x02000C0E

   @param_id
     0x00000002

   @sys_call_req_params{tz_xpu_violation_err_fatal_req_s}
     @table{weak__tz__xpu__violation__err__fatal__req__s}

   @sys_call_rsp_params{tz_xpu_violation_err_fatal_rsp_s}
     @table{weak__tz__pil__subsystem__query__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_CONFIG_XPU_VIOLATION_ERR_FATAL                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0E)

#define TZ_CONFIG_XPU_VIOLATION_ERR_FATAL_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup secure_display_ctrl

   Notifies the TZ software that secure display is enabled or disabled.

   @owner_id
     0x00000002

   @command_id
     0x00000C0F

   @param_id
     0x00000001

   @sys_call_params{tz_memprot_sd_ctrl_req_s}
     @table{weak__tz__memprot__sd__ctrl__req__s}

   @common_structs
      tz_common_rsp_s

   @return
     E_SUCCESS on success; error code otherwise.
*/
#define TZ_MEM_PROTECT_SD_CTRL                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0F)

#define TZ_MEM_PROTECT_SD_CTRL_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_lock2_flat

  Protects or removes protection from a list of memory chunks.

   @smc_id
     0x02000C11

   @param_id
     0x00000026

   @sys_call_params{tz_memprot_lock2_req_s}
     @table{weak__tz__memprot__lock2__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_LOCK2_FLATTENED   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x11)

#define TZ_MEM_PROTECT_LOCK2_FLATTENED_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_map2_flat

   Maps a list of memory chunks (sets up VA to PA mapping) on a
   QSEE-managed context bank.

   @smc_id
     0x02000C12

   @param_id
     0x00000028

   @sys_call_params{tz_memprot_map2_req_s}
     @table{weak__tz__memprot__map2__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_MAP2_FLATTENED    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x12)

#define TZ_MEM_PROTECT_MAP2_FLATTENED_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_8( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_unmap2_flat

   Removes VA to PA mapping on a QSEE-managed context bank.

   @smc_id
     0x02000C13

   @param_id
     0x00000005

   @sys_call_params{tz_memprot_unmap2_req_s}
     @table{weak__tz__memprot__unmap2__req__s}

   @sys_call_rsp_params{tz_memprot_rsp_s}
     @table{weak__tz__memprot__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_UNMAP2_FLATTENED  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x13)

#define TZ_MEM_PROTECT_UNMAP2_FLATTENED_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup hyp_mem_protect_sd_ctrl

   Enables/disables secure display.

   @owner_id
     0x00000002

   @command_id
     0x00000C18

   @sys_call_params{hyp_memprot_sd_ctrl_s}
     @table{weak__hyp__memprot__sd_ctrl__s}

   @common_structs
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_MEM_PROTECT_SD_CTRL                   \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x14)

/**
   @ingroup mem_protect_dev_var

   Sets secure Virtual Address Ranges (VAR) in the requested
   device's virtual address map in an specific device.

   @smc_id
     0x02000C15

   @param_id
     0x00000000

   @sys_call_params{tz_memprot_dev_var_req_s}
     @table{weak__tz__memprot__dev__var__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_DEVICE_VAR                        \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x15)

/* TODO Syscall does not exist */
#define TZ_MEM_PROTECT_DEVICE_VAR_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup mem_protect_assign

   Assigns a list of intermediate PAs from a
   source Virtual Machine (VM) to a destination VM.

   @smc_id
     0x02000C16

   @sys_call_params{hyp_memprot_assign_s}
     @table{weak__hyp__memprot__assign__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_MEM_PROTECT_ASSIGN                   \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x16)

#define HYP_MEM_PROTECT_ASSIGN_PARAM_ID                   \
   TZ_SYSCALL_CREATE_PARAM_ID_7(TZ_SYSCALL_PARAM_TYPE_BUF_RO,   \
                                TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO,   \
                                TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_BUF_RO,   \
                                TZ_SYSCALL_PARAM_TYPE_VAL,      \
                                TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup mem_protect_tag

   Memory that is assigned using HYP_MEM_PROTECT_ASSIGN for	a
   CPZ usecase is tagged in the TZ software using TZ_MEM_PROTECT_TAGVM
   for applications to know if memory is tagged.

   @smc_id
     0x02000C17

   @sys_call_params{tz_memprot_tagvm_s}
     @table{weak__tz__memprot__tagvm__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_TAGVM                   \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x17)

#define TZ_MEM_PROTECT_TAGVM_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup sid_protect_assign

   Performs a use case-specific dynamic SID assignment.

   @smc_id
     0x02000C18

   @sys_call_params{hyp_sidprot_assign_s}
     @table{weak__hyp__sidprot__assign__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_SID_PROTECT_ASSIGN                   \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x18)

#define HYP_SID_PROTECT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup tz_assign_io_range

   Applies use case-specific XPU protection.

   @smc_id
     0x02000C19

   @sys_call_params{tz_assign_io_range_s}
     @table{weak__tz__assign__io__range__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_ASSIGN_IO_RANGE                   \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x19)

#define TZ_ASSIGN_IO_RANGE_PARAM_ID          \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup allow_access_smmu_protected_regions

   Maps regions to HLOS so it can dump on NS device.
   Only to be used for debug purposes
   It only opens up pre-defined whitelisted address range

   @smc_id
     0x02000C1A

   @sys_call_params{tz_assign_io_range_s}
     @table{weak__tz__assign__io__range__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_ALLOW_ACCESS_TO_SMMU_PROTECTED_REGIONS                   \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x1A)

#define HYP_ALLOW_ACCESS_TO_SMMU_PROTECTED_REGIONS_PARAM_ID          \
    TZ_SYSCALL_CREATE_PARAM_ID_5(TZBSP_SYSCALL_PARAM_TYPE_BUF_RO,   \
                                 TZBSP_SYSCALL_PARAM_TYPE_VAL,      \
                                 TZBSP_SYSCALL_PARAM_TYPE_BUF_RO,   \
                                 TZBSP_SYSCALL_PARAM_TYPE_VAL,      \
                                 TZBSP_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup camerass_security_status

   TBD

   @smc_id
     0x02001801

   @param_id
     0x00000000

   @param[out] status Security staus of the CameraSS.

   @return
     Zero on success, negative value otherwise.
*/
#define TZ_SVC_CAMERASS_SECURITY_STATUS \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CAMERASS, 0x01)

#define TZ_SVC_CAMERASS_SECURITY_STATUS_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup camerass_reg_read

   TBD

   @smc_id
     0x02001802

   @param_id
     0x00000002

   @param[in] region Region to be read.
   @param[in] offset Offset to be read.
   @param[out] val Value at that offset.

   @return
     Zero on success, negative value otherwise.
*/
#define TZ_SVC_CAMERASS_REG_READ \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CAMERASS, 0x02)

#define TZ_SVC_CAMERASS_REG_READ_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
    TZ_SYSCALL_PARAM_TYPE_VAL, \
    TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup camerass_reg_write

   TBD

   @smc_id
     0x02001803

   @param_id
     0x00000003

   @param[in] region Region to be written.
   @param[in] offset Offset to be written.
   @param[in] val Values to write to offset.

   @return
     E_INVALID_ARG on failure; zero otherwise.
*/
#define TZ_SVC_CAMERASS_REG_WRITE \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CAMERASS, 0x03)

#define TZ_SVC_CAMERASS_REG_WRITE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup camerass_reset_hw_block

   TBD

   @smc_id
     0x02001805

   @param_id
     0x00000002

   @param[in] region Region to be reset.
   @param[in] mask Address to be reset.

   @return
     Zero on success, negative value otherwise.
*/
#define TZ_SVC_CAMERASS_RESET_HW_BLOCK \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CAMERASS, 0x05)

#define TZ_SVC_CAMERASS_RESET_HW_BLOCK_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup camerass_dynamic_phy_protect

   System call to dynamically protect / unprotect camera PHYs
   Exepected to be called by camera HLOS driver

   @smc_id
     0x02001806

   @param_id
     0x00000002

   @param[in] protect (1) or unprotect (0)
   @param[in] phy_id - phy ID to protect/unprotect

   @return
     E_SUCCESS on success, error otherwise.
*/
#define TZ_SVC_CAMERASS_DYNAMIC_PHY_PROTECT \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CAMERASS, 0x06)

#define TZ_SVC_CAMERASS_DYNAMIC_PHY_PROTECT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup camerass_dynamic_phy_protect

   System call to dynamically protect / unprotect camera PHY LANEs
   Exepected to be called by camera HLOS driver

   @smc_id
     0x02001807

   @param_id
     0x00000002

   @param[in] protect (1) or unprotect (0)
   @param[in] phy_id_mask - CPHY/DPHY Lanes to toggle

   @return
     E_SUCCESS on success, error otherwise.
*/
#define TZ_SVC_CAMERASS_DYNAMIC_PHY_PROTECT_LANES \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CAMERASS, 0x07)

#define TZ_SVC_CAMERASS_DYNAMIC_PHY_PROTECT_LANES_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup camerass_set_phy_topology

   System call to set camera topology before starting seccam
   Exepected to be called by camera HLOS driver

   @smc_id
     0x02001808

   @param_id
     0x00000002

   @param[in] phy_id - phy_id
   @param[in] topology - Camare topology mapped to phy_id

   @return
     E_SUCCESS on success, error otherwise.
*/
#define TZ_SVC_CAMERASS_SET_TOPOLOGY \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CAMERASS, 0x08)

#define TZ_SVC_CAMERASS_SET_TOPOLOGY_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup camerass_notify_stream_status

   System call to notify streaming start/stop
   Exepected to be called by camera HLOS driver

   @smc_id
     0x02001809

   @param_id
     0x00000002

   @param[in] phy_id - phy_id mapped to topology
   @param[in] streaming - start/stop stream

   @return
     E_SUCCESS on success, error otherwise.
*/
#define TZ_SVC_CAMERASS_NOTIFY_STREAM_STATUS \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CAMERASS, 0x09)

#define TZ_SVC_CAMERASS_NOTIFY_STREAM_STATUS_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup dyn_gpu_aperture_reg_prog

   Dynamically configure GPU SMMU self-program aperture.

   @smc_id
     0x02000C1B

   @sys_call_params{dyn_gpu_aperture_reg_prog_s}
     @table{weak__dyn__gpuaperture__assign__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_DYN_GPU_APERTURE_REG_PROG                              \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x1B)

#define TZ_DYN_GPU_APERTURE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup tz_enable_shm_bridge

   Enabled the SHM mechanism

   @smc_id
     0x02000C1C

   @sys_call_params{tz_enable_shm_brdige_s} ?
     @table{weak__tz_enable_shm_brdige__s} ?

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SHM_BRIDGE_ENABLE                   \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x1C)

#define TZ_SHM_BRIDGE_ENABLE_PARAM_ID          \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup tz_delete_shm_bridge

   Enabled the SHM mechanism

   @smc_id
     0x02000C1D

   @sys_call_params{tz_delete_shm_brdige_s} ?
     @table{weak__tz_delete_shm_brdige__s} ?

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SHM_BRIDGE_DELETE                   \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x1D)

#define TZ_SHM_BRIDGE_DELETE_PARAM_ID          \
    TZ_SYSCALL_CREATE_PARAM_ID_1( \
        TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup tz_create_shm_bridge

   Creates a bridge between TZ & other VMs to share memory
   with TZ

   @smc_id
     0x02000C1E

   @sys_call_params{tz_create_shm_brdige_s} ?
     @table{weak__tz_create_shm_brdige__s} ?

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SHM_BRIDGE_CREATE                   \
   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x1E)

#define TZ_SHM_BRIDGE_CREATE_PARAM_ID          \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL)    

/**
   @ingroup mem_handle_tlb_fault

   Handle TLB faults that are routed to ns EL1 

   @smc_id
     0x02000C1F

   @param_id
     0x00000000

   @sys_call_params{uint64}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_MEM_HANDLE_TLB_FAULT                                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x1F)

#define HYP_MEM_HANDLE_TLB_FAULT_PARAM_ID                            \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )      

/**
   @ingroup ocmem_lock_region

   System call to lock an OCMEM region.

   @smc_id
     0x02000F01

   @param_id
     0x00000004

   @sys_call_params{tz_ocmem_lock_region_req_s}
     @table{weak__tz__ocmem__lock__region__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_LOCK_REGION       TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_OCMEM, 0x1)

#define TZ_OCMEM_LOCK_REGION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup ocmem_unlock_region

   System call to unlock an OCMEM region.

   @smc_id
     0x02000F02

   @param_id
     0x00000003

   @sys_call_params{tz_ocmem_unlock_region_req_s}
     @table{weak__tz__ocmem__unlock__region__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_UNLOCK_REGION     TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_OCMEM, 0x2)

#define TZ_OCMEM_UNLOCK_REGION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup ocmem_enable_mem_dump

   System call to enable memory dumping for an OCMEM region.

   @smc_id
     0x02000F03

   @param_id
     0x00000003

   @sys_call_params{tz_ocmem_enable_mem_dump_req_s}
     @table{weak__tz__ocmem__enable__mem__dump__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_ENABLE_MEM_DUMP   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_OCMEM, 0x3)

#define TZ_OCMEM_ENABLE_MEM_DUMP_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup ocmem_disable_mem_dump

   System call to disable memory dumping for an OCMEM region.

   @smc_id
     0x02000F04

   @param_id
     0x00000003

   @sys_call_params{tz_ocmem_disable_mem_dump_req_s}
     @table{weak__tz__ocmem__disable__mem__dump__req__s}

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_DISABLE_MEM_DUMP  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_OCMEM, 0x4)

#define TZ_OCMEM_DISABLE_MEM_DUMP_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup es_save_partition_hash

   Saves the eMMC partition kernel/system hash (SHA-256).
   This service is used by the LK to save the kernel partition hash
   and by the HLOS to save the system partition hash.

   @smc_id
     0x02001001

   @param_id
     0x000000082

   @sys_call_req_params{tz_es_save_partition_hash_req_s}
     @table{weak__tz__es__save__partition__hash__req__s}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_ES_SAVE_PARTITION_HASH_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_ES, 0x1)

#define TZ_ES_SAVE_PARTITION_HASH_ID_PARAM_ID TZ_SYSCALL_CREATE_PARAM_ID_3( \
    TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tz_es_set_ice_key

   Sets a specified key to the ICE hardware.
   This function is used by the PFE solution.

   @smc_id
     0x02001001

   @param_id
     0x000000082

   @sys_call_req_params{tz_es_set_ice_key_req_s}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_ES_SET_ICE_KEY_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_ES, 0x2)

#define TZ_ES_SET_ICE_KEY_PARAM_ID TZ_SYSCALL_CREATE_PARAM_ID_5( \
        TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
        TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup set_config_ice_key

   Sets a specified key to the ICE hardware, and configure the key
   with the given parametes.

   @smc_id 0x02001004

   @sys_call_req_params{tz_set_config_ice_key_req_t}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_ES_SET_CONFIG_ICE_KEY_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_ES, 0x4)

#define TZ_ES_SET_CONFIG_ICE_KEY_PARAM_ID TZ_SYSCALL_CREATE_PARAM_ID_5( \
        TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
        TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup tz_es_invalidate_ice_key

   Invalidates a specified key of the ICE hardware.
   This function is used by the PFE solution.

   @smc_id
     0x00001001

   @param_id
     0x000000082

   @sys_call_req_params{tz_es_invalidate_ice_key_req_s}
     @table{weak__tz__es__invalidate__ice__key__req__s}

   @return
     E_SUCCESS on success; negative value on failure.
*/

#define TZ_ES_INVALIDATE_ICE_KEY_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_ES, 0x3)

#define TZ_ES_INVALIDATE_ICE_KEY_PARAM_ID TZ_SYSCALL_CREATE_PARAM_ID_1( \
        TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup hdcp_write_registers

   This service is used to write registers in the
   HDCP HDMI secure resource group. Additionally, whenever this syscall
   is invoked, the TZ software attempts to update the MMSS_HDMI_HDCP_CTRL2 register
   to reflect whether HDCP HDMI ecryption is to be enforced.

   Five registers can be written to with one invocation of this syscall.
   Note that the order in which these registers are written is not guaranteed,
   so the caller should take care to invoke this syscall multiple times if
   a certain order is required (as may be the case with the HWIO_MMSS_HDMI_HDCP_SHA_CTRL
   and HWIO_MMSS_HDMI_HDCP_SHA_DATA registers).

   @smc_id
     0x02001101

   @param_id
     0x0000000A

   @sys_call_req_params{tz_hdcp_write_registers_req_s}
     @table{weak__tz__hdcp__write__registers__req__s}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_HDCP_WRITE_REGISTERS TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_HDCP, 0x1)

#define TZ_HDCP_WRITE_REGISTERS_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_10( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup read_dbg_policy_content

   If loaded by boot, the debug policy is maintained in secure memory. This service is
   used to expose a copy of the debug policy to the caller.

   @smc_id
     0x00001101

   @param_id
     0x00000022

   @sys_call_rsp_params{tz_read_debug_policy_content_rsp_t}
     @table{weak__tz__read__debug__policy__content__rsp__s}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_READ_DEBUG_POLICY_CONTENT TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_READ_DBG_POLICY, 0x1)

#define TZ_READ_DEBUG_POLICY_CONTENT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup tz_get_key_to_hyp

   Generate/Get a KDF key from TZ to Hyp.
   Generate unique Key per device based on h/w key.
   Every call to this syscall will return same key for
   same device and key_id.

   NOTE: Hyp should not allow this call coming from Non-Sec El1. TZ may
         checks if Key buffer belongs to Hyp memory or not where ever possible.


   @smc_id
     0x02001C01

   @param_id
     0x00000083

   @sys_call_params{tz_get_key_to_hyp_req_s}
     @table{weak__tz__get__key__to__hyp__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_GET_KEY_TO_HYP_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_HYP, 0x01)

#define TZ_GET_KEY_TO_HYP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup tz_hyp_to_ta_svc_fwd

   Forwards a Hyp/EL2 service request to a TA and
   routes the TA's response back to Hyp/EL2.

   NOTE: Hyp should not allow this call coming from Non-Sec El1.

   @smc_id
     0x02001C02

   @param_id
     0x00000224

   @sys_call_params{tz_hyp_to_ta_svc_fwd_req_s}
     @table{weak__tz__hyp__to__ta__svc__fwd__req__s}

   @return
     rsp0 - Zero on success, negative value on failure.
     rsp1 - Return output data length for Success case.
 */
#define TZ_HYP_TO_TA_SVC_FWD_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_HYP, 0x02)

#define TZ_HYP_TO_TA_SVC_FWD_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL,\
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)



/**
   @ingroup gic_error_recovery


   @smc_id
     0x02001D01

   @param_id
     0x00000022

   @return
     Zero on success, negative value on failure.
 */
#define TZ_GIC_ERROR_RECOVERY_SMC_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_GIC, 0x01)

#define TZ_GIC_ERROR_RECOVERY_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)


/* TODO Syscall description */
#define TZ_TEE_SCHEDULE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TEE, TZ_PVC_EXECUTIVE_TEE, 0x00)

#define TZ_TEE_SCHEDULE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/* TODO Syscall description */
#define TZ_EXECUTIVE_EXT_SCHEDULE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TEE_HLOS, TZ_SVC_EXECUTIVE_EXT, 0x01)

#define TZ_EXECUTIVE_EXT_SCHEDULE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/*
   DEPRECATED SYSCALL: DO NOT REUSE
*/
#define TZ_EXECUTIVE_ID                                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_EXECUTIVE, 0x01)

/**
   @ingroup tzos_app_start

   Load and initialize a secure application.

   @smc_id
     0x32000101

   @param_id
     0x00000003

   @sys_call_params{tz_os_app_start_req_s}
     @table{weak__tz__os__app__start__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_APP_START_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x01)

#define TZ_OS_APP_START_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_app_shutdown

   Shut down a secure application.

   @smc_id
     0x32000102

   @param_id
     0x00000001

   @sys_call_params{tz_os_app_shutdown_req_s}
     @table{weak__tz__os__app__shutdown__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_APP_SHUTDOWN_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x02)

#define TZ_OS_APP_SHUTDOWN_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_app_lookup

   Query a secure application already loaded in memory.

   @smc_id
     0x32000103

   @param_id
     0x00000022

   @sys_call_params{tz_os_app_lookup_req_s}
     @table{weak__tz__os__app__lookup__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_APP_LOOKUP_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x03)

#define TZ_OS_APP_LOOKUP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_app_get_state

   Get the thread state of a secure application.

   @smc_id
     0x32000104

   @param_id
     0x00000001

   @sys_call_params{tz_os_app_get_state_req_s}
     @table{weak__tz__os__app__get__state__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_APP_GET_STATE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x04)

#define TZ_OS_APP_GET_STATE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_app_region_notification

   Initialize the entire secure application memory region.

   @smc_id
     0x32000105

   @param_id
     0x00000003

   @sys_call_params{tz_os_app_region_notification_req_s}
     @table{weak__tz__os__app__region__notification__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_APP_REGION_NOTIFICATION_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x05)

#define TZ_OS_APP_REGION_NOTIFICATION_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_register_log_buffer

   Register the secure application log buffer.

   @smc_id
     0x32000106

   @param_id
     0x00000022

   @sys_call_params{tz_os_register_log_buffer_req_s}
     @table{weak__tz__os__register__log__buffer__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_REGISTER_LOG_BUFFER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x06)

#define TZ_OS_REGISTER_LOG_BUFFER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_load_services_image

   Load a secure services image in secure user space.

   @smc_id
     0x32000107

   @param_id
     0x00000003

   @sys_call_params{tz_os_load_services_image_req_s}
     @table{weak__tz__os__load__services__image__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_LOAD_SERVICES_IMAGE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x07)

#define TZ_OS_LOAD_SERVICES_IMAGE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_unload_services_image

   Unload a secure services image.

   @smc_id
     0x32000108

   @param_id
     0x00000000

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_UNLOAD_SERVICES_IMAGE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x08)

#define TZ_OS_UNLOAD_SERVICES_IMAGE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

#if 0 /* NOSUPPORT_95XX */
/**
   @ingroup tzos_app_start_embedded

   Load and initialize an embedded secure application.

   @smc_id
     0x32000109

   @param_id
     0x00000022

   @sys_call_params{tz_os_app_start_embedded_req_s}
     @table{weak__tz__os__app__start_embedded__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_APP_START_EMBEDDED_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x09)

#define TZ_OS_APP_START_EMBEDDED_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )
#endif

/**
   @ingroup tzos_query_embedded_images_support

   Check if TZ supports embedded images and return true if
   libraries are loaded

   @smc_id
     0x3200010A

   @param_id
     0x00000000

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_QUERY_EMBEDDED_IMAGES_SUPPORT_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x0A)

#define TZ_OS_QUERY_EMBEDDED_IMAGES_SUPPORT_PARAM_ID \
    TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup tzbsp_is_log_encryption_enabled

   Querry whether log encryption feature is supported and enabled.

   @smc_id
     0x3200010B

   @param_id
     0x00000000

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_LOG_ENCR_ALLOWED_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x0B)

#define TZ_OS_LOG_ENCR_ALLOWED_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup tzos_request_encr_log_buffer

   Request to copy encrypted TZ/TA logs in hlos buffer .

   @smc_id
     0x3200010C

   @param_id
     0x00000023

   @sys_call_params{tz_os_request_encr_log_buffer_req_s}
     @table{weak__tz__os__request__encr__log__buffer__req__s}

   @return
     Zero on success, negative value on failure or if feature is not supported.
 */
#define TZ_OS_REQUEST_ENCR_LOG_BUFFER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x0C)

#define TZ_OS_REQUEST_ENCR_LOG_BUFFER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_app_region_clear_notification

   Uninitialize and unlock the secure application memory region.

   @smc_id
     0x32000109

   @param_id
     0x00000000

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_APP_REGION_CLEAR_NOTIFICATION_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x09)

#define TZ_OS_APP_REGION_CLEAR_NOTIFICATION_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0


/**
   @ingroup tzos_register_listener

   Register a listener service.

   @smc_id
     0x32000201

   @param_id
     0x00000083

   @sys_call_params{tz_os_register_listener_req_s}
     @table{weak__tz__os__register__listener__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_REGISTER_LISTENER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x01)

#define TZ_OS_REGISTER_LISTENER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_deregister_listener_service

   Deregister a listener service.

   @smc_id
     0x32000202

   @param_id
     0x00000001

   @sys_call_params{tz_os_deregister_listener_req_s}
     @table{weak__tz__os__deregister__listener__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_DEREGISTER_LISTENER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x02)

#define TZ_OS_DEREGISTER_LISTENER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_listener_response_handler

   Listener response handler for registered listener services. When the
   TZ software receives a listener service response, this handler
   function relays the response to the appropriate secure application.

   @smc_id
     0x32000203

   @param_id
     0x00000002

   @sys_call_params{tz_os_listener_response_handler_req_s}
     @table{weak__tz__os__listener__response__handler__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_LISTENER_RESPONSE_HANDLER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x03)

#define TZ_OS_LISTENER_RESPONSE_HANDLER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_cont_blocked_req

   Continues a blocked request.

   @smc_id
     0x32000204

   @param_id
     0x00000001

   @sys_call_params{tz__os__continue__blocked__request__req_s}
     @table{weak__tz__os__continue__blocked__request__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_CONTINUE_BLOCKED_REQUEST_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x04)

#define TZ_OS_CONTINUE_BLOCKED_REQUEST_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_listener_response_wlist_handler

   Listener response handler for registered listener services. When the
   TZ software receives a listener service response, this handler
   function relays the response to the appropriate secure application.

   @smc_id
     0x32000205

   @param_id
     0x00000204

   @sys_call_params{tz_os_listener_response_wlist_handler_req_s}
     @table{weak__tz__os__listener__response__wlist__handler__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_LISTENER_RESPONSE_WLIST_HANDLER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x05)

#define TZ_OS_LISTENER_RESPONSE_WLIST_HANDLER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
    TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL,       \
    TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup tzos_register_listener_smcinvoke

   Register a listener service compatible with SMCInvoke.

   @smc_id
     0x32000206

   @param_id
     0x00000083

   @sys_call_params{tz_os_register_listener_req_s}
     @table{weak__tz__os__register__listener__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_REGISTER_LISTENER_SMCINVOKE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x06)

#define TZ_OS_REGISTER_LISTENER_SMCINVOKE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_continue_blocked_req_smcinvoke

   Continues a blocked request on a listener service compatible with SMCInvoke

   @smc_id
     0x32000207

   @param_id
     0x00000001

   @sys_call_params{tz_os_continue_blocked_request_smci_req_s}
     @table{weak__tz__os__continue__blocked__request__smci__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_CONTINUE_BLOCKED_REQUEST_SMCINVOKE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x07)

#define TZ_OS_CONTINUE_BLOCKED_REQUEST_SMCINVOKE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_load_external_image

   Loads an extenal image into memory.

   @smc_id
     0x32000301

   @param_id
     0x00000003

   @sys_call_params{tz_os_load_external_image_req_s}
     @table{weak__tz__os__load__external__image__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_LOAD_EXTERNAL_IMAGE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_EXTERNAL, 0x01)

#define TZ_OS_LOAD_EXTERNAL_IMAGE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_unload_external_image

   Unloads an external image.

   @smc_id
     0x32000302

   @param_id
     0x00000000

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_UNLOAD_EXTERNAL_IMAGE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_EXTERNAL, 0x02)

#define TZ_OS_UNLOAD_EXTERNAL_IMAGE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup tzos_rpmb_provision_key

   Provisions an RPMB key.

   @note1hang This is a one-time irreversible operation.

   @smc_id
     0x32000401

   @param_id
     0x00000001

   @sys_call_params{tz_os_rpmb_provision_key_req_s}
     @table{weak__tz__os__rpmb__provision__key__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_RPMB_PROVISION_KEY_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_RPMB, 0x01)

#define TZ_OS_RPMB_PROVISION_KEY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_rpmb_erase

   Erases the RPMB partition.

   @smc_id
     0x32000402

   @param_id
     0x00000000

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_RPMB_ERASE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_RPMB, 0x02)

#define TZ_OS_RPMB_ERASE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup tzos_rpmb_check_prov_status

   Checks the RPMB provisioning status.

   @smc_id
     0x32000403

   @param_id
     0x00000000

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_RPMB_CHECK_PROV_STATUS_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_RPMB, 0x03)

#define TZ_OS_RPMB_CHECK_PROV_STATUS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup tzos_ks_gen_key

   Generate a key for the keystore service.

   @smc_id
     0x32000501

   @param_id
     0x00000022

   @sys_call_params{tz_os_ks_req_s}
     @table{weak__tz__os__ks__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_KS_GEN_KEY_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_KEYSTORE, 0x01)

#define TZ_OS_KS_GEN_KEY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_ks_del_key

   Delete a key for the keystore service.

   @smc_id
     0x32000502

   @param_id
     0x00000022

   @sys_call_params{tz_os_ks_req_s}
     @table{weak__tz__os__ks__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_KS_DEL_KEY_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_KEYSTORE, 0x02)

#define TZ_OS_KS_DEL_KEY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_ks_get_max_keys

   Get the maximum number of keys supported by the keystore service.

   @smc_id
     0x32000503

   @param_id
     0x00000001

   @sys_call_params{tz_os_ks_get_max_keys_req_s}
     @table{weak__tz__os__ks__get__max__keys__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_KS_GET_MAX_KEYS_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_KEYSTORE, 0x03)

#define TZ_OS_KS_GET_MAX_KEYS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_ks_set_pipe_key

   Set a pipe key for the keystore service.

   @smc_id
     0x32000504

   @param_id
     0x00000022

   @sys_call_params{tz_os_ks_req_s}
     @table{weak__tz__os__ks__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_KS_SET_PIPE_KEY_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_KEYSTORE, 0x04)

#define TZ_OS_KS_SET_PIPE_KEY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzos_ks_update_key

   Update a key for the keystore service.

   @smc_id
     0x32000505

   @param_id
     0x00000022

   @sys_call_params{tz_os_ks_req_s}
     @table{weak__tz__os__ks__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_KS_UPDATE_KEY_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_KEYSTORE, 0x05)

#define TZ_OS_KS_UPDATE_KEY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
  @ingroup tz_os_ice_key_config_restore_req_s

  Restore ice key configuration in case of storage core reset

  @smc_id
    0x32001702

  @param_id
    0x00000002

  @sys_call_req_params{tz_os_ice_key_config_restore_req_s}
     @table{weak__tz__os__ice__key__config__restore__req__s__s}

  @return
    None
*/
#define TZ_OS_KS_RESTORE_KEY_CONFIG_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_KEYSTORE, 0x06)

#define TZ_OS_KS_RESTORE_KEY_CONFIG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )
/**
   @ingroup smc_invoke

   Communicate to objects over SMC

   @smc_id
     0x32000600

   @param_id
     0x224

   @sys_call_params{tz_os_smc_invoke_req_s}
     @table{weak__tz__os__smc__invoke__req__s}

   @sys_call_params{tz_os_smc_invoke_rsp_s}
     @table{weak__tz__os__smc__invoke__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_SMC_INVOKE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_SMC_INVOKE, 0x0)

#define TZ_SMC_INVOKE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
    TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
    TZ_SYSCALL_PARAM_TYPE_VAL, \
    TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup smc_invoke_callback_response

   Respond to a callback request over SMC

   @smc_id
     0x32000601

   @param_id
     0x22

   @sys_call_params{tz_os_smc_invoke_cback_req_s}
     @table{weak__tz__os__smc__invoke__cback__req__s}

   @sys_call_params{tz_os_smc_invoke_rsp_s}
     @table{weak__tz__os__smc__invoke__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_SMC_INVOKE_CBACK_RSP_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_SMC_INVOKE, 0x1)

#define TZ_SMC_INVOKE_CBACK_RSP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
    TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup smc_invoke

   Communicate to objects over SMC using cache coherent buffers

   @smc_id
     0x32000602

   @param_id
     0x224

   @sys_call_params{tz_os_smc_invoke_req_s}
     @table{weak__tz__os__smc__invoke__req__s}

   @sys_call_params{tz_os_smc_invoke_rsp_s}
     @table{weak__tz__os__smc__invoke__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_SMC_INVOKE_CACHE_COHERENT_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_SMC_INVOKE, 0x2)

/**
   @ingroup tzapp_qsapp_send_data

   Sends data to a QSEE secure application.

   @smc_id
     0x30000001

   @param_id
     0x00000885

   @sys_call_params{tz_app_send_data_req_s}
     @table{weak__tz__app__send__data__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_APP_QSAPP_SEND_DATA_ID \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x01)

#define TZ_APP_QSAPP_SEND_DATA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup  tzapp_gpapp_open_session

   Opens a session in a secure Global Platform (GP) application.

   @smc_id
     0x30000002

   @param_id
     0x00000885

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
*/
#define TZ_APP_GPAPP_OPEN_SESSION_ID \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x02)

#define TZ_APP_GPAPP_OPEN_SESSION_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup  tzapp_gpapp_close_session

   Closes a session in a secure GP applications.

   @smc_id
     0x30000003

   @param_id
     0x00000885

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
*/
#define TZ_APP_GPAPP_CLOSE_SESSION_ID \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x03)

#define TZ_APP_GPAPP_CLOSE_SESSION_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/* TODO Syscall description */
#define TZ_DYNAMIC_SID_SYSCALL_ID               \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0xDF)

#define TZ_DYNAMIC_SID_SYSCALL_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup  tzapp_gpapp_invoke_command

   Invokes a command in a secure GP application.

   @smc_id
     0x30000004

   @param_id
     0x00000885

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
*/
#define TZ_APP_GPAPP_INVOKE_COMMAND_ID \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x04)

#define TZ_APP_GPAPP_INVOKE_COMMAND_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tzapp_qsapp_send_data_wlist

   Sends data to a QSEE secure application.

   @smc_id
     0x30000006

   @param_id
     0x00008885

   @sys_call_params{tz_app_send_data_wlist_req_s}
     @table{weak__tz__app__send__data__wlist__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_APP_QSAPP_SEND_DATA_WLIST_ID                                      \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x06)

#define TZ_APP_QSAPP_SEND_DATA_WLIST_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_7( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup  tzapp_gpapp_open_session_wlist

   Opens a session in a secure Global Platform (GP) application.  Also
   passes a white list information pointer to validate memory
   addresses shared with the TA are approved by the HLOS.

   @smc_id
     0x30000007

   @param_id
     0x00008885

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
*/
#define TZ_APP_GPAPP_OPEN_SESSION_WLIST_ID \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x07)

#define TZ_APP_GPAPP_OPEN_SESSION_WLIST_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_7( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup  tzapp_gpapp_close_session_wlist

   Closes a session in a secure GP applications.  Also passes a white
   list information pointer to validate memory addresses shared with
   the TA are approved by the HLOS.

   @smc_id
     0x30000008

   @param_id
     0x00008885

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
*/
#define TZ_APP_GPAPP_CLOSE_SESSION_WLIST_ID \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x08)

#define TZ_APP_GPAPP_CLOSE_SESSION_WLIST_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_7( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup  tzapp_gpapp_invoke_command_wlist

   Invokes a command in a secure GP application.  Also passes a white
   list information pointer to validate memory addresses shared with
   the TA are approved by the HLOS.

   @smc_id
     0x30000009

   @param_id
     0x00008885

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
*/
#define TZ_APP_GPAPP_INVOKE_COMMAND_WLIST_ID \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x09)

#define TZ_APP_GPAPP_INVOKE_COMMAND_WLIST_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_7( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )


/**
@ingroup  tz_tree_skext_info_id

This SMC ID is to pass any init parameters to SKExtension on
windows platforms.

@smc_id
0x3000000A

@param_id
0x00000022

@sys_call_params{tz_os_rsp_s}
@table{weak__tz__os__rsp__s}

@return
Zero on success, negative value on failure.
*/
#define TZ_TREE_SKEXT_INFO_ID \
TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x0A)

#define TZ_TREE_SKEXT_INFO_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2(TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL)

/* This definition is provided as an example for OEMs and does not
 * need to be included in the syscall documentation
 */
#define TZ_OEM_TEST_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_OEM, TZ_SVC_OEM, 0x01)


/**
   @ingroup tzbsp_oem_error_handler

   System call for handling OEM errors.

   @smc_id
     0x02001B01

   @param_id
     0x00000001

   @sys_call_req_params{uint32_t}
     @table{}

   @sys_call_rsp_params{}
     @table{}

   @return
     E_SUCCESS if the call succeeded; no return otherwise.
*/

#define TZ_OEM_ERROR_NOTIFICATION_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PCIE , 0x01)

#define TZ_OEM_ERROR_NOTIFICATION_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup hyp_notify_rtic_dtb_location

   Apps boot loader loads multiple device tree bindings (DTBs). One of those
   DTBs is for RTIC (Run-Time Integrity Check) use-case. It contains location
   of the measurement parameters used by RTIC service in EL2 as well as in
   RTIC TA.

   The purpose of this function is to notify the RTIC DTB location so that
   the DTB contents can be used to locate measurement paramameter.

   @smc_id
     0x02001901

   @sys_call_params{hyp_notify_rtic_dtb_s}
     @table{weak__hyp__notify__rtc__dtb__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_NOTIFY_RTIC_DTB_LOCATION    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RTIC, 0x1)

#define HYP_NOTIFY_RTIC_DTB_LOCATION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RO, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup hyp_share_el2_report_mem_hlos

   Provides the location of the shared memory for EL2 to write report
   for HLOS.

   The purpose of this function is for HLOS to notify the location of the
   memory that can be used by EL2 to write the report to.

   @smc_id
     0x02001902

   @sys_call_params{hyp_share_el2_report_mem_hlos_s}
     @table{weak__hyp__share__el2__report__mem__hlos__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_SHARE_EL2_REPORT_MEM_HLOS    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RTIC, 0x2)

#define HYP_SHARE_EL2_REPORT_MEM_HLOS_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tz_share_el2_report_mem_rtic

   Provides the location of the shared memory for EL2 to write report
   for RTIC service in QSEE.

   The purpose of this function is for hyp to notify the location of the
   memory that can be used by RTIC service in QSEE to read the EL2
   report from.

   @smc_id
     0x02001903

   @sys_call_params{tz_share_el2_report_mem_rtic_s}
     @table{weak__tz__share__el2__report__mem__rtic__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SHARE_EL2_REPORT_MEM_RTIC    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RTIC, 0x3)

#define TZ_SHARE_EL2_REPORT_MEM_RTIC_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup hyp_enable_hlos_mem_protection

   Restricts the memory permissions for HLOS in the Stage 2 memory
   mappings.

   The purpose of this function is to allow HLOS to decide when to protect
   its memory regions. This is originally due to ARM errata, which requires
   HLOS to self-modify its code region before Stage 2 memory protection can
   be enabled.

   @smc_id
     0x02001904

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_ENABLE_HLOS_MEM_PROTECTION    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RTIC, 0x4)

#define HYP_ENABLE_HLOS_MEM_PROTECTION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup hyp_invalidate_el2_report_mem_hlos

   Provides the information that the location of the shared memory for EL2 to write report
   for HLOS is invalid now.

   The purpose of this function is for HLOS to notify invalidation of the
   location of the memory that is used by EL2 to write the report to.

   @smc_id
     0x02001905

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_INVALIDATE_EL2_REPORT_MEM_HLOS    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RTIC, 0x5)

#define HYP_INVALIDATE_EL2_REPORT_MEM_HLOS_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0


/**
   @ingroup hyp_image_manager

   Calls into hypervisor image manager for mapping memory, authentication,
   and launching of images by hyp.

   @smc_id
     0xC3000001

   @sys_call_params{hyp_image_manager_s}
     @table{weak__hyp__image__manager__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define HYP_IMAGE_MANAGER_SMC_ID    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_OEM, 0x0, 0x01)

/**
   @ingroup send_storage_cmd

  Sends the commands to storage devices.

  

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SEND_STORAGE_CMD_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_STORAGE, 0x01)

#define TZ_SEND_STORAGE_CMD_ID_PARAM_ID \
   TZ_SYSCALL_CREATE_PARAM_ID_4(TZ_SYSCALL_PARAM_TYPE_BUF_RO,TZ_SYSCALL_PARAM_TYPE_VAL,TZ_SYSCALL_PARAM_TYPE_BUF_RW,TZ_SYSCALL_PARAM_TYPE_VAL)


/**
   @ingroup tzbsp_qwes_init_attestation

   Sends Init Device attestation IPC request to TME-L.

   @smc_id
     0x02001E01

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_QWES_INIT_ATTESTATION_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_QWES, 0x1)

#define TZ_QWES_INIT_ATTESTATION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3(TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW)

/**
   @ingroup tzbsp_qwes_get_attestation_report

   Sends Get Device Attestation report IPC request to TME-L.

   @smc_id
     0x02001E02

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_QWES_GET_DEVICE_ATTESTATION_REPORT_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_QWES, 0x2)

#define  TZ_QWES_GET_DEVICE_ATTESTATION_REPORT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_7(TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL,\
                               TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL,\
                               TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW)

/**
   @ingroup tzbsp_qwes_device_provision

   Sends Device Provisioning IPC request to TME-L.

   @smc_id
     0x02001E03

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_QWES_DEVICE_PROVISION_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_QWES, 0x3)

#define  TZ_QWES_DEVICE_PROVISION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5(TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL,\
                               TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW)

/**
   @ingroup tzbsp_qwes_bindings_check

   Sends Bindings Check IPC request to TME-L.

   @smc_id
     0x02001E03

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_QWES_BINDINGS_CHECK_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_QWES, 0x4)

#define  TZ_QWES_BINDINGS_CHECK_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5(TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL,\
                               TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW)

/** @addtogroup tz_datatypes
@{ */

/**
   Legacy common system call request header used in all system calls.
   No longer supported.
 */
typedef struct tz_syscall_req_s
{
   uint32  len;                 /**< Total length of available memory
                                     for requests and responses. */

   uint32  command_start_off;   /**< Offset of command-specific data
                                     from the beginning of the buffer. */

   uint32  response_write_off;  /**< Start offset of the memory to
                                     where the response is to be
                                     written.  The offset is computed
                                     from the start of this
                                     structure. */

   uint32  command_id;          /**< Unique identifier that
                                     establishes which command follows
                                     this structure. */
} __attribute__ ((packed)) tz_syscall_req_t;

/**
   Common system call response header used in all system calls that
   have responses.
 */
typedef struct tz_syscall_rsp_s
{
   int   status;              /**< Status of the syscall_req
                                   executed: \n
                                   0 means the command was not completed \n
                                   1 means the command was completed
                                   @newpagetable */
} __attribute__ ((packed)) tz_syscall_rsp_t;

/**
   VA range structure in the Set VA Range command.
 */
typedef struct tz_va_range_s
{
  uint64  va_start; /**< VA start address. */
  uint32  va_size;  /**< VA range size. */
}tz_va_range_t;

/** @} */ /* end_addtogroup tz_datatypes */

/*----------------------------------------------------------------------------
 * Parameter Structures
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * BOOT Service Structures
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_macros_constants
@{ */

/**
   @name HLOS Start Address Slots
   HLOS start address slots provided by the core service
   TZ_CORE_SVC_START_ADDR_REQ_ID.
   @{
*/
/** CPU address is AArch64/AArch32. */
#define TZ_BOOT_ADDR_AARCH64               0x00000001
#define TZ_BOOT_ADDR_AARCH32               0x00000000
/** CPU initial boot. */
#define TZ_BOOT_ADDR_COLD                  0x00000002
/** CPU subsequent boots/resets. */
#define TZ_BOOT_ADDR_WARM                  0x00000004

/** @} */ /* end_name_group HLOS Start Address Slots */

/** @} */ /* end_addtogroup tz_macros_constants */



/**
   @weakgroup weak_tz_set_boot_addr_s
@{
*/
typedef struct tz_set_boot_addr_s
{
  uint32 boot_addr;                /**< The address to which the QSEE
                                        is to jump after CPU
                                        initialization.  It must be a
                                        physical address, but can be
                                        NULL. Addresses in the secure
                                        memory are not accepted and
                                        cause the service to return
                                        E_BAD_ADDRESS. */
  uint32 affl0_bmsk;               /**< A bitmask of the CPUs
                                        identified by the AFFL0 field
                                        of the MPIDR. A value of zero
                                        is not allowed.*/
  uint32 affl1_bmsk;               /**< A bitmask of the CPUs
                                        identified by the AFFL1 field
                                        of the MPIDR. A value of zero
                                        is not allowed. */
  uint32 affl2_bmsk;               /**< A bitmask of the CPUs
                                        identified by the AFFL2 field
                                        of the MPIDR. A value of zero
                                        is not allowed. */
  uint32 affl3_bmsk;               /**< A bitmask of the CPUs
                                        identified by the AFFL3 field
                                        of the MPIDR. A value of zero
                                        is not allowed. */
  uint32 flags;                    /**< Combination of TZ_BOOT_ADDR_*
                                        values. The same address can
                                        be set to multiple HLOS start
                                        address slots by bitwise ORing
                                        the values. Unused bits are
                                        ignored and must be kept at
                                        zero. A value of zero is not
                                        allowed. */
} __attribute__ ((packed)) tz_set_boot_addr_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_set_milestone_s
@{
*/
typedef struct tz_set_milestone_s
{
  uint64 e_entry;                  /**< Address to which the QSEE
                                        is to jump after the milestone call.
                                        It must be a physical address.
                                        Addresses in the secure memory are
                                        not accepted and cause the service
                                        to return E_BAD_ADDRESS. */
  uint32 mode;                     /**< Used on targets that support
                                        flashless boot to provide boot
                                        mode information.  */
  uint32 ei_class;                 /**< Information from the ELF header of the image
                                        to jump to after the milestone
                                        call.  */

} __attribute__ ((packed)) tz_set_milestone_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */
/**

    HLOS boot parameter types.
 */
typedef struct hlos_boot_params_s
{
  uintptr_t el1_x0; /**< Value to set register X0. */
  uintptr_t el1_x1; /**< Value to set register X1. */
  uintptr_t el1_x2; /**< Value to set register X2. */
  uintptr_t el1_x3; /**< Value to set register X3. */
  uintptr_t el1_x4; /**< Value to set register X4. */
  uintptr_t el1_x5; /**< Value to set register X5. */
  uintptr_t el1_x6; /**< Value to set register X6. */
  uintptr_t el1_x7; /**< Value to set register X7. */
  uintptr_t el1_x8; /**< Value to set register X8. */
  uintptr_t el1_elr; /**< Value to set Exception Link Register (ELR). */
} hlos_boot_params_t;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_do_modeswitch_s
@{
*/

typedef struct tz_do_modeswitch_s
{
  hlos_boot_params_t hlos_boot_info;  /**< HLOS boot information. */

} __attribute__ ((packed)) tz_do_modeswitch_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_secure_wdog_disable_req_s
@{
*/
typedef struct tz_secure_wdog_disable_req_s
{
  boolean disable;                 /**< If TRUE, the secure WDOG is
                                        disabled; enabled otherwise. */
} __attribute__ ((packed)) tz_secure_wdog_disable_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_secure_wdog_trigger_req_s
@{
*/
typedef struct tz_secure_wdog_trigger_req_s
{
  boolean disable_debug;           /**< If TRUE, the watchdog debug image
                                        is disabled (that is, it will not run
                                        after a watchdog reset). */
} __attribute__ ((packed)) tz_secure_wdog_trigger_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_lock_xpu_s
@{
*/
typedef struct tz_lock_xpu_s
{
  uint64 elf_header_addr;          /**< Address to which the QSEE will
                                   extract the ELF header associated with
                                   the processor ID given as a second argument. */
  uint32 program_header;           /**< Program header. */

  uint32 proc_id;                  /**< Processor ID associated with the ELF header. */

  uint32 dummy;                    /**< Dummy. */
} __attribute__ ((packed)) tz_lock_xpu_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_hw_for_ram_dump_req_s
@{
*/
typedef struct tz_config_hw_for_ram_dump_req_s
{
  uint32 disable_wd_dbg;           /**< If TRUE, the watchdog debug image
                                        is disabled (that is, it will not run
                                        after a watchdog reset). */

  uint32 boot_partition_sel;       /**< Value programmed into the
                                        BOOT_PARTION_SELECT register. */
} __attribute__ ((packed)) tz_config_hw_for_ram_dump_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/** Video core state. */
typedef enum
{
  TZ_SUBSYS_STATE_SUSPEND = 0,      /**< Suspend the subsystem. */
  TZ_SUBSYS_STATE_RESUME,           /**< Resume the subsystem. */

  TZ_SUBSYS_STATE_VENUS_RESTORE_THRESHOLD
  /**< Adding a new state as a workaround for a Venus hardware bug.
       At present, this is expected to be a temporary change, so the state
       was named as something Venus-specific rather than something generic. */

} tz_subsys_state_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_video_set_state_req_s
@{
*/
typedef struct tz_video_set_state_req_s
{
  uint32 state;                    /**< Requested video state; one
                                        of the tz_video_state_e_type
                                        parameters. */

  uint32 spare;                    /**< Reserved for future use.
                                        Set to zero. */
} __attribute__ ((packed)) tz_video_set_state_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_spin_with_irqs_fiqs_disabled_s
@{
*/
typedef struct tz_spin_with_irqs_fiqs_disabled_s
{
  uint32 spare;                       /**< Reserved for future use. */
} __attribute__ ((packed)) tz_spin_with_irqs_fiqs_disabled_t;

/** @} */ /* end_weakgroup */


/** @addtogroup tz_datatypes
@{ */

/*----------------------------------------------------------------------------
 * PIL Service Structures
 * -------------------------------------------------------------------------*/
/**
    Peripheral types.
 */
typedef enum
{
  TZ_PIL_AUTH_MODEM_PROC,       /**< This enum member is deprecated.
                                     Use #TZ_PIL_AUTH_MODEM_FW_PROC. */
  TZ_PIL_AUTH_QDSP6_PROC,       /**< LPASS Hexagon@tm processor. */
  TZ_PIL_AUTH_SPS_PROC,         /**< This enum member is deprecated. */
  TZ_PIL_AUTH_EXEC,             /**< QSEE generic executive image. */
  TZ_PIL_AUTH_MODEM_SW_PROC,    /**< Modem software processor. */
  TZ_PIL_AUTH_MODEM_FW_PROC,    /**< Modem firmware processor. */
  TZ_PIL_AUTH_WLAN_PROC,        /**< Riva processor. */
  TZ_PIL_AUTH_SEC_APP,          /**< QSEE software secure applications. */
  TZ_PIL_AUTH_GNSS_PROC,        /**< GNSS processor. */
  TZ_PIL_AUTH_VIDEO_PROC,       /**< Video processor. */
  TZ_PIL_AUTH_VPU_PROC,         /**< VPU processor. */
  TZ_PIL_AUTH_BCSS_PROC,        /**< BCSS processor. */
  TZ_PIL_AUTH_SSC_PROC,         /**< SSC Hexagon@tm processor. */
#ifndef CHIPSET_53xx
  TZ_PIL_AUTH_GPU_UCODE_PROC,   /**< GPU Microcode. */
#else
  TZ_PIL_AUTH_ROOTPD,
#endif
  TZ_PIL_AUTH_SPSS_PROC,        /**< Secure processor. */
  TZ_PIL_AUTH_IPA_GSI_PROC,     /**< IPA GSI Firmware. */
  TZ_PIL_AUTH_SEC_TEE,          /**< QSEE software secure TEE. */
  TZ_PIL_AUTH_SEC_LIB,          /**< QSEE software secure library. */
  TZ_PIL_AUTH_CDSP_PROC,        /**< CDSP processor. */
  TZ_PIL_AUTH_GSI_QUP_PROC,     /**< Firmware for the GSI/QUP. */
  TZ_PIL_AUTH_UEFIFV_PROC,      /**< UEFI Firmware Volumes. */
  TZ_PIL_AUTH_ABLFV_PROC,       /**< ABL (LinuxLoader). */
  TZ_PIL_AUTH_ACPI,             /**< ACPI Tables. */
  TZ_PIL_AUTH_NPU_PROC,         /**< NPU Firmware. */
  TZ_PIL_AUTH_EXT_DSP_PROC,     /**< External DSP Firmware. */
  TZ_PIL_AUTH_DAM_APP_PROC,     /**< Downloadable App Module (non-sec apps used in TX alone). */
  TZ_PIL_AUTH_CVP_PROC,         /**< CVP Firmware. */
  TZ_PIL_AUTH_IPA_UC_PROC,      /**< IPA uC Firmware. */
#ifdef CHIPSET_53xx
  TZ_PIL_AUTH_USERPD1 = 0x112,
  TZ_PIL_AUTH_USERPD2 = 0x212,
  TZ_PIL_AUTH_USERPD3 = 0x312,
  TZ_PIL_AUTH_USERPD4 = 0x412,
  TZ_PIL_AUTH_IUPD1 = 0x12C,
  TZ_PIL_AUTH_IUPD2 = 0x22C,
  TZ_PIL_AUTH_OEMUSERPD1 = 0x163,
  TZ_PIL_AUTH_OEMUSERPD2 = 0x263,
  TZ_PIL_AUTH_OEMUSERPD3 = 0x363,
  TZ_PIL_AUTH_OEMUSERPD4 = 0x463,
#endif
  TZ_PIL_AUTH_UNSUPPORTED_PROC  /**< Processor not supported. */
} tz_pil_proc_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/** Macro for checking whether a given proc is of secure SW type */
#define TZ_PIL_IS_SEC_PROC_TYPE(_proc) \
        ((_proc) == TZ_PIL_AUTH_SEC_APP || \
         (_proc) == TZ_PIL_AUTH_SEC_TEE || \
         (_proc) == TZ_PIL_AUTH_SEC_LIB)

/**
   @weakgroup weak_tz_pil_init_req_s
@{
*/
typedef struct tz_pil_init_req_s
{
  uint32               proc;           /**< Processor identifier for
                                            the current operation.
                                            See #tz_pil_proc_e_type. */
  uint8*               image_hdr_ptr;  /**< Pointer to the physical
                                            address location of the
                                            ELF header for the given
                                            processor identifier. */
} __attribute__ ((packed)) tz_pil_init_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_mem_req_s
@{
*/
typedef struct tz_pil_mem_req_s
{
  uint32               proc;        /**< Processor identifier for the
                                         current operation. See
                                         #tz_pil_proc_e_type. */

  uint64               start_addr;  /**< The HLOS can load the image
                                         in any memory location, so
                                         this is used to take the
                                         difference between the
                                         program header addresses and
                                         start_addr. */

  uint32               image_len;   /**< Total length of the image in
                                         memory. */
} __attribute__ ((packed)) tz_pil_mem_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_auth_reset_req_s
@{
*/
typedef struct tz_pil_auth_reset_req_s
{
  uint32               proc;        /**< Processor identifier for the
                                         current operation. See
                                         #tz_pil_proc_e_type. */
} __attribute__ ((packed)) tz_pil_auth_reset_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_unlock_req_s
@{
*/
typedef struct tz_pil_unlock_req_s
{
  uint32               proc;        /**< Processor ientifier for the
                                         current operation. See
                                         #tz_pil_proc_e_type. */
} __attribute__ ((packed)) tz_pil_unlock_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_subsystem_query_req_s
@{
*/
typedef struct tz_pil_subsystem_query_req_s
{
  uint32               proc;        /**< Processor/subsystem
                                        identifier. See
                                        #tz_pil_proc_e_type. */

} __attribute__ ((packed)) tz_pil_subsystem_query_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_subsystem_query_rsp_s
@{
*/
typedef struct tz_pil_subsystem_query_rsp_s
{
  uint32               verdict;     /**< TRUE (1) if the answer to
                                         the query is true; FALSE
                                         (0) otherwise. */
} __attribute__ ((packed)) tz_pil_subsystem_query_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_mem_area_query_req_s
@{
*/
typedef struct tz_pil_mem_area_query_req_s
{
  uint32               spare;       /**< Unused; set to zero.   */

} __attribute__ ((packed)) tz_pil_mem_area_query_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_mem_area_query_rsp_s
@{
*/
typedef struct tz_pil_mem_area_query_rsp_s
{
  uint64               start;       /**< Start address of a relocatable
                                         PIL region (inclusive). */

  uint64               end;         /**< End address of a relocatable
                                         PIL region (exclusive). */
} __attribute__ ((packed)) tz_pil_mem_area_query_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_modem_restart_s
@{
*/
typedef struct tz_pil_modem_restart_s
{
  uint32 config;                  /**< Value programmed into the
                                        MSS_RESTART register. */

  uint32 spare;                  /**< Spare. */
} __attribute__ ((packed)) tz_pil_modem_restart_t;

/** @} */ /* end_weakgroup */



/**
   @weakgroup weak_tz_pil_share_memory_s
@{
*/
typedef struct tz_pil_share_memory_s
{
  uint32  start;       /**< Start address of the memory. start=0 and size=0
                            means the full HLOS memory is shared. */
  uint32  size;         /**< Length of the memory. start=0 and size=0 means the full HLOS memory is shared. */
  uint32  proc;        /**< Processor identifier for the current operation. See #tz_pil_proc_e_type. */
  uint32  share_type;   /**< Bitwise OR of the TZ_PIL_SHARE_TYPE_* definitions.
                             Zero revokes sharing of previously shared memory. */
} __attribute__ ((packed)) tz_pil_share_memory_t;
/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_pil_protect_memory_s
@{
*/

typedef struct tz_pil_protect_memory_s
{
  uint32  subsystem_id;       /**< Subsystem ID. */
  uint64  phy_base;           /**< Physical address from which to protect. */
  uint32  size;               /**< Size  of the memory area to protect. */

} __attribute__ ((packed)) tz_pil_protect_memory_t;


/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_clear_protect_memory_s
@{
*/
typedef struct tz_pil_clear_protect_memory_s
{
  uint32  subsystem_id;       /**< Subsystem ID. */
  uint64  phy_base;           /**< Physical start address for the protected memory. */
  uint32  size;               /**< Size of the protected memory area. */
  uint32  auth_key;           /**< Authentication key that was saved in the
                                   secure channel while protecting the region.  */
} __attribute__ ((packed)) tz_pil_clear_protect_memory_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_get_rollback_version_req_s
@{
*/
typedef struct tz_pil_get_rollback_version_req_s
{
  uint32_t             subsys_id;       /**< Subsystem-ID   */

} __attribute__ ((packed)) tz_pil_get_rollback_version_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_get_rollback_version_rsp_s
@{
*/
typedef struct tz_pil_get_rollback_version_rsp_s
{
  uint32_t             version;       /**< Subsystem version */

} __attribute__ ((packed)) tz_pil_get_rollback_version_rsp_t;

/** @} */ /* end_weakgroup */


#define TZ_PIL_SHARE_TYPE_READ 0x1
#define TZ_PIL_SHARE_TYPE_WRITE 0x2


/*----------------------------------------------------------------------------
 * DUMP Service Structures
 * -------------------------------------------------------------------------*/
/** @addtogroup tz_datatypes
@{ */

/**
  * Memory protection-related device types.
  */
typedef enum
{
  TZ_DEVICE_VIDEO        =  0,      /**< Video subsystem. */
  TZ_DEVICE_MDSS         =  1,      /**< Mobile display subsystem. */
  TZ_DEVICE_LPASS        =  2,      /**< Low-power audio subsystem. */
  TZ_DEVICE_MDSS_BOOT    =  3,      /**< MDSS at cold boot. */
  TZ_DEVICE_USB1_HS      =  4,      /**< High speed USB. */
  TZ_DEVICE_OCMEM        =  5,      /**< OCMEM registers. */
  TZ_DEVICE_LPASS_CORE   =  6,      /**< Low power audio subsystem. */
  TZ_DEVICE_VPU          =  7,      /**< Video processing unit. */
  TZ_DEVICE_COPSS_SMMU   =  8,      /**< System memory mapping unit.*/
  TZ_DEVICE_USB3_0       =  9,      /**< USB 3.0.*/
  TZ_DEVICE_USB3_1       = 10,      /**< USB 3.0.*/
  TZ_DEVICE_PCIE_0       = 11,      /**< Peripheral component interconnect express.*/
  TZ_DEVICE_PCIE_1       = 12,      /**< Peripheral component interconnect express.*/
  TZ_DEVICE_BCSS         = 13,      /**< BCSS processor.*/
  TZ_DEVICE_VCAP         = 14,      /**< VCAP.*/
  TZ_DEVICE_PCIE20       = 15,      /**< Peripheral component interconnect express 2.0.*/
  TZ_DEVICE_IPA          = 16,      /**< Integrated parallel architecture.*/
  TZ_DEVICE_APPS         = 17,      /**< Application processor subsystem.*/
  TZ_DEVICE_GPU          = 18,      /**< Graphics processing unit.*/
  TZ_DEVICE_UFS          = 19,      /**< Universal flash storage.*/
  TZ_DEVICE_ICE          = 20,      /**< Inline crypto engine.*/
  TZ_DEVICE_ROT          = 21,      /* TODO. */
  TZ_DEVICE_VFE          = 22,      /* TODO. */
  TZ_DEVICE_ANOC0        = 23,      /* TODO. */
  TZ_DEVICE_ANOC1        = 24,      /* TODO. */
  TZ_DEVICE_ANOC2        = 25,      /* TODO. */
  TZ_DEVICE_CPP          = 26,      /* TODO. */
  TZ_DEVICE_JPEG         = 27,      /* TODO. */
  TZ_DEVICE_SLPI         = 28,      /* TODO. */
  TZ_DEVICE_RPM          = 29,
  TZ_DEVICE_MODEM        = 30,
  TZ_DEVICE_GPU_UCODE    = 31,
  TZ_DEVICE_MMNOC        = 32,
  TZ_DEVICE_SP           = 33,
  TZ_DEVICE_CDSP         = 34,
  TZ_DEVICE_MSS_NAV      = 35,
  TZ_DEVICE_NPU          = 36,      /**< Neural processing unit. */
  TZ_DEVICE_COUNT,
  TZ_DEVICE_MAX = 0x7FFFFFFF,
}tz_device_e_type;

typedef enum
{
  TZ_RETURN_FROM_ENTRY = 0,
  TZ_RETURN_FROM_EXIT = 1,
  TZ_RETURN_MAX = 0x7FFFFFFF,
} tz_ddr_self_refresh_return_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_pil_qdsp6_nmi_req_s
@{
*/
typedef struct tz_pil_qdsp6_nmi_req_s
{
  uint32               val;         /**< Value for the LPASS Hexagon
                                         NMI register. Bits other
                                         than the least significant
                                         one are ignored by the
                                         call. */
} __attribute__ ((packed)) tz_pil_qdsp6_nmi_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_set_cpu_ctx_buf_req_s
@{
*/
typedef struct tz_dump_set_cpu_ctx_buf_req_s
{
  uint32*              addr;        /**< Address of the WDT reset
                                         context dump buffer. When a
                                         nonsecure WDT expires, the
                                         CPU contexts are saved to
                                         this buffer. The entire
                                         buffer must be in nonsecure
                                         memory. */

  uint32               size;        /**< Size of the WDT reset context
                                         dump buffer in bytes. */
} __attribute__ ((packed)) tz_dump_set_cpu_ctx_buf_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_set_cache_buf_req_s
@{
*/
typedef struct tz_dump_set_cache_buf_req_s
{
  uint32*              addr;        /**< Address of L1 or L2 dump buffer.

                                         When a nonsecure WDT that is
                                         configured as a Fast Interrupt
                                         Request (FIQ) expires, or if the
                                         nonsecure side requests L1 dumping,
                                         L1 caches are saved to this buffer.

                                         The entire buffer must be in nonsecure
                                         memory and must be 4 KB aligned.
                                         L2 is dumped only on a nonsecure
                                         side request. */

  uint32               size;        /**< Size of the L1 dump buffer in
                                         bytes. */
} __attribute__ ((packed)) tz_dump_set_cache_buf_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_cache_buf_size_req_s
@{
*/
typedef struct tz_dump_query_cache_buf_size_req_s
{
  uint32               spare;       /**< Spare field for future use;
                                         must be zero. */
} __attribute__ ((packed)) tz_dump_query_cache_buf_size_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_cache_buf_size_rsp_s
@{
*/
typedef struct tz_dump_query_cache_buf_size_rsp_s
{
  uint32               size;        /**< After a successfull call, the L1/L2
                                         dump size in bytes is stored
                                         here. */
} __attribute__ ((packed)) tz_dump_query_cache_buf_size_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_set_ocmem_buf_req_s
@{
*/
typedef struct tz_dump_set_ocmem_buf_req_s
{
  uint32*                       addr;
    /**< Address of OCMEM dump buffer. When a nonsecure WDT as an FIQ
         expires, or if the nonsecure side requests OCMEM dumping, OCMEM
         data is saved to this buffer.

         The entire buffer must be in nonsecure memory and must be 4KB aligned. */

  uint32                        size;
    /**< Size of the dump buffer in bytes. */
} __attribute__ ((packed)) tz_dump_set_ocmem_buf_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_ocmem_buf_size_req_s
@{
*/
typedef struct tz_dump_query_ocmem_buf_size_req_s
{
  uint32               spare;       /**< Spare field for future use;
                                         must be zero. */
} __attribute__ ((packed)) tz_dump_query_ocmem_buf_size_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_ocmem_buf_size_rsp_s
@{
*/
typedef struct tz_dump_query_ocmem_buf_size_rsp_s
{
  uint32                        size;        /**< After a successfull call, the
                                                  OCMEM dump size in bytes is
                                                  stored here. */
} __attribute__ ((packed)) tz_dump_query_ocmem_buf_size_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_security_allows_mem_dump_rsp_s
@{
*/
typedef struct tz_security_allows_mem_dump_rsp_s
{
  uint32              allowed;              /**< If nonzero, the security policy
                                                 permits dumping memory for
                                                 debugging. */
  uint32              is_dload_cookie_set;  /**< Boolean (TRUE/FALSE) */
  uint32              apdp_flag_status;     /**< Status of each APDP Flag */
} __attribute__ ((packed)) tz_security_allows_mem_dump_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_security_is_retail_unlock_rsp_s
@{
*/
typedef struct tz_security_is_retail_unlock_rsp_s
{
  uint32                      enabled;
                                    /**< If nonzero, the retail unlock
                                         is enabled. */
} __attribute__ ((packed)) tz_security_is_retail_unlock_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ddr_self_refresh_return_req_s
@{
*/
typedef struct tz_ddr_self_refresh_return_req_s
{
  uint32                      return_type;
                                    /**< Type of API being returned from;
                                         see #tz_ddr_self_refresh_return_type. */
} __attribute__ ((packed)) tz_ddr_self_refresh_return_req_s;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * INLINE_CRYPTO_ENGINE Service Structures
 * -------------------------------------------------------------------------*/

/*  Pass in config parameters, otherwise default_cntx_flag should be TRUE for defaults
    Default chosen config (if default_cntx_flag is TRUE):
    encr_key_size     :   AES_128
    encr_mode         :   XTS
    key_selection     :   USE_LUT_SW_KEY
    decr_bypass       :   BYPASS_DISABLE
    encr_bypass       :   BYPASS_DISABLE


*/

/**
   @weakgroup weak_tz_os_ice_set_context_config_req_s


@{
*/
typedef struct tz_os_ice_set_context_config_req_s
{
  uint32  ce;				 /**< ICE number. */
  uint32  pipe; 			 /**< ICE key LUT index. */
  boolean default_cntx_flag; /**< Default context size. */
  uint8   encr_key_size;	 /**< Encryption key size. */
  uint8   decr_bypass;		 /**< Decryption bypass. */
  uint8   encr_bypass;		 /**< Encryption bypass. */
} __attribute__ ((packed)) tz_os_ice_set_context_config_req_t;

/** @} */ /* end_weakgroup */

/*  ce              :   ICE number (10 is UFS ICE, 20 is EMMC ICE)
    pipe            :   ICE key LUT index from which the key is disabled
*/

/**
   @weakgroup weak_tz_os_ice_key_config_restore_req_s
@{
*/
typedef struct tz_os_ice_key_config_restore_req_s
{
  uint32 ce;     /**< ICE number (10 is UFS ICE, 20 is EMMC ICE). */
} __attribute__ ((packed)) tz_os_ice_key_config_restore_req_t;


/**
   @weakgroup weak_tz_os_ice_disable_sequence_req_s
@{
*/
typedef struct tz_os_ice_disable_sequence_req_s
{
  uint32 pipe;	 /**< ICE key LUT index from which the key is disabled. */
  uint32 ce;	 /**< ICE number (10 is UFS ICE, 20 is EMMC ICE). */
} __attribute__ ((packed)) tz_os_ice_disable_sequence_req_t;

/** @} */ /* end_weakgroup */


/*----------------------------------------------------------------------------
 * RES_LOCKING Service Structures
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_datatypes
@{ */

/**
   Hardware resources available for configuration and locking by
   the QSEE.

   @note1hang The CE numbering is logical and does not map to actual
              hardware CE naming directly.
*/
typedef enum
{
  TZ_RESOURCE_CE1    = 1, /**< This enum member is deprecated.
                                  Use #TZ_RESOURCE_CE_MSS. */
  TZ_RESOURCE_CE_MSS = 1, /**< Modem CE resource. */
  TZ_RESOURCE_CE2    = 2, /**< This enum member is
                                  deprecated. Use #TZ_RESOURCE_CE_AP. */
  TZ_RESOURCE_CE_AP  = 2, /**< Application processor's CE resource. */
  TZ_RESOURCE_RNG    = 3  /**< Random number generator. */
} TZ_XPU_RESOURCE_ET;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_lock_resource_req_s
@{
*/
typedef struct tz_lock_resource_req_s
{
  uint32               res;         /**< Resource to lock. See
                                         #TZ_XPU_RESOURCE_ET. */

  uint32               lock;        /**< Set to 1 to lock the resource
                                         and 0 to unlock it. */
} __attribute__ ((packed)) tz_lock_resource_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_lock_resource_rsp_s
@{
*/
typedef struct tz_lock_resource_rsp_s
{
  int32                ret;         /**< Return code: 0 means success;
                                         E_BUSY means the resource is
                                         locked; other negative
                                         values mean failure. */
} __attribute__ ((packed)) tz_lock_resource_rsp_t;

/** @} */ /* end_weakgroup */


/** @addtogroup tz_macros_constants
@{ */

/** @name Crypto Engine Access Method
@{ */

/**
   Application processor accesses the crypto engine using registers.
*/
#define TZ_RESCFG_AP_CE_REGISTER_USE  0
/**
   Application processor accesses the crypto engine using the data mover.
*/
#define TZ_RESCFG_AP_CE_ADM_USE       1

/** @} */ /* end_namegroup Crypto Engine Access Method */

/** @} */ /* end_addtogroup tz_macros_constants */


/**
   @weakgroup weak_tz_config_resource_req_s
@{
*/
typedef struct tz_config_resource_req_s
{
  uint32               res;         /**< Resource to configure. See
                                         #TZ_XPU_RESOURCE_ET. */

  uint32               arg;         /**< Argument to the resource
                                         configuration. */
} __attribute__ ((packed)) tz_config_resource_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/** BLSP subsystem owner. */
typedef enum
{
  tzbsp_blsp_owner_none = 0,  /**< No BLSP owner. */
  tzbsp_blsp_owner_apss = 1,  /**< BLSP owner is the APSS. */
  tzbsp_blsp_owner_adsp = 2,  /**< BLSP owner is the ADSP. */
  tzbsp_blsp_owner_tz   = 3   /**< BLSP owner is the TZ software. */
} tzbsp_blsp_owner_ss_type;
/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_modify_ownership_req_s
@{
*/
typedef struct tz_modify_blsp_ownership_req_s
{
  uint32                   blsp_periph_num;   /**< BLSP peripheral number. */
  tzbsp_blsp_owner_ss_type    owner_ss;       /**< Owner subsystem. See #tzbsp_blsp_owner_ss_type. */
} __attribute__ ((packed)) tz_modify_blsp_ownership_req_t;
/** @} */ /* end_weakgroup */

/** BLSP Handle SSR. */

/**
   @weakgroup weak_tz_handle_ssr_req_s
@{
*/
typedef struct tz_blsp_handle_ssr_req_s
{
  uint32                   processor_type;   /**< Processor type. */
} __attribute__ ((packed)) tz_blsp_handle_ssr_req_t;
/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * INFO Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_info_is_svc_available_req_s
@{
*/
typedef struct tz_info_is_svc_available_req_s
{
  uint32               id;          /**< Command ID of the queried service. */
} __attribute__ ((packed)) tz_info_is_svc_available_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_info_is_svc_available_rsp_s
@{
*/
typedef struct tz_info_is_svc_available_rsp_s
{
  boolean              found;       /**< If TRUE, the searched service
                                         was found; FALSE otherwise. */
} __attribute__ ((packed)) tz_info_is_svc_available_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_info_get_diag_req_s
@{
*/
typedef struct tz_info_get_diag_req_s
{
  uint8*               buffer;      /**< Pointer to the buffer to where the
                                         QSEE software copies the
                                         diagnostic area. */
  uint32               buffer_size; /**< Size of the buffer in bytes.
                                         This must be at least the
                                         size of the diagnostic
                                         area. If the buffer is larger
                                         than the diagnostic area, the
                                         remaining bytes are ignored
                                         by the QSEE software. */
} __attribute__ ((packed)) tz_info_get_diag_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_feature_version_req_s
@{
*/
typedef struct tz_feature_version_req_s
{
  uint32               feature_id;  /**< Feature for which to request
                                         the version information.
                                         See #tz_feature_t. */
} __attribute__ ((packed)) tz_feature_version_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_feature_version_rsp_s
@{
*/
typedef struct tz_feature_version_rsp_s
{
  uint32               version;     /**< Version of the specified
                                         feature, or zero if the
                                         feature is undefined. */
} __attribute__ ((packed)) tz_feature_version_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_get_secure_state_rsp_s
@{
*/
typedef struct tz_get_secure_state_rsp_s
{
  uint32                        status_0;  /**< First half of the secure status. */
  uint32                        status_1;  /**< Second half of the secure status. */
}__attribute__ ((packed)) tz_get_secure_state_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_get_secure_state_legacy_req_s
@{
*/
typedef struct tz_get_secure_state_legacy_req_s
{
  uint32*                       status_ptr;  /**< Pointer to the query status struct. */
  uint32                        status_len;  /**< Length of the query status struct. */
}__attribute__ ((packed)) tz_get_secure_state_legacy_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_bw_prof_req_s
@{
*/
typedef struct tz_bw_prof_req_s
{
  uint32*                       req_ptr;  /**< Pointer to a buffer with request structure. */
  uint32                        req_len;  /**< Length of the request structure buffer. */
  uint32*                       rsp_ptr;  /**< Pointer to a buffer with response structure */
  uint32                        rsp_len;  /**< Length of the response structure buffer. */
}__attribute__ ((packed)) tz_bw_prof_req_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * Secure Halt service
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_get_sec_halt_status_rsp_s
@{
*/
typedef struct tz_get_sec_halt_status_rsp_s
{
  uint32                        ret0; /**< Generic return value 0. */
  uint32                        ret1; /**< Generic return value 1. */
  uint32                        ret2; /**< Generic return value 2. */
  uint32                        ret3; /**< Generic return value 3. */
} __attribute__ ((packed)) tz_get_sec_halt_status_rsp_t;
/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * SSD Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_ssd_decrypt_img_req_s
@{
*/
typedef struct tz_ssd_decrypt_img_req_s
{
  uint64               img_ptr;     /**< Double pointer to the image. */
  uint32               img_len_ptr; /**< Pointer to the length of the
                                         image. */
} __attribute__ ((packed)) tz_ssd_decrypt_img_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_encrypt_keystore_req_s
@{
*/
typedef struct tz_ssd_encrypt_keystore_req_s
{
  uint64               keystore_ptr;      /**< Double pointer to the
                                               keystore. */
  uint32               keystore_len_ptr;  /**< Pointer to the length
                                               of the keystore. */
} __attribute__ ((packed)) tz_ssd_encrypt_keystore_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_protect_keystore_req_s
@{
*/
typedef struct tz_ssd_protect_keystore_req_s
{
  uint32*                       keystore_ptr;  /**< Pointer to the SSD keystore. */
  uint32                        keystore_len;  /**< Length of the keystore. */
} __attribute__ ((packed)) tz_ssd_protect_keystore_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/**
   Return types for the Load Keystore command.
*/
typedef enum
{
  TZ_SSD_PKS_SUCCESS            = 0, /**< Successful return. */
  TZ_SSD_PKS_INVALID_PTR        = 1, /**< Keystore pointer invalid. */
  TZ_SSD_PKS_INVALID_LEN        = 2, /**< Keystore length incorrect. */
  TZ_SSD_PKS_UNALIGNED_PTR      = 3, /**< Keystore pointer not word
                                             aligned. */
  TZ_SSD_PKS_PROTECT_MEM_FAILED = 4, /**< Failure when protecting
                                             the keystore memory. */
  TZ_SSD_PKS_INVALID_NUM_KEYS   = 5, /**< Unsupported number of
                                          keys passed. If a valid
                                          pointer to nonsecure
                                          memory is passed, it
                                          is not a keystore; it is
                                          a likely return code. */
  TZ_SSD_PKS_DECRYPT_FAILED     = 6  /**< Keystore could not be
                                          decrypted. */
} tz_ssd_pks_return_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_ssd_protect_keystore_rsp_s
@{
*/
typedef struct tz_ssd_protect_keystore_rsp_s
{
  uintptr_t                     status;       /**< Status of the Load Keystore
                                                   call. See
                                                   #tz_ssd_pks_return_type.
                                               */
} __attribute__ ((packed)) tz_ssd_protect_keystore_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_parse_md_req_s
@{
*/
typedef struct tz_ssd_parse_md_req_s
{
  uint32                        md_len;       /**< Length of the metadata. */
  uint32*                       md;           /**< Pointer to the metadata to
                                                   parse. The memory must be
                                                   word-aligned, as the type
                                                   implies. */
} __attribute__ ((packed)) tz_ssd_parse_md_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/**
   Return types for the parse metadata command.
*/
typedef enum
{
  TZ_SSD_PMD_ENCRYPTED             = 0,    /**< Parsing was successful
                                                   and the image is
                                                   encrypted. */
  TZ_SSD_PMD_NOT_ENCRYPTED         = 1,    /**< Parsing was successful
                                                   and the image is not
                                                   encrypted. */
  TZ_SSD_PMD_NO_MD_FOUND           = 3,    /**< No SSD metadata
                                                   start tag was found
                                                   in the given
                                                   memory. This
                                                   probably indicates
                                                   that the image is
                                                   not encrypted. It
                                                   could also indicate
                                                   that the start tag
                                                   was truncated. */
  TZ_SSD_PMD_BUSY                  = 4,    /**< Parsing was not attempted
                                                   because no parsing context
                                                   was available at this
                                                   time. */
  TZ_SSD_PMD_BAD_MD_PTR_OR_LEN     = 5,    /**< Could not search the
                                                   memory because the
                                                   pointer or length was
                                                   bad. */
  TZ_SSD_PMD_PARSING_INCOMPLETE    = 6,    /**< The parsed metadata
                                                   is incomplete. Call
                                                   again with a larger
                                                   buffer to scan. */
  TZ_SSD_PMD_PARSING_FAILED        = 7,    /**< There was a parsing
                                                   error in the metadata. */
  TZ_SSD_PMD_SETUP_CIPHER_FAILED   = 8,    /**< There was a problem
                                                   finding the correct key
                                                   in the keystore or
                                                   initializing the crypto
                                                   hardware. */
} tz_ssd_pmd_return_type;

/** @} */ /* end_addtogroup tz_datatypes */


/**
   @weakgroup weak_tz_ssd_parse_md_rsp_info_s
@{
*/
typedef struct tz_ssd_parse_md_rsp_info_s
{
  uintptr_t                     status;       /**< Status of the request. See
                                                   #tz_ssd_pmd_return_type.
                                               */
  uintptr_t                     md_ctx_id;    /**< Context ID to use for a
                                                   decrypt fragment call. */
  uintptr_t                     md_end_ptr;   /**< Pointer to the first memory
                                                   after the SSD metadata.
                                                   This pointer is always
                                                   word-aligned. If the
                                                   image was encrypted all at
                                                   once, this is the
                                                   start of the encrypted
                                                   data. */
} __attribute__ ((packed)) tz_ssd_parse_md_rsp_info_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_ssd_parse_md_rsp_s
@{
*/
typedef struct tz_ssd_parse_md_rsp_s
{
  struct tz_ssd_parse_md_rsp_info_s    rsp;   /**< Parse MD response
                                                   structure. */
} __attribute__ ((packed)) tz_ssd_parse_md_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_decrypt_img_frag_req_s
@{
*/
typedef struct tz_ssd_decrypt_img_frag_req_s
{
  uint32                        md_ctx_id;    /**< Context ID given in the
                                                   parse metadata call. */
  uint32                        last_frag;    /**< Whether the passed
                                                   fragment is the
                                                   last one (1 = last
                                                   fragment). */
  uint32                        frag_len;     /**< Length of the
                                                   fragment. The
                                                   fragment length must
                                                   be a multiple of the
                                                   encryption block
                                                   size.  */
  uint32*                       frag;         /**< Encrypted fragment.
                                                   The fragment must
                                                   be word-aligned, as
                                                   noted by the
                                                   uint32* type. */
} __attribute__ ((packed)) tz_ssd_decrypt_frag_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_decrypt_elf_seg_frag_req_s
@{
*/
typedef struct tz_ssd_decrypt_elf_seg_frag_req_s
{
  uint32                        md_ctx_id;    /**< Context ID given in the
                                                   parse metadata call. */
  uint32                        segment_num;  /**< ELF segment number
                                                   for the given
                                                   fragment. Note that
                                                   all the fragments
                                                   for a given ELF
                                                   segment must be
                                                   received before
                                                   going to the next
                                                   segment. */
  uint32                        last_seg;     /**< Whether the current
                                                   segment is the last
                                                   one (1 = last segment). */
  uint32                        last_frag;    /**< Whether the passed
                                                   fragment is the last
                                                   one for a given
                                                   segment_num (1 =
                                                   last fragment). */
  uint32                        frag_len;     /**< Length of the fragment.
                                                   The fragment length
                                                   must be a multiple of the
                                                   encryption block size
                                                   unless the last_frag flag
                                                   is passed.  */
  uint32*                       frag;         /**< Encrypted fragment.
                                                   The fragment must
                                                   be word-aligned, as
                                                   noted by the
                                                   uint32* type. */
} __attribute__ ((packed)) tz_ssd_decrypt_elf_seg_frag_req_t;

/** @} */ /* end_weakgroup */



/** @addtogroup tz_datatypes
@{ */

/**
   Return types for the Decrypt (image/ELF segment) Fragment command.
*/
typedef enum
{
  TZ_SSD_DF_SUCCESS               = 0,     /**< Fragment decrypted
                                                   successfully. */
  TZ_SSD_DF_BAD_FRAG_PTR_OR_LEN   = 1,     /**< Fragment pointer or
                                                   length incorrect. */
  TZ_SSD_DF_CTX_NOT_FOUND         = 2,     /**< Fragment context
                                                   not found in
                                                   currently available
                                                   contexts. */
  TZ_SSD_DF_LEN_ALIGNMENT_FAILED  = 3,     /**< Length alignment of the
                                                   fragment is incorrect. */
  TZ_SSD_DF_DECRYPTION_FAILED     = 4      /**< Problem with decryption. */
} tz_ssd_df_return_type;

/** @} */ /* end_addtogroup tz_datatypes */


/**
   @weakgroup weak_tz_ssd_decrypt_frag_rsp_s
@{
*/
typedef struct tz_ssd_decrypt_frag_rsp_s
{
  uintptr_t                     status;       /**< Status of the decrypt
                                                   request. See
                                                   #tz_ssd_df_return_type.
                                               */
} __attribute__ ((packed)) tz_ssd_decrypt_frag_rsp_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * FUSE Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_sw_fuse_req_s
@{
*/
typedef struct tz_sw_fuse_req_s
{
  uint32               fuse_id;     /**< Fuse ID. See
                                         #tz_sw_fuse_t. */
} __attribute__ ((packed)) tz_sw_fuse_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_sw_fuse_rsp_s
@{
*/
typedef struct tz_sw_fuse_rsp_s
{
  boolean              fuse_blown;  /**< If TRUE, the specified fuse
                                         was blown; FALSE
                                         otherwise. */
} __attribute__ ((packed)) tz_sw_fuse_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_write_row_s
@{
*/
typedef struct tz_qfprom_write_row_s
{
  uint32               bus_clk_khz;        /**< Bus clock speed in kHz. */
  uint32               raw_row_address;    /**< Raw row address of the fuse
                                                to blow. */
  uint32*              row_data;           /**< Pointer to the data buffer. */
  uint32               row_data_len;       /**< Size of the data buffer. */
} __attribute__ ((packed)) tz_qfprom_write_row_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_write_multiple_rows_s
@{
*/
typedef struct tz_qfprom_write_multiple_rows_s
{
  uint32*              row_array;          /**< Row array. */
  uint32               row_array_size;     /**< Size of the row array. */
  uint32               bus_clk_khz;        /**< Bus clock in kHz. */
  uint32*              qfprom_api_status;  /**< Status code from the QFPROM. */
} __attribute__ ((packed)) tz_qfprom_write_multiple_rows_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_read_row_s
@{
*/
typedef struct tz_qfprom_read_row_s
{
  uint32               row_address;        /**< Row address of the fuse to
                                                read. */
  int32                addr_type;          /**< Indicates whether the address
                                                is raw corrected. */
  uint32*              row_data;           /**< Pointer to the buffer that is
                                                to hold the read data. */
  uint32*              qfprom_api_status;  /**< Status code from the QFPROM. */
} __attribute__ ((packed)) tz_qfprom_read_row_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_rollback_write_row_s
@{
*/
typedef struct tz_qfprom_rollback_write_row_s
{
  uint32               bus_clk_khz;        /**< Bus clock in kHz. */
  uint32               raw_row_address;    /**< Raw row address of the fuse
                                                to blow. */
  uint32*              row_data;           /**< Pointer to the data buffer. */
  uint32               row_data_len;       /**< Size of the buffer that is
                                                to hold the read data. */
} __attribute__ ((packed)) tz_qfprom_rollback_write_row_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * CRYPTO Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_get_prng_data_s
@{
*/
typedef struct tz_get_prng_data_s
{
  uint8*                out_buf;     /**< Pointer to the buffer to fill
                                          with data from the PRNG. */
  uint32                out_buf_sz;  /**< Size of the output buffer. */
} __attribute__ ((packed)) tz_get_prng_data_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * SC_KEY_EXCHANGE Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_sc_init_s
@{
*/
typedef struct tz_sc_init_s
{
  uint32                ssid;        /**< Subsystem ID. See
                                          #tz_sc_ss_e_type. */
  void*                 address_ptr; /**< Pointer to the address location. */
  uint32                len;         /**< Length of the allocated memory. */
  int*                  status_ptr;  /**< Return status. */
} __attribute__ ((packed)) tz_sc_init_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_sc_deinit_s
@{
*/
typedef struct tz_sc_deinit_s
{
  uint32               ssid;          /**< Subsystem ID. */
  int*                 status_ptr;    /**< Return status. */
} __attribute__ ((packed)) tz_sc_deinit_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * MEMORY_PROTECTION Service Structures
 * -------------------------------------------------------------------------*/

/**
   @weakgroup weak_hyp_memprot_chunks_info_s
@{
*/
typedef struct hyp_memprot_ipa_info_s
{
  uint64              IPAaddr;         /**< IPA address is a 64-bit intermediate physical
                                            address. */

  uint64              IPAsize;         /**< Size in bytes of the IPA address. */

} __attribute__ ((packed)) hyp_memprot_ipa_info_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_hyp_memprot_dstVM_perm_info_s
@{
*/
typedef struct hyp_memprot_dstVM_perm_info_s
{
  uint32              dstVM;         /**< Destination VM defined by ACVirtualMachineId. */

  uint32              dstVMperm;     /**< Permissions of the IPA to be mapped to VM, bitwise OR of AC_PERM. */

  uint64                ctx;          /**< Destination of the VM-specific context information. */

  uint32              ctxsize;       /**< Size of context buffer in bytes. */

} hyp_memprot_dstVM_perm_info_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_hyp_memprot_assign_s
@{
*/
typedef struct hyp_memprot_assign_s
{
  const hyp_memprot_ipa_info_t           *IPAinfolist;    /**< List of IPA information structures. */

  uint32                                 IPAinfolistsize; /**< Size in bytes of the IPAinfolist entries. */

  const uint32                           *sourceVMlist;   /**< Source VMlist, defined by ACVirtualMachineId.  */

  uint32                                 srcVMlistsize;   /**< Size of sourceVMlist in bytes*/

  const hyp_memprot_dstVM_perm_info_t    *destVMlist;     /**< Defined by hyp_memprot_dstVM_perm_info_t. */

  uint32                                 destVMlistsize;   /**< Size of destVMlistAndPerm in bytes. */

  uint64                                 spare;           /**< Reserved for future use. */
} hyp_memprot_assign_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_tagvm_s
@{
*/
typedef struct tz_memprot_tagVM_t
{
  const hyp_memprot_ipa_info_t   *IPAinfolist;    /**< List of IPA information structures. */

  uint32                         IPAinfolistsize; /**< Size in bytes of the IPAinfolist entries. */

  const uint32                   *destVMlist;     /**< Destination VMlist, defined by ACVirtualMachineId. */

  uint32                         dstVMlistsize;   /**< Total number of entries in destVMlist. */

  uint64                         flags;           /**< Reserved for future use. */
} __attribute__ ((packed)) tz_memprot_tagVM_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_protect_memory_s
@{
*/
typedef struct tz_protect_memory_s
{
  uint64               start;       /**< Start address of the range. */
  uint64               end;         /**< End address of the range. */
  uint32               tag_id;      /**< Tag to determine the
                                         permissions. */
  boolean              lock;        /**< TRUE to protect; FALSE
                                         to unprotect. */
} __attribute__ ((packed)) tz_protect_memory_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_assign_io_range_s
@{
*/
typedef struct tz_assign_io_range_s
{
  uint64               start;       /**< Start address of the range. */
  uint64               end;         /**< End address of the range. */
  uint32               dest;        /**< Entity being assigned to/unassigned
                                         from. */
  uint32               perm;        /**< Permissions. */
  uint32               lock;        /**< Nonzero to protect; Zero to
                                         unprotect. */
} __attribute__ ((packed)) tz_assign_io_range_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * RTIC service structures
 * -------------------------------------------------------------------------*/

 /**
   @weakgroup weak_hyp_notify_rtic_dtb_s
@{
*/
typedef struct hyp_notify_rtic_dtb_t
{
  const uint64_t   KernelPhysBase;    /**< Kernel Physical Base.    */
  const uintptr_t  DTBAddress;        /**< Address of the RTIC DTB. */

  size_t         DTBSize;             /**< Size of the RTIC DTB.    */
} __attribute__ ((packed)) hyp_notify_rtic_dtb_t;

/** @} */ /* end_weakgroup */

 /**
   @weakgroup weak_hyp_share_el2_report_mem_hlos_s
@{
*/
typedef struct hyp_share_el2_report_mem_hlos_t
{
  const uintptr_t   EL2ReportMemAddress;  /**< Address of the EL2 report memory.  */

  size_t         EL2ReportMemSize;        /**< Size of the EL2 report memory.     */
} __attribute__ ((packed)) hyp_share_el2_report_mem_hlos_t;

 /**
   @weakgroup weak_tz_share_el2_report_mem_rtic_s_s
@{
*/
typedef struct tz_share_el2_report_mem_rtic_s_t
{
  const uintptr_t   EL2ReportMemAddress;  /**< Address of the EL2 report memory  */

  size_t         EL2ReportMemSize;        /**< Size of the EL2 report memory.    */
} __attribute__ ((packed)) tz_share_el2_report_mem_rtic_s_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/**
   Return error codes for TZ_SHARE_EL2_REPORT_MEM_RTIC SMC call.
*/
typedef enum
{
  RTIC_SUCCESS                       = 0, /**< Success.                                        */
  RTIC_ERR_SYSCALL_REISSUED          = 1, /**< Error due to syscall being re-issued.           */
  RTIC_ERR_INVAL_ARGS_NULL           = 2, /**< Error due to one or more arguments are Null.    */
  RTIC_ERR_INVAL_ARGS_OVERFLOW       = 3, /**< Error due to integer overflow with arguments.   */
  RTIC_ERR_INSUFFICIENT_REGION_SIZE  = 4, /**< Error due to insufficient region size provides. */
  RTIC_ERR_INVAL_REGION              = 5, /**< Error due to invalid region passed.             */
} rtic_err_response_t;

/** @} */ /* end_addtogroup tz_datatypes */

/*----------------------------------------------------------------------------
 * Memory protection service structures
 * -------------------------------------------------------------------------*/

/**
   @weakgroup weak_tz_restore_sec_cfg_req_s
@{
*/

typedef struct tz_restore_sec_cfg_req_s
{
  uint32                        device;   /**< Device to be restored;
                                               see #tz_device_e_type. */
  uint32                        spare;    /**< Spare. */
} __attribute__ ((packed)) tz_restore_sec_cfg_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_set_vmidmt_memtype_req_s
@{
*/
typedef struct tz_set_vmidmt_memtype_req_s
{
  uint32                        device;   /**< Device to be configured;
                                               see #tz_device_e_type. */
  uint32                        vmid_idx; /**< Index for the memtype to be set;
                                               0xFF for all indecies. */
  uint32                        memtype;  /**< Memory type value to be set. */
} __attribute__ ((packed)) tz_set_vmidmt_memtype_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_video_var_req_s
@{
*/
typedef struct tz_memprot_video_var_req_s
{
  uint64                     cp_start;
    /**< Start address of the secure memory (CP) range in the device's virtual
         address space. */
  uint32                     cp_size;
    /**< Size of the secure memory range (CP) in the device's virtual address
         space. The end address is calculated as: cp_end = [cp_start +
         cp_size - 1]. */
  uint64                     cp_nonpixel_start;
    /**< Start address of nonpixel data in the device's virtual address space.
         Set cp_start < cp_nonpixel_start < cp_end cp_nonpixel so that the
         range does not overlap with the firmware image. */
  uint32                     cp_nonpixel_size;
    /**< Size of nonpixel data in the device's virtual address space. The end
         address is calculated as: cp_nonpixel_end = [cp_nonpixel_start +
         cp_nonpixel_size - 1]. Also, set cp_nonpixel_start < cp_nonpixel_end
         @le cp_end cp_nonpixel so that the range does not overlap with the
         firmware image. */
} __attribute__ ((packed)) tz_memprot_video_var_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_dev_var_req_s
@{
*/
typedef struct tz_memprot_dev_var_req_s
{
  uint32                     device_id;
  /**< ID of the device where this memory is to be mapped.
       See #tz_device_e_type. */
  tz_va_range_t              *va_list;
    /**< List of start addresses and sizes of the secure memory ranges in the device's
         virtual address space. */
  uint32                     list_size;
    /**< Size of the VAR list. */
} __attribute__ ((packed)) tz_memprot_dev_var_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_ptbl_mem_size_req_s
@{
*/
typedef struct tz_memprot_ptbl_mem_size_req_s
{
   uint32                        spare;      /**< Spare field for future use;
                                                  must be zero. */
} __attribute__ ((packed)) tz_memprot_ptbl_mem_size_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_ptbl_mem_size_rsp_s
@{
*/
typedef struct tz_memprot_ptbl_mem_size_rsp_s
{
  uint32                       mem_size;    /**< Required size of the
                                                 page table memory
                                                 region in bytes. */
  int32                        ret;         /**< Extended return code; 0 for
                                                 success, negative values
                                                 mean failure. */
} __attribute__ ((packed)) tz_memprot_ptbl_mem_size_rsp_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_ptbl_mem_init_req_s
@{
*/
typedef struct tz_memprot_ptbl_mem_init_req_s
{
  uint64                        addr;       /**< Start address of the page
                                                 table memory region
                                                 (16 kB aligned).  */
  uint32                        size;       /**< Size of page table memory
                                                 region in bytes. */
  uint32                        spare;      /**< Spare field for future use;
                                                 must be zero. */
} __attribute__ ((packed)) tz_memprot_ptbl_mem_init_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_memprot_rsp_s
@{
*/
typedef struct tz_memprot_rsp_s
{
  uint32                        rsp;       /**< Result of this syscall.  */
} __attribute__ ((packed)) tz_memprot_rsp_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_set_cp_pool_size_req_s
@{
*/
typedef struct tz_memprot_set_cp_pool_size_req_s
{
   uint32                        size;       /**< CP pool size */
   uint32                        spare;      /**< Spare field for future use;
                                                  must be zero. */
} __attribute__ ((packed)) tz_memprot_set_cp_pool_size_req_t;

/** @} */ /* end_weakgroup */


/** @addtogroup tz_datatypes
@{ */

/** Enumerates memory usage. */
typedef enum
{
  TZ_MEM_USAGE_CP_VIDEO_BITSTREAM  = 0x1, /**< Memory used for video
                                                  bistream (compressed)
                                                  CP data. */
  TZ_MEM_USAGE_CP_VIDEO_PIXEL      = 0x2, /**< Memory used for video
                                                  pixel (uncompressed)
                                                  CP data. */
  TZ_MEM_USAGE_CP_VIDEO_NONPIXEL   = 0x3, /**< Memory used for video
                                                  nonpixel CP data. */
  TZ_MEM_USAGE_CP_SD               = 0x4, /**< Memory used for secure
                                                  display data. */
  TZ_MEM_USAGE_CP_SC               = 0x5, /**< Memory used for secure
                                                  camera data. */
  TZ_MEM_USAGE_CP_APP              = 0x6, /**< Memory used for application
                                                  data. */
  TZ_MEM_USAGE_COUNT,
  TZ_MEM_USAGE_UNKNOWN  = 0x7fffffff
}tz_mem_usage_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_memprot_chunks_info_s
@{
*/
typedef struct tz_memprot_chunks_info_s
{
  uint32*             chunk_list;     /**< Array of chunk addresses. Each entry
                                           is a 32-bit physical address.
                                           chunk_list_size gives the number of
                                           entries in the array. */
  uint32              chunk_list_size;/**< Number of chunks in the chunk
                                           list. */
  uint32              chunk_size;     /**< Size of each memory chunk (in bytes)
                                           in the chunk list. */
} __attribute__ ((packed)) tz_memprot_chunks_info_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_map_info_s
@{
*/
typedef struct tz_memprot_map_info_s
{
  uint32              device_id;    /**< ID of the device where this memory is
                                         to be mapped. See #tz_device_e_type.
                                         The usage hint provided while
                                         protecting the memory can be used to
                                         validate this request. */
  uint32              ctxt_bank_id; /**< Context bank ID where this memory
                                         is to be (un)mapped. These
                                         correspond to context bank numbers
                                         and are unique only for the device.
                                         Values start from 0, which corresponds
                                         to CB0. */
  uint32              dva_start;    /**< Start address in the device's virtual
                                         address space to which to map. */
  uint32              size;         /**< Size in bytes of the virtual address
                                         range to map. The end address is:
                                         dva_end = dva_start + size - 1. */
} __attribute__ ((packed)) tz_memprot_map_info_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_macros_constants
@{ */


/** @name Memory Protection Method Flags
@{ */

/**
   TLB invalidate as part of a map/unmap operation.
*/
#define TZ_MEMPROT_FLAG_TLBINVALIDATE       0x00000001

/** @} */ /* end_namegroup Memory Protection Method Flags */

/** @} */ /* end_addtogroup tz_macros_constants */

/**
   @weakgroup weak_tz_memprot_lock2_legacy_req_s
@{
*/
typedef struct tz_memprot_lock2_req_s
{
  tz_memprot_chunks_info_t      chunks;    /**< Memory chunk list. */
  uint32                        mem_usage; /**< Memory usage.
                                                See #tz_mem_usage_e_type. */
  uint32                        lock;      /**< 1 (TRUE) to lock (protect);
                                                0 (FALSE) to unlock (unprotect). */
  uint32                        flags;     /**< Flags field for future use;
                                                must be zero when no flags are
                                                set. */
} __attribute__ ((packed)) tz_memprot_lock2_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_memprot_map2_legacy_req_s
@{
*/
typedef struct tz_memprot_map2_req_s
{
  tz_memprot_chunks_info_t       chunks;    /**< Memory chunk list. */
  tz_memprot_map_info_t          map;       /**< Map-to information. */
  uint32                         flags;     /**< Flags field, which is a bitwise
                                                 OR of TZ_MEMPROT_FLAG_* flags.
                                                 Currently supported flags:
                                                 TLBINVALIDATE. \n
                                                 Must be zero when no flags are
                                                 set. */
} __attribute__ ((packed)) tz_memprot_map2_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_memprot_unmap2_legacy_req_s
@{ */
typedef struct tz_memprot_unmap2_req_s
{
  tz_memprot_map_info_t         map;        /**< Mapped-at information. */
  uint32                        flags;      /**< Flags field, which is a bitwise
                                                 OR of TZ_MEMPROT_FLAG_* flags.
                                                 Currently supported flags:
                                                 TLBINVALIDATE. \n
                                                 Must be zero when no flags are
                                                 set. */
} __attribute__ ((packed)) tz_memprot_unmap2_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_tlbinval_req_s
@{ */
typedef struct tz_memprot_tlbinval_req_s
{
  uint32                device_id;    /**< ID of the device that is to have its
                                           TLB invalidated.
                                           See #tz_device_e_type. */
  uint32                ctxt_bank_id; /**< Context bank ID that is to have its
                                           TLB invalidated. These correspond to
                                           context bank numbers and are unique
                                           only for the device. Values start
                                           from 0, which corresponds to CB0. */
  uint32                flags;        /**< Flags field for future use;
                                           must be zero when no flags are set. */
  uint32                spare;        /**< Spare field for future use;
                                           must be zero. */
} __attribute__ ((packed)) tz_memprot_tlbinval_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/** Configure whether xPU violations are to be error fatal. */
typedef enum
{
  TZ_XPU_VIOLATION_ERR_FATAL_ENABLE  = 0, /**< xPU violations are error fatal. */
  TZ_XPU_VIOLATION_ERR_FATAL_DISABLE = 1, /**< xPU violations are not error
                                               fatal. */
  TZ_XPU_VIOLATION_ERR_FATAL_NOOP    = 2, /**< No operation. */
} tz_xpu_violation_err_fatal_t;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_xpu_violation_err_fatal_req_s
@{
*/
typedef struct tz_xpu_violation_err_fatal_req_s
{
  tz_xpu_violation_err_fatal_t       config;     /**< Enable, disable, or NOOP.
                                                      See tz_xpu_violation_err_fatal_t. */
  uint32                             spare;      /**< Unused. */
} __attribute__ ((packed)) tz_xpu_violation_err_fatal_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_xpu_violation_err_fatal_rsp_s
@{
*/
typedef struct tz_xpu_violation_err_fatal_rsp_s
{
  tz_xpu_violation_err_fatal_t status   ;     /**< Enable or disable. */
} __attribute__ ((packed)) tz_xpu_violation_err_fatal_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_memprot_sd_ctrl_req_s
@{ */
typedef struct tz_memprot_sd_ctrl_req_s
{
  uint32                enable;    /**< 1 to enable, 0 to disable */
} __attribute__ ((packed)) tz_memprot_sd_ctrl_req_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * OCMEM commands
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_datatypes
@{ */

/** OCMEM client IDs. */
typedef enum
{
  TZ_OCMEM_CLNT_UNUSED,       /**< Default client when unused. */
  TZ_OCMEM_CLNT_GRAPHICS,     /**< Maps to Oxili Virtual Machine ID (VMID). */
  TZ_OCMEM_CLNT_VIDEO,        /**< Maps to Venus CP_VMID. */
  TZ_OCMEM_CLNT_LP_AUDIO,     /**< Maps to ADSP_VMID. */
  TZ_OCMEM_CLNT_SENSORS,      /**< Maps to ADSP_VMID. */
  TZ_OCMEM_CLNT_BLAST,        /**< Maps to ADSP_VMID. */
  TZ_OCMEM_CLNT_DEBUG,        /**< Maps to HLOS_VMID; for debugging only. */
}tz_ocmem_client_e_type;

/** OCMEM operation mode. */
typedef enum
{
  TZ_OCMEM_OP_MODE_WIDE,      /**< Wide mode. */
  TZ_OCMEM_OP_MODE_THIN,      /**< Thin mode. */
  TZ_OCMEM_OP_MODE_NOT_SET,   /**< Operation mode not set. */
}tz_ocmem_op_mode_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_ocmem_lock_region_req_s
@{
*/
typedef struct tz_ocmem_lock_region_req_s
{
  uint32                        client_id;   /**< OCMEM client requesting the
                                                  memory region. See
                                                  #tz_ocmem_client_e_type. */

  uint32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  uint32                        size;        /**< Size of the requested region. */

  uint32                        mode;        /**< Requested mode (wide or
                                                  narrow). See
                                                  #tz_ocmem_op_mode_e_type. */
}__attribute__ ((packed)) tz_ocmem_lock_region_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ocmem_unlock_region_req_s
@{
*/
typedef struct tz_ocmem_unlock_region_req_s
{
  uint32                        client_id;   /**< OCMEM client that currently
                                                  owns the region. See
                                                  #tz_ocmem_client_e_type. */

  uint32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  uint32                        size;        /**< Size of requested region. */
}__attribute__ ((packed)) tz_ocmem_unlock_region_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ocmem_enable_mem_dump_req_s
@{
*/
typedef struct tz_ocmem_enable_mem_dump_req_s
{
  uint32                        client_id;   /**< OCMEM client that currently
                                                  has this region locked. See
                                                  #tz_ocmem_client_e_type. */

  uint32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  uint32                        size;        /**< Size of the region. */
}__attribute__ ((packed)) tz_ocmem_enable_mem_dump_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ocmem_disable_mem_dump_req_s
@{
*/
typedef struct tz_ocmem_disable_mem_dump_req_s
{
  uint32                        client_id;   /**< OCMEM client that currently
                                                  has this region locked. See
                                                  #tz_ocmem_client_e_type */

  uint32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  uint32                        size;        /**< Size of the region. */
}__attribute__ ((packed)) tz_ocmem_disable_mem_dump_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/** eMMC partition ID for the enterprise security servcie save hash. */
typedef enum
{
    TZ_ES_PARTITION_ID_KERNEL = 0,       /**< Kernel partition ID. */
    TZ_ES_PARTITION_ID_SYSTEM = 1,       /**< System partition ID. */
    TZ_ES_PARTITION_ID_RECOVERY = 2,     /**< Recovery partition ID. */
    TZ_ES_MAX_PARTITIONS                 /**< Maximum number of partitions. */
}tz_partition_id_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_es_save_partition_hash_req_s
@{
*/
typedef struct tz_es_save_partition_hash_req_s
{
  uint32                        partition_id;
                                /**< Partition ID (system, recovery, or kernel).
                                     See #tz_partition_id_e_type. */

  uint8*                        hash;     /**< Hash digest for SHA-256. */
  uint32                        hash_sz;  /**< Size of hash digest. */
}__attribute__ ((packed)) tz_es_save_partition_hash_req_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * NS_FIQ_DEBUG Service Structures
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_macros_constants
@{ */

/** @name Nonsecure Debug FIQ Flags
@{ */

/** Nonsecure debug FIQ is level-triggered. */
#define TZ_NS_DEBUG_FIQ_LEVEL_TRIGGERED              0x0
/** Nonsecure debug FIQ is edge-triggered. */
#define TZ_NS_DEBUG_FIQ_EDGE_TRIGGERED               0x1
/** Nonsecure debug FIQ handler runs in the Thumb state. */
#define TZ_NS_DEBUG_FIQ_THUMB                        0x2

/** @} */ /* end_namegroup Nonsecure Debug FIQ Flags */

/** @} */ /* end_addtogroup tz_macros_constants */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_req_s
@{
*/
typedef struct tz_config_ns_debug_fiq_req_s
{
  const uint32* vector; /**< Base address of the nonsecure FIQ handler. The
                             handler executes in nonsecure SVC mode. */
  const uint32* stack; /**< Stack for the nonsecure FIQ handler. The
                            QSEE software never modifies the stack. */
  uint32* ctx_buf; /**< Context buffer for the nonsecure FIQ handler. Passed
                        in to the handler via register r0. The TrustZone
                        software always modifies the entire context. */
  uint32  ctx_size; /**< Context buffer size in bytes. */
  uint32 intnum; /**< Number of the debug interrupt on the Qualcomm Generic
                      Interrupt Controller (QGIC). The QSEE software only
                      allows using designated interrupts as debug FIQs and
                      returns an error code if the interrupt is not one of
                      the supported ones. */
  uint32 flags; /**< Configuration flags; a bitwise OR of the
                     TZ_NS_FIQ_DEBUG_* definitions. The handler is assumed
                     to execute in ARM mode. For Thumb mode handlers, flag
                     TZ_NS_FIQ_DEBUG_THUMB must be set. */
} __attribute__ ((packed)) tz_config_ns_debug_fiq_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_ctx_size_rsp_s
@{
*/
typedef struct tz_config_ns_debug_fiq_ctx_size_rsp_s
{
  uint32 size; /**< Context size in bytes. */
} __attribute__ ((packed)) tz_config_ns_debug_fiq_ctx_size_rsp_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_int_ok_req_s
@{
*/
typedef struct tz_config_ns_debug_fiq_int_ok_req_s
{
  uint32 intnum; /**< QGIC interrupt number. */
} __attribute__ ((packed)) tz_config_ns_debug_fiq_int_ok_req_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_int_ok_rsp_s
@{
*/
typedef struct tz_config_ns_debug_fiq_int_ok_rsp_s
{
  uint32 verdict; /**< Contains 1 if the interrupt is supported as an FIQ
                       debug interrupt; 0 otherwise. */
} __attribute__ ((packed)) tz_config_ns_debug_fiq_int_ok_rsp_t;
/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * CPU Configuration Service
 * -------------------------------------------------------------------------*/

/**
   @weakgroup weak_tz_cpu_config_req_s
@{
*/
typedef struct tz_cpu_config_req_s
{
  uint32                        id;   /**< ID of the configured item. */
  uint32                        arg0; /**< Generic argument 0. */
  uint32                        arg1; /**< Generic argument 1. */
  uint32                        arg2; /**< Generic argument 2. */
} __attribute__ ((packed)) tz_cpu_config_req_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_cpu_config_query_req_s
@{
*/
typedef struct tz_cpu_config_query_req_s
{
  uint32                        id;   /**< ID of the queried item. */
  uint32                        arg0; /**< Generic argument 0. */
  uint32                        arg1; /**< Generic argument 1. */
  uint32                        arg2; /**< Generic argument 2. */
} __attribute__ ((packed)) tz_cpu_config_query_req_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_cpu_config_query_rsp_s
@{
*/
typedef struct tz_cpu_config_query_rsp_s
{
  uint32                        ret0; /**< Generic return value 0. */
  uint32                        ret1; /**< Generic return value 1. */
  uint32                        ret2; /**< Generic return value 2. */
  uint32                        ret3; /**< Generic return value 3. */
} __attribute__ ((packed)) tz_cpu_config_query_rsp_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_hdcp_write_registers_req_s
@{
*/
typedef struct tz_hdcp_write_registers_req_s
{
  uint32                        addr1;     /**< Address 1 of the register to write. */
  uint32                        value1;    /**< Value 1 to be written. */
  uint32                        addr2;     /**< Address 2 of the register to write. */
  uint32                        value2;    /**< Value 2 to be written. */
  uint32                        addr3;     /**< Address 3 of the register to write. */
  uint32                        value3;    /**< Value 3 to be written. */
  uint32                        addr4;     /**< Address 4 of the register to write. */
  uint32                        value4;    /**< Value 4 to be written. */
  uint32                        addr5;     /**< Address 5 of the register to write. */
  uint32                        value5;    /**< Value 5 to be written. */
} __attribute__ ((packed)) tz_hdcp_write_registers_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_macros_constants
@{ */

/** @name Hyp Key Types
@{ */

#define HYP_ROOT_KEY_ID              0x0
/* #define HYP_MLVM_KEY_ID              0x1 .. Deprecated */

/** @} */ /* end_namegroup Hyp Key Types */

/** @} */ /* end_addtogroup tz_macros_constants */

/**
   @weakgroup weak_tz_get_key_to_hyp_req_s
@{
*/
typedef struct tz_get_key_to_hyp_req_s
{
  uint32_t                      key_id;    /**< Key Identifier. */
  uint8_t*                      key;       /**< Key Data OUT buffer. */
  uint32_t                      key_len;   /**< Key Size. */
}__attribute__ ((packed)) tz_get_key_to_hyp_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak__tz__hyp__to__ta__svc__fwd__req__s
@{
*/
typedef struct tz_hyp_to_ta_svc_fwd_req_s
{
  void*                      in_buf;    	  /**< Input buffer. */
  size_t                     in_buf_len;      /**< Input buffer length. */
  void*                      out_buf;         /**< Output buffer. */
  size_t                     out_buf_len;     /**< Output buffer length. */
}__attribute__ ((packed)) tz_hyp_to_ta_svc_fwd_req_t;

/** @} */ /* end_weakgroup */



/*----------------------------------------------------------------------------
 * TZ Operating System calls
 * -------------------------------------------------------------------------*/
/** @addtogroup tz_datatypes
@{ */

/**
   Structure of the command response from QSEE.
 */
typedef enum
{
  QSEE_APP_ID      = 0xEE01ULL, /**< QSEE response type when the response
                                  data is the application ID. */
  QSEE_LISTENER_ID = 0xEE02ULL, /**< Deprecated: do not reuse. */
  QSEE_APP_STATE   = 0xEE03ULL, /**< QSEE response type when the response
                                  data is the Application state. */
  QSEE_NONE        = 0xEE04ULL,
  QSEE_GENERAL     = 0xEE05ULL, /**< QSEE response type when the response
                                      data is specific to the request */
  QSEE_RESERVED    = 0xFFFFFFFFFFFFFFFF  /* Required to make the enum 8 bytes. */
}tz_os_resp_type;

/**
   QSEE response result values.  Unlisted negative values indicate failure.
 */
typedef enum
{
  QSEE_RESULT_SUCCESS               = 0,

  QSEE_RESULT_INCOMPLETE            = 1,

  QSEE_RESULT_BLOCKED_ON_LISTENER   = 2,

  QSEE_RESULT_CBACK_REQUEST         = 3,

  QSEE_RESULT_RESERVED              = 0xFFFFFFFF  /* Required to make the enum 4 bytes. */

}tz_os_resp_result;

/** @} */ /* end_addtogroup tz_datatypes */


/**
   @weakgroup weak_tz_os_app_start_req_s
@{
*/
typedef struct tz_os_app_start_req_s
{
  size_t                mdt_len;     /**< Combined length of the ELF
                                          and all program headers. */
  size_t                img_len;     /**< Combined length of all the
                                          loadable segments. */
  uintptr_t             pa;          /**< Points to the region
                                          where the application and
                                          the headers are located, as
                                          described in Elf32_Ehdr and
                                          Elf32_Phdr. */
} __attribute__ ((packed)) tz_os_app_start_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_app_start_embedded_req_s
@{
*/
typedef struct tz_os_app_start_embedded_req_s
{
  char  *app_name;
  size_t app_name_len;
} __attribute__ ((packed)) tz_os_app_start_embedded_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_rsp_s
@{
*/
typedef struct tz_os_rsp_s
{
  tz_os_resp_result     result;       /**< Status result of the call. */
  tz_os_resp_type       resp_type;    /**< Type of response. */
  uint32                data;         /**< Data from the call, if applicable. */
} __attribute__ ((packed)) tz_os_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_app_shutdown_req_s
@{
*/
typedef struct tz_os_app_shutdown_req_s
{
  uint32_t               app_id;      /**< Application shutdown-specific
                                           request. */
} __attribute__ ((packed)) tz_os_app_shutdown_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_app_lookup_req_s
@{
*/
typedef struct tz_os_app_lookup_req_s
{
  char*                  app_name;    /**< Application
                                           lookup-specific
                                           request. */
  size_t                 len;         /**< Length of the application name in bytes */
} __attribute__ ((packed)) tz_os_app_lookup_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_app_get_state_req_s
@{
*/
typedef struct tz_os_app_get_state_req_s
{
  uint32_t               app_id;      /**< Secure application ID. */
} __attribute__ ((packed)) tz_os_app_get_state_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_app_region_notification_req_s
@{
*/
typedef struct tz_os_app_region_notification_req_s
{
  uintptr_t              applications_region_addr;
                                      /**< Application region address. */
  size_t                 applications_region_size;
                                      /**< Application region size in bytes. */
} __attribute__ ((packed)) tz_os_app_region_notification_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_register_log_buffer_req_s
@{
*/
typedef struct tz_os_register_log_buffer_req_s
{
  void*                  phys_addr;  /**< Log buffer physical address. */
  size_t                 len;        /**< Size of the log buffer in bytes. */
} __attribute__ ((packed)) tz_os_register_log_buffer_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */
   
/** enumerates log id's for requesting encrypted logs*/
typedef enum 
{
  TZBSP_DIAG_LOG_ENCR_ID        = 0,
  TZBSP_QSEE_LOG_ENCR_ID        = 1,
  TZBSP_UNSUPPORTED_LOG_ENCR_ID = 2,
}tzbsp_log_encr_id;

/* Error codes for log_encryption request */
typedef enum
{
  TZBSP_LOG_ENCR_SUCCESS                      = 0,  /**< Encrypted logs copied to hlos buffer successfully */
  TZBSP_LOG_ENCR_FAILURE                      = 1,  /**< Failed to Encrypt the requested logs */
  TZBSP_LOG_ENCR_INVAL_LOG_ID                 = 2,  /**< Log_id for requesting encrypted logs is invalid */
  TZBSP_LOG_ENCR_DISABLED                     = 3,  /**< Log encryption feature is disabled */
  TZBSP_LOG_ENCR_KEY_GEN_FAILED               = 4,  /**< Key generation for log encryption request failed */
  TZBSP_LOG_ENCR_BUFF_SIZE_INVALID            = 5,  /**< Buffer size of the buffer to copy encrypted logs to is not valid */
  TZBSP_LOG_ENCR_DATA_LEN_INVAL               = 6,  /**< Length of the data for encryption is not valid */
  TZBSP_LOG_ENCR_QSEE_LOG_BUFF_NOT_REGISTERED = 7,  /**< QSEE log buffer is not registered */
  TZBSP_LOG_ENCR_UCLIB_ERR_OUT_OF_MEMORY      = 8,  /**< Failed to create crypto context */ 
  TZBSP_LOG_ENCR_CIPHER_INIT_FAILED           = 9, /**< Failed to initialize crypto handle */
  TZBSP_LOG_ENCR_CIPHER_SET_PARAM_FAILED      = 10, /**< Failed to set crypto parameters */
  TZBSP_LOG_ENCR_CIPHER_UPDATE_FAILED         = 11, /**< Crypto cipher update operation failed */
  TZBSP_LOG_ENCR_CIPHER_FINAL_FAILED          = 12, /**< Crypto cipher final operation failed */ 
}TzbspLogEncrErrorCode;

/** @} */ /* end_addtogroup tz_datatypes */
/**
   @weakgroup weak_tz_os_request_encr_log_buffer_req_s
@{
*/
typedef struct tz_os_request_encr_log_buffer_req_s
{
  void*                  phys_addr;  /**< Physical address of the log buffer to copy encr logs. */
  size_t                 len;        /**< Size of the log buffer in bytes. */
  tzbsp_log_encr_id               log_id;     /**< 0: encryption request for Diag log
                                          1: encryption request for TA logs */
} __attribute__ ((packed)) tz_os_request_encr_log_buffer_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_load_services_image_req_s
@{
*/
typedef struct tz_os_load_services_image_req_s
{
  size_t                mdt_len;     /**< Combined length of the ELF
                                          and all program headers. */
  size_t                img_len;     /**< Combined length of all the
                                          loadable segments. */
  uintptr_t             pa;          /**< Points to the region
                                          where the application and
                                          the headers are located, as
                                          described in Elf32_Ehdr and
                                          Elf32_Phdr. */
} __attribute__ ((packed)) tz_os_load_services_image_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_register_listener_req_s
@{
*/
typedef struct tz_os_register_listener_req_s
{
  uint32_t              listener_id; /**< ID of the listener service. */
  void*                 sb_ptr;      /**< Pointer to the shared buffer. */
  size_t                sb_len;      /**< Length of the shared buffer. */
} __attribute__ ((packed)) tz_os_register_listener_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_deregister_listener_req_s
@{
*/
typedef struct tz_os_register_delistener_req_s
{
  uint32_t              listener_id; /**< ID of the listener service. */
} __attribute__ ((packed)) tz_os_deregister_listener_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_listener_response_handler_req_s
@{
*/
typedef struct tz_os_listener_response_handler_req_s
{
  uint32_t              listener_id; /**< ID of the listener service. */
  int32_t               status;      /**< Status of the listener service. */
} __attribute__ ((packed)) tz_os_listener_response_handler_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_listener_response_handler_wlist_req_s
@{
*/
typedef struct tz_os_listener_response_wlist_handler_req_s
{
  uint32_t              listener_id; /**< ID of the listener service. */
  int32_t               status;      /**< Status of the listener service. */
  const void*           qwl;         /**< Validation metadata. */
  size_t                qwl_len;     /**< Size of metadata in bytes. */
} __attribute__ ((packed)) tz_os_listener_response_wlist_handler_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_continue_blocked_request_req_s
@{
*/
typedef struct tz_os_continue_blocked_request_req_s
{
  uint32_t               app_id;      /**< ID of the application to
                                           continue. */
} __attribute__ ((packed)) tz_os_continue_blocked_request_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_continue_blocked_request_smci_req_s
@{
*/
typedef struct tz_os_continue_blocked_request_smci_req_s
{
  uint32_t               session_id;  /**< ID of the session to
                                           continue. */
} __attribute__ ((packed)) tz_os_continue_blocked_request_smci_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_load_external_image_req_s
@{
*/
typedef struct tz_os_load_external_image_req_s
{
  size_t                mdt_len;     /**< Combined length of the ELF
                                          and all program headers. */
  size_t                img_len;     /**< Combined length of all the
                                          loadable segments. */
  uintptr_t             pa;          /**< Points to the region
                                          where the application and
                                          the headers are located, as
                                          described in Elf32_Ehdr and
                                          Elf32_Phdr. */
} __attribute__ ((packed)) tz_os_load_external_image_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_rpmb_provision_key_req_s
@{
*/
typedef struct tz_os_rpmb_provision_key_req_s
{
  size_t                key_type;    /**< Key type to provision. */
} __attribute__ ((packed)) tz_os_rpmb_provision_key_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_os_ks_req_s
@{
*/
typedef struct tz_os_ks_req_s
{
  uintptr_t*            req;        /**< Keystore request buffer. */
  size_t                req_len;    /**< Keystore request length in bytes. */
} __attribute__ ((packed)) tz_os_ks_req_s;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_ks_get_max_keys_req_s
@{
*/
typedef struct tz_os_ks_get_max_keys_req_s
{
  size_t                flags;       /**< Optional flags. */
} __attribute__ ((packed)) tz_os_ks_get_max_keys_req_s;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_smc_invoke_req_s
@{
*/
typedef struct tz_os_smc_invoke_req_s
{
  void*                 inBuf;      /**< Serialized MINK buffer from caller. */
  size_t                inBuf_len;  /**< Size of serialized buffer. */
  void*                 outBuf;     /**< Serialized MINK buffer with optional
                                         invocation request to the caller. */
  size_t                outBuf_len; /**< Size of serialized buffer. */
} __attribute__ ((packed)) tz_os_smc_invoke_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_smc_invoke_cback_req_s
@{
*/
typedef struct tz_os_smc_invoke_cback_req_s
{
  void*                 outBuf;     /**< Serialized MINK buffer with callback
                                         invocation response. */
  size_t                outBuf_len; /**< Size of serialized buffer. */
} __attribute__ ((packed)) tz_os_smc_invoke_cback_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_os_smc_invoke_rsp_s
@{
*/
typedef struct tz_os_smc_invoke_rsp_s
{
  int32_t               result;       /**< Status result of the call
                                           (SMC Invoke completion or listener
                                            callback invocation)*/
  uint32_t              resp_data;    /**< SMC Invoke result or app_id for
                                           listener invocation */
  uint32_t              data;         /**< Listener ID for listener callback
                                           invocations */
} __attribute__ ((packed)) tz_os_smc_invoke_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_app_send_data_req_s
@{
*/
typedef struct tz_app_send_data_req_s
{
  uint32_t              app_id;       /**< App ID of requested secure application */
  void*                 req;          /**< Request buffer. */
  size_t                req_len;      /**< Size of request in bytes. */
  void*                 rsp;          /**< Response buffer. */
  size_t                rsp_len;      /**< Size of response in bytes. */
} __attribute__ ((packed)) tz_app_send_data_req_t;

/**
   Format of scatter lists when sent to a TA that is implemented in
   aa64.
 */
typedef struct ScatterListEntryAA64 {
  uint64_t addr;
  uint32_t len;
} __attribute__ ((packed)) ScatterListEntryAA64;

/**
   Format of scatter lists when sent to a TA that is implemented in
   aa32.
 */
typedef struct ScatterListEntryAA32 {
  uint32_t addr;
  uint32_t len;
} __attribute__ ((packed)) ScatterListEntryAA32;

/**
  If this flag is set, the offset locates a single physical address
  inside the request buffer, and `sizeOrCount` is the size of the
  memory being shared at that physical address.

  Otherwise, the offset locates an array of {start, len} pairs (a
  "scatter/gather list"), and `sizeOrCount` gives the number of
  entries in that array.
*/

#define QWL_FLAG_CONTIGUOUS   (UINT32_C(1) << 31)

/**
   When this flag is set, physical addresses and scatter gather entry
   sizes are 64-bit values.  Otherwise, they are 32-bit values.
*/
#define QWL_FLAG_64           (UINT32_C(1) << 30)

/**
   These bits of `offsetAndFlags` hold an offset into the request
   buffer.
*/
#define QWL_OFFSET_MASK        ((UINT32_C(1) << 30) - 1)


/**
   Structure passed by qseecom that describes the location of scatter
   gather lists embedded in the request portion of the
   request/response buffer sent to TAs.  The scatter lists are copied
   out of the request/response buffer so they may be used for
   validation of TA requests to share memory.  Qseecom may send an
   array of 0...10 QWLEntry structures.
 */
typedef struct QWLEntry {
  /** Bits 0..29 are the offset from the beginning of the request
      buffer to the beginning of the first item in the scatter list.
      Bit 30 indicates whether the list is of type
      ScatterListEntryAA64 or ScatterListEntryAA32.  Bit 31 indicates
      whether the list is only one entry long. */
  uint32_t offsetAndFlags;

  /** If bit 31 is set in offsetAndFlags, this element provides the
      length for the address provided in the single entry scatter
      list.  If bit 31 is not set, this provides the number of entires
      in the scatter list at the offset described in
      offsetAndFlags. */
  uint32_t sizeOrCount;
} __attribute__ ((packed)) QWLEntry;

/**
   @weakgroup weak_tz_app_send_data_wlist_req_s
@{
*/
typedef struct tz_app_send_data_wlist_req_s
{
  uint32_t              app_id;       /**< App ID of requested secure
                                           application. */
  void*                 req;          /**< Request buffer. */
  size_t                req_len;      /**< Size of request in bytes. */
  void*                 rsp;          /**< Response buffer. */
  size_t                rsp_len;      /**< Size of response in bytes. */
  const void*           qwl;          /**< Validation metadata. */
  size_t                qwl_len;      /**< Size of metadata in bytes. */
} __attribute__ ((packed)) tz_app_send_data_wlist_req_t;


typedef enum
{
   TZ_CIPHER_MODE_XTS_128 = 0,
   TZ_CIPHER_MODE_CBC_128 = 1,
   TZ_CIPHER_MODE_XTS_256 = 3,
   TZ_CIPHER_MODE_CBC_256 = 4
} tz_ice_capability_index_type;

typedef enum
{
    TZ_ICE_DATA_UNIT_SIZE_INVALID = 0x00,
    TZ_ICE_DATA_UNIT_SIZE_512   = 0x01,
    TZ_ICE_DATA_UNIT_SIZE_1024  = 0x02,
    TZ_ICE_DATA_UNIT_SIZE_2048  = 0x04,
    TZ_ICE_DATA_UNIT_SIZE_4096  = 0x08,
    TZ_ICE_DATA_UNIT_SIZE_8192  = 0x10,
    TZ_ICE_DATA_UNIT_SIZE_16384 = 0x20,
    TZ_ICE_DATA_UNIT_SIZE_32768 = 0x40,
    TZ_ICE_DATA_UNIT_SIZE_65536 = 0x80,
} tz_ice_data_unit_type;

typedef struct tz_set_config_ice_key_req_s
{
  uint32            pipe;              /* ICE key LUT index */
  uint64            key_ptr_base;      /* Physical address of the key buffer to be written (In XTS it will hold [data_key | salt_key]*/
  uint32            key_ptr_len;       /* The size of the key ptr buffer (in XTS it will hold key_data_size + salt_data_size) */
  uint32            capability_index;  /* The cipher mode and key size. See #tz_ice_capability_index_type. */
  uint32            data_unit_size;    /* Data unit size. See #tz_ice_data_unit_mask_type*/
} __attribute__ ((packed)) tz_set_config_ice_key_req_t;

typedef struct hyp_image_manager_s
{
  void*             smcParams;        /* Pointer to SMC_PARAMETERS struct that contains information for the selected action */
  uint32            action;           /* HYP_IMG_VERSION=0x1,HYP_IMG_AUTHENTICATE=0x2,HYP_IMG_MAP_MEMORY=0x3,HYP_IMG_LAUNCH=0x4,HYP_IMG_CANCEL=0x5 */
} __attribute__ ((packed)) hyp_image_manager_t;

typedef struct hyp_mem_lock_ns_region_s
{
  uint64 start;
  uint64 end;
  hyp_memprot_dstVM_perm_info_t *destVMlist;
  uint32  dstVMlistsize;
} __attribute__((packed))hyp_mem_lock_ns_region_t;

typedef enum
{
	TZ_STORAGE_TYPE_NULL,
    TZ_STORAGE_TYPE_SPINOR,
	TZ_STORAGE_TYPE_MAX
   
} tz_storage_type;

typedef enum
{
    TZ_STORAGE_INIT = 0x00,
	TZ_STORAGE_READ = 0x01,
	TZ_STORAGE_WRITE = 0x02,
	TZ_STORAGE_ERASE = 0x03,
	TZ_STORAGE_GET_INFO =0x04,
	TZ_STORAGE_DEINIT =0x05,
	TZ_STORAGE_CMD_MAX =0x06,
   
} tz_storage_cmd;

/**
   Storage response result values.  Unlisted negative values indicate failure.
 */
typedef enum
{
	STORAGE_CMD_RESULT_SUCCESS           = 0,

	STORAGE_CMD_RESULT_FAIL              = 1,

	STORAGE_CMD_RESULT_UNSUPPORTED       = 2,

	STORAGE_CMD_RESULT_INVALID_PARAMETER = 3,

	STORAGE_CMD_RESULT_DEVICE_NOT_FOUND  = 4,

	STORAGE_CMD_RESULT_RESERVED              = 0xFFFFFFFF  /* Required to make the enum 4 bytes. */

}tz_storage_resp_result;

typedef struct 
{
	uint32  Data1;
	uint16  Data2;
	uint16  Data3;
	uint8   Data4[8];
} STORMGR_GUID;

typedef struct tz_storage_info
{
  uint64 total_blocks; /* Total number of blocks */
  uint32 block_size; /* Size of a block in bytes. */
  uint32 page_size; /* Size of a page/sector in bytes. */
  uint32 num_physical; /* Number of physical partitions. */
  uint64 manufacturer_id; /* ID of the manufacturer. */
  uint64 serial_num; /* Serial number of the device. */
  char   fw_version[32]; /* String containing the firmware version, null terminated. */
  char   memory_type[5]; /* String containing the Memory type: NOR, UFS, eMMC, NAND. */
  char   product_name[32];
}__attribute__ ((packed)) tz_storage_info;

/**
  SPINOR Sycall Command Structure
  */
typedef struct tz_stor_cmd
{
	uint64            storage_type;       /* Type of Storage Device */
	uint64            slot_num;           /* Hardware slot number */
	uint64            LUN;                /* Storage LUN */
	uint64            GUID_ptr;           /* Partition GUID Pointer  */
	uint64            storage_cmd;        /* Storage Command */  
}__attribute__ ((packed)) tz_stor_cmd;

typedef struct tz_stor_cmd_details
{
	uint64            LBA;              /* Start address for the command */  
	uint64            length;                  /* Size of the storage transaction. */   
	uint64            data_buffer_ptr;         /* Pointer to the buffer */
	uint64            data_buffer_size;
 }tz_stor_cmd_details;   
//}__attribute__ ((packed)) tz_stor_cmd_details;

/**
  SPINOR Sycall response Structure
  */
typedef struct tz_stor_resp
{
	uint64            storage_cmd;           /* Storage Command */    
	uint64            data_buffer_ptr;       /* Pointer of the reda/write/getinfo buffer */
	uint64            data_buffer_size;       /* size of the buffer */ 
}__attribute__ ((packed)) tz_stor_resp;

/**
   @weakgroup weak_tz_send_storage_cmd_req_s
@{
*/

typedef struct tz_send_storage_cmd_req_s
{
   tz_stor_cmd                  *cmd_metadata;    /*Pointer to the tz_stor_cmd struct */
   uint64                       cmd_metadata_size;
   tz_stor_cmd_details 		    *cmd_details;     /*Pointer to the command details struct */
   uint64                       cmd_details_size; /*size of the command details struct */
} __attribute__ ((packed)) tz_send_storage_cmd_req_t;


/**
   @weakgroup weak_tz_send_storage_cmd_rsp_s
@{
*/
typedef struct tz_send_storage_cmd_rsp_s
{
  tz_storage_resp_result             result;               /**< Status result of the operation performed by the command. */
  tz_stor_resp                       *response_buffer;     /**Pointer to the tz_stor_resp structure **/
  uint64                             response_buffer_size; /** length of the tz_stor_resp structure  **/ 
} __attribute__ ((packed)) tz_send_storage_cmd_rsp_t;


/** @} */ /* end_weakgroup */


/* SDC ICE configuration input request  */

typedef struct ice_set_key_config_context_req_s
{
    uint32  index;
    uint8   key_len;
    uint8   alg_mode;
    uint8   key_mode;
} __attribute__ ((packed)) ice_set_key_config_context_req_t;

#endif
