/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_INGRESS_SCHEDULER_INCLUDED__

#define __SOC_TMC_API_INGRESS_SCHEDULER_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>





#define SOC_TMC_ING_SCH_DONT_TOUCH                (0xFFFFFFFF)
#define SOC_TMC_ING_NUMBER_OF_HIGHEST_LATENCY_PACKETS (8)











typedef enum
{
  
  SOC_TMC_ING_SCH_MESH_LOCAL=0,
  
  SOC_TMC_ING_SCH_MESH_CON1=1,
  
  SOC_TMC_ING_SCH_MESH_CON2=2,
  
  SOC_TMC_ING_SCH_MESH_CON3=3,
  
  SOC_TMC_ING_SCH_MESH_CON4=4,
  
  SOC_TMC_ING_SCH_MESH_CON5=5,
  
  SOC_TMC_ING_SCH_MESH_CON6=6,
  
  SOC_TMC_ING_SCH_MESH_CON7=7,
  
  SOC_TMC_ING_SCH_MESH_LAST=8
}SOC_TMC_ING_SCH_MESH_CONTEXTS;

#define   SOC_TMC_ING_NOF_SCH_MESH_CONTEXTSS SOC_TMC_ING_SCH_MESH_LAST


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 max_rate;
  
  uint32 max_burst;
  
  int     slow_start_enable;
  uint32  slow_start_rate_phase_0;
  uint32  slow_start_rate_phase_1;

}SOC_TMC_ING_SCH_SHAPER;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ING_SCH_SHAPER shaper;
  
  uint32 weight;
  
  SOC_TMC_ING_SCH_MESH_CONTEXTS id;
}SOC_TMC_ING_SCH_MESH_CONTEXT_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ING_SCH_MESH_CONTEXT_INFO contexts[SOC_TMC_ING_SCH_MESH_LAST];
  
  uint32 nof_entries;
  
  SOC_TMC_ING_SCH_SHAPER total_rate_shaper;
}SOC_TMC_ING_SCH_MESH_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 weight1;
  
  uint32 weight2;
}SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT fabric_hp;
  
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT fabric_lp;
  
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT global_hp;
  
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT global_lp;
}SOC_TMC_ING_SCH_CLOS_WFQS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ING_SCH_SHAPER local;
  
  SOC_TMC_ING_SCH_SHAPER fabric_unicast;
  
  SOC_TMC_ING_SCH_SHAPER fabric_multicast;

} SOC_TMC_ING_SCH_CLOS_HP_SHAPERS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
    
  SOC_TMC_ING_SCH_SHAPER fabric_unicast;
  
  SOC_TMC_ING_SCH_SHAPER fabric_multicast;   
    
} SOC_TMC_ING_SCH_CLOS_LP_SHAPERS;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ING_SCH_SHAPER local;
  
  SOC_TMC_ING_SCH_SHAPER fabric;
  
  SOC_TMC_ING_SCH_CLOS_LP_SHAPERS lp;
  
  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS hp;

}SOC_TMC_ING_SCH_CLOS_SHAPERS;



typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ING_SCH_CLOS_SHAPERS shapers;
  
  SOC_TMC_ING_SCH_CLOS_WFQS weights;
}SOC_TMC_ING_SCH_CLOS_INFO;











void
  SOC_TMC_ING_SCH_SHAPER_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER *info
  );

void
  SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_MESH_CONTEXT_INFO *info
  );

void
  SOC_TMC_ING_SCH_MESH_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_MESH_INFO *info
  );

void
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT *info
  );

void
  SOC_TMC_ING_SCH_CLOS_WFQS_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_WFQS *info
  );

void
  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_HP_SHAPERS *info
  );

void
  SOC_TMC_ING_SCH_CLOS_SHAPERS_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_SHAPERS *info
  );

void
  SOC_TMC_ING_SCH_CLOS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_ING_SCH_MESH_CONTEXTS_to_string(
    SOC_SAND_IN SOC_TMC_ING_SCH_MESH_CONTEXTS enum_val
  );

void
  SOC_TMC_ING_SCH_SHAPER_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_SHAPER *info
  );

void
  SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_MESH_CONTEXT_INFO *info
  );

void
  SOC_TMC_ING_SCH_MESH_INFO_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_MESH_INFO *info
  );

void
  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT *info
  );

void
  SOC_TMC_ING_SCH_CLOS_WFQS_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_WFQS *info
  );

void
  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_print(
    SOC_SAND_IN  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS *info
  );

void
  SOC_TMC_ING_SCH_CLOS_SHAPERS_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_SHAPERS *info
  );

void
  SOC_TMC_ING_SCH_CLOS_INFO_print(
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_INFO *info
  );

void
  SOC_TMC_ING_SCH_CLOS_INFO_SHAPERS_dont_touch(
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO *info
  );

void
  SOC_TMC_ING_SCH_MESH_INFO_SHAPERS_dont_touch(
    SOC_SAND_OUT SOC_TMC_ING_SCH_MESH_INFO *info
  );
#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
