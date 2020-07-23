/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_EGR_QUEUING_INCLUDED__

#define __ARAD_EGR_QUEUING_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_api_egr_queuing.h>
#include <soc/dpp/ARAD/arad_api_general.h>



#define ARAD_EGR_NOF_Q_PAIRS                                     (256)
#define ARAD_EGR_NOF_BASE_Q_PAIRS                                (256)
#define ARAD_EGR_NOF_Q_PAIRS_IN_PS                               (8)
#define ARAD_EGR_NOF_PS                                          (ARAD_EGR_NOF_BASE_Q_PAIRS / ARAD_EGR_NOF_Q_PAIRS_IN_PS)
#define ARAD_EGR_NOF_TCG_IDS                                     (ARAD_EGR_NOF_PS*ARAD_NOF_TCGS)
#define ARAD_NOF_TCGS_IN_PS                                      (8)

#define ARAD_EGR_BASE_Q_PAIRS_NDX_MAX                            (ARAD_EGR_NOF_BASE_Q_PAIRS-1)
#define ARAD_EGR_INVALID_BASE_Q_PAIR                             (ARAD_EGR_BASE_Q_PAIRS_NDX_MAX)

#define ARAD_EGQ_THRESHOLD_PD_NOF_BITS                           (15)
#define ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS                        (14)

#define ARAD_EGR_Q_PRIORITY_TC_MAX                               (7)
#define ARAD_EGR_Q_PRIORITY_DP_MAX                               (3)

#define ARAD_EGQ_PD_INTERFACE_NOF_BITS                           (15)
#define ARAD_EGQ_SIZE_256_INTERFACE_NOF_BITS                     (14)


#define ARAD_EGR_QUEUING_MC_QUEUE_OFFSET(q_pair) \
    (q_pair + 16)

#define ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(profile,egress_tc) \
  ((profile << 3) + egress_tc)

#define ARAD_EGR_SGMII_RATE                                      (1)


#define ARAD_EGQ_NIF_PORT_CAL_BW_GIVE_UPON_REQUEST (125)

#define ARAD_EGR_MAX_CAL_SIZE                                 (128)
#define ARAD_EGR_MAX_ENTRY_INDX                               (127)

#define ARAD_EGR_MAXIMAL_NUMBER_OF_MUX_COMPETITORS            (ARAD_NIF_NOF_NIFS)
#define ARAD_EGR_OTHERS_INDX                                  (28)


#define ARAD_EGQ_NIF_PORT_CAL_BW_INVALID          127



#define ARAD_EGR_QUEUEING_HIGER_REQ_MAX_RATE_MBPS 100000





#define ARAD_EGQ_ERP_BASE_Q_PAIR (31*8)
















int
  arad_egr_queuing_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_QUEUING_PARTITION_SCHEME  scheme
  );


uint32
  arad_egr_queuing_q_pair_port_tc_find(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     q_pair,
    SOC_SAND_OUT uint8                      *found,
    SOC_SAND_OUT ARAD_FAP_PORT_ID           *ofp_idx,
    SOC_SAND_OUT uint32                     *tc,
    SOC_SAND_OUT int                        *core_id
  );



uint32
  arad_egr_ofp_thresh_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID    ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_thresh_type
  );


uint32
  arad_egr_ofp_thresh_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_thresh_type
  );


uint32
  arad_egr_ofp_thresh_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_THRESH_TYPE *ofp_thresh_type
  );


uint32
  arad_egr_sched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  );


uint32
  arad_egr_sched_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh
  );



int
  arad_egr_q_nif_cal_set_unsafe(
    SOC_SAND_IN int unit
  );


uint32
  arad_egr_sched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  );


uint32
  arad_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  );

uint32
  arad_egr_sched_port_fc_thresh_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  int                threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH *thresh
  );


uint32
  arad_egr_unsched_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh
  );


uint32
  arad_egr_unsched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  );


uint32
  arad_egr_dev_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_FC_DEVICE_THRESH *thresh,
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *exact_thresh
  );


uint32
  arad_egr_dev_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_FC_DEVICE_THRESH *thresh
  );


uint32
  arad_egr_dev_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *thresh
  );


uint32
  arad_egr_xaui_spaui_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  ARAD_EGR_FC_CHNIF_THRESH *thresh,
    SOC_SAND_OUT ARAD_EGR_FC_CHNIF_THRESH *exact_thresh
  );


uint32
  arad_egr_xaui_spaui_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  ARAD_EGR_FC_CHNIF_THRESH *thresh
  );


uint32
  arad_egr_xaui_spaui_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT ARAD_EGR_FC_CHNIF_THRESH *thresh
  );


uint32
  arad_egr_ofp_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  ARAD_EGR_FC_OFP_THRESH   *thresh,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *exact_thresh
  );


uint32
  arad_egr_ofp_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  ARAD_EGR_FC_OFP_THRESH   *thresh
  );


uint32
  arad_egr_ofp_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *thresh
  );


uint32
  arad_egr_ofp_scheduling_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_EGR_OFP_SCH_INFO    *info
  );


uint32
  arad_egr_ofp_scheduling_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_OFP_SCH_INFO    *info
  );


uint32
  arad_egr_ofp_scheduling_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_EGR_OFP_SCH_INFO    *info
  );


int
  arad_egr_q_prio_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO_MAPPING_TYPE   map_type_ndx,
    SOC_SAND_IN  uint32                         tc_ndx,
    SOC_SAND_IN  uint32                         dp_ndx,
    SOC_SAND_IN  uint32                         map_profile_ndx,
    SOC_SAND_IN  ARAD_EGR_Q_PRIORITY            *priority
  );


int
  arad_egr_q_prio_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO_MAPPING_TYPE   map_type_ndx,
    SOC_SAND_IN  uint32                         tc_ndx,
    SOC_SAND_IN  uint32                         dp_ndx,
    SOC_SAND_IN  uint32                         map_profile_ndx,
    SOC_SAND_OUT ARAD_EGR_Q_PRIORITY            *priority
  );


int
  arad_egr_q_profile_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 map_profile_id
  );


int
  arad_egr_q_profile_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *map_profile_id
  );


int
  arad_egr_q_cgm_interface_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 cgm_interface
  );


uint32
  arad_egr_dsp_pp_shaper_mode_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_SHAPER_MODE      shaper_mode
  );

uint32
  arad_egr_dsp_pp_shaper_mode_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_SHAPER_MODE      shaper_mode
  );

uint32
  arad_egr_dsp_pp_shaper_mode_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx
  );


uint32
  arad_egr_dsp_pp_shaper_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_SHAPER_MODE     *shaper_mode
  );



int
  arad_egr_dsp_pp_to_base_q_pair_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_IN  uint32     tm_port,
    SOC_SAND_IN  uint32     base_q_pair
  );

uint32
  arad_egr_dsp_pp_to_base_q_pair_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  uint32                   tm_port,
    SOC_SAND_IN  uint32                   base_q_pair
  );

uint32
  arad_egr_dsp_pp_to_base_q_pair_get_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID           ofp_ndx
  );


uint32
  arad_egr_dsp_pp_to_base_q_pair_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_OUT uint32                      *base_q_pair
  );


int
  arad_egr_dsp_pp_priorities_mode_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core_id,
    SOC_SAND_IN  uint32                         tm_port,
    SOC_SAND_IN  ARAD_EGR_PORT_PRIORITY_MODE    priority_mode
  );

uint32
  arad_egr_dsp_pp_priorities_mode_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_PRIORITY_MODE    priority_mode
  );

uint32
  arad_egr_dsp_pp_priorities_mode_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID              ofp_ndx
  );


uint32
  arad_egr_dsp_pp_priorities_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_PRIORITY_MODE    *priority_mode
  );


uint32	
  arad_egr_queuing_dev_set_unsafe(
	SOC_SAND_IN	 int	unit,
    SOC_SAND_IN  int    core,
	SOC_SAND_IN  ARAD_EGR_QUEUING_DEV_TH	*info
  );

uint32	
  arad_egr_queuing_dev_get_unsafe(
	SOC_SAND_IN	 int	unit,
    SOC_SAND_IN  int    core,
	SOC_SAND_OUT ARAD_EGR_QUEUING_DEV_TH	*info
  );

uint32    
  arad_egr_queuing_global_drop_set_unsafe(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_IN  int    threshold_value,
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type 
  );

uint32    
  arad_egr_queuing_global_drop_get_unsafe(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_OUT int*    threshold_value,
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type 
  );

uint32
  arad_egr_queuing_sp_tc_drop_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
uint32
  arad_egr_queuing_sp_tc_drop_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );

uint32
  arad_egr_queuing_sp_reserved_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );

uint32
  arad_egr_queuing_sp_reserved_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );

uint32 
  arad_egr_queuing_sch_unsch_drop_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_IN    SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  );

uint32
  arad_egr_queuing_global_fc_set_unsafe(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_IN  int    threshold_value, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  );

uint32
  arad_egr_queuing_global_fc_get_unsafe(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_OUT int*   threshold_value, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  );

uint32 
  arad_egr_queuing_mc_tc_fc_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value
  );

uint32 
  arad_egr_queuing_mc_tc_fc_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*    threshold_value
  );



uint32	
  arad_egr_queuing_mc_cos_map_set_unsafe(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	tc_ndx,
	SOC_SAND_IN	uint32	dp_ndx,
	SOC_SAND_IN ARAD_EGR_QUEUING_MC_COS_MAP	*info
  );

uint32	
  arad_egr_queuing_mc_cos_map_get_unsafe(
	SOC_SAND_IN	 int	unit,
    SOC_SAND_IN  int    core,
	SOC_SAND_IN	 uint32	tc_ndx,
	SOC_SAND_IN	 uint32	dp_ndx,
	SOC_SAND_OUT ARAD_EGR_QUEUING_MC_COS_MAP	*info
  );


uint32	
  arad_egr_queuing_if_fc_set_unsafe(
	SOC_SAND_IN	 int	unit,
	SOC_SAND_IN	 ARAD_INTERFACE_ID	if_ndx,
	SOC_SAND_IN  ARAD_EGR_QUEUING_IF_FC	*info
  );

uint32	
  arad_egr_queuing_if_fc_get_unsafe(
	SOC_SAND_IN	 int	unit,
	SOC_SAND_IN	 ARAD_INTERFACE_ID	if_ndx,
	SOC_SAND_OUT ARAD_EGR_QUEUING_IF_FC	*info
  );


soc_error_t
arad_egr_queuing_is_high_priority_port_get(int unit, int core, uint32 tm_port, int *is_high_priority);



uint32	
  arad_egr_queuing_if_fc_uc_set_unsafe(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	uc_if_profile_ndx,
	SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC	*info
  );

uint32	
  arad_egr_queuing_if_fc_uc_get_unsafe(
	SOC_SAND_IN	 int	unit,
    SOC_SAND_IN  int    core,
	SOC_SAND_IN	 uint32	uc_if_profile_ndx,
	SOC_SAND_OUT ARAD_EGR_QUEUING_IF_UC_FC	*info
  );



uint32	
  arad_egr_queuing_if_fc_mc_set_unsafe(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	mc_if_profile_ndx,
	SOC_SAND_IN uint32    pd_th
  );

uint32	
  arad_egr_queuing_if_fc_mc_get_unsafe(
	SOC_SAND_IN	 int	unit,
	SOC_SAND_IN	 uint32	mc_if_profile_ndx,
	SOC_SAND_OUT uint32   *pd_th
  );

uint32 
  arad_egr_queuing_if_uc_map_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  soc_port_if_t interface_type,
    SOC_SAND_IN  uint32     internal_if_id,
    SOC_SAND_IN  int        profile
  );

uint32 
  arad_egr_queuing_if_mc_map_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  soc_port_if_t interface_type,
    SOC_SAND_IN  uint32     internal_if_id,
    SOC_SAND_IN  int        profile
  );


uint32
  arad_egr_queuing_ofp_tcg_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  uint32                         tm_port,
    SOC_SAND_IN  ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  );

uint32
  arad_egr_queuing_ofp_tcg_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  uint32                         tm_port,
    SOC_SAND_OUT ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  );

uint32
  arad_egr_queuing_ofp_tcg_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  uint32                   tm_port,    
    SOC_SAND_IN  ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  );


uint32
  arad_egr_queuing_tcg_weight_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  );

uint32
  arad_egr_queuing_tcg_weight_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  );

uint32
  arad_egr_queuing_tcg_weight_set_verify_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  );

uint32
  arad_egr_queuing_tcg_weight_get_verify_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN ARAD_EGR_TCG_SCH_WFQ       *tcg_weight
  );


uint32
  arad_egr_thresh_fld_to_mnt_exp(
    SOC_SAND_IN  ARAD_REG_FIELD     *thresh_fld,
    SOC_SAND_IN  uint32            thresh,
    SOC_SAND_OUT uint32            *exact_thresh,
    SOC_SAND_OUT uint32            *thresh_fld_val
  );

uint32
  arad_egr_mnt_exp_to_thresh_fld(
    SOC_SAND_IN  ARAD_REG_FIELD  *thresh_fld,
    SOC_SAND_IN  uint32         *thresh_fld_val_in,
    SOC_SAND_OUT uint32         *thresh_out
  );

uint32
  ARAD_EGR_Q_PRIORITY_verify(
    SOC_SAND_IN  ARAD_EGR_Q_PRIORITY *info
  );


int
  arad_egr_queuing_partition_scheme_set_unsafe(
      SOC_SAND_IN   int unit,
      SOC_SAND_IN   ARAD_EGR_QUEUING_PARTITION_SCHEME	scheme
  );



int soc_arad_egr_congestion_statistics_get(
  SOC_SAND_IN int unit,
  SOC_SAND_IN int core,
  SOC_SAND_INOUT ARAD_EGR_CGM_CONGENSTION_STATS *cur_stats,   
  SOC_SAND_INOUT ARAD_EGR_CGM_CONGENSTION_STATS *max_stats,   
  SOC_SAND_INOUT ARAD_EGR_CGM_CONGENSTION_COUNTERS *counters, 
  SOC_SAND_IN int disable_updates 
  );




int
  arad_egr_q_fqp_scheduler_config(
    SOC_SAND_IN int unit
  );




int 
arad_egr_queuing_egr_interface_alloc(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

int 
arad_egr_queuing_egr_interface_free(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

uint32
arad_egr_threshold_types_verify(
   SOC_SAND_IN    int   unit,
   SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type);

uint32
    arad_egr_ofp_fc_q_pair_thresh_set_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  int                 core,
      SOC_SAND_IN  uint32                 egress_tc,
      SOC_SAND_IN  uint32                 threshold_type,
      SOC_SAND_IN  ARAD_EGR_THRESH_INFO      *thresh_info
    );

int arad_egr_queuing_init_thresholds(int unit, int port_rate, int nof_priorities, int nof_channels, SOC_TMC_EGR_QUEUING_CGM_INIT_THRESHOLDS* cgm_init_thresholds);

#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

