/*! \file bcmcth_trunk_vp_dev.h
 *
 * This file contains device TRUNK_VP initialization and cleanup functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_VP_DEV_H
#define BCMCTH_TRUNK_VP_DEV_H
/*!
 * \brief Device TRUNK_VP feature initialiation function.
 *
 * Initializes TRUNK_VP feature based on the device TRUNK_VP resolution mode.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_trunk_vp_dev_init(int unit, bool warm);

/*!
 * \brief Device TRUNK_VP feature cleanup function.
 *
 * Releases resources allocated for TRUNK_VP feature.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_trunk_vp_dev_cleanup(int unit, bool warm);

#endif /* BCMCTH_TRUNK_VP_DEV_H */
