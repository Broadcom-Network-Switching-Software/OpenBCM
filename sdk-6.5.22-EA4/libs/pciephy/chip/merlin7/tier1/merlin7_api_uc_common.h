/*************************************************************************************
 *                                                                                   *
 * Copyright: (c) 2019 Broadcom.                                                     *
 * Broadcom Proprietary and Confidential. All rights reserved.                       *
 */

/***************************************************************************************
 ***************************************************************************************
 *                                                                                     *
 *  Revision      :                                      *
 *                                                                                     *
 *  Description   :  Defines and Enumerations required by Merlin7 APIs                *
 *                                                                                     *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.                                                               *
 *  No portions of this material may be reproduced in any form without                 *
 *  the written permission of:                                                         *
 *      Broadcom Corporation                                                           *
 *      5300 California Avenue                                                         *
 *      Irvine, CA  92617                                                              *
 *                                                                                     *
 *  All information contained in this document is Broadcom Corporation                 *
 *  company private proprietary, and trade secret.                                     *
 */

/** @file merlin7_api_uc_common.h
 * Defines and Enumerations shared by Merlin7 IP Specific API and Microcode
 */

#ifndef MERLIN7_API_UC_COMMON_H
#define MERLIN7_API_UC_COMMON_H

/* Add Merlin7 specific items below this */ 

/* Please note that when adding entries here you should update the #defines in the merlin7_pcieg3_common.h */

/** OSR_MODES Enum */
enum merlin7_pcieg3_osr_mode_enum {
  MERLIN7_PCIEG3_OSX1    = 0,
  MERLIN7_PCIEG3_OSX2    = 1,
  MERLIN7_PCIEG3_OSX3    = 2,
  MERLIN7_PCIEG3_OSX3P3  = 3,
  MERLIN7_PCIEG3_OSX4    = 4,
  MERLIN7_PCIEG3_OSX5    = 5,
  MERLIN7_PCIEG3_OSX7P5  = 6,
  MERLIN7_PCIEG3_OSX8    = 7,
  MERLIN7_PCIEG3_OSX8P25 = 8,
  MERLIN7_PCIEG3_OSX10   = 9
};

/* UNIQUIFY_PUBLIC_END  - Marker used by API Uniquify script */

/** LANE_RATE Enum **/
enum merlin7_pcieg3_rate_enum {
  MERLIN7_PCIEG3_GEN1 = 0,
  MERLIN7_PCIEG3_GEN2 = 1,
  MERLIN7_PCIEG3_GEN3 = 2
};

/* The following functions translate between a VCO frequency in MHz and the
 * vco_rate that is found in the Core Config Variable Structure using the
 * formula:
 *
 *     vco_rate = (frequency_in_ghz * 4.0) - 22.0
 *
 * Both functions round to the nearest resulting value.  This
 * provides the highest accuracy possible, and ensures that:
 *
 *     vco_rate == MHZ_TO_VCO_RATE(VCO_RATE_TO_MHZ(vco_rate))
 *
 * In the microcode, these functions should only be called with a numeric
 * literal parameter.
 */
#define MHZ_TO_VCO_RATE(mhz) ((uint8_t)((((uint16_t)(mhz) + 125) / 250) - 22))
#define VCO_RATE_TO_MHZ(vco_rate) (((uint16_t)(vco_rate) + 22) * 250)

/* BOUNDARIES FOR FIR TAP VALUES
 *
 * Sources:  M16_TXFIR document (Section 1.1.2)
 *
 *   The following constraints are enforced by CL72 SW    
 *     pre + main + post1 + post2   <= TXFIR_SUM_LIMIT
 *     main - (pre + post1 + post2) >= TXFIR_V2_LIMIT
 *     pre + post                   <= TXFIR_PRE_POST_SUM_LIMIT
 *     main                         >= TXFIR_MAIN_CL72_MIN
 */
#define TXFIR_PRE_MIN   ( 0)
#define TXFIR_PRE_MAX   (10)
#define TXFIR_MAIN_MIN  ( 0)
#define TXFIR_MAIN_MAX  (60)
#define TXFIR_POST1_MIN ( 0)
#define TXFIR_POST1_MAX (24)
#define TXFIR_POST2_MIN ( 0)
#define TXFIR_POST2_MAX ( 6)

#define TXFIR_SUM_LIMIT (60)
#define TXFIR_V2_LIMIT  ( 6)
#define TXFIR_PRE_POST_SUM_LIMIT (24)
#define TXFIR_MAIN_CL72_MIN      (33)

/** INIT values for FIR taps */
#define TXFIR_PRE_INIT   ( 4)
#define TXFIR_MAIN_INIT  (40)
#define TXFIR_POST1_INIT (16)

/** Minimum and maximum values for rx_vga_ctrl_val */
/*  These are hardware limits for VGA. The tuning limits are defined separately in ucode */
#define RX_VGA_CTRL_VAL_MIN (0)
#define RX_VGA_CTRL_VAL_MAX (31)

/* BEGIN_GENERATED_TEMPERATURE_CODE */
/*
 * The formula for PVTMON is:
 *
 *     T = 434.10000 - 0.54619 * reg10bit
 */
#define _bin_to_degC_double(bin_) (434.10000 - (0.54619 * (USR_DOUBLE)(bin_)))


/* Identify the temperature from the PVTMON output. */
#define _bin_to_degC(bin_) (((((int32_t)(  910373683L) +           \
                               ((int32_t)(bin_) * (   -1145443L))) \
                              >> 20) + 1) >> 1)

/* Identify the PVTMON output corresponding to the temperature. */
#define _degC_to_bin(degc_) (((((int32_t)(  416692764L) +           \
                                ((int32_t)(degc_) * (    -959900L))) \
                               >> 18) + 1) >> 1)
/* END_GENERATED_TEMPERATURE_CODE */

#endif   
