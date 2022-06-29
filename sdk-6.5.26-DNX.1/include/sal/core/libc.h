/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: 	libc.h
 * Purpose: 	Some C library functions to remove dependencies
 *		of the driver on external libraries.
 *
 * The compile-time flag -DRTOS_STRINGS should be used if your RTOS
 * supports the standard <string.h> routines.  The RTOS library or
 * built-in versions of these routines are likely to be much more
 * efficient than the stand-in versions below.
 */

#ifndef _SAL_LIBC_H
#define _SAL_LIBC_H
/*
 * Note: <stddef.h> is required due to references to size_t
 * in this file. 
 * <ctype.h> is required for code including <sal/appl/sal.h>
 * and directly calling APIs in C library.
 */
#include <stddef.h>
#include <ctype.h>

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/kernel.h>
#endif

#include <sal/types.h>
#include <stdarg.h>

#ifndef NULL
#define	NULL	0
#endif

#define sal_getpid getpid
#define sal_kill kill

/**
 * \brief
 * Flags for \ref sal_alloc_internal
 * {
 */
/*
 * Allocate the memory with malloc, without any special treatment.
 */
#define SAL_ALLOC_F_NONE              0
/*
 * Allocate the memory with calloc, so it will be zeroed.
 */
#define SAL_ALLOC_F_ZERO              0x01
/*
 * Add the amount of allocated/freed memory to a counter to ignore this amount for memory testing purposes.
 */
#define SAL_ALLOC_F_IGNORE_COUNTERS   0x02

#if defined(UNIX) || defined(__KERNEL__) || \
    defined(LINUX)
/* { */
/*
 * Use RTOS string operations only if it is not blocked by
 * USE_CUSTOM_STR_UTILS.
 */
#if !defined(USE_CUSTOM_STR_UTILS)
/* { */
#define RTOS_STRINGS
/* } */
#endif
/* } */
#endif

#if defined(RTOS_STRINGS)
/* { */
#if defined(__KERNEL__)
/* { */
#include <linux/string.h>
/* } */
#else
/* { */
#include <string.h>
#include <strings.h>
/* } */
#endif
/* } */
#else
/* { */
#if defined(USE_CUSTOM_STR_UTILS)
/* { */
/*
 * Even when using RTOS_STRINGS, still keep the option of
 * invoking, directly, the standard c-runtime library. This
 * is so because not all code uses 'sal_..." procedures (yet).
 */
#if defined(__KERNEL__)
/* { */
#include <linux/string.h>
/* } */
#else
/* { */
#include <string.h>
/* } */
#endif
/* } */
#endif
/* } */
#endif

#ifdef RTOS_STRINGS
/* { */

#define sal_strlen              strlen
#define sal_strcpy              strcpy
#define sal_strncpy             strncpy
#define sal_strcmp              strcmp
#define sal_strstr              strstr
#define sal_strcat              strcat
#define sal_strncat             strncat
#define sal_strncmp             strncmp
#define sal_strchr              strchr
#define sal_strrchr             strrchr
#define sal_strspn              strspn
#define sal_strtok              strtok
#define sal_strtoul             strtoul
#define sal_strtol              strtol
#define sal_toupper             toupper
#define sal_tolower             tolower
#define sal_atoi                atoi
#define sal_isspace             isspace
#define sal_islower             islower

#define sal_abort               abort 
#define sal_fgets               fgets 
#define sal_fgetc               fgetc 
#define sal_fputs               fputs 
#define sal_fputc               fputc 
#define sal_fwrite              fwrite 
#define sal_fprintf             fprintf 
#define sal_fflush              fflush 
#define sal_vfprintf            vfprintf 

#define sal_strcasecmp          strcasecmp
#define sal_strncasecmp         strncasecmp

#if defined(memcpy)
void *sal_memcpy_wrapper(void *, const void *, size_t);
#define sal_memcpy		sal_memcpy_wrapper
#else
#define sal_memcpy		memcpy
#endif

#define sal_memset      memset
#define sal_memmove     memmove

/* } */
#else /* !RTOS_STRINGS */
/* { */
extern int sal_strlen(const char *s) SAL_ATTR_WEAK;
extern char *sal_strcpy(char *, const char *) SAL_ATTR_WEAK;
extern char *sal_strncpy(char *, const char *, size_t) SAL_ATTR_WEAK;
extern int sal_strcmp(const char *, const char *) SAL_ATTR_WEAK;
extern void *sal_memcpy(void *, const void *, size_t) SAL_ATTR_WEAK;
extern void *sal_memset(void *, int, size_t) SAL_ATTR_WEAK;
extern int sal_strncmp(const char *s1, const char *s2, size_t n) SAL_ATTR_WEAK;
extern char *sal_strstr(const char *haystack, const char *needle) SAL_ATTR_WEAK;
extern char *sal_strncat(char *dest, const char *src, size_t n) SAL_ATTR_WEAK;
extern char *sal_strchr(const char *s, int c) SAL_ATTR_WEAK;
extern char *sal_strrchr(const char *s, int c) SAL_ATTR_WEAK;
extern size_t sal_strspn(const char *s, const char *accept) SAL_ATTR_WEAK;
extern char *sal_strcat(char *dest, const char *src) SAL_ATTR_WEAK;
extern int sal_isspace(int c) SAL_ATTR_WEAK;
extern int sal_islower(int c) SAL_ATTR_WEAK;
extern int sal_strcasecmp(const char *dest, const char *src) SAL_ATTR_WEAK;
extern int sal_strncasecmp(const char *dest, const char *src, size_t cnt) SAL_ATTR_WEAK;
#if (0)
/* { */
/*
 * These procedures have not yet been implemented on our code so we still use
 * the standard library.
 */
extern void *sal_memmove(void *, const void *, size_t) SAL_ATTR_WEAK;
extern unsigned long int sal_strtoul(const char *nptr, 
        char **endptr, int base) SAL_ATTR_WEAK;
extern long int sal_strtol(const char *nptr, char **endptr, int base) SAL_ATTR_WEAK;
extern int sal_toupper(int c) SAL_ATTR_WEAK;
extern int sal_tolower(int c) SAL_ATTR_WEAK;
extern int sal_atoi(const char *nptr) SAL_ATTR_WEAK;
extern void sal_abort(void) SAL_ATTR_WEAK;

 /* Should be avoided, not thread safe !!! */
extern char *sal_strtok(char *str, const char *delim) SAL_ATTR_WEAK;
/* } */
#else
/* { */
#define sal_memmove     memmove
#define sal_strtoul strtoul
#define sal_strtol  strtol
#define sal_toupper toupper
#define sal_tolower tolower
#define sal_atoi    atoi
#define sal_abort   abort

#define sal_strtok  strtok
/* } */
#endif
/* } */
#endif /* !RTOS_STRINGS */

/*
 * Do exactly as sal_strncpy() but always set NULL ('0') as last character
 * of the destination string.
 * Unlike strncpy(), the destination here is ended with NULL regardless
 * of the contents of the source.
 */
extern char *sal_strncpy_s(char *dst, const char *src, size_t length) SAL_ATTR_WEAK;
/*
 * ALWAYS use our version of strnlen, since the original strnlen()
 * depends on '_GNU_SOURCE' definition which is not available on all
 * the various compilations.
 */
extern int sal_strnlen(const char *, size_t) SAL_ATTR_WEAK;
/*
 * ALWAYS use our version of strnrchr, since not all linux/unix versions have it.
 * (This function is required for security reasons)
 */
extern char *sal_strnrchr(const char *pp, int ch, size_t len);
/*
 * ALWAYS use our version of strnchr, since not all linux/unix versions have it.
 * (This function is required for security reasons)
 */
extern char *sal_strnchr(const char *pp, int ch, size_t len);
/*
 * ALWAYS use our version of strnstr, since not all linux/unix versions have it.
 * (This function is required for security reasons)
 */
extern char *sal_strnstr(const char *haystack, const char *needle, size_t len) SAL_ATTR_WEAK;
/* Always use our version of memcmp, since it is broken 
 * in certain OS-versions (e..g Linux 2.4.18)
 */
extern int sal_memcmp(const void *, const void *, size_t) SAL_ATTR_WEAK;
/**
 * This function calculates the maximal number of characters that can
 * be appended to 'dst' and, then, appends 'src' as per the calculated
 * size.
 * Also it always sets NULL ('0') as last character of the destination string
 * ('dst').
 * If no character could be appended to 'dst' then return value is set to NULL.
 */
extern char * sal_strncat_s(char *dst, const char *src, size_t dest_max_size) SAL_ATTR_WEAK;
/* Always use our version of strdup, strndup, which uses 
 * sal_alloc() instead of malloc(), and can be freed by sal_free()
 */
extern char *sal_strdup(const char *s) SAL_ATTR_WEAK;
extern char *sal_strndup(const char *s, size_t) SAL_ATTR_WEAK;
extern size_t sal_strlcpy(char *, const char *, size_t) SAL_ATTR_WEAK;

extern char *sal_strtok_r(char *s1, const char *delim, char **s2) SAL_ATTR_WEAK;

extern int sal_ctoi(const char *s, char **end) SAL_ATTR_WEAK;	/* C constant to integer */
extern void sal_itoa(char *buf, uint32 num,
		     int base, int caps, int prec) SAL_ATTR_WEAK;

#ifdef COMPILER_HAS_DOUBLE
extern void sal_ftoa(char *buf, double f, int decimals) SAL_ATTR_WEAK;
#endif

extern int sal_vsnprintf(char *buf, size_t bufsize,
			 const char *fmt, va_list ap)
    COMPILER_ATTRIBUTE ((format (printf, 3, 0))) SAL_ATTR_WEAK;
extern int sal_vsprintf(char *buf, const char *fmt, va_list ap)
    COMPILER_ATTRIBUTE ((format (printf, 2, 0))) SAL_ATTR_WEAK;
extern int sal_snprintf(char *buf, size_t bufsize, const char *fmt, ...)
    COMPILER_ATTRIBUTE ((format (printf, 3, 4))) SAL_ATTR_WEAK;
extern int sal_sprintf(char *buf, const char *fmt, ...)
    COMPILER_ATTRIBUTE ((format (printf, 2, 3))) SAL_ATTR_WEAK;
extern void sal_free_safe(void *ptr) SAL_ATTR_WEAK;

#define SAL_RAND_MAX 32767

extern int sal_rand(void) SAL_ATTR_WEAK;
extern void sal_srand(unsigned seed) SAL_ATTR_WEAK;
extern uint32 sal_ceil_func(uint32 numerators , uint32 denominator) SAL_ATTR_WEAK;
extern uint32 sal_floor_func(uint32 numerators , uint32 denominator) SAL_ATTR_WEAK;

extern char *sal_strcasestr(const char *dest, const char *src) SAL_ATTR_WEAK;
extern long long sal_strtoll(const char *s, char **end, int base) SAL_ATTR_WEAK;
extern unsigned long long sal_strtoull(const char *s, char **end, int base) SAL_ATTR_WEAK;
extern void sal_strlwr(char *s) SAL_ATTR_WEAK;
extern void sal_strupr(char *s) SAL_ATTR_WEAK;
extern int sal_isupper(int c) SAL_ATTR_WEAK;
extern int sal_isalnum(int c) SAL_ATTR_WEAK;
extern int sal_isalpha(int c) SAL_ATTR_WEAK;
extern int sal_isdigit(int c) SAL_ATTR_WEAK;
extern int sal_isxdigit(int c) SAL_ATTR_WEAK;
extern void *sal_bsearch(const void *el, const void *arr, size_t numel,
                         size_t elsz, int (*cmpfn)(const void *, const void *)) SAL_ATTR_WEAK;
extern void sal_qsort(void *arr, size_t numel,
                      size_t elsz, int (*cmpfn)(const void *, const void *)) SAL_ATTR_WEAK;
/*sal_strncaseindex: is checking if the string contains the substring*/
extern const char * sal_strncaseindex(const char *s, const char *sub,
                                size_t max_s_len, size_t max_sub_len) SAL_ATTR_WEAK;
#endif	/* !_SAL_LIBC_H */
