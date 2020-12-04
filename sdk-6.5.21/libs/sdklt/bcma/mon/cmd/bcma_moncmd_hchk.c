/*! \file bcma_moncmd_hchk.c
 *
 *  CLI command for 'hchk' command user interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/mon/bcma_moncmd_hchk.h>

/*!
 * \brief CLI 'hchk' command for debugging.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
int
bcma_moncmd_hchk(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int unit;
    bool test_result;

    /* Variables initialization */
    unit = cli->cmd_unit;

    /* Attach test driver */
    rv = bcma_testutil_drv_attach(unit);
    if (SHR_FAILURE(rv)) {
        return BCMA_CLI_CMD_FAIL;
    }

    rv = bcma_testutil_drv_check_health(unit, &test_result);
    if (SHR_FAILURE(rv)) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}
