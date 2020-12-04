/*! \file bcma_salcmd_date.c
 *
 * Test date API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_date.h>

#include <bcma/sal/bcma_salcmd_date.h>

int
bcma_salcmd_date(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    sal_date_t date;

    cli_out("%s\n", sal_date(&date));

    return BCMA_CLI_CMD_OK;
}
