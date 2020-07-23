/*! \file bcma_bcmmgmtcmd_dev.c
 *
 * CLI command for device control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_error.h>

#include <bcma/cli/bcma_cli.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmmgmt/bcmmgmt.h>

#include <bcma/bcmmgmt/bcma_bcmmgmt.h>
#include <bcma/bcmmgmt/bcma_bcmmgmtcmd_dev.h>

static int
bcmmgmtcmd_dev_stop(int unit, bool all_dev)
{
    int rv = SHR_E_NONE;

    if (all_dev) {
        /* Stop the whole system. */
        rv = bcma_bcmmgmt_stop();
        if (SHR_FAILURE(rv)) {
            cli_out("%sFailed to stop the system: %s (%d)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, shr_errmsg(rv), rv);
        }
    } else {
        /* Stop the current device. */
        if (!bcmmgmt_dev_attached(unit)) {
            cli_out("%sUnit %d is already stopped.\n",
                    BCMA_CLI_CONFIG_ERROR_STR, unit);
            return SHR_E_NONE;
        }
        rv = bcmmgmt_dev_detach(unit);
        if (SHR_FAILURE(rv)) {
            cli_out("%sUnit %d failed to stop: %s (%d)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, unit, shr_errmsg(rv), rv);
        }
    }

    return rv;
}

static int
bcmmgmtcmd_dev_start(int unit, bool all_dev)
{
    int rv = SHR_E_NONE;
    int u;

    if (all_dev) {
        for (u = 0; u < BCMDRD_CONFIG_MAX_UNITS; u++) {
            if (bcmmgmt_dev_attached(u)) {
                cli_out("%sUnit %d is already running.\n",
                        BCMA_CLI_CONFIG_ERROR_STR, u);
                return SHR_E_NONE;
            }
        }
        /* Start the whole system. */
        rv = bcma_bcmmgmt_start(false, false);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFailed to start the system: %s (%d)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, shr_errmsg(rv), rv);
        }
    } else {
        /* Start the current device. */
        if (bcmmgmt_dev_attached(unit)) {
            cli_out("%sUnit %d is already running.\n",
                    BCMA_CLI_CONFIG_ERROR_STR, unit);
            return SHR_E_NONE;
        }
        rv = bcmmgmt_dev_attach(unit, false);
        if (SHR_FAILURE(rv)) {
            cli_out("%sUnit %d failed to start: %s (%d)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, unit, shr_errmsg(rv), rv);
        }
    }

    return rv;
}

static int
bcmmgmtcmd_dev_restart(int unit, bool all_dev)
{
    int rv;

    rv = bcmmgmtcmd_dev_stop(unit, all_dev);
    if (SHR_SUCCESS(rv)) {
        rv = bcmmgmtcmd_dev_start(unit, all_dev);
    }
    return rv;
}

static void
bcmmgmtcmd_dev_status(int unit, bool all_dev)
{
    int idx;
    const char *status;

    for (idx = 0; idx < BCMDRD_CONFIG_MAX_UNITS; idx++) {
        if (!all_dev && idx != unit) {
            continue;
        }
        if (!bcmdrd_dev_exists(idx)) {
            continue;
        }
        status = bcmmgmt_dev_attached(idx) ? "Running" : "Stopped";
        cli_out("Unit %d: %s\n", idx, status);
    }
}

int
bcma_bcmmgmtcmd_dev(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    bool all_dev = false;
    const char *arg, *cmd;

    unit = cli->cmd_unit;

    cmd = BCMA_CLI_ARG_GET(args);
    if (cmd == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg) {
        if (sal_strcasecmp(arg, "all") == 0) {
            all_dev = true;
        } else {
            return BCMA_CLI_CMD_USAGE;
        }
    }

    if (sal_strcasecmp("stop", cmd) == 0) {
        if (SHR_FAILURE(bcmmgmtcmd_dev_stop(unit, all_dev))) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else if (sal_strcasecmp("start", cmd) == 0) {
        if (SHR_FAILURE(bcmmgmtcmd_dev_start(unit, all_dev))) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else if (sal_strcasecmp("restart", cmd) == 0) {
        if (SHR_FAILURE(bcmmgmtcmd_dev_restart(unit, all_dev))) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else if (sal_strcasecmp("status", cmd) == 0) {
        bcmmgmtcmd_dev_status(unit, all_dev);
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    return BCMA_CLI_CMD_OK;
}
