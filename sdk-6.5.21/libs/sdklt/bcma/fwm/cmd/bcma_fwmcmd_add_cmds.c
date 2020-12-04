/*! \file bcma_fwmcmd_add_cmds.c
 *
 * Add CLI commands for Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/fwm/bcma_fwmcmd_fwm.h>
#include <bcma/fwm/bcma_fwmcmd.h>

static bcma_cli_command_t shcmd_fwm = {
    .name = "fwm",
    .func = bcma_fwmcmd_fwm,
    .desc = BCMA_FWMCMD_FWM_DESC,
    .synop = BCMA_FWMCMD_FWM_SYNOP,
    .help = { BCMA_FWMCMD_FWM_HELP },
    .examples = BCMA_FWMCMD_FWM_EXAMPLES
};

int
bcma_fwmcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_fwm, 0);
    return 0;
}

