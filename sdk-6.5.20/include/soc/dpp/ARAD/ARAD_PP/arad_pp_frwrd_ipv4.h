
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FRWRD_IPV4_INCLUDED__

#define __ARAD_PP_FRWRD_IPV4_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>





#define ARAD_PP_IPV4_VRF_BITMAP_SIZE (128)

#define JER_PP_KAPS_PAYLOAD_IS_FEC(_payload)     (_payload & (1 << 17))
#define JER_PP_KAPS_PAYLOAD_IS_MC(_payload)      (_payload & (1 << 18))
#define JER_PP_KAPS_PAYLOAD_IS_DEFAULT(_payload) (_payload & (1 << 19))

#define JER_PP_KAPS_FEC_ENCODE(_payload)     (_payload | (1 << 17))
#define JER_PP_KAPS_MC_ENCODE(_payload)      (_payload | (1 << 18))
#define JER_PP_KAPS_DEFAULT_ENCODE(_payload) (_payload | (1 << 19))

#define JER_PP_KAPS_FEC_DECODE(_payload)     (_payload & ~(1 << 17))
#define JER_PP_KAPS_MC_DECODE(_payload)      (_payload & ~(1 << 18))
#define JER_PP_KAPS_DEFAULT_DECODE(_payload) (_payload & ~(1 << 19))










typedef enum
{
  
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET = ARAD_PP_PROC_DESC_BASE_FRWRD_IPV4_FIRST,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_PRINT,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_UNSAFE,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_VERIFY,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_PRINT,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_VERIFY,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_UNSAFE,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_PRINT,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_UNSAFE,
  ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_VERIFY,
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET,
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET_PRINT,
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET,
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET_PRINT,
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD_PRINT,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD_VERIFY,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_PRINT,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_VERIFY,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_PRINT,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR,
  ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_PRINT,
  ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_VERIFY,
  ARAD_PP_FRWRD_IPV4_HOST_ADD,
  ARAD_PP_FRWRD_IPV4_HOST_ADD_PRINT,
  ARAD_PP_FRWRD_IPV4_HOST_ADD_UNSAFE,
  ARAD_PP_FRWRD_IPV4_HOST_ADD_VERIFY,
  ARAD_PP_FRWRD_IPV4_HOST_GET,
  ARAD_PP_FRWRD_IPV4_HOST_GET_PRINT,
  ARAD_PP_FRWRD_IPV4_HOST_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV4_HOST_GET_VERIFY,
  ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK,
  ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_IPV4_HOST_REMOVE,
  ARAD_PP_FRWRD_IPV4_HOST_REMOVE_PRINT,
  ARAD_PP_FRWRD_IPV4_HOST_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_IPV4_HOST_REMOVE_VERIFY,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD_PRINT,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD_VERIFY,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_PRINT,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_VERIFY,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_PRINT,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR,
  ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_PRINT,
  ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_VERIFY,
  SOC_PPC_FRWRD_IPV4_VRF_INFO_SET,
  SOC_PPC_FRWRD_IPV4_VRF_INFO_SET_PRINT,
  SOC_PPC_FRWRD_IPV4_VRF_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_IPV4_VRF_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_IPV4_VRF_INFO_GET,
  SOC_PPC_FRWRD_IPV4_VRF_INFO_GET_PRINT,
  SOC_PPC_FRWRD_IPV4_VRF_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_IPV4_VRF_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD_PRINT,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD_VERIFY,
  ARAD_PP_FRWRD_IPV4_VRF_IS_SUPPORTED_VERIFY,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_PRINT,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_VERIFY,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_PRINT,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_PRINT,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_VERIFY,
  ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR,
  ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT,
  ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY,
  SOC_PPC_FRWRD_IPV4_MEM_STATUS_GET,
  SOC_PPC_FRWRD_IPV4_MEM_STATUS_GET_UNSAFE,
  SOC_PPC_FRWRD_IPV4_MEM_STATUS_GET_VERIFY,
  ARAD_PP_FRWRD_IPV4_MEM_DEFRAGE,
  ARAD_PP_FRWRD_IPV4_MEM_DEFRAGE_UNSAFE,
  ARAD_PP_FRWRD_IPV4_MEM_DEFRAGE_VERIFY,
  ARAD_PP_FRWRD_IPV4_GET_PROCS_PTR,
  ARAD_PP_FRWRD_IPV4_GET_ERRS_PTR,
  
  ARAD_PP_FRWRD_IPV4_DEF_ACTION_SET,
  ARAD_PP_FRWRD_IPV4_L3_VPN_DEFAULT_ROUTING_ENABLE_SET,
  ARAD_PP_FRWRD_IPV4_L3_VPN_DEFAULT_ROUTING_ENABLE_GET,
  ARAD_PP_FRWRD_IPV4_LPM_ROUTE_ADD,
  ARAD_PP_FRWRD_IPV4_LPM_ROUTE_IS_EXIST,
  ARAD_PP_FRWRD_IPV4_LPM_ROUTE_CLEAR,
  ARAD_PP_FRWRD_IPV4_LPM_ROUTE_REMOVE,
  ARAD_PP_FRWRD_IPV4_LPM_ROUTE_GET,
  ARAD_PP_FRWRD_IPV4_LPM_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_IPV4_LEM_ROUTE_ADD,
  ARAD_PP_FRWRD_IPV4_LEM_ROUTE_GET,
  ARAD_PP_FRWRD_IPV4_LEM_ROUTE_REMOVE,
  ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_ADD,
  ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET,
  ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_REMOVE,
  ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTING_TABLE_CLEAR,


  ARAD_PP_FRWRD_IPV4_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_IPV4_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_IPV4_FIRST,
  ARAD_PP_FRWRD_IPV4_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_ROUTE_STATUS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_LOCATION_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_FOUND_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_ROUTES_STATUS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_ROUTES_LOCATION_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_EXACT_MATCH_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_ROUTE_VAL_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
  SOC_PPC_FRWRD_IP_CACHE_MODE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_GROUP_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_IPV4_EEP_OUT_OF_RANGE_ERR,
  
  ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,
  ARAD_PP_FRWRD_IPV4_MUST_ALL_VRF_ERR,
  ARAD_PP_FRWRD_IPV4_CACHE_NOT_SUPPORTED_ERR,
  ARAD_PP_FRWRD_IPV4_DEFRAGE_NOT_SUPPORTED_ERR,
  ARAD_PP_FRWRD_IPV4_OPERATION_NOT_SUPPORTED_ON_VRF_ERR,

  
  ARAD_PP_FRWRD_IPV4_ERR_LAST
} ARAD_PP_FRWRD_IPV4_ERR;


typedef struct
{
  uint32
    default_fec;
  uint32
    nof_lpm_entries_in_lpm;
  uint32
    nof_vrfs;
  SHR_BITDCL
    vrf_modified_bitmask[ARAD_PP_IPV4_VRF_BITMAP_SIZE];
  uint8
    cache_modified;
  uint32
    cache_mode;
#ifdef SOC_SAND_DEBUG
  
  uint8
    lem_add_fail;
#endif

} ARAD_PP_IPV4_INFO;









uint32
  arad_pp_frwrd_ipv4_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_ip_routes_cache_mode_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                  route_types
  );

uint32
  arad_pp_frwrd_ip_routes_cache_mode_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                  route_types
  );

uint32
  arad_pp_frwrd_ip_routes_cache_mode_enable_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  );


uint32
  arad_pp_frwrd_ip_routes_cache_mode_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT uint32                                  *route_types
  );


uint32
  arad_pp_frwrd_ip_routes_cache_commit_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ip_routes_cache_commit_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  );


uint32
  arad_pp_frwrd_ipv4_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );

uint32
  arad_pp_frwrd_ipv4_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );

uint32
  arad_pp_frwrd_ipv4_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_ipv4_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );


uint32
  arad_pp_frwrd_ipv4_uc_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv4_uc_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id
  );


uint32
  arad_pp_frwrd_ipv4_uc_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_frwrd_ipv4_uc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );


uint32
  arad_pp_frwrd_ipv4_uc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_ipv4_uc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range
  );


uint32
  arad_pp_frwrd_ipv4_uc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv4_uc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key
  );


uint32
  arad_pp_frwrd_ipv4_uc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_frwrd_ipv4_uc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_ipv4_host_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  );

uint32
  arad_pp_frwrd_ipv4_host_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info
  );


uint32
  arad_pp_frwrd_ipv4_host_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  );

uint32
  arad_pp_frwrd_ipv4_host_get_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key
  );


uint32
  arad_pp_frwrd_ipv4_host_get_block_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                              *nof_entries
  );

uint32
  arad_pp_frwrd_ipv4_host_get_block_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key
  );


uint32
  arad_pp_frwrd_ipv4_host_remove_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key
  );

uint32
  arad_pp_frwrd_ipv4_host_remove_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key
  );


uint32
  arad_pp_frwrd_ipv4_mc_route_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  );

uint32
  arad_pp_frwrd_ipv4_mc_route_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *route_info
  );


uint32
  arad_pp_frwrd_ipv4_mc_route_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                             exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  );

uint32
  arad_pp_frwrd_ipv4_mc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );


uint32
  arad_pp_frwrd_ipv4_mc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_ipv4_mc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key
  );


uint32
  arad_pp_frwrd_ipv4_mc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv4_mc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key
  );


uint32
  arad_pp_frwrd_ipv4_mc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_frwrd_ipv4_mc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_ipmc_rp_add_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  );

uint32
  arad_pp_frwrd_ipmc_rp_get_block_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  rp_id,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range_key,
    SOC_SAND_OUT int32                                  *l3_intfs,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_ipmc_rp_remove_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  );

uint32
  arad_pp_frwrd_ipmc_rp_add_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  );

uint32
  arad_pp_frwrd_ipmc_rp_get_block_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  rp_id,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range_key,
    SOC_SAND_OUT int32                                  *l3_intfs,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_ipmc_rp_remove_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  );



uint32
  arad_pp_frwrd_ipv4_vrf_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );

uint32
  arad_pp_frwrd_ipv4_vrf_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );

uint32
  arad_pp_frwrd_ipv4_vrf_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  );


uint32
  arad_pp_frwrd_ipv4_vrf_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );


uint32
  arad_pp_frwrd_ipv4_vrf_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_IN  uint32                                      flags,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv4_vrf_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_IN  uint32                                      flags
  );


uint32
  arad_pp_frwrd_ipv4_vrf_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_IN  uint32                                      flags,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_frwrd_ipv4_vrf_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );


uint32
  arad_pp_frwrd_ipv4_vrf_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_ipv4_vrf_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key
  );


uint32
  arad_pp_frwrd_ipv4_vrf_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_ipv4_vrf_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key
  );


uint32
  arad_pp_frwrd_ipv4_vrf_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  );

uint32
  arad_pp_frwrd_ipv4_vrf_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  );


uint32
  arad_pp_frwrd_ipv4_vrf_all_routing_tables_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 flags
  );

uint32
  arad_pp_frwrd_ipv4_vrf_all_routing_tables_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 flags
  );

uint32
    arad_pp_frwrd_em_dest_to_fec(
      SOC_SAND_IN int                 unit,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_PAYLOAD  *payload,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO *route_info
    );



uint32
  arad_pp_frwrd_ipv4_mem_status_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MEM_STATUS      *mem_status
  );

uint32
  arad_pp_frwrd_ipv4_mem_status_get_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx
  );


uint32
  arad_pp_frwrd_ipv4_mem_defrage_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  );

uint32
  arad_pp_frwrd_ipv4_mem_defrage_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_ipv4_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_ipv4_get_errs_ptr(void);

uint32
    arad_pp_frwrd_ipv4_host_lem_payload_build(
      SOC_SAND_IN int                               unit,
      SOC_SAND_IN SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO   *routing_info,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD          *payload
    );

uint32
    arad_pp_frwrd_ipv4_em_dest_to_sand_dest(
      SOC_SAND_IN int               unit,
      SOC_SAND_IN uint32                dest,
      SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID *dest_id
    );
uint32
    arad_pp_frwrd_ipv4_sand_dest_to_em_dest(
      SOC_SAND_IN int              unit,
      SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *dest_id,
      SOC_SAND_OUT uint32 *dest
    );
uint32
    arad_pp_frwrd_ipv4_sand_dest_to_fwd_decision(
      SOC_SAND_IN int              unit,
      SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *dest_id,
      SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *em_dest
    );

#if defined(INCLUDE_KBP)
uint32
    arad_pp_frwrd_ipv4_uc_rpf_kaps_dbal_route_get(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                               vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET              *route_key,
      SOC_SAND_IN  uint8                                route_scale,
      SOC_SAND_INOUT uint8                              *hit_bit,
      SOC_SAND_OUT uint32                               *fec_id,
      SOC_SAND_OUT uint8                                *found
      );
#endif




soc_error_t
  arad_pp_frwrd_ip_ipmc_ssm_add(
     SOC_SAND_IN  int unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
     SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
     SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE* success
   );

soc_error_t
  arad_pp_frwrd_ip_ipmc_ssm_delete(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key
   );

soc_error_t
  arad_pp_frwrd_ip_ipmc_ssm_get(
     SOC_SAND_IN  int unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
     SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
     SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS*    route_status,
     SOC_SAND_OUT uint8* found
   );




uint32
  arad_pp_frwrd_ip_ipmc_ssm_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

soc_error_t
  arad_pp_frwrd_ip_ipmc_ssm_clear(
    SOC_SAND_IN  int                                 unit
  );


uint32
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_ROUTER_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_INFO *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_VRF_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VRF_INFO *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_GLBL_INFO *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_HOST_KEY_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_MC_RAW_ROUTE_INFO_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO *info
  );

uint32
  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



