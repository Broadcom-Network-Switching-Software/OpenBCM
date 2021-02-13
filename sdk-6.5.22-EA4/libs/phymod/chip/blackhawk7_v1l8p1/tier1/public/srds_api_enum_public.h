/********************************************************************************
 *                                                                              *
 * Copyright: (c) 2020 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/******************************************************************************
 ******************************************************************************
 *  Revision      :   *
 *                                                                            *
 *  Description   :  Enum types used by Serdes API functions                  *
 */

#ifndef SRDS_API_ENUM_PUBLIC_H
#define SRDS_API_ENUM_PUBLIC_H

/*! @file
 *  @brief Enum types used by Serdes API functions
 */

/*! @addtogroup APITag
 * @{
 */

/*! @defgroup SrdsAPIEnumTag Common Enum Types */

/*! @addtogroup SrdsAPIEnumTag
 * @{
 */

/** PRBS Polynomial Enum */
enum srds_prbs_polynomial_enum {
    SRDS_PRBS_7  = 0,
    SRDS_PRBS_9  = 1,
    SRDS_PRBS_11 = 2,
    SRDS_PRBS_15 = 3,
    SRDS_PRBS_23 = 4,
    SRDS_PRBS_31 = 5,
    SRDS_PRBS_58 = 6,
    SRDS_PRBS_49 = 7,
    SRDS_PRBS_10 = 8,
    SRDS_PRBS_20 = 9,
    SRDS_PRBS_13 = 10,
    SRDS_USER_40_BIT_REPEAT = 15,
    SRDS_PRBS_AUTO_DETECT = 20,
    SRDS_PCS_PRBS_7 = 100,
    SRDS_PCS_PRBS_15 = 101,
    SRDS_PCS_PRBS_23 = 102,
    SRDS_PCS_PRBS_31 = 103,
    SRDS_PRBS_UNKNOWN = 255
};


/** PRBS Checker Mode Enum */
enum srds_prbs_checker_mode_enum {
    SRDS_PRBS_SELF_SYNC_HYSTERESIS       = 0,
    SRDS_PRBS_INITIAL_SEED_HYSTERESIS    = 1,
    SRDS_PRBS_INITIAL_SEED_NO_HYSTERESIS = 2
};


/** TX_PI Fixed Frequency and Jitter Generation Modes Enum */
enum srds_tx_pi_freq_jit_gen_enum {
    SRDS_TX_PI_FIXED_FREQ      = 0,
    SRDS_TX_PI_SSC_LOW_FREQ    = 1,
    SRDS_TX_PI_SSC_HIGH_FREQ   = 2,
    SRDS_TX_PI_SJ              = 3
};

/** RX AFE Settings Enum */
enum srds_rx_afe_settings_enum {
    SRDS_RX_AFE_PF,
    SRDS_RX_AFE_PF2,
    SRDS_RX_AFE_VGA,
    SRDS_RX_AFE_FGA,
    SRDS_RX_AFE_DFE1,
    SRDS_RX_AFE_DFE2,
    SRDS_RX_AFE_DFE3,
    SRDS_RX_AFE_DFE4,
    SRDS_RX_AFE_DFE5,
    SRDS_RX_AFE_DFE6,
    SRDS_RX_AFE_DFE7,
    SRDS_RX_AFE_DFE8,
    SRDS_RX_AFE_DFE9,
    SRDS_RX_AFE_DFE10,
    SRDS_RX_AFE_DFE11,
    SRDS_RX_AFE_DFE12,
    SRDS_RX_AFE_DFE13,
    SRDS_RX_AFE_DFE14,
    SRDS_RX_AFE_DFE15,
    SRDS_RX_AFE_DFE16,
    SRDS_RX_AFE_DFE17,
    SRDS_RX_AFE_DFE18,
    SRDS_RX_AFE_PF3,
    SRDS_RX_AFE_FFE_PRE,
    SRDS_RX_AFE_FFE_POST,
    SRDS_RX_FFE
};

/** Repeater Mode Enum */
enum srds_rptr_mode_enum {
    SRDS_DATA_IN_SIDE,
    SRDS_DIG_LPBK_SIDE,
    SRDS_RMT_LPBK_SIDE
};

/** Core Powerdown Modes Enum */
enum srds_core_pwrdn_mode_enum {
    SRDS_PWR_ON,
    SRDS_PWRDN,
    SRDS_PWRDN_DEEP,
    SRDS_PWRDN_TX,
    SRDS_PWRDN_RX,
    SRDS_PWROFF_DEEP,
    SRDS_PWRDN_TX_DEEP,
    SRDS_PWRDN_RX_DEEP,
    SRDS_PWR_ON_TX,
    SRDS_PWR_ON_RX
};

/** Eye Scan Direction Enum */
enum srds_eye_scan_dir_enum {
    SRDS_EYE_SCAN_HORIZ = 0,
    SRDS_EYE_SCAN_VERTICAL = 1,
    SRDS_EYE_SCAN_SLICE = 2,
    SRDS_EYE_SCAN_INTR  = 3
};

/* Used by #serdes_display_diag_data to select diag_level             */
/* example:                                                           */
/*  diag_level = SRDS_DIAG_LANE | SRDS_DIAG_CORE | SRDS_DIAG_EVENT    */
enum srds_diag_level_enum {
    SRDS_DIAG_LANE = 1,
    SRDS_DIAG_CORE = 1<<1,
    SRDS_DIAG_EVENT = 1<<2,
    SRDS_DIAG_EYE = 1<<3,
    SRDS_DIAG_REG_CORE = 1<<4,
    SRDS_DIAG_REG_LANE = 1<<5,
    SRDS_DIAG_UC_CORE = 1<<6,
    SRDS_DIAG_UC_LANE = 1<<7,
    SRDS_DIAG_LANE_DEBUG = 1<<8,
    SRDS_DIAG_BER_VERT = 1<<9,
    SRDS_DIAG_BER_HORZ = 1<<10,
    SRDS_DIAG_EVENT_SAFE = 1<<11,
    SRDS_DIAG_TIMESTAMP = 1<<12,
    SRDS_DIAG_PRBS_PROJ = 1<<13,
    SRDS_DIAG_PRBS_PROJ_30S = 1<<14,
    SRDS_DIAG_INFO_TABLE = 1<<15
};

/* Used by #serdes_diag_access to select type of access */
enum srds_diag_access_enum {
    SRDS_REG_READ,
    SRDS_REG_RMW,
    SRDS_CORE_RAM_READ_BYTE,
    SRDS_CORE_RAM_RMW_BYTE,
    SRDS_CORE_RAM_READ_WORD,
    SRDS_CORE_RAM_RMW_WORD,
    SRDS_LANE_RAM_READ_BYTE,
    SRDS_LANE_RAM_RMW_BYTE,
    SRDS_LANE_RAM_READ_WORD,
    SRDS_LANE_RAM_RMW_WORD,
    SRDS_GLOB_RAM_READ_BYTE,
    SRDS_GLOB_RAM_RMW_BYTE,
    SRDS_GLOB_RAM_READ_WORD,
    SRDS_GLOB_RAM_RMW_WORD,
    SRDS_PROG_RAM_READ_BYTE,
    SRDS_PROG_RAM_READ_WORD,
    SRDS_UC_CMD,
    SRDS_EN_BREAKPOINT,
    SRDS_GOTO_BREAKPOINT,
    SRDS_RD_BREAKPOINT,
    SRDS_DIS_BREAKPOINT,
    SRDS_BER_PROJ_DATA
};

/* Used by linktrn_diag to select type of event log */
enum srds_linktrn_diag_log_level_enum {
    SRDS_LINKTRN_DIAG_LOG_INITIAL = 1,
    SRDS_LINKTRN_DIAG_LOG_DETAIL = 1<<1
};

/*! @} SrdsAPIEnumTag */
/*! @} APITag */
#endif
