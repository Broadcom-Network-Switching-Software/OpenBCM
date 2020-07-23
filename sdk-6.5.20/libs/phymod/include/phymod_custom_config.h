/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System interface definitions for SDKLT
 * This file is used to build the hybrid SDK with SDKLT source code. It is
 * derived from $SDK6/include/phymod_custom_config.h by modifying the included
 * header files and default definition of formatting macros. It enables all
 * supported phy chips by default, and also allows to build the lightweight
 * development system with SDK6 phymod library by specifying $$PHYMOD
 * environment variable (e.g. PHYMOD=$SDK6/libs/phymod) on command line.
 */

#ifndef PHYMOD_CUSTOM_CONFIG_H
#define PHYMOD_CUSTOM_CONFIG_H

#include <bsl/bsl.h>
#include <shr/shr_error.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <sal/sal_sleep.h>

#ifndef USER_DEFINED_PHYMOD_CHIPS
#define PHYMOD_EAGLE_SUPPORT
#define PHYMOD_FALCON_SUPPORT
#define PHYMOD_FALCON16_SUPPORT
#define PHYMOD_MERLIN16_SUPPORT
#define PHYMOD_QSGMIIE_SUPPORT
#define PHYMOD_TSCE_SUPPORT
#define PHYMOD_TSCF_SUPPORT
#define PHYMOD_TSCF16_SUPPORT
#define PHYMOD_TSCE16_SUPPORT
#define PHYMOD_VIPER_SUPPORT
#define PHYMOD_FURIA_SUPPORT
#define PHYMOD_SESTO_SUPPORT
#define PHYMOD_QUADRA28_SUPPORT
#define PHYMOD_QTCE_SUPPORT
#define PHYMOD_HURACAN_SUPPORT
#define PHYMOD_MADURA_SUPPORT
#define PHYMOD_DINO_SUPPORT
#define PHYMOD_PHY8806X_SUPPORT
#define PHYMOD_TSCE_DPLL_SUPPORT
#define PHYMOD_EAGLE_DPLL_SUPPORT
#define PHYMOD_BLACKHAWK_SUPPORT
#define PHYMOD_TSCBH_SUPPORT
#define PHYMOD_TSCF_GEN3_SUPPORT
#define PHYMOD_FALCON_DPLL_SUPPORT
#define PHYMOD_QTCE16_SUPPORT
#define PHYMOD_NULL_SUPPORT
#define PHYMOD_TSCBH_GEN2_SUPPORT
#define PHYMOD_MERLIN7_SUPPORT
#define PHYMOD_TSCE7_SUPPORT
#define PHYMOD_TSCF16_GEN3_SUPPORT
#define PHYMOD_FALCON16_V1L4P1_SUPPORT
#define PHYMOD_TSCBH_FLEXE_SUPPORT
#define PHYMOD_TSCBH_GEN3_SUPPORT
#define PHYMOD_BLACKHAWK7_V1L8P1_SUPPORT
#define PHYMOD_BLACKHAWK7_L8P2_SUPPORT
#endif /* USER_DEFINED_PHYMOD_CHIPS */

#define PHYMOD_DEBUG_ERROR(stuff_) \
    LOG_ERROR(BSL_LS_SOC_PHYMOD, stuff_)

#define PHYMOD_DEBUG_VERBOSE(stuff_) \
    LOG_VERBOSE(BSL_LS_SOC_PHYMOD, stuff_)

#define PHYMOD_DEBUG_WARN(stuff_) \
    LOG_WARN(BSL_LS_SOC_PHYMOD, stuff_)

#define PHYMOD_DIAG_OUT(stuff_) \
    cli_out stuff_

/* These functions map directly to SAL functions */
#define PHYMOD_USLEEP   sal_usleep
#define PHYMOD_SLEEP    sal_sleep
#define PHYMOD_STRCMP   sal_strcmp
#define PHYMOD_MEMSET   sal_memset
#define PHYMOD_MEMCPY   sal_memcpy
#define PHYMOD_MALLOC   sal_alloc
#define PHYMOD_FREE     sal_free

/* These functions map directly to SAL functions */
#define PHYMOD_STRLEN   sal_strlen
#define PHYMOD_STRSTR   sal_strstr
#define PHYMOD_STRCHR   sal_strchr
#define PHYMOD_STRNCMP  sal_strncmp
#define PHYMOD_SNPRINTF sal_snprintf
#define PHYMOD_SPRINTF  sal_sprintf
#define PHYMOD_STRCAT   sal_strcat
#define PHYMOD_STRNCAT  sal_strncat
#define PHYMOD_STRCPY   sal_strcpy
#define PHYMOD_STRNCPY  sal_strncpy
#define PHYMOD_ATOI     sal_atoi
#define PHYMOD_THREAD_CREATE sal_thread_create
#define PHYMOD_TIME_USECS sal_time_usecs
#define PHYMOD_SRAND    sal_srand
#define PHYMOD_RAND     sal_rand
#define PHYMOD_SPL      sal_spl
#define PHYMOD_SPLHI    sal_splhi

#define PHYMOD_STRTOUL  sal_strtoul

/* Use SDK-versions of stdint types */
#define PHYMOD_CONFIG_DEFINE_UINT8_T    0
#define PHYMOD_CONFIG_DEFINE_UINT16_T   0
#define PHYMOD_CONFIG_DEFINE_UINT32_T   0
#define PHYMOD_CONFIG_DEFINE_UINT64_T   0

#define PHYMOD_CONFIG_DEFINE_INT8_T     0
#define PHYMOD_CONFIG_DEFINE_INT16_T    0
#define PHYMOD_CONFIG_DEFINE_INT32_T    0

/* No need to define size_t */
#define PHYMOD_CONFIG_DEFINE_SIZE_T     0

#define PHYMOD_CONFIG_DEFINE_PRIu32     0
#define PHYMOD_CONFIG_DEFINE_PRId32     0
#define PHYMOD_CONFIG_DEFINE_PRIx32     0
#define PHYMOD_CONFIG_DEFINE_INT64_T    0
#define PHYMOD_CONFIG_DEFINE_PRIu64     0
#define PHYMOD_CONFIG_DEFINE_PRId64     0
#define PHYMOD_CONFIG_DEFINE_PRIx64     0

/* Allow floating point except for Linux kernel builds */
#ifndef __KERNEL__
#define PHYMOD_CONFIG_INCLUDE_FLOATING_POINT    1
#endif

/* Enable external info table by default */
#ifndef PHYMOD_CONFIG_EXTERNAL_INFO_TABLE_EN
#define PHYMOD_CONFIG_EXTERNAL_INFO_TABLE_EN    1
#endif

/* Include register reset values in PHY symbol tables */
#define PHYMOD_CONFIG_INCLUDE_RESET_VALUES      1

/* Match PHYMOD error code with shared error codes */
#define PHYMOD_CONFIG_DEFINE_ERROR_CODES        0

typedef enum {
    PHYMOD_E_NONE       = SHR_E_NONE,
    PHYMOD_E_INTERNAL   = SHR_E_INTERNAL,
    PHYMOD_E_MEMORY     = SHR_E_MEMORY,
    PHYMOD_E_IO         = SHR_E_INTERNAL,
    PHYMOD_E_PARAM      = SHR_E_PARAM,
    PHYMOD_E_CORE       = SHR_E_PORT,
    PHYMOD_E_PHY        = SHR_E_PORT,
    PHYMOD_E_BUSY       = SHR_E_BUSY,
    PHYMOD_E_FAIL       = SHR_E_FAIL,
    PHYMOD_E_TIMEOUT    = SHR_E_TIMEOUT,
    PHYMOD_E_RESOURCE   = SHR_E_RESOURCE,
    PHYMOD_E_CONFIG     = SHR_E_CONFIG,
    PHYMOD_E_UNAVAIL    = SHR_E_UNAVAIL,
    PHYMOD_E_INIT       = SHR_E_INIT,
    PHYMOD_E_EMPTY      = SHR_E_EMPTY,
    PHYMOD_E_LIMIT      = SHR_E_LIMIT           /* Must come last */
} phymod_error_t;

#endif /* PHYMOD_CUSTOM_CONFIG_H */
