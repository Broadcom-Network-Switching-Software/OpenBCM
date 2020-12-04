/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * Check for basic source code architecture violations.
 *
 * Note that if this file is included multiple times, it must
 * be propcessed for errors every time.
 */

/*
 * Default source check configuration
 */

#ifndef SCHK_SAL_APPL_HEADERS_IN_SOC
#define SCHK_SAL_APPL_HEADERS_IN_SOC    1
#endif
#ifndef SCHK_BCM_HEADERS_IN_SOC
#define SCHK_BCM_HEADERS_IN_SOC         1
#endif
#ifndef SCHK_PRINTK_IN_SOC
#define SCHK_PRINTK_IN_SOC              1
#endif
#ifndef SCHK_SAL_APPL_HEADERS_IN_BCM
#define SCHK_SAL_APPL_HEADERS_IN_BCM    1
#endif
#ifndef SCHK_PRINTK_IN_BCM
#define SCHK_PRINTK_IN_BCM              1
#endif
#ifndef SCHK_SOC_CM_PRINT_IN_APPL
#define SCHK_SOC_CM_PRINT_IN_APPL       0
#endif
#ifndef SCHK_SOC_CM_DEBUG_IN_APPL
#define SCHK_SOC_CM_DEBUG_IN_APPL       0
#endif
#ifndef SCHK_SSCANF_IN_ANY
#define SCHK_SSCANF_IN_ANY              1
#endif
#ifndef SCHK_FSCANF_IN_ANY
#define SCHK_FSCANF_IN_ANY              1
#endif

/* Help macros */
#if defined(_SAL_CONFIG_H) || defined(_SAL_PCI_H) || \
    defined(_SAL_IO_H) || defined(_SAL_H)
#ifndef _SAL_APPL_HDRS
#define _SAL_APPL_HDRS
#endif
#endif

#if defined(SOURCE_CHECK_LIBSOC)

#if SCHK_SAL_APPL_HEADERS_IN_SOC
#if defined(_SAL_APPL_HDRS)
#error SAL_APPL_headers_included_from_SOC_sources
#endif
#endif

#if SCHK_BCM_HEADERS_IN_SOC
#if defined(__BCM_TYPES_H__)
#error BCM_headers_included_from_SOC_sources
#endif
#endif

#if SCHK_PRINTK_IN_SOC
#ifndef printk
#define printk          DO_NOT_USE_printk_in_SOC
#endif
#endif

#endif /* defined(SOURCE_CHECK_LIBSOC) */


#if defined(SOURCE_CHECK_LIBBCM)

#if SCHK_SAL_APPL_HEADERS_IN_BCM
#if defined(_SAL_APPL_HDRS)
#error SAL_APPL_headers_included_from_BCM_sources
#endif
#endif

#if SCHK_PRINTK_IN_BCM
#ifndef printk
#define printk          DO_NOT_USE_printk_in_BCM
#endif
#endif

#endif /* defined(SOURCE_CHECK_LIBBCM) */


#if defined(SOURCE_CHECK_LIBAPPL) && 0

#if SCHK_SOC_CM_PRINT_IN_APPL
#ifndef soc_cm_print
#define soc_cm_print    DO_NOT_USE_soc_cm_print_in_APPL
#endif
#endif

#if SCHK_SOC_CM_DEBUG_IN_APPL
#ifndef soc_cm_debug
#define soc_cm_debug    DO_NOT_USE_soc_cm_debug_in_APPL
#endif
#endif

#endif /* defined(SOURCE_CHECK_LIBAPPL) */


/*
 * Checks that apply to all source files
 */

#if defined(SOURCE_CHECK_LIBSOC) || \
    defined(SOURCE_CHECK_LIBBCM) || \
    defined(SOURCE_CHECK_LIBAPPL)

#if SCHK_SSCANF_IN_ANY
#ifndef sscanf
#define sscanf          DO_NOT_USE_sscanf
#endif
#endif

#if SCHK_FSCANF_IN_ANY
#ifndef fscanf
#define fscanf          DO_NOT_USE_fscanf
#endif
#endif

#endif
