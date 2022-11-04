
#ifndef _TSN_DBAL_H_INCLUDED_
#define _TSN_DBAL_H_INCLUDED_

#include <soc/dnx/dbal/dbal.h>

shr_error_e dnx_tsn_counter_enable_hw_set(
    int unit,
    uint32 enable);

shr_error_e dnx_tsn_counter_tick_period_hw_set(
    int unit,
    uint32 tick_period_ns,
    uint32 tick_period_min_ns);

shr_error_e dnx_tsn_counter_start_attributes_hw_set(
    int unit,
    uint64 start_tas_tod_time,
    uint64 start_tsn_value,
    uint32 start_trigger);

shr_error_e dnx_tsn_counter_start_trigger_down_hw_poll(
    int unit,
    sal_usecs_t timeout,
    int32 min_polls);

shr_error_e dnx_tsn_counter_curr_time_hw_get(
    int unit,
    uint64 *curr_time);

shr_error_e dnx_tsn_counter_synced_counters_value_hw_get(
    int unit,
    uint64 *tsn_counter,
    uint64 *tas_tod,
    uint64 *ptp_time);

shr_error_e dnx_tsn_taf_global_config_hw_init(
    int unit);

shr_error_e dnx_tsn_taf_active_set_hw_get(
    int unit,
    uint32 *active_set);

shr_error_e dnx_tsn_taf_active_set_hw_set(
    int unit,
    uint32 active_set);

shr_error_e dnx_tsn_taf_gate_params_active_hw_set(
    int unit,
    uint32 set,
    uint32 taf_gate,
    uint32 is_active);

shr_error_e dnx_tsn_taf_gate_params_base_time_hw_set(
    int unit,
    uint32 set,
    uint32 taf_gate,
    uint64 base_time,
    uint32 is_active);

shr_error_e dnx_tsn_taf_gate_params_hw_set(
    int unit,
    uint32 set,
    uint32 taf_gate,
    uint32 nof_ti_entries,
    uint32 is_active,
    uint64 base_time,
    uint32 first_gate_state,
    uint32 ti_index_start);

shr_error_e dnx_tsn_taf_gate_params_hw_clear(
    int unit,
    uint32 set,
    uint32 taf_gate);

shr_error_e dnx_tsn_taf_gate_control_list_hw_set(
    int unit,
    uint32 set,
    uint32 ti_index,
    uint64 num_tick_period,
    uint32 next_gate_state);

shr_error_e dnx_tsn_taf_gate_is_running_hw_get(
    int unit,
    uint32 taf_gate,
    uint32 *is_running);

shr_error_e dnx_tsn_taf_reject_settings_hw_init(
    int unit);

shr_error_e dnx_tsn_tas_hw_init(
    int unit);

shr_error_e dnx_tsn_tas_active_set_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 *active_set);

shr_error_e dnx_tsn_tas_active_set_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 active_set);

shr_error_e dnx_tsn_tas_port_params_active_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 tas_port,
    uint32 is_active);

shr_error_e dnx_tsn_tas_port_params_base_params_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 tas_port,
    uint64 base_time,
    uint32 is_active);

shr_error_e dnx_tsn_tas_port_params_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 tas_port,
    uint32 nof_ti,
    uint32 is_active,
    uint64 base_time,
    uint32 first_gate_state,
    uint32 first_stop_preemptable,
    uint32 queue_base,
    dbal_enum_value_field_egq_ps_mode_e ps_mode,
    uint32 esb_port_num);

shr_error_e dnx_tsn_tas_port_params_hw_clear(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 tas_port);

shr_error_e dnx_tsn_tas_port_control_list_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 tas_port,
    uint32 ti_index,
    uint64 nof_tick_periods,
    uint32 next_gate_state,
    uint32 next_stop_preemptable);

shr_error_e dnx_tsn_tas_port_is_running_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 tas_port,
    uint32 *is_running);

shr_error_e dnx_tsn_cqf_port_table_range_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 start_ix,
    uint32 stop_ix);

shr_error_e dnx_tsn_cqf_port_table_range_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 *start_ix,
    uint32 *stop_ix);

shr_error_e dnx_tsn_cqf_active_set_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 *active_set);

shr_error_e dnx_tsn_cqf_active_set_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 active_set);

shr_error_e dnx_tsn_cqf_port_params_active_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint32 is_active);

shr_error_e dnx_tsn_cqf_port_params_base_params_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint64 base_time,
    uint32 is_active);

shr_error_e dnx_tsn_cqf_port_params_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint64 base_time,
    uint32 queue_base,
    dbal_enum_value_field_egq_ps_mode_e ps_mode);

shr_error_e dnx_tsn_cqf_port_params_hw_clear(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port);

shr_error_e dnx_tsn_cqf_port_control_list_nof_tick_periods_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint32 ti_index,
    uint64 nof_tick_periods);

shr_error_e dnx_tsn_cqf_port_control_list_nof_tick_periods_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint32 ti_index,
    uint64 *nof_tick_periods);

shr_error_e dnx_tsn_cqf_port_control_list_gate_and_phase_hw_init(
    int unit,
    bcm_core_t core_id,
    uint32 cqf_port,
    uint32 ti_index,
    uint32 next_gate_state,
    uint32 next_phase);

shr_error_e dnx_tsn_cqf_global_config_hw_init(
    int unit);

shr_error_e dnx_tsn_cqf_global_config_hw_enable(
    int unit);

shr_error_e dnx_tsn_cqf_port_params_hw_init(
    int unit,
    int cqf_port);

shr_error_e dnx_tsn_power_down_set(
    int unit,
    int power_down);
#endif
