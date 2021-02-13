/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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

#if defined(BCM_88660_A0)


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

    
    {
        
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
  
    
    SOCDNX_IF_ERR_EXIT(soc_arad_sr_cell_format(unit, data_cell_sent, ARAD_DATA_CELL_BYTE_SIZE, packed_cpu_data_cell_sent));
 
    
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

