/*! \file bcma_clicmd_pwd.c
 *
 * CLI 'pwd' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/io/bcma_io_dir.h>

#include <bcma/cli/bcma_clicmd_pwd.h>

int
bcma_clicmd_pwd(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    char buf[256];
    char *s;

    s = bcma_io_dir_pwd(buf, sizeof(buf));
    if (s == NULL) {
        cli_out("%s%s: Unable to determine current directory\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));

        return BCMA_CLI_CMD_FAIL;
    }

    cli_out("%s\n", s);

    return BCMA_CLI_CMD_OK;
}
