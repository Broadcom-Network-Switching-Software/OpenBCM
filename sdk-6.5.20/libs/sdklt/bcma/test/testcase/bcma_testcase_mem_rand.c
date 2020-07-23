/*! \file bcma_testcase_mem_rand.c
 *
 * Test case for memory random address/data.
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
#include <bcma/test/testcase/bcma_testcase_mem_rand.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define MEM_RAND_PARAM_HELP \
    "  Memory     - The memory to be tested\n"\
    "  IndexStart - Starting index\n"\
    "  IndexEnd   - Ending index\n"\
    "  ITERation  - Number of iterations to repeat\n"\
    "  SEED       - The seed to use in the random function\n"\
    "  PipeStart  - Starting pipe\n"\
    "  PipeEnd    - Ending pipe\n"\
    "  BlockType  - The block type which the memories belong to\n"\
    "\nExample:\n"\
    "  tr 51 m=L2Xm is=min ie=max iter=10 seed=0x00decade\n"\
    "  tr 51\n"

#define MEM_RAND_GROUP_SIZE 4

typedef struct mem_rand_param_s {
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

    /* Seed used to generate address index and test data */
    uint32_t seed;

    /* Buffer for write/read/mask data */
    uint32_t wdata[MEM_RAND_GROUP_SIZE][BCMDRD_MAX_PT_WSIZE];
    uint32_t rdata[BCMDRD_MAX_PT_WSIZE];
    uint32_t mask[BCMDRD_MAX_PT_WSIZE];

    /* address index set */
    int addr_index[MEM_RAND_GROUP_SIZE];
    int addr_index_cur_pos;

    /* The number of memories which can not get expected value */
    int err_cnt;

    /* Number of sid */
    size_t num_of_sid;

    /* progress report control */
    bcma_testutil_progress_ctrl_t *pc;
} mem_rand_param_t;

static int
mem_rand_select(int unit)
{
    return true;
}

static int
mem_rand_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                     mem_rand_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    char *name = NULL, *index_start = NULL, *index_end = NULL;
    char *inst_start = NULL, *inst_end = NULL, *blktype = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    param->iter = 1500;
    param->seed = 0xdecade;
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
            (bcma_testutil_parse_table_index
                (index_start, &param->user_index_start));
    }

    if (index_end != NULL) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_parse_table_index
                (index_end, &param->user_index_end));
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
mem_rand_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                uint32_t flags, void **bp)
{
    mem_rand_param_t *mem_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    mem_param = sal_alloc(sizeof(mem_rand_param_t), "bcmaTestCaseMemRandPrm");
    if (mem_param == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(mem_param, 0, sizeof(mem_rand_param_t));

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    SHR_IF_ERR_EXIT
        (mem_rand_parse_param(unit, cli, a, mem_param, show));

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
mem_rand_help(int unit, void *bp)
{
    cli_out("%s", MEM_RAND_PARAM_HELP);
}

static void
mem_rand_recycle(int unit, void *bp)
{
    mem_rand_param_t *mem_param = (mem_rand_param_t *)bp;

    if (mem_param != NULL && mem_param->pc != NULL) {
        /* if testing is interrupted by ctrlc, need to stop progress thread */
        bcma_testutil_progress_done(mem_param->pc);
    }

    return;
}

static uint32_t
mem_rand_randword(void)
{
    uint32_t word = 0;
    int idx;

    /*
     * This should make all 32 bits fairly random
     */
    for (idx = 0; idx < 3; idx++) {
        /* Rotate left 7 */
        word = (word << 7) | ((word >> 25) & 0x7f);
        /* Add some random bits */
        /* coverity[dont_call] */
        word ^= (uint32_t)sal_rand();
    }

    return word;
}

static int
mem_rand_randnum(int max)
{
    /* coverity[dont_call] */
    return (sal_rand() >> 4) % max;
}

static int
mem_rand_entity_handle_write(int unit, bcmdrd_sid_t sid,
                             bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                             void *user_data)
{
    mem_rand_param_t *mem_param = (mem_rand_param_t*)user_data;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_param, SHR_E_PARAM);

    /* skip instance that user not specified */
    if (dyn_info->tbl_inst < mem_param->inst_start
        || dyn_info->tbl_inst > mem_param->inst_end) {
        SHR_EXIT();
    }

    if (mem_param->addr_index_cur_pos < 0 ||
        mem_param->addr_index_cur_pos >= MEM_RAND_GROUP_SIZE) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_write
            (unit, sid, dyn_info, NULL,
             mem_param->wdata[mem_param->addr_index_cur_pos]));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_entity_handle_read(int unit, bcmdrd_sid_t sid,
                            bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                            void *user_data)
{
    mem_rand_param_t *mem_param = (mem_rand_param_t*)user_data;
    bcmdrd_sym_info_t sinfo;
    int wsize;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_param, SHR_E_PARAM);

    /* skip instance that user not specified */
    if (dyn_info->tbl_inst < mem_param->inst_start
        || dyn_info->tbl_inst > mem_param->inst_end) {
        SHR_EXIT();
    }

    if (mem_param->addr_index_cur_pos < 0 ||
        mem_param->addr_index_cur_pos >= MEM_RAND_GROUP_SIZE) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));

    wsize = sinfo.entry_wsize;

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_read(unit, sid, dyn_info, ovrr_info,
                                   mem_param->rdata, wsize));

    if (bcma_testutil_pt_data_comp(mem_param->rdata,
                                   mem_param->wdata[mem_param->addr_index_cur_pos],
                                   mem_param->mask, wsize) != 0) {
        if (ovrr_info == NULL) {
            cli_out("Memory %s, address index %d, table instance %d, got:\n",
                    sinfo.name, dyn_info->index, dyn_info->tbl_inst);
        } else {
            cli_out("Memory %s, address index %d, table instance %d (pipe %d), got:\n",
                    sinfo.name, dyn_info->index, dyn_info->tbl_inst, *((int *)ovrr_info));
        }
        bcma_testutil_pt_data_dump(mem_param->rdata, wsize);
        cli_out("Expected:");
        bcma_testutil_pt_data_dump(mem_param->wdata[mem_param->addr_index_cur_pos], wsize);
        cli_out("Mask:");
        bcma_testutil_pt_data_dump(mem_param->mask, wsize);

        mem_param->err_cnt++;
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_entity_handle_clear(int unit, bcmdrd_sid_t sid,
                             bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                             void *user_data)
{
    mem_rand_param_t *mem_param = (mem_rand_param_t*)user_data;
    uint32_t wdata[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_param, SHR_E_PARAM);

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
mem_rand_table_handle(int unit, bcmdrd_sid_t sid, void *user_data)
{
    mem_rand_param_t *mem_param = (mem_rand_param_t*)user_data;
    int wsize, count, i, j;

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
    count = mem_param->index_end - mem_param->index_start + 1;
    if (count < MEM_RAND_GROUP_SIZE) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_inst_adjust(unit, sid, &mem_param->inst_start,
                                      &mem_param->inst_end));

    SHR_IF_ERR_EXIT
        (bcma_testutil_cond_init(unit, sid, BCMA_TESTUTIL_PT_MEM_RD_WR));

    /* Log */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%5d/%5d: %s\n"), sid,
                            (int)mem_param->num_of_sid,
                            bcmdrd_pt_sid_to_name(unit, sid)));

    SHR_IF_ERR_EXIT
        (bcma_testutil_progress_update
            (mem_param->pc, mem_param->cur_iter * mem_param->num_of_sid + sid));

    /* Disable SER control. */
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_ser_ctrl(unit, sid, false));

    /* Get memory mask */
    wsize = bcmdrd_pt_entry_wsize(unit, sid);
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_mask_get
            (unit, sid, mem_param->index_start, mem_param->mask, wsize, FALSE));

    /* Generate random address */
    mem_param->addr_index_cur_pos = -1;
    for (i = 0; i < MEM_RAND_GROUP_SIZE; i++) {
        mem_param->addr_index[i] = mem_param->index_start +
                                   mem_rand_randnum(count);

        for (j = 0; j < i; j++) {
            if (mem_param->addr_index[i] == mem_param->addr_index[j]) {
                i--;
                break;
            }
        }
    }

    /* Generate test data */
    for (i = 0; i < MEM_RAND_GROUP_SIZE; i++) {
        for (j = 0; j < wsize; j++) {
            mem_param->wdata[i][j] = mem_rand_randword() & mem_param->mask[j];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_table_cleanup(int unit, bcmdrd_sid_t sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_cond_cleanup(unit, sid, BCMA_TESTUTIL_PT_MEM_RD_WR));
exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_entity_next_id(int unit, void *user_data)
{
    mem_rand_param_t *mem_param = (mem_rand_param_t*)user_data;

    if (mem_param == NULL) {
        return -1;
    }

    mem_param->addr_index_cur_pos++;

    if (mem_param->addr_index_cur_pos < MEM_RAND_GROUP_SIZE) {
        return mem_param->addr_index[mem_param->addr_index_cur_pos];
    } else {
        /* end this traverse */
        mem_param->addr_index_cur_pos = -1;
        return -1;
    }
}

static int
mem_rand_test_cb(int unit, void *bp, uint32_t option)
{
    mem_rand_param_t *mem_param = (mem_rand_param_t *)bp;
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
    pt_iter.table_handle = mem_rand_table_handle;
    pt_iter.table_cleanup = mem_rand_table_cleanup;
    pt_iter.entity_next_id = mem_rand_entity_next_id;
    pt_iter.entity_handle[0] = mem_rand_entity_handle_write;
    pt_iter.entity_handle[1] = mem_rand_entity_handle_read;
    pt_iter.entity_handle_pipes[1] = true;
    pt_iter.entity_handle[2] = mem_rand_entity_handle_clear;
    pt_iter.user_data = bp;

    if (mem_param->pc != NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    mem_param->pc = bcma_testutil_progress_start
                        (unit, mem_param->num_of_sid * mem_param->iter,
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
mem_rand_init_cb(int unit, void *bp, uint32_t option)
{
    mem_rand_param_t *mem_param = (mem_rand_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    sal_srand(mem_param->seed);

    /* Disable background hardware update */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_hw_update(unit,
                                        mem_param->name,
                                        BCMA_TESTUTIL_PT_MEM_RD_WR,
                                        false));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_rand_cleanup_cb(int unit, void *bp, uint32_t option)
{
    mem_rand_param_t *mem_param = (mem_rand_param_t *)bp;

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

static bcma_test_proc_t mem_rand_proc[] = {
    {
        .desc = "disable hardware update and generate seed for random address/data.\n",
        .cb = mem_rand_init_cb,
    },
    {
        .desc = "fill/verify random address and data.\n",
        .cb = mem_rand_test_cb,
    },
    {
        .desc = "enable hardware update.\n",
        .cb = mem_rand_cleanup_cb,
    },
};

static bcma_test_op_t mem_rand_op = {
    .select = mem_rand_select,
    .parser = mem_rand_parser,
    .help = mem_rand_help,
    .recycle = mem_rand_recycle,
    .procs = mem_rand_proc,
    .proc_count = COUNTOF(mem_rand_proc)
};

bcma_test_op_t *
bcma_testcase_mem_rand_op_get(void)
{
    return &mem_rand_op;
}
