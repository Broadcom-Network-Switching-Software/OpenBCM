/*! \file shr_debug.h
 *
 * Generic macros for tracing function call trees.
 *
 * The main principle is to establish a single point of exit for each
 * function, and then combine this with a standard method of logging
 * error conditions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_DEBUG_H
#define SHR_DEBUG_H

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_exception.h>
#include <sal/sal_alloc.h>

/*! Default value for logging function enter/exit. */
#ifndef SHR_FUNC_DEBUG
#define SHR_FUNC_DEBUG  0
#endif

/*! Allow function enter/exit logging to be compiled out. */
#if (SHR_FUNC_DEBUG == 1)
#define SHR_LOG_DEBUG(ls_, stuff_) LOG_DEBUG(ls_, stuff_)
#else
#define SHR_LOG_DEBUG(ls_, stuff_) (void)_func_unit
#endif

/*! Allow error trace logging to be compiled out. */
#ifndef SHR_ERROR_TRACE
#define SHR_ERROR_TRACE 1
#endif

/*! Use dedicated source if available. */
#ifdef BSL_LS_SHR_ETRACE
#define SHR_ETRACE_LOG_MODULE BSL_LS_SHR_ETRACE
#else
#define SHR_ETRACE_LOG_MODULE BSL_LOG_MODULE
#endif

/*! Log format for normal error trace messages. */
#if (SHR_ERROR_TRACE == 1)
#define SHR_ERR_TRACE(stuff_) LOG_VERBOSE(SHR_ETRACE_LOG_MODULE, stuff_)
#else
#define SHR_ERR_TRACE(stuff_)
#endif

/*! Log format for verbose error trace messages. */
#if (SHR_ERROR_TRACE == 1)
#define SHR_ERR_VTRACE(stuff_) LOG_DEBUG(SHR_ETRACE_LOG_MODULE, stuff_)
#else
#define SHR_ERR_VTRACE(stuff_)
#endif

/*! Allow exception handler support to be compiled out. */
#ifndef SHR_ERROR_EXCEPTION
#define SHR_ERROR_EXCEPTION 1
#endif

/*! Request exception processing from application. */
#if (SHR_ERROR_EXCEPTION == 1)
#define SHR_EXCEPTION(_unit, _ec, _file, _line, _func)   \
    shr_exception(_unit, _ec, _file, _line, _func)
#else
#define SHR_EXCEPTION(_unit, _ec, _file, _line, _func)
#endif

/*!
 * \brief Function entry declarations and code.
 *
 * This macro must appear in each function right after the local
 * variable declarations.
 *
 * The macro declares a temporary return value variable for use by the
 * error checking macros, because if this variable is declared locally
 * within each macro, the stack usage increases significantly if error
 * checking macros are used extensively in a single function.
 *
 * The associated debug log message will use BSL_LOG_MODULE as the log
 * source, so this name must be assigned to a relevant BSL log source
 * in advance.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *     ...
 * }
 * \endcode
 *
 * \param [in] _unit Switch unit number.
 */
#define SHR_FUNC_ENTER(_unit)                           \
    int _func_unit = _unit;                             \
    shr_error_t _func_rv = SHR_E_NONE;                  \
    shr_error_t _tmp_rv;                                \
    (void)_tmp_rv;                                      \
    SHR_LOG_DEBUG(BSL_LOG_MODULE,                       \
                  (BSL_META_U(_func_unit, "enter\n")));

/*!
 * \brief Single point of exit code.
 *
 * This macro must appear at the very bottom of each function, and it
 * must be preceded an 'exit' label and optionally some resource
 * clean-up code.
 *
 * The associated debug log message will use BSL_LOG_MODULE as the log
 * source, so this name must be assigned to a relevant BSL log source
 * in advance.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 */
#define SHR_FUNC_EXIT()                                 \
    SHR_LOG_DEBUG(BSL_LOG_MODULE,                       \
                  (BSL_META_U(_func_unit, "exit\n")));  \
    return _func_rv;

/*!
 * \brief Goto single point of exit.
 *
 * Go to label 'exit', which must be defined explicitly in each
 * function.
 *
 * This macro is normally not called directly, but it is used as part
 * of other error handling macros.
 */
#define SHR_EXIT() goto exit

/*!
 * \brief Default error log message.
 *
 * Construct a basic log message based on the currently assigned error
 * code.
 */
#define SHR_ERR_STR()                                           \
    (BSL_META_U(_func_unit, "%s\n"), shr_errmsg(_func_rv))

/*!
 * \brief Assign function return value and exit with verbose message.
 *
 * Log a verbose custom message and go to the function's single point
 * of exit.
 *
 * The error message must be specified as a single expression using
 * one of the BSL_META_xxx macros.
 *
 * Since no standard trace message is logged, this macro should not be
 * used for normal error handling.
 *
 * \param [in] _rv Return value.
 * \param [in] _stuff Custom print statement (see description).
 */
#define SHR_VERBOSE_MSG_EXIT(_rv, _stuff)       \
    do {                                        \
        _func_rv = _rv;                         \
        SHR_ERR_VTRACE(_stuff);                 \
        SHR_EXIT();                             \
    } while (0)

/*!
 * \brief Assign function return value and exit with verbose message.
 *
 * Since no standard trace message is logged, this macro should not be
 * used for normal error handling.
 *
 * \param [in] _rv Return value.
 */
#define SHR_VERBOSE_EXIT(_rv)                   \
    SHR_VERBOSE_MSG_EXIT(_rv, SHR_ERR_STR())

/*!
 * \brief Warn on expression error.
 *
 * Evaluate an expression and log a warning message if the expression
 * evaluates to a standard error code. The result is assigned to the
 * macro-based function return value (declared by \ref
 * SHR_FUNC_ENTER), but code execution continues immediately below the
 * statement (i.e. no jump to the single point of exit).
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     SHR_IF_ERR_CONT(some_other_func(unit));
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate.
 */
#define SHR_IF_ERR_CONT(_expr)                  \
    do {                                        \
        _tmp_rv = _expr;                        \
        if (SHR_FAILURE(_tmp_rv)) {             \
            _func_rv = _tmp_rv;                 \
            SHR_ERR_TRACE(SHR_ERR_STR());       \
        }                                       \
    } while (0)

/*!
 * \brief Error-exit with custom message.
 *
 * Log a custom error message and go to the function's single point of
 * exit.
 *
 * The error message must be specified as a single expression using
 * one of the BSL_META_xxx macros.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     if (drv_lookup(unit, local_idx) == NULL) {
 *         SHR_ERR_MSG_EXIT
 *             (SHR_E_NOT_FOUND,
 *              (BSL_META_U(unit,
 *                          "Operation failed for instance %d\n"),
 *               local_idx));
 *     }
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _rv Error code.
 * \param [in] _stuff Custom print statement (see description).
 */
#define SHR_ERR_MSG_EXIT(_rv, _stuff)                   \
    do {                                                \
        _func_rv = _rv;                                 \
        SHR_ERR_TRACE(_stuff);                          \
        SHR_EXCEPTION(_func_unit, _func_rv,             \
                      BSL_FILE, BSL_LINE, BSL_FUNC);    \
        SHR_EXIT();                                     \
    } while (0)

/*!
 * \brief Error-exit.
 *
 * Log an error message and go to the function's single point of exit.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     if (drv_lookup(unit, local_idx) == NULL) {
 *         SHR_ERR_EXIT(SHR_E_NOT_FOUND);
 *     }
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _rv Error code.
 */
#define SHR_ERR_EXIT(_rv)                       \
    SHR_ERR_MSG_EXIT(_rv, SHR_ERR_STR())

/*!
 * \brief Error-exit on expression error.
 *
 * Evaluate an expression and if it evaluates to a standard error
 * code, then log an error message and go to the function's single
 * point of exit.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     SHR_IF_ERR_EXIT
 *         (some_other_func(unit));
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate.
 */
#define SHR_IF_ERR_EXIT(_expr)                  \
    do {                                        \
        _tmp_rv = _expr;                        \
        if (SHR_FAILURE(_tmp_rv)) {             \
            SHR_ERR_EXIT(_tmp_rv);              \
        }                                       \
    } while (0)

/*!
 * \brief Verbose-exit on expression error.
 *
 * Similar to \ref SHR_IF_ERR_EXIT, but the error message is logged as
 * a verbose error trace, i.e. it will not be shown if only the
 * standard error trace messages are enabled.
 *
 * \param [in] _expr Expression to evaluate.
 */
#define SHR_IF_ERR_VERBOSE_EXIT(_expr)          \
    do {                                        \
        _tmp_rv = _expr;                        \
        if (SHR_FAILURE(_tmp_rv)) {             \
            SHR_VERBOSE_EXIT(_tmp_rv);          \
        }                                       \
    } while (0)

/*!
 * \brief Error-exit on expression error with exception.
 *
 * Similar to \ref SHR_IF_ERR_EXIT, but if the error code matches the
 * exceptions value, then the error message is logged as a verbose
 * error trace, i.e. it will not be shown if only the standard error
 * trace messages are enabled. Typical exception values are
 * SHR_E_NOT_FOUND and SHR_E_UNAVAIL.
 *
 * \param [in] _expr Expression to evaluate.
 * \param [in] _rv_verbose Error code for which to use verbose message.
 */
#define SHR_IF_ERR_EXIT_VERBOSE_IF(_expr, _rv_verbose)  \
    do {                                                \
        _tmp_rv = _expr;                                \
        if (SHR_FAILURE(_tmp_rv)) {                     \
            if (_tmp_rv == _rv_verbose) {               \
                SHR_VERBOSE_EXIT(_tmp_rv);              \
            }                                           \
            SHR_ERR_EXIT(_tmp_rv);                      \
        }                                               \
    } while (0)

/*!
 * \brief Exit on expression error with custom message.
 *
 * Evaluate an expression and if it evaluates to a standard error
 * code, then log a custom error message and go to the function's
 * single point of exit.
 *
 * The error message must be specified as a single expression using
 * one of the BSL_META_xxx macros.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     SHR_IF_ERR_MSG_EXIT
 *         (some_other_func(unit),
 *          (BSL_META_U(unit,
 *                      "Operation failed for instance %d\n"),
 *           local_idx));
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate.
 * \param [in] _stuff Custom print statement (see description).
 */
#define SHR_IF_ERR_MSG_EXIT(_expr, _stuff)      \
    do {                                        \
        _tmp_rv = _expr;                        \
        if (SHR_FAILURE(_tmp_rv)) {             \
            SHR_ERR_MSG_EXIT(_tmp_rv, _stuff);  \
        }                                       \
    } while (0)

/*!
 * \brief Error-exit on expression error with exception.
 *
 * Evaluate an expression and if it evaluates to a standard error code
 * which differs from the exception value, then log an error message
 * and go to the function's single point of exit.
 *
 * Typical exception error codes are \ref SHR_E_NOT_FOUND and \ref
 * SHR_E_UNAVAIL, because these error codes may be a simple reflection
 * of the device state or capabilties, which are used to control the
 * logical flow of the function.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int rv;
 *
 *     SHR_FUNC_ENTER(unit);
 *
 *     rv = some_other_func(unit);
 *     SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
 *     ...
 *
 *   exit:
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate.
 * \param [in] _rv_except Error code to ignore.
 */
#define SHR_IF_ERR_EXIT_EXCEPT_IF(_expr, _rv_except)    \
    do {                                                \
        _tmp_rv = _expr;                                \
        if (SHR_FAILURE(_tmp_rv) &&                     \
            _tmp_rv != (shr_error_t)_rv_except) {       \
            SHR_ERR_EXIT(_tmp_rv);                      \
        }                                               \
    } while (0)

/*!
 * \brief Check for function error state.
 *
 * This macro is a Boolean expression, which evaluates to TRUE, if the
 * macro-based function return value (declared by \ref SHR_FUNC_ENTER)
 * is set to error.
 *
 * It can be used to clean up allocated resources in case of failure,
 * for example:
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *     ...
 *
 *   exit:
 *     if (SHR_FUNC_ERR()) {
 *         ...
 *     }
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 */
#define SHR_FUNC_ERR()                          \
    SHR_FAILURE(_func_rv)

/*!
 * \brief Check for specific function error value.
 *
 * This macro is a Boolean expression, which evaluates to TRUE, if the
 * macro-based function return value (declared by \ref SHR_FUNC_ENTER)
 * matches the specified value.
 *
 * It can be used to take special actions for specific error codes.
 *
 * \code{.c}
 * void my_func(int unit)
 * {
 *     int local_idx;
 *
 *     SHR_FUNC_ENTER(unit);
 *     ...
 *     alloc_resources();
 *     ...
 *
 *   exit:
 *     if (SHR_FUNC_VAL_IS(SHR_E_UNAVAIL)) {
 *         free_resources();
 *     }
 *     SHR_FUNC_EXIT();
 * }
 * \endcode
 *
 * \param [in] _rv Return value to check against.
 */
#define SHR_FUNC_VAL_IS(_rv)                    \
    (_func_rv == (_rv))


/*!
 * \brief Check for null-pointer.
 *
 * Check if a pointer is NULL, and if so, log an error and exit.
 *
 * The macro is intended for both input parameter checks and memory
 * allocation errors.
 *
 * \param [in] _ptr Pointer to check.
 * \param [in] _rv Function return value to use if pointer is NULL.
 */
#define SHR_NULL_CHECK(_ptr, _rv)                       \
    do {                                                \
        if ((_ptr) == NULL) {                           \
            SHR_ERR_MSG_EXIT(_rv,                       \
                             (BSL_META_U(_func_unit,    \
                                         "null\n")));   \
        }                                               \
    } while (0)

/*!
 * \brief Memory allocation with leak check.
 *
 * This macro is intended to prevent memory leaks from premature
 * pointer recycling. This type of leak happens if a pointer to an
 * allocated block of memory is used for another memory allocation
 * before the first block is freed.
 *
 * If the above error is detected, the macro will log an error and
 * exit the current function, otherwise a normall memory allocation
 * will be done using sal_alloc.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * The macro depends on \ref SHR_FUNC_ENTER and may be used in
 * conjunction with \ref SHR_FREE.
 *
 * \param [in] _ptr Pointer variable to use for memory allocation.
 * \param [in] _sz Amount of memory to allocate (in bytes).
 * \param [in] _str Debug string to be passed to sal_alloc.
 */
#define SHR_ALLOC(_ptr, _sz, _str)                      \
    do {                                                \
        if ((_ptr) == NULL) {                           \
            (_ptr) = sal_alloc(_sz, _str);              \
        } else {                                        \
            LOG_ERROR(BSL_LOG_MODULE,                   \
                      (BSL_META_U(_func_unit,           \
                                  "not null (%s)\n"),   \
                       _str));                          \
            SHR_EXIT();                                 \
        }                                               \
    } while (0)

/*!
 * \brief Free memory and reset pointer.
 *
 * This macro will free allocated memory and reset the associated
 * pointer to NULL. The memory will be freed using sal_free.
 *
 * The macro may be used in conjunction with \ref SHR_ALLOC.
 *
 * \param [in] _ptr Pointer to memory to be freed.
 */
#define SHR_FREE(_ptr)                          \
    do {                                        \
        if ((_ptr) != NULL) {                   \
            sal_free((void *)(_ptr));           \
            (_ptr) = NULL;                      \
        }                                       \
    } while (0)

#endif /* SHR_DEBUG_H */
