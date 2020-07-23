/*! \file bcma_clicmd_shell.c
 *
 * CLI 'shell' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/io/bcma_io_shell.h>

#include <bcma/cli/bcma_clicmd_shell.h>

int
bcma_clicmd_shell(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_io_shell(NULL, NULL, NULL);

    return BCMA_CLI_CMD_OK;
}
