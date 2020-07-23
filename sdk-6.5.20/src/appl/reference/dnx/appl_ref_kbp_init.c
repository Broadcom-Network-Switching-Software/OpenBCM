/** \file appl_ref_kbp_init.c
 * 
 *
 * KBP initialization procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

/*
 * {
 */

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>

#if defined(INCLUDE_KBP)

#include <bcm/switch.h>
#include <stdlib.h>
#include "appl_ref_kbp_init.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
extern uint8 dnx_kbp_device_enabled(
    int unit);
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * Global and Static
 */
/*
 * }
 */

/**
 * \brief
 *   Initialize KBP
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
appl_dnx_kbp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_kbp_device_enabled(unit))
    {
#ifndef ADAPTER_SERVER_MODE
        /** Load the KBP driver module */
        if (!system("/sbin/insmod kbp_driver.ko 2> /dev/null"))
        {
            system("/sbin/insmod kbp_driver.ko 2> /dev/null");
        }
#endif

        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamInit, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   De-initialize KBP
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
appl_dnx_kbp_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamInit, 0));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
#endif /* defined(INCLUDE_KBP) */
#undef BSL_LOG_MODULE
