/*! \file bcm56880_a0_tm_mirror_on_drop.h
 *
 * File containing Mirror-on-drop functions specific to
 * bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_MIRROR_ON_DROP_H
#define BCM56880_A0_TM_MIRROR_ON_DROP_H

#include <bcmlrd/bcmlrd_types.h>
#include <bcmtm/mod/bcmtm_mirror_on_drop_internal.h>

/*!
 * \brief BCMTM MoD driver get for TD4.
 *
 * \param [in] unit Logical unit number.
 *
 * \return Return the driver function for MoD configuration.
 */
extern int
bcm56880_a0_tm_mod_drv_get(int unit, void *mod_drv);

#endif /* BCM56880_A0_TM_MIRROR_ON_DROP_H */
