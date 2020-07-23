/*! \file bcmecmp_dev.h
 *
 * This file contains device ECMP initialization and cleanup functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_DEV_H
#define BCMECMP_DEV_H
/*!
 * \brief Device ECMP feature initialiation function.
 *
 * Initializes ECMP feature based on the device ECMP resolution mode.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmecmp_dev_init(int unit, bool warm);

/*!
 * \brief Device ECMP feature cleanup function.
 *
 * Releases resources allocated for ECMP feature.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmecmp_dev_cleanup(int unit, bool warm);

/*!
 * \brief Device ECMP pre-configuration function.
 *
 * This function is called during warm start for ECMP groups pre-processing.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmecmp_dev_pre_config(int unit, bool warm);

#endif /* BCMECMP_DEV_H */
