/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_tdm.h
 */


#ifndef _SOC_TOMAHAWK3_TDM_H_
#define _SOC_TOMAHAWK3_TDM_H_

#include <soc/esw/port.h>
#include <soc/init/tomahawk3_flexport_defines.h>
#include <soc/init/tomahawk3_set_tdm.h>
#include <soc/init/tomahawk3_ep_init.h>

/*** START SDK API COMMON CODE ***/

typedef enum {
    TDM_IDB_PORT_SCHED,
    TDM_IDB_PKT_SCHED,
    TDM_MMU_MAIN_SCHED,
    TDM_MMU_EB_PORT_SCHED,
    TDM_MMU_EB_PKT_SCHED
} tdm_sched_inst_name_e;


extern int soc_tomahawk3_tdm_set_cal_config(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe,
    int phy_port,
    int is_port_down);

extern int soc_tomahawk3_tdm_clear_edb_credit_counter_per_port(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num,
    int phy_port,
    int is_port_down);

extern int soc_tomahawk3_tdm_clear_edb_credit_counter(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_tdm_set_out_port_map(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_tdm_idb_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_tdm_mmu_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern void soc_tomahawk3_get_pipe_map(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    uint32 *pipe_map);

extern int soc_tomahawk3_tdm_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_tdm_sched_flexport(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int ports_down);

extern int soc_tomahawk3_tdm_idb_flexport(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_tdm_mmu_flexport(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);

/*** END SDK API COMMON CODE ***/

#endif
