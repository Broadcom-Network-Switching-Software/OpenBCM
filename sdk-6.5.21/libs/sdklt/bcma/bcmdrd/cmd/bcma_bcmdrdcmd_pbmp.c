/*! \file bcma_bcmdrdcmd_pbmp.c
 *
 * CLI 'pbmp' command handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_pb_format.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcma/cli/bcma_cli_var.h>
#include <bcma/bcmdrd/bcma_bcmdrdcmd_pbmp.h>

/*! Default local environment variable name for 'pbmp' execution result. */
#define BCMA_PBMP_RESULT_VAR "0"

int
bcma_bcmdrdcmd_pbmp(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    bcmdrd_pbmp_t pbmp;
    shr_pb_t *pb;

    if (BCMA_CLI_ARG_CNT(args) != 1) {
        return BCMA_CLI_CMD_USAGE;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (bcmdrd_pbmp_parse(arg, &pbmp) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    pb = shr_pb_create();
    if (!pb) {
        return BCMA_CLI_CMD_FAIL;
    }

    shr_pb_format_uint32(pb, NULL, pbmp.w, COUNTOF(pbmp.w), 0);
    bcma_cli_var_set(cli, BCMA_PBMP_RESULT_VAR, (char *)shr_pb_str(pb), TRUE);
    cli_out("%s\n", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}
