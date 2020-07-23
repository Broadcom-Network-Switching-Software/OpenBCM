/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_VSI



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>























uint32
  soc_ppd_rif_mpls_labels_range_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_MPLS_LABELS_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  res = arad_pp_rif_mpls_labels_range_set_verify(
          unit,
          rif_labels_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_labels_range_set_unsafe(
          unit,
          rif_labels_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_labels_range_set()", 0, 0);
}


uint32
  soc_ppd_rif_mpls_labels_range_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_MPLS_LABELS_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  res = arad_pp_rif_mpls_labels_range_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_labels_range_get_unsafe(
          unit,
          rif_labels_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_labels_range_get()", 0, 0);
}


uint32
  soc_ppd_rif_mpls_label_map_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_rif_mpls_label_map_add_verify(
          unit,
          mpls_key,
          lif_index,
          term_info,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_label_map_add_unsafe(
          unit,
          mpls_key,
          lif_index,
          term_info,
          rif_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_label_map_add()", 0, 0);
}


uint32
  soc_ppd_rif_mpls_label_map_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_INOUT SOC_PPC_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  res = arad_pp_rif_mpls_label_map_remove_verify(
          unit,
          mpls_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_label_map_remove_unsafe(
          unit,
          mpls_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_label_map_remove()", 0, 0);
}


uint32
  soc_ppd_rif_mpls_label_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_rif_mpls_label_map_get_verify(
          unit,
          mpls_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_label_map_get_unsafe(
          unit,
          mpls_key,
          lif_index,
          term_info,
          rif_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_label_map_get()", 0, 0);
}


uint32
  soc_ppd_rif_ip_tunnel_term_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 flags,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_rif_ip_tunnel_term_add_verify(
          unit,
          flags, 
          term_key,
          lif_index,
          term_info,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ip_tunnel_term_add_unsafe(
          unit,
          flags, 
          term_key,
          lif_index,
          term_info,
          rif_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ip_tunnel_term_add()", lif_index, 0);
}


uint32
  soc_ppd_rif_ip_tunnel_term_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  res = arad_pp_rif_ip_tunnel_term_remove_verify(
          unit,
          term_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ip_tunnel_term_remove_unsafe(
          unit,
          term_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ip_tunnel_term_remove()", 0, 0);
}


uint32
  soc_ppd_rif_ip_tunnel_term_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_rif_ip_tunnel_term_get_verify(
          unit,
          term_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ip_tunnel_term_get_unsafe(
          unit,
          term_key,
          lif_index,
          term_info,
          rif_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ip_tunnel_term_get()", 0, 0);
}


uint32
  soc_ppd_rif_vsid_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_VSID_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res = arad_pp_rif_vsid_map_set_verify(
          unit,
          vsid_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_vsid_map_set_unsafe(
          unit,
          vsid_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_vsid_map_set()", vsid_ndx, 0);
}


uint32
  soc_ppd_rif_vsid_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_VSID_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res = arad_pp_rif_vsid_map_get_verify(
          unit,
          vsid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_vsid_map_get_unsafe(
          unit,
          vsid_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_vsid_map_get()", vsid_ndx, 0);
}


uint32
  soc_ppd_rif_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res = arad_pp_rif_info_set_verify(
          unit,
          rif_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_info_set_unsafe(
          unit,
          rif_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_info_set()", rif_ndx, 0);
}


uint32
  soc_ppd_rif_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID                              rif_ndx,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_RIF_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res = arad_pp_rif_info_get_verify(
          unit,
          rif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_info_get_unsafe(
          unit,
          rif_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_info_get()", rif_ndx, 0);
}


uint32
  soc_ppd_rif_ttl_scope_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_TTL_SCOPE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_rif_ttl_scope_set_verify(
          unit,
          ttl_scope_ndx,
          ttl_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ttl_scope_set_unsafe(
          unit,
          ttl_scope_ndx,
          ttl_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ttl_scope_set()", ttl_scope_ndx, 0);
}


uint32
  soc_ppd_rif_ttl_scope_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_TTL_SCOPE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ttl_val);

  res = arad_pp_rif_ttl_scope_get_verify(
          unit,
          ttl_scope_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ttl_scope_get_unsafe(
          unit,
          ttl_scope_ndx,
          ttl_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ttl_scope_get()", ttl_scope_ndx, 0);
}



uint32
soc_ppd_rif_native_routing_vlan_tags_set(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  native_routing_vlan_tags
   ) { 
    uint32
       res = SOC_SAND_OK; 
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
    
    res = arad_pp_rif_native_routing_vlan_tags_set_verify(
       unit, 
       intf_id, 
       native_routing_vlan_tags
       ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
    
    SOC_SAND_TAKE_DEVICE_SEMAPHORE; 
    
    res = arad_pp_rif_native_routing_vlan_tags_set_unsafe(
       unit, 
       intf_id, 
       native_routing_vlan_tags
       ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore); 
    
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE; 
    ARAD_PP_DO_NOTHING_AND_EXIT; 
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_native_routing_vlan_tags_set()", intf_id, 0);
}




uint32
soc_ppd_rif_native_routing_vlan_tags_get(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  SOC_PPC_RIF_ID                      intf_id,
   SOC_SAND_OUT  uint8                              *native_routing_vlan_tags
   ) { 
    uint32
       res = SOC_SAND_OK; 
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
    
    res = arad_pp_rif_native_routing_vlan_tags_get_verify(
       unit, 
       intf_id, 
       native_routing_vlan_tags
       ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
    
    SOC_SAND_TAKE_DEVICE_SEMAPHORE; 
    
    res = arad_pp_rif_native_routing_vlan_tags_get_unsafe(
       unit, 
       intf_id, 
       native_routing_vlan_tags
       ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore); 
    
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE; 
    ARAD_PP_DO_NOTHING_AND_EXIT; 
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_native_routing_vlan_tags_get()", intf_id, 0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>


