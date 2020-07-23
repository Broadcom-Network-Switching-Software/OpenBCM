/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LIF_COS_INCLUDED__

#define __SOC_PPC_API_LIF_COS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>













typedef enum
{
  
  SOC_PPC_LIF_COS_AC_PROFILE_TYPE_NONE = 0,
  
  SOC_PPC_LIF_COS_AC_PROFILE_TYPE_FORCE_ALWAYS = 1,
  
  SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_TAG_ELSE_FORCE = 2,
  
  SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_IP_ELSE_FORCE = 3,
  
  SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_TAG_ELSE_NONE = 4,
  
  SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_IP_ELSE_NONE = 5,
  
  SOC_PPC_NOF_LIF_COS_AC_PROFILE_TYPES = 6
}SOC_PPC_LIF_COS_AC_PROFILE_TYPE;

typedef enum
{
  
  SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_NONE = 0,
  
  SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_FORCE_ALWAYS = 1,
  
  SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_MAP = 2,
  
  SOC_PPC_NOF_LIF_COS_PWE_PROFILE_TYPES = 3
}SOC_PPC_LIF_COS_PWE_PROFILE_TYPE;

typedef enum
{
  
  SOC_PPC_LIF_COS_OPCODE_TYPE_L3 = 1,
  
  SOC_PPC_LIF_COS_OPCODE_TYPE_L2 = 2,
  
  SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP = 4,
  
  SOC_PPC_NOF_LIF_COS_OPCODE_TYPES = 8
}SOC_PPC_LIF_COS_OPCODE_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LIF_COS_AC_PROFILE_TYPE type;
  
  SOC_SAND_PP_TC tc;
  
  SOC_SAND_PP_DP dp;
  
  uint32 map_table;

} SOC_PPC_LIF_COS_AC_PROFILE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LIF_COS_PWE_PROFILE_TYPE type;
  
  SOC_SAND_PP_TC tc;
  
  SOC_SAND_PP_DP dp;
  
  uint32 map_table;

} SOC_PPC_LIF_COS_PWE_PROFILE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 map_when_ip;
  
  uint8 map_when_mpls;
  
  uint8 map_when_l2;
  
  uint8 map_from_tc_dp;
  
  SOC_SAND_PP_TC forced_tc;
  
  SOC_SAND_PP_DP forced_dp;
  
  uint8 use_layer2_pcp;

} SOC_PPC_LIF_COS_PROFILE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_TC tc;
  
  SOC_SAND_PP_DP dp;
  
  uint32 dscp;

} SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_IP_TYPE ip_type;
  
  SOC_SAND_PP_IPV4_TOS tos;
  
  SOC_SAND_PP_IPV4_TOS dscp_exp;
} SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MPLS_EXP in_exp;

} SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 outer_tpid;
  
  SOC_SAND_PP_PCP_UP incoming_up;
  
  SOC_SAND_PP_DEI_CFI incoming_dei;

} SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_TC tc;
  
  SOC_SAND_PP_DP dp;

} SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_packet_valid;
  
  uint8 is_qos_only;
  
  uint8 ac_offset;

} SOC_PPC_LIF_COS_OPCODE_ACTION_INFO;










void
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_AC_PROFILE_INFO *info
  );

void
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PWE_PROFILE_INFO *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_INFO *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  );

void
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LIF_COS_AC_PROFILE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_TYPE enum_val
  );

const char*
  SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_TYPE enum_val
  );

const char*
  SOC_PPC_LIF_COS_OPCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_TYPE enum_val
  );

void
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO *info
  );

void
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  );

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  );

void
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

