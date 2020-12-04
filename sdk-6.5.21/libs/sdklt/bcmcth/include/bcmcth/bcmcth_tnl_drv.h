/*! \file bcmcth_tnl_drv.h
 *
 * BCMCTH TNL ingress map Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BD) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_DRV_H
#define BCMCTH_TNL_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmcth/bcmcth_tnl_internal.h>
#include <bcmcth/bcmcth_tnl_decap_internal.h>

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_tnl_encap_drv_t *_bc##_cth_tnl_encap_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_tnl_decap_drv_t *_bc##_cth_tnl_decap_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize the TNL encap driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_tnl_encap_drv_init(int unit);

/*!
 * \brief Initialize the TNL decap driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_tnl_decap_drv_init(int unit);

/*!
 * \brief Get the tnl encap driver for a device.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Pointer to device driver
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Driver nto available.
 */
extern int
bcmcth_tnl_encap_drv_get(int unit, bcmcth_tnl_encap_drv_t **drv);

/*!
 * \brief Get the tnl decap driver for a device.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Pointer to device driver
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Driver nto available.
 */
extern int
bcmcth_tnl_decap_drv_get(int unit, bcmcth_tnl_decap_drv_t **drv);

/*!
 * \brief Register device-specific ingress imm call back function.
 *
 * \param [in]  unit         Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_tnl_imm_register(int unit);

#endif /* BCMCTH_TNL_DRV_H */
