/*! \file bcmmgmt_pkt.c
 *
 * Start and stop HMI packet drivers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmbd/bcmbd.h>

#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_socket.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_trace.h>

#include <bcmmgmt/bcmmgmt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

/*******************************************************************************
 * Local definitions
 */

#define SHARED_BUF_SIZE         (9472) /* Jumbo frame support. */
#define SHARED_BUF_COUNT        (1024)

static bool bpool = false;
static bool ppool = false;

/*******************************************************************************
 * Public functions
 */

int
bcmmgmt_pkt_core_init(void)
{
    int unit;
    int psim;
    bool sim_enabled = false;
    int ndev = 0;
    bcmpkt_bpool_status_t bpool_status;
    bcmpkt_ppool_status_t ppool_status;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (ppool && bpool) {
        /* Already initialized. */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_unet_attach());

    sim_enabled = false;
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmdrd_dev_exists(unit)) {
            psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
            if (psim) {
                sim_enabled = true;
            }
            ndev++;
        }
    }

    if (!sim_enabled && ndev > 0) {
        SHR_IF_ERR_EXIT
            (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));
        if (!bpool_status.active) {
            SHR_IF_ERR_EXIT
                (bcmpkt_bpool_create(BCMPKT_BPOOL_SHARED_ID,
                                     SHARED_BUF_SIZE, SHARED_BUF_COUNT));
            bpool = true;
        }

        SHR_IF_ERR_EXIT
            (bcmpkt_ppool_status_get(&ppool_status));
        if (!ppool_status.active) {
            SHR_IF_ERR_EXIT
                (bcmpkt_ppool_create(BCMPKT_PPOOL_COUNT_DEF));
            ppool = true;
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmmgmt_pkt_core_cleanup();
    }

    SHR_FUNC_EXIT();
}

int
bcmmgmt_pkt_core_cleanup(void)
{
    int rv;
    int actual_rv = SHR_E_NONE;

    if (ppool) {
        rv = bcmpkt_ppool_destroy();
        if (rv != SHR_E_NONE && actual_rv == SHR_E_NONE) {
            actual_rv = rv;
        }
        if (SHR_SUCCESS(rv)) {
            ppool = false;
        }
    }
    if (bpool) {
        rv = bcmpkt_bpool_cleanup();
        if (rv != SHR_E_NONE && actual_rv == SHR_E_NONE) {
            actual_rv = rv;
        }
        if (SHR_SUCCESS(rv)) {
            bpool = false;
        }
    }

    return actual_rv;
}

int
bcmmgmt_pkt_dev_init(int unit, bool warm)
{
    bcmpkt_dev_drv_types_t drv_type;
    int psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);

    SHR_FUNC_ENTER(unit);

    if (psim) {
        SHR_EXIT();
    }

    if  (bcmpkt_dev_drv_attached(unit)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_attach(unit, BCMPKT_DEV_DRV_T_AUTO));
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_drv_type_get(unit, &drv_type));
    if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
        SHR_IF_ERR_EXIT
            (bcmpkt_socket_drv_attach(unit, BCMPKT_SOCKET_DRV_T_AUTO));
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_trace_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmmgmt_pkt_dev_cleanup(int unit)
{
    int rv;
    int actual_rv = SHR_E_NONE;
    int psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool initialized = false;

    if (psim) {
        return SHR_E_NONE;
    }

    rv = bcmpkt_socket_drv_detach(unit);
    if (actual_rv == SHR_E_NONE) {
        actual_rv = rv;
    }
    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (actual_rv == SHR_E_NONE) {
        actual_rv = rv;
    }
    if (initialized) {
        rv = bcmpkt_dev_cleanup(unit);
        if (actual_rv == SHR_E_NONE) {
            actual_rv = rv;
        }
    }
    rv = bcmpkt_dev_drv_detach(unit);
    if (actual_rv == SHR_E_NONE) {
        actual_rv = rv;
    }

    return actual_rv;
}

int
bcmmgmt_pkt_dev_cleanup_keep_netif(int unit)
{
    int rv;
    int actual_rv = SHR_E_NONE;
    int psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);

    if (psim) {
       return SHR_E_NONE;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Keeping packet I/O active\n")));

    rv = bcmpkt_socket_drv_detach(unit);
    if (actual_rv == SHR_E_NONE) {
        actual_rv = rv;
    }
    rv = bcmpkt_dev_drv_detach(unit);
    if (actual_rv == SHR_E_NONE) {
        actual_rv = rv;
    }

    return actual_rv;
}

int
bcmmgmt_pkt_dev_init_all(bool warm)
{
    bcmpkt_dev_drv_types_t drv_type;
    int unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmmgmt_pkt_core_init());

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }

        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
            continue;
        }

        if  (bcmpkt_dev_drv_attached(unit)) {
            continue;
        }

        /* Base driver is required for packet I/O initialization. */
        if (!bcmbd_attached(unit)) {
            SHR_IF_ERR_EXIT
                (bcmbd_attach(unit));
        }

        SHR_IF_ERR_EXIT
            (bcmpkt_dev_drv_attach(unit, BCMPKT_DEV_DRV_T_AUTO));
        SHR_IF_ERR_EXIT
            (bcmpkt_dev_drv_type_get(unit, &drv_type));
        if (drv_type == BCMPKT_DEV_DRV_T_KNET) {
            SHR_IF_ERR_EXIT
                (bcmpkt_socket_drv_attach(unit, BCMPKT_SOCKET_DRV_T_AUTO));
        }

        SHR_IF_ERR_EXIT
            (bcmpkt_trace_init(unit, warm));
    }

exit:
    SHR_FUNC_EXIT();
}

