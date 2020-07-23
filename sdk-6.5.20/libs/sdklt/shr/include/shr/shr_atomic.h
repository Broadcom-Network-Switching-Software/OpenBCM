/*! \file shr_atomic.h
 *
 * Atomic variable support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_ATOMIC_H
#define SHR_ATOMIC_H

#include <sal/sal_spinlock.h>

/*! Opaque atomic integer type. */
typedef void *shr_atomic_int_t;

/*!
 * \brief Create atomic integer.
 *
 * \param [in] val Initial value of atomic integer.
 *
 * \return Atomic integer or NULL on error.
 */
extern shr_atomic_int_t
shr_atomic_int_create(int val);

/*!
 * \brief Destroy atomic integer.
 *
 * Free resources allocated via \ref shr_atomic_int_create.
 *
 * \param [in] aint Atomic integer to destroy.
 */
extern void
shr_atomic_int_destroy(shr_atomic_int_t aint);

/*!
 * \brief Get current value of atomic integer.
 *
 * \param [in] aint Atomic integer to operate on.
 *
 * \return Current value of atomic integer.
 */
extern int
shr_atomic_int_get(shr_atomic_int_t aint);

/*!
 * \brief Set atomic integer value.
 *
 * \param [in] aint Atomic integer to operate on.
 * \param [in] val New value for atomic integer.
 */
extern void
shr_atomic_int_set(shr_atomic_int_t aint, int val);

/*!
 * \brief Add value to atomic integer.
 *
 * \param [in] aint Atomic integer to operate on.
 * \param [in] val Value to add to atomic integer.
 *
 * \return Updated value of atomic integer.
 */
extern int
shr_atomic_int_add(shr_atomic_int_t aint, int val);

/*!
 * \brief Increment atomic integer value by 1.
 *
 * \param [in] aint Atomic integer to operate on.
 *
 * \return Updated value of atomic integer.
 */
extern int
shr_atomic_int_incr(shr_atomic_int_t aint);

/*!
 * \brief Decrement atomic integer value by 1.
 *
 * \param [in] aint Atomic integer to operate on.
 *
 * \return Updated value of atomic integer.
 */
extern int
shr_atomic_int_decr(shr_atomic_int_t aint);

#endif /* SHR_ATOMIC_H */
