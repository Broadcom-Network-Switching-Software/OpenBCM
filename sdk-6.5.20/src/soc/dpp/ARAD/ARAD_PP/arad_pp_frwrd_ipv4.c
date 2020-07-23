
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD
#include <soc/mem.h>




#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
 
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_lpm_mngr.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#if defined(INCLUDE_KBP)
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#endif
#include <soc/dpp/drv.h>





#define ARAD_PP_FRWRD_IPV4_ROUTE_TYPES_MAX                       (SOC_PPC_NOF_FRWRD_IP_CACHE_MODES-1)
#define ARAD_PP_FRWRD_IPV4_MEM_NDX_MIN                           (1)
#define ARAD_PP_FRWRD_IPV4_MEM_NDX_MAX                           (5)
#define ARAD_PP_FRWRD_IPV4_ROUTE_VAL_MAX                         (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_IPV4_TYPE_MAX                              (SOC_PPC_NOF_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPES-1)
#define ARAD_PP_FRWRD_IPV4_GROUP_MIN                             (0xE0000000)
#define ARAD_PP_FRWRD_IPV4_GROUP_MAX                             (0xEFFFFFFF)
#define ARAD_PP_FRWRD_IPV4_GET_BLOCK_LEM_NOF_ENTRIES             (130)
#define ARAD_PP_FRWRD_IPV4_LEM_ENTRIES_TO_SCAN                    SOC_SAND_TBL_ITER_SCAN_ALL



#define ARAD_PP_IHB_CONFIG_TBL_NDX_BUILD(vrf_ndx, is_mcast) (vrf_ndx)


 
#define ARAD_PP_FRWRD_IPV4_HOST_TABLE_PAYLOAD_ASD_HI_NOF_BITS                       (4)















CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_frwrd_ipv4[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_HOST_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_VRF_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_VRF_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_VRF_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_VRF_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_VRF_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_VRF_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_VRF_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_VRF_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_IS_SUPPORTED_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_MEM_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_MEM_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_IPV4_MEM_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MEM_DEFRAGE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MEM_DEFRAGE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_MEM_DEFRAGE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_GET_ERRS_PTR),
  

   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_DEF_ACTION_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_L3_VPN_DEFAULT_ROUTING_ENABLE_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_L3_VPN_DEFAULT_ROUTING_ENABLE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_ADD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_IS_EXIST),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_CLEAR),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_GET_BLOCK),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_LEM_ROUTE_ADD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_LEM_ROUTE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_LEM_ROUTE_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_ADD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET_BLOCK),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTING_TABLE_CLEAR),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_frwrd_ipv4[] =
{
  
  {
    ARAD_PP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR",
    "The parameter 'route_types' is out of range. \n\r "
    "The range is: NONE or LPM .\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_ROUTE_STATUS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_ROUTE_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'route_status' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_IP_ROUTE_STATUSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_LOCATION_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_LOCATION_OUT_OF_RANGE_ERR",
    "The parameter 'location' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_FOUND_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_FOUND_OUT_OF_RANGE_ERR",
    "The parameter 'found' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_ROUTES_STATUS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_ROUTES_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'routes_status' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_IP_ROUTE_STATUSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_ROUTES_LOCATION_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_ROUTES_LOCATION_OUT_OF_RANGE_ERR",
    "The parameter 'routes_location' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_IP_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_EXACT_MATCH_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_EXACT_MATCH_OUT_OF_RANGE_ERR",
    "The parameter 'exact_match' is out of range. \n\r "
    "The range is: 1 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mem_ndx' is out of range. \n\r "
    "The range is: 0 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_ROUTE_VAL_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_ROUTE_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'route_val' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR",
    "The parameter 'uc_table_resouces' is out of range. \n\r "
    "for Arad-B it has to be LPM only, to add to host use host APIs.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR",
    "The parameter 'mc_table_resouces' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_IPV4_MC_HOST_TABLE_RESOURCES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FRWRD_IP_CACHE_MODE_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_IP_CACHE_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'mc_table_resouces' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_IP_CACHE_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_GROUP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_GROUP_OUT_OF_RANGE_ERR",
    "The parameter 'group' is out of range. \n\r "
    "The range is: 224.0.0.0 - 239.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
   {
    ARAD_PP_FRWRD_IPV4_EEP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_IPV4_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,
    "ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR",
    "Destination in IPv4 MC routing info can be \n\r"
    "FEC-ptr or MC-group only\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_MUST_ALL_VRF_ERR,
    "ARAD_PP_FRWRD_IPV4_MUST_ALL_VRF_ERR",
    "operation can be done for all-VRFs and not one VRF \n\r"
    "set vrf_ndx = SOC_PPC_FRWRD_IP_ALL_VRFS_ID\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_CACHE_NOT_SUPPORTED_ERR,
    "ARAD_PP_FRWRD_IPV4_CACHE_NOT_SUPPORTED_ERR",
    "caching is not supported (supplied in operation mode)\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_DEFRAGE_NOT_SUPPORTED_ERR,
    "ARAD_PP_FRWRD_IPV4_DEFRAGE_NOT_SUPPORTED_ERR",
    "defragment is not supported (supplied in operation mode)\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_IPV4_OPERATION_NOT_SUPPORTED_ON_VRF_ERR,
    "ARAD_PP_FRWRD_IPV4_OPERATION_NOT_SUPPORTED_ON_VRF_ERR",
    "operation is not supported on single VRF, use SOC_PPC_FRWRD_IP_ALL_VRFS_ID \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },




  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};






STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_rpf_dbal_route_add(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY      *route_key,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO     *route_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
    );

STATIC
  uint32
    arad_pp_frwrd_ipv4_def_action_get(
      SOC_SAND_IN int                                     unit,
      SOC_SAND_IN uint32                                      vrf_ndx,
      SOC_SAND_IN uint8                                     is_mcast,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION          *def_action
    );

STATIC
  uint32
    arad_pp_frwrd_ipv4_def_action_set(
      SOC_SAND_IN int                                     unit,
      SOC_SAND_IN uint32                                      vrf_ndx,
      SOC_SAND_IN uint8                                     is_mcast,
      SOC_SAND_IN SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION          *def_action
    );


uint32
  arad_pp_frwrd_ipv4_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK,
    vrf;
  SOC_PPC_FRWRD_IPV4_GLBL_INFO
    glbl_info;
  SOC_PPC_FRWRD_IPV4_VRF_INFO
    vrf_info;
  uint32
    nof_vrfs;
#ifndef PLISIM
  uint32
    data;
#endif
  uint32
    fld_val;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

 
  fld_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_RPF_CFGr, REG_PORT_ANY, 0, RPF_ALLOW_DEFAULT_VALUEf,  fld_val));

  if (SOC_IS_JERICHO(unit)) {
      
      ARAD_DO_NOTHING_AND_EXIT;
  }

  SOC_PPC_FRWRD_IPV4_GLBL_INFO_clear(&glbl_info);
  glbl_info.mc_table_resouces = SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_TCAM_ONLY;
  glbl_info.uc_table_resouces = SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_ONLY;
  glbl_info.router_info.uc_default_action.type = SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
  glbl_info.router_info.uc_default_action.value.route_val = 0;
  glbl_info.router_info.mc_default_action.type = SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
  glbl_info.router_info.mc_default_action.value.route_val = 0;

  res = arad_pp_frwrd_ipv4_def_action_set(
          unit,
          0,
          FALSE,
          &glbl_info.router_info.uc_default_action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_frwrd_ipv4_def_action_set(
          unit,
          0,
          TRUE,
          &glbl_info.router_info.mc_default_action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PPC_FRWRD_IPV4_VRF_INFO_clear(&vrf_info);
  vrf_info.use_dflt_non_vrf_routing = FALSE;
  vrf_info.router_info.uc_default_action.type = SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
  vrf_info.router_info.uc_default_action.value.route_val = 0;
  vrf_info.router_info.mc_default_action.type = SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
  vrf_info.router_info.mc_default_action.value.route_val = 0;

  res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.get(
          unit,
          &nof_vrfs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  if(nof_vrfs == 0) {
      goto exit;
  }

  for(vrf = 1; vrf <= nof_vrfs-1; vrf++)
  {
    res = arad_pp_frwrd_ipv4_def_action_set(
            unit,
            vrf,
            FALSE,
            &vrf_info.router_info.uc_default_action
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_frwrd_ipv4_def_action_set(
            unit,
            vrf,
            TRUE,
            &vrf_info.router_info.mc_default_action
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  }

  
#ifndef PLISIM
  if (!SAL_BOOT_PLISIM) {
  data = 0x70000000;
  res = arad_fill_table_with_entry(unit, IHB_LPMm, MEM_BLOCK_ANY, &data); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  }
#endif

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_init_unsafe()", 0, 0);
}

STATIC void
  arad_pp_ipv4_unicast_mask_ip(
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET       *route_key,
    SOC_SAND_OUT  SOC_SAND_PP_IPV4_SUBNET       *masked_route_key
  )
{
  masked_route_key->prefix_len = route_key->prefix_len;
  if (route_key->prefix_len != 0)
  {

    masked_route_key->ip_address = route_key->ip_address &
      SOC_SAND_BITS_MASK(
        SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN - 1,
        SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN - route_key->prefix_len
      );
  }
  else
  {
    masked_route_key->ip_address = 0;
  }
  return;
}


STATIC
  uint32
    arad_pp_frwrd_fec_to_em_dest(
      SOC_SAND_IN int              unit,
      SOC_SAND_IN SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
      SOC_SAND_OUT uint32 *dest,
      SOC_SAND_OUT uint32 *add_info
    )
{
  uint32
    res,
    add_info_lcl[1];


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 

  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          &(routing_info->frwrd_decision),
          dest,
          add_info_lcl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  *add_info = *add_info_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_fec_to_em_dest()",0,0);
}

uint32
    arad_pp_frwrd_ipv4_sand_dest_to_fwd_decision(
      SOC_SAND_IN int              unit,
      SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *dest_id,
      SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *em_dest
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_DECISION_INFO_clear(em_dest);

  if (dest_id->dest_type == SOC_SAND_PP_DEST_FEC)
  {
    ARAD_PP_FRWRD_DECISION_FEC_SET(unit, em_dest,dest_id->dest_val);
  }
  else if (dest_id->dest_type == SOC_SAND_PP_DEST_MULTICAST)
  {
    ARAD_PP_FRWRD_DECISION_MC_GROUP_SET(unit, em_dest,dest_id->dest_val);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_sand_dest_to_fwd_decision()",0,0);
}


uint32
    arad_pp_frwrd_ipv4_sand_dest_to_em_dest(
      SOC_SAND_IN int              unit,
      SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *dest_id,
      SOC_SAND_OUT uint32 *dest
    )
{
  SOC_PPC_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res,
    add_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_DECISION_INFO_clear(&em_dest);

  res = arad_pp_frwrd_ipv4_sand_dest_to_fwd_decision(
          unit,
          dest_id,
          &em_dest
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);
  
  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          &em_dest,
          dest,
          &add_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_sand_dest_to_em_dest()",0,0);
}

  

uint32
    arad_pp_frwrd_em_dest_to_fec(
      SOC_SAND_IN int                 unit,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_PAYLOAD  *payload,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO *route_info
    )
{
  uint32
    flags = ARAD_PP_FWD_DECISION_PARSE_LEGACY;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  flags |= (payload->flags & ARAD_PP_FWD_DECISION_PARSE_EEI);
  
  flags |= (payload->flags & ARAD_PP_FWD_DECISION_PARSE_OUTLIF);
  
  res = arad_pp_fwd_decision_in_buffer_parse(
          unit,          
          payload->dest,
          payload->asd,
          flags,
          &route_info->frwrd_decision
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_em_dest_to_fec()",0,0);
}


uint32
    arad_pp_frwrd_ipv4_em_dest_to_sand_dest(
      SOC_SAND_IN int               unit,
      SOC_SAND_IN uint32                dest,
      SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID *dest_id
    )
{
  SOC_PPC_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_DECISION_INFO_clear(&em_dest);

  res = arad_pp_fwd_decision_in_buffer_parse(
          unit,          
          dest,
          0,
          ARAD_PP_FWD_DECISION_PARSE_DEST,
          &em_dest
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (em_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC)
  {
    dest_id->dest_type = SOC_SAND_PP_DEST_FEC;
    dest_id->dest_val = em_dest.dest_id;
  }
  else if (em_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_MC)
  {
    dest_id->dest_type = SOC_SAND_PP_DEST_MULTICAST;
    dest_id->dest_val = em_dest.dest_id;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_em_dest_to_sand_dest()",0,0);
}


STATIC
  void
    arad_pp_frwrd_ipv4_host_lem_request_key_build(
       SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY_TYPE type,
      SOC_SAND_IN uint32 vrf_ndx,
      SOC_SAND_IN uint32 dip,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY *key
    )
{
  uint32
    num_bits;

  ARAD_PP_LEM_ACCESS_KEY_clear(key);

  key->type = type;

  key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IP_HOST;

  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = dip;
  
  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST;
  key->param[1].nof_bits = (uint8)num_bits;
  key->param[1].value[0] = vrf_ndx;

  key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST;
  key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_IP_HOST;
}


STATIC
  void
    arad_pp_frwrd_ipv4_host_lem_request_key_parse(
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_KEY *host_key
    )
{
  host_key->ip_address = key->param[0].value[0];
  host_key->vrf_ndx = key->param[1].value[0];
}


uint32
    arad_pp_frwrd_ipv4_host_lem_payload_build(
      SOC_SAND_IN int                               unit,  
      SOC_SAND_IN SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO   *routing_info,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD          *payload
    )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);

  res = arad_pp_frwrd_fec_to_em_dest(
          unit,
          routing_info,
          &payload->dest,
          &payload->asd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  
  
  if (routing_info->mac_lsb > 0) {
      uint32 mac_lsb;

      mac_lsb = routing_info->mac_lsb;

       
      ARAD_PP_LEM_ACCESS_HI_TO_EEI(mac_lsb, payload->asd);
  }

  
  if (routing_info->native_vsi != 0) {

      payload->flags = ARAD_PP_FWD_DECISION_PARSE_FORMAT_3B;
      
      payload->native_vsi = routing_info->native_vsi;
  }
  
  else if(routing_info->frwrd_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE) {
      payload->flags = ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
  }
  
  else if(routing_info->frwrd_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
      payload->flags = ARAD_PP_FWD_DECISION_PARSE_EEI;
  }
  else{
      payload->flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
  }
  
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_host_lem_payload_build()",0,0);

}






STATIC
  void
    arad_pp_frwrd_ipmc_rp_lem_request_key_build(
       SOC_SAND_IN int                       unit,
      SOC_SAND_IN SOC_PPC_FRWRD_IP_MC_RP_INFO   *rp_info,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *key
    )
{

  ARAD_PP_LEM_ACCESS_KEY_clear(key);
  
  key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_IPMC_BIDIR;

  key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IPMC_BIDIR;

  key->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPMC_BIDIR;
  key->param[0].value[0] = rp_info->inrif;
  
  key->param[1].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPMC_BIDIR;
  key->param[1].value[0] = rp_info->rp_id;

  key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPMC_BIDIR;
  key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_IPMC_BIDIR;
   

}


STATIC
  void
    arad_pp_frwrd_ipmc_rp_lem_request_key_parse(
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT SOC_PPC_FRWRD_IP_MC_RP_INFO *rp_info
    )
{
  rp_info->inrif = key->param[0].value[0];
  rp_info->rp_id = key->param[1].value[0];
}


STATIC
  uint32
    arad_pp_frwrd_ipv4_def_action_set(
      SOC_SAND_IN int                                     unit,
      SOC_SAND_IN uint32                                      vrf_ndx,
      SOC_SAND_IN uint8                                     is_mcast,
      SOC_SAND_IN SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION          *def_action
    )
{
  ARAD_PP_IHB_VRF_CONFIG_TBL_DATA
    tbl_data;
  SOC_PPC_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res,
    ndx,
    dest,
    old_dest=0,
    add_info;
#if defined(INCLUDE_KBP)
  uint8 table_id;
  uint32 success;
  uint32 payload;
  SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
#endif
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_DEF_ACTION_SET);

  
  ndx = ARAD_PP_IHB_CONFIG_TBL_NDX_BUILD(
          vrf_ndx,
          is_mcast
        );

  
  SOC_PPC_FRWRD_DECISION_INFO_clear(&em_dest);

  switch(def_action->type)
  {
    case SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE:
      em_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_TRAP;
      em_dest.additional_info.trap_info.action_profile.frwrd_action_strength = def_action->value.action_profile.frwrd_action_strength;
      em_dest.additional_info.trap_info.action_profile.snoop_action_strength = def_action->value.action_profile.snoop_action_strength;
      em_dest.additional_info.trap_info.action_profile.trap_code = def_action->value.action_profile.trap_code;
      break;
    case SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC:
      em_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
      em_dest.dest_id = def_action->value.route_val;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_DEFAULT_ACTIION_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }

  em_dest.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
  em_dest.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;

  res = arad_pp_fwd_decision_in_buffer_build(unit, ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT, &em_dest, &dest, &add_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
	  res = arad_pp_ihb_vrf_config_tbl_get_unsafe(
			  unit,
			  ndx,
			  &tbl_data
			);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	  
	  if(!is_mcast) 
	  {
		  old_dest = tbl_data.default_action_uc;
		  tbl_data.default_action_uc = dest;
	  }
	  else
	  {
		  old_dest = tbl_data.default_action_mc;
		  tbl_data.default_action_mc = dest;
	  }

	  if (old_dest != dest)
	  {
		res = arad_pp_ihb_vrf_config_tbl_set_unsafe(
				unit,
				ndx,
				&tbl_data
			  );
		SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
	  }
  }
#if defined(INCLUDE_KBP)
  else if (JER_KAPS_ENABLE(unit)) {
      qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_VRF;
      qual_vals[0].val.arr[0] = vrf_ndx;
      qual_vals[0].is_valid.arr[0] = SOC_SAND_U32_MAX;

      if(is_mcast)
      {
          table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS;
      }
      else
      {
          table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
      }
      payload = JER_PP_KAPS_DEFAULT_ENCODE(dest); 

      res = arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0, &payload, &success);
  }
#endif
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_def_action_set()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_def_action_get(
      SOC_SAND_IN int                                     unit,
      SOC_SAND_IN uint32                                      vrf_ndx,
      SOC_SAND_IN uint8                                     is_mcast,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION          *def_action
    )
{
  ARAD_PP_IHB_VRF_CONFIG_TBL_DATA
    tbl_data;
  SOC_PPC_FRWRD_DECISION_INFO
    em_dest;
  uint32
    dest,
    res,
    ndx;
#if defined(INCLUDE_KBP)
  uint8 table_id;
  uint8 found;
  uint32 payload;
  SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
#endif
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_DEF_ACTION_SET);

  
  ndx = ARAD_PP_IHB_CONFIG_TBL_NDX_BUILD(
          vrf_ndx,
          is_mcast
        );

#if defined(INCLUDE_KBP)
  if (JER_KAPS_ENABLE(unit)) {
      qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_VRF;
      qual_vals[0].val.arr[0] = vrf_ndx;
      qual_vals[0].is_valid.arr[0] = SOC_SAND_U32_MAX;

      if(is_mcast) 
      {
          table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS;
      }
      else
      {
          table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
      }

      res = arad_pp_dbal_entry_get(unit, table_id, qual_vals, &payload, 0, NULL, &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      dest = JER_PP_KAPS_DEFAULT_DECODE(payload);
  }
  else 
#endif 
  {
      res = arad_pp_ihb_vrf_config_tbl_get_unsafe(
              unit,
              ndx,
              &tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if(!is_mcast) 
      {
          dest = tbl_data.default_action_uc;
      }
      else
      {
          dest = tbl_data.default_action_mc;
      }
  }


  res = arad_pp_fwd_decision_in_buffer_parse(unit, dest, 0, ARAD_PP_FWD_DECISION_PARSE_EEI|ARAD_PP_FWD_DECISION_PARSE_DEST, &em_dest);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  switch(em_dest.type)
  {
    case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
      def_action->type =  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE;
      def_action->value.action_profile.frwrd_action_strength = em_dest.additional_info.trap_info.action_profile.frwrd_action_strength;
      def_action->value.action_profile.snoop_action_strength = em_dest.additional_info.trap_info.action_profile.snoop_action_strength;
      def_action->value.action_profile.trap_code = em_dest.additional_info.trap_info.action_profile.trap_code;
      break;
    case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
      def_action->type = SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
      def_action->value.route_val = em_dest.dest_id;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_GENERAL_FRWRD_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_def_action_get()",0,0);
}

#if defined(INCLUDE_KBP)

STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_rpf_kbp_key_mask_encode(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID             frwrd_table_id,
      SOC_SAND_IN  uint32                               vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET              *route_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *mask
    )
{
  uint32
      res,
      mask_full = SOC_SAND_U32_MAX;
  SOC_SAND_PP_IPV6_ADDRESS
      route_key_mask;
  SOC_SAND_PP_IPV4_SUBNET
      route_mask,
      masked_route_mask,
      masked_route_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_KEY_clear(data);
  ARAD_PP_LEM_ACCESS_KEY_clear(mask);
  sal_memset(&route_key_mask, 0x0, sizeof(SOC_SAND_PP_IPV6_ADDRESS));


  
  arad_pp_ipv4_unicast_mask_ip(route_key, &masked_route_key);

  route_mask.ip_address = 0xffffffff;
  route_mask.prefix_len = route_key->prefix_len;

  
  arad_pp_ipv4_unicast_mask_ip(&route_mask, &masked_route_mask);

  

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, masked_route_key.ip_address, data);
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, masked_route_mask.ip_address, mask);

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, vrf_ndx, data);
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, mask_full, mask);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kbp_key_mask_encode()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_rpf_kbp_key_mask_decode(
      SOC_SAND_IN  int                        unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID      frwrd_table_id,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *mask,
      SOC_SAND_OUT  uint32                      *vrf_ndx,
      SOC_SAND_OUT  SOC_SAND_PP_IPV4_SUBNET     *route_key
    )
{
  uint32
    bit_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(mask);
  SOC_SAND_CHECK_NULL_INPUT(vrf_ndx);
  SOC_SAND_CHECK_NULL_INPUT(route_key);

  soc_sand_SAND_PP_IPV4_SUBNET_clear(route_key);
  *vrf_ndx = 0;



  

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 0, &route_key->ip_address, data, mask);
  
  route_key->prefix_len = 0;
  for (bit_ndx = 0; bit_ndx < SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS; bit_ndx++) {
      if (SHR_BITGET(mask->param[0].value, bit_ndx)) {
          route_key->prefix_len = SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS - bit_ndx;
          break;
      }
  }

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 1, vrf_ndx, data, mask);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kbp_key_mask_decode()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_rpf_kbp_result_encode(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                               frwrd_table_id,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO   *routing_info,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD           *payload
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
      arad_pp_frwrd_ipv4_host_lem_payload_build(
        unit,
        routing_info,
        payload
      );

      ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kbp_result_encode()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_kbp_key_mask_encode(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID     frwrd_table_id,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY  *route_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *mask
    )
{
  uint32
    lcl_mask,
    mask_full = SOC_SAND_U32_MAX,
      res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_KEY_clear(data);
  ARAD_PP_LEM_ACCESS_KEY_clear(mask);

  if ((route_key->group_prefix_len != 32)&&((route_key->source.ip_address != 0)||(route_key->source.prefix_len != 0))) {
	  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
  }
  if ((route_key->inrif_valid == 1)&&(route_key->source.prefix_len != 32)) {
	  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
  }

  

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, route_key->inrif, data);
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, route_key->inrif_valid == 0 ? 0 : mask_full, mask);

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, route_key->source.ip_address, data);
  lcl_mask = (route_key->source.prefix_len > 0) ? 
      SOC_SAND_BITS_MASK(31, 32 - route_key->source.prefix_len) : 0;
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, lcl_mask, mask);

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 2, route_key->group, data);
  
  lcl_mask = (route_key->group == 0) ? 0 : SOC_SAND_BITS_MASK(31, 32 - route_key->group_prefix_len);
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 2, lcl_mask, mask);

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 3, route_key->vrf_ndx, data);
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 3, mask_full, mask);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_kbp_key_mask_encode()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_kbp_key_mask_decode(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID     frwrd_table_id,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *mask,
      SOC_SAND_OUT  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY  *route_key
    )
{
  uint32
    bit_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(mask);
  SOC_SAND_CHECK_NULL_INPUT(route_key);

  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(route_key);

  

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 0, &route_key->inrif, data, mask);
  route_key->inrif_valid = TRUE;

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 1, &route_key->source.ip_address, data, mask);
  route_key->source.prefix_len = 0;
  for (bit_ndx = 0; bit_ndx < SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS; bit_ndx++) {
      if (SHR_BITGET(mask->param[0].value, bit_ndx)) {
          route_key->source.prefix_len = SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS - bit_ndx;
          break;
      }
  }

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 2, &route_key->group, data, mask);

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 3, &route_key->vrf_ndx, data, mask);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_kbp_key_mask_decode()",0,0);
}

#if defined(BCM_JERICHO_SUPPORT)
STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_rpf_kaps_dbal_route_add(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                               vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET              *route_key,
      SOC_SAND_IN  uint32                               fec_id,
      SOC_SAND_IN  uint8                                route_scale,
      SOC_SAND_IN  uint32                               flags,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
    )
{
    uint32 payload, res;
    SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], vrf_ndx, SOC_SAND_U32_MAX);
    DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[1], route_key->ip_address, route_key->prefix_len);

    
    if(flags & _BCM_L3_FLAGS2_RAW_ENTRY) {
        payload = fec_id;
    } else {
        payload = JER_PP_KAPS_FEC_ENCODE(fec_id); 
        if (route_key->prefix_len == 0) {
            payload = JER_PP_KAPS_DEFAULT_ENCODE(payload); 
        }
    }

    if (route_scale) {
        if (route_key->prefix_len <= (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length)) {
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS;
        } else {
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS;
        }
    }

    res = jer_pp_kaps_db_enabled(unit, vrf_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0,  &payload, success);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kaps_dbal_route_add()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_rpf_kaps_dbal_route_remove(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                               vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET              *route_key,
      SOC_SAND_IN  uint8                                route_scale
    )
{
    uint32 res;
    SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE             success;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], vrf_ndx, SOC_SAND_U32_MAX);
    DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[1], route_key->ip_address, route_key->prefix_len);

    if (route_scale) {
        if (route_key->prefix_len <= (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length)) {
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS;
        } else {
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS;
        }
    }

    res = jer_pp_kaps_db_enabled(unit, vrf_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kaps_route_remove()",0,0);
}

uint32
    arad_pp_frwrd_ipv4_uc_rpf_kaps_dbal_route_get(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                               vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET              *route_key,
      SOC_SAND_IN  uint8                                route_scale,
      SOC_SAND_INOUT uint8                              *hit_bit,
      SOC_SAND_OUT uint32                               *fec_id,
      SOC_SAND_OUT uint8                                *found
      )
{
    uint32 payload, res;
    SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], vrf_ndx, SOC_SAND_U32_MAX);
    DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[1], route_key->ip_address, route_key->prefix_len);

    if (route_scale) {
        if (route_key->prefix_len <= (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length)) {
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS;
        } else {
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS;
        }
    }

#ifdef BCM_88675_A0
    res = jer_pp_kaps_db_enabled(unit, vrf_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
#endif 

    res = arad_pp_dbal_entry_get(unit, table_id, qual_vals, &payload, 0, hit_bit, found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (*found) {
        *fec_id = JER_PP_KAPS_DEFAULT_DECODE(JER_PP_KAPS_FEC_DECODE(payload));
    } else {
        *fec_id = 0;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kaps_route_get()",0,0);
}
#endif 

#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_bridge_kbp_key_mask_encode(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID     frwrd_table_id,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY  *route_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *mask
    )
{
  uint32
    lcl_mask = 0,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_KEY_clear(data);
  ARAD_PP_LEM_ACCESS_KEY_clear(mask);

  if ((route_key->group_prefix_len != 32)&&((route_key->source.ip_address != 0)||(route_key->source.prefix_len != 0))) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
  }

  

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, route_key->source.ip_address, data);
  lcl_mask = (route_key->source.prefix_len > 0) ?
                 SOC_SAND_BITS_MASK(31, 32 - route_key->source.prefix_len) : 0;
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, lcl_mask, mask);

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, route_key->group, data);
  
  lcl_mask = (route_key->group == 0) ? 0 : SOC_SAND_BITS_MASK(31, 32 - route_key->group_prefix_len);
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, lcl_mask, mask);

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 2, route_key->fid, data);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_bridge_kbp_key_mask_encode()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_bridge_kbp_key_mask_decode(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID     frwrd_table_id,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *mask,
      SOC_SAND_OUT  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY  *route_key
    )
{
  uint32
    bit_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(mask);
  SOC_SAND_CHECK_NULL_INPUT(route_key);

  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(route_key);

  

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 0, &route_key->source.ip_address, data, mask);
  route_key->source.prefix_len = 0;
  for (bit_ndx = 0; bit_ndx < SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS; bit_ndx++) {
      if (SHR_BITGET(mask->param[0].value, bit_ndx)) {
          route_key->source.prefix_len = SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS - bit_ndx;
          break;
      }
  }

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 1, &route_key->group, data, mask);

  
  ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 2, &route_key->fid, data, mask);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_bridge_kbp_key_mask_decode()",0,0);
}

#endif

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_rpf_dbal_route_add(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY      *route_key,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO     *route_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
    )
{
    uint32 res, prefix_len, priority,weighted_priority;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int is_to_tcam;
    uint32 payload[ARAD_TCAM_ACTION_MAX_LEN];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    sal_memset(payload, 0, sizeof(payload));
    prefix_len = route_key->group_prefix_len;
    weighted_priority = 32*(route_key->group_prefix_len) + route_key->source.prefix_len;

    priority = 0;

    
    is_to_tcam = (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) || (SOC_IS_ARADPLUS_AND_BELOW(unit));

    if (is_to_tcam) {
        
        priority = SOC_SAND_NOF_BITS_IN_UINT32*32 + 32 - weighted_priority;
    }

    DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->group, prefix_len);

    DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(&qual_vals[1], route_key->source.ip_address, route_key->source.prefix_len);

    if ((SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1)
         && SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam == ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) {
        DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[2], route_key->inrif, SOC_SAND_U32_MAX);
    } else {
        DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[2], route_key->inrif, route_key->inrif_valid ? SOC_SAND_U32_MAX : 0);
    }

    if (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_NO_IPV4_VRF) {
        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[3], route_key->vrf_ndx, SOC_SAND_U32_MAX);
    }

    
    if (route_info->flags & _BCM_IPMC_FLAGS_RAW_ENTRY) {
        payload[0]=route_info->dest_id.dest_val;
    } else {
        res = arad_pp_frwrd_ipv4_sand_dest_to_em_dest(unit, &(route_info->dest_id), payload);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

#if defined(INCLUDE_KBP)
    if (JER_KAPS_ENABLE(unit)){
        if (!is_to_tcam) {
            if (SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1) {
                res = jer_pp_kaps_db_verify(unit, route_key->inrif);
            } else {
                res = jer_pp_kaps_db_enabled(unit, route_key->vrf_ndx);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            
            if (route_key->group_prefix_len == 0 && !(route_info->flags & _BCM_IPMC_FLAGS_RAW_ENTRY)) {
                payload[0] += JER_PP_KAPS_DEFAULT_ENCODE(0);
            }
        }
    }
#endif

    
    
    res = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, qual_vals, priority,  payload, success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_rpf_dbal_route_add()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_rpf_dbal_route_remove(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY   *route_key
    )
{
    uint32 res;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    int is_to_tcam = (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) || SOC_IS_ARADPLUS_AND_BELOW(unit);
#endif
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE             success;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    DBAL_QUAL_VALS_CLEAR(qual_vals);


    if (route_key->group != 0) {
        DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->group, route_key->group_prefix_len);
    }else {
        
        
        DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->group, 0);
    }
    DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(&qual_vals[1], route_key->source.ip_address, route_key->source.prefix_len);
    if ((SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1)
          && SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam == ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) {
        DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[2], route_key->inrif, SOC_SAND_U32_MAX);
    } else {
        DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[2], route_key->inrif, route_key->inrif_valid ? SOC_SAND_U32_MAX : 0);
    }
    if(SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_NO_IPV4_VRF) {
        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[3], route_key->vrf_ndx, SOC_SAND_U32_MAX);
    }

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if (JER_KAPS_ENABLE(unit)){
        if (!is_to_tcam) {
            if (SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1) {
                res = jer_pp_kaps_db_verify(unit, route_key->inrif);
            } else {
                res = jer_pp_kaps_db_enabled(unit, route_key->vrf_ndx);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);            
        }
    }
#endif

    res = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, qual_vals, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_rpf_dbal_route_remove()",0,0);

}

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_rpf_dbal_route_get(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY      *route_key,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO     *route_info,
      SOC_SAND_OUT uint8                                *found,
      SOC_SAND_OUT uint8                                *hit_bit
    )
{
    uint32 *payload;
    
    ARAD_PP_LEM_ACCESS_PAYLOAD kbp_payload;
    uint32 res, priority = 0;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    int is_to_tcam = ((SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) || SOC_IS_ARADPLUS_AND_BELOW(unit));
#endif

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->group, route_key->group_prefix_len);
    DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(&qual_vals[1], route_key->source.ip_address, route_key->source.prefix_len);
    if ((SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1)
        && SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam == ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE) {
        DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[2], route_key->inrif, SOC_SAND_U32_MAX);
    } else {
        DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[2], route_key->inrif, route_key->inrif_valid ? SOC_SAND_U32_MAX : 0);
    }
    if (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_NO_IPV4_VRF) {
        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[3], route_key->vrf_ndx, SOC_SAND_U32_MAX);
    }

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if (JER_KAPS_ENABLE(unit)){
        if (!is_to_tcam) {
            if (SOC_DPP_CONFIG(unit)->pp.ipmc_lookup_model == 1) {
                res = jer_pp_kaps_db_verify(unit, route_key->inrif);
            } else {
                res = jer_pp_kaps_db_enabled(unit, route_key->vrf_ndx);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }
#endif

    payload = (uint32 *)&kbp_payload;
    res = arad_pp_dbal_entry_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, qual_vals, payload, &priority, hit_bit, found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if ((*found) && (route_info->flags &_BCM_IPMC_FLAGS_RAW_ENTRY)) {
        route_info->dest_id.dest_val = payload[0];
    } else if (*found) {
        res = arad_pp_frwrd_ipv4_em_dest_to_sand_dest(unit, payload[0], &(route_info->dest_id));
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_rpf_dbal_route_get()",0,0);

}


#if defined(INCLUDE_KBP)

STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_rpf_kbp_route_add(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                               vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET              *route_key,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO   *routing_info,
      SOC_SAND_IN  uint8                                is_scale,
      SOC_SAND_IN  uint32                               flags,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
    )
{
  uint32
      priority,
      table_ndx,
      nof_tables = 2,
      res;
  ARAD_TCAM_ACTION
      action;
  ARAD_PP_LEM_ACCESS_KEY
      data,
      mask;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_KBP_FRWRD_IP_TBL_ID
      frwrd_table_id;
  ARAD_KBP_FRWRD_IP_TBL_ID
      base_frwrd_table_id;
  SOC_DPP_DBAL_SW_TABLE_IDS table_id;
  SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

    
    priority = 32 - route_key->prefix_len;

    if (!ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED && SOC_IS_JERICHO(unit)) {

        if(route_key->prefix_len == 0) {
            
            uint32 kaps_payload;
            if (is_scale && ARAD_KBP_ENABLE_IPV4_DC) {
                if (vrf_ndx > 255) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,10,exit);
                }
                table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS;
            } else {
                table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS;
            }

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], vrf_ndx, SOC_SAND_U32_MAX);

            if(flags & _BCM_L3_FLAGS2_RAW_ENTRY) {
                kaps_payload = routing_info->fec_id;
            } else {
                kaps_payload = JER_PP_KAPS_FEC_ENCODE(routing_info->fec_id);
                kaps_payload = JER_PP_KAPS_DEFAULT_ENCODE(kaps_payload);
            }

            res = jer_pp_kaps_db_enabled(unit, vrf_ndx);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            res = arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0,  &kaps_payload, success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        } else {

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[1], vrf_ndx, SOC_SAND_U32_MAX);
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->ip_address, route_key->prefix_len);

            if (is_scale && ARAD_KBP_ENABLE_IPV4_DC) {
                if (vrf_ndx > 255) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,10,exit);
                }
                frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_DC;
                table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC;
            }
            else{
                if ((vrf_ndx == 0) && (ARAD_KBP_ENABLE_IPV4_UC_PUBLIC)) {
                    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0;
                    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_FWD_KBP;
                } else {
                    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
                    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP;
                }
            }

            if(flags & _BCM_L3_FLAGS2_RAW_ENTRY) {
                payload.dest = routing_info->frwrd_decision.dest_id;
                payload.flags = ARAD_PP_FWD_DECISION_PARSE_RAW_DATA;
            } else {
                res = arad_pp_frwrd_ipv4_uc_rpf_kbp_result_encode(
                        unit, 
                        frwrd_table_id,
                        routing_info, 
                        &payload
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
            }

            res = arad_pp_dbal_entry_add(unit,
                                   table_id,
                                   qual_vals,
                                   priority,
                                   &payload,
                                   success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }

    else {
        
#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0)){
            nof_tables = 1;
        }
#endif 

        if (soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0x0) && (soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0x0) == 0)) {
            nof_tables = 1;
        }

        if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
            base_frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1;
        }else {
            base_frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
        }

        for(table_ndx = 0; table_ndx < nof_tables; table_ndx++)
		{
            frwrd_table_id = base_frwrd_table_id + table_ndx;

            soc_sand_os_memset(&action, 0x0, sizeof(action));

            
            res = arad_pp_frwrd_ipv4_uc_rpf_kbp_key_mask_encode(
               unit,
               frwrd_table_id,
               vrf_ndx, 
               route_key, 
               &data,
               &mask
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

            
            res = arad_pp_frwrd_ipv4_uc_rpf_kbp_result_encode(
               unit, 
               frwrd_table_id,
               routing_info, 
               &payload
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

            
            res = arad_pp_tcam_route_kbp_add_unsafe(
               unit,
               frwrd_table_id,
               &data,
               &mask,
               priority,
               &payload,
               success
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kbp_route_add()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_rpf_kbp_route_add(
      SOC_SAND_IN  int                                  unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY      *route_key,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO     *route_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
    )
{
  uint32
      priority,
      res;
  ARAD_TCAM_ACTION                                
      action;
  ARAD_PP_LEM_ACCESS_KEY       
      data,
      mask;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_KBP_FRWRD_IP_TBL_ID
      frwrd_table_id;
  SOC_PPC_FRWRD_DECISION_INFO 
      em_dest;
  SOC_DPP_DBAL_SW_TABLE_IDS
      table_id;
  SOC_PPC_FP_QUAL_VAL
      qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_memset(&action, 0x0, sizeof(action));

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_MC;
    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FWD_KBP;
    priority = 32 - route_key->source.prefix_len; 

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

    if (SOC_IS_JERICHO(unit)) {

        if ((route_key->group_prefix_len != 32)&&(route_key->source.prefix_len != 0)) {
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
        }
        if (!ARAD_KBP_IPV4_LOOKUP_MODEL_1_IS_IN_USE(unit)) {
            
            if ((route_key->inrif_valid == 1)&&(route_key->source.prefix_len != 32)) {
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
            }
        }

#if 0 
        if(route_key->group_prefix_len == 0) {
            
            uint32 kaps_payload;
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS;

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], route_key->vrf_ndx, SOC_SAND_U32_MAX);

            if (route_info->flags & _BCM_IPMC_FLAGS_RAW_ENTRY) {
                
                kaps_payload = route_info->dest_id.dest_val;
            } else {
                res = arad_pp_frwrd_ipv4_sand_dest_to_em_dest(unit, &(route_info->dest_id), &kaps_payload);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
                kaps_payload = JER_PP_KAPS_DEFAULT_ENCODE(kaps_payload);
            }

            res = jer_pp_kaps_db_enabled(unit, route_key->vrf_ndx);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            res = arad_pp_dbal_entry_add(unit, table_id, qual_vals, priority,  &kaps_payload, success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        } else
#endif
        {

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], route_key->vrf_ndx, SOC_SAND_U32_MAX);
            if (ARAD_KBP_IPV4_LOOKUP_MODEL_1_IS_IN_USE(unit)) {
                DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[1], route_key->inrif, SOC_SAND_U32_MAX);
            } else {
                DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[1], route_key->inrif, route_key->inrif_valid ? SOC_SAND_U32_MAX : 0);
            }
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[2], route_key->group, route_key->group_prefix_len);
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(&qual_vals[3], route_key->source.ip_address, route_key->source.prefix_len);

            if(route_info->flags & _BCM_IPMC_FLAGS_RAW_ENTRY) {
                
                payload.dest = route_info->dest_id.dest_val;
                payload.flags = ARAD_PP_FWD_DECISION_PARSE_RAW_DATA;
            } else {
                
                res = arad_pp_frwrd_ipv4_sand_dest_to_fwd_decision(
                        unit,
                        &(route_info->dest_id),
                        &em_dest
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
                ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
                res = arad_pp_fwd_decision_in_buffer_build(
                        unit,
                        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
                        &em_dest,
                        &payload.dest,
                        &payload.asd
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
            }

            res = arad_pp_dbal_entry_add(unit,
                                   table_id,
                                   qual_vals,
                                   priority,
                                   &payload,
                                   success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }
    else{
        
        res = arad_pp_frwrd_ipv4_mc_kbp_key_mask_encode(
                unit,
                frwrd_table_id,
                route_key,
                &data,
                &mask
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

        
        res = arad_pp_frwrd_ipv4_sand_dest_to_fwd_decision(
                unit,
                &(route_info->dest_id),
                &em_dest
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
        res = arad_pp_fwd_decision_in_buffer_build(
                unit,
                ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
                &em_dest,
                &payload.dest,
                &payload.asd
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

        
        res = arad_pp_tcam_route_kbp_add_unsafe(
                unit,
                frwrd_table_id,
                &data,
                &mask,
                priority,
                &payload,
                success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_rpf_kbp_route_add()",0,0);
}


STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_rpf_kbp_route_remove(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                               vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET              *route_key,
      SOC_SAND_IN  uint8                                is_scale
    )
{
  uint32
      table_ndx,
      nof_tables = 2,
      res;
  ARAD_PP_LEM_ACCESS_KEY       
      data,
      mask;
  ARAD_KBP_FRWRD_IP_TBL_ID
      frwrd_table_id, frwrd_table_id_base;
  SOC_DPP_DBAL_SW_TABLE_IDS table_id;
  SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
  SOC_SAND_SUCCESS_FAILURE  success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED && SOC_IS_JERICHO(unit)) {

        if(route_key->prefix_len == 0) {
            
            if (is_scale && ARAD_KBP_ENABLE_IPV4_DC) {
                if (vrf_ndx > 255) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,10,exit);
                }
                table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS;
            } else {
                table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS;
            }

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], vrf_ndx, SOC_SAND_U32_MAX);

            res = jer_pp_kaps_db_enabled(unit, vrf_ndx);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            res = arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        } else {

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[1], vrf_ndx, SOC_SAND_U32_MAX);
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->ip_address, route_key->prefix_len);

            if (is_scale && ARAD_KBP_ENABLE_IPV4_DC) {
                if (vrf_ndx > 255) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,10,exit);
                }
                frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_DC;
                table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC;
            }
            else{
                if ((vrf_ndx == 0) && (ARAD_KBP_ENABLE_IPV4_UC_PUBLIC)) {
                    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0;
                    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_FWD_KBP;
                } else {
                    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
                    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP;
                }
            }

            res = arad_pp_dbal_entry_delete(unit, 
                                   table_id,
                                   qual_vals,
                                   &success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }
    else{
        
#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0)){
            nof_tables = 1;
        }    
#endif 
    
        if (soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0x0) && (soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0x0) == 0)) {
            nof_tables = 1;
        }

        if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
            frwrd_table_id_base = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1;
        } else{
            frwrd_table_id_base = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
        }

        for(table_ndx = 0; table_ndx < nof_tables; table_ndx++)
        {
            frwrd_table_id = frwrd_table_id_base + table_ndx;

            
            res = arad_pp_frwrd_ipv4_uc_rpf_kbp_key_mask_encode(
               unit,
               frwrd_table_id,
               vrf_ndx, 
               route_key, 
               &data,
               &mask
               );
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

            
            res = arad_pp_tcam_route_kbp_remove_unsafe(
               unit,
               frwrd_table_id,
               &data,
               &mask
               );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kbp_route_remove()",0,0);
}


STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_rpf_kbp_route_remove(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY      *route_key
    )
{
    uint32
        res;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;
    SOC_DPP_DBAL_SW_TABLE_IDS
        table_id;
    SOC_PPC_FP_QUAL_VAL
        qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE
        success;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    if(SOC_IS_JERICHO(unit)) {

        if ((route_key->group_prefix_len != 32)&&(route_key->source.prefix_len != 0)) {
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
        }
        if (!ARAD_KBP_IPV4_LOOKUP_MODEL_1_IS_IN_USE(unit)) {
            
            if ((route_key->inrif_valid == 1)&&(route_key->source.prefix_len != 32)) {
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
            }
        }

#if 0 
        if(route_key->group_prefix_len == 0) {
            
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS;

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], route_key->vrf_ndx, SOC_SAND_U32_MAX);

            res = jer_pp_kaps_db_enabled(unit, route_key->vrf_ndx);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            res = arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        } else
#endif
        {

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], route_key->vrf_ndx, SOC_SAND_U32_MAX);
            if (ARAD_KBP_IPV4_LOOKUP_MODEL_1_IS_IN_USE(unit)) {
                DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[1], route_key->inrif, SOC_SAND_U32_MAX);
            } else {
                DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[1], route_key->inrif, route_key->inrif_valid ? SOC_SAND_U32_MAX : 0);
            }
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[2], route_key->group, route_key->group_prefix_len);
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(&qual_vals[3], route_key->source.ip_address, route_key->source.prefix_len);

            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FWD_KBP;

            res = arad_pp_dbal_entry_delete(
                    unit, 
                    table_id,
                    qual_vals,
                    &success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }
    else {
        frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_MC;

        
        res = arad_pp_frwrd_ipv4_mc_kbp_key_mask_encode(
                unit,
                frwrd_table_id,
                route_key,
                &data,
                &mask
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

        
        res = arad_pp_tcam_route_kbp_remove_unsafe(
                unit,
                frwrd_table_id,
                &data,
                &mask
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_rpf_kbp_route_remove()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_rpf_kbp_route_get(
      SOC_SAND_IN  int                                  unit,
      SOC_SAND_IN  uint32                               vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET              *route_key,
      SOC_SAND_IN  uint8                                is_scale,
	  SOC_SAND_IN  uint8                                is_host,
      SOC_SAND_IN  uint32                               flags,
	  SOC_SAND_OUT uint8                                *accessed,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO   *routing_info,
      SOC_SAND_OUT uint8                                *found
    )
{
  uint32
      priority,
      res;
  ARAD_PP_LEM_ACCESS_PAYLOAD
      payload;
  ARAD_PP_LEM_ACCESS_KEY       
      data,
      mask;
  ARAD_KBP_FRWRD_IP_TBL_ID
      frwrd_table_id;
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO 
      host_route_info;
  SOC_DPP_DBAL_SW_TABLE_IDS table_id;
  SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
  uint32 kaps_payload;
  uint32 payload_flags = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

    if (!ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED && SOC_IS_JERICHO(unit)) {

        if(route_key->prefix_len == 0) {
            
            if (is_scale && ARAD_KBP_ENABLE_IPV4_DC) {
                if (vrf_ndx > 255) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,10,exit);
                }
                table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS;
            } else {
                table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS;
            }

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], vrf_ndx, SOC_SAND_U32_MAX);

            res = jer_pp_kaps_db_enabled(unit, vrf_ndx);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            res = arad_pp_dbal_entry_get(unit, table_id, qual_vals, &kaps_payload, 0, NULL, found);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        } else {

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[1], vrf_ndx, SOC_SAND_U32_MAX);
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->ip_address, route_key->prefix_len);

            if (is_scale && ARAD_KBP_ENABLE_IPV4_DC) {
                if (vrf_ndx > 255) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,10,exit);
                }
                frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_DC;
                table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC;
            }
            else{
                if ((vrf_ndx == 0) && (ARAD_KBP_ENABLE_IPV4_UC_PUBLIC)) {
                    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0;
                    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_FWD_KBP;
                } else {
                    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
                    table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP;
                    
                    payload.native_vsi = is_host;
                }
            }

            res = arad_pp_dbal_entry_get(unit, 
                                         table_id,
                                         qual_vals,
                                         (void*)&payload,
                                         &priority,
                                         NULL,
                                         found);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
        }
    }
    else {

        if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
            frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1;
        } else{
            frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
        }
         
        
        res = arad_pp_frwrd_ipv4_uc_rpf_kbp_key_mask_encode(
           unit,
           frwrd_table_id,
           vrf_ndx, 
           route_key, 
           &data,
           &mask
           );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

        
        res = arad_pp_tcam_route_kbp_get_unsafe(
           unit,
           frwrd_table_id,
           &data,
           &mask,
           &priority,
           &payload,
           found
           );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 
    }
	
    if (*found) {
         
        if (!(JER_PP_KAPS_PAYLOAD_IS_FEC(payload.dest))
             && !(JER_PP_KAPS_PAYLOAD_IS_DEFAULT(payload.dest))
             && !(JER_PP_KAPS_PAYLOAD_IS_MC(payload.dest))) {
            payload_flags |= _BCM_L3_FLAGS2_RAW_ENTRY;
        }

        if(!ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED && SOC_IS_JERICHO(unit) && (route_key->prefix_len == 0)) {
            
            if(flags & _BCM_L3_FLAGS2_RAW_ENTRY) {
                
                routing_info->frwrd_decision.dest_id = routing_info->fec_id = kaps_payload;
            } else {
                routing_info->frwrd_decision.dest_id = routing_info->fec_id = JER_PP_KAPS_DEFAULT_DECODE(JER_PP_KAPS_FEC_DECODE(kaps_payload));
            }
        } else if(!ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED && SOC_IS_JERICHO(unit)
                  && ((flags & _BCM_L3_FLAGS2_RAW_ENTRY) || (payload_flags & _BCM_L3_FLAGS2_RAW_ENTRY))) {
            
            routing_info->frwrd_decision.dest_id = payload.dest | (payload.asd << 19);
        } else {
            SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&host_route_info);
            res = arad_pp_frwrd_em_dest_to_fec(
                    unit, 
                    &payload, 
                    routing_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            routing_info->fec_id = routing_info->frwrd_decision.dest_id;

            if (is_host) {
                
                ARAD_PP_LEM_ACCESS_HI_FROM_EEI(payload.asd, routing_info->mac_lsb);

                
                routing_info->native_vsi = payload.native_vsi;

                if (payload.flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED) {
                    *accessed = TRUE;
                }
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kbp_route_get()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_rpf_kbp_route_get(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY      *route_key,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO     *route_info,
      SOC_SAND_OUT uint8                                *found
    )
{
  uint32
      priority,
      res;
  ARAD_PP_LEM_ACCESS_PAYLOAD
      payload;
  ARAD_PP_LEM_ACCESS_KEY       
      data,
      mask;
  ARAD_KBP_FRWRD_IP_TBL_ID
      frwrd_table_id;
  SOC_DPP_DBAL_SW_TABLE_IDS
      table_id;
  SOC_PPC_FP_QUAL_VAL
      qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
  uint32                    
      payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S];
#if 0
  uint32
      kaps_payload[ARAD_TCAM_ACTION_MAX_LEN];
#endif

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    
    if(SOC_IS_JERICHO(unit)) {

        if ((route_key->group_prefix_len != 32)&&(route_key->source.prefix_len != 0)) {
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
        }
        if (!ARAD_KBP_IPV4_LOOKUP_MODEL_1_IS_IN_USE(unit)) {
            
            if ((route_key->inrif_valid == 1)&&(route_key->source.prefix_len != 32)) {
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
            }
        }

#if 0 
        if(route_key->group_prefix_len == 0) {
            
            uint8 hit_bit = 0;
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS;

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], route_key->vrf_ndx, SOC_SAND_U32_MAX);

            res = jer_pp_kaps_db_enabled(unit, route_key->vrf_ndx);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            res = arad_pp_dbal_entry_get(unit, table_id, qual_vals, kaps_payload, &priority, &hit_bit, found);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        } else
#endif
        {

            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[0], route_key->vrf_ndx, SOC_SAND_U32_MAX);
            if (ARAD_KBP_IPV4_LOOKUP_MODEL_1_IS_IN_USE(unit)) {
                DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[1], route_key->inrif, SOC_SAND_U32_MAX);
            } else {
                DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[1], route_key->inrif, route_key->inrif_valid ? SOC_SAND_U32_MAX : 0);
            }
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[2], route_key->group, route_key->group_prefix_len);
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(&qual_vals[3], route_key->source.ip_address, route_key->source.prefix_len);

            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FWD_KBP;

            res = arad_pp_dbal_entry_get(
                    unit,
                    table_id,
                    qual_vals,
                    (void*)&payload,
                    &priority,
                    NULL,
                    found);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
    }
    else {
        frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_MC;

        
        res = arad_pp_frwrd_ipv4_mc_kbp_key_mask_encode(
                unit,
                frwrd_table_id,
                route_key,
                &data,
                &mask
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

        
        res = arad_pp_tcam_route_kbp_get_unsafe(
                unit,
                frwrd_table_id,
                &data,
                &mask,
                &priority,
                &payload,
                found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 
    }
    if (*found) {
#if 0
        if(SOC_IS_JERICHO(unit) && (route_key->group_prefix_len == 0)) {
            
            if(route_info->flags & _BCM_IPMC_FLAGS_RAW_ENTRY) {
                
                route_info->dest_id.dest_val = kaps_payload[0];
            } else {
                res = arad_pp_frwrd_ipv4_em_dest_to_sand_dest(unit, kaps_payload[0], &(route_info->dest_id));
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            }
        } else
#endif
        if(SOC_IS_JERICHO(unit) && route_info->flags & _BCM_IPMC_FLAGS_RAW_ENTRY) {
            
            route_info->dest_id.dest_val = payload.dest | (payload.asd << 19);
        } else {
            sal_memset(&payload_data, 0x0, ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S * sizeof(uint32));

            
            res = arad_pp_lem_access_payload_build(
                    unit,
                    &payload,
                    payload_data
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            res = arad_pp_frwrd_ipv4_em_dest_to_sand_dest(
                    unit,
                    payload_data[0], 
                    &(route_info->dest_id)
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_rpf_kbp_route_get()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_rpf_kbp_table_clear(
      SOC_SAND_IN  int     unit
    )
{
    uint32
        nof_tables = 2,
        table_ndx,
        frwrd_table_id,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
        frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED;

        
        res = arad_pp_tcam_kbp_table_clear(
                unit,
                frwrd_table_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1;

        
        res = arad_pp_tcam_kbp_table_clear(
                unit,
                frwrd_table_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    }else {
    #ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0)){
          nof_tables = 1;
        }
    #endif 
            
        if (soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0x0) && (soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0x0) == 0)) {
            nof_tables = 1;
        }

        if (ARAD_KBP_ENABLE_IPV4_DC) {
            
            frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_DC;
            res = arad_pp_tcam_kbp_table_clear(unit,frwrd_table_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }

        if (ARAD_KBP_ENABLE_IPV4_UC_PUBLIC) {
            
            frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0;
            res = arad_pp_tcam_kbp_table_clear(unit, frwrd_table_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }

        for(table_ndx = 0; table_ndx < nof_tables; table_ndx++)
        {
            frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0 + table_ndx;

            
            res = arad_pp_tcam_kbp_table_clear(
                    unit,
                    frwrd_table_id
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 
        }
        if(SOC_IS_JERICHO(unit)) {
            
            res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_uc_rpf_kbp_table_clear()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_rpf_kbp_table_clear(
      SOC_SAND_IN  int                       unit
    )
{
    uint32
        frwrd_table_id,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_MC;

    
    res = arad_pp_tcam_kbp_table_clear(
            unit,
            frwrd_table_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 
#if 0 
    if(SOC_IS_JERICHO(unit)) {
        
        res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_rpf_kbp_table_clear()",0,0);
}

STATIC
 uint32
  arad_pp_frwrd_ipv4_kbp_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID               frwrd_table_id,
    SOC_SAND_IN  uint8                                  is_host_table,
    SOC_SAND_IN  uint32                                 vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE       *block_range_key,
    SOC_SAND_OUT  uint32                                *vrf_ndx_out,
    SOC_SAND_OUT  SOC_SAND_PP_IPV4_SUBNET               *route_keys_uc,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY        *route_keys_mc,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO     *route_infos_uc,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO       *route_infos_mc,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS          *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION        *routes_location,
    SOC_SAND_OUT uint32                                 *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
      nof_entries_lcl,
      route_ndx,
    indx;
  uint32       
    vrf_ndx_lcl,             
    payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S] = {0};
  ARAD_PP_LEM_ACCESS_KEY
      *read_keys = NULL,
      *read_keys_mask = NULL;
  ARAD_PP_LEM_ACCESS_PAYLOAD
      *read_vals = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);
  if ((frwrd_table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_MC) || (frwrd_table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE)) {
      SOC_SAND_CHECK_NULL_INPUT(route_keys_mc);
      SOC_SAND_CHECK_NULL_INPUT(route_infos_mc);
  }
  else {
    if (is_host_table)
    {
      SOC_SAND_CHECK_NULL_INPUT(vrf_ndx_out);
    }
    SOC_SAND_CHECK_NULL_INPUT(route_keys_uc);
    SOC_SAND_CHECK_NULL_INPUT(route_infos_uc);
  }

  if(block_range_key->entries_to_act == 0)
  {
    *nof_entries = 0;
    goto exit;
  }

  
  read_keys_mask = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range_key->entries_to_act,"read_keys");
  read_keys = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range_key->entries_to_act,"read_keys");
  read_vals = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range_key->entries_to_act,"read_vals");
  res = soc_sand_os_memset(read_keys, 0x0, sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range_key->entries_to_act);
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  res = soc_sand_os_memset(read_keys_mask, 0x0, sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range_key->entries_to_act);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  res = soc_sand_os_memset(read_vals, 0x0, sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range_key->entries_to_act);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  
  res = arad_pp_tcam_route_kbp_get_block_unsafe(
            unit,
            frwrd_table_id,
            block_range_key,
            read_keys,
            read_keys_mask,
            read_vals,
            &nof_entries_lcl
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  
  route_ndx = 0;
  for(indx = 0; indx < nof_entries_lcl; ++indx)
  {
      
      if ((frwrd_table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_MC) || (frwrd_table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE)) {
          if (frwrd_table_id != ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE){
              res = arad_pp_frwrd_ipv4_mc_kbp_key_mask_decode(
                    unit,
                    frwrd_table_id,
                    &read_keys[indx],
                    &read_keys_mask[route_ndx],
                    &route_keys_mc[route_ndx]
              );
          } else {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
              res = arad_pp_frwrd_ipv4_mc_bridge_kbp_key_mask_decode(
                    unit,
                    frwrd_table_id,
                    &read_keys[indx],
                    &read_keys_mask[route_ndx],
                    &route_keys_mc[route_ndx]
              );
#endif
          }
          SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

          sal_memset(&payload_data, 0x0, ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S * sizeof(uint32));

          
          res = arad_pp_lem_access_payload_build(
                  unit,
                  &read_vals[indx],
                  payload_data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

          res = arad_pp_frwrd_ipv4_em_dest_to_sand_dest(
                  unit,
                  payload_data[0], 
                  &(route_infos_mc[route_ndx].dest_id)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      } else {
		  if (is_host_table){
			  res = arad_pp_frwrd_ipv4_uc_rpf_kbp_key_mask_decode(
					  unit,
					  frwrd_table_id,
					  &read_keys[indx],
					  &read_keys_mask[indx],
					  &vrf_ndx_out[route_ndx],
					  &(route_keys_uc[route_ndx])
					);
			  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
		  } else {
			  res = arad_pp_frwrd_ipv4_uc_rpf_kbp_key_mask_decode(
					  unit,
					  frwrd_table_id,
					  &read_keys[indx],
					  &read_keys_mask[indx],
					  &vrf_ndx_lcl,
					  &(route_keys_uc[route_ndx])
					);
			  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

			  
			  if (vrf_ndx_lcl != vrf_ndx) {
				  continue;
	  		  }
		  }
		  res = arad_pp_frwrd_em_dest_to_fec(
				  unit, 
				  &read_vals[indx], 
				  &(route_infos_uc[route_ndx])
				);
		  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
		  route_infos_uc[route_ndx].fec_id = route_infos_uc[route_ndx].frwrd_decision.dest_id;

		  if (is_host_table) {
			  
			  ARAD_PP_LEM_ACCESS_HI_FROM_EEI(read_vals[indx].asd, route_infos_uc[route_ndx].mac_lsb);

			  
			  route_infos_uc[route_ndx].native_vsi = read_vals[indx].native_vsi;
		  }
	  }

      if(routes_location){
        routes_location[route_ndx] = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_TCAM;
      }
      if(routes_status){
		  routes_status[route_ndx] = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
		  if (read_vals[indx].flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED) {
			  routes_status[route_ndx] |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
		  }
	  }
      route_ndx ++;
  }
  *nof_entries = route_ndx;

exit:
  if (read_keys)
  {
    soc_sand_os_free_any_size(read_keys);
  }
  if (read_keys_mask)
  {
    soc_sand_os_free_any_size(read_keys_mask);
  }
  if (read_vals)
  {
    soc_sand_os_free_any_size(read_vals);
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_kbp_route_get_block_unsafe()", 0, 0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_mc_bridge_kbp_table_clear(
      SOC_SAND_IN  int                       unit
    )
{
    uint32 res = SOC_SAND_OK;
    uint32 frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pp_tcam_kbp_table_clear(
            unit,
            frwrd_table_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_bridge_kbp_table_clear()",0,0);
}

#endif

STATIC
 uint32
  arad_pp_frwrd_ipv4_dbal_route_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS              frwrd_table_id,
    SOC_SAND_IN  uint32                                 vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE       *block_range_key,
    SOC_SAND_OUT  SOC_SAND_PP_IPV4_SUBNET               *route_keys_uc,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY        *route_keys_mc,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO     *route_infos_uc,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO       *route_infos_mc,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS          *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION        *routes_location,
    SOC_SAND_OUT uint32                                 *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
      route_ndx,
      block_start_ndx,
    indx;
  uint32
    i;

  SOC_PPC_FP_QUAL_VAL
      *qual_vals_array = NULL;

  SOC_PPC_FP_QUAL_TYPE qual_type;

  uint32
      *payload = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);
  if (frwrd_table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS || frwrd_table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID) {
      SOC_SAND_CHECK_NULL_INPUT(route_keys_mc);
      SOC_SAND_CHECK_NULL_INPUT(route_infos_mc);
  }
  else {
    SOC_SAND_CHECK_NULL_INPUT(route_keys_uc);
    SOC_SAND_CHECK_NULL_INPUT(route_infos_uc);
  }

  if(block_range_key->entries_to_act == 0)
  {
    *nof_entries = 0;
    goto exit;
  }

  
  qual_vals_array = soc_sand_os_malloc_any_size(sizeof(SOC_PPC_FP_QUAL_VAL) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX * block_range_key->entries_to_act,"qual_vals_array");
  payload = soc_sand_os_malloc_any_size(sizeof(uint32) * BYTES2WORDS(sizeof(uint32)) * block_range_key->entries_to_act,"payload");

  for (i=0; i < block_range_key->entries_to_act; i++) {
      DBAL_QUAL_VALS_CLEAR(qual_vals_array + i * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
  }
  res = soc_sand_os_memset(payload, 0x0, sizeof(uint32) * BYTES2WORDS(sizeof(uint32)) * block_range_key->entries_to_act);
  SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

  if (vrf_ndx != -1) {
      qual_vals_array[0].type = SOC_PPC_FP_QUAL_IRPP_VRF;
      qual_vals_array[0].val.arr[0] = vrf_ndx;
      qual_vals_array[0].is_valid.arr[0] = SOC_SAND_U32_MAX;
  }

  
  block_start_ndx = *nof_entries;
  res = arad_pp_dbal_block_get(unit, block_range_key, frwrd_table_id, qual_vals_array, payload,  nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  
  for(indx = 0; indx < (*nof_entries - block_start_ndx); ++indx)
  {
      route_ndx = indx;
      
      for (i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
          qual_type = qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].type;
          if ((qual_type == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_type == BCM_FIELD_ENTRY_INVALID)) {
              break;
          }
          if (frwrd_table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS) {
              if (qual_type == SOC_PPC_FP_QUAL_IRPP_IN_RIF){
                  route_keys_mc[route_ndx].inrif = qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].val.arr[0] &
                                                   qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].is_valid.arr[0];
                  if (route_keys_mc[route_ndx].inrif) {
                      route_keys_mc[route_ndx].inrif_valid = TRUE;
                  } else {
                      route_keys_mc[route_ndx].inrif_valid = FALSE;
                  }
              }
              if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP) {
                  route_keys_mc[route_ndx].source.ip_address = qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].val.arr[0];
                  route_keys_mc[route_ndx].source.prefix_len = _shr_ip_mask_length(qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].is_valid.arr[0]);
              }
              if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP) {
                  
                  route_keys_mc[route_ndx].group = qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].val.arr[0] | ARAD_PP_FRWRD_IPV4_GROUP_MIN;
                  route_keys_mc[route_ndx].group_prefix_len = _shr_ip_mask_length(qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].is_valid.arr[0]);
              }
              if (qual_type == SOC_PPC_FP_QUAL_IRPP_VRF) {
                  route_keys_mc[route_ndx].vrf_ndx = qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].val.arr[0];
              }
          } else if (frwrd_table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS) {
              if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP) {
                  route_keys_uc[route_ndx].ip_address = qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].val.arr[0];
                  route_keys_uc[route_ndx].prefix_len = _shr_ip_mask_length(qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].is_valid.arr[0]);
              }
          }else if (frwrd_table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID) {
              if (qual_type == SOC_PPC_FP_QUAL_FID) {
                  route_keys_mc[route_ndx].fid = qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].val.arr[0];              
              }   
              if (qual_type == SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP) {
                  route_keys_mc[route_ndx].group = qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].val.arr[0];
                  route_keys_mc[route_ndx].group_prefix_len = _shr_ip_mask_length(qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].is_valid.arr[0]);
              }
              if (qual_type == SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_SIP) {
                  route_keys_mc[route_ndx].source.ip_address = qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].val.arr[0];
                  route_keys_mc[route_ndx].source.prefix_len = _shr_ip_mask_length(qual_vals_array[indx * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX + i].is_valid.arr[0]);
              }
          }
      }

      if (frwrd_table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS || frwrd_table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID) {

          res = arad_pp_frwrd_ipv4_em_dest_to_sand_dest(unit, payload[indx], &(route_infos_mc[route_ndx].dest_id)); 
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);          
      }

#if defined(INCLUDE_KBP)
      if (frwrd_table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS) {
          route_infos_uc[route_ndx].fec_id = JER_PP_KAPS_DEFAULT_DECODE(JER_PP_KAPS_FEC_DECODE(payload[indx]));
      }
#endif
      if(routes_location)
      {
          routes_location[route_ndx] = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM;
      }
      if(routes_status)
      {
          
          routes_status[route_ndx] = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
      }
  }

exit:
  if (qual_vals_array)
  {
    soc_sand_os_free_any_size(qual_vals_array);
  }
  if (payload)
  {
    soc_sand_os_free_any_size(payload);
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_dbal_route_get_block()", 0, 0);
}


STATIC
  uint32
    arad_pp_frwrd_ipv4_lpm_route_add(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_IN  uint32                                      fec_id,
      SOC_SAND_IN  uint8                                     is_pending_op,
      SOC_SAND_IN  SOC_PPC_FRWRD_IP_ROUTE_LOCATION                 hw_target_type,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
    )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_SAND_PP_IPV4_SUBNET
     masked_route_key;
  uint32
    res;
  uint8
    success_bool;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_ADD);

  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  arad_pp_ipv4_unicast_mask_ip(
     route_key,
     &masked_route_key
   );
 
  res = arad_pp_ipv4_lpm_mngr_prefix_add(
          &lpm_mngr,
          vrf_ndx,
          &masked_route_key,
          fec_id,
          is_pending_op,
          hw_target_type,
          &success_bool
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  *success = (success_bool == TRUE) ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_lpm_route_add()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_lpm_vrf_clear(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx
    )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint8
    pending_op;
  uint32
    res,
    default_fec;
  uint8
    is_shared;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_CLEAR);
  
  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.default_fec.get(
          unit,
          &default_fec
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_sw_db_ipv4_is_shared_lpm_memory(unit,vrf_ndx,&is_shared);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(!is_shared) 
   {
      res = arad_pp_ipv4_lpm_mngr_vrf_clear(
              &lpm_mngr,
              vrf_ndx,
              default_fec,
              pending_op
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else
  {
      res = arad_pp_ipv4_lpm_mngr_vrf_routes_clear(
              &lpm_mngr,
              vrf_ndx,
              default_fec,
              pending_op
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_lpm_vrf_clear()",0,0);
}


STATIC
  uint32
    arad_pp_frwrd_ipv4_lpm_all_vrfs_clear(
      SOC_SAND_IN  int                                     unit
    )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint32
    nof_vrfs;
  uint32
    res,
    default_fec;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_CLEAR);
  
  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.default_fec.get(
          unit,
          &default_fec
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.get(
          unit,
          &nof_vrfs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  
  res = arad_pp_ipv4_lpm_mngr_all_vrfs_clear(
          &lpm_mngr,
          nof_vrfs,
          default_fec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_lpm_all_vrfs_clear()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_lpm_route_remove(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_IN  uint8                                     is_pending_op,
      SOC_SAND_OUT uint8                                     *success
    )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_SAND_PP_IPV4_SUBNET
    masked_route_key;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_REMOVE);
  
  
  if (route_key->prefix_len == 0)
  {
    goto exit;
  }

  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  arad_pp_ipv4_unicast_mask_ip(
    route_key,
    &masked_route_key
  );

  arad_pp_ipv4_lpm_mngr_prefix_remove(
          &lpm_mngr,
          vrf_ndx,
          &masked_route_key,
          is_pending_op,
          success
        );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_lpm_route_remove()",0,0);
}


STATIC
  uint32
    arad_pp_frwrd_ipv4_lpm_route_get(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_IN  uint8                                     exact_match,
      SOC_SAND_OUT uint32                                      *fec_id,
      SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS         *pending_type,
      SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION          *hw_target,
      SOC_SAND_OUT uint8                                     *found
    )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_SAND_PP_IPV4_SUBNET
    masked_route_key;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_GET);

  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  arad_pp_ipv4_unicast_mask_ip(
    route_key,
    &masked_route_key
  );

  res = arad_pp_ipv4_lpm_mngr_sys_fec_get(
          &lpm_mngr,
          vrf_ndx,
          &masked_route_key,
          exact_match,
          fec_id,
          pending_type,
          hw_target,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_lpm_route_get()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_lpm_route_get_block(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                  vrf_ndx,
      SOC_SAND_IN  uint8                                   is_scale,
      SOC_SAND_INOUT  SOC_PPC_IP_ROUTING_TABLE_RANGE       *block_range,
      SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                 *route_key,
      SOC_SAND_OUT uint32                                  *fec_id,
      SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *pending_type,
      SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *hw_target,
      SOC_SAND_OUT uint32                                  *nof_entries
    )
{

  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint32
    res;
#if defined(INCLUDE_KBP)
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
    *route_infos_uc = NULL;
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_LPM_ROUTE_GET_BLOCK);
  
#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM || JER_KAPS_ENABLE(unit))
  {
    uint32 read_index = 0;

    
    route_infos_uc = soc_sand_os_malloc_any_size(sizeof(SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO) * block_range->entries_to_act,"route_infos_uc");
    if (!route_infos_uc)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 42, exit);
    }
    res = soc_sand_os_memset(route_infos_uc, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO) * block_range->entries_to_act);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

	if (ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM) {
        ARAD_KBP_FRWRD_IP_TBL_ID frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;

        res = arad_pp_frwrd_ipv4_kbp_route_get_block_unsafe(
                unit,
                frwrd_table_id,
                FALSE ,
                vrf_ndx,
                block_range,
                NULL ,
                route_key,
                NULL,
                route_infos_uc,
                NULL,
                pending_type,
                hw_target,
                nof_entries
              );
	}else{
        res = arad_pp_frwrd_ipv4_dbal_route_get_block(
                unit,
                SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS,
                vrf_ndx,
                block_range,
                route_key,
                NULL,
                route_infos_uc,
                NULL,
                pending_type,
                hw_target,
                nof_entries
              );
    }

    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
    
    for (read_index = 0; read_index < *nof_entries; ++read_index)
    {
      fec_id[read_index] = route_infos_uc[read_index].fec_id;
    }
  }
  else
#endif 
  {
    res = arad_pp_sw_db_ipv4_lpm_mngr_get(
            unit,
            &lpm_mngr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_pp_ipv4_lpm_mngr_get_block(
            &lpm_mngr,
            vrf_ndx,
            block_range->start.type,
            &block_range->start.payload,
            block_range->entries_to_scan,
            block_range->entries_to_act,
            route_key,
            fec_id,
            pending_type,
            hw_target,
            nof_entries
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
#if defined(INCLUDE_KBP)
  if (route_infos_uc)
  {
    soc_sand_os_free_any_size(route_infos_uc);
  }
#endif 
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_lpm_route_get_block()",0,0);
}




STATIC
  uint32
    arad_pp_frwrd_ipv4_lem_route_add(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO              *routing_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
    )
{
  uint32
    res;
  ARAD_PP_LEM_ACCESS_PAYLOAD payload;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_LEM_ROUTE_ADD);
  
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  res = arad_pp_frwrd_ipv4_host_lem_payload_build(unit,routing_info,&payload);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (SOC_IS_JERICHO(unit)) {
      SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
      SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

      DBAL_QUAL_VALS_CLEAR(qual_vals);
      DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->ip_address, route_key->prefix_len);
      DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[1], vrf_ndx, SOC_SAND_U32_MAX);

      if (route_key->prefix_len != 32) {
          
          table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM;
      }

      res = arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0,  &payload, success);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } else {
      ARAD_PP_LEM_ACCESS_REQUEST request;
      ARAD_PP_LEM_ACCESS_ACK_STATUS ack;

      ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
      ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

      request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
      arad_pp_frwrd_ipv4_host_lem_request_key_build(
         unit,
         ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST,
         vrf_ndx,
         route_key->ip_address,
         &request.key
         );

      res = arad_pp_lem_access_entry_add_unsafe(
         unit,
         &request,
         &payload,
         &ack);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if(ack.is_success)
      {
        *success = SOC_SAND_SUCCESS;
      }
      else
      {
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      }
  }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_lem_route_add()",0,0);
}


STATIC
  uint32
    arad_pp_frwrd_ipv4_lem_route_get(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_OUT  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO             *routing_info,
      SOC_SAND_OUT uint8                                     *found,
      SOC_SAND_OUT uint8                                     *accessed
    )
{
  uint32
    res;
  ARAD_PP_LEM_ACCESS_PAYLOAD payload;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_LEM_ROUTE_GET);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(found);
  SOC_SAND_CHECK_NULL_INPUT(accessed);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  *accessed = FALSE;

  if (SOC_IS_JERICHO(unit)) {
      SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
      SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

      DBAL_QUAL_VALS_CLEAR(qual_vals);
      DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->ip_address, route_key->prefix_len);
      DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[1], vrf_ndx, SOC_SAND_U32_MAX);

      if (route_key->prefix_len != 32) {
          
          table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM;
      }

      res = arad_pp_dbal_entry_get(unit, table_id, qual_vals, &payload, 0, accessed, found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } else {
      ARAD_PP_LEM_ACCESS_REQUEST request;
      ARAD_PP_LEM_ACCESS_ACK_STATUS ack;

      ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
      ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

      arad_pp_frwrd_ipv4_host_lem_request_key_build(
          unit,
          ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST,
          vrf_ndx,
          route_key->ip_address,
          &request.key
      );

      res = arad_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          found
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if (*found)
  {
      res = arad_pp_frwrd_em_dest_to_fec(
              unit,
              &payload,
              routing_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      
      ARAD_PP_LEM_ACCESS_HI_FROM_EEI(payload.asd, routing_info->mac_lsb);

      
      routing_info->native_vsi = payload.native_vsi;

      if (payload.flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED) {
          *accessed = TRUE;
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_lem_route_get()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ipv4_lem_route_remove_unsafe(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key
    )
{
  uint32
    res, success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_LEM_ROUTE_REMOVE);
  
  SOC_SAND_CHECK_NULL_INPUT(route_key);

  if (SOC_IS_JERICHO(unit)) {
      SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM;
      SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

      DBAL_QUAL_VALS_CLEAR(qual_vals);
      DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[0], route_key->ip_address, route_key->prefix_len);
      DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[1], vrf_ndx, SOC_SAND_U32_MAX);

      if (route_key->prefix_len != 32) {
          
          table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM;
      }

      res = arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } else {
      ARAD_PP_LEM_ACCESS_REQUEST request;
      ARAD_PP_LEM_ACCESS_ACK_STATUS ack;

      ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
      ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

      request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
      arad_pp_frwrd_ipv4_host_lem_request_key_build(
          unit,
          ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST,
          vrf_ndx,
          route_key->ip_address,
          &request.key
      );

      res = arad_pp_lem_access_entry_remove_unsafe(
                unit,
                &request,
                &ack);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_lem_route_remove()",0,0);

}

STATIC uint32
  arad_pp_frwrd_ipv4_uc_or_vpn_route_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_IN  uint32                                      fec_id,
    SOC_SAND_IN  uint8                                     is_pending_op,
    SOC_SAND_IN  uint8                                     route_scale,
    SOC_SAND_IN  uint32                                      flags,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{

  uint32
    res = SOC_SAND_OK;
  
#if defined(INCLUDE_KBP)
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
      routing_info;
#endif
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_ADD);
    
#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM) 
  {
      

      SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&routing_info);
      routing_info.fec_id = fec_id;
      routing_info.frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
      routing_info.frwrd_decision.dest_id = fec_id;
      routing_info.frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
      routing_info.frwrd_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;

      res = arad_pp_frwrd_ipv4_uc_rpf_kbp_route_add(
                unit,
                vrf_ndx,
                route_key,
                &routing_info,
                route_scale,
                flags,
                success);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
  }
#if defined(BCM_JERICHO_SUPPORT)
  else if (JER_KAPS_ENABLE(unit))
  {
      
      res = arad_pp_frwrd_ipv4_uc_rpf_kaps_dbal_route_add(
                unit,
                vrf_ndx,
                route_key,
                fec_id,
                route_scale,
                flags,
                success);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
  }
#endif 
  else
#endif
  if (SOC_IS_JERICHO(unit)) {
      
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR, SOC_SAND_NULL_POINTER_ERR, exit);
  } else 
  {
      res = arad_pp_frwrd_ipv4_lpm_route_add(
              unit,
              vrf_ndx,
              route_key,
              fec_id,
              is_pending_op,
              SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (*success == SOC_SAND_SUCCESS)
      {
        res = arad_pp_sw_db_ipv4_cache_vrf_modified_set(unit,vrf_ndx,TRUE);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_or_vpn_route_add()", 0, 0);
}


STATIC uint32
  arad_pp_frwrd_ipv4_uc_or_vpn_route_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_IN  uint8                                     get_flags,
    SOC_SAND_IN  uint8                                     route_scale,
    SOC_SAND_IN  uint32                                       flags,
    SOC_SAND_OUT uint32                                      *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS         *pending_type,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;
#if defined(INCLUDE_KBP)
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
      routing_info;
#if defined(BCM_JERICHO_SUPPORT)
  uint32 kaps_fec_id;
#endif
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  SOC_SAND_CHECK_NULL_INPUT(found);
  
#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM) 
  {
      

      SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&routing_info);

      res = arad_pp_frwrd_ipv4_uc_rpf_kbp_route_get(
                unit, 
                vrf_ndx,
                route_key,
                route_scale,
				FALSE,
                flags,
				NULL,
                &routing_info, 
                found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (*found) {
          *fec_id = routing_info.frwrd_decision.dest_id;
          *pending_type = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
          *location = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_TCAM;
      }
  }
#if defined(BCM_JERICHO_SUPPORT)
  else if (JER_KAPS_ENABLE(unit))
  {
      uint8 hit_bit = 0;
      if (get_flags & SOC_PPC_FRWRD_IP_CLEAR_ON_GET) {
          hit_bit = SOC_DPP_DBAL_HITBIT_FLAG_CLEAR;
      }
      res = arad_pp_frwrd_ipv4_uc_rpf_kaps_dbal_route_get(
                unit,
                vrf_ndx,
                route_key,
                route_scale,
                &hit_bit,
                &kaps_fec_id,
                found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

      if (*found) {
          *fec_id = kaps_fec_id;
          *pending_type = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
          *location = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM;
          if (hit_bit) {
              *pending_type |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
          }
      }
  }
#endif 
  else 
#endif
  if (SOC_IS_JERICHO(unit)) {
      
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR, SOC_SAND_NULL_POINTER_ERR, exit);
  } else 
  {
      res = arad_pp_frwrd_ipv4_lpm_route_get(
              unit,
              vrf_ndx,
              route_key,
              get_flags,
              fec_id,
              pending_type,
              location,
              found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
     
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_or_vpn_route_get()", 0, 0);
}


STATIC uint32
  arad_pp_frwrd_ipv4_uc_or_vpn_route_get_block_unsafe(
  SOC_SAND_IN  int                                     unit,
  SOC_SAND_IN  uint32                                      vrf_ndx,
    SOC_SAND_INOUT  SOC_PPC_IP_ROUTING_TABLE_RANGE                  *block_range,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_OUT uint32                                      *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS         *pending_type,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint32                                      *nof_entries
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET_BLOCK);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    res = arad_pp_frwrd_ipv4_lpm_route_get_block(
            unit,
            vrf_ndx,
            0,
            block_range,
            route_key,
            fec_id,
            pending_type,
            location,
            nof_entries
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_or_vpn_route_get_block()", 0, 0);
}


STATIC uint32
  arad_pp_frwrd_ipv4_uc_or_vpn_route_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      vrf_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_IN  uint8                                     is_pending_op,
    SOC_SAND_IN  uint8                                     route_scale,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    success_bool = TRUE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_REMOVE);
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  
#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM) 
  {
      res = arad_pp_frwrd_ipv4_uc_rpf_kbp_route_remove(
                unit,
                vrf_ndx,
                route_key,
                route_scale);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
  }
# if defined(BCM_JERICHO_SUPPORT)
  else if (JER_KAPS_ENABLE(unit))
  {
      res = arad_pp_frwrd_ipv4_uc_rpf_kaps_dbal_route_remove(
                unit,
                vrf_ndx,
                route_key,
                route_scale);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
  }
#endif 
  else 
#endif
  if (SOC_IS_JERICHO(unit)) {
      
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR, SOC_SAND_NULL_POINTER_ERR, exit);
  } else 
  {
      
      res = arad_pp_frwrd_ipv4_lpm_route_remove(
              unit,
              vrf_ndx,
              route_key,
              is_pending_op,
              &success_bool
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (success_bool)
      {
        res = arad_pp_sw_db_ipv4_cache_vrf_modified_set(unit,vrf_ndx,TRUE);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
  }

exit:
  *success = (success_bool == TRUE) ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_or_vpn_route_remove()", 0, 0);
}



STATIC
  uint32
    arad_pp_frwrd_ipv4_uc_or_vpn_routing_table_clear(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_OR_VPN_ROUTING_TABLE_CLEAR);

#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM) 
  {
      res = arad_pp_frwrd_ipv4_uc_rpf_kbp_table_clear(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (JER_KAPS_ENABLE(unit))
  {
      res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

      if (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) {
          res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS);
          SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }
  else
#endif
  {
      
      res = arad_pp_frwrd_ipv4_lpm_vrf_clear(
              unit,
              vrf_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_or_vpn_routing_table_clear()", 0, 0);
}


STATIC uint32
  arad_pp_frwrd_ipv4_vrf_modified_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT  uint8                                 *modified
  )
{
  uint32
    *vrf_used_bmp = NULL;
  uint32
    tmp,
    indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  vrf_used_bmp = (uint32 *)soc_sand_os_malloc(sizeof(uint32) * ARAD_PP_IPV4_VRF_BITMAP_SIZE, "vrf_used_bmp");
  if (vrf_used_bmp == NULL)
  {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit);
  }

  res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_read(
          unit,
          0,
          0,
          32 * ARAD_PP_IPV4_VRF_BITMAP_SIZE,
          vrf_used_bmp
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (vrf_ndx == SOC_PPC_FRWRD_IP_ALL_VRFS_ID)
  {
    for (indx = 0 ; indx < ARAD_PP_IPV4_VRF_BITMAP_SIZE; ++indx)
    {
      if (vrf_used_bmp[indx]!= 0)
      {
        *modified = TRUE;
        goto exit;
      }
    }
    *modified = FALSE;
  }
  else
  {
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(vrf_used_bmp,vrf_ndx,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    *modified = (uint8)tmp;
  }

exit:
  if (vrf_used_bmp != NULL)
  {
      soc_sand_os_free(vrf_used_bmp);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_modified_get()", vrf_ndx, 0);
}


STATIC uint32
  arad_pp_frwrd_ipv4_vrf_independent_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  )
{
  uint32
    res;
  uint8
    is_shared;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  if (vrf_ndx == SOC_PPC_FRWRD_IP_ALL_VRFS_ID)
  {
      goto exit;
  }

  res = arad_pp_sw_db_ipv4_is_shared_lpm_memory(unit,vrf_ndx,&is_shared);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      
        
  if(is_shared) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_OPERATION_NOT_SUPPORTED_ON_VRF_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_modified_get()", vrf_ndx, 0);
}


STATIC
  void
    arad_pp_frwrd_global_ipv4mc_lem_request_key_build(
       SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY_TYPE type,
      SOC_SAND_IN SOC_PPC_RIF_ID rif_id,
      SOC_SAND_IN uint32 dip,
      SOC_SAND_IN uint32 sip,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY *key
    )
{
  uint32
    num_bits;

  ARAD_PP_LEM_ACCESS_KEY_clear(key);
  
  key->type = type;

  key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_GLOBAL_IPV4_MC;
  
  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_GLOBAL_IPV4_MC;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = (dip & 0xFFFFFFF); 

  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_GLOBAL_IPV4_MC;
  key->param[1].nof_bits = (uint8)num_bits;
  key->param[1].value[0] = sip;

  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_GLOBAL_IPV4_MC;
  key->param[2].nof_bits = (uint8)num_bits;
  key->param[2].value[0] = rif_id; 

  key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_GLOBAL_IPV4_MC;
  key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_GLOBAL_IPV4_MC;
}


STATIC
  void
    arad_pp_frwrd_global_ipv4mc_lem_request_key_parse(
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY               *key,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY      *route_keys
    )
{
  
  route_keys->group = key->param[0].value[0];
  route_keys->group |= 0xE0000000;
  route_keys->source.ip_address = key->param[1].value[0];
  route_keys->source.prefix_len = sizeof(route_keys->source.ip_address);
  route_keys->inrif = key->param[2].value[0];
  route_keys->inrif_valid = 1;
}


STATIC
  uint32
    arad_pp_frwrd_global_ipv4mc_lem_payload_build(
      SOC_SAND_IN int                                  unit,  
      SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID           *dest_id,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD          *payload
    )
{
  SOC_PPC_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_DECISION_INFO_clear(&em_dest);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);

  res = arad_pp_frwrd_ipv4_sand_dest_to_fwd_decision(
          unit,
          dest_id,
          &em_dest
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          &em_dest,
          &payload->dest,
          &payload->asd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if(em_dest.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE) {
    payload->flags = ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
  }
  else if(em_dest.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
    payload->flags = ARAD_PP_FWD_DECISION_PARSE_EEI;
  }
  else{
    payload->flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_global_ipv4mc_lem_payload_build()", 0, 0);
}

STATIC
  uint32
    arad_pp_frwrd_global_ipv4mc_lem_route_add(
      SOC_SAND_IN  int                                          unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO            *routing_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
    )

{
  uint32
    res;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
  arad_pp_frwrd_global_ipv4mc_lem_request_key_build(
     unit,
    ARAD_PP_LEM_ACCESS_KEY_TYPE_GLOBAL_IPV4_MC,
    route_key->inrif,
    route_key->group,
    route_key->source.ip_address,
    &request.key
  );

  res = arad_pp_frwrd_global_ipv4mc_lem_payload_build(
    unit,
    &(routing_info->dest_id),
    &payload
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  if(ack.is_success)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_global_ipv4mc_lem_route_add()",0,0);
}


STATIC
  uint32
    arad_pp_frwrd_global_ipv4mc_lem_route_get(
      SOC_SAND_IN  int                                        unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY           *route_key,
      SOC_SAND_OUT  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO         *routing_info,
      SOC_SAND_OUT uint8                                     *found,
      SOC_SAND_OUT uint8                                     *accessed
    )
{
  uint32
    res;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  *accessed = FALSE;
  arad_pp_frwrd_global_ipv4mc_lem_request_key_build(
     unit,
    ARAD_PP_LEM_ACCESS_KEY_TYPE_GLOBAL_IPV4_MC,
    route_key->inrif,
    route_key->group,
    route_key->source.ip_address,
    &request.key
  );

  res = arad_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!(*found))
  {
    goto exit;
  }

  res = arad_pp_frwrd_ipv4_em_dest_to_sand_dest(
                                                unit,
                                                payload.dest,
                                               &routing_info->dest_id
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (payload.flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED) {
    *accessed = TRUE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_global_ipv4mc_lem_route_get()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_global_ipv4mc_lem_route_remove(
      SOC_SAND_IN  int                                        unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY           *route_key
    )
{
  uint32
    res;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(route_key);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
  arad_pp_frwrd_global_ipv4mc_lem_request_key_build(
     unit,
    ARAD_PP_LEM_ACCESS_KEY_TYPE_GLOBAL_IPV4_MC,
    route_key->inrif,
    route_key->group,
    route_key->source.ip_address,
    &request.key
  );

  res = arad_pp_lem_access_entry_remove_unsafe(
          unit,
          &request,
          &ack);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);     
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_global_ipv4mc_lem_route_remove()",0,0);
}


STATIC
  uint32
    arad_pp_frwrd_global_ipv4mc_lem_get_block_unsafe(
      SOC_SAND_IN  int                                        unit,
      SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE          *block_range_key,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY           *route_keys,
      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO          *routes_info,
      SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS             *routes_status,
      SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION           *routes_location,
      SOC_SAND_OUT    uint32                                 *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    read_index = 0,
    valid_index = 0;
  ARAD_PP_LEM_ACCESS_KEY            key;
  ARAD_PP_LEM_ACCESS_KEY            key_mask;
  uint32                            access_only=0;
  ARAD_PP_LEM_ACCESS_KEY            *read_keys=NULL;
  ARAD_PP_LEM_ACCESS_PAYLOAD        *read_vals=NULL;
  SOC_SAND_TABLE_BLOCK_RANGE        block_range;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
  *nof_entries = 0;

  block_range.entries_to_act = block_range_key->entries_to_act;
  block_range.entries_to_scan = block_range_key->entries_to_scan;
  block_range.iter = block_range_key->start.payload.arr[0];

  
  arad_pp_frwrd_global_ipv4mc_lem_request_key_build(
     unit,
      ARAD_PP_LEM_ACCESS_KEY_TYPE_GLOBAL_IPV4_MC,
      0,
      0,
      0,
      &key
   );

  
  key_mask.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_GLOBAL_IPV4_MC;
  key_mask.prefix.value = SOC_IS_JERICHO(unit)? ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED: 0x3;
  key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_GLOBAL_IPV4_MC;

  
  read_keys = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range.entries_to_act,"read_keys");
  read_vals = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range.entries_to_act,"read_vals");

  res = arad_pp_frwrd_lem_get_block_unsafe(unit,&key,&key_mask,NULL,access_only,&block_range,read_keys,read_vals,nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  for (read_index = 0; read_index < *nof_entries; ++read_index)
  {
    if ((read_keys[read_index].type == ARAD_PP_LEM_ACCESS_KEY_TYPE_GLOBAL_IPV4_MC))
    {
      arad_pp_frwrd_global_ipv4mc_lem_request_key_parse(
                                                        &read_keys[read_index],
                                                        &route_keys[valid_index]
                                                        );

      
      routes_status[valid_index] = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;

      if(read_vals[read_index].flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED){
        routes_status[valid_index] |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
      }
      routes_location[valid_index] = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_HOST;

      res = arad_pp_frwrd_ipv4_em_dest_to_sand_dest(
                                                unit,
                                                read_vals[read_index].dest,
                                               &routes_info[valid_index].dest_id
         );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      ++valid_index;
    }
  }

  *nof_entries =  valid_index;
  
  if (SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
    block_range_key->start.payload.arr[0] = SOC_SAND_UINT_MAX;
    block_range_key->start.payload.arr[1] = SOC_SAND_UINT_MAX;
  } else {
    block_range_key->start.payload.arr[0] = block_range.iter;
  }

exit:
  if (read_keys!=NULL) {
    soc_sand_os_free_any_size(read_keys);
  }

  if (read_vals!=NULL) {
    soc_sand_os_free_any_size(read_vals);
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_global_ipv4mc_lem_get_block_unsafe()", 0, 0);
}


STATIC
  uint32
    arad_pp_frwrd_global_ipv4mc_lem_clear_all_unsafe(
      SOC_SAND_IN  int                                        unit
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_KEY            key;
  ARAD_PP_LEM_ACCESS_KEY            key_mask;
  SOC_SAND_TABLE_BLOCK_RANGE        block_range;
  uint32                            nof_entries;
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        action;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

  
  arad_pp_frwrd_global_ipv4mc_lem_request_key_build(
     unit,
      ARAD_PP_LEM_ACCESS_KEY_TYPE_GLOBAL_IPV4_MC,
      0,
      0,
      0,
      &key
   );

  
  key_mask.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_GLOBAL_IPV4_MC;
  key_mask.prefix.value = SOC_IS_JERICHO(unit)? ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED: 0x3;
  key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_GLOBAL_IPV4_MC;

  
  action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

  res = arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&key,&key_mask,NULL,&block_range,&action, TRUE,&nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_global_ipv4mc_lem_clear_all_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_ip_routes_cache_mode_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                  route_types
  )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint8
    cache;
  uint32
    nof_vrfs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_UNSAFE);

  if (SOC_IS_JERICHO(unit)) {
      
      ARAD_DO_NOTHING_AND_EXIT;
  }

  
  res = arad_pp_frwrd_ipv4_vrf_independent_verify(unit,vrf_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!(lpm_mngr.init_info.flags & ARAD_PP_LPV4_LPM_SUPPORT_CACHE))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_CACHE_NOT_SUPPORTED_ERR, SOC_SAND_NULL_POINTER_ERR, exit);
  }

  if (vrf_ndx == SOC_PPC_FRWRD_IP_ALL_VRFS_ID)
  {
    res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.get(
            unit,
            &nof_vrfs
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    nof_vrfs = 1;
  }

  cache = (uint8)(route_types & SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM);
  res = arad_pp_ipv4_lpm_mngr_cache_set(&lpm_mngr,vrf_ndx,nof_vrfs,cache);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (!cache)
  {
    res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_clear(
            unit,
            0,
            32 * ARAD_PP_IPV4_VRF_BITMAP_SIZE
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
  }

  
  res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_mode.set(unit,route_types);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_routes_cache_mode_enable_set_unsafe()", vrf_ndx, 0);
}

uint32
  arad_pp_frwrd_ip_routes_cache_mode_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                  route_types
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_VERIFY);

  if (vrf_ndx != SOC_PPC_FRWRD_IP_ALL_VRFS_ID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MUST_ALL_VRF_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(route_types, ARAD_PP_FRWRD_IPV4_ROUTE_TYPES_MAX, ARAD_PP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_routes_cache_mode_enable_set_verify()", vrf_ndx, 0);
}

uint32
  arad_pp_frwrd_ip_routes_cache_mode_enable_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_VERIFY);

  if (vrf_ndx != SOC_PPC_FRWRD_IP_ALL_VRFS_ID)
  {
    SOC_SAND_ERR_IF_ABOVE_NOF(vrf_ndx, SOC_DPP_DEFS_GET(unit, nof_vrfs), SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_routes_cache_mode_enable_get_verify()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ip_routes_cache_mode_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT uint32                                  *route_types
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_types);

  if (SOC_IS_JERICHO(unit)) {
      
      *route_types = SOC_SAND_PP_IP_TYPE_NONE;
      ARAD_DO_NOTHING_AND_EXIT;
  }


  res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.cache_mode.get(unit,route_types);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_routes_cache_mode_enable_get_unsafe()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ip_routes_cache_commit_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint8
    changed=0;
  uint32
    vrf_id,
    nof_vrfs,
    vrf_used_bmp[ARAD_PP_IPV4_VRF_BITMAP_SIZE];
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  if (SOC_IS_JERICHO(unit)) {
      
      *success = SOC_SAND_SUCCESS;
      ARAD_DO_NOTHING_AND_EXIT;
  }


  
  res = arad_pp_frwrd_ipv4_vrf_independent_verify(unit,vrf_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 
  res = arad_pp_frwrd_ipv4_vrf_modified_get(
          unit,
          vrf_ndx,
          &changed
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (!changed)
  {
    *success = SOC_SAND_SUCCESS;
    goto exit;
  }
  
  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_read(
          unit,
          0,
          0,
          32 * ARAD_PP_IPV4_VRF_BITMAP_SIZE,
          vrf_used_bmp
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  if (vrf_ndx == SOC_PPC_FRWRD_IP_ALL_VRFS_ID)
  {
    res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.get(
            unit,
            &nof_vrfs
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
   
    vrf_id = 0;
  }
  else
  {
    nof_vrfs = 1;
    vrf_id = vrf_ndx;
  }

  
  res = arad_pp_ipv4_lpm_mngr_sync(
          &lpm_mngr,
          vrf_id,
          vrf_used_bmp,
          nof_vrfs,
          TRUE,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (*success!=SOC_SAND_SUCCESS)
  {
    goto exit;
  }
  
  if (vrf_ndx == SOC_PPC_FRWRD_IP_ALL_VRFS_ID)
  {
    res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.vrf_modified_bitmask.bit_range_clear(
            unit,
            0,
            32 * ARAD_PP_IPV4_VRF_BITMAP_SIZE
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
  }
  else
  {
    res = arad_pp_sw_db_ipv4_cache_vrf_modified_set(unit,vrf_ndx,FALSE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_routes_cache_commit_unsafe()", vrf_ndx, 0);
}

STATIC uint32
  arad_pp_frwrd_ipv4_vrf_is_supported_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint8                                 all_permitted
  )
{
  uint32
    nof_vrfs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_IS_SUPPORTED_VERIFY);

  if (SOC_IS_JERICHO(unit)) {
      
      ARAD_DO_NOTHING_AND_EXIT;
  }


  res = sw_state_access[unit].dpp.soc.arad.pp.ipv4_info.nof_vrfs.get(
          unit,
          &nof_vrfs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (all_permitted && vrf_ndx == SOC_PPC_FRWRD_IP_ALL_VRFS_ID)
  {
    goto exit;
  }

  if (vrf_ndx >= nof_vrfs)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR,101,exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_is_supported_verify()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ip_routes_cache_commit_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(route_types, ARAD_PP_FRWRD_IPV4_ROUTE_TYPES_MAX, ARAD_PP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR, 10, exit);

  res = arad_pp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_routes_cache_commit_verify()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ipv4_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
	  res = arad_pp_frwrd_ipv4_def_action_set(
			  unit,
			  0,
			  FALSE,
			  &glbl_info->router_info.uc_default_action
			);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	  res = arad_pp_frwrd_ipv4_def_action_set(
			  unit,
			  0,
			  TRUE,
			  &glbl_info->router_info.mc_default_action
			);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  
  res = arad_pp_ipmc_not_found_proc_update(unit,glbl_info->router_info.flood_unknown_mc);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  fld_val = glbl_info->router_info.rpf_default_route;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_RPF_CFGr, REG_PORT_ANY, 0, RPF_ALLOW_DEFAULT_VALUEf,  fld_val));

  if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "lpm_l2_mc", 0)){
      res = arad_pp_ipv4mc_bridge_lookup_update(unit,glbl_info->router_info.ipv4_mc_l2_lookup);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_glbl_info_set_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_ipv4_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_GLBL_INFO, glbl_info, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_glbl_info_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_glbl_info_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPC_FRWRD_IPV4_GLBL_INFO_clear(glbl_info);

  glbl_info->router_info.uc_default_action.type = 0;
  glbl_info->router_info.mc_default_action.type = 0;

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
	  res = arad_pp_frwrd_ipv4_def_action_get(
			  unit,
			  0,
			  FALSE,
			  &glbl_info->router_info.uc_default_action
			);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	  res = arad_pp_frwrd_ipv4_def_action_get(
			  unit,
			  0,
			  TRUE,
			  &glbl_info->router_info.mc_default_action
			);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  
  res = arad_pp_ipmc_not_found_proc_get(unit,&glbl_info->router_info.flood_unknown_mc);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_ipv4mc_bridge_lookup_get(unit,&glbl_info->router_info.ipv4_mc_l2_lookup);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_RPF_CFGr, REG_PORT_ANY, 0, RPF_ALLOW_DEFAULT_VALUEf, &fld_val));
  glbl_info->router_info.rpf_default_route = fld_val;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_glbl_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_uc_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    pending_op;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD_UNSAFE);

  res = arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_frwrd_ipv4_uc_or_vpn_route_add_unsafe(
          unit,
          0,
          &route_key->subnet,
          fec_id,
          pending_op,
          route_key->route_scale,
          0, 
          success
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_route_add_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_uc_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY, route_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fec_id, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 20, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
    &route_key->subnet
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_route_add_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_uc_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  
  SOC_SAND_CHECK_NULL_INPUT(found);
  
  res = arad_pp_frwrd_ipv4_uc_or_vpn_route_get_unsafe(
          unit,
          0,
          &route_key->subnet,
          exact_match,
          route_key->route_scale,
          0, 
          fec_id,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_route_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_uc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY, route_key, 10, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
          &route_key->subnet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_route_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_uc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    i;
  SOC_SAND_PP_IPV4_SUBNET
    *subnets = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(fec_ids);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    subnets = soc_sand_os_malloc(sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range->entries_to_act,"subnets");
    if (subnets == NULL)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit);
    }
    soc_sand_os_memset(subnets, 0, sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range->entries_to_act);

    res = arad_pp_frwrd_ipv4_uc_or_vpn_route_get_block_unsafe(
            unit,
            0,
            block_range,
            subnets,
            fec_ids,
            routes_status,
            routes_location,
            nof_entries
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    for(i = 0; i < *nof_entries; i++)
    {
      SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_clear(&route_keys[i]);
      soc_sand_os_memcpy(&(route_keys[i].subnet), &(subnets[i]), sizeof(SOC_SAND_PP_IPV4_SUBNET));
    }

exit:
  if (subnets)
  {
    soc_sand_os_free(subnets);
  }
  
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_route_get_block_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_uc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_IP_ROUTING_TABLE_RANGE, block_range, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_route_get_block_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_uc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    pending_op;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);
  
  res = arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_frwrd_ipv4_uc_or_vpn_route_remove_unsafe(
          unit,
          0,
          &route_key->subnet,
          pending_op,
          route_key->route_scale,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_route_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_uc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY, route_key, 10, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
          &route_key->subnet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_route_remove_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_uc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_UNSAFE);

  res = arad_pp_frwrd_ipv4_uc_or_vpn_routing_table_clear(
          unit,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_routing_table_clear_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_uc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_uc_routing_table_clear_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_host_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  )
{
  SOC_SAND_PP_IPV4_SUBNET
    route_key;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  soc_sand_SAND_PP_IPV4_SUBNET_clear(&route_key);
  route_key.ip_address = host_key->ip_address;
  route_key.prefix_len = 32;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM && (!host_key->is_local_host)) 
  {
      res = arad_pp_frwrd_ipv4_uc_rpf_kbp_route_add(
                unit,
                host_key->vrf_ndx,
                &route_key,
                routing_info,
                host_key->route_scale,
                routing_info->frwrd_decision.flags,
                success);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
  }
  else 
#endif
  {
      route_key.prefix_len = host_key->route_scale ? (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) : 32;
      res = arad_pp_frwrd_ipv4_lem_route_add(
              unit,
              host_key->vrf_ndx,
              &route_key,
              routing_info,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_add_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_host_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_ADD_VERIFY);

  res = SOC_PPC_FRWRD_IPV4_HOST_KEY_verify(unit, host_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_verify(unit, routing_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_add_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_host_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  )
{
  uint8
    accessed = FALSE;
  uint32
    old_fld_val,
    fld_val;
  uint32
    res = SOC_SAND_OK;
#if defined(INCLUDE_KBP)
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY 
      route_key;
#endif
  SOC_SAND_PP_IPV4_SUBNET
    route_subnet;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(found);

  soc_sand_SAND_PP_IPV4_SUBNET_clear(&route_subnet);
  route_subnet.ip_address = host_key->ip_address;
  route_subnet.prefix_len = 32;

#if defined(INCLUDE_KBP)

  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM && (!host_key->is_local_host))
  {
      SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&route_key);

      route_key.subnet.ip_address = host_key->ip_address;
      route_key.subnet.prefix_len = 32;

      res = arad_pp_frwrd_ipv4_uc_rpf_kbp_route_get(
                unit, 
                host_key->vrf_ndx,
                &route_key.subnet,
                host_key->route_scale,
				TRUE,
                routing_info->frwrd_decision.flags,
				&accessed,
                routing_info, 
                found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	  if (*found) {
		  *route_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
		  if (accessed) {
			*route_status |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
		  }
		  *location = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_TCAM;
	  }
  }
  else 
#endif
  {
      SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(routing_info);

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0, LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf, &old_fld_val));

      
      fld_val = (flags & SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET)? 0x1:0x2;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0, LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf,  fld_val));

      route_subnet.prefix_len = host_key->route_scale ? (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) : 32;
      res = arad_pp_frwrd_ipv4_lem_route_get(
              unit,
              host_key->vrf_ndx,
              &route_subnet,
              routing_info,
              found,
              &accessed
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (old_fld_val != fld_val) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  13,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0, LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf,  old_fld_val));
      }

      
      *route_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
      if (accessed) {
        *route_status |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
      }
      *location = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_HOST;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_host_get_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_GET_VERIFY);

  res = SOC_PPC_FRWRD_IPV4_HOST_KEY_verify(unit, host_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_host_get_block_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
#if defined(INCLUDE_KBP)
    *vrf_ndx_out = NULL,
#endif 
    read_index = 0,
    valid_index = 0;
  ARAD_PP_LEM_ACCESS_KEY            key;
  ARAD_PP_LEM_ACCESS_KEY            key_mask;
  uint32                            access_only=0;
  ARAD_PP_LEM_ACCESS_KEY            *read_keys=NULL;
  ARAD_PP_LEM_ACCESS_PAYLOAD        *read_vals=NULL;
#if defined(INCLUDE_KBP)
  SOC_SAND_PP_IPV4_SUBNET           *route_keys_uc= NULL;
#endif 


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(host_keys);
  SOC_SAND_CHECK_NULL_INPUT(routes_info);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM)
  {
    SOC_PPC_IP_ROUTING_TABLE_RANGE          
        block_range_key;

    
    SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&block_range_key);
    block_range_key.entries_to_act = block_range->entries_to_act;
    block_range_key.entries_to_scan = block_range->entries_to_scan;
    block_range_key.start.payload.arr[0] = block_range->iter;

    
    vrf_ndx_out = soc_sand_os_malloc_any_size(sizeof(uint32) * block_range->entries_to_act, "vrf_ndx_out");
    route_keys_uc = soc_sand_os_malloc_any_size(sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range->entries_to_act, "vrf_ndx_out");
    if ((!vrf_ndx_out) || (!route_keys_uc))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 42, exit);
    }
    res = soc_sand_os_memset(vrf_ndx_out, 0x0, sizeof(uint32) * block_range->entries_to_act);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    res = soc_sand_os_memset(route_keys_uc, 0x0, sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range->entries_to_act);
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

    res = arad_pp_frwrd_ipv4_kbp_route_get_block_unsafe(
            unit,
            ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0,
            TRUE ,
            0 ,
            &block_range_key,
            vrf_ndx_out,
            route_keys_uc,
            NULL,
            routes_info,
            NULL,
            routes_status,
            NULL,
            nof_entries
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
    block_range->iter = block_range_key.start.payload.arr[0];

    
    for (read_index = 0; read_index < *nof_entries; ++read_index)
    {
      host_keys[read_index].ip_address = route_keys_uc[read_index].ip_address;
      host_keys[read_index].vrf_ndx = vrf_ndx_out[read_index];
    }
  }
  else
#endif 
  {
      ARAD_PP_LEM_ACCESS_KEY_clear(&key);
      ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);


      *nof_entries = 0;

      if(flags & SOC_PPC_FRWRD_IP_HOST_GET_ACCESSED_ONLY){
          access_only |= ARAD_PP_FRWRD_LEM_GET_BLOCK_ACCESSED_ONLY;
      }

      if(flags & SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET){
          access_only |= ARAD_PP_FRWRD_LEM_GET_BLOCK_CLEAR_ACCESSED;
      }
      
      
      arad_pp_frwrd_ipv4_host_lem_request_key_build(
         unit,
          ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST,
          0,
          0,
          &key
       );

      
      key.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST;
      key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
      key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST;

      
      read_keys = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range->entries_to_act,"read_keys");
      read_vals = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range->entries_to_act,"read_vals");


      res = arad_pp_frwrd_lem_get_block_unsafe(unit,&key,&key_mask,NULL,access_only ,block_range,read_keys,read_vals,nof_entries);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      
      for ( read_index = 0; read_index < *nof_entries; ++read_index)
      {
        if ((read_keys[read_index].type == ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST))
        {

          arad_pp_frwrd_ipv4_host_lem_request_key_parse(
            &read_keys[read_index],
            &host_keys[valid_index]
          );

         

          routes_status[valid_index] = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;

          if(read_vals[read_index].flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED){
              routes_status[valid_index] |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
          }

          res = arad_pp_frwrd_em_dest_to_fec(
                    unit,
                    &read_vals[read_index],
                    &routes_info[valid_index]
                  );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
         ++valid_index;

        }
      }

      *nof_entries =  valid_index;
  }
exit:

  if (read_keys!=NULL) {
      soc_sand_os_free_any_size(read_keys);
  }

  if (read_vals!=NULL) {
      soc_sand_os_free_any_size(read_vals);
  }
#if defined(INCLUDE_KBP)
  if (vrf_ndx_out)
  {
    soc_sand_os_free_any_size(vrf_ndx_out);
  }
  if (route_keys_uc)
  {
    soc_sand_os_free_any_size(route_keys_uc);
  }
#endif 


  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_get_block_unsafe()", 0, 0);
}



STATIC uint32
  arad_pp_frwrd_ipv4_host_vrf_clear_unsafe(
    SOC_SAND_IN  int                                  unit
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_KEY            key;
  ARAD_PP_LEM_ACCESS_KEY            key_mask;
  SOC_SAND_TABLE_BLOCK_RANGE        block_range;
  uint32                            nof_entries;
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        action;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_UNSAFE);


  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);


  
  arad_pp_frwrd_ipv4_host_lem_request_key_build(
     unit,
      ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST,
      0,
      0,
      &key
   );

  
  key.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST;
  key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
  key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST;


  
  action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

  res = arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&key,&key_mask,NULL,&block_range,&action, TRUE,&nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_get_block_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_host_get_block_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_VERIFY);


  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_get_block_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_host_remove_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key
  )
{
  SOC_SAND_PP_IPV4_SUBNET
    route_key;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(host_key);

  soc_sand_SAND_PP_IPV4_SUBNET_clear(&route_key);
  route_key.ip_address = host_key->ip_address;
  route_key.prefix_len = 32;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM && (!host_key->is_local_host) ) 
  {
      res = arad_pp_frwrd_ipv4_uc_rpf_kbp_route_remove(
                unit,
                host_key->vrf_ndx,
                &route_key,
                host_key->route_scale);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
  }
  else 
#endif
  {
      route_key.prefix_len = host_key->route_scale ? (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) : 32;
      res = arad_pp_frwrd_ipv4_lem_route_remove_unsafe(
              unit,
              host_key->vrf_ndx,
              &route_key
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_host_remove_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_REMOVE_VERIFY);

  res = SOC_PPC_FRWRD_IPV4_HOST_KEY_verify(unit, host_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_remove_verify()", 0, 0);
}



uint32
  arad_pp_frwrd_ipv4_mc_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  
  if (!(SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable) &&
       route_key->inrif_valid &&
       route_key->group &&
       route_key->source.prefix_len) {
      res = arad_pp_frwrd_global_ipv4mc_lem_route_add(unit, route_key,route_info,success);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  } else {

#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_MC) 
    {
        res = arad_pp_frwrd_ipv4_mc_rpf_kbp_route_add(
                  unit,
                  route_key,
                  route_info,
                  success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else
#endif
    {
        res = arad_pp_frwrd_ipv4_mc_rpf_dbal_route_add(
                  unit,
                  route_key,
                  route_info,
                  success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipv4_mc_route_add_unsafe()",0,0);
}

uint32
  arad_pp_frwrd_ipv4_mc_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *route_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD_VERIFY);

  res = SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_verify(unit, route_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(route_key->vrf_ndx, (SOC_DPP_DEFS_GET(unit, nof_vrfs) -1), 0, SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  
  if (!(route_info->flags & _BCM_IPMC_FLAGS_RAW_ENTRY)) {
      res = SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_verify(unit, route_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
      res = SOC_PPC_FRWRD_IPV4_MC_RAW_ROUTE_INFO_verify(unit, route_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mc_route_add_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_mc_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 get_flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32 res = SOC_SAND_OK;
  uint8 hit_bit = 0;
  uint8 accessed = FALSE;
  uint32 old_fld_val = 0;
  uint32 fld_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(found);

  
  if (!(SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable) &&
       route_key->inrif_valid &&
       route_key->group &&
       route_key->source.prefix_len) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0, LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf, &old_fld_val));
      
      fld_val = (get_flags & SOC_PPC_FRWRD_IP_CLEAR_ON_GET)? 0x1:0x2;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0, LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf,  fld_val));
      res = arad_pp_frwrd_global_ipv4mc_lem_route_get(unit, 
                                                      route_key,
                                                      route_info,
                                                      found,
                                                      &accessed);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      if (old_fld_val != fld_val) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  13,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0, LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf,  old_fld_val));
      }
      
      *route_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
      if (accessed) {
        *route_status |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
      }
      *location = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_HOST;
  } else {
#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_MC) 
    {
        res = arad_pp_frwrd_ipv4_mc_rpf_kbp_route_get(
                  unit,
                  route_key,
                  route_info,
                  found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        *route_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
        *location = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_TCAM;
    }
    else
#endif
    {
      if (get_flags & SOC_PPC_FRWRD_IP_CLEAR_ON_GET) {
          hit_bit = SOC_DPP_DBAL_HITBIT_FLAG_CLEAR;
      }
      res = arad_pp_frwrd_ipv4_mc_rpf_dbal_route_get(
                  unit,
                  route_key,
                  route_info,
                  found,
                  &hit_bit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

      if (*found) {
          *route_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
          *location = SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM;
          if (hit_bit) {
              *route_status |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
          }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mc_route_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_mc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_VERIFY);
  
  SOC_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(route_key->vrf_ndx, (SOC_DPP_DEFS_GET(unit, nof_vrfs) -1), 0, SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_verify(unit, route_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mc_route_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_mc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(routes_info);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(routes_location);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  if (block_range_key->entries_from_lem) {
    res = arad_pp_frwrd_global_ipv4mc_lem_get_block_unsafe(unit,
                                                           block_range_key,
                                                           route_keys,
                                                           routes_info,
                                                           routes_status,
                                                           routes_location,
                                                           nof_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } else {
#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_MC) 
    {
      res = arad_pp_frwrd_ipv4_kbp_route_get_block_unsafe(
              unit,
              ARAD_KBP_FRWRD_TBL_ID_IPV4_MC,
              FALSE, 
              0, 
              block_range_key,
              NULL,
              NULL,
              route_keys,
              NULL,
              routes_info,
              routes_status,
              routes_location,
              nof_entries
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
    }
  else
#endif
  {
      SOC_DPP_DBAL_TABLE_INFO table;
      uint32 vrf_ndx = (*block_range_key).start.payload.arr[1];
      

      res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, &table); 
#if defined(BCM_JERICHO_SUPPORT) && defined(INCLUDE_KBP)
      if (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
          if ((JER_KAPS_ENABLE_PUBLIC_DB(unit)) && (vrf_ndx == 0)) {
              
              res = arad_pp_frwrd_ipv4_dbal_route_get_block(
                      unit,
                      SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS,
                      0,
                      block_range_key,
                      NULL,
                      route_keys,
                      NULL,
                      routes_info,
                      routes_status,
                      routes_location,
                      nof_entries
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          }
          else {
              res = arad_pp_frwrd_ipv4_dbal_route_get_block(
                      unit,
                      SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS,
                      vrf_ndx,
                      block_range_key,
                      NULL,
                      route_keys,
                      NULL,
                      routes_info,
                      routes_status,
                      routes_location,
                      nof_entries
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);  
          }
      }
      else 
#endif
      {
          res = arad_pp_frwrd_ipv4_dbal_route_get_block(
                  unit,
                  SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS,
                  vrf_ndx,
                  block_range_key,
                  NULL,
                  route_keys,
                  NULL,
                  routes_info,
                  routes_status,
                  routes_location,
                  nof_entries
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit); 
      }
  }

    
    if (!(SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable) && 
        SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&(block_range_key->start.payload))) {
      block_range_key->entries_from_lem = TRUE;
      block_range_key->entries_to_act = ARAD_PP_FRWRD_IPV4_GET_BLOCK_LEM_NOF_ENTRIES;
      block_range_key->entries_to_scan = ARAD_PP_FRWRD_IPV4_LEM_ENTRIES_TO_SCAN;
      SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range_key->start.payload.arr[0]));
      SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range_key->start.payload.arr[1]));
    }
  }

exit:
  
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mc_route_get_block_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_mc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_IP_ROUTING_TABLE_RANGE, block_range_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mc_route_get_block_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_mc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32 res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);
  
  if (!(SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable) &&
       route_key->inrif_valid &&
       route_key->group &&
       route_key->source.prefix_len) {
    res = arad_pp_frwrd_global_ipv4mc_lem_route_remove(unit, 
                                                       route_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    *success = SOC_SAND_SUCCESS;
  } else {
#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_MC) 
    {
        res = arad_pp_frwrd_ipv4_mc_rpf_kbp_route_remove(
                  unit,
                  route_key
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        *success = SOC_SAND_SUCCESS;
    }
    else
#endif

    {
        res = arad_pp_frwrd_ipv4_mc_rpf_dbal_route_remove(
                  unit,
                  route_key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        *success = SOC_SAND_SUCCESS; 
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mc_route_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_mc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_VERIFY);
  
  SOC_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(route_key->vrf_ndx, (SOC_DPP_DEFS_GET(unit, nof_vrfs) -1), 0, SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_verify(unit, route_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mc_route_remove_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_mc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_UNSAFE);

#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_IPV4_MC) 
  {
    res = arad_pp_frwrd_ipv4_mc_rpf_kbp_table_clear(
                unit
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else{
    res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
  }
#endif
  
  if (!(SOC_DPP_CONFIG(unit)->l3.ipmc_vpn_lookup_enable)){
    res = arad_pp_frwrd_global_ipv4mc_lem_clear_all_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mc_routing_table_clear_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_mc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mc_routing_table_clear_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipmc_rp_add_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipmc_rp_add_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_ipmc_rp_get_block_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  rp_id,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range_key,
    SOC_SAND_OUT int32                                  *l3_intfs,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32  
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipmc_rp_get_block_verify()", 0, 0);
}



uint32
  arad_pp_frwrd_ipmc_rp_remove_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  )
{
  uint32  
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipmc_rp_remove_verify()", 0, 0);
}



uint32
  arad_pp_frwrd_ipmc_rp_add_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO   *rp_info
  )
{
  uint32
    res;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(rp_info);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
  arad_pp_frwrd_ipmc_rp_lem_request_key_build(
    unit,
    rp_info,
    &request.key
  );
  
  payload.is_dynamic = 0;
  
  res = arad_pp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  if(!ack.is_success)
  {
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipmc_rp_add_unsafe()",0,0);

}

uint32
  arad_pp_frwrd_ipmc_rp_get_block_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  rp_id,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range_key,
    SOC_SAND_OUT int32                                  *l3_intfs,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    read_index = 0,
    valid_index = 0;
  ARAD_PP_LEM_ACCESS_KEY            key;
  ARAD_PP_LEM_ACCESS_KEY            key_mask;
  uint32                            access_only=0;
  SOC_PPC_FRWRD_IP_MC_RP_INFO   rp_key, rp_mask;
  ARAD_PP_LEM_ACCESS_KEY            *read_keys=NULL;
  ARAD_PP_LEM_ACCESS_PAYLOAD        *read_vals=NULL;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(l3_intfs);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);


  *nof_entries = 0;

  
  SOC_PPC_FRWRD_IP_MC_RP_INFO_clear(&rp_key);
  SOC_PPC_FRWRD_IP_MC_RP_INFO_clear(&rp_mask);

  
  rp_key.rp_id = rp_id;
  arad_pp_frwrd_ipmc_rp_lem_request_key_build(
      unit,
      &rp_key,
      &key
   );

  
  rp_mask.rp_id = 0xFFFF;
  arad_pp_frwrd_ipmc_rp_lem_request_key_build(
      unit,
      &rp_mask,
      &key_mask
   );
  
  key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
  key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST;

  
  read_keys = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range_key->entries_to_act,"read_keys");
  read_vals = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range_key->entries_to_act,"read_vals");


  res = arad_pp_frwrd_lem_get_block_unsafe(unit,&key,&key_mask,NULL,access_only ,block_range_key,read_keys,read_vals,nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  SOC_PPC_FRWRD_IP_MC_RP_INFO_clear(&rp_key);
  
  for ( read_index = 0; read_index < *nof_entries; ++read_index)
  {
    if ((read_keys[read_index].type == ARAD_PP_LEM_ACCESS_KEY_TYPE_IPMC_BIDIR))
    {

      arad_pp_frwrd_ipmc_rp_lem_request_key_parse(
        &read_keys[read_index],
        &rp_key
      );

      if(rp_key.rp_id == rp_id)
      {
          l3_intfs[valid_index] = rp_key.inrif;
          ++valid_index;
      }
    }
  }

  *nof_entries =  valid_index;

exit:

    if (read_keys!=NULL) {
        soc_sand_os_free_any_size(read_keys);
    }

    if (read_vals!=NULL) {
        soc_sand_os_free_any_size(read_vals);
    }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_host_get_block_unsafe()", 0, 0);
}


STATIC  uint32
  arad_pp_frwrd_ipmc_rp_remove_one_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO   *rp_info
  )
{
  uint32
    res;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(rp_info);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
  arad_pp_frwrd_ipmc_rp_lem_request_key_build(
    unit,
    rp_info,
    &request.key
  );
  
  res = arad_pp_lem_access_entry_remove_unsafe(
          unit,
          &request,
          &ack);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ipmc_rp_remove_one_unsafe()",0,0);

}


STATIC uint32
  arad_pp_frwrd_ipmc_rp_remove_block_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO   *rp_info
  )
{
  uint32
    res = SOC_SAND_OK,
    nof_entries = 0;
  ARAD_PP_LEM_ACCESS_KEY            key;
  ARAD_PP_LEM_ACCESS_KEY            key_mask;
  SOC_PPC_FRWRD_IP_MC_RP_INFO   rp_key, rp_mask;
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        action;
  SOC_SAND_TABLE_BLOCK_RANGE        block_range;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rp_info);

  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

  SOC_PPC_FRWRD_IP_MC_RP_INFO_clear(&rp_key);
  SOC_PPC_FRWRD_IP_MC_RP_INFO_clear(&rp_mask);

  
  rp_key.rp_id = rp_info->rp_id;
  arad_pp_frwrd_ipmc_rp_lem_request_key_build(
      unit,
      &rp_key,
      &key
   );

  
  rp_mask.rp_id = 0xFFFF;
  arad_pp_frwrd_ipmc_rp_lem_request_key_build(
      unit,
      &rp_mask,
      &key_mask
   );
  
  key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
  key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST;

  
  action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

  res = arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&key,&key_mask,NULL,&block_range,&action, TRUE,&nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipmc_rp_remove_block_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipmc_rp_remove_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  if (rp_info->flags & SOC_PPC_FRWRD_IP_MC_BIDIR_IGNORE_RIF) {
      res = arad_pp_frwrd_ipmc_rp_remove_block_unsafe(unit,rp_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  else{
      res = arad_pp_frwrd_ipmc_rp_remove_one_unsafe(unit,rp_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipmc_rp_remove_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_vrf_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_VRF_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  res = arad_pp_frwrd_ipv4_def_action_set(
          unit,
          vrf_ndx,
          FALSE,
          &vrf_info->router_info.uc_default_action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_frwrd_ipv4_def_action_set(
          unit,
          vrf_ndx,
          TRUE,
          &vrf_info->router_info.mc_default_action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_info_set_unsafe()", vrf_ndx, 0);
}

uint32
  arad_pp_frwrd_ipv4_vrf_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_VRF_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(vrf_ndx, SOC_DPP_DEFS_GET(unit, nof_vrfs), SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_VRF_INFO, vrf_info, 20, exit);

  res = arad_pp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_info_set_verify()", vrf_ndx, 0);
}

uint32
  arad_pp_frwrd_ipv4_vrf_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_VRF_INFO_GET_VERIFY);

  
  SOC_SAND_ERR_IF_ABOVE_NOF(vrf_ndx, SOC_DPP_DEFS_GET(unit, nof_vrfs), SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);

  res = arad_pp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_info_get_verify()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ipv4_vrf_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_VRF_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  SOC_PPC_FRWRD_IPV4_VRF_INFO_clear(vrf_info);
  res = arad_pp_frwrd_ipv4_def_action_get(
          unit,
          vrf_ndx,
          FALSE,
          &vrf_info->router_info.uc_default_action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_frwrd_ipv4_def_action_get(
          unit,
          vrf_ndx,
          TRUE,
          &vrf_info->router_info.mc_default_action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_info_get_unsafe()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ipv4_vrf_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_IN  uint32                                      flags,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    pending_op;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_frwrd_ipv4_uc_or_vpn_route_add_unsafe(
          unit,
          vrf_ndx,
          &route_key->subnet,
          fec_id,
          pending_op,
          route_key->route_scale,
          flags,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_route_add_unsafe()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ipv4_vrf_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_IN  uint32                                      flags
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, ARAD_PP_VRF_ID_MIN, SOC_DPP_DEFS_GET(unit, nof_vrfs) - 1, SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY, route_key, 20, exit);
  if (!(flags & _BCM_L3_FLAGS2_RAW_ENTRY)) {
      SOC_SAND_ERR_IF_ABOVE_NOF(fec_id, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 30, exit);
  }
  else
  {
      SOC_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(fec_id, ARAD_PP_RAW_ID_MAX, 0, SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 40, exit);
  }

  res = soc_sand_pp_ipv4_subnet_verify(
          &route_key->subnet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = arad_pp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_route_add_verify()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ipv4_vrf_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 get_flags,
    SOC_SAND_IN  uint32                                      flags,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_ipv4_uc_or_vpn_route_get_unsafe(
          unit,
          vrf_ndx,
          &route_key->subnet,
          get_flags,
          route_key->route_scale,
          flags,
          fec_id,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_route_get_unsafe()", vrf_ndx, 0);
}

uint32
  arad_pp_frwrd_ipv4_vrf_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, ARAD_PP_VRF_ID_MIN, SOC_DPP_DEFS_GET(unit, nof_vrfs) - 1, SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY, route_key, 20, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
         &route_key->subnet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_route_get_verify()", vrf_ndx, 0);
}


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
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_PP_IPV4_SUBNET
    *subnets = NULL;
  uint32
    i = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(fec_ids);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    subnets = soc_sand_os_malloc(sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range_key->entries_to_act,"subnets1");
    if (subnets == NULL)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit);
    }
    soc_sand_os_memset(subnets, 0, sizeof(SOC_SAND_PP_IPV4_SUBNET) * block_range_key->entries_to_act);

    res = arad_pp_frwrd_ipv4_uc_or_vpn_route_get_block_unsafe(
            unit,
            vrf_ndx,
            block_range_key,
            subnets,
            fec_ids,
            routes_status,
            routes_location,
            nof_entries
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    for(i = 0; i < *nof_entries; i++)
    {
      SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&route_keys[i]);
      soc_sand_os_memcpy(&(route_keys[i].subnet), &(subnets[i]), sizeof(SOC_SAND_PP_IPV4_SUBNET));
    }

exit:
  soc_sand_os_free(subnets);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_route_get_block_unsafe()", vrf_ndx, 0);
}

uint32
  arad_pp_frwrd_ipv4_vrf_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, ARAD_PP_VRF_ID_MIN, SOC_DPP_DEFS_GET(unit, nof_vrfs) - 1, SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_IP_ROUTING_TABLE_RANGE, block_range_key, 20, exit);

  res = arad_pp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_route_get_block_verify()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ipv4_vrf_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    pending_op;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_frwrd_ipv4_uc_or_vpn_route_remove_unsafe(
          unit,
          vrf_ndx,
          &route_key->subnet,
          pending_op,
          route_key->route_scale,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_route_remove_unsafe()", vrf_ndx, 0);
}

uint32
  arad_pp_frwrd_ipv4_vrf_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(vrf_ndx, ARAD_PP_VRF_ID_MIN, SOC_DPP_DEFS_GET(unit, nof_vrfs) - 1, SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY, route_key, 20, exit);

  res = soc_sand_pp_ipv4_subnet_verify(
    &route_key->subnet
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_route_remove_verify()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ipv4_vrf_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_UNSAFE);

  res = arad_pp_frwrd_ipv4_uc_or_vpn_routing_table_clear(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_routing_table_clear_unsafe()", vrf_ndx, 0);
}

uint32
  arad_pp_frwrd_ipv4_vrf_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_VERIFY);

  res = arad_pp_frwrd_ipv4_vrf_is_supported_verify(
          unit,
          vrf_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_routing_table_clear_verify()", vrf_ndx, 0);
}


uint32
  arad_pp_frwrd_ipv4_vrf_all_routing_tables_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 flags
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE);

#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM) 
  {
      res = arad_pp_frwrd_ipv4_uc_rpf_kbp_table_clear(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (JER_KAPS_ENABLE(unit) && !(flags & SOC_PPC_FRWRD_IP_HOST_ONLY))
  {
      res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) {
          res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS);
          SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }
  else
#endif
  {
      if(!(flags & SOC_PPC_FRWRD_IP_HOST_ONLY)) {
          if (!SOC_IS_JERICHO(unit)) {
              res = arad_pp_frwrd_ipv4_lpm_all_vrfs_clear(unit);
              SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          }
      }
  }

  if(!(flags & SOC_PPC_FRWRD_IP_LPM_ONLY)) {
          res = arad_pp_frwrd_ipv4_host_vrf_clear_unsafe(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_all_routing_tables_clear_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ipv4_vrf_all_routing_tables_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 flags
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_vrf_all_routing_tables_clear_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_ipv4_mem_status_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MEM_STATUS      *mem_status
  )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  ARAD_PP_ARR_MEM_ALLOCATOR_MEM_STATUS      
    arad_mem_status;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_MEM_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mem_status);

  SOC_PPC_FRWRD_IPV4_MEM_STATUS_clear(mem_status);

  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_arr_mem_allocator_mem_status_get(&(lpm_mngr.init_info.mem_allocators[mem_ndx]),&arad_mem_status);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  mem_status->mem_stat.max_free_block_size = arad_mem_status.max_free_block_size;
  mem_status->mem_stat.nof_fragments = arad_mem_status.nof_fragments;
  mem_status->mem_stat.total_free = arad_mem_status.total_free;
  mem_status->mem_stat.total_size = arad_mem_status.total_size;

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mem_status_get_unsafe()", mem_ndx, 0);
}

uint32
  arad_pp_frwrd_ipv4_mem_status_get_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_MEM_STATUS_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(mem_ndx, ARAD_PP_FRWRD_IPV4_MEM_NDX_MIN, ARAD_PP_FRWRD_IPV4_MEM_NDX_MAX, ARAD_PP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mem_status_get_verify()", mem_ndx, 0);
}


uint32
  arad_pp_frwrd_ipv4_mem_defrage_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  ARAD_PP_ARR_MEM_ALLOCATOR_DEFRAG_INFO
    mem_defrag_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MEM_DEFRAGE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(defrag_info);

  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_PP_ARR_MEM_ALLOCATOR_DEFRAG_INFO_clear(&mem_defrag_info);
  ARAD_COPY(&mem_defrag_info.block_range,&defrag_info->range,sizeof(SOC_SAND_TABLE_BLOCK_RANGE),1);

  if (!lpm_mngr.init_info.mem_allocators[mem_ndx].support_defragment)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_DEFRAGE_NOT_SUPPORTED_ERR, 5, exit);
  }

  res = arad_pp_arr_mem_allocator_defrag(&(lpm_mngr.init_info.mem_allocators[mem_ndx]),&mem_defrag_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mem_defrage_unsafe()", mem_ndx, 0);
}

uint32
  arad_pp_frwrd_ipv4_mem_defrage_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MEM_DEFRAGE_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(mem_ndx, ARAD_PP_FRWRD_IPV4_MEM_NDX_MIN, ARAD_PP_FRWRD_IPV4_MEM_NDX_MAX, ARAD_PP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO, defrag_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_mem_defrage_verify()", mem_ndx, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_ipv4_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_frwrd_ipv4;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_ipv4_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_frwrd_ipv4;
}
uint32
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->route_val, ARAD_PP_FRWRD_IPV4_ROUTE_VAL_MAX, ARAD_PP_FRWRD_IPV4_ROUTE_VAL_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_ACTION_PROFILE, &(info->action_profile), 11, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_verify()",0,0);
}




soc_error_t
  arad_pp_frwrd_ip_ipmc_ssm_add(
     SOC_SAND_IN  int unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
     SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
     SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE* success
   )
{
    uint32 payload;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 priority;
    int table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    SOC_PPC_FRWRD_DECISION_INFO em_dest;
    ARAD_PP_LEM_ACCESS_PAYLOAD kbp_payload;
    ARAD_PP_LEM_ACCESS_KEY data, mask;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(route_key);
    SOCDNX_NULL_CHECK(route_info);
    SOCDNX_NULL_CHECK(success);

    priority = SOC_SAND_NOF_BITS_IN_UINT32 - route_key->group_prefix_len;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ipv4_mc_route_add_verify(unit,route_key,route_info));

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    DBAL_QUAL_VAL_ENCODE_FID(&qual_vals[0], route_key->fid);
    DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_DIP(&qual_vals[1], route_key->group, route_key->group_prefix_len);
    DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_SIP(&qual_vals[2], route_key->source.ip_address, route_key->source.prefix_len);

    if (route_key->with_vlan_lkup && SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
        DBAL_QUAL_VAL_ENCODE_VLAN_INNERMOST(&qual_vals[3], route_key->vlan_tag);
        table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID;
    }

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        if(ARAD_KBP_ENABLE_IPV4_MC_BRIDGE && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_ELK_LPM))
        {
             
             SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ipv4_mc_bridge_kbp_key_mask_encode(
                unit,
                ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE,
                route_key,
                &data,
                &mask
              ));

             
             SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ipv4_sand_dest_to_fwd_decision(
                            unit, &(route_info->dest_id), &em_dest));

             ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&kbp_payload);
             SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fwd_decision_in_buffer_build(
                            unit, ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
                            &em_dest, &kbp_payload.dest, &kbp_payload.asd));
             SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals, priority,  &kbp_payload, success));
        }
        else
#endif
        {
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ipv4_sand_dest_to_em_dest(unit, &(route_info->dest_id), &payload));
            
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals, priority,  &payload, success));
        }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_pp_frwrd_ip_ipmc_ssm_delete(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key)
{
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE success;
    int table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(route_key);

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    DBAL_QUAL_VAL_ENCODE_FID(&qual_vals[0], route_key->fid);
    DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_DIP(&qual_vals[1], route_key->group, route_key->group_prefix_len);
    DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_SIP(&qual_vals[2], route_key->source.ip_address, route_key->source.prefix_len);

    if (route_key->with_vlan_lkup && SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
        DBAL_QUAL_VAL_ENCODE_VLAN_INNERMOST(&qual_vals[3], route_key->vlan_tag);
        table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID;
    }

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(JER_KAPS_ENABLE(unit) &&  (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_KAPS_LPM)){
        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_db_enabled(unit, 0 ));
    }
#endif

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));
    

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_pp_frwrd_ip_ipmc_ssm_get(
     SOC_SAND_IN  int unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
     SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
     SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS*    route_status,
     SOC_SAND_OUT uint8* found)
{
    uint32 payload, priority;
    uint8 hit_bit = 0;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID;
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    ARAD_PP_LEM_ACCESS_PAYLOAD kbp_payload;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    DBAL_QUAL_VAL_ENCODE_FID(&qual_vals[0], route_key->fid);
    DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_DIP(&qual_vals[1], route_key->group, route_key->group_prefix_len);
    DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_SIP(&qual_vals[2], route_key->source.ip_address, route_key->source.prefix_len);

    if (route_key->with_vlan_lkup && SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
        DBAL_QUAL_VAL_ENCODE_VLAN_INNERMOST(&qual_vals[3], route_key->vlan_tag);
        table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID;
    }

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(JER_KAPS_ENABLE(unit) && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_KAPS_LPM)){
        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_db_enabled(unit, 0));        
    }
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV4_MC_BRIDGE && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_ELK_LPM))
    {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, &kbp_payload, &priority, &hit_bit, found));
        if (*found) {
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ipv4_em_dest_to_sand_dest(unit, kbp_payload.dest, &(route_info->dest_id)));
        }
    }
    else
#endif
    {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, &payload, &priority, &hit_bit, found));

        if (*found) {
            
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ipv4_em_dest_to_sand_dest(unit, payload, &(route_info->dest_id)));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_frwrd_ip_ipmc_ssm_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(routes_info);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(routes_location);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_IPV4_MC_BRIDGE
     && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_ELK_LPM))
  {
      res = arad_pp_frwrd_ipv4_kbp_route_get_block_unsafe(
              unit,
              ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE,
              FALSE, 
              0, 
              block_range_key,
              NULL,
              NULL,
              route_keys,
              NULL,
              routes_info,
              routes_status,
              routes_location,
              nof_entries
            );
  }
  else
#endif
  {
      res = arad_pp_frwrd_ipv4_dbal_route_get_block(
              unit,
              SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID,
              -1,
              block_range_key,
              NULL,
              route_keys,
              NULL,
              routes_info,
              routes_status,
              routes_location,
              nof_entries
             );
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_ipmc_ssm_get_block_unsafe()", 0, 0);
}

soc_error_t
  arad_pp_frwrd_ip_ipmc_ssm_clear(
    SOC_SAND_IN  int                                 unit
  )
{
  int table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID;

  SOCDNX_INIT_FUNC_DEFS;

  if (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
      table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID;
  }

#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_IPV4_MC_BRIDGE
     && (SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode == BCM_IPMC_SSM_ELK_LPM))
    {
        SOCDNX_IF_ERR_EXIT(arad_pp_frwrd_ipv4_mc_bridge_kbp_table_clear(unit));
    }
    else
#endif
    {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_clear(unit, table_id));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_FRWRD_IPV4_TYPE_MAX, ARAD_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL, &(info->value), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_ROUTER_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION, &(info->uc_default_action), 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION, &(info->mc_default_action), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_ROUTER_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_VRF_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VRF_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_ROUTER_INFO, &(info->router_info), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_VRF_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_GLBL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_ROUTER_INFO, &(info->router_info), 10, exit);
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  if(info->group != 0) {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->group, ARAD_PP_FRWRD_IPV4_GROUP_MIN, ARAD_PP_FRWRD_IPV4_GROUP_MAX, ARAD_PP_FRWRD_IPV4_GROUP_OUT_OF_RANGE_ERR, 10, exit);
  }
  
  SOC_SAND_ERR_IF_ABOVE_NOF(info->inrif, SOC_DPP_CONFIG(unit)->l3.nof_rifs, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_HOST_KEY_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_NOF(info->vrf_ndx, SOC_DPP_DEFS_GET(unit, nof_vrfs), SOC_PPC_VRF_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_HOST_KEY_verify()",info->vrf_ndx,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_NOF(info->fec_id, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->dest_id.dest_type != SOC_SAND_PP_DEST_MULTICAST && info->dest_id.dest_type != SOC_SAND_PP_DEST_FEC)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,10,exit);
  }
  if (info->dest_id.dest_type == SOC_SAND_PP_DEST_FEC)
  {
    SOC_SAND_ERR_IF_ABOVE_NOF(info->dest_id.dest_val, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (info->dest_id.dest_type == SOC_SAND_PP_DEST_MULTICAST)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_id.dest_val, ARAD_MAX_MC_ID(unit),ARAD_MULT_MC_ID_OUT_OF_RANGE_ERR,30,exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_MC_RAW_ROUTE_INFO_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  SOC_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(info->dest_id.dest_val, ARAD_PP_RAW_ID_MAX, 0, ARAD_MULT_MC_RAW_ID_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_MC_RAW_ROUTE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
 
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


