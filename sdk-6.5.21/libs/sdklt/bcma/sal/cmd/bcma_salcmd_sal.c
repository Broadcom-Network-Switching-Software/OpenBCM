/*! \file bcma_salcmd_sal.c
 *
 * CLI 'list' command for CMICd
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/sal/bcma_sal_alloc_debug.h>
#include <bcma/sal/bcma_salcmd_sal.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_SHELL

static int
cmd_alloc(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *cmd;
    const char *prefix;

    cmd = BCMA_CLI_ARG_GET(args);
    if (!cmd) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcmp(cmd, "trace") == 0) {
        prefix = BCMA_CLI_ARG_GET(args);
        if (!prefix) {
            return BCMA_CLI_CMD_USAGE;
        }
        if (*prefix == '-') {
            prefix = "";
        }
        bcma_sal_alloc_debug_trace_prefix_set(prefix);
    } else if (sal_strcmp(cmd, "notrace") == 0) {
        prefix = BCMA_CLI_ARG_GET(args);
        if (!prefix) {
            return BCMA_CLI_CMD_USAGE;
        }
        if (*prefix == '-') {
            prefix = "";
        }
        bcma_sal_alloc_debug_notrace_prefix_set(prefix);
    } else if (sal_strcmp(cmd, "track") == 0) {
        prefix = BCMA_CLI_ARG_GET(args);
        if (!prefix) {
            return BCMA_CLI_CMD_USAGE;
        }
        if (*prefix == '-') {
            prefix = "";
        }
        bcma_sal_alloc_debug_track_prefix_set(prefix);
    } else if (sal_strcmp(cmd, "status") == 0) {
        bcma_sal_alloc_debug_status();
    } else if (sal_strcmp(cmd, "dump") == 0) {
        bcma_sal_alloc_debug_dump();
    } else if (sal_strcmp(cmd, "fill") == 0) {
        bcma_sal_alloc_debug_fill_enable_set(true);
    } else if (sal_strcmp(cmd, "nofill") == 0) {
        bcma_sal_alloc_debug_fill_enable_set(false);
    } else {
        cli_out("%sunknown or unsupported command: '%s'\n",
                BCMA_CLI_CONFIG_ERROR_STR, cmd);
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_salcmd_sal(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_FAIL;
    const char *api;

    api = BCMA_CLI_ARG_GET(args);
    if (!api) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcmp(api, "alloc") == 0) {
        rv = cmd_alloc(cli, args);
    } else {
        cli_out("%sunknown or unsupported API: '%s'\n",
                BCMA_CLI_CONFIG_ERROR_STR, api);
    }

    return rv;
}
