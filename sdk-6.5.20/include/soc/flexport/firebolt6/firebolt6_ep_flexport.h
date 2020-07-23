/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef FIREBOLT6_EP_FLEXPORT_H
#define FIREBOLT6_EP_FLEXPORT_H

/* Transmit Start Count */
typedef struct soc_fb6_edb_xmit_start_count_s {
    /* NOTE: DON'T ALTER FIELD ORDER */
    uint8 line_rate;       /* Line-rate */
    uint8 oversub_3_2;     /* 3:2 Oversub */
    uint8 oversub_2_1;     /* 2:1 Oversub */
} soc_fb6_edb_xmit_start_count_t;

/* EP2MMU and Prebuffer core config */
typedef struct soc_fb6_ep_cfg_s {
    /* NOTE: DON'T ALTER FIELD ORDER */
    int speed;
    int egr_mmu_credit_pmq_lr;                                  /* Baseline pmq */
    int egr_mmu_credit_pmq_os;                                  /* Baseline pmq */
    int egr_mmu_credit_egl_lr;                                  /* Baseline eagle */
    int egr_mmu_credit_egl_os;                                  /* Baseline eagle */
    int egr_mmu_credit_fl_lr;                                  /* Baseline falcon */
    int egr_mmu_credit_fl_os;                                  /* Baseline falcon */
    int egr_mmu_credit_fl_lr_1012;                             /* 1012 LR Falcon */
    int egr_mmu_credit_fl_os_1012;                             /* 1012 OS Falcon */
    int egr_mmu_credit_fl_lr_1250;                             /* 1250 LR Falcon */
    int egr_mmu_credit_fl_os_1250;                             /* 1250 OS Falcon */
    soc_fb6_edb_xmit_start_count_t xmit_cnt_l2_latency;  /* L2 Latency Mode */
    soc_fb6_edb_xmit_start_count_t xmit_cnt_l3_latency;  /* L3, Full Latency Modes */
    soc_fb6_edb_xmit_start_count_t xmit_cnt_full_latency;/* L3, Full Latency Modes */
} soc_fb6_ep_core_cfg_t;


extern int soc_firebolt6_flex_ep_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_firebolt6_flex_ep_reconfigure_remove(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_firebolt6_flex_ep_reconfigure_add(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_firebolt6_flex_ep_port_up(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_firebolt6_ep_display_xmit_cnt(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_firebolt6_ep_flexport_remove_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_firebolt6_ep_flexport_add_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int
soc_firebolt6_ep_flexport_enable_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state);


#endif
