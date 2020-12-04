/*! \file shr_atmoic_int.c
 *
 * Atomic integer variable support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_alloc.h>
#include <shr/shr_atomic.h>

/* Atomic integer type */
typedef struct atomic_int_ctrl_s {
    sal_spinlock_t lock;
    int val;
} atomic_int_ctrl_t;

shr_atomic_int_t
shr_atomic_int_create(int val)
{
    atomic_int_ctrl_t *aic;

    aic = sal_alloc(sizeof(*aic), "salAtomicInt");;
    if (aic) {
        aic->val = val;
        aic->lock = sal_spinlock_create("salAtomicInt");
        if (aic->lock == NULL) {
            sal_free(aic);
            aic = NULL;
        }
    }
    return (shr_atomic_int_t)aic;
}

void
shr_atomic_int_destroy(shr_atomic_int_t aint)
{
    atomic_int_ctrl_t *aic = (atomic_int_ctrl_t *)aint;

    if (aic) {
        if (aic->lock) {
            sal_spinlock_destroy(aic->lock);
        }
        sal_free(aic);
    }
}

int
shr_atomic_int_get(shr_atomic_int_t aint)
{
    atomic_int_ctrl_t *aic = (atomic_int_ctrl_t *)aint;

    if (aic) {
        return aic->val;
    }
    return 0;
}

void
shr_atomic_int_set(shr_atomic_int_t aint, int val)
{
    atomic_int_ctrl_t *aic = (atomic_int_ctrl_t *)aint;

    if (aic && aic->lock) {
        sal_spinlock_lock(aic->lock);
        aic->val = val;
        sal_spinlock_unlock(aic->lock);
    }
}

int
shr_atomic_int_add(shr_atomic_int_t aint, int val)
{
    atomic_int_ctrl_t *aic = (atomic_int_ctrl_t *)aint;
    int rv = 0;

    if (aic && aic->lock) {
        sal_spinlock_lock(aic->lock);
        aic->val += val;
        rv = aic->val;
        sal_spinlock_unlock(aic->lock);
    }
    return rv;
}

int
shr_atomic_int_incr(shr_atomic_int_t aint)
{
    return shr_atomic_int_add(aint, 1);
}

int
shr_atomic_int_decr(shr_atomic_int_t aint)
{
    return shr_atomic_int_add(aint, -1);
}
