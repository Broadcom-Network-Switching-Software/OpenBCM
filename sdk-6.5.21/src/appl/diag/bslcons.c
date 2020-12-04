/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Console Sink
 */

#include <sal/core/libc.h> 
#include <sal/appl/io.h> 
#include <appl/diag/bslsink.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/bslenable.h>

static bslsink_sink_t console_sink;
static int console_enabled = 1;
static int thread_check_enabled = 0;

STATIC int
bslcons_vfprintf(void *file, const char *format, va_list args)
{
    return sal_vprintf(format, args);
}

STATIC int
bslcons_thread_check(bsl_meta_t *meta)
{
    int i;
    int thread_id = sal_thread_id_get();

    if (bslenable_thread_get(meta->layer, meta->source, 0) == 0) {
        return 1;
    }
    for (i = 0; i < MAX_BSLENABLE_THREAD_COUNT; i ++) {
        if (thread_id == bslenable_thread_get(meta->layer, meta->source, i)) {
            return 1;
        }
    }

    return 0;
}

STATIC int
bslcons_check(bsl_meta_t *meta)
{
    if (!console_enabled) {
        return 0;
    }
    if (thread_check_enabled) {
        return bslcons_thread_check(meta);
    }
    return console_enabled;
}

int
bslcons_init(void)
{
    bslsink_sink_t *sink;

    /* Create console sink */
    sink = &console_sink;
    bslsink_sink_t_init(sink);
    sal_strncpy(sink->name, "console", sizeof(sink->name));
    sink->vfprintf = bslcons_vfprintf;
    sink->check = bslcons_check;
    sink->enable_range.min = bslSeverityOff+1;
#ifdef BSL_TRACE_INCLUDE
    /* bslSeverityDebug will be handled by the trace sink by default */
    sink->enable_range.max = bslSeverityVerbose;
#else
    sink->enable_range.max = bslSeverityCount-1;
#endif
    sink->options = BSLSINK_OPT_NO_ECHO;
    bslsink_sink_add(sink);

    /* Configure log prefix */
    sal_strncpy(sink->prefix_format, "%u:%F: ",
                sizeof(sink->prefix_format));
    sink->prefix_range.min = bslSeverityOff+1;
    sink->prefix_range.max = bslSeverityWarn;

    return 0;
}

int
bslcons_is_enabled(void)
{
    return console_enabled;
}

int
bslcons_enable(int enable)
{
    int cur_enable = bslcons_is_enabled();

    console_enabled = enable;

    return cur_enable;
}

int
bslcons_thread_check_is_enabled(void)
{
    return thread_check_enabled;
}

int
bslcons_thread_check_enable(int enable)
{
    int cur_enable = bslcons_thread_check_is_enabled();

    thread_check_enabled = enable;

    return cur_enable;
}
