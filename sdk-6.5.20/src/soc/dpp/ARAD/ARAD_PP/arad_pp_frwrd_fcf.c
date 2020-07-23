
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD




#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <soc/dpp/ARAD/arad_sim_em.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fcf.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_lpm_mngr.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vtt.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/drv.h>





#define ARAD_PP_FRWRD_FCF_VFT_MAX                                ((1 << ARAD_PP_FRWRD_FCF_VFI_BITS) - 1)
#define ARAD_PP_FRWRD_FCF_D_ID_MAX                                ((1 << ARAD_PP_FRWRD_FCF_D_ID_BITS) - 1)
#define ARAD_PP_FRWRD_FCF_S_ID_MAX                                ((1 << ARAD_PP_FRWRD_FCF_S_ID_BITS) - 1)




#define ARAD_PP_FRWRD_IP_FC_VRF                          ((SOC_DPP_CONFIG(unit))->pp.fcoe_reserved_vrf)


#define ARAD_PP_FRWRD_FC_OP_ADD    (1)
#define ARAD_PP_FRWRD_FC_OP_REMOVE (2)
#define ARAD_PP_FRWRD_FC_OP_GET    (3)


#define ARAD_PP_FRWRD_FC_SEM_RESULT_INDX (0xae2)




#define ARAD_PP_FRWRD_FC_VFT_VAL_TO_KEY_FIX(_vfi)  ((_vfi)<<1); 

#define ARAD_PP_FRWRD_FC_VFT_VAL_FROM_KEY_FIX(_vfi)  ((_vfi)>>1); 













CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_frwrd_fcf[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FCF_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FCF_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FCF_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FCF_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FCF_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FCF_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FCF_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FCF_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_REMOVE_VERIFY),

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_ZONING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_GET_ERRS_PTR),
  

   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_DEF_ACTION_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_L3_VPN_DEFAULT_ROUTING_ENABLE_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_L3_VPN_DEFAULT_ROUTING_ENABLE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_LPM_ROUTE_ADD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_LPM_ROUTE_IS_EXIST),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_LPM_ROUTE_CLEAR),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_LPM_ROUTE_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_LPM_ROUTE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_LPM_ROUTE_GET_BLOCK),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_LEM_ROUTE_ADD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_LEM_ROUTE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_LEM_ROUTE_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_OR_VPN_ROUTE_ADD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_OR_VPN_ROUTE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_OR_VPN_ROUTE_GET_BLOCK),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_OR_VPN_ROUTE_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FCF_OR_VPN_ROUTING_TABLE_CLEAR),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_frwrd_fcf[] =
{
  
  {
    ARAD_PP_FRWRD_FCF_ROUTE_TYPES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_ROUTE_TYPES_OUT_OF_RANGE_ERR",
    "The parameter 'route_types' is out of range. \n\r "
    "The range is: NONE or LPM .\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FRWRD_FCF_ROUTE_STATUS_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_FCF_ROUTE_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'route_status' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_FCF_ROUTE_STATUSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_FOUND_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_FOUND_OUT_OF_RANGE_ERR",
    "The parameter 'found' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_FRWRD_FCF_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_ROUTES_STATUS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_ROUTES_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'routes_status' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_FCF_ROUTE_STATUSS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_FRWRD_FCF_ROUTE_LOCATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_EXACT_MATCH_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_EXACT_MATCH_OUT_OF_RANGE_ERR",
    "The parameter 'exact_match' is out of range. \n\r "
    "The range is: 1 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_MEM_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_MEM_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mem_ndx' is out of range. \n\r "
    "The range is: 0 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_ROUTE_VAL_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_ROUTE_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'route_val' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_FRWRD_FCF_ROUTER_DEFAULT_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_TABLE_RESOUCES_OUT_OF_RANGE_ERR",
    "The parameter 'uc_table_resouces' is out of range. \n\r "
    "for Arad-B it has to be LPM only, to add to host use host APIs.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR",
    "The parameter 'mc_table_resouces' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_FRWRD_FCF_MC_HOST_TABLE_RESOURCES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_CACHE_MODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_CACHE_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'mc_table_resouces' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_FRWRD_FCF_CACHE_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_GROUP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_GROUP_OUT_OF_RANGE_ERR",
    "The parameter 'group' is out of range. \n\r "
    "The range is: 224.0.0.0 - 239.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
   {
    ARAD_PP_FRWRD_FCF_EEP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_MC_ILLEGAL_DEST_TYPE_ERR,
    "ARAD_PP_FRWRD_FCF_MC_ILLEGAL_DEST_TYPE_ERR",
    "Destination in fcf MC routing info can be \n\r"
    "FEC-ptr or MC-group only\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_MUST_ALL_VFI_ERR,
    "ARAD_PP_FRWRD_FCF_MUST_ALL_VFI_ERR",
    "operation can be done for all-VFIs and not one VFI \n\r"
    "set vfi_ndx = ARAD_PP_FRWRD_FCF_ALL_VFIS_ID\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_CACHE_NOT_SUPPORTED_ERR,
    "ARAD_PP_FRWRD_FCF_CACHE_NOT_SUPPORTED_ERR",
    "caching is not supported (supplied in operation mode)\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_DEFRAGE_NOT_SUPPORTED_ERR,
    "ARAD_PP_FRWRD_FCF_DEFRAGE_NOT_SUPPORTED_ERR",
    "defragment is not supported (supplied in operation mode)\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_OPERATION_NOT_SUPPORTED_ON_VFI_ERR,
    "ARAD_PP_FRWRD_FCF_OPERATION_NOT_SUPPORTED_ON_VFI_ERR",
    "operation is not supported on single VFI, use ARAD_PP_FRWRD_FCF_ALL_VFIS_ID \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_IP_VRF_NOT_MATCH_RESERVED_ERR,
    "ARAD_PP_FRWRD_FCF_IP_VRF_NOT_MATCH_RESERVED_ERR",
    "the VRF used for the FCF route entry is not the resereved VRF \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_FRWRD_D_ID_PREFIX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_FRWRD_D_ID_PREFIX_OUT_OF_RANGE_ERR",
    "for Forward entries added to LEM, D_ID prefix has to be 24 or 8 \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_FRWRD_INTERNAL_IMP_ERR,
    "ARAD_PP_FRWRD_FCF_FRWRD_INTERNAL_IMP_ERR",
    "internal implementation error, due to calling internal function with unexpected parameters \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_PREFIX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_PREFIX_OUT_OF_RANGE_ERR",
    "route prefix range is 0-8 \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_LPM_DEST_ILLEGAL_ERR,
    "ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_LPM_DEST_ILLEGAL_ERR",
    "LPM destination has to be FEC \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  



  
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};






uint32
  arad_pp_frwrd_fcf_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
	return 0;
}





 uint32
  arad_pp_fcf_route_key_to_ip_key(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY    *fcf_route_key,
    SOC_SAND_OUT  uint32                        *ip_vrf_id,
    SOC_SAND_OUT  SOC_SAND_PP_IPV4_SUBNET       *ip_route_key
  )
{

    uint32
        tmp;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (ARAD_LPM_CUSTOM_LOOKUP_ENABLED(unit)) { 
            *ip_vrf_id = fcf_route_key->vfi; 
            ip_route_key->ip_address = fcf_route_key->d_id; 
            ip_route_key->prefix_len = fcf_route_key->prefix_len;
            SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_route_key_to_ip_key()",fcf_route_key->d_id, fcf_route_key->prefix_len);
        }

        
    *ip_vrf_id = (ARAD_PP_FRWRD_IP_FC_VRF);    

    if (fcf_route_key->flags == SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN_NPORT) {
        *ip_vrf_id = (ARAD_PP_FRWRD_IP_FC_VRF+1);
    }else {
        *ip_vrf_id = (ARAD_PP_FRWRD_IP_FC_VRF);
    }

    
    
    
    
    
    

    
    
    ip_route_key->ip_address = fcf_route_key->vfi << ARAD_PP_FRWRD_FCF_KEY_VFI_LSB;

    

    
    tmp = (fcf_route_key->d_id >> (ARAD_PP_FRWRD_FCF_D_ID_ROUTE_LSB)) & ARADP_PP_FRWRD_FCF_MASK_BITS(ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS);

    
    ip_route_key->ip_address |= tmp << ARAD_PP_FRWRD_FCF_KEY_D_ID_LSB;

    
    ip_route_key->prefix_len = ARAD_PP_FRWRD_FCF_VFI_BITS + fcf_route_key->prefix_len;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_route_key_to_ip_key()",fcf_route_key->vfi,fcf_route_key->d_id);
}


 uint32
  arad_pp_fcf_ip_key_to_fc_key(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_OUT  SOC_PPC_FRWRD_FCF_ROUTE_KEY   *fcf_route_key,
    SOC_SAND_IN  uint32                         ip_vrf_id,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET       *ip_route_key
  )
{
    uint32
        tmp;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (ip_vrf_id != ARAD_PP_FRWRD_IP_FC_VRF) {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_IP_VRF_NOT_MATCH_RESERVED_ERR,10,exit);
    }

    
    
    
    
    
    

    
    
    fcf_route_key->vfi  = (ip_route_key->ip_address >> ARAD_PP_FRWRD_FCF_KEY_VFI_LSB) & ARADP_PP_FRWRD_FCF_MASK_BITS(ARAD_PP_FRWRD_FCF_VFI_BITS);

    

    

    tmp = (ip_route_key->ip_address >> ARAD_PP_FRWRD_FCF_KEY_D_ID_LSB) & ARADP_PP_FRWRD_FCF_MASK_BITS(ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS);

    
    fcf_route_key->d_id = tmp << ARAD_PP_FRWRD_FCF_D_ID_ROUTE_LSB;

    
    fcf_route_key->prefix_len = ip_route_key->prefix_len - ARAD_PP_FRWRD_FCF_VFI_BITS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_ip_key_to_fc_key()",ip_vrf_id,ip_route_key->ip_address);
}


 uint32
  arad_pp_fcf_key_to_sem_key(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY    *fcf_route_key,
    SOC_SAND_OUT  ARAD_PP_ISEM_ACCESS_KEY       *isem_key
  )
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_FCOE;
    isem_key->key_info.fcoe.d_id = fcf_route_key->d_id;
    isem_key->key_info.fcoe.vsan = fcf_route_key->vfi;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_key_to_sem_key()",fcf_route_key->vfi,fcf_route_key->d_id);
}



 uint32
  arad_pp_fcf_key_from_sem_key(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT  SOC_PPC_FRWRD_FCF_ROUTE_KEY  *fcf_route_key,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY       *isem_key
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    fcf_route_key->d_id = isem_key->key_info.fcoe.d_id;
    fcf_route_key->prefix_len = ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_key_to_sem_key()",fcf_route_key->vfi,fcf_route_key->d_id);
}




 uint32
  arad_pp_fcf_key_sem_op(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY    *fcf_route_key,
    SOC_SAND_IN  uint8                          db_op, 
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success,
    SOC_SAND_OUT uint8                          *found
  )
{
    uint32 
      res;
    ARAD_PP_ISEM_ACCESS_KEY
        isem_key;
    ARAD_PP_ISEM_ACCESS_ENTRY                            
        isem_entry;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pp_fcf_key_to_sem_key(unit,fcf_route_key,&isem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    isem_entry.sem_result_ndx = ARAD_PP_FRWRD_FC_SEM_RESULT_INDX; 

    
    if (db_op == ARAD_PP_FRWRD_FC_OP_ADD) { 
        res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else if (db_op == ARAD_PP_FRWRD_FC_OP_GET) { 
        res = arad_pp_isem_access_entry_get_unsafe(unit, &isem_key, &isem_entry, found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        if(success){
            *success = SOC_SAND_SUCCESS;
        }
    }
    else if (db_op == ARAD_PP_FRWRD_FC_OP_REMOVE) { 
        res = arad_pp_isem_access_entry_remove_unsafe(unit, &isem_key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        if(success){
            *success = SOC_SAND_SUCCESS;
        }
    }
    else{
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_INTERNAL_IMP_ERR,120,exit);
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_key_sem_add()",fcf_route_key->vfi,fcf_route_key->d_id);
}


 uint32
  arad_pp_fcf_route_key_to_lem_key(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY    *fcf_route_key,
     SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY         *lem_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_KEY_clear(lem_key);

  

  
  if(fcf_route_key->prefix_len == ARAD_PP_FRWRD_FCF_D_ID_BITS){
      lem_key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_FC_LOCAL;
      lem_key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_FC_LOCAL;

      
      lem_key->param[0].nof_bits = ARAD_PP_FRWRD_FCF_D_ID_BITS;
      lem_key->param[0].value[0] = fcf_route_key->d_id;

      
      lem_key->param[1].nof_bits = ARAD_PP_FRWRD_FCF_VFI_BITS;
      lem_key->param[1].value[0] = ARAD_PP_FRWRD_FC_VFT_VAL_TO_KEY_FIX(fcf_route_key->vfi); 

      lem_key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
      lem_key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_LOCAL;
  }
  
  else if(fcf_route_key->prefix_len == ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS){
      lem_key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_FC_REMOTE;
      lem_key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_FC_REMOTE;

      lem_key->param[0].nof_bits = ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS;
      lem_key->param[0].value[0] = ARAD_PP_FRWRD_FCF_GET_DOMAIN(fcf_route_key->d_id);

      lem_key->param[1].nof_bits = ARAD_PP_FRWRD_FCF_VFI_BITS;
      lem_key->param[1].value[0] = ARAD_PP_FRWRD_FC_VFT_VAL_TO_KEY_FIX(fcf_route_key->vfi); 

      lem_key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
      lem_key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_REMOTE;
  }
  else{
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_D_ID_PREFIX_OUT_OF_RANGE_ERR,10,exit);
  }

  if (fcf_route_key->flags == SOC_PPC_FRWRD_FCF_ROUTE_HOST_NPORT) {      
      lem_key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_FC_LOCAL_N_PORT;
      lem_key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_LOCAL_N_PORT;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_route_key_to_lem_key()",fcf_route_key->d_id,fcf_route_key->prefix_len);
}



  void
    arad_pp_fcf_route_key_from_lem_key(
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_KEY *route_key
    )
{
    if (key->type == ARAD_PP_LEM_ACCESS_KEY_TYPE_FC_LOCAL) {
        route_key->vfi = ARAD_PP_FRWRD_FC_VFT_VAL_FROM_KEY_FIX(key->param[1].value[0]);
        route_key->prefix_len = ARAD_PP_FRWRD_FCF_D_ID_BITS;
        route_key->d_id = key->param[0].value[0];
    }
    else{ 
        route_key->vfi = ARAD_PP_FRWRD_FC_VFT_VAL_FROM_KEY_FIX(key->param[1].value[0]);
        route_key->prefix_len = ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS;
        ARAD_PP_FRWRD_FCF_SET_DOMAIN(route_key->d_id, key->param[1].value[0]);
    }
}


 uint32
  arad_pp_fcf_frwrd_decision_to_lem_payload(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO    *frwrd_info,
     SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD     *payload
  )
{
  uint32 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);

  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          frwrd_info,
          &payload->dest,
          &payload->asd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if(frwrd_info->additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE) {
      payload->flags = ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
  }
  else if(frwrd_info->additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
      payload->flags = ARAD_PP_FWD_DECISION_PARSE_EEI;
  }
  else{
      payload->flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_frwrd_decision_to_lem_payload()",0,0);
}

 uint32
  arad_pp_fcf_frwrd_decision_from_lem_payload(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD     *payload,
     SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO    *frwrd_info
  )
{
  uint32
    flags = ARAD_PP_FWD_DECISION_PARSE_LEGACY;
  uint32 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_DECISION_INFO_clear(frwrd_info);

  
  if(payload->flags & ARAD_PP_FWD_DECISION_PARSE_EEI)
  {
    flags |= ARAD_PP_FWD_DECISION_PARSE_EEI;
  }
  if(payload->flags & ARAD_PP_FWD_DECISION_PARSE_OUTLIF)
  {
    flags |= ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
  }

  res = arad_pp_fwd_decision_in_buffer_parse(
          unit,          
          payload->dest,
          payload->asd,
          flags,
          frwrd_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_frwrd_decision_from_lem_payload()",0,0);
}




 uint32
  arad_pp_fcf_route_info_to_lem_payload(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO   *fcf_route_info,
     SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD     *payload
  )
{
  uint32 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);

  res = arad_pp_fcf_frwrd_decision_to_lem_payload(
          unit,
          &(fcf_route_info->frwrd_decision),
          payload
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_route_info_to_lem_payload()",0,0);
}




 uint32
  arad_pp_fcf_route_info_from_lem_payload(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD     *payload,
     SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_INFO   *fcf_route_info,
     SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS *route_status
  )
{
  uint32 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_FCF_ROUTE_INFO_clear(fcf_route_info);

  
  res = arad_pp_fcf_frwrd_decision_from_lem_payload(
          unit,
          payload,
          &(fcf_route_info->frwrd_decision)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  *route_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
  if (payload->flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED) {
    *route_status |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_route_info_from_lem_payload()",0,0);
}



 uint32
  arad_pp_fcf_zoning_key_to_lem_key(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY   *fcf_zone_key,
     SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY         *lem_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_KEY_clear(lem_key);

  
  lem_key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_FC_ZONING;
  lem_key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_FC_ZONING;

  lem_key->param[0].nof_bits = ARAD_PP_FRWRD_FCF_S_ID_BITS;
  lem_key->param[0].value[0] = fcf_zone_key->d_id;

  lem_key->param[1].nof_bits = ARAD_PP_FRWRD_FCF_D_ID_BITS;
  lem_key->param[1].value[0] = fcf_zone_key->s_id;

  lem_key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
  lem_key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_ZONING;

  lem_key->param[2].nof_bits = ARAD_PP_FRWRD_FCF_VFI_BITS;
  lem_key->param[2].value[0] = ARAD_PP_FRWRD_FC_VFT_VAL_TO_KEY_FIX(fcf_zone_key->vfi);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_zoning_key_to_lem_key()",fcf_zone_key->d_id,fcf_zone_key->s_id);
}



  void
    arad_pp_fcf_zone_key_from_lem_key(
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_KEY *zone_key
    )
{
    zone_key->s_id = key->param[0].value[0];
    zone_key->d_id = key->param[1].value[0];
    zone_key->vfi = ARAD_PP_FRWRD_FC_VFT_VAL_FROM_KEY_FIX(key->param[2].value[0]);
}




 uint32
  arad_pp_fcf_zone_info_to_lem_payload(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO  *fcf_zone_info,
     SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD     *payload
  )
{
  uint32 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);

  res = arad_pp_fcf_frwrd_decision_to_lem_payload(
          unit,
          &(fcf_zone_info->frwrd_decision),
          payload
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_zone_info_to_lem_payload()",0,0);
}


 uint32
  arad_pp_fcf_zone_info_from_lem_payload(
     SOC_SAND_IN  int                         unit,
     SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD     *payload,
     SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_INFO  *fcf_zone_info,
     SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS *route_status
  )
{
  uint32 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_FCF_ZONING_INFO_clear(fcf_zone_info);

  
  res = arad_pp_fcf_frwrd_decision_from_lem_payload(
          unit,
          payload,
          &(fcf_zone_info->frwrd_decision)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  *route_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
  if (payload->flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED) {
    *route_status |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_route_info_from_lem_payload()",0,0);
}



 uint32
  arad_pp_fcf_key_lem_op(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY       *fcf_route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY      *fcf_zone_key,
    SOC_SAND_IN  uint8                              db_op,
    SOC_SAND_INOUT  SOC_PPC_FRWRD_FCF_ROUTE_INFO   *fcf_route_info,
    SOC_SAND_INOUT  SOC_PPC_FRWRD_FCF_ZONING_INFO  *fcf_zone_info,
    SOC_SAND_OUT uint8                          *found,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS *route_status
  )
{
    uint32 
      res;
    ARAD_PP_LEM_ACCESS_REQUEST
      request;
    ARAD_PP_LEM_ACCESS_PAYLOAD
      lem_payload;
    ARAD_PP_LEM_ACCESS_ACK_STATUS
      ack;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&lem_payload);
    ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);


    
    if (fcf_route_key) {
        res = arad_pp_fcf_route_key_to_lem_key(unit,fcf_route_key,&request.key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if (fcf_zone_key) {
        res = arad_pp_fcf_zoning_key_to_lem_key(unit,fcf_zone_key,&request.key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else{
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_INTERNAL_IMP_ERR,30,exit);
    }

    
    if (db_op == ARAD_PP_FRWRD_FC_OP_ADD) {
        if (fcf_route_info) {
            res = arad_pp_fcf_route_info_to_lem_payload(unit,fcf_route_info,&lem_payload);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        }
        else if (fcf_zone_key) {
            res = arad_pp_fcf_zone_info_to_lem_payload(unit,fcf_zone_info,&lem_payload);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        }
        else{
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_INTERNAL_IMP_ERR,60,exit);
        }
    }

    
    if (db_op == ARAD_PP_FRWRD_FC_OP_ADD) { 
        request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
        res = arad_pp_lem_access_entry_add_unsafe(
                unit,
                &request,
                &lem_payload,
                &ack);
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
        if (success) {
            *success = (ack.is_success)?SOC_SAND_SUCCESS:SOC_SAND_FAILURE_OUT_OF_RESOURCES;
        }
    }
    else if (db_op == ARAD_PP_FRWRD_FC_OP_REMOVE) {
        request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
        res = arad_pp_lem_access_entry_remove_unsafe(
                unit,
                &request,
                &ack);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        if (success) {
            *success = (ack.is_success)?SOC_SAND_SUCCESS:SOC_SAND_FAILURE_OUT_OF_RESOURCES;
        }
    }
    else if (db_op == ARAD_PP_FRWRD_FC_OP_GET) {
        res = arad_pp_lem_access_entry_by_key_get_unsafe(
                unit,
                &(request.key),
                &lem_payload,
                found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        if (*found)
        {
            
            if (fcf_route_key && fcf_route_info && route_status) {
                res = arad_pp_fcf_route_info_from_lem_payload(unit,&lem_payload,fcf_route_info,route_status);
                SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
            }
            else if (fcf_zone_key && fcf_zone_info && route_status) {
                res = arad_pp_fcf_zone_info_from_lem_payload(unit,&lem_payload,fcf_zone_info,route_status);
                SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
            }
            else{
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_INTERNAL_IMP_ERR,120,exit);
            }
        }
    }
    else{
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_INTERNAL_IMP_ERR,130,exit);
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_fcf_key_lem_op()",db_op,0);
}


  uint32
    arad_pp_frwrd_fcf_lpm_route_add(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY                *fcf_route_key,
      SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO               *fcf_route_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                   *success
    )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint32                        
      ip_vrf_id;
  SOC_SAND_PP_IPV4_SUBNET       
      ip_route_key;
  uint32
    res;
  uint8
    success_bool;
  uint8                                      
      is_pending_op;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_LPM_ROUTE_ADD);  
  
  if ((SOC_DPP_CONFIG(unit))->pp.fcoe_enable) {
    
    if (fcf_route_key->prefix_len >= ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS) {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_PREFIX_OUT_OF_RANGE_ERR,10,exit);
    }

    
    if (fcf_route_info->frwrd_decision.type != SOC_PPC_FRWRD_DECISION_TYPE_FEC) {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_LPM_DEST_ILLEGAL_ERR,20,exit);
    }
  }

  
  res = arad_pp_fcf_route_key_to_ip_key(
            unit,
            fcf_route_key,
            &ip_vrf_id,
            &ip_route_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


  
  res = arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
          unit,
          SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          &is_pending_op
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


  res = arad_pp_ipv4_lpm_mngr_prefix_add(
          &lpm_mngr,
          ip_vrf_id,
          &ip_route_key,
          fcf_route_info->frwrd_decision.dest_id,
          is_pending_op,
          SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM,
          &success_bool
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  *success = (success_bool == TRUE) ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

  if (*success == SOC_SAND_SUCCESS)
  {
    res = arad_pp_sw_db_ipv4_cache_vrf_modified_set(unit,ip_vrf_id,TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_fcf_lpm_route_add()",0,0);
}


  uint32
    arad_pp_frwrd_fcf_lpm_vfi_clear(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                  vfi_ndx
    )
{
	return 0;
}



  uint32
    arad_pp_frwrd_fcf_lpm_all_vfis_clear(
      SOC_SAND_IN  int                                     unit
    )
{
	return 0;
}


  uint32
    arad_pp_frwrd_fcf_lpm_route_remove(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY               *fcf_route_key,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                   *success
    )
{
    ARAD_PP_IPV4_LPM_MNGR_INFO
       lpm_mngr;
     uint32                        
         ip_vrf_id;
     SOC_SAND_PP_IPV4_SUBNET       
         ip_route_key;
     uint32
       res;
     uint8                                      
         success_bool,
         is_pending_op;

     SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_LPM_ROUTE_REMOVE);

     SOC_SAND_CHECK_NULL_INPUT(fcf_route_key);
     SOC_SAND_CHECK_NULL_INPUT(success);

     if ((SOC_DPP_CONFIG(unit))->pp.fcoe_enable) {
         
         if (fcf_route_key->prefix_len >= ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS) {
             SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_PREFIX_OUT_OF_RANGE_ERR,10,exit);
         }
     }

     
     res = arad_pp_fcf_route_key_to_ip_key(
               unit,
               fcf_route_key,
               &ip_vrf_id,
               &ip_route_key);
     SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

     res = arad_pp_sw_db_ipv4_lpm_mngr_get(
             unit,
             &lpm_mngr
           );
     SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

     
     res = arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(
             unit,
             SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
             &is_pending_op
           );
     SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


     arad_pp_ipv4_lpm_mngr_prefix_remove(
             &lpm_mngr,
             ip_vrf_id,
             &ip_route_key,
             is_pending_op,
             &success_bool
           );

     *success = (success_bool == TRUE) ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

     if (*success == SOC_SAND_SUCCESS)
     {
         res = arad_pp_sw_db_ipv4_cache_vrf_modified_set(unit,ip_vrf_id,TRUE);
         SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
     }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_fcf_lpm_route_remove()",0,0);
}



  uint32
    arad_pp_frwrd_fcf_lpm_route_get(
      SOC_SAND_IN  int                                    unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY               *fcf_route_key,
      SOC_SAND_IN  uint8                                     exact_match,
      SOC_SAND_OUT  SOC_PPC_FRWRD_FCF_ROUTE_INFO              *fcf_route_info,
      SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS            *pending_type,
      SOC_SAND_OUT uint8                                     *found
    )
{
 ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  uint32                        
      ip_vrf_id;
  SOC_SAND_PP_IPV4_SUBNET       
      ip_route_key;
  uint32
    res;
  SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION 
      hw_target_type;
  SOC_SAND_PP_SYSTEM_FEC_ID
      fec_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_LPM_ROUTE_GET);

  SOC_SAND_CHECK_NULL_INPUT(fcf_route_key);
  SOC_SAND_CHECK_NULL_INPUT(fcf_route_info);
  SOC_SAND_CHECK_NULL_INPUT(pending_type);
  SOC_SAND_CHECK_NULL_INPUT(found);
  
  
  if (fcf_route_key->prefix_len >= ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_PREFIX_OUT_OF_RANGE_ERR,10,exit);
  }

  
  res = arad_pp_fcf_route_key_to_ip_key(
            unit,
            fcf_route_key,
            &ip_vrf_id,
            &ip_route_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          unit,
          &lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_pp_ipv4_lpm_mngr_sys_fec_get(
          &lpm_mngr,
          ip_vrf_id,
          &ip_route_key,
          exact_match,
          &fec_id,
          (SOC_PPC_FRWRD_IP_ROUTE_STATUS*)pending_type,
          &hw_target_type,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


  fcf_route_info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
  fcf_route_info->frwrd_decision.dest_id = fec_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_fcf_lpm_route_get()",0,0);
}


  uint32
    arad_pp_frwrd_fcf_lpm_route_get_block(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vfi_ndx,
      SOC_SAND_INOUT  SOC_PPC_IP_ROUTING_TABLE_RANGE               *block_range,
      SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_OUT uint32                                      *route_info,
      SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS                   *pending_type,
      SOC_SAND_OUT uint32                                      *nof_entries
    )
{
	return 0;
}





  uint32
    arad_pp_frwrd_fcf_lem_route_add(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vfi_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
      SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO              *routing_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
    )
{
	return 0;

}



  uint32
    arad_pp_frwrd_fcf_lem_route_get(
      SOC_SAND_IN  int                                    unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY               *zoning_key,
      SOC_SAND_OUT  SOC_PPC_FRWRD_FCF_ZONING_INFO       *routing_info,
      SOC_SAND_OUT uint8                                     *found,
      SOC_SAND_OUT uint8                                     *accessed
    )
{
	return 0;

}


  uint32
    arad_pp_frwrd_fcf_lem_route_remove_unsafe(
      SOC_SAND_IN  int                                     unit,
      SOC_SAND_IN  uint32                                      vfi_ndx,
      SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key
    )
{
	return 0;

}

 uint32
  arad_pp_frwrd_fcf_or_vpn_route_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      vfi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_IN  uint32                    route_info,
    SOC_SAND_IN  uint8                                     is_pending_op,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
	return 0;
}


 uint32
  arad_pp_frwrd_fcf_or_vpn_route_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      vfi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_IN  uint8                                     exact_match,
    SOC_SAND_OUT uint32                                      *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS         *pending_type,
    SOC_SAND_OUT uint8                                     *found
  )
{
	return 0;
}


 uint32
  arad_pp_frwrd_fcf_or_vpn_route_get_block_unsafe(
  SOC_SAND_IN  int                                     unit,
  SOC_SAND_IN  uint32                                      vfi_ndx,
    SOC_SAND_INOUT  SOC_PPC_IP_ROUTING_TABLE_RANGE                  *block_range,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_OUT uint32                                      *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS         *pending_type,
    SOC_SAND_OUT uint32                                      *nof_entries
    )
{
	return 0;
}


 uint32
  arad_pp_frwrd_fcf_or_vpn_route_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      vfi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                           *route_key,
    SOC_SAND_IN  uint8                                     is_pending_op,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
	return 0;
}




  uint32
    arad_pp_frwrd_fcf_lpm_routing_table_clear(
      SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags
  )
{
    return arad_pp_frwrd_ipv4_vrf_routing_table_clear_unsafe(unit,ARAD_PP_FRWRD_IP_FC_VRF);
}


 uint32
  arad_pp_frwrd_fcf_vfi_modified_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                              vfi_ndx,
    SOC_SAND_OUT  uint8                                 *modified
  )
{
	return 0;
}


 uint32
  arad_pp_frwrd_fcf_vfi_independent_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                              vfi_ndx
  )
{
	return 0;
}


 uint32
  arad_pp_frwrd_fcf_vfi_is_supported_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                              vfi_ndx,
    SOC_SAND_IN  uint8                                 all_permitted
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_routes_cache_commit_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                              vfi_ndx
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_GLBL_INFO            *glbl_info
  )
{
  uint32
    fc_ethertype = 0x8906;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FCF_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  if (glbl_info->enable) {
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ETHERTYPE_CUSTOMr, SOC_CORE_ALL, 0, ETHERTYPE_CUSTOM_1f, fc_ethertype));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_glbl_info_set_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_fcf_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_GLBL_INFO                *glbl_info
  )
{
	return 0;
}

uint32
  arad_pp_frwrd_fcf_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_GLBL_INFO                *glbl_info
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_ROUTE_ADD_UNSAFE);

    
    if (route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_LOCAL_DOMAIN) {
        res = arad_pp_fcf_key_sem_op(unit,route_key,ARAD_PP_FRWRD_FC_OP_ADD,success,0);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    
    if (route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN || route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN_NPORT) {
        if (SOC_IS_JERICHO(unit)) {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
            SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
            uint32 payload;
            uint32 tmp;

            SOC_PPC_FP_QUAL_VAL_clear(qual_vals);            

            qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            tmp = (route_key->d_id >> (ARAD_PP_FRWRD_FCF_D_ID_ROUTE_LSB)) & ARADP_PP_FRWRD_FCF_MASK_BITS(ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS);
            qual_vals[0].val.arr[0] = tmp;
            qual_vals[0].is_valid.arr[0] = ARADP_PP_FRWRD_FCF_MASK_BITS(route_key->prefix_len) << (ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS - route_key->prefix_len);

            qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI;
            qual_vals[1].val.arr[0] = route_key->vfi << ARAD_PP_FRWRD_FCF_KEY_NOF_R_BITS;
            qual_vals[1].is_valid.arr[0] = SOC_SAND_U32_MAX;

            payload = (route_info->frwrd_decision.dest_id);

            payload = (payload + (1 << 17)); 
            
            if(route_key->flags == SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN_NPORT){
                qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
                res = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, qual_vals, 0, &payload, success);
            }else {
                res = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS, qual_vals, 0, &payload, success);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
#endif
        } else {
            res = arad_pp_frwrd_fcf_lpm_route_add(unit,route_key,route_info,success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }        
    }
    
    if (route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_HOST || route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_HOST_NPORT) {
        res = arad_pp_fcf_key_lem_op(unit,route_key,0,ARAD_PP_FRWRD_FC_OP_ADD,(SOC_PPC_FRWRD_FCF_ROUTE_INFO*)route_info,0,0,success,0);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_route_add_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fcf_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO            *route_info
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                               exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS      *route_status,
    SOC_SAND_OUT uint8                               *found
  )
{
	uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(found);
  
  
  if (route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_LOCAL_DOMAIN) {
      res = arad_pp_fcf_key_sem_op(unit,route_key,ARAD_PP_FRWRD_FC_OP_GET,0,found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      *route_status = SOC_PPC_FRWRD_FCF_ROUTE_STATUS_COMMITED;
  }
  
  if (route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN || route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN_NPORT) {
      if (SOC_IS_JERICHO(unit)) {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
            SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
            uint32 payload;
            uint32 tmp;

            SOC_PPC_FP_QUAL_VAL_clear(qual_vals);            

            qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            tmp = (route_key->d_id >> (ARAD_PP_FRWRD_FCF_D_ID_ROUTE_LSB)) & ARADP_PP_FRWRD_FCF_MASK_BITS(ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS);
            qual_vals[0].val.arr[0] = tmp;
            qual_vals[0].is_valid.arr[0] = ARADP_PP_FRWRD_FCF_MASK_BITS(route_key->prefix_len) << (ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS - route_key->prefix_len);

            qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI;
            qual_vals[1].val.arr[0] = route_key->vfi << ARAD_PP_FRWRD_FCF_KEY_NOF_R_BITS;
            qual_vals[1].is_valid.arr[0] = SOC_SAND_U32_MAX;            
            
            if(route_key->flags == SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN_NPORT){
                qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
                res = arad_pp_dbal_entry_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, qual_vals, &payload, 0, 0,found);
            }else {
                res = arad_pp_dbal_entry_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS, qual_vals, &payload, 0, 0, found);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            route_info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
            route_info->frwrd_decision.dest_id = payload;
#endif
        } else {
            res = arad_pp_frwrd_fcf_lpm_route_get(unit,route_key,exact_match,route_info,route_status,found);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
  }
  
  if (route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_HOST || route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_HOST_NPORT) {
      res = arad_pp_fcf_key_lem_op(unit,route_key,0,ARAD_PP_FRWRD_FC_OP_GET,route_info,0,found,0,route_status);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_route_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fcf_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_INFO            *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
	return 0;
}

uint32
  arad_pp_frwrd_fcf_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY         *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE            *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_ROUTE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);
  
  
  if (route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_LOCAL_DOMAIN) {
      res = arad_pp_fcf_key_sem_op(unit,route_key,ARAD_PP_FRWRD_FC_OP_REMOVE,0,0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  
  if (route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN || route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN_NPORT) {
      if (SOC_IS_JERICHO(unit)) {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
            SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
            uint32 tmp;

            SOC_PPC_FP_QUAL_VAL_clear(qual_vals);            

            qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
            tmp = (route_key->d_id >> (ARAD_PP_FRWRD_FCF_D_ID_ROUTE_LSB)) & ARADP_PP_FRWRD_FCF_MASK_BITS(ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS);
            qual_vals[0].val.arr[0] = tmp;
            qual_vals[0].is_valid.arr[0] = ARADP_PP_FRWRD_FCF_MASK_BITS(route_key->prefix_len) << (ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS - route_key->prefix_len);

            qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI;
            qual_vals[1].val.arr[0] = route_key->vfi << ARAD_PP_FRWRD_FCF_KEY_NOF_R_BITS;
            qual_vals[1].is_valid.arr[0] = SOC_SAND_U32_MAX;
            
            if(route_key->flags == SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN_NPORT){
                qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
                res = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS, qual_vals, success);
            }else {
                res = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS, qual_vals, success);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
#endif
        } else {
            res = arad_pp_frwrd_fcf_lpm_route_remove(unit,route_key,success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
  }
  
  if (route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_HOST || route_key->flags & SOC_PPC_FRWRD_FCF_ROUTE_HOST_NPORT) {
      res = arad_pp_fcf_key_lem_op(unit,route_key,0,ARAD_PP_FRWRD_FC_OP_REMOVE,0,0,0,success,0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_route_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fcf_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key
  )
{
	return 0;
}

uint32
    arad_pp_frwrd_fcf_route_lem_clear_from_shadow(int unit)
{
    uint32 nof_entries = 0, prefix, logical_prefix1 = 0, logical_prefix2 = 0, logical_prefix3 = 0;
    uint32 entry_key_array[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    uint32 data_out[ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S * DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    int i;

    SOCDNX_INIT_FUNC_DEFS; 
    
    if(!SOC_DPP_IS_LEM_SIM_ENABLE(unit)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("exact match shadowing must be enabled for this diagnostics")));
    }   

    
    arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_LOCAL,&logical_prefix1);
    arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_REMOTE,&logical_prefix2);
    arad_pp_lem_access_app_to_prefix_get(unit,ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_LOCAL_N_PORT,&logical_prefix3);

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))){

        SOCDNX_SAND_IF_ERR_EXIT(chip_sim_em_get_block(unit, ARAD_CHIP_SIM_LEM_BASE,  ARAD_CHIP_SIM_LEM_KEY, ARAD_CHIP_SIM_LEM_PAYLOAD,
                                NULL, entry_key_array, data_out, &nof_entries, &block_range));

        for (i = 0; i < nof_entries; i++) {
            
            prefix = 0;
            soc_sand_bitstream_get_any_field(&(entry_key_array[i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]),
              SOC_DPP_DEFS_GET(unit, lem_width) - SOC_DPP_DEFS_GET(unit, nof_lem_prefixes),
              SOC_DPP_DEFS_GET(unit, nof_lem_prefixes), &prefix);

            if ((prefix == logical_prefix1) || (prefix == logical_prefix2) || (prefix == logical_prefix3)) 
            {
                chip_sim_exact_match_entry_remove_unsafe(unit, ARAD_CHIP_SIM_LEM_BASE, &entry_key_array[ i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S],ARAD_CHIP_SIM_LEM_KEY);                
            }
        }
    }    

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_frwrd_fcf_routing_table_clear_lem_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags
  )
{
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_FRWRD_FCF_ROUTE_KEY       fcf_route_key;
    ARAD_PP_LEM_ACCESS_KEY            lem_key;
    ARAD_PP_LEM_ACCESS_KEY            lem_key_mask;
    SOC_SAND_TABLE_BLOCK_RANGE        block_range;
    uint32                            nof_entries;
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        action;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR_UNSAFE);

    ARAD_PP_LEM_ACCESS_KEY_clear(&lem_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&lem_key_mask);
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_PPC_FRWRD_FCF_ROUTE_KEY_clear(&fcf_route_key);

    
    action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

    
    lem_key.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
    lem_key.prefix.value = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_LOCAL;
    lem_key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
    lem_key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;

    res = arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&lem_key,&lem_key_mask,NULL,&block_range,&action, TRUE,&nof_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

     
    lem_key.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
    lem_key.prefix.value = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_REMOTE;

    
    
    action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

    res = arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&lem_key,&lem_key_mask,NULL,&block_range,&action, TRUE,&nof_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    
    lem_key.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
    lem_key.prefix.value = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_LOCAL_N_PORT;

    
    
    action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

    res = arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&lem_key,&lem_key_mask,NULL,&block_range,&action, TRUE,&nof_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(SOC_DPP_IS_LEM_SIM_ENABLE(unit)){
        arad_pp_frwrd_fcf_route_lem_clear_from_shadow(unit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_routing_table_clear_lem_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_fcf_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR_UNSAFE);

  
  res = arad_pp_frwrd_fcf_lpm_routing_table_clear(
          unit,
          vfi_ndx,
          flags
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
  res = arad_pp_frwrd_fcf_routing_table_clear_lem_unsafe(
          unit,
          vfi_ndx,
          flags
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_routing_table_clear_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fcf_routing_table_clear_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags
)
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_zoning_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_ZONING_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(zoning_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_fcf_key_lem_op(unit,0,zoning_key,ARAD_PP_FRWRD_FC_OP_ADD,0,(SOC_PPC_FRWRD_FCF_ZONING_INFO*)routing_info,0,success,0);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_zoning_add_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fcf_zoning_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO      *routing_info
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_zoning_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY       *zoning_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS     *route_status,
    SOC_SAND_OUT uint8                             *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_ZONING_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(zoning_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_FRWRD_FCF_ZONING_INFO_clear(routing_info);

  res = arad_pp_fcf_key_lem_op(unit,0,zoning_key,ARAD_PP_FRWRD_FC_OP_GET,0,routing_info,found,0,route_status);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_zoning_get_unsafe()", 0, 0);

}

uint32
  arad_pp_frwrd_fcf_zoning_get_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_zoning_get_block_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_INFO      *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
	return 0;
}



 uint32
  arad_pp_frwrd_fcf_zoning_vfi_clear_unsafe(
    SOC_SAND_IN  int                                  unit
  )
{
	return 0;
}

uint32
  arad_pp_frwrd_fcf_zoning_get_block_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_zoning_remove_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_ZONING_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(zoning_key);

  res = arad_pp_fcf_key_lem_op(unit,0,zoning_key,ARAD_PP_FRWRD_FC_OP_REMOVE,0,0,0,0,0);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_zoning_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fcf_zoning_remove_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key
  )
{
	return 0;
}




uint32
  arad_pp_frwrd_fcf_zoning_table_clear_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               flags,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY         *key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO        *info
  )
{

    uint32
      res = SOC_SAND_OK;
    ARAD_PP_LEM_ACCESS_KEY            lem_key;
    ARAD_PP_LEM_ACCESS_KEY            lem_key_mask;
    SOC_SAND_TABLE_BLOCK_RANGE        block_range;
    uint32                            nof_entries;
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        action;
    SOC_PPC_FRWRD_FCF_ZONING_KEY key_mask;



    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FCF_ZONING_TABLE_CLEAR);


    ARAD_PP_LEM_ACCESS_KEY_clear(&lem_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&lem_key_mask);
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_PPC_FRWRD_FCF_ZONING_KEY_clear(&key_mask);


    
    if (flags & SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_VFT) {
        key_mask.vfi = ARAD_PP_FRWRD_FCF_VFT_MAX;
    }
    if (flags & SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_D_ID) {
        key_mask.d_id = ARAD_PP_FRWRD_FCF_D_ID_MAX;
    }
    if (flags & SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_S_ID) {
        key_mask.s_id = ARAD_PP_FRWRD_FCF_S_ID_MAX;
    }

    
    res = arad_pp_fcf_zoning_key_to_lem_key(unit,key,&lem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_pp_fcf_zoning_key_to_lem_key(unit,&key_mask,&lem_key_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    lem_key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
    lem_key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;


    
    action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

    res = arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&lem_key,&lem_key_mask,NULL,&block_range,&action, TRUE,&nof_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fcf_zoning_table_clear_unsafe()", 0, 0);
}

soc_error_t
  arad_pp_frwrd_fcf_npv_switch_set(
    int                                 unit,
    int                                 enable
  )
{
    int curr_enable;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_frwrd_fcf_npv_switch_get(unit,&curr_enable));


    if(curr_enable == enable){
        return 0;
    }
    if (enable) {
        
        
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_npv_programs_init(unit));

    } else {

        
        SOCDNX_IF_ERR_EXIT(arad_pp_flp_n_port_programs_disable(unit));        
    }

    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_fcoe_zoning_set(unit, enable));

    if (soc_property_get(unit, spn_FCOE_NPV_BRIDGE_MODE, 0)) {
        arad_pp_dbal_vtt_fcoe_npv_mode_no_my_mac_set(unit, enable); 
		
		SOCDNX_SAND_IF_ERR_EXIT(arad_egr_prog_editor_invalidate_program_by_id(unit, ARAD_EGR_PROG_EDITOR_PROG_FCOE_NP_PORT, enable));
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_prog_editor_invalidate_program_by_id(unit, ARAD_EGR_PROG_EDITOR_PROG_FCOE_N_PORT, enable));
    }    

    exit:
        SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_pp_frwrd_fcf_vsan_mode_set(
    int                                 unit,
    int                                 is_vsan_from_vsi
  )
{
    int curr_is_vsan_from_vsi;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_frwrd_fcf_vsan_mode_get(unit,&curr_is_vsan_from_vsi));


    if(curr_is_vsan_from_vsi == is_vsan_from_vsi){
        return 0;
    }
    
    
    SOCDNX_IF_ERR_EXIT(arad_pp_flp_fcoe_vsan_mode_set(unit, is_vsan_from_vsi));

    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vtt_fc_vsan_mode_update(unit, is_vsan_from_vsi));

exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_pp_frwrd_fcf_vsan_mode_get(
    int                                 unit,
    int*                                is_vsan_from_vsi
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    arad_pp_flp_fcoe_is_vsan_from_vsi_mode(unit, is_vsan_from_vsi);    

    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_pp_frwrd_fcf_npv_switch_get(
    int                                 unit,
    int*                                enable
  )
{
    int is_zoning_enabled;

    SOCDNX_INIT_FUNC_DEFS;

    arad_pp_flp_fcoe_is_zoning_enabled(unit, &is_zoning_enabled);
    (*enable) = !is_zoning_enabled;

    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_frwrd_fcf_zoning_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                               flags,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY         *key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO        *info
  )
{
	return 0;
}


uint32
  arad_pp_frwrd_fcf_vfi_all_routing_tables_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 flags
  )
{
	return 0;
}

uint32
  arad_pp_frwrd_fcf_vfi_all_routing_tables_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 flags
  )
{
	return 0;
}




CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_fcf_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_frwrd_fcf;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_fcf_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_frwrd_fcf;
}

uint32
  SOC_PPC_FRWRD_FCF_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_GLBL_INFO *info
  )
{
	return 0;
}

uint32
  SOC_PPC_FRWRD_FCF_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY *info
  )
{
	return 0;
}


uint32
  SOC_PPC_FRWRD_FCF_ROUTE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO *info
  )
{
	return 0;
}

uint32
  SOC_PPC_FRWRD_FCF_ZONING_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY *info
  )
{
	return 0;
}

uint32
  SOC_PPC_FRWRD_FCF_ZONING_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO *info
  )
{
	return 0;
}




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


