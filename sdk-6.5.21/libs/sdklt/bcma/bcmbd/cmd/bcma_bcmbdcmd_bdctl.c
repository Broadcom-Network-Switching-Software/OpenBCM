/*! \file bcma_bcmbdcmd_bdctl.c
 *
 * BCMBD CLI 'bdctl' command
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmbd/bcmbd.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_bdctl.h>

int
bcma_bcmbdcmd_bdctl(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = SHR_E_NONE;
    int unit;
    const char *arg;

    unit = cli->cmd_unit;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        cli_out("Unit %d is%s attached.\n",
                unit, bcmbd_attached(unit) ? "" : " not");
    } else if (bcma_cli_parse_cmp("attach", arg, 0)) {
        rv = bcmbd_attach(unit);
    } else if (bcma_cli_parse_cmp("detach", arg, 0)) {
        rv = bcmbd_detach(unit);
    } else if (bcma_cli_parse_cmp("reset", arg, 0)) {
        rv = bcmbd_dev_reset(unit);
    } else if (bcma_cli_parse_cmp("init", arg, 0)) {
        rv = bcmbd_dev_init(unit);
    } else if (bcma_cli_parse_cmp("start", arg, 0)) {
        rv = bcmbd_dev_start(unit);
    } else if (bcma_cli_parse_cmp("stop", arg, 0)) {
        rv = bcmbd_dev_stop(unit);
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    if (rv == SHR_E_UNIT) {
        cli_out("%sInvalid unit number: %d\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
    } else if (rv == SHR_E_UNAVAIL) {
        cli_out("%sUnit %d: operation not supported.\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit);
    } else if (rv != SHR_E_NONE) {
        cli_out("%sUnit %d: operation failed (%d).\n",
                BCMA_CLI_CONFIG_ERROR_STR, unit, rv);
    }

    return BCMA_CLI_CMD_OK;
}
