/*! \file bcma_cli_path.c
 *
 *  Functions for retrieving and parsing the CLI 'path' variable.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_path.h>

char *
bcma_cli_path_tok_get(bcma_cli_t *cli, char **s, char **s2)
{
    const char *path;

    path = bcma_cli_var_get(cli, BCMA_CLI_VAR_PATH);
    if (path == NULL) {
        path = ".";
    }

    if (s != NULL) {
        *s = sal_strdup(path);
        if (*s == NULL) {
            return NULL;
        }
    }

    /* Each path is separated by space or colon in $BCMA_CLI_VAR_PATH  */
    return sal_strtok_r((s != NULL) ? *s : NULL, " :", s2);
}
