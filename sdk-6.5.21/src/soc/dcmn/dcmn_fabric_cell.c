/*
 * 
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

#include <soc/dcmn/dcmn_fabric_cell.h>
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif 
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/drv.h>
#endif 
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/drv.h>
#endif 

int
soc_inband_route_set(
                     int unit,
                     int route_id,
                     soc_fabric_inband_route_t* route
                     )
{
  SOCDNX_INIT_FUNC_DEFS;

  if (!SOC_UNIT_VALID(unit)) {
	  SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
  }

#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    SOCDNX_IF_ERR_EXIT
		(soc_dpp_fabric_inband_route_set(unit, route_id, route));
  }
#endif
exit:
  SOCDNX_FUNC_RETURN;
}

int
soc_inband_route_get(
                     int unit,
                     int route_id,
                     soc_fabric_inband_route_t* route
                     )
{
  SOCDNX_INIT_FUNC_DEFS;

  if (!SOC_UNIT_VALID(unit)) {
	  SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
  }
#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    SOCDNX_IF_ERR_EXIT
		(soc_dpp_fabric_inband_route_get(unit, route_id, route));
  }
#endif

exit:
  SOCDNX_FUNC_RETURN;
}

int
soc_inband_route_group_set(
                           int unit,
                           int group_id,
                           int flags,
                           int route_count,
                           int *route_ids
                           )
{
#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    return soc_dpp_fabric_inband_route_group_set(unit, group_id, flags, route_count, route_ids);
  }
#endif
  return SOC_E_UNIT;
}

int
soc_inband_route_group_get(
                           int unit,
                           int group_id,
                           int flags,
                           int route_count_max,
                           int *route_count,
                           int *route_ids
                           )
{
#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    return soc_dpp_fabric_inband_route_group_get(unit, group_id, flags, route_count_max, route_count, route_ids);
  }
#endif
  return SOC_E_UNIT;
}

int 
soc_inband_mem_read(
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
#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    return soc_dpp_fabric_mem_read(unit, flags, route_id, max_count, mem, copyno, index, entry_data, array_count);
  }
#endif
  return SOC_E_UNIT;
}

int
soc_inband_mem_write(
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
#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    return soc_dpp_fabric_mem_write(unit, flags, route_id, array_count, mem, copyno, index, entry_data);
  }
#endif
  return SOC_E_UNIT;
} 

int 
soc_inband_reg_read(
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
#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    return soc_dpp_fabric_reg_read(unit, flags, route_id, max_count, reg, port, index, data, array_count);
  }
#endif
  return SOC_E_UNIT;
}

int 
soc_inband_reg_write(
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
#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    return soc_dpp_fabric_reg_write(unit, flags, route_id, array_count, reg, port, index, data);
  }
#endif
  return SOC_E_UNIT;
}

int
soc_inband_reg_above_64_read(
                                 int unit, uint32 flags,
                                 int route_id,
                                 int max_count,
                                 soc_reg_t *reg,
                                 int *port,
                                 int *index,
                                 soc_reg_above_64_val_t *data,
                                 int *array_count
)
{
#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    return soc_dpp_fabric_reg_above_64_read(unit, flags, route_id, max_count, reg, port, index, data, array_count);
  }
#endif
  return SOC_E_UNIT;
}

int
soc_inband_reg_above_64_write(
                                  int unit, uint32 flags,
                                  int route_id, int array_count,
                                  soc_reg_t *reg,
                                  int *port,
                                  int *index,
                                  soc_reg_above_64_val_t *data
)
{
#ifdef BCM_DPP_SUPPORT
  if(SOC_IS_DPP(unit))
  {
    return soc_dpp_fabric_reg_above_64_write(unit, flags, route_id, array_count, reg, port, index, data);
  }
#endif
  return SOC_E_UNIT;
}

soc_error_t 
soc_dcmn_parse_captured_cell(int unit, soc_reg_above_64_val_t reg_val, dcmn_captured_cell_t *captured_cell)
{
    uint32 val = 0;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(captured_cell, 0, sizeof(dcmn_captured_cell_t));
    
#define DCMN_PARSING_DATA_CELL_SOURCE_DEVICE_START 543
#define DCMN_PARSING_DATA_CELL_SOURCE_DEVICE_LENGTH 11
    SHR_BITCOPY_RANGE(&val, 0, reg_val, DCMN_PARSING_DATA_CELL_SOURCE_DEVICE_START, DCMN_PARSING_DATA_CELL_SOURCE_DEVICE_LENGTH);
    captured_cell->source_device = val;
#define DCMN_PARSING_DATA_CELL_DEST_START 554
#define DCMN_PARSING_DATA_CELL_DEST_LENGTH 16
    val = 0;
    SHR_BITCOPY_RANGE(&val, 0, reg_val, DCMN_PARSING_DATA_CELL_DEST_START, DCMN_PARSING_DATA_CELL_DEST_LENGTH);
    captured_cell->dest = val;
#define DCMN_PARSING_DATA_CELL_FIRST_CELL_IND_START 536
#define DCMN_PARSING_DATA_CELL_FIRST_CELL_IND_LENGTH 1
    val = 0;
    SHR_BITCOPY_RANGE(&val, 0, reg_val, DCMN_PARSING_DATA_CELL_FIRST_CELL_IND_START, DCMN_PARSING_DATA_CELL_FIRST_CELL_IND_LENGTH);
    captured_cell->is_last_cell = val;

    SOCDNX_FUNC_RETURN; 
}

STATIC soc_error_t 
soc_dcmn_fabric_cell_table_dest_fill(int unit, soc_dcmn_fabric_cell_parse_table_t *parse_table, uint32 nof_lines, soc_dcmn_fabric_cell_info_t *cell_info)
{
    int line;
    SOCDNX_INIT_FUNC_DEFS;

    for (line = 0; line < nof_lines; line++)
    {
        switch (parse_table[line].dest_id)
        {
            case soc_dcmn_fabric_cell_dest_id_src_cell_type:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.cell_type;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_src_device:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.source_device;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_src_level:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.source_level;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_dest_level:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.destination_level;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_fip:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fip_link;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_fe1:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fe1_link;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_fe2:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fe2_link;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_fe3:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.fe3_link;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_is_inband:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.is_inband;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_ack:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.ack;
                break;
           case soc_dcmn_fabric_cell_dest_id_src_pipe_id:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.header.pipe_id;
                break;
            case soc_dcmn_fabric_cell_dest_id_src_payload:
                parse_table[line].dest = (uint32 *) &cell_info->cell.sr_cell.payload.data;
                break;
            case soc_dcmn_fabric_cell_dest_id_cap_cell_type:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.cell_type;
                break;
            case soc_dcmn_fabric_cell_dest_id_cap_src_device:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.source_device;
                break;
            case soc_dcmn_fabric_cell_dest_id_cap_cell_dst:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.dest;
                break;
            case soc_dcmn_fabric_cell_dest_id_cap_cell_ind:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.is_last_cell;
                break;
            case soc_dcmn_fabric_cell_dest_id_cap_cell_size:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.cell_size;
                break;
            case soc_dcmn_fabric_cell_dest_id_cap_cell_pipe_index:
                parse_table[line].dest = (uint32 *) &cell_info->cell.captured_cell.pipe_index;
                break;
            case soc_dcmn_fabric_cell_dest_id_cap_cell_payload:
                parse_table[line].dest = (uint32 *) cell_info->cell.captured_cell.data; 
                break;
            case soc_dcmn_fabric_cell_dest_id_cap_cell_payload_2:
                parse_table[line].dest = (uint32 *) cell_info->cell.captured_cell.data;
                break;

            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG_STR("UNAVAIL line id")));
                break;
        }
        
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_fabric_cell_parser(int unit, soc_dcmn_fabric_cell_entry_t entry, soc_dcmn_fabric_cell_entry_t entry_2 , soc_dcmn_fabric_cell_parse_table_t *parse_table, uint32 nof_lines, soc_dcmn_fabric_cell_info_t *cell_info, int is_two_parts)
{
    int line,
        nof_uint32,
        i,
        rv;
    soc_dcmn_fabric_cell_entry_t val;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_dcmn_fabric_cell_table_dest_fill(unit, parse_table, nof_lines, cell_info);
    SOCDNX_IF_ERR_EXIT(rv);
    
    sal_memset(val, 0x00, sizeof(soc_dcmn_fabric_cell_entry_t));

    LOG_DEBUG(BSL_LS_SOC_PKT,
              (BSL_META_U(unit,
                          "Received Cell Parse\n")));
    LOG_DEBUG(BSL_LS_SOC_PKT,
              (BSL_META_U(unit,
                          "-------------------\n")));
    LOG_DEBUG(BSL_LS_SOC_PKT,
              (BSL_META_U(unit,
                          "Entry print: ")));
    for (i = 0; i < SOC_DCMN_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32; i++)
    {
        LOG_DEBUG(BSL_LS_SOC_PKT,
                  (BSL_META_U(unit,
                              "0x%08x, "), entry[i]));
    }
    LOG_DEBUG(BSL_LS_SOC_PKT,
              (BSL_META_U(unit,
                          "\n")));

    for (line = 0; line < nof_lines; line++)
    {
        
        if ((parse_table[line].dest_id == soc_dcmn_fabric_cell_dest_id_cap_cell_payload_2) && is_two_parts)
        {
            SHR_BITCOPY_RANGE(val, 0, entry_2, parse_table[line].src_start_bit, parse_table[line].length);
        }
        else 
        {
            
            SHR_BITCOPY_RANGE(val, 0, entry, parse_table[line].src_start_bit, parse_table[line].length); 
        }

        
        nof_uint32 = (parse_table[line].length + 31) / 32; 
        if(nof_uint32 == 1)
        {
          LOG_DEBUG(BSL_LS_SOC_PKT,
                    (BSL_META_U(unit,
                                "%s: 0x%x"), parse_table[line].field_name, *val));
        } else 
        {
          LOG_DEBUG(BSL_LS_SOC_PKT,
                    (BSL_META_U(unit,
                                "%s: 0x%08x"), parse_table[line].field_name, *val));
          for (i = 1; i < nof_uint32; i++)
          {
              LOG_DEBUG(BSL_LS_SOC_PKT,
                        (BSL_META_U(unit,
                                    ", 0x%08x"), val[i]));
          }
        }
        LOG_DEBUG(BSL_LS_SOC_PKT,
                  (BSL_META_U(unit,
                              "\n")));

        
        SHR_BITCOPY_RANGE(parse_table[line].dest, parse_table[line].dest_start_bit, val, 0, parse_table[line].length);  

        SHR_BITCLR_RANGE(val, 0, parse_table[line].length);
    }
    LOG_DEBUG(BSL_LS_SOC_PKT,
              (BSL_META_U(unit,
                          "-------------------\n\n")));

exit:
    SOCDNX_FUNC_RETURN; 
}

#undef _ERR_MSG_MODULE_NAME

