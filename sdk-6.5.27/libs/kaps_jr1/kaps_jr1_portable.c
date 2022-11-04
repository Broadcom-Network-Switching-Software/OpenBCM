

#define _FILE_OFFSET_BITS 64

#include "kaps_jr1_portable.h"
#include "sal/core/alloc.h"
#include "sal/core/libc.h"
#include "sal/core/sync.h"
#include "sal/appl/sal.h"
#include "sal/appl/io.h"
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sal/core/libc.h>

struct kaps_jr1_pthread_mutex
{
    sal_mutex_t mutex;
};

int
kaps_jr1_gettimeofday(
    struct kaps_jr1_timeval *tv)
{
    struct timeval t;
    int retval;

    retval = gettimeofday(&t, 0);

    tv->tv_sec = t.tv_sec;
    tv->tv_ns = t.tv_usec * 1000;

    return retval;
}

int
kaps_jr1_gettimerval(
    struct kaps_jr1_timeval *tv)
{
    struct timeval t;
    int retval;

    retval = gettimeofday(&t, 0);

    tv->tv_sec = t.tv_sec;
    tv->tv_ns = t.tv_usec * 1000;

    return retval;
}

kaps_jr1_time_t
kaps_jr1_time(
    kaps_jr1_time_t * time_in_sec)
{
    time_t rawtime = time(NULL);
    kaps_jr1_time_t ktime = (kaps_jr1_time_t) rawtime;

    if (time_in_sec)
        *time_in_sec = ktime;

    return ktime;
}

struct kaps_jr1_tm *
kaps_jr1_localtime(
    kaps_jr1_time_t * time_in_sec,
    struct kaps_jr1_tm *info)
{
    time_t rawtime = (time_t) (*time_in_sec);
    struct tm timeinfo;

    localtime_r(&rawtime, &timeinfo);
    kaps_jr1_memcpy(info, &timeinfo, sizeof(struct kaps_jr1_tm));

    return info;
}

void *
kaps_jr1_memset(
    void *str,
    int32_t val,
    uint32_t size)
{
    sal_memset(str, val, size);
    return str;
}

void *
kaps_jr1_memcpy(
    void *dest,
    const void *src,
    uint32_t size)
{
    sal_memcpy(dest, src, size);
    return dest;
}

int
kaps_jr1_memcmp(
    const void *s1,
    const void *s2,
    uint32_t size)
{
    int ret = 0;
    ret = sal_memcmp(s1, s2, size);
    return ret;
}

void *
kaps_jr1_sysmalloc(
    uint32_t size)
{
    void *ptr = NULL;
    ptr = sal_alloc((size), __FILE__);
    return ptr;
}

void *
kaps_jr1_syscalloc(
    uint32_t nmemblk,
    uint32_t size)
{
    void *ptr = NULL;

    ptr = sal_alloc2((nmemblk * size), SAL_ALLOC_F_ZERO, "kaps_jr1_sdk");

    return ptr;
}

void
kaps_jr1_sysfree(
    void *ptr)
{
    sal_free(ptr);
}

void
kaps_jr1_abort(
    )
{
    abort();
}

int
kaps_jr1_vsnprintf(
    char *buff,
    uint32_t buffsize,
    const char *fmt,
    va_list ap)
{
    return sal_vsnprintf(buff, buffsize, fmt, ap);
}

int
kaps_jr1_sprintf(
    char *buff,
    const char *fmt,
    ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vsprintf(buff, fmt, ap);
    va_end(ap);

    return r;
}

int
kaps_jr1_snprintf(
    char *buff,
    uint32_t buffsize,
    const char *fmt,
    ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vsnprintf(buff, buffsize, fmt, ap);
    va_end(ap);

    return r;
}

int
kaps_jr1_printf(
    const char *fmt,
    ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vprintf(fmt, ap);
    va_end(ap);

    return r;
}

int
kaps_jr1_fputs(
    const char *str,
    FILE * fp)
{
    int r;

    r = sal_fprintf(fp, str);
    return r;
}

int
kaps_jr1_vprintf(
    const char *fmt,
    va_list ap)
{
    return sal_vprintf(fmt, ap);
}

int
kaps_jr1_vfprintf(
    FILE * fp,
    const char *fmt,
    va_list ap)
{
    if (!fp)
        return 0;

    return sal_vfprintf(fp, fmt, ap);
}

int
kaps_jr1_fprintf(
    FILE * fp,
    const char *fmt,
    ...)
{
    int r;
    va_list ap;

    if (!fp)
        return 0;

    va_start(ap, fmt);
    r = sal_vfprintf(fp, fmt, ap);
    va_end(ap);

    return r;
}

int
kaps_jr1_fscanf(
    FILE * fp,
    const char *fmt,
    ...)
{
    va_list ap;
    void *arg;
    char *str = strdup(fmt);
    char *token;
    int r = 0, c = 0;

    if (!fp)
        goto error;

    va_start(ap, fmt);
    for (token = kaps_jr1_strsep(&str, " "); token; token = kaps_jr1_strsep(&str, " "))
    {
        if (strstr(token, "%"))
        {
            char *tkn = kaps_jr1_sysmalloc(strlen(token) + 1);
            strcpy(tkn, token);
            tkn[strlen(token)] = ' ';
            arg = va_arg(ap, void *);
            c = fscanf(fp, tkn, arg);
            kaps_jr1_sysfree(tkn);

            if (c == EOF)
            {
                r = EOF;
                va_end(ap);
                kaps_jr1_sysfree(str);
                return r;
            }
            r++;
        }
    }
    va_end(ap);
error:
    kaps_jr1_sysfree(str);
    return r;
}

void *
kaps_jr1_memmove(
    void *dst,
    const void *src,
    uint32_t size)
{
    uint8_t *a = dst;
    const uint8_t *b = src;

    if (a <= b || a >= (b + size))
    {
        while (size--)
            *a++ = *b++;
    }
    else
    {
        a = a + size - 1;
        b = b + size - 1;

        while (size--)
            *a-- = *b--;
    }

    return dst;
}

void
kaps_jr1_enable_error_box(
    int32_t enable)
{
    (void) enable;
}

void
kaps_jr1_srandom(
    uint32_t seed)
{
    sal_srand(seed);
}

int kaps_jr1_random(void)
{
    return sal_rand();
}

int
kaps_jr1_random_r(
    uint32_t * seedp)
{
    return rand_r(seedp);
}

FILE *kaps_jr1_open_file(const char *fname)
{
    char *orig, *dname;
    char *p = NULL;
    size_t slen;
    int retval;

    orig = sal_strdup(fname);
    dname = dirname(orig);
    slen = strlen(dname);
    if (dname[slen - 1] == '/')
        dname[slen - 1] = 0;
    for (p = dname + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            retval = mkdir(dname, S_IRWXU);
            if ((retval != 0) && (errno != EEXIST)) {
                *p = '/';
                kaps_jr1_sysfree(orig);
                return NULL;
            }

            *p = '/';
        }
    }
    retval = mkdir(dname, S_IRWXU);
    kaps_jr1_sysfree(orig);

    if ((retval != 0) && (errno != EEXIST))
        return NULL;

    return fopen(fname, "w");
}

char *
kaps_jr1_strsep(
    char **stringp,
    const char *delim)
{
    return strsep(stringp, delim);
}

int
kaps_jr1_fseek(
    FILE * fp,
    int32_t offset,
    int whence)
{
    return fseek(fp, offset, whence);
}

int
kaps_jr1_fclose(
    FILE * fp)
{
    return fclose(fp);
}

void
kaps_jr1_usleep(
    uint32_t usecs)
{
    sal_usleep(usecs);
}

void *
kaps_jr1_mmap(
    void *addr,
    uint32_t length,
    int prot,
    int flags,
    int fd,
    uint64_t offset)
{
    return mmap(addr, length, prot, flags, fd, offset);
}

int
kaps_jr1_munmap(
    void *addr,
    uint32_t length)
{
    return munmap(addr, length);
}

int
kaps_jr1_open(
    const char *pathname,
    int flags,
    uint32_t mode)
{
    return open(pathname, flags, mode);
}

int32_t
kaps_jr1_identity(
    const int32_t b)
{
    return b;
}

int32_t
kaps_jr1_assert_detail(
    const char *msg,
    const char *file,
    int32_t line)
{
    kaps_jr1_printf("ERROR %s:%d: %s\n", file, line, msg);
    kaps_jr1_abort();
    return 0;
}

int32_t
kaps_jr1_assert_detail_or_error(
    const char *msg,
    uint32_t return_error,
    uint32_t error_code,
    const char *file,
    int32_t line)
{
    kaps_jr1_printf("ERROR %s:%d: %s\n", file, line, msg);
    if (!return_error)
        kaps_jr1_abort();
    return error_code;
}

int
kaps_jr1_mutex_init(
    struct kaps_jr1_mutex **mutex)
{
    struct kaps_jr1_pthread_mutex *pmutex = NULL;
    int ret_val = 0;

    if (mutex == NULL)
        return EINVAL;

    pmutex = kaps_jr1_syscalloc(1, sizeof(struct kaps_jr1_pthread_mutex));
    if (pmutex == NULL)
        return ENOMEM;

    pmutex->mutex = sal_mutex_create("KBP Mutex");
    if (pmutex->mutex)
    {
        *mutex = (struct kaps_jr1_mutex *) pmutex;
    }
    else
    {
        
        kaps_jr1_sysfree(pmutex);
        ret_val = ENOMEM;
    }

    return ret_val;
}

int
kaps_jr1_mutex_destroy(
    struct kaps_jr1_mutex *mutex)
{
    struct kaps_jr1_pthread_mutex *pmutex;
    int ret_val = 0;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kaps_jr1_pthread_mutex *) mutex;

    
    sal_mutex_destroy(pmutex->mutex);
    kaps_jr1_sysfree(pmutex);

    return ret_val;
}

int
kaps_jr1_mutex_lock(
    struct kaps_jr1_mutex *mutex)
{
    struct kaps_jr1_pthread_mutex *pmutex;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kaps_jr1_pthread_mutex *) mutex;
    return sal_mutex_take(pmutex->mutex, sal_mutex_FOREVER);
}

int
kaps_jr1_mutex_trylock(
    struct kaps_jr1_mutex *mutex)
{
    struct kaps_jr1_pthread_mutex *pmutex;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kaps_jr1_pthread_mutex *) mutex;
    return sal_mutex_take(pmutex->mutex, sal_mutex_NOWAIT);
}

int
kaps_jr1_mutex_unlock(
    struct kaps_jr1_mutex *mutex)
{
    struct kaps_jr1_pthread_mutex *pmutex;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kaps_jr1_pthread_mutex *) mutex;
    return sal_mutex_give(pmutex->mutex);

}
 
