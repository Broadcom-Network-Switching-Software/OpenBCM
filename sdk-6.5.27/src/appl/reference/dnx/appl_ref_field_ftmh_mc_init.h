/** \file appl_ref_field_ftmh_mc_init.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_FIELD_FTMH_MC_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_FIELD_FTMH_MC_INIT_H_INCLUDED

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
 * configure PMF context to parse FTMH packets for MC packets
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_field_ftmh_mc_init(
    int unit);

/**
 * \brief
 *   This function determines if the app should be skipped
 *
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_ftmh_mc_cb(
    int unit,
    int *dynamic_flags);
/*
 * }
 */
#endif /* APPL_REF_FIELD_FTMH_MC_INIT_H_INCLUDED */
