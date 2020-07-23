/*! \file bcmcth_tnl_l3_intf.h
 *
 * This file contains egress L3 tunnel interface
 * related data structures and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_L3_INTF_H
#define BCMCTH_TNL_L3_INTF_H

#include <shr/shr_bitop.h>
#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmcth/generated/tnl_ha.h>

/*! No tunnel. */
#define    BCMCTH_TNL_TYPE_NONE    0
/*! IPV4 tunnel. */
#define    BCMCTH_TNL_TYPE_IPV4    1
/*! IPV6 tunnel. */
#define    BCMCTH_TNL_TYPE_IPV6    2
/*! MPLS tunnel. */
#define    BCMCTH_TNL_TYPE_MPLS    3

/*!
 * L3 EIF tunnel interface sw state.
 */
typedef struct bcmcth_tnl_l3_intf_state_s {
    /*! Tunnel interface sw state table size. */
    uint32_t                tbl_sz;
    /*! Tunnel interface sw state. */
    bcmcth_tnl_l3_intf_t        *intf;
    /*! Tunnel interface backup sw state. */
    bcmcth_tnl_l3_intf_t        *intf_ha;
} bcmcth_tnl_l3_intf_state_t;

/*!
 * \brief Get tunnel interface state pointer.
 *
 * Get tunnel interface state pointer for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [out] state         Returned state pointer.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_tnl_l3_intf_state_get(int unit, bcmcth_tnl_l3_intf_state_t **state);

/*!
 * \brief L3 sw resources init.
 *
 * Initialize L3 SW data structures for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  warm          Cold/Warm boot.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_tnl_l3_intf_sw_state_init(int unit,
                                 bool warm);

/*!
 * \brief L3 sw resources cleanup.
 *
 * Cleanup L3 SW data structures allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_tnl_l3_intf_sw_state_cleanup(int unit);

#endif /* BCMCTH_TNL_L3_INTF_H */
