#ifndef SHA_SHARED_H
#define SHA_SHARED_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tzw/1.2/securemsm/uclib/usr/legacy/unifiedcrypto/shared/sha/inc/sha_shared.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
10/06/16   shl    Added arm v8 crypto support
09/15/16   shl    fixed deinit problem
05/12/16   yk     add hmac init/update/final support
04/05/16   jd     add md5 support
07/16/10   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uc_comdef.h"
#include "sha_core.h"

/**
 * SHA-2 Context definition
 */


typedef enum
{
  SW_AUTH_ALG_NULL               = 0x1,
  SW_AUTH_ALG_SHA1               = 0x2,
  SW_AUTH_ALG_SHA256,
  SW_AUTH_ALG_SHA224,
  SW_AUTH_ALG_SHA384,
  SW_AUTH_ALG_SHA512,
  SW_AUTH_ALG_MD5,
  SW_AUTH_ALG_SM3,
  SW_AUTH_ALG_MAX,
  SW_AUTH_ALG_INVALID            = 0x7FFFFFFF
} SW_Auth_Alg_Type;

typedef enum
{
  SW_HASH_PARAM_MODE              = 0x01,
  SW_HASH_PARAM_IV                = 0x02,
  SW_HASH_PARAM_HMAC_KEY          = 0x03,
  SW_HASH_PARAM_AUTH_KEY          = 0x04,
  SW_HASH_PARAM_COPY              = 0x05,
  SW_HASH_PARAM_HMAC_COPY         = 0x06,
  SW_HASH_PARAM_MAX               = 0x7FFFFFFF
} SW_Auth_Param_Type;

typedef struct
{
  uint32             counter[2];
  uint32             iv[16];
  uint32             auth_iv[8];
  uint8              leftover[SW_SHA_BLOCK_SIZE];
  uint32             leftover_size;
  SW_Auth_Alg_Type   auth_alg;
  uint8              firstBlock;
  uint8              lastBlock;
  uint32             opad[SW_SHA512_DIGEST_SIZE/4];
  uint32             hmac_key[32];
  SHA512_CTX         sha512_384ctx;  //SHA Context for SHA-512/384
  void              *ctx_arm;        //arm v8 support
} SW_SHA_Ctx;

// this structure is added to solve issues of the existing implementation
// such as free the same pointer twice, corrupt the data in some calling sequence.
typedef struct
{
  uint32        alg;   // for reset purpose
  SW_SHA_Ctx   *ctx;
} HashHandleType;

#ifndef _DEFINED_CRYPTOCNTXHANDLE
#define _DEFINED_CRYPTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

#ifndef IOVECDEF
typedef struct
{
   void         *pvBase;
   uint32_t     dwLen;
} IovecType;

typedef  struct
{
  IovecType     *iov;
  uint32_t      size;
} IovecListType;

#define IOVECDEF
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
 * @brief  This function will create a message digest hash using the
 *         algorithm specified.
 *
 * @param[in] hash         The hash algorithm
 * @param[in] msg          The message to hash
 * @param[in] msg_len      The length of the message
 * @param[in,out] digest   The digest to store
 * @param[in] digest_len   Length of the output message digest hash
 *                         buffer in bytes.
 *
 * @return 0 on success, negative on failure
 */
sw_crypto_errno_enum_type SW_Hash(SW_Auth_Alg_Type auth_alg, const uint8_t *msg, uint32_t msg_len, uint8_t *digest, uint32_t digest_len);


/**
 * @brief This functions initiazlize the hash context
 *
 * @param handle     [in]  Pointer to pointer to hash context
 * @param auth_alg  [ in]  Hash algorithm
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_Hash_Init(CryptoCntxHandle **handle, SW_Auth_Alg_Type auth_alg);


/**
 * @brief Resets the hash context initialized by SW_Hash_Init
 *
 * @param handle     [in]  Pointer to hash context
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_Hash_Reset(CryptoCntxHandle *handle);

/**
 * @brief This function will hash data into the hash context
 *        structure, which must have been initialized by
 *        SW_Hash_Update().
 *
 * @param handle      [in] Pointer to Hash context
 * @param ioVecIn     [in] Input vector

 *
 * @return sw_crypto_errno_enum_type
 *
 * @see SW_CRYPTO_Hash_Init
 *
 */
sw_crypto_errno_enum_type SW_Hash_Update(CryptoCntxHandle *handle,IovecListType ioVecIn);

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_Hash_Final (CryptoCntxHandle *handle,IovecListType *ioVecOut);


/**
 * @brief Deinitialization for sha1/sha256
 *
 * @param  void
 *
 * @return void
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_Hash_Deinit(CryptoCntxHandle **handle);

/**
 * @brief  This function will hash some data into the hash context
 *         structure, which Must have been initialized by
 *         SW_Hash_Init(...).
 *
 * @param handle[in]    The hash context
 * @param[in]             Pointer to the msg to hash
 * @param[in]             Length of the msg to hash
 *
 * @return 0 on success, negative on failure
 *
 * @see SW_Hash_Init
 */
sw_crypto_errno_enum_type SW_Hash_Update_Ez(CryptoCntxHandle* handle, const uint8_t *msg, uint32_t msg_len);

/**
 * @brief  Compute the digest hash value
 *
 * @param handle[in]     The hash context
 * @param digest[in]       Pointer to output message digest hash
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes.
 *
 * @return 0 on success, negative on failure
 *
 * @see SW_Hash_Init
 */
sw_crypto_errno_enum_type SW_Hash_Final_Ez(CryptoCntxHandle* handle, uint8_t *digest, uint32_t digest_len);

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_Hmac (uint8 * key_ptr, uint32 keylen, IovecListType ioVecIn, IovecListType * ioVecOut, SW_Auth_Alg_Type  pAlgo);

/**
 * @brief This functions sets the Hash paramaters - Mode and Key
 *        for HMAC.
 *
 * @param cntx       [in] Pointer to cipher context handle
 * @param nParamID   [in] HMAC parameter id to set
 * @param pParam     [in] Pointer to parameter data
 * @param cParam     [in] Size of parameter data in bytes
 * @param Algo      [in] Algorithm type
 *
 * @return CeMLErrorType
 *
 */
sw_crypto_errno_enum_type SW_Hash_SetParam (CryptoCntxHandle*        cntx,
                                SW_Auth_Param_Type  nParamID,
                                const void         *pParam, // key pointer in case of SW_HASH_PARAM_HMAC_KEY
                                uint32             cParam,  // key length in case of SW_HASH_PARAM_HMAC_KEY
                                SW_Auth_Alg_Type  pAlgo);

/**
 * @brief Intialize a hmac context for hmac update and final functions
 *
 * @param cntx                     [in] Pointer to a pointer to the hmac context
 * @param pAlgo                    [in] Algorithm type
 *
 * @return sw_crypto_errno_enum_type
 *
 *
 */
sw_crypto_errno_enum_type SW_Hmac_Init(CryptoCntxHandle**  cntx,SW_Auth_Alg_Type  pAlgo);

/**
 * @brief This function will reset the context for the next Hmac operation.
 *
 * @param handle        [in]  Pointer of hmac context
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_Hmac_Reset(CryptoCntxHandle *handle);

/**
 * @brief This function will hmac data into the hmac context
 *        structure, which must have been initialized by
 *        CeMLHmacInit.
 *
 * @param cntx      [in] Pointer to Hmac context
 * @param pAlgo     [in] Input message to be
 *                     hmaced
 * @return sw_crypto_errno_enum_type
 *
 *
 */
sw_crypto_errno_enum_type SW_Hmac_Update(CryptoCntxHandle*  cntx,IovecListType  ioVecIn);

/**
 * @brief Compute the final digest hmac value.
 *
 * @param cntx      [in] Pointer to Hmac context
 * @param ioVecIn   [out] Pointer to output digest

 * @return sw_crypto_errno_enum_type
 *
 *
 */
sw_crypto_errno_enum_type SW_Hmac_Final(CryptoCntxHandle* cntx,IovecListType  *ioVecOut);

/**
 * @brief Deintialize a hmac context
 *
 * @param cntx      [in] Pointer to a pointer to the hmac context
 *
 * @return sw_crypto_errno_enum_type
 *
 *
 */
sw_crypto_errno_enum_type SW_Hmac_Deinit(CryptoCntxHandle** cntx);

#endif /* AES_SHARED */
