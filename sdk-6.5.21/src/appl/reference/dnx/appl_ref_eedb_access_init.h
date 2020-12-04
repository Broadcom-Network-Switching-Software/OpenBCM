/** \file appl_ref_eedb_access_init.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_EEDB_ACCESS_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_EEDB_ACCESS_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/**
 * \brief
 *
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_eedb_access_init(
    int unit);

/**
 * \brief
 *   This function determines if the app should be skipped, EEDB access
 *
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_eedb_access_cb(
    int unit,
    int *dynamic_flags);

/*
 * }
 */
#endif /* APPL_REF_L2_INIT_H_INCLUDED */
