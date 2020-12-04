/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane4_ep_flexport.h
 * Purpose:
 * Requires:
 */

#ifndef HURRICANE4_EP_FLEXPORT_H
#define HURRICANE4_EP_FLEXPORT_H

/* Transmit Start Count */
typedef struct soc_hr4_edb_xmit_start_count_s {
    /* NOTE: DON'T ALTER FIELD ORDER */
    uint8 line_rate;       /* Line-rate */
    uint8 oversub_3_2;     /* 3:2 Oversub */
    uint8 oversub_2_1;     /* 2:1 Oversub */
} soc_hr4_edb_xmit_start_count_t;

/* EP2MMU and Prebuffer core config */
typedef struct soc_hr4_ep_cfg_s {
    /* NOTE: DON'T ALTER FIELD ORDER */
    int speed;
    int egr_mmu_credit_pmq_lr;                                  /* Baseline pmq */
    int egr_mmu_credit_pmq_os;                                  /* Baseline pmq */
    int egr_mmu_credit_egl_lr;                                  /* Baseline eagle */
    int egr_mmu_credit_egl_os;                                  /* Baseline eagle */
    int egr_mmu_credit_fl_lr;                                  /* Baseline falcon */
    int egr_mmu_credit_fl_os;                                  /* Baseline falcon */
    soc_hr4_edb_xmit_start_count_t xmit_cnt_l2_latency;  /* L2 Latency Mode */
    soc_hr4_edb_xmit_start_count_t xmit_cnt_l3_latency;  /* L3, Full Latency Modes */
    soc_hr4_edb_xmit_start_count_t xmit_cnt_full_latency;/* L3, Full Latency Modes */
} soc_hr4_ep_core_cfg_t;


extern int soc_hurricane4_flex_ep_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_hurricane4_flex_macsec_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_hurricane4_flex_macsec_port_up(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_hurricane4_flex_ep_reconfigure_remove(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_hurricane4_flex_ep_reconfigure_add(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_hurricane4_flex_ep_port_up(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_hurricane4_ep_display_xmit_cnt(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_hurricane4_ep_flexport_remove_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_hurricane4_ep_flexport_add_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_hurricane4_ep_flexport_enable_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state);


#endif
