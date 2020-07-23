/*! \file bcm56990_a0_tm_obm.h
 *
 * File containing obm related defines and internal function for
 * bcm56990_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_TM_OBM_H
#define BCM56990_A0_TM_OBM_H

#include <bcmpc/bcmpc_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmtm/obm/bcmtm_obm_internal.h>

/*!
 * \brief  Initialize obm for 56990_a0.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   warm         Warm boot.
 *
 * \retval OBM handler functions for this device type.
 */
extern int
bcm56990_a0_tm_obm_init(int unit, bool warm);

/*!
 * \brief Get OBM driver for the given device.
 *
 * \param [in] unit Logical unit number.
 * \param [out] obm_drv OBM driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcm56990_a0_tm_obm_drv_get(int unit, void *obm_drv);


#endif /* BCM56990_A0_TM_OBM_H */
