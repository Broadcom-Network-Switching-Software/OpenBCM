/*! \file bcma_clicmd_sort.c
 *
 * CLI 'sort' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bcma/io/bcma_io_file.h>
#include <bcma/io/bcma_io_shell.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_redir.h>
#include <bcma/cli/bcma_clicmd_sort.h>

#define SORT_CMD_SIZE_MAX     128

int
bcma_clicmd_sort(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    char cmd[SORT_CMD_SIZE_MAX];
    char *buf = cmd;
    int  bufsz = SORT_CMD_SIZE_MAX;
    int  len;
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
    len = sal_snprintf(buf, bufsz, "%s", "sort ");
    buf += len;
    bufsz -= len;
    if (len < 0 || bufsz <= 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Build system sort command from CLI arguments */
    for (arg = BCMA_CLI_ARG_GET(args); arg; arg = BCMA_CLI_ARG_GET(args)) {
        if (arg[0] == '-') {
            /* Add option to command */
            len = sal_snprintf(buf, bufsz, "%s ", arg);
        } else {
            /* No other arguments except "option" */
            return BCMA_CLI_CMD_BAD_ARG;
        }
        buf += len;
        bufsz -= len;
        if (len < 0 || bufsz <= 0) {
            return BCMA_CLI_CMD_FAIL;
        }
    }
    bcma_io_shell(cmd, ifile, ofile);

    return BCMA_CLI_CMD_OK;
}

