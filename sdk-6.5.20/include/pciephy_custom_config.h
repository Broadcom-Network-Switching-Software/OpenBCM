/*! \file pciephy_custom_config.h
 *
 * System interface definitions for Switch SDK
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PCIEPHY_CUSTOM_CONFIG_H_
#define _PCIEPHY_CUSTOM_CONFIG_H_

#include <shared/bsl.h>
#include <shared/error.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <sal/appl/io.h>

/*
 * Log functions.
 */
#define PCIEPHY_DIAG_OUT         cli_out

/*
 * Libc functions.
 */
#define PCIEPHY_MEMSET           sal_memset
#define PCIEPHY_STRLEN           sal_strlen
#define PCIEPHY_STRNCAT          sal_strncat
#define PCIEPHY_STRCPY           sal_strcpy
#define PCIEPHY_STRCMP           sal_strcmp
#define PCIEPHY_STRNCMP          sal_strncmp
#define PCIEPHY_SPRINTF          sal_sprintf
#define PCIEPHY_UDELAY           sal_udelay

/* Use SDK-version types */
#define PCIEPHY_CUSTOM_CONFIG_TYPE_INT8_T
#define PCIEPHY_CUSTOM_CONFIG_TYPE_INT16_T
#define PCIEPHY_CUSTOM_CONFIG_TYPE_INT32_T
#define PCIEPHY_CUSTOM_CONFIG_TYPE_UINT8_T
#define PCIEPHY_CUSTOM_CONFIG_TYPE_UINT16_T
#define PCIEPHY_CUSTOM_CONFIG_TYPE_UINT32_T
#define PCIEPHY_CUSTOM_CONFIG_TYPE_UINT64_T
#define PCIEPHY_CUSTOM_CONFIG_TYPE_UINTPRT_T
#define PCIEPHY_CUSTOM_CONFIG_TYPE_ERROR_T

/*
 * Types.
 */
typedef signed char             int8_t;
typedef signed short int        int16_t;
typedef signed int              int32_t;
typedef uint8                   uint8_t;
typedef uint16                  uint16_t;
typedef uint32                  uint32_t;
typedef uint64                  uint64_t;
typedef unsigned long int       uintptr_t;

/*
 * Error codes.
 */
typedef enum {
    PCIEPHY_E_NONE       = _SHR_E_NONE,
    PCIEPHY_E_INTERNAL   = _SHR_E_INTERNAL,
    PCIEPHY_E_MEMORY     = _SHR_E_MEMORY,
    PCIEPHY_E_IO         = _SHR_E_INTERNAL,
    PCIEPHY_E_PARAM      = _SHR_E_PARAM,
    PCIEPHY_E_CORE       = _SHR_E_PORT,
    PCIEPHY_E_PHY        = _SHR_E_PORT,
    PCIEPHY_E_BUSY       = _SHR_E_BUSY,
    PCIEPHY_E_FAIL       = _SHR_E_FAIL,
    PCIEPHY_E_TIMEOUT    = _SHR_E_TIMEOUT,
    PCIEPHY_E_RESOURCE   = _SHR_E_RESOURCE,
    PCIEPHY_E_CONFIG     = _SHR_E_CONFIG,
    PCIEPHY_E_UNAVAIL    = _SHR_E_UNAVAIL,
    PCIEPHY_E_INIT       = _SHR_E_INIT,
    PCIEPHY_E_EMPTY      = _SHR_E_EMPTY,
    PCIEPHY_E_LIMIT      = _SHR_E_LIMIT           /* Must come last */
} pciephy_error_t;

#endif /* _PCIEPHY_CUSTOM_CONFIG_H_ */
