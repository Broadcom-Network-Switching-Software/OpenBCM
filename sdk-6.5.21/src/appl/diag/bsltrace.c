/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Trace Buffer Sink
 */
#include <soc/drv.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <sal/appl/io.h>
#include <appl/diag/bslsink.h>
#include <appl/diag/bslenable.h>
#include <shared/bsl.h>

/*
 * BSL copies the trace string to the buffer in two steps.
 * First the prefix string is copied, then the log string.
 * So each trace string uses two trace buffer entries.
 * Using a short entry length and double the max entries.
 */
#define MAX_NUM_TRACE_ENTRIES 1000
#define MAX_TRACE_ENTRY_LEN 64

typedef struct bsl_trace_buf_s {
    char        * buffer_start;
    int           buffer_size;
    int           cur_num_entries;
    int           max_num_entries;
    int           entry_length;
    char        * head;
    sal_mutex_t   lock;
} bsl_trace_buf_t;

static bsl_trace_buf_t trace;
static bslsink_sink_t trace_sink;

/*
 * Note that inserting trace messages during an interrupt is
 * only allowed in cases where the interrupt handling is done
 * at thread level.
 */
STATIC int bsltrace_add(const char *format, va_list args)
{
    int str_len = 0;

    /* Lock access to trace buf */
    sal_mutex_take(trace.lock, sal_mutex_FOREVER);

    /* Add string to head of trace buffer */
    str_len = sal_vsprintf(trace.head, format, args);

    if (str_len > trace.entry_length) {
        /* Handle Error */
    } else if (str_len < 0) {
        /* Handle Error */
    } else {
        /* Increment head*/
        trace.head = (char *)(trace.head) + trace.entry_length;

        /* Handle wrap */
        if (trace.head == (trace.buffer_start + trace.buffer_size) ) {
            trace.head = trace.buffer_start;
        }

        /* Count up to max number of entries until full,
           then simply overwrite the oldest */
        if (trace.cur_num_entries < trace.max_num_entries) {
            trace.cur_num_entries++;
        }
    }

    /* Unlock access to trace buf */
    sal_mutex_give(trace.lock);

    return str_len;
}

STATIC int
bsltrace_vfprintf(void *file, const char *format, va_list args)
{
    /* Need to print to circular trace buffer */
    return bsltrace_add(format, args);
}

STATIC int
bsltrace_cleanup(bslsink_sink_t * sink)
{

    /* Free allocated trace buffer */
    if (trace.buffer_start) {
        sal_free(trace.buffer_start);
    }

    return 0;
}

int bsltrace_clear(void)
{
    if (!trace.lock) {
        cli_out("Trace Buffer Not Initialized\n");
        return 0;
    }

    /* Lock access to trace buf */
    sal_mutex_take(trace.lock, sal_mutex_FOREVER);

    trace.head = trace.buffer_start;
    trace.cur_num_entries = 0;

    /* Unlock access to trace buf */
    sal_mutex_give(trace.lock);

    return 0;
}

/* Print the last "entries" entries.
 * Print all entries if "entries" < 0
 */
int bsltrace_print(int entries)
{
    char * entry = NULL;
    int i, j;

    if (!trace.lock) {
        cli_out("Trace Buffer Not Initialized\n");
        return 0;
    }

    /* Lock access to trace buf */
    sal_mutex_take(trace.lock, sal_mutex_FOREVER);

    if (trace.cur_num_entries == 0) {
        cli_out("Empty Trace Buffer\n");

        /* Unlock access to trace buf */
        sal_mutex_give(trace.lock);

        return 0;
    } else if (trace.cur_num_entries < trace.max_num_entries) {
        entry = trace.buffer_start;
    }
    else {
        entry = trace.head;
    }

    /* Print trace oldest to newest */
    if ((entries < 0) ||
        (entries > (trace.cur_num_entries / 2))) {
        j = 0;
    } else {
        j = (trace.cur_num_entries - (entries * 2));
    }

    for (i = 0; i < trace.cur_num_entries; i++) {
        if (i >= j) {
            cli_out("%s", entry);
        }
        entry += trace.entry_length;
        if (entry == (trace.buffer_start + trace.buffer_size)) {
            entry = trace.buffer_start;
        }
    }

    /* Unlock access to trace buf */
    sal_mutex_give(trace.lock);

    return 0;
}

/* As each trace string uses two trace buffer entries,
 * Adjust the number of entries accordingly..
 */
int bsltrace_config_set(int nentry, int size)
{
    int n, sz;
 
    if (!trace.lock) {
        cli_out("Trace Buffer Not Initialized\n");
        return 0;
    }

    if ((trace.max_num_entries == (nentry * 2)) &&
        (trace.entry_length == size)) {
        /* No Changes */
        return 0;
    }
    n = (nentry > 0) ? (nentry * 2) : trace.max_num_entries;
    sz = (size > 0) ? size : trace.entry_length;

    /* Lock access to trace buf */
    sal_mutex_take(trace.lock, sal_mutex_FOREVER);

    sal_free(trace.buffer_start);

    trace.buffer_start =
        sal_alloc(sz * n, "bsltrace");
    trace.head = trace.buffer_start;
    trace.buffer_size = sz * n;
    trace.cur_num_entries = 0;
    trace.max_num_entries = n;
    trace.entry_length = sz;

    /* Unlock access to trace buf */
    sal_mutex_give(trace.lock);

    return 0;
}

int bsltrace_config_get(int *nentry, int *size)
{
    *nentry = trace.max_num_entries / 2;
    *size = trace.entry_length;
    return trace.cur_num_entries / 2;
}

int
bsltrace_init(void)
{

    bslsink_sink_t *sink;

    if (!soc_property_get(0, "tracesink", 1)) {
        return 0;
    }

    /* Create mutex lock to protect buffer access */
    if (!trace.lock) {
        trace.lock = sal_mutex_create("trace_lock");
    }

    /* Alocate and initialize trace buffer */
    trace.buffer_start =
        sal_alloc(MAX_TRACE_ENTRY_LEN * MAX_NUM_TRACE_ENTRIES, "bsltrace");

    if (trace.buffer_start == NULL) {
        return 1;
    }
    sal_memset(trace.buffer_start,
               0,
               MAX_TRACE_ENTRY_LEN * MAX_NUM_TRACE_ENTRIES);

    /* Initialize the trace buffer management info */
    trace.head = trace.buffer_start;
    trace.buffer_size = MAX_TRACE_ENTRY_LEN * MAX_NUM_TRACE_ENTRIES;
    trace.cur_num_entries = 0;
    trace.max_num_entries = MAX_NUM_TRACE_ENTRIES;
    trace.entry_length = MAX_TRACE_ENTRY_LEN;

    /* Create and configure the trace sink */
    sink = &trace_sink;
    bslsink_sink_t_init(sink);
    sal_strncpy(sink->name, "trace", sizeof(sink->name));
    sink->vfprintf = bsltrace_vfprintf;
    sink->cleanup = bsltrace_cleanup;
    sink->enable_range.min = bslSeverityDebug;
    sink->enable_range.max = bslSeverityDebug;

    /* Configure the trace prefix */
    sal_strncpy(sink->prefix_format, "%u:%L%S%s (%P:%T):",
                sizeof(sink->prefix_format));
    sink->prefix_range.min = bslSeverityDebug;
    sink->prefix_range.max = bslSeverityDebug;

    bslsink_sink_add(sink);

    return 0;
}
