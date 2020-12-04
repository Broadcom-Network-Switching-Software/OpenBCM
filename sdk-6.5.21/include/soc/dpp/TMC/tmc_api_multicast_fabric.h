/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_MULTICAST_FABRIC_INCLUDED__

#define __SOC_TMC_API_MULTICAST_FABRIC_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/dpp_config_defs.h>




#define SOC_TMC_MULT_FABRIC_NOF_BE_CLASSES      3

#define SOC_TMC_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS(unit) SOC_SAND_DIV_ROUND_UP(SOC_DPP_DEFS_GET(unit, nof_fabric_links),SOC_SAND_REG_SIZE_BITS)
#define SOC_TMC_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS_MAX SOC_SAND_DIV_ROUND_UP(SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS),SOC_SAND_REG_SIZE_BITS)











typedef uint32 SOC_TMC_MULT_FABRIC_CLS;

typedef enum
{
  
  SOC_TMC_MULT_FABRIC_CLS_MIN=0,
  
  SOC_TMC_MULT_FABRIC_CLS_MAX=3,
  
  SOC_TMC_MULT_FABRIC_NOF_CLS
}SOC_TMC_MULT_FABRIC_CLS_RNG;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FAP_PORT_ID mcast_class_port_id;
  
  uint8 multicast_class_valid;
}SOC_TMC_MULT_FABRIC_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 rate;
  
  uint32 max_burst;
}SOC_TMC_MULT_FABRIC_SHAPER_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_MULT_FABRIC_PORT_INFO be_sch_port;
  
  uint32 weight;
}SOC_TMC_MULT_FABRIC_BE_CLASS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_MULT_FABRIC_SHAPER_INFO be_shaper;
  
  uint8 wfq_enable;
  
  SOC_TMC_MULT_FABRIC_BE_CLASS_INFO be_sch_port[SOC_TMC_MULT_FABRIC_NOF_BE_CLASSES];
}SOC_TMC_MULT_FABRIC_BE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_MULT_FABRIC_SHAPER_INFO gr_shaper;
  
  SOC_TMC_MULT_FABRIC_PORT_INFO gr_sch_port;
}SOC_TMC_MULT_FABRIC_GR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_MULT_FABRIC_GR_INFO guaranteed;
  
  SOC_TMC_MULT_FABRIC_BE_INFO best_effort;
  
  uint32 max_rate;
  
  uint32 max_burst;
  
  uint8 credits_via_sch;
}SOC_TMC_MULT_FABRIC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 bitmap[SOC_TMC_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS_MAX];
}SOC_TMC_MULT_FABRIC_ACTIVE_LINKS;

#define SOC_TMC_MULT_FABRIC_FLOW_CONTROL_DONT_MAP 0xFFFFFFFF

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

   
  uint32 gfmc_lb_fc_map;

   
  uint32 bfmc0_lb_fc_map;

   
  uint32 bfmc1_lb_fc_map;

   
  uint32 bfmc2_lb_fc_map;


}SOC_TMC_MULT_FABRIC_FLOW_CONTROL_MAP;











void
  SOC_TMC_MULT_FABRIC_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_PORT_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_SHAPER_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_SHAPER_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_BE_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_BE_CLASS_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_BE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_BE_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_GR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_GR_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_ACTIVE_LINKS_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_ACTIVE_LINKS *info
  );

void
  SOC_TMC_MULT_FABRIC_FLOW_CONTROL_MAP_clear(
    SOC_SAND_OUT SOC_TMC_MULT_FABRIC_FLOW_CONTROL_MAP *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_MULT_FABRIC_CLS_RNG_to_string(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_CLS_RNG enum_val
  );

void
  SOC_TMC_MULT_FABRIC_PORT_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_PORT_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_SHAPER_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_SHAPER_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_BE_CLASS_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_BE_CLASS_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_BE_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_BE_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_GR_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_GR_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_INFO_print(
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_INFO *info
  );

void
  SOC_TMC_MULT_FABRIC_ACTIVE_LINKS_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_MULT_FABRIC_ACTIVE_LINKS *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
