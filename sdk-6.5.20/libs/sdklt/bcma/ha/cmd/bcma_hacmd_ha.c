/*! \file bcma_hacmd_ha.c
 *
 * CLI command to display HA memory usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/ha/bcma_ha.h>
#include <bcma/ha/bcma_ha_mem_usage.h>

#include <bcma/ha/bcma_hacmd_ha.h>

static int
cmd_alloc_status(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int unit = cli->cmd_unit;
    shr_pb_t *pb;
    bool gen_mem_inc = true;
    bool fmt_hdr = true;

    pb = shr_pb_create();

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        /* Format the memory usage of all components */
        bcma_ha_mem_usage(unit, gen_mem_inc, pb, fmt_hdr, false);
    } else {
        /* Format the detailed component memory usage */
        if (arg[0] == '*' && arg[1] == '\0') {
            /* Display for all components */
            bcma_ha_mem_usage(unit, gen_mem_inc, pb, fmt_hdr, true);
        } else {
            /* Display for the specified components */
            do {
                bcma_ha_comp_mem_usage(unit, arg, gen_mem_inc, pb, fmt_hdr);
                fmt_hdr = false;
            } while ((arg = BCMA_CLI_ARG_GET(args)) != NULL);
        }
    }
    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return  BCMA_CLI_CMD_OK;
}

static int
cmd_alloc(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp(arg, "status") == 0) {
        return cmd_alloc_status(cli, args);
    }

    return BCMA_CLI_CMD_USAGE;
}

static int
cmd_file(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int rv;
    int unit = cli->cmd_unit;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp(arg, "info") == 0) {
        char buf[80];
        /* Display the HA file location */
        rv = bcma_ha_mem_name_get(unit, sizeof(buf), buf);
        if (SHR_SUCCESS(rv)) {
            cli_out("HA file: %s\n", buf);
        } else {
            cli_out("%sFailed to get HA memory name (%s)\n",
                    BCMA_CLI_CONFIG_ERROR_STR, shr_errmsg(rv));
        }
        return BCMA_CLI_CMD_OK;
    }

    return BCMA_CLI_CMD_USAGE;
}

int
bcma_hacmd_ha(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp(arg, "alloc") == 0) {
        return cmd_alloc(cli, args);
    } else if (sal_strcasecmp(arg, "file") == 0) {
        return cmd_file(cli, args);
    }

    return BCMA_CLI_CMD_USAGE;
}
