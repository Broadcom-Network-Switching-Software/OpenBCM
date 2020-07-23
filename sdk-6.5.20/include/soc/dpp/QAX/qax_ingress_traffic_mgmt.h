/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_mgmt.h
 */

#ifndef __QAX_INGRESS_TRAFFIC_MGMT_INCLUDED__

#define __QAX_INGRESS_TRAFFIC_MGMT_INCLUDED__

#include <soc/dpp/ARAD/arad_tbl_access.h>


#define QAX_ITM_NOF_MAX_LATENCY_MEASUREMENTS  (8)

int
  qax_itm_init(
    SOC_SAND_IN  int  unit
  );

int
  qax_itm_per_queue_info_set(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          queue,
    SOC_SAND_IN   ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  );

int
  qax_itm_per_queue_info_get(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          queue,
    SOC_SAND_OUT   ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  );

int
  qax_itm_in_pp_port_scheduler_compensation_profile_set(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_IN   int  scheduler_profile
          );

int
  qax_itm_in_pp_port_scheduler_compensation_profile_get(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_OUT  int*  scheduler_profile
          );

int
  qax_itm_profile_ocb_only_set(
    SOC_SAND_IN   int unit,
    SOC_SAND_IN   int rate_class,
    SOC_SAND_IN   int is_ocb_only
  );

int
  qax_itm_profile_ocb_only_get(
    SOC_SAND_IN   int unit,
    SOC_SAND_IN   int rate_class,
    SOC_SAND_OUT  int *is_ocb_only
  );

int 
  qax_itm_congestion_statistics_get(
      SOC_SAND_IN int unit,
      SOC_SAND_IN int core,
      SOC_SAND_OUT ARAD_ITM_CGM_CONGENSTION_STATS *stats 
  );

int
  qax_itm_min_free_resources_stat_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN SOC_TMC_ITM_CGM_RSRC_STAT_TYPE type,
    SOC_SAND_OUT uint64 *value
  );

int
  qax_itm_dp_discard_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  discard_dp
  );

int
  qax_itm_dp_discard_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  );

int
  qax_itm_committed_q_size_set(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  uint32                  rt_cls_ndx,
      SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info, 
      SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
      );
int
  qax_itm_committed_q_size_get(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  uint32                  rt_cls_ndx,
      SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info
      );

int
  qax_itm_fadt_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_FADT_DROP_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO  *exact_info
  );

int
  qax_itm_fadt_tail_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO  *info
  );
int
  qax_itm_tail_drop_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *exact_info
          );

int
  qax_itm_tail_drop_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *info
          );

int
  qax_itm_wred_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  );

int
  qax_itm_wred_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  );

int
  qax_itm_wred_exp_wq_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq,
    SOC_SAND_IN  uint8                   enable
          );
int
  qax_itm_wred_exp_wq_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT  uint32                  *exp_wq
          );

int
  qax_itm_dyn_total_thresh_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN int    core,
    SOC_SAND_IN uint8  is_ocb_only,
                int32  reservation_increase_array[SOC_DPP_DEFS_MAX(NOF_CORES)][SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] 
          );
int
  qax_itm_queue_dyn_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_DYN_INFO *info
          );  

int
  qax_itm_dram_bound_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
                 SOC_TMC_ITM_DRAM_BOUND_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BOUND_INFO  *exact_info
          );

int
  qax_itm_dram_bound_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BOUND_INFO  *info
          );




int
  qax_itm_vsq_pg_tc_profile_mapping_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 src_pp_port,
    SOC_SAND_IN int pg_tc_profile
  );

int
  qax_itm_vsq_pg_tc_profile_mapping_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 src_pp_port,
    SOC_SAND_OUT int *pg_tc_profile
  );

int
  qax_itm_vsq_pg_tc_profile_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN int pg_tc_profile_id,
    SOC_SAND_IN uint32 pg_tc_bitmap
  );

int
  qax_itm_vsq_pg_tc_profile_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core_id,
    SOC_SAND_IN int         pg_tc_profile_id,
    SOC_SAND_OUT uint32     *pg_tc_bitmap
  );

int
  qax_itm_vsq_pb_prm_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              pg_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_PRM *pg_prm
  );

int
  qax_itm_vsq_pb_prm_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              pg_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_PRM *pg_prm
  );

int
  qax_itm_src_vsqs_mapping_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN int src_pp_port,
    SOC_SAND_IN int src_port_vsq_index,
    SOC_SAND_IN int pg_base,
    SOC_SAND_IN uint8 enable
  );

int
  qax_itm_src_vsqs_mapping_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core_id,
    SOC_SAND_IN  int src_pp_port,
    SOC_SAND_OUT int *src_port_vsq_index,
    SOC_SAND_OUT int *pg_base,
    SOC_SAND_OUT uint8 *enable
  );

int
  qax_itm_vsq_wred_get(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
      SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
      SOC_SAND_IN  uint32                 drop_precedence_ndx,
      SOC_SAND_IN  int                    pool_id,
      SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info
  );

int
  qax_itm_vsq_wred_set(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
      SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
      SOC_SAND_IN  uint32                 drop_precedence_ndx,
      SOC_SAND_IN  int                    pool_id,
      SOC_SAND_IN  SOC_TMC_ITM_WRED_QT_DP_INFO *info,
      SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *exact_info
  );

int
  qax_itm_vsq_wred_gen_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  );

int
  qax_itm_vsq_wred_gen_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  );

int
  qax_itm_vsq_tail_drop_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  );

int
  qax_itm_vsq_fc_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO  *info
  );

int
  qax_itm_vsq_tail_drop_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  );

int
  qax_itm_vsq_tail_drop_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  );

int
  qax_itm_vsq_src_port_rjct_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_SRC_PORT_INFO       *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_SRC_PORT_INFO       *exact_info
  );

int
  qax_itm_vsq_src_port_rjct_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_SRC_PORT_INFO       *exact_info
  );

int
  qax_itm_vsq_pg_rjct_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                     drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_INFO    *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_INFO    *exact_info
  );

int
  qax_itm_vsq_pg_rjct_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                     drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_INFO    *exact_info
  );

int
  qax_itm_vsq_qt_rt_cls_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX    vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  );

int
  qax_itm_vsq_qt_rt_cls_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX    vsq_in_group_ndx,
    SOC_SAND_OUT uint32                 *vsq_rt_cls
  );

int
  qax_itm_vsq_src_port_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN int    core_id,
    SOC_SAND_IN int    src_port_vsq_index,
    SOC_SAND_OUT uint32 *src_pp_port,
    SOC_SAND_OUT uint8  *enable
  );

int
  qax_itm_vsq_pg_mapping_get(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN int     core_id,
    SOC_SAND_IN uint32  pg_vsq_base,
    SOC_SAND_IN int     cosq,
    SOC_SAND_OUT uint32 *src_pp_port,
    SOC_SAND_OUT uint8  *enable
  );

int 
  qax_itm_category_rngs_set( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info 
  ); 

int 
  qax_itm_category_rngs_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_CATEGORY_RNGS *info 
  ); 

int
  qax_itm_glob_rcs_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_DROP_TH *exact_info
  );

int
  qax_itm_glob_rcs_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  );

int
  qax_itm_vsq_fc_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_FC_INFO    *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO    *exact_info
  );

int
  qax_itm_vsq_fc_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO    *info
  );

int
  qax_itm_global_resource_allocation_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core, 
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  );

int
  qax_itm_global_resource_allocation_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core, 
    SOC_SAND_IN SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  );

int qax_itm_credits_adjust_size_set (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_IN int   delta,
    SOC_SAND_IN SOC_TMC_ITM_PKT_SIZE_ADJUST_INFO* additional_info
        );

int qax_itm_credits_adjust_size_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_OUT int   *delta
        ); 

int
  qax_ingress_drop_status(
    SOC_SAND_IN int   unit,
    SOC_SAND_OUT uint32 *is_max_size 
  );

int qax_itm_ingress_latency_init(
    SOC_SAND_IN  int  unit
        );
int qax_itm_max_latency_pkts_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int max_count,
    SOC_SAND_OUT SOC_TMC_MAX_LATENCY_PACKETS_INFO *max_latency_packets,
    SOC_SAND_OUT int *actual_count
        );


int
qax_itm_sys_red_queue_size_boundaries_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info
    );


int
  qax_itm_sys_red_queue_size_boundaries_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
      );


int
qax_itm_sys_red_q_based_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
    );


int
qax_itm_sys_red_q_based_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_DP_INFO *info
    );


int
qax_itm_sys_red_glob_rcs_set(
        SOC_SAND_IN  int                 unit,
        SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info);


int
qax_itm_sys_red_glob_rcs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
    );

#endif 

