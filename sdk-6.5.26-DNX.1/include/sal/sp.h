/*! \file sal_sp.h
 *
 * Default implementation of single-producer variable types.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_SP_H
#define SAL_SP_H

#ifndef SAL_SP_VAR_INIT

#include <sal/types.h>

/*!
 * \brief Basic single-producer integer type.
 *
 * The SDK uses atomic single-producer (SP) data types in several
 * places, typically for lock-free state variables. Some code analysis
 * tools may flag such variables as potential sources of race
 * conditions, unless a proper memory barrier is in place.
 *
 * In order to avoid warnings from such tools, while at the same time
 * avoiding unnecessary code complexity and performance reductions, a
 * special data type is provided for such variables. The data type and
 * its associated accessor functions can be optimized for a given
 * hardware platform as needed.
 *
 * For C11-enabled compilers, the SP data type and accessor functions
 * can be mapped to the native C11 atomic API. Please refer to
 * sal_stdc.h for an example of this.
 *
 * Example:
 *
 * \code{.c}
 * static sal_sp_int my_var;
 *
 * {
 *     ...
 *     sal_sp_store_int(&my_var, 4);
 *     return sal_sp_load_int(&my_var);
 * }
 * \endcode
 */
typedef struct sal_sp_int_s {
    /*! Current value. */
    volatile int val;
} sal_sp_int;

/*!
 * \brief Basic single-producer uint32_t type.
 *
 * Please refer to \ref sal_sp_int for details.
 *
 * Example usage:
 *
 * \code{.c}
 * static sal_sp_int32_t my_var;
 *
 * {
 *     ...
 *     sal_sp_store_uint32_t(&my_var, 4);
 *     return sal_sp_load_uint32_t(&my_var);
 * }
 * \endcode
 */
typedef struct sal_sp_uint32_s {
    /*! Current value. */
    volatile uint32 val;
} sal_sp_uint32_t;

/*!
 * \brief Basic single-producer void pointer type.
 *
 * Please refer to \ref sal_sp_int for details.
 *
 * Example usage:
 *
 * \code{.c}
 * static sal_sp_uintptr_t my_var;
 *
 * {
 *     ...
 *     sal_sp_store_uintptr_t(&my_var, ptr);
 *     return sal_sp_load_uintptr_t(&my_var);
 * }
 * \endcode
 */
typedef struct sal_sp_uintptr_s {
    /*! Current value. */
    volatile sal_vaddr_t val;
} sal_sp_uintptr_t;

/*!
 * Initialize single-producer variable.
 *
 * Example:
 *
 * \code{.c}
 * static sal_sp_int my_var = SAL_SP_VAR_INIT(7);
 * \endcode
 *
 * \param [in] _val Initial value.
 */
#define SAL_SP_VAR_INIT(_val) { .val = (_val) }

/*!
 * Assign value to single-producer integer variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 * \param [in] val New value for single-producer variable.
 */
static inline void
sal_sp_store_int(sal_sp_int *sp, int val)
{
    sp->val = val;
}

/*!
 * Read value of single-producer integer variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 *
 * \return Current value of single-producer variable.
 */
static inline int
sal_sp_load_int(sal_sp_int *sp)
{
    return sp->val;
}

/*!
 * Add value to single-producer integer variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 * \param [in] val Value to add.
 *
 * \return Value of variable before addition.
 */
static inline int
sal_sp_add_int(sal_sp_int *sp, int val)
{
    int ret = sp->val;
    sp->val += val;
    return ret;
}

/*!
 * Subtract value from single-producer integer variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 * \param [in] val Value to subtract.
 *
 * \return Value of variable before subtraction.
 */
static inline int
sal_sp_sub_int(sal_sp_int *sp, int val)
{
    int ret = sp->val;
    sp->val -= val;
    return ret;
}

/*!
 * Assign value to single-producer uint32_t variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 * \param [in] val New value for single-producer variable.
 */
static inline void
sal_sp_store_uint32_t(sal_sp_uint32_t *sp, uint32 val)
{
    sp->val = val;
}

/*!
 * Read value of single-producer uint32_t variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 *
 * \return Current value of single-producer variable.
 */
static inline uint32
sal_sp_load_uint32_t(sal_sp_uint32_t *sp)
{
    return sp->val;
}

/*!
 * Add value to single-producer uint32_t variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 * \param [in] val Value to add.
 *
 * \return Value of variable before addition.
 */
static inline uint32
sal_sp_add_uint32_t(sal_sp_uint32_t *sp, uint32 val)
{
    uint32 ret = sp->val;
    sp->val += val;
    return ret;
}

/*!
 * Subtract value from single-producer uint32_t variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 * \param [in] val Value to subtract.
 *
 * \return Value of variable before subtraction.
 */
static inline uint32
sal_sp_sub_uint32_t(sal_sp_uint32_t *sp, uint32 val)
{
    uint32 ret = sp->val;
    sp->val -= val;
    return ret;
}

/*!
 * Assign value to single-producer pointer variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 * \param [in] ptr New value for single-producer variable.
 */
static inline void
sal_sp_store_uintptr_t(sal_sp_uintptr_t *sp, sal_vaddr_t ptr)
{
    sp->val = ptr;
}

/*!
 * Read value of single-producer pointer variable.
 *
 * \param [in] sp Pointer to single-producer variable.
 *
 * \return Current value of single-producer variable.
 */
static inline sal_vaddr_t
sal_sp_load_uintptr_t(sal_sp_uintptr_t *sp)
{
    return sp->val;
}

#endif /* SAL_SP_VAR_INIT */

#endif /* SAL_SP_H */
