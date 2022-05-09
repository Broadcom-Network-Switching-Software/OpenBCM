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
 * \brief -
 *  Initialize TSN counter
 */
shr_error_e dnx_tsn_counter_init(
    int unit);

#endif /* _TSN_COUNTER_H_INCLUDED_ */
