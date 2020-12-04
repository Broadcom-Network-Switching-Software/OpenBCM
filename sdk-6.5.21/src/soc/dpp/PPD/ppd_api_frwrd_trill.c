
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_trill.h>






















uint32
  soc_ppd_frwrd_trill_multicast_key_mask_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  res = arad_pp_frwrd_trill_multicast_key_mask_set_verify(
          unit,
          masked_fields
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_multicast_key_mask_set_unsafe(
          unit,
          masked_fields
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_key_mask_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_trill_multicast_key_mask_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  res = arad_pp_frwrd_trill_multicast_key_mask_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_multicast_key_mask_get_unsafe(
          unit,
          masked_fields
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_key_mask_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_trill_unicast_route_add(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       nickname_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *learn_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_trill_unicast_route_add_verify(
          unit,
          nickname_key,
          learn_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_unicast_route_add_unsafe(
          unit,
          nickname_key,
          learn_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_unicast_route_add()", nickname_key, 0);
}


uint32
  soc_ppd_frwrd_trill_unicast_route_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *learn_info,
    SOC_SAND_OUT uint8                      *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(learn_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  res = arad_pp_frwrd_trill_unicast_route_get_verify(
          unit,
          nickname_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_unicast_route_get_unsafe(
          unit,
          nickname_key,
          learn_info,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_unicast_route_get()", nickname_key, 0);
}


uint32
  soc_ppd_frwrd_trill_unicast_route_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_trill_unicast_route_remove_verify(
          unit,
          nickname_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_unicast_route_remove_unsafe(
          unit,
          nickname_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_unicast_route_remove()", nickname_key, 0);
}



uint32
  soc_ppd_frwrd_trill_ingress_lif_add(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID               lif_index,
    SOC_SAND_IN  uint32                       nickname_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO   *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_trill_ingress_lif_add_verify(
          unit,
          lif_index,
          nickname_key,
          trill_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_ingress_lif_add_unsafe(
          unit,
          lif_index,
          nickname_key,
          trill_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_ingress_lif_add()", nickname_key, 0);
}


uint32
  soc_ppd_frwrd_trill_ingress_lif_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID             *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_TRILL_INFO  *trill_info,
    SOC_SAND_OUT uint8                      *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(trill_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  res = arad_pp_frwrd_trill_ingress_lif_get_verify(
          unit,
          nickname_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_ingress_lif_get_unsafe(
          unit,
          nickname_key,
          lif_index,
          trill_info,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_ingress_lif_get()", nickname_key, 0);
}


uint32
  soc_ppd_frwrd_trill_ingress_lif_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                  *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_trill_ingress_lif_remove_verify(
          unit,
          nickname_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_ingress_lif_remove_unsafe(
          unit,
          nickname_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_ingress_lif_remove()", nickname_key, 0);
}


uint32
  soc_ppd_frwrd_trill_multicast_route_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                      mc_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_trill_multicast_route_add_verify(
          unit,
          trill_mc_key,
          mc_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_multicast_route_add_unsafe(
          unit,
          trill_mc_key,
          mc_id,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_route_add()", 0, 0);
}


uint32
  soc_ppd_frwrd_trill_multicast_route_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_OUT uint32                      *mc_id,
    SOC_SAND_OUT uint8                     *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(mc_id);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  *mc_id = 0;

  res = arad_pp_frwrd_trill_multicast_route_get_verify(
          unit,
          trill_mc_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_multicast_route_get_unsafe(
          unit,
          trill_mc_key,
          mc_id,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_route_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_trill_multicast_route_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);

  res = arad_pp_frwrd_trill_multicast_route_remove_verify(
          unit,
          trill_mc_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_multicast_route_remove_unsafe(
          unit,
          trill_mc_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_multicast_route_remove()", 0, 0);
}


uint32
  soc_ppd_frwrd_trill_adj_info_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                     enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_trill_adj_info_set_verify(
          unit,
          mac_address_key,
          mac_auth_info,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_adj_info_set_unsafe(
          unit,
          mac_address_key,
          mac_auth_info,
          enable,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_adj_info_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_trill_adj_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_OUT SOC_PPC_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_OUT uint8                     *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = arad_pp_frwrd_trill_adj_info_get_verify(
          unit,
          mac_address_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_adj_info_get_unsafe(
          unit,
          mac_address_key,
          mac_auth_info,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_adj_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_trill_global_info_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_frwrd_trill_global_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_global_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_global_info_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_trill_global_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_frwrd_trill_global_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_global_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_global_info_get()", 0, 0);
}



uint32
  soc_ppd_frwrd_trill_vsi_entry_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsi,
    SOC_SAND_IN  uint32                     flags,
    SOC_SAND_IN  uint32                     high_vid,
    SOC_SAND_IN  uint32                     low_vid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_VSI_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_trill_vsi_entry_add_verify(
          unit,
          vsi,
          high_vid,
          low_vid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_vsi_entry_add_unsafe(
          unit,
          vsi,
          flags,
          high_vid,
          low_vid,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_vsi_entry_add()", vsi, 0);
}

uint32
  soc_ppd_frwrd_trill_vsi_entry_remove(
     SOC_SAND_IN  int                     unit,
     SOC_SAND_IN  uint32                     vsi,
     SOC_SAND_IN  uint32                     flags,
     SOC_SAND_IN  uint32                     high_vid,
     SOC_SAND_IN  uint32                     low_vid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_VSI_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_trill_vsi_entry_remove_verify(
            unit,
            vsi,
            high_vid,
            low_vid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_vsi_entry_remove_unsafe(
          unit,
          vsi,
          flags,
          high_vid,
          low_vid
        );

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_vsi_entry_remove()", vsi, 0);
}

uint32
  soc_ppd_frwrd_trill_native_inner_tpid_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     tpid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_NATIVE_INNER_TPID_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_trill_native_inner_tpid_add_verify(
          unit,
          tpid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_trill_native_inner_tpid_add_unsafe(
          unit,
          tpid,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_trill_native_inner_tpid_add()", tpid, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>



