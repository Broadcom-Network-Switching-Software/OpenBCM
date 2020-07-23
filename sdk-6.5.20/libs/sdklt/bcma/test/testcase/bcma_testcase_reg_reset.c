/*! \file bcma_testcase_reg_reset.c
 *
 * Test case for register reset.
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
#include <bcma/test/testcase/bcma_testcase_reg_reset.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define REG_RESET_PARAM_HELP \
    "  Register     - The register to be tested\n"\
    "  PipeStart    - Starting pipe\n"\
    "  PipeEnd      - Ending pipe\n"\
    "  BlockType    - The block type which the registers belong to\n"\
    "\nExample:\n"\
    "  tr 1 bt=ipipe\n"\
    "  tr 1\n"

typedef struct reg_reset_param_s {
    char name[BCMA_TESTUTIL_STR_MAX_SIZE];

    /* Starting and ending table instance */
    int inst_start;
    int inst_end;

    int blktype;

    /* Number of registers which can not get expected value */
    int err_cnt;
} reg_reset_param_t;

static int
reg_reset_select(int unit)
{
    return true;
}

static int
reg_reset_parse_param(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                      reg_reset_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    char *name = NULL, *inst_start = NULL, *inst_end = NULL, *blktype = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    param->inst_start = -1;
    param->inst_end = -1;
    param->blktype = -1;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Register", "str", &name, NULL);
    bcma_cli_parse_table_add(&pt, "PipeStart", "str", &inst_start, NULL);
    bcma_cli_parse_table_add(&pt, "PipeEnd", "str", &inst_end, NULL);
    bcma_cli_parse_table_add(&pt, "BlockType", "str", &blktype, NULL);

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
reg_reset_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a, uint32_t flags, void **bp)
{
    reg_reset_param_t *reg_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    reg_param = sal_alloc(sizeof(reg_reset_param_t), "bcmaTestCaseRegResetPrm");
    if (reg_param == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(reg_param, 0, sizeof(reg_reset_param_t));

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    SHR_IF_ERR_EXIT
        (reg_reset_parse_param(unit, cli, a, reg_param, show));

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
reg_reset_help(int unit, void *bp)
{
    cli_out("%s", REG_RESET_PARAM_HELP);
}

static void
reg_reset_recycle(int unit, void *bp)
{
    return;
}

static int
reg_reset_table_handle(int unit, bcmdrd_sid_t sid, void *user_data)
{
    reg_reset_param_t *reg_param = (reg_reset_param_t *)user_data;

    SHR_FUNC_ENTER(unit);

    /* Skip registers which can not be tested */
    if (bcmdrd_pt_is_notest(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (bcma_testutil_drv_pt_skip(unit, sid, BCMA_TESTUTIL_PT_REG_RESET) == TRUE) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_inst_adjust(unit, sid, &reg_param->inst_start,
                                      &reg_param->inst_end));

    SHR_IF_ERR_EXIT
        (bcma_testutil_cond_init(unit, sid, BCMA_TESTUTIL_PT_REG_RESET));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s\n"),
                 bcmdrd_pt_sid_to_name(unit, sid)));

exit:
    SHR_FUNC_EXIT();
}

static int
reg_reset_table_cleanup(int unit, bcmdrd_sid_t sid)
{
    SHR_FUNC_ENTER(unit);

    if (bcma_testutil_cond_cleanup(unit, sid, BCMA_TESTUTIL_PT_REG_RESET)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
reg_reset_entity_handle(int unit, bcmdrd_sid_t sid,
                        bcmbd_pt_dyn_info_t *dyn_info, void *read_ovrr_info,
                        void *user_data)
{
    reg_reset_param_t *reg_param = (reg_reset_param_t *)user_data;
    const uint32_t *resetval = bcmdrd_pt_default_value_get(unit, sid);
    uint32_t data[2];
    bcmdrd_sym_info_t sinfo;
    int wsize;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(resetval, SHR_E_UNAVAIL);

    if (dyn_info->tbl_inst < reg_param->inst_start
        || dyn_info->tbl_inst > reg_param->inst_end) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));
    wsize = sinfo.entry_wsize;
    if (wsize > 2) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmbd_pt_read(unit, sid, dyn_info, read_ovrr_info, data, wsize));

    if (bcma_testutil_pt_data_comp(data, resetval, &resetval[wsize], wsize) != 0) {
        if (read_ovrr_info == NULL) {
            cli_out("Register %s, address index %d, table instance %d, got:",
                    sinfo.name, dyn_info->index, dyn_info->tbl_inst);
        } else {
            cli_out("Register %s, address index %d, table instance %d (pipe %d), got:",
                    sinfo.name, dyn_info->index, dyn_info->tbl_inst, *((int *)read_ovrr_info));
        }
        bcma_testutil_pt_data_dump(data, wsize);
        cli_out("Expected:");
        bcma_testutil_pt_data_dump(resetval, wsize);
        cli_out("Mask:");
        bcma_testutil_pt_data_dump((resetval + wsize), wsize);

        reg_param->err_cnt++;
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
reg_reset_test_cb(int unit, void *bp, uint32_t option)
{
    reg_reset_param_t *reg_param = (reg_reset_param_t *)bp;
    bcma_testutil_pt_iter_t pt_iter;
    const char *name;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    name = reg_param->name;
    if (sal_strcmp(reg_param->name, "*") == 0) {
        name = BCMA_TESTUTIL_PT_ALL_REGS;
    }

    bcma_testutil_pt_iterate_init(&pt_iter);
    pt_iter.unit = unit;
    pt_iter.name = name;
    pt_iter.blktype = reg_param->blktype;
    pt_iter.table_handle = reg_reset_table_handle;
    pt_iter.table_cleanup = reg_reset_table_cleanup;
    pt_iter.entity_handle[0] = reg_reset_entity_handle;
    pt_iter.entity_handle_pipes[0] = true;
    pt_iter.user_data = bp;

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_iterate(&pt_iter));

    if (reg_param->err_cnt > 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Register reset test failed.\n")));
    }

    SHR_FUNC_EXIT();
}

static bcma_test_proc_t reg_reset_proc[] = {
    {
        .desc = "read->compare: reset value\n",
        .cb = reg_reset_test_cb,
    },
};

static bcma_test_op_t reg_reset_op = {
    .select = reg_reset_select,
    .parser = reg_reset_parser,
    .help = reg_reset_help,
    .recycle = reg_reset_recycle,
    .procs = reg_reset_proc,
    .proc_count = COUNTOF(reg_reset_proc),
};

bcma_test_op_t *
bcma_testcase_reg_reset_op_get(void)
{
    return &reg_reset_op;
}

