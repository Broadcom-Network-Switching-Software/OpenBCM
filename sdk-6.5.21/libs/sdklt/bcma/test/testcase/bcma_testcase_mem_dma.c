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
#include <shr/shr_atomic.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_time.h>

#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>

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

#define MEM_DMA_MAX_ENTRY_NUM              (60000)
#define MEM_DMA_READ_THREAD_NUM            (2)
#define MEM_DMA_READ_THREAD_WAIT_TIME      (10) /* secs */
#define MEM_DMA_READ_THREAD_STOP_WAITTIME  (2) /* secs */

#define MEM_DMA_PARAM_HELP \
    "  Memory        - The memory to be tested\n"\
    "  IndexStart    - Starting index\n"\
    "  IndexEnd      - Ending index\n"\
    "  IntrMode      - Whether to use interrupt mode\n"\
    "                   True  : Interrupt mode\n"\
    "                   False : Polling mode (default)\n"\
    "  DmaRead       - Read memory in DMA mode\n"\
    "                   True  : DMA mode (default)\n"\
    "                   False : PIO mode\n"\
    "  DmaLowToHigh  - Bottom-to-top fill\n"\
    "                  (Only available when DmaRead = True)\n"\
    "  DmaReadThread - Read memory in DMA mode with multithread\n"\
    "                  (Only available when DmaRead = True)\n"\
    "                   0 : Read just one time in single thread (default)\n"\
    "                   N : Read N times with two threads simultaneously\n"\
    "                       It will use two DMA channels during the test\n"\
    "  SEED          - The seed to use in the random function\n"\
    "  PipeStart     - Starting pipe\n"\
    "  PipeEnd       - Ending pipe\n"\
    "  BlockType     - The block type which the memories belong to\n"\
    "\nExample:\n"\
    "    Run Memory DMA testing for all memories\n"\
    "      tr 71\n"\
    "    Run Memory DMA testing for the specfic one\n"\
    "      tr 71 m=L2Xm is=min ie=max\n"\
    "    Run Memory DMA testing with polling mode."\
    "      tr 71 im=false\n"\
    "    Run Memory DMA testing, however read data with PIO instead of DMA\n"\
    "      tr 71 dr=0\n"\
    "    Run Memory DMA testing, read data twice with DMA in multithread"\
    "      tr 71 dr=1 drt=2\n"

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
    int dma_read_thread_num;

    /* Seed to generate random data */
    uint32_t seed;
    uint32_t rand_data;

    /* Number of memories which can not get expected value */
    shr_atomic_int_t err_cnt;

    /* Number of sid */
    size_t num_of_sid;

    /* Whether to use interrupt mode */
    int intr_mode;

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
    param->index_start = BCMA_TESTUTIL_TABLE_MIN_INDEX;
    param->index_end = BCMA_TESTUTIL_TABLE_MAX_INDEX;
    param->inst_start = -1;
    param->inst_end = -1;
    param->blktype = -1;
    param->intr_mode = FALSE;
    param->dma_read = TRUE;
    param->dma_low2high = TRUE;
    param->dma_read_thread_num = 0;
    param->err_cnt = shr_atomic_int_create(0);

    bcma_cli_parse_table_init(cli, &pt);

    SHR_NULL_CHECK(param->err_cnt, SHR_E_INTERNAL);

    bcma_cli_parse_table_add(&pt, "Memory", "str", &name, NULL);
    bcma_cli_parse_table_add(&pt, "IndexStart", "str", &index_start, NULL);
    bcma_cli_parse_table_add(&pt, "IndexEnd", "str", &index_end, NULL);
    bcma_cli_parse_table_add(&pt, "PipeStart", "str", &inst_start, NULL);
    bcma_cli_parse_table_add(&pt, "PipeEnd", "str", &inst_end, NULL);
    bcma_cli_parse_table_add(&pt, "BlockType", "str", &blktype, NULL);
    bcma_cli_parse_table_add(&pt, "SEED", "hex", &param->seed, NULL);
    bcma_cli_parse_table_add(&pt, "DmaLowToHigh", "bool", &param->dma_low2high, NULL);
    bcma_cli_parse_table_add(&pt, "DmaRead", "bool", &param->dma_read, NULL);
    bcma_cli_parse_table_add(&pt, "DmaReadThread", "int", &param->dma_read_thread_num, NULL);
    bcma_cli_parse_table_add(&pt, "IntrMode", "bool", &param->intr_mode, NULL);

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

    if (mem_param != NULL) {
        if (mem_param->pc != NULL) {
            /* if testing is interrupted by ctrlc, need to stop progress thread */
            bcma_testutil_progress_done(mem_param->pc);
        }
        if (mem_param->err_cnt != NULL) {
            shr_atomic_int_destroy(mem_param->err_cnt);
            mem_param->err_cnt = NULL;
        }
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

/* the parameter for single memory access */
typedef struct mem_dma_access_param_s {
    /* general */
    int unit;
    bcmdrd_sid_t sid;
    int start;
    int end;
    int tbl_inst;
    const uint32_t *cmp_mem_data;
    const uint32_t *cmp_mem_mask;
    uint64_t cmp_mem_addr;
    bool dma_low2high;
    bool intr_mode;
    shr_atomic_int_t err_cnt;

    /* extra for dma-read */
    int dma_read_thread_num;
    shr_atomic_int_t dma_read_thread_completed;
} mem_dma_access_param_t;

static int
mem_dma_table_write(int unit, mem_dma_access_param_t *access_param)
{
    int count, idx_start, idx_end;
    sal_time_t tstart, tend;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(access_param, SHR_E_PARAM);

    count = access_param->end - access_param->start + 1;
    if (access_param->dma_low2high == true) {
        idx_start = access_param->start;
        idx_end = access_param->end;
    } else {
        idx_start = access_param->end;
        idx_end = access_param->start;
    }

    tstart = sal_time_usecs();
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_range_write
            (unit, access_param->sid, idx_start, idx_end,
             access_param->tbl_inst, access_param->cmp_mem_addr,
             access_param->intr_mode));

    tend = sal_time_usecs();
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Memory[pipe(%d)]:%s Count:%d TSLAM time in usec:%"PRId32" \n"),
              access_param->tbl_inst,
              bcmdrd_pt_sid_to_name(unit, access_param->sid),
              count, SAL_USECS_SUB(tend, tstart)));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_dma_table_read_single(mem_dma_access_param_t *access_param)
{
    bcmdrd_sym_info_t sinfo;
    int wsize, count, mem_size = 0, idx_start, idx_end, idx;
    const uint32_t *wbuf = NULL;
    uint32_t *rbuf = NULL, *rmem = NULL;
    uint64_t raddr;
    int unit = (access_param != NULL)? access_param->unit: BSL_UNIT_UNKNOWN;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(access_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, access_param->sid, &sinfo));
    wsize = sinfo.entry_wsize;

    count = access_param->end - access_param->start + 1;
    mem_size = count * wsize * sizeof(uint32_t);

    rmem = bcmdrd_hal_dma_alloc(unit, mem_size, "bcmaTestTblDmaRd", &raddr);
    SHR_NULL_CHECK(rmem, SHR_E_MEMORY);
    sal_memset(rmem, 0, mem_size);

    if (access_param->dma_low2high == true) {
        idx_start = access_param->start;
        idx_end = access_param->end;
    } else {
        idx_start = access_param->end;
        idx_end = access_param->start;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_range_read
            (unit, access_param->sid, idx_start, idx_end,
             access_param->tbl_inst, raddr, access_param->intr_mode));

    /* Compare data */
    wbuf = access_param->cmp_mem_data;
    rbuf = rmem;
    for (idx = 0; idx < count; idx++) {
        if (bcma_testutil_pt_data_comp(wbuf, rbuf,
                                       access_param->cmp_mem_mask,
                                       wsize) != 0) {
            cli_out("Memory %s, address index %d, got:", sinfo.name, idx);
            bcma_testutil_pt_data_dump(rbuf, wsize);
            cli_out("Expected:");
            bcma_testutil_pt_data_dump(wbuf, wsize);
            cli_out("Mask:");
            bcma_testutil_pt_data_dump(access_param->cmp_mem_mask, wsize);

            shr_atomic_int_incr(access_param->err_cnt);
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        wbuf += wsize;
        rbuf += wsize;
    }

exit:
    if (rmem != NULL) {
        bcmdrd_hal_dma_free(unit, mem_size, rmem, raddr);
    }
    SHR_FUNC_EXIT();
}

static void
mem_dma_table_read_thread(shr_thread_t tc, void *arg)
{
    mem_dma_access_param_t *read_param = (mem_dma_access_param_t *)arg;
    int rv;
    int read_cur = 0;

    if (read_param == NULL) {
        cli_out("Error, cannot find read_param");
        return;
    }

    while (1) {
        if (shr_thread_stopping(tc)) {
            break;
        }
        rv = mem_dma_table_read_single(read_param);
        if (SHR_FAILURE(rv)) {
            break;
        }
        read_cur++;
        if (read_cur == read_param->dma_read_thread_num) {
            break;
        }
    }
    shr_atomic_int_incr(read_param->dma_read_thread_completed);
}

static int
mem_dma_table_read(mem_dma_access_param_t *access_param)
{
    int i;
    int unit = (access_param != NULL)? access_param->unit: BSL_UNIT_UNKNOWN;
    shr_thread_t *read_thread_pid[MEM_DMA_READ_THREAD_NUM] = {NULL};
    shr_atomic_int_t dma_read_thread_completed = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(access_param, SHR_E_PARAM);

    if (access_param->dma_read_thread_num == 0) {
        /* just read one time */
        SHR_IF_ERR_EXIT
            (mem_dma_table_read_single(access_param));
    } else {
        /* read several times with multithread */
        sal_usecs_t datadma_time_start;
        int err_cnt_begin = shr_atomic_int_get(access_param->err_cnt);
        int err_cnt_end;

        dma_read_thread_completed = shr_atomic_int_create(0);
        SHR_NULL_CHECK(dma_read_thread_completed, SHR_E_INTERNAL);
        access_param->dma_read_thread_completed = dma_read_thread_completed;

        for (i = 0; i < MEM_DMA_READ_THREAD_NUM; i++) {
            read_thread_pid[i] = shr_thread_start
                                    ("bcmatestcasemememdmathread",
                                     SAL_THREAD_PRIO_DEFAULT,
                                     mem_dma_table_read_thread,
                                     (void *)access_param);
            SHR_NULL_CHECK(read_thread_pid[i], SHR_E_INTERNAL);
        }

        datadma_time_start = sal_time_usecs();

        while (1) {
            int atomic_value = shr_atomic_int_get(dma_read_thread_completed);
            sal_usecs_t elapsed_time = sal_time_usecs() - datadma_time_start;

            if (atomic_value == MEM_DMA_READ_THREAD_NUM) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "dma read : elapsed time %"PRIu32" usec\n"),
                                        (uint32_t)elapsed_time));
                break;
            }
            if (elapsed_time > MEM_DMA_READ_THREAD_WAIT_TIME * SECOND_USEC) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "dma read : timeout\n")));
                SHR_ERR_EXIT(SHR_E_TIMEOUT);
                break;
            }
            sal_usleep(1);
        }

        err_cnt_end = shr_atomic_int_get(access_param->err_cnt);
        if (err_cnt_end - err_cnt_begin > 0) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }

exit:
    for (i = 0; i < MEM_DMA_READ_THREAD_NUM; i++) {
        if (read_thread_pid[i] != NULL) {
            int rv;
            rv = shr_thread_stop(read_thread_pid[i],
                                 MEM_DMA_READ_THREAD_STOP_WAITTIME * SECOND_USEC);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "dma read : cannot stop thread\n")));
            }
            read_thread_pid[i] = NULL;
        }
    }
    if (dma_read_thread_completed != NULL) {
        shr_atomic_int_destroy(dma_read_thread_completed);
        dma_read_thread_completed = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
mem_dma_table_clear(int unit, mem_dma_access_param_t *access_param)
{
    uint32_t *wmem = NULL;
    uint64_t waddr = 0;
    int count, mem_size, idx_start, idx_end;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(access_param, SHR_E_PARAM);

    count = access_param->end - access_param->start + 1;
    mem_size = count * bcmdrd_pt_entry_wsize(unit, access_param->sid) * sizeof(uint32_t);

    wmem = bcmdrd_hal_dma_alloc(unit, mem_size, "bcmaTestTblDmaClr", &waddr);
    if (wmem == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(wmem, 0, mem_size);

    if (access_param->dma_low2high == true) {
        idx_start = access_param->start;
        idx_end = access_param->end;
    } else {
        idx_start = access_param->end;
        idx_end = access_param->start;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_range_write
            (unit, access_param->sid, idx_start, idx_end,
             access_param->tbl_inst, waddr, access_param->intr_mode));

exit:
    if (wmem != NULL) {
        bcmdrd_hal_dma_free(unit, mem_size, wmem, waddr);
    }

    SHR_FUNC_EXIT();
}

static int
mem_common_table_read(int unit, mem_dma_access_param_t *access_param)
{
    bcmdrd_sym_info_t sinfo;
    bcmbd_pt_dyn_info_t dyn_info;
    int wsize, count, mem_size = 0, idx;
    const uint32_t *wbuf = NULL;
    uint32_t *rbuf = NULL, *rmem = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(access_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, access_param->sid, &sinfo));
    wsize = sinfo.entry_wsize;

    count = access_param->end - access_param->start + 1;
    mem_size = count * wsize * sizeof(uint32_t);

    rmem = sal_alloc(mem_size, "bcmaTestTblPioRd");
    SHR_NULL_CHECK(rmem, SHR_E_MEMORY);

    rbuf = rmem;
    for (idx = access_param->start; idx <= access_param->end; idx++) {
        dyn_info.index = idx;
        dyn_info.tbl_inst = access_param->tbl_inst;

        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit, access_param->sid, &dyn_info, NULL, rbuf, wsize));

        rbuf += wsize;
    }

    /* Compare data */
    wbuf = access_param->cmp_mem_data;
    if (access_param->dma_low2high == true) {
        rbuf = rmem;
    } else {
        rbuf = rmem + (count - 1) * wsize;
    }
    for (idx = 0; idx < count; idx++) {
        if (bcma_testutil_pt_data_comp(wbuf, rbuf, access_param->cmp_mem_mask, wsize) != 0) {
            cli_out("Memory %s, address index %d, got:", sinfo.name, idx);
            bcma_testutil_pt_data_dump(rbuf, wsize);
            cli_out("Expected:");
            bcma_testutil_pt_data_dump(wbuf, wsize);
            cli_out("Mask:");
            bcma_testutil_pt_data_dump(access_param->cmp_mem_mask, wsize);

            shr_atomic_int_incr(access_param->err_cnt);
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        wbuf += wsize;
        if (access_param->dma_low2high == true) {
            rbuf += wsize;
        } else {
            rbuf -= wsize;
        }
    }

exit:
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
    uint32_t *wmem = NULL;
    uint64_t wmem_addr;
    int wmem_size = 0;
    int i;
    uint32_t wmem_mask[BCMDRD_MAX_PT_WSIZE];

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

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_address_adjust(unit, sid, &index_start,
                                              &index_end,
                                              BCMA_TESTUTIL_PT_MEM_DMA_RD_WR));
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
            (unit, sid, index_start, wmem_mask, wsize, FALSE));

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
                uint32_t *buf;
                mem_dma_access_param_t access_param;

                if (count <= MEM_DMA_MAX_ENTRY_NUM) {
                    chunk_size = count;
                } else {
                    chunk_size = MEM_DMA_MAX_ENTRY_NUM;
                }

                end = start + chunk_size - 1;

                /* allocate memory */
                wmem_size = chunk_size * wsize * sizeof(uint32_t);
                wmem = bcmdrd_hal_dma_alloc(unit, wmem_size,
                                            "bcmaTestTblDmaWr", &wmem_addr);
                SHR_NULL_CHECK(wmem, SHR_E_MEMORY);
                sal_memset(wmem, 0, wmem_size);

                /* fill pattern */
                buf = wmem;
                for (i = 0; i < chunk_size; i++) {
                    mem_dma_fill_data(buf, wmem_mask, wsize,
                                      &mem_param->rand_data);
                    buf += wsize;
                }

                /* Write data */
                sal_memset(&access_param, 0, sizeof(access_param));
                access_param.unit = unit;
                access_param.sid = sid;
                access_param.start = start;
                access_param.end = end;
                access_param.tbl_inst = tbl_inst,
                access_param.cmp_mem_data = wmem;
                access_param.cmp_mem_addr = wmem_addr;
                access_param.cmp_mem_mask = wmem_mask;
                access_param.dma_low2high =
                    (mem_param->dma_low2high == TRUE)? true: false,
                access_param.intr_mode =
                    (mem_param->intr_mode == TRUE)? true: false;
                access_param.err_cnt = mem_param->err_cnt;
                access_param.dma_read_thread_num = mem_param->dma_read_thread_num;

                SHR_IF_ERR_EXIT(mem_dma_table_write(unit, &access_param));

                /* Read and verify */
                if (mem_param->dma_read == TRUE) {
                    rv = mem_dma_table_read(&access_param);
                } else {
                    rv = mem_common_table_read(unit, &access_param);
                }

                if (SHR_FAILURE(rv)) {
                    SHR_ERR_EXIT(rv);
                }

                /* free memory and clear table for the next round */
                if (wmem != NULL) {
                    bcmdrd_hal_dma_free(unit, wmem_size, wmem, wmem_addr);
                    wmem = NULL;
                }
                SHR_IF_ERR_EXIT(mem_dma_table_clear(unit, &access_param));

                count -= chunk_size;
                start += chunk_size;
            }

            min = start;
        }
    }

exit:
    if (wmem != NULL) {
        bcmdrd_hal_dma_free(unit, wmem_size, wmem, wmem_addr);
        wmem = NULL;
    }
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

    if (shr_atomic_int_get(mem_param->err_cnt) > 0) {
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
