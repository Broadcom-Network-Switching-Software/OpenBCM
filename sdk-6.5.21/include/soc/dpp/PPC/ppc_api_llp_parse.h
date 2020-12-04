/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LLP_PARSE_INCLUDED__

#define __SOC_PPC_API_LLP_PARSE_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>







#define SOC_PPC_TPID_PROFILE_NDX_MIN                             (0)
#define SOC_PPC_TPID_PROFILE_NDX_MAX                             (7)
#define SOC_PPC_TPID_PROFILE_ID_MIN                              (0)
#define SOC_PPC_TPID_PROFILE_ID_MAX                              (3)


#define  SOC_PPC_LLP_PARSE_NOF_TPID_VALS (5)


#define SOC_PPC_LLP_PARSE_FLAGS_OUTER_C   (1)
#define SOC_PPC_LLP_PARSE_FLAGS_INNER_C   (2)









typedef enum
{
  
  SOC_PPC_LLP_PARSE_TPID_INDEX_NONE = 0,
  
  SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1 = 1,
  
  SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2 = 2,
  
  SOC_PPC_LLP_PARSE_TPID_INDEX_ISID_TPID = 3,
  
  SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS = 4
}SOC_PPC_LLP_PARSE_TPID_INDEX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_TPID tpid_vals[SOC_PPC_LLP_PARSE_NOF_TPID_VALS];

} SOC_PPC_LLP_PARSE_TPID_VALUES;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 index;
  
  SOC_SAND_PP_VLAN_TAG_TYPE type;

} SOC_PPC_LLP_PARSE_TPID_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LLP_PARSE_TPID_INFO tpid1;
  
  SOC_PPC_LLP_PARSE_TPID_INFO tpid2;

} SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LLP_PARSE_TPID_INDEX outer_tpid;
  
  SOC_PPC_LLP_PARSE_TPID_INDEX inner_tpid;
  
  uint8 is_outer_prio;

} SOC_PPC_LLP_PARSE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR


   
   uint32 flags;
  
  SOC_PPC_TRAP_CODE action_trap_code;
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT tag_format;
  
  SOC_SAND_PP_VLAN_TAG_TYPE priority_tag_type;
  
  uint32 dlft_edit_command_id;
  
  uint32 dflt_edit_pcp_profile;
  
  uint8 initial_c_tag;

} SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO;










void
  SOC_PPC_LLP_PARSE_TPID_VALUES_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_VALUES *info
  );

void
  SOC_PPC_LLP_PARSE_TPID_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_INFO *info
  );

void
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO *info
  );

void
  SOC_PPC_LLP_PARSE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_INFO *info
  );

void
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_PARSE_TPID_INDEX_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_INDEX enum_val
  );

void
  SOC_PPC_LLP_PARSE_TPID_VALUES_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_VALUES *info
  );

void
  SOC_PPC_LLP_PARSE_TPID_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_INFO *info
  );

void
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO *info
  );

void
  SOC_PPC_LLP_PARSE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO *info
  );

void
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

