/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System interface definitions for SDKLT.
 */

#ifndef PHYMOD_CUSTOM_CONFIG_H
#define PHYMOD_CUSTOM_CONFIG_H

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <sal/sal_alloc.h>
#include <sal/sal_mutex.h>

#include <shr/shr_error.h>

#include <phymod_custom_chip_config.h>

#define PHYMOD_DEBUG_ERROR(stuff_)      LOG_ERROR(BSL_LS_BCMPC_PHYMOD, stuff_)

#define PHYMOD_DEBUG_WARN(stuff_)       LOG_WARN(BSL_LS_BCMPC_PHYMOD, stuff_)

#define PHYMOD_DEBUG_VERBOSE(stuff_)    LOG_VERBOSE(BSL_LS_BCMPC_PHYMOD, stuff_)

#define PHYMOD_DIAG_OUT(stuff_)         cli_out stuff_

#define PHYMOD_CONFIG_DEFINE_SIZE_T     0
#define PHYMOD_CONFIG_DEFINE_UINT8_T    0
#define PHYMOD_CONFIG_DEFINE_UINT16_T   0
#define PHYMOD_CONFIG_DEFINE_UINT32_T   0
#define PHYMOD_CONFIG_DEFINE_UINT64_T   0
#define PHYMOD_CONFIG_DEFINE_INT8_T     0
#define PHYMOD_CONFIG_DEFINE_INT16_T    0
#define PHYMOD_CONFIG_DEFINE_INT32_T    0
#define PHYMOD_CONFIG_DEFINE_INT64_T    0
#define PHYMOD_CONFIG_DEFINE_PRIu32     0
#define PHYMOD_CONFIG_DEFINE_PRId32     0
#define PHYMOD_CONFIG_DEFINE_PRIx32     0
#define PHYMOD_CONFIG_DEFINE_PRIu64     0
#define PHYMOD_CONFIG_DEFINE_PRId64     0
#define PHYMOD_CONFIG_DEFINE_PRIx64     0

/* Allow floating point by default */
#ifndef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
#define PHYMOD_CONFIG_INCLUDE_FLOATING_POINT    1
#endif

#if PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
#include <math.h>
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

/* Structure defines */
#define phymod_mutex_t          sal_mutex_t

/* Define the PHYMOD_XXX functions */
#define PHYMOD_USLEEP           sal_usleep
#define PHYMOD_SLEEP            sal_sleep
#define PHYMOD_MALLOC           sal_alloc
#define PHYMOD_FREE             sal_free
#define PHYMOD_MEMSET           sal_memset
#define PHYMOD_MEMCPY           sal_memcpy
#define PHYMOD_STRCMP           sal_strcmp
#define PHYMOD_STRNCMP          sal_strncmp
#define PHYMOD_STRNCPY          sal_strncpy
#define PHYMOD_STRCHR           sal_strchr
#define PHYMOD_STRSTR           sal_strstr
#define PHYMOD_STRLEN           sal_strlen
#define PHYMOD_STRCPY           sal_strcpy
#define PHYMOD_STRCAT           sal_strcat
#define PHYMOD_STRNCAT          sal_strncat
#define PHYMOD_STRTOUL          sal_strtoul
#define PHYMOD_SPRINTF          sal_sprintf
#define PHYMOD_SNPRINTF         sal_snprintf
#define PHYMOD_TIME_USECS       sal_time_usecs
#define PHYMOD_SRAND            sal_srand
#define PHYMOD_RAND             sal_rand
#define PHYMOD_SPL              phymod_spl
#define PHYMOD_SPLHI            phymod_splhi

static inline int phymod_spl(int level)
{
    return 0;
}

static inline int phymod_splhi(void)
{
    return 0;
}

/* Define macros used in PHYMOD library */
#ifndef COMPILER_64_SET
#define COMPILER_64_SET(dst, src_hi, src_lo) \
    ((dst) = (((uint64_t)((uint32_t)(src_hi))) << 32) | \
              ((uint64_t)((uint32_t)(src_lo))))
#endif
#ifndef COMPILER_64_COPY
#define COMPILER_64_COPY(dst, src)    ((dst) = (src))
#endif
#ifndef COMPILER_64_LO
#define COMPILER_64_LO(src)           ((uint32_t)(src))
#endif
#ifndef COMPILER_64_ADD_64
#define COMPILER_64_ADD_64(dst, src)  ((dst) += (src))
#endif
#ifndef COMPILER_64_UMUL_32
#define COMPILER_64_UMUL_32(dst, src) ((dst) *= (src))
#endif
#ifndef COMPILER_64_UDIV_64
#define COMPILER_64_UDIV_64(dst, src) ((dst) /= (src))
#endif
#ifndef COMPILER_64_UDIV_32
#define COMPILER_64_UDIV_32(dst, src)                \
  do {                                               \
           uint64_t u64_src;                         \
           COMPILER_64_SET(u64_src, 0, src);         \
           COMPILER_64_UDIV_64(dst, u64_src);        \
    } while(0)

#endif
#ifndef COMPILER_64_SHL
#define COMPILER_64_SHL(dst, bits)    ((dst) <<= (bits))
#endif
#ifndef COMPILER_64_LT
#define COMPILER_64_LT(src1, src2)    ((src1) < (src2))
#endif
#ifndef COMPILER_64_GE
#define COMPILER_64_GE(src1, src2)    ((src1) >= (src2))
#endif
#ifndef COMPILER_64_IS_ZERO
#define COMPILER_64_IS_ZERO(src)        ((src) == 0)
#endif
#ifndef COMPILER_64_HI
#define COMPILER_64_HI(src)             ((uint32_t) ((src) >> 32))
#endif


#endif /* PHYMOD_CUSTOM_CONFIG_H */
