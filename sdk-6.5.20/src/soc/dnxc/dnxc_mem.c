
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXC MEM
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_MEM
#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_mem.h>
#include <soc/sand/sand_mem.h>
#include <soc/drv.h>
#include <soc/memory.h>
#include <soc/mem.h>
#include <shared/shrextend/shrextend_debug.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/drv.h>
#include <soc/dnx/dbal/dbal.h>
#endif 
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_fabric_cell.h>
#endif 

#ifdef JR2_CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#include <soc/dnxc/dnxc_crash_recovery.h>
#endif 


int
dnxc_fill_table_with_index_val(
    int unit,
    soc_mem_t mem,              
    soc_field_t field           
    )
{
    uint32 *entries = NULL, *entry = NULL;
    uint32 field_val[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 entry_words;
    int blk_index, blk_index_min, blk_index_max;
    int index, index_min, index_max;
    int array_index, array_index_min = 0, array_index_max = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(sand_alloc_dma_mem
                    (unit, 0, (void **) &entries, SOC_MEM_TABLE_BYTES(unit, mem), "dnxc_fill_table_with_index_val"));

    
    blk_index_min = SOC_MEM_BLOCK_MIN(unit, mem);
    blk_index_max = SOC_MEM_BLOCK_MAX(unit, mem);
#ifdef DNX_EMULATION_1_CORE
    if (SOC_IS_JERICHO2_ONLY(unit))
    {   
        switch (SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)))
        {       
            case SOC_BLK_KAPS:
            case SOC_BLK_IPPA:
            case SOC_BLK_IPPB:
            case SOC_BLK_IPPC:
            case SOC_BLK_IPPD:
            case SOC_BLK_IPPE:
            case SOC_BLK_IPPF:
            case SOC_BLK_CDUM:
            case SOC_BLK_MACT:
            case SOC_BLK_TCAM:
            case SOC_BLK_IQM:
            case SOC_BLK_ITPPD:
            case SOC_BLK_ITPP:
            case SOC_BLK_ETPPA:
            case SOC_BLK_ETPPB:
            case SOC_BLK_ETPPC:
            case SOC_BLK_ERPP:
            case SOC_BLK_EDB:
            case SOC_BLK_DQM:
            case SOC_BLK_CRPS:
            case SOC_BLK_MRPS:
            case SOC_BLK_EGQ:
            case SOC_BLK_EPNI:
            case SOC_BLK_EPRE:
            case SOC_BLK_ECGM:
            case SOC_BLK_CGM:
            case SOC_BLK_FDR:
            case SOC_BLK_IDB:
            case SOC_BLK_MCP:
            case SOC_BLK_OCB:
            case SOC_BLK_ILE:
            case SOC_BLK_SCH:
            case SOC_BLK_IPT:
            case SOC_BLK_IPS:
            case SOC_BLK_IRE:
            case SOC_BLK_NMG:
            case SOC_BLK_EPS:
            case SOC_BLK_FQP:
            case SOC_BLK_RQP:
            case SOC_BLK_PQP:
            case SOC_BLK_SQM:
            case SOC_BLK_SPB:
            case SOC_BLK_DDP:
            case SOC_BLK_PDM:
            case SOC_BLK_BDM:
            case SOC_BLK_SIF:
                blk_index_max = blk_index_min;  
                break;
            default:
                break;
        }
    }
#endif

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    entry_words = soc_mem_entry_words(unit, mem);

    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        array_index_max = (SOC_MEM_NUMELS(unit, mem) - 1) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
    }

    
    for (index = index_min; index <= index_max; index++)
    {
        entry = entries + (entry_words * index);
        if (field == INVALIDf)
        {
            *entry = index;
        }
        else
        {
            *field_val = index;
            soc_mem_field_width_fit_set(unit, mem, entry, field, field_val);
        }
    }

    
    for (blk_index = blk_index_min; blk_index <= blk_index_max; blk_index++)
    {
        for (array_index = array_index_min; array_index <= array_index_max; array_index++)
        {
            SHR_IF_ERR_EXIT(soc_mem_array_write_range
                            (unit, 0, mem, array_index, blk_index, index_min, index_max, entries));
        }
    }

exit:
    if (entries != NULL)
    {
        SHR_IF_ERR_EXIT(sand_free_dma_mem(unit, 0, (void **) &entries));
    }
    SHR_FUNC_EXIT;
}


#ifdef DNXC_RUNTIME_DEBUG
dnxc_runtime_debug_t dnxc_runtime_debug_per_device[SOC_MAX_NUM_DEVICES] = { {0} };


void
dnxc_runtime_debug_update_print_time(
    int unit,
    const char *string_to_print)
{
    unsigned secs_s, mins_s;    
    unsigned secs_l, mins_l;    
    dnxc_runtime_debug_t *debug = dnxc_runtime_debug_per_device + unit;
    sal_time_t current_secs = sal_time();
    if (debug->run_stage == dnxc_runtime_debug_state_loading)
    {
        debug->run_stage = dnxc_runtime_debug_state_initializing;
        debug->last_time = debug->start_time = sal_time();
    }

    secs_s = current_secs - debug->start_time;
    secs_l = current_secs - debug->last_time;
    mins_s = secs_s / 60;
    mins_l = secs_l / 60;
    debug->last_time = current_secs;
    LOG_INFO(BSL_LS_SOC_INIT, ("==> u:%d %s from start: %u:%2.2u:%2.2u  from last: %u:%2.2u:%2.2u\n",
                               unit, string_to_print, mins_s / 60, mins_s % 60, secs_s % 60, mins_l / 60, mins_l % 60,
                               secs_l % 60));
}
#endif 

#ifdef BCM_DNX_SUPPORT
int
dnx_mem_read_eps_protect_start(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 *eps_protect_flag,
    soc_reg_above_64_val_t * eps_shaper_set)
{
    soc_reg_t reg_shaper = EPS_EGRESS_SHAPER_ENABLE_SETTINGSr;
    soc_port_t reg_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(eps_protect_flag, _SHR_E_PARAM, "eps_protect_flag");
    SHR_NULL_CHECK(eps_shaper_set, _SHR_E_PARAM, "eps_shaper_set");

    if (SOC_IS_DNX(unit))
    {
        switch (mem)
        {
            case EPS_OTM_CALENDAR_CRDT_TABLEm:
            case EPS_OTM_HP_CRDT_TABLEm:
            case EPS_OTM_LP_CRDT_TABLEm:
            case EPS_CAL_CAL_INDX_MUXm:
            {
                reg_port = blk - SOC_MEM_BLOCK_MIN(unit, mem);
                SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_shaper, reg_port, 0, eps_shaper_set[0]));
                eps_protect_flag[0] = 1;
                SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify
                                (unit, reg_shaper, reg_port, 0, OTM_CRDT_TABLE_READ_ENf, 0x0));
                break;
            }
            case EPS_QP_CREDIT_TABLEm:
            {
                reg_port = blk - SOC_MEM_BLOCK_MIN(unit, mem);
                SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_shaper, reg_port, 0, eps_shaper_set[0]));
                eps_protect_flag[0] = 1;
                SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify
                                (unit, reg_shaper, reg_port, 0, QP_CRDT_TABLE_READ_ENf, 0x0));
                break;
            }
            case EPS_TCG_CREDIT_TABLEm:
            case EPS_TCG_CBMm:
            {
                reg_port = blk - SOC_MEM_BLOCK_MIN(unit, mem);
                SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_shaper, reg_port, 0, eps_shaper_set[0]));
                eps_protect_flag[0] = 1;
                SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify
                                (unit, reg_shaper, reg_port, 0, TCG_CRDT_TABLE_READ_ENf, 0x0));
                break;
            }
            case INVALIDm:
            {
                soc_reg_above_64_val_t data;

                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_shaper, 0, 0, eps_shaper_set[0]));
                eps_protect_flag[0] = 1;
                SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_shaper, 1, 0, eps_shaper_set[1]));
                eps_protect_flag[1] = 1;

                
                SOC_REG_ABOVE_64_COPY(data, eps_shaper_set[0]);
                soc_reg_above_64_field32_set(unit, reg_shaper, data, QP_CRDT_TABLE_READ_ENf, 0);
                soc_reg_above_64_field32_set(unit, reg_shaper, data, TCG_CRDT_TABLE_READ_ENf, 0);
                soc_reg_above_64_field32_set(unit, reg_shaper, data, OTM_CRDT_TABLE_READ_ENf, 0);
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_shaper, 0, 0, data));
                
                SOC_REG_ABOVE_64_COPY(data, eps_shaper_set[1]);
                soc_reg_above_64_field32_set(unit, reg_shaper, data, QP_CRDT_TABLE_READ_ENf, 0);
                soc_reg_above_64_field32_set(unit, reg_shaper, data, TCG_CRDT_TABLE_READ_ENf, 0);
                soc_reg_above_64_field32_set(unit, reg_shaper, data, OTM_CRDT_TABLE_READ_ENf, 0);
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_shaper, 1, 0, data));
                break;
            }
            default:
            {
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_mem_read_eps_protect_end(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 *eps_protect_flag,
    soc_reg_above_64_val_t * eps_shaper_set)
{
    soc_reg_t reg_shaper = EPS_EGRESS_SHAPER_ENABLE_SETTINGSr;
    soc_port_t reg_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(eps_protect_flag, _SHR_E_PARAM, "eps_protect_flag");
    SHR_NULL_CHECK(eps_shaper_set, _SHR_E_PARAM, "eps_shaper_set");

    if (SOC_IS_DNX(unit))
    {
        switch (mem)
        {
            case EPS_OTM_CALENDAR_CRDT_TABLEm:
            case EPS_OTM_HP_CRDT_TABLEm:
            case EPS_OTM_LP_CRDT_TABLEm:
            case EPS_CAL_CAL_INDX_MUXm:
            case EPS_QP_CREDIT_TABLEm:
            case EPS_TCG_CREDIT_TABLEm:
            case EPS_TCG_CBMm:
            {
                reg_port = blk - SOC_MEM_BLOCK_MIN(unit, mem);
                if (eps_protect_flag[0])
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_shaper, reg_port, 0, eps_shaper_set[0]));
                }
                break;
            }
            case INVALIDm:
            {
                if (eps_protect_flag[0])
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_shaper, 0, 0, eps_shaper_set[0]));
                }
                if (eps_protect_flag[1])
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_shaper, 1, 0, eps_shaper_set[1]));
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

#undef BSL_LOG_MODULE
