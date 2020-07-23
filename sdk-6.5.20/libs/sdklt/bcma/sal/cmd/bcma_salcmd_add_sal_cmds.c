/*! \file bcma_salcmd_add_sal_cmds.c
 *
 * Add CLI commands for SAL debug.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/sal/bcma_salcmd.h>

#include <bcma/sal/bcma_salcmd_sal.h>
#include <bcma/sal/bcma_salcmd_thread.h>
#include <bcma/sal/bcma_salcmd_date.h>
#include <bcma/sal/bcma_salcmd_lmm.h>
#include <bcma/sal/bcma_salcmd_backtrace.h>

static bcma_cli_command_t shcmd_sal = {
    .name = "sal",
    .func = bcma_salcmd_sal,
    .desc = BCMA_SALCMD_SAL_DESC,
    .synop = BCMA_SALCMD_SAL_SYNOP,
    .help = { BCMA_SALCMD_SAL_HELP },
    .examples = BCMA_SALCMD_SAL_EXAMPLES
};

static bcma_cli_command_t shcmd_thread = {
    .name = "thread",
    .func = bcma_salcmd_thread,
    .desc = BCMA_SALCMD_THREAD_DESC,
    .synop = BCMA_SALCMD_THREAD_SYNOP,
    .help = { BCMA_SALCMD_THREAD_HELP },
    .examples = BCMA_SALCMD_THREAD_EXAMPLES
};

static bcma_cli_command_t shcmd_date = {
    .name = "date",
    .func = bcma_salcmd_date,
    .desc = BCMA_SALCMD_DATE_DESC,
    .synop = BCMA_SALCMD_DATE_SYNOP,
    .help = { BCMA_SALCMD_DATE_HELP },
};

static bcma_cli_command_t shcmd_lmm = {
    .name = "LMM",
    .func = bcma_salcmd_lmm,
    .desc = BCMA_SALCMD_LMM_DESC,
    .synop = BCMA_SALCMD_LMM_SYNOP,
};

static bcma_cli_command_t shcmd_backtrace = {
    .name = "BackTrace",
    .func = bcma_salcmd_backtrace,
    .cleanup = bcma_salcmd_backtrace_cleanup,
    .desc = BCMA_SALCMD_BACKTRACE_DESC,
    .synop = BCMA_SALCMD_BACKTRACE_SYNOP,
    .help = { BCMA_SALCMD_BACKTRACE_HELP },
    .examples = BCMA_SALCMD_BACKTRACE_EXAMPLES,
};

void
bcma_salcmd_add_sal_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_sal, 0);
    bcma_cli_add_command(cli, &shcmd_thread, 0);
    bcma_cli_add_command(cli, &shcmd_date, 0);
    bcma_cli_add_command(cli, &shcmd_lmm, 0);
    bcma_cli_add_command(cli, &shcmd_backtrace, 0);
}
