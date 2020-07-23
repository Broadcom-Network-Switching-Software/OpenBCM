/*************************************************************************************
 *                                                                                   *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/*************************************************************************************
 *************************************************************************************
 *                                                                                   *
 *  Revision      :   *
 *                                                                                   *
 *  Description   :  Defines and Enumerations required by Falcon16 ucode             *
 *                                                                                   *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                             *
 *  No portions of this material may be reproduced in any form without               *
 *  the written permission of:                                                       *
 *      Broadcom Corporation                                                         *
 *      5300 California Avenue                                                       *
 *      Irvine, CA  92617                                                            *
 *                                                                                   *
 *  All information contained in this document is Broadcom Corporation               *
 *  company private proprietary, and trade secret.                                   *
 */

/** @file falcon16_api_uc_common.h
 * Defines and Enumerations shared by Falcon16 IP Specific API and Microcode
 */

#ifndef FALCON16_API_UC_COMMON_H
#define FALCON16_API_UC_COMMON_H

/* Add Falcon16 specific items below this */

/* Please note that when adding entries here you should update the #defines in the falcon16_tsc_common.h */

/** OSR_MODES Enum */
enum falcon16_tsc_osr_mode_enum {
  /* If the enumerations change, then consider updating OSR_MODE_SUPPORTS_EYE_TESTS(). */
  FALCON16_TSC_OSX1    = 0,
  FALCON16_TSC_OSX2    = 1,
  FALCON16_TSC_OSX4    = 2,
  FALCON16_TSC_OSX16P5 = 8,
  FALCON16_TSC_OSX20P625 = 12,
  FALCON16_TSC_OSX2P5  = 16     /* Analog 2p5 OSR Mode for TSC */
};

/** Return true if an OSR mode supports eye tests. **/
#define OSR_MODE_SUPPORTS_EYE_TESTS(mode) ((mode) <= FALCON16_TSC_OSX4)

/** CDR mode Enum **/
enum falcon16_tsc_cdr_mode_enum {
  FALCON16_TSC_CDR_MODE_OS_ALL_EDGES         = 0,
  FALCON16_TSC_CDR_MODE_OS_PATTERN           = 1,
  FALCON16_TSC_CDR_MODE_OS_PATTERN_ENHANCED  = 2,
  FALCON16_TSC_CDR_MODE_BR_PATTERN           = 3
};

/** Lane User Control Clause93/72 Force Value **/
enum falcon16_tsc_cl93n72_frc_val_enum {
  FALCON16_TSC_CL93N72_FORCE_OS  = 0,
  FALCON16_TSC_CL93N72_FORCE_BR  = 1
};

/** FLR Timeout Time Enum
 *  Please change flr_timeout_time_ary in process_pmd_lane.c to match any changes here.
 */
enum falcon16_tsc_flr_timeout_enum {
  FALCON16_TSC_FLR_TIMEOUT_50MS  = 0,
  FALCON16_TSC_FLR_TIMEOUT_75MS  = 1,
  FALCON16_TSC_FLR_TIMEOUT_100MS = 2,
  FALCON16_TSC_FLR_TIMEOUT_150MS = 3
};

/* The following functions translate between a VCO frequency in MHz and the
 * vco_rate that is found in the Core Config Variable Structure using the
 * formula:
 *
 *     vco_rate = (frequency_in_ghz * 16.0) - 232.0
 * 
 * The valid VCO ranges from 14.5GHz to 30GHz.
 *
 * Both functions round to the nearest resulting value.  This
 * provides the highest accuracy possible, and ensures that:
 *
 *     vco_rate == MHZ_TO_VCO_RATE(VCO_RATE_TO_MHZ(vco_rate))
 *
 * In the microcode, these functions should only be called with a numeric
 * literal parameter.
 */
#define MHZ_TO_VCO_RATE(mhz) ((uint8_t)(((((uint16_t)(mhz) * 2) + 62) / 125) - 232))
#define VCO_RATE_TO_MHZ(vco_rate) (((((uint16_t)(vco_rate) + 232) * 125) + 1) >> 1)

/* BOUNDARIES FOR FIR TAP VALUES
 *
 * Sources:  Falcon16 AMS
 *           IEEE Draft P802.3/D2.1, 06 February 2015
 *
 * Hardware limits the sum of the taps to be TXFIR_SUM_LIMIT:
 *
 *     abs(pre) + abs(main) + abs(post1) + abs(post2) + abs(post3)
 *         <= TXFIR_SUM_LIMIT
 */
#define TXFIR_SUM_LIMIT (100)
/*
 * The abs() functions are used because the tap's magnitude flips polarity
 * for 1 versus a 0.  Since pre and main cannot be negative, the abs() is
 * dropped for them.
 *
 * The pre and post1 taps have bitfield limits:
 */
#define TXFIR_PRE_MIN   (  0)
#define TXFIR_PRE_MAX   ( 31)
#define TXFIR_MAIN_MIN  (  0)
#define TXFIR_MAIN_MAX  (127)
#define TXFIR_POST1_MIN (-63)
#define TXFIR_POST1_MAX ( 63)
#define TXFIR_POST2_MIN (-15)
#define TXFIR_POST2_MAX ( 15)
#define TXFIR_POST3_MIN ( -7)
#define TXFIR_POST3_MAX (  7)
/*
 * Note (c) under Table 72-8 of IEEE 802.3 states that V2 must be greater than
 * 40 mV peak-to-zero in all situations.  We test this using the equation:
 *
 *     abs(main) - abs(pre) - abs(post1) - abs(post2) - abs(post3) >=
 *         TXFIR_V2_LIMIT
 *
 * 40 mV peak-to-zero corresponds to 80 mV peak-to-peak.
 */
#define TXFIR_V2_LIMIT (8)

/* Maximum values for rx_vga_ctrl_val */
#define RX_VGA_CTRL_VAL_MAX (37)


/* BEGIN_GENERATED_TEMPERATURE_CODE */
/*
 * The formula for PVTMON is:
 *
 *     T = 434.10000 - 0.53504 * reg10bit
 */
#define _bin_to_degC_double(bin_) (434.10000 - (0.53504 * (USR_DOUBLE)(bin_)))


/* Identify the temperature from the PVTMON output. */
#define _bin_to_degC(bin_) (((((int32_t)(  910373683L) +           \
                               ((int32_t)(bin_) * (   -1122060L))) \
                              >> 20) + 1) >> 1)

/* Identify the PVTMON output corresponding to the temperature. */
#define _degC_to_bin(degc_) (((((int32_t)(  425376459L) +           \
                                ((int32_t)(degc_) * (    -979904L))) \
                               >> 18) + 1) >> 1)
/* END_GENERATED_TEMPERATURE_CODE */

#endif
