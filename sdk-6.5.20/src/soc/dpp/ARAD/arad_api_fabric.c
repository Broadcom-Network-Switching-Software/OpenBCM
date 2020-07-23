#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
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
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_fabric.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_general.h>



























uint32
  arad_fabric_fc_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC           *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fabric_fc_enable_verify(
          unit,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fabric_fc_enable_set_unsafe(
          unit,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_enable_set()",0,0);
}


uint32
  arad_fabric_fc_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_FABRIC_FC           *info_rx,
    SOC_SAND_OUT ARAD_FABRIC_FC           *info_tx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fabric_fc_enable_get_unsafe(
          unit,
          info_rx,
          info_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_enable_get()",0,0);
}


uint32
  arad_fabric_fc_shaper_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        link_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER            *info,
    SOC_SAND_OUT ARAD_FABRIC_FC_SHAPER            *exact_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_SHAPER_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(shaper_mode);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = arad_fabric_fc_shaper_verify(
          unit,
          link_ndx,
          shaper_mode,
          info,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fabric_fc_shaper_set_unsafe(
          unit,
          link_ndx,
          shaper_mode,
          info,
          exact_info
        );

  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_shaper_set()",link_ndx,0);
}




uint32
  arad_fabric_fc_shaper_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         link_ndx,
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER            *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(shaper_mode);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fabric_fc_shaper_get_unsafe(
          unit,
          link_ndx,
          shaper_mode,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_shaper_get()",link_ndx,0);
}




uint32
  arad_fabric_stand_alone_fap_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_single_fap_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_STAND_ALONE_FAP_MODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_single_fap_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fabric_stand_alone_fap_mode_get_unsafe(
    unit,
    is_single_fap_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_stand_alone_fap_mode_get()",0,0);
}


uint32
  arad_fabric_connect_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FABRIC_CONNECT_MODE fabric_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_CONNECT_MODE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = arad_fabric_connect_mode_verify(
    unit,
    fabric_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fabric_connect_mode_set_unsafe(
    unit,
    fabric_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_connect_mode_set()",0,0);
}



uint32
  arad_fabric_topology_status_connectivity_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    link_index_min,
    SOC_SAND_IN  int                    link_index_max,
    SOC_SAND_OUT ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TOPOLOGY_STATUS_CONNECTIVITY_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(connectivity_map);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_fabric_topology_status_connectivity_get_unsafe(
    unit,
    link_index_min,
    link_index_max,
    connectivity_map
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_topology_status_connectivity_get()",0,0);
}



void
  arad_ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(
    SOC_SAND_OUT ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_FABRIC_CELL_FORMAT_clear(
    SOC_SAND_OUT ARAD_FABRIC_CELL_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_CELL_FORMAT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_FABRIC_FC_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT ARAD_FABRIC_FC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_FC_clear(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_FABRIC_FC_SHAPER_clear(
    SOC_SAND_OUT ARAD_FABRIC_FC_SHAPER *shaper
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(shaper);

  SOC_TMC_FABRIC_FC_SHAPER_clear(shaper);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_FABRIC_FC_SHAPER_MODE_INFO_clear(
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(shaper_mode);

  SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO_clear(shaper_mode);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_LINK_STATE_INFO_clear(
    SOC_SAND_OUT ARAD_LINK_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_LINK_STATE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  arad_ARAD_FABRIC_CONNECT_MODE_to_string(
    SOC_SAND_IN  ARAD_FABRIC_CONNECT_MODE enum_val
  )
{
  return SOC_TMC_FABRIC_CONNECT_MODE_to_string(enum_val);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
