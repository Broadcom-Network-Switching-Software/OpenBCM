/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk2_flexport.h
*/


#ifndef SOC_TOMAHAWK2_FLEXPORT_H
#define SOC_TOMAHAWK2_FLEXPORT_H


/*! @file tomahawk2_flexport.h
 *   @brief FlexPort functions and structures for Tomahawk2.
 *   Details are shown below.
 */

#include <soc/flexport/flexport_common.h>
#include <soc/flexport/tomahawk2_tdm_flexport.h>
#include <soc/flexport/tomahawk2_mmu_flex_init.h>
#include <soc/flexport/tomahawk2_idb_flexport.h>
#include <soc/flexport/tomahawk2_ep_flexport.h>

#include <soc/flexport/tomahawk2_mac_flexport.h>

/*! @struct soc_tomahawk2_flex_scratch_t
 *   @brief Scratch pad locals. Caller must not allocate. Callee allocates
 *          at entry and deallocates before return. This area is pointed
 *          to by the cookie in soc_port_schedule_state_t struct type.
 */
typedef struct soc_tomahawk2_flex_scratch_s {
    /*!< Previous OVS tables needed by the OVS consolidation sequence */
    soc_tdm_schedule_pipe_t prev_tdm_ingress_schedule_pipe[SOC_MAX_NUM_PIPES];
    soc_tdm_schedule_pipe_t prev_tdm_egress_schedule_pipe[SOC_MAX_NUM_PIPES];
    /*!< Current state of OVS tables; is changing during flexport sequence
     *   initial state is prev OVS tables and after OVS flexport sequence
     *   converges to new_ OVS tables*/
    soc_tdm_schedule_pipe_t curr_tdm_ingress_schedule_pipe[SOC_MAX_NUM_PIPES];
    soc_tdm_schedule_pipe_t curr_tdm_egress_schedule_pipe[SOC_MAX_NUM_PIPES];
    /*!< Previous and new Oversub ratios; per PIPE and per HALF PIPE
     * These are coded as follows:
     *  10  - this is LR;
     *  15  - this is OVS with 3:2;         ovs_ratio <= 1.5;
     *  20  - this is OVS with 2:1;  1.5 <  ovs_ratio <= 2.0;
     */
    int prev_pipe_ovs_ratio[SOC_MAX_NUM_PIPES]; /* per PIPE ovs_ratio
                                                 * per Half PIPE ovs_ratio */
    int prev_hpipe_ovs_ratio[SOC_MAX_NUM_PIPES][_TH2_OVS_HPIPE_COUNT_PER_PIPE];
    int new_pipe_ovs_ratio[SOC_MAX_NUM_PIPES]; /* per PIPE ovs_ratio
                                                * per Half PIPE ovs_ratio */
    int new_hpipe_ovs_ratio[SOC_MAX_NUM_PIPES][_TH2_OVS_HPIPE_COUNT_PER_PIPE];

    /* This keeps the exact bitrate of the out_port_map.log_port_speed[] */
    enum port_speed_e exact_out_log_port_speed[SOC_MAX_NUM_PORTS];
} soc_tomahawk2_flex_scratch_t;


extern void *soc_tomahawk2_port_lane_info_alloc (void);


extern int soc_tomahawk2_set_port_lane_info (
    int         unit,
    const void *lane_info_ptr,
    int         port_idx,
    int         pgw,
    int         xlp
    );


extern int soc_tomahawk2_get_port_lane_info (
    int         unit,
    const void *lane_info_ptr,
    int *       port_idx,
    int *       pgw,
    int *       xlp
    );


extern int soc_tomahawk2_reconfigure_ports (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk2_flex_start (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk2_flex_end (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk2_flex_top_port_down (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );


extern int soc_tomahawk2_flex_top_port_up (
    int                        unit,
    soc_port_schedule_state_t *port_schedule_state
    );

extern int soc_tomahawk2_get_ct_class(int speed);

extern int (*flexport_phases[MAX_FLEX_PHASES]) (int unit,
                                                soc_port_schedule_state_t
                                                *port_schedule_state);

#endif /* SOC_TOMAHAWK2_FLEXPORT_H */
