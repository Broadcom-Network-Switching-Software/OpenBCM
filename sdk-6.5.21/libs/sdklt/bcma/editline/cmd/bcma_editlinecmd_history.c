/*! \file bcma_editlinecmd_history.c
 *
 * Implementation the CLI "history" command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/editline/bcma_readline.h>
#include <bcma/editline/bcma_editlinecmd_history.h>

int
bcma_editlinecmd_history(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    char *line;
    int idx = 0;
    int hist_len, cnt;

    hist_len = bcma_editline_history_length();
    cnt = hist_len;

    if ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
        if (bcma_cli_parse_int(arg, &cnt) != 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        if (cnt < 0) {
            cli_out("%sInvalid count: %d\n", BCMA_CLI_CONFIG_ERROR_STR, cnt);
            return BCMA_CLI_CMD_FAIL;
        }
    }

    if (cnt < hist_len) {
        idx = hist_len - cnt;
    }
    idx += history_base;

    while ((line = bcma_editline_history_get(idx)) != NULL) {
        cli_out("%5d  %s\n", idx, line);
        idx++;
    }
    return BCMA_CLI_CMD_OK;
}
