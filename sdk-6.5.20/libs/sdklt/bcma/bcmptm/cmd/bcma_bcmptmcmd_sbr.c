/*! \file bcma_bcmptmcmd_sbr.c
 *
 * CLI commands for PTM SBR Debug.
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

#include <bcma/bcmptm/bcma_bcmptmcmd_sbr.h>

int
bcma_bcmptmcmd_sbr(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    const char *cmd, *arg;
    shr_pb_t *pb = NULL;

    unit = cli->cmd_unit;

    cmd = BCMA_CLI_ARG_GET(args);
    if (cmd == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp("stats", cmd) == 0) {
        arg = BCMA_CLI_ARG_GET(args);
        if (arg) {
            pb = shr_pb_create();
            if (sal_strcasecmp("show", arg) == 0) {
                if (SHR_FAILURE(bcmptm_sbr_dump_sw_state(unit, pb))) {
                    shr_pb_destroy(pb);
                    return BCMA_CLI_CMD_FAIL;
                }
            } else if (sal_strcasecmp("show_ha", arg) == 0) {
                if (SHR_FAILURE(bcmptm_sbr_dump_ha_sw_state(unit, pb))) {
                    shr_pb_destroy(pb);
                    return BCMA_CLI_CMD_FAIL;
                }
            } else {
                shr_pb_destroy(pb);
                return BCMA_CLI_CMD_USAGE;
            }
        } else {
            return BCMA_CLI_CMD_USAGE;
        }
    } else {
        return BCMA_CLI_CMD_USAGE;
    }
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
    return BCMA_CLI_CMD_OK;
}
