/*----------------------------------------------------------------------
 * $Id: temod.h,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators
 *----------------------------------------------------------------------*/
/*
 * 
 * $Copyright:
 * All Rights Reserved.$
 */
#ifndef _TEMOD2PLL_H_
#define _TEMOD2PLL_H_

#ifndef _DV_TB_
#define _SDK_TEMOD2PLL_ 1
#endif

#ifdef _SDK_TEMOD2PLL_
#include <phymod/phymod.h>
#endif

#ifndef PHYMOD_ST
#ifdef _SDK_TEMOD2PLL_
  #define PHYMOD_ST  const phymod_access_t
#else
  #define PHYMOD_ST  temod_st
#endif /* _SDK_TEMOD2PLL_ */
#endif /* PHYMOD_ST */

#ifdef _DV_TB_
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"
#endif

/* So far 4 bits debug mask are used by TMOD */
/* Moved to temod_device.h chip specific debug.
#define TEMOD2PLL_DBG_REGACC     (1L << 3) * Print all register accesses *
#define TEMOD2PLL_DBG_FUNCVALOUT (1L << 2) * All values returned by Tier1*
#define TEMOD2PLL_DBG_FUNCVALIN  (1L << 1) * All values pumped into Tier1*
#define TEMOD2PLL_DBG_FUNC       (1L << 0) * Every time we enter a  Tier1*
*/

#define TEMOD2PLL_OVERRIDE_CFG 0x80000000
#define TEMOD2PLL_CL73_ABILITY_2P5G_POSITION (11)
#define TEMOD2PLL_CL73_ABILITY_5G_POSITION (12)
typedef enum {
    TEMOD2PLL_AN_MODE_CL73 = 0,
    TEMOD2PLL_AN_MODE_CL37,
    TEMOD2PLL_AN_MODE_CL73BAM,
    TEMOD2PLL_AN_MODE_CL37BAM,
    TEMOD2PLL_AN_MODE_SGMII  ,
    TEMOD2PLL_AN_MODE_HPAM ,          /* this macro needed to be identical to temod_an_type */
    TEMOD2PLL_AN_MODE_CL37_SGMII,
    TEMOD2PLL_AN_MODE_NONE,
    TEMOD2PLL_AN_MODE_TYPE_COUNT
}temod2pll_an_mode_type_t;

typedef enum {
    TEMOD2PLL_TX_LANE_RESET = 0,
    TEMOD2PLL_TX_LANE_TRAFFIC_ENABLE = 1,
    TEMOD2PLL_TX_LANE_TRAFFIC_DISABLE = 2,
    TEMOD2PLL_TX_LANE_RESET_TRAFFIC_ENABLE = 3,
    TEMOD2PLL_TX_LANE_RESET_TRAFFIC_DISABLE = 4,
    TEMOD2PLL_TX_LANE_TYPE_COUNT
}tx_lane_disable_type_t;

typedef struct temod2pll_an_control_s {
  temod2pll_an_mode_type_t an_type;
  uint16_t num_lane_adv;
  uint16_t enable;
  uint16_t pd_kx_en;
  uint16_t pd_kx4_en;
  uint16_t pd_2p5g_en;
  an_property_enable  an_property_type;
} temod2pll_an_control_t;

typedef enum {
    TEMOD2PLL_CL73_100GBASE_CR10 = 0,
    TEMOD2PLL_CL73_40GBASE_CR4,
    TEMOD2PLL_CL73_40GBASE_KR4,
    TEMOD2PLL_CL73_10GBASE_KR,
    TEMOD2PLL_CL73_10GBASE_KX4,
    TEMOD2PLL_CL73_1000BASE_KX,
    TEMOD2PLL_CL73_2p5GBASE_X,
    TEMOD2PLL_CL73_5GBASE_KR1,
    TEMOD2PLL_CL73_SPEED_COUNT
}temod2pll_cl73_speed_t;

typedef enum {
    TEMOD2PLL_CL73_BAM_20GBASE_KR2 = 0,
    TEMOD2PLL_CL73_BAM_20GBASE_CR2,
    TEMOD2PLL_CL73_BAM_SPEED_COUNT
}temod2pll_cl73_bam_speed_t;

typedef enum {
    TEMOD2PLL_CL37_SGMII_10M = 0,
    TEMOD2PLL_CL37_SGMII_100M,
    TEMOD2PLL_CL37_SGMII_1000M,
    TEMOD2PLL_CL37_SGMII_SPEED_COUNT
}temod2pll_cl37_sgmii_speed_t;


typedef enum {
    TEMOD2PLL_CL37_BAM_13GBASE_X4 = 0,
    TEMOD2PLL_CL37_BAM_15GBASE_X4,
    TEMOD2PLL_CL37_BAM_15p75GBASE_X2,
    TEMOD2PLL_CL37_BAM_16GBASE_X4,
    TEMOD2PLL_CL37_BAM_20GBASE_X4_CX4,
    TEMOD2PLL_CL37_BAM_20GBASE_X4,
    TEMOD2PLL_CL37_BAM_20GBASE_X2,
    TEMOD2PLL_CL37_BAM_20GBASE_X2_CX4,
    TEMOD2PLL_CL37_BAM_21GBASE_X4,
    TEMOD2PLL_CL37_BAM_25p455GBASE_X4,
    TEMOD2PLL_CL37_BAM_31p5GBASE_X4,
    TEMOD2PLL_CL37_BAM_32p7GBASE_X4,
    TEMOD2PLL_CL37_BAM_40GBASE_X4,
    TEMOD2PLL_CL37_BAM_SPEED1_COUNT
}temod2pll_cl37_bam_speed1_t;

typedef enum {
    TEMOD2PLL_CL37_BAM_2p5GBASE_X = 0,
    TEMOD2PLL_CL37_BAM_5GBASE_X4,
    TEMOD2PLL_CL37_BAM_6GBASE_X4,
    TEMOD2PLL_CL37_BAM_10GBASE_X4,
    TEMOD2PLL_CL37_BAM_10GBASE_X4_CX4,
    TEMOD2PLL_CL37_BAM_10GBASE_X2,
    TEMOD2PLL_CL37_BAM_10GBASE_X2_CX4,
    TEMOD2PLL_CL37_BAM_BAM_10p5GBASE_X2,
    TEMOD2PLL_CL37_BAM_12GBASE_X4,
    TEMOD2PLL_CL37_BAM_12p5GBASE_X4,
    TEMOD2PLL_CL37_BAM_12p7GBASE_X2,
    TEMOD2PLL_CL37_BAM_SPEED_COUNT
}temod2pll_cl37_bam_speed_t;


typedef enum {
    TEMOD2PLL_NO_PAUSE = 0,
    TEMOD2PLL_SYMM_PAUSE,
    TEMOD2PLL_ASYM_PAUSE,
    TEMOD2PLL_ASYM_SYMM_PAUSE,
    TEMOD2PLL_AN_PAUSE_COUNT
}temod2pll_an_pause_t;

typedef enum {
    TEMOD2PLL_HG2_CODEC_OFF = 0,
    TEMOD2PLL_HG2_CODEC_ON_8BYTE_IPG,
    TEMOD2PLL_HG2_CODEC_ON_9BYTE_IPG,
    TEMOD2PLL_HG2_CODEC_COUNT
}temod2pll_hg2_codec_t;

#define TEMOD2PLL_FEC_NOT_SUPRTD         0
#define TEMOD2PLL_FEC_SUPRTD_NOT_REQSTD  1
#define TEMOD2PLL_FEC_CL74_SUPRTD_REQSTD 4

/**
\struct temod2pll_an_init_s

This embodies all parameters required to program autonegotiation features in the
PHY for both CL37 and CL73 type autonegotiation and the BAM variants.
For details of individual fields, please refer to microarchitectural doc.
*/
typedef struct temod2pll_an_init_s{
  uint16_t  an_fail_cnt;
  uint16_t  an_oui_ctrl;
  uint16_t  linkfailtimer_dis;
  uint16_t  linkfailtimerqua_en;
  uint16_t  an_good_check_trap;
  uint16_t  an_good_trap;
  uint16_t  disable_rf_report;
  uint16_t  cl37_bam_ovr1g_pgcnt;
  uint16_t  cl73_remote_fault;
  uint16_t  cl73_nonce_match_over;
  uint16_t  cl73_nonce_match_val;
  uint16_t  cl73_transmit_nonce;
  uint16_t  base_selector;
} temod2pll_an_init_t;


/**
\struct temod2pll_an_timers_s
TBD
*/
typedef struct temod2pll_an_timers_s{
  uint16_t  value;
} temod2pll_an_timers_t;



typedef struct temod2pll_an_adv_ability_s{
  temod2pll_cl73_speed_t an_base_speed;
  temod2pll_cl37_bam_speed_t an_bam_speed;
  temod2pll_cl37_bam_speed1_t an_bam_speed1;
  temod2pll_an_pause_t an_pause;
  uint16_t an_fec;
  uint16_t an_cl72;
  uint16_t an_hg2;
  temod2pll_cl37_sgmii_speed_t cl37_sgmii_speed;
}temod2pll_an_adv_ability_t;

/**
\struct temod2pll_an_ability_s

TBD

*/
typedef struct temod2pll_an_ability_s {
  temod2pll_an_adv_ability_t cl37_adv; /*includes cl37 and cl37-bam related (fec, cl72) */
  temod2pll_an_adv_ability_t cl73_adv; /*includes cl73 and cl73-bam related */
} temod2pll_an_ability_t;

/**
\struct temod2pll_an_oui_s
  oui                : New oui
  override_bam73_adv : Adv new OUI for BAM73
  override_bam73_det : Detect new OUI for BAM73
  override_hpam_adv  : Adv new OUI for HPAM
  override_hpam_det  : Detect new OUI for HPAM
  override_bam37_adv : Adv new OUI for BAM37
  override_bam37_det : Detect new OUI for BAM37
 */
typedef struct temod2pll_an_oui_s {
  uint32_t oui;
  uint16_t oui_override_bam73_adv;
  uint16_t oui_override_bam73_det;
  uint16_t oui_override_hpam_adv;
  uint16_t oui_override_hpam_det;
  uint16_t oui_override_bam37_adv;
  uint16_t oui_override_bam37_det;
} temod2pll_an_oui_t;

typedef struct temod2pll_ts_tx_info_s {
    uint16_t ts_val_lo;
    uint16_t ts_val_hi;
    uint16_t ts_seq_id;
    uint16_t ts_sub_nanosec;
} temod2pll_ts_tx_info_t;

/* Bit error mask to generate FEC error. */
typedef struct temod2pll_fec_error_mask_s {
    uint32_t error_mask_bit_31_0;     /* Error mask bit 31-0. */
    uint32_t error_mask_bit_63_32;    /* Error mask bit 63-32. */
    uint16_t error_mask_bit_79_64;    /* Error mask bit 79-64. */
} temod2pll_fec_error_mask_t;

extern int temod2pll_pll_reset_enable_set (PHYMOD_ST *pa , int enable);
extern int temod2pll_speed_id_get (PHYMOD_ST *pc,  int *speed_id);
extern int temod2pll_update_port_mode( PHYMOD_ST *pa);
extern int temod2pll_get_plldiv (PHYMOD_ST *pa,  uint32_t *pll_div);
extern int temod2pll_tx_rx_polarity_get (PHYMOD_ST *pc, uint32_t* txp, uint32_t* rxp);
extern int temod2pll_rx_lane_control_set(PHYMOD_ST* pc, int enable);
extern int temod2pll_rx_lane_control_get(PHYMOD_ST* pc, int *enable);
extern int temod2pll_tx_rx_polarity_set (PHYMOD_ST *pc, uint32_t txp, uint32_t rxp);
extern int temod2pll_pmd_lane_swap_tx_get (PHYMOD_ST *pc, uint32_t *tx_lane_map);
extern int temod2pll_pmd_addr_lane_swap (PHYMOD_ST *pc, uint32_t addr_lane_index);
extern int temod2pll_disable_get (PHYMOD_ST *pc, uint32_t* enable);
extern int temod2pll_pll_sequencer_control(PHYMOD_ST *pa, int enable);
extern int temod2pll_pcs_rx_lane_swap_get ( PHYMOD_ST *pc,  uint32_t *rx_swap);
extern int temod2pll_pcs_tx_lane_swap_get ( PHYMOD_ST *pc,  uint32_t *tx_swap);
extern int temod2pll_master_port_num_set(PHYMOD_ST *pa,  int port_num);
extern int temod2pll_set_spd_intf(PHYMOD_ST *pc, temod2pll_spd_intfc_type spd_intf, int no_trig);
extern int temod2pll_pll_lock_wait(PHYMOD_ST* pc, int timeOutValue);
extern int temod2pll_credit_control(PHYMOD_ST* pc, int enable);
#ifdef _DV_TB_
extern int temod2pll_tx_lane_control(PHYMOD_ST* pc, int enable, tx_lane_disable_type_t tx_dis_type);
#endif
#ifdef _SDK_TEMOD2PLL_
extern int temod2pll_tx_lane_control_set(PHYMOD_ST* pc, tx_lane_disable_type_t tx_dis_type);
extern int temod2pll_tx_lane_control_get(PHYMOD_ST* pc, int *reset, int *enable);
#endif
extern int temod2pll_power_control(PHYMOD_ST* pc);
extern int temod2pll_override_set(PHYMOD_ST* pc, override_type_t or_type, uint16_t or_val);
extern int temod2pll_override_get(PHYMOD_ST* pc, override_type_t or_type, uint16_t *or_en,  uint16_t *or_val);
extern int temod2pll_credit_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit);
extern int temod2pll_rx_lane_control_set(PHYMOD_ST* pc, int enable);
extern int temod2pll_rx_lane_control_get(PHYMOD_ST* pc, int *enable);
#ifdef _DV_TB_
extern int temod2pll_mld_am_timers_set(PHYMOD_ST* pc, int rx_am_timer_init,int tx_am_timer_init_val);
#endif
extern int temod2pll_init_pmd(PHYMOD_ST* pc, int pmd_touched, int uc_active);
extern int temod2pll_pmd_osmode_set(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd_intf, int os_mode);
extern int temod2pll_pmd_osmode_get(PHYMOD_ST* pc, int *os_mode);
extern int temod2pll_pll_lock_get(PHYMOD_ST* pc, int* lockStatus);
extern int temod2pll_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus);
extern int temod2pll_wait_pmd_lock(PHYMOD_ST* pc, int timeOutValue, int* lockStatus);
extern int temod2pll_pcs_bypass_ctl (PHYMOD_ST* pc, int cntl);
extern int temod2pll_pmd_reset_bypass (PHYMOD_ST* pc, int cntl);
extern int temod2pll_set_pll_mode(PHYMOD_ST* pc, int pmd_tched, temod2pll_spd_intfc_type sp, int pll_mode, int dual_pll);
extern int temod2pll_tick_override_set(PHYMOD_ST* pc, int tick_override, int numerator, int denominator);
extern int temod2pll_tx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int temod2pll_tx_pmd_loopback_control(PHYMOD_ST* pc, int cntl);
extern int temod2pll_rx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int temod2pll_encode_set(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd_intf, uint16_t enable);
extern int temod2pll_decode_set(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd_intf, uint16_t enable);
extern int temod2pll_hg_enable_get(PHYMOD_ST* pc, int *enable);
extern int temod2pll_spd_intf_get(PHYMOD_ST* pc, int *spd_intf);
extern int temod2pll_revid_read(PHYMOD_ST* pc, uint32_t *revid);
extern int temod2pll_init_pcs(PHYMOD_ST* pc);
extern int temod2pll_pmd_reset_seq_dp(PHYMOD_ST* pc, int pmd_touched);
extern int temod2pll_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched);
extern int temod2pll_pram_abl_enable_set(PHYMOD_ST* pc, int enable);
extern int temod2pll_wait_sc_done(PHYMOD_ST* pc, uint16_t *data);
extern int temod2pll_get_pcs_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int temod2pll_get_pcs_latched_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int temod2pll_disable_set(PHYMOD_ST* pc);
extern int temod2pll_init_pcs_ilkn(PHYMOD_ST* pc);
extern int temod2pll_pcs_ilkn_chk(PHYMOD_ST* pc, int *ilkn_set);
extern int temod2pll_pmd_x4_reset(PHYMOD_ST* pc);
extern int temod2pll_pmd_x4_reset_get(PHYMOD_ST* pc, int *is_in_reset);
extern int temod2pll_init_pmd_sw(PHYMOD_ST* pc, int pmd_touched, int uc_active,
                             temod2pll_spd_intfc_type spd_intf,  int t_pma_os_mode);
extern int temod2pll_fecmode_set(PHYMOD_ST* pc, int fec_enable);
extern int temod2pll_fecmode_get(PHYMOD_ST* pc, uint32_t *fec_enable);
extern int temod2pll_credit_override_set(PHYMOD_ST* pc, credit_type_t credit_type, uint16_t  userCredit);
extern int temod2pll_credit_override_get(PHYMOD_ST* pc, credit_type_t credit_type, uint16_t *userCredit_en, uint16_t *userCredit);
extern int temod2pll_cjpat_crpat_mode_set(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd, int port, int pat, int pnum, int size, int ipgsz);
extern int temod2pll_cjpat_crpat_check(PHYMOD_ST* pc);
extern int temod2pll_cjpat_crpat_control(PHYMOD_ST* pc, int enable);
extern int temod2pll_pcs_rx_lane_swap(PHYMOD_ST *pc, int rx_lane_map);
extern int temod2pll_pcs_tx_lane_swap(PHYMOD_ST *pc, int tx_lane_map);
extern int temod2pll_trigger_speed_change(PHYMOD_ST* pc);
extern int temod2pll_init_pmd_qsgmii(PHYMOD_ST* pc, int pmd_touched, int uc_active);
extern int temod2pll_rx_lane_control_get(PHYMOD_ST* pc, int *value);
extern int temod2pll_pmd_override_control(PHYMOD_ST* pc, int cntl, int value);
extern int temod2pll_plldiv_lkup_get(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd_intf, phymod_ref_clk_t ref_clk, uint32_t *plldiv, uint16_t *speed_vec);
extern int temod2pll_osmode_lkup_get(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd_intf, uint32_t *osmode);
extern int temod2pll_osdfe_on_lkup_get(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd_intf, uint32_t *osdfe_on);
extern int temod2pll_scrambling_dis_lkup_get(PHYMOD_ST* pc, temod2pll_spd_intfc_type spd_intf, uint32_t *scrambling_dis);

extern int temod2pll_port_enable_set(PHYMOD_ST *pc, int enable);
extern int temod2pll_tx_squelch_set(PHYMOD_ST *pc, int enable);
extern int temod2pll_rx_squelch_set(PHYMOD_ST *pc, int enable);
extern int temod2pll_port_enable_get(PHYMOD_ST *pc, int *tx_enable, int *rx_enable);
extern int temod2pll_tx_squelch_get(PHYMOD_ST *pc, int *enable);
extern int temod2pll_rx_squelch_get(PHYMOD_ST *pc, int *enable);
int temod2pll_eee_control_set(PHYMOD_ST* pc, uint32_t enable);
int temod2pll_eee_control_get(PHYMOD_ST* pc, uint32_t *enable);
extern int temod2pll_pll_config_get(const phymod_access_t *pa, temod2pll_pll_mode_type  *pll_mode);
extern int temod2pll_pll_config_set(const phymod_access_t *pa, temod2pll_pll_mode_type  pll_mode, phymod_ref_clk_t ref_clk);
#ifdef _SDK_TEMOD2PLL_
extern int temod2pll_autoneg_set_init(PHYMOD_ST* pc, temod2pll_an_init_t *an_init_st);
extern int temod2pll_autoneg_control(PHYMOD_ST* pc, temod2pll_an_control_t *an_control);
extern int temod2pll_autoneg_restart(PHYMOD_ST* pc, temod2pll_an_control_t *an_control);
extern int temod2pll_autoneg_timer_init(PHYMOD_ST* pc);
extern int temod2pll_cl48_lfrfli_init(PHYMOD_ST* pc);
extern int temod2pll_autoneg_set(PHYMOD_ST* pc, temod2pll_an_ability_t *an_ability_st);
extern int temod2pll_set_an_port_mode(PHYMOD_ST* pc, int enable, int nl_encoded, int starting_lane, int single_port);
extern int temod2pll_refclk_set(PHYMOD_ST* pc, phymod_ref_clk_t ref_clock);
extern int temod2pll_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map);
extern int temod2pll_pmd_addr_lane_swap (PHYMOD_ST *pc, uint32_t addr_lane_index);
extern int temod2pll_tsc12_control(PHYMOD_ST* pc, int cl82_multi_pipe_mode, int cl82_mld_phys_map);
int temod2pll_tx_loopback_get(PHYMOD_ST* pc, uint32_t *enable);
extern int temod2pll_autoneg_local_ability_get(PHYMOD_ST* pc, temod2pll_an_ability_t *an_ability_st);
extern int temod2pll_autoneg_remote_ability_get(PHYMOD_ST* pc, temod2pll_an_ability_t *an_ability_st);
extern int temod2pll_autoneg_control_get(PHYMOD_ST* pc, temod2pll_an_control_t *an_control, int *an_complete);
extern  int temod2pll_diag(PHYMOD_ST *pc, temod2pll_diag_type diag_type);
extern  int temod2pll_diag_disp(PHYMOD_ST *pc, const char* mystr);
extern int temod2pll_clause72_control(PHYMOD_ST* pc, uint32_t cl_72_en);                /* CLAUSE_72_CONTROL */

extern int temod2pll_st_control_field_set (PHYMOD_ST* pc,uint16_t st_entry_no,override_type_t  st_control_field,uint16_t st_field_value);
extern int temod2pll_st_control_field_get (PHYMOD_ST* pc,uint16_t st_entry_no, override_type_t  st_control_field, uint16_t *st_field_value);
extern int temod2pll_st_credit_field_set (PHYMOD_ST* pc,uint16_t st_entry_no,credit_type_t  credit_type,uint16_t st_field_value);
extern int temod2pll_st_credit_field_get (PHYMOD_ST* pc,uint16_t st_entry_no,credit_type_t  credit_type,uint16_t* st_field_value);
extern int temod2pll_mld_am_timers_set(PHYMOD_ST* pc);
extern int temod2pll_an_ncl72_inhibit_timer_get(PHYMOD_ST* pc, uint16_t *value);
extern int temod2pll_an_ncl72_inhibit_timer_set(PHYMOD_ST* pc, uint16_t value);
extern int temod2pll_cl74_chng_default (PHYMOD_ST* pc);

extern int temod2pll_an_oui_set(PHYMOD_ST* pc, temod2pll_an_oui_t oui);
extern int temod2pll_an_oui_get(PHYMOD_ST* pc, temod2pll_an_oui_t *oui);
extern int temod2pll_cl37_high_vco_set(PHYMOD_ST* pc, int value);
extern int temod2pll_pcs_ilkn_mode_set(PHYMOD_ST* pc);              /* INIT_PCS_ILKN */
extern int temod2pll_pll_select(PHYMOD_ST* pc, int pll_select);
extern uint8_t temod2pll_pll_index_get(PHYMOD_ST* pc);
extern int temod2pll_1588_ts_enable_set(PHYMOD_ST* pc, uint32_t ts_enable);
extern int temod2pll_1588_ts_enable_get(PHYMOD_ST* pc, uint32_t *ts_enable);
extern int temod2pll_1588_ts_offset_set(PHYMOD_ST* pc, uint32_t ts_offset);
extern int temod2pll_1588_pcs_control_set(PHYMOD_ST* pc);
extern int temod2pll_1588_fclk_set(PHYMOD_ST* pc);
extern int temod2pll_1588_ui_values_set(PHYMOD_ST* pc);
extern int temod2pll_1588_txrx_fixed_latency_set(PHYMOD_ST* pc);
extern int temod2pll_1588_rx_deskew_set(PHYMOD_ST* pc, phymod_timesync_compensation_mode_t timesync_am_norm_mode);
extern int temod2pll_bond_in_pwrdn_override(PHYMOD_ST* pc, uint32_t enable);
extern int temod2pll_hg2_codec_set(PHYMOD_ST* pc, temod2pll_hg2_codec_t hg2_codec);
extern int temod2pll_hg2_codec_get(PHYMOD_ST* pc, temod2pll_hg2_codec_t* hg2_codec);
extern int temod2pll_1588_tx_info_get(PHYMOD_ST* pc, temod2pll_ts_tx_info_t* ts_tx_info);
#endif

#ifdef _DV_TB_
extern int temod2pll_1588_set_rx_deskew(PHYMOD_ST* pc);
#endif

extern int temod2pll_pll_select(PHYMOD_ST* pc, int pll_select);
extern int temod2pll_fec_correctable_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int temod2pll_fec_uncorrectable_counter_get(PHYMOD_ST* pc, uint32_t* count);
int temod2pll_get_mapped_speed(temod2pll_spd_intfc_type spd_intf, int *speed);
extern int temod2pll_get_ui_values(int speed, uint16_t *ui_msb, uint16_t *ui_lsb);
extern int temod2pll_get_am_bits(int speed, long *am_bits);
extern int temod2pll_get_txrx_fixed_lat(int speed, uint16_t *tx_fixed_int, uint16_t *tx_fixed_frac, uint16_t *rx_fixed_int, uint16_t *rx_fixed_frac);
#ifdef GEN2_16NM_CODE
int temod2pll_get_pll_ndiv(int pll_mode, int *pll_ndiv);
#endif /*GEN2_16NM_CODE*/
int temod2pll_get_actual_speed(int speed_id, int *speed);

extern int temod2pll_synce_stg0_mode_set(PHYMOD_ST *pc, int mode);
extern int temod2pll_synce_stg0_mode_get(PHYMOD_ST *pc, int *mode);
extern int temod2pll_synce_stg1_mode_set(PHYMOD_ST *pc, int mode);
extern int temod2pll_synce_stg1_mode_get(PHYMOD_ST *pc, int *mode);
extern int temod2pll_synce_clk_ctrl_set(PHYMOD_ST *pc, uint32_t val);
extern int temod2pll_synce_clk_ctrl_get(PHYMOD_ST *pc, uint32_t *val);
extern int temod2pll_refclk_get(PHYMOD_ST* pc, phymod_ref_clk_t* ref_clk);
extern int temod2pll_fec_error_inject_set(PHYMOD_ST *pc,
                                          uint16_t error_control_map,
                                          temod2pll_fec_error_mask_t bit_error_mask);
extern int temod2pll_fec_error_inject_get(PHYMOD_ST *pc,
                                          uint16_t* error_control_map,
                                          temod2pll_fec_error_mask_t* bit_error_mask);

#endif  /*  _temod2pll_H_ */

