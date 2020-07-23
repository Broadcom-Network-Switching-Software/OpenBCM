/*! \file bcma_clicmd_delay.c
 *
 * CLI 'delay' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_udelay.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/cli/bcma_clicmd_delay.h>

int
bcma_clicmd_delay(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *a;
    int usec;

    a = BCMA_CLI_ARG_GET(args);
    if (a == NULL || BCMA_CLI_ARG_CNT(args) > 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_int(a, &usec) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (usec < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    sal_udelay(usec);

    return BCMA_CLI_CMD_OK;
}
