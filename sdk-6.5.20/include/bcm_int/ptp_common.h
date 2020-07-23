/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	ptp_common.h
 * Purpose:	Common PTP API functions
 */

#if defined(INCLUDE_PTP)

#include <soc/defs.h>
#include <soc/drv.h>

#include <sal/core/dpc.h>

#include <bcm/ptp.h>
#include <bcm/error.h>


#ifdef BCM_PTP_EXT_SERVO_SUPPORT
#define IDT_BC_CLOCK
#define IDT_COMPILE_MAX_NUM_STACK_INSTANCES BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES 

#include <bcm_int/common/ptp.h>

#include "cmnInitConfiguration.h"
#include "cmnReferenceTrackerTypes.h"
#include "mngDeviceAdaptorTS.h"
#include "mngApi/mngApi.h"
#include "mngUserCallbacks.h"
#include "stackAdaptorApi/mngConfigApi.h"

#endif /* BCM_PTP_EXT_SERVO_SUPPORT */


extern int bcm_common_ptp_acceptable_master_add(int,bcm_ptp_stack_id_t,int,int,int,bcm_ptp_clock_peer_address_t *);
extern int bcm_common_ptp_acceptable_master_enabled_get(int,bcm_ptp_stack_id_t,int,int,uint8 *);
extern int bcm_common_ptp_acceptable_master_enabled_set(int,bcm_ptp_stack_id_t,int,int,uint8);
extern int bcm_common_ptp_acceptable_master_list(int,bcm_ptp_stack_id_t,int,int,int,int *,bcm_ptp_clock_peer_address_t *);
extern int bcm_common_ptp_acceptable_master_remove(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_clock_peer_address_t *);
extern int bcm_common_ptp_acceptable_master_table_clear(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_acceptable_master_table_size_get(int,bcm_ptp_stack_id_t,int,int,int *);
extern int bcm_common_ptp_cb_register(int,bcm_ptp_cb_types_t,bcm_ptp_cb,void *);
extern int bcm_common_ptp_cb_unregister(int,bcm_ptp_cb_types_t,bcm_ptp_cb);
extern int bcm_common_ptp_clock_accuracy_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_clock_accuracy_t *);
extern int bcm_common_ptp_clock_accuracy_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_clock_accuracy_t *);
extern int bcm_common_ptp_clock_quality_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_clock_quality_t *, uint32);
extern int bcm_common_ptp_clock_quality_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_clock_quality_t, uint32);
extern int bcm_common_ptp_g8275p1_traceability_info_clock_class_map(int,bcm_ptp_stack_id_t,int,bcm_ptp_g8275p1_clock_traceability_info_t, uint8 *);
extern int bcm_common_ptp_clock_create(int,bcm_ptp_stack_id_t,bcm_ptp_clock_info_t *);
extern int bcm_common_ptp_clock_current_dataset_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_current_dataset_t *);
extern int bcm_common_ptp_clock_default_dataset_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_default_dataset_t *);
extern int bcm_common_ptp_clock_domain_get(int,bcm_ptp_stack_id_t,int,uint32 *);
extern int bcm_common_ptp_clock_domain_set(int,bcm_ptp_stack_id_t,int,uint32);
extern int bcm_common_ptp_clock_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_clock_info_t *);
extern int bcm_common_ptp_clock_parent_dataset_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_parent_dataset_t *);
extern int bcm_common_ptp_phase_offset_set(int,bcm_ptp_stack_id_t,int,const bcm_ptp_phase_offset_t *); 
extern int bcm_common_ptp_phase_offset_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_phase_offset_t *); 
extern int bcm_common_ptp_clock_port_announce_receipt_timeout_get(int,bcm_ptp_stack_id_t,int,uint32,uint32 *);
extern int bcm_common_ptp_clock_port_announce_receipt_timeout_set(int,bcm_ptp_stack_id_t,int,uint32,uint32);
extern int bcm_common_ptp_clock_port_configure(int,bcm_ptp_stack_id_t,int,uint32,bcm_ptp_clock_port_info_t *);
extern int bcm_common_ptp_clock_port_dataset_get(int,bcm_ptp_stack_id_t,int,uint32,bcm_ptp_port_dataset_t *);
extern int bcm_common_ptp_clock_port_delay_mechanism_get(int,bcm_ptp_stack_id_t,int,uint32,uint32 *);
extern int bcm_common_ptp_clock_port_delay_mechanism_set(int,bcm_ptp_stack_id_t,int,uint32,uint32);
extern int bcm_common_ptp_clock_port_disable(int,bcm_ptp_stack_id_t,int,uint32);
extern int bcm_common_ptp_clock_port_enable(int,bcm_ptp_stack_id_t,int,uint32);
extern int bcm_common_ptp_clock_port_identity_get(int,bcm_ptp_stack_id_t,int,uint32,bcm_ptp_port_identity_t *);
extern int bcm_common_ptp_clock_port_info_get(int,bcm_ptp_stack_id_t,int,uint32,bcm_ptp_clock_port_info_t *);
extern int bcm_common_ptp_clock_port_latency_get(int,bcm_ptp_stack_id_t,int,uint32,uint32 *,uint32 *);
extern int bcm_common_ptp_clock_port_latency_set(int,bcm_ptp_stack_id_t,int,uint32,uint32,uint32);
extern int bcm_common_ptp_clock_port_log_announce_interval_get(int,bcm_ptp_stack_id_t,int,uint32,int *);
extern int bcm_common_ptp_clock_port_log_announce_interval_set(int,bcm_ptp_stack_id_t,int,uint32,int);
extern int bcm_common_ptp_clock_port_log_min_delay_req_interval_get(int,bcm_ptp_stack_id_t,int,uint32,int *);
extern int bcm_common_ptp_clock_port_log_min_delay_req_interval_set(int,bcm_ptp_stack_id_t,int,uint32,int);
extern int bcm_common_ptp_clock_port_log_min_pdelay_req_interval_get(int,bcm_ptp_stack_id_t,int,uint32,int *);
extern int bcm_common_ptp_clock_port_log_min_pdelay_req_interval_set(int,bcm_ptp_stack_id_t,int,uint32,int);
extern int bcm_common_ptp_clock_port_log_sync_interval_get(int,bcm_ptp_stack_id_t,int,uint32,int *);
extern int bcm_common_ptp_clock_port_log_sync_interval_set(int,bcm_ptp_stack_id_t,int,uint32,int);
extern int bcm_common_ptp_clock_port_mac_get(int,bcm_ptp_stack_id_t,int,uint32,bcm_mac_t *);
extern int bcm_common_ptp_clock_port_protocol_get(int,bcm_ptp_stack_id_t,int,uint32,bcm_ptp_protocol_t *);
extern int bcm_common_ptp_clock_port_type_get(int,bcm_ptp_stack_id_t,int,uint32,bcm_ptp_port_type_t *);
extern int bcm_common_ptp_clock_port_version_number_get(int,bcm_ptp_stack_id_t,int,uint32,uint32 *);
extern int bcm_common_ptp_clock_priority1_get(int,bcm_ptp_stack_id_t,int,uint32 *);
extern int bcm_common_ptp_clock_priority1_set(int,bcm_ptp_stack_id_t,int,uint32);
extern int bcm_common_ptp_clock_priority2_get(int,bcm_ptp_stack_id_t,int,uint32 *);
extern int bcm_common_ptp_clock_priority2_set(int,bcm_ptp_stack_id_t,int,uint32);
extern int bcm_common_ptp_clock_local_priority_get(int,bcm_ptp_stack_id_t,int,uint8 *);
extern int bcm_common_ptp_clock_local_priority_set(int,bcm_ptp_stack_id_t,int,uint8);
extern int bcm_common_ptp_clock_max_steps_removed_get(int,bcm_ptp_stack_id_t,int,uint8 *);
extern int bcm_common_ptp_clock_max_steps_removed_set(int,bcm_ptp_stack_id_t,int,uint8);
extern int bcm_common_ptp_clock_peer_age_timer_get(int,bcm_ptp_stack_id_t,int,uint8 *);
extern int bcm_common_ptp_clock_peer_age_timer_set(int,bcm_ptp_stack_id_t,int,uint8);
extern int bcm_common_ptp_pkt_flags_override_get(int,bcm_ptp_stack_id_t,int,uint32 *,uint32 *);
extern int bcm_common_ptp_pkt_flags_override_set(int,bcm_ptp_stack_id_t,int,uint32,uint32);

extern int bcm_common_ptp_clock_slaveonly_get(int,bcm_ptp_stack_id_t,int,uint32 *);
extern int bcm_common_ptp_clock_slaveonly_set(int,bcm_ptp_stack_id_t,int,uint32);
extern int bcm_common_ptp_clock_time_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_timestamp_t *);
extern int bcm_common_ptp_clock_time_properties_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_time_properties_t *);
extern int bcm_common_ptp_clock_time_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_timestamp_t *);
extern int bcm_common_ptp_clock_timescale_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_timescale_t *);
extern int bcm_common_ptp_clock_timescale_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_timescale_t *);
extern int bcm_common_ptp_clock_traceability_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_trace_t *);
extern int bcm_common_ptp_clock_traceability_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_trace_t *);
extern int bcm_common_ptp_clock_user_description_set(int,bcm_ptp_stack_id_t,int,uint8 *);
extern int bcm_common_ptp_clock_utc_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_utc_t *);
extern int bcm_common_ptp_clock_utc_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_utc_t *);
extern int bcm_common_ptp_ctdev_alarm_callback_register(int,bcm_ptp_stack_id_t,int,bcm_ptp_ctdev_alarm_cb);
extern int bcm_common_ptp_ctdev_alarm_callback_unregister(int,bcm_ptp_stack_id_t,int);
extern int bcm_common_ptp_ctdev_alpha_get(int,bcm_ptp_stack_id_t,int,uint16 *,uint16 *);
extern int bcm_common_ptp_ctdev_alpha_set(int,bcm_ptp_stack_id_t,int,uint16,uint16);
extern int bcm_common_ptp_ctdev_enable_get(int,bcm_ptp_stack_id_t,int,int *,uint32 *);
extern int bcm_common_ptp_ctdev_enable_set(int,bcm_ptp_stack_id_t,int,int,uint32);
extern int bcm_common_ptp_ctdev_verbose_get(int,bcm_ptp_stack_id_t,int,int *);
extern int bcm_common_ptp_ctdev_verbose_set(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_detach(int);
extern int bcm_common_ptp_foreign_master_dataset_get(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_foreign_master_dataset_t *);
extern int bcm_common_ptp_lock(int);
extern int bcm_common_ptp_unlock(int);
extern int bcm_common_ptp_init(int);
extern int bcm_common_ptp_input_channel_precedence_mode_set(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_input_channel_switching_mode_set(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_input_channels_get(int,bcm_ptp_stack_id_t,int,int *,bcm_ptp_channel_t *);
extern int bcm_common_ptp_input_channels_set(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_channel_t *);
extern int bcm_common_ptp_modular_enable_get(int,bcm_ptp_stack_id_t,int,int *,uint32 *);
extern int bcm_common_ptp_modular_enable_set(int,bcm_ptp_stack_id_t,int,int,uint32);
extern int bcm_common_ptp_modular_phyts_get(int,bcm_ptp_stack_id_t,int,int *,int *);
extern int bcm_common_ptp_modular_phyts_set(int,bcm_ptp_stack_id_t,int,int,int);
extern int bcm_common_ptp_modular_portbitmap_get(int,bcm_ptp_stack_id_t,int,bcm_pbmp_t *);
extern int bcm_common_ptp_modular_portbitmap_set(int,bcm_ptp_stack_id_t,int,bcm_pbmp_t);
extern int bcm_common_ptp_modular_verbose_get(int,bcm_ptp_stack_id_t,int,int *);
extern int bcm_common_ptp_modular_verbose_set(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_packet_counters_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_packet_counters_t *);
extern int bcm_common_ptp_packet_counters_clear(int,bcm_ptp_stack_id_t,int,int,uint32);
extern int bcm_common_ptp_peer_dataset_get(int, bcm_ptp_stack_id_t,int,int,int,bcm_ptp_peer_entry_t *,int *);
extern int bcm_common_ptp_primary_domain_get(int,bcm_ptp_stack_id_t,int,int *);
extern int bcm_common_ptp_primary_domain_set(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_servo_configuration_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_servo_config_t *);
extern int bcm_common_ptp_servo_configuration_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_servo_config_t *);
extern int bcm_common_ptp_servo_status_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_servo_status_t *);
extern int bcm_common_ptp_servo_threshold_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_servo_threshold_t *);
extern int bcm_common_ptp_servo_threshold_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_servo_threshold_t *);
extern int bcm_common_ptp_signal_output_get(int,bcm_ptp_stack_id_t,int,int *,bcm_ptp_signal_output_t *);
extern int bcm_common_ptp_signal_output_remove(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_signal_output_set(int,bcm_ptp_stack_id_t,int,int *,bcm_ptp_signal_output_t *);
extern int bcm_common_ptp_signaled_unicast_master_add(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_clock_unicast_master_t *,uint32);
extern int bcm_common_ptp_signaled_unicast_master_remove(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_clock_peer_address_t *);
extern int bcm_common_ptp_signaled_unicast_slave_list(int,bcm_ptp_stack_id_t,int,int,int,int *,bcm_ptp_clock_peer_t *);
extern int bcm_common_ptp_signaled_unicast_slave_table_clear(int,bcm_ptp_stack_id_t,int,int,int);
extern int bcm_common_ptp_stack_create(int,bcm_ptp_stack_info_t *);
extern int bcm_common_ptp_stack_destroy(int,bcm_ptp_stack_id_t);
extern int bcm_common_ptp_stack_get(int,bcm_ptp_stack_id_t, bcm_ptp_stack_info_t *);
extern int bcm_common_ptp_stack_get_all(int,int,bcm_ptp_stack_info_t *,int *);
extern int bcm_common_ptp_static_unicast_master_add(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_clock_unicast_master_t *);
extern int bcm_common_ptp_static_unicast_master_list(int,bcm_ptp_stack_id_t,int,int,int,int *,bcm_ptp_clock_peer_address_t *);
extern int bcm_common_ptp_static_unicast_master_remove(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_clock_peer_address_t *);
extern int bcm_common_ptp_static_unicast_master_table_clear(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_static_unicast_master_table_size_get(int,bcm_ptp_stack_id_t,int,int,int *);
extern int bcm_common_ptp_static_unicast_slave_add(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_clock_peer_t *);
extern int bcm_common_ptp_static_unicast_slave_list(int,bcm_ptp_stack_id_t,int,int,int,int *,bcm_ptp_clock_peer_t *);
extern int bcm_common_ptp_static_unicast_slave_remove(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_clock_peer_t *);
extern int bcm_common_ptp_static_unicast_slave_table_clear(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_sync_phy(int,bcm_ptp_stack_id_t,int,bcm_ptp_sync_phy_input_t);
extern int bcm_common_ptp_bs_time_info_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_bs_time_info_t *);
extern int bcm_common_ptp_telecom_g8265_init(int,bcm_ptp_stack_id_t,int);
extern int bcm_common_ptp_telecom_g8265_network_option_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_telecom_g8265_network_option_t *);
extern int bcm_common_ptp_telecom_g8265_network_option_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_telecom_g8265_network_option_t);
extern int bcm_common_ptp_telecom_g8265_packet_master_add(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_clock_port_address_t *);
extern int bcm_common_ptp_telecom_g8265_packet_master_best_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_telecom_g8265_pktmaster_t *);
extern int bcm_common_ptp_telecom_g8265_packet_master_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_clock_port_address_t *,bcm_ptp_telecom_g8265_pktmaster_t *);
extern int bcm_common_ptp_telecom_g8265_packet_master_list(int,bcm_ptp_stack_id_t,int,int,int *,int *,bcm_ptp_telecom_g8265_pktmaster_t *);
extern int bcm_common_ptp_telecom_g8265_packet_master_lockout_set(int,bcm_ptp_stack_id_t,int,uint8,bcm_ptp_clock_port_address_t *);
extern int bcm_common_ptp_telecom_g8265_packet_master_non_reversion_set(int,bcm_ptp_stack_id_t,int,uint8,bcm_ptp_clock_port_address_t *);
extern int bcm_common_ptp_telecom_g8265_packet_master_priority_override(int,bcm_ptp_stack_id_t,int,uint8,bcm_ptp_clock_port_address_t *);
extern int bcm_common_ptp_telecom_g8265_packet_master_priority_set(int,bcm_ptp_stack_id_t,int,uint16,bcm_ptp_clock_port_address_t *);
extern int bcm_common_ptp_telecom_g8265_packet_master_remove(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_clock_port_address_t *);
extern int bcm_common_ptp_telecom_g8265_packet_master_wait_duration_set(int,bcm_ptp_stack_id_t,int,uint64,bcm_ptp_clock_port_address_t *);
extern int bcm_common_ptp_telecom_g8265_pktstats_thresholds_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_telecom_g8265_pktstats_t *);
extern int bcm_common_ptp_telecom_g8265_pktstats_thresholds_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_telecom_g8265_pktstats_t);
extern int bcm_common_ptp_telecom_g8265_quality_level_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_telecom_g8265_quality_level_t);
extern int bcm_common_ptp_telecom_g8265_receipt_timeout_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_message_type_t,uint32 *);
extern int bcm_common_ptp_telecom_g8265_receipt_timeout_set(int,bcm_ptp_stack_id_t,int,bcm_ptp_message_type_t,uint32);
extern int bcm_common_ptp_telecom_g8265_shutdown(int);
extern int bcm_common_ptp_time_format_set(int,bcm_ptp_stack_id_t,bcm_ptp_time_type_t);
extern int bcm_common_ptp_timesource_input_status_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_timesource_status_t *);
extern int bcm_common_ptp_tod_input_sources_get(int,bcm_ptp_stack_id_t,int,int *,bcm_ptp_tod_input_t *);
extern int bcm_common_ptp_tod_input_sources_set(int,bcm_ptp_stack_id_t,int,int,bcm_ptp_tod_input_t *);
extern int bcm_common_ptp_tod_output_get(int,bcm_ptp_stack_id_t,int,int *,bcm_ptp_tod_output_t *);
extern int bcm_common_ptp_tod_output_remove(int,bcm_ptp_stack_id_t,int,int);
extern int bcm_common_ptp_tod_output_set(int,bcm_ptp_stack_id_t,int,int *,bcm_ptp_tod_output_t *);
extern int bcm_common_ptp_synce_output_set(int,bcm_ptp_stack_id_t,int,int,int);
extern int bcm_common_ptp_synce_output_get(int,bcm_ptp_stack_id_t,int,int *);
extern int bcm_common_ptp_transparent_clock_default_dataset_get(int,bcm_ptp_stack_id_t,int,bcm_ptp_transparent_clock_default_dataset_t *);
extern int bcm_common_ptp_transparent_clock_port_dataset_get(int,bcm_ptp_stack_id_t,int,uint16,bcm_ptp_transparent_clock_port_dataset_t *);
extern int bcm_common_ptp_unicast_request_duration_get(int,bcm_ptp_stack_id_t,int,int,uint32 *);
extern int bcm_common_ptp_unicast_request_duration_max_get(int,bcm_ptp_stack_id_t,int,int,uint32 *);
extern int bcm_common_ptp_unicast_request_duration_max_set(int,bcm_ptp_stack_id_t,int,int,uint32);
extern int bcm_common_ptp_unicast_request_duration_min_get(int,bcm_ptp_stack_id_t,int,int,uint32 *);
extern int bcm_common_ptp_unicast_request_duration_min_set(int,bcm_ptp_stack_id_t,int,int,uint32);
extern int bcm_common_ptp_unicast_request_duration_set(int,bcm_ptp_stack_id_t,int,int,uint32);
extern int bcm_common_ptp_clock_port_drop_counters_enable_set( int ,bcm_ptp_stack_id_t ,int ,uint32 ,int );
extern int bcm_common_ptp_clock_port_drop_counters_get(int ,bcm_ptp_stack_id_t ,int ,uint32 ,bcm_ptp_clock_port_packet_drop_counters_t *);
extern int bcm_common_clock_bmca_override_set(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, uint32 flags, uint32 enable_mask);
extern int bcm_common_clock_bmca_override_get(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, uint32 *flags, uint32 *enable_mask);
extern int bcm_common_ptp_clock_apts_enable_get(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, int *enable);
extern int bcm_common_ptp_clock_apts_enable_set(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, int enable);
extern int bcm_common_ptp_clock_apts_source_prio_get(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, int *num_sources, bcm_ptp_clock_apts_source_t *priority_list);
extern int bcm_common_ptp_clock_apts_source_prio_set(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, int num_sources, bcm_ptp_clock_apts_source_t *priority_list);
extern int bcm_common_ptp_clock_apts_mode_get(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, bcm_ptp_clock_apts_mode_t *current_mode);
extern int bcm_common_ptp_clock_apts_source_enable_get(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, bcm_ptp_clock_apts_source_t source, int *enable);
extern int bcm_common_ptp_clock_apts_source_enable_set(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, bcm_ptp_clock_apts_source_t source, int enable);
extern int bcm_common_ptp_clock_apts_gps_offset_set(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, int offset_nsec);
extern int bcm_common_ptp_clock_apts_gps_offset_get(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, int *offset_nsec);
extern int bcm_common_ptp_clock_flags_set( int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, uint32 clock_flags);
extern int bcm_common_ptp_clock_port_tlv_config_add(int unit, bcm_ptp_stack_id_t ptp_id, int clock_index, uint32 clock_port, bcm_ptp_tlv_type_t tlv_type, uint16 data_len, uint8 *data, bcm_ptp_tlv_action_t action, int config_id);
extern int bcm_common_ptp_clock_port_tlv_config_delete(int unit, bcm_ptp_stack_id_t ptp_id, int clock_index, uint32 clock_port, int config_id);
extern int bcm_common_ptp_clock_port_tlv_config_delete_all(int unit, bcm_ptp_stack_id_t ptp_id, int clock_index, uint32 clock_port);
extern int bcm_common_ptp_clock_tlv_enable_set(int unit, bcm_ptp_stack_id_t ptp_id, int clock_index, uint32 clock_port, int config_id, int enable);
extern int bcm_common_ptp_clock_tlv_enable_get(int unit, bcm_ptp_stack_id_t ptp_id, int clock_index, uint32 clock_port, int config_id, int *enable);


extern int bcm_common_esmc_tx(int, int, bcm_pbmp_t, bcm_esmc_pdu_data_t *);
#if defined (BCM_QAX_SUPPORT) || defined (BCM_QUX_SUPPORT)
extern void bcm_esmc_pdu_tx_dnx(int, uint8 *, int);
#endif
extern int bcm_common_esmc_rx_callback_register(int, int, bcm_esmc_rx_cb);
extern int bcm_common_esmc_rx_callback_unregister(int, int);
extern int bcm_common_esmc_tunnel_get(int, int, int *);
extern int bcm_common_esmc_tunnel_set(int, int, int);
extern int bcm_common_esmc_g781_option_get(int, int, bcm_esmc_network_option_t *);
extern int bcm_common_esmc_g781_option_set(int, int, bcm_esmc_network_option_t);
extern int bcm_common_esmc_QL_SSM_map(int, bcm_esmc_network_option_t, bcm_esmc_quality_level_t, uint8 *);
extern int bcm_common_esmc_SSM_QL_map(int, bcm_esmc_network_option_t, uint8, bcm_esmc_quality_level_t *);

extern int bcm_common_tdpll_dpll_bindings_get(int, int, int, bcm_tdpll_dpll_bindings_t *);
extern int bcm_common_tdpll_dpll_bindings_set(int, int, int, bcm_tdpll_dpll_bindings_t *);
extern int bcm_common_tdpll_dpll_reference_get(int, int, int, int *, int *);
extern int bcm_common_tdpll_dpll_bandwidth_get(int, int, int, bcm_tdpll_dpll_bandwidth_t *);
extern int bcm_common_tdpll_dpll_bandwidth_set(int, int, int, bcm_tdpll_dpll_bandwidth_t *);
extern int bcm_common_tdpll_dpll_phase_control_get(int, int, int, bcm_tdpll_dpll_phase_control_t *);
extern int bcm_common_tdpll_dpll_phase_control_set(int, int, int, bcm_tdpll_dpll_phase_control_t *);

extern int bcm_common_tdpll_input_clock_control(int, int, int);
extern int bcm_common_tdpll_input_clock_mac_get(int, int, int, bcm_mac_t *);
extern int bcm_common_tdpll_input_clock_mac_set(int, int, int, bcm_mac_t *);
extern int bcm_common_tdpll_input_clock_frequency_error_get(int, int, int, int *);
extern int bcm_common_tdpll_input_clock_threshold_state_get(int, int, int, bcm_tdpll_input_clock_monitor_type_t, int *);
extern int bcm_common_tdpll_input_clock_enable_get(int, int, int, int *);
extern int bcm_common_tdpll_input_clock_enable_set(int, int, int, int);
extern int bcm_common_tdpll_input_clock_l1mux_get(int, int, int, bcm_tdpll_input_clock_l1mux_t *);
extern int bcm_common_tdpll_input_clock_l1mux_set(int, int, int, bcm_tdpll_input_clock_l1mux_t *);
extern int bcm_common_tdpll_input_clock_valid_get(int, int, int, int *);
extern int bcm_common_tdpll_input_clock_valid_set(int, int, int, int);
extern int bcm_common_tdpll_input_clock_frequency_get(int, int, int, uint32 *, uint32 *);
extern int bcm_common_tdpll_input_clock_frequency_set(int, int, int, uint32, uint32);
extern int bcm_common_tdpll_input_clock_ql_get(int, int, int, bcm_esmc_quality_level_t *);
extern int bcm_common_tdpll_input_clock_ql_set(int, int, int, bcm_esmc_quality_level_t);
extern int bcm_common_tdpll_input_clock_priority_get(int, int, int, int *);
extern int bcm_common_tdpll_input_clock_priority_set(int, int, int, int);
extern int bcm_common_tdpll_input_clock_lockout_get(int, int, int, int *);
extern int bcm_common_tdpll_input_clock_lockout_set(int, int, int, int);
extern int bcm_common_tdpll_input_clock_monitor_interval_get(int, int, uint32 *);
extern int bcm_common_tdpll_input_clock_monitor_interval_set(int, int, uint32);
extern int bcm_common_tdpll_input_clock_monitor_threshold_get(int, int, bcm_tdpll_input_clock_monitor_type_t, uint32 *);
extern int bcm_common_tdpll_input_clock_monitor_threshold_set(int, int, bcm_tdpll_input_clock_monitor_type_t, uint32);
extern int bcm_common_tdpll_input_clock_ql_enabled_get(int, int, int, int *);
extern int bcm_common_tdpll_input_clock_ql_enabled_set(int, int, int, int);
extern int bcm_common_tdpll_input_clock_revertive_get(int, int, int, int *);
extern int bcm_common_tdpll_input_clock_revertive_set(int, int, int, int);
extern int bcm_common_tdpll_input_clock_best_get(int, int, int, int *);
extern int bcm_common_tdpll_input_clock_monitor_callback_register(int, int, bcm_tdpll_input_clock_monitor_cb);
extern int bcm_common_tdpll_input_clock_monitor_callback_unregister(int, int);
extern int bcm_common_tdpll_input_clock_selector_callback_register(int, int, bcm_tdpll_input_clock_selector_cb);
extern int bcm_common_tdpll_input_clock_selector_callback_unregister(int, int);
extern int bcm_common_tdpll_input_clock_ql_change_callback_register(int, int, bcm_tdpll_input_clock_ql_change_cb);
extern int bcm_common_tdpll_input_clock_ql_change_callback_unregister(int, int);

extern int bcm_common_tdpll_input_clock_callback_register(int, int, bcm_tdpll_callback_type_t, bcm_tdpll_input_clock_cb);
extern int bcm_common_tdpll_input_clock_callback_unregister(int, int, bcm_tdpll_callback_type_t);

extern int bcm_common_tdpll_output_clock_create(int, int, int, bcm_pbmp_t, bcm_tdpll_dpll_bandwidth_t *);
extern int bcm_common_tdpll_output_clock_enable_get(int, int, int, int *);
extern int bcm_common_tdpll_output_clock_enable_set(int, int, int, int);
extern int bcm_common_tdpll_output_clock_synth_frequency_get(int, int, int, uint32 *, uint32 *);
extern int bcm_common_tdpll_output_clock_synth_frequency_set(int, int, int, uint32, uint32);
extern int bcm_common_tdpll_output_clock_deriv_frequency_get(int, int, int, uint32 *);
extern int bcm_common_tdpll_output_clock_deriv_frequency_set(int, int, int, uint32);
extern int bcm_common_tdpll_output_clock_holdover_data_get(int, int, int, bcm_tdpll_holdover_data_t *);
extern int bcm_common_tdpll_output_clock_holdover_frequency_set(int, int, int, bcm_tdpll_frequency_correction_t);
extern int bcm_common_tdpll_output_clock_holdover_mode_get(int, int, int, bcm_tdpll_holdover_mode_t *);
extern int bcm_common_tdpll_output_clock_holdover_mode_set(int, int, int, bcm_tdpll_holdover_mode_t);
extern int bcm_common_tdpll_output_clock_holdover_reset(int, int, int);

extern int bcm_common_tdpll_esmc_rx_state_machine(int, int, int, bcm_esmc_pdu_data_t *);
extern int bcm_common_tdpll_esmc_ql_get(int, int, int, bcm_esmc_quality_level_t *);
extern int bcm_common_tdpll_esmc_ql_set(int, int, int, bcm_esmc_quality_level_t);
extern int bcm_common_tdpll_esmc_holdover_ql_get(int, int, int, bcm_esmc_quality_level_t *);
extern int bcm_common_tdpll_esmc_holdover_ql_set(int, int, int, bcm_esmc_quality_level_t);
extern int bcm_common_tdpll_esmc_mac_get(int, int, int, bcm_mac_t *);
extern int bcm_common_tdpll_esmc_mac_set(int, int, int, bcm_mac_t *);
extern int bcm_common_tdpll_esmc_rx_enable_get(int, int, int *);
extern int bcm_common_tdpll_esmc_rx_enable_set(int, int, int);
extern int bcm_common_tdpll_esmc_tx_enable_get(int, int, int, int *);
extern int bcm_common_tdpll_esmc_tx_enable_set(int, int, int, int);
extern int bcm_common_tdpll_esmc_rx_portbitmap_get(int, int, int, bcm_pbmp_t *);
extern int bcm_common_tdpll_esmc_rx_portbitmap_set(int, int, int, bcm_pbmp_t);
extern int bcm_common_tdpll_esmc_tx_portbitmap_get(int, int, int, bcm_pbmp_t *);
extern int bcm_common_tdpll_esmc_tx_portbitmap_set(int, int, int, bcm_pbmp_t);
extern int bcm_common_tdpll_esmc_essm_code_set(int ,int ,int , bcm_esmc_essm_code_t);
extern int bcm_common_tdpll_esmc_essm_code_get(int ,int ,int , bcm_esmc_essm_code_t*);
extern int bcm_common_ptp_external_phy_synchronize(int ,int ,int , bcm_ptp_external_phy_config_t *);
extern int bcm_common_tdpll_event_callback_register(int ,int , bcm_tdpll_event_cb_f );
extern int bcm_common_tdpll_event_callback_unregister(int ,int);


#define PTP_SERVO_IS_BCM(unit, ptp_id, clock_num) \
	(_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array[clock_num].servo_type == bcmPTPServoTypeBCM)

#define PTP_SERVO_IS_EXT(unit, ptp_id, clock_num) \
	(_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array[clock_num].servo_type == bcmPTPServoTypeExt)

#define PTP_CLOCK_IS_G8265P1_PROF(clock_info) \
	( SHR_BITGET(&(clock_info->flags), PTP_CLOCK_FLAGS_TELECOM_PROFILE_BIT) && \
	(((clock_info->flags >> 8)&0x0F) == e_PtpProfType_8265p1) )

#define PTP_CLOCK_IS_G8275P1_PROF(clock_info) \
	( ((clock_info->flags >> 8)&0x0F) == e_PtpProfType_8275p1 )

#define PTP_CLOCK_IS_G8275P2_PROF(clock_info) \
	( ((clock_info->flags >> 8)&0x0F) == e_PtpProfType_8275p2 )

#ifdef BCM_PTP_EXT_SERVO_SUPPORT

extern void mngPtpLoggerStart( T_srvMessageLoggingConfig *logConfig );
extern void mngPtpSessionStart( T_cmnConfigParameterSet *configParam, T_userCallbacks *);
extern void mngPtpSessionStop( void );

extern void    format_ipaddr(char buf[IPADDR_STR_LEN], ip_addr_t ipaddr);

int _bcm_ptp_servo_init(int, bcm_ptp_stack_id_t, int, bcm_ptp_osc_type_t, T_srvReferenceTrackerType);
int _bcm_ptp_servo_deinit(int, bcm_ptp_stack_id_t, int);

int _bcm_ptp_servo_master_add(int, bcm_ptp_stack_id_t, int, uint16, bcm_ptp_port_identity_t *, 
        bcm_ptp_clock_port_address_t *, uint8, unsigned char);
int _bcm_ptp_servo_master_remove(int ,bcm_ptp_stack_id_t ,int , uint16, uint8, bcm_ptp_port_identity_t * ,bcm_ptp_port_identity_t * , bcm_ptp_clock_port_address_t *);
int _bcm_ptp_servo_master_replace(int , bcm_ptp_stack_id_t, int, uint16, bcm_ptp_port_identity_t *, bcm_ptp_port_identity_t *, uint8);

int _bcm_ptp_servo_best_master_update(int , bcm_ptp_stack_id_t , int , bcm_ptp_port_identity_t * );
int _bcm_ptp_servo_pdv_threshold_set(int , bcm_ptp_stack_id_t ,  int , bcm_ptp_telecom_g8265_pktstats_t);
int _bcm_ptp_servo_master_lockout_set(int , bcm_ptp_stack_id_t , int , int , bcm_ptp_port_identity_t *);

int _bcm_ptp_servo_debug_level_set(int ,bcm_ptp_stack_id_t ,int ,uint16 ,uint32 );
int _bcm_ptp_servo_sys_log_set(int unit, bcm_ptp_stack_id_t , int , uint32 , bcm_ip_t , uint16 );

#ifdef BCM_PTP_EXT_SERVO_ABS_TS_SUPPORT
/* absolute timestamps */
int _bcm_ptp_servo_AddFullTimestamps(int , bcm_ptp_stack_id_t , int , bcm_ptp_port_identity_t *, int, uint8 *, _bcm_ptp_ts_type_t, uint32);
#else
/* timestamp differences */
int _bcm_ptp_servo_AddTimestampDiffs(int , bcm_ptp_stack_id_t , int , bcm_ptp_port_identity_t *, int, bcm_ptp_timestamp_t, uint8* , _bcm_ptp_ts_type_t , uint32 );
#endif/* BCM_PTP_EXT_SERVO_ABS_TS_SUPPORT */

int _bcm_ptp_ext_servo_master_table_entry_add(int, bcm_ptp_stack_id_t , int, bcm_ptp_port_identity_t *, bcm_ptp_clock_port_address_t *, uint8, int *);
int _bcm_ptp_ext_servo_master_table_entry_delete( int unit, bcm_ptp_stack_id_t, int, bcm_ptp_port_identity_t *, bcm_ptp_clock_port_address_t *, uint8, int * ); 

#endif /* BCM_PTP_EXT_SERVO_SUPPORT */

#define BCM_PTP_CLOCK_PORT_MAX_TLV_CONFIG_NUM           (20)
#define BCM_PTP_CLOCK_PORT_MAX_TLV_CONFIG_LEN           (100)

typedef struct _bcm_ptp_clock_port_tlv_config_context_e {
    bcm_ptp_tlv_type_t tlv_type;
    uint16  tlv_len;
    uint8   tlv_data[BCM_PTP_CLOCK_PORT_MAX_TLV_CONFIG_LEN];
    bcm_ptp_tlv_action_t tlv_action;
    int config_id;
    int enable;
    uint8 in_use;
}_bcm_ptp_clock_port_tlv_config_context_t;

extern _bcm_ptp_clock_port_tlv_config_context_t g_tlv_config_context[32][BCM_PTP_CLOCK_PORT_MAX_TLV_CONFIG_NUM];

#endif	/* defined(INCLUDE_PTP) */

