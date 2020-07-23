/*! \file bcm56880_a0_tm_multicast_internal.h
 *
 * File containing Multicast PT parameters for
 * bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_MULTICAST_INTERNAL_H
#define BCM56880_A0_TM_MULTICAST_INTERNAL_H

#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmtm/multicast/bcmtm_mc_group.h>

#define TD4_NHOPS_PER_REPL_INDEX_SPARSE_MODE  6
#define TD4_NHOPS_PER_REPL_INDEX_DENSE_MODE  64
#define TD4_MAX_MC_PKT_REPL 1024
#define TD4_MAX_REPL_LIST_COUNT 147456

/*!
 * \brief Multicast chip driver get.
 *
 * \param [in] unit Unit number.
 * \param [out] mc_drv Multicast driver.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcm56880_a0_tm_mc_drv_get(int unit, void *mc_drv);

#endif /* BCM56880_A0_TM_MULTICAST_INTERNAL_H */
