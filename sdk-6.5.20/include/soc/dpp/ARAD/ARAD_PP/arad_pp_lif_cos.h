/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LIF_COS_INCLUDED__

#define __ARAD_PP_LIF_COS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_lif_cos.h>














typedef enum
{
  
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET = ARAD_PP_PROC_DESC_BASE_LIF_COS_FIRST,
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET_PRINT,
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET_UNSAFE,
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_SET_VERIFY,
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET,
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET_PRINT,
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET_VERIFY,
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_GET_UNSAFE,
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET,
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET_PRINT,
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET_UNSAFE,
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_SET_VERIFY,
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET,
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET_PRINT,
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET_VERIFY,
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_GET_UNSAFE,
  SOC_PPC_LIF_COS_PROFILE_INFO_SET,
  SOC_PPC_LIF_COS_PROFILE_INFO_SET_PRINT,
  SOC_PPC_LIF_COS_PROFILE_INFO_SET_UNSAFE,
  SOC_PPC_LIF_COS_PROFILE_INFO_SET_VERIFY,
  SOC_PPC_LIF_COS_PROFILE_INFO_GET,
  SOC_PPC_LIF_COS_PROFILE_INFO_GET_PRINT,
  SOC_PPC_LIF_COS_PROFILE_INFO_GET_VERIFY,
  SOC_PPC_LIF_COS_PROFILE_INFO_GET_UNSAFE,
  ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET,
  ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_PRINT,
  ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_UNSAFE,
  ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_SET_VERIFY,
  ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET,
  ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_PRINT,
  ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_VERIFY,
  ARAD_PP_LIF_COS_PROFILE_MAP_L2_INFO_GET_UNSAFE,
  ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET,
  ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_PRINT,
  ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_UNSAFE,
  ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_SET_VERIFY,
  ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET,
  ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_PRINT,
  ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_VERIFY,
  ARAD_PP_LIF_COS_PROFILE_MAP_IP_INFO_GET_UNSAFE,
  ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET,
  ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PRINT,
  ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_UNSAFE,
  ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_VERIFY,
  ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET,
  ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PRINT,
  ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_VERIFY,
  ARAD_PP_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_UNSAFE,
  ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET,
  ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PRINT,
  ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_UNSAFE,
  ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_VERIFY,
  ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET,
  ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PRINT,
  ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_VERIFY,
  ARAD_PP_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_UNSAFE,
  SOC_PPC_LIF_COS_OPCODE_TYPES_SET,
  SOC_PPC_LIF_COS_OPCODE_TYPES_SET_PRINT,
  SOC_PPC_LIF_COS_OPCODE_TYPES_SET_UNSAFE,
  SOC_PPC_LIF_COS_OPCODE_TYPES_SET_VERIFY,
  SOC_PPC_LIF_COS_OPCODE_TYPES_GET,
  SOC_PPC_LIF_COS_OPCODE_TYPES_GET_PRINT,
  SOC_PPC_LIF_COS_OPCODE_TYPES_GET_VERIFY,
  SOC_PPC_LIF_COS_OPCODE_TYPES_GET_UNSAFE,
  ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET,
  ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_PRINT,
  ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_UNSAFE,
  ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_VERIFY,
  ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET,
  ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_PRINT,
  ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_VERIFY,
  ARAD_PP_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_UNSAFE,
  ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET,
  ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_PRINT,
  ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_UNSAFE,
  ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_VERIFY,
  ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET,
  ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_PRINT,
  ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_VERIFY,
  ARAD_PP_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_UNSAFE,
  ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET,
  ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_PRINT,
  ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_UNSAFE,
  ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_SET_VERIFY,
  ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET,
  ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_PRINT,
  ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_VERIFY,
  ARAD_PP_LIF_COS_OPCODE_TC_DP_MAP_GET_UNSAFE,
  ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET,
  ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_PRINT,
  ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_UNSAFE,
  ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_VERIFY,
  ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET,
  ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_PRINT,
  ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_VERIFY,
  ARAD_PP_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_UNSAFE,
  ARAD_PP_LIF_COS_GET_PROCS_PTR,
  ARAD_PP_LIF_COS_GET_ERRS_PTR,
  

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_SET_UNSAFE,
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_GET_UNSAFE,

  
  ARAD_PP_LIF_COS_PROCEDURE_DESC_LAST
} ARAD_PP_LIF_COS_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LIF_COS_PROFILE_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LIF_COS_FIRST,
  ARAD_PP_LIF_COS_COS_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_COS_MAP_TBL_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_COS_OPCODE_NDX_OUT_OF_RANGE_ERR,
  SOC_PPC_LIF_COS_OPCODE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_COS_TAG_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_COS_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_COS_MAP_TABLE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_COS_IP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_COS_OUTER_TPID_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_COS_AC_OFFSET_OUT_OF_RANGE_ERR,
  

  ARAD_PP_LIF_MAP_WHEN_IP_MPLS_MISMATCH_ERR,
  ARAD_PP_LIF_COS_MAP_FROM_TC_DP_DISABLED_ERR,

  
  ARAD_PP_LIF_COS_ERR_LAST
} ARAD_PP_LIF_COS_ERR;

typedef struct
{
  uint8 map_from_tc_dp;
} ARAD_PP_SW_DB_LIF_COS;








soc_error_t
  arad_pp_lif_cos_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_lif_cos_ac_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO                 *profile_info
  );

uint32
  arad_pp_lif_cos_ac_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO                 *profile_info
  );

uint32
  arad_pp_lif_cos_ac_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx
  );


uint32
  arad_pp_lif_cos_ac_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_AC_PROFILE_INFO                 *profile_info
  );


uint32
  arad_pp_lif_cos_pwe_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO                *profile_info
  );

uint32
  arad_pp_lif_cos_pwe_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO                *profile_info
  );

uint32
  arad_pp_lif_cos_pwe_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx
  );


uint32
  arad_pp_lif_cos_pwe_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PWE_PROFILE_INFO                *profile_info
  );


uint32
  arad_pp_lif_cos_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      cos_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO                    *profile_info
  );

uint32
  arad_pp_lif_cos_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      cos_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO                    *profile_info
  );

uint32
  arad_pp_lif_cos_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      cos_profile_ndx
  );


uint32
  arad_pp_lif_cos_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      cos_profile_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_INFO                    *profile_info
  );


uint32
  arad_pp_lif_cos_profile_map_l2_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY          *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

uint32
  arad_pp_lif_cos_profile_map_l2_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY          *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

uint32
  arad_pp_lif_cos_profile_map_l2_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY          *map_key
  );


uint32
  arad_pp_lif_cos_profile_map_l2_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY          *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );


uint32
  arad_pp_lif_cos_profile_map_ip_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY          *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

uint32
  arad_pp_lif_cos_profile_map_ip_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY          *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

uint32
  arad_pp_lif_cos_profile_map_ip_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY          *map_key
  );


uint32
  arad_pp_lif_cos_profile_map_ip_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY          *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );


uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY        *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY        *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY        *map_key
  );


uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY        *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );


uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY       *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY       *map_key,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY       *map_key
  );


uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY       *map_key,
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );


uint32
  arad_pp_lif_cos_opcode_types_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_TYPE                     opcode_type
  );

uint32
  arad_pp_lif_cos_opcode_types_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_TYPE                     opcode_type
  );

uint32
  arad_pp_lif_cos_opcode_types_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx
  );


uint32
  arad_pp_lif_cos_opcode_types_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      opcode_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_TYPE                     *opcode_type
  );


uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx
  );


uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx
  );


uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  arad_pp_lif_cos_opcode_tc_dp_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

uint32
  arad_pp_lif_cos_opcode_tc_dp_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

uint32
  arad_pp_lif_cos_opcode_tc_dp_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx
  );


uint32
  arad_pp_lif_cos_opcode_tc_dp_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx
  );


uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lif_cos_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lif_cos_get_errs_ptr(void);

uint32
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO *info
  );

uint32
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO *info
  );

uint32
  SOC_PPC_LIF_COS_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO *info
  );

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  );

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  );

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  );

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  );

uint32
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  );

uint32
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

