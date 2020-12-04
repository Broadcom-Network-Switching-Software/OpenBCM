/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_INGRESS_TRAFFIC_MGMT_INCLUDED__

#define __SOC_TMC_API_INGRESS_TRAFFIC_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>





#define SOC_TMC_ITM_NOF_VSQS(unit)(SOC_TMC_ITM_VSQ_GROUPA_SZE(unit) + \
                                   SOC_TMC_ITM_VSQ_GROUPB_SZE(unit) + \
                                   SOC_TMC_ITM_VSQ_GROUPC_SZE(unit) + \
                                   SOC_TMC_ITM_VSQ_GROUPD_SZE(unit) + \
                                   SOC_TMC_ITM_VSQ_GROUPE_SZE(unit) + \
                                   SOC_TMC_ITM_VSQ_GROUPF_SZE(unit))

#define SOC_TMC_ITM_NOF_VSQS_IN_GROUP(unit, vsq_group) (                                                       \
            (vsq_group == SOC_TMC_ITM_VSQ_GROUP_CTGRY              ) ? (SOC_TMC_ITM_VSQ_GROUPA_SZE(unit)) : (  \
            (vsq_group == SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS  ) ? (SOC_TMC_ITM_VSQ_GROUPB_SZE(unit)) : (  \
            (vsq_group == SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS) ? (SOC_TMC_ITM_VSQ_GROUPC_SZE(unit)) : (  \
            (vsq_group == SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG        ) ? (SOC_TMC_ITM_VSQ_GROUPD_SZE(unit)) : (  \
            (vsq_group == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT           ) ? (SOC_TMC_ITM_VSQ_GROUPE_SZE(unit)) : (  \
            (vsq_group == SOC_TMC_ITM_VSQ_GROUP_PG                 ) ? (SOC_TMC_ITM_VSQ_GROUPF_SZE(unit)) : 0 ))))))

#define SOC_TMC_ITM_PRIO_MAP_SIZE_IN_UINT32S 2



#define SOC_TMC_ITM_PER_PACKET_COMPENSATION_ENABLED(unit) (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 0 : \
                                                           ((SOC_DPP_CONFIG(unit)->pdm_extension.max_hdr_comp_ptr && \
                                                           (SOC_DPP_CONFIG(unit)->arad->init.dram.pdm_mode == ARAD_INIT_PDM_MODE_REDUCED)) || \
                                                            SOC_IS_QAX(unit)))
#define SOC_TMC_ITM_COMPENSATION_LEGACY_MODE(unit) (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->per_packet_compensation_legacy_mode)




#define  SOC_TMC_ITM_NOF_DRAM_BUFFS (2*1024*1024)




#define  SOC_TMC_ITM_DBUFF_MMC_MAX (64*1024)


#define SOC_TMC_ITM_SYS_RED_Q_SIZE_RANGES   16
#define SOC_TMC_ITM_SYS_RED_DRP_PROBS       16
#define SOC_TMC_ITM_SYS_RED_BUFFS_RNGS      4

  
#define SOC_TMC_ITM_VSQ_A_QT_RT_CLS_MAX(unit) (SOC_DPP_DEFS_GET(unit, max_vsq_a_rt_cls))
#define SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)   (SOC_DPP_DEFS_GET(unit, max_vsq_rt_cls))
#define SOC_TMC_ITM_VSQ_NOF_RATE_CLASSES(unit)  (SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit) + 1)
#define SOC_TMC_ITM_RATE_CLASS_MAX    63
#define SOC_TMC_ITM_NOF_RATE_CLASSES  (SOC_TMC_ITM_RATE_CLASS_MAX + 1)


#define SOC_TMC_ITM_WRED_GRANULARITY        16

#define SOC_TMC_WRED_NORMALIZE_FACTOR  (0x28F5C29)

#define SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_MSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 59 : 60)
#define SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB             53
#define SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit)  (SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_MSB(unit) - SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB + 1)
#define SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_MSB             64
#define SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 60 : 61)
#define SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit)  (SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_MSB - SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit) + 1)


#define SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_MSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 47 : 48)
#define SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB             41
#define SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit)  (SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_MSB(unit) - SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB + 1)
#define SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_MSB             52
#define SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit)       (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 48 : 49)
#define SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit)  (SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_MSB - SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit) + 1)

#define SOC_TMC_ITM_VSQ_WRED_AVRG_TH_MNT_MSB        7
#define SOC_TMC_ITM_VSQ_WRED_AVRG_TH_MNT_LSB        0
#define SOC_TMC_ITM_VSQ_WRED_AVRG_TH_MNT_NOF_BITS   (SOC_TMC_ITM_VSQ_WRED_AVRG_TH_MNT_MSB - SOC_TMC_ITM_VSQ_WRED_AVRG_TH_MNT_LSB + 1)
#define SOC_TMC_ITM_VSQ_WRED_AVRG_TH_EXP_MSB        12
#define SOC_TMC_ITM_VSQ_WRED_AVRG_TH_EXP_LSB        8
#define SOC_TMC_ITM_VSQ_WRED_AVRG_TH_EXP_NOF_BITS   (SOC_TMC_ITM_VSQ_WRED_AVRG_TH_EXP_MSB - SOC_TMC_ITM_VSQ_WRED_AVRG_TH_EXP_LSB + 1)


#define SOC_TMC_ITM_WRED_MAX_PACKET_SIZE ((1 << 14) - 128)
#define SOC_TMC_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC SOC_TMC_ITM_WRED_MAX_PACKET_SIZE




#define SOC_TMC_ITM_CREDIT_WATCHDOG_NORMAL_DELETE_THRESHOLD 500
#define SOC_TMC_ITM_CREDIT_WATCHDOG_NORMAL_STATUS_MSG_THRESHOLD 33
#define SOC_TMC_ITM_CREDIT_WATCHDOG_AGGRESSIVE_WD_STATUS_MSG_MESSAGE_THRESHOLD 3

#define SOC_TMC_ITM_CR_WD_Q_TH_OPERATION_FAILED ((uint32)(-1))


#define SOC_TMC_ITM_NOF_RSRC_POOLS 2 
#define SOC_TMC_ITM_SHRD_RJCT_TH_MAX 0xfff
#define SOC_TMC_ITM_RESOURCE_ALLOCATION_SHARED_MAX 0x3fffff




#define SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX     0XFFFFFF


#define SOC_TMC_ITM_GLOB_RCS_DROP_BDS_SIZE_MAX      0x7FF8000 


#define SOC_TMC_ITM_GLOB_RCS_DROP_UC_SIZE_MAX       0X7FFFFF


#define SOC_TMC_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX  0X3FFFFF


#define SOC_TMC_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX      0XFFFF


#define SOC_TMC_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX      0X7FFF


#define SOC_TMC_ITM_VSQ_GRNT_BD_SIZE_MAX            0X3FFFFF


#define SOC_TMC_ITM_VSQ_FC_BD_SIZE_MAX              0X3FFFFF


#define SOC_TMC_ITM_FADT_ALPHA_MAX  (7)
#define SOC_TMC_ITM_FADT_ALPHA_MIN  (-7)











typedef enum
{
  
  SOC_TMC_ITM_CGM_MGMT_GUARANTEE_LOOSE = 0,
  
  SOC_TMC_ITM_CGM_MGMT_GUARANTEE_STRICT = 1
} SOC_TMC_ITM_CGM_MGMT_GUARANTEE_MODE;


typedef enum
{
  
  SOC_TMC_ITM_PKT_SIZE_ADJUST_QUEUE = 0,

  
  SOC_TMC_ITM_PKT_SIZE_ADJUST_PORT = 1,

  
  SOC_TMC_ITM_PKT_SIZE_ADJUST_APPEND_SIZE_PTR = 2

} SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE;



typedef struct {
    
    int index;
    
    int value;

}  SOC_TMC_ITM_PKT_SIZE_ADJUST_INFO;


typedef uint32 SOC_TMC_ITM_VSQ_NDX;

typedef enum
{
  
  SOC_TMC_ITM_QT_NDX_00=0,
  
  SOC_TMC_ITM_QT_NDX_01=1,
  
  SOC_TMC_ITM_QT_NDX_02=2,
  
  SOC_TMC_ITM_QT_NDX_03=3,
  
  SOC_TMC_ITM_QT_NDX_04=4,
  
  SOC_TMC_ITM_QT_NDX_05=5,
  
  SOC_TMC_ITM_QT_NDX_06=6,
  
  SOC_TMC_ITM_QT_NDX_07=7,
  
  SOC_TMC_ITM_QT_NDX_08=8,
  
  SOC_TMC_ITM_QT_NDX_09=9,
  
  SOC_TMC_ITM_QT_NDX_10=10,
  
  SOC_TMC_ITM_QT_NDX_11=11,
  
  SOC_TMC_ITM_QT_NDX_12=12,
  
  SOC_TMC_ITM_QT_NDX_13=13,
  
  SOC_TMC_ITM_QT_NDX_14=14,
  
  SOC_TMC_ITM_QT_NDX_15=15,
  
  SOC_TMC_ITM_NOF_QT_NDXS_ARAD = 16,

  SOC_TMC_ITM_QT_NDX_16 = 16,
  SOC_TMC_ITM_QT_NDX_17 = 17,
  SOC_TMC_ITM_QT_NDX_18 = 18,
  SOC_TMC_ITM_QT_NDX_19 = 19,
  SOC_TMC_ITM_QT_NDX_21 = 21,
  SOC_TMC_ITM_QT_NDX_22 = 22,
  SOC_TMC_ITM_QT_NDX_23 = 23,
  SOC_TMC_ITM_QT_NDX_24 = 24,
  SOC_TMC_ITM_QT_NDX_25 = 25,
  SOC_TMC_ITM_QT_NDX_26 = 26,
  SOC_TMC_ITM_QT_NDX_27 = 27,
  SOC_TMC_ITM_QT_NDX_28 = 28,
  SOC_TMC_ITM_QT_NDX_29 = 29,
  SOC_TMC_ITM_QT_NDX_30 = 30,
  SOC_TMC_ITM_QT_NDX_31 = 31,
  SOC_TMC_ITM_NOF_QT_NDXS = 32,
  SOC_TMC_ITM_QT_NDX_INVALID = SOC_TMC_ITM_NOF_QT_NDXS,
  
  SOC_TMC_ITM_NOF_QT_STATIC = 9,
  SOC_TMC_ITM_QT_PUSH_Q_NDX = SOC_TMC_ITM_QT_NDX_15,
  
  SOC_TMC_ITM_PREDEFIEND_OFFSET = 128
}SOC_TMC_ITM_QT_NDX;

typedef enum
{
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_00=0,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_01=1,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_02=2,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_03=3,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_04=4,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_05=5,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_06=6,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_07=7,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_08=8,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_09=9,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_10=10,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_11=11,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_12=12,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_13=13,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_14=14,
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_15=15,
  
  SOC_TMC_ITM_NOF_CR_DISCNT_CLS_NDXS = 16
}SOC_TMC_ITM_CR_DISCNT_CLS_NDX;

typedef enum
{
  
  SOC_TMC_ITM_DBUFF_SIZE_BYTES_256=256,
  
  SOC_TMC_ITM_DBUFF_SIZE_BYTES_512=512,
  
  SOC_TMC_ITM_DBUFF_SIZE_BYTES_1024=1024,
  
  SOC_TMC_ITM_DBUFF_SIZE_BYTES_2048=2048,
  
  SOC_TMC_ITM_DBUFF_SIZE_BYTES_4096=4096,
  
  SOC_TMC_ITM_NOF_DBUFF_SIZES=4
}SOC_TMC_ITM_DBUFF_SIZE_BYTES;

#define SOC_TMC_ITM_DBUFF_SIZE_BYTES_MIN SOC_TMC_ITM_DBUFF_SIZE_BYTES_256
#define SOC_TMC_ITM_DBUFF_SIZE_BYTES_MAX SOC_TMC_ITM_DBUFF_SIZE_BYTES_2048

#define SOC_TMC_ITM_VSQ_GROUPA_SZE(unit) (SOC_DPP_DEFS_GET(unit, nof_vsq_a))
#define SOC_TMC_ITM_VSQ_GROUPB_SZE(unit) (SOC_DPP_DEFS_GET(unit, nof_vsq_b))
#define SOC_TMC_ITM_VSQ_GROUPC_SZE(unit) (SOC_DPP_DEFS_GET(unit, nof_vsq_c))
#define SOC_TMC_ITM_VSQ_GROUPD_SZE(unit) (SOC_DPP_DEFS_GET(unit, nof_vsq_d))
#define SOC_TMC_ITM_VSQ_GROUPE_SZE(unit) (SOC_DPP_DEFS_GET(unit, nof_vsq_e))
#define SOC_TMC_ITM_VSQ_GROUPF_SZE(unit) (SOC_DPP_DEFS_GET(unit, nof_vsq_f))

#define SOC_TMC_ITM_VSQ_NDX_MIN(unit) 0
#define SOC_TMC_ITM_VSQ_NDX_MAX(unit) \
    (SOC_TMC_ITM_VSQ_GROUPA_SZE(unit) +  \
     SOC_TMC_ITM_VSQ_GROUPB_SZE(unit) +  \
     SOC_TMC_ITM_VSQ_GROUPC_SZE(unit) +  \
     SOC_TMC_ITM_VSQ_GROUPD_SZE(unit) +  \
     SOC_TMC_ITM_VSQ_GROUPE_SZE(unit) +  \
     SOC_TMC_ITM_VSQ_GROUPF_SZE(unit) - 1)

#define SOC_TMC_ITM_VSQ_NDX_RNG_LAST(unit) (SOC_TMC_ITM_VSQ_NDX_MAX(unit) + 1)

typedef int SOC_TMC_ITM_VSQ_GROUP_SIZE;
typedef int SOC_TMC_ITM_VSQ_NDX_RNG;
typedef enum
{
  
  SOC_TMC_ITM_ADMIT_TST_00=0,
  
  SOC_TMC_ITM_ADMIT_TST_01=1,
  
  SOC_TMC_ITM_ADMIT_TST_02=2,
  
  SOC_TMC_ITM_ADMIT_TST_03=3,
  
  SOC_TMC_ITM_ADMIT_TSTS_LAST
}SOC_TMC_ITM_ADMIT_TSTS;

typedef enum
{
  
  SOC_TMC_ITM_VSQ_GROUP_CTGRY=0,
  
  SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS=1,
  
  SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS=2,
  
  SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG=3,
  
   SOC_TMC_ITM_VSQ_GROUP_LAST_PETRA,
  
   SOC_TMC_ITM_VSQ_GROUP_LLFC = 4,
  
   SOC_TMC_ITM_VSQ_GROUP_PFC = 5,
  
   SOC_TMC_ITM_VSQ_GROUP_SRC_PORT = 4,
  
   SOC_TMC_ITM_VSQ_GROUP_PG = 5,
  
   SOC_TMC_ITM_VSQ_GROUP_LAST_ARAD
}SOC_TMC_ITM_VSQ_GROUP;

#define SOC_TMC_NOF_VSQ_GROUPS             SOC_TMC_ITM_VSQ_GROUP_LAST_ARAD
#define SOC_TMC_NOF_SRC_BASED_VSQ_GROUPS  (SOC_TMC_ITM_VSQ_GROUP_PG - SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG)
#define SOC_TMC_NOF_NON_SRC_BASED_VSQ_GROUPS  (SOC_TMC_NOF_VSQ_GROUPS - SOC_TMC_NOF_SRC_BASED_VSQ_GROUPS)
#define SOC_TMC_NOF_VSQ_PG_MAPPING_PROFILES 16
typedef struct
{
  uint32 c2;
  uint32 c3;
  uint32 c1;
  uint32 max_avrg_th;
  uint32 min_avrg_th;
  uint32 vq_wred_pckt_sz_ignr;
  uint32 vq_max_szie_bds_mnt;
  uint32 vq_max_szie_bds_exp;
  uint32 vq_max_size_words_mnt;
  uint32 vq_max_size_words_exp;
} SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA;

typedef enum
{
  
  SOC_TMC_ITM_STAG_ENABLE_MODE_DISABLED=0,
  
  SOC_TMC_ITM_STAG_ENABLE_MODE_STAT_IF_NO_DEQ=1,
  
  SOC_TMC_ITM_STAG_ENABLE_MODE_ENABLED_WITH_DEQ=2,
  
  SOC_TMC_ITM_NOF_STAG_ENABLE_MODES=3
}SOC_TMC_ITM_STAG_ENABLE_MODE;

typedef enum {
    SOC_TMC_INGRESS_THRESHOLD_INVALID = -1,
    SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES = 0,
    SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES, 
    SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS = SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES, 
    SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS,
    SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES
} SOC_TMC_INGRESS_THRESHOLD_TYPE_E;

#define SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES    (SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS - SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES)


typedef enum {
    SOC_TMC_INGRESS_DRAM_BOUND = 0,
    SOC_TMC_INGRESS_DRAM_BOUND_RECOVERY_FAILURE,
    SOC_TMC_INGRESS_DRAM_BOUND_NOF_TYPES
} SOC_TMC_INGRESS_DRAM_BOUND_TYPE_E;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ITM_DBUFF_SIZE_BYTES dbuff_size;
  
  uint32 uc_nof_buffs;
  
  uint32 full_mc_nof_buffs;
  
  uint32 mini_mc_nof_buffs;
}SOC_TMC_ITM_DRAM_BUFFERS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_THRESH_WITH_HYST_INFO hp;
  
  SOC_TMC_THRESH_WITH_HYST_INFO lp;

}SOC_TMC_ITM_GLOB_RCS_FC_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE bdbs;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE unicast;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE full_mc;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE mini_mc;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE ocb;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE ocb_p0;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE ocb_p1;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE mix_p0;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE mix_p1;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE ocb_pdb;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE p0;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE p1;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE p0_pd;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE p1_pd;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE p0_byte;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE p1_byte;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE hdrm;
  
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE hdrm_pd;

}SOC_TMC_ITM_GLOB_RCS_FC_TH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_THRESH_WITH_HYST_INFO bdbs[SOC_TMC_NOF_DROP_PRECEDENCE];
  SOC_TMC_THRESH_WITH_HYST_INFO ocb_bdbs[SOC_TMC_NOF_DROP_PRECEDENCE];
  
  SOC_TMC_THRESH_WITH_HYST_INFO bds[SOC_TMC_NOF_DROP_PRECEDENCE];
  SOC_TMC_THRESH_WITH_HYST_INFO ocb_bds[SOC_TMC_NOF_DROP_PRECEDENCE];
  
  SOC_TMC_THRESH_WITH_HYST_INFO unicast[SOC_TMC_NOF_DROP_PRECEDENCE];
  
  SOC_TMC_THRESH_WITH_HYST_INFO full_mc[SOC_TMC_NOF_DROP_PRECEDENCE];
  
  SOC_TMC_THRESH_WITH_HYST_INFO mini_mc[SOC_TMC_NOF_DROP_PRECEDENCE];
  
  SOC_TMC_THRESH_WITH_HYST_INFO mem_excess[SOC_TMC_NOF_DROP_PRECEDENCE];
  SOC_TMC_THRESH_WITH_HYST_INFO ocb_mem_excess[SOC_TMC_NOF_DROP_PRECEDENCE];
  
  SOC_TMC_THRESH_WITH_HYST_INFO ocb_uc[SOC_TMC_NOF_DROP_PRECEDENCE];
  
  SOC_TMC_THRESH_WITH_HYST_INFO ocb_mc[SOC_TMC_NOF_DROP_PRECEDENCE];

  
  SOC_TMC_THRESH_WITH_HYST_INFO ocb_shrd_pool[SOC_TMC_ITM_NOF_RSRC_POOLS][SOC_TMC_NOF_DROP_PRECEDENCE];
  SOC_TMC_THRESH_WITH_HYST_INFO mix_shrd_pool[SOC_TMC_ITM_NOF_RSRC_POOLS][SOC_TMC_NOF_DROP_PRECEDENCE];

    
  
  SOC_TMC_THRESH_WITH_HYST_INFO global_shrd_pool_0[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES][SOC_TMC_NOF_DROP_PRECEDENCE];
  SOC_TMC_THRESH_WITH_HYST_INFO global_shrd_pool_1[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES][SOC_TMC_NOF_DROP_PRECEDENCE];

  
  SOC_TMC_THRESH_WITH_HYST_INFO global_free_sram[SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES][SOC_TMC_NOF_DROP_PRECEDENCE];
  SOC_TMC_THRESH_WITH_HYST_INFO global_free_sram_only[SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES][SOC_TMC_NOF_DROP_PRECEDENCE];

  
  SOC_TMC_THRESH_WITH_HYST_INFO global_free_dram[SOC_TMC_NOF_DROP_PRECEDENCE];

}SOC_TMC_ITM_GLOB_RCS_DROP_TH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ITM_QT_NDX cr_req_type_ndx;
  
  
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX credit_cls;
  
  uint32 rate_cls;
  
  uint32 vsq_connection_cls;
  
  uint32 vsq_traffic_cls;
  
  uint32 signature;
}SOC_TMC_ITM_QUEUE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint32 pq_inst_que_size[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];

  
  uint32 pq_inst_que_buff_size;
}SOC_TMC_ITM_QUEUE_DYN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vsq_ctgry0_end;
  
  uint32 vsq_ctgry1_end;
  
  uint32 vsq_ctgry2_end;
}SOC_TMC_ITM_CATEGORY_RNGS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 ctgry_test_en;
  
  uint8 ctgry_trffc_test_en;
  
  uint8 ctgry2_3_cnctn_test_en;
  
  uint8 sttstcs_tag_test_en;
   
  uint8 pfc_test_en;
   
  uint8 llfc_test_en;
}SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT test_a;
  
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT test_b;
}SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  int32 off_to_slow_th;
  
  int32 off_to_normal_th;
  
  int32 slow_to_normal_th;
  
  int32 normal_to_slow_th;
  
  uint32 multiplier;
}SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 backoff_enter_th;
  
  uint32 backoff_exit_th;
}SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 backlog_enter_th;
  
  uint32 backlog_exit_th;
}SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 backslow_enter_th;
  
  uint32 backslow_exit_th;
}SOC_TMC_ITM_CR_REQUEST_BACKSLOW_TH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  int32 satisfied_empty_q_th;
  
  int32 max_credit_balance_empty_q;
  
  uint8 exceed_max_empty_q;
}SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH backoff_th;
  
  SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH backlog_th;
  
    SOC_TMC_ITM_CR_REQUEST_BACKSLOW_TH backslow_th;
  
  SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH empty_queues;
}SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 cr_wd_stts_msg_gen;
  
  uint32 cr_wd_dlt_q_th;
}SOC_TMC_ITM_CR_WD_Q_TH;

#define SOC_TMC_ITM_CR_SLOW_LEVELS 7
#define SOC_TMC_ITM_CR_SLOW_LEVEL_VAL_MAX 0xfff

typedef struct
{
  int slow_level_thresh_up[SOC_TMC_ITM_CR_SLOW_LEVELS];
  int slow_level_thresh_down[SOC_TMC_ITM_CR_SLOW_LEVELS];
} SOC_TMC_ITM_CR_SLOW_LEVEL_THRESHOLDS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ITM_CR_WD_Q_TH wd_th;
  
  SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH hungry_th;
  
  SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH satisfied_th;
  
  uint8 is_low_latency;
  
  uint8 is_remote_credit_value;
  uint8 is_ocb_only;
  uint8 is_high_priority;
  uint32 is_high_bandwidth_profile;
  SOC_TMC_ITM_CR_SLOW_LEVEL_THRESHOLDS slow_level_thresholds;
}SOC_TMC_ITM_CR_REQUEST_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  int32 discount;
}SOC_TMC_ITM_CR_DISCOUNT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 wred_en;
  
  uint32 exp_wq;
}SOC_TMC_ITM_VSQ_WRED_GEN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 wred_en;
  
  uint8 ignore_packet_size;
  
  uint32 min_avrg_th;
  
  uint32 max_avrg_th;
  
  uint32 max_packet_size;
  
  uint32 max_probability;
}SOC_TMC_ITM_WRED_QT_DP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 max_inst_q_size;
  
  uint32 max_inst_q_size_bds;
}SOC_TMC_ITM_TAIL_DROP_INFO;

typedef struct
{
    uint32 max_threshold[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];

    uint32 min_threshold[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];

    int32 adjust_factor[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];

  
    
  uint32 max_inst_q_size_bds;
    
  int8 adjust_factor_bds;

} SOC_TMC_ITM_FADT_DROP_INFO;

typedef struct
{
    uint32 max_threshold;
    uint32 min_threshold;
    int alpha;

    uint32 free_max_threshold;
    uint32 free_min_threshold;

} SOC_TMC_ITM_DRAM_BOUND_THRESHOLD;


typedef struct
{
    uint32 max_threshold;
    uint32 min_threshold;
    int alpha;

    uint32 free_min_threshold;

} SOC_TMC_ITM_OCB_FADT_DROP_THRESHOLD;


typedef struct 
{
    
    SOC_TMC_ITM_DRAM_BOUND_THRESHOLD sram_words_dram_threshold[SOC_TMC_INGRESS_DRAM_BOUND_NOF_TYPES];
    SOC_TMC_ITM_DRAM_BOUND_THRESHOLD sram_pds_dram_threshold[SOC_TMC_INGRESS_DRAM_BOUND_NOF_TYPES];

    uint32 qsize_recovery_th;

} SOC_TMC_ITM_DRAM_BOUND_INFO;


#define SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET ((uint32)-1)

typedef struct
{
  
    
  uint32 guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
  
  uint32 guaranteed_size_bds;

} SOC_TMC_ITM_GUARANTEED_INFO;

typedef struct soc_dpp_guaranteed_pair_s {
    uint32  total;
    uint32  used; 
} soc_dpp_guaranteed_pair_t;


typedef struct  {
    soc_dpp_guaranteed_pair_t guaranteed[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_TMC_ITM_GUARANTEED_RESOURCE;




typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 min_scan_cycle_period_micro;
  
  uint32 max_flow_msg_gen_rate_nano;
  
  uint32 bottom_queue;
  
  uint32 top_queue;
}SOC_TMC_ITM_CR_WD_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_THRESH_WITH_HYST_INFO q_size_fc;
  
  SOC_TMC_THRESH_WITH_HYST_INFO bd_size_fc;
  
  
  SOC_TMC_THRESH_WITH_HYST_INFO size_fc[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
  
  SOC_TMC_THRESH_WITH_FADT_HYST_INFO fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
}SOC_TMC_ITM_VSQ_FC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 max_inst_q_size;
  
  uint32 max_inst_q_size_bds;
  
  int32 alpha;

  
  
  uint32 max_inst_q_size_th[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
}SOC_TMC_ITM_VSQ_TAIL_DROP_INFO;

typedef struct
{
  
  soc_field_t max_field;
  
  soc_field_t min_field;
  
  soc_field_t alpha_field;
} SOC_TMC_ITM_VSQ_FADT_FIELDS_INFO;

typedef struct
{
    
    uint32 max_guaranteed[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
    
    uint32 max_shared[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
    
    uint32 max_headroom[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_TMC_ITM_VSQ_SRC_PORT_INFO;

typedef struct
{
    
    uint32 max_headroom;
    
    uint32 max_headroom_nominal;
    
    uint32 max_headroom_extension;
} SOC_TMC_ITM_VSQ_PG_HDRM_INFO;

typedef struct
{
    
    uint32 max_guaranteed[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
    
    SOC_TMC_FADT_INFO max_shared[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
    
    SOC_TMC_ITM_VSQ_PG_HDRM_INFO max_headroom[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_TMC_ITM_VSQ_PG_INFO;


typedef struct
{
  
  uint32 sign;
  
  uint32 exp;
  
  uint32 offset;
  
  uint32 floor;
} SOC_TMC_ITM_VSQ_FADT_FC_INFO;

typedef struct
{
  
  uint8 pool_id;
  
  uint8 is_lossles;
   
  uint8 use_min_port;
  
  SOC_TMC_ITM_VSQ_FADT_FC_INFO fadt_fc;
  
  
  uint8 admit_profile;

} SOC_TMC_ITM_VSQ_PG_PRM;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ITM_STAG_ENABLE_MODE enable_mode;
  
  uint32 vsq_index_msb;
  
  uint32 vsq_index_lsb;
  
  uint8 dropp_en;
  
  uint32 dropp_lsb;

}SOC_TMC_ITM_STAG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 q_num_low;
  
  uint32 q_num_high;

}SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE q_range;
  
  uint32 rate;
  
  uint32 sch_port;

}SOC_TMC_ITM_INGRESS_SHAPE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 map[SOC_TMC_ITM_PRIO_MAP_SIZE_IN_UINT32S];

}SOC_TMC_ITM_PRIORITY_MAP_TMPLT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 drop_probs[SOC_TMC_ITM_SYS_RED_DRP_PROBS];

}SOC_TMC_ITM_SYS_RED_DROP_PROB;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint32 adm_th;
  
  uint32 prob_th;
  
  uint32 drp_th;
  
  uint32 drp_prob_low;
  
  uint32 drp_prob_high;

}SOC_TMC_ITM_SYS_RED_QT_DP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 queue_size_boundaries[SOC_TMC_ITM_SYS_RED_Q_SIZE_RANGES];

}SOC_TMC_ITM_SYS_RED_QT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint32 aging_timer;
  
  uint8 reset_expired_q_size;
  
  uint8 aging_only_dec_q_size;
}SOC_TMC_ITM_SYS_RED_EG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 unicast_rng_ths[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS - 1];
  
  uint32 multicast_rng_ths[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS - 1];
  
  uint32 bds_rng_ths[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS - 1];
  
  uint32 ocb_rng_ths[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS - 1];

  
  uint32 ocb_pdbs_rng_ths[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS - 1];

}SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 unicast_rng_vals[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS];
  
  uint32 multicast_rng_vals[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS];
  
  uint32 bds_rng_vals[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS];
  
  uint32 ocb_rng_vals[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS];
  
  uint32 ocb_pdbs_rng_vals[SOC_TMC_ITM_SYS_RED_BUFFS_RNGS];
}SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS thresholds;
  
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS values;
}SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO;

typedef struct 
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint8 new_tc[SOC_TMC_NOF_TRAFFIC_CLASSES];
}SOC_TMC_ITM_TC_MAPPING;


typedef struct 
{
  
  uint32 bdb_free;
  
  uint32 bd_occupied;
  
  uint32 bd2_free;
	
  
  uint32 db_uni_occupied;
  
  uint32 db_uni_free;
  
  uint32 db_full_mul_free;
  
  uint32 db_mini_mul_free;

  
  uint32 free_bdb_mini_occu;
  
  uint32 free_db_uni_mini_occu;
  
  uint32 free_bdb_full_mul_mini_occu;
  
  uint32 free_bdb_mini_mul_mini_occu;

  
  
  uint32 min_bdb_free;
  
  uint32 sram_buf_free;
  
  uint32 sram_buf_min_free;
  
  uint32 sram_pdbs_free;
  
  uint32 sram_pdbs_min_free;


}SOC_TMC_ITM_CGM_CONGENSTION_STATS;

typedef struct
{
    
    uint32 pool_0;
    
    uint32 pool_1;
    
    uint32 headroom;
    
    uint32 nominal_headroom;
    
    uint32 reserved;
    
    uint32 total;
} SOC_TMC_ITM_INGRESS_CONGESTION_RESOURCE;

typedef struct
{
    
    
    SOC_TMC_ITM_INGRESS_CONGESTION_RESOURCE dram;
    
    SOC_TMC_ITM_INGRESS_CONGESTION_RESOURCE ocb;
    
    
    SOC_TMC_ITM_INGRESS_CONGESTION_RESOURCE global[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_TMC_ITM_INGRESS_CONGESTION_MGMT;

typedef struct
{
    
    
    uint32 dram_reserved;
    
    uint32 ocb_reserved;
    
    
    uint32 reserved[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES];
} SOC_TMC_ITM_INGRESS_RESERVED_RESOURCE;


typedef enum
{
    SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_BDB = 0,
    SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_OCB_BUFFERS = 1,
    SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_MINI_MC_BUFFERS = 2,
    SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_FULL_MC_BUFFERS = 3,
    SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_VOQ_DRAM_BDB = 4,
    SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_VOQ_OCB_BDB = 5,
    SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_SRAM_BUFFERS = 6,
    SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_SRAM_PDB = 7
} SOC_TMC_ITM_CGM_RSRC_STAT_TYPE;


typedef struct
{
    soc_gport_t dest_gport; 
    int cosq;
    uint64 latency;  
    uint32 latency_flow;
}SOC_TMC_MAX_LATENCY_PACKETS_INFO;











int  tmc_itm_mantissa_exp_field_set(
    int                  unit,
    soc_mem_t mem_id,
    soc_field_t field_id,
    int mantissa_bits,
    int exp_bits,
    int factor,
    int round_up,
    void *data,
    uint32 threshold, 
    uint32* result_threshold
        );

void  tmc_itm_mantissa_exp_field_get(
    int                  unit,
    soc_mem_t mem_id,
    soc_field_t field_id,
    int mantissa_bits,
    int exp_bits,
    int factor,
    void *data,
    uint32* result_threshold
        );

int soc_tmc_itm_voq_threshold_resolution_get(int unit, SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type, int* resolution);

void
  SOC_TMC_ITM_DRAM_BUFFERS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BUFFERS_INFO *info
  );

void
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_FC_TYPE *info
  );

void
  SOC_TMC_ITM_GLOB_RCS_FC_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_FC_TH *info
  );

void
  SOC_TMC_ITM_GLOB_RCS_DROP_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  );

void
  SOC_TMC_ITM_QUEUE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_INFO *info
  );

void
  SOC_TMC_ITM_CATEGORY_RNGS_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CATEGORY_RNGS *info
  );

void
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_clear(
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT *info
  );

void
  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH *info
  );

void
  SOC_TMC_ITM_CR_WD_Q_TH_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_WD_Q_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_REQUEST_INFO *info
  );

void
  SOC_TMC_ITM_CR_DISCOUNT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_DISCOUNT_INFO *info
  );

void
  SOC_TMC_ITM_WRED_QT_DP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info
  );

void
  SOC_TMC_ITM_TAIL_DROP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_TAIL_DROP_INFO *info
  );

void
  SOC_TMC_ITM_FADT_DROP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO *info
  );

void
  SOC_TMC_ITM_DRAM_BOUND_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BOUND_INFO *info
  );

SOC_TMC_ITM_DRAM_BOUND_THRESHOLD* SOC_TMC_ITM_DRAM_BOUND_INFO_thresh_get(
    int                 unit,
    SOC_TMC_ITM_DRAM_BOUND_INFO* info,
    SOC_TMC_INGRESS_DRAM_BOUND_TYPE_E dram_thresh,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E resource_type);

void
SOC_TMC_ITM_VSQ_PG_PRM_clear (
   SOC_TMC_ITM_VSQ_PG_PRM *info
);
void
  SOC_TMC_ITM_VSQ_SRC_PORT_INFO_clear (
     SOC_TMC_ITM_VSQ_SRC_PORT_INFO  *info
  );
void
  SOC_TMC_ITM_VSQ_PG_INFO_clear (
     SOC_TMC_ITM_VSQ_PG_INFO  *info
  );
void
  SOC_TMC_ITM_CR_WD_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_CR_WD_INFO *info
  );

void
  SOC_TMC_ITM_VSQ_FC_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO *info
  );

void
  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO *info
  );

void
  SOC_TMC_ITM_VSQ_WRED_GEN_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_WRED_GEN_INFO *info
  );

void
  SOC_TMC_ITM_STAG_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_STAG_INFO *info
  );

void
  SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE_clear(
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE *info
  );

void
  SOC_TMC_ITM_INGRESS_SHAPE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_SHAPE_INFO *info
  );

void
  SOC_TMC_ITM_PRIORITY_MAP_TMPLT_clear(
    SOC_SAND_OUT SOC_TMC_ITM_PRIORITY_MAP_TMPLT *info
  );

void
  SOC_TMC_ITM_SYS_RED_DROP_PROB_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_DROP_PROB *info
  );

void
  SOC_TMC_ITM_SYS_RED_QT_DP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_QT_DP_INFO *info
  );

void
  SOC_TMC_ITM_SYS_RED_QT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_QT_INFO *info
  );

void
  SOC_TMC_ITM_SYS_RED_EG_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_EG_INFO *info
  );

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS *info
  );

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS *info
  );

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO *info
  );
void
  SOC_TMC_ITM_TC_MAPPING_clear(
    SOC_SAND_OUT SOC_TMC_ITM_TC_MAPPING *info
  );
#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_ITM_QT_NDX_to_string(
    SOC_SAND_IN  SOC_TMC_ITM_QT_NDX enum_val
  );

const char*
  SOC_TMC_ITM_CR_DISCNT_CLS_NDX_to_string(
    SOC_SAND_IN  SOC_TMC_ITM_CR_DISCNT_CLS_NDX enum_val
  );

const char*
  SOC_TMC_ITM_DBUFF_SIZE_BYTES_to_string(
    SOC_SAND_IN SOC_TMC_ITM_DBUFF_SIZE_BYTES enum_val
  );

const char*
  SOC_TMC_ITM_VSQ_GROUP_SIZE_to_string(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_TMC_ITM_VSQ_GROUP_SIZE enum_val
  );

const char*
  SOC_TMC_ITM_VSQ_NDX_RNG_to_string(
     SOC_SAND_IN  int unit,
     SOC_SAND_IN SOC_TMC_ITM_VSQ_NDX_RNG enum_val
  );

const char*
  SOC_TMC_ITM_ADMIT_TSTS_to_string(
    SOC_SAND_IN SOC_TMC_ITM_ADMIT_TSTS enum_val
  );

const char*
  SOC_TMC_ITM_VSQ_GROUP_to_string(
    SOC_SAND_IN SOC_TMC_ITM_VSQ_GROUP enum_val
  );

void
  SOC_TMC_ITM_DRAM_BUFFERS_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_DRAM_BUFFERS_INFO *info
  );

void
  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_print(
    SOC_SAND_IN SOC_TMC_ITM_GLOB_RCS_FC_TYPE *info
  );

void
  SOC_TMC_ITM_GLOB_RCS_FC_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_GLOB_RCS_FC_TH *info
  );

void
  SOC_TMC_ITM_GLOB_RCS_DROP_TH_print_no_table(
    SOC_SAND_IN SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  );

void
  SOC_TMC_ITM_GLOB_RCS_DROP_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  );

void
  SOC_TMC_ITM_QUEUE_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_QUEUE_INFO *info
  );

void
  SOC_TMC_ITM_CATEGORY_RNGS_print(
    SOC_SAND_IN SOC_TMC_ITM_CATEGORY_RNGS *info
  );

void
  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_print(
    SOC_SAND_IN SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT *info
  );

void
  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH *info
  );

void
  SOC_TMC_ITM_CR_WD_Q_TH_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_WD_Q_TH *info
  );

void
  SOC_TMC_ITM_CR_REQUEST_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_REQUEST_INFO *info
  );

void
  SOC_TMC_ITM_CR_DISCOUNT_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_DISCOUNT_INFO *info
  );

void
  SOC_TMC_ITM_WRED_QT_DP_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_WRED_QT_DP_INFO *info
  );

void
  SOC_TMC_ITM_TAIL_DROP_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_TAIL_DROP_INFO *info
  );

void
  SOC_TMC_ITM_CR_WD_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_CR_WD_INFO *info
  );

void
  SOC_TMC_ITM_VSQ_FC_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_VSQ_FC_INFO *info
  );

void
  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_VSQ_TAIL_DROP_INFO *info
  );

void
  SOC_TMC_ITM_VSQ_WRED_GEN_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_VSQ_WRED_GEN_INFO *info
  );

void
  SOC_TMC_ITM_STAG_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_STAG_INFO *info
  );

void
  SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE_print(
    SOC_SAND_IN SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE *info
  );

void
  SOC_TMC_ITM_INGRESS_SHAPE_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_INGRESS_SHAPE_INFO *info
  );

void
  SOC_TMC_ITM_PRIORITY_MAP_TMPLT_print(
    SOC_SAND_IN SOC_TMC_ITM_PRIORITY_MAP_TMPLT *info
  );

void
  SOC_TMC_ITM_SYS_RED_DROP_PROB_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_DROP_PROB *info
  );

void
  SOC_TMC_ITM_SYS_RED_QT_DP_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_QT_DP_INFO *info
  );

void
  SOC_TMC_ITM_SYS_RED_QT_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_QT_INFO *info
  );

void
  SOC_TMC_ITM_SYS_RED_EG_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_EG_INFO *info
  );

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS *info
  );

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS *info
  );

void
  SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO_print(
    SOC_SAND_IN SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO *info
  );

#endif 




typedef enum
{
    SOC_PKT_SIZE_ADJUST_SRC_SCHEDULER,
    SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM_IRPP,
    SOC_PKT_SIZE_ADJUST_SRC_CRPS_IQM,
    SOC_PKT_SIZE_ADJUST_SRC_STAT_REPOR_IN,
    SOC_PKT_SIZE_ADJUST_SRC_STAT_REPOR_OUT
} SOC_COMPENSATION_PKT_SIZE_SRC_TYPE;

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
