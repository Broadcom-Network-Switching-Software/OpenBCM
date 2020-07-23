/*----------------------------------------------------------------------
 * $Id: qmod.h,
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

#ifndef _QMOD_H_
#define _QMOD_H_

#ifndef _DV_TB_
#define _SDK_QMOD_ 1
#endif

#ifdef _SDK_QMOD_
#include <phymod/phymod.h>
#endif

#ifndef PHYMOD_ST  
#ifdef _SDK_QMOD_
  #define PHYMOD_ST   const phymod_access_t
#else
  #define PHYMOD_ST  qmod_st
#endif /* _SDK_QMOD_ */
#endif /* PHYMOD_ST */

#ifdef _DV_TB_
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"
#endif

typedef enum {
    QMOD_AN_MODE_NONE = 0,
    QMOD_AN_MODE_CL37 = 1,
    QMOD_AN_MODE_CL37BAM = 2,
    QMOD_AN_MODE_SGMII = 4,
    QMOD_AN_MODE_TYPE_COUNT
}qmod_an_mode_type_t;

typedef enum {
    QMOD_TX_LANE_RESET = 0,
    QMOD_TX_LANE_TRAFFIC_ENABLE = 1,
    QMOD_TX_LANE_TRAFFIC_DISABLE = 2,
    QMOD_TX_LANE_RESET_TRAFFIC_ENABLE = 3,
    QMOD_TX_LANE_RESET_TRAFFIC_DISABLE = 4,
    QMOD_TX_LANE_TYPE_COUNT

}tx_lane_disable_type_t;


typedef struct qmod_an_adv_ability_s{
  uint16_t an_base_speed; 
  uint16_t an_bam_speed; 
  uint16_t an_bam_speed1; 
  uint16_t an_nxt_page; 
  uint16_t an_pause;
  uint16_t an_duplex;
  uint16_t cl37_sgmii_speed;
  qmod_an_mode_type_t an_type;
} qmod_an_adv_ability_t;

#define QMOD_AN_FULL_DUPLEX_SET   0x1
#define QMOD_AN_HALF_DUPLEX_SET   0x2

typedef struct qmod_an_ability_s {
    qmod_an_adv_ability_t cl37_adv;  /*include cl37 and cl37-bam related */
} qmod_an_ability_t;

typedef struct qmod_an_control_s {
  qmod_an_mode_type_t an_type; 
  uint16_t num_lane_adv; 
  uint16_t enable;
  uint16_t pd_kx_en;
  uint16_t pd_kx4_en;
  an_property_enable  an_property_type;
} qmod_an_control_t;

typedef enum {
    QMOD_CL37_SGMII_10M = 0,
    QMOD_CL37_SGMII_100M,
    QMOD_CL37_SGMII_1000M,
    QMOD_CL37_SGMII_SPEED_COUNT
}qmod_cl37_sgmii_speed_t;

typedef enum {
    QMOD_CL37_BAM_2p5GBASE_X = 0,
    QMOD_CL37_BAM_SPEED_COUNT
}qmod_cl37_bam_speed_t;

typedef enum {
    QMOD_NO_PAUSE = 0,
    QMOD_SYMM_PAUSE,
    QMOD_ASYM_PAUSE,
    QMOD_ASYM_SYMM_PAUSE,
    QMOD_AN_PAUSE_COUNT
}qmod_an_pause_t;

typedef struct qmod_an_init_s{
  uint16_t  an_fail_cnt;
  uint16_t  an_oui_ctrl; 
  uint16_t  linkfailtimer_dis; 
  uint16_t  linkfailtimerqua_en; 
  uint16_t  an_good_check_trap; 
  uint16_t  an_good_trap; 
  uint16_t  disable_rf_report; 
  uint16_t  cl37_bam_ovr1g_pgcnt; 
  uint16_t  base_selector;
} qmod_an_init_t;

typedef enum {
    QMOD_PMD_LANE_SIGNAL_DETECT_OVRD = 0,
    QMOD_PMD_LANE_RX_LOCK_OVRD,
    QMOD_PMD_LANE_OVRD_COUNT
}qmod_pmd_lane_override_t;


typedef struct qmod_an_timers_s{
  uint16_t  value;
} qmod_an_timers_t;

extern int qmod_pll_lock_wait(PHYMOD_ST* pc, int timeOutValue);
extern int qmod_credit_control(PHYMOD_ST* pc, int enable);
extern int qmod_tx_lane_control(PHYMOD_ST* pc, int enable, tx_lane_disable_type_t tx_dis_type);
extern int qmod_power_control(PHYMOD_ST* pc);
extern int qmod_override_set(PHYMOD_ST* pc, override_type_t or_type, uint16_t or_val);
extern int qmod_credit_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit);
extern int qmod_rx_lane_control_set(PHYMOD_ST* pc, int enable);        
extern int qmod_mld_am_timers_set(PHYMOD_ST* pc, int rx_am_timer_init,int tx_am_timer_init_val);
extern int qmod_init_pmd(PHYMOD_ST* pc, int pmd_touched, int uc_active);
extern int qmod_pmd_osmode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf, int os_mode);
extern int qmod_pmd_osmode_get(PHYMOD_ST* pc, int* os_mode);
extern int qmod_pll_lock_get(PHYMOD_ST* pc, int* lockStatus);
extern int qmod_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus);
extern int qmod_wait_pmd_lock(PHYMOD_ST* pc, int timeOutValue, int* lockStatus);
extern int qmod_pcs_bypass_ctl (PHYMOD_ST* pc, int cntl);
extern int qmod_pmd_reset_bypass (PHYMOD_ST* pc, int cntl);
extern int qmod_set_pll_mode(PHYMOD_ST* pc, int pmd_tched, qmod_spd_intfc_type sp, int pll_mode);
extern int qmod_tick_override_set(PHYMOD_ST* pc, int tick_override, int numerator, int denominator);
extern int qmod_tx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int qmod_tx_pmd_loopback_control(PHYMOD_ST* pc, int cntl);
extern int qmod_rx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int qmod_encode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf);
extern int qmod_encode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf);
extern int qmod_decode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf);
extern int qmod_decode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf);
extern int qmod_spd_intf_get(PHYMOD_ST* pc, int *spd_intf);
extern int qmod_revid_read(PHYMOD_ST* pc, uint32_t *revid);
extern int qmod_init_pcs(PHYMOD_ST* pc);
extern int qmod_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched);
extern int qmod_wait_sc_done(PHYMOD_ST* pc, uint16_t *data);
extern int qmod_get_pcs_link_status(PHYMOD_ST* pc, uint32_t sub_port, uint32_t *link);
extern int qmod_disable_set(PHYMOD_ST* pc);
extern int qmod_pmd_x4_reset(PHYMOD_ST* pc);
extern int qmod_init_pmd_sw(PHYMOD_ST* pc, int pmd_touched, int uc_active,
                             qmod_spd_intfc_type spd_intf,  int t_pma_os_mode);
extern int qmod_credit_override_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit);
extern int qmod_pmd_lane_reset(PHYMOD_ST* pc);
extern int qmod_cjpat_crpat_mode_set(PHYMOD_ST* pc, qmod_spd_intfc_type spd, int port, int pat, int pnum, int size, int ipgsz);
extern int qmod_cjpat_crpat_check(PHYMOD_ST* pc);
extern int qmod_cjpat_crpat_control(PHYMOD_ST* pc, int enable);
extern int qmod_pcs_lane_swap(PHYMOD_ST *pc, int lane_map);
extern int qmod_pcs_lane_swap_get (PHYMOD_ST *pc,  uint32_t *tx_rx_swap);
extern int qmod_master_port_num_set(PHYMOD_ST *pa,  int port_num);
extern int qmod_pmd_addr_lane_swap (PHYMOD_ST *pc, uint32_t addr_lane_index);
extern int qmod_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map);
extern int qmod_trigger_speed_change(PHYMOD_ST* pc);
extern int qmod_rx_lane_control_get(PHYMOD_ST* pc, int *value);
extern int qmod_pmd_override_control(PHYMOD_ST* pc, int cntl, int value);
extern int qmod_pll_sequencer_control(PHYMOD_ST *pa, int enable);
extern int qmod_tx_rx_polarity_set (PHYMOD_ST *pc, uint32_t txp, uint32_t rxp);
extern int qmod_pmd_lane_swap_tx_get (PHYMOD_ST *pc, uint32_t *tx_lane_map);
extern int qmod_pram_abl_enable_set(PHYMOD_ST* pc, int enable);
extern int qmod_tx_rx_polarity_get (PHYMOD_ST *pc, uint32_t* txp, uint32_t* rxp);
extern int qmod_port_enable_set(PHYMOD_ST *pc, int enable);
extern int qmod_tx_squelch_set(PHYMOD_ST *pc, int enable);
extern int qmod_rx_squelch_set(PHYMOD_ST *pc, int enable);
extern int qmod_tx_squelch_get(PHYMOD_ST *pc, int *enable);
extern int qmod_rx_squelch_get(PHYMOD_ST *pc, int *enable);
extern int qmod_tx_lane_control_set(PHYMOD_ST* pc, tx_lane_disable_type_t tx_dis_type);
extern int qmod_tx_lane_control_get(PHYMOD_ST* pc, int *reset, int *enable);
extern int qmod_get_plldiv (PHYMOD_ST *pc,  uint32_t *pll_div);
extern int qmod_update_port_mode( PHYMOD_ST *pa, int *pll_restart);
extern int qmod_plldiv_lkup_get(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf, uint32_t *plldiv, uint16_t *speed_vec);
extern int qmod_osdfe_on_lkup_get(PHYMOD_ST* pc, qmod_spd_intfc_type spd_intf, uint32_t *osdfe_on);
extern int qmod_pll_reset_enable_set (PHYMOD_ST *pa , int enable);
extern int qmod_set_spd_intf(PHYMOD_ST *pc, qmod_spd_intfc_type spd_intf, int no_trig);
extern int qmod_speed_id_get (PHYMOD_ST *pc,  int *speed_id, int subport);
extern int qmod_refclk_set(PHYMOD_ST* pc, qmod_ref_clk_t refclk);
int qmod_tx_loopback_get(PHYMOD_ST* pc, uint32_t *enable);      
extern int qmod_autoneg_local_ability_get(PHYMOD_ST* pc, qmod_an_ability_t *an_ability_st, int sub_port);
extern int qmod_autoneg_remote_ability_get(PHYMOD_ST* pc, qmod_an_ability_t *an_ability_st, int sub_port);
extern int qmod_autoneg_control_get(PHYMOD_ST* pc, int sub_port, qmod_an_control_t *an_control, int *an_complete);
extern int qmod_set_qport_spd(PHYMOD_ST* pc, int sub_port, qmod_spd_intfc_type spd_intf);
extern int qmod_pmd_lane_override(PHYMOD_ST* pc, qmod_pmd_lane_override_t ovrd_type);
#ifdef _SDK_QMOD_
extern int qmod_autoneg_set_init(PHYMOD_ST* pc, qmod_an_init_t *an_init_st); 
extern int qmod_autoneg_control(PHYMOD_ST* pc, qmod_an_control_t *an_control, int sub_port); 
extern int qmod_autoneg_timer_init(PHYMOD_ST* pc);
extern int qmod_autoneg_set(PHYMOD_ST* pc, qmod_an_ability_t *an_ability_st, int sub_port);
extern int qmod_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane, int single_port);
extern int qmod_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map);
extern int qmod_lane_info(PHYMOD_ST* pc, int *lane, int *sub_port);
extern int qmod_get_mapped_speed(qmod_spd_intfc_type spd_intf, int *speed);
extern int qmod_diag_disp(PHYMOD_ST *pc, const char* mystr);
extern int qmod_reset(PHYMOD_ST* pc);
extern int qmod_autoneg_cl37_base_abilities(PHYMOD_ST* pc, qmod_an_ability_t *an_ability, int sub_port);
extern int qmod_an_cl37_bam_abilities(PHYMOD_ST* pc, qmod_an_ability_t *an_ability, int sub_port);
extern int qmod_speedchange_get(PHYMOD_ST* pc, uint32_t *enable);
extern int qmod_get_qport_spd(PHYMOD_ST* pc, int sub_port, qmod_spd_intfc_type *spd_intf);
extern int qmod_port_state_set(PHYMOD_ST* pc, int qmod_state_type, int sub_port, int val);
extern int qmod_port_state_get(PHYMOD_ST* pc, int qmod_state_type, int *all_sub_port_val);
extern int qmod_pmd_x4_reset_get(PHYMOD_ST* pc, int *value);
#endif

#define QMOD_PORT_STATE_CONFIGED    1
#define QMOD_PORT_STATE_TX_SQUELCH  2
#define QMOD_PORT_STATE_RX_SQUELCH  3

#define QMOD_NOF_LANES_IN_CORE (4)

int qmod_eee_control_set(PHYMOD_ST* pc, uint32_t enable);
int qmod_eee_control_get(PHYMOD_ST* pc, uint32_t *enable);

extern int qmod_synce_mode_set(PHYMOD_ST *pc, int mode0, int mode1);
extern int qmod_synce_mode_get(PHYMOD_ST *pc, int *mode0, int *mode1);
extern int qmod_synce_clk_ctrl_set(PHYMOD_ST *pc, uint32_t val);
extern int qmod_synce_clk_ctrl_get(PHYMOD_ST *pc, uint32_t *val);

int get_mapped_speed(qmod_spd_intfc_type spd_intf, int *speed);
int get_actual_speed(int speed_id, int *speed);

#endif  /*  _qmod_H_ */

