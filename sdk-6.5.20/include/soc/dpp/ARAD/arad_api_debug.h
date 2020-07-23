/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_DEBUG_INCLUDED__

#define __ARAD_API_DEBUG_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_debug.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>














typedef enum
{
  
  ARAD_DBG_RST_DOMAIN_INGR = 0,
  
  ARAD_DBG_RST_DOMAIN_EGR = 1,
  
  ARAD_DBG_RST_DOMAIN_SCH = 2,
  
  ARAD_DBG_RST_DOMAIN_FULL= 3,
    
  ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC = 4,
  
  ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC = 5,
  
  ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC = 6,
  
  ARAD_DBG_NOF_RST_DOMAINS = 7
}ARAD_DBG_RST_DOMAIN;

typedef SOC_TMC_DBG_AUTOCREDIT_INFO                            ARAD_DBG_AUTOCREDIT_INFO;



uint32
  arad_dbg_autocredit_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_AUTOCREDIT_INFO *info,
    SOC_SAND_OUT uint32                  *exact_rate
  );



uint32
  arad_dbg_autocredit_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_DBG_AUTOCREDIT_INFO *info
  );


uint32
  arad_dbg_ingr_reset(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_dbg_dev_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_DBG_RST_DOMAIN      rst_domain
  );

void
  arad_ARAD_DBG_AUTOCREDIT_INFO_clear(
    SOC_SAND_OUT ARAD_DBG_AUTOCREDIT_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



