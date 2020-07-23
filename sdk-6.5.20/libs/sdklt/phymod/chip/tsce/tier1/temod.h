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
#ifndef _TEMOD_H_
#define _TEMOD_H_

#ifndef _DV_TB_
#define _SDK_TEMOD_ 1
#endif

#ifdef _SDK_TEMOD_
#include <phymod/phymod.h>
#endif

#ifndef PHYMOD_ST
#ifdef _SDK_TEMOD_
  #define PHYMOD_ST  const phymod_access_t
#else
  #define PHYMOD_ST  temod_st
#endif /* _SDK_TEMOD_ */
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
#define TEMOD_DBG_REGACC     (1L << 3) * Print all register accesses * 
#define TEMOD_DBG_FUNCVALOUT (1L << 2) * All values returned by Tier1*
#define TEMOD_DBG_FUNCVALIN  (1L << 1) * All values pumped into Tier1*
#define TEMOD_DBG_FUNC       (1L << 0) * Every time we enter a  Tier1*
*/

#define TEMOD_OVERRIDE_CFG 0x80000000

typedef enum {
    TEMOD_NONE = 0, 
    TEMOD_DECODER_CL49,
    TEMOD_DECODER_BRCMCL49,
    TEMOD_DECODER_CUSTOM,
    TEMOD_DECODER_CL48,
    TEMOD_DECODER_CL36,
    TEMOD_DECODER_TYPE_COUNT
}temod_decoder_type_t;

typedef enum {
    TEMOD_ENCODER_DISABLE = 0, 
    TEMOD_ENCODER_CL48,
    TEMOD_ENCODER_RXAUI_CL48,
    TEMOD_ENCODER_CL36,
    TEMOD_ENCODER_CL82,
    TEMOD_ENCODER_CL49,
    TEMOD_ENCODER_BRCMCL49,
    TEMOD_ENCODER_TYPE_COUNT
}temod_encoder_type_t;

typedef enum {
    TEMOD_AN_SET_RF_DISABLE = 0,
    TEMOD_AN_SET_SGMII_SPEED,
    TEMOD_AN_SET_SGMII_MASTER,
    TEMOD_AN_SET_HG_MODE,
    TEMOD_AN_SET_FEC_MODE,
    TEMOD_AN_SET_CL72_MODE,
    TEMOD_AN_SET_CL37_ATTR,
    TEMOD_AN_SET_CL48_SYNC,
    TEMOD_AN_SET_LK_FAIL_INHIBIT_TIMER_NO_CL72,
    TEMOD_AN_SET_CL73_FEC_OFF,
    TEMOD_AN_SET_CL73_FEC_ON,
    TEMOD_AN_SET_SPEED_PAUSE,
    TEMOD_AN_SET_TYPE_COUNT
}temod_an_set_type_t;

typedef union {
    int temod_an_set_rf_disable;
    int temod_an_set_sgmii_speed;
    int temod_an_set_sgmii_master;
    temod_an_set_type_t temod_an_set_cl37_attr;
}temod_an_set_value_t;     
    

typedef enum {
    TEMOD_AN_MODE_CL73 = 0,
    TEMOD_AN_MODE_CL37,
    TEMOD_AN_MODE_CL73BAM ,
    TEMOD_AN_MODE_CL37BAM ,
    TEMOD_AN_MODE_SGMII  ,
    TEMOD_AN_MODE_HPAM ,          /* this macro needed to be identical to temod_an_type */
    TEMOD_AN_MODE_CL37_SGMII,
    TEMOD_AN_MODE_NONE ,
    TEMOD_AN_MODE_TYPE_COUNT
}temod_an_mode_type_t;

typedef enum {
    TEMOD_AN_PORT_MODE_QUAD = 0,    /* 4 port on the core */
    TEMOD_AN_PORT_MODE_SINGLE,      /* 1 port using all 4 lane */
    TEMOD_AN_PORT_MODE_DUAL,        /* two port each using 2 lanes */
    TEMOD_AN_PORT_MODE_TRI_23,      /* three port, lane 2 and3 are aggregated */
    TEMOD_AN_PORT_MODE_TRI_10,      /* three ports, lane 0 and 1 are aggregated */
    TSMOD_AN_PORT_MODE_TYPE_COUNT
}an_port_mode_type_t;

typedef enum {
    TEMOD_TX_LANE_RESET = 0,
    TEMOD_TX_LANE_TRAFFIC_ENABLE = 1,  
    TEMOD_TX_LANE_TRAFFIC_DISABLE = 2,  
    TEMOD_TX_LANE_RESET_TRAFFIC_ENABLE = 3,
    TEMOD_TX_LANE_RESET_TRAFFIC_DISABLE = 4,
    TEMOD_TX_LANE_TYPE_COUNT
}tx_lane_disable_type_t;

typedef struct temod_an_control_s {
  temod_an_mode_type_t an_type; 
  uint16_t num_lane_adv; 
  uint16_t enable;
  uint16_t pd_kx_en;
  uint16_t pd_kx4_en; 
  an_property_enable  an_property_type;
} temod_an_control_t;

typedef enum {
    TEMOD_CL73_100GBASE_CR10 = 0,  
    TEMOD_CL73_40GBASE_CR4,  
    TEMOD_CL73_40GBASE_KR4,  
    TEMOD_CL73_10GBASE_KR,
    TEMOD_CL73_10GBASE_KX4,
    TEMOD_CL73_1000BASE_KX,
    TEMOD_CL73_SPEED_COUNT
}temod_cl73_speed_t;

typedef enum {
    TEMOD_CL73_BAM_20GBASE_KR2 = 0,  
    TEMOD_CL73_BAM_20GBASE_CR2,  
    TEMOD_CL73_BAM_SPEED_COUNT
}temod_cl73_bam_speed_t;

typedef enum {
    TEMOD_CL37_SGMII_10M = 0,  
    TEMOD_CL37_SGMII_100M,  
    TEMOD_CL37_SGMII_1000M,  
    TEMOD_CL37_SGMII_SPEED_COUNT
}temod_cl37_sgmii_speed_t;


typedef enum {
    TEMOD_CL37_BAM_13GBASE_X4 = 0,  
    TEMOD_CL37_BAM_15GBASE_X4,  
    TEMOD_CL37_BAM_15p75GBASE_X2,  
    TEMOD_CL37_BAM_16GBASE_X4,  
    TEMOD_CL37_BAM_20GBASE_X4_CX4,  
    TEMOD_CL37_BAM_20GBASE_X4,  
    TEMOD_CL37_BAM_20GBASE_X2,  
    TEMOD_CL37_BAM_20GBASE_X2_CX4,  
    TEMOD_CL37_BAM_21GBASE_X4,  
    TEMOD_CL37_BAM_25p455GBASE_X4,  
    TEMOD_CL37_BAM_31p5GBASE_X4,  
    TEMOD_CL37_BAM_32p7GBASE_X4,  
    TEMOD_CL37_BAM_40GBASE_X4,  
    TEMOD_CL37_BAM_SPEED1_COUNT
}temod_cl37_bam_speed1_t;

typedef enum {
    TEMOD_CL37_BAM_2p5GBASE_X = 0,  
    TEMOD_CL37_BAM_5GBASE_X4,  
    TEMOD_CL37_BAM_6GBASE_X4,  
    TEMOD_CL37_BAM_10GBASE_X4,  
    TEMOD_CL37_BAM_10GBASE_X4_CX4,  
    TEMOD_CL37_BAM_10GBASE_X2,  
    TEMOD_CL37_BAM_10GBASE_X2_CX4,  
    TEMOD_CL37_BAM_BAM_10p5GBASE_X2,  
    TEMOD_CL37_BAM_12GBASE_X4,  
    TEMOD_CL37_BAM_12p5GBASE_X4,
    TEMOD_CL37_BAM_12p7GBASE_X2,
    TEMOD_CL37_BAM_SPEED_COUNT
}temod_cl37_bam_speed_t;


typedef enum {
    TEMOD_NO_PAUSE = 0,  
    TEMOD_SYMM_PAUSE,
    TEMOD_ASYM_PAUSE,
    TEMOD_ASYM_SYMM_PAUSE,  
    TEMOD_AN_PAUSE_COUNT
}temod_an_pause_t;

typedef enum {
    TEMOD_HG2_CODEC_OFF = 0,
    TEMOD_HG2_CODEC_ON_8BYTE_IPG,
    TEMOD_HG2_CODEC_ON_9BYTE_IPG,
    TEMOD_HG2_CODEC_COUNT
}temod_hg2_codec_t;

#define TEMOD_FEC_NOT_SUPRTD         0
#define TEMOD_FEC_SUPRTD_NOT_REQSTD  1
#define TEMOD_FEC_CL74_SUPRTD_REQSTD 4

#define TEMOD_SGMII_SLAVE            0x0
#define TEMOD_SGMII_MASTER           0x1

/**
\struct temod_an_init_s

This embodies all parameters required to program autonegotiation features in the
PHY for both CL37 and CL73 type autonegotiation and the BAM variants.
For details of individual fields, please refer to microarchitectural doc.
*/
typedef struct temod_an_init_s{
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
} temod_an_init_t;


/**
\struct temod_an_timers_s
TBD
*/
typedef struct temod_an_timers_s{
  uint16_t  value;
} temod_an_timers_t;


/**
\struct temod_an_adv_ability_s
TBD
*/
typedef struct temod_an_adv_ability_s{
  temod_cl73_speed_t an_base_speed; 
  temod_cl37_bam_speed_t an_bam_speed; 
  temod_cl37_bam_speed1_t an_bam_speed1; 
  temod_an_pause_t an_pause; 
  uint16_t an_fec; 
  uint16_t an_cl72;
  uint16_t an_hg2; 
  temod_cl37_sgmii_speed_t cl37_sgmii_speed;
}temod_an_adv_ability_t;

/**
\struct temod_an_ability_s

TBD

*/
typedef struct temod_an_ability_s {
  temod_an_adv_ability_t cl37_adv; /*includes cl37 and cl37-bam related (fec, cl72) */
  temod_an_adv_ability_t cl73_adv; /*includes cl73 and cl73-bam related */
} temod_an_ability_t;

/**
\struct temod_an_oui_s
  oui                : New oui
  override_bam73_adv : Adv new OUI for BAM73
  override_bam73_det : Detect new OUI for BAM73
  override_hpam_adv  : Adv new OUI for HPAM
  override_hpam_det  : Detect new OUI for HPAM
  override_bam37_adv : Adv new OUI for BAM37
  override_bam37_det : Detect new OUI for BAM37
 */
typedef struct temod_an_oui_s {
  uint32_t oui;
  uint16_t oui_override_bam73_adv;
  uint16_t oui_override_bam73_det;
  uint16_t oui_override_hpam_adv;
  uint16_t oui_override_hpam_det;
  uint16_t oui_override_bam37_adv;
  uint16_t oui_override_bam37_det;
} temod_an_oui_t;

extern int temod_pll_reset_enable_set (PHYMOD_ST *pa , int enable);
extern int temod_speed_id_get (PHYMOD_ST *pc,  int *speed_id);
extern int temod_update_port_mode( PHYMOD_ST *pa, int *pll_restart);
extern int temod_get_plldiv (PHYMOD_ST *pa,  uint32_t *pll_div);
extern int temod_tx_rx_polarity_get (PHYMOD_ST *pc, uint32_t* txp, uint32_t* rxp);
extern int temod_rx_lane_control_set(PHYMOD_ST* pc, int enable);
extern int temod_rx_lane_control_get(PHYMOD_ST* pc, int *enable);
extern int temod_tx_rx_polarity_set (PHYMOD_ST *pc, uint32_t txp, uint32_t rxp);
extern int temod_pmd_lane_swap_tx_get (PHYMOD_ST *pc, uint32_t *tx_lane_map);
extern int temod_pmd_addr_lane_swap (PHYMOD_ST *pc, uint32_t addr_lane_index);
extern int temod_disable_get (PHYMOD_ST *pc, uint32_t* enable);
extern int temod_pll_sequencer_control(PHYMOD_ST *pa, int enable);
extern int temod_pcs_lane_swap_get (PHYMOD_ST *pc,  uint32_t *tx_rx_swap);
extern int temod_master_port_num_set(PHYMOD_ST *pa,  int port_num);
extern int temod_set_spd_intf(PHYMOD_ST *pc, temod_spd_intfc_type spd_intf, int no_trig);
extern int temod_pll_lock_wait(PHYMOD_ST* pc, int timeOutValue);
extern int temod_credit_control(PHYMOD_ST* pc, int enable);
extern int temod_tx_lane_control_set(PHYMOD_ST* pc, tx_lane_disable_type_t tx_dis_type);
extern int temod_tx_lane_control_get(PHYMOD_ST* pc, int *reset, int *enable);
extern int temod_override_clear(PHYMOD_ST* pc, override_type_t or_type);
extern int temod_override_set(PHYMOD_ST* pc, override_type_t or_type, uint16_t or_val);
extern int temod_override_get(PHYMOD_ST* pc, override_type_t or_type, uint16_t *or_en,  uint16_t *or_val);
extern int temod_credit_set(PHYMOD_ST* pc, credit_type_t credit_type, uint16_t userCredit);
extern int temod_rx_lane_control_set(PHYMOD_ST* pc, int enable);        
extern int temod_rx_lane_control_get(PHYMOD_ST* pc, int *enable);        
extern int temod_init_pmd(PHYMOD_ST* pc, int pmd_touched, int uc_active);
extern int temod_pmd_osmode_set(PHYMOD_ST* pc, temod_spd_intfc_type spd_intf, int os_mode);
extern int temod_pmd_osmode_get(PHYMOD_ST* pc, int *os_mode);
extern int temod_pll_lock_get(PHYMOD_ST* pc, int* lockStatus);
extern int temod_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus);
extern int temod_wait_pmd_lock(PHYMOD_ST* pc, int timeOutValue, int* lockStatus);
extern int temod_pcs_bypass_ctl (PHYMOD_ST* pc, int cntl);
extern int temod_pmd_reset_bypass (PHYMOD_ST* pc, int cntl);
extern int temod_set_pll_mode(PHYMOD_ST* pc, int pmd_tched, temod_spd_intfc_type sp, int pll_mode);
extern int temod_tick_override_set(PHYMOD_ST* pc, int tick_override, int numerator, int denominator);
extern int temod_tx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int temod_tx_pmd_loopback_control(PHYMOD_ST* pc, int cntl);
extern int temod_rx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int temod_encode_set(PHYMOD_ST* pc, temod_spd_intfc_type spd_intf, uint16_t enable);
extern int temod_decode_set(PHYMOD_ST* pc, temod_spd_intfc_type spd_intf, uint16_t enable);
extern int temod_hg_enable_get(PHYMOD_ST* pc, int *enable);
extern int temod_spd_intf_get(PHYMOD_ST* pc, int *spd_intf);
extern int temod_revid_read(PHYMOD_ST* pc, uint32_t *revid);
extern int temod_init_pcs(PHYMOD_ST* pc);
extern int temod_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched);
extern int temod_pram_abl_enable_set(PHYMOD_ST* pc, int enable);
extern int temod_wait_sc_done(PHYMOD_ST* pc, uint16_t *data);
extern int temod_get_pcs_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int temod_get_pcs_latched_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int temod_disable_set(PHYMOD_ST* pc);
extern int temod_init_pcs_ilkn(PHYMOD_ST* pc);
extern int temod_pcs_ilkn_chk(PHYMOD_ST* pc, int *ilkn_set);
extern int temod_pmd_x4_reset(PHYMOD_ST* pc);
extern int temod_init_pmd_sw(PHYMOD_ST* pc, int pmd_touched, int uc_active,
                             temod_spd_intfc_type spd_intf,  int t_pma_os_mode);
extern int temod_fecmode_set(PHYMOD_ST* pc, int fec_enable);
extern int temod_fecmode_get(PHYMOD_ST* pc, uint32_t *fec_enable);
extern int temod_credit_override_set(PHYMOD_ST* pc, credit_type_t credit_type, uint16_t  userCredit);
extern int temod_credit_override_get(PHYMOD_ST* pc, credit_type_t credit_type, uint16_t *userCredit_en, uint16_t *userCredit);

extern int temod_pcs_lane_swap(PHYMOD_ST *pc, int lane_map);
extern int temod_trigger_speed_change(PHYMOD_ST* pc);
extern int temod_init_pmd_qsgmii(PHYMOD_ST* pc, int pmd_touched, int uc_active);
extern int temod_rx_lane_control_get(PHYMOD_ST* pc, int *value);
extern int temod_pmd_override_control(PHYMOD_ST* pc, int cntl, int value);
extern int temod_plldiv_lkup_get(PHYMOD_ST* pc, temod_spd_intfc_type spd_intf, phymod_ref_clk_t ref_clk, uint32_t *plldiv, uint16_t *speed_vec);
extern int temod_osmode_lkup_get(PHYMOD_ST* pc, temod_spd_intfc_type spd_intf, uint32_t *osmode);
extern int temod_osdfe_on_lkup_get(PHYMOD_ST* pc, temod_spd_intfc_type spd_intf, uint32_t *osdfe_on);
extern int temod_scrambling_dis_lkup_get(PHYMOD_ST* pc, temod_spd_intfc_type spd_intf, uint32_t *scrambling_dis);


extern int temod_port_enable_set(PHYMOD_ST *pc, int enable);
extern int temod_tx_squelch_set(PHYMOD_ST *pc, int enable);
extern int temod_rx_squelch_set(PHYMOD_ST *pc, int enable);
extern int temod_port_enable_get(PHYMOD_ST *pc, int *tx_enable, int *rx_enable);
extern int temod_tx_squelch_get(PHYMOD_ST *pc, int *enable);
extern int temod_rx_squelch_get(PHYMOD_ST *pc, int *enable);

int temod_eee_control_set(PHYMOD_ST* pc, uint32_t enable);
int temod_eee_control_get(PHYMOD_ST* pc, uint32_t *enable);

extern int temod_pll_config_get(const phymod_access_t *pa, temod_pll_mode_type  *pll_mode);
extern int temod_pll_config_set(const phymod_access_t *pa, temod_pll_mode_type  pll_mode, phymod_ref_clk_t ref_clk);

#ifdef _DV_TB_
extern int temod_mld_am_timers_set(PHYMOD_ST* pc, int rx_am_timer_init,int tx_am_timer_init_val);
#endif /* _DV_TB_ */

#ifdef _SDK_TEMOD_
extern int temod_autoneg_set_init(PHYMOD_ST* pc, temod_an_init_t *an_init_st); 
extern int temod_autoneg_control(PHYMOD_ST* pc, temod_an_control_t *an_control);
extern int temod_autoneg_restart(PHYMOD_ST* pc, temod_an_control_t *an_control);
extern int temod_autoneg_timer_init(PHYMOD_ST* pc);
extern int temod_cl48_lfrfli_init(PHYMOD_ST* pc);
extern int temod_autoneg_set(PHYMOD_ST* pc, temod_an_ability_t *an_ability_st);
extern int temod_set_an_port_mode(PHYMOD_ST* pc, int enable, int nl_encoded, int starting_lane, int single_port);
extern int temod_refclk_set(PHYMOD_ST* pc, phymod_ref_clk_t ref_clock); 
extern int temod_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map);
extern int temod_pmd_addr_lane_swap (PHYMOD_ST *pc, uint32_t addr_lane_index);
extern int temod_tsc12_control(PHYMOD_ST* pc, int cl82_multi_pipe_mode, int cl82_mld_phys_map);
int temod_tx_loopback_get(PHYMOD_ST* pc, uint32_t *enable);      
extern int temod_autoneg_local_ability_get(PHYMOD_ST* pc, temod_an_ability_t *an_ability_st);
extern int temod_autoneg_remote_ability_get(PHYMOD_ST* pc, temod_an_ability_t *an_ability_st);
extern int temod_autoneg_control_get(PHYMOD_ST* pc, temod_an_control_t *an_control, int *an_complete);
extern  int temod_diag(PHYMOD_ST *pc, temod_diag_type diag_type);
extern int temod_diag_disp(PHYMOD_ST *pc, const char* cmd_str);
extern int temod_clause72_control(PHYMOD_ST* pc, uint32_t cl_72_en);                /* CLAUSE_72_CONTROL */

extern int temod_st_control_field_set (PHYMOD_ST* pc,uint16_t st_entry_no,override_type_t  st_control_field,uint16_t st_field_value);
extern int temod_st_control_field_get (PHYMOD_ST* pc,uint16_t st_entry_no, override_type_t  st_control_field, uint16_t *st_field_value);
extern int temod_st_credit_field_set (PHYMOD_ST* pc,uint16_t st_entry_no,credit_type_t  credit_type,uint16_t st_field_value);
extern int temod_st_credit_field_get (PHYMOD_ST* pc,uint16_t st_entry_no,credit_type_t  credit_type,uint16_t* st_field_value);
extern int temod_mld_am_timers_set(PHYMOD_ST* pc);
extern int temod_an_ncl72_inhibit_timer_get(PHYMOD_ST* pc, uint16_t *value);
extern int temod_an_ncl72_inhibit_timer_set(PHYMOD_ST* pc, uint16_t value);
extern int temod_cl74_chng_default (PHYMOD_ST* pc);

extern int temod_an_oui_set(PHYMOD_ST* pc, temod_an_oui_t oui);
extern int temod_an_oui_get(PHYMOD_ST* pc, temod_an_oui_t *oui);
extern int temod_cl37_high_vco_set(PHYMOD_ST* pc, int value);
extern int temod_pcs_ilkn_mode_set(PHYMOD_ST* pc);              /* INIT_PCS_ILKN */
extern int temod_pmd_x4_reset_get(PHYMOD_ST* pc, int *value);

extern int temod_sgmii_master_set(PHYMOD_ST* pc, int master);
extern int temod_sgmii_master_get(PHYMOD_ST* pc, int* master);
extern int temod_synce_mode_set(PHYMOD_ST *pc, int mode);
extern int temod_synce_mode_get(PHYMOD_ST *pc, int *mode);
#endif

extern int temod_fec_correctable_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int temod_fec_uncorrectable_counter_get(PHYMOD_ST* pc, uint32_t* count);

extern int temod_encode_mode_get(PHYMOD_ST* pc, temod_encoder_type_t *mode);
extern int temod_bip_error_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int temod_cl49_ber_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int temod_cl82_ber_counter_get(PHYMOD_ST* pc, uint32_t* count);

int get_mapped_speed(temod_spd_intfc_type spd_intf, int *speed);
int get_actual_speed(int speed_id, int *speed);

extern int temod_hg2_codec_set(PHYMOD_ST* pc, temod_hg2_codec_t hg2_codec);
extern int temod_hg2_codec_get(PHYMOD_ST* pc, temod_hg2_codec_t* hg2_codec);
#endif  /*  _temod_H_ */

