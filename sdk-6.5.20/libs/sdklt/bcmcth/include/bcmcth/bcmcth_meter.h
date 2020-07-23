/*! \file bcmcth_meter.h
 *
 * BCMCTH Meter top-level APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_H
#define BCMCTH_METER_H

#include <sal/sal_types.h>

/*!
 * \brief Attach METER driver.
 *
 * Initialize the feature and install the driver functions.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_meter_attach(int unit, bool warm);

/*!
 * \brief Detach METER driver.
 *
 * Cleanup the resources allocated during atatch.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_meter_detach(int unit);

#endif /* BCMCTH_METER_H */
