/*! \file bcmbd_mmu_intr_internal.h
 *
 * BD mmu interrupt driver internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_MMU_INTR_INTERNAL_H
#define BCMBD_MMU_INTR_INTERNAL_H

#include <bcmbd/bcmbd_mmu_intr.h>

/*!
 * \brief Install device-specific mmu interrupt driver.
 *
 * Install device-specific mmu interrupt driver into top-level mmu
 * interrupt API.
 *
 * Use \c intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_drv mmu interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_mmu_intr_drv_init(int unit, bcmbd_intr_drv_t *intr_drv);

/*!
 * \brief Install device-specific mmu instrument interrupt driver.
 *
 * Install device-specific mmu instrument interrupt driver into top-level
 * mmu instrument interrupt API.
 *
 * Use \c intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_drv mmu instrument interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_mmui_intr_drv_init(int unit, bcmbd_intr_drv_t *intr_drv);

#endif /* BCMBD_MMU_INTR_INTERNAL_H */
