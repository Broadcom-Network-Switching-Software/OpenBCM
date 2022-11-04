/**
 * 
 * DNX PTP headers
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef DNX_ONESYNC_H_INCLUDED
/* { */
#define DNX_ONESYNC_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/*
 * STRUCTURES
 * {
 */
/*
 * }
 */

/**
 * \brief
 *   Initialize PTP module.
 *
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int bcm_dnx_knetsync_init(
    int unit);

/**
 * \brief
 *   Deinit PTP module.
 *
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int bcm_dnx_knetsync_deinit(
    int unit);

/* } */
#endif /* DNX_PTP_H_INCLUDED */
