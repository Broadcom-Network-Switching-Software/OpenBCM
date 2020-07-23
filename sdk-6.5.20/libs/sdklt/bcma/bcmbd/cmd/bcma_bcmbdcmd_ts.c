/*! \file bcma_bcmbdcmd_ts.c
 *
 * BCMBD CLI 'TS' command
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_types.h>

#include <shr/shr_error.h>

#include <bcmbd/bcmbd_ts.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_ts.h>

static int
ts_timestamp_enable_get(int unit, int ts_inst)
{
    int rv;
    bool ts_en = false;
    uint64_t timestamp = 0;

    rv = bcmbd_ts_timestamp_enable_get(unit, ts_inst, &ts_en);
    if (SHR_FAILURE(rv)) {
        if (rv != SHR_E_PARAM) {
            cli_out("%sFailed to get TS %d timestamp enable status (%s).\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ts_inst, shr_errmsg(rv));
        }
        return rv;
    }

    cli_out("TS %d timestamp counter is %s",
            ts_inst, ts_en ? "enabled" : "disabled");
    if (ts_en) {
        rv = bcmbd_ts_timestamp_nanosec_get(unit, ts_inst, &timestamp);
        if (SHR_SUCCESS(rv)) {
            cli_out(" (%"PRIu64" ns)", timestamp);
        }
    }
    cli_out(".\n");
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to get TS %d timestamp value (%s).\n",
                BCMA_CLI_CONFIG_ERROR_STR, ts_inst, shr_errmsg(rv));
    }

    return rv;
}

static int
ts_timestamp_enable_set(int unit, int ts_inst, bool ts_en)
{
    int rv;

    rv = bcmbd_ts_timestamp_enable_set(unit, ts_inst, ts_en);
    if (SHR_SUCCESS(rv)) {
        cli_out("%s TS %d timestamp counter.\n",
                ts_en ? "Enable" : "Disable", ts_inst);
    } else if (rv != SHR_E_PARAM) {
        cli_out("%sFailed to set enable status for TS %d timestamp (%s).\n",
                BCMA_CLI_CONFIG_ERROR_STR, ts_inst, shr_errmsg(rv));
    }

    return rv;
}

static int
cmd_ts_timestamp(bcma_cli_t *cli, bcma_cli_args_t *args, bool ts_set)
{
    int rv = SHR_E_NONE;
    int unit = cli->cmd_unit;
    int idx = 0, ts_inst = -1, ts_en = -1;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "inst", "int", &ts_inst, NULL);
    if (ts_set) {
        bcma_cli_parse_table_add(&pt, "TimeStampEnable", "bool", &ts_en, NULL);
    }
    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    if (ts_set && (ts_inst < 0 || ts_en < 0)) {
        return BCMA_CLI_CMD_USAGE;
    }

    for (idx = 0;; idx++) {
        /* TS instance is specified from CLI input */
        if (ts_inst >= 0) {
            idx = ts_inst;
        }
        if (!ts_set) {
            /* Get enable status */
            rv = ts_timestamp_enable_get(unit, idx);
        } else {
            /* Set enable status */
            rv = ts_timestamp_enable_set(unit, idx, ts_en);
        }

        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_PARAM) {
                if (ts_inst >= 0) {
                    cli_out("%sInvalid TS instance %d\n",
                            BCMA_CLI_CONFIG_ERROR_STR, ts_inst);
                } else {
                    /* Reaches the maximum TS instance number */
                    return BCMA_CLI_CMD_OK;
                }
            } else {
                break;
            }
        }

        if (ts_inst >= 0) {
            break;
        }
    }

    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

int
bcma_bcmbdcmd_ts(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    if ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
        if (bcma_cli_parse_cmp("get", arg, 0)) {
            return cmd_ts_timestamp(cli, args, false);
        } else if (bcma_cli_parse_cmp("set", arg, 0)) {
            return cmd_ts_timestamp(cli, args, true);
        }

    }

    return BCMA_CLI_CMD_USAGE;
}
