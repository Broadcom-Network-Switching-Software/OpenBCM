/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_PMF_LOW_LEVEL_DB_INCLUDED__

#define __SOC_TMC_API_PMF_LOW_LEVEL_DB_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_pmf_low_level_ce.h>
#include <soc/dpp/TMC/tmc_api_tcam.h>
#include <soc/dpp/TMC/tmc_api_tcam_key.h>






#define  SOC_TMC_PMF_TCAM_NOF_BANKS (SOC_TMC_TCAM_NOF_BANKS)


#define  SOC_TMC_PMF_TCAM_NOF_LKP_RESULTS (4)










typedef enum
{
  
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_9_0 = 0,
  
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_19_10 = 1,
  
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_29_20 = 2,
  
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_39_30 = 3,
  
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_9_0 = 4,
  
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_19_10 = 5,
  
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_29_20 = 6,
  
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_39_30 = 7,
  
  SOC_TMC_NOF_PMF_DIRECT_TBL_KEY_SRCS = 8
}SOC_TMC_PMF_DIRECT_TBL_KEY_SRC;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 db_id;
  
  uint32 entry_id;

} SOC_TMC_PMF_TCAM_ENTRY_ID;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 id;
  
  uint32 cycle_ndx;

} SOC_TMC_PMF_LKP_PROFILE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 bank_id_enabled[SOC_TMC_PMF_TCAM_NOF_BANKS];

} SOC_TMC_PMF_TCAM_BANK_SELECTION;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PMF_TCAM_BANK_SELECTION bank_sel[SOC_TMC_PMF_TCAM_NOF_LKP_RESULTS];

} SOC_TMC_PMF_TCAM_RESULT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_valid;
  
  SOC_TMC_PMF_TCAM_KEY_SRC key_src;
  
  SOC_TMC_PMF_TCAM_RESULT_INFO bank_prio;

} SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO;




typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_TCAM_KEY key;
  
  uint32 priority;
  
  SOC_TMC_TCAM_OUTPUT output;

} SOC_TMC_PMF_TCAM_DATA;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 val;

} SOC_TMC_PMF_DIRECT_TBL_DATA;









void
  SOC_TMC_PMF_TCAM_ENTRY_ID_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_ENTRY_ID *info
  );

void
  SOC_TMC_PMF_LKP_PROFILE_clear(
    SOC_SAND_OUT SOC_TMC_PMF_LKP_PROFILE *info
  );

void
  SOC_TMC_PMF_TCAM_BANK_SELECTION_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_BANK_SELECTION *info
  );

void
  SOC_TMC_PMF_TCAM_DATA_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_DATA *info
  );


void
  SOC_TMC_PMF_TCAM_RESULT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_RESULT_INFO *info
  );

void
  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO *info
  );

void
  SOC_TMC_PMF_DIRECT_TBL_DATA_clear(
    SOC_SAND_OUT SOC_TMC_PMF_DIRECT_TBL_DATA *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC enum_val
  );

void
  SOC_TMC_PMF_TCAM_ENTRY_ID_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_ENTRY_ID *info
  );

void
  SOC_TMC_PMF_LKP_PROFILE_print(
    SOC_SAND_IN  SOC_TMC_PMF_LKP_PROFILE *info
  );

void
  SOC_TMC_PMF_TCAM_BANK_SELECTION_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_BANK_SELECTION *info
  );

void
  SOC_TMC_PMF_TCAM_RESULT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_RESULT_INFO *info
  );

void
  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO *info
  );


void
  SOC_TMC_PMF_TCAM_DATA_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_DATA *info
  );

void
  SOC_TMC_PMF_DIRECT_TBL_DATA_print(
    SOC_SAND_IN  SOC_TMC_PMF_DIRECT_TBL_DATA *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

