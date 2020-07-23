/*! \file bcmbd_miim_internal.h
 *
 * BD MIIM internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_MIIM_INTERNAL_H
#define BCMBD_MIIM_INTERNAL_H

#include <bcmbd/bcmbd_miim.h>

/*!
 * \brief See \ref bcmbd_miim_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_MEMORY Insufficient resources.
 */
typedef int (*bcmbd_miim_init_f)(int unit);

/*!
 * \brief See \ref bcmbd_miim_cleanup.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmbd_miim_cleanup_f)(int unit);

/*!
 * \brief See \ref bcmbd_miim_op.
 *
 * \param [in] unit Unit number.
 * \param [in] op Structure with MIIM operation parameters.
 * \param [in,out] data Data to write or data read.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation did not complete within normal time.
 */
typedef int (*bcmbd_miim_op_f)(int unit, bcmbd_miim_op_t *op, uint32_t *val);

/*!
 * \brief Configure MIIM bus parameters.
 *
 * \brief See \ref bcmbd_miim_rate_config_set.
 *
 * \param [in] unit Unit number.
 * \param [in] internal Apply configuration to internal MIIM bus.
 * \param [in] busno MIIM bus number (use -1 for all).
 * \param [in] ratecfg MIIM data rate configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Unsupported MIIM bus parameter.
 */
typedef int (*bcmbd_miim_rate_config_set_f)(int unit, bool internal, int busno,
                                            bcmbd_miim_rate_config_t *ratecfg);

/*!
 * \brief MIIM driver object.
 */
typedef struct bcmbd_miim_drv_s {

    /*! Initialize MIIM driver. */
    bcmbd_miim_init_f init;

    /*! Clean up MIIM driver. */
    bcmbd_miim_cleanup_f cleanup;

    /*! Execute MIIM operation. */
    bcmbd_miim_op_f miim_op;

    /*! Configure MIIM bus data rate, etc. */
    bcmbd_miim_rate_config_set_f rate_config_set;

} bcmbd_miim_drv_t;

/*!
 * \brief Assign device-specific MIIM driver.
 *
 * \param [in] unit Unit number.
 * \param [in] miim_drv Driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_miim_drv_init(int unit, const bcmbd_miim_drv_t *miim_drv);

#endif /* BCMBD_MIIM_INTERNAL_H */
