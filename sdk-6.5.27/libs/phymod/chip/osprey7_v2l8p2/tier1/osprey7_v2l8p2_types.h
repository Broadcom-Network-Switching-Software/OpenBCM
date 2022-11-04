/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2021 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/*****************************************************************************************
 *****************************************************************************************
 *                                                                                       *
 *  Revision      :   *
 *                                                                                       *
 *  Description   :  Common types used by Serdes API functions                           *
 *                                                                                       *
 *****************************************************************************************
 *****************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OSPREY7_V2L8P2_API_TYPES_H
#define OSPREY7_V2L8P2_API_TYPES_H

#include "osprey7_v2l8p2_ipconfig.h"
#include "osprey7_v2l8p2_common.h"
#include "osprey7_v2l8p2_select_defns.h"
#if !defined(SMALL_FOOTPRINT)
#include "osprey7_v2l8p2_lane_detailed_status.h"
#endif

/*! @file
 *  @brief Common types used by Serdes API functions
 */

/*! @addtogroup APITag
 * @{
 */

/*! @defgroup SerdesAPITypesTag Common Core Types
 * Contains structs and typedefs which are used throughout
 * Serdes APIs.
 */

/*! @addtogroup SerdesAPITypesTag
 * @{
 */

/*! Used for error reporting APIs
 *
 */
typedef struct osprey7_v2l8p2_triage_info_st {
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
} osprey7_v2l8p2_triage_info;



#define SIZE_OF_DBGFB_STATS_STRUCT 488  /* Change the size if osprey7_v2l8p2_dbgfb_stats_st size changes */
/*! Used for maintaining correctness of SIZE_OF_DBGFB_STATS_STRUCT
 *
 */
typedef struct {
    uint8_t dbgfb_tmp[sizeof(osprey7_v2l8p2_dbgfb_stats_st)==SIZE_OF_DBGFB_STATS_STRUCT ? 1: -1];
} osprey7_v2l8p2_dbgfb_stats_st_size_check;

/*! Used for debug feedback APIs
 *
 */
typedef struct osprey7_v2l8p2_dbgfb_cfg_s {
    uint8_t y;
    uint8_t x;
    uint32_t time_in_us;
    int32_t data1;
    int32_t data2;
    int32_t data3;
} osprey7_v2l8p2_dbgfb_cfg_st;

#define DBGFB_CFG_ST_INIT {0,0,0,0,0,0}

/*----------------------------------------*/
/*  Lane/Core structs (without bitfields) */
/*----------------------------------------*/

/*! Lane Config Variable Structure in Microcode
 *
 */
struct osprey7_v2l8p2_uc_lane_config_field_st {
  uint8_t  lane_cfg_from_pcs ;
  uint8_t  an_enabled        ;
  uint8_t  dfe_on            ;
  uint8_t  dfe_lp_mode       ;
  uint8_t  force_brdfe_on    ;
  uint8_t  media_type        ;
  uint8_t  unreliable_los    ;
  uint8_t  scrambling_dis    ;
  uint8_t  cl72_auto_polarity_en ;
  uint8_t  cl72_restart_timeout_en;
  uint8_t  force_er          ;
  uint8_t  force_nr          ;
  uint8_t  lp_has_prec_en    ;
  uint8_t  force_pam4_mode   ; /*!< Used to override the pam4mode pin */
  uint8_t  force_nrz_mode    ; /*!< Used to override the pam4mode pin */
};
#define UC_LANE_CONFIG_FIELD_INIT {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

/*! Core Config Variable Structure in Microcode
 *
 */
struct osprey7_v2l8p2_uc_core_config_field_st {
  uint8_t  vco_rate          ;
  uint8_t  core_cfg_from_pcs ;
  uint8_t  osr_5_en          ;
  uint8_t  reserved          ;
};
#define UC_CORE_CONFIG_FIELD_INIT {0,0,0,0}

/*! Lane Config Struct
 *
 */
struct  osprey7_v2l8p2_uc_lane_config_st {
  struct osprey7_v2l8p2_uc_lane_config_field_st field;
  uint16_t word;
};
#define UC_LANE_CONFIG_INIT {UC_LANE_CONFIG_FIELD_INIT, 0}

/*! Core Config Struct
 *
 */
struct  osprey7_v2l8p2_uc_core_config_st {
  struct osprey7_v2l8p2_uc_core_config_field_st field;
  uint16_t word;
  int32_t vco_rate_in_Mhz; /*!< if > 0 then will get converted and replace field.vco_rate when update is called */
};
#define UC_CORE_CONFIG_INIT {UC_CORE_CONFIG_FIELD_INIT, 0, 0}

/*! Lane User Control Function Structure in Microcode
 *
 */
struct osprey7_v2l8p2_usr_ctrl_func_st {
  uint16_t pf_adaptation           ; /*!< Main Peaking Filter Adaptation */
  uint16_t pf2_adaptation          ; /*!< Low Frequency Peaking Filter adaptation */
  uint16_t pf3_adaptation          ; /*!< Low Frequency Peaking Filter adaptation */
  uint16_t dc_adaptation           ; /*!< DC Offset Adaptation */
  uint16_t vga_adaptation          ; /*!< Variable Gain Amplifier adaptation */
  uint16_t slicer_voffset_tuning   ; /*!< Slicer Vertical Offset tuning */
  uint16_t slicer_hoffset_tuning   ; /*!< Slicer Horizontal Offset tuning */
  uint16_t phase_offset_adaptation ; /*!< Phase Offset adaptation */
  uint16_t eye_margin_estimation   ; /*!< Eye margin estimation */
  uint16_t all_adaptation          ; /*!< All adaptation */
  uint16_t afe_bw_adaptation       ; /*!< AFE b/w adaptation */
  uint16_t reserved3               ; /*!< reserved3 */
  uint16_t ffe_adaptation          ; /*!< FFE adaptation */
  uint16_t reserved                ;
  uint16_t ss_pam4es_dfe_data_cdr  ; /*!< Steady state PAM4ES DFE data CDR */
  uint16_t ss_open_loop_debug      ; /*!< Steady State Open Loop debug */
};

/*! Lane User DFE Control Function Structure in Microcode
 *
 */
struct osprey7_v2l8p2_usr_ctrl_dfe_func_st {
  uint8_t dfe_tap1_adaptation      ; /*!< DFE tap 1 adaptation */
  uint8_t dfe_fx_taps_adaptation   ; /*!< DFE Fixed taps adaptation */
  uint8_t dfe_fl_taps_adaptation   ; /*!< DFE Floating taps adaptation */
  uint8_t dfe_dcd_adaptation       ; /*!< DFE tap Duty Cycle Distortion */
};

/*! Lane User Control Disable Function Struct
 *
 */
struct osprey7_v2l8p2_usr_ctrl_disable_functions_st {
  struct osprey7_v2l8p2_usr_ctrl_func_st field;
  uint16_t word;
};

/*! Lane User Control Disable DFE Function Struct
 *
 */
struct osprey7_v2l8p2_usr_ctrl_disable_dfe_functions_st {
  struct osprey7_v2l8p2_usr_ctrl_dfe_func_st field;
  uint8_t  byte;
};

/*! Used in BER APIs
 *
 */
struct osprey7_v2l8p2_ber_data_st {
    uint64_t num_errs;
    uint64_t num_bits;
    uint8_t lcklost;
    uint8_t prbs_chk_en;
    uint8_t cdrlcklost;
    uint8_t prbs_lck_state;
};


/*! Used for checking platform-specific padding
 *
 */
struct osprey7_v2l8p2_check_platform_info_st {
    char c;
    uint32_t d;
};

#define BER_DATA_ST_INIT {0, 0, 0, 0, 0, 0}

/*! @def NUM_ILV
 * The number of interleaves per lane
 */
#define NUM_ILV 4

#define DATA_ARR_SIZE (6)
#define PHASE_ARR_SIZE (3)

#define DFE_ARR_SIZE (38)


/* Change the size if osprey7_v2l8p2_detailed_lane_status_st size changes */

#define SIZE_OF_EXTENDED_LANE_STATE (1166 + 2)



#ifndef SMALL_FOOTPRINT
/*! Used for maintaining correctness of SIZE_OF_EXTENDED_LANE_STATE
 *
 */
typedef struct {
    uint8_t extended_lane_st_tmp[sizeof(struct osprey7_v2l8p2_detailed_lane_status_st)==SIZE_OF_EXTENDED_LANE_STATE ? 1: -1];
} osprey7_v2l8p2_detailed_lane_status_st_size_check;
#endif /* !SMALL_FOOTPRINT */

/****************************************************************************
 * @name Diagnostic Sampling
 *
 ****************************************************************************/
/***/

#ifdef STANDALONE_EVENT
#define DIAG_MAX_SAMPLES (64)
#else
/**
 * Diagnostic sample set size, OSPREY7 variant.
 * Applies to collections of BER measurements, eye margins, etc.
 */
#define DIAG_MAX_SAMPLES (128)
#endif


/*! @} SerdesAPITypesTag */
/*! @} APITag */
#endif
#ifdef __cplusplus
}
#endif

