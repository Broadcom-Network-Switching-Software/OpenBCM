/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_FRWRD_FCF_INCLUDED__

#define __SOC_PPD_API_FRWRD_FCF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_frwrd_fcf.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_FRWRD_FCF_GLBL_INFO_SET = SOC_PPD_PROC_DESC_BASE_FRWRD_FCF_FIRST,
  SOC_PPD_FRWRD_FCF_GLBL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_FCF_GLBL_INFO_GET,
  SOC_PPD_FRWRD_FCF_GLBL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_FCF_ROUTE_ADD,
  SOC_PPD_FRWRD_FCF_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_FCF_ROUTE_GET,
  SOC_PPD_FRWRD_FCF_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_FCF_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_FCF_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_FCF_ROUTE_REMOVE, 
  SOC_PPD_FRWRD_FCF_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_FCF_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_FCF_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_FCF_ZONING_ADD,
  SOC_PPD_FRWRD_FCF_ZONING_ADD_PRINT,
  SOC_PPD_FRWRD_FCF_ZONING_GET,
  SOC_PPD_FRWRD_FCF_ZONING_GET_PRINT,
  SOC_PPD_FRWRD_FCF_ZONING_GET_BLOCK,
  SOC_PPD_FRWRD_FCF_ZONING_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_FCF_ZONING_REMOVE,
  SOC_PPD_FRWRD_FCF_ZONING_REMOVE_PRINT,
  SOC_PPD_FRWRD_FCF_ZONING_TABLE_CLEAR,
  SOC_PPD_FRWRD_FCF_ZONING_TABLE_CLEAR_PRINT,
  
  SOC_PPD_FRWRD_FCF_GET_PROCS_PTR,
  
  
  SOC_PPD_FRWRD_FCF_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_FCF_PROCEDURE_DESC;











uint32
  soc_ppd_frwrd_fcf_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_GLBL_INFO                *glbl_info
  );


uint32
  soc_ppd_frwrd_fcf_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_GLBL_INFO                *glbl_info
  );


uint32
  soc_ppd_frwrd_fcf_route_add(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_fcf_route_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                               exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS      *route_status,
    SOC_SAND_OUT uint8                               *found
  );


uint32
  soc_ppd_frwrd_fcf_route_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE      *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_INFO           *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS         *routes_status,
    SOC_SAND_OUT uint32                                 *nof_entries
  );


uint32
  soc_ppd_frwrd_fcf_route_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY          *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
  );


uint32
  soc_ppd_frwrd_fcf_routing_table_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags
   );


uint32
  soc_ppd_frwrd_fcf_zoning_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );


uint32
  soc_ppd_frwrd_fcf_zoning_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT uint8                           *found
  );


uint32
  soc_ppd_frwrd_fcf_zoning_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_INFO      *zoning_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                            *nof_entries
  );


uint32
  soc_ppd_frwrd_fcf_zoning_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY             *zoning_key
  );



uint32
  soc_ppd_frwrd_fcf_zoning_table_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               flags,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY         *key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO        *info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_frwrd_fcf_get_procs_ptr(void);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

