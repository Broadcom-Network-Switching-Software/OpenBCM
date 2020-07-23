/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    protect.c
 *
 * Purpose: 
 *
 * Functions:
 *      _bcm_ptp_intr_context
 *      _bcm_ptp_mutex_create
 *      _bcm_ptp_mutex_destroy
 *      _bcm_ptp_mutex_take
 *      _bcm_ptp_mutex_give
 *      _bcm_ptp_sem_create
 *      _bcm_ptp_sem_destroy
 *      _bcm_ptp_sem_take
 *      _bcm_ptp_sem_give
 */

#if defined(INCLUDE_PTP)

/* ---------------------------------------------------------------------------------------------- */
/* ABSTRACT: Allow work around for broken SAL semaphore and mutex issues                          */
/* ---------------------------------------------------------------------------------------------- */

#include <sal/core/time.h>

#include <bcm_int/common/ptp.h>

/* ============================= */
/* Mutex functions               */
/* ============================= */

_bcm_ptp_mutex_t
_bcm_ptp_mutex_create(char *desc)
{
    return sal_mutex_create(desc);
}

void
_bcm_ptp_mutex_destroy(_bcm_ptp_mutex_t m)
{
    sal_mutex_destroy(m);
}

int
_bcm_ptp_mutex_take(_bcm_ptp_mutex_t m, int usec)
{
    int rv = -1;
    sal_usecs_t wait_time = sal_time_usecs() + usec;

    while ((rv != 0) && (int32)(wait_time - sal_time_usecs()) > 0) {
        rv = sal_mutex_take(m, wait_time - sal_time_usecs());
    }

    return rv;
}

int
_bcm_ptp_mutex_give(_bcm_ptp_mutex_t m)
{
    return sal_mutex_give(m);
}


/* ============================= */
/* Semaphore functions           */
/* ============================= */

_bcm_ptp_sem_t
_bcm_ptp_sem_create(char *desc, int binary, int initial_count)
{
    return sal_sem_create(desc, binary, initial_count);
}

void
_bcm_ptp_sem_destroy(_bcm_ptp_sem_t b)
{
    sal_sem_destroy(b);
}

int
_bcm_ptp_sem_take(_bcm_ptp_sem_t b, int usec)
{
    int rv = -1;
    sal_usecs_t end_time = sal_time_usecs() + usec;
    int32 wait_time = usec;

    while ((rv != 0) && (wait_time > 0)) {
        rv = sal_sem_take(b, wait_time);
        wait_time = end_time - sal_time_usecs();
    }

    return rv;
}

int
_bcm_ptp_sem_give(_bcm_ptp_sem_t b)
{
    return sal_sem_give(b);
}

#endif /* INCLUDE_PTP */
