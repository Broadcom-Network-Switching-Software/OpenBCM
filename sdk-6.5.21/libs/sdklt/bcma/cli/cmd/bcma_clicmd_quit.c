/*! \file bcma_clicmd_quit.c
 *
 * CLI 'quit' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_quit.h>

int
bcma_clicmd_quit(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    COMPILER_REFERENCE(cli);
    COMPILER_REFERENCE(args);

    return BCMA_CLI_CMD_EXIT;
}
