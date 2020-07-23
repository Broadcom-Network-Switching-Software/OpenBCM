/*! \file bcma_testcase_counter_width.c
 *
 * Broadcom test cases for verifying the counter register width.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <sal/sal_libc.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_pt_iterate.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/testcase/bcma_testcase_counter_width.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

typedef struct counter_width_test_param_s {
    char *sname;    /* Symbol Name */
    bcmdrd_fid_t *fid_list;
} counter_width_test_param_t;

static int
counter_width_select(int unit)
{
    COMPILER_REFERENCE(unit);
    return true;
}

static int
counter_width_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                  uint32_t flags, void **bp)
{
    bcma_cli_parse_table_t pt;
    char *sname = "*";
    counter_width_test_param_t *p = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    /* Allocate the resource for savng test related parameters */
    SHR_ALLOC(p, sizeof(*p), "bcmaTestCaseCtrWidthPrm");
    if (p == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Record allocated resource to buffer pointer for
     * recycling the resource at Teardown phase
     */
    *bp = (void *)p;
    sal_memset(p, 0, sizeof(*p));

    /* Populate the default parameters */
    p->sname = NULL;

    /* Initialize the parse table for parsing the input arguments from CLI */
    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "CounterReg", "str",
                             &sname, NULL);

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
    if (sname) {
        p->sname = sal_strdup(sname);
    }

    /* Clear parse table */
    bcma_cli_parse_table_done(&pt);

    if (p->sname == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Should specify a memory name\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
counter_width_help(int unit, void *bp)
{
    counter_width_test_param_t *p = (counter_width_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    cli_out("  CounterReg - Counter Register name\n");
    cli_out("  ====\n");
    cli_out("  sname=%s\n", p->sname ? p->sname : "(null)");
    return;
}

static void
counter_width_recycle(int unit, void *bp)
{
    counter_width_test_param_t *p = (counter_width_test_param_t *)bp;
    if (p == NULL) {
        return;
    }
    if (p->sname) {
        SHR_FREE(p->sname);
    }
    if (p->fid_list) {
        SHR_FREE(p->fid_list);
    }
    return;
}

static int
counter_width_table_handle(int unit, bcmdrd_sid_t sid, void *user_data)
{
    /* Skip NOTEST register/memory */
    if (bcmdrd_pt_is_notest(unit, sid)) {
        return SHR_E_UNAVAIL;
    }

    /* Skip register/memory which can not be tested */
    if (bcmdrd_pt_is_readonly(unit, sid)) {
        return SHR_E_UNAVAIL;
    }

    /* Should be counter register/memory */
    if (!bcmdrd_pt_is_counter(unit, sid)) {
        return SHR_E_UNAVAIL;
    }

    /* Should be per-port register/memory */
    if (!bcmdrd_pt_is_port_reg(unit, sid) &&
        !bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        return SHR_E_UNAVAIL;
    }

    if (bcma_testutil_drv_pt_skip(unit, sid, BCMA_TESTUTIL_PT_REG_RD_WR) == TRUE) {
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

static int
counter_width_entity_handle(int unit, bcmdrd_sid_t sid,
                            bcmbd_pt_dyn_info_t *dyn_info,
                            void *read_ovrr_info,
                            void *user_data)
{
    bcmdrd_sym_info_t sinfo;
    uint32_t odata[BCMDRD_MAX_PT_WSIZE], rdata[BCMDRD_MAX_PT_WSIZE],
             wdata[BCMDRD_MAX_PT_WSIZE];
    size_t wsize;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dyn_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));
    wsize = sinfo.entry_wsize;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Test %s (index %"PRId32" inst %"PRId32")\n"),
                            bcmdrd_pt_sid_to_name(unit, sid),
                            dyn_info->index, dyn_info->tbl_inst));

    /*
     * Step 1: Store the old value
     */
    SHR_IF_ERR_EXIT(bcmbd_pt_read(unit, sid, dyn_info, NULL, odata, wsize));

    /*
     * Step 2: Write all one.
     */
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_mask_get
            (unit, sid, dyn_info->index, wdata, wsize, FALSE));
    SHR_IF_ERR_EXIT
        (bcmbd_pt_write(unit, sid, dyn_info, NULL, wdata));

    /*
     * Step 3: Read and compare.
     */
    SHR_IF_ERR_EXIT
        (bcmbd_pt_read(unit, sid, dyn_info, read_ovrr_info, rdata, wsize));

    if (bcma_testutil_pt_data_comp(rdata, wdata, wdata, wsize) != 0) {
        cli_out("Memory %s, address index %"PRId32", "
                "table instance %"PRId32", got:\n",
                sinfo.name, dyn_info->index, dyn_info->tbl_inst);
        bcma_testutil_pt_data_dump(rdata, wsize);
        cli_out("Expected:");
        bcma_testutil_pt_data_dump(wdata, wsize);
        cli_out("Mask:");
        bcma_testutil_pt_data_dump(wdata, wsize);

        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /*
     * Step 4: Write the old value back.
     */
    SHR_IF_ERR_EXIT(bcmbd_pt_write(unit, sid, dyn_info, NULL, odata));

exit:
    SHR_FUNC_EXIT();
}

static int
counter_width_test(int unit, void *bp, uint32_t option)
{
    counter_width_test_param_t *p = (counter_width_test_param_t *)bp;
    bcma_testutil_pt_iter_t pt_iter;

    SHR_FUNC_ENTER(unit);

    bcma_testutil_pt_iterate_init(&pt_iter);
    pt_iter.unit = unit;
    pt_iter.blktype = BCMA_TESTUTIL_PT_ALL_BLOCK_TYPE;
    pt_iter.table_handle = counter_width_table_handle;
    pt_iter.entity_handle[0] = counter_width_entity_handle;
    pt_iter.entity_handle_pipes[0] = true;
    pt_iter.user_data = (void*)p;

    if (sal_strcmp(p->sname, "*") == 0) {
        pt_iter.name = BCMA_TESTUTIL_PT_ALL_REGS;
        SHR_IF_ERR_EXIT(bcma_testutil_pt_iterate(&pt_iter));
        pt_iter.name = BCMA_TESTUTIL_PT_ALL_MEMS;
        SHR_IF_ERR_EXIT(bcma_testutil_pt_iterate(&pt_iter));
    } else {
        pt_iter.name = p->sname;
        SHR_IF_ERR_EXIT(bcma_testutil_pt_iterate(&pt_iter));
    }

exit:
    SHR_FUNC_EXIT();
}

#define BCMA_TEST_PROC_DESCRIPTION \
    "Execute the counter register width test.\n" \
    "    Step 1: Store the old value\n" \
    "    Step 2: Write all one.\n" \
    "    Step 3: Read and compare.\n" \
    "    Step 4: Write the old value back.\n" \
    ""

static bcma_test_proc_t counter_width_proc[] = {
    {
        .desc = BCMA_TEST_PROC_DESCRIPTION,
        .cb = counter_width_test,
    },
};

#undef BCMA_TEST_PROC_DESCRIPTION

static bcma_test_op_t counter_width_op = {
    .select = counter_width_select,
    .parser = counter_width_parser,
    .help = counter_width_help,
    .recycle = counter_width_recycle,
    .procs = counter_width_proc,
    .proc_count = COUNTOF(counter_width_proc),
};

bcma_test_op_t *
bcma_testcase_counter_width_op_get(void)
{
    return &counter_width_op;
}
