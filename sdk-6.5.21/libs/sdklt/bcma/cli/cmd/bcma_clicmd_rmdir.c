/*! \file bcma_clicmd_rmdir.c
 *
 * CLI 'rmdir' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/io/bcma_io_dir.h>

#include <bcma/cli/bcma_clicmd_rmdir.h>

int
bcma_clicmd_rmdir(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *a;

    if (BCMA_CLI_ARG_CNT(args) != 1) {
        return BCMA_CLI_CMD_USAGE;
    }

    a = BCMA_CLI_ARG_GET(args);

    if (bcma_io_dir_rmdir(a) < 0) {
        cli_out("%s: Warning: failed to delete directory: %s\n",
                BCMA_CLI_ARG_CMD(args), a);
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}
