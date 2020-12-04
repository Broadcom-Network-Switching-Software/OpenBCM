/*! \file bcma_clicmd_cd.c
 *
 * CLI 'cd' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/io/bcma_io_dir.h>

#include <bcma/cli/bcma_clicmd_cd.h>

int
bcma_clicmd_cd(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    const char *a = BCMA_CLI_ARG_GET(args);

    if (bcma_io_dir_cd(a) != 0) {
        if (a == NULL) {
            cli_out("%s: Invalid home directory\n", BCMA_CLI_ARG_CMD(args));
        } else {
            cli_out("%s: Invalid directory: %s\n", BCMA_CLI_ARG_CMD(args), a);
        }

        rv = BCMA_CLI_CMD_FAIL;
    }

    return rv;
}
