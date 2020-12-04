/*! \file bcmcth_agm_drv.h
 *
 * BCMCTH Aggregation Group Monitor (AGM) driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_AGM_DRV_H
#define BCMCTH_AGM_DRV_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>

/*! Timesync instance used in AGM. */
#define BCMAGM_TS_INST      1

/*! Maximun number of LT fields in MON_AGM_CONTROLt */
#define BCMCTH_AGM_CONTROL_FIELDS_MAX                32

/*! The data structure for MON_AGM_CONTROLt entry. */
typedef struct bcmcth_agm_control_s {
    /*! AGM identifier. */
    uint8_t agm_id;

    /*! Auto start time mode. */
    bool start_time_auto;

    /*! Start timestamp. */
    uint64_t start_time;

    /*! Start timestamp for return. */
    uint64_t start_time_oper;

    /*! Monitor start. */
    bool monitor;

    /*! Bitmap of fileds to be operated */
    SHR_BITDCLNAME(fbmp, BCMCTH_AGM_CONTROL_FIELDS_MAX);
} bcmcth_agm_control_t;

/*!
 * \brief Update MON_AGM_CONTROLt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_AGM_CONTROLt table.
 *
 * \param [in] unit Unit number.
 * \param [in] agm_ctrl bcmcth_agm_control_t structure to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_agm_control_set_f)(int unit,
                                        bcmcth_agm_control_t *agm_ctrl);

/*!
 * \brief AGM driver object
 *
 * AGM driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH AGM module by
 * \ref bcmcth_agm_drv_init() from the top layer. BCMCTH AGM internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_agm_drv_s {
    /*! Update AGM hardware register(s) */
    bcmcth_agm_control_set_f agm_control_set;
} bcmcth_agm_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_agm_drv_t *_bd##_cth_agm_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Get the AGM driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_agm_drv_init(int unit);

/*!
 * \brief Initialize device-specific AGM driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_agm_init(int unit);

/*!
 * \brief Clean up device-specific AGM driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_agm_cleanup(int unit);

/*!
 * \brief Update MON_AGM_CONTROLt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_AGM_CONTROLt table.
 *
 * \param [in] unit Unit number.
 * \param [in] agm_ctrl bcmcth_agm_control_t structure to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
extern int
bcmcth_agm_control_set(int unit,
                       bcmcth_agm_control_t *agm_ctrl);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The AGM custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmcth_agm_imm_init(int unit);

#endif /* BCMCTH_AGM_DRV_H */
