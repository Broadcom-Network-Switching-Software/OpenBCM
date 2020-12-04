/*! \file bcma_bslsink.h
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
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BSLSINK_H
#define BCMA_BSLSINK_H

#include <stdarg.h>

#include <bsl/bsl_ext.h>

#include <shr/shr_bitop.h>

/*! Maximum length of sink name. */
#define BCMA_BSLSINK_SINK_NAME_MAX           32

/*! Maximum length of prefix format string. */
#define BCMA_BSLSINK_PREFIX_FORMAT_MAX       32

/*! Maximum length of formatted prefix string. */
#ifndef BCMA_BSLSINK_PREFIX_MAX
#define BCMA_BSLSINK_PREFIX_MAX              128
#endif

/*! Maximum number of unit supported. */
#define BCMA_BSLSINK_MAX_NUM_UNITS           16

/*! Maximum number of ports supported. */
#define BCMA_BSLSINK_MAX_NUM_PORTS           384

/*! Pseudo-unit to allow separate control of BSL_UNIT_UNKNOWN messages. */
#define BCMA_BSLSINK_UNIT_UNKNOWN            (BCMA_BSLSINK_MAX_NUM_UNITS - 1)

/*! Sink option for suppressing the special ECHO source. */
#define BCMA_BSLSINK_OPT_NO_ECHO             (1L << 0)

/*! Sink option to enable prefix string. */
#define BCMA_BSLSINK_OPT_SHELL_PREFIX        (1L << 1)

/*! Severity range. */
typedef struct bcma_bslsink_severity_range_s {
    bsl_severity_t min; /*!< Minimum severity level. */
    bsl_severity_t max; /*!< Maximum severity level. */
} bcma_bslsink_severity_range_t;

/*! Output sink object. */
typedef struct bcma_bslsink_sink_s {

    /*! Next sink. */
    struct bcma_bslsink_sink_s *next;

    /*! Sink name. */
    char name[BCMA_BSLSINK_SINK_NAME_MAX];

    /*! Unique ID for retrieval and removal. */
    int sink_id;

    /*! Options for controlling sink behavior (BSLSINK_OPT_xxx. */
    unsigned int options;

    /*! Low-level output function. */
    int (*vfprintf)(void *, const char *, va_list);

    /*! Optional function for custom filtering. */
    int (*check)(bsl_meta_t *);

    /*! Optional function for cleaning up dynamic resources. */
    int (*cleanup)(struct bcma_bslsink_sink_s *);

    /*! Opaque file handle. */
    void *file;

    /*! Messages within this severity range will be printed. */
    bcma_bslsink_severity_range_t enable_range;

    /*! Messages within this severity range will be prefixed. */
    bcma_bslsink_severity_range_t prefix_range;

    /*! Messages with these units will be printed (use -1 to skip check). */
    SHR_BITDCLNAME(units, BCMA_BSLSINK_MAX_NUM_UNITS);

    /*! Messages with these ports will be printed (use -1 to skip check). */
    SHR_BITDCLNAME(ports, BCMA_BSLSINK_MAX_NUM_PORTS);

    /*! Messages with this parameter will be printed (-1 to ignore). */
    int xtra;

    /*! Skip this many characters of the BSL_FILE path. */
    int path_offset;

    /*! Prefix configuration. */
    char prefix_format[BCMA_BSLSINK_PREFIX_FORMAT_MAX+1];

} bcma_bslsink_sink_t;

/*!
 * \brief <brief-description>
 *
 * <long-description>
 *
 * \param [in] sink Sink object handle.
 * \param [in] meta Log message meta data.
 * \param [in] format Log message format string.
 * \param [in] args Log message parameters.
 *
 * \return Number of characters sent to sink(s).
 */
extern int
bcma_bslsink_out(bcma_bslsink_sink_t *sink, bsl_meta_t *meta,
                 const char *format, va_list args);

/*!
 * \brief Initialize sink object.
 *
 * \param [in] sink Sink object handle.
 */
extern void
bcma_bslsink_sink_t_init(bcma_bslsink_sink_t *sink);

/*!
 * \brief Clean up sink resources.
 *
 * This is done by invoking the cleanup function provided by the sink
 * itself.
 *
 * \param [in] sink Sink object handle.
 *
 * \return Always 0.
 */
extern int
bcma_bslsink_sink_cleanup(bcma_bslsink_sink_t *sink);

/*!
 * \brief Add sink to the list of installed output sinks.
 *
 * Output sinks are organized as a linked list, which is traversed for
 * each log messages.
 *
 * \param [in] new_sink Sink object handle.
 *
 * \return Sink ID assigned to this sink.
 */
extern int
bcma_bslsink_sink_add(bcma_bslsink_sink_t *new_sink);

/*!
 * \brief Find sink object based on sink name.
 *
 * Look for sink with the specified name in the list of installed
 * sinks.
 *
 * \param [in] name Sink name.
 *
 * \return Sink object handler if found, otherwise NULL.
 */
extern bcma_bslsink_sink_t *
bcma_bslsink_sink_find(const char *name);

/*!
 * \brief Find sink object based on sink ID.
 *
 * Look for sink with the specified sink ID in the list of installed
 * sinks.
 *
 * \param [in] sink_id Sink ID.
 *
 * \return Sink object handler if found, otherwise NULL.
 */
extern bcma_bslsink_sink_t *
bcma_bslsink_sink_find_by_id(int sink_id);

/*!
 * \brief Clean up sink module.
 *
 * Traverse list of installed sinks and free associated resources.
 *
 * \return Always 0.
 */
extern int
bcma_bslsink_cleanup(void);

/*!
 * \brief Initialize sink module.
 *
 * \return Always 0.
 */
extern int
bcma_bslsink_init(void);

#endif /* BCMA_BSLSINK_H */
