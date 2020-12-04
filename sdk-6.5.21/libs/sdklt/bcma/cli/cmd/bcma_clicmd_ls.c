/*! \file bcma_clicmd_ls.c
 *
 * CLI 'ls' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/io/bcma_io_dir.h>

#include <bcma/cli/bcma_clicmd_ls.h>

#define OPT_BUF_SIZE 80

int
bcma_clicmd_ls(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *a = BCMA_CLI_ARG_GET(args);
    char opt[OPT_BUF_SIZE];
    int opt_size = 0;

    /* Collect all separate options to a single -<options> */
    while (a && a[0] == '-' && a[1] != '\0') {
        if (opt_size == 0) {
            opt_size += sal_sprintf(opt, "-");
        }
        opt_size += sal_snprintf(opt + opt_size, OPT_BUF_SIZE - opt_size,
                                 "%s", &a[1]);
        a = BCMA_CLI_ARG_GET(args);
    }

    /* Display current directory if no specified target. */
    if (a == NULL) {
        a = ".";
    }

    /* List each directory contents of each input path. */
    do {
        bcma_io_dir_ls(a, opt_size ? opt : NULL);
    } while ((a = BCMA_CLI_ARG_GET(args)) != NULL);

    return BCMA_CLI_CMD_OK;
}
