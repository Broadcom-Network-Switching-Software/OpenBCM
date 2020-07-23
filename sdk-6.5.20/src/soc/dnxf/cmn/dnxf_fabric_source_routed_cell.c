/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF FABRIC SOURCE ROUTED CELL
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC


#include <shared/bsl.h>

#include <soc/error.h>

#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_fabric_cell.h>
#include <soc/dnxc/dnxc_cells_buffer.h>

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_fabric_source_routed_cell.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/dnxf_stack.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/mbcm.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>



#define SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION      (0)
#define SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION      (1)
#define SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION      (2)
#define SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION      (3)




shr_error_e
soc_dnxf_cpu2cpu_write(
    int                            unit,
    const dnxc_sr_cell_link_list_t  *sr_link_list,
    uint32                         data_in_size,
    uint32                         *data_in
  )
{
    dnxc_vsc256_sr_cell_t cell;
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&cell.payload.data[0], 0x0, sizeof(uint32)*DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32);
    cell.header.cell_type = soc_dnxc_fabric_cell_type_sr_cell;

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_stk_modid_get,(unit, &cell.header.source_device));
    SHR_IF_ERR_EXIT(rc);

    cell.header.source_level = sr_link_list->src_entity_type;
    cell.header.destination_level = sr_link_list->dest_entity_type;
    cell.header.fip_link = sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION];
    cell.header.fe1_link = sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION];
    cell.header.fe2_link = sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION];
    cell.header.fe3_link = sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION];
    cell.header.is_inband = 0;
    cell.header.ack = 0;  
    cell.header.pipe_id = sr_link_list->pipe_id; 
    
    sal_memcpy(cell.payload.data, data_in, 
               data_in_size < DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32 ? WORDS2BYTES(data_in_size) : WORDS2BYTES(DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32));

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_sr_cell_send,(unit, &cell));
    SHR_IF_ERR_EXIT(rc);
                            
exit:
    SHR_FUNC_EXIT;

}


shr_error_e
soc_dnxf_route2sr_link_list(
  int unit,
  const soc_dnxc_fabric_route_t *route,
  dnxc_sr_cell_link_list_t *sr_link_list
)
{ 
    int  hop_index, rc;
    bcm_fabric_link_connectivity_t link_partner;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(route, _SHR_E_PARAM, "route");
    SHR_NULL_CHECK(sr_link_list, _SHR_E_PARAM, "sr_link_list");

    if(0 == route->number_of_hops)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "zero hops");
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_connectivity_status_get,(unit, route->hop_ids[0], &link_partner));
    SHR_IF_ERR_EXIT(rc);

    switch(dnxf_data_fabric.general.device_mode_get(unit)) {
        case soc_dnxf_fabric_device_mode_single_stage_fe2:
        case soc_dnxf_fabric_device_mode_multi_stage_fe2:
            sr_link_list->src_entity_type = bcmFabricDeviceTypeFE2;
            break;
        case soc_dnxf_fabric_device_mode_multi_stage_fe13:
            if(bcmFabricDeviceTypeFE2 == link_partner.device_type) {
                sr_link_list->src_entity_type = bcmFabricDeviceTypeFE1;
            } else {
                sr_link_list->src_entity_type = bcmFabricDeviceTypeFE3;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "failed to get source device");
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
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "failed to get destination device");
            }
            break;

        case 2:
            if(bcmFabricDeviceTypeFE2 == link_partner.device_type)
                sr_link_list->dest_entity_type = bcmFabricDeviceTypeFE3;
            else if(bcmFabricDeviceTypeFE13 == link_partner.device_type || bcmFabricDeviceTypeFE3 == link_partner.device_type )
                sr_link_list->dest_entity_type = bcmFabricDeviceTypeFOP;
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "failed to get destination device");
            }
            break;

        case 3:
            if(bcmFabricDeviceTypeFE2 == link_partner.device_type)
                sr_link_list->dest_entity_type = bcmFabricDeviceTypeFOP;
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "failed to get destination device");
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "number_of_hops %d is out of limit",route->number_of_hops);
    }

    
    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION] = 0;
    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION] = 0;
    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION] = 0;
    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION] = 0;

    for (hop_index=0; hop_index<route->number_of_hops; hop_index++)
    {
        switch (hop_index)
        {
            case 0:
                if (sr_link_list->src_entity_type == dnxcFabricDeviceTypeFE2)
                {
                    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
                }
                else if (sr_link_list->src_entity_type == dnxcFabricDeviceTypeFE1)
                {
                    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
                }
                else if (sr_link_list->src_entity_type == dnxcFabricDeviceTypeFE3) 
                {
                    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
                } else 
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "source entity %d is unknown",sr_link_list->src_entity_type);
                }
                break;
            case 1:
                if (sr_link_list->src_entity_type == dnxcFabricDeviceTypeFE2)
                {            
                    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
                }
                else if (sr_link_list->src_entity_type == dnxcFabricDeviceTypeFE1)
                {
                    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE2_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
                }
                else if (sr_link_list->src_entity_type == dnxcFabricDeviceTypeFE3)
                {
                    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
                }
                else 
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "source entity %d is unknown",sr_link_list->src_entity_type);
                }
                break;
            case 2:
                if (sr_link_list->src_entity_type == dnxcFabricDeviceTypeFE2)
                {            
                    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FIP_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
                }
                else if (sr_link_list->src_entity_type == dnxcFabricDeviceTypeFE1)
                {
                    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE3_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
                }
                else if (sr_link_list->src_entity_type == dnxcFabricDeviceTypeFE3)
                {
                    sr_link_list->path_links[SOC_DNXF_FABRIC_SRC_VSC256_V1_CELL_PATH_LINK_FE1_SWITCH_POSITION] = (uint8) route->hop_ids[hop_index];
                }
                else 
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "source entity %d is unknown",sr_link_list->src_entity_type);
                }
                break;
        }
    }

    
    sr_link_list->pipe_id = route->pipe_id;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_dnxf_sr_cell_send( 
  int unit, 
  uint32 flags, 
  soc_dnxc_fabric_route_t* route, 
  uint32 data_in_size, 
  uint32 *data_in
)
{
    shr_error_e rc = _SHR_E_NONE;
    dnxc_sr_cell_link_list_t sr_link_list;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
  
    SHR_NULL_CHECK(route, _SHR_E_PARAM, "route");
    SHR_NULL_CHECK(data_in, _SHR_E_PARAM, "data_in");
  
    DNXF_UNIT_LOCK_TAKE(unit);

    if(0 == route->number_of_hops) {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "route has zero hops");
    }
  
    rc = soc_dnxf_route2sr_link_list(unit, route, &sr_link_list);
   SHR_IF_ERR_EXIT(rc);

    rc = soc_dnxf_cpu2cpu_write(unit, &sr_link_list, data_in_size,data_in);
    SHR_IF_ERR_EXIT(rc);
    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_dnxf_sr_cell_payload_receive( 
 int unit, 
 uint32 flags, 
 uint32 data_out_max_size, 
 uint32 *data_out_size, 
 uint32 *data_out
)
{
    shr_error_e rc = _SHR_E_NONE;
    dnxc_vsc256_sr_cell_t cell;
    uint32 *payload_start_ptr = data_out;
    int header_offset = 0, header_field_idx, value;

    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
  
    sal_memset(&cell, 0x00, sizeof(dnxc_vsc256_sr_cell_t));
    DNXF_UNIT_LOCK_TAKE(unit);

    rc = soc_dnxf_sr_cell_receive(unit, &cell);
    SHR_IF_ERR_EXIT(rc);

    (*data_out_size) = data_out_max_size<DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32 ? data_out_max_size : DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32;

    if (flags == _SHR_FABRIC_CELL_RX_HEADER_PREPEND)
    {
        header_offset = _shr_dnxc_fabric_cell_header_field_type_nof;
        (*data_out_size) += _shr_dnxc_fabric_cell_header_field_type_nof;

        for (header_field_idx = 0; header_field_idx < _shr_dnxc_fabric_cell_header_field_type_nof; header_field_idx++)
        {
            switch(header_field_idx)
            {
                case _shr_dnxc_fabric_cell_header_field_type_source_device:
                    value = cell.header.source_device;
                    break;
                case _shr_dnxc_fabric_cell_header_field_type_fip_link:
                    value = cell.header.fip_link;
                    break;
                case _shr_dnxc_fabric_cell_header_field_type_pipe:
                    value = cell.header.pipe_id;
                    break;
                
                default:
                    
                    break;

            }
            data_out[header_field_idx] = value;
        }

        
        payload_start_ptr = data_out + header_offset;
    }

    sal_memcpy(payload_start_ptr, cell.payload.data, WORDS2BYTES(*data_out_size - header_offset));
  
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_dnxf_sr_cell_receive(int unit, dnxc_vsc256_sr_cell_t* cell)
{
    int rv, is_empty;
    SHR_FUNC_INIT_VARS(unit);
    
    
    rv = dnxc_cells_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->sr_cells_buffer, &is_empty);
    SHR_IF_ERR_EXIT(rv);

    
    
    while (is_empty) 
    {
        rv = soc_dnxf_fabric_cell_load(unit);
        if (rv == _SHR_E_EMPTY)
        {
            break;
        }
        SHR_IF_ERR_EXIT(rv);

        rv = dnxc_cells_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->sr_cells_buffer, &is_empty);
        SHR_IF_ERR_EXIT(rv);
    }
    if (is_empty)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

    
    rv = dnxc_cells_buffer_get(unit, &SOC_DNXF_CONTROL(unit)->sr_cells_buffer, cell);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT; 
}


#undef BSL_LOG_MODULE

