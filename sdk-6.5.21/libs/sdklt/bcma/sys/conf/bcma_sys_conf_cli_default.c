/*! \file bcma_sys_conf_cli_default.c
 *
 * Initialize CLI with the most default commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcma/bcmtrm/bcma_bcmtrmcmd.h>
#include <bcma/bcmptm/bcma_bcmptmcmd.h>
#include <bcma/bcmdi/bcma_bcmdicmd.h>
#include <bcma/mcs/bcma_mcscmd.h>
#include <bcma/i2c/bcma_i2ccmd.h>
#include <bcma/mon/bcma_moncmd.h>
#include <bcma/pcie/bcma_pciecmd.h>

#include <bcma/sys/bcma_sys_conf.h>

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_cli_default(bcma_sys_conf_t *sc)
{
    int rv;

    /* Initialize CLI infracstructure with basic switch command set */
    rv = bcma_sys_conf_cli_basic(sc);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Add TRM debug commands to debug shell */
    bcma_bcmtrmcmd_add_cmds(sc->dsh);

    /* Add PTM debug commands to debug shell */
    bcma_bcmptmcmd_add_cmds(sc->dsh);

    /* Add Flexcode Loader commands to debug shell */
    bcma_bcmdicmd_add_cmds(sc->dsh);

    /* Add Micro Controller Subsystem (MCS) commands to debug shell */
    bcma_mcscmd_add_cmds(sc->dsh);

    /* Initialize MCS firmware debug output handlers */
    bcma_sys_conf_bd_cli_init(sc);

    /* Add I2C commands to debug shell */
    bcma_i2ccmd_add_cmds(sc->dsh);

    /* Add monitoring commands to debug shell */
    bcma_moncmd_add_cmds(sc->dsh);

    /* Add PCIe commands to debug shell */
    bcma_pciecmd_add_cmds(sc->dsh);

    return SHR_E_NONE;
}
