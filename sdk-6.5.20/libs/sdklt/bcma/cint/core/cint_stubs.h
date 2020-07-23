/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_stubs.h
 * Purpose:     stubs for CINT portability interfaces
 */

#ifndef   _CINT_STUBS_H_
#define   _CINT_STUBS_H_

#ifndef CINT_CONFIG_INCLUDE_STUBS
#define CINT_CONFIG_INCLUDE_STUBS 0
#endif

#if CINT_CONFIG_INCLUDE_STUBS == 1
#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <stdarg.h>

extern int cint_stub_printf(const char *format, ...);
extern int cint_stub_vprintf(const char *format, va_list ap);
extern void *cint_stub_malloc(size_t size);
extern void *cint_stub_realloc(void *ptr, size_t size);
extern void cint_stub_free(void *ptr);
extern char *cint_stub_strrchr(const char *s, int c);
extern int cint_stub_strcmp(const char *s1, const char *s2);
extern char *cint_stub_strstr(const char *s1, const char *s2);
extern void *cint_stub_memcpy(void *dest, const void *src, size_t n);
extern void *cint_stub_memset(void *s, int c, size_t n);
extern int cint_stub_sprintf(char *str, const char *format, ...);
extern int cint_stub_snprintf(char *str, size_t size, const char *format, ...);
extern int cint_stub_vsnprintf(char *str, size_t size, const char *format, va_list ap);
extern char *cint_stub_strcat(char *dest, const char *src);
extern char *cint_stub_strdup(const char *s);
extern size_t cint_stub_strlen(const char *s);
extern char *cint_stub_strncpy(char *dest, const char *src, size_t n);
extern void cint_stub_fatal_error(const char *msg);
extern size_t cint_stub_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern int cint_stub_ferror(FILE *stream);
extern int cint_stub_getc(FILE *stream);
extern int cint_stub_ferror(FILE *stream);
extern FILE *cint_stub_fopen(const char *path, const char *mode);
extern int cint_stub_fclose(FILE *stream);


#define CINT_PRINTF cint_stub_printf
#define CINT_VPRINTF cint_stub_vprintf
#define CINT_MALLOC(sz) cint_stub_malloc(sz)
#define CINT_FREE cint_stub_free
#define CINT_STRRCHR cint_stub_strrchr
#define CINT_STRCMP cint_stub_strcmp
#define CINT_STRSTR cint_stub_strstr
#define CINT_MEMCPY cint_stub_memcpy
#define CINT_MEMSET cint_stub_memset
#define CINT_SPRINTF cint_stub_sprintf
#define CINT_SNPRINTF cint_stub_snprintf
#define CINT_VSNPRINTF cint_stub_vsnprintf
#define CINT_STRCAT cint_stub_strcat
#define CINT_STRDUP cint_stub_strdup
#define CINT_STRLEN cint_stub_strlen

#define CINT_STRNCPY cint_stub_strncpy
#define CINT_FATAL_ERROR(msg) cint_stub_fatal_error(msg);

#define CINT_FERROR cint_stub_ferror
#define CINT_GETC cint_stub_getc

#ifndef CINT_CONFIG_FILE_IO
#define CINT_CONFIG_FILE_IO 0
#endif

#ifndef CINT_CONFIG_INCLUDE_PARSER_READLINE
#define CINT_CONFIG_INCLUDE_PARSER_READLINE 0
#endif

#ifndef CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY
#define CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY 0
#endif

/* File IO interfaces */
#if CINT_CONFIG_FILE_IO == 1
#define CINT_FREAD cint_stub_fread

#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
#define CINT_FOPEN cint_stub_fopen
#define CINT_FCLOSE cint_stub_fclose
#endif /* CINT_CONFIG_INCLUDE_XINCLUDE */

#endif /* CINT_CONFIG_FILE_IO */

#if CINT_CONFIG_INCLUDE_PARSER_READLINE == 1
#ifndef CINT_READLINE
extern char* readline(const char* p);
#define CINT_READLINE readline
#endif
#endif

#if CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY == 1
extern void add_history(char* p);
#ifndef CINT_ADD_HISTORY
#define CINT_ADD_HISTORY add_history
#endif
#endif

#endif /* CINT_CONFIG_INCLUDE_STUBS */
#endif /* _CINT_STUBS_H_ */
