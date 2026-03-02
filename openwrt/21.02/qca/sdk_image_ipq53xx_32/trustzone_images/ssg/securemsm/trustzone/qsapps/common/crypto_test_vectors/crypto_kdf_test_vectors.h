/*
@file crypto_kdf_test_vectors.h
@brief This file contains test vectors for kdf.
*/
/*===========================================================================
   Copyright (c) 2018,2023 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tzw/1.2/securemsm/trustzone/qsapps/common/crypto_test_vectors/crypto_kdf_test_vectors.h#2 $
  $DateTime: 2023/03/10 06:48:43 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
03/09/2020   abs     commented KDF NULL keys for crypto test in ipq53xx
11/16/2018   jp      added kdf test vectors
===========================================================================*/
#ifndef __CRYPTO_KDF_TEST_VECTORS_H__
#define __CRYPTO_KDF_TEST_VECTORS_H__

#include <com_dtypes.h>

/* kdf test vectors */
#define MAX_TEST_OUTPUT_KEY_LEN         32

static UINT8_A key_label[] = {"I am deriving a key"};
static UINT8_A key_context[] = {"To verify the kdf working"};
static UINT8_A key_input[] = {0x12,0x23,0x56,0x73,
                              0xA3,0xff,0xEf,0xAB,
                              0xCD,0x1F,0x56,0xEF,
                              0xA3,0xff,0xEf,0xAB,
                              0xCD,0x1F,0x56,0xEF,
                              0x12,0x23,0x56,0x73,
                              0xCD,0x1F,0x56,0xEF,
                              0xCD,0x1F,0x56,0xEF,
                              0x12,0x23,0x56,0x73,
                              0xCD,0x1F,0x56,0xEF,
                              0xCD,0x1F,0x56,0xEF,
                              0xA3,0xff,0xEf,0xAB};

typedef struct{
  uint8_t*           input_key;
  uint32_t           input_key_len;
  uint8_t*           label;
  uint32_t           label_len;
  uint8_t*           context;
  uint32_t           context_len;
  bool               is_err_expected;
} __attribute__ ((packed)) kdf_vector_t;

kdf_vector_t kdf_test_vectors[] = {
#ifndef CHIPSET_53xx // FIXME_53xx 
  {NULL, 16, key_label, sizeof(key_label), key_context, sizeof(key_context), false},
  {NULL, 32, key_label, sizeof(key_label), key_context, sizeof(key_context), false},
#endif
  {key_input, 16, key_label, sizeof(key_label), key_context, sizeof(key_context), false},
  {key_input, 32, key_label, sizeof(key_label), key_context, sizeof(key_context), false},
  {key_input, 16, key_label, 0, key_context, sizeof(key_context), false},
  {key_input, 32, key_label, 0, key_context, sizeof(key_context), false},
  {key_input, 16, NULL, 0, key_context, sizeof(key_context), false},
  {key_input, 32, NULL, 0, key_context, sizeof(key_context), false},
  {key_input, 16, NULL, sizeof(key_label), key_context, sizeof(key_context), true},
  {key_input, 32, NULL, sizeof(key_label), key_context, sizeof(key_context), true},
  {key_input, 16, key_label, sizeof(key_label), key_context, 0, false},
  {key_input, 32, key_label, sizeof(key_label), key_context, 0, false},
  {key_input, 16, key_label, sizeof(key_label), NULL, 0, false},
  {key_input, 32, key_label, sizeof(key_label), NULL, 0, false},
  {key_input, 16, key_label, sizeof(key_label), NULL, sizeof(key_context), true},
  {key_input, 32, key_label, sizeof(key_label), NULL, sizeof(key_context), true},
};

#endif // __CRYPTO_KDF_TEST_VECTORS_H__
