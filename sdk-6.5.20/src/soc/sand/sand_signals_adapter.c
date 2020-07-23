
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/system.h>

#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/pp_stage.h>

#include "sand_signals_internal.h"

#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

typedef struct {
    char*  start; 
    char*  data;
    uint32 length;
    int    nof;
} adapter_info_t;

#define ADAPTER_INFO_GET_UINT32(mc_var, mc_info)                                                               \
    if((mc_info)->length >= sizeof(uint32))                                                                    \
    {                                                                                                          \
        mc_var = bcm_ntohl(*((uint32 *)(mc_info)->data));                                                      \
        (mc_info)->length -= sizeof(uint32); (mc_info)->data += sizeof(uint32);                                \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Request 4(uint32) vs %u%s%s\n", (mc_info)->length, EMPTY, EMPTY); \
    }

#define ADAPTER_INFO_SKIP_MEM(mc_mem_length, mc_info)                                                          \
    if((mc_info)->length >= mc_mem_length)                                                                     \
    {                                                                                                          \
        (mc_info)->length -= mc_mem_length; (mc_info)->data += mc_mem_length;                                  \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Signal size:%u is more than buffer available:%u%s\n",             \
                                                    mc_mem_length, (mc_info)->length, EMPTY);                  \
    }

#define ADAPTER_INFO_GET_MEM(mc_mem, mc_mem_length, mc_info)                                                   \
    if(mc_mem_length > DSIG_MAX_SIZE_STR)                                                                      \
    {                                                                                                          \
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Signal size:%u is more than maximum supported:%u%s\n",            \
                                                mc_mem_length, DSIG_MAX_SIZE_STR, EMPTY);                      \
    }                                                                                                          \
    if((mc_info)->length >= mc_mem_length)                                                                     \
    {                                                                                                          \
        sal_memcpy(mc_mem, (mc_info)->data, mc_mem_length);                                                    \
        (mc_info)->length -= mc_mem_length; (mc_info)->data += mc_mem_length;                                  \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Signal size:%u is more than buffer available:%u%s\n",             \
                                                    mc_mem_length, (mc_info)->length, EMPTY);                  \
    }

static void
sand_adapter_clear_block_signals(
    int unit,
    pp_block_t *cur_pp_block)
{
    pp_stage_t *cur_pp_stage;
    int i_st;

    
    for(i_st = 0; i_st < cur_pp_block->stage_num; i_st++)
    {
        cur_pp_stage = &cur_pp_block->stages[i_st];

        if(cur_pp_stage->stage_debug_signals != NULL)
        {
            utilex_free(cur_pp_stage->stage_debug_signals);
            cur_pp_stage->stage_debug_signals = NULL;
            cur_pp_stage->stage_signal_num = 0;
        }
    }
    cur_pp_block->adapter_signal_num = 0;
}

void
sand_adapter_clear_signals(
    int unit)
{
    device_t * device;
    int i_block;

    if ((device = sand_signal_device_get(unit)) != NULL)
    {
        for (i_block = 0; i_block < device->block_num; i_block++)
        {
            sand_adapter_clear_block_signals(unit, &device->pp_blocks[i_block]);
        }
    }
}

shr_error_e
sand_adapter_get_stage_signals(
        device_t * device,
        int unit,
        char *block_n,
        pp_stage_t *pp_stage,
        int flags)
{
    adapter_info_t adapter_info;
    uint32 name_length;
    int retry_count = 3;
    int i_sig;
    pp_stage_t *subst_pp_stage = NULL, *cur_pp_stage = NULL;

    SHR_FUNC_INIT_VARS(unit);

    
    if(pp_stage->status & SIGNAL_STAGE_HAS_SUBSTITUTE)
    {
        cur_pp_stage = pp_stage->peer_stage;
    }
    else
    {
        cur_pp_stage = pp_stage;
    }

    sal_memset(&adapter_info, 0 , sizeof(adapter_info_t));

    if(cur_pp_stage->ms_id == DSIG_ILLEGAL_MS_ID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal MS_ID for stage:%s\n", cur_pp_stage->name);
    }

    while(retry_count)
    {
        SHR_IF_ERR_EXIT(adapter_get_signal(unit, cur_pp_stage->ms_id, &adapter_info.length, &adapter_info.start));
        if((adapter_info.length == 0) || (adapter_info.start == NULL))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No signal info received from adapter for stage:%s length:%u\n",
                                                                             cur_pp_stage->name, adapter_info.length);
        }
        adapter_info.data = adapter_info.start;
        
        ADAPTER_INFO_GET_UINT32(adapter_info.nof, &adapter_info);
        
        if((adapter_info.nof == 0) && (flags & SIGNALS_MATCH_RETRY))
        {   
            if(adapter_info.start != NULL)
            {
                sal_free(adapter_info.start);
                adapter_info.start = NULL;
            }
            retry_count--;
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Retry:%d for stage:%s buffer length:%u\n"),
                                                     3 - retry_count, cur_pp_stage->name, adapter_info.length));
        }
        else
        {   
            break;
        }
    }

    if(adapter_info.nof == 0)
    {
        
        SHR_EXIT();
    }

    

    if((cur_pp_stage->stage_debug_signals = utilex_alloc(sizeof(debug_signal_t) * adapter_info.nof)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of:%zu failed for debug signals\n",
                                                                        sizeof(debug_signal_t) * adapter_info.nof);
    }
    cur_pp_stage->stage_signal_num = 0;

    
    if(cur_pp_stage->status & SIGNAL_STAGE_IS_SUBSTITUTE)
    {
        if((subst_pp_stage = cur_pp_stage->peer_stage) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Peer stage should be initiated for:%s\n", cur_pp_stage->name);
        }
        
        if((subst_pp_stage->stage_debug_signals = utilex_alloc(sizeof(debug_signal_t) * adapter_info.nof)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of:%zu failed for debug signals\n",
                                                                            sizeof(debug_signal_t) * adapter_info.nof);
        }
        subst_pp_stage->stage_signal_num = 0;
    }

    for(i_sig = 0; i_sig < adapter_info.nof; i_sig++)
    {
        int signal_size;
        debug_signal_t *debug_signal, *asic_debug_signal;
        char signal_str[DSIG_MAX_SIZE_STR + 1];
        char signal_name[DSIG_MAX_SIZE_STR];
        ADAPTER_INFO_GET_UINT32(name_length, &adapter_info);
        ADAPTER_INFO_GET_MEM(signal_name, name_length, &adapter_info);
        if(name_length >= RHNAME_MAX_SIZE)
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Signal:%s is longer than max:%d\n"), signal_name, name_length));
        }
        
        ADAPTER_INFO_GET_UINT32(signal_size, &adapter_info);
        if(signal_size > DSIG_MAX_SIZE_BITS)
        {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Signal:%s-%s-%s size:%d is more than maximum:%d\n"),
                                           block_n, cur_pp_stage->name, signal_name, signal_size, DSIG_MAX_SIZE_BITS));
            ADAPTER_INFO_SKIP_MEM((signal_size + 3) / 4 + 2, &adapter_info);
            continue;
        }

        
        signal_name[name_length] = 0;
        
        if((asic_debug_signal = sand_adapter_extract_signal(unit, device, signal_name, block_n, cur_pp_stage->name)) != NULL)
        {
            debug_signal = &cur_pp_stage->stage_debug_signals[cur_pp_stage->stage_signal_num++];
        }
        else if((subst_pp_stage != NULL) && ((asic_debug_signal =
                    sand_adapter_extract_signal(unit, device, signal_name, block_n, subst_pp_stage->name)) != NULL))
        {
            debug_signal = &subst_pp_stage->stage_debug_signals[subst_pp_stage->stage_signal_num++];
        }
        else
        {
            debug_signal = &cur_pp_stage->stage_debug_signals[cur_pp_stage->stage_signal_num++];
        }

        debug_signal->size = signal_size;

        sal_memset(signal_str, 0, DSIG_MAX_SIZE_STR + 1);
        ADAPTER_INFO_GET_MEM(signal_str, (debug_signal->size + 3) / 4 + 2, &adapter_info);

        parse_long_integer(debug_signal->adapter_value, DSIG_MAX_SIZE_UINT32, signal_str);

        
        sal_snprintf(debug_signal->hw, RHSTRING_MAX_SIZE - 1, "%s", signal_name);
        
        if(asic_debug_signal != NULL)
        {
            sal_strncpy(debug_signal->to,        asic_debug_signal->to, RHNAME_MAX_SIZE - 1);
            sal_strncpy(debug_signal->from,      asic_debug_signal->from, RHNAME_MAX_SIZE - 1);
            sal_strncpy(debug_signal->attribute, asic_debug_signal->attribute, RHNAME_MAX_SIZE - 1);
            debug_signal->description = sand_signal_metadata_description(device, debug_signal->attribute);
            if(!ISEMPTY(asic_debug_signal->expansion))
            {
                sal_strncpy(debug_signal->expansion, asic_debug_signal->expansion, RHNAME_MAX_SIZE - 1);
            }
            if(!ISEMPTY(asic_debug_signal->resolution))
            {
                sal_strncpy(debug_signal->resolution, asic_debug_signal->resolution, RHNAME_MAX_SIZE - 1);
            }
        }
        else
        {
            
            if(cur_pp_stage->stage_to != NULL)
            {
                sal_strncpy(debug_signal->to,    cur_pp_stage->stage_to->name, RHNAME_MAX_SIZE - 1);
            }
            sal_strncpy(debug_signal->from,      cur_pp_stage->name, RHNAME_MAX_SIZE - 1);
            sal_strncpy(debug_signal->attribute, signal_name, RHNAME_MAX_SIZE - 1);
            
            if ((sand_signal_struct_get(device, signal_name, cur_pp_stage->name, NULL, block_n)) != NULL)
            {   
                sal_strncpy(debug_signal->expansion, signal_name, RHNAME_MAX_SIZE - 1);
            }
            if ((sand_signal_resolve_get(device, signal_name, cur_pp_stage->name, NULL, block_n)) != NULL)
            {   
                sal_strncpy(debug_signal->resolution, signal_name, RHNAME_MAX_SIZE - 1);
            }
        }
    }
    if(adapter_info.length != 0)
    {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("%d bytes left in %s buffer\n"), adapter_info.length, cur_pp_stage->name));
    }

exit:
    if(adapter_info.start != NULL)
        sal_free(adapter_info.start);
    SHR_FUNC_EXIT;
}

shr_error_e
sand_adapter_update_stages(
        int unit,
        device_t * device)
{
    adapter_info_t stage_info_m;
    int i_bl, i_st;
    int i_ms, name_length;
    pp_stage_t *pp_stage;
    pp_block_t *pp_block;
    uint32 ms_id;
    char adapter_name[RHNAME_MAX_SIZE];
    int stage_found;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_get_block_names(unit, (int *)&stage_info_m.length, &stage_info_m.start));
    if((stage_info_m.start == NULL) || (stage_info_m.length == 0))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Adapter Returned NULL buffer %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    stage_info_m.data = stage_info_m.start;
    
    ADAPTER_INFO_GET_UINT32(stage_info_m.nof, &stage_info_m);
    
    for(i_ms = 0; (i_ms < stage_info_m.nof) && (stage_info_m.length > sizeof(uint32) * 2); i_ms++)
    {
        stage_found = FALSE;
        ADAPTER_INFO_GET_UINT32(ms_id, &stage_info_m);
        ADAPTER_INFO_GET_UINT32(name_length,  &stage_info_m);
        ADAPTER_INFO_GET_MEM(adapter_name, name_length, &stage_info_m);
        adapter_name[name_length] = 0;
        for (i_bl = 0; i_bl < device->block_num; i_bl++)
        {
            
            if(stage_found == TRUE)
                break;

            pp_block = &device->pp_blocks[i_bl];
            for (i_st = 0; i_st < pp_block->stage_num; i_st++)
            {
                pp_stage = &pp_block->stages[i_st];
                if(!sal_strcasecmp(pp_stage->adapter_name, adapter_name))
                {
                    pp_stage->ms_id = ms_id;
                    
                    if(i_st != pp_block->stage_num - 1)
                    {
                        pp_stage->stage_to = &pp_block->stages[i_st + 1];
                    }
                    
                    if(pp_stage->peer_stage != NULL)
                    {
                        int i_cur_st;
                        pp_stage_t *cur_pp_stage;
                        for (i_cur_st = 0; i_cur_st < pp_block->stage_num; i_cur_st++)
                        {
                            cur_pp_stage = &pp_block->stages[i_cur_st];
                            if(!sal_strcasecmp(cur_pp_stage->name, pp_stage->peer_stage->name))
                            {
                                pp_stage->peer_stage->stage_to = &pp_block->stages[i_cur_st + 1];
                                break;
                            }
                        }
                    }
                    stage_found = TRUE;
                    break;
                }
            }
        }
        if(stage_found == FALSE)
        {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("PP Stage not found for adapter MS:%s ID:%d\n"), adapter_name, ms_id));
        }
    }

    if(stage_info_m.length != 0)
    {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("%d bytes left in stages buffer\n"), stage_info_m.length));
    }
exit:
    if(stage_info_m.start != NULL)
        sal_free(stage_info_m.start);
    SHR_FUNC_EXIT;
}

debug_signal_t *
sand_adapter_extract_signal(int unit, device_t *device, char *name_n, char *block_n, char *from_n)
{
    pp_block_t *cur_pp_block;
    debug_signal_t *debug_signal;
    int i, j;

    
    if(ISEMPTY(name_n) || ISEMPTY(block_n) || ISEMPTY(from_n))
        return NULL;

    for (i = 0; i < device->block_num; i++)
    {
        cur_pp_block = &device->pp_blocks[i];

        if (sal_strcasecmp(cur_pp_block->name, block_n))
            continue;

        for (j = 0; j < cur_pp_block->block_signal_num; j++)
        {
            debug_signal = &cur_pp_block->block_debug_signals[j];
            if (sal_strcasecmp(debug_signal->from, from_n))
                continue;

            if (!sal_strcasecmp(debug_signal->attribute, name_n) || !sal_strcasecmp(debug_signal->adapter_name, name_n))
                return debug_signal;
        }
    }

    return NULL;
}
