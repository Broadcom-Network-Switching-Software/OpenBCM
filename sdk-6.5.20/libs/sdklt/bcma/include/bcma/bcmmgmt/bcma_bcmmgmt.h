/*! \file bcma_bcmmgmt.h
 *
 * Common functions for bcmmgmt commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMMGMT_H
#define BCMA_BCMMGMT_H

#include <bcmmgmt/bcmmgmt.h>

/*! Maximum number of YAML configuration files supported */
#define BCMA_BCMMGMT_MAX_NUM_CONFIG_FILES    8

/*!
 * \brief Specify SDK configuration file(s).
 *
 * This function can be called multiple times until all configuration
 * file slots have been filled.
 *
 * Set \c cfgfile_idx = -1 to use next available slot, or use values
 * starting from zero to select a specific slot.
 *
 * If \c cfgfile_idx = -1 and \c cfgfile is NULL, then all previously
 * installed configuration files will be removed.
 *
 * This function must be called prior to \ref bcma_bcmmgmt_init
 * to make the SDK configuration files take effect.
 *
 * \param [in] cfgfile SDK configuration file name.
 * \param [in] cfgfile_idx The configuration file index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM File name invalid or too long.
 * \retval SHR_E_FULL Too many configuration files.
 * \retval SHR_E_NOT_FOUND File does not exist.
 */
extern int
bcma_bcmmgmt_cfgyml_file_set(const char *cfgfile, int cfgfile_idx);

/*!
 * \brief Get next installed configuration file.
 *
 * Retrieve configuration files which were previosuly installed via
 * \ref bcma_bcmmgmt_cfgyml_file_set.
 *
 * To get the first installed configuration file, \c cfgfile must be
 * set to NULL. To get the following configuration file, \c cfgfile
 * must be set to the pointer returned in the previous call.
 *
 * \param [in] cfgfile Pointer to previous configuration file.
 *
 * \return Pointer to next configuration file, or NULL if not found.
 */
extern const char *
bcma_bcmmgmt_cfgyml_file_get(const char *cfgfile);

/*!
 * \brief Perform default SDK start-up sequence.
 *
 * This function will start all SDK components and attach all devices.
 *
 * Prior to calling this function, devices must be created in the DRD
 * and any SDK configuration files must be specified via \ref
 * bcma_bcmmgmt_cfgyml_file_set.
 *
 * If no SDK configuration files were specified, \c default_cfgyml_file
 * will be used as the default configuration.
 * No error is reported if \c default_cfgyml_file is NULL or the file is not
 * found.
 *
 * If the warm boot flag \c warm is set, the specified SDK configuration
 * files will be ignored.
 *
 * If the \c core_only flag is set while a non-NULL ISSU \c ver_info
 * is provided, the ISSU process will leave the system in an undefined
 * state. Hence this parameter combination will be rejected with a \c
 * SHR_E_INIT error.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 * \param [in] core_only Set to true to skip attaching devices.
 * \param [in] default_cfgyml_file Name of SDK default configuration file.
 * \param [in] ver_info Structure with strings specifying prior
 *                      and current SDKLT versions. If NULL, no ISSU.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core components failed to start.
 * \retval SHR_E_PARTIAL One or more devices did failed to start.
 * \retval SHR_E_INIT Core-only ISSU initialization is not permitted.
 */
extern int
bcma_bcmmgmt_init(bool warm, bool core_only,
                  const char *default_cfgyml_file,
                  const bcmmgmt_issu_info_t *ver_info);

/*!
 * \brief Shut down the SDK System Manager.
 *
 * This function will detach all running devices and stop all
 * registered SDK components.
 *
 * \param [in] graceful Indicates if the shutdown should complete all
 *                      pending LT operations or simply shut down as
 *                      fast as possible.
 * \param [in] keep_netif Indicates if the shutdown should retain
 *                        network interfaces managed by the operating
 *                        system (e.g. Linux KNET network interfaces).
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcma_bcmmgmt_shutdown(bool graceful, bool keep_netif);

/*!
 * \brief Initialize SDK core and load SDK configuration files.
 *
 * This function is normally only for internal use by \ref
 * bcma_bcmmgmt_start, but can be used to allow early access to
 * configuration variables.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL SDK core initialization failed.
 */
extern int
bcma_bcmmgmt_config_load(bool warm);

/*!
 * \brief SDK core start-up sequence.
 *
 * This function will start SDK core components and attach all devices,
 * including reload the SDK configuration files.
 *
 * This function can be call be called after \ref bcma_bcmmgmt_stop
 * to restart the whole system.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 * \param [in] core_only Set to true to skip attaching devices.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core components failed to start.
 * \retval SHR_E_PARTIAL One or more devices failed to start.
 * \retval SHR_E_INTERNAL \ref bcma_bcmmgmt_init is not done.
 */
extern int
bcma_bcmmgmt_start(bool warm, bool core_only);

/*!
 * \brief SDK core stop sequence.
 *
 * This function will detach all devices and stop SDK core components.
 *
 * As opposed to \ref bcma_bcmmgmt_shutdown, the System Manager will remain
 * configured.
 *
 * \return SHR_E_NONE on success, otherwise error code.
 */
extern int
bcma_bcmmgmt_stop(void);

#endif /* BCMA_BCMMGMT_H */
