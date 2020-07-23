/*! \file bcmlm_ctrl.c
 *
 * Link controller.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>
#include <shr/shr_pb_format.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_thread.h>
#include <sal/sal_mutex.h>
#include <sal/sal_time.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmlm/bcmlm_drv_internal.h>
#include "bcmlm_internal.h"

/******************************************************************************
* Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLM_CTRL

/* link control database */
static bcmlm_ctrl_t *bcmlm_ctrl[BCMLM_DEV_NUM_MAX];

#define LC_LOCK(unit) \
    sal_mutex_take(bcmlm_ctrl[unit]->lock, SAL_MUTEX_FOREVER)

#define LC_UNLOCK(unit) \
    sal_mutex_give(bcmlm_ctrl[unit]->lock)

/******************************************************************************
* Private functions
 */
static void
bcmlm_ctrl_debug_dump(int unit)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    shr_pb_t *pb = shr_pb_create();

    shr_pb_printf(pb, "BCMLM debug dump.\n");
    shr_pb_format_uint32(pb, "pbm_linkscan: ",
                         lmctrl->pbm_linkscan.w,
                         COUNTOF(lmctrl->pbm_linkscan.w), 0);
    shr_pb_format_uint32(pb, "pbm_sw: ",
                         lmctrl->pbm_sw.w, COUNTOF(lmctrl->pbm_sw.w), 0);
    shr_pb_format_uint32(pb, " pbm_hw: ",
                         lmctrl->pbm_hw.w, COUNTOF(lmctrl->pbm_hw.w), 0);
    shr_pb_format_uint32(pb, " pbm_ovr: ",
                         lmctrl->pbm_ovr.w, COUNTOF(lmctrl->pbm_ovr.w), 0);
    shr_pb_format_uint32(pb, " pbm_ovr_link: ",
                         lmctrl->pbm_ovr_link.w,
                         COUNTOF(lmctrl->pbm_ovr_link.w), 0);
    shr_pb_format_uint32(pb, " pbm_newly_enabled: ",
                         lmctrl->pbm_newly_enabled.w,
                         COUNTOF(lmctrl->pbm_newly_enabled.w), 0);
    shr_pb_format_uint32(pb, " pbm_newly_disabled: ",
                         lmctrl->pbm_newly_disabled.w,
                         COUNTOF(lmctrl->pbm_newly_disabled.w), 0);
    shr_pb_format_uint32(pb, " pbm_suspend: ",
                         lmctrl->pbm_suspend.w,
                         COUNTOF(lmctrl->pbm_suspend.w), 0);
    shr_pb_format_uint32(pb, " pbm_valid: ",
                         lmctrl->pbm_valid.w,
                         COUNTOF(lmctrl->pbm_valid.w), 0);
    shr_pb_format_uint32(pb, " pbm_manual_sync: ",
                         lmctrl->pbm_manual_sync.w,
                         COUNTOF(lmctrl->pbm_manual_sync.w), 0);
    shr_pb_format_uint32(pb, " pbm_sync_change: ",
                         lmctrl->pbm_sync_change.w,
                         COUNTOF(lmctrl->pbm_sync_change.w), 0);

    shr_pb_printf(pb, " hw_conf_change %d ", lmctrl->hw_change);
    shr_pb_printf(pb, " hw_change %d ", lmctrl->hw_change);
    shr_pb_printf(pb, " ovr_change %d ", lmctrl->ovr_change);
    shr_pb_printf(pb, " force_change %d ", lmctrl->force_change);

    shr_pb_format_uint32(pb, " pbm_logical_link: ",
                         lmctrl->pbm_ha->pbm_logical_link.w,
                         COUNTOF(lmctrl->pbm_ha->pbm_logical_link.w), 0);
    shr_pb_format_uint32(pb, " pbm_fault: ",
                         lmctrl->pbm_ha->pbm_fault.w,
                         COUNTOF(lmctrl->pbm_ha->pbm_fault.w), 0);
    shr_pb_format_uint32(pb, " pbm_physical_link: ",
                         lmctrl->pbm_ha->pbm_physical_link.w,
                         COUNTOF(lmctrl->pbm_ha->pbm_physical_link.w), 0);
    shr_pb_format_uint32(pb, " pbm_force: ",
                         lmctrl->pbm_ha->pbm_force.w,
                         COUNTOF(lmctrl->pbm_ha->pbm_force.w), 0);
    shr_pb_format_uint32(pb, " pbm_force_link: ",
                         lmctrl->pbm_ha->pbm_force_link.w,
                         COUNTOF(lmctrl->pbm_ha->pbm_force_link.w), 0);
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit," %s\n"),shr_pb_str(pb)));

    shr_pb_destroy(pb);
}

static int
bcmlm_ctrl_scan_get(int unit, shr_port_t port, bool *scan)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    *scan = false;
    if (lmctrl->tc) {
        *scan = BCMDRD_PBMP_MEMBER(lmctrl->pbm_linkscan, port) ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Get physical link state
 * NOTE: This function should be invoked within LC_LOCK protection.
 */
static int
bcmlm_phylink_get(int unit, shr_port_t port, int *link)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    bool scan;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    *link = 0;
    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ha->pbm_force, port)) {
        /* Get the forced PHY link state from PC module. */
        SHR_IF_ERR_EXIT
            (bcmlm_sw_linkscan_link_get(unit, port, link));
    } else if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port)) {
        *link = BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr_link, port) ? 1 : 0;
    } else if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port)) {
        (void)bcmlm_ctrl_scan_get(unit, port, &scan);
        if (scan) {
            bcmdrd_pbmp_t hw_link;
            SHR_IF_ERR_EXIT
                (bcmlm_hw_linkscan_link_get(unit, &hw_link));
            *link = BCMDRD_PBMP_MEMBER(hw_link, port) ? 1 : 0;
        } else {
            /*
             * When scan disabled, HW linkscan context would be cleared.
             * Get the physical link from PC module.
             */
            SHR_IF_ERR_EXIT
                (bcmlm_sw_linkscan_link_get(unit, port, link));
        }
    } else if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_sw, port)) {
        SHR_IF_ERR_EXIT
            (bcmlm_sw_linkscan_link_get(unit, port, link));
    } else if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_valid, port) &&
               !BCMDRD_PBMP_MEMBER(lmctrl->pbm_suspend, port)) {
        SHR_IF_ERR_EXIT
            (bcmlm_sw_linkscan_link_get(unit, port, link));
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                  (BSL_META_UP(unit, port,
                               "Unresolved port to get phy link\n")));
        SHR_EXIT();
    }

exit:
    if (*link) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ha->pbm_physical_link, port);
    } else {
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ha->pbm_physical_link, port);
    }

    SHR_FUNC_EXIT();
}

static int
bcmlm_link_update(int unit, shr_port_t port)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    int cur_logical_link = 0, new_link = 0;
    int cur_fault = 0, new_fault = 0;
    int lc_locked = 0;
    int logical_link, fault_chk;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    LC_LOCK(unit);
    lc_locked = 1;

    if (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_valid, port)) {
        SHR_EXIT();
    }

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_suspend, port)) {
        SHR_EXIT();
    }

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_manual_sync, port)) {
        /* On manual-sync mode, only need to update the PHY state */
        SHR_IF_ERR_EXIT
            (bcmlm_phylink_get(unit, port, &new_link));
        SHR_EXIT();
    }

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ha->pbm_logical_link, port)) {
        cur_logical_link = 1;
    }
    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ha->pbm_fault, port)) {
        cur_fault = 1;
    }

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_sync_change, port)) {
        /*
         * When MANUAL_SYNC becomes 0 from 1, use the cached PHY link as the
         * source of new_link.
         */
        if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ha->pbm_physical_link, port)) {
            new_link = 1;
        }
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_sync_change, port);
    } else {
        SHR_IF_ERR_EXIT
            (bcmlm_phylink_get(unit, port, &new_link));
    }
    logical_link = new_link;

    if (SHR_FAILURE(bcmlm_linkscan_fault_check_enabled(unit, &fault_chk))) {
        fault_chk = 0;
    }
    if (new_link && fault_chk &&
        !BCMDRD_PBMP_MEMBER(lmctrl->pbm_ha->pbm_force, port) &&
        (BCMDRD_PBMP_MEMBER(lmctrl->pbm_sw, port) ||
         BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port))) {
        /* Get fault when scan is on and link is up. */
            SHR_IF_ERR_EXIT
                (bcmlm_linkscan_fault_get(unit, port, &new_fault));
            if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port)) {
                if (new_fault) {
                    /*
                     * Enable rescan for linked with fault port
                     * in hw linkscan mode.
                     */
                    lmctrl->hw_change = 1;
                }
            }
            logical_link = (new_link && !new_fault) ? 1 : 0;
    }

    if (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_newly_enabled, port) &&
        ((cur_logical_link && logical_link && (cur_fault == new_fault)) ||
         (!cur_logical_link && !logical_link))) {
        /* No change */
        SHR_EXIT();
    }

    if (!new_link) {
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ha->pbm_fault, port);
    }

    if ((cur_logical_link != new_link) || (cur_fault != new_fault)) {
        SHR_IF_ERR_EXIT
            (bcmlm_port_update(unit, port, logical_link));
    }

    if (new_link) {
        if (new_fault) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ha->pbm_fault, port);
        } else {
            BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ha->pbm_fault, port);
        }
    }

    if (logical_link) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ha->pbm_logical_link, port);
    } else {
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ha->pbm_logical_link, port);
    }

    if (cur_logical_link != new_link) {
        if (new_link) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UP(unit, port, "port %d: link up\n"),
                      port));
        } else {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UP(unit, port, "port %d: link down\n"),
                      port));
        }
    }

    if (cur_fault != new_fault) {
        if (new_fault) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UP(unit, port, "port %d: fault detected\n"),
                      port));
        } else {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UP(unit, port, "port %d: fault cleared\n"),
                      port));
        }
    }

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_newly_enabled, port) &&
        !BCMDRD_PBMP_MEMBER(lmctrl->pbm_newly_disabled, port)) {
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_newly_enabled, port);
    }

    /* Notify the application for link change */
    if (logical_link != cur_logical_link) {
        SHR_IF_ERR_CONT
            (bcmlm_link_state_tbl_notify(unit, port, logical_link));
    }

    lc_locked = 0;
    LC_UNLOCK(unit);

exit:
    if (lc_locked) {
        LC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

static void
bcmlm_linkscan_hw_interrupt(int unit, uint32_t data)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    if (SHR_FAILURE(bcmlm_hw_linkscan_intr_clear(unit))) {
        return;
    }

    if ((lmctrl != NULL) && (lmctrl->tc != NULL)) {
        lmctrl->hw_change = 1;
        shr_thread_wake(lmctrl->tc);
    }
}

static void
bcmlm_linkscan_thread(shr_thread_t tc, void *arg)
{
    int unit = (unsigned long)(arg);
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    shr_port_t port;
    bcmdrd_pbmp_t pbm_link, pbm_update, empty, new, pbm_hw, pbm_sw, pbm_ovr;
    bcmdrd_pbmp_t pbm_force;
    int interval;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Linkscan starting\n")));

    while (!lmctrl->ready) {
        sal_thread_yield();
    }

    /* Initialize variables. */
    BCMDRD_PBMP_CLEAR(pbm_update);

    BCMDRD_PBMP_CLEAR(lmctrl->pbm_newly_disabled);

    /* Register for hardware linkscan interrupt. */
    if (SHR_FAILURE
            (bcmlm_hw_linkscan_intr_cb_set(unit,
                                           bcmlm_linkscan_hw_interrupt))) {
        SHR_EXIT();
    }

    /* Config HW Linkscan */
    if (SHR_FAILURE
            (bcmlm_hw_linkscan_config(unit, lmctrl->pbm_hw))) {
        SHR_EXIT();
    }
    /*
     * Update hardware linkscan port configure.
     * hw_conf might not be changed when toggling linkscan.
     */
    LC_LOCK(unit);
    if (BCMDRD_PBMP_NOT_NULL(lmctrl->pbm_hw)) {
        lmctrl->hw_conf_change = 1;
    }
    LC_UNLOCK(unit);

    while (1) {
        interval = lmctrl->interval_us;

        LC_LOCK(unit);
        if (BCMDRD_PBMP_NOT_NULL(lmctrl->pbm_newly_disabled)) {
            BCMDRD_PBMP_ITER(lmctrl->pbm_newly_disabled, port) {
                BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_newly_disabled, port);
                if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_newly_enabled, port)) {
                    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_newly_enabled, port);
                }
            }
        }
        LC_UNLOCK(unit);

        if (lmctrl->force_change) {
            LC_LOCK(unit);
            lmctrl->force_change = 0;
            BCMDRD_PBMP_ASSIGN(pbm_force, lmctrl->pbm_ha->pbm_force);
            BCMDRD_PBMP_AND(pbm_force, lmctrl->pbm_linkscan);
            BCMDRD_PBMP_XOR(pbm_force, lmctrl->pbm_suspend);
            BCMDRD_PBMP_AND(pbm_force, lmctrl->pbm_ha->pbm_force);
            BCMDRD_PBMP_AND(pbm_force, lmctrl->pbm_linkscan);
            BCMDRD_PBMP_ASSIGN(pbm_link, lmctrl->pbm_ha->pbm_force_link);
            BCMDRD_PBMP_AND(pbm_link, pbm_force);
            BCMDRD_PBMP_ASSIGN(pbm_update, pbm_link);
            BCMDRD_PBMP_XOR(pbm_update, lmctrl->pbm_ha->pbm_logical_link);
            BCMDRD_PBMP_AND(pbm_update, pbm_force);
            LC_UNLOCK(unit);
            BCMDRD_PBMP_ITER(pbm_update, port) {
                bcmlm_link_update(unit, port);
            }
        }

        if (lmctrl->hw_conf_change) {
            LC_LOCK(unit);
            lmctrl->hw_conf_change = 0;
            BCMDRD_PBMP_ASSIGN(pbm_hw, lmctrl->pbm_hw);
            BCMDRD_PBMP_XOR(pbm_hw, lmctrl->pbm_suspend);
            BCMDRD_PBMP_AND(pbm_hw, lmctrl->pbm_hw);
            LC_UNLOCK(unit);

            /* Update hardware linkscan port configure */
            if (SHR_FAILURE(bcmlm_hw_linkscan_config(unit, pbm_hw))) {
                /* Try to update hw linkscan config again in next round */
                lmctrl->hw_conf_change = 1;
            }

            BCMDRD_PBMP_ITER(pbm_hw, port) {
                bcmlm_link_update(unit, port);
            }
        }

        if (lmctrl->hw_change) {
            LC_LOCK(unit);
            lmctrl->hw_change = 0;
            bcmlm_hw_linkscan_link_get(unit, &pbm_link);
            BCMDRD_PBMP_ASSIGN(pbm_hw, lmctrl->pbm_hw);
            BCMDRD_PBMP_XOR(pbm_hw, lmctrl->pbm_suspend);
            BCMDRD_PBMP_AND(pbm_hw, lmctrl->pbm_hw);
            BCMDRD_PBMP_AND(pbm_link, pbm_hw);
            BCMDRD_PBMP_ASSIGN(pbm_update, pbm_link);
            BCMDRD_PBMP_XOR(pbm_update, lmctrl->pbm_ha->pbm_logical_link);
            BCMDRD_PBMP_AND(pbm_update, pbm_hw);
            BCMDRD_PBMP_ASSIGN(new, lmctrl->pbm_newly_enabled);
            BCMDRD_PBMP_AND(new, pbm_hw);
            BCMDRD_PBMP_OR(pbm_update, new);
            LC_UNLOCK(unit);
            BCMDRD_PBMP_ITER(pbm_update, port) {
                bcmlm_link_update(unit, port);
            }
        }
        if (lmctrl->ovr_change) {
            LC_LOCK(unit);
            lmctrl->ovr_change = 0;
            BCMDRD_PBMP_ASSIGN(pbm_ovr, lmctrl->pbm_ovr);
            BCMDRD_PBMP_XOR(pbm_ovr, lmctrl->pbm_suspend);
            BCMDRD_PBMP_AND(pbm_ovr, lmctrl->pbm_ovr);
            BCMDRD_PBMP_ASSIGN(pbm_link, lmctrl->pbm_ovr_link);
            BCMDRD_PBMP_AND(pbm_link, pbm_ovr);
            BCMDRD_PBMP_ASSIGN(pbm_update, pbm_link);
            BCMDRD_PBMP_XOR(pbm_update, lmctrl->pbm_ha->pbm_logical_link);
            BCMDRD_PBMP_AND(pbm_update, pbm_ovr);
            BCMDRD_PBMP_ASSIGN(new, lmctrl->pbm_newly_enabled);
            BCMDRD_PBMP_AND(new, pbm_ovr);
            BCMDRD_PBMP_OR(pbm_update, new);
            LC_UNLOCK(unit);
            BCMDRD_PBMP_ITER(pbm_update, port) {
                bcmlm_link_update(unit, port);
            }
        }
        if (!BCMDRD_PBMP_IS_NULL(lmctrl->pbm_sw)) {
            LC_LOCK(unit);
            BCMDRD_PBMP_ASSIGN(pbm_sw, lmctrl->pbm_sw);
            BCMDRD_PBMP_XOR(pbm_sw, lmctrl->pbm_suspend);
            BCMDRD_PBMP_AND(pbm_sw, lmctrl->pbm_sw);
            LC_UNLOCK(unit);
            BCMDRD_PBMP_ITER(pbm_sw, port) {
                bcmlm_link_update(unit, port);
            }
        } else {
            interval = SAL_SEM_FOREVER;
        }
        shr_thread_sleep(tc, interval);
        if (shr_thread_stopping(tc)) {
            break;
        }
    }

    /* To prevent getting HW linkscan interrupt after linkscan is disabled,
     * HW linkscan must be disabled.
     */
    BCMDRD_PBMP_CLEAR(empty);
    if (SHR_FAILURE((bcmlm_hw_linkscan_config(unit, empty)))) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Failed to disable hardware linkscan.\n")));
    }

    /* Unregister for hardware linkscan interrupt. */
    bcmlm_hw_linkscan_intr_cb_set(unit, NULL);

exit:
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Linkscan exiting\n")));
}

static void
bcmlm_port_add_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    bool scan = false;

    if (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_valid, port)) {
        return;
    }

    LC_LOCK(unit);
    (void)bcmlm_ctrl_scan_get(unit, port, &scan);
    if (scan) {
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_suspend, port);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_newly_enabled, port);
        if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port)) {
            lmctrl->hw_conf_change = 1;
        }
    } else {
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_suspend, port);
    }
    LC_UNLOCK(unit);

    if (scan) {
        shr_thread_wake(lmctrl->tc);
    }
}

static void
bcmlm_port_del_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    bool scan = false;

    if (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_valid, port)) {
        return;
    }
    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_suspend, port)) {
        return;
    }

    LC_LOCK(unit);
    BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_suspend, port);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ha->pbm_logical_link, port);
    (void)bcmlm_ctrl_scan_get(unit, port, &scan);

    if (scan) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_newly_disabled, port);
        if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port)) {
            lmctrl->hw_conf_change = 1;
        }
    }
    LC_UNLOCK(unit);

    if (scan) {
        shr_thread_wake(lmctrl->tc);
    }
}

static void
bcmlm_port_force_up_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    bool scan = false;

    LC_LOCK(unit);
    BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ha->pbm_force, port);
    BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ha->pbm_force_link, port);
    (void)bcmlm_ctrl_scan_get(unit, port, &scan);

    if (scan) {
        lmctrl->force_change = 1;
    }
    LC_UNLOCK(unit);

    if (scan) {
        shr_thread_wake(lmctrl->tc);
    } else {
        bcmlm_link_update(unit, port);
    }
}

static void
bcmlm_port_force_down_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    bool scan = false;

    LC_LOCK(unit);
    BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ha->pbm_force, port);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ha->pbm_force_link, port);
    (void)bcmlm_ctrl_scan_get(unit, port, &scan);

    if (scan) {
        lmctrl->force_change = 1;
    }
    LC_UNLOCK(unit);

    if (scan) {
        shr_thread_wake(lmctrl->tc);
    } else {
        bcmlm_link_update(unit, port);
    }
}

static void
bcmlm_port_no_force_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    shr_port_t port = (shr_port_t)ev_data;
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    bool scan = false;

    LC_LOCK(unit);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ha->pbm_force, port);
    (void)bcmlm_ctrl_scan_get(unit, port, &scan);
    if (scan) {
        if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port)) {
            lmctrl->hw_change = 1;
        }
        if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port)) {
            lmctrl->ovr_change = 1;
        }
    }
    LC_UNLOCK(unit);

    if (scan) {
        shr_thread_wake(lmctrl->tc);
    }
}

/******************************************************************************
 * Public Functions
 */
int
bcmlm_ctrl_stop(int unit)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_NONE);

    SHR_IF_ERR_EXIT
        (bcmlm_ctrl_linkscan_enable(unit, 0));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_cleanup(int unit)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_NONE);

    if (lmctrl) {
        if (lmctrl->lock) {
            sal_mutex_destroy(lmctrl->lock);
        }
        sal_free(lmctrl);
        bcmlm_ctrl[unit] = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_init(int unit, bool warm)
{
    bcmlm_ctrl_t *lmctrl = NULL;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;

    SHR_FUNC_ENTER(unit);

    if (bcmlm_ctrl[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (bcmlm_ctrl_stop(unit));
        SHR_IF_ERR_EXIT
            (bcmlm_ctrl_cleanup(unit));
    }

    lmctrl = sal_alloc(sizeof(bcmlm_ctrl_t), "bcmlmCtrl");
    if (!lmctrl) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Init bcmlm control info */
    sal_memset(lmctrl, 0, sizeof(bcmlm_ctrl_t));

    /* Alloc the HA pbm structure */
    ha_alloc_size = sizeof(bcmlm_pbm_ha_t);
    ha_req_size = ha_alloc_size;
    lmctrl->pbm_ha = shr_ha_mem_alloc(unit, BCMMGMT_LM_COMP_ID,
                                      BCMLM_CTRL_SUB_COMP_ID,
                                      "bcmlmCtrlPbmp",
                                      &ha_alloc_size);

    SHR_NULL_CHECK(lmctrl->pbm_ha, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(lmctrl->pbm_ha, 0, ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_LM_COMP_ID,
                                        BCMLM_CTRL_SUB_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMLM_PBM_HA_T_ID));
    }

    lmctrl->lock = sal_mutex_create("bcmlm_ctrl_lock");
    if (!lmctrl->lock) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    sal_snprintf(lmctrl->name, sizeof(lmctrl->name), "LM_THREAD.%d", unit);
    lmctrl->interval_us = BCMLM_LINKSCAN_INTERVAL_DEFAULT;
    bcmlm_ctrl[unit] = lmctrl;

exit:
    if (SHR_FUNC_ERR()) {
        if (lmctrl && lmctrl->lock) {
            sal_mutex_destroy(lmctrl->lock);
        }
        if (lmctrl) {
            sal_free(lmctrl);
        }
        bcmlm_ctrl[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_ready(int unit)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    lmctrl->ready = 1;

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_interval_update(int unit, uint32_t interval)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    lmctrl->interval_us = interval;

    shr_thread_wake(lmctrl->tc);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_linkscan_enable(int unit, bool enable)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    sal_usecs_t wait_usec = 10 * SECOND_USEC;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    if (!bcmdrd_feature_is_real_hw(unit)) {
        /* Simulator/emulator requires longer time to finish. */
        wait_usec = 30 * SECOND_USEC;
    }

    if (enable) {
        /* Linkscan is already running */
        if (lmctrl->tc != NULL) {
            if (shr_thread_done(lmctrl->tc)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Linkscan has exited abnormally.\n")));
                SHR_IF_ERR_MSG_EXIT
                    (shr_thread_stop(lmctrl->tc, wait_usec),
                     (BSL_META_U(unit, "Failed to clean up thread.\n")));

            } else {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Linkscan is already running.\n")));
                SHR_EXIT();
            }
        }

        lmctrl->tc = shr_thread_start(lmctrl->name,
                                      LINKSCAN_THREAD_PRI,
                                      bcmlm_linkscan_thread,
                                      ((void *)(unsigned long)(unit)));

        if (lmctrl->tc == NULL) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }

    } else if (lmctrl->tc != NULL) {

        SHR_IF_ERR_EXIT
            (shr_thread_stop(lmctrl->tc, wait_usec));
        lmctrl->tc = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_linkscan_mode_update(int unit,
                                shr_port_t port,
                                bcmlm_linkscan_mode_t mode)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    if ((BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port) &&
         (mode == BCMLM_LINKSCAN_MODE_HW)) ||
        (BCMDRD_PBMP_MEMBER(lmctrl->pbm_sw, port) &&
         (mode == BCMLM_LINKSCAN_MODE_SW)) ||
        (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port) &&
         (mode == BCMLM_LINKSCAN_MODE_OVERRIDE)) ||
        (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_linkscan, port) &&
         (mode == BCMLM_LINKSCAN_MODE_NONE))) {
        /* No linkscan mode change */
        SHR_EXIT();
    }

    LC_LOCK(unit);

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port) ||
        mode == BCMLM_LINKSCAN_MODE_HW) {
        lmctrl->hw_conf_change = 1;
    }
    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port) ||
        mode == BCMLM_LINKSCAN_MODE_OVERRIDE) {
        lmctrl->ovr_change = 1;
    }
    if (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_linkscan, port) &&
        mode != BCMLM_LINKSCAN_MODE_NONE) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_newly_enabled, port);
    }
    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_linkscan, port) &&
        mode == BCMLM_LINKSCAN_MODE_NONE) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_newly_disabled, port);
    }
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_hw, port);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_sw, port);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ovr, port);

    switch (mode) {
    case BCMLM_LINKSCAN_MODE_NONE:
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_linkscan, port);
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_suspend, port);
        break;
    case BCMLM_LINKSCAN_MODE_SW:
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_sw, port);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_linkscan, port);
        break;
    case BCMLM_LINKSCAN_MODE_HW:
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_hw, port);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_linkscan, port);
        break;
    case BCMLM_LINKSCAN_MODE_OVERRIDE:
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ovr, port);
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_linkscan, port);
        break;
    default:
        LC_UNLOCK(unit);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    LC_UNLOCK(unit);

    /* Register change now */
    shr_thread_wake(lmctrl->tc);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_override_link_state_update(int unit,
                                      shr_port_t port,
                                      bool link)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    LC_LOCK(unit);

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr_link, port) ^ link) {
        lmctrl->ovr_change = 1;
    }

    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_ovr_link, port);

    if (link) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ovr_link, port);
    }
    LC_UNLOCK(unit);

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port)) {
        shr_thread_wake(lmctrl->tc);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_manual_sync_update(int unit,
                             shr_port_t port,
                             bool manual_sync)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    bool scan = false;
    bool cur_manual_sync = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_manual_sync, port)) {
        cur_manual_sync = true;
    }
    if (manual_sync == cur_manual_sync) {
        /* No change */
        SHR_EXIT();
    }

    LC_LOCK(unit);
    (void)bcmlm_ctrl_scan_get(unit, port, &scan);
    if (manual_sync) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_manual_sync, port);
    } else {
        if (!scan) {
            if (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_suspend, port)) {
                /*
                 * When scan is disabled and MANUAL_SYNC from 1->0,
                 * trigger port update.
                 */
                BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_sync_change, port);
            }
        } else {
            if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ovr, port)) {
                lmctrl->ovr_change = 1;
            }
            if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_hw, port)) {
                lmctrl->hw_change = 1;
            }
        }
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_manual_sync, port);
    }
    LC_UNLOCK(unit);

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_sync_change, port)) {
        (void) bcmlm_link_update(unit, port);
    }
    if (scan) {
        shr_thread_wake(lmctrl->tc);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_port_state_lookup(int unit,
                             shr_port_t port,
                             int *link_state,
                             int *phy_link)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    bool scan = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmlm_ctrl_scan_get(unit, port, &scan));

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_suspend, port)) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_CONFIG,
             (BSL_META_UP(unit, port, "Port is suspend.\n")));
    }

    if (!scan) {
        /* When scan is disabled, lookup op will update port state. */
        (void)bcmlm_link_update(unit, port);
    }

    LC_LOCK(unit);

    *link_state = 0;
    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ha->pbm_logical_link, port)) {
        *link_state = 1;
    }

    *phy_link = 0;
    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_ha->pbm_physical_link, port)) {
        *phy_link = 1;
    }

    LC_UNLOCK(unit);
    if (BSL_LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        bcmlm_ctrl_debug_dump(unit);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_port_add(int unit, shr_port_t port)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];
    bool scan, wake = false, lock = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    LC_LOCK(unit);
    lock = true;
    BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_valid, port);
    if (bcmpc_lport_to_pport(unit, port) == BCMPC_INVALID_PPORT) {
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_suspend, port);
    } else {
        BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_suspend, port);
    }

    if (BCMDRD_PBMP_MEMBER(lmctrl->pbm_suspend, port)) {
        SHR_EXIT();
    }

    (void)bcmlm_ctrl_scan_get(unit, port, &scan);
    if (scan && !BCMDRD_PBMP_MEMBER(lmctrl->pbm_manual_sync, port)) {
        /* scan enabled and MANUAL_SYNC=0, insert lm_link state by thread */
        wake = true;
    }

exit:
    if (lock) {
        LC_UNLOCK(unit);
    }
    if (wake) {
        shr_thread_wake(lmctrl->tc);
    }
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_port_delete(int unit, shr_port_t port)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);
    LC_LOCK(unit);
    BCMDRD_PBMP_PORT_REMOVE(lmctrl->pbm_valid, port);
    LC_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_port_state_validate(int unit, shr_port_t port)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    LC_LOCK(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    if (!BCMDRD_PBMP_MEMBER(lmctrl->pbm_valid, port) ||
        BCMDRD_PBMP_MEMBER(lmctrl->pbm_suspend, port)) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    LC_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmlm_ctrl_link_reload(int unit)
{
    bcmlm_ctrl_t *lmctrl = bcmlm_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmlm_imm_link_restore(unit, lmctrl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_port_event_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvPortAdd",
                                         bcmlm_port_add_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvPortDelete",
                                         bcmlm_port_del_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvLinkForceUp",
                                         bcmlm_port_force_up_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvLinkForceDown",
                                         bcmlm_port_force_down_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvLinkForceNone",
                                         bcmlm_port_no_force_event_hdl));
exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_port_event_unregister(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvPortAdd",
                                           bcmlm_port_add_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvPortDelete",
                                           bcmlm_port_del_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvLinkForceUp",
                                           bcmlm_port_force_up_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvLinkForceDown",
                                           bcmlm_port_force_down_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvLinkForceNone",
                                           bcmlm_port_no_force_event_hdl));
exit:
    SHR_FUNC_EXIT();
}
