/*! \file bcma_cli_ctrlc.c
 *
 * Wrappers for CLI Ctrl-C call-back vectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli_ctrlc.h>

#include "cli_internal.h"

static bool ctrlc_sig_active;

static int
ctrlc_sig_cb(void *cb_data)
{
    int *seq_no = (int *)cb_data;

    ctrlc_sig_active = true;

    /* Negative sequence number will bypass hard-break */
    if (*seq_no < 0) {
        return 0;
    }

    if (*seq_no == 0) {
        /*
         * Return non-zero to break the function immediately as hard-break
         * since the previous Ctrl-C signals are not processed through the
         * soft-break approach.
         */
        LOG_INFO(BSL_LS_APPL_SHELL,
                 (BSL_META("Hard break from Ctrl-C\n")));
        return -1;
    }

    *seq_no = *seq_no - 1;

    return 0;
}

bool
bcma_cli_ctrlc_break(void)
{
    return ctrlc_sig_active;
}

int
bcma_cli_ctrlc_sig_set(bool sig_active)
{
    ctrlc_sig_active = sig_active;

    return 0;
}

int
bcma_cli_ctrlc_cb_set(bcma_cli_t *cli, const bcma_cli_ctrlc_cb_t *ctrlc_cb,
                      void *cookie)
{
    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    cli->ctrlc_cb = ctrlc_cb;
    cli->ctrlc_cb_data = cookie;

    return 0;
}

int
bcma_cli_ctrlc_enable_set(bcma_cli_t *cli, int mode)
{
    const bcma_cli_ctrlc_cb_t *cb;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Perform call-back*/
    cb = cli->ctrlc_cb;
    if (cb && cb->ctrlc_enable_set) {
        return cb->ctrlc_enable_set(cli->ctrlc_cb_data, mode);
    }

    return 0;
}

int
bcma_cli_ctrlc_exec(bcma_cli_t *cli, int (*func)(void *), void *data,
                    int num_soft_break)
{
    const bcma_cli_ctrlc_cb_t *cb;
    int seq_no = num_soft_break;

    /* Check for valid CLI structure */
    if (cli == NULL || BAD_CLI(cli)) {
        return -1;
    }

    /* Sanity check for NULL function */
    if (func == NULL) {
        return -1;
    }

    /* Perform call-back */
    cb = cli->ctrlc_cb;
    if (cb && cb->ctrlc_exec) {
        if (num_soft_break == 0) {
            return cb->ctrlc_exec(cli->ctrlc_cb_data, func, data,
                                  NULL, NULL, BCMA_CLI_CMD_INTR);
        }
        bcma_cli_ctrlc_sig_set(false);
        return cb->ctrlc_exec(cli->ctrlc_cb_data, func, data,
                              ctrlc_sig_cb, &seq_no, BCMA_CLI_CMD_INTR);
    }

    /* Perform function call without Ctrl-C hook */
    return func(data);
}
