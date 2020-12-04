/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __BFCMAP_CONFIG_H__
#define __BFCMAP_CONFIG_H__

#include <buser_config.h>

/*
 * BFCMAP_MAX_UNITS is the number of dev_ctrl structs created
 * which is same as the number of fcmap ports.
 */
#ifndef BFCMAP_MAX_UNITS
#define BFCMAP_MAX_UNITS                    BMF_MAX_MT_PORTS
#endif

#define BFCMAP_NUM_PORT                     BMF_MAX_MT_PORTS

#ifndef BFCMAP_CONFIG_DEFINE_UINT8_T
#define BFCMAP_CONFIG_DEFINE_UINT8_T        BMF_CONFIG_DEFINE_UINT8_T
#endif

/* Default type definition for uint8 */
#ifndef BFCMAP_CONFIG_TYPE_UINT8_T
#define BFCMAP_CONFIG_TYPE_UINT8_T          BMF_CONFIG_TYPE_UINT8_T
#endif

#ifndef BFCMAP_CONFIG_DEFINE_INT8_T
#define BFCMAP_CONFIG_DEFINE_INT8_T         BMF_CONFIG_DEFINE_INT8_T
#endif

#ifndef BFCMAP_CONFIG_TYPE_INT8_T
#define BFCMAP_CONFIG_TYPE_INT8_T           BMF_CONFIG_TYPE_INT8_T
#endif

/* Type buint16_t is not provided by the system */
#ifndef BFCMAP_CONFIG_DEFINE_UINT16_T
#define BFCMAP_CONFIG_DEFINE_UINT16_T       BMF_CONFIG_DEFINE_UINT16_T
#endif

/* Default type definition for uint16 */
#ifndef BFCMAP_CONFIG_TYPE_UINT16_T
#define BFCMAP_CONFIG_TYPE_UINT16_T         BMF_CONFIG_TYPE_UINT16_T
#endif

#ifndef BFCMAP_CONFIG_DEFINE_INT16_T
#define BFCMAP_CONFIG_DEFINE_INT16_T        BMF_CONFIG_DEFINE_INT16_T
#endif

#ifndef BFCMAP_CONFIG_TYPE_INT16_T
#define BFCMAP_CONFIG_TYPE_INT16_T          BMF_CONFIG_TYPE_INT16_T
#endif

/* Type buint32_t is not provided by the system */
#ifndef BFCMAP_CONFIG_DEFINE_UINT32_T
#define BFCMAP_CONFIG_DEFINE_UINT32_T       BMF_CONFIG_DEFINE_UINT32_T
#endif

/* Default type definition for uint32 */
#ifndef BFCMAP_CONFIG_TYPE_UINT32_T
#define BFCMAP_CONFIG_TYPE_UINT32_T         BMF_CONFIG_TYPE_UINT32_T
#endif

#ifndef BFCMAP_CONFIG_DEFINE_INT32_T
#define BFCMAP_CONFIG_DEFINE_INT32_T        BMF_CONFIG_DEFINE_INT32_T
#endif

#ifndef BFCMAP_CONFIG_TYPE_INT32_T
#define BFCMAP_CONFIG_TYPE_INT32_T          BMF_CONFIG_TYPE_INT32_T
#endif

#ifndef BFCMAP_CONFIG_DEFINE_UINT64_T
#define BFCMAP_CONFIG_DEFINE_UINT64_T       BMF_CONFIG_DEFINE_UINT64_T
#endif

/* Default type definition for uint64 */
#ifndef BFCMAP_CONFIG_TYPE_UINT64_T
#define BFCMAP_CONFIG_TYPE_UINT64_T         BMF_CONFIG_TYPE_UINT64_T
#endif

#ifndef BFCMAP_CONFIG_DEFINE_INT64_T
#define BFCMAP_CONFIG_DEFINE_INT64_T        BMF_CONFIG_DEFINE_INT64_T
#endif

#ifndef BFCMAP_CONFIG_TYPE_INT64_T
#define BFCMAP_CONFIG_TYPE_INT64_T          BMF_CONFIG_TYPE_INT64_T
#endif


#endif /* __BFCMAP_CONFIG_H__ */


