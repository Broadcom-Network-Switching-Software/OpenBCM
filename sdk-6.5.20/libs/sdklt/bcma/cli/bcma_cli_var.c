/*! \file bcma_cli_var.c
 *
 * Wrappers for CLI variable call-back vectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>

#include "cli_internal.h"

int
bcma_cli_var_cb_set(bcma_cli_t *cli, const bcma_cli_var_cb_t *var_cb,
                    void *cookie)
{
    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    cli->var_cb = var_cb;
    cli->var_cb_data = cookie;

    return 0;
}

const char *
bcma_cli_var_get(bcma_cli_t *cli, const char *name)
{
    const bcma_cli_var_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return NULL;
    }

    /* Perform call-back */
    cb = cli->var_cb;
    if (cb && cb->var_get) {
        return cb->var_get(cli->var_cb_data, name);
    }

    return NULL;
}

int
bcma_cli_var_set(bcma_cli_t *cli, const char *name, const char *value,
                 int local)
{
    const bcma_cli_var_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    if (name == NULL) {
        return -1;
    }

    /*
     * Except the CLI command result variable $?, only
     * alphanumeric characters and underscores are accepted as
     * valid CLI variable names.
     */
    if (sal_strcmp(name, "?") != 0) {
        const char *ptr = name;

        while (*ptr != '\0') {
            if (!sal_isalnum(*ptr) && *ptr != '_') {
                cli_out("%sVariable name must contain only "
                        "alphanumeric characters and underscores.\n",
                        BCMA_CLI_CONFIG_ERROR_STR);
                return -1;
            }
            ptr++;
        }
    }

    /* Perform call-back */
    cb = cli->var_cb;
    if (cb && cb->var_set) {
        return cb->var_set(cli->var_cb_data, name, value, local);
    }

    return -1;
}

int
bcma_cli_var_result_set(bcma_cli_t *cli, int rv)
{
    char v_str[12];

    sal_snprintf(v_str, sizeof(v_str), "%d", rv);

    return bcma_cli_var_set(cli, BCMA_CLI_VAR_RESULT, v_str, TRUE);
}

int
bcma_cli_var_bool_get(bcma_cli_t *cli, const char *name, int deflt)
{
    int val = 0;
    const char *str_val = bcma_cli_var_get(cli, name);

    if (str_val == NULL) {
        /* Return default value if the variable is not set. */
        return deflt;
    }

    if (bcma_cli_parse_int(str_val, &val) < 0) {
        /* If not an integer, look for Boolean strings */
        if (sal_strcasecmp(str_val, "true") == 0) {
            return TRUE;
        }
        if (sal_strcasecmp(str_val, "false") == 0) {
            return FALSE;
        }
        cli_out("bcma_cli_var_bool_get: %s: Invalid Boolean value\n", name);
        return deflt;
    }

    return (val == 0) ? FALSE : TRUE;
}

int
bcma_cli_var_scope_push(bcma_cli_t *cli)
{
    const bcma_cli_var_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Perform call-back */
    cb = cli->var_cb;
    if (cb && cb->var_scope_push) {
        return cb->var_scope_push(cli->var_cb_data);
    }

    return -1;
}

int
bcma_cli_var_scope_pop(bcma_cli_t *cli)
{
    const bcma_cli_var_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Perform call-back */
    cb = cli->var_cb;
    if (cb && cb->var_scope_pop) {
        return cb->var_scope_pop(cli->var_cb_data);
    }

    return -1;
}
