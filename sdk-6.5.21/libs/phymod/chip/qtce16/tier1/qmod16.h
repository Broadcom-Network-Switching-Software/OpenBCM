/*----------------------------------------------------------------------
 * 
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

#ifndef _QMOD16_H_
#define _QMOD16_H_

#ifndef _DV_TB_
#define _SDK_QMOD16_ 1
#endif

#ifdef _SDK_QMOD16_
#include <phymod/phymod.h>
#endif

#ifndef PHYMOD_ST  
#ifdef _SDK_QMOD16_
  #define PHYMOD_ST   const phymod_access_t
#else
  #define PHYMOD_ST  qmod16_st
#endif /* _SDK_QMOD16_ */
#endif /* PHYMOD_ST */

#ifdef _DV_TB_
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"
#endif

typedef enum {
    QMOD16_AN_MODE_NONE = 0,
    QMOD16_AN_MODE_CL37 = 1,
    QMOD16_AN_MODE_CL37BAM = 2,
    QMOD16_AN_MODE_SGMII = 4,
    QMOD16_AN_MODE_TYPE_COUNT
}qmod16_an_mode_type_t;

typedef enum {
    QMOD16_TX_LANE_RESET = 0,
    QMOD16_TX_LANE_TRAFFIC_ENABLE = 1,
    QMOD16_TX_LANE_TRAFFIC_DISABLE = 2,
    QMOD16_TX_LANE_RESET_TRAFFIC_ENABLE = 3,
    QMOD16_TX_LANE_RESET_TRAFFIC_DISABLE = 4,
    QMOD16_TX_LANE_TYPE_COUNT

}tx_lane_disable_type_t;


typedef struct qmod16_an_adv_ability_s{
  uint16_t an_base_speed; 
  uint16_t an_bam_speed; 
  uint16_t an_bam_speed1; 
  uint16_t an_nxt_page; 
  uint16_t an_pause;
  uint16_t an_duplex;
  uint16_t cl37_sgmii_speed;
  qmod16_an_mode_type_t an_type;
} qmod16_an_adv_ability_t;

#define QMOD16_AN_FULL_DUPLEX_SET   0x1
#define QMOD16_AN_HALF_DUPLEX_SET   0x2

typedef struct qmod16_an_ability_s {
    qmod16_an_adv_ability_t cl37_adv;  /*include cl37 and cl37-bam related */
} qmod16_an_ability_t;

typedef struct qmod16_an_control_s {
  qmod16_an_mode_type_t an_type; 
  uint16_t num_lane_adv; 
  uint16_t enable;
  uint16_t pd_kx_en;
  uint16_t pd_kx4_en;
  an_property_enable  an_property_type;
} qmod16_an_control_t;

typedef enum {
    QMOD16_CL37_SGMII_10M = 0,
    QMOD16_CL37_SGMII_100M,
    QMOD16_CL37_SGMII_1000M,
    QMOD16_CL37_SGMII_2500M = 4,
    QMOD16_CL37_SGMII_SPEED_COUNT
}qmod16_cl37_sgmii_speed_t;

typedef enum {
    QMOD16_CL37_BAM_2p5GBASE_X = 0,
    QMOD16_CL37_BAM_SPEED_COUNT
}qmod16_cl37_bam_speed_t;

typedef enum {
    QMOD16_NO_PAUSE = 0,
    QMOD16_SYMM_PAUSE,
    QMOD16_ASYM_PAUSE,
    QMOD16_ASYM_SYMM_PAUSE,
    QMOD16_AN_PAUSE_COUNT
}qmod16_an_pause_t;

typedef struct qmod16_an_init_s{
  uint16_t  an_fail_cnt;
  uint16_t  an_oui_ctrl; 
  uint16_t  linkfailtimer_dis; 
  uint16_t  linkfailtimerqua_en; 
  uint16_t  an_good_check_trap; 
  uint16_t  an_good_trap; 
  uint16_t  disable_rf_report; 
  uint16_t  cl37_bam_ovr1g_pgcnt; 
  uint16_t  base_selector;
} qmod16_an_init_t;


typedef struct qmod16_an_timers_s{
  uint16_t  value;
} qmod16_an_timers_t;

extern int qmod16_pll_lock_wait(PHYMOD_ST* pc, int timeOutValue);
extern int qmod16_credit_control(PHYMOD_ST* pc, int enable);
extern int qmod16_tx_lane_control(PHYMOD_ST* pc, int enable, tx_lane_disable_type_t tx_dis_type);
extern int qmod16_power_control(PHYMOD_ST* pc);
extern int qmod16_override_set(PHYMOD_ST* pc, override_type_t or_type, uint16_t or_val);
extern int qmod16_credit_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit);
extern int qmod16_rx_lane_control_set(PHYMOD_ST* pc, int enable);        
extern int qmod16_mld_am_timers_set(PHYMOD_ST* pc, int rx_am_timer_init,int tx_am_timer_init_val);
extern int qmod16_init_pmd(PHYMOD_ST* pc, int pmd_touched, int uc_active);
extern int qmod16_pmd_osmode_set(PHYMOD_ST* pc, qmod16_spd_intfc_type spd_intf, int os_mode);
extern int qmod16_pmd_osmode_get(PHYMOD_ST* pc, int* os_mode);
extern int qmod16_pll_lock_get(PHYMOD_ST* pc, int* lockStatus);
extern int qmod16_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus);
extern int qmod16_wait_pmd_lock(PHYMOD_ST* pc, int timeOutValue, int* lockStatus);
extern int qmod16_pcs_bypass_ctl (PHYMOD_ST* pc, int cntl);
extern int qmod16_pmd_reset_bypass (PHYMOD_ST* pc, int cntl);
extern int qmod16_set_pll_mode(PHYMOD_ST* pc, int pmd_tched, qmod16_spd_intfc_type sp, int pll_mode);
extern int qmod16_tick_override_set(PHYMOD_ST* pc, int tick_override, int numerator, int denominator);
extern int qmod16_tx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int qmod16_tx_pmd_loopback_control(PHYMOD_ST* pc, int cntl);
extern int qmod16_rx_loopback_control(PHYMOD_ST* pc, int enable, int starting_lane, int port_type);
extern int qmod16_rx_loopback_get(PHYMOD_ST* pc, uint32_t *enable);
extern int qmod16_spd_intf_get(PHYMOD_ST* pc, int *spd_intf);
extern int qmod16_revid_read(PHYMOD_ST* pc, uint32_t *revid);
extern int qmod16_init_pcs(PHYMOD_ST* pc);
extern int qmod16_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched);
extern int qmod16_wait_sc_done(PHYMOD_ST* pc);
extern int qmod16_get_pcs_link_status(PHYMOD_ST* pc, uint32_t sub_port, uint32_t *link);
extern int qmod16_disable_set(PHYMOD_ST* pc);
extern int qmod16_pmd_x4_reset(PHYMOD_ST* pc);
extern int qmod16_init_pmd_sw(PHYMOD_ST* pc, int pmd_touched, int uc_active,
                             qmod16_spd_intfc_type spd_intf,  int t_pma_os_mode);
extern int qmod16_credit_override_set(PHYMOD_ST* pc, credit_type_t credit_type, int userCredit);
extern int qmod16_pmd_lane_reset(PHYMOD_ST* pc);
extern int qmod16_pcs_lane_swap(PHYMOD_ST *pc, int lane_tx_map, int lane_rx_map);
extern int qmod16_pcs_lane_swap_get (PHYMOD_ST *pc,  uint32_t *tx_swap, uint32_t *rx_swap);
extern int qmod16_master_port_num_set(PHYMOD_ST *pa,  int port_num);
extern int qmod16_pmd_addr_lane_swap (PHYMOD_ST *pc, uint32_t addr_lane_index);
extern int qmod16_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map);
extern int qmod16_rx_lane_control_get(PHYMOD_ST* pc, int *value);
extern int qmod16_trigger_speed_change(PHYMOD_ST* pc);
extern int qmod16_pmd_override_control(PHYMOD_ST* pc, int cntl, int value);
extern int qmod16_tx_rx_polarity_set (PHYMOD_ST *pc, uint32_t txp, uint32_t rxp);
extern int qmod16_pmd_lane_swap_tx_get (PHYMOD_ST *pc, uint32_t *tx_lane_map);
extern int qmod16_pram_abl_enable_set(PHYMOD_ST* pc, int enable);
extern int qmod16_tx_rx_polarity_get (PHYMOD_ST *pc, uint32_t* txp, uint32_t* rxp);
extern int qmod16_clause72_control(PHYMOD_ST *pc, int cl72_en);
extern int qmod16_port_enable_set(PHYMOD_ST *pc, int enable);
extern int qmod16_update_port_mode(PHYMOD_ST *pa, int single_port_mode);
extern int qmod16_tx_squelch_set(PHYMOD_ST *pc, int enable);
extern int qmod16_rx_squelch_set(PHYMOD_ST *pc, int enable);
extern int qmod16_tx_squelch_get(PHYMOD_ST *pc, int *enable);
extern int qmod16_rx_squelch_get(PHYMOD_ST *pc, int *enable);
extern int qmod16_tx_lane_control_set(PHYMOD_ST* pc, tx_lane_disable_type_t tx_dis_type);
extern int qmod16_tx_lane_control_get(PHYMOD_ST* pc, int *reset, int *enable);
extern int qmod16_get_plldiv (PHYMOD_ST *pc,  uint32_t *pll_div);
extern int qmod16_plldiv_lkup_get(PHYMOD_ST* pc, qmod16_spd_intfc_type spd_intf, uint32_t *plldiv, uint16_t *speed_vec);
extern int qmod16_osdfe_on_lkup_get(PHYMOD_ST* pc, qmod16_spd_intfc_type spd_intf, uint32_t *osdfe_on);
extern int qmod16_pll_reset_enable_set (PHYMOD_ST *pa , int enable);
extern int qmod16_set_spd_intf(PHYMOD_ST *pc, qmod16_spd_intfc_type spd_intf, int no_trig);
extern int qmod16_speed_id_get (PHYMOD_ST *pc,  int *speed_id, int subport);
extern int qmod16_refclk_set(PHYMOD_ST* pc, qmod16_ref_clk_t refclk);
int qmod16_tx_loopback_get(PHYMOD_ST* pc, uint32_t *enable);      
extern int qmod16_autoneg_local_ability_get(PHYMOD_ST* pc, qmod16_an_ability_t *an_ability_st, int sub_port);
extern int qmod16_autoneg_remote_ability_get(PHYMOD_ST* pc, qmod16_an_ability_t *an_ability_st, int sub_port);
extern int qmod16_autoneg_resolution_ability_get(PHYMOD_ST* pc, qmod16_an_ability_t *an_ability_st, int sub_port);
extern int qmod16_autoneg_control_get(PHYMOD_ST* pc, int sub_port, qmod16_an_control_t *an_control, int *an_complete);
extern int qmod16_set_qport_spd(PHYMOD_ST* pc, int sub_port, qmod16_spd_intfc_type spd_intf);
#ifdef _SDK_QMOD16_
extern int qmod16_autoneg_set_init(PHYMOD_ST* pc, qmod16_an_init_t *an_init_st); 
extern int qmod16_autoneg_control(PHYMOD_ST* pc, qmod16_an_control_t *an_control, int sub_port); 
extern int qmod16_autoneg_timer_init(PHYMOD_ST* pc);
extern int qmod16_autoneg_set(PHYMOD_ST* pc, qmod16_an_ability_t *an_ability_st, int sub_port);
extern int qmod16_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane, int single_port);
extern int qmod16_pmd_lane_swap_tx(PHYMOD_ST *pc, uint32_t tx_lane_map);
extern int qmod16_lane_info(PHYMOD_ST* pc, int *lane, int *sub_port);
extern int qmod16_get_mapped_speed(qmod16_spd_intfc_type spd_intf, int *speed);
extern int qmod16_diag_disp(PHYMOD_ST *pc, const char* mystr);
extern int qmod16_reset(PHYMOD_ST* pc);
extern int qmod16_autoneg_cl37_base_abilities(PHYMOD_ST* pc, qmod16_an_ability_t *an_ability, int sub_port);
extern int qmod16_an_cl37_bam_abilities(PHYMOD_ST* pc, qmod16_an_ability_t *an_ability, int sub_port);
extern int qmod16_speedchange_get(PHYMOD_ST* pc, uint32_t *enable);
extern int qmod16_get_qport_spd(PHYMOD_ST* pc, int sub_port, qmod16_spd_intfc_type *spd_intf);
extern int qmod16_port_state_set(PHYMOD_ST* pc, int qmod16_state_type, int sub_port, int val);
extern int qmod16_port_state_get(PHYMOD_ST* pc, int qmod16_state_type, int *all_sub_port_val);
extern int qmod16_pmd_x4_reset_get(PHYMOD_ST* pc, int *value);
extern int qmod16_usgmii_shim_fifo_threshold_set(PHYMOD_ST* pc);
#endif

#define QMOD16_PORT_STATE_CONFIGED    1
#define QMOD16_PORT_STATE_TX_SQUELCH  2
#define QMOD16_PORT_STATE_RX_SQUELCH  3

int qmod16_eee_control_set(PHYMOD_ST* pc, uint32_t enable);
int qmod16_eee_control_get(PHYMOD_ST* pc, uint32_t *enable);

extern int qmod16_synce_mode_set(PHYMOD_ST *pc, int mode0, int mode1);
extern int qmod16_synce_mode_get(PHYMOD_ST *pc, int *mode0, int *mode1);
extern int qmod16_synce_clk_ctrl_set(PHYMOD_ST *pc, uint32_t val);
extern int qmod16_synce_clk_ctrl_get(PHYMOD_ST *pc, uint32_t *val);
extern int qmod16_tx_fsm_status_get(PHYMOD_ST *pc, int sub_port, int* fsm);

int get_mapped_speed(qmod16_spd_intfc_type spd_intf, int *speed);
int get_actual_speed(int speed_id, int *speed);

#endif  /*  _qmod16_H_ */

