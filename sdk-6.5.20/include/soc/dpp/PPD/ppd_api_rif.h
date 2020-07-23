/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_RIF_INCLUDED__

#define __SOC_PPD_API_RIF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_rif.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_mpls_term.h>














typedef enum
{
  
  SOC_PPD_RIF_MPLS_LABELS_RANGE_SET = SOC_PPD_PROC_DESC_BASE_RIF_FIRST,
  SOC_PPD_RIF_MPLS_LABELS_RANGE_GET,
  SOC_PPD_RIF_MPLS_LABEL_MAP_ADD,
  SOC_PPD_RIF_MPLS_LABEL_MAP_REMOVE,
  SOC_PPD_RIF_MPLS_LABEL_MAP_GET,
  SOC_PPD_RIF_IP_TUNNEL_TERM_ADD,
  SOC_PPD_RIF_IP_TUNNEL_TERM_REMOVE,
  SOC_PPD_RIF_IP_TUNNEL_TERM_GET,
  SOC_PPD_RIF_VSID_MAP_SET,
  SOC_PPD_RIF_VSID_MAP_SET_PRINT,
  SOC_PPD_RIF_VSID_MAP_GET,
  SOC_PPD_RIF_VSID_MAP_GET_PRINT,
  SOC_PPD_RIF_INFO_SET,
  SOC_PPD_RIF_INFO_SET_PRINT,
  SOC_PPD_RIF_INFO_GET,
  SOC_PPD_RIF_INFO_GET_PRINT,
  SOC_PPD_RIF_TTL_SCOPE_SET,
  SOC_PPD_RIF_TTL_SCOPE_SET_PRINT,
  SOC_PPD_RIF_TTL_SCOPE_GET,
  SOC_PPD_RIF_TTL_SCOPE_GET_PRINT,
  SOC_PPD_RIF_GET_PROCS_PTR,
  



  
  SOC_PPD_RIF_PROCEDURE_DESC_LAST
} SOC_PPD_RIF_PROCEDURE_DESC;










uint32
  soc_ppd_rif_mpls_labels_range_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );


uint32
  soc_ppd_rif_mpls_labels_range_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );


uint32
  soc_ppd_rif_mpls_label_map_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_rif_mpls_label_map_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_INOUT SOC_PPC_LIF_ID                              *lif_index
  );


uint32
  soc_ppd_rif_mpls_label_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_rif_ip_tunnel_term_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );


uint32
  soc_ppd_rif_ip_tunnel_term_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  );


uint32
  soc_ppd_rif_ip_tunnel_term_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );


uint32
  soc_ppd_rif_vsid_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  );


uint32
  soc_ppd_rif_vsid_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info
  );


uint32
  soc_ppd_rif_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  );


uint32
  soc_ppd_rif_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID                              rif_ndx,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info
  );


uint32
  soc_ppd_rif_ttl_scope_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                          ttl_val
  );


uint32
  soc_ppd_rif_ttl_scope_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               ttl_scope_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                          *ttl_val
  );



uint32
soc_ppd_rif_native_routing_vlan_tags_set(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                                 intf_id, 
   SOC_SAND_IN  uint8                                  native_routing_vlan_tags
   );

uint32
soc_ppd_rif_native_routing_vlan_tags_get(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_OUT  uint8                                 *native_routing_vlan_tags
   );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

