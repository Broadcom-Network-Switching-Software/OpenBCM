/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FIFO DMA Test
 *
 * The FIFO DMA test fills up the L2_MOD_FIFO with random values through SCHAN.
 * Then, it reads the L2_MOD_FIFO through FIFO DMA to an external memory. Data
 * integrity check is done by comparing the source and destination memories.
 * FIFO DMA read performance in bps is calculated.
 */

#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <sal/core/time.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/debug.h>
#include <soc/dma.h>
#include <soc/cmic.h>
#include <soc/l2x.h>
#include "testlist.h"

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT) || \
    defined(BCM_CMICX_SUPPORT)
#include <soc/cmicm.h>
#endif

#define FIFODMA_PARAM_TEST_TYPE_DEFAULT 1
#define FIFODMA_PARAM_RATE_CALC_INT_DEFAULT 10
#define FIFODMA_PARAM_VCHAN_DEFAULT 0
#define FIFODMA_PARAM_DEBUG_DEFAULT 0

#define FIFODMA_TIMEOUT_DEFAULT 10

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CMICM_SUPPORT)

typedef struct fifodma_data_s {
    unsigned int param_test_type;
    unsigned int param_rate_calc_interval;
    unsigned int param_vchan;
    unsigned int param_debug;
    unsigned int fifo_depth;
    unsigned int fifo_width;
    unsigned int stop_perf_test;
    volatile sal_usecs_t td_acc;
    unsigned int* host_base;
    volatile int total_fifo_drains;
    uint32 cmc;
    uint32 ch;
    uint32 test_fail;
    uint32 bad_input;
} fifodma_data_t;

static fifodma_data_t fifodma_data;

const char fd_test_usage[] =
    "FIFO DMA test usage: [L2_MOD_FIFO]\n"
    " \n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pedantic compiler\n";
#else
    "VChan=<int>      -  Virtual CMC Channel; default 0\n"
    "                      TD3: Only 0 is valid for L2_MOD_FIFO\n"
    "                      TH : Only 0, 4 and 8 are valid for L2_MOD_FIFO\n"
    "RateCalcIn=<int> -  Duration for rate calculation; default is 10 s\n"
    "TestType=<1/0>   -  0: feature test; 1: performance test (default)\n"
    "Debug=<1/0>      -  Debug mode with verbose info\n"
    ;
#endif


static void
fifodma_soc_chk_fifodma_status(int unit, uint32 vchan, uint32 cmc, uint32 ch)
{
    uint32 rval;

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        {
            uint32 rval, hostmem_timeout, hostmem_overflow, done, error;

            soc_fifodma_masks_get(unit, &hostmem_timeout, &hostmem_overflow,
                                  &error, &done);
            soc_fifodma_status_get(unit, ch, &rval);
            if (rval & done) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "FIFO DMA engine terminated for cmc[%d]:ch[%d]\n"),
                    cmc, ch));
                if (rval & error) {
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "FIFO DMA engine encountered error: [0x%x]\n"), rval));
                }
            }
        }
    }
#endif
    if (soc_feature(unit, soc_feature_cmicm)) {
        rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
        if (soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval,
                              DONEf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "FIFO DMA engine terminated for cmc[%d]:ch[%d]\n"), cmc, ch));
            if (soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval,
                              ERRORf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "FIFO DMA engine encountered error: [0x%x]\n"), rval));
            }
        }
    }
}

static void
fifodma_soc_set_cmc_ch(int unit, uint32 vchan)
{
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        fifodma_data.cmc = vchan / CMICX_N_DMA_CHAN;
        fifodma_data.ch  = vchan % CMICX_N_DMA_CHAN;
    }
#endif
    if (soc_feature(unit, soc_feature_cmicm)) {
        fifodma_data.cmc = vchan / N_DMA_CHAN;
        fifodma_data.ch  = vchan % N_DMA_CHAN;
    }
}

static void
fifodma_soc_remap_addr(int unit, uint32 cmc)
{
    uint32 addr_remap;

    if (soc_feature(unit, soc_feature_cmicm) && cmc != 0) {
        addr_remap = soc_pci_read(unit,
                     CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(0));
        (void) soc_pci_write(unit,
                     CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc), addr_remap);
        addr_remap = soc_pci_read(unit,
                     CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(0));
        (void) soc_pci_write(unit,
                     CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc), addr_remap);
        addr_remap = soc_pci_read(unit,
                     CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(0));
        (void) soc_pci_write(unit,
                     CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc), addr_remap);
        if (SOC_REG_IS_VALID(unit, CMIC_CMC0_HOSTMEM_ADDR_REMAP_3r)) {
            addr_remap = soc_pci_read(unit,
                            CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(0));
            (void) soc_pci_write(unit,
                    CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(cmc), addr_remap);
        }
    }
}

static void
fifodma_parse_test_params(int unit, args_t* a)
{
    parse_table_t parse_table_fifodma_test;

    parse_table_init(unit, &parse_table_fifodma_test);
    parse_table_add(&parse_table_fifodma_test, "VChan", PQ_INT|PQ_DFL, 0,
                    &(fifodma_data.param_vchan), NULL);
    parse_table_add(&parse_table_fifodma_test, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &(fifodma_data.param_rate_calc_interval), NULL);
    parse_table_add(&parse_table_fifodma_test, "TestType", PQ_INT|PQ_DFL, 0,
                    &(fifodma_data.param_test_type), NULL);
    parse_table_add(&parse_table_fifodma_test, "Debug", PQ_INT|PQ_DFL, 0,
                    &(fifodma_data.param_debug), NULL);

    if (parse_arg_eq(a, &parse_table_fifodma_test) < 0 || ARG_CNT(a) != 0) {
        test_error(unit, "%s: Invalid option: %s\n",
                   ARG_CMD(a), ARG_CUR(a) ? ARG_CUR(a) : "*");
        fifodma_data.test_fail = 1;
        fifodma_data.bad_input = 1;
        test_msg("%s", fd_test_usage);
        parse_arg_eq_done(&parse_table_fifodma_test);
        return;
    }

    /* Note:  
     * The channel number for L2_MOD_FIFO is hard coded in Hardware
     *        -- TD3: Only 0 is valid for L2_MOD_FIFO
     *        -- TH : Only 0, 4 and 8 are valid for L2_MOD_FIFO
     */
    if (fifodma_data.param_vchan >= SOC_DCHAN_NUM(unit)) {
        fifodma_data.param_vchan = FIFODMA_PARAM_VCHAN_DEFAULT;
    }

    cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
    cli_out("\nVChan       = %0d", fifodma_data.param_vchan);
    cli_out("\nRateCalcInt = %0d", fifodma_data.param_rate_calc_interval);
    cli_out("\nTestType    = %0d", fifodma_data.param_test_type);
    cli_out("\nDebug       = %0d", fifodma_data.param_debug);
    cli_out("\n -----------------------------------------------------");
}

static void
fifodma_debug_print_fifo_info(int unit, char *msg)
{
    uint64 wr_ptr, rd_ptr;
    int num_entries;
    uint32 vchan = fifodma_data.param_vchan;

    if (fifodma_data.param_debug) {
        (void) soc_mem_fifo_dma_get_num_entries(unit, vchan, &num_entries);
        (void) soc_reg_get(unit, L2_MOD_FIFO_WR_PTRr, 0, 0, (&wr_ptr));
        (void) soc_reg_get(unit, L2_MOD_FIFO_RD_PTRr, 0, 0, (&rd_ptr));
        cli_out("\n[%s] valid_entries %0d", msg, num_entries);
        cli_out("\n[%s] wr_ptr %0d", msg, (uint32) COMPILER_64_LO(wr_ptr));
        cli_out("\n[%s] wr_ptr %0d", msg, (uint32) COMPILER_64_LO(rd_ptr));
    }
}

static void
fifodma_fill_l2_mod_fifo(int unit, unsigned int num_entries)
{
    uint64 wr_ptr, rd_ptr;
    unsigned int i, j;
    unsigned int *wr_data = NULL;
    uint64 zero_64;
    wr_data = (unsigned int *) soc_cm_salloc(unit,
                    fifodma_data.fifo_width, "wr_data");
    if(!wr_data) {
        cli_out("\nError in allocating memory for wr_data");
        return;
    }

    COMPILER_64_ZERO(zero_64);
    (void) soc_reg_set(unit, L2_MOD_FIFO_WR_PTRr, 0, 0, zero_64);
    (void) soc_reg_set(unit, L2_MOD_FIFO_RD_PTRr, 0, 0, zero_64);
    sal_sleep(2);
    (void) soc_reg_get(unit, L2_MOD_FIFO_WR_PTRr, 0, 0, (&wr_ptr));
    (void) soc_reg_get(unit, L2_MOD_FIFO_RD_PTRr, 0, 0, (&rd_ptr));
    fifodma_debug_print_fifo_info(unit, "before filling");

    for (i = 0; i < num_entries; i++) {
        for (j = 0; j < fifodma_data.fifo_width; j++) {
            /* coverity[dont_call : FALSE] */
            wr_data[j] = sal_rand();
        }
        (void) soc_mem_write(unit, L2_MOD_FIFOm, COPYNO_ALL, i, wr_data);
        COMPILER_64_ADD_32(wr_ptr, 0x1);
    }
    (void) soc_reg_set(unit, L2_MOD_FIFO_WR_PTRr, 0, 0, wr_ptr);
    fifodma_debug_print_fifo_info(unit, "after filling");

    soc_cm_sfree(unit, (void *)wr_data);
}

#define BSAND_BYTE_SWAP(x) ((((x) << 24)) | (((x) & 0x00ff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24)))

static unsigned int
fifodma_chk_data_integrity(int unit, unsigned int* base_addr,
                           unsigned int start_index, unsigned int num_entries)
{
    unsigned int i, j, match = 1;
    unsigned int fifo_width = fifodma_data.fifo_width;
    unsigned int *rd_data;
    volatile unsigned int* exp_data;

    exp_data = base_addr;
    rd_data = (unsigned int *) soc_cm_salloc(unit, fifo_width, "rd_data");
    if(!rd_data) {
        cli_out("\nError in allocating memory for rd_data");
        return 0;
    }

    for (i = 0; i < num_entries; i++) {
        (void) soc_mem_read(unit, L2_MOD_FIFOm, COPYNO_ALL, start_index + i,
                            rd_data);
        for (j = 0; j < fifo_width; j++) {
#if 0
            /* convert LE to BE */
            rd_data[j] = BSAND_BYTE_SWAP(rd_data[j]);
#endif
            if (exp_data[fifo_width * i + j] != rd_data[j]) {
                cli_out("\nIndex %0d mismatched: Exp: 0x%x, Act: 0x%x",
                        start_index + i,
                        exp_data[fifo_width * i + j],
                        rd_data[j]);
                match = 0;
            }
        }
    }
    soc_cm_sfree(unit, rd_data);

    return(match);
}

static void
fifodma_test_feature(int unit)
{
    int match = 0;
    unsigned int fifo_depth = fifodma_data.fifo_depth;
    uint32 vchan = fifodma_data.param_vchan;
    uint32 cmc = fifodma_data.cmc;
    uint32 ch = fifodma_data.ch;
    unsigned int* host_base = fifodma_data.host_base;

    cli_out("\nFill up L2_MOD_FIFO through SBUS");
    soc_mem_fifo_dma_stop(unit, vchan);
    fifodma_fill_l2_mod_fifo(unit, fifo_depth);
    fifodma_debug_print_fifo_info(unit, "1");

    /* start fifo dma */
    cli_out("\nfifo_dma_start");
    (void) soc_mem_fifo_dma_start(unit, vchan, L2_MOD_FIFOm, COPYNO_ALL,
                           fifo_depth, host_base);
    fifodma_debug_print_fifo_info(unit, "2");
    cli_out("\nSleep 1 second ");
    sal_sleep(1);
    fifodma_debug_print_fifo_info(unit, "3");

    cli_out("\nSet RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEM");
    (void) soc_mem_fifo_dma_set_entries_read(unit, vchan, fifo_depth);
    fifodma_debug_print_fifo_info(unit, "4");

    /* check data integrity */
    cli_out("\n\nCheck l2_mod_fifo data, addr_base %p, entry_num %d",
            host_base, fifo_depth);
    match = fifodma_chk_data_integrity(unit, host_base, 0x0, fifo_depth);

    /* check and clear error */
    fifodma_soc_chk_fifodma_status(unit, vchan, cmc, ch);

    /* stop fifo dma */
    cli_out("\nfifo_dma_stop");
    soc_mem_fifo_dma_stop(unit, vchan);

    if (match == 1) {
        cli_out("\n\n********** DATA INTEGRITY CHECK PASSED **********\n");
    } else {
        fifodma_data.test_fail = 1;
        test_error(unit, "\n*ERROR: **** DATA INTEGRITY CHECK FAILED ****\n");
    }
}

static void
fifodma_test_performance(void* up)
{
    /* int num_valid_entries = 0; */
    uint64 wr_ptr, rd_ptr;
    sal_usecs_t time0, time1;
    uint32 timeout;
    int unit = PTR_TO_INT(up);
    unsigned int fifo_depth = fifodma_data.fifo_depth;
    uint32 vchan = fifodma_data.param_vchan;
    uint32 cmc = fifodma_data.cmc;
    uint32 ch = fifodma_data.ch;
    unsigned int* host_base = fifodma_data.host_base;

    cli_out("\nFill up L2_MOD_FIFO through SBUS");
    soc_mem_fifo_dma_stop(unit, vchan);
    fifodma_fill_l2_mod_fifo(unit, fifo_depth);
    fifodma_debug_print_fifo_info(unit, "0");

    do {
        fifodma_debug_print_fifo_info(unit, "while_loop_1 a");

        /* kick off fifo dma */
        time0 = sal_time_usecs();
        (void) soc_mem_fifo_dma_start(unit, vchan, L2_MOD_FIFOm, COPYNO_ALL,
                               fifo_depth, host_base);

        (void) soc_reg_get(unit, L2_MOD_FIFO_WR_PTRr, 0, 0, (&wr_ptr));
        (void) soc_reg_get(unit, L2_MOD_FIFO_RD_PTRr, 0, 0, (&rd_ptr));
        time1 = sal_time_usecs();
        fifodma_debug_print_fifo_info(unit, "while_loop_1 b");

        /* check if fifo dma is done */
        timeout = FIFODMA_TIMEOUT_DEFAULT;
/*         while ((num_valid_entries != fifo_depth ||
                wr_ptr != rd_ptr) &&
               (timeout-- > 0)) { */
        while ((COMPILER_64_LO(wr_ptr) != COMPILER_64_LO(rd_ptr)) &&
               (timeout-- > 0)) {
            (void) soc_reg_get(unit, L2_MOD_FIFO_WR_PTRr, 0, 0, (&wr_ptr));
            (void) soc_reg_get(unit, L2_MOD_FIFO_RD_PTRr, 0, 0, (&rd_ptr));
            time1 = sal_time_usecs();
            if (timeout % 2 == 0) {
                fifodma_debug_print_fifo_info(unit, "while_loop_2");
            }
            sal_usleep(1000);
        }

        fifodma_debug_print_fifo_info(unit, "while_loop_1 c");
        if (fifodma_data.param_debug) {
            cli_out("\n------------------------------\n");
        }

        /* record transfered fifo_drans */
        fifodma_data.total_fifo_drains++;
        fifodma_data.td_acc = SAL_USECS_ADD(fifodma_data.td_acc,
                                            SAL_USECS_SUB(time1, time0));

        /* set RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEM */
        (void) soc_mem_fifo_dma_set_entries_read(unit, vchan, fifo_depth);

        /* check and clear error */
        fifodma_soc_chk_fifodma_status(unit, vchan, cmc, ch);

        /* stop fifo dma */
        soc_mem_fifo_dma_stop(unit, vchan);

        /* update wr_ptr for next iteration */
        (void) soc_reg_get(unit, L2_MOD_FIFO_WR_PTRr, 0, 0, &wr_ptr);
        COMPILER_64_ADD_32(wr_ptr, fifo_depth);
        (void) soc_reg_set(unit, L2_MOD_FIFO_WR_PTRr, 0, 0, wr_ptr);
    } while (fifodma_data.stop_perf_test == 0);

    sal_thread_exit(0);
}

static void
fifodma_measure_rate(unsigned int interval_in_seconds)
{
    uint64 drains_cnt_start, drains_cnt_end, byte_transferred;
    sal_usecs_t td_acc_start, td_acc_end, actual_drain_time;
    char byte_transferred_str[32];
    uint32 byte_per_usec, div1, div2;
    unsigned int fifo_depth = fifodma_data.fifo_depth;
    unsigned int fifo_width = fifodma_data.fifo_width;

    cli_out("\nMeasuring data transfer rate over an interval of %0d seconds",
            interval_in_seconds);

    /* actual transfer time */
    COMPILER_64_SET(drains_cnt_start, 0, fifodma_data.total_fifo_drains);
    td_acc_start = fifodma_data.td_acc;
    sal_sleep(interval_in_seconds);
    COMPILER_64_SET(drains_cnt_end, 0, fifodma_data.total_fifo_drains);
    td_acc_end = fifodma_data.td_acc;
    actual_drain_time = SAL_USECS_SUB(td_acc_end, td_acc_start);

    /* byte transfered */
    COMPILER_64_DELTA(byte_transferred, drains_cnt_start,
                                        drains_cnt_end);
    COMPILER_64_UMUL_32(byte_transferred, fifo_depth);
    COMPILER_64_UMUL_32(byte_transferred, fifo_width);
    format_uint64_decimal(byte_transferred_str, byte_transferred, 0);

    /* transfer rate, with accuracy .xxxx */
    div1 = (uint32) COMPILER_64_LO(byte_transferred);
    div2 = (uint32) actual_drain_time;
    byte_per_usec = _shr_div_exp10(div1, div2, 4);

    cli_out("\n*** Actual draining time    = %d us", actual_drain_time);
    cli_out("\n*** Total bytes transferred = %s Byte", byte_transferred_str);
    cli_out("\n*** Transfer rate           = %0d.%04d Byte/us",
            byte_per_usec/10000, byte_per_usec%10000);
}

int fifodma_test_init(int unit, args_t *a, void **pa)
{
    int i;

    cli_out("\nCalling fifodma_test_init\n");
    if (soc_feature(unit, soc_feature_arl_hashed)) {
        SOC_IF_ERROR_RETURN(soc_l2x_stop(unit));
    }

    /* initialize parameter/variable with default value */
    fifodma_data.param_test_type = FIFODMA_PARAM_TEST_TYPE_DEFAULT;
    fifodma_data.param_rate_calc_interval = FIFODMA_PARAM_RATE_CALC_INT_DEFAULT;
    fifodma_data.param_vchan = FIFODMA_PARAM_VCHAN_DEFAULT;
    fifodma_data.param_debug = FIFODMA_PARAM_DEBUG_DEFAULT;

    fifodma_data.fifo_depth = soc_mem_index_count(unit, L2_MOD_FIFOm);
    fifodma_data.fifo_width = soc_mem_entry_words(unit, L2_MOD_FIFOm);
    fifodma_data.test_fail = 0;
    fifodma_data.bad_input = 0;
    fifodma_data.stop_perf_test = 0;
    fifodma_data.total_fifo_drains = 0;
    fifodma_data.cmc = 0;
    fifodma_data.ch  = 0;

    /* parse parameters from cli */
    fifodma_parse_test_params(unit, a);
    if (fifodma_data.bad_input) {
        goto test_done;
    }

    /* initialize host memory */
    fifodma_data.host_base = (unsigned int*)sal_dma_alloc(
                        fifodma_data.fifo_depth * fifodma_data.fifo_width *
                        sizeof(unsigned int), "HOST_BASE");
    for (i = 0; i < (fifodma_data.fifo_depth * fifodma_data.fifo_width); i++) {
        fifodma_data.host_base[i] = 0x0;
    }

    /* initialize internal variables */
    fifodma_soc_set_cmc_ch(unit, fifodma_data.param_vchan);

    /* address remap for CMICM */
    fifodma_soc_remap_addr(unit, fifodma_data.cmc);

    cli_out("\n\nTEST FIFO INFO");
    cli_out("\nFIFO name : L2_MOD_FIFOm");
    cli_out("\nFIFO depth: %0d", fifodma_data.fifo_depth);
    cli_out("\nFIFO width: %0d", fifodma_data.fifo_width);
    cli_out("\n");

test_done:
    return 0;
}

int fifodma_test_cleanup(int unit, void *pa)
{
    if (fifodma_data.bad_input) {
        goto test_done;
    }
    cli_out("\nCalling fifodma_fifoma_test_cleanup\n");

test_done:
    sal_dma_free(fifodma_data.host_base);
    cli_out("\n");
    return 0;
}

int fifodma_test(int unit, args_t *a, void *pa)
{
    sal_thread_t pid_fifodma;
    int rv = 0;

    if (fifodma_data.bad_input) {
        goto test_done;
    }

    if (fifodma_data.param_test_type == FIFODMA_PARAM_TEST_TYPE_DEFAULT) {
        cli_out("\nDoing FIFO DMA performance test");
        pid_fifodma = sal_thread_create("FIFO DMA", 32 * 1024 * 1024, 200,
                                        fifodma_test_performance,
                                        INT_TO_PTR(unit));

        sal_usleep(1000000);
        fifodma_measure_rate(fifodma_data.param_rate_calc_interval);
        cli_out("\npid_fifodma = %p", pid_fifodma);
        fifodma_data.stop_perf_test = 1;
        sal_usleep(100000);
    } else {
        cli_out("\nDoing FIFO DMA feature test");
        fifodma_test_feature(unit);
    }

    rv = (fifodma_data.test_fail == 1)? BCM_E_FAIL : BCM_E_NONE;

test_done:
    return rv;
}

#endif /* BCM_ESW_SUPPORT && BCM_CMICM_SUPPORT */
