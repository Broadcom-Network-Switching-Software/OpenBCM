/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_fabric.h>
#include <soc/dpp/TMC/tmc_api_framework.h>


























void
  SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO));
  info->far_unit = SOC_TMC_DEVICE_ID_IRRELEVANT;
  info->far_dev_type = SOC_TMC_FAR_NOF_DEVICE_TYPES;
  info->far_link_id = 0;
  info->is_connected_expected = 0;
  info->is_phy_connect = 0;
  info->is_logically_connected = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR));
  for (ind=0; ind<SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS); ++ind)
  {
    SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_clear(&(info->link_info[ind]));
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_CELL_FORMAT_clear(
    SOC_SAND_OUT SOC_TMC_FABRIC_CELL_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FABRIC_CELL_FORMAT));
  info->variable_cell_size_enable = TRUE;
  info->segmentation_enable = TRUE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_FC_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_TMC_FABRIC_FC *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FABRIC_FC));
  for (ind=0; ind<SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++ind)
  {
    info->enable[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_COEXIST_INFO_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_TMC_FABRIC_COEXIST_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FABRIC_COEXIST_INFO));
  info->enable = 0;
  for (ind=0; ind<SOC_TMC_FABRIC_NOF_COEXIST_DEV_ENTRIES; ++ind)
  {
    info->coexist_dest_map[ind] = 0;
  }
  for (ind=0; ind<SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++ind)
  {
    info->fap_id_odd_link_map[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_LINKS_STATUS_SINGLE_clear(
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_STATUS_SINGLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FABRIC_LINKS_STATUS_SINGLE));
  info->valid = 0;
  info->up_tx = 0;
  info->up_rx = 0;
  info->errors_bm = 0;
  info->indications_bm = 0;
  info->crc_err_counter = 0;
  info->leaky_bucket_counter = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_LINKS_STATUS_ALL_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_STATUS_ALL *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_FABRIC_LINKS_STATUS_ALL));
  info->error_in_some = 0;
  for (ind=0; ind<SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++ind)
  {
    SOC_TMC_FABRIC_LINKS_STATUS_SINGLE_clear(&(info->single_link_status[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_LINK_STATE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_LINK_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_LINK_STATE_INFO));
  info->on_off = SOC_TMC_LINK_NOF_STATES;
  info->serdes_also = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_TMC_FABRIC_FC_SHAPER_clear(
      SOC_SAND_OUT SOC_TMC_FABRIC_FC_SHAPER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  sal_memset(info, 0x0, sizeof(SOC_TMC_FABRIC_FC_SHAPER));
  info->data_shaper.bytes = 0;
  info->data_shaper.cells = 0;
  info->fc_shaper.bytes = 0;
  info->fc_shaper.cells = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO_clear(
      SOC_SAND_OUT SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  sal_memset(info, 0x0, sizeof(SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO));
  info->shaper_mode = SOC_TMC_FABRIC_SHAPER_NOF_MODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1


uint32
  soc_tmcfabric_topology_status_connectivity_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_TMC_TOPOLOGY_STATUS_CONNECTIVITY_PRINT);
  SOC_SAND_CHECK_NULL_INPUT(connectivity_map);

  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_print(unit,connectivity_map, TRUE);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_tmcfabric_topology_status_connectivity_print()",0,0);
}

const char*
  SOC_TMC_LINK_ERR_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_LINK_ERR_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_LINK_ERR_TYPE_CRC:
    str = "crc";
  break;
  case SOC_TMC_LINK_ERR_TYPE_SIZE:
    str = "size";
  break;
  case SOC_TMC_LINK_ERR_TYPE_MISALIGN:
    str = "misalign";
  break;
  case SOC_TMC_LINK_ERR_TYPE_CODE_GROUP:
    str = "code_group";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_LINK_INDICATE_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_LINK_INDICATE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_LINK_INDICATE_TYPE_SIG_LOCK:
    str = "sig_lock";
  break;
  case SOC_TMC_LINK_INDICATE_TYPE_ACCEPT_CELL:
    str = "accept_cell";
  break;
  case SOC_TMC_LINK_INDICATE_INTRNL_FIXED:
    str = "intrnl_fixed";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_FABRIC_CONNECT_MODE_to_string(
    SOC_SAND_IN SOC_TMC_FABRIC_CONNECT_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FABRIC_CONNECT_MODE_FE:
    str = "fe";
  break;
  case SOC_TMC_FABRIC_CONNECT_MODE_BACK2BACK:
    str = "back2back";
  break;
  case SOC_TMC_FABRIC_CONNECT_MODE_MESH:
    str = "mesh";
  break;
  case SOC_TMC_FABRIC_CONNECT_MODE_MULT_STAGE_FE:
    str = "mult_stage_fe";
  break;
  case SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP:
    str = "single_fap";
  break;
  case SOC_TMC_FABRIC_NOF_CONNECT_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_LINK_STATE_to_string(
    SOC_SAND_IN  SOC_TMC_LINK_STATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_LINK_STATE_ON:
    str = "on";
  break;
  case SOC_TMC_LINK_STATE_OFF:
    str = "off";
  break;
  case SOC_TMC_LINK_NOF_STATES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_FABRIC_LINE_CODING_to_string(
    SOC_SAND_IN  SOC_TMC_FABRIC_LINE_CODING enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FABRIC_LINE_CODING_8_10:
    str = "coding_8_10";
  break;
  case SOC_TMC_FABRIC_LINE_CODING_8_9_FEC:
    str = "coding_8_9_fec";
  break;
  case SOC_TMC_FABRIC_NOF_LINE_CODINGS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_LINKS_CONNECT_MAP_STAT_INFO_print(
    SOC_SAND_IN  SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "far_unit: %u\n\r"),info->far_unit));
  LOG_CLI((BSL_META_U(unit,
                      "far_dev_type %s "), SOC_TMC_FAR_DEVICE_TYPE_to_string(info->far_dev_type)));
  LOG_CLI((BSL_META_U(unit,
                      "far_link_id: %u\n\r"),info->far_link_id));
  LOG_CLI((BSL_META_U(unit,
                      "is_connected_expected: %u\n\r"),info->is_connected_expected));
  LOG_CLI((BSL_META_U(unit,
                      "is_phy_connect: %u\n\r"),info->is_phy_connect));
  LOG_CLI((BSL_META_U(unit,
                      "is_sending_traffic: %u\n\r"),info->is_logically_connected));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_CELL_FORMAT_print(
    SOC_SAND_IN SOC_TMC_FABRIC_CELL_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Variable_cell_size_enable: %u, "),info->variable_cell_size_enable));
  LOG_CLI((BSL_META_U(unit,
                      "Segmentation_enable: %u\n\r"),info->segmentation_enable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_FC_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_FABRIC_FC *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Enable[%2u]: %d\n\r"),ind,info->enable[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_COEXIST_INFO_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_FABRIC_COEXIST_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Enable: %d\n\r"),info->enable));
  for (ind=0; ind<SOC_TMC_FABRIC_NOF_COEXIST_DEV_ENTRIES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Coexist_dest_map[%3u]: %d\n\r"),ind,info->coexist_dest_map[ind]));
  }
  for (ind=0; ind<SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Fap_id_odd_link_map[%2u]: %d\n\r"),ind,info->fap_id_odd_link_map[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_print(
    SOC_SAND_IN SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info,
    SOC_SAND_IN uint8                         table_format
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if(!table_format)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Far_unit: %u "),info->far_unit));
    LOG_CLI((BSL_META_U(unit,
                        "Far_dev_type %s "),
             SOC_TMC_FAR_DEVICE_TYPE_to_string(info->far_dev_type)
             ));
    LOG_CLI((BSL_META_U(unit,
                        "Far_link_id: %u "),info->far_link_id));
    LOG_CLI((BSL_META_U(unit,
                        "Is_phy_connect: %u "),info->is_phy_connect));
    LOG_CLI((BSL_META_U(unit,
                        "Is_logically_connected: %u "),info->is_logically_connected));
    LOG_CLI((BSL_META_U(unit,
                        "Is_connected_expected: %u "),info->is_connected_expected));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "%4u(%s)| %2u   |     %s     |     %s     |    %s \n\r"),
             info->far_unit,
             SOC_TMC_FAR_DEVICE_TYPE_to_string(info->far_dev_type),
             info->far_link_id,
             (info->is_phy_connect)?"+":"-",
             (info->is_logically_connected)?"+":"-",
             (info->is_connected_expected)?"+":"-"
             ));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#define SOC_TMC_SRD_LANE_INVALID 60
uint32
  soc_tmcfbr2srd_lane_id(
    SOC_SAND_IN  uint32 fabric_link_id
  )
{
  uint32
    serdes_lane_id;

  if (fabric_link_id <= 27)
  {
    
    serdes_lane_id = fabric_link_id + 32;
  }
  else if (fabric_link_id <= 31)
  {
    
    serdes_lane_id = fabric_link_id - 16;
  }
  else if (fabric_link_id <= 35)
  {
    
    serdes_lane_id = fabric_link_id - 4;
  }
  else
  {
    serdes_lane_id = SOC_TMC_SRD_LANE_INVALID;
  }

  return serdes_lane_id;
}

void
  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR *info,
    SOC_SAND_IN uint8                     table_format
  )
{
  uint32
    ind=0,
    nof_valid_links = 0;
  uint8
    is_fabric_link;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if(table_format)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Fabr |Srds | Far     | Far  | Is Phy    | Is Logic  | Is Far Side\n\r"
                        "Link |Lane | Device  | Link | Connected | Connected | Expected   \n\r"
                        "-----------------------------------------------------------------\n\r"
                 )));
  }
  for (ind=0; ind<SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++ind)
  {
    
    is_fabric_link = SOC_SAND_NUM2BOOL_INVERSE(info->link_info[ind].far_unit == SOC_TMC_DEVICE_ID_IRRELEVANT);

    
    if (is_fabric_link)
    {
      if(table_format)
      {
        LOG_CLI((BSL_META_U(unit,
                            " %2d  | %2d  |"),ind, soc_tmcfbr2srd_lane_id(ind)));
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "Link_info[%u]: SerDes id: %u"),ind, soc_tmcfbr2srd_lane_id(ind)));
      }
      SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_print(&(info->link_info[ind]), table_format);

      if (info->link_info[ind].is_logically_connected == TRUE)
      {
        nof_valid_links++;
      }
    }
  }

  LOG_CLI((BSL_META_U(unit,
                      "Number of valid links: %d\n\r"),nof_valid_links));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_LINK_STATE_INFO_print(
    SOC_SAND_IN SOC_TMC_LINK_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "On_off %s \n\r"),
           SOC_TMC_LINK_STATE_to_string(info->on_off)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Serdes_also: %u\n\r"),info->serdes_also));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_TMC_FABRIC_LINKS_STATUS_ALL_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_FABRIC_LINKS_STATUS_ALL *info,
    SOC_SAND_IN SOC_SAND_PRINT_FLAVORS     print_flavour
  )
{
  uint32
    ind=0,
    nof_valid_links = 0;
  char
    crc_err_cnt[11];
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "error_in_some: %u\n\r"),info->error_in_some));

    LOG_CLI((BSL_META_U(unit,
                        "    |Srds|       Indications      |             Errors         |Up |Up |Leaky  \n\r"
                        "Link|Lane| Signal |Accept|Internal|          |Cell| mis  |Code |TX |RX |Bucket \n\r"
                        "ID  |ID  |  Lock  |Cells |  Fix   |   CRC    |Size|align |Group|   |   | Th    \n\r"
                        "-------------------------------------------------------------------------------\n\r"
                 )));

  for (ind=0; ind<SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++ind)
  {
    if(info->single_link_status[ind].indications_bm & SOC_TMC_LINK_INDICATE_TYPE_ACCEPT_CELL)
    {
      nof_valid_links++;
    }
    if(info->single_link_status[ind].valid == FALSE)
    {
      continue;
    }
    if(SOC_SAND_PRINT_FLAVORS_ERRS_ONLY == print_flavour && !info->single_link_status[ind].errors_bm)
    {
      continue;
    }
    if (!(info->single_link_status[ind].crc_err_counter))
    {
      sal_sprintf(crc_err_cnt, "%s",  (info->single_link_status[ind].errors_bm & SOC_TMC_LINK_ERR_TYPE_CRC)?"   ***    ":"     -    ");
    }
    else
    {
      sal_sprintf(crc_err_cnt, "%10u", info->single_link_status[ind].crc_err_counter);
    }
    LOG_CLI((BSL_META_U(unit,
                        " %2d | %2d |  %s   | %s  |  %s   |%s|%s | %s  | %s |%s|%s|   %2u    \n\r"),
             ind,
             soc_tmcfbr2srd_lane_id(ind),
             (info->single_link_status[ind].indications_bm & SOC_TMC_LINK_INDICATE_TYPE_SIG_LOCK)?" + ":"***",
             (info->single_link_status[ind].indications_bm & SOC_TMC_LINK_INDICATE_TYPE_ACCEPT_CELL)?" + ":"***",
             (info->single_link_status[ind].indications_bm & SOC_TMC_LINK_INDICATE_INTRNL_FIXED)?"***":" - ",
             crc_err_cnt,
             (info->single_link_status[ind].errors_bm & SOC_TMC_LINK_ERR_TYPE_SIZE)?"***":" - ",
             (info->single_link_status[ind].errors_bm & SOC_TMC_LINK_ERR_TYPE_MISALIGN)?"***":" - ",
             (info->single_link_status[ind].errors_bm & SOC_TMC_LINK_ERR_TYPE_CODE_GROUP)?"***":" - ",
             (info->single_link_status[ind].up_tx)?" + ":"***",
             (info->single_link_status[ind].up_rx)?" + ":"***",
             info->single_link_status[ind].leaky_bucket_counter
             ));
  }

  LOG_CLI((BSL_META_U(unit,
                      "Number of valid links: %d\n\r"),nof_valid_links));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>
