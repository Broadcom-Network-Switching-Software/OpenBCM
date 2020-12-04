/*! \file bcma_bcmptmcmd_ptm.c
 *
 * CLI commands for PTM Debug.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_error.h>

#include <bcma/cli/bcma_cli.h>

#include <bcmptm/bcmptm.h>

#include <bcma/bcmptm/bcma_bcmptmcmd_ptm.h>

static int
bcmptmcmd_wal_stats(int unit)
{
    bcmptm_wal_stats_info_t stats_info;
    int tmp_rv;
    tmp_rv = bcmptm_wal_stats_info_get(unit, &stats_info);
    if (SHR_SUCCESS(tmp_rv)) {
        cli_out("\nwal_stats_enabled = %s,\n"
                "max_trans_count = %u (of %u),\n"
                "max_msg_count = %u (of %u),\n"
                "max_op_count = %u (of %u),\n"
                "max_word_count = %u (of %u),\n"
                "max_undo_op_count = %u (of %u),\n"
                "max_undo_word_count = %u (of %u),\n"
                "max_words_in_msg = %u, max_ops_in_msg = %u,\n"
                "max_msgs_in_trans = %u, max_slam_msgs_in_trans = %u\n\n",
                stats_info.stats_enabled ? "True" : "False",
                stats_info.max_trans_count, stats_info.cfg_trans_count,
                stats_info.max_msg_count, stats_info.cfg_msg_count,
                stats_info.max_op_count, stats_info.cfg_op_count,
                stats_info.max_word_count, stats_info.cfg_word_count,
                stats_info.max_undo_op_count, stats_info.cfg_undo_op_count,
                stats_info.max_undo_word_count, stats_info.cfg_undo_word_count,

                stats_info.max_words_in_msg,
                stats_info.max_ops_in_msg,
                stats_info.max_msgs_in_trans,
                stats_info.max_slam_msgs_in_trans);
    }
    return tmp_rv;
}

int
bcma_bcmptmcmd_ptm(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    const char *arg, *cmd;

    unit = cli->cmd_unit;

    cmd = BCMA_CLI_ARG_GET(args);
    if (cmd == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp("wal_stats", cmd) == 0) {
        arg = BCMA_CLI_ARG_GET(args);
        if (arg) {
            if (sal_strcasecmp("enable", arg) == 0) {
                if (SHR_FAILURE(bcmptm_wal_stats_info_enable(unit, TRUE))) {
                    return BCMA_CLI_CMD_FAIL;
                }
            } else if (sal_strcasecmp("disable", arg) == 0) {
                if (SHR_FAILURE(bcmptm_wal_stats_info_enable(unit, FALSE))) {
                    return BCMA_CLI_CMD_FAIL;
                }
            } else if (sal_strcasecmp("clear", arg) == 0) {
                if (SHR_FAILURE(bcmptm_wal_stats_info_clear(unit))) {
                    return BCMA_CLI_CMD_FAIL;
                }
            } else if (sal_strcasecmp("show", arg) == 0) {
                if (SHR_FAILURE(bcmptmcmd_wal_stats(unit))) {
                    return BCMA_CLI_CMD_FAIL;
                }
            } else {
                return BCMA_CLI_CMD_USAGE;
            }
        } else {
            return BCMA_CLI_CMD_USAGE;
        }
    } else {
        return BCMA_CLI_CMD_USAGE;
    }
    return BCMA_CLI_CMD_OK;
}
