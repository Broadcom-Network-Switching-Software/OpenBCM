/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_cos.h>






















uint32
  soc_ppd_lif_cos_ac_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO                 *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_lif_cos_ac_profile_info_set_verify(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_ac_profile_info_set_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_ac_profile_info_set()", profile_ndx, 0);
}


uint32
  soc_ppd_lif_cos_ac_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_AC_PROFILE_INFO                 *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_lif_cos_ac_profile_info_get_verify(
          unit,
          profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_ac_profile_info_get_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_ac_profile_info_get()", profile_ndx, 0);
}


uint32
  soc_ppd_lif_cos_pwe_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO                *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_lif_cos_pwe_profile_info_set_verify(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_pwe_profile_info_set_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_pwe_profile_info_set()", profile_ndx, 0);
}


uint32
  soc_ppd_lif_cos_pwe_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PWE_PROFILE_INFO                *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_lif_cos_pwe_profile_info_get_verify(
          unit,
          profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_pwe_profile_info_get_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_pwe_profile_info_get()", profile_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      cos_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO                    *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_lif_cos_profile_info_set_verify(
          unit,
          cos_profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_info_set_unsafe(
          unit,
          cos_profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_info_set()", cos_profile_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      cos_profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_INFO                    *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_lif_cos_profile_info_get_verify(
          unit,
          cos_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_info_get_unsafe(
          unit,
          cos_profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_info_get()", cos_profile_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_map_l2_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY          *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  res = arad_pp_lif_cos_profile_map_l2_info_set_verify(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_map_l2_info_set_unsafe(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_l2_info_set()", map_tbl_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_map_l2_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY          *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  res = arad_pp_lif_cos_profile_map_l2_info_get_verify(
          unit,
          map_tbl_ndx,
          map_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_map_l2_info_get_unsafe(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_l2_info_get()", map_tbl_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_map_ip_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY          *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  res = arad_pp_lif_cos_profile_map_ip_info_set_verify(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_map_ip_info_set_unsafe(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_ip_info_set()", map_tbl_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_map_ip_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY          *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  res = arad_pp_lif_cos_profile_map_ip_info_get_verify(
          unit,
          map_tbl_ndx,
          map_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_map_ip_info_get_unsafe(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_ip_info_get()", map_tbl_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_map_mpls_label_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY        *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  res = arad_pp_lif_cos_profile_map_mpls_label_info_set_verify(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_map_mpls_label_info_set_unsafe(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_mpls_label_info_set()", map_tbl_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_map_mpls_label_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY        *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  res = arad_pp_lif_cos_profile_map_mpls_label_info_get_verify(
          unit,
          map_tbl_ndx,
          map_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_map_mpls_label_info_get_unsafe(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_mpls_label_info_get()", map_tbl_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_map_tc_dp_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY       *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  res = arad_pp_lif_cos_profile_map_tc_dp_info_set_verify(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_map_tc_dp_info_set_unsafe(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_tc_dp_info_set()", map_tbl_ndx, 0);
}


uint32
  soc_ppd_lif_cos_profile_map_tc_dp_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY       *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  res = arad_pp_lif_cos_profile_map_tc_dp_info_get_verify(
          unit,
          map_tbl_ndx,
          map_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_profile_map_tc_dp_info_get_unsafe(
          unit,
          map_tbl_ndx,
          map_key,
          map_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_tc_dp_info_get()", map_tbl_ndx, 0);
}


uint32
  soc_ppd_lif_cos_opcode_types_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_TYPE                     opcode_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_OPCODE_TYPES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_lif_cos_opcode_types_set_verify(
          unit,
          opcode_ndx,
          opcode_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_types_set_unsafe(
          unit,
          opcode_ndx,
          opcode_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_types_set()", opcode_ndx, 0);
}


uint32
  soc_ppd_lif_cos_opcode_types_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_TYPE                     *opcode_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_COS_OPCODE_TYPES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(opcode_type);

  res = arad_pp_lif_cos_opcode_types_get_verify(
          unit,
          opcode_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_types_get_unsafe(
          unit,
          opcode_ndx,
          opcode_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_types_get()", opcode_ndx, 0);
}


uint32
  soc_ppd_lif_cos_opcode_ipv6_tos_map_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO              *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_lif_cos_opcode_ipv6_tos_map_set_verify(
          unit,
          opcode_ndx,
          ipv6_tos_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_ipv6_tos_map_set_unsafe(
          unit,
          opcode_ndx,
          ipv6_tos_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_ipv6_tos_map_set()", opcode_ndx, ipv6_tos_ndx);
}


uint32
  soc_ppd_lif_cos_opcode_ipv6_tos_map_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO              *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_lif_cos_opcode_ipv6_tos_map_get_verify(
          unit,
          opcode_ndx,
          ipv6_tos_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_ipv6_tos_map_get_unsafe(
          unit,
          opcode_ndx,
          ipv6_tos_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_ipv6_tos_map_get()", opcode_ndx, ipv6_tos_ndx);
}


uint32
  soc_ppd_lif_cos_opcode_ipv4_tos_map_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO              *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_lif_cos_opcode_ipv4_tos_map_set_verify(
          unit,
          opcode_ndx,
          ipv4_tos_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_ipv4_tos_map_set_unsafe(
          unit,
          opcode_ndx,
          ipv4_tos_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_ipv4_tos_map_set()", opcode_ndx, ipv4_tos_ndx);
}


uint32
  soc_ppd_lif_cos_opcode_ipv4_tos_map_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO              *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_lif_cos_opcode_ipv4_tos_map_get_verify(
          unit,
          opcode_ndx,
          ipv4_tos_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_ipv4_tos_map_get_unsafe(
          unit,
          opcode_ndx,
          ipv4_tos_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_ipv4_tos_map_get()", opcode_ndx, ipv4_tos_ndx);
}


uint32
  soc_ppd_lif_cos_opcode_tc_dp_map_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO              *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_lif_cos_opcode_tc_dp_map_set_verify(
          unit,
          opcode_ndx,
          tc_ndx,
          dp_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_tc_dp_map_set_unsafe(
          unit,
          opcode_ndx,
          tc_ndx,
          dp_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_tc_dp_map_set()", opcode_ndx, tc_ndx);
}


uint32
  soc_ppd_lif_cos_opcode_tc_dp_map_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO              *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_lif_cos_opcode_tc_dp_map_get_verify(
          unit,
          opcode_ndx,
          tc_ndx,
          dp_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_tc_dp_map_get_unsafe(
          unit,
          opcode_ndx,
          tc_ndx,
          dp_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_tc_dp_map_get()", opcode_ndx, tc_ndx);
}


uint32
  soc_ppd_lif_cos_opcode_vlan_tag_map_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  uint8                                       tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO              *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_lif_cos_opcode_vlan_tag_map_set_verify(
          unit,
          opcode_ndx,
          tag_type_ndx,
          pcp_ndx,
          dei_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_vlan_tag_map_set_unsafe(
          unit,
          opcode_ndx,
          tag_type_ndx,
          pcp_ndx,
          dei_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_vlan_tag_map_set()", opcode_ndx, 0);
}


uint32
  soc_ppd_lif_cos_opcode_vlan_tag_map_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO              *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_lif_cos_opcode_vlan_tag_map_get_verify(
          unit,
          opcode_ndx,
          tag_type_ndx,
          pcp_ndx,
          dei_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_lif_cos_opcode_vlan_tag_map_get_unsafe(
          unit,
          opcode_ndx,
          tag_type_ndx,
          pcp_ndx,
          dei_ndx,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_vlan_tag_map_get()", opcode_ndx, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>



