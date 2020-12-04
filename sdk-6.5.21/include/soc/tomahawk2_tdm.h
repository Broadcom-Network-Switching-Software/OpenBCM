/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk2_tdm.h
 */

#ifndef _SOC_TOMAHAWK2_TDM_H_
#define _SOC_TOMAHAWK2_TDM_H_

#include <soc/esw/port.h>


/*** START SDK API COMMON CODE ***/

extern int _soc_tomahawk2_tdm_mmu_calendar_set(
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_idb_calendar_set(
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_oversub_group_set(
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_oversub_group_set_sel(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int set_idb, int set_mmu);


extern void _soc_tomahawk2_speed_to_ovs_class_mapping(int  unit,
                                                      int  speed,
                                                      int *ovs_class);

extern void soc_tomahawk2_get_hsp_info(int  unit,
                                        int  speed,
                                        int  is_idb,
                                        int  is_mmu,
                                        int *is_hsp,
                                        int *same_spacing);

extern void soc_tomahawk2_port_ratio_get(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int clport, int *mode, int prev_or_new);


extern int _soc_tomahawk2_tdm_pkt_shaper_calendar_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_idb_opportunistic_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state, int cpu_op_en,
    int lb_opp_en, int opp1_port_en, int opp2_port_en, int opp_ovr_sub_en);


extern int _soc_tomahawk2_tdm_mmu_opportunistic_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int cpu_op_en, int lb_opp_en, int opp1_port_en, int opp2_port_en,
    int opp_ovr_sub_en);


extern int _soc_tomahawk2_tdm_idb_hsp_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_mmu_hsp_set(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);


extern int _soc_tomahawk2_tdm_idb_ppe_credit_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int full_credit_threshold_0,
    int opp_credit_threshold_0,
    int full_credit_threshold_1,
    int opp_credit_threshold_1 );


extern int _soc_tomahawk2_tdm_calculation(
    int unit,
    soc_port_schedule_state_t *port_schedule_state);


extern void soc_tomahawk2_pipe_map_get(
    int     unit,
    soc_port_schedule_state_t *port_schedule_state, uint32 *pipe_map);


extern int soc_tomahawk2_tdm_init(
    int unit,
    soc_port_schedule_state_t *
    port_schedule_state);


extern int _soc_tomahawk2_tdm_idb_dpp_ctrl_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int credits);


extern void soc_tomahawk2_ovs_pipe_map_get(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    uint32 *ovs_pipe_map);


extern void soc_print_port_schedule_state(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern void soc_print_port_map(int unit, soc_port_map_type_t *port_map);


extern void soc_print_tdm_schedule_slice( int unit, 
    soc_tdm_schedule_t *tdm_schedule, int hpipe, int is_ing);


extern void soc_print_port_resource(int unit,
    soc_port_resource_t *port_resource, int entry_num);

extern void
soc_tomahawk2_port_schedule_speed_remap(
    int  unit,
    soc_port_schedule_state_t *port_schedule_state);

extern int
soc_tomahawk2_tdm_get_tdm_speed(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int log_port,
    int prev_or_new,
    int *speed,
    int *num_lanes);

/*** END SDK API COMMON CODE ***/

#endif	/* !_SOC_TOMAHAWK2_TDM_H_ */
