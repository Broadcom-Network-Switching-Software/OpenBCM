/*! \file bcma_testcase_mem_mbist.c
 *
 * Test case for Memory built-in self testing.
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
#include <bcmlt/bcmlt.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/testcase/bcma_testcase_mem_mbist.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TEST
#define SLEEP_TIME (500000)
#define EXTRA_DELAY (10000)

#define MEM_MBIST_DEFAULT_DMA_MODE                 (TRUE)
#define MEM_MBIST_DEFAULT_TEST_ALL                 (-1)
#define MEM_MBIST_DMA_DEFAULT_HW_COMMAND_INTERVAL  (0x3)
#define MEM_MBIST_DMA_DEFAULT_WAITING_TIME         (10) /* sec */
#define MEM_MBIST_DMA_DEFAULT_POLLING_INTERVAL     (100) /* usec */

#define MEM_MBIST_PARAM_HELP \
    "  ID      - The testing item index                    \n"\
    "  DMAmode - Whether to run BIST in DMA mode if support\n"\
    "\nExample:\n"\
    "    tr 515\n"\
    "    tr 515 dma=true\n"\
    "    tr 515 dma=true id=1\n"

typedef struct mem_mbist_param_s {
    /* testing item id */
    int testing_id;

    /* whether to run in DMA mode */
    int dma_mode;
} mem_mbist_param_t;

static int
mem_mbist_select(int unit)
{
    return true;
}

static int
mem_mbist_parse_param(bcma_cli_t *cli, bcma_cli_args_t *a,
                      mem_mbist_param_t *param, bool show)
{
    bcma_cli_parse_table_t pt;
    bool pt_is_init = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* setup parse table */
    bcma_cli_parse_table_init(cli, &pt);
    pt_is_init = true;
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "ID", "int",
                                  &(param->testing_id), NULL));
    SHR_IF_ERR_EXIT
        (bcma_cli_parse_table_add(&pt, "DMAmode", "bool",
                                  &param->dma_mode, NULL));

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

exit:
    if (pt_is_init == true) {
        bcma_cli_parse_table_done(&pt);
    }
    SHR_FUNC_EXIT();
}

static int
mem_mbist_parser(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                 uint32_t flags, void **bp)
{
    mem_mbist_param_t *mem_param = NULL;
    bool show = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bp, SHR_E_PARAM);

    SHR_ALLOC(mem_param, sizeof(mem_mbist_param_t), "bcmaTestCaseMemMbistPrm");
    SHR_NULL_CHECK(mem_param, SHR_E_MEMORY);


    if (flags & BCMA_TEST_ENGINE_F_HELP) {
        show = true;
    }

    /* init */
    mem_param->testing_id = MEM_MBIST_DEFAULT_TEST_ALL;
    mem_param->dma_mode = MEM_MBIST_DEFAULT_DMA_MODE;

    /* parse */
    SHR_IF_ERR_EXIT
        (mem_mbist_parse_param(cli, a, mem_param, show));

    *bp = (void *)mem_param;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(mem_param);
    }
    SHR_FUNC_EXIT();
}

static void
mem_mbist_help(int unit, void *bp)
{
    const bcma_testutil_mem_mbist_entry_t *test_array = NULL;
    const bcma_testutil_mem_mbist_dma_entry_t *test_array_dma = NULL;
    int test_count, test_i, rv;

    cli_out("%s", MEM_MBIST_PARAM_HELP);

    rv = bcma_testutil_drv_mem_mbist_info(unit, &test_array, &test_count);
    if (SHR_FAILURE(rv)) {
        return;
    }
    if (test_count > 0 && test_array == NULL) {
        return;
    }

    cli_out("Available testing items :\n");
    for (test_i = 0; test_i < test_count; test_i++) {
        cli_out("    [%2d] %s\n", test_i, test_array[test_i].name);
    }

    rv = bcma_testutil_drv_mem_mbist_dma_info(unit, &test_array_dma, &test_count);
    if (SHR_FAILURE(rv)) {
        return;
    }
    if (test_count > 0 && test_array_dma == NULL) {
        return;
    }

    cli_out("Available testing items w/ DMA:\n");
    for (test_i = 0; test_i < test_count; test_i++) {
        cli_out("    [%2d] %s\n", test_i, test_array_dma[test_i].name);
    }
}

static void
mem_mbist_recycle(int unit, void *bp)
{
    /* mem_param would be freed in testdb_run_teardown */
    return;
}

static int
mem_mbist_run_single(int unit,
                     const bcma_testutil_mem_mbist_entry_t *test_info)
{
    int op_count, op_i;
    int control_write_idx = 0;
    int data_write_idx = 0;
    int verify_idx = 0;
    uint32_t read_data = 0;
    uint32_t read_control = 0;
    const uint32_t *array_opcode;
    const uint32_t *pattern_control_write;
    const uint32_t *pattern_data_write;
    const uint64_t *pattern_status_verify;
    const uint8_t *array_control_write;
    const uint32_t *array_data_write;
    const uint64_t *array_status_verify;
    bcmdrd_sid_t sid_control;
    bcmdrd_sid_t sid_write_data;
    bcmdrd_sid_t sid_read_data;
    int error_count = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(test_info, SHR_E_PARAM);

    array_opcode = test_info->array_opcode;
    pattern_control_write = test_info->pattern_control_write;
    pattern_data_write = test_info->pattern_data_write;
    pattern_status_verify = test_info->pattern_status_verify;
    array_control_write = test_info->array_control_write;
    array_data_write = test_info->array_data_write;
    array_status_verify = test_info->array_status_verify;
    op_count =  test_info->op_count;

    SHR_NULL_CHECK(pattern_control_write, SHR_E_PARAM);
    SHR_NULL_CHECK(pattern_data_write, SHR_E_PARAM);
    SHR_NULL_CHECK(pattern_status_verify, SHR_E_PARAM);
    SHR_NULL_CHECK(array_control_write, SHR_E_PARAM);
    SHR_NULL_CHECK(array_data_write, SHR_E_PARAM);
    SHR_NULL_CHECK(array_status_verify, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_name_to_sid(unit, "TOP_UC_TAP_CONTROLr", &sid_control));
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_name_to_sid(unit, "TOP_UC_TAP_WRITE_DATAr", &sid_write_data));
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_name_to_sid(unit, "TOP_UC_TAP_READ_DATAr", &sid_read_data));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Number of instruction :%"PRId32"\n"), op_count));

    for (op_i = 0; op_i < op_count; op_i++) {

        uint8_t loop;
        uint8_t loop_i;
        uint8_t command;
        uint8_t command_type;
        bcmbd_pt_dyn_info_t dyn_info;
        uint32_t write_data;
        uint32_t verify_data;
        uint32_t verify_mask;

        dyn_info.index = 0;
        dyn_info.tbl_inst = 0;

        if ((op_i % 100 == 0) || (op_i == op_count - 1)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "running... (%"PRId32".%"PRId32" %%)\n"),
                     ((op_i + 1) * 10000 / op_count) / 100,
                     ((op_i + 1) * 10000 / op_count) % 100));
        }

        loop = ((array_opcode[op_i] & BCMA_TESTCASE_MEM_MBIST_OP_LOOP_MASK) >>
                BCMA_TESTCASE_MEM_MBIST_OP_LOOP_SHIFT) + 1;
        command = (array_opcode[op_i] &
                   BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_MASK);
        command_type = (array_opcode[op_i] &
                        BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_MASK);


        for (loop_i = 0; loop_i < loop; loop_i ++) {
            uint32_t pt_wsize;

            if (command_type == BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_0) {
                switch(command) {
                    case BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_SLEEP:
                        sal_usleep(SLEEP_TIME);
                        break;
                    case BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_READ_DATA:
                        pt_wsize = bcmdrd_pt_entry_wsize(unit, sid_read_data);
                        SHR_IF_ERR_EXIT(
                            bcmbd_pt_read(unit, sid_read_data, &dyn_info, NULL,
                                          &read_data, pt_wsize));
                        break;
                    case BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_READ_CONTROL:
                        pt_wsize = bcmdrd_pt_entry_wsize(unit, sid_control);
                        SHR_IF_ERR_EXIT(
                            bcmbd_pt_read(unit, sid_control, &dyn_info, NULL,
                                          &read_control, pt_wsize));
                        break;
                    case BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_WRITE_DATA:
                        write_data = array_data_write[data_write_idx];
                        SHR_IF_ERR_EXIT(
                            bcmbd_pt_write(unit, sid_write_data,
                                           &dyn_info, NULL, &write_data));
                        if (loop_i == loop - 1) {
                            data_write_idx++;
                        }
                        break;
                    default:
                        /* should not happen */
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                 }
            } else {
                int pattern_idx;

                if ((command & BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_MASK)
                     == BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_OTHER) {
                    pattern_idx = -1;
                } else {
                    pattern_idx =
                        (command &
                         BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_MASK) >>
                        BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_SHIFT;
                }

                switch(command_type) {
                    case BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_WRITE_CONTROL:
                        if (pattern_idx == -1) {
                            write_data = array_control_write[control_write_idx];
                        } else {
                            write_data = pattern_control_write[pattern_idx];
                        }
                        SHR_IF_ERR_EXIT(
                            bcmbd_pt_write(unit, sid_control, &dyn_info,
                                           NULL, &write_data));
                        if (loop_i == loop - 1 && pattern_idx == -1) {
                            control_write_idx++;
                        }
                        break;
                    case BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_WRITE_DATA_CONTROL:
                        if (pattern_idx == -1) {
                            write_data = array_data_write[data_write_idx];
                        } else {
                            write_data = pattern_data_write[pattern_idx];
                        }
                        SHR_IF_ERR_EXIT(
                            bcmbd_pt_write(unit, sid_write_data, &dyn_info,
                                           NULL, &write_data));
                        sal_usleep(EXTRA_DELAY);
                        write_data = pattern_control_write[0];
                        SHR_IF_ERR_EXIT(
                            bcmbd_pt_write(unit, sid_control, &dyn_info,
                                           NULL, &write_data));
                        if (loop_i == loop - 1 && pattern_idx == -1) {
                            data_write_idx++;
                        }
                        break;
                    case BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_STATUS_VERIFY:
                        if (pattern_idx == -1) {
                            verify_data = array_status_verify[verify_idx] >>
                                          32;
                            verify_mask = array_status_verify[verify_idx] &
                                          0xffffffff;
                        } else {
                            verify_data = pattern_status_verify[pattern_idx] >>
                                          32;
                            verify_mask = pattern_status_verify[pattern_idx] &
                                          0xffffffff;
                        }
                        if((read_data & verify_mask) != verify_data) {
                            LOG_ERROR(BSL_LOG_MODULE,
                                      (BSL_META_U(unit,
                                                  "ERROR Got: 0x%"PRIx32" "
                                                  "Expected: 0x%"PRIx32","
                                                  "     mask 0x%"PRIx32".\n"),
                                                  read_data, verify_data,
                                                  verify_mask));
                            error_count++;
                        }
                        if (loop_i == loop - 1 && pattern_idx == -1) {
                            verify_idx++;
                        }
                        break;
                    default:
                        /* should not happen */
                        LOG_ERROR(BSL_LOG_MODULE,
                                  (BSL_META_U(unit,
                                              "something error in command\n")));
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }

    if (error_count != 0) {
        cli_out("error_count: %"PRId32"\n", error_count);
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int mem_mbist_dma_write_word(const uint8_t* buf_start, uint8_t** buf_ptr,
                                    uint32_t buf_size, uint32_t word)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(buf_ptr, SHR_E_PARAM);

    /* check it there is enough memory for writing */
    if (buf_size - ((*buf_ptr) - buf_start) < 4) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* write */
    *(*buf_ptr) = word & 0xff;
    *(*buf_ptr + 1) = (word >> 8) & 0xff;
    *(*buf_ptr + 2) = (word >> 16) & 0xff;
    *(*buf_ptr + 3) = (word >> 24) & 0xff;
    *buf_ptr += 4;

exit:
    SHR_FUNC_EXIT();
}

static int
mem_mbist_dma_run_single(int unit,
                         const bcma_testutil_mem_mbist_dma_entry_t *test_info)
{
    bcmdrd_sid_t sid_dma_data;
    bcmdrd_sid_t sid_dma_control;
    bcmdrd_fid_t fid_dma_control_hw_interval;
    bcmdrd_fid_t fid_dma_control_enable;
    bcmdrd_fid_t fid_dma_control_start;
    bcmdrd_sid_t sid_dma_status;
    bcmdrd_fid_t sid_dma_status_program_end;
    bcmdrd_fid_t sid_dma_status_program_pass;
    int dma_size = 0;
    uint8_t *dma_buf = NULL, *dma_buf_ptr = NULL;
    uint64_t dma_addr = 0;
    uint32_t op_idx, data_idx;
    uint32_t curtime, testing_end, testing_pass;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(test_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_name_to_sid(unit, "TOP_CPU2TAP_DMA_CMD_MEMm", &sid_dma_data));

    dma_size = test_info->entry_num *
               BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, sid_dma_data));
    dma_buf = bcmdrd_hal_dma_alloc(unit, dma_size, "bcmaTestCaseMBISTDmaMem",
                                   &dma_addr);

    if (dma_buf == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(dma_buf, 0, dma_size);

    /* Fill DMA data */
    dma_buf_ptr = dma_buf;
    data_idx = 0;
    for (op_idx = 0; op_idx < test_info->op_count; op_idx++) {
        uint8_t command, loop, loop_idx, word_idx;

        command = (test_info->array_opcode[op_idx] &
                   BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_MASK)
                   >> BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_SHIFT;
        loop = ((test_info->array_opcode[op_idx] &
                 BCMA_TESTCASE_MEM_MBIST_DMA_OP_LOOP_MASK)
                >> BCMA_TESTCASE_MEM_MBIST_DMA_OP_LOOP_SHIFT) + 1;

        for (loop_idx = 0; loop_idx < loop; loop_idx++) {
            switch(command) {
                case BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_WRITE_DATA:
                    SHR_IF_ERR_EXIT
                        (mem_mbist_dma_write_word
                            (dma_buf, &dma_buf_ptr, dma_size,
                             test_info->array_data[data_idx]));
                    data_idx++;
                    break;
                case BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_WRITE_LONG_PATTERN:
                    for (word_idx = 0;
                         word_idx < bcmdrd_pt_entry_wsize(unit, sid_dma_data);
                         word_idx++) {
                        SHR_IF_ERR_EXIT
                            (mem_mbist_dma_write_word
                                (dma_buf, &dma_buf_ptr, dma_size,
                                 test_info->array_long_pattern[word_idx]));
                    }
                    break;
                default:
                    /* write pattern N (N = command) */
                    SHR_IF_ERR_EXIT
                        (mem_mbist_dma_write_word
                            (dma_buf, &dma_buf_ptr, dma_size,
                             test_info->array_pattern[command]));
                    break;
            }
        }
    }

    /* sanity check, the buffer should be full-filled */
    if (dma_buf_ptr - dma_buf != dma_size) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* enable dma */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_name_to_sid(unit, "TOP_CPU2TAP_DMA_CMD_MEM_CONTROLr",
                               &sid_dma_control));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_name_to_fid(unit, sid_dma_control,
                                     "DMA_CMD_MEM_ENABLE",
                                     &fid_dma_control_enable));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_name_to_fid(unit, sid_dma_control,
                                     "DEFAULT_WAIT_TIME",
                                     &fid_dma_control_hw_interval));
    SHR_IF_ERR_EXIT
            (bcma_testutil_pt_field_set
                (unit, sid_dma_control, fid_dma_control_enable,
                 0, 0, 1));

    SHR_IF_ERR_EXIT
            (bcma_testutil_pt_field_set
                (unit, sid_dma_control, fid_dma_control_hw_interval,
                 0, 0, MEM_MBIST_DMA_DEFAULT_HW_COMMAND_INTERVAL));


    /* write data */
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_mem_range_write
            (unit, sid_dma_data, 0, test_info->entry_num - 1, 0, dma_addr));

    /* start */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_name_to_fid(unit, sid_dma_control, "START",
                                     &fid_dma_control_start));

    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_set
            (unit, sid_dma_control, fid_dma_control_start, 0, 0, 1));

    /* wait mbist running until end */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_name_to_sid(unit, "TOP_CPU2TAP_DMA_CMD_MEM_STATUSr",
                               &sid_dma_status));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_name_to_fid(unit, sid_dma_status, "PROGRAM_END",
                                     &sid_dma_status_program_end));
    curtime = 0;
    testing_end = 0;
    while (curtime < MEM_MBIST_DMA_DEFAULT_WAITING_TIME * SECOND_USEC) {

        sal_usleep(MEM_MBIST_DMA_DEFAULT_POLLING_INTERVAL);

        curtime += MEM_MBIST_DMA_DEFAULT_POLLING_INTERVAL;

        if (curtime % SECOND_USEC == 0) {
            cli_out(".");
        }

        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_field_get
                (unit, sid_dma_status, sid_dma_status_program_end,
                 0, 0, &testing_end));

        if (testing_end == 1) {
            break;
        }
    }
    if (testing_end == 0) {
        cli_out("timeout error\n");
        SHR_IF_ERR_EXIT(SHR_E_TIMEOUT);
    }

    /* read the result */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_name_to_fid(unit, sid_dma_status, "PASS",
                                     &sid_dma_status_program_pass));
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_get
            (unit, sid_dma_status, sid_dma_status_program_pass, 0, 0,
             &testing_pass));

    if (testing_pass == 1) {
        cli_out("pass, duration = %"PRId32" usec\n", curtime);
    } else {
        bcmdrd_sid_t sid_dma_error_msg;
        int idx;

        cli_out("failed\n");

        /* dump debug message */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_name_to_sid(unit, "TOP_CPU2TAP_DMA_RESULT_ERROR_MEMm",
                                   &sid_dma_error_msg));

        cli_out("error dump\n");
        for (idx = 0; idx <= bcmdrd_pt_index_max(unit, sid_dma_error_msg);
             idx++) {
            bcmbd_pt_dyn_info_t dyn_info;
            uint32_t pt_data[BCMDRD_MAX_PT_WSIZE], pt_size;

            dyn_info.index = idx;
            dyn_info.tbl_inst = 0;
            pt_size = bcmdrd_pt_entry_wsize(unit, sid_dma_error_msg);

            SHR_IF_ERR_EXIT
                (bcmbd_pt_read(unit, sid_dma_error_msg,
                               &dyn_info, NULL, pt_data, pt_size));
            cli_out("%"PRId32" ",idx);
            bcma_testutil_pt_data_dump(pt_data, pt_size);
        }

        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (dma_buf != NULL) {
        bcmdrd_hal_dma_free(unit, dma_size, dma_buf, dma_addr);
    }
    SHR_FUNC_EXIT();
}

static int
mem_mbist_run(int unit, void *bp, uint32_t option)
{
    mem_mbist_param_t *mem_param = (mem_mbist_param_t *)bp;
    const bcma_testutil_mem_mbist_entry_t     *test_array = NULL;
    const bcma_testutil_mem_mbist_dma_entry_t *test_array_dma = NULL;
    int test_start, test_end, test_count, test_i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_param, SHR_E_PARAM);

    /* preconfig if required */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcma_testutil_drv_mem_mbist_preconfig(unit), SHR_E_UNAVAIL);

    /* stop counter collection during testing */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_CONTROL", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "COLLECTION_ENABLE", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

    /* get mbist infomation */
    if (mem_param->dma_mode == TRUE) {
        SHR_IF_ERR_EXIT(
            bcma_testutil_drv_mem_mbist_dma_info(unit, &test_array_dma, &test_count));
        if (test_count > 0) {
            SHR_NULL_CHECK(test_array_dma, SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_EXIT(
            bcma_testutil_drv_mem_mbist_info(unit, &test_array, &test_count));
        if (test_count > 0) {
            SHR_NULL_CHECK(test_array, SHR_E_PARAM);
        }
    }
    if (test_count == 0) {
        cli_out("no patterns needed to run\n");
        SHR_EXIT();
    }

    /* run */
    if (mem_param->testing_id != MEM_MBIST_DEFAULT_TEST_ALL) {
        if (mem_param->testing_id < 0 || mem_param->testing_id >= test_count) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit, "input testing id error\n")));
        }
        test_start = mem_param->testing_id;
        test_end = mem_param->testing_id;
    } else {
        test_start = 0;
        test_end = test_count - 1;
    }

    for (test_i = test_start; test_i <= test_end; test_i++) {
        const char *item_name;

        if (mem_param->dma_mode == TRUE) {
            item_name = test_array_dma[test_i].name;
        } else {
            item_name = test_array[test_i].name;
        }

        SHR_NULL_CHECK(item_name, SHR_E_PARAM);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Run testing %"PRId32" : %s (Total %"PRId32")\n"),
                             test_i, item_name,
                             test_end - test_start + 1));

        if (mem_param->dma_mode == TRUE) {
            SHR_IF_ERR_EXIT(
                mem_mbist_dma_run_single(unit, &(test_array_dma[test_i])));
        } else {
            SHR_IF_ERR_EXIT(
                mem_mbist_run_single(unit, &(test_array[test_i])));
        }
    }


exit:
    if (SHR_FUNC_ERR()) {
        cli_out("mem_mbist_run failed");
    }
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static bcma_test_proc_t mem_mbist_proc[] = {
    {
        .desc = "run Memory BIST\n",
        .cb = mem_mbist_run,
    },
};

static bcma_test_op_t mem_mbist_op = {
    .select = mem_mbist_select,
    .parser = mem_mbist_parser,
    .help = mem_mbist_help,
    .recycle = mem_mbist_recycle,
    .procs = mem_mbist_proc,
    .proc_count = COUNTOF(mem_mbist_proc),
};

bcma_test_op_t *
bcma_testcase_mem_mbist_op_get(void)
{
    return &mem_mbist_op;
}
