/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/cosq/scheduler_port.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_PORT_H_INCLUDED_
#define _DNX_SCHEDULER_PORT_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>

/**
 * \brief - Init Scheduler port TC/TCG calendar configurations
 */
shr_error_e dnx_scheduler_port_shaper_calendar_init(
    int unit);

/**
 * \brief -
 *  set maximal credit rate of Port TC
 */
shr_error_e dnx_scheduler_port_priority_shaper_rate_set(
    int unit,
    bcm_port_t logical_port,
    uint32 priority_ndx,
    uint32 rate);

/**
 * \brief -
 *  get maximal credit rate of port TC
 */
shr_error_e dnx_scheduler_port_priority_shaper_rate_get(
    int unit,
    bcm_port_t logical_port,
    int priority_ndx,
    uint32 *rate);

/**
 * \brief -
 *  set maximal credit rate of TCG
 */
shr_error_e dnx_scheduler_port_tcg_shaper_rate_set(
    int unit,
    bcm_port_t logical_port,
    int tcg_ndx,
    uint32 rate);

/**
 * \brief -
 *  get maximal credit rate of TCG
 */
shr_error_e dnx_scheduler_port_tcg_shaper_rate_get(
    int unit,
    bcm_port_t logical_port,
    int tcg_ndx,
    uint32 *rate);

/**
 * \brief -
 *  set max burst of port TC
 */
shr_error_e dnx_scheduler_port_priority_max_burst_set(
    int unit,
    bcm_port_t logical_port,
    uint32 port_priority,
    uint32 max_burst);

/**
 * \brief -
 *  get max burst of port TC
 */
shr_error_e dnx_scheduler_port_priority_max_burst_get(
    int unit,
    bcm_port_t logical_port,
    uint32 port_priority,
    uint32 *max_burst);

/**
 * \brief -
 *  set max burst of port TCG
 */
shr_error_e dnx_scheduler_port_tcg_max_burst_set(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_index,
    uint32 max_burst);

/**
 * \brief -
 *  get max burst of port TCG
 */
shr_error_e dnx_scheduler_port_tcg_max_burst_get(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_index,
    uint32 *max_burst);

/**
 * \brief -
 *  reset max burst of port TCG to default value
 */
shr_error_e dnx_scheduler_port_tcg_max_burst_reset(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_index);

/**
 * \brief -
 *  enable/disable port scheduler
 */
shr_error_e dnx_scheduler_hr_port_enable_set(
    int unit,
    int core,
    int base_hr,
    int nof_priorities,
    int enable);

/**
 * \brief -
 *  enable/disable port scheduler
 */
shr_error_e dnx_scheduler_port_enable_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief -
 * enable/disable interface of the provided port
 */
int dnx_scheduler_port_interface_enable_set(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief -
 * Color all HRs of the provided port with given color group
 */
int dnx_scheduler_port_hr_color_group_set(
    int unit,
    bcm_port_t logical_port,
    int priority,
    int group);

/**
 * \brief -
 * Get HR color group
 */
int dnx_scheduler_port_hr_color_group_get(
    int unit,
    bcm_port_t logical_port,
    int priority,
    int *group);

/**
 * \brief -
 * Color all HRs of the provided port 
 * with the color which currently has lowest bandwidth.
 * max interface speed is considered as port bandwidth
 */
int dnx_scheduler_port_hr_color_decide_and_set(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * Uncolor all HRs of the provided port 
 */
int dnx_scheduler_port_hr_color_unset(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * configure number of port priorities for the provided port
 */
int dnx_scheduler_port_priorities_nof_set(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * clear number of port priorities for the provided port
 */
int dnx_scheduler_port_priorities_nof_clear(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * configure number of priority propagation priorities for the provided port
 */
int dnx_scheduler_port_priority_propagation_priorities_nof_set(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * clear number of priority propagation priorities for the provided port
 */
int dnx_scheduler_port_priority_propagation_priorities_nof_clear(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * configure priority/ies for the interface(s) of the provided port
 */
int dnx_scheduler_port_interface_priority_set(
    int unit,
    bcm_port_t port);
/**
 * \brief -
 * clear priority/ies for the interface(s) of the provided port
 */
int dnx_scheduler_port_interface_priority_clear(
    int unit,
    bcm_port_t port);

/**
 * \brief -
 * map between SCH interface(s) and EGQ interfaces for the provided port
 */
int dnx_scheduler_port_egq_to_sch_if_map(
    int unit,
    bcm_port_t port);

/**
 * \brief -
 * unmap between SCH interface(s) and EGQ interfaces for the provided port
 */
int dnx_scheduler_port_egq_to_sch_if_unmap(
    int unit,
    bcm_port_t port);

/**
 * \brief -
 * disable interface level FC from EGQ to SCH for the provided port
 */
int dnx_scheduler_port_egq_to_sch_if_fc_disable(
    int unit,
    bcm_port_t port);

/**
 * \brief -
 * map all port HRs to TCG 0 as the default
 */
int dnx_scheduler_port_tc_to_tcg_map_init(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * Set TCG bandwidth of 1 or 2 priority port to maximum.
 * TC bandwidth for all ports and TCG bandwidth for 4 and 8 priorities ports are set in tune
 */
int dnx_scheduler_port_shapers_bandwidth_init(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * Set TCG bandwidth of 1 or 2 priority port to default value.
 */
int dnx_scheduler_port_shapers_bandwidth_reset(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * Set max burst of TC and TCG shapers of the port to default value
 */
int dnx_scheduler_port_shapers_max_burst_reset(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief -
 * allocate and configure sch interace for a port
 */
int dnx_scheduler_interface_allocate(
    int unit,
    bcm_port_t port);

/**
 * \brief -
 * deallocate and configure sch interace for a port
 */
int dnx_scheduler_interface_deallocate(
    int unit,
    soc_port_t port);

/**
 * \brief - get port rate from scheduler interface calendar
 */
int dnx_scheduler_port_rate_hw_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *rate);

/**
 * \brief - Set port rate to swstate for scheduler interface calendar
 */
int dnx_scheduler_port_rate_sw_set(
    int unit,
    bcm_port_t logical_port,
    uint32 rate);

/**
 * \brief - configure all modified scheduler interface calendars
 */
int dnx_scheduler_port_rate_hw_set(
    int unit);

/**
 * \brief -
 * Set interface shaper bandwidth
 */
int dnx_scheduler_interface_shaper_rate_set(
    int unit,
    bcm_port_t logical_port,
    uint32 rate);

/**
 * \brief -
 * Get interface shaper bandwidth
 */
int dnx_scheduler_interface_shaper_rate_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *if_rate);

/**
 * \brief - map port to interface for non channelized calendar
 */
int dnx_scheduler_port_to_interface_map_set(
    int unit,
    bcm_port_t port);

/**
 * \brief - unmap port to interface for non channelized calendar
 */
int dnx_scheduler_port_to_interface_unmap_set(
    int unit,
    bcm_port_t port);

/**
 * \brief Function to set the EIR weight of a logical port TCG
 *  
 * @param unit [in] - unit
 * @param logical_port - logical_port 
 * @param cosq [in] - cosq
 * @param is_valid [in] - is valid
 * @param weight [in] - EIR weight to set
 *  
 * \note  When setting TCG weight FC must be forced on the port 
 *        in order for the change to take effect
 * @return int 
 */
int dnx_scheduler_port_tcg_weight_set(
    int unit,
    bcm_port_t logical_port,
    bcm_cos_queue_t cosq,
    int is_valid,
    int weight);

/**
 * \brief Function to get the EIR weight of a logical port TCG
 *  
 * @param unit [in] - unit
 * @param logical_port [in]- logical_port 
 * @param cosq [in] - cosq
 * @param is_valid [out] - is valid
 * @param weight [out] - EIR weight
 * 
 * @return int 
 */
int dnx_scheduler_port_tcg_weight_get(
    int unit,
    bcm_port_t logical_port,
    bcm_cos_queue_t cosq,
    int *is_valid,
    int *weight);

/**
 * \brief -
 * Get maximal value of max burst for TC / TCG shapers of the port
 */
int dnx_scheduler_port_shaper_maximal_max_burst_get(
    int unit,
    int is_tcg,
    uint32 *max_burst);

/**
 * \brief -
 * Set interface calendar for port priority propagation.
 * The calendar contains one slot for each HR 
 * and has maximal traversal rate
 */
int dnx_scheduler_interface_priority_propagation_calendar_set(
    int unit,
    bcm_port_t port,
    int is_remove);

#endif /* _DNX_SCHEDULER_PORT_H_INCLUDED_ */
