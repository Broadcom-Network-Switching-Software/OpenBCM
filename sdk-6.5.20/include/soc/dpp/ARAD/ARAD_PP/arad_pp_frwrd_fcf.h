
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FRWRD_FCF_INCLUDED__

#define __ARAD_PP_FRWRD_FCF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>

#include <soc/dpp/PPC/ppc_api_frwrd_fcf.h>







 


#define ARAD_PP_FRWRD_FCF_D_ID_BITS (24)

#define ARAD_PP_FRWRD_FCF_S_ID_BITS (24)

#define ARAD_PP_FRWRD_FCF_VFI_BITS (13)

#define ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS (8)



#define ARAD_PP_FRWRD_FCF_D_ID_ROUTE_LSB (ARAD_PP_FRWRD_FCF_D_ID_BITS-ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS)

#define ARADP_PP_FRWRD_FCF_MASK_BITS(__n)   ((1 << (__n)) - 1)
#define ARAD_PP_FRWRD_FCF_GET_DOMAIN(__d_id)  ((__d_id >> (ARAD_PP_FRWRD_FCF_D_ID_ROUTE_LSB)) & ARADP_PP_FRWRD_FCF_MASK_BITS(ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS))


#define ARAD_PP_FRWRD_FCF_SET_DOMAIN(__d_id, __domain)  ((__d_id) |= ((__domain & ARADP_PP_FRWRD_FCF_MASK_BITS(ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS)) << ARAD_PP_FRWRD_FCF_D_ID_ROUTE_LSB))


 

    
    
    
    
    
    



#define ARAD_PP_FRWRD_FCF_KEY_NOF_R_BITS (1)
#define ARAD_PP_FRWRD_FCF_KEY_D_ID_LSB     (32 - ARAD_PP_FRWRD_FCF_VFI_BITS - ARAD_PP_FRWRD_FCF_D_ID_ROUTE_BITS)
#define ARAD_PP_FRWRD_FCF_KEY_VFI_LSB     (32 - ARAD_PP_FRWRD_FCF_VFI_BITS + ARAD_PP_FRWRD_FCF_KEY_NOF_R_BITS) 









typedef enum
{
  
  SOC_PPC_FRWRD_FCF_GLBL_INFO_SET,
  SOC_PPC_FRWRD_FCF_GLBL_INFO_SET_PRINT,
  SOC_PPC_FRWRD_FCF_GLBL_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_FCF_GLBL_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_FCF_GLBL_INFO_GET,
  SOC_PPC_FRWRD_FCF_GLBL_INFO_GET_PRINT,
  SOC_PPC_FRWRD_FCF_GLBL_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_FCF_GLBL_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_FCF_ROUTE_ADD,
  ARAD_PP_FRWRD_FCF_ROUTE_ADD_PRINT,
  ARAD_PP_FRWRD_FCF_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_FCF_ROUTE_ADD_VERIFY,
  ARAD_PP_FRWRD_FCF_ROUTE_GET,
  ARAD_PP_FRWRD_FCF_ROUTE_GET_PRINT,
  ARAD_PP_FRWRD_FCF_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_FCF_ROUTE_GET_VERIFY,
  ARAD_PP_FRWRD_FCF_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_FCF_ROUTE_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_FCF_ROUTE_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_FCF_ROUTE_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_FCF_ROUTE_REMOVE,
  ARAD_PP_FRWRD_FCF_ROUTE_REMOVE_PRINT,
  ARAD_PP_FRWRD_FCF_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_FCF_ROUTE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR,
  ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR_PRINT,
  ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR_UNSAFE,
  ARAD_PP_FRWRD_FCF_ROUTING_TABLE_CLEAR_VERIFY,
  ARAD_PP_FRWRD_FCF_ZONING_ADD,
  ARAD_PP_FRWRD_FCF_ZONING_ADD_PRINT,
  ARAD_PP_FRWRD_FCF_ZONING_ADD_UNSAFE,
  ARAD_PP_FRWRD_FCF_ZONING_ADD_VERIFY,
  ARAD_PP_FRWRD_FCF_ZONING_GET,
  ARAD_PP_FRWRD_FCF_ZONING_GET_PRINT,
  ARAD_PP_FRWRD_FCF_ZONING_GET_UNSAFE,
  ARAD_PP_FRWRD_FCF_ZONING_GET_VERIFY,
  ARAD_PP_FRWRD_FCF_ZONING_GET_BLOCK,
  ARAD_PP_FRWRD_FCF_ZONING_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_FCF_ZONING_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_FCF_ZONING_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_FCF_ZONING_REMOVE,
  ARAD_PP_FRWRD_FCF_ZONING_REMOVE_PRINT,
  ARAD_PP_FRWRD_FCF_ZONING_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_FCF_ZONING_REMOVE_VERIFY,

  ARAD_PP_FRWRD_FCF_ZONING_TABLE_CLEAR,
  ARAD_PP_FRWRD_FCF_ZONING_TABLE_CLEAR_PRINT,
  ARAD_PP_FRWRD_FCF_GET_PROCS_PTR,
  ARAD_PP_FRWRD_FCF_GET_ERRS_PTR,
  
  ARAD_PP_FRWRD_FCF_DEF_ACTION_SET,
  ARAD_PP_FRWRD_FCF_L3_VPN_DEFAULT_ROUTING_ENABLE_SET,
  ARAD_PP_FRWRD_FCF_L3_VPN_DEFAULT_ROUTING_ENABLE_GET,
  ARAD_PP_FRWRD_FCF_LPM_ROUTE_ADD,
  ARAD_PP_FRWRD_FCF_LPM_ROUTE_IS_EXIST,
  ARAD_PP_FRWRD_FCF_LPM_ROUTE_CLEAR,
  ARAD_PP_FRWRD_FCF_LPM_ROUTE_REMOVE,
  ARAD_PP_FRWRD_FCF_LPM_ROUTE_GET,
  ARAD_PP_FRWRD_FCF_LPM_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_FCF_LEM_ROUTE_ADD,
  ARAD_PP_FRWRD_FCF_LEM_ROUTE_GET,
  ARAD_PP_FRWRD_FCF_LEM_ROUTE_REMOVE,
  ARAD_PP_FRWRD_FCF_OR_VPN_ROUTE_ADD,
  ARAD_PP_FRWRD_FCF_OR_VPN_ROUTE_GET,
  ARAD_PP_FRWRD_FCF_OR_VPN_ROUTE_GET_BLOCK,
  ARAD_PP_FRWRD_FCF_OR_VPN_ROUTE_REMOVE,
  ARAD_PP_FRWRD_FCF_OR_VPN_ROUTING_TABLE_CLEAR,


  ARAD_PP_FRWRD_FCF_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_FCF_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FRWRD_FCF_ROUTE_TYPES_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_FCF_FIRST,
  ARAD_PP_FRWRD_FCF_SUCCESS_OUT_OF_RANGE_ERR,
  SOC_PPC_FRWRD_FCF_ROUTE_STATUS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_FOUND_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_ROUTES_STATUS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_EXACT_MATCH_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_MEM_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_ROUTE_VAL_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_CACHE_MODE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_GROUP_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_EEP_OUT_OF_RANGE_ERR,
  
  ARAD_PP_FRWRD_FCF_MC_ILLEGAL_DEST_TYPE_ERR,
  ARAD_PP_FRWRD_FCF_MUST_ALL_VFI_ERR,
  ARAD_PP_FRWRD_FCF_CACHE_NOT_SUPPORTED_ERR,
  ARAD_PP_FRWRD_FCF_DEFRAGE_NOT_SUPPORTED_ERR,
  ARAD_PP_FRWRD_FCF_OPERATION_NOT_SUPPORTED_ON_VFI_ERR,
  ARAD_PP_FRWRD_FCF_IP_VRF_NOT_MATCH_RESERVED_ERR,
  ARAD_PP_FRWRD_FCF_FRWRD_D_ID_PREFIX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_FRWRD_INTERNAL_IMP_ERR,
  ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_PREFIX_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_FCF_FRWRD_ROUTE_LPM_DEST_ILLEGAL_ERR,


  
  ARAD_PP_FRWRD_FCF_ERR_LAST
} ARAD_PP_FRWRD_FCF_ERR;









uint32
  arad_pp_frwrd_fcf_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );



uint32
  arad_pp_frwrd_fcf_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_GLBL_INFO                *glbl_info
  );

uint32
  arad_pp_frwrd_fcf_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_GLBL_INFO                *glbl_info
  );

uint32
  arad_pp_frwrd_fcf_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_fcf_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_GLBL_INFO                *glbl_info
  );


uint32
  arad_pp_frwrd_fcf_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_fcf_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO            *route_info
  );


uint32
  arad_pp_frwrd_fcf_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_INFO                  *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_frwrd_fcf_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );


uint32
  arad_pp_frwrd_fcf_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_INFO                  *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_fcf_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range
  );


uint32
  arad_pp_frwrd_fcf_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_frwrd_fcf_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key
  );


uint32
  arad_pp_frwrd_fcf_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags

  );

uint32
  arad_pp_frwrd_fcf_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags
  );


uint32
  arad_pp_frwrd_fcf_zoning_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  );

uint32
  arad_pp_frwrd_fcf_zoning_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO      *routing_info
  );


uint32
  arad_pp_frwrd_fcf_zoning_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT uint8                             *found
  );

uint32
  arad_pp_frwrd_fcf_zoning_get_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key
  );


uint32
  arad_pp_frwrd_fcf_zoning_get_block_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_INFO      *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                              *nof_entries
  );

uint32
  arad_pp_frwrd_fcf_zoning_get_block_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key
  );


uint32
  arad_pp_frwrd_fcf_zoning_remove_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key
  );

uint32
  arad_pp_frwrd_fcf_zoning_remove_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key
  );



uint32
  arad_pp_frwrd_fcf_zoning_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                               flags,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY         *key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO        *info
  );

uint32
  arad_pp_frwrd_fcf_zoning_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                               flags,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY         *key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO        *info
  );

void
    arad_pp_fcf_route_key_from_lem_key(
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_KEY *route_key
    );
	

soc_error_t
  arad_pp_frwrd_fcf_npv_switch_set(
    int unit,
    int enable
  );

soc_error_t
  arad_pp_frwrd_fcf_vsan_mode_set(
    int                                 unit,
    int                                 is_vft_from_vsi
  );

soc_error_t
  arad_pp_frwrd_fcf_npv_switch_get(
    int unit,
    int* enable
  );

soc_error_t
  arad_pp_frwrd_fcf_vsan_mode_get(
    int                                 unit,
    int*                                is_vsan_from_vsi
  );
  

CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_fcf_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_fcf_get_errs_ptr(void);



uint32
  SOC_PPC_FRWRD_FCF_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_GLBL_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FCF_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY *info
  );


uint32
  SOC_PPC_FRWRD_FCF_ROUTE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO *info
  );

uint32
  SOC_PPC_FRWRD_FCF_ZONING_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY *info
  );

uint32
  SOC_PPC_FRWRD_FCF_ZONING_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO *info
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



