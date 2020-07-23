/*! \file bcma_sys_conf_hybrid_sdk.h
 *
 * Example functions for initializing a simple SDK-based application for hybrid system.
 *
 * The initialization is built primarily using the building blocks
 * provided in \ref bcma_sys_conf.h.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SYS_CONF_HYBRID_SDK_H
#define BCMA_SYS_CONF_HYBRID_SDK_H

#include <bcma/sys/bcma_sys_conf_sdk.h>

/*!
 * \brief Common SDK initialization for hybrid system.
 *
 * This function performs a typical SDK initialization sequence in
 * full or partially.
 *
 * Setting init_stage to 0 will perform a full initialization.
 *
 * \param [in] init_stage Initialization stage to execute.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_hybrid_sdk_init(bcma_sys_init_stage_t init_stage);

/*!
 * \brief CLI redirection hook for BSL output sink.
 *
 * This function should be installed into the BSL output hook to allow
 * command output to be fed back into the CLI input function.
 *
 * The call signature is identical to the BSL output hook itself.
 *
 * \param [in] meta Meta data for this log message.
 * \param [in] format A printf-style format string.
 * \param [in] args List of printf-style arguments.
 *
 * \return Negative value on error.
 */
extern int
bcma_sys_conf_hybrid_sdk_cli_redir_bsl(bsl_meta_t *meta, const char *format,
                                       va_list args);

/*!
 * \brief Common SDK run main thread for hybrid system.
 *
 * This function runs the main SDK thread, which is typically a CLI
 * interface.
 *
 * \param [in] unit Unit number.
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully.
 * \retval BCMA_CLI_CMD_FAIL CLI is not initialized successfully.
 */
extern int
bcma_sys_conf_hybrid_sdk_run(int unit);

/*!
 * \brief bcmlt shell for hybrid system.
 *
 * This function is used to switch command line to bcmlt shell.
 *
 * \param [in] unit Unit number.
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully.
 * \retval BCMA_CLI_CMD_FAIL CLI is not initialized successfully.
 */
extern int
bcma_sys_conf_hybrid_sdk_bsh(int unit);

/*!
 * \brief Process and execute command line on bcmlt shell.
 *
 * <long-description>
 *
 * \param [in] unit Unit number.
 * \param [in] str Raw command string
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully.
 * \retval BCMA_CLI_CMD_NOT_FOUND Command was not recognized.
 * \retval BCMA_CLI_CMD_AMBIGUOUS Command was not unique.
 * \retval BCMA_CLI_CMD_EXIT Command requested shell to terminate.
 * \retval BCMA_CLI_CMD_* Command returned an error.
 */
extern int
bcma_sys_conf_hybrid_sdk_bsh_process(int unit, const char *str);

/*!
 * \brief sdklt debug shell for hybrid system.
 *
 * This function is used to switch command line to sdklt debug shell.
 *
 * \param [in] unit Unit number.
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully.
 * \retval BCMA_CLI_CMD_FAIL CLI is not initialized successfully.
 */
extern int
bcma_sys_conf_hybrid_sdk_dsh(int unit);

/*!
 * \brief Process and execute command line on sdklt debug shell.
 *
 * <long-description>
 *
 * \param [in] unit Unit number.
 * \param [in] str Raw command string
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully.
 * \retval BCMA_CLI_CMD_NOT_FOUND Command was not recognized.
 * \retval BCMA_CLI_CMD_AMBIGUOUS Command was not unique.
 * \retval BCMA_CLI_CMD_EXIT Command requested shell to terminate.
 * \retval BCMA_CLI_CMD_* Command returned an error.
 */
extern int
bcma_sys_conf_hybrid_sdk_dsh_process(int unit, const char *str);

/*!
 * \brief Common SDK clean-up for hybrid system.
 *
 * This function reverts the SDK initialization sequence performed by
 * \ref bcma_sys_conf_hybrid_sdk_init.
 *
 * Setting init_stage to 0 will perform a full clean-up.
 *
 * \param [in] init_stage Initialization stage to execute.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_hybrid_sdk_cleanup(bcma_sys_init_stage_t init_stage);

/*!
 * \brief Attach device.
 *
 * This function will start the device-specific parts of
 * the components registered into the System Manager.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_hybrid_sdk_attach(int unit);

/*!
 * \brief Check if a device is attached.
 *
 * Check if a device has been attached using
 * \ref bcma_sys_conf_hybrid_sdk_attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Device is attached.
 * \retval false Device is not attached.
 */
extern bool
bcma_sys_conf_hybrid_sdk_attached(int unit);

/*!
 * \brief Detach device.
 *
 * This function will stop the device-specific parts of
 * the components registered into the System Manager.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_hybrid_sdk_detach(int unit);

/*!
 * \brief Set system configuration option for hybrid system.
 *
 * A configuration option can be passed as an integer value, a string
 * value, or a combination of the two.
 *
 * Please refer to \ref bcma_sys_conf_opt_t for a description of the
 * supported configuration options.
 *
 * \param [in] opt Configuration option to set.
 * \param [in] val Integer value for the specified configuration option.
 * \param [in] valstr String value for the specified configuration option.
 *
 * \retval 0 No errors
 * \retval -1 Unrecognized option or invalid setting.
 */
extern int
bcma_sys_conf_hybrid_sdk_option_set(bcma_sys_conf_opt_t opt,
                                    int val, const char *valstr);

#endif /* BCMA_SYS_CONF_HYBRID_SDK_H */
