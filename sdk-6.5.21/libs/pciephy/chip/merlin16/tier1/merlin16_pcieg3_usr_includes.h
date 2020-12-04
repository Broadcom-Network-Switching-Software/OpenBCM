/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/**************************************************************************************
 *  File Name     :  merlin16_pcieg3_usr_includes.h                                        *
 *  Created On    :  05/07/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Header file which includes all required std libraries and macros *
 *  Revision      :   *
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

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#if defined _MSC_VER
#define API_FUNCTION_NAME __FUNCTION__
#else
#define API_FUNCTION_NAME __func__
#endif

/* Redefine macros according your compiler requirements */
#include <pciephy/pciephy.h>
#define USR_PRINTF(paren_arg_list)    (PCIEPHY_DIAG_OUT paren_arg_list)
#define USR_MEMSET(mem, val, num)     PCIEPHY_MEMSET(mem, val, num)
#define USR_STRLEN(string)            PCIEPHY_STRLEN(string)
#define USR_STRNCAT(str1, str2, num)  PCIEPHY_STRNCAT(str1, str2, num)
#define USR_STRCPY(str1, str2)        PCIEPHY_STRCPY(str1, str2)
#define USR_STRCMP(str1, str2)        PCIEPHY_STRCMP(str1, str2)
#define USR_STRNCMP(str1, str2, num)  PCIEPHY_STRNCMP(str1, str2, num)
#define USR_SPRINTF(paren_arg_list)   (PCIEPHY_SPRINTF paren_arg_list)
#define USR_UINTPTR                   uintptr_t

#ifdef SERDES_API_FLOATING_POINT
#define USR_DOUBLE                    double
#else
#define USR_DOUBLE       int
#define double       undefined
#define float        undefined
#endif

/* Syncronization macro Definitions */
#define USR_CREATE_LOCK 
#define USR_ACQUIRE_LOCK 
#define USR_RELEASE_LOCK 
#define USR_DESTROY_LOCK

/* Implementation specific macros below */
#ifdef SRDS_API_ALL_FUNCTIONS_HAVE_ACCESS_STRUCT
# ifndef NO_VARIADIC_MACROS
#define usr_logger_write(...) logger_write(sa__, -1,__VA_ARGS__)
# endif
#define USR_DELAY_MS(stuff) merlin16_pcieg3_delay_ms(sa__,stuff)
#define USR_DELAY_US(stuff) merlin16_pcieg3_delay_us(sa__,stuff)
#define USR_DELAY_NS(stuff) merlin16_pcieg3_delay_ns(sa__,stuff)
#else
# ifndef NO_VARIADIC_MACROS
#define usr_logger_write(...)         logger_write(0,__VA_ARGS__)
#define usr_logger_verbose_write(...) logger_write(1,__VA_ARGS__)
#define usr_logger_csv_write(...)     csv_write(__VA_ARGS__)
# endif
#define USR_DELAY_MS(stuff) merlin16_pcieg3_delay_ms(stuff)
#define USR_DELAY_US(stuff) merlin16_pcieg3_delay_us(stuff)
#define USR_DELAY_NS(stuff) merlin16_pcieg3_delay_ns(stuff)
#endif

#endif
