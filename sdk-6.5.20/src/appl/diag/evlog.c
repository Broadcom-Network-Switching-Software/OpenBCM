/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        evlog.c
 * Purpose:
 * Requires:    
 */

#include <shared/evlog.h>
#include <appl/diag/evlog.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/error.h>

#if defined(INCLUDE_SHARED_EVLOG)

#define EV_ALLOC(ev, targ, size) do {                                   \
        targ = sal_alloc(size, "evlog");                                \
        if ((targ) == NULL) {                                           \
            diag_evlog_destroy(ev);                                     \
            return NULL;                                                \
        }                                                               \
        sal_memset(targ, 0, size);                                      \
    } while (0)

shared_evlog_t *
diag_evlog_create(int max_args, int max_entries)
{
    shared_evlog_t *ev = NULL;

    EV_ALLOC(ev, ev, sizeof(*ev));
    EV_ALLOC(ev, ev->ev_strs, (sizeof(char *) * max_entries));
    EV_ALLOC(ev, ev->ev_args, (sizeof(int) * max_args * max_entries));
    EV_ALLOC(ev, ev->ev_ts, (sizeof(sal_usecs_t) * max_entries));
    EV_ALLOC(ev, ev->ev_tid, (sizeof(sal_thread_t) * max_entries));
    EV_ALLOC(ev, ev->ev_flags, (sizeof(uint32) * max_entries));
    ev->max_entries = max_entries;
    ev->max_args = max_args;

    return ev;
}
#undef EV_ALLOC

#define EV_FREE(targ) if ((targ) != NULL) sal_free(targ)

void
diag_evlog_destroy(shared_evlog_t *ev)
{
    if (ev != NULL) {
        EV_FREE(ev->ev_strs);
        EV_FREE(ev->ev_args);
        EV_FREE(ev->ev_ts);
        EV_FREE(ev->ev_tid);
        EV_FREE(ev->ev_flags);

        sal_free(ev);
    }
}
#undef EV_FREE


/* To do:  Implement a reasonable "dump" function */
int
diag_evlog_dump(shared_evlog_t *ev, int count, int oldest)
{
    /*
     * If count == 0, dump all entries in log
     * If count > 0, dump that many entries moving forward in time.
     * If count < 0, dump that many entries moving backward in time.
     *
     * If oldest is true, dump earliest entries.
     * Otherwise dump most recent entries.
     */

    return BCM_E_UNAVAIL;
}

#endif  /* INCLUDE_SHARED_EVLOG */
