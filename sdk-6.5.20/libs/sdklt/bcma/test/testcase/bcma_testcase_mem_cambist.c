/*! \file bcma_testcase_mem_cambist.c
 *
 * Test case for CAM built-in self testing.
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
#include <sal/sal_sleep.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_pt_iterate.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/testcase/bcma_testcase_mem_cambist.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST
#define CAMBIST_DEFAULT_WAITING_TIME             (10) /* mins */
#define CAMBIST_DEFAULT_POLLING_INTERVAL       (1000) /* usec */
#define CAMBIST_DEFAULT_POLLING_INTERVAL_LONG (50000) /* usec */


#define MEM_CAMBIST_PARAM_HELP \
    "  Memory     - The CAM to be tested\n"\
    "  PipeStart  - Starting pipe\n"\
    "  PipeEnd    - Ending pipe\n"\
    "\nExample:\n"\
    "    Run built-in self testing on the CAM related to IFP\n"\
    "      tr 516 m=IFP\n"\
    "    Run built-in self testing on instacne 0 only\n"\
    "      tr 516 ps=0 pe=0\n"\
    "    Run built-in self testing on all CAM\n"\
    "      tr 516\n"

typedef struct mem_cambist_param_s {
    /* Memory name */
    char name[BCMA_TESTUTIL_STR_MAX_SIZE];

    /* Starting and ending table instance */
    int user_inst_start;
    int user_inst_end;
} mem_cambist_param_t;


static int
mem_cambist_select(int unit)
{
    return true;
}

static int
mem_cambist_parse_param(bcma_cli_t *cli, bcma_cli_args_t *a,
                        mem_cambist_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;
    char *parse_name = NULL, *inst_start = NULL, *inst_end = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "Memory", "str", &parse_name, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PipeStart", "str", &inst_start, NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "PipeEnd", "str", &inst_end, NULL));

    /* parse start */
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

    /* Override parameter by user input */
    if (parse_name != NULL) {
        size_t parse_name_len = sal_strlen(parse_name);

        if (parse_name_len >= BCMA_TESTUTIL_STR_MAX_SIZE) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        sal_memcpy(param->name, parse_name, parse_name_len + 1);
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

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
mem_cambist_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                   uint32_t flags, void **bp)
{
    mem_cambist_param_t *mem_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(mem_param, sizeof(mem_cambist_param_t),
              "bcmaTestCaseMemCambistPrm");
    SHR_NULL_CHECK(mem_param, SHR_E_MEMORY);

    sal_strcpy(mem_param->name, BCMA_TESTUTIL_PT_ALL_REGS);
    mem_param->user_inst_start = -1;
    mem_param->user_inst_end = -1;

    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }
    SHR_IF_ERR_EXIT
        (mem_cambist_parse_param(cli, a, mem_param, show));

    *bp = (void *)mem_param;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(mem_param);
    }
    SHR_FUNC_EXIT();
}

static void
mem_cambist_help(int unit, void *bp)
{
    cli_out("%s", MEM_CAMBIST_PARAM_HELP);
}

static void
mem_cambist_recycle(int unit, void *bp)
{
    /* mem_param would be freed in testdb_run_teardown */
    return;
}

static int
mem_cambist_test_single(int unit,
                        uint32_t     reg_index,
                        bcmdrd_sid_t control_reg,
                        uint32_t     control_reg_inst,
                        bcmdrd_fid_t control_field_enable,
                        uint32_t     control_field_enable_value,
                        bcmdrd_fid_t control_field_mode,
                        uint32_t     control_field_mode_value,
                        bcmdrd_fid_t control_field_reset,
                        bcmdrd_sid_t result_reg,
                        int          result_reg_inst_start,
                        int          result_reg_inst_end,
                        bcmdrd_fid_t result_field_status,
                        uint32_t     result_field_status_expect)
{

    int result_reg_inst;
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_pt_index_valid(unit, control_reg,
                               control_reg_inst, reg_index)) {
        SHR_EXIT(); /* skip invalid inst or index */
    }

    /* de-asserting reset */
    if (control_field_reset != INVALIDf) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_field_set
                (unit, control_reg, control_field_reset,
                 reg_index, control_reg_inst, 0));
    }

    /* write mode value */
    if (control_field_mode != INVALIDf) {
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_info_get(unit, control_reg,
                                      control_field_mode, &finfo));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "  Setup %s (inst %"PRId32"), %s = %"PRId32"\n"),
                  bcmdrd_pt_sid_to_name(unit, control_reg), control_reg_inst,
                  finfo.name, control_field_mode_value));

        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_field_set(unit, control_reg, control_field_mode,
                                        reg_index,
                                        control_reg_inst,
                                        control_field_mode_value));
    }


    /* write enable value */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_info_get(unit, control_reg,
                                  control_field_enable, &finfo));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "  Setup %s (inst %"PRId32"), %s = %"PRId32"\n"),
              bcmdrd_pt_sid_to_name(unit, control_reg), control_reg_inst,
              finfo.name, control_field_enable_value));

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_set(unit, control_reg, control_field_enable,
                                    reg_index,
                                    control_reg_inst,
                                    control_field_enable_value));

    /* asserting reset */
    if (control_field_reset != INVALIDf) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_field_set
                (unit, control_reg, control_field_reset,
                 reg_index, control_reg_inst, 1));
    }


    /* check status (maybe need to check multiple result instance) */
    for (result_reg_inst = result_reg_inst_start;
         result_reg_inst <= result_reg_inst_end;
         result_reg_inst++) {

        uint32_t field_value_get = 0;
        uint32_t curtime = 0;
        int testing_pass = 0;

        if (!bcmdrd_pt_index_valid(unit, result_reg,
                                   result_reg_inst, reg_index)) {
            continue; /* skip invalid inst or index */
        }

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "  Check %s (inst %"PRId32")\n"),
                  bcmdrd_pt_sid_to_name(unit, result_reg), result_reg_inst));

        while (curtime < CAMBIST_DEFAULT_WAITING_TIME * MINUTE_USEC) {

            if (bcmdrd_feature_is_real_hw(unit) == true) {
                sal_usleep(CAMBIST_DEFAULT_POLLING_INTERVAL);
                curtime += CAMBIST_DEFAULT_POLLING_INTERVAL;
            } else {
                sal_usleep(CAMBIST_DEFAULT_POLLING_INTERVAL_LONG);
                curtime += CAMBIST_DEFAULT_POLLING_INTERVAL_LONG;
            }

            SHR_IF_ERR_EXIT
                (bcma_testutil_pt_field_get
                    (unit, result_reg, result_field_status,
                     reg_index, result_reg_inst, &field_value_get));

            if (field_value_get == result_field_status_expect) {
                testing_pass = 1;
                break;
            } else if ((result_field_status_expect & 0x2) &&
                        !(field_value_get & 0x2)) {
                /*
                 * bit 0 : bist_done
                 * bit 1 : bist_go
                 * CAMBIST engine did not start when bit 1 is zero, just exit
                 */
                testing_pass = 0;
                break;
            }

            if (curtime % (5 * SECOND_USEC) == 0) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Still in wait, %"PRId32" sec "\
                                         "(cur status : %"PRIu32")\n"),
                                        curtime / SECOND_USEC, field_value_get));
            }
        }

        if (testing_pass == 0) {
            cli_out("  Failed at %s (inst %"PRId32") %s = %"PRId32", "\
                    "  returned status is %"PRId32\
                    "  (expect:%"PRId32") at %s (inst = %"PRId32")\n",
                    bcmdrd_pt_sid_to_name(unit, control_reg),
                    control_reg_inst, finfo.name, control_field_enable_value,
                    field_value_get, result_field_status_expect,
                    bcmdrd_pt_sid_to_name(unit, result_reg),
                    result_reg_inst);
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else {
            if (curtime < SECOND_USEC) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "pass, run time = %"PRId32" usec\n"),
                                        curtime));
            } else {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "pass, run time = %"PRId32".02%"PRId32" sec\n"),
                                        curtime / SECOND_USEC,
                                        (curtime % SECOND_USEC) / 1000));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mem_cambist_table_handle(int unit, bcmdrd_sid_t sid, void *user_data)
{
    mem_cambist_param_t *mem_param = (mem_cambist_param_t*)user_data;
    bool is_bist;
    int control_inst_idx, control_inst_cnt, result_inst_cnt, index;
    bcma_testutil_mem_cambist_entry_t bist_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_cambist_info(unit, sid, &is_bist, &bist_info));

    if (is_bist == false) {
        SHR_EXIT();
    }

    /* skip if this register is not preferred by the user */
    if (sal_strcmp(mem_param->name, BCMA_TESTUTIL_PT_ALL_REGS)) {
        if (sal_strstr(bcmdrd_pt_sid_to_name(unit, bist_info.control_reg),
                       mem_param->name) == NULL) {
            SHR_EXIT();
        }
    }

    /* parameter check */
    control_inst_cnt = bcmdrd_pt_num_tbl_inst(unit, bist_info.control_reg);
    result_inst_cnt = bcmdrd_pt_num_tbl_inst(unit, bist_info.result_reg);
    if (control_inst_cnt != result_inst_cnt && control_inst_cnt != 1) {
        /*
         * if the acc_type of control register is unique,
         *   acc_type of result register should be unique too
         *    ==> the number of instance should be the same
         *
         * if the acc_type of control register is duplicate
         *   acc_type of result register should be unique
         *    ==> control_inst_cnt = 1,
         *        result_inst_cnt = the number of pipeline depends on chip
         */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "the instance number is not resonable for \
                               %s(%"PRId32") and %s(%"PRId32")\n"),
                   bcmdrd_pt_sid_to_name(unit, bist_info.control_reg),
                   control_inst_cnt,
                   bcmdrd_pt_sid_to_name(unit, bist_info.result_reg),
                   result_inst_cnt));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (bcmdrd_pt_index_min(unit, bist_info.control_reg) !=
        bcmdrd_pt_index_min(unit, bist_info.result_reg)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "the min index is different between \
                               %s(%"PRId32") and %s(%"PRId32")\n"),
                   bcmdrd_pt_sid_to_name(unit, bist_info.control_reg),
                   bcmdrd_pt_index_min(unit, bist_info.control_reg),
                   bcmdrd_pt_sid_to_name(unit, bist_info.result_reg),
                   bcmdrd_pt_index_min(unit, bist_info.result_reg)));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (bcmdrd_pt_index_max(unit, bist_info.control_reg) !=
        bcmdrd_pt_index_max(unit, bist_info.result_reg)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "the max index is different between \
                               %s(%"PRId32") and %s(%"PRId32")\n"),
                   bcmdrd_pt_sid_to_name(unit, bist_info.control_reg),
                   bcmdrd_pt_index_max(unit, bist_info.control_reg),
                   bcmdrd_pt_sid_to_name(unit, bist_info.result_reg),
                   bcmdrd_pt_index_max(unit, bist_info.result_reg)));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    /*
     * start testing
     *
     *   For each index
     *      For each instance of control register
     *        For each testing mode
     *          For each enable field
     *             For each enable value
     *                mem_cambist_test_single(is_setup = true)
     *                mem_cambist_test_single(is_setup = false)
     *             End for
     *          End for
     *       End for
     *     End for
     *  End for
     */
    for (index = bcmdrd_pt_index_min(unit, bist_info.control_reg);
         index <= bcmdrd_pt_index_max(unit, bist_info.control_reg); index ++)
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Test %s (index %d)\n"),
                  bcmdrd_pt_sid_to_name(unit, bist_info.control_reg), index));

        for (control_inst_idx = 0; control_inst_idx < control_inst_cnt;
             control_inst_idx++) {

            int mode_idx, mode_num;

            if (mem_param->user_inst_start != -1 &&
                mem_param->user_inst_end != -1) {
                if (control_inst_idx < mem_param->user_inst_start ||
                    control_inst_idx > mem_param->user_inst_end) {
                    continue;
                }
            }

            if (bist_info.control_field_mode == INVALIDf) {
                mode_num = 1;
            } else {
                mode_num = bist_info.control_field_mode_num;
            }

            for (mode_idx = 0; mode_idx < mode_num; mode_idx++) {

                int enable_idx;

                for (enable_idx = 0;
                     enable_idx < bist_info.control_field_enable_num;
                     enable_idx++) {

                    int enable_shift;

                    for (enable_shift = 0;
                         enable_shift < bist_info.control_field_enable_arr_count
                                        [enable_idx];
                         enable_shift++) {

                        int result_reg_inst_start;
                        int result_reg_inst_end;
                        int test_step;

                        /*
                         * if the number of control register is 1 (duplicate)
                         *   ==> check all instance of result register
                         * if the number of control register multiple (unique)
                         *   ==> check the corresponding instance of result register
                         */
                        if (control_inst_cnt == 1) {
                            result_reg_inst_start = 0;
                            result_reg_inst_end = result_inst_cnt - 1;
                        } else {
                            result_reg_inst_start = control_inst_idx;
                            result_reg_inst_end = control_inst_idx;
                        }


                        /*
                         * step 1. enable CAMBIST (enable value = 1 << enable_shift)
                         *           ex. if count = 3
                         *               we will test it with en_value = 1, 2 and 4
                         * step 2. disable CAMBIST (enable value = 0)
                         */
                        for (test_step = 1; test_step <= 2; test_step++) {

                            uint32_t status_expect = (test_step == 1)?
                                                     bist_info.result_field_status_expect: 0;
                            uint32_t enable_value = (test_step == 1)?
                                                    (1 << enable_shift): 0;

                            SHR_IF_ERR_EXIT
                                (mem_cambist_test_single
                                    (unit, index,
                                     bist_info.control_reg, control_inst_idx,
                                     bist_info.control_field_enable_arr[enable_idx],
                                     enable_value,
                                     bist_info.control_field_mode,
                                     mode_idx,
                                     bist_info.control_field_reset,
                                     bist_info.result_reg,
                                     result_reg_inst_start, result_reg_inst_end,
                                     bist_info.result_field_status,
                                     status_expect));
                        }
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
mem_cambist_run(int unit, void *bp, uint32_t option)
{
    mem_cambist_param_t *mem_param = (mem_cambist_param_t *)bp;
    bcma_testutil_pt_iter_t pt_iter;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_param, SHR_E_PARAM);

    bcma_testutil_pt_iterate_init(&pt_iter);
    pt_iter.unit = unit;
    pt_iter.name = BCMA_TESTUTIL_PT_ALL_REGS;
    pt_iter.blktype = BCMA_TESTUTIL_PT_ALL_BLOCK_TYPE;
    pt_iter.table_handle = mem_cambist_table_handle;
    pt_iter.user_data = (void *)mem_param;

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_iterate(&pt_iter));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "mem_cambist_run failed.\n")));
    }

    SHR_FUNC_EXIT();
}

static bcma_test_proc_t mem_cambist_proc[] = {
    {
        .desc = "run CAMBIST\n",
        .cb = mem_cambist_run,
    },
};

static bcma_test_op_t mem_cambist_op = {
    .select = mem_cambist_select,
    .parser = mem_cambist_parser,
    .help = mem_cambist_help,
    .recycle = mem_cambist_recycle,
    .procs = mem_cambist_proc,
    .proc_count = COUNTOF(mem_cambist_proc),
};

bcma_test_op_t *
bcma_testcase_mem_cambist_op_get(void)
{
    return &mem_cambist_op;
}
