/*! \file cth_uft_be.h
 *
 * UFT device specific info
 *
 * This file contains device specific info which are internal to UFT CTH.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef CTH_UFT_BE_H
#define CTH_UFT_BE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>

/*******************************************************************************
 * Defines
 */

#define UFT_BE_DEV(unit)          (uft_be_dev_info[unit])
#define UFT_BE_DRV(unit)          (uft_be_driver[unit])
#define UFT_BE_DRV_EXEC(unit, fn) (UFT_BE_DRV(unit)->fn)

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Populate device info into sw data structure
 *
 * \param [in] unit Device unit.
 * \param [in] var_info Device variant info
 * \param [out] dev Device info data structure
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_be_device_info_init(int unit,
                               const bcmptm_uft_var_drv_t *var_info,
                               uft_be_dev_info_t *dev);

#endif /* CTH_UFT_BE_H */
