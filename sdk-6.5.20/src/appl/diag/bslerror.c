/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Error Sink
 */

#include <sal/core/libc.h>
#include <sal/appl/io.h>
#include <appl/diag/bslsink.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/bslerror.h>
#include <sal/appl/sal.h>

static bslsink_sink_t error_sink;
static int error_enabled = 0;
static int exit_flag = 0;

STATIC int
bslerror_printf(const char *format, ...)
{
    int rc;
    va_list vargs;

    va_start(vargs, format);
    rc = sal_vprintf(format, vargs);
    va_end(vargs);

    return rc;
}

STATIC int
bslerror_vfprintf(void *file, const char *format, va_list args)
{
    int rv;
    rv = sal_vprintf(format, args);
    if (exit_flag) {
        bslerror_printf("\nExited bcm shell on BSL error or fatal.\n");
        sal_reboot();
    } else {
        exit_flag = 1;
    }
    return rv;
}

STATIC int
bslerror_check(bsl_meta_t *meta)
{
    return error_enabled;
}

int
bslerror_init(void)
{
    bslsink_sink_t *sink;

    /* Create error sink */
    sink = &error_sink;
    bslsink_sink_t_init(sink);
    sal_strncpy(sink->name, "error", sizeof(sink->name));
    sink->vfprintf = bslerror_vfprintf;
    sink->check = bslerror_check;
    sink->enable_range.min = bslSeverityOff+1;
    sink->enable_range.max = bslSeverityError;
    sink->options = BSLSINK_OPT_NO_ECHO;

    /* Configure log prefix */
    sal_strncpy(sink->prefix_format, "%f[%l]%F unit %u:",
                sizeof(sink->prefix_format));
    sink->prefix_range.min = bslSeverityOff+1;
    sink->prefix_range.max = bslSeverityError;

    bslsink_sink_add(sink);

    return 0;
}

int
bslerror_enable_set(int enable)
{
    if (enable) {
        bslerror_printf("Turned on exit bcm shell on BSL error or fatal.\n");
    } else {
        if (enable != error_enabled) {
            bslerror_printf("Turned off exit bcm shell on BSL error or fatal.\n");
        }
    }
    return error_enabled = enable;
}
