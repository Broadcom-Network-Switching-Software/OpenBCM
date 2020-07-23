/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __JER_INGRESS_SCHEDULER_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>

























soc_error_t
  jer_ingress_scheduler_init(
    SOC_SAND_IN  int                 unit
  );

soc_error_t
jer_ingress_scheduler_clos_slow_start_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    );



soc_error_t
jer_ingress_scheduler_clos_slow_start_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    );



soc_error_t
jer_ingress_scheduler_mesh_slow_start_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER   *shaper_info
    );



soc_error_t
jer_ingress_scheduler_mesh_slow_start_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN SOC_TMC_ING_SCH_SHAPER   *shaper_info
    );



soc_error_t
  jer_ingress_scheduler_clos_bandwidth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT uint32              *rate
  );


soc_error_t
  jer_ingress_scheduler_clos_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  uint32              rate
    );


soc_error_t
  jer_ingress_scheduler_mesh_bandwidth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT uint32              *rate
  );


soc_error_t
  jer_ingress_scheduler_mesh_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  uint32              rate
    );



soc_error_t
  jer_ingress_scheduler_clos_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *burst
  );


soc_error_t
  jer_ingress_scheduler_clos_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 burst
    );


soc_error_t
  jer_ingress_scheduler_mesh_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *burst
  );


soc_error_t
  jer_ingress_scheduler_mesh_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 burst
    );


soc_error_t
  jer_ingress_scheduler_clos_sched_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *weight
  );


soc_error_t
  jer_ingress_scheduler_clos_sched_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 weight
    );


soc_error_t
  jer_ingress_scheduler_mesh_sched_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_OUT int                 *weight
  );


soc_error_t
  jer_ingress_scheduler_mesh_sched_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 gport, 
    SOC_SAND_IN  int                 weight
    );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
