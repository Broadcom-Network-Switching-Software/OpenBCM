/*! \file bcm56780_a0_cth_tnl_mpls_prot.c
 *
 * This file defines the detach, attach routines of
 * TNL mpls protection switching driver for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif

#include <shr/shr_error.h>
#include <bcmcth/bcmcth_tnl_mpls_prot.h>

/*!
 * \brief Tunnel MPLS protection enable set.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      Transaction id.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  nhop_id       Nexthop id.
 * \param [in]  prot          Enable/Disable protection.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
bcmcth_tnl_mpls_prot_drv_t *
bcm56780_a0_cth_tnl_mpls_prot_drv_get(int unit)
{
    return NULL;
}
