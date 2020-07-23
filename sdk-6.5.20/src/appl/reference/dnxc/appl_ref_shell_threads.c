/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/shell.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/switch.h>
#include <bcm_int/common/debug.h>

/*************
 * DEFINES   *
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COUNTER

/*************
 * TYPE DEFS *
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief - Disable all threads created after init using shell commands
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_shell_threads_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sh_process_command(unit, "phy prbsstat stop") != CMD_OK)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR - phy prbsstat stop\n");
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
