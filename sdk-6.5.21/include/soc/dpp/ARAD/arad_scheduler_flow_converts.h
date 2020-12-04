/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_SCHEDULER_FLOW_CONVERTS_H_INCLUDED__

#define __ARAD_SCHEDULER_FLOW_CONVERTS_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>

























typedef struct
{
  uint32                                sch_number;
  uint32                                cos;
  uint32                                hr_sel_dual;
  uint32                                peak_rate_man;
  uint32                                peak_rate_exp;
  uint32                                max_burst;
  uint32                                max_burst_update;
  uint32                                slow_rate_index;
} ARAD_SCH_INTERNAL_SUB_FLOW_DESC;

uint32
  arad_sch_INTERNAL_SUB_FLOW_to_SUB_FLOW_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     int                     core,
    SOC_SAND_IN     ARAD_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow,
    SOC_SAND_OUT    ARAD_SCH_SUBFLOW           *sub_flow
    );

uint32
  arad_sch_SUB_FLOW_to_INTERNAL_SUB_FLOW_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     int                     core,
    SOC_SAND_IN     ARAD_SCH_SUBFLOW             *sub_flow,
    SOC_SAND_OUT    ARAD_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow
    );

uint32
  arad_sch_from_internal_subflow_shaper_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     ARAD_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow,
    SOC_SAND_OUT    ARAD_SCH_SUBFLOW           *sub_flow
    );

uint32
  arad_sch_to_internal_subflow_shaper_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     ARAD_SCH_SUBFLOW           *sub_flow,
    SOC_SAND_OUT    ARAD_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow,
    SOC_SAND_IN     uint32                     round_up
    );




#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


