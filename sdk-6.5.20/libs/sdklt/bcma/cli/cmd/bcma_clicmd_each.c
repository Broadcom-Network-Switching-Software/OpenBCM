/*! \file bcma_clicmd_each.c
 *
 * CLI 'each' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <shr/shr_pb.h>

#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>

#include <bcma/cli/bcma_clicmd.h>
#include <bcma/cli/bcma_clicmd_each.h>

/*
 * Cookie for Ctrl-C wrapper function do_each_ctrlc.
 */
typedef struct each_info_s {

    /* CLI object. */
    bcma_cli_t *cli;

    /* CLI command arguments. */
    bcma_cli_args_t *args;

    /* Loop is interrupted due to error returns. */
    int err;

    /* Arguments for the command executed in command 'each'. */
    bcma_cli_args_t cmd_args;

    /* Variable names */
    bcma_cli_tokens_t ctok_vars;

    /* List of variable values */
    bcma_cli_tokens_t ctok_vlist;

    /* Current variable values */
    bcma_cli_tokens_t ctok_vals;

} each_info_t;

/*
 * Tokenize a list of elements separated by commas.
 */
static int
tokenize_list(const char *str, bcma_cli_tokens_t *tok)
{
    return bcma_cli_tokenize(str, tok, ",");
}

/*
 * Tokenize a list element, which may be a scalar or a {:} tuple,
 * e.g. {red:blue:green}.
 */
static int
tokenize_element(const char *str, bcma_cli_tokens_t *tok)
{
    const char *delim = "{}:";

    if (bcma_cli_tokenize(str, tok, delim) < 0) {
        return -1;
    }
    if (tok->argc > 1 || sal_strchr(delim, *str)) {
        /* Extra syntax checks if tuple */
        if (*str != '{' || sal_strchr(str + 1, '{') != NULL) {
            return -1;
        }
        if (sal_strchr(str, '}') != (str + sal_strlen(str) - 1)) {
            return -1;
        }
    }
    return 0;
}

/*
 * Format content in bcma_cli_tokens_t structure in 'each' command.
 */
static const char *
format_element(each_info_t *ei, shr_pb_t * pb, bcma_cli_tokens_t *ctok)
{
    int idx;

    if (ctok == NULL) {
        return "";
    }

    if (ctok->argc > 1) {
        shr_pb_printf(pb, "{");
    }
    for (idx = 0; idx < ctok->argc; idx++) {
        shr_pb_printf(pb, "%s%s", idx > 0 ? ":" : "", ctok->argv[idx]);
    }
    if (ctok->argc > 1) {
        shr_pb_printf(pb, "}");
    }
    return shr_pb_str(pb);
}

/*
 * Function to prcoess "each" command.
 */
static int
do_each_ctrlc(void *data)
{
    int rv = BCMA_CLI_CMD_OK;
    each_info_t *ei = (each_info_t *)data;
    bcma_cli_t *cli = ei->cli;
    bcma_cli_args_t *args = ei->args;
    char *arg, *ptr;
    const char *cmd;
    bcma_cli_tokens_t *vars = &ei->ctok_vars;
    bcma_cli_tokens_t *vlist = &ei->ctok_vlist;
    bcma_cli_tokens_t *vals = &ei->ctok_vals;
    int lperr_stop = bcma_cli_var_bool_get(cli, BCMA_CLICMD_LOOPERROR, TRUE);
    int vdx, idx, cmd_cnt;

    arg = BCMA_CLI_ARG_GET(args);
    ptr = sal_strchr(arg, '=');
    if (ptr == NULL || ptr == arg || *(ptr + 1) == '\0') {
        return BCMA_CLI_CMD_USAGE;
    }
    *ptr++ = '\0';

    /* Create list of variable names */
    if (tokenize_element(arg, vars) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Create list of variable value elements */
    if (tokenize_list(ptr, vlist) < 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Execute commands for each element */
    cmd_cnt = 0;
    for (vdx = 0; vdx < vlist->argc; vdx++) {
        /* Tokenize the group of value(s) */
        if (tokenize_element(vlist->argv[vdx], vals) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        /* Sanity check for the count for variables and values*/
        if (vars->argc != vals->argc) {
            shr_pb_t *pb = shr_pb_create();
            shr_pb_printf(pb, "%sUnmatched element ",
                          BCMA_CLI_CONFIG_ERROR_STR);
            format_element(ei, pb, vals);
            cli_out("%s\n", shr_pb_str(pb));
            shr_pb_destroy(pb);
            return BCMA_CLI_CMD_FAIL;
        }
        /* Update the environment variable(s) */
        for (idx = 0; idx < vals->argc; idx++) {
            rv = bcma_cli_var_set(cli, vars->argv[idx],
                                  vals->argv[idx], TRUE);
            if (rv < 0) {
                cli_out("%sFailed to set variable %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, vars->argv[idx]);
                return BCMA_CLI_CMD_FAIL;
            }
        }

        /* Back up command list from previous iteration */
        while (cmd_cnt) {
            BCMA_CLI_ARG_PREV(args);
            cmd_cnt--;
        }

        /* Execute the command list in order. */
        while ((cmd = BCMA_CLI_ARG_GET(args)) != NULL) {
            rv = bcma_cli_cmd_args_process(cli, cmd, &ei->cmd_args);
            if (rv == BCMA_CLI_CMD_INTR) {
                return rv;
            }
            if (rv != BCMA_CLI_CMD_OK && lperr_stop) {
                ei->err = 1;
                break;
            }
            cmd_cnt++;
        }
        if (ei->err == 1) {
            break;
        }
    }
    return rv;
}

int
bcma_clicmd_each(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = 0;
    int idx;
    each_info_t *ei;

    if (BCMA_CLI_ARG_CNT(args) < 2) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Allocate work area */
    ei = sal_alloc(sizeof(*ei), "bcmaCliEachCmdInfo");
    if (ei == NULL) {
        cli_out("%sFailed to allocate memory for each_cmd work data\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(ei, 0, sizeof(*ei));
    ei->cli = cli;
    ei->args = args;

    rv = bcma_cli_ctrlc_exec(cli, do_each_ctrlc, ei, 1);

    /*
     * Display warning message if the loop is interrupted by Ctrl-C or
     * error return.
     */
    if (ei->err || rv == BCMA_CLI_CMD_INTR) {
        shr_pb_t *pb = shr_pb_create();
        shr_pb_printf(pb, "%s: Warning: Looping aborted on ",
                      BCMA_CLI_ARG_CMD(args));
        format_element(ei, pb, &ei->ctok_vars);
        shr_pb_printf(pb, "=");
        format_element(ei, pb, &ei->ctok_vals);
        cli_out("%s\n", shr_pb_str(pb));
        shr_pb_destroy(pb);

        /* Discard arguments interrupted in loop. */
        BCMA_CLI_ARG_DISCARD(args);
    }

    /* Delete temporary variables */
    for (idx = 0; idx < ei->ctok_vars.argc; idx++) {
        bcma_cli_var_set(cli, ei->ctok_vars.argv[idx], NULL, TRUE);
    }

    sal_free(ei);

    return rv;
}
