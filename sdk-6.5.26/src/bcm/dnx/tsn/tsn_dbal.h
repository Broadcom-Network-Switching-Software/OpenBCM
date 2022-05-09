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
 * \brief - Current value of PTP time.
 *
 * \param [in] unit -  Unit-ID
 * \param [out] ptp_time - Current PTP time
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_counter_ptp_time_hw_get(
    int unit,
    bcm_ptp_timestamp_t * ptp_time);

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

#endif /* _TSN_DBAL_H_INCLUDED_ */
