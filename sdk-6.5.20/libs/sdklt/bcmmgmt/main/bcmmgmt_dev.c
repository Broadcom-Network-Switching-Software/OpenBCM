/*! \file bcmmgmt_dev.c
 *
 * Device attach and detach APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmlt/bcmlt.h>

#include <bcmha/bcmha_mem.h>

#include <bcmmgmt/bcmmgmt.h>
#include <bcmmgmt/bcmmgmt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_DEV

/*******************************************************************************
 * Public functions
 */

int
bcmmgmt_dev_attach(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Attaching device (%s-start)\n"),
              warm ? "warm" : "cold"));

    SHR_IF_ERR_EXIT
        (bcmha_mem_init(unit, NULL, NULL, NULL, NULL, 0));

    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmha_mem_reset(unit));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_device_attach(unit, warm));

    SHR_IF_ERR_EXIT
        (bcmmgmt_pkt_dev_init(unit, warm));

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmha_mem_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

bool
bcmmgmt_dev_attached(int unit)
{
    return bcmlt_device_attached(unit);
}

int
bcmmgmt_dev_attach_all(bool warm)
{
    int unit;
    int attempted = 0;
    int succeeded = 0;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        /* Assume that gaps exit be between different units */
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        /* Skip units that are already running */
        if (bcmmgmt_dev_attached(unit)) {
            continue;
        }
        attempted++;
        if (bcmmgmt_dev_attach(unit, warm) == SHR_E_NONE) {
            succeeded++;
        }
    }
    return (succeeded == attempted) ? SHR_E_NONE : SHR_E_PARTIAL;
}

int
bcmmgmt_dev_internal_detach(int unit, bool keep_netif)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Detaching device\n")));

    /* Notify any registered callbacks */
    bcmmgmt_shutdown_cb_execute(unit);

    if (keep_netif) {
        SHR_IF_ERR_CONT
            (bcmmgmt_pkt_dev_cleanup_keep_netif(unit));
    } else {
        SHR_IF_ERR_CONT
            (bcmmgmt_pkt_dev_cleanup(unit));
    }

    SHR_IF_ERR_CONT
        (bcmlt_device_detach(unit));

    SHR_IF_ERR_CONT
        (bcmha_mem_cleanup(unit));

    SHR_FUNC_EXIT();
}

int
bcmmgmt_dev_detach(int unit)
{
    return bcmmgmt_dev_internal_detach(unit, false);
}

int
bcmmgmt_dev_detach_all(void)
{
    int unit;
    int rv;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        /* Assume that gaps exit be between different units */
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }
        /* Skip units that are not running */
        if (!bcmmgmt_dev_attached(unit)) {
            continue;
        }
        rv = bcmmgmt_dev_detach(unit);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }
    return SHR_E_NONE;
}
