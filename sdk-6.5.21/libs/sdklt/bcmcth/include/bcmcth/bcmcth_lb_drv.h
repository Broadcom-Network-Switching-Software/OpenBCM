/*! \file bcmcth_lb_drv.h
 *
 * BCMCTH Load Balance (LB) driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_LB_DRV_H
#define BCMCTH_LB_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>

#include <bcmcth/bcmcth_lb_hash_info.h>

/*!
 * \brief Get device-specific load balance hashing information.
 *
 * \param [in] unit Unit number.
 * \param [out] hash_info Device-specific hash information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*bcmcth_lb_hash_info_get_f)(int unit, bcmcth_lb_hash_info_t *hash_info);

/*!
 * \brief LB driver object
 *
 * LB driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH LB module by
 * \ref bcmcth_lb_drv_init() from the top layer. BCMCTH LB internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_lb_drv_s {

    /*! Get load balance hashing information. */
    bcmcth_lb_hash_info_get_f hash_info_get;

} bcmcth_lb_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_lb_drv_t *_bc##_cth_lb_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Get the LB driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_lb_drv_init(int unit);

/*!
 * \brief Get device-specific load balance hashing information.
 *
 * \param [in] unit Unit number.
 * \param [out] hash_info Device-specific hash information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_lb_hash_info_get(int unit, bcmcth_lb_hash_info_t *hash_info);

#endif /* BCMCTH_LB_DRV_H */
