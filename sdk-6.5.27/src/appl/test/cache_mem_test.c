/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_MEM

/* we need this junk function only to avoid building error of pedantic compilation */
void ___junk_function_cache_mem_test(void){
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <shared/bsl.h>
#include <sal/types.h>
#include <sal/compiler.h>
#include <sal/core/libc.h>
#include <soc/defs.h>
#include <soc/mcm/memregs.h>
#include <soc/mem.h>
#include <soc/mcm/allenum.h>

#include <soc/shared/sat.h>
#include <soc/drv.h>
#include <appl/diag/system.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <bcm_int/dpp/counters.h>
#endif

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_mem.h>
#include <soc/dnxc/intr.h>

#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/auto_generated/common_enum.h>
#include <soc/access/access.h>
#include <soc/access/access_regmem_util.h>
#include <soc/access/access_sbusdma_internal.h>
#include <soc/access/access_sbusdma.h>
#include <soc/access/access_intr.h>
#include <soc/dnxc/dnxc_access_intr.h>
#endif
#endif

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/sand/shrextend/shrextend_debug.h>
#undef BSL_LOG_MODULE
#define BSL_LOG_MODULE BSL_LS_SOC_ACCESS
#endif

#ifdef BCM_SAND_SUPPORT
#include <soc/sand/sand_mem.h>
#include <bcm/error.h>
#include <bcm/link.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/auto_generated/dnx_link_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>
shr_error_e dnx_hbmc_temp_monitor_deinit(int unit);
#endif
#include "cache_mem_test.h"
#include "testlist.h"
#ifndef BCM_DNX_SUPPORT
/* Function Declaration */
int bcm_common_linkscan_enable_set(int unit, int us);
#endif

/* MACRO Definitions */
#define SOC_MEM_ENTRY_LENGTH_MAX   180
/* FUNCTION Definitions */
int cache_mem_test_read_and_compare(SOC_SAND_IN int unit, SOC_SAND_IN soc_mem_t mem, void* data);
uint32 cache_mem_test_generate_value(uint32 unit, soc_mem_t mem, uint32 array_index, uint32 index, cache_mem_test_write_value_pattern_t pattern);
void cache_mem_test_create_mask(SOC_SAND_IN  uint32 unit,SOC_SAND_IN soc_mem_t mem, SOC_SAND_IN size_t mask_len, SOC_SAND_OUT uint32* mask);
void do_cache_mem_test_print_usage(void);

uint32
cache_mem_test_generate_value(uint32 unit, soc_mem_t mem, uint32 array_index, uint32 index, cache_mem_test_write_value_pattern_t pattern)
{
    switch (pattern)
    {
        case    cache_mem_test_write_value_pattern_all_ones:
            return 0xFFFFFFFF;
        case    cache_mem_test_write_value_pattern_all_zeroes:
            return 0;
        case    cache_mem_test_write_value_pattern_incremental:
            return index;
        case    cache_mem_test_write_value_pattern_smart:
            return (unit + mem + array_index + index);
    }
    return 0;
}

#ifdef BCM_SAND_SUPPORT
static int cache_mem_dynamic_check(uint32 unit,soc_mem_t mem)
{
    switch (mem)
    {
        case KAPS_TCAM_HIT_INDICATIONm:
        case OAMP_CLS_FLEX_CRC_TCAMm:
        case OAMP_CLS_TRAP_CODE_TCAMm:
        case OAMP_PE_0_PROG_TCAMm:
        case OAMP_PE_1_PROG_TCAMm:
        case OAMP_RSP_TCAMm:
        case OAMP_RX_OAM_ID_TCAMm:
#if defined(BCM_PETRA_SUPPORT)
        case EDB_EEDB_TOP_BANKm:
        case EDB_EEDB_BANKm:
        case PPDB_B_LIF_TABLEm:
        case PPDB_B_LIF_TABLE_AC_2_EEIm:
        case PPDB_B_LIF_TABLE_AC_2_OUT_LIFm:
        case PPDB_B_LIF_TABLE_AC_MPm:
        case PPDB_B_LIF_TABLE_IP_TTm:
        case PPDB_B_LIF_TABLE_ISID_MPm:
        case PPDB_B_LIF_TABLE_ISID_P2Pm:
        case PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm:
        case PPDB_B_LIF_TABLE_LABEL_PWE_MPm:
        case PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm:
        case PPDB_B_LIF_TABLE_TRILLm:
        case KAPS_BUCKET_MAP_MEMORYm:
#endif
        case KAPS_BUCKET_MEMORYm:
            return 1;
    }
    return 0;
}

static int _cache_mem_test_blk_disable_skip(int unit, soc_mem_t mem)
{
    int rc = 0;
    int blk;
    for (blk = SOC_MEM_BLOCK_MIN(unit, mem); blk <= SOC_MEM_BLOCK_MAX(unit, mem); blk++)
    {
        if (SOC_INFO(unit).block_valid[blk])
        {
            rc++;
        }
    }
    if (rc > 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

static int cache_mem_cache_check_skip(int unit, soc_mem_t mem)
{
    int     rc = 0;
    if (SOC_IS_DNX(unit))
    {
        switch (mem)
        {
            
            case MDB_ARM_MEM_0m:
            
            case FSRD_FSRD_PROM_MEMm:
            {
                rc = 1;
                break;
            }

            case MDB_EEDB_ENTRY_BANKm:
            case ETPPB_ENC_3_PEMA_LOAD_SELECT_ARRAY_ENTRY_BY_VAR_INDEXm:
            case ETPPB_ENC_3_PEMA_LOAD_SELECT_CHUNK_LITERARYm:
            {
                if (SOC_IS_JERICHO2_B(unit))
                {
                    rc = 1;
                    break;
                }
            }
            case OFT_TMC_RD_FIFOS_DESCRIPTORSm:
            case ESB_ESB_QUEUE_MUX_0m:
            case ESB_ESB_QUEUE_MUX_1m:
            {
                if (SOC_IS_J2X(unit))
                {
                    rc = 1;
                    break;
                }
            }
            default:
            {
                break;
            }
        }

        if (_cache_mem_test_blk_disable_skip(unit, mem))
        {
            rc = 1;
        }
    }

    return rc;
}


int cache_memory_test_result_dump(int unit, tr_do_cache_mem_test_t* test_params)
{
    soc_mem_t   mem;

    /* list skip memory */
    cli_out("Force skipped memory:\n");
    for (mem=0; mem<NUM_SOC_MEM; mem++)
    {
        if (test_params->mem_test_result_flag[mem] & CACHE_MEM_TEST_FORCE_SKIP_FLAG)
        {
            cli_out("    %-8d%s\n", mem, SOC_MEM_NAME(unit, mem));
        }
    }

    /* list read failure memory */
    cli_out("\nRead failure memory:\n");
    for (mem=0; mem<NUM_SOC_MEM; mem++)
    {
        if (test_params->mem_test_result_flag[mem] & CACHE_MEM_TEST_READ_FAILUE_FLAG)
        {
            cli_out("    %-8d%s\n", mem, SOC_MEM_NAME(unit, mem));
        }
    }

    /* list compare failure memory */
    cli_out("\nCompare failure memory:\n");
    for (mem=0; mem<NUM_SOC_MEM; mem++)
    {
        if (test_params->mem_test_result_flag[mem] & CACHE_MEM_TEST_COMPARE_FAILUE_FLAG)
        {
            cli_out("    %-8d%-50s", mem, SOC_MEM_NAME(unit, mem));
            if (SOC_MEM_FIELD_VALID(unit, mem, PARITYf))
            {
                cli_out("PARITY\n");
            }
            else if (SOC_MEM_FIELD_VALID(unit, mem, ECCf))
            {
                cli_out("ECC\n");
            }
            else
            {
                cli_out("\n");
            }
        }
    }

    /* list broadcast compare memory */
    cli_out("\nBroadcast compare memory:\n");
    for (mem=0; mem<NUM_SOC_MEM; mem++)
    {
        if (test_params->mem_test_result_flag[mem] & CACHE_MEM_TEST_COMPARE_BCAST_FLAG)
        {
            cli_out("    %-8d%s\n", mem, SOC_MEM_NAME(unit, mem));
        }
    }

    return SOC_E_NONE;
}
#if defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
static void cache_memory_get_broadcast_info(int unit, soc_mem_t mem, soc_mem_t *mem_write, int *copyno)
{
    /* see soc_broadcastmem_2_cachemem */
    soc_mem_t bcast_mem = INVALIDm;
    int bcast_copyno = MEM_BLOCK_ALL;

    if (SOC_IS_DNXF(unit))
    {
        switch (mem)
        {
            case QRH_CUCTm:
                bcast_mem = BRDC_QRH_CUCTm;
                break;
            case QRH_DUCTm:
                bcast_mem = BRDC_QRH_DUCTm;
                break;
            case QRH_FFLBm:
                bcast_mem = BRDC_QRH_FFLBm;
                break;
            case QRH_MCLBTm:
                bcast_mem = BRDC_QRH_MCLBTm;
                break;
            case QRH_MCSFFm:
                bcast_mem = BRDC_QRH_MCSFFm;
                break;
            case QRH_MNOLm:
                bcast_mem = BRDC_QRH_MNOLm;
                break;
            default:
                bcast_mem = INVALIDm;
                break;
        }
    }
    else if (SOC_IS_DNX(unit))
    {
        bcast_copyno = soc_mem_broadcast_block_get(unit, mem);
        if (bcast_copyno == SOC_CORE_ALL) 
        {
            bcast_copyno = 0;
        }
        if (bcast_copyno)
        {
            bcast_mem = mem;
        }
    }

    *mem_write = bcast_mem;
    *copyno = bcast_copyno;
    return;
}

int cache_memory_test_broadcast_iter_callback(int unit, soc_mem_t mem, void* data)
{
    int         rv = SOC_E_NONE;
    soc_mem_t   bcast_mem;
    int         bcast_copyno;
    uint32      array_index, start_array_index, end_array_index;
    uint32      entry_index, start_entry_index, end_entry_index;
    uint32      ii, entry_dw;
    uint32      write_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32      mem_field_mask[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32      read_hw_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32      read_cache_value[SOC_MEM_ENTRY_LENGTH_MAX];
    tr_do_cache_mem_test_t *test_params = (tr_do_cache_mem_test_t *)data;
    
    if (!SOC_MEM_IS_VALID(unit, mem)) 
    {
        goto done;
    }
    if (SOC_MEM_IS_ALIAS(unit, mem)) 
    {
        goto done;
	}
    if(!soc_mem_is_cachable(unit, mem))
    {
        /* skip memory*/
        goto done;
    }
    if (!soc_mem_cache_get(unit, mem, MEM_BLOCK_ALL)) 
    {
        /* skip memory*/
        goto done;
    }
    cache_memory_get_broadcast_info(unit, mem, &bcast_mem, &bcast_copyno);
    if (bcast_mem == INVALIDm)
    {
        /* skip memory*/
        goto done;
    }

    test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_COMPARE_BCAST_FLAG;
    cli_out("CACHE_MEM_TEST_BCAST: mem=%-20s bcast_mem=%-25s, bcast_copyno=%d\n",
            SOC_MEM_NAME(unit, mem), SOC_MEM_NAME(unit, bcast_mem), bcast_copyno);
    cache_mem_test_create_mask(unit, mem, sizeof(mem_field_mask), mem_field_mask);
    entry_dw = soc_mem_entry_words(unit, mem);
    start_array_index = 0;
    end_array_index = 0;
    if (SOC_MEM_IS_ARRAY(unit,mem))
    {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    }
    start_entry_index     = parse_memory_index(unit, mem, "min");
    end_entry_index       = parse_memory_index(unit, mem, "max");
    
    for (array_index = start_array_index; array_index <= end_array_index; array_index++)
    {
        for (entry_index = start_entry_index; entry_index <= end_entry_index; entry_index++)
        {
            for (ii = 0; ii < entry_dw; ii++)
            {
                write_value[ii] = cache_mem_test_generate_value(unit, mem, array_index, entry_index, test_params->write_value_pattern);
                write_value[ii] = write_value[ii] & mem_field_mask[ii];
            }

            rv = soc_mem_array_write(unit, bcast_mem, array_index, bcast_copyno, entry_index, write_value);
            if (rv != SOC_E_NONE)
            {
                LOG_WARN(BSL_LS_APPL_TESTS,
                          (BSL_META("CACHE_MEM_TEST_BCAST: WRITE SOC: mem %d %s, array %d, index %d failed\n"),
                           bcast_mem, SOC_MEM_NAME(unit, bcast_mem), array_index, entry_index));
                goto done;
            }

            rv = soc_mem_array_read(unit, mem, array_index, MEM_BLOCK_ANY, entry_index, read_cache_value);
            rv |= soc_mem_array_read_flags(unit, mem, array_index, MEM_BLOCK_ANY, entry_index, read_hw_value, SOC_MEM_DONT_USE_CACHE);
            if (rv != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST_BCAST: Read FAILED rv %d: mem %d %s, array %d, entry %d\n"),
                                        rv, mem, SOC_MEM_NAME(unit, mem), array_index, entry_index));
                goto done;
            }

            for (ii = 0; ii < entry_dw; ii++)
            {
                if ((read_cache_value[ii] & mem_field_mask[ii]) != write_value[ii])
                {
                    rv = SOC_E_MEMORY;
                }
                if ((read_hw_value[ii] & mem_field_mask[ii]) != write_value[ii])
                {
                    rv = SOC_E_MEMORY;
                }
            }
            if (rv != SOC_E_NONE)
            {
                test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_COMPARE_FAILUE_FLAG;
                cli_out("CACHE_MEM_TEST_BCAST compare: mem %d %s, array %d, entry %d\n", 
                        mem, SOC_MEM_NAME(unit, mem), array_index, entry_index);
                cli_out("    %-20s", "write_value:");
                for (ii = 0; ii < entry_dw; ii++)
                {
                    cli_out("0x%08x ", write_value[ii]);
                }
                cli_out("\n");
                cli_out("    %-20s", "read_cache_value:");
                for (ii = 0; ii < entry_dw; ii++)
                {
                    cli_out("0x%08x ", read_cache_value[ii] & mem_field_mask[ii]);
                }
                cli_out("\n");
                cli_out("    %-20s", "read_hw_value:");
                for (ii = 0; ii < entry_dw; ii++)
                {
                    cli_out("0x%08x ", read_hw_value[ii] & mem_field_mask[ii]);
                }
                cli_out("\n");
                goto done;
            }
        }
    }

done:
    if (rv != SOC_E_NONE) {
        test_params->result |= rv;
    } else {
        test_params->stat_mem_succeed_cnt++;
    }

    return rv;

}

int cache_memory_test_cache_broadcast(int unit, tr_do_cache_mem_test_t* test_params)
{
    int rv;

    rv = soc_mem_iterate(unit, cache_memory_test_broadcast_iter_callback, (void*)test_params);

    return rv;
}

#endif

#endif


#if defined(BCM_DNXC_SUPPORT) && defined(BCM_ACCESS_SUPPORT)
static int access_do_cache_mem_test_init(int unit, tr_do_cache_mem_test_t *test_params, char* mem_name)
{
    int rv = 0;
    bcm_switch_event_control_t type;
    access_regmem_id_t regmem;
    access_block_id_t blk_type;

    blk_type = BLOCK_NOF_BLOCKS;
    if (sal_strnlen(test_params->block_name_parse, 1) != 0)
    {
        blk_type = access_block_type_name_match(unit, test_params->block_name_parse);
    }
    test_params->block_type = blk_type;

    if (sal_strnlen(test_params->access_type_parse, 1) != 0)
    {
        if (sal_strncasecmp(test_params->access_type_parse, "dma",  SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            test_params->write_type                 = cache_mem_test_write_type_dma;
            test_params->test_type                  = cache_mem_test_type_access_mode;
        }
        else if (sal_strncasecmp(test_params->access_type_parse, "schan",  SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            test_params->write_type                 = cache_mem_test_write_type_schan;
            test_params->test_type                  = cache_mem_test_type_access_mode;
        }
    }

    test_params->access_test_regmem_flags = ACCESS_CACHE_REGMEM_TEST_FLAG_MEM;
    test_params->mem_id = ACCESS_INVALID_REGMEM_ID;
    if (sal_strnlen(test_params->regmem_name_parse, 1) != 0)
    {
        if (sal_strncasecmp(test_params->regmem_name_parse, "mem",  SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            test_params->access_test_regmem_flags = ACCESS_CACHE_REGMEM_TEST_FLAG_MEM;
        }
        else if (sal_strncasecmp(test_params->regmem_name_parse, "reg",  SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            test_params->access_test_regmem_flags = ACCESS_CACHE_REGMEM_TEST_FLAG_REG;
        }
        else if (sal_strncasecmp(test_params->regmem_name_parse, "all",  SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            test_params->access_test_regmem_flags = ACCESS_CACHE_REGMEM_TEST_FLAG_MEM | ACCESS_CACHE_REGMEM_TEST_FLAG_REG;
        }
        else
        {
            regmem = access_parse_regmem_by_name(unit, test_params->regmem_name_parse);
            if (regmem != ACCESS_INVALID_REGMEM_ID)
            {
                test_params->access_test_regmem_flags = 0;
                test_params->mem_id = regmem;
            }
        }
    }

    /* pares mem-id for back-compariable without access cache test mode */
    if ((test_params->test_type != cache_mem_test_type_all_mems) &&
        (test_params->test_type != cache_mem_test_type_access_mode))
    {
        regmem = access_parse_regmem_by_name(unit, mem_name);
        if (regmem == ACCESS_INVALID_REGMEM_ID)
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Memory name %s is invalid\n"), mem_name));
            return CMD_USAGE;
        }
        test_params->mem_id = regmem;
    }

    if (test_params->test_type != cache_mem_test_type_access_mode)
    {
        /* Get Test Type */
        if (test_params->test_type > cache_mem_test_type_all_mems || test_params->test_type < cache_mem_test_type_single)
        {
            return CMD_USAGE;
        }

        /* Get Write Type */
        if (test_params->write_type > cache_mem_test_write_type_schan || test_params->write_type < cache_mem_test_write_type_dma)
        {
            return CMD_USAGE;
        }

        /* Get Pattern */
        if (test_params->write_value_pattern > cache_mem_test_write_value_pattern_smart || test_params->write_value_pattern < cache_mem_test_write_value_pattern_all_ones)
        {
            return CMD_USAGE;
        }

        if (test_params->test_part > cache_mem_test_cache_broadcast || test_params->test_part < cache_mem_test_full)
        {
            return CMD_USAGE;
        }
    }

    rv = bcm_linkscan_enable_set(unit, 0);
    if (_SHR_E_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "ACCESS CACHE_MEM_TEST: unit %d ERROR: Unable to disable linkscan\n"), unit));
        return CMD_FAIL;
    }
    soc_counter_stop(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        dnx_hbmc_temp_monitor_deinit(unit);
    }
#endif

    /** mask all interrupts */
    if ((test_params->test_part != cache_mem_test_cache_only) && (test_params->test_part != cache_mem_test_read_only))
    {
        type.action = bcmSwitchEventMask;
        type.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
        type.index = 0;
        rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);
        if (_SHR_E_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "ACCESS CACHE_MEM_TEST: unit %d ERROR: Unable to mask all interrupts\n"), unit));
            return CMD_FAIL;
        }
    }

    return 0;
}

int access_cache_mem_test_compare_entry(
    access_runtime_info_t *runtime_info,
    const access_device_regmem_t *rm_info,
    uint32 *data1,
    uint32 *data2)
{
    int rv;
    uint32 ii, width_in_words;
    uint32 mask[SOC_MEM_ENTRY_LENGTH_MAX];

    width_in_words = ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info);

    sal_memset(mask, 0, sizeof(mask));
    rv = access_regmem_datamask_rw_get(runtime_info->unit, rm_info->global_regmem_id, mask);
    if (SHR_FAILURE(rv))
    {
        return rv;
    }
    for (ii=0; ii<width_in_words; ii++)
    {
        data1[ii] &= mask[ii];
        data2[ii] &= mask[ii];
    }
    if (!SHR_BITEQ_RANGE(data1, data2, 0, rm_info->width_in_bits))
    {
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


int access_cache_mem_test_compare_data(
    access_runtime_info_t *runtime_info,
    const access_device_regmem_t *rm_info,
    cache_mem_test_write_value_pattern_t pattern,
    uint32 array_idx,
    uint32 entry_start,
    uint32 entry_num,
    uint32 *data)
{
    uint32 mask[SOC_MAX_MEM_WORDS];
    uint32 expect[SOC_MAX_MEM_WORDS];
    uint32 *cmp_buffer;
    uint32 data_value, width_in_words, entry_idx, ii, str_len;
    char *str_read = NULL;
    char *str_expect = NULL;

    SOC_INIT_FUNC_DEFS;

    width_in_words = ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info);
    str_len = width_in_words * 8 + 8;
    str_read = sal_alloc(str_len, "str_read");
    str_expect = sal_alloc(str_len, "str_expect");
    if ((str_read == NULL) || (str_expect == NULL))
    {
        _SOC_IF_ERR_EXIT(SOC_E_MEMORY);
    }

    sal_memset(mask, 0, sizeof(mask));
    _SOC_IF_ERR_EXIT(access_regmem_datamask_rw_get(runtime_info->unit, rm_info->global_regmem_id, mask));

    for (entry_idx=0; entry_idx<entry_num; entry_idx++)
    {
        cmp_buffer = data + entry_idx * width_in_words;
        data_value = cache_mem_test_generate_value(runtime_info->unit, rm_info->global_regmem_id, 
                                                   array_idx, entry_start + entry_idx, pattern);
        for (ii=0; ii<width_in_words; ii++)
        {
            expect[ii] = data_value & mask[ii];
            cmp_buffer[ii] = cmp_buffer[ii] & mask[ii];
        }

        if (!SHR_BITEQ_RANGE(cmp_buffer, expect, 0, width_in_words*32))
        {
            _shr_format_long_integer(str_read, cmp_buffer, width_in_words*4);
            _shr_format_long_integer(str_expect, expect, width_in_words*4);

            LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("compare_data fail: mem=%s, array=%d, entry=%d\n"),
                                         ACCESS_REGMEM_NAME(rm_info->global_regmem_id),
                                         array_idx, entry_start + entry_idx));
            LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("    %-12s: %s\n"), "ReadValue", str_read));
            LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("    %-12s: %s\n"), "ExpectValue", str_expect));

            _SOC_IF_ERR_EXIT(SOC_E_FAIL);
        }
    }

exit:
    if (str_read != NULL)
    {
        sal_free(str_read);
        str_read = NULL;
    }
    if (str_expect != NULL)
    {
        sal_free(str_expect);
        str_expect = NULL;
    }
    SOC_FUNC_RETURN;
}



int access_cache_mem_test_generate_data(
    access_runtime_info_t *runtime_info,
    const access_device_regmem_t *rm_info,
    cache_mem_test_write_value_pattern_t pattern,
    uint32 array_idx,
    uint32 entry_start,
    uint32 entry_num,
    uint32 *data)
{
    int rc;
    uint32 mask[SOC_MAX_MEM_WORDS];
    uint32 data_value, width_in_words, entry_idx, ii, total_idx;

    sal_memset(mask, 0, sizeof(mask));
    width_in_words = ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info);
    rc = access_regmem_datamask_rw_get(runtime_info->unit, rm_info->global_regmem_id, mask);
    if (_SHR_E_FAILURE(rc))
    {
        return rc;
    }
    for (entry_idx=0; entry_idx<entry_num; entry_idx++)
    {
        data_value = cache_mem_test_generate_value(runtime_info->unit, rm_info->global_regmem_id, 
                                                   array_idx, entry_start + entry_idx, pattern);
        for (ii=0; ii<width_in_words; ii++)
        {
            total_idx = entry_idx*width_in_words + ii;
            data[total_idx] = data_value & mask[ii];
        }
    }

    return 0;
}

static int access_cachemem_dma_array_range(
    int unit,
    uint32 flags,
    access_regmem_id_t regmem,
    access_local_block_instance_id_t instance,
    int array_idx,
    int entry_cnt,
    void *dma_buffer)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
    access_local_regmem_id_t local_regmem;

    SHR_FUNC_INIT_VARS(unit);
    /* get the local device info for calling the local device function */
    SHR_IF_ERR_EXIT(access_regmem_global2local(runtime_info, regmem, &local_regmem));

    SHR_IF_ERR_EXIT(access_local_regmem_sbusdma(runtime_info, flags, local_regmem, instance, array_idx, 0, entry_cnt, 0, dma_buffer));

exit:
    SHR_FUNC_EXIT;
}


/* 
    cache regmem test with below step
    1. read and save old regmem value.
    2. read cache value compare it with hw value.
    3. write regmem with the specfic value based on the patten.
    4. read regmem to check if match writed value.
    5. re-store regmem with saved value.
*/ 
static int access_cache_mem_test_new_access_mode(
    access_runtime_info_t *runtime_info,
    const access_device_regmem_t *rm_info,
    tr_do_cache_mem_test_t *test_params)
{
    int rv, dma_access;
    uint32 ii, array_idx, entry_idx, inst, flags;
    uint32 entry_cnt, width_in_words, buf_size;
    
    const access_device_type_info_t *device_info;
    const access_device_block_t *local_block;
    uint32 nof_instances, instances_start_index;
    const uint8 *block_instances_flags;

    char *str_entry_value = NULL;
    uint32 *dma_buffer = NULL;
    uint32 *pattern_buffer = NULL;
    uint32 *old_entry_value = NULL;
    uint32 *entry_ptr = NULL, *entry_ptr1 = NULL;
    uint32 cache_value[SOC_MEM_ENTRY_LENGTH_MAX];

    SOC_INIT_FUNC_DEFS;

    /* Regmem access mode: DMA or Schan */
    dma_access = 0;
    if (test_params->write_type == cache_mem_test_write_type_dma)
    {
        dma_access = 1;
    }

    /* Regmem paramter */
    device_info = runtime_info->device_type_info;
    local_block = device_info->blocks + rm_info->local_block_id;
    nof_instances = local_block->nof_instances;
    instances_start_index = local_block->instances_start_index;
    block_instances_flags = runtime_info->block_instance_flags + instances_start_index;
    entry_cnt = 1;
    if ACCESS_REGMEM_IS_MEMORY(rm_info)
    {
        entry_cnt = rm_info->u.mem.mem_nof_elements;
    }
    width_in_words = ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info);
    buf_size = entry_cnt * width_in_words * 4;

    str_entry_value = sal_alloc(width_in_words*8+8, "str_entry_value");
    if (str_entry_value == NULL)
    {
        rv = SOC_E_MEMORY;
        _SOC_IF_ERR_EXIT(rv);
    }
    dma_buffer = soc_cm_salloc(runtime_info->unit, buf_size, "dma_buffer");
    if (dma_buffer == NULL)
    {
        rv = SOC_E_MEMORY;
        _SOC_IF_ERR_EXIT(rv);
    }
    old_entry_value = sal_alloc(buf_size, "old_entry_value");
    if (old_entry_value == NULL)
    {
        rv = SOC_E_MEMORY;
        _SOC_IF_ERR_EXIT(rv);
    }
    pattern_buffer = sal_alloc(buf_size, "pattern_buffer");
    if (pattern_buffer == NULL)
    {
        rv = SOC_E_MEMORY;
        _SOC_IF_ERR_EXIT(rv);
    }
    
    for (inst=0; inst<nof_instances; inst++)
    {
        if (block_instances_flags[inst] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED)
        {
            continue;
        }

        for (ii=0; ii<rm_info->nof_array_indices; ii++)
        {
            array_idx = rm_info->first_array_index + ii;

            /* step-1. read and save old regmem value.*/
            sal_memset(old_entry_value, 0, buf_size);
            if (dma_access)
            {
                sal_memset(dma_buffer, 0, buf_size);
                flags = FLAG_ACCESS_DO_NOT_USE_CACHE;
                rv = access_cachemem_dma_array_range(runtime_info->unit, flags, rm_info->global_regmem_id,
                                                     inst, array_idx, entry_cnt, dma_buffer);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("DMA read Orig fail: %s %s, inst=%d, array_idx=%d\n"),
                             ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), inst, array_idx));
                    _SOC_IF_ERR_EXIT(rv);
                }
                sal_memcpy(old_entry_value, dma_buffer, buf_size);
            }
            else
            {
                for (entry_idx=0; entry_idx<entry_cnt; entry_idx++)
                {
                    entry_ptr = old_entry_value + entry_idx*width_in_words;
                    rv = access_regmem(runtime_info->unit, FLAG_ACCESS_DO_NOT_USE_CACHE,
                                       rm_info->global_regmem_id, inst, array_idx, entry_idx, entry_ptr);
                    if (_SHR_E_FAILURE(rv))
                    {
                        LOG_WARN(BSL_LS_APPL_TESTS,
                                 (BSL_META("Read Orig Fail: %s %s, array=%d, entry=%d\n"),
                                 ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                                 ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx));
                        _SOC_IF_ERR_EXIT(rv);
                    }
                }
            }
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "    %-60s  inst=%d, array_idx=%d\n"),
                                            "step-1. read and save old regmem value:", inst, array_idx));

            /* step-2. read cache value and compare it with hw value.*/
            for (entry_idx=0; entry_idx<entry_cnt; entry_idx++)
            {
                entry_ptr = old_entry_value + entry_idx*width_in_words;
                sal_memset(cache_value, 0, sizeof(cache_value));
                rv = access_regmem(runtime_info->unit, 0, rm_info->global_regmem_id,
                                   inst, array_idx, entry_idx, cache_value);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("Read Orig cache Fail: %s %s, array=%d, entry=%d\n"),
                             ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx));
                    _SOC_IF_ERR_EXIT(rv);
                }
                rv = access_cache_mem_test_compare_entry(runtime_info, rm_info, entry_ptr, cache_value);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("HW vs Cache compare Fail: %s %s, array=%d, entry=%d\n"),
                             ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx));
                    _shr_format_long_integer(str_entry_value, entry_ptr, width_in_words*4);
                    LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("    HW value:     %s\n"), str_entry_value));
                    _shr_format_long_integer(str_entry_value, cache_value, width_in_words*4);
                    LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("    Cache value:  %s\n"), str_entry_value));
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "    %-60s  inst=%d, array_idx=%d\n"),
                                            "step-2. read cache value and compare it with hw value:", inst, array_idx));

            /* if test_part is cache only, skip below sequence */
            if (test_params->test_part == cache_mem_test_cache_only)
            {
                continue;
            }

            /* step 3. write regmem with the specfic value based on the patten. */
            rv = access_cache_mem_test_generate_data(runtime_info, rm_info, test_params->write_value_pattern,
                                                     array_idx, 0, entry_cnt, pattern_buffer);
            if (_SHR_E_FAILURE(rv))
            {
                LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("ACCESS_CACHE_TEST generate pattern fail!!\n")));
                _SOC_IF_ERR_EXIT(rv);
            }
            if (dma_access)
            {
                sal_memcpy(dma_buffer, pattern_buffer, buf_size);
                flags = FLAG_ACCESS_IS_WRITE;
                rv = access_cachemem_dma_array_range(runtime_info->unit, flags, rm_info->global_regmem_id,
                                                     inst, array_idx, entry_cnt, dma_buffer);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("DMA Write pattern fail: %s %s, inst=%d, array_idx=%d\n"),
                             ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), inst, array_idx));
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
            else
            {
                for (entry_idx=0; entry_idx<entry_cnt; entry_idx++)
                {
                    entry_ptr = pattern_buffer + entry_idx*width_in_words;
                    rv = access_regmem(runtime_info->unit, FLAG_ACCESS_IS_WRITE, rm_info->global_regmem_id,
                                        inst, array_idx, entry_idx, entry_ptr);
                    if (_SHR_E_FAILURE(rv))
                    {
                        LOG_WARN(BSL_LS_APPL_TESTS,
                                 (BSL_META("Write pattern  Fail: %s %s, array=%d, entry=%d\n"),
                                 ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                                 ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx));
                        _SOC_IF_ERR_EXIT(rv);
                    }
                }
            }
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "    %-60s  inst=%d, array_idx=%d\n"),
                                            "step 3. write regmem with the specfic patten:", inst, array_idx));

            /* step-4. read regmem to check if match writed value. compare with pattern_buffer */
            sal_memset(dma_buffer, 0, buf_size);
            if (dma_access)
            {
                flags = FLAG_ACCESS_DO_NOT_USE_CACHE;
                rv = access_cachemem_dma_array_range(runtime_info->unit, flags, rm_info->global_regmem_id,
                                                     inst, array_idx, entry_cnt, dma_buffer);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("DMA Read pattern fail: %s %s, inst=%d, array_idx=%d\n"),
                             ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), inst, array_idx));
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
            else
            {
                for (entry_idx=0; entry_idx<entry_cnt; entry_idx++)
                {
                    entry_ptr = dma_buffer + entry_idx*width_in_words;
                    rv = access_regmem(runtime_info->unit, 0, rm_info->global_regmem_id,
                                        inst, array_idx, entry_idx, entry_ptr);
                    if (_SHR_E_FAILURE(rv))
                    {
                        LOG_WARN(BSL_LS_APPL_TESTS,
                                 (BSL_META("Read pattern Fail: %s %s, array=%d, entry=%d\n"),
                                 ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                                 ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx));
                        _SOC_IF_ERR_EXIT(rv);
                    }
                }
            }
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "    %-60s  inst=%d, array_idx=%d\n"),
                                            "    step 4-1. read regmem:", inst, array_idx));
            /* compare dma_buffer vs pattern_buffer */
            for (entry_idx=0; entry_idx<entry_cnt; entry_idx++)
            {
                entry_ptr = dma_buffer + entry_idx*width_in_words;
                entry_ptr1 = pattern_buffer + entry_idx*width_in_words;
                rv = access_cache_mem_test_compare_entry(runtime_info, rm_info, entry_ptr, entry_ptr1);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("Read vs Pattern compare Fail: %s %s, array=%d, entry=%d\n"),
                             ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx));
                    _shr_format_long_integer(str_entry_value, entry_ptr, width_in_words*4);
                    LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("    Read value:     %s\n"), str_entry_value));
                    _shr_format_long_integer(str_entry_value, entry_ptr1, width_in_words*4);
                    LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("    Pattern value:  %s\n"), str_entry_value));
                    _SOC_IF_ERR_EXIT(rv);
                }
            }

            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "    %-60s  inst=%d, array_idx=%d\n"),
                                            "    step 4-2. compare read vs pattern:", inst, array_idx));

            /* step 5. re-store regmem with saved value (old_entry_value). */
            if (dma_access)
            {
                sal_memcpy(dma_buffer, old_entry_value, buf_size);
                flags = FLAG_ACCESS_IS_WRITE;
                rv = access_cachemem_dma_array_range(runtime_info->unit, flags, rm_info->global_regmem_id, inst,
                                                     array_idx, entry_cnt, dma_buffer);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("DMA restore old value fail: %s %s, inst=%d, array_idx=%d\n"),
                             ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), inst, array_idx));
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
            else
            {
                for (entry_idx=0; entry_idx<entry_cnt; entry_idx++)
                {
                    entry_ptr = old_entry_value + entry_idx*width_in_words;
                    rv = access_regmem(runtime_info->unit, FLAG_ACCESS_IS_WRITE, rm_info->global_regmem_id,
                                       inst, array_idx, entry_idx, entry_ptr);
                    if (_SHR_E_FAILURE(rv))
                    {
                        LOG_WARN(BSL_LS_APPL_TESTS,
                                 (BSL_META("Restore old value  Fail: %s %s, array=%d, entry=%d\n"),
                                 ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                                 ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx));
                        _SOC_IF_ERR_EXIT(rv);
                    }
                }
            }

            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "    %-60s  inst=%d, array_idx=%d\n"),
                                            "step 5. re-store regmem with saved value:", inst, array_idx));
        }
    }

exit:
    if (dma_buffer != NULL)
    {
        soc_cm_sfree(runtime_info->unit, dma_buffer);
        dma_buffer = NULL;
    }
    if (str_entry_value != NULL)
    {
        sal_free(str_entry_value);
        str_entry_value = NULL;
    }
    if (pattern_buffer != NULL)
    {
        sal_free(pattern_buffer);
        pattern_buffer = NULL;
    }
    if (old_entry_value != NULL)
    {
        sal_free(old_entry_value);
        old_entry_value = NULL;
    }

    SOC_FUNC_RETURN;
}


static int access_cache_mem_test_write_cb(
    access_runtime_info_t *runtime_info,
    const access_device_regmem_t *rm_info,
    tr_do_cache_mem_test_t *test_params)
{
    int rv;
    uint32 write_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32 ii, array_idx, entry_idx, inst, nof_entry;
    uint32 entry_cnt, width_in_words, buf_size, flags;
    void *write_dma_buffer = NULL;
    const access_device_type_info_t *device_info;
    const access_device_block_t *local_block;
    uint32 nof_instances, instances_start_index;
    const uint8 *block_instances_flags;
    char *str_write_value = NULL;

    SOC_INIT_FUNC_DEFS;

    str_write_value = sal_alloc(ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info)*8+8, "str_write_value");
    if (str_write_value == NULL)
    {
        rv = SOC_E_MEMORY;
        _SOC_IF_ERR_EXIT(rv);
    }

    if ((test_params->write_type == cache_mem_test_write_type_dma) && ACCESS_REGMEM_IS_MEMORY(rm_info))
    {
        device_info = runtime_info->device_type_info;
        local_block = device_info->blocks + rm_info->local_block_id;
        nof_instances = local_block->nof_instances;
        instances_start_index = local_block->instances_start_index;
        block_instances_flags = runtime_info->block_instance_flags + instances_start_index;

        entry_cnt = rm_info->u.mem.mem_nof_elements;
        width_in_words = ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info);
        buf_size = entry_cnt * width_in_words;
        buf_size = buf_size * 4;
        write_dma_buffer = soc_cm_salloc(runtime_info->unit, buf_size, "cache mem test write");
        if (write_dma_buffer == NULL)
        {
            rv = SOC_E_MEMORY;
            LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("Write: soc_cm_salloc fail\n")));
            _SOC_IF_ERR_EXIT(rv);
        }

        for (inst=0; inst<nof_instances; inst++)
        {
            if (block_instances_flags[inst] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED)
            {
                continue;
            }
            for (ii=0; ii<rm_info->nof_array_indices; ii++)
            {
                array_idx = rm_info->first_array_index + ii;

                sal_memset(write_dma_buffer, 0, buf_size);
                rv = access_cache_mem_test_generate_data(runtime_info, rm_info, test_params->write_value_pattern,
                                                         array_idx, 0, entry_cnt, write_dma_buffer);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("access_cache_mem_test_generate_data: failed for dma\n")));
                    _SOC_IF_ERR_EXIT(rv);
                }

                flags = FLAG_ACCESS_IS_WRITE;
                rv = access_cachemem_dma_array_range(runtime_info->unit, flags, rm_info->global_regmem_id, inst,
                                                     array_idx, rm_info->u.mem.mem_nof_elements, write_dma_buffer);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("DMA write fail: mem=%s, inst=%d, array=%d\n"),
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), inst, array_idx));
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
        }
    }
    else
    {
        if (ACCESS_REGMEM_IS_MEMORY(rm_info))
        {
            nof_entry = rm_info->u.mem.mem_nof_elements;
        }
        else
        {
            nof_entry = 1;
        }
        for (ii=0; ii<rm_info->nof_array_indices; ii++)
        {
            array_idx = rm_info->first_array_index + ii;
            for (entry_idx=0; entry_idx<nof_entry; entry_idx++)
            {
                rv = access_cache_mem_test_generate_data(runtime_info, rm_info, test_params->write_value_pattern,
                                                         array_idx, entry_idx, 1, write_value);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("access_cache_mem_test_generate_data: failed\n")));
                    _SOC_IF_ERR_EXIT(rv);
                }
                _shr_format_long_integer(str_write_value, write_value, ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info)*4);
                rv = access_regmem(runtime_info->unit, FLAG_ACCESS_IS_WRITE, rm_info->global_regmem_id,
                                   ACCESS_ALL_BLOCK_INSTANCES, array_idx, entry_idx, write_value);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("access_write fail: %s %s, array=%d, entry=%d, write_value=%s\n"),
                             ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx, str_write_value));
                    _SOC_IF_ERR_EXIT(rv);
                }
                else
                {
                    LOG_VERBOSE(BSL_LS_APPL_TESTS,
                                (BSL_META("access_write: %s %s, array=%d, entry=%d, write_value=%s\n"),
                                ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                                ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx, str_write_value));
                }
            }
        }
    }

exit:
    if (write_dma_buffer != NULL)
    {
        soc_cm_sfree(runtime_info->unit, write_dma_buffer);
        write_dma_buffer = NULL;
    }
    if (str_write_value != NULL)
    {
        sal_free(str_write_value);
        str_write_value = NULL;
    }
    SOC_FUNC_RETURN;
}

static int access_cache_mem_test_read_compare_cb(
    access_runtime_info_t *runtime_info,
    const access_device_regmem_t *rm_info,
    tr_do_cache_mem_test_t *test_params)
{
    int rv;
    uint32 read_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32 ii, array_idx, entry_idx, inst, nof_entry;
    uint32 entry_cnt, width_in_words, buf_size, flags;
    void *read_dma_buffer = NULL;
    const access_device_type_info_t *device_info;
    const access_device_block_t *local_block;
    uint32 nof_instances, instances_start_index;
    const uint8 *block_instances_flags;
    char *str_read_value = NULL;

    SOC_INIT_FUNC_DEFS;
    
    str_read_value = sal_alloc(ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info)*8+8, "str_read_value");
    if (str_read_value == NULL)
    {
        rv = SOC_E_MEMORY;
        _SOC_IF_ERR_EXIT(rv);
    }

    if ((test_params->write_type == cache_mem_test_write_type_dma) && ACCESS_REGMEM_IS_MEMORY(rm_info))
    {
        device_info = runtime_info->device_type_info;
        local_block = device_info->blocks + rm_info->local_block_id;
        nof_instances = local_block->nof_instances;
        instances_start_index = local_block->instances_start_index;
        block_instances_flags = runtime_info->block_instance_flags + instances_start_index;

        entry_cnt = rm_info->u.mem.mem_nof_elements;
        width_in_words = ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info);
        buf_size = entry_cnt * width_in_words;
        buf_size = buf_size * 4;
        read_dma_buffer = soc_cm_salloc(runtime_info->unit, buf_size, "cache mem test read");
        if (read_dma_buffer == NULL)
        {
            rv = SOC_E_MEMORY;
            LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("Read_Compare : soc_cm_salloc fail\n")));
            _SOC_IF_ERR_EXIT(rv);
        }

        for (inst=0; inst<nof_instances; inst++)
        {
            if (block_instances_flags[inst] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED)
            {
                continue;
            }
            for (ii=0; ii<rm_info->nof_array_indices; ii++)
            {
                array_idx = rm_info->first_array_index + ii;

                sal_memset(read_dma_buffer, 0, buf_size);
                flags = FLAG_ACCESS_DO_NOT_USE_CACHE;
                rv = access_cachemem_dma_array_range(runtime_info->unit, flags,
                                                     rm_info->global_regmem_id, inst, array_idx,
                                                     rm_info->u.mem.mem_nof_elements, read_dma_buffer);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("DMA read fail: mem=%s, inst=%d, array=%d\n"),
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), inst, array_idx));
                    _SOC_IF_ERR_EXIT(rv);
                }

                rv = access_cache_mem_test_compare_data(runtime_info, rm_info, test_params->write_value_pattern,
                                                         array_idx, 0, entry_cnt, read_dma_buffer);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS, 
                             (BSL_META("DMA read doen't meet expect!!, inst=%d, array_idx=%d\n"),
                                       inst, array_idx ));
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
        }
    }
    else
    {
        if (ACCESS_REGMEM_IS_MEMORY(rm_info))
        {
            nof_entry = rm_info->u.mem.mem_nof_elements;
        }
        else
        {
            nof_entry = 1;
        }

        for (ii=0; ii<rm_info->nof_array_indices; ii++)
        {
            array_idx = rm_info->first_array_index + ii;
            for (entry_idx=0; entry_idx<nof_entry; entry_idx++)
            {
                rv = access_regmem(runtime_info->unit, FLAG_ACCESS_DO_NOT_USE_CACHE, rm_info->global_regmem_id,
                                   ACCESS_ALL_BLOCK_INSTANCES, array_idx, entry_idx, read_value);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS,
                             (BSL_META("Read Fail: %s %s, array=%d, entry=%d\n"),
                             ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                             ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx));
                    _SOC_IF_ERR_EXIT(rv);
                }
                else
                {
                    _shr_format_long_integer(str_read_value, read_value, ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info)*4);
                    LOG_VERBOSE(BSL_LS_APPL_TESTS,
                                (BSL_META("Read result: %s %s, array=%d, entry=%d, read_value=%s\n"),
                                ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                                ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx, str_read_value));
                }

                rv = access_cache_mem_test_compare_data(runtime_info, rm_info, test_params->write_value_pattern,
                                                         array_idx, entry_idx, 1, read_value);
                if (_SHR_E_FAILURE(rv))
                {
                    LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("Read doen't meet expect!!, array_idx=%d, entry_idx=%d\n"),
                                                          array_idx, entry_idx));
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
        }
    }

exit:
    if (read_dma_buffer != NULL)
    {
        soc_cm_sfree(runtime_info->unit, read_dma_buffer);
        read_dma_buffer = NULL;
    }
    if (str_read_value != NULL)
    {
        sal_free(str_read_value);
        str_read_value = NULL;
    }
    SOC_FUNC_RETURN;
}

static int access_cache_mem_test_cache_compare_cb(
    access_runtime_info_t *runtime_info,
    const access_device_regmem_t *rm_info,
    tr_do_cache_mem_test_t *test_params)
{
    int rv;
    uint32 hw_read_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32 cache_read_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32 mask[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32 ii, jj, array_idx, entry_idx, width_in_words, nof_entry;

    SOC_INIT_FUNC_DEFS;
    width_in_words = ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info);
    sal_memset(mask, 0, sizeof(mask));
    _SOC_IF_ERR_EXIT(access_regmem_datamask_rw_get(runtime_info->unit, rm_info->global_regmem_id, mask));

    if (ACCESS_REGMEM_IS_MEMORY(rm_info))
    {
        nof_entry = rm_info->u.mem.mem_nof_elements;
    }
    else
    {
        nof_entry = 1;
    }

    for (ii=0; ii<rm_info->nof_array_indices; ii++)
    {
        array_idx = rm_info->first_array_index + ii;
        for (entry_idx=0; entry_idx<nof_entry; entry_idx++)
        {
            rv = access_regmem(runtime_info->unit, FLAG_ACCESS_DO_NOT_USE_CACHE, rm_info->global_regmem_id,
                                ACCESS_ALL_BLOCK_INSTANCES, array_idx, entry_idx, hw_read_value);
            rv |= access_regmem(runtime_info->unit, 0, rm_info->global_regmem_id,
                                ACCESS_ALL_BLOCK_INSTANCES, array_idx, entry_idx, cache_read_value);
            if (_SHR_E_FAILURE(rv))
            {
                LOG_WARN(BSL_LS_APPL_TESTS,
                         (BSL_META("Read Fail: %s %s, array=%d, entry=%d\n"),
                         ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                         ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx));
                _SOC_IF_ERR_EXIT(rv);
            }

            rv = 0; 
            for (jj = 0; jj < width_in_words; jj++)
            {
                if ((hw_read_value[jj] & mask[jj]) != (cache_read_value[jj] & mask[jj]))
                {
                    rv = SOC_E_FAIL;
                    break;
                }
            }
            if (_SHR_E_FAILURE(rv))
            {
                cli_out("COMPARE FAILED: %s %s, array=%d, entry=%d",
                         ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                         ACCESS_REGMEM_NAME(rm_info->global_regmem_id), array_idx, entry_idx);
                cli_out("\n    %-8s", "HW:");
                for (jj = 0; jj< width_in_words; jj++)
                {
                    cli_out("%08x ", hw_read_value[jj]);
                }
                
                cli_out("\n    %-8s", "Cache:");
                for (jj = 0; jj < width_in_words; jj++)
                {
                    cli_out("%08x ", cache_read_value[jj]);
                }
                cli_out("\n    %-8s", "Mask:");
                for (jj = 0; jj < width_in_words; jj++)
                {
                    cli_out("%08x ", mask[jj]);
                }
                cli_out("\n");
                _SOC_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

static int access_cache_mem_test_is_skip(
    access_runtime_info_t *runtime_info,
    const access_device_regmem_t *rm_info,
    tr_do_cache_mem_test_t *test_params)
{
    int rc = 0;

    switch (rm_info->global_regmem_id)
    {
        case rMESH_TOPOLOGY_MESH_STATUSES:
        case rMDB_ARM_KAPS_GLOBAL_EVENT:
        case rMDB_ARM_KAPS_R_5_FIFO_MON_EVENT:
        case rMDB_ARM_KAPS_MEMORY_A_ERROR:
        case rMDB_ARM_KAPS_MEMORY_B_ERROR:
        case rMDB_ARM_KAPS_SEARCH_0_A_ERROR:
        case rMDB_ARM_KAPS_SEARCH_1_A_ERROR:
        case rMDB_ARM_KAPS_SEARCH_0_B_ERROR:
        case rMDB_ARM_KAPS_SEARCH_1_B_ERROR:
        case rMDB_ARM_KAPS_STATUS_CFIFO:
        case rMDB_ARM_KAPS_STATUS_RFIFO:
        case rMDB_ARM_KAPS_IBC_COMMAND_WORD:
        case rMDB_ARM_KAPS_IBC_RESPONSE_DATA:
        case rMDB_ARM_KAPS_IBC_QUEUE_STATUS:
        case rMDB_ARM_KAPS_IBC_FIFO_CORRECTABLE_STATUS:
        case rMDB_ARM_KAPS_IBC_FIFO_UNCORRECTABLE_STATUS:
        case rMDB_ARM_KAPS_R_5_CORE_MEM_CONTROL:
        case rMDB_ARM_KAPS_R_5_ECC_ERR_WR_TCM_A:
        case rMDB_ARM_KAPS_R_5_ECC_ERR_WR_TCM_B_0:
        case rMDB_ARM_KAPS_R_5_ECC_ERR_WR_TCM_B_1:
        case rMDB_ARM_KAPS_R_5_ECC_ERR_RD_TCM_A:
        case rMDB_ARM_KAPS_R_5_ECC_ERR_RD_TCM_B_0:
        case rMDB_ARM_KAPS_R_5_ECC_ERR_RD_TCM_B_1:
        case rMDB_ARM_KAPS_R_5_AXI_ERROR_STATUS_0:
        case rMDB_ARM_KAPS_R_5_AXI_ERROR_STATUS_1:
        case rMDB_ARM_KAPS_R_5_DEBUG_STATUS:
        case rMDB_ARM_KAPS_R_5_DEBUG_STATUS_EVENT_BUS_0:
        case rMDB_ARM_KAPS_R_5_DEBUG_STATUS_EVENT_BUS_1:
        case rMDB_ARM_KAPS_R_5_DAP_APB_CTRL:
        case rMDB_ARM_KAPS_R_5_DAP_APB_RDATA:
        {
            rc = 1;
            break;
        }
        case mSCH_FORCE_STATUS_MESSAGE:
        {
            rc = 1;
            break;
        }

        default:
        {
            break;
        }
    }
    return rc;
}


static void access_cache_memory_test_result_dump(
    access_runtime_info_t *runtime_info,
    tr_do_cache_mem_test_t* test_params,
    uint32 nof_regmems)
{
    access_local_regmem_id_t local_regmem;
    const access_device_regmem_t *rm_info;

    cli_out("ACCESS_CACHE_MEM_TEST result:\n");
    for (local_regmem=0; local_regmem<nof_regmems; local_regmem++)
    {
        if (test_params->access_test_result_flag[local_regmem] == 0)
        {
            continue;
        }
        rm_info = ACCESS_LOCAL_REGMEM_RMINFO(runtime_info, local_regmem);
        cli_out("    %-8d %s %-60s%-4x", local_regmem,
                ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                ACCESS_REGMEM_NAME(rm_info->global_regmem_id),
                test_params->access_test_result_flag[local_regmem]);
        if (test_params->access_test_result_flag[local_regmem] & CACHE_MEM_TEST_FORCE_SKIP_FLAG)
        {
            cli_out("FORCE_SKIP ");
        }
        if (test_params->access_test_result_flag[local_regmem] & CACHE_MEM_TEST_ACCESS_FAIL_FLAG)
        {
            cli_out("TEST_FAIL");
        }
        cli_out("\n");
    }

    return;
}

static int access_cache_mem_test_single_mem_cb(
    access_runtime_info_t *runtime_info,
    access_local_regmem_id_t local_regmem,
    tr_do_cache_mem_test_t *test_params)
{
    const access_device_regmem_t *rm_info;

    SOC_INIT_FUNC_DEFS;

    rm_info = ACCESS_LOCAL_REGMEM_RMINFO(runtime_info, local_regmem);
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "%-6d: Cache test for %s %s[pattern=%d, %s]:  "),
                                 local_regmem, ACCESS_REGMEM_IS_MEMORY(rm_info)? " Memory " : "Register",
                                 ACCESS_REGMEM_NAME(rm_info->global_regmem_id), test_params->write_value_pattern,
                                 (test_params->write_type == cache_mem_test_write_type_dma)? " DMA " : "SCHAN"));
    if (test_params->test_type == cache_mem_test_type_access_mode)
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "access_mode\n")));
    }
    else if (test_params->test_part == cache_mem_test_full)
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "full\n")));
    }
    else if (test_params->test_part == cache_mem_test_write_only)
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "write_only\n")));
    }
    else if (test_params->test_part == cache_mem_test_read_only)
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "read_only\n")));
    }
    else if (test_params->test_part == cache_mem_test_cache_only)
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "cache_only\n")));
    }
    else if (test_params->test_part == cache_mem_test_cache_broadcast)
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "broadcast\n")));
    }
    else
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(runtime_info->unit, "null\n")));
    }

    if (test_params->test_type == cache_mem_test_type_access_mode)
    {
        _SOC_IF_ERR_EXIT(access_cache_mem_test_new_access_mode(runtime_info, rm_info, test_params));
        SOC_EXIT;
    }
    switch(test_params->test_part)
    {
        case cache_mem_test_full:
        {
            _SOC_IF_ERR_EXIT(access_local_regmem_cache_set(runtime_info, local_regmem, 1));
            
            _SOC_IF_ERR_EXIT(access_cache_mem_test_write_cb(runtime_info, rm_info, test_params));

            _SOC_IF_ERR_EXIT(access_cache_mem_test_read_compare_cb(runtime_info, rm_info, test_params));
            break;
        }
        case cache_mem_test_write_only:
        {
            _SOC_IF_ERR_EXIT(access_local_regmem_cache_set(runtime_info, local_regmem, 1));
            
            _SOC_IF_ERR_EXIT(access_cache_mem_test_write_cb(runtime_info, rm_info, test_params));
            break;
        }
        case cache_mem_test_read_only:
        {
            _SOC_IF_ERR_EXIT(access_cache_mem_test_read_compare_cb(runtime_info, rm_info, test_params));
            break;
        }
        case cache_mem_test_cache_only:
        {
            /*soc_mem_iterate(unit, cache_mem_test_read_and_compare_wo_expected, tr_do_cache_mem_test);*/
            _SOC_IF_ERR_EXIT(access_cache_mem_test_cache_compare_cb(runtime_info, rm_info, test_params));
            break;
        }
        case cache_mem_test_cache_broadcast:
        {
            /*rv = cache_memory_test_cache_broadcast(unit, test_params);*/
            break;
        }
        default:
        {
            break;
        }
    }

exit:
    SOC_FUNC_RETURN;
}


static int cache_test_access_mem_iter(int unit, tr_do_cache_mem_test_t *test_params)
{
    int rv = SOC_E_NONE;
    access_runtime_info_t *runtime_info;
    const access_device_type_info_t *device_info;
    const access_device_regmem_t *rm_info;
    access_local_regmem_id_t local_regmem, nof_regmems, start_regmem, end_regmem;
    access_local_block_id_t local_blk_id;
    const access_device_block_t *block;
    access_block_id_t block_type;

    runtime_info = access_runtime_info_get(unit);
    device_info = runtime_info->device_type_info;
    nof_regmems = device_info->nof_mems + device_info->nof_regs;
    test_params->access_test_result_flag = sal_alloc(sizeof(uint32) * nof_regmems, "access_test_result_flag");
    if (test_params->access_test_result_flag == NULL)
    {
        rv = SOC_E_MEMORY;
        test_params->result |= rv;
        return rv;
    }
    sal_memset(test_params->access_test_result_flag, 0, sizeof(uint32) * nof_regmems);

    if (test_params->access_test_regmem_flags & ACCESS_CACHE_REGMEM_TEST_FLAG_REG)
    {
        start_regmem = 0;
    }
    else
    {
        start_regmem = device_info->nof_regs;
    }
    if (test_params->access_test_regmem_flags & ACCESS_CACHE_REGMEM_TEST_FLAG_MEM)
    {
        end_regmem = device_info->nof_mems + device_info->nof_regs;
    }
    else
    {
        end_regmem = device_info->nof_regs;
    }
    
    /* Iter all memory & register */
    for (local_regmem = start_regmem; local_regmem < end_regmem; local_regmem++)
    {
        rm_info = ACCESS_LOCAL_REGMEM_RMINFO(runtime_info, local_regmem);
        local_blk_id = rm_info->local_block_id;
        block = ACCESS_DEV_BLK_INFO(runtime_info, local_blk_id);
        block_type = block->block_type;

        if ((block_type == BLOCK_CMIC) || (block_type == BLOCK_IPROC) || (block_type == BLOCK_ECI))
        {
            continue;
        }

        /* Skip if blcok doesn't match input */
        if (test_params->block_type < BLOCK_NOF_BLOCKS)
        {
            if (test_params->block_type != block_type)
            {
                continue;
            }
        }
        
        if (!access_local_regmem_cachable(runtime_info, local_regmem))
        {
            continue;
        }
        if (ACCESS_REGMEM_IS_ALIAS(rm_info))
        {
            continue;
        }
        if ((!ACCESS_REGMEM_IS_MEMORY(rm_info)) && (rm_info->base_address < 256))
        {
            continue;
        }

        test_params->stat_mem_total_cnt++;
        if (access_cache_mem_test_is_skip(runtime_info, rm_info, test_params))
        {
            test_params->stat_mem_not_tested_cnt++;
            test_params->access_test_result_flag[local_regmem] |= CACHE_MEM_TEST_FORCE_SKIP_FLAG;
            continue;
        }

        rv = access_cache_mem_test_single_mem_cb(runtime_info, local_regmem, test_params);
        if (_SHR_E_FAILURE(rv))
        {
            LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("ACCESS CACHE_MEM_TEST fail on %s!!\n"),
                                          ACCESS_REGMEM_NAME(rm_info->global_regmem_id)));
            test_params->stat_mem_fail_cnt++;
            test_params->access_test_result_flag[local_regmem] |= CACHE_MEM_TEST_ACCESS_FAIL_FLAG;
            test_params->result |= rv;
        }
        else
        {
            test_params->stat_mem_succeed_cnt++;
        }
    }

    access_cache_memory_test_result_dump(runtime_info, test_params, nof_regmems);
    sal_free(test_params->access_test_result_flag);
    test_params->access_test_result_flag = NULL;

    return rv;
}

static int access_do_cache_mem_test_all(int unit, tr_do_cache_mem_test_t *test_params)
{
    int rv;

    if (test_params->test_part == cache_mem_test_cache_broadcast)
    {
        rv = 0;
    }
    else
    {
        rv = cache_test_access_mem_iter(unit, test_params);
    }

    return rv;     
}

static int access_do_cache_mem_test_single(int unit, tr_do_cache_mem_test_t *test_params)
{
    int rv;
    access_regmem_id_t regmem;
    access_runtime_info_t *runtime_info;
    access_local_regmem_id_t local_regmem;

    runtime_info = access_runtime_info_get(unit);

    regmem = (access_regmem_id_t)test_params->mem_id;
    rv = access_regmem_global2local(runtime_info, regmem, &local_regmem);
    if (SHR_FAILURE(rv))
    {
        test_params->result |= rv;

        LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("access_do_cache_mem_test_single: Invalid regmem\n")));
        return rv;
    }
    rv = access_cache_mem_test_single_mem_cb(runtime_info, local_regmem, test_params);
    if (_SHR_E_FAILURE(rv))
    {
        LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META("access_do_cache_mem_test_single fail: mem=%s!!\n"),
                                     ACCESS_REGMEM_NAME(regmem)));
        test_params->result |= rv;
    }

    return rv;
}


static int access_do_cache_mem_test(int unit, tr_do_cache_mem_test_t *test_params)
{
    int rv;
    switch(test_params->test_type)
    {
        case cache_mem_test_type_access_mode:
        {
            if (test_params->mem_id == ACCESS_INVALID_REGMEM_ID)
            {
                rv = access_do_cache_mem_test_all(unit, test_params);
            }
            else
            {
                rv = access_do_cache_mem_test_single(unit, test_params);
            }
            break;
        }
        case cache_mem_test_type_all_mems:  /* Run tests for all memories */
        {
            rv = access_do_cache_mem_test_all(unit, test_params);
            break;
        }
        case cache_mem_test_type_single:    /* Run test for specific memory */
        {
            rv = access_do_cache_mem_test_single(unit, test_params);
            break;
        }
        default:
        {
            rv = 0;
            break;
        }
    }

    if (test_params->result)
    {
        test_error(unit, "ACCESS_CACHE_MEM_TEST Failed!\n");
    }
    if (_SHR_E_FAILURE(rv))
    {
        return CMD_FAIL;
    }
    else
    {
        return CMD_OK;
    }
}
#endif


void cache_mem_test_create_mask(
        SOC_SAND_IN  uint32                 unit,
        SOC_SAND_IN  soc_mem_t              mem,
        SOC_SAND_IN  size_t                 mask_len,
        SOC_SAND_OUT uint32                 *mask)
{
    int                     i;
    uint32                  entry_dw = soc_mem_entry_words(unit, mem);
    soc_mem_info_t          *meminfo;
    sal_memset(mask, 0, mask_len);

    meminfo         = &SOC_MEM_INFO(unit, mem);

    for ( i = 0; i < meminfo->nFields; i++)
    {
        /* Skip the Read-only/Write-Only/Signal/Reserved fields */
        if ((meminfo->fields[i].flags & SOCF_RO) ||  (meminfo->fields[i].flags & SOCF_WO) || (meminfo->fields[i].flags & SOCF_SIG) || (meminfo->fields[i].flags & SOCF_RES) )
            continue;
        SHR_BITSET_RANGE(mask, meminfo->fields[i].bp, meminfo->fields[i].len);
    }

    /* Print the Mask */
    LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: Mask for memory %d %s, nFields %d mask 0x"),
                        mem, SOC_MEM_NAME(unit, mem), meminfo->nFields));

    for ( i = entry_dw - 1; i >= 0; i--)
    {
        LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("%X"), mask[i]));
    }

}

uint32 cache_mem_test_fill_values_soc_mem_write_callback(uint32 unit, soc_mem_t mem, uint32     i_array, tr_do_cache_mem_test_t *test_params)
{
    uint32      start_index, end_index, index;
    uint32      entry_index = 0;
    uint32      write_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32      mem_field_mask[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32                  entry_dw = soc_mem_entry_words(unit, mem);
    soc_error_t             rv = SOC_E_NONE;
    soc_mem_t               mem_orig = mem;
    SOC_MEM_ALIAS_TO_ORIG(unit, mem_orig);

    start_index     = parse_memory_index(unit, mem, "min");
    end_index       = parse_memory_index(unit, mem, "max");

    cache_mem_test_create_mask(unit, mem, sizeof(mem_field_mask), mem_field_mask);

    LOG_INFO(BSL_LS_APPL_TESTS,
                (BSL_META("CACHE_MEM_TEST: WRITE SOC: mem %d %s, num_of_entries %d, entry_dw %d\n"),
                         mem, SOC_MEM_NAME(unit, mem), end_index - start_index + 1/*table_size*/, entry_dw));

    for (index = start_index; index <= end_index; index++ )
    {
        for (entry_index = 0; entry_index < entry_dw; entry_index++)
        {
            write_value[entry_index]  = cache_mem_test_generate_value(unit, mem_orig, i_array, index, test_params->write_value_pattern) & mem_field_mask[entry_index];
        }
        rv = soc_mem_array_write(unit, mem, i_array, MEM_BLOCK_ALL, index, write_value);
        LOG_VERBOSE(BSL_LS_APPL_TESTS,
                      (BSL_META("CACHE_MEM_TEST: WRITE SOC: mem %d %s, array %d, index=%d, write_value=%08X %08X\n"),
                       mem, SOC_MEM_NAME(unit, mem), i_array, index, write_value[0], write_value[1]));
        if (rv != SOC_E_NONE)
        {
            LOG_WARN(BSL_LS_APPL_TESTS,
                      (BSL_META("CACHE_MEM_TEST: WRITE SOC: mem %d %s, array %d, index %d failed\n"),
                       mem, SOC_MEM_NAME(unit, mem), i_array, index));
            break;
        }
    }

    if (rv != SOC_E_NONE)
        test_params->result |= rv;

    return rv;
}



int cache_mem_test_fill_values_dma_callback(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int copyno,
    SOC_SAND_IN int array_index,
    SOC_SAND_IN int index,
    SOC_SAND_OUT uint32 *value,
    SOC_SAND_IN int entry_sz,
    SOC_SAND_IN void *opaque)
{
    uint32          i               = 0;
    uint32          entry_size      = entry_sz;
    uint32          mem_field_mask[SOC_MEM_ENTRY_LENGTH_MAX];
    tr_do_cache_mem_test_t *test_params= ((tr_do_cache_mem_test_t *)opaque);
    soc_mem_t       mem             = test_params->mem_id;
    soc_mem_t       mem_orig        = mem;
    SOC_MEM_ALIAS_TO_ORIG(unit, mem_orig);
    *value = 0;

    cache_mem_test_create_mask(unit, mem, sizeof(mem_field_mask), mem_field_mask);

    for (i = 0; i < entry_size; i++)
    {
        value[i] = cache_mem_test_generate_value(unit, mem_orig, array_index, index, test_params->write_value_pattern) & mem_field_mask[i];
        LOG_DEBUG(BSL_LS_APPL_TESTS,
                    (BSL_META("CACHE_MEM_TEST: WRITE DMA: mem  %d %s i %d value 0x%X mask 0x%X \n"),
                            mem, SOC_MEM_NAME(unit, mem), i, value[i], mem_field_mask[i]));

    }

    return SOC_E_NONE;
}

int
cache_mem_test_write_iter_callback(int unit, soc_mem_t mem, void* data)
{
    uint32      rv = SOC_E_NONE;
    uint32      start_array_index = 0, end_array_index = 0;
    uint32      i_array;
    tr_do_cache_mem_test_t *test_params = (tr_do_cache_mem_test_t *)data;
#ifdef BCM_DNX_SUPPORT
    uint32 mux_switched = 0;
#endif

    SOC_INIT_FUNC_DEFS;

    test_params->stat_mem_total_cnt++;

#if defined(BCM_SAND_SUPPORT)
    if ((SOC_IS_ARAD(unit)) || (SOC_IS_DNXF(unit)) || (SOC_IS_DNX(unit)))
    {
        if(!soc_mem_is_cachable(unit, mem))
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: WRITE: non-cachable memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));
            test_params->stat_mem_not_tested_cnt++;
            SOC_EXIT;
        }
    }
#endif

    if (SOC_MEM_IS_ARRAY(unit,mem))
    {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    }

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    /*The following statement will be removed after SDK-91354 will be fixed.*/
    if (mem == SER_ACC_TYPE_MAPm)
    {
        SOC_EXIT;
    }
#endif

#if defined(BCM_PETRA_SUPPORT)
    /*The following statement will be removed after SDK-91354 will be fixed.*/
    if (mem == EDB_LINK_LAYER_OR_ARP_NEW_FORMATm || mem == EDB_PROTECTION_ENTRYm || mem == EDB_TRILL_FORMAT_FULL_ENTRYm || mem == EDB_TRILL_FORMAT_HALF_ENTRYm || mem == KAPS_TCMm)
    {
        SOC_EXIT;
    }
#endif

#ifdef BCM_SAND_SUPPORT
    if (SOC_MEM_IS_ALIAS(unit, mem)) {
        SOC_EXIT;
    }

    
    if (mem == KAPS_RPB_TCAM_CPU_COMMANDm ||
        mem == KAPS_RPB_TCAM_HIT_INDICATIONm ||
        mem == OAMP_FLEX_VER_MASK_TEMPm ||
        mem == SCH_FORCE_STATUS_MESSAGEm ||
        mem == SCH_SCHEDULER_INITm)
    {
        SOC_EXIT;
    }
#if defined(BCM_PETRA_SUPPORT)
    if (mem == PPDB_B_LARGE_EM_FORMAT_0_TYPE_0m)
    {
        SOC_EXIT;
    }
#endif
    
    if(test_params->write_type == cache_mem_test_write_type_dma &&
        soc_mem_entry_words(unit, mem) + 2 > CMIC_SCHAN_WORDS(unit)) {
        SOC_EXIT;
    }

    /* The following memories are Dynamic memory in QUX, we will skip them */
    if (cache_mem_dynamic_check(unit,mem)){
        SOC_EXIT;
    }

#if defined(BCM_PETRA_SUPPORT)
    /* Table IPSEC_TUNNEL_DATA is not SERrable and has no cache */
    if (SOC_IS_QUX(unit))
    {
        if (mem == IPSEC_TUNNEL_DATAm)
        {
            SOC_EXIT;
        }
    }
#endif

    if (test_params->test_type == cache_mem_test_type_all_mems)
    {
        if (cache_mem_cache_check_skip(unit, mem))
        {
            test_params->stat_mem_not_tested_cnt++;
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: WRITE: force skipped memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));
            test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_FORCE_SKIP_FLAG;
            SOC_EXIT;
        }
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)){
        rv = dnx_egr_esb_nif_calendar_mux_switch_start(unit, mem, SOC_MEM_BLOCK_MIN(unit, mem), &mux_switched);
        if (rv != SOC_E_NONE) {
            LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "dnx_egr_esb_nif_calendar_mux_switch_start memory %s failed\r\n"), SOC_MEM_NAME(unit, mem)));
        }
    }
#endif

    switch (test_params->write_type)
    {
        case cache_mem_test_write_type_dma:
        {
            for (i_array = start_array_index; i_array <= end_array_index; i_array++)
            {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: WRITE DMA: write memory %d %s \n"), mem, SOC_MEM_NAME(unit, mem)));
                test_params->mem_id = mem;
#ifdef BCM_PETRA_SUPPORT
                if (SOC_IS_ARAD(unit))
                {
                    /* DMA write via caching currently Implemented only in Arad.*/
                    rv = arad_fill_table_with_variable_values_by_caching(unit, mem, i_array, MEM_BLOCK_ALL, -1, -1,
                            cache_mem_test_fill_values_dma_callback, test_params);
                }
                else
#endif
                {
                    /* Fill the values in the non-dma way - as we do for schan writes */
                    
                    rv = cache_mem_test_fill_values_soc_mem_write_callback( unit, mem, i_array, data);
                }
            }
            break;
        }
        case cache_mem_test_write_type_schan:
        {
            for (i_array = start_array_index; i_array <= end_array_index; i_array++)
            {
                rv = cache_mem_test_fill_values_soc_mem_write_callback( unit, mem, i_array, data);
            }
        }
    }

    if (rv != SOC_E_NONE) {
        test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_WRITE_FAILUE_FLAG;
        test_params->stat_mem_fail_cnt++;
        test_params->result |= rv;
    } else {
        test_params->stat_mem_succeed_cnt++;
    }

exit:
    SOC_FUNC_RETURN; 
}

int
cache_mem_test_read_and_compare_wo_expected(int unit, soc_mem_t mem, void* data)
{
    soc_error_t rv = SOC_E_NONE;
    uint32      start_array_index = 0, end_array_index = 0, i_array;
    uint32      start_index, end_index, index;
    uint32      entry_index = 0;
    uint32      read_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32      read_cache_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32      mem_field_mask[SOC_MEM_ENTRY_LENGTH_MAX];
    tr_do_cache_mem_test_t *test_params = (tr_do_cache_mem_test_t *)data;
    uint32                  entry_dw;
#ifdef BCM_DNX_SUPPORT
    uint32 mux_switched = 0;
#endif
    test_params->stat_mem_total_cnt++;

#ifdef BCM_SAND_SUPPORT
    if ((SOC_IS_ARAD(unit)) || (SOC_IS_DNXF(unit)) || (SOC_IS_DNX(unit)))
    {
        if (SOC_MEM_IS_ALIAS(unit, mem)) 
        {
            test_params->stat_mem_not_tested_cnt++;
            test_params->stat_mem_succeed_cnt--;
            test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_ALIAS_SKIP_FLAG;
            goto done;
	    }

        if(!soc_mem_is_cachable(unit, mem))
        {
            /* skip memory*/
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ:  non-cachable memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));
            test_params->stat_mem_not_tested_cnt++;
            test_params->stat_mem_succeed_cnt--;
            test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_NON_CACHEABLE_SKIP_FLAG;
            goto done;
        }
        if (!soc_mem_cache_get(unit, mem, MEM_BLOCK_ALL)) {
            test_params->stat_mem_not_tested_cnt++;
            test_params->stat_mem_succeed_cnt--;
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ: uncached memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));

            test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_NOT_SHADOWED_SKIP_FLAG;
            goto done;
        }

        if (test_params->test_type == cache_mem_test_type_all_mems)
        {
            if (cache_mem_cache_check_skip(unit, mem))
            {
                test_params->stat_mem_not_tested_cnt++;
                test_params->stat_mem_succeed_cnt--;

                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ: force skipped memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));
                test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_FORCE_SKIP_FLAG;
                goto done;
            }
        }
    }
    SOC_MEM_ALIAS_TO_ORIG(unit, mem);
#endif

    entry_dw        = soc_mem_entry_words(unit, mem);
    start_index     = parse_memory_index(unit, mem, "min");
    end_index       = parse_memory_index(unit, mem, "max");
    cache_mem_test_create_mask(unit, mem, sizeof(mem_field_mask), mem_field_mask);

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ read memory %d %s, num_of_entries %d  \n"),
             mem, SOC_MEM_NAME(unit, mem), end_index - start_index + 1/*table_size*/));

    if (SOC_MEM_IS_ARRAY(unit,mem)) {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    }

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    /*The following statement will be removed after SDK-91354 will be fixed.*/
    if (mem == SER_ACC_TYPE_MAPm)
    {
        return 0;
    }
#endif

#if defined(BCM_PETRA_SUPPORT)
    /*The following statement will be removed after SDK-91354 will be fixed.*/
    if (mem == EDB_LINK_LAYER_OR_ARP_NEW_FORMATm || mem == EDB_PROTECTION_ENTRYm || mem == EDB_TRILL_FORMAT_FULL_ENTRYm || mem == EDB_TRILL_FORMAT_HALF_ENTRYm || mem == KAPS_TCMm)
    {
        return 0;
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
	{
        rv = dnx_egr_esb_nif_calendar_mux_switch_start(unit, mem, SOC_MEM_BLOCK_MIN(unit, mem), &mux_switched);
        if (rv != SOC_E_NONE)
		{
            LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "dnx_egr_esb_nif_calendar_mux_switch_start memory %s failed\r\n"), SOC_MEM_NAME(unit, mem)));
        }
	}
#endif
    for (i_array = start_array_index; i_array <= end_array_index; i_array++)
    {
        for (index = start_index; index <= end_index; index++ )
        {
            rv = soc_mem_array_read(unit, mem, i_array, MEM_BLOCK_ANY, index, read_cache_value);
            rv |= soc_mem_array_read_flags(unit, mem, i_array, MEM_BLOCK_ANY, index, read_value, SOC_MEM_DONT_USE_CACHE);
            if (rv != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST:Read FAILED rv %d: read_value 0x%X cache_value 0x%X mask 0x%X mem %d %s, index %d, array %d\n"),
                                        rv, read_value[entry_index], read_cache_value[entry_index], mem_field_mask[entry_index], mem, SOC_MEM_NAME(unit, mem), index, i_array));
                test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_READ_FAILUE_FLAG;
                goto done;
            }

            for (entry_index = 0; entry_index < entry_dw; entry_index++)
            {
                if ((read_value[entry_index] & mem_field_mask[entry_index]) == (read_cache_value[entry_index] & mem_field_mask[entry_index]))
                {
                    LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ: read_value 0x%X cache_value 0x%X, mask 0x%X, mem %d, index %d, entry_index, %d array %d, len %d\n"),
                            read_value[entry_index] & mem_field_mask[entry_index],
                            read_cache_value[entry_index] & mem_field_mask[entry_index],
                            mem_field_mask[entry_index],
                            mem, index, entry_index, i_array, entry_dw));
                }
                else
                {
                    LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: COMPARE FAILED: read_value 0x%X, cache_value 0x%X, (0x%X != 0x%X), mask 0x%X mem %d, index %d, entry_index %d array %d\n"),
                            read_value[entry_index] & mem_field_mask[entry_index],
                            read_cache_value[entry_index] & mem_field_mask[entry_index],
                            read_value[entry_index], read_cache_value[entry_index],
                            mem_field_mask[entry_index],
                            mem, index, entry_index, i_array));
                    test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_COMPARE_FAILUE_FLAG;
                    rv = SOC_E_LIMIT /* Unknown error */;
                    goto done;
                }
            }
        }
    }

done:
    if (rv != SOC_E_NONE) {
        test_params->stat_mem_fail_cnt++;
        test_params->result |= rv;
    } else {
        test_params->stat_mem_succeed_cnt++;
    }

    return rv;

}

int
cache_mem_test_read_and_compare(SOC_SAND_IN int unit, SOC_SAND_IN soc_mem_t mem, void* data)
{
    soc_error_t rv = SOC_E_NONE;
    uint32      start_array_index = 0, end_array_index = 0, i_array;
    uint32      start_index, end_index, index;
    uint32      entry_index = 0;
    uint32      read_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32      read_cache_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32      expected_value[SOC_MEM_ENTRY_LENGTH_MAX];
    uint32      mem_field_mask[SOC_MEM_ENTRY_LENGTH_MAX];
    tr_do_cache_mem_test_t *test_params = (tr_do_cache_mem_test_t *)data;
    uint32                  entry_dw;
    soc_mem_t               mem_orig = mem;
#ifdef BCM_DNX_SUPPORT
    uint32 mux_switched = 0;
#endif

    SOC_MEM_ALIAS_TO_ORIG(unit, mem_orig);

    SOC_REG_ABOVE_64_CLEAR(read_value);
    SOC_REG_ABOVE_64_CLEAR(read_cache_value);
    SOC_REG_ABOVE_64_CLEAR(expected_value);
    SOC_REG_ABOVE_64_CLEAR(mem_field_mask);

    if (test_params->test_part != cache_mem_test_full) {
        test_params->stat_mem_total_cnt++;
    }

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    /*The following statement will be removed after SDK-91354 will be fixed.*/
    if (mem == SER_ACC_TYPE_MAPm)
    {
        return 0;
    }
#endif

#if defined(BCM_PETRA_SUPPORT)
    /*The following statement will be removed after SDK-91354 will be fixed.*/
    if (mem == EDB_LINK_LAYER_OR_ARP_NEW_FORMATm || mem == EDB_PROTECTION_ENTRYm || mem == EDB_TRILL_FORMAT_FULL_ENTRYm || mem == EDB_TRILL_FORMAT_HALF_ENTRYm || mem == KAPS_TCMm)
    {
        return 0;
    }
#endif

#ifdef BCM_SAND_SUPPORT
    if (SOC_MEM_IS_ALIAS(unit, mem)) {
        return 0;
    }

    
    if (mem == KAPS_RPB_TCAM_CPU_COMMANDm ||
        mem == KAPS_RPB_TCAM_HIT_INDICATIONm ||
        mem == OAMP_FLEX_VER_MASK_TEMPm ||
        mem == SCH_FORCE_STATUS_MESSAGEm ||
        mem == SCH_SCHEDULER_INITm)
    {
        return 0;
    }
#if defined(BCM_PETRA_SUPPORT)
    if (mem == PPDB_B_LARGE_EM_FORMAT_0_TYPE_0m)
    {
        return 0;
    }
#endif

    
    if(test_params->write_type == cache_mem_test_write_type_dma &&
        soc_mem_entry_words(unit, mem) + 2 > CMIC_SCHAN_WORDS(unit)) {
        return 0;
    }

#if defined(BCM_PETRA_SUPPORT)
    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) &&
        (mem == SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR__CALm ||
         mem == SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm ||
         mem == SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm))
    {
        return 0;
    }
#endif

    /* The following memories are Dynamic memory in QUX, we will skip them */
    if (cache_mem_dynamic_check(unit,mem)){
        return 0;
    }

#if defined(BCM_PETRA_SUPPORT)
    /* Table IPSEC_TUNNEL_DATA is not SERrable and has no cache */
    if (SOC_IS_QUX(unit))
    {
        if (mem == IPSEC_TUNNEL_DATAm)
        {
            return 0;
        }
    }
#endif
    if (SOC_IS_Q2A(unit))
    {
        if (mem == IPS_MEM_2E8000m)
        {
            return 0;
        }
    }

    if (test_params->test_type == cache_mem_test_type_all_mems)
    {
        if (cache_mem_cache_check_skip(unit, mem))
        {
            test_params->stat_mem_not_tested_cnt++;
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: WRITE: force skipped memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));
            test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_FORCE_SKIP_FLAG;
            return 0;
        }
    }
#endif


#if defined(BCM_SAND_SUPPORT)
    if ((SOC_IS_ARAD(unit)) || (SOC_IS_DNXF(unit)) || (SOC_IS_DNX(unit)))
    {
        if(!soc_mem_is_cachable(unit, mem))
        {
            /* skip memory*/
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ: uncachable memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));
            if (test_params->test_part != cache_mem_test_full) {
                test_params->stat_mem_not_tested_cnt++;
                test_params->stat_mem_succeed_cnt--;
            }
            goto done;
        }

        if (!soc_mem_cache_get(unit, mem, MEM_BLOCK_ALL)) {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ: uncached memory %d %s\n"), mem, SOC_MEM_NAME(unit, mem)));
            if (test_params->test_part != cache_mem_test_full) {
                test_params->stat_mem_not_tested_cnt++;
                test_params->stat_mem_succeed_cnt--;
            }
            goto done;
        }
    }
#endif

    entry_dw        = soc_mem_entry_words(unit, mem);
    start_index     = parse_memory_index(unit, mem, "min");
    end_index       = parse_memory_index(unit, mem, "max");
    cache_mem_test_create_mask(unit, mem, sizeof(mem_field_mask), mem_field_mask);

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ read memory %d %s, num_of_entries %d  \n"),
             mem, SOC_MEM_NAME(unit, mem), end_index - start_index + 1/*table_size*/));

    if (SOC_MEM_IS_ARRAY(unit,mem)) {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    }
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        rv = dnx_egr_esb_nif_calendar_mux_switch_start(unit, mem, SOC_MEM_BLOCK_MIN(unit, mem), &mux_switched);
        if (rv != SOC_E_NONE)
        {
            LOG_WARN(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "dnx_egr_esb_nif_calendar_mux_switch_start memory %s failed\r\n"), SOC_MEM_NAME(unit, mem)));
        }
	}
#endif
    for (i_array = start_array_index; i_array <= end_array_index; i_array++)
    {
        for (index = start_index; index <= end_index; index++ )
        {
            rv = soc_mem_array_read(unit, mem, i_array, MEM_BLOCK_ANY, index, read_cache_value);
            rv |= soc_mem_array_read_flags(unit, mem, i_array, MEM_BLOCK_ANY, index, read_value, SOC_MEM_DONT_USE_CACHE);
            if (rv != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST:Read FAILED rv %d:  mem(%d : %s) read_value 0x%X cache_value 0x%X mask 0x%X, index %d, array %d\n"),
                                        rv, mem, SOC_MEM_NAME(unit, mem), read_value[entry_index], read_cache_value[entry_index], mem_field_mask[entry_index], index, i_array));
                test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_READ_FAILUE_FLAG;
                goto done;
            }

            for (entry_index = 0; entry_index < entry_dw; entry_index++)
            {
                expected_value[entry_index] = cache_mem_test_generate_value(unit,  mem_orig, i_array, index, test_params->write_value_pattern) & mem_field_mask[entry_index];

                if ((read_value[entry_index] & mem_field_mask[entry_index] ) == (expected_value[entry_index] & mem_field_mask[entry_index]) &&
                        (read_cache_value[entry_index] & mem_field_mask[entry_index] ) == (expected_value[entry_index] & mem_field_mask[entry_index]))
                {
                    LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: READ: read_value 0x%X cache_value 0x%X == expected_value 0x%X, (0x%X == 0x%X), mask 0x%X, mem %d, index %d, entry_index, %d array %d, len %d\n"),
                            read_value[entry_index] & mem_field_mask[entry_index],
                            read_cache_value[entry_index] & mem_field_mask[entry_index],
                            expected_value[entry_index] & mem_field_mask[entry_index],
                            read_value[entry_index], expected_value[entry_index],
                            mem_field_mask[entry_index],
                            mem, index, entry_index, i_array, entry_dw));
                }
                else
                {
                    LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: COMPARE FAILED: mem(%d : %s) read_value 0x%X, cache_value 0x%X, expected_value 0x%X, (0x%X != 0x%X != 0x%X), mask 0x%X, index %d, entry_index %d array %d\n"),
                            mem, SOC_MEM_NAME(unit, mem), read_value[entry_index] & mem_field_mask[entry_index],
                            read_cache_value[entry_index] & mem_field_mask[entry_index],
                            expected_value[entry_index] & mem_field_mask[entry_index],
                            read_value[entry_index], read_cache_value[entry_index], expected_value[entry_index],
                            mem_field_mask[entry_index],
                            index, entry_index, i_array));
                    test_params->mem_test_result_flag[mem] |= CACHE_MEM_TEST_COMPARE_FAILUE_FLAG;
                    rv = SOC_E_LIMIT /* Unknown error */;
                    goto done;
                }
            }
        }
    }
done:
    if (rv != SOC_E_NONE) {
        test_params->stat_mem_fail_cnt++;
        test_params->stat_mem_succeed_cnt--;
        test_params->result |= rv;
    } else {
        if (test_params->test_part != cache_mem_test_full) {
            test_params->stat_mem_succeed_cnt++;
        }
    }

    return rv;

}

int
cache_mem_test_write_read_and_compare(SOC_SAND_IN int unit, SOC_SAND_IN soc_mem_t mem, void* data) {
    if (cache_mem_test_write_iter_callback(unit, mem, data) < 0) {
        return BCM_E_INTERNAL;
    }
    if (cache_mem_test_read_and_compare(unit, mem, data) < 0) {
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

void do_cache_mem_test_print_usage(void)
{
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("Usage for Cache Memory Test: \n"
            "test_type=X         where X=0 for specific memory, X=1 for all memories\n"
            "write_type=X        where X=0 for DMA write,       X=1 for SCHAN write\n"
            "pattern=X           where X=0 for All-Ones,  X=1 for All-Zeroes,      X=2 for Incremental,    X=3 for Smart pattern\n"
            "part=X              where X=0 for Full-Test, X=1 for Only-Write-Part, X=2 for Only-Read-Part, X=3 for Cache VS HW test X=3 for Broadcast cahce test\n")));
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("mem_id=X            where X is memory id for specific memory test (applicable with test_type=0)\n")));
}

int
do_cache_mem_test_init(int unit, args_t *a, void **p)
{

    cmd_result_t            rv      = CMD_OK;
    tr_do_cache_mem_test_t  *test_params;
    parse_table_t parse_table;
    char *mem;
    int parse_arg_init = 0;

    *p = NULL;
    if ((test_params = sal_alloc(sizeof (tr_do_cache_mem_test_t), "cache_mem_test")) == 0)
    {
        rv = CMD_FAIL;
        goto done;
    }
    sal_memset(test_params, 0, sizeof (*test_params));
    *p = (void *) test_params;

    /* Set default params */
    test_params->test_type                  = cache_mem_test_type_single;
    test_params->write_type                 = cache_mem_test_write_type_schan;
    test_params->write_value_pattern        = cache_mem_test_write_value_pattern_incremental;
    test_params->test_part                  = cache_mem_test_full;
    test_params->mem_id                     = INVALIDm;
    test_params->result                     = CMD_OK;

    /** seting default values **/
    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table,  "test_type",  PQ_INT,         (void *)(test_params->test_type), &(test_params->test_type), NULL);
    parse_table_add(&parse_table,  "write_type", PQ_INT,         (void *)(test_params->write_type), &(test_params->write_type), NULL);
    parse_table_add(&parse_table,  "pattern",    PQ_INT,         (void *)(test_params->write_value_pattern), &(test_params->write_value_pattern), NULL);
    parse_table_add(&parse_table,  "part",       PQ_INT | PQ_DFL,(void *)(test_params->test_part),  &(test_params->test_part), NULL);
    parse_table_add(&parse_table,  "mem_id",     PQ_STRING,      "",    &mem,               NULL);

    parse_table_add(&parse_table,  "access",     PQ_STRING,      "",    &(test_params->access_type_parse),  NULL);
    parse_table_add(&parse_table,  "Block",      PQ_STRING,      "",    &(test_params->block_name_parse),  NULL);
    parse_table_add(&parse_table,  "regmem",     PQ_STRING,      "",    &(test_params->regmem_name_parse),  NULL);
    
    parse_arg_init = 1;


    /** parsing arguments and checking seting needed values, checking validity of given options **/
    if (parse_arg_eq(a, &parse_table) < 0) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s: Invalid option: %s\n"), ARG_CMD(a), ARG_CUR(a)));
        /* print usage */
        goto usage;
    }

    /** making sure no extra options were given **/ 
    if (ARG_CNT(a) != 0) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s: extra options starting with \"%s\"\n"), ARG_CMD(a), ARG_CUR(a)));
        /* print usage */
        goto usage;
    }

#if defined(BCM_DNXC_SUPPORT) && defined(BCM_ACCESS_SUPPORT)
    if (ACCESS_IS_INITIALIZED(unit))
    {
        rv = access_do_cache_mem_test_init(unit, test_params, mem);
        if (rv == CMD_USAGE)
        {
            goto usage;
        }
        goto done;
    }
#endif /* BCM_ACCESS_SUPPORT */

    /* Get Test Type */
    if (test_params->test_type > cache_mem_test_type_all_mems || test_params->test_type < cache_mem_test_type_single)
    {
        goto usage;
    }

    /* Get Write Type */
    if (test_params->write_type > cache_mem_test_write_type_schan || test_params->write_type < cache_mem_test_write_type_dma)
    {
        goto usage;;
    }

    /* Get Pattern */
    if (test_params->write_value_pattern > cache_mem_test_write_value_pattern_smart || test_params->write_value_pattern < cache_mem_test_write_value_pattern_all_ones)
    {
        goto usage;
    }

    if (test_params->test_part > cache_mem_test_cache_broadcast || test_params->test_part < cache_mem_test_full)
    {
        goto usage;
    }

    /* Get Memory ID */
    if (test_params->test_type != cache_mem_test_type_all_mems)
    {
        if (parse_memory_name(unit, (soc_mem_t *)&(test_params->mem_id), mem, NULL, NULL) < 0) 
        {
            /* check memory if memory id */
            test_params->mem_id = sal_ctoi(mem, NULL);
        }
        if (!SOC_MEM_IS_VALID(unit, test_params->mem_id))
        {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"Memory %s is invalid\n"), mem));
            test_params->mem_id = INVALIDm;
            goto usage;
        }
    }
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit))
    {
        bcm_common_linkscan_enable_set(unit,0);
        soc_counter_stop(unit);
        rv = bcm_dpp_counter_bg_enable_set(unit, FALSE);
        if (CMD_OK == rv)
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST:unit %d counter processor background accesses suspended\n"), unit));
        }
        else
        {
            LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("CACHE_MEM_TEST: unit %d counter processor background access suspend failed: %d (%s)\n"),
                    unit, rv, _SHR_ERRMSG(rv)));
        }
        
        if (test_params->test_part != cache_mem_test_cache_only)
        {
            if ((rv |= soc_dpp_device_reset(unit, SOC_DPP_RESET_MODE_REG_ACCESS, SOC_DPP_RESET_ACTION_INOUT_RESET)) < 0)
            {
                LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "CACHE_MEM_TEST: unit %d ERROR: Unable to reinit  \n"), unit));
            }
        }
        goto done;
    }
#endif

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        goto done;
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        bcm_common_linkscan_enable_set(unit,0);
        soc_counter_stop(unit);

        if (test_params->test_part != cache_mem_test_cache_only && test_params->test_part != cache_mem_test_read_only)
        {
            /** mask all interrupts */
            bcm_switch_event_control_t type;

            type.action = bcmSwitchEventMask;
            type.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
            type.index = 0;

            if ((rv = bcm_dnx_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1)) < 0)
            {
                LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "CACHE_MEM_TEST: unit %d ERROR: Unable to mask all interrupts\n"), unit));
            }
        }
        goto done;
    }
#endif

usage:
    do_cache_mem_test_print_usage();
    rv = CMD_USAGE;

done:
    if (rv != CMD_OK && test_params != NULL)
        if (test_params)
            test_params->result |= rv;
    if (parse_arg_init)
    {
        parse_arg_eq_done(&parse_table);
    }

    return rv;
}



int
do_cache_mem_test_done(int unit,  void *p)
{
    cmd_result_t rv;
    tr_do_cache_mem_test_t  *test_params = (tr_do_cache_mem_test_t*)p;

    rv = test_params->result;
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "CACHE_MEM_TEST: unit %d Done: Total %u, Failed %u, Not tested %u, Succeeded %u\n"), 
                                 unit, 
                                 test_params->stat_mem_total_cnt, 
                                 test_params->stat_mem_fail_cnt, 
                                 test_params->stat_mem_not_tested_cnt, 
                                 test_params->stat_mem_succeed_cnt));
    sal_free (p);
    return rv;
}

int
do_cache_mem_test(int unit,  args_t *a, void* tr_do_cache_mem_test)
{
    soc_error_t rv = SOC_E_NONE;
    tr_do_cache_mem_test_t *test_params = tr_do_cache_mem_test;

#ifdef BCM_SAND_SUPPORT
    int cache_enable = 1;
#endif

#ifdef BCM_PETRA_SUPPORT
    uint32 val_bak;
#endif
#ifdef BCM_DNX_SUPPORT
    uint32 eps_special_clock_control = 0;
#endif
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_JERICHO(unit) &&
        (SOC_REG_IS_VALID(unit, FDA_ENABLE_DYNAMIC_MEMORY_ACCESSr)) &&
        ((test_params->test_type == cache_mem_test_type_all_mems) || (test_params->mem_id == FDA_FDA_MCm))) {
        READ_FDA_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, &val_bak);
        /* Allow writes to FDA_FDA_MCm, and stay with this configuration for later writes to this memory */
        WRITE_FDA_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1);
    }

    /* Jericho+: indirect_wr_mask register masking mechanism is opposite for IHB_FEC_PATH_SELECT/IHP_VTT_PATH_SELECT table */
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        WRITE_IHB_INDIRECT_WR_MASKr(unit,SOC_CORE_ALL, 0);
        WRITE_IHP_INDIRECT_WR_MASKr(unit,SOC_CORE_ALL, 0);
    }
#endif

#if defined(BCM_DNXC_SUPPORT) && defined(BCM_ACCESS_SUPPORT)
    if (ACCESS_IS_INITIALIZED(unit))
    {
        rv = access_do_cache_mem_test(unit, test_params);
        return rv;
    }
#endif /* BCM_ACCESS_SUPPORT */

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_J2X(unit))
    {
        /* disable ecc indication on sbus for EPS block, because some memories used by traffic */
        READ_EPS_SPECIAL_CLOCK_CONTROLSr(unit, SOC_CORE_ALL, &eps_special_clock_control);\
        soc_reg_field32_modify(unit, EPS_SPECIAL_CLOCK_CONTROLSr, SOC_CORE_ALL, DISABLE_ECC_INDICATION_ON_SBUSf, 1);
    }
#endif

    switch(test_params->test_type)
    {
        case cache_mem_test_type_all_mems:  /* Run tests for all memories */
        {
#if defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
            if (test_params->test_part == cache_mem_test_cache_broadcast)
            {
                rv = cache_memory_test_cache_broadcast(unit, test_params);
                break;
            }
#endif

#ifdef BCM_SAND_SUPPORT
            /* Turn on cache memory for all tables */
            if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_write_only)
            {
                cache_enable = TRUE;
                if (soc_mem_iterate(unit, sand_tbl_mem_cache_mem_set, &cache_enable) < 0)
                {
                    LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "CACHE_MEM_TEST: unit %d cache enable failed\n"), unit));
                }
            }
#endif

            /* Do the write/read part of the test */
            if (test_params->test_part == cache_mem_test_full)
            {
                if ((soc_mem_iterate(unit, cache_mem_test_write_read_and_compare, tr_do_cache_mem_test)) < 0) {
                    rv = SOC_E_INTERNAL;
                }
                if (test_params->result) {
                    break;
                }
            }
            if (test_params->test_part == cache_mem_test_write_only)
            {
                if ((soc_mem_iterate(unit, cache_mem_test_write_iter_callback, tr_do_cache_mem_test)) < 0) {
                    rv = SOC_E_INTERNAL;
                }
                if (test_params->result) {
                    break;
                }
            }
            /* Do the read part of the test */
            if (test_params->test_part == cache_mem_test_read_only)
            {
                if ((soc_mem_iterate(unit, cache_mem_test_read_and_compare, tr_do_cache_mem_test)) < 0) {
                    rv = SOC_E_INTERNAL;
                }
                if (test_params->result) {
                    break;
                }
            }
            /* Do the HW vs Cache part of the test */
            if (test_params->test_part == cache_mem_test_cache_only)
            {
                if ((soc_mem_iterate(unit, cache_mem_test_read_and_compare_wo_expected, tr_do_cache_mem_test)) < 0) {
                    rv = SOC_E_INTERNAL;
                }
                if (test_params->result) {
                    break;
                }
            }

#ifdef BCM_SAND_SUPPORT
            /* Turn off cache memory for all tables */
            if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_read_only)
            {
                cache_enable = FALSE;
                if (soc_mem_iterate(unit, sand_tbl_mem_cache_mem_set, &cache_enable) < 0)
                    LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "CACHE_MEM_TEST: unit %d cache disable failed\n"), unit));
            }
#endif
            break;

        }
        case cache_mem_test_type_single:    /* Run test for specific memory */
        {
#ifdef BCM_SAND_SUPPORT
            /* Turn on cache memory for all tables */
            if (test_params->test_part != cache_mem_test_cache_only)
            {
                cache_enable = TRUE;
                sand_tbl_mem_cache_mem_set(unit, test_params->mem_id, &cache_enable);
            }
#endif
            /* Do the write part of the test */
            if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_write_only)
            {
                rv = cache_mem_test_write_iter_callback(unit, test_params->mem_id, tr_do_cache_mem_test);
                if (rv) {
                    break;
                }
            }
            /* Do the read part of the test */
            if (test_params->test_part == cache_mem_test_full || test_params->test_part == cache_mem_test_read_only)
            {
                rv = cache_mem_test_read_and_compare(unit, test_params->mem_id, tr_do_cache_mem_test);
                if (rv) {
                    break;
                }
            }
            /* Do the HW vs Cache part of the test */
            if (test_params->test_part == cache_mem_test_cache_only)
            {
                rv = cache_mem_test_read_and_compare_wo_expected(unit, test_params->mem_id, tr_do_cache_mem_test);
                if (rv) {
                    break;
                }
            }
#ifdef BCM_SAND_SUPPORT
            /* Turn off cache memory for all tables */
            if (test_params->test_part != cache_mem_test_cache_only)
            {
                cache_enable = FALSE;
                sand_tbl_mem_cache_mem_set(unit, test_params->mem_id, &cache_enable);
            }
#endif
            break;
        }
        case cache_mem_test_type_access_mode:
        {
            break;
        }
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_J2X(unit))
    {
        WRITE_EPS_SPECIAL_CLOCK_CONTROLSr(unit, SOC_CORE_ALL, eps_special_clock_control);\
    }
#endif

#ifdef BCM_PETRA_SUPPORT
    /* restore the value*/
    if (SOC_IS_JERICHO(unit) &&
        (SOC_REG_IS_VALID(unit, FDA_ENABLE_DYNAMIC_MEMORY_ACCESSr)) &&
        ((test_params->test_type == cache_mem_test_type_all_mems) || (test_params->mem_id == FDA_FDA_MCm))) {
        WRITE_FDA_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, val_bak);
    }
#endif

    if (rv != SOC_E_NONE)
        test_params->result |= rv;

    if (test_params->result) {
        test_error(unit, "Cache Memory Test Failed!\n");
    }

#ifdef BCM_SAND_SUPPORT
    if (SOC_IS_SAND(unit) && (test_params->test_type == cache_mem_test_type_all_mems))
    {
        cache_memory_test_result_dump(unit, test_params);
    }
#endif

    return rv;
}

int soc_mem_single_test(int unit, soc_mem_t mem)
{
    soc_error_t rv = SOC_E_NONE;

	tr_do_cache_mem_test_t *test_params = NULL;

    if ((test_params = sal_alloc(sizeof (tr_do_cache_mem_test_t), "cache_mem_test_single")) == 0)
    {
        rv = SOC_E_MEMORY;
        goto done;
    }
    sal_memset(test_params, 0, sizeof (*test_params));

	test_params->mem_id = mem;
	test_params->test_type = cache_mem_test_type_single;
	test_params->write_type = cache_mem_test_write_type_schan;
	test_params->write_value_pattern = cache_mem_test_write_value_pattern_smart;
	test_params->test_part = cache_mem_test_full;

	rv = do_cache_mem_test(unit,  NULL, test_params);

    rv = do_cache_mem_test_done(unit, test_params);
done:
     return rv;

}
#endif /*(BCM_ESW_SUPPORT) || (BCM_PETRA_SUPPORT) || (BCM_DFE_SUPPORT) */
