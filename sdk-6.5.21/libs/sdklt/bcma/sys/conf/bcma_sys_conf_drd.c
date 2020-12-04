/*! \file bcma_sys_conf_drd.c
 *
 * Initialize device resource database (DRD).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd.h>

#include <bcma/sys/bcma_sys_probe.h>
#include <bcma/sys/bcma_sys_conf.h>

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_drd_init(bcma_sys_conf_t *sc)
{
    int ndev;

    ndev = bcma_sys_probe();

    return ndev;
}

int
bcma_sys_conf_drd_cleanup(bcma_sys_conf_t *sc)
{
    int unit;

    /* Ensure that base driver is idle before cleaning up DRD */
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmbd_attached(unit)) {
            bcmbd_detach(unit);
        }
    }

    bcma_sys_probe_cleanup();

    return 0;
}
