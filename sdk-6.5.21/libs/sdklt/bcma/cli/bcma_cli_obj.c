/*! \file bcma_cli_obj.c
 *
 * Create/destroy CLI object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli.h>

#include "cli_internal.h"

bcma_cli_t *
bcma_cli_create(void)
{
    bcma_cli_t *cli = sal_alloc(sizeof(*cli), "bcmaCliObj");

    if (cli == NULL) {
        return NULL;
    }

    /* Initialize cliironment */
    if (bcma_cli_init(cli) < 0) {
        sal_free(cli);
        return NULL;
    }

    /* Mark object as dynamically allocated */
    cli->dyn_sig = CLI_SIGNATURE;

    return cli;
}

int
bcma_cli_destroy(bcma_cli_t *cli)
{
    if (cli == NULL || BAD_OBJ(cli)) {
        return -1;
    }

    /* Free CLI resources */
    bcma_cli_cleanup(cli);

    /* Mark object as invalid */
    cli->dyn_sig = 0;

    sal_free(cli);

    return 0;
}

bcma_cli_cmd_list_t *
bcma_cli_cmd_entry_create(void)
{
    bcma_cli_cmd_list_t *cl = sal_alloc(sizeof(*cl), "bcmaCliCmd");

    if (cl == NULL) {
        return NULL;
    }

    return cl;
}

int
bcma_cli_cmd_entry_destroy(bcma_cli_cmd_list_t *cl)
{
    if (cl == NULL) {
        return -1;
    }

    sal_free(cl);

    return 0;
}
