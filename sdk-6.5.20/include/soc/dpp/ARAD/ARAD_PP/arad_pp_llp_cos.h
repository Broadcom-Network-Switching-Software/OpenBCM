/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LLP_COS_INCLUDED__

#define __ARAD_PP_LLP_COS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_llp_cos.h>





#define ARAD_PP_LLP_COS_NOF_SUBNET_CLASSIFY (16)

#define ARAD_PP_LLP_TRAP_NOF_UD_ETHER_TYPE   (7)
#define ARAD_PP_LLP_TRAP_NEXT_PRTCL_NDX_UD   (0xff)

#define ARAD_PP_LLP_COS_L2_NEXT_PRTCL_UD_NDX_TO_TBL_NDX(ndx) (ndx+1)
#define ARAD_PP_LLP_COS_L2_NEXT_PRTCL_TBL_NDX_TO_UD_NDX(ndx) (ndx-1)


#define ARAD_PP_LLP_COS_NOF_UP_TO_TC_TABLES (4)


#define ARAD_PP_LLP_COS_NOF_UP_TO_DP_TABLES (2)


#define ARAD_PP_LLP_COS_NOF_DE_TO_DP_TABLES (1)


#define ARAD_PP_LLP_COS_MAX_UP (7)


#define ARAD_PP_LLP_COS_MAX_DE (1)


#define ARAD_PP_LLP_COS_MAX_DP (3)










typedef enum
{
  
  SOC_PPC_LLP_COS_GLBL_INFO_SET = ARAD_PP_PROC_DESC_BASE_LLP_COS_FIRST,
  SOC_PPC_LLP_COS_GLBL_INFO_SET_PRINT,
  SOC_PPC_LLP_COS_GLBL_INFO_SET_UNSAFE,
  SOC_PPC_LLP_COS_GLBL_INFO_SET_VERIFY,
  SOC_PPC_LLP_COS_GLBL_INFO_GET,
  SOC_PPC_LLP_COS_GLBL_INFO_GET_PRINT,
  SOC_PPC_LLP_COS_GLBL_INFO_GET_VERIFY,
  SOC_PPC_LLP_COS_GLBL_INFO_GET_UNSAFE,
  SOC_PPC_LLP_COS_PORT_INFO_SET,
  SOC_PPC_LLP_COS_PORT_INFO_SET_PRINT,
  SOC_PPC_LLP_COS_PORT_INFO_SET_UNSAFE,
  SOC_PPC_LLP_COS_PORT_INFO_SET_VERIFY,
  SOC_PPC_LLP_COS_PORT_INFO_GET,
  SOC_PPC_LLP_COS_PORT_INFO_GET_PRINT,
  SOC_PPC_LLP_COS_PORT_INFO_GET_VERIFY,
  SOC_PPC_LLP_COS_PORT_INFO_GET_UNSAFE,
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET,
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET_PRINT,
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET_UNSAFE,
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET_VERIFY,
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET,
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET_PRINT,
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET_VERIFY,
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET_UNSAFE,
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET,
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET_PRINT,
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET_UNSAFE,
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET_VERIFY,
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET,
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET_PRINT,
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET_VERIFY,
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET_UNSAFE,
  ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET,
  ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_PRINT,
  ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_UNSAFE,
  ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_VERIFY,
  ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET,
  ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_PRINT,
  ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_VERIFY,
  ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_UNSAFE,
  ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET,
  ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET_PRINT,
  ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET_UNSAFE,
  ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET_VERIFY,
  ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET,
  ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET_PRINT,
  ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET_VERIFY,
  ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET_UNSAFE,
  ARAD_PP_LLP_COS_PROTOCOL_BASED_SET,
  ARAD_PP_LLP_COS_PROTOCOL_BASED_SET_PRINT,
  ARAD_PP_LLP_COS_PROTOCOL_BASED_SET_UNSAFE,
  ARAD_PP_LLP_COS_PROTOCOL_BASED_SET_VERIFY,
  ARAD_PP_LLP_COS_PROTOCOL_BASED_GET,
  ARAD_PP_LLP_COS_PROTOCOL_BASED_GET_PRINT,
  ARAD_PP_LLP_COS_PROTOCOL_BASED_GET_VERIFY,
  ARAD_PP_LLP_COS_PROTOCOL_BASED_GET_UNSAFE,
  ARAD_PP_LLP_COS_GET_PROCS_PTR,
  ARAD_PP_LLP_COS_GET_ERRS_PTR,
  

  ARAD_PP_LLP_COS_PROTOCOL_BASED_ETHER_TYPE_PROCESS,

  
  ARAD_PP_LLP_COS_PROCEDURE_DESC_LAST
} ARAD_PP_LLP_COS_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LLP_COS_LOCAL_PORT_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LLP_COS_FIRST,
  ARAD_PP_LLP_COS_MAPPING_TBL_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_TABLE_ID_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_ENTRY_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_RANGE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_L4_PRTCL_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_ETHER_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_DEFAULT_DP_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_TC_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_TC_TO_UP_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_UP_TO_DP_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_TC_TO_DP_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_IP_QOS_TO_TC_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_DEFAULT_TC_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_VALUE1_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_VALUE2_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_START_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_END_OUT_OF_RANGE_ERR,
  

  SOC_PPC_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR,
  ARAD_PP_LLP_COS_MAPPING_TBL_NDX_RANGE_MISMATCH_ERR,
  ARAD_PP_LLP_COS_IPV4_SUBNET_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_COS_ETHER_TYPE_NOT_EXIT_ERR,

  
  ARAD_PP_LLP_COS_ERR_LAST
} ARAD_PP_LLP_COS_ERR;









uint32
  arad_pp_llp_cos_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_llp_cos_l2_next_prtcl_type_to_ndx(
    SOC_SAND_IN  SOC_PPC_L2_NEXT_PRTCL_TYPE prtcl_type,
    SOC_SAND_OUT uint8 *ndx
  );


uint32
  arad_pp_llp_cos_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  );

uint32
  arad_pp_llp_cos_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  );

uint32
  arad_pp_llp_cos_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_llp_cos_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  );


uint32
  arad_pp_llp_cos_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  uint32                                  local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  );

uint32
  arad_pp_llp_cos_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  );

uint32
  arad_pp_llp_cos_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  local_port_ndx
  );


uint32
  arad_pp_llp_cos_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  uint32                                  local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  );


uint32
  arad_pp_llp_cos_mapping_table_entry_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  );

uint32
  arad_pp_llp_cos_mapping_table_entry_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  );

uint32
  arad_pp_llp_cos_mapping_table_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx
  );


uint32
  arad_pp_llp_cos_mapping_table_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  );


uint32
  arad_pp_llp_cos_l4_port_range_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  );

uint32
  arad_pp_llp_cos_l4_port_range_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  );

uint32
  arad_pp_llp_cos_l4_port_range_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx
  );


uint32
  arad_pp_llp_cos_l4_port_range_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  );


uint32
  arad_pp_llp_cos_l4_port_range_to_tc_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  );

uint32
  arad_pp_llp_cos_l4_port_range_to_tc_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  );

uint32
  arad_pp_llp_cos_l4_port_range_to_tc_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx
  );


uint32
  arad_pp_llp_cos_l4_port_range_to_tc_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  );


uint32
  arad_pp_llp_cos_ipv4_subnet_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  );

uint32
  arad_pp_llp_cos_ipv4_subnet_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  );

uint32
  arad_pp_llp_cos_ipv4_subnet_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  );


uint32
  arad_pp_llp_cos_ipv4_subnet_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_OUT SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  );


uint32
  arad_pp_llp_cos_protocol_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_llp_cos_protocol_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  );

uint32
  arad_pp_llp_cos_protocol_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx
  );


uint32
  arad_pp_llp_cos_protocol_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_cos_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_cos_get_errs_ptr(void);

uint32
  SOC_PPC_LLP_COS_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO *info
  );

uint32
  SOC_PPC_LLP_COS_TC_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO *info
  );

uint32
  SOC_PPC_LLP_COS_UP_USE_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_UP_USE *info
  );

uint32
  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES *info
  );

uint32
  SOC_PPC_LLP_COS_PORT_L2_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L2_INFO *info
  );

uint32
  SOC_PPC_LLP_COS_PORT_L3_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L3_INFO *info
  );

uint32
  SOC_PPC_LLP_COS_PORT_L4_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L4_INFO *info
  );

uint32
  SOC_PPC_LLP_COS_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO *info
  );

uint32
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  );

uint32
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO *info
  );

uint32
  SOC_PPC_LLP_COS_PRTCL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO *info
  );

uint32
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

