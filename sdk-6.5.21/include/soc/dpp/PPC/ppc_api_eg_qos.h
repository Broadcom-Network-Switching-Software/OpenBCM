/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_EG_QOS_INCLUDED__

#define __SOC_PPC_API_EG_QOS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>













typedef enum
{
  
  SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4 = 0,
  
  SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6 = 1,
  
  SOC_PPC_NOF_EG_QOS_UNIFORM_PHP_TYPES = 2
}SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 in_dscp_exp;
  
  SOC_SAND_PP_DP dp;
   
  uint32 remark_profile;

} SOC_PPC_EG_QOS_MAP_KEY;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 in_dscp_exp;
   
  uint32 remark_profile;
  
  SOC_PPC_PKT_HDR_TYPE pkt_hdr_type;
  
  SOC_SAND_PP_DP dp;

} SOC_PPC_EG_ENCAP_QOS_MAP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_IPV4_TOS ipv4_tos;
  
  SOC_SAND_PP_IPV6_TC ipv6_tc;
  
  SOC_SAND_PP_MPLS_EXP mpls_exp;

} SOC_PPC_EG_QOS_PARAMS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_IPV4_TOS ip_dscp;
  
  SOC_SAND_PP_MPLS_EXP mpls_exp;

} SOC_PPC_EG_ENCAP_QOS_PARAMS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 exp_map_profile;
  
  SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE php_type;
  
  SOC_SAND_PP_MPLS_EXP exp;

} SOC_PPC_EG_QOS_PHP_REMARK_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 exp_map_profile;
  
#ifdef BCM_88660
  
  uint32 marking_profile;
#endif 

} SOC_PPC_EG_QOS_PORT_INFO;

#ifdef BCM_88660
typedef struct 
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_DP resolved_dp_ndx;
  
  SOC_SAND_PP_TC tc_ndx;
  
  uint32 in_lif_profile;
  
  uint32 marking_profile;
  
  uint32 dp_map_disabled;
} SOC_PPC_EG_QOS_MARKING_KEY;

typedef struct 
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_IPV4_TOS ip_dscp;
  
  SOC_SAND_PP_MPLS_EXP mpls_exp;
} SOC_PPC_EG_QOS_MARKING_PARAMS;


typedef struct 
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 resolved_dp_bitmap;
  
  uint32 in_lif_profile_bitmap;
  
  uint32 resolved_in_lif_profile_bitmap;
  
  uint32 dp_map_mode;
} SOC_PPC_EG_QOS_GLOBAL_INFO;

#endif 









void
  SOC_PPC_EG_QOS_MAP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_MAP_KEY *info
  );

void
  SOC_PPC_EG_ENCAP_QOS_MAP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_QOS_MAP_KEY *info
  );

void
  SOC_PPC_EG_QOS_PARAMS_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_PARAMS *info
  );

void
  SOC_PPC_EG_ENCAP_QOS_PARAMS_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_QOS_PARAMS *info
  );

void
  SOC_PPC_EG_QOS_PHP_REMARK_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_PHP_REMARK_KEY *info
  );

void
  SOC_PPC_EG_QOS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_PORT_INFO *info
  );

#ifdef BCM_88660
void
  SOC_PPC_EG_QOS_MARKING_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_MARKING_KEY *info
  );

void
  SOC_PPC_EG_QOS_MARKING_PARAMS_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_MARKING_PARAMS *info
  );

void
  SOC_PPC_EG_QOS_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_GLOBAL_INFO *info
  );
#endif 

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE enum_val
  );

void
  SOC_PPC_EG_QOS_MAP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY *info
  );

void
  SOC_PPC_EG_ENCAP_QOS_MAP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY *info
  );

void
  SOC_PPC_EG_QOS_PARAMS_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PARAMS *info
  );

void
  SOC_PPC_EG_ENCAP_QOS_PARAMS_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_PARAMS *info
  );

void
  SOC_PPC_EG_QOS_PHP_REMARK_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY *info
  );

void
  SOC_PPC_EG_QOS_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PORT_INFO *info
  );

#ifdef BCM_88660
void
  SOC_PPC_EG_QOS_MARKING_KEY_print(
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_KEY *info
  );

void
  SOC_PPC_EG_QOS_MARKING_PARAMS_print(
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_PARAMS *info
  );

void 
  SOC_PPC_EG_QOS_GLOBAL_INFO_print(
    SOC_SAND_IN SOC_PPC_EG_QOS_GLOBAL_INFO *info
  );
#endif 

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

