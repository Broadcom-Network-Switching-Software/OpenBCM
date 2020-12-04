/*! \file bcmpc_cmicx_pmac.h
 *
 * Shared PMAC access driver for CMICx-based devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_CMICX_PMAC_H
#define BCMPC_CMICX_PMAC_H

#include <bcmpc/bcmpc_drv_internal.h>

/*!
 * \brief Get PMAC access driver for a physical port.
 *
 * The PMAC access driver is passed to the PMAC driver as a callback
 * function for the actual hardware access.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number on this unit.
 *
 * \return PMAC access driver.
 */
extern bcmpmac_reg_access_t *
bcmpc_cmicx_pmac_bus_get(int unit, bcmpc_pport_t pport);

#endif /* BCMPC_CMICX_PMAC_H */
