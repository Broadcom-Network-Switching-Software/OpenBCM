/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_LLP_COS_INCLUDED__

#define __SOC_PPD_API_LLP_COS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_llp_cos.h>

#include <soc/dpp/PPD/ppd_api_general.h>















typedef enum
{
  
  SOC_PPD_LLP_COS_GLBL_INFO_SET = SOC_PPD_PROC_DESC_BASE_LLP_COS_FIRST,
  SOC_PPD_LLP_COS_GLBL_INFO_SET_PRINT,
  SOC_PPD_LLP_COS_GLBL_INFO_GET,
  SOC_PPD_LLP_COS_GLBL_INFO_GET_PRINT,
  SOC_PPD_LLP_COS_PORT_INFO_SET,
  SOC_PPD_LLP_COS_PORT_INFO_SET_PRINT,
  SOC_PPD_LLP_COS_PORT_INFO_GET,
  SOC_PPD_LLP_COS_PORT_INFO_GET_PRINT,
  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET,
  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_PRINT,
  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET,
  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET_PRINT,
  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET,
  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_PRINT,
  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_GET,
  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_GET_PRINT,
  SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET,
  SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_PRINT,
  SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET,
  SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_PRINT,
  SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_SET,
  SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_SET_PRINT,
  SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_GET,
  SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_GET_PRINT,
  SOC_PPD_LLP_COS_PROTOCOL_BASED_SET,
  SOC_PPD_LLP_COS_PROTOCOL_BASED_SET_PRINT,
  SOC_PPD_LLP_COS_PROTOCOL_BASED_GET,
  SOC_PPD_LLP_COS_PROTOCOL_BASED_GET_PRINT,
  SOC_PPD_LLP_COS_GET_PROCS_PTR,
  
  
  SOC_PPD_LLP_COS_PROCEDURE_DESC_LAST
} SOC_PPD_LLP_COS_PROCEDURE_DESC;










uint32
  soc_ppd_llp_cos_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  );


uint32
  soc_ppd_llp_cos_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  );


uint32
  soc_ppd_llp_cos_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                                         core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  );


uint32
  soc_ppd_llp_cos_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  );


uint32
  soc_ppd_llp_cos_mapping_table_entry_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                table_id_ndx,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  );


uint32
  soc_ppd_llp_cos_mapping_table_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                table_id_ndx,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  );


uint32
  soc_ppd_llp_cos_l4_port_range_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  );


uint32
  soc_ppd_llp_cos_l4_port_range_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                range_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  );


uint32
  soc_ppd_llp_cos_l4_port_range_to_tc_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                   l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  );


uint32
  soc_ppd_llp_cos_l4_port_range_to_tc_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                   l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                dest_port_match_range_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  );


uint32
  soc_ppd_llp_cos_ipv4_subnet_based_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                     *subnet,
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  );


uint32
  soc_ppd_llp_cos_ipv4_subnet_based_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                     *subnet,
    SOC_SAND_OUT SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  );


uint32
  soc_ppd_llp_cos_protocol_based_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                      ether_type_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );


uint32
  soc_ppd_llp_cos_protocol_based_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                      ether_type_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

