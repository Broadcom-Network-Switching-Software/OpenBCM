/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_sch.h
 */

#ifndef __JER_SCH_INCLUDED__

#define __JER_SCH_INCLUDED__

int
  soc_jer_sch_init(
    SOC_SAND_IN   int                    unit
    );

uint32
  jer_sch_device_rate_entry_core_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_OUT uint32              *rate
   ) ;

uint32
  jer_sch_device_rate_entry_core_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_IN  uint32              rate
  ) ;

int 
soc_jer_sch_e2e_interface_allocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

int 
soc_jer_sch_e2e_interface_deallocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

int 
  soc_jer_sch_cal_max_size_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_OUT  uint32*                max_cal_size
   );

int
  soc_jer_sch_cal_tbl_set(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   uint32                 sch_to_set ,
    SOC_SAND_IN   uint32                 slots_count,
                  uint32*                slots
  );

int
  soc_jer_sch_cal_tbl_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   uint32                 sch_to_set ,
    SOC_SAND_IN   uint32                 slots_count,
    SOC_SAND_OUT  uint32*                slots
  );

int soc_jer_sch_prio_propagation_enable_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int enable
    );

int soc_jer_sch_prio_propagation_enable_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT int *enable
    );

int soc_jer_sch_prio_propagation_port_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_IN  int is_high_prio
   );

int soc_jer_sch_prio_propagation_port_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_OUT int *is_high_prio
   );

uint32
  jer_sch_slow_max_rates_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_IN  int                 slow_rate_val
  );

uint32
  jer_sch_slow_max_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_OUT int                 *slow_rate_val
  );

uint32
jer_sch_slow_max_rates_per_level_set(
   SOC_SAND_IN int unit, 
   SOC_SAND_IN int core, 
   SOC_SAND_IN int level , 
   SOC_SAND_IN int slow_rate_type, 
   SOC_SAND_IN int slow_rate_val);

uint32
jer_sch_slow_max_rates_per_level_get(
   SOC_SAND_IN  int   unit,
   SOC_SAND_IN  int   core,
   SOC_SAND_IN  int   level,
   SOC_SAND_IN  int   slow_rate_type,
   SOC_SAND_OUT int*  slow_rate_val);

uint32
jer_ofp_rates_sch_port_priority_rate_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_IN  uint32             rate
    );

uint32
  jer_ofp_rates_sch_port_priority_rate_get(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     uint32            priority_ndx,    
    SOC_SAND_OUT    uint32            *rate
  );

uint32
jer_ofp_rates_sch_tcg_shaper_rate_set(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_IN     uint32               rate);

uint32
jer_ofp_rates_sch_tcg_shaper_rate_get(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_OUT    uint32*               rate);

int
jer_ofp_rates_sch_tcg_shaper_disable(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            base_qpair,
    SOC_SAND_IN     uint32            nof_q_pairs);

uint32
jer_ofp_rates_sch_port_priority_hw_set(
   SOC_SAND_IN   int    unit,
   SOC_SAND_IN   int    core);

uint32
jer_ofp_rates_sch_port_priority_max_burst_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority, 
    SOC_SAND_IN  uint32             max_burst 
   );

uint32
jer_ofp_rates_sch_port_priority_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority, 
    SOC_SAND_OUT uint32             *max_burst
   );

uint32
jer_ofp_rates_sch_tcg_shaper_max_burst_set(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_IN     uint32               max_burst
   );

uint32
jer_ofp_rates_sch_tcg_shaper_max_burst_get(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_OUT    uint32*               max_burst
   );

int
_jer_sch_rates_calendar_index_get(int unit, 
                                  int core, 
                                  int offset, 
                                  int is_hr_calendar,
                                  uint32 *index);

#endif 

