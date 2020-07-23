/*! \file bcm56990_a0_cth_l3_prot.c
 *
 * This file defines the detach, attach routines of
 * L3 nexthop protection switching driver for bcm56990_a0.
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
#include <bcmcth/bcmcth_l3_prot.h>

/*!
 * \brief L3 nexthop protection enable set.
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
int
bcm56990_a0_l3_prot_enable_set(int unit, uint32_t trans_id,
                               bcmltd_sid_t lt_id, uint32_t nhop_id,
                               bool prot)
{
    return SHR_E_NONE;
}
