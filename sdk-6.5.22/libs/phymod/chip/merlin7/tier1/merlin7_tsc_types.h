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

/** @file merlin7_tsc_types.h
 * Common types used by Serdes API functions
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MERLIN7_TSC_API_TYPES_H
#define MERLIN7_TSC_API_TYPES_H

#include "merlin7_tsc_ipconfig.h"
#include "merlin7_tsc_usr_includes.h"

typedef struct merlin7_tsc_triage_info_st {
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
} merlin7_tsc_triage_info;



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
struct merlin7_tsc_uc_lane_config_field_st {
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
  uint16_t afe_char_mode_enable;
  uint16_t channel_est_mode_enable;
};
#define UC_LANE_CONFIG_FIELD_INIT {0,0,0,0,0,0,0,0,0,0,0,0}

/** Core Config Variable Structure in Microcode */
struct merlin7_tsc_uc_core_config_field_st {
  uint8_t  core_cfg_from_pcs ;
  uint8_t  vco_rate          ;
  uint8_t  an_los_workaround ;
  uint8_t  reserved1         ;
  uint8_t  reserved2         ;
};
#define UC_CORE_CONFIG_FIELD_INIT {0,0,0,0,0}

/** Lane Config Struct */
struct  merlin7_tsc_uc_lane_config_st {
  struct merlin7_tsc_uc_lane_config_field_st field;
  uint16_t word;
};
#define UC_LANE_CONFIG_INIT {UC_LANE_CONFIG_FIELD_INIT, 0}

/** Core Config Struct */
struct  merlin7_tsc_uc_core_config_st {
  struct merlin7_tsc_uc_core_config_field_st field;
  uint16_t word;
  int32_t vco_rate_in_Mhz; /* if >0 then will get converted and replace field.vco_rate when update is called */
};
#define UC_CORE_CONFIG_INIT {UC_CORE_CONFIG_FIELD_INIT, 0, 0}

/** Lane User Control Function Structure in Microcode */
struct merlin7_tsc_usr_ctrl_func_st {
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
struct merlin7_tsc_usr_ctrl_dfe_func_st {
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
struct merlin7_tsc_usr_ctrl_disable_functions_st {
  struct merlin7_tsc_usr_ctrl_func_st field;
  uint8_t  byte;
};

/** Lane User Control Disable DFE Function Struct */
struct merlin7_tsc_usr_ctrl_disable_dfe_functions_st {
  struct merlin7_tsc_usr_ctrl_dfe_func_st field;
  uint8_t  byte;
};
struct ber_data_st {
    uint64_t num_errs;
    uint64_t num_bits;
    uint8_t lcklost;
    uint8_t prbs_chk_en;
    uint8_t cdrlcklost;
    uint8_t prbs_lck_state;
};

struct check_platform_info_st {
    char c;
    uint32_t d;
};

#define BER_DATA_ST_INIT {0, 0, 0, 0, 0, 0}

#define NUM_ILV 2

#define DATA_ARR_SIZE 6
#define PHASE_ARR_SIZE 3

#define DFE_ARR_SIZE 26

struct merlin7_tsc_detailed_lane_status_st {
    /* =========== 32-bit variables start =========== */
    /** PRBS Error Count */
    uint32_t prbs_chk_errcnt;
    /* =========== 32-bit variables end ============= */
    /* =========== 16-bit variables start =========== */
    /** One Hot DSC State */
    uint16_t dsc_sm[2];
    /** Horizontal left eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16_t heye_left;
    /** Horizontal right eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16_t heye_right;
    /** Vertical upper eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16_t veye_upper;
    /** Vertical lower eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16_t veye_lower;
    /** Link time in milliseconds */
    uint16_t link_time;
    /** Frequency offset of local reference clock with respect to RX data in ppm */
    int16_t ppm;
    /** Estimate for Main tap */
    int16_t main_tap_est;
    /** RX PI counter value (Phase Error) for data slicer */
    uint16_t rx_pi_cnt_perr;
    /** uC lane configuration */
    uint16_t lane_config_word;
    /** Vertical threshold voltage of p1 slicer (mV) */
    int16_t p1_lvl;
    /** Vertical threshold voltage of m1 slicer (mV) [Used to read out 'channel loss hint' in PAM4 mode] */
    int16_t m1_lvl;
    /** Frequency offset of local reference clock with respect to TX data in ppm */
    int16_t tx_ppm;
    /* =========== 16-bit variables end ============ */
    /* =========== 8-bit variables start =========== */
    /** Order of PRBS Checker */
    uint8_t prbs_chk_order;
    /** Temperature Index */
    uint8_t  temp_idx;
    /** TX driver to disable high voltage control */
    uint8_t  ams_tx_drv_hv_disable;
    /** Analog Resistor Calibration value */
    uint8_t  ams_tx_ana_rescal;
    /* Analog Amplitude control */
    uint8_t  amp_ctrl;
    /* No of TXFIR Taps Enabled */
    uint8_t num_txfir_taps;
    /** TX equalization FIR pre tap weight */
    uint8_t  pre_tap;
    /** TX equalization FIR main tap weight */
    uint8_t  main_tap;
    /** TX equalization FIR post1 tap weight */
    int8_t   post1_tap;
    /** TX equalization FIR post2 tap weight */
    int8_t   post2_tap;
    /** TX equalization FIR post3 tap weight */
    int8_t   post3_tap;
    /** TX equalization FIR rpara*/
    uint8_t  rpara;
    /** Signal Detect */
    uint8_t  sigdet;
    /** PMD RX Lock */
    uint8_t  pmd_lock;
    /** Restart Count */
    uint8_t restart_count;
    /** Reset Count */
    uint8_t reset_count;
    /**  PMD Lock Count */
    uint8_t pmd_lock_count;
    /** Variable Gain Amplifier settings */
    int8_t  vga;
    /** Main Peaking Filter setting */
    uint8_t  pf;
    /** Low Frequency Peaking Filter settings */
    uint8_t  pf2;
    /** Main Frequency Peaking Filter reg settings */
    uint8_t  pf_reg[3];
    /** Data Slicer Threshold */
    int8_t   data_thresh;
    /** Phase Slicer Threshold */
    int8_t   phase_thresh;
    /** LMS Slicer Threshold */
    int8_t   lms_thresh;
    /** Horizontal offset between data and dataq */
    uint8_t  ddq_hoffset;
    /** Horizontal offset between phase and phaseq */
    uint8_t  ppq_hoffset;
    /** Horizontal offset between lms and lmsq */
    uint8_t  llq_hoffset;
    /** Horizontal offset between data and phase */
    uint8_t  dp_hoffset;
    /** Horizontal offset between data and lms */
    uint8_t  dl_hoffset;
    /** DC offset DAC control value */
    int8_t   dc_offset;
    /** Slicer calibration control codes (datap) */
    int8_t   thctrl_dp[4];
    /** Slicer calibration control codes (datan) */
    int8_t   thctrl_dn[4];
    /** Slicer calibration control codes (phasep) */
    int8_t   thctrl_zp[4];
    /** Slicer calibration control codes (phasen) */
    int8_t   thctrl_zn[4];
    /** Slicer calibration control codes (lms) */
    int8_t   thctrl_l[4];
    /** PRBS Checker Enable */
    uint8_t  prbs_chk_en;
    /** PRBS Checker Invert */
    uint8_t  prbs_chk_inv;
    /** PRBS Checker Lock */
    uint8_t  prbs_chk_lock;
    /** uC stopped state **/
    uint8_t stop_state;
    /** Delay of zero crossing slicer, m1, wrt to data in PI codes */
    int8_t clk90;
    /** Delay of diagnostic/lms slicer, p1, wrt to data in PI codes */
    int8_t clkp1;
    /** Peaking Filter Main Settings */
    int8_t pf_main;
    /** Peaking Filter Hiz mode enable */
    uint8_t pf_hiz;
    /** Low Frequency Peaking filter control */
    int8_t pf2_ctrl;
    /** P1 eyediag status */
    int8_t p1_lvl_ctrl;
    /** DFE tap 1 value */
    int8_t dfe1;
    /** DFE tap 2 value */
    int8_t dfe2;
    /** DFE tap 3 value */
    int8_t dfe3;
    /** DFE tap 4 value */
    int8_t dfe4;
    /** DFE tap 5 value */
    int8_t dfe5;
    /** DFE tap 6 value */
    int8_t dfe6;
    /** DFE tap 1 Duty Cycle Distortion */
    int8_t dfe1_dcd;
    /** DFE tap 2 Duty Cycle Distortion */
    int8_t dfe2_dcd;
    /** Slicer calibration control codes (p1 even) */
    int8_t pe;
    /** Slicer calibration control codes (data even) */
    int8_t ze;
    /** Slicer calibration control codes (m1 even) */
    int8_t me;
    /** Slicer calibration control codes (p1 odd) */
    int8_t po;
    /** Slicer calibration control codes (data odd) */
    int8_t zo;
    /** Slicer calibration control codes (m1 odd) */
    int8_t mo;
    /** uC lane status */
    uint8_t ucv_status;
    /** TX equalization FIR pre tap weight */
    int8_t txfir_pre;
    /** TX equalization FIR main tap weight */
    int8_t txfir_main;
    /** TX equalization FIR post1 tap weight */
    int8_t txfir_post1;
    /** TX equalization FIR post2 tap weight */
    int8_t txfir_post2;
    /** Baud Rate Phase Detector enable */
    uint8_t br_pd_en;
    /** lane_reset_state **/
    uint8_t reset_state;
    /** lane_tx_reset_state **/
    uint8_t tx_reset_state;
    /** Sigdet offset correction - positive **/
    uint8_t soc_pos;
    /** Sigdet offset correction - negative **/
    uint8_t soc_neg;
    /** BER Error Rate string **/
    char ber_string[10];
    /** Big endian */
    uint8_t big_endian;
    /* =========== 8-bit variables end ============== */
};
/* Change the size if merlin7_tsc_detailed_lane_status_st size changes */
/* Add new extended lane struct fields > 1byte to the reverse byte list in the decoder */
#define SIZE_OF_EXTENDED_LANE_STATE 132

#if !defined(EXCLUDE_FOR_SWIG)
typedef struct { 
    uint8_t extended_lane_st_tmp[sizeof(struct merlin7_tsc_detailed_lane_status_st)==SIZE_OF_EXTENDED_LANE_STATE ? 1: -1]; 
} merlin7_tsc_detailed_lane_status_st_size_check;
#endif /* EXCLUDE_FOR_SWIG */

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

