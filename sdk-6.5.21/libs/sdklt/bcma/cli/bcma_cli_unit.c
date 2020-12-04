/*! \file bcma_cli_unit.c
 *
 * Wrappers for CLI Ctrl-C call-back vectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli_unit.h>

#include "cli_internal.h"

int
bcma_cli_unit_set(bcma_cli_t *cli, int unit)
{
    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    if (bcma_cli_unit_valid(cli, unit) || unit == -1) {
        cli->cur_unit = unit;
    }

    return 0;
}

int
bcma_cli_unit_cb_set(bcma_cli_t *cli, const bcma_cli_unit_cb_t *unit_cb,
                     void *cookie)
{
    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    cli->unit_cb = unit_cb;
    cli->unit_cb_data = cookie;

    return 0;
}

int
bcma_cli_unit_max(bcma_cli_t *cli)
{
    const bcma_cli_unit_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return 0;
    }

    /* Perform call-back */
    cb = cli->unit_cb;
    if (cb && cb->unit_max) {
        return cb->unit_max(cli->unit_cb_data);
    }

    return 0;
}

int
bcma_cli_unit_valid(bcma_cli_t *cli, int unit)
{
    const bcma_cli_unit_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return FALSE;
    }

    /* Perform call-back */
    cb = cli->unit_cb;
    if (cb && cb->unit_valid) {
        return cb->unit_valid(cli->unit_cb_data, unit);
    }

    return FALSE;
}

int
bcma_cli_unit_feature_enabled(bcma_cli_t *cli, int unit, int feature)
{
    const bcma_cli_unit_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return FALSE;
    }

    /* Perform call-back */
    cb = cli->unit_cb;
    if (cb && cb->unit_feature_enabled) {
        return cb->unit_feature_enabled(cli->unit_cb_data, unit, feature);
    }

    return FALSE;
}
