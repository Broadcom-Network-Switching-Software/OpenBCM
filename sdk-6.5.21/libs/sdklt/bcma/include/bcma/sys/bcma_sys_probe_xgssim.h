/*! \file bcma_sys_probe_xgssim.h
 *
 * Probe for switch devices in built-in XGS simulator.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SYS_PROBE_XGSSIM_H
#define BCMA_SYS_PROBE_XGSSIM_H

/*!
 * \brief Clean up resources allocated by \ref bcma_sys_probe_xgssim.
 *
 * Please refer to \ref bcma_sys_probe_cleanup.
 *
 * \return Always 0.
 */
int
bcma_sys_probe_xgssim_cleanup(void);

/*!
 * \brief Probe for supported devices in built-in XGS simulator.
 *
 * Please refer to \ref bcma_sys_probe.
 *
 * \return Number of devices successfully added to the DRD.
 */
extern int
bcma_sys_probe_xgssim(void);

/*!
 * \brief Override probed device type.
 *
 * Please refer to \ref bcma_sys_probe_override.
 *
 * \param [in] unit Unit number.
 * \param [in] dev_name Device name, e.g. "bcm56800_a0".
 *
 * \retval 0 Override successfully enabled.
 * \retval -1 Specified device not supported.
 */
extern int
bcma_sys_probe_xgssim_override(int unit, const char *dev_name);

#endif /* BCMA_SYS_PROBE_XGSSIM_H */
