/*! \file bcma_testcase_mem_reset.c
 *
 * Test case for memory reset.
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
#include <bcma/test/testcase/bcma_testcase_mem_reset.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define MEM_RESET_PARAM_HELP \
    "  Memory     - The memory to be tested\n"\
    "  IndexStart - Starting index\n"\
    "  IndexEnd   - Ending index\n"\
    "  PatZero    - Fill memory with 0\n"\
    "  PatOne     - Fill memory with 1\n"\
    "  PatFive    - Fill memory with 5\n"\
    "  PatA       - Fill memory with a\n"\
    "  PatChecker - Fill memory with alternating 0101... and 1010...\n"\
    "  PatInvckr  - Fill memory with alternating 1010... and 0101...\n"\
    "  PatLinear  - Write a unique value in every location\n"\
    "               to make sure all locations are distinct\n"\
    "  PatRandom  - Write a unique value in every location\n"\
    "               using pseudo-random data\n"\
    "  PipeStart  - Starting pipe\n"\
    "  PipeEnd    - Ending pipe\n"\
    "  BlockType  - The block type which the memories belong to\n"\
    "\nExample:\n"\
    "  tr 54 m=L2Xm is=min ie=max pz=1 po=1 pf=1 pa=1 pc=1 pi=1 pl=1 pr=1\n"\
    "  tr 54\n"

/* Data pattern type */
typedef enum mem_pattern_type_e {
    /* Keep writing same word */
    MEM_PAT_TYPE_PLAIN = 0,

    /* XOR 0xffffffff between entries */
    MEM_PAT_TYPE_XOR,

    /* Add 0xdeadbeef between word */
    MEM_PAT_TYPE_RANDOM,

    /* Add 1 between entries */
    MEM_PAT_TYPE_INCR
} mem_pattern_type_t;

/* User visiable pattern */
typedef enum mem_pattern_e {
    /* Fill memory with 0 */
    MEM_PAT_ZERO = 0,

    /* Fill memory with 1 */
    MEM_PAT_ONE,

    /* Fill memory with 5 */
    MEM_PAT_FIVE,

    /* Fill memory with a */
    MEM_PAT_A,

    /* Fill memory with alternating 0101... and 1010... */
    MEM_PAT_CHECKER,

    /* Fill memory with alternating 1010... and 0101... */
    MEM_PAT_INVCKR,

    /* Write a unique value in every location using increment data */
    MEM_PAT_LINEAR,

    /* Write a unique value in every location using pseudo-random data */
    MEM_PAT_RANDOM,

    MEM_PAT_COUNT
} mem_pattern_t;

typedef struct mem_reset_param_s {
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

    /* Data pattern applied or not */
    int pat_zero;
    int pat_one;
    int pat_five;
    int pat_a;
    int pat_checker;
    int pat_invckr;
    int pat_linear;
    int pat_random;

    /* Pattern type and seed */
    int pattern_type;
    uint32_t pattern_init_seed;
    uint32_t pattern_cur_seed;

    /* Buffer for write/read/mask data */
    uint32_t wdata[BCMDRD_MAX_PT_WSIZE];
    uint32_t rdata[BCMDRD_MAX_PT_WSIZE];
    uint32_t mask[BCMDRD_MAX_PT_WSIZE];

    /* Number of memories which can not get expected value */
    int err_cnt;

    /* Number of sid */
    size_t num_of_sid;

    /* progress report control */
    bcma_testutil_progress_ctrl_t *pc;
} mem_reset_param_t;

static int
mem_reset_select(int unit)
{
    return true;
}

static int
mem_reset_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      mem_reset_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    char *name = NULL, *index_start = NULL, *index_end = NULL;
    char *inst_start = NULL, *inst_end = NULL, *blktype = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    param->user_index_start = BCMA_TESTUTIL_TABLE_MIN_INDEX;
    param->user_index_end = BCMA_TESTUTIL_TABLE_MAX_INDEX;
    param->user_inst_start = -1;
    param->user_inst_end = -1;
    param->blktype = -1;
    param->pat_zero = 1;
    param->pat_one = 1;
    param->pat_five = 1;
    param->pat_a = 1;
    param->pat_checker = 1;
    param->pat_invckr = 1;
    param->pat_linear = 1;
    param->pat_random = 1;

    bcma_cli_parse_table_init(cli, &pt);
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Memory", "str", &name, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "IndexStart", "str", &index_start, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "IndexEnd", "str", &index_end, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PipeStart", "str", &inst_start, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PipeEnd", "str", &inst_end, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "BlockType", "str", &blktype, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PatZero", "bool", &param->pat_zero, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PatOne", "bool", &param->pat_one, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PatFive", "bool", &param->pat_five, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PatA", "bool", &param->pat_a, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PatChecker", "bool", &param->pat_checker, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PatInvckr", "bool", &param->pat_invckr, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PatLinear", "bool", &param->pat_linear, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PatRandom", "bool", &param->pat_random, NULL));

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
mem_reset_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                 uint32_t flags, void **bp)
{
    mem_reset_param_t *mem_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    mem_param = sal_alloc(sizeof(mem_reset_param_t), "bcmaTestCaseMemResetPrm");
    if (mem_param == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(mem_param, 0, sizeof(mem_reset_param_t));

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    SHR_IF_ERR_EXIT
        (mem_reset_parse_param(unit, cli, a, mem_param, show));

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
mem_reset_help(int unit, void *bp)
{
    cli_out("%s", MEM_RESET_PARAM_HELP);
}

static void
mem_reset_recycle(int unit, void *bp)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t *)bp;

    if (mem_param != NULL && mem_param->pc != NULL) {
        /* if testing is interrupted by ctrlc, need to stop progress thread */
        bcma_testutil_progress_done(mem_param->pc);
    }

    return;
}

static void
mem_reset_fill_pattern_data(uint32_t *data, const uint32_t *mask, int wsize,
                            int pat_type, uint32_t pat_seed, uint32_t index)
{
    int idx;

    switch (pat_type) {
    case MEM_PAT_TYPE_PLAIN:
    default:
        break;

    case MEM_PAT_TYPE_XOR:
        if (index % 2) {
            pat_seed ^= 0xffffffff;
        }
        break;

    case MEM_PAT_TYPE_RANDOM:
        pat_seed += (0xdeadbeef * index);
        break;

    case MEM_PAT_TYPE_INCR:
        pat_seed += index;
        break;
    }

    for (idx = 0; idx < wsize; idx++) {
        data[idx] = mask[idx] & pat_seed;
    }
}

static int
mem_reset_entity_handle_write(int unit, bcmdrd_sid_t sid,
                              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                              void *user_data)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t*)user_data;
    int wsize = bcmdrd_pt_entry_wsize(unit, sid);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_param, SHR_E_PARAM);

    /* skip index and instance that user not specified */
    if (dyn_info->tbl_inst < mem_param->inst_start
        || dyn_info->tbl_inst > mem_param->inst_end) {
        SHR_EXIT();
    }
    if (dyn_info->index < mem_param->index_start
        || dyn_info->index > mem_param->index_end) {
        SHR_EXIT();
    }

    mem_reset_fill_pattern_data(mem_param->wdata, mem_param->mask, wsize,
                                mem_param->pattern_type,
                                mem_param->pattern_cur_seed, dyn_info->index);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_write(unit, sid, dyn_info, NULL,
                                    mem_param->wdata));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_entity_handle_read(int unit, bcmdrd_sid_t sid,
                             bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                             void *user_data)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t*)user_data;
    const uint32_t *resetval = bcma_testutil_drv_pt_default_value_get(unit, sid);
    bcmdrd_sym_info_t sinfo;
    int wsize;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_param, SHR_E_PARAM);
    SHR_NULL_CHECK(resetval, SHR_E_UNAVAIL);

    /* skip index and instance that user not specified */
    if (dyn_info->tbl_inst < mem_param->inst_start
        || dyn_info->tbl_inst > mem_param->inst_end) {
        SHR_EXIT();
    }
    if (dyn_info->index < mem_param->index_start
        || dyn_info->index > mem_param->index_end) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));
    wsize = sinfo.entry_wsize;

    /* read and verify */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_read(unit, sid, dyn_info, ovrr_info,
                                   mem_param->rdata, wsize));

    if (bcma_testutil_pt_data_comp(mem_param->rdata, resetval, mem_param->mask, wsize) != 0) {
        cli_out("Memory %s, address index %d, table instance %d, got:\n",
                sinfo.name, dyn_info->index, dyn_info->tbl_inst);
        bcma_testutil_pt_data_dump(mem_param->rdata, wsize);
        cli_out("Expected:");
        bcma_testutil_pt_data_dump(resetval, wsize);
        cli_out("Mask:");
        bcma_testutil_pt_data_dump(mem_param->mask, wsize);

        mem_param->err_cnt++;
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_table_handle(int unit, bcmdrd_sid_t sid, void *user_data)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t*)user_data;
    int wsize, count;

    SHR_FUNC_ENTER(unit);

    /* Skip memories which can not be tested */
    if (bcmdrd_pt_attr_is_fifo(unit, sid) || bcmdrd_pt_is_notest(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (bcma_testutil_drv_pt_skip(unit, sid, BCMA_TESTUTIL_PT_MEM_RESET)) {
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
                                              BCMA_TESTUTIL_PT_MEM_RESET));
    count = mem_param->index_end - mem_param->index_start + 1;
    if (count <= 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_inst_adjust(unit, sid, &mem_param->inst_start,
                                      &mem_param->inst_end));

    /* Log */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "PAT%08x %5d/%5d: %s\n"), mem_param->pattern_init_seed,
                 sid, (int)mem_param->num_of_sid,
                 bcmdrd_pt_sid_to_name(unit, sid)));

    SHR_IF_ERR_EXIT
        (bcma_testutil_progress_update(mem_param->pc, sid));

    /* Disable SER control. */
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_ser_ctrl(unit, sid, false));

    /* Get memory mask */
    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_mask_get
            (unit, sid, mem_param->index_start, mem_param->mask, wsize, TRUE));

    /* init current seed */
    mem_param->pattern_cur_seed = mem_param->pattern_init_seed;

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_test_pattern(int unit, mem_reset_param_t *mem_param)
{
    bcma_testutil_pt_iter_t pt_iter;
    const char *name = mem_param->name;

    SHR_FUNC_ENTER(unit);

    if (sal_strcmp(mem_param->name, "*") == 0) {
        name = BCMA_TESTUTIL_PT_ALL_MEMS;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Test pattern %08x\n"),
                         mem_param->pattern_init_seed));

    if (mem_param->pc != NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    mem_param->pc = bcma_testutil_progress_start
                        (unit, mem_param->num_of_sid,
                         BCMA_TESTUTIL_PROGRESS_REPORT_INTERVAL);
    SHR_NULL_CHECK(mem_param->pc, SHR_E_MEMORY);

    /* Write all memories with data pattern. */
    bcma_testutil_pt_iterate_init(&pt_iter);
    pt_iter.unit = unit;
    pt_iter.name = name;
    pt_iter.blktype = mem_param->blktype;
    pt_iter.table_handle = mem_reset_table_handle;
    pt_iter.entity_handle[0] = mem_reset_entity_handle_write;
    pt_iter.user_data = (void*)mem_param;

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_iterate(&pt_iter));

    if (mem_param->err_cnt > 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Chip reset. */
    SHR_IF_ERR_EXIT
        (bcmbd_dev_reset(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_dev_init(unit));

    /* Clear progress indication. */
    SHR_IF_ERR_EXIT
        (bcma_testutil_progress_update(mem_param->pc, 0));

    /* Read all memories. */
    bcma_testutil_pt_iterate_init(&pt_iter);
    pt_iter.unit = unit;
    pt_iter.name = name;
    pt_iter.blktype = mem_param->blktype;
    pt_iter.table_handle = mem_reset_table_handle;
    pt_iter.entity_handle[0] = mem_reset_entity_handle_read;
    pt_iter.entity_handle_pipes[0] = true;
    pt_iter.user_data = (void*)mem_param;

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_iterate(&pt_iter));

    if (mem_param->err_cnt > 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Memory reset test failed.\n")));
    }
    if (mem_param->pc != NULL) {
        bcma_testutil_progress_done(mem_param->pc);
        mem_param->pc = NULL;
    }

    SHR_FUNC_EXIT();
}

static int
mem_reset_test_pat_zero_cb(int unit, void *bp, uint32_t option)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!mem_param->pat_zero) {
        SHR_EXIT();
    }

    /* Detect bits stuck at 1 */
    mem_param->pattern_type = MEM_PAT_TYPE_PLAIN;
    mem_param->pattern_init_seed = 0x00000000;

    SHR_IF_ERR_EXIT
        (mem_reset_test_pattern(unit, mem_param));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_test_pat_one_cb(int unit, void *bp, uint32_t option)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!mem_param->pat_one) {
        SHR_EXIT();
    }

    /* Detect bits stuck at 0 */
    mem_param->pattern_type = MEM_PAT_TYPE_PLAIN;
    mem_param->pattern_init_seed = 0xffffffff;

    SHR_IF_ERR_EXIT
        (mem_reset_test_pattern(unit, mem_param));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_test_pat_five_cb(int unit, void *bp, uint32_t option)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!mem_param->pat_five) {
        SHR_EXIT();
    }

    mem_param->pattern_type = MEM_PAT_TYPE_PLAIN;
    mem_param->pattern_init_seed = 0x55555555;

    SHR_IF_ERR_EXIT
        (mem_reset_test_pattern(unit, mem_param));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_test_pat_a_cb(int unit, void *bp, uint32_t option)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!mem_param->pat_a) {
        SHR_EXIT();
    }

    mem_param->pattern_type = MEM_PAT_TYPE_PLAIN;
    mem_param->pattern_init_seed = 0xaaaaaaaa;

    SHR_IF_ERR_EXIT
        (mem_reset_test_pattern(unit, mem_param));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_test_pat_checker_cb(int unit, void *bp, uint32_t option)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!mem_param->pat_checker) {
        SHR_EXIT();
    }

    /* XOR 5's and a's generate a pattern shown below.
     * this is useful to detecting horizontally or vertically shorted data lines.
     * 0 1 0 1 0 1 0 1
     * 1 0 1 0 1 0 1 0
     * 0 1 0 1 0 1 0 1
     * 1 0 1 0 1 0 1 0
     */
    mem_param->pattern_type = MEM_PAT_TYPE_XOR;
    mem_param->pattern_init_seed = 0x55555555;

    SHR_IF_ERR_EXIT
        (mem_reset_test_pattern(unit, mem_param));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_test_pat_invckr_cb(int unit, void *bp, uint32_t option)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!mem_param->pat_invckr) {
        SHR_EXIT();
    }

    mem_param->pattern_type = MEM_PAT_TYPE_XOR;
    mem_param->pattern_init_seed = 0xaaaaaaaa;

    SHR_IF_ERR_EXIT
        (mem_reset_test_pattern(unit, mem_param));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_test_pat_linear_cb(int unit, void *bp, uint32_t option)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!mem_param->pat_linear) {
        SHR_EXIT();
    }

    /*
     * Write a unique value in every location to make sure all
     * locations are distinct (detect shorted address bits).
     * The unique value used is the address so dumping the failed
     * memory may indicate where the bad data came from.
     */
    mem_param->pattern_type = MEM_PAT_TYPE_INCR;
    mem_param->pattern_init_seed = 0;

    SHR_IF_ERR_EXIT
        (mem_reset_test_pattern(unit, mem_param));

exit:
    SHR_FUNC_EXIT();
}

static int
mem_reset_test_pat_random_cb(int unit, void *bp, uint32_t option)
{
    mem_reset_param_t *mem_param = (mem_reset_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!mem_param->pat_random) {
        SHR_EXIT();
    }

    mem_param->pattern_type = MEM_PAT_TYPE_RANDOM;
    mem_param->pattern_init_seed = 0xd246fe4b;

    SHR_IF_ERR_EXIT
        (mem_reset_test_pattern(unit, mem_param));

exit:
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t mem_reset_proc[] = {
    {
        .desc = "fill memory with 0.\n",
        .cb = mem_reset_test_pat_zero_cb,
    },
    {
        .desc = "fill memory with 1.\n",
        .cb = mem_reset_test_pat_one_cb,
    },
    {
        .desc = "fill memory with 5.\n",
        .cb = mem_reset_test_pat_five_cb,
    },
    {
        .desc = "fill memory with a.\n",
        .cb = mem_reset_test_pat_a_cb,
    },
    {
        .desc = "fill memory with alternating 0101... and 1010...\n",
        .cb = mem_reset_test_pat_checker_cb,
    },
    {
        .desc = "fill memory with alternating 1010... and 0100...\n",
        .cb = mem_reset_test_pat_invckr_cb,
    },
    {
        .desc = "write a unique value in every location to make sure all locations are distinct.\n",
        .cb = mem_reset_test_pat_linear_cb,
    },
    {
        .desc = "write a unique value in every location using pseudo-random data.\n",
        .cb = mem_reset_test_pat_random_cb,
    },
};

static bcma_test_op_t mem_reset_op = {
    .select = mem_reset_select,
    .parser = mem_reset_parser,
    .help = mem_reset_help,
    .recycle = mem_reset_recycle,
    .procs = mem_reset_proc,
    .proc_count = COUNTOF(mem_reset_proc),
};

bcma_test_op_t *
bcma_testcase_mem_reset_op_get(void)
{
    return &mem_reset_op;
}
