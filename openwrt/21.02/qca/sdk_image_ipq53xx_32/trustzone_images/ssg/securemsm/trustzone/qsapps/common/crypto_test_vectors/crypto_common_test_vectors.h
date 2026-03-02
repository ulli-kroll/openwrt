/*
    @file crypto_common_test_vectors.h
    @brief This file contains all test vectors used in various crypto test apps
*/
/*===========================================================================
  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

    EDIT HISTORY FOR FILE
    $Header: //components/rel/ssg.tzw/1.2/securemsm/trustzone/qsapps/common/crypto_test_vectors/crypto_common_test_vectors.h#1 $
    $DateTime: 2020/11/02 23:12:26 $
    $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/
#ifndef __CRYPTO_COMMON_TEST_VECTORS_H__
#define __CRYPTO_COMMON_TEST_VECTORS_H__

#include <stdint.h>

#define UINT8_A uint8_t __attribute__ ((aligned(128)))

UINT8_A aes_large_plain_text_1[1024];
UINT8_A aes_large_cipher_text_1[1024];
UINT8_A aes_large_plain_text2[32*1024];
UINT8_A aes_large_cipher_text2[33*1024];
UINT8_A aes_large_cipher_text3[33*1024];
UINT8_A aes_performance[128*1024];
UINT8_A sha_large_hash_text[40*1024];

#endif // __CRYPTO_COMMON_TEST_VECTORS_H__
