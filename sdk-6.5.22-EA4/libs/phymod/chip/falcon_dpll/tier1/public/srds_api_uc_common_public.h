/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision      :  $Id: srds_api_uc_common_public.h 1560 2017-07-17 23:46:38Z kirand $  *
 *                                                                                *
 *  Description   :  Defines and Enumerations required by Serdes ucode            *
 *                                                                                *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.                                                          *
 *  No portions of this material may be reproduced in any form without            *
 *  the written permission of:                                                    *
 *      Broadcom Corporation                                                      *
 *      5300 California Avenue                                                    *
 *      Irvine, CA  92617                                                         *
 *                                                                                *
 *  All information contained in this document is Broadcom Corporation            *
 *  company private proprietary, and trade secret.                                *
 */

/** @file srds_api_uc_common.h
 * Defines and Enumerations shared by Serdes API and Microcode
 */

#ifndef SRDS_API_UC_COMMON_PUBLIC_H
#define SRDS_API_UC_COMMON_PUBLIC_H

/*-----------------------------*/
/*  Generic Serdes items first */
/*-----------------------------*/

/** DSC_STATES Enum */
enum srds_dsc_state_enum {
  SRDS_DSC_STATE_UNKNOWN        = 255,
  SRDS_DSC_STATE_RESET          = 0,
  SRDS_DSC_STATE_RESTART        = 1,
  SRDS_DSC_STATE_CONFIG         = 2,
  SRDS_DSC_STATE_WAIT_FOR_SIG   = 3,
  SRDS_DSC_STATE_ACQ_CDR        = 4,
  SRDS_DSC_STATE_CDR_SETTLE     = 5,
  SRDS_DSC_STATE_HW_TUNE        = 6,
  SRDS_DSC_STATE_UC_TUNE        = 7,
  SRDS_DSC_STATE_MEASURE        = 8,
  SRDS_DSC_STATE_DONE           = 9
};

/** SERDES_PMD_UC_COMMANDS Enum */
enum srds_pmd_uc_cmd_enum {
  SRDS_CMD_NULL                 =  0,
  SRDS_CMD_UC_CTRL              =  1,
  SRDS_CMD_HEYE_OFFSET          =  2,
  SRDS_CMD_VEYE_OFFSET          =  3,
  SRDS_CMD_UC_DBG               =  4,
  SRDS_CMD_DIAG_EN              =  5,
  SRDS_CMD_READ_UC_LANE_BYTE    =  6,
  SRDS_CMD_WRITE_UC_LANE_BYTE   =  7,
  SRDS_CMD_READ_UC_CORE_BYTE    =  8,
  SRDS_CMD_WRITE_UC_CORE_BYTE   =  9,
  SRDS_CMD_READ_UC_LANE_WORD    = 10,
  SRDS_CMD_WRITE_UC_LANE_WORD   = 11,
  SRDS_CMD_READ_UC_CORE_WORD    = 12,
  SRDS_CMD_WRITE_UC_CORE_WORD   = 13,
  SRDS_CMD_EVENT_LOG_CTRL       = 14,
  SRDS_CMD_EVENT_LOG_READ       = 15,
  SRDS_CMD_CAPTURE_BER_START    = 16,
  SRDS_CMD_READ_DIAG_DATA_BYTE  = 17,
  SRDS_CMD_READ_DIAG_DATA_WORD  = 18,
  SRDS_CMD_CAPTURE_BER_END      = 19,
  SRDS_CMD_CALC_CRC             = 20,
  SRDS_CMD_FREEZE_STEADY_STATE  = 21,
  SRDS_CMD_TDT_EN               = 22
};

/** SERDES_UC_CTRL_COMMANDS Enum */
enum srds_pmd_uc_ctrl_cmd_enum {
  SRDS_CMD_UC_CTRL_STOP_GRACEFULLY = 0,
  SRDS_CMD_UC_CTRL_STOP_IMMEDIATE  = 1,
  SRDS_CMD_UC_CTRL_RESUME          = 2
};

/** SERDES_UC_DIAG_COMMANDS Enum */
enum srds_pmd_uc_diag_cmd_enum {
  SRDS_CMD_UC_DIAG_DISABLE         = 3,
  SRDS_CMD_UC_DIAG_PASSIVE         = 1,
  SRDS_CMD_UC_DIAG_DENSITY         = 2,
  SRDS_CMD_UC_DIAG_START_VSCAN_EYE = 4,
  SRDS_CMD_UC_DIAG_START_HSCAN_EYE = 5,
  SRDS_CMD_UC_DIAG_GET_EYE_SAMPLE  = 6
};

/** SERDES_EVENT_LOG_READ Enum */
enum srds_pmd_event_rd_cmd_enum {
  SRDS_CMD_EVENT_LOG_READ_START = 0,
  SRDS_CMD_EVENT_LOG_READ_NEXT  = 1,
  SRDS_CMD_EVENT_LOG_READ_DONE  = 2
};

/** SERDES_UC_DBG_COMMANDS Enum */
enum srds_pmd_uc_dbg_cmd_enum {
  SRDS_CMD_UC_DBG_DIE_TEMP      = 0,
  SRDS_CMD_UC_DBG_TIMESTAMP     = 1,
  SRDS_CMD_UC_DBG_LANE_IDX      = 2,
  SRDS_CMD_UC_DBG_LANE_TIMER    = 3
};

/** Media type Enum */
enum srds_media_type_enum {
  SRDS_MEDIA_TYPE_PCB_TRACE_BACKPLANE = 0,
  SRDS_MEDIA_TYPE_COPPER_CABLE        = 1,
  SRDS_MEDIA_TYPE_OPTICS              = 2,
  SRDS_MEDIA_TYPE_OPTICS_ZR           = 3
};

/** DIAG_BER mode settings **/
enum srds_diag_ber_mode_enum {
    SRDS_DIAG_BER_POS  = 0,
    SRDS_DIAG_BER_NEG  = 1,
    SRDS_DIAG_BER_VERT = 0,
    SRDS_DIAG_BER_HORZ = 1<<1,
    SRDS_DIAG_BER_PASS = 0,
    SRDS_DIAG_BER_INTR = 1<<2,
    SRDS_DIAG_BER_P1_NARROW = 1<<3,
    SRDS_DIAG_BER_P1_WIDE = 0,
    SRDS_DIAG_BER_FAST = 1<<6

};

/** SERDES_TDT_PATTERN Enum */
enum srds_tdt_pattern_enum {
  SRDS_TDT_PAT_40         = 0,
  SRDS_TDT_PAT_80         = 1,
  SRDS_TDT_PAT_120        = 2,
  SRDS_TDT_PAT_160        = 3,
  SRDS_TDT_PAT_200        = 4,
  SRDS_TDT_PAT_240        = 5,
  SRDS_TDT_PAT_PRBS7      = 6,
  SRDS_TDT_PAT_PRBS9      = 7
};

/** SERDES_TDT_REQUEST Enum */
enum srds_tdt_req_enum {
  SRDS_TDT_NULL           = 0,
  SRDS_TDT_START          = 1,
  SRDS_TDT_STEP           = 3,
  SRDS_TDT_READ           = 4,
  SRDS_TDT_DISABLE        = 5
};

typedef uint8_t float8_t;

/*------------------------------*/
/** Serdes Event Code Structure */
/*------------------------------*/

/** Core User Control Event Log Level **/
#define USR_CTRL_CORE_EVENT_LOG_UPTO_FULL (0xFF)

#define event_code_t uint8_t
#define log_level_t  uint8_t

/** Event Log Code Enum */
enum srds_event_code_enum {
    SRDS_EVENT_CODE_UNKNOWN = 0,
    SRDS_EVENT_CODE_ENTRY_TO_DSC_RESET,
    SRDS_EVENT_CODE_RELEASE_USER_RESET,
    SRDS_EVENT_CODE_EXIT_FROM_DSC_RESET,
    SRDS_EVENT_CODE_ENTRY_TO_CORE_RESET,
    SRDS_EVENT_CODE_RELEASE_USER_CORE_RESET,
    SRDS_EVENT_CODE_ACTIVE_RESTART_CONDITION,
    SRDS_EVENT_CODE_EXIT_FROM_RESTART,
    SRDS_EVENT_CODE_WRITE_TR_COARSE_LOCK,
    SRDS_EVENT_CODE_CL72_READY_FOR_COMMAND,
    SRDS_EVENT_CODE_EACH_WRITE_TO_CL72_TX_CHANGE_REQUEST,
    SRDS_EVENT_CODE_FRAME_LOCK,
    SRDS_EVENT_CODE_LOCAL_RX_TRAINED,
    SRDS_EVENT_CODE_DSC_LOCK,
    SRDS_EVENT_CODE_FIRST_RX_PMD_LOCK,
    SRDS_EVENT_CODE_PMD_RESTART_FROM_CL72_CMD_INTF_TIMEOUT,
    SRDS_EVENT_CODE_LP_RX_READY,
    SRDS_EVENT_CODE_STOP_EVENT_LOG,
    SRDS_EVENT_CODE_GENERAL_EVENT_0,
    SRDS_EVENT_CODE_GENERAL_EVENT_1,
    SRDS_EVENT_CODE_GENERAL_EVENT_2,
    SRDS_EVENT_CODE_ERROR_EVENT,
    SRDS_EVENT_CODE_NUM_TIMESTAMP_WRAPAROUND_MAXOUT,
    SRDS_EVENT_CODE_RESTART_PMD_ON_CDR_LOCK_LOST,
    SRDS_EVENT_CODE_SM_STATUS_RESTART,
    SRDS_EVENT_CODE_CORE_PROGRAMMING,
    SRDS_EVENT_CODE_LANE_PROGRAMMING,
    SRDS_EVENT_CODE_RESTART_PMD_ON_CLOSE_EYE,
    SRDS_EVENT_CODE_RESTART_PMD_ON_DFE_TAP_CONFIG,
    SRDS_EVENT_CODE_CL72_AUTO_POLARITY_CHANGE,
    SRDS_EVENT_CODE_RESTART_FROM_CL72_MAX_TIMEOUT,
    SRDS_EVENT_CODE_CL72_LOCAL_TX_CHANGED,
    SRDS_EVENT_CODE_FIRST_WRITE_TO_CL72_TX_CHANGE_REQUEST,
    SRDS_EVENT_CODE_FRAME_UNLOCK,
    SRDS_EVENT_CODE_ENTRY_TO_CORE_PLL1_RESET,
    SRDS_EVENT_CODE_RELEASE_USER_CORE_PLL1_RESET,
    SRDS_EVENT_CODE_ACTIVE_WAIT_FOR_SIG,
    SRDS_EVENT_CODE_EXIT_FROM_WAIT_FOR_SIG,
    SRDS_EVENT_CODE_START_VGA_TUNING,
    SRDS_EVENT_CODE_START_FX_DFE_TUNING,
    SRDS_EVENT_CODE_START_PF_TUNING,
    SRDS_EVENT_CODE_START_EYE_MEAS_TUNING,
    SRDS_EVENT_CODE_START_MAIN_TAP_TUNING,
    SRDS_EVENT_CODE_START_FL_TAP_TUNING,
    SRDS_EVENT_CODE_EXIT_FROM_DSC_INIT,
    SRDS_EVENT_CODE_ENTRY_TO_DSC_INIT,
    SRDS_EVENT_CODE_DSC_PAUSE,
    SRDS_EVENT_CODE_DSC_UC_TUNE,
    SRDS_EVENT_CODE_DSC_DONE,
    SRDS_EVENT_CODE_RESTART_PMD_ON_SHORT_CHANNEL_DETECTED,
    SRDS_EVENT_CODE_CL72_FCTTS_TF_TRN,
    SRDS_EVENT_CODE_FLR_ACTIVATED,
    SRDS_EVENT_CODE_FLR_SIGDET_DEASSERT,
    SRDS_EVENT_CODE_FLR_SIGDET_ASSERT,
    SRDS_EVENT_CODE_FLR_DEACTIVATED_DUE_TO_TIMEOUT,

    /* !!! add new event code above this line !!! */
    SRDS_EVENT_CODE_MAX,
    SRDS_EVENT_CODE_TIMESTAMP_WRAP_AROUND = 255
};

/** uC Event Error Enum */
enum srds_event_err_code_enum {
    SRDS_GENERIC_UC_ERROR = 0,
    SRDS_INVALID_REENTRY_ID,
    SRDS_DSC_CONFIG_INVALID_REENTRY_ID,
    SRDS_INVALID_OTP_CONFIG_FAIL_PLL,
    SRDS_INVALID_CORE_TEMP_IDX,
    SRDS_INVALID_OTP_CONFIG_FAIL_DFE,
    SRDS_INVALID_VCO_RATE_REQUEST
};

#endif
