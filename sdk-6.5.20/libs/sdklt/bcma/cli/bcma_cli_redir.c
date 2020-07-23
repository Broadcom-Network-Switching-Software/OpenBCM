/*! \file bcma_cli_redir.c
 *
 * Wrappers for CLI I/O redirection ("|") call-back vectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli_redir.h>

#include "cli_internal.h"

int
bcma_cli_redir_cb_set(bcma_cli_t *cli, const bcma_cli_redir_cb_t *redir_cb,
                      void *cookie)
{
    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    cli->redir_cb = redir_cb;
    cli->redir_cb_data = cookie;

    return 0;
}

int
bcma_cli_redir_enable_set(bcma_cli_t *cli, int enable)
{
    const bcma_cli_redir_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Perform call-back*/
    cb = cli->redir_cb;
    if (cb && cb->redir_enable_set) {
        return cb->redir_enable_set(cli->redir_cb_data, enable);
    }

    return 0;
}

int
bcma_cli_redir_cmd_done(bcma_cli_t *cli)
{
    const bcma_cli_redir_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Perform call-back*/
    cb = cli->redir_cb;
    if (cb && cb->redir_cmd_done) {
        return cb->redir_cmd_done(cli->redir_cb_data);
    }

    return 0;
}

const char *
bcma_cli_redir_input_name_get(bcma_cli_t *cli)
{
    const bcma_cli_redir_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return NULL;
    }

    /* Perform call-back*/
    cb = cli->redir_cb;
    if (cb && cb->redir_input_name_get) {
        return cb->redir_input_name_get(cli->redir_cb_data);
    }

    return NULL;
}

const char *
bcma_cli_redir_output_name_get(bcma_cli_t *cli)
{
    const bcma_cli_redir_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return NULL;
    }

    /* Perform call-back*/
    cb = cli->redir_cb;
    if (cb && cb->redir_output_name_get) {
        return cb->redir_output_name_get(cli->redir_cb_data);
    }

    return NULL;
}
