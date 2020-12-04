/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON FABRIC MULTICAST
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <soc/mem.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/sand/sand_mem.h>
#include <soc/sbusdma_internal.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <shared/bitop.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <soc/dnxf/ramon/ramon_fabric_multicast.h>

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

#define _SOC_RAMON_FABRIC_MULTICAST_TABLE_SIZE (32 * 1024)

#define DNXF_MULTICAST_SBUSDMA_RESERVED_CHANNEL 3

static uint32* ramon_multi_entry_array = NULL;


shr_error_e
soc_ramon_fabric_multicast_multi_write_range(int unit, int mem_flags, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array)
{
    int rv;
    int cmc;
    int ch;
    SHR_FUNC_INIT_VARS(unit);

    if ((mem_flags == SOC_MEM_WRITE_SET_ONLY) || (mem_flags == SOC_MEM_WRITE_STATUS_ONLY)) {
        rv = cmicx_sbusdma_vchan_to_cmc_ch(unit, DNXF_MULTICAST_SBUSDMA_RESERVED_CHANNEL, &cmc, &ch);
        SHR_IF_ERR_EXIT(rv);
    }

    if (mem_flags == SOC_MEM_WRITE_SET_ONLY) {
        rv = cmicx_sbusdma_ch_try_get(unit, cmc, ch);
        SHR_IF_ERR_EXIT(rv);
    }

    rv = soc_mem_array_write_range_multi_cmc(unit, mem_flags, RTP_MULTI_CAST_TABLE_UPDATEm, 0, MEM_BLOCK_ALL, group_min, group_max, entry_array, DNXF_MULTICAST_SBUSDMA_RESERVED_CHANNEL);
    SHR_IF_ERR_EXIT(rv);

    if (mem_flags == SOC_MEM_WRITE_STATUS_ONLY) {
        rv = cmicx_sbusdma_ch_put(unit, cmc, ch);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_fabric_multicast_multi_read_range(int unit, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = soc_mem_array_read_range(unit, RTP_MULTI_CAST_TABLE_UPDATEm, 0, MEM_BLOCK_ALL, group_min, group_max, entry_array);
    SHR_IF_ERR_EXIT(rv);


exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_fabric_multicast_local_route_default_init(int unit)
{
    bcm_fabric_module_vector_t data;
    uint32 mc_table_size = 0;
    uint32 mc_local_route_bit = 0;
    uint32 min_nof_links_val = 1;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(data, 0, sizeof(data));

    if (!SAL_BOOT_PLISIM)
    {
        
        SHR_IF_ERR_EXIT(soc_ramon_fabric_multicast_local_route_bit_get(unit, &mc_local_route_bit));

        SHR_BITSET(data, mc_local_route_bit);

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &mc_table_size)));

        SHR_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, RTP_MULTI_CAST_TABLE_UPDATEm, 0, 0, MEM_BLOCK_ALL, 0, mc_table_size-1, data));

        
        SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, RTP_MNOLRm, MEM_BLOCK_ALL, &min_nof_links_val));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_multicast_local_route_bit_get(int unit, uint32 *mc_local_route_bit)
{
    uint32 mc_table_entry_size = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_entry_size_get, (unit, &mc_table_entry_size)));
    *mc_local_route_bit = mc_table_entry_size;

exit:
    SHR_FUNC_EXIT;
}



shr_error_e 
soc_ramon_fabric_multicast_low_prio_drop_select_priority_set(int unit, soc_dnxf_fabric_priority_t prio)
{
    uint32 reg_val, low_prio_select;
    int blk, nof_qrh, nof_dch;
    SHR_FUNC_INIT_VARS(unit);
    
    
    switch(prio)
    {
        case soc_dnxf_fabric_priority_0:
            low_prio_select = 0x1;
            break;
        case soc_dnxf_fabric_priority_1:
            low_prio_select = 0x3;
            break;
        case soc_dnxf_fabric_priority_2:
            low_prio_select = 0x7;
            break;
        case soc_dnxf_fabric_priority_3:
            low_prio_select = 0xf;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong priority %d",prio);
    }

    
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (blk = 0; blk < nof_qrh; blk++) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOW_PR_MUL_CTRLr, blk, 0, &reg_val));
        soc_reg_field_set(unit, QRH_LOW_PR_MUL_CTRLr, &reg_val, LOW_PR_MUL_PRIOSELf, low_prio_select);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_LOW_PR_MUL_CTRLr, blk, 0, reg_val));
    }

    
    nof_dch =  dnxf_data_device.blocks.nof_instances_dch_get(unit);
    for (blk = 0; blk < nof_dch; blk++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, DCH_DCH_ENABLERS_REGISTER_1r, blk, 0, &reg_val));
        soc_reg_field_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, &reg_val, LOW_PR_DROP_ENf, low_prio_select);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, blk, 0, reg_val));
    }


      
exit:
    SHR_FUNC_EXIT;
  
}



shr_error_e 
soc_ramon_fabric_multicast_low_prio_drop_select_priority_get(int unit, soc_dnxf_fabric_priority_t* prio)
{
    uint32 reg_val, low_prio_select;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOW_PR_MUL_CTRLr, 0, 0, &reg_val));
    low_prio_select = soc_reg_field_get(unit, QRH_LOW_PR_MUL_CTRLr, reg_val, LOW_PR_MUL_PRIOSELf);
    
    switch(low_prio_select)
    {
        case 0x1:
            *prio = soc_dnxf_fabric_priority_0;
            break;
        case 0x3:
            *prio = soc_dnxf_fabric_priority_1;
            break;
        case 0x7:
            *prio = soc_dnxf_fabric_priority_2;
            break; 
        case 0xf:
            *prio = soc_dnxf_fabric_priority_3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized priority %d",low_prio_select);
    }
    
exit:
    SHR_FUNC_EXIT;
  
}


shr_error_e 
soc_ramon_fabric_multicast_low_prio_threshold_validate(int unit, bcm_fabric_control_t type, int arg)
{
    uint64  val64;
    SHR_FUNC_INIT_VARS(unit);
      
    COMPILER_64_SET(val64, 0, arg);
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            if(SOC_FAILURE(soc_reg64_field_validate(unit, QRH_LOW_PR_MULTHr, LOW_PR_MUL_DROP_TH_HIGHf, val64))) {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Argument is too big %d",arg);
            }
            break;
        case bcmFabricMcLowPrioDropThDown:
            if(SOC_FAILURE(soc_reg64_field_validate(unit, QRH_LOW_PR_MULTHr, LOW_PR_MUL_DROP_TH_LOWf, val64))) {
                 SHR_ERR_EXIT(_SHR_E_PARAM, "Argument is too big %d",arg);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
     
exit:
    SHR_FUNC_EXIT;
  
}


shr_error_e 
soc_ramon_fabric_multicast_low_prio_threshold_set(int unit, bcm_fabric_control_t type, int arg)
{
    uint64 reg_val, val64;
    int blk, nof_qrh;
    SHR_FUNC_INIT_VARS(unit);
	
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    COMPILER_64_SET(val64, 0, arg);
    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            for (blk = 0; blk < nof_qrh; blk++) {
                SHR_IF_ERR_EXIT(soc_reg64_get(unit, QRH_LOW_PR_MULTHr, blk, 0, &reg_val));
                soc_reg64_field_set(unit, QRH_LOW_PR_MULTHr, &reg_val, LOW_PR_MUL_DROP_TH_HIGHf, val64);
                SHR_IF_ERR_EXIT(soc_reg64_set(unit, QRH_LOW_PR_MULTHr, blk, 0, reg_val));
            }
            break;
        case bcmFabricMcLowPrioDropThDown:
            for (blk = 0; blk < nof_qrh; blk++) {
                SHR_IF_ERR_EXIT(soc_reg64_get(unit, QRH_LOW_PR_MULTHr, blk, 0, &reg_val));
                soc_reg64_field_set(unit, QRH_LOW_PR_MULTHr, &reg_val, LOW_PR_MUL_DROP_TH_LOWf, val64);
                SHR_IF_ERR_EXIT(soc_reg64_set(unit, QRH_LOW_PR_MULTHr, blk, 0, reg_val));
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
      
exit:
    SHR_FUNC_EXIT;
  
}


shr_error_e 
soc_ramon_fabric_multicast_low_prio_threshold_get(int unit, bcm_fabric_control_t type, int* arg)
{
    uint64 reg_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg64_get(unit, QRH_LOW_PR_MULTHr, 0, 0, &reg_val));

    switch(type)
    {
        case bcmFabricMcLowPrioDropThUp:
            *arg = soc_reg_field_get(unit, QRH_LOW_PR_MULTHr, COMPILER_64_LO(reg_val), LOW_PR_MUL_DROP_TH_HIGHf);
            break;
        case bcmFabricMcLowPrioDropThDown:
            *arg = soc_reg_field_get(unit, QRH_LOW_PR_MULTHr, COMPILER_64_LO(reg_val), LOW_PR_MUL_DROP_TH_LOWf);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
    
exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_multicast_low_priority_drop_enable_set(int unit, int arg)
{
    uint32 reg_val;
    int blk, nof_qrh;
    SHR_FUNC_INIT_VARS(unit);
    
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (blk = 0; blk < nof_qrh; blk++) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOW_PR_MUL_CTRLr, blk, 0, &reg_val));
        soc_reg_field_set(unit, QRH_LOW_PR_MUL_CTRLr, &reg_val, LOW_PR_MUL_ENf, (arg ? 1 : 0));
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_LOW_PR_MUL_CTRLr, blk, 0, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_multicast_low_priority_drop_enable_get(int unit, int* arg)
{
    uint32 reg_val;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOW_PR_MUL_CTRLr, 0, 0, &reg_val));
    *arg = soc_reg_field_get(unit, QRH_LOW_PR_MUL_CTRLr, reg_val, LOW_PR_MUL_ENf);
  
exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_multicast_priority_range_validate(int unit, bcm_fabric_control_t type, int arg)
{
    uint32 max_id ;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &max_id)));

     if(arg < 0 || arg >= max_id) {
         SHR_ERR_EXIT(_SHR_E_PARAM, "arg %d is invalid for type %d, outside of multicast id range",arg, type);
     }
    
exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_multicast_priority_range_set(int unit, bcm_fabric_control_t type, int arg)
{
    uint32 reg_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    switch(type)
    {    
        case bcmFabricMcLowPrioMin:
            soc_reg_field_set(unit, BRDC_DCH_LOW_PR_MUL_0r, &reg_val, LOW_PR_MUL_QUE_LOWf, arg);
            SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_LOW_PR_MUL_0r(unit, reg_val));
            break;
        case bcmFabricMcLowPrioMax:
            soc_reg_field_set(unit, BRDC_DCH_LOW_PR_MUL_1r, &reg_val, LOW_PR_MUL_QUE_HIGHf, arg);
            SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_LOW_PR_MUL_1r(unit, reg_val));
            break;
        case bcmFabricMcMidPrioMin:
            soc_reg_field_set(unit, BRDC_DCH_MID_PR_MUL_0r, &reg_val, MID_PR_MUL_QUE_LOWf, arg);
            SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_MID_PR_MUL_0r(unit, reg_val));
            break;
        case bcmFabricMcMidPrioMax:
            soc_reg_field_set(unit, BRDC_DCH_MID_PR_MUL_1r, &reg_val, MID_PR_MUL_QUE_HIGHf, arg);
            SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_MID_PR_MUL_1r(unit, reg_val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
    
exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_multicast_priority_range_get(int unit, bcm_fabric_control_t type, int* arg)
{
    uint32 reg_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    switch(type)
    {    
        case bcmFabricMcLowPrioMin:
            SHR_IF_ERR_EXIT(READ_DCH_LOW_PR_MUL_0r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_LOW_PR_MUL_0r, reg_val, LOW_PR_MUL_QUE_LOWf);
            break;
        case bcmFabricMcLowPrioMax:
            SHR_IF_ERR_EXIT(READ_DCH_LOW_PR_MUL_1r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_LOW_PR_MUL_1r, reg_val, LOW_PR_MUL_QUE_HIGHf);
            break;
        case bcmFabricMcMidPrioMin:
            SHR_IF_ERR_EXIT(READ_DCH_MID_PR_MUL_0r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_MID_PR_MUL_0r, reg_val, MID_PR_MUL_QUE_LOWf);
            break;
        case bcmFabricMcMidPrioMax:
            SHR_IF_ERR_EXIT(READ_DCH_MID_PR_MUL_1r(unit, 0, &reg_val));
            *arg = soc_reg_field_get(unit, DCH_MID_PR_MUL_1r, reg_val, MID_PR_MUL_QUE_HIGHf);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type %d",type);
    }
    
exit:
    SHR_FUNC_EXIT;
}



int 
soc_ramon_fabric_multicast_multi_set(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    soc_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array,
    int *local_route_enable_array) 
{
    int rv, i;
    uint32* entry = NULL;
    int size;
    int index_max = 0, index_min = 0;
    soc_multicast_t group;
    int entry_words;
    uint32 mem_flags = 0;
    uint32 mc_table_entry_size = 0;
    uint32 mc_local_route_bit = 0;
    bcm_fabric_module_vector_t data = {0};
    SHR_FUNC_INIT_VARS(unit);

    
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
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong params - groups\n");
        }

        entry_words = BYTES2WORDS(soc_mem_entry_bytes(unit, RTP_MULTI_CAST_TABLE_UPDATEm));
        
        size = (index_max - index_min + 1) * WORDS2BYTES(entry_words);
        
        ramon_multi_entry_array = soc_cm_salloc(unit, size, "RTP_MULTI_CAST_TABLE_UPDATEm");

        if (ramon_multi_entry_array == NULL) {
             SHR_ERR_EXIT(_SHR_E_PARAM, "failed to allocate memory");
        }

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_read_range, (unit, index_min, index_max, ramon_multi_entry_array));
        SHR_IF_ERR_EXIT(rv);

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_entry_size_get, (unit, &mc_table_entry_size));
        SHR_IF_ERR_EXIT(rv);

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_local_route_bit_get, (unit, &mc_local_route_bit));
        SHR_IF_ERR_EXIT(rv);

        for (i = 0; i < ngroups; i++)
        {
            group = groups[i];
            entry = ramon_multi_entry_array + (entry_words * (group - index_min));
            SHR_BITCOPY_RANGE(data, 0, entry, 0, BCM_FABRIC_MAX_MULTICAST_TABLE_ENTRY_SIZE);

            if (local_route_enable_array != NULL)
            {
                
                SHR_BITWRITE(data, mc_local_route_bit, !local_route_enable_array[i]);
            }

            if (dest_array != NULL)
            {
                
                SHR_BITCOPY_RANGE(data, 0, dest_array[i], 0, mc_table_entry_size);
            }

            soc_mem_field_set(unit, RTP_MULTI_CAST_TABLE_UPDATEm, entry, LINK_BIT_MAPf, data);
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

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_write_range, (unit, mem_flags, index_min, index_max, ramon_multi_entry_array));
    SHR_IF_ERR_EXIT(rv);

exit:
    if ((ramon_multi_entry_array != NULL) && (mem_flags == SOC_MEM_WRITE_STATUS_ONLY || mem_flags == 0)) {
        soc_cm_sfree(unit, ramon_multi_entry_array);
        ramon_multi_entry_array = NULL;
    }
    SHR_FUNC_EXIT;
}


int 
soc_ramon_fabric_multicast_multi_get(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    soc_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array,
    int *local_route_enable_array) 
{
    int rv,i;
    uint32 *entry_array = NULL, *entry = NULL;
    int size;
    int index_max, index_min;
    bcm_fabric_module_vector_t temp_dest = {0};
    soc_multicast_t group;
    int entry_words;
    uint32 mc_local_route_bit = 0;
    SHR_FUNC_INIT_VARS(unit);

    
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
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong params - groups\n");
    }

    entry_words = BYTES2WORDS(soc_mem_entry_bytes(unit, RTP_MULTI_CAST_TABLE_UPDATEm));
    
    size = (index_max - index_min + 1) * WORDS2BYTES(entry_words);
    
    entry_array = soc_cm_salloc(unit, size, "RTP_MULTI_CAST_TABLE_UPDATEm");
    if (entry_array == NULL) 
    {
         SHR_ERR_EXIT(_SHR_E_PARAM, "failed to allocate memory");
    }

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_read_range, (unit, index_min, index_max, entry_array));
    SHR_IF_ERR_EXIT(rv);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_local_route_bit_get, (unit, &mc_local_route_bit));
    SHR_IF_ERR_EXIT(rv);

    for (i=0; i<ngroups; i++) 
    {
        group = groups[i];
        entry = entry_array + (entry_words * (group - index_min));
        soc_mem_field_get(unit, RTP_MULTI_CAST_TABLE_UPDATEm, entry, LINK_BIT_MAPf, temp_dest);

        if (local_route_enable_array != NULL)
        {
            
            local_route_enable_array[i] = !(SHR_BITGET(temp_dest, mc_local_route_bit));
        }

        if (dest_array != NULL)
        {
            SHR_BITCOPY_RANGE(dest_array[i], 0, temp_dest, 0, BCM_FABRIC_MAX_MULTICAST_TABLE_ENTRY_SIZE);

            
            SHR_BITCLR(dest_array[i], mc_local_route_bit);
        }
    }

exit:
    if (entry_array != NULL)
    {
        soc_cm_sfree(unit, entry_array);
    }
    SHR_FUNC_EXIT;
}


int
soc_ramon_fabric_multicast_direct_offset_set(int unit, int fap_modid_offset)
{
    soc_dnxf_multicast_mode_t mc_mode;
    soc_reg_above_64_val_t data;
    uint32 fap_id, nof_links, local_modid;
    int nof_qrh, qrh_index;
    uint32 reg_val32;
    uint32 nof_mc_groups_created;
    SHR_FUNC_INIT_VARS(unit);

    mc_mode = dnxf_data_fabric.multicast.mode_get(unit);
    if (soc_dnxf_multicast_mode_direct != mc_mode)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "modid offset configuration is supported only for direct multicast mode.");
    }

    SHR_IF_ERR_EXIT(dnxf_state.mc.nof_mc_groups_created.get(unit, &nof_mc_groups_created));
    if (nof_mc_groups_created > 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "modid offset configuration needs to be set before creating the first multicast group. %d multicast groups were already created.", nof_mc_groups_created);
    }

    nof_links = dnxf_data_port.general.nof_links_get(unit);

    if ((fap_modid_offset < 0) || (fap_modid_offset > (SOC_DNXF_MODID_NOF - nof_links)))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "modid offset %d is out of range.", fap_modid_offset);
    }

    
    SOC_REG_ABOVE_64_ALLONES(data);
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, RTP_FLGMm, MEM_BLOCK_ALL, data));

    for (fap_id = fap_modid_offset; fap_id < (fap_modid_offset + nof_links); fap_id++)
    {
        local_modid = fap_id - fap_modid_offset;
        SHR_IF_ERR_EXIT(READ_RTP_FLGMm(unit, MEM_BLOCK_ANY, fap_id, &reg_val32));
        soc_mem_field_set(unit, RTP_FLGMm, &reg_val32, GROUP_NUMf, &local_modid);
        SHR_IF_ERR_EXIT(WRITE_RTP_FLGMm(unit, MEM_BLOCK_ALL, fap_id, &reg_val32));
    }

    
    SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, &reg_val32));
    soc_reg_field_set(unit,RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr,&reg_val32,ALRC_FORCE_CALCULATIONf,1);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, reg_val32));

    sal_usleep(1000);

    soc_reg_field_set(unit,RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr,&reg_val32,ALRC_FORCE_CALCULATIONf,0);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, reg_val32));

    
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (qrh_index = 0; qrh_index < nof_qrh; qrh_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_MULTICAST_MODE_SELECTIONr, qrh_index, 0, &reg_val32));
        soc_reg_field_set(unit, QRH_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_LIST_OF_FAPS_OFFSETf, fap_modid_offset);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_MULTICAST_MODE_SELECTIONr, qrh_index, 0, reg_val32));
    }

exit:
    SHR_FUNC_EXIT;
}


int
soc_ramon_fabric_multicast_direct_offset_get(int unit, int* fap_modid_offset)
{
    soc_dnxf_multicast_mode_t mc_mode;
    int qrh_index;
    uint32 reg_val32;
    SHR_FUNC_INIT_VARS(unit);

    mc_mode = dnxf_data_fabric.multicast.mode_get(unit);
    if (soc_dnxf_multicast_mode_direct != mc_mode)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "modid offset configuration is supported only for direct multicast mode.");
    }

    qrh_index = 0;
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_MULTICAST_MODE_SELECTIONr, qrh_index, 0, &reg_val32));
    *fap_modid_offset = soc_reg_field_get(unit, QRH_MULTICAST_MODE_SELECTIONr, reg_val32, MC_LIST_OF_FAPS_OFFSETf);

exit:
    SHR_FUNC_EXIT;
}
