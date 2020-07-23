/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LIF_COS_INCLUDED__

#define __SOC_PPD_API_LIF_COS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_lif_cos.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_LIF_COS_AC_PROFILE_INFO_SET = SOC_PPD_PROC_DESC_BASE_LIF_COS_FIRST,
  SOC_PPD_LIF_COS_AC_PROFILE_INFO_SET_PRINT,
  SOC_PPD_LIF_COS_AC_PROFILE_INFO_GET,
  SOC_PPD_LIF_COS_AC_PROFILE_INFO_GET_PRINT,
  SOC_PPD_LIF_COS_PWE_PROFILE_INFO_SET,
  SOC_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PRINT,
  SOC_PPD_LIF_COS_PWE_PROFILE_INFO_GET,
  SOC_PPD_LIF_COS_PWE_PROFILE_INFO_GET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_INFO_SET,
  SOC_PPD_LIF_COS_PROFILE_INFO_SET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_INFO_GET,
  SOC_PPD_LIF_COS_PROFILE_INFO_GET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET,
  SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET,
  SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET,
  SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET,
  SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET,
  SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET,
  SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET,
  SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PRINT,
  SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET,
  SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_TYPES_SET,
  SOC_PPD_LIF_COS_OPCODE_TYPES_SET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_TYPES_GET,
  SOC_PPD_LIF_COS_OPCODE_TYPES_GET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET,
  SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET,
  SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET,
  SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET,
  SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET,
  SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET,
  SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET,
  SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_PRINT,
  SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET,
  SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_PRINT,
  SOC_PPD_LIF_COS_GET_PROCS_PTR,
  
  
  SOC_PPD_LIF_COS_PROCEDURE_DESC_LAST
} SOC_PPD_LIF_COS_PROCEDURE_DESC;










uint32
  soc_ppd_lif_cos_ac_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO             *profile_info
  );


uint32
  soc_ppd_lif_cos_ac_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_AC_PROFILE_INFO             *profile_info
  );


uint32
  soc_ppd_lif_cos_pwe_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO            *profile_info
  );


uint32
  soc_ppd_lif_cos_pwe_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PWE_PROFILE_INFO            *profile_info
  );


uint32
  soc_ppd_lif_cos_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                cos_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO                *profile_info
  );


uint32
  soc_ppd_lif_cos_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                cos_profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_INFO                *profile_info
  );


uint32
  soc_ppd_lif_cos_profile_map_l2_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  );


uint32
  soc_ppd_lif_cos_profile_map_l2_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  );


uint32
  soc_ppd_lif_cos_profile_map_ip_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  );


uint32
  soc_ppd_lif_cos_profile_map_ip_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  );


uint32
  soc_ppd_lif_cos_profile_map_mpls_label_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  );


uint32
  soc_ppd_lif_cos_profile_map_mpls_label_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  );


uint32
  soc_ppd_lif_cos_profile_map_tc_dp_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  );


uint32
  soc_ppd_lif_cos_profile_map_tc_dp_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  );


uint32
  soc_ppd_lif_cos_opcode_types_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_TYPE                 opcode_type
  );


uint32
  soc_ppd_lif_cos_opcode_types_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_TYPE                 *opcode_type
  );


uint32
  soc_ppd_lif_cos_opcode_ipv6_tos_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                         ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  soc_ppd_lif_cos_opcode_ipv6_tos_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                         ipv6_tos_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  soc_ppd_lif_cos_opcode_ipv4_tos_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  soc_ppd_lif_cos_opcode_ipv4_tos_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        ipv4_tos_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  soc_ppd_lif_cos_opcode_tc_dp_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  soc_ppd_lif_cos_opcode_tc_dp_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  soc_ppd_lif_cos_opcode_vlan_tag_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  uint8                                 outer_tpid,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         dei_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  soc_ppd_lif_cos_opcode_vlan_tag_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         dei_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

