/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LIF_TABLE_INCLUDED__

#define __SOC_PPC_API_LIF_TABLE_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_lif.h>
#include <soc/dpp/PPC/ppc_api_rif.h>












typedef enum
{
  
  SOC_PPC_LIF_ENTRY_TYPE_EMPTY = 0x1,
  
  SOC_PPC_LIF_ENTRY_TYPE_AC = 0x2,
  
  SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP = 0x4,
  
  SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP = 0x8,
  
  SOC_PPC_LIF_ENTRY_TYPE_PWE = 0x10,
  
  SOC_PPC_LIF_ENTRY_TYPE_ISID = 0x20,
  
  SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF = 0x40,
  
  SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF = 0x80,
  
  SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK = 0x100,
  
  SOC_PPC_LIF_ENTRY_TYPE_EXTENDER = 0x200,
  
  SOC_PPC_LIF_ENTRY_TYPE_ALL = (int)0xFFFFFFFF,
  
  SOC_PPC_NOF_LIF_ENTRY_TYPES = 10
}SOC_PPC_LIF_ENTRY_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_L2_LIF_AC_INFO ac;
  
  SOC_PPC_L2_LIF_PWE_INFO pwe;
  
  SOC_PPC_L2_LIF_ISID_INFO isid;
  
  SOC_PPC_RIF_INFO rif;
  
  SOC_PPC_RIF_IP_TERM_INFO ip_term_info;
  
  SOC_PPC_MPLS_TERM_INFO mpls_term_info;
  
  SOC_PPC_L2_LIF_TRILL_INFO trill;
  
  SOC_PPC_L2_LIF_EXTENDER_INFO extender;

} SOC_PPC_LIF_ENTRY_PER_TYPE_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LIF_ENTRY_TYPE type;
  
  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO value;
  
  SOC_PPC_LIF_ID index;

} SOC_PPC_LIF_ENTRY_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 entries_type_bm;
  
  uint8 accessed_only;

} SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 accessed;

} SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO;










void
  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_PER_TYPE_INFO *info
  );

void
  SOC_PPC_LIF_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO *info
  );

void
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE *info
  );

void
  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  );


#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LIF_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE enum_val
  );

void
  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO *info,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE  type
  );

void
  SOC_PPC_LIF_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO *info
  );

void
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE *info
  );

void
  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  );


#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
