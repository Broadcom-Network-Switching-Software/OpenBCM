/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_stubs.c
 * Purpose:     portability stub definitions
 */

#include "cint_stubs.h"

#if CINT_CONFIG_INCLUDE_STUBS == 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
cint_stub_printf(const char *format, ...)
{
    int rv;

    va_list ap;
    va_start(ap, format);
    rv = cint_stub_vprintf(format, ap);
    va_end(ap);

    return rv;
}

int
cint_stub_vprintf(const char *format, va_list ap)
{
    return vprintf(format, ap);
}

void *
cint_stub_malloc(size_t size)
{
    return malloc(size);
}

void *
cint_stub_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void
cint_stub_free(void *ptr)
{
    free(ptr);
}

char *
cint_stub_strrchr(const char *s, int c)
{
    return strrchr(s,c);
}

int
cint_stub_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

char *
cint_stub_strstr(const char *s1, const char *s2)
{
    return strstr(s1, s2);
}

void *
cint_stub_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest, src, n);
}

void *
cint_stub_memset(void *s, int c, size_t n)
{
    return memset(s, c, n);
}

int
cint_stub_sprintf(char *str, const char *format, ...)
{
    int rv;

    va_list ap;
    va_start(ap, format);
    rv = vsprintf(str, format, ap);
    va_end(ap);

    return rv;
}

int
cint_stub_snprintf(char *str, size_t size, const char *format, ...)
{
    int rv;

    va_list ap;
    va_start(ap, format);
    rv = cint_stub_vsnprintf(str, size, format, ap);
    va_end(ap);

    return rv;
}

int
cint_stub_vsnprintf(char *str, size_t size,
                    const char *format, va_list ap)
{
    return vsnprintf(str, size, format, ap);
}

char *
cint_stub_strcat(char *dest, const char *src)
{
    return strcat(dest, src);
}

char *
cint_stub_strdup(const char *s)
{
    return strdup(s);
}

size_t
cint_stub_strlen(const char *s)
{
    return strlen(s);
}

char *
cint_stub_strncpy(char *dest, const char *src, size_t n)
{
    return strncpy(dest, src, n);
}

void
cint_stub_fatal_error(const char *msg)
{
    fputs(msg, stderr);
    exit(2);
}

FILE *
cint_stub_fopen(const char *path, const char *mode)
{
    return fopen(path, mode);
}

int
cint_stub_fclose(FILE *stream)
{
    return fclose(stream);
}

size_t
cint_stub_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fread(ptr, size, nmemb, stream);
}

int
cint_stub_ferror(FILE *stream)
{
    return ferror(stream);
}

int
cint_stub_getc(FILE *stream)
{
    return getc(stream);
}

#endif /* CINT_CONFIG_CINT_STUBS */
