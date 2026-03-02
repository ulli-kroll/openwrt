/**
 * Copyright (c) 2017-2018,2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __SAMPLEAPP_H
#define __SAMPLEAPP_H

/* commands supported from sample client */
#define CLIENT_CMD0_GET_VERSION                               0
#define CLIENT_CMD1_BASIC_DATA                                1
#define CLIENT_CMD2_REGISTER_SB_PTR                           2
#define CLIENT_CMD3_RUN_CRYTPO_TEST                           3
#define CLIENT_CMD4_RUN_CRYTPO_PERF                           4
#define CLIENT_CMD5_RUN_SFS_TEST                              5
#define CLIENT_CMD6_RUN_FS_TEST                               6
#define CLIENT_CMD7_RUN_RSA_TEST                              7
#define CLIENT_CMD8_RUN_RSA_PERF_TEST                         8
#define CLIENT_CMD9_RUN_CMNLIB_TEST                           9
#define CLIENT_CMD10_RUN_CORE_TEST                            10
#define CLIENT_CMD11_RUN_SECURECHANNEL_TEST                   11
#define CLIENT_CMD12_RUN_MESSAGE_PASSING                      12
#define CLIENT_CMD13_RUN_MISC_TEST                            13
// 14 and 15 are used for RPMB operations in the sample client.
#define RESERVED_FOR_RPMB_OPS_0                               14
#define RESERVED_FOR_RPMB_OPS_1                               15
#define CLIENT_CMD16_RUN_BUSES_TEST                           16
#define CLIENT_CMD17_RUN_STOR_TEST                            17
#define CLIENT_CMD18_RUN_FUSE_TEST                            18
#define CLIENT_CMD19_RUN_BUSES_SPI_TEST                       19
#define CLIENT_CMD22_RUN_SECURE_UI_DISPLAY_TEST               22
#define CLIENT_CMD23_RUN_SECURE_UI_TOUCH_TEST                 23
#define CLIENT_CMD25_RUN_SAFE_TEST                            25
#define CLIENT_CMD26_RUN_ABORT_TEST                           26
#define CLIENT_CMD27_RUN_SMPLSERV_TEST                        27
#define CLIENT_CMD29_RUN_BIO_LIB_TEST                         29
#define CLIENT_CMD30_RUN_BULLETIN_BOARD_TEST                  30
#define CLIENT_CMD31_RUN_TLMM_TEST                            31
#define CLIENT_CMD32_RUN_ABORT_TEST                           32
#define CLIENT_CMD33_RUN_SMPLCERT_TEST                        33
#define CLIENT_CMD34_RUN_TRANS_TEST                           34
#define CLIENT_CMD35_RUN_OEM_BUF_TEST                         35
#define CLIENT_CMD36_RUN_INTLOCK_TEST                         36
#define CLIENT_CMD37_REG_SHARED_BUFS                          37
#define CLIENT_CMD38_ACCESS_REGION                            38
//#define CLIENT_CMD39_RUN_SANDBOX_TEST                         39
#define CLIENT_CMD39_RSA_SIGN_TEST                            39
#define CLIENT_CMD40_AES_ENC_TEST                             40
#define CLIENT_CMD41_AES_DEC_TEST                             41
#define CLIENT_CMD42_RSA_ENC_DEC_TEST                         42
#define CLIENT_CMD43_RUN_FUSE_WRITE_TEST                      43

#define CLIENT_CMD44_RUN_TA_HEAP_DEBUGGING_TEST               44
#define CLIENT_CMD45_RUN_SANDBOX_TEST                         45

#define CLIENT_CMD50_RUN_LISTENER_FRAMEWORK_TEST              50
#define CLIENT_CMD51_RUN_VFP_TEST                             51
#define CLIENT_CMD52_SPIN_TEST                                52
#define CLIENT_CMD53_RUN_LOG_BITMASK_TEST                     53
#define CLIENT_CMD55_RUN_SPCOM_TEST                           55
#define CLIENT_CMD56_RUN_SP_KEYMASTER_TEST                    56

#define CLIENT_CMD58_RUN_SP_IRIS_TEST                         58

#define CLIENT_CMD60_RUN_HDCP_KEY_WRITE_TEST                  60

#define CLIENT_CMD99_RUN_SFS_PERF_TEST                        99
#define CLIENT_CMD100_RUN_CACHE_TEST                         100
#define CLIENT_CMD101_RUN_SHAREDBUFFER_TEST                  101
#define CLIENT_CMD102_RUN_SHAREDBUFFER_RO_TEST               102
#define CLIENT_CMD103_RUN_WLIST_TEST                         103
#define CLIENT_CMD104_RUN_WLIST_LISTENER_TEST                104
#define CLIENT_CMD108_RUN_NS_SYSREG_TEST                     108
#define CLIENT_CMD109_RUN_EL2_RTIC_REPORT_TEST               109
#define CLIENT_CMD112_RUN_DEBUGPOLICY_TEST                   112
#define CLIENT_CMD113_RUN_DEBUGPOLICY_TEST_SANITY            113
#define CLIENT_CMD_114_RUN_SFS_MAX_FILES_TEST                114
#define CLIENT_CMD115_RUN_PERSISTENT_OBJ_DEP_TESTS           115
#define CLIENT_CMD116_RUN_PERSISTENT_OBJ_FUNC_TESTS          116
#define CLIENT_CMD117_RUN_PERSISTENT_OBJ_PERF_TESTS          117
#define CLIENT_CMD118_RUN_FIPS_SERVICES_TESTS                118
#define CLIENT_CMD119_RUN_OEM_BUF_CHIPSET_TEST               119
#define CLIENT_CMD120_RUN_DYNAMIC_GP_LISTENER_SIZE_TEST      120
#define CLIENT_CMD121_RUN_KEY_DERIVE_TEST                    121
#define CLIENT_CMD122_RUN_KEY_CLEAR                          122

#define CLIENT_CMD999_RUN_SMPLSERV_CRASH_TEST                999


/* Error code: status sent as response to command from sample client*/

#define SUCCESS                                (0)
#define FAILED                                (-1)
#define FAIL_REGISTER_SB                      (-2)
#define FAIL_DEREGISTER_SB                    (-3)
#define FAIL_PREP_SB_NON_SECURE_READ          (-4)
#define FAIL_PREP_SB_SECURE_READ              (-5)
#define FAIL_CMD_BUF_TOO_SMALL_FOR_CMD_ID     (-6)
#define FAIL_CMD_RSP_BUFS_TOO_SMALL           (-7)
#define FAIL_SHARED_BUFF_TOO_SMALL            (-8)
#define FAIL_UNDEFINED_COMMAND                (-9)
#define FAIL_DEVICE_NOT_PROVISIONED_ERROR    (-10)

#endif //__SAMPLEAPP_H
