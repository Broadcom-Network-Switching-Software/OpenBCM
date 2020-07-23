/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ARAD_API_INGRESS_SCHEDULER_INCLUDED__

#define __ARAD_API_INGRESS_SCHEDULER_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_ingress_scheduler.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>



#define ARAD_ING_SCH_MAX_WEIGHT_VALUE                        63
#define ARAD_ING_SCH_MAX_MAX_CREDIT_VALUE                    65535
#define ARAD_ING_SCH_MAX_NOF_ENTRIES                         ARAD_ING_SCH_NUM_OF_CONTEXTS
#define ARAD_ING_SCH_MAX_ID_VALUE                            ARAD_ING_SCH_NUM_OF_CONTEXTS

#define ARAD_ING_SCH_CLOS_NOF_HP_SHAPERS       3
#define ARAD_ING_SCH_CLOS_NOF_GLOBAL_SHAPERS   2
#define ARAD_ING_SCH_CLOS_NOF_LP_SHAPERS       2











#define ARAD_ING_SCH_MESH_LOCAL                           SOC_TMC_ING_SCH_MESH_LOCAL
#define ARAD_ING_SCH_MESH_CON1                            SOC_TMC_ING_SCH_MESH_CON1
#define ARAD_ING_SCH_MESH_CON2                            SOC_TMC_ING_SCH_MESH_CON2
#define ARAD_ING_SCH_MESH_CON3                            SOC_TMC_ING_SCH_MESH_CON3
#define ARAD_ING_SCH_MESH_CON4                            SOC_TMC_ING_SCH_MESH_CON4
#define ARAD_ING_SCH_MESH_CON5                            SOC_TMC_ING_SCH_MESH_CON5
#define ARAD_ING_SCH_MESH_CON6                            SOC_TMC_ING_SCH_MESH_CON6
#define ARAD_ING_SCH_MESH_CON7                            SOC_TMC_ING_SCH_MESH_CON7
#define ARAD_ING_SCH_MESH_LAST                            SOC_TMC_ING_SCH_MESH_LAST

#define ARAD_ING_SCH_DONT_TOUCH							  SOC_TMC_ING_SCH_DONT_TOUCH						  	   

typedef SOC_TMC_ING_SCH_MESH_CONTEXTS                          ARAD_ING_SCH_MESH_CONTEXTS;

typedef SOC_TMC_ING_SCH_SHAPER                                 ARAD_ING_SCH_SHAPER;
typedef SOC_TMC_ING_SCH_MESH_CONTEXT_INFO                      ARAD_ING_SCH_MESH_CONTEXT_INFO;
typedef SOC_TMC_ING_SCH_MESH_INFO                              ARAD_ING_SCH_MESH_INFO;
typedef SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT                       ARAD_ING_SCH_CLOS_WFQ_ELEMENT;
typedef SOC_TMC_ING_SCH_CLOS_WFQS                              ARAD_ING_SCH_CLOS_WFQS;
typedef SOC_TMC_ING_SCH_CLOS_HP_SHAPERS                        ARAD_ING_SCH_CLOS_HP_SHAPERS;
typedef SOC_TMC_ING_SCH_CLOS_LP_SHAPERS                        ARAD_ING_SCH_CLOS_LP_SHAPERS;
typedef SOC_TMC_ING_SCH_CLOS_SHAPERS                           ARAD_ING_SCH_CLOS_SHAPERS;
typedef SOC_TMC_ING_SCH_CLOS_INFO                              ARAD_ING_SCH_CLOS_INFO;












uint32
  arad_ingress_scheduler_mesh_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *exact_mesh_info
  );


soc_error_t
  arad_ingress_scheduler_mesh_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  uint32              rate
    );


soc_error_t
  arad_ingress_scheduler_mesh_sched_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 weight
    );


soc_error_t
  arad_ingress_scheduler_mesh_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 burst
    );

uint32
  arad_ingress_scheduler_mesh_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *mesh_info
  );


soc_error_t
  arad_ingress_scheduler_mesh_bandwidth_get(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT uint32              *rate
    );


soc_error_t
  arad_ingress_scheduler_mesh_sched_get(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *weight
    );


soc_error_t
  arad_ingress_scheduler_mesh_burst_get(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *burst
    );

soc_error_t
  arad_ingress_scheduler_clos_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info
  );


soc_error_t
  arad_ingress_scheduler_clos_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  uint32              rate
    );

soc_error_t
  arad_ingress_scheduler_clos_sched_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 weight
    );


soc_error_t
  arad_ingress_scheduler_clos_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 burst
    );

soc_error_t
  arad_ingress_scheduler_clos_sched_get(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *weight
    );


soc_error_t
  arad_ingress_scheduler_clos_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *burst
  );

soc_error_t
  arad_ingress_scheduler_clos_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *clos_info
  );


soc_error_t
  arad_ingress_scheduler_clos_bandwidth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT uint32              *rate
  );

void
  arad_ARAD_ING_SCH_SHAPER_clear(
    SOC_SAND_OUT ARAD_ING_SCH_SHAPER *info
  );

void
  arad_ARAD_ING_SCH_MESH_CONTEXT_INFO_clear(
    SOC_SAND_OUT ARAD_ING_SCH_MESH_CONTEXT_INFO *info
  );

void
  arad_ARAD_ING_SCH_MESH_INFO_clear(
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO *info
  );

void
  arad_ARAD_ING_SCH_CLOS_WFQ_ELEMENT_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_WFQ_ELEMENT *info
  );

void
  arad_ARAD_ING_SCH_CLOS_WFQS_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_WFQS *info
  );

void
  arad_ARAD_ING_SCH_CLOS_HP_SHAPERS_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_HP_SHAPERS *info
  );

void
  arad_ARAD_ING_SCH_CLOS_SHAPERS_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_SHAPERS *info
  );

void
  arad_ARAD_ING_SCH_CLOS_INFO_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_ING_SCH_MESH_CONTEXTS_to_string(
    SOC_SAND_IN ARAD_ING_SCH_MESH_CONTEXTS enum_val
  );

void
  arad_ARAD_ING_SCH_SHAPER_print(
    SOC_SAND_IN ARAD_ING_SCH_SHAPER *info
  );

void
  arad_ARAD_ING_SCH_MESH_CONTEXT_INFO_print(
    SOC_SAND_IN ARAD_ING_SCH_MESH_CONTEXT_INFO *info
  );

void
  arad_ARAD_ING_SCH_MESH_INFO_print(
    SOC_SAND_IN ARAD_ING_SCH_MESH_INFO *info
  );

void
  arad_ARAD_ING_SCH_CLOS_WFQ_ELEMENT_print(
    SOC_SAND_IN ARAD_ING_SCH_CLOS_WFQ_ELEMENT *info
  );

void
  arad_ARAD_ING_SCH_CLOS_WFQS_print(
    SOC_SAND_IN ARAD_ING_SCH_CLOS_WFQS *info
  );

void
  arad_ARAD_ING_SCH_CLOS_HP_SHAPERS_print(
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_HP_SHAPERS *info
  );

void
  arad_ARAD_ING_SCH_CLOS_SHAPERS_print(
    SOC_SAND_IN ARAD_ING_SCH_CLOS_SHAPERS *info
  );

void
  arad_ARAD_ING_SCH_CLOS_INFO_print(
    SOC_SAND_IN ARAD_ING_SCH_CLOS_INFO *info
  );

void
  arad_ARAD_ING_SCH_CLOS_INFO_SHAPER_dont_touch(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO *info
  );

void
  arad_ARAD_ING_SCH_MESH_INFO_SHAPERS_dont_touch(
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO *info
  );

#endif 



uint8
  arad_ingress_scheduler_conversion_test_api(
    SOC_SAND_IN uint8 is_regression,
    SOC_SAND_IN uint8 silent
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
