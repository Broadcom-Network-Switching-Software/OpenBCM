/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        l2notify.c
 * Purpose:     L2 table change notification, async task
 * Requires:
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/l2.h>

#include "bcmx_int.h"

/****************************************************************
 *
 * BCMX L2 address change notification
 *
 *    Works with the BCM dispatch layer.
 *
 *    Maintains a list of callbacks locally.  Global indication of
 *    whether BCMX notify is active is used to determine if
 *    bcm_l2_addr_register is called when new devices are attached.
 *
 *    Semantics for callbacks are the same, except that a bcmx_l2_addr_t
 *    is used.
 *
 *    L2 notify configuration is locked during callbacks, so
 *    application MUST not make register/unregister calls from
 *    within callbacks.
 *
 *    Assumes the lower layers are initialized elsewhere.
 */

#define BCMX_L2_NOTIFY_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_L2_NOTIFY_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

typedef struct l2n_handler_s {
    struct l2n_handler_s        *next;
    bcmx_l2_notify_f             lh_f;
    void                        *userdata;
} l2n_handler_t;

static sal_mutex_t _l2n_lock;
static l2n_handler_t *handler_list;

#define CHECK_INIT    do {                                             \
    BCMX_READY_CHECK;                                                  \
    if (_l2n_lock == NULL &&                                           \
        ((_l2n_lock = sal_mutex_create("bcmx_l2_notify")) == NULL))    \
        return BCM_E_MEMORY;                                           \
    } while (0)

#define L2N_LOCK sal_mutex_take(_l2n_lock, sal_mutex_FOREVER)
#define L2N_UNLOCK sal_mutex_give(_l2n_lock)

/* The callback registered with the BCM layer */

STATIC void
_bcm_l2_addr_cb(int unit, bcm_l2_addr_t *l2addr, int insert, void *userdata)
{
    bcmx_l2_addr_t bcmx_l2;
    l2n_handler_t *handler;

    COMPILER_REFERENCE(userdata);

    if (handler_list == NULL || _l2n_lock == NULL) {
        return;
    }
    sal_memset(&bcmx_l2, 0, sizeof(bcmx_l2));
    bcmx_l2_addr_from_bcm(&bcmx_l2, NULL, l2addr);
    bcmx_l2.bcm_unit = unit;

    L2N_LOCK;
    handler = handler_list;
    while (handler != NULL) {
        (handler->lh_f)(&bcmx_l2, insert, handler->userdata);
        handler = handler->next;
    }
    L2N_UNLOCK;
}


/* Search for a handler in our list; assumes config lock held */

static INLINE l2n_handler_t *
l2n_handler_find(bcmx_l2_notify_f cb, void *userdata, l2n_handler_t **prev)
{
    l2n_handler_t *handler = handler_list;

    if (prev != NULL) {
        *prev = NULL;
    }
    while (handler != NULL) {
        if ((handler->lh_f == cb) && (userdata == handler->userdata)) {
            return handler;
        }
        if (prev != NULL) {
            *prev = handler;
        }
        handler = handler->next;
    }

    return NULL;
}

/* Force registration to rlink layer */
STATIC int
_bcmx_l2_rlink_register(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int bcm_unit, i;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_register(bcm_unit, _bcm_l2_addr_cb, NULL);
        BCM_IF_ERROR_RETURN
            (BCMX_L2_NOTIFY_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_notify_register
 */

int
bcmx_l2_notify_register(bcmx_l2_notify_f callback, void *userdata)
{
    l2n_handler_t *handler;
    int rv = BCM_E_NONE;

    CHECK_INIT;

    L2N_LOCK;

    handler = l2n_handler_find(callback, userdata, NULL);
    if (handler == NULL) {  /* Not there */
        handler = sal_alloc(sizeof(l2n_handler_t), "bcmx_l2_addr_reg");
        if (handler == NULL) {
            rv = BCM_E_MEMORY;
        } else {
            handler->lh_f = callback;
            handler->userdata = userdata;

            handler->next = handler_list;
            handler_list = handler;
        }
    }

    L2N_UNLOCK;

    if (handler != NULL) {  /* Always reregister w/ rlink */
        (void)_bcmx_l2_rlink_register();
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_notify_unregister
 */

int
bcmx_l2_notify_unregister(bcmx_l2_notify_f callback, void * userdata)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    l2n_handler_t *prev, *handler;
    int unreg = FALSE;  /* Should we unregister? (No callbacks registered). */

    CHECK_INIT;

    L2N_LOCK;

    handler = l2n_handler_find(callback, userdata, &prev);
    if (handler == NULL) {  /* Wasn't there */
        L2N_UNLOCK;
        return BCM_E_NONE;
    }

    if (prev != NULL) {
        prev->next = handler->next;
    } else { /* First thing in list */
        handler_list = handler->next;
    }
    sal_free(handler);

    if (handler_list == NULL) {  /* Nothing registered */
        unreg = TRUE;
    }

    L2N_UNLOCK;

    if (unreg) {
        BCMX_UNIT_ITER(bcm_unit, i) {
            tmp_rv = bcm_l2_addr_unregister(bcm_unit,
                                            _bcm_l2_addr_cb,
                                            NULL);
        BCM_IF_ERROR_RETURN
            (BCMX_L2_NOTIFY_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_notify_start
 * Purpose:
 *      Start L2 notify
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Just registers with BCM layer to get messages.
 */

int
bcmx_l2_notify_start(void)
{
    int rv;

    BCMX_READY_CHECK;

    rv = _bcmx_l2_rlink_register();

    return rv;
}


/*
 * Function:
 *      bcmx_l2_notify_stop
 * Purpose:
 *      Stop L2 notification at BCMX layer
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Unregisters with BCM layer for all units; Does nothing to
 *      lower layers.
 */

int
bcmx_l2_notify_stop(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_READY_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_unregister(bcm_unit, _bcm_l2_addr_cb, NULL);
        BCM_IF_ERROR_RETURN
            (BCMX_L2_NOTIFY_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_device_add
 * Purpose:
 *      Add a device to the L2 notification list
 * Parameters:
 *      bcm_unit     - Unit being added
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Only matters if currently running.  No local state, just
 *      register BCMX callback with BCM layer.
 */

int
bcmx_l2_device_add(int bcm_unit)
{
    int rv = BCM_E_NONE;
    int reg = FALSE;

    CHECK_INIT;

    /* If any registered callbacks, make sure registered on this device */
    L2N_LOCK; /* Make sure the handler_list is not being updated */
    if (handler_list != NULL) {
        reg = TRUE;
    }
    L2N_UNLOCK;

    if (reg) {
        rv = bcm_l2_addr_register(bcm_unit, _bcm_l2_addr_cb, NULL);
    }

    return rv;
}

int
bcmx_l2_device_remove(int bcm_unit)
{
    BCMX_READY_CHECK;

    return bcm_l2_addr_unregister(bcm_unit, _bcm_l2_addr_cb, NULL);
}
