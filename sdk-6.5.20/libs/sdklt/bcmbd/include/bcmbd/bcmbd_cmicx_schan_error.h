/*! \file bcmbd_cmicx_schan_error.h
 *
 * Override default S-channel error handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_ERROR_H
#define BCMBD_CMICX_ERROR_H

#include <sal/sal_types.h>

/*!
 * \brief Function to handle S-channel PIO errors.
 *
 * A function of this type can be used to override the default
 * behavior of the S-channel PIO error function.
 *
 * If the function returns an error, the normal error log message will
 * be shown, i.e. the override function only needs to determine
 * whether a real error heppened or not.
 *
 * \param [in] unit Unit number.
 * \param [in] ch S-channel number.
 * \param [in] schan_msg S-channel message buffer.
 * \param [in] reg_val Content of CMIC_SCHAN_CTRL register.
 *
 * \retval SHR_E_NONE No S-channel error tp report.
 * \retval SHR_E_FAIL S-channel operation failed.
 */
typedef int (*bcmbd_cmicx_schan_error_f)(int unit, int ch,
                                         uint32_t *schan_msg,
                                         uint32_t reg_val);

/*!
 * \brief Configure S-channel PIO error function.
 *
 * This function will be invoked is the S-channel PIO error bit is set
 * after an S-channel operation completes.
 *
 * The installed function should consult all relevant error
 * information (and optionally the originating S-channel message) and
 * decide whether the error condition is fatal.
 *
 * \param [in] unit Unit number.
 * \param [in] efunc S-channel PIO error function to install.
 *
 * \retval SHR_E_NONE Function successfully installed.
 */
extern int
bcmbd_cmicx_schan_error_func_set(int unit, bcmbd_cmicx_schan_error_f efunc);

/*!
 * \brief Install S-channel error function for reduced number of pipes.
 *
 * Some access types will cause invalid S-channel errors if not all
 * pipes are enabled. This function will install an appropriate
 * S-channel error handler function for such device configuration via
 * \ref bcmbd_cmicx_schan_error_func_set.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Function successfully installed.
 */
extern int
bcmbd_cmicx_schan_subpipe_error_func_init(int unit);

#endif /* BCMBD_CMICX_ERROR_H */
