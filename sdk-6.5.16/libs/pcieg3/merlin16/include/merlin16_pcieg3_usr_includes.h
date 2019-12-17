/**************************************************************************************
 *  File Name     :  merlin16_pcieg3_usr_includes.h                                        *
 *  Created On    :  05/07/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Header file which includes all required std libraries and macros *
 *  Revision      :   *
 *                                                                                    *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.                                                              *
 *  No portions of this material may be reproduced in any form without                *
 *  the written permission of:                                                        *
 *      Broadcom Corporation                                                          *
 *      5300 California Avenue                                                        *
 *      Irvine, CA  92617                                                             *
 *                                                                                    *
 *  All information contained in this document is Broadcom Corporation                *
 *  company private proprietary, and trade secret.                                    *
 */

/** @file merlin16_pcieg3_usr_includes.h
 * Header file which includes all required std libraries and macros
 */

/* The user is expected to replace the macro definitions with their required implementation */

#ifndef MERLIN16_PCIEG3_API_USR_INCLUDES_H
#define MERLIN16_PCIEG3_API_USR_INCLUDES_H

/* Standard libraries that can be replaced by your custom libraries */
#ifdef _MSC_VER
/* Enclose all standard headers in a pragma to remove warings for MS compiler */
#pragma warning( push, 0 )
#endif
#ifdef NON_SDK
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#else
#include <shared/bsl.h>
#include <sal/core/libc.h>

#ifdef __KERNEL__
typedef unsigned long int       uintptr_t;
#endif
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif

/* Redefine macros according your compiler requirements */
#define USR_PRINTF(paren_arg_list)    bsl_printf paren_arg_list
#define USR_MEMSET(mem, val, num)     sal_memset(mem, val, num)
#define USR_STRLEN(string)            sal_strlen(string)
#define USR_STRNCAT(str1, str2, num)  sal_strncat(str1, str2, num)
#define USR_STRCPY(str1, str2)        sal_strcpy(str1, str2)
#define USR_STRCMP(str1, str2)        sal_strcmp(str1, str2)
#define USR_STRNCMP(str1, str2, num)  sal_strncmp(str1, str2, num)
#define USR_SPRINTF(paren_arg_list)   ((void)sal_sprintf paren_arg_list)
#define USR_UINTPTR                   uintptr_t

#ifdef SERDES_API_FLOATING_POINT
#define USR_DOUBLE                    double
#else
#define USR_DOUBLE       int
#define double       undefined
#define float        undefined
#endif

/* Implementation specific macros below */
#ifdef SRDS_API_ALL_FUNCTIONS_HAVE_ACCESS_STRUCT
#define USR_DELAY_MS(stuff) merlin16_pcieg3_delay_ms(sa__,stuff)
#define USR_DELAY_US(stuff) merlin16_pcieg3_delay_us(sa__,stuff)
#define USR_DELAY_NS(stuff) merlin16_pcieg3_delay_ns(sa__,stuff)
#else
#define USR_DELAY_MS(stuff) merlin16_pcieg3_delay_ms(stuff)
#define USR_DELAY_US(stuff) merlin16_pcieg3_delay_us(stuff)
#define USR_DELAY_NS(stuff) merlin16_pcieg3_delay_ns(stuff)
#endif

#ifndef UINT16_MAX
#define UINT16_MAX                    0xFFFFU
#endif
#ifndef UINT32_MAX
#define UINT32_MAX                    0xFFFFFFFFU
#endif

#endif
