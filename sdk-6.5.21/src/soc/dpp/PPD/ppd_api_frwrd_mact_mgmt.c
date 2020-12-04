
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>

#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
























uint32
  soc_ppd_frwrd_mact_lookup_type_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_LOOKUP_TYPE         lookup_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_ERR_IF_ABOVE_MAX(lookup_type, SOC_PPC_NOF_FRWRD_MACT_LOOKUP_TYPES-1, ARAD_PP_FRARD_MACT_FRWRD_MACT_LOOKUP_TYPE_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_lookup_type_set_unsafe(
          unit,
          lookup_type,
          0x2
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_lookup_type_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_oper_mode_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  res = arad_pp_frwrd_mact_oper_mode_info_set_verify(
          unit,
          oper_mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_oper_mode_info_set_unsafe(
          unit,
          oper_mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_oper_mode_info_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_oper_mode_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_oper_mode_info_get_unsafe(
          unit,
          oper_mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_oper_mode_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_aging_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  res = arad_pp_frwrd_mact_aging_info_set_verify(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_aging_info_set_unsafe(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_info_set()", 0, 0);
}



uint32
  soc_ppd_frwrd_mact_aging_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_aging_info_get_unsafe(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_info_get()", 0, 0);
}



uint32
  soc_ppd_frwrd_mact_aging_one_pass_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO *pass_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ONE_PASS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pass_info);
  SOC_SAND_CHECK_NULL_INPUT(success);
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_aging_one_pass_set_unsafe(
          unit,
          pass_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_one_pass_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_aging_events_handle_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(aging_info);
  res = arad_pp_frwrd_mact_aging_events_handle_info_set_verify(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = arad_pp_frwrd_mact_aging_events_handle_info_set_unsafe(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_events_handle_info_set()", 0, 0);
}

uint32
  soc_ppd_frwrd_mact_aging_events_handle_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = arad_pp_frwrd_mact_aging_events_handle_info_get_unsafe(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_events_handle_info_get()", 0, 0);
}

uint32
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FID                                 fid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_mact_fid_profile_to_fid_map_set_verify(
          unit,
          fid_profile_ndx,
          fid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe(
          unit,
          fid_profile_ndx,
          fid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_fid_profile_to_fid_map_set()", fid_profile_ndx, 0);
}


uint32
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_OUT SOC_PPC_FID                                 *fid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fid);

  res = arad_pp_frwrd_mact_fid_profile_to_fid_map_get_verify(
          unit,
          fid_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_fid_profile_to_fid_map_get_unsafe(
          unit,
          fid_profile_ndx,
          fid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_fid_profile_to_fid_map_get()", fid_profile_ndx, 0);
}


uint32
  soc_ppd_frwrd_mact_mac_limit_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  res = arad_pp_frwrd_mact_mac_limit_glbl_info_set_verify(
          unit,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe(
          unit,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mac_limit_glbl_info_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_mac_limit_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_mac_limit_glbl_info_get_unsafe(
          unit,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mac_limit_glbl_info_get()", 0, 0);
}



uint32
  soc_ppd_frwrd_mact_learn_profile_limit_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  res = arad_pp_frwrd_mact_learn_profile_limit_info_set_verify(
          unit,
          mac_learn_profile_ndx,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_learn_profile_limit_info_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_profile_limit_info_set()", mac_learn_profile_ndx, 0);
}


uint32
  soc_ppd_frwrd_mact_learn_profile_limit_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  res = arad_pp_frwrd_mact_learn_profile_limit_info_get_verify(
          unit,
          mac_learn_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_learn_profile_limit_info_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_profile_limit_info_get()", mac_learn_profile_ndx, 0);
}

uint32
  soc_ppd_frwrd_mact_event_handle_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  res = arad_pp_frwrd_mact_event_handle_profile_set_verify(
          unit,
          mac_learn_profile_ndx,
          event_handle_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = arad_pp_frwrd_mact_event_handle_profile_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          event_handle_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_handle_profile_set()", mac_learn_profile_ndx, 0);
}

uint32
  soc_ppd_frwrd_mact_event_handle_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                                  *event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(event_handle_profile);
  res = arad_pp_frwrd_mact_event_handle_profile_get_verify(
          unit,
          mac_learn_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = arad_pp_frwrd_mact_event_handle_profile_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          event_handle_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_handle_profile_get()", mac_learn_profile_ndx, 0);
}






uint32
  soc_ppd_frwrd_mact_fid_aging_profile_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32         mac_learn_profile_ndx,
    SOC_SAND_IN  uint32         fid_aging_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  res = arad_pp_frwrd_mact_fid_aging_profile_set_verify(
          unit,
          mac_learn_profile_ndx,
          fid_aging_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = arad_pp_frwrd_mact_fid_aging_profile_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          fid_aging_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_fid_aging_profile_set()", mac_learn_profile_ndx, 0);
}

uint32
  soc_ppd_frwrd_mact_fid_aging_profile_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       mac_learn_profile_ndx,
    SOC_SAND_OUT uint32      *fid_aging_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(fid_aging_profile);
  res = arad_pp_frwrd_mact_fid_aging_profile_get_verify(
          unit,
          mac_learn_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = arad_pp_frwrd_mact_fid_aging_profile_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          fid_aging_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_fid_aging_profile_get()", mac_learn_profile_ndx, 0);
}


uint32
 soc_ppd_frwrd_mact_aging_profile_config(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32          fid_aging_profile,
    SOC_SAND_IN  uint32          fid_aging_cycles
  )
{
    ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE    aging_event_table;
    SOC_PPC_FRWRD_MACT_OPER_MODE_INFO       oper_mode_info;
    int32 age_ndx;
    uint32 is_owned;
    int32 final_age;
    uint8  config_aged_out = FALSE;
    uint8  config_deleted = FALSE;

    uint32 rv = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(&aging_event_table);

    soc_ppd_frwrd_mact_oper_mode_info_get(unit, &oper_mode_info);

    
    switch (oper_mode_info.learning_mode) {
    case    SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case    SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
    case    SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
        config_deleted = TRUE;
        break;
    case    SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case    SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
        config_aged_out = TRUE;
        break;
    default:
        config_deleted = TRUE;
        config_aged_out = TRUE;
    }
    final_age = (ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES - 2) - fid_aging_cycles; 

    for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
    {
        for (age_ndx = 0; age_ndx <= final_age; ++age_ndx)
        {
            aging_event_table.age_action[age_ndx][is_owned].aged_out = config_aged_out;
            aging_event_table.age_action[age_ndx][is_owned].deleted = config_deleted;
            aging_event_table.age_action[age_ndx][is_owned].refreshed = FALSE;
        }
      
        for (; age_ndx < (ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES - 2); ++age_ndx)
        {
            aging_event_table.age_action[age_ndx][is_owned].aged_out = FALSE;
            aging_event_table.age_action[age_ndx][is_owned].deleted = FALSE;
            aging_event_table.age_action[age_ndx][is_owned].refreshed = FALSE;
        }

        aging_event_table.age_action[ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES - 2][is_owned].aged_out = FALSE;
        aging_event_table.age_action[ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES - 2][is_owned].deleted = FALSE;
        aging_event_table.age_action[ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES - 2][is_owned].refreshed = TRUE;

        aging_event_table.age_action[ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES - 1][is_owned].aged_out = FALSE;
        aging_event_table.age_action[ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES - 1][is_owned].deleted = FALSE;
        aging_event_table.age_action[ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES - 1][is_owned].refreshed = FALSE;
    }

        rv = arad_pp_frwrd_mact_age_conf_write(unit, fid_aging_profile, &aging_event_table);
        SOC_SAND_CHECK_FUNC_RESULT(rv, 30, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_profile_config()", fid_aging_profile, 0);
 }


uint32
  soc_ppd_frwrd_mact_event_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(event_buf);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_event_get_unsafe(
          unit,
          event_buf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_event_handle_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);
  res = arad_pp_frwrd_mact_event_handle_info_set_verify(
          unit,
          event_key,
          handle_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = arad_pp_frwrd_mact_event_handle_info_set_unsafe(
          unit,
          event_key,
          handle_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_handle_info_set()", 0, 0);
}

uint32
  soc_ppd_frwrd_mact_event_handle_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);
  res = arad_pp_frwrd_mact_event_handle_info_get_verify(
          unit,
          event_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = arad_pp_frwrd_mact_event_handle_info_get_unsafe(
          unit,
          event_key,
          handle_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_handle_info_get()", 0, 0);
}

uint32
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  res = arad_pp_frwrd_mact_learn_msgs_distribution_info_set_verify(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_msgs_distribution_info_set()", 0, 0);
}

uint32
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_learn_msgs_distribution_info_get_unsafe(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_msgs_distribution_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  res = arad_pp_frwrd_mact_shadow_msgs_distribution_info_set_verify(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_mac_limit_exceeded_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(exceed_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_mac_limit_exceeded_info_get_unsafe(
          unit,
          exceed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mac_limit_exceeded_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_mac_limit_range_map_info_get(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  int8                                           range_num,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO    *map_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_info);

  
  res = arad_pp_frwrd_mact_mac_limit_range_map_info_get_verify(
          unit,
          range_num
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_mac_limit_range_map_info_get_unsafe(
          unit,
          range_num,
          map_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mac_limit_range_map_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_mac_limit_mapping_info_get(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO      *map_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_mac_limit_mapping_info_get_unsafe(
          unit,
          map_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mac_limit_mapping_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mapped_val,
    SOC_SAND_OUT uint32                                 *limit_tbl_idx,
    SOC_SAND_OUT uint32                                 *is_reserved
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_tbl_idx);
 SOC_SAND_CHECK_NULL_INPUT(is_reserved);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_limit_mapped_val_to_table_index_get_unsafe(
          unit,
          mapped_val,
          limit_tbl_idx,
          is_reserved
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_frwrd_mact_port_info_set_verify(
          unit,
          local_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_port_info_set_unsafe(
          unit,
          core_id,
          local_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_port_info_set()", local_port_ndx, 0);
}


uint32
  soc_ppd_frwrd_mact_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_frwrd_mact_port_info_get_verify(
          unit,
          local_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_port_info_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_port_info_get()", local_port_ndx, 0);
}


uint32
  soc_ppd_frwrd_mact_trap_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  res = arad_pp_frwrd_mact_trap_info_set_verify(
          unit,
          trap_type_ndx,
          port_profile_ndx,
          action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_trap_info_set_unsafe(
          unit,
          trap_type_ndx,
          port_profile_ndx,
          action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_trap_info_set()", 0, port_profile_ndx);
}


uint32
  soc_ppd_frwrd_mact_trap_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  res = arad_pp_frwrd_mact_trap_info_get_verify(
          unit,
          trap_type_ndx,
          port_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_trap_info_get_unsafe(
          unit,
          trap_type_ndx,
          port_profile_ndx,
          action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_trap_info_get()", 0, port_profile_ndx);
}


uint32
  soc_ppd_frwrd_mact_ip_compatible_mc_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_pp_frwrd_mact_ip_compatible_mc_info_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_ip_compatible_mc_info_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_ip_compatible_mc_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_ip_compatible_mc_info_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_ip_compatible_mc_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_event_parse(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_INFO              *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_PARSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(event_buf);
  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  res = arad_pp_frwrd_mact_event_parse_verify(
          unit,
          event_buf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_event_parse_unsafe(
          unit,
          event_buf,
          mact_event
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_parse()", 0, 0);
}

uint32
  soc_ppd_frwrd_mact_learn_msg_conf_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_PPC_FRWRD_MACT_LEARN_MSG          *learn_msg,
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF       *learn_msg_conf
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_learn_msg_conf_get_unsafe(
          unit,
          learn_msg,
          learn_msg_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_msg_conf_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_learn_msg_parse(
      SOC_SAND_IN   int                                   unit,
      SOC_SAND_IN   SOC_PPC_FRWRD_MACT_LEARN_MSG                  *learn_msg,
      SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO       *learn_events
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF             learn_msg_conf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(learn_msg);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_ppd_frwrd_mact_learn_msg_conf_get(
         unit,
         learn_msg,
         &learn_msg_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);


  res = arad_pp_frwrd_mact_learn_msg_parse_unsafe(
          unit,
          learn_msg,
          &learn_msg_conf,
          learn_events
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_msg_parse()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_mim_init_set(
      SOC_SAND_IN   int                                   unit,
      SOC_SAND_IN   uint8                                    mim_initialized
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_MIM_INIT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_mim_init_set_unsafe(
          unit,
          mim_initialized
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mim_init_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_mim_init_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_OUT  uint8                            *mim_initialized
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_MIM_INIT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mim_initialized);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_mim_init_get_unsafe(
          unit,
          mim_initialized
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mim_init_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_routed_learning_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32                appFlags
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ROUTED_LEARNING_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_routed_learning_set_unsafe(
          unit,
          appFlags
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_routed_learning_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_routed_learning_get(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT uint32                *appFlags
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ROUTED_LEARNING_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_routed_learning_get_unsafe(
          unit,
          appFlags
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_routed_learning_get()", 0, 0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>



