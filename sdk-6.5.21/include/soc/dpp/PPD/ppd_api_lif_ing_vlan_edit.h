/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LIF_ING_VLAN_EDIT_INCLUDED__

#define __SOC_PPD_API_LIF_ING_VLAN_EDIT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_lif_ing_vlan_edit.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET = SOC_PPD_PROC_DESC_BASE_LIF_ING_VLAN_EDIT_FIRST,
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET,
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET,
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET,
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET,
  SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PRINT,
  SOC_PPD_LIF_ING_VLAN_EDIT_GET_PROCS_PTR,
  
  
  SOC_PPD_LIF_ING_VLAN_EDIT_PROCEDURE_DESC_LAST
} SOC_PPD_LIF_ING_VLAN_EDIT_PROCEDURE_DESC;










uint32
  soc_ppd_lif_ing_vlan_edit_command_id_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key,
    SOC_SAND_IN  uint32                                command_id
  );


uint32
  soc_ppd_lif_ing_vlan_edit_command_id_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key,
    SOC_SAND_OUT uint32                                *command_id
  );


uint32
  soc_ppd_lif_ing_vlan_edit_command_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                command_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO      *command_info
  );


uint32
  soc_ppd_lif_ing_vlan_edit_command_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                command_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO      *command_info
  );


uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );


uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );


uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );


uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );


uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );


uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

