/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log FIle Sink
 */

#include <sal/core/libc.h> 
#include <sal/core/spl.h> 
#include <sal/core/alloc.h> 
#include <sal/core/dpc.h> 
#include <sal/core/sync.h>
#include <sal/appl/io.h> 
#include <appl/diag/bslsink.h>
#include <appl/diag/bslfile.h>

#ifndef NO_FILEIO

#include <stdio.h>

static FILE *file_fp = NULL;
static char *file_nm = NULL;

static bslsink_sink_t logfile_sink;
static sal_mutex_t file_lock = NULL;

#define FILE_LOCK \
        if (file_lock != NULL) \
        { sal_mutex_take(file_lock, sal_mutex_FOREVER); }

#define FILE_UNLOCK \
        if (file_lock != NULL) \
        { sal_mutex_give(file_lock); }

STATIC void
bslfile_dpc(void *a1, void *a2, void *a3, void *a4, void *a5)
{
    char *fmt = (char *)a1;
    FILE_LOCK;
    if (file_fp) {
	sal_fprintf(file_fp, fmt, a2, a3, a4, a5);
	sal_fflush(file_fp);
    }
    FILE_UNLOCK;
}

STATIC int
bslfile_vfprintf(void *file, const char *format, va_list args)
{
    int	retv = 0;

    FILE_LOCK;
    if (file_fp) {
	if (sal_int_context()) {
	    void *a1, *a2, *a3, *a4;
	    a1 = va_arg(args, void *);
	    a2 = va_arg(args, void *);
	    a3 = va_arg(args, void *);
	    a4 = va_arg(args, void *);
	    /* Passing format in lieu of fake owner to avoid wasting an arg */
	    retv = sal_dpc(bslfile_dpc, (void *)format, a1, a2, a3, a4);
	} else {
	    retv = sal_vfprintf(file_fp, format, args);
	    sal_fflush(file_fp);
	}
    }
    FILE_UNLOCK;
    return retv;
}

STATIC int
bslfile_check(bsl_meta_t *meta)
{
    return (file_fp != NULL);
}

STATIC int
bslfile_cleanup(struct bslsink_sink_s *sink)
{
    if (file_nm) {
	bslfile_close();
    }
    sal_mutex_destroy(file_lock);
    file_lock = NULL;
    return 0;
}

FILE *
bslfile_fp(void)
{
    return file_fp;
}

#endif /* NO_FILEIO */

int
bslfile_init(void)
{
#ifndef NO_FILEIO
    bslsink_sink_t *sink;

    /* Create mutex */
    file_lock = sal_mutex_create("BSLFILE MUTEX");
    if (file_lock == NULL) {
        sal_printf("bslfile: Mutex create error\n");
        return -1;
    }

    /* Create logfile sink */
    sink = &logfile_sink;
    bslsink_sink_t_init(sink);
    sal_strncpy(sink->name, "logfile", sizeof(sink->name));
    sink->vfprintf = bslfile_vfprintf;
    sink->check = bslfile_check;
    sink->cleanup = bslfile_cleanup;
    sink->enable_range.min = bslSeverityOff+1;
    sink->enable_range.max = bslSeverityCount-1;

    /* Configure log prefix */
    sal_strncpy(sink->prefix_format, "%u:%F: ",
                sizeof(sink->prefix_format));
    sink->prefix_range.min = bslSeverityOff+1;
    sink->prefix_range.max = bslSeverityWarn;

    bslsink_sink_add(sink);
#endif

    return 0;
}

char *
bslfile_name(void)
{
#ifdef NO_FILEIO
    return "<no filesystem>";
#else
    return(file_nm);
#endif
}

int
bslfile_close(void)
{
#ifndef NO_FILEIO
    FILE_LOCK;
    if (file_nm) {
        sal_free(file_nm);
        file_nm = NULL;
    }
    if (file_fp) {
	sal_fclose(file_fp);
	file_fp = 0;
    }
    FILE_UNLOCK;
#endif
    return 0;
}

int
bslfile_open(char *filename, int append)
{
#ifndef NO_FILEIO
    FILE_LOCK;
    if (file_nm) {
	bslfile_close();
    }
    if ((file_fp = sal_fopen(filename, append ? "a" : "w")) == 0) {
        sal_printf("bslfile: File open error\n");
        FILE_UNLOCK;
	return -1;
    }
    file_nm = sal_strdup(filename);
    if (file_nm == NULL) {
        bslfile_close();
        sal_printf("bslfile: strdup failed\n");
        FILE_UNLOCK;
        return -1;
    }
    FILE_UNLOCK;
#endif /* NO_FILEIO */
    return 0;
}

int
bslfile_is_enabled(void)
{
#ifndef NO_FILEIO
    if (file_fp != NULL) {
        return 1;
    }
#endif
    return 0;
}

int
bslfile_enable(int enable)
{
    int cur_enable = bslfile_is_enabled();
#ifndef NO_FILEIO
    FILE_LOCK;
    if (file_fp == NULL && enable) {
	if (file_nm == NULL) {
	    sal_printf("bslfile: No log file\n");
            FILE_UNLOCK;
	    return -1;
	}
	if ((file_fp = sal_fopen(file_nm, "a")) == 0) {
	    sal_printf("bslfile: File open error\n");
            FILE_UNLOCK;
	    return -1;
	}
    }

    if (file_fp != NULL && !enable) {
        bslfile_close();
    }
    FILE_UNLOCK;
#endif
    return cur_enable;
}
