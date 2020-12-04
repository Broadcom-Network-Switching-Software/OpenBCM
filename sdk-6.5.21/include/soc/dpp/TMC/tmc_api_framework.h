/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_FRAMEWORK_INCLUDED__

#define __SOC_TMC_API_FRAMEWORK_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>




#define SOC_TMC_FUNCS_OFFSET1                                                     (100|SOC_TMC_PROC_BITS)
#define SOC_TMC_TOPOLOGY_STATUS_CONNECTIVITY_PRINT                                (SOC_TMC_FUNCS_OFFSET1 +    1)









typedef enum SOC_TMC_ERR_LIST
{
  SOC_TMC_NO_ERR                     = SOC_SAND_OK,
  SOC_TMC_GEN_ERR                    = SOC_SAND_ERR,

  SOC_TMC_START_ERR_LIST_NUMBER      = SOC_SAND_TMC_START_ERR_NUMBER,
  SOC_TMC_REGS_FIELD_VAL_OUT_OF_RANGE_ERR,

  SOC_TMC_INPUT_OUT_OF_RANGE,
  SOC_TMC_CONFIG_ERR,

  SOC_TMC_LAST_ERR
} SOC_TMC_ERR;











#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
