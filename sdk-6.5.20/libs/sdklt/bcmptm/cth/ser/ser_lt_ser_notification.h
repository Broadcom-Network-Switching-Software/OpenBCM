/*! \file ser_lt_ser_notification.h
 *
 * Interface functions for SER_NOTIFICATION IMM LT.
 *
 * Used to Update data to SER_NOTIFICATION IMM LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_LT_SER_NOTIFICATION_H
#define SER_LT_SER_NOTIFICATION_H

#include "bcmdrd/bcmdrd_types.h"

/*!
 * \brief Sync cache data to IMM LT SER_NOTIFICATION.
 *
 *
 * \param [in] unit                Unit number.
 * \param [in] sid                 PT ID.
 * \param [in] hw_fault            H/W fault.
 * \param [in] high_severity_err   High severity SER error.
 * \param [in] insert              1: insert entry to this LT, 0: update entry of this LT.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_hw_fault_notify(int unit, bcmdrd_sid_t sid,
                           bool hw_fault, bool high_severity_err);

/*!
 * \brief Init IMM LT SER_NOTIFICATION.
 *
 *
 * \param [in] unit                Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_notification_init(int unit);

#endif /* SER_LT_SER_NOTIFICATION_H */

