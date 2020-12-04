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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv6.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>






















uint32
  soc_ppd_frwrd_ipv6_uc_route_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY        *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_UC_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv6_uc_route_add_verify(
          unit,
          route_key,
          route_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_uc_route_add_unsafe(
          unit,
          route_key,
          route_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_uc_route_add()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_uc_route_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY        *route_key,
    SOC_SAND_IN  uint8                                  get_flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO            *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS          *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION        *location,
    SOC_SAND_OUT uint8                                  *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_ipv6_uc_route_get_verify(
          unit,
          route_key,
          get_flags
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_uc_route_get_unsafe(
          unit,
          route_key,
          get_flags,
          route_info,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_uc_route_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_uc_route_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE       *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY        *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO            *route_infos,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS          *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION        *routes_location,
    SOC_SAND_OUT uint32                                 *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(route_infos);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(routes_location);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_ipv6_uc_route_get_block_verify(
          unit,
          block_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_uc_route_get_block_unsafe(
          unit,
          block_range_key,
          route_keys,
          route_infos,
          routes_status,
          routes_location,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_uc_route_get_block()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_uc_route_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY        *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_UC_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv6_uc_route_remove_verify(
          unit,
          route_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_uc_route_remove_unsafe(
          unit,
          route_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_uc_route_remove()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_uc_routing_table_clear(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipv6_uc_routing_table_clear_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_uc_routing_table_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_uc_routing_table_clear()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_host_add(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                   vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *host_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  SOC_DPP_DBAL_SW_TABLE_IDS
    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM;
  SOC_PPC_FP_QUAL_VAL
    qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
  uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  if (host_key->route_scale) {
      table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM;
  }

  arad_pp_frwrd_ipv4_host_lem_payload_build(unit,routing_info,&payload);

  arad_pp_frwrd_ipv6_prefix_to_mask(SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS, mask);

  DBAL_QUAL_VALS_CLEAR(qual_vals);
  DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], vrf_ndx, SOC_SAND_U32_MAX);
  DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[1], host_key->subnet.ipv6_address.address, mask);
  DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[2], host_key->subnet.ipv6_address.address, mask);

  res = arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0,  &payload, success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_host_add()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_host_get(
    SOC_SAND_IN   int                                      unit,
    SOC_SAND_IN   uint32                                   vrf_ndx,
    SOC_SAND_IN   SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *host_key,
    SOC_SAND_OUT  SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT  uint8                                   *found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  SOC_DPP_DBAL_SW_TABLE_IDS
    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM;
  SOC_PPC_FP_QUAL_VAL
    qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
  uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  *found = FALSE;

  if (host_key->route_scale) {
      table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM;
  }

  arad_pp_frwrd_ipv6_prefix_to_mask(SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS, mask);

  DBAL_QUAL_VALS_CLEAR(qual_vals);
  DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], vrf_ndx, SOC_SAND_U32_MAX);
  DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[1], host_key->subnet.ipv6_address.address, mask);
  DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[2], host_key->subnet.ipv6_address.address, mask);

  res = arad_pp_dbal_entry_get(unit, table_id, qual_vals, &payload, 0, 0, found);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  
  *route_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
  if (payload.flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED) {
    *route_status |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
  }

  if (*found)
  {
      res = arad_pp_frwrd_em_dest_to_fec(
              unit,
              &payload,
              routing_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit_semaphore);

      
      ARAD_PP_LEM_ACCESS_HI_FROM_EEI(payload.asd, routing_info->mac_lsb);

      
      routing_info->native_vsi = payload.native_vsi;
  }

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_host_add()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_host_delete(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                   vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *host_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_DPP_DBAL_SW_TABLE_IDS
    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM;
  SOC_PPC_FP_QUAL_VAL
    qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
  uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  if (host_key->route_scale) {
      table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM;
  }

  arad_pp_frwrd_ipv6_prefix_to_mask(SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS, mask);

  DBAL_QUAL_VALS_CLEAR(qual_vals);
  DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], vrf_ndx, SOC_SAND_U32_MAX);
  DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[1], host_key->subnet.ipv6_address.address, mask);
  DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[2], host_key->subnet.ipv6_address.address, mask);

  res = arad_pp_dbal_entry_delete(unit, table_id, qual_vals, success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_host_delete()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_mc_route_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY        *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO       *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_MC_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv6_mc_route_add_verify(
          unit,
          route_key,
          route_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_mc_route_add_unsafe(
          unit,
          route_key,
          route_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_mc_route_add()", 0, 0);
}



uint32
  soc_ppd_frwrd_ipv6_mc_compress_sip_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY        *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO       *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_MC_COMPRESS_SIP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv6_mc_route_add_verify(
          unit,
          route_key,
          route_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_mc_compress_sip_add_unsafe(
          unit,
          route_key,
          route_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_mc_compress_sip_add()", 0, 0);
}



uint32
  soc_ppd_frwrd_ipv6_mc_route_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY        *route_key,
    SOC_SAND_IN  uint8                                  get_flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO       *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS          *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION        *location,
    SOC_SAND_OUT uint8                                  *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_ipv6_mc_route_get_verify(
          unit,
          route_key,
          get_flags
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_mc_route_get_unsafe(
          unit,
          route_key,
          get_flags,
          route_info,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_mc_route_get()", 0, 0);
}



uint32
  soc_ppd_frwrd_ipv6_mc_compress_sip_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY        *route_key,
    SOC_SAND_IN  uint8                                  get_flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO       *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS          *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION        *location,
    SOC_SAND_OUT uint8                                  *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_MC_COMPRESS_SIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_ipv6_mc_route_get_verify(
          unit,
          route_key,
          get_flags
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_mc_compress_sip_get_unsafe(
          unit,
          route_key,
          get_flags,
          route_info,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_mc_compress_sip_get()", 0, 0);
}



uint32
  soc_ppd_frwrd_ipv6_mc_route_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE       *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY        *route_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO       *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS          *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION        *routes_location,
    SOC_SAND_OUT uint32                                 *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(routes_info);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(routes_location);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_ipv6_mc_route_get_block_verify(
          unit,
          block_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_mc_route_get_block_unsafe(
          unit,
          block_range_key,
          route_key,
          routes_info,
          routes_status,
          routes_location,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_mc_route_get_block()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_mc_route_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY        *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_MC_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv6_mc_route_remove_verify(
          unit,
          route_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_mc_route_remove_unsafe(
          unit,
          route_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_mc_route_remove()", 0, 0);
}



uint32
  soc_ppd_frwrd_ipv6_mc_compress_sip_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY        *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_MC_COMPRESS_SIP_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv6_mc_route_remove_verify(
          unit,
          route_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_mc_compress_sip_remove_unsafe(
          unit,
          route_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_mc_compress_sip_remove()", 0, 0);
}



uint32
  soc_ppd_frwrd_ipv6_mc_routing_table_clear(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipv6_mc_routing_table_clear_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_mc_routing_table_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_mc_routing_table_clear()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipv6_vrf_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                         vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VRF_INFO            *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV6_VRF_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  res = arad_pp_frwrd_ipv6_vrf_info_set_verify(
          unit,
          vrf_ndx,
          vrf_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_vrf_info_set_unsafe(
          unit,
          vrf_ndx,
          vrf_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_vrf_info_set()", vrf_ndx, 0);
}


uint32
  soc_ppd_frwrd_ipv6_vrf_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                         vrf_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_VRF_INFO            *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV6_VRF_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  res = arad_pp_frwrd_ipv6_vrf_info_get_verify(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_vrf_info_get_unsafe(
          unit,
          vrf_ndx,
          vrf_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_vrf_info_get()", vrf_ndx, 0);
}


uint32
  soc_ppd_frwrd_ipv6_vrf_route_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                         vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY       *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_VRF_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv6_vrf_route_add_verify(
          unit,
          vrf_ndx,
          route_key,
          route_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_vrf_route_add_unsafe(
          unit,
          vrf_ndx,
          route_key,
          route_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_vrf_route_add()", vrf_ndx, 0);
}


uint32
  soc_ppd_frwrd_ipv6_vrf_route_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                         vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY       *route_key,
    SOC_SAND_IN  uint8                                  exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO            *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS          *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION        *location,
    SOC_SAND_OUT uint8                                  *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_ipv6_vrf_route_get_verify(
          unit,
          vrf_ndx,
          route_key,
          exact_match
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_vrf_route_get_unsafe(
          unit,
          vrf_ndx,
          route_key,
          exact_match,
          route_info,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_vrf_route_get()", vrf_ndx, 0);
}


uint32
  soc_ppd_frwrd_ipv6_vrf_route_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                         vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE       *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY       *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO            *route_infos,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS          *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION        *routes_location,
    SOC_SAND_OUT uint32                                 *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(route_infos);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(routes_location);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_ipv6_vrf_route_get_block_verify(
          unit,
          vrf_ndx,
          block_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_vrf_route_get_block_unsafe(
          unit,
          vrf_ndx,
          block_range_key,
          route_keys,
          route_infos,
          routes_status,
          routes_location,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_vrf_route_get_block()", vrf_ndx, 0);
}


uint32
  soc_ppd_frwrd_ipv6_vrf_route_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                         vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY       *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv6_vrf_route_remove_verify(
          unit,
          vrf_ndx,
          route_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_vrf_route_remove_unsafe(
          unit,
          vrf_ndx,
          route_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_vrf_route_remove()", vrf_ndx, 0);
}


uint32
  soc_ppd_frwrd_ipv6_vrf_routing_table_clear(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                         vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipv6_vrf_routing_table_clear_verify(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_vrf_routing_table_clear_unsafe(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_vrf_routing_table_clear()", vrf_ndx, 0);
}


uint32
  soc_ppd_frwrd_ipv6_vrf_all_routing_tables_clear(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipv6_vrf_all_routing_tables_clear_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv6_vrf_all_routing_tables_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv6_vrf_all_routing_tables_clear()", 0, 0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>



