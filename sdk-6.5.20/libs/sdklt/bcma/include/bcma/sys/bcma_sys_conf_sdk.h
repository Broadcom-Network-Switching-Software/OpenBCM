/*! \file bcma_sys_conf_sdk.h
 *
 * Example functions for initializing a simple SDK-based application.
 *
 * The initialization is built primarily using the building blocks
 * provided in \ref bcma_sys_conf.h.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SYS_CONF_SDK_H
#define BCMA_SYS_CONF_SDK_H

#include <bcma/sys/bcma_sys_conf.h>

/*!
 * \brief System initialization stages.
 *
 * Used to do only partial initialization when running
 * applications. Mainly intended for debugging.
 */
typedef enum bcma_sys_init_stage_e {
    BCMA_INIT_STAGE_ALL = 0,
    BCMA_INIT_STAGE_BASE,
    BCMA_INIT_STAGE_CLI,
    BCMA_INIT_STAGE_PROBE,
    BCMA_INIT_STAGE_SYSM,
    BCMA_INIT_STAGE_ATTACH,
    BCMA_INIT_STAGE_MAX
} bcma_sys_init_stage_t;

/*!
 * \brief Common SDK initialization.
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
bcma_sys_conf_sdk_init(bcma_sys_init_stage_t init_stage);

/*!
 * \brief Common SDK run main thread.
 *
 * This function runs the main SDK thread, which is typically a CLI
 * interface.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_sdk_run(void);

/*!
 * \brief Common SDK clean-up.
 *
 * This function reverts the SDK initialization sequence performed by
 * \ref bcma_sys_conf_init.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_sdk_cleanup(void);

/*!
 * \brief Run debug shell.
 *
 * This function launches the debug shell, which can be useful for
 * debugging the SDK initialization process.
 *
 * \retval 0 No errors
 */
extern int
bcma_sys_conf_sdk_debug_shell(void);

/*!
 * \brief Set system configuration option.
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
bcma_sys_conf_sdk_option_set(bcma_sys_conf_opt_t opt,
                             int val, const char *valstr);

#endif /* BCMA_SYS_CONF_SDK_H */
