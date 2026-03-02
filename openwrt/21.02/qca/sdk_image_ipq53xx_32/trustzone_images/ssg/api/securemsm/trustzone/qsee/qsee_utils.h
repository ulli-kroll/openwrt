/*===========================================================================
  Copyright (c) 2018 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

#pragma once

#include <stdint.h>
#include "cdefs.h"

#ifndef MIN
  #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef MAX
  #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef ARR_SIZE
  #define  ARR_SIZE( a )  C_LENGTHOF(a)
#endif

#define htonll(x) \
((uint64_t)((((uint64_t)(x) & 0x00000000000000ffll) << 56) | \
  (((uint64_t)(x) & 0x000000000000ff00ll) << 40) | \
  (((uint64_t)(x) & 0x0000000000ff0000ll) << 24) | \
                  (((uint64_t)(x) & 0x00000000ff000000ll) <<  8) | \
                  (((uint64_t)(x) & 0x000000ff00000000ll) >>  8) | \
                  (((uint64_t)(x) & 0x0000ff0000000000ll) >> 24) | \
  (((uint64_t)(x) & 0x00ff000000000000ll) >> 40) | \
  (((uint64_t)(x) & 0xff00000000000000ull) >> 56) ))

#define htonl(x) \
        ((uint32_t)((((uint32_t)(x) & 0x000000ffU) << 24) | \
                  (((uint32_t)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32_t)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32_t)(x) & 0xff000000U) >> 24)))

#define htons(x) \
        ((uint16_t)((((uint16_t)(x) & 0x00ff) << 8) | \
                  (((uint16_t)(x) & 0xff00) >> 8)))

#define ntohll(x) \
((uint64_t)((((uint64_t)(x) & 0x00000000000000ffll) << 56) | \
  (((uint64_t)(x) & 0x000000000000ff00ll) << 40) | \
  (((uint64_t)(x) & 0x0000000000ff0000ll) << 24) | \
                  (((uint64_t)(x) & 0x00000000ff000000ll) <<  8) | \
                  (((uint64_t)(x) & 0x000000ff00000000ll) >>  8) | \
                  (((uint64_t)(x) & 0x0000ff0000000000ll) >> 24) | \
  (((uint64_t)(x) & 0x00ff000000000000ll) >> 40) | \
  (((uint64_t)(x) & 0xff00000000000000ull) >> 56) ))

#define ntohl(x) \
        ((uint32_t)((((uint32_t)(x) & 0x000000ffU) << 24) | \
                  (((uint32_t)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32_t)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32_t)(x) & 0xff000000U) >> 24)))

#define ntohs(x) \
        ((uint16_t)((((uint16_t)(x) & 0x00ff) << 8) | \
        (((uint16_t)(x) & 0xff00) >> 8)))
