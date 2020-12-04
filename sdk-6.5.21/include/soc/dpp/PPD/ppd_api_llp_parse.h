/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LLP_PARSE_INCLUDED__

#define __SOC_PPD_API_LLP_PARSE_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_llp_parse.h>

#include <soc/dpp/PPD/ppd_api_general.h>














typedef enum
{
  
  SOC_PPD_LLP_PARSE_TPID_VALUES_SET = SOC_PPD_PROC_DESC_BASE_LLP_PARSE_FIRST,
  SOC_PPD_LLP_PARSE_TPID_VALUES_SET_PRINT,
  SOC_PPD_LLP_PARSE_TPID_VALUES_GET,
  SOC_PPD_LLP_PARSE_TPID_VALUES_GET_PRINT,
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET,
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_PRINT,
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET,
  SOC_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET_PRINT,
  SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET,
  SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PRINT,
  SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET,
  SOC_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PRINT,
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET,
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PRINT,
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET,
  SOC_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PRINT,
  SOC_PPD_LLP_PARSE_GET_PROCS_PTR,
  



  
  SOC_PPD_LLP_PARSE_PROCEDURE_DESC_LAST
} SOC_PPD_LLP_PARSE_PROCEDURE_DESC;










uint32
  soc_ppd_llp_parse_tpid_values_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_VALUES               *tpid_vals
  );


uint32
  soc_ppd_llp_parse_tpid_values_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_VALUES               *tpid_vals
  );


uint32
  soc_ppd_llp_parse_tpid_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );


uint32
  soc_ppd_llp_parse_tpid_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tpid_profile_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO         *tpid_profile_info
  );


uint32
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  uint32                                tpid_profile_id
  );


uint32
  soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_OUT uint32                                *tpid_profile_id
  );


uint32
  soc_ppd_llp_parse_packet_format_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );


uint32
  soc_ppd_llp_parse_packet_format_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

