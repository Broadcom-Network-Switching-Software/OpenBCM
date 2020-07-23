/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_FRWRD_IPV6_INCLUDED__

#define __SOC_PPD_API_FRWRD_IPV6_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_frwrd_ipv6.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>













typedef enum
{
  
  SOC_PPD_FRWRD_IPV6_UC_ROUTE_ADD = SOC_PPD_PROC_DESC_BASE_FRWRD_IPV6_FIRST,
  SOC_PPD_FRWRD_IPV6_UC_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_IPV6_UC_ROUTE_GET,
  SOC_PPD_FRWRD_IPV6_UC_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV6_UC_ROUTE_REMOVE,
  SOC_PPD_FRWRD_IPV6_UC_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV6_MC_ROUTE_ADD,
  SOC_PPD_FRWRD_IPV6_MC_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_IPV6_MC_ROUTE_GET,
  SOC_PPD_FRWRD_IPV6_MC_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV6_MC_ROUTE_REMOVE,
  SOC_PPD_FRWRD_IPV6_MC_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV6_VRF_INFO_SET,
  SOC_PPD_FRWRD_IPV6_VRF_INFO_SET_PRINT,
  SOC_PPD_FRWRD_IPV6_VRF_INFO_GET,
  SOC_PPD_FRWRD_IPV6_VRF_INFO_GET_PRINT,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTE_ADD,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTE_GET,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTE_REMOVE,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR,
  SOC_PPD_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV6_GET_PROCS_PTR,
  



  
  SOC_PPD_FRWRD_IPV6_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_IPV6_PROCEDURE_DESC;










uint32
  soc_ppd_frwrd_ipv6_uc_route_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_ipv6_uc_route_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                           flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO             *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_frwrd_ipv6_uc_route_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO             *route_infos,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *routes_location,
    SOC_SAND_OUT uint32                            *nof_entries
  );


uint32
  soc_ppd_frwrd_ipv6_uc_route_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_ipv6_uc_routing_table_clear(
    SOC_SAND_IN  int                           unit
  );


uint32
  soc_ppd_frwrd_ipv6_host_add(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                   vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *host_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_ipv6_host_get(
    SOC_SAND_IN   int                                      unit,
    SOC_SAND_IN   uint32                                   vrf_ndx,
    SOC_SAND_IN   SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *host_key,
    SOC_SAND_OUT  SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT  uint8                                   *found
  );


uint32
  soc_ppd_frwrd_ipv6_host_delete(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                   vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY         *host_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_ipv6_mc_route_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );



uint32
  soc_ppd_frwrd_ipv6_mc_compress_sip_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );



uint32
  soc_ppd_frwrd_ipv6_mc_route_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                           flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_frwrd_ipv6_mc_compress_sip_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                           flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_frwrd_ipv6_mc_route_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *routes_location,
    SOC_SAND_OUT uint32                            *nof_entries
  );


uint32
  soc_ppd_frwrd_ipv6_mc_route_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );



uint32
  soc_ppd_frwrd_ipv6_mc_compress_sip_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );



uint32
  soc_ppd_frwrd_ipv6_mc_routing_table_clear(
    SOC_SAND_IN  int                           unit
  );


uint32
  soc_ppd_frwrd_ipv6_vrf_info_set(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                          vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VRF_INFO             *vrf_info
  );


uint32
  soc_ppd_frwrd_ipv6_vrf_info_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                          vrf_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_VRF_INFO             *vrf_info
  );


uint32
  soc_ppd_frwrd_ipv6_vrf_route_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                          vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY        *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_ipv6_vrf_route_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                          vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY        *route_key,
    SOC_SAND_IN  uint8                                   flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO            *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_frwrd_ipv6_vrf_route_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                          vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY        *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO            *route_infos,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *routes_location,
    SOC_SAND_OUT uint32                            *nof_entries
  );


uint32
  soc_ppd_frwrd_ipv6_vrf_route_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                          vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY        *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_ipv6_vrf_routing_table_clear(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                          vrf_ndx
  );


uint32
  soc_ppd_frwrd_ipv6_vrf_all_routing_tables_clear(
    SOC_SAND_IN  int                           unit
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

