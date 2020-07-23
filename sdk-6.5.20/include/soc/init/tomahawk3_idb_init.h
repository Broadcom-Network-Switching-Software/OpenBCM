/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       tomahawk3_idb_init.h
 */



#ifndef _SOC_TOMAHAWK3_IDB_INIT_H_
#define _SOC_TOMAHAWK3_IDB_INIT_H_

#include <soc/init/tomahawk3_flexport_defines.h>
#include <soc/init/tomahawk3_tdm.h>


/*** START SDK API COMMON CODE ***/

#define _TH3_OBM_PARAMS_LOSSY_INDEX                     0
#define _TH3_OBM_PARAMS_LOSSLESS_INDEX                  1
#define _TH3_OBM_PARAMS_LOSSY_PLUS_LOSSLESS_INDEX       2
#define _TH3_OBM_PARAMS_DROP_COUNT_SELECT_LOSSLESS0     2
#define _TH3_OBM_PARAMS_DROP_COUNT_SELECT_LOSSY_LO      0
#define _TH3_IDB_CA_PEEK_DEPTH_FRONT_PANEL_PORT         6
#define _TH3_IDB_CA_PEEK_DEPTH_CPU_LPBK_PORT            2

extern int soc_tomahawk3_get_pipe_from_phy_port(int pnum);

extern int soc_tomahawk3_get_pipe_pm_from_phy_port(int pnum);

extern int soc_tomahawk3_get_subp_from_phy_port(int pnum);

extern int soc_tomahawk3_phy_is_lpbk_port(int pnum);

extern int soc_tomahawk3_phy_is_cpu_mgmt_port(int pnum);

extern int soc_tomahawk3_idb_pa_reset(int unit,
    soc_port_map_type_t *port_map, int lport, int reset);

extern int soc_tomahawk3_idb_obm_reset(int unit,
    soc_port_map_type_t *port_map, int lport, int reset);

extern int soc_tomahawk3_idb_obm_rst_rel_all(int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_idb_ca_buffer_config(
    int unit, soc_port_map_type_t *port_map, int lport);

extern int soc_tomahawk3_idb_ca_wr_domain_reset(
    int unit, soc_port_map_type_t *port_map, int lport, int reset);

extern int soc_tomahawk3_idb_ca_rd_domain_reset(
    int unit, soc_port_map_type_t *port_map, int lport, int reset);

extern int soc_tomahawk3_idb_ca_wr_dom_rst_rel_all(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_idb_ca_rd_dom_rst_rel_all(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_idb_init_pnum_mapping_tbl(
    int unit, soc_port_map_type_t *port_map, int lport);

extern int soc_tomahawk3_idb_init_idb_to_dev_pmap_tbl(
    int unit, soc_port_map_type_t *port_map, int lport);

extern int soc_tomahawk3_idb_wr_obm_shared_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_wr_obm_thresh(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_wr_obm_flow_ctrl_cfg(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_wr_obm_fc_threshold(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_obm_pri_map_set(
    int unit, soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_obm_buffer_config(
    int unit, soc_port_map_type_t *port_map, int lport);

extern int soc_tomahawk3_idb_obm_tdm_cfg(
    int unit, soc_port_map_type_t *port_map, int lport);

extern int soc_tomahawk3_idb_outer_tpid_config(int unit,
    soc_port_map_type_t *port_map, int lport, int enable,
    int tag_num, int tag_value);

extern int soc_tomahawk3_en_epc_link_bmap(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_en_dest_port_bmap(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_enable_dpp_ctrl(int unit, int pipe);

extern int soc_tomahawk3_ip_slot_pipeline_config(
    int unit, soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_idb_wr_obm_ct_thresh(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_wr_ca_ct_thresh(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

int soc_tomahawk3_idb_ca_ct_enable(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport, int ct_enable);

int soc_tomahawk3_idb_ca_peek_depth_cfg(int unit,
    soc_port_map_type_t *port_map, int lport);

int soc_tomahawk3_idb_lpbk_ca_peek_depth_cfg(int unit, int pipe_num);

int soc_tomahawk3_idb_cpu_ca_peek_depth_cfg(int unit, int pipe_num);

int soc_tomahawk3_idb_wr_obm_port_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

int soc_tomahawk3_idb_wr_obm_monitor_stats_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_ca_bmop_set(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_obm_poll_buffer_empty(int unit,
    int pipe_num, int pm_num, int subp);

extern int soc_tomahawk3_idb_ca_poll_buffer_empty(int unit,
    int pipe_num, int pm_num, int subp);

extern int soc_tomahawk3_idb_lpbk_ca_in_reset(int unit, int pipe_num);

extern int soc_tomahawk3_idb_ca_lpbk_poll_buffer_empty(int unit, int pipe_num);

extern int soc_tomahawk3_idb_cpu_ca_in_reset(int unit, int pipe_num);

extern int soc_tomahawk3_idb_ca_cpu_poll_buffer_empty(int unit, int pipe_num);

extern int soc_tomahawk3_idb_lpbk_ca_reset_buffer(int unit,
       int pipe_num, int reset_buffer);

extern int soc_tomahawk3_idb_cpu_ca_reset_buffer(int unit,
       int pipe_num, int reset_buffer);

extern int soc_tomahawk3_idb_force_saf_duration_timer_cfg(int unit,
       soc_port_schedule_state_t *port_schedule_state, int pipe_num);

extern int soc_tomahawk3_speed_to_e2e_settings_class_map(int speed);

extern int soc_tomahawk3_idb_wr_obm_force_saf_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_wr_obm_lag_detection_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_wr_ca_force_saf_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_wr_ca_lag_detection_config(int unit,
    soc_port_schedule_state_t *port_schedule_state, int lport);

extern int soc_tomahawk3_idb_init(int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_ipipe_init(int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_idb_port_down(int unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int soc_tomahawk3_idb_port_up(int unit,
    soc_port_schedule_state_t *port_schedule_state);

/*** END SDK API COMMON CODE ***/

#endif


