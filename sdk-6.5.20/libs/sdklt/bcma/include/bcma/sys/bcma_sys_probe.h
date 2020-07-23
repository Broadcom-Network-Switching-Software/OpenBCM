/*! \file bcma_sys_probe.h
 *
 * Probe for siwtch devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SYS_PROBE_H
#define BCMA_SYS_PROBE_H

/*!
 * \brief Clean up resources allocated by \ref bcma_sys_probe.
 *
 * \return Always 0.
 */
int
bcma_sys_probe_cleanup(void);

/*!
 * \brief Probe for supported devices.
 *
 * This is a system-specific function which will probe for supported
 * switch devices and add them to the device manager in the Device
 * Resource Database (DRD).
 *
 * \return Number of devices successfully added to the DRD.
 */
extern int
bcma_sys_probe(void);

/*!
 * \brief Override probed device type.
 *
 * This function is typically used when a specific SKU (bond-out) is
 * not available for testing yet. In this case a base device can be
 * used instead, but the SDK will treat it as the specified SKU.
 *
 * The override device type must be specified by its device name.
 *
 * \param [in] unit Unit number.
 * \param [in] dev_name Device name, e.g. "bcm56800_a0".
 *
 * \retval 0 Override successfully enabled.
 * \retval -1 Specified device not supported.
 */
extern int
bcma_sys_probe_override(int unit, const char *dev_name);

/*!
 * \brief Configure probing behavior.
 *
 * This function is used to modify the default behavior of the probing
 * function by passing a configuration option string to the probe
 * function.
 *
 * The available configuration options are system-specific.
 *
 * \param [in] config System-specific string, e.g. "noschanrpc".
 *
 * \retval 0 Configuration successfully applied.
 * \retval -1 Specified configuration not supported.
 */
extern int
bcma_sys_probe_config_set(const char *config);

/*!
 * \brief Get probe configuration.
 *
 * This function may be used to retrieve the configuration options
 * supported by the probe function for the current system.
 *
 * Passing a NULL pointer will return a pointer to the first supported
 * configuration option. Passing this option in a second call will
 * return the next supported configuration option and so forth.
 *
 * A NULL pointer will be returned when the list of supported
 * configuration options is exhausted.
 *
 * Example:
 *
 * \code{.c}
 * {
 *     const char *probe_cfg;
 *     printf("Valid probe configurations are:\n");
 *     probe_cfg = NULL;
 *     do {
 *         probe_cfg = bcma_sys_probe_config_get(probe_cfg);
 *         if (probe_cfg) {
 *             printf("%s\n", probe_cfg);
 *         }
 *     } while (probe_cfg);
 * }
 * \endcode
 *
 * \param [in] prev Previous configuration option (NULL to get first).
 *
 * \return Pointer to next supported configuration option.
 */
extern const char *
bcma_sys_probe_config_get(const char *prev);

#endif /* BCMA_SYS_PROBE_H */
