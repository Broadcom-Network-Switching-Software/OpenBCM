/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 FABRIC MULTICAST
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/mem.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <shared/bitop.h>

#include <soc/dcmn/dcmn_wb.h>

#if defined(BCM_88750_A0)

#include <soc/dfe/fe1600/fe1600_fabric_multicast.h>


STATIC soc_error_t soc_fe1600_fabric_multicast_multi_read_range(int unit, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array);


#define _SOC_FE1600_FABRIC_MULTICAST_MAX_MC_MODULES (144)
#define _SOC_FE1600_FABRIC_MULTICAST_TABLE_SIZE (64 * 1024)


soc_error_t 
soc_fe1600_fabric_multicast_low_prio_drop_select_priority_set(int unit, soc_dfe_fabric_priority_t prio)
{
    uint32 reg_val, low_prio_select;
    SOCDNX_INIT_FUNC_DEFS;
    
    
    switch(prio)
    {
        case soc_dfe_fabric_priority_0:
            low_prio_select = 0x1;
            break;
        case soc_dfe_fabric_priority_1:
            low_prio_select = 0x3;
            break;
        case soc_dfe_fabric_priority_2:
            low_prio_select = 0x7;
            break;
        case soc_dfe_fabric_priority_3:
            low_prio_select = 0xf;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong priority %d"),prio));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MUL_CTRLr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_LOW_PR_MUL_CTRLr, &reg_val, LOW_PR_MUL_PRIOSELf, low_prio_select);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_LOW_PR_MUL_CTRLr(unit, reg_val));

    
#ifdef BCM_88950
    if (SOC_IS_FE3200(unit))
    {
        int blk, nof_dch;

        nof_dch =  SOC_DFE_DEFS_GET(unit, nof_instances_dch);
        for (blk = 0; blk < nof_dch; blk++)
        {
                SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_2r(unit, blk, &reg_val));
                soc_reg_field_set(unit, DCH_DCH_ENABLERS_REGISTER_2r, &reg_val, LOW_PR_DROP_ENf, low_prio_select);
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_2r(unit, blk, reg_val));
        }
    }
#endif 

      
exit:
    SOCDNX_FUNC_RETURN;
  
}


soc_error_t 
soc_fe1600_fabric_multicast_low_prio_drop_select_priority_get(int unit, soc_dfe_fabric_priority_t* prio)
{
    uint32 reg_val, low_prio_select;
    SOCDNX_INIT_FUNC_DEFS; 
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MUL_CTRLr(unit, &reg_val));
      
    low_prio_select = soc_reg_field_get(unit, RTP_LOW_PR_MUL_CTRLr, reg_val, LOW_PR_MUL_PRIOSELf);
    
    switch(low_prio_select)
    {
        case 0x1:
            *prio = soc_dfe_fabric_priority_0;
            break;
        case 0x3:
            *prio = soc_dfe_fabric_priority_1;
            break;
        case 0x7:
            *prio = soc_dfe_fabric_priority_2;
            break; 
        case 0xf:
            *prio = soc_dfe_fabric_priority_3;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Unrecognized priority %d"),low_prio_select));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
  
}


soc_error_t 
soc_fe1600_fabric_multicast_low_prio_threshold_validate(int unit, bcm_fabric_control_t type, int arg)
{
    uint64  val64;
    SOCDNX_INIT_FUNC_DEFS; 
      
    COMPILER_64_SET(val64, 0, arg);
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            if(SOC_FAILURE(soc_reg64_field_validate(unit, RTP_LOW_PR_MULTHr, LOW_PR_MUL_DROP_TH_HIGHf, val64))) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Argument is too big %d"),arg));
            }
            break;
        case bcmFabricMcLowPrioDropThDown:
            if(SOC_FAILURE(soc_reg64_field_validate(unit, RTP_LOW_PR_MULTHr, LOW_PR_MUL_DROP_TH_LOWf, val64))) {
                 SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Argument is too big %d"),arg));
            }
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
     
exit:
    SOCDNX_FUNC_RETURN;
  
}


soc_error_t 
soc_fe1600_fabric_multicast_low_prio_threshold_set(int unit, bcm_fabric_control_t type, int arg)
{
    uint64 reg_val, val64;
    SOCDNX_INIT_FUNC_DEFS; 
	
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MULTHr(unit, &reg_val));
    COMPILER_64_SET(val64, 0, arg);
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            soc_reg64_field_set(unit, RTP_LOW_PR_MULTHr, &reg_val, LOW_PR_MUL_DROP_TH_HIGHf, val64);
            break;
        case bcmFabricMcLowPrioDropThDown:
            soc_reg64_field_set(unit, RTP_LOW_PR_MULTHr, &reg_val, LOW_PR_MUL_DROP_TH_LOWf, val64);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
    
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_LOW_PR_MULTHr(unit, reg_val));
      
exit:
    SOCDNX_FUNC_RETURN;
  
}


soc_error_t 
soc_fe1600_fabric_multicast_low_prio_threshold_get(int unit, bcm_fabric_control_t type, int* arg)
{
    uint64 reg_val;
    SOCDNX_INIT_FUNC_DEFS; 
      
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MULTHr(unit, &reg_val));
      
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            *arg = soc_reg_field_get(unit, RTP_LOW_PR_MULTHr, COMPILER_64_LO(reg_val), LOW_PR_MUL_DROP_TH_HIGHf);
            break;
        case bcmFabricMcLowPrioDropThDown:
            *arg = soc_reg_field_get(unit, RTP_LOW_PR_MULTHr, COMPILER_64_LO(reg_val), LOW_PR_MUL_DROP_TH_LOWf);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_multicast_low_priority_drop_enable_set(int unit, int arg)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS; 
     
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MUL_CTRLr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_LOW_PR_MUL_CTRLr, &reg_val, LOW_PR_MUL_ENf, (arg ? 1 : 0));
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_LOW_PR_MUL_CTRLr(unit, reg_val));
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_multicast_low_priority_drop_enable_get(int unit, int* arg)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS; 
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_LOW_PR_MUL_CTRLr(unit, &reg_val));
    *arg = soc_reg_field_get(unit, RTP_LOW_PR_MUL_CTRLr, reg_val, LOW_PR_MUL_ENf);
  
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_multicast_priority_range_validate(int unit, bcm_fabric_control_t type, int arg)
{
    uint32 max_id ;
    SOCDNX_INIT_FUNC_DEFS; 
    
    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_size_get, (unit, &max_id)));

     if(arg < 0 || arg >= max_id) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("arg %d is invalid for type %d, outside of multicast id range"),arg, type));
     }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_multicast_priority_range_set(int unit, bcm_fabric_control_t type, int arg)
{
    uint32 reg_val = 0;
    SOCDNX_INIT_FUNC_DEFS; 
    
    switch(type)
    {    
        case bcmFabricMcLowPrioMin:
            soc_reg_field_set(unit, DCH_LOW_PR_MUL_0r, &reg_val, LOW_PR_MUL_QUE_LOWf, arg);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_0r(unit, 0, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_0r(unit, 1, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_0r(unit, 2, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_0r(unit, 3, reg_val));
            break;
        case bcmFabricMcLowPrioMax:
            soc_reg_field_set(unit, DCH_LOW_PR_MUL_1r, &reg_val, LOW_PR_MUL_QUE_HIGHf, arg);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_1r(unit, 0, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_1r(unit, 1, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_1r(unit, 2, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_LOW_PR_MUL_1r(unit, 3, reg_val));
            break;
        case bcmFabricMcMidPrioMin:
            soc_reg_field_set(unit, DCH_MID_PR_MUL_0r, &reg_val, MID_PR_MUL_QUE_LOWf, arg);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_0r(unit, 0, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_0r(unit, 1, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_0r(unit, 2, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_0r(unit, 3, reg_val));
            break;
        case bcmFabricMcMidPrioMax:
            soc_reg_field_set(unit, DCH_MID_PR_MUL_1r, &reg_val, MID_PR_MUL_QUE_HIGHf, arg);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_1r(unit, 0, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_1r(unit, 1, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_1r(unit, 2, reg_val));
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_MID_PR_MUL_1r(unit, 3, reg_val));
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_multicast_priority_range_get(int unit, bcm_fabric_control_t type, int* arg)
{
    uint32 reg_val = 0;
    SOCDNX_INIT_FUNC_DEFS;
    
    switch(type)
    {    
        case bcmFabricMcLowPrioMin:
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOW_PR_MUL_0r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_LOW_PR_MUL_0r, reg_val, LOW_PR_MUL_QUE_LOWf);
            break;
        case bcmFabricMcLowPrioMax:
            SOCDNX_IF_ERR_EXIT(READ_DCH_LOW_PR_MUL_1r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_LOW_PR_MUL_1r, reg_val, LOW_PR_MUL_QUE_HIGHf);
            break;
        case bcmFabricMcMidPrioMin:
            SOCDNX_IF_ERR_EXIT(READ_DCH_MID_PR_MUL_0r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_MID_PR_MUL_0r, reg_val, MID_PR_MUL_QUE_LOWf);
            break;
        case bcmFabricMcMidPrioMax:
            SOCDNX_IF_ERR_EXIT(READ_DCH_MID_PR_MUL_1r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_MID_PR_MUL_1r, reg_val, MID_PR_MUL_QUE_HIGHf);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported control type %d"),type));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_fe1600_fabric_multicast_multi_set(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    soc_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array) 
{
    int rv, i;
    uint32 *entry_array = NULL, *entry = NULL;
    int size;
    int index_max = 0, index_min = 0;
    soc_multicast_t group;
    int entry_words;
    uint32 mem_flags;

    SOCDNX_INIT_FUNC_DEFS; 

    
    if (!flags || (flags & _SHR_FABRIC_MULTICAST_SET_ONLY))
    {
        
        
        index_min = soc_mem_index_max(unit, RTP_MULTI_CAST_TABLE_UPDATEm);
        index_max = soc_mem_index_min(unit, RTP_MULTI_CAST_TABLE_UPDATEm);
        for (i = 0; i < ngroups; i++)
        {
            
            if (groups[i] < index_min)
            {
                index_min = groups[i];
            }
            
            if (groups[i] > index_max)
            {
                index_max = groups[i];
            }
        }

        if (index_min > index_max)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong params - groups\n")));
        }

        
        size = (index_max - index_min + 1) * soc_mem_entry_bytes(unit, RTP_MCTm);
        
        entry_array = soc_cm_salloc(unit, size, "RTP_MCTm");
        if (entry_array == NULL) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("failed to allocate memory")));
        }

        if (SOC_IS_FE1600(unit))
        {
            
            sal_memset(entry_array, 0, size);
        } 
        else
        {
            rv = soc_fe1600_fabric_multicast_multi_read_range(unit, index_min, index_max, entry_array);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        entry_words = BYTES2WORDS(soc_mem_entry_bytes(unit, RTP_MULTI_CAST_TABLE_UPDATEm));
        for (i = 0; i < ngroups; i++)
        {
            group = groups[i];
            entry = entry_array + (entry_words * (group - index_min));
            soc_mem_field_set(unit, RTP_MULTI_CAST_TABLE_UPDATEm, entry, LINK_BIT_MAPf, dest_array[i]);
        }

    }

    if (flags & _SHR_FABRIC_MULTICAST_COMMIT_ONLY) {
        mem_flags = SOC_MEM_WRITE_COMMIT_ONLY;
    } else if (flags & _SHR_FABRIC_MULTICAST_SET_ONLY) {
        mem_flags = SOC_MEM_WRITE_SET_ONLY;
    } else if (flags & _SHR_FABRIC_MULTICAST_STATUS_ONLY) {
        mem_flags = SOC_MEM_WRITE_STATUS_ONLY;
    } else {
        mem_flags = 0;
    }

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_multicast_multi_write_range, (unit, mem_flags, index_min, index_max, entry_array));
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    if (entry_array != NULL) {
        soc_cm_sfree(unit, entry_array);
    }
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SOCDNX_FUNC_RETURN;
}

int 
soc_fe1600_fabric_multicast_multi_get(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    soc_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array) 
{
    int rv,i;
    uint32 *entry_array = NULL, *entry = NULL;
    int size;
    int index_max, index_min;
    bcm_fabric_module_vector_t *cur_dest_array;
    soc_multicast_t group;
    int entry_words;
    SOCDNX_INIT_FUNC_DEFS;

    
    index_min = soc_mem_index_max(unit, RTP_MULTI_CAST_TABLE_UPDATEm);
    index_max = soc_mem_index_min(unit, RTP_MULTI_CAST_TABLE_UPDATEm);
    for (i = 0; i < ngroups; i++)
    {
        
        if (groups[i] < index_min)
        {
            index_min = groups[i];
        }
        
        if (groups[i] > index_max)
        {
            index_max = groups[i];
        }
    }

    if (index_min > index_max)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong params - groups\n")));
    }

    
    size = (index_max - index_min + 1) * soc_mem_entry_bytes(unit, RTP_MCTm);
    
    entry_array = soc_cm_salloc(unit, size, "RTP_MULTI_CAST_TABLE_UPDATEm");
    if (entry_array == NULL) 
    {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("failed to allocate memory")));
    }

    rv = soc_fe1600_fabric_multicast_multi_read_range(unit, index_min, index_max, entry_array);
    SOCDNX_IF_ERR_EXIT(rv);

    entry_words = BYTES2WORDS(soc_mem_entry_bytes(unit, RTP_MCTm));
    for (i=0, cur_dest_array=dest_array; i<ngroups; i++, cur_dest_array++) 
    {
        group = groups[i];
        entry = entry_array + (entry_words * (group - index_min));
        soc_mem_field_get(unit, RTP_MCTm, entry, LINK_BIT_MAPf, *cur_dest_array);
    }

exit:
    if (entry_array != NULL) 
    {
        soc_cm_sfree(unit, entry_array);
    }
    SOCDNX_FUNC_RETURN;
}


STATIC 
soc_dfe_multicast_read_range_info_t _soc_fe1600_fabric_multicast_read_range_info_standard[] = {
    {
        0,
        _SOC_FE1600_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        1
    }
};

soc_error_t
soc_fe1600_fabric_multicast_multi_read_info_get(int unit, soc_dfe_multicast_read_range_info_t **info, int *info_size)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    *info = _soc_fe1600_fabric_multicast_read_range_info_standard;
    *info_size = sizeof(_soc_fe1600_fabric_multicast_read_range_info_standard) / sizeof(soc_dfe_multicast_read_range_info_t);

    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe1600_fabric_multicast_multi_write_range(int unit, int mem_flags, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    rv = soc_mem_array_write_range(unit, mem_flags, RTP_MULTI_CAST_TABLE_UPDATEm, 0, MEM_BLOCK_ALL, group_min, group_max, entry_array);
    SOCDNX_IF_ERR_EXIT(rv);
    

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_fe1600_fabric_multicast_multi_read_range(int unit, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array)
{
    int rv;
    uint32 *cur_entry_array, *entry;
    int entry_index,info_index,  entry_words;
    int read_index_min, read_index_max;
    uint32 data[_SHR_BITDCLSIZE(_SOC_FE1600_FABRIC_MULTICAST_MAX_MC_MODULES)];
    uint32 mc_table_entry_size;
    soc_dfe_multicast_read_range_info_t *info;
    int info_size;
    SOCDNX_INIT_FUNC_DEFS;

    
    entry_words = BYTES2WORDS(soc_mem_entry_bytes(unit, RTP_MULTI_CAST_TABLE_UPDATEm));
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_entry_size_get, (unit, &mc_table_entry_size));
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_multicast_multi_read_info_get, (unit, &info, &info_size));
    SOCDNX_IF_ERR_EXIT(rv);

    for (info_index = 0; 
         info_index < info_size; 
         info_index++)
    {
        if (group_min < info[info_index].group_max && group_max > info[info_index].group_min) 
        {
            read_index_min = (group_min > info[info_index].group_min) ? (group_min - info[info_index].group_min) : 0;
            read_index_max = (group_max < info[info_index].group_max) ? (group_max - info[info_index].group_min) : _SOC_FE1600_FABRIC_MULTICAST_TABLE_SIZE - 1;
            cur_entry_array = entry_array + (read_index_min + info[info_index].group_min - group_min) * entry_words;

            rv = soc_mem_array_read_range(unit, RTP_MCTm, info[info_index].table , MEM_BLOCK_ANY, read_index_min, read_index_max, cur_entry_array);
            SOCDNX_IF_ERR_EXIT(rv);

            
            if (info[info_index].is_first_half == 0)
            {
                for (entry_index = read_index_min; entry_index <= read_index_max; entry_index++)
                {
                    
                    entry = cur_entry_array + (entry_words * entry_index);
                    soc_mem_field_get(unit, RTP_MCTm, entry, LINK_BIT_MAPf, data);
                    SHR_BITCOPY_RANGE(data, 0, data, mc_table_entry_size, mc_table_entry_size);
                    soc_mem_field_set(unit, RTP_MCTm, entry, LINK_BIT_MAPf, data );
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_multicast_read_info(int unit, soc_multicast_t group, int *mc_table_index, int *mc_table_entry, int *is_first_half)
{
    soc_dfe_multicast_read_range_info_t *info;
    int info_index, info_size, found, rv;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_multicast_multi_read_info_get, (unit, &info, &info_size));
    SOCDNX_IF_ERR_EXIT(rv);    

    
    found = 0;
    for (info_index = 0; 
         info_index < info_size; 
         info_index++)
    {
        if (group <= info[info_index].group_max && group >= info[info_index].group_min)
        {
            *mc_table_index = info[info_index].table;
            *mc_table_entry = group - info[info_index].group_min;
            *is_first_half = info[info_index].is_first_half;
            found = 1;
            break;
        }
    }

    if (found == 0)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("group (%d) read info wasn't found\n"), group));
    }
exit:
    SOCDNX_FUNC_RETURN;
}



#endif 

#undef _ERR_MSG_MODULE_NAME

