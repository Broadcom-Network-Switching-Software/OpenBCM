/*! \file bcmtm_sysm.h
 *
 * BCMTM System manager initialization
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_SYSM_H
#define BCMTM_SYSM_H

#include <sal/sal_types.h>
/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int bcmtm_##_bd##_drv_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_tm_var_drv_attach(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Initialize TM device driver.
 *
 * Install TM driver functions and initialize device features.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmtm_attach(int unit);

/*!
 * \brief Clean up TM driver.
 *
 * Release any resources allocated by \ref bcmtm_attach.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 */
extern int
bcmtm_detach(int unit, bool warm);

/*!
 * \brief Start TM component.
 *
 * Register Tm component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmtm_start(void);

/*!
 * \brief Stop the TM component.
 *
 * Stop any threads created by TM component.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_INTERNAL Component stop failed.
 */
extern int
bcmtm_stop(int unit, bool warm);

/*!
 * \brief Run the TM component.
 *
 * Implement sys sym Run of TM component.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot mode.
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_INTERNAL Component stop failed.
 */
extern int
bcmtm_run(int unit, bool warm);

#endif /* BCMTM_SYSM_H */
