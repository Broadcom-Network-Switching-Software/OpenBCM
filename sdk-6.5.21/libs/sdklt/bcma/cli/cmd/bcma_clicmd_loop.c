/*! \file bcma_clicmd_loop.c
 *
 * CLI 'loop' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>

#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_loop.h>

/*! Context for Ctrl-C wrapper function loop_ctrlc. */
typedef struct loop_info_s {

    /* CLI object */
    bcma_cli_t *cli;

    /* CLI command arguments */
    bcma_cli_args_t *args;

    /* Number of loop times to be executed, -1 means forever */
    int num;

    /* Number of current loop time */
    int cnt;

    /* Loop is interrupted due to error returns */
    int err;

    /* Auguments for the commands executed from the 'loop' command */
    bcma_cli_args_t *cmd_args;

} loop_info_t;

/*
 * Function to process "loop" command.
 */
static int
do_loop_ctrlc(void *data)
{
    loop_info_t *lpinfo = (loop_info_t *)data;
    bcma_cli_t *cli = lpinfo->cli;
    bcma_cli_args_t *args = lpinfo->args;
    int lpforever = (lpinfo->num == -1) ? TRUE : FALSE;
    int rv = 0;
    int lperr_stop = bcma_cli_var_bool_get(cli, BCMA_CLICMD_LOOPERROR, TRUE);
    int arg_start;
    const char *c;

    /* Save the start command */
    arg_start = args->arg;

    /* Execute command in loop. */
    for (lpinfo->cnt = 0; lpforever || lpinfo->cnt < lpinfo->num;
         lpinfo->cnt++) {
        /* Restore command aguments to the start command. */
        args->arg = arg_start;

        /* Execute the command list in order. */
        while ((c = BCMA_CLI_ARG_GET(args)) != NULL) {
            rv = bcma_cli_cmd_args_process(cli, c, lpinfo->cmd_args);
            if (rv == BCMA_CLI_CMD_INTR) {
                return rv;
            }
            if (rv != BCMA_CLI_CMD_OK && lperr_stop) {
                lpinfo->err = 1;
                return rv;
            }
        }
    }

    return rv;
}

int
bcma_clicmd_loop(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = 0;
    int lpnum, lpforever = FALSE;
    const char *a;
    loop_info_t loop_info, *lpinfo = &loop_info;
    bcma_cli_args_t *cmd_args;

    if (BCMA_CLI_ARG_CNT(args) < 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    /*
     * Need a new CLI command arguments for processing the commands within
     * the 'loop' command arguments. The 'loop' command arguements can not
     * be reused in case the parsed arguments are messed up.
     */
    cmd_args = sal_alloc(sizeof(*cmd_args), "bcmaCliLoopCmdArgs");
    if (cmd_args == NULL) {
        cli_out("%sFailed to allocate memory for loop_cmd arguments\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    a = BCMA_CLI_ARG_GET(args);
    if (sal_strcmp(a, "*") == 0) {
        lpforever = TRUE;
    } else {
        if (bcma_cli_parse_int(a, &lpnum) < 0 || lpnum < 0) {
            cli_out("%s%s: Invalid loop count: %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args), a);
            sal_free(cmd_args);
            return BCMA_CLI_CMD_FAIL;
        }
    }

    sal_memset(lpinfo, 0, sizeof(*lpinfo));
    lpinfo->cli = cli;
    lpinfo->args = args;
    lpinfo->num = lpforever ? -1 : lpnum;
    lpinfo->cmd_args = cmd_args;

    rv = bcma_cli_ctrlc_exec(cli, do_loop_ctrlc, lpinfo, 1);

    sal_free(cmd_args);

    /* Display warning message if loop is interrupted by ctrl-c or error return. */
    if (lpinfo->err || rv == BCMA_CLI_CMD_INTR) {
        cli_out("%s: Warning: Looping aborted on the %dth loop\n",
                BCMA_CLI_ARG_CMD(args), lpinfo->cnt + 1);

        /* Discard the reset arguments when loop was interrupted. */
        BCMA_CLI_ARG_DISCARD(args);
    }

    return rv;
}
