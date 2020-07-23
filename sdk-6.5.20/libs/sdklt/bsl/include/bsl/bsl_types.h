/*! \file bsl_types.h
 *
 * Broadcom System Log (BSL)
 *
 * BSL data types and macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BSL_TYPES_H
#define BSL_TYPES_H

/* Required for variable number of arguments */
#include <stdarg.h>

/*! Optionally define NULL to avoid dependency on external header files. */
#ifndef NULL
#define NULL (void *)0
#endif

/* Auto-generated layers and levels */
#include <bsl/bsl_enum.h>

/*! Compact representation of log message origin. */
typedef unsigned int bsl_packed_meta_t;

/*!
 * \name Log message formatting options.
 * \anchor BSL_META_OPT_xxx
 */

/*! \{ */

/*! Indicates that this message contains the beginning of a log message. */
#define BSL_META_OPT_START      0x1

/*! Indicates that this message contains the end of a log message. */
#define BSL_META_OPT_END        0x2

/*! Request a newline between the prefix and the main log message. */
#define BSL_META_OPT_PFX_NL     0x4

/*! \} */

/*! Meta data to accompany each log message. */
typedef struct bsl_meta_s {

    /*! Log output options (see \ref BSL_META_OPT_xxx). */
    unsigned int options;

    /*! Name of function which sent this message. */
    const char *func;

    /*! Name of source file which sent this message. */
    const char *file;

    /*! Line number of source file which sent this message. */
    int line;

    /*! Logging layer for this message. */
    bsl_layer_t layer;

    /*! Logging source for this message. */
    bsl_source_t source;

    /*! Severity level for this message. */
    bsl_severity_t severity;

    /*! Unit number for this message. */
    int unit;

    /*! Port number for this message. */
    int port;

    /*! Extra (context-dependent) parameter for this message, e.g. COS queue. */
    int xtra;

} bsl_meta_t;

/*! Constant to indicate unknown unit number. */
#define BSL_UNIT_UNKNOWN    (-1)

/*!
 * \brief Function to output log message.
 *
 * This is an application-provided function for outputting log
 * messages to one or more output sinks (terminal, file, etc.).
 *
 * \param [in] meta Meta data for this log message.
 * \param [in] format A printf-style format string.
 * \param [in] args List of printf-style arguments.
 *
 * \return Negative value on error.
 */
typedef int (*bsl_out_hook_f)(bsl_meta_t *meta, const char *format, va_list args);

/*!
 * \brief Check if log message should be output.
 *
 * This application-provided function should check whether a log
 * message should be passed to the output function at all.
 *
 * The function wil be called for all log messages in the execution
 * path, so it must be highly optimized.
 *
 * \param [in] meta_pack Packed meta data.
 *
 * \retval true if message should be passed to the output function.
 * \retval false if message should be skipped.
 */
typedef int (*bsl_check_hook_f)(bsl_packed_meta_t meta_pack);

/*! BSL configuration structure provided by the application. */
typedef struct bsl_config_s {

    /*! Function to output log messages. */
    bsl_out_hook_f out_hook;

    /*! Function to check if log message should be output. */
    bsl_check_hook_f check_hook;

} bsl_config_t;

#endif /* BSL_TYPES_H */
