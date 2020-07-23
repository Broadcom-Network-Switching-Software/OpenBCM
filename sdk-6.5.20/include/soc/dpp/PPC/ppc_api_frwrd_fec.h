/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_FRWRD_FEC_INCLUDED__

#define __SOC_PPC_API_FRWRD_FEC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>

#include <soc/dpp/dpp_config_defs.h>





#define SOC_PPC_FEC_LB_CRC_17111      0x00
#define SOC_PPC_FEC_LB_CRC_10491      0x01
#define SOC_PPC_FEC_LB_CRC_155F5      0x02
#define SOC_PPC_FEC_LB_CRC_19715      0x03
#define SOC_PPC_FEC_LB_CRC_13965      0x04
#define SOC_PPC_FEC_LB_CRC_1698D      0x05
#define SOC_PPC_FEC_LB_CRC_1105D      0x06
#define SOC_PPC_FEC_LB_KEY            0x07    
#define SOC_PPC_FEC_LB_ROUND_ROBIN    0x08    
#define SOC_PPC_FEC_LB_2_CLOCK        0x09    
#define SOC_PPC_FEC_LB_CRC_10861      0x0A
#define SOC_PPC_FEC_LB_CRC_10285      0x0B
#define SOC_PPC_FEC_LB_CRC_101A1      0x0C
#define SOC_PPC_FEC_LB_CRC_12499      0x0D
#define SOC_PPC_FEC_LB_CRC_1F801      0x0E
#define SOC_PPC_FEC_LB_CRC_172E1      0x0F
#define SOC_PPC_FEC_LB_CRC_1EB21      0x10

#define SOC_PPC_FEC_LB_CRC_0x8003       0x11
#define SOC_PPC_FEC_LB_CRC_0x8011       0x12
#define SOC_PPC_FEC_LB_CRC_0x8423       0x13
#define SOC_PPC_FEC_LB_CRC_0x8101       0x14
#define SOC_PPC_FEC_LB_CRC_0x84a1       0x15
#define SOC_PPC_FEC_LB_CRC_0x9019       0x16


#define SOC_PPC_FEC_VRF_STATE_NOT_CREATED     0
#define SOC_PPC_FEC_VRF_STATE_REGULAR         1
#define SOC_PPC_FEC_VRF_STATE_DOUBLE_CAPACITY 2









typedef enum
{
  
  SOC_PPC_FEC_TYPE_DROP = 0,
  
  SOC_PPC_FEC_TYPE_IP_MC = 1,
  
  SOC_PPC_FEC_TYPE_IP_UC = 2,
  
  SOC_PPC_FEC_TYPE_TRILL_MC = 3,
  
  SOC_PPC_FEC_TYPE_TRILL_UC = 4,
  
  SOC_PPC_FEC_TYPE_BRIDGING_INTO_TUNNEL = 5,
  
  SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC = 6,
  
  SOC_PPC_FEC_TYPE_MPLS_LSR = 7,
  
  SOC_PPC_NOF_FEC_TYPES_PB = 8,
  
  SOC_PPC_FEC_TYPE_SIMPLE_DEST = SOC_PPC_NOF_FEC_TYPES_PB,
  
  SOC_PPC_FEC_TYPE_MPLS_LSR_EEI_OUTLIF,
  
  SOC_PPC_FEC_TYPE_ROUTING,
  
  SOC_PPC_FEC_TYPE_TUNNELING,
  
  SOC_PPC_FEC_TYPE_FORWARD,
  
  SOC_PPC_FEC_TYPE_TUNNELING_EEI_OUTLIF,
  
  SOC_PPC_FEC_TYPE_TUNNELING_EEI_MPLS_COMMAND,
  
  SOC_PPC_NOF_FEC_TYPES_ARAD

}SOC_PPC_FEC_TYPE;

typedef enum
{
  
  SOC_PPC_FRWRD_FEC_RPF_MODE_NONE = 0x0,
  
  SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE = 0x1,
  
  SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT = 0x2,  
  
  SOC_PPC_FRWRD_FEC_RPF_MODE_MC_EXPLICIT = 0x4,
  
  SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP = 0x8,
  
  SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP = 0x10,
  
  SOC_PPC_NOF_FRWRD_FEC_RPF_MODES = 6
}SOC_PPC_FRWRD_FEC_RPF_MODE;

typedef enum
{
  
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE = 0,
  
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE_FACILITY = 1,
  
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH = 2,
  
  SOC_PPC_NOF_FRWRD_FEC_PROTECT_TYPES = 3
}SOC_PPC_FRWRD_FEC_PROTECT_TYPE;

typedef enum
{
  
  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_NONE = 0,
  
  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ONE_FEC = 1,
  
  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ECMP = 2,
  
  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_PATH_PROTECT = 3,
  
  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_FACILITY_PROTECT = 4,
  
  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ECMP_PROTECTED = 5,
  
  SOC_PPC_NOF_FRWRD_FEC_ENTRY_USE_TYPES = 6
}SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE;

typedef enum
{
  
  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ALL = 0,
  
  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_APP_TYPE = 1,
  
  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ACCESSED = 2,
  
  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED = 3,
  
  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_FACILITY_PROTECTED = 4,
  
  SOC_PPC_NOF_FRWRD_FEC_MATCH_RULE_TYPES = 5
}SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 ecmp_sizes[SOC_DPP_DEFS_MAX(ECMP_MAX_SIZE)];
  
  uint32 ecmp_sizes_nof_entries;

} SOC_PPC_FRWRD_FEC_GLBL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_FEC_RPF_MODE rpf_mode;
  
  SOC_PPC_RIF_ID expected_in_rif;

} SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_AC_ID out_ac_id;
  
  SOC_PPC_RIF_ID out_rif;
  
  uint32 dist_tree_nick;

} SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FEC_TYPE type;
  
  SOC_SAND_PP_DESTINATION_ID dest;
  
  uint32 eep;
  
  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO app_info;
  
  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO rpf_info;
  
  SOC_PPC_ACTION_PROFILE trap_info;

} SOC_PPC_FRWRD_FEC_ENTRY_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 trap_if_accessed;
  
  uint8 accessed;

} SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 oam_instance_id;

} SOC_PPC_FRWRD_FEC_PROTECT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 size;
  
  uint32 base_fec_id;
  
  uint8 is_protected;
  
  uint32 rpf_fec_index;
#ifdef BCM_88660_A0
  
  uint8 is_stateful;
#endif 
} SOC_PPC_FRWRD_FEC_ECMP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE type;
  
  uint32 nof_entries;

} SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE type;
  
  uint32 value;

} SOC_PPC_FRWRD_FEC_MATCH_RULE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 use_port_id;
  
  uint16 seed;
  
  uint8 hash_func_id;
  
  uint8 hash_func_id_second_hier;
  
  uint8 key_shift;
  
  uint8 key_shift_second_hier;


} SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 nof_headers;
  
  uint8 start_from_bos;
  
  uint8 skip_bos;


} SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO;










void
  SOC_PPC_FRWRD_FEC_GLBL_INFO_clear(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_GLBL_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ECMP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_MATCH_RULE *info
  );

void
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_FEC_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FEC_TYPE enum_val
  );

const char*
  SOC_PPC_FEC_TYPE_to_string_short(
    SOC_SAND_IN  SOC_PPC_FEC_TYPE enum_val
  );

const char*
  SOC_PPC_FRWRD_FEC_RPF_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_RPF_MODE enum_val
  );

const char*
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_TYPE enum_val
  );

const char*
  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE enum_val
  );

const char*
  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE enum_val
  );

void
  SOC_PPC_FRWRD_FEC_GLBL_INFO_print(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ECMP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE *info
  );

void
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  );

void
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
