/*! \file bcma_testcase_cmicd_fifodma.c
 *
 * Broadcom test cases for CMICD FIFODMA  access
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>

#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_fifodma.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_fifodma.h>
#include <bcma/test/util/bcma_testutil_drv.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define SCMH_BANK_SET(d,v)      F32_SET(d,1,2,v)
#define SCMH_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define SCMH_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define SCMH_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define SCMH_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define SCMH_OPCODE_SET(d,v)    F32_SET(d,26,6,v)


typedef struct cmicd_fifodma_test_param_s {
    char *sname;            /* Symbol Name */
    int intrmode;           /* 0:Polling, 1:Intr, Default=0 */
    int debug;              /* 0:False, 1:True, Default=0 */

    int unit;
    int ctrl_interval;
    shr_thread_t handler_tc;
    shr_thread_t trigger_tc;
    bcmbd_fifodma_data_t data;
    bcmbd_fifodma_work_t work;
    int buf_entries;
    int fifo_chan;
    int test_result;
} cmicd_fifodma_test_param_t;

#define FIFODMA_DIRTY_DATA      0xdeadbeef

#define FIFO_POP_CMD_MSG        0x2a

static int
cmicd_fifodma_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICD);
}

static uint32_t
cmicd_schan_opcode_set(int unit, int op_code, int dst_blk, int acc_type,
                       int data_len, int dma, uint32_t ignore_mask)
{
    uint32_t oc = 0;

    SCMH_OPCODE_SET(oc, op_code);
    SCMH_DSTBLK_SET(oc, dst_blk);
    SCMH_ACCTYPE_SET(oc, acc_type);
    SCMH_DATALEN_SET(oc, data_len);
    SCMH_DMA_SET(oc, dma);
    SCMH_BANK_SET(oc, ignore_mask);

    return oc;
}

static void
cmicd_fifodma_data_process(cmicd_fifodma_test_param_t *p, uint32_t *addr, int ptr, int entries)
{
    int i;
    uint32_t j;

    for (i = 0; i < entries; i++) {
        for (j = 0; j < p->data.data_width; j++) {
            if (addr[j] == FIFODMA_DIRTY_DATA) {
                p->test_result = SHR_E_FAIL;
                return;
            }
        }
        ptr = (ptr + 1) % p->buf_entries;
        addr += p->data.data_width;
        if (ptr == 0) {
            addr = p->work.buf_cache;
        }
    }

    p->test_result = SHR_E_NONE;
}

static void
cmicd_fifodma_intr_cb(int unit, void *data)
{
    cmicd_fifodma_test_param_t *p = (cmicd_fifodma_test_param_t *)data;
    shr_thread_wake(p->handler_tc);
}

static int
cmicd_fifodma_instance_setup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_fifodma_instance_setup(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicd_fifodma_instance_trigger(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_fifodma_instance_trigger(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicd_fifodma_setup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (cmicd_fifodma_instance_setup(unit));

exit:
    SHR_FUNC_EXIT();
}

static void
cmicd_fifodma_handler(shr_thread_t tc, void *vp)
{
    cmicd_fifodma_test_param_t *p = (cmicd_fifodma_test_param_t *)vp;
    int unit = p->unit;
    uint32_t *buf, timeout, ptr, num;
    uint32_t i;
    int rv;

    rv = bcmbd_fifodma_start(unit, p->fifo_chan, &p->work);
    if (SHR_FAILURE(rv)) {
        return;
    }

    buf = p->work.buf_cache;
    for (i = 0; i < p->data.num_entries * p->data.data_width; i++) {
        buf[i] = FIFODMA_DIRTY_DATA;
    }
    timeout = p->intrmode ? 0 : p->ctrl_interval;

    while (1) {
        if (p->intrmode) {
            shr_thread_sleep(tc, p->ctrl_interval);
        }
        if (shr_thread_stopping(tc)) {
            break;
        }

        rv = bcmbd_fifodma_pop(unit, p->fifo_chan, timeout, &ptr, &num);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_EMPTY) {
                continue;
            } else {
                break;
            }
        }
        cmicd_fifodma_data_process(p, buf + ptr * p->data.data_width, ptr, num);
        if (p->debug) {
            LOG_CLI((BSL_META_U(unit, "Valid entries %"PRIu32"\n"), num));
        }
    }

    bcmbd_fifodma_stop(unit, p->fifo_chan);
}

static void
cmicd_fifodma_trigger(shr_thread_t tc, void *vp)
{
    cmicd_fifodma_test_param_t *p = (cmicd_fifodma_test_param_t *)vp;
    int unit = p->unit;

    while (1) {
        shr_thread_sleep(tc, p->ctrl_interval / 2);
        if (shr_thread_stopping(tc)) {
            break;
        }
        (void)cmicd_fifodma_instance_trigger(unit);
    }
}

static int
cmicd_fifodma_work_create(int unit, cmicd_fifodma_test_param_t *p)
{
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_symbols_t *symbols;
    bcmdrd_sym_info_t sinfo = {0};
    int blktype;
    struct mem_info_s {
        uint32_t acctype;
        uint32_t offset;
        int index;
        int blknum;
        int lane;
    } mi = {0};

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (!cinfo) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if (SHR_FAILURE(bcmdrd_pt_info_get_by_name(unit, p->sname, &sinfo))) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (!(sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Initialize FIFODMA work */
    blktype = bcmdrd_pt_blktype_get(unit, sinfo.sid, 0);
    mi.acctype = BCMBD_CMICD_BLKACC2ACCTYPE(sinfo.blktypes);
    mi.offset = sinfo.offset;
    mi.index = sinfo.index_min;
    mi.blknum = bcmdrd_chip_block_number(cinfo, blktype, 0);
    if (sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        mi.lane = -1;
    }

    p->data.start_addr = bcmbd_block_port_addr(unit, mi.blknum, mi.lane,
                                               mi.offset, mi.index);
    p->data.data_width = sinfo.entry_wsize;
    p->data.op_code = cmicd_schan_opcode_set(unit, FIFO_POP_CMD_MSG,
                                             mi.blknum, mi.acctype,
                                             sinfo.entry_wsize * sizeof(uint32_t),
                                             0, 0);
    p->data.num_entries = p->buf_entries;
    p->data.thresh_entries = p->data.num_entries / 10;

    p->work.data = &p->data;
    p->work.cb = cmicd_fifodma_intr_cb;
    p->work.cb_data = p;
    p->work.flags = p->intrmode ? FIFODMA_WF_INT_MODE : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
cmicd_fifodma_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    cmicd_fifodma_test_param_t *p = NULL;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Allocate the resource for savng test related parameters */
    SHR_ALLOC(p, sizeof(*p),"bcmaTestCaseCmicdFIFODMAPrm");
    if (p == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->sname = sal_strdup("L2_MOD_FIFOm");
    p->intrmode = 0;
    p->debug = 0;

    p->unit = unit;
    p->ctrl_interval = 100000;
    p->buf_entries = 1024;
    p->fifo_chan = 0;
    p->test_result = SHR_E_FAIL;

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "IntrMode", "bool",
                             &p->intrmode, NULL);
    bcma_cli_parse_table_add(&pt, "Debug", "bool",
                             &p->debug, NULL);

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

    /* Create the FIFODMA work */
    SHR_IF_ERR_EXIT(cmicd_fifodma_work_create(unit, p));

exit:
    SHR_FUNC_EXIT();
}

static void
cmicd_fifodma_help(int unit, void *bp)
{
    cmicd_fifodma_test_param_t *p = (cmicd_fifodma_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    cli_out("  IntrMode         - Interrupt Mode[0: Polling, 1: Intr]\n");
    cli_out("  Debug            - Debug Mode[0: False, 1: True]\n");
    cli_out("  ======================================================\n");
    cli_out("  sname            =%s\n", p->sname ? p->sname : "(null)");
    cli_out("  intrmode         =%"PRId32"\n", p->intrmode);
    cli_out("  debug            =%"PRId32"\n", p->debug);
    cli_out("  buf_entries      =%"PRId32"\n", p->buf_entries);
    cli_out("  fifo_chan        =%"PRId32"\n", p->fifo_chan);
    cli_out("  start_addr       =0x%"PRIx32"\n", p->data.start_addr);
    cli_out("  data_width       =%"PRId32"\n", p->data.data_width);
    cli_out("  opcode           =0x%"PRIx32"\n", p->data.op_code);
    cli_out("  num_entries      =%"PRId32"\n", p->data.num_entries);
    cli_out("  thresh_entries   =%"PRId32"\n", p->data.thresh_entries);

    return;
}

static void
cmicd_fifodma_recycle(int unit, void *bp)
{
    cmicd_fifodma_test_param_t *p = (cmicd_fifodma_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    if (p->sname) {
        SHR_FREE(p->sname);
    }
    return;
}

/*!
 * \brief FIFODMA write, read and compare test.
 *
 * This function is intend to test FIFODMA operation
 *
 * \param [in] unit Device Number.
 * \param [in] bp Buffer pointer for test case parameters.
 * \param [in] option TDB options.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_FAIL Comparing fails.
 * \return SHR_E_xxx Other failures.
 */
static int
cmicd_fifodma_test(int unit, void *bp, uint32_t option)
{
    cmicd_fifodma_test_param_t *p = (cmicd_fifodma_test_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    /* 0. Setup the FIFODMA instance. */
    SHR_IF_ERR_EXIT(cmicd_fifodma_setup(unit));

    /* 1. Start a handler thread to process FIFO DMA data of L2_MOD_FIFO */
    p->handler_tc = shr_thread_start("bcmaTestCaseCmicdFIFODMAHandler", -1,
                                     cmicd_fifodma_handler, (void *)p);
    if (p->handler_tc == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Could not start FIFODMA test handler thread.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* 2. Start a trigger thread to update L2_MOD_FIFO_RD_PTR and L2_MOD_FIFO_WR_PTR */
    p->trigger_tc = shr_thread_start("bcmaTestCaseCmicdFIFODMATrigger", -1,
                                     cmicd_fifodma_trigger, (void *)p);
    if (p->trigger_tc == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Could not start FIFODMA test trigger thread.\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_usleep(p->ctrl_interval * 20);

    /* 3. Record the test result */
    SHR_ERR_EXIT(p->test_result);

exit:
    /* 4. Terminate handler and trigger threads */
    if (p->handler_tc) {
        if (SHR_FAILURE(shr_thread_stop(p->handler_tc, 500000))) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "FIFODMA test handler thread will not exit.\n")));
        }
    }

    if (p->trigger_tc) {
        if (SHR_FAILURE(shr_thread_stop(p->trigger_tc, 500000))) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "FIFODMA test trigger thread will not exit.\n")));
        }
    }

    SHR_FUNC_EXIT();
}

#define BCMA_TEST_PROC_DESCRIPTION \
    "Execute the FIFO DMA test.\n" \
    "    0). Enable L2_INSERT in L2_MOD_FIFO_ENABLE.\n" \
    "    1). Start a handler thread to process FIFO DMA data of L2_MOD_FIFO.\n" \
    "    2). Start a trigger thread to update L2_MOD_FIFO_RD_PTR and L2_MOD_FIFO_WR_PTR.\n" \
    "    3). Record the test result.\n" \
    "    4). Terminate handler and trigger threads.\n" \
    ""

static bcma_test_proc_t cmicd_fifodma_proc[] = {
    {
        .desc = BCMA_TEST_PROC_DESCRIPTION,
        .cb = cmicd_fifodma_test,
    },
};

static bcma_test_op_t cmicd_fifodma_op = {
    .select = cmicd_fifodma_select,
    .parser = cmicd_fifodma_parser,
    .help = cmicd_fifodma_help,
    .recycle = cmicd_fifodma_recycle,
    .procs = cmicd_fifodma_proc,
    .proc_count = COUNTOF(cmicd_fifodma_proc),
};

bcma_test_op_t *
bcma_testcase_cmicd_fifodma_op_get(void)
{
    return &cmicd_fifodma_op;
}
