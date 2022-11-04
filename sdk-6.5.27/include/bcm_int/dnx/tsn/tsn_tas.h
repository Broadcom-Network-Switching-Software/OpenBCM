/*
 * tsn_tas.h
 *
 *  Created on: Mar 30, 2022
 *      Author: nt893888
 */

#ifndef INCLUDE_BCM_INT_DNX_TSN_TSN_TAS_H_
#define INCLUDE_BCM_INT_DNX_TSN_TSN_TAS_H_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - Create TAS profile
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] profile - TAS profile of the port.
 * \param [out] pid - profile id, if -1:error.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_tas_profile_create(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_t * profile,
    bcm_cosq_tas_profile_id_t * pid);

/**
 * \brief - Get TAS profile status.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id.
 * \param [out] status - TAS profile status.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_tas_status_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_status_t * status);

/**
 * \brief - Get TAS profile.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id.
 * \param [out] profile - TAS profile.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_tas_profile_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t * profile);

/**
 * \brief - Set TAS profile,
 *          can be call only for Init//Expired profiles.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id.
 * \param [in] profile - updated TAS profile.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_tas_profile_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t * profile);

/**
 * \brief - Commit TAS profile
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id to commit.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_tas_profile_commit(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid);

/**
 * \brief - Destroy TAS profile
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id to destroy.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_tas_profile_destroy(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid);

/**
 * \brief - Reset TAS port
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] arg - should be 1
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_tas_control_reset(
    int unit,
    bcm_port_t port,
    int arg);

#endif /* INCLUDE_BCM_INT_DNX_TSN_TSN_TAS_H_ */
