/*****************************************************************************************
 *****************************************************************************************
 *                                                                                       *
 *  Revision      :   *
 *                                                                                       *
 *  Description   :  Common types used by Serdes API functions                           *
 *                                                                                       *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                                 *
 *  No portions of this material may be reproduced in any form without                   *
 *  the written permission of:                                                           *
 *      Broadcom Corporation                                                             *
 *      5300 California Avenue                                                           *
 *      Irvine, CA  92617                                                                *
 *                                                                                       *
 *  All information contained in this document is Broadcom Corporation                   *
 *  company private proprietary, and trade secret.                                       *
 */

/** @file merlin16_types.h
 * Common types used by Serdes API functions
 */

#ifndef MERLIN16_API_TYPES_H
#define MERLIN16_API_TYPES_H

#include <phymod/phymod_types.h>
#include "merlin16_ipconfig.h"
#include "merlin16_usr_includes.h"


/*----------------------------------------*/
/*  Lane/Core structs (without bitfields) */
/*----------------------------------------*/

/** Lane Config Variable Structure in Microcode */
struct merlin16_uc_lane_config_field_st {
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

/** Core Config Variable Structure in Microcode */
struct merlin16_uc_core_config_field_st {
  uint8_t  core_cfg_from_pcs ;
  uint8_t  vco_rate          ;
  uint8_t  an_los_workaround ;
  uint8_t  reserved1         ;
  uint8_t  reserved2         ;
};

/** Lane Config Struct */
struct  merlin16_uc_lane_config_st {
  struct merlin16_uc_lane_config_field_st field;
  uint16_t word;
};

/** Core Config Struct */
struct  merlin16_uc_core_config_st {
  struct merlin16_uc_core_config_field_st field;
  uint16_t word;
  int vco_rate_in_Mhz; /* if >0 then will get converted and replace field.vco_rate when update is called */
};

/** Lane User Control Function Structure in Microcode */
struct merlin16_usr_ctrl_func_st {
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
struct merlin16_usr_ctrl_dfe_func_st {
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
struct merlin16_usr_ctrl_disable_functions_st {
  struct merlin16_usr_ctrl_func_st field;
  uint8_t  byte;
};

/** Lane User Control Disable DFE Function Struct */
struct merlin16_usr_ctrl_disable_dfe_functions_st {
  struct merlin16_usr_ctrl_dfe_func_st field;
  uint8_t  byte;
};
struct ber_data_st {
    uint64_t num_errs;
    uint64_t num_bits;
    uint8_t lcklost;
    uint8_t prbs_chk_en;
};


/****************************************************************************
 * @name Diagnostic Sampling
 */
/**@{*/

#ifdef STANDALONE_EVENT
#define DIAG_MAX_SAMPLES (64)
#else
/**
 * Diagnostic sample set size, non-FALCON16 variant.
 * Applies to collections of BER measurements, eye margins, etc.
 */
#define DIAG_MAX_SAMPLES (32)
#endif

/**@}*/

#endif
