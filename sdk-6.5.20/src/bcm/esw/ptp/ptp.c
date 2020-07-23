/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ptp.c
 *
 * Purpose: 
 *
 * Functions:
 *      bcm_esw_ptp_acceptable_master_add
 *      bcm_esw_ptp_acceptable_master_enabled_get
 *      bcm_esw_ptp_acceptable_master_enabled_set
 *      bcm_esw_ptp_acceptable_master_list
 *      bcm_esw_ptp_acceptable_master_remove
 *      bcm_esw_ptp_acceptable_master_table_clear
 *      bcm_esw_ptp_acceptable_master_table_size_get
 *      bcm_esw_ptp_cb_register
 *      bcm_esw_ptp_cb_unregister
 *      bcm_esw_ptp_clock_accuracy_get
 *      bcm_esw_ptp_clock_accuracy_set
 *      bcm_esw_ptp_clock_max_steps_removed_get
 *      bcm_esw_ptp_clock_max_steps_removed_set
 *      bcm_esw_ptp_clock_create
 *      bcm_esw_ptp_clock_current_dataset_get
 *      bcm_esw_ptp_clock_default_dataset_get
 *      bcm_esw_ptp_clock_domain_get
 *      bcm_esw_ptp_clock_domain_set
 *      bcm_esw_ptp_clock_get
 *      bcm_esw_ptp_clock_parent_dataset_get
 *      bcm_esw_ptp_clock_port_announce_receipt_timeout_get
 *      bcm_esw_ptp_clock_port_announce_receipt_timeout_set
 *      bcm_esw_ptp_clock_port_configure
 *      bcm_esw_ptp_clock_port_dataset_get
 *      bcm_esw_ptp_clock_port_delay_mechanism_get
 *      bcm_esw_ptp_clock_port_delay_mechanism_set
 *      bcm_esw_ptp_clock_port_disable
 *      bcm_esw_ptp_clock_port_enable
 *      bcm_esw_ptp_clock_port_identity_get
 *      bcm_esw_ptp_clock_port_info_get
 *      bcm_esw_ptp_clock_port_latency_get
 *      bcm_esw_ptp_clock_port_latency_set
 *      bcm_esw_ptp_clock_port_log_announce_interval_get
 *      bcm_esw_ptp_clock_port_log_announce_interval_set
 *      bcm_esw_ptp_clock_port_log_min_delay_req_interval_get
 *      bcm_esw_ptp_clock_port_log_min_delay_req_interval_set
 *      bcm_esw_ptp_clock_port_log_min_pdelay_req_interval_get
 *      bcm_esw_ptp_clock_port_log_min_pdelay_req_interval_set
 *      bcm_esw_ptp_clock_port_log_sync_interval_get
 *      bcm_esw_ptp_clock_port_log_sync_interval_set
 *      bcm_esw_ptp_clock_port_mac_get
 *      bcm_esw_ptp_clock_port_protocol_get
 *      bcm_esw_ptp_clock_port_type_get
 *      bcm_esw_ptp_clock_port_version_number_get
 *      bcm_esw_ptp_clock_priority1_get
 *      bcm_esw_ptp_clock_priority1_set
 *      bcm_esw_ptp_clock_priority2_get
 *      bcm_esw_ptp_clock_priority2_set
 *      bcm_esw_ptp_clock_slaveonly_get
 *      bcm_esw_ptp_clock_slaveonly_set
 *      bcm_esw_ptp_clock_time_get
 *      bcm_esw_ptp_clock_time_properties_get
 *      bcm_esw_ptp_clock_time_set
 *      bcm_esw_ptp_clock_timescale_get
 *      bcm_esw_ptp_clock_timescale_set
 *      bcm_esw_ptp_clock_traceability_get
 *      bcm_esw_ptp_clock_traceability_set
 *      bcm_esw_ptp_clock_user_description_set
 *      bcm_esw_ptp_clock_utc_get
 *      bcm_esw_ptp_clock_utc_set
 *      bcm_esw_ptp_ctdev_alarm_callback_register
 *      bcm_esw_ptp_ctdev_alarm_callback_unregister
 *      bcm_esw_ptp_ctdev_alpha_get
 *      bcm_esw_ptp_ctdev_alpha_set
 *      bcm_esw_ptp_ctdev_enable_get
 *      bcm_esw_ptp_ctdev_enable_set
 *      bcm_esw_ptp_ctdev_verbose_get
 *      bcm_esw_ptp_ctdev_verbose_set
 *      bcm_esw_ptp_detach
 *      bcm_esw_ptp_foreign_master_dataset_get
 *      bcm_esw_ptp_lock
 *      bcm_esw_ptp_unlock
 *      bcm_esw_ptp_init
 *      bcm_esw_ptp_input_channel_precedence_mode_set
 *      bcm_esw_ptp_input_channel_switching_mode_set
 *      bcm_esw_ptp_input_channels_get
 *      bcm_esw_ptp_input_channels_set
 *      bcm_esw_ptp_modular_enable_get
 *      bcm_esw_ptp_modular_enable_set
 *      bcm_esw_ptp_modular_phyts_get
 *      bcm_esw_ptp_modular_phyts_set
 *      bcm_esw_ptp_modular_portbitmap_get
 *      bcm_esw_ptp_modular_portbitmap_set
 *      bcm_esw_ptp_modular_verbose_get
 *      bcm_esw_ptp_modular_verbose_set
 *      bcm_esw_ptp_packet_counters_get
 *      bcm_esw_ptp_packet_counters_clear
 *      bcm_esw_ptp_peer_dataset_get
 *      bcm_esw_ptp_primary_domain_get
 *      bcm_esw_ptp_primary_domain_set
 *      bcm_esw_ptp_servo_configuration_get
 *      bcm_esw_ptp_servo_configuration_set
 *      bcm_esw_ptp_servo_status_get
 *      bcm_esw_ptp_servo_threshold_get
 *      bcm_esw_ptp_servo_threshold_set
 *      bcm_esw_ptp_signal_output_get
 *      bcm_esw_ptp_signal_output_remove
 *      bcm_esw_ptp_signal_output_set
 *      bcm_esw_ptp_signaled_unicast_master_add
 *      bcm_esw_ptp_signaled_unicast_master_remove
 *      bcm_esw_ptp_signaled_unicast_slave_list
 *      bcm_esw_ptp_signaled_unicast_slave_table_clear
 *      bcm_esw_ptp_signaling_arbiter_register
 *      bcm_esw_ptp_signaling_arbiter_unregister
 *      bcm_esw_ptp_stack_create
 *      bcm_esw_ptp_static_unicast_master_add
 *      bcm_esw_ptp_static_unicast_master_list
 *      bcm_esw_ptp_static_unicast_master_table_clear
 *      bcm_esw_ptp_static_unicast_master_table_size_get
 *      bcm_esw_ptp_static_unicast_slave_add
 *      bcm_esw_ptp_static_unicast_slave_list
 *      bcm_esw_ptp_static_unicast_slave_remove
 *      bcm_esw_ptp_static_unicast_slave_table_clear
 *      bcm_esw_ptp_sync_phy
 *      bcm_esw_ptp_telecom_g8265_init
 *      bcm_esw_ptp_telecom_g8265_network_option_get
 *      bcm_esw_ptp_telecom_g8265_network_option_set
 *      bcm_esw_ptp_telecom_g8265_packet_master_add
 *      bcm_esw_ptp_telecom_g8265_packet_master_best_get
 *      bcm_esw_ptp_telecom_g8265_packet_master_get
 *      bcm_esw_ptp_telecom_g8265_packet_master_list
 *      bcm_esw_ptp_telecom_g8265_packet_master_lockout_set
 *      bcm_esw_ptp_telecom_g8265_packet_master_non_reversion_set
 *      bcm_esw_ptp_telecom_g8265_packet_master_priority_override
 *      bcm_esw_ptp_telecom_g8265_packet_master_priority_set
 *      bcm_esw_ptp_telecom_g8265_packet_master_remove
 *      bcm_esw_ptp_telecom_g8265_packet_master_wait_duration_set
 *      bcm_esw_ptp_telecom_g8265_pktstats_thresholds_get
 *      bcm_esw_ptp_telecom_g8265_pktstats_thresholds_set
 *      bcm_esw_ptp_telecom_g8265_quality_level_set
 *      bcm_esw_ptp_telecom_g8265_receipt_timeout_get
 *      bcm_esw_ptp_telecom_g8265_receipt_timeout_set
 *      bcm_esw_ptp_telecom_g8265_shutdown
 *      bcm_esw_ptp_time_format_set
 *      bcm_esw_ptp_timesource_input_status_get
 *      bcm_esw_ptp_tod_input_sources_get
 *      bcm_esw_ptp_tod_input_sources_set
 *      bcm_esw_ptp_tod_output_get
 *      bcm_esw_ptp_tod_output_remove
 *      bcm_esw_ptp_tod_output_set
 *      bcm_esw_ptp_transparent_clock_default_dataset_get
 *      bcm_esw_ptp_transparent_clock_port_dataset_get
 *      bcm_esw_ptp_unicast_request_duration_get
 *      bcm_esw_ptp_unicast_request_duration_max_get
 *      bcm_esw_ptp_unicast_request_duration_max_set
 *      bcm_esw_ptp_unicast_request_duration_min_get
 *      bcm_esw_ptp_unicast_request_duration_min_set
 *      bcm_esw_ptp_unicast_request_duration_set
 *      bcm_esw_ptp_clock_peer_age_timer_set
 *      bcm_esw_ptp_clock_peer_age_timer_get
 *      bcm_esw_tdpll_input_clock_ql_change_callback_register
 *      bcm_esw_tdpll_input_clock_ql_change_callback_unregister
 *      bcm_esw_esmc_tx
 *      bcm_esw_esmc_rx_callback_register
 *      bcm_esw_esmc_rx_callback_unregister
 *      bcm_esw_esmc_tunnel_get
 *      bcm_esw_esmc_tunnel_set
 *      bcm_esw_esmc_g781_option_get()
 *      bcm_esw_esmc_g781_option_set()
 *      bcm_esw_esmc_QL_SSM_map
 *      bcm_esw_esmc_SSM_QL_map
 *      bcm_esw_tdpll_dpll_bindings_get
 *      bcm_esw_tdpll_dpll_bindings_set
 *      bcm_esw_tdpll_dpll_reference_get
 *      bcm_esw_tdpll_dpll_bandwidth_get
 *      bcm_esw_tdpll_dpll_bandwidth_set
 *      bcm_esw_tdpll_dpll_phase_control_get
 *      bcm_esw_tdpll_dpll_phase_control_set
 *      bcm_esw_tdpll_input_clock_control
 *      bcm_esw_tdpll_input_clock_mac_get
 *      bcm_esw_tdpll_input_clock_mac_set
 *      bcm_esw_tdpll_input_clock_frequency_error_get
 *      bcm_esw_tdpll_input_clock_threshold_state_get
 *      bcm_esw_tdpll_input_clock_enable_get
 *      bcm_esw_tdpll_input_clock_enable_set
 *      bcm_esw_tdpll_input_clock_l1mux_get
 *      bcm_esw_tdpll_input_clock_l1mux_set
 *      bcm_esw_tdpll_input_clock_valid_get
 *      bcm_esw_tdpll_input_clock_valid_set
 *      bcm_esw_tdpll_input_clock_frequency_get
 *      bcm_esw_tdpll_input_clock_frequency_set
 *      bcm_esw_tdpll_input_clock_ql_get
 *      bcm_esw_tdpll_input_clock_ql_set
 *      bcm_esw_tdpll_input_clock_priority_get
 *      bcm_esw_tdpll_input_clock_priority_set
 *      bcm_esw_tdpll_input_clock_lockout_get
 *      bcm_esw_tdpll_input_clock_lockout_set
 *      bcm_esw_tdpll_input_clock_monitor_interval_get
 *      bcm_esw_tdpll_input_clock_monitor_interval_set
 *      bcm_esw_tdpll_input_clock_monitor_threshold_get
 *      bcm_esw_tdpll_input_clock_monitor_threshold_set
 *      bcm_esw_tdpll_input_clock_ql_enabled_get
 *      bcm_esw_tdpll_input_clock_ql_enabled_set
 *      bcm_esw_tdpll_input_clock_revertive_get
 *      bcm_esw_tdpll_input_clock_revertive_set
 *      bcm_esw_tdpll_input_clock_best_get
 *      bcm_esw_tdpll_input_clock_monitor_cb_register
 *      bcm_esw_tdpll_input_clock_monitor_cb_unregister
 *      bcm_esw_tdpll_input_clock_selector_cb_register
 *      bcm_esw_tdpll_input_clock_selector_cb_unregister
 *      bcm_esw_tdpll_output_clock_enable_get
 *      bcm_esw_tdpll_output_clock_enable_set
 *      bcm_esw_tdpll_output_clock_synth_frequency_get
 *      bcm_esw_tdpll_output_clock_synth_frequency_set
 *      bcm_esw_tdpll_output_clock_deriv_frequency_get
 *      bcm_esw_tdpll_output_clock_deriv_frequency_set
 *      bcm_esw_tdpll_output_clock_holdover_data_get
 *      bcm_esw_tdpll_output_clock_holdover_frequency_set
 *      bcm_esw_tdpll_output_clock_holdover_mode_get
 *      bcm_esw_tdpll_output_clock_holdover_mode_set
 *      bcm_esw_tdpll_output_clock_holdover_reset
 *      bcm_esw_tdpll_esmc_rx_state_machine
 *      bcm_esw_tdpll_esmc_ql_get
 *      bcm_esw_tdpll_esmc_ql_set
 *      bcm_esw_tdpll_esmc_holdover_ql_get
 *      bcm_esw_tdpll_esmc_holdover_ql_set
 *      bcm_esw_tdpll_esmc_mac_get
 *      bcm_esw_tdpll_esmc_mac_set
 *      bcm_esw_tdpll_esmc_rx_enable_get
 *      bcm_esw_tdpll_esmc_rx_enable_set
 *      bcm_esw_tdpll_esmc_tx_enable_get
 *      bcm_esw_tdpll_esmc_tx_enable_set
 *      bcm_esw_tdpll_esmc_rx_portbitmap_get
 *      bcm_esw_tdpll_esmc_rx_portbitmap_set
 *      bcm_esw_tdpll_esmc_tx_portbitmap_get
 *      bcm_esw_tdpll_esmc_tx_portbitmap_set
 *      
 *      Private Interface functions
 *     _bcm_esw_ptp_hw_clear(int unit)
 */


#if defined(INCLUDE_PTP)

#include <soc/defs.h>
#include <soc/drv.h>

#include <sal/core/dpc.h>

#include <bcm/ptp.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>

/****************************************************************************/
/*                     Internal functions implementation                    */
/****************************************************************************/
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HELIX4_SUPPORT) || \
    defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
    defined(BCM_SABER2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
STATIC int
_bcm_esw_ptp_hw_clear(int unit);
#endif

/*
 * Function:
 *    _bcm_esw_ptp_hw_clear
 * Purpose:
 *    Internal routine used to clear all TimeSync Interrupt
 *    registers and table to default values
 * Parameters:
 *    unit           - (IN) BCM device number.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int 
_bcm_esw_ptp_hw_clear(int unit)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HELIX4_SUPPORT) || \
    defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
    defined(BCM_SABER2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (!SOC_IS_TRIDENT2PLUS(unit) && !SOC_IS_TRIDENT2(unit) &&
        !SOC_IS_KATANA(unit) && !SOC_IS_TRIUMPH3(unit) &&
        !(SOC_IS_FIRELIGHT(unit))) {
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, 0);
        if (soc_feature(unit, soc_feature_time_v3)) {
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, 0);
        }
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_ptp_acceptable_master_add
 * Purpose:
 *      Add an entry to the acceptable master table of a PTP clock.
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      priority1_alt_value - Alternate priority1
 *      *master_info - Acceptable master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_acceptable_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int priority1_alt_value,
    bcm_ptp_clock_peer_address_t *master_info)
{
    int rc;

    rc = bcm_common_ptp_acceptable_master_add(unit, ptp_id, clock_num, port_num, 
             priority1_alt_value, master_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_acceptable_master_enabled_get
 * Purpose:
 *      Get maximum number of acceptable master table entries of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *enabled - Acceptable master table enabled Boolean
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_acceptable_master_enabled_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint8 *enabled)
{
    int rc;

    rc = bcm_common_ptp_acceptable_master_enabled_get(unit, ptp_id, clock_num, port_num, 
             enabled);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_acceptable_master_enabled_set
 * Purpose:
 *      
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      enabled - Acceptable master table enabled Boolean
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_acceptable_master_enabled_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint8 enabled)
{
    int rc;

    rc = bcm_common_ptp_acceptable_master_enabled_set(unit, ptp_id, clock_num, port_num, 
             enabled);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_acceptable_master_list
 * Purpose:
 *      List the acceptable master table of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      max_num_masters - Maximum number of acceptable master table entries
 *      *num_masters - Number of acceptable master table entries
 *      *master_addr - Acceptable master table
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_acceptable_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_masters,
    int *num_masters,
    bcm_ptp_clock_peer_address_t *master_addr)
{
    int rc;

    rc = bcm_common_ptp_acceptable_master_list(unit, ptp_id, clock_num, port_num, 
             max_num_masters, num_masters, master_addr);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_acceptable_master_remove
 * Purpose:
 *      Remove an entry from the acceptable master table of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *master_info - Acceptable master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_acceptable_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_address_t *master_info)
{
    int rc;

    rc = bcm_common_ptp_acceptable_master_remove(unit, ptp_id, clock_num, port_num, 
             master_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_acceptable_master_table_clear
 * Purpose:
 *      Clear (i.e. remove all entries from) the acceptable master table of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_acceptable_master_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num)
{
    int rc;

    rc = bcm_common_ptp_acceptable_master_table_clear(unit, ptp_id, clock_num, port_num);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_acceptable_master_table_size_get
 * Purpose:
 *      Get maximum number of acceptable master table entries of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *max_table_entries - Maximum number of acceptable master table entries
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_acceptable_master_table_size_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int *max_table_entries)
{
    int rc;

    rc = bcm_common_ptp_acceptable_master_table_size_get(unit, ptp_id, clock_num, port_num, 
             max_table_entries);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_cb_register
 * Purpose:
 *      Register a callback for handling PTP events.
 * Parameters:
 *      unit - Unit number 
 *      cb_types - The set of PTP callbacks types for which the specified callback should be called
 *      cb - A pointer to the callback function to call for the specified PTP events
 *      *user_data - Pointer to user data to supply in the callback
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_cb_register(
    int unit,
    bcm_ptp_cb_types_t cb_types,
    bcm_ptp_cb cb,
    void *user_data)
{
    int rc;

    rc = bcm_common_ptp_cb_register(unit, cb_types, cb, user_data);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_cb_unregister
 * Purpose:
 *      Unregister a callback for handling PTP events.
 * Parameters:
 *      unit - Unit number 
 *      cb_types - The set of PTP events to unregister for the specified callback
 *      cb - A pointer to the callback function to unregister from the specified PTP events
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_cb_unregister(
    int unit,
    bcm_ptp_cb_types_t cb_types,
    bcm_ptp_cb cb)
{
    int rc;

    rc = bcm_common_ptp_cb_unregister(unit, cb_types, cb);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_accuracy_get
 * Purpose:
 *      Get clock quality, clock accuracy member of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *accuracy - PTP clock accuracy
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_accuracy_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_accuracy_t *accuracy)
{
    int rc;

    rc = bcm_common_ptp_clock_accuracy_get(unit, ptp_id, clock_num, accuracy);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_accuracy_set
 * Purpose:
 *      Set clock quality, clock accuracy member of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *accuracy - PTP clock accuracy
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_accuracy_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_accuracy_t *accuracy)
{
    int rc;

    rc = bcm_common_ptp_clock_accuracy_set(unit, ptp_id, clock_num, accuracy);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_max_steps_removed_get
 * Purpose:
 *      Get maxStepsRemoved configured for the PTP clock.
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *max_steps_removed - max steps removed for the PTP clock
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_max_steps_removed_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *max_steps_removed)
{
    int rc;

    rc = bcm_common_ptp_clock_max_steps_removed_get(unit, ptp_id, clock_num, max_steps_removed);

    return (rc);
}

/*
 * Function:
 *     bcm_esw_ptp_clock_max_steps_removed_set
 * Purpose:
 *     Set max steps removed for the PTP clock.
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      max_steps_removed - max acceptable steps removed
 * Returns:
 *      int
 * Notes:
 */
int bcm_esw_ptp_clock_max_steps_removed_set
(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 max_steps_removed)
{
    int rc;

    rc = bcm_common_ptp_clock_max_steps_removed_set(unit, ptp_id, clock_num, max_steps_removed);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_quality_get
 * Purpose:
 *      Get clock quality information a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *clock_quality - PTP clock quality
 *      flags - identify fields of bcm_ptp_clock_quality_t to get
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_quality_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_quality_t *clock_quality,
    uint32 flags)
{
    int rc;

    rc = bcm_common_ptp_clock_quality_get(unit, ptp_id, clock_num, clock_quality, flags);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_quality_set
 * Purpose:
 *      Set clock quality information of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_quality - PTP clock quality
 *      flags - identify fields of bcm_ptp_clock_quality_t to configure
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_quality_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_quality_t clock_quality,
    uint32 flags)
{
    int rc;

    rc = bcm_common_ptp_clock_quality_set(unit, ptp_id, clock_num, clock_quality, flags);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_g8275p1_traceability_info_clock_class_map
 * Purpose:
 *      Retrieve applicable clock class value for given clock traceability information
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      traceability_info - clock traceability information
 *      clock_class - clock class applicable to the provided traceability information as per G8275.1
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_g8275p1_traceability_info_clock_class_map(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_g8275p1_clock_traceability_info_t traceability_info,
    uint8 *clock_class)
{
    int rc;

    rc = bcm_common_ptp_g8275p1_traceability_info_clock_class_map(unit, ptp_id, clock_num, traceability_info, clock_class);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_local_priority_get
 * Purpose:
 *      Get local priority of a PTP clock.
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *local_priority - PTP clock local priority
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_local_priority_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *local_priority)
{
    int rc;

    rc = bcm_common_ptp_clock_local_priority_get(unit, ptp_id, clock_num, local_priority);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_local_priority_set
 * Purpose:
 *      Set local priority of a PTP clock.
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      local_priority - PTP clock local priority
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_local_priority_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 local_priority)
{
    int rc;

    rc = bcm_common_ptp_clock_local_priority_set(unit, ptp_id, clock_num, local_priority);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_peer_age_timer_get
 * Purpose:
 *      Get slave peer aging time for PTP clock
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *aging_time - PTP clock peer aging time
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_peer_age_timer_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *aging_time)
{
    int rc;

    rc = bcm_common_ptp_clock_peer_age_timer_get(unit, ptp_id, clock_num, aging_time);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_peer_age_timer_set
 * Purpose:
 *      Set slave peer aging time for PTP clock
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      aging_time - PTP clock peer aging time
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_peer_age_timer_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 aging_time)
{
    int rc;

    rc = bcm_common_ptp_clock_peer_age_timer_set(unit, ptp_id, clock_num, aging_time);

    return (rc);
}


/*
 * Function:
 *      bcm_esw_ptp_pkt_flags_override_get
 * Purpose:
 *      Get Override Mask/Value of flags for PTP clock
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      override_flag - Override Mask for flags to set
 *      override_value - Override Value for flags to set
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_pkt_flags_override_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *override_flag,
    uint32 *override_value)
{
    int rc;

    rc = bcm_common_ptp_pkt_flags_override_get(unit, ptp_id, clock_num, override_flag, override_value);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_pkt_flags_override_set 
 * Purpose:
 *      Set Override Mask/Value of flags for PTP clock
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      override_flag - Override Mask for flags to set
 *      override_value - Override Value for flags to set
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_pkt_flags_override_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 override_flag,
    uint32 override_value)
{
    int rc;

    rc = bcm_common_ptp_pkt_flags_override_set(unit, ptp_id, clock_num, override_flag, override_value);

    return (rc);
}


/*
 * Function:
 *      bcm_esw_ptp_clock_create
 * Purpose:
 *      Create a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      *clock_info - PTP clock information
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_create(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_clock_info_t *clock_info)
{
    int rc;

    rc = bcm_common_ptp_clock_create(unit, ptp_id, clock_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_current_dataset_get
 * Purpose:
 *      Get PTP clock current dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *dataset - Current dataset
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_current_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_current_dataset_t *dataset)
{
    int rc;

    rc = bcm_common_ptp_clock_current_dataset_get(unit, ptp_id, clock_num, dataset);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_default_dataset_get
 * Purpose:
 *      Get PTP clock current dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *dataset - Current dataset
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_default_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_default_dataset_t *dataset)
{
    int rc;

    rc = bcm_common_ptp_clock_default_dataset_get(unit, ptp_id, clock_num, dataset);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_domain_get
 * Purpose:
 *      Get PTP domain member of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *domain - PTP clock domain
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_domain_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *domain)
{
    int rc;

    rc = bcm_common_ptp_clock_domain_get(unit, ptp_id, clock_num, domain);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_domain_set
 * Purpose:
 *      Set PTP domain member of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      domain - PTP clock domain
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_domain_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 domain)
{
    int rc;

    rc = bcm_common_ptp_clock_domain_set(unit, ptp_id, clock_num, domain);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_get
 * Purpose:
 *      Get configuration information (data and metadata) of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *clock_info - PTP clock information
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_info_t *clock_info)
{
    int rc;

    rc = bcm_common_ptp_clock_get(unit, ptp_id, clock_num, clock_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_parent_dataset_get
 * Purpose:
 *      Get PTP clock parent dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *dataset - Current dataset
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_parent_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_parent_dataset_t *dataset)
{
    int rc;

    rc = bcm_common_ptp_clock_parent_dataset_get(unit, ptp_id, clock_num, dataset);

    return (rc);
}


/*
 * Function:
 *      bcm_esw_ptp_phase_offset_set
 * Purpose:
 *      Set phase offsets for path asymmetry compensation
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *offset - PTP servo phase holdover state offsets
 * Returns:
 *      int
 * Notes:
 */

int
bcm_esw_ptp_phase_offset_set(
     int unit,
     bcm_ptp_stack_id_t ptp_id,
     int clock_num,
     const bcm_ptp_phase_offset_t *offset) 
{
    int rc;

    rc = bcm_common_ptp_phase_offset_set(unit, ptp_id, clock_num, offset);
    
    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_phase_offset_get
 * Purpose:
 *      Get phase offsets for path asymmetry compensation
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *offset - PTP servo phase holdover state offsets
 * Returns:
 *      int
 * Notes:
 */

int
bcm_esw_ptp_phase_offset_get(
     int unit,
     bcm_ptp_stack_id_t ptp_id,
     int clock_num,
     bcm_ptp_phase_offset_t *offset) 
{
    int rc;

    rc = bcm_common_ptp_phase_offset_get(unit, ptp_id, clock_num, offset);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_announce_receipt_timeout_get
 * Purpose:
 *      Get announce receipt timeout member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *timeout - PTP clock port announce receipt timeout
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_announce_receipt_timeout_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *timeout)
{
    int rc;

    rc = bcm_common_ptp_clock_port_announce_receipt_timeout_get(unit, ptp_id, clock_num, 
             clock_port, timeout);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_announce_receipt_timeout_set
 * Purpose:
 *      Set announce receipt timeout member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      timeout - PTP clock port announce receipt timeout
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_announce_receipt_timeout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 timeout)
{
    int rc;

    rc = bcm_common_ptp_clock_port_announce_receipt_timeout_set(unit, ptp_id, clock_num, 
             clock_port, timeout);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_configure
 * Purpose:
 *      Configure a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *info - PTP clock port configuration information
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_configure(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_clock_port_info_t *info)
{
    int rc;

    rc = bcm_common_ptp_clock_port_configure(unit, ptp_id, clock_num, clock_port, info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_dataset_get
 * Purpose:
 *      Get PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *dataset - Current dataset
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_port_dataset_t *dataset)
{
    int rc;

    rc = bcm_common_ptp_clock_port_dataset_get(unit, ptp_id, clock_num, clock_port, dataset);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_delay_mechanism_get
 * Purpose:
 *      Get delay mechanism member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *delay_mechanism - PTP clock port delay mechanism
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_delay_mechanism_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *delay_mechanism)
{
    int rc;

    rc = bcm_common_ptp_clock_port_delay_mechanism_get(unit, ptp_id, clock_num, clock_port, 
             delay_mechanism);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_delay_mechanism_set
 * Purpose:
 *      Set delay mechanism member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      delay_mechanism - PTP clock port delay mechanism
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_delay_mechanism_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 delay_mechanism)
{
    int rc;

    rc = bcm_common_ptp_clock_port_delay_mechanism_set(unit, ptp_id, clock_num, clock_port, 
             delay_mechanism);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_disable
 * Purpose:
 *      Disable a PTP clock port of an ordinary clock (OC) or a boundary clock (BC).
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_disable(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port)
{
    int rc;

    rc = bcm_common_ptp_clock_port_disable(unit, ptp_id, clock_num, clock_port);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_enable
 * Purpose:
 *      Enable a PTP clock port of an ordinary clock (OC) or a boundary clock (BC).
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_enable(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port)
{
    int rc;

    rc = bcm_common_ptp_clock_port_enable(unit, ptp_id, clock_num, clock_port);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_identity_get
 * Purpose:
 *      Get port identity of a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *identity - PTP port identity
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_identity_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_port_identity_t *identity)
{
    int rc;

    rc = bcm_common_ptp_clock_port_identity_get(unit, ptp_id, clock_num, clock_port, identity);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_info_get
 * Purpose:
 *      Get configuration information (data and metadata) of a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *info - PTP clock port configuration information
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_info_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_clock_port_info_t *info)
{
    int rc;

    rc = bcm_common_ptp_clock_port_info_get(unit, ptp_id, clock_num, clock_port, info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_latency_get
 * Purpose:
 *      Get PTP clock port expected asymmetric latencies.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *latency_in - PTP clock port inbound latency (ns)
 *      *latency_out - PTP clock port outbound latency (ns)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_latency_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *latency_in,
    uint32 *latency_out)
{
    int rc;

    rc = bcm_common_ptp_clock_port_latency_get(unit, ptp_id, clock_num, clock_port, latency_in, 
             latency_out);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_latency_set
 * Purpose:
 *      Set PTP clock port expected asymmetric latencies.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      latency_in - PTP clock port inbound latency (ns)
 *      latency_out - PTP clock port outbound latency (ns)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_latency_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 latency_in,
    uint32 latency_out)
{
    int rc;

    rc = bcm_common_ptp_clock_port_latency_set(unit, ptp_id, clock_num, clock_port, latency_in, 
             latency_out);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_log_announce_interval_get
 * Purpose:
 *      Get log announce interval of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *interval - PTP clock port log announce interval
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_log_announce_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    int rc;

    rc = bcm_common_ptp_clock_port_log_announce_interval_get(unit, ptp_id, clock_num, clock_port, 
             interval);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_log_announce_interval_set
 * Purpose:
 *      Set log announce interval of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      interval - PTP clock port log announce interval
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_log_announce_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    int rc;

    rc = bcm_common_ptp_clock_port_log_announce_interval_set(unit, ptp_id, clock_num, clock_port, 
             interval);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_log_min_delay_req_interval_get
 * Purpose:
 *      Get log minimum delay request interval member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *interval - PTP clock port log minimum delay request interval
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_log_min_delay_req_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    int rc;

    rc = bcm_common_ptp_clock_port_log_min_delay_req_interval_get(unit, ptp_id, clock_num, 
             clock_port, interval);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_log_min_delay_req_interval_set
 * Purpose:
 *      Set log minimum delay request interval member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      interval - PTP clock port log minimum delay request interval
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_log_min_delay_req_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    int rc;

    rc = bcm_common_ptp_clock_port_log_min_delay_req_interval_set(unit, ptp_id, clock_num, 
             clock_port, interval);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_log_min_pdelay_req_interval_get
 * Purpose:
 *      Get log minimum peer delay (PDelay) request interval member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *interval - PTP clock port log minimum peer delay (PDelay) request interval
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_log_min_pdelay_req_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    int rc;

    rc = bcm_common_ptp_clock_port_log_min_pdelay_req_interval_get(unit, ptp_id, clock_num, 
             clock_port, interval);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_log_min_pdelay_req_interval_set
 * Purpose:
 *      Set log minimum peer delay (PDelay) request interval member of a PTP
 *      clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      interval - PTP clock port log minimum peer delay (PDelay) request interval
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_log_min_pdelay_req_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    int rc;

    rc = bcm_common_ptp_clock_port_log_min_pdelay_req_interval_set(unit, ptp_id, clock_num, 
             clock_port, interval);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_log_sync_interval_get
 * Purpose:
 *      Get log sync interval member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *interval - PTP clock port log sync interval
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_log_sync_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    int rc;

    rc = bcm_common_ptp_clock_port_log_sync_interval_get(unit, ptp_id, clock_num, clock_port, 
             interval);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_log_sync_interval_set
 * Purpose:
 *      Set log sync interval member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      interval - PTP clock port log sync interval
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_log_sync_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    int rc;

    rc = bcm_common_ptp_clock_port_log_sync_interval_set(unit, ptp_id, clock_num, clock_port, 
             interval);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_mac_get
 * Purpose:
 *      Get MAC address of a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *mac - PTP clock port MAC address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_mac_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_mac_t *mac)
{
    int rc;

    rc = bcm_common_ptp_clock_port_mac_get(unit, ptp_id, clock_num, clock_port, mac);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_protocol_get
 * Purpose:
 *      Get network protocol of a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *protocol - PTP clock port network protocol
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_protocol_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_protocol_t *protocol)
{
    int rc;

    rc = bcm_common_ptp_clock_port_protocol_get(unit, ptp_id, clock_num, clock_port, protocol);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_type_get
 * Purpose:
 *      Get port type of a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *type - PTP clock port type
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_type_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_port_type_t *type)
{
    int rc;

    rc = bcm_common_ptp_clock_port_type_get(unit, ptp_id, clock_num, clock_port, type);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_port_version_number_get
 * Purpose:
 *      Get PTP version number member of a PTP clock port dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *version - PTP clock port PTP version number
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_port_version_number_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *version)
{
    int rc;

    rc = bcm_common_ptp_clock_port_version_number_get(unit, ptp_id, clock_num, clock_port, 
             version);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_priority1_get
 * Purpose:
 *      Get priority1 member of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *priority1 - PTP clock priority1.
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_priority1_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *priority1)
{
    int rc;

    rc = bcm_common_ptp_clock_priority1_get(unit, ptp_id, clock_num, priority1);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_priority1_set
 * Purpose:
 *      Set priority1 member of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      priority1 - PTP clock priority1
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_priority1_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 priority1)
{
    int rc;

    rc = bcm_common_ptp_clock_priority1_set(unit, ptp_id, clock_num, priority1);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_priority2_get
 * Purpose:
 *      Get priority2 member of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *priority2 - PTP clock priority2
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_priority2_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *priority2)
{
    int rc;

    rc = bcm_common_ptp_clock_priority2_get(unit, ptp_id, clock_num, priority2);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_priority2_set
 * Purpose:
 *      Set priority2 member of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      priority2 - PTP clock priority2
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_priority2_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 priority2)
{
    int rc;

    rc = bcm_common_ptp_clock_priority2_set(unit, ptp_id, clock_num, priority2);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_slaveonly_get
 * Purpose:
 *      Get slave-only (SO) flag of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *slaveonly - PTP clock slave-only (SO) flag
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_slaveonly_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *slaveonly)
{
    int rc;

    rc = bcm_common_ptp_clock_slaveonly_get(unit, ptp_id, clock_num, slaveonly);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_slaveonly_set
 * Purpose:
 *      Set slave-only (SO) flag of a PTP clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      slaveonly - PTP clock slave-only (SO) flag.
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_slaveonly_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 slaveonly)
{
    int rc;

    rc = bcm_common_ptp_clock_slaveonly_set(unit, ptp_id, clock_num, slaveonly);

    return (rc);
}

#ifdef BCM_GREYHOUND2_SUPPORT
static bcm_time_if_t time_if_id[BCM_MAX_NUM_UNITS] = {-1};
#endif /* BCM_GREYHOUND2_SUPPORT */
/*
 * Function:
 *      bcm_esw_ptp_clock_time_get
 * Purpose:
 *      Get local time of a PTP node.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *time - PTP timestamp
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_time_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timestamp_t *time)
{
    int rc;

#ifdef BCM_GREYHOUND2_SUPPORT
    if (soc_feature(unit, soc_feature_use_local_1588)) {
#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            time_if_id[unit] = 0;
        }
#endif /* BCM_WARM_BOOT_SUPPORT */
        if (_bcm_ptp_running(unit) != BCM_E_NONE) {
            bcm_time_capture_t time1;

            bcm_time_capture_t_init(&time1);
            BCM_IF_ERROR_RETURN(
                bcm_esw_time_capture_get(unit, time_if_id[unit], &time1));
            time->nanoseconds = time1.free.nanoseconds;
            COMPILER_64_COPY(time->seconds,time1.free.seconds);
            return BCM_E_NONE;
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    rc = bcm_common_ptp_clock_time_get(unit, ptp_id, clock_num, time);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_time_properties_get
 * Purpose:
 *      Get PTP clock time properties dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *data - Time properties dataset
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_time_properties_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_time_properties_t *data)
{
    int rc;

    rc = bcm_common_ptp_clock_time_properties_get(unit, ptp_id, clock_num, data);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_time_set
 * Purpose:
 *      Set local time of a PTP node.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *time - PTP timestamp
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_time_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timestamp_t *time)
{
    int rc;

#ifdef BCM_GREYHOUND2_SUPPORT
    if (soc_feature(unit, soc_feature_use_local_1588)) {
        bcm_time_interface_t    time_if;
        uint32                  sec, regval = 0;
        uint64                  val64;

        if (_bcm_ptp_running(unit) != BCM_E_NONE) {
            if (time_if_id[unit] != -1) {
                (void)bcm_esw_time_interface_delete(unit, time_if_id[unit]);
            }
            bcm_time_interface_t_init(&time_if);
            BCM_IF_ERROR_RETURN(bcm_esw_time_interface_add(unit, &time_if));
            time_if_id[unit] = time_if.id;

            WRITE_TS_TO_CORE_SYNC_ENABLEr(unit, 1);
            sec = COMPILER_64_LO(time->seconds);
            soc_reg_field_set(unit, IEEE1588_TIME_SECr, &regval,
                              ONE_SECf, sec % 2);
            sec = (sec >> 1) & 0x7FFFFFFF;
            soc_reg_field_set(unit, IEEE1588_TIME_SECr, &regval,
                              UPPER_SECf, sec);
            BCM_IF_ERROR_RETURN(
                WRITE_IEEE1588_TIME_SECr(unit, sec));
            BCM_IF_ERROR_RETURN(
                WRITE_IEEE1588_TIME_FRAC_SEC_UPPERr(unit,
                                                    time->nanoseconds >> 6));
            BCM_IF_ERROR_RETURN(
                WRITE_IEEE1588_TIME_FRAC_SEC_LOWERr(unit,
                                                    time->nanoseconds << 26));
            BCM_IF_ERROR_RETURN(
                READ_IEEE1588_TIME_CONTROLr(unit, &regval));
            regval |= 0x1f;    /* Set load-enable bits, |B04...B00|. */
            BCM_IF_ERROR_RETURN(
                WRITE_IEEE1588_TIME_CONTROLr(unit, regval));
            regval &= ~(0x1f); /* Clear load-enable bits, |B04...B00|. */
            BCM_IF_ERROR_RETURN(
                WRITE_IEEE1588_TIME_FREQ_CONTROLr(unit, 0x010000000));
            regval |= 0x20;    /* Set count-enable bit, |B05|. */
            BCM_IF_ERROR_RETURN(
                WRITE_IEEE1588_TIME_CONTROLr(unit, regval));
            COMPILER_64_COPY(val64, time->seconds);
            COMPILER_64_UMUL_32(val64, 1000000000);
            COMPILER_64_ADD_32(val64, time->nanoseconds);
#ifdef BCM_FIRELIGHT_SUPPORT
            if (SOC_IS_FIRELIGHT(unit)) {
                /* coverity[result_independent_of_operands] */
                BCM_IF_ERROR_RETURN(
                    WRITE_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_UPPERr(unit,
                                                            COMPILER_64_HI(val64)));
                /* coverity[result_independent_of_operands] */
                BCM_IF_ERROR_RETURN(
                    WRITE_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_LOWERr(unit,
                                                            COMPILER_64_LO(val64)));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                /* coverity[result_independent_of_operands] */
                BCM_IF_ERROR_RETURN(
                    WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit,
                                                            COMPILER_64_HI(val64)));
                /* coverity[result_independent_of_operands] */
                BCM_IF_ERROR_RETURN(
                    WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit,
                                                            COMPILER_64_LO(val64)));
            }
            return BCM_E_NONE;
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
    rc = bcm_common_ptp_clock_time_set(unit, ptp_id, clock_num, time);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_timescale_get
 * Purpose:
 *      Get timescale members of a PTP clock time properties dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *timescale - PTP clock timescale properties
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_timescale_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timescale_t *timescale)
{
    int rc;

    rc = bcm_common_ptp_clock_timescale_get(unit, ptp_id, clock_num, timescale);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_timescale_set
 * Purpose:
 *      Set timescale members of a PTP clock time properties dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *timescale - PTP clock timescale properties
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_timescale_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timescale_t *timescale)
{
    int rc;

    rc = bcm_common_ptp_clock_timescale_set(unit, ptp_id, clock_num, timescale);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_traceability_get
 * Purpose:
 *      Get traceability members of a PTP clock time properties dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *trace - PTP clock traceability properties
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_traceability_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_trace_t *trace)
{
    int rc;

    rc = bcm_common_ptp_clock_traceability_get(unit, ptp_id, clock_num, trace);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_traceability_set
 * Purpose:
 *      Set traceability members of a PTP clock time properties dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *trace - PTP clock traceability properties.
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_traceability_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_trace_t *trace)
{
    int rc;

    rc = bcm_common_ptp_clock_traceability_set(unit, ptp_id, clock_num, trace);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_user_description_set
 * Purpose:
 *      Set user description member of the default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *desc - User description
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_user_description_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *desc)
{
    int rc;

    rc = bcm_common_ptp_clock_user_description_set(unit, ptp_id, clock_num, desc);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_utc_get
 * Purpose:
 *      Get UTC members of a PTP clock time properties dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *utc - PTP clock coordinated universal time (UTC) properties
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_utc_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_utc_t *utc)
{
    int rc;

    rc = bcm_common_ptp_clock_utc_get(unit, ptp_id, clock_num, utc);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_clock_utc_set
 * Purpose:
 *      Set UTC members of a PTP clock time properties dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *utc - PTP clock coordinated universal time (UTC) properties
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_clock_utc_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_utc_t *utc)
{
    int rc;

    rc = bcm_common_ptp_clock_utc_set(unit, ptp_id, clock_num, utc);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_ctdev_alarm_callback_register
 * Purpose:
 *      Register a C-TDEV alarm callback function.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      alarm_cb - pointer to C-TDEV alarm callback function
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_ctdev_alarm_callback_register(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_ctdev_alarm_cb alarm_cb)
{
    int rc;

    rc = bcm_common_ptp_ctdev_alarm_callback_register(unit, ptp_id, clock_num, alarm_cb);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_ctdev_alarm_callback_unregister
 * Purpose:
 *      Unregister a C-TDEV alarm callback function.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_ctdev_alarm_callback_unregister(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rc;

    rc = bcm_common_ptp_ctdev_alarm_callback_unregister(unit, ptp_id, clock_num);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_ctdev_alpha_get
 * Purpose:
 *      Get C-TDEV recursive algorithm forgetting factor (alpha).
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *alpha_numerator - Forgetting factor numerator
 *      *alpha_denominator - Forgetting factor denominator
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_ctdev_alpha_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 *alpha_numerator,
    uint16 *alpha_denominator)
{
    int rc;

    rc = bcm_common_ptp_ctdev_alpha_get(unit, ptp_id, clock_num, alpha_numerator, 
             alpha_denominator);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_ctdev_alpha_set
 * Purpose:
 *      Set C-TDEV recursive algorithm forgetting factor (alpha).
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      alpha_numerator - Forgetting factor numerator
 *      alpha_denominator - Forgetting factor denominator
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_ctdev_alpha_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 alpha_numerator,
    uint16 alpha_denominator)
{
    int rc;

    rc = bcm_common_ptp_ctdev_alpha_set(unit, ptp_id, clock_num, alpha_numerator, 
             alpha_denominator);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_ctdev_enable_get
 * Purpose:
 *      Get enable/disable state of C-TDEV processing.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *enable - Enable Boolean
 *      *flags - C-TDEV control flags (UNUSED)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_ctdev_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *enable,
    uint32 *flags)
{
    int rc;

    rc = bcm_common_ptp_ctdev_enable_get(unit, ptp_id, clock_num, enable, flags);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_ctdev_enable_set
 * Purpose:
 *      Set enable/disable state of C-TDEV processing.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      enable - Enable Boolean
 *      flags - C-TDEV control flags (UNUSED)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_ctdev_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable,
    uint32 flags)
{
    int rc;

    rc = bcm_common_ptp_ctdev_enable_set(unit, ptp_id, clock_num, enable, flags);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_ctdev_verbose_get
 * Purpose:
 *      Get verbose program control option of C-TDEV processing.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *verbose - Verbose Boolean
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_ctdev_verbose_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *verbose)
{
    int rc;

    rc = bcm_common_ptp_ctdev_verbose_get(unit, ptp_id, clock_num, verbose);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_ctdev_verbose_set
 * Purpose:
 *      Set verbose program control option of C-TDEV processing.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      verbose - Verbose Boolean
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_ctdev_verbose_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int verbose)
{
    int rc;

    rc = bcm_common_ptp_ctdev_verbose_set(unit, ptp_id, clock_num, verbose);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_detach
 * Purpose:
 *      Shut down the PTP subsystem
 * Parameters:
 *      unit - Unit number 
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_detach(
    int unit)
{
    int rc;

    rc = bcm_common_ptp_detach(unit);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_foreign_master_dataset_get
 * Purpose:
 *      Get the foreign master dataset of a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *data_set - Foreign master dataset
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_foreign_master_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_foreign_master_dataset_t *data_set)
{
    int rc;

    rc = bcm_common_ptp_foreign_master_dataset_get(unit, ptp_id, clock_num, port_num, data_set);

    return (rc);
}


/*
 * Function:
 *      bcm_esw_ptp_lock
 * Purpose:
 *      Locks the unit mutex
 * Parameters:
 *      unit - Unit number
 * Returns:
 *      int
 * Notes:
 */
int bcm_esw_ptp_lock(
   int unit)
{
    int rc;

    rc = bcm_common_ptp_lock(unit);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_unlock
 * Purpose:
 *      UnLocks the unit mutex
 * Parameters:
 *      unit - Unit number
 * Returns:
 *      int
 * Notes:
 */
int bcm_esw_ptp_unlock(
   int unit)
{
    int rc;

    rc = bcm_common_ptp_unlock(unit);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_init
 * Purpose:
 *      Initialize the PTP subsystem
 * Parameters:
 *      unit - Unit number 
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_init(
    int unit)
{
    int rc;

#if defined(INCLUDE_PTP)
#if defined(BCM_HURRICANE2_SUPPORT)
    if (!SOC_WARM_BOOT(unit)  && 
         (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit))) {
        int i;
        int ptp_app = 0;
        bcm_field_qset_t qset;
    					
        for (i = 0; i < SOC_CMCS_NUM(unit); i++) {
            if (i == SOC_PCI_CMC(unit)) {
                /* Skip CPU queue */
                continue;
            }
            if (NUM_CPU_ARM_COSQ(unit, i)) {
                /* uC application exists */
                ptp_app = 1;
                break;
            }
        }
        																	
        if (ptp_app) {
            BCM_FIELD_QSET_INIT(qset);
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_group_create_id(unit, qset, 
                                              BCM_FIELD_GROUP_PRIO_ANY, 1));
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_entry_create_id(unit, 1, 0));
            
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_action_ports_add(unit, 0, bcmFieldActionEgressMask,
            				PBMP_CMIC(unit)));
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_entry_install(unit, 0));
        }
    }
#endif /* BCM_HURRICANE2_SUPPORT */
#endif /* INCLUDE_PTP */

    rc = bcm_common_ptp_init(unit);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_input_channel_precedence_mode_set
 * Purpose:
 *      Set PTP input channels precedence mode.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      channel_select_mode - Input channel precedence mode selector
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_input_channel_precedence_mode_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int channel_select_mode)
{
    int rc;

    rc = bcm_common_ptp_input_channel_precedence_mode_set(unit, ptp_id, clock_num, 
             channel_select_mode);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_input_channel_switching_mode_set
 * Purpose:
 *      Set PTP input channels switching mode.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      channel_switching_mode - Channel switching mode selector
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_input_channel_switching_mode_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int channel_switching_mode)
{
    int rc;

    rc = bcm_common_ptp_input_channel_switching_mode_set(unit, ptp_id, clock_num, 
             channel_switching_mode);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_input_channels_get
 * Purpose:
 *      Set PTP input channels.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *num_channels - (IN) Max number of channels (time sources)
 *                      (OUT) Number of returned channels (time sources).
 *      *channels - Channels (time sources)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_input_channels_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_channels,
    bcm_ptp_channel_t *channels)
{
    int rc;

    rc = bcm_common_ptp_input_channels_get(unit, ptp_id, clock_num, num_channels, channels);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_input_channels_set
 * Purpose:
 *      Set PTP input channels.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      num_channels - (IN)  Max number of channels (time sources)
 *                     (OUT) Number of returned channels (time sources).
 *      *channels - Channels (time sources)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_input_channels_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_channels,
    bcm_ptp_channel_t *channels)
{
    int rc;

    rc = bcm_common_ptp_input_channels_set(unit, ptp_id, clock_num, num_channels, channels);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_synce_output_get
 * Purpose:
 *      Gets SyncE output control state.
 * Parameters:
 *      unit - Unit number.
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      state - Non-zero to indicate that SyncE output should be controlled by TOP
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_synce_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *state)
{
  int rc;

  rc = bcm_common_ptp_synce_output_get(unit, ptp_id, clock_num, state);

  return (rc);
}
/*
 * Function:
 *      bcm_esw_ptp_synce_output_set
 * Purpose:
 *      Set SyncE output control on/off.
 * Parameters:
 *      unit  - Unit number.
 *      ptp_id  - PTP stack ID
 *      clock_num - PTP clock number
 *      synce_port - PTP SyncE clock port
 *      state - Non-zero to indicate that SyncE output should be controlled by TOP
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_synce_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int synce_port,
    int state)
{
  int rc;

  rc = bcm_common_ptp_synce_output_set(unit, ptp_id, clock_num, synce_port, state);

  return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_modular_enable_get
 * Purpose:
 *      Get enable/disable state of modular system functionality.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *enable - Enable Boolean
 *      *flags - Modular system control flags (UNUSED)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_modular_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *enable,
    uint32 *flags)
{
    int rc;

    rc = bcm_common_ptp_modular_enable_get(unit, ptp_id, clock_num, enable, flags);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_modular_enable_set
 * Purpose:
 *      Set enable/disable state of modular system functionality.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      enable - Enable Boolean
 *      flags - Modular system control flags (UNUSED)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_modular_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable,
    uint32 flags)
{
    int rc;

    rc = bcm_common_ptp_modular_enable_set(unit, ptp_id, clock_num, enable, flags);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_modular_phyts_get
 * Purpose:
 *      Get PHY timestamping configuration state and data.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *phyts - PHY timestamping Boolean
 *      *framesync_pin - Framesync GPIO pin
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_modular_phyts_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *phyts,
    int *framesync_pin)
{
    int rc;

    rc = bcm_common_ptp_modular_phyts_get(unit, ptp_id, clock_num, phyts, framesync_pin);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_modular_phyts_set
 * Purpose:
 *      Set PHY timestamping configuration state and data.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      phyts - PHY timestamping Boolean
 *      framesync_pin - Framesync GPIO pin
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_modular_phyts_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int phyts,
    int framesync_pin)
{
    int rc;

    rc = bcm_common_ptp_modular_phyts_set(unit, ptp_id, clock_num, phyts, framesync_pin);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_modular_portbitmap_get
 * Purpose:
 *      Get PHY port bitmap.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *pbmp - Port bitmap
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_modular_portbitmap_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_pbmp_t *pbmp)
{
    int rc;

    rc = bcm_common_ptp_modular_portbitmap_get(unit, ptp_id, clock_num, pbmp);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_modular_portbitmap_set
 * Purpose:
 *      Set PHY port bitmap.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      pbmp - Port bitmap
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_modular_portbitmap_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_pbmp_t pbmp)
{
    int rc;

    rc = bcm_common_ptp_modular_portbitmap_set(unit, ptp_id, clock_num, pbmp);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_modular_verbose_get
 * Purpose:
 *      Get verbose program control option of modular system functionality.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *verbose - Verbose Boolean
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_modular_verbose_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *verbose)
{
    int rc;

    rc = bcm_common_ptp_modular_verbose_get(unit, ptp_id, clock_num, verbose);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_modular_verbose_set
 * Purpose:
 *      Set verbose program control option of modular system functionality.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      verbose - Verbose Boolean
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_modular_verbose_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int verbose)
{
    int rc;

    rc = bcm_common_ptp_modular_verbose_set(unit, ptp_id, clock_num, verbose);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_packet_counters_get
 * Purpose:
 *      Get packet counters.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *counters - Packet counts/statistics
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_packet_counters_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_packet_counters_t *counters)
{
    int rc;

    rc = bcm_common_ptp_packet_counters_get(unit, ptp_id, clock_num, counters);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_packet_counters_clear
 * Purpose:
 *      Reset packet counters.
 * Parameters:
 *      unit           - Unit number
 *      ptp_id         - PTP stack ID
 *      clock_num      - PTP clock number
 *      port_num       - PTP clock port number (to clear port-specific stats)
 *      counter_bitmap - counter bitmap
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_packet_counters_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 counter_bitmap)
{
    int rc;

    rc = bcm_common_ptp_packet_counters_clear(unit, ptp_id, clock_num, port_num, counter_bitmap);

    return (rc);
}

int
bcm_esw_ptp_clock_port_drop_counters_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_clock_port_packet_drop_counters_t *counters)
{
    int rc;

    rc = bcm_common_ptp_clock_port_drop_counters_get(unit, ptp_id, clock_num, clock_port, counters);

    return (rc);
}

int
bcm_esw_ptp_clock_port_drop_counters_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int enable)
{
    int rc;
    rc = bcm_common_ptp_clock_port_drop_counters_enable_set( unit,ptp_id,clock_num,clock_port,enable);
    return (rc);
}
/*
 * Function:
 *      bcm_esw_ptp_peer_dataset_get
 * Purpose:
 *      Get Peer Dataset
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP port number
 *      max_num_peers -
 *      peers - list of peer entries
 *      *num_peers - # of peer entries returned in peers
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_peer_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_peers,
    bcm_ptp_peer_entry_t *peers,
    int *num_peers)
{
    int rc;

    rc = bcm_common_ptp_peer_dataset_get(unit, ptp_id, clock_num, port_num, max_num_peers, peers, num_peers);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_primary_domain_get
 * Purpose:
 *      Get primary domain member of a PTP transparent clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *primary_domain - PTP transparent clock primary domain
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_primary_domain_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *primary_domain)
{
    int rc;

    rc = bcm_common_ptp_primary_domain_get(unit, ptp_id, clock_num, primary_domain);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_primary_domain_set
 * Purpose:
 *      Set primary domain member of a PTP transparent clock default dataset.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      primary_domain - PTP transparent clock primary domain.
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_primary_domain_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int primary_domain)
{
    int rc;

    rc = bcm_common_ptp_primary_domain_set(unit, ptp_id, clock_num, primary_domain);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_servo_configuration_get
 * Purpose:
 *      Get PTP servo configuration properties.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *config - PTP servo configuration properties
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_servo_configuration_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_config_t *config)
{
    int rc;

    rc = bcm_common_ptp_servo_configuration_get(unit, ptp_id, clock_num, config);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_servo_configuration_set
 * Purpose:
 *      Set PTP servo configuration properties.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *config - PTP servo configuration properties
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_servo_configuration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_config_t *config)
{
    int rc;

    rc = bcm_common_ptp_servo_configuration_set(unit, ptp_id, clock_num, config);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_servo_status_get
 * Purpose:
 *      Get PTP servo state and status information.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *status - PTP servo state and status information
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_servo_status_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_status_t *status)
{
    int rc;

    rc = bcm_common_ptp_servo_status_get(unit, ptp_id, clock_num, status);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_servo_threshold_get
 * Purpose:
 *      Get PTP servo threshold information.
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *threshold - PTP servo threshold
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_servo_threshold_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_threshold_t *threshold)
{
    int rc;

    rc = bcm_common_ptp_servo_threshold_get(unit, ptp_id, clock_num, threshold);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_servo_threshold_set
 * Purpose:
 *      Set PTP servo threshold information.
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *threshold - PTP servo threshold
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_servo_threshold_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_threshold_t *threshold)
{
    int rc;

    rc = bcm_common_ptp_servo_threshold_set(unit, ptp_id, clock_num, threshold);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_signal_output_get
 * Purpose:
 *      Get PTP signal output.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *signal_output_count - (IN)  Max number of signal outputs
 *                             (OUT) Number signal outputs returned
 *      *signal_output_id - Array of signal outputs
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_signal_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *signal_output_count,
    bcm_ptp_signal_output_t *signal_output_id)
{
    int rc;

    rc = bcm_common_ptp_signal_output_get(unit, ptp_id, clock_num, signal_output_count, 
             signal_output_id);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_signal_output_remove
 * Purpose:
 *      Remove PTP signal output.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      signal_output_id - Signal to remove/invalidate
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_signal_output_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int signal_output_id)
{
    int rc;

    rc = bcm_common_ptp_signal_output_remove(unit, ptp_id, clock_num, signal_output_id);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_signal_output_set
 * Purpose:
 *      Set PTP signal output.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *signal_output_id - ID of signal
 *      *output_info - Signal information
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_signal_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *signal_output_id,
    bcm_ptp_signal_output_t *output_info)
{
    int rc;

    rc = bcm_common_ptp_signal_output_set(unit, ptp_id, clock_num, signal_output_id, 
             output_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_signaled_unicast_master_add
 * Purpose:
 *      Add an entry to the signaled unicast master table.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *master_info - Acceptable master address
 *      mask - Signaled unicast master configuration flags
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_signaled_unicast_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_unicast_master_t *master_info,
    uint32 mask)
{
    int rc;

    rc = bcm_common_ptp_signaled_unicast_master_add(unit, ptp_id, clock_num, port_num, 
             master_info, mask);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_signaled_unicast_master_remove
 * Purpose:
 *      Remove an entry from the signaled unicast master table.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *master_info - Acceptable master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_signaled_unicast_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_address_t *master_info)
{
    int rc;

    rc = bcm_common_ptp_signaled_unicast_master_remove(unit, ptp_id, clock_num, port_num, 
             master_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_signaled_unicast_slave_list
 * Purpose:
 *      List the signaled unicast slaves.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      max_num_slaves - Maximum number of signaled unicast slaves
 *      *num_slaves - Number of signaled unicast slaves
 *      *slave_info - Signaled unicast slave table
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_signaled_unicast_slave_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_slaves,
    int *num_slaves,
    bcm_ptp_clock_peer_t *slave_info)
{
    int rc;

    rc = bcm_common_ptp_signaled_unicast_slave_list(unit, ptp_id, clock_num, port_num, 
             max_num_slaves, num_slaves, slave_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_signaled_unicast_slave_table_clear
 * Purpose:
 *      Cancel active unicast services for slaves and clear signaled unicast
 *      slave table entries that correspond to caller-specified master port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      callstack - Boolean to unsubscribe/unregister unicast services.
 *          TRUE : Cancellation message to slave and PTP stack.
 *          FALSE: Cancellation message tunneled to slave only.
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_signaled_unicast_slave_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int callstack)
{
    int rc;

    rc = bcm_common_ptp_signaled_unicast_slave_table_clear(unit, ptp_id, clock_num, port_num, 
             callstack);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_signaling_arbiter_register
 * Purpose:
 *      Register a signaling arbiter function that contains the accept/reject logic
 *      for incoming signaling messages
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      arb       - (IN) PTP signaling message arbiter function pointer.
 *      user_data - (IN) User data.
 * Returns:
 *      BCM_E_xx : Function status
 * Notes:
 */
int
bcm_esw_ptp_signaling_arbiter_register(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_signaling_arbiter_t arb,
    void *user_data)
{
    int rc;

    rc = _bcm_ptp_register_signaling_arbiter(unit, arb, user_data);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_signaling_arbiter_unregister
 * Purpose:
 *      Unregister a signaling arbiter function that contains the accept/reject logic
 *      for incoming signaling messages
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      arb       - (IN) PTP signaling message arbiter function pointer.
 *      user_data - (IN) User data.
 * Returns:
 *      BCM_E_xx : Function status
 * Notes:
 */
int
bcm_esw_ptp_signaling_arbiter_unregister(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rc;

    rc = _bcm_ptp_unregister_signaling_arbiter(unit);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_stack_create
 * Purpose:
 *      Create a PTP stack instance
 * Parameters:
 *      unit - Unit number 
 *      *ptp_info - Pointer to an PTP Stack Info structure
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_stack_create(
    int unit,
    bcm_ptp_stack_info_t *ptp_info)
{
    int rc;

    rc = bcm_common_ptp_stack_create(unit, ptp_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_stack_destroy
 * Purpose:
 *      Destroy a PTP stack instance
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - ID of stack to destroy
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_stack_destroy(
    int unit,
    bcm_ptp_stack_id_t ptp_id)
{
    int rc;

    rc = bcm_common_ptp_stack_destroy(unit, ptp_id);
    /* Clear HW interrup registers */
    _bcm_esw_ptp_hw_clear(unit);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_stack_get
 * Purpose:
 *      Get a PTP stack instance
 * Parameters:
 *      unit - Unit number
 *      ptp_id - ID of stack to get
 *      ptp_info - info structs for stack
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_stack_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_stack_info_t *ptp_info)
{
    int rc;

    rc = bcm_common_ptp_stack_get(unit, ptp_id, ptp_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_stack_get_all
 * Purpose:
 *      Gets all PTP stack instances
 * Parameters:
 *      unit - Unit number
 *      max_size - maximum number of stacks to return
 *      ptp_info - info structs for stacks
 *      no_of_stacks - actual number of stacks that can be returned
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_stack_get_all(
    int unit,
    int max_size, 
    bcm_ptp_stack_info_t *ptp_info,
    int *no_of_stacks)
{
    int rc;

    rc = bcm_common_ptp_stack_get_all(unit, max_size, ptp_info, no_of_stacks);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_static_unicast_master_add
 * Purpose:
 *      Add an entry to the unicast master table of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *master_info - Acceptable master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_static_unicast_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_unicast_master_t *master_info)
{
    int rc;

    rc = bcm_common_ptp_static_unicast_master_add(unit, ptp_id, clock_num, port_num, 
             master_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_static_unicast_master_list
 * Purpose:
 *      List the unicast master table of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      max_num_masters - Maximum number of unicast master table entries
 *      *num_masters - Number of unicast master table entries
 *      *master_addr - Unicast master table
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_static_unicast_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_masters,
    int *num_masters,
    bcm_ptp_clock_peer_address_t *master_addr)
{
    int rc;

    rc = bcm_common_ptp_static_unicast_master_list(unit, ptp_id, clock_num, port_num, 
             max_num_masters, num_masters, master_addr);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_static_unicast_master_remove
 * Purpose:
 *      Remove an entry from the static unicast master table.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *master_addr - Unicast master table
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_static_unicast_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_address_t *master_addr)
{
    int rc;

    rc = bcm_common_ptp_static_unicast_master_remove(unit, ptp_id, clock_num, port_num, 
             master_addr);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_static_unicast_master_table_clear
 * Purpose:
 *      Clear (i.e. remove all entries from) the unicast master table 
 *      of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_static_unicast_master_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num)
{
    int rc;

    rc = bcm_common_ptp_static_unicast_master_table_clear(unit, ptp_id, clock_num, port_num);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_static_unicast_master_table_size_get
 * Purpose:
 *      Get maximum number of unicast master table entries of a PTP clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *max_table_entries - Maximum number of unicast master table entries
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_static_unicast_master_table_size_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int *max_table_entries)
{
    int rc;

    rc = bcm_common_ptp_static_unicast_master_table_size_get(unit, ptp_id, clock_num, port_num, 
             max_table_entries);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_static_unicast_slave_add
 * Purpose:
 *      Add a unicast slave to a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *slave_info - Unicast slave information.
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_static_unicast_slave_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_t *slave_info)
{
    int rc;

    rc = bcm_common_ptp_static_unicast_slave_add(unit, ptp_id, clock_num, port_num, slave_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_static_unicast_slave_list
 * Purpose:
 *      List the unicast slaves of a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      max_num_slaves - Maximum number of unicast slave table entries
 *      *num_slaves - Number of unicast slave table entries
 *      *slave_info - Unicast slave table
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_static_unicast_slave_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_slaves,
    int *num_slaves,
    bcm_ptp_clock_peer_t *slave_info)
{
    int rc;

    rc = bcm_common_ptp_static_unicast_slave_list(unit, ptp_id, clock_num, port_num, 
             max_num_slaves, num_slaves, slave_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_static_unicast_slave_remove
 * Purpose:
 *      Remove a unicast slave of a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *slave_info - Unicast slave information
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_static_unicast_slave_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_t *slave_info)
{
    int rc;

    rc = bcm_common_ptp_static_unicast_slave_remove(unit, ptp_id, clock_num, port_num, 
             slave_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_static_unicast_slave_table_clear
 * Purpose:
 *      Clear (i.e. remove all) unicast slaves of a PTP clock port.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_static_unicast_slave_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num)
{
    int rc;

    rc = bcm_common_ptp_static_unicast_slave_table_clear(unit, ptp_id, clock_num, port_num);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_sync_phy
 * Purpose:
 *      Instruct TOP to send a PHY Sync pulse, and mark the time that the PHYs are now synced to.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      sync_input - PTP PHY SYNC inputs
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_sync_phy(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_sync_phy_input_t sync_input)
{
    int rc;

    rc = bcm_common_ptp_sync_phy(unit, ptp_id, clock_num, sync_input);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_bs_time_info_get
 * Purpose:
 *      Instruct TOP to send a PHY Sync pulse, and mark the time that the PHYs are now synced to.
 * Parameters:
 *      unit - Unit number
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *time - BSHB time output
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_bs_time_info_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_bs_time_info_t *time)
{
    int rc;
    rc = bcm_common_ptp_bs_time_info_get(unit, ptp_id, clock_num, time);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_init
 * Purpose:
 *      Initialize the telecom profile.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_init(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_init(unit, ptp_id, clock_num);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_network_option_get
 * Purpose:
 *      Get telecom profile ITU-T G.781 network option.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *network_option - Telecom profile ITU-T G.781 network option
 *          | Disable | Option I | Option II | Option III |
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_network_option_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_network_option_t *network_option)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_network_option_get(unit, ptp_id, clock_num, network_option);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_network_option_set
 * Purpose:
 *      Set telecom profile ITU-T G.781 network option.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      network_option - Telecom profile ITU-T G.781 network option.
 *          | Disable | Option I | Option II | Option III |
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_network_option_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_network_option_t network_option)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_network_option_set(unit, ptp_id, clock_num, network_option);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_add
 * Purpose:
 *      Add a packet master.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *address - Packet master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_port_address_t *address)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_add(unit, ptp_id, clock_num, port_num, 
             address);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_best_get
 * Purpose:
 *      Get current best packet master clock as defined by telecom profile
 *      master selection logic.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *pktmaster - Best packet master clock
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_best_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_pktmaster_t *pktmaster)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_best_get(unit, ptp_id, clock_num, pktmaster);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_get
 * Purpose:
 *      Get packet master by address.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *address - Packet master address
 *      *pktmaster - Packet master
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_port_address_t *address,
    bcm_ptp_telecom_g8265_pktmaster_t *pktmaster)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_get(unit, ptp_id, clock_num, address, 
             pktmaster);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_list
 * Purpose:
 *      Get list of in-use packet masters.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      max_master_count - Maximum number of packet masters
 *      *num_masters - Number of in-use packet masters
 *      *best_master - Best packet master list index
 *      *pktmaster - Packet masters
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int max_master_count,
    int *num_masters,
    int *best_master,
    bcm_ptp_telecom_g8265_pktmaster_t *pktmaster)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_list(unit, ptp_id, clock_num, 
             max_master_count, num_masters, best_master, pktmaster);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_lockout_set
 * Purpose:
 *      Set packet master lockout to exclude from master selection process.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      lockout - Packet master lockout Boolean
 *      *address - Packet master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_lockout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 lockout,
    bcm_ptp_clock_port_address_t *address)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_lockout_set(unit, ptp_id, clock_num, lockout, 
             address);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_non_reversion_set
 * Purpose:
 *      Set packet master non-reversion to control master selection process
 *      if master fails or is unavailable.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      nonres - Packet master non-reversion Boolean
 *      *address - Packet master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_non_reversion_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 nonres,
    bcm_ptp_clock_port_address_t *address)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_non_reversion_set(unit, ptp_id, clock_num, 
             nonres, address);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_priority_override
 * Purpose:
 *      Set priority override of packet master.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      override - Packet master priority override Boolean.
 *      *address - Packet master address
 * Returns:
 *      int
 * Notes: 
 *      Priority override controls whether a packet master's priority value
 *      is set automatically using grandmaster priority1 / priority2 in PTP
 *      announce message (i.e., override equals TRUE) or via host API calls
 *      (i.e., override equals FALSE).
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_priority_override(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 override,
    bcm_ptp_clock_port_address_t *address)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_priority_override(unit, ptp_id, clock_num, 
             override, address);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_priority_set
 * Purpose:
 *      Set priority of packet master.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      priority - Packet master priority
 *      *address - Packet master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_priority_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 priority,
    bcm_ptp_clock_port_address_t *address)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_priority_set(unit, ptp_id, clock_num, 
             priority, address);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_remove
 * Purpose:
 *      Remove a packet master.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *address - Packet master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_port_address_t *address)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_remove(unit, ptp_id, clock_num, port_num, 
             address);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_packet_master_wait_duration_set
 * Purpose:
 *      Set packet master wait-to-restore duration to control master selection
 *      process if master fails or is unavailable.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      wait_sec - Packet master wait-to-restore duration
 *      *address - Packet master address
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_packet_master_wait_duration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint64 wait_sec,
    bcm_ptp_clock_port_address_t *address)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_packet_master_wait_duration_set(unit, ptp_id, clock_num, 
             wait_sec, address);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_pktstats_thresholds_get
 * Purpose:
 *      Get packet timing and PDV statistics thresholds req'd for packet timing
 *      signal fail (PTSF) analysis.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *thresholds - Packet timing and PDV statistics thresholds
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_pktstats_thresholds_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_pktstats_t *thresholds)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_pktstats_thresholds_get(unit, ptp_id, clock_num, 
             thresholds);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_pktstats_thresholds_set
 * Purpose:
 *      Set packet timing and PDV statistics thresholds req'd for packet timing
 *      signal fail (PTSF) analysis.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      thresholds - Packet timing and PDV statistics thresholds
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_pktstats_thresholds_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_pktstats_t thresholds)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_pktstats_thresholds_set(unit, ptp_id, clock_num, 
             thresholds);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_quality_level_set
 * Purpose:
 *      Set the ITU-T G.781 quality level (QL) of local clock.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      ql - Quality level
 * Returns:
 *      int
 * Notes: 
 *      Ref. ITU-T G.781 and ITU-T G.8265.1.
 *      Quality level is mapped to PTP clockClass.
 *      Quality level is used to infer ITU-T G.781 synchronization network
 *      option. ITU-T G.781 synchronization network option must be uniform
 *      among slave and its packet masters.
 */
int
bcm_esw_ptp_telecom_g8265_quality_level_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_quality_level_t ql)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_quality_level_set(unit, ptp_id, clock_num, ql);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_receipt_timeout_get
 * Purpose:
 *      Get PTP |Announce|Sync|Delay_Resp| message receipt timeout req'd for
 *      packet timing signal fail (PTSF) analysis.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      message_type - PTP message type
 *      *receipt_timeout - PTP |Announce|Sync|Delay_Resp| message receipt timeout
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_receipt_timeout_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_message_type_t message_type,
    uint32 *receipt_timeout)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_receipt_timeout_get(unit, ptp_id, clock_num, message_type, 
             receipt_timeout);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_receipt_timeout_set
 * Purpose:
 *      Set PTP |Announce|Sync|Delay_Resp| message receipt timeout req'd for
 *      packet timing signal fail (PTSF) analysis.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      message_type - PTP message type
 *      receipt_timeout - PTP |Announce|Sync|Delay_Resp| message receipt timeout
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_receipt_timeout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_message_type_t message_type,
    uint32 receipt_timeout)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_receipt_timeout_set(unit, ptp_id, clock_num, message_type, 
             receipt_timeout);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_telecom_g8265_shutdown
 * Purpose:
 *      Shut down the telecom profile.
 * Parameters:
 *      unit - Unit number 
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_telecom_g8265_shutdown(
    int unit)
{
    int rc;

    rc = bcm_common_ptp_telecom_g8265_shutdown(unit);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_time_format_set
 * Purpose:
 *      Set Time Of Day format for PTP stack instance.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      type - Time of Day format
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_time_format_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_time_type_t type)
{
    int rc;

    rc = bcm_common_ptp_time_format_set(unit, ptp_id, type);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_timesource_input_status_get
 * Purpose:
 *      Get time source status.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *status - Time source status
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_timesource_input_status_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timesource_status_t *status)
{
    int rc;

    rc = bcm_common_ptp_timesource_input_status_get(unit, ptp_id, clock_num, status);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_tod_input_sources_get
 * Purpose:
 *      Get PTP ToD input source(s).
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *num_tod_sources - (IN) Maximum Number of ToD inputs
 *                         (OUT) Number of ToD inputs
 *      *tod_sources - Array of ToD inputs
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_tod_input_sources_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_tod_sources,
    bcm_ptp_tod_input_t *tod_sources)
{
    int rc;

    rc = bcm_common_ptp_tod_input_sources_get(unit, ptp_id, clock_num, num_tod_sources, 
             tod_sources);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_tod_input_sources_set
 * Purpose:
 *      Set PTP TOD input(s).
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      num_tod_sources - Number of ToD inputs
 *      *tod_sources - PTP ToD input configuration
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_tod_input_sources_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_tod_sources,
    bcm_ptp_tod_input_t *tod_sources)
{
    int rc;

    rc = bcm_common_ptp_tod_input_sources_set(unit, ptp_id, clock_num, num_tod_sources, 
             tod_sources);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_tod_output_get
 * Purpose:
 *      Get PTP TOD output(s).
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *tod_output_count - (IN)  Max number of ToD outputs
 *                          (OUT) Number ToD outputs returned
 *      *tod_output - Array of ToD outputs
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_tod_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *tod_output_count,
    bcm_ptp_tod_output_t *tod_output)
{
    int rc;

    rc = bcm_common_ptp_tod_output_get(unit, ptp_id, clock_num, tod_output_count, tod_output);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_tod_output_remove
 * Purpose:
 *      Remove a ToD output.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      tod_output_id - ToD output ID
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_tod_output_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int tod_output_id)
{
    int rc;

    rc = bcm_common_ptp_tod_output_remove(unit, ptp_id, clock_num, tod_output_id);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_tod_output_set
 * Purpose:
 *      Set PTP ToD output.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *tod_output_id - ToD output ID
 *      *output_info - ToD output configuration
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_tod_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *tod_output_id,
    bcm_ptp_tod_output_t *output_info)
{
    int rc;

    rc = bcm_common_ptp_tod_output_set(unit, ptp_id, clock_num, tod_output_id, output_info);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_transparent_clock_default_dataset_get
 * Purpose:
 *      Get PTP Transparent clock default dataset
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      *data_set - PTP transparent clock default dataset
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_transparent_clock_default_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_transparent_clock_default_dataset_t *data_set)
{
    int rc;

    rc = bcm_common_ptp_transparent_clock_default_dataset_get(unit, ptp_id, clock_num, data_set);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_transparent_clock_port_dataset_get
 * Purpose:
 *      Get PTP transparent clock port dataset
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      clock_port - PTP clock port number
 *      *data_set - PTP transparent clock port dataset
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_transparent_clock_port_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 clock_port,
    bcm_ptp_transparent_clock_port_dataset_t *data_set)
{
    int rc;

    rc = bcm_common_ptp_transparent_clock_port_dataset_get(unit, ptp_id, clock_num, clock_port, 
             data_set);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_unicast_request_duration_get
 * Purpose:
 *      Get durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *duration - durationField (sec)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_unicast_request_duration_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 *duration)
{
    int rc;

    rc = bcm_common_ptp_unicast_request_duration_get(unit, ptp_id, clock_num, port_num, 
             duration);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_unicast_request_duration_max_get
 * Purpose:
 *      Get maximum durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *duration_max - maximum durationField (sec)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_unicast_request_duration_max_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 *duration_max)
{
    int rc;

    rc = bcm_common_ptp_unicast_request_duration_max_get(unit, ptp_id, clock_num, port_num, 
             duration_max);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_unicast_request_duration_max_set
 * Purpose:
 *      Set maximum durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      duration_max - maximum durationField (sec)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_unicast_request_duration_max_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 duration_max)
{
    int rc;

    rc = bcm_common_ptp_unicast_request_duration_max_set(unit, ptp_id, clock_num, port_num, 
             duration_max);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_unicast_request_duration_min_get
 * Purpose:
 *      Set minimum durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      *duration_min - minimum durationField (sec)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_unicast_request_duration_min_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 *duration_min)
{
    int rc;

    rc = bcm_common_ptp_unicast_request_duration_min_get(unit, ptp_id, clock_num, port_num, 
             duration_min);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_unicast_request_duration_min_set
 * Purpose:
 *      Set minimum durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      duration_min - minimum durationField (sec)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_unicast_request_duration_min_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 duration_min)
{
    int rc;

    rc = bcm_common_ptp_unicast_request_duration_min_set(unit, ptp_id, clock_num, port_num, 
             duration_min);

    return (rc);
}

/*
 * Function:
 *      bcm_esw_ptp_unicast_request_duration_set
 * Purpose:
 *      Set durationField of REQUEST_UNICAST_TRANSMISSION TLV.
 * Parameters:
 *      unit - Unit number 
 *      ptp_id - PTP stack ID
 *      clock_num - PTP clock number
 *      port_num - PTP clock port number
 *      duration - durationField (sec)
 * Returns:
 *      int
 * Notes:
 */
int
bcm_esw_ptp_unicast_request_duration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 duration)
{
    int rc;

    rc = bcm_common_ptp_unicast_request_duration_set(unit, ptp_id, clock_num, port_num, 
             duration);

    return (rc);
}


/*
 * Function:
 *      bcm_esmc_tx()
 * Purpose:
 *      ESMC PDU transmit (Tx).
 * Parameters:
 *      unit          - (IN) Unit number.
 *      stack_id      - (IN) Stack identifier index.
 *      pbmp          - (IN) Tx port bitmap.
 *      esmc_pdu_data - (IN) ESMC PDU data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_esmc_tx(
    int unit,
    int stack_id,
    bcm_pbmp_t pbmp,
    bcm_esmc_pdu_data_t *esmc_pdu_data)
{
    int rv;

    rv = bcm_common_esmc_tx(unit, stack_id, pbmp, esmc_pdu_data);

    return rv;
}

/*
 * Function:
 *      bcm_esmc_rx_callback_register()
 * Purpose:
 *      Register ESMC PDU Rx callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      rx_cb    - (IN) ESMC PDU Rx callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_esmc_rx_callback_register(
    int unit,
    int stack_id,
    bcm_esmc_rx_cb rx_cb)
{
    int rv;

    rv = bcm_common_esmc_rx_callback_register(unit, stack_id, rx_cb);

    return rv;
}

/*
 * Function:
 *      bcm_esmc_rx_callback_unregister()
 * Purpose:
 *      Unregister ESMC PDU Rx callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_esmc_rx_callback_unregister(
    int unit,
    int stack_id)
{
    int rv;

    rv = bcm_common_esmc_rx_callback_unregister(unit, stack_id);

    return rv;
}

/*
 * Function:
 *      bcm_esw_tdpll_input_clock_ql_change_callback_register()
 * Purpose:
 *      Register input clock esmc quality change callback.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      ql_change_cb - (IN) Input clock esmc QL change callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      QL change callback generates an event to notify user if a
 *      QL level change happens to take necessary action if required. 
 */
int
bcm_esw_tdpll_input_clock_ql_change_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_ql_change_cb ql_change_cb)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_ql_change_callback_register(unit, stack_id, ql_change_cb);

    return rv;
}

/*
 * Function:
 *      bcm_esw_tdpll_input_clock_ql_change_callback_unregister()
 * Purpose:
 *      Un-register input clock esmc quality change callback.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      QL change callback generates an event to notify user if a
 *      QL level change happens to take necessary action if required. 
 */

int
bcm_esw_tdpll_input_clock_ql_change_callback_unregister(
    int unit,
    int stack_id)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_ql_change_callback_unregister(unit, stack_id);

    return rv;
}


/*
 * Function:
 *      bcm_esmc_tunnel_get()
 * Purpose:
 *      Get ESMC PDU tunneling-enabled Boolean.
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      stack_id - (IN)  PTP stack ID.
 *      enable   - (OUT) Enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Boolean controls whether ToP tunnels slow-protocol Ethertype packets
 *      - which include ESMC PDUs - to host for processing.
 */
int
bcm_esw_esmc_tunnel_get(
    int unit,
    int stack_id,
    int *enable)
{
    int rv;

    rv = bcm_common_esmc_tunnel_get(unit, stack_id, enable);

    return rv;
}

/*
 * Function:
 *      bcm_esmc_tunnel_set()
 * Purpose:
 *      Set ESMC PDU tunneling-enabled Boolean.
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      stack_id - (IN)  PTP stack ID.
 *      enable   - (OUT) Enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Boolean controls whether ToP tunnels slow-protocol Ethertype packets
 *      - which include ESMC PDUs - to host for processing.
 */
int
bcm_esw_esmc_tunnel_set(
    int unit,
    int stack_id,
    int enable)
{
    int rv;

    rv = bcm_common_esmc_tunnel_set(unit, stack_id, enable);

    return rv;
}

/*
 * Function:
 *      bcm_esmc_g781_option_get()
 * Purpose:
 *      Get ITU-T G.781 networking option for SyncE.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      g781_option - (OUT) ITU-T G.781 networking option.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_esmc_g781_option_get(
    int unit,
    int stack_id,
    bcm_esmc_network_option_t *g781_option)
{
    int rv;

    rv = bcm_common_esmc_g781_option_get(unit, stack_id, g781_option);

    return rv;
}

/*
 * Function:
 *      bcm_esmc_g781_option_set()
 * Purpose:
 *      Set ITU-T G.781 networking option for SyncE.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      g781_option - (IN) ITU-T G.781 networking option.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_esmc_g781_option_set(
    int unit,
    int stack_id,
    bcm_esmc_network_option_t g781_option)
{
    int rv;

    rv = bcm_common_esmc_g781_option_set(unit, stack_id, g781_option);

    return rv;
}

/*
 * Function:
 *      bcm_esmc_QL_SSM_map()
 * Purpose:
 *      Get synchronization status message (SSM) code corresponding to
 *      ITU-T G.781 quality level (QL).
 * Parameters:
 *      unit     - (IN) Unit number.
 *      opt      - (IN)  ITU-T G.781 networking option.
 *      ql       - (IN)  ITU-T G.781 quality level.
 *      ssm_code - (OUT) SSM code.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_esmc_QL_SSM_map(
    int unit,
    bcm_esmc_network_option_t opt,
    bcm_esmc_quality_level_t ql,
    uint8 *ssm_code)
{
    int rv;

    rv = bcm_common_esmc_QL_SSM_map(unit, opt, ql, ssm_code);

    return rv;
}

/*
 * Function:
 *      bcm_esmc_SSM_QL_map()
 * Purpose:
 *      Get ITU-T G.781 quality level (QL) corresponding to synchronization
 *      status message (SSM) code.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      opt      - (IN)  ITU-T G.781 networking option.
 *      ssm_code - (IN)  SSM code.
 *      ql       - (OUT) ITU-T G.781 quality level.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_esmc_SSM_QL_map(
    int unit,
    bcm_esmc_network_option_t opt,
    uint8 ssm_code,
    bcm_esmc_quality_level_t *ql)
{
    int rv;

    rv = bcm_common_esmc_SSM_QL_map(unit, opt, ssm_code, ql);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_dpll_bindings_get()
 * Purpose:
 *      Get logical DPLL instance input/output bindings.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      bindings   - (OUT) DPLL instance bindings.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_dpll_bindings_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bindings_t *bindings)
{
    int rv;

    rv = bcm_common_tdpll_dpll_bindings_get(unit, stack_id, dpll_index, bindings);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_dpll_bindings_set()
 * Purpose:
 *      Set logical DPLL instance input/output bindings.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      bindings   - (IN) DPLL instance bindings.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_dpll_bindings_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bindings_t *bindings)
{
    int rv;

    rv = bcm_common_tdpll_dpll_bindings_set(unit, stack_id, dpll_index, bindings);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_dpll_reference_get()
 * Purpose:
 *      Get reference clocks for DPLL instances.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      stack_id     - (IN) Stack identifier index.
 *      max_num_dpll - (IN) Maximum number of DPLL instances.
 *      dpll_ref     - (OUT) DPLL instance reference clocks.
 *      num_dpll     - (OUT) Number of DPLL instances.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Reference switching. Gets current reference clocks of DPLL instances.
 */
int
bcm_esw_tdpll_dpll_reference_get(
    int unit,
    int stack_id,
    int max_num_dpll,
    int *dpll_ref,
    int *num_dpll)
{
    int rv;

    rv = bcm_common_tdpll_dpll_reference_get(unit, stack_id, max_num_dpll, dpll_ref, num_dpll);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_dpll_bandwidth_get()
 * Purpose:
 *      Get DPLL bandwidth.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      bandwidth  - (OUT) DPLL bandwidth.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_dpll_bandwidth_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bandwidth_t *bandwidth)
{
    int rv;

    rv = bcm_common_tdpll_dpll_bandwidth_get(unit, stack_id, dpll_index, bandwidth);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_dpll_bandwidth_set()
 * Purpose:
 *      Set DPLL bandwidth.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      bandwidth  - (IN) DPLL bandwidth.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_dpll_bandwidth_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bandwidth_t *bandwidth)
{
    int rv;

    rv = bcm_common_tdpll_dpll_bandwidth_set(unit, stack_id, dpll_index, bandwidth);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_dpll_phase_control_get()
 * Purpose:
 *      Get DPLL instance's phase control configuration.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      stack_id      - (IN) Stack identifier index.
 *      dpll_index    - (IN) DPLL instance number.
 *      phase_control - (OUT) Phase control configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_dpll_phase_control_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_phase_control_t *phase_control)
{
    int rv;

    rv = bcm_common_tdpll_dpll_phase_control_get(unit, stack_id, dpll_index, phase_control);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_dpll_phase_control_set()
 * Purpose:
 *      Set DPLL instance's phase control configuration.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      stack_id      - (IN) Stack identifier index.
 *      dpll_index    - (IN) DPLL instance number.
 *      phase_control - (IN) Phase control configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_dpll_phase_control_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_phase_control_t *phase_control)
{
    int rv;

    rv = bcm_common_tdpll_dpll_phase_control_set(unit, stack_id, dpll_index, phase_control);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_control()
 * Purpose:
 *      Start/stop T-DPLL input clock monitoring, reference selection, and switching
 *      state machine.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      enable   - (IN) Enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_control(
    int unit,
    int stack_id,
    int enable)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_control(unit, stack_id, enable);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_mac_get()
 * Purpose:
 *      Get MAC address of input clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      mac         - (OUT) Input clock MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_mac_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_mac_t *mac)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_mac_get(unit, stack_id, clock_index, mac);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_mac_set()
 * Purpose:
 *      Set MAC address of input clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      mac         - (IN) Input clock MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_mac_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_mac_t *mac)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_mac_set(unit, stack_id, clock_index, mac);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_frequency_error_get()
 * Purpose:
 *      Get fractional frequency error of an input clock from input-clock
 *      monitoring process.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      stack_id       - (IN) Stack identifier index.
 *      clock_index    - (IN) Input clock index.
 *      freq_error_ppb - (OUT) Input clock fractional frequency error (ppb).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_frequency_error_get(
    int unit,
    int stack_id,
    int clock_index,
    int *freq_error_ppb)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_frequency_error_get(unit, stack_id, clock_index, freq_error_ppb);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_threshold_state_get()
 * Purpose:
 *      Get monitor threshold state of an input clock from input-clock
 *      monitoring process.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      stack_id        - (IN) Stack identifier index.
 *      clock_index     - (IN) Input clock index.
 *      threshold_type  - (IN) Input-clock monitoring threshold type.
 *      threshold_state - (OUT) Input-clock monitoring threshold state Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_threshold_state_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    int *threshold_state)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_threshold_state_get(unit, stack_id, clock_index, threshold_type, threshold_state);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_enable_get()
 * Purpose:
 *      Get input clock enable Boolean.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      enable      - (OUT) Input clock enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_enable_get(
    int unit,
    int stack_id,
    int clock_index,
    int *enable)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_enable_get(unit, stack_id, clock_index, enable);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_enable_set()
 * Purpose:
 *      Set input-clock enable Boolean.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      enable      - (IN) Input clock enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_enable_set(
    int unit,
    int stack_id,
    int clock_index,
    int enable)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_enable_set(unit, stack_id, clock_index, enable);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_l1mux_get()
 * Purpose:
 *      Get L1 mux mapping (mux index and port number) of input clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      l1mux       - (OUT) L1 mux mapping.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_l1mux_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_l1mux_t *l1mux)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_l1mux_get(unit, stack_id, clock_index, l1mux);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_l1mux_set()
 * Purpose:
 *      Set L1 mux mapping (mux index and port number) of input clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      l1mux       - (IN) L1 mux mapping.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_l1mux_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_l1mux_t *l1mux)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_l1mux_set(unit, stack_id, clock_index, l1mux);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_valid_get()
 * Purpose:
 *      Get valid Boolean of an input clock from input-clock monitoring process.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      valid       - (OUT) Input clock valid Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_valid_get(
    int unit,
    int stack_id,
    int clock_index,
    int *valid)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_valid_get(unit, stack_id, clock_index, valid);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_valid_set()
 * Purpose:
 *      Set input-clock valid Boolean from monitoring process.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      valid       - (IN) Input clock valid Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Assignment is transient. Valid Boolean shall be reset by subsequent
 *      input-clock monitoring decision.
 */
int
bcm_esw_tdpll_input_clock_valid_set(
    int unit,
    int stack_id,
    int clock_index,
    int valid)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_valid_set(unit, stack_id, clock_index, valid);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_frequency_get()
 * Purpose:
 *      Get input clock frequency.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      stack_id          - (IN) Stack identifier index.
 *      clock_index       - (IN) Input clock index.
 *      clock_frequency   - (OUT) Frequency (Hz).
 *      tsevent_frequency - (OUT) TS event frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *clock_frequency,
    uint32 *tsevent_frequency)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_frequency_get(unit, stack_id, clock_index, clock_frequency, tsevent_frequency);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_frequency_set()
 * Purpose:
 *      Set input clock frequency.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      stack_id          - (IN) Stack identifier index.
 *      clock_index       - (IN) Input clock index.
 *      clock_frequency   - (IN) Frequency (Hz).
 *      tsevent_frequency - (IN) TS event frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 clock_frequency,
    uint32 tsevent_frequency)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_frequency_set(unit, stack_id, clock_index, clock_frequency, tsevent_frequency);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_ql_get()
 * Purpose:
 *      Get input clock quality level (QL).
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      ql          - (OUT) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_ql_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_esmc_quality_level_t *ql)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_ql_get(unit, stack_id, clock_index, ql);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_ql_set()
 * Purpose:
 *      Set input clock quality level (QL).
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      ql          - (IN) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_ql_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_esmc_quality_level_t ql)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_ql_set(unit, stack_id, clock_index, ql);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_priority_get()
 * Purpose:
 *      Get input clock priority for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      priority    - (OUT) Input clock priority.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_priority_get(
    int unit,
    int stack_id,
    int clock_index,
    int *priority)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_priority_get(unit, stack_id, clock_index, priority);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_priority_set()
 * Purpose:
 *      Set input clock priority for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      priority    - (IN) Input clock priority.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_priority_set(
    int unit,
    int stack_id,
    int clock_index,
    int priority)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_priority_set(unit, stack_id, clock_index, priority);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_lockout_get()
 * Purpose:
 *      Get input clock lockout Boolean for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      lockout     - (OUT) Input clock lockout Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_lockout_get(
    int unit,
    int stack_id,
    int clock_index,
    int *lockout)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_lockout_get(unit, stack_id, clock_index, lockout);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_lockout_set()
 * Purpose:
 *      Set input clock lockout Boolean for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      lockout     - (IN) Input clock lockout Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_lockout_set(
    int unit,
    int stack_id,
    int clock_index,
    int lockout)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_lockout_set(unit, stack_id, clock_index, lockout);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_interval_get()
 * Purpose:
 *      Get input clock monitoring interval.
 * Parameters:
 *      unit             - (IN) Unit number.
 *      stack_id         - (IN) Stack identifier index.
 *      monitor_interval - (OUT) Input clock monitoring interval (sec).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Monitoring interval defines the period over which fractional frequency error
 *      of an input clock is calculated for purposes of threshold-based comparison /
 *      validation.
 */
int
bcm_esw_tdpll_input_clock_monitor_interval_get(
    int unit,
    int stack_id,
    uint32 *monitor_interval)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_monitor_interval_get(unit, stack_id, monitor_interval);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_interval_set()
 * Purpose:
 *      Set input clock monitoring interval.
 * Parameters:
 *      unit             - (IN) Unit number.
 *      stack_id         - (IN) Stack identifier index.
 *      monitor_interval - (IN) Input clock monitoring interval (sec).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Monitoring interval defines the period over which fractional frequency error
 *      of an input clock is calculated for purposes of threshold-based comparison /
 *      validation.
 */
int
bcm_esw_tdpll_input_clock_monitor_interval_set(
    int unit,
    int stack_id,
    uint32 monitor_interval)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_monitor_interval_set(unit, stack_id, monitor_interval);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_threshold_get()
 * Purpose:
 *      Get monitor threshold for input-clock valid classification required
 *      in reference selection.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      stack_id       - (IN) Stack identifier index.
 *      threshold_type - (IN) Input clock monitoring threshold type.
 *      threshold      - (OUT) Input clock monitoring threshold (ppb).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_monitor_threshold_get(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    uint32 *threshold)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_monitor_threshold_get(unit, stack_id, threshold_type, threshold);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_threshold_set()
 * Purpose:
 *      Set monitor threshold for input-clock valid classification required
 *      in reference selection.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      stack_id       - (IN) Stack identifier index.
 *      threshold_type - (IN) Input-clock monitoring threshold type.
 *      threshold      - (IN) Input-clock monitoring threshold (ppb).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_monitor_threshold_set(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    uint32 threshold)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_monitor_threshold_set(unit, stack_id, threshold_type, threshold);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_ql_enabled_get()
 * Purpose:
 *      Get QL-enabled Boolean for reference selection.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql_enabled - (OUT) QL-enabled Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_ql_enabled_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *ql_enabled)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_ql_enabled_get(unit, stack_id, dpll_index, ql_enabled);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_ql_enabled_set()
 * Purpose:
 *      Set QL-enabled Boolean for reference selection.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql_enabled - (IN) QL-enabled Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_ql_enabled_set(
    int unit,
    int stack_id,
    int dpll_index,
    int ql_enabled)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_ql_enabled_set(unit, stack_id, dpll_index, ql_enabled);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_revertive_get()
 * Purpose:
 *      Get revertive mode Boolean for reference selection and switching.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      revertive  - (OUT) Revertive mode Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_revertive_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *revertive)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_revertive_get(unit, stack_id, dpll_index, revertive);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_revertive_set()
 * Purpose:
 *      Set revertive mode Boolean for reference selection and switching.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      revertive  - (IN) Revertive mode Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_input_clock_revertive_set(
    int unit,
    int stack_id,
    int dpll_index,
    int revertive)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_revertive_set(unit, stack_id, dpll_index, revertive);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_best_get()
 * Purpose:
 *      Get best (i.e. selected) reference for a DPLL instance.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      best_clock - (OUT) Best / preferred input clock index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Best / preferred input clock might not be active reference of DPLL
 *      instance, e.g. if revertive option is not set.
 */
int
bcm_esw_tdpll_input_clock_best_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *best_clock)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_best_get(unit, stack_id, dpll_index, best_clock);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_callback_register()
 * Purpose:
 *      Register input clock monitoring callback.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      monitor_cb - (IN) Input clock monitoring callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Input clock monitoring callback generates an event to notify user if
 *      state has changed w.r.t. a threshold criterion.
 */
int
bcm_esw_tdpll_input_clock_monitor_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_cb monitor_cb)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_monitor_callback_register(unit, stack_id, monitor_cb);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_callback_unregister()
 * Purpose:
 *      Unregister input clock monitoring callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Input clock monitoring callback generates an event to notify user if
 *      state has changed w.r.t. a threshold criterion.
 */
int
bcm_esw_tdpll_input_clock_monitor_callback_unregister(
    int unit,
    int stack_id)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_monitor_callback_unregister(unit, stack_id);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_selector_callback_register()
 * Purpose:
 *      Register input clock reference selection callback.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      selector_cb - (IN) Input clock reference selection callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Reference selection callback generates an event to notify user if a
 *      new reference is selected but automatic switching to it is deferred,
 *      because revertive option is not enabled.
 */
int
bcm_esw_tdpll_input_clock_selector_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_selector_cb selector_cb)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_selector_callback_register(unit, stack_id, selector_cb);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_selector_callback_unregister()
 * Purpose:
 *      Unregister input clock reference selection callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Reference selection callback generates an event to notify user if a
 *      new reference is selected but automatic switching to it is deferred,
 *      because revertive option is not enabled.
 */
int
bcm_esw_tdpll_input_clock_selector_callback_unregister(
    int unit,
    int stack_id)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_selector_callback_unregister(unit, stack_id);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_callback_register()
 * Purpose:
 *      Register input clock callback.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      type       - (IN) callback type.
 *      callback   - (IN) Input clock callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 */
int
bcm_esw_tdpll_input_clock_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_callback_type_t type,
    bcm_tdpll_input_clock_cb callback)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_callback_register(unit, stack_id, type, callback);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_callback_unregister()
 * Purpose:
 *      Unregister input clock callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      type       - (IN) callback type.
 * Returns:
 *      BCM_E_XXX - Function status.
 */
int
bcm_esw_tdpll_input_clock_callback_unregister(
    int unit,
    int stack_id,
    bcm_tdpll_callback_type_t type)
{
    int rv;

    rv = bcm_common_tdpll_input_clock_callback_unregister(unit, stack_id, type);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_create()
 * Purpose:
 *      Create the output clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      pbmp        - (IN) Port bit-map.
 *      bandwidth   - (IN) Bandwidth for the DPLL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_create(
    int unit,
    int stack_id,
    int clock_index,
    bcm_pbmp_t pbmp,
    bcm_tdpll_dpll_bandwidth_t *bandwidth)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_create(unit, stack_id, clock_index, pbmp, bandwidth);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_enable_get()
 * Purpose:
 *      Get output clock enable Boolean.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      enable      - (OUT) Output clock enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_enable_get(
    int unit,
    int stack_id,
    int clock_index,
    int *enable)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_enable_get(unit, stack_id, clock_index, enable);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_enable_set()
 * Purpose:
 *      Set output clock enable Boolean.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      enable      - (IN) Output clock enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_enable_set(
    int unit,
    int stack_id,
    int clock_index,
    int enable)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_enable_set(unit, stack_id, clock_index, enable);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_synth_frequency_get()
 * Purpose:
 *      Get output-clock (synthesizer) frequency.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      stack_id          - (IN) Stack identifier index.
 *      clock_index       - (IN) Output clock index.
 *      synth_frequency   - (OUT) Synthesizer frequency (Hz).
 *      tsevent_frequency - (OUT) TS event frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_synth_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *synth_frequency,
    uint32 *tsevent_frequency)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_synth_frequency_get(unit, stack_id, clock_index, synth_frequency, tsevent_frequency);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_synth_frequency_set()
 * Purpose:
 *      Set output-clock (synthesizer) frequency.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      stack_id          - (IN) Stack identifier index.
 *      clock_index       - (IN) Output clock index.
 *      synth_frequency   - (IN) Synthesizer frequency (Hz).
 *      tsevent_frequency - (IN) TS event frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_synth_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 synth_frequency,
    uint32 tsevent_frequency)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_synth_frequency_set(unit, stack_id, clock_index, synth_frequency, tsevent_frequency);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_deriv_frequency_get()
 * Purpose:
 *      Get synthesizer derivative-clock frequency.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      stack_id        - (IN) Stack identifier index.
 *      clock_index     - (IN) Output clock index.
 *      deriv_frequency - (OUT) Derivative clock frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_deriv_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *deriv_frequency)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_deriv_frequency_get(unit, stack_id, clock_index, deriv_frequency);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_deriv_frequency_set()
 * Purpose:
 *      Set synthesizer derivative clock frequency.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      stack_id        - (IN) Stack identifier index.
 *      clock_index     - (IN) Output clock index.
 *      deriv_frequency - (IN) Derivative clock frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_deriv_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 deriv_frequency)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_deriv_frequency_set(unit, stack_id, clock_index, deriv_frequency);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_data_get()
 * Purpose:
 *      Get holdover configuration data.
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      hdata       - (OUT) Holdover configuration data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_holdover_data_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_data_t *hdata)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_holdover_data_get(unit, stack_id, clock_index, hdata);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_frequency_set()
 * Purpose:
 *      Set manual holdover frequency correction.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      hfreq       - (IN) Holdover frequency correction (ppt).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Manual frequency correction is used if holdover mode is manual.
 */
int
bcm_esw_tdpll_output_clock_holdover_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_frequency_correction_t hfreq)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_holdover_frequency_set(unit, stack_id, clock_index, hfreq);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_mode_get()
 * Purpose:
 *      Get holdover mode.
 *      |Manual|Instantaneous|One-Second|Fast Average|Slow Average|
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      hmode       - (OUT) Holdover mode.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_holdover_mode_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_mode_t *hmode)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_holdover_mode_get(unit, stack_id, clock_index, hmode);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_mode_set()
 * Purpose:
 *      Set holdover mode.
 *      |Manual|Instantaneous|Fast Average|Slow Average|
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      hmode       - (IN) Holdover mode.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_holdover_mode_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_mode_t hmode)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_holdover_mode_set(unit, stack_id, clock_index, hmode);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_reset()
 * Purpose:
 *      Reset holdover frequency calculations.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_output_clock_holdover_reset(
    int unit,
    int stack_id,
    int clock_index)
{
    int rv;

    rv = bcm_common_tdpll_output_clock_holdover_reset(unit, stack_id, clock_index);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_rx_state_machine()
 * Purpose:
 *      Execute state machine for Rx ESMC PDU.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      stack_id      - (IN) Stack identifier index.
 *      ingress_port  - (IN) Ingress port.
 *      esmc_pdu_data - (IN) ESMC PDU.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      State machine updates the SSM/QL of a T-DPLL input clock, transmits an
 *      ESMC event PDU upon change to SSM/QL of selected reference clock, etc.
 */
int
bcm_esw_tdpll_esmc_rx_state_machine(
    int unit,
    int stack_id,
    int ingress_port,
    bcm_esmc_pdu_data_t *esmc_pdu_data)
{
    int rv;

    rv = bcm_common_tdpll_esmc_rx_state_machine(unit, stack_id, ingress_port, esmc_pdu_data);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_ql_get()
 * Purpose:
 *      Get quality level (QL) for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql         - (OUT) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_ql_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t *ql)
{
    int rv;

    rv = bcm_common_tdpll_esmc_ql_get(unit, stack_id, dpll_index, ql);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_ql_set()
 * Purpose:
 *      Set quality level (QL) for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql         - (IN) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Assignment is transient. QL shall be overwritten by subsequent
 *      ESMC PDUs received from a T-DPLL selected reference clock.
 */
int
bcm_esw_tdpll_esmc_ql_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t ql)
{
    int rv;

    rv = bcm_common_tdpll_esmc_ql_set(unit, stack_id, dpll_index, ql);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_holdover_ql_get()
 * Purpose:
 *      Get quality level (QL) for ESMC during holdover.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql         - (OUT) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_holdover_ql_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t *ql)
{
    int rv;

    rv = bcm_common_tdpll_esmc_holdover_ql_get(unit, stack_id, dpll_index, ql);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_holdover_ql_set()
 * Purpose:
 *      Set quality level (QL) for ESMC during holdover.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql         - (IN) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_holdover_ql_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t ql)
{
    int rv;

    rv = bcm_common_tdpll_esmc_holdover_ql_set(unit, stack_id, dpll_index, ql);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_mac_get()
 * Purpose:
 *      Get MAC address for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      mac        - (OUT) Local port MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      MAC address is used as source MAC in Tx'd ESMC info and event PDUs.
 *      MAC address is used in discard-from-self logic for reflected PDUs.
 */
int
bcm_esw_tdpll_esmc_mac_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_mac_t *mac)
{
    int rv;

    rv = bcm_common_tdpll_esmc_mac_get(unit, stack_id, dpll_index, mac);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_mac_set()
 * Purpose:
 *      Set MAC address for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      mac        - (IN) Local port MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      MAC address is used as source MAC in Tx'd ESMC info and event PDUs.
 *      MAC address is used in discard-from-self logic for reflected PDUs.
 */
int
bcm_esw_tdpll_esmc_mac_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_mac_t *mac)
{
    int rv;

    rv = bcm_common_tdpll_esmc_mac_set(unit, stack_id, dpll_index, mac);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_rx_enable_get()
 * Purpose:
 *      Get ESMC PDU receive (Rx) enable state.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      enable   - (OUT) ESMC PDU receive enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_rx_enable_get(
    int unit,
    int stack_id,
    int *enable)
{
    int rv;

    rv = bcm_common_tdpll_esmc_rx_enable_get(unit, stack_id, enable);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_rx_enable_set()
 * Purpose:
 *      Set ESMC PDU receive (Rx) enable state.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      enable   - (IN) ESMC PDU receive enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_rx_enable_set(
    int unit,
    int stack_id,
    int enable)
{
    int rv;

    rv = bcm_common_tdpll_esmc_rx_enable_set(unit, stack_id, enable);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_tx_enable_get()
 * Purpose:
 *      Get ESMC PDU transmit (Tx) enable state.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      enable     - (OUT) ESMC PDU transmit enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_tx_enable_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *enable)
{
    int rv;

    rv = bcm_common_tdpll_esmc_tx_enable_get(unit, stack_id, dpll_index, enable);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_tx_enable_set()
 * Purpose:
 *      Set ESMC PDU transmit (Tx) enable state.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      enable     - (IN) ESMC PDU transmit enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_tx_enable_set(
    int unit,
    int stack_id,
    int dpll_index,
    int enable)
{
    int rv;

    rv = bcm_common_tdpll_esmc_tx_enable_set(unit, stack_id, dpll_index, enable);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_rx_portbitmap_get()
 * Purpose:
 *      Get port bitmap for ESMC Rx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      pbmp       - (OUT) Rx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_rx_portbitmap_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t *pbmp)
{
    int rv;

    rv = bcm_common_tdpll_esmc_rx_portbitmap_get(unit, stack_id, dpll_index, pbmp);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_rx_portbitmap_set()
 * Purpose:
 *      Set port bitmap for ESMC Rx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      pbmp       - (IN) Rx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_rx_portbitmap_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t pbmp)
{
    int rv;

    rv = bcm_common_tdpll_esmc_rx_portbitmap_set(unit, stack_id, dpll_index, pbmp);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_tx_portbitmap_get()
 * Purpose:
 *      Get port bitmap for ESMC Tx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      pbmp       - (OUT) Tx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_tx_portbitmap_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t *pbmp)
{
    int rv;

    rv = bcm_common_tdpll_esmc_tx_portbitmap_get(unit, stack_id, dpll_index, pbmp);

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_esmc_tx_portbitmap_set()
 * Purpose:
 *      Set port bitmap for ESMC Tx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      pbmp       - (IN) Tx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_tx_portbitmap_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t pbmp)
{
    int rv;

    rv = bcm_common_tdpll_esmc_tx_portbitmap_set(unit, stack_id, dpll_index, pbmp);

    return rv;
}

/*
 * Function:
 *      bcm_esw_tdpll_esmc_essm_code_set()
 * Purpose:
 *      Get port bitmap for ESMC Tx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      essm code - (IN) synce clock type .
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_essm_code_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_essm_code_t essm_code)
{
    int rv;

    rv = bcm_common_tdpll_esmc_essm_code_set(unit, stack_id, dpll_index, essm_code);

    return rv;
}

/*
 * Function:
 *      bcm_esw_tdpll_esmc_essm_code_get()
 * Purpose:
 *      Set port bitmap for ESMC Tx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      essm_code    - (OUT) synce clock type .
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_tdpll_esmc_essm_code_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_essm_code_t *essm_code)
{
    int rv;

    rv = bcm_common_tdpll_esmc_essm_code_get(unit, stack_id, dpll_index, essm_code);

    return rv;
}

/* Set flags to be used for overriding BMCA */
int bcm_esw_ptp_clock_bmca_override_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 flags,
    uint32 enable_mask)
{
    int rv;

    rv = bcm_common_clock_bmca_override_set(unit, ptp_id, clock_num, flags, enable_mask);

    return rv;
}

/* Get flags to be used for overriding BMCA */
int bcm_esw_ptp_clock_bmca_override_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *flags,
    uint32 *enable_mask)
{
    int rv;

    rv = bcm_common_clock_bmca_override_get(unit, ptp_id, clock_num, flags, enable_mask);

    return rv;
}

/*
 * This API is used to enable/disable the APTS automatic switching
 * feature.
 */
int bcm_esw_ptp_clock_apts_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *enable)
{
    int rv;
    rv = bcm_common_ptp_clock_apts_enable_get(unit, ptp_id, clock_num, enable);
    return rv;
}

/*
 * This API is used to enable/disable the APTS automatic switching
 * feature.
 */
int bcm_esw_ptp_clock_apts_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable)
{
    int rv;
    rv = bcm_common_ptp_clock_apts_enable_set(unit, ptp_id, clock_num, enable);
    return rv;
}

/*
 * This API is used to get the priority order for time and frequency
 * sources.
 */
int bcm_esw_ptp_clock_apts_source_prio_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_sources,
    bcm_ptp_clock_apts_source_t *priority_list)
{
    int rv;
    rv = bcm_common_ptp_clock_apts_source_prio_get(unit, ptp_id, clock_num,
            num_sources, priority_list);
    return rv;
}

/*
 * This API is used to get the priority order for time and frequency
 * sources.
 */
int bcm_esw_ptp_clock_apts_source_prio_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_sources,
    bcm_ptp_clock_apts_source_t *priority_list)
{
    int rv;
    rv = bcm_common_ptp_clock_apts_source_prio_set(unit, ptp_id, clock_num,
                num_sources, priority_list);
    return rv;
}

/*
 *  This API can be used by the user application to retrieve the current
 * operating mode of the APTS system.
 */
int bcm_esw_ptp_clock_apts_mode_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_apts_mode_t *current_mode)
{
    int rv;
    rv = bcm_common_ptp_clock_apts_mode_get(unit, ptp_id, clock_num,
            current_mode);
    return rv;
}

/*
 * This API is used by the to indicate if a time and/or frequency source
 * is available or not.
 */
int bcm_esw_ptp_clock_apts_source_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_apts_source_t source,
    int *enable)
{
    int rv;
    rv = bcm_common_ptp_clock_apts_source_enable_get(unit, ptp_id, clock_num,
            source, enable);
    return rv;
}

/*
 * This API is used by the to indicate if a time and/or frequency source
 * is available or not.
 */
int bcm_esw_ptp_clock_apts_source_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_apts_source_t source,
    int enable)
{
    int rv;
    rv = bcm_common_ptp_clock_apts_source_enable_set(unit, ptp_id, clock_num,
            source, enable);
    return rv;
}

/*
 * Function:
 *      bcm_esw_ptp_external_phy_synchronize()
 * Purpose:
 *      Run state machine to synchronize CMIC time with external PHY time.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      clock_num - (IN) clock index.
 *      extphy_config - (IN) external PHY config.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_external_phy_synchronize(
    int unit,
    int stack_id,
    int clock_num,
    bcm_ptp_external_phy_config_t *extphy_config)
{
    int rv;

    rv = bcm_common_ptp_external_phy_synchronize(unit, stack_id, clock_num, extphy_config);
    return rv;
}

int 
bcm_esw_tdpll_event_callback_register(
    int unit, 
    int ptp_id, 
    bcm_tdpll_event_cb_f callback)
{
    int rv;
    rv = bcm_common_tdpll_event_callback_register(unit, ptp_id, callback);
    return rv;
}

int 
bcm_esw_tdpll_event_callback_unregister(
    int unit, 
    int ptp_id)
{
    int rv;
    rv = bcm_common_tdpll_event_callback_unregister(unit, ptp_id);
    return rv;
}

/*
 * This API is used to configure a fixed offset to compensate for delays
 * in the GPS receiver path
 */
int bcm_esw_ptp_clock_apts_gps_offset_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int offset_nsec)
{
    int rv;
    rv = bcm_common_ptp_clock_apts_gps_offset_set(unit, ptp_id, clock_num, offset_nsec);
    return rv;
}

/*
 * This API is used to configure a fixed offset to compensate for delays
 * in the GPS receiver path
 */
int bcm_esw_ptp_clock_apts_gps_offset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *offset_nsec)
{
    int rv;
    rv = bcm_common_ptp_clock_apts_gps_offset_get(unit, ptp_id, clock_num, offset_nsec);
    return rv;
}

/* Add a new TLV config */
int bcm_esw_ptp_clock_port_tlv_config_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port,
    bcm_ptp_tlv_type_t tlv_type,
    uint16 data_len,
    uint8 *data,
    bcm_ptp_tlv_action_t action,
    int config_id)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_common_ptp_clock_port_tlv_config_add(unit, ptp_id, clock_index, clock_port, tlv_type, data_len, data, action, config_id);

    return rv;
}

/* delete existing TLV config */
int bcm_esw_ptp_clock_port_tlv_config_delete(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port,
    int config_id)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_common_ptp_clock_port_tlv_config_delete(unit, ptp_id, clock_index, clock_port, config_id);

    return rv;
}


/* delete all TLV config */
int bcm_esw_ptp_clock_port_tlv_config_delete_all(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_common_ptp_clock_port_tlv_config_delete_all(unit, ptp_id, clock_index, clock_port);

    return rv;
}


/* Enable or disable a config */
int bcm_esw_ptp_clock_tlv_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port,
    int config_id,
    int enable)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_common_ptp_clock_tlv_enable_set(unit, ptp_id, clock_index, clock_port, config_id, enable);

    return rv;
}


/* Get the status of a config */
int bcm_esw_ptp_clock_tlv_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_index,
    uint32 clock_port,
    int config_id,
    int *enable)
{
    int rv = BCM_E_UNAVAIL;

    rv = bcm_common_ptp_clock_tlv_enable_get(unit, ptp_id, clock_index, clock_port, config_id, enable);

    return rv;
}

#endif /* defined(INCLUDE_PTP)*/

