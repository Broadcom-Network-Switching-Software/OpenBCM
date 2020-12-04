/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RDPC routines
 */

#include <shared/bsl.h>
#include <shared/rdpc.h>
#include <soc/error.h>

#define RDPC_MUTEX_TIMEOUT (100000)

static void shr_rdpc_dispatcher(void *owner, void* p0, void* p1, void* p2, void* p3)
{
    shr_rdpc_t *rdpc = (shr_rdpc_t *)owner;
    sal_usecs_t next_call = 0;

    if (rdpc->running) {
        next_call = rdpc->func(&p0, &p1, &p2, &p3);
    }

    if (sal_mutex_take(rdpc->call_count_lock, RDPC_MUTEX_TIMEOUT) != 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("RDPC dispatch failed to get mutex\n")));
    } else {
        if (next_call && rdpc->running && rdpc->run_count == 1) {
            int rv = sal_dpc_time(next_call, &shr_rdpc_dispatcher, (void*)rdpc, p0, p1, p2, p3);
            if (rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("RDPC scheduling of DPC failed\n")));
                rdpc->run_count--;
            }
        } else {
            if (rdpc->run_count <= 0) {
                /* run count was incremented every time this rdpc was started, and decremented
                   when it did not reschedule itself.  If we're here, it should be positive */
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("RDPC run count invalid\n")));
            } else {
                /* Either the RDPC was stopped, or it was started multiple times (possibly racing a stop) */
                /* in the latter case, let it drain so only one DPC is scheduled for it */
                rdpc->run_count--;
            }
        }
        sal_mutex_give(rdpc->call_count_lock);
    }
}

/*
 * Function:
 *      shr_rdpc_callback_create
 * Purpose:
 *      Creates a new RDPC callback in the provided structure.  RDPC will not be called until started.
 * Parameters:
 *      rdpc    - (IN/OUT) Pointer to RDPC structure to be initialized/created
 *      func    - (IN) The callback function
 * Returns:
 *      Error code or SOC_E_NONE
 * Notes:
 */
int shr_rdpc_callback_create(shr_rdpc_t *rdpc, shr_rdpc_fn_t func)
{
    rdpc->func = func;
    rdpc->call_count_lock = sal_mutex_create("rdpc");
    rdpc->run_count = 0;
    rdpc->running = 0;

    return rdpc->call_count_lock ? SOC_E_NONE : SOC_E_MEMORY;
}

/*
 * Function:
 *      shr_rdpc_callback_created
 * Purpose:
 *      Indicates whether a shr_rdpc_t structures with static/global allocation has been created
 * Parameters:
 *      rdpc    - (IN/OUT) Pointer to RDPC structure to be initialized/created
 * Returns:
 *      SOC_E_INIT (callback is null) or SOC_E_NONE (callback has been created)
 * Notes:
 */
int shr_rdpc_callback_created(shr_rdpc_t *rdpc)
{
    return (rdpc->call_count_lock) ? SOC_E_NONE : SOC_E_INIT;
}

/*
 * Function:
 *      shr_rdpc_callback_start
 * Purpose:
 *      Runs the callback after the specified interval.  The value returned from
 *      the callback will be used as the interval before the subsequent call.
 *      If callback is already scheduled to run, it will execute at the earlier time.
 * Parameters:
 *      rdpc           - (IN) Pointer to RDPC structure
 *      first_interval - time in usecs before the callback should be called
 *      p0...p3        - callback will get pointers to these void* parameters
 * Returns:
 *      Error code or SOC_E_NONE
 * Notes:
 */
int shr_rdpc_callback_start(shr_rdpc_t *rdpc, sal_usecs_t first_interval,
                                      void* p0, void* p1, void* p2, void* p3)
{
    int rv = SOC_E_NONE;

    if (sal_mutex_take(rdpc->call_count_lock, RDPC_MUTEX_TIMEOUT) != 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("RDPC dispatch failed to get mutex\n")));
        return SOC_E_INTERNAL;
    }

    rdpc->running = 1;
    rdpc->run_count++;
    rv = sal_dpc_time(first_interval, &shr_rdpc_dispatcher, (void*)rdpc, (void*)p0, (void*)p1, (void*)p2, (void*)p3);

    sal_mutex_give(rdpc->call_count_lock);

    return rv;
}

/*
 * Function:
 *      shr_rdpc_callback_stop
 * Purpose:
 *      Stops further calls of the callback.
 * Parameters:
 *      rdpc           - (IN) Pointer to RDPC structure
 * Returns:
 *      Error code or SOC_E_NONE
 * Notes:
 */
int shr_rdpc_callback_stop(shr_rdpc_t *rdpc)
{
    int rv = SOC_E_NONE;

    if (sal_mutex_take(rdpc->call_count_lock, RDPC_MUTEX_TIMEOUT) != 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("RDPC dispatch failed to get mutex\n")));
        return SOC_E_INTERNAL;
    }
    rdpc->running = 0;
    sal_mutex_give(rdpc->call_count_lock);
    return rv;
}

/*
 * Function:
 *      shr_rdpc_callback_finished
 * Purpose:
 *      Indicates whether the callback structure is still in use after having been stopped.
 * Parameters:
 *      rdpc           - (IN) Pointer to RDPC structure
 * Returns:
 *      SOC_E_BUSY : still in use.  The callback should not be destroyed yet.
 *      SOC_E_NONE : all activity has been completed, callback may be destroyed.
 * Notes:
 */
int shr_rdpc_callback_finished(shr_rdpc_t *rdpc)
{
    int rv = SOC_E_NONE;

    if (sal_mutex_take(rdpc->call_count_lock, RDPC_MUTEX_TIMEOUT) != 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("RDPC dispatch failed to get mutex\n")));
        return SOC_E_INTERNAL;
    }
    if (rdpc->run_count > 0) {
        rv = SOC_E_BUSY;
    }
    sal_mutex_give(rdpc->call_count_lock);
    return rv;
}
/*
 * Function:
 *      shr_rdpc_callback_destroy
 * Purpose:
 *      Releases resources associated with the RDPC structure
 * Parameters:
 *      rdpc           - (IN) Pointer to RDPC structure
 * Returns:
 *      SOC_E_BUSY : callback is still in use and was not destroyed
 *      SOC_E_NONE
 * Notes:
 */
int shr_rdpc_callback_destroy(shr_rdpc_t *rdpc)
{
    int rv = shr_rdpc_callback_finished(rdpc);
    if (rv == 0) {
        sal_mutex_destroy(rdpc->call_count_lock);
    }

    return rv;
}
