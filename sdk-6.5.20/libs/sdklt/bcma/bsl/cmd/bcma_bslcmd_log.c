/*! \file bcma_bslcmd_log.c
 *
 * CLI BSL shell commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bsl/bcma_bslfile.h>

#include <bcma/bsl/bcma_bslcmd_log.h>

#ifndef BCMA_BSL_CONFIG_DEFAULT_LOG_FILE
#define BCMA_BSL_CONFIG_DEFAULT_LOG_FILE "bcm.log"
#endif

static int
log_on(char *log_file, int append, int quiet)
{
    bool new_file = false;
    char *cur_file = bcma_bslfile_name();

    if (!append || !cur_file || !bcma_bslfile_is_enabled() ||
        sal_strcmp(log_file, cur_file) != 0) {
        if (bcma_bslfile_open(log_file, append) < 0) {
            cli_out("%sCould not start logging\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }
        new_file = true;
    }

    if (bcma_bslfile_enable(1) < 0) {
        cli_out("%sCould not start logging\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!quiet) {
        cli_out("File logging %s to %s\n",
                new_file ? "started" : "continued",
                log_file);
    }

    return BCMA_CLI_CMD_OK;
}

static int
log_off(char *log_file, int quiet)
{
    if (log_file == NULL || !bcma_bslfile_is_enabled()) {
        cli_out("File logging is not active.\n");
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_bslfile_enable(0) < 0) {
        cli_out("%sCould not stop logging to %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, log_file);
        return BCMA_CLI_CMD_FAIL;
    }

    if (!quiet) {
        cli_out("File logging to %s stopped.\n", log_file);
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_bslcmd_log(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    char *file;
    int append = 1;
    int quiet = 0;
    const char *arg;
    bcma_cli_parse_table_t pt;

    if ((file = bcma_bslfile_name()) == NULL) {
        file = BCMA_BSL_CONFIG_DEFAULT_LOG_FILE;
    }

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        cli_out("Logging to file %s: %s\n",
                file,
                bcma_bslfile_is_enabled() ? "enabled" : "disabled");
        return BCMA_CLI_CMD_OK;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "File", "str", &file, NULL);
    bcma_cli_parse_table_add(&pt, "Append", "bool", &append, NULL);
    bcma_cli_parse_table_add(&pt, "Quiet", "bool", &quiet, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        /* Default is to turn logging on */
        arg = "on";
    }

    if (sal_strcasecmp("on", arg) == 0) {
        rv = log_on(file, append, quiet);
    } else if (sal_strcasecmp("off", arg) == 0) {
        rv = log_off(file, quiet);
    } else {
        rv = BCMA_CLI_CMD_USAGE;
    }

    bcma_cli_parse_table_done(&pt);

    return rv;
}
