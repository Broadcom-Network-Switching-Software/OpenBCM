/*! \file bcma_bcmpkt_rx.c
 *
 * Packet I/O Rx dispatcher.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_sleep.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_socket.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcma/bcmpkt/bcma_bcmpkt_rx.h>

#define BSL_LOG_MODULE BSL_LS_APPL_RX

/* Mutex lock for updating callbacks. */
static sal_mutex_t rx_cb_mutex[BCMDRD_CONFIG_MAX_UNITS];

/* The linked list head of Rx dispatchers. */
static bcma_bcmpkt_dispatch_info_t *dispatch_info[BCMDRD_CONFIG_MAX_UNITS];

/* The linked list head of global Rx callbacks. */
static bcma_bcmpkt_rx_cb_info_t *glb_rx_cb[BCMDRD_CONFIG_MAX_UNITS];

static bcma_bcmpkt_rx_cb_info_t *
bcma_bcmpkt_rx_handler_find(bcma_bcmpkt_rx_cb_info_t *cblist, uint8_t priority,
                            bcma_bcmpkt_rx_cb_f cb_func)
{
    while (cblist != NULL) {
        if (cblist->priority > priority) {
            return NULL;
        }
        if (cblist->cb_func == cb_func && cblist->priority == priority) {
            return cblist;
        }
        cblist = cblist->next;
    }
    return NULL;
}

static void
bcma_bcmpkt_rx_handler_destroy_all(bcma_bcmpkt_rx_cb_info_t *cblist)
{
    bcma_bcmpkt_rx_cb_info_t *pos;

    while ((pos = cblist) != NULL) {
        cblist = cblist->next;
        SHR_FREE(pos);
    }
}

static bcma_bcmpkt_dispatch_info_t *
bcma_bcmpkt_rx_dispatcher_find(int unit, int netif_id)
{
    bcma_bcmpkt_dispatch_info_t *pos = NULL;

    for (pos = dispatch_info[unit]; pos != NULL; pos = pos->next) {
        if (pos->netif_id == netif_id) {
            return pos;
        }
    }
    return NULL;
}

static int
bcma_bcmpkt_rx_handler_state_check(int unit, int netif_id, uint8_t priority,
                                   bcma_bcmpkt_rx_cb_f cb_func)
{
    bcma_bcmpkt_dispatch_info_t *dpdata = NULL;

    SHR_FUNC_ENTER(unit);

    if (netif_id == BCMA_BCMPKT_NETIF_ALL) {
        /* Check if the same global callback was registered. */
        if (bcma_bcmpkt_rx_handler_find(glb_rx_cb[unit], priority, cb_func)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Global callback exists.\n")));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        /* Check if the global callback  was registered to netif's dispatcher. */
        for (dpdata = dispatch_info[unit]; dpdata != NULL;
             dpdata = dpdata->next) {
            if (bcma_bcmpkt_rx_handler_find(dpdata->cb_info, priority,
                                            cb_func)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Callback exists on network interface (%d).\n"),
                           dpdata->netif_id));
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
        }
    } else {
        dpdata = bcma_bcmpkt_rx_dispatcher_find(unit, netif_id);
        if (dpdata == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Rx dispatcher not created on network interface (%d).\n"),
                       netif_id));
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }

        if (bcma_bcmpkt_rx_handler_find(dpdata->cb_info, priority, cb_func)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Callback exists on network interface (%d).\n"),
                       dpdata->netif_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bcma_bcmpkt_rx_handler_cnt_update(int unit, int netif_id, bool decrease)
{
    bcma_bcmpkt_dispatch_info_t *dpdata = NULL;

    if (netif_id == BCMA_BCMPKT_NETIF_ALL) {
        for (dpdata = dispatch_info[unit]; dpdata != NULL;
             dpdata = dpdata->next) {
            if (decrease) {
                dpdata->glb_hndlr_cnt--;
            } else {
               dpdata->glb_hndlr_cnt++;
            }
        }
    } else {
        dpdata = bcma_bcmpkt_rx_dispatcher_find(unit, netif_id);
        if (dpdata != NULL) {
            if (decrease) {
                dpdata->hndlr_cnt--;
            } else {
               dpdata->hndlr_cnt++;
            }
        }
    }

    return;
}

bool
bcma_bcmpkt_rx_netif_valid(int unit, int netif_id)
{
    int rv = SHR_E_NONE;
    int max_num = BCMA_BCMPKT_DEFAULT_NETIF_MAX;
    bcmpkt_dev_drv_types_t dev_drv_type;

    rv = bcmpkt_dev_drv_type_get(unit, &dev_drv_type);

    if (SHR_SUCCESS(rv) && dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        (void)bcmpkt_netif_max_get(unit, &max_num);
    }

    if (!(netif_id == BCMA_BCMPKT_NETIF_ALL || \
          (netif_id > BCMA_BCMPKT_NETIF_RESERVED && \
           netif_id < max_num))) {
       return FALSE;
    }

    return TRUE;
}

int
bcma_bcmpkt_rx_register(int unit, int netif_id, uint32_t flags,
                        uint8_t priority, const char *name,
                        bcma_bcmpkt_rx_cb_f cb_func, void *cb_data)
{
    int locked = FALSE;
    bcma_bcmpkt_dispatch_info_t *dpdata = NULL;
    bcma_bcmpkt_rx_cb_info_t **indirect;
    bcma_bcmpkt_rx_cb_info_t *cbdata = NULL;
    bcma_bcmpkt_rx_cb_info_t cb_info = {
        .next = NULL,
        .flags = flags,
        .priority = priority,
        .name = name,
        .cb_func = cb_func,
        .cb_data = cb_data,
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!bcma_bcmpkt_rx_netif_valid(unit, netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (dispatch_info[unit] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "No Rx dispatcher created.\n")));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT(sal_mutex_take(rx_cb_mutex[unit], SAL_MUTEX_FOREVER));
    locked = TRUE;

    /* Check the same callback was registered. */
    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_rx_handler_state_check(unit, netif_id, priority, cb_func));

    SHR_ALLOC(cbdata, sizeof(bcma_bcmpkt_rx_cb_info_t), "bcmaBcmpktRxCbInfo");
    SHR_NULL_CHECK(cbdata, SHR_E_MEMORY);
    sal_memcpy(cbdata, &cb_info, sizeof(cb_info));

    if (netif_id == BCMA_BCMPKT_NETIF_ALL) {
        indirect = &glb_rx_cb[unit];
    } else {
        dpdata = bcma_bcmpkt_rx_dispatcher_find(unit, netif_id);
        if (dpdata == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        indirect = &dpdata->cb_info;
    }

    /* Find the position before which to insert the callback. Callbacks of
     *  the same priority will be placed in the order they are registered.
     */
    while ((*indirect) != NULL) {
        if ((*indirect)->priority > cbdata->priority) {
            break;
        }
        indirect = &(*indirect)->next;
    }
    cbdata->next = *indirect;
    *indirect = cbdata;

    /* Update dispatcher's Rx callback count*/
    bcma_bcmpkt_rx_handler_cnt_update(unit, netif_id, FALSE);

exit:
    if (locked) {
        sal_mutex_give(rx_cb_mutex[unit]);
    }
    if (SHR_FUNC_ERR()) {
        SHR_FREE(cbdata);
    }
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_rx_unregister(int unit, int netif_id, uint8_t priority,
                          bcma_bcmpkt_rx_cb_f cb_func)
{
    int locked = FALSE;
    bcma_bcmpkt_dispatch_info_t *dpdata = NULL;
    bcma_bcmpkt_rx_cb_info_t *cbdata = NULL;
    bcma_bcmpkt_rx_cb_info_t **indirect;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!bcma_bcmpkt_rx_netif_valid(unit, netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (dispatch_info[unit] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "No Rx dispatcher created.\n")));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT(sal_mutex_take(rx_cb_mutex[unit], SAL_MUTEX_FOREVER));
    locked = TRUE;

    if (netif_id == BCMA_BCMPKT_NETIF_ALL) {
        indirect = &glb_rx_cb[unit];
    } else {
        dpdata = bcma_bcmpkt_rx_dispatcher_find(unit, netif_id);
        if (dpdata == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Rx dispatcher not created on network interface (%d).\n"),
                       netif_id));
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        indirect = &dpdata->cb_info;
    }

    while ((*indirect) != NULL) {
        if ((*indirect)->cb_func == cb_func &&
            (*indirect)->priority == priority) {
            break;
        }
        indirect = &(*indirect)->next;
    }

    if ((*indirect) == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Callback not registered on network interface (%d).\n"),
                   netif_id));
        SHR_EXIT();
    }

    (*indirect)->cb_pending = TRUE;
    sal_usleep(100000);
    cbdata = *indirect;
    *indirect = (*indirect)->next;
    SHR_FREE(cbdata);

    /* Update dispatcher's Rx callback count*/
    bcma_bcmpkt_rx_handler_cnt_update(unit, netif_id, TRUE);

exit:
    if (locked) {
        sal_mutex_give(rx_cb_mutex[unit]);
    }

    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_packet_process(int unit, int netif_id, bcmpkt_packet_t *packet,
                           void *cookie)
{
    int handled = FALSE;
    bcma_bcmpkt_rx_result_t rv;
    bcma_bcmpkt_dispatch_info_t *dispatch = NULL;
    bcma_bcmpkt_rx_cb_info_t *cblist, *glb_cblist, **curr;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cookie, SHR_E_PARAM);

    dispatch = (bcma_bcmpkt_dispatch_info_t *)cookie;
    dispatch->rx_pkts++;
    dispatch->rx_bytes += BCMPKT_PACKET_LEN(packet);

    cblist = dispatch->cb_info;
    glb_cblist = glb_rx_cb[unit];
    while (cblist != NULL && glb_cblist != NULL) {
        if (cblist->priority < glb_cblist->priority) {
            curr = &cblist;
        } else {
            curr = &glb_cblist;
        }
        if ((*curr)->cb_func && !(*curr)->cb_pending) {
            rv = (*curr)->cb_func(unit, netif_id, packet, (*curr)->cb_data);
            if (rv == BCMA_BCMPKT_RX_HANDLED) {
                handled = TRUE;
                dispatch->pkts_handled++;
                SHR_EXIT();
            }
            if (rv != BCMA_BCMPKT_RX_NOT_HANDLED) {
                /* One packet may be handled by mutiple bad callbacks. */
                dispatch->bad_hndlr_pkts++;
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Callback (%s) returned invalid value(%d).\n"),
                          (*curr)->name, rv));
            }
        }
        *curr = (*curr)->next;
    }

    if (cblist != NULL) {
        curr = &cblist;
    } else {
        curr = &glb_cblist;
    }

    while (*curr != NULL) {
        if ((*curr)->cb_func && !(*curr)->cb_pending) {
            rv = (*curr)->cb_func(unit, netif_id, packet, (*curr)->cb_data);
            if (rv == BCMA_BCMPKT_RX_HANDLED) {
                handled = TRUE;
                dispatch->pkts_handled++;
                SHR_EXIT();
            }
            if (rv != BCMA_BCMPKT_RX_NOT_HANDLED) {
                /* One packet may be handled by mutiple bad callbacks. */
                dispatch->bad_hndlr_pkts++;
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Callback (%s) returned invalid value(%d).\n"),
                          (*curr)->name, rv));
            }
        }
        *curr = (*curr)->next;
    }

    if (!handled) {
        /* Bad handled packet also be treated as NOT handled. */
        dispatch->non_hndlr_pkts++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_rx_dispatcher_create(int unit, int netif_id, uint32_t flags,
                                 bcmpkt_rx_cb_f dispatch_func)
{
    bcma_bcmpkt_dispatch_info_t *dpdata = NULL;
    bcma_bcmpkt_dispatch_info_t **indirect;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Check if the netif's dispatcher exists. */
    if (bcma_bcmpkt_rx_dispatcher_find(unit, netif_id)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Rx dispatcher exists on network interface (%d).\n"),
                   netif_id));
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    if (rx_cb_mutex[unit] == NULL) {
        rx_cb_mutex[unit] = sal_mutex_create("bcmaBcmpktRxCb");
        SHR_NULL_CHECK(rx_cb_mutex[unit], SHR_E_MEMORY);
    }

    SHR_ALLOC(dpdata, sizeof(bcma_bcmpkt_dispatch_info_t),
              "bcmaBcmpktDispatch");
    SHR_NULL_CHECK(dpdata, SHR_E_MEMORY);
    sal_memset(dpdata, 0, sizeof(bcma_bcmpkt_dispatch_info_t));
    dpdata->netif_id = netif_id;
    dpdata->dispatch_func = dispatch_func;

    SHR_IF_ERR_EXIT
        (bcmpkt_rx_register(unit, netif_id, 0, dispatch_func, dpdata));

    /* Insert at the tail of the linked list. */
    indirect = &dispatch_info[unit];
    while (*indirect != NULL) {
        indirect = &(*indirect)->next;
    }
    *indirect = dpdata;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Create Rx dispatcher on network interface (%d) succeed.\n"),
                 netif_id));
exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(dpdata);
        if (rx_cb_mutex[unit] != NULL && dispatch_info[unit] == NULL) {
            sal_mutex_destroy(rx_cb_mutex[unit]);
            rx_cb_mutex[unit] = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_rx_dispatcher_destroy(int unit, int netif_id)
{
    bcma_bcmpkt_dispatch_info_t *dpdata = NULL;
    bcma_bcmpkt_dispatch_info_t **indirect;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Find the netif's dispatcher. */
    indirect = &dispatch_info[unit];
    while ((*indirect) != NULL && (*indirect)->netif_id != netif_id) {
        indirect = &(*indirect)->next;
    }

    if (*indirect == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Rx dispatcher does not exist on network interface (%d).\n"),
                  netif_id));
        SHR_EXIT();
    }

    SHR_IF_ERR_CONT
        (bcmpkt_rx_unregister(unit, netif_id, (*indirect)->dispatch_func, NULL));

    /* Destroy all callbacks from  the netif's dispatcher. */
    bcma_bcmpkt_rx_handler_destroy_all((*indirect)->cb_info);

    /* Delete the netif's dispatcher. */
    dpdata = *indirect;
    *indirect = (*indirect)->next;
    SHR_FREE(dpdata);

    /* If no dispatcher exists, destroy the mutex and delete all global callbacks. */
    if (dispatch_info[unit] == NULL) {
        if (rx_cb_mutex[unit] != NULL) {
            sal_mutex_destroy(rx_cb_mutex[unit]);
            rx_cb_mutex[unit] = NULL;
        }

        bcma_bcmpkt_rx_handler_destroy_all(glb_rx_cb[unit]);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Destroy Rx dispatcher on network interface (%d) succeed.\n"),
                 netif_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_bcmpkt_rx_dispatcher_traverse(int unit,
                                   bcma_bcmpkt_dispatcher_traverse_cb_f cb_func,
                                   void *cb_data)
{
    bcma_bcmpkt_dispatch_info_t *curr, *next;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (dispatch_info[unit] == NULL) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "No Rx dispatcher entry.\n")));
        SHR_EXIT();
    }

    curr = dispatch_info[unit];
    while (curr != NULL) {
        next = curr->next;
        SHR_IF_ERR_EXIT(cb_func(unit, curr, cb_data));
        curr = next;
    }

exit:
    SHR_FUNC_EXIT();
}

