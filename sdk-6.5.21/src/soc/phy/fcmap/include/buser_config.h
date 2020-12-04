/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __BMF_CONFIG_H__
#define __BMF_CONFIG_H__

#ifndef __PEDANTIC__
#ifndef COMPILER_OVERRIDE_NO_LONGLONG
#ifndef COMPILER_HAS_LONGLONG
#define COMPILER_HAS_LONGLONG
#endif /* !COMPILER_HAS_LONGLONG */
#endif /* !COMPILER_OVERRIDE_NO_LONGLONG */
#endif /* !__PEDANTIC__ */

#include <bcompiler.h>

/* Maximum number of ports per chip supported */
#ifndef BMF_MAX_PORTS_PER_MT_DEV
#define BMF_MAX_PORTS_PER_MT_DEV         8
#endif

/*
 * We are allowing Max 16 FCMAP ports for BCM84756 device.
 * For BCM8806x, it is the Max FCMAP ports which is the closest
 * multiple of 8 to switch platform allowed max ports.
 */
#ifndef BMF_MAX_MT_PORTS
#if defined(BCM84756_A0)
#define BMF_MAX_MT_PORTS			 16
#else
#define BMF_MAX_MT_PORTS    (SOC_MAX_NUM_PORTS - (SOC_MAX_NUM_PORTS % BMF_MAX_PORTS_PER_MT_DEV))
#endif
#endif

#ifndef BMF_CONFIG_DEFINE_UINT8_T
#define BMF_CONFIG_DEFINE_UINT8_T               1         
#endif

/* Default type definition for uint8 */
#ifndef BMF_CONFIG_TYPE_UINT8_T
#define BMF_CONFIG_TYPE_UINT8_T                 unsigned char
#endif

#ifndef BMF_CONFIG_DEFINE_INT8_T
#define BMF_CONFIG_DEFINE_INT8_T               1         
#endif

#ifndef BMF_CONFIG_TYPE_INT8_T
#define BMF_CONFIG_TYPE_INT8_T                 char
#endif

/* Type buint16_t is not provided by the system */
#ifndef BMF_CONFIG_DEFINE_UINT16_T
#define BMF_CONFIG_DEFINE_UINT16_T              1         
#endif

/* Default type definition for uint16 */
#ifndef BMF_CONFIG_TYPE_UINT16_T
#define BMF_CONFIG_TYPE_UINT16_T                unsigned short
#endif

#ifndef BMF_CONFIG_DEFINE_INT16_T
#define BMF_CONFIG_DEFINE_INT16_T              1         
#endif

#ifndef BMF_CONFIG_TYPE_INT16_T
#define BMF_CONFIG_TYPE_INT16_T                short int
#endif

/* Type buint32_t is not provided by the system */
#ifndef BMF_CONFIG_DEFINE_UINT32_T
#define BMF_CONFIG_DEFINE_UINT32_T              1         
#endif

/* Default type definition for uint32 */
#ifndef BMF_CONFIG_TYPE_UINT32_T
#define BMF_CONFIG_TYPE_UINT32_T                unsigned int
#endif

#ifndef BMF_CONFIG_DEFINE_INT32_T
#define BMF_CONFIG_DEFINE_INT32_T              1         
#endif

#ifndef BMF_CONFIG_TYPE_INT32_T
#define BMF_CONFIG_TYPE_INT32_T                int
#endif

#ifndef BMF_CONFIG_DEFINE_UINT64_T
#define BMF_CONFIG_DEFINE_UINT64_T              1         
#endif

/* Default type definition for uint64 */
#ifndef BMF_CONFIG_TYPE_UINT64_T
#define BMF_CONFIG_TYPE_UINT64_T                BCOMPILER_COMPILER_UINT64
#endif

#ifndef BMF_CONFIG_DEFINE_INT64_T
#define BMF_CONFIG_DEFINE_INT64_T              1         
#endif

#ifndef BMF_CONFIG_TYPE_INT64_T
#define BMF_CONFIG_TYPE_INT64_T                BCOMPILER_COMPILER_INT64
#endif


#endif /* __BMF_CONFIG_H__ */


