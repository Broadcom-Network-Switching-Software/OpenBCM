/*! \file bcma_sys_conf_cli_basic.c
 *
 * Initialize CLI with the most basic commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcma/bcmlt/bcma_bcmltcmd.h>
#include <bcma/bcmdrd/bcma_bcmdrdcmd.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_dev.h>
#include <bcma/tecsup/bcma_tecsupcmd.h>
#include <bcma/bcmpc/bcma_bcmpccmd.h>
#include <bcma/fwm/bcma_fwmcmd.h>
#include <bcma/ha/bcma_hacmd.h>

#include <bcma/sys/bcma_sys_conf.h>

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_cli_basic(bcma_sys_conf_t *sc)
{
    /* Use default CLI prompt prefix function */
    if (bcma_sys_conf_cli_prompt_prefix_default_set(sc) < 0) {
        return SHR_E_FAIL;
    }

    /* Initialize basic CLI */
    if (bcma_sys_conf_cli_init(sc) < 0) {
        return SHR_E_FAIL;
    }

    /* Add CLI command completion support */
    bcma_sys_conf_clirlc_init();

    /* Add CLI commands for LT/PT operations */
    bcma_bcmltcmd_add_cmds(sc->cli);

    /* Add CLI commands for LT-based applications */
    bcma_bcmltcmd_add_appl_cmds(sc->cli);

    /* Add CLI commands for DRD to debug shell */
    bcma_bcmdrdcmd_add_cmds(sc->dsh);

    /* Add CLI commands for base driver to debug shell */
    bcma_bcmbdcmd_add_cmicd_cmds(sc->dsh);
    bcma_bcmbdcmd_add_cmicx_cmds(sc->dsh);
    bcma_bcmbdcmd_add_dev_cmds(sc->dsh);

    /* Add CLI commands for technical support */
    bcma_tecsupcmd_add_cmds(sc->cli);

    /* Add Port Control commands */
    bcma_bcmpccmd_add_cmds(sc->cli);

    /* Add PHY diagnostics commands to debug shell */
    bcma_bcmpccmd_add_phy_cmds(sc->dsh);

    /* Add Firmware Management commands to debug shell */
    bcma_fwmcmd_add_cmds(sc->dsh);

    /* Add CLI commands for HA memory diagnostics to debug shell */
    bcma_hacmd_add_cmds(sc->dsh);

    return SHR_E_NONE;
}
