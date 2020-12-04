/*! \file bcma_moncmd_ft.c
 *
 * CLI commands for FlowTracker Debug.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_error.h>

#include <bcmcth/bcmcth_mon_flowtracker_learn.h>

#include <bcma/mon/bcma_moncmd_ft.h>

#include <bcmbd/mcs_shared/mcs_mon_flowtracker_msg.h>

#include <bcmcth/bcmcth_mon_flowtracker_util.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

static int
bcmmoncmd_ft_learn_dump(int unit)
{
    bcmcth_mon_flowtracker_learn_ctrl_debug_t info;
    int tmp_rv, pipe;
    bcmcth_mon_flowtracker_learn_ctrl_pipe_t *pipe_info = NULL;
    tmp_rv = bcmcth_mon_ft_learn_thread_info_dump(unit, &info);
    if (SHR_SUCCESS(tmp_rv)) {
        for (pipe = 0; pipe < info.num_pipes; pipe++) {
            pipe_info = &(info.pipe_info[pipe]);
            cli_out("\nthread_id(Pipe %d) = %p\n"
                    "sleep_intvl(Pipe %d) = %u\n"
                    "is_active(Pipe %d) = %d\n",
                    pipe, pipe_info->thread_ctrl,
                    pipe, pipe_info->sleep_intvl,
                    pipe, pipe_info->is_active
                    );
        }
    }

    return tmp_rv;
}

static int
bcmmoncmd_ft_export_stats(int unit) {

    mcs_ft_msg_ctrl_stats_export_t msg;
    int tmp_rv;

    sal_memset(&msg, 0, sizeof(msg));

    tmp_rv = bcmcth_mon_ft_util_export_stats_get(unit, &msg);

    if (SHR_SUCCESS(tmp_rv)) {
        cli_out("Export packets      : %u \n", msg.num_pkts);
        cli_out("Export flows        : %u \n", msg.num_flows);
        cli_out("Export time stamp   : %u \n", msg.ts);
        cli_out("Sequence number     : %u \n", msg.seq_num);
        cli_out("\n");

    }
    cli_out("\n");

    return tmp_rv;
}

static int
bcmmoncmd_ft_learn_stats(int unit) {

    mcs_ft_msg_ctrl_stats_learn_t msg;
    int tmp_rv, i;
    char *status[] = FT_LEARN_PKT_STATUS_NAMES;

    sal_memset(&msg, 0, sizeof(msg));

    tmp_rv = bcmcth_mon_ft_util_learn_stats_get(unit, &msg);

    if (SHR_SUCCESS(tmp_rv)) {
        cli_out(" +------------------------+-------------+--------------------+ \n");
        cli_out(" |         Status         |    Count    |   Average (usec)   | \n");
        cli_out(" +------------------------+-------------+--------------------+ \n");
        for (i = 0; i < msg.num_status; i++) {
            cli_out(" | %s | %11u | %18u |\n",
                    status[i], msg.count[i], msg.avg_usecs[i]);
            cli_out(" +------------------------+-------------+--------------------+ \n");
        }
        cli_out("\n");
    }
        return tmp_rv;
}

static int
bcmmoncmd_ft_age_stats(int unit) {

    mcs_ft_msg_ctrl_stats_age_t msg;
    int tmp_rv;

    sal_memset(&msg, 0, sizeof(msg));

    tmp_rv = bcmcth_mon_ft_util_age_stats_get(unit, &msg);

    if (SHR_SUCCESS(tmp_rv)) {
        cli_out("Num flows aged : %u \n", msg.num_flows);

    }
    cli_out("\n");
    return tmp_rv;
}

int
bcma_moncmd_ft(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    const char *arg, *cmd;

    unit = cli->cmd_unit;

    cmd = BCMA_CLI_ARG_GET(args);
    if (cmd == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp("sw", cmd) == 0) {
        arg = BCMA_CLI_ARG_GET(args);
        if (arg) {
            if (sal_strcasecmp("dump", arg) == 0) {
                /* Learn thread info. */
                if (SHR_FAILURE(bcmmoncmd_ft_learn_dump(unit))) {
                    return BCMA_CLI_CMD_FAIL;
                }
                /* Add other SW data structures dump as needed */
            } else {
                return BCMA_CLI_CMD_USAGE;
            }
        } else {
            return BCMA_CLI_CMD_USAGE;
        }
    } else if (sal_strcasecmp("stats", cmd) == 0) {
        arg = BCMA_CLI_ARG_GET(args);
        if (arg) {
            if (sal_strcasecmp("learn", arg) == 0) {
                /* Learn stats. */
                if (SHR_FAILURE(bcmmoncmd_ft_learn_stats(unit))) {
                    return BCMA_CLI_CMD_FAIL;
                }
            } else if (sal_strcasecmp("export", arg) == 0) {
                /* Export stats. */
                if (SHR_FAILURE(bcmmoncmd_ft_export_stats(unit))) {
                    return BCMA_CLI_CMD_FAIL;
                }
            } else if (sal_strcasecmp("age", arg) == 0) {
                /* Age stats. */
                if (SHR_FAILURE(bcmmoncmd_ft_age_stats(unit))) {
                    return BCMA_CLI_CMD_FAIL;
                }
            } else {
                return BCMA_CLI_CMD_USAGE;
            }
        }
    } else {
        return BCMA_CLI_CMD_USAGE;
    }
    return BCMA_CLI_CMD_OK;
}
