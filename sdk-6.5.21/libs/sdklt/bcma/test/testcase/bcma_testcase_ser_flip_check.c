/*! \file bcma_testcase_ser_flip_check.c
 *
 *  Test case for SER.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_ser_flip_check.h>
#include <sal/sal_libc.h>
#include <bcma/test/testcase/bcma_testcase_ser_flip_check.h>
#include <bcma/test/util/bcma_testutil_ser.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static int
bcma_testcase_ser_ih_select(int unit)
{
    return bcma_testutil_ser_enabled(unit);
}

static int
bcma_testcase_ser_ih_param_parse(int unit, bcma_cli_t *cli,
                                 bcma_cli_args_t *a, uint32_t flags,
                                 void **bp)
{
    bcma_testutil_ser_ih_param_t *ser_ih_param = NULL;
    size_t alloc_size = sizeof(bcma_testutil_ser_ih_param_t);
    bool show = false;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    ser_ih_param = sal_alloc(alloc_size, "bcmaTestCaseSERIhPrm");
    SHR_NULL_CHECK(ser_ih_param, SHR_E_MEMORY);

    sal_memset(ser_ih_param, 0, alloc_size);

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }
    rv = bcma_testutil_ser_ih_param_parse(unit, cli, a, ser_ih_param, show);
    SHR_IF_ERR_EXIT(rv);

    *bp = (void*)ser_ih_param;

exit:
    if (SHR_FUNC_ERR()) {
        if (ser_ih_param != NULL) {
            sal_free(ser_ih_param);
        }
        *bp = NULL;
    }
    SHR_FUNC_EXIT();
}

static void
bcma_testcase_ser_ih_help(int unit, void *bp)
{
    bcma_testutil_ser_ih_print_help(unit);

    return;
}

static int
bcma_testcase_ser_ih_init(int unit, void *bp, uint32_t option)
{
    return bcma_testutil_ser_ih_init(unit, (bcma_testutil_ser_ih_param_t *)bp);
}

static int
bcma_testcase_ser_ih_cleanup(int unit, void *bp, uint32_t option)
{
    return bcma_testutil_ser_ih_cleanup(unit, (bcma_testutil_ser_ih_param_t *)bp);
}

static int
bcma_testcase_ser_ih_write(int unit, void *bp, uint32_t option)
{
    bcma_testutil_ser_ih_param_t *ser_ih_p = (bcma_testutil_ser_ih_param_t *)bp;

    return bcma_testutil_ser_ih_write(unit, ser_ih_p);
}

static int
bcma_testcase_ser_ih_read_check(int unit, void *bp, uint32_t option)
{
    bcma_testutil_ser_ih_param_t *ser_ih_p = (bcma_testutil_ser_ih_param_t *)bp;

    return bcma_testutil_ser_ih_read_check(unit, ser_ih_p);
}

static bcma_test_proc_t bcma_testcase_ser_ih_proc[] = {
    {
        .desc = "Init SER In_House test.\n",
        .cb = bcma_testcase_ser_ih_init,
    },
    {
        .desc = "Try to write all PTs.\n",
        .cb = bcma_testcase_ser_ih_write,
    },
    {
        .desc = "Try to read and check all PTs.\n",
        .cb = bcma_testcase_ser_ih_read_check,
    },
    {
        .desc = "Clean up SER In_House test.\n",
        .cb = bcma_testcase_ser_ih_cleanup,
    }
};

static bcma_test_op_t bcma_test_case_ser_ih_op = {
    .select = bcma_testcase_ser_ih_select,
    .parser = bcma_testcase_ser_ih_param_parse,
    .help = bcma_testcase_ser_ih_help,
    .recycle = NULL,
    .procs = bcma_testcase_ser_ih_proc,
    .proc_count = COUNTOF(bcma_testcase_ser_ih_proc)
};

bcma_test_op_t *
bcma_testcase_ser_ih_op_get(void)
{
    return &bcma_test_case_ser_ih_op;
}

