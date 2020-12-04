/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON FABRIC TOPOLOGY
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC 
#include <shared/bsl.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxf/ramon/ramon_fabric_topology.h>
#include <soc/dnxc/error.h>
#include <soc/defs.h>
#include <soc/error.h>


#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

shr_error_e 
soc_ramon_fabric_topology_isolate_set(int unit, soc_dnxc_isolation_status_t val)
{
    uint32 reg_val32 = 0;
    uint64 reg_val64;
    uint32 gpd_rmgr_global_time = 0;
    uint32 gpd_rmgr_global_at_core_clock_steps = 0;
    uint32 gpd_spread_time = 0;
    uint32 gpd_rmgr = 0;
    uint32 max_base_index = 0;
    uint32 core_clock_speed;
    uint32 gpd_threshold;
    uint32 sleep_time_msec;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg_val64);

    if(soc_dnxc_isolation_status_isolated == val) {
        SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
        soc_reg64_field_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_STARTf, 0);
        SHR_IF_ERR_EXIT(WRITE_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, reg_val64));

        sal_usleep(1000);

        
        SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
        soc_reg64_field_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_STARTf, 1);
        SHR_IF_ERR_EXIT(WRITE_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, reg_val64));

        
        
        gpd_rmgr = soc_reg64_field32_get(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, reg_val64, GRACEFUL_PD_RMGRf);
        gpd_rmgr_global_at_core_clock_steps = gpd_rmgr * dnxf_data_fabric.reachability.rmgr_units_get(unit);
        gpd_rmgr_global_time = ((gpd_rmgr_global_at_core_clock_steps * 1000) / (dnxf_data_device.general.core_clock_khz_get(unit))) * 1000;   

        SHR_IF_ERR_EXIT(READ_QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, 0, &reg_val32));
        max_base_index = soc_reg_field_get(unit, QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val32, MAX_BASE_INDEXf);

        
        gpd_spread_time = (max_base_index * gpd_rmgr_global_time * dnxf_data_fabric.reachability.gpd_rmgr_time_factor_get(unit)) / 1000;
        sal_usleep(gpd_spread_time);

        
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_rmgr_set, (unit, 0)));

        
        core_clock_speed = dnxf_data_device.general.core_clock_khz_get(unit);
        SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
        gpd_threshold = soc_reg64_field32_get(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, reg_val64, GRACEFUL_PD_CNT_THf);
        sleep_time_msec = (gpd_threshold / core_clock_speed) * 10; 
        sal_usleep(sleep_time_msec*1000);

        SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
        soc_reg64_field_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_STARTf, 0);
        SHR_IF_ERR_EXIT(WRITE_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, reg_val64));

    } else {
        
        SHR_IF_ERR_EXIT(soc_ramon_fabric_topology_mesh_topology_reset(unit));

        
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_topology_rmgr_set, (unit, 1)));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_topology_mesh_topology_reset(int unit)
{
    uint32 reg_val32;
    soc_reg_above_64_val_t reg_above_64_val;
    int cch_index,nof_instances_cch;
    SHR_FUNC_INIT_VARS(unit);

    nof_instances_cch = dnxf_data_device.blocks.nof_instances_cch_get(unit);

    if (SOC_DNXF_IS_FE13(unit))
    {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        for (cch_index = 0; cch_index < nof_instances_cch ; cch_index++)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, CCH_AUTO_DOC_NAME_15r, cch_index, 0, reg_above_64_val));
        }
    }

    
    for (cch_index = 0; cch_index < nof_instances_cch ; cch_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, cch_index, 0, &reg_val32));
        soc_reg_field_set(unit,CCH_FILTERED_CELLS_CONFIGURATIONSr,&reg_val32,FILTER_CELLSf,1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, cch_index, 0, reg_val32));
    }

    sal_usleep(1000);

     
    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 0);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));
    
    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 1);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));
    

    for (cch_index = 0; cch_index < nof_instances_cch ; cch_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, cch_index, 0, &reg_val32));
        soc_reg_field_set(unit,CCH_FILTERED_CELLS_CONFIGURATIONSr,&reg_val32,FILTER_CELLSf,0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_FILTERED_CELLS_CONFIGURATIONSr, cch_index, 0, reg_val32));
    }

    if (SOC_DNXF_IS_FE13(unit))
    {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        for (cch_index = 0; cch_index < nof_instances_cch ; cch_index++)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, CCH_AUTO_DOC_NAME_15r, cch_index, 0, reg_above_64_val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_topology_modid_group_map_set(int unit, soc_module_t modid, int group)
{
    uint32 val = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_FLGMm(unit, MEM_BLOCK_ANY, modid, &val));
    soc_mem_field_set(unit, RTP_FLGMm, &val, GROUP_NUMf, (uint32*)&group);
    SHR_IF_ERR_EXIT(WRITE_RTP_FLGMm(unit, MEM_BLOCK_ALL, modid, &val));

    
    SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, &val));
    soc_reg_field_set(unit,RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr,&val,ALRC_FORCE_CALCULATIONf,1);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, val));

    sal_usleep(1000);

    soc_reg_field_set(unit,RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr,&val,ALRC_FORCE_CALCULATIONf,0);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, val));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_topology_mc_sw_control_set(int unit, int group, int enable)
{
    soc_reg_above_64_val_t reg_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_MCLBT_GROUP_STATIC_CONFIGr(unit, reg_val));
    SHR_BITWRITE(reg_val, group, enable);
    SHR_IF_ERR_EXIT(WRITE_RTP_MCLBT_GROUP_STATIC_CONFIGr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_topology_mc_sw_control_get(int unit, int group, int *enable)
{
    soc_reg_above_64_val_t reg_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_MCLBT_GROUP_STATIC_CONFIGr(unit, reg_val));
    *enable = (SHR_BITGET(reg_val, group)) ? (TRUE) : (FALSE);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_topology_mc_set(int unit, soc_module_t destination_local_id, int links_count, soc_port_t *links_array)
{
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 val;
    soc_port_t link = -1;
    int i_link = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    sal_memset(bmp, 0, sizeof(uint32)*DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32);
    
    for(i_link = 0 ; i_link < links_count ; i_link++)
    {
        link = links_array[i_link];
        if (link != SOC_PORT_INVALID) 
        {
            SHR_BITSET(bmp,link);
        }   
    }
    
    SHR_IF_ERR_EXIT(WRITE_RTP_SGRm(unit, MEM_BLOCK_ALL, destination_local_id, bmp));

    
    SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, &val));
    soc_reg_field_set(unit,RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr,&val,ALRC_FORCE_CALCULATIONf,1);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, val));

    sal_usleep(1000);

    soc_reg_field_set(unit,RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr,&val,ALRC_FORCE_CALCULATIONf,0);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, val));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_topology_mc_get(int unit, soc_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array)
{
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    int i_link = 0;
    int link_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    sal_memset(bmp, 0, sizeof(uint32)*DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32);
    
    
    SHR_IF_ERR_EXIT(READ_RTP_SGRm(unit, MEM_BLOCK_ANY, destination_local_id, bmp));

    *links_count = 0;
    
    SHR_BIT_ITER(bmp, dnxf_data_fabric.topology.nof_local_modid_get(unit), i_link)
    {
        if (link_index > links_count_max)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "links_array is too small for required links.");
        }

        links_array[link_index] = i_link;
        (link_index)++;
    }

    *links_count = link_index;
    
exit:
    SHR_FUNC_EXIT;
}



shr_error_e 
soc_ramon_fabric_topology_isolate_get(int unit, soc_dnxc_isolation_status_t* val)
{
    uint32 value;
    soc_reg_above_64_val_t reg_above64_val;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit,reg_above64_val));
      
    value = soc_reg_above_64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGRf);

    if(value == 0)
        *val = soc_dnxc_isolation_status_isolated;
    else
        *val = soc_dnxc_isolation_status_active;
      
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_topology_rmgr_set(int unit, int enable)
{
    soc_reg_above_64_val_t reg_above64_val;
    uint32 core_clock_speed = 0;
    uint32 rmgr = 0;
    uint32 rmgr_global = 0;
    uint32 rmgr_global_at_core_clock_steps = 0;
    int rate_between_links = 0;
    int full_cycle_rate = 0;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(reg_above64_val, 0, sizeof(reg_above64_val));

    
    if (enable)
    {
        core_clock_speed = dnxf_data_device.general.core_clock_khz_get(unit);

        
        rate_between_links = dnxf_data_fabric.reachability.gen_rate_link_delta_get(unit);
        rmgr = (rate_between_links * core_clock_speed) / 1000000;

        
        full_cycle_rate = dnxf_data_fabric.reachability.gen_rate_full_cycle_get(unit);
        rmgr_global_at_core_clock_steps = ((full_cycle_rate / 100) * core_clock_speed ) / 10000;
        rmgr_global = rmgr_global_at_core_clock_steps / dnxf_data_fabric.reachability.rmgr_units_get(unit);
    } else {
        rmgr = 0;
        rmgr_global = 0;
    }

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit,reg_above64_val));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGRf, rmgr);
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGR_GLOBALf, rmgr_global);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit,reg_above64_val));

exit:
    SHR_FUNC_EXIT; 
}


shr_error_e 
soc_ramon_fabric_topology_lb_set(int unit, soc_module_t destination_local_id, int links_count, soc_port_t *links_array)
{
    int i, nof_links;
    int max_link_score;
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 totsf_val = 0, slsct_val, score, sctinc_val, sctinc, last_score = 0;
    soc_port_t link;
    soc_field_t scrub_en;
    soc_reg_above_64_val_t reg_above64_val;
    SHR_FUNC_INIT_VARS(unit);
    
    
    BCM_PBMP_COUNT(dnxf_data_port.general.supported_phys_get(unit)->pbmp , nof_links);
    if (soc_dnxf_load_balancing_mode_normal != dnxf_data_fabric.topology.load_balancing_mode_get(unit)
        && links_count > nof_links / 2)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "links_count > %d is allowed only in normal load balancing mode", nof_links / 2);
    }

    
    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    scrub_en = soc_reg_above_64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, SCT_SCRUB_ENABLEf);
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, SCT_SCRUB_ENABLEf, 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

    
    soc_mem_field_set(unit, RTP_TOTSFm, &totsf_val, TOTAL_LINKSf, (uint32*)&links_count);
    
    
    sal_memset(bmp, 0, sizeof(bmp));

    for(i = 0 ; i<links_count ; i++)
    {
        link = links_array[i];
        if (link != SOC_PORT_INVALID) 
        {
            SHR_BITSET(bmp,link);
        }
    }
    
    
    SHR_IF_ERR_EXIT(dnxf_state.lb.group_to_first_link.set(unit, destination_local_id, links_array[0]));
    
    
    for(i = 0 ; i<links_count ; i++)
    {
        link = links_array[i];
        if (link != SOC_PORT_INVALID) 
        {
            
            SHR_IF_ERR_EXIT(WRITE_RTP_RCGLBTm(unit, MEM_BLOCK_ALL, link, bmp));
            
            
            SHR_IF_ERR_EXIT(WRITE_RTP_TOTSFm(unit, MEM_BLOCK_ALL, link, &totsf_val));
            
            
            slsct_val = 0;
            max_link_score = dnxf_data_fabric.topology.max_link_score_get(unit);
            if (soc_dnxf_load_balancing_mode_destination_unreachable == dnxf_data_fabric.topology.load_balancing_mode_get(unit)) {
                score = max_link_score;
            } else {
                score = (max_link_score * (i + 1)) / links_count;    
            }
            soc_mem_field_set(unit, RTP_SLSCTm, &slsct_val, SCORE_OF_LINKf, &score);
            SHR_IF_ERR_EXIT(WRITE_RTP_SLSCTm(unit, MEM_BLOCK_ALL, link, &slsct_val));
    
            
            sctinc_val = 0;
            sctinc = score-last_score;
            last_score=score;
            soc_mem_field_set(unit, RTP_SCTINCm, &sctinc_val, SCORE_TO_INCf, &sctinc);
            SHR_IF_ERR_EXIT(WRITE_RTP_SCTINCm(unit, MEM_BLOCK_ALL, link, &sctinc_val));
        }
    }
    
    
    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, SCT_SCRUB_ENABLEf, scrub_en);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_topology_lb_get(int unit, soc_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array)
{
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 totsf_val;
    int i, link_index;
    soc_port_t link = -1;
    SHR_FUNC_INIT_VARS(unit);
    
    
    SHR_IF_ERR_EXIT(dnxf_state.lb.group_to_first_link.get(unit, destination_local_id, &link));
    SHR_IF_ERR_EXIT(READ_RTP_RCGLBTm(unit, MEM_BLOCK_ANY, link, bmp));
    *links_count = 0;
    
    PBMP_SFI_ITER(unit, i)
    {      
        if(SHR_BITGET(bmp,i)) 
        {
            SHR_IF_ERR_EXIT(READ_RTP_TOTSFm(unit, MEM_BLOCK_ALL, i, &totsf_val));
            soc_mem_field_get(unit, RTP_TOTSFm, &totsf_val, TOTAL_LINKSf, (uint32*)links_count);
            break;
        }
    }

    if ( (*links_count) > links_count_max)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "links_array is too small");
    }

    link_index=0;
    PBMP_SFI_ITER(unit, i)
    {
        if(SHR_BITGET(bmp,i))
        {
            links_array[link_index] = i;
            (link_index)++;
        }
    }
    
    for ( ; link_index < (*links_count) ; link_index++)
    {
        links_array[link_index] = -1;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_topology_local_route_control_cells_enable_set(int unit, int enable)
{
    uint32 reg_val;
    int first_qrh = 0, last_qrh = 0, qrh_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_DNXF_IS_FE13(unit)) {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Disabling LR is allowed only on FE in FE13 mode");
    }

    
    first_qrh = 0;
    last_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit) / 2;

    for (qrh_index = first_qrh; qrh_index < last_qrh; qrh_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, qrh_index, 0, &reg_val));
        soc_reg_field_set(unit,QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val, ENABLE_CTRL_LOCAL_FE_1_ROUTINGf, enable);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, qrh_index, 0, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}


#undef BSL_LOG_MODULE
