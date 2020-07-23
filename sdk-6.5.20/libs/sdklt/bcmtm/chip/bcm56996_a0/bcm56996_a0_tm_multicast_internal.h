/*! \file bcm56996_a0_tm_multicast_internal.h
 *
 * File containing Multicast PT parameters for
 * bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56996_A0_TM_MULTICAST_INTERNAL_H
#define BCM56996_A0_TM_MULTICAST_INTERNAL_H

#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmtm/multicast/bcmtm_mc_group.h>

#define TH4G_NHOPS_PER_REPL_INDEX_SPARSE_MODE 4
#define TH4G_NHOPS_PER_REPL_INDEX_DENSE_MODE  64
#define TH4G_MAX_REPL_LIST_COUNT              4096
#define TH4G_MAX_MC_PKT_REPL                  384

/*!
 * \brief Initialization of multicast PT
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcm56996_a0_tm_multicast_init (int unit, int warm);

extern int
bcm56996_a0_tm_mc_drv_get(int unit, void *mc_drv);

#endif /* BCM56996_A0_TM_MULTICAST_INTERNAL_H */
