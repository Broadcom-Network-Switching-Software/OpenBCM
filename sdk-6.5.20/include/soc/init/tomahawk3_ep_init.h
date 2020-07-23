/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_ep_init.h
 */


#ifndef _SOC_TOMAHAWK3_EP_INIT_H_
#define _SOC_TOMAHAWK3_EP_INIT_H_

#include <soc/init/tomahawk3_flexport_defines.h>
#include <soc/init/tomahawk3_set_tdm.h>

/*** START SDK API COMMON CODE ***/

#define TH3_EP_XMIT_XNT_NUM_FREQS  1

/* Transmit Start Count */
typedef struct soc_th3_edb_xmit_start_count_s {
    /* NOTE: DON'T ALTER FIELD ORDER */
    uint8 saf;       /* SAF value */
    uint8 ct;        /* CT val */
} soc_th3_edb_xmit_start_count_t;

/* EP2MMU and Prebuffer core config */
typedef struct soc_th3_ep_cfg_s {
    /* NOTE: DON'T ALTER FIELD ORDER */
    int speed;
    int ct_class;
    int egr_mmu_credit;         /* for general cases */
    int egr_mmu_credit_pfc_opt; /* for PFC optimized at 1325/1000 */
    soc_th3_edb_xmit_start_count_t xmit_cnt_per_freq[TH3_EP_XMIT_XNT_NUM_FREQS];
} soc_th3_ep_core_cfg_t;


extern int soc_tomahawk3_div_round(
    int nominator,
    int denominator);

extern int soc_tomahawk3_get_set_slot_pipeline_latency(
    int core_freq,
    int dpp_freq,
    int dppl,
    int A_in,
    int B_in,
    int N_in,
    int C_in,
    int D_in,
    int E_in,
    int *slot_latency);

extern int soc_tomahawk3_ep_set_slot_pipeline_config(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_ep_set_dev_to_phy(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down);

extern int soc_tomahawk3_ep_set_mmu_cell_credit(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down);

extern int soc_tomahawk3_get_speed_encoding(
    int speed,
    int *speed_encoding);

extern int soc_tomahawk3_get_ct_class(int speed);

extern int soc_tomahawk3_ep_get_ep2mmu_credit(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int phy_port);

extern int soc_tomahawk3_ep_get_ep2mmu_credit_per_speed(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int speed,
    int *credit);

extern int soc_tomahawk3_ep_set_ct_class(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down);

extern int soc_tomahawk3_ep_set_xmit_start_cnt(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down);

extern int soc_tomahawk3_ep_get_xmit_start_count(
    int  unit, soc_port_schedule_state_t *port_schedule_state,
    int  logical_port, int *xmit_start_cnt);

extern int soc_tomahawk3_ep_set_egr_enable(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down);

extern int soc_tomahawk3_ep_init_egr_enable(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_ep_edb_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_ep_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_ep_drain_port(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port);

extern int soc_tomahawk3_ep_sft_rst_port(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int rst_on);

extern int soc_tomahawk3_ep_port_down(int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_ep_port_up(int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_ep_reconfigure_remove(int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_flex_ep_reconfigure_add(int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_ep_credit_rst_port(
    int unit,
    int phy_port);

/*** END SDK API COMMON CODE ***/

#endif
