/*! \file pciephy_custom_config.h
 *
 * System interface definitions for Switch SDKLT
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PCIEPHY_CUSTOM_CONFIG_H
#define PCIEPHY_CUSTOM_CONFIG_H

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_udelay.h>
#include <shr/shr_error.h>

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
 * Error codes.
 */
typedef enum {
    PCIEPHY_E_NONE       = SHR_E_NONE,
    PCIEPHY_E_INTERNAL   = SHR_E_INTERNAL,
    PCIEPHY_E_MEMORY     = SHR_E_MEMORY,
    PCIEPHY_E_IO         = SHR_E_INTERNAL,
    PCIEPHY_E_PARAM      = SHR_E_PARAM,
    PCIEPHY_E_CORE       = SHR_E_PORT,
    PCIEPHY_E_PHY        = SHR_E_PORT,
    PCIEPHY_E_BUSY       = SHR_E_BUSY,
    PCIEPHY_E_FAIL       = SHR_E_FAIL,
    PCIEPHY_E_TIMEOUT    = SHR_E_TIMEOUT,
    PCIEPHY_E_RESOURCE   = SHR_E_RESOURCE,
    PCIEPHY_E_CONFIG     = SHR_E_CONFIG,
    PCIEPHY_E_UNAVAIL    = SHR_E_UNAVAIL,
    PCIEPHY_E_INIT       = SHR_E_INIT,
    PCIEPHY_E_EMPTY      = SHR_E_EMPTY,
    PCIEPHY_E_LIMIT      = SHR_E_LIMIT           /* Must come last */
} pciephy_error_t;

#endif /* PCIEPHY_CUSTOM_CONFIG_H */
