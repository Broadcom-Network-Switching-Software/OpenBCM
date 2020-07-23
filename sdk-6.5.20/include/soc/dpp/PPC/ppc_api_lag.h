/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LAG_INCLUDED__

#define __SOC_PPC_API_LAG_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>






#define  SOC_PPC_LAG_MAX_ID (255)



#define  SOC_PPC_LAG_MEMBERS_MAX (256)

#define SOC_PPC_LAG_LB_CRC_0x14D         0x0
#define SOC_PPC_LAG_LB_CRC_0x1C3         0x1
#define SOC_PPC_LAG_LB_CRC_0x1CF         0x2
#define SOC_PPC_LAG_LB_KEY               0x3             
#define SOC_PPC_LAG_LB_ROUND_ROBIN       0x4   
#define SOC_PPC_LAG_LB_2_CLOCK           0x5    
#define SOC_PPC_LAG_LB_CRC_0x10861       0x6
#define SOC_PPC_LAG_LB_CRC_0x10285       0x7
#define SOC_PPC_LAG_LB_CRC_0x101A1       0x8
#define SOC_PPC_LAG_LB_CRC_0x12499       0x9
#define SOC_PPC_LAG_LB_CRC_0x1F801       0xA
#define SOC_PPC_LAG_LB_CRC_0x172E1       0xB
#define SOC_PPC_LAG_LB_CRC_0x1EB21       0xC
#define SOC_PPC_LAG_LB_CRC_0x13965       0xD
#define SOC_PPC_LAG_LB_CRC_0x1698D       0xE
#define SOC_PPC_LAG_LB_CRC_0x1105D       0xF

#define SOC_PPC_LAG_LB_CRC_0x8003       0x10
#define SOC_PPC_LAG_LB_CRC_0x8011       0x11
#define SOC_PPC_LAG_LB_CRC_0x8423       0x12
#define SOC_PPC_LAG_LB_CRC_0x8101       0x13
#define SOC_PPC_LAG_LB_CRC_0x84a1       0x14
#define SOC_PPC_LAG_LB_CRC_0x9019       0x15





#define SOC_PPC_LAG_MEMBER_INGRESS_DISABLE  0x1
#define SOC_PPC_LAG_MEMBER_EGRESS_DISABLE   0x2






typedef enum
{
  
  SOC_PPC_LAG_HASH_FRST_HDR_FARWARDING = 0,
  
  SOC_PPC_LAG_HASH_FRST_HDR_LAST_TERMINATED = 1,
  
  SOC_PPC_NOF_LAG_HASH_FRST_HDRS = 2
}SOC_PPC_LAG_HASH_FRST_HDR;

typedef enum
{
  
  SOC_PPC_LAG_LB_TYPE_HASH = 0,
  
  SOC_PPC_LAG_LB_TYPE_ROUND_ROBIN = 1,
  
  SOC_PPC_LAG_LB_TYPE_SMOOTH_DIVISION = 2,
  
  SOC_PPC_NOF_LAG_LB_TYPES = 3
}SOC_PPC_LAG_LB_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 use_port_id;
  
  uint32 seed;
  
  uint8 hash_func_id;
  
  uint8 key_shift;
  
  uint8 eli_search;

} SOC_PPC_LAG_HASH_GLOBAL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 nof_headers;
  
  SOC_PPC_LAG_HASH_FRST_HDR first_header_to_parse;
  
  uint8 start_from_bos;
  
  uint8 include_bos;

} SOC_PPC_LAG_HASH_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_HASH_MASKS mask;
  
  uint8 is_symmetric_key;
  
  uint8 expect_cw;

} SOC_PPC_HASH_MASK_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 sys_port;
  
  uint32 member_id;

  
  uint32 flags;

} SOC_PPC_LAG_MEMBER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 nof_entries;
  
  SOC_PPC_LAG_MEMBER members[SOC_PPC_LAG_MEMBERS_MAX];
  
  SOC_PPC_LAG_LB_TYPE lb_type;

  
  uint8 is_stateful;

} SOC_PPC_LAG_INFO;










void
  SOC_PPC_LAG_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_HASH_GLOBAL_INFO *info
  );

void
  SOC_PPC_LAG_HASH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_HASH_PORT_INFO *info
  );

void
  SOC_PPC_HASH_MASK_INFO_clear(
    SOC_SAND_OUT SOC_PPC_HASH_MASK_INFO *info
  );

void
  SOC_PPC_LAG_MEMBER_clear(
    SOC_SAND_OUT SOC_PPC_LAG_MEMBER *info
  );

void
  SOC_PPC_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LAG_HASH_FRST_HDR_to_string(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_FRST_HDR enum_val
  );

const char*
  SOC_PPC_LAG_LB_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LAG_LB_TYPE enum_val
  );

void
  SOC_PPC_LAG_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO *info
  );

void
  SOC_PPC_LAG_HASH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO *info
  );

void
  SOC_PPC_HASH_MASK_INFO_print(
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO *info
  );

void
  SOC_PPC_LAG_MEMBER_print(
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER *info
  );

void
  SOC_PPC_LAG_INFO_print(
    SOC_SAND_IN  SOC_PPC_LAG_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

