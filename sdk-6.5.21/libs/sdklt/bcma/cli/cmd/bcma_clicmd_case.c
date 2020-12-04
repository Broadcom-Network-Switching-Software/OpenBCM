/*! \file bcma_clicmd_case.c
 *
 * CLI 'case' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_case.h>

int
bcma_clicmd_case(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    const char *key, *val, *cmd;

    key = BCMA_CLI_ARG_GET(args);
    if (key == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    while ((val = BCMA_CLI_ARG_GET(args)) != NULL) {
        cmd = BCMA_CLI_ARG_GET(args);
        if (cmd == NULL) {
            return BCMA_CLI_CMD_USAGE;
        }

        if (sal_strcmp(val, key) == 0 || sal_strcmp(val, "*") == 0) {
            rv = bcma_cli_cmd_process(cli, cmd);
            BCMA_CLI_ARG_DISCARD(args);
            break;
        }
    }

    return rv;
}
