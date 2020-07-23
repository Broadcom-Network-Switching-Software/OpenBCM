/*! \file bcmbd_otp.h
 *
 * APis for extracting OTP (One-Time Programming) information from
 * switch devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_OTP_H
#define BCMBD_OTP_H

#include <sal/sal_types.h>

/*!
 * OTP parameter types.
 *
 * Used by \ref bcmbd_otp_param_get.
 */
typedef enum bcmbd_otp_param_e {

    /*! Recommended AVS voltage in mV. */
    BCMBD_OTP_PRM_AVS_VOLTAGE,

    /* Must be last. */
    BCMBD_OTP_PRM_COUNT

} bcmbd_otp_param_t;

/*!
 * \brief Get OTP parameter value.
 *
 * \param [in] unit Unit number.
 * \param [in] otp_param Type of parameter to get.
 * \param [out] val OTP parameter value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT OTP driver not initialized.
 * \retval SHR_E_UNAVAIL OTP parameter not supported for this device.
 */
extern int
bcmbd_otp_param_get(int unit, bcmbd_otp_param_t otp_param, uint32_t *val);

#endif /* BCMBD_OTP_H */
