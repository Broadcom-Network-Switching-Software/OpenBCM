/**
 * \file src/bcm/dnx/tsn/tsn_dbal.h
 * 
 *
 * DBAL access functions for TSN
 */

#ifndef _TSN_DBAL_H_INCLUDED_
#define _TSN_DBAL_H_INCLUDED_

/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dbal/dbal.h>
/*
 * }
 */

/**
 * \brief - Enable TSN counter output
 *
 * \param [in] unit -  Unit-ID
 * \param [in] enable -  enable status
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_enable_hw_set(
    int unit,
    uint32 enable);

/**
 * \brief - Enable TAS-ToD output
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ts_source -  index of TS counter serves as the source of TAS-ToD
 * \param [in] enable -  enable status
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_tas_tod_enable_hw_set(
    int unit,
    uint32 ts_source,
    uint32 enable);

/**
 * \brief - Set TSN counter tick period and tick period minimum
 *
 * \param [in] unit -  Unit-ID
 * \param [in] tick_period_ns -  The TSN counter tick period (nsec)
 * \param [in] tick_period_min_ns - The minimal TSN counter tick period (nsec)
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_tick_period_hw_set(
    int unit,
    uint32 tick_period_ns,
    uint32 tick_period_min_ns);

/**
 * \brief - Set TSN counter start attributes
 *
 * \param [in] unit -  Unit-ID
 * \param [in] start_tas_tod_time - When to start the TSN counter in TAS-ToD units
 * \param [in] start_tsn_value - The initial TSN counter value
 * \param [in] start_trigger - Triggers TSN counter start
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_start_attributes_hw_set(
    int unit,
    uint64 start_tas_tod_time,
    uint64 start_tsn_value,
    uint32 start_trigger);

/**
 * \brief - Polling TSN counter has started
 *
 * \param [in] unit -  Unit-ID
 * \param [in] min_polls - minimum polls
 * \param [in] timeout time out value, maximal time for polling
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_start_trigger_down_hw_poll(
    int unit,
    sal_usecs_t timeout,
    int32 min_polls);

/**
 * \brief - Get current time in nano-seconds.
 *
 * \param [in] unit -  Unit-ID
 * \param [out] curr_time - Current time in nano-seconds
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_curr_time_hw_get(
    int unit,
    uint64 *curr_time);

/**
 * \brief - Current values of TSN counter and TAS-ToD. Counters are synced.
 *
 * \param [in] unit -  Unit-ID
 * \param [out] tsn_counter - Current TSN counter
 * \param [out] tas_tod - Current TAS-ToD
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_synced_counters_value_hw_get(
    int unit,
    uint64 *tsn_counter,
    uint64 *tas_tod);

/**
 * \brief - TAF global table init
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_taf_global_config_hw_init(
    int unit);

/**
 * \brief - Get active set value from hw
 *
 * \param [in] unit -  Unit-ID
 * \param [out] active_set - Active set value
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_taf_active_set_hw_get(
    int unit,
    uint32 *active_set);

/**
 * \brief - Set active set value to hw
 *
 * \param [in] unit -  Unit-ID
 * \param [in] active_set - Active set value
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_taf_active_set_hw_set(
    int unit,
    uint32 active_set);

/**
 * \brief - Set TAF gate params attributes to hw.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] set -  Table set
 * \param [in] taf_gate - TAF gate id
 * \param [in] nof_ti_entries - Number of interval entries in gate control list
 * \param [in] is_active - indicate if TAF gate is active
 * \param [in] base_time -  Tick-period to start running (48-bit TICK counter)
 * \param [in] first_gate_state -  First gate state (0 - closed , 1 - opened)
 * \param [in] ti_index_start -  The first index in gate control list
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_taf_gate_params_hw_set(
    int unit,
    uint32 set,
    uint32 taf_gate,
    uint32 nof_ti_entries,
    uint32 is_active,
    uint64 base_time,
    uint32 first_gate_state,
    uint32 ti_index_start);

/**
 * \brief - Set TAF gate control list to hw.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] set -  Table set
 * \param [in] ti_index - Index in gate control list
 * \param [in] num_tick_period -  Number of ticks in interval
 * \param [in] next_gate_state -  Next gate state (0 - closed , 1 - opened)
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_taf_gate_control_list_hw_set(
    int unit,
    uint32 set,
    uint32 ti_index,
    uint32 num_tick_period,
    uint32 next_gate_state);

/**
 * \brief - Init TAS hw.
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_tas_hw_init(
    int unit);

/**
 * \brief - Set active set value to hw
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] active_set - Active set value
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_tas_active_set_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 active_set);

/**
 * \brief - Set port params attributes to hw.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] set - Table set
 * \param [in] tas_port - Tas port value
 * \param [in] nof_ti - number of time intervals
 * \param [in] is_active - indicate if port tas is active
 * \param [in] base_time - Tick-period to start running on the list
 * \param [in] first_gate_state - first interval gate state
 * \param [in] first_stop_preemptable - first interval stop preemptable value
 * \param [in] queue_base - Queue base value
 * \param [in] ps_mode - Priority mode for a qpair group
 * \param [in] esb_port_num -Low priority ESB queue number
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
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

/**
 * \brief - Set port control list attributes to hw.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] set - Table set
 * \param [in] tas_port - Tas port value
 * \param [in] ti_index - Time interval index
 * \param [in] nof_tick_periods - number of tick period
 * \param [in] next_gate_state - next interval gate state
 * \param [in] next_stop_preemptable - next interval stop preemptable value
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_tas_port_control_list_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 tas_port,
    uint32 ti_index,
    uint64 nof_tick_periods,
    uint32 next_gate_state,
    uint32 next_stop_preemptable);

/**
 * \brief - Set port params table range value to hw.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] set - Table set
 * \param [in] start_ix - First gate being used in "set" of the tables
 * \param [in] stop_ix - Last gate being used in "set" of the tables
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_port_table_range_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 start_ix,
    uint32 stop_ix);

/**
 * \brief - Get port params table range value to hw.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] set - Table set
 * \param [out] start_ix - First gate being used in "set" of the tables
 * \param [out] stop_ix - Last gate being used in "set" of the tables
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_port_table_range_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 *start_ix,
    uint32 *stop_ix);

/**
 * \brief - Current value of active set
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [out] active_set - Current active set
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_active_set_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 *active_set);

/**
 * \brief - Set active set value to hw
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] active_set - Active set value
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_active_set_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 active_set);

/**
 * \brief - Deactivate active copy.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] set - Table set
 * \param [in] cqf_port - CQF port value
 * \param [in] is_active - indicate if port cqf is active
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_port_params_active_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint32 is_active);

/**
 * \brief - Set port params attributes to hw.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] set - Table set
 * \param [in] cqf_port - CQF port value
 * \param [in] base_time - Tick-period to start running on the list
 * \param [in] queue_base - Queue base value
 * \param [in] ps_mode - Priority mode for a qpair group
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_port_params_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint64 base_time,
    uint32 queue_base,
    dbal_enum_value_field_egq_ps_mode_e ps_mode);

/**
 * \brief - clear port params attributes hw.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] set - Table set
 * \param [in] cqf_port - CQF port value
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_port_params_hw_clear(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port);

/**
 * \brief - Set port contorl list number of tick period to HW.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] set - Table set
 * \param [in] cqf_port - CQF port value
 * \param [in] ti_index - Time interval index
 * \param [in] nof_tick_periods - number of tick period
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_port_control_list_nof_tick_periods_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint32 ti_index,
    uint64 nof_tick_periods);

/**
 * \brief - Set port contorl list number of tick period to HW.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] set - Table set
 * \param [in] cqf_port - CQF port value
 * \param [in] ti_index - Time interval index
 * \param [out] nof_tick_periods - number of tick period
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_port_control_list_nof_tick_periods_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint32 ti_index,
    uint64 *nof_tick_periods);

/**
 * \brief - Set port control list next gate state and next phase to hw.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id - Core ID
 * \param [in] cqf_port - CQF port value
 * \param [in] ti_index - Time interval index
 * \param [in] next_gate_state - next interval gate state
 * \param [in] next_phase - next phase
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_port_control_list_gate_and_phase_hw_init(
    int unit,
    bcm_core_t core_id,
    uint32 cqf_port,
    uint32 ti_index,
    uint32 next_gate_state,
    uint32 next_phase);

/**
 * \brief - Init CQF global config HW.
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_global_config_hw_init(
    int unit);

/**
 * \brief - Enable CQF global config HW.
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_global_config_hw_enable(
    int unit);

/**
 * \brief - Init CQF port params values.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] cqf_port - CQF port value
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_cqf_port_params_hw_init(
    int unit,
    int cqf_port);
#endif /* _TSN_DBAL_H_INCLUDED_ */
