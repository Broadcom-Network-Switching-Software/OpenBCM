/*----------------------------------------------------------------------
 * $Id: temod16.h,
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

#ifndef _TEMOD16_H_
#define _TEMOD16_H_

#ifndef _DV_TB_
#define _SDK_TEMOD16_ 1
#endif

#ifdef _SDK_TEMOD16_
#include <phymod/phymod.h>
#endif

#ifndef PHYMOD_ST
#ifdef _SDK_TEMOD16_
  #define PHYMOD_ST   const phymod_access_t
#else
  #define PHYMOD_ST  temod16_st
#endif /* _SDK_TEMOD16_ */
#endif /* PHYMOD_ST */

#ifdef _DV_TB_
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"
#endif

typedef enum {
    TEMOD16_AN_MODE_CL73 = 0,
    TEMOD16_AN_MODE_CL37,
    TEMOD16_AN_MODE_CL73BAM,
    TEMOD16_AN_MODE_CL37BAM,
    TEMOD16_AN_MODE_SGMII,
    TEMOD16_AN_MODE_HPAM,
    TEMOD16_AN_MODE_CL37_SGMII,
    TEMOD16_AN_MODE_NONE,
    TSMOD16_AN_MODE_TYPE_COUNT
}temod16_an_mode_type_t;

typedef enum {
    TEMOD16_CL37_SGMII_10M = 0,
    TEMOD16_CL37_SGMII_100M,
    TEMOD16_CL37_SGMII_1000M,
    TEMOD16_CL37_SGMII_SPEED_COUNT
}temod16_cl37_sgmii_speed_t;

typedef enum {
    TEMOD16_NO_PAUSE = 0,
    TEMOD16_SYMM_PAUSE,
    TEMOD16_ASYM_PAUSE,
    TEMOD16_ASYM_SYMM_PAUSE,
    TEMOD16_AN_PAUSE_COUNT
}temod16_an_pause_t;

typedef enum {
    TEMOD16_CL73_100GBASE_CR10 = 0,
    TEMOD16_CL73_40GBASE_CR4,
    TEMOD16_CL73_40GBASE_KR4,
    TEMOD16_CL73_10GBASE_KR,
    TEMOD16_CL73_10GBASE_KX4,
    TEMOD16_CL73_1000BASE_KX,
    TEMOD16_CL73_SPEED_COUNT
}temod16_cl73_speed_t;

typedef enum {
    TEMOD16_CL37_BAM_2p5GBASE_X = 0,
    TEMOD16_CL37_BAM_5GBASE_X4,
    TEMOD16_CL37_BAM_6GBASE_X4,
    TEMOD16_CL37_BAM_10GBASE_X4,
    TEMOD16_CL37_BAM_10GBASE_X4_CX4,
    TEMOD16_CL37_BAM_10GBASE_X2,
    TEMOD16_CL37_BAM_10GBASE_X2_CX4,
    TEMOD16_CL37_BAM_BAM_10p5GBASE_X2,
    TEMOD16_CL37_BAM_12GBASE_X4,
    TEMOD16_CL37_BAM_12p5GBASE_X4,
    TEMOD16_CL37_BAM_12p7GBASE_X2,
    TEMOD16_CL37_BAM_SPEED_COUNT
}temod16_cl37_bam_speed_t;

typedef enum {
    TEMOD16_CL37_BAM_13GBASE_X4 = 0,
    TEMOD16_CL37_BAM_15GBASE_X4,
    TEMOD16_CL37_BAM_15p75GBASE_X2,
    TEMOD16_CL37_BAM_16GBASE_X4,
    TEMOD16_CL37_BAM_20GBASE_X4_CX4,
    TEMOD16_CL37_BAM_20GBASE_X4,
    TEMOD16_CL37_BAM_20GBASE_X2,
    TEMOD16_CL37_BAM_20GBASE_X2_CX4,
    TEMOD16_CL37_BAM_21GBASE_X4,
    TEMOD16_CL37_BAM_25p455GBASE_X4,
    TEMOD16_CL37_BAM_31p5GBASE_X4,
    TEMOD16_CL37_BAM_32p7GBASE_X4,
    TEMOD16_CL37_BAM_40GBASE_X4,
    TEMOD16_CL37_BAM_SPEED1_COUNT
}temod16_cl37_bam_speed1_t;

typedef enum {
    TEMOD16_CL73_BAM_20GBASE_KR2 = 0,
    TEMOD16_CL73_BAM_20GBASE_CR2,
    TEMOD16_CL73_BAM_SPEED_COUNT
}temod16_cl73_bam_speed_t;

#define TEMOD16_FEC_NOT_SUPRTD         0
#define TEMOD16_FEC_SUPRTD_NOT_REQSTD  1
#define TEMOD16_FEC_CL74_SUPRTD_REQSTD 4

typedef enum {
    TEMOD16_TX_LANE_RESET = 0,
    TEMOD16_TX_LANE_TRAFFIC_ENABLE = 1,
    TEMOD16_TX_LANE_TRAFFIC_DISABLE = 2,
    TEMOD16_TX_LANE_RESET_TRAFFIC_ENABLE = 3,
    TEMOD16_TX_LANE_RESET_TRAFFIC_DISABLE = 4,
    TEMOD16_TX_LANE_TYPE_COUNT
}tx_lane_disable_type_t;


typedef struct temod16_an_adv_ability_s{
  uint16_t an_base_speed;
  uint16_t an_bam_speed;
  uint16_t an_bam_speed1;
  uint16_t an_nxt_page;
  uint16_t an_pause;
  uint16_t an_fec;
  uint16_t an_cl72;
  uint16_t an_hg2;
  uint16_t cl37_sgmii_speed;
  temod16_an_mode_type_t an_type;
}temod16_an_adv_ability_t;

typedef struct temod16_an_ability_s {
    temod16_an_adv_ability_t cl37_adv;  /*this will include all the cl37 and cl37-bam related  (everything inlclduing fec, cl72 ) */
    temod16_an_adv_ability_t cl73_adv ;/*this will include all the cl73 and cl73-bam related */
} temod16_an_ability_t;

typedef struct temod16_an_control_s {
  temod16_an_mode_type_t an_type;
  uint16_t num_lane_adv;
  uint16_t enable;
  uint16_t pd_kx_en;
  uint16_t pd_kx4_en;
  an_property_enable  an_property_type;
} temod16_an_control_t;


typedef struct temod16_an_init_s{
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
} temod16_an_init_t;


typedef struct temod16_an_timers_s{
  uint16_t  value;
} temod16_an_timers_t;

typedef struct temod16_device_aux_modes_s {
    uint32_t core_id ;
    uint16_t st_current_entry ;
    uint16_t st_hcd[4] ;
    uint16_t st_pll_div[4] ;
    uint16_t st_os[4] ;

    uint16_t hto_enable[4]  ;
    uint16_t hto_pll_div[4] ;
    uint16_t hto_os[4] ;
} temod16_device_aux_modes_t ;

#define TEMOD16_CL72_CONTROL_NO_TRIGER  0x80
#define TEMOD16_CL72_CONTROL_MASK       0x0f

int temod16_get_mapped_speed(temod16_spd_intfc_type spd_intf, int *speed);
int temod16_get_pll_ndiv(int pll_mode, int *pll_ndiv);
int temod16_get_actual_speed(int speed_id, int *speed);

/* use temod16_spd_intfc_type instead from temod16_enum_defines.h  */
typedef struct sc_table_entry_s {
  int illegal_;
} sc_table_entry_t;


extern int temod16_pll_lock_wait(PHYMOD_ST* pc, int timeOutValue);
extern int temod16_credit_control(PHYMOD_ST* pc, int enable);
extern int temod16_power_control(PHYMOD_ST* pc);
extern int temod16_override_set(PHYMOD_ST* pc, override_type_t or_type, uint16_t or_val);
extern int temod16_credit_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit);
extern int temod16_rx_lane_control_set(PHYMOD_ST* pc, int enable);
extern int temod16_init_pmd(PHYMOD_ST* pc, int pmd_touched, int uc_active);
extern int temod16_pmd_osmode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, int os_mode);
extern int temod16_pll_lock_get(PHYMOD_ST* pc, int* lockStatus);
extern int temod16_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus);
extern int temod16_wait_pmd_lock(PHYMOD_ST* pc, int timeOutValue, int* lockStatus);
extern int temod16_pcs_bypass_ctl (PHYMOD_ST* pc, int cntl);
extern int temod16_pmd_reset_bypass (PHYMOD_ST* pc, int cntl);
extern int temod16_set_pll_mode(PHYMOD_ST* pc, int pmd_tched, temod16_spd_intfc_type sp, int pll_mode);
extern int temod16_tick_override_set(PHYMOD_ST* pc, int tick_override, int numerator, int denominator);
extern int temod16_tx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int temod16_tx_pmd_loopback_control(PHYMOD_ST* pc, int cntl);
extern int temod16_rx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int temod16_encode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, uint16_t enable);
extern int temod16_decode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, uint16_t enable);
extern int temod16_spd_intf_get(PHYMOD_ST* pc, int *spd_intf);
extern int temod16_revid_read(PHYMOD_ST* pc, uint32_t *revid);
extern int temod16_init_pcs(PHYMOD_ST* pc);
extern int temod16_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched);
extern int temod16_wait_sc_done(PHYMOD_ST* pc, uint16_t *data);
extern int temod16_get_pcs_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int temod16_disable_set(PHYMOD_ST* pc);
extern int temod16_init_pcs_ilkn(PHYMOD_ST* pc);
extern int temod16_pmd_x4_reset(PHYMOD_ST* pc);
extern int temod16_init_pmd_sw(PHYMOD_ST* pc, int pmd_touched, int uc_active,
                             temod16_spd_intfc_type spd_intf,  int t_pma_os_mode);
extern int temod16_fecmode_set(PHYMOD_ST* pc, int fec_enable);
extern int temod16_fecmode_get(PHYMOD_ST* pc, uint32_t* fec_enable);
extern int temod16_credit_override_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit);
extern int temod16_pmd_lane_reset(PHYMOD_ST* pc);
extern int temod16_cjpat_crpat_mode_set(PHYMOD_ST* pc, temod16_spd_intfc_type spd, int port, int pat, int pnum, int size, int ipgsz);
extern int temod16_cjpat_crpat_check(PHYMOD_ST* pc);
extern int temod16_cjpat_crpat_control(PHYMOD_ST* pc, int enable);
extern int temod16_pcs_lane_swap(PHYMOD_ST *pc, int lane_map);
extern int temod16_trigger_speed_change(PHYMOD_ST* pc);
extern int temod16_init_pmd_qsgmii(PHYMOD_ST* pc, int pmd_touched, int uc_active);
extern int temod16_rx_lane_control_get(PHYMOD_ST* pc, int *value);
extern int temod16_tx_loopback_get(PHYMOD_ST* pc, uint32_t *enable);
extern int temod16_update_port_mode(PHYMOD_ST* pc, int* pll_restart);
extern int temod16_tx_lane_control_set(PHYMOD_ST* pc, tx_lane_disable_type_t tx_dis_type);
extern int temod16_refclk_set(PHYMOD_ST* pc, phymod_ref_clk_t ref_clock);
extern int temod16_pcs_ilkn_mode_set(PHYMOD_ST* pc);
extern int temod16_master_port_num_set( PHYMOD_ST *pc,  int port_num);
extern int temod16_cl48_lfrfli_init(PHYMOD_ST* pc);
extern int temod16_mld_am_timers_set(PHYMOD_ST* pc);
extern int temod16_cl74_chng_default(PHYMOD_ST* pc);
extern int temod16_speed_id_get(PHYMOD_ST* pc, int *speed_id);
extern int temod16_autoneg_control_get(PHYMOD_ST* pc, temod16_an_control_t *an_control, int *an_complete);
extern int temod16_pll_config_get(const phymod_access_t *pa, temod16_pll_mode_type  *pll_mode);
extern int temod16_hg_enable_get(PHYMOD_ST* pc, int *enable);
extern int temod16_pcs_ilkn_chk(PHYMOD_ST* pc, int *ilkn_set);
extern int temod16_clause72_control(PHYMOD_ST* pc, int enable);
extern int temod16_cl37_high_vco_set(PHYMOD_ST* pc, int value);
extern int temod16_osdfe_on_lkup_get(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, uint32_t* osdfe_on);
extern int temod16_scrambling_dis_lkup_get(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, uint32_t* scrambling_dis);
extern int temod16_override_clear(PHYMOD_ST* pc, override_type_t or_type);
extern int temod16_set_spd_intf(PHYMOD_ST *pc, temod16_spd_intfc_type spd_intf, int no_trig);
extern int temod16_rx_squelch_set(PHYMOD_ST *pc, int enable);
extern int temod16_rx_squelch_get(PHYMOD_ST *pc, int *val);
extern int temod16_port_enable_set(PHYMOD_ST *pc, int enable);
extern int temod16_tx_squelch_set(PHYMOD_ST *pc, int enable);
extern int temod16_tx_squelch_get(PHYMOD_ST *pc, int *val);
extern int temod16_tx_lane_control_get(PHYMOD_ST* pc, int *reset, int *enable);
extern int temod16_pmd_lane_swap(PHYMOD_ST *pc);
extern int temod16_get_pcs_latched_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int temod16_pmd_x4_reset_get(PHYMOD_ST* pc, int *is_in_reset);
extern int temod16_pmd_osmode_get(PHYMOD_ST* pc, int *os_mode);
extern int temod16_plldiv_lkup_get(PHYMOD_ST* pc, temod16_spd_intfc_type spd_intf, phymod_ref_clk_t ref_clk,  uint32_t *plldiv, uint16_t *speed_vec);
extern int temod16_pll_config_set(const phymod_access_t *pa, temod16_pll_mode_type  pll_mode, phymod_ref_clk_t ref_clk);
extern int temod16_pll_reset_enable_set(PHYMOD_ST *pa , int enable);
extern int temod16_disable_get(PHYMOD_ST *pa, uint32_t *enable);
extern int temod16_eee_control_set(PHYMOD_ST* pc, uint32_t enable);
extern int temod16_eee_control_get(PHYMOD_ST* pc, uint32_t *enable);

#ifdef _SDK_TEMOD16_
extern int temod16_autoneg_set_init(PHYMOD_ST* pc, temod16_an_init_t *an_init_st);
extern int temod16_autoneg_control(PHYMOD_ST* pc, temod16_an_control_t *an_control);
extern int temod16_autoneg_timer_init(PHYMOD_ST* pc);
extern int temod16_autoneg_set(PHYMOD_ST* pc, temod16_an_ability_t *an_ability_st);
extern int temod16_set_an_port_mode(PHYMOD_ST* pc, int enable, int nl_encoded, int starting_lane, int single_port);
extern int temod16_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map_0, uint32_t tx_lane_map_1, uint32_t tx_lane_map_2, uint32_t tx_lane_map_3);
extern int temod16_tsc12_control(PHYMOD_ST* pc, int cl82_multi_pipe_mode, int cl82_mld_phys_map);
#endif

extern int temod16_autoneg_local_ability_get(PHYMOD_ST* pc, temod16_an_ability_t *an_ability_st);
extern int temod16_diag_disp(PHYMOD_ST *pc, const char* cmd_str);
extern int temod16_autoneg_remote_ability_get(PHYMOD_ST* pc, temod16_an_ability_t *an_ability_st);
extern int temod16_set_an_fail_count(PHYMOD_ST* pc, int count);

extern int temod16_synce_stg0_mode_set(PHYMOD_ST *pc, int mode);
extern int temod16_synce_stg0_mode_get(PHYMOD_ST *pc, int *mode);
extern int temod16_synce_stg1_mode_set(PHYMOD_ST *pc, int mode);
extern int temod16_synce_stg1_mode_get(PHYMOD_ST *pc, int *mode);
extern int temod16_synce_clk_ctrl_set(PHYMOD_ST *pc, uint32_t val);
extern int temod16_synce_clk_ctrl_get(PHYMOD_ST *pc, uint32_t *val);

#endif  /*  _temod16_H_ */

