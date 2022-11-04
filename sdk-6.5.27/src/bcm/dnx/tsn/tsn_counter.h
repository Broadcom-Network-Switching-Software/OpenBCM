
#ifndef _TSN_COUNTER_H_INCLUDED_
#define _TSN_COUNTER_H_INCLUDED_

shr_error_e dnx_tsn_counter_convert_ptp_time_to_tsn_counter(
    int unit,
    bcm_ptp_timestamp_t * ptp_time,
    uint64 *tsn_counter);

shr_error_e dnx_tsn_counter_nanoseconds_to_ticks_convert(
    int unit,
    uint64 nanoseconds,
    uint64 *ticks);

shr_error_e dnx_tsn_counter_curr_time_get(
    int unit,
    uint64 *curr_time);

shr_error_e dnx_tsn_counter_ptp_curr_time_get(
    int unit,
    bcm_ptp_timestamp_t * ptp_curr_time);

shr_error_e dnx_tsn_counter_config_change_time_get(
    int unit,
    bcm_ptp_timestamp_t * ptp_base_time,
    uint64 cycle_time,
    uint32 time_margin,
    bcm_ptp_timestamp_t * actual_change_time);

shr_error_e dnx_tsn_counter_init(
    int unit);

shr_error_e dnx_tsn_counter_restart(
    int unit);

shr_error_e dnx_tsn_counter_restart_required_get(
    int unit,
    int *is_restart_required);

#endif
