/*! \file bcma_hchkcmd_hchk.c
 *
 *  CLI command for hchk command user interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/hchk/bcma_hchkcmd_hchk.h>
#include <bcma/io/bcma_io_file.h>
#include <bcmbd/bcmbd.h>
#include <shr/shr_pb.h>
#include <bcma/test/util/bcma_testutil_drv.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_TEST

#define TOTAL_HCHK_ARGUMENT 0

/*!
 * \brief CLI 'hchk' command for debugging.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
int
bcma_hchkcmd_hchk(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    bool test_result;

    /* Variables initialization */
    unit = cli->cmd_unit;

    /* Attach test driver */
    bcma_testutil_drv_attach(unit);

    bcma_testutil_drv_check_health(unit, &test_result);


    return BCMA_CLI_CMD_OK;
}
