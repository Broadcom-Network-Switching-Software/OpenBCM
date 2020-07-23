/*! \file bcma_testutil_ser.c
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
#include <bcmlt/bcmlt.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <sal/sal_sleep.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_ser.h>
#include <bcma/test/util/bcma_testutil_ser_flip_check.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_config.h>

#include <bcmptm/bcmptm_ser_testutil.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define BCMA_PT_ECC_CHECK_TYPE       2

#define SER_PARAM_HELP \
    "  Must run 'Test mode noreinit=true' before Tr 144\n"\
    "  SID          - Physical table name.\n"\
    "  INstance     - Physical table instance.\n"\
    "  Copy         - Physical table copy number. Only few PTs support this.\n"\
    "  IndexType    - Single: SER error injected to appointed index.\n"\
    "                 Spread: SER error injected to first/middle/last index of PT.\n"\
    "  Index        - Physical table index.\n"\
    "  InjectOnly   - 1: Inject SER error, and then dectect the injected SER error\n"\
    "                    by SW threads scan.\n"\
    "               - 0: Not only inject SER error,\n"\
    "                    but also make SER correction and validation.\n"\
    "  ErrBitnum    - 1: Inject 1bit SER error to PT.\n"\
    "               - 2: Inject 2bit SER error to PT.\n"\
    "  XorBank      - 0: Inject SER error to non-XOR bank of PT.\n"\
    "               - 1: Inject SER error to XOR bank of PT owning XOR bank.\n"\
    "  WaitTime     - Give a little time(milliseconds) to SERC logic to correct SER error.\n"\
    "                 Only used when parameter 'InjectOnly' is '0'.\n"\
    "\n" \
    "  TestCtrlReg  - 1: test value of SER control registers.\n"\
    "                 Is not used along with other parameters\n"\
    "\n" \
    "  SSID         - Physical table name. Run TR 144 test from this PT.\n" \
    "  PtNum        - Number of physical table to be tested.\n"\
    "  OneInst      - 1: Only test one instance for a PT.\n"\
    "  AllRespType  - 0: Don't test PT which recovery type is NONE.\n" \
    "  SingleBitErr - 1: Only test single bit error of ECC PTs. \n" \
    "  CheckData    - 1: Write non-zero data to a PT, \n" \
    "                    and check whether the non-zero data can be recovered.\n"

#define SER_EXAMPLES \
        "\nExamples:\n"\
        "  tr 144 SID=L2Xm\n"\
        "  tr 144 SID=L2Xm ErrBitnum=1 Checkdata=1\n"\
        "  tr 144 SID=L2Xm instance=1 IndexType=Spread InjectOnly=1 ErrBitnum=2 XorBank=0\n"\
        "  tr 144 SID=L2Xm instance=1 IndexType=Single index=8192 InjectOnly=1 ErrBitnum=2 XorBank=1\n"\
        "  tr 144 SID=L2Xm instance=0 IndexType=Single index=2 InjectOnly=0 ErrBitnum=2 WaitTime=500\n"\
        "  tr 144 TestCtrlReg=1\n" \
        "  tr 144 SSID=L2Xm PtNum=N\n"\
        "  tr 144 OneInst=1\n"\
        "  tr 144 OneInst=1 SingleBitErr=1\n"\
        "  tr 144 SingleBitErr=1\n"\
        "  tr 144 OneInst=1 AllRespType=1\n" \
        "  tr 144 OneInst=1 AllRespType=1 Checkdata=1\n" \
        "  tr 144 OneInst=1 AllRespType=1 Checkdata=1 SingleBitErr=1\n"

#define NAME_SPACE_SIZE       108
#define INDEX_TYPE_SIZE       8
#define TIME_TO_WAIT          120
#define RETRY_TIME            20

/*!\brief SER recovery type */
typedef enum ser_recovery_type_s {
    /*! All recovery type. */
    SER_RECOVERY_ALL = 0,
    /*! Correct SER error entry with cache data */
    SER_RECOVERY_CACHE_RESTORE = 1,
    /*! Clear SER error entry. */
    SER_RECOVERY_ENTRY_CLEAR = 2,
    /*! Not handle SER error. */
    SER_RECOVERY_NO_OPERATION = 3,
} bcmptm_ser_recovery_type_t;

typedef enum {
    /* Fail to pass tr 144 */
    SER_TEST_FAIL = 0,
    /* Do not test PT which recovery type is none */
    SER_SKIP_RESP_NONE,
    /* Only test 1bit error of PT which has ECC check */
    SER_SKIP_PARITY_CHECK,
    /* Only test PTs which is scanned by SW thread */
    SER_SKIP_NO_SW_SCAN,
} bcma_testutil_fail_skip_t;

typedef struct bcma_testutil_ser_tr_log_s {
    /* SID is not tested by TR 144 or does not pass TR 144 */
    bcmdrd_sid_t sid;
    /* 1: not tested, 0: does not pass */
    bcma_testutil_fail_skip_t skip_or_fail;
} bcma_testutil_ser_tr_log_t;

typedef struct bcma_testutil_ser_tr_stats_s {
    /* Number of SRAM PTs tested by TR 144 */
    uint32_t pt_num_test;
    /* Number of TCAM PTs tested by TR 144 */
    uint32_t tcam_pt_num_test;
    /* Number of PTs which do not pass TR 144 */
    uint32_t pt_num_fail;
    /* Number of PTs which need to be skipped by TR 144 */
    uint32_t pt_num_skip;
    /* Test start time stamp */
    sal_usecs_t start_time_stamp;
    /* Test end time stamp */
    sal_usecs_t end_time_stamp;
} bcma_testutil_ser_tr_stats_t;

/* Statistics for TR 144 TestCtrlReg=1 */
typedef struct bcma_testutil_ser_tr_ctrl_reg_stats_s {
    /* Number of tested internal buffer or bus. */
    uint32_t int_bb_en_num;
    /* Number of internal buffer or bus which SER protection is not enabled. */
    uint32_t int_bb_en_num_fail;
    /* Number of tested internal buffer or bus for 1 bit. */
    uint32_t int_bb_1bit_num;
    /* Number of tested internal buffer or bus which is not protected by ECC. */
    uint32_t int_bb_no_1bit_num;
    /* Number of internal buffer or bus  which 1 bit error reporting is not enabled. */
    uint32_t int_bb_1bit_num_fail;
    /* Number of tested PT */
    uint32_t pt_en_num;
    /* Number of PT which SER protection is not enabled */
    uint32_t pt_en_num_fail;
    /* Number of tested PT for the 1bit test */
    uint32_t pt_1bit_num;
    /* Number of tested PT which 1 it error reporting is not enabled */
    uint32_t pt_1bit_num_fail;
} bcma_testutil_ser_tr_ctrl_reg_stats_t;

static char pt_name_space[BCMDRD_CONFIG_MAX_UNITS][NAME_SPACE_SIZE];
static char index_type_space[BCMDRD_CONFIG_MAX_UNITS][INDEX_TYPE_SIZE];
static bcma_testutil_ser_tr_stats_t tr_stats[BCMDRD_CONFIG_MAX_UNITS];
static bcma_testutil_ser_tr_log_t *tr_log_ptr[BCMDRD_CONFIG_MAX_UNITS];
static bcma_testutil_ser_tr_ctrl_reg_stats_t tr_ctrl_stats[BCMDRD_CONFIG_MAX_UNITS];
static uint64_t ser_stats_total_cnt = 0;
static uint64_t ser_stats_detailed_cnt[7][4];

static void
bcma_testutil_ser_tr_log_init(int unit, uint32_t pt_num, uint32_t pt_tcam_num)
{
    size_t alloc_size = sizeof(bcma_testutil_ser_tr_log_t) * pt_num;

    tr_stats[unit].pt_num_test = pt_num;
    tr_stats[unit].tcam_pt_num_test = pt_tcam_num;
    if (tr_log_ptr[unit] == NULL) {
        tr_log_ptr[unit] = sal_alloc(alloc_size, "bcmaTr144Log");
        if (tr_log_ptr[unit] == NULL) {
            return;
        }
        sal_memset(tr_log_ptr[unit], 0, alloc_size);
    }
    tr_stats[unit].start_time_stamp = sal_time_usecs();
}

static void
bcma_testutil_ser_fail_pt_record(int unit, bcmdrd_sid_t sid,
                                 bcma_testutil_fail_skip_t skip_or_fail)
{
    uint32_t offset = 0;
    bcma_testutil_ser_tr_log_t *log_ptr;

    if (tr_log_ptr[unit] == NULL) {
        return;
    }
    offset = tr_stats[unit].pt_num_fail + tr_stats[unit].pt_num_skip;
    if (offset >= tr_stats[unit].pt_num_test) {
        return;
    }
    log_ptr = tr_log_ptr[unit] + offset;
    log_ptr->sid = sid;
    log_ptr->skip_or_fail = skip_or_fail;
    if (skip_or_fail == SER_TEST_FAIL) {
        tr_stats[unit].pt_num_fail++;
    } else {
        tr_stats[unit].pt_num_skip++;
    }
}

static void
bcma_testutil_ser_sid_print(int unit, bcma_testutil_fail_skip_t type)
{
    bcma_testutil_ser_tr_log_t *log_ptr;
    uint32_t i = 0, skip_fail_pt_num = 0, new_line = 0;

    skip_fail_pt_num = tr_stats[unit].pt_num_skip + tr_stats[unit].pt_num_fail;
    new_line = 0;
    for (i = 0; i < skip_fail_pt_num; i++) {
        log_ptr = tr_log_ptr[unit] + i;
        if (log_ptr->skip_or_fail == type) {
            cli_out("[%s][%d]  ",
                bcmdrd_pt_sid_to_name(unit, log_ptr->sid), log_ptr->sid);
            new_line++;
            if (new_line % 2 == 0) {
                cli_out("\n");
            }
        }
    }
    cli_out("\n");
}

static void
bcma_testutil_ser_test_print(int unit, int num_entry_tested)
{
    sal_usecs_t time_spent_per_pt = 0, time_spent = 0;

    if (tr_log_ptr[unit] == NULL) {
        return;
    }
    tr_stats[unit].end_time_stamp = sal_time_usecs();
    time_spent = (tr_stats[unit].end_time_stamp > tr_stats[unit].start_time_stamp) ?
        (tr_stats[unit].end_time_stamp - tr_stats[unit].start_time_stamp) :
        ((sal_usecs_t)(-1) + tr_stats[unit].end_time_stamp - tr_stats[unit].start_time_stamp);

    time_spent_per_pt = time_spent / (num_entry_tested + 1);

    cli_out("Time spent on test: [%d]us, number of tested entry: [%d], Time spent per entry: [%d]us\n",
            time_spent, num_entry_tested, time_spent_per_pt);

    cli_out("Tested PT num: [%d], TCAM PT num: [%d], Skipped PT num: [%d], Failed PT num: [%d]\n",
            tr_stats[unit].pt_num_test, tr_stats[unit].tcam_pt_num_test,
            tr_stats[unit].pt_num_skip, tr_stats[unit].pt_num_fail);

    cli_out("Skipped PTs which recovery type is None: \n");
    bcma_testutil_ser_sid_print(unit, SER_SKIP_RESP_NONE);

    cli_out("Skipped PTs which have parity check for 1bit error reporting test: \n");
    bcma_testutil_ser_sid_print(unit, SER_SKIP_PARITY_CHECK);

    cli_out("Skipped PTs which have not been scanned by SW threads: \n");
    bcma_testutil_ser_sid_print(unit, SER_SKIP_NO_SW_SCAN);

    cli_out("Failed PTs list:\n");
    bcma_testutil_ser_sid_print(unit, SER_TEST_FAIL);

    sal_free(tr_log_ptr[unit]);
    tr_log_ptr[unit] = NULL;
    tr_stats[unit].pt_num_test = 0;
    tr_stats[unit].tcam_pt_num_test = 0;
    tr_stats[unit].pt_num_skip = 0;
    tr_stats[unit].pt_num_fail = 0;
}

static void
bcma_testutil_ser_ctrl_reg_test_print(int unit)
{
    uint32_t success = 0;
    bcma_testutil_ser_tr_ctrl_reg_stats_t *ctrl_stats = &tr_ctrl_stats[unit];

    success = ctrl_stats->int_bb_en_num - ctrl_stats->int_bb_en_num_fail;
    cli_out("SER check for Internel buffers or buses: \n");
    cli_out("    total [%d], fail [%d], success [%d].\n",
        ctrl_stats->int_bb_en_num, ctrl_stats->int_bb_en_num_fail, success);

    success = ctrl_stats->int_bb_1bit_num -
        ctrl_stats->int_bb_no_1bit_num - ctrl_stats->int_bb_1bit_num_fail;
    cli_out("1bit error reporting for Internel buffers or buses: \n");
    cli_out("    total [%d], none [%d], fail [%d] success [%d].\n\n",
        ctrl_stats->int_bb_1bit_num, ctrl_stats->int_bb_no_1bit_num,
        ctrl_stats->int_bb_1bit_num_fail, success);

    success = ctrl_stats->pt_en_num - ctrl_stats->pt_en_num_fail;
    cli_out("SER check for physical tables: \n");
    cli_out("    total [%d], fail [%d], success [%d].\n",
        ctrl_stats->pt_en_num, ctrl_stats->pt_en_num_fail, success);

    success = ctrl_stats->pt_1bit_num - ctrl_stats->pt_1bit_num_fail;
    cli_out("1bit error reporting for physical tables: \n");
    cli_out("    total [%d], fail [%d] success [%d].\n",
        ctrl_stats->pt_1bit_num, ctrl_stats->pt_1bit_num_fail, success);
}

static int
bcma_testutil_ser_rand(void)
{
    sal_usecs_t usecs_time = sal_time_usecs();

    if (usecs_time != 0) {
        sal_srand(usecs_time);
    }
    return sal_rand();
}

static int
bcma_testutil_ser_pt_info_gen(int unit, int index_num,
                              int inst_num, int copy_num, int check_type,
                              bcma_testutil_ser_param_t *ser_param)
{
    int copy_no, map_inst, rv;
    bool index_valid, inst_valid;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    if (ser_param->single_bit_test) {
        ser_param->err_bit_num = 1;
    } else {
        /* 1: parity check, 2: ecc check */
        ser_param->err_bit_num = (check_type > 1) ? 2 : 1;
    }
    ser_param->index = bcma_testutil_ser_rand() % index_num;
    ser_param->inst = bcma_testutil_ser_rand() % inst_num;
    ser_param->copy = -1;
    if (copy_num > 0) {
        ser_param->copy = bcma_testutil_ser_rand() % copy_num;
    }
    rv = bcmdrd_pt_name_to_sid(unit, ser_param->pt_id_name, &sid);
    SHR_IF_ERR_EXIT(rv);

    map_inst = ser_param->inst;
    copy_no = -1;
    inst_valid = bcmptm_ser_testutil_pt_inst_remap(unit, sid, &map_inst, &copy_no);
    if (inst_valid == FALSE) {
        cli_out("Invalid inst[%d]->[%d] of PT[%s]\n",
                ser_param->inst, map_inst, ser_param->pt_id_name);
        rv = SHR_E_FAIL;
        SHR_IF_ERR_EXIT(rv);
    }
    ser_param->inst = map_inst;
    index_valid = bcmdrd_pt_index_valid(unit, sid, ser_param->inst,
                                        ser_param->index);
    if (index_valid == 0) {
        ser_param->inst = 0;
        ser_param->index = index_num - 1;
    }
    while (index_valid == 0) {
        index_valid = bcmdrd_pt_index_valid(unit, sid, ser_param->inst,
                                            ser_param->index);
        if (ser_param->index > 0) {
            ser_param->index--;
        } else {
            break;
        }
    }
    if (index_valid == 0) {
        cli_out("Fail to find valid inst of PT[%s]\n", ser_param->pt_id_name);
        rv = SHR_E_FAIL;
        SHR_IF_ERR_EXIT(rv);
    }
    for (copy_no = -1; copy_no <= copy_num; copy_no++) {
        if (bcmptm_ser_testutil_pt_copy_no_valid(unit, sid, ser_param->inst, copy_no)) {
            ser_param->copy = copy_no < 0 ? 0 : copy_no;
            break;
        }
    }
    if (ser_param->copy == -1) {
        cli_out("Fail to find valid copy id of PT[%s]\n", ser_param->pt_id_name);
        rv = SHR_E_FAIL;
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

static bool
bcma_testutil_ser_check_is_enabled(int unit, bcmdrd_sid_t sid)
{
    bcmlt_entry_handle_t ser_pt_control = 0;
    uint64_t ecc_parity_check = 0;
    int rv = SHR_E_NONE;
    const char *pt_name = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "SER_PT_CONTROL", &ser_pt_control);
    SHR_IF_ERR_EXIT(rv);

    pt_name = bcmdrd_pt_sid_to_name(unit, sid);

    rv = bcmlt_entry_field_symbol_add(ser_pt_control, "PT_ID", pt_name);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(ser_pt_control, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    /* No data in SER_PT_CONTROL for PTs which do not support SER protection */
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmlt_entry_field_get(ser_pt_control, "ECC_PARITY_CHECK",
                               &ecc_parity_check);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (ser_pt_control) {
        bcmlt_entry_free(ser_pt_control);
    }
    if (ecc_parity_check == 0 || SHR_FUNC_ERR()) {
        return 0;
    } else {
        return 1;
    }
}

static bool
bcma_testutil_ser_scan_mode_oper_get(int unit, bcmdrd_sid_t sid)
{
    bcmlt_entry_handle_t ser_pt_control = 0;
    uint64_t scan_mode = 0;
    int rv = SHR_E_NONE;
    const char *pt_name = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "SER_PT_CONTROL", &ser_pt_control);
    SHR_IF_ERR_EXIT(rv);

    pt_name = bcmdrd_pt_sid_to_name(unit, sid);

    rv = bcmlt_entry_field_symbol_add(ser_pt_control, "PT_ID", pt_name);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(ser_pt_control, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    /* No data in SER_PT_CONTROL for PTs which do not support SER protection */
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmlt_entry_field_get(ser_pt_control, "SCAN_MODE_OPER",
                               &scan_mode);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (ser_pt_control) {
        bcmlt_entry_free(ser_pt_control);
    }
    if (scan_mode == 0 || SHR_FUNC_ERR()) {
        return 0;
    } else {
        return 1;
    }
}

static int
bcma_testutil_ser_check_enable(int unit, bcmdrd_sid_t sid, bool enable)
{
    bcmlt_entry_handle_t ser_pt_control = 0;
    uint64_t check = enable ? 1 : 0;
    int rv = SHR_E_NONE;
    const char *pt_name = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "SER_PT_CONTROL", &ser_pt_control);
    SHR_IF_ERR_EXIT(rv);

    pt_name = bcmdrd_pt_sid_to_name(unit, sid);

    rv = bcmlt_entry_field_symbol_add(ser_pt_control, "PT_ID", pt_name);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_add(ser_pt_control, "ECC_PARITY_CHECK", check);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(ser_pt_control, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (ser_pt_control) {
        bcmlt_entry_free(ser_pt_control);
    }
    SHR_FUNC_EXIT();
}

static bool
bcma_testutil_ser_pt_skip(int unit, bcmdrd_sid_t sid, int inject_only)
{
    int index_count = 0, ecc_checking = 0;
    int skip = 0, scan_mode_oper = 0;

    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return TRUE;
    }
    ecc_checking = bcma_testutil_ser_check_is_enabled(unit, sid);
    if (ecc_checking == 0) {
        return TRUE;
    }
    if (inject_only) {
        scan_mode_oper = bcma_testutil_ser_scan_mode_oper_get(unit, sid);
        /* DISABLE = 0 , USING_HW_SCAN = 2*/
        if (scan_mode_oper == 0 || scan_mode_oper == 2) {
            return TRUE;
        }
    }
    (void)bcmptm_ser_testutil_index_valid(unit, sid, 0, &index_count);
    if (index_count == 0) {
        return TRUE;
    }
    if (bcmdrd_pt_attr_is_fifo(unit, sid)) {
        return TRUE;
    }
    skip = bcma_testutil_drv_pt_skip(unit, sid, BCMA_TESTUTIL_PT_MEM_TR_144);
    if (skip == TRUE) {
        return TRUE;
    }
    return FALSE;
}

static int
bcma_testutil_ser_stats_cnt_validate(int unit, int event_num_expected,
                                     int blk_type, int recovery_type,
                                     int *event_num_missed)
{
    bcmlt_entry_handle_t ser_stats = 0;
    uint64_t total_cnt_cur = 0, detailed_cnt_cur = 0;
    int incr_total = 0, incr_detailed = 0;
    int rv = SHR_E_NONE, i, retry_time = RETRY_TIME;
    char *stats_blk_keys[] = {
        "SER_BLK_ALL", "SER_BLK_MMU", "SER_BLK_IPIPE",
        "SER_BLK_EPIPE", "SER_BLK_PGW", "SER_BLK_PORT",
        "SER_BLK_SEC"
    };
    char *stats_recovery_keys[] = {
        "SER_RECOVERY_ALL", "SER_RECOVERY_CACHE_RESTORE",
        "SER_RECOVERY_ENTRY_CLEAR", "SER_RECOVERY_NO_OPERATION"
    };
    const char *blk_key = NULL, *recovery_key = NULL;

    SHR_FUNC_ENTER(unit);

    if ((COUNTOF(stats_blk_keys) <= blk_type) ||
        (COUNTOF(stats_recovery_keys) <= recovery_type)) {
        rv = SHR_E_PARAM;
        SHR_IF_ERR_EXIT(rv);
    }
    rv = bcmlt_entry_allocate(unit, "SER_STATS", &ser_stats);
    SHR_IF_ERR_EXIT(rv);

retry:
    /* Loop start */
    for (i = 0; i < 2; i++) {
        if (i == 0) {
            blk_key = stats_blk_keys[0];
            recovery_key = stats_recovery_keys[0];
        } else {
            blk_key = stats_blk_keys[blk_type];
            recovery_key = stats_recovery_keys[recovery_type];
        }
        rv = bcmlt_entry_field_symbol_add(ser_stats, "BLK_TYPE", blk_key);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmlt_entry_field_symbol_add(ser_stats, "RECOVERY_TYPE", recovery_key);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmlt_entry_commit(ser_stats, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
        SHR_IF_ERR_EXIT(rv);

        if (i == 0) {
            rv = bcmlt_entry_field_get(ser_stats, "TOTAL_ERR_CNT", &total_cnt_cur);
            SHR_IF_ERR_EXIT(rv);

            incr_total = total_cnt_cur - ser_stats_total_cnt;
            /* Re-init */
            if (incr_total < 0) {
                ser_stats_total_cnt = 0;
                incr_total = total_cnt_cur - ser_stats_total_cnt;
            }
        } else {
            rv = bcmlt_entry_field_get(ser_stats, "TOTAL_ERR_CNT", &detailed_cnt_cur);
            SHR_IF_ERR_EXIT(rv);

            incr_detailed = detailed_cnt_cur - ser_stats_detailed_cnt[blk_type][recovery_type];
            /* Re-init */
            if (incr_detailed < 0) {
                ser_stats_detailed_cnt[blk_type][recovery_type] = 0;
                incr_detailed = detailed_cnt_cur - ser_stats_detailed_cnt[blk_type][recovery_type];
            }
        }
    }
    if (incr_detailed != incr_total) {
        if (retry_time-- > 0) {
            /* Two entries of LT SER_STATS are not updated meanwhile */
            sal_usleep(TIME_TO_WAIT * 4);
            goto retry;
        }
    }
    /* Loop end */
    ser_stats_total_cnt = total_cnt_cur;
    ser_stats_detailed_cnt[blk_type][recovery_type] = detailed_cnt_cur;
    if (incr_detailed != incr_total) {
        cli_out("Error in SER_STATS [%d] <> [%d].\n", incr_detailed, incr_total);
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }
    if (incr_total > event_num_expected) {
        cli_out("Num[%d] of SER events is triggered by others.\n",
            (incr_total - event_num_expected));
        *event_num_missed = 0;
    } else if (incr_total < event_num_expected) {
        *event_num_missed = event_num_expected - incr_total;
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        *event_num_missed = 0;
    }

exit:
    if (ser_stats) {
        bcmlt_entry_free(ser_stats);
    }
    SHR_FUNC_EXIT();
}

static int
bcma_testutil_ser_ser_stats_validate(int unit, bcmdrd_sid_t sid,
                                     int recovery_type, int ser_event_tt_num,
                                     int *ser_event_missed)
{
    int rv = SHR_E_NONE, blk_type, ser_blk_type;

    SHR_FUNC_ENTER(unit);

    blk_type = bcmdrd_pt_blktype_get(unit, sid, 0);
    rv = bcmptm_ser_testutil_blk_type_map(unit, blk_type, NULL, &ser_blk_type);
    SHR_IF_ERR_EXIT(rv);

    rv = bcma_testutil_ser_stats_cnt_validate(unit, ser_event_tt_num,
                                              ser_blk_type, recovery_type,
                                              ser_event_missed);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static bool
bcma_testutil_same_pt_check(int unit, bcmdrd_sid_t sid0, bcmdrd_sid_t sid1)
{
    /* static */
    bcmdrd_pt_ser_en_ctrl_t en_ctrl_0, en_ctrl_1;
    int rv = SHR_E_NONE;

    rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC, sid0, &en_ctrl_0);
    if (SHR_FAILURE(rv)) {
        /* TCAM */
        return TRUE;
    }
    rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC, sid1, &en_ctrl_1);
    if (SHR_FAILURE(rv)) {
        /* TCAM */
        return TRUE;
    }
    if (en_ctrl_0.sid == en_ctrl_1.sid &&
        en_ctrl_0.fid == en_ctrl_1.fid &&
        en_ctrl_0.fmask == en_ctrl_1.fmask) {
        return TRUE;
    }
    return FALSE;
}

static int
bcma_testutil_ser_log_validate(int unit, bcmdrd_sid_t sid,
                               int index, int instance)
{
    bcmlt_entry_handle_t log_entry = 0, log_status_entry = 0;
    int rv = SHR_E_NONE;
    int index_remap;
    bcmdrd_sid_t sid_remap, sid_from_log;
    const char *pt_name_reported;
    uint64_t index_from_log, inst_from_log, log_index;
    bool shadow_table;

    SHR_FUNC_ENTER(unit);

    index_remap = index;
    bcmptm_ser_testutil_mem_remap(unit, sid, index, 0,
                                  &sid_remap, &index_remap, &shadow_table);
    if (shadow_table) {
        /*
         * For shadow table, SW corrects its SER error by *_A,
         * however, records HW reported SID to SER_LOG.
         */
        sid_remap = sid;
    }

    /* Get index of the latest log from SER_LOG_STATUS */
    rv = bcmlt_entry_allocate(unit, "SER_LOG_STATUS", &log_status_entry);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(log_status_entry,
                            BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(log_status_entry, "SER_LOG_ID", &log_index);
    SHR_IF_ERR_EXIT(rv);

    /* Add index of the latest log to SER_LOG as key */
    rv = bcmlt_entry_allocate(unit, "SER_LOG", &log_entry);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_add(log_entry, "SER_LOG_ID", log_index);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(log_entry,
                            BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_symbol_get(log_entry, "PT_ID",
                                      &pt_name_reported);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmdrd_pt_name_to_sid(unit, pt_name_reported, &sid_from_log);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(log_entry, "PT_INDEX",
                               &index_from_log);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(log_entry, "PT_INSTANCE",
                               &inst_from_log);
    SHR_IF_ERR_EXIT(rv);

    if (sid_remap != sid_from_log ||
        index_remap != (int)index_from_log ||
        instance != (int)inst_from_log) {
        cli_out("Warning: Inject error into [%s][%d][%d], but get [%s][%d][%d] from log\n",
                bcmdrd_pt_sid_to_name(unit, sid), instance, index_remap,
                pt_name_reported, (int)inst_from_log, (int)index_from_log);
        if (bcma_testutil_same_pt_check(unit, sid, sid_from_log) == FALSE) {
            rv = SHR_E_FAIL;
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (log_entry) {
        bcmlt_entry_free(log_entry);
    }
    if (log_status_entry) {
        bcmlt_entry_free(log_status_entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcma_testutil_ser_injection_status_validate(int unit, bcmdrd_sid_t sid,
                                            int index, int instance,
                                            int copy, bool inject_only)
{
    bcmlt_entry_handle_t inject_status_entry = 0;
    int rv = SHR_E_NONE;
    uint64_t injection_status = 0, validation = 0;
    const char *pt_name_reported;
    uint64_t index_reported = 0, instance_reported = 0, copy_reported = 0;
    bcmdrd_sid_t sid_reported = 0, sid_remap;
    int index_remap;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "SER_INJECTION_STATUS", &inject_status_entry);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(inject_status_entry,
                            BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(inject_status_entry, "SER_ERR_INJECTED",
                               &injection_status);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(inject_status_entry, "SER_ERR_INJECTED",
                               &injection_status);
    SHR_IF_ERR_EXIT(rv);

    if(injection_status == 0) {
        rv = SHR_E_FAIL;
        SHR_IF_ERR_EXIT(rv);
    }
    if (!inject_only) {
        rv = bcmlt_entry_field_get(inject_status_entry, "SER_ERR_CORRECTED",
                                 &validation);
        SHR_IF_ERR_EXIT(rv);

        if(validation == 0) {
            rv = SHR_E_FAIL;
            SHR_IF_ERR_EXIT(rv);
        }
    }
    rv = bcmlt_entry_field_symbol_get(inject_status_entry, "PT_ID",
                                      &pt_name_reported);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmdrd_pt_name_to_sid(unit, pt_name_reported, &sid_reported);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(inject_status_entry, "PT_INDEX",
                               &index_reported);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(inject_status_entry, "PT_INSTANCE",
                               &instance_reported);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(inject_status_entry, "PT_COPY",
                               &copy_reported);
    SHR_IF_ERR_EXIT(rv);

    bcmptm_ser_testutil_mem_remap(unit, sid, index, 1,
                                  &sid_remap, &index_remap, NULL);

    if (sid_remap != sid_reported ||
        index_remap != (int)index_reported ||
        instance != (int)instance_reported ||
        copy != (int)copy_reported) {
        cli_out("Inject SER error to PT [%s]<>[%s] index [%d] instance [%d] copy [%d].\n",
                pt_name_reported, bcmdrd_pt_sid_to_name(unit, sid_remap),
                (int)index_reported, (int)instance_reported, (int)copy_reported);
        cli_out("Instead of PT [%s] index [%d] instance [%d] copy [%d].\n",
                bcmdrd_pt_sid_to_name(unit, sid), index, instance, copy);
        rv = SHR_E_FAIL;
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (inject_status_entry) {
        bcmlt_entry_free(inject_status_entry);
    }
    SHR_FUNC_EXIT();

}

static void
bcma_testutil_ser_bb_parity_ctrl_check(int unit)
{
    int rv = SHR_E_NONE;
    bcmdrd_ser_rmr_id_t rid = 0;
    size_t num_rid = 0, i;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;
    bcmdrd_ser_rmr_id_t *rid_list = NULL;

    (void)bcmdrd_pt_ser_rmr_id_list_get(unit, 0, NULL, &num_rid);

    rid_list = sal_alloc(sizeof(bcmdrd_ser_rmr_id_t) * (num_rid + 1), "bcmaSerBufTst");

    (void)bcmdrd_pt_ser_rmr_id_list_get(unit, num_rid, rid_list, &num_rid);

    for (i = 0; i < num_rid; i++) {
        rid = rid_list[i];
        sal_memset(&ctrl_info, 0, sizeof(ctrl_info));
        rv = bcmdrd_pt_ser_rmr_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC, rid, &ctrl_info);
        tr_ctrl_stats[unit].int_bb_en_num++;
        if (SHR_FAILURE(rv)) {
            tr_ctrl_stats[unit].int_bb_en_num_fail++;
            cli_out("ERR: No ECC check of buffer or bus [%s].\n",
                    bcmdrd_pt_ser_rmr_to_name(unit, rid));
            continue;
        }
        rv = bcmptm_ser_testutil_ctrl_reg_validate(unit, ctrl_info.sid, ctrl_info.fid);
        if (SHR_FAILURE(rv)) {
            tr_ctrl_stats[unit].int_bb_en_num_fail++;
            cli_out("ERR: ECC check of buffer or bus [%s] is not enabled.\n",
                    bcmdrd_pt_ser_rmr_to_name(unit, rid));
        }
    }
    for (i = 0; i < num_rid; i++) {
        rid = rid_list[i];
        sal_memset(&ctrl_info, 0, sizeof(ctrl_info));
        rv = bcmdrd_pt_ser_rmr_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_ECC1B, rid, &ctrl_info);
        tr_ctrl_stats[unit].int_bb_1bit_num++;
        if (SHR_FAILURE(rv)) {
            tr_ctrl_stats[unit].int_bb_no_1bit_num++;
            cli_out("WARN: No 1bit error reporting of buffer or bus [%s].\n",
                    bcmdrd_pt_ser_rmr_to_name(unit, rid));
            continue;
        }
        rv = bcmptm_ser_testutil_ctrl_reg_validate(unit, ctrl_info.sid, ctrl_info.fid);
        if (SHR_FAILURE(rv)) {
            tr_ctrl_stats[unit].int_bb_1bit_num_fail++;
            cli_out("ERR: bit error reporting of buffer or bus [%s] is not enabled.\n",
                    bcmdrd_pt_ser_rmr_to_name(unit, rid));
        }
    }
    if (rid_list != NULL) {
        sal_free(rid_list);
    }
}

static void
bcma_testutil_ser_pt_parity_ctrl_check(int unit)
{
    bcmdrd_sid_t sid = 0;
    int rv = SHR_E_NONE;
    size_t sid_count = 0;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;
    bool is_tcam;

    (void)bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_count);

    for (sid = 0; sid < sid_count; sid++) {
        /* Test SER control field */
        sal_memset(&ctrl_info, 0, sizeof(ctrl_info));
        is_tcam = bcmdrd_pt_attr_is_cam(unit, sid);
        if (is_tcam) {
            rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_TCAM,
                                           sid, &ctrl_info);
        } else {
            rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC,
                                           sid, &ctrl_info);
        }
        if (SHR_FAILURE(rv)) {
            continue;
        }
        tr_ctrl_stats[unit].pt_en_num++;
        rv = bcmptm_ser_testutil_ctrl_reg_validate(unit, ctrl_info.sid, ctrl_info.fid);
        if (SHR_FAILURE(rv)) {
            tr_ctrl_stats[unit].pt_en_num_fail++;
            cli_out("ERR: SER CTRL register of PT[%s] is not enabled.\n",
                    bcmdrd_pt_sid_to_name(unit, sid));
        }
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_ECC1B,
                                       sid, &ctrl_info);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        tr_ctrl_stats[unit].pt_1bit_num++;
        rv = bcmptm_ser_testutil_ctrl_reg_validate(unit, ctrl_info.sid, ctrl_info.fid);
        if (SHR_FAILURE(rv)) {
            tr_ctrl_stats[unit].pt_1bit_num_fail++;
            cli_out("ERR: 1bit err reporting register of PT[%s] is not enabled.\n",
                    bcmdrd_pt_sid_to_name(unit, sid));
        }
    }
}

static void
bcma_testutil_ser_stride_get(int unit, bcma_testutil_ser_param_t *ser_param,
                             bcmdrd_sid_t pt_id, int index_num,
                             int inst_num, int copy_num, int *index_stride,
                             int *inst_stride, int *copy_stride)
{
    int is_port_reg = 0, pipe_num;

    if (bcmdrd_pt_is_port_reg(unit, pt_id) ||
        bcmdrd_pt_is_soft_port_reg(unit, pt_id)) {
        is_port_reg = 1;
    }
    if (ser_param->only_one_inst == 0) {
        /* All instance */
        /* Instance number of ADDR_SPLIT and ADDR_SPLIT_SPLIT is 1 */
        if (bcmptm_ser_testutil_pt_acctype_is_addr_split(unit, pt_id) ||
            bcmptm_ser_testutil_pt_acctype_is_addr_split_split(unit, pt_id)) {
            pipe_num = bcmdrd_pt_num_pipe_inst(unit, pt_id);
            if (is_port_reg) {
                *index_stride = index_num;
                *inst_stride = inst_num / pipe_num;
                *inst_stride = *inst_stride ? *inst_stride : 1;
                *copy_stride = 1;
            } else {
                *index_stride = index_num / pipe_num;
                *index_stride = *index_stride ? *index_stride : 1;
                *inst_stride = 1;
                *copy_stride = 1;
            }
        } else {
            if (is_port_reg) {
                *index_stride = index_num;
                *inst_stride = inst_num;
                /* copy number is larger than 1 */
                *copy_stride = 1;
            } else {
                if (sal_strcasecmp(ser_param->index_type, "spread") == 0) {
                    *index_stride = index_num / 2 - 1;
                    *index_stride = (*index_stride > 0) ? *index_stride : 1;
                } else {
                    *index_stride = index_num;
                }
                *inst_stride = 1;
                *copy_stride = 1;
            }
        }
    } else if (sal_strcasecmp(ser_param->index_type, "spread") == 0) {
        if (is_port_reg) {
            *index_stride = index_num;
            *inst_stride = inst_num / 2 - 1;
            *inst_stride = (*inst_stride > 0) ? *inst_stride : 1;
            *copy_stride = copy_num ? copy_num : 1;
        } else {
            *inst_stride = inst_num;
            *index_stride = index_num / 2 - 1;
            *index_stride = (*index_stride > 0) ? *index_stride : 1;
            *copy_stride = 1;
        }
    } else {
        /* Only one instance and only one index */
        *index_stride = index_num;
        *inst_stride = inst_num;
        *copy_stride = 1;
    }
}

static int
bcma_testutil_ser_injection_commit(int unit, bcma_testutil_ser_param_t *ser_param,
                                   int index, int inst, int copy_no, int check_type,
                                   bcmlt_entry_handle_t *inject_entry)
{
    int rv = SHR_E_NONE, single_bit_err;
    uint64_t xor_bank, wait_time;
    uint64_t copy_no_u64 = copy_no, inst_u64 = inst, index_u64 = index;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_field_symbol_add(*inject_entry, "PT_ID", ser_param->pt_id_name);
    SHR_IF_ERR_EXIT(rv);

    xor_bank = ser_param->xor_bank ? 1: 0;
    rv = bcmlt_entry_field_add(*inject_entry, "XOR_BANK", xor_bank);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_symbol_add(*inject_entry, "INJECT_VALIDATE",
                                      ser_param->inject_only ?
                                      "SER_NO_VALIDATION":"SER_VALIDATION");
    SHR_IF_ERR_EXIT(rv);

    wait_time = ser_param->wait_time;
    if (wait_time == 0) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL)) {
            wait_time = 500;
        } else {
            wait_time = TIME_TO_WAIT;
        }
    }
    if (ser_param->inject_only == 0) {
        rv = bcmlt_entry_field_add(*inject_entry, "TIME_TO_WAIT", wait_time);
        SHR_IF_ERR_EXIT(rv);
    }
    if (ser_param->err_bit_num == 0) {
        /* 1: parity check 2: ecc check */
        single_bit_err = (check_type == 1) ? 1 : 0;
    } else {
        single_bit_err = (ser_param->err_bit_num == 1) ? 1 : 0;
    }
    rv = bcmlt_entry_field_symbol_add(*inject_entry, "INJECT_ERR_BIT_NUM",
                                      single_bit_err ?
                                      "SER_SINGLE_BIT_ERR" : "SER_DOUBLE_BIT_ERR");
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_add(*inject_entry, "PT_COPY", copy_no_u64);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_add(*inject_entry, "PT_INSTANCE", inst_u64);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_add(*inject_entry, "PT_INDEX", index_u64);
    SHR_IF_ERR_EXIT(rv);
    rv = bcmlt_entry_commit(*inject_entry,
                            BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

    if (ser_param->inject_only) {
        /*
         * Wait for SRAM SCAN to detect the injected SER error, and wait for
         * SERC thread to correct the injected SER error.
         */
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL)) {
            sal_usleep(wait_time * 50);
        } else {
            sal_usleep(wait_time * 500);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcma_testutil_ser_check_entry_data(int unit, bcmdrd_sid_t sid,
                                   int index, int inst, int copy_no, bool ecc,
                                   int resp_type,
                                   bool single_bit_err,
                                   uint32_t *exp_sw_entry_data,
                                   uint32_t *exp_hw_entry_data)
{
    int rv = SHR_E_NONE, i, j, diff_bit = 0, re_enable = 0;
    int entry_wsize = 0;
    uint32_t hw_entry_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t sw_entry_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t xor_value;
    bool resp_none = 0, resp_clear = 0, resp_cache = 0;

    SHR_FUNC_ENTER(unit);

    if (resp_type == SER_RECOVERY_NO_OPERATION) {
        resp_none = 1;
    } else if (resp_type == SER_RECOVERY_ENTRY_CLEAR) {
        resp_clear = 1;
    } else if (resp_type == SER_RECOVERY_CACHE_RESTORE) {
        resp_cache = 1;
    } else {
        cli_out("Recovery type [%d] of [%s] is invalid!\n",
                resp_type, bcmdrd_pt_sid_to_name(unit, sid));
        rv = SHR_E_FAIL;
        SHR_IF_ERR_EXIT(rv);
    }
    if (resp_none) {
        /* SER error is not corrected. Before read it, SER check should be disabled. */
        rv = bcma_testutil_ser_check_enable(unit, sid, 0);
        SHR_IF_ERR_EXIT(rv);
        re_enable = 1;
    }
    sal_memset(hw_entry_data, 0, sizeof(hw_entry_data));
    /* From HW */
    rv = bcmptm_ser_testutil_pt_read(unit, sid, index, inst,
                                     copy_no, hw_entry_data,
                                     BCMDRD_MAX_PT_WSIZE, 1);
    SHR_IF_ERR_EXIT(rv);

    if (bcmptm_ser_testutil_pt_acctype_is_data_split(unit, sid)) {
        /* Read data from all pipes */
        inst = -1;
    }
    /* From PTcache */
    sal_memset(sw_entry_data, 0, sizeof(sw_entry_data));
    rv = bcmptm_ser_testutil_pt_read(unit, sid, index, inst,
                                     copy_no, sw_entry_data,
                                     BCMDRD_MAX_PT_WSIZE, 0);
    SHR_IF_ERR_EXIT(rv);

    if (sw_entry_data[0] == 0) {
        cli_out("No PTcache for [%s]\n", bcmdrd_pt_sid_to_name(unit, sid));
        if (resp_cache || resp_clear) {
            if (resp_cache) {
                cli_out("WARN: There is no PTcache for [%s],"
                        " but recovery type of the PT is \'cache_restore\'.\n",
                        bcmdrd_pt_sid_to_name(unit, sid));
            }
            sal_memset(exp_sw_entry_data, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
            sal_memset(exp_hw_entry_data, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
        }
        /* From H/W, but clear parity and hit fields */
        sal_memcpy(sw_entry_data, hw_entry_data, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);

        bcmptm_ser_testutil_parity_field_clear(unit, sid, sw_entry_data);
    } else {
        if (resp_clear) {
            cli_out("WARN: There is PTcache for [%s], but recovery type of the PT is \'clear\'.\n"
                    " So after SER correction, data is inconsistent between PTcache and H/W table.\n",
                    bcmdrd_pt_sid_to_name(unit, sid));
            sal_memset(exp_hw_entry_data, 0, sizeof(uint32_t) * BCMDRD_MAX_PT_WSIZE);
        }
    }
    entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
    diff_bit = 0;
    rv = SHR_E_NONE;
    for (i = 0; i < entry_wsize; i++) {
        xor_value = (sw_entry_data[i] ^ exp_sw_entry_data[i]);
        /* ERR is injected to ECC or parity field, so value of data fields is same. */
        if (xor_value) {
            rv = SHR_E_FAIL;
            break;
        }
        /* XOR */
        xor_value = (hw_entry_data[i] ^ exp_hw_entry_data[i]);
        if (xor_value == 0) {
            continue;
        }
        if (resp_none == 0) {
            rv = SHR_E_FAIL;
            break;
        }
        /* PT which recovery is none */
        for (j = 0; j < 32; j++) {
            if (xor_value & (1 << j)) {
                diff_bit++;
            }
        }
        if (diff_bit > 2) {
            rv = SHR_E_FAIL;
            break;
        } else if ((diff_bit == 2 && single_bit_err == 1) ||
                   (diff_bit == 1 && single_bit_err == 0)) {
            rv = SHR_E_FAIL;
            break;
        }
    }
    if (SHR_SUCCESS(rv) && resp_none && !diff_bit) {
        cli_out("SER error of [%s] index [%d] inst [%d] copy_no [%d]"
                " should not be corrected, which recovery type is none.\n",
                bcmdrd_pt_sid_to_name(unit, sid), index, inst, copy_no);
        rv = SHR_E_FAIL;
        /* exit */
        SHR_IF_ERR_EXIT(rv);
    }
    if (SHR_FAILURE(rv)) {
        cli_out("Fail to correct SER error of [%s] index [%d] inst [%d]"
                " copy_no [%d] with right data.\n",
                bcmdrd_pt_sid_to_name(unit, sid), index, inst, copy_no);
        for (i = 0; i < entry_wsize; i++) {
            xor_value = sw_entry_data[i] ^ exp_sw_entry_data[i];
            if (xor_value) {
                cli_out("Index 0x%x: expected sw: 0x%08x, sw: 0x%08x.\n",
                        i, exp_sw_entry_data[i], sw_entry_data[i]);
            }
            xor_value = hw_entry_data[i] ^ exp_hw_entry_data[i];
            if (xor_value) {
                cli_out("Index 0x%x: expected hw: 0x%08x, hw: 0x%08x.\n",
                        i, exp_hw_entry_data[i], hw_entry_data[i]);
            }
        }
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (re_enable) {
         (void)bcma_testutil_ser_check_enable(unit, sid, 1);
    }
    SHR_FUNC_EXIT();
}

void
bcma_testutil_ser_print_help(int unit)
{

    cli_out("%s", SER_PARAM_HELP);

    cli_out("%s", SER_EXAMPLES);

    return;
}

bool
bcma_testutil_ser_enabled(int unit)
{
    uint64_t fld_val;
    bool psim = FALSE, bcmsim = FALSE;

    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bcmsim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    if (psim || bcmsim) {
        return FALSE;
    } else {
        fld_val = bcmbd_config_get(unit, 1, "SER_CONFIG", "SER_ENABLE");
        return fld_val ? TRUE : FALSE;
    }
}

int
bcma_testutil_ser_counter_poll_config(int unit, bool disable)
{
    int rv;
    static uint64_t poll_stopped = 0;
    bcmlt_entry_handle_t ent_hdl = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "CTR_CONTROL", &ent_hdl);
    SHR_IF_ERR_EXIT(rv);

    if (disable) {
        rv = bcmlt_entry_commit(ent_hdl,
                                BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmlt_entry_field_get(ent_hdl, "COLLECTION_ENABLE", &poll_stopped);
        SHR_IF_ERR_EXIT(rv);

        /* Disable counter collection */
        rv = bcmlt_entry_field_add(ent_hdl, "COLLECTION_ENABLE", 0);
        SHR_IF_ERR_EXIT(rv);
    } else {
        /* Revert counter collection */
        rv = bcmlt_entry_field_add(ent_hdl, "COLLECTION_ENABLE", poll_stopped);
        SHR_IF_ERR_EXIT(rv);
    }
    if (poll_stopped != 0) {
        rv = bcmlt_entry_commit(ent_hdl,
                                BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (ent_hdl) {
        bcmlt_entry_free(ent_hdl);
    }
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to %s counter polling\n", disable ? "disable" : "revert");
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_mem_scan_enable(int unit, int enable, int resume)
{
    bcmlt_entry_handle_t control_entry = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    uint64_t enable_scan = enable ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "SER_CONTROL", &control_entry);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_add(control_entry, "TCAM_SCAN", enable_scan);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_add(control_entry, "SRAM_SCAN", enable_scan);
    SHR_IF_ERR_EXIT(rv);

    if (resume == 0) {
        rv = bcmlt_entry_field_add(control_entry, "SRAM_ENTRIES_READ_PER_INTERVAL", 64 * 1024);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmlt_entry_field_add(control_entry, "SRAM_SCAN_CHUNK_SIZE", 1024);
        SHR_IF_ERR_EXIT(rv);
    } else {
        rv = bcmlt_entry_field_add(control_entry, "SRAM_ENTRIES_READ_PER_INTERVAL", 4096);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmlt_entry_field_add(control_entry, "SRAM_SCAN_CHUNK_SIZE", 256);
        SHR_IF_ERR_EXIT(rv);
    }

    rv = bcmlt_entry_commit(control_entry, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (control_entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(control_entry);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_inject_validate(int unit, bcma_testutil_ser_param_t *ser_param, int *num_entry)
{
    bcmlt_entry_handle_t inject_entry = 0;
    int rv = SHR_E_NONE, rv_data_check, validate = 1;
    bcmdrd_sid_t pt_id = INVALIDm, pt_id_ptcache = INVALIDm;
    int copy_num, inst_num, index_num, check_type, recovery_type;
    int ser_event_num = 0, ser_event_missed = 0;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE], hw_entry_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t pipe_map;
    int inst_stride = 1, index_stride = 1, copy_stride = 1;
    int index = 0, index_ptcache, inst = 0, copy_no = 0, copy_no_temp;
    int ecc, recovery_type_2, single_bit_err, clr_ser_err = 0;
    int blktype, num_test_entry = 0, i;
    const char *pt_id_cache_view = NULL;
    int index_start , index_end ;

    SHR_FUNC_ENTER(unit);

    single_bit_err = (ser_param->err_bit_num == 1) ? 1 : 0;

    rv = bcma_testutil_ser_pt_status_get(unit, ser_param->pt_id_name, single_bit_err,
                                         &index_num, &inst_num, &copy_num,
                                         &check_type, &recovery_type);
    SHR_IF_ERR_EXIT(rv);

    /* 2: ecc, 1: parity */
    ecc = (check_type == 2) ? 1 : 0;

    if (inst_num == 0 || index_num == 0) {
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }
    rv = bcmdrd_pt_name_to_sid(unit, ser_param->pt_id_name, &pt_id);
    SHR_IF_ERR_EXIT(rv);
       SHR_IF_ERR_EXIT
        (bcma_testutil_cond_init(unit, pt_id, BCMA_TESTUTIL_PT_MEM_TR_144));
    rv = bcma_testutil_drv_mem_address_adjust(unit ,pt_id, &index_start , &index_end,
                             BCMA_TESTUTIL_PT_MEM_TR_144);

    index = ser_param->index;
    pt_id_ptcache = pt_id;
    recovery_type_2 = recovery_type;

    bcmptm_ser_testutil_mem_remap(unit, pt_id, index,
                                  0, &pt_id_ptcache,
                                  &index_ptcache, NULL);
    if (pt_id != pt_id_ptcache) {
        pt_id_cache_view = bcmdrd_pt_sid_to_name(unit, pt_id_ptcache);
        rv = bcma_testutil_ser_pt_status_get
            (unit, pt_id_cache_view, single_bit_err, NULL,
             NULL, NULL, NULL, &recovery_type_2);
        SHR_IF_ERR_EXIT(rv);

        if (recovery_type_2 != SER_RECOVERY_CACHE_RESTORE) {
            /* No PTcache view, so assign PTcache view as old view SID */
            pt_id_ptcache = pt_id;
        } else {
            recovery_type = recovery_type_2;
        }
    }

    bcma_testutil_ser_stride_get(unit, ser_param, pt_id,
                                 index_num, inst_num, copy_num,
                                 &index_stride, &inst_stride, &copy_stride);

    rv = bcmlt_entry_allocate(unit, "SER_INJECTION", &inject_entry);
    SHR_IF_ERR_EXIT(rv);

    if (ser_param->check_entry_data) {
        sal_memset(entry_data, 0, sizeof(entry_data));
        rv = bcma_testutil_ser_ih_mem_datamask_get(unit, pt_id_ptcache, entry_data);
        SHR_IF_ERR_EXIT(rv);
    }

    blktype = bcmdrd_pt_blktype_get(unit, pt_id, 0);
    (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &pipe_map);

    if (recovery_type == SER_RECOVERY_NO_OPERATION) {
        clr_ser_err = 1;
    }
    if (ser_param->inject_only &&
        bcma_testutil_ser_scan_mode_oper_get(unit, pt_id)) {
        validate = 0;
        /*
         * SCAN threads do not detect the injected SER error.
         * So need to clear it here.
         */
        clr_ser_err = 1;
    }

    for (; index < index_num; index += index_stride) {
        inst = ser_param->inst;
        for (; inst < inst_num; inst += inst_stride) {
            if (!bcmdrd_pt_index_valid(unit, pt_id, inst, index)) {
                continue;
            }
            if (!(pipe_map & (1 << inst))) {
                continue;
            }
            copy_no = (copy_num > 0) ? ser_param->copy : -1;
            for (; copy_no < copy_num; copy_no += copy_stride) {
                /* For data_split, inst can be larger than 1 */
                if (!bcmptm_ser_testutil_pt_copy_no_valid(unit, pt_id, inst, copy_no)) {
                    continue;
                }
                num_test_entry++;
                copy_no_temp = (copy_no < 0) ? 0 : copy_no;

                if (ser_param->check_entry_data) {
                    if (pt_id != pt_id_ptcache) {
                        index_ptcache = index;
                        bcmptm_ser_testutil_mem_remap(unit, pt_id, index,
                                                      0, &pt_id_ptcache,
                                                      &index_ptcache, NULL);
                    }
                    /* Write non-zero data. */
                    rv = bcmptm_ser_testutil_pt_hw_write(unit, pt_id_ptcache,
                                                         index_ptcache, inst,
                                                         copy_no, entry_data, 0);
                    SHR_IF_ERR_EXIT(rv);

                    sal_memset(hw_entry_data, 0, sizeof(hw_entry_data));
                    rv = bcmptm_ser_testutil_pt_read(unit, pt_id_ptcache,
                                                     index_ptcache, inst,
                                                     copy_no, hw_entry_data,
                                                     BCMDRD_MAX_PT_WSIZE, 1);
                    SHR_IF_ERR_EXIT(rv);
                }

                rv = bcma_testutil_ser_injection_commit(unit, ser_param,
                                                        index, inst, copy_no_temp,
                                                        check_type, &inject_entry);
                SHR_IF_ERR_EXIT(rv);

                cli_out("Testing PT [%s] inst [%d] index [%d] copy [%d]...\n",
                        ser_param->pt_id_name, inst, index, copy_no);

                ser_event_num++;

                rv_data_check = SHR_E_NONE;
                /* ser_param->inject_only should be 0, when ser_param->check_entry_data = 1 */
                if (ser_param->check_entry_data) {
                    /* Check non-zero data. */
                    rv_data_check = bcma_testutil_ser_check_entry_data
                        (unit, pt_id_ptcache, index_ptcache,
                         inst, copy_no, ecc, recovery_type,
                         single_bit_err, entry_data, hw_entry_data);
                }
                if (clr_ser_err) {
                    /* Need to remove SER error before exiting.*/
                    /* From right data from cache */
                    (void)bcmptm_ser_testutil_pt_read(unit, pt_id, index, inst,
                                                      copy_no, entry_data,
                                                      BCMDRD_MAX_PT_WSIZE, 0);
                    /* Clear SER error */
                    rv = bcmptm_ser_testutil_pt_hw_write(unit, pt_id, index,
                                                         inst, copy_no,
                                                         entry_data, 1);
                    SHR_IF_ERR_EXIT(rv);
                }
                rv = rv_data_check;
                SHR_IF_ERR_EXIT(rv);

                rv = bcma_testutil_ser_injection_status_validate
                    (unit, pt_id, index, inst, copy_no_temp, ser_param->inject_only);
                SHR_IF_ERR_EXIT(rv);

                if (validate) {
                    rv = bcma_testutil_ser_log_validate(unit, pt_id, index, inst);
                    SHR_IF_ERR_EXIT(rv);
                }
            }
        }
    }

exit:
    if (inject_entry) {
        bcmlt_entry_free(inject_entry);
    }
    *num_entry = num_test_entry;
    if (SHR_SUCCESS(rv) && validate) {
        if (recovery_type == SER_RECOVERY_NO_OPERATION) {
            ser_event_num *= 2;
        }
        i = RETRY_TIME;
        while (i-- > 0) {
            rv = bcma_testutil_ser_ser_stats_validate(unit, pt_id,
                                                      recovery_type,
                                                      ser_event_num,
                                                      &ser_event_missed);
            if (ser_event_missed == 0) {
               break;
            }
            ser_event_num = ser_event_missed;
            /* Wait for all SER events are handled and recorded */
            sal_usleep(TIME_TO_WAIT * 500);
        }
        if (ser_event_missed != 0) {
            cli_out("Num[%d] of SER event is missed.\n", ser_event_missed);
            /* rv == SHR_E_FAIL */
        }
    }
    bcma_testutil_cond_cleanup (unit, pt_id , BCMA_TESTUTIL_PT_MEM_TR_144);
    if (SHR_SUCCESS(rv)) {
        cli_out("PT [%s] passes the test.\n", ser_param->pt_id_name);
    } else {
        cli_out("Fail to test index [%d] inst [%d] copy [%d] of PT [%s].\n",
                index, inst, copy_no, ser_param->pt_id_name);
        SHR_IF_ERR_CONT(rv);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_all_pts_test(int unit, bcma_testutil_ser_param_t *ser_param)
{
    bcmdrd_sid_t sid = 0;
    size_t sid_num_all = 0, sid_iter = 0, sid_num_tmp;
    int sid_tested_all_num, tcam_sid_tested_num = 0;
    int num_entry_tested = 0, num_entry = 0;
    int rv = SHR_E_NONE, single_bit_test;
    int copy_num, inst_num, index_num, check_type, recovery_type;
    bcmlt_entry_handle_t ser_stats = 0;
    uint64_t total_cnt_cur = 0;

    SHR_FUNC_ENTER(unit);

    /* Try to test all PTs */
    rv = bcmdrd_pt_sid_list_get(unit, 0, NULL, &sid_num_all);
    SHR_IF_ERR_EXIT(rv);

    sid_tested_all_num = 0;
    sid_iter = ser_param->ssid;
    ser_param->test_pt_num = ser_param->test_pt_num ?
        ser_param->test_pt_num : (int)sid_num_all;

    sid_num_tmp = ser_param->test_pt_num + ser_param->ssid;
    sid_num_all = (sid_num_tmp < sid_num_all) ? sid_num_tmp : sid_num_all;

    for (; sid_iter < sid_num_all; sid_iter++) {
        sid = (bcmdrd_sid_t)sid_iter;
        if (bcma_testutil_ser_pt_skip(unit, sid, ser_param->inject_only)) {
            continue;
        }
        sid_tested_all_num++;
        if (bcmdrd_pt_attr_is_cam(unit, sid)) {
            tcam_sid_tested_num++;
        }
    }
    bcma_testutil_ser_tr_log_init(unit, sid_tested_all_num, tcam_sid_tested_num);
    ser_param->xor_bank = 0;

    rv = bcmlt_entry_allocate(unit, "SER_STATS", &ser_stats);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_symbol_add(ser_stats, "BLK_TYPE", "SER_BLK_ALL");
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_symbol_add(ser_stats, "RECOVERY_TYPE", "SER_RECOVERY_ALL");
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(ser_stats, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(ser_stats, "TOTAL_ERR_CNT", &total_cnt_cur);
    if (total_cnt_cur == 0) {
        ser_stats_total_cnt = 0;
        sal_memset(ser_stats_detailed_cnt, 0, sizeof(ser_stats_detailed_cnt));
    }

    sid_iter = ser_param->ssid;
    for (; sid_iter < sid_num_all; sid_iter++) {
        sid = (bcmdrd_sid_t)sid_iter;
        if (bcma_testutil_ser_pt_skip(unit, sid, 0)) {
            continue;
        }
        ser_param->pt_id_name = bcmdrd_pt_sid_to_name(unit, sid);

        single_bit_test = ser_param->single_bit_test ? 1 : 0;

        rv = bcma_testutil_ser_pt_status_get(unit, ser_param->pt_id_name,
                                             single_bit_test,
                                             &index_num, &inst_num, &copy_num,
                                             &check_type, &recovery_type);
        if (SHR_FAILURE(rv)) {
            bcma_testutil_ser_fail_pt_record(unit, sid, SER_TEST_FAIL);
            continue;
        }
        /* Test 1bit error for ECC checking */
        if (single_bit_test && check_type != 2) {
            bcma_testutil_ser_fail_pt_record(unit, sid, SER_SKIP_PARITY_CHECK);
            continue;
        }
        if (ser_param->all_resp_type == 0 &&
            recovery_type == SER_RECOVERY_NO_OPERATION) {
            bcma_testutil_ser_fail_pt_record(unit, sid, SER_SKIP_RESP_NONE);
            continue;
        }
        if (ser_param->inject_only &&
            bcma_testutil_ser_pt_skip(unit, sid, 1)) {
            bcma_testutil_ser_fail_pt_record(unit, sid, SER_SKIP_NO_SW_SCAN);
            continue;
        }
        if (ser_param->rand_gen_info) {
            rv = bcma_testutil_ser_pt_info_gen(unit, index_num, inst_num,
                                               copy_num, check_type, ser_param);
        } else {
            if (ser_param->single_bit_test) {
                ser_param->err_bit_num = 1;
            } else {
                /* 1: parity check, 2: ecc check */
                ser_param->err_bit_num = (check_type > 1) ? 2 : 1;
            }
        }
        if (SHR_SUCCESS(rv)) {
            rv = bcma_testutil_ser_inject_validate(unit, ser_param, &num_entry);
            num_entry_tested += num_entry;
        }
        if (SHR_FAILURE(rv)) {
            bcma_testutil_ser_fail_pt_record(unit, sid, SER_TEST_FAIL);
        }
    }

    if (tr_stats[unit].pt_num_fail == 0) {
        SHR_EXIT();
    } else {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    bcma_testutil_ser_test_print(unit, num_entry_tested);
    if (ser_stats) {
        bcmlt_entry_free(ser_stats);
    }

    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_parity_ctrl_check(int unit)
{
    SHR_FUNC_ENTER(unit);

    cli_out("Test whether SER check of all blocks is enabled...\n");
    bcmptm_ser_testutil_blk_parity_ctrl_validate(unit);

    cli_out("Test whether SER check of all buffer and bus is enabled...\n");
    bcma_testutil_ser_bb_parity_ctrl_check(unit);

    cli_out("Test whether SER check of all PTs is enabled...\n");
    bcma_testutil_ser_pt_parity_ctrl_check(unit);

    bcma_testutil_ser_ctrl_reg_test_print(unit);

    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_param_parse(int unit, bcma_cli_t *cli, bcma_cli_args_t *a,
                              bcma_testutil_ser_param_t *ser_param, bool show)
{
    bcma_cli_parse_table_t pt;
    bcmdrd_sid_t sid = INVALIDm;
    int rv = SHR_E_NONE;
    char *pt_name_ptr = NULL, *index_type_ptr = NULL, *start_pt_name_str = NULL;
    int pt_name_size = 0, index_type_size = 0, valid = 0, max_index = 0;

    SHR_FUNC_ENTER(unit);

    bcma_cli_parse_table_init(cli, &pt);

    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(ser_param, SHR_E_PARAM);

    bcma_cli_parse_table_add(&pt, "SID", "str", &(pt_name_ptr), NULL);
    bcma_cli_parse_table_add(&pt, "INstance", "int", &(ser_param->inst), NULL);
    bcma_cli_parse_table_add(&pt, "IndexType", "str", &(index_type_ptr), NULL);
    bcma_cli_parse_table_add(&pt, "Index", "int", &(ser_param->index), NULL);
    bcma_cli_parse_table_add(&pt, "InjectOnly", "bool", &(ser_param->inject_only), NULL);
    bcma_cli_parse_table_add(&pt, "ErrBitnum", "int", &(ser_param->err_bit_num), NULL);
    bcma_cli_parse_table_add(&pt, "XorBank", "bool", &(ser_param->xor_bank), NULL);
    bcma_cli_parse_table_add(&pt, "WaitTime", "int", &(ser_param->wait_time), NULL);
    bcma_cli_parse_table_add(&pt, "Copy", "int", &(ser_param->copy), NULL);
    bcma_cli_parse_table_add(&pt, "TestCtrlReg", "int", &(ser_param->test_ctrl_reg), NULL);
    bcma_cli_parse_table_add(&pt, "CheckData", "int", &(ser_param->check_entry_data), NULL);

    bcma_cli_parse_table_add(&pt, "SSID", "str", &(start_pt_name_str), NULL);
    bcma_cli_parse_table_add(&pt, "PtNum", "int", &(ser_param->test_pt_num), NULL);
    bcma_cli_parse_table_add(&pt, "OneInst", "int", &(ser_param->only_one_inst), NULL);
    bcma_cli_parse_table_add(&pt, "AllRespType", "int", &(ser_param->all_resp_type), NULL);
    bcma_cli_parse_table_add(&pt, "SingleBitErr", "int", &(ser_param->single_bit_test), NULL);

    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (BCMA_CLI_ARG_CNT(a) > 0) {
        cli_out("%s: Unknown argument %s\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (show) {
        bcma_cli_parse_table_show(&pt, NULL);
        SHR_EXIT();
    }

    sal_memset(pt_name_space[unit], 0, NAME_SPACE_SIZE);
    if (ser_param->test_ctrl_reg == 1) {
        /* exit */
        SHR_EXIT();
    } else if (pt_name_ptr == NULL) {
        sal_memcpy(pt_name_space[unit], PT_NAME_NONE, sal_strlen(PT_NAME_NONE));
        ser_param->pt_id_name = pt_name_space[unit];
        if (index_type_ptr == NULL) {
            sal_memcpy(index_type_space[unit], "single", sal_strlen("single"));
        } else {
            index_type_size = sal_strlen(index_type_ptr);
            if (index_type_size >= INDEX_TYPE_SIZE) {
                cli_out("ERR: Index type is invalid\n");
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
            sal_memcpy(index_type_space[unit], index_type_ptr, index_type_size);
        }
        ser_param->index_type = index_type_space[unit];
        cli_out("INFO: Parameter SID is not configured, so one or more PTs will be tested.\n");
        ser_param->ssid = 0;
        if (start_pt_name_str != NULL) {
            (void)bcmdrd_pt_name_to_sid(unit, start_pt_name_str, &ser_param->ssid);
        }
        if (ser_param->only_one_inst == 1) {
            /* Configure instance, index etc. randomly */
            ser_param->rand_gen_info = 1;
        }
        cli_out("Test from SID[%d][%s]\n",
            ser_param->ssid, bcmdrd_pt_sid_to_name(unit, ser_param->ssid));
        /* exit */
        SHR_EXIT();
    }
    /* Only test one instance for appointed PT */
    ser_param->only_one_inst = 1;

    pt_name_size = sal_strlen(pt_name_ptr);
    if (pt_name_size >= NAME_SPACE_SIZE) {
        cli_out("ERR: String SID is too long\n");
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    sal_memset(pt_name_space[unit], 0, NAME_SPACE_SIZE);
    sal_memcpy(pt_name_space[unit], pt_name_ptr, pt_name_size);
    /* Should be uppercase characters */
    sal_strupr(pt_name_space[unit]);
    /* L2Xm, ICFG_IDM_INTR_ENABLEr */
    sal_strlwr(&pt_name_space[unit][pt_name_size - 1]);
    ser_param->pt_id_name = pt_name_space[unit];

    sal_memset(index_type_space[unit], 0, INDEX_TYPE_SIZE);
    if (index_type_ptr == NULL) {
        sal_memcpy(index_type_space[unit], "single", sal_strlen("single"));
    } else {
        index_type_size = sal_strlen(index_type_ptr);
        if (index_type_size >= INDEX_TYPE_SIZE) {
            cli_out("ERR: Index type is invalid\n");
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        sal_memcpy(index_type_space[unit], index_type_ptr, index_type_size);
    }
    ser_param->index_type = index_type_space[unit];
    rv = bcmdrd_pt_name_to_sid(unit, ser_param->pt_id_name, &sid);
    if (SHR_FAILURE(rv)) {
        cli_out("ERROR: Inputting PT name is invalid!\n");
        return SHR_E_PARAM;
    }
    if (sal_strcasecmp(ser_param->index_type, "spread") == 0) {
        if (bcmdrd_pt_is_port_reg(unit, sid) ||
            bcmdrd_pt_is_soft_port_reg(unit, sid)) {
            cli_out("WARN: inputting instance is not used for port based reg!\n");
            ser_param->inst = 0;
        } else {
            valid = bcmptm_ser_testutil_index_valid(unit, sid, 0, &max_index);
            if (valid == 0) {
                cli_out("ERR: Depth of SID is zero!\n");
                return SHR_E_FAIL;
            }
            cli_out("WARN: inputting index is not used!\n");
            ser_param->index = 0;
        }
    } else if (sal_strcasecmp(ser_param->index_type, "single") != 0) {
        cli_out("ERR: Index type is invalid!\n");
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (ser_param->err_bit_num > 2) {
        cli_out("ERR: Number of SER error bit is larger than 2!\n");
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (ser_param->inject_only && ser_param->check_entry_data) {
        cli_out("ERR: allow not to check entry data when parameter \'InjectOnly\' is true!\n");
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    bcma_cli_parse_table_done(&pt);

    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_pt_status_get(int unit, const char *pt_name,
                                bool single_bit_err, int *index_num,
                                int *inst_num, int *copy_num, int *check_type,
                                int *recovery_type)
{
    bcmlt_entry_handle_t pt_status_entry = 0;
    uint64_t data;
    int rv = SHR_E_NONE, check_type_v;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "SER_PT_STATUS", &pt_status_entry);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_symbol_add(pt_status_entry, "PT_ID", pt_name);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(pt_status_entry, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

    if (inst_num != NULL) {
        rv = bcmlt_entry_field_get(pt_status_entry, "PT_INST_NUM", &data);
        SHR_IF_ERR_EXIT(rv);
        *inst_num = (int)data;
    }
    if (copy_num != NULL) {
        rv = bcmlt_entry_field_get(pt_status_entry, "PT_COPY_NUM", &data);
        SHR_IF_ERR_EXIT(rv);
        *copy_num = (int)data;
    }
    if (index_num != NULL) {
        rv = bcmlt_entry_field_get(pt_status_entry, "PT_INDEX_NUM", &data);
        SHR_IF_ERR_EXIT(rv);
        *index_num = (int)data;
    }
    rv = bcmlt_entry_field_get(pt_status_entry, "SER_CHECK_TYPE", &data);
    SHR_IF_ERR_EXIT(rv);
    check_type_v = (int)data;

    if (check_type != NULL) {
        *check_type = check_type_v;
    }
    if (recovery_type == NULL) {
        SHR_EXIT();
    }
    if (single_bit_err) {
        rv = bcmlt_entry_field_get(pt_status_entry, "SER_RECOVERY_TYPE_FOR_SINGLE_BIT", &data);
        SHR_IF_ERR_EXIT(rv);
        *recovery_type = (int)data;
    } else {
        /* 1: parity check */
        if (check_type_v == 1) {
            rv = bcmlt_entry_field_get(pt_status_entry, "SER_RECOVERY_TYPE_FOR_SINGLE_BIT", &data);
            SHR_IF_ERR_EXIT(rv);
            *recovery_type = (int)data;
        } else {
            rv = bcmlt_entry_field_get(pt_status_entry, "SER_RECOVERY_TYPE_FOR_DOUBLE_BIT", &data);
            SHR_IF_ERR_EXIT(rv);
            *recovery_type = (int)data;
        }
    }

exit:
    if (pt_status_entry) {
        bcmlt_entry_free(pt_status_entry);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_ctrl_hsf_machine_enable(int unit, bool enable)
{
    uint64_t hsf_en = enable ? 1 : 0;
    bcmlt_entry_handle_t ser_control = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "SER_CONTROL", &ser_control);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_add(ser_control, "HIGH_SEVERITY_ERR_SUPPRESSION", hsf_en);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(ser_control, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (ser_control) {
        bcmlt_entry_free(ser_control);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ser_ctrl_squash_config(int unit, uint64_t time)
{
    bcmlt_entry_handle_t ser_control = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "SER_CONTROL", &ser_control);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_add(ser_control, "SQUASH_DUPLICATED_SER_EVENT_INTERVAL", time);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(ser_control, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (ser_control) {
        bcmlt_entry_free(ser_control);
    }
    SHR_FUNC_EXIT();
}

