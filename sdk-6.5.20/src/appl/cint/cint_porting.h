/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_porting.h
 * Purpose:     CINT porting interfaces
 */

#ifndef __CINT_PORTING_H__
#define __CINT_PORTING_H__

#include "cint_config.h"

/*
 * The following portability macros must be defined
 * before your can build the interpreter. 
 */

#if CINT_CONFIG_INCLUDE_STDLIB == 1

/*
 * Standard C Library compatibility 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef CINT_PRINTF
#define CINT_PRINTF printf
#endif

#ifndef CINT_FPRINTF
#define CINT_FPRINTF fprintf
#endif

#ifndef CINT_VPRINTF
#define CINT_VPRINTF vprintf
#endif

#ifndef CINT_MALLOC
#define CINT_MALLOC(sz) malloc(sz)
#endif

#ifndef CINT_REALLOC
#define CINT_REALLOC realloc
#endif

#ifndef CINT_FREE
#define CINT_FREE free
#endif

#ifndef CINT_STRNCPY
#define CINT_STRNCPY strncpy
#endif

#ifndef CINT_STRRCHR
#define CINT_STRRCHR strrchr
#endif

#ifndef CINT_STRCMP
#define CINT_STRCMP strcmp
#endif

#ifndef CINT_STRNCMP
#define CINT_STRNCMP strncmp
#endif

#ifndef CINT_STRNCASECMP
#define CINT_STRNCASECMP strncasecmp
#endif

#ifndef CINT_STRSTR
#define CINT_STRSTR strstr
#endif

#ifndef CINT_MEMCPY
#define CINT_MEMCPY memcpy
#endif

#ifndef CINT_MEMSET
#define CINT_MEMSET memset
#endif

#ifndef CINT_SPRINTF
#define CINT_SPRINTF sprintf
#endif

#ifndef CINT_SNPRINTF
#define CINT_SNPRINTF snprintf
#endif

#ifndef CINT_VSNPRINTF
#define CINT_VSNPRINTF vsnprintf
#endif

#ifndef CINT_VFPRINTF
#define CINT_VFPRINTF vfprintf
#endif

#ifndef CINT_FFLUSH
#define CINT_FFLUSH fflush
#endif

#ifndef CINT_STRCAT
#define CINT_STRCAT strcat
#endif

#ifndef CINT_STRNCAT
#define CINT_STRNCAT strncat
#endif

#ifndef CINT_STRDUP
#define CINT_STRDUP strdup
#endif

#ifndef CINT_STRLEN
#define CINT_STRLEN strlen
#endif

#if CINT_CONFIG_INCLUDE_PARSER_READLINE == 1
#ifndef CINT_READLINE
#include <readline/readline.h>
#define CINT_READLINE readline
#endif
#endif

#if CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY == 1
#ifndef CINT_ADD_HISTORY
#include <readline/history.h>
#define CINT_ADD_HISTORY add_history
#endif
#endif

#ifndef CINT_GETC
#define CINT_GETC getc
#endif

#ifndef CINT_FERROR
#define CINT_FERROR ferror
#endif

#if CINT_CONFIG_FILE_IO == 1

#ifndef CINT_FOPEN
#define CINT_FOPEN fopen
#endif

#ifndef CINT_FCLOSE
#define CINT_FCLOSE fclose
#endif

#ifndef CINT_FREAD
#define CINT_FREAD fread
#endif

#endif /* CINT_CONFIG_FILE_IO */

#define CINT_FATAL_ERROR(msg) do { fputs(msg, stderr); exit(2); } while(0)

#endif /* CINT_CONFIG_INCLUDE_STDLIB == 1 */



#if CINT_CONFIG_INCLUDE_SDK_SAL == 1

/*
 * Use the SDK SAL interface 
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <shared/bsl.h>
#include <sal/appl/io.h>

#ifndef CINT_PRINTF
extern int cint_printk(const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 1, 2))); 
#define CINT_PRINTF cint_printk
#endif

#ifndef CINT_FPRINTF
#define CINT_FPRINTF sal_fprintf
#endif

#ifndef CINT_VPRINTF
#define CINT_VPRINTF bsl_vprintf
#endif

#ifndef CINT_MALLOC
#define CINT_MALLOC(sz) sal_alloc(sz, "cint")
#endif

/* SAL doesn't not support realloc */

#ifndef CINT_FREE
#define CINT_FREE sal_free
#endif

#ifndef CINT_STRNCPY
#define CINT_STRNCPY sal_strncpy
#endif

#ifndef CINT_STRRCHR
#define CINT_STRRCHR strrchr
#endif

#ifndef CINT_STRCMP
#define CINT_STRCMP sal_strcmp
#endif

#ifndef CINT_STRNCMP
#define CINT_STRNCMP sal_strncmp
#endif

#ifndef CINT_STRNCASECMP
#define CINT_STRNCASECMP sal_strncasecmp
#endif

#ifndef CINT_STRSTR
#define CINT_STRSTR strstr
#endif

#ifndef CINT_MEMCPY
#define CINT_MEMCPY sal_memcpy
#endif

#ifndef CINT_MEMSET
#define CINT_MEMSET sal_memset
#endif

#ifndef CINT_SPRINTF
#define CINT_SPRINTF sal_sprintf
#endif

#ifndef CINT_SNPRINTF
#define CINT_SNPRINTF sal_snprintf
#endif

#ifndef CINT_VSNPRINTF
#define CINT_VSNPRINTF sal_vsnprintf
#endif

#ifndef CINT_VFPRINTF
#define CINT_VFPRINTF sal_vfprintf
#endif

#ifndef CINT_FFLUSH
#define CINT_FFLUSH sal_fflush
#endif

#ifndef CINT_STRCAT
#define CINT_STRCAT strcat
#endif

#ifndef CINT_STRNCAT
#define CINT_STRNCAT strncat
#endif

#ifndef CINT_STRDUP
#define CINT_STRDUP sal_strdup
#endif

#ifndef CINT_STRLEN
#define CINT_STRLEN sal_strlen
#endif

#if CINT_CONFIG_INCLUDE_PARSER_READLINE == 1
#ifndef CINT_READLINE
#ifndef INCLUDE_EDITLINE
extern char* readline(const char* p); 
#endif
#define CINT_READLINE readline
#endif
#endif

#if CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY == 1
#ifndef INCLUDE_EDITLINE
extern void add_history(char* p);
#endif 
#ifndef CINT_ADD_HISTORY 
#define CINT_ADD_HISTORY add_history
#endif
#endif

#ifndef CINT_GETC
#define CINT_GETC getc
#endif

#ifndef CINT_FERROR
#define CINT_FERROR ferror
#endif

#if CINT_CONFIG_FILE_IO == 1

#ifndef CINT_FOPEN
#define CINT_FOPEN sal_fopen
#endif

#ifndef CINT_FCLOSE
#define CINT_FCLOSE sal_fclose
#endif

#ifndef CINT_FREAD
#define CINT_FREAD sal_fread
#endif

#endif /* CINT_CONFIG_FILE_IO */

#if CINT_CONFIG_INCLUDE_POSIX_TIMER == 1

#include <sal/core/time.h>

#define CINT_TIMER_GET sal_time_usecs()

#endif /* CINT_CONFIG_INCLUDE_POSIX_TIMER */

#include <appl/diag/cmd_cint.h>
#define CINT_FATAL_ERROR(msg) cmd_cint_fatal_error(msg)

#endif /* CINT_CONFIG_INCLUDE_SDK_SAL == 1 */

/*
 * Test interfaces for CINT portability
 */
#if CINT_CONFIG_INCLUDE_STUBS == 1
#include "cint_stubs.h"
#endif /* CINT_CONFIG_INCLUDE_STUBS == 1 */

/*
 * These macros must be defined to something at this point, 
 * either as a result of CINT_CONFIG_INCLUDE_STDLIB = 1
 * or by defining them externally in your cint_customer_config.h
 */

#ifndef CINT_PRINTF
#error CINT_PRINTF macro is not defined
#endif

#ifndef CINT_VPRINTF
#error CINT_VPRINTF macro is not defined
#endif

#ifndef CINT_MALLOC
#error CINT_MALLOC macro is not defined
#endif

#ifndef CINT_FREE
#error CINT_FREE macro is not defined
#endif

#ifndef CINT_STRRCHR
#error CINT_STRRCHR macro is not defined
#endif

#ifndef CINT_STRCMP
#error CINT_STRCMP macro is not defined
#endif

#ifndef CINT_STRSTR
#error CINT_STRSTR macro is not defined
#endif

#ifndef CINT_MEMCPY
#error CINT_MEMCPY macro is not defined
#endif

#ifndef CINT_MEMSET
#error CINT_MEMSET macro is not defined
#endif

#ifndef CINT_SPRINTF
#error CINT_SPRINTF macro is not defined
#endif

#ifndef CINT_SNPRINTF
#error CINT_SNPRINTF macro is not defined
#endif

#ifndef CINT_VSNPRINTF
#error CINT_VSNPRINTF macro is not defined
#endif

#ifndef CINT_STRCAT
#error CINT_STRCAT macro is not defined
#endif

#ifndef CINT_STRDUP
#error CINT_STRDUP macro is not defined
#endif

#ifndef CINT_STRLEN
#error CINT_STRLEN macro is not defined
#endif

#if CINT_CONFIG_INCLUDE_FILE_IO == 1

#ifndef CINT_FOPEN
#error CINT_FOPEN macro is not defined
#endif

#ifndef CINT_FCLOSE
#error CINT_FCLOSE macro is not defined
#endif

#ifndef CINT_FREAD
#error CINT_FREAD macro is not defined
#endif

#endif /* CINT_CONFIG_INCLUDE_FILE_IO */

#ifndef COMPILER_ATTRIBUTE
#define COMPILER_ATTRIBUTE(_a) __attribute__ (_a)
#endif

#endif /* __CINT_PORTING_H__ */




