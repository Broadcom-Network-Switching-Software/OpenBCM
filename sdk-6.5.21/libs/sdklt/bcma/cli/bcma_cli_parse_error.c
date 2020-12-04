/*! \file bcma_cli_parse_error.c
 *
 * Implementation of CLI command parser error functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli_parse.h>

int
bcma_cli_parse_error(const char *desc, const char *arg)
{
    if (desc) {
        if (arg) {
            cli_out("%sUnable to parse %s%sargument '%s'\n",
                    BCMA_CLI_CONFIG_ERROR_STR, desc, desc[0] ? " " : "", arg);
        } else {
            cli_out("%sMissing %s argument\n",
                    BCMA_CLI_CONFIG_ERROR_STR, desc);
        }
    }
    return BCMA_CLI_CMD_FAIL;
}
