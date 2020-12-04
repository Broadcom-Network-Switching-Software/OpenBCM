/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/**************************************************************************************
 *  File Name     :  falcon16_tsc_usr_includes.h                                        *
 *  Created On    :  05/07/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Header file which includes all required std libraries and macros *
 *  Revision      :   *
 */

/** @file falcon16_tsc_usr_includes.h
 * Header file which includes all required std libraries and macros
 */

/* The user is expected to replace the macro definitions with their required implementation */

#ifndef FALCON16_TSC_API_USR_INCLUDES_H
#define FALCON16_TSC_API_USR_INCLUDES_H

/* Standard libraries that can be replaced by your custom libraries */
#ifdef _MSC_VER
/* Enclose all standard headers in a pragma to remove warings for MS compiler */
#pragma warning( push, 0 )
#endif
#ifdef NON_SDK
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#if defined _MSC_VER
#define API_FUNCTION_NAME __FUNCTION__
#else
#define API_FUNCTION_NAME __func__
#endif

/* Redefine macros according your compiler requirements */
#ifdef NON_SDK
#define USR_PRINTF(paren_arg_list)    ((void)usr_logger_write paren_arg_list)
#define USR_VPRINTF(paren_arg_list)   ((void)usr_logger_verbose_write paren_arg_list)
#define USR_CSVPRINTF(paren_arg_list) ((void)usr_logger_csv_write paren_arg_list)

#define USR_MEMSET(mem, val, num)     memset(mem, val, num)
#define USR_STRLEN(string)            strlen(string)
#define USR_STRNCAT(str1, str2, num)  strncat(str1, str2, num)
#define USR_STRCPY(str1, str2)        strcpy(str1, str2)
#define USR_STRCMP(str1, str2)        strcmp(str1, str2)
#define USR_STRNCMP(str1, str2, num)  strncmp(str1, str2, num)
#ifndef NO_VARIADIC_MACROS
#define USR_SPRINTF(...)   (void)sprintf (__VA_ARGS__)
#endif
#define USR_UINTPTR                   uintptr_t

#ifdef SERDES_API_FLOATING_POINT
#define USR_DOUBLE                    double
#else
#define USR_DOUBLE       int
#define double       undefined
#define float        undefined
#endif

/* Syncronization macro Definitions */
#ifndef NO_VARIADIC_MACROS
#define USR_CREATE_LOCK
#define USR_ACQUIRE_LOCK
#define USR_RELEASE_LOCK
#define USR_DESTROY_LOCK
#endif

/* Implementation specific macros below */
#ifdef SRDS_API_ALL_FUNCTIONS_HAVE_ACCESS_STRUCT
# ifndef NO_VARIADIC_MACROS
#define usr_logger_write(...) logger_write(sa__, -1,__VA_ARGS__)
# endif
#define USR_DELAY_MS(stuff) falcon16_tsc_delay_ms(sa__,stuff)
#define USR_DELAY_US(stuff) falcon16_tsc_delay_us(sa__,stuff)
#define USR_DELAY_NS(stuff) falcon16_tsc_delay_ns(sa__,stuff)
#else
# ifndef NO_VARIADIC_MACROS
#define usr_logger_write(...)         logger_write(0,__VA_ARGS__)
#define usr_logger_verbose_write(...) logger_write(1,__VA_ARGS__)
#define usr_logger_csv_write(...)     csv_write(__VA_ARGS__)
# endif
#define USR_DELAY_MS(stuff) falcon16_tsc_delay_ms(stuff)
#define USR_DELAY_US(stuff) falcon16_tsc_delay_us(stuff)
#define USR_DELAY_NS(stuff) falcon16_tsc_delay_ns(stuff)
#endif

#else

#define USR_PRINTF(stuff)             PHYMOD_DEBUG_ERROR(stuff)
#define USR_MEMSET(mem, val, num)     PHYMOD_MEMSET(mem, val, num)
#define USR_STRLEN(string)            PHYMOD_STRLEN(string)
#define USR_STRCAT(str1, str2)        PHYMOD_STRCAT(str1, str2)
#define USR_STRNCAT(str1, str2, num)  PHYMOD_STRNCAT(str1, str2, num)
#define USR_STRCPY(str1, str2)        PHYMOD_STRCPY(str1, str2)
#define USR_STRCMP(str1, str2)        PHYMOD_STRCMP(str1, str2)
#define USR_STRNCMP(str1, str2, num)  PHYMOD_STRNCMP(str1, str2, num)
#define USR_SPRINTF                   PHYMOD_SPRINTF
#define USR_ACQUIRE_LOCK
#define USR_RELEASE_LOCK

#define USR_DELAY_MS(var) falcon16_tsc_delay_ms(var)
#define USR_DELAY_US(var) falcon16_tsc_delay_us(var)
#define USR_DELAY_NS(var) falcon16_tsc_delay_ns(var)

#ifdef SERDES_API_FLOATING_POINT
#define USR_DOUBLE                    double
#else
#define USR_DOUBLE       int
#define double       undefined
#define float        undefined
#endif

#endif

#ifndef UINT16_MAX
#define UINT16_MAX                    0xFFFFU
#endif
#ifndef UINT32_MAX
#define UINT32_MAX                    0xFFFFFFFFU
#endif

typedef unsigned long int       uintptr_t;

#define USR_UINTPTR    uintptr_t

#endif
