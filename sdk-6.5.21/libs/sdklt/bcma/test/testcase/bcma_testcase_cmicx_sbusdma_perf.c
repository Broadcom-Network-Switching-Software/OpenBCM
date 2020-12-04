/*! \file bcma_testcase_cmicx_sbusdma_perf.c
 *
 * Broadcom test cases for CMICX SBUSDMA access performance.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_atomic.h>
#include <shr/shr_thread.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>

#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_mem.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_symbols.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_sbusdma_perf.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define MAX_THREAD_NUM          16
#define MAX_DMA_BUF_NUM         4

/* CLI parse table data for SBUSDMA performance test */
typedef struct parse_data_s {
    char *tbl_str;
    char *blktype_str;
    int ent_eq;
    int ent_lt;
    int ent_gt;
    int desc_mode;
    int num_desc;
    int num_thread;
    int fixed_chan;
    int num_dma_buf;
    int num_ent_dma;
    int num_iter;
    int idx_start;
    int idx_end;
    int sid_info_show;
} parse_data_t;

/* SID information */
typedef struct sid_info_s {
    bcmdrd_sid_t sid;
    int wsize;
    int idx_start;
    int idx_end;
    int range;
    int data_size;
    int num_tbl_inst;
} sid_info_t;

/* Struct for DMA buffer */
typedef struct dma_data_s {
    uint8_t *buf;
    uint64_t paddr;
    int buf_size;
    shr_atomic_int_t buf_set_sync;
} dma_data_t;

/* Struct for performance test configuration */
typedef struct perf_cfg_s {
    bool desc_mode;
    int num_desc;
    int chan;
    int num_ent_dma;
    int num_iter;
} perf_cfg_t;

/* Struct for performance test statistics */
typedef struct perf_stats_s {
    int num_dma_bytes;
    int num_dma_entries;
    int num_dma_op;
    sal_usecs_t start_time;
    sal_usecs_t end_time;
} perf_stats_t;

/* Struct for performance test data per thread */
typedef struct perf_data_s {
    int unit;
    int id;
    sid_info_t *sid_info;
    int sid_info_size;
    int sid_cnt;
    int max_data_size;
    bcmbd_sbusdma_desc_addr_t *desc;
    dma_data_t dma_data[MAX_DMA_BUF_NUM];
    int dma_data_num;
    shr_thread_t th_dma_run;
    sal_sem_t th_run_done;
    shr_thread_t th_data_fill;
    sal_sem_t th_fill_done;
    perf_cfg_t cfg;
    perf_stats_t stats;
} perf_data_t;

/* Performance test information */
typedef struct test_info_s {
    int unit;
    parse_data_t parse_data;
    perf_data_t *perf_data[MAX_THREAD_NUM];
    int perf_data_num;
} test_info_t;

#define SBUSDMA_MAGIC_DATA1     0xa5

/* Performance thread index */
static int perf_idx;

/*
 * Callback for DRD symbols iterate function
 */
static int
filter_sym(const bcmdrd_symbol_t *s, bcmdrd_sid_t sid, void *vptr)
{
    test_info_t *ti = (test_info_t *)vptr;
    parse_data_t *pd = &ti->parse_data;
    int unit = ti->unit;
    int sect, idx_start, idx_end, range;
    int num_ent, num_sect, num_tbl_inst;
    int blktype;
    bcmdrd_sym_info_t sinfo;
    perf_data_t *perf;
    sid_info_t *si;

    if (bcma_testutil_drv_pt_skip(unit, sid, BCMA_TESTUTIL_PT_MEM_TR_530)) {
        return 0;
    }
    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return 0;
    }
    if (!bcmdrd_pt_is_dmaable(unit, sid)) {
        return 0;
    }

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, &sinfo))) {
        return 0;
    }

    /* Filter PTs with number of entries */
    range = sinfo.index_max - sinfo.index_min + 1;
    /* Number of PT entries must equal to */
    if (pd->ent_eq > 0 && range != pd->ent_eq) {
        return 0;
    }
    /* Number of PT entries must less than */
    if (pd->ent_lt > 0 && range >= pd->ent_lt) {
        return 0;
    }
    /* Number of PT entries must greater than */
    if (pd->ent_gt > 0 && range <= pd->ent_gt) {
        return 0;
    }

    num_tbl_inst = bcmdrd_pt_num_tbl_inst(unit, sid);
    if (num_tbl_inst < 1) {
        return 0;
    }

    /* Filter for block type */
    if (pd->blktype_str && pd->blktype_str[0] != '*') {
        const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);
        blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
        if (blktype >= 0 && blktype < cinfo->nblktypes) {
            if (sal_strncasecmp(pd->blktype_str, cinfo->blktype_names[blktype],
                                sal_strlen(pd->blktype_str)) != 0) {
                return 0;
            }
        }
    }

    /* Check for starting and ending index of table for DMA operation */
    idx_start = sinfo.index_min;
    idx_end = sinfo.index_max;
    if (pd->idx_start > idx_start) {
        idx_start = pd->idx_start;
    }
    if (pd->idx_end >= 0 && pd->idx_end < idx_end) {
        idx_end = pd->idx_end;
    }
    if (idx_start > idx_end) {
        idx_start = idx_end;
    }
    range = idx_end - idx_start + 1;

    /*
     * Calculate sections of DMA operation for table if number of entries
     * per DMA operation is limited.
     */
    num_sect = 1;
    num_ent = pd->num_ent_dma;
    if (num_ent > 0) {
        num_sect = (range + num_ent - 1) / num_ent;
    }

    for (sect = 0; sect < num_sect; sect++) {
        perf = ti->perf_data[perf_idx];
        si = perf->sid_info + perf->sid_cnt;

        if (perf->sid_cnt >= perf->sid_info_size) {
            return 0;
        }

        /* Collect the SID information for DMA operations per thread */
        si->sid = sid;
        si->wsize = sinfo.entry_wsize;
        si->idx_start = idx_start + sect * num_ent;
        si->idx_end = (sect + 1 == num_sect) ? idx_end :
                                               si->idx_start + num_ent - 1;
        si->range = si->idx_end - si->idx_start + 1;
        si->data_size = 4 * sinfo.entry_wsize * si->range;
        si->num_tbl_inst = num_tbl_inst;

        if (si->data_size > perf->max_data_size) {
            perf->max_data_size = si->data_size;
        }

        perf->sid_cnt++;
        perf_idx++;
        if (perf_idx >= pd->num_thread) {
            perf_idx = 0;
        }
    }

    return 0;
}

/*
 * Retrieve the next DMA data buffer
 */
static inline int
dma_data_next(perf_data_t *perf, dma_data_t *dd, int buf_idx)
{
    if (perf->dma_data_num > 1) {
        shr_atomic_int_set(dd->buf_set_sync, 0);
        buf_idx++;
        if (buf_idx >= perf->dma_data_num) {
            buf_idx = 0;
        }
    }
    return buf_idx;
}

/*
 * DMA data buffer cleanup function
 */
static void
dma_data_cleanup(int unit, dma_data_t *dd)
{
    if (dd->buf) {
        bcmdrd_hal_dma_free(unit, dd->buf_size, dd->buf, dd->paddr);
    }
    if (dd->buf_set_sync) {
        shr_atomic_int_destroy(dd->buf_set_sync);
    }
}

/*
 * DMA data buffer create function
 */
static int
dma_data_init(int unit, dma_data_t *dd, int size)
{
    sal_memset(dd, 0, sizeof(*dd));

    dd->buf = bcmdrd_hal_dma_alloc(unit, size,
                                   "bcmaTestCaseCmicxSbusdmaPerfDmaBuf",
                                   &dd->paddr);
    if (dd->buf == NULL) {
        return SHR_E_MEMORY;
    }
    dd->buf_size = size;

    dd->buf_set_sync = shr_atomic_int_create(0);
    if (dd->buf_set_sync == NULL) {
        dma_data_cleanup(unit, dd);
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

/*
 * Performance data cleanup function
 */
static void
perf_data_cleanup(test_info_t *ti)
{
    int idx, d;
    parse_data_t *pd = &ti->parse_data;
    int unit = ti->unit;

    for (idx = 0; idx < pd->num_thread; idx++) {
        perf_data_t *perf = ti->perf_data[idx];

        if (perf == NULL) {
            break;
        }
        if (perf->sid_info) {
            sal_free(perf->sid_info);
        }
        if (perf->th_run_done) {
            sal_sem_destroy(perf->th_run_done);
        }
        if (perf->th_fill_done) {
            sal_sem_destroy(perf->th_fill_done);
        }
        for (d = 0; d < perf->dma_data_num; d++) {
            dma_data_cleanup(unit, &perf->dma_data[d]);
        }
        if (perf->desc) {
            bcmbd_sbusdma_desc_addr_t *da = perf->desc;
            if (da->addr) {
                bcmdrd_hal_dma_free(unit, da->size, da->addr, da->paddr);
            }
            sal_free(perf->desc);
        }
        sal_free(perf);
        ti->perf_data[idx] = NULL;
    }
}

/*
 * Performance data initialization function
 */
static int
perf_data_init(test_info_t *ti)
{
    int idx, d, rv;
    int unit = ti->unit;
    size_t num_sids = 0;
    int num_thread_sids;
    parse_data_t *pd = &ti->parse_data;
    perf_data_t *perf;
    perf_cfg_t *cfg;
    const bcmdrd_symbols_t *symbols;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_sid_list_get(unit, 0, NULL, &num_sids));

    /* Maximum number of SIDs per thread */
    num_thread_sids = (num_sids + pd->num_thread - 1) / pd->num_thread;

    /* Create performance data for each thread */
    for (idx = 0; idx < pd->num_thread; idx++) {
        perf = sal_alloc(sizeof(*perf), "bcmaTestcaseCmicxSbusdmaPerfData");
        if (perf == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        ti->perf_data[idx] = perf;
        sal_memset(perf, 0, sizeof(*perf));

        cfg = &perf->cfg;
        cfg->desc_mode = (pd->desc_mode != 0);
        cfg->num_desc = pd->desc_mode ? pd->num_desc : 0;
        cfg->chan = (pd->fixed_chan != 0) ? idx : -1;
        cfg->num_ent_dma = pd->num_ent_dma;
        cfg->num_iter = pd->num_iter;

        /* Allocate buffer to keep SID information per thread */
        perf->sid_info = sal_alloc(sizeof(sid_info_t) * num_thread_sids,
                                   "bcmaTestcaseCmicxSbusdmaPerfSidInfo");
        if (perf->sid_info == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        perf->sid_info_size = num_thread_sids;

        perf->th_run_done = sal_sem_create("bcmaTestcaseCmicxSbusdmaPerfSemR",
                                          SAL_SEM_BINARY, 0);
        if (perf->th_run_done == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        perf->th_fill_done = sal_sem_create("bcmaTestcaseCmicxSbusdmaPerfSemF",
                                           SAL_SEM_BINARY, 0);
        if (perf->th_fill_done == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        /*
         * Externally create descriptor buffer to avoid DMA memory is allocated
         * for each BD DMA batch work API
         */
        if (cfg->desc_mode && cfg->num_desc > 0) {
            bcmbd_sbusdma_desc_addr_t *da;
            da = sal_alloc(sizeof(bcmbd_sbusdma_desc_addr_t),
                                   "bcmaTestcaseCmicxSbusdmaPerfDesc");
            if (da == NULL) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            sal_memset(da, 0, sizeof(*da));
            da->size = 256;
            da->addr = bcmdrd_hal_dma_alloc(unit, da->size,
                                            "bcmaTestcaseCmicxSbusdmaPerfDesc",
                                            &da->paddr);
            if (da->addr == NULL) {
                sal_free(da);
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            sal_memset(da->addr, 0, da->size);
            perf->desc = da;
        }
        perf->dma_data_num = pd->num_dma_buf;
        perf->unit = unit;
        perf->id = idx;
    }

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    /* Collect SIDs information for each thread */
    perf_idx = 0;
    rv = bcma_testutil_symbols_iterate(symbols, pd->tbl_str, filter_sym, ti);
    if (rv <= 0) {
        cli_out("No matching symbols for test\n");
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * The DMA buffer size depends on the max data size per thread,
     * This value is decided after the SIDs information are collected.
     */
    for (idx = 0; idx < pd->num_thread; idx++) {
        perf = ti->perf_data[idx];
        for (d = 0; d < perf->dma_data_num; d++) {
            rv = dma_data_init(unit, &perf->dma_data[d], perf->max_data_size);
            if (SHR_FAILURE(rv)) {
                cli_out("Failed to allocate DMA memory\n");
                SHR_ERR_EXIT(rv);
            }
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        perf_data_cleanup(ti);
    }
    SHR_FUNC_EXIT();
}

static int
cmicx_sbusdma_perf_select(int unit)
{
    return bcmdrd_feature_enabled(unit, BCMDRD_FT_CMICX);
}

static int
cmicx_sbusdma_perf_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                          uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    test_info_t *ti;
    parse_data_t *pd;
    char *tbl_str = NULL;
    char *blktype_str = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    ti = sal_alloc(sizeof(*ti),"bcmaTestCaseCmicxSbusdmaPerf");
    if (ti == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    *bp = (void *)ti;

    sal_memset(ti, 0, sizeof(*ti));

    pd = &ti->parse_data;
    pd->ent_eq = -1;
    pd->ent_gt = -1;
    pd->ent_lt = -1;
    pd->desc_mode = 0;
    pd->num_desc = 1;
    pd->num_thread = 1;
    pd->fixed_chan = -1;
    pd->num_dma_buf = 1;
    pd->num_ent_dma = -1;
    pd->num_iter = 1;
    pd->idx_start = -1;
    pd->idx_end = -1;
    pd->sid_info_show = 0;

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Memory", "str", &tbl_str, NULL);
    bcma_cli_parse_table_add(&pt, "BlockType", "str", &blktype_str, NULL);
    bcma_cli_parse_table_add(&pt, "ENTriesEQ", "int", &pd->ent_eq, NULL);
    bcma_cli_parse_table_add(&pt, "ENTriesLT", "int", &pd->ent_lt, NULL);
    bcma_cli_parse_table_add(&pt, "ENTriesGT", "int", &pd->ent_gt, NULL);
    bcma_cli_parse_table_add(&pt, "DescMode", "bool", &pd->desc_mode, NULL);
    bcma_cli_parse_table_add(&pt, "NumDescs", "int", &pd->num_desc, NULL);
    bcma_cli_parse_table_add(&pt, "NumTHreads", "int", &pd->num_thread, NULL);
    bcma_cli_parse_table_add(&pt, "FixedChan", "bool", &pd->fixed_chan, NULL);
    bcma_cli_parse_table_add(&pt, "NumDmaBufs", "int", &pd->num_dma_buf, NULL);
    bcma_cli_parse_table_add(&pt, "NumEntDma", "int", &pd->num_ent_dma, NULL);
    bcma_cli_parse_table_add(&pt, "NumIter", "int", &pd->num_iter, NULL);
    bcma_cli_parse_table_add(&pt, "IndexStart", "int", &pd->idx_start, NULL);
    bcma_cli_parse_table_add(&pt, "IndexEnd", "int", &pd->idx_end, NULL);
    bcma_cli_parse_table_add(&pt, "SidShow", "bool", &pd->sid_info_show, NULL);

    /* Parse the parse table */
    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        bcma_cli_parse_table_done(&pt);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Show input arguments for "Test Help" CMD */
    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        bcma_cli_parse_table_show(&pt, NULL);
    }

    /* Save parameter parsed from CLI */
    if (tbl_str) {
        pd->tbl_str = sal_strdup(tbl_str);
    }
    if (blktype_str) {
        pd->blktype_str = sal_strdup(blktype_str);
    }

    /* Clear parse table */
    bcma_cli_parse_table_done(&pt);

    if (pd->num_thread <= 0 || pd->num_thread > MAX_THREAD_NUM) {
        cli_out("Invalid value (%d) for NumTHreads\n", pd->num_thread);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (pd->num_dma_buf <= 0 || pd->num_dma_buf > MAX_DMA_BUF_NUM) {
        cli_out("Invalid value (%d) for NumDmaBufs\n", pd->num_dma_buf);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (perf_data_init(ti));

exit:
    SHR_FUNC_EXIT();
}

static void
cmicx_sbusdma_perf_help(int unit, void *bp)
{
    cli_out("  Memory - Selected table name (default is all).\n");
    cli_out("  BlockType - Selected block type (default is all)\n");
    cli_out("  ENTriesEQ - Selected tables with entries (equal to)\n");
    cli_out("  ENTriesLT - Selected tables with entries (less than)\n");
    cli_out("  ENTriesGT - Selected tables with entries (greater than)\n");
    cli_out("  DescMode - Descriptor mode[0: register, 1: descriptor]\n");
    cli_out("  NumDescs - Number of descriptors (valid in descriptor mode)\n");
    cli_out("  NumTHreads - Number of threads to run SBUSDMA operations\n");
    cli_out("  FixedChan - Fixed SBUSDMA channel [0: random, 1: fixed]\n");
    cli_out("  NumDmaBufs - Number of DMA buffers\n");
    cli_out("  NumEntDma - Maximum entries limited in DMA buffer\n");
    cli_out("  NumIter - Number of iterations to run\n");
    cli_out("  IndexStart - Starting index\n");
    cli_out("  IndexEnd - Ending index\n");
    cli_out("  SidShow - Show SID information [0: hide, 1: show]\n");
}

static void
cmicx_sbusdma_perf_recycle(int unit, void *bp)
{
    test_info_t *p = (test_info_t *)bp;
    parse_data_t *pd;

    if (p == NULL) {
        return;
    }

    pd = &p->parse_data;
    SHR_FREE(pd->tbl_str);
    SHR_FREE(pd->blktype_str);

    perf_data_cleanup(p);
}

/*
 * Function to set work data of BD API for SBUSDMA operation
 */
static int
sbusdma_work_data_set(perf_data_t *perf, bcmbd_sbusdma_data_t *data,
                      sid_info_t *si, bcmbd_pt_dyn_info_t *dyn_info,
                      dma_data_t *dd)
{
    int rv;
    int unit = perf->unit;
    perf_cfg_t *cfg = &perf->cfg;
    uint32_t cmd_words[2], cwcnt;

    rv = bcmbd_pt_cmd_hdr_get(unit, si->sid, dyn_info, NULL,
                              BCMBD_PT_CMD_WRITE,
                              COUNTOF(cmd_words), cmd_words, &cwcnt);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    data->op_code = cmd_words[0];
    data->start_addr = cmd_words[1];
    data->data_width = si->wsize;
    data->op_count = si->range;
    data->buf_paddr = dd->paddr;
    if (cfg->chan >= 0) {
        data->force_chan = true;
        data->chan = cfg->chan;
    }

    return SHR_E_NONE;
}

/*
 * Function to write data to SBUSDMA buffer for SBUSDMA write operation
 */
static void
sbusdma_sid_data_set(sid_info_t *si, dma_data_t *dd)
{
    sal_memset(dd->buf, SBUSDMA_MAGIC_DATA1, si->wsize * si->range);
}

/*
 * SBUSDMA operations for the SIDs collected in the performance thread
 */
static void
perf_sbusdma_run(perf_data_t *perf)
{
    perf_cfg_t *cfg = &perf->cfg;
    perf_stats_t *stats = &perf->stats;
    int unit = perf->unit;
    int rv = SHR_E_NONE;
    int idx, tbl_inst;
    int cur_buf = 0;
    bcmbd_pt_dyn_info_t dyn_info = {0};
    bcmbd_sbusdma_data_t data[4] = {{0}};
    bcmbd_sbusdma_work_t work = {0};
    dma_data_t *dd;

    for (idx = 0; idx < perf->sid_cnt; idx++) {
        sid_info_t *si = perf->sid_info + idx;

        for (tbl_inst = 0; tbl_inst < si->num_tbl_inst; tbl_inst++) {
            dyn_info.tbl_inst = tbl_inst;
            dyn_info.index = si->idx_start;

            dd = perf->dma_data + cur_buf;
            if (perf->dma_data_num == 1) {
                sbusdma_sid_data_set(si, dd);
            } else {
                /* Wait for the buffer ready from the data_fill thread */
                while (shr_atomic_int_get(dd->buf_set_sync) != 1) {
                    sal_usleep(1);
                }
            }

            rv = sbusdma_work_data_set(perf, data, si, &dyn_info, dd);
            if (SHR_FAILURE(rv)) {
                cli_out("Skip %s, tbl_inst %d\n",
                        bcmdrd_pt_sid_to_name(unit, si->sid), tbl_inst);
                cur_buf = dma_data_next(perf, dd, cur_buf);
                continue;
            }

            work.data = data;
            work.items = 1;
            work.flags = SBUSDMA_WF_WRITE_CMD;

            if (cfg->desc_mode) {
                if (perf->desc) {
                    work.desc = perf->desc;
                }
                rv = bcmbd_sbusdma_batch_work_create(unit, &work);
            } else {
                rv = bcmbd_sbusdma_light_work_init(unit, &work);
            }
            if (SHR_FAILURE(rv)) {
                cli_out("%s: Failed to init SBUSDMA work for %s mode "
                        "at tbl_inst = %d/%d\n",
                        cfg->desc_mode ? "descriptor" : "register",
                        bcmdrd_pt_sid_to_name(unit, si->sid),
                        tbl_inst, si->num_tbl_inst);
                cur_buf = dma_data_next(perf, dd, cur_buf);
                continue;
            }

            if (SHR_SUCCESS(bcmbd_sbusdma_work_execute(unit, &work))) {
                stats->num_dma_bytes += si->data_size;
                stats->num_dma_entries += si->range;
                stats->num_dma_op++;
            } else {
                cli_out("%s: sbusdma_write failed at tbl_inst = %d/%d\n",
                        bcmdrd_pt_sid_to_name(unit, si->sid),
                        tbl_inst, si->num_tbl_inst);
            }
            if (cfg->desc_mode) {
                rv = bcmbd_sbusdma_batch_work_delete(unit, &work);
                if (SHR_FAILURE(rv)) {
                    cli_out("%s: bcmbd_sbusdma_batch_work_delete failed "
                            "at tbl_inst = %d/%d\n",
                            bcmdrd_pt_sid_to_name(unit, si->sid),
                            tbl_inst, si->num_tbl_inst);
                }
            }

            cur_buf = dma_data_next(perf, dd, cur_buf);
        }
    }
}

/*
 * Thread to run the SBUSDMA operations
 */
static void
sbusdma_run(shr_thread_t th, void *arg)
{
    perf_data_t *perf = (perf_data_t *)(arg);
    perf_cfg_t *cfg = &perf->cfg;
    perf_stats_t *stats = &perf->stats;
    int idx;

    stats->start_time = sal_time_usecs();

    for (idx = 0; idx < cfg->num_iter; idx++) {
        perf_sbusdma_run(perf);
    }

    stats->end_time = sal_time_usecs();

    sal_sem_give(perf->th_run_done);
}

/*
 * Thread to fill the DMA data buffer for performance thread if the number of
 * DMA data buffer is more than 1.
 */
static void
sbusdma_data_fill(shr_thread_t th, void *arg)
{
    perf_data_t *perf = (perf_data_t *)(arg);
    int idx, tbl_inst;
    int cur_buf = 0;

    for (idx = 0; idx < perf->sid_cnt; idx++) {
        sid_info_t *si = perf->sid_info + idx;

        for (tbl_inst = 0; tbl_inst < si->num_tbl_inst; tbl_inst++) {
            dma_data_t *dd = perf->dma_data + cur_buf;

            while (shr_atomic_int_get(dd->buf_set_sync) != 0) {
                sal_usleep(1);
            }

            sbusdma_sid_data_set(si, dd);

            shr_atomic_int_set(dd->buf_set_sync, 1);

            cur_buf++;
            if (cur_buf >= perf->dma_data_num) {
                cur_buf = 0;
            }
        }
    }

    sal_sem_give(perf->th_fill_done);
}

/*
 * Display statistics of the performance result
 */
static void
sbusdma_perf_stats(test_info_t *ti)
{
    int idx, time, s;
    sal_usecs_t start_time = -1, end_time = 0;
    int num_bytes = 0, num_entries = 0, num_dma_ops = 0;
    parse_data_t *pd = &ti->parse_data;

    for (idx = 0; idx < pd->num_thread; idx++) {
        perf_data_t *perf = ti->perf_data[idx];
        perf_stats_t *pstats = &perf->stats;

        time = pstats->end_time - pstats->start_time;
        cli_out("Thread[%u]: write %d bytes, %d entries\n"
                "    %d usecs, %d SID tested, %d DMAs, %d usec/DMA\n",
                perf->id, pstats->num_dma_bytes, pstats->num_dma_entries,
                time, perf->sid_cnt, pstats->num_dma_op,
                pstats->num_dma_op == 0 ? 0 : time / pstats->num_dma_op);
        if (pd->sid_info_show) {
            for (s = 0; s < perf->sid_cnt; s++) {
                sid_info_t *si = perf->sid_info + s;
                cli_out(" %s: wsize = %d, range = %d\n",
                        bcmdrd_pt_sid_to_name(perf->unit, si->sid),
                        si->wsize, si->range);
            }
        }

        if (pstats->start_time < start_time) {
            start_time = pstats->start_time;
        }
        if (pstats->end_time > end_time) {
            end_time = pstats->end_time;
        }
        num_bytes += pstats->num_dma_bytes;
        num_entries += pstats->num_dma_entries;
        num_dma_ops += pstats->num_dma_op;
    }
    time = end_time - start_time;
    cli_out("\n%d usecs, write %d KB, %d entries, %d DMAs, %d usec/DMA\n",
            time, num_bytes >> 10, num_entries, num_dma_ops,
            num_dma_ops == 0 ? 0 : time / num_dma_ops);
}

static int
cmicx_sbusdma_perf_test(int unit, void *bp, uint32_t option)
{
    test_info_t *p = (test_info_t *)bp;
    parse_data_t *pd;
    int idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p, SHR_E_PARAM);

    pd = &p->parse_data;

    /* Create performance threads to run SBUSDMA operations */
    for (idx = 0; idx < pd->num_thread; idx++) {
        perf_data_t *perf = p->perf_data[idx];

        /*
         * If the number of DMA buffers for each performance thread is
         * greater than 1, create data-fill thread for each performance thread
         * to fill the DMA buffers.
         */
        if (pd->num_dma_buf > 1) {
            perf->th_data_fill = shr_thread_start("bcmaTestDmaPfD", -1,
                                                  sbusdma_data_fill, perf);
            if (perf->th_data_fill == NULL) {
                rv = SHR_E_FAIL;
                break;
            }
        }

        perf->th_dma_run = shr_thread_start("bcmaTestDmaPfR", -1,
                                            sbusdma_run, perf);
        if (perf->th_dma_run == NULL) {
            rv = SHR_E_FAIL;
            break;
        }
    }

    /* Wait for the performance thread to finish */
    for (idx = 0; idx < pd->num_thread; idx++) {
        perf_data_t *perf = p->perf_data[idx];

        if (perf->th_data_fill) {
            sal_sem_take(perf->th_fill_done, 60 * SECOND_USEC);
            rv = shr_thread_stop(perf->th_data_fill, 5 * SECOND_USEC);
            if (SHR_FAILURE(rv)) {
                cli_out("Failed to stop data fill thread-%d\n", idx);
            }
        }
        if (perf->th_dma_run) {
            sal_sem_take(perf->th_run_done, 60 * SECOND_USEC);
            rv = shr_thread_stop(perf->th_dma_run, 5 * SECOND_USEC);
            if (SHR_FAILURE(rv)) {
                cli_out("Failed to stop performance thread-%d\n", idx);
            }
        }
    }

    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    /* Display statistics of the performance threads */
    sbusdma_perf_stats(p);

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t cmicx_sbusdma_perf_proc[] = {
    {
        .desc = "run SBUS DMA performance test.\n",
        .cb = cmicx_sbusdma_perf_test,
    },
};

static bcma_test_op_t cmicx_sbusdma_perf_op = {
    .select = cmicx_sbusdma_perf_select,
    .parser = cmicx_sbusdma_perf_parser,
    .help = cmicx_sbusdma_perf_help,
    .recycle = cmicx_sbusdma_perf_recycle,
    .procs = cmicx_sbusdma_perf_proc,
    .proc_count = COUNTOF(cmicx_sbusdma_perf_proc),
};

bcma_test_op_t *
bcma_testcase_cmicx_sbusdma_perf_op_get(void)
{
    return &cmicx_sbusdma_perf_op;
}
