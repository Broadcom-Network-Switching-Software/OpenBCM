/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_egr_queuing.h
 */

#ifndef __JER_EGR_QUEUING_INCLUDED__

#define __JER_EGR_QUEUING_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/ARAD/arad_api_egr_queuing.h>


#define SOC_JER_EGR_IF_MIN   SOC_DPP_IMP_DEFS_GET(unit, egr_if_min)
#define SOC_JER_EGR_IF_CPU   SOC_DPP_IMP_DEFS_GET(unit, egr_if_cpu)
#define SOC_JER_EGR_IF_OLP   SOC_DPP_IMP_DEFS_GET(unit, egr_if_olp)
#define SOC_JER_EGR_IF_OAMP  SOC_DPP_IMP_DEFS_GET(unit, egr_if_oamp)
#define SOC_JER_EGR_IF_RCY   SOC_DPP_IMP_DEFS_GET(unit, egr_if_rcy)
#define SOC_JER_EGR_IF_SAT   SOC_DPP_IMP_DEFS_GET(unit, egr_if_sat)
#define SOC_JER_EGR_IF_IPSEC SOC_DPP_IMP_DEFS_GET(unit, egr_if_ipsec)
#define SOC_JER_EGR_IF_MAX   SOC_DPP_IMP_DEFS_GET(unit, egr_if_max)


int soc_jer_egr_interface_init(int unit);
int soc_jer_egr_interface_alloc(int unit, soc_port_t port);
int soc_jer_egr_interface_free(int unit, soc_port_t port);
int soc_jer_egr_queuing_init_thresholds(int unit, int port_rate,int nof_priorities, int nof_channels, SOC_TMC_EGR_QUEUING_CGM_INIT_THRESHOLDS* cgm_init_thresholds);

int
  soc_jer_egr_port2egress_offset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *egr_if
  );

int
    soc_jer_egr_is_channelized(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_OUT uint32                *is_channalzied
   );

int soc_jer_egr_tm_init(int unit);

typedef enum soc_jer_egr_cal_type_e {
    socJerCalTypeFQP = 0,
    socJerCalTypePQP,
    socJerCalTypesCount
} soc_jer_egr_cal_type_t;

int
  soc_jer_egr_q_nif_cal_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN soc_jer_egr_cal_type_t cal_to_update
  );

int
  soc_jer_egr_q_nif_cal_set_all(
    SOC_SAND_IN int unit
  );

int
  soc_jer_egr_q_fqp_scheduler_config(
    SOC_SAND_IN int unit
  );

int 
soc_jer_egr_q_fast_port_set(
        SOC_SAND_IN  int unit, 
        SOC_SAND_IN  soc_port_t port, 
        SOC_SAND_IN  int is_ilkn, 
        SOC_SAND_IN  int turn_on
        );

uint32
  soc_jer_egr_queuing_sp_tc_drop_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );

uint32
  soc_jer_egr_queuing_sp_tc_drop_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );

uint32 
  soc_jer_egr_queuing_sch_unsch_drop_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_IN    SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  );


uint32 
  soc_jer_egr_queuing_sch_unsch_drop_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_OUT   SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  );
uint32
jer_egr_threshold_types_verify(
   SOC_SAND_IN    int   unit,
   SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type
  );

uint32
  soc_jer_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  );

uint32
  soc_jer_egr_sched_port_fc_thresh_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  int                threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH *thresh
  );

uint32
  soc_jer_egr_sched_q_fc_thresh_set_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core,
    SOC_SAND_IN  int       prio,
    SOC_SAND_IN  int threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH  *thresh
  );

uint32	
  soc_jer_egr_queuing_if_fc_uc_set_unsafe(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	uc_if_profile_ndx,
	SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC	*info
  );

int	
  soc_jer_egr_queuing_if_fc_uc_get_unsafe(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	uc_if_profile_ndx,
	SOC_SAND_OUT ARAD_EGR_QUEUING_IF_UC_FC	*info
  );
  
int
soc_jer_egr_nrdy_th_profile_data_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 profile_data);

int
soc_jer_egr_nrdy_th_profile_data_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_OUT uint32 *profile_data);

int
  jer_egr_ofp_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *thresh
  );

uint32
  soc_jer_egr_unsched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  );

uint32 soc_jer_sp_tc_init(int unit, int core, ARAD_EGR_QUEUING_DEV_TH    *info);

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

