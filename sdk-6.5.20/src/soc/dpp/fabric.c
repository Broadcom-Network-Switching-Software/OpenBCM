/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC

#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_fabric_source_routed_cell.h>

#include <soc/dpp/mbcm.h>
#include <soc/dpp/fabric.h>
#include <soc/dpp/drv.h>
#ifdef BCM_ARAD_SUPPORT
  #include <soc/dpp/ARAD/arad_api_mgmt.h>
  #include <soc/dpp/ARAD/arad_cell.h>
  #include <soc/dpp/ARAD/arad_fabric.h>
#endif

#define NOF_COMMANDS(unit, flags)\
(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT)?\
    1:\
    SOC_IS_ARAD(unit) ? 5:1


int
soc_dpp_fabric_inband_route2sr_link_list(
  int unit,
  const soc_fabric_inband_route_t* route,
  SOC_TMC_SR_CELL_LINK_LIST *sr_link_list
)
{
  int 
    rv,
    hop_index;
  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR 
    connectivity_map;
  int
    soc_sand_rv = 0;
  uint8
    is_multi_stage;
  int nof_links = 0;
  SOCDNX_INIT_FUNC_DEFS;

  sal_memset(sr_link_list, 0, sizeof(SOC_TMC_SR_CELL_LINK_LIST));

  SOCDNX_NULL_CHECK(sr_link_list);

  soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_nof_links_get,(unit, &nof_links)));
  SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(&connectivity_map);
  soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_topology_status_connectivity_get,(unit, 0, nof_links - 1, &connectivity_map)));
  SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

  
  if (route->number_of_hops > FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS)
  {
    rv = SOC_E_LIMIT;
    goto soc_dpp_attach_error;
  }
  
  if ( route->number_of_hops == 0)
  {
    sr_link_list->dest_entity_type = SOC_SAND_FIP_ENTITY;
  }
  else if (route->number_of_hops == 1)
  {
    if (connectivity_map.link_info[(route->link_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE1)
    {
      sr_link_list->dest_entity_type = SOC_SAND_FE1_ENTITY;
    } 
    else if (connectivity_map.link_info[(route->link_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE2)
    {
      sr_link_list->dest_entity_type = SOC_SAND_FE2_ENTITY;
    }
    else if (connectivity_map.link_info[(route->link_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FAP)
    {
      sr_link_list->dest_entity_type = SOC_SAND_FOP_ENTITY;
    }
    else
    {
      return SOC_E_LIMIT;
    }
  }
  else if (route->number_of_hops == 2)
  {
    if (connectivity_map.link_info[(route->link_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE1)
    {
      sr_link_list->dest_entity_type = SOC_SAND_FE2_ENTITY;
    } 
    else if (connectivity_map.link_info[(route->link_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE2)
    {
      sr_link_list->dest_entity_type = SOC_SAND_FOP_ENTITY;
    }
    else
    {
      return SOC_E_LIMIT;
    }
  }
  else if (route->number_of_hops == 3)
  {
    sr_link_list->dest_entity_type = SOC_SAND_FE3_ENTITY;
  }
  else if (route->number_of_hops == 4)
  {
    sr_link_list->dest_entity_type = SOC_SAND_FOP_ENTITY;
  }
  else
  {
    return SOC_E_LIMIT;
  }
  
  if(connectivity_map.link_info[(route->link_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE2)
  {
    is_multi_stage = FALSE;
  }
  else
  {
    is_multi_stage = TRUE;
  }

  if(is_multi_stage)
  {
    for (hop_index=0; hop_index<route->number_of_hops; hop_index++)
    {
      sr_link_list->path_links[hop_index] = (uint8) route->link_ids[hop_index];
    }
  }
  else
  {
    sr_link_list->path_links[0] = (uint8) route->link_ids[0];
    sr_link_list->path_links[1] = 0;
    sr_link_list->path_links[2] = (uint8) route->link_ids[1];
    sr_link_list->path_links[3] = 0;
  }
  return SOC_E_NONE;  

soc_dpp_attach_error:
  LOG_ERROR(BSL_LS_SOC_FABRIC,
            (BSL_META_U(unit,
                        "soc_dpp_fabric_route2sr_link_list: (%s)\n"), 
                        soc_errmsg(rv)));    
  return rv;
exit:
  SOCDNX_FUNC_RETURN;
}


int
soc_dpp_fabric_route2sr_link_list(
  int unit,
  const soc_dcmn_fabric_route_t* route,
  dcmn_sr_cell_link_list_t *sr_link_list
)
{
    int hop_index;
    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR connectivity_map;
    uint8 is_multi_stage;
    int nof_links = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(sr_link_list);
    nof_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links);

    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(&connectivity_map);
    SOCDNX_SAND_IF_ERR_RETURN(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_topology_status_connectivity_get,
                                                   (unit, 0, nof_links - 1, &connectivity_map)));

    switch (route->number_of_hops) {
    case 0:
        sr_link_list->dest_entity_type = dcmnFabricDeviceTypeFIP;
        break;
    case 1:
        if (connectivity_map.link_info[(route->hop_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE1) {
            sr_link_list->dest_entity_type = dcmnFabricDeviceTypeFE1;
        } 
        else if (connectivity_map.link_info[(route->hop_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE2) {
            sr_link_list->dest_entity_type = dcmnFabricDeviceTypeFE2;
        }
        else if (connectivity_map.link_info[(route->hop_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FAP) {
            sr_link_list->dest_entity_type = dcmnFabricDeviceTypeFOP;
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("failed to get destination device")));
        }
        break;
    case 2:
        if (connectivity_map.link_info[(route->hop_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE1) {
            sr_link_list->dest_entity_type = dcmnFabricDeviceTypeFE2;
        } 
        else if (connectivity_map.link_info[(route->hop_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE2) {
            sr_link_list->dest_entity_type = dcmnFabricDeviceTypeFOP;
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("failed to get destination device")));
        }
        break;
    case 3:
        sr_link_list->dest_entity_type = SOC_SAND_FE3_ENTITY;
        break;
    case 4:
        sr_link_list->dest_entity_type = SOC_SAND_FOP_ENTITY;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("number_of_hops %d is out of limit"), route->number_of_hops));
    }

    is_multi_stage = (connectivity_map.link_info[(route->hop_ids[0])].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE2) ? FALSE : TRUE; 
    
    if(is_multi_stage) {
        for (hop_index = 0; hop_index < route->number_of_hops; ++hop_index) {
            sr_link_list->path_links[hop_index] = (uint8) route->hop_ids[hop_index];
        }
    }
    else {
        sr_link_list->path_links[0] = (uint8) route->hop_ids[0];
        sr_link_list->path_links[1] = 0;
        sr_link_list->path_links[2] = (uint8) route->hop_ids[1];
        sr_link_list->path_links[3] = 0;
    }
 
    
    sr_link_list->pipe_id = route->pipe_id;

exit:
  SOCDNX_FUNC_RETURN;
}


STATIC int
soc_dpp_fabric_inband_route_is_valid(
  int unit,
  int route_id,
  uint8 *is_valid
)
{
#ifdef BCM_ARAD_SUPPORT
  uint32
    is_valid_lcl;
#endif

  SOCDNX_INIT_FUNC_DEFS;
  
  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(is_valid);
  
  if (route_id >= SOC_DPP_NUM_OF_ROUTES) {
    *is_valid = FALSE;
    SOC_EXIT;
  }

#ifdef BCM_ARAD_SUPPORT
  if(SOC_IS_ARAD(unit)){
    SOCDNX_IF_ERR_EXIT(soc_dpp_arad_fabric_inband_is_valid(
      unit,
      route_id,
      &is_valid_lcl
    ));
    *is_valid = is_valid_lcl;
  }
#endif 

  *is_valid = (SOC_DPP_CONTROL(unit))->inband_route_entries[route_id].is_valid;
exit:
  SOCDNX_FUNC_RETURN;
}

STATIC int
soc_dpp_fabric_inband_route_group_is_valid(
  int unit,
  int group_id,
  uint8 *is_valid
)
{
#ifdef BCM_ARAD_SUPPORT
  uint32
    is_valid_lcl;
#endif

  SOCDNX_INIT_FUNC_DEFS;
  
  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(is_valid);
  
  if (group_id > SOC_DPP_MAX_NUM_OF_ROUTE_GROUPS) {
    *is_valid = FALSE;
    SOC_EXIT;
  }

#ifdef BCM_ARAD_SUPPORT
  if(SOC_IS_ARAD(unit)){
    soc_dpp_arad_fabric_inband_is_valid(
      unit,
      group_id,
      &is_valid_lcl
    );
    *is_valid = is_valid_lcl;
  }
#endif 

  *is_valid = (SOC_DPP_CONTROL(unit))->inband_route_entries[group_id].is_group_valid;
exit:
  SOCDNX_FUNC_RETURN;
}

int
soc_dpp_fabric_inband_route_group_set(
  int unit,
  int group_id,
  int flags,
  int route_count, 
  int *route_ids
)
{
  int
    rv;
  uint32 
    route_index;
  soc_dpp_route_entry_t
    *route_entries;
  soc_fabric_inband_route_t
    route;
  SOCDNX_INIT_FUNC_DEFS;

  if (group_id > SOC_DPP_MAX_NUM_OF_ROUTE_GROUPS){
    rv = SOC_E_LIMIT;
    goto soc_dpp_attach_error;
  }

  if(route_count > SOC_DPP_NUM_OF_ROUTES) {
    rv = SOC_E_LIMIT;
    goto soc_dpp_attach_error;
  }

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(route_ids);

  
  for (route_index = 0; route_index <route_count; route_index++){
    rv = soc_dpp_fabric_inband_route_get(
      unit,
      route_ids[route_index],
      &route
    );
    if(SOC_FAILURE(rv)) {
      goto soc_dpp_attach_error;
    }
    if (route.number_of_hops == 0) {
      rv = SOC_E_PARAM;
      return rv;
    }
  }

#ifdef BCM_ARAD_SUPPORT
  if(SOC_IS_ARAD(unit)) {
    rv = soc_dpp_arad_fabric_inband_route_group_set(
      unit,
      group_id,
      flags,
      route_count,
      route_ids
    );
    return rv;
  }
#endif 

  route_entries = (SOC_DPP_CONTROL(unit))->inband_route_entries;

  for (route_index = 0; route_index <route_count; route_index++){
    route_entries[route_ids[route_index]].group = group_id;
    route_entries[route_ids[route_index]].is_group_valid = TRUE;
  }

  return SOC_E_NONE;

soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_inband_route_group_set: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
    return rv;

exit:
  SOCDNX_FUNC_RETURN;
}

int
soc_dpp_fabric_inband_route_set(
  int unit, 
  int route_id, 
  soc_fabric_inband_route_t *route
)
{
  int 
    rv,
    index;  
  soc_dpp_route_entry_t
    *entry;
  SOCDNX_INIT_FUNC_DEFS;

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(route);

  if (route_id >= SOC_DPP_NUM_OF_ROUTES)
  {
    rv = SOC_E_LIMIT;
    goto soc_dpp_attach_error;
  }

  if(route->number_of_hops > 3) {
    rv = SOC_E_PARAM;
    return rv;
  }

  
  for(index = 0; index < route->number_of_hops; ++index) {
    if(route->link_ids[index] < 0) {
      rv = SOC_E_PARAM;
      return rv;
    }

    if(index < 2 && route->link_ids[index] >= 64) {
      rv = SOC_E_PARAM;
      return rv;
    }

    if(index == 2 && route->link_ids[index] >= 128) {
      rv = SOC_E_PARAM;
      return rv;
    }
  }

#ifdef BCM_ARAD_SUPPORT
  if(SOC_IS_ARAD(unit)){
    rv = soc_dpp_arad_fabric_inband_route_set(
      unit,
      route_id,
      route
    );
    return rv;
  }
#endif 

  entry = &(SOC_DPP_CONTROL(unit))->inband_route_entries[route_id];

  if(route->number_of_hops == 0) {
    entry->is_valid = FALSE;
    return SOC_E_NONE;
  }
  entry->route = *route;
  entry->is_valid = TRUE;
  entry->group = 0;
  entry->is_group_valid = FALSE;

  return SOC_E_NONE;  

soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_inband_route_set: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
    return rv;
exit:
  SOCDNX_FUNC_RETURN;
}

int
soc_dpp_fabric_inband_route_group_get( 
  int unit, 
  int group_id, 
  int flags, 
  int route_count_max, 
  int *route_count, 
  int *route_ids 
) 
{
  int
    rv;
  uint32
    entry_index;
  soc_dpp_route_entry_t
    *route_entries;
  SOCDNX_INIT_FUNC_DEFS;

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(route_count);
  SOCDNX_NULL_CHECK(route_ids);

  if (group_id > SOC_DPP_MAX_NUM_OF_ROUTE_GROUPS)
  {
    rv = SOC_E_LIMIT;
    goto soc_dpp_attach_error;
  }

#ifdef BCM_ARAD_SUPPORT
  if(SOC_IS_ARAD(unit)) {
    rv = soc_dpp_arad_fabric_inband_route_group_get(
      unit,
      group_id,
      flags,
      route_count_max,
      route_count,
      route_ids
    );
    return rv;
  }
#endif 

  route_entries = (SOC_DPP_CONTROL(unit))->inband_route_entries;

  *route_count = 0;
  for (entry_index = 0; entry_index < SOC_DPP_NUM_OF_ROUTES; entry_index++)
  {
    if(route_entries[entry_index].is_valid
      && route_entries[entry_index].is_group_valid
      && route_entries[entry_index].group == group_id
    ) {
      route_ids[*route_count] = entry_index;    
      if (++(*route_count) > route_count_max)
      {
        rv = SOC_E_LIMIT;
        goto soc_dpp_attach_error;
      }
    }
  }

  return SOC_E_NONE;

  soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_inband_route_group_get: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
    return rv;
exit:
  SOCDNX_FUNC_RETURN;
}

int
soc_dpp_fabric_inband_route_get(
  int unit, 
  int route_id, 
  soc_fabric_inband_route_t *route
)
{
  int 
    rv;
  SOCDNX_INIT_FUNC_DEFS;

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(route);

  if (route_id >= SOC_DPP_NUM_OF_ROUTES)
  {
    rv = SOC_E_LIMIT;
    goto soc_dpp_attach_error;
  }

#ifdef BCM_ARAD_SUPPORT
  if(SOC_IS_ARAD(unit)){
    rv = soc_dpp_arad_fabric_inband_route_get(
      unit,
      route_id,
      route
    );
    if(SOC_FAILURE(rv)){
      goto soc_dpp_attach_error;
    }
    return rv;
  }
#endif 

  if(!(SOC_DPP_CONTROL(unit))->inband_route_entries[route_id].is_valid)
  {
    rv = SOC_E_PARAM;
    goto soc_dpp_attach_error;
  }
  *route = (SOC_DPP_CONTROL(unit))->inband_route_entries[route_id].route;
  
  return SOC_E_NONE;

soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_inband_route_get: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
    return rv;
exit:
  SOCDNX_FUNC_RETURN;
}


uint32 
soc_dpp_fabric_sr_cell_send( 
  int unit, 
  uint32 flags, 
  soc_dcmn_fabric_route_t* route,
  uint32 data_in_size, 
  uint32 *data_in
)
{
    dcmn_sr_cell_link_list_t sr_link_list;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(route);
    SOCDNX_NULL_CHECK(data_in);

    SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_route2sr_link_list(unit, route, &sr_link_list));
  
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_cpu2cpu_write,(unit, &sr_link_list, data_in_size, data_in)));

exit:
  SOCDNX_FUNC_RETURN;
}

uint32 
soc_dpp_fabric_sr_cell_receive(
  int unit, 
  uint32 flags, 
  uint32 data_out_max_size, 
  uint32 *data_out_size, 
  uint32 *data_out
)
{
    soc_dcmn_fabric_cell_entry_t entry;
    vsc256_sr_cell_t data_cell_rcv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(data_out_size);
    SOCDNX_NULL_CHECK(data_out);
    
    sal_memset(&data_cell_rcv, 0x0, sizeof(vsc256_sr_cell_t));
    sal_memset(entry, 0x0, sizeof(entry));
  
    
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_cell_cpu_data_get, (unit, (uint32*)entry)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_cell_parse(unit, entry, &data_cell_rcv));
        
    (*data_out_size) = data_out_max_size < WORDS2BYTES(VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32) ? 
                       data_out_max_size : WORDS2BYTES(VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32) ;

    sal_memcpy(data_out, data_cell_rcv.payload.data, *data_out_size);

exit:
  SOCDNX_FUNC_RETURN;
}


int 
  soc_dpp_fabric_reg_read(
    int unit, 
    uint32 flags, 
    int route_id,
    int max_count, 
    soc_reg_t *reg,  
    int *port,
    int *index,
    uint64 *data, 
    int *array_count
  )
{
  int
  regs_index,
    i,
    rv = SOC_E_NONE;    
  SOC_TMC_SR_CELL_LINK_LIST 
    sr_link_list;  
  uint32 
    soc_sand_dev_id;
  uint32
    temp_data_32[64/32],
    soc_sand_rv = SOC_E_UNAVAIL;
  soc_fabric_inband_route_t
    route;
  uint8
    nof_commands,
    nof_regs_in_cell,
    is_valid;

#ifdef BCM_ARAD_SUPPORT
  soc_reg_above_64_val_t
    temp_data[VSC256_MAX_COMMANDS_ARRAY_SIZE];
#endif 

  SOCDNX_INIT_FUNC_DEFS;

  
  if(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT) {
    for(i = 0; i < max_count; ++i) {
      if(SOC_REG_IS_ABOVE_64(unit, reg[i])) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("for regs above 64 use soc_dpp_fabric_reg_above_64_write")));
      }
    }
  }

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  sal_memset(&sr_link_list, 0, sizeof(SOC_TMC_SR_CELL_LINK_LIST));
  sal_memset(&route, 0, sizeof(soc_fabric_inband_route_t));

  SOCDNX_NULL_CHECK(reg);
  SOCDNX_NULL_CHECK(port);
  SOCDNX_NULL_CHECK(index);
  SOCDNX_NULL_CHECK(data);
  SOCDNX_NULL_CHECK(array_count);

  soc_sand_dev_id = unit;
  
  if(SOC_FABRIC_INBAND_IS_GROUP(route_id)) {
    rv = SOC_E_LIMIT;
    goto soc_dpp_attach_error;
  }
  
  
  SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_is_valid(unit, route_id, &is_valid));
  if(!is_valid) {
    rv = SOC_E_PARAM;
    goto soc_dpp_attach_error;
  }

  
#ifdef BCM_ARAD_SUPPORT
     if (SOC_IS_ARAD(unit)) {
      rv = soc_dpp_arad_fabric_inband_route_get(
        unit,
        route_id,
        &route
      );

      if (SOC_FAILURE(rv)) 
      {
        goto soc_dpp_attach_error;
      }
    }
    else
#endif 
  {
        LOG_ERROR(BSL_LS_SOC_FABRIC,
                  (BSL_META_U(unit,
                              "soc_dpp_fabric_send_sr_cell: support only ARAD and PETRAB devices\n")));    
        return SOC_E_UNAVAIL;
  }

  rv = soc_dpp_fabric_inband_route2sr_link_list(
    unit,
    &route,
    &sr_link_list
  );
  
  if (SOC_FAILURE(rv)) 
  {
    goto soc_dpp_attach_error;
  }
  

  nof_commands = NOF_COMMANDS(unit, flags);


  for (regs_index=0; regs_index < max_count; regs_index += nof_commands)
  {
    if (!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT))
    {
      nof_regs_in_cell = max_count - regs_index;
      if(nof_regs_in_cell > nof_commands) {
        nof_regs_in_cell = nof_commands;
      }

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        soc_sand_rv = arad_transaction_with_fe1600(
          soc_sand_dev_id,
          &sr_link_list,
          0,
          FALSE,
          &reg[regs_index],
          NULL,
          &port[regs_index],
          &index[regs_index],
          NULL,
          nof_regs_in_cell,
          FALSE,
          TRUE,
          FALSE,
          temp_data,
          NULL
        );
        for(i = 0; i< nof_regs_in_cell; ++i){
          if(SOC_REG_IS_64(unit, reg[regs_index + i])) {
            sal_memcpy(&data[regs_index + i], temp_data[i], 64/8);
          }
          else{
            sal_memcpy(&data[regs_index + i], temp_data[i], 32/8);
          }
        }
      }
#endif 

      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);  
    } 
    else
    {
#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        soc_sand_rv = arad_read_from_fe600_unsafe(
          soc_sand_dev_id,
          &sr_link_list,
          sizeof(uint32),
          reg[regs_index],
          temp_data_32
        );
      }
#endif 

      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
      sal_memcpy(&data[regs_index], temp_data_32, 32/8);
    }
  }
  *array_count = max_count;
  return SOC_E_NONE; 

soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_send_sr_cell: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
exit:
    SOCDNX_FUNC_RETURN;
}

int 
  soc_dpp_fabric_reg_write(
   int unit, 
   uint32 flags, 
   int route_id,
   int array_count, 
   soc_reg_t *reg, 
   int *port,
   int *index,
   uint64 *data
  )
{
  int
    regs_index,
    route_index,
    routes_num,
    *routes_ids = NULL,
    nof_commands,
    nof_regs_in_cell,
    i,
    rv = SOC_E_NONE;
  SOC_TMC_SR_CELL_LINK_LIST 
    sr_link_list;  
  uint32 
    soc_sand_dev_id;
  uint32
#ifdef BCM_ARAD_SUPPORT
    arad_temp_data_32[VSC256_MAX_COMMANDS_ARRAY_SIZE*(128/32)],
#endif
    soc_sand_rv = SOC_E_UNAVAIL; 
  soc_fabric_inband_route_t
    route;
  uint8
    is_valid;
    
  SOCDNX_INIT_FUNC_DEFS;

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(reg);
  SOCDNX_NULL_CHECK(port);
  SOCDNX_NULL_CHECK(index);
  SOCDNX_NULL_CHECK(data);

  
  if(!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT)) {
    for(i = 0; i < array_count; ++i) {
      if(SOC_REG_IS_ABOVE_64(unit, reg[i])) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("for regs above 64 use soc_dpp_fabric_reg_above_64_write")));
      }
    }
  }

  sal_memset(&sr_link_list, 0, sizeof(SOC_TMC_SR_CELL_LINK_LIST));

  soc_sand_dev_id = unit;
  
  nof_commands = NOF_COMMANDS(unit, flags);

  routes_ids = sal_alloc(sizeof(int)*SOC_DPP_NUM_OF_ROUTES, "soc_dpp_fabric_reg_write.routes_ids");
  if(routes_ids == NULL) {
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_reg_write: unit %d failed - allocation failure\n"), unit));    
    goto soc_dpp_attach_error;
  }
  
  for(regs_index = 0; regs_index < array_count; regs_index += nof_commands) {
    nof_regs_in_cell = array_count - regs_index;
    if(nof_regs_in_cell > nof_commands) {
      nof_regs_in_cell = nof_commands;
    }
    for(i = 0; i< nof_regs_in_cell; ++i) {
#ifdef BCM_ARAD_SUPPORT
      if(SOC_IS_ARAD(unit)){
        sal_memcpy(&arad_temp_data_32[i*(128/32)], &data[regs_index+i], (128/8));
      }
#endif 
    }

#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit) && SOC_FABRIC_INBAND_IS_GROUP(route_id)){
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_group_is_valid(unit, SOC_FABRIC_INBAND_ROUTE_GRP_GET(route_id), &is_valid));
      if(!is_valid) {
        rv = SOC_E_PARAM;
        goto soc_dpp_attach_error;
      }
  
      soc_sand_rv = arad_transaction_with_fe1600(
        soc_sand_dev_id,
        &sr_link_list,
        SOC_FABRIC_INBAND_ROUTE_GRP_GET(route_id),
        TRUE,
        &reg[regs_index],
        NULL,
        &port[regs_index],
        &index[regs_index],
        arad_temp_data_32,
        nof_regs_in_cell,
        TRUE,
        TRUE,
        FALSE,
        NULL,
        NULL
      );
      rv = handle_sand_result(soc_sand_rv);
      if(SOC_FAILURE(rv)) {
        goto soc_dpp_attach_error;
      }
      continue;
    }
#endif 


    
    if(SOC_FABRIC_INBAND_IS_GROUP(route_id)){
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_group_is_valid(unit, SOC_FABRIC_INBAND_ROUTE_GRP_GET(route_id), &is_valid));
      if(!is_valid) {
        rv = SOC_E_PARAM;
        goto soc_dpp_attach_error;
      }

      rv = soc_dpp_fabric_inband_route_group_get(
        unit,
        SOC_FABRIC_INBAND_IS_GROUP(route_id),
        0,
        SOC_DPP_NUM_OF_ROUTES,
        &routes_num,
        routes_ids
      );
      if(SOC_FAILURE(rv)) {
        goto soc_dpp_attach_error;
      }
    }
    else
    {
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_is_valid(unit, route_id, &is_valid));
      if(!is_valid) {
        rv = SOC_E_PARAM;
        goto soc_dpp_attach_error;
      }

      routes_num = 1;
      routes_ids[0] = route_id;
    }

    for(route_index=0; route_index < routes_num; route_index++)
    {
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_is_valid(unit, route_id, &is_valid));
      if(!is_valid) {
        continue;
      }

      
      rv = soc_dpp_fabric_inband_route_get(
        unit,
        routes_ids[route_index],
        &route
      );
      if(SOC_FAILURE(rv)) {
        goto soc_dpp_attach_error;
      }

      rv = soc_dpp_fabric_inband_route2sr_link_list(
        unit,
        &route,
        &sr_link_list
      );
      if (SOC_FAILURE(rv)) 
      {
        goto soc_dpp_attach_error;
      }

         
      if (!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT))
      {      
#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
          soc_sand_rv = arad_transaction_with_fe1600(
            soc_sand_dev_id,
            &sr_link_list,
            route_id,
            FALSE,
            &reg[regs_index],
            NULL,
            &port[regs_index],
            &index[regs_index],
            arad_temp_data_32,
            nof_regs_in_cell,
            TRUE,
            TRUE,
            FALSE,
            NULL,
            NULL
          );
        }
#endif 

        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    
      } 
      else
      {
#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
          soc_sand_rv = arad_write_to_fe600_unsafe(
            soc_sand_dev_id,
            &sr_link_list,
            sizeof(uint32),
            reg[regs_index],
            arad_temp_data_32
          );
        }
#endif 

        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    }
  }
  if(routes_ids) {
    sal_free(routes_ids);
  }
  return SOC_E_NONE; 

soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_send_sr_cell: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
exit:
  if(routes_ids) {
    sal_free(routes_ids);
  }
  SOCDNX_FUNC_RETURN;
}

int 
  soc_dpp_fabric_mem_read(
    int unit, 
    uint32 flags, 
    int route_id, 
    int max_count,  
    soc_mem_t *mem, 
    int *copyno,
    int *index,
    void **entry_data, 
    int *array_count
  ) 
{
  int
    mems_index,
    nof_commands,
    nof_mems_in_cell,
    rv;
  SOC_TMC_SR_CELL_LINK_LIST 
    sr_link_list;  
  uint32 
    soc_sand_dev_id;
  uint32
    soc_sand_rv = SOC_E_UNAVAIL;
  soc_fabric_inband_route_t
    route;
  uint8
    is_valid;

#ifdef BCM_ARAD_SUPPORT
  int i;
  soc_reg_above_64_val_t
    temp_data[VSC256_MAX_COMMANDS_ARRAY_SIZE];
#endif 

  SOCDNX_INIT_FUNC_DEFS;

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(entry_data);
  SOCDNX_NULL_CHECK(mem);
  SOCDNX_NULL_CHECK(copyno);
  SOCDNX_NULL_CHECK(index);
  SOCDNX_NULL_CHECK(array_count);

  sal_memset(&sr_link_list, 0, sizeof(SOC_TMC_SR_CELL_LINK_LIST));
      
  soc_sand_dev_id = unit;
  
  if(SOC_FABRIC_INBAND_IS_GROUP(route_id)) {
    rv = SOC_E_PARAM;
    goto soc_dpp_attach_error;
  }

  
  SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_is_valid(unit, route_id, &is_valid));
  if(!is_valid) {
    rv = SOC_E_PARAM;
    goto soc_dpp_attach_error;
  }

  
#ifdef BCM_ARAD_SUPPORT
      if (SOC_IS_ARAD(unit)) {
        rv = soc_dpp_arad_fabric_inband_route_get(
          unit,
          route_id,
          &route
        );

        if(SOC_FAILURE(rv)) {
          goto soc_dpp_attach_error;
        }
      }
  else
#endif 
  {
        LOG_ERROR(BSL_LS_SOC_FABRIC,
                  (BSL_META_U(unit,
                              "soc_dpp_fabric_send_sr_cell: support only ARAD and PETRAB devices\n")));    
        return SOC_E_UNAVAIL;
  }

  rv = soc_dpp_fabric_inband_route2sr_link_list(
    unit,
    &route,
    &sr_link_list
  );
  if (SOC_FAILURE(rv)) 
  {
    goto soc_dpp_attach_error;
  }
  

  nof_commands = NOF_COMMANDS(unit, flags);


  for (mems_index=0; mems_index < max_count; mems_index += nof_mems_in_cell)
  {
    nof_mems_in_cell = max_count - mems_index;
    if(nof_mems_in_cell > nof_commands) {
      nof_mems_in_cell = nof_commands;
    }

    if (!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT))
    {      

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        soc_sand_rv = arad_transaction_with_fe1600(
          soc_sand_dev_id,
          &sr_link_list,
          route_id,
          FALSE,
          NULL,
          &mem[mems_index],
          &copyno[mems_index],
          &index[mems_index],
          NULL,
          nof_mems_in_cell,
          FALSE,
          TRUE,
          TRUE,
          temp_data,
          NULL
        );
        for(i=0; i< nof_mems_in_cell ; ++i)
        {
          sal_memcpy(entry_data[mems_index+i], temp_data[i], 128/8);
        }
      }
#endif 

      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
    } 
    else
    {
#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        soc_sand_rv = arad_indirect_read_from_fe600_unsafe(
          soc_sand_dev_id,
          &sr_link_list,
          nof_mems_in_cell,
          mem[mems_index],
          entry_data[mems_index]
        );
      }
#endif 

    SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
    }
  }
  
  *array_count = max_count;

  return SOC_E_NONE;

  soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_send_sr_cell: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
    return rv;
exit:
  SOCDNX_FUNC_RETURN;
}    

int
  soc_dpp_fabric_mem_write(
     int unit, 
     uint32 flags, 
     int route_id, 
     int array_count,
     soc_mem_t *mem, 
     int *copyno,
     int *index,
     void **entry_data 
  ) 
{
  int
  mems_index,
    route_index,
    routes_num,
    *routes_ids = NULL,
    nof_commands,
    nof_mems_in_cell,
    i,
    rv = SOC_E_NONE;
  SOC_TMC_SR_CELL_LINK_LIST 
    sr_link_list;  
  uint32 
    soc_sand_dev_id;
  uint32
#ifdef BCM_ARAD_SUPPORT
    arad_temp_data_32[VSC256_MAX_COMMANDS_ARRAY_SIZE*(128/32)],
#endif
    soc_sand_rv = SOC_E_UNAVAIL;
  soc_fabric_inband_route_t
    route;
  uint8
    is_valid;
  SOCDNX_INIT_FUNC_DEFS;

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(entry_data);
  SOCDNX_NULL_CHECK(mem);  
  SOCDNX_NULL_CHECK(copyno);  
  SOCDNX_NULL_CHECK(index);  

  sal_memset(&sr_link_list, 0, sizeof(SOC_TMC_SR_CELL_LINK_LIST));
  soc_sand_dev_id = unit;
  
  nof_commands = NOF_COMMANDS(unit, flags);

  routes_ids = sal_alloc(sizeof(int)*SOC_DPP_NUM_OF_ROUTES, "soc_dpp_fabric_mem_write.routes_ids");
  if(routes_ids == NULL) {
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_mem_write: unit %d failed - allocation failure\n"), unit));    
    goto soc_dpp_attach_error;
  }

  for(mems_index = 0; mems_index < array_count; mems_index += nof_commands) {
    nof_mems_in_cell = array_count - mems_index;
    if(nof_mems_in_cell > nof_commands) {
      nof_mems_in_cell = nof_commands;
    }

    for(i = 0; i< nof_mems_in_cell; ++i) {
#ifdef BCM_ARAD_SUPPORT
      if(SOC_IS_ARAD(unit)){
        sal_memcpy(&arad_temp_data_32[i*(128/32)], entry_data[mems_index+i], (128/8));
      }
#endif
    }

#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit) && SOC_FABRIC_INBAND_IS_GROUP(route_id)){
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_group_is_valid(unit, SOC_FABRIC_INBAND_ROUTE_GRP_GET(route_id), &is_valid));
      if(!is_valid) {
        rv = SOC_E_PARAM;
        goto soc_dpp_attach_error;
      }

      soc_sand_rv = arad_transaction_with_fe1600(
        soc_sand_dev_id,
        &sr_link_list,
        SOC_FABRIC_INBAND_ROUTE_GRP_GET(route_id),
        TRUE,
        &mem[mems_index],
        copyno,
        NULL,
        index,
        arad_temp_data_32,
        nof_mems_in_cell,
        TRUE,
        TRUE,
        TRUE,
        NULL,
        NULL
        );
      rv = handle_sand_result(soc_sand_rv);
      if(SOC_FAILURE(rv)) {
        goto soc_dpp_attach_error;
      }
      continue;
    }
#endif 

    
    if(SOC_FABRIC_INBAND_IS_GROUP(route_id)){
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_group_is_valid(unit, SOC_FABRIC_INBAND_ROUTE_GRP_GET(route_id), &is_valid));
      if(!is_valid) {
        rv = SOC_E_PARAM;
        goto soc_dpp_attach_error;
      }

      rv = soc_dpp_fabric_inband_route_group_get(
        unit,
        SOC_FABRIC_INBAND_IS_GROUP(route_id),
        0,
        SOC_DPP_NUM_OF_ROUTES,
        &routes_num,
        routes_ids
      );
      if(SOC_FAILURE(rv)) {
        goto soc_dpp_attach_error;
      }
    }
    else
    {
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_is_valid(unit, route_id, &is_valid));
      if(!is_valid) {
        rv = SOC_E_PARAM;
        goto soc_dpp_attach_error;
      }

      routes_num = 1;
      routes_ids[0] = route_id;
    }

    for(route_index=0; route_index < routes_num; route_index++)
    {
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_is_valid(unit, route_id, &is_valid));
      if(!is_valid) {
        continue;
      }

      rv = soc_dpp_fabric_inband_route_get(
        unit,
        routes_ids[route_index],
        &route
        );
      if(SOC_FAILURE(rv)) {
        goto soc_dpp_attach_error;
      }

      rv = soc_dpp_fabric_inband_route2sr_link_list(
        unit,
        &route,
        &sr_link_list
        );
      if (SOC_FAILURE(rv)) 
      {
        goto soc_dpp_attach_error;
      }

      if (!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT))
      {      
#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
          soc_sand_rv = arad_transaction_with_fe1600(
            soc_sand_dev_id,
            &sr_link_list,
            route_id,
            FALSE,
            NULL,
            &mem[mems_index],
            &copyno[mems_index],
            &index[mems_index],
            arad_temp_data_32,
            nof_mems_in_cell,
            TRUE,
            TRUE,
            TRUE,
            NULL,
            NULL
            );
        }
#endif 

        SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

      } 
      else
      {
#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
          soc_sand_rv = arad_write_to_fe600_unsafe(
            soc_sand_dev_id,
            &sr_link_list,
            sizeof(uint32),
            mem[mems_index],
            arad_temp_data_32
          );
        }
#endif 

        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    }
  }
  if(routes_ids) {
    sal_free(routes_ids);
  }
  return SOC_E_NONE; 

soc_dpp_attach_error:
    if(routes_ids) {
      sal_free(routes_ids);
    }
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_send_sr_cell: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
    return rv;

exit:
  if(routes_ids) {
    sal_free(routes_ids);
  }
  SOCDNX_FUNC_RETURN;
}

int 
  soc_dpp_fabric_reg_above_64_read(
    int unit, 
    uint32 flags, 
    int route_id,
    int max_count, 
    soc_reg_t *reg, 
    int *port,
    int *index,
    soc_reg_above_64_val_t *data, 
    int *array_count
  )
{
  int
  regs_index,
    rv;    
  SOC_TMC_SR_CELL_LINK_LIST 
    sr_link_list;  
  uint32 
    soc_sand_dev_id;
  uint32
    soc_sand_rv = SOC_E_UNAVAIL;
  soc_fabric_inband_route_t
    route;
  uint8
    is_valid,
    nof_commands,
    nof_regs_in_cell;
  SOCDNX_INIT_FUNC_DEFS;

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  sal_memset(&sr_link_list, 0, sizeof(SOC_TMC_SR_CELL_LINK_LIST));
  sal_memset(&route, 0, sizeof(soc_fabric_inband_route_t));

  SOCDNX_NULL_CHECK(reg);
  SOCDNX_NULL_CHECK(port);
  SOCDNX_NULL_CHECK(index);
  SOCDNX_NULL_CHECK(data);
  SOCDNX_NULL_CHECK(array_count);

  soc_sand_dev_id = unit;
  
  if(SOC_FABRIC_INBAND_IS_GROUP(route_id)) {
    rv = SOC_E_LIMIT;
    goto soc_dpp_attach_error;
  }
  
  
  SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_is_valid(unit, route_id, &is_valid));
  if(!is_valid) {
    rv = SOC_E_PARAM;
    goto soc_dpp_attach_error;
  }
#ifdef BCM_ARAD_SUPPORT
     if (SOC_IS_ARAD(unit)) {
      rv = soc_dpp_arad_fabric_inband_route_get(
        unit,
        route_id,
        &route
      );

      if (SOC_FAILURE(rv)) 
      {
        goto soc_dpp_attach_error;
      }
    }
    else
#endif 
  {
        LOG_ERROR(BSL_LS_SOC_FABRIC,
                  (BSL_META_U(unit,
                              "soc_dpp_fabric_send_sr_cell: support only ARAD and PETRAB devices\n")));    
        return SOC_E_UNAVAIL;
  }

  rv = soc_dpp_fabric_inband_route2sr_link_list(
    unit,
    &route,
    &sr_link_list
  );
  
  if (SOC_FAILURE(rv)) 
  {
    goto soc_dpp_attach_error;
  }
  
  nof_commands = NOF_COMMANDS(unit, flags);
  
  for (regs_index=0; regs_index < max_count; regs_index += nof_commands)
  {
    if (!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT))
    {
      nof_regs_in_cell = max_count - regs_index;
      if(nof_regs_in_cell > nof_commands) {
        nof_regs_in_cell = nof_commands;
      }

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        soc_sand_rv = arad_transaction_with_fe1600(
          soc_sand_dev_id,
          &sr_link_list,
          0,
          FALSE,
          &reg[regs_index],
          NULL,
          &port[regs_index],
          &index[regs_index],
          NULL,
          nof_regs_in_cell,
          FALSE,
          TRUE,
          FALSE,
          &data[regs_index],
          NULL
        );
      }
#endif 

      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);  
    } 
    else
    {

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
        soc_sand_rv = arad_read_from_fe600_unsafe(
          soc_sand_dev_id,
          &sr_link_list,
          sizeof(uint32),
          reg[regs_index],
          data[regs_index]
        );
      }
#endif 

      SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

    }
  }

  *array_count = max_count;

  return SOC_E_NONE; 

soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_send_sr_cell: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
  return rv;

exit:
  SOCDNX_FUNC_RETURN;
}

int 
  soc_dpp_fabric_reg_above_64_write(
   int unit, 
   uint32 flags, 
   int route_id,
   int array_count, 
   soc_reg_t *reg, 
   int *port,
   int *index,
   soc_reg_above_64_val_t *data
  )
{
  int
  regs_index,
    route_index,
    routes_num,
    *routes_ids = NULL,
    nof_commands,
    nof_regs_in_cell,
    i,
    rv = SOC_E_NONE;
  SOC_TMC_SR_CELL_LINK_LIST 
    sr_link_list;  
  uint32 
    soc_sand_dev_id;
  uint32
#ifdef BCM_ARAD_SUPPORT
    arad_temp_data_32[VSC256_MAX_COMMANDS_ARRAY_SIZE*(128/32)],
#endif
    soc_sand_rv = SOC_E_UNAVAIL;
  soc_fabric_inband_route_t
    route;
  uint8
    is_valid;
  SOCDNX_INIT_FUNC_DEFS;

  if(!SOC_UNIT_NUM_VALID(unit))
  {
    return SOC_E_UNIT;
  }

  SOCDNX_NULL_CHECK(reg);
  SOCDNX_NULL_CHECK(port);
  SOCDNX_NULL_CHECK(index);
  SOCDNX_NULL_CHECK(data);

  sal_memset(&sr_link_list, 0, sizeof(SOC_TMC_SR_CELL_LINK_LIST));

  soc_sand_dev_id = unit;

  nof_commands = NOF_COMMANDS(unit, flags);

  routes_ids = sal_alloc(sizeof(int)*SOC_DPP_NUM_OF_ROUTES, "soc_dpp_fabric_reg_above_64_write.routes_ids");
  if(routes_ids == NULL) {
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_reg_above_64_write: unit %d failed - allocation failure\n"), unit));    
    goto soc_dpp_attach_error;
  }

  for(regs_index = 0; regs_index < array_count; regs_index += nof_commands) {
    nof_regs_in_cell = array_count - regs_index;
    if(nof_regs_in_cell > nof_commands) {
      nof_regs_in_cell = nof_commands;
    }
    for(i=0; i< nof_regs_in_cell; ++i) {
#ifdef BCM_ARAD_SUPPORT
      if(SOC_IS_ARAD(unit)){
        sal_memcpy(&arad_temp_data_32[i*(128/32)], data[regs_index+i], 128/8);
      }
#endif
    }

#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit) && SOC_FABRIC_INBAND_IS_GROUP(route_id)){
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_group_is_valid(unit, SOC_FABRIC_INBAND_ROUTE_GRP_GET(route_id), &is_valid));
      if(!is_valid) {
        rv = SOC_E_PARAM;
        goto soc_dpp_attach_error;
      }

      soc_sand_rv = arad_transaction_with_fe1600(
        soc_sand_dev_id,
        &sr_link_list,
        SOC_FABRIC_INBAND_ROUTE_GRP_GET(route_id),
        TRUE,
        &reg[regs_index],
        NULL,
        &port[regs_index],
        &index[regs_index],
        arad_temp_data_32,
        nof_regs_in_cell,
        TRUE,
        TRUE,
        FALSE,
        NULL,
        NULL
      );
      rv = handle_sand_result(soc_sand_rv);
      if(SOC_FAILURE(rv)) {
        goto soc_dpp_attach_error;
      }
      continue;
    }
#endif 


    
    if(SOC_FABRIC_INBAND_IS_GROUP(route_id)){
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_group_is_valid(unit, SOC_FABRIC_INBAND_ROUTE_GRP_GET(route_id), &is_valid));
      if(!is_valid) {
        rv = SOC_E_PARAM;
        goto soc_dpp_attach_error;
      }

      rv = soc_dpp_fabric_inband_route_group_get(
        unit,
        SOC_FABRIC_INBAND_IS_GROUP(route_id),
        0,
        SOC_DPP_NUM_OF_ROUTES,
        &routes_num,
        routes_ids
      );
      if(SOC_FAILURE(rv)) {
        goto soc_dpp_attach_error;
      }
    }
    else
    {
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_is_valid(unit, route_id, &is_valid));
      if(!is_valid) {
        rv = SOC_E_PARAM;
        goto soc_dpp_attach_error;
      }

      routes_num = 1;
      routes_ids[0] = route_id;
    }

    for(route_index=0; route_index < routes_num; route_index++)
    {
      
      SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_inband_route_is_valid(unit, route_id, &is_valid));
      if(!is_valid) {
        continue;
      }

      rv = soc_dpp_fabric_inband_route_get(
        unit,
        routes_ids[route_index],
        &route
      );
      if(SOC_FAILURE(rv)) {
        goto soc_dpp_attach_error;
      }

      rv = soc_dpp_fabric_inband_route2sr_link_list(
        unit,
        &route,
        &sr_link_list
      );
      if (SOC_FAILURE(rv)) 
      {
        goto soc_dpp_attach_error;
      }
      
      if (!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT))
      {      

#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
          soc_sand_rv = arad_transaction_with_fe1600(
            soc_sand_dev_id,
            &sr_link_list,
            route_id,
            FALSE,
            &reg[regs_index],
            NULL,
            &port[regs_index],
            &index[regs_index],
            arad_temp_data_32,
            nof_regs_in_cell,
            TRUE,
            TRUE,
            FALSE,
            NULL,
            NULL
          );
        }
#endif 

        SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
    
      } 
      else
      {
#ifdef BCM_ARAD_SUPPORT
   if (SOC_IS_ARAD(unit)) {
          soc_sand_rv = arad_write_to_fe600_unsafe(
            soc_sand_dev_id,
            &sr_link_list,
            sizeof(uint32),
            reg[regs_index],
            arad_temp_data_32
          );
        }
#endif 

        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    }
  }
  if(routes_ids) {
    sal_free(routes_ids);
  }
  return SOC_E_NONE; 

  soc_dpp_attach_error:
    if(routes_ids) {
      sal_free(routes_ids);
    }
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_send_sr_cell: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
    return rv;

exit:
  if(routes_ids) {
    sal_free(routes_ids);
  }
  SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dpp_fabric_reachability_status_get(int unit, int moduleid, int links_max, uint32 *links_array, int *links_count)
{
    SOCDNX_INIT_FUNC_DEFS;
    
#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        _rv = arad_fabric_reachability_status_get(unit, moduleid, links_max, links_array, links_count);
    } else 
#endif 
    {
        _rv = SOC_E_UNAVAIL;
    }

    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_dpp_fabric_link_status_all_get(int unit, int links_array_max_size, uint32* link_status, uint32* errored_token_count, int* links_array_count)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        _rv = arad_fabric_link_status_all_get(unit, links_array_max_size, link_status, errored_token_count, links_array_count);
    } else 
#endif 
    {
        _rv = SOC_E_UNAVAIL;
    }

    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dpp_fabric_link_status_get(int unit, bcm_port_t link_id, uint32 *link_status, uint32 *errored_token_count)
{
    SOCDNX_INIT_FUNC_DEFS;
    
#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        _rv = arad_fabric_link_status_get(unit, link_id, link_status, errored_token_count);
    } else 
#endif 
    {
        _rv = SOC_E_UNAVAIL;
    }

    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dpp_fabric_pipes_config(int unit)
{    
    int nof_pipes;
    int i, j, rv;
    int is_valid = 1;
    soc_dcmn_fabric_pipe_map_t fabric_pipe_map_valid_config[SOC_DCMN_FABRIC_PIPE_NUM_OF_FABRIC_PIPE_VALID_CONFIGURATIONS];

    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes = SOC_DPP_FABRIC_PIPES_CONFIG(unit).nof_pipes; 

    rv = soc_dcmn_fabric_pipe_map_initalize_valid_configurations(unit, SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_tdm_priority_min, fabric_pipe_map_valid_config);
    SOCDNX_IF_ERR_EXIT(rv);
        
    if (nof_pipes == 1)
    {
        
        for (i = 0; i < SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES ; ++i)
        {
            SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[i] = 0;
            SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[i] = 0;
            SOC_DPP_FABRIC_PIPES_CONFIG(unit).mapping_type = soc_dcmn_fabric_pipe_map_single;
        }
        SOC_EXIT;
    }
    
    SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[0] = soc_property_suffix_num_get(unit, 0, spn_FABRIC_PIPE_MAP, "uc", SOC_DPP_PROPERTY_UNAVAIL);
    if (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[0] == SOC_DPP_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("unicast priority 0 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[1] = soc_property_suffix_num_get(unit, 1, spn_FABRIC_PIPE_MAP, "uc", SOC_DPP_PROPERTY_UNAVAIL);
    if (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[1] == SOC_DPP_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("unicast priority 1 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[2] = soc_property_suffix_num_get(unit, 2, spn_FABRIC_PIPE_MAP, "uc", SOC_DPP_PROPERTY_UNAVAIL);
    if (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[2] == SOC_DPP_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("unicast priority 2 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[3] = soc_property_suffix_num_get(unit, 3, spn_FABRIC_PIPE_MAP, "uc", SOC_DPP_PROPERTY_UNAVAIL); 
    if (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[3] == SOC_DPP_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("unicast priority 3 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[0] = soc_property_suffix_num_get(unit, 0, spn_FABRIC_PIPE_MAP, "mc", SOC_DPP_PROPERTY_UNAVAIL);
    if (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[0] == SOC_DPP_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("multicast priority 0 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[1] = soc_property_suffix_num_get(unit, 1, spn_FABRIC_PIPE_MAP, "mc", SOC_DPP_PROPERTY_UNAVAIL);
    if (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[1] == SOC_DPP_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("multicast priority 1 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[2] = soc_property_suffix_num_get(unit, 2, spn_FABRIC_PIPE_MAP, "mc", SOC_DPP_PROPERTY_UNAVAIL);
    if (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[2] == SOC_DPP_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("multicast priority 2 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[3] = soc_property_suffix_num_get(unit, 3, spn_FABRIC_PIPE_MAP, "mc", SOC_DPP_PROPERTY_UNAVAIL);
    if (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[3] == SOC_DPP_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("multicast priority 3 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    
    for (i = 0; i < SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES ; ++i)
    {
        if ( (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[i] >= nof_pipes) || (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[i] >= nof_pipes) ||
             (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[i] < 0) || (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[i] < 0)  )
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid pipe number - more than configured")));
        }
    }

    
    if (nof_pipes != 1)
    {
        for (i = 0; i < SOC_DCMN_FABRIC_PIPE_NUM_OF_FABRIC_PIPE_VALID_CONFIGURATIONS ; ++i)
        {
            if (nof_pipes == fabric_pipe_map_valid_config[i].nof_pipes)
            {
                is_valid = 1;
                for (j = 0; j < SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES; ++j)
                {
                    is_valid= (is_valid && (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_uc[j] == fabric_pipe_map_valid_config[i].config_uc[j]) &&
                                           (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[j] == fabric_pipe_map_valid_config[i].config_mc[j]));
                }
                if (is_valid)
                {
                    SOC_DPP_FABRIC_PIPES_CONFIG(unit).mapping_type = fabric_pipe_map_valid_config[i].mapping_type;
                    break;
                }
            }
        } 
    }

    if (!is_valid){ 

        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid configuration")));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

uint32
soc_dpp_fabric_topology_status_get(int unit, soc_dpp_fabric_topology_status_t *topo_stat) 
{ 
    SOCDNX_INIT_FUNC_DEFS;
 
    topo_stat->fabric_connect_mode = SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode;
    if (topo_stat->fabric_connect_mode != SOC_TMC_FABRIC_CONNECT_MODE_MESH)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid Configuration - topology status is only relevant for Mesh mode")));
    }
    topo_stat->is_mesh_mc = (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable == 1);
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_modid_group_get,
                                            (unit, 0, 2, topo_stat->fap_id_group0_array, &topo_stat->fap_id_group0_count)));
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_modid_group_get,
                                            (unit, 1, 2, topo_stat->fap_id_group1_array, &topo_stat->fap_id_group1_count)));
    if (!topo_stat->is_mesh_mc) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_modid_group_get, 
                                                (unit, 2, 2, topo_stat->fap_id_group2_array, &topo_stat->fap_id_group2_count)));
    }
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_topology_get,
                                            (unit, 0, 36, &topo_stat->links_group0_count, topo_stat->links_group0_array)));
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_topology_get,
                                            (unit, 1, 36, &topo_stat->links_group1_count, topo_stat->links_group1_array)));
    if (!topo_stat->is_mesh_mc) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_topology_get, 
                                                (unit, 2, 36, &topo_stat->links_group2_count, topo_stat->links_group2_array)));
    }
    
exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC uint32
  soc_dpp_fabric_cell_parse_table_get(
    int unit, 
    uint32 max_nof_lines, 
    soc_dcmn_fabric_cell_parse_table_t *parse_table, 
    uint32 *nof_lines
  )
{
    soc_dcmn_fabric_cell_parse_table_t temp_table[] = SOC_ARAD_FABRIC_CELL_CAPTURED_SRC_FORMAT;
    SOCDNX_INIT_FUNC_DEFS;

    if (max_nof_lines < SOC_ARAD_FABRIC_CELL_CAPTURED_SRC_FORMAT_NOF_LINES)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Bigger table is required\n")));
    } 
    
    sal_memcpy(parse_table, temp_table, sizeof(soc_dcmn_fabric_cell_parse_table_t) * SOC_ARAD_FABRIC_CELL_CAPTURED_SRC_FORMAT_NOF_LINES);

    *nof_lines = SOC_ARAD_FABRIC_CELL_CAPTURED_SRC_FORMAT_NOF_LINES;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_dpp_fabric_cell_parse(
    int unit, 
    soc_dcmn_fabric_cell_entry_t entry, 
    vsc256_sr_cell_t *cell
  ) 
{
    uint32 nof_lines;
    soc_dcmn_fabric_cell_info_t cell_info;
    soc_dcmn_fabric_cell_parse_table_t parse_table[SOC_DCMN_FABRIC_CELL_PARSE_TABLE_MAX_NOF_LINES];
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_cell_parse_table_get(unit, SOC_DCMN_FABRIC_CELL_PARSE_TABLE_MAX_NOF_LINES, parse_table, &nof_lines));

    SOCDNX_IF_ERR_EXIT(soc_dcmn_fabric_cell_parser(unit, entry, entry, parse_table, nof_lines, &cell_info, 0));

    *cell = cell_info.cell.sr_cell;

exit:
    SOCDNX_FUNC_RETURN;

}

uint32
soc_dpp_fabric_rx_fifo_diag_get(int unit, soc_dpp_fabric_rx_fifo_diag_t *rx_fifo_diag)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rx_fifo_status_get, (unit, rx_fifo_diag)));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
soc_dpp_fabric_force_set(int unit, soc_dpp_fabric_force_t force )
{
    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_DPP_IS_MESH(unit) || SOC_DPP_SINGLE_FAP(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("This feature is only supported in CLOS mode")));
    } else {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_force_set, (unit, force))); 
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
soc_dpp_fabric_sync_e_enable_get(int unit, int is_master, int *is_fabric_synce)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(is_fabric_synce);
    *is_fabric_synce = 0;
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOC_EXIT;
    }

    MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_sync_e_enable_get, (unit, is_master, is_fabric_synce));

exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

