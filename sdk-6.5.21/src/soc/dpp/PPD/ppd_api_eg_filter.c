
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_filter.h>






















uint32
  soc_ppd_eg_filter_port_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_eg_filter_port_info_set_verify(
          unit,
          out_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_port_info_set_unsafe(
          unit,
          core_id,
          out_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_port_info_set()", out_port_ndx, 0);
}


uint32
  soc_ppd_eg_filter_port_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_eg_filter_port_info_get_verify(
          unit,
          out_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_port_info_get_unsafe(
          unit,
          core_id,
          out_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_port_info_get()", out_port_ndx, 0);
}

#ifdef BCM_88660_A0
uint32
  soc_ppd_eg_filter_global_info_set(
    SOC_SAND_IN int                               unit,
    SOC_SAND_IN SOC_PPC_EG_FILTER_GLOBAL_INFO          *global_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(global_info);

  res = arad_pp_eg_filter_global_info_set_verify(
          unit,
          global_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_global_info_set_unsafe(
          unit,
          global_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_global_info_set()", 0, 0);
}

uint32
  soc_ppd_eg_filter_global_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_GLOBAL_INFO        *global_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(global_info);

  res = arad_pp_eg_filter_global_info_get_verify(
          unit,
          global_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_global_info_get_unsafe(
          unit,
          global_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_global_info_get()", 0, 0);
}

#endif 


uint32
  soc_ppd_eg_filter_vsi_port_membership_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_filter_vsi_port_membership_set_verify(
          unit,
          vsid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_vsi_port_membership_set_unsafe(
          unit,
          core_id,
          vsid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_vsi_port_membership_set()", vsid_ndx, out_port_ndx);
}


uint32
  soc_ppd_eg_filter_vsi_port_membership_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  res = arad_pp_eg_filter_vsi_port_membership_get_verify(
          unit,
          vsid_ndx,
          out_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_vsi_port_membership_get_unsafe(
          unit,
          core_id,
          vsid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_vsi_port_membership_get()", vsid_ndx, out_port_ndx);
}


uint32
  soc_ppd_eg_filter_vsi_membership_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_OUT uint32                               ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_VSI_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_filter_vsi_membership_get_verify(
          unit,
          vsid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_vsi_membership_get_unsafe(
          unit,
          core_id,
          vsid_ndx,
          ports
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_vsi_membership_get()", vsid_ndx, 0);
}


uint32
  soc_ppd_eg_filter_cvid_port_membership_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_filter_cvid_port_membership_set_verify(
          unit,
          cvid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_cvid_port_membership_set_unsafe(
          unit,
          cvid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_cvid_port_membership_set()", cvid_ndx, out_port_ndx);
}


uint32
  soc_ppd_eg_filter_cvid_port_membership_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  res = arad_pp_eg_filter_cvid_port_membership_get_verify(
          unit,
          cvid_ndx,
          out_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_cvid_port_membership_get_unsafe(
          unit,
          cvid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_cvid_port_membership_get()", cvid_ndx, out_port_ndx);
}


uint32
  soc_ppd_eg_filter_port_acceptable_frames_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          eg_acceptable_frames_port_profile,
    SOC_SAND_IN  uint32                          llvp_port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO          *eg_prsr_out_key,
    SOC_SAND_IN  uint8                           accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);

  res = arad_pp_eg_filter_port_acceptable_frames_set_verify(
          unit,
          eg_acceptable_frames_port_profile,
          llvp_port_profile,
          eg_prsr_out_key,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_port_acceptable_frames_set_unsafe(
          unit,
          eg_acceptable_frames_port_profile,
          llvp_port_profile,
          eg_prsr_out_key,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_port_acceptable_frames_set()", eg_acceptable_frames_port_profile, 0);
}


uint32
  soc_ppd_eg_filter_port_acceptable_frames_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           out_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *eg_prsr_out_key,
    SOC_SAND_OUT uint8                          *accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  res = arad_pp_eg_filter_port_acceptable_frames_get_verify(
          unit,
          out_port_ndx,
          eg_prsr_out_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_port_acceptable_frames_get_unsafe(
          unit,
          out_port_ndx,
          eg_prsr_out_key,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_port_acceptable_frames_get()", out_port_ndx, 0);
}


uint32
  soc_ppd_eg_filter_pep_acceptable_frames_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_IN  uint8                          accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pep_key);

  res = arad_pp_eg_filter_pep_acceptable_frames_set_verify(
          unit,
          pep_key,
          vlan_format_ndx,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_pep_acceptable_frames_set_unsafe(
          unit,
          pep_key,
          vlan_format_ndx,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_pep_acceptable_frames_set()", 0, 0);
}


uint32
  soc_ppd_eg_filter_pep_acceptable_frames_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_OUT uint8                          *accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  res = arad_pp_eg_filter_pep_acceptable_frames_get_verify(
          unit,
          pep_key,
          vlan_format_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_pep_acceptable_frames_get_unsafe(
          unit,
          pep_key,
          vlan_format_ndx,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_pep_acceptable_frames_get()", 0, 0);
}


uint32
  soc_ppd_eg_filter_pvlan_port_type_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE    pvlan_port_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  res = arad_pp_eg_filter_pvlan_port_type_set_verify(
          unit,
          src_sys_port_ndx,
          pvlan_port_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_pvlan_port_type_set_unsafe(
          unit,
          src_sys_port_ndx,
          pvlan_port_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_pvlan_port_type_set()", 0, 0);
}


uint32
  soc_ppd_eg_filter_pvlan_port_type_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE    *pvlan_port_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(pvlan_port_type);

  res = arad_pp_eg_filter_pvlan_port_type_get_verify(
          unit,
          src_sys_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_pvlan_port_type_get_unsafe(
          unit,
          src_sys_port_ndx,
          pvlan_port_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_pvlan_port_type_get()", 0, 0);
}


uint32
  soc_ppd_eg_filter_split_horizon_out_ac_orientation_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_filter_split_horizon_out_ac_orientation_set_verify(
          unit,
          out_ac_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_split_horizon_out_ac_orientation_set_unsafe(
          unit,
          out_ac_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_split_horizon_out_ac_orientation_set()", out_ac_ndx, 0);
}


uint32
  soc_ppd_eg_filter_split_horizon_out_ac_orientation_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(orientation);

  res = arad_pp_eg_filter_split_horizon_out_ac_orientation_get_verify(
          unit,
          out_ac_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_split_horizon_out_ac_orientation_get_unsafe(
          unit,
          out_ac_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_split_horizon_out_ac_orientation_get()", out_ac_ndx, 0);
}



uint32
  soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_filter_split_horizon_out_lif_orientation_set_verify(
          unit,
          lif_eep_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_split_horizon_out_lif_orientation_set_unsafe(
          unit,
          lif_eep_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_split_horizon_out_lif_orientation_set()", lif_eep_ndx, 0);
}



uint32
  soc_ppd_eg_filter_split_horizon_out_lif_orientation_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          lif_eep_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(orientation);

  res = arad_pp_eg_filter_split_horizon_out_lif_orientation_get_verify(
          unit,
          lif_eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_filter_split_horizon_out_lif_orientation_get_unsafe(
          unit,
          lif_eep_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_filter_split_horizon_out_lif_orientation_get()", lif_eep_ndx, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>



