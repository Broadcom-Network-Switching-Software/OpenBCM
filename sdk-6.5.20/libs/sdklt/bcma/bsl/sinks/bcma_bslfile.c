/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log File Sink
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <sal/sal_alloc.h>

#include <shr/shr_atomic.h>

#include <bcma/bsl/bcma_bslsink.h>
#include <bcma/bsl/bcma_bslfile.h>

#include <bcma/io/bcma_io_file.h>

static bcma_io_file_handle_t *file_fp = NULL;
static char *file_nm = NULL;
static shr_atomic_int_t logging_in_progress;

static bcma_bslsink_sink_t logfile_sink;

/* Synchronized close */
static void
bslfile_close(void)
{
    bcma_io_file_handle_t *fp = file_fp;

    file_fp = NULL;
    while (shr_atomic_int_get(logging_in_progress) > 0) {
        sal_usleep(100 * MSEC_USEC);
    }
    if (fp) {
        bcma_io_file_close(fp);
    }
}

static int
bslfile_vfprintf(void *file, const char *format, va_list args)
{
    bcma_io_file_handle_t *fp = file_fp;
    int retv = 0;

    if (file_fp == NULL) {
        return 0;
    }

    shr_atomic_int_incr(logging_in_progress);

    /* Check file_fp again to avoid race with bslfile_close */
    if (file_fp) {
        /* Use local variable as file_fp might be set to NULL */
        retv = bcma_io_file_vprintf(fp, format, args);
        bcma_io_file_flush(fp);
    }

    shr_atomic_int_decr(logging_in_progress);

    return retv;
}

static int
bslfile_check(bsl_meta_t *meta)
{
    return (file_fp != NULL);
}

static int
bslfile_cleanup(struct bcma_bslsink_sink_s *sink)
{
    if (file_nm) {
        bcma_bslfile_close();
    }

    shr_atomic_int_destroy(logging_in_progress);

    return 0;
}

int
bcma_bslfile_init(void)
{
    bcma_bslsink_sink_t *sink;

    /* Create logfile sink */
    sink = &logfile_sink;
    bcma_bslsink_sink_t_init(sink);
    sal_strncpy(sink->name, "logfile", sizeof(sink->name));
    sink->vfprintf = bslfile_vfprintf;
    sink->check = bslfile_check;
    sink->cleanup = bslfile_cleanup;
    sink->enable_range.min = BSL_SEV_OFF + 1;
    sink->enable_range.max = BSL_SEV_COUNT - 1;

    /* Configure log prefix */
    sal_strncpy(sink->prefix_format, "%u:%F: ",
                sizeof(sink->prefix_format));
    sink->prefix_range.min = BSL_SEV_OFF + 1;
    sink->prefix_range.max = BSL_SEV_COUNT - 1;

    if ((logging_in_progress = shr_atomic_int_create(0)) == NULL) {
        return -1;
    }

    bcma_bslsink_sink_add(sink);

    return 0;
}

char *
bcma_bslfile_name(void)
{
    return file_nm;
}

int
bcma_bslfile_close(void)
{
    bslfile_close();
    if (file_nm) {
        sal_free(file_nm);
        file_nm = NULL;
    }
    return 0;
}

int
bcma_bslfile_open(char *filename, int append)
{
    if (file_nm) {
        bcma_bslfile_close();
    }
    file_nm = sal_strdup(filename);
    if (file_nm == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("strdup failed\n")));
        return -1;
    }
    if ((file_fp = bcma_io_file_open(filename, append ? "a" : "w")) == 0) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("File \"%s\" open error\n"), filename));
        bcma_bslfile_close();
        return -1;
    }
    return 0;
}

int
bcma_bslfile_is_enabled(void)
{
    if (file_fp != NULL) {
        return 1;
    }
    return 0;
}

int
bcma_bslfile_enable(int enable)
{
    int cur_enable = bcma_bslfile_is_enabled();

    if (file_fp == NULL && enable) {
        if (file_nm == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("No log file\n")));
            return -1;
        }
        if ((file_fp = bcma_io_file_open(file_nm, "a")) == 0) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META("File open error\n")));
            return -1;
        }
    }

    if (file_fp != NULL && !enable) {
        bslfile_close();
    }
    return cur_enable;
}
