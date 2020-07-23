/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_FRWRD_BMACT_INCLUDED__

#define __SOC_PPC_API_FRWRD_BMACT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>






#define  SOC_PPC_BFID_EQUAL_TO_BVID (0)


#define  SOC_PPC_BFID_IS_0 (1)


#define  SOC_PPC_BMACT_ENTRY_TYPE_FRWRD      (0x1) 
#define  SOC_PPC_BMACT_ENTRY_TYPE_LEARN      (0x2) 
#define  SOC_PPC_BMACT_ENTRY_TYPE_MC_DEST    (0x4) 
#define  SOC_PPC_BMACT_ENTRY_TYPE_LAG        (0x8) 
#define  SOC_PPC_BMACT_ENTRY_TYPE_STATIC     (0x10)
 
 









typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 stp_topology_id;
  
  uint32 b_fid_profile;
  
  SOC_PPC_FRWRD_DECISION_INFO uknown_da_dest;
  
  uint32 default_forward_profile;

} SOC_PPC_BMACT_BVID_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_VLAN_ID first_vid;
  
  SOC_SAND_PP_VLAN_ID last_vid;

} SOC_PPC_BMACT_PBB_TE_VID_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_VLAN_ID b_vid;
  
  SOC_SAND_PP_MAC_ADDRESS b_mac_addr;
  
  SOC_PPC_PORT local_port_ndx;
  
  uint8 core;
  
  uint32 flags;
  
} SOC_PPC_BMACT_ENTRY_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 sys_port_id;
  
  uint32 mc_id;
  
  uint32 i_sid_domain;
  
  SOC_PPC_FEC_ID sa_learn_fec_id;
  
  uint8 drop_sa;
  
  uint8 is_dynamic;

  
  uint32 age_status;
} SOC_PPC_BMACT_ENTRY_INFO;










void
  SOC_PPC_BMACT_BVID_INFO_clear(
    SOC_SAND_OUT SOC_PPC_BMACT_BVID_INFO *info
  );

void
  SOC_PPC_BMACT_PBB_TE_VID_RANGE_clear(
    SOC_SAND_OUT SOC_PPC_BMACT_PBB_TE_VID_RANGE *info
  );

void
  SOC_PPC_BMACT_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_BMACT_ENTRY_KEY *info
  );

void
  SOC_PPC_BMACT_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_BMACT_ENTRY_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_BMACT_BVID_INFO_print(
    SOC_SAND_IN  SOC_PPC_BMACT_BVID_INFO *info
  );

void
  SOC_PPC_BMACT_PBB_TE_VID_RANGE_print(
    SOC_SAND_IN  SOC_PPC_BMACT_PBB_TE_VID_RANGE *info
  );

void
  SOC_PPC_BMACT_ENTRY_KEY_print(
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY *info
  );

void
  SOC_PPC_BMACT_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

