/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_DEBUG_INCLUDED__

#define __SOC_TMC_API_DEBUG_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>














typedef enum
{
  
  SOC_TMC_DBG_FORCE_MODE_NONE=0,
  
  SOC_TMC_DBG_FORCE_MODE_LOCAL=1,
  
  SOC_TMC_DBG_FORCE_MODE_FABRIC=2,
  
  SOC_TMC_DBG_NOF_FORCE_MODES=3
}SOC_TMC_DBG_FORCE_MODE;

typedef enum
{
  
  SOC_TMC_DBG_FLUSH_MODE_DEQUEUE=0,
  
  SOC_TMC_DBG_FLUSH_MODE_DELETE=1,
  
  SOC_TMC_DBG_NOF_FLUSH_MODES=2
}SOC_TMC_DBG_FLUSH_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 first_queue;
  
  uint32 last_queue;
  
  uint32 rate;
}SOC_TMC_DBG_AUTOCREDIT_INFO;











void
  SOC_TMC_DBG_AUTOCREDIT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_DBG_AUTOCREDIT_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_DBG_FORCE_MODE_to_string(
    SOC_SAND_IN SOC_TMC_DBG_FORCE_MODE enum_val
  );

const char*
  SOC_TMC_DBG_FLUSH_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_DBG_FLUSH_MODE enum_val
  );

void
  SOC_TMC_DBG_AUTOCREDIT_INFO_print(
    SOC_SAND_IN SOC_TMC_DBG_AUTOCREDIT_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
