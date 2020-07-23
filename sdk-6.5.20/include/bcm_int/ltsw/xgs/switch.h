/*! \file switch.h
 *
 * Switch control function declare APIs for XGS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_XGS_SC_H
#define BCMINT_XGS_SC_H

#include <bcm/types.h>
#include <bcm/switch.h>

/*!
 * \brief Configure a port based control.
 *
 * \param [in] unit Unit number
 * \param [in] port Port number.
 * \param [in] type Control type.
 * \param [in] arg Control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xgs_scp_set(int unit, int port, bcm_switch_control_t type, int arg);

/*!
 * \brief Retrieve a port based control setting.
 *
 * \param [in] unit Unit number
 * \param [in] port Port number.
 * \param [in] type Control type.
 * \param [out] arg Control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xgs_scp_get(int unit, int port, bcm_switch_control_t type, int *arg);

/*!
 * \brief Configure a device-wide control.
 *
 * \param [in] unit Unit number
 * \param [in] type Control type.
 * \param [in] arg Control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xgs_sc_set(int unit, bcm_switch_control_t type, int arg);


/*!
 * \brief Retrieve a device-wide control setting.
 *
 * \param [in] unit Unit number
 * \param [in] type Control type.
 * \param [out] arg Control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xgs_sc_get(int unit, bcm_switch_control_t type, int *arg);

/*!
 * \brief Switch module initialization.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xgs_switch_init(int unit);

/*!
 * \brief Switch module detach.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Entry not found.
 */
extern int
bcmint_xgs_switch_detach(int unit);

#endif /* BCMINT_XGS_SC_H */
