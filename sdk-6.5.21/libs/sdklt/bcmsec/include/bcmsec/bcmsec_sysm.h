/*! \file bcmsec_sysm.h
 *
 * BCMSEC System manager initialization
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMSEC_SYSM_H
#define BCMSEC_SYSM_H

#include <sal/sal_types.h>
/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int bcmsec_##_bd##_drv_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize SEC device driver.
 *
 * Install TM driver functions and initialize device features.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmsec_attach(int unit);

/*!
 * \brief Clean up SEC driver.
 *
 * Release any resources allocated by \ref bcmsec_attach.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot.
 */
extern int
bcmsec_detach(int unit, bool warm);

/*!
 * \brief Start SEC component.
 *
 * Register SEC component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmsec_start(void);

/*!
 * \brief Stop the SEC component.
 *
 * Stop any threads created by SEC component.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_INTERNAL Component stop failed.
 */
extern int
bcmsec_stop(int unit, bool warm);

/*!
 * \brief Run the SEC component.
 *
 * Implement sys sym Run of SEC component.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot mode.
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_INTERNAL Component stop failed.
 */
extern int
bcmsec_run(int unit, bool warm);

#endif /* BCMSEC_SYSM_H */
