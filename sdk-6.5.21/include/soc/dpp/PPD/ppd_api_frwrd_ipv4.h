/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_FRWRD_IPV4_INCLUDED__

#define __SOC_PPD_API_FRWRD_IPV4_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>













typedef enum
{
  
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET = SOC_PPD_PROC_DESC_BASE_FRWRD_IPV4_FIRST,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_PRINT,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_PRINT,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_COMMIT,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_COMMIT_PRINT,
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_SET,
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_GET,
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_ADD,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_GET,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV4_HOST_ADD,
  SOC_PPD_FRWRD_IPV4_HOST_ADD_PRINT,
  SOC_PPD_FRWRD_IPV4_HOST_GET,
  SOC_PPD_FRWRD_IPV4_HOST_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_HOST_GET_BLOCK,
  SOC_PPD_FRWRD_IPV4_HOST_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV4_HOST_REMOVE,
  SOC_PPD_FRWRD_IPV4_HOST_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_ADD,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_GET,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_INFO_SET,
  SOC_PPD_FRWRD_IPV4_VRF_INFO_SET_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_INFO_GET,
  SOC_PPD_FRWRD_IPV4_VRF_INFO_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_ADD,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_GET,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR,
  SOC_PPD_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV4_MEM_STATUS_GET,
  SOC_PPD_FRWRD_IPV4_MEM_STATUS_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_MEM_DEFRAGE,
  SOC_PPD_FRWRD_IPV4_MEM_DEFRAGE_PRINT,
  SOC_PPD_FRWRD_IPV4_GET_PROCS_PTR,
  
  
  SOC_PPD_FRWRD_IPV4_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_IPV4_PROCEDURE_DESC;










uint32
  soc_ppd_frwrd_ip_routes_cache_mode_enable_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                route_types
  );


uint32
  soc_ppd_frwrd_ip_routes_cache_mode_enable_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT uint32                                *route_types
  );


uint32
  soc_ppd_frwrd_ip_routes_cache_commit(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                route_types,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_frwrd_ipv4_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );


uint32
  soc_ppd_frwrd_ipv4_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );


uint32
  soc_ppd_frwrd_ipv4_uc_route_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_frwrd_ipv4_uc_route_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                               exact_match,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_frwrd_ipv4_uc_route_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                *nof_entries
  );


uint32
  soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS        table_id,
    SOC_SAND_OUT int                             *is_table_initiated
  );


uint32
  soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS        table_id,
    SOC_SAND_IN  uint32                           flags,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL              qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
    SOC_SAND_OUT void                            *payload,
    SOC_SAND_OUT uint32                          *priority,
    SOC_SAND_OUT uint8                           *hit_bit,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_frwrd_ipv4_uc_route_iterator_init(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS        table_id,
    SOC_SAND_OUT SOC_DPP_DBAL_PHYSICAL_DB_TYPES    *physical_db_type
  );


uint32
  soc_ppd_frwrd_ipv4_uc_route_iterator_deinit(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS        table_id
  );


uint32
  soc_ppd_frwrd_ipv4_uc_route_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_frwrd_ipv4_uc_routing_table_clear(
    SOC_SAND_IN  int                               unit
  );


uint32
  soc_ppd_frwrd_ipv4_host_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_ipv4_host_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_frwrd_ipv4_host_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                            *nof_entries
  );


uint32
  soc_ppd_frwrd_ipv4_host_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key
  );


uint32
  soc_ppd_frwrd_ipv4_mc_route_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_ipv4_mc_route_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                           flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_frwrd_ipv4_mc_route_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY         *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *routes_location,
    SOC_SAND_OUT uint32                            *nof_entries
  );


uint32
  soc_ppd_frwrd_ipv4_mc_route_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_frwrd_ipv4_mc_routing_table_clear(
    SOC_SAND_IN  int                               unit
  );



uint32
  soc_ppd_frwrd_ipmc_rp_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  );



uint32
  soc_ppd_frwrd_ipmc_rp_get_block(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  rp_id,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range_key,
    SOC_SAND_OUT int32                                  *l3_intfs,
    SOC_SAND_OUT uint32                                  *nof_entries
  );


uint32
  soc_ppd_frwrd_ipmc_rp_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  );



uint32
  soc_ppd_frwrd_ipv4_vrf_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );


uint32
  soc_ppd_frwrd_ipv4_vrf_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );


uint32
  soc_ppd_frwrd_ipv4_vrf_route_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_IN  uint32                                      flags,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_frwrd_ipv4_vrf_route_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                               exact_match,
    SOC_SAND_IN  uint32                                      flags,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_frwrd_ipv4_vrf_route_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                *nof_entries
  );


uint32
  soc_ppd_frwrd_ipv4_vrf_route_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_frwrd_ipv4_vrf_routing_table_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  );


uint32
  soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                 flags
  );


uint32
  soc_ppd_frwrd_ipv4_mem_status_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       mem_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MEM_STATUS      *mem_status
  );


uint32
  soc_ppd_frwrd_ipv4_mem_defrage(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       mem_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  );


uint32
  soc_ppd_frwrd_ipv4_mc_ssm_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY         *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *routes_location,
    SOC_SAND_OUT uint32                            *nof_entries
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

