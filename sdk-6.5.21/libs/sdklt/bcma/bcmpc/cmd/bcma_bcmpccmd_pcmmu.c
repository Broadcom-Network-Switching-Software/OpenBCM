/*! \file bcma_bcmpccmd_pcmmu.c
 *
 * CLI debug command for PC/TM interfaction.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_error.h>

#include <bcmpc/bcmpc_tm.h>

#include <bcma/bcmpc/bcma_bcmpccmd_pcmmu.h>

int
bcma_bcmpccmd_pcmmu(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit;
    bool manual;
    const char *arg;

    unit = cli->cmd_unit;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg) {
        while (arg) {
            if (sal_strcasecmp(arg, "manual") == 0) {
                bcmpc_tm_manual_update_mode_set(unit, true);
            } else if (sal_strcasecmp(arg, "auto") == 0) {
                bcmpc_tm_manual_update_mode_set(unit, false);
            } else if (sal_strcasecmp(arg, "update") == 0) {
                bcmpc_tm_update_now(unit);
            } else {
                cli_out("%sUnrecognized sub-command: %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, arg);
                return BCMA_CLI_CMD_FAIL;
            }
            arg = BCMA_CLI_ARG_GET(args);
        }
    } else {
        rv = bcmpc_tm_manual_update_mode_get(unit, &manual);
        if (SHR_FAILURE(rv)) {
            cli_out("%sUnit %d: error getting MMU update mode\n",
                    BCMA_CLI_CONFIG_ERROR_STR, unit);
            return BCMA_CLI_CMD_FAIL;
        }
        cli_out("Current MMU update mode: %s\n",
                manual ? "manual" : "auto");
    }

    return BCMA_CLI_CMD_OK;
}
