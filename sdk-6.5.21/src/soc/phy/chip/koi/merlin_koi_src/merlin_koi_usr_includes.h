/**************************************************************************************
 *  File Name     :  merlin_koi_usr_includes.h                                        *
 *  Created On    :  05/07/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Header file which includes all required std libraries and macros *
 *  Revision      :  $Id: merlin_koi_usr_includes.h 996 2015-03-30 22:39:03Z kirand $ *
 *                                                                                    *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                              *
 *  No portions of this material may be reproduced in any form without                *
 *  the written permission of:                                                        *
 *      Broadcom Corporation                                                          *
 *      5300 California Avenue                                                        *
 *      Irvine, CA  92617                                                             *
 *                                                                                    *
 *  All information contained in this document is Broadcom Corporation                *
 *  company private proprietary, and trade secret.                                    *
 */

/** @file merlin_koi_usr_includes.h
 * Header file which includes all required std libraries and macros
 */

/* The user is expected to replace the macro definitions with their required implementation */

#ifndef MERLIN_KOI_API_USR_INCLUDES_H
#define MERLIN_KOI_API_USR_INCLUDES_H

/* Standard libraries that can be replaced by your custom libraries */
#ifdef _MSC_VER
/* Enclose all standard headers in a pragma to remove warings for MS compiler */
#pragma warning( push, 0 )
#endif
#ifdef NON_SDK
#include <stdint.h>
#include <string.h>
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif
#include <shared/bsl.h>

#define USR_PRINTF(paren_arg_list)    LOG_CLI(paren_arg_list)
/* Redefine macros according your compiler requirements */
#if defined(NON_SDK) || defined(NO_SAL_APPL) || defined(RTOS_STRINGS)
#define USR_MEMSET(mem, val, num)     memset(mem, val, num)
#define USR_STRLEN(string)            strlen(string)
#define USR_STRNCAT(str1, str2)       strcat(str1, str2)
#define USR_STRCPY(str1, str2)        strcpy(str1, str2)
#define USR_STRCMP(str1, str2)        strcmp(str1, str2)
#define USR_STRNCMP(str1, str2, num)  strncmp(str1, str2, num)

#ifdef SERDES_API_FLOATING_POINT
#define USR_DOUBLE                    double
#else
#define USR_DOUBLE       int
#define double       undefined
#define float        undefined
#endif

#else /* defined(NON_SDK) || defined(NO_SAL_APPL) || defined(RTOS_STRINGS) */

#define USR_MEMSET(mem, val, num)    sal_memset((mem), (val), (num))
#define USR_STRLEN(string)           sal_strlen(string)
#define USR_STRNCAT(str1, str2)      sal_strcat(str1, str2)
#define USR_STRCPY(str1, str2)       sal_strcpy(str1, str2)
#define USR_STRCMP(str1, str2)       sal_strcmp(str1, str2)
#define USR_STRNCMP(str1, str2, num) sal_strncmp(str1, str2, num)
#ifdef SERDES_API_FLOATING_POINT
#define USR_DOUBLE                    double
#else
#define USR_DOUBLE       int

#if 0
#define double       undefined
#define float        undefined
#endif

#ifndef UINT16_MAX
#define UINT16_MAX                    0xFFFFU
#endif
#ifndef UINT32_MAX
#define UINT32_MAX                    0xFFFFFFFFU
#endif
#define USR_UNUSED_VAR(x)             (void)(x)

#endif
#endif /* defined(NON_SDK) || defined(NO_SAL_APPL) || defined(RTOS_STRINGS) */

#endif
