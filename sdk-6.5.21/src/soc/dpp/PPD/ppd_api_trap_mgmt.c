/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TRAP



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>






















uint32
  soc_ppd_trap_frwrd_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info,
    SOC_SAND_IN  int                                    core_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_frwrd_profile_info_set_verify(
          unit,
          trap_code_ndx,
          profile_info,
          core_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_frwrd_profile_info_set_unsafe(
          unit,
          trap_code_ndx,
          profile_info,
          core_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_frwrd_profile_info_set()", 0, 0);
}


uint32
  soc_ppd_trap_frwrd_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_OUT SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_frwrd_profile_info_get_verify(
          unit,
          trap_code_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_frwrd_profile_info_get_unsafe(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_frwrd_profile_info_get()", 0, 0);
}


uint32
  soc_ppd_trap_snoop_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_snoop_profile_info_set_verify(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_snoop_profile_info_set_unsafe(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_snoop_profile_info_set()", 0, 0);
}


uint32
  soc_ppd_trap_snoop_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_OUT SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_snoop_profile_info_get_verify(
          unit,
          trap_code_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_snoop_profile_info_get_unsafe(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_snoop_profile_info_get()", 0, 0);
}


uint32
  soc_ppd_trap_to_eg_action_map_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                      eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_TO_EG_ACTION_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_trap_to_eg_action_map_set_verify(
          unit,
          trap_type_bitmap_ndx,
          eg_action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_to_eg_action_map_set_unsafe(
          unit,
          trap_type_bitmap_ndx,
          eg_action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_to_eg_action_map_set()", 0, 0);
}


uint32
  soc_ppd_trap_to_eg_action_map_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx,
    SOC_SAND_OUT uint32                                      *eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_TO_EG_ACTION_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_action_profile);

  res = arad_pp_trap_to_eg_action_map_get_verify(
          unit,
          trap_type_bitmap_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_to_eg_action_map_get_unsafe(
          unit,
          trap_type_bitmap_ndx,
          eg_action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_to_eg_action_map_get()", 0, 0);
}


uint32
  soc_ppd_trap_eg_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO             *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_EG_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_eg_profile_info_set_verify(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_eg_profile_info_set_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_eg_profile_info_set()", profile_ndx, 0);
}


uint32
  soc_ppd_trap_eg_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO             *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_EG_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_eg_profile_info_get_verify(
          unit,
          profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_eg_profile_info_get_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_eg_profile_info_get()", profile_ndx, 0);
}


uint32
  soc_ppd_trap_mact_event_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT uint32                                      buff[SOC_PPC_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_OUT uint32                                      *buff_len
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_MACT_EVENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(buff_len);

  res = arad_pp_trap_mact_event_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_mact_event_get_unsafe(
          unit,
          buff,
          buff_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_mact_event_get()", 0, 0);
}


uint32
  soc_ppd_trap_mact_event_parse(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      buff[SOC_PPC_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                      buff_len,
    SOC_SAND_OUT SOC_PPC_TRAP_MACT_EVENT_INFO                    *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_MACT_EVENT_PARSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  res = arad_pp_trap_mact_event_parse_verify(
          unit,
          buff,
          buff_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_mact_event_parse_unsafe(
          unit,
          buff,
          buff_len,
          mact_event
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_mact_event_parse()", 0, 0);
}



uint32
  soc_ppd_trap_packet_parse(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                 *buff,
    SOC_SAND_IN  uint32                                buff_len,
    SOC_SAND_OUT SOC_PPC_TRAP_PACKET_INFO             *packet_info,
    SOC_SAND_OUT soc_pkt_t                            *dnx_pkt
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_TRAP_MGMT_PACKET_PARSE_INFO
    packet_parse_info;
  int32
    semaphore_was_taken ;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  semaphore_was_taken = 0 ;
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(buff);
  SOC_SAND_CHECK_NULL_INPUT(packet_info);

  if (buff_len == 0)
  {
    packet_info->size_in_bytes = 0;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }


  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  semaphore_was_taken = 1 ;
  res = arad_pp_trap_packet_parse_info_get_unsafe(unit, &packet_parse_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PPC_TRAP_PACKET_INFO_clear(packet_info);
 

  res = arad_pp_trap_packet_parse_unsafe(unit, buff, buff_len,&packet_parse_info,packet_info,(soc_pkt_t *)dnx_pkt);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  if (semaphore_was_taken)
  {
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_trap_packet_parse()", buff_len, 0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>



