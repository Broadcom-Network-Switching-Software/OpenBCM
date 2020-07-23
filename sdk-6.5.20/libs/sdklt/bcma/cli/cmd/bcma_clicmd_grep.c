/*! \file bcma_clicmd_grep.c
 *
 * CLI 'grep' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/io/bcma_io_file.h>
#include <bcma/io/bcma_io_shell.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_redir.h>

#include <bcma/cli/bcma_clicmd_grep.h>

#define GREP_CMD_SIZE_MAX       128

int
bcma_clicmd_grep(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    char cmd[GREP_CMD_SIZE_MAX];
    char *buf = cmd;
    int bufsz = GREP_CMD_SIZE_MAX;
    int len;
    int num_expr;
    const char *ifile = bcma_cli_redir_input_name_get(cli);
    const char *ofile = bcma_cli_redir_output_name_get(cli);
    bcma_io_file_handle_t fh;

    if (ifile == NULL || ofile == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Test whether the redirection input file exists. */
    fh = bcma_io_file_open(ifile, "r");
    if (fh == NULL) {
        return BCMA_CLI_CMD_BAD_ARG;
    }
    bcma_io_file_close(fh);

    /* Base command */
    len = sal_snprintf(buf, bufsz, "%s", "grep -a ");
    buf += len;
    bufsz -= len;
    if (len < 0 || bufsz <= 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Build system grep command from CLI arguments */
    num_expr = 0;
    for (arg = BCMA_CLI_ARG_GET(args); arg; arg = BCMA_CLI_ARG_GET(args)) {
        if (arg[0] == '-') {
            /* Add (unquoted) option to command */
            len = sal_snprintf(buf, bufsz, "%s ", arg);
        } else {
            ++num_expr;
            /* Add quoted expression to command */
            len = sal_snprintf(buf, bufsz, "'%s' ", arg);
        }
        buf += len;
        bufsz -= len;
        if (len < 0 || bufsz <= 0) {
            return BCMA_CLI_CMD_FAIL;
        }
    }

    if (num_expr != 1) {
        /* Missing expression or multiple expressions */
        return BCMA_CLI_CMD_BAD_ARG;
    }

    bcma_io_shell(cmd, ifile, ofile);

    return BCMA_CLI_CMD_OK;
}
