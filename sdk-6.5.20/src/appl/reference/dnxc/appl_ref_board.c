/** \file appl_ref_board.c
 * functions which are board specific
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_OTHER

/*
* INCLUDE FILES:
* {
*/

#include <appl/reference/dnxc/appl_ref_board.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/cm.h>

/*
 * This function will perform a system reset on the given unit or report if a system reset is supported.
 * If SOC_SYSTEM_RESET_FLAG_IS_SUPPORTED is specified in flags, it will return SOC_E_NONE if supported.
 * Otherwise it will perform a system reset if supported, and return SOC_E_NONE on success.
 */
int
dnxc_perform_system_reset(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Handle the case of an invalid or unsupported device */
    if (!SOC_UNIT_VALID(unit) || (!SOC_IS_DNX(unit) && !SOC_IS_DNXF(unit))
        )
    {
        SHR_SET_CURRENT_ERR(SOC_E_UNAVAIL);
        SHR_EXIT();
    }
    /** Handle the case where the function is only asked to perform the system reset */
    else if ((flags & SOC_SYSTEM_RESET_FLAG_IS_SUPPORTED) == 0)
    { /** Implement performing system reset and PCIe reset of the device from the board */

    }
exit:

    SHR_FUNC_EXIT;
}
