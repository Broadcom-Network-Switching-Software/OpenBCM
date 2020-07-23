/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_INGRESS_TRAFFIC_MGMT_INCLUDED__

#define __ARAD_API_INGRESS_TRAFFIC_MGMT_INCLUDED__



#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>







#define  ARAD_ITM_NOF_DRAM_BUFFS (SOC_TMC_ITM_NOF_DRAM_BUFFS)


#define  ARAD_ITM_DBUFF_FMC_MAX (64 * 1024)


#define  ARAD_ITM_DBUFF_MMC_MAX (SOC_TMC_ITM_DBUFF_MMC_MAX)


#define ARAD_ITM_DBUFF_CACHE 3500

#define ARAD_ITM_NOF_VSQS 356



#define ARAD_ITM_PRIO_MAP_BIT_SIZE       64
#define ARAD_ITM_PRIO_MAP_SIZE_IN_UINT32S  2
#define ARAD_ITM_PRIO_NOF_SEGMENTS(unit)       SOC_SAND_DIV_ROUND_UP(SOC_DPP_DEFS_GET(unit, nof_queues), ARAD_ITM_PRIO_MAP_BIT_SIZE)


#define ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION 16


#define  ARAD_ITM_DBUFF_UC_MAX ARAD_ITM_NOF_DRAM_BUFFS


#define ARAD_ITM_SYS_RED_DROP_P_VAL      100
#define ARAD_ITM_SYS_RED_Q_SIZE_RANGES   16
#define ARAD_ITM_SYS_RED_DRP_PROBS       16
#define ARAD_ITM_SYS_RED_BUFFS_RNGS      4

  
#define ARAD_ITM_VSQ_QT_RT_CLS_MAX  SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)

#define ARAD_ITM_RATE_CLASS_MAX    SOC_TMC_ITM_RATE_CLASS_MAX
#define ARAD_NOF_VSQ_GROUPS        SOC_TMC_NOF_VSQ_GROUPS

#define ARAD_ITM_VSQ_GROUP_CTGRY_SIZE                       SOC_TMC_ITM_VSQ_GROUPA_SZE(unit) 
#define ARAD_ITM_VSQ_GROUP_CTGRY_OFFSET                     ARAD_ITM_VSQ_GROUP_CTGRY_SIZE
#define ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_SIZE           SOC_TMC_ITM_VSQ_GROUPB_SZE(unit) 
#define ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_OFFSET         (ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_SIZE + ARAD_ITM_VSQ_GROUP_CTGRY_OFFSET)
#define ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_SIZE         SOC_TMC_ITM_VSQ_GROUPC_SZE(unit) 
#define ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_OFFSET       (ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_SIZE + ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_OFFSET)
#define ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_SIZE                 SOC_TMC_ITM_VSQ_GROUPD_SZE(unit)
#define ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_OFFSET               (ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_SIZE + ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_OFFSET)
#define ARAD_ITM_VSQ_GROUP_LLFC_SIZE                        SOC_TMC_ITM_VSQ_GROUPE_SZE(unit)
#define ARAD_ITM_VSQ_GROUP_LLFC_OFFSET                      (ARAD_ITM_VSQ_GROUP_LLFC_SIZE + ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_OFFSET)
#define ARAD_ITM_VSQ_GROUP_PFC_SIZE                         SOC_TMC_ITM_VSQ_GROUPF_SZE(unit)
#define ARAD_ITM_VSQ_GROUP_PFC_OFFSET                       (ARAD_ITM_VSQ_GROUP_PFC_SIZE + ARAD_ITM_VSQ_GROUP_LLFC_OFFSET)


#define ARAD_ITM_CR_WD_Q_TH_MIN_MSEC             100
#define ARAD_ITM_CR_WD_Q_TH_MAX_MSEC             500

#define ARAD_ITM_VSQ_GROUP_LAST          SOC_TMC_ITM_VSQ_GROUP_LAST_ARAD
#define ARAD_ITM_ADMIT_TSTS_LAST         SOC_TMC_ITM_ADMIT_TSTS_LAST

#define ARAD_ITM_DBUFF_SIZE_BYTES_MIN        SOC_TMC_ITM_DBUFF_SIZE_BYTES_MIN
#define ARAD_ITM_DBUFF_SIZE_BYTES_MAX        SOC_TMC_ITM_DBUFF_SIZE_BYTES_MAX










#define ARAD_ITM_QT_NDX_00                                SOC_TMC_ITM_QT_NDX_00
#define ARAD_ITM_QT_NDX_01                                SOC_TMC_ITM_QT_NDX_01
#define ARAD_ITM_QT_NDX_02                                SOC_TMC_ITM_QT_NDX_02
#define ARAD_ITM_QT_NDX_03                                SOC_TMC_ITM_QT_NDX_03
#define ARAD_ITM_QT_NDX_04                                SOC_TMC_ITM_QT_NDX_04
#define ARAD_ITM_QT_NDX_05                                SOC_TMC_ITM_QT_NDX_05
#define ARAD_ITM_QT_NDX_06                                SOC_TMC_ITM_QT_NDX_06
#define ARAD_ITM_QT_NDX_07                                SOC_TMC_ITM_QT_NDX_07
#define ARAD_ITM_QT_NDX_08                                SOC_TMC_ITM_QT_NDX_08
#define ARAD_ITM_QT_NDX_09                                SOC_TMC_ITM_QT_NDX_09
#define ARAD_ITM_QT_NDX_10                                SOC_TMC_ITM_QT_NDX_10
#define ARAD_ITM_QT_NDX_11                                SOC_TMC_ITM_QT_NDX_11
#define ARAD_ITM_QT_NDX_12                                SOC_TMC_ITM_QT_NDX_12
#define ARAD_ITM_QT_NDX_13                                SOC_TMC_ITM_QT_NDX_13
#define ARAD_ITM_QT_NDX_14                                SOC_TMC_ITM_QT_NDX_14
#define ARAD_ITM_QT_NDX_15                                SOC_TMC_ITM_QT_NDX_15

typedef SOC_TMC_ITM_QT_NDX                                ARAD_ITM_QT_NDX;

#define ARAD_ITM_CR_DISCNT_CLS_NDX_00                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_00
#define ARAD_ITM_CR_DISCNT_CLS_NDX_01                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_01
#define ARAD_ITM_CR_DISCNT_CLS_NDX_02                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_02
#define ARAD_ITM_CR_DISCNT_CLS_NDX_03                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_03
#define ARAD_ITM_CR_DISCNT_CLS_NDX_04                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_04
#define ARAD_ITM_CR_DISCNT_CLS_NDX_05                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_05
#define ARAD_ITM_CR_DISCNT_CLS_NDX_06                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_06
#define ARAD_ITM_CR_DISCNT_CLS_NDX_07                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_07
#define ARAD_ITM_CR_DISCNT_CLS_NDX_08                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_08
#define ARAD_ITM_CR_DISCNT_CLS_NDX_09                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_09
#define ARAD_ITM_CR_DISCNT_CLS_NDX_10                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_10
#define ARAD_ITM_CR_DISCNT_CLS_NDX_11                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_11
#define ARAD_ITM_CR_DISCNT_CLS_NDX_12                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_12
#define ARAD_ITM_CR_DISCNT_CLS_NDX_13                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_13
#define ARAD_ITM_CR_DISCNT_CLS_NDX_14                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_14
#define ARAD_ITM_CR_DISCNT_CLS_NDX_15                     SOC_TMC_ITM_CR_DISCNT_CLS_NDX_15
#define ARAD_ITM_NOF_CR_DISCNT_CLS_NDXS                   SOC_TMC_ITM_NOF_CR_DISCNT_CLS_NDXS
typedef SOC_TMC_ITM_CR_DISCNT_CLS_NDX                          ARAD_ITM_CR_DISCNT_CLS_NDX;

#define ARAD_ITM_DBUFF_SIZE_BYTES_256                     SOC_TMC_ITM_DBUFF_SIZE_BYTES_256
#define ARAD_ITM_DBUFF_SIZE_BYTES_512                     SOC_TMC_ITM_DBUFF_SIZE_BYTES_512
#define ARAD_ITM_DBUFF_SIZE_BYTES_1024                    SOC_TMC_ITM_DBUFF_SIZE_BYTES_1024
#define ARAD_ITM_DBUFF_SIZE_BYTES_2048                    SOC_TMC_ITM_DBUFF_SIZE_BYTES_2048
#define ARAD_ITM_DBUFF_SIZE_BYTES_4096                    SOC_TMC_ITM_DBUFF_SIZE_BYTES_4096
#define ARAD_ITM_NOF_DBUFF_SIZES                          SOC_TMC_ITM_NOF_DBUFF_SIZES
typedef SOC_TMC_ITM_DBUFF_SIZE_BYTES                           ARAD_ITM_DBUFF_SIZE_BYTES;

#define ARAD_ITM_VSQ_GROUPA_SZE(unit)                           SOC_TMC_ITM_VSQ_GROUPA_SZE(unit)
#define ARAD_ITM_VSQ_GROUPB_SZE(unit)                           SOC_TMC_ITM_VSQ_GROUPB_SZE(unit)
#define ARAD_ITM_VSQ_GROUPC_SZE(unit)                           SOC_TMC_ITM_VSQ_GROUPC_SZE(unit)
#define ARAD_ITM_VSQ_GROUPD_SZE(unit)                           SOC_TMC_ITM_VSQ_GROUPD_SZE(unit)
#define ARAD_ITM_VSQ_GROUPE_SZE(unit)                           SOC_TMC_ITM_VSQ_GROUPE_SZE(unit)
#define ARAD_ITM_VSQ_GROUPF_SZE(unit)                           SOC_TMC_ITM_VSQ_GROUPF_SZE(unit)
typedef SOC_TMC_ITM_VSQ_GROUP_SIZE                             ARAD_ITM_VSQ_GROUP_SIZE;

#define ARAD_ITM_VSQ_NDX_MIN(unit)                              SOC_TMC_ITM_VSQ_NDX_MIN(unit)
#define ARAD_ITM_VSQ_NDX_MAX(unit)                              SOC_TMC_ITM_VSQ_NDX_MAX_ARAD(unit)
typedef SOC_TMC_ITM_VSQ_NDX_RNG                               ARAD_ITM_VSQ_NDX_RNG;

#define ARAD_ITM_ADMIT_TST_00                             SOC_TMC_ITM_ADMIT_TST_00
#define ARAD_ITM_ADMIT_TST_01                             SOC_TMC_ITM_ADMIT_TST_01
#define ARAD_ITM_ADMIT_TST_02                             SOC_TMC_ITM_ADMIT_TST_02
#define ARAD_ITM_ADMIT_TST_03                             SOC_TMC_ITM_ADMIT_TST_03
typedef SOC_TMC_ITM_ADMIT_TSTS                                 ARAD_ITM_ADMIT_TSTS;

#define ARAD_ITM_VSQ_GROUP_CTGRY                          SOC_TMC_ITM_VSQ_GROUP_CTGRY
#define ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS              SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS
#define ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS            SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS
#define ARAD_ITM_VSQ_GROUP_STTSTCS_TAG                    SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG
#define ARAD_ITM_VSQ_GROUP_LLFC                           SOC_TMC_ITM_VSQ_GROUP_LLFC
#define ARAD_ITM_VSQ_GROUP_PFC                            SOC_TMC_ITM_VSQ_GROUP_PFC
#define ARAD_ITM_VSQ_GROUP_SRC_PORT                       SOC_TMC_ITM_VSQ_GROUP_SRC_PORT
#define ARAD_ITM_VSQ_GROUP_PG                             SOC_TMC_ITM_VSQ_GROUP_PG

typedef SOC_TMC_ITM_VSQ_GROUP                                  ARAD_ITM_VSQ_GROUP;

typedef SOC_TMC_ITM_DRAM_BUFFERS_INFO                          ARAD_ITM_DRAM_BUFFERS_INFO;
typedef SOC_TMC_ITM_GLOB_RCS_FC_TYPE                           ARAD_ITM_GLOB_RCS_FC_TYPE;
typedef SOC_TMC_ITM_GLOB_RCS_FC_TH                             ARAD_ITM_GLOB_RCS_FC_TH;
typedef SOC_TMC_ITM_GLOB_RCS_DROP_TH                           ARAD_ITM_GLOB_RCS_DROP_TH;
typedef SOC_TMC_ITM_QUEUE_INFO                                 ARAD_ITM_QUEUE_INFO;
typedef SOC_TMC_ITM_QUEUE_DYN_INFO                             ARAD_ITM_QUEUE_DYN_INFO;
typedef SOC_TMC_ITM_CATEGORY_RNGS                              ARAD_ITM_CATEGORY_RNGS;
typedef SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT                       ARAD_ITM_ADMIT_ONE_TEST_TMPLT;
typedef SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO                      ARAD_ITM_ADMIT_TEST_TMPLT_INFO;
typedef SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH                       ARAD_ITM_CR_REQUEST_HUNGRY_TH;
typedef SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH                      ARAD_ITM_CR_REQUEST_BACKOFF_TH;
typedef SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH                      ARAD_ITM_CR_REQUEST_BACKLOG_TH;
typedef SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH                      ARAD_ITM_CR_REQUEST_EMPTY_Q_TH;
typedef SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH                    ARAD_ITM_CR_REQUEST_SATISFIED_TH;
typedef SOC_TMC_ITM_CR_WD_Q_TH                                 ARAD_ITM_CR_WD_Q_TH;
typedef SOC_TMC_ITM_CR_REQUEST_INFO                            ARAD_ITM_CR_REQUEST_INFO;
typedef SOC_TMC_ITM_CR_DISCOUNT_INFO                           ARAD_ITM_CR_DISCOUNT_INFO;
typedef SOC_TMC_ITM_WRED_QT_DP_INFO                            ARAD_ITM_WRED_QT_DP_INFO;
typedef SOC_TMC_ITM_TAIL_DROP_INFO                             ARAD_ITM_TAIL_DROP_INFO;
typedef SOC_TMC_ITM_CR_WD_INFO                                 ARAD_ITM_CR_WD_INFO;
typedef SOC_TMC_ITM_VSQ_FC_INFO                                ARAD_ITM_VSQ_FC_INFO;
typedef SOC_TMC_ITM_VSQ_TAIL_DROP_INFO                         ARAD_ITM_VSQ_TAIL_DROP_INFO;
typedef SOC_TMC_ITM_VSQ_WRED_GEN_INFO                          ARAD_ITM_VSQ_WRED_GEN_INFO;
typedef SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE                      ARAD_ITM_INGRESS_SHAPE_Q_RANGE;
typedef SOC_TMC_ITM_INGRESS_SHAPE_INFO                         ARAD_ITM_INGRESS_SHAPE_INFO;
typedef SOC_TMC_ITM_PRIORITY_MAP_TMPLT                         ARAD_ITM_PRIORITY_MAP_TMPLT;
typedef SOC_TMC_ITM_SYS_RED_DROP_PROB                          ARAD_ITM_SYS_RED_DROP_PROB;
typedef SOC_TMC_ITM_SYS_RED_QT_DP_INFO                         ARAD_ITM_SYS_RED_QT_DP_INFO;
typedef SOC_TMC_ITM_SYS_RED_QT_INFO                            ARAD_ITM_SYS_RED_QT_INFO;
typedef SOC_TMC_ITM_SYS_RED_EG_INFO                            ARAD_ITM_SYS_RED_EG_INFO;
typedef SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS                       ARAD_ITM_SYS_RED_GLOB_RCS_THS;
typedef SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS                      ARAD_ITM_SYS_RED_GLOB_RCS_VALS;
typedef SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO                      ARAD_ITM_SYS_RED_GLOB_RCS_INFO;

typedef SOC_TMC_ITM_VSQ_NDX                                    ARAD_ITM_VSQ_NDX;
typedef SOC_TMC_ITM_CGM_CONGENSTION_STATS                      ARAD_ITM_CGM_CONGENSTION_STATS;

typedef enum{
  
  ARAD_ITM_STAG_ENABLE_MODE_DISABLED=0,
  
  ARAD_ITM_STAG_ENABLE_MODE_STAT_IF_NO_DEQ=1,
  
  ARAD_ITM_STAG_ENABLE_MODE_ENABLED_WITH_DEQ=2,
  
  ARAD_ITM_NOF_STAG_ENABLE_MODES=3
}ARAD_ITM_STAG_ENABLE_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  ARAD_ITM_STAG_ENABLE_MODE enable_mode;
  
  uint32 vsq_index_msb;
  
  uint32 vsq_index_lsb;
  
  uint8 dropp_en;
  
  uint32 dropp_lsb;

}ARAD_ITM_STAG_INFO;














uint32
  arad_itm_dram_buffs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs
  );


uint32
  arad_itm_glob_rcs_fc_set(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   ARAD_ITM_GLOB_RCS_FC_TH  *info,
    SOC_SAND_OUT  ARAD_ITM_GLOB_RCS_FC_TH  *exact_info
  );


uint32
  arad_itm_glob_rcs_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_FC_TH  *info
  );


int
  arad_itm_glob_rcs_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *exact_info
  );


int
  arad_itm_glob_rcs_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *info
  );


int
  arad_itm_category_rngs_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info
  );


int
  arad_itm_category_rngs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT ARAD_ITM_CATEGORY_RNGS *info
  );


uint32
  arad_itm_admit_test_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );


uint32
  arad_itm_admit_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );


uint32
  arad_itm_cr_request_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_INFO *info,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *exact_info
  );


uint32
  arad_itm_cr_request_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32          qt_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *info
  );


uint32
  arad_itm_cr_discount_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_DISCOUNT_INFO *info
  );


uint32
  arad_itm_cr_discount_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_DISCOUNT_INFO *info
  );


uint32
  arad_itm_queue_test_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TSTS      test_tmplt
  );


uint32
  arad_itm_queue_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TSTS      *test_tmplt
  );


int
  arad_itm_wred_exp_wq_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq,
    SOC_SAND_IN  uint8                   enable
  );


int
  arad_itm_wred_exp_wq_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT  uint32                  *exp_wq
  );


int
  arad_itm_wred_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  );


int
  arad_itm_wred_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  );


int
  arad_itm_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT  ARAD_ITM_TAIL_DROP_INFO  *exact_info
  );


int
  arad_itm_tail_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *info
  );

int
  arad_itm_fadt_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_FADT_DROP_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO  *exact_info
  );

int
  arad_itm_fadt_tail_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO  *info
  );



uint32
  arad_itm_cr_wd_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  ARAD_ITM_CR_WD_INFO      *info,
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *exact_info
  );


uint32
  arad_itm_cr_wd_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *info
  );



uint32
  arad_itm_enable_ecn(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled 
  );


uint32
  arad_itm_get_ecn_enabled(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled 
  );



int
  arad_itm_vsq_qt_rt_cls_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  );


int
  arad_itm_vsq_qt_rt_cls_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_OUT uint32                 *vsq_rt_cls
  );


int
  arad_itm_vsq_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *exact_info
  );


int
  arad_itm_vsq_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *info
  );


int
  arad_itm_vsq_tail_drop_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP             vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                         drop_precedence_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_IN  int                            is_headroom,
    SOC_SAND_IN  ARAD_ITM_VSQ_TAIL_DROP_INFO    *info,
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO    *exact_info
  );


int
  arad_itm_vsq_tail_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  int                            is_headroom,
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  );


int
  arad_itm_vsq_tail_drop_default_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  );


int
  arad_itm_vsq_wred_gen_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP         vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );


int
  arad_itm_vsq_wred_gen_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP         vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );


int
  arad_itm_vsq_wred_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  );


int
  arad_itm_vsq_wred_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  );



uint32
  arad_itm_vsq_counter_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_cob_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  );



uint32
  arad_itm_vsq_counter_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT  uint8                    *is_cob_only,
    SOC_SAND_OUT ARAD_ITM_VSQ_GROUP       *vsq_group_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_NDX         *vsq_in_group_ndx
  );



uint32
  arad_itm_vsq_counter_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT uint32                  *pckt_count
  );


uint32
  arad_itm_queue_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *old_info,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *info
  );


uint32
  arad_itm_queue_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT ARAD_ITM_QUEUE_INFO      *info
  );


int
  arad_itm_queue_dyn_info_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  int    core,
    SOC_SAND_IN  uint32 queue_ndx,
    SOC_SAND_OUT ARAD_ITM_QUEUE_DYN_INFO	  *info
  );


int
  arad_itm_dyn_total_thresh_set(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  int      core_id,
    SOC_SAND_IN  uint8    is_ocb_only,
                 int32    reservation_increase[SOC_DPP_DEFS_MAX(NOF_CORES)][SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] 
  );


uint32
  arad_itm_ingress_shape_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info
  );


uint32
  arad_itm_ingress_shape_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_INFO *info
  );


uint32
  arad_itm_priority_map_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );


uint32
  arad_itm_priority_map_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_OUT ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );


uint32
  arad_itm_priority_map_tmplt_select_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  );


uint32
  arad_itm_priority_map_tmplt_select_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_OUT uint32                 *priority_map
  );


uint32
  arad_itm_sys_red_drop_prob_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_DROP_PROB *info
  );


uint32
  arad_itm_sys_red_drop_prob_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_DROP_PROB *info
  );


int
  arad_itm_sys_red_queue_size_boundaries_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info
  );


int
  arad_itm_sys_red_queue_size_boundaries_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  );


int
  arad_itm_sys_red_q_based_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );


int
  arad_itm_sys_red_q_based_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );


uint32
  arad_itm_sys_red_eg_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_EG_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *exact_info
  );


uint32
  arad_itm_sys_red_eg_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *info
  );


int
  arad_itm_sys_red_glob_rcs_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );


int
  arad_itm_sys_red_glob_rcs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );


uint32
  arad_itm_vsq_index_global2group(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX   vsq_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_GROUP *vsq_group,
    SOC_SAND_OUT uint32            *vsq_in_group_ndx,
    SOC_SAND_OUT uint8             *is_ocb_only
  );

uint32
  arad_itm_vsq_index_group2global(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP vsq_group,
    SOC_SAND_IN  uint32            vsq_in_group_ndx,
    SOC_SAND_IN  uint8             is_ocb_only,
    SOC_SAND_OUT ARAD_ITM_VSQ_NDX   *vsq_ndx
  );

void
  arad_ARAD_ITM_DRAM_BUFFERS_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_DRAM_BUFFERS_INFO *info
  );


void
  arad_ARAD_ITM_GLOB_RCS_FC_TYPE_clear(
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_FC_TYPE *info
  );

void
  arad_ARAD_ITM_GLOB_RCS_FC_TH_clear(
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_FC_TH *info
  );

void
  arad_ARAD_ITM_GLOB_RCS_DROP_TH_clear(
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *info
  );

void
  arad_ARAD_ITM_QUEUE_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_QUEUE_INFO *info
  );

void
  arad_ARAD_ITM_CATEGORY_RNGS_clear(
    SOC_SAND_OUT ARAD_ITM_CATEGORY_RNGS *info
  );

void
  arad_ARAD_ITM_ADMIT_ONE_TEST_TMPLT_clear(
    SOC_SAND_OUT ARAD_ITM_ADMIT_ONE_TEST_TMPLT *info
  );

void
  arad_ARAD_ITM_ADMIT_TEST_TMPLT_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

void
  arad_ARAD_ITM_CR_REQUEST_HUNGRY_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_HUNGRY_TH *info
  );

void
  arad_ARAD_ITM_CR_REQUEST_BACKOFF_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_BACKOFF_TH *info
  );

void
  arad_ARAD_ITM_CR_REQUEST_BACKLOG_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_BACKLOG_TH *info
  );

void
  arad_ARAD_ITM_CR_REQUEST_EMPTY_Q_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_EMPTY_Q_TH *info
  );

void
  arad_ARAD_ITM_CR_REQUEST_SATISFIED_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_SATISFIED_TH *info
  );

void
  arad_ARAD_ITM_CR_WD_Q_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_WD_Q_TH *info
  );

void
  arad_ARAD_ITM_CR_REQUEST_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *info
  );

void
  arad_ARAD_ITM_CR_DISCOUNT_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_CR_DISCOUNT_INFO *info
  );

void
  arad_ARAD_ITM_WRED_QT_DP_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  );

void
  arad_ARAD_ITM_TAIL_DROP_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO *info
  );

void
  arad_ARAD_ITM_CR_WD_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO *info
  );

void
  arad_ARAD_ITM_VSQ_FC_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO *info
  );

void
  arad_ARAD_ITM_VSQ_TAIL_DROP_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  );

void
  arad_ARAD_ITM_VSQ_WRED_GEN_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );

void
  arad_ARAD_ITM_STAG_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_STAG_INFO *info
  );

void
  arad_ARAD_ITM_INGRESS_SHAPE_Q_RANGE_clear(
    SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_Q_RANGE *info
  );

void
  arad_ARAD_ITM_INGRESS_SHAPE_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_INFO *info
  );

void
  arad_ARAD_ITM_PRIORITY_MAP_TMPLT_clear(
    SOC_SAND_OUT ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );

void
  arad_ARAD_ITM_SYS_RED_DROP_PROB_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_DROP_PROB *info
  );

void
  arad_ARAD_ITM_SYS_RED_QT_DP_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );

void
  arad_ARAD_ITM_SYS_RED_QT_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  );

void
  arad_ARAD_ITM_SYS_RED_EG_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *info
  );

void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_THS_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_THS *info
  );

void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_VALS_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_VALS *info
  );

void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1


const char*
  arad_ARAD_ITM_DBUFF_SIZE_BYTES_to_string(
    SOC_SAND_IN ARAD_ITM_DBUFF_SIZE_BYTES enum_val
  );


const char*
  arad_ARAD_ITM_ADMIT_TSTS_to_string(
    SOC_SAND_IN ARAD_ITM_ADMIT_TSTS enum_val
  );



const char*
  arad_ARAD_ITM_VSQ_GROUP_to_string(
    SOC_SAND_IN ARAD_ITM_VSQ_GROUP enum_val
  );



void
  arad_ARAD_ITM_DRAM_BUFFERS_INFO_print(
    SOC_SAND_IN ARAD_ITM_DRAM_BUFFERS_INFO *info
  );


void
  arad_ARAD_ITM_GLOB_RCS_FC_TYPE_print(
    SOC_SAND_IN ARAD_ITM_GLOB_RCS_FC_TYPE *info
  );



void
  arad_ARAD_ITM_GLOB_RCS_FC_TH_print(
    SOC_SAND_IN ARAD_ITM_GLOB_RCS_FC_TH *info
  );

void
  arad_ARAD_ITM_GLOB_RCS_DROP_TH_print_no_table(
    SOC_SAND_IN ARAD_ITM_GLOB_RCS_DROP_TH *info
  );

void
  arad_ARAD_ITM_GLOB_RCS_DROP_TH_print(
    SOC_SAND_IN ARAD_ITM_GLOB_RCS_DROP_TH *info
  );



void
  arad_ARAD_ITM_QUEUE_INFO_print(
    SOC_SAND_IN ARAD_ITM_QUEUE_INFO *info
  );



void
  arad_ARAD_ITM_CATEGORY_RNGS_print(
    SOC_SAND_IN ARAD_ITM_CATEGORY_RNGS *info
  );



void
  arad_ARAD_ITM_ADMIT_ONE_TEST_TMPLT_print(
    SOC_SAND_IN ARAD_ITM_ADMIT_ONE_TEST_TMPLT *info
  );



void
  arad_ARAD_ITM_ADMIT_TEST_TMPLT_INFO_print(
    SOC_SAND_IN ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  );



void
  arad_ARAD_ITM_CR_REQUEST_HUNGRY_TH_print(
    SOC_SAND_IN ARAD_ITM_CR_REQUEST_HUNGRY_TH *info
  );



void
  arad_ARAD_ITM_CR_REQUEST_BACKOFF_TH_print(
    SOC_SAND_IN ARAD_ITM_CR_REQUEST_BACKOFF_TH *info
  );



void
  arad_ARAD_ITM_CR_REQUEST_BACKLOG_TH_print(
    SOC_SAND_IN ARAD_ITM_CR_REQUEST_BACKLOG_TH *info
  );



void
  arad_ARAD_ITM_CR_REQUEST_EMPTY_Q_TH_print(
    SOC_SAND_IN ARAD_ITM_CR_REQUEST_EMPTY_Q_TH *info
  );



void
  arad_ARAD_ITM_CR_REQUEST_SATISFIED_TH_print(
    SOC_SAND_IN ARAD_ITM_CR_REQUEST_SATISFIED_TH *info
  );



void
  arad_ARAD_ITM_CR_WD_Q_TH_print(
    SOC_SAND_IN ARAD_ITM_CR_WD_Q_TH *info
  );



void
  arad_ARAD_ITM_CR_REQUEST_INFO_print(
    SOC_SAND_IN ARAD_ITM_CR_REQUEST_INFO *info
  );



void
  arad_ARAD_ITM_CR_DISCOUNT_INFO_print(
    SOC_SAND_IN ARAD_ITM_CR_DISCOUNT_INFO *info
  );



void
  arad_ARAD_ITM_WRED_QT_DP_INFO_print(
    SOC_SAND_IN ARAD_ITM_WRED_QT_DP_INFO *info
  );



void
  arad_ARAD_ITM_TAIL_DROP_INFO_print(
    SOC_SAND_IN ARAD_ITM_TAIL_DROP_INFO *info
  );



void
  arad_ARAD_ITM_CR_WD_INFO_print(
    SOC_SAND_IN ARAD_ITM_CR_WD_INFO *info
  );



void
  arad_ARAD_ITM_VSQ_FC_INFO_print(
    SOC_SAND_IN ARAD_ITM_VSQ_FC_INFO *info
  );



void
  arad_ARAD_ITM_VSQ_TAIL_DROP_INFO_print(
    SOC_SAND_IN ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  );



void
  arad_ARAD_ITM_VSQ_WRED_GEN_INFO_print(
    SOC_SAND_IN ARAD_ITM_VSQ_WRED_GEN_INFO *info
  );



void
  arad_ARAD_ITM_STAG_INFO_print(
    SOC_SAND_IN ARAD_ITM_STAG_INFO *info
  );



void
  arad_ARAD_ITM_INGRESS_SHAPE_Q_RANGE_print(
    SOC_SAND_IN ARAD_ITM_INGRESS_SHAPE_Q_RANGE *info
  );

void
  arad_ARAD_ITM_INGRESS_SHAPE_INFO_print(
    SOC_SAND_IN ARAD_ITM_INGRESS_SHAPE_INFO *info
  );



void
  arad_ARAD_ITM_PRIORITY_MAP_TMPLT_print(
    SOC_SAND_IN ARAD_ITM_PRIORITY_MAP_TMPLT *info
  );



void
  arad_ARAD_ITM_SYS_RED_DROP_PROB_print(
    SOC_SAND_IN ARAD_ITM_SYS_RED_DROP_PROB *info
  );



void
  arad_ARAD_ITM_SYS_RED_QT_DP_INFO_print(
    SOC_SAND_IN ARAD_ITM_SYS_RED_QT_DP_INFO *info
  );



void
  arad_ARAD_ITM_SYS_RED_QT_INFO_print(
    SOC_SAND_IN ARAD_ITM_SYS_RED_QT_INFO *info
  );



void
  arad_ARAD_ITM_SYS_RED_EG_INFO_print(
    SOC_SAND_IN ARAD_ITM_SYS_RED_EG_INFO *info
  );



void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_THS_print(
    SOC_SAND_IN ARAD_ITM_SYS_RED_GLOB_RCS_THS *info
  );



void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_VALS_print(
    SOC_SAND_IN ARAD_ITM_SYS_RED_GLOB_RCS_VALS *info
  );



void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_INFO_print(
    SOC_SAND_IN ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  );


#endif 


int
  arad_itm_committed_q_size_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  );


int
  arad_itm_committed_q_size_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  );


uint32
  arad_itm_pfc_tc_map_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN int32                    tc_in,
    SOC_SAND_IN int32                    port_id,
    SOC_SAND_IN int32                    tc_out
  );

uint32
  arad_itm_pfc_tc_map_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int32                   tc_in,
    SOC_SAND_IN  int32                   port_id,
    SOC_SAND_OUT int32                   *tc_out
  );
 
int
  arad_itm_dp_discard_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN  uint32                  discard_dp
  );
 
int
  arad_itm_dp_discard_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  ); 


uint32
  arad_plus_itm_alpha_set(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       rt_cls_ndx,
    SOC_SAND_IN  uint32       drop_precedence_ndx,
    SOC_SAND_IN  int32        alpha 
  );


uint32
  arad_plus_itm_alpha_get(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       rt_cls_ndx,
    SOC_SAND_IN  uint32       drop_precedence_ndx,
    SOC_SAND_OUT int32        *alpha 
  );


uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_set(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint8    enabled 
  );


uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint8    *enabled 
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


