/*! \file bcma_bcmtrmcmd_trm.c
 *
 * CLI command to display high-water mark for number of outstanding
 * entries/transactions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_pb.h>

#include <bcma/cli/bcma_cli.h>

#include <bcmtrm/trm_api.h>

#include <bcma/bcmtrm/bcma_bcmtrmcmd_trm.h>

static void
display_trm_stat_t_format(shr_pb_t *pb, const char *prefix,
                          shr_lmm_stat_t *stat)
{
    const char *p = prefix ? prefix : "";

    shr_pb_printf(pb, "%sIn use: %"PRIu32"\n", p, stat->in_use);
    shr_pb_printf(pb, "%sAvailable: %"PRIu32"\n", p, stat->avail);
    shr_pb_printf(pb, "%sHigh-water mark: %"PRIu32"\n", p, stat->hwtr_mark);
}

static int
bcmtrmcmd_trm_status(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    shr_lmm_stat_t lmm_stat, *stat = &lmm_stat;
    shr_pb_t *pb;

    pb = shr_pb_create();
    if (pb == NULL) {
        cli_out("%sFail to create print buffer.\n", BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    sal_memset(stat, 0, sizeof(*stat));
    bcmtrm_get_trans_hdl_stat(stat);
    shr_pb_printf(pb, "Transactions:\n");
    display_trm_stat_t_format(pb, "    ", stat);

    sal_memset(stat, 0, sizeof(*stat));
    bcmtrm_get_entry_hdl_stat(stat);
    shr_pb_printf(pb, "Entries:\n");
    display_trm_stat_t_format(pb, "    ", stat);

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}

int
bcma_bcmtrmcmd_trm(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp(arg, "stats") == 0) {
        return bcmtrmcmd_trm_status(cli, args);
    } else {
        return BCMA_CLI_CMD_BAD_ARG;
    }

    return BCMA_CLI_CMD_OK;
}
