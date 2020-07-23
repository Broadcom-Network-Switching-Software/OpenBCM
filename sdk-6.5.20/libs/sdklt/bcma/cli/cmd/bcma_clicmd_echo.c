/*! \file bcma_clicmd_echo.c
 *
 * CLI 'echo' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_echo.h>

int
bcma_clicmd_echo(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *a;
    int suppress_nl = FALSE;

    a = BCMA_CLI_ARG_GET(args);
    if (a && sal_strcmp(a, "-n") == 0) {
        suppress_nl = TRUE;
        a = BCMA_CLI_ARG_GET(args);
    }

    while (a != NULL) {
        cli_out("%s", a);
        if ((a = BCMA_CLI_ARG_GET(args)) != NULL) {
            cli_out(" ");
        }
    }

    if (!suppress_nl) {
        cli_out("\n");
    }

    return BCMA_CLI_CMD_OK;
}
