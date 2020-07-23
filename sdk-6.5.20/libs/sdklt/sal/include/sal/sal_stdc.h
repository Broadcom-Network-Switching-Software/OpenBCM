/*! \file sal_stdc.h
 *
 * Convenience file for mapping SAL C library functions to standard C.
 *
 * This file should never be included by regular source files as this
 * may silently break our SAL portability rules.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_STDC_H
#define SAL_STDC_H

#if !defined(SAL_CONFIG_H) || defined(SAL_NO_STDC)

/*
 * If we get here it means that some file other than sal_config.h
 * included this file before sal_config.h did.
 */
#error sal_stdc.h file cannot be included by regular source files

#else

/*!
 * \cond SAL_STDC
 */

#include <stddef.h>
#define SAL_CONFIG_DEFINE_SIZE_T        0

#include <inttypes.h>
#define SAL_CONFIG_DEFINE_UINT8_T       0
#define SAL_CONFIG_DEFINE_UINT16_T      0
#define SAL_CONFIG_DEFINE_UINT32_T      0
#define SAL_CONFIG_DEFINE_UINT64_T      0
#define SAL_CONFIG_DEFINE_UINTPTR_T     0
#define SAL_CONFIG_DEFINE_INT8_T        0
#define SAL_CONFIG_DEFINE_INT16_T       0
#define SAL_CONFIG_DEFINE_INT32_T       0
#define SAL_CONFIG_DEFINE_INT64_T       0
#define SAL_CONFIG_DEFINE_PRIu32        0
#define SAL_CONFIG_DEFINE_PRId32        0
#define SAL_CONFIG_DEFINE_PRIx32        0
#define SAL_CONFIG_DEFINE_PRIu64        0
#define SAL_CONFIG_DEFINE_PRId64        0
#define SAL_CONFIG_DEFINE_PRIx64        0

#include <stdbool.h>
#define SAL_CONFIG_DEFINE_BOOL_T        0

#include <string.h>
#ifndef sal_memcmp
#define sal_memcmp memcmp
#endif
#ifndef sal_memcpy
#define sal_memcpy memcpy
#endif
#ifndef sal_memset
#define sal_memset memset
#endif
#ifndef sal_memmove
#define sal_memmove memmove
#endif
#ifndef sal_strcpy
#define sal_strcpy strcpy
#endif
#ifndef sal_strncpy
#define sal_strncpy strncpy
#endif
#ifndef sal_strlen
#define sal_strlen strlen
#endif
#ifndef sal_strcmp
#define sal_strcmp strcmp
#endif
#ifndef sal_strncmp
#define sal_strncmp strncmp
#endif
#ifndef sal_strchr
#define sal_strchr strchr
#endif
#ifndef sal_strrchr
#define sal_strrchr strrchr
#endif
#ifndef sal_strstr
#define sal_strstr strstr
#endif
#ifndef sal_strcat
#define sal_strcat strcat
#endif
#ifndef sal_strncat
#define sal_strncat strncat
#endif

#include <ctype.h>
#ifndef sal_tolower
#define sal_tolower tolower
#endif
#ifndef sal_toupper
#define sal_toupper toupper
#endif
#ifndef sal_isspace
#define sal_isspace isspace
#endif
#ifndef sal_isupper
#define sal_isupper isupper
#endif
#ifndef sal_islower
#define sal_islower islower
#endif
#ifndef sal_isalpha
#define sal_isalpha isalpha
#endif
#ifndef sal_isdigit
#define sal_isdigit isdigit
#endif
#ifndef sal_isalnum
#define sal_isalnum isalnum
#endif
#ifndef sal_isxdigit
#define sal_isxdigit isxdigit
#endif

#include <stdlib.h>
#ifndef sal_exit
#define sal_exit exit
#endif
#ifndef sal_abort
#define sal_abort abort
#endif
#ifndef sal_strtol
#define sal_strtol strtol
#endif
#ifndef sal_strtoul
#define sal_strtoul strtoul
#endif
#ifndef sal_strtoll
#define sal_strtoll strtoll
#endif
#ifndef sal_strtoull
#define sal_strtoull strtoull
#endif
#ifndef sal_atoi
#define sal_atoi atoi
#endif
#ifndef sal_abs
#define sal_abs abs
#endif
#ifndef sal_rand
#define sal_rand rand
#endif
#ifndef sal_srand
#define sal_srand srand
#endif
#ifndef sal_qsort
#define sal_qsort qsort
#endif

#include <stdio.h>
#ifndef sal_vsnprintf
#define sal_vsnprintf vsnprintf
#endif
#ifndef sal_vsprintf
#define sal_vsprintf vsprintf
#endif
#ifndef sal_snprintf
#define sal_snprintf snprintf
#endif
#ifndef sal_sprintf
#define sal_sprintf sprintf
#endif

/*!
 * \endcond
 */

#endif /* SAL_CONFIG_H */

#else

/*
 * If we get here it means that some file other than sal_config.h
 * included this file after sal_config.h already included it once.
 */
#error sal_stdc.h file cannot be included by regular source files

#endif /* SAL_STDC_H */
