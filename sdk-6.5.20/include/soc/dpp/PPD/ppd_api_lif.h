/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LIF_INCLUDED__

#define __SOC_PPD_API_LIF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_lif.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_L2_LIF_PWE_MAP_RANGE_SET = SOC_PPD_PROC_DESC_BASE_LIF_FIRST,
  SOC_PPD_L2_LIF_PWE_MAP_RANGE_SET_PRINT,
  SOC_PPD_L2_LIF_PWE_MAP_RANGE_GET,
  SOC_PPD_L2_LIF_PWE_MAP_RANGE_GET_PRINT,
  SOC_PPD_L2_LIF_PWE_ADD,
  SOC_PPD_L2_LIF_PWE_ADD_PRINT,
  SOC_PPD_L2_LIF_PWE_GET,
  SOC_PPD_L2_LIF_PWE_GET_PRINT,
  SOC_PPD_L2_LIF_PWE_REMOVE,
  SOC_PPD_L2_LIF_PWE_REMOVE_PRINT,
  SOC_PPD_L2_LIF_VLAN_COMPRESSION_ADD,
  SOC_PPD_L2_LIF_VLAN_COMPRESSION_ADD_PRINT,
  SOC_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE,
  SOC_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_PRINT,
  SOC_PPD_L2_LIF_VLAN_COMPRESSION_GET,
  SOC_PPD_L2_LIF_VLAN_COMPRESSION_GET_PRINT,
  SOC_PPD_T20E_L2_LIF_AC_MAP_KEY_SET,
  SOC_PPD_T20E_L2_LIF_AC_MAP_KEY_SET_PRINT,
  SOC_PPD_T20E_L2_LIF_AC_MAP_KEY_GET,
  SOC_PPD_T20E_L2_LIF_AC_MAP_KEY_GET_PRINT,
  SOC_PPD_L2_LIF_AC_MAP_KEY_SET,
  SOC_PPD_L2_LIF_AC_MAP_KEY_SET_PRINT,
  SOC_PPD_L2_LIF_AC_MAP_KEY_GET,
  SOC_PPD_L2_LIF_AC_MAP_KEY_GET_PRINT,
  SOC_PPD_L2_LIF_AC_MP_INFO_SET,
  SOC_PPD_L2_LIF_AC_MP_INFO_SET_PRINT,
  SOC_PPD_L2_LIF_AC_MP_INFO_GET,
  SOC_PPD_L2_LIF_AC_MP_INFO_GET_PRINT,
  SOC_PPD_L2_LIF_AC_ADD,
  SOC_PPD_L2_LIF_AC_ADD_PRINT,
  SOC_PPD_L2_LIF_AC_GET,
  SOC_PPD_L2_LIF_AC_GET_PRINT,
  SOC_PPD_L2_LIF_AC_WITH_COS_ADD,
  SOC_PPD_L2_LIF_AC_WITH_COS_ADD_PRINT,
  SOC_PPD_L2_LIF_AC_WITH_COS_GET,
  SOC_PPD_L2_LIF_AC_WITH_COS_GET_PRINT,
  SOC_PPD_L2_LIF_AC_REMOVE,
  SOC_PPD_L2_LIF_AC_REMOVE_PRINT,
  SOC_PPD_L2_LIF_L2CP_TRAP_SET,
  SOC_PPD_L2_LIF_L2CP_TRAP_SET_PRINT,
  SOC_PPD_L2_LIF_L2CP_TRAP_GET,
  SOC_PPD_L2_LIF_L2CP_TRAP_GET_PRINT,
  SOC_PPD_L2_LIF_ISID_ADD,
  SOC_PPD_L2_LIF_ISID_ADD_PRINT,
  SOC_PPD_L2_LIF_ISID_GET,
  SOC_PPD_L2_LIF_ISID_GET_PRINT,
  SOC_PPD_L2_LIF_VSI_TO_ISID,
  SOC_PPD_L2_LIF_VSI_TO_ISID_PRINT,
  SOC_PPD_L2_LIF_ISID_REMOVE,
  SOC_PPD_L2_LIF_ISID_REMOVE_PRINT,
  SOC_PPD_L2_LIF_DEFAULT_FRWRD_INFO_SET,
  SOC_PPD_L2_LIF_DEFAULT_FRWRD_INFO_SET_PRINT,
  SOC_PPD_L2_LIF_DEFAULT_FRWRD_INFO_GET,
  SOC_PPD_L2_LIF_DEFAULT_FRWRD_INFO_GET_PRINT,
  SOC_PPD_L2_LIF_GRE_ADD,
  SOC_PPD_L2_LIF_GRE_GET,
  SOC_PPD_L2_LIF_GRE_REMOVE,
  SOC_PPD_L2_LIF_VXLAN_ADD,
  SOC_PPD_L2_LIF_VXLAN_GET,
  SOC_PPD_L2_LIF_VXLAN_REMOVE,
  SOC_PPD_LIF_GET_PROCS_PTR,
  
  
  SOC_PPD_LIF_PROCEDURE_DESC_LAST
} SOC_PPD_LIF_PROCEDURE_DESC;










uint32
  soc_ppd_l2_lif_pwe_map_range_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE                  *in_vc_range
  );


uint32
  soc_ppd_l2_lif_pwe_map_range_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_L2_LIF_IN_VC_RANGE                  *in_vc_range
  );


uint32
  soc_ppd_l2_lif_pwe_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                      in_vc_label,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO                     *pwe_additional_info,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_l2_lif_pwe_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                      in_vc_label,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO          *pwe_additional_info,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_l2_lif_pwe_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                      in_vc_label,
    SOC_SAND_IN  uint8                                remove_from_sem,
    SOC_SAND_IN  uint8                                is_gal,
    SOC_SAND_OUT uint32                                *lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO   *pwe_additional_info
  );


uint32
  soc_ppd_l2_lif_vlan_compression_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                   *vlan_range_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_l2_lif_vlan_compression_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                   *vlan_range_key
  );


uint32
  soc_ppd_l2_lif_vlan_compression_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                         local_port_ndx,
    SOC_SAND_OUT SOC_PPC_L2_PORT_VLAN_RANGE_INFO      *vlan_range_info
  );


uint32
  soc_ppd_l2_lif_ac_map_key_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER             *qual_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE              key_mapping
  );


uint32
  soc_ppd_l2_lif_ac_map_key_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER             *qual_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE              *key_mapping
  );


uint32
  soc_ppd_l2_lif_ac_mp_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                         lif_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO              *info
  );


uint32
  soc_ppd_l2_lif_ac_mp_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                         lif_ndx,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MP_INFO              *info
  );


uint32
  soc_ppd_l2_lif_ac_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO                      *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_l2_lif_ac_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_INFO                      *ac_info,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_l2_lif_ac_with_cos_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO                *acs_group_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_l2_lif_ac_with_cos_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *base_lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_GROUP_INFO                *acs_group_info,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_l2_lif_ac_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  );


uint32
  soc_ppd_l2_lif_l2cp_trap_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE             handle_type
  );


uint32
  soc_ppd_l2_lif_l2cp_trap_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE             *handle_type
  );


uint32
  soc_ppd_l2_lif_isid_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_IN  uint8                                       is_ingress,
    SOC_SAND_IN  uint8                                       is_egress,
    SOC_SAND_IN  uint8                                       isem_only,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_l2_lif_isid_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_SYS_VSI_ID                          *vsi_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_l2_lif_vsi_to_isid(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                                vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_ISID 								   *isid_id
  );


uint32
  soc_ppd_l2_lif_isid_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  uint8                                       is_ingress,
    SOC_SAND_IN  uint8                                       is_egress,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi,
    SOC_SAND_IN  uint8                                       isem_only,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  );


uint32
  soc_ppd_l2_lif_gre_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO                    *gre_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  );


uint32
  soc_ppd_l2_lif_gre_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_INFO                    *gre_info,
    SOC_SAND_OUT uint8                               *found
  );



uint32
  soc_ppd_l2_lif_gre_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                   *gre_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                           *lif_index
  );


uint32
  soc_ppd_l2_lif_vxlan_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_IN  int                                         ignore_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  );


uint32
  soc_ppd_l2_lif_vxlan_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_OUT uint8                               *found
  );



uint32
  soc_ppd_l2_lif_vxlan_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                   *vxlan_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                           *lif_index
  );




uint32 soc_ppd_l2_lif_extender_add(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY        *extender_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                     lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO       *extender_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success);




uint32
  soc_ppd_l2_lif_default_frwrd_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY            *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION         *action_info
  );


uint32
  soc_ppd_l2_lif_default_frwrd_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY            *dflt_frwrd_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION         *action_info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
