/*! \file bcma_cli_init.c
 *
 * Functions to initialize and cleanup a CLI object.
 *
 * These functions are independent of the object create/destroy
 * functions in order to allow the CLI object to be declared as static
 * data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli.h>

#include "cli_internal.h"

int
bcma_cli_init(bcma_cli_t *cli)
{
    if (cli == NULL) {
        return -1;
    }

    sal_memset(cli, 0, sizeof(*cli));

    /* Initialize object signature for sanity checks */
    cli->cli_sig = CLI_SIGNATURE;

    /* Mark current unit as invalid by default */
    cli->cur_unit = -1;

    return 0;
}

int
bcma_cli_cleanup(bcma_cli_t *cli)
{
    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Free command list */
    bcma_cli_cmd_list_clear(cli);

    /* Mark object as uninitialized */
    cli->cli_sig = 0;

    return 0;
}

int
bcma_cli_cmd_list_clear(bcma_cli_t *cli)
{
    bcma_cli_cmd_list_t *cl;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Walk command list and free all entries */
    while (cli->command_root != NULL) {
        cl = cli->command_root;
        cli->command_root = cli->command_root->next;
        if (cl->cmd && cl->cmd->cleanup) {
            cl->cmd->cleanup(cli);
        }
        bcma_cli_cmd_entry_destroy(cl);
    }

    return 0;
}

int
bcma_cli_input_cb_set(bcma_cli_t *cli, const char *prompt,
                      bcma_cli_gets_f cb_gets,
                      bcma_cli_history_add_f cb_history_add)
{
    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    cli->prompt = prompt;
    cli->gets = cb_gets;
    cli->history_add = cb_history_add;

    return 0;
}
