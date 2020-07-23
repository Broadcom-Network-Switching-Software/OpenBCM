/*! \file bcma_clicmd_add_system_cmds.c
 *
 * CLI system commands support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_version.h>
#include <bcma/cli/bcma_clicmd_shell.h>
#include <bcma/cli/bcma_clicmd_grep.h>
#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_sort.h>

/* CLI system command set */
static bcma_cli_command_t cmd_version = {
    .name = "version",
    .func = bcma_clicmd_version,
    .desc = BCMA_CLICMD_VERSION_DESC,
};

static bcma_cli_command_t cmd_shell = {
    .name = "shell",
    .func = bcma_clicmd_shell,
    .desc = BCMA_CLICMD_SHELL_DESC,
};

static bcma_cli_command_t cmd_grep = {
    .name = "grep",
    .func = bcma_clicmd_grep,
    .desc = BCMA_CLICMD_GREP_DESC,
    .synop = BCMA_CLICMD_GREP_SYNOP,
    .help = { BCMA_CLICMD_GREP_HELP },
    .examples = BCMA_CLICMD_GREP_EXAMPLES
};

static bcma_cli_command_t cmd_sort = {
    .name = "sort",
    .func = bcma_clicmd_sort,
    .desc = BCMA_CLICMD_SORT_DESC,
    .synop = BCMA_CLICMD_SORT_SYNOP,
    .help = { BCMA_CLICMD_SORT_HELP },
    .examples = BCMA_CLICMD_SORT_EXAMPLES
};

int
bcma_clicmd_add_system_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_version, 0);
    bcma_cli_add_command(cli, &cmd_shell, 0);
    bcma_cli_add_command(cli, &cmd_grep, 0);
    bcma_cli_add_command(cli, &cmd_sort, 0);

    return 0;
}
