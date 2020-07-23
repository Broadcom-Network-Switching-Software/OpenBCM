/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CCM DMA Test
 *
 * The CCM DMA test transfers a source memory to a destination memory with one
 * or more CMC in parallel. The memories can be internal or external. First, the
 * test initializes the source memory with random values. Then, it kicks off one
 * or more CCM DMA transfer from the source memory to the destination memory.
 * Data integrity check is done by comparing the source and destination
 * memories. The CCM DMA performance in calculated by dividing the number of CCM
 * DMA performed x the CCM DMA transfer size by the duration of the performance
 * test.
 */

#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <sal/core/time.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/debug.h>
#include <soc/dma.h>
#include <soc/l2x.h>
#include <soc/uc.h>
#include "testlist.h"
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT)
#include <soc/cmicm.h>
#endif
#include <soc/ccmdma.h>
#define EXT_TO_EXT 0
#define EXT_TO_INT 1
#define INT_TO_EXT 2
#define INT_TO_INT 3
#define MIXED_MODE 4
#define FEATURE_TEST 0
#define PERFORMANCE_TEST 1

#define CMC_BITMAP_PARAM  0x1
#define CHAN_BITMAP_PARAM 0x1
#define XFER_TYPE_PARAM   0
#define ENTRY_COUNT_PARAM 1024
#define TEST_TYPE_PARAM 1
#define CCMDMA_RATE_CALC_INTERVAL_PARAM 10

#if defined(BCM_ESW_SUPPORT) && defined(BCM_CCMDMA_SUPPORT)
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)

sal_thread_t pid_ccmdma[SOC_CMCS_NUM_MAX];

typedef struct cdt_data_s {
    unsigned int cmc_bitmap;
    unsigned int chan_bitmap;
    unsigned int xfer_type;
    unsigned int entry_count;
    unsigned int test_type;
    unsigned int rate_calc_interval;
    unsigned int use_api;
    unsigned int sram_base;
    unsigned int sram_size;
    unsigned int* host0_base[SOC_CMCS_NUM_MAX];
    unsigned int* host1_base[SOC_CMCS_NUM_MAX];
    volatile unsigned int transfer_count[SOC_CMCS_NUM_MAX];
    unsigned int do_ccmdma_wrapper_cmc;
    unsigned int do_ccmdma_wrapper_chan;
    unsigned int max_cmc;
    unsigned int max_chan;
    unsigned int stream;
} cd_test_data_t;

static cd_test_data_t cd_test_data;

char cd_test_usage[] =
    "CCM DMA test usage:\n"
    " \n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pedantic compiler\n";
#else
    "CmcBitmap=<hex>  -  Specify which of the 3 available CMCs to use; default is 0x1\n"
    "ChanBitmap=<hex> -  Specify which of the chan to use; default is 0x1\n"
    "XferType=<0..4>  -  Data transfer scenarios, 0: external->external;\n"
    "                    1: external->internal; 2: internal->external;\n"
    "                    3: internal->internal; 4: mixed XferType 0, 2, and 3\n"
    "TestType=<1/0>   -  0: feature test; 1: performance test; default is 1\n"
    "EntryCount=<int> -  Number of entries per CCM DMA; default is 1024 DWords\n"
    "RateCalcInt=<int>-  Duration for rate calculation; default is 10 s\n"
    "UseAPI=<1/0>     -  Switch between SOC APIs and manual coding; default is 1\n"
    "SramBase=<hex>   -  SRAM base address; default is 0x1b000000 for Tomahawk and\n"
    "                    0x400000 for Trident2+\n"
    "SramSize=<hex>   -  SRAM Size\n"
    ;
#endif


static void
parse_ccmdma_test_params(int unit, args_t* a)
{
    parse_table_t parse_table_ccmdma_test;

    parse_table_init(unit, &parse_table_ccmdma_test);
    parse_table_add(&parse_table_ccmdma_test, "CmcBitmap",    PQ_HEX|PQ_DFL, 0,
                    &(cd_test_data.cmc_bitmap),               NULL);
    parse_table_add(&parse_table_ccmdma_test, "ChanBitmap",   PQ_HEX|PQ_DFL, 0,
                    &(cd_test_data.chan_bitmap),              NULL);
    parse_table_add(&parse_table_ccmdma_test, "XferType",     PQ_INT|PQ_DFL, 0,
                    &(cd_test_data.xfer_type),                NULL);
    parse_table_add(&parse_table_ccmdma_test, "EntryCount",   PQ_INT|PQ_DFL, 0,
                    &(cd_test_data.entry_count),              NULL);
    parse_table_add(&parse_table_ccmdma_test, "TestType",     PQ_INT|PQ_DFL, 0,
                    &(cd_test_data.test_type),                NULL);
    parse_table_add(&parse_table_ccmdma_test, "RateCalcInt",  PQ_INT|PQ_DFL, 0,
                    &(cd_test_data.rate_calc_interval), NULL);
    parse_table_add(&parse_table_ccmdma_test, "UseAPI",       PQ_BOOL|PQ_DFL, 0,
                    &(cd_test_data.use_api),                  NULL);
    parse_table_add(&parse_table_ccmdma_test, "SramBase",     PQ_HEX|PQ_DFL, 0,
                    &(cd_test_data.sram_base),                NULL);
    parse_table_add(&parse_table_ccmdma_test, "SramSize",     PQ_HEX|PQ_DFL, 0,
                    &(cd_test_data.sram_size),                NULL);
    if (parse_arg_eq(a, &parse_table_ccmdma_test) < 0 || ARG_CNT(a) != 0) {
        test_error(unit,
                   "%s: Invalid option: %s\n", ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        test_msg(cd_test_usage);
        parse_arg_eq_done(&parse_table_ccmdma_test);
    }

    if ((cd_test_data.xfer_type != EXT_TO_EXT) &&
        (SOC_PCI_CMCS_NUM(unit) < SOC_CMCS_NUM(unit))) {
            test_error(unit,
                   "Embedded processor enabled, unable to test internal SRAM\n");
    }

    if (cd_test_data.xfer_type == MIXED_MODE) {
        /* mixed mode runs all three CMCs in different transfer types */
        cd_test_data.cmc_bitmap = 7;
    }

    cli_out("\n ------------- PRINTING TEST PARAMS ------------------");
    cli_out("\nCmcBitmap   = 0x%0x",  cd_test_data.cmc_bitmap);
    cli_out("\nChanBitmap  = 0x%0x",  cd_test_data.chan_bitmap);
    cli_out("\nXferType    = %0d",    cd_test_data.xfer_type);
    cli_out("\nEntryCount  = %0d",    cd_test_data.entry_count);
    cli_out("\nTestType    = %0d",    cd_test_data.test_type);
    cli_out("\nRateCalcInt = %0d",    cd_test_data.rate_calc_interval);
    cli_out("\nUseAPI      = %0d",    cd_test_data.use_api);
    cli_out("\nSramBase    = 0x%0x",  cd_test_data.sram_base);
    cli_out("\nSramSize    = 0x%0x",  cd_test_data.sram_size);
    cli_out("\n -----------------------------------------------------");
}


static unsigned int
ccmdma_soc_read(int unit, unsigned int addr)
{
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        return (soc_cm_iproc_read(unit, addr));
    } else
#endif /* BCM_CMICX_SUPPORT */
    if (soc_feature(unit, soc_feature_cmicm)) {
        return (soc_pci_mcs_read(unit, addr));
    }

    return 0;
}


static void
ccmdma_soc_write(int unit, unsigned int addr, unsigned int data)
{
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_cm_iproc_write(unit, addr, data);
    } else
#endif /* BCM_CMICX_SUPPORT */
    if (soc_feature(unit, soc_feature_cmicm)) {
        soc_pci_mcs_write(unit, addr, data);
    }
}

static void
clear_ext_memory(unsigned char* start_addr, unsigned int num_bytes)
{
    unsigned int i;

    for (i = 0; i < num_bytes; i++) {
        start_addr[i] = 0x0;
    }
}


static void
clear_internal_memory(int unit, unsigned int start_addr, unsigned int num_words)
{
    unsigned int i;

    for (i = 0; i < num_words; i++)  {
        ccmdma_soc_write(unit, (start_addr + sizeof(unsigned int) * i), 0x0);
    }
}


static void
ccmdma_test_clean(int unit)
{
    soc_ccmdma_clean(unit, cd_test_data.max_cmc);
}


static unsigned int
xfer_type_per_cmc(int unit, unsigned int xfer_type, unsigned int cmc)
{
    if (xfer_type == MIXED_MODE) {
        /* If xfer_type is mixed mode, make CMC 0  EXT_to_EXT, CMC 1 INT_TO_EXT,
           and CMC 2 INT_TO_INT */
        switch (cmc) {
            case 0: return(EXT_TO_EXT); break;
            case 1: return(INT_TO_EXT); break;
            case 2: return(INT_TO_INT); break;
        }
    }
    return xfer_type;
}

static void
do_ccmdma_th(int unit, unsigned int cmc, unsigned int* host0_mem_start_addr,
          unsigned int* host1_mem_start_addr, unsigned int entry_count,
          unsigned int host0_is_internal, unsigned int host1_is_internal,
          volatile unsigned int* transfer_count)
{
    int rv;

    do {
            rv = soc_ccmdma_copy(unit, (sal_paddr_t*)host0_mem_start_addr,
                                       (sal_paddr_t*)host1_mem_start_addr,
                                       host0_is_internal, host1_is_internal,
                                       entry_count, 0, cmc);
            if(rv != SOC_E_NONE) {
                cli_out("\nWARNING: soc_ccmdma_copy returned with error %x\n", rv);
                return;
            }
            cd_test_data.transfer_count[cmc]++;
    } while (cd_test_data.stream != 0);
}


#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
static void
do_ccmdma_cmicx(int unit, unsigned int cmc, unsigned int* host0_mem_start_addr,
          unsigned int* host1_mem_start_addr, unsigned int entry_count,
          unsigned int host0_is_internal, unsigned int host1_is_internal,
          volatile unsigned int* transfer_count)
{
    int rv;
    unsigned int bitmap, chan, vchan = 0;

    bitmap = cd_test_data.chan_bitmap;

    do {
        for (chan = 0; chan < cd_test_data.max_chan; chan++) {
            if (bitmap & (0x1 << chan)) {
                vchan = (cd_test_data.max_chan * cmc) + chan;
                rv = soc_ccmdma_copy(unit, (sal_paddr_t*)host0_mem_start_addr,
                                           (sal_paddr_t*)host1_mem_start_addr,
                                 host0_is_internal, host1_is_internal,
                                 entry_count, 0, vchan);
                if(rv != SOC_E_NONE) {
                    cli_out("\nWARNING: soc_ccmdma_copy returned with error %x\n", rv);
                    return;
                }
                cd_test_data.transfer_count[cmc]++;
            }
        }
    } while (cd_test_data.stream != 0);
}
#endif


static void
do_ccmdma(int unit, unsigned int cmc, unsigned int* host0_mem_start_addr,
          unsigned int* host1_mem_start_addr, unsigned int entry_count,
          unsigned int host0_is_internal, unsigned int host1_is_internal,
          volatile unsigned int* transfer_count)
{
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit)) {
        do_ccmdma_cmicx(unit, cmc, host0_mem_start_addr, host1_mem_start_addr,
                  entry_count, host0_is_internal, host1_is_internal,
                  transfer_count);
    } else
#endif
    if (SOC_IS_TOMAHAWK(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
        do_ccmdma_th(unit, cmc, host0_mem_start_addr, host1_mem_start_addr,
                  entry_count, host0_is_internal, host1_is_internal,
                  transfer_count);
    }
}


static void
initialize_memory(int unit, unsigned int cmc_bitmap, unsigned int entry_count)
{
    unsigned int cmc;
    unsigned int i;
    unsigned int xfer_type;
#ifdef PTRS_ARE_64BITS
    unsigned int* int_mem_base = (unsigned int*)((uint64)cd_test_data.sram_base);
#else
    unsigned int* int_mem_base = (unsigned int*)(cd_test_data.sram_base);
#endif

    if ((cd_test_data.sram_base == 0) && (cd_test_data.xfer_type != EXT_TO_EXT)) {
        test_error(unit, "\nSramBase Unspecified");
    }

    for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++) {
        xfer_type = xfer_type_per_cmc(unit, cd_test_data.xfer_type, cmc);
        if ((cmc_bitmap >> cmc) % 2 != 0) {
            if ((xfer_type == EXT_TO_EXT) || (xfer_type == EXT_TO_INT)) {
                cd_test_data.host0_base[cmc] =
                                  sal_dma_alloc(cd_test_data.entry_count *
                         sizeof(unsigned int), "HOST0_BASE");
                clear_ext_memory((unsigned char*) cd_test_data.host0_base[cmc],
                                 (entry_count * sizeof(unsigned int)));
            } else {
                cd_test_data.host0_base[cmc] = int_mem_base;
                int_mem_base += entry_count;
#ifdef PTRS_ARE_64BITS
                clear_internal_memory(unit, (uint64)cd_test_data.host0_base[cmc],
                                      entry_count);
#else
                clear_internal_memory(unit, (uint32)cd_test_data.host0_base[cmc],
                                      entry_count);
#endif
            }

            if ((xfer_type == EXT_TO_EXT) || (xfer_type == INT_TO_EXT)) {
                cd_test_data.host1_base[cmc] =
                                  sal_dma_alloc(cd_test_data.entry_count *
                        sizeof(unsigned int), "HOST0_BASE");
                clear_ext_memory((unsigned char*) cd_test_data.host1_base[cmc],
                                 (entry_count * sizeof(unsigned int)));
            } else {
                cd_test_data.host1_base[cmc] = int_mem_base;
                int_mem_base += entry_count;
#ifdef PTRS_ARE_64BITS
                clear_internal_memory(unit, (uint64)cd_test_data.host1_base[cmc],
                                      entry_count);
#else
                clear_internal_memory(unit, (uint32)cd_test_data.host1_base[cmc],
                                      entry_count);
#endif
            }

            for (i = 0; i < entry_count; i++)  {
                if ((xfer_type == EXT_TO_EXT) || (xfer_type == EXT_TO_INT)) {
                    cd_test_data.host0_base[cmc][i] = sal_rand();
                }  else  {
#ifdef PTRS_ARE_64BITS
                    ccmdma_soc_write(unit, (uint64)cd_test_data.host0_base[cmc] +
                                      sizeof(unsigned int) * i, sal_rand());
#else
                    ccmdma_soc_write(unit, (uint32)cd_test_data.host0_base[cmc] +
                                      sizeof(unsigned int) * i, sal_rand());
#endif
                }
            }
        }
    }

    cli_out("\n----------------MEMORY INITIALIZED-------------------");
    for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++)  {
        cli_out("\nFor CMC %0d, host0_base = %p, host1_base = %p",
                cmc, cd_test_data.host0_base[cmc], cd_test_data.host1_base[cmc]);
    }
    cli_out("\n-----------------------------------------------------");
}

#define BSAND_BYTE_SWAP(x) ((((x) << 24)) | (((x) & 0x00ff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24)))

static unsigned int
check_data_integrity(int unit, unsigned int cmc_bitmap, unsigned int num_entries)
{
    unsigned int cmc;
    unsigned int i;
    unsigned int xfer_type;
    unsigned int src_word;
    unsigned int dest_word;
    unsigned int match = 1;
    int big_pio, big_packet, big_other;

    soc_endian_get(unit, &big_pio, &big_packet, &big_other);

    for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++)  {
        xfer_type = xfer_type_per_cmc(unit, cd_test_data.xfer_type, cmc);
        if (((cmc_bitmap >> cmc) % 2) != 0)  {
            for (i = 0; i < num_entries; i++)  {
                if ((xfer_type == EXT_TO_EXT) || (xfer_type == EXT_TO_INT)) {
                    src_word = cd_test_data.host0_base[cmc][i];
                }  else  {
#ifdef PTRS_ARE_64BITS
                    src_word = ccmdma_soc_read(unit,
                                   (uint64)cd_test_data.host0_base[cmc] +
                                                (sizeof(unsigned int) * i));
#else
                    src_word = ccmdma_soc_read(unit,
                                   (uint32)cd_test_data.host0_base[cmc] +
                                                (sizeof(unsigned int) * i));
#endif
                    if (big_other) {
                        src_word = BSAND_BYTE_SWAP(src_word);
                    }
                }

                if ((xfer_type == EXT_TO_EXT) || (xfer_type == INT_TO_EXT)) {
                    dest_word = cd_test_data.host1_base[cmc][i];
                }  else  {
#ifdef PTRS_ARE_64BITS
                    dest_word = ccmdma_soc_read(unit,
                                   (uint64)cd_test_data.host1_base[cmc] +
                                                 (sizeof(unsigned int) * i));
#else
                    dest_word = ccmdma_soc_read(unit,
                                   (uint32)cd_test_data.host1_base[cmc] +
                                                 (sizeof(unsigned int) * i));
#endif
                    if (big_other) {
                        dest_word = BSAND_BYTE_SWAP(dest_word);
                    }
                }

                if (src_word != dest_word) {
                    match = 0;
                    test_error(unit, "\nMismatch: CMC=%0d | host0_addr=%p, Src_word=0x%08x | host1_addr=%p, Dest_word=0x%08x",
                               cmc, (cd_test_data.host0_base[cmc] + i), src_word,
                               (cd_test_data.host1_base[cmc] + i), dest_word);
                    break;
                }
            }
        }
    }
    return(match);
}


static void
ccmdma_feature_test(int unit)
{
    unsigned int cmc, match;
    unsigned int xfer_type;
    unsigned int src_type;
    unsigned int dst_type;

    initialize_memory(unit, cd_test_data.cmc_bitmap, cd_test_data.entry_count);

    for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++)  {
        xfer_type = xfer_type_per_cmc(unit, cd_test_data.xfer_type, cmc);
        if((xfer_type == INT_TO_EXT) || (xfer_type == INT_TO_INT)) {
            src_type = 1;
        }
        else {
            src_type = 0;
        }

        if((xfer_type == EXT_TO_INT) || (xfer_type == INT_TO_INT)) {
            dst_type = 1;
        }
        else {
            dst_type = 0;
        }

        if ((cd_test_data.cmc_bitmap >> cmc) % 2 != 0)  {
            do_ccmdma(unit, cmc, cd_test_data.host0_base[cmc],
                      cd_test_data.host1_base[cmc],
                      cd_test_data.entry_count,
                      src_type,
                      dst_type,
                      &cd_test_data.transfer_count[cmc]);
        }
    }
    match = check_data_integrity(unit, cd_test_data.cmc_bitmap,
                                 cd_test_data.entry_count);

    if (match == 1)  {
        cli_out("\n************* DATA INTEGRITY CHECK PASSED **************");
    }  else  {
        test_error(unit, "\n************* DATA INTEGRITY CHECK FAILED **************");
    }
}


static void
do_ccmdma_wrapper(void* up)
{
    int unit = PTR_TO_INT(up);
    unsigned int xfer_type = xfer_type_per_cmc(unit, cd_test_data.xfer_type,
                                               cd_test_data.do_ccmdma_wrapper_cmc);

    do_ccmdma(unit, cd_test_data.do_ccmdma_wrapper_cmc,
              cd_test_data.host0_base[cd_test_data.do_ccmdma_wrapper_cmc],
              cd_test_data.host1_base[cd_test_data.do_ccmdma_wrapper_cmc],
              cd_test_data.entry_count,
              (unsigned int)((xfer_type == INT_TO_EXT) ||
                             (xfer_type == INT_TO_INT)),
              (unsigned int)((xfer_type == EXT_TO_INT) ||
                             (xfer_type == INT_TO_INT)),
              (&cd_test_data.transfer_count[cd_test_data.do_ccmdma_wrapper_cmc]));

    while (cd_test_data.stream != 0) {
        sal_usleep(100000);
    }

    sal_thread_exit(0);
}


static void
ccmdma_measure_rate(int unit, unsigned int entry_count,
                    unsigned int interval_in_seconds)
{
    unsigned int cmc;
    unsigned int transfer_count_start[SOC_CMCS_NUM_MAX];
    unsigned int transfer_count_end[SOC_CMCS_NUM_MAX];
    unsigned long byte_transferred;

    for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++) {
        transfer_count_start[cmc] = cd_test_data.transfer_count[cmc];
        cli_out("\ntransfer_count_start[%0d] = %u",
                cmc, transfer_count_start[cmc]);
    }

    cli_out("\nMeasuring data transfer rate over an interval of %0d seconds",
            interval_in_seconds);
    sal_sleep(interval_in_seconds);

    for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++) {
        transfer_count_end[cmc] = cd_test_data.transfer_count[cmc];
        cli_out("\ntransfer_count_end[%0d] = %u",
                cmc, transfer_count_end[cmc]);
    }

    for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++) {
        byte_transferred = (transfer_count_end[cmc] - transfer_count_start[cmc]) *
                            entry_count * 4;
        cli_out("\n***CCM DMA transfer rate on CMC %0d = %lu byte/s",
                cmc, (byte_transferred/interval_in_seconds));
    }
}

static void
ccmdma_performance_test(int unit)
{
    unsigned int cmc;
    char str[80];

    initialize_memory(unit, cd_test_data.cmc_bitmap, cd_test_data.entry_count);

    cd_test_data.stream = 1;

    for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++) {
        if ((cd_test_data.cmc_bitmap >> cmc) % 2 != 0) {
            sal_sprintf(str, "CCMDMA_CMC%0d", cmc);
            cd_test_data.do_ccmdma_wrapper_cmc = cmc;

            pid_ccmdma[cmc] = sal_thread_create(str, 32 * 1024 * 1024, 200,
                                                do_ccmdma_wrapper,
                                                INT_TO_PTR(unit));

            sal_usleep(100000);
            cli_out("\npid_ccmdma[%0d] = %p", cmc, pid_ccmdma[cmc]);
        }
    }

    sal_usleep(100000);

    ccmdma_measure_rate(unit, cd_test_data.entry_count,
                        cd_test_data.rate_calc_interval);

    cd_test_data.stream = 0;
    sal_usleep(100000);
}


int ccmdma_test_init(int unit, args_t *a, void **pa)
{
    unsigned int cmc;
    unsigned int xfer_type;
    uint32 addr_remap;

    cli_out("\nCalling ccmdma_test_init\n");

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        cli_out("\nSOC feature is CMICX\n");
    } else
#endif /* BCM_CMICX_SUPPORT */
    if (soc_feature(unit, soc_feature_cmicm)) {
        cli_out("\nSOC feature is CMICM\n");
    } else {
        cli_out("\n*WARN SOC feature is unknown\n");
    }

    if (soc_property_get(unit, spn_CCM_DMA_ENABLE, 0) == 0) {
        test_error(unit, "CCM DMA is disabled. Please add ccm_dma_enable=1 to config.bcm\n");
    }

    cd_test_data.cmc_bitmap         = CMC_BITMAP_PARAM;
    cd_test_data.chan_bitmap        = CHAN_BITMAP_PARAM;
    cd_test_data.xfer_type          = XFER_TYPE_PARAM;
    cd_test_data.entry_count        = ENTRY_COUNT_PARAM;
    cd_test_data.test_type          = TEST_TYPE_PARAM;
    cd_test_data.rate_calc_interval = CCMDMA_RATE_CALC_INTERVAL_PARAM;
    cd_test_data.use_api            = 1;

    (void) soc_uc_sram_extents(unit, &cd_test_data.sram_base, &cd_test_data.sram_size);

    cd_test_data.do_ccmdma_wrapper_cmc = 0;
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        cd_test_data.max_cmc = SOC_PCI_CMCS_NUM(unit);
        cd_test_data.max_chan = 2;
    } else
#endif /* BCM_CMICX_SUPPORT */
    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        cd_test_data.max_cmc = SOC_PCI_CMCS_NUM(unit);
        cd_test_data.max_chan = 1;
    } else {
        cd_test_data.max_cmc = 1;
        cd_test_data.max_chan = 1;
    }

    cd_test_data.stream = 0;
    parse_ccmdma_test_params(unit, a);
    ccmdma_test_clean(unit);

    if (soc_feature(unit, soc_feature_cmicm)
            || soc_feature(unit, soc_feature_cmicd_v2)
            || soc_feature(unit, soc_feature_cmicd_v3)) {
        for (cmc = 1; cmc < cd_test_data.max_cmc; cmc++) {
            addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(0));
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc), addr_remap);
            addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(0));
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc), addr_remap);
            addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(0));
            soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc), addr_remap);
        }
        for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++) {
            xfer_type = xfer_type_per_cmc(unit, cd_test_data.xfer_type, cmc);
            cd_test_data.transfer_count[cmc] = 0;
            if (SOC_REG_IS_VALID(unit, CMIC_CMC0_HOSTMEM_ADDR_REMAP_3r)) {
                if (xfer_type != EXT_TO_EXT) {
                    /* Set hostmem_addr_remap for internal SRAM */
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(cmc),
                              0x1);
                } else {
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(cmc),
                              0x1f);
                }
            } else {
                if (xfer_type != EXT_TO_EXT) {
                    /* Set hostmem_addr_remap for internal SRAM */
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc),
                              0x7bbc);
                } else {
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc),
                              0xffbbc);
                }
            }
        }
    } else {
        for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++) {
            xfer_type = xfer_type_per_cmc(unit, cd_test_data.xfer_type, cmc);
            cd_test_data.transfer_count[cmc] = 0;
        }
    }

    return 0;
}

int ccmdma_test_cleanup(int unit, void *pa)
{
    unsigned int cmc;
    unsigned int xfer_type;

    cli_out("\nCalling ccmdma_test_cleanup");
    ccmdma_test_clean(unit);
    for (cmc = 0; cmc < cd_test_data.max_cmc; cmc++) {
        if (soc_feature(unit, soc_feature_cmicm)
            || soc_feature(unit, soc_feature_cmicd_v2)
            || soc_feature(unit, soc_feature_cmicd_v3)) {
            /* Restore hostmem_addr_remap to default value */
            if (SOC_REG_IS_VALID(unit, CMIC_CMC0_HOSTMEM_ADDR_REMAP_3r)) {
                soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(cmc),
                              0x1f);
            } else {
                soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc),
                              0xffbbc);
            }
        }
        xfer_type = xfer_type_per_cmc(unit, cd_test_data.xfer_type, cmc);
        if ((xfer_type == EXT_TO_EXT) ||
            (xfer_type == EXT_TO_INT)) {
            sal_dma_free(cd_test_data.host0_base[cmc]);
        }
        if ((xfer_type == EXT_TO_EXT) ||
            (xfer_type == INT_TO_EXT)) {
            sal_dma_free(cd_test_data.host1_base[cmc]);
        }
        cd_test_data.host0_base[cmc] = 0;
        cd_test_data.host1_base[cmc] = 0;
    }

    cli_out("\n");
    return 0;
}


int ccmdma_test(int unit, args_t *a, void *pa)
{
    if (cd_test_data.test_type == PERFORMANCE_TEST) {
        ccmdma_performance_test(unit);
    } else {
        ccmdma_feature_test(unit);
    }
    return 0;
}
#endif /* BCM_CMICM_SUPPORT || BCM_CMICX_SUPPORT */
#endif /* BCM_ESW_SUPPORT && BCM_CCMDMA_SUPPORT */
