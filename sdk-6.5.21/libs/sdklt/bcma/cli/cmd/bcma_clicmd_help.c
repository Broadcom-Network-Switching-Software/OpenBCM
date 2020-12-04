/*! \file bcma_clicmd_help.c
 *
 * CLI 'help' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_help.h>

int
bcma_clicmd_help(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_cli_cmd_list_t *cl;
    bcma_cli_command_t *cmd;

    if (args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        cli_out("\nSummary of commands:\n\n");
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
            cli_out("%-15s  %s\n", cmd->name, cmd->desc);
            cl = cl->next;
        }
        cli_out("\nFor more information about a command, "
                "enter \"help <command-name>\"\n\n");
        return BCMA_CLI_CMD_OK;
    }

    switch (bcma_cli_cmd_lookup(cli, BCMA_CLI_ARG_GET(args), &cmd)) {
    case BCMA_CLI_CMD_OK:
        break;
    case BCMA_CLI_CMD_AMBIGUOUS:
        cli_out("%sCommand not unique\n", BCMA_CLI_CONFIG_ERROR_STR);
        bcma_cli_show_cmds_avail(cli, args->argv[0]);
        return BCMA_CLI_CMD_FAIL;
    default:
        cli_out("%sUnknown command\n", BCMA_CLI_CONFIG_ERROR_STR);
        bcma_cli_show_cmds_avail(cli, NULL);
        return BCMA_CLI_CMD_FAIL;
    }

    return (bcma_cli_cmd_usage_show(cli, cmd) == 0) ? BCMA_CLI_CMD_OK :
                                                      BCMA_CLI_CMD_FAIL;
}
