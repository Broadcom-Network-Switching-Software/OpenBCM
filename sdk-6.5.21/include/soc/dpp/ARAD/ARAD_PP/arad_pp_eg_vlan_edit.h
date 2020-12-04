
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_EG_VLAN_EDIT_INCLUDED__

#define __ARAD_PP_EG_VLAN_EDIT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/PPC/ppc_api_eg_vlan_edit.h>














typedef enum
{
  
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET = ARAD_PP_PROC_DESC_BASE_EG_VLAN_EDIT_FIRST,
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET_PRINT,
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET_UNSAFE,
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_SET_VERIFY,
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET,
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET_PRINT,
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET_VERIFY,
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_GET_UNSAFE,
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET,
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET_PRINT,
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET_UNSAFE,
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_SET_VERIFY,
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET,
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET_PRINT,
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET_VERIFY,
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_GET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET,
  ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET,
  ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET,
  ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET,
  ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_SET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_STAG_GET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PRINT,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_GET_PROCS_PTR,
  ARAD_PP_EG_VLAN_EDIT_GET_ERRS_PTR,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET_VERIFY,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_UNSAFE,
  ARAD_PP_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_VERIFY,
  

  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_TBL_SET,
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_TBL_GET,

  
  ARAD_PP_EG_VLAN_EDIT_PROCEDURE_DESC_LAST
} ARAD_PP_EG_VLAN_EDIT_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_EG_VLAN_EDIT_TRANSMIT_OUTER_TAG_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_EG_VLAN_EDIT_FIRST,
  ARAD_PP_EG_VLAN_EDIT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VLAN_EDIT_KEY_MAPPING_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VLAN_EDIT_PEP_EDIT_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VLAN_EDIT_TAG_FORMAT_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VLAN_EDIT_EDIT_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VLAN_EDIT_TPID_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VLAN_EDIT_VID_SOURCE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SOURCE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VLAN_EDIT_TAGS_TO_REMOVE_OUT_OF_RANGE_ERR,
  

  ARAD_PP_EG_VLAN_EDIT_PCP_DEI_SRC_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_VLAN_EDIT_VID_SRC_OUT_OF_RANGE_ERR,
  ARAD_PP_HDR_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_EPNI_PCP_DEI_MAP_KEY_OUT_OF_RANGE_ERR,
  SOC_PPC_PKT_FRWRD_TYPE_NDX_OUT_OF_RANGE_ERR,

  
  
  ARAD_PP_EG_VLAN_EDIT_ERR_LAST
} ARAD_PP_EG_VLAN_EDIT_ERR;









uint32
  arad_pp_eg_vlan_edit_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_eg_vlan_edit_pep_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_PEP_INFO               *pep_info
  );

uint32
  arad_pp_eg_vlan_edit_pep_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_PEP_INFO               *pep_info
  );

uint32
  arad_pp_eg_vlan_edit_pep_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key
  );


uint32
  arad_pp_eg_vlan_edit_pep_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_PEP_INFO               *pep_info
  );


uint32
  arad_pp_eg_vlan_edit_command_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY            *command_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO           *command_info
  );

uint32
  arad_pp_eg_vlan_edit_command_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY            *command_key,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO           *command_info
  );

uint32
  arad_pp_eg_vlan_edit_command_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY            *command_key
  );


uint32
  arad_pp_eg_vlan_edit_command_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY            *command_key,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO           *command_info
  );


uint32
  arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                 transmit_outer_tag
  );

uint32
  arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                 transmit_outer_tag
  );

uint32
  arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );


uint32
  arad_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint8                                 *transmit_outer_tag
  );


uint32
  arad_pp_eg_vlan_edit_pcp_profile_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY    key_mapping
  );

uint32
  arad_pp_eg_vlan_edit_pcp_profile_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY    key_mapping
  );

uint32
  arad_pp_eg_vlan_edit_pcp_profile_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx
  );


uint32
  arad_pp_eg_vlan_edit_pcp_profile_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY    *key_mapping
  );


uint32
  arad_pp_eg_vlan_edit_pcp_map_stag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  );

uint32
  arad_pp_eg_vlan_edit_pcp_map_stag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  );

uint32
  arad_pp_eg_vlan_edit_pcp_map_stag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx
  );


uint32
  arad_pp_eg_vlan_edit_pcp_map_stag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  );


uint32
  arad_pp_eg_vlan_edit_pcp_map_ctag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  );

uint32
  arad_pp_eg_vlan_edit_pcp_map_ctag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  );

uint32
  arad_pp_eg_vlan_edit_pcp_map_ctag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx
  );


uint32
  arad_pp_eg_vlan_edit_pcp_map_ctag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  );


uint32
  arad_pp_eg_vlan_edit_pcp_map_untagged_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  );

uint32
  arad_pp_eg_vlan_edit_pcp_map_untagged_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  );

uint32
  arad_pp_eg_vlan_edit_pcp_map_untagged_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx
  );


uint32
  arad_pp_eg_vlan_edit_pcp_map_untagged_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  );


uint32 
  arad_pp_eg_vlan_edit_pcp_map_dscp_exp_get_unsafe( 
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                     pkt_type_ndx, 
    SOC_SAND_IN uint8                                    dscp_exp_ndx, 
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp, 
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei 
  ); 

uint32 
  arad_pp_eg_vlan_edit_pcp_map_dscp_exp_get_verify( 
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                     pkt_type_ndx, 
    SOC_SAND_IN uint8                                    dscp_exp_ndx
  ); 


uint32 
  arad_pp_eg_vlan_edit_pcp_map_dscp_exp_set_unsafe( 
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                     pkt_type_ndx, 
    SOC_SAND_IN uint8                                    dscp_exp_ndx, 
    SOC_SAND_IN SOC_SAND_PP_PCP_UP                             out_pcp, 
    SOC_SAND_IN SOC_SAND_PP_DEI_CFI                            out_dei 
  ); 

uint32 
  arad_pp_eg_vlan_edit_pcp_map_dscp_exp_set_verify( 
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_PKT_FRWRD_TYPE                     pkt_type_ndx, 
    SOC_SAND_IN uint8                                    dscp_exp_ndx, 
    SOC_SAND_IN SOC_SAND_PP_PCP_UP                             out_pcp, 
    SOC_SAND_IN SOC_SAND_PP_DEI_CFI                            out_dei 
  ); 


soc_error_t arad_pp_eg_vlan_edit_packet_is_tagged_set(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY    *eg_command_key,
    SOC_SAND_IN uint32                              is_tagged
    );


soc_error_t arad_pp_eg_vlan_edit_packet_is_tagged_get(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY    *eg_command_key,
    SOC_SAND_OUT uint32                             *is_tagged
    );



CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_vlan_edit_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_vlan_edit_get_errs_ptr(void);

uint32
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_PEP_INFO *info
  );

uint32
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY *info
  );

uint32
  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  );

uint32
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



