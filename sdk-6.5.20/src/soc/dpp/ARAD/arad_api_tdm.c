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

#define _ERR_MSG_MODULE_NAME BSL_SOC_TDM


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_tdm.h>
#include <soc/dpp/ARAD/arad_tdm.h>






















uint32
  arad_tdm_ftmh_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_FTMH_SET);

  

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_tdm_ftmh_set_verify(
          unit,
          port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_ftmh_set_unsafe(
          unit,
          core_id,
          port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ftmh_set()", port_ndx, 0);
}


uint32
  arad_tdm_ftmh_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT ARAD_TDM_FTMH_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_FTMH_GET);

  

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_tdm_ftmh_get_verify(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_ftmh_get_unsafe(
          unit,
          core_id,
          port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ftmh_get()", port_ndx, 0);
}


uint32
  arad_tdm_opt_size_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_OPT_SIZE_SET);

  

  res = arad_tdm_opt_size_set_verify(
          unit,
          cell_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_opt_size_set_unsafe(
          unit,
          cell_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_opt_size_set()", cell_size, 0);
}


uint32
  arad_tdm_opt_size_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *cell_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_OPT_SIZE_GET);

  

  SOC_SAND_CHECK_NULL_INPUT(cell_size);

  res = arad_tdm_opt_size_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_opt_size_get_unsafe(
          unit,
          cell_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_opt_size_get()", 0, 0);
}


uint32
  arad_tdm_stand_size_range_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_STAND_SIZE_RANGE_SET);

  

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  res = arad_tdm_stand_size_range_set_verify(
          unit,
          size_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_stand_size_range_set_unsafe(
          unit,
          size_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_stand_size_range_set()", 0, 0);
}


uint32
  arad_tdm_stand_size_range_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_STAND_SIZE_RANGE_GET);

  

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  res = arad_tdm_stand_size_range_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_stand_size_range_get_unsafe(
          unit,
          size_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_stand_size_range_get()", 0, 0);
}


uint32
  arad_tdm_direct_routing_profile_map_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint32                   direct_routing_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_SET);

  

  res = arad_tdm_direct_routing_profile_map_set_verify(
          unit,
          port_ndx,
          direct_routing_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_direct_routing_profile_map_set_unsafe(
          unit,
          port_ndx,
          direct_routing_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_profile_map_set()", port_ndx, 0);
}


uint32
  arad_tdm_direct_routing_profile_map_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint32                   *direct_routing_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_GET);

  

  SOC_SAND_CHECK_NULL_INPUT(direct_routing_profile);

  res = arad_tdm_direct_routing_profile_map_get_verify(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_direct_routing_profile_map_get_unsafe(
          unit,
          port_ndx,
          direct_routing_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_profile_map_get()", port_ndx, 0);
}


uint32
  arad_tdm_direct_routing_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_IN  uint8 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_SET);

  

  SOC_SAND_CHECK_NULL_INPUT(direct_routing_info);

  res = arad_tdm_direct_routing_set_verify(
          unit,
          direct_routing_profile,
          direct_routing_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_direct_routing_set_unsafe(
          unit,
          direct_routing_profile,
          direct_routing_info,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_set()", direct_routing_profile, 0);
}


uint32
  arad_tdm_direct_routing_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_OUT ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_OUT uint8 *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_GET);

  

  SOC_SAND_CHECK_NULL_INPUT(direct_routing_info);

  res = arad_tdm_direct_routing_get_verify(
          unit,
          direct_routing_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_direct_routing_get_unsafe(
          unit,
          direct_routing_profile,
          direct_routing_info,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_get()", direct_routing_profile, 0);
}


uint32
  arad_tdm_port_packet_crc_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port_ndx,          
    SOC_SAND_IN  uint8                    is_enable,         
    SOC_SAND_IN  uint8                    configure_ingress, 
    SOC_SAND_IN  uint8                    configure_egress   
  )
{
  int core;
  uint32
    tm_port,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_PORT_PACKET_CRC_SET);

  
  res = soc_port_sw_db_local_to_tm_port_get(unit, port_ndx , &tm_port , &core);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_tdm_port_packet_crc_set_verify(
          unit,
          tm_port,
          is_enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_port_packet_crc_set_unsafe(unit, port_ndx, is_enable, configure_ingress, configure_egress);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_port_packet_crc_set()", port_ndx, 0);
}


uint32
  arad_tdm_port_packet_crc_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t               port_ndx,
    SOC_SAND_OUT uint8                    *is_ingress_enabled,
    SOC_SAND_OUT uint8                    *is_egress_enabled
  )
{
  int core;
  uint32
    tm_port,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_PORT_PACKET_CRC_GET);

  
  res = soc_port_sw_db_local_to_tm_port_get(unit, port_ndx , &tm_port , &core);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_tdm_port_packet_crc_get_verify(
          unit,
          tm_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  res = arad_tdm_port_packet_crc_get_unsafe(unit, port_ndx, is_ingress_enabled, is_egress_enabled);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_port_packet_crc_get()", port_ndx, 0);
}

void
  ARAD_TDM_FTMH_OPT_UC_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_OPT_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_OPT_UC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_OPT_MC_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_OPT_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_OPT_MC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_STANDARD_UC_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_STANDARD_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_STANDARD_UC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_STANDARD_MC_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_STANDARD_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_STANDARD_MC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_INFO_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  ARAD_TDM_MC_STATIC_ROUTE_INFO_clear(
    SOC_SAND_OUT ARAD_TDM_MC_STATIC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_DIRECT_ROUTING_INFO_clear(
    SOC_SAND_OUT ARAD_TDM_DIRECT_ROUTING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_TDM_ING_ACTION_to_string(
    SOC_SAND_IN  ARAD_TDM_ING_ACTION enum_val
  )
{
  return SOC_TMC_TDM_ING_ACTION_to_string(enum_val);
}

const char*
  ARAD_TDM_EG_ACTION_to_string(
    SOC_SAND_IN  ARAD_TDM_EG_ACTION enum_val
  )
{
  return SOC_TMC_TDM_EG_ACTION_to_string(enum_val);
}

void
  ARAD_TDM_FTMH_OPT_UC_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_OPT_UC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_OPT_MC_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_OPT_MC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_STANDARD_UC_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_STANDARD_UC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_STANDARD_MC_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_STANDARD_MC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_print(
    SOC_SAND_IN  ARAD_TDM_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TDM_FTMH_INFO_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  ARAD_TDM_MC_STATIC_ROUTE_INFO_print(
    SOC_SAND_IN  ARAD_TDM_MC_STATIC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

