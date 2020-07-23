/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LLP_SA_AUTH_INCLUDED__

#define __SOC_PPC_API_LLP_SA_AUTH_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/TMC/tmc_api_general.h>






#define  SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_VIDS (0xFFFFFFFF)


#define  SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS (0xFFFFFFFF)








typedef enum
{
  
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_PORT = 0x1,
  
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_VID = 0x2,
  
  SOC_PPC_LLP_SA_MATCH_RULE_TYPE_ALL = (int)0xFFFFFFFF,
  
  SOC_PPC_NOF_LLP_SA_AUTH_MATCH_RULE_TYPES = 3
}SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 sa_auth_enable;

} SOC_PPC_LLP_SA_AUTH_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 tagged_only;
  
  SOC_SAND_PP_VLAN_ID expect_tag_vid;
  
  SOC_SAND_PP_SYS_PORT_ID expect_system_port;

} SOC_PPC_LLP_SA_AUTH_MAC_INFO;
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE rule_type;
  
  SOC_SAND_PP_SYS_PORT_ID port;
  
  SOC_SAND_PP_VLAN_ID vid;

} SOC_PPC_LLP_SA_AUTH_MATCH_RULE;










void
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_PORT_INFO *info
  );

void
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MAC_INFO *info
  );

void
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MATCH_RULE *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE enum_val
  );

void
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_PORT_INFO *info
  );

void
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MAC_INFO *info
  );

void
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

