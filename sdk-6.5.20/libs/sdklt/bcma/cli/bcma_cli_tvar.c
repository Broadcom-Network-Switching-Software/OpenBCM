/*! \file bcma_cli_tvar.c
 *
 * Wrappers for CLI variable tag call-back vectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli_tvar.h>

#include "cli_internal.h"

int
bcma_cli_tvar_cb_set(bcma_cli_t *cli, const bcma_cli_tvar_cb_t *tvar_cb,
                     void *cookie)
{
    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    cli->tvar_cb = tvar_cb;
    cli->tvar_cb_data = cookie;

    return 0;
}

const char *
bcma_cli_tvar_get(bcma_cli_t *cli, const char *name, uint32_t *tag)
{
    const bcma_cli_tvar_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return NULL;
    }

    /* Perform call-back */
    cb = cli->tvar_cb;
    if (cb && cb->tvar_get) {
        return cb->tvar_get(cli->tvar_cb_data, name, tag);
    }
    return NULL;
}

int
bcma_cli_tvar_set(bcma_cli_t *cli, const char *name, const char *value,
                  uint32_t tag, int local)
{
    const bcma_cli_tvar_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Perform call-back */
    cb = cli->tvar_cb;
    if (cb && cb->tvar_set) {
        return cb->tvar_set(cli->tvar_cb_data, name, value, tag, local);
    }
    return -1;
}

int
bcma_cli_tvar_unset_by_tag(bcma_cli_t *cli, uint32_t tag, int local)
{
    const bcma_cli_tvar_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Perform call-back */
    cb = cli->tvar_cb;
    if (cb && cb->tvar_unset_by_tag) {
        return cb->tvar_unset_by_tag(cli->var_cb_data, tag, local);
    }
    return -1;
}
