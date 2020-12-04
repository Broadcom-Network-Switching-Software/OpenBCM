/*! \file bcma_clicmd_version.c
 *
 * CLI 'version' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_version.h>

static const char *version_str = "No version information.\n";

int
bcma_clicmd_version(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    COMPILER_REFERENCE(cli);
    COMPILER_REFERENCE(args);

    cli_out("%s", version_str);

    return BCMA_CLI_CMD_OK;
}

int
bcma_clicmd_version_string_set(const char *vstr)
{
    /* Assume that provided string is static or const */
    version_str = vstr;

    return 0;
}

const char *
bcma_clicmd_version_string_get()
{
    return version_str;
}
