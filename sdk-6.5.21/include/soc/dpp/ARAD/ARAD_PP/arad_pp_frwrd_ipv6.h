/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FRWRD_IPV6_INCLUDED__

#define __ARAD_PP_FRWRD_IPV6_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_frwrd_ipv6.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>
#include <soc/dpp/PPC/ppc_api_general.h>






#define ARAD_PP_IPV6_MC_GROUP_NOF_BITS 120
#define ARAD_PP_IPV6_MC_GROUP_NOF_BYTES 15
#define ARAD_PP_IPV6_MC_GROUP_NOF_UINT32S 4

#define ARAD_PP_IPV6_MC_PREFIX_NOF_BITS 8
#define ARAD_PP_IPV6_MC_ADDR_PREFIX   0xff000000









typedef enum
{
  
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_ADD = ARAD_PP_PROC_DESC_BASE_FRWRD_IPV6_FIRST,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_ADD_PRINT,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_ADD_VERIFY,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET_PRINT,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET_VERIFY,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_REMOVE,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_PRINT,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR,
  ARAD_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_PRINT,
  ARAD_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_VERIFY,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_ADD,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_ADD_PRINT,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_ADD_VERIFY,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET_PRINT,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET_VERIFY,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_REMOVE,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_PRINT,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR,
  ARAD_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_PRINT,
  ARAD_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_VERIFY,
  SOC_PPC_FRWRD_IPV6_VRF_INFO_SET,
  SOC_PPC_FRWRD_IPV6_VRF_INFO_SET_PRINT,
  SOC_PPC_FRWRD_IPV6_VRF_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_IPV6_VRF_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_IPV6_VRF_INFO_GET,
  SOC_PPC_FRWRD_IPV6_VRF_INFO_GET_PRINT,
  SOC_PPC_FRWRD_IPV6_VRF_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_IPV6_VRF_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_ADD,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_ADD_PRINT,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_ADD_VERIFY,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET_PRINT,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET_VERIFY,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_PRINT,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_PRINT,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_VERIFY,
  ARAD_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR,
  ARAD_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT,
  ARAD_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY,
  ARAD_PP_FRWRD_IPV6_GET_PROCS_PTR,
  ARAD_PP_FRWRD_IPV6_GET_ERRS_PTR,
  

  ARAD_PP_FRWRD_IP_TCAM_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_IP_TCAM_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_IP_TCAM_ROUTE_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_IP_TCAM_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_IP_TCAM_ROUTING_TABLE_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_IPV6_MC_COMPRESS_SIP_ADD,
  ARAD_PP_FRWRD_IPV6_MC_COMPRESS_SIP_ADD_UNSAFE,
  ARAD_PP_FRWRD_IPV6_MC_COMPRESS_SIP_GET,
  ARAD_PP_FRWRD_IPV6_MC_COMPRESS_SIP_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV6_MC_COMPRESS_SIP_REMOVE,
  ARAD_PP_FRWRD_IPV6_MC_COMPRESS_SIP_REMOVE_UNSAFE,

  
  ARAD_PP_FRWRD_IPV6_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_IPV6_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FRWRD_IPV6_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_IPV6_FIRST,
  ARAD_PP_FRWRD_IPV6_ROUTE_STATUS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV6_LOCATION_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV6_FOUND_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV6_ROUTES_STATUS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV6_ROUTES_LOCATION_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV6_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV6_EXACT_MATCH_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV6_TYPE_OUT_OF_RANGE_ERR,
  

  ARAD_PP_IPV6_DEFAULT_ACTION_TYPE_NOT_SUPPORTED_ERR,
  ARAD_PP_IPV6_DEFAULT_ACTION_WRONG_TRAP_CODE_ERR,
  ARAD_PP_FRWRD_IPV6_MC_ILLEGAL_DEST_TYPE_ERR,
  ARAD_PP_FRWRD_IP_TCAM_ENTRY_DOESNT_EXIST_ERR,

  
  ARAD_PP_FRWRD_IPV6_ERR_LAST
} ARAD_PP_FRWRD_IPV6_ERR;









uint32
  arad_pp_frwrd_ipv6_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_ipv6_uc_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO          *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv6_uc_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO            *route_info
  );


uint32
  arad_pp_frwrd_ipv6_uc_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO            *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_frwrd_ipv6_uc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );


uint32
  arad_pp_frwrd_ipv6_uc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO            *route_infos,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_ipv6_uc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key
  );


uint32
  arad_pp_frwrd_ipv6_uc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv6_uc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY             *route_key
  );


uint32
  arad_pp_frwrd_ipv6_uc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_frwrd_ipv6_uc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_ipv6_mc_route_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  );

uint32
  arad_pp_frwrd_ipv6_mc_route_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *route_info
  );



uint32
  arad_pp_frwrd_ipv6_mc_compress_sip_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  );



uint32
  arad_pp_frwrd_ipv6_mc_route_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                             exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  );

uint32
  arad_pp_frwrd_ipv6_mc_route_get_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                             exact_match
  );


uint32
  arad_pp_frwrd_ipv6_mc_compress_sip_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                             exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  );


uint32
  arad_pp_frwrd_ipv6_mc_route_get_block_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *routes_location,
    SOC_SAND_OUT uint32                              *nof_entries
  );

uint32
  arad_pp_frwrd_ipv6_mc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key
  );


uint32
  arad_pp_frwrd_ipv6_mc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv6_mc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY             *route_key
  );


uint32
  arad_pp_frwrd_ipv6_mc_compress_sip_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );


uint32
  arad_pp_frwrd_ipv6_mc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_frwrd_ipv6_mc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_ipv6_vrf_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VRF_INFO                 *vrf_info
  );

uint32
  arad_pp_frwrd_ipv6_vrf_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VRF_INFO                 *vrf_info
  );

uint32
  arad_pp_frwrd_ipv6_vrf_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  );


uint32
  arad_pp_frwrd_ipv6_vrf_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_VRF_INFO                 *vrf_info
  );


uint32
  arad_pp_frwrd_ipv6_vrf_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv6_vrf_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO            *route_info
  );


uint32
  arad_pp_frwrd_ipv6_vrf_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO                 *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_frwrd_ipv6_vrf_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );


uint32
  arad_pp_frwrd_ipv6_vrf_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO                 *route_infos,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_ipv6_vrf_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key
  );


uint32
  arad_pp_frwrd_ipv6_vrf_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv6_vrf_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key
  );


uint32
  arad_pp_frwrd_ipv6_vrf_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  );

uint32
  arad_pp_frwrd_ipv6_vrf_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  );


uint32
  arad_pp_frwrd_ipv6_vrf_all_routing_tables_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_frwrd_ipv6_vrf_all_routing_tables_clear_verify(
    SOC_SAND_IN  int                                 unit
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_ipv6_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_ipv6_get_errs_ptr(void);

uint32
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL_verify(
    SOC_SAND_IN  int                                          unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL *info
  );

uint32
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION *info
  );

uint32
  SOC_PPC_FRWRD_IPV6_ROUTER_INFO_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_ROUTER_INFO *info
  );

uint32
  SOC_PPC_FRWRD_IPV6_VRF_INFO_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VRF_INFO *info
  );

uint32
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY *info
  );

uint32
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY *info
  );

uint32
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY *info
  );

uint32
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO *info
  );

uint32 SOC_PPC_FRWRD_IPV6_MC_RAW_ROUTE_INFO_verify(
    SOC_SAND_IN int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO *info
  );

void
  arad_pp_frwrd_ipv6_prefix_to_mask(
    SOC_SAND_IN  uint32 prefix,
    SOC_SAND_OUT uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S]
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

