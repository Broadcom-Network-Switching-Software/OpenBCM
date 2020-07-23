/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME

  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/arad_cell.h>
#include <soc/mcm/memregs.h>

#include <soc/dpp/fabric.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>

#if defined(BCM_88650_A0)



#define ARAD_CELL_PATH_LINK_FIP_SWITCH_POSITION      (0)
#define ARAD_CELL_PATH_LINK_FE1_SWITCH_POSITION      (1)
#define ARAD_CELL_PATH_LINK_FE2_SWITCH_POSITION      (2)
#define ARAD_CELL_PATH_LINK_FE3_SWITCH_POSITION      (3)
#define ARAD_CELL_MAX_NOF_TRIES_WAITING_FOR_ACK      (1000)
#define ARAD_CELL_ADDRESS_POSITION_0                 (0)
#define ARAD_CELL_ADDRESS_POSITION_1                 (1)
#define ARAD_CELL_ADDRESS_POSITION_2                 (2)
#define ARAD_CELL_ADDRESS_POSITION_3                 (3)
#define ARAD_CELL_ADDRESS_POSITION_4                 (4)
#define ARAD_CELL_WRITE_POSITION_0                   (0)
#define ARAD_CELL_WRITE_POSITION_1                   (1)
#define ARAD_CELL_WRITE_POSITION_2                   (2)
#define ARAD_CELL_WRITE_POSITION_4                   (4)
#define ARAD_FE600_RTP_INDIRECT_RW_ADDR              (0x0441 * 4)
#define ARAD_FE600_RTP_INDIRECT_WRITE_DATA0          (0x0420 * 4)
#define ARAD_FE600_RTP_INDIRECT_WRITE_DATA1          (0x0421 * 4)
#define ARAD_FE600_RTP_INDIRECT_WRITE_DATA2          (0x0422 * 4)
#define ARAD_FE600_RTP_INDIRECT_RW_TRIGGER           (0x0440 * 4)
#define ARAD_FE600_RTP_INDIRECT_READ_DATA0           (0x0430 * 4)
#define ARAD_FE600_RTP_INDIRECT_READ_DATA1           (0x0431 * 4)
#define ARAD_FE600_RTP_INDIRECT_READ_DATA2           (0x0432 * 4)
#define ARAD_CELL_NOF_CELL_IDENTS                    (0x1ff)

#define SOC_SAND_FAILURE(_sand_ret) \
    ((handle_sand_result(_sand_ret)) < 0)



int
soc_dpp_arad_fabric_inband_is_valid(
  int unit, 
  int id,
  uint32 *is_valid
)
{
  uint32
    reg;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(READ_FDT_IN_BAND_MEMm(unit, MEM_BLOCK_ANY, id, &reg));
  *is_valid = soc_FDT_IN_BAND_MEMm_field32_get(unit, &reg, VALIDf);

exit:
  SOCDNX_FUNC_RETURN;
}


int
soc_dpp_arad_fabric_inband_route_group_set(
  int unit,
  int group_id,
  int flags,
  int route_count, 
  int *route_ids
)
{
  uint32
    group_id0 = group_id,
    route_index;
  uint32
    reg_32;
  uint32 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  for (route_index = 0; route_index <route_count; route_index++){

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_FDT_IN_BAND_MEMm(unit, MEM_BLOCK_ANY, route_ids[route_index], &reg_32));
    soc_FDT_IN_BAND_MEMm_field_set(unit, &reg_32, DESTINATION_GROUPf, &group_id0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_FDT_IN_BAND_MEMm(unit, MEM_BLOCK_ANY, route_ids[route_index], &reg_32));
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_dpp_arad_fabric_inband_route_group_set()",0,0);
}


STATIC
int soc_fabric_inband_route_t_2reg_32(int unit, const soc_fabric_inband_route_t* route, uint32 *reg){
  int rv;
  uint32 fields;
  SOC_TMC_SR_CELL_LINK_LIST sr_link_list;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(reg);
  *reg = 0;
  fields = 0;

  soc_FDT_IN_BAND_MEMm_field32_set(unit, reg, DESTINATION_GROUPf, fields);
  rv = soc_dpp_fabric_inband_route2sr_link_list(unit, route, &sr_link_list);
  if(rv) {
    return rv;
  }

  switch(sr_link_list.dest_entity_type)
  {
  case SOC_SAND_FE1_ENTITY: 
  case SOC_SAND_FE3_ENTITY: 
      fields = 0; 
      break;
  case SOC_SAND_FE2_ENTITY: 
      fields = 1; 
      break;
  default: 
      return SOC_E_PARAM;
  }

  
  soc_FDT_IN_BAND_MEMm_field32_set(0, reg, DESTINATION_LEVELf, fields);
  if(route->number_of_hops >= 1) {
    fields = sr_link_list.path_links[0];
    soc_FDT_IN_BAND_MEMm_field32_set(0, reg, FIP_SWITCHf, fields);
  }
  if(route->number_of_hops >= 2) {
    fields = sr_link_list.path_links[1];
    soc_FDT_IN_BAND_MEMm_field32_set(0, reg, FE_1_SWITCHf, fields);
  }
  if(route->number_of_hops >= 3) {
    fields = sr_link_list.path_links[2];
    soc_FDT_IN_BAND_MEMm_field32_set(0, reg, FE_2_SWITCHf, fields);
  }

  if(route->number_of_hops == 0) {
    fields = FALSE;
  }
  else
  {
    fields = TRUE;
  }

  soc_FDT_IN_BAND_MEMm_field32_set(0, reg, VALIDf, fields);

exit:
  SOCDNX_FUNC_RETURN;
}


int
soc_dpp_arad_fabric_inband_route_set(
  int unit, 
  int route_id, 
  soc_fabric_inband_route_t *route
)
{
  int 
    rv;  
  uint32
    reg;

  rv = soc_fabric_inband_route_t_2reg_32(unit, route, &reg);
  if(SOC_FAILURE(rv)) {
    goto soc_dpp_attach_error;
  }
  rv = WRITE_FDT_IN_BAND_MEMm(unit, MEM_BLOCK_ANY, route_id, &reg);
  if(SOC_FAILURE(rv)) {
    goto soc_dpp_attach_error;
  }
  
  return SOC_E_NONE;  

soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_inband_route_set: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
    return rv;
}


int
soc_dpp_arad_fabric_inband_route_group_get( 
  int unit, 
  int group_id, 
  int flags, 
  int route_count_max, 
  int *route_count, 
  int *route_ids 
) 
{
  uint32
    index,
    reg,
    cur_route_group,
    is_valid;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  *route_count = 0;
  for (index = 0; index < SOC_DPP_ARAD_NUM_OF_ENTRIES; ++index)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_FDT_IN_BAND_MEMm(unit, MEM_BLOCK_ANY, index, &reg));
    soc_FDT_IN_BAND_MEMm_field_get(unit, &reg, DESTINATION_GROUPf, &cur_route_group);
    soc_FDT_IN_BAND_MEMm_field_get(unit, &reg, VALIDf, &is_valid);
    if((cur_route_group == group_id) && is_valid) {
      route_ids[(*route_count)++] = index;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_dpp_arad_fabric_inband_route_group_get()", 0, 0);
}


STATIC int
sr_link_list2soc_dpp_fabric_route(
  int unit,
  SOC_TMC_SR_CELL_LINK_LIST sr_link_list,
  soc_fabric_inband_route_t *route
)
{
  int 
    hop_index;
  uint32 
    soc_sand_dev_id;
  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR 
    connectivity_map;
  uint32
    res = 0;
  SOCDNX_INIT_FUNC_DEFS;
  
  sal_memset(route, 0, sizeof(soc_fabric_inband_route_t));

  soc_sand_dev_id = (unit);
  
  SOCDNX_NULL_CHECK(route);

  res = arad_fabric_topology_status_connectivity_get_unsafe(
    0,
    SOC_DPP_DEFS_GET(unit, nof_fabric_links) - 1,
    soc_sand_dev_id,
    &connectivity_map
  );
  if(SOC_SAND_FAILURE(res)) {
    return SOC_E_INTERNAL;
  }
  
  if ( sr_link_list.dest_entity_type == SOC_SAND_FIP_ENTITY)
  {
    route->number_of_hops = 0;
  }
  else if (sr_link_list.dest_entity_type == SOC_SAND_FE1_ENTITY)
  {
    if (connectivity_map.link_info[sr_link_list.path_links[0]].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE1)
    {
      route->number_of_hops = 1;
    }
    else
    {
      return SOC_E_LIMIT;
    }
  }
  else if(sr_link_list.dest_entity_type == SOC_SAND_FE2_ENTITY)
  {
     if (connectivity_map.link_info[sr_link_list.path_links[0]].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE2)
     {
       route->number_of_hops = 1;
     }
     else if(connectivity_map.link_info[sr_link_list.path_links[0]].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE1)
     {
       route->number_of_hops = 2;
     }
     else
     {
       return SOC_E_LIMIT;
     }
  }
  else if(sr_link_list.dest_entity_type == SOC_SAND_FE3_ENTITY)
  {
    route->number_of_hops = 3;
  }
  else if (sr_link_list.dest_entity_type == SOC_SAND_FOP_ENTITY)
  {
    if (connectivity_map.link_info[sr_link_list.path_links[0]].far_dev_type == SOC_TMC_FAR_DEVICE_TYPE_FE2)
    {
      route->number_of_hops = 2;
    }
    else
    {
      route->number_of_hops = 4;
    }
  }
  
  for (hop_index=0; hop_index<route->number_of_hops; hop_index++)
  {
    route->link_ids[hop_index] = sr_link_list.path_links[hop_index];
  }

exit:
  SOCDNX_FUNC_RETURN;
}


STATIC int
reg_32_2soc_fabric_inband_route_t(
  int unit,
  const uint32 reg,
  soc_fabric_inband_route_t *route
)
{
  int rv;
  uint32 fields;
  SOC_TMC_SR_CELL_LINK_LIST sr_link_list;
  SOCDNX_INIT_FUNC_DEFS;
  
  SOCDNX_NULL_CHECK(route);
  fields = 0;
  sal_memset(&sr_link_list, 0, sizeof(sr_link_list));
  soc_FDT_IN_BAND_MEMm_field_get(0, &reg, VALIDf, &fields);
  if(!fields) {
    sr_link_list.dest_entity_type = SOC_SAND_FIP_ENTITY;
  }
  soc_FDT_IN_BAND_MEMm_field_get(0, &reg, DESTINATION_LEVELf, &fields);
  sr_link_list.dest_entity_type = fields == 1 ? SOC_SAND_FE2_ENTITY : SOC_SAND_FE3_ENTITY;
  soc_FDT_IN_BAND_MEMm_field_get(0, &reg, FIP_SWITCHf, &fields);
  sr_link_list.path_links[0] = fields;
  soc_FDT_IN_BAND_MEMm_field_get(0, &reg, FE_1_SWITCHf, &fields);
  sr_link_list.path_links[1] = fields;
  soc_FDT_IN_BAND_MEMm_field_get(0, &reg, FE_2_SWITCHf, &fields);
  sr_link_list.path_links[2] = fields;
  rv = sr_link_list2soc_dpp_fabric_route(unit, sr_link_list, route);
  if(rv){
    return rv;
  }

exit:
  SOCDNX_FUNC_RETURN;
}


int
soc_dpp_arad_fabric_inband_route_get(
  int unit, 
  int route_id, 
  soc_fabric_inband_route_t *route
)
{
  int 
    rv;  
  uint32
    reg;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_FDT_IN_BAND_MEMm(unit, MEM_BLOCK_ANY, route_id, &reg));
  rv = reg_32_2soc_fabric_inband_route_t(unit, reg, route);
  if(SOC_FAILURE(rv)) {
    goto soc_dpp_attach_error;
  }

  if(route->number_of_hops == 0)
  {
    rv = SOC_E_PARAM;
    goto soc_dpp_attach_error;
  }
  return SOC_E_NONE;  

soc_dpp_attach_error:
    LOG_ERROR(BSL_LS_SOC_FABRIC,
              (BSL_META_U(unit,
                          "soc_dpp_fabric_inband_route_set: unit %d failed (%s)\n"), 
                          unit, soc_errmsg(rv)));    
    return rv;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_dpp_arad_fabric_inband_route_get()", 0, 0);
}




 soc_error_t
soc_arad_parse_cell(int unit, uint8 is_inband, uint32* buf, vsc256_sr_cell_t* cell) 
{
    int rc;
    soc_reg_above_64_val_t header_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(header_data);
    if(is_inband)
    {
        SHR_BITCOPY_RANGE(header_data, 0, buf, ARAD_SR_DATA_CELL_PAYLOAD_LENGTH, 8 * VSC256_SR_DATA_CELL_HEADER_SIZE);
    }
    else
    {
        
        SHR_BITCOPY_RANGE(header_data, ARAD_RECEIVED_DATA_CELL_HEADER_OFFSET_VSC256, buf,
            ARAD_RECEIVED_DATA_CELL_HEADER_START, 8 * VSC256_SR_DATA_CELL_HEADER_SIZE - ARAD_RECEIVED_DATA_CELL_HEADER_OFFSET_VSC256);

        
        SHR_BITCOPY_RANGE(header_data, VSC256_SR_DATA_CELL_CELL_TYPE_START, buf, ARAD_RECEIVED_DATA_CELL_TYPE_START, VSC256_SR_DATA_CELL_CELL_TYPE_LENGTH);
    }

    rc = soc_vsc256_parse_header(unit, header_data, cell);
    SOCDNX_IF_ERR_EXIT(rc);

    if(!cell->header.is_inband) {
        SHR_BITCOPY_RANGE(cell->payload.data, 0, buf, 0, ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS) ;
    } else {
        rc = soc_vsc256_parse_payload(unit, buf, cell, ARAD_INBAND_PAYLOAD_CELL_OFFSET);
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


STATIC soc_error_t 
soc_arad_sr_cell_format(int unit, const vsc256_sr_cell_t* cell, int buf_size_bytes, uint32* buf) 
{
    soc_error_t rc;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(buf, 0, buf_size_bytes);

    if(buf_size_bytes < ARAD_DATA_CELL_BYTE_SIZE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SR cell minimum buffer size is %d"),ARAD_DATA_CELL_BYTE_SIZE));
    }

    if(cell->header.is_inband && VSC256_CELL_FORMAT_FE600 == cell->payload.inband.cell_format) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("VSC256 can't be sent in SOC_SAND_FE600 format")));
    }

    
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.is_fe600) {
        
        rc = soc_vsc256_to_vsc128_build_header(unit, cell, buf_size_bytes - (ARAD_SR_DATA_CELL_PAYLOAD_LENGTH/8), buf + (ARAD_SR_DATA_CELL_PAYLOAD_LENGTH/(8*sizeof(uint32))));
        SOCDNX_IF_ERR_EXIT(rc);
    } else {
        
        rc = soc_vsc256_build_header(unit, cell, buf_size_bytes - (ARAD_SR_DATA_CELL_PAYLOAD_LENGTH/8), buf + ((ARAD_SR_DATA_CELL_PAYLOAD_LENGTH + SOC_DPP_DEFS_GET(unit, source_routed_cells_header_offset))/(8*sizeof(uint32))));
        SOCDNX_IF_ERR_EXIT(rc);
    }

    
    if(!cell->header.is_inband) {
        SHR_BITCOPY_RANGE(buf, ARAD_SR_DATA_CELL_PAYLOAD_LENGTH - ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS,
            cell->payload.data, 0, ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS);
    }else {
        rc = soc_vsc256_build_payload(unit, cell , buf_size_bytes, buf, ARAD_INBAND_PAYLOAD_CELL_OFFSET);
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    SOCDNX_FUNC_RETURN;  
}

uint32
  arad_sr_data_cell_to_fe1600_buffer(
    SOC_SAND_IN  int32          unit,
    SOC_SAND_IN  vsc256_sr_cell_t          *data_cell,
    SOC_SAND_OUT uint32         *packed_data_cell
  )
{
    return soc_arad_sr_cell_format(unit, data_cell, ARAD_DATA_CELL_BYTE_SIZE, packed_data_cell);
}

STATIC uint32
device_entity_value(
    SOC_SAND_IN     SOC_SAND_DEVICE_ENTITY            device_entity,
    SOC_SAND_OUT    dcmn_fabric_device_type_t*     dcmn_device_entity
  )
{
  switch(device_entity)
  {
    case SOC_SAND_DONT_CARE_ENTITY:
      *dcmn_device_entity = dcmnFabricDeviceTypeUnknown;
      return SOC_SAND_OK;
    case SOC_SAND_FE1_ENTITY:
      *dcmn_device_entity = dcmnFabricDeviceTypeFE1;
      return SOC_SAND_OK;
    case SOC_SAND_FE2_ENTITY:
      *dcmn_device_entity = dcmnFabricDeviceTypeFE2;
      return SOC_SAND_OK;
    case SOC_SAND_FE3_ENTITY:
      *dcmn_device_entity = dcmnFabricDeviceTypeFE3;
      return SOC_SAND_OK;
    case SOC_SAND_FAP_ENTITY:
      *dcmn_device_entity = dcmnFabricDeviceTypeFAP;
      return SOC_SAND_OK;
    case SOC_SAND_FOP_ENTITY:
      *dcmn_device_entity = dcmnFabricDeviceTypeFOP;
      return SOC_SAND_OK;
    case SOC_SAND_FIP_ENTITY:
      *dcmn_device_entity = dcmnFabricDeviceTypeFIP;
      return SOC_SAND_OK;
    case SOC_SAND_FE13_ENTITY:
      *dcmn_device_entity = dcmnFabricDeviceTypeFE13;
      return SOC_SAND_OK;
    default:
    {
      return SOC_SAND_ERR;
    }
  }
}



STATIC uint32
arad_build_data_cell_for_fe1600(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST          *sr_link_list,
    SOC_SAND_IN  soc_reg_t                        *regs,
    SOC_SAND_IN  soc_mem_t                        *mems,
    SOC_SAND_IN  int32                         *port_or_copyno,
    SOC_SAND_IN  int32                         *index,
    SOC_SAND_IN  uint32                         *data_in,
    SOC_SAND_IN  uint32                         nof_words,
    SOC_SAND_IN  uint32                        is_write,
    SOC_SAND_IN  uint32                        is_inband,
    SOC_SAND_IN  uint8                        is_memory,
    SOC_SAND_OUT vsc256_sr_cell_t                 *data_cell_sent
  )
{
  uint8
    i,
    at;
  uint16
    cell_id;
  int32
    block,
    index_valid;    
  uint32
    maddr,
    blkoff;
  soc_mem_info_t
    *meminfo;
  dcmn_fabric_device_type_t
    dcmn_device_entity;
  SOCDNX_INIT_FUNC_DEFS;

  
  data_cell_sent->header.cell_type = 1 ;
  data_cell_sent->header.source_device = (uint32) unit ;
  
  data_cell_sent->header.source_level = dcmnFabricDeviceTypeFIP ;
  if(device_entity_value(sr_link_list->dest_entity_type, &dcmn_device_entity))
  {
    return SOC_SAND_ERR;
  }
  data_cell_sent->header.destination_level = dcmn_device_entity;
  
  data_cell_sent->header.fip_link =
    sr_link_list->path_links[ARAD_CELL_PATH_LINK_FIP_SWITCH_POSITION] ;
  
  data_cell_sent->header.fe1_link =
    sr_link_list->path_links[ARAD_CELL_PATH_LINK_FE1_SWITCH_POSITION] ;
  
  data_cell_sent->header.fe2_link =
    sr_link_list->path_links[ARAD_CELL_PATH_LINK_FE2_SWITCH_POSITION] ;
  
  data_cell_sent->header.fe3_link =
    sr_link_list->path_links[ARAD_CELL_PATH_LINK_FE3_SWITCH_POSITION] ;
  

  data_cell_sent->header.ack = 0 ;

  data_cell_sent->payload.inband.cell_format = VSC256_CELL_FORMAT_FE1600;
  SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.cell.current_cell_ident.get(unit, &cell_id));
  ++cell_id;
  SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.cell.current_cell_ident.set(unit, cell_id));
  data_cell_sent->payload.inband.cell_id = cell_id;
  data_cell_sent->payload.inband.seq_num = 0;
  data_cell_sent->payload.inband.nof_commands = nof_words;

  if (is_inband)
  {
    if(nof_words > VSC256_MAX_COMMANDS_ARRAY_SIZE) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Number of command above the maximum")));
    }
    data_cell_sent->header.is_inband = TRUE;
    
    if (!is_memory) 
    {
      for(i = 0; i < nof_words; ++i) {
        if (!SOC_REG_IS_VALID(unit, regs[i])) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid register %s"), SOC_REG_NAME(unit, regs[i])));
        }

        if(is_write)
        {
          data_cell_sent->payload.inband.commands[i].opcode = soc_dcmn_inband_reg_write;
        }
        else
        {
          data_cell_sent->payload.inband.commands[i].opcode = soc_dcmn_inband_reg_read;
        }

        maddr = soc_reg_addr_get(unit, regs[i], port_or_copyno[i], index[i],
                                 SOC_REG_ADDR_OPTION_WRITE, &block, &at);
        data_cell_sent->payload.inband.commands[i].schan_block = ((maddr >> SOC_BLOCK_BP) & 0xf) | (((maddr >> SOC_BLOCK_MSB_BP) & 0x1) << 4);
        if(SOC_REG_IS_ABOVE_64(unit, regs[i])) {
          data_cell_sent->payload.inband.commands[i].length = SOC_REG_ABOVE_64_INFO(unit, regs[i]).size;
        }
        else if(SOC_REG_IS_64(unit, regs[i])) {
            data_cell_sent->payload.inband.commands[i].length = 8;
        } else {
            data_cell_sent->payload.inband.commands[i].length = 4;
        }
        
        if(data_cell_sent->payload.inband.commands[i].length > 16) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("can't read more than 128 bits data")));
        }

        
        blkoff = ((data_cell_sent->payload.inband.commands[i].schan_block & 0xf) << SOC_BLOCK_BP) | (((data_cell_sent->payload.inband.commands[i].schan_block >> 4) & 0x3) << SOC_BLOCK_MSB_BP);
        data_cell_sent->payload.inband.commands[i].offset = maddr - blkoff;

        if(is_write) {
          sal_memcpy(
            data_cell_sent->payload.inband.commands[i].data,
            &data_in[i*VSC256_COMMAND_DATA_SIZE_U32],
            data_cell_sent->payload.inband.commands[i].length);
        }
      }
    }
    else 
    {
      for(i = 0; i < nof_words; ++i) {
        if (!soc_mem_is_valid(unit, mems[i])) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid memory %s"), SOC_MEM_NAME(unit, mems[i])));
        }
        meminfo = &SOC_MEM_INFO(unit, mems[i]);

        if (port_or_copyno[i] == MEM_BLOCK_ANY) {
          block = SOC_MEM_BLOCK_ANY(unit, mems[i]);
        } else {
          block = port_or_copyno[i];
        }

        if (!SOC_MEM_BLOCK_VALID(unit, mems[i], block)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid block %d for memory %s"), block, SOC_MEM_NAME(unit, mems[i])));
        }
    
        index_valid = (index[i] >= 0 && index[i] <= soc_mem_index_max(unit, mems[i]));
        if (!index_valid) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid index %d for memory %s"), index[i], SOC_MEM_NAME(unit, mems[i])));
        }
        if (is_write)
        {
          data_cell_sent->payload.inband.commands[i].opcode = soc_dcmn_inband_mem_write;
        }
        else
        {
          data_cell_sent->payload.inband.commands[i].opcode = soc_dcmn_inband_mem_read;
        }
        maddr = soc_mem_addr_get(unit, mems[i], 0, block, index[i], &at);
        data_cell_sent->payload.inband.commands[i].schan_block = ((maddr >> SOC_BLOCK_BP) & 0xf) | (((maddr >> SOC_BLOCK_MSB_BP) & 0x1) << 4);
        data_cell_sent->payload.inband.commands[i].length = meminfo->bytes;

        
        blkoff = ((data_cell_sent->payload.inband.commands[i].schan_block & 0xf) << SOC_BLOCK_BP) | (((data_cell_sent->payload.inband.commands[i].schan_block >> 4) & 0x3) << SOC_BLOCK_MSB_BP);
        data_cell_sent->payload.inband.commands[i].offset = maddr - blkoff;

        if(is_write) {
          if(meminfo->bytes > MAX_DATA_SIZE*sizeof(uint32)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("can't read more than 128 byte data")));
          }
          sal_memcpy(
            data_cell_sent->payload.inband.commands[i].data,
            &data_in[i*VSC256_COMMAND_DATA_SIZE_U32],
            meminfo->bytes
          );
        }
      }
    }
  }
  else 
  {
    if(nof_words > ARAD_SR_DATA_CELL_PAYLOAD_LENGTH/8) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Cell\'s size above the maximum")));
    }
    data_cell_sent->header.is_inband = FALSE ;
    soc_sand_os_memcpy(&(data_cell_sent->payload.data), data_in, nof_words);        
  }

  return SOC_SAND_OK;

exit:
  SOCDNX_FUNC_RETURN; 
}


STATIC uint32
arad_build_data_cell_for_fe600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in,
    SOC_SAND_IN  uint32                        nof_words,
    SOC_SAND_IN  uint8                       is_write,
    SOC_SAND_IN  uint8                       is_indirect,
    SOC_SAND_IN  uint8                       is_inband,
    SOC_SAND_OUT SOC_SAND_DATA_CELL                  *data_cell_sent
  )
{
  uint32
    res = SOC_SAND_OK ,
    tmp_field_for_data_cell_add_fields = 0,
    trigger = 0x1 ,
    iter_nof_words = 0 ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_BUILD_DATA_CELL_FOR_FE600);

  
  data_cell_sent->cell_type = 1 ;
  data_cell_sent->source_id = (uint16) unit ;
  
  data_cell_sent->data_cell.source_routed.src_level = 1 ;
  data_cell_sent->data_cell.source_routed.dest_level =
    (uint8) soc_sand_actual_entity_value( sr_link_list->dest_entity_type ) ;
  

  data_cell_sent->data_cell.source_routed.fip_switch =
    sr_link_list->path_links[ARAD_CELL_PATH_LINK_FIP_SWITCH_POSITION] ;
  
  data_cell_sent->data_cell.source_routed.fe1_switch =
    sr_link_list->path_links[ARAD_CELL_PATH_LINK_FE1_SWITCH_POSITION] ;
  
  data_cell_sent->data_cell.source_routed.fe2_switch =
    sr_link_list->path_links[ARAD_CELL_PATH_LINK_FE2_SWITCH_POSITION] ;
  
  data_cell_sent->data_cell.source_routed.fe3_switch =
    sr_link_list->path_links[ARAD_CELL_PATH_LINK_FE3_SWITCH_POSITION] ;
  

  if (is_inband)
  {
    data_cell_sent->data_cell.source_routed.indirect = is_indirect ;
    data_cell_sent->data_cell.source_routed.read_or_write = is_write ;

    data_cell_sent->data_cell.source_routed.inband_cell = 1 ;
    data_cell_sent->data_cell.source_routed.ack = 0 ;

    if (!is_indirect) 
    {
      if (!is_write) 
      {
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_0] = (uint16) offset ;
        
      }
      else 
      {
        for (iter_nof_words = 0 ; iter_nof_words < nof_words ; ++iter_nof_words)
        {
          data_cell_sent->data_cell.source_routed.add_wr_cell[iter_nof_words] = (uint16) offset ;
          
          data_cell_sent->data_cell.source_routed.data_wr_cell[iter_nof_words] = data_in[ iter_nof_words ] ;
        }
      }
    }
    else 
    {
      if (!is_write) 
      {
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_0] =
          (uint16) ARAD_FE600_RTP_INDIRECT_RW_ADDR ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_1] =
          (uint16) ARAD_FE600_RTP_INDIRECT_RW_TRIGGER ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_2] =
          (uint16) ARAD_FE600_RTP_INDIRECT_READ_DATA0 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_3] =
          (uint16) ARAD_FE600_RTP_INDIRECT_READ_DATA1 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_4] =
          (uint16) ARAD_FE600_RTP_INDIRECT_READ_DATA2 ;

        data_cell_sent->data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_0] = SOC_SAND_BIT(31)|offset;
        
        data_cell_sent->data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_1] = trigger ;
        
        for (iter_nof_words = 0 ; iter_nof_words < ARAD_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL ; ++iter_nof_words)
        {
          data_cell_sent->data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_2 + iter_nof_words] = 0x0 ;
        }
        
      }
      else 
      {
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_0] =
          (uint16) ARAD_FE600_RTP_INDIRECT_RW_ADDR ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_1] =
          (uint16) ARAD_FE600_RTP_INDIRECT_WRITE_DATA0 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_2] =
          (uint16) ARAD_FE600_RTP_INDIRECT_WRITE_DATA1 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_3] =
          (uint16) ARAD_FE600_RTP_INDIRECT_WRITE_DATA2 ;
        data_cell_sent->data_cell.source_routed.add_wr_cell[ARAD_CELL_ADDRESS_POSITION_4] =
          (uint16) ARAD_FE600_RTP_INDIRECT_RW_TRIGGER ;

        data_cell_sent->data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_0] = offset;
        
        for (iter_nof_words = 0 ; iter_nof_words < nof_words ; ++iter_nof_words)
        {
          data_cell_sent->data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_1 + iter_nof_words] =
            data_in[ iter_nof_words ] ;
        }
        data_cell_sent->data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_4] = trigger ;
        
      }
    }

    res = sw_state_access[unit].dpp.soc.arad.tm.cell.current_cell_ident.get(
            unit,
            &(data_cell_sent->data_cell.source_routed.cell_ident)
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.cell.current_cell_ident.set(
            unit,
            ( (data_cell_sent->data_cell.source_routed.cell_ident + 1) % ARAD_CELL_NOF_CELL_IDENTS )
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 9, exit);

    data_cell_sent->data_cell.source_routed.cell_format = 0 ;
  }
  else 
  {
    data_cell_sent->data_cell.source_routed.indirect = 0 ;
    data_cell_sent->data_cell.source_routed.read_or_write = 0 ;

    data_cell_sent->data_cell.source_routed.inband_cell = 0 ;
    data_cell_sent->data_cell.source_routed.ack = 0 ;

    for (iter_nof_words = 0 ; iter_nof_words < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter_nof_words)
    {
      data_cell_sent->data_cell.source_routed.data_wr_cell[iter_nof_words] = data_in[ iter_nof_words ] ;
    }

    for (iter_nof_words = 0 ; iter_nof_words < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter_nof_words)
    {
      tmp_field_for_data_cell_add_fields = 0 ;

      res = soc_sand_bitstream_get_any_field(
              &(data_in[SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD]),
              iter_nof_words * SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
              &(tmp_field_for_data_cell_add_fields)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      data_cell_sent->data_cell.source_routed.add_wr_cell[iter_nof_words] = (uint16) tmp_field_for_data_cell_add_fields;
      
    }

    tmp_field_for_data_cell_add_fields = 0 ;
    res = soc_sand_bitstream_get_any_field(
            &(data_in[SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD]),
            SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD * SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
            SOC_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_LENGTH,
            &(tmp_field_for_data_cell_add_fields)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    data_cell_sent->data_cell.source_routed.cell_ident = (uint16) tmp_field_for_data_cell_add_fields;
    

    data_cell_sent->data_cell.source_routed.cell_format = 0 ;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_build_data_cell_for_fe600()",0,0);
}



uint32
  arad_sr_send_cell(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                is_fe1600,
    SOC_SAND_IN  uint32                route_group_id,
    SOC_SAND_IN  uint8                 is_group,
    SOC_SAND_IN  SOC_SAND_DATA_CELL    *data_cell_sent_old_format,
    SOC_SAND_IN  vsc256_sr_cell_t      *data_cell_sent
  )
{
  uint32
    tmp_u32_fip_switch = 0 ,
    tmp_u32_output_link_in_five_bits = 0 ,
    tmp_reg,
    tmp_interrupt_reg,
    res = SOC_SAND_OK;
  uint64
      tmp_reg64, field64;
  uint32
    packed_cpu_data_cell_sent[ARAD_DATA_CELL_U32_SIZE];
  uint8
    is_set;
  soc_timeout_t
    to;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SR_SEND_CELL);

  sal_memset(
          packed_cpu_data_cell_sent,
          0x0,
          ARAD_DATA_CELL_BYTE_SIZE
        );
  
  
  if (is_fe1600)
  {
    res = arad_sr_data_cell_to_fe1600_buffer(
          unit,
          data_cell_sent,
          packed_cpu_data_cell_sent
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } 
  else
  {
    res = soc_sand_data_cell_to_buffer(
          data_cell_sent_old_format,
          packed_cpu_data_cell_sent
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
 

  
   SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, WRITE_FDT_CPU_DATA_CELL_0r(unit, packed_cpu_data_cell_sent));
   SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_FDT_CPU_DATA_CELL_1r(unit, packed_cpu_data_cell_sent + 256/(8*sizeof(uint32))));
   SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, WRITE_FDT_CPU_DATA_CELL_2r(unit, packed_cpu_data_cell_sent + 2*256/(8*sizeof(uint32))));
   SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_FDT_CPU_DATA_CELL_3r(unit, packed_cpu_data_cell_sent + 3*256/(8*sizeof(uint32))));
 
  
  if(is_fe1600){
    tmp_u32_fip_switch = data_cell_sent->header.fip_link ;
  }
  else{
    tmp_u32_fip_switch = data_cell_sent_old_format->data_cell.source_routed.fip_switch ;
  }
  res = soc_sand_bitstream_set_any_field(
          &(tmp_u32_fip_switch),
          0 ,
          SOC_SAND_DATA_CELL_FIP_SWITCH_LENGTH+1,
          &tmp_u32_output_link_in_five_bits
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  if(is_fe1600) {
    if(!is_group){
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1040, exit, READ_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr(unit, &tmp_reg));
        soc_reg_field_set(unit, FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr, &tmp_reg, CPU_LINK_NUMf, tmp_u32_output_link_in_five_bits);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1050, exit, WRITE_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr(unit, tmp_reg));

        tmp_reg = 0;
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1060, exit, READ_FDT_TRANSMIT_DATA_CELL_TRIGGERr(unit, &tmp_reg));
        soc_reg_field_set(unit, FDT_TRANSMIT_DATA_CELL_TRIGGERr, &tmp_reg, CPU_TRGf, 0x1);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1070, exit, WRITE_FDT_TRANSMIT_DATA_CELL_TRIGGERr(unit, tmp_reg));
    }
    else{
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1080, exit, READ_FDT_IN_BAND_MANAGMENENTr(unit, &tmp_reg64));
      COMPILER_64_SET(field64,0,0x1);    
      soc_reg64_field_set(unit, FDT_IN_BAND_MANAGMENENTr, &tmp_reg64, IN_BAND_ACK_CLEARf, field64);
        
      res = arad_polling(
              unit,
              ARAD_TIMEOUT,
              ARAD_MIN_POLLS,
              FDT_IN_BAND_MANAGMENENTr,
              REG_PORT_ANY,
              0,
              IN_BAND_ACK_CLEARf,
              0
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            
      
      soc_timeout_init(&to, ARAD_TIMEOUT, ARAD_MIN_POLLS);
      for(;;) {
        COMPILER_64_SET(field64,0,data_cell_sent->payload.inband.cell_id);    
        soc_reg64_field_set(unit, FDT_IN_BAND_MANAGMENENTr, &tmp_reg64, IN_BAND_CELL_IDf, field64);
        COMPILER_64_SET(field64,0,route_group_id);    
        soc_reg64_field_set(unit, FDT_IN_BAND_MANAGMENENTr, &tmp_reg64, IN_BAND_DESTINATION_GROUPf, field64);
        COMPILER_64_SET(field64,0,ARAD_IN_BAND_TIMEOUT_ATTEMPTS_CFG);    
        soc_reg64_field_set(unit, FDT_IN_BAND_MANAGMENENTr, &tmp_reg64, IN_BAND_TIMEOUT_ATTEMPTS_CFGf, field64);
        COMPILER_64_SET(field64,0,0x1);    
        soc_reg64_field_set(unit, FDT_IN_BAND_MANAGMENENTr, &tmp_reg64, IN_BAND_TRIGGERf, field64);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, WRITE_FDT_IN_BAND_MANAGMENENTr(unit, tmp_reg64));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1110, exit, READ_FDT_INTERRUPT_REGISTERr_REG32(unit, &tmp_interrupt_reg));
        is_set = soc_reg_field_get(unit, FDT_INTERRUPT_REGISTERr, tmp_interrupt_reg, IN_BAND_LAST_READ_CNTf);
#ifdef PLISIM
        if (SAL_BOOT_PLISIM) {
            is_set = TRUE;
        }
#endif
        if(is_set) {
          break;
        }
        if (soc_timeout_check(&to)) {
          SOC_SAND_SET_ERROR_CODE(ARAD_DIAG_DRAM_ACCESS_TIMEOUT_ERR, 20, exit);
        }
      }
    }
  }
  else{
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1120, exit, READ_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr(unit, &tmp_reg));
    soc_reg_field_set(unit, FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr, &tmp_reg, CPU_LINK_NUMf, tmp_u32_output_link_in_five_bits);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, WRITE_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr(unit, tmp_reg));

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1140, exit, READ_FDT_TRANSMIT_DATA_CELL_TRIGGERr(unit, &tmp_reg));
    soc_reg_field_set(unit, FDT_TRANSMIT_DATA_CELL_TRIGGERr, &tmp_reg, CPU_TRGf, 0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1150, exit, WRITE_FDT_TRANSMIT_DATA_CELL_TRIGGERr(unit, tmp_reg));
    
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sr_send_cell()",0,0);
}

uint32
  arad_cell_ack_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint8                      is_inband,
    SOC_SAND_OUT uint32                     *ack,
    SOC_SAND_OUT uint8                      *success
  )
{
  uint32
    reg_val,
    success_a,
    success_b,
    success_c,
    success_d;
  soc_reg_above_64_val_t
    payload_msb,
    payload_lsb,
    header,
    data;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CELL_ACK_GET);

  SOC_SAND_CHECK_NULL_INPUT(ack);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if(is_inband)
  {
#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
        *success = TRUE;
        ARAD_DO_NOTHING_AND_EXIT;
    }
#endif
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1250, exit, READ_FDR_INBAND_HEADER_VALIDr(unit, &reg_val));
    *success = soc_reg_field_get(unit, FDR_INBAND_HEADER_VALIDr, reg_val, INBAND_HEADER_VALIDf);
    if(*success)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1260, exit, READ_FDR_INBAND_PAYLOAD_MSBr(unit, payload_msb));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1270, exit, READ_FDR_INBAND_PAYLOAD_LSBr(unit, payload_lsb));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1280, exit, READ_FDR_INBAND_HEADERr(unit, header));
      SHR_BITCOPY_RANGE(ack, 0, payload_lsb, 0, 512);
      SHR_BITCOPY_RANGE(ack, 512, payload_msb, 0, 512);
      SHR_BITCOPY_RANGE(ack, 1024, header, 0, VSC256_SR_DATA_CELL_HEADER_SIZE*8);
      ARAD_DO_NOTHING_AND_EXIT;
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 5, exit);
    }
  }
  

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1290, exit, READ_FDR_INTERRUPT_REGISTER_1r(unit, &reg_val));
  success_a = soc_reg_field_get(unit, FDR_INTERRUPT_REGISTER_1r, reg_val, PRM_CPUDATACELLFNE_A_0f);
  success_b = soc_reg_field_get(unit, FDR_INTERRUPT_REGISTER_1r, reg_val, PRM_CPUDATACELLFNE_A_1f);
  success_c = soc_reg_field_get(unit, FDR_INTERRUPT_REGISTER_1r, reg_val, PRM_CPUDATACELLFNE_B_0f);
  success_d = soc_reg_field_get(unit, FDR_INTERRUPT_REGISTER_1r, reg_val, PRM_CPUDATACELLFNE_B_1f);

#ifdef PLISIM
  
  if (SAL_BOOT_PLISIM) {
      *success = TRUE;
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1292, exit, READ_FDT_CPU_DATA_CELL_2r(unit, data));
      SHR_BITCOPY_RANGE(ack, 0, data, 0, 256);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1297, exit, READ_FDT_CPU_DATA_CELL_3r(unit, data));
      SHR_BITCOPY_RANGE(ack, 256, data, 0, 256 + VSC256_SR_DATA_CELL_HEADER_SIZE*8);
      ARAD_DO_NOTHING_AND_EXIT;
  }
#endif

  if ( (!success_a) && (!success_b) && (!success_c) && (!success_d) )
  {
    *success = FALSE;
    ARAD_DO_NOTHING_AND_EXIT;
  }

  if (success_a)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1300, exit, READ_FDR_CPU_DATA_CELL_A_PRIMARYr(unit, data));
    sal_memcpy(ack, data,ARAD_DATA_CELL_RECEIVED_BYTE_SIZE);
  } else if (success_b)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1310, exit, READ_FDR_CPU_DATA_CELL_B_PRIMARYr(unit, data));
    sal_memcpy(ack, data, ARAD_DATA_CELL_RECEIVED_BYTE_SIZE);
  } else if (success_c)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1320, exit, READ_FDR_CPU_DATA_CELL_C_PRIMARYr(unit, data));
    sal_memcpy(ack, data, ARAD_DATA_CELL_RECEIVED_BYTE_SIZE);
  } else if (success_d)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1330, exit, READ_FDR_CPU_DATA_CELL_D_PRIMARYr(unit, data));
    sal_memcpy(ack, data, ARAD_DATA_CELL_RECEIVED_BYTE_SIZE);
  }

  
  *success = TRUE;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_cell_ack_get()",0,0);
}


STATIC uint32
  arad_sr_rcv_cell(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                        is_fe1600,
    SOC_SAND_IN  uint8                        is_inband,
    SOC_SAND_OUT SOC_SAND_DATA_CELL           *data_cell_old_format,
    SOC_SAND_OUT vsc256_sr_cell_t             *data_cell,
    SOC_SAND_OUT uint32                       *size, 
    SOC_SAND_OUT uint8                        *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    packed_cpu_data_cell_rcv[ARAD_DATA_CELL_U32_SIZE] ;
  uint32
    temp_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SR_RCV_CELL);

  if(is_fe1600) {
    SOC_SAND_CHECK_NULL_INPUT(data_cell);
  }
  else{
    SOC_SAND_CHECK_NULL_INPUT(data_cell_old_format);
  }
  SOC_SAND_CHECK_NULL_INPUT(success);

  sal_memset(packed_cpu_data_cell_rcv, 0x0, ARAD_DATA_CELL_U32_SIZE * sizeof(uint32));
  
  
  res = arad_cell_ack_get(
          unit,
          is_inband,
          packed_cpu_data_cell_rcv,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if(!is_inband) {
    
    SOC_SAND_CHECK_NULL_INPUT(size);
    temp_size = 0;
    SHR_BITCOPY_RANGE(&temp_size, 0, packed_cpu_data_cell_rcv, ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS, ARAD_RECUEVED_DATA_CELL_CELL_SIZE_LENGTH_IN_BITS);
    ++temp_size;
#ifdef PLISIM
    
    if (SAL_BOOT_PLISIM) {
        temp_size = ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS/8;
    }
#endif
    if(temp_size > ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS/8) {
      
      temp_size = ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS/8;
    }
    *size = temp_size;
  }

  
  if (is_fe1600)
  {
    res = soc_arad_parse_cell(
          unit,
          is_inband,
          packed_cpu_data_cell_rcv,          
          data_cell
        );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  } 
  else
  {
    

    SHR_BITCOPY_RANGE(packed_cpu_data_cell_rcv, ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS, 
                      packed_cpu_data_cell_rcv, ARAD_RECEIVED_DATA_CELL_HEADER_START, ARAD_RECEIVED_DATA_CELL_HEADER_SIZE_IN_BITS_VSC128);

    SHR_BITCOPY_RANGE(packed_cpu_data_cell_rcv, 
                      ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS + ARAD_RECEIVED_DATA_CELL_HEADER_SIZE_IN_BITS_VSC128 - VSC256_SR_DATA_CELL_CELL_TYPE_LENGTH, 
                      packed_cpu_data_cell_rcv, 
                      ARAD_RECEIVED_DATA_CELL_HEADER_START,
                      VSC256_SR_DATA_CELL_CELL_TYPE_LENGTH);

     
    res = soc_sand_buffer_to_data_cell(
          packed_cpu_data_cell_rcv,
          0,
          data_cell_old_format
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } 

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_rcv_cell()",0,0);
}

uint32
  arad_transaction_with_fe1600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST        *sr_link_list,
    SOC_SAND_IN  uint32                       route_group_id,
    SOC_SAND_IN  uint8                        is_group,
    SOC_SAND_IN  soc_reg_t                    *regs,
    SOC_SAND_IN  soc_mem_t                    *mems,
    SOC_SAND_IN  int32                        *port_or_copyno,
    SOC_SAND_IN  int32                        *index,
    SOC_SAND_IN  uint32                       *data_in, 
    SOC_SAND_IN  uint32                       size,  
    SOC_SAND_IN  uint8                        is_write,
    SOC_SAND_IN  uint8                        is_inband,
    SOC_SAND_IN  uint8                        is_memory,
    SOC_SAND_OUT soc_reg_above_64_val_t       *data_out, 
    SOC_SAND_OUT uint32                       *data_out_size 
  )
{
  uint8
    success = FALSE;
  uint32
    i;
  uint32
    start_long = 0 ,
    nof_words = 0 ,    
    res = SOC_SAND_OK ;
  vsc256_sr_cell_t
    data_cell_sent,
    data_cell_rcv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  sal_memset(&data_cell_sent, 0x0, sizeof(vsc256_sr_cell_t));
  
  sal_memset(&data_cell_rcv, 0x0, sizeof(vsc256_sr_cell_t));
  nof_words = size;
  
  if (is_inband)
  {
    

    res = arad_build_data_cell_for_fe1600(
      unit,
      sr_link_list,
      regs,
      mems,
      port_or_copyno,
      index,
      &data_in[start_long],
      nof_words,
      is_write,
      is_inband,
      is_memory,
      &data_cell_sent
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = arad_sr_send_and_wait_ack(
            unit,
            &data_cell_sent,
            NULL,
            route_group_id,
            is_group,
            &data_cell_rcv,
            NULL
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    if (
        data_cell_sent.payload.inband.cell_id
        != data_cell_rcv.payload.inband.cell_id
       )
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_CELL_DIFFERENT_CELL_IDENT_ERR, 153, exit);
    }

  }
  else if (!is_write) 
  {
     res = arad_sr_rcv_cell(
            unit,
            TRUE,
            FALSE,
            NULL,
            &data_cell_rcv,
            data_out_size,
            &success            
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);

    if (success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_CELL_NO_RECEIVED_CELL_ERR, 155, exit);
    }


  }
  else 
  {
    
    res = arad_build_data_cell_for_fe1600(
            unit,
            sr_link_list,
            regs,
            mems,
            port_or_copyno,
            index,
            &data_in[start_long],
            nof_words,
            is_write,
            is_memory,
            is_inband,
            &data_cell_sent
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);

    res = arad_sr_send_cell(
            unit,
            TRUE,
            route_group_id,
            is_group,
            NULL,
            &data_cell_sent
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 158, exit);
  }

  
  if (is_inband)
  {
    if(!is_write) 
    {
      for (i=0; i < size; i++)
      {
        SOC_REG_ABOVE_64_CLEAR(data_out[i]);
        SHR_BITCOPY_RANGE(data_out[i], 0, data_cell_rcv.payload.inband.commands[i].data, 0, 128);
      }        
    }
  }
  else if (!is_write) 
  {
    for(i=0; i < (nof_words*8)/128; ++i) {
      SOC_REG_ABOVE_64_CLEAR(data_out[i]);
      SHR_BITCOPY_RANGE(data_out[i], 0, data_cell_rcv.payload.data, i*128, 128);
    }
  }    
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_transaction_with_fe1600()",0,0);
}




STATIC uint32
  arad_transaction_with_fe600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint8                       is_write,
    SOC_SAND_IN  uint8                       is_indirect,
    SOC_SAND_IN  uint8                       is_inband,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint8
    success = FALSE;
  uint32
    iter = 0 ,
    iter_size = 0 ;
  uint32
    start_long = 0 ,
    nof_words = 0 ,
    tmp_field_for_data_cell_add_fields = 0,
    tmp_data_out[ARAD_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL] = {0},
    tmp_not_inband_data_out[SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S] = {0},
    res = SOC_SAND_OK ;
  SOC_SAND_DATA_CELL
    data_cell_sent,
    data_cell_rcv ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TRANSACTION_WITH_FE600);

  
  while (iter_size < size)
  {
    sal_memset(&data_cell_sent, 0x0, sizeof(SOC_SAND_DATA_CELL));
    
    sal_memset(&data_cell_rcv, 0x0, sizeof(SOC_SAND_DATA_CELL));
    
    if (is_inband)
      {
      if( ( !is_write ) && ( !is_indirect ) ) 
      {
        nof_words = 1  ;
      }
      else if( ( is_write ) && ( !is_indirect ) ) 
      {
        nof_words = SOC_SAND_MIN(SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD , (size - iter_size) / ARAD_CELL_NOF_BYTES_IN_UINT32 );
      }
      else if( ( !is_write ) && ( is_indirect ) ) 
      {
        nof_words = ARAD_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL;
      }
      else 
      {
        nof_words = SOC_SAND_MIN(ARAD_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL , (size - iter_size) / ARAD_CELL_NOF_BYTES_IN_UINT32 );
      }
    }
    else 
    {
      nof_words = 1;
    }
    if (is_inband)
    {
      
      res = arad_build_data_cell_for_fe600(
              unit,
              sr_link_list,
              offset,
              data_in + start_long,
              nof_words,
              is_write,
              is_indirect,
              is_inband,
              &data_cell_sent
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      
      res = arad_sr_send_and_wait_ack(
              unit,
              NULL,
              &data_cell_sent,
              FALSE,
              0,
              NULL,
              &data_cell_rcv
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

      if (
          data_cell_sent.data_cell.source_routed.cell_ident
          != data_cell_rcv.data_cell.source_routed.cell_ident
         )
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_CELL_DIFFERENT_CELL_IDENT_ERR, 153, exit);
      }

    }
    else if (!is_write) 
    {
       res = arad_sr_rcv_cell(
              unit,
              FALSE,
              is_inband,
              &data_cell_rcv,
              NULL,
              NULL,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);

      if (success == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_CELL_NO_RECEIVED_CELL_ERR, 155, exit);
      }


    }
    else 
    {
      
      res = arad_build_data_cell_for_fe600(
              unit,
              sr_link_list,
              offset,
              data_in + start_long,
              nof_words,
              is_write,
              is_indirect,
              is_inband,
              &data_cell_sent
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);

      res = arad_sr_send_cell(
              unit,
              FALSE,
              0,
              FALSE,
              &data_cell_sent,
              NULL
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 158, exit);
    }

    
    if (is_inband)
    {
      if(!is_write)
      {
        if(is_indirect) 
        {
          if(start_long != 0)
          {
            SOC_SAND_SET_ERROR_CODE(ARAD_CELL_WRITE_OUT_OF_BOUNDARY,160,exit);
          }

          tmp_data_out[start_long] =
            data_cell_rcv.data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_0] ;
          tmp_data_out[start_long + 1] =
            data_cell_rcv.data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_1] ;
          tmp_data_out[start_long + 2] =
            data_cell_rcv.data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_2] ;
        }
        else 
        {
          if(start_long >= ARAD_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL)
          {
            SOC_SAND_SET_ERROR_CODE(ARAD_CELL_WRITE_OUT_OF_BOUNDARY,162,exit);
          }

          tmp_data_out[start_long] = data_cell_rcv.data_cell.source_routed.data_wr_cell[ARAD_CELL_WRITE_POSITION_0] ;
        }
      }
    }
    else if (!is_write) 
    {
      if (start_long > SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S - SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD) {
         SOC_SAND_SET_ERROR_CODE(ARAD_CELL_WRITE_OUT_OF_BOUNDARY,162,exit);
      }
      for (iter = 0 ; iter < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
      {        
        tmp_not_inband_data_out[start_long + iter] = data_cell_rcv.data_cell.source_routed.data_wr_cell[iter] ;
      }

      for (iter = 0 ; iter < SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ; ++iter)
      {
        tmp_field_for_data_cell_add_fields = data_cell_rcv.data_cell.source_routed.add_wr_cell[iter];

        res = soc_sand_bitstream_set_any_field(
                &(tmp_field_for_data_cell_add_fields),
                SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH * iter,
                SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH,
                &(tmp_not_inband_data_out[start_long + SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD])
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
      }

      tmp_field_for_data_cell_add_fields = data_cell_rcv.data_cell.source_routed.cell_ident;

      res = soc_sand_bitstream_set_any_field(
              &(tmp_field_for_data_cell_add_fields),
              SOC_SAND_SR_DATA_CELL_ADDRESS_LENGTH * SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD,
              SOC_SAND_SR_DATA_CELL_NOT_INBAND_CELL_IDENT_LENGTH,
              &(tmp_not_inband_data_out[start_long + SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

    }

    if (is_inband)
    {
      if( ( !is_write ) && ( !is_indirect ) ) 
      {
        ++start_long  ;
        iter_size = iter_size + ARAD_CELL_NOF_BYTES_IN_UINT32 ;
      }
      else if( ( is_write ) && ( !is_indirect ) ) 
      {
        start_long = start_long + SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ;
        iter_size = iter_size + ARAD_CELL_NOF_BYTES_IN_UINT32 * SOC_SAND_SR_DATA_CELL_NOF_WORDS_IN_PAYLOAD ;
      }
      else 
      {
        start_long = start_long + ARAD_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL ;
        iter_size = iter_size + ARAD_CELL_NOF_BYTES_IN_UINT32 * ARAD_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL ;
      }
    }
    else
    {
      start_long += SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S;
      iter_size = iter_size + ARAD_CELL_NOF_BYTES_IN_UINT32 * SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S ;
    }
  }
  
  if(
     (!is_write)
     && (is_inband)
    )
  {
    for (iter = 0 ; iter < start_long ; ++iter)
    {
      data_out[iter] = tmp_data_out[iter];
    }
  }
  else if(
          (!is_inband)
          && (!is_write)
         )
  {
    for (iter = 0 ; iter < start_long ; ++iter)
    {
      data_out[iter] = tmp_not_inband_data_out[iter];
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_transaction_with_fe600()",0,0);
}


uint32
  arad_read_from_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_READ_FROM_FE600_UNSAFE);

  
  res = arad_transaction_with_fe600(
          unit,
          sr_link_list,
          offset,
          NULL,
          size,
          0,
          0,
          1,
          data_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_read_from_fe600_unsafe()",0,0);
}

uint32
  arad_write_to_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST        *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_WRITE_FROM_FE600_UNSAFE);

  
  
  res = arad_transaction_with_fe600(
          unit,
          sr_link_list,
          offset,
          data_in,
          size,
          1,
          0,
          1,
          NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_write_to_fe600_unsafe()",0,0);
}



uint32
  arad_indirect_read_from_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST          *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INDIRECT_READ_FROM_FE600_UNSAFE);

  
  res = arad_transaction_with_fe600(
          unit,
          sr_link_list,
          offset,
          NULL,
          size ,
          0,
          1,
          1,
          data_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_indirect_read_from_fe600_unsafe()",0,0);

}



uint32
  arad_indirect_write_to_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in
  )
{
  uint32
    res = SOC_SAND_OK ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INDIRECT_WRITE_FROM_FE600_UNSAFE);

  
  res = arad_transaction_with_fe600(
          unit,
          sr_link_list,
          offset,
          data_in,
          size ,
          1,
          1,
          1,
          NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_indirect_write_fe600_unsafe()",0,0);

}


uint32
  arad_cpu2cpu_write_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST          *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        *data_in
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CPU2CPU_WITH_FE600_WRITE_UNSAFE);

  
  res = arad_transaction_with_fe600(
          unit,
          sr_link_list,
          0,
          data_in,
          size,
          1,
          0,
          0,
          NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cpu2cpu_write_unsafe()",0,0);

}



uint32
  arad_cpu2cpu_read_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CPU2CPU_WITH_FE600_READ_UNSAFE);

  
  res = arad_transaction_with_fe600(
          unit,
          NULL,
          0,
          NULL,
          SOC_SAND_DATA_CELL_PAYLOAD_IN_BYTES,
          0,
          0,
          0,
          data_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cpu2cpu_read_unsafe()",0,0);

}



uint32
  arad_sr_send_and_wait_ack(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  vsc256_sr_cell_t              *data_cell_sent,
    SOC_SAND_IN  SOC_SAND_DATA_CELL            *data_cell_sent_old,
    SOC_SAND_IN  uint32                        route_group_id,
    SOC_SAND_IN  uint8                         is_group,
    SOC_SAND_OUT vsc256_sr_cell_t              *data_cell_rcv,
    SOC_SAND_OUT SOC_SAND_DATA_CELL            *data_cell_rcv_old
  )
{
  uint8
    success = FALSE;
  uint32
    try_i = 0 ;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SR_SEND_AND_WAIT_ACK);

  if(data_cell_sent) {
    SOC_SAND_CHECK_NULL_INPUT(data_cell_rcv);
    res = arad_sr_send_cell(
            unit,
            TRUE,
            route_group_id,
            is_group,
            NULL,
            data_cell_sent
          );
  }
  else{
    SOC_SAND_CHECK_NULL_INPUT(data_cell_sent_old);
    SOC_SAND_CHECK_NULL_INPUT(data_cell_rcv_old);
    res = arad_sr_send_cell(
            unit,
            FALSE,
            0,
            FALSE,
            data_cell_sent_old,
            NULL
          );
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  do
  {
    if(data_cell_sent) {
      res = arad_sr_rcv_cell(
              unit,
              TRUE,
              FALSE,
              NULL,
              data_cell_rcv,
              NULL,
              &success
            );
    }
    else{
      res = arad_sr_rcv_cell(
              unit,
              FALSE,
              FALSE,
              data_cell_rcv_old,
              NULL,
              NULL,
              &success
            );
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  } while((success == FALSE) && (try_i++ < ARAD_CELL_MAX_NOF_TRIES_WAITING_FOR_ACK));

  if (try_i >= ARAD_CELL_MAX_NOF_TRIES_WAITING_FOR_ACK)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_CELL_NO_RECEIVED_CELL_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_send_and_wait_ack()",0,0);
}


uint32
  arad_fabric_cell_cpu_data_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT uint32                  *cell_buffer
  )
{
    uint8 success;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_cell_ack_get(unit, FALSE, cell_buffer, &success));

    if (success == FALSE){
        SOCDNX_EXIT_WITH_ERR(SOC_E_EMPTY, (_BSL_SOCDNX_MSG("No cell was recieved\n")));
        
    }
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_sr_cell_send(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  vsc256_sr_cell_t   *data_cell_sent
  )
{
    uint32 packed_cpu_data_cell_sent[ARAD_DATA_CELL_U32_SIZE];
    uint32 tmp_u32_fip_switch = 0, tmp_u32_output_link_in_five_bits = 0, tmp_reg;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(packed_cpu_data_cell_sent, 0x0, ARAD_DATA_CELL_BYTE_SIZE);
  
    
    SOCDNX_IF_ERR_EXIT(arad_sr_data_cell_to_fe1600_buffer(unit, data_cell_sent, packed_cpu_data_cell_sent));
 
    
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_CPU_DATA_CELL_0r(unit, packed_cpu_data_cell_sent));
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_CPU_DATA_CELL_1r(unit, packed_cpu_data_cell_sent + 256 / (8 * sizeof(uint32))));
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_CPU_DATA_CELL_2r(unit, packed_cpu_data_cell_sent + 2 * 256 / (8 * sizeof(uint32))));
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_CPU_DATA_CELL_3r(unit, packed_cpu_data_cell_sent + 3 * 256 / (8 * sizeof(uint32))));
 
    
    tmp_u32_fip_switch = data_cell_sent->header.fip_link;

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_bitstream_set_any_field(&(tmp_u32_fip_switch), 0, SOC_SAND_DATA_CELL_FIP_SWITCH_LENGTH+1, &tmp_u32_output_link_in_five_bits));

    SOCDNX_IF_ERR_EXIT(READ_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr(unit, &tmp_reg));
    soc_reg_field_set(unit, FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr, &tmp_reg, CPU_LINK_NUMf, tmp_u32_output_link_in_five_bits);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBERr(unit, tmp_reg));

    tmp_reg = 0;
    SOCDNX_IF_ERR_EXIT(READ_FDT_TRANSMIT_DATA_CELL_TRIGGERr(unit, &tmp_reg));
    soc_reg_field_set(unit, FDT_TRANSMIT_DATA_CELL_TRIGGERr, &tmp_reg, CPU_TRGf, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_TRANSMIT_DATA_CELL_TRIGGERr(unit, tmp_reg));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_fabric_cpu2cpu_write(    
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN dcmn_sr_cell_link_list_t                *sr_link_list,
    SOC_SAND_IN uint32                                  data_in_size,
    SOC_SAND_IN uint32                                  *data_in
  )
{
    vsc256_sr_cell_t cell;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(&cell.payload.data[0], 0x0, sizeof(uint32)*VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32);

    cell.header.cell_type = soc_dcmn_fabric_cell_type_sr_cell;
    cell.header.source_device = (uint32) unit;
    cell.header.source_level = dcmnFabricDeviceTypeFIP;
    cell.header.destination_level = sr_link_list->dest_entity_type;
    cell.header.fip_link = sr_link_list->path_links[ARAD_CELL_PATH_LINK_FIP_SWITCH_POSITION];
    cell.header.fe1_link = sr_link_list->path_links[ARAD_CELL_PATH_LINK_FE1_SWITCH_POSITION];
    cell.header.fe2_link = sr_link_list->path_links[ARAD_CELL_PATH_LINK_FE2_SWITCH_POSITION];
    cell.header.fe3_link = sr_link_list->path_links[ARAD_CELL_PATH_LINK_FE3_SWITCH_POSITION];
    cell.header.ack = 0;
    cell.header.is_inband = 0;
    cell.header.pipe_id = sr_link_list->pipe_id; 

    sal_memcpy(cell.payload.data, data_in, 
               data_in_size < VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32 ? WORDS2BYTES(data_in_size) : WORDS2BYTES(VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32));

    SOCDNX_IF_ERR_EXIT(arad_sr_cell_send(unit, &cell));

exit:
      SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

#endif 

