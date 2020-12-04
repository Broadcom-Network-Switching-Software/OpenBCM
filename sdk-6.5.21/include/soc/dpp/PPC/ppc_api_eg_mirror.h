/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_EG_MIRROR_INCLUDED__

#define __SOC_PPC_API_EG_MIRROR_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>













typedef struct
{
    uint32 mirror_command;  
    uint8 forward_strength; 
    uint8 mirror_strength;  
    uint8 forward_en;       
    uint8 mirror_en;        

} dpp_outbound_mirror_config_t;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable_mirror;
  
  uint32 dflt_profile;

} SOC_PPC_EG_MIRROR_PORT_DFLT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 outbound_mirror_enable;
  
  SOC_PPC_PORT outbound_port_ndx;  

} SOC_PPC_EG_MIRROR_PORT_INFO;










void
  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_MIRROR_PORT_DFLT_INFO *info
  );

void
  SOC_PPC_EG_MIRROR_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_MIRROR_PORT_INFO *info
  );
#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO *info
  );

void
  SOC_PPC_EG_MIRROR_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_MIRROR_PORT_INFO *info
  );


#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

