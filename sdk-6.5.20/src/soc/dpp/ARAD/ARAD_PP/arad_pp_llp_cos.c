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

#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP

#include <shared/bsl.h>



#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_cos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/cmic.h> 





#define ARAD_PP_LLP_COS_MAPPING_TBL_NDX_MAX                      (SOC_PPC_NOF_LLP_COS_MAPPING_TABLES-1)
#define ARAD_PP_LLP_COS_TABLE_ID_NDX_MAX                         (SOC_PPC_NOF_LLP_COS_MAPPING_TABLES-1)
#define ARAD_PP_LLP_COS_RANGE_NDX_MAX                            (2)
#define ARAD_PP_LLP_COS_L4_PRTCL_TYPE_NDX_MAX                    (SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1)
#define ARAD_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_MAX             (SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1)
#define ARAD_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_MAX            (SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1)
#define ARAD_PP_LLP_COS_PROFILE_NDX_MAX                          (7)
#define ARAD_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_MAX             (3)
#define ARAD_PP_LLP_COS_TC_TO_UP_INDEX_MAX                       (3)
#define ARAD_PP_LLP_COS_UP_TO_DP_INDEX_MAX                       (3)
#define ARAD_PP_LLP_COS_TC_TO_DP_INDEX_MAX                       (0)
#define ARAD_PP_LLP_COS_IP_QOS_TO_TC_INDEX_MAX                   (1)
#define ARAD_PP_LLP_COS_VALUE1_MAX                               (7)
#define ARAD_PP_LLP_COS_VALUE2_MAX                               (7)

#define ARAD_PP_LLP_COS_IPV4_SUBNET_NDX_MAX (15)

#define ARAD_PP_LLP_COS_PCP_PROFILE_MAX    (3)
#define ARAD_PP_LLP_COS_TOS_INDEX_MAX      (1)
#define ARAD_PP_LLP_COS_UP_MAP_PROFILE_MAX (3)
#define ARAD_PP_LLP_COS_DP_PROFILE_MAX     (3)






#define ARAD_PP_LLP_COS_PCP_DECODE_TBL_OFFSET(table_id_ndx, entry_ndx) \
  ARAD_PP_FLDS_TO_BUFF_2(table_id_ndx, 3, entry_ndx, 3)


#define ARAD_PP_LLP_COS_INCOMING_UP_MAP_OFFSET(up_profile, up_pcp) \
  ARAD_PP_FLDS_TO_BUFF_2(up_profile, 2, up_pcp, 3)


#define ARAD_PP_LLP_COS_DROP_PRECEDENCE_MAP_PCP_OFFSET(dp_profile, up_pcp) \
  ARAD_PP_FLDS_TO_BUFF_2(dp_profile, 2, up_pcp, 3)


#define ARAD_PP_LLP_COS_L4_PORT_RANGES_2_TC_TABLE_OFFSET(is_tcp, src_port_range, dest_port_range) \
  ARAD_PP_FLDS_TO_BUFF_3(is_tcp, 1, src_port_range, 2, dest_port_range, 2)






typedef enum
{
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_NONE = 0,
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_MPLS = 8,
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_MAC_IN_MAC = 9,
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_ARP = 10,
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_CFM = 11,
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_IPV4 = 12,
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_IPV6 = 13,
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_FCOE = 14,
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_TRILL = 15,
  ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX_USER_DEFINED = 0xff
} ARAD_PP_LLP_COS_L2_NEXT_PRTCL_NDX;





CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_llp_cos[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_PROTOCOL_BASED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_PROTOCOL_BASED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_PROTOCOL_BASED_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_PROTOCOL_BASED_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_PROTOCOL_BASED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_PROTOCOL_BASED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_PROTOCOL_BASED_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_PROTOCOL_BASED_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_COS_PROTOCOL_BASED_ETHER_TYPE_PROCESS),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_llp_cos[] =
{
  
  {
    ARAD_PP_LLP_COS_MAPPING_TBL_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_MAPPING_TBL_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mapping_tbl_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LLP_COS_MAPPING_TABLES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_TABLE_ID_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_TABLE_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'table_id_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LLP_COS_MAPPING_TABLES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_ENTRY_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_ENTRY_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'entry_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LLP_COS_MAPPING_TABLES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_RANGE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_RANGE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'range_ndx' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_L4_PRTCL_TYPE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_L4_PRTCL_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'l4_prtcl_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'src_port_match_range_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'dest_port_match_range_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_L4_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'in_up_to_tc_and_de_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_TC_TO_UP_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_TC_TO_UP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tc_to_up_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_UP_TO_DP_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_UP_TO_DP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'up_to_dp_index' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_TC_TO_DP_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_TC_TO_DP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tc_to_dp_index' is out of range. \n\r "
    "Must be 0 for AradB.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_IP_QOS_TO_TC_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_IP_QOS_TO_TC_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'ip_qos_to_tc_index' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_VALUE1_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_VALUE1_OUT_OF_RANGE_ERR",
    "The parameter 'value1' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_VALUE2_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_VALUE2_OUT_OF_RANGE_ERR",
    "The parameter 'value2' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_START_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_START_OUT_OF_RANGE_ERR",
    "The parameter 'start' is out of range. \n\r "
    "The range is: 0 - 65535.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_END_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_END_OUT_OF_RANGE_ERR",
    "The parameter 'end' is out of range. \n\r "
    "The range is: 0 - 65535.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    SOC_PPC_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR,
    "SOC_PPC_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR",
    "Table is not supported in AradB. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
   
  {
    ARAD_PP_LLP_COS_MAPPING_TBL_NDX_RANGE_MISMATCH_ERR,
    "ARAD_PP_LLP_COS_MAPPING_TBL_NDX_RANGE_MISMATCH_ERR",
    "Table index value is not in range for this table. \n\r "
    "Allowed ranges:\n\r "
    "up_to_de_tc table: 0 - 3\n\r "
    "ipv4_tos_to_dp_tc_valid table: 0 - 1\n\r "
    "ipv6_tc_to_dp_tc_valid table: 0 - 1\n\r "
    "incoming_up_to_up table: 0 - 3\n\r "
    "up_to_dp table: 0 - 3\n\r "
    "Other tables: 0 - 0\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_IPV4_SUBNET_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_COS_IPV4_SUBNET_NDX_OUT_OF_RANGE_ERR",
    "'entry_ndx' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_COS_ETHER_TYPE_NOT_EXIT_ERR,
    "ARAD_PP_LLP_COS_ETHER_TYPE_NOT_EXIT_ERR",
    "Ether-type is not one of the predefined ether-types, and was not. \n\r "
    "configured as a user-defined ether-type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_llp_cos_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_LLP_COS_MAPPING_TABLE
    mapping_tbl_ndx;
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO
    entry;
  uint32
    table_id_ndx = 0,
    entry_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_PCID_LITE_SKIP(unit);


  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&entry);

  
  mapping_tbl_ndx = SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC;

  for (table_id_ndx=0; table_id_ndx<ARAD_PP_LLP_COS_NOF_UP_TO_TC_TABLES; ++table_id_ndx)
  {
    for (entry_ndx=0; entry_ndx<(ARAD_PP_LLP_COS_MAX_UP + 1); ++entry_ndx)
    {
      
      entry.value1 = ARAD_PP_LLP_COS_MAX_DE - entry_ndx/4;
      
      entry.value2 = entry_ndx;
      
      res = arad_pp_llp_cos_mapping_table_entry_set_unsafe(
        unit,
        mapping_tbl_ndx,
        table_id_ndx,
        entry_ndx,
        &entry
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

  
  mapping_tbl_ndx = SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP;

  for (table_id_ndx=0; table_id_ndx<ARAD_PP_LLP_COS_NOF_UP_TO_DP_TABLES; ++table_id_ndx)
  {
    for (entry_ndx=0; entry_ndx<(ARAD_PP_LLP_COS_MAX_UP + 1); ++entry_ndx)
    {
      
      entry.value1 = ARAD_PP_LLP_COS_MAX_DP - entry_ndx/2;
      res = arad_pp_llp_cos_mapping_table_entry_set_unsafe(
        unit,
        mapping_tbl_ndx,
        table_id_ndx,
        entry_ndx,
        &entry
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

  
  mapping_tbl_ndx = SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP;

  for (table_id_ndx=0; table_id_ndx<ARAD_PP_LLP_COS_NOF_DE_TO_DP_TABLES; ++table_id_ndx)
  {
    for (entry_ndx=0; entry_ndx<(ARAD_PP_LLP_COS_MAX_DE + 1); ++entry_ndx)
    {
      
      entry.value1 = entry_ndx;
      
      res = arad_pp_llp_cos_mapping_table_entry_set_unsafe(
        unit,
        mapping_tbl_ndx,
        table_id_ndx,
        entry_ndx,
        &entry
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_init_unsafe()", table_id_ndx, entry_ndx);
}


uint32
  arad_pp_llp_cos_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_DE_TO_DP_MAPr, SOC_CORE_ALL, 0, DEFAULT_DROP_PRECEDENCEf,  glbl_info->default_dp));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_glbl_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_llp_cos_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_GLBL_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_GLBL_INFO, glbl_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_glbl_info_set_verify()", 0, 0);
}

uint32
  arad_pp_llp_cos_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_GLBL_INFO_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_glbl_info_get_verify()", 0, 0);
}


uint32
  arad_pp_llp_cos_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res,
    fld_val;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPC_LLP_COS_GLBL_INFO_clear(glbl_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_DE_TO_DP_MAPr, SOC_CORE_ALL, 0, DEFAULT_DROP_PRECEDENCEf, &fld_val));

  glbl_info->default_dp = (SOC_SAND_PP_DP)fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_glbl_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_llp_cos_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_PINFO_LLR_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_ihp_pinfo_llr_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  tbl_data.default_traffic_class = port_info->default_tc;

  
  tbl_data.pcp_profile = port_info->l2_info.tbls_select.in_up_to_tc_and_de_index;
  tbl_data.incoming_up_map_profile = port_info->l2_info.tbls_select.tc_to_up_index;
  tbl_data.drop_precedence_profile = port_info->l2_info.tbls_select.up_to_dp_index;

  tbl_data.ignore_incoming_up = SOC_SAND_BOOL2NUM(!port_info->l2_info.up_use.use_for_tc);
  tbl_data.use_dei = SOC_SAND_BOOL2NUM(port_info->l2_info.use_dei);
  tbl_data.tc_port_protocol_enable = SOC_SAND_BOOL2NUM(port_info->l2_info.use_l2_protocol);
  tbl_data.ignore_incoming_pcp = SOC_SAND_BOOL2NUM(port_info->l2_info.ignore_pkt_pcp_for_tc);

  
  tbl_data.tc_dp_tos_index = port_info->l3_info.ip_qos_to_tc_index;
  tbl_data.tc_dp_tos_enable = SOC_SAND_BOOL2NUM(port_info->l3_info.use_ip_qos);
  tbl_data.tc_subnet_enable = SOC_SAND_BOOL2NUM(port_info->l3_info.use_ip_subnet);

  
  tbl_data.tc_l_4_protocol_enable = port_info->l4_info.use_l4_prtcl;

  res = arad_pp_ihp_pinfo_llr_tbl_set_unsafe(
          unit,
          core_id,
          local_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_cos_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_PORT_INFO, port_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_cos_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_port_info_get_verify()", local_port_ndx, 0);
}


uint32
  arad_pp_llp_cos_port_info_get_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  int                                        core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_PINFO_LLR_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPC_LLP_COS_PORT_INFO_clear(port_info);

  res = arad_pp_ihp_pinfo_llr_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  port_info->default_tc = (SOC_SAND_PP_TC)tbl_data.default_traffic_class;

  
  port_info->l2_info.tbls_select.in_up_to_tc_and_de_index = tbl_data.pcp_profile;
  port_info->l2_info.tbls_select.tc_to_up_index = tbl_data.incoming_up_map_profile;
  port_info->l2_info.tbls_select.up_to_dp_index = tbl_data.drop_precedence_profile;

  port_info->l2_info.up_use.use_for_tc = SOC_SAND_NUM2BOOL(!tbl_data.ignore_incoming_up);
  port_info->l2_info.use_dei = SOC_SAND_NUM2BOOL(tbl_data.use_dei);
  port_info->l2_info.use_l2_protocol = SOC_SAND_NUM2BOOL(tbl_data.tc_port_protocol_enable);
  port_info->l2_info.ignore_pkt_pcp_for_tc = SOC_SAND_NUM2BOOL(tbl_data.ignore_incoming_pcp);

  
  port_info->l3_info.ip_qos_to_tc_index = tbl_data.tc_dp_tos_index;
  port_info->l3_info.use_ip_qos = (uint8)tbl_data.tc_dp_tos_enable;
  port_info->l3_info.use_ip_subnet = (uint8)tbl_data.tc_subnet_enable;

  
  port_info->l4_info.use_l4_prtcl = SOC_SAND_NUM2BOOL(tbl_data.tc_l_4_protocol_enable);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_port_info_get_unsafe()", local_port_ndx, 0);
}


uint32
  arad_pp_llp_cos_mapping_table_entry_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE             mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO  *entry
  )
{
  uint32  
      res = SOC_SAND_OK,
      reg_offset,
      bit_offset,
      offset,
      reg_val[1],
      reg64_vals_arr[2],
      fld_val,
      is_ipv4; 
  ARAD_PP_IHP_TOS_2_COS_TBL_DATA
      tos_2_cos_tbl_data;
  soc_reg_above_64_val_t
      reg_above64_val;
  uint64
      reg64_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET_UNSAFE);
  COMPILER_64_ZERO(reg64_val);
  SOC_SAND_CHECK_NULL_INPUT(entry);

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  ARAD_CLEAR(reg64_vals_arr, uint32, 2);
 
  
  is_ipv4 = 0;

  switch (mapping_tbl_ndx)
  {
  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC:
    
    offset = ARAD_PP_LLP_COS_PCP_DECODE_TBL_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 8;
    bit_offset = (offset % 8) * 4;
    fld_val = ((entry->value1 << 3) + entry->value2);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_IHP_PCP_DECODING_TABLEr(unit, REG_PORT_ANY, reg_above64_val));
    
    res = soc_sand_bitstream_set_field(
            &reg_above64_val[reg_offset],
            bit_offset,
            4 ,
            fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, WRITE_IHP_PCP_DECODING_TABLEr(unit, REG_PORT_ANY, reg_above64_val));

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID:
    is_ipv4 = 1;
    

  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID:
    offset = ARAD_IHP_TOS_2_COS_KEY_ENTRY_OFFSET(is_ipv4, table_id_ndx, entry_ndx);

    res = arad_pp_ihp_tos_2_cos_tbl_get_unsafe(
      unit,
      offset,
      &tos_2_cos_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    tos_2_cos_tbl_data.dp = entry->value1;
    tos_2_cos_tbl_data.tc = entry->value2;
    tos_2_cos_tbl_data.valid = SOC_SAND_BOOL2NUM(entry->valid);

    res = arad_pp_ihp_tos_2_cos_tbl_set_unsafe(
      unit,
      offset,
      &tos_2_cos_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP:
    
    offset = ARAD_PP_LLP_COS_INCOMING_UP_MAP_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 8;
    bit_offset = (offset % 8) * 3;
    fld_val = entry->value1;

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, READ_IHP_INCOMING_UP_MAPr(unit, REG_PORT_ANY, reg_offset, reg_val));

    res = soc_sand_bitstream_set_field(
      reg_val,
      bit_offset,
      3 ,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1040, exit, WRITE_IHP_INCOMING_UP_MAPr(unit, REG_PORT_ANY, reg_offset, *reg_val));

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_TC_TO_UP:

    
    bit_offset = entry_ndx * 3;
    fld_val = entry->value1;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_TRAFFIC_CLASS_TO_USER_PRIORITYr(unit, SOC_CORE_ALL, reg_val));

    res = soc_sand_bitstream_set_field(
      reg_val,
      bit_offset,
      3 ,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_TRAFFIC_CLASS_TO_USER_PRIORITYr(unit, SOC_CORE_ALL, *reg_val));

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP:
    
    if (entry_ndx) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_DE_TO_DP_MAPr, SOC_CORE_ALL, 0, DP_WHEN_DE_IS_1f,  entry->value1));
    }
    else {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_DE_TO_DP_MAPr, SOC_CORE_ALL, 0, DP_WHEN_DE_IS_0f,  entry->value1));
    }

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP:

    
    offset = ARAD_PP_LLP_COS_DROP_PRECEDENCE_MAP_PCP_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 16;
    bit_offset = (offset % 16) * 2;
    fld_val = entry->value1;

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DROP_PRECEDENCE_MAP_PCPr(unit, SOC_CORE_ALL, &reg64_val));
    reg64_vals_arr[0] = COMPILER_64_LO(reg64_val);
    reg64_vals_arr[1] = COMPILER_64_HI(reg64_val);

    res = soc_sand_bitstream_set_field(
      &reg64_vals_arr[reg_offset],
      bit_offset,
      2 ,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    COMPILER_64_SET(reg64_val, reg64_vals_arr[1], reg64_vals_arr[0]);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DROP_PRECEDENCE_MAP_PCPr(unit, SOC_CORE_ALL,  reg64_val));

    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR, 140, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_mapping_table_entry_set_unsafe()", mapping_tbl_ndx, table_id_ndx);
}

uint32
  arad_pp_llp_cos_mapping_table_entry_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    up = 0,
    de = 0,
    dp = 0,
    tc = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET_VERIFY);

  res = arad_pp_llp_cos_mapping_table_entry_get_verify(
    unit,
    mapping_tbl_ndx,
    table_id_ndx,
    entry_ndx
    );
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO, entry, 40, exit);

  switch (mapping_tbl_ndx)
  {
  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC:
    de = entry->value1;
    tc = entry->value2;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID:
    dp = entry->value1;
    tc = entry->value2;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID:
    dp = entry->value1;
    tc = entry->value2;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP:
    up = entry->value1;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_TC_TO_UP:
    up = entry->value1;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP:
    dp = entry->value1;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP:
    dp = entry->value1;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR, 50, exit);
    break;
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 60, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(de, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 80, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 90, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_mapping_table_entry_set_verify()", 0, table_id_ndx);
}

uint32
  arad_pp_llp_cos_mapping_table_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  uint32
    up = 0,
    de = 0,
    tc = 0,
    ipv4_tos = 0,
    ipv6_tc = 0,
    tbl_id_ndx_max = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mapping_tbl_ndx, ARAD_PP_LLP_COS_MAPPING_TBL_NDX_MAX, ARAD_PP_LLP_COS_MAPPING_TBL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(table_id_ndx, ARAD_PP_LLP_COS_TABLE_ID_NDX_MAX, ARAD_PP_LLP_COS_TABLE_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

  switch (mapping_tbl_ndx)
  {
  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC:
    up = entry_ndx;
    tbl_id_ndx_max = ARAD_PP_LLP_COS_PCP_PROFILE_MAX;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID:
    ipv4_tos = entry_ndx;
    tbl_id_ndx_max = ARAD_PP_LLP_COS_TOS_INDEX_MAX;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID:
    ipv6_tc = entry_ndx;
    tbl_id_ndx_max = ARAD_PP_LLP_COS_TOS_INDEX_MAX;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP:
    up = entry_ndx;
    tbl_id_ndx_max = ARAD_PP_LLP_COS_UP_MAP_PROFILE_MAX;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_TC_TO_UP:
    tc = entry_ndx;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP:
    de = entry_ndx;
    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP:
    up = entry_ndx;
    tbl_id_ndx_max = ARAD_PP_LLP_COS_DP_PROFILE_MAX;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR, 50, exit);
    break;
  }

  
  SOC_SAND_ERR_IF_ABOVE_MAX(up, SOC_SAND_PP_PCP_UP_MAX, SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR, 60, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(de, SOC_SAND_PP_DEI_CFI_MAX, SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR, 80, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 100, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ipv4_tos, SOC_SAND_PP_IPV4_TOS_MAX, SOC_SAND_PP_IPV4_TOS_OUT_OF_RANGE_ERR, 110, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ipv6_tc, SOC_SAND_PP_IPV6_TC_MAX, SOC_SAND_PP_IPV6_TC_OUT_OF_RANGE_ERR, 120, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(table_id_ndx, tbl_id_ndx_max, ARAD_PP_LLP_COS_MAPPING_TBL_NDX_RANGE_MISMATCH_ERR, 130, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_mapping_table_entry_get_verify()", 0, table_id_ndx);
}


uint32
  arad_pp_llp_cos_mapping_table_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{
  uint32  
      res = SOC_SAND_OK,
      reg_offset,
      bit_offset,
      offset,
      reg_val,
      fld_val,
      is_ipv4;
  soc_reg_above_64_val_t
      reg_above64_val;
  uint64
      reg64_val; 
  ARAD_PP_IHP_TOS_2_COS_TBL_DATA
    tos_2_cos_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry);

  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(entry);
  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

  
  is_ipv4 = 0;

  switch (mapping_tbl_ndx)
  {
  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC:
    
    offset = ARAD_PP_LLP_COS_PCP_DECODE_TBL_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 8;
    bit_offset = (offset % 8) * 4;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1140, exit, READ_IHP_PCP_DECODING_TABLEr(unit, REG_PORT_ANY, reg_above64_val));

    res = soc_sand_bitstream_get_field(
      &reg_above64_val[reg_offset],
      bit_offset,
      4 ,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    entry->value2 = SOC_SAND_GET_BITS_RANGE(fld_val, 2, 0);
    entry->value1 = SOC_SAND_GET_BIT(fld_val, 3);

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID:
    is_ipv4 = 1;
    

  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID:
    offset = ARAD_IHP_TOS_2_COS_KEY_ENTRY_OFFSET(is_ipv4, table_id_ndx, entry_ndx);

    res = arad_pp_ihp_tos_2_cos_tbl_get_unsafe(
      unit,
      offset,
      &tos_2_cos_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    entry->value1 = tos_2_cos_tbl_data.dp;
    entry->value2 = tos_2_cos_tbl_data.tc;
    entry->valid = SOC_SAND_NUM2BOOL(tos_2_cos_tbl_data.valid);

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP:
    
    offset = ARAD_PP_LLP_COS_INCOMING_UP_MAP_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 8;
    bit_offset = (offset % 8) * 3;

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1150, exit, READ_IHP_INCOMING_UP_MAPr(unit, REG_PORT_ANY, reg_offset, &reg_val));

    res = soc_sand_bitstream_get_field(
      &reg_val,
      bit_offset,
      3 ,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    entry->value1 = fld_val;

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_TC_TO_UP:

    
    bit_offset = entry_ndx * 3;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_TRAFFIC_CLASS_TO_USER_PRIORITYr(unit, SOC_CORE_ALL, &reg_val));

    res = soc_sand_bitstream_get_field(
      &reg_val,
      bit_offset,
      3 ,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    entry->value1 = fld_val;

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP:

      if (entry_ndx) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_DE_TO_DP_MAPr, SOC_CORE_ALL, 0, DP_WHEN_DE_IS_1f, &fld_val));
      }
      else {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_DE_TO_DP_MAPr, SOC_CORE_ALL, 0, DP_WHEN_DE_IS_0f, &fld_val));
      }

    entry->value1 = fld_val;

    break;

  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP:

    
    offset = ARAD_PP_LLP_COS_DROP_PRECEDENCE_MAP_PCP_OFFSET(table_id_ndx, entry_ndx);
    reg_offset = offset / 16;
    bit_offset = (offset % 16) * 2;

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DROP_PRECEDENCE_MAP_PCPr(unit, SOC_CORE_ALL, &reg64_val));
    if (reg_offset == 0) {
        reg_val = COMPILER_64_LO(reg64_val);
    } else {
        reg_val = COMPILER_64_HI(reg64_val);
    }

    res = soc_sand_bitstream_get_field(
      &reg_val,
      bit_offset,
      2 ,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    entry->value1 = fld_val;

    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_LLP_COS_MAPPING_TABLE_NOT_SUPPORTED_ERR, 140, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_mapping_table_entry_get_unsafe()", mapping_tbl_ndx, table_id_ndx);
}


uint32
  arad_pp_llp_cos_l4_port_range_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    reg_val,
    fld_val,
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1250, exit, READ_IHP_TRAFFIC_CLASS_L_4_RANGEr(unit, REG_PORT_ANY, range_ndx, &reg_val));

  fld_val = range_info->start;
  soc_reg_field_set(unit, IHP_TRAFFIC_CLASS_L_4_RANGEr, &reg_val, TC_L_4_RANGE_N_MINf, fld_val);

  fld_val = range_info->end;
  soc_reg_field_set(unit, IHP_TRAFFIC_CLASS_L_4_RANGEr, &reg_val, TC_L_4_RANGE_N_MAXf, fld_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1260, exit, WRITE_IHP_TRAFFIC_CLASS_L_4_RANGEr(unit, REG_PORT_ANY, range_ndx, reg_val));

  fld_val = SOC_SAND_BOOL2NUM_INVERSE(range_info->in_range);
  if (range_ndx == 0) {
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_TRAFFIC_CLASS_L_4_RANGE_CFGr, SOC_CORE_ALL, 0, TC_L_4_RANGE_OUTSIDE_0f,  fld_val));
  }
  else if (range_ndx == 1) {
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_TRAFFIC_CLASS_L_4_RANGE_CFGr, SOC_CORE_ALL, 0, TC_L_4_RANGE_OUTSIDE_1f,  fld_val));
  }
  else {
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_TRAFFIC_CLASS_L_4_RANGE_CFGr, SOC_CORE_ALL, 0, TC_L_4_RANGE_OUTSIDE_2f,  fld_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_l4_port_range_info_set_unsafe()", range_ndx, 0);
}

uint32
  arad_pp_llp_cos_l4_port_range_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, ARAD_PP_LLP_COS_RANGE_NDX_MAX, ARAD_PP_LLP_COS_RANGE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO, range_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_l4_port_range_info_set_verify()", range_ndx, 0);
}

uint32
  arad_pp_llp_cos_l4_port_range_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, ARAD_PP_LLP_COS_RANGE_NDX_MAX, ARAD_PP_LLP_COS_RANGE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_l4_port_range_info_get_verify()", range_ndx, 0);
}


uint32
  arad_pp_llp_cos_l4_port_range_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    reg_val,
    fld_val,
    res;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_clear(range_info);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1360, exit, READ_IHP_TRAFFIC_CLASS_L_4_RANGEr(unit, REG_PORT_ANY, range_ndx, &reg_val));

  fld_val = soc_reg_field_get(unit, IHP_TRAFFIC_CLASS_L_4_RANGEr, reg_val, TC_L_4_RANGE_N_MINf);
  range_info->start = (uint16)fld_val;

  fld_val = soc_reg_field_get(unit, IHP_TRAFFIC_CLASS_L_4_RANGEr, reg_val, TC_L_4_RANGE_N_MAXf);
  range_info->end = (uint16)fld_val;

  if (range_ndx == 0) {
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_TRAFFIC_CLASS_L_4_RANGE_CFGr, SOC_CORE_ALL, 0, TC_L_4_RANGE_OUTSIDE_0f, &fld_val));
  }
  else if (range_ndx == 1) {
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_TRAFFIC_CLASS_L_4_RANGE_CFGr, SOC_CORE_ALL, 0, TC_L_4_RANGE_OUTSIDE_1f, &fld_val));
  }
  else {
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_TRAFFIC_CLASS_L_4_RANGE_CFGr, SOC_CORE_ALL, 0, TC_L_4_RANGE_OUTSIDE_2f, &fld_val));
  }
  range_info->in_range = SOC_SAND_NUM2BOOL_INVERSE(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_l4_port_range_info_set_unsafe()", range_ndx, 0);
}


uint32
  arad_pp_llp_cos_l4_port_range_to_tc_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK,
    offset,
    reg_offset,
    bit_offset,
    reg_val[1],
    fld_val;
  uint8
    is_tcp,
    is_udp;
  soc_reg_above_64_val_t
	  reg_above64_val;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tc_info);

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

  is_tcp = (uint8)(l4_prtcl_type_ndx != SOC_SAND_PP_L4_PRTCL_TYPE_UDP);
  is_udp = (uint8)(l4_prtcl_type_ndx != SOC_SAND_PP_L4_PRTCL_TYPE_TCP);

  if (is_tcp)
  {
    offset = ARAD_PP_LLP_COS_L4_PORT_RANGES_2_TC_TABLE_OFFSET(
      is_tcp,
      src_port_match_range_ndx,
      dest_port_match_range_ndx);

	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "tcp\n")));

    
    reg_offset = offset / 8;
    bit_offset = (offset % 8) * 4;
    fld_val = (((SOC_SAND_BOOL2NUM(tc_info->valid)) << 3) + tc_info->tc);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1460, exit, READ_IHP_L_4_PORT_RANGES_2_TC_TABLEr(unit, REG_PORT_ANY, reg_above64_val));

	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "reg_above64_val[0]: 0x08%x\n"), reg_above64_val[0]));
    LOG_INFO(BSL_LS_SOC_LLP,
             (BSL_META_U(unit,
                         "reg_above64_val[1]: 0x08%x\n"), reg_above64_val[1]));
    LOG_INFO(BSL_LS_SOC_LLP,
             (BSL_META_U(unit,
                         "reg_above64_val[2]: 0x08%x\n"), reg_above64_val[2]));
    LOG_INFO(BSL_LS_SOC_LLP,
             (BSL_META_U(unit,
                         "reg_above64_val[3]: 0x08%x\n"), reg_above64_val[3]));

	*reg_val = reg_above64_val[reg_offset];

	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "reg_offset: %u\n"), reg_offset));
	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "reg_val: %u\n"), *reg_val));

    res = soc_sand_bitstream_set_field(
      reg_val,
      bit_offset,
      4 ,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "bit_offset: %u\n"), bit_offset));
	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "fld_val: %u\n"), fld_val));
	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "reg_val: %u\n"), *reg_val));

	reg_above64_val[reg_offset] = *reg_val;

	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "reg_above64_val[0]: 0x08%x\n"), reg_above64_val[0]));
    LOG_INFO(BSL_LS_SOC_LLP,
             (BSL_META_U(unit,
                         "reg_above64_val[1]: 0x08%x\n"), reg_above64_val[1]));
    LOG_INFO(BSL_LS_SOC_LLP,
             (BSL_META_U(unit,
                         "reg_above64_val[2]: 0x08%x\n"), reg_above64_val[2]));
    LOG_INFO(BSL_LS_SOC_LLP,
             (BSL_META_U(unit,
                         "reg_above64_val[3]: 0x08%x\n"), reg_above64_val[3]));

	SOC_SAND_SOC_IF_ERROR_RETURN(res, 1470, exit, WRITE_IHP_L_4_PORT_RANGES_2_TC_TABLEr(unit, REG_PORT_ANY, reg_above64_val));

	SOC_SAND_SOC_IF_ERROR_RETURN(res, 1480, exit, READ_IHP_L_4_PORT_RANGES_2_TC_TABLEr(unit, REG_PORT_ANY, reg_above64_val));

	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "reg_above64_val[0]: 0x08%x\n"), reg_above64_val[0]));
    LOG_INFO(BSL_LS_SOC_LLP,
             (BSL_META_U(unit,
                         "reg_above64_val[1]: 0x08%x\n"), reg_above64_val[1]));
    LOG_INFO(BSL_LS_SOC_LLP,
             (BSL_META_U(unit,
                         "reg_above64_val[2]: 0x08%x\n"), reg_above64_val[2]));
    LOG_INFO(BSL_LS_SOC_LLP,
             (BSL_META_U(unit,
                         "reg_above64_val[3]: 0x08%x\n"), reg_above64_val[3]));

  }

  if (is_udp)
  {
    offset = ARAD_PP_LLP_COS_L4_PORT_RANGES_2_TC_TABLE_OFFSET(
      (SOC_SAND_BOOL2NUM(!is_udp)),
      src_port_match_range_ndx,
      dest_port_match_range_ndx);

	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "udp\n")));

    
    reg_offset = offset / 8;
    bit_offset = offset % 8 * 4;
    fld_val = (((SOC_SAND_BOOL2NUM(tc_info->valid)) << 3) + tc_info->tc);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1490, exit, READ_IHP_L_4_PORT_RANGES_2_TC_TABLEr(unit, REG_PORT_ANY, reg_above64_val));
	*reg_val = reg_above64_val[reg_offset];

			LOG_INFO(BSL_LS_SOC_LLP,
                                 (BSL_META_U(unit,
                                             "reg_above64_val[0]: 0x08%x\n"), reg_above64_val[0]));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_above64_val[1]: 0x08%x\n"), reg_above64_val[1]));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_above64_val[2]: 0x08%x\n"), reg_above64_val[2]));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_above64_val[3]: 0x08%x\n"), reg_above64_val[3]));

  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_offset: %u\n"), reg_offset));
	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "reg_val: %u\n"), *reg_val));

    res = soc_sand_bitstream_set_field(
      reg_val,
      bit_offset,
      4 ,
      fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "bit_offset: %u\n"), bit_offset));
	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "fld_val: %u\n"), fld_val));
	LOG_INFO(BSL_LS_SOC_LLP,
                 (BSL_META_U(unit,
                             "reg_val: %u\n"), *reg_val));

    reg_above64_val[reg_offset] = *reg_val;

			LOG_INFO(BSL_LS_SOC_LLP,
                                 (BSL_META_U(unit,
                                             "reg_above64_val[0]: 0x08%x\n"), reg_above64_val[0]));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_above64_val[1]: 0x08%x\n"), reg_above64_val[1]));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_above64_val[2]: 0x08%x\n"), reg_above64_val[2]));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_above64_val[3]: 0x08%x\n"), reg_above64_val[3]));

	SOC_SAND_SOC_IF_ERROR_RETURN(res, 1500, exit, WRITE_IHP_L_4_PORT_RANGES_2_TC_TABLEr(unit, REG_PORT_ANY, reg_above64_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_l4_port_range_to_tc_info_set_unsafe()", 0, src_port_match_range_ndx);
}

uint32
  arad_pp_llp_cos_l4_port_range_to_tc_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(l4_prtcl_type_ndx, ARAD_PP_LLP_COS_L4_PRTCL_TYPE_NDX_MAX, ARAD_PP_LLP_COS_L4_PRTCL_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(src_port_match_range_ndx, ARAD_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_MAX, ARAD_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dest_port_match_range_ndx, ARAD_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_MAX, ARAD_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR, 30, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_TC_INFO, tc_info, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_l4_port_range_to_tc_info_set_verify()", 0, src_port_match_range_ndx);
}

uint32
  arad_pp_llp_cos_l4_port_range_to_tc_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_VERIFY);

  if ((l4_prtcl_type_ndx != SOC_SAND_PP_L4_PRTCL_TYPE_TCP) &&
      (l4_prtcl_type_ndx != SOC_SAND_PP_L4_PRTCL_TYPE_UDP))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_COS_L4_PRTCL_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(src_port_match_range_ndx, ARAD_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_MAX, ARAD_PP_LLP_COS_SRC_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dest_port_match_range_ndx, ARAD_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_MAX, ARAD_PP_LLP_COS_DEST_PORT_MATCH_RANGE_NDX_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_l4_port_range_to_tc_info_get_verify()", 0, src_port_match_range_ndx);
}


uint32
  arad_pp_llp_cos_l4_port_range_to_tc_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK,
    offset,
    reg_offset,
    bit_offset,
    reg_val,
    fld_val;
  uint8
    is_tcp;
  soc_reg_above_64_val_t
	reg_above64_val;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tc_info);

  SOC_PPC_LLP_COS_TC_INFO_clear(tc_info);
  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

  is_tcp = (uint8)(l4_prtcl_type_ndx == SOC_SAND_PP_L4_PRTCL_TYPE_TCP);

  offset = ARAD_PP_LLP_COS_L4_PORT_RANGES_2_TC_TABLE_OFFSET(
    is_tcp,
    src_port_match_range_ndx,
    dest_port_match_range_ndx);

  
  reg_offset = offset / 8;
  bit_offset = offset % 8 * 4;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1600, exit, READ_IHP_L_4_PORT_RANGES_2_TC_TABLEr(unit, REG_PORT_ANY, reg_above64_val));

  		LOG_INFO(BSL_LS_SOC_LLP,
                         (BSL_META_U(unit,
                                     "reg_above64_val[0]: 0x08%x\n"), reg_above64_val[0]));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_above64_val[1]: 0x08%x\n"), reg_above64_val[1]));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_above64_val[2]: 0x08%x\n"), reg_above64_val[2]));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_above64_val[3]: 0x08%x\n"), reg_above64_val[3]));

  reg_val = reg_above64_val[reg_offset];

  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_offset: %u\n"), reg_offset));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "reg_val: %u\n"), reg_val));

  res = soc_sand_bitstream_get_field(
    &reg_val,
    bit_offset,
    4 ,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "bit_offset: %u\n"), bit_offset));
  LOG_INFO(BSL_LS_SOC_LLP,
           (BSL_META_U(unit,
                       "fld_val: %u\n"), fld_val));

  tc_info->valid = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val, 3));
  tc_info->tc = (SOC_SAND_PP_TC)SOC_SAND_GET_BITS_RANGE(fld_val, 2, 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_l4_port_range_to_tc_info_get_unsafe()", 0, src_port_match_range_ndx);
}


uint32
  arad_pp_llp_cos_ipv4_subnet_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  res = arad_pp_ihp_subnet_classify_tbl_get_unsafe(
    unit,
    entry_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (subnet_based_info->tc_is_valid)
  {
    tbl_data.ipv4_subnet = subnet->ip_address;
    tbl_data.ipv4_subnet_mask = (32 - subnet->prefix_len);
  }
  tbl_data.ipv4_subnet_tc = subnet_based_info->tc;
  tbl_data.ipv4_subnet_tc_valid = SOC_SAND_BOOL2NUM(subnet_based_info->tc_is_valid);

  res = arad_pp_ihp_subnet_classify_tbl_set_unsafe(
    unit,
    entry_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_ipv4_subnet_based_set_unsafe()", entry_ndx, 0);
}

uint32
  arad_pp_llp_cos_ipv4_subnet_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, ARAD_PP_LLP_COS_IPV4_SUBNET_NDX_MAX, ARAD_PP_LLP_COS_IPV4_SUBNET_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  res = soc_sand_pp_ipv4_subnet_verify(subnet);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_IPV4_SUBNET_INFO, subnet_based_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_ipv4_subnet_based_set_verify()", entry_ndx, 0);
}

uint32
  arad_pp_llp_cos_ipv4_subnet_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, ARAD_PP_LLP_COS_IPV4_SUBNET_NDX_MAX, ARAD_PP_LLP_COS_IPV4_SUBNET_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_ipv4_subnet_based_get_verify()", entry_ndx, 0);
}


uint32
  arad_pp_llp_cos_ipv4_subnet_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_OUT SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  soc_sand_SAND_PP_IPV4_SUBNET_clear(subnet);
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_clear(subnet_based_info);

  res = arad_pp_ihp_subnet_classify_tbl_get_unsafe(
    unit,
    entry_ndx,
    &tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  subnet->ip_address = tbl_data.ipv4_subnet;
  subnet->prefix_len = (uint8)(32 - tbl_data.ipv4_subnet_mask);
  subnet_based_info->tc = (SOC_SAND_PP_TC)tbl_data.ipv4_subnet_tc;
  subnet_based_info->tc_is_valid = SOC_SAND_NUM2BOOL(tbl_data.ipv4_subnet_tc_valid);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_ipv4_subnet_based_set_unsafe()", entry_ndx, 0);
}


uint32
  arad_pp_llp_cos_protocol_based_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_offset,
    ether_type_internal;
  uint8
    update_hw,
    alloc_ether_type,
    found;
  ARAD_PP_IHP_PORT_PROTOCOL_TBL_DATA
    ihp_port_protocol_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_PROTOCOL_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  tbl_offset = 0;
  *success = SOC_SAND_SUCCESS;
  alloc_ether_type = TRUE;
  update_hw = TRUE;

  res = arad_pp_l2_next_prtcl_type_find(
          unit,
          ether_type_ndx,
          &ether_type_internal,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (found)
  {
    
    tbl_offset =
      ARAD_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(ether_type_internal, profile_ndx);

    res = arad_pp_ihp_port_protocol_tbl_get_unsafe(
            unit,
            tbl_offset,
            &ihp_port_protocol_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ihp_port_protocol_tbl_data.traffic_class_valid)
    {
      if (prtcl_assign_info->tc_is_valid)
      {
        if (ihp_port_protocol_tbl_data.traffic_class == prtcl_assign_info->tc)
        {
          
          update_hw = FALSE;
          alloc_ether_type = FALSE;
        }
        else 
        {
          
          alloc_ether_type = FALSE;
        }
      }
      else 
      {
        
        res = arad_pp_l2_next_prtcl_type_deallocate(
                unit,
                ether_type_ndx
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        alloc_ether_type = FALSE;
      }
    }
  }
  
  if (alloc_ether_type)
  {
    
    res = arad_pp_l2_next_prtcl_type_allocate(
            unit,
            ether_type_ndx,
            &ether_type_internal,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (*success == SOC_SAND_SUCCESS)
  {
    if (!found)
    {
      tbl_offset =
        ARAD_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(ether_type_internal, profile_ndx);

      res = arad_pp_ihp_port_protocol_tbl_get_unsafe(
              unit,
              tbl_offset,
              &ihp_port_protocol_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

    if (update_hw)
    {
      ihp_port_protocol_tbl_data.traffic_class = prtcl_assign_info->tc;
      ihp_port_protocol_tbl_data.traffic_class_valid = SOC_SAND_BOOL2NUM(prtcl_assign_info->tc_is_valid);

      res = arad_pp_ihp_port_protocol_tbl_set_unsafe(
              unit,
              tbl_offset,
              &ihp_port_protocol_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_protocol_based_set_unsafe()", profile_ndx, ether_type_ndx);
}


uint32
  arad_pp_llp_cos_protocol_based_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_PROTOCOL_BASED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, ARAD_PP_LLP_COS_PROFILE_NDX_MAX, ARAD_PP_LLP_COS_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_PRTCL_INFO, prtcl_assign_info, 30, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_protocol_based_set_verify()", profile_ndx, ether_type_ndx);
}

uint32
  arad_pp_llp_cos_protocol_based_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_PROTOCOL_BASED_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, ARAD_PP_LLP_COS_PROFILE_NDX_MAX, ARAD_PP_LLP_COS_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_protocol_based_get_verify()", profile_ndx, ether_type_ndx);
}


uint32
  arad_pp_llp_cos_protocol_based_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_offset,
    ether_type_internal;
  uint8
    found;
  ARAD_PP_IHP_PORT_PROTOCOL_TBL_DATA
    ihp_port_protocol_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_PROTOCOL_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);

  SOC_PPC_LLP_COS_PRTCL_INFO_clear(prtcl_assign_info);

  res = arad_pp_l2_next_prtcl_type_find(
          unit,
          ether_type_ndx,
          &ether_type_internal,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (found)
  {
    tbl_offset =
      ARAD_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(ether_type_internal, profile_ndx);

    res = arad_pp_ihp_port_protocol_tbl_get_unsafe(
      unit,
      tbl_offset,
      &ihp_port_protocol_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    prtcl_assign_info->tc_is_valid =
      SOC_SAND_NUM2BOOL(ihp_port_protocol_tbl_data.traffic_class_valid);
    prtcl_assign_info->tc = (SOC_SAND_PP_TC)ihp_port_protocol_tbl_data.traffic_class;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_COS_ETHER_TYPE_NOT_EXIT_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_cos_protocol_based_set_unsafe()", profile_ndx, ether_type_ndx);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_cos_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_llp_cos;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_cos_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_llp_cos;
}

uint32
  SOC_PPC_LLP_COS_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_TC_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_TC_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_UP_USE_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_UP_USE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_UP_USE_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_up_to_tc_and_de_index, ARAD_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_MAX, ARAD_PP_LLP_COS_IN_UP_TO_TC_AND_DE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc_to_up_index, ARAD_PP_LLP_COS_TC_TO_UP_INDEX_MAX, ARAD_PP_LLP_COS_TC_TO_UP_INDEX_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->up_to_dp_index, ARAD_PP_LLP_COS_UP_TO_DP_INDEX_MAX, ARAD_PP_LLP_COS_UP_TO_DP_INDEX_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc_to_dp_index, ARAD_PP_LLP_COS_TC_TO_DP_INDEX_MAX, ARAD_PP_LLP_COS_TC_TO_DP_INDEX_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_PORT_L2_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L2_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_UP_USE, &(info->up_use), 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES, &(info->tbls_select), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_PORT_L2_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_PORT_L3_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ip_qos_to_tc_index, ARAD_PP_LLP_COS_IP_QOS_TO_TC_INDEX_MAX, ARAD_PP_LLP_COS_IP_QOS_TO_TC_INDEX_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_PORT_L3_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_PORT_L4_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_PORT_L4_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_PORT_L2_INFO, &(info->l2_info), 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_PORT_L3_INFO, &(info->l3_info), 11, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_COS_PORT_L4_INFO, &(info->l4_info), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->value1, ARAD_PP_LLP_COS_VALUE1_MAX, ARAD_PP_LLP_COS_VALUE1_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->value2, ARAD_PP_LLP_COS_VALUE2_MAX, ARAD_PP_LLP_COS_VALUE2_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_PRTCL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_PRTCL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_range, 1, SOC_SAND_PP_IN_RANGE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


