/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#define _FILE_OFFSET_BITS 64

#include "kbp_portable.h"
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

struct kbp_pthread_mutex {
    sal_mutex_t mutex;
};

int kbp_gettimeofday(struct kbp_timeval * tv)
{
    struct timeval t;
    int retval;

    retval = gettimeofday(&t, 0);

    tv->tv_sec = t.tv_sec;
    tv->tv_ns = t.tv_usec * 1000;

    return retval;
}

int kbp_gettimerval(struct kbp_timeval * tv)
{
    struct timeval t;
    int retval;

    retval = gettimeofday(&t, 0);

    tv->tv_sec = t.tv_sec;
    tv->tv_ns = t.tv_usec * 1000;

    return retval;
}

kbp_time_t kbp_time(kbp_time_t *time_in_sec)
{
    time_t rawtime = time(NULL);
    kbp_time_t ktime = (kbp_time_t) rawtime;

    if (time_in_sec)
        *time_in_sec = ktime;

    return ktime;
}

struct kbp_tm *kbp_localtime(kbp_time_t *time_in_sec, struct kbp_tm *info)
{
    time_t rawtime = (time_t) (*time_in_sec);
    struct tm timeinfo;

    localtime_r(&rawtime, &timeinfo);
    kbp_memcpy(info, &timeinfo, sizeof(struct kbp_tm));

    return info;
}

void *kbp_memset(void *str, int32_t val, uint32_t size)
{
    sal_memset(str, val, size);
    return str;
}

void *kbp_memcpy(void *dest, const void *src, uint32_t size)
{
    sal_memcpy(dest, src, size);
    return dest;
}

int kbp_memcmp(const void *s1, const void *s2, uint32_t size)
{
    int ret = 0;

    ret = sal_memcmp(s1, s2, size);
    return ret;
}

void *kbp_sysmalloc(uint32_t size)
{
    void *ptr = NULL;

    ptr = sal_alloc((size), __FILE__);
    return ptr;
}

void *kbp_syscalloc(uint32_t nmemblk, uint32_t size)
{
    void *ptr = NULL;

    ptr = sal_alloc((nmemblk*size), __FILE__);
    kbp_memset(ptr, 0, (nmemblk*size));
    return ptr;
}

void kbp_sysfree(void *ptr)
{
    sal_free(ptr);
}

void kbp_abort()
{
    abort();
}

int kbp_vsnprintf(char *buff, uint32_t buffsize, const char *fmt, va_list ap)
{
    return sal_vsnprintf(buff, buffsize, fmt, ap);
}


int kbp_sprintf(char *buff, const char *fmt, ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vsprintf(buff, fmt, ap);
    va_end(ap);

    return r;
}

int kbp_snprintf(char *buff, uint32_t buffsize, const char *fmt, ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vsnprintf(buff, buffsize, fmt, ap);
    va_end(ap);

    return r;
}


int kbp_printf(const char *fmt, ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vprintf(fmt, ap);
    va_end(ap);

    return r;
}


int kbp_fputs(const char *str, FILE *fp)
{
    int r;

    r = sal_fprintf(fp, str);
    return r;
}


int kbp_vprintf(const char *fmt, va_list ap)
{
    return sal_vprintf(fmt, ap);
}

int kbp_vfprintf(FILE * fp, const char *fmt, va_list ap)
{
    if (!fp)
        return 0;

    return sal_vfprintf(fp, fmt, ap);
}

int kbp_fprintf(FILE * fp, const char *fmt, ...)
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

int kbp_fscanf(FILE * fp, const char *fmt, ...)
{
    va_list ap;
    void *arg;
    char *str, *token;
    int r, c;

    if (!fp)
        return 0;

    str = strdup(fmt);
    r = 0;
    c = 0;

    va_start(ap, fmt);
    for (token = kbp_strsep(&str, " "); token; token = kbp_strsep(&str, " ")) {
        if (strstr(token, "%")) {
            char *tkn = kbp_sysmalloc(strlen(token) + 1);

            strcpy(tkn, token);
            tkn[strlen(token)] = ' ';
            arg = va_arg(ap, void *);
            c = fscanf(fp, tkn, arg);
            kbp_sysfree(tkn);

            if (c == EOF) {
                r = EOF;
                break;
            }
            r++;
        }
    }

    va_end(ap);
    kbp_sysfree(str);
    return r;
}

void *kbp_memmove(void *dst, const void *src, uint32_t size)
{
    uint8_t *a = dst;
    const uint8_t *b = src;

    if (a <= b || a >= (b + size)) {
        while (size--)
            *a++ = *b++;
    } else {
        a = a + size - 1;
        b = b + size - 1;

        while (size--)
            *a-- = *b--;
    }

    return dst;
}

void kbp_enable_error_box(int32_t enable)
{
    (void) enable;
}

void kbp_srandom(uint32_t seed)
{
    sal_srand(seed);
}

int kbp_random(void)
{
    return sal_rand();
}

int kbp_random_r(uint32_t *seedp)
{
    return rand_r(seedp);
}

static char **list_directory(const char *dir_name, const char *suffix,
                             uint32_t *array_size,
                             char **array, uint32_t *num_found)
{
    DIR *dir = NULL;
    int32_t i;
    struct dirent *dt;
    char *buf = NULL;

    dir = opendir(dir_name);
    if (dir == NULL) {
        printf("failed to open '%s' directory\n", dir_name);
        goto list_d_error;
    }

    for (dt = readdir(dir);
        dt != NULL;
        dt = readdir(dir)) {
        struct stat st;

        if (strcmp(dt->d_name, ".") == 0 || strcmp(dt->d_name, "..") == 0)
            continue;

        buf = kbp_sysmalloc(strlen(dir_name) + strlen("/") + strlen(dt->d_name) + 10);
        sprintf(buf, "%s/%s", dir_name, dt->d_name);

        if (stat(buf, &st) < 0) {
            printf("Could not stat file '%s'\n", buf);
            goto list_d_error;
        }

        if (S_ISDIR(st.st_mode)) {
            array = list_directory(buf, suffix, array_size, array, num_found);
            if (array == NULL)
                goto list_d_error;
            continue;
        }

        if (suffix) {
            int32_t len = strlen(buf);
            char *tmp = buf + len - strlen(suffix);
            if (strcmp(tmp, suffix) != 0) {
                kbp_sysfree(buf);
                buf = NULL;
                continue;
            }
        }

        if (*array_size == *num_found) {
            char **new_array = realloc(array, *array_size * 2 * sizeof(char *));
            if (!new_array)
                goto list_d_error;
            *array_size = *array_size * 2;
            array = new_array;
        }

        array[*num_found] = buf;
        *num_found = *num_found + 1;
    }

    closedir(dir);
    return array;

list_d_error:
    if (buf)
        kbp_sysfree(buf);
    if (dir)
        closedir(dir);
    if (array) {
        for (i = 0; i < *num_found; i++) {
            kbp_sysfree(array[i]);
        }
        kbp_sysfree(array);
    }
    *num_found = 0;
    return NULL;
}

char **kbp_get_files_recursive(const char *location, const char *suffix, uint32_t *num_found)
{
    struct stat st;
    uint32_t array_size = 100;
    char **result;

    *num_found = 0;

    result = kbp_syscalloc(array_size, sizeof(void *));
    if (!result)
        return NULL;

    if (stat(location, &st) < 0) {
        kbp_sysfree(result);
        return NULL;
    }

    if (S_ISDIR(st.st_mode)) {
        result = list_directory(location, suffix, &array_size, result, num_found);
        return result;
    }

    result[0] = strdup(location);
    if (!result[0]) {
        kbp_sysfree(result);
        return NULL;
    }
    *num_found = 1;
    return result;
}

FILE *kbp_open_file(const char *fname)
{
    char *orig, *dname;
    char *p = NULL;
    size_t slen;
    int retval;

    orig = strdup(fname);
    dname = dirname(orig);
    slen = strlen(dname);
    if (dname[slen - 1] == '/')
        dname[slen - 1] = 0;
    for (p = dname + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            retval = mkdir(dname, S_IRWXU);
            if ((retval != 0) && (errno != EEXIST)) {
                kbp_sysfree(orig);
                *p = '/';
                return NULL;
            }

            *p = '/';
        }
    }
    retval = mkdir(dname, S_IRWXU);
    kbp_sysfree(orig);

    if ((retval != 0) && (errno != EEXIST))
        return NULL;

    return fopen(fname, "w");
}

char* kbp_strsep(char **stringp, const char *delim)
{
    return strsep(stringp, delim);
}

int kbp_fseek(FILE * fp, int32_t offset, int whence)
{
    return fseek(fp, offset, whence);
}

int kbp_fclose(FILE * fp)
{
    return fclose(fp);
}

void kbp_usleep(uint32_t usecs)
{
    sal_usleep(usecs);
}

void *kbp_mmap(void *addr, uint32_t length, int prot, int flags,
               int fd, uint64_t offset)
{
    return mmap(addr, length, prot, flags, fd, offset);
}

int kbp_munmap(void *addr, uint32_t length)
{
    return munmap(addr, length);
}

int kbp_open(const char *pathname, int flags, uint32_t mode)
{
    return open(pathname, flags, mode);
}

int32_t kbp_identity(const int32_t b)
{
    return b;
}

int32_t kbp_assert_detail(const char *msg, const char *file, int32_t line)
{
    kbp_printf("ERROR %s:%d: %s\n", file, line, msg);
    kbp_abort();
    return 0;
}

int32_t kbp_assert_detail_or_error(const char *msg, uint32_t return_error, uint32_t error_code, const char *file, int32_t line)
{
    kbp_printf("ERROR %s:%d: %s\n", file, line, msg);
    if (!return_error)
        kbp_abort();
    return error_code;
}

int kbp_mutex_init(struct kbp_mutex **mutex)
{
    struct kbp_pthread_mutex *pmutex = NULL;
    int ret_val = 0;

    if (mutex == NULL)
        return EINVAL;

    pmutex = kbp_syscalloc(1, sizeof(struct kbp_pthread_mutex));
    if (pmutex == NULL)
        return ENOMEM;

    pmutex->mutex = sal_mutex_create("KBP Mutex");
    if (pmutex->mutex) {
        *mutex = (struct kbp_mutex *) pmutex;
    }
    else {
        
        kbp_sysfree(pmutex);
        ret_val = ENOMEM;
    }

    return ret_val;
}

int kbp_mutex_destroy(struct kbp_mutex *mutex)
{
    struct kbp_pthread_mutex *pmutex;
    int ret_val = 0;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kbp_pthread_mutex *) mutex;

    
    sal_mutex_destroy(pmutex->mutex);
    kbp_sysfree(pmutex);

    return ret_val;
}

int kbp_mutex_lock(struct kbp_mutex *mutex)
{
    struct kbp_pthread_mutex *pmutex;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kbp_pthread_mutex *) mutex;
    return sal_mutex_take(pmutex->mutex, sal_mutex_FOREVER);
}

int kbp_mutex_trylock(struct kbp_mutex *mutex)
{
    struct kbp_pthread_mutex *pmutex;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kbp_pthread_mutex *) mutex;
    return sal_mutex_take(pmutex->mutex, sal_mutex_NOWAIT);
}

int kbp_mutex_unlock(struct kbp_mutex *mutex)
{
    struct kbp_pthread_mutex *pmutex;

    if (mutex == NULL)
        return EINVAL;

    pmutex = (struct kbp_pthread_mutex *) mutex;
    return sal_mutex_give(pmutex->mutex);
}
 
