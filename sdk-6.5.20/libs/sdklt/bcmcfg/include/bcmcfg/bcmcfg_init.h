/*! \file bcmcfg_init.h
 *
 * BCMCFG initialization interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_INIT_H
#define BCMCFG_INIT_H

#include <sal/sal_types.h>

/*!
 * \brief BCMCFG basic initialization.
 *
 * Perform non-HA part of system manager stage "init".
 *
 * \param [in] unit Unit number.
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcfg_init(int unit, bool warm);

/*!
 * \brief BCMCFG configuration load.
 *
 * Perform non-HA part of system manager stage "config".
 *
 * \param [in] unit Unit number.
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcfg_config(int unit, bool warm);

/*!
 * \brief BCMCFG per-unit shutdown.
 *
 * Perform non-HA part of system manager stage "shutdown".
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcfg_shutdown(int unit);

/*!
 * \brief Check if BCMCFG is ready for basic queries.
 *
 * \param [in] unit Unit number.
 *
 * \retval true BCMCFG is ready.
 * \retval false BCMCFG is not ready.
 */
extern bool
bcmcfg_ready(int unit);

#endif /* BCMCFG_INIT_H */
