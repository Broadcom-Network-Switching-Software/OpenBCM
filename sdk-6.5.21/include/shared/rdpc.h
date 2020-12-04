/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RDPC routines
 */

#ifndef _SHR_RDPC_H
#define _SHR_RDPC_H

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/dpc.h>
#include <sal/core/sync.h>

/* Repeating Delayed Procedure Call:
 *   Wrapper on top of SAL DPC layer to provide a safe mechanism for
 *   scheduling / unscheduling repeated calls to a function.
 */

/* shr_rdpc_fn_t should return the time interval until the next call, or 0 to indicate it should stop */
typedef sal_usecs_t (*shr_rdpc_fn_t)(void **, void **, void **, void **);

typedef struct shr_rdpc_s {
    shr_rdpc_fn_t func;
    sal_mutex_t call_count_lock;
    int run_count;
    int running;
} shr_rdpc_t;

extern int shr_rdpc_callback_create(shr_rdpc_t *rdpc, shr_rdpc_fn_t func);
extern int shr_rdpc_callback_created(shr_rdpc_t *rdpc);
extern int shr_rdpc_callback_start(shr_rdpc_t *rdpc, sal_usecs_t first_interval, void *, void *, void *, void *);
extern int shr_rdpc_callback_stop(shr_rdpc_t *rdpc);
extern int shr_rdpc_callback_finished(shr_rdpc_t *rdpc);
extern int shr_rdpc_callback_destroy(shr_rdpc_t *rdpc);

#endif	/* !_SHR_RDPC_H */
