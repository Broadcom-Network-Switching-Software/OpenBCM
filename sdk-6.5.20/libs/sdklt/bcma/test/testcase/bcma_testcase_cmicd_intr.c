/*! \file bcma_testcase_cmicd_intr.c
 *
 * Broadcom test cases for CMICD interrupts
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
#include <bcma/test/testcase/bcma_testcase_cmicd_intr.h>

#include <bcmbd/bcmbd_cmicd_intr.h>
#include <bcmbd/bcmbd_cmicd_acc.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define SWI_INTR(_swi_num) (CMICD_IRQ_SW_INTR0 + (_swi_num))

static int intr_cnt;
static int intr_num;
static int intr_cmc;

#define MAX_INTRS       4
#define MAX_CMCS        3

typedef struct cmicd_intr_test_param_s {
    int cmc;
    int swi_num;
} cmicd_intr_test_param_t;

static void swi_intr_cmc(int unit, uint32_t intr_param)
{
    CMIC_CMC_SW_INTR_CONFIGr_t swi;

    intr_cnt++;
    intr_num = intr_param & 0x3;
    intr_cmc = intr_param >> 2;

    /* Clear interrupt condition */
    CMIC_CMC_SW_INTR_CONFIGr_CLR(swi);
    CMIC_CMC_SW_INTR_CONFIGr_SW_INTR_STAT_BIT_POSITIONf_SET(swi, intr_num);
    WRITE_CMIC_CMC_SW_INTR_CONFIGr(unit, intr_cmc, swi);
}


static void swi_trig_cmc(int unit, int cmc, uint32_t swi_num)
{
    CMIC_CMC_SW_INTR_CONFIGr_t swi;

    /* Set software interrupt */
    CMIC_CMC_SW_INTR_CONFIGr_CLR(swi);
    CMIC_CMC_SW_INTR_CONFIGr_SW_INTR_STAT_BIT_POSITIONf_SET(swi, swi_num);
    CMIC_CMC_SW_INTR_CONFIGr_SW_INTR_STAT_BIT_VALUEf_SET(swi, 1);
    WRITE_CMIC_CMC_SW_INTR_CONFIGr(unit, cmc, swi);
}

static int
cmicd_intr_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICD);
}

static int
cmicd_intr_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                  uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    cmicd_intr_test_param_t *p = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Allocate the resource for savng test related parameters */
    SHR_ALLOC(p, sizeof(*p),"bcmaTestCaseCmicdIntrPrm");
    if (p == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->cmc = 0;
    p->swi_num = 1;

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "CMC", "int",
                             &p->cmc, NULL);
    bcma_cli_parse_table_add(&pt, "SoftWareIntr", "int",
                             &p->swi_num, NULL);

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

    /* Clear parse table */
    bcma_cli_parse_table_done(&pt);

    /* If CLI inputs exceed the boundary of parameters, reconfigure to default value */
    if (p->cmc < 0 || p->cmc >= MAX_CMCS) {
        p->cmc = 0;
    }
    if (p->swi_num < 0 || p->swi_num >= MAX_INTRS) {
        p->swi_num = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

static void
cmicd_intr_help(int unit, void *bp)
{
    cmicd_intr_test_param_t *p = (cmicd_intr_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    cli_out("  CMC - CMC Number[0-2]\n");
    cli_out("  SoftWareIntr - Software Interrupt Number[0-3]\n");
    cli_out("  ====\n");
    cli_out("  cmc=%"PRId32"\n", p->cmc);
    cli_out("  swi_num=%"PRId32"\n", p->swi_num);
    return;
}

static int
cmicd_intr_test(int unit, void *bp, uint32_t option)
{
    cmicd_intr_test_param_t *p = (cmicd_intr_test_param_t *)bp;
    int cmc = p->cmc;
    int swi_num = p->swi_num;
    int intr_param = (cmc << 2) | swi_num;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* 0. Set per-source interrupt handler */
    intr_cnt = intr_num = intr_cmc = 0;
    bcmbd_cmicd_intr_func_set(unit, cmc, SWI_INTR(swi_num),
                              swi_intr_cmc, intr_param);

    /* 1. Generate interrupt with interrupt enabled */
    bcmbd_cmicd_intr_enable(unit, cmc, SWI_INTR(swi_num));
    swi_trig_cmc(unit, cmc, swi_num);

    sal_usleep(1000);

    /* 2. Check that we got our interrupt */
    if (intr_cnt != 1 || intr_num != swi_num) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Can not get the interrupt!\n")));
        bcmbd_cmicd_intr_disable(unit, cmc, SWI_INTR(swi_num));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* 3. Generate interrupt with interrupt disabled */
    bcmbd_cmicd_intr_disable(unit, cmc, SWI_INTR(swi_num));
    swi_trig_cmc(unit, cmc, swi_num);

    sal_usleep(1000);

    /* 4. Check that interrupt count is unchanged */
    if (intr_cnt != 1 || intr_num != swi_num) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Not the expected interrupt times!\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* 5. Enable interrupt to allow pending interrupt through */
    bcmbd_cmicd_intr_enable(unit, cmc, SWI_INTR(swi_num));

    sal_usleep(1000);

    /* 6. Check that we got our second interrupt */
    if (intr_cnt != 2 || intr_num != swi_num) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Can not get the second interrupt!\n")));
    }

    /* 7. Disable interrupt */
    bcmbd_cmicd_intr_disable(unit, cmc, SWI_INTR(swi_num));

    /* 8. Remove per-source interrupt handler */
    bcmbd_cmicd_intr_func_set(unit, cmc, SWI_INTR(swi_num),
                              NULL, 0);
exit:
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

static bcma_test_proc_t cmicd_intr_proc[] = {
    {
        .desc = BCMA_TEST_PROC_DESCRIPTION,
        .cb = cmicd_intr_test,
    },
};

#undef BCMA_TEST_PROC_DESCRIPTION

static bcma_test_op_t cmicd_intr_op = {
    .select = cmicd_intr_select,
    .parser = cmicd_intr_parser,
    .help = cmicd_intr_help,
    .recycle = NULL,
    .procs = cmicd_intr_proc,
    .proc_count = COUNTOF(cmicd_intr_proc),
};

bcma_test_op_t *
bcma_testcase_cmicd_intr_op_get(void)
{
    return &cmicd_intr_op;
}
