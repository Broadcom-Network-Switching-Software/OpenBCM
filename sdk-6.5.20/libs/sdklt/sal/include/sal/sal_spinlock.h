/*! \file sal_spinlock.h
 *
 * Spinlock API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_SPINLOCK_H
#define SAL_SPINLOCK_H

#include <sal/sal_types.h>

/*! Opaque spinlock handle. */
typedef struct sal_spinlock_s *sal_spinlock_t;

/*!
 * \brief Create spinlock.
 *
 * \param [in] desc Spinlock naming for tracking purposes.
 *
 * \return Spinlock handle or NULL on error.
 */
extern sal_spinlock_t
sal_spinlock_create(char *desc);

/*!
 * \brief Destroy spinlock.
 *
 * Free all resources associated with a spinlock.
 *
 * \param [in] lock Spinlock handle from \ref sal_spinlock_create.
 *
 * \return 0 if no errors, otherwise non-zero value.
 */
extern int
sal_spinlock_destroy(sal_spinlock_t lock);

/*!
 * \brief Acquire spinlock.
 *
 * \param [in] lock Spinlock handle from \ref sal_spinlock_create.
 *
 * \return 0 if no errors, otherwise non-zero value.
 */
extern int
sal_spinlock_lock(sal_spinlock_t lock);

/*!
 * \brief Release spinlock.
 *
 * \param [in] lock Spinlock handle from \ref sal_spinlock_create.
 *
 * \return 0 if no errors, otherwise non-zero value.
 */
extern int
sal_spinlock_unlock(sal_spinlock_t lock);

#endif /* SAL_SPINLOCK_H */
