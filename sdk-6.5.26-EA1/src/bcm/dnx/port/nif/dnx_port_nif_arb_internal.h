/** \file dnx_port_nif_arb_internal.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _PORT_NIF_ARB_INTERNAL_H_
#define _PORT_NIF_ARB_INTERNAL_H_

#include <bcm/types.h>
#include <soc/sand/shrextend/shrextend_error.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_arb.h>

typedef enum
{
    DNX_PORT_ARB_MAP_DESTINATION_MACSEC_0 = 0,

    DNX_PORT_ARB_MAP_DESTINATION_MACSEC_1 = 1,

    DNX_PORT_ARB_MAP_DESTINATION_BYPASS = 2,

    DNX_PORT_ARB_MAP_DESTINATION_NOF
} dnx_port_map_destination_e;

/**
 * \brief - Return the Corresponding Tx context (eth or tmac)
 *
 */
shr_error_e dnx_port_arb_tx_context_get(
    int unit,
    int logical_port,
    int *arb_context);

/**
 * \brief - Configure stop data for given logical port
 *
 */
shr_error_e dnx_port_arb_stop_data_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief - Configure disable Datapath for given logical port
 *
 */
shr_error_e dnx_port_arb_disable_datapath_set(
    int unit,
    bcm_port_t logical_port,
    int disable_datapath);
/**
 * \brief - Configure power down for given logical port
 *
 */
shr_error_e dnx_port_arb_global_powerdown_set(
    int unit,
    int enable);

/**
 * \brief - Configure PM power down for given CDU
 *
 */
shr_error_e dnx_port_arb_pm_powerdown_set(
    int unit,
    int cdu_id,
    int enable);

/**
 * \brief - Configure power down for arbiter tmac
 *
 */
shr_error_e dnx_port_arb_tmac_powerdown_set(
    int unit,
    int enable);

/**
 * \brief - Configure mapping destination for given logical port
 *
 */
shr_error_e dnx_port_arb_map_destination_set(
    int unit,
    bcm_port_t logical_port,
    dnx_port_map_destination_e mapping_type);

/**
 * \brief - Set arbiter rx pm minimal packet size to default
 *
 */
shr_error_e dnx_port_arb_rx_pm_default_min_pkt_size_set(
    int unit);

/**
 * \brief - Read mapping destination from hw for given logical port
 *
 */
shr_error_e dnx_port_arb_map_destination_get(
    int unit,
    bcm_port_t logical_port,
    dnx_port_map_destination_e * mapping_type);

/**
 * \brief - Configure mac mode config for given logical port
 *
 */
shr_error_e dnx_port_arb_port_mac_mode_config_set(
    int unit,
    bcm_port_t logical_port,
    uint32 mac_mode);

/**
 * \brief - Configure port reset for given tmac logical port
 *
 */
shr_error_e dnx_port_arb_tmac_port_reset(
    int unit,
    bcm_port_t logical_port,
    uint32 direction,
    int enable);

/**
 * \brief - Calculate msw to tmac credits and set to hw
 *
 */
shr_error_e dnx_port_arb_tmac_credits_calc_and_set(
    int unit,
    bcm_port_t logical_port,
    int speed);

/**
 * \brief -  credits init in HW
 *
 */
shr_error_e dnx_port_arb_credits_init_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief - Enable sif for given instance id
 *
 */
shr_error_e dnx_port_arb_sif_enable_set(
    int unit,
    int sif_port_id,
    int enable);

/**
 * \brief - Configure sif port's offset for given instance id
 *
 */
shr_error_e dnx_port_arb_sif_port_set(
    int unit,
    int sif_port_id,
    int enable);

/**
 * \brief - Configure sif port reset for given instance id
 *
 */
shr_error_e dnx_port_arb_sif_reset_set(
    int unit,
    int sif_port_id,
    int enable);

/**
 * \brief - Calculate the required sections for a link list with a given speed
 *
 */
shr_error_e dnx_port_arb_link_list_sections_required_get(
    int unit,
    int speed,
    int *nof_sections);

/**
 * \brief - Calculate the new mac mode for a given logical port
 *
 */
shr_error_e dnx_port_arb_mac_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *mac_mode);

/**
 * \brief - Count the number of mac ports for given logical port
 *
 */
shr_error_e dnx_port_arb_mac_mode_config_nof_ports_get(
    int unit,
    bcm_port_t logical_port,
    int *mac_mode_config_nof_ports);

/**
 * \brief - Set the credits value sent from specific logical port to ARB
 *
 */
shr_error_e dnx_port_arb_pm_credits_hw_set(
    int unit,
    bcm_port_t logical_port,
    int credits_val);

/**
 * \brief - Enable OFT dynamic memories
 *
 */
shr_error_e dnx_port_arb_enable_dynamic_memories_set(
    int unit,
    int enable);

/**
 * \brief - Return Rx calendars to be set for a given logical port
 *
 */
shr_error_e dnx_port_arb_rx_calendar_ids_get(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t * calendars_to_be_set);

/**
 * \brief - Return Tx calendars to be set for a given logical port
 *
 */
shr_error_e dnx_port_arb_tx_calendar_ids_get(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t * calendars_to_be_set);

/**
 * \brief - Init all the arbiter calendars
 *
 */
shr_error_e dnx_port_arb_init_all_calendars(
    int unit);
/**
 * \brief - Configure Tx bypass fifo reset
 *
 */
shr_error_e dnx_port_arb_tx_bypass_fifo_reset_set(
    int unit,
    int in_reset);

/**
 * \brief - Set tx start threshold
 *
 */
shr_error_e dnx_port_arb_tmac_start_tx_threshold_hw_set(
    int unit,
    bcm_port_t logical_port,
    int threshold_value);

#endif /* _PORT_NIF_ARB_INTERNAL_H_ */
