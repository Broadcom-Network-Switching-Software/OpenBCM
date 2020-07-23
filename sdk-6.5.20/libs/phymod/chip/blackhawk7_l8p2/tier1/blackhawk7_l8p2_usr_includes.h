/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/**************************************************************************************
 *  File Name     :  blackhawk7_l8p2_usr_includes.h                                        *
 *  Created On    :  05/07/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Header file which includes all required std libraries and macros *
 *  Revision      :   *
 */

/** @file blackhawk7_l8p2_usr_includes.h
 * Header file which includes all required std libraries and macros
 */

/* The user is expected to replace the macro definitions with their required implementation */

#ifndef BLACKHAWK7_L8P2_API_USR_INCLUDES_H
#define BLACKHAWK7_L8P2_API_USR_INCLUDES_H

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
#ifndef NO_VARIADIC_MACROS
#define USR_PRINTF(paren_arg_list)    ((void)usr_logger_write paren_arg_list)
#else
#define USR_PRINTF(paren_arg_list)    
#endif /* NO_VARIADIC_MACROS */

#define USR_MEMSET(mem, val, num)     memset(mem, val, num)
#define USR_STRLEN(string)            strlen(string)
#define USR_STRNCAT(str1, str2, num)  strncat(str1, str2, num)
#define USR_STRCPY(str1, str2)        strcpy(str1, str2)
#define USR_STRCMP(str1, str2)        strcmp(str1, str2)
#define USR_STRNCMP(str1, str2, num)  strncmp(str1, str2, num)
#define USR_SNPRINTF                  snprintf
#define USR_CALLOC(nmemb, size)       calloc(nmemb, size)
#define USR_FREE(ptr)                 free(ptr)
#define USR_UINTPTR                   uintptr_t


#else /* NON_SDK */

#include <phymod/phymod.h>

#define USR_PRINTF(stuff)             PHYMOD_DEBUG_ERROR(stuff)
#define USR_MEMSET(mem, val, num)     PHYMOD_MEMSET(mem, val, num)
#define USR_STRLEN(string)            PHYMOD_STRLEN(string)
#define USR_STRCAT(str1, str2)        PHYMOD_STRCAT(str1, str2)
#define USR_STRNCAT(str1, str2, num)  PHYMOD_STRNCAT(str1, str2, num)
#define USR_STRCPY(str1, str2)        PHYMOD_STRCPY(str1, str2)
#define USR_STRNCPY(str1, str2, num)  PHYMOD_STRNCPY(str1, str2, num)
#define USR_STRCMP(str1, str2)        PHYMOD_STRCMP(str1, str2)
#define USR_STRNCMP(str1, str2, num)  PHYMOD_STRNCMP(str1, str2, num)
#define USR_SPRINTF                   PHYMOD_SPRINTF
#define USR_ACQUIRE_LOCK
#define USR_RELEASE_LOCK
#define USR_CREATE_LOCK
#define USR_CALLOC(memb, size)        PHYMOD_MALLOC(memb * size, " ")
#define USR_FREE                      PHYMOD_FREE
#define USR_SNPRINTF                  PHYMOD_SNPRINTF

typedef unsigned long int       uintptr_t;

#endif /* NON_SDK */

#define USR_UINTPTR     uintptr_t

#ifdef SERDES_API_FLOATING_POINT
#define USR_DOUBLE      double
#else
#define USR_DOUBLE      int
#define double          undefined
#define float           undefined
#endif

#ifndef UINT16_MAX
#define UINT16_MAX      0xFFFFU
#endif

/* Mutex Definitions */
#ifndef NO_VARIADIC_MACROS
#define USR_CREATE_LOCK 
#define USR_ACQUIRE_LOCK 
#define USR_RELEASE_LOCK 
#define USR_DESTROY_LOCK
#endif

/* Implementation specific macros below */
#ifndef NO_VARIADIC_MACROS
/* logger_write() is an API user provided Dependency function. In practice it's return value is
 * never checked. To do so, the following define can be used. If using one of the API error checking
 * macros (like EFUN()) which calls the API error processing functons, the
 * ERR_CODE_SRDS_REG_ACCESS_FAIL error code should be returned to avoid error processing recursion.
 * The (void) cast serves to quiet compiler warnings if so configured, otherwise it can be removed.
 *  #define usr_logger_write(...)  ((void)(logger_write(0, -1, __VA_ARGS__) ? ERR_CODE_SRDS_REG_ACCESS_FAIL : ERR_CODE_NONE))
 */
#define usr_logger_write(...) logger_write(sa__, -1,__VA_ARGS__)

/* usr_logger_verbose_write() appears broken (no 'sa__' arg), is likely not being used, and may be removed. */
#define usr_logger_verbose_write(...) logger_write(1,__VA_ARGS__)
#endif
#define USR_DELAY_MS(stuff) blackhawk7_l8p2_acc_delay_ms(sa__,stuff)
#define USR_DELAY_US(stuff) blackhawk7_l8p2_acc_delay_us(sa__,stuff)
#define USR_DELAY_NS(stuff) blackhawk7_l8p2_acc_delay_ns(sa__,stuff)

#endif
