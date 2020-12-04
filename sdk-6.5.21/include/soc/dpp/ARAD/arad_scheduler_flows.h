/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_SCHEDULER_FLOWS_H_INCLUDED__

#define __ARAD_SCHEDULER_FLOWS_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>


























uint32
  arad_sch_flow_ipf_config_mode_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_SCH_FLOW_IPF_CONFIG_MODE mode
  );


uint32
  arad_sch_flow_ipf_config_mode_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_SCH_FLOW_IPF_CONFIG_MODE mode
  );

uint32
  arad_sch_flow_ipf_config_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT ARAD_SCH_FLOW_IPF_CONFIG_MODE *mode
  );


uint32
  arad_sch_flow_nof_subflows_get(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     ARAD_SCH_FLOW_ID          base_flow_id,
    SOC_SAND_OUT    uint32                   *nof_subflows
  );


uint32
  arad_sch_flow_nof_subflows_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     ARAD_SCH_FLOW_ID          base_flow_id,
    SOC_SAND_IN     uint32                    nof_subflows,
    SOC_SAND_IN     uint8                   is_odd_even
  );


uint32
  arad_sch_flow_slow_enable_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     ARAD_SCH_FLOW_ID           flow_ndx,
    SOC_SAND_IN     uint8                   is_slow_enabled
  );



uint32
  arad_sch_flow_slow_enable_get(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     ARAD_SCH_FLOW_ID          flow_ndx,
    SOC_SAND_OUT    uint8                   *is_slow_enabled
  );


uint32
  arad_sch_flow_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow
  );


uint32
  arad_sch_flow_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID     flow_ndx,
    SOC_SAND_IN  ARAD_SCH_FLOW        *flow
  );


uint32
  arad_sch_flow_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_OUT ARAD_SCH_FLOW           *flow
  );


uint32
  arad_sch_flow_status_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  ARAD_SCH_FLOW_STATUS    state
  );


uint32
  arad_sch_flow_max_burst_save_and_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID    flow_id,
    SOC_SAND_IN  uint32              new_max_burst,
    SOC_SAND_OUT uint32              *old_max_burst
  );


uint32
  arad_sch_flow_status_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  ARAD_SCH_FLOW_STATUS    state
  );





#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


