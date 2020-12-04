/*! \file sal_assert.h
 *
 * Configure assert macro.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_ASSERT_H
#define SAL_ASSERT_H

#include <sal/sal_types.h>

/*! Default assert behavior. */
#define	SAL_ASSERT_DEFAULT(_expr) \
    (void)((_expr) || (sal_assert(#_expr, __FILE__, __LINE__), 0))

#if SAL_CONFIG_DEFINE_ASSERT == 1
#  ifndef assert
#    ifdef NDEBUG
#      define assert(_expr) do { } while (0)
#    else
#      define assert(_expr) SAL_CONFIG_MACRO_ASSERT(_expr)
#    endif
#  endif
#endif

/*!
 * \brief Assert function.
 *
 * The assert function is called when an assert expression evaluates
 * to false. Typically the assert function will print the failing
 * expression and location and then abort the program.
 *
 * \param [in] expr Failing expression as a string.
 * \param [in] file File containing the failing expression.
 * \param [in] line Line number of the failing expression.
 *
 * \return Nothing.
 */
typedef void (*sal_assert_fn_t)(const char *expr, const char *file, int line);

/*!
 * \brief Override default assert function.
 *
 * Overriding the default assert function is useful, for example if
 * aborting the program upon an assert is not desriable.
 *
 * \param [in] fn New assert function.
 *
 * \return Nothing.
 */
extern void
sal_assert_set(sal_assert_fn_t fn);

/*!
 * \brief Wrapper function for configured assert function.
 *
 * \param [in] expr Failing expression as a string.
 * \param [in] file File containing the failing expression.
 * \param [in] line Line number of the failing expression.
 *
 * \return Nothing.
 */
extern void
sal_assert(const char *expr, const char *file, int line);

#endif /* SAL_ASSERT_H */
