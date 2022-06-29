/**
 * \file src/bcm/dnx/tsn/tsn_counter.h
 * 
 *
 * TSN counter related functionality
 */

#ifndef _TSN_COUNTER_H_INCLUDED_
#define _TSN_COUNTER_H_INCLUDED_

/*
 * INCLUDE FILES:
 * {
 */

/*
 * }
 */

/**
 * \brief - Convert PTP time into TSN-counter
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ptp_time - input PTP time
 * \param [out] tsn_counter - output TSN-counter
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_convert_ptp_time_to_tsn_counter(
    int unit,
    bcm_ptp_timestamp_t * ptp_time,
    uint64 *tsn_counter);

/**
 * \brief - Convert PTP time into ticks
 *
 * \param [in] unit -  Unit-ID
 * \param [in] nanoseconds - input in nanoseconds
 * \param [out] ticks - output in ticks
 */
shr_error_e dnx_tsn_counter_nanoseconds_to_ticks_convert(
    int unit,
    uint64 nanoseconds,
    uint64 *ticks);

/**
 * \brief - Get the current time in nano-seconds
 *
 * \param [in] unit -  Unit-ID
 * \param [out] curr_time - current time in nano-seconds
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_curr_time_get(
    int unit,
    uint64 *curr_time);

/**
 * \brief - Get the current time in ptp time
 *
 * \param [in] unit -  Unit-ID
 * \param [out] ptp_curr_time - current time in ptp time
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_ptp_curr_time_get(
    int unit,
    bcm_ptp_timestamp_t * ptp_curr_time);

/**
 * \brief - Get the actual PTP time to start running profile
 *          current_time = current_ptp_time_get() + time_margin
 *          if input base time is in the past (base time < current time), base time should be updated to a future time.
 *          updated base time => original base time + N * cycle time >= current time (N is an integer)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ptp_base_time - requested base time
 * \param [in] cycle_time - profile cycle time (nano seconds)
 * \param [in] time_margin - time margin (nano seconds)
 * \param [out] actual_change_time - Actual time to start running profile
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_config_change_time_get(
    int unit,
    bcm_ptp_timestamp_t * ptp_base_time,
    uint64 cycle_time,
    uint32 time_margin,
    bcm_ptp_timestamp_t * actual_change_time);

/**
 * \brief -
 *  Initialize TSN counter
 */
shr_error_e dnx_tsn_counter_init(
    int unit);

#endif /* _TSN_COUNTER_H_INCLUDED_ */
