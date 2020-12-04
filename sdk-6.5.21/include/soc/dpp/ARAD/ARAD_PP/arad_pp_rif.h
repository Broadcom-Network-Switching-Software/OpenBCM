/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_RIF_INCLUDED__

#define __ARAD_PP_RIF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>




#define ARAD_PP_RIF_COS_PROFILE_MAX              63
#define ARAD_PP_RIF_DEFAULT_FORWARD_PROFILE_MAX  3
#define ARAD_RIF_VSI_ASSIGNMENT_MODE_MAX         3
#define ARAD_RIF_PROTECTION_POINTER_MAX          ((1<<14)-1)
#define ARAD_RIF_TRAP_CODE_INDEX_MAX             7











typedef enum
{
  
  SOC_PPC_RIF_MPLS_LABELS_RANGE_SET = ARAD_PP_PROC_DESC_BASE_RIF_FIRST,
  SOC_PPC_RIF_MPLS_LABELS_RANGE_SET_PRINT,
  SOC_PPC_RIF_MPLS_LABELS_RANGE_SET_UNSAFE,
  SOC_PPC_RIF_MPLS_LABELS_RANGE_SET_VERIFY,
  SOC_PPC_RIF_MPLS_LABELS_RANGE_GET,
  SOC_PPC_RIF_MPLS_LABELS_RANGE_GET_PRINT,
  SOC_PPC_RIF_MPLS_LABELS_RANGE_GET_VERIFY,
  SOC_PPC_RIF_MPLS_LABELS_RANGE_GET_UNSAFE,
  ARAD_PP_RIF_MPLS_LABEL_MAP_ADD,
  ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_PRINT,
  ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_UNSAFE,
  ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_INTERNAL_UNSAFE,
  ARAD_PP_RIF_MPLS_LABEL_MAP_ADD_VERIFY,
  ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE,
  ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE_PRINT,
  ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE_UNSAFE,
  ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE_VERIFY,
  ARAD_PP_RIF_MPLS_LABEL_MAP_GET,
  ARAD_PP_RIF_MPLS_LABEL_MAP_GET_PRINT,
  ARAD_PP_RIF_MPLS_LABEL_MAP_GET_UNSAFE,
  ARAD_PP_RIF_MPLS_LABEL_MAP_GET_INTERNAL_UNSAFE,
  ARAD_PP_RIF_MPLS_LABEL_MAP_GET_VERIFY,
  ARAD_PP_RIF_IP_TUNNEL_TERM_ADD,
  ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_PRINT,
  ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_UNSAFE,
  ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_INTERNAL_UNSAFE,
  ARAD_PP_RIF_IP_TUNNEL_TERM_ADD_VERIFY,
  ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE,
  ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE_PRINT,
  ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE_UNSAFE,
  ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE_VERIFY,
  ARAD_PP_RIF_IP_TUNNEL_TERM_GET,
  ARAD_PP_RIF_IP_TUNNEL_TERM_GET_PRINT,
  ARAD_PP_RIF_IP_TUNNEL_TERM_GET_UNSAFE,
  ARAD_PP_RIF_IP_TUNNEL_TERM_GET_INTERNAL_UNSAFE,
  ARAD_PP_RIF_IP_TUNNEL_TERM_GET_VERIFY,
  ARAD_PP_RIF_VSID_MAP_SET,
  ARAD_PP_RIF_VSID_MAP_SET_PRINT,
  ARAD_PP_RIF_VSID_MAP_SET_UNSAFE,
  ARAD_PP_RIF_VSID_MAP_SET_VERIFY,
  ARAD_PP_RIF_VSID_MAP_GET,
  ARAD_PP_RIF_VSID_MAP_GET_PRINT,
  ARAD_PP_RIF_VSID_MAP_GET_VERIFY,
  ARAD_PP_RIF_VSID_MAP_GET_UNSAFE,
  SOC_PPC_RIF_INFO_SET,
  SOC_PPC_RIF_INFO_SET_PRINT,
  SOC_PPC_RIF_INFO_SET_UNSAFE,
  SOC_PPC_RIF_INFO_SET_VERIFY,
  SOC_PPC_RIF_INFO_GET,
  SOC_PPC_RIF_INFO_GET_PRINT,
  SOC_PPC_RIF_INFO_GET_VERIFY,
  SOC_PPC_RIF_INFO_GET_UNSAFE,
  ARAD_PP_RIF_TTL_SCOPE_SET,
  ARAD_PP_RIF_TTL_SCOPE_SET_PRINT,
  ARAD_PP_RIF_TTL_SCOPE_SET_UNSAFE,
  ARAD_PP_RIF_TTL_SCOPE_SET_VERIFY,
  ARAD_PP_RIF_TTL_SCOPE_GET,
  ARAD_PP_RIF_TTL_SCOPE_GET_PRINT,
  ARAD_PP_RIF_TTL_SCOPE_GET_VERIFY,
  ARAD_PP_RIF_TTL_SCOPE_GET_UNSAFE,
  ARAD_PP_RIF_GET_PROCS_PTR,
  ARAD_PP_RIF_GET_ERRS_PTR,
  ARAD_PP_GET_IN_RIF_PROFILE_FROM_VRF_UNSAFE,
  ARAD_PP_SET_IN_RIF_PROFILE_TO_VRF_UNSAFE,
  

  
  ARAD_PP_RIF_PROCEDURE_DESC_LAST
} ARAD_PP_RIF_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_RIF_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_RIF_FIRST,
  ARAD_PP_RIF_DIP_KEY_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_FIRST_LABEL_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_LAST_LABEL_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_ROUTING_ENABLERS_BM_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_MPLS_LABEL_INVALID_RANGE_ERR,
  ARAD_PP_RIF_VRF_NOT_FOUND_ERR,
  ARAD_PP_RIF_LIF_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_TPID_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_RIF_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_RIF_SERVICE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_RIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR,
  ARAD_RIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR,
  ARAD_RIF_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR,
  ARAD_RIF_MPLS_TERM_NEXT_PRTCL_OUT_OF_RANGE_ERR,
  ARAD_RIF_TRAP_CODE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_RIF_LEARN_RECORD_OUT_OF_RANGE_ERR,
  ARAD_RIF_FORWARADING_CODE_OUT_OF_RANGE_ERR,

  
  ARAD_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR,
  ARAD_PP_RIF_IP_TUNNEL_TERM_KEY_ILLEGAL_ERR,



  
  ARAD_PP_RIF_ERR_LAST
} ARAD_PP_RIF_ERR;

typedef struct
{
  
  PARSER_HINT_ARR uint8 *rif_urpf_mode;
} ARAD_PP_RIF_TO_LIF_GROUP_MAP;










uint32
  arad_pp_rif_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_rif_mpls_labels_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );

uint32
  arad_pp_rif_mpls_labels_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );

uint32
  arad_pp_rif_mpls_labels_range_get_verify(
    SOC_SAND_IN  int                                 unit
  );

soc_error_t
 arad_pp_rif_global_urpf_mode_set(int unit, int index, int urpf_mode);

uint32
  arad_pp_rif_mpls_labels_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );


uint32
  arad_pp_rif_mpls_label_map_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_rif_mpls_label_map_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_rif_mpls_label_map_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  );


uint32
  arad_pp_rif_mpls_label_map_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_INOUT SOC_PPC_LIF_ID                              *lif_index
  );

uint32
  arad_pp_rif_mpls_label_map_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key
  );


uint32
  arad_pp_rif_mpls_label_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_rif_mpls_label_map_get_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_OUT  SOC_PPC_LIF_ID                             *lif_index,
    SOC_SAND_OUT  SOC_PPC_MPLS_TERM_INFO                     *term_info,
    SOC_SAND_OUT  SOC_PPC_RIF_INFO                           *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_rif_mpls_label_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY                  *mpls_key
  );


uint32
  arad_pp_rif_ip_tunnel_term_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                    flags,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY                    *term_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_rif_ip_tunnel_term_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY                *term_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_rif_ip_tunnel_term_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                    flags,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY                    *term_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  );


uint32
  arad_pp_rif_ip_tunnel_term_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY                    *term_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  );

uint32
  arad_pp_rif_ip_tunnel_term_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY                 *term_key
  );


uint32
  arad_pp_rif_ip_tunnel_term_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY                *term_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );


uint32
  arad_pp_rif_ip_tunnel_term_get_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY                *term_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_OUT  SOC_PPC_LIF_ID                             *lif_index,
    SOC_SAND_OUT  SOC_PPC_RIF_IP_TERM_INFO                   *term_info,
    SOC_SAND_OUT  SOC_PPC_RIF_INFO                           *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );


uint32
  arad_pp_rif_ip_tunnel_term_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY                *term_key
  );


uint32
  arad_pp_rif_vsid_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  );

uint32
  arad_pp_rif_vsid_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  );

uint32
  arad_pp_rif_vsid_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx
  );


uint32
  arad_pp_rif_vsid_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info
  );


uint32
  arad_pp_rif_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  );

uint32
  arad_pp_rif_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PPC_RIF_INFO                            *rif_info
  );

uint32
  arad_pp_rif_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID                              rif_ndx
  );


uint32
  arad_pp_rif_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_RIF_ID                              rif_ndx,
    SOC_SAND_OUT SOC_PPC_RIF_INFO                            *rif_info
  );


uint32
  arad_pp_rif_ttl_scope_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  );

uint32
  arad_pp_rif_ttl_scope_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  );

uint32
  arad_pp_rif_ttl_scope_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx
  );


uint32
  arad_pp_rif_ttl_scope_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl_val
  );





uint32
arad_pp_rif_native_routing_vlan_tags_set_unsafe(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  native_routing_vlan_tags
   );

uint32
arad_pp_rif_native_routing_vlan_tags_set_verify(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  native_routing_vlan_tags
   );

uint32
arad_pp_rif_native_routing_vlan_tags_get_unsafe(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_OUT  uint8                                  *native_routing_vlan_tags
   );

uint32
arad_pp_rif_native_routing_vlan_tags_get_verify(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPC_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  *native_routing_vlan_tags
   );

uint32
  arad_pp_l2_rif_mpls_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY              *isem_key,
    SOC_SAND_OUT SOC_PPC_MPLS_LABEL_RIF_KEY           *rif_key
  );

uint32
  arad_pp_rif_ip_tunnel_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY              *isem_key,
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_KEY              *term_key
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_rif_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_rif_get_errs_ptr(void);

uint32
  SOC_PPC_RIF_MPLS_LABELS_RANGE_verify(
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE *info
  );

uint32
  SOC_PPC_MPLS_LABEL_RIF_KEY_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *info
  );

uint32
  SOC_PPC_RIF_INFO_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PPC_RIF_INFO *info
  );

uint32
  SOC_PPC_RIF_IP_TERM_INFO_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO *info
  );


uint32
  arad_pp_get_in_rif_profile_from_vrf_internal_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID  vrf_id,
    SOC_SAND_OUT uint8   *out_is_in_rif_profile
  );


uint32
  arad_pp_set_in_rif_profile_to_vrf_internal_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID  vrf_id,
    SOC_SAND_IN  uint8   is_in_rif_profile
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
