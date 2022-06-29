/**
 * \file src/bcm/dnx/tsn/tsn_taf.h
 * 
 *
 * TAF (Time aware filtering) functionality
 */

#ifndef _TSN_TAF_H_INCLUDED_
#define _TSN_TAF_H_INCLUDED_

/*
 * INCLUDE FILES:
 * {
 */

/*
 * }
 */

/**
 * \brief -
 *  Initialize TSN TAF
 */
shr_error_e dnx_tsn_taf_init(
    int unit);

/**
 * \brief -
 *  Allocate TAF Gate
 *
 * \param [in] unit - the relevant unit
 * \param [in] flags - use BCM_TSN_TAF_WITH_ID to allocate gate with ID
 * \param [out] taf_gate_id - allocated gate ID
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_taf_gate_create(
    int unit,
    int flags,
    int *taf_gate_id);

/**
 * \brief -
 *  Free TAF Gate
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate_id - gate ID to free
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_taf_gate_destroy(
    int unit,
    int taf_gate_id);

/**
 * \brief -
 *  Allocate TAF profile
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] profile - profile info
 * \param [out] pid - allocated profile ID
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_taf_profile_create(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t * profile,
    bcm_tsn_taf_profile_id_t * pid);

/**
 * \brief -
 *  Free TAF profile
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_taf_profile_destroy(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid);

/**
 * \brief -
 *  Set TAF profile info
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 * \param [in] profile - profile info
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile);

/**
 * \brief -
 *  Get TAF profile info
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 * \param [out] profile - profile info
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile);

/**
 * \brief -
 *  Commit TAF profile to be written into HW
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_taf_profile_commit(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid);

/**
 * \brief -
 *  Get TAF profile status
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 * \param [out] status - profile status
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
shr_error_e dnx_tsn_taf_profile_status_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_status_t * status);

/**
 * \brief - Reset TAF gate
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] taf_gate - gate ID
 * \param [in] arg - should be 1
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_taf_gate_reset(
    int unit,
    int taf_gate,
    int arg);

#endif /* _TSN_TAF_H_INCLUDED_ */
