/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DFE FABRIC SOURCE ROUTED CELL
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC


#include <shared/bsl.h>
#include <bcm/fabric.h>

#include <soc/error.h>

#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_fabric_cell.h>
#include <soc/dcmn/dcmn_cells_buffer.h>

#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_fabric_source_routed_cell.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/dfe_stack.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/mbcm.h>



#define SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION      (0)
#define SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION      (1)
#define SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION      (2)
#define SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION      (3)




soc_error_t
soc_dfe_cpu2cpu_write(
    int                            unit,
    const dcmn_sr_cell_link_list_t  *sr_link_list,
    uint32                         data_in_size,
    uint32                         *data_in
  )
{
    vsc256_sr_cell_t cell;
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(&cell.payload.data[0], 0x0, sizeof(uint32)*VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32);
    cell.header.cell_type = soc_dcmn_fabric_cell_type_sr_cell;

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_stk_modid_get,(unit, &cell.header.source_device));
    SOCDNX_IF_ERR_EXIT(rc);

    cell.header.source_level = sr_link_list->src_entity_type;
    cell.header.destination_level = sr_link_list->dest_entity_type;
    cell.header.fip_link = sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION];
    cell.header.fe1_link = sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION];
    cell.header.fe2_link = sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION];
    cell.header.fe3_link = sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION];
    cell.header.is_inband = 0;
    cell.header.ack = 0;  
    cell.header.pipe_id = sr_link_list->pipe_id; 
    
    sal_memcpy(cell.payload.data, data_in, 
               data_in_size < VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32 ? WORDS2BYTES(data_in_size) : WORDS2BYTES(VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32));

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_sr_cell_send,(unit, &cell));
    SOCDNX_IF_ERR_EXIT(rc);
                            
exit:
    SOCDNX_FUNC_RETURN;

}

soc_error_t
soc_dfe_sr_cell_format_type_get(int unit, const vsc256_sr_cell_t* cell, soc_dfe_fabric_link_cell_size_t* vsc_format)
{
    soc_port_t link;
    soc_error_t rc;
    SOCDNX_INIT_FUNC_DEFS;

    
    switch (cell->header.source_level)
    {
    case bcmFabricDeviceTypeFE2:
        link = cell->header.fe2_link;
        break;
    case bcmFabricDeviceTypeFE1:
        link = cell->header.fe1_link;
        break;
    case bcmFabricDeviceTypeFE3:
        link = cell->header.fe3_link;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("source level %d is unknown"),cell->header.source_level));
    }

    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_cell_format_get,(unit, link, vsc_format));
    SOCDNX_IF_ERR_EXIT(rc);
    
exit:
    SOCDNX_FUNC_RETURN;     
}


soc_error_t
soc_dfe_route2sr_link_list(
  int unit,
  const soc_dcmn_fabric_route_t *route,
  dcmn_sr_cell_link_list_t *sr_link_list
)
{ 
    int  hop_index, rc;
    bcm_fabric_link_connectivity_t link_partner;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(route);
    SOCDNX_NULL_CHECK(sr_link_list);
    
    if(0 == route->number_of_hops)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("zero hops")));
    }
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_connectivity_status_get,(unit, route->hop_ids[0], &link_partner));
    SOCDNX_IF_ERR_EXIT(rc);
      
    switch(SOC_DFE_CONFIG(unit).fabric_device_mode) {
        case soc_dfe_fabric_device_mode_single_stage_fe2:
        case soc_dfe_fabric_device_mode_multi_stage_fe2:
            sr_link_list->src_entity_type = bcmFabricDeviceTypeFE2;
            break;
        case soc_dfe_fabric_device_mode_multi_stage_fe13:
            if(bcmFabricDeviceTypeFE2 == link_partner.device_type) {
                sr_link_list->src_entity_type = bcmFabricDeviceTypeFE1;
            } else {
                sr_link_list->src_entity_type = bcmFabricDeviceTypeFE3;
            }
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("failed to get source device")));
    }
    

    switch(route->number_of_hops) 
    {
        case 1:
            if(bcmFabricDeviceTypeFE2 == link_partner.device_type)
                    sr_link_list->dest_entity_type = bcmFabricDeviceTypeFE2;
                else if(bcmFabricDeviceTypeFAP == link_partner.device_type || bcmFabricDeviceTypeFOP == link_partner.device_type )
                    sr_link_list->dest_entity_type = bcmFabricDeviceTypeFOP;
                else if(bcmFabricDeviceTypeFE13 == link_partner.device_type || bcmFabricDeviceTypeFE3 == link_partner.device_type )
                    sr_link_list->dest_entity_type = bcmFabricDeviceTypeFE3;
                else
                {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("failed to get destination device")));
                }
              break;
              
        case 2:
                if(bcmFabricDeviceTypeFE2 == link_partner.device_type)
                    sr_link_list->dest_entity_type = bcmFabricDeviceTypeFE3;
                else if(bcmFabricDeviceTypeFE13 == link_partner.device_type || bcmFabricDeviceTypeFE3 == link_partner.device_type )
                    sr_link_list->dest_entity_type = bcmFabricDeviceTypeFOP;
                else
                {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("failed to get destination device")));
                }
              break;
              
        case 3:
                if(bcmFabricDeviceTypeFE2 == link_partner.device_type)
                    sr_link_list->dest_entity_type = bcmFabricDeviceTypeFOP;
                else
                {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("failed to get destination device")));
                }
              break;
              
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("number_of_hops %d is out of limit"),route->number_of_hops));
    }
    
    
    sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION] = 0;
    sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION] = 0;
    sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION] = 0;
    sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION] = 0;

    for (hop_index=0; hop_index<route->number_of_hops; hop_index++)
    {
        switch (hop_index)
        {
        case 0:
            if (sr_link_list->src_entity_type == dcmnFabricDeviceTypeFE2)
            {
                sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
            }
            else if (sr_link_list->src_entity_type == dcmnFabricDeviceTypeFE1)
            {
                sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
            }
            else if (sr_link_list->src_entity_type == dcmnFabricDeviceTypeFE3) 
            {
                sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
            } else 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("source entity %d is unknown"),sr_link_list->src_entity_type));
            }
            break;
        case 1:
            if (sr_link_list->src_entity_type == dcmnFabricDeviceTypeFE2)
            {            
                sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
            }
            else if (sr_link_list->src_entity_type == dcmnFabricDeviceTypeFE1)
            {
                sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
            }
            else if (sr_link_list->src_entity_type == dcmnFabricDeviceTypeFE3)
            {
                sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
            }
            else 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("source entity %d is unknown"),sr_link_list->src_entity_type));
            }
            break;
        case 2:
            if (sr_link_list->src_entity_type == dcmnFabricDeviceTypeFE2)
            {            
                sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
            }
            else if (sr_link_list->src_entity_type == dcmnFabricDeviceTypeFE1)
            {
                sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
            }
            else if (sr_link_list->src_entity_type == dcmnFabricDeviceTypeFE3)
            {
                sr_link_list->path_links[SOC_DFE_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
            }
            else 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("source entity %d is unknown"),sr_link_list->src_entity_type));
            }
            break;
        }
    }

    
    sr_link_list->pipe_id = route->pipe_id;
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dfe_sr_cell_send( 
  int unit, 
  uint32 flags, 
  soc_dcmn_fabric_route_t* route, 
  uint32 data_in_size, 
  uint32 *data_in
)
{
    soc_error_t rc = SOC_E_NONE;
    dcmn_sr_cell_link_list_t sr_link_list;
    SOCDNX_INIT_FUNC_DEFS;

  
    SOCDNX_NULL_CHECK(route);
    SOCDNX_NULL_CHECK(data_in);
  
    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    if(0 == route->number_of_hops) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("route has zero hops")));
    }
  
    rc = soc_dfe_route2sr_link_list(unit, route, &sr_link_list);
   SOCDNX_IF_ERR_EXIT(rc);

    rc = soc_dfe_cpu2cpu_write(unit, &sr_link_list, data_in_size,data_in);
    SOCDNX_IF_ERR_EXIT(rc);
    
exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dfe_sr_cell_payload_receive( 
 int unit, 
 uint32 flags, 
 uint32 data_out_max_size, 
 uint32 *data_out_size, 
 uint32 *data_out
)
{
    soc_error_t rc = SOC_E_NONE;
    vsc256_sr_cell_t cell;
    SOCDNX_INIT_FUNC_DEFS;
    
  
    sal_memset(&cell, 0x00, sizeof(vsc256_sr_cell_t));
    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    rc = soc_dfe_sr_cell_receive(unit, &cell);
    SOCDNX_IF_ERR_EXIT(rc);

    (*data_out_size) = data_out_max_size<VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32 ? data_out_max_size : VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32;
    sal_memcpy(data_out, cell.payload.data, WORDS2BYTES(*data_out_size));
  
exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dfe_sr_cell_receive(int unit, vsc256_sr_cell_t* cell)
{
    int rv, is_empty;
    SOCDNX_INIT_FUNC_DEFS;
    
    
    rv = dcmn_cells_buffer_is_empty(unit, &SOC_DFE_CONTROL(unit)->sr_cells_buffer, &is_empty);
    SOCDNX_IF_ERR_EXIT(rv);

    
    
    while (is_empty) 
    {
        rv = soc_dfe_fabric_cell_load(unit);
        if (rv == SOC_E_EMPTY)
        {
            break;
        }
        SOCDNX_IF_ERR_EXIT(rv);

        rv = dcmn_cells_buffer_is_empty(unit, &SOC_DFE_CONTROL(unit)->sr_cells_buffer, &is_empty);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    if (is_empty)
    {
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_EMPTY);
    }

    
    rv = dcmn_cells_buffer_get(unit, &SOC_DFE_CONTROL(unit)->sr_cells_buffer, cell);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN; 
}


#undef _ERR_MSG_MODULE_NAME

