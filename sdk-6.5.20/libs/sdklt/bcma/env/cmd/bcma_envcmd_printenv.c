/*! \file bcma_envcmd_printenv.c
 *
 * CLI Environment shell commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bsl/bsl.h>

#include <bcma/env/bcma_env.h>

#include <bcma/env/bcma_envcmd_printenv.h>

#include "envcmd_internal.h"

int
bcma_envcmd_printenv(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_env_handle_t eh;
    const char *scope;
    int do_local = FALSE;
    int do_global = FALSE;
    int do_all = FALSE;
    int scopes;

    if (cli == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    eh = (bcma_env_handle_t)cli->var_cb_data;
    if (eh == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        /* Print everything */
        do_local  = TRUE;
        do_global = TRUE;
    } else {
        scope = BCMA_CLI_ARG_GET(args);
        if (sal_strcasecmp("local", scope) == 0) {
            /* Local only */
            do_local = TRUE;
        } else if (sal_strcasecmp("global", scope) == 0) {
            /* Global only  */
            do_global = TRUE;
        } else if (sal_strcasecmp("all", scope) == 0) {
            /* All scopes */
            do_all = TRUE;
            do_global = TRUE;
        } else {
            return BCMA_CLI_CMD_USAGE;
        }
    }
    if (BCMA_CLI_ARG_CNT(args) != 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    scopes = bcma_env_num_scopes_get(eh);
    if (do_all) {
        int lvl;
        char lvl_str[64];

        for (lvl = 1; lvl <= scopes; lvl++) {
            sal_snprintf(lvl_str, sizeof(lvl_str), "Scope Level %d", lvl);
            bcma_envcmd_show_var_list(eh, lvl_str, lvl);
        }
    } else if (do_local) {
        if (scopes > 0) {
            bcma_envcmd_show_var_list(eh, "Local Variables", 1);
        }
    }
    if (do_global) {
        bcma_envcmd_show_var_list(eh, "Global Variables", 0);
    }

    return BCMA_CLI_CMD_OK;
}
