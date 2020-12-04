/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LLP_FILTER_INCLUDED__

#define __SOC_PPD_API_LLP_FILTER_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_llp_filter.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET = SOC_PPD_PROC_DESC_BASE_LLP_FILTER_FIRST,
  SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_PRINT,
  SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET,
  SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_PRINT,
  SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD,
  SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_PRINT,
  SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE,
  SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_PRINT,
  SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET,
  SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_PRINT,
  SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET,
  SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_PRINT,
  SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_SET,
  SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_SET_PRINT,
  SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_GET,
  SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_GET_PRINT,
  SOC_PPD_LLP_FILTER_GET_PROCS_PTR,
  
  
  SOC_PPD_LLP_FILTER_PROCEDURE_DESC_LAST
} SOC_PPD_LLP_FILTER_PROCEDURE_DESC;










uint32
  soc_ppd_llp_filter_ingress_vlan_membership_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  uint32                                ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );


uint32
  soc_ppd_llp_filter_ingress_vlan_membership_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_OUT uint32                                ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );


uint32
  soc_ppd_llp_filter_ingress_vlan_membership_port_add(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN   int                              core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  );


uint32
  soc_ppd_llp_filter_ingress_vlan_membership_port_remove(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN   int                              core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  );


uint32
  soc_ppd_llp_filter_ingress_acceptable_frames_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *action_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_llp_filter_ingress_acceptable_frames_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                      *action_profile
  );


uint32
  soc_ppd_llp_filter_designated_vlan_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN   int                              core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid,
    SOC_SAND_IN  uint8                               accept,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_llp_filter_designated_vlan_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN   int                              core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_SAND_PP_VLAN_ID                         *vid,
    SOC_SAND_OUT uint8                               *accept
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

