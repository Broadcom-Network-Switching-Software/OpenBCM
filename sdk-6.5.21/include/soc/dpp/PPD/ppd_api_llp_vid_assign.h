/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LLP_VID_ASSIGN_INCLUDED__

#define __SOC_PPD_API_LLP_VID_ASSIGN_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_llp_vid_assign.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_llp_sa_auth.h>













typedef enum
{
  
  SOC_PPD_LLP_VID_ASSIGN_PORT_INFO_SET = SOC_PPD_PROC_DESC_BASE_LLP_VID_ASSIGN_FIRST,
  SOC_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_PORT_INFO_GET,
  SOC_PPD_LLP_VID_ASSIGN_PORT_INFO_GET_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD,
  SOC_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_MAC_BASED_REMOVE,
  SOC_PPD_LLP_VID_ASSIGN_MAC_BASED_REMOVE_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_MAC_BASED_GET,
  SOC_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK,
  SOC_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET,
  SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET,
  SOC_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET,
  SOC_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_GET,
  SOC_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_PRINT,
  SOC_PPD_LLP_VID_ASSIGN_GET_PROCS_PTR,
  
  
  SOC_PPD_LLP_VID_ASSIGN_PROCEDURE_DESC_LAST
} SOC_PPD_LLP_VID_ASSIGN_PROCEDURE_DESC;











uint32
  soc_ppd_llp_vid_assign_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  );


uint32
  soc_ppd_llp_vid_assign_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  );


uint32
  soc_ppd_llp_vid_assign_mac_based_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_llp_vid_assign_mac_based_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key
  );


uint32
  soc_ppd_llp_vid_assign_mac_based_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_llp_vid_assign_mac_based_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE           *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key_arr,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MAC_INFO             *vid_assign_info_arr,
    SOC_SAND_OUT uint32                                *nof_entries
  );


uint32
  soc_ppd_llp_vid_assign_ipv4_subnet_based_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                     *subnet,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  );


uint32
  soc_ppd_llp_vid_assign_ipv4_subnet_based_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                     *subnet,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  );


uint32
  soc_ppd_llp_vid_assign_protocol_based_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  uint16                                ether_type_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_llp_vid_assign_protocol_based_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  uint16                                ether_type_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

