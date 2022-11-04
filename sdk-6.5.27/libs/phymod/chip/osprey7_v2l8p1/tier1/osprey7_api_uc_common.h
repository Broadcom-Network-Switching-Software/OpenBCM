/*************************************************************************************
 *                                                                                   *
 * Copyright: (c) 2021 Broadcom.                                                     *
 * Broadcom Proprietary and Confidential. All rights reserved.                       *
 *                                                                                   *
 *************************************************************************************/


/** @file osprey7_api_uc_common.h
 * Defines and Enumerations shared by Osprey7 IP Specific API and Microcode
 */

#ifndef OSPREY7_API_UC_COMMON_H
#define OSPREY7_API_UC_COMMON_H

/* Add Osprey7 specific items below this */
#define OSPREY7_V2L8P1_LANE_TIMER_SHIFT (5)

/* Please note that when adding entries here you should update the #defines in the osprey7_v2l8p1_common.h */

/** DSC_STATES Enum **/
/** These state values must match the dsc_state bitfield in the register map. **/
/** If these values change, then consider changing the test functions below.  **/
enum srds_dsc_state_enum {
  DSC_STATE_UNKNOWN      = 255,
  DSC_STATE_RESET        = 0,
  DSC_STATE_RESTART      = 1,
  DSC_STATE_CONFIG       = 2,
  DSC_STATE_WAIT_FOR_SIG = 3,
  DSC_STATE_ACQ_CDR      = 4,
  DSC_STATE_CDR_SETTLE   = 5,
  DSC_STATE_HW_TUNE      = 6,
  DSC_STATE_UC_TUNE      = 7,
  DSC_STATE_MEASURE      = 8,
  DSC_STATE_DONE         = 9
};

/** Is the state between DSC_STATE_RESET and DSC_STATE_WAIT_FOR_SIG? **/
#define is_dsc_state_at_or_before_wait_for_sig(state) (state <= DSC_STATE_WAIT_FOR_SIG)


#define EYE_SCAN_NRZ_VERTICAL_IDX_MAX  (124)
#define EYE_SCAN_NRZ_VERTICAL_STEP       (4)
#define EYE_SCAN_PAM_VERTICAL_IDX_MAX  (255)
#define EYE_SCAN_PAM_VERTICAL_STEP       (2)

#define EYE_SCAN_NRZ_HORIZONTAL_WIDTH  (128)
#define EYE_SCAN_NRZ_HORIZONTAL_STEP     (2)
#define EYE_SCAN_PAM_HORIZONTAL_WIDTH   (64)
#define EYE_SCAN_PAM_HORIZONTAL_STEP     (1)

#define BER_PROJ_NRZ_VERTICAL_IDX_MAX  (127)
#define BER_PROJ_NRZ_VERTICAL_STEP       (2)
#define BER_PROJ_NRZ_HORIZONTAL_WIDTH   (64)
#define BER_PROJ_NRZ_HORIZONTAL_STEP     (2)

/** OSR_MODES Enum */
enum osprey7_v2l8p1_osr_mode_enum {
  /* If the enumerations change, then consider updating OSR_MODE_SUPPORTS_EYE_TESTS(). */
  /*  NOTE:  If adding or removing ANY of these you MUST change the initializer below. */
  OSPREY7_V2L8P1_OSX1      = 0,
  OSPREY7_V2L8P1_OSX2      = 1,
  OSPREY7_V2L8P1_OSX4      = 2,
  OSPREY7_V2L8P1_OSX5      = 3,
  OSPREY7_V2L8P1_OSX8      = 9,
  OSPREY7_V2L8P1_OSX16     = 18,
  OSPREY7_V2L8P1_OSX32     = 26,
  OSPREY7_V2L8P1_OSX64     = 34,
  OSPREY7_V2L8P1_OSX33     = 17,
  OSPREY7_V2L8P1_OSX41P25  = 25,
  OSPREY7_V2L8P1_OSX42P5   = 33,
  OSPREY7_V2L8P1_OSR_UNINITIALIZED = 0x3f
};

/* UNIQUIFY_PUBLIC_END  - Marker used by API Uniquify script */

/*This is an initializer to sort the OSR modes in order so that EVAL can walk*/
/*through them proper and choose them in order.*/
#define INIT_OSR_ASSOC_TABLE                                                                    \
        {{OSPREY7_V2L8P1_OSX1, 1000}, {OSPREY7_V2L8P1_OSX2, 2000}, {OSPREY7_V2L8P1_OSX4, 4000},          \
         {OSPREY7_V2L8P1_OSX5, 5000}, {OSPREY7_V2L8P1_OSX8, 8000}, {OSPREY7_V2L8P1_OSX16, 16000},        \
         {OSPREY7_V2L8P1_OSX32, 32000}, {OSPREY7_V2L8P1_OSX33, 33000}, {OSPREY7_V2L8P1_OSX41P25, 41250}, \
         {OSPREY7_V2L8P1_OSX42P5, 42500}, {OSPREY7_V2L8P1_OSX64, 64000}}

#define INIT_OSR_MODE_ENUM {OSPREY7_V2L8P1_OSX1, OSPREY7_V2L8P1_OSX2, OSPREY7_V2L8P1_OSX4, \
                            OSPREY7_V2L8P1_OSX5, OSPREY7_V2L8P1_OSX8, OSPREY7_V2L8P1_OSX16, \
                            OSPREY7_V2L8P1_OSX32, OSPREY7_V2L8P1_OSX33, OSPREY7_V2L8P1_OSX41P25, \
                            OSPREY7_V2L8P1_OSX42P5, OSPREY7_V2L8P1_OSX64, 0xff}

#define INIT_OSR_MODE_INT {1000, 2000, 4000, 5000, 8000, 16000, 32000, 33000, 41250, 42500, 64000}

/** Return true if an OSR mode supports eye tests. **/
#define OSR_MODE_SUPPORTS_EYE_TESTS(mode) ((mode) < OSPREY7_V2L8P1_OSX16)

/** CDR mode Enum **/
enum osprey7_v2l8p1_cdr_mode_enum {
  OSPREY7_V2L8P1_CDR_MODE_OS_ALL_EDGES             = 0,
  OSPREY7_V2L8P1_CDR_MODE_OS_PATTERN               = 1,
  OSPREY7_V2L8P1_CDR_MODE_OS_PATTERN_ENHANCED      = 2,
  OSPREY7_V2L8P1_CDR_MODE_BR_PATTERN               = 3,
  OSPREY7_V2L8P1_CDR_MODE_PAM4_NR_OS_PATTERN       = 4,
  OSPREY7_V2L8P1_CDR_MODE_PAM4_NR_BR_PATTERN       = 5,
  OSPREY7_V2L8P1_CDR_MODE_PAM4_ER_DFE_DATA_PATTERN = 6,
  OSPREY7_V2L8P1_CDR_MODE_PAM4_ER_PATTERN          = 7
};

/** Lane User Control Clause93/72 Force Value **/
enum osprey7_v2l8p1_cl93n72_frc_val_enum {
  OSPREY7_V2L8P1_CL93N72_FORCE_OS  = 0,
  OSPREY7_V2L8P1_CL93N72_FORCE_BR  = 1
};

/** FLR Timeout Time Enum
 *  Please change flr_timeout_time_ary in process_pmd_lane.c to match any changes here.
 */
enum osprey7_v2l8p1_flr_timeout_enum {
  OSPREY7_V2L8P1_FLR_TIMEOUT_50MS  = 0,
  OSPREY7_V2L8P1_FLR_TIMEOUT_75MS  = 1,
  OSPREY7_V2L8P1_FLR_TIMEOUT_100MS = 2,
  OSPREY7_V2L8P1_FLR_TIMEOUT_150MS = 3,
  OSPREY7_V2L8P1_FLR_TIMEOUT_250MS = 4,
  OSPREY7_V2L8P1_FLR_TIMEOUT_500MS = 5
};

/** AFE Override Slicer Selection Value
    Notation indicates functional slicer name **/
typedef enum {
  INVALID_SLICER = 0,
  DATA23_SLICER  = 1,
  DATA05_SLICER  = 3,
  PHASE1_SLICER  = 4,
  PHASE02_SLICER = 5,
  DATA14_SLICER  = 2,
  DFE_TAPS_2_3   = 6,
  LMS_SLICER     = 8
} afe_override_slicer_sel_t;

/* The following functions translate between a VCO frequency in MHz and the
 * vco_rate that is found in the Core Config Variable Structure using the
 * formula:
 *
 *     vco_rate = (frequency_in_ghz * 8.0) - 232.0
 *
 * The valid VCO ranges from 41GHz to 57GHz
 *
 * Both functions round to the nearest resulting value.  This
 * provides the highest accuracy possible, and ensures that:
 *
 *     vco_rate == MHZ_TO_VCO_RATE(VCO_RATE_TO_MHZ(vco_rate))
 *
 * In the microcode, these functions should only be called with a numeric
 * literal parameter.
 */
#define MHZ_TO_VCO_RATE(mhz) ((uint8_t)((((uint16_t)(mhz) + 62) / 125) - 232))
#define VCO_RATE_TO_MHZ(vco_rate) (((uint16_t)(vco_rate) + 232) * 125)

/* BOUNDARIES FOR FIR TAP VALUES
 *
 * Hardware limits the sum of the taps to be TXFIR_SUM_LIMIT or TXFIR_PAM4_SUM_LIMIT:
 *
 *     sum(n=0..11, abs(tap[n])) <= TXFIR_NRZ_SUM_LIMIT, if in NRZ mode
 *     sum(n=0..11, abs(tap[n])) <= TXFIR_PAM4_UC_SUM_LIMIT, if in PAM4 mode
 */
#define TXFIR_NRZ_SUM_LIMIT     (127)
#define TXFIR_PAM4_UC_SUM_LIMIT (168)

/*
 * All taps have bitfield limits:
 */
#define TXFIR_NRZ_TAP_MIN     (-127)
#define TXFIR_NRZ_TAP_MAX     ( 127)
#define TXFIR_PAM4_UC_TAP_MIN (-168)
#define TXFIR_PAM4_UC_TAP_MAX ( 168)

/*
 * Peaking Filter Boundaries and Lookup Tables
 */
#define PF_MIN_VALUE          ( 0)
#define PF_MAX_VALUE          (15)
#define PF2_MIN_VALUE         ( 0)
#define PF2_MAX_VALUE         ( 7)
#define PF3_MIN_VALUE         ( 0)
#define PF3_MAX_VALUE         (15)

#define SSA_MODE              (3)

/* defined(__arm__) || defined(EMULATION_EN) */

/*
 * Note (c) under Table 72-8 of IEEE 802.3 states that V2 must be greater than
 * 40 mV peak-to-zero in all situations.  We test this using the equation:
 *
 *     abs(main) - abs(pre) - abs(post1) >=
 *         TXFIR_V2_LIMIT
 *
 * 40 mV peak-to-zero corresponds to 80 mV peak-to-peak.
 */
#define TXFIR_NRZ_V2_LIMIT  (12)
#define TXFIR_PAM4_V2_LIMIT (16)

/* Maximum values for rx_vga_ctrl_val */
#define RX_VGA_CTRL_VAL_MAX (78)

/**************************************************************************
 *               PVTMON code generated by script                          *
 **************************************************************************/

/* BEGIN_GENERATED_TEMPERATURE_CODE */

/*
 * The formula for PVTMON is:
 *
 *     T = 356.07000 - 0.23755 * reg_bin
 */
#define _bin_to_degC_double(bin_) (356.07000 - (0.23755 * (USR_DOUBLE)(bin_)))


/* Identify the temperature from the PVTMON output. */
#define _bin_to_degC(bin_) (((((int32_t)(  746732913L) +           \
                               ((int32_t)(bin_) * (    -498178L))) \
                              >> 20) + 1) >> 1)

/* Identify the PVTMON output corresponding to the temperature. */
#define _degC_to_bin(degc_) (((((int32_t)(  196467300L) +           \
                                ((int32_t)(degc_) * (    -551766L))) \
                               >> 16) + 1) >> 1)

/* Scale temperature change in C to equivalent change in PVTMON output. */
#define _scale_degC_to_bin(degc_) (((((int32_t)(degc_) * (    -551766L)) \
                               >> 16) + 1) >> 1)


/* END_GENERATED_TEMPERATURE_CODE */

#endif
