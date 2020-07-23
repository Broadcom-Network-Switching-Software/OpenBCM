/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_SSR_H_INCLUDED__

#define __SOC_TMC_API_SSR_H_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_api_ssr.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
















typedef enum{
  SOC_TMC_SW_DB_CFG_VERSION_INVALID = 0,
  SOC_TMC_SW_DB_CFG_VERSION_NUM_0 = 1,
  SOC_TMC_SW_DB_CFG_NOF_VERSIONS
}SOC_TMC_SW_DB_CFG_VERSION_NUM;














#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
