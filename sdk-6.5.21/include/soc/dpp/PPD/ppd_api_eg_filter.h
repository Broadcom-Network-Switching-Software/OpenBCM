/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_EG_FILTER_INCLUDED__

#define __SOC_PPD_API_EG_FILTER_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_llp_filter.h>
#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_EG_FILTER_PORT_INFO_SET = SOC_PPD_PROC_DESC_BASE_EG_FILTER_FIRST,
  SOC_PPD_EG_FILTER_PORT_INFO_SET_PRINT,
  SOC_PPD_EG_FILTER_PORT_INFO_GET,
  SOC_PPD_EG_FILTER_PORT_INFO_GET_PRINT,
  SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET,
  SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_PRINT,
  SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET,
  SOC_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_PRINT,
  SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET,
  SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_PRINT,
  SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET,
  SOC_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_PRINT,
  SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET,
  SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PRINT,
  SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET,
  SOC_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PRINT,
  SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET,
  SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PRINT,
  SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET,
  SOC_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PRINT,
  SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET,
  SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET_PRINT,
  SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET,
  SOC_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET_PRINT,
  SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET,
  SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_PRINT,
  SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET,
  SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_PRINT,
  SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET,
  SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_PRINT,
  SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET,
  SOC_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_PRINT,
  SOC_PPD_EG_FILTER_VSI_MEMBERSHIP_GET,
  SOC_PPD_EG_FILTER_VSI_MEMBERSHIP_GET_PRINT,
  SOC_PPD_EG_FILTER_GET_PROCS_PTR,
  
  
  SOC_PPD_EG_FILTER_PROCEDURE_DESC_LAST
} SOC_PPD_EG_FILTER_PROCEDURE_DESC;










uint32
  soc_ppd_eg_filter_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO                 *port_info
  );


uint32
  soc_ppd_eg_filter_port_info_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PORT_INFO                 *port_info
  );

#ifdef BCM_88660_A0

uint32
  soc_ppd_eg_filter_global_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_GLOBAL_INFO          *global_info
  );


uint32
  soc_ppd_eg_filter_global_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_GLOBAL_INFO        *global_info
  );
#endif 


uint32
  soc_ppd_eg_filter_vsi_port_membership_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  uint8                               is_member
  );


uint32
  soc_ppd_eg_filter_vsi_port_membership_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_OUT uint8                               *is_member
  );


uint32
  soc_ppd_eg_filter_vsi_membership_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_VSI_ID                       vsid_ndx,
    SOC_SAND_OUT uint32                               ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );


uint32
  soc_ppd_eg_filter_cvid_port_membership_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  uint8                               is_member
  );


uint32
  soc_ppd_eg_filter_cvid_port_membership_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         cvid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_OUT uint8                               *is_member
  );


uint32
  soc_ppd_eg_filter_port_acceptable_frames_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               eg_acceptable_frames_port_profile,
    SOC_SAND_IN  uint32                               llvp_port_profile,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO               *eg_prsr_out_key,
    SOC_SAND_IN  uint8                                accept
  );


uint32
  soc_ppd_eg_filter_port_acceptable_frames_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                out_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *eg_prsr_out_key,
    SOC_SAND_OUT uint8                               *accept
  );


uint32
  soc_ppd_eg_filter_pep_acceptable_frames_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_IN  uint8                               accept
  );


uint32
  soc_ppd_eg_filter_pep_acceptable_frames_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_PEP_KEY                             *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_OUT uint8                               *accept
  );


uint32
  soc_ppd_eg_filter_pvlan_port_type_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE           pvlan_port_type
  );


uint32
  soc_ppd_eg_filter_pvlan_port_type_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE           *pvlan_port_type
  );


uint32
  soc_ppd_eg_filter_split_horizon_out_ac_orientation_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION           orientation
  );


uint32
  soc_ppd_eg_filter_split_horizon_out_ac_orientation_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_AC_ID                               out_ac_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION           *orientation
  );


uint32
  soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION           orientation
  );


uint32
  soc_ppd_eg_filter_split_horizon_out_lif_orientation_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               lif_eep_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION           *orientation
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

