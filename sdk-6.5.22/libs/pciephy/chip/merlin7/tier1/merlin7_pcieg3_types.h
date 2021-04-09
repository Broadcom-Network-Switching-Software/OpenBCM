/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/*****************************************************************************************
 *****************************************************************************************
 *                                                                                       *
 *  Revision      :   *
 *                                                                                       *
 *  Description   :  Common types used by Serdes API functions                           *
 */

/** @file merlin7_pcieg3_types.h
 * Common types used by Serdes API functions
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MERLIN7_PCIEG3_API_TYPES_H
#define MERLIN7_PCIEG3_API_TYPES_H

#include "merlin7_pcieg3_ipconfig.h"
#include "merlin7_pcieg3_usr_includes.h"

typedef struct merlin7_pcieg3_triage_info_st {
   uint32_t     api_ver;
   uint32_t     ucode_ver;
   uint8_t      core;
   uint8_t      lane;

   err_code_t   error;
   uint16_t     line;
   
   uint16_t     stop_status;
   uint16_t     sw_exception;
   uint16_t     hw_exception;
   uint16_t     stack_overflow;
   uint16_t     overflow_lane_id;
   uint16_t     cmd_info;

   uint8_t      pmd_lock;
   uint8_t      pmd_lock_chg;
   uint8_t      sig_det;
   uint8_t      sig_det_chg;
   uint16_t     dsc_one_hot[2];
} merlin7_pcieg3_triage_info;



#define _Y_ (6)
#define _X_ (3)
typedef struct dbgfb_stats_s {
        int32_t  data1[_X_][_Y_];
        uint32_t data2[_X_][_Y_];
        uint32_t data3[_X_][_Y_];
        int32_t  data4[_X_][_Y_];
} dbgfb_data_st;

typedef struct dbgfb_data_s {
    uint32_t time_in_us;
    dbgfb_data_st data;
    uint8_t pam4es_en;
    uint8_t temp[3];
} dbgfb_stats_st;

#if !defined(EXCLUDE_FOR_SWIG)
#define SIZE_OF_DBGFB_STATS_STRUCT 296  /* Change the size if dbgfb_stats_st size changes */
typedef struct { 
    uint8_t dbgfb_tmp[sizeof(dbgfb_stats_st)==SIZE_OF_DBGFB_STATS_STRUCT ? 1: -1]; 
} dbgfb_stats_st_size_check;
#endif

typedef struct dbgfb_cfg_s {
    uint8_t y;
    uint8_t x;
    uint32_t time_in_us;
    int32_t data1;
    uint32_t data2;
    uint32_t data3;
} dbgfb_cfg_st;

#define DBGFB_CFG_ST_INIT {0,0,0,0,0,0}

/*----------------------------------------*/
/*  Lane/Core structs (without bitfields) */
/*----------------------------------------*/

/** Lane Config Variable Structure in Microcode */
struct merlin7_pcieg3_uc_lane_config_field_st {
  uint8_t  lane_cfg_from_pcs ;
  uint8_t  an_enabled        ;
  uint8_t  dfe_on            ;
  uint8_t  force_brdfe_on    ;
  uint8_t  media_type        ;
  uint8_t  unreliable_los    ;
  uint8_t  scrambling_dis    ;
  uint8_t  cl72_auto_polarity_en ;
  uint8_t  cl72_restart_timeout_en;
  uint16_t reserved;
};
#define UC_LANE_CONFIG_FIELD_INIT {0,0,0,0,0,0,0,0,0,0}

/** Core Config Variable Structure in Microcode */
struct merlin7_pcieg3_uc_core_config_field_st {
  uint8_t  core_cfg_from_pcs ;
  uint8_t  vco_rate          ;
  uint8_t  an_los_workaround ;
  uint8_t  reserved1         ;
  uint8_t  reserved2         ;
};
#define UC_CORE_CONFIG_FIELD_INIT {0,0,0,0,0}

/** Lane Config Struct */
struct  merlin7_pcieg3_uc_lane_config_st {
  struct merlin7_pcieg3_uc_lane_config_field_st field;
  uint16_t word;
};
#define UC_LANE_CONFIG_INIT {UC_LANE_CONFIG_FIELD_INIT, 0}

/** Core Config Struct */
struct  merlin7_pcieg3_uc_core_config_st {
  struct merlin7_pcieg3_uc_core_config_field_st field;
  uint16_t word;
  int vco_rate_in_Mhz; /* if >0 then will get converted and replace field.vco_rate when update is called */
};
#define UC_CORE_CONFIG_INIT {UC_CORE_CONFIG_FIELD_INIT, 0, 0}

/** Lane User Control Function Structure in Microcode */
struct merlin7_pcieg3_usr_ctrl_func_st {
  /** Peaking Filter Adaptation */
  uint8_t pf_adaptation            ;
  /** DC Offset Adaptation */
  uint8_t dc_adaptation            ;
  /** Variable Gain Amplifier adaptation */
  uint8_t vga_adaptation           ;
  /** Slicer Offset tuning */
  uint8_t slicer_offset_tuning     ;
  /** Clk90 Offset adaptation */
  uint8_t clk90_offset_adaptation  ;
  /** P1 Level tuning */
  uint8_t p1_level_tuning          ;
  /** Eye adaptation */
  uint8_t eye_adaptation           ;
  /** All adaptation */
  uint8_t all_adaptation           ;
};

/** Lane User DFE Control Function Structure in Microcode */
struct merlin7_pcieg3_usr_ctrl_dfe_func_st {
  /** DFE tap 1 adaptation */
  uint8_t dfe_tap1_adaptation      ;
  /** DFE tap 2 adaptation */
  uint8_t dfe_tap2_adaptation      ;
  /** DFE tap 3 adaptation */
  uint8_t dfe_tap3_adaptation      ;
  /** DFE tap 4 adaptation */
  uint8_t dfe_tap4_adaptation      ;
  /** DFE tap 5 adaptation */
  uint8_t dfe_tap5_adaptation      ;
  /** DFE tap 1 Duty Cycle Distortion */
  uint8_t dfe_tap1_dcd             ;
  /** DFE tap 2 Duty Cycle Distortion */
  uint8_t dfe_tap2_dcd             ;
};

/** Lane User Control Disable Function Struct */
struct merlin7_pcieg3_usr_ctrl_disable_functions_st {
  struct merlin7_pcieg3_usr_ctrl_func_st field;
  uint8_t  byte;
};

/** Lane User Control Disable DFE Function Struct */
struct merlin7_pcieg3_usr_ctrl_disable_dfe_functions_st {
  struct merlin7_pcieg3_usr_ctrl_dfe_func_st field;
  uint8_t  byte;
};
struct ber_data_st {
    uint64_t num_errs;
    uint64_t num_bits;
    uint8_t lcklost;
    uint8_t prbs_chk_en;
    uint8_t cdrlcklost;
};

struct check_platform_info_st {
    char c;
    uint32_t d;
};

#define BER_DATA_ST_INIT {0, 0, 0, 0, 0}

#define NUM_SLICERS 2

#define DATA_ARR_SIZE 6
#define PHASE_ARR_SIZE 3

#define DFE_ARR_SIZE 26


/****************************************************************************
 * @name Diagnostic Sampling
 */
/**@{*/

#ifdef STANDALONE_EVENT
#define DIAG_MAX_SAMPLES (64)
#else
/**
 * Diagnostic sample set size, MERLIN7 variant.
 * Applies to collections of BER measurements, eye margins, etc.
 */
#define DIAG_MAX_SAMPLES (32)
#endif

/**@}*/

#endif
#ifdef __cplusplus
}
#endif

