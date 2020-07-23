/*! \file bcmmgmt.h
 *
 * Top-level starting and stopping the SDK.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMMGMT_H
#define BCMMGMT_H

#include <bcmmgmt/bcmmgmt_sysm.h>

/*!
 * \brief ISSU versions list.
 */
typedef struct bcmmgmt_issu_info_s {

    /*!
      * String containing ISSU start SDKLT version,
      * from START_SDK_VERSION supplied during SDKLT build.
      */
    char *start_ver;

    /*!
      * String containing ISSU current SDKLT version,
      * from SDK_VERSION supplied during SDKLT build.
      */
    char *current_ver;

} bcmmgmt_issu_info_t;

/*!
 * \brief Start ISSU upgrade.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 * \param [in] issu_info If NULL there is no ISSU event, otherwise
 *                       this structure contains the start and current versions.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmmgmt_issu_start(bool warm, const bcmmgmt_issu_info_t *issu_info);

/*!
 * \brief ISSU upgrade done.
 *
 * This function releases all the resources that were used by the ISSU. In
 * particular it unloads the ISSU DLL. It is assume that this function is being
 * called once all the units had completed their upgrade.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmmgmt_issu_done(void);

/*!
 * \brief Perform default SDK start-up sequence.
 *
 * This function will start all SDK components and attach all devices.
 *
 * Prior to calling this function, devices must be created in the DRD.
 * The specified SDK configuration file \c conf_file is loaded via \ref
 * bcmmgmt_core_config_load in this function. The configuration file
 * \c conf_file will be ignored if the warm boot flag \c warm is set.
 *
 * No error is reported if \c conf_file is NULL or the file is not
 * found.
 *
 * If the operating environment does not support file I/O, please
 * refer to \ref bcmmgmt_core_config_parse.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 * \param [in] conf_file Name of SDK configuration file.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core components failed to start.
 * \retval SHR_E_PARTIAL One or more devices failed to start.
 */
extern int
bcmmgmt_init(bool warm, const char *conf_file);

/*!
 * \brief Shut down the SDK System Manager.
 *
 * This function will detach all running devices and stop all
 * registered SDK components. Any configured network interfaces will
 * be removed as well.
 *
 * \param [in] graceful Indicates if the shutdown should complete all
 *                      pending LT operations or simply shut down as
 *                      fast as possible.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmmgmt_shutdown(bool graceful);

/*!
 * \brief Shut down System Manager, but keep network interfaces.
 *
 * This function will detach all running devices and stop all
 * registered SDK components.
 *
 * Network interfaces managed by the operating system (such as the
 * Linux KNET module) will keep functioning.
 *
 * \param [in] graceful Indicates if the shutdown should complete all
 *                      pending LT operations or simply shut down as
 *                      fast as possible.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmmgmt_shutdown_keep_netif(bool graceful);

/*!
 * \brief Shutdown callback function type.
 *
 * Functions of this type can be registered via \ref
 * bcmmgmt_shutdown_cb_register.
 *
 * When a device is detached, all callbacks are executed with the unit
 * number of the detached device.
 *
 * When the SDK core is shut down, all callbacks are executed with the
 * unit number set to -1.
 *
 * \param [in] unit Unit number.
 * \param [in] cb_data Context provided during registration.
 */
typedef void (*bcmmgmt_shutdown_cb_f)(int unit, void *cb_data);

/*!
 * \brief Register a shutdown callback function.
 *
 * This function can be called multiple times with different callback
 * functions.
 *
 * If a callback has been registered already, this function will
 * return success without adding any new entry in the callback
 * function list.
 *
 * \param [in] cb_func Callback function.
 * \param [in] cb_data Context to be passed back (may be NULL).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Insufficient resources for registration.
 */
extern int
bcmmgmt_shutdown_cb_register(bcmmgmt_shutdown_cb_f cb_func, void *cb_data);

/*!
 * \brief Unregister a shutdown callback function.
 *
 * Remove a function which was previously registered via \ref
 * bcmmgmt_shutdown_cb_register.
 *
 * \param [in] cb_func Callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmmgmt_shutdown_cb_unregister(bcmmgmt_shutdown_cb_f cb_func);

/*!
 * \brief Initialize core utilities.
 *
 * This function initializes and instantiates various core utilities
 * of the SDK.
 *
 * This function must be called before the System Manager is started.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core utilities failed to initialize.
 */
extern int
bcmmgmt_core_init(void);

/*!
 * \brief Clean up core utilities.
 *
 * This function cleans up all initialization done by \ref
 * bcmmgmt_core_init.
 *
 * \return Always SHR_E_NONE.
 */
extern int
bcmmgmt_core_cleanup(void);

/*!
 * \brief Load and parse SDK configuration file.
 *
 * This function may be called multiple times with different file names.
 *
 * The SDK core utilities must be initialized via \ref
 * bcmmgmt_core_init before this function is called.
 *
 * When this function is needed, the call to \ref bcmmgmt_init should
 * be replaced with a sequence like this (error checking skipped for
 * clarity):
 *
 * \code
 * bcmmgmt_core_init();
 * if (!warm_boot) {
 *     bcmmgmt_core_config_load(conf_file1);
 *     bcmmgmt_core_config_load(conf_file2);
 * }
 * bcmmgmt_core_start(warm_boot, bcmmgmt_sysm_default_comp_list());
 * bcmlt_device_attach(unit, warm_boot);
 * \endcode
 *
 * \param [in] conf_file Name of SDK configuration file.
 *
 * \retval SHR_E_NONE Configuration file successfully parsed.
 * \retval SHR_E_INIT Core utilities not initialized.
 * \retval SHR_E_PARAM Invalid file name.
 * \retval SHR_E_NOT_FOUND Configuration file not found.
 * \retval SHR_E_CONFIG Error parsing configuration file.
 */
extern int
bcmmgmt_core_config_load(const char *conf_file);

/*!
 * \brief Parse SDK configuration string.
 *
 * This function parses a YAML-formatted SDK configuration string,
 * which is useful for environments without a file system.
 *
 * The function may be called multiple times with different YAML
 * configuration strings.
 *
 * The SDK core utilities must be initialized via \ref
 * bcmmgmt_core_init before this function is called.
 *
 * When this function is needed, the call to \ref bcmmgmt_init should
 * be replaced with a sequence like this (error checking skipped for
 * clarity):
 *
 * \code
 * bcmmgmt_core_init();
 * if (!warm_boot) {
 *     bcmmgmt_core_config_parse(yaml_str1);
 *     bcmmgmt_core_config_parse(yaml_str2);
 * }
 * bcmmgmt_core_start(warm_boot, bcmmgmt_sysm_default_comp_list());
 * bcmlt_device_attach(unit, warm_boot);
 * \endcode
 *
 * \param [in] yml_str YAML-formatted SDK configuration string.
 *
 * \retval SHR_E_NONE Configuration string successfully parsed.
 * \retval SHR_E_INIT Core utilities not initialized.
 * \retval SHR_E_CONFIG Error parsing configuration string.
 */
extern int
bcmmgmt_core_config_parse(const char *yml_str);

/*!
 * \brief Start core components.
 *
 * This function will register all the specified SDK components with
 * the System Manager and start the device-independent part of the
 * components.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 * \param [in] comp_list List of component start functions.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core components failed to start.
 */
extern int
bcmmgmt_core_start(bool warm, const bcmmgmt_comp_list_t *comp_list);

/*!
 * \brief Shut down all core components of the SDK.
 *
 * \param [in] graceful Indicates if the shutdown should complete all
 *                      pending LT operations or simply shut down as
 *                      fast as possible.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more core components did not start correctly.
 */
extern int
bcmmgmt_core_stop(bool graceful);

/*!
 * \brief Attach device.
 *
 * This function will register a device with the BCMLT API and the
 * System Manager, after which the System Manager will start the
 * device-specific parts of the installed SDK components.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL The device failed to attach.
 */
extern int
bcmmgmt_dev_attach(int unit, bool warm);

/*!
 * \brief Check if a device is attached.
 *
 * Check if a device has been attached using \ref bcmmgmt_dev_attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Device is attached.
 * \retval false Device is not attached.
 */
extern bool
bcmmgmt_dev_attached(int unit);

/*!
 * \brief Attach all devices.
 *
 * This function will call \ref bcmmgmt_dev_attach for all devices
 * that exist in the DRD.
 *
 * The System Manager must be started via \ref bcmmgmt_core_start
 * before calling this API.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARTIAL One or more devices did failed to attach.
 */
extern int
bcmmgmt_dev_attach_all(bool warm);

/*!
 * \brief Detach a device.
 *
 * This function will stop the device-specific parts of the components
 * registered with the System Manager and unregister the device from
 * the BCMLT API.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL The device failed to detach.
 */
extern int
bcmmgmt_dev_detach(int unit);

/*!
 * \brief Detach all devices.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL One or more devices failed to detach.
 */
extern int
bcmmgmt_dev_detach_all(void);

/*!
 * \brief Initialize packet I/O including all required modules for a device.
 *
 * When warm-booting, the function may be called prior to \ref bcmmgmt_init.
 *
 * \param [in] warm Set to true to use warm-boot start-up sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Packet I/O failed to initialize.
 */
extern int
bcmmgmt_pkt_dev_init_all(bool warm);

#endif /* BCMMGMT_H */
