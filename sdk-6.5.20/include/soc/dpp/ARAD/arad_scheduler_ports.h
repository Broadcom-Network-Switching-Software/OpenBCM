/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_SCHEDULER_PORTS_H_INCLUDED__

#define __ARAD_SCHEDULER_PORTS_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>
#include <soc/dpp/ARAD/arad_ports.h>




#define ARAD_NOF_TCG_IN_BITS                        (3)




#define ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx) \
   (ARAD_BASE_PORT_TC2PS(base_port_tc)*ARAD_NOF_TCGS_IN_PS + tcg_ndx)




typedef struct
{
  
  uint32 rate;
  
  uint32 max_burst;
} ARAD_SCH_PORT_PRIORITY_RATE_INFO;

typedef struct
{
  
  uint32 rate;
  
  uint32 max_burst;
} ARAD_SCH_TCG_RATE_INFO;












  
uint32
  arad_sch_port_sched_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_SCH_PORT_INFO  *port_info
  );


uint32
  arad_sch_port_sched_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_SCH_PORT_INFO  *port_info
  );




uint32
  arad_sch_port_sched_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_SCH_PORT_INFO  *port_info
  );


uint32
  arad_sch_port_tcg_weight_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX        tcg_ndx,
    SOC_SAND_IN  ARAD_SCH_TCG_WEIGHT *tcg_weight
  );

uint32
  arad_sch_port_tcg_weight_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX        tcg_ndx,
    SOC_SAND_OUT ARAD_SCH_TCG_WEIGHT *tcg_weight
  );

uint32
  arad_sch_port_tcg_weight_set_verify_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_SCH_TCG_WEIGHT       *tcg_weight
  );

uint32
  arad_sch_port_tcg_weight_get_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX        tcg_ndx,
    SOC_SAND_OUT ARAD_SCH_TCG_WEIGHT *tcg_weight
  );


uint32
  arad_sch_port_hp_class_conf_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  );


uint32
  arad_sch_port_hp_class_conf_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  );


uint32
  arad_sch_port_hp_class_conf_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  ARAD_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  );


uint32
  arad_sch_hr_to_port_assign_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core,
    SOC_SAND_IN  uint32        tm_port,
    SOC_SAND_IN  uint8         is_port_hr
  );


uint32
  arad_sch_hr_to_port_assign_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  int           core,
    SOC_SAND_IN  uint32        tm_port,
    SOC_SAND_OUT uint8         *is_port_hr
  );


uint32
  arad_sch_se2port_tc_id_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  ARAD_SCH_SE_ID    se_id,
    SOC_SAND_OUT ARAD_SCH_PORT_ID  *port_id,
    SOC_SAND_OUT uint32            *tc 
  );


uint32
  arad_sch_port_tc2se_id_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  uint32            tm_port,
    SOC_SAND_IN  uint32            tc,
    SOC_SAND_OUT ARAD_SCH_SE_ID    *se_id
  );


uint32
  arad_sch_port_priority_shaper_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     uint32            priority_ndx,
    SOC_SAND_IN     uint32            rate
  );

uint32
  arad_sch_port_priority_shaper_rate_get_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     uint32            priority_ndx,    
    SOC_SAND_OUT    ARAD_SCH_PORT_PRIORITY_RATE_INFO *info
  );

uint32 
arad_sch_port_priority_shaper_max_burst_set_unsafe(
    SOC_SAND_IN     int                  unit,
    SOC_SAND_IN     int                  core,
    SOC_SAND_IN     uint32               tm_port,
    SOC_SAND_IN     uint32               priority_ndx,
    SOC_SAND_IN     uint32               burst
  );


uint32
  arad_sch_port_priority_shaper_hw_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core
  );



uint32
  arad_sch_tcg_shaper_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_IN     int               rate
  );

uint32
  arad_sch_tcg_shaper_rate_get_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,    
    SOC_SAND_OUT    ARAD_SCH_TCG_RATE_INFO  *info
  );

uint32
arad_sch_tcg_shaper_max_burst_set_unsafe(
    SOC_SAND_IN     int                  unit,
    SOC_SAND_IN     int                  core,
    SOC_SAND_IN     uint32               tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX         tcg_ndx,
    SOC_SAND_IN     uint32               burst
  );



void
  arad_ARAD_SCH_PORT_PRIORITY_RATE_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_PORT_PRIORITY_RATE_INFO *info
  );

void
  arad_ARAD_SCH_TCG_RATE_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_TCG_RATE_INFO *info
  );

int 
arad_sch_e2e_interface_allocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

int 
arad_sch_e2e_interface_deallocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

int
  arad_sch_port_rate_kbits_per_sec_to_qaunta(
    SOC_SAND_IN       int       unit,
    SOC_SAND_IN       uint32    rate,     
    SOC_SAND_IN       uint32    credit_div, 
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   quanta  
  );

int
  arad_sch_port_qunta_to_rate_kbits_per_sec(
    SOC_SAND_IN       int       unit,
    SOC_SAND_IN       uint32    quanta, 
    SOC_SAND_IN       uint32    credit_div,  
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   rate      
  );

uint32
arad_sch_calendar_info_get(int unit, int core, int hr_calendar_num ,  int is_priority_rate_calendar ,
                          uint32 *credit_div, uint32 *nof_ticks, uint32 *quanta_nof_bits);

int
arad_sch_port_sched_min_bw_group_get(
    SOC_SAND_IN     int                  unit,
    SOC_SAND_IN     int                  core,
    SOC_SAND_OUT    uint32               *group
  );

int 
arad_sch_port_hr_group_get(
    SOC_SAND_IN     int                  unit,
    SOC_SAND_IN     int                  core,
    SOC_SAND_IN     uint32               tm_port,
    SOC_SAND_OUT    uint32               *group
  );
#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

