/*! \file bcm_int/dnx/vsi/vsi.h
 *
 * Internal DNX VSI APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _VSI_API_INCLUDED__
/*
 * {
 */
#define _VSI_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Default vsi value*/
#define DNX_VSI_DEFAULT (0)

/*
 * }
 */
/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * { 
 */

/**
 * \brief - Retrieve the maximum number of VSIs in the system
 *
 * \param [in] unit - unit id
 * \param [out] nof_vsis - number of vsis
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vsi_count_get(
    int unit,
    int *nof_vsis);

/*
 * } 
 */
#endif/*_VSI_API_INCLUDED__*/
