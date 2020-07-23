/*! \file bcma_testcase_reg_wr.c
 *
 * Test case for register write/read.
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
#include <sal/sal_sleep.h>

#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/util/bcma_testutil_pt_iterate.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/testcase/bcma_testcase_reg_wr.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST


#define REG_WR_PARAM_HELP \
    "  Register     - The register to be tested\n"\
    "  PatZero      - Fill register with 0\n"\
    "  PatOne       - Fill register with 1\n"\
    "  PatFive      - Fill register with 5\n"\
    "  PatA         - Fill register with a\n"\
    "  PipeStart    - Starting pipe\n"\
    "  PipeEnd      - Ending pipe\n"\
    "  BlockType    - The block type which the registers belong to\n"\
    "\nExample:\n"\
    "  tr 3 bt=ipipe pz=0 po=0 pf=0 pa=1\n"\
    "  tr 3\n"

typedef struct reg_wr_param_s {
    char name[BCMA_TESTUTIL_STR_MAX_SIZE];

    /* Starting and ending table instance */
    int inst_start;
    int inst_end;

    int blktype;

    /* Data pattern applied or not */
    int pat_zero;
    int pat_one;
    int pat_five;
    int pat_a;

    uint32_t pattern;

    /* Number of registers which can not get expected value */
    int err_cnt;
} reg_wr_param_t;

static int
reg_wr_select(int unit)
{
    return true;
}

static int
reg_wr_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                   reg_wr_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    char *name = NULL, *inst_start = NULL, *inst_end = NULL, *blktype = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    param->inst_start = -1;
    param->inst_end = -1;
    param->blktype = -1;
    param->pat_zero = 1;
    param->pat_one = 1;
    param->pat_five = 1;
    param->pat_a = 1;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Register", "str", &name, NULL);
    bcma_cli_parse_table_add(&pt, "PipeStart", "str", &inst_start, NULL);
    bcma_cli_parse_table_add(&pt, "PipeEnd", "str", &inst_end, NULL);
    bcma_cli_parse_table_add(&pt, "BlockType", "str", &blktype, NULL);
    bcma_cli_parse_table_add(&pt, "PatZero", "bool", &param->pat_zero, NULL);
    bcma_cli_parse_table_add(&pt, "PatOne", "bool", &param->pat_one, NULL);
    bcma_cli_parse_table_add(&pt, "PatFive", "bool", &param->pat_five, NULL);
    bcma_cli_parse_table_add(&pt, "PatA", "bool", &param->pat_a, NULL);

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

    if (name != NULL) {
        sal_strncpy(param->name, name, sizeof(param->name) - 1);
    } else {
        sal_strcpy(param->name, "*");
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
reg_wr_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a, uint32_t flags, void **bp)
{
    reg_wr_param_t *reg_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    reg_param = sal_alloc(sizeof(reg_wr_param_t), "bcmaTestCaseRegWrPrm");
    if (reg_param == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(reg_param, 0, sizeof(reg_wr_param_t));

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    SHR_IF_ERR_EXIT
        (reg_wr_parse_param(unit, cli, a, reg_param, show));

    *bp = (void*)reg_param;

exit:
    if (SHR_FUNC_ERR()) {
        if (reg_param != NULL) {
            sal_free(reg_param);
        }
    }

    SHR_FUNC_EXIT();
}

static void
reg_wr_help(int unit, void *bp)
{
    cli_out("%s", REG_WR_PARAM_HELP);
}

static void
reg_wr_recycle(int unit, void *bp)
{
    return;
}

static int
reg_wr_disable_hw_update_cb(int unit, void *bp, uint32_t option)
{
    reg_wr_param_t *reg_param = (reg_wr_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_hw_update(unit,
                                        reg_param->name,
                                        BCMA_TESTUTIL_PT_REG_RD_WR,
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
reg_wr_table_handle(int unit, bcmdrd_sid_t sid, void *user_data)
{
    reg_wr_param_t *reg_param = (reg_wr_param_t *)user_data;

    SHR_FUNC_ENTER(unit);

    /* Skip registers which can not be tested */
    if (bcmdrd_pt_is_notest(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (bcma_testutil_drv_pt_skip(unit, sid, BCMA_TESTUTIL_PT_REG_RD_WR) == TRUE) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_inst_adjust(unit, sid, &reg_param->inst_start,
                                      &reg_param->inst_end));

    SHR_IF_ERR_EXIT
        (bcma_testutil_cond_init(unit, sid, BCMA_TESTUTIL_PT_REG_RD_WR));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s\n"),
                 bcmdrd_pt_sid_to_name(unit, sid)));

exit:
    SHR_FUNC_EXIT();
}

static int
reg_wr_table_cleanup(int unit, bcmdrd_sid_t sid)
{
    SHR_FUNC_ENTER(unit);


    if (bcma_testutil_cond_cleanup(unit, sid, BCMA_TESTUTIL_PT_REG_RD_WR)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
reg_wr_entity_handle(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info, void *read_ovrr_info,
                     void *user_data)
{
    reg_wr_param_t *reg_param = (reg_wr_param_t *)user_data;
    bcmdrd_sym_info_t sinfo;
    uint32_t rdata[2], rrdata[2], wdata[2], pat[2], mask[2] = {0}, notmask[2];
    int wsize, i;

    SHR_FUNC_ENTER(unit);

    if (dyn_info->tbl_inst < reg_param->inst_start
        || dyn_info->tbl_inst > reg_param->inst_end) {
        SHR_EXIT();
    }

    /*
     * Step 1: store the old value
     * Step 2: write test data
     * Step 3: read and compare test data
     * Step 4: write the old value back
    */

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));
    wsize = sinfo.entry_wsize;
    if (wsize > 2) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_mask_get
            (unit, sid, dyn_info->index, mask, wsize, FALSE));

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_read(unit, sid, dyn_info, NULL, rdata, wsize));

    /* Apply data pattern */
    for (i = 0; i < wsize; i++) {
        pat[i] = reg_param->pattern;
    }

    BCMA_TESTUTIL_AND(pat, mask, wsize);

    BCMA_TESTUTIL_SET(notmask, mask, wsize);
    BCMA_TESTUTIL_NOT(notmask, wsize);

    BCMA_TESTUTIL_SET(wdata, rdata, wsize);
    BCMA_TESTUTIL_AND(wdata, notmask, wsize);
    BCMA_TESTUTIL_OR(wdata, pat, wsize);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_write(unit, sid, dyn_info, NULL, wdata));

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_read(unit, sid, dyn_info, read_ovrr_info,
                                   rrdata, wsize));

    BCMA_TESTUTIL_AND(rrdata, mask, wsize);

    if (bcma_testutil_pt_data_comp(rrdata, pat, mask, wsize) != 0) {
        if (read_ovrr_info == NULL) {
            cli_out("Register %s, address index %d, table instance %d, got:",
                    sinfo.name, dyn_info->index, dyn_info->tbl_inst);
        } else {
            cli_out("Register %s, address index %d, table instance %d (pipe %d), got:",
                    sinfo.name, dyn_info->index, dyn_info->tbl_inst, *((int *)read_ovrr_info));
        }
        bcma_testutil_pt_data_dump(rrdata, wsize);
        cli_out("Expected:");
        bcma_testutil_pt_data_dump(pat, wsize);
        cli_out("Mask:");
        bcma_testutil_pt_data_dump(mask, wsize);

        reg_param->err_cnt++;
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_write(unit, sid, dyn_info, NULL, rdata));

exit:
    SHR_FUNC_EXIT();
}

static int
reg_wr_test_pattern(int unit, reg_wr_param_t *reg_param)
{
    bcma_testutil_pt_iter_t pt_iter;
    const char *name = reg_param->name;

    SHR_FUNC_ENTER(unit);

    if (sal_strcmp(reg_param->name, "*") == 0) {
        name = BCMA_TESTUTIL_PT_ALL_REGS;
    }

    bcma_testutil_pt_iterate_init(&pt_iter);
    pt_iter.unit = unit;
    pt_iter.name = name;
    pt_iter.blktype = reg_param->blktype;
    pt_iter.table_handle = reg_wr_table_handle;
    pt_iter.table_cleanup = reg_wr_table_cleanup;
    pt_iter.entity_handle[0] = reg_wr_entity_handle;
    pt_iter.entity_handle_pipes[0] = true;
    pt_iter.user_data = (void*)reg_param;

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_iterate(&pt_iter));

    if (reg_param->err_cnt > 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Register w/r test failed.\n")));
    }

    SHR_FUNC_EXIT();
}

static int
reg_wr_test_all_0_cb(int unit, void *bp, uint32_t option)
{
    reg_wr_param_t *reg_param = (reg_wr_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!reg_param->pat_zero) {
        SHR_EXIT();
    }

    reg_param->pattern = 0x00000000;

    SHR_IF_ERR_EXIT
        (reg_wr_test_pattern(unit, reg_param));

exit:
    SHR_FUNC_EXIT();
}

static int
reg_wr_test_all_1_cb(int unit, void *bp, uint32_t option)
{
    reg_wr_param_t *reg_param = (reg_wr_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!reg_param->pat_one) {
        SHR_EXIT();
    }

    reg_param->pattern = 0xffffffff;

    SHR_IF_ERR_EXIT
        (reg_wr_test_pattern(unit, reg_param));

exit:
    SHR_FUNC_EXIT();
}

static int
reg_wr_test_all_5_cb(int unit, void *bp, uint32_t option)
{
    reg_wr_param_t *reg_param = (reg_wr_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!reg_param->pat_five) {
        SHR_EXIT();
    }

    reg_param->pattern = 0x55555555;

    SHR_IF_ERR_EXIT
        (reg_wr_test_pattern(unit, reg_param));

exit:
    SHR_FUNC_EXIT();
}

static int
reg_wr_test_all_a_cb(int unit, void *bp, uint32_t option)
{
    reg_wr_param_t *reg_param = (reg_wr_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    if (!reg_param->pat_a) {
        SHR_EXIT();
    }

    reg_param->pattern = 0xaaaaaaaa;

    SHR_IF_ERR_EXIT
        (reg_wr_test_pattern(unit, reg_param));

exit:
    SHR_FUNC_EXIT();
}

static int
reg_wr_enable_hw_update_cb(int unit, void *bp, uint32_t option)
{
    reg_wr_param_t *reg_param = (reg_wr_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_pt_hw_update(unit,
                                        reg_param->name,
                                        BCMA_TESTUTIL_PT_REG_RD_WR,
                                        true));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Enable hardware update failed.\n")));
    }

    SHR_FUNC_EXIT();
}

static bcma_test_proc_t reg_wr_proc[] = {
    {
        .desc = "disable hardware update\n",
        .cb = reg_wr_disable_hw_update_cb,
    },
    {
        .desc = "write->read...compare all 0\n",
        .cb = reg_wr_test_all_0_cb,
    },
    {
        .desc = "write->read...compare all 1\n",
        .cb = reg_wr_test_all_1_cb,
    },
    {
        .desc = "write->read...compare all 5\n",
        .cb = reg_wr_test_all_5_cb,
    },
    {
        .desc = "write->read...compare all a\n",
        .cb = reg_wr_test_all_a_cb,
    },
    {
        .desc = "enable hardware update\n",
        .cb = reg_wr_enable_hw_update_cb,
    },
};

static bcma_test_op_t reg_wr_op = {
    .select = reg_wr_select,
    .parser = reg_wr_parser,
    .help = reg_wr_help,
    .recycle = reg_wr_recycle,
    .procs = reg_wr_proc,
    .proc_count = COUNTOF(reg_wr_proc)
};

bcma_test_op_t *
bcma_testcase_reg_wr_op_get(void)
{
    return &reg_wr_op;
}

