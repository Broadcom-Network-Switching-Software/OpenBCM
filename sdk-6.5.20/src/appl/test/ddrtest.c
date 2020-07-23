/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>

#include <sal/types.h>

#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/appl/pci.h>
#include <sal/appl/config.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/progress.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/l2x.h>
#include <soc/shmoo_ddr40.h>
#include <soc/phy/ddr40.h>
#include <soc/shmoo_sjc16.h>
#include <soc/phy/ydc_ddr_bist.h>

#include "testlist.h"

#ifdef BCM_DDR3_SUPPORT

#define CI_DDR_MASK 0x3fffff /* [21:0] bit field for writes */
#define C3_CI_DDR_MASK 0x7fffff /* [22:0] bit field for writes */

#define DEFAULT_PATTERN 0xd0d1d0d1

/* DDR Bist Flags*/
#define DDR_BIST_CONS_ADDR_8_BANKS     (0x01)
#define DDR_BIST_ADDRESS_SHIFT_MODE    (0X02)
#define DDR_BIST_INFINITE              (0x04)
#define DDR_BIST_ALL_ADDRESS           (0x08)
#define DDR_BIST_STOP                  (0x10)
#define DDR_BIST_GET_DATA              (0x20)
#define DDR_BIST_TWO_ADDRESS_MODE      (0x40)

typedef enum _DDRMemTests {
  DDR_ALL_TESTS = 0,
  DDR_STANDARD_TEST = 1,
  DATA_BUS_WALKING_ONES = 2,
  DATA_BUS_WALKING_ZEROS = 3,
  DDR_DATA_EQ_ADDR = 4,
  DDR_INDIRECT_TEST = 5,
  /* leave as last */
  MEM_TEST_LAST
} DDRMemTests_t;

typedef enum ddr_bist_data_pattern_mode {
    /*
     * costum pattern. Use the pattern as is.
     */
    DRAM_BIST_DATA_PATTERN_CUSTOM = 0,
    /*
     *  The PRBS will be used to generate the pattern towards
     *  the DRAM.
     */
    DRAM_BIST_DATA_PATTERN_RANDOM_PRBS = 1,
    /*
     *  Fill the data to write by 1010101... (Bits). The DATA_MODE
     *  may use this data pattern in different ways see
     *  ARAD_DRAM_BIST_DATA_MODE. 
     */
    DRAM_BIST_DATA_PATTERN_DIFF = 2,
    /*
     *  Fill the data to write by 11111111... (Bits). The DATA_MODE
     *  may use this data pattern in different ways see
     *  ARAD_DRAM_BIST_DATA_MODE. Random mode ignores these
     *  values.
     */
    DRAM_BIST_DATA_PATTERN_ONE = 3,
    /*
     *  Fill the data to write by 00000000... (Bits). The DATA_MODE
     *  may use this data pattern in different ways see
     *  ARAD_DRAM_BIST_DATA_MODE. Random mode ignores these
     *  values.
     */
    DRAM_BIST_DATA_PATTERN_ZERO = 4,
    /*
     * A different bit is selected from pattern0 - pattern7 in an incremental manner.
     * The selected bit is duplicated on all of the dram data bus.
     */
    DRAM_BIST_DATA_PATTERN_BIT_MODE = 5,
    /*
     * data shift mode.
     * Every write/read command the data will be shifted 1 bit to the left in a cyclic manner.
     * The initial pattern is pattern0 which is duplicated 8 times.
     * This DATA_MODE create 8 consecutive dram transactions.
     */
    DRAM_BIST_DATA_PATTERN_SHIFT_MODE = 6,
    /*
     * data address mode.
     * Every command data will be equal to the address it is written to.
     */
    DRAM_BIST_DATA_PATTERN_ADDR_MODE = 7,
    /*
     * nof data pattern modes
     */
    DRAM_BIST_NOF_DATA_PATTERN_MODES = 8
} ddr_bist_data_pattern_mode_t;

#define DRAM_BIST_NOF_PATTERNS (8)

typedef struct ddr_bist_test_run_info_s {
    /*
     *  Number of Write commands per cycle. Range 0-225. 0 then
     *  no write will be performed.
     */
    uint32 write_weight;
    /*
     *  Number of Read commands per cycle. Range 0-225. 0 then
     *  no write will be performed.
     */
    uint32 read_weight;
    /*
     * number of actions
     */
    uint32 bist_num_actions;
    /*
     *  Start address (offset) of the Bist test commands.
     *  The address must be a multiple of 32 Bytes.
     *  Units: Bytes. Range: 0 - DRAM size-32.
     */
    uint32 bist_start_address;
    /*
     *  End address (offset) of the Bist test commands. If end address is
     *  reached, address is reset to the initial write or read
     *  address. The format is the same as the start address.
     *  For example, for 1 Gb interface, set to 0x7FFFFE0.
     *  For 2 Gb interface, set to 0xFFFFFE0.
     */

    uint32 bist_end_address;
    /*
     * pattern info
     */
    ddr_bist_data_pattern_mode_t pattern_mode;
    /*
     * patterns
     */
    uint32 pattern[DRAM_BIST_NOF_PATTERNS];   
    /*
     * flags
     */
    uint32 ddr_bist_flags;
} ddr_bist_test_run_info_t;

const char* DDRMemTestStr[] = {"All DDR tests",
                        "DDR Standard burst",
                        "DDR walking ones on data bus",
                        "DDR walking zeros on data bus",
                        "DDR Data == Address",
                        "DDR Indirect pattern"};

typedef struct ddr_test_params_s {
    int unit;
    DDRMemTests_t ddr_test_mode;
    int32 pattern;
    int ci_interface;
    int ddr_start_addr;
    int ddr_step_addr;
    int ddr_burst;
    int ddr_iter;
    int bank;
    int max_row;
    int max_col;
    int write_test;
    int read_test;
    soc_pbmp_t dram_ndx;
    ddr_bist_test_run_info_t run_info;
} ddr_test_params_t;

static ddr_test_params_t      *ddr_test_params[SOC_MAX_NUM_DEVICES];

char ddr_test_usage[] = 
"DDR Memory Test Usage:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
"MemTest=<0,1,2,3,4> <args>\n"
#else
"MemTest=<0,1,2,3,4> where:\n"
"   0 - All Standard tests\n"
"   1 - Standard DDR burst write/read test\n"
"   2 - Walking ones on data bus\n"
"   3 - Walking zeros on data bus\n"
"   4 - DDR DATA == ADDRESS test \n"
"   5 - Indirect Memory Test\n"
"                            \n"
" Options for Standard DDR test: \n"
"  StartAddr=<0xvalue>     specifies starting memory address\n"
"  AddrStepInc=<0xvalue>   specifies address step \n"
"  BurstSize=<value>       number of burst to write, followed by same number of reads\n"
"  Pattern=<0xvalue>       memory test pattern \n"
"                                             \n"
" Options for all tests except indirect:      \n"
"  Iterations=<value>      how many times to run test, defaults to one\n"
"                         note: iterations=0 will run test continuously \n"
"  CI=<value>              CI interface to run test on, default is all of them\n"
"                                             \n"
" Indirect Memory Test options:\n"
"  Pattern=<0xvalue> if not specifed address will be used as data \n"
"  Bank=<value>      specifies bank within DDR to test (0-7)\n"
"  MaxRow=<value>    specified the maximum row within the bank to test \n"
"  MaxCol=<value>    specified the maximum column within the bank to test \n"
"  CI=<value>        what ci interface to run test on \n"
"                                             \n"
"NOTE: If MemTest is not specified or is set to a value of 0,\n"
"      then all tests (except indirect) are run\n"
#endif
;

int
ddr_test_init(int unit, args_t *a, void **p)
{
    ddr_test_params_t *dt;
    int mem_test;
    parse_table_t pt;
    int rv = -1;
    int ddr_step_addr;
    int ddr_start_addr;
    int ddr_test_count;
    int ddr_burst_size;
    int ci_interface;
    int bank, max_row, max_col;
    int pattern;
    int write_only, read_only;

    dt = ddr_test_params[unit];
    if (dt == NULL) {
        dt = sal_alloc(sizeof(ddr_test_params_t), "ddrtest");
        if (dt == NULL) {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "%s: cannot allocate memory test data\n"), ARG_CMD(a)));
            return -1;
        }
        sal_memset(dt, 0, sizeof(ddr_test_params_t));
        ddr_test_params[unit] = dt;
    }

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "MemTest", PQ_INT, (void *) 0,
          &mem_test, NULL);
    parse_table_add(&pt, "StartAddr", PQ_INT, (void*) 0,
            &ddr_start_addr, NULL);
    parse_table_add(&pt, "AddrStepInc", PQ_INT, (void *) 1,
            &ddr_step_addr, NULL);
    parse_table_add(&pt, "WriteOnly", PQ_INT, (void *) (0),
            &write_only, NULL);
    parse_table_add(&pt, "ReadOnly", PQ_INT, (void *) (0),
            &read_only, NULL);
        parse_table_add(&pt, "BurstSize", PQ_INT, (void *) (256),
            &ddr_burst_size, NULL);


    parse_table_add(&pt, "Pattern", PQ_INT, (void *) (DEFAULT_PATTERN),
          &pattern, NULL);

    parse_table_add(&pt, "Iterations", PQ_INT, (void *) 1,
            &ddr_test_count, NULL);
    parse_table_add(&pt, "CI", PQ_INT, (void *) (-1),
            &ci_interface, NULL);
    parse_table_add(&pt, "Bank", PQ_INT, (void *) (-1),
            &bank, NULL);
    parse_table_add(&pt, "MaxRow", PQ_INT, INT_TO_PTR(SOC_DDR3_NUM_ROWS(unit)),
            &max_row, NULL);
    parse_table_add(&pt, "MaxCol", PQ_INT, INT_TO_PTR(SOC_DDR3_NUM_COLUMNS(unit)),
            &max_col, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",ddr_test_usage);
        goto done;
    }

    if (ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",ddr_test_usage);
        goto done;
    }

    dt->unit = unit;
    dt->pattern = pattern;
    dt->ci_interface = ci_interface;
    dt->ddr_start_addr = ddr_start_addr;
    dt->ddr_step_addr = ddr_step_addr;
    dt->ddr_burst = ddr_burst_size;
    dt->ddr_iter = ddr_test_count;
    dt->ddr_test_mode = mem_test;
    dt->bank = bank;
    dt->max_row = max_row;
    dt->max_col = max_col;
    dt->write_test = write_only;
    dt->read_test = read_only;
    dt->max_col = max_col;
    *p = dt;
    rv = 0;

    done:
       parse_arg_eq_done(&pt);
       return rv;
}

int StartDDRFunctionalTest(ddr_test_params_t *testparams,
                            int ci_start,
                            int ci_end,
                            DDRMemTests_t mode) {

    uint32 uData, ddr_iter;
    int ci, unit;
    ddr_test_params_t *dt = testparams;
    uint32 uDDRMask;

    unit = dt->unit;
    uDDRMask = CI_DDR_MASK;
    ddr_iter = dt->ddr_iter & uDDRMask;

    /*
    * Be able to Show test results for each iteration, unless ddr_iter == 0
    * then results are displayed after some interval
    */

    if (ddr_iter != 0) ddr_iter = 1;

    /*
    * Set the test mode as follows:
    * 0 - Standard writing burst of data and alt_data, followed by reads
    * 1 - Walking ones test on the data lines (test data not used).
    * 2 - Walking zeros on the test data lines (test data not used). 
    * 3 - Used Data == Address (test data not used)
    * - clear ram_done
    * - clear ram_test
    * - clear ram_test_fail
    * - set ram_test to initiate test
    */

    uData = 0;
    for (ci = ci_start; ci < ci_end; ci++) {
    
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
        
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_ITERr(unit, ci, ddr_iter));
        SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, ci, &uData));
        soc_reg_field_set(unit, CI_DDR_TESTr, &uData, MODEf,(mode-1));
        soc_reg_field_set(unit, CI_DDR_TESTr, &uData, RAM_DONEf, 1);      /* W1TC */
        soc_reg_field_set(unit, CI_DDR_TESTr, &uData, RAM_TESTf, 0);      /* clear */
        soc_reg_field_set(unit, CI_DDR_TESTr, &uData, RAM_TEST_FAILf, 1); /* W1TC */
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TESTr(unit, ci, uData));

        /* set ram_test - to start the test */
        SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, ci, &uData));
        soc_reg_field_set(unit, CI_DDR_TESTr, &uData, RAM_TESTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TESTr(unit, ci, uData));
    }

    return BCM_E_NONE;
}


int CheckDDRFunctionalTest(ddr_test_params_t *testparams,
                            int ci_start, int ci_end,
                            uint64 *pFailedCount,
                            uint64 *pTimedOutCount,
                            uint64 *pPassedCount) {
    int unit, ci, rv = BCM_E_NONE;
    uint32 ddr_iter, uData = 0;
    uint8 bFailed = 0;
    uint8 bDone = 0;
    ddr_test_params_t *dt = testparams;
    uint32 uDDRMask;

    unit = dt->unit;
    uDDRMask = CI_DDR_MASK;
    ddr_iter = dt->ddr_iter & uDDRMask;

    for (ci = ci_start; ci < ci_end; ci++) {
        SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, ci, &uData));
        bFailed = soc_reg_field_get(unit,CI_DDR_TESTr, uData, RAM_TEST_FAILf);
        bDone = soc_reg_field_get(unit, CI_DDR_TESTr, uData, RAM_DONEf);
        if (bFailed) {
          COMPILER_64_ADD_32(pFailedCount[ci], 1); rv = -1;
        } else if (bDone == 0 && ddr_iter != 0) {
          COMPILER_64_ADD_32(pTimedOutCount[ci], 1); rv = -1;
        } else {
          COMPILER_64_ADD_32(pPassedCount[ci], 1);
        }
    }
    return rv;
}


/* show results for each iteration */
int DumpDDRResults(int unit,
                    int ci_start, int ci_end,
                    int iter_count,
                    uint64 *pFailedCount,
                    uint64 *pTimedOutCount,
                    uint64 *pPassedCount) {
    int i, ci;
    uint32 uFailedAddr=0;
    uint32 uFailedData[8] = {0};
    uint32 uData = 0;
    uint8 bFailed = 0;

    /* print results for each iteration */
    for(ci = ci_start; ci < ci_end; ci++) {
        if (iter_count != -1 ) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Iteration:%d CI%d Fail:0x%08x%08x Pass:0x%08x%08x Timedout:0x%08x%08x\n"),
                      iter_count,ci,
                      COMPILER_64_HI(pFailedCount[ci]),COMPILER_64_LO(pFailedCount[ci]),
                      COMPILER_64_HI(pPassedCount[ci]),COMPILER_64_LO(pPassedCount[ci]),
                      COMPILER_64_HI(pTimedOutCount[ci]),COMPILER_64_LO(pTimedOutCount[ci])));
        } else { /* running continuously */
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "CI%d Fail:0x%08x%08x Pass:0x%08x%08x \n"),ci,
                      COMPILER_64_HI(pFailedCount[ci]),COMPILER_64_LO(pFailedCount[ci]),
                      COMPILER_64_HI(pPassedCount[ci]),COMPILER_64_LO(pPassedCount[ci])));
        }
        /* if this CI interface failed, print the failing addr and data */
        SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, ci, &uData));
        bFailed = soc_reg_field_get(unit, CI_DDR_TESTr, uData, RAM_TEST_FAILf);
        if (bFailed) {
            SOC_IF_ERROR_RETURN(READ_CI_FAILED_ADDRr(unit,ci,&uFailedAddr));
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA0r(unit,ci,&uFailedData[0]));
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA1r(unit,ci,&uFailedData[1]));
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA2r(unit,ci,&uFailedData[2]));
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA3r(unit,ci,&uFailedData[3]));
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA4r(unit,ci,&uFailedData[4]));
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA5r(unit,ci,&uFailedData[5]));
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA6r(unit,ci,&uFailedData[6]));
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TEST_FAILED_DATA7r(unit,ci,&uFailedData[7]));
            cli_out("CI%d Failing address = (0x%08x)\n",ci,uFailedAddr);
            for (i = 0; i < 8; i++) {
                if (i == 4) {
                    cli_out("\n");
                }
                cli_out("failing_data_%d = (0x%08x)  ", i, uFailedData[i]);
            }
            cli_out("\n");

            /* clear the failing status bit for this ci */
            SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, ci, &uData));
            soc_reg_field_set(unit, CI_DDR_TESTr, &uData, RAM_TEST_FAILf, 1); /* W1TC */
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TESTr(unit, ci, uData));
        }
        
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    }
    return BCM_E_NONE;
}

/* run single DDR test */
int DDRTest(ddr_test_params_t *testparams) {
    ddr_test_params_t *dt = testparams;
    DDRMemTests_t ddr_test;
    int rv, unit, iter_count, ci, ci_start, ci_end, stat=0;
    uint32 uData=0, pattern, uDDRIter, uStartAddr, uStepSize, uDDRBurstSize;
    uint64 uFailedCount[SOC_MAX_NUM_CI_BLKS];
    uint64 uPassedCount[SOC_MAX_NUM_CI_BLKS];
    uint64 uTimedOutCount[SOC_MAX_NUM_CI_BLKS];
    uint32 uDDRMask;

    for (ci = 0; ci < SOC_MAX_NUM_CI_BLKS; ci++) {
        COMPILER_64_ZERO(uFailedCount[ci]);
        COMPILER_64_ZERO(uPassedCount[ci]);
        COMPILER_64_ZERO(uTimedOutCount[ci]);
    }

    unit = dt->unit;
    uDDRMask = CI_DDR_MASK;

    if (dt->ci_interface < 0) {
        /* test all of them */
        ci_start = 0; 
        ci_end = 3; 
    } else {
        ci_start = dt->ci_interface;
        ci_end = ci_start+1;
        LOG_WARN(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Testing only CI%d interface\n"),ci_start));
    }
    ddr_test = dt->ddr_test_mode;
    uDDRIter = dt->ddr_iter & uDDRMask;

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Running Test %s\n"), DDRMemTestStr[ddr_test]));

    if (dt->pattern != DEFAULT_PATTERN ) {
        pattern = dt->pattern & 0xffffffff;
        if (DDR_STANDARD_TEST != ddr_test) {
            LOG_WARN(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "NOTE: Test data is not used during data bus test\n")));
        }
    } else {
        /* no pattern specified use alternating 0101.. pattern */
        pattern = 0x55555555;
    }

    uStartAddr = dt->ddr_start_addr & uDDRMask;
    uStepSize = dt->ddr_step_addr & uDDRMask;
    uDDRBurstSize = dt->ddr_burst & uDDRMask;

    if (DDR_STANDARD_TEST == ddr_test ||
        DDR_DATA_EQ_ADDR == ddr_test) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META("start_addr=0x%x,addr_step_inc=0x%x,burst_size=%d\n"),uStartAddr,uStepSize,uDDRBurstSize));
        if (DDR_STANDARD_TEST == ddr_test) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META("using pattern=0x%8x, and alt_pattern=0x%8x\n"),pattern,~pattern));
        }
    }

    for (ci = ci_start; ci < ci_end; ci++) {
      /* set test specific attributes */
      /* SOC_IF_ERROR_RETURN(WRITE_CI_DEBUGr(unit,ci,0x1)); ? inject auto refresh */
      SOC_IF_ERROR_RETURN(WRITE_CI_DDR_STARTr(unit,ci,uStartAddr));
      SOC_IF_ERROR_RETURN(WRITE_CI_DDR_STEPr(unit,ci,uStepSize));
      SOC_IF_ERROR_RETURN(WRITE_CI_DDR_BURSTr(unit,ci,uDDRBurstSize));
      /* only STANDARD_DDR test uses pattern data */
      if (DDR_STANDARD_TEST == ddr_test) {
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_DATA0r(unit,ci,pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_DATA1r(unit,ci,pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_DATA2r(unit,ci,pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_DATA3r(unit,ci,pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_DATA4r(unit,ci,pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_DATA5r(unit,ci,pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_DATA6r(unit,ci,pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_DATA7r(unit,ci,pattern));
        /* set the alt_data */
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_ALT_DATA0r(unit,ci,~pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_ALT_DATA1r(unit,ci,~pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_ALT_DATA2r(unit,ci,~pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_ALT_DATA3r(unit,ci,~pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_ALT_DATA4r(unit,ci,~pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_ALT_DATA5r(unit,ci,~pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_ALT_DATA6r(unit,ci,~pattern));
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TEST_ALT_DATA7r(unit,ci,~pattern));
        SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit,ci,&uData));
      }
    }

    /* Run the particular test uDDRIter times */
    for(iter_count = 0; iter_count < uDDRIter; iter_count++) {
        /* start the test for this iteration */
        if (( rv = StartDDRFunctionalTest(dt, ci_start,
                                          ci_end, ddr_test)) < 0 ) {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "%s failed to start %s test.\n"),SOC_CHIP_STRING(unit),
                       DDRMemTestStr[ddr_test]));
            return rv;
        }
        /* wait long enough for all ci's being tested to be done */
        sal_sleep(2);

        /* check the test results for this iteration */
        if (( rv = CheckDDRFunctionalTest(dt,
                                            ci_start, ci_end,
                                            uFailedCount,
                                            uTimedOutCount,
                                            uPassedCount)) < 0 ) {
          
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "%s failed %s test.\n"),SOC_CHIP_STRING(unit),
                       DDRMemTestStr[ddr_test]));

            /* one or more CI's failed
            * dump register debug info here
            */
            stat = -1;
        }

        /* display the results for this iteration */
        if (( rv = DumpDDRResults(unit,
                                    ci_start, ci_end,
                                    iter_count,
                                    uFailedCount,
                                    uTimedOutCount,
                                    uPassedCount)) < 0) {
            return rv;
        }

    }

    /* special case if ddr_iter == 0, run test continuously */
    if (uDDRIter == 0) {
        /* start the test to run continuously */
        if (( rv = StartDDRFunctionalTest(dt, ci_start,
                                          ci_end, ddr_test)) < 0 ) {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "%s failed to start %s test.\n"),SOC_CHIP_STRING(unit),
                       DDRMemTestStr[ddr_test]));
            return rv;
        }
        cli_out("Running test continuously CTRL-C to STOP.. \n");
        while(1) {

            /* check results at some interval */
            sal_sleep(2);

            if (( rv = CheckDDRFunctionalTest(dt,
                                                ci_start, ci_end,
                                                uFailedCount,
                                                uTimedOutCount,
                                                uPassedCount)) < 0 ) {
              
                LOG_ERROR(BSL_LS_APPL_TESTS,
                          (BSL_META_U(unit,
                                      "%s failed %s test.\n"),SOC_CHIP_STRING(unit),
                           DDRMemTestStr[ddr_test]));
            
                /* one or more CI's failed
                * dump register debug info here
                */
                stat = -1;
            }

            /* display results while test is running */
            if (( rv = DumpDDRResults(unit,
                                        ci_start, ci_end,
                                        -1,
                                        uFailedCount,
                                        uTimedOutCount,
                                        uPassedCount)) < 0) {
                return rv;
            }
        }
    }
    
    /* clean up after test run */
    for (ci = ci_start; ci < ci_end; ci++) {
        SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit,ci,&uData));
        soc_reg_field_set(unit,CI_DDR_TESTr,&uData,RAM_DONEf,1);      /* W1TC */
        soc_reg_field_set(unit,CI_DDR_TESTr,&uData,RAM_TESTf,0);      /* clear */
        soc_reg_field_set(unit,CI_DDR_TESTr,&uData,RAM_TEST_FAILf,1); /* W1TC */
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TESTr(unit,ci,uData));
    }
    
    return stat;
}

/* run DDR Indirect Pattern test */
int DDRIndirectTest(ddr_test_params_t *testparams) {
    ddr_test_params_t *dt = testparams;
    uint8 bUseAddrAsData = FALSE;
    uint32 uDataWR[8] = {0};
    uint32 uDataRD[8] = {0};
    uint32 pla_addr, comp_data, pattern=0;
    uint32 row, max_row, col, max_col;
    int i, unit, bank, start_bank, end_bank, ci, ci_start, ci_end, stat;

    unit = dt->unit;

    if (dt->bank == -1) {
      start_bank = 0;
      end_bank = 8;
    } else {
      start_bank = dt->bank & 0x7;
      end_bank = start_bank+1;
    }
    
    if (dt->ci_interface < 0) {
      ci_start = 0; ci_end = SOC_DDR3_NUM_MEMORIES(unit);
    } else {
      ci_start = dt->ci_interface;
      ci_end = ci_start+1;
      LOG_WARN(BSL_LS_APPL_TESTS,
               (BSL_META_U(unit,
                           "Testing only CI%d interface\n"),ci_start));
    }
    
    /* if no pattern specified use the address as data */
    if (dt->pattern == -1) {
      bUseAddrAsData = TRUE;
    } else {
      pattern = dt->pattern & 0xffffffff;
    }

    max_row = (dt->max_row <= SOC_DDR3_NUM_ROWS(unit)) ?
                            dt->max_row : SOC_DDR3_NUM_ROWS(unit);
    max_col = (dt->max_col <= SOC_DDR3_NUM_COLUMNS(unit)) ?
                            dt->max_col : SOC_DDR3_NUM_COLUMNS(unit);

    for(ci=ci_start;ci<ci_end;ci++) {
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META("Filling ci%d\n"), ci));
        for (bank=start_bank;bank<end_bank;bank++) {
            for(row=0;row<max_row;row++) {
                for (col=0;col<max_col;col++) {
                    pla_addr = bank;
                    pla_addr |= col << 3;
                    pla_addr |= row << 9;
                    for (i = 0; i < 8; i++ ) { 
                        if (bUseAddrAsData) {
                            uDataWR[i] = pla_addr;
                        } else {
                            uDataWR[i] = pattern;
                        }
                    }
                    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META("Filling ci%d bank[%d],row[0x%x],cols[0x%x-0x%x] \n"),
                              ci,bank,row,col<<4,(col<<4)+0xf));

                    stat = soc_ddr40_write(unit,ci,pla_addr,uDataWR[0],uDataWR[1],uDataWR[2],
                             uDataWR[3],uDataWR[4],uDataWR[5],uDataWR[6], uDataWR[7]);
                    if (0 != stat) {
                        /* Write Error */
                        return stat;
                    }
                }
            }
        }
    }

    /* Read back and compare, verify each CI's DDR Memory */
    for(ci=ci_start;ci<ci_end;ci++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META("Verifying ci%d\n"), ci));
        for (bank=start_bank;bank<end_bank;bank++) {
            for(row=0;row<max_row;row++) {
                for (col=0;col<max_col;col++) {
                    pla_addr = bank;
                    pla_addr |= col << 3;
                    pla_addr |= row << 9;
                    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META("Verifing ci%d bank[%d],row[0x%x],cols[0x%x-0x%x] \n"),
                              ci,bank,row,col<<4,(col<<4)+0xf));
                    stat = soc_ddr40_read(unit,ci,pla_addr,&uDataRD[0],&uDataRD[1],&uDataRD[2],
                             &uDataRD[3],&uDataRD[4],&uDataRD[5],&uDataRD[6],&uDataRD[7]); 
                    if (0 != stat) {
                        /* Read Error */
                        return stat;
                    }
                    /* check the data */
                    for (i = 0; i < 8; i++ ) {
                        comp_data = bUseAddrAsData ? pla_addr : pattern;
                        if (uDataRD[i] != comp_data) {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                      (BSL_META_U(unit,
                                                  "Data compare failure at "
                                                  "pla_addr:(0x%x)\n"),pla_addr));
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                      (BSL_META_U(unit,
                                                  "Expected (0x%x) got (0x%x)\n"),
                                       comp_data,uDataRD[i]));
                            /* Compare Error */
                            return -1;
                        }
                    }
                }
            }
        }
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    }

    return 0;
}

int
ddr_test(int unit, args_t *a, void *p)
{
    ddr_test_params_t *dt = p;
    DDRMemTests_t ddr_test = dt->ddr_test_mode;
    int mem_test, rv = 0;

    if (DDR_ALL_TESTS == ddr_test) {
        /* All Tests */
        for(mem_test = 1; mem_test < MEM_TEST_LAST; mem_test++) {
            if (DDR_INDIRECT_TEST == mem_test) {
                /* skip the indirect test -- takes too long */
                continue;
            }
            dt->ddr_test_mode = mem_test;
            if (DDRTest(dt) != 0) {
                rv = -1;
            }
        }
        return rv;
    } else if (DDR_INDIRECT_TEST == ddr_test) {
        return DDRIndirectTest(dt);
    } else {
        return DDRTest(dt);
    }
}

int
ddr_test_done(int unit, void *p)
{
    ddr_test_params_t *dt = p;
    uint32 uData=0;
    int ci, ci_start, ci_end;
    if (dt->ci_interface < 0) {
        /* test all of them */
        ci_start = 0;
        ci_end = 3; 
    } else {
        ci_start = dt->ci_interface;
        ci_end = ci_start+1;
    }
    /* stop the test by turning off ram_test */
    cli_out("Stopping Test.\n");
    for (ci = ci_start; ci < ci_end; ci++) {
        SOC_IF_ERROR_RETURN(READ_CI_DDR_TESTr(unit, ci, &uData));
        soc_reg_field_set(unit, CI_DDR_TESTr, &uData, RAM_TESTf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_TESTr(unit, ci, uData));
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "DDR Mem Tests Done\n")));
    sal_free(ddr_test_params[unit]);
    ddr_test_params[unit] = NULL;
    return 0;
}

#if defined(BCM_KATANA2_SUPPORT)
int _ddr_bist_configure(int unit, int ci, ddr_bist_test_run_info_t *info, uint32 *bist_pattern) {
    uint32 reg_val;
    int i;
    const static soc_reg_t bist_pattern_words_reg[DRAM_BIST_NOF_PATTERNS] = {BISTPATTERNWORD0r, BISTPATTERNWORD1r, BISTPATTERNWORD2r,
                    BISTPATTERNWORD3r, BISTPATTERNWORD4r, BISTPATTERNWORD5r, BISTPATTERNWORD6r, BISTPATTERNWORD7r};

    const static soc_field_t bist_pattern_words_field[DRAM_BIST_NOF_PATTERNS] = {BISTPATTERN0f, BISTPATTERN1f, BISTPATTERN2f,
                       BISTPATTERN3f, BISTPATTERN4f, BISTPATTERN5f, BISTPATTERN6f, BISTPATTERN7f};

    SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit,ci,&reg_val));
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, WRITEWEIGHTf, info->write_weight);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, READWEIGHTf, info->read_weight);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, PATTERNBITMODEf, 
                                (info->pattern_mode == DRAM_BIST_DATA_PATTERN_BIT_MODE) ? 1 : 0);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, TWOADDRMODEf,
                                (info->ddr_bist_flags & DDR_BIST_TWO_ADDRESS_MODE) ? 1 : 0);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, PRBSMODEf,
                                (info->pattern_mode == DRAM_BIST_DATA_PATTERN_RANDOM_PRBS) ? 1 : 0);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, CONSADDR8BANKSf,
                                (info->ddr_bist_flags & DDR_BIST_CONS_ADDR_8_BANKS) ? 1 : 0);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, ADDRESSSHIFTMODEf,
                                (info->ddr_bist_flags & DDR_BIST_ADDRESS_SHIFT_MODE) ? 1 : 0);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, DATASHIFTMODEf,
                                (info->pattern_mode == DRAM_BIST_DATA_PATTERN_SHIFT_MODE) ? 1 : 0);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, DATAADDRMODEf,
                                (info->pattern_mode == DRAM_BIST_DATA_PATTERN_ADDR_MODE) ? 1 : 0);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, INDWRRDADDRMODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit,ci,reg_val));

    SOC_IF_ERROR_RETURN(READ_BISTNUMBEROFACTIONSr(unit,ci,&reg_val));
    soc_reg_field_set(unit, BISTNUMBEROFACTIONSr, &reg_val, BISTNUMACTIONSf,
                                        (info->ddr_bist_flags & DDR_BIST_INFINITE) ? 0 : info->bist_num_actions);
    SOC_IF_ERROR_RETURN(WRITE_BISTNUMBEROFACTIONSr(unit,ci,reg_val));

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTSTARTADDRESSr, ci, 0, &reg_val));
    soc_reg_field_set(unit, BISTSTARTADDRESSr, &reg_val, BISTSTARTADDRESSf,
                                info->bist_start_address);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, BISTSTARTADDRESSr, ci, 0, reg_val));

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTENDADDRESSr, ci, 0, &reg_val));
    soc_reg_field_set(unit, BISTENDADDRESSr, &reg_val, BISTENDADDRESSf,
                                info->bist_end_address);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, BISTENDADDRESSr, ci, 0, reg_val));

    for(i=0; i < DRAM_BIST_NOF_PATTERNS; ++i) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, bist_pattern_words_reg[i], ci, 0, &reg_val));
        soc_reg_field_set(unit, bist_pattern_words_reg[i], &reg_val, bist_pattern_words_field[i], bist_pattern[i]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, bist_pattern_words_reg[i], ci, 0, reg_val));
    }

    return BCM_E_NONE;

}

int _ddr_bist_action(int unit, int ci, uint8  is_infinite, uint8  stop) {
  uint32 reg_val,to_usec;
  int done=0;
  soc_timeout_t to;

  if (stop == FALSE) {
      SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit,ci,&reg_val));
      soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, BISTENf, 0);
      SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit,ci,reg_val));
      SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit,ci,&reg_val));
      soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, BISTENf, 1);
      SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit,ci,reg_val));
  }

  if (is_infinite == FALSE && stop == FALSE) {  
    to_usec = SAL_BOOT_QUICKTURN ? 5000000 : 200000;

    soc_timeout_init(&to, to_usec, 0);
    while (!soc_timeout_check(&to)) {

        READ_BISTSTATUSESr(unit, ci, &reg_val);
        done = soc_reg_field_get(unit, BISTSTATUSESr,
                                   reg_val, BISTFINISHEDf);
        if (done) {
            break;
        }
        sal_usleep(1000);
    }
    if (!done) {
        return BCM_E_TIMEOUT;
    }
  }

  if (is_infinite == FALSE || stop == TRUE) {
      SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit,ci,&reg_val));
      soc_reg_field_set(unit, BISTCONFIGURATIONSr, &reg_val, BISTENf, 0);
      SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit,ci,reg_val));
  }

  return BCM_E_NONE;
}

int _ddr_bist_test_start(int unit, int ci, ddr_bist_test_run_info_t *info) {
    int i;
    uint32 pattern_lcl[DRAM_BIST_NOF_PATTERNS];

    if(info->pattern_mode == DRAM_BIST_DATA_PATTERN_DIFF) {
        for(i=0; i <  DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = 0x55555555;
        }
    } else if(info->pattern_mode == DRAM_BIST_DATA_PATTERN_ONE) {
        for(i=0; i <  DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = 0xffffffff;
        }
    } else if(info->pattern_mode == DRAM_BIST_DATA_PATTERN_ZERO) {
        for(i=0; i <  DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = 0x00000000;
        }
    } else {
        for(i=0; i <  DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = info->pattern[i];
        }
    }

    if((info->ddr_bist_flags & (DDR_BIST_STOP | DDR_BIST_GET_DATA)) == 0) {
         /* If one of this flags in set - don't configure */
        if (_ddr_bist_configure(unit, ci, info, pattern_lcl) < 0) {
            return BCM_E_FAIL;
        }
     }
     if((info->ddr_bist_flags & DDR_BIST_GET_DATA) == 0) {
         if( _ddr_bist_action(unit, ci,
                 (info->ddr_bist_flags & DDR_BIST_INFINITE) ? TRUE : FALSE,
                 (info->ddr_bist_flags & DDR_BIST_STOP) ? TRUE : FALSE  ) < 0) {
             return BCM_E_FAIL;
         }
    }

    return BCM_E_NONE;
}

#endif /* BCM_KATANA2_SUPPORT */
#endif /* BCM_DDR3_SUPPORT */

#if defined(BCM_DDR3_SUPPORT) || defined(BCM_PETRA_SUPPORT)

int
ddr_bist_test_init(int unit, args_t *a, void **p)
{
    ddr_test_params_t *dt;
    parse_table_t pt;
    char* dram_ndx_srt=NULL;
    int rv = BCM_E_UNAVAIL;
    int i;
    uint32 use_default_test, cons_addr_8_banks, address_shift_mode;
    uint32 infinite, all_adress, stop, get_data, two_addr_mode;

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit) || SOC_IS_DPP_DRC_COMBO28(unit)) {
        return arad_bist_test_init(unit, a, p);
    }
#endif

    dt = ddr_test_params[unit];
    if (dt == NULL) {
        dt = sal_alloc(sizeof(ddr_test_params_t), "ddrtest");
        if (dt == NULL) {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "%s: cannot allocate memory test data\n"), ARG_CMD(a)));
            return -1;
        }
        sal_memset(dt, 0, sizeof(ddr_test_params_t));
        ddr_test_params[unit] = dt;
    }
    
        if(ARG_CNT(a) == 0 ){
            use_default_test = 1;
        } else {
            use_default_test = 0;
        }
    
        parse_table_init(unit, &pt);
    
        parse_table_add(&pt, "DramNdx", PQ_STRING, (void *) (0),
                &(dram_ndx_srt), NULL);
        parse_table_add(&pt, "WriteWeight", PQ_INT, (void *) 0,
                &(dt->run_info.write_weight), NULL);
        parse_table_add(&pt, "ReadWeight", PQ_INT, (void*) 0,
                &(dt->run_info.read_weight), NULL);
        parse_table_add(&pt, "BistNumActions", PQ_INT, (void *) 1,
                &(dt->run_info.bist_num_actions), NULL);
        parse_table_add(&pt, "StartAddr", PQ_INT, (void *) 0,
                &(dt->run_info.bist_start_address), NULL);
        parse_table_add(&pt, "EndAddr", PQ_INT, (void *) (0),
                &(dt->run_info.bist_end_address), NULL);
        parse_table_add(&pt, "PatternMode", PQ_INT, (void *) (0),
                &(dt->run_info.pattern_mode), NULL);
        parse_table_add(&pt, "Pattern0", PQ_INT, (void *) (0),
                &(dt->run_info.pattern[0]), NULL);
        parse_table_add(&pt, "Pattern1", PQ_INT, (void *) (0),
                &(dt->run_info.pattern[1]), NULL);
        parse_table_add(&pt, "Pattern2", PQ_INT, (void *) (0),
                &(dt->run_info.pattern[2]), NULL);
        parse_table_add(&pt, "Pattern3", PQ_INT, (void *) (0),
                &(dt->run_info.pattern[3]), NULL);
        parse_table_add(&pt, "Pattern4", PQ_INT, (void *) (0),
                &(dt->run_info.pattern[4]), NULL);
        parse_table_add(&pt, "Pattern5", PQ_INT, (void *) (0),
                &(dt->run_info.pattern[5]), NULL);
        parse_table_add(&pt, "Pattern6", PQ_INT, (void *) (0),
                &(dt->run_info.pattern[6]), NULL);
        parse_table_add(&pt, "Pattern7", PQ_INT, (void *) (0),
                &(dt->run_info.pattern[7]), NULL);
        parse_table_add(&pt, "ConsAddr8Banks", PQ_INT, (void *) (1),
                &cons_addr_8_banks, NULL);
        parse_table_add(&pt, "AddressShiftMode", PQ_INT, (void *) (0),
                &address_shift_mode, NULL);
        parse_table_add(&pt, "Infinite", PQ_INT, (void *) (0),
                &infinite, NULL);
        parse_table_add(&pt, "AllAdress", PQ_INT, (void *) (0),
                &all_adress, NULL);
        parse_table_add(&pt, "Stop", PQ_INT, (void *) (0),
                &stop, NULL);
        parse_table_add(&pt, "GetData", PQ_INT, (void *) (0),
                &get_data, NULL);
        parse_table_add(&pt, "TwoAddrMode", PQ_INT, (void *) (0),
                &two_addr_mode, NULL);
    
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            goto done;
        }
    
        if (ARG_CNT(a) != 0) {
            cli_out("%s: extra options starting with \"%s\"\n",
                    ARG_CMD(a), ARG_CUR(a));
            goto done;
        }
    
        if( use_default_test != 0) {
            all_adress = 1;
        }
    
        dt->unit = unit;
        dt->run_info.ddr_bist_flags = 0;
        if(cons_addr_8_banks) {
            dt->run_info.ddr_bist_flags |= DDR_BIST_CONS_ADDR_8_BANKS;
        }
        if(address_shift_mode) {
            dt->run_info.ddr_bist_flags |= DDR_BIST_ADDRESS_SHIFT_MODE;
        }
        if(infinite) {
            dt->run_info.ddr_bist_flags |= DDR_BIST_INFINITE;
        }
        if(all_adress) {
            dt->run_info.ddr_bist_flags |= DDR_BIST_ALL_ADDRESS;
        }
        if(stop) {
            dt->run_info.ddr_bist_flags |= DDR_BIST_STOP;
        }
        if(get_data) {
            dt->run_info.ddr_bist_flags |= DDR_BIST_GET_DATA;
        }
        if(two_addr_mode) {
            dt->run_info.ddr_bist_flags |= DDR_BIST_TWO_ADDRESS_MODE;
        }
    

        if (*dram_ndx_srt == '\0') {
            /* Test all memories by default */
            BCM_PBMP_CLEAR(dt->dram_ndx);
            for (i = 0; i < SOC_DDR3_NUM_MEMORIES(unit); i++) {
                BCM_PBMP_PORT_ADD(dt->dram_ndx, i);
            }
        } else if (parse_small_integers(unit, dram_ndx_srt, &dt->dram_ndx) < 0) {
            test_error(unit, "DramNdx parameter was not enter correctly\n");
            return -1;
        } 
        
        *p = dt;
        rv = 0;
    
        done:
           parse_arg_eq_done(&pt);
           return rv;
}


int
ddr_bist_test_done(int unit, void *p)
{
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit) || SOC_IS_DPP_DRC_COMBO28(unit)) {
        return arad_bist_test_done(unit, p);
    }
#endif

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "DDR Bist Tests Done\n")));
    sal_free(ddr_test_params[unit]);
    ddr_test_params[unit] = NULL;
    return BCM_E_NONE;
}

#define KT2_DDR_DATA_WIDTH         16
#define KT2_BIST_BITS_PER_ADDR     256

int ddr_bist_test(int unit, args_t *a, void *p)
{
#if defined(BCM_KATANA2_SUPPORT)
    ddr_test_params_t *dt = p;
    uint32 i, ci;
    uint32 reg_val, bist_err_occured, bist_full_err_cnt, bist_single_err_cnt, bist_global_err_cnt;
    uint32 max_bist_end_addr=0;
    uint32 max_bist_start_addr=0;
    uint32 uFailedData[8] = {0};
    uint32 uFailedAddr = 0;
    soc_pbmp_t valid_dram_pbmp;
#endif

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit) || SOC_IS_DPP_DRC_COMBO28(unit)) {
        return arad_bist_test(unit, a, p);
    }
#endif

#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit) ) {
        max_bist_end_addr = (1 << soc_reg_field_length(unit, 
                                  BISTENDADDRESSr, BISTENDADDRESSf)) - 1;
        max_bist_start_addr = (1 << soc_reg_field_length(unit, 
                                    BISTSTARTADDRESSr, BISTSTARTADDRESSf)) - 1;

        BCM_PBMP_CLEAR(valid_dram_pbmp);
        for (i = 0; i < SOC_DDR3_NUM_MEMORIES(unit); i++) {
            BCM_PBMP_PORT_ADD(valid_dram_pbmp, i);
        }

        SOC_PBMP_ITER(dt->dram_ndx, ci) {

            if (!SOC_PBMP_MEMBER(valid_dram_pbmp, ci))  {
                continue;
            }

            LOG_WARN(BSL_LS_APPL_TESTS,
                  (BSL_META_U(unit, "Testing DRAM %d\n"), ci));

            if (dt->run_info.ddr_bist_flags & DDR_BIST_ALL_ADDRESS) {
                /* Remove all other flags */
                dt->run_info.ddr_bist_flags = DDR_BIST_ALL_ADDRESS;
                dt->run_info.bist_start_address = 0x0;
                dt->run_info.bist_end_address =  SOC_DDR3_NUM_COLUMNS(unit) *
                                                 SOC_DDR3_NUM_ROWS(unit) *
                                                 SOC_DDR3_NUM_BANKS(unit) *
                                                 KT2_DDR_DATA_WIDTH /
                                                 KT2_BIST_BITS_PER_ADDR - 1;
                if (dt->run_info.bist_end_address > max_bist_end_addr ) {
                    LOG_ERROR(BSL_LS_APPL_TESTS,
                              (BSL_META_U(unit,
                                          "bist_end_address:%x exceeded max bist addr:%x"
                                          " so Resetting to max \n"),
                               dt->run_info.bist_end_address, max_bist_end_addr));
                    dt->run_info.bist_end_address = max_bist_end_addr;
                } 
                dt->run_info.bist_num_actions =  dt->run_info.bist_end_address + 1;

                dt->run_info.write_weight = 1;
                dt->run_info.read_weight = 0;
                dt->run_info.pattern_mode = DRAM_BIST_DATA_PATTERN_ADDR_MODE;

               if (_ddr_bist_test_start(dt->unit, ci, &(dt->run_info)) < 0) {
                    cli_out("Error: First _ddr_bist_test_start () "
                            "Failed:\n");
                    return BCM_E_FAIL;
                }
                
                dt->run_info.write_weight = 0;
                dt->run_info.read_weight = 1;
                if (_ddr_bist_test_start(dt->unit, ci, &(dt->run_info)) < 0) {
                     cli_out("Error: Second _ddr_bist_test_start () "
                             "Failed:\n");
                     return BCM_E_FAIL;
                 }
            } else { /* All Address */
                if (dt->run_info.bist_end_address > max_bist_end_addr ) {
                    LOG_ERROR(BSL_LS_APPL_TESTS,
                              (BSL_META_U(dt->unit,
                                          "bist_end_address:%x exceeded max bist addr:%x"
                                          " so Resetting to max \n"),
                               dt->run_info.bist_end_address, max_bist_end_addr));
                    dt->run_info.bist_end_address = max_bist_end_addr;
                } 
                if (dt->run_info.bist_start_address > max_bist_start_addr ) {
                    LOG_ERROR(BSL_LS_APPL_TESTS,
                              (BSL_META_U(dt->unit,
                                          "bist_start_address:%x exceeded max bist addr:%x"
                                          " so Resetting to max - 1 \n"),
                               dt->run_info.bist_start_address, 
                               max_bist_end_addr));
                    dt->run_info.bist_end_address = max_bist_start_addr - 1;
                } 
                if (_ddr_bist_test_start(dt->unit, ci, &(dt->run_info)) < 0) {
                     cli_out("Error: Second _ddr_bist_test_start () "
                             "Failed:\n");
                     return BCM_E_FAIL;
                 }
            }
#if defined(BCM_SABER2_SUPPORT)      
            if (SOC_IS_SABER2(unit)) {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit,
                            BISTERROROCCURREDr, ci, 0, &bist_err_occured));

                SOC_IF_ERROR_RETURN(soc_reg32_get(unit,
                            BISTFULLMASKERRORCOUNTERr, ci, 0,
                            &bist_full_err_cnt));

                SOC_IF_ERROR_RETURN(soc_reg32_get(unit,
                            BISTSINGLEBITMASKERRORCOUNTERr, ci, 0,
                            &bist_single_err_cnt));

                SOC_IF_ERROR_RETURN(soc_reg32_get(unit,
                            BISTGLOBALERRORCOUNTERr, ci, 0,
                            &bist_global_err_cnt));
 
            } else 
#endif            
            {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTERROROCCURREDr, ci, 0, &reg_val));
                bist_err_occured = soc_reg_field_get(unit, BISTERROROCCURREDr, reg_val, ERROCCURREDf);

                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTFULLMASKERRORCOUNTERr, ci, 0, &reg_val));
                bist_full_err_cnt = soc_reg_field_get(unit, BISTFULLMASKERRORCOUNTERr, reg_val, FULLERRCNTf);

                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTSINGLEBITMASKERRORCOUNTERr, ci, 0, &reg_val));
                bist_single_err_cnt = soc_reg_field_get(unit, BISTSINGLEBITMASKERRORCOUNTERr, reg_val, SINGLEERRCNTf);
                 
                 SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTGLOBALERRORCOUNTERr, ci, 0, &reg_val));
                bist_global_err_cnt = soc_reg_field_get(unit, BISTGLOBALERRORCOUNTERr, reg_val, GLOBALERRCNTf); 
            }

            if ((bist_err_occured != 0x0)  || (bist_full_err_cnt != 0x0)  || (bist_single_err_cnt != 0x0)  || (bist_global_err_cnt != 0x0)){
                cli_out("\nCI: %d \nResults:\n", ci);
                cli_out("Number of errors occured: 0x%x\n",
                        bist_err_occured);
                cli_out("Number of full errors: %d\n", bist_full_err_cnt);
                cli_out("Number of single errors: %d\n",
                        bist_single_err_cnt);
                cli_out("Number of global errors: %d\n",
                        bist_global_err_cnt);

                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTLASTADDRERRr,
                                                      ci, 0, &uFailedAddr));
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTLASTDATAERRWORD0r,
                                                   ci, 0, &uFailedData[0]));
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTLASTDATAERRWORD1r,
                                                   ci, 0, &uFailedData[1]));
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTLASTDATAERRWORD2r,
                                                   ci, 0, &uFailedData[2]));
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTLASTDATAERRWORD3r,
                                                   ci, 0, &uFailedData[3]));
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTLASTDATAERRWORD4r,
                                                   ci, 0, &uFailedData[4]));
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTLASTDATAERRWORD5r,
                                                   ci, 0, &uFailedData[5]));
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTLASTDATAERRWORD6r,
                                                   ci, 0, &uFailedData[6]));
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTLASTDATAERRWORD7r,
                                                   ci, 0, &uFailedData[7]));

                cli_out("CI%d Last Failing address = (0x%08x)\n",ci,
                                                          uFailedAddr);
                for (i = 0; i < 8; i++) {
                    if ( i == 4) {
                        cli_out("\n");
                    }
                    cli_out("failing_data_%d = (0x%08x)  ", i,
                                                    uFailedData[i]);
                }
                cli_out("\n");

                return BCM_E_FAIL;
            }
        }
        return BCM_E_NONE;
    } else
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        uint32 FailedAddr = 0;
        ydc_ddr_bist_info_t bist_info;
        ydc_ddr_bist_err_cnt_t error_count;
        LOG_WARN(BSL_LS_APPL_TESTS,
                (BSL_META_U(unit, "Testing DRAM\n")));

        SOC_IF_ERROR_RETURN(soc_iproc_ddr_init(unit));

        sal_memset(&bist_info, 0, sizeof(ydc_ddr_bist_info_t));

        if (SOC_IS_HELIX5(unit)) {
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 51510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            bist_info.addr_prbs_mode = 0;
        } else {
            bist_info.write_weight = 64;
            bist_info.read_weight = 64;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 65536;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x00FFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
        }

        SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_config_set(unit, 0, &bist_info));

        SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, 0, &error_count));

        if ((error_count.bist_err_occur != 0x0)  || (error_count.bist_full_err_cnt != 0x0) ||
            (error_count.bist_single_err_cnt != 0x0) || (error_count.bist_global_err_cnt != 0x0)) {
                cli_out("\nResults:\n");
                cli_out("Number of errors occurred: 0x%x\n",
                        error_count.bist_err_occur);
                cli_out("Number of full errors: %d\n", error_count.bist_full_err_cnt);
                cli_out("Number of single errors: %d\n",
                        error_count.bist_single_err_cnt);
                cli_out("Number of global errors: %d\n",
                        error_count.bist_global_err_cnt);

                SOC_IF_ERROR_RETURN(READ_DDR_BISTLASTADDRERRr(unit,&FailedAddr));
                cli_out("Last Failing address = (0x%08x)\n", FailedAddr);
                cli_out("\n");

                return BCM_E_FAIL;
        }
        return BCM_E_NONE;
    } else
#endif /* BCM_HELIX5_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
}

#endif /* defined(BCM_DDR3_SUPPORT) || defined(BCM_PETRA_SUPPORT) */
