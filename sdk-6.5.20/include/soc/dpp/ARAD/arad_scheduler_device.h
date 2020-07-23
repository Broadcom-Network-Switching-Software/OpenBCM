/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_SCHEDULER_DEVICE_H_INCLUDED__

#define __ARAD_SCHEDULER_DEVICE_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>






#define ARAD_SCH_DEVICE_RATE_INTERVAL_RESOLUTION (128)


#define ARAD_SCH_DEVICE_RATE_INTERVAL_MIN (ARAD_SCH_DEVICE_RATE_INTERVAL_RESOLUTION)



#define ARAD_SCH_DEVICE_RATE_INTERVAL_MAX (1048575)


#define ARAD_SCH_DEVICE_IF_WEIGHT_MIN 0
#define ARAD_SCH_DEVICE_IF_WEIGHT_MAX 1023





#define ARAD_SCH_IS_CHNIF_ID(unit, offset) (offset < SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces))  
#define ARAD_SCH_OFFSET_TO_NON_CHANNELIZED_OFFSET(unit, sch_offset) (sch_offset - SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces)) 
















uint32
  arad_sch_if2sched_offset(
    uint32 nif_id
  );


uint32
  arad_sch_device_rate_entry_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_IN  uint32                  rate
  );


uint32
  arad_sch_device_rate_entry_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_IN  uint32                  rate
  );


uint32
  arad_sch_device_rate_entry_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_OUT uint32                  *rate
  );


int
  arad_sch_if_shaper_rate_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  uint32              if_rate
  );


uint32
  arad_sch_if_shaper_rate_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID    if_ndx,
    SOC_SAND_IN  uint32              if_rate
  );


int
  arad_sch_if_shaper_rate_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core,
    SOC_SAND_IN  uint32        tm_port,
    SOC_SAND_OUT uint32        *if_rate
  );


uint32
  arad_sch_device_if_weight_idx_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  uint32              weight_index
  );


uint32
  arad_sch_device_if_weight_idx_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              weight_index
  );


uint32
  arad_sch_device_if_weight_idx_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_OUT uint32              *weight_index
  );


uint32
  arad_sch_if_weight_conf_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_IF_WEIGHTS      *if_weights
  );


uint32
  arad_sch_if_weight_conf_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_IF_WEIGHTS      *if_weights
  );


uint32
  arad_sch_if_weight_conf_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_SCH_IF_WEIGHTS      *if_weights
  );



uint32
  arad_sch_ch_if_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            sch_offset,
    SOC_SAND_IN     uint32            rate
  );

uint32
  arad_sch_ch_if_rate_get_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            sch_offset,
    SOC_SAND_OUT    uint32            *rate
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

