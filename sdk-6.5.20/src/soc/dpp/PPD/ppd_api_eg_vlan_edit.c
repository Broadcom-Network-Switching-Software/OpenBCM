
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_vlan_edit.h>






















uint32
  soc_ppd_eg_vlan_edit_pep_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                                 *pep_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(pep_info);

  res = arad_pp_eg_vlan_edit_pep_info_set_verify(
          unit,
          pep_key,
          pep_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pep_info_set_unsafe(
          unit,
          pep_key,
          pep_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pep_info_set()", 0, 0);
}


uint32
  soc_ppd_eg_vlan_edit_pep_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                                 *pep_key,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(pep_info);

  res = arad_pp_eg_vlan_edit_pep_info_get_verify(
          unit,
          pep_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pep_info_get_unsafe(
          unit,
          pep_key,
          pep_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pep_info_get()", 0, 0);
}


uint32
  soc_ppd_eg_vlan_edit_command_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(command_key);
  SOC_SAND_CHECK_NULL_INPUT(command_info);

  res = arad_pp_eg_vlan_edit_command_info_set_verify(
          unit,
          command_key,
          command_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_command_info_set_unsafe(
          unit,
          command_key,
          command_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_command_info_set()", 0, 0);
}


uint32
  soc_ppd_eg_vlan_edit_command_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(command_key);
  SOC_SAND_CHECK_NULL_INPUT(command_info);

  res = arad_pp_eg_vlan_edit_command_info_get_verify(
          unit,
          command_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_command_info_get_unsafe(
          unit,
          command_key,
          command_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_command_info_get()", 0, 0);
}


uint32
  soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                     transmit_outer_tag
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_verify(
          unit,
          local_port_ndx,
          vid_ndx,
          transmit_outer_tag
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          vid_ndx,
          transmit_outer_tag
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set()", local_port_ndx, vid_ndx);
}


uint32
  soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint8                                     *transmit_outer_tag
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(transmit_outer_tag);

  res = arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_verify(
          unit,
          local_port_ndx,
          vid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          vid_ndx,
          transmit_outer_tag
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get()", local_port_ndx, vid_ndx);
}


uint32
  soc_ppd_eg_vlan_edit_pcp_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY        key_mapping
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_vlan_edit_pcp_profile_info_set_verify(
          unit,
          pcp_profile_ndx,
          key_mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_profile_info_set_unsafe(
          unit,
          pcp_profile_ndx,
          key_mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pcp_profile_info_set()", pcp_profile_ndx, 0);
}


uint32
  soc_ppd_eg_vlan_edit_pcp_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY        *key_mapping
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(key_mapping);

  res = arad_pp_eg_vlan_edit_pcp_profile_info_get_verify(
          unit,
          pcp_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_profile_info_get_unsafe(
          unit,
          pcp_profile_ndx,
          key_mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pcp_profile_info_get()", pcp_profile_ndx, 0);
}


uint32
  soc_ppd_eg_vlan_edit_pcp_map_stag_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_vlan_edit_pcp_map_stag_set_verify(
          unit,
          pcp_profile_ndx,
          tag_pcp_ndx,
          tag_dei_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_map_stag_set_unsafe(
          unit,
          pcp_profile_ndx,
          tag_pcp_ndx,
          tag_dei_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pcp_map_stag_set()", pcp_profile_ndx, tag_pcp_ndx);
}


uint32
  soc_ppd_eg_vlan_edit_pcp_map_stag_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_eg_vlan_edit_pcp_map_stag_get_verify(
          unit,
          pcp_profile_ndx,
          tag_pcp_ndx,
          tag_dei_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_map_stag_get_unsafe(
          unit,
          pcp_profile_ndx,
          tag_pcp_ndx,
          tag_dei_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pcp_map_stag_get()", pcp_profile_ndx, tag_pcp_ndx);
}


uint32
  soc_ppd_eg_vlan_edit_pcp_map_ctag_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_vlan_edit_pcp_map_ctag_set_verify(
          unit,
          pcp_profile_ndx,
          tag_up_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_map_ctag_set_unsafe(
          unit,
          pcp_profile_ndx,
          tag_up_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pcp_map_ctag_set()", pcp_profile_ndx, tag_up_ndx);
}


uint32
  soc_ppd_eg_vlan_edit_pcp_map_ctag_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_eg_vlan_edit_pcp_map_ctag_get_verify(
          unit,
          pcp_profile_ndx,
          tag_up_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_map_ctag_get_unsafe(
          unit,
          pcp_profile_ndx,
          tag_up_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pcp_map_ctag_get()", pcp_profile_ndx, tag_up_ndx);
}


uint32
  soc_ppd_eg_vlan_edit_pcp_map_untagged_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_vlan_edit_pcp_map_untagged_set_verify(
          unit,
          pcp_profile_ndx,
          tc_ndx,
          dp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_map_untagged_set_unsafe(
          unit,
          pcp_profile_ndx,
          tc_ndx,
          dp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pcp_map_untagged_set()", pcp_profile_ndx, tc_ndx);
}


uint32
  soc_ppd_eg_vlan_edit_pcp_map_untagged_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_eg_vlan_edit_pcp_map_untagged_get_verify(
          unit,
          pcp_profile_ndx,
          tc_ndx,
          dp_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_map_untagged_get_unsafe(
          unit,
          pcp_profile_ndx,
          tc_ndx,
          dp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pcp_map_untagged_get()", pcp_profile_ndx, tc_ndx);
}


uint32 
  soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get( 
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                     pkt_type_ndx, 
    SOC_SAND_IN uint8                                    dscp_exp_ndx, 
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp, 
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei 
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_eg_vlan_edit_pcp_map_dscp_exp_get_verify(
          unit,
          pkt_type_ndx,
          dscp_exp_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_map_dscp_exp_get_unsafe(
          unit,
          pkt_type_ndx,
          dscp_exp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get()", pkt_type_ndx, dscp_exp_ndx);
}

uint32 
  soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set( 
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                     pkt_type_ndx, 
    SOC_SAND_IN uint8                                    dscp_exp_ndx, 
    SOC_SAND_IN SOC_SAND_PP_PCP_UP                             out_pcp, 
    SOC_SAND_IN SOC_SAND_PP_DEI_CFI                            out_dei 
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_vlan_edit_pcp_map_dscp_exp_set_verify(
          unit,
          pkt_type_ndx,
          dscp_exp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_vlan_edit_pcp_map_dscp_exp_set_unsafe(
        unit,
          pkt_type_ndx,
          dscp_exp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_vlan_edit_pcp_map_dscp_exp_set_verify()", pkt_type_ndx, dscp_exp_ndx);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>



