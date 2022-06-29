/*
 * tsn_tas.h
 *
 *  Created on: Apr 12, 2022
 *      Author: nt893888
 */

#ifndef _TSN_TAS_H_INCLUDED_
#define _TSN_TAS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/port.h>
#include <bcm/cosq.h>

/**
 * \brief - Init function for time aware scheduling
 *
 * \param [in] unit -HW identifier of unit.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_tas_init(
    int unit);

/**
 * \brief - Init function for time aware scheduling
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] core_id -Core ID.
 * \param [in] table_set -table set to config
 * \param [in] pid - pid value.
 *
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_tas_profile_hw_set(
    int unit,
    bcm_core_t core_id,
    int table_set,
    bcm_cosq_tas_profile_id_t pid);

#endif /* _TSN_TAS_H_INCLUDED_ */
