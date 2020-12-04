/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Random index memory test implemented via S-Channel table read/write.
 *
 * This test function is a variation on memrand for random memory
 * testing.  It was requested by Govind to do the following:
 *
 * Write every word in the memory each pass.
 * Write all then read all
 * Vary the pattern each write, for example, from all 1s to all 0s.
 * For different seeds, to order of writes to the memory should vary.
 *
 * Randomizing algorithm:
 *   Inputs: current value, original seed.
 */

#include <shared/bsl.h>

#include <sal/types.h>

#include <sal/appl/pci.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/progress.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/l2x.h>
#include <soc/memory.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif /* BCM_DFE_SUPPORT */
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif /* BCM_DNXF_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT */

#include "testlist.h"

#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
/*
 * Return true if a and b are relatively prime
 * Bad O(n) algorithm
 */
static uint32
relprime(uint32 a, uint32 b)
{
    uint32 i;
    uint32 min = a < b ? a : b;
    uint32 max = a < b ? b : a;

    if (max % min == 0) {
        return 0;
    }

    if ((a % 2 == 0) && (b % 2 == 0)) {
        return 0;
    }

    for (i = 3; i <= min / 2; i += 2) {
        if ((a % i == 0) && (b % i == 0)) {
            return 0;
        }
    }

    return 1;
}

/*
 * Return the first value greater than or equal to start that is
 * relatively prime to val.
 */
static uint32
get_rel_prime(uint32 val, uint32 start)
{
    while (!relprime(val, start)) {
        start++;
    }

    return start;
}


/*
 * Memory test status structure
 */

typedef struct rand_work_s {
    int was_debug_mode;
    int orig_enable;
    int saved_tcam_protect_write;
    int mem_scan;
    int scan_rate;
    sal_usecs_t scan_interval;
    int sram_scan;
    int sram_scan_rate;
    sal_usecs_t sram_scan_interval;
    int iters;                          /* Total iterations to perform */

    soc_mem_t mem;                      /* Memory to test */

    int copyno;                         /* copy to test (or COPYNO_ALL) */
    int copyno_total;                   /* Number of copies */

    int index_min;                      /* Lowest index in range to test */
    int index_max;                      /* Last index in range to test */
    int index_total;                    /* Size of index range (max-min+1-holes) */
    int array_index_start;              /* Lowest array index in range to test */
    int array_index_end;                /* Last array index in range to test */

    uint32 data_start;                  /* initial data value */
    uint32 data;                        /* current data value */
    uint32 seed;                        /* seed */
    int ecc_as_data;                    /* treat ecc field as regular field */
    int report_progress;
    uint32 addr_incr;                   /* for internal random */
    uint32 first_addr;
    uint32 cur_addr;
} rand_work_t;

static rand_work_t      *rand_work[SOC_MAX_NUM_DEVICES];


#if defined (BCM_ESW_SUPPORT) || defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT)
STATIC int
addr_rand_common_clear(int unit, soc_mem_t mem, int copyno)
{
#if defined(INCLUDE_MEM_SCAN) || defined(BCM_SRAM_SCAN_SUPPORT)
    rand_work_t *rw = rand_work[unit];
#endif
    int rv;
    
    SOC_MEM_TEST_SKIP_CACHE_SET(unit, 1);
    if ((rv = soc_mem_parity_control(unit, mem, copyno, FALSE)) < 0) {
        test_error(unit, "Could not disable parity warnings on memory %s\n",
                   SOC_MEM_UFNAME(unit, mem));
        return -1;
    }
#ifdef INCLUDE_MEM_SCAN
    if(!SOC_IS_DNX(unit)) {
        if ((rw->mem_scan = soc_mem_scan_running(unit, &rw->scan_rate,
                                                 &rw->scan_interval)) > 0) {
            if (soc_mem_scan_stop(unit)) {
                return -1;
            }
        }
    }
#endif /* INCLUDE_MEM_SCAN */
#ifdef BCM_SRAM_SCAN_SUPPORT
    if(!SOC_IS_DNX(unit)) {
        if ((rw->sram_scan = soc_sram_scan_running(unit, &rw->sram_scan_rate,
                                                 &rw->sram_scan_interval)) > 0) {
            if (soc_sram_scan_stop(unit)) {
                return -1;
            }
        }
    }
#endif /* BCM_SRAM_SCAN_SUPPORT */
    return 0;
}

STATIC int
addr_rand_common_restore(int unit, soc_mem_t mem, int copyno)
{
#if defined(INCLUDE_MEM_SCAN) || defined(BCM_SRAM_SCAN_SUPPORT)
    rand_work_t *rw = rand_work[unit];
#endif

    SOC_MEM_TEST_SKIP_CACHE_SET(unit, 0);
    if (soc_mem_parity_restore(unit, mem, copyno) < 0) {
        test_error(unit, "Could not enable parity warnings on memory %s\n",
                   SOC_MEM_UFNAME(unit, mem));
        return -1;
    }
#ifdef INCLUDE_MEM_SCAN
    if (rw->mem_scan) {
        if (soc_mem_scan_start(unit, rw->scan_rate, rw->scan_interval)) {
            return -1;
        }
    }
#endif /* INCLUDE_MEM_SCAN */
#ifdef BCM_SRAM_SCAN_SUPPORT
    if (rw->sram_scan) {
        if (soc_sram_scan_start(unit, rw->sram_scan_rate, rw->sram_scan_interval)) {
            return -1;
        }
    }
#endif /* BCM_SRAM_SCAN_SUPPORT */
    return 0;
}
#endif /* BCM_ESW_SUPPORT || BCM_PETRA_SUPPORT */



char tr52_test_usage[] = 
"TR52 (single memory test write repeatedly to all indices of the given memory as many times as requested) usage:\n"
" \n"
 #ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pedantic compiler\n";
 #else

  "M=<value>	Memory - The memory to be checked\n"
  "IS=<value>	IndexStart - Starting index\n"
  "IE=<value>	IndexEnd - Ending index\n"
  "AIS=<value>	ArrayIndexStart - Array starting index (in case of array memory)\n"
  "AIE=<value>	ArrayIndexEnd - Array ending index (in case of array memory)\n"
  "ITER=<value>	ITERations - number of iterations to repeat TR\n"
  "ID=<value>	InitialData - the basic data to be written in the test \n"
  "SEED=<value>	SEED - the seed to use in the random function\n"
  "EAD=<value>	EccAsData - treat ECC field as regular field\n"
  "RP=<value>	ReportProgress - \n"
  "Help=<1/0>                      -  Specifies if tr 7 help is on and exit or off (off by default)\n"
  "To enable printing values (write/read) of each memory proceed: debug +test +verb\n"
  "\n";
  #endif



int
addr_rand_init(int unit, args_t *a, void **p)
{
    rand_work_t         *rw;
    char                *mem_name = NULL;
    char                *idx_start_str, *idx_end_str;
    char                *array_idx_start_str, *array_idx_end_str;
    int                 blk;
    int                 rv = -1;
    uint32 help_status_default = 0;
    uint32 help_status=0;
    parse_table_t       pt;

    rw = rand_work[unit];
    if (rw == NULL) {
        rw = sal_alloc(sizeof(rand_work_t), "randaddr");
        if (rw == NULL) {
            cli_out("%s: cannot allocate memory test data\n", ARG_CMD(a));
            return -1;
        }
        sal_memset(rw, 0, sizeof(rand_work_t));
        rand_work[unit] = rw;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt,  "Memory",     PQ_STRING, "",
                    &mem_name, NULL);
    parse_table_add(&pt,  "IndexStart", PQ_STRING, (void *) "min",
                    &idx_start_str, NULL);
    parse_table_add(&pt,  "IndexEnd",   PQ_STRING, (void *) "max",
                    &idx_end_str, NULL);
    parse_table_add(&pt,  "ArrayIndexStart", PQ_STRING, (void *) "min", &array_idx_start_str, NULL);
    parse_table_add(&pt,  "ArrayIndexEnd",   PQ_STRING, (void *) "max", &array_idx_end_str, NULL);
    parse_table_add(&pt,  "ITERations", PQ_INT, (void *) 10,
                    &rw->iters, NULL);
    parse_table_add(&pt,  "SEED",       PQ_INT, (void *) 0xdecade,
                    &rw->seed, NULL );
    parse_table_add(&pt,  "EccAsData",  PQ_BOOL, (void *)1,
                    &rw->ecc_as_data, NULL );
    parse_table_add(&pt,  "InitialData", PQ_INT, (void *) 0xffffffff,
                    &rw->data_start, NULL );
    parse_table_add(&pt,  "ReportProgress", PQ_BOOL, (void *)1,
                    &rw->report_progress, NULL);
    parse_table_add(&pt, "Help",    PQ_DFL|PQ_INT, &help_status_default, &help_status,           NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        goto done;
    }

    if (ARG_CNT(a) != 0) {
        cli_out("%s: Invalid argument: \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        goto done;
    }
    if (mem_name == NULL || *mem_name == '\0') {
        cli_out("Memory not specified.\n Please use memory=<memory_name>\n");
        goto done;
    }
    if (help_status == 1) {
        rv=-1;
        cli_out("%s\n",tr52_test_usage);
        goto done;
    }

    if (mem_name == 0 || *mem_name == 0 ||
        parse_memory_name(unit,
                          &rw->mem,
                          mem_name,
                          &rw->copyno,
                          0) < 0) {
        test_error(unit,
                   "Missing or unknown memory name "
                   "(use listmem for list)\n");
        goto done;
    }

    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
        if (!soc_mem_is_valid(unit, rw->mem) ||
            soc_mem_is_readonly(unit, rw->mem) ||
            soc_mem_index_max(unit, rw->mem) < 3) {
            if (soc_feature(unit,soc_feature_esm_support)) {
               if ((rw->mem == L3_DEFIPm && SOC_MEM_IS_ENABLED(unit, L3_DEFIPm)) ||
                   (rw->mem == L3_DEFIP_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_ONLYm)) ||
                   (rw->mem == L3_DEFIP_DATA_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_DATA_ONLYm)) ||
                   (rw->mem == L3_DEFIP_HIT_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_DATA_ONLYm)) ||
                   (rw->mem == L3_DEFIP_PAIR_128_DATA_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_PAIR_128_DATA_ONLYm)) ||
                   (rw->mem == L3_DEFIP_PAIR_128_HIT_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_PAIR_128_HIT_ONLYm))) {
                   /* The above internal memories will not be supported if External TCAM is present.
                   These internal memories are set to 0 and hence can not run the test on these memories.*/
                   return BCM_E_UNAVAIL;
               } else {
                   test_error(unit,
                       "Cannot test memory %s with this command\n",
                          SOC_MEM_UFNAME(unit, rw->mem));
                   goto done;
                 }
            } else {
                   test_error(unit,
                           "Cannot test memory %s with this command\n",
                           SOC_MEM_UFNAME(unit, rw->mem));
                   goto done;
                  }
        }
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
    }
#if defined (BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit,soc_feature_no_vfp) &&
        ((rw->mem >= VFP_TCAMm && rw->mem <= VFP_TCAM_PIPE3m) ||
         (rw->mem >= VFP_POLICY_TABLEm && rw->mem <= VFP_POLICY_TABLE_PIPE3m))) {
        test_msg("VFP is not supported.Cannot test memory %s \n", SOC_MEM_UFNAME(unit, rw->mem));
        rv = SOC_E_UNAVAIL;
        goto done;
    }
#endif /*BCM_TOMAHAWK2_SUPPORT*/
#if defined(BCM_DFE_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if (SOC_IS_SAND(unit) && (soc_mem_is_writeonly(unit, rw->mem) || soc_mem_is_signal(unit, rw->mem))) {
        test_error(unit, "Memory %s is writeonly/signal \n",
                   SOC_MEM_UFNAME(unit, rw->mem));
        goto done;
    }
#endif
#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        switch(rw->mem) {
            case BRDC_FSRD_FSRD_WL_EXT_MEMm:
            case IQM_MEM_8000000m:
            case NBI_TBINS_MEMm:
            case NBI_RBINS_MEMm:
            case PORT_WC_UCMEM_DATAm:
            case ECI_MBU_MEMm:
            case EGQ_CBMm:
            case EGQ_RDMUCm:

               test_error(unit, "Memory %s is invalid/readonly/writeonly/signal \n",
                       SOC_MEM_UFNAME(unit, rw->mem));

               goto done;
            default:
                break;
        }
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        if (!SOC_MEM_IS_VALID(unit, rw->mem)
            || (soc_mem_flags(unit, rw->mem) & (SOC_MEM_FLAG_READONLY | SOC_MEM_FLAG_WRITEONLY | SOC_MEM_FLAG_SIGNAL)) != 0)
        {
            return 1;
        }
        switch (rw->mem)
        {
            case BRDC_QRH_FFLBm:
            case BRDC_QRH_MCLBTm:
            case BRDC_QRH_MCSFFm:
            case BRDC_QRH_MNOLm:
            case FSRD_FSRD_PROM_MEMm:
            case QRH_FFLBm:
            case QRH_MCLBTm:
            case QRH_MCSFFm:
            case QRH_MNOLm:
               test_error(unit, "Memory %s is invalid/readonly/writeonly/signal \n",
                       SOC_MEM_UFNAME(unit, rw->mem));
                goto done;       /* Skip these tables */
            default:
                break;
        }
    }
#endif
#if defined (BCM_DFE_SUPPORT)
    if(SOC_IS_DFE(unit)) 
    { 
        int rv;
        int is_filtered = 0;
        
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_test_mem_filter, (unit, rw->mem, &is_filtered));
        if (rv != SOC_E_NONE)
        {
            return rv;
        }
        if (is_filtered)
        {
               test_error(unit, "Memory %s is invalid/readonly/writeonly/signal \n",
                       SOC_MEM_UFNAME(unit,rw->mem));
               goto done;
        }
    }
#endif /*BCM_DFE_SUPPORT*/

    if (rw->copyno == COPYNO_ALL) {
        rw->copyno_total = 0;
        SOC_MEM_BLOCK_ITER(unit, rw->mem, blk) {
            rw->copyno_total += 1;
        }
    } else {
        rw->copyno_total = 1;
        if (!SOC_MEM_BLOCK_VALID(unit, rw->mem, rw->copyno)) {
            {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
                test_error(unit,
                           "Copy number out of range for memory %s\n",
                           SOC_MEM_UFNAME(unit, rw->mem));
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
            }
            goto done;
        }
    }

    rw->index_min = parse_memory_index(unit, rw->mem, idx_start_str);
    rw->index_max = parse_memory_index(unit, rw->mem, idx_end_str);
    rw->array_index_start = parse_memory_array_index(unit, rw->mem, array_idx_start_str);
    rw->array_index_end = parse_memory_array_index(unit, rw->mem, array_idx_end_str);
    if (rw->array_index_start > rw->array_index_end ) {
        unsigned temp_ai = rw->array_index_start;
        rw->array_index_start = rw->array_index_end;
        rw->array_index_end = temp_ai;
        cli_out("WARNING: switching start and end array indices to %u-%u\n", rw->array_index_start, rw->array_index_end);
    }
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FB_FX_HX(unit)) {
        if (rw->index_min < soc_mem_index_min(unit, rw->mem)) {
            rw->index_min = soc_mem_index_min(unit, rw->mem);
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
    rw->index_total = rw->index_max - rw->index_min + 1;

    if (rw->index_total <= 1 || rw->copyno_total < 1) {
        test_error(unit,
                   "Min copyno/index must be less than max copyno/index\n");
        goto done;
    }

    /* Place MMU in debug mode if testing a CBP memory */
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if (soc_mem_is_debug(unit, rw->mem) &&
        (rw->was_debug_mode = soc_mem_debug_set(unit, 0)) < 0) {
        test_error(unit, "Could not put MMU in debug mode\n");
        goto done;
    }
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */

    /* Disable non-atomic TCAM write handling */
    rw->saved_tcam_protect_write = SOC_CONTROL(unit)->tcam_protect_write;
    SOC_CONTROL(unit)->tcam_protect_write = FALSE;

#if defined (BCM_DFE_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    if(SOC_IS_DFE(unit) || SOC_IS_DNXF(unit)) {
        SOC_MEM_TEST_SKIP_CACHE_SET(unit, 1);
        /* Disable any parity control (herc) */
        if (soc_mem_parity_control(unit, rw->mem, rw->copyno, FALSE) < 0) {
            test_error(unit, "Could not disable parity warnings on memory %s\n",
                   SOC_MEM_UFNAME(unit, rw->mem));
            goto done;
        }
    } else 
#endif /* BCM_DFE_SUPPORT */
    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
        if (addr_rand_common_clear(unit, rw->mem, rw->copyno)) {
            goto done;
        }
#endif /* BCM_ESW_SUPPORT || BCM_PETRA_SUPPORT */
    }

    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
        if (soc_mem_cache_get(unit, rw->mem,
                              rw->copyno == COPYNO_ALL ?
                              MEM_BLOCK_ALL : rw->copyno)) {
            cli_out("WARNING: Caching is enabled on memory %s.%s\n",
                    SOC_MEM_UFNAME(unit, rw->mem),
                    SOC_BLOCK_NAME(unit, rw->copyno));
        }
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
    }

#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if (soc_mem_cpu_write_control(unit, rw->mem, rw->copyno, TRUE,
                                  &rw->orig_enable) < 0) {
        test_error(unit, "Could not enable exclusive cpu write on memory %s\n",
                   SOC_MEM_UFNAME(unit, rw->mem));
        goto done;
    }
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */

    /*
     * Turn off L2 task to keep it from going crazy if L2 memory is
     * being tested.
     */

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
        (void)soc_l2x_stop(unit);
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit) && (rw->mem == EGR_PERQ_XMT_COUNTERSm)) {
        rv = soc_reg_field32_modify(unit, EGR_EDB_HW_CONTROLr, REG_PORT_ANY,
                                    XGS_COUNTER_COMPAT_MODEf, 1);
       if (rv != SOC_E_NONE) {
           goto done;
       }
    }
    if (SOC_IS_SHADOW(unit) && SOC_BLOCK_IS_CMP(unit,
        SOC_MEM_BLOCK_MIN(unit, rw->mem), SOC_BLK_MS_ISEC)) {
        rv = soc_reg_field32_modify(unit, ISEC_MASTER_CTRLr, 1,
                                    XGS_COUNTER_COMPAT_MODEf, 1);
        if (rv != SOC_E_NONE) {
            return -1;
       }
        rv = soc_reg_field32_modify(unit, ISEC_MASTER_CTRLr, 5,
                                    XGS_COUNTER_COMPAT_MODEf, 1);
      if (rv != SOC_E_NONE) {
           return -1;
       }
    }
    if (SOC_IS_SHADOW(unit) && SOC_BLOCK_IS_CMP(unit,
        SOC_MEM_BLOCK_MIN(unit, rw->mem), SOC_BLK_MS_ESEC)) {
        rv = soc_reg_field32_modify(unit, ESEC_MASTER_CTRLr, 1,
                                    XGS_COUNTER_COMPAT_MODEf, 1);
        if (rv != SOC_E_NONE) {
            return -1;
        }
        rv = soc_reg_field32_modify(unit, ESEC_MASTER_CTRLr, 5,
                                    XGS_COUNTER_COMPAT_MODEf, 1);
        if (rv != SOC_E_NONE) {
           return -1;
        }
    }
#endif

    /*
     * Turn off FP.
     * Required when testing FP and METERING memories or tests will fail.
     * We don't care about the return value as the BCM layer may not have
     * been initialized at all.
     */
    (void)bcm_field_detach(unit);

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_esm_support)) {
        /* Some tables have dependency on the content of other table */
        switch (rw->mem) {
        case EXT_ACL360_TCAM_DATAm:
        case EXT_ACL360_TCAM_DATA_IPV6_SHORTm:
            rv = soc_mem_clear(unit, EXT_ACL360_TCAM_MASKm, 
                               MEM_BLOCK_ALL, TRUE);
            if (rv < 0) {
                test_error(unit, "Could not clear EXT_ACL360_TCAM_MASK\n");
                goto done;
            }
            break;
        case EXT_ACL432_TCAM_DATAm:
        case EXT_ACL432_TCAM_DATA_IPV6_LONGm:
        case EXT_ACL432_TCAM_DATA_L2_IPV4m:
        case EXT_ACL432_TCAM_DATA_L2_IPV6m:
            rv = soc_mem_clear(unit, EXT_ACL432_TCAM_MASKm, 
                               MEM_BLOCK_ALL, TRUE);
            if (rv < 0) {
                test_error(unit, "Could not clear EXT_ACL432_TCAM_MASK\n");
                goto done;
            }
            break;
        default:
            break;
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_etu_support)) {
        /* Some tables have dependency on the content of other table */
        switch (rw->mem) {
        case EXT_ACL480_TCAM_DATAm:
            if (SOC_FAILURE(soc_mem_clear(unit, EXT_ACL480_TCAM_MASKm,
                MEM_BLOCK_ALL, TRUE)))
            {
                test_error(unit, "Failed to clear EXT_ACL360_TCAM_MASKm\n");
                parse_arg_eq_done(&pt);
                return -1;
            }
            break;
        default:
            break;
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    /* Choose a value for increment that is
       relatively prime to total index. */
    rw->addr_incr = get_rel_prime(rw->index_total, rw->seed *
                                  rw->index_total);
    rw->first_addr = (rw->addr_incr % rw->index_total) + rw->index_min;
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Running with simple seed.  Incr: %d. First 0x%x.\n"),
              rw->addr_incr, rw->first_addr));

    *p = rw;

    rv = 0;

 done:
    parse_arg_eq_done(&pt);
    return rv;
}

#define NEXT_ADDR(cur) \
  ((((cur) - rw->index_min + rw->addr_incr) % rw->index_total) + rw->index_min)


int
addr_rand(int unit, args_t *a, void *p)
{
    rand_work_t         *rw = p;
    uint32              mask[SOC_MAX_MEM_WORDS];
    uint32              tcammask[SOC_MAX_MEM_WORDS];
    uint32              eccmask[SOC_MAX_MEM_WORDS];
    uint32              forcemask[SOC_MAX_MEM_WORDS];
    uint32              forcedata[SOC_MAX_MEM_WORDS];
    uint32              accum_tcammask, accum_forcemask;
    int                 iter, i, dw, word, copyno;
    unsigned            array_index;
    uint32              data[SOC_MAX_MEM_WORDS];
    char                status[160];
    int                 count;
    int                 skip_count;
    uint32              miscomp;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8  rev_id;
#endif

    COMPILER_REFERENCE(a);

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit) &&
        _soc_hr4_mem_skip(unit, rw->mem)) {
        return 0;
    }
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit) &&
        _soc_fb6_mem_skip(unit, rw->mem)) {
        return 0;
    }
#endif /* BCM_HURRICSUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id == BCM56982_DEVICE_ID) {
        if (soc_mem_test_skip(unit, rw->mem, 0)) {
            return 0;
        }
    }
#endif

    if (rw->index_total <= 0) {
        return -1;
    }   
    sal_memset(data, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    dw = soc_mem_entry_words(unit, rw->mem);
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) ||\
    defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_DNXF_SUPPORT) ||\
    defined(BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if(SOC_IS_DPP(unit) || SOC_IS_DFE(unit) || SOC_IS_GREYHOUND2(unit) || SOC_IS_DNXF(unit) || SOC_IS_DNX(unit)) {
        soc_mem_datamask_rw_get(unit, rw->mem, mask);
    } else 
#endif
    {
        soc_mem_datamask_get(unit, rw->mem, mask);
    }
    soc_mem_tcammask_get(unit, rw->mem, tcammask);
    soc_mem_eccmask_get(unit, rw->mem, eccmask);
    soc_mem_forcedata_get(unit, rw->mem, forcemask, forcedata);
    accum_tcammask = 0;
    for (i = 0; i < dw; i++) {
        accum_tcammask |= tcammask[i];
    }
    accum_forcemask = 0;
    for (i = 0; i < dw; i++) {
        accum_forcemask |= forcemask[i];
    }
    if (!rw->ecc_as_data) {
        for (i = 0; i < dw; i++) {
            mask[i] &= ~eccmask[i];
        }
    }
    soc_mem_datamask_memtest(unit, rw->mem, mask);

    progress_init(rw->copyno_total * rw->iters * rw->index_total * (rw->array_index_end - rw->array_index_start + 1) * 2, 3, 0);

    SOC_MEM_BLOCK_ITER(unit, rw->mem, copyno) {
        if (rw->copyno != COPYNO_ALL && rw->copyno != copyno) {
            continue;
        }

        for (array_index = rw->array_index_start; array_index <= rw->array_index_end; ++array_index) {

            if (rw->array_index_start != 0 || rw->array_index_end != rw->array_index_start) {
                {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
                    sal_sprintf(status,
                                "Running %d iterations on %s[%u].%s[%d-%d]",
                                rw->iters,
                                SOC_MEM_UFNAME(unit, rw->mem),
                                array_index,
                                SOC_BLOCK_NAME(unit, copyno),
                                rw->index_min, rw->index_max);
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
                }
            } else {
                {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
                    sal_sprintf(status,
                                "Running %d iterations on %s.%s[%d-%d]",
                                rw->iters,
                                SOC_MEM_UFNAME(unit, rw->mem),
                                SOC_BLOCK_NAME(unit, copyno),
                                rw->index_min, rw->index_max);
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
                }
            }

            progress_status(status);

            for (iter = 0; iter < rw->iters; iter++) {
                /* Set up addr and data and write to all memory addresses */

                rw->cur_addr = rw->first_addr;
                rw->data = rw->data_start;

                count = 0;
                skip_count = 0;
                do {
                    count++;
                    do {
                        if (soc_mem_test_skip(unit, rw->mem, rw->cur_addr)) {
                            rw->cur_addr = NEXT_ADDR(rw->cur_addr);
                            skip_count++;
                        } else {
                            break;
                        }
                    } while (rw->cur_addr != rw->first_addr);

                    if (rw->cur_addr == rw->first_addr) {
                        if ((count == 1 && skip_count == rw->index_total) || (count != 1)) {
                            break;
                        }
                    }
                    
                    if (soc_mem_entry_words(unit, rw->mem) % 2 == 0) {
                        rw->data = ~rw->data;
                    }
                    for (word = 0; word < dw; word++) {
                        data[word] = rw->data & mask[word];
                        rw->data = ~rw->data;
                    }
                    if (accum_tcammask) {
                        /* data read back has dependency on mask */
                        if ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ESM) ||
                           (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ETU)) {
                            for (word = 0; word < dw; word++) {
                                data[word] &= ~tcammask[word];
                            }
                        } else if (soc_feature(unit, soc_feature_xy_tcam)) {
                            for (word = 0; word < dw; word++) {
                                data[word] |= tcammask[word];
                            }
                        }
                    }
                    if (accum_forcemask) {
                        for (word = 0; word < dw; word++) {
                            data[word] &= ~forcemask[word];
                            data[word] |= forcedata[word];
                        }
                    }

#ifdef BCM_TOMAHAWK3_SUPPORT
                    if (rw->mem == CDMIB_MEMm && array_index == 12) {
                        data[14] = 0x00000000;
                        data[15] = 0x00000000;
                    }
#endif

                    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                        if (soc_mem_array_write(unit, rw->mem, array_index, copyno,
                                          rw->cur_addr, data) < 0) {
                            cli_out("Write ERROR: table %s.%s[%d] iteration %d\n",
                                    SOC_MEM_UFNAME(unit, rw->mem),
                                    SOC_BLOCK_NAME(unit, copyno),
                                    rw->cur_addr, iter);
                            if (rw->array_index_start != 0 || rw->array_index_end != rw->array_index_start) {
                                cli_out("At array index %u.\n", array_index);
                            }
                            goto break_all;
                        }
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
                    }

                    rw->cur_addr = NEXT_ADDR(rw->cur_addr);
                    if (rw->report_progress) {
                        progress_report(1);
                    }
                } while (rw->cur_addr != rw->first_addr);

                /* Reset addr and data and Verify values written */

                rw->cur_addr = rw->first_addr;
                rw->data = rw->data_start;

                miscomp = 0;

                if ((count + skip_count) != rw->index_total) {
                    cli_out("WARNING:  number of writes != index total\n");
                }

                count = 0;
                skip_count = 0;
                do {
                    count++;
                    do {
                        if (soc_mem_test_skip(unit, rw->mem, rw->cur_addr)) {
                            rw->cur_addr = NEXT_ADDR(rw->cur_addr);
                            skip_count++;
                        } else {
                            break;
                        }
                    } while (rw->cur_addr != rw->first_addr);

                    if (rw->cur_addr == rw->first_addr) {
                        if ((count == 1 && skip_count == rw->index_total) || (count != 1)) {
                            break;
                        }
                    }
                    if (soc_mem_entry_words(unit, rw->mem) % 2 == 0) {
                        rw->data = ~rw->data;
                    }
                    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
                        if (soc_mem_array_read(unit, rw->mem, array_index, copyno,
                                         rw->cur_addr, data) < 0) {
                            cli_out("Read ERROR: table %s.%s[%d] iteration %d\n",
                                    SOC_MEM_UFNAME(unit, rw->mem),
                                    SOC_BLOCK_NAME(unit, copyno),
                                    rw->cur_addr, iter);
                            if (rw->array_index_start != 0 || rw->array_index_end != rw->array_index_start) {
                                cli_out("At array index %u.\n", array_index);
                            }
                            goto break_all;
                        }
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
                    }

                    for (word = 0; word < dw; word++) {
                        uint32 comp_word = rw->data & mask[word];
                        if (accum_tcammask) {
                            /* data read back has dependency on mask */
                            if ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ESM)||
                               (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_ETU)) {
                                comp_word &= ~tcammask[word];
                            } else if (soc_feature(unit, soc_feature_xy_tcam)) {
                                comp_word |= tcammask[word];
                            }
                        }
                        if (accum_forcemask) {
                            comp_word &= ~forcemask[word];
                            comp_word |= forcedata[word];
                        }
#ifdef BCM_TOMAHAWK3_SUPPORT
                        if (rw->mem == CDMIB_MEMm) {
                            comp_word &= data[word];
                        }
#endif
                        if ((data[word] ^ comp_word) & mask[word]) {
                            soc_pci_analyzer_trigger(unit);

                            if (rw->array_index_start != 0 || rw->array_index_end != rw->array_index_start) {
                                {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
                                    cli_out("Compare ERROR: table %s[%u].%s iteration %d\n",
                                            SOC_MEM_UFNAME(unit, rw->mem), array_index,
                                            SOC_BLOCK_NAME(unit, copyno), iter+1);
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
                                }
                             } else {
                                {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
                                    cli_out("Compare ERROR: table %s.%s iteration %d\n",
                                            SOC_MEM_UFNAME(unit, rw->mem),
                                            SOC_BLOCK_NAME(unit, copyno), iter+1);
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
                                }
                            }
                            cli_out("Wrote word %d: 0x%08x.\n", word, comp_word);
                            cli_out("Read word %d:  0x%08x.\n", word, data[word]);
                            cli_out("Difference:    0x%08x.\n",
                                    comp_word ^ data[word]);
                            miscomp = 1;
                        }
                        rw->data = ~rw->data;
                    }

                    rw->cur_addr = NEXT_ADDR(rw->cur_addr);

                    if (rw->report_progress) {
                        progress_report(1);
                    }
                } while ((rw->cur_addr != rw->first_addr) && (!miscomp));

                if (miscomp) {
                    cli_out("Cycle aborted due to error\n");
                    test_error(unit, "\n");
                }
            }
        }
    }

 break_all:
    progress_done();

    return 0;
}

int
addr_rand_done(int unit, void *p)
{
    rand_work_t         *rw = p;
#ifdef BCM_SHADOW_SUPPORT
    int rv = SOC_E_NONE;
#endif

    if (rw) {
        /* Take MMU out of debug mode if testing a CBP memory */
#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNXF_SUPPORT)
        if (soc_mem_is_debug(unit,rw->mem) &&
            rw->was_debug_mode >= 0 &&
            soc_mem_debug_set(unit, rw->was_debug_mode) < 0) {
            test_error(unit, "Could not restore previous MMU debug state\n");
            return -1;
        }
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */

        /* Restore non-atomic TCAM write handling status */
        SOC_CONTROL(unit)->tcam_protect_write = rw->saved_tcam_protect_write;

#if defined (BCM_ESW_SUPPORT) || defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) || defined (BCM_DNXF_SUPPORT)
        if (soc_mem_cpu_write_control(unit, rw->mem, rw->copyno,
                                      rw->orig_enable, &rw->orig_enable) < 0) {
        test_error(unit, "Could not disable exclusive cpu write on memory "
                       "%s\n",
               SOC_MEM_UFNAME(unit, rw->mem));
        return -1;
    }
#endif /* BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */

#if defined (BCM_DFE_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    if(SOC_IS_DFE(unit) || SOC_IS_DNXF(unit)) {
        SOC_MEM_TEST_SKIP_CACHE_SET(unit, 0);
        if (soc_mem_parity_restore(unit, rw->mem, rw->copyno) < 0) {
            test_error(unit, "Could not enable parity warnings on memory %s\n",
               SOC_MEM_UFNAME(unit, rw->mem));
            return -1;
        }
    } else 
#endif /* BCM_DFE_SUPPORT */
    {    
#if defined (BCM_ESW_SUPPORT) || defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT)
        if (addr_rand_common_restore(unit, rw->mem, rw->copyno)) {
            return -1;
        }
#endif /* BCM_ESW_SUPPORT || BCM_PETRA_SUPPORT */
    }

#ifdef BCM_SHADOW_SUPPORT
        if (SOC_IS_SHADOW(unit) && (rw->mem == EGR_PERQ_XMT_COUNTERSm)) {
            rv = soc_reg_field32_modify(unit, EGR_EDB_HW_CONTROLr, REG_PORT_ANY,
                                        XGS_COUNTER_COMPAT_MODEf, 0);
           if (rv != SOC_E_NONE) {
               return -1;
           }
        }
        if (SOC_IS_SHADOW(unit) && SOC_BLOCK_IS_CMP(unit,
            SOC_MEM_BLOCK_MIN(unit, rw->mem), SOC_BLK_MS_ISEC)) {
            rv = soc_reg_field32_modify(unit, ISEC_MASTER_CTRLr, 1,
                                        XGS_COUNTER_COMPAT_MODEf, 0);
           if (rv != SOC_E_NONE) {
               return -1;
           }
            rv = soc_reg_field32_modify(unit, ISEC_MASTER_CTRLr, 5,
                                        XGS_COUNTER_COMPAT_MODEf, 0);
           if (rv != SOC_E_NONE) {
               return -1;
           }
        }
        if (SOC_IS_SHADOW(unit) && SOC_BLOCK_IS_CMP(unit,
            SOC_MEM_BLOCK_MIN(unit, rw->mem), SOC_BLK_MS_ESEC)) {
            rv = soc_reg_field32_modify(unit, ESEC_MASTER_CTRLr, 1,
                                        XGS_COUNTER_COMPAT_MODEf, 0);
           if (rv != SOC_E_NONE) {
               return -1;
           }
            rv = soc_reg_field32_modify(unit, ESEC_MASTER_CTRLr, 5,
                                        XGS_COUNTER_COMPAT_MODEf, 0);
           if (rv != SOC_E_NONE) {
               return -1;
           }
        }
#endif
    }

    return 0;
}

#endif /*  BCM_ESW_SUPPORT || BCM_DFE_SUPPORT || BCM_PETRA_SUPPORT */
