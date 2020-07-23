/*! \file bcma_testcase_mem_rand_w.c
 *
 * Test case for memory random address sets.
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
#include <bcma/test/testcase/bcma_testcase_mem_rand_w.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define MEM_RAND_W_PARAM_HELP \
    "  Memory     - The memory to be tested\n"\
    "  IndexStart - Starting index\n"\
    "  IndexEnd   - Ending index\n"\
    "  ITERation  - Number of iterations to repeat\n"\
    "  SEED       - The seed to use in the random function\n"\
    "  InitData   - The basic data to be written in the test\n"\
    "  PipeStart  - Starting pipe\n"\
    "  PipeEnd    - Ending pipe\n"\
    "  BlockType  - The block type which the memories belong to\n"\
    "\nExample:\n"\
    "  tr 52 m=L2Xm is=min ie=max iter=10 seed=0x00decade id=0xffffffff\n"\
    "  tr 52\n"

typedef struct mem_rand_w_param_s {
    /* Memory name */
    char name[BCMA_TESTUTIL_STR_MAX_SIZE];

    /* Starting and ending index */
    int index_start;
    int index_end;
    int user_index_start;
    int user_index_end;

    /* Starting and ending table instance */
    int inst_start;
    int inst_end;
    int user_inst_start;
    int user_inst_end;

    int blktype;

    /* Test iteration */
    int iter;
    int cur_iter;

    /* Seed used to generate address index */
    uint32_t seed;

    /* Buffer for write/read/mask data */
    uint32_t wdata[BCMDRD_MAX_PT_WSIZE];
    uint32_t rdata[BCMDRD_MAX_PT_WSIZE];
    uint32_t mask[BCMDRD_MAX_PT_WSIZE];
    uint32_t wsize;

    /* Address index calculated in testing */
    int first_addr;
    int inc_addr;
    int cur_addr;

    /* Test data */
    uint32_t init_data;
    uint32_t cur_data;

    /* Number of memories which can not get expected value */
    int err_cnt;

    /* Number of sid */
    size_t num_of_sid;

    /* progress report control */
    bcma_testutil_progress_ctrl_t *pc;
} mem_rand_w_param_t;

static int
mem_rand_w_select(int unit)
{
    return true;
}

static int
mem_rand_w_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                       mem_rand_w_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    char *name = NULL, *index_start = NULL, *index_end = NULL;
    char *inst_start = NULL, *inst_end = NULL, *blktype = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    param->iter = 10;
    param->seed = 0xdecade;
    param->init_data = 0xffffffff;
    param->user_index_start = BCMA_TESTUTIL_TABLE_MIN_INDEX;
    param->user_index_end = BCMA_TESTUTIL_TABLE_MAX_INDEX;
    param->user_inst_start = -1;
    param->user_inst_end = -1;
    param->blktype = -1;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Memory", "str", &name, NULL);
    bcma_cli_parse_table_add(&pt, "IndexStart", "str", &index_start, NULL);
    bcma_cli_parse_table_add(&pt, "IndexEnd", "str", &index_end, NULL);
    bcma_cli_parse_table_add(&pt, "PipeStart", "str", &inst_start, NULL);
    bcma_cli_parse_table_add(&pt, "PipeEnd", "str", &inst_end, NULL);
    bcma_cli_parse_table_add(&pt, "BlockType", "str", &blktype, NULL);
    bcma_cli_parse_table_add(&pt, "ITERation", "int", &param->iter, NULL);
    bcma_cli_parse_table_add(&pt, "SEED", "hex", &param->seed, NULL);
    bcma_cli_parse_table_add(&pt, "InitData", "hex", &param->init_data, NULL);

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
            (bcma_testutil_parse_table_index(index_start, &param->user_index_start));
    }

    if (index_end != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_table_index(index_end, &param->user_index_end));
    }

    if (inst_start != NULL) {
        if (sal_strcasecmp(inst_start, "min") != 0) {
            param->user_inst_start = sal_ctoi(inst_start, NULL);
        }
    }

    if (inst_end != NULL) {
        if (sal_strcasecmp(inst_end, "max") != 0) {
            param->user_inst_end = sal_ctoi(inst_end, NULL);
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
mem_rand_w_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                  uint32_t flags, void **bp)
{
    mem_rand_w_param_t *mem_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    mem_param = sal_alloc(sizeof(mem_rand_w_param_t), "bcmaTestCaseMemRandWPrm");
    if (mem_param == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(mem_param, 0, sizeof(mem_rand_w_param_t));

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    SHR_IF_ERR_EXIT
        (mem_rand_w_parse_param(unit, cli, a, mem_param, show));

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
mem_rand_w_help(int unit, void *bp)
{
    cli_out("%s", MEM_RAND_W_PARAM_HELP);
}

static void
mem_rand_w_recycle(int unit, void *bp)
{
    mem_rand_w_param_t *mem_param = (mem_rand_w_param_t *)bp;

    if (mem_param != NULL && mem_param->pc != NULL) {
        /* if testing is interrupted by ctrlc, need to stop progress thread */
        bcma_testutil_progress_done(mem_param->pc);
    }

    return;
}

/*
 * Return true if a and b are relatively prime
 * Bad O(n) algorithm
 */
static uint32_t
mem_rand_w_relprime(uint32_t a, uint32_t b)
{
    uint32_t idx;
    uint32_t min = a < b ? a : b;
    uint32_t max = a < b ? b : a;

    if (max % min == 0) {
        return 0;
    }

    if ((a % 2 == 0) && (b % 2 == 0)) {
        return 0;
    }

    for (idx = 3; idx <= min / 2; idx += 2) {
        if ((a % idx == 0) && (b % idx == 0)) {
            return 0;
        }
    }

    return 1;
}

/*
 * Return the first value greater than or equal to start that is
 * relatively prime to val.
 */
static uint32_t
mem_rand_w_get_rel_prime(uint32_t val, uint32_t start)
{
    while (!mem_rand_w_relprime(val, start)) {
        start++;
    }

    return start;
}

static int
mem_rand_w_entity_next_id(int unit, void *user_data)
{
    mem_rand_w_param_t *mem_param = (mem_rand_w_param_t*)user_data;
    uint32_t total_addr;
    uint32_t i;

    if (mem_param == NULL) {
        return -1;
    }

    /* Calculate next address */
    total_addr = mem_param->index_end - mem_param->index_start + 1;
    mem_param->cur_addr =
        ((mem_param->cur_addr -
          mem_param->index_start + mem_param->inc_addr) % total_addr)
        + mem_param->index_start;

    if (mem_param->cur_addr == mem_param->first_addr) {
        /* end this traverse */
        mem_param->cur_data = mem_param->init_data;
        mem_param->cur_addr = mem_param->index_start;
        mem_param->first_addr = -1;
        return -1;
    }

    if (mem_param->first_addr == -1) {
        mem_param->first_addr = mem_param->cur_addr;
    }

    /* update new data for the next entity */
    if (mem_param->wsize % 2 == 0) {
        mem_param->cur_data= ~mem_param->cur_data;
    }

    for (i = 0; i < mem_param->wsize; i++) {
        mem_param->wdata[i] = mem_param->cur_data & mem_param->mask[i];
        mem_param->cur_data = ~mem_param->cur_data;
    }

    return mem_param->cur_addr;
}

static int
mem_rand_w_entity_handle_write(int unit, bcmdrd_sid_t sid,
                               bcmbd_pt_dyn_info_t *dyn_info,
                               void *ovrr_info, void *user_data)
{
    mem_rand_w_param_t *mem_param = (mem_rand_w_param_t*)user_data;

    SHR_FUNC_ENTER(unit);

    /* skip instance that user not specified */
    if (dyn_info->tbl_inst < mem_param->inst_start
        || dyn_info->tbl_inst > mem_param->inst_end) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_write(unit, sid, dyn_info, NULL,
                                    mem_param->wdata));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_w_entity_handle_read(int unit, bcmdrd_sid_t sid,
                              bcmbd_pt_dyn_info_t *dyn_info,
                              void *ovrr_info, void *user_data)
{
    mem_rand_w_param_t *mem_param = (mem_rand_w_param_t*)user_data;
    bcmdrd_sym_info_t sinfo;

    SHR_FUNC_ENTER(unit);

    /* skip instance that user not specified */
    if (dyn_info->tbl_inst < mem_param->inst_start
        || dyn_info->tbl_inst > mem_param->inst_end) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_read(unit, sid, dyn_info, ovrr_info,
                                   mem_param->rdata,
                                   mem_param->wsize));

    if (bcma_testutil_pt_data_comp(mem_param->rdata, mem_param->wdata,
                                   mem_param->mask, mem_param->wsize) != 0) {
        if (ovrr_info == NULL) {
            cli_out("Memory %s, address index %d, table instance %d, got:\n",
                    sinfo.name, dyn_info->index, dyn_info->tbl_inst);
        } else {
            cli_out("Memory %s, address index %d, table instance %d (pipe %d), got:\n",
                    sinfo.name, dyn_info->index, dyn_info->tbl_inst, *((int *)ovrr_info));
        }
        bcma_testutil_pt_data_dump(mem_param->rdata, mem_param->wsize);
        cli_out("Expected:");
        bcma_testutil_pt_data_dump(mem_param->wdata, mem_param->wsize);
        cli_out("Mask:");
        bcma_testutil_pt_data_dump(mem_param->mask, mem_param->wsize);

        mem_param->err_cnt++;
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_w_entity_handle_clear(int unit, bcmdrd_sid_t sid,
                               bcmbd_pt_dyn_info_t *dyn_info,
                               void *ovrr_info,
                               void *user_data)
{
    mem_rand_w_param_t *mem_param = (mem_rand_w_param_t*)user_data;
    uint32_t wdata[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    /* skip instance that user not specified */
    if (dyn_info->tbl_inst < mem_param->inst_start
        || dyn_info->tbl_inst > mem_param->inst_end) {
        SHR_EXIT();
    }

    sal_memset(wdata, 0, sizeof(wdata));

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_write(unit, sid, dyn_info, NULL, wdata));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_w_table_handle(int unit, bcmdrd_sid_t sid, void *user_data)
{
    mem_rand_w_param_t *mem_param = (mem_rand_w_param_t*)user_data;
    uint32_t total_addr;

    SHR_FUNC_ENTER(unit);

    /* Skip memories which can not be tested */
    if (bcmdrd_pt_attr_is_fifo(unit, sid) || bcmdrd_pt_is_notest(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (bcma_testutil_drv_pt_skip(unit, sid, BCMA_TESTUTIL_PT_MEM_RD_WR) == TRUE) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Adjust index and instance testing range */
    mem_param->index_start = mem_param->user_index_start;
    mem_param->index_end = mem_param->user_index_end;
    mem_param->inst_start = mem_param->user_inst_start;
    mem_param->inst_end = mem_param->user_inst_end;
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_address_adjust(unit, sid,
                                              &mem_param->index_start,
                                              &mem_param->index_end,
                                              BCMA_TESTUTIL_PT_MEM_RD_WR));
    total_addr = mem_param->index_end - mem_param->index_start + 1;
    if (total_addr <= 1) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_inst_adjust(unit, sid, &mem_param->inst_start,
                                      &mem_param->inst_end));

    /* Log */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%5d/%5d: %s\n"), sid,
                            (int)mem_param->num_of_sid,
                            bcmdrd_pt_sid_to_name(unit, sid)));

    SHR_IF_ERR_EXIT
        (bcma_testutil_cond_init(unit, sid, BCMA_TESTUTIL_PT_MEM_RD_WR));

    SHR_IF_ERR_EXIT
        (bcma_testutil_progress_update
            (mem_param->pc, mem_param->cur_iter * mem_param->num_of_sid + sid));

    /* Disable SER control. */
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_ser_ctrl(unit, sid, false));

    /* Get memory mask */
    mem_param->wsize = bcmdrd_pt_entry_wsize(unit, sid);
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_mask_get
            (unit, sid, mem_param->index_start, mem_param->mask,
             mem_param->wsize, FALSE));

    /* Choose a value for increment that is relatively prime to total index */
    /* coverity[dont_call : FALSE] */
    mem_param->inc_addr = mem_rand_w_get_rel_prime(total_addr, sal_rand());
    mem_param->first_addr = -1;
    mem_param->cur_addr = mem_param->index_start;
    mem_param->cur_data = mem_param->init_data;

exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_w_table_cleanup(int unit, bcmdrd_sid_t sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_cond_cleanup(unit, sid, BCMA_TESTUTIL_PT_MEM_RD_WR));
exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_w_test_cb(int unit, void *bp, uint32_t option)
{
    mem_rand_w_param_t *mem_param = (mem_rand_w_param_t *)bp;
    bcma_testutil_pt_iter_t pt_iter;
    const char *name;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    name = mem_param->name;
    if (sal_strcmp(mem_param->name, "*") == 0) {
        name = BCMA_TESTUTIL_PT_ALL_MEMS;
    }

    bcma_testutil_pt_iterate_init(&pt_iter);
    pt_iter.unit = unit;
    pt_iter.name = name;
    pt_iter.blktype = mem_param->blktype;
    pt_iter.table_handle = mem_rand_w_table_handle;
    pt_iter.table_cleanup = mem_rand_w_table_cleanup;
    pt_iter.entity_next_id = mem_rand_w_entity_next_id;
    pt_iter.entity_handle[0] = mem_rand_w_entity_handle_write;
    pt_iter.entity_handle[1] = mem_rand_w_entity_handle_read;
    pt_iter.entity_handle_pipes[1] = true;
    pt_iter.entity_handle[2] = mem_rand_w_entity_handle_clear;
    pt_iter.user_data = bp;

    if (mem_param->pc != NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    mem_param->pc = bcma_testutil_progress_start
                        (unit, mem_param->num_of_sid *mem_param->iter,
                         BCMA_TESTUTIL_PROGRESS_REPORT_INTERVAL);
    SHR_NULL_CHECK(mem_param->pc, SHR_E_MEMORY);

    for (mem_param->cur_iter = 0; mem_param->cur_iter < mem_param->iter;
         mem_param->cur_iter++) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_iterate(&pt_iter));
    }

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

static int
mem_rand_w_init_cb(int unit, void *bp, uint32_t option)
{
    mem_rand_w_param_t *mem_param = (mem_rand_w_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (mem_param->seed) {
        sal_srand(mem_param->seed);
    }

    /* Disable background hardware update */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_hw_update(unit,
                                        mem_param->name,
                                        BCMA_TESTUTIL_PT_MEM_RD_WR,
                                        false));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Disable hardware update failed.\n")));
    }

    SHR_FUNC_EXIT();
}

static int
mem_rand_w_cleanup_cb(int unit, void *bp, uint32_t option)
{
    mem_rand_w_param_t *mem_param = (mem_rand_w_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Enable background hardware update */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_hw_update(unit,
                                        mem_param->name,
                                        BCMA_TESTUTIL_PT_MEM_RD_WR,
                                        true));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Enable hardware update failed.\n")));
    }

    SHR_FUNC_EXIT();
}

static bcma_test_proc_t mem_rand_w_proc[] = {
    {
        .desc = "disable hardware update.\n",
        .cb = mem_rand_w_init_cb,
    },
    {
        .desc = "write a random address set with XOR data.\n",
        .cb = mem_rand_w_test_cb,
    },
    {
        .desc = "enable hardware update.\n",
        .cb = mem_rand_w_cleanup_cb,
    },
};

static bcma_test_op_t mem_rand_w_op = {
    .select = mem_rand_w_select,
    .parser = mem_rand_w_parser,
    .help = mem_rand_w_help,
    .recycle = mem_rand_w_recycle,
    .procs = mem_rand_w_proc,
    .proc_count = COUNTOF(mem_rand_w_proc)
};

bcma_test_op_t *
bcma_testcase_mem_rand_w_op_get(void)
{
    return &mem_rand_w_op;
}
