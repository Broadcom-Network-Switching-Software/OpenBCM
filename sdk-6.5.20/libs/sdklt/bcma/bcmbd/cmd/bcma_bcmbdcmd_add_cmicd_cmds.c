/*! \file bcma_bcmbdcmd_add_cmicd_cmds.c
 *
 * Add CLI commands for CMICd.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd.h>

#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_get.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_set.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_geti.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_seti.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_list.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_insert.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_delete.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_lookup.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_pop.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_pid.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd_schan.h>

static bcma_cli_command_t shcmd_get = {
    .name = "Get",
    .func = bcma_bcmbdcmd_cmicd_get,
    .desc = BCMA_BCMBDCMD_CMICD_GET_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_GET_SYNOP,
    .help = {
        BCMA_BCMBDCMD_CMICD_GET_HELP
#if BCMA_CLI_CONFIG_MAX_HELP_LINES > 1
      , BCMA_BCMBDCMD_CMICD_GET_HELP_2
#endif
    },
    .examples = BCMA_BCMBDCMD_CMICD_GET_EXAMPLES
};

static bcma_cli_command_t shcmd_set = {
    .name = "Set",
    .func = bcma_bcmbdcmd_cmicd_set,
    .desc = BCMA_BCMBDCMD_CMICD_SET_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_SET_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_SET_HELP },
    .examples = BCMA_BCMBDCMD_CMICD_SET_EXAMPLES
};

static bcma_cli_command_t shcmd_geti = {
    .name = "geti",
    .func = bcma_bcmbdcmd_cmicd_geti,
    .desc = BCMA_BCMBDCMD_CMICD_GETI_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_GETI_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_GETI_HELP }
};

static bcma_cli_command_t shcmd_seti = {
    .name = "seti",
    .func = bcma_bcmbdcmd_cmicd_seti,
    .desc = BCMA_BCMBDCMD_CMICD_SETI_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_SETI_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_SETI_HELP },
    .examples = BCMA_BCMBDCMD_CMICD_SETI_EXAMPLES
};

static bcma_cli_command_t shcmd_list = {
    .name = "List",
    .func = bcma_bcmbdcmd_cmicd_list,
    .desc = BCMA_BCMBDCMD_CMICD_LIST_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_LIST_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_LIST_HELP }
};

static bcma_cli_command_t shcmd_insert = {
    .name = "insert",
    .func = bcma_bcmbdcmd_cmicd_insert,
    .desc = BCMA_BCMBDCMD_CMICD_INSERT_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_INSERT_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_INSERT_HELP },
    .examples = BCMA_BCMBDCMD_CMICD_INSERT_EXAMPLES
};

static bcma_cli_command_t shcmd_delete = {
    .name = "delete",
    .func = bcma_bcmbdcmd_cmicd_delete,
    .desc = BCMA_BCMBDCMD_CMICD_DELETE_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_DELETE_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_DELETE_HELP },
    .examples = BCMA_BCMBDCMD_CMICD_DELETE_EXAMPLES
};

static bcma_cli_command_t shcmd_lookup = {
    .name = "lookup",
    .func = bcma_bcmbdcmd_cmicd_lookup,
    .desc = BCMA_BCMBDCMD_CMICD_LOOKUP_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_LOOKUP_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_LOOKUP_HELP },
    .examples = BCMA_BCMBDCMD_CMICD_LOOKUP_EXAMPLES
};

static bcma_cli_command_t shcmd_pop = {
    .name = "pop",
    .func = bcma_bcmbdcmd_cmicd_pop,
    .desc = BCMA_BCMBDCMD_CMICD_POP_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_POP_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_POP_HELP },
    .examples = BCMA_BCMBDCMD_CMICD_POP_EXAMPLES
};

static bcma_cli_command_t shcmd_pid = {
    .name = "pid",
    .func = bcma_bcmbdcmd_cmicd_pid,
    .desc = BCMA_BCMBDCMD_CMICD_PID_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_PID_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_PID_HELP }
};

static bcma_cli_command_t shcmd_schan = {
    .name = "schan",
    .func = bcma_bcmbdcmd_cmicd_schan,
    .desc = BCMA_BCMBDCMD_CMICD_SCHAN_DESC,
    .synop = BCMA_BCMBDCMD_CMICD_SCHAN_SYNOP,
    .help = { BCMA_BCMBDCMD_CMICD_SCHAN_HELP }
};

void
bcma_bcmbdcmd_add_cmicd_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_get, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_set, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_geti, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_seti, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_list, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_insert, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_delete, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_lookup, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_pop, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_pid, BCMDRD_FT_CMICD);
    bcma_cli_add_command(cli, &shcmd_schan, BCMDRD_FT_CMICD);
}
