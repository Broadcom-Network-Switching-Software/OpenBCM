/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LLP_COS_INCLUDED__

#define __SOC_PPC_API_LLP_COS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>






#define  SOC_PPC_LLP_COS_L4_RANGE_NO_MATCH (0xFFFFFFFF)









typedef enum
{
  
  SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC = 0,
  
  SOC_PPC_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID = 1,
  
  SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID = 2,
  
  SOC_PPC_LLP_COS_MAPPING_TABLE_TC_TO_DP = 3,
  
  SOC_PPC_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP = 4,
  
  SOC_PPC_LLP_COS_MAPPING_TABLE_TC_TO_UP = 5,
  
  SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP = 6,
  
  SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP = 7,
  
  SOC_PPC_NOF_LLP_COS_MAPPING_TABLES = 8
}SOC_PPC_LLP_COS_MAPPING_TABLE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_DP default_dp;

} SOC_PPC_LLP_COS_GLBL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 tc;
  
  uint8 valid;

} SOC_PPC_LLP_COS_TC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 use_for_tc;
  
  uint8 use_for_out_up;

} SOC_PPC_LLP_COS_UP_USE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 in_up_to_tc_and_de_index;
  
  uint32 tc_to_up_index;
  
  uint32 up_to_dp_index;
  
  uint32 tc_to_dp_index;

} SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LLP_COS_UP_USE up_use;
  
  uint8 use_l2_protocol;
  
  uint8 ignore_pkt_pcp_for_tc;
  
  uint8 use_dei;
  
  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES tbls_select;

} SOC_PPC_LLP_COS_PORT_L2_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 use_ip_qos;
  
  uint32 ip_qos_to_tc_index;
  
  uint8 use_ip_subnet;
  
  uint8 use_mpls_term_lbl;

} SOC_PPC_LLP_COS_PORT_L3_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 use_l4_prtcl;

} SOC_PPC_LLP_COS_PORT_L4_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LLP_COS_PORT_L2_INFO l2_info;
  
  SOC_PPC_LLP_COS_PORT_L3_INFO l3_info;
  
  SOC_PPC_LLP_COS_PORT_L4_INFO l4_info;
  
  SOC_SAND_PP_TC default_tc;

} SOC_PPC_LLP_COS_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 value1;
  
  uint32 value2;
  
  uint8 valid;

} SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_TC tc;
  
  uint8 tc_is_valid;

} SOC_PPC_LLP_COS_IPV4_SUBNET_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_TC tc;
  
  uint8 tc_is_valid;

} SOC_PPC_LLP_COS_PRTCL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint16 start;
  
  uint16 end;
  
  uint8 in_range;

} SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO;










void
  SOC_PPC_LLP_COS_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_GLBL_INFO *info
  );

void
  SOC_PPC_LLP_COS_TC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_TC_INFO *info
  );

void
  SOC_PPC_LLP_COS_UP_USE_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_UP_USE *info
  );

void
  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES *info
  );

void
  SOC_PPC_LLP_COS_PORT_L2_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_L2_INFO *info
  );

void
  SOC_PPC_LLP_COS_PORT_L3_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_L3_INFO *info
  );

void
  SOC_PPC_LLP_COS_PORT_L4_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_L4_INFO *info
  );

void
  SOC_PPC_LLP_COS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_INFO *info
  );

void
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  );

void
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_IPV4_SUBNET_INFO *info
  );

void
  SOC_PPC_LLP_COS_PRTCL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PRTCL_INFO *info
  );

void
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_COS_MAPPING_TABLE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE enum_val
  );

void
  SOC_PPC_LLP_COS_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO *info
  );

void
  SOC_PPC_LLP_COS_TC_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO *info
  );

void
  SOC_PPC_LLP_COS_UP_USE_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_UP_USE *info
  );

void
  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES *info
  );

void
  SOC_PPC_LLP_COS_PORT_L2_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L2_INFO *info
  );

void
  SOC_PPC_LLP_COS_PORT_L3_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L3_INFO *info
  );

void
  SOC_PPC_LLP_COS_PORT_L4_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L4_INFO *info
  );

void
  SOC_PPC_LLP_COS_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO *info
  );

void
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  );

void
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO *info
  );

void
  SOC_PPC_LLP_COS_PRTCL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO *info
  );

void
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
