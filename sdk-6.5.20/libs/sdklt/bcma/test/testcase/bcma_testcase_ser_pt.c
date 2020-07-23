/*! \file bcma_testcase_ser_pt.c
 *
 *  Test case for SER.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlt/bcmlt.h>
#include <bcmptm/bcmptm_ser_testutil.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_ser.h>
#include <bcma/test/testcase/bcma_testcase_ser_pt.h>
#include <sal/sal_libc.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static int
bcma_testcase_ser_select(int unit)
{
    return bcma_testutil_ser_enabled(unit);
}

static int
bcma_testcase_ser_param_parse(int unit, bcma_cli_t *cli, bcma_cli_args_t *a, uint32_t flags, void **bp)
{
    bcma_testutil_ser_param_t *ser_param = NULL;
    size_t alloc_size = sizeof(bcma_testutil_ser_param_t);
    bool show = false;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    ser_param = sal_alloc(alloc_size, "bcmaTestCaseSERPrm");
    SHR_NULL_CHECK(ser_param, SHR_E_MEMORY);

    sal_memset(ser_param, 0, alloc_size);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }
    rv = bcma_testutil_ser_param_parse(unit, cli, a, ser_param, show);
    SHR_IF_ERR_EXIT(rv);

    *bp = (void*)ser_param;

exit:
    if (SHR_FUNC_ERR()) {
        if (ser_param != NULL) {
            sal_free(ser_param);
        }
        *bp = NULL;
    }

    SHR_FUNC_EXIT();
}

static void
bcma_testcase_ser_help(int unit, void *bp)
{
    bcma_testutil_ser_print_help(unit);

    return;
}

static int
bcma_testcase_ser_prepare(int unit, int inject_only)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcma_testutil_ser_counter_poll_config(unit, 1);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmptm_ser_testutil_refresh_regs_disable(unit, 1);
    SHR_IF_ERR_EXIT(rv);

    rv = bcma_testutil_ser_ctrl_hsf_machine_enable(unit, FALSE);
    SHR_IF_ERR_EXIT(rv);

    rv = bcma_testutil_ser_ctrl_squash_config(unit, 0);
    SHR_IF_ERR_EXIT(rv);

    if (inject_only) {
        rv = bcma_testutil_ser_mem_scan_enable(unit, 1, 0);
    } else {
        rv = bcma_testutil_ser_mem_scan_enable(unit, 0, 0);
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
bcma_testcase_ser_resume(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_ser_counter_poll_config(unit, 0));

    SHR_IF_ERR_EXIT
        (bcma_testutil_ser_ctrl_squash_config(unit, 10000));

    SHR_IF_ERR_EXIT
        (bcmptm_ser_testutil_refresh_regs_disable(unit, 0));

    SHR_IF_ERR_EXIT
        (bcma_testutil_ser_ctrl_hsf_machine_enable(unit, TRUE));

    SHR_IF_ERR_EXIT
        (bcma_testutil_ser_mem_scan_enable(unit, 1, 1));
exit:
    SHR_FUNC_EXIT();
}

static int
bcma_testcase_ser_inject_validate(int unit, void *bp, uint32_t option)
{
    bcma_testutil_ser_param_t *ser_param = (bcma_testutil_ser_param_t *)bp;
    int rv = SHR_E_NONE, num_entry;

    SHR_FUNC_ENTER(unit);

    rv = bcma_testcase_ser_prepare(unit, ser_param->inject_only);
    SHR_IF_ERR_EXIT(rv);

    if (ser_param->test_ctrl_reg) {
        rv = bcma_testutil_ser_parity_ctrl_check(unit);
    } else if (sal_strcmp(ser_param->pt_id_name, PT_NAME_NONE) != 0) {
        rv = bcma_testutil_ser_inject_validate(unit, ser_param, &num_entry);
    } else {
        /* Test all PTs */
        rv = bcma_testutil_ser_all_pts_test(unit, ser_param);
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    (void)bcma_testcase_ser_resume(unit);
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t bcma_testcase_ser_proc[] = {
    {
        .desc = "Inject or validate SER error.\n",
        .cb = bcma_testcase_ser_inject_validate,
    }
};

static bcma_test_op_t bcma_test_case_ser_op = {
    .select = bcma_testcase_ser_select,
    .parser = bcma_testcase_ser_param_parse,
    .help = bcma_testcase_ser_help,
    .recycle = NULL,
    .procs = bcma_testcase_ser_proc,
    .proc_count = COUNTOF(bcma_testcase_ser_proc)
};

bcma_test_op_t *
bcma_testcase_ser_op_get(void)
{
    return &bcma_test_case_ser_op;
}

