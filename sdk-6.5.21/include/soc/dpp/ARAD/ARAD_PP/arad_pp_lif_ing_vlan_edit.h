
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_PP_LIF_ING_VLAN_EDIT_INCLUDED__

#define __ARAD_PP_LIF_ING_VLAN_EDIT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_lif_ing_vlan_edit.h>













typedef enum
{
  
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET = ARAD_PP_PROC_DESC_BASE_LIF_ING_VLAN_EDIT_FIRST,
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_PRINT,
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_UNSAFE,
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_VERIFY,
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET,
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_PRINT,
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_VERIFY,
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_UNSAFE,
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET,
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_PRINT,
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE,
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_VERIFY,
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET,
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_PRINT,
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_VERIFY,
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PRINT,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PRINT,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PRINT,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PRINT,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PRINT,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PRINT,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE,
  ARAD_PP_LIF_ING_VLAN_EDIT_GET_PROCS_PTR,
  ARAD_PP_LIF_ING_VLAN_EDIT_GET_ERRS_PTR,
  

  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_SET,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_TBL_GET,

  
  ARAD_PP_LIF_ING_VLAN_EDIT_PROCEDURE_DESC_LAST
} ARAD_PP_LIF_ING_VLAN_EDIT_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_ID_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LIF_ING_VLAN_EDIT_FIRST,
  ARAD_PP_LIF_ING_VLAN_EDIT_COMMAND_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_TAG_PCP_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_OUT_PCP_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_TAG_UP_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_TC_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_DP_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_TPID_PROFILE_OUT_OF_RANGE_ERR,
  

  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_VID_SRC_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_INCONSISTANT_TBLS_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_DP_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_LIF_ING_VLAN_EDIT_PCP_DEI_OUT_OF_RANGE_ERR,
  
  ARAD_PP_LIF_ING_VLAN_EDIT_ERR_LAST
} ARAD_PP_LIF_ING_VLAN_EDIT_ERR;









uint32
  arad_pp_lif_ing_vlan_edit_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_lif_ing_vlan_edit_command_id_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key,
    SOC_SAND_IN  uint32                                  command_id
  );

uint32
  arad_pp_lif_ing_vlan_edit_command_id_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key,
    SOC_SAND_IN  uint32                                  command_id
  );

uint32
  arad_pp_lif_ing_vlan_edit_command_id_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key
  );


uint32
  arad_pp_lif_ing_vlan_edit_command_id_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key,
    SOC_SAND_OUT uint32                                  *command_id
  );


uint32
  arad_pp_lif_ing_vlan_edit_command_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  command_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO      *command_info
  );

uint32
  arad_pp_lif_ing_vlan_edit_command_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  command_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO      *command_info
  );

uint32
  arad_pp_lif_ing_vlan_edit_command_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  command_ndx
  );


uint32
  arad_pp_lif_ing_vlan_edit_command_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  command_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO      *command_info
  );


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_stag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tag_pcp_ndx,
    SOC_SAND_IN  uint8                                 tag_dei_ndx,
    SOC_SAND_IN  uint8                                   out_pcp,
    SOC_SAND_IN  uint8                                 out_dei
  );

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_stag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tag_pcp_ndx,
    SOC_SAND_IN  uint8                                 tag_dei_ndx,
    SOC_SAND_IN  uint8                                   out_pcp,
    SOC_SAND_IN  uint8                                 out_dei
  );

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_stag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tag_pcp_ndx,
    SOC_SAND_IN  uint8                                 tag_dei_ndx
  );


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_stag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tag_pcp_ndx,
    SOC_SAND_IN  uint8                                 tag_dei_ndx,
    SOC_SAND_OUT uint8                                   *out_pcp,
    SOC_SAND_OUT uint8                                 *out_dei
  );


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_ctag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tag_up_ndx,
    SOC_SAND_IN  uint8                                   out_pcp,
    SOC_SAND_IN  uint8                                 out_dei
  );

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_ctag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tag_up_ndx,
    SOC_SAND_IN  uint8                                   out_pcp,
    SOC_SAND_IN  uint8                                 out_dei
  );

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_ctag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tag_up_ndx
  );


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_ctag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tag_up_ndx,
    SOC_SAND_OUT uint8                                   *out_pcp,
    SOC_SAND_OUT uint8                                 *out_dei
  );


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_untagged_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tc_ndx,
    SOC_SAND_IN  uint8                                   dp_ndx,
    SOC_SAND_IN  uint8                                   out_pcp,
    SOC_SAND_IN  uint8                                 out_dei
  );

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_untagged_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tc_ndx,
    SOC_SAND_IN  uint8                                   dp_ndx,
    SOC_SAND_IN  uint8                                   out_pcp,
    SOC_SAND_IN  uint8                                 out_dei
  );

uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_untagged_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tc_ndx,
    SOC_SAND_IN  uint8                                   dp_ndx
  );


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_untagged_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  uint8                                   tc_ndx,
    SOC_SAND_IN  uint8                                   dp_ndx,
    SOC_SAND_OUT uint8                                   *out_pcp,
    SOC_SAND_OUT uint8                                 *out_dei
  );


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_dp_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dp_profile_ndx,
    SOC_SAND_IN  uint32                                   dp_ndx,
    SOC_SAND_IN  uint32                                sel_pcp_dei,
    SOC_SAND_IN  uint8                                   out_pcp,
    SOC_SAND_IN  uint8                                 out_dei
  );


uint32
  arad_pp_lif_ing_vlan_edit_pcp_map_dp_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dp_profile_ndx,
    SOC_SAND_IN  uint32                                   dp_ndx,
    SOC_SAND_IN  uint32                                 sel_pcp_dei,
    SOC_SAND_OUT uint8                                   *out_pcp,
    SOC_SAND_OUT uint8                                 *out_dei
  );




uint32
  arad_pp_lif_ing_vlan_edit_inner_global_info_set(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_IN int value
  );

uint32
  arad_pp_lif_ing_vlan_edit_outer_global_info_set(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_IN int value
  );

uint32
  arad_pp_lif_ing_vlan_edit_inner_global_info_get(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_OUT int *value
  );

uint32
  arad_pp_lif_ing_vlan_edit_outer_global_info_get(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_OUT int *value
  );



CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lif_ing_vlan_edit_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lif_ing_vlan_edit_get_errs_ptr(void);

uint32
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  );

uint32
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY *info
  );

uint32
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



