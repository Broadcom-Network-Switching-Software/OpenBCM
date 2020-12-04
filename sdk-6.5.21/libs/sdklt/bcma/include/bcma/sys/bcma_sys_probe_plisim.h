/*! \file bcma_sys_probe_plisim.h
 *
 * Probe for switch devices via PLI network protocol.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SYS_PROBE_PLISIM_H
#define BCMA_SYS_PROBE_PLISIM_H

/*!
 * \brief Clean up resources allocated by \ref bcma_sys_probe_plisim.
 *
 * Please refer to \ref bcma_sys_probe_cleanup.
 *
 * \return Always 0.
 */
int
bcma_sys_probe_plisim_cleanup(void);

/*!
 * \brief Probe for supported devices via PLI network protocol.
 *
 * Please refer to \ref bcma_sys_probe.
 *
 * \return Number of devices successfully added to the DRD.
 */
extern int
bcma_sys_probe_plisim(void);

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
bcma_sys_probe_plisim_override(int unit, const char *dev_name);

/*!
 * \brief Configure probing behavior.
 *
 * Please refer to \ref bcma_sys_probe_config_set.
 *
 * \param [in] config System-specific string, e.g. "noschanrpc".
 *
 * \retval 0 Configuration successfully applied.
 * \retval -1 Specified configuration not supported.
 */
extern int
bcma_sys_probe_plisim_config_set(const char *config);

/*!
 * \brief Get probe configuration.
 *
 * Please refer to \ref bcma_sys_probe_config_get.
 *
 * \param [in] prev Previous configuration option (NULL to get first).
 *
 * \return Pointer to next supported configuration option.
 */
extern const char *
bcma_sys_probe_plisim_config_get(const char *prev);

#endif /* BCMA_SYS_PROBE_PLISIM_H */
