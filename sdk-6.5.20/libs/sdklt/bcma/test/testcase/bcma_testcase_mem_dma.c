/*! \file bcma_testcase_mem_dma.c
 *
 * Test case for memory DMA.
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
#include <sal/sal_time.h>

#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/util/bcma_testutil_pt_iterate.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_progress.h>
#include <bcma/test/testcase/bcma_testcase_mem_dma.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define MEM_DMA_MAX_ENTRY_NUM 60000

#define MEM_DMA_PARAM_HELP \
    "  Memory       - The memory to be tested\n"\
    "  IndexStart   - Starting index\n"\
    "  IndexEnd     - Ending index\n"\
    "  DmaLowToHigh - Bottom-to-top fill\n"\
    "  DmaRead      - Read memory in DMA mode\n"\
    "  SEED         - The seed to use in the random function\n"\
    "  PipeStart    - Starting pipe\n"\
    "  PipeEnd      - Ending pipe\n"\
    "  BlockType    - The block type which the memories belong to\n"\
    "\nExample:\n"\
    "  tr 71 m=L2Xm is=min ie=max dlth=1 dr=0 seed=0x00decade\n"\
    "  tr 71\n"

typedef struct mem_dma_param_s {
    /* Memory name */
    char name[BCMA_TESTUTIL_STR_MAX_SIZE];

    /* Starting and ending index */
    int index_start;
    int index_end;

    /* Starting and ending table instance */
    int inst_start;
    int inst_end;

    int blktype;

    /* DMA configuration */
    int dma_read;
    int dma_low2high;

    /* Buffer for write/read/mask data */
    uint32_t mask[BCMDRD_MAX_PT_WSIZE];
    uint32_t *wmem;
    uint64_t waddr;
    uint32_t *rmem;
    uint64_t raddr;

    /* Seed to generate random data */
    uint32_t seed;
    uint32_t rand_data;

    /* Number of memories which can not get expected value */
    int err_cnt;

    /* Number of sid */
    size_t num_of_sid;

    /* progress report control */
    bcma_testutil_progress_ctrl_t *pc;
} mem_dma_param_t;

static int
mem_dma_select(int unit)
{
    return true;
}

static int
mem_dma_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                    mem_dma_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    char *name = NULL, *index_start = NULL, *index_end = NULL;
    char *inst_start = NULL, *inst_end = NULL, *blktype = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    param->seed = 0xdecade;
    param->dma_low2high = 1;
    param->dma_read = 1;
    param->index_start = BCMA_TESTUTIL_TABLE_MIN_INDEX;
    param->index_end = BCMA_TESTUTIL_TABLE_MAX_INDEX;
    param->inst_start = -1;
    param->inst_end = -1;
    param->blktype = -1;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Memory", "str", &name, NULL);
    bcma_cli_parse_table_add(&pt, "IndexStart", "str", &index_start, NULL);
    bcma_cli_parse_table_add(&pt, "IndexEnd", "str", &index_end, NULL);
    bcma_cli_parse_table_add(&pt, "PipeStart", "str", &inst_start, NULL);
    bcma_cli_parse_table_add(&pt, "PipeEnd", "str", &inst_end, NULL);
    bcma_cli_parse_table_add(&pt, "BlockType", "str", &blktype, NULL);
    bcma_cli_parse_table_add(&pt, "SEED", "hex", &param->seed, NULL);
    bcma_cli_parse_table_add(&pt, "DmaLowToHigh", "int", &param->dma_low2high, NULL);
    bcma_cli_parse_table_add(&pt, "DmaRead", "int", &param->dma_read, NULL);

    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BCMA_CLI_ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (show) {
        bcma_cli_parse_table_show(&pt, NULL);
    }

    /* Test on all memories if no memory name is specified */
    if (name != NULL) {
        sal_strncpy(param->name, name, sizeof(param->name) - 1);
    } else {
        sal_strcpy(param->name, "*");
    }

    if (index_start != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_table_index(index_start, &param->index_start));
    }

    if (index_end != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_table_index(index_end, &param->index_end));
    }

    if (inst_start != NULL) {
        if (sal_strcasecmp(inst_start, "min") != 0) {
            param->inst_start = sal_ctoi(inst_start, NULL);
        }
    }

    if (inst_end != NULL) {
        if (sal_strcasecmp(inst_end, "max") != 0) {
            param->inst_end = sal_ctoi(inst_end, NULL);
        }
    }

    if (blktype != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_drv_chip_blktype_get(unit, blktype,
                                                &param->blktype));
     }

exit:
    bcma_cli_parse_table_done(&pt);

    SHR_FUNC_EXIT();
}

static int
mem_dma_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
               uint32_t flags, void **bp)
{
    mem_dma_param_t *mem_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);

    mem_param = sal_alloc(sizeof(mem_dma_param_t), "bcmaTestCaseMemDmaPrm");
    if (mem_param == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(mem_param, 0, sizeof(mem_dma_param_t));

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    SHR_IF_ERR_EXIT
        (mem_dma_parse_param(unit, cli, a, mem_param, show));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_sid_list_get(unit, 0, NULL, &mem_param->num_of_sid));

    *bp = (void*)mem_param;

exit:
    if (SHR_FUNC_ERR()) {
        if (mem_param != NULL) {
            sal_free(mem_param);
        }
    }

    SHR_FUNC_EXIT();
}

static void
mem_dma_help(int unit, void *bp)
{
    cli_out("%s", MEM_DMA_PARAM_HELP);
}

static void
mem_dma_recycle(int unit, void *bp)
{
    mem_dma_param_t *mem_param = (mem_dma_param_t *)bp;

    if (mem_param != NULL && mem_param->pc != NULL) {
        /* if testing is interrupted by ctrlc, need to stop progress thread */
        bcma_testutil_progress_done(mem_param->pc);
    }

    return;
}

static uint32_t
mem_dma_rand32(uint32_t prev)
{
    return (1664525L * prev + 1013904223L);
}

static void
mem_dma_fill_data(uint32_t *data, const uint32_t *mask, int wsize, uint32_t *seed)
{
    int idx;

    for (idx = 0; idx < wsize; idx++) {
        *seed = mem_dma_rand32(*seed);
        data[idx] = (*seed) & mask[idx];
    }
}

static int
mem_dma_table_write(int unit, bcmdrd_sid_t sid, int start, int end,
                    int tbl_inst, mem_dma_param_t *mem_param)
{
    int wsize = bcmdrd_pt_entry_wsize(unit, sid);
    int count, mem_size, idx_start, idx_end, idx;
    uint32_t *buf;
    sal_time_t tstart, tend;

    SHR_FUNC_ENTER(unit);

    count = end - start + 1;
    mem_size = count * wsize * sizeof(uint32_t);

    mem_param->wmem = bcmdrd_hal_dma_alloc(unit, mem_size, "bcmaTestTblDmaWr",
                                           &mem_param->waddr);
    if (mem_param->wmem == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(mem_param->wmem, 0, mem_size);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "DMA buffer @%p\n"),
               (void *)mem_param->wmem));

    buf = mem_param->wmem;
    for (idx = 0; idx < count; idx++) {
        mem_dma_fill_data(buf, mem_param->mask, wsize, &mem_param->rand_data);
        buf += wsize;
    }

    idx_start = mem_param->dma_low2high ? start : end;
    idx_end = mem_param->dma_low2high ? end : start;

    tstart = sal_time_usecs();
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_range_write(unit,
                                           sid, idx_start, idx_end, tbl_inst,
                                           mem_param->waddr));
    tend = sal_time_usecs();
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Memory[pipe(%d)]:%s Count:%d TSLAM time in usec:%"PRId32" \n"),
              tbl_inst, bcmdrd_pt_sid_to_name(unit, sid), count, SAL_USECS_SUB(tend, tstart)));


exit:
    if (SHR_FUNC_ERR()) {
        if (mem_param->wmem != NULL) {
            bcmdrd_hal_dma_free(unit, mem_size, mem_param->wmem, mem_param->waddr);
        }
    }

    SHR_FUNC_EXIT();
}

static int
mem_dma_table_read(int unit, bcmdrd_sid_t sid, int start, int end,
                   int tbl_inst, mem_dma_param_t *mem_param)
{
    bcmdrd_sym_info_t sinfo;
    int wsize, count, mem_size = 0, idx_start, idx_end, idx;
    uint32_t *wbuf = NULL, *rbuf = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));
    wsize = sinfo.entry_wsize;

    count = end - start + 1;
    mem_size = count * wsize * sizeof(uint32_t);

    mem_param->rmem = bcmdrd_hal_dma_alloc(unit, mem_size, "bcmaTestTblDmaRd",
                                           &mem_param->raddr);
    if (mem_param->rmem == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(mem_param->rmem, 0, mem_size);

    idx_start = mem_param->dma_low2high ? start : end;
    idx_end = mem_param->dma_low2high ? end : start;

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_range_read(unit,
                                          sid, idx_start, idx_end, tbl_inst,
                                          mem_param->raddr));

    /* Compare data */
    wbuf = mem_param->wmem;
    rbuf = mem_param->rmem;
    for (idx = 0; idx < count; idx++) {
        if (bcma_testutil_pt_data_comp(wbuf, rbuf, mem_param->mask, wsize) != 0) {
            cli_out("Memory %s, address index %d, got:", sinfo.name, idx);
            bcma_testutil_pt_data_dump(rbuf, wsize);
            cli_out("Expected:");
            bcma_testutil_pt_data_dump(wbuf, wsize);
            cli_out("Mask:");
            bcma_testutil_pt_data_dump(mem_param->mask, wsize);

            mem_param->err_cnt++;
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        wbuf += wsize;
        rbuf += wsize;
    }

exit:
    if (mem_param->wmem != NULL) {
        bcmdrd_hal_dma_free(unit, mem_size, mem_param->wmem, mem_param->waddr);
    }

    if (mem_param->rmem != NULL) {
        bcmdrd_hal_dma_free(unit, mem_size, mem_param->rmem, mem_param->raddr);
    }

    SHR_FUNC_EXIT();
}

static int
mem_dma_table_clear(int unit, bcmdrd_sid_t sid, int start, int end,
                    int tbl_inst, mem_dma_param_t *mem_param)
{
    uint32_t *wmem = NULL;
    uint64_t waddr = 0;
    int wsize = bcmdrd_pt_entry_wsize(unit, sid);
    int count, mem_size, idx_start, idx_end;

    SHR_FUNC_ENTER(unit);

    count = end - start + 1;
    mem_size = count * wsize * sizeof(uint32_t);

    wmem = bcmdrd_hal_dma_alloc(unit, mem_size, "bcmaTestTblDmaClr", &waddr);
    if (wmem == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(wmem, 0, mem_size);

    idx_start = mem_param->dma_low2high ? start : end;
    idx_end = mem_param->dma_low2high ? end : start;

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_range_write(unit,
                                           sid, idx_start, idx_end, tbl_inst,
                                           waddr));

exit:
    if (wmem != NULL) {
        bcmdrd_hal_dma_free(unit, mem_size, wmem, waddr);
    }

    SHR_FUNC_EXIT();
}

static int
mem_common_table_read(int unit, bcmdrd_sid_t sid, int start, int end,
                      int tbl_inst, mem_dma_param_t *mem_param)
{
    bcmdrd_sym_info_t sinfo;
    bcmbd_pt_dyn_info_t dyn_info;
    int wsize, count, mem_size = 0, idx;
    uint32_t *wbuf = NULL, *rbuf = NULL, *rmem = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));
    wsize = sinfo.entry_wsize;

    count = end - start + 1;
    mem_size = count * wsize * sizeof(uint32_t);

    rmem = sal_alloc(mem_size, "bcmaTestTblPioRd");
    SHR_NULL_CHECK(rmem, SHR_E_MEMORY);

    rbuf = rmem;
    for (idx = start; idx <= end; idx++) {
        dyn_info.index = idx;
        dyn_info.tbl_inst = tbl_inst;

        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit, sid, &dyn_info, NULL, rbuf, wsize));

        rbuf += wsize;
    }

    /* Compare data */
    wbuf = mem_param->wmem;
    if (mem_param->dma_low2high) {
        rbuf = rmem;
    } else {
        rbuf = rmem + (count - 1) * wsize;
    }
    for (idx = 0; idx < count; idx++) {
        if (bcma_testutil_pt_data_comp(wbuf, rbuf, mem_param->mask, wsize) != 0) {
            cli_out("Memory %s, address index %d, got:", sinfo.name, idx);
            bcma_testutil_pt_data_dump(rbuf, wsize);
            cli_out("Expected:");
            bcma_testutil_pt_data_dump(wbuf, wsize);
            cli_out("Mask:");
            bcma_testutil_pt_data_dump(mem_param->mask, wsize);

            mem_param->err_cnt++;
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        wbuf += wsize;
        if (mem_param->dma_low2high) {
            rbuf += wsize;
        } else {
            rbuf -= wsize;
        }
    }

exit:
    if (mem_param->wmem != NULL) {
        bcmdrd_hal_dma_free(unit, mem_size, mem_param->wmem, mem_param->waddr);
    }

    if (rmem != NULL) {
        sal_free(rmem);
    }

    SHR_FUNC_EXIT();
}

static int
mem_dma_table_handle(int unit, bcmdrd_sid_t sid, void *user_data)
{
    mem_dma_param_t *mem_param = (mem_dma_param_t*)user_data;
    int index_start = mem_param->index_start, index_end = mem_param->index_end;
    int inst_start = mem_param->inst_start, inst_end = mem_param->inst_end;
    int wsize, count, tbl_inst, chunk_size, rv;
    int min, max, start, end;

    SHR_FUNC_ENTER(unit);

    /*
     * Skip memories which can not be tested
     */
    if (bcmdrd_pt_attr_is_fifo(unit, sid) || bcmdrd_pt_is_notest(unit, sid)) {
        SHR_EXIT();
    }

    if (bcma_testutil_drv_pt_skip(unit, sid, BCMA_TESTUTIL_PT_MEM_DMA_RD_WR) == TRUE) {
        SHR_EXIT();
    }

    bcma_testutil_drv_mem_address_adjust(unit, sid, &index_start, &index_end,
                                         BCMA_TESTUTIL_PT_MEM_DMA_RD_WR);
    count = index_end - index_start + 1;
    if (count <= 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_inst_adjust(unit, sid, &inst_start, &inst_end));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%5d/%5d: %s\n"), sid,
                            (int)mem_param->num_of_sid,
                            bcmdrd_pt_sid_to_name(unit, sid)));

    SHR_IF_ERR_EXIT
        (bcma_testutil_cond_init(unit, sid, BCMA_TESTUTIL_PT_MEM_DMA_RD_WR));

    SHR_IF_ERR_EXIT
        (bcma_testutil_progress_update(mem_param->pc, sid));

    /* Disable SER control. */
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_ser_ctrl(unit, sid, false));

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Get memory mask */
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_mask_get
            (unit, sid, index_start, mem_param->mask, wsize, FALSE));

    for (tbl_inst = inst_start; tbl_inst <= inst_end; tbl_inst++) {
        /* Check table instance first. */
        if (!bcmdrd_pt_index_valid(unit, sid, tbl_inst, -1)) {
            continue;
        }

        min = index_start;
        max = index_end;

        while (min <= max) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_pt_valid_index_range_get(unit, sid, tbl_inst,
                                                        min, max, &start, &end));

            count = end - start + 1;
            while (count > 0) {
                if (count <= MEM_DMA_MAX_ENTRY_NUM) {
                    chunk_size = count;
                } else {
                    chunk_size = MEM_DMA_MAX_ENTRY_NUM;
                }

                end = start + chunk_size - 1;

                /* Write data */
                SHR_IF_ERR_EXIT
                    (mem_dma_table_write(unit, sid, start, end, tbl_inst,
                                         mem_param));

                /* Read and verify */
                if (mem_param->dma_read) {
                    rv = mem_dma_table_read(unit, sid, start, end, tbl_inst,
                                            mem_param);
                } else {
                    rv = mem_common_table_read(unit, sid, start, end, tbl_inst,
                                               mem_param);
                }

                if (SHR_FAILURE(rv)) {
                    SHR_ERR_EXIT(rv);
                }

                /* Clear table */
                SHR_IF_ERR_EXIT
                    (mem_dma_table_clear(unit, sid, start, end, tbl_inst,
                                         mem_param));

                count -= chunk_size;
                start += chunk_size;
            }

            min = start;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mem_dma_table_cleanup(int unit, bcmdrd_sid_t sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_cond_cleanup(unit, sid, BCMA_TESTUTIL_PT_MEM_DMA_RD_WR));
exit:
    SHR_FUNC_EXIT();
}

static int
mem_dma_init_cb(int unit, void *bp, uint32_t option)
{
    mem_dma_param_t *mem_param = (mem_dma_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    sal_srand(mem_param->seed);

    /* Disable background hardware update */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_hw_update(unit,
                                        mem_param->name,
                                        BCMA_TESTUTIL_PT_MEM_DMA_RD_WR,
                                        false));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_dma_cleanup_cb(int unit, void *bp, uint32_t option)
{
    mem_dma_param_t *mem_param = (mem_dma_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Enable background hardware update */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_hw_update(unit,
                                        mem_param->name,
                                        BCMA_TESTUTIL_PT_MEM_DMA_RD_WR,
                                        true));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Enable hardware update failed.\n")));
    }

    SHR_FUNC_EXIT();
}

static int
mem_dma_test_cb(int unit, void *bp, uint32_t option)
{
    mem_dma_param_t *mem_param = (mem_dma_param_t *)bp;
    bcma_testutil_pt_iter_t pt_iter;
    const char *name;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    name = mem_param->name;
    if (sal_strcmp(mem_param->name, "*") == 0) {
        name = BCMA_TESTUTIL_PT_ALL_MEMS;
    }

    /* coverity[dont_call] */
    mem_param->rand_data = sal_rand();

    bcma_testutil_pt_iterate_init(&pt_iter);
    pt_iter.unit = unit;
    pt_iter.name = name;
    pt_iter.blktype = mem_param->blktype;
    pt_iter.table_handle = mem_dma_table_handle;
    pt_iter.table_cleanup = mem_dma_table_cleanup;
    pt_iter.user_data = bp;

    if (mem_param->pc != NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    mem_param->pc = bcma_testutil_progress_start
                        (unit, mem_param->num_of_sid,
                         BCMA_TESTUTIL_PROGRESS_REPORT_INTERVAL);
    SHR_NULL_CHECK(mem_param->pc, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_iterate(&pt_iter));

    if (mem_param->err_cnt > 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Memory w/r test failed. (%s)\n"),
                   mem_param->name));
    }
    if (mem_param->pc != NULL) {
        bcma_testutil_progress_done(mem_param->pc);
        mem_param->pc = NULL;
    }

    SHR_FUNC_EXIT();
}

static bcma_test_proc_t mem_dma_proc[] = {
    {
        .desc = "disable hardware update and generate seed for random data.\n",
        .cb = mem_dma_init_cb,
    },
    {
        .desc = "dma access with random data.\n",
        .cb = mem_dma_test_cb,
    },
    {
        .desc = "enable hardware update.\n",
        .cb = mem_dma_cleanup_cb,
    },
};

static bcma_test_op_t mem_dma_op = {
    .select = mem_dma_select,
    .parser = mem_dma_parser,
    .help = mem_dma_help,
    .recycle = mem_dma_recycle,
    .procs = mem_dma_proc,
    .proc_count = COUNTOF(mem_dma_proc),
};

bcma_test_op_t *
bcma_testcase_mem_dma_op_get(void)
{
    return &mem_dma_op;
}
