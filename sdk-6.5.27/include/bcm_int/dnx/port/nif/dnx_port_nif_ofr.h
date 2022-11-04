/**
 *
 * \file dnx_port_nif_ofr.h
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * NIF OFR procedures for DNX.
 *
 */

#ifndef _NIF_OFR_H_
#define _NIF_OFR_H_

#include <bcm/types.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_enum_field_types.h>

#define DNX_PORT_NIF_OFR_DEFAULT_PRD_PROFILE  (0)

shr_error_e dnx_port_ofr_init(
    int unit);

shr_error_e dnx_port_ofr_power_down(
    int unit);

shr_error_e dnx_port_ofr_port_add(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_port_priority_hw_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_port_priority_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_port_remove(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_link_list_switch_prepare(
    int unit,
    bcm_port_t logical_port,
    uint8 **alloc_info);

shr_error_e dnx_port_ofr_link_list_switch(
    int unit,
    bcm_port_t logical_port,
    uint8 **alloc_info);

shr_error_e dnx_port_ofr_rx_mem_link_list_set(
    int unit,
    bcm_port_t port,
    int enable);

shr_error_e dnx_port_ofr_rx_sch_context_set(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_port_enable(
    int unit,
    bcm_port_t port,
    int enable);

shr_error_e dnx_port_ofr_rx_sch_context_enable(
    int unit,
    bcm_port_t port,
    int enable);

shr_error_e dnx_port_ofr_ilkn_rx_sch_context_enable(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t priority,
    int enable);

shr_error_e dnx_port_ofr_ilkn_rx_config_hw_set(
    int unit,
    bcm_port_t port,
    int is_tdm_priority_enabled,
    int is_high_priority_enabled,
    int is_low_priority_enabled);

shr_error_e dnx_port_ofr_ilkn_rx_config_hw_get(
    int unit,
    int ilkn_id,
    uint32 *is_tdm_priority_enabled,
    uint32 *is_high_priority_enabled,
    uint32 *is_low_priority_enabled);

shr_error_e dnx_port_ofr_ilkn_rx_counter_reset_set(
    int unit,
    uint32 ilkn_id,
    int is_tdm,
    uint32 in_reset);

shr_error_e dnx_port_ofr_double_link_list_calc(
    int unit,
    bcm_port_t port,
    int nof_sections,
    uint8 **alloc_info);

shr_error_e dnx_port_ofr_rmc_machine_id_get(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int priority_group,
    int *priority_id);

shr_error_e dnx_port_ofr_rx_fifo_status_counter_map(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_rx_fifo_status_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *mapped);

shr_error_e dnx_port_ofr_tx_fifo_status_counter_map(
    int unit,
    int counter,
    bcm_port_t port);

shr_error_e dnx_port_ofr_tx_fifo_status_get(
    int unit,
    bcm_port_t port,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *pm_credits,
    uint32 *mapped);

shr_error_e dnx_port_ofr_preemption_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable);

shr_error_e dnx_port_ofr_preemption_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);

/**
 * \brief - Determine TDM RMC priority (either high or low).
 * * For TDM hybrid ports the TDM priority is determine according to the 'other' priority
 * * For TDM only ports use high priority as this is the only RMC used by the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [out] is_over_high - Is TDM over high RMC (otherwise LOW).
 *
 */
shr_error_e dnx_port_ofr_tdm_is_over_high_get(
    int unit,
    bcm_port_t port,
    int *is_over_high);

#endif /* _NIF_OFR_H_ */
