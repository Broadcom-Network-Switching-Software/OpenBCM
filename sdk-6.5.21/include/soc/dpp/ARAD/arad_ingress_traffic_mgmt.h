/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_INGRESS_TRAFFIC_MGMT_INCLUDED__

#define __ARAD_INGRESS_TRAFFIC_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/ARAD/arad_api_ingress_traffic_mgmt.h>






#define ARAD_ITM_HUNGRY_TH_MULTIPLIER_OFFSET  4

  
#define  ARAD_ITM_QT_CC_CLS_MAX              31

  
#define ARAD_ITM_IPS_QT_MAX(unit)            (SOC_DPP_DEFS_GET(unit, nof_credit_request_profiles) - 1)

  
#define  ARAD_ITM_QT_CR_CLS_MAX              (ARAD_ITM_NOF_CR_DISCNT_CLS_NDXS - 1)

  
#define  ARAD_ITM_QT_RT_CLS_MAX              63


#define ARAD_ITM_WQ_MAX                      31

#define ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO 33333 
#define ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO_AGGRESSIVE_WD_STATUS_MSG 1000 

#define ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO_PER_MODE { \
  ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO, \
  ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO, \
  ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO_AGGRESSIVE_WD_STATUS_MSG, \
  ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS / 2000}


#define  ARAD_ITM_DISCARD_DP_MAX              4

#define ARAD_ITM_GRNT_BYTES_MAX                                  (256*1024*1024)
#define ARAD_ITM_GRNT_BDS_MAX                                    (2*1024*1024)
#define ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED                        0xffffff
#define ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS                   8
#define ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS                   4
#define ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX               (128*1024*1024)










typedef enum
{
  ARAD_ITM_FWD_ACTION_TYPE_FWD = 0,
  ARAD_ITM_FWD_ACTION_TYPE_SNOOP = 1,
  ARAD_ITM_FWD_ACTION_TYPE_INBND_MIRR = 2,
  ARAD_ITM_FWD_ACTION_TYPE_OUTBND_MIRR = 3,
  ARAD_ITM_NOF_FWD_ACTION_TYPES = 4
}ARAD_ITM_FWD_ACTION_TYPE;

typedef struct soc_dpp_guaranteed_q_resource_s {
    soc_dpp_guaranteed_pair_t dram;
    soc_dpp_guaranteed_pair_t ocb;
} soc_dpp_guaranteed_q_resource_t;












uint32
  arad_itm_init(
    SOC_SAND_IN  int  unit
  );


uint32
  arad_ingress_traffic_mgmt_init(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_itm_dram_buffs_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs
  );


uint32
  arad_itm_dram_buffs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs
  );


uint32
  arad_itm_glob_rcs_fc_set_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   ARAD_ITM_GLOB_RCS_FC_TH  *info,
    SOC_SAND_OUT  ARAD_ITM_GLOB_RCS_FC_TH  *exact_info
  );


uint32
  arad_itm_glob_rcs_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_FC_TH  *info
  );


uint32
  arad_itm_glob_rcs_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_FC_TH  *info
  );


uint32
  arad_itm_glob_rcs_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *exact_info
  );


uint32
  arad_itm_glob_rcs_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info
  );


uint32
  arad_itm_glob_rcs_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *info
  );


uint32
  arad_itm_category_rngs_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info
  );


uint32
  arad_itm_category_rngs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info
  );


uint32
  arad_itm_category_rngs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_CATEGORY_RNGS *info
  );


uint32
  arad_itm_admit_test_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );


uint32
  arad_itm_admit_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );


uint32
  arad_itm_admit_test_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );


uint32
  arad_itm_cr_request_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_INFO *info,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *exact_info
  );


uint32
  arad_itm_cr_request_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_INFO *info
  );


uint32
  arad_itm_cr_request_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *info
  );


uint32
  arad_itm_cr_discount_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_DISCOUNT_INFO *info
  );


uint32
  arad_itm_cr_discount_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_DISCOUNT_INFO *info
  );


uint32
  arad_itm_cr_discount_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_DISCOUNT_INFO *info
  );


uint32
  arad_itm_queue_test_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TSTS      test_tmplt
  );


uint32
  arad_itm_queue_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TSTS      test_tmplt
  );


uint32
  arad_itm_queue_test_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TSTS      *test_tmplt
  );


uint32
  arad_itm_wred_exp_wq_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq,
    SOC_SAND_IN  uint8                   enable
  );


uint32
  arad_itm_wred_exp_wq_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq
  );


uint32
  arad_itm_wred_exp_wq_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT uint32                  *exp_wq
  );


uint32
  arad_itm_wred_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  );


uint32
  arad_itm_wred_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info
  );


uint32
  arad_itm_wred_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  );


uint32
  arad_itm_tail_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *exact_info
  );


uint32
  arad_itm_tail_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info
  );


uint32
  arad_itm_tail_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *info
  );


uint32
  arad_itm_cr_wd_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  ARAD_ITM_CR_WD_INFO      *info,
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *exact_info
  );


uint32
  arad_itm_cr_wd_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  ARAD_ITM_CR_WD_INFO      *info
  );


uint32
  arad_itm_cr_wd_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *info
  );



uint32
  arad_itm_enable_ecn_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled 
  );


uint32
  arad_itm_get_ecn_enabled_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled 
  );



uint32
  arad_itm_vsq_qt_rt_cls_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  );


uint32
  arad_itm_vsq_qt_rt_cls_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  );


uint32
  arad_itm_vsq_qt_rt_cls_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_OUT uint32                 *vsq_rt_cls
  );


uint32
  arad_itm_vsq_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *exact_info
  );


uint32
  arad_itm_vsq_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info
  );


uint32
  arad_itm_vsq_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *info
  );


uint32
  arad_itm_vsq_tail_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_TAIL_DROP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO *exact_info
  );


uint32
  arad_itm_vsq_tail_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  );


uint32
  arad_itm_vsq_tail_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  );


uint32
  arad_itm_vsq_tail_drop_get_default_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  );


uint32
  arad_itm_vsq_wred_gen_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );


uint32
  arad_itm_vsq_wred_gen_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );


uint32
  arad_itm_vsq_wred_gen_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );

uint32
  arad_itm_vsq_wred_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  );


uint32
  arad_itm_vsq_wred_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info
  );


uint32
  arad_itm_vsq_wred_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  );
uint32
  arad_itm_man_exp_buffer_set(
    SOC_SAND_IN  int32  value,
    SOC_SAND_IN  uint32 mnt_lsb,
    SOC_SAND_IN  uint32 mnt_nof_bits,
    SOC_SAND_IN  uint32 exp_lsb,
    SOC_SAND_IN  uint32 exp_nof_bits,
    SOC_SAND_IN  uint8 is_signed,
    SOC_SAND_OUT uint32  *output_field,
    SOC_SAND_OUT int32  *exact_value
  );
uint32
  arad_itm_man_exp_buffer_get(
    SOC_SAND_IN  uint32  buffer,
    SOC_SAND_IN  uint32 mnt_lsb,
    SOC_SAND_IN  uint32 mnt_nof_bits,
    SOC_SAND_IN  uint32 exp_lsb,
    SOC_SAND_IN  uint32 exp_nof_bits,
    SOC_SAND_IN  uint8 is_signed,
    SOC_SAND_OUT int32  *value
  );
uint32
    arad_itm_vsq_in_group_size_get(
       SOC_SAND_IN  int                      unit, 
       SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx, 
       SOC_SAND_OUT uint32                   *vsq_in_group_size);
uint32
  arad_itm_vsq_idx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                    is_cob_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  );


uint32
  arad_itm_vsq_counter_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_cob_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  );



uint32
  arad_itm_vsq_counter_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_cob_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  );



uint32
  arad_itm_vsq_counter_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT uint8                    *is_cob_only,
    SOC_SAND_OUT ARAD_ITM_VSQ_GROUP       *vsq_group_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_NDX         *vsq_in_group_ndx
  );



uint32
  arad_itm_vsq_counter_read_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT uint32                  *pckt_count
  );



uint32
  arad_itm_queue_is_ocb_only_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              queue_ndx,
     SOC_SAND_OUT uint8               *enable
     );

uint32
  arad_itm_queue_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *old_info,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *info
  );


uint32
  arad_itm_queue_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *info
  );


uint32
  arad_itm_queue_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT ARAD_ITM_QUEUE_INFO      *info
  );


uint32
  arad_itm_queue_dyn_info_get_unsafe(
	SOC_SAND_IN  int 				unit,
    SOC_SAND_IN  int                 core,
	SOC_SAND_IN  uint32 				 queue_ndx,
	SOC_SAND_OUT ARAD_ITM_QUEUE_DYN_INFO	  *info
  );

int
  arad_ingress_drop_status(
    SOC_SAND_IN int   unit,
    SOC_SAND_OUT uint32 *is_max_size 
  );


uint32
  arad_itm_dyn_total_thresh_set_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  int32    reservation_increase 
  );


uint32
  arad_itm_ingress_shape_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info
  );


uint32
  arad_itm_ingress_shape_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info
  );


uint32
  arad_itm_ingress_shape_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_INFO *info
  );


uint32
  arad_itm_priority_map_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );


uint32
  arad_itm_priority_map_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );


uint32
  arad_itm_priority_map_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_OUT ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );


uint32
  arad_itm_priority_map_tmplt_select_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  );


uint32
  arad_itm_priority_map_tmplt_select_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  );


uint32
  arad_itm_priority_map_tmplt_select_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_OUT uint32                 *priority_map
  );


uint32
  arad_itm_sys_red_drop_prob_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_DROP_PROB *info
  );


uint32
  arad_itm_sys_red_drop_prob_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_DROP_PROB *info
  );


uint32
  arad_itm_sys_red_drop_prob_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_DROP_PROB *info
  );


uint32
  arad_itm_sys_red_queue_size_boundaries_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info
  );

uint32
  x_itm_sys_red_queue_size_boundaries_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info,
    SOC_SAND_IN int mnt_lsb,
    SOC_SAND_IN int mnt_nof_bits,
    SOC_SAND_IN int exp_lsb,
    SOC_SAND_IN int exp_nof_bits
  );

uint32
  arad_itm_sys_red_queue_size_boundaries_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info
  );


uint32
  arad_itm_sys_red_queue_size_boundaries_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  );
uint32
  x_itm_sys_red_queue_size_boundaries_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_IN int mnt_lsb,
    SOC_SAND_IN int mnt_nof_bits,
    SOC_SAND_IN int exp_lsb,
    SOC_SAND_IN int exp_nof_bits
  );


uint32
  arad_itm_sys_red_q_based_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );


uint32
  arad_itm_sys_red_q_based_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );


uint32
  arad_itm_sys_red_q_based_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );


uint32
  arad_itm_sys_red_eg_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_EG_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *exact_info
  );


uint32
  arad_itm_sys_red_eg_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_EG_INFO *info
  );


uint32
  arad_itm_sys_red_eg_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *info
  );


uint32
  arad_itm_sys_red_glob_rcs_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );


uint32
  arad_itm_sys_red_glob_rcs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );


uint32
  arad_itm_sys_red_glob_rcs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );




uint32
  arad_itm_convert_admit_one_test_tmplt_to_u32(
    SOC_SAND_IN ARAD_ITM_ADMIT_ONE_TEST_TMPLT test,
    SOC_SAND_OUT uint32                       *test_in_sand_u32
  );

uint32
  arad_itm_convert_u32_to_admit_one_test_tmplt(
    SOC_SAND_IN  uint32                       test_in_sand_u32,
    SOC_SAND_OUT ARAD_ITM_ADMIT_ONE_TEST_TMPLT *test
  );

uint32
  arad_itm_dbuff_internal2size(
    SOC_SAND_IN  uint32                   dbuff_size_internal,
    SOC_SAND_OUT ARAD_ITM_DBUFF_SIZE_BYTES *dbuff_size
  );

uint32
  arad_itm_dbuff_size2internal(
    SOC_SAND_IN  ARAD_ITM_DBUFF_SIZE_BYTES dbuff_size,
    SOC_SAND_OUT uint32                   *dbuff_size_internal
  );


uint32
  arad_b_itm_glob_rcs_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info
  );

uint32
    arad_b_itm_glob_rcs_drop_set_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_THRESH_WITH_HYST_INFO mem_size[ARAD_NOF_DROP_PRECEDENCE],
      SOC_SAND_OUT ARAD_THRESH_WITH_HYST_INFO exact_mem_size[ARAD_NOF_DROP_PRECEDENCE]
    );

uint32
  arad_b_itm_glob_rcs_drop_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT ARAD_THRESH_WITH_HYST_INFO *mem_size
  );


uint32
  arad_itm_committed_q_size_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  );

uint32
  arad_itm_committed_q_size_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info
  );

uint32
  arad_itm_committed_q_size_get_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        rt_cls_ndx
  );

uint32
  arad_itm_committed_q_size_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  );


soc_error_t
arad_itm_pfc_tc_map_set_unsafe(const int unit, const int tc_in, const int port_id, const int tc_out);

soc_error_t
arad_itm_pfc_tc_map_get_unsafe(const int unit, const int tc_in, const int port_id, int *tc_out); 


int
  arad_itm_dp_discard_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  discard_dp
  );

uint32
  arad_itm_dp_discard_set_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        discard_dp
  );


int
  arad_itm_dp_discard_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  );


uint32 
  arad_itm_setup_dp_map(
    SOC_SAND_IN  int unit
  );


uint32
  arad_plus_itm_alpha_set_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       rt_cls_ndx,
    SOC_SAND_IN  uint32       drop_precedence_ndx,
    SOC_SAND_IN  int32        alpha 
  );


uint32
  arad_plus_itm_alpha_get_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       rt_cls_ndx,
    SOC_SAND_IN  uint32       drop_precedence_ndx,
    SOC_SAND_OUT int32        *alpha 
  );


uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_set_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint8    enabled 
  );


uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_get_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint8    *enabled 
  );


int arad_itm_congestion_statistics_get(
  SOC_SAND_IN int unit,
  SOC_SAND_IN int core,
  SOC_SAND_OUT ARAD_ITM_CGM_CONGENSTION_STATS *stats 
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

