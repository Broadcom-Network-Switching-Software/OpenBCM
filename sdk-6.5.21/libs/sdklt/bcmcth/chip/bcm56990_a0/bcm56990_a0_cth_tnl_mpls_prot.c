/*! \file bcm56990_a0_cth_tnl_mpls_prot.c
 *
 * This file defines the detach, attach routines of
 * TNL mpls protection switching driver for bcm56990_a0.
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

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Tunnel MPLS protection enable set.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  nhop_id       Nexthop id.
 * \param [in]  prot          Enable/Disable protection.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcm56990_a0_tnl_mpls_prot_enable_set(int unit, const bcmltd_op_arg_t *op_arg,
                                     bcmltd_sid_t lt_id, uint32_t nhop_id,
                                     bool prot)
{
    return SHR_E_NONE;
}

/*!
 * \brief Tunnel MPLS protection enable get.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  nhop_id       Nexthop id.
 * \param [in]  prot          Enable/Disable protection PTR.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcm56990_a0_tnl_mpls_prot_enable_get(int unit, const bcmltd_op_arg_t *op_arg,
                                     bcmltd_sid_t lt_id, uint32_t nhop_id,
                                     bool *prot)
{
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */
/* Driver structure. */
bcmcth_tnl_mpls_prot_drv_t bcm56990_a0_tnl_mpls_prot_drv = {
    .mpls_prot_set = bcm56990_a0_tnl_mpls_prot_enable_set,
    .mpls_prot_get = bcm56990_a0_tnl_mpls_prot_enable_get,
};

bcmcth_tnl_mpls_prot_drv_t *
bcm56990_a0_cth_tnl_mpls_prot_drv_get(int unit)
{
    return &bcm56990_a0_tnl_mpls_prot_drv;
}
