/*! \file bcma_clicmd_add_script_cmds.c
 *
 * CLI script commands support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_echo.h>
#include <bcma/cli/bcma_clicmd_loop.h>
#include <bcma/cli/bcma_clicmd_for.h>
#include <bcma/cli/bcma_clicmd_each.h>
#include <bcma/cli/bcma_clicmd_if.h>
#include <bcma/cli/bcma_clicmd_case.h>
#include <bcma/cli/bcma_clicmd_expr.h>
#include <bcma/cli/bcma_clicmd_delay.h>
#include <bcma/cli/bcma_clicmd_sleep.h>
#include <bcma/cli/bcma_clicmd_time.h>
#include <bcma/cli/bcma_clicmd.h>

/* CLI script command set */
static bcma_cli_command_t cmd_echo = {
    .name = "echo",
    .func = bcma_clicmd_echo,
    .desc = BCMA_CLICMD_ECHO_DESC,
    .synop = BCMA_CLICMD_ECHO_SYNOP,
    .help = { BCMA_CLICMD_ECHO_HELP }
};

static bcma_cli_command_t cmd_loop = {
    .name = "loop",
    .func = bcma_clicmd_loop,
    .desc = BCMA_CLICMD_LOOP_DESC,
    .synop = BCMA_CLICMD_LOOP_SYNOP,
    .help = { BCMA_CLICMD_LOOP_HELP }
};

static bcma_cli_command_t cmd_for = {
    .name = "for",
    .func = bcma_clicmd_for,
    .desc = BCMA_CLICMD_FOR_DESC,
    .synop = BCMA_CLICMD_FOR_SYNOP,
    .help = {
        BCMA_CLICMD_FOR_HELP
#if BCMA_CLI_CONFIG_MAX_HELP_LINES > 1
      , BCMA_CLICMD_FOR_HELP_2
#endif
    },
    .examples = BCMA_CLICMD_FOR_EXAMPLES
};

static bcma_cli_command_t cmd_each = {
    .name = "each",
    .func = bcma_clicmd_each,
    .desc = BCMA_CLICMD_EACH_DESC,
    .synop = BCMA_CLICMD_EACH_SYNOP,
    .help = {
        BCMA_CLICMD_EACH_HELP
#if BCMA_CLI_CONFIG_MAX_HELP_LINES > 1
      , BCMA_CLICMD_EACH_HELP_2
#endif
    },
    .examples = BCMA_CLICMD_EACH_EXAMPLES
};

static bcma_cli_command_t cmd_if = {
    .name = "if",
    .func = bcma_clicmd_if,
    .desc = BCMA_CLICMD_IF_DESC,
    .synop = BCMA_CLICMD_IF_SYNOP,
    .help = {
        BCMA_CLICMD_IF_HELP
#if BCMA_CLI_CONFIG_MAX_HELP_LINES > 1
      , BCMA_CLICMD_IF_HELP_2
#endif
    },
    .examples = BCMA_CLICMD_IF_EXAMPLES
};

static bcma_cli_command_t cmd_case = {
    .name = "case",
    .func = bcma_clicmd_case,
    .desc = BCMA_CLICMD_CASE_DESC,
    .synop = BCMA_CLICMD_CASE_SYNOP,
    .help = { BCMA_CLICMD_CASE_HELP },
    .examples = BCMA_CLICMD_CASE_EXAMPLES
};

static bcma_cli_command_t cmd_expr = {
    .name = "expr",
    .func = bcma_clicmd_expr,
    .desc = BCMA_CLICMD_EXPR_DESC,
    .synop = BCMA_CLICMD_EXPR_SYNOP,
    .help = { BCMA_CLICMD_EXPR_HELP }
};

static bcma_cli_command_t cmd_delay = {
    .name = "delay",
    .func = bcma_clicmd_delay,
    .desc = BCMA_CLICMD_DELAY_DESC,
    .synop = BCMA_CLICMD_DELAY_SYNOP,
    .help = { BCMA_CLICMD_DELAY_HELP }
};

static bcma_cli_command_t cmd_sleep = {
    .name = "sleep",
    .func = bcma_clicmd_sleep,
    .desc = BCMA_CLICMD_SLEEP_DESC,
    .synop = BCMA_CLICMD_SLEEP_SYNOP,
    .help = { BCMA_CLICMD_SLEEP_HELP }
};

static bcma_cli_command_t cmd_time = {
    .name = "time",
    .func = bcma_clicmd_time,
    .desc = BCMA_CLICMD_TIME_DESC,
    .synop = BCMA_CLICMD_TIME_SYNOP,
    .help = { BCMA_CLICMD_TIME_HELP },
    .examples = BCMA_CLICMD_TIME_EXAMPLES
};

int
bcma_clicmd_add_script_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_echo, 0);
    bcma_cli_add_command(cli, &cmd_loop, 0);
    bcma_cli_add_command(cli, &cmd_for, 0);
    bcma_cli_add_command(cli, &cmd_each, 0);
    bcma_cli_add_command(cli, &cmd_if, 0);
    bcma_cli_add_command(cli, &cmd_case, 0);
    bcma_cli_add_command(cli, &cmd_expr, 0);
    bcma_cli_add_command(cli, &cmd_delay, 0);
    bcma_cli_add_command(cli, &cmd_sleep, 0);
    bcma_cli_add_command(cli, &cmd_time, 0);

    return 0;
}
