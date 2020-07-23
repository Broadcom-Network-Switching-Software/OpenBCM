/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:       dscdump.h
 */

#ifndef _DSCDUMP_H_
#define _DSCDUMP_H_

#include "types.h"


/*------------------------------*/
/** Serdes Core State Structure */
/*------------------------------*/
typedef struct {
    /** Core DP Reset State */
    uint8 core_reset;
    /**  PLL Powerdown enable */
    uint8 pll_pwrdn;
    /** Micro active enable */
    uint8 uc_active;
    /** Comclk Frequency in Mhz */
    uint16 comclk_mhz;
    /** uCode Major Version number */
    uint16 ucode_version;
    /** uCode Minor Version number */
    uint8 ucode_minor_version;
    /** AFE Hardware version */
    uint8 afe_hardware_version;
    /** uC Die Temperature Index */
    uint8 temp_idx;
    /** Average Die Temperature (13-bit format) */
    uint16 avg_tmon;
    /** Analog Resistor Calibration value */
    uint8 rescal;
    /** VCO Rate in MHz */
    uint16 vco_rate_mhz;
    /**  Analog VCO Range */
    uint8 analog_vco_range;
    /** PLL Divider value */
    uint8 pll_div;
    /** PLL Lock */
    uint8 pll_lock;
    /** PLL Lock Change */
    uint8 pll_lock_chg;
    /** Live die temperature in Celsius */
    int16 die_temp;
    /** Core Status Variable */
    uint8 core_status;
} bcm_core_state_st;

/*------------------------------*/
/** Serdes OSR Mode Structure   */
/*------------------------------*/
typedef struct {
    /** TX OSR Mode */
    uint8 tx;
    /** RX OSR Mode */
    uint8 rx;
    /** OSR Mode for TX and RX (used when both TX and RX should have same OSR Mode) */
    uint8 tx_rx;
} bcm_osr_mode_st;

/*------------------------------*/
/** Serdes Lane State Structure */
/*------------------------------*/
typedef struct {
    /** uC lane configuration */
    uint16 ucv_config;
    /** Frequency offset of local reference clock with respect to RX data in ppm */
    int16 rx_ppm;
    /** Vertical threshold voltage of p1 slicer (mV) */
    int16 p1_lvl;
    /** Vertical threshold voltage of m1 slicer (mV) */
    int16 m1_lvl;
    /** Link time in milliseconds */
    uint16 link_time;
    /** OSR Mode */
    bcm_osr_mode_st osr_mode;
    /** Signal Detect */
    uint8 sig_det;
    /** Signal Detect Change */
    uint8 sig_det_chg;
    /** PMD RX Lock */
    uint8 rx_lock;
    /** PMD RX Lock Change */
    uint8 rx_lock_chg;
    /** Delay of zero crossing slicer, m1, wrt to data in PI codes */
    int8 clk90;
    /** Delay of diagnostic/lms slicer, p1, wrt to data in PI codes */
    int8 clkp1;
    /** Peaking Filter Main Settings */
    int8 pf_main;
    /** Peaking Filter Hiz mode enable */
    int8 pf_hiz;
    /** Peaking Filter DC gain adjustment for CTLE */
    int8 pf_bst;
    /** Low Frequency Peaking filter control */
    int8 pf2_ctrl;
    /** Variable Gain Amplifier settings */
    int8 vga;
    /** DC offset DAC control value */
    int8 dc_offset;
    /** P1 eyediag status */
    int8 p1_lvl_ctrl;
    /** DFE tap 1 value */
    int8 dfe1;
    /** DFE tap 2 value */
    int8 dfe2;
    /** DFE tap 3 value */
    int8 dfe3;
    /** DFE tap 4 value */
    int8 dfe4;
    /** DFE tap 5 value */
    int8 dfe5;
    /** DFE tap 6 value */
    int8 dfe6;
    /** DFE tap 1 Duty Cycle Distortion */
    int8 dfe1_dcd;
    /** DFE tap 2 Duty Cycle Distortion */
    int8 dfe2_dcd;
    /** Slicer calibration control codes (p1 even) */
    int8 pe;
    /** Slicer calibration control codes (data even) */
    int8 ze;
    /** Slicer calibration control codes (m1 even) */
    int8 me;
    /** Slicer calibration control codes (p1 odd) */
    int8 po;
    /** Slicer calibration control codes (data odd) */
    int8 zo;
    /** Slicer calibration control codes (m1 odd) */
    int8 mo;
    /** Frequency offset of local reference clock with respect to TX data in ppm */
    int16 tx_ppm;
    /** TX equalization FIR pre tap weight */
    int8 txfir_pre;
    /** TX equalization FIR main tap weight */
    int8 txfir_main;
    /** TX equalization FIR post1 tap weight */
    int8 txfir_post1;
    /** TX equalization FIR post2 tap weight */
    int8 txfir_post2;
    /** TX equalization FIR post3 tap weight */
    int8 txfir_post3;
    /** Horizontal left eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16 heye_left;
    /** Horizontal right eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16 heye_right;
    /** Vertical upper eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16 veye_upper;
    /** Vertical lower eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16 veye_lower;
    /** Baud Rate Phase Detector enable */
    uint8 br_pd_en;
    /** lane_reset_state **/
    uint8 reset_state;
    /** uC stopped state **/
    uint8 stop_state;
} bcm_lane_state_st;

#ifdef SERDES_API_FLOATING_POINT 
#define USR_DOUBLE       double 
#else
#define USR_DOUBLE       int
#endif

/* Lane status Struct */
struct bcm_detailed_lane_status_st {
    /** Frequency offset of local reference clock with respect to RX data in ppm */
    USR_DOUBLE ppm;
    /** Estimate for Main tap */
    USR_DOUBLE main_tap_est;
    /** PRBS Error Count */
    uint32 prbs_chk_errcnt;
    /** One Hot DSC State */
    uint16 dsc_sm[2];
    /** Horizontal left eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16 heye_left;
    /** Horizontal right eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16 heye_right;
    /** Vertical upper eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16 veye_upper;
    /** Vertical lower eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
    uint16 veye_lower;
    /** Link time in milliseconds */
    uint16 link_time;
    /** Temperature Index */
    uint8 temp_idx;
    /** TX driver to disable high voltage control */
    uint8 ams_tx_drv_hv_disable;
    /** Analog Resistor Calibration value */
    uint8 ams_tx_ana_rescal;
    /* Analog Amplitude control */
    uint8 amp_ctrl;
    /** TX equalization FIR pre tap weight */
    uint8 pre_tap;
    /** TX equalization FIR main tap weight */
    uint8 main_tap;
    /** TX equalization FIR post1 tap weight */
    uint8 post1_tap;
    /** TX equalization FIR post2 tap weight */
    int8 post2_tap;
    /** TX equalization FIR post3 tap weight */
    int8 post3_tap;
    /** Signal Detect */
    uint8 sigdet;
    /** PMD RX Lock */
    uint8 pmd_lock;
    /** PLL Select */
    uint8 pmd_pll_select;
    /** Variable Gain Amplifier settings */
    uint8 vga;
    /** Main Peaking Filter setting */
    uint8 pf;
    /** Low Frequency Peaking Filter settings */
    uint8 pf2;
    /** Data Slicer Threshold */
    int8 data_thresh;
    /** Phase Slicer Threshold */
    int8 phase_thresh;
    /** LMS Slicer Threshold */
    int8 lms_thresh;
    /** Horizontal offset between data and dataq */
    uint8 ddq_hoffset;
    /** Horizontal offset between phase and phaseq */
    uint8 ppq_hoffset;
    /** Horizontal offset between lms and lmsq */
    uint8 llq_hoffset;
    /** Horizontal offset between data and phase */
    uint8 dp_hoffset;
    /** Horizontal offset between data and lms */
    uint8 dl_hoffset;
    /** DFE tap values */
    int8 dfe[26][4];
    /** DC offset DAC control value */
    int8 dc_offset;
    /** Slicer calibration control codes (datap) */
    int8 thctrl_dp[4];
    /** Slicer calibration control codes (datan) */
    int8 thctrl_dn[4];
    /** Slicer calibration control codes (phasep) */
    int8 thctrl_zp[4];
    /** Slicer calibration control codes (phasen) */
    int8 thctrl_zn[4];
    /** Slicer calibration control codes (lms) */
    int8 thctrl_l[4];
    /** PRBS Checker Enable */
    uint8 prbs_chk_en;
    /** Order of PRBS Checker */
    uint8 prbs_chk_order;
    /** PRBS Checker Lock */
    uint8 prbs_chk_lock;
    /** lane_reset_state **/
    uint8 reset_state;
    /** uC stopped state **/
    uint8 stop_state;
};

/** TX AFE Settings Enum */
enum srds_tx_afe_settings_enum {
    TX_AFE_PRE,
    TX_AFE_MAIN,
    TX_AFE_POST1,
    TX_AFE_POST2,
    TX_AFE_POST3,
    TX_AFE_AMP,
    TX_AFE_DRIVERMODE
};

/** Evenet Log Display Mode Enum */
enum srds_event_log_display_mode_enum {
    EVENT_LOG_HEX,
    EVENT_LOG_DECODED,
    EVENT_LOG_HEX_AND_DECODED,
    EVENT_LOG_MODE_MAX
};

/** Event Log Code Enum */
enum srds_event_code_enum {
    EVENT_CODE_UNKNOWN = 0,
    EVENT_CODE_ENTRY_TO_DSC_RESET,
    EVENT_CODE_RELEASE_USER_RESET,
    EVENT_CODE_EXIT_FROM_DSC_RESET,
    EVENT_CODE_ENTRY_TO_CORE_RESET,
    EVENT_CODE_RELEASE_USER_CORE_RESET,
    EVENT_CODE_ACTIVE_RESTART_CONDITION,
    EVENT_CODE_EXIT_FROM_RESTART,
    EVENT_CODE_WRITE_TR_COARSE_LOCK,
    EVENT_CODE_CL72_READY_FOR_COMMAND,
    EVENT_CODE_EACH_WRITE_TO_CL72_TX_CHANGE_REQUEST,
    EVENT_CODE_FRAME_LOCK,
    EVENT_CODE_LOCAL_RX_TRAINED,
    EVENT_CODE_DSC_LOCK,
    EVENT_CODE_FIRST_RX_PMD_LOCK,
    EVENT_CODE_PMD_RESTART_FROM_CL72_CMD_INTF_TIMEOUT,
    EVENT_CODE_LP_RX_READY,
    EVENT_CODE_STOP_EVENT_LOG,
    EVENT_CODE_GENERAL_EVENT_0,
    EVENT_CODE_GENERAL_EVENT_1,
    EVENT_CODE_GENERAL_EVENT_2,
    EVENT_CODE_ERROR_EVENT,
    EVENT_CODE_NUM_TIMESTAMP_WRAPAROUND_MAXOUT,
    EVENT_CODE_RESTART_PMD_ON_CDR_LOCK_LOST,
    EVENT_CODE_SM_STATUS_RESTART,
    EVENT_CODE_CORE_PROGRAMMING,
    EVENT_CODE_LANE_PROGRAMMING,
    EVENT_CODE_RESTART_PMD_ON_CLOSE_EYE,
    EVENT_CODE_RESTART_PMD_ON_DFE_TAP_CONFIG,
    EVENT_CODE_CL72_AUTO_POLARITY_CHANGE,
    EVENT_CODE_RESTART_FROM_CL72_MAX_TIMEOUT,
    EVENT_CODE_CL72_LOCAL_TX_CHANGED,
    EVENT_CODE_FIRST_WRITE_TO_CL72_TX_CHANGE_REQUEST,
    EVENT_CODE_FRAME_UNLOCK,
    EVENT_CODE_ENTRY_TO_CORE_PLL1_RESET,
    EVENT_CODE_RELEASE_USER_CORE_PLL1_RESET,

    /* !!! add new event code above this line !!! */
    EVENT_CODE_MAX,
    EVENT_CODE_TIMESTAMP_WRAP_AROUND = 255
};

/** uC Event Error Enum */
enum srds_event_err_code_enum {
    GENERIC_UC_ERROR = 0,
    INVALID_REENTRY_ID,
    DSC_CONFIG_INVALID_REENTRY_ID,
    INVALID_OTP_CONFIG,
    INVALID_CORE_TEMP_IDX
};

/** Core Config Variable Structure in Microcode */
struct bcm_uc_core_config_field_st {
    uint8 vco_rate;
    uint8 core_cfg_from_pcs;
    uint8 reserved;
};

/** Core Config Struct */
struct  bcm_uc_core_config_st {
    struct bcm_uc_core_config_field_st field;
    uint16 word;
    int vco_rate_in_Mhz; /* if >0 then will get converted and replace field.vco_rate when update is called */
};

extern int bcm_do_dscdump(phy_ctrl_t *pc, int sys_port);


#endif /* _DSCDUMP_H_ */
