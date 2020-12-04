/*! \file bcma_envcmd_setenv.c
 *
 * CLI Environment shell commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/env/bcma_env.h>

#include <bcma/env/bcma_envcmd_setenv.h>

#include <bcma/cli/bcma_cli_var.h>

#include "envcmd_internal.h"

int
bcma_envcmd_setenv(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_env_handle_t eh;
    char *name, *value;
    int rv = 0;

    if (cli == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    eh = (bcma_env_handle_t)cli->var_cb_data;
    if (eh == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    switch (BCMA_CLI_ARG_CNT(args)) {
    case 0:
        /* Show local variables in current scope */
        bcma_envcmd_show_var_list(eh, "Global Variables", 0);
        break;
    case 1:
        /* Remove variable */
        rv = bcma_env_var_unset(eh, BCMA_CLI_ARG_GET(args), FALSE, TRUE);
        break;
    case 2:
        /* Add variable */
        name = BCMA_CLI_ARG_GET(args);
        value= BCMA_CLI_ARG_GET(args);
        if (bcma_env_var_exists(eh, name, TRUE, FALSE)) {
            cli_out("%s: Warning: variable %s shadowed by local variable\n",
                    BCMA_CLI_ARG_CMD(args), name);
        }
        /*
         * Call the set function through CLI to check the validity of
         * the variable name.
         */
        rv = bcma_cli_var_set(cli, name, value, FALSE);
        break;
    default:
        /* Error */
        return BCMA_CLI_CMD_USAGE;
    }
    return (rv) ? BCMA_CLI_CMD_FAIL : BCMA_CLI_CMD_OK;
}
