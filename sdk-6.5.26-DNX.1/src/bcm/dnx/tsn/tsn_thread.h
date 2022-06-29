/**
 * \file src/bcm/dnx/tsn/tsn_thread.h
 * 
 *
 * TSN manager thread
 */

#ifndef _TSN_THREAD_H_INCLUDED_
#define _TSN_THREAD_H_INCLUDED_

/*
 * INCLUDE FILES:
 * {
 */

/*
 * }
 */

/**
 * \brief - Init TSN thread database
 */
shr_error_e dnx_tsn_thread_db_init(
    int unit);

/**
 * \brief - Add new profile to thread pending list
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ptp_base_time - profile base time
 * \param [in] type - gate type (TAS,TAF)
 * \param [in] pid - TSN profile id
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_thread_profile_commit(
    int unit,
    bcm_ptp_timestamp_t * ptp_base_time,
    dnx_tsn_gate_type_e type,
    int pid);

/**
 * \brief - Clear gate's pending list and disable gate
 *
 * \param [in] unit -  Unit-ID
 * \param [in] type - gate type (TAS,TAF)
 * \param [in] gate_id - TSN gate id
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_thread_profile_reset_all(
    int unit,
    dnx_tsn_gate_type_e type,
    int gate_id);

#endif /* _TSN_THREAD_H_INCLUDED_ */
