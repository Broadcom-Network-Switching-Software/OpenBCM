/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 FABRIC TOPOLOGY
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC 
#include <shared/bsl.h>

#include <soc/dfe/cmn/dfe_drv.h>

#include <soc/dfe/fe3200/fe3200_fabric_topology.h>
#include <soc/dcmn/error.h>
#include <soc/defs.h>
#include <soc/error.h>


soc_error_t 
soc_fe3200_fabric_topology_reachability_mask_set(int unit, soc_pbmp_t active_links, soc_dcmn_isolation_status_t val)
{
    soc_port_t port;
    soc_reg_above_64_val_t reg_val;
    uint32 reg32_val = 0;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (val == soc_dcmn_isolation_status_isolated)
    {
        
        SOCDNX_IF_ERR_EXIT(READ_RTP_RESERVED_10r(unit, &reg32_val));

            
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit, 0x0));
    }

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_val));
    
    SOC_PBMP_ITER(active_links, port) {
        if (val == soc_dcmn_isolation_status_isolated) {
            
            SHR_BITCLR(reg_val, port);
        } else {
            
            SHR_BITSET(reg_val, port);
        }
    }
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_val));
      
    if (val == soc_dcmn_isolation_status_isolated)
    {
        
        sal_usleep(30000);

        
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_fabric_topology_nof_links_to_min_nof_links_default(int unit, int nof_links, int *min_nof_links)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    *min_nof_links = 0;

    SOCDNX_FUNC_RETURN; 
}


int
soc_fe3200_fabric_topology_min_nof_links_set(int unit, int min_nof_links)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    if (min_nof_links != 0)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("FE3200 do not support min number of links feature")));
    }
exit:
    SOCDNX_FUNC_RETURN; 
}



int
soc_fe3200_fabric_topology_min_nof_links_get(int unit, int *min_nof_links)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    *min_nof_links = 0;

    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
soc_fe3200_fabric_topology_isolate_set(int unit, soc_dcmn_isolation_status_t val)
{
    uint32 reg_val32;

    SOCDNX_INIT_FUNC_DEFS;

    if(soc_dcmn_isolation_status_active == val) {

        sal_usleep(2000);

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_topology_mesh_topology_reset(unit));
                   
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_topology_rmgr_set, (unit, 1)));
    } else {
        uint64 reg64_val, reg64_ones;
        COMPILER_64_ZERO(reg64_val);

        
        SOCDNX_IF_ERR_EXIT(READ_RTP_RESERVED_10r(unit,&reg_val32));

        
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit,0x0));
               
        
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_topology_rmgr_set, (unit, 0)));

        
        sal_usleep(30000);

        
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit,reg_val32));        

        
        SOCDNX_IF_ERR_EXIT(READ_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, &reg64_val));
        COMPILER_64_ALLONES(reg64_ones);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg64_ones));
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg64_val));
    } 
      
    sal_usleep(20000);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_fabric_topology_mesh_topology_reset(int unit)
{
    uint32 reg_val32;
    int ccs_index,nof_instances_ccs;
    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_ccs = SOC_DFE_DEFS_GET(unit, nof_instances_ccs);

    
    for (ccs_index = 0; ccs_index < nof_instances_ccs ; ccs_index++)
    {
        SOCDNX_IF_ERR_EXIT(READ_CCS_FILTERED_CELLS_CONFIGURATIONSr(unit,ccs_index,&reg_val32));
        soc_reg_field_set(unit,CCS_FILTERED_CELLS_CONFIGURATIONSr,&reg_val32,FILTER_CELLSf,1);
        SOCDNX_IF_ERR_EXIT(WRITE_CCS_FILTERED_CELLS_CONFIGURATIONSr(unit,ccs_index,reg_val32));
    }

    sal_usleep(1000);

     
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));
    
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));
    

    for (ccs_index = 0; ccs_index < nof_instances_ccs ; ccs_index++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_CCS_FILTERED_CELLS_CONFIGURATIONSr(unit,ccs_index,&reg_val32));
        soc_reg_field_set(unit,CCS_FILTERED_CELLS_CONFIGURATIONSr,&reg_val32,FILTER_CELLSf,0);
        SOCDNX_IF_ERR_EXIT(WRITE_CCS_FILTERED_CELLS_CONFIGURATIONSr(unit,ccs_index,reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
