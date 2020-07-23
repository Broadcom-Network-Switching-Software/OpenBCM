/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LLP_MIRROR_INCLUDED__

#define __SOC_PPC_API_LLP_MIRROR_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>










#define DPP_MIRROR_ACTION_NDX_MIN 1
#define DPP_MIRROR_ACTION_NDX_MAX 15





typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 tagged_dflt;
  
  uint32 untagged_dflt;

  
  uint8 is_tagged_dflt;
  uint8 is_untagged_only;

} SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO;










void
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

