/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_END2END_SCHEDULER_INCLUDED__

#define __ARAD_END2END_SCHEDULER_INCLUDED__



#include <soc/dpp/cosq.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>

























uint32
  arad_scheduler_end2end_init(
    SOC_SAND_IN  int                 unit
  );

void
arad_sch_deallocate_cache_for_per1k(int unit);



uint32
  arad_scheduler_end2end_regs_init(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_sch_slow_max_rates_verify(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_sch_slow_max_rates_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_IN  int                 slow_rate_val
  );


uint32
  arad_sch_slow_max_rates_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_OUT int      *slow_rate_val
  );


uint32
  arad_sch_aggregate_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_INFO        *se,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow
  );


uint32
  arad_sch_aggregate_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_ID      se_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_INFO        *se,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow
  );


uint32
  arad_sch_aggregate_group_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_INFO        *se,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow
  );


uint32
  arad_sch_aggregate_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_OUT ARAD_SCH_SE_INFO        *se,
    SOC_SAND_OUT ARAD_SCH_FLOW           *flow
  );



uint32
  arad_sch_per1k_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_IN  ARAD_SCH_GLOBAL_PER1K_INFO *per1k_info
  );


uint32
  arad_sch_per1k_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_IN  ARAD_SCH_GLOBAL_PER1K_INFO *per1k_info
  );


uint32
  arad_sch_per1k_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_OUT ARAD_SCH_GLOBAL_PER1K_INFO *per1k_info
  );


uint32
  arad_sch_flow_to_queue_mapping_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_IN  uint32                 nof_quartets_to_map,
    SOC_SAND_IN  ARAD_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  );


uint32
  arad_sch_flow_to_queue_mapping_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_IN  uint32                 nof_quartets_to_map,
    SOC_SAND_IN  ARAD_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  );


uint32
  arad_sch_flow_to_queue_mapping_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_OUT ARAD_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  );


uint8
  arad_sch_is_flow_id_se_id(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_id
  );


uint32
  arad_sch_se_group_get(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     ARAD_SCH_SE_ID        se_ndx,
    SOC_SAND_OUT    ARAD_SCH_GROUP*         group
  );



uint32
  arad_sch_se_group_set(
    SOC_SAND_IN     int              unit,
    SOC_SAND_IN     int              core,
    SOC_SAND_IN     ARAD_SCH_SE_ID        se_ndx,
    SOC_SAND_IN     ARAD_SCH_GROUP        group
  );


uint32
  arad_sch_nof_quartets_to_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_interdigitated,
    SOC_SAND_IN  uint8                 is_composite,
    SOC_SAND_OUT uint32                 *nof_quartets_to_map
  );

uint32
  arad_sch_flow_delete_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID         flow_ndx
  );

uint32
  arad_sch_flow_is_deleted_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID         flow_ndx,
    SOC_SAND_OUT uint8                 *flow_is_reset
  );

uint32
  arad_flow_and_up_info_get_unsafe(
    SOC_SAND_IN     int                      unit,
    SOC_SAND_IN     int                      core,
    SOC_SAND_IN     uint32                      flow_id,
    SOC_SAND_IN     uint32                      reterive_status,
    SOC_SAND_INOUT  ARAD_SCH_FLOW_AND_UP_INFO   *flow_and_up_info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


