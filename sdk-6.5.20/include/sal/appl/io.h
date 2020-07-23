/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	io.h
 * Purpose: 	SAL I/O definitions
 */

#ifndef _SAL_IO_H
#define _SAL_IO_H

#include <sal/core/libc.h>

/* Allow global override of console I/O */

#include <sdk_config.h>

#ifdef SDK_CONFIG_SAL_VPRINTF
#define sal_vprintf SDK_CONFIG_SAL_VPRINTF
#endif

#ifdef SDK_CONFIG_SAL_READLINE
#define sal_readline SDK_CONFIG_SAL_READLINE
#endif


#ifdef NO_SAL_APPL

/* You must specify a stdarg */
#include <stdarg.h>

/*
 * The only things exported or required are console read/write
 * These must be provided by the target environment. 
 */
extern int sal_portable_vprintf(const char* fmt, va_list vargs)
    COMPILER_ATTRIBUTE ((format (printf, 1, 0)));
extern char* sal_portable_readline(char *prompt, char *buf, int bufsize);

#define sal_vprintf sal_portable_vprintf


/* These are provided internally using the above */
extern int sal_portable_printf(const char* fmt, ...)
    COMPILER_ATTRIBUTE ((format (printf, 1, 2)));
extern char* sal_portable_readline_internal(char* prompt, char* buf, int bufsize, char* defl); 

#define sal_readline sal_portable_readline_internal
#define sal_printf sal_portable_printf

#define SAL_VT100_SO		"\033[7m"	/* Stand-out begin */
#define SAL_VT100_SE		"\033[m"	/* Stand-out end */
#define SAL_VT100_CE		"\033[K"	/* Kill to end of line */

#else

#ifdef __KERNEL__       /* Take care of the kernel mode */
typedef void* FILE;     /* Dummy for declarations. Never used. */
#define EOF (-1)
#else
#include <stdio.h>
#endif

#include <stdarg.h>
#include <stdlib.h>

#ifdef VA_COPY_ARRAY
#  define va_copy(to, from)	((to)[0] = (from)[0])
#endif

#ifdef VA_COPY_PTR
#  define va_copy(to, from)	((to) = (from))
#endif

#ifndef va_copy
# if defined(MOUSSE) || defined(BMW) || defined(NEGEV) || defined(GTO) || defined(__DCC__)
#  define va_copy(to, from)	((to)[0] = (from)[0])
# elif defined(UNIX) || defined(IDTRP334) || defined(__BORLAND__) || \
     defined(linux) || defined(MBZ) || defined(IDT438) || defined(NSX) || \
       defined(METROCORE) || \
     defined(KEYSTONE)
#  define va_copy(to, from)	((to) = (from))
# else
#  error va_copy?
# endif
#endif

#include <sal/types.h>

#define SAL_NAME_MAX		512

#define SAL_VT100_SO		"\033[7m"	/* Stand-out begin */
#define SAL_VT100_SE		"\033[m"	/* Stand-out end */
#define SAL_VT100_CE		"\033[K"	/* Kill to end of line */

typedef struct sal_console_info_s {
    int cols;           /* characters per row */
    int rows;           /* number of rows */
} sal_console_info_t;

extern int sal_console_init(void);
extern int sal_console_info_get(sal_console_info_t *info);
extern int sal_console_write(const void *buf, int count);
extern int sal_console_read(void *buf, int count);
extern char *sal_console_gets(char *buf, int size);
extern int sal_printf(const char *fmt, ...)
    COMPILER_ATTRIBUTE ((format (printf, 1, 2)));
extern int sal_vprintf(const char *fmt, va_list varg)
    COMPILER_ATTRIBUTE ((format (printf, 1, 0)));

extern int sal_open(char *name, int oflag, ...);
extern int sal_close(int fd);

extern FILE *sal_fopen(char *name, char *mode);

extern int sal_fclose(FILE *fp);

extern int sal_fread(void *buf, int size, int num, FILE *fp);
extern int sal_feof(FILE *fp);
extern int sal_ferror(FILE *fp);
extern int sal_fsize(FILE *fp);

extern int sal_homedir_set(char *dir);
extern char *sal_homedir_get(char *buf, size_t size);

extern int sal_umask(int mask);
extern char *sal_getcwd(char *buf, size_t size);
extern int sal_ls(char *filename, char *flags);
extern int sal_cd(char *filename);
extern int sal_remove(char *filename);
extern int sal_rename(char *file_old, char *file_new);
extern int sal_mkdir(char *path);
extern int sal_rmdir(char *path);
extern char *sal_dirname(char *path, char *dest);
extern char *sal_basename(char *path, char *dest);

#if defined(UNIX) || defined(LINUX)
#define sal_fgets fgets
#define sal_fwrite fwrite
#define sal_fprintf fprintf
#define sal_fgetc fgetc
#define sal_ftell ftell
#define sal_fseek fseek
#define sal_fputc fputc
#define sal_fputs fputs
#define sal_isatty isatty
#define sal_fflush fflush
#define sal_vfprintf vfprintf
#else
extern size_t sal_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
extern char* sal_fgets(char *s, int size, FILE *fp);
extern int sal_fprintf(FILE *fp, const char *fmt, ...)
    COMPILER_ATTRIBUTE ((format (printf, 2, 3)));
extern int sal_vfprintf(FILE *fp, const char *fmt, va_list varg)
    COMPILER_ATTRIBUTE ((format (printf, 2, 0)));
extern int sal_fgetc(FILE *fp);
extern int sal_fputc(int c, FILE *fp);
extern int sal_fputs(const char *s, FILE *fp);
extern int sal_isatty (int );
extern int sal_fflush(FILE *fp);
#endif


/*
 * Directory I/O
 */

struct sal_dirent {
    char	d_name[SAL_NAME_MAX];
};

typedef struct {
    int			is_netio;
    void		*dirp;		/* NETIO_DIR or DIR pointer */
    struct sal_dirent	de;
} SAL_DIR;

extern SAL_DIR *sal_opendir(char *dirName);
extern int sal_closedir(SAL_DIR *pDir);
extern struct sal_dirent *sal_readdir(SAL_DIR *pDir);
extern void sal_rewinddir(SAL_DIR *pDir);

/*
 * Command line editor I/O
 */

extern char* sal_readline(char *prompt, char *buf, int bufsize, char *defl);
extern void sal_readline_config(char *(*complete)(char *pathname, int *unique),
			    int (*list_possib)(char *pathname, char ***avp));
extern int sal_readchar(const char *prompt);

extern int sal_flash_init(int format);
extern int sal_flash_boot(char *file);
extern int sal_flash_sync(void);

/*
 * Some of our BSPs have routines have a sysSerialPrintf routine that
 * allows the console serial port to be written directly in polled mode
 * with interrupts turned off.  When this is available, it is preferred
 * over logMsg.
 */

#if defined(MOUSSE) || defined(BMW) || defined(MBZ) || \
    defined(IDTRP334) || defined(IDT438) || defined(NSX) || \
      defined(METROCORE) || \
    defined(KEYSTONE)
#define HAVE_DIRECT_SERIAL
#endif

#ifdef HAVE_DIRECT_SERIAL
extern void sysSerialPutc(int c);
extern int sysSerialGetc(void);
extern void sysSerialPrintString(char *s);
extern void sysSerialPrintHex(uint32 value, int cr);
#endif /* HAVE_DIRECT_SERIAL */

#endif /* NO_SAL_APPL */

#endif	/* !_SAL_IO_H */
