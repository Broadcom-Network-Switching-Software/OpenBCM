/*! \file bcma_testcase_cmicd_ccmdma.c
 *
 * Broadcom test cases for CMICD CCMDMA access
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

#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_ccmdma.h>

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_ccmdma.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

typedef struct cmicd_ccmdma_test_param_s {
    int intr_mode;          /* 0:Polling, 1:Intr, Default=0 */
    int xfer_size;          /* transfer size (multiple of 4 bytes) */

    uint32_t *src_buf, *dst_buf;
    uint64_t src_paddr, dst_paddr;
    bcmbd_ccmdma_work_t work;
} cmicd_ccmdma_test_param_t;

#define TRANS_MEM_SIZE          0x10000
#define CCMDMA_MAGIC_DATA       0xaa555a5a

static int
cmicd_ccmdma_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICD);
}

static int
cmicd_ccmdma_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                    uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    cmicd_ccmdma_test_param_t *p = NULL;
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Allocate the resource for savng test related parameters */
    SHR_ALLOC(p, sizeof(*p),"bcmaTestCaseCmicdCCMDMAPrm");
    if (p == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->intr_mode = 0;
    p->xfer_size = TRANS_MEM_SIZE;

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "IntMode", "bool",
                             &p->intr_mode, NULL);
    bcma_cli_parse_table_add(&pt, "XferCount", "int",
                             &p->xfer_size, NULL);

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

    if ((p->xfer_size <= 0) || (p->xfer_size % sizeof(uint32_t))) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: Invalid transfer size %d\n"), p->xfer_size));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    p->src_buf = bcmdrd_hal_dma_alloc(unit, p->xfer_size,
                                      "bcmaTestCaseCmicdCCMDMASrcBuf",
                                      &p->src_paddr);
    if (!p->src_buf) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }
    for (i = 0; i < p->xfer_size / sizeof(uint32_t); i++) {
        p->src_buf[i] = CCMDMA_MAGIC_DATA;
    }

    p->dst_buf = bcmdrd_hal_dma_alloc(unit, p->xfer_size,
                                      "bcmaTestCaseCmicdCCMDMADstBuf",
                                      &p->dst_paddr);
    if (!p->dst_buf) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(p->dst_buf, 0, p->xfer_size);

    p->work.xfer_count = p->xfer_size / sizeof(uint32_t);
    p->work.flags = p->intr_mode ? CCMDMA_WF_INT_MODE : 0;

    p->work.src_paddr = p->src_paddr;
    p->work.dst_paddr = p->dst_paddr;
    p->work.flags |= CCMDMA_WF_DST_OUTER;

exit:
    SHR_FUNC_EXIT();
}

static void
cmicd_ccmdma_help(int unit, void *bp)
{
    cmicd_ccmdma_test_param_t *p = (cmicd_ccmdma_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    cli_out("  IntMode      - Interrupt Mode[0: Pulling, 1: Intr]\n");
    cli_out("  XferSize     - Transfer size, should be multiple of 4 bytes\n");
    cli_out("  ====================\n");
    cli_out("  intr_mode    =%"PRId32"\n", p->intr_mode);
    cli_out("  xfer_size    =%"PRId32"\n", p->xfer_size);
    cli_out("  src_paddr    =0x%"PRIx64"\n", p->src_paddr);
    cli_out("  dst_paddr    =0x%"PRIx64"\n", p->dst_paddr);
    return;
}

static void
cmicd_ccmdma_recycle(int unit, void *bp)
{
    cmicd_ccmdma_test_param_t *p = (cmicd_ccmdma_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    if (p->src_buf) {
        bcmdrd_hal_dma_free(unit, p->xfer_size, p->src_buf, p->src_paddr);
        p->src_buf = NULL;
    }
    if (p->dst_buf) {
        bcmdrd_hal_dma_free(unit, p->xfer_size, p->dst_buf, p->dst_paddr);
        p->dst_buf = NULL;
    }
    return;
}

/*!
 * \brief Compare with two memories.
 *
 * This function is to compare with two memories.
 *
 * \param [in] dst Dest memory pointer.
 * \param [in] src Source memory pointer.
 * \param [in] wsize Size of memory in 32-bit words.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_FAIL Comparing fails.
 */
static int
cmicd_ccmdma_mem_cmp(uint32_t *dst, uint32_t *src, uint32_t wsize)
{
    uint32_t i;

    for (i = 0; i < wsize; i++) {
        if (src[i] != dst[i]) {
            return SHR_E_FAIL;
        }
    }

    return SHR_E_NONE;
}

static int
cmicd_ccmdma_test(int unit, void *bp, uint32_t option)
{
    cmicd_ccmdma_test_param_t *p = (cmicd_ccmdma_test_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmbd_ccmdma_xfer(unit, &p->work));
    SHR_IF_ERR_EXIT(cmicd_ccmdma_mem_cmp(p->dst_buf,
                                         p->src_buf,
                                         p->xfer_size / sizeof(uint32_t)));

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t cmicd_ccmdma_proc[] = {
    {
        .desc = "run CCM DMA access test.\n",
        .cb = cmicd_ccmdma_test,
    },
};

static bcma_test_op_t cmicd_ccmdma_op = {
    .select = cmicd_ccmdma_select,
    .parser = cmicd_ccmdma_parser,
    .help = cmicd_ccmdma_help,
    .recycle = cmicd_ccmdma_recycle,
    .procs = cmicd_ccmdma_proc,
    .proc_count = COUNTOF(cmicd_ccmdma_proc),
};

bcma_test_op_t *
bcma_testcase_cmicd_ccmdma_op_get(void)
{
    return &cmicd_ccmdma_op;
}
