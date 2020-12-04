/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System interface definitions for Switch SDK
 */

#ifndef __PHYMOD_CUSTOM_CONFIG_H__
#define __PHYMOD_CUSTOM_CONFIG_H__

#include <shared/bsl.h>
#include <shared/error.h>

#include <sal/core/libc.h>
#include <sal/core/alloc.h>

#include <sal/core/thread.h>
#include <sal/core/time.h>

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

/* Do not map directly to SAL function */
#define PHYMOD_USLEEP   soc_phymod_usleep
#define PHYMOD_SLEEP    soc_phymod_sleep
#define PHYMOD_STRCMP   soc_phymod_strcmp
#define PHYMOD_MEMSET   soc_phymod_memset
#define PHYMOD_MEMCPY   soc_phymod_memcpy
#define PHYMOD_MALLOC   soc_phymod_alloc
#define PHYMOD_FREE     soc_phymod_free

/* These functions map directly to SAL functions */
#define PHYMOD_STRLEN   sal_strlen
#define PHYMOD_STRSTR   sal_strstr
#define PHYMOD_STRCHR   sal_strchr
#define PHYMOD_STRNCMP  sal_strncmp
#define PHYMOD_SNPRINTF sal_snprintf
#define PHYMOD_SPRINTF  sal_sprintf
#define PHYMOD_STRCAT   sal_strcat
#define PHYMOD_STRNCAT   sal_strncat
#define PHYMOD_STRCPY   sal_strcpy
#define PHYMOD_STRNCPY  sal_strncpy
#define PHYMOD_ATOI     sal_atoi
#define PHYMOD_THREAD_CREATE sal_thread_create
#define PHYMOD_TIME_USECS sal_time_usecs
#define PHYMOD_SRAND      sal_srand
#define PHYMOD_RAND       sal_rand
#define PHYMOD_SPL        sal_spl
#define PHYMOD_SPLHI      sal_splhi

#ifdef __KERNEL__
#define PHYMOD_STRTOUL  simple_strtol
#else
#ifndef __int8_t_defined
#define __int8_t_defined
typedef signed char         int8_t;
typedef signed short int    int16_t;
typedef signed int          int32_t;
#endif
#ifndef __uint32_t_defined
#define __uint32_t_defined
typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned int        uint32_t;
#endif
#define PHYMOD_STRTOUL  sal_strtoul
#endif

#include <sal/appl/io.h>

#define PHYMOD_PRINTF   sal_printf

/* Use SDK-versions of stdint types */
#define PHYMOD_CONFIG_DEFINE_UINT8_T    0
#define uint8_t uint8
#define PHYMOD_CONFIG_DEFINE_UINT16_T   0
#define uint16_t uint16
#define PHYMOD_CONFIG_DEFINE_UINT32_T   0
#define uint32_t uint32
#define PHYMOD_CONFIG_DEFINE_UINT64_T   0
#define PHYMOD_CONFIG_DEFINE_INT64_T   0
/*
 * Unfortunately, the code above may define 'uint64_t'.
 * If this comes from 'stdint.h' then we leave it as is.
 * Otherwise, we 'undef' it to let the code below define it.
 * This whole section is added here to allow for COMPILER_OVERRIDE_NO_LONGLONG
 */
#ifndef _STDINT_H
/* { */
#ifdef uint64_t
/* { */
#undef uint64_t
/* } */
#endif
#define uint64_t uint64

#ifdef int64_t
/* { */
#undef int64_t
/* } */
#endif
#define int64_t int64
/* } */
#endif

#define PHYMOD_CONFIG_DEFINE_INT8_T     0
#define PHYMOD_CONFIG_DEFINE_INT16_T    0
#define PHYMOD_CONFIG_DEFINE_INT32_T    0

/* No need to define size_t */
#define PHYMOD_CONFIG_DEFINE_SIZE_T     0

/* Allow floating point except for Linux kernel builds */
#ifndef __KERNEL__
#define PHYMOD_CONFIG_INCLUDE_FLOATING_POINT    1
#endif

/* Enable external info table by default */
#ifndef PHYMOD_CONFIG_EXTERNAL_INFO_TABLE_EN
#define PHYMOD_CONFIG_EXTERNAL_INFO_TABLE_EN    1
#endif

/* Include register reset values in PHY symbol tables */
#define PHYMOD_CONFIG_INCLUDE_RESET_VALUES  1

/* Match PHYMOD error code with shared error codes */
#define PHYMOD_CONFIG_DEFINE_ERROR_CODES    0

typedef enum {
    PHYMOD_E_NONE       = _SHR_E_NONE,
    PHYMOD_E_INTERNAL   = _SHR_E_INTERNAL,
    PHYMOD_E_MEMORY     = _SHR_E_MEMORY,
    PHYMOD_E_IO         = _SHR_E_INTERNAL, 
    PHYMOD_E_PARAM      = _SHR_E_PARAM,
    PHYMOD_E_CORE       = _SHR_E_PORT,
    PHYMOD_E_PHY        = _SHR_E_PORT,
    PHYMOD_E_BUSY       = _SHR_E_BUSY,
    PHYMOD_E_FAIL       = _SHR_E_FAIL,
    PHYMOD_E_TIMEOUT    = _SHR_E_TIMEOUT,
    PHYMOD_E_RESOURCE   = _SHR_E_RESOURCE,
    PHYMOD_E_CONFIG     = _SHR_E_CONFIG,
    PHYMOD_E_UNAVAIL    = _SHR_E_UNAVAIL,
    PHYMOD_E_INIT       = _SHR_E_INIT,
    PHYMOD_E_EMPTY      = _SHR_E_EMPTY,
    PHYMOD_E_LIMIT      = _SHR_E_LIMIT           /* Must come last */
} phymod_error_t;

#endif /* __PHYMOD_CUSTOM_CONFIG_H__ */
