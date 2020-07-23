/*! \file bcmcth_ts_drv.h
 *
 * BCMCTH TimeSync driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TS_DRV_H
#define BCMCTH_TS_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmcth/bcmcth_ts_synce_drv.h>
#include <bcmcth/bcmcth_ts_tod_drv.h>

/*! HA subcomponent ID for TS synce. */
#define BCMTS_SYNCE_SUB_COMP_ID (1)

/*! \brief TS variant specific information */
typedef struct bcmcth_ts_info_s {
    /*! Header ID for PTP packet*/
    uint8_t ptp_hdr_id;
    /*! Zone number for ptp packet */
    uint8_t ptp_zone;
} bcmcth_ts_info_t;

/*!
 * \brief Initialize the TS.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 * \param [in] info TS info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ts_init_f)(int unit,
                                int warm,
                                const bcmcth_ts_info_t *info);

/*!
 * \brief TS driver object
 *
 * TS driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH TS module by
 * \ref bcmcth_ts_drv_init() from the top layer. BCMCTH TS internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_ts_drv_s {
    /*! TS variant specific info */
    const bcmcth_ts_info_t *info;
    /*! Initialization function */
    bcmcth_ts_init_f init;
    /*! SYNCE driver functions */
    bcmcth_ts_synce_drv_t *synce_drv;
    /*! ToD driver functions */
    bcmcth_ts_tod_drv_t *tod_drv;
} bcmcth_ts_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_ts_drv_t *_bc##_cth_ts_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! Signature of TS info get function */
typedef int (*cth_ts_info_get_f)(bcmcth_ts_drv_t *drv);

/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_cth_ts_info_get(bcmcth_ts_drv_t *drv);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Get the TS driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern bcmcth_ts_drv_t *
bcmcth_ts_drv_get(int unit);

/*!
 * \brief Initialize the TS driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ts_drv_init(int unit);

/*!
 * \brief Initialize the TS components.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ts_init(int unit, bool warm);

/*!
 * \brief De-initialize the TS components.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ts_deinit(int unit);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The TS custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ts_imm_init(int unit, bool warm);

#endif /* BCMCTH_TS_DRV_H */
