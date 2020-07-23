/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_PPC_API_FP_EGR_INCLUDED__

#define __SOC_PPC_API_FP_EGR_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>













typedef enum
{
  
  SOC_PPC_FP_EGR_MANAGE_TYPE_ADD = 0,
  
  SOC_PPC_FP_EGR_MANAGE_TYPE_RMV = 1,
  
  SOC_PPC_FP_EGR_MANAGE_TYPE_GET = 2,
  
  SOC_PPC_NOF_FP_EGR_MANAGE_TYPES = 3
}SOC_PPC_FP_EGR_MANAGE_TYPE;










#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_FP_EGR_MANAGE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FP_EGR_MANAGE_TYPE  enum_val
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

