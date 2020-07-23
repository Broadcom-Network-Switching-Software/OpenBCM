/*! \file bcma_bcmmgmtcmd_config.c
 *
 * CLI command for configuration management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_error.h>
#include <shr/shr_pb.h>
#include <shr/shr_pb_format.h>

#include <bcmbd/bcmbd_config.h>

#include <bcmlrd/bcmlrd_init.h>
#include <bcmcfg/bcmcfg_init.h>

#include <bcmmgmt/bcmmgmt.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmmgmt/bcma_bcmmgmt.h>
#include <bcma/bcmmgmt/bcma_bcmmgmtcmd_config.h>

static int
bcmmgmtcmd_config_load(int unit)
{
    int rv = SHR_E_NONE;
    bool warm = false;

    if (bcmcfg_ready(unit)) {
        cli_out("%sConfiguration already loaded\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return SHR_E_FAIL;
    }

    rv = bcmlrd_init();
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rv = bcma_bcmmgmt_config_load(false);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rv = bcmcfg_init(unit, warm);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rv = bcmcfg_config(unit, warm);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    return rv;
}

static int
bcmmgmtcmd_config_get(int unit, char *var)
{
    shr_pb_t *pb;
    char *table, *field;
    uint64_t val64;

    if (var == NULL) {
        cli_out("%sMissing configuration parameter\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return SHR_E_PARAM;
    }

    if ((field = sal_strchr(var, '.')) == NULL) {
        cli_out("%sMalformed configuration variable: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, var);
        return SHR_E_PARAM;
    }

    table = var;
    *field++ = 0;

    val64 = bcmbd_config64_get(unit, 0, table, field);

    pb = shr_pb_create();
    cli_out("%s.%s=%s(%"PRIu64")\n", table, field,
            shr_pb_format_uint64(pb, NULL, &val64, 1, 0), val64);
    shr_pb_destroy(pb);

    return SHR_E_NONE;
}

static int
bcmmgmtcmd_config_status(int unit)
{
    cli_out("Configuration%s loaded\n",
            bcmcfg_ready(unit) ? "" : " not");

    return SHR_E_NONE;
}

int
bcma_bcmmgmtcmd_config(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    char *cmd, *var;

    unit = cli->cmd_unit;

    cmd = BCMA_CLI_ARG_GET(args);
    if (cmd == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("load", cmd, 0)) {
        if (SHR_FAILURE(bcmmgmtcmd_config_load(unit))) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else if (bcma_cli_parse_cmp("get", cmd, 0)) {
        var = BCMA_CLI_ARG_GET(args);
        if (SHR_FAILURE(bcmmgmtcmd_config_get(unit, var))) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else if (bcma_cli_parse_cmp("status", cmd, 0)) {
        bcmmgmtcmd_config_status(unit);
    } else {
        return BCMA_CLI_CMD_USAGE;
    }

    return BCMA_CLI_CMD_OK;
}
