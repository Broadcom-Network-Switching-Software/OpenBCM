/*! \file bcma_testutil_ser_flip_check.c
 *
 *  SER utility.
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
#include <sal/sal_sem.h>
#include <sal/sal_time.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbol_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcmlt/bcmlt.h>

#include <bcmptm/bcmptm_ser_testutil.h>

#include <bcma/test/util/bcma_testutil_ser_flip_check.h>
#include <bcma/test/util/bcma_testutil_ser.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

/* Data pattern, 0x0000 0000 */
#define SER_TEST_PATTERN_ALL0 0
/* Data pattern, 0xffff ffff */
#define SER_TEST_PATTERN_ALL1 1
/* Data pattern, 0x5555 5555 */
#define SER_TEST_PATTERN_CHB  2
/* Data pattern, 0xaaaa aaaa */
#define SER_TEST_PATTERN_ICHB 3
/* Default data pattern */
#define SER_TEST_PATTERN_DEFAULT     SER_TEST_PATTERN_CHB

#define SER_TEST_DATA_GET(_pattern, _base_word) \
    do { \
        if (_pattern == SER_TEST_PATTERN_ALL1) { \
            _base_word = 0xFFFFFFFF; \
        } else if (_pattern == SER_TEST_PATTERN_CHB) { \
            _base_word = 0x55555555; \
        } else if (_pattern == SER_TEST_PATTERN_ICHB) { \
            _base_word = 0xAAAAAAAA; \
        } else { \
            _base_word = 0x00000000; \
        } \
    } while(0)

/* Write memory data */
#define SER_WR_ONLY_MODE         (1 << 0)
/* Read and check memory data */
#define SER_RD_ONLY_MODE         (1 << 1)
/* Read, write and check memory data */
#define SER_RD_WR_MODE_DEFAULT   (SER_WR_ONLY_MODE | SER_RD_ONLY_MODE)

/* debug message */
#define SER_NON_QUIET_MODE_DEFAULT   0
/* Print debug message */
#define SER_DEBUG_MODE   0

#define SER_IH_PARAM_HELP \
    "This test is intended to be used for in house SER testing. The test can be\n" \
    "to write all SW accessible memories on chip and read them back to check for\n" \
    "flips. The assumption is that this test will be run at regular intervals in a\n" \
    "loop that is part of a master soc script as per test requirements. The test\n" \
    "provides information on the total memory tested, total number of flips and\n" \
    "the logical coordinate of each flip. However it does not provide information\n" \
    "about the physical coordinate of each flip.\n" \
    "\n" \
    "Configuration parameters passed from CLI:\n" \
    "TestPat: Test Pattern used to fill the memories\n" \
    "        0: All 0s\n" \
    "        1: All 1s\n" \
    "        2: Logical checker board\n" \
    "        3: Logical inverse checker board\n" \
    "RdWrMode: Whether the test writes or reads the memories\n" \
    "        1: Test only writes all the memories under test\n" \
    "        2: Test only reads back all memories under test and counts flips\n" \
    "        3: Test 1 & 2\n"

#define SER_IH_EXAMPLES \
            "\nExamples:\n"\
            "  tr 901 QuietMode=0\n"\
            "  tr 901 RdWrMode=3 TestPat=2\n"

static void
bcma_testutil_ser_mem_skip(int unit, bcmdrd_sid_t sid, bool *skip)
{
    int index_count = 0;
    const char *pt_name = NULL;
    int rv, max_index, recovery_type;
    int inst_num, copy_num, check_type;

    *skip = FALSE;
    if (bcmdrd_pt_is_reg(unit, sid) ||
        bcmdrd_pt_is_valid(unit, sid) == 0 ||
        bcmdrd_pt_is_readonly(unit, sid) ||
        bcmdrd_pt_attr_is_fifo(unit, sid)) {
        *skip = TRUE;
        return;
    }
    pt_name = bcmdrd_pt_sid_to_name(unit, sid);
    rv = bcma_testutil_ser_pt_status_get(unit, pt_name, 0, &max_index,
                                         &inst_num, &copy_num, &check_type,
                                         &recovery_type);
    if (SHR_FAILURE(rv)) {
        *skip = TRUE;
        return;
    }
    (void)bcmptm_ser_testutil_index_valid(unit, sid, 0, &index_count);
    if (index_count == 0) {
        *skip = TRUE;
        return;
    }
    if (bcma_testutil_drv_pt_skip(unit, sid, BCMA_TESTUTIL_PT_MEM_TR_901) == TRUE) {
        *skip = TRUE;
        return;
    }
    return;
}

void
bcma_testutil_ser_ih_print_help(int unit)
{
    cli_out("%s", SER_IH_PARAM_HELP);

    cli_out("%s", SER_IH_EXAMPLES);

    return;
}

int
bcma_testutil_ser_ih_init(int unit, bcma_testutil_ser_ih_param_t *ser_ih_param)
{
    size_t sid_num = 0, sid = 0, entry_size = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_num);
    SHR_IF_ERR_EXIT(rv);

    ser_ih_param->mask_array = (uint32_t **)sal_alloc
        (sid_num * sizeof(uint32_t *), "bcmaTestCaseSERIhMaskPoint");
    entry_size = BCMDRD_MAX_PT_WSIZE * sizeof(uint32_t);
    for (sid = 0; sid < sid_num; sid++) {
        ser_ih_param->mask_array[sid] = (uint32_t *)sal_alloc
            (entry_size, "bcmaTestCaseSERIhMask");
        sal_memset(ser_ih_param->mask_array[sid], 0, entry_size);
        rv = bcma_testutil_ser_ih_mem_datamask_get
            (unit, sid, ser_ih_param->mask_array[sid]);
        SHR_IF_ERR_EXIT(rv);
    }
    ser_ih_param->num_mem_written = 0;
    ser_ih_param->num_mem_read = 0;
    ser_ih_param->num_bit_tested = 0;
    ser_ih_param->num_bit_flip = 0;
    ser_ih_param->num_mem_check_fail = 0;
    ser_ih_param->num_mem_read_fail = 0;

    rv = bcmptm_ser_testutil_refresh_regs_disable(unit, 1);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmptm_ser_testutil_all_pts_enable(unit, 0);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_ih_cleanup(int unit, bcma_testutil_ser_ih_param_t *ser_ih_p)
{
    size_t sid_num = 0, sid = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    cli_out("\n-----------------");
    cli_out("\n   TEST RESULT   ");
    cli_out("\n-----------------");
    if (ser_ih_p->ser_rd_wr_mode & SER_WR_ONLY_MODE) {
        cli_out("\nTotal memories written = %0d", ser_ih_p->num_mem_written);
    }
    if (ser_ih_p->ser_rd_wr_mode & SER_RD_ONLY_MODE) {
        cli_out("\nTotal memories read = %0d", ser_ih_p->num_mem_read);
        cli_out("\nTotal Flips = %0d", ser_ih_p->num_bit_flip);
        cli_out("\nTotal bits tested = %0d kb", ser_ih_p->num_bit_tested / 1024);
        cli_out("\nTotal memories read unsuccessfully = %0d", ser_ih_p->num_mem_read_fail);
        cli_out("\nTotal memories check unsuccessfully = %0d", ser_ih_p->num_mem_check_fail);
    }
    cli_out("\n-----------------\n");

    rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_num);
    SHR_IF_ERR_EXIT(rv);

    for (sid = 0; sid < sid_num; sid++) {
        sal_free(ser_ih_p->mask_array[sid]);
        ser_ih_p->mask_array[sid] = NULL;
    }
    sal_free(ser_ih_p->mask_array);
    ser_ih_p->mask_array = NULL;

    rv = bcmptm_ser_testutil_refresh_regs_disable(unit, 0);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static void
bcma_testutil_mem_data_check(int unit, bcmdrd_sid_t sid, int row,
                             uint32_t *write_data, uint32_t *read_data, int entry_wsize,
                             bcma_testutil_ser_ih_param_t *ser_ih_p, int *fail_cnt)
{
    uint32_t *mask_array = NULL, mask = 0, comp_mask = 0;
    int k = 0, bit, bit_num = sizeof(uint32_t) * 8, col = 0;

    if (*fail_cnt > 100) {
        return;
    }
    mask_array = ser_ih_p->mask_array[sid];
    for (k = 0; k < entry_wsize; k++) {
        mask = mask_array[k];
        if (read_data[k] == ((write_data[k] & mask) | (read_data[k] & (~mask)))) {
            continue;
        }
        if (!ser_ih_p->ser_quiet_mode) {
            cli_out("\nwrite_data[%d] = 0x%08x, read_data[%d] = 0x%08x, mask = 0x%08x.",
                    k, write_data[k], k, read_data[k], mask);
        }
        for (bit = 0; bit < bit_num; bit++) {
            comp_mask = (1 << bit) & mask;
            if ((read_data[k] & comp_mask) == (write_data[k] & comp_mask)) {
                continue;
            }
            col = k * bit_num + bit;
            (*fail_cnt)++;
            if (!ser_ih_p->ser_quiet_mode) {
                cli_out("\nFlip: Row = %0d, Col = %0d", row, col);
            }
        }
        if (*fail_cnt > 100) {
            break;
        }
    }
    if (!ser_ih_p->ser_quiet_mode && *fail_cnt > 0) {
        ser_ih_p->num_bit_flip += *fail_cnt;
        cli_out("\nERROR: %s had %0d bit flipped!",
                bcmdrd_pt_sid_to_name(unit, sid), *fail_cnt);
    }
}

static int
bcma_testutil_mem_data_read(int unit, bcmdrd_sid_t sid,
                            bcma_testutil_ser_ih_param_t *ser_ih_p)
{
    uint32_t write_data[BCMDRD_MAX_PT_WSIZE], base_word;
    uint32_t read_data[BCMDRD_MAX_PT_WSIZE];
    int rv, i = 0, min_index, max_index, num_entries = 0;
    int inst_no = 0, inst_num, entry_wsize = 0, rv_read = SHR_E_NONE;
    int copy_no, copy_num, row, fail_cnt = 0;
    int pattern = ser_ih_p->ser_test_pattern;
    int check_type, recovery_type;
    bool valid = 0, port_base_reg = 0;
    const char *pt_name = NULL;
    int blktype;
    uint32_t pipe_map;

    SHR_FUNC_ENTER(unit);

    SER_TEST_DATA_GET(pattern, base_word);
    for (i = 0; i < BCMDRD_MAX_PT_WSIZE; i++) {
        write_data[i] = base_word;
    }
    if (bcmdrd_pt_is_port_reg(unit, sid) ||
        bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        port_base_reg = 1;
    }
    min_index = 0;
    pt_name = bcmdrd_pt_sid_to_name(unit, sid);
    rv = bcma_testutil_ser_pt_status_get(unit, pt_name, 0, &max_index,
                                         &inst_num, &copy_num, &check_type,
                                         &recovery_type);
    SHR_IF_ERR_EXIT(rv);
    max_index -= 1;

    if (!ser_ih_p->ser_quiet_mode) {
        cli_out("\nReading and Checking %s.", bcmdrd_pt_sid_to_name(unit, sid));
    }
    copy_no = (copy_num > 0) ? 0 : -1;
    entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
    for (inst_no = 0; inst_no < inst_num; inst_no++) {
        blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
        (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &pipe_map);
        if (!(pipe_map & (1 << inst_no))) {
            continue;
        }
        valid = bcmptm_ser_testutil_pt_copy_no_valid(unit, sid, inst_no, copy_no);
        if (valid == 0) {
            continue;
        }
        for (i = min_index; i <= max_index; i++) {
            valid = bcmdrd_pt_index_valid(unit, sid, inst_no, i);
            if (valid == 0) {
                continue;
            }
            if (bcmptm_ser_testutil_pt_acctype_is_addr_split(unit, sid) ||
                bcmptm_ser_testutil_pt_acctype_is_data_split(unit, sid)) {
                inst_no = -1;
            }
            rv_read = bcmptm_ser_testutil_pt_read(unit, sid, i, inst_no,
                                                  copy_no, read_data,
                                                  BCMDRD_MAX_PT_WSIZE, 1);
            SHR_IF_ERR_EXIT(rv_read);
            row = port_base_reg ? inst_no : i;
            bcma_testutil_mem_data_check(unit, sid, row, write_data,
                                         read_data, entry_wsize,
                                         ser_ih_p, &fail_cnt);
        }
        /* Access type is DATA_SPLIT or ADDR_SPLIT */
        if (inst_no < 0) {
            break;
        }
    }
    num_entries = port_base_reg ? inst_num : (max_index - min_index + 1);
    copy_num = copy_num ? copy_num : 1;
    ser_ih_p->num_bit_tested += num_entries * entry_wsize * sizeof(uint32_t) * copy_num;

exit:
    if (SHR_FAILURE(rv_read)) {
        ser_ih_p->num_mem_read_fail++;
    } else if (fail_cnt > 0) {
        ser_ih_p->num_mem_check_fail++;
    }
    ser_ih_p->num_mem_read++;
    if (SHR_FAILURE(rv)) {
        cli_out("\nFail to read and check memory %s, inst[%d] index[%d].",
                bcmdrd_pt_sid_to_name(unit, sid), inst_no, i);
    }
    SHR_FUNC_EXIT();
}

static int
bcma_testutil_mem_data_write(int unit, bcmdrd_sid_t sid,
                             bcma_testutil_ser_ih_param_t *ser_ih_p)
{
    int rv, i, min_index, max_index, recovery_type;
    int inst_no, inst_num, copy_num, copy_no, check_type;
    uint32_t wr_data[BCMDRD_MAX_PT_WSIZE], base_word;
    bool unique_mode, valid;
    const char *pt_name = NULL;
    bool port_base_reg = 0;

    SHR_FUNC_ENTER(unit);

    SER_TEST_DATA_GET(ser_ih_p->ser_test_pattern, base_word);
    for (i = 0; i < BCMDRD_MAX_PT_WSIZE; i++) {
        wr_data[i] = base_word & ser_ih_p->mask_array[sid][i];
    }

    if (bcmdrd_pt_is_port_reg(unit, sid) ||
        bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        port_base_reg = 1;
    }
    min_index = 0;
    pt_name = bcmdrd_pt_sid_to_name(unit, sid);
    rv = bcma_testutil_ser_pt_status_get(unit, pt_name, 0, &max_index,
                                         &inst_num, &copy_num, &check_type,
                                         &recovery_type);
    SHR_IF_ERR_EXIT(rv);
    max_index -= 1;

    bcmptm_ser_testutil_is_unique_mode(unit, sid, &unique_mode);

    for (copy_no = -1; copy_no < copy_num; copy_no++) {
        for (inst_no = 0; inst_no < inst_num; inst_no++) {
            valid = bcmptm_ser_testutil_pt_copy_no_valid(unit, sid, inst_no, copy_no);
            if (valid == 0) {
                continue;
            }
            for (i = min_index; i <= max_index; i++) {
                if (bcmdrd_pt_index_valid(unit, sid, inst_no, i) == 0) {
                    continue;
                }
                if ((unique_mode == 0) && (!port_base_reg)) {
                    /* Write data to all instances */
                    inst_no = -1;
                }
                rv = bcmptm_ser_testutil_pt_hw_write(unit, sid, i, inst_no,
                                                     -1, wr_data, 1);
                SHR_IF_ERR_EXIT(rv);
            }
            if ((unique_mode == 0) && (!port_base_reg)) {
                break;
            }
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        cli_out("\nFail to write memory %s.", bcmdrd_pt_sid_to_name(unit, sid));
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_ih_write(int unit, bcma_testutil_ser_ih_param_t *ser_ih_p)
{
    size_t sid_num = 0, sid = 0;
    int acc_type = 0, rv;
    bool skip = FALSE;

    SHR_FUNC_ENTER(unit);

    if (!(ser_ih_p->ser_rd_wr_mode & SER_WR_ONLY_MODE)) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }
    rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_num);
    SHR_IF_ERR_EXIT(rv);

    if (!ser_ih_p->ser_quiet_mode) {
        cli_out("\nWriting all memories...");
    }
    for (sid = 0; sid < sid_num; sid++) {
        bcma_testutil_ser_mem_skip(unit, sid, &skip);
        if (skip) {
            continue;
        }
        if (!ser_ih_p->ser_quiet_mode) {
            acc_type = bcmdrd_pt_acctype_get(unit, sid);
            cli_out("\nWriting %s, acc_type = %0d.",
                    bcmdrd_pt_sid_to_name(unit, sid), acc_type);
        }
        (void)bcma_testutil_mem_data_write(unit, sid, ser_ih_p);
        ser_ih_p->num_mem_written++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_ih_read_check(int unit, bcma_testutil_ser_ih_param_t *ser_ih_p)
{
    size_t sid_num = 0, sid = 0;
    int rv = SHR_E_NONE;
    bool skip = FALSE;

    SHR_FUNC_ENTER(unit);

    if (!(ser_ih_p->ser_rd_wr_mode & SER_RD_ONLY_MODE)) {
        SHR_EXIT();
    }
    rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_num);
    SHR_IF_ERR_EXIT(rv);

    if (!ser_ih_p->ser_quiet_mode) {
        cli_out("\nReading and Checking all memories...");
    }
    for (sid = 0; sid < sid_num; sid++) {
        bcma_testutil_ser_mem_skip(unit, sid, &skip);
        if (skip) {
            continue;
        }
        rv = bcma_testutil_mem_data_read(unit, sid, ser_ih_p);
        if (SHR_FAILURE(rv)) {
            cli_out("\n Fail to read or check %s...", bcmdrd_pt_sid_to_name(unit, sid));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_ih_param_parse(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                                 bcma_testutil_ser_ih_param_t *ser_ih_param, bool show)
{
    bcma_cli_parse_table_t pt;

    SHR_FUNC_ENTER(unit);

    bcma_cli_parse_table_init(cli, &pt);

    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(ser_ih_param, SHR_E_PARAM);

    ser_ih_param->ser_test_pattern = SER_TEST_PATTERN_DEFAULT;
    ser_ih_param->ser_rd_wr_mode = SER_RD_WR_MODE_DEFAULT;
    ser_ih_param->ser_quiet_mode = SER_NON_QUIET_MODE_DEFAULT;

    bcma_cli_parse_table_add(&pt, "TestPat", "int",
                             &(ser_ih_param->ser_test_pattern), NULL);
    bcma_cli_parse_table_add(&pt, "RdWrMode", "int",
                             &(ser_ih_param->ser_rd_wr_mode), NULL);
    bcma_cli_parse_table_add(&pt, "QuietMode", "int",
                             &(ser_ih_param->ser_quiet_mode), NULL);

    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    } else {
        cli_out("\n-----------------");
        cli_out("\n TEST PARAMETERS ");
        cli_out("\n-----------------");
        cli_out("\nTestPat    = %0d", ser_ih_param->ser_test_pattern);
        cli_out("\nRdWrMode   = %0d", ser_ih_param->ser_rd_wr_mode);
        cli_out("\nQuietMode  = %0d", ser_ih_param->ser_quiet_mode);
        cli_out("\n-----------------\n");
    }
    if (show) {
        bcma_cli_parse_table_show(&pt, NULL);
        SHR_EXIT();
    }

exit:
    bcma_cli_parse_table_done(&pt);

    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_ih_mem_datamask_get(int unit, bcmdrd_sid_t sid, uint32_t *entry_data)
{
    size_t fields_num = 0, i = 0;
    int rv = SHR_E_NONE;
    bcmdrd_fid_t *fields_list = NULL;
    bcmdrd_fid_t fid = INVALIDf;
    uint32_t fval[BCMDRD_MAX_PT_WSIZE];

    SHR_FUNC_ENTER(unit);

    rv = bcmdrd_pt_fid_list_get(unit, sid, 0, NULL, &fields_num);
    SHR_IF_ERR_EXIT(rv);

    fields_list = (bcmdrd_fid_t *)sal_alloc
        (fields_num * sizeof(bcmdrd_fid_t), "bcmaTestCaseSERIhFieldList");
    SHR_NULL_CHECK(fields_list, SHR_E_MEMORY);

    rv = bcmdrd_pt_fid_list_get(unit, sid, fields_num, fields_list, &fields_num);
    SHR_IF_ERR_EXIT(rv);

    sal_memset(fval, 0xFF, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);

    for (i = 0; i < fields_num; i++) {
        fid = fields_list[i];
        rv = bcmdrd_pt_field_set(unit, sid, entry_data, fid, fval);
        SHR_IF_ERR_EXIT(rv);
    }
    bcmptm_ser_testutil_parity_field_clear(unit, sid, entry_data);

exit:
    if (fields_list != NULL) {
        sal_free(fields_list);
        fields_list = NULL;
    }
    SHR_FUNC_EXIT();
}

