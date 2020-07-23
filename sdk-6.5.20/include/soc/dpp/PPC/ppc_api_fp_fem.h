/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_PPC_FP_FEM_INCLUDED__

#define __SOC_PPC_FP_FEM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_fp.h>













typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_for_entry;
  
  uint32 db_strength;
  
  uint32 db_id;
  
  uint32 entry_strength;
  
  uint32 entry_id;
  
  SOC_PPC_FP_ACTION_TYPE action_type[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
  
  uint8 is_base_positive[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];

} SOC_PPC_FP_FEM_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_cycle_fixed;
  
  uint8 cycle_id;

} SOC_PPC_FP_FEM_CYCLE;










void
  SOC_PPC_FP_FEM_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_FP_FEM_ENTRY *info
  );

void
  SOC_PPC_FP_FEM_CYCLE_clear(
    SOC_SAND_OUT SOC_PPC_FP_FEM_CYCLE *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_FP_FEM_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY *info
  );

void
  SOC_PPC_FP_FEM_CYCLE_print(
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

