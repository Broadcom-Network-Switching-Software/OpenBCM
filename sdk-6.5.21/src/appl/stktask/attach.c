/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Attach Application
 *
 * A callback suitable for use in the stack task database update
 * callback is provided.  It automatically attaches and detaches
 * units through the BCM RPC attachments.  A callback mechanism
 * is provided to notify users that a unit has been added or
 * removed.
 */

#include <shared/bsl.h>

#include <sdk_config.h>
#include <assert.h>
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/init.h>

#include <bcm_int/control.h>
#ifdef BCM_RPC_SUPPORT
#include <bcm_int/rpc/rpc.h>
#endif /* BCM_RPC_SUPPORT */

#include <appl/stktask/attach.h>
#include <appl/stktask/stktask.h>

#ifndef ATTACH_THREAD_STACK
#define ATTACH_THREAD_STACK  SAL_THREAD_STKSZ
#endif

#ifndef ATTACH_THREAD_PRIO
#define ATTACH_THREAD_PRIO   50
#endif


static volatile int _bcm_stack_attach_thread_active;

typedef struct _attach_cb_s {
    struct _attach_cb_s      *next;
    bcm_stack_attach_cb_f    callback;
} _attach_cb_t;

static _attach_cb_t *_attach_cb;

static sal_mutex_t at_lock;
#define AT_LOCK sal_mutex_take(at_lock, sal_mutex_FOREVER)
#define AT_UNLOCK sal_mutex_give(at_lock)

/*
 * Add a function to the callback list
 */
int
bcm_stack_attach_register(bcm_stack_attach_cb_f callback)
{
    _attach_cb_t  *acb;

    if (callback == NULL) {
        return BCM_E_FAIL;
    }
    acb = sal_alloc(sizeof(*acb), "attach cb");
    if (acb == NULL) {
        return BCM_E_MEMORY;
    }
    acb->callback = callback;
    acb->next = _attach_cb;
    _attach_cb = acb;
    return BCM_E_NONE;
}

/*
 * Remove a function from the callback list
 */
int
bcm_stack_attach_unregister(bcm_stack_attach_cb_f callback)
{
    _attach_cb_t  *acb, *pacb;

    pacb = NULL;
    for (acb = _attach_cb; acb; pacb = acb, acb = acb->next) {
        if (acb->callback == callback) {
            if (pacb == NULL) {
                _attach_cb = acb->next;
            } else {
                pacb->next = acb->next;
            }
            sal_free(acb);
            return BCM_E_NONE;
        }
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Run the callback list
 */
STATIC void
_bcm_stack_attach_cb_run(int unit, int attached,
                         cpudb_entry_t *cpuent, int cpuunit)
{
    _attach_cb_t  *acb;

    for (acb = _attach_cb; acb; acb = acb->next) {
        (*acb->callback)(unit, attached, cpuent, cpuunit);
    }
#ifdef BCM_RPC_SUPPORT
    /* notify rpc to fail any queued requests */
    if (!attached) {
        bcm_rpc_detach(unit);
    }
#endif /* BCM_RPC_SUPPORT */
}

/*
 * Detach bcm units to match the cpu database.
 */
int
bcm_stack_detach(cpudb_ref_t db_ref)
{
    int            ismaster;
    cpudb_entry_t  *nent;
    cpudb_entry_t  oent;
    int            unit, dunit, rv;
    cpudb_key_t    *key;

    AT_LOCK;
    ismaster = db_ref->local_entry == db_ref->master_entry;

    /* find any dispatch entries that need to be deleted */
    sal_memset(&oent, 0, sizeof(oent));

    for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
        if (BCM_CONTROL(unit) == NULL) {
            continue;
        }
        if (BCM_IS_REMOTE(unit)) {
            key = (cpudb_key_t *)BCM_CONTROL(unit)->drv_control;
            CPUDB_KEY_SEARCH(db_ref, *key, nent);
            if (nent != NULL && ismaster) {
                continue;
            }
            dunit = BCM_CONTROL(unit)->unit;
            sal_memcpy(&oent.base.key, key, sizeof(cpudb_key_t));
            oent.flags = 0;
            rv = bcm_detach(unit);
        } else if (BCM_IS_LOCAL(unit)) {
            if (ismaster) {
                continue;
            }
            if (!(BCM_CAPABILITY(unit) & BCM_CAPA_CALLBACK)) {
                continue;
            }
            dunit = unit;
            rv = 0;
            BCM_CAPABILITY(unit) &= ~BCM_CAPA_CALLBACK;
            sal_memset(&oent.base.key, 0, sizeof(cpudb_key_t));
            oent.flags = CPUDB_F_IS_LOCAL;
        } else {
            continue;
        }
        if (rv >= 0) {
            _bcm_stack_attach_cb_run(unit, 0, &oent, dunit);
        }
    }
    AT_UNLOCK;

    return BCM_E_NONE;
}

/*
 * Attach bcm units to match the cpu database.
 */
int
bcm_stack_attach(cpudb_ref_t db_ref)
{
    int            ismaster;
    cpudb_entry_t  *nent;
    int            dunit, rv;
    char           keybuf[CPUDB_KEY_STRING_LEN];
    char           *dtype, *dsubtype;
    int            terminate;

    AT_LOCK;
    ismaster = db_ref->local_entry == db_ref->master_entry;

    /* add any new cpu units */
    if (ismaster) {
        terminate = 0;

        /* attach local units first */
        CPUDB_FOREACH_ENTRY(db_ref, nent) {
            if (terminate) {
                break;
            }
            if (!(nent->flags & CPUDB_F_IS_LOCAL)) {
                continue;
            }
            dtype = NULL;
            dsubtype = NULL;
            rv = 0;
            for (dunit = 0; dunit < nent->base.num_units; dunit++) {
                rv = bcm_find(dtype, dsubtype, dunit);
                if (rv < 0) {
                    rv = bcm_attach(-1, dtype, dsubtype, dunit);
                }
                if (rv < 0) {
                    terminate = 1;
                    break;
                }
                if (rv >= 0 && !(BCM_CAPABILITY(rv) & BCM_CAPA_CALLBACK)) {
                    _bcm_stack_attach_cb_run(rv, 1, nent, dunit);
                    BCM_CAPABILITY(rv) |= BCM_CAPA_CALLBACK;
                }
            }
        }

        /* attach remote units */
        CPUDB_FOREACH_ENTRY(db_ref, nent) {
            if (terminate) {
                break;
            }
            if (nent->flags & CPUDB_F_IS_LOCAL) {
                continue;
            }
            cpudb_key_format(nent->base.key, keybuf, sizeof(keybuf));
            dtype = "client";
            dsubtype = keybuf;
            rv = 0;
            for (dunit = 0; dunit < nent->base.num_units; dunit++) {
                rv = bcm_find(dtype, dsubtype, dunit);
                if (rv < 0) {
                    rv = bcm_attach(-1, dtype, dsubtype, dunit);
                }
                if (rv < 0) {
                    terminate = 1;
                    break;
                }
                if (rv >= 0 && !(BCM_CAPABILITY(rv) & BCM_CAPA_CALLBACK)) {
                    _bcm_stack_attach_cb_run(rv, 1, nent, dunit);
                    BCM_CAPABILITY(rv) |= BCM_CAPA_CALLBACK;
                }
            }
        }
    }
    AT_UNLOCK;

    return BCM_E_NONE;
}


/*
 * Detach and attach bcm units to match the cpu database.
 */

#ifdef BROADCOM_DEBUG

/* Variables and callback for diagnostics */
static int dt_unit, at_unit, at_cpu;
static cpudb_entry_t *at_cpuent;

static void
_bcm_stack_at_dt_update(int unit,
                        int attach,
                        cpudb_entry_t *cpuent,
                        int cpuunit)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(cpuunit);
    if (attach) {
        at_unit++;
        /* count number of unique CPUs */
        if (cpuent != at_cpuent) {
            at_cpu++;
            at_cpuent = cpuent;
        }
    } else {
        dt_unit++;
    }
}

/* Debug version */

STATIC void
_bcm_stack_detach_attach(cpudb_ref_t db_ref)
{
    int rv;
    char           keybuf[CPUDB_KEY_STRING_LEN];
    int            dbg_unit;
    cpudb_entry_t  *nent;

    AT_LOCK;

    dbg_unit = 0;
    CPUDB_FOREACH_ENTRY(db_ref, nent) {
        dbg_unit += nent->base.num_units;
    }

    rv = bcm_stack_attach_register(_bcm_stack_at_dt_update);
    if (BCM_SUCCESS(rv)) {
        dt_unit = at_unit = at_cpu = 0;
        at_cpuent = NULL;
        bcm_stack_detach(db_ref);
        bcm_stack_attach(db_ref);
        bcm_stack_attach_unregister(_bcm_stack_at_dt_update);
    }
    AT_UNLOCK;

    bcm_st_event_send(BCM_SUCCESS(rv) ?
                      BCM_STE_ATTACH_SUCCESS : BCM_STE_ATTACH_FAILURE);
    
    cpudb_key_format(db_ref->local_entry->base.key, keybuf, sizeof(keybuf));
    if (db_ref->local_entry == db_ref->master_entry) {
        LOG_INFO(BSL_LS_TKS_COMMON,
                 (BSL_META("STACK: master on %s (%d cpu%s, %d unit%s)\n"),
                  keybuf,
                  db_ref->num_cpus, db_ref->num_cpus == 1 ? "" : "s",
                  dbg_unit, dbg_unit == 1 ? "" : "s"));
    } else {
        char  keymast[CPUDB_KEY_STRING_LEN];

        cpudb_key_format(db_ref->master_entry->base.key,
                         keymast, sizeof(keymast));
        LOG_INFO(BSL_LS_TKS_COMMON,
                 (BSL_META("STACK: slave on %s (%d cpus, master %s)\n"),
                  keybuf, db_ref->num_cpus, keymast));
    }

    if (dt_unit > 0) {
        LOG_INFO(BSL_LS_TKS_COMMON,
                 (BSL_META("STACK: detach %d unit%s\n"),
                  dt_unit, dt_unit == 1 ? "" : "s"));
    }

    if (at_cpu > 0) {
        LOG_INFO(BSL_LS_TKS_COMMON,
                 (BSL_META("STACK: attach %d unit%s on %d cpu%s\n"),
                  at_unit, at_unit == 1 ? "" : "s",
                  at_cpu, at_cpu == 1 ? "" : "s"));
    }
}

#else

/* Non-debug version */

STATIC void
_bcm_stack_detach_attach(cpudb_ref_t db_ref)
{
    AT_LOCK;
    bcm_stack_detach(db_ref);
    bcm_stack_attach(db_ref);
    AT_UNLOCK;

    bcm_st_event_send(BCM_STE_ATTACH_SUCCESS);
}

#endif /* BROADCOM_DEBUG */

/*
 *   asynchronous _bcm_stack_attach()
 */

STATIC void
_bcm_stack_attach_thread(void *cookie)
{
    cpudb_ref_t db_ref = (cpudb_ref_t)cookie;

    _bcm_stack_attach_thread_active++;

    _bcm_stack_detach_attach(db_ref);

    _bcm_stack_attach_thread_active--;

    sal_thread_exit(0);

}

/*
 * The actual callout routine that the stack task calls.
 *
 * The stack attach update call takes place in the ATP RX thread
 * context.  Any attach callback might require the use of packet
 * services that would need to use the RX thread (BCM RPC calls for
 * example using ATP), so those callbacks are done in a new thread
 * context.
 */
int
bcm_stack_attach_update(cpudb_ref_t db_ref)
{
    if (at_lock == NULL) {
        return BCM_E_INIT;
    }

    if (_attach_cb != NULL) {

        (void) sal_thread_create("bcmATTACH",
                                 ATTACH_THREAD_STACK,
                                 ATTACH_THREAD_PRIO,
                                 _bcm_stack_attach_thread,
                                 (void *)db_ref);
    } else {
        _bcm_stack_detach_attach((void *)db_ref);
    }
    return BCM_E_NONE;
}

/*
 * Return TRUE if the attach process is running
 */
int
bcm_stack_attach_running(void)
{
    if ((_bcm_stack_attach_thread_active < 0) ||
        (_bcm_stack_attach_thread_active >= 2)) {
        assert(0);
    }
    return _bcm_stack_attach_thread_active != 0;
}

/*
 * Initialize resources needed by the attach process.
 */
int
bcm_stack_attach_init(void)
{
    if (at_lock == NULL) {
        at_lock = sal_mutex_create("bcm_attach");
        if (at_lock == NULL) {
            return BCM_E_MEMORY;
        }
    }
    return BCM_E_NONE;
}

