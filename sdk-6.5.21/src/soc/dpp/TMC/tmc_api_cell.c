/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/







#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/TMC/tmc_api_cell.h>


























void
  SOC_TMC_SR_CELL_LINK_LIST_clear(
    SOC_SAND_OUT SOC_TMC_SR_CELL_LINK_LIST *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SR_CELL_LINK_LIST));
  for (ind = 0; ind < SOC_TMC_CELL_NOF_LINKS_IN_PATH_LINKS; ++ind)
  {
    info->path_links[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CELL_MC_TBL_DATA_clear(
    SOC_SAND_OUT SOC_TMC_CELL_MC_TBL_DATA *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_CELL_MC_TBL_DATA));
  for (ind = 0; ind < SOC_TMC_CELL_MC_DATA_IN_UINT32S; ++ind)
  {
    info->data[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CELL_MC_TBL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_CELL_MC_TBL_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_CELL_MC_TBL_INFO));
  info->fe_location = SOC_TMC_CELL_NOF_FE_LOCATIONS;
  for (ind = 0; ind < SOC_TMC_CELL_MC_NOF_LINKS; ++ind)
  {
    info->path_links[ind] = 0;
  }
  for (ind = 0; ind < SOC_TMC_CELL_MC_NOF_CHANGES; ++ind)
  {
    info->filter[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_CELL_FE_LOCATION_to_string(
    SOC_SAND_IN  SOC_TMC_CELL_FE_LOCATION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CELL_FE_LOCATION_FE1:
    str = "fe1";
  break;
  case SOC_TMC_CELL_FE_LOCATION_FE2:
    str = "fe2";
  break;
  case SOC_TMC_CELL_NOF_FE_LOCATIONS:
    str = "nof_fe_locations";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_SR_CELL_LINK_LIST_print(
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_CELL_NOF_LINKS_IN_PATH_LINKS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "path_links[%u]: %u\n\r"), ind,info->path_links[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CELL_MC_TBL_DATA_print(
    SOC_SAND_IN  SOC_TMC_CELL_MC_TBL_DATA *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_CELL_MC_DATA_IN_UINT32S; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "data[%u]: %u\n\r"),ind,info->data[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_CELL_MC_TBL_INFO_print(
    SOC_SAND_IN  SOC_TMC_CELL_MC_TBL_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "fe_location %s "), SOC_TMC_CELL_FE_LOCATION_to_string(info->fe_location)));
  for (ind = 0; ind < SOC_TMC_CELL_MC_NOF_LINKS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "path_links[%u]: %u\n\r"),ind,info->path_links[ind]));
  }
  for (ind = 0; ind < SOC_TMC_CELL_MC_NOF_CHANGES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "filter[%u]: %u\n\r"),ind,info->filter[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>
