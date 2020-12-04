/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Sinks
 *
 * The BSL sink structure control how log messages are presented on a
 * particular sink.  A sink can be a console, a file, or some other
 * output devices.
 *
 * The sink will prefix each log message according to a preconfigured
 * setting configured via a printf-like format string.  Please consult
 * the format_prefix function in bslsink.c for a complete list of
 * format specifiers.
 *
 * A number of options can be configured to control the sink behavior:
 *
 * BSLSINK_OPT_NO_ECHO
 * This option suppresses all log messages from the ECHO source
 * (bslSourceEcho).  It is used for console sinks to avoid duplicating
 * the console echo of CLI input functions such as readline.  The CLI
 * will log the input string to allow it to be captured in a log file.
 *
 * BSLSINK_OPT_SHELL_PREFIX
 * Normally the prefix string is suppressed for log messages from the
 * SHELL sources (bslSourceShell and bslSourceEcho).  If this option
 * is set, the prefix string (if configured) will also be shown for
 * SHELL output.
 */

#ifndef _DIAG_BSLSINK_H
#define _DIAG_BSLSINK_H

#include <stdarg.h>
#include <shared/bslext.h>
#include <bcm/types.h>

#define BSLSINK_SINK_NAME_MAX           32
#define BSLSINK_PREFIX_FORMAT_MAX       32

#ifndef BSLSINK_PREFIX_MAX
#define BSLSINK_PREFIX_MAX              144
#endif

#define BSLSINK_MAX_NUM_UNITS           (BCM_UNITS_MAX + 1)
#define BSLSINK_MAX_NUM_PORTS           BCM_PBMP_PORT_MAX

/* Pseudo-unit to allow separate control of BSL_UNIT_UNKNOWN messages */
#define BSLSINK_UNIT_UNKNOWN            (BSLSINK_MAX_NUM_UNITS - 1)

/* Sink options */
#define BSLSINK_OPT_NO_ECHO             (1L << 0)
#define BSLSINK_OPT_SHELL_PREFIX        (1L << 1)

typedef struct bslsink_severity_range_s {
    bsl_severity_t min;
    bsl_severity_t max;
} bslsink_severity_range_t;

typedef struct bslsink_sink_s {

    /* Next sink */
    struct bslsink_sink_s *next;

    /* Sink name */
    char name[BSLSINK_SINK_NAME_MAX];

    /* Unique ID for retrieval and removal */
    int sink_id;

    /* Options for controlling sink behavior (BSLSINK_OPT_xxx */
    uint32 options;

    /* Low-level output function */
    int (*vfprintf)(void *, const char *, va_list);

    /* Optional function for custom filtering */
    int (*check)(bsl_meta_t *);

    /* Optional function for cleaning up dynamic resources */
    int (*cleanup)(struct bslsink_sink_s *);

    /* Opaque file handle */
    void *file;

    /* Messages within this severity range will be printed */
    bslsink_severity_range_t enable_range;

    /* Messages within this severity range will be prefixed */
    bslsink_severity_range_t prefix_range;

    /* Messages with these units will be printed (use -1 to skip check) */
    SHR_BITDCLNAME(units, BSLSINK_MAX_NUM_UNITS);

    /* Messages with these ports will be printed (use -1 to skip check) */
    SHR_BITDCLNAME(ports, BSLSINK_MAX_NUM_PORTS);

    /* Messages with this parameter will be printed (-1 to ignore) */
    int xtra;

    /* Skip this many characters of the BSL_FILE path */
    int path_offset;

    /* Prefix configuration */
    char prefix_format[BSLSINK_PREFIX_FORMAT_MAX+1];

} bslsink_sink_t;

extern int
bslsink_out(bslsink_sink_t *sink, bsl_meta_t *meta,
            const char *format, va_list args);

extern void
bslsink_sink_t_init(bslsink_sink_t *sink);

extern int
bslsink_sink_cleanup(bslsink_sink_t *sink);

extern int
bslsink_sink_add(bslsink_sink_t *new_sink);

extern bslsink_sink_t *
bslsink_sink_find(const char *name);

extern bslsink_sink_t *
bslsink_sink_find_by_id(int sink_id);

extern int
bslsink_cleanup(void);

extern int
bslsink_init(void);

#endif /* !_DIAG_BSLSINK_H */
