/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_EG_VLAN_EDIT_INCLUDED__

#define __SOC_PPD_API_EG_VLAN_EDIT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_eg_vlan_edit.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_EG_VLAN_EDIT_PEP_INFO_SET = SOC_PPD_PROC_DESC_BASE_EG_VLAN_EDIT_FIRST,
  SOC_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PEP_INFO_GET,
  SOC_PPD_EG_VLAN_EDIT_PEP_INFO_GET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET,
  SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO_GET,
  SOC_PPD_EG_VLAN_EDIT_COMMAND_INFO_GET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET,
  SOC_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET,
  SOC_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET,
  SOC_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET,
  SOC_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_GET,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_GET,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET,
  SOC_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PRINT,
  SOC_PPD_EG_VLAN_EDIT_GET_PROCS_PTR,
  
  
  SOC_PPD_EG_VLAN_EDIT_PROCEDURE_DESC_LAST
} SOC_PPD_EG_VLAN_EDIT_PROCEDURE_DESC;










uint32
  soc_ppd_eg_vlan_edit_pep_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_PEP_INFO               *pep_info
  );


uint32
  soc_ppd_eg_vlan_edit_pep_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_PEP_INFO               *pep_info
  );


uint32
  soc_ppd_eg_vlan_edit_command_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY            *command_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO           *command_info
  );


uint32
  soc_ppd_eg_vlan_edit_command_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY            *command_key,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO           *command_info
  );


uint32
  soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  uint8                               transmit_outer_tag
  );


uint32
  soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_OUT uint8                               *transmit_outer_tag
  );


uint32
  soc_ppd_eg_vlan_edit_pcp_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY    key_mapping
  );


uint32
  soc_ppd_eg_vlan_edit_pcp_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY    *key_mapping
  );


uint32
  soc_ppd_eg_vlan_edit_pcp_map_stag_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );


uint32
  soc_ppd_eg_vlan_edit_pcp_map_stag_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );


uint32
  soc_ppd_eg_vlan_edit_pcp_map_ctag_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );


uint32
  soc_ppd_eg_vlan_edit_pcp_map_ctag_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );


uint32
  soc_ppd_eg_vlan_edit_pcp_map_untagged_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );


uint32
  soc_ppd_eg_vlan_edit_pcp_map_untagged_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );



uint32 
  soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set( 
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                     pkt_type_ndx, 
    SOC_SAND_IN uint8                                    dscp_exp_ndx, 
    SOC_SAND_IN SOC_SAND_PP_PCP_UP                             out_pcp, 
    SOC_SAND_IN SOC_SAND_PP_DEI_CFI                            out_dei 
  );


uint32 
  soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get( 
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                     pkt_type_ndx, 
    SOC_SAND_IN uint8                                    dscp_exp_ndx, 
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp, 
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei 
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

