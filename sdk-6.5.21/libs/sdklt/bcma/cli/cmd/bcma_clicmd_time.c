/*! \file bcma_clicmd_time.c
 *
 * CLI 'time' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_time.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_time.h>

static void
format_time(sal_usecs_t t)
{
    sal_usecs_t sec;

    sec = t / 1000000;
    cli_out("%"PRIu32".%06"PRIu32" sec", sec, t - sec * 1000000);
}

int
bcma_clicmd_time(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    const char *a;
    sal_usecs_t t0, ts, te;

    a = BCMA_CLI_ARG_GET(args);
    if (a == NULL) {
        format_time(sal_time_usecs());
        cli_out("\n");
        return BCMA_CLI_CMD_OK;
    }

    t0 = sal_time_usecs();
    for (ts = t0; a != NULL; a = BCMA_CLI_ARG_GET(args), ts = sal_time_usecs()) {
        rv = bcma_cli_cmd_process(cli, a);
        if (rv != BCMA_CLI_CMD_OK) {
            return rv;
        }
        te = sal_time_usecs();
        cli_out("time{%s} = ", a);
        format_time(te - ts);
        cli_out("\n");
    }
    cli_out("time{TOTAL} = ");
    format_time(te - t0);
    cli_out("\n");

    return BCMA_CLI_CMD_OK;
}
