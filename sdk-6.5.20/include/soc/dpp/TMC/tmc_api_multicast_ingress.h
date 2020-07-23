/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_MULTICAST_INGRESS_INCLUDED__

#define __SOC_TMC_API_MULTICAST_INGRESS_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>














typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_TR_CLS map[SOC_TMC_NOF_TRAFFIC_CLASSES];
}SOC_TMC_MULT_ING_TR_CLS_MAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_DEST_INFO destination;
  
  uint32 cud;
}SOC_TMC_MULT_ING_ENTRY;











void
  SOC_TMC_MULT_ING_TR_CLS_MAP_clear(
    SOC_SAND_OUT SOC_TMC_MULT_ING_TR_CLS_MAP *info
  );

void
  SOC_TMC_MULT_ING_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_MULT_ING_ENTRY *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_MULT_ING_TR_CLS_MAP_print(
    SOC_SAND_IN SOC_TMC_MULT_ING_TR_CLS_MAP *info
  );

void
  SOC_TMC_MULT_ING_ENTRY_print(
    SOC_SAND_IN SOC_TMC_MULT_ING_ENTRY *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
