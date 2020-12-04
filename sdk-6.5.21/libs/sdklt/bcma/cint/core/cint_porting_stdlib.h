/*
 * $Id: cint_porting_stdlib.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_porting_stdlib.h
 * Purpose:     CINT porting stdlib interfaces
 */

#ifndef CINT_PORTING_STDLIB_H
#define CINT_PORTING_STDLIB_H

#include "cint_config.h"

/*
 * The following portability macros must be defined
 * before your can build the interpreter.
 */

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
#ifndef CINT_READLINE_FREE
#define CINT_READLINE_FREE free
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

#ifndef CINT_FATAL_ERROR
#define CINT_FATAL_ERROR(msg) do { fputs(msg, stderr); exit(2); } while(0)
#endif

#endif /* CINT_PORTING_STDLIB_H */

