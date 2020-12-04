/*! \file bcma_clicmd_sleep.c
 *
 * CLI 'sleep' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/cli/bcma_clicmd_sleep.h>

int
bcma_clicmd_sleep(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *a;
    int quiet = 0, sec = 1, usec = 0;

    a = BCMA_CLI_ARG_GET(args);

    if (a && sal_strcasecmp(a, "quiet") == 0) {
        quiet = 1;
        a = BCMA_CLI_ARG_GET(args);
    }

    if (a) {
        if (bcma_cli_parse_int(a, &sec) < 0 || sec < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        a = BCMA_CLI_ARG_GET(args);
    }

    if (a) {
        if (bcma_cli_parse_int(a, &usec) < 0 || usec < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
    }

    if (BCMA_CLI_ARG_CNT(args) > 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (!quiet) {
        if (usec == 0) {
            cli_out("Sleeping for %d second%s\n", sec, sec > 1 ? "s" : "");
        } else {
            cli_out("Sleeping for %d.%06d seconds\n",
                    sec + usec / 1000000, usec % 1000000);
        }
    }

    if (sec) {
        sal_sleep(sec);
    }

    if (usec) {
        sal_usleep(usec);
    }

    /*
     * The Ctrl-C signal is handled in sal_sleep/sal_usleep, but we can still
     * using bcma_cli_ctrlc_break() to check whether the command is interrupted
     * by Ctrl-C.
     */
    return bcma_cli_ctrlc_break() ? BCMA_CLI_CMD_INTR : BCMA_CLI_CMD_OK;
}
