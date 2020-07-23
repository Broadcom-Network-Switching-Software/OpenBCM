/*! \file bcma_sys_conf_drd.c
 *
 * Initialize device resource database (DRD) functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd.h>

#include <bcma/cli/bcma_cli_unit.h>
#include <bcma/bcmdrd/bcma_bcmdrdcmd.h>
#include <bcma/sys/bcma_sys_probe.h>
#include <bcma/sys/bcma_sys_conf.h>

/*******************************************************************************
 * CLI unit management
 */

static int
unit_max(void *cookie)
{
    return BCMDRD_CONFIG_MAX_UNITS;
}

static int
unit_valid(void *cookie, int unit)
{
    return bcmdrd_dev_exists(unit);
}

static int
unit_feature_enabled(void *cookie, int unit, int feature)
{
    if (!bcmdrd_dev_exists(unit)) {
        return FALSE;
    }

    return bcmdrd_feature_enabled(unit, feature);
}

static bcma_cli_unit_cb_t unit_cb = {
    .unit_max = unit_max,
    .unit_valid = unit_valid,
    .unit_feature_enabled = unit_feature_enabled
};

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_drd_init(bcma_sys_conf_t *sc)
{
    int ndev;

    /* Check for valid sys_conf structure */
    if (sc == NULL) {
        return -1;
    }

    bcma_cli_unit_cb_set(sc->cli, &unit_cb, NULL);
    bcma_cli_unit_cb_set(sc->dsh, &unit_cb, NULL);

    ndev = bcma_sys_probe();

    /* Select unit 0 by default */
    if (ndev > 0) {
        bcma_cli_unit_set(sc->cli, 0);
        bcma_cli_unit_set(sc->dsh, 0);
    }


    /* Add CLI commands for device resource database to debug shell */
    bcma_bcmdrdcmd_add_cmds(sc->dsh);

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

    bcma_cli_unit_set(sc->cli, -1);
    bcma_cli_unit_set(sc->dsh, -1);

    return 0;
}
