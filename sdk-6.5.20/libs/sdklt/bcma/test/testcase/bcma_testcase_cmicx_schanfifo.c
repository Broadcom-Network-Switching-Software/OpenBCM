/*! \file bcma_testcase_cmicx_schanfifo.c
 *
 * Broadcom test cases for CMICX SCHANFIFO access
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_schanfifo.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/util/bcma_testutil_symbols.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_schanfifo.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define SCMH_BANK_SET(d,v)      F32_SET(d,1,2,v)
#define SCMH_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define SCMH_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define SCMH_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define SCMH_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define SCMH_OPCODE_SET(d,v)    F32_SET(d,26,6,v)

#define MAX_TEST_SYMS                   8
#define MAX_SCHANFIFO_WORK              2
#define MAX_SCHANFIFO_POLLS             1000000
#define SCHANFIFO_ERSP_WSIZE            32

#define SCHANFIFO_MAGIC_DATA            0xdeadbeef

#define READ_MEMORY_CMD_MSG             0x07
#define WRITE_MEMORY_CMD_MSG            0x09

typedef struct cmds_info_s {
    int num_chan;
    size_t cmds_size;
    uint32_t num_cmds;
    uint32_t *req_buff[MAX_SCHANFIFO_WORK];
    uint32_t req_wsize[MAX_SCHANFIFO_WORK];
    uint64_t buff_paddr[MAX_SCHANFIFO_WORK];
    uint32_t entry_wsize[MAX_TEST_SYMS];
    uint32_t cmds_woffs[MAX_TEST_SYMS];
    int polls;
} cmds_info_t;

typedef struct cmicx_schanfifo_test_param_s {
    int num_syms;               /* Memory Count, */
    char *syms[MAX_TEST_SYMS];  /* Symbol Names */
    bool ccmdma;
    cmds_info_t cmds;
} cmicx_schanfifo_test_param_t;

static int
cmicx_syms_get(int unit, int num_syms, const char **sname)
{
    const bcmdrd_symbols_t *symbols;
    bcma_testutil_symbol_selector_t ss;
    bcmdrd_symbol_t s;
    bcmdrd_sym_info_t sinfo;
    int i;

    SHR_FUNC_ENTER(unit);

    /* Initialize the symbol selector */
    sal_memset(&ss, 0, sizeof(ss));

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ss.pflags |= BCMDRD_SYMBOL_FLAG_MEMORY | BCMDRD_SYMBOL_FLAG_CACHEABLE;

    ss.aflags |= BCMDRD_SYMBOL_FLAG_NOTEST | BCMDRD_SYMBOL_FLAG_READONLY |
                 BCMDRD_SYMBOL_FLAG_FIFO | BCMDRD_SYMBOL_FLAG_CAM |
                 BCMDRD_SYMBOL_FLAG_SOFT_PORT;

    for (i = 0; i < num_syms; i++) {
        do {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcma_testutil_symbol_get(symbols, &ss, &s));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmdrd_pt_info_get_by_name(unit, s.name, &sinfo));
        } while (sinfo.index_max > 0x1000 || sinfo.entry_wsize < 4);
        sname[i] = s.name;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_schanfifo_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX);
}

static uint32_t
cmicx_schan_opcode_set(int unit, int op_code, int dst_blk, int acc_type,
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

static int
cmicx_schanfifo_cmds_create(int unit, cmicx_schanfifo_test_param_t *p)
{
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_symbols_t *symbols;
    bcmdrd_sym_info_t sinfo = {0};
    uint32_t *cmds_buf = NULL;
    uint64_t buf_paddr;
    size_t cmds_buf_sz = 0;
    int blktype;
    struct mem_info_s {
        uint32_t acctype;
        uint32_t offset;
        int index;
        int blknum;
        int lane;
    } mi = {0};
    int cmd_msg, woffs;
    int i, j;
    uint32_t k;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    SHR_NULL_CHECK(cinfo, SHR_E_NOT_FOUND);

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    SHR_NULL_CHECK(symbols, SHR_E_FAIL);

    if (SHR_FAILURE(bcmbd_schanfifo_info_get(unit,
                                             &p->cmds.num_chan,
                                             &p->cmds.cmds_size))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    cmds_buf_sz = p->num_syms * SCHANFIFO_ERSP_WSIZE * sizeof(uint32_t);
    for (i = 0; i < MAX_SCHANFIFO_WORK; i++) {
        cmds_buf = NULL;
        buf_paddr = 0;
        if (p->ccmdma) {
            cmds_buf = bcmdrd_hal_dma_alloc(unit, cmds_buf_sz,
                                            "bcmaTestCaseSCHANFIFOCmdsBuf",
                                            &buf_paddr);
        } else {
            SHR_ALLOC(cmds_buf, cmds_buf_sz, "bcmaTestCaseSCHANFIFOCmdsBuf");
        }
        SHR_NULL_CHECK(cmds_buf, SHR_E_MEMORY);

        p->cmds.req_buff[i] = cmds_buf;
        p->cmds.buff_paddr[i] = buf_paddr;
        for (j = 0, woffs = 0; j < p->num_syms; j++) {
            if (SHR_FAILURE(bcmdrd_pt_info_get_by_name(unit, p->syms[j], &sinfo))) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Error: Unable to parse symbol: %s\n"),
                           p->syms[j]));
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
            if (!(sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
                SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
            }

            if (i == 0) {
                p->cmds.entry_wsize[j] = sinfo.entry_wsize;
                p->cmds.cmds_woffs[j] = woffs;
            }
            blktype = bcmdrd_pt_blktype_get(unit, sinfo.sid, 0);
            mi.acctype = BCMBD_CMICX_BLKACC2ACCTYPE(sinfo.blktypes);
            mi.offset = sinfo.offset;
            mi.index = sinfo.index_min;
            mi.blknum = bcmdrd_chip_block_number(cinfo, blktype, 0);
            if (sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
                mi.lane = -1;
            }
            cmd_msg = i == 0 ? WRITE_MEMORY_CMD_MSG : READ_MEMORY_CMD_MSG;
            cmds_buf[woffs + 0] = cmicx_schan_opcode_set(unit, cmd_msg,
                                                         mi.blknum, mi.acctype,
                                                         sinfo.entry_wsize *
                                                         sizeof(uint32_t),
                                                         0, 0);
            cmds_buf[woffs + 1] = bcmbd_block_port_addr(unit, mi.blknum, mi.lane,
                                                        mi.offset, mi.index);
            woffs += 2;
            if (i == 0) {
                for (k = 0; k < sinfo.entry_wsize; k++) {
                    cmds_buf[woffs + k] = SCHANFIFO_MAGIC_DATA + k;
                }
                woffs += sinfo.entry_wsize;
            }
        }
        p->cmds.req_wsize[i] = woffs;
    }

    p->cmds.num_cmds = p->num_syms;
    p->cmds.polls = MAX_SCHANFIFO_POLLS;

exit:
    if (SHR_FUNC_ERR()) {
        for (i = 0; i < MAX_SCHANFIFO_WORK; i++) {
            if (p->ccmdma) {
                if (p->cmds.req_buff[i]) {
                    bcmdrd_hal_dma_free(unit, cmds_buf_sz,
                                        p->cmds.req_buff[i],
                                        p->cmds.buff_paddr[i]);
                    p->cmds.req_buff[i] = NULL;
                    p->cmds.buff_paddr[i] = 0;
                }
            } else {
                SHR_FREE(p->cmds.req_buff[i]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

static int
cmicx_schanfifo_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    const char *syms[MAX_TEST_SYMS] = {NULL};
    cmicx_schanfifo_test_param_t *p = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Allocate the resource for savng test related parameters */
    SHR_ALLOC(p, sizeof(*p),"bcmaTestCaseCmicxSCHANFIFOPrm");
    if (p == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->num_syms = 4;

    /* Get the symbol names. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cmicx_syms_get(unit, MAX_TEST_SYMS, syms));

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "CCMDMA", "bool",
                             &p->ccmdma, NULL);
    bcma_cli_parse_table_add(&pt, "NumMem", "int",
                             &p->num_syms, NULL);
    bcma_cli_parse_table_add(&pt, "Mem", "str",
                             &syms[0], NULL);
    bcma_cli_parse_table_add(&pt, "Mem0", "str",
                             &syms[0], NULL);
    bcma_cli_parse_table_add(&pt, "Mem1", "str",
                             &syms[1], NULL);
    bcma_cli_parse_table_add(&pt, "Mem2", "str",
                             &syms[2], NULL);
    bcma_cli_parse_table_add(&pt, "Mem3", "str",
                             &syms[3], NULL);
    bcma_cli_parse_table_add(&pt, "Mem4", "str",
                             &syms[4], NULL);
    bcma_cli_parse_table_add(&pt, "Mem5", "str",
                             &syms[5], NULL);
    bcma_cli_parse_table_add(&pt, "Mem6", "str",
                             &syms[6], NULL);
    bcma_cli_parse_table_add(&pt, "Mem7", "str",
                             &syms[7], NULL);

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
    if (syms[0]) {
        p->syms[0] = sal_strdup(syms[0]);
    }
    if (syms[1]) {
        p->syms[1] = sal_strdup(syms[1]);
    }
    if (syms[2]) {
        p->syms[2] = sal_strdup(syms[2]);
    }
    if (syms[3]) {
        p->syms[3] = sal_strdup(syms[3]);
    }
    if (syms[4]) {
        p->syms[4] = sal_strdup(syms[4]);
    }
    if (syms[5]) {
        p->syms[5] = sal_strdup(syms[5]);
    }
    if (syms[6]) {
        p->syms[6] = sal_strdup(syms[6]);
    }
    if (syms[7]) {
        p->syms[7] = sal_strdup(syms[7]);
    }

    /* Clear parse table */
    bcma_cli_parse_table_done(&pt);

    if (p->num_syms < 1 || p->num_syms > MAX_TEST_SYMS) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: memory num is out of range \n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (cmicx_schanfifo_cmds_create(unit, p));

exit:
    SHR_FUNC_EXIT();
}

static void
cmicx_schanfifo_help(int unit, void *bp)
{
    cmicx_schanfifo_test_param_t *p = (cmicx_schanfifo_test_param_t *)bp;
    int i, woffs;

    if (p == NULL) {
        return;
    }
    cli_out("  CCMDMA           - %s\n", p->ccmdma ? "Enabled" : "Disabled");
    cli_out("  NumMem           - Number of test memories[1...5]\n");
    cli_out("  Mem, Mem[0...7]  - Memory name\n");
    cli_out("  ======================================================\n");
    cli_out("  NumMem   =%"PRId32"\n", p->num_syms);

    for (i = 0; i < p->num_syms; i++) {
        woffs = p->cmds.cmds_woffs[i];
        cli_out("  ========Mem%"PRId32" %s WR Info=========\n", i, p->syms[i]);
        cli_out("  opcode       =0x%08"PRIx32"\n", p->cmds.req_buff[0][woffs + 0]);
        cli_out("  start_addr   =0x%08"PRIx32"\n", p->cmds.req_buff[0][woffs + 1]);
        cli_out("  data_width   =%"PRId32"\n", p->cmds.entry_wsize[i]);
        woffs = 2;
        cli_out("  ========Mem%"PRId32" %s RD Info=========\n", i, p->syms[i]);
        cli_out("  opcode       =0x%08"PRIx32"\n", p->cmds.req_buff[1][woffs + 0]);
        cli_out("  start_addr   =0x%08"PRIx32"\n", p->cmds.req_buff[1][woffs + 1]);
        cli_out("  data_width   =%"PRId32"\n", p->cmds.entry_wsize[i]);
    }

    return;
}

static void
cmicx_schanfifo_recycle(int unit, void *bp)
{
    cmicx_schanfifo_test_param_t *p = (cmicx_schanfifo_test_param_t *)bp;
    size_t cmds_buf_sz;
    int i;
    if (p == NULL) {
        return;
    }
    if (p->syms[0]) {
        SHR_FREE(p->syms[0]);
    }
    if (p->syms[1]) {
        SHR_FREE(p->syms[1]);
    }
    if (p->syms[2]) {
        SHR_FREE(p->syms[2]);
    }
    if (p->syms[3]) {
        SHR_FREE(p->syms[3]);
    }
    if (p->syms[4]) {
        SHR_FREE(p->syms[4]);
    }
    if (p->syms[5]) {
        SHR_FREE(p->syms[5]);
    }
    if (p->syms[6]) {
        SHR_FREE(p->syms[6]);
    }
    if (p->syms[7]) {
        SHR_FREE(p->syms[7]);
    }

    cmds_buf_sz = p->num_syms * SCHANFIFO_ERSP_WSIZE * sizeof(uint32_t);
    for (i = 0; i < MAX_SCHANFIFO_WORK; i++) {
        if (p->ccmdma) {
            if (p->cmds.req_buff[i]) {
                bcmdrd_hal_dma_free(unit, cmds_buf_sz,
                                    p->cmds.req_buff[i],
                                    p->cmds.buff_paddr[i]);
                p->cmds.req_buff[i] = NULL;
                p->cmds.buff_paddr[i] = 0;
            }
        } else {
            SHR_FREE(p->cmds.req_buff[i]);
        }
    }

    return;
}

/*!
 * \brief SCHANFIFO write, read and compare test.
 *
 * This function is intend to test SCHANFIFO operation
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
cmicx_schanfifo_test(int unit, void *bp, uint32_t option)
{
    uint32_t mask[BCMDRD_MAX_PT_WSIZE];
    cmicx_schanfifo_test_param_t *p = (cmicx_schanfifo_test_param_t *)bp;
    bcmdrd_sid_t sid;
    uint32_t done_cmds;
    uint32_t *resp_buff;
    uint32_t *data, *resp;
    uint32_t i;
    uint32_t flags;
    uint64_t buff_paddr;
    int chan;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    /* 0. Initialize SCHAN FIFO */
    flags = SCHANFIFO_IF_POLL_WAIT | SCHANFIFO_IF_IGNORE_SER_ABORT;
    if (p->ccmdma) {
        flags |= SCHANFIFO_IF_CCMDMA_WR;
    }

    SHR_IF_ERR_EXIT
        (bcmbd_schanfifo_init(unit, p->cmds.polls, flags));

    /* 1. Send write operations to SCHAN FIFO channel */
    chan = 0;

    if (p->ccmdma) {
        buff_paddr = p->cmds.buff_paddr[chan];
        flags = SCHANFIFO_OF_SET_START | SCHANFIFO_OF_DIS_RESP;

    } else {
        buff_paddr = 0;
        flags = SCHANFIFO_OF_SET_START;
    }

    SHR_IF_ERR_EXIT
        (bcmbd_schanfifo_ops_send(unit, chan, p->cmds.num_cmds,
                                  p->cmds.req_buff[chan],
                                  p->cmds.req_wsize[chan],
                                  buff_paddr,
                                  flags));

    /* 2. Wait complete status in SCHAN FIFO channel */
    flags = SCHANFIFO_OF_WAIT_COMPLETE | SCHANFIFO_OF_CLEAR_START;
    SHR_IF_ERR_EXIT(
        bcmbd_schanfifo_status_get(unit, chan, p->cmds.num_cmds,
                                   flags, &done_cmds, &resp_buff));
    if (done_cmds != p->cmds.num_cmds) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Chan %d done not equal to request.\n"),
                  chan));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* 3. Send read operations to SCHAN FIFO channel */
    chan = 1;

    flags = SCHANFIFO_OF_SET_START;
    SHR_IF_ERR_EXIT
        (bcmbd_schanfifo_ops_send(unit, chan, p->cmds.num_cmds,
                                  p->cmds.req_buff[chan],
                                  p->cmds.req_wsize[chan],
                                  0,
                                  flags));

    /* 4. Wait complete status in SCHAN FIFO channel */
    flags = SCHANFIFO_OF_WAIT_COMPLETE | SCHANFIFO_OF_CLEAR_START;
    SHR_IF_ERR_EXIT
        (bcmbd_schanfifo_status_get(unit, chan, p->cmds.num_cmds,
                                    flags, &done_cmds, &resp_buff));
    if (done_cmds != p->cmds.num_cmds) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Chan %d done not equal to request.\n"),
                  chan));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* 5. Compare wrtie and read entry data */
    for (i = 0; i < p->cmds.num_cmds; i++) {
        data = p->cmds.req_buff[0] + p->cmds.cmds_woffs[i] + 2;
        resp = resp_buff + i * SCHANFIFO_ERSP_WSIZE + 1;

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_name_to_sid(unit, p->syms[i], &sid));

        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_mask_get(unit, sid, 0, mask,
                                       p->cmds.entry_wsize[i], TRUE));

        if (bcma_testutil_pt_data_comp(data, resp, mask, p->cmds.entry_wsize[i])
           != 0) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Memory %s compare data fail.\n"),
                      p->syms[i]));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

#define BCMA_TEST_PROC_DESCRIPTION \
    "Execute the SCHAN FIFO test.\n" \
    "   0). Initialize SCHAN FIFO.\n" \
    "   1). Send write operations to SCHAN FIFO channel.\n" \
    "   2). Wait complete status in SCHAN FIFO channel.\n" \
    "   3). Send read operations to SCHAN FIFO channel.\n" \
    "   4). Wait complete status in SCHAN FIFO channel.\n" \
    "   5). Compare wrtie and read entry data.\n" \
    ""

static bcma_test_proc_t cmicx_schanfifo_proc[] = {
    {
        .desc = BCMA_TEST_PROC_DESCRIPTION,
        .cb = cmicx_schanfifo_test,
    },
};

static bcma_test_op_t cmicx_schanfifo_op = {
    .select = cmicx_schanfifo_select,
    .parser = cmicx_schanfifo_parser,
    .help = cmicx_schanfifo_help,
    .recycle = cmicx_schanfifo_recycle,
    .procs = cmicx_schanfifo_proc,
    .proc_count = COUNTOF(cmicx_schanfifo_proc),
};

bcma_test_op_t *
bcma_testcase_cmicx_schanfifo_op_get(void)
{
    return &cmicx_schanfifo_op;
}
