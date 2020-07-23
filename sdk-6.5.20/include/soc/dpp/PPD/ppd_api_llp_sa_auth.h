/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LLP_SA_AUTH_INCLUDED__

#define __SOC_PPD_API_LLP_SA_AUTH_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_llp_sa_auth.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_SET = SOC_PPD_PROC_DESC_BASE_LLP_SA_AUTH_FIRST,
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_SET_PRINT,
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_GET,
  SOC_PPD_LLP_SA_AUTH_PORT_INFO_GET_PRINT,
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_SET,
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_SET_PRINT,
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_GET,
  SOC_PPD_LLP_SA_AUTH_MAC_INFO_GET_PRINT,
  SOC_PPD_LLP_SA_AUTH_GET_BLOCK,
  SOC_PPD_LLP_SA_AUTH_GET_BLOCK_PRINT,
  SOC_PPD_LLP_SA_AUTH_GET_PROCS_PTR,
  



  
  SOC_PPD_LLP_SA_AUTH_PROCEDURE_DESC_LAST
} SOC_PPD_LLP_SA_AUTH_PROCEDURE_DESC;










uint32
  soc_ppd_llp_sa_auth_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  );


uint32
  soc_ppd_llp_sa_auth_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_PORT_INFO               *port_auth_info
  );


uint32
  soc_ppd_llp_sa_auth_mac_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key,
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_IN  uint8                               enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_llp_sa_auth_mac_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key,
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MAC_INFO                *mac_auth_info,
    SOC_SAND_OUT uint8                               *enable
  );


uint32
  soc_ppd_llp_sa_auth_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE              *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *mac_address_key_arr,
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MAC_INFO                *auth_info_arr,
    SOC_SAND_OUT uint32                                *nof_entries
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_llp_sa_auth_get_procs_ptr(void);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

