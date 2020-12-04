/*! \file bcma_testcase_cmicx_intr.c
 *
 * Broadcom test cases for CMICx interrupts
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_intr.h>
#include <bcma/test/util/bcma_testutil_drv.h>

#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx_acc.h>

#include "bcma_testcase_cmicx_intr_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

typedef struct cmicx_intr_test_param_s {
    const char *host;
    int host_index;
} cmicx_intr_test_param_t;

int bcma_testcase_cmicx_intr_cnt;
int bcma_testcase_cmicx_intr_num;

static bcma_testcase_swi_test_t *cmicx_intr_swi_test;
static char *cmicx_intr_def_host;
static int cmicx_intr_swi_test_size;

static int
cmicx_intr_swi_test_get(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX2)) {
        SHR_IF_ERR_EXIT(bcma_testcase_cmicx2_intr_func_get(unit, &cmicx_intr_swi_test,
                                                           &cmicx_intr_swi_test_size,
                                                           &cmicx_intr_def_host));
    } else {
        SHR_IF_ERR_EXIT(bcma_testcase_cmicx_intr_func_get(unit, &cmicx_intr_swi_test,
                                                          &cmicx_intr_swi_test_size,
                                                          &cmicx_intr_def_host));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_intr_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX);
}

static int
cmicx_intr_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                  uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    char def_sname[128];
    char *sname = NULL;
    cmicx_intr_test_param_t *p = NULL;
    bcma_testcase_swi_test_t *swit;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(cmicx_intr_swi_test_get(unit));

    sal_memset(def_sname, 0, 128);
    sal_memcpy(def_sname, cmicx_intr_def_host, 128);
    sname = def_sname;

    /* Allocate the resource for savng test related parameters */
    SHR_ALLOC(p, sizeof(*p),"bcmaTestCaseCmicxIntrPrm");
    if (p == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->host = NULL;
    p->host_index = 0;

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Host", "str",
                             &sname, NULL);

    /* Parse the parse table */
    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        bcma_cli_parse_table_done(&pt);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (BCMA_CLI_ARG_CNT(a) > 0) {
        cli_out("%s: Unknown argument %s\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        bcma_cli_parse_table_done(&pt);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Show input arguments for "Test Help" CMD */
    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        bcma_cli_parse_table_show(&pt, NULL);
    }

    /* Save parameter parsed from CLI */
    for (i = 0; i < cmicx_intr_swi_test_size; i++) {
        swit = &cmicx_intr_swi_test[i];
        if (swit->host == NULL) {
            continue;
        }
        if (sal_strncasecmp(swit->host, sname,
                            sal_strlen(sname)) == 0) {
            p->host = swit->host;
            p->host_index = i;
        }
    }

    /* Clear parse table */
    bcma_cli_parse_table_done(&pt);

    if (p->host == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                   "Error: Should specify a valid host name\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
cmicx_intr_help(int unit, void *bp)
{
    cmicx_intr_test_param_t *p = (cmicx_intr_test_param_t *)bp;
    bcma_testcase_swi_test_t *swit;
    int i;

    if (p == NULL) {
        return;
    }

    (void)cmicx_intr_swi_test_get(unit);

    cli_out("  Host - Host device[");
    for (i = 0; i < cmicx_intr_swi_test_size; i++) {
        swit = &cmicx_intr_swi_test[i];
        if (swit->host == NULL) {
            continue;
        }
        cli_out("%s%s", swit->host, (i < cmicx_intr_swi_test_size - 1) ? "," : "");
    }
    cli_out("]\n");
    cli_out("  ====\n");
    cli_out("  host=%s\n", p->host ? p->host : "(null)");
    cli_out("  host_index=%"PRId32"\n", p->host_index);
    return;
}

static int
cmicx_intr_test(int unit, void *bp, uint32_t option)
{
    cmicx_intr_test_param_t *p = (cmicx_intr_test_param_t *)bp;
    bcma_testcase_swi_test_t *swit;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(cmicx_intr_swi_test_get(unit));

    swit = &cmicx_intr_swi_test[p->host_index];

    /* 0. Set per-source interrupt handler */
    bcma_testcase_cmicx_intr_cnt = bcma_testcase_cmicx_intr_num = 0;
    bcmbd_cmicx_intr_func_set(unit, IPROC_IRQ_ICFG_SW_PROG_INTR,
                              swit->swi_intr, IPROC_IRQ_ICFG_SW_PROG_INTR);

    /* 1. Generate interrupt with interrupt enabled */
    bcmbd_cmicx_intr_enable(unit, IPROC_IRQ_ICFG_SW_PROG_INTR);
    swit->swi_trig(unit);

    sal_usleep(10000);

    /* 2. Check that we got our interrupt */
    if (bcma_testcase_cmicx_intr_cnt != 1 ||
        bcma_testcase_cmicx_intr_num != IPROC_IRQ_ICFG_SW_PROG_INTR) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_FAIL,
                            (BSL_META_U(unit,
                             "Can not get the interrupt!\n")));
    }

    /* 3. Generate interrupt with interrupt disabled */
    bcmbd_cmicx_intr_disable(unit, IPROC_IRQ_ICFG_SW_PROG_INTR);
    swit->swi_trig(unit);

    sal_usleep(10000);

    /* 4. Check that interrupt count is unchanged */
    if (bcma_testcase_cmicx_intr_cnt != 1 ||
        bcma_testcase_cmicx_intr_num != IPROC_IRQ_ICFG_SW_PROG_INTR) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_FAIL,
                            (BSL_META_U(unit,
                             "Not the expected interrupt times!\n")));
    }

    /* 5. Enable interrupt to allow pending interrupt through */
    bcmbd_cmicx_intr_enable(unit, IPROC_IRQ_ICFG_SW_PROG_INTR);

    sal_usleep(20000);

    /* 6. Check that we got our second interrupt */
    if (bcma_testcase_cmicx_intr_cnt != 2 ||
        bcma_testcase_cmicx_intr_num != IPROC_IRQ_ICFG_SW_PROG_INTR) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_FAIL,
                            (BSL_META_U(unit,
                             "Can not get the second interrupt!\n")));
    }

exit:
    /* 7. Disable interrupt */
    bcmbd_cmicx_intr_disable(unit, IPROC_IRQ_ICFG_SW_PROG_INTR);

    /* 8. Remove per-source interrupt handler */
    bcmbd_cmicx_intr_func_set(unit, IPROC_IRQ_ICFG_SW_PROG_INTR,
                              NULL, 0);
    SHR_FUNC_EXIT();
}

#define BCMA_TEST_PROC_DESCRIPTION \
    "Execute the SW interrupts test.\n" \
    "    0). Set per-source interrupt handler.\n" \
    "    1). Generate interrupt with interrupt enabled.\n" \
    "    2). Check that we got our interrupt.\n" \
    "    3). Generate interrupt with interrupt disabled.\n" \
    "    4). Check that interrupt count is unchanged.\n" \
    "    5). Enable interrupt to allow pending interrupt through.\n" \
    "    6). Check that we got our second interrupt.\n" \
    "    7). Disable interrupt.\n" \
    "    8). Remove per-source interrupt handler.\n" \
    ""

static bcma_test_proc_t cmicx_intr_proc[] = {
    {
        .desc = BCMA_TEST_PROC_DESCRIPTION,
        .cb = cmicx_intr_test,
    },
};

#undef BCMA_TEST_PROC_DESCRIPTION

static bcma_test_op_t cmicx_intr_op = {
    .select = cmicx_intr_select,
    .parser = cmicx_intr_parser,
    .help = cmicx_intr_help,
    .recycle = NULL,
    .procs = cmicx_intr_proc,
    .proc_count = COUNTOF(cmicx_intr_proc),
};

bcma_test_op_t *
bcma_testcase_cmicx_intr_op_get(void)
{
    return &cmicx_intr_op;
}

