/*! \file bcma_bslcmd_console.c
 *
 * CLI BSL shell commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/bsl/bcma_bslcons.h>

#include <bcma/bsl/bcma_bslcmd_console.h>


int
bcma_bslcmd_console(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        cli_out("Console is %s\n",
                bcma_bslcons_is_enabled() ?
                (bcma_bslcons_cli_only_get() ? "CLI only" : "on") :
                "off");
        return BCMA_CLI_CMD_OK;
    }

    bcma_bslcons_cli_only_set(0);
    if (sal_strcasecmp("on", arg) == 0) {
        bcma_bslcons_enable(1);
    } else if (sal_strcasecmp("off", arg) == 0) {
        bcma_bslcons_enable(0);
    } else if (sal_strcasecmp("cli", arg) == 0) {
        bcma_bslcons_enable(1);
        bcma_bslcons_cli_only_set(1);
    } else {
        return BCMA_CLI_CMD_USAGE;
    }
    return BCMA_CLI_CMD_OK;
}
