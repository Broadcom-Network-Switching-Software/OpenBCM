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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>






















uint32
  soc_ppd_l2_lif_pwe_map_range_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE                    *in_vc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_MAP_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_vc_range);

  res = arad_pp_l2_lif_pwe_map_range_set_verify(
          unit,
          in_vc_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_pwe_map_range_set_unsafe(
          unit,
          in_vc_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_pwe_map_range_set()", 0, 0);
}


uint32
  soc_ppd_l2_lif_pwe_map_range_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_L2_LIF_IN_VC_RANGE                      *in_vc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_MAP_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_vc_range);

  res = arad_pp_l2_lif_pwe_map_range_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_pwe_map_range_get_unsafe(
          unit,
          in_vc_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_pwe_map_range_get()", 0, 0);
}


uint32
  soc_ppd_l2_lif_pwe_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                  in_vc_label,
    SOC_SAND_IN  SOC_PPC_LIF_ID                          lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO     *pwe_additional_info,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO                *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_pwe_add_verify(
          unit,
          in_vc_label,
          lif_index,
          pwe_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_pwe_add_unsafe(
          unit,
          in_vc_label,
          lif_index,
		  pwe_additional_info,
          pwe_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_pwe_add()", 0, 0);
}


uint32
  soc_ppd_l2_lif_pwe_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                  in_vc_label,
    SOC_SAND_OUT SOC_PPC_LIF_ID                          *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO      *pwe_additional_info,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_INFO                 *pwe_info,
    SOC_SAND_OUT uint8                                   *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_pwe_get_verify(
          unit,
          in_vc_label
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_pwe_get_unsafe(
          unit,
          in_vc_label,
          lif_index,
		  pwe_additional_info,
          pwe_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_pwe_get()", 0, 0);
}


uint32
  soc_ppd_l2_lif_pwe_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                  in_vc_label,
    SOC_SAND_IN  uint8                                   remove_from_sem,
    SOC_SAND_IN  uint8                                   is_gal,
    SOC_SAND_OUT uint32                                  *lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO      *pwe_additional_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  res = arad_pp_l2_lif_pwe_remove_verify(
          unit,
          in_vc_label,
          remove_from_sem
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_pwe_remove_unsafe(
          unit,
          in_vc_label,
          remove_from_sem,
          is_gal,
          lif_index,
		  pwe_additional_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_pwe_remove()", 0, 0);
}


uint32
  soc_ppd_l2_lif_vlan_compression_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                       *vlan_range_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_vlan_compression_add_verify(
          unit,
          vlan_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_vlan_compression_add_unsafe(
          unit,
          vlan_range_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_vlan_compression_add()", 0, 0);
}


uint32
  soc_ppd_l2_lif_vlan_compression_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                       *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_key);

  res = arad_pp_l2_lif_vlan_compression_remove_verify(
          unit,
          vlan_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_vlan_compression_remove_unsafe(
          unit,
          vlan_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_vlan_compression_remove()", 0, 0);
}


uint32
  soc_ppd_l2_lif_vlan_compression_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                               local_port_ndx,
    SOC_SAND_OUT SOC_PPC_L2_PORT_VLAN_RANGE_INFO            *vlan_range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_info);

  res = arad_pp_l2_lif_vlan_compression_get_verify(
          unit,
          local_port_ndx,
          vlan_range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_vlan_compression_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          vlan_range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_vlan_compression_get()", local_port_ndx, 0);
}


uint32
  soc_ppd_l2_lif_ac_map_key_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE                  key_mapping
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_MAP_KEY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_l2_lif_ac_map_key_set_verify(
          unit,
          qual_key,
          key_mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_ac_map_key_set_unsafe(
          unit,
          qual_key,
          key_mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_ac_map_key_set()", 0, 0);
}


uint32
  soc_ppd_l2_lif_ac_map_key_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE                  *key_mapping
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_MAP_KEY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(key_mapping);

  res = arad_pp_l2_lif_ac_map_key_get_verify(
          unit,
          qual_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_ac_map_key_get_unsafe(
          unit,
          qual_key,
          key_mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_ac_map_key_get()", 0, 0);
}


uint32
  soc_ppd_l2_lif_ac_mp_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_LIF_AC_MP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_pp_l2_lif_ac_mp_info_set_verify(
          unit,
          lif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_ac_mp_info_set_unsafe(
          unit,
          lif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_ac_mp_info_set()", lif_ndx, 0);
}


uint32
  soc_ppd_l2_lif_ac_mp_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_LIF_AC_MP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_pp_l2_lif_ac_mp_info_get_verify(
          unit,
          lif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_ac_mp_info_get_unsafe(
          unit,
          lif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_ac_mp_info_get()", lif_ndx, 0);
}


uint32
  soc_ppd_l2_lif_ac_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!((ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TEST2) && (SOC_DPP_CONFIG(unit)->pp.test2))) {
      res = arad_pp_l2_lif_ac_add_verify(
          unit,
          ac_key,
          lif_index,
          ac_info
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_ac_add_unsafe(
          unit,
          ac_key,
          lif_index,
          ac_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_ac_add()", 0, 0);
}


uint32
  soc_ppd_l2_lif_ac_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_ac_get_verify(
          unit,
          ac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_ac_get_unsafe(
          unit,
          ac_key,
          lif_index,
          ac_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_ac_get()", 0, 0);
}


uint32
  soc_ppd_l2_lif_ac_with_cos_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO                    *acs_group_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_WITH_COS_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(acs_group_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_ac_with_cos_add_verify(
          unit,
          ac_key,
          base_lif_index,
          acs_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_ac_with_cos_add_unsafe(
          unit,
          ac_key,
          base_lif_index,
          acs_group_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_ac_with_cos_add()", 0, 0);
}


uint32
  soc_ppd_l2_lif_ac_with_cos_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *base_lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_GROUP_INFO                    *acs_group_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_WITH_COS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(base_lif_index);
  SOC_SAND_CHECK_NULL_INPUT(acs_group_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_ac_with_cos_get_verify(
          unit,
          ac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_ac_with_cos_get_unsafe(
          unit,
          ac_key,
          base_lif_index,
          acs_group_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_ac_with_cos_get()", 0, 0);
}


uint32
  soc_ppd_l2_lif_ac_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  
  res = arad_pp_l2_lif_ac_remove_verify(
          unit,
          ac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_ac_remove_unsafe(
          unit,
          ac_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_ac_remove()", 0, 0);
}


uint32
  soc_ppd_l2_lif_l2cp_trap_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE                 handle_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_L2CP_TRAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  res = arad_pp_l2_lif_l2cp_trap_set_verify(
          unit,
          l2cp_key,
          handle_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_l2cp_trap_set_unsafe(
          unit,
          l2cp_key,
          handle_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_l2cp_trap_set()", 0, 0);
}


uint32
  soc_ppd_l2_lif_l2cp_trap_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE                 *handle_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_L2CP_TRAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_type);

  res = arad_pp_l2_lif_l2cp_trap_get_verify(
          unit,
          l2cp_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_l2cp_trap_get_unsafe(
          unit,
          l2cp_key,
          handle_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_l2cp_trap_get()", 0, 0);
}


uint32
  soc_ppd_l2_lif_isid_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO                        *isid_info,
    SOC_SAND_IN  uint8                                       is_ingress,
    SOC_SAND_IN  uint8                                       is_egress,
    SOC_SAND_IN  uint8                                       isem_only,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_isid_add_verify(
          unit,
          vsi_ndx,
          isid_key,
          lif_index,
          isid_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_isid_add_unsafe(
          unit,
          vsi_ndx,
          isid_key,
          lif_index,
          isid_info,
          is_ingress,
          is_egress,
          isem_only,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_isid_add()", vsi_ndx, 0);
}


uint32
  soc_ppd_l2_lif_isid_get(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_SYS_VSI_ID                          *vsi_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_OUT uint8                                   *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(vsi_index);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_isid_get_verify(
          unit,
          isid_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_isid_get_unsafe(
          unit,
          isid_key,
          lif_index,
          vsi_index,
          isid_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_isid_get()", 0, 0);
}


uint32
  soc_ppd_l2_lif_vsi_to_isid(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_ISID                      *isid_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VSI_TO_ISID);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(isid_id);

  res = arad_pp_l2_lif_vsi_to_isid_verify(
          unit,
          vsi_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_vsi_to_isid_unsafe(
          unit,
          vsi_ndx,
          isid_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_vsi_to_isid()", 0, 0);
}


uint32
  soc_ppd_l2_lif_isid_remove(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  uint8                                       is_ingress,
    SOC_SAND_IN  uint8                                       is_egress,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi,
    SOC_SAND_IN  uint8                                       isem_only,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(isid_key);

  res = arad_pp_l2_lif_isid_remove_verify(
          unit,
          isid_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_isid_remove_unsafe(
          unit,
          isid_key,
          is_ingress,
          is_egress,
          vsi,
          isem_only,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_isid_remove()", 0, 0);
}


uint32
  soc_ppd_l2_lif_gre_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                         *gre_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO                        *gre_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(gre_key);
  SOC_SAND_CHECK_NULL_INPUT(gre_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_gre_add_verify(
          unit,
          gre_key,
          lif_index,
          gre_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_gre_add_unsafe(
          unit,
          gre_key,
          lif_index,
          gre_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_gre_add()", lif_index, 0);
}


uint32
  soc_ppd_l2_lif_gre_get(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_INFO                    *gre_info,
    SOC_SAND_OUT uint8                                   *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(gre_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(gre_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_gre_get_verify(
          unit,
          gre_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_gre_get_unsafe(
          unit,
          gre_key,
          lif_index,
          gre_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_gre_get()", 0, 0);
}


uint32
  soc_ppd_l2_lif_gre_remove(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(gre_key);

  res = arad_pp_l2_lif_gre_remove_verify(
          unit,
          gre_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_gre_remove_unsafe(
          unit,
          gre_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_gre_remove()", 0, 0);
}


uint32
  soc_ppd_l2_lif_vxlan_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                         *vxlan_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO                        *vxlan_info,
    SOC_SAND_IN  int                                         ignore_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vxlan_key);
  SOC_SAND_CHECK_NULL_INPUT(vxlan_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_vxlan_add_verify(
          unit,
          vxlan_key,
          lif_index,
          vxlan_info,
          ignore_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_vxlan_add_unsafe(
          unit,
          vxlan_key,
          lif_index,
          vxlan_info,
          ignore_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_vxlan_add()", lif_index, 0);
}


uint32
  soc_ppd_l2_lif_vxlan_get(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_OUT uint8                                   *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vxlan_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(vxlan_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_vxlan_get_verify(
          unit,
          vxlan_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_vxlan_get_unsafe(
          unit,
          vxlan_key,
          lif_index,
          vxlan_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_vxlan_get()", 0, 0);
}


uint32
  soc_ppd_l2_lif_vxlan_remove(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vxlan_key);

  res = arad_pp_l2_lif_vxlan_remove_verify(
          unit,
          vxlan_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_vxlan_remove_unsafe(
          unit,
          vxlan_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_vxlan_remove()", 0, 0);
}


uint32 soc_ppd_l2_lif_extender_add(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY        *extender_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                     lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO       *extender_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success)
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_CHECK_NULL_INPUT(extender_key);
    SOC_SAND_CHECK_NULL_INPUT(extender_info);
    SOC_SAND_CHECK_NULL_INPUT(success);

    res = arad_pp_l2_lif_extender_add_verify(unit, extender_key, lif_index, extender_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_l2_lif_extender_add_unsafe(unit, extender_key, lif_index, extender_info, success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_extender_add()", 0, 0);
}




uint32
  soc_ppd_l2_lif_default_frwrd_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dflt_frwrd_key);
  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_l2_lif_default_frwrd_info_set_verify(
          unit,
          dflt_frwrd_key,
          action_info   
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_default_frwrd_info_set_unsafe(
          unit,
          dflt_frwrd_key,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_default_frwrd_info_set()", 0, 0);
}


uint32
  soc_ppd_l2_lif_default_frwrd_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dflt_frwrd_key);
  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = arad_pp_l2_lif_default_frwrd_info_get_verify(
          unit,
          dflt_frwrd_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_l2_lif_default_frwrd_info_get_unsafe(
          unit,
          dflt_frwrd_key,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_l2_lif_default_frwrd_info_get()", 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>



