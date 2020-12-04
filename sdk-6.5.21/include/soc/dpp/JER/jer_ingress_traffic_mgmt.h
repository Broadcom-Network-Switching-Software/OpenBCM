/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __JER_INGRESS_TRAFFIC_MGMT_INCLUDED__

#define __JER_INGRESS_TRAFFIC_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_framework.h> 
#include <soc/error.h> 
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_api_ingress_traffic_mgmt.h> 









int
  jer_itm_init(
    SOC_SAND_IN  int  unit
  );

uint32
  jer_itm_enable_ecn_set(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled 
  );


uint32
  jer_itm_enable_ecn_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled 
  );

 
uint32 
  jer_itm_ingress_shape_set( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  ); 
 
 
uint32 
  jer_itm_ingress_shape_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  ); 
 
 
int 
  jer_itm_category_rngs_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info
  );
 
int 
  jer_itm_category_rngs_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_CATEGORY_RNGS *info 
  );

uint32
  jer_itm_admit_test_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

uint32
  jer_itm_admit_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              admt_tst_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

int jer_itm_ocb_fadt_enable_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int  enable
    );

int jer_itm_ocb_fadt_enable_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT int*  enable
    );

int jer_itm_ocb_fadt_drop_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_OCB_FADT_DROP_THRESHOLD  *info,
    SOC_SAND_OUT SOC_TMC_ITM_OCB_FADT_DROP_THRESHOLD  *exact_info
    );

int jer_itm_ocb_fadt_drop_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_OCB_FADT_DROP_THRESHOLD  *info
    );

int
  jer_itm_vsq_wred_gen_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  );
int
  jer_itm_vsq_wred_gen_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  );
int
    jer_itm_vsq_wred_get(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_IN  int                    pool_id,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info);

int
    jer_itm_vsq_wred_set(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_IN  int                    pool_id,
        SOC_SAND_IN  SOC_TMC_ITM_WRED_QT_DP_INFO *info, 
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *exact_info);
int
  jer_itm_vsq_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  );
int
  jer_itm_vsq_tail_drop_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  );
int
  jer_itm_vsq_tail_drop_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  );
int
  jer_itm_vsq_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO     *exact_info
  );
int
  jer_itm_vsq_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO     *info
  );

int
  jer_itm_vsq_fc_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO  *info
  );


uint32 
  jer_itm_setup_dp_map(
    SOC_SAND_IN  int unit
  );
int
jer_itm_vsq_src_reserve_set(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP   vsq_type,
   SOC_SAND_IN  uint32                  vsq_rt_cls_ndx,
   SOC_SAND_IN  uint32                  drop_precedence_ndx,
   SOC_SAND_IN  int                     pool_id,
   SOC_SAND_IN  uint32                  reserved_amount,
   SOC_SAND_OUT uint32*                 exact_reserved_amount
   );
int
jer_itm_vsq_src_reserve_get(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP   vsq_type,
   SOC_SAND_IN  uint32                  vsq_rt_cls_ndx,
   SOC_SAND_IN  uint32                  drop_precedence_ndx,
   SOC_SAND_IN  int                     pool_id,
   SOC_SAND_OUT uint32                  *reserved_amount
   );
int
jer_itm_resource_allocation_set(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  uint8                   pool_id,
   SOC_SAND_IN  uint8                   is_ocb_only,
   SOC_SAND_IN  uint32                  max_shared_pool,
   SOC_SAND_IN  uint32                  max_headroom
   );
int
jer_itm_resource_allocation_get(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  uint8                   pool_id,
   SOC_SAND_IN  uint8                   is_ocb_only,
   SOC_SAND_OUT uint32                  *max_shared_pool,
   SOC_SAND_OUT uint32                  *max_headroom
   );
int
  jer_itm_global_resource_allocation_get(
      SOC_SAND_IN  int unit, 
      SOC_SAND_IN  int core, 
      SOC_SAND_OUT  SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  );
int
  jer_itm_global_resource_allocation_set(
      SOC_SAND_IN  int unit, 
      SOC_SAND_IN  int core, 
      SOC_SAND_IN  SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  );

int
  jer_itm_dyn_total_thresh_set(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  int      core_id,
    SOC_SAND_IN  uint8    is_ocb_only,
                 int32    reservation_increase[SOC_DPP_DEFS_MAX(NOF_CORES)][SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] 
  );

int
  jer_ingress_drop_status(
    SOC_SAND_IN int     unit,
    SOC_SAND_OUT uint32 *is_max_size 
  );

int
jer_itm_vsq_pg_tc_profile_mapping_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 src_pp_port,
    SOC_SAND_IN int pg_tc_profile
   );

int
jer_itm_vsq_pg_tc_profile_mapping_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 src_pp_port,
    SOC_SAND_OUT int *pg_tc_profile
   );

int
jer_itm_vsq_pg_tc_profile_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN int pg_tc_profile_id,
    SOC_SAND_IN uint32 pg_tc_bitmap
   );

int
jer_itm_vsq_pg_tc_profile_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core_id,
    SOC_SAND_IN int         pg_tc_profile_id,
    SOC_SAND_OUT uint32     *pg_tc_bitmap
   );
int
  jer_itm_vsq_pg_prm_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_PRM *info
  );
int
  jer_itm_vsq_pg_prm_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              vsq_rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_PRM *info
  );

int
jer_itm_vsq_pg_fadt_fc_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int              fadt_enable
    );

int
jer_itm_vsq_pg_fadt_fc_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT int                *fadt_enable
    );

int
jer_itm_src_vsqs_mapping_set(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int src_port_vsq_index,
       SOC_SAND_IN int src_pp_port,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN uint8 enable
       );
int
jer_itm_src_vsqs_mapping_get(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int src_pp_port,
       SOC_SAND_OUT int *src_port_vsq_index,
       SOC_SAND_OUT int *pg_base,
       SOC_SAND_OUT uint8 *enable
       );
int
    jer_itm_vsq_src_port_get(
       SOC_SAND_IN int    unit,
       SOC_SAND_IN int    core_id,
       SOC_SAND_IN int    src_port_vsq_index,
       SOC_SAND_OUT uint32 *src_pp_port,
       SOC_SAND_OUT uint8  *enable
       );
int
jer_itm_vsq_pg_mapping_get(
   SOC_SAND_IN int     unit,
   SOC_SAND_IN int     core_id,
   SOC_SAND_IN uint32  pg_vsq_base,
   SOC_SAND_IN int     cosq,
   SOC_SAND_OUT uint32 *src_pp_port,
   SOC_SAND_OUT uint8  *enable
   );
uint32
jer_itm_vsq_pg_ocb_set(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN int numq,
       SOC_SAND_IN uint8* ocb_only
       );
uint32
jer_itm_vsq_pg_ocb_get(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN int numq,
       SOC_SAND_OUT uint8* ocb_only
       );
int
jer_itm_glob_rcs_drop_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core_id,
    SOC_SAND_OUT  SOC_TMC_ITM_GLOB_RCS_DROP_TH  *info
    );
int
  jer_itm_glob_rcs_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_DROP_TH *exact_info
  );
int
  jer_itm_sys_red_queue_size_boundaries_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info
  );
int
  jer_itm_sys_red_queue_size_boundaries_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  );
int
    jer_itm_sys_red_glob_rcs_set(
        SOC_SAND_IN  int                 unit,
        SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );
int
  jer_itm_sys_red_glob_rcs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );

int
  jer_itm_in_pp_port_scheduler_compensation_profile_set(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_IN   int  scheduler_profile
          );

int
  jer_itm_in_pp_port_scheduler_compensation_profile_get(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_OUT  int*  scheduler_profile
          );

int jer_itm_credits_adjust_size_set (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_IN int   delta,
    SOC_SAND_IN SOC_TMC_ITM_PKT_SIZE_ADJUST_INFO* additional_info
  );

int jer_itm_credits_adjust_size_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_OUT int   *delta
  );

int jer_itm_sched_compensation_offset_and_delta_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   queue_profile,
    SOC_SAND_IN int   queue_delta, 
    SOC_SAND_IN int   in_pp_port_profile,
    SOC_SAND_IN int   in_pp_port_delta, 
    SOC_SAND_OUT int* offset,
    SOC_SAND_OUT int* delta
        ); 
 
int jer_itm_sch_final_delta_map_set (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   delta,
    SOC_SAND_IN uint32   delta_profile
  );

int jer_itm_sch_final_delta_map_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   delta,
    SOC_SAND_OUT uint32   *delta_profile
  );

int jer_itm_sch_final_delta_set (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN uint32   delta_profile,
    SOC_SAND_IN int   final_delta
  );

int jer_itm_sch_final_delta_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN uint32   delta_profile,
    SOC_SAND_OUT int   *final_delta
  );

int 
  jer_itm_congestion_statistics_get(
      SOC_SAND_IN int unit,
      SOC_SAND_IN int core,
      SOC_SAND_OUT ARAD_ITM_CGM_CONGENSTION_STATS *stats 
  );

int
  jer_itm_min_free_resources_stat_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN SOC_TMC_ITM_CGM_RSRC_STAT_TYPE type,
    SOC_SAND_OUT uint64 *value
  );

int jer_itm_ingress_latency_init(int unit);
int jer_itm_max_latency_pkts_get(int unit, int max_count, SOC_TMC_MAX_LATENCY_PACKETS_INFO *max_latency_packets, int *actual_count);

int jer_itm_rate_limit_mpps_set(int unit, int rate);
int jer_itm_rate_limit_mpps_get(int unit, int* rate);

soc_error_t jer_itm_queue_flush_set(int unit, int core_id, int queue_id, int timeout);


#include <soc/dpp/SAND/Utils/sand_footer.h> 
 
#endif
