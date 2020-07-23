/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LLP_VID_ASSIGN_INCLUDED__

#define __SOC_PPC_API_LLP_VID_ASSIGN_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>













typedef enum
{
  
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_TAGGED = 0x4,
  
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_UNTAGGED = 0x8,
  
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_ALL = (int)0xFFFFFFFF,
  
  SOC_PPC_NOF_LLP_VID_ASSIGN_MATCH_RULE_TYPES = 3
}SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_VLAN_ID pvid;
  
  uint8 ignore_incoming_tag;
  
  uint8 enable_subnet_ip;
  
  uint8 enable_protocol;
  
  uint8 enable_sa_based;

} SOC_PPC_LLP_VID_ASSIGN_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_VLAN_ID vid;
  
  uint8 use_for_untagged;
  
  uint8 override_tagged;

} SOC_PPC_LLP_VID_ASSIGN_MAC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_VLAN_ID vid;
  
  uint8 vid_is_valid;

} SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_VLAN_ID vid;
  
  uint8 vid_is_valid;

} SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE rule_type;
  
  SOC_SAND_PP_VLAN_ID vid;

} SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE;










void
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PORT_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MAC_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE enum_val
  );

void
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO *info
  );

void
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

