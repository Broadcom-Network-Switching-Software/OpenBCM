/*! \file bcma_clicmd_for.c
 *
 * CLI 'for' command.
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
#include <bcma/cli/bcma_clicmd_for.h>

#define VAR_DEFAULT_FMT "%d"

/*
 * Cookie for Ctrl-C wrapper function for_ctrlc.
 */
typedef struct for_info_s {

    /* CLI object */
    bcma_cli_t *cli;

    /* CLI command arguments */
    bcma_cli_args_t *args;

    /* Variable name in for-loop */
    const char *var_name;

    /* Variable value format */
    const char *var_fmt;

    /* Current variable value in for-loop */
    int cur_idx;

    /* Loop is interrupted due to error returns */
    int err;

    /* Arguments for the command executed in the 'for' command */
    bcma_cli_args_t *cmd_args;

} for_info_t;

/*
 * Function to prcoess "for" command.
 */
static int
do_for_ctrlc(void *data)
{
    for_info_t *finfo = (for_info_t *)data;
    bcma_cli_t *cli = finfo->cli;
    bcma_cli_args_t *args = finfo->args;
    const char *c;
    char *s2 = NULL, *var_name;
    char *s_start, *s_stop, *s_step;
    char *fmt;
    int start, stop, step;
    int arg_start;
    char val[80];
    int rv = 0;
    int lperr_stop = bcma_cli_var_bool_get(cli, BCMA_CLICMD_LOOPERROR, TRUE);

    c = BCMA_CLI_ARG_GET(args);

    var_name = sal_strtok_r((char *)c, "=", &s2);
    s_start = sal_strtok_r(NULL, ",", &s2);
    s_stop = sal_strtok_r(NULL, ",", &s2);
    s_step = sal_strtok_r(NULL, ",", &s2);
    fmt = sal_strtok_r(NULL, "\0", &s2);

    /* Check for necessary arguments */
    if (var_name == NULL || s_start == NULL || s_stop == NULL) {
        cli_out("%s%s: Invalid loop format\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));
        return BCMA_CLI_CMD_FAIL;
    }

    /* Check arguments format */
    if (bcma_cli_parse_int(s_start, &start) < 0 ||
        bcma_cli_parse_int(s_stop, &stop) < 0) {
        cli_out("%s%s: Invalid loop format\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));
        return BCMA_CLI_CMD_FAIL;
    }

    if (s_step) {
        if (bcma_cli_parse_int(s_step, &step) < 0 || step == 0) {
            cli_out("%s%s: Invalid loop format\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CMD(args));
            return BCMA_CLI_CMD_FAIL;
        }
    } else {
        /* Default step value */
        step = 1;
    }

    finfo->var_name = var_name;
    finfo->var_fmt = fmt;

    if (fmt == NULL) {
        /* Default format */
        fmt = VAR_DEFAULT_FMT;
    }

    /* Save the start command */
    arg_start = args->arg;

    /* Execute commands in for loop. */
    for (finfo->cur_idx = start;
         (step > 0 && finfo->cur_idx <= stop) ||
         (step < 0 && finfo->cur_idx >= stop);
         finfo->cur_idx += step) {
        sal_snprintf(val, sizeof(val), fmt, finfo->cur_idx, "", "", "");
        bcma_cli_var_set(cli, var_name, val, TRUE);

        /* Restore command arguments to the start command. */
        args->arg = arg_start;

        /* Execute the command list in order. */
        while ((c = BCMA_CLI_ARG_GET(args)) != NULL) {
            rv = bcma_cli_cmd_args_process(cli, c, finfo->cmd_args);
            if (rv == BCMA_CLI_CMD_INTR) {
                return rv;
            }
            if (rv != BCMA_CLI_CMD_OK && lperr_stop) {
                finfo->err = 1;
                return rv;
            }
        }
    }

    return rv;
}

int
bcma_clicmd_for(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = 0;
    for_info_t for_info, *finfo = &for_info;
    bcma_cli_args_t *cmd_args;

    if (BCMA_CLI_ARG_CNT(args) < 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    /*
     * Need a new CLI command arguments for processing the commands within
     * the 'for' command arguments. The 'for' command arguements can not
     * be reused in case the parsed arguments are messed up.
     */
    cmd_args = sal_alloc(sizeof(*cmd_args), "bcmaCliForCmdArgs");
    if (cmd_args == NULL) {
        cli_out("%sFailed to allocate memory for for_cmd arguments\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    sal_memset(finfo, 0, sizeof(*finfo));
    finfo->cli = cli;
    finfo->args = args;
    finfo->cmd_args = cmd_args;

    rv = bcma_cli_ctrlc_exec(cli, do_for_ctrlc, finfo, 1);

    sal_free(cmd_args);

    if (finfo->var_name == NULL) {
        return rv;
    }

    /* Display warning message if loop is interrupted by ctrl-c or error return. */
    if (finfo->err || rv == BCMA_CLI_CMD_INTR) {
        if (finfo->var_fmt == NULL) {
            finfo->var_fmt = VAR_DEFAULT_FMT;
        }
        cli_out("%s: Warning: Looping aborted on %s=",
                BCMA_CLI_ARG_CMD(args), finfo->var_name);
        cli_out(finfo->var_fmt, finfo->cur_idx, "", "", "");
        cli_out("\n");

        /* Discard arguments interrupted in loop. */
        BCMA_CLI_ARG_DISCARD(args);
    }

    bcma_cli_var_set(cli, finfo->var_name, NULL, TRUE);

    return rv;
}
