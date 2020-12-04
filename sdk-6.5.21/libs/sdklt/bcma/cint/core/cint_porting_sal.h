/*
 * $Id: cint_porting_sal.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_porting_sal.h
 * Purpose:     CINT porting SAL interfaces
 */

#ifndef CINT_PORTING_SAL_H
#define CINT_PORTING_SAL_H

#include "cint_config.h"

/*
 * The following portability macros must be defined
 * before your can build the interpreter.
 */

#ifndef COMPILER_ATTRIBUTE
#define COMPILER_ATTRIBUTE(_a) __attribute__ (_a)
#endif

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

#ifndef CINT_STRDUP
#define CINT_STRDUP sal_strdup
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

#ifndef CINT_FATAL_ERROR
#include <appl/diag/cmd_cint.h>
#define CINT_FATAL_ERROR(msg) cmd_cint_fatal_error(msg)
#endif

#endif /* CINT_PORTING_SAL_H */

