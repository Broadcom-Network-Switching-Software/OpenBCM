/*! \file bsl.h
 *
 * Broadcom System Log (BSL)
 *
 * These defines for INTERNAL driver usage only, i.e. not application
 * code.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BSL_H
#define BSL_H

#include <bsl/bsl_types.h>

/*!
 * By default all log messages are compiled into the library.
 *
 * By overriding the default value it is possible to exclude debug and
 * verbose messages from the library. This will improve the overall
 * performance slightly, all depending on how heavily these messages
 * are used in a given sub-component.
 *
 * If compiler optimizations are enabled, the driver footprint will be
 * reduced as well.
 *
 * The log level value represents the maximum severity of messages
 * enabled (\ref bsl_severity_t). For example to exclude both debug
 * and verbose messages, BSL_CONFIG_LOG_LEVEL should be set to 4 (\ref
 * BSL_SEV_INFO).
 */
#ifndef BSL_CONFIG_LOG_LEVEL
#define BSL_CONFIG_LOG_LEVEL    7
#endif

/*! Check if verbose logging is compiled. */
#define BSL_CONFIG_LOG_VERBOSE  (BSL_CONFIG_LOG_LEVEL >= 5)

/*! Check if debug logging is compiled. */
#define BSL_CONFIG_LOG_DEBUG    (BSL_CONFIG_LOG_LEVEL >= 6)

/*! Macro for specifying the current source file. */
#ifndef BSL_FILE
#define BSL_FILE    __FILE__
#endif

/*! Macro for specifying the current source file line number. */
#ifndef BSL_LINE
#define BSL_LINE    __LINE__
#endif

/*! Assume that compiler can provide current function name. */
#ifndef BSL_HAS_FUNC
#define BSL_HAS_FUNC            1
#endif

/*! Assume that compiler is C99-compliant. */
#ifndef BSL_USE_C99_FUNC
#define BSL_USE_C99_FUNC        1
#endif

/*! Macro for specifying the current function name. */
#ifndef BSL_FUNC
#  if BSL_HAS_FUNC
#    if BSL_USE_C99_FUNC
#      define BSL_FUNC    __func__
#    else
#      define BSL_FUNC    __FUNCTION__
#    endif
#  else
#    define BSL_FUNC    "<nofunc>"
#  endif
#endif

/*! Format checks do not need to be portable */
#ifndef BSL_ATTR
#  if defined(__GNUC__) && !defined(__PEDANTIC__)
#    define BSL_ATTR(_a) __attribute__(_a)
#  else
#    define BSL_ATTR(_a)
#  endif
#endif

/*! Message macro which includes the standard meta data. */
#define BSL_META(str_) \
    "<c=%uf=%sl=%dF=%s>" \
    str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC

/*! Message macro which include a unit number. */
#define BSL_META_U(u_, str_) \
    "<c=%uf=%sl=%dF=%su=%d>" \
    str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, u_

/*! Message macro which include a unit and a port number. */
#define BSL_META_UP(u_, p_, str_) \
    "<c=%uf=%sl=%dF=%su=%dp=%d>" \
    str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, u_, p_

/*! Message macro which include a unit and a port number. */
#define BSL_META_UX(u_, x_, str_) \
    "<c=%uf=%sl=%dF=%su=%dx=%d>" \
    str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, u_, x_

/*! Message macro which include unit, port and some extra parameter. */
#define BSL_META_UPX(u_, p_, x_, str_) \
    "<c=%uf=%sl=%dF=%su=%dp=%dx=%d>" \
    str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, u_, p_, x_

/*!
 * Same as \ref BSL_META, but with a formatting option (\ref
 * BSL_META_OPT_xxx).
 */
#define BSL_META_O(opt_, str_) \
    "<c=%uf=%sl=%dF=%so=%u>" \
    str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, opt_

/*!
 * Same as \ref BSL_META_U, but with a formatting option (\ref
 * BSL_META_OPT_xxx).
 */
#define BSL_META_OU(opt_, u_, str_) \
    "<c=%uf=%sl=%dF=%so=%uu=%d>" \
    str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, opt_, u_

/*!
 * Same as \ref BSL_META_UP, but with a formatting option (\ref
 * BSL_META_OPT_xxx).
 */
#define BSL_META_OUP(opt_, u_, p_, str_) \
    "<c=%uf=%sl=%dF=%so=%uu=%dp=%d>" \
    str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, opt_, u_, p_

/*!
 * Same as \ref BSL_META_UPX, but with a formatting option (\ref
 * BSL_META_OPT_xxx).
 */
#define BSL_META_OUPX(opt_, u_, p_, x_, str_) \
    "<c=%uf=%sl=%dF=%so=%uu=%dp=%dx=%d>" \
    str_, mchk_, BSL_FILE, BSL_LINE, BSL_FUNC, opt_, u_, p_, x_

/*!
 * \brief Macro for invoking "fast" checker.
 *
 * The "fast" checker is normally a simple table lookup to see if a
 * message should be printed with the current output settings.
 *
 * The underlying checker function is application-provided.
 *
 * The local mchk_ variable is used by the BSL_META macro, if this
 * macro is part of stuff_ (which is the most common case).
 */
#define BSL_LOG(chk_, stuff_) do {              \
        if (bsl_fast_check(chk_)) {             \
            unsigned int mchk_ = chk_;          \
            (void)mchk_;                        \
            bsl_printf stuff_;                  \
        }                                       \
    } while (0)

/*!
 * \brief Macro for disabled log levels.
 *
 * This macro optionally replaces the \ref BSL_LOG macro.
 *
 * The macro has the same format as the \ref BSL_LOG macro in order to
 * avoid compiler warnings about unused variables and functions.
 *
 * An optimizing compiler is expected to eliminate unused/dead code.
 */
#define BSL_DISABLE(chk_, stuff_) do {              \
        if (0) {             \
            unsigned int mchk_ = chk_;          \
            (void)mchk_;                        \
            bsl_printf stuff_;                  \
        }                                       \
    } while (0)

/*! Any layer log macro for fatal error messages. */
#define BSL_LOG_FATAL(ls_, stuff_)      BSL_LOG(ls_|BSL_FATAL, stuff_)

/*! Any layer log macro for error messages. */
#define BSL_LOG_ERROR(ls_, stuff_)      BSL_LOG(ls_|BSL_ERROR, stuff_)

/*! Any layer log macro for warning messages. */
#define BSL_LOG_WARN(ls_, stuff_)       BSL_LOG(ls_|BSL_WARN, stuff_)

/*! Any layer log macro for informational messages. */
#define BSL_LOG_INFO(ls_, stuff_)       BSL_LOG(ls_|BSL_INFO, stuff_)

/*! Any layer log macro for verbose messages. */
#if BSL_CONFIG_LOG_VERBOSE
#define BSL_LOG_VERBOSE(ls_, stuff_)    BSL_LOG(ls_|BSL_VERBOSE, stuff_)
#else
#define BSL_LOG_VERBOSE(ls_, stuff_)    BSL_DISABLE(ls_|BSL_VERBOSE, stuff_)
#endif

/*! Any layer log macro for debug messages. */
#if BSL_CONFIG_LOG_DEBUG
#define BSL_LOG_DEBUG(ls_, stuff_)      BSL_LOG(ls_|BSL_DEBUG, stuff_)
#else
#define BSL_LOG_DEBUG(ls_, stuff_)      BSL_DISABLE(ls_|BSL_DEBUG, stuff_)
#endif

/*! Default CLI message layer/source/severity. */
#define BSL_LSS_CLI                     (BSL_L_APPL | BSL_S_SHELL | BSL_INFO)

/*! Shell output from core driver. */
#define BSL_LOG_CLI(stuff_)             BSL_LOG(BSL_LSS_CLI, stuff_)

/*! Shell output from CLI (interactive shell). */
#define cli_out bsl_printf

/*! Wrapper macro for layer/source/severity checker. */
#define BSL_LOG_CHECK(packed_meta_)     bsl_fast_check(packed_meta_)

/*! Any layer checker macro for fatal error messages. */
#define BSL_LOG_CHECK_FATAL(ls_)        BSL_LOG_CHECK(ls_|BSL_FATAL)

/*! Any layer checker macro for error messages. */
#define BSL_LOG_CHECK_ERROR(ls_)        BSL_LOG_CHECK(ls_|BSL_ERROR)

/*! Any layer checker macro for warning messages. */
#define BSL_LOG_CHECK_WARN(ls_)         BSL_LOG_CHECK(ls_|BSL_WARN)

/*! Any layer checker macro for informational messages. */
#define BSL_LOG_CHECK_INFO(ls_)         BSL_LOG_CHECK(ls_|BSL_INFO)

/*! Any layer checker macro for verbose messages. */
#define BSL_LOG_CHECK_VERBOSE(ls_)      BSL_LOG_CHECK(ls_|BSL_VERBOSE)

/*! Any layer checker macro for debug messages. */
#if BSL_CONFIG_LOG_DEBUG
#define BSL_LOG_CHECK_DEBUG(ls_)        BSL_LOG_CHECK(ls_|BSL_DEBUG)
#else
#define BSL_LOG_CHECK_DEBUG(ls_)        0
#endif

/*!
 * \name Short-hand logging macros.
 * \anchor LOG_xxx
 *
 * The following short-hand notations should be the preferred across
 * the entire SDK code base, however by defining the primary macros
 * with a BSL_ prefix, this header file can coexist with application
 * header files (e.g. syslog.h) which define their own LOG_xxx macros
 * with conflicting names.
 */

/*! \{ */

/*! Short-hand for \ref BSL_LOG_FATAL. */
#ifndef LOG_FATAL
#define LOG_FATAL               BSL_LOG_FATAL
#endif

/*! Short-hand for \ref BSL_LOG_ERROR. */
#ifndef LOG_ERROR
#define LOG_ERROR               BSL_LOG_ERROR
#endif

/*! Short-hand for \ref BSL_LOG_WARN. */
#ifndef LOG_WARN
#define LOG_WARN                BSL_LOG_WARN
#endif

/*! Short-hand for \ref BSL_LOG_INFO. */
#ifndef LOG_INFO
#define LOG_INFO                BSL_LOG_INFO
#endif

/*! Short-hand for \ref BSL_LOG_VERBOSE. */
#ifndef LOG_VERBOSE
#define LOG_VERBOSE             BSL_LOG_VERBOSE
#endif

/*! Short-hand for \ref BSL_LOG_DEBUG. */
#ifndef LOG_DEBUG
#define LOG_DEBUG               BSL_LOG_DEBUG
#endif

/*! Short-hand for \ref BSL_LOG_CLI. */
#ifndef LOG_CLI
#define LOG_CLI                 BSL_LOG_CLI
#endif

/*! Short-hand for \ref BSL_LOG_CHECK. */
#ifndef LOG_CHECK
#define LOG_CHECK               BSL_LOG_CHECK
#endif

/*! Short-hand for \ref BSL_LOG_CHECK_FATAL. */
#ifndef LOG_CHECK_FATAL
#define LOG_CHECK_FATAL         BSL_LOG_CHECK_FATAL
#endif

/*! Short-hand for \ref BSL_LOG_CHECK_ERROR. */
#ifndef LOG_CHECK_ERROR
#define LOG_CHECK_ERROR         BSL_LOG_CHECK_ERROR
#endif

/*! Short-hand for \ref BSL_LOG_CHECK_WARN. */
#ifndef LOG_CHECK_WARN
#define LOG_CHECK_WARN          BSL_LOG_CHECK_WARN
#endif

/*! Short-hand for \ref BSL_LOG_CHECK_INFO. */
#ifndef LOG_CHECK_INFO
#define LOG_CHECK_INFO          BSL_LOG_CHECK_INFO
#endif

/*! Short-hand for \ref BSL_LOG_CHECK_VERBOSE. */
#ifndef LOG_CHECK_VERBOSE
#define LOG_CHECK_VERBOSE       BSL_LOG_CHECK_VERBOSE
#endif

/*! Short-hand for \ref BSL_LOG_CHECK_DEBUG. */
#ifndef LOG_CHECK_DEBUG
#define LOG_CHECK_DEBUG         BSL_LOG_CHECK_DEBUG
#endif

/*! \} */

/*!
 * \brief Check if log message should be output.
 *
 * Wrapper for application-provided function to check whether a log
 * message should be passed to the output function at all.
 *
 * Also see \ref bsl_check_hook_f.
 *
 * \param [in] chk Packed meta data.
 *
 * \retval true if message should be passed to the output function.
 * \retval false if message should be skipped.
 */
extern int
bsl_fast_check(bsl_packed_meta_t chk);

/*!
 * \brief <brief-description>
 *
 * Initialize (clear) a meta data structure.
 *
 * \param [in] meta Meta data structure.
 *
 * \return Nothing.
 */
extern void
bsl_meta_t_init(bsl_meta_t *meta);

/*!
 * \brief Function to output meta-tagged log message.
 *
 * This function will extract embedded meta data from the format
 * string and build a proper meta data structure, which can be passed
 * to the application-provided output function.
 *
 * Also see \ref bsl_vlog.
 *
 * \param [in] format A printf-style format string.
 * \param [in] args List of printf-style arguments.
 *
 * \return Negative value on error.
 */
extern int
bsl_vprintf(const char *format, va_list args)
    BSL_ATTR ((format (printf, 1, 0)));

/*!
 * \brief Function to output meta-tagged log message.
 *
 * See \ref bsl_vprintf for details.
 *
 * \param [in] format A printf-style format string.
 *
 * \return Negative value on error.
 */
extern int
bsl_printf(const char *format, ...)
    BSL_ATTR ((format (printf, 1, 2)));

/*!
 * \brief Function to output log message.
 *
 * Wrapper for an application-provided function for outputting log
 * messages to one or more output sinks (terminal, file, etc.).
 *
 * Also see \ref bsl_out_hook_f.
 *
 * \param [in] meta Meta data for this log message.
 * \param [in] format A printf-style format string.
 * \param [in] args List of printf-style arguments.
 *
 * \return Negative value on error.
 */
extern int
bsl_vlog(bsl_meta_t *meta, const char *format, va_list args)
    BSL_ATTR ((format (printf, 2, 0)));

/*!
 * \brief Function to output log message.
 *
 * See \ref bsl_vlog for details.
 *
 * \param [in] meta Meta data for this log message.
 * \param [in] format A printf-style format string.
 *
 * \return Negative value on error.
 */
extern int
bsl_log(bsl_meta_t *meta, const char *format, ...)
    BSL_ATTR ((format (printf, 2, 3)));

#endif /* BSL_H */
