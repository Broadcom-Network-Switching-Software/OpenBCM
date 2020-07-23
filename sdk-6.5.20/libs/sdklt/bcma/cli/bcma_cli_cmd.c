/*! \file bcma_cli_cmd.c
 *
 * Main module of the CLI (Command Line Interpreter).
 *
 * This module contains the main command loop as well as lookup and
 * dispatch functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_redir.h>
#include <bcma/cli/bcma_cli_unit.h>
#include <bcma/cli/bcma_cli.h>

#include "cli_internal.h"

/*******************************************************************************
 * Local definitions
 */

/* User data for the Ctrl-C wrapper function for CLI command process. */
typedef struct cmd_process_ctrlc_data_s {
    bcma_cli_t *cli;
    bcma_cli_args_t *args;
    const char *str;
} cmd_process_ctrlc_data_t;

/*******************************************************************************
 * Local functions
 */

static bool
cli_cmd_exact_matched(const char *ref, const char *str, const char term)
{
    if (sal_strlen(ref) == sal_strlen(str)) {
        return true;
    }

    return bcma_cli_parse_abbr_cmp(ref, str, term);
}

static bool
cli_cmd_return_break(int rv)
{
    switch (rv) {
    case BCMA_CLI_CMD_USAGE:
    case BCMA_CLI_CMD_INTR:
    case BCMA_CLI_CMD_EXIT:
        return true;
    default:
        return false;
    }
    return false;
}

/*
 * Parse the unit prefix from the CLI command.
 * Accept formats are:
 * 1:<cmd>    - Specify one single device.
 * 0,3:<cmd>  - Specify different devices separated by commas.
 * 0-4:<cmd>  - Specify a range of devices separated by hyphens.
 * *:<cmd>    - Specify all valid devices by an asterisk.
 * Return value:
 * 0          - The unit prefix is successfully parsed.
 *              If the returned str remains unchanged,
 *              the unit prefix is not specified.
 * -1         - Failed to parse the unit prefix or
 *              the specified unit is invalid.
 */
static int
cli_cmd_unit_prefix(bcma_cli_t *cli, const char **str,
                    int max_units, SHR_BITDCL *ubit_array)
{
#if BCMA_CLI_CONFIG_UNIT_PREFIX == 1
    char ch;
    int cmd_unit;
    char buf[80];
    char *dst_s = buf, *dst_e = buf + sizeof(buf) - 1;
    const char *ptr = *str;

    do {
        ch = *ptr++;
        if (sal_isspace(ch) || ch == ';' || ch == '|' || ch == '\0') {
            break;
        }
        /* Command unit prefix is specified */
        if ((*dst_s = ch) == ':') {
            /* Remove the command unit prefix from the command */
            *dst_s = '\0';
            *str = ptr;

            SHR_BITCLR_RANGE(ubit_array, 0, max_units);

            if (buf[0] == '*' && buf[1] == '\0') {
                /* Specify all units */
                for (cmd_unit = 0; cmd_unit < max_units; cmd_unit++) {
                    if (bcma_cli_unit_valid(cli, cmd_unit)) {
                        SHR_BITSET(ubit_array, cmd_unit);
                    }
                }
            } else {
                if (bcma_cli_parse_bit_list(buf, max_units, ubit_array) < 0) {
                    cli_out("%sInvalid command unit syntax %s\n",
                            BCMA_CLI_CONFIG_ERROR_STR, buf);
                    return -1;
                }
                /* Check whether the specified units are all valid */
                SHR_BIT_ITER(ubit_array, max_units, cmd_unit) {
                    if (!bcma_cli_unit_valid(cli, cmd_unit)) {
                        cli_out("%sUnit %d is not valid\n",
                                BCMA_CLI_CONFIG_ERROR_STR, cmd_unit);
                        return -1;
                    }
                }
            }
            return 0;
        }
    } while (++dst_s <= dst_e);
#endif
    return 0;
}

/*
 * Parse and execute command line.
 */
static int
cli_cmd_dispatch(bcma_cli_t *cli, bcma_cli_args_t *args,
                 const char *str, const char **c_next)
{
    bcma_cli_command_t *clicmd;
    char *cmd;
    int rv = BCMA_CLI_CMD_USAGE;

    if (bcma_cli_ctrlc_break()) {
        return BCMA_CLI_CMD_INTR;
    }

    /* Parse command line */
    sal_memset(args, 0, sizeof(*args));
    if (bcma_cli_parse_args(cli, str, c_next, args) < 0) {
        return rv;
    }

    /* Empty command */
    if (BCMA_CLI_ARG_CNT(args) == 0) {
        /* Update the current CLI unit if nothing is specified */
        cli->cur_unit = cli->cmd_unit;
        /* Ignore empty line */
        return BCMA_CLI_CMD_OK;
    }

    cmd = args->argv[0];

    /* Lookup command and execute if found */
    switch (bcma_cli_cmd_lookup(cli, cmd, &clicmd)) {
    case BCMA_CLI_CMD_OK:
        args->argv[0] = clicmd->name;
        /* Skip command name when passing arguments to command handlers */
        BCMA_CLI_ARG_NEXT(args);
        rv = clicmd->func(cli, args);
        if (rv == BCMA_CLI_CMD_OK && BCMA_CLI_ARG_CUR(args) != NULL) {
            /*
             * Warn about unconsumed arguments.  If a command doesn't
             * need all its arguments, it should use BCMA_CLI_ARG_DISCARD to
             * eat them.
             */
            cli_out("%s: WARNING: excess arguments ignored "
                    "beginning with '%s'\n", clicmd->name, BCMA_CLI_ARG_CUR(args));
        }
        /* Indicate command complete to redirection handler */
        bcma_cli_redir_cmd_done(cli);
        break;
    case BCMA_CLI_CMD_AMBIGUOUS:
        cli_out("%sAmbiguous command\n", BCMA_CLI_CONFIG_ERROR_STR);
        bcma_cli_show_cmds_avail(cli, args->argv[0]);
        return BCMA_CLI_CMD_AMBIGUOUS;
    default:
        if (cli->cmd_except) {
            rv = cli->cmd_except(cli, args);
            if (rv != BCMA_CLI_CMD_NOT_FOUND) {
                break;
            }
        }
        cli_out("%sInvalid command\n", BCMA_CLI_CONFIG_ERROR_STR);
        bcma_cli_show_cmds_avail(cli, NULL);
        return BCMA_CLI_CMD_NOT_FOUND;
    }

    /* Print out common error messages */
    switch (rv) {
    case BCMA_CLI_CMD_NO_SYM:
        cli_out("%sChip symbol information not available\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        break;
    case BCMA_CLI_CMD_BAD_ARG:
        cli_out("%sBad argument or wrong number of arguments\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        break;
    case BCMA_CLI_CMD_USAGE:
        if (clicmd != NULL) {
            bcma_cli_cmd_usage_brief_show(cli, clicmd);
        }
        break;
    default:
        break;
    }

    return rv;
}

/*
 * Ctrl-C wrapper function for CLI command process
 */
static int
cli_cmd_process_ctrlc(void *data)
{
    cmd_process_ctrlc_data_t *cmd_data = (cmd_process_ctrlc_data_t *)data;
    bcma_cli_t *cli = cmd_data->cli;
    bcma_cli_args_t *args = cmd_data->args;
    const char *str = cmd_data->str;

    return bcma_cli_cmd_args_process(cli, str, args);
}

/*******************************************************************************
 * Public functions
 */

int
bcma_cli_cmd_enabled(bcma_cli_t *cli, int feature)
{
    if (feature == 0) {
        return TRUE;
    }
    return bcma_cli_unit_feature_enabled(cli, cli->cmd_unit, feature);
}

int
bcma_cli_gets(bcma_cli_t *cli, const char *prompt, int max, char *str)
{
    if (cli && cli->gets) {
        return cli->gets(cli, prompt, max, str);
    }

    return BCMA_CLI_CMD_FAIL;
}

void
bcma_cli_history_add(bcma_cli_t *cli, int max, char *str)
{
    if (cli && cli->history_add) {
        cli->history_add(max, str);
    }
}

int
bcma_cli_show_cmds_avail(bcma_cli_t *cli, char *prefix)
{
    bcma_cli_cmd_list_t *cl;
    bcma_cli_command_t *cmd;
    int cmds = 0;

    if (!cli || BAD_CLI(cli)) {
        return FALSE;
    }

    cli_out("Available commands:");
    cl = cli->command_root;
    while (cl) {
        cmd = cl->cmd;
        if (cmd == NULL) {
            /* Should never get here */
            continue;
        }
        if (!bcma_cli_cmd_enabled(cli, cl->feature)) {
            cl = cl->next;
            continue;
        }
        if (prefix && bcma_cli_parse_cmp(cmd->name, prefix, 0) == false) {
            cl = cl->next;
            continue;
        }
        if (cmds++) {
            cli_out(",");
        }
        cli_out(" %s", cmd->name);
        cl = cl->next;
    }
    cli_out("\n");

    return 0;
}

int
bcma_cli_cmd_lookup(bcma_cli_t *cli, const char *name,
                    bcma_cli_command_t **clicmd)
{
    bcma_cli_cmd_list_t *cl;
    bcma_cli_command_t *cmd;
    int rv = BCMA_CLI_CMD_NOT_FOUND;

    if (cli == NULL) {
        return BCMA_CLI_CMD_NOT_FOUND;
    }

    cl = cli->command_root;
    *clicmd = NULL;

    while (cl) {
        cmd = cl->cmd;
        if (cmd == NULL) {
            /* Should never get here */
            continue;
        }
        if (!bcma_cli_cmd_enabled(cli, cl->feature)) {
            cl = cl->next;
            continue;
        }
        if (bcma_cli_parse_cmp(cmd->name, name, 0)) {
            if (cli_cmd_exact_matched(cmd->name, name, 0)) {
                /* Exact match */
                *clicmd = cmd;
                rv = BCMA_CLI_CMD_OK;
                break;
            }
            rv = (*clicmd) ? BCMA_CLI_CMD_AMBIGUOUS : BCMA_CLI_CMD_OK;
            *clicmd = cmd;
        }
        cl = cl->next;
    }
    return rv;
}

int
bcma_cli_cmd_args_process(bcma_cli_t *cli, const char *str,
                          bcma_cli_args_t *args)
{
    const char *c_next, *cmd;
    int rv = BCMA_CLI_CMD_OK;
    int max_units = bcma_cli_unit_max(cli);
    SHR_BITDCLNAME(units_bitarray, max_units);

    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    c_next = str;
    while (c_next && *c_next && !cli_cmd_return_break(rv)) {
        cmd = c_next;
        if (cli_cmd_unit_prefix(cli, &cmd, max_units, units_bitarray) < 0) {
            /* Failed to parse the unit prefix */
            rv = BCMA_CLI_CMD_USAGE;
            bcma_cli_var_result_set(cli, rv);
            break;
        }
        if (cmd == c_next) {
            /* The unit prefix is not specified */
            rv = cli_cmd_dispatch(cli, args, cmd, &c_next);
            bcma_cli_var_result_set(cli, rv);
        } else {
            int cmd_unit;
            SHR_BIT_ITER(units_bitarray, max_units, cmd_unit) {
                /* Update the command unit */
                cli->cmd_unit = cmd_unit;

                rv = cli_cmd_dispatch(cli, args, cmd, &c_next);
                bcma_cli_var_result_set(cli, rv);
                if (cli_cmd_return_break(rv)) {
                    break;
                }
            }
        }
    }

    return rv;
}

int
bcma_cli_cmd_process(bcma_cli_t *cli, const char *str)
{
    int rv;
    bcma_cli_args_t *args;
    cmd_process_ctrlc_data_t ctrlc_data, *cmd_data = &ctrlc_data;

    args = sal_alloc(sizeof(*args), "bcmaCliCmdArgs");
    if (args == NULL) {
        cli_out("%sFailed to allocate command argument buffer\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    cmd_data->cli = cli;
    cmd_data->args = args;
    cmd_data->str = str;

    rv = bcma_cli_ctrlc_exec(cli, cli_cmd_process_ctrlc, cmd_data, 1);
    if (rv == BCMA_CLI_CMD_INTR) {
        /* The interrupt result may not yet set to the CLI result variable */
        bcma_cli_var_result_set(cli, rv);
        /* Reset the redirection status in case it was skipped by Ctrl-C */
        bcma_cli_redir_enable_set(cli, -1);
    }

    sal_free(args);

    return rv;
}

int
bcma_cli_add_command(bcma_cli_t *cli, bcma_cli_command_t *clicmd, int feature)
{
    bcma_cli_cmd_list_t *cl;

    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    if (clicmd == NULL) {
        bcma_cli_cmd_list_clear(cli);
        return 0;
    }

    /* Do not reinstall the same command structure */
    for (cl = cli->command_root; cl; cl = cl->next) {
        if (cl->cmd == clicmd) {
            /* Already installed */
            return 0;
        }
    }

    if (clicmd->synop == NULL) {
        clicmd->synop = "";
    }

    cl = bcma_cli_cmd_entry_create();
    if (cl == NULL) {
        return -1;
    }
    cl->cmd = clicmd;
    cl->feature = feature;
    cl->next = cli->command_root;
    cli->command_root = cl;

    return 0;
}

int
bcma_cli_eof_exit_set(bcma_cli_t *cli, bool enable)
{
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    cli->eof_exit = enable;

    return 0;
}

int
bcma_cli_cmd_exception_set(bcma_cli_t *cli, bcma_cli_cmd_except_f cmd_except)
{
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    cli->cmd_except = cmd_except;

    return 0;
}

int
bcma_cli_cmd_loop(bcma_cli_t *cli)
{
    int rv;
    char prompt[BCMA_CLI_CONFIG_IO_MAX_PROMPT];

    if (cli->gets == NULL) {
        return -1;
    }

    bcma_cli_ctrlc_enable_set(cli, 1);
    bcma_cli_var_scope_push(cli);
    bcma_cli_var_result_set(cli, 0);

    while (TRUE) {
        int unit = cli->cur_unit;

        /* Default command unit is the current CLI unit */
        cli->cmd_unit = unit;

        /* Generate the prompt with unit suffix */
        if (unit < 0) {
            sal_snprintf(prompt, sizeof(prompt)-1,
                         "%s> ", cli->prompt);
        } else {
            sal_snprintf(prompt, sizeof(prompt)-1,
                         "%s%s.%d> ",
                         bcma_cli_prompt_prefix(cli),
                         cli->prompt, unit);
        }

        /* Read a command */
        rv = bcma_cli_gets(cli, prompt, sizeof(cli->ibuf), cli->ibuf);
        if (rv == BCMA_CLI_CMD_EXIT) {
            if (cli->eof_exit) {
                cli_out("exit\n");
                break;
            }
            cli_out("\n");
        }
        if (rv != BCMA_CLI_CMD_OK) {
            continue;
        }

        /* Add command to CLI history */
        bcma_cli_history_add(cli, sizeof(cli->ibuf), cli->ibuf);

        /* Execute the command */
        if (bcma_cli_cmd_process(cli, cli->ibuf) == BCMA_CLI_CMD_EXIT) {
            break;
        }
    }

    bcma_cli_var_scope_pop(cli);
    bcma_cli_ctrlc_enable_set(cli, 0);

    return 0;
}
