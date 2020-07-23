/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_FRWRD_TRILL_INCLUDED__

#define __SOC_PPC_API_FRWRD_TRILL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_general.h>






#define  SOC_PPC_TRILL_MC_IGNORE_FIELD (0xFFFFFFFF)


#define  SOC_PPC_TRILL_ACCEPT_ALL_PORTS (0xffffffff)

#define DEFAULT_TRILL_ETHER_TYPE                                0x2109









typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 mask_ing_nickname;
  
  uint8 mask_adjacent_nickname;
  
  uint8 mask_fid;

} SOC_PPC_TRILL_MC_MASKED_FIELDS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 tree_nick;
  
  SOC_PPC_FID fid;

  
  SOC_PPC_FID outer_vid; 
  SOC_PPC_FID inner_vid; 

  
  uint32 ing_nick;
  
  uint32 adjacent_eep;

  
  uint8 esadi;

  
  uint8 tts;
} SOC_PPC_TRILL_MC_ROUTE_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 expect_adjacent_eep;
  
  SOC_SAND_PP_SYS_PORT_ID expect_system_port;

} SOC_PPC_TRILL_ADJ_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 cfg_ttl;
  uint32 ethertype;
} SOC_PPC_FRWRD_TRILL_GLOBAL_INFO;










void
  SOC_PPC_TRILL_MC_MASKED_FIELDS_clear(
    SOC_SAND_OUT SOC_PPC_TRILL_MC_MASKED_FIELDS *info
  );

void
  SOC_PPC_TRILL_MC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_TRILL_MC_ROUTE_KEY *info
  );

void
  SOC_PPC_TRILL_ADJ_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRILL_ADJ_INFO *info
  );

void
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_TRILL_MC_MASKED_FIELDS_print(
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS *info
  );

void
  SOC_PPC_TRILL_MC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY *info
  );

void
  SOC_PPC_TRILL_ADJ_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO *info
  );

void
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

