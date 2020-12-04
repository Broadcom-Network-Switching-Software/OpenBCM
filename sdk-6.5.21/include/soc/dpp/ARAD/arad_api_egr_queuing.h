/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_EGR_QUEUING_INCLUDED__

#define __ARAD_API_EGR_QUEUING_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/cosq.h>




#define ARAD_EGR_OFP_SCH_WFQ_WEIGHT_MAX 255

#define ARAD_EGR_OFP_INTERFACE_PRIO_NONE SOC_TMC_EGR_OFP_INTERFACE_PRIO_NONE

#define ARAD_NOF_THRESH_TYPES                                       SOC_TMC_NOF_THRESH_TYPES











#define ARAD_EGR_OFP_INTERFACE_PRIO_HIGH                  SOC_TMC_EGR_OFP_INTERFACE_PRIO_HIGH
#define ARAD_EGR_OFP_INTERFACE_PRIO_MID                   SOC_TMC_EGR_OFP_INTERFACE_PRIO_MID
#define ARAD_EGR_OFP_INTERFACE_PRIO_LOW                   SOC_TMC_EGR_OFP_INTERFACE_PRIO_LOW
#define ARAD_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST            SOC_TMC_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST
#define ARAD_EGR_OFP_CHNIF_NOF_PRIORITIES                 SOC_TMC_EGR_OFP_CHNIF_NOF_PRIORITIES
typedef SOC_TMC_EGR_OFP_INTERFACE_PRIO                        ARAD_EGR_OFP_INTERFACE_PRIO;

#define ARAD_EGR_QUEUING_PARTITION_SCHEME_DISCRETE        SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_DISCRETE
#define ARAD_EGR_QUEUING_PARTITION_SCHEME_STRICT          SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_STRICT
#define ARAD_NOF_EGR_QUEUING_PARTITION_SCHEMES            SOC_TMC_NOF_EGR_QUEUING_PARTITION_SCHEMES
typedef SOC_TMC_EGR_QUEUING_PARTITION_SCHEME                  ARAD_EGR_QUEUING_PARTITION_SCHEME;

typedef SOC_TMC_EGR_QUEUING_TH_DB_GLOBAL                      ARAD_EGR_QUEUING_TH_DB_GLOBAL;
typedef SOC_TMC_EGR_QUEUING_TH_DB_POOL                        ARAD_EGR_QUEUING_TH_DB_POOL;
typedef SOC_TMC_EGR_QUEUING_TH_DB_PORT                        ARAD_EGR_QUEUING_TH_DB_PORT;
typedef SOC_TMC_EGR_QUEUING_DEV_TH                            ARAD_EGR_QUEUING_DEV_TH;
typedef SOC_TMC_EGR_QUEUING_MC_COS_MAP                        ARAD_EGR_QUEUING_MC_COS_MAP;
typedef SOC_TMC_EGR_QUEUING_IF_FC                             ARAD_EGR_QUEUING_IF_FC;
typedef SOC_TMC_EGR_QUEUING_IF_UC_FC                          ARAD_EGR_QUEUING_IF_UC_FC;

#define ARAD_EGR_PORT_THRESH_TYPE_0                       SOC_TMC_EGR_PORT_THRESH_TYPE_0
#define ARAD_EGR_PORT_THRESH_TYPE_1                       SOC_TMC_EGR_PORT_THRESH_TYPE_1
#define ARAD_EGR_PORT_THRESH_TYPE_2                       SOC_TMC_EGR_PORT_THRESH_TYPE_2
#define ARAD_EGR_PORT_THRESH_TYPE_3                       SOC_TMC_EGR_PORT_THRESH_TYPE_3
#define ARAD_EGR_PORT_THRESH_TYPE_4                       SOC_TMC_EGR_PORT_THRESH_TYPE_4
#define ARAD_EGR_PORT_THRESH_TYPE_5                       SOC_TMC_EGR_PORT_THRESH_TYPE_5
#define ARAD_EGR_PORT_THRESH_TYPE_6                       SOC_TMC_EGR_PORT_THRESH_TYPE_6
#define ARAD_EGR_PORT_THRESH_TYPE_7                       SOC_TMC_EGR_PORT_THRESH_TYPE_7
#define ARAD_EGR_PORT_THRESH_TYPE_8                       SOC_TMC_EGR_PORT_THRESH_TYPE_8
#define ARAD_EGR_PORT_THRESH_TYPE_9                       SOC_TMC_EGR_PORT_THRESH_TYPE_9
#define ARAD_EGR_PORT_THRESH_TYPE_10                      SOC_TMC_EGR_PORT_THRESH_TYPE_10
#define ARAD_EGR_PORT_THRESH_TYPE_11                      SOC_TMC_EGR_PORT_THRESH_TYPE_11
#define ARAD_EGR_PORT_THRESH_TYPE_12                      SOC_TMC_EGR_PORT_THRESH_TYPE_12
#define ARAD_EGR_PORT_THRESH_TYPE_13                      SOC_TMC_EGR_PORT_THRESH_TYPE_13
#define ARAD_EGR_PORT_THRESH_TYPE_14                      SOC_TMC_EGR_PORT_THRESH_TYPE_14
#define ARAD_EGR_PORT_THRESH_TYPE_15                      SOC_TMC_EGR_PORT_THRESH_TYPE_15
#define ARAD_EGR_PORT_NOF_THRESH_TYPES                    SOC_TMC_EGR_PORT_NOF_THRESH_TYPES_ARAD
typedef SOC_TMC_EGR_PORT_THRESH_TYPE                          ARAD_EGR_PORT_THRESH_TYPE;

#define ARAD_EGR_Q_PRIO_ALL                               SOC_TMC_EGR_Q_PRIO_ALL
#define ARAD_EGR_Q_PRIO_0                                 SOC_TMC_EGR_Q_PRIO_0
#define ARAD_EGR_Q_PRIO_1                                 SOC_TMC_EGR_Q_PRIO_1
#define ARAD_EGR_Q_PRIO_2                                 SOC_TMC_EGR_Q_PRIO_2
#define ARAD_EGR_Q_PRIO_3                                 SOC_TMC_EGR_Q_PRIO_3
#define ARAD_EGR_Q_PRIO_4                                 SOC_TMC_EGR_Q_PRIO_4
#define ARAD_EGR_Q_PRIO_5                                 SOC_TMC_EGR_Q_PRIO_5
#define ARAD_EGR_Q_PRIO_6                                 SOC_TMC_EGR_Q_PRIO_6
#define ARAD_EGR_Q_PRIO_7                                 SOC_TMC_EGR_Q_PRIO_7
#define ARAD_EGR_NOF_Q_PRIO                               SOC_TMC_EGR_NOF_Q_PRIO_ARAD
typedef SOC_TMC_EGR_Q_PRIO                                    ARAD_EGR_Q_PRIO;

typedef SOC_TMC_EGR_OFP_SCH_INFO                              ARAD_EGR_OFP_SCH_INFO;
typedef SOC_TMC_EGR_DROP_THRESH                               ARAD_EGR_DROP_THRESH;
typedef SOC_TMC_EGR_FC_DEVICE_THRESH                          ARAD_EGR_FC_DEVICE_THRESH;
typedef SOC_TMC_EGR_FC_CHNIF_THRESH                           ARAD_EGR_FC_CHNIF_THRESH;
typedef SOC_TMC_EGR_FC_OFP_THRESH                             ARAD_EGR_FC_OFP_THRESH;

#define ARAD_EGR_UCAST_TO_SCHED                           SOC_TMC_EGR_UCAST_TO_SCHED
#define ARAD_EGR_MCAST_TO_UNSCHED                         SOC_TMC_EGR_MCAST_TO_UNSCHED
#define ARAD_EGR_NOF_Q_PRIO_MAPPING_TYPES                 SOC_TMC_EGR_NOF_Q_PRIO_MAPPING_TYPES
typedef SOC_TMC_EGR_Q_PRIO_MAPPING_TYPE                       ARAD_EGR_Q_PRIO_MAPPING_TYPE;

#define ARAD_EGR_PORT_SHAPER_DATA_MODE                    SOC_TMC_EGR_PORT_SHAPER_DATA_MODE
#define ARAD_EGR_PORT_SHAPER_PACKET_MODE                  SOC_TMC_EGR_PORT_SHAPER_PACKET_MODE
#define ARAD_EGR_NOF_PORT_SHAPER_MODES                    SOC_TMC_EGR_NOF_PORT_SHAPER_MODES
typedef SOC_TMC_EGR_PORT_SHAPER_MODE                          ARAD_EGR_PORT_SHAPER_MODE;

#define ARAD_EGR_PORT_ONE_PRIORITY                        SOC_TMC_EGR_PORT_ONE_PRIORITY                          
#define ARAD_EGR_PORT_TWO_PRIORITIES                      SOC_TMC_EGR_PORT_TWO_PRIORITIES
#define ARAD_EGR_PORT_EIGHT_PRIORITIES                    SOC_TMC_EGR_PORT_EIGHT_PRIORITIES
#define ARAD_EGR_NOF_PORT_PRIORITY_MODES                  SOC_TMC_EGR_NOF_PORT_PRIORITY_MODES
typedef SOC_TMC_EGR_PORT_PRIORITY_MODE                        ARAD_EGR_PORT_PRIORITY_MODE;

typedef SOC_TMC_EGR_Q_PRIORITY                                ARAD_EGR_Q_PRIORITY;

typedef SOC_TMC_EGR_FC_DEV_THRESH_INNER                       ARAD_EGR_FC_DEV_THRESH_INNER;
typedef SOC_TMC_EGR_OFP_SCH_WFQ                               ARAD_EGR_OFP_SCH_WFQ;
typedef SOC_TMC_EGR_THRESH_INFO                               ARAD_EGR_THRESH_INFO;

typedef SOC_TMC_EGR_QUEUING_TCG_INFO                          ARAD_EGR_QUEUING_TCG_INFO;
typedef SOC_TMC_EGR_TCG_SCH_WFQ                               ARAD_EGR_TCG_SCH_WFQ;


#define ARAD_EGR_CGM_OTM_PORTS_NUM 256
#define ARAD_EGR_CGM_QUEUES_NUM 256
#define ARAD_EGR_CGM_IF_NUM SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES)

typedef struct
{
    
    uint16 pd;
    
    uint16 db;
    
    uint16 uc_pd;
    
    uint16 mc_pd;
    
    uint16 uc_db;
    
    uint16 mc_db;
    
    uint32 uc_pd_if[ARAD_EGR_CGM_IF_NUM];
    
    uint32 mc_pd_if[ARAD_EGR_CGM_IF_NUM];
    
    uint32 uc_size_256_if[ARAD_EGR_CGM_IF_NUM];
    
    uint32 mc_size_256_if[ARAD_EGR_CGM_IF_NUM];

    
    uint16 uc_pd_port[ARAD_EGR_CGM_OTM_PORTS_NUM];
    uint16 mc_pd_port[ARAD_EGR_CGM_OTM_PORTS_NUM];
    
    uint16 uc_pd_queue[ARAD_EGR_CGM_QUEUES_NUM];
    uint16 mc_pd_queue[ARAD_EGR_CGM_QUEUES_NUM];
    
    uint32 uc_db_port[ARAD_EGR_CGM_OTM_PORTS_NUM];
    
    uint32 mc_db_port[ARAD_EGR_CGM_OTM_PORTS_NUM];
    
    uint32 uc_db_queue[ARAD_EGR_CGM_QUEUES_NUM];
    
    uint32 mc_db_queue[ARAD_EGR_CGM_QUEUES_NUM];

    
    uint16 mc_pd_sp[2];
    
    uint16 mc_db_sp[2];
    
    uint16 mc_pd_sp_tc[16];
    
    uint16 mc_db_sp_tc[16];

    
    uint16 mc_rsvd_pd_sp[2]; 
    
    uint16 mc_rsvd_db_sp[2]; 

} ARAD_EGR_CGM_CONGENSTION_STATS;




typedef struct
{
    
    uint32 uc_pd_dropped;
    
    uint32 mc_rep_pd_dropped;
    
    uint32 uc_db_dropped_by_rqp;
    
    uint32 uc_db_dropped_by_pqp;
    
    uint32 mc_db_dropped;

    
    uint32 mc_rep_db_dropped;
} ARAD_EGR_CGM_CONGENSTION_COUNTERS;














uint32
  arad_egr_ofp_thresh_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID    tm_port,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_thresh_type
  );


uint32
  arad_egr_ofp_thresh_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_THRESH_TYPE *ofp_thresh_type
  );


uint32
  arad_egr_sched_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  );


uint32
  arad_egr_sched_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  );


uint32
  arad_egr_unsched_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  );

uint32
  arad_egr_sched_port_fc_thresh_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  int                threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH *thresh
  );
uint32
  arad_egr_sched_q_fc_thresh_set(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core,
    SOC_SAND_IN  int       prio,
    SOC_SAND_IN  int threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH  *thresh
  );


uint32
  arad_egr_unsched_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  );


int
  arad_egr_dev_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_FC_DEVICE_THRESH *thresh,
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *exact_thresh
  );


int
  arad_egr_dev_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *thresh
  );


int
  arad_egr_ofp_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  ARAD_EGR_FC_OFP_THRESH   *thresh,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *exact_thresh
  );


int
  arad_egr_ofp_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *thresh
  );


uint32
  arad_egr_ofp_scheduling_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_EGR_OFP_SCH_INFO    *info
  );


uint32
  arad_egr_ofp_scheduling_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_EGR_OFP_SCH_INFO    *info
  );


uint32
  arad_egr_dsp_pp_to_base_q_pair_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_OUT uint32                      *base_q_pair
  );


uint32
  arad_egr_dsp_pp_priorities_mode_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_PRIORITY_MODE    *priority_mode
  );


int
  arad_egr_queuing_dev_set(
	SOC_SAND_IN	 int	unit,
    SOC_SAND_IN  int    core,
	SOC_SAND_IN  ARAD_EGR_QUEUING_DEV_TH	*info
  );

int
  arad_egr_queuing_dev_get(
	SOC_SAND_IN	 int	unit,
    SOC_SAND_IN  int    core,
	SOC_SAND_OUT ARAD_EGR_QUEUING_DEV_TH	*info
  );

int
  arad_egr_queuing_global_drop_set(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core,
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_IN  int    threshold_value,
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  );

int
  arad_egr_queuing_global_drop_get(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core,
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_OUT int*    threshold_value,
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  );

int
  arad_egr_queuing_sp_tc_drop_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );

int
  arad_egr_queuing_sp_tc_drop_get(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
int
  arad_egr_queuing_sp_reserved_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
int
  arad_egr_queuing_sp_reserved_get(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  );
uint32 
  arad_egr_queuing_sch_unsch_drop_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_IN    SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  );

int
  arad_egr_queuing_global_fc_set(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_IN  int    threshold_value, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  );

int
  arad_egr_queuing_global_fc_get(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_OUT int*    threshold_value, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  );

int
  arad_egr_queuing_mc_tc_fc_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value
  );

int
  arad_egr_queuing_mc_tc_fc_get(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value
  );


int
  arad_egr_queuing_mc_cos_map_set(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	tc_ndx,
	SOC_SAND_IN	uint32	dp_ndx,
	SOC_SAND_IN ARAD_EGR_QUEUING_MC_COS_MAP	*info
  );

uint32	
  arad_egr_queuing_mc_cos_map_get(
	SOC_SAND_IN	 int	unit,
    SOC_SAND_IN  int    core,
	SOC_SAND_IN	 uint32	tc_ndx,
	SOC_SAND_IN	 uint32	dp_ndx,
	SOC_SAND_OUT ARAD_EGR_QUEUING_MC_COS_MAP	*info
  );


int
  arad_egr_queuing_if_fc_set(
	SOC_SAND_IN	 int	unit,
	SOC_SAND_IN	 ARAD_INTERFACE_ID	if_ndx,
	SOC_SAND_IN  ARAD_EGR_QUEUING_IF_FC	*info
  );

uint32
  arad_egr_queuing_if_fc_get(
	SOC_SAND_IN	 int	unit,
	SOC_SAND_IN	 ARAD_INTERFACE_ID	if_ndx,
	SOC_SAND_OUT ARAD_EGR_QUEUING_IF_FC	*info
  );


int
  arad_egr_queuing_if_fc_uc_max_set(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	uc_if_profile_ndx,
	SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC	*info
  );

int
  arad_egr_queuing_if_fc_uc_set(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	uc_if_profile_ndx,
	SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC	*info
  );

int
  arad_egr_queuing_if_fc_uc_get(
	SOC_SAND_IN	 int	unit,
    SOC_SAND_IN  int    core,
	SOC_SAND_IN	 uint32	uc_if_profile_ndx,
	SOC_SAND_OUT ARAD_EGR_QUEUING_IF_UC_FC	*info
  );



int
  arad_egr_queuing_if_fc_mc_set(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	mc_if_profile_ndx,
	SOC_SAND_IN uint32    pd_th
  );

int
  arad_egr_queuing_if_fc_mc_get(
	SOC_SAND_IN	 int	unit,
	SOC_SAND_IN	 uint32	mc_if_profile_ndx,
	SOC_SAND_OUT uint32   *pd_th
  );

uint32 
  arad_egr_queuing_if_uc_map_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  soc_port_if_t interface_type,
    SOC_SAND_IN  uint32     internal_if_id,
    SOC_SAND_IN  int        profile
  );

uint32 
  arad_egr_queuing_if_mc_map_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  soc_port_if_t interface_type,
    SOC_SAND_IN  uint32     internal_if_id,
    SOC_SAND_IN  int        profile
  );


uint32
  arad_egr_queuing_ofp_tcg_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  uint32                         tm_port,    
    SOC_SAND_IN  ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  );

uint32
  arad_egr_queuing_ofp_tcg_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               tm_port,    
    SOC_SAND_OUT ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  );


uint32
  arad_egr_queuing_tcg_weight_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID          tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  );

uint32
  arad_egr_queuing_tcg_weight_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  );


void
  arad_ARAD_EGR_Q_PRIORITY_clear(
    SOC_SAND_OUT ARAD_EGR_Q_PRIORITY *info
  );

void
  arad_ARAD_EGR_DROP_THRESH_clear(
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH *info
  );

void
  arad_ARAD_EGR_THRESH_INFO_clear(
    SOC_SAND_OUT ARAD_EGR_THRESH_INFO *info
  );

void
  arad_ARAD_EGR_QUEUING_TH_DB_GLOBAL_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_TH_DB_GLOBAL *info
  );

void
  arad_ARAD_EGR_QUEUING_TH_DB_POOL_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_TH_DB_POOL *info
  );

void
  arad_ARAD_EGR_QUEUING_TH_DB_PORT_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_TH_DB_PORT *info
  );

void
  arad_ARAD_EGR_QUEUING_DEV_TH_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_DEV_TH *info
  );

void
  arad_ARAD_EGR_QUEUING_MC_COS_MAP_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_MC_COS_MAP *info
  );

void
  arad_ARAD_EGR_QUEUING_IF_FC_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_IF_FC *info
  );

void
  arad_ARAD_EGR_QUEUING_IF_UC_FC_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_IF_UC_FC *info
  );

void
  arad_ARAD_EGR_FC_DEV_THRESH_INNER_clear(
    SOC_SAND_OUT ARAD_EGR_FC_DEV_THRESH_INNER *info
  );

void
  arad_ARAD_EGR_FC_DEVICE_THRESH_clear(
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *info
  );

void
  arad_ARAD_EGR_FC_CHNIF_THRESH_clear(
    SOC_SAND_OUT ARAD_EGR_FC_CHNIF_THRESH *info
  );

void
  arad_ARAD_EGR_FC_OFP_THRESH_clear(
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH *info
  );

void
  arad_ARAD_EGR_OFP_SCH_WFQ_clear(
    SOC_SAND_OUT ARAD_EGR_OFP_SCH_WFQ *info
  );

void
  arad_ARAD_EGR_OFP_SCH_INFO_clear(
    SOC_SAND_OUT ARAD_EGR_OFP_SCH_INFO *info
  );

void
  ARAD_EGR_QUEUING_TCG_INFO_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_TCG_INFO *info
  );

void
  ARAD_EGR_TCG_SCH_WFQ_clear(
    SOC_SAND_OUT ARAD_EGR_TCG_SCH_WFQ *info
  );


#if ARAD_DEBUG_IS_LVL1

void
  ARAD_EGR_Q_PRIORITY_print(
    SOC_SAND_IN  ARAD_EGR_Q_PRIORITY *info
  );

const char*
  arad_ARAD_EGR_Q_PRIO_to_string(
    SOC_SAND_IN ARAD_EGR_Q_PRIO enum_val
  );



const char*
  arad_ARAD_EGR_PORT_THRESH_TYPE_to_string(
    SOC_SAND_IN ARAD_EGR_PORT_THRESH_TYPE enum_val
  );



const char*
  arad_ARAD_EGR_OFP_INTERFACE_PRIO_to_string(
    SOC_SAND_IN ARAD_EGR_OFP_INTERFACE_PRIO enum_val
  );



const char*
  arad_ARAD_EGR_Q_PRIO_MAPPING_TYPE_to_string(
    SOC_SAND_IN ARAD_EGR_Q_PRIO_MAPPING_TYPE enum_val
  );



void
  arad_ARAD_EGR_DROP_THRESH_print(
    SOC_SAND_IN ARAD_EGR_DROP_THRESH *info
  );

void
  arad_ARAD_EGR_THRESH_INFO_print(
    SOC_SAND_IN ARAD_EGR_THRESH_INFO *info
  );

void
  arad_ARAD_EGR_FC_DEV_THRESH_INNER_print(
    SOC_SAND_IN ARAD_EGR_FC_DEV_THRESH_INNER *info
  );



void
  arad_ARAD_EGR_FC_DEVICE_THRESH_print(
    SOC_SAND_IN ARAD_EGR_FC_DEVICE_THRESH *info
  );


void
  arad_ARAD_EGR_FC_CHNIF_THRESH_print(
    SOC_SAND_IN ARAD_EGR_FC_CHNIF_THRESH *info
  );



void
  arad_ARAD_EGR_FC_OFP_THRESH_print(
    SOC_SAND_IN ARAD_EGR_FC_OFP_THRESH *info
  );



void
  arad_ARAD_EGR_OFP_SCH_WFQ_print(
    SOC_SAND_IN ARAD_EGR_OFP_SCH_WFQ *info
  );



void
  arad_ARAD_EGR_OFP_SCH_INFO_print(
    SOC_SAND_IN ARAD_EGR_OFP_SCH_INFO *info
  );

void
  ARAD_EGR_QUEUING_TCG_INFO_print(
    SOC_SAND_IN ARAD_EGR_QUEUING_TCG_INFO *info
  );

void
  ARAD_EGR_TCG_SCH_WFQ_print(
    SOC_SAND_IN ARAD_EGR_TCG_SCH_WFQ *info
  );

#endif 




#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

