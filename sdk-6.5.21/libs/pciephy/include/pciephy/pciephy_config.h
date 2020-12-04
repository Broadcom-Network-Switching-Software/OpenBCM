/*! \file pciephy_config.h
 *
 * Interface definitions for PCIEPHY
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PCIEPHY_CONFIG_H_
#define _PCIEPHY_CONFIG_H_

/*!
 * System dependent definitions.
 */
#include <pciephy_custom_config.h>

/*
 * If any of the definitions are not provided by the custom config,
 * the following defines will be used by default.
 */

/*
 * Log functions.
 */
#ifndef PCIEPHY_DIAG_OUT
#define PCIEPHY_DIAG_OUT         printf
#endif

/*
 * Libc functions.
 */
#ifndef PCIEPHY_MEMSET
#define PCIEPHY_MEMSET           memset
#endif
#ifndef PCIEPHY_STRLEN
#define PCIEPHY_STRLEN           strlen
#endif
#ifndef PCIEPHY_STRNCAT
#define PCIEPHY_STRNCAT          strncat
#endif
#ifndef PCIEPHY_STRCPY
#define PCIEPHY_STRCPY           strcpy
#endif
#ifndef PCIEPHY_STRCMP
#define PCIEPHY_STRCMP           strcmp
#endif
#ifndef PCIEPHY_STRNCMP
#define PCIEPHY_STRNCMP          strncmp
#endif
#ifndef PCIEPHY_SPRINTF
#define PCIEPHY_SPRINTF          sprintf
#endif
#ifndef PCIEPHY_UDELAY
#define PCIEPHY_UDELAY           udelay
#endif

/*
 * Types.
 */
#ifndef PCIEPHY_CUSTOM_CONFIG_TYPE_INT8_T
typedef signed char             int8_t;
#endif
#ifndef PCIEPHY_CUSTOM_CONFIG_TYPE_INT16_T
typedef signed short int        int16_t;
#endif
#ifndef PCIEPHY_CUSTOM_CONFIG_TYPE_INT32_T
typedef signed int              int32_t;
#endif
#ifndef PCIEPHY_CUSTOM_CONFIG_TYPE_UINT8_T
typedef unsigned char           uint8_t;
#endif
#ifndef PCIEPHY_CUSTOM_CONFIG_TYPE_UINT16_T
typedef unsigned short int      uint16_t;
#endif
#ifndef PCIEPHY_CUSTOM_CONFIG_TYPE_UINT32_T
typedef unsigned int            uint32_t;
#endif
#ifndef PCIEPHY_CUSTOM_CONFIG_TYPE_UINT64_T
typedef unsigned long long      uint64_t;
#endif
#ifndef PCIEPHY_CUSTOM_CONFIG_TYPE_UINTPRT_T
typedef unsigned long int       uintptr_t;
#endif

/*
 * Error codes.
 */
#ifndef PCIEPHY_CUSTOM_CONFIG_TYPE_ERROR_T
typedef enum {
    PCIEPHY_E_NONE       = 0,
    PCIEPHY_E_INTERNAL   = -1,
    PCIEPHY_E_MEMORY     = -2,
    PCIEPHY_E_IO         = -3,
    PCIEPHY_E_PARAM      = -4,
    PCIEPHY_E_CORE       = -5,
    PCIEPHY_E_PHY        = -6,
    PCIEPHY_E_BUSY       = -7,
    PCIEPHY_E_FAIL       = -8,
    PCIEPHY_E_TIMEOUT    = -9,
    PCIEPHY_E_RESOURCE   = -10,
    PCIEPHY_E_CONFIG     = -11,
    PCIEPHY_E_UNAVAIL    = -12,
    PCIEPHY_E_INIT       = -13,
    PCIEPHY_E_EMPTY      = -14,
    PCIEPHY_E_LIMIT      = -15           /* Must come last */
} pciephy_error_t;
#endif

#endif /* _PCIEPHY_CONFIG_H_ */
