/*! \file bcma_testcase_cmicd_sbusdma_batch.c
 *
 * Broadcom test cases for CMICD SBUSDMA Batch access
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
#include <sal/sal_sem.h>

#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_cmicd_mem.h>
#include <bcmbd/bcmbd_sbusdma.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_symbols.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_sbusdma_batch.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define SCMH_BANK_SET(d,v)      F32_SET(d,1,2,v)
#define SCMH_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define SCMH_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define SCMH_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define SCMH_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define SCMH_OPCODE_SET(d,v)    F32_SET(d,26,6,v)

#define MAX_TEST_MEMS                 5
#define MAX_SBUSDMA_WORK              2

typedef struct cmicd_sbusdma_batch_test_param_s {
    int num;                    /* Memory Count, */
    char *mems[MAX_TEST_MEMS];  /* Symbol Names */
    int intrmode;               /* 0:Pulling, 1:Intr, Default=0 */
    int clean;                  /* 0:False, 1:True, Default=0 */
    int async;                  /* 0:False, 1:True, Default=0 */
    int overlap;                /* 0:False, 1:True, Default=0 */
    int skipover;               /* 0:False, 1:True, Default=0 */
    int jumpto;                 /* 0:False, 1:True, Default=0 */

    sal_sem_t sem_sync;
    bcmbd_sbusdma_data_t *data[MAX_SBUSDMA_WORK];
    bcmbd_sbusdma_work_t work[MAX_SBUSDMA_WORK];
    void *buf[MAX_SBUSDMA_WORK][MAX_TEST_MEMS];
    uint64_t buf_paddr[MAX_SBUSDMA_WORK][MAX_TEST_MEMS];
    uint32_t buf_size[MAX_TEST_MEMS];
} cmicd_sbusdma_batch_test_param_t;

#define SBUSDMA_MAGIC_DATA1     0xa5
#define SBUSDMA_MAGIC_DATA2     0x5a

#define READ_MEMORY_CMD_MSG     0x07
#define WRITE_MEMORY_CMD_MSG    0x09

/*!
 * \brief Get the first available symbol name.
 *
 * This function is for getting the first available symbol name.
 *
 * \param [in] unit Device Number.
 * \param [out] sname Symbol Name.
 *
 * \retval SHR_E_NONE Get the symbol name successfully.
 * \retval SHR_E_INTERNAL Symbol table is emty.
 * \retval SHR_E_NOT_FOUND Not found the available symbol.
 */
static int
cmicd_sbusdma_batch_mems_get(int unit, int mem_cnt, const char **sname)
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

    for (i = 0; i < mem_cnt; i++) {
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
cmicd_sbusdma_batch_select(int unit)
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
cmicd_sbusdma_cb(int unit, void *data)
{
    cmicd_sbusdma_batch_test_param_t *p = (cmicd_sbusdma_batch_test_param_t *)data;
    sal_sem_give(p->sem_sync);
}

static int
cmicd_sbusdma_batch_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    const char *mems[MAX_TEST_MEMS] = {NULL};
    cmicd_sbusdma_batch_test_param_t *p = NULL;
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
    int i;
    /* Always use table instance 0 */
    int tbl_inst = 0;
    uint32_t pipe_info, pmask, linst, sect_size;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Allocate the resource for savng test related parameters */
    SHR_ALLOC(p, sizeof(*p),"bcmaTestCaseCmicdSBUSDMABatchPrm");
    if (p == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->num = 5;
    p->intrmode = 0;
    p->clean = 0;
    p->async = 0;
    p->overlap = 0;
    p->skipover = 0;
    p->jumpto = 0;

    /* Get the first available symbol name. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cmicd_sbusdma_batch_mems_get(unit, MAX_TEST_MEMS, mems));

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "NumMem", "int",
                             &p->num, NULL);
    bcma_cli_parse_table_add(&pt, "Mem", "str",
                             &mems[0], NULL);
    bcma_cli_parse_table_add(&pt, "Mem0", "str",
                             &mems[0], NULL);
    bcma_cli_parse_table_add(&pt, "Mem1", "str",
                             &mems[1], NULL);
    bcma_cli_parse_table_add(&pt, "Mem2", "str",
                             &mems[2], NULL);
    bcma_cli_parse_table_add(&pt, "Mem3", "str",
                             &mems[3], NULL);
    bcma_cli_parse_table_add(&pt, "Mem4", "str",
                             &mems[4], NULL);

    bcma_cli_parse_table_add(&pt, "IntrMode", "bool",
                             &p->intrmode, NULL);
    bcma_cli_parse_table_add(&pt, "Clean", "bool",
                             &p->clean, NULL);
    bcma_cli_parse_table_add(&pt, "Async", "bool",
                             &p->async, NULL);
    bcma_cli_parse_table_add(&pt, "Overlap", "bool",
                             &p->overlap, NULL);
    bcma_cli_parse_table_add(&pt, "SkipOver", "bool",
                             &p->skipover, NULL);
    bcma_cli_parse_table_add(&pt, "JumpTo", "bool",
                             &p->jumpto, NULL);

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
    if (mems[0]) {
        p->mems[0] = sal_strdup(mems[0]);
    }
    if (mems[1]) {
        p->mems[1] = sal_strdup(mems[1]);
    }
    if (mems[2]) {
        p->mems[2] = sal_strdup(mems[2]);
    }
    if (mems[3]) {
        p->mems[3] = sal_strdup(mems[3]);
    }
    if (mems[4]) {
        p->mems[4] = sal_strdup(mems[4]);
    }

    /* Clear parse table */
    bcma_cli_parse_table_done(&pt);

    if (p->num < 1 || p->num > MAX_TEST_MEMS) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: memory num is out of range \n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    p->sem_sync = sal_sem_create("bcmaTestCaseCmicdSBUSDMABatch", SAL_SEM_BINARY, 0);
    if (!p->sem_sync) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Initialize the sbusdma work */
    p->data[0] = sal_alloc(sizeof(bcmbd_sbusdma_data_t) * MAX_SBUSDMA_WORK * p->num,
                           "bcmaTestCaseCmicdSBUSDMABatchWorkData");
    if (!p->data[0]) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(p->data[0], 0, sizeof(bcmbd_sbusdma_data_t) * MAX_SBUSDMA_WORK * p->num);
    p->data[1] = &(p->data[0][p->num]);

    /* Structure the work data */
    for (i = 0; i < p->num; i++) {
        if (p->mems[i] == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error: Should specify a memory name for Mem%d\n"), i));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Check the memory */
        if (SHR_FAILURE(bcmdrd_pt_info_get_by_name(unit, p->mems[i], &sinfo))) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        if (!(sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
        }

        /* Set up the memory information */
        blktype = bcmdrd_pt_blktype_get(unit, sinfo.sid, 0);
        mi.acctype = BCMBD_CMICD_BLKACC2ACCTYPE(sinfo.blktypes);
        mi.offset = sinfo.offset;
        mi.index = sinfo.index_min;
        mi.blknum = bcmdrd_chip_block_number(cinfo, blktype, 0);
        if (sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
            mi.lane = -1;
        }

        pipe_info = bcmbd_cmicd_pipe_info(unit, mi.offset, mi.acctype,
                                          blktype, -1);
        /* pmask > 0 if (access type == UNIQUE && (block type == IP/EP/MMU_ITM/MMU_EB)) */
        pmask = BCMBD_CMICD_PIPE_PMASK(pipe_info);
        /* linst > 0 if (block type == MMU_ITM/MMU_EB/MMU_GLB), indicate the number of base index */
        linst = BCMBD_CMICD_PIPE_LINST(pipe_info);
        sect_size = BCMBD_CMICD_PIPE_SECT_SIZE(pipe_info);
        if (pmask != 0) {
            /* Select a specific block instance for unique access type */
            mi.acctype = tbl_inst;
        }
        if (linst != 0 && sect_size != 0) {
            mi.offset += tbl_inst * sect_size;
        }

        /* Allocate the DMA buffer */
        if (p->overlap) {
            p->buf_size[0] += (sinfo.index_max - sinfo.index_min + 1) *
                               sinfo.entry_wsize * sizeof(uint32_t);
            p->data[0][i].attrs = p->data[1][i].attrs = SBUSDMA_DA_APND_BUF;
        } else {
            p->buf_size[i] = (sinfo.index_max - sinfo.index_min + 1) *
                              sinfo.entry_wsize * sizeof(uint32_t);
            p->buf[0][i] = bcmdrd_hal_dma_alloc(unit, p->buf_size[i],
                                                "bcmaTestCaseCmicdSBUSDMABatchDataBuffer0",
                                                &p->buf_paddr[0][i]);
            if (!p->buf[0][i]) {
                SHR_IF_ERR_EXIT(SHR_E_MEMORY);
            }
            sal_memset(p->buf[0][i], SBUSDMA_MAGIC_DATA1, p->buf_size[i]);
            p->buf[1][i] = bcmdrd_hal_dma_alloc(unit, p->buf_size[i],
                                                "bcmaTestCaseCmicdSBUSDMABatchDataBuffer1",
                                                &p->buf_paddr[1][i]);
            if (!p->buf[1][i]) {
                SHR_IF_ERR_EXIT(SHR_E_MEMORY);
            }
            sal_memset(p->buf[1][i], SBUSDMA_MAGIC_DATA1, p->buf_size[i]);
        }

        /* Configure the work data */
        p->data[0][i].op_code = cmicd_schan_opcode_set(unit, READ_MEMORY_CMD_MSG,
                                                       mi.blknum, mi.acctype,
                                                       sinfo.entry_wsize * sizeof(uint32_t),
                                                       0, 0);
        p->data[1][i].op_code = cmicd_schan_opcode_set(unit, WRITE_MEMORY_CMD_MSG,
                                                       mi.blknum, mi.acctype,
                                                       sinfo.entry_wsize * sizeof(uint32_t),
                                                       0, 0);
        p->data[0][i].buf_paddr = p->buf_paddr[0][i];
        p->data[1][i].buf_paddr = p->buf_paddr[1][i];
        p->data[0][i].start_addr = p->data[1][i].start_addr =
            bcmbd_block_port_addr(unit, mi.blknum, mi.lane, mi.offset, mi.index);
        p->data[0][i].data_width = p->data[1][i].data_width = sinfo.entry_wsize;
        p->data[0][i].addr_gap = p->data[1][i].addr_gap = 0;
        p->data[0][i].op_count = p->data[1][i].op_count = sinfo.index_max - sinfo.index_min + 1;
    }

    if (p->overlap) {
        p->buf[0][0] = bcmdrd_hal_dma_alloc(unit, p->buf_size[0],
                                            "bcmaTestCaseCmicdSBUSDMABatchDataBuffer0",
                                            &p->buf_paddr[0][0]);
        if (!p->buf[0][0]) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }
        sal_memset(p->buf[0][0], SBUSDMA_MAGIC_DATA2, p->buf_size[0]);
        p->buf[1][0] = bcmdrd_hal_dma_alloc(unit, p->buf_size[0],
                                            "bcmaTestCaseCmicdSBUSDMABatchDataBuffer1",
                                            &p->buf_paddr[1][0]);
        if (!p->buf[1][0]) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }
        sal_memset(p->buf[1][0], SBUSDMA_MAGIC_DATA2, p->buf_size[0]);
        p->data[0][0].buf_paddr = p->buf_paddr[0][0];
        p->data[1][0].buf_paddr = p->buf_paddr[1][0];
    }

    /* Mark skip over or jump to */
    if (p->skipover && p->num >= 3) {
        p->data[0][1].attrs |= SBUSDMA_DA_SKIP_OVER;
        p->data[1][1].attrs |= SBUSDMA_DA_SKIP_OVER;
    }
    if (p->jumpto && p->num >= 5) {
        p->data[0][3].jump_item = p->data[1][3].jump_item = p->num - 1;
        p->data[0][3].attrs |= SBUSDMA_DA_JUMP_TO;
        p->data[1][3].attrs |= SBUSDMA_DA_JUMP_TO;
    }

    /* Structure the read work */
    p->work[0].data = p->data[0];
    p->work[0].items = p->num;
    if (p->async) {
        p->work[0].cb = cmicd_sbusdma_cb;
        p->work[0].cb_data = p;
    }
    p->work[0].flags = SBUSDMA_WF_READ_CMD;
    if (p->intrmode) {
        p->work[0].flags |= SBUSDMA_WF_INT_MODE;
    }

    /* Structure the write work */
    p->work[1].data = p->data[1];
    p->work[1].items = p->num;
    if (p->async) {
        p->work[1].cb = cmicd_sbusdma_cb;
        p->work[1].cb_data = p;
    }
    p->work[1].flags = SBUSDMA_WF_WRITE_CMD;
    if (p->intrmode) {
        p->work[1].flags |= SBUSDMA_WF_INT_MODE;
    }

    /* Create the work */
    SHR_IF_ERR_EXIT
        (bcmbd_sbusdma_batch_work_create(unit, &p->work[0]));
    SHR_IF_ERR_EXIT
        (bcmbd_sbusdma_batch_work_create(unit, &p->work[1]));

exit:
    SHR_FUNC_EXIT();
}

static void
cmicd_sbusdma_batch_help(int unit, void *bp)
{
    cmicd_sbusdma_batch_test_param_t *p = (cmicd_sbusdma_batch_test_param_t *)bp;
    int i;
    if (p == NULL) {
        return;
    }
    cli_out("  NumMem           - Number of test memories[1...5]\n");
    cli_out("  Mem, Mem[0...4]  - Memory name\n");
    cli_out("  IntrMode         - Interrupt Mode[0: Polling, 1: Intr]\n");
    cli_out("  Clean            - Clean Mode[0: False, 1: True]\n");
    cli_out("  Async            - Async Mode[0: False, 1: True]\n");
    cli_out("  Overlap          - Overlap Mode[0: False, 1: True]\n");
    cli_out("  Skipover         - Skipover the 2nd mem(valid when NumMem >=3)\n"
            "                     [0: False, 1: True]\n");
    cli_out("  Jumpto           - The 4th mem will jump to the last\n"
            "                     (valid when NumMem is 5)[0: False, 1: True]\n");
    cli_out("  ======================================================\n");
    cli_out("  NumMem   =%"PRId32"\n", p->num);
    cli_out("  IntrMode =%"PRId32"\n", p->intrmode);
    cli_out("  Clean    =%"PRId32"\n", p->clean);
    cli_out("  Async    =%"PRId32"\n", p->async);
    cli_out("  Overlap  =%"PRId32"\n", p->overlap);
    cli_out("  SkipOver =%"PRId32"\n", p->skipover);
    cli_out("  JumpTo   =%"PRId32"\n", p->jumpto);

    for (i = 0; i < p->num; i++) {
        cli_out("  ========Mem%"PRId32" %s RD Info=========\n", i, p->mems[i]);
        cli_out("  opcode       =0x%"PRIx32"\n", p->data[0][i].op_code);
        cli_out("  buf_paddr    =0x%"PRIx64"\n", p->data[0][i].buf_paddr);
        cli_out("  start_addr   =0x%"PRIx32"\n", p->data[0][i].start_addr);
        cli_out("  data_width   =%"PRId32"\n", p->data[0][i].data_width);
        cli_out("  op_count     =%"PRId32"\n", p->data[0][i].op_count);
        cli_out("  attrs        =0x%"PRIx32"\n", p->data[0][i].attrs);
        cli_out("  ========Mem%"PRId32" %s WR Info=========\n", i, p->mems[i]);
        cli_out("  opcode       =0x%"PRIx32"\n", p->data[1][i].op_code);
        cli_out("  buf_paddr    =0x%"PRIx64"\n", p->data[1][i].buf_paddr);
        cli_out("  start_addr   =0x%"PRIx32"\n", p->data[1][i].start_addr);
        cli_out("  data_width   =%"PRId32"\n", p->data[1][i].data_width);
        cli_out("  op_count     =%"PRId32"\n", p->data[1][i].op_count);
        cli_out("  attrs        =0x%"PRIx32"\n", p->data[1][i].attrs);
    }

    return;
}

static void
cmicd_sbusdma_batch_recycle(int unit, void *bp)
{
    cmicd_sbusdma_batch_test_param_t *p = (cmicd_sbusdma_batch_test_param_t *)bp;
    int i, j;
    if (p == NULL) {
        return;
    }
    if (p->mems[0]) {
        SHR_FREE(p->mems[0]);
    }
    if (p->mems[1]) {
        SHR_FREE(p->mems[1]);
    }
    if (p->mems[2]) {
        SHR_FREE(p->mems[2]);
    }
    if (p->mems[3]) {
        SHR_FREE(p->mems[3]);
    }
    if (p->mems[4]) {
        SHR_FREE(p->mems[4]);
    }

    /* Delete the work */
    bcmbd_sbusdma_batch_work_delete(unit, &p->work[0]);
    bcmbd_sbusdma_batch_work_delete(unit, &p->work[1]);

    for (i = 0; i < MAX_SBUSDMA_WORK; i++) {
        for (j = 0; j < p->num; j++) {
            if (p->buf[i][j] && p->buf_paddr[i][j] && p->buf_size[j]) {
                bcmdrd_hal_dma_free(unit, p->buf_size[j], p->buf[i][j], p->buf_paddr[i][j]);
            }
        }
    }
    if (p->data[0]) {
        SHR_FREE(p->data[0]);
    }

    if (p->sem_sync) {
        sal_sem_destroy(p->sem_sync);
        p->sem_sync = NULL;
    }
    return;
}

/*!
 * \brief Compare with two entries.
 *
 * This function is to compare with two entries.
 *
 * \param [in] dst Dest entries pointer.
 * \param [in] src Source entries pointer.
 * \param [in] wsize Size of entry in 32-bit words.
 * \param [in] count Count of entries to compare.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_FAIL Comparing fails.
 */
static int
sbusdma_entries_cmp(char *dst, char *src, uint32_t wsize, int cnt)
{
    int i, off, len;

    len = (wsize - 1) * sizeof(uint32_t);
    for (i = 0; i < cnt; i++) {
        off = i * wsize * sizeof(uint32_t);
        if (sal_memcmp(&dst[off], &src[off], len)) {
            return SHR_E_FAIL;
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Sbusdma write, read and compare test.
 *
 * This function is intend to test sbusdma write operation
 * for a specific memory.
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
cmicd_sbusdma_batch_test(int unit, void *bp, uint32_t option)
{
    cmicd_sbusdma_batch_test_param_t *p = (cmicd_sbusdma_batch_test_param_t *)bp;
    char *dst, *src;
    uint32_t wsize, cnt;
    int clean;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    clean = p->clean;
next_round:
    /* Submit the work for write */
    if (p->async) {
        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_work_add(unit, &p->work[1]));
        sal_sem_take(p->sem_sync, SAL_SEM_FOREVER);
        if (p->work[1].state != SBUSDMA_WORK_DONE) {
            SHR_IF_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_work_execute(unit, &p->work[1]));
    }

    /* Submit the work for read */
    if (p->async) {
        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_work_add(unit, &p->work[0]));
        sal_sem_take(p->sem_sync, SAL_SEM_FOREVER);
        if (p->work[0].state != SBUSDMA_WORK_DONE) {
            SHR_IF_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_work_execute(unit, &p->work[0]));
    }

    /* Check the data */
    dst = (char *)p->buf[0][0];
    src = (char *)p->buf[1][0];
    for (i = 0; i < p->num; i++) {
        if (p->data[0][i].attrs & SBUSDMA_DA_SKIP_OVER) {
            continue;
        }
        if (p->data[0][i].attrs & SBUSDMA_DA_JUMP_TO) {
            i = p->data[0][i].jump_item - 1;
            continue;
        }
        wsize = p->data[0][i].data_width;
        cnt = p->data[0][i].op_count;
        if (!(p->overlap)) {
            dst = (char *)p->buf[0][i];
            src = (char *)p->buf[1][i];
        }
        SHR_IF_ERR_EXIT
            (sbusdma_entries_cmp(dst, src, wsize, cnt));
        if (p->overlap) {
            dst += wsize * cnt * sizeof(uint32_t);
            src += wsize * cnt * sizeof(uint32_t);
        }
    }

    /* Structure the clean work */
    if (clean) {
        if (p->overlap) {
            sal_memset(p->buf[1][0], 0, p->buf_size[0]);
        } else {
            for (i = 0; i < p->num; i++) {
                sal_memset(p->buf[1][i], 0, p->buf_size[i]);
            }
        }
        p->work[1].flags |= SBUSDMA_WF_SGL_DATA;
        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_batch_work_delete(unit, &p->work[1]));
        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_batch_work_create(unit, &p->work[1]));
        clean = 0;
        goto next_round;
    }

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t cmicd_sbusdma_batch_proc[] = {
    {
        .desc = "run SBUS DMA batch access test.\n",
        .cb = cmicd_sbusdma_batch_test,
    },
};

static bcma_test_op_t cmicd_sbusdma_batch_op = {
    .select = cmicd_sbusdma_batch_select,
    .parser = cmicd_sbusdma_batch_parser,
    .help = cmicd_sbusdma_batch_help,
    .recycle = cmicd_sbusdma_batch_recycle,
    .procs = cmicd_sbusdma_batch_proc,
    .proc_count = COUNTOF(cmicd_sbusdma_batch_proc),
};

bcma_test_op_t *
bcma_testcase_cmicd_sbusdma_batch_op_get(void)
{
    return &cmicd_sbusdma_batch_op;
}
