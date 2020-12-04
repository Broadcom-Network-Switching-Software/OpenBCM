/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_METERING
#include <shared/bsl.h>




#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>





















uint32
  soc_ppd_mtr_meters_group_info_set(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int									 core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO                  *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  res = arad_pp_mtr_meters_group_info_set_verify(
          unit,
		  core_id,
          mtr_group_ndx,
          mtr_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_meters_group_info_set_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          mtr_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_meters_group_info_set()", mtr_group_ndx, 0);
}


uint32
  soc_ppd_mtr_meters_group_info_get(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_GROUP_INFO                  *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  res = arad_pp_mtr_meters_group_info_get_verify(
	      core_id,
          unit,
          mtr_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_meters_group_info_get_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          mtr_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_meters_group_info_get()", mtr_group_ndx, 0);
}


uint32
  soc_ppd_mtr_bw_profile_add(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO             *bw_profile_info,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO             *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_mtr_bw_profile_add_verify(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx,
          bw_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_bw_profile_add_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx,
          bw_profile_info,
          exact_bw_profile_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_bw_profile_add()", mtr_group_ndx, bw_profile_ndx);
}


uint32
  soc_ppd_mtr_bw_profile_get(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO             *bw_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);

  res = arad_pp_mtr_bw_profile_get_verify(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_bw_profile_get_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx,
          bw_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_bw_profile_get()", mtr_group_ndx, bw_profile_ndx);
}


uint32
  soc_ppd_mtr_bw_profile_remove(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_mtr_bw_profile_remove_verify(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_bw_profile_remove_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_bw_profile_remove()", mtr_group_ndx, bw_profile_ndx);
}


uint32
  soc_ppd_mtr_meter_ins_to_bw_profile_map_set(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);

  res = arad_pp_mtr_meter_ins_to_bw_profile_map_set_verify(
          unit,
		  core_id,
          meter_ins_ndx,
          bw_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe(
          unit,
		  core_id,
          meter_ins_ndx,
          bw_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_meter_ins_to_bw_profile_map_set()", 0, 0);
}


uint32
  soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int									 core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_OUT uint32                                  *bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);
  SOC_SAND_CHECK_NULL_INPUT(bw_profile_id);

  res = arad_pp_mtr_meter_ins_to_bw_profile_map_get_verify(
          unit,
		  core_id,
          meter_ins_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe(
          unit,
		  core_id,
          meter_ins_ndx,
          bw_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_meter_ins_to_bw_profile_map_get()", 0, 0);
}


uint32
  soc_ppd_mtr_eth_policer_enable_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_mtr_eth_policer_enable_set_verify(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_enable_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_enable_set()", 0, 0);
}


uint32
  soc_ppd_mtr_eth_policer_enable_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = arad_pp_mtr_eth_policer_enable_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_enable_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_enable_get()", 0, 0);
}


uint32
  soc_ppd_mtr_eth_policer_params_set(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  res = arad_pp_mtr_eth_policer_params_set_verify(
          unit,
          port_ndx,
          eth_type_ndx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_params_set_unsafe(
          unit,
          port_ndx,
          eth_type_ndx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_params_set()", port_ndx, 0);
}


uint32
  soc_ppd_mtr_eth_policer_params_get(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  res = arad_pp_mtr_eth_policer_params_get_verify(
          unit,
          port_ndx,
          eth_type_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_params_get_unsafe(
          unit,
          port_ndx,
          eth_type_ndx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_params_get()", port_ndx, 0);
}


uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  res = arad_pp_mtr_eth_policer_glbl_profile_set_verify(
          unit,
          glbl_profile_idx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_glbl_profile_set_unsafe(
          unit,
          glbl_profile_idx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_glbl_profile_set()", 0, 0);
}


uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  res = arad_pp_mtr_eth_policer_glbl_profile_get_verify(
          unit,
          glbl_profile_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_glbl_profile_get_unsafe(
          unit,
          glbl_profile_idx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_glbl_profile_get()", 0, 0);
}


uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_mtr_eth_policer_glbl_profile_map_set_verify(
          unit,
          port_ndx,
          eth_type_ndx,
          glbl_profile_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe(
          unit,
          port_ndx,
          eth_type_ndx,
          glbl_profile_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_glbl_profile_map_set()", 0, 0);
}


uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_OUT uint32                  *glbl_profile_idx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_profile_idx);

  res = arad_pp_mtr_eth_policer_glbl_profile_map_get_verify(
          unit,
          port_ndx,
          eth_type_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_glbl_profile_map_get_unsafe(
          unit,
          port_ndx,
          eth_type_ndx,
          glbl_profile_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_glbl_profile_map_get()", 0, 0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>


