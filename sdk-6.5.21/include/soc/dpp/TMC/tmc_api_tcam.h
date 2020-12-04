/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_TCAM_INCLUDED__

#define __SOC_TMC_API_TCAM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>






#define  SOC_TMC_TCAM_NOF_BANKS (4)


#define  SOC_TMC_TCAM_NOF_CYCLES (2)

#ifndef SOC_TMC_TCAM_NL_88650_MAX_NOF_ENTRIES
#if defined(INCLUDE_KBP)

#define SOC_TMC_TCAM_NL_88650_MAX_NOF_ENTRIES (1 << 20)
#else  

#define SOC_TMC_TCAM_NL_88650_MAX_NOF_ENTRIES (2)
#endif 
#endif 









typedef enum
{
  
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_72_BITS = 0,
  
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_144_BITS = 1,
  
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_288_BITS = 2,
  
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_80_BITS = 0,
  
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_160_BITS = 1,
  
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_320_BITS = 2,

  
  SOC_TMC_NOF_TCAM_BANK_ENTRY_SIZES = 3

}SOC_TMC_TCAM_BANK_ENTRY_SIZE;

typedef enum
{
  
  SOC_TMC_TCAM_USER_FWDING_IPV4_MC = 0,
  
  SOC_TMC_TCAM_USER_FWDING_IPV6_UC = 1,
  
  SOC_TMC_TCAM_USER_FWDING_IPV6_MC = 2,
  
  SOC_TMC_TCAM_USER_FP = 3,
  
  SOC_TMC_TCAM_USER_EGRESS_ACL = 4,
  
  SOC_TMC_NOF_TCAM_USERS 
}SOC_TMC_TCAM_USER;

typedef enum
{
  
  SOC_TMC_TCAM_BANK_OWNER_PMF_0 = 0,

  
  SOC_TMC_TCAM_BANK_OWNER_PMF_1 = 1,

  
  SOC_TMC_TCAM_BANK_OWNER_FLP_TCAM = 2,

  
  SOC_TMC_TCAM_BANK_OWNER_FLP_TRAPS = 3,

  
  SOC_TMC_TCAM_BANK_OWNER_VT = 4,

  
  SOC_TMC_TCAM_BANK_OWNER_TT = 5,

  
  SOC_TMC_TCAM_BANK_OWNER_EGRESS_ACL = 6,  

  
  SOC_TMC_TCAM_NOF_BANK_OWNERS = 7 
}SOC_TMC_TCAM_BANK_OWNER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 val;
  
  uint32 nof_bits;

} SOC_TMC_TCAM_DB_PREFIX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 priority;
  
  SOC_TMC_TCAM_BANK_ENTRY_SIZE entry_size;
  
  uint32 prefix_length;

} SOC_TMC_TCAM_DB_USER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_present[SOC_TMC_TCAM_NOF_BANKS];
  
  SOC_TMC_TCAM_DB_PREFIX prefix[SOC_TMC_TCAM_NOF_BANKS];
  
  SOC_TMC_TCAM_DB_USER user;

} SOC_TMC_TCAM_DB_INFO;


typedef enum
{
  
  SOC_TMC_TCAM_ACTION_SIZE_FIRST_20_BITS = 0x1,
  
  SOC_TMC_TCAM_ACTION_SIZE_SECOND_20_BITS = 0x2,
  
  SOC_TMC_TCAM_ACTION_SIZE_THIRD_20_BITS = 0x4,
  
  SOC_TMC_TCAM_ACTION_SIZE_FORTH_20_BITS = 0x8,
  
  SOC_TMC_NOF_TCAM_ACTION_SIZES = 4
  
}SOC_TMC_TCAM_ACTION_SIZE;









void
  SOC_TMC_TCAM_DB_PREFIX_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_DB_PREFIX *info
  );

void
  SOC_TMC_TCAM_DB_USER_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_DB_USER *info
  );

void
  SOC_TMC_TCAM_DB_INFO_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_DB_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(
    SOC_SAND_IN  SOC_TMC_TCAM_BANK_ENTRY_SIZE enum_val
  );
const char*
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_ARAD_to_string(
    SOC_SAND_IN  SOC_TMC_TCAM_BANK_ENTRY_SIZE enum_val
  );
const char*
  SOC_TMC_TCAM_USER_to_string(
    SOC_SAND_IN  SOC_TMC_TCAM_USER enum_val
  );
const char*
  SOC_TMC_TCAM_BANK_OWNER_to_string(
    SOC_SAND_IN  SOC_TMC_TCAM_BANK_OWNER enum_val
  );

void
  SOC_TMC_TCAM_DB_PREFIX_print(
    SOC_SAND_IN  SOC_TMC_TCAM_DB_PREFIX *info
  );

void
  SOC_TMC_TCAM_DB_USER_print(
    SOC_SAND_IN  SOC_TMC_TCAM_DB_USER *info
  );

void
  SOC_TMC_TCAM_DB_INFO_print(
    SOC_SAND_IN  SOC_TMC_TCAM_DB_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

