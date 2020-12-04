/*! \file bcmmgmt_shutdown_keep_netif.c
 *
 * Single entry point shut-down of core SDK components.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlt/bcmlt.h>
#include <bcmmgmt/bcmmgmt_internal.h>

#include <bcmmgmt/bcmmgmt.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

/*******************************************************************************
 * Public functions
 */

int
bcmmgmt_shutdown_keep_netif(bool graceful)
{
    int unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Shutdown without cleaning up network interfaces */
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmmgmt_dev_attached(unit)) {
            continue;
        }
        SHR_IF_ERR_CONT
            (bcmmgmt_dev_internal_detach(unit, true));
    }

    SHR_IF_ERR_CONT
        (bcmmgmt_core_internal_stop(graceful, true));

    SHR_IF_ERR_CONT
        (bcmmgmt_core_cleanup());

    SHR_FUNC_EXIT();
}
