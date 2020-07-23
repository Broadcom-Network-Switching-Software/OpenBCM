/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __BBASE_TYPES_H
#define __BBASE_TYPES_H

#include <buser_config.h>

#if BMF_CONFIG_DEFINE_UINT8_T == 1
typedef BMF_CONFIG_TYPE_UINT8_T buint8_t; 
#endif

#if BMF_CONFIG_DEFINE_INT8_T == 1
typedef BMF_CONFIG_TYPE_INT8_T bint8_t; 
#endif

#if BMF_CONFIG_DEFINE_UINT16_T == 1
typedef BMF_CONFIG_TYPE_UINT16_T buint16_t; 
#endif

#if BMF_CONFIG_DEFINE_INT16_T == 1
typedef BMF_CONFIG_TYPE_INT16_T bint16_t; 
#endif

#if BMF_CONFIG_DEFINE_UINT32_T == 1
typedef BMF_CONFIG_TYPE_UINT32_T buint32_t; 
#endif

#if BMF_CONFIG_DEFINE_INT32_T == 1
typedef BMF_CONFIG_TYPE_INT32_T bint32_t; 
#endif

#if BMF_CONFIG_DEFINE_UINT64_T == 1
typedef BMF_CONFIG_TYPE_UINT64_T buint64_t; 
#endif

#if BMF_CONFIG_DEFINE_INT64_T == 1
typedef BMF_CONFIG_TYPE_INT64_T bint64_t; 
#endif


typedef buint8_t bmac_addr_t[6]; 

#define BMAC_TO_32_HI(mac)\
   ((((buint8_t *)(mac))[0] << 8  )|\
    (((buint8_t *)(mac))[1] << 0  ))
#define BMAC_TO_32_LO(mac)\
   ((((buint8_t *)(mac))[2] << 24 )|\
    (((buint8_t *)(mac))[3] << 16 )|\
    (((buint8_t *)(mac))[4] << 8  )|\
    (((buint8_t *)(mac))[5] << 0  ))
#define BMAC_BUILD_FROM_32(mac, hi, lo)\
   ((buint8_t *)(mac))[0] = ((hi) >> 8)  & 0xff ;\
   ((buint8_t *)(mac))[1] = ((hi) >> 0)  & 0xff ;\
   ((buint8_t *)(mac))[2] = ((lo) >> 24) & 0xff ;\
   ((buint8_t *)(mac))[3] = ((lo) >> 16) & 0xff ;\
   ((buint8_t *)(mac))[4] = ((lo) >> 8 ) & 0xff ;\
   ((buint8_t *)(mac))[5] = ((lo) >> 0 ) & 0xff ;

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef STATIC
#define STATIC static
#endif

typedef int   (*bprint_fn)(const char *format, ...);
#define BPRINT_FN_DEBUG      0x1
#define BPRINT_FN_PRINTF     0x2


#endif /* __BBASE_TYPES_H */

