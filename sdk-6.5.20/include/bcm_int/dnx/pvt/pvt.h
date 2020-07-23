/** \file bcm_int/dnx/pvt/pvt.h
 * 
 * Internal DNX PVT APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PVT_H_INCLUDED
/* { */
#define PVT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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

/**
 * \brief
 *   Initialize PVT monitor module.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_pvt_mon_init(
    int unit);

/* } */
#endif /* PVT_H_INCLUDED */
