/** \file appl_ref_kbp_init.c
 * 
 *
 * KBP initialization procedures for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
#include <soc/sand/shrextend/shrextend_debug.h>

#if defined(INCLUDE_KBP)

#include <bcm/switch.h>
#include <stdlib.h>
#include "appl_ref_kbp_init.h"
#include <bcm_int/dnx/kbp/kbp_xpt.h>

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
        char buf[80] = { 0 };
        char cmd_str[128] = { 0 };
        int offset = 0;

        SHR_IF_ERR_EXIT(dnx_kbp_pcie_bus_id_mapping_format_get(unit, buf, sizeof(buf)));
        offset += sal_snprintf(cmd_str, sizeof(cmd_str) - 1, "/sbin/insmod kbp_driver.ko ");
        if (buf[0] != 0)
        {
            offset += sal_snprintf(cmd_str + offset, sizeof(cmd_str) - offset - 1, "pcie_bus_mapping=");
            offset += sal_snprintf(cmd_str + offset, sizeof(cmd_str) - 1, "\"%s\"", buf);
        }
        offset += sal_snprintf(cmd_str + offset, sizeof(cmd_str) - offset - 1, " 2> /dev/null");

        /** rmmod should happen externally to the BCM driver prior to trying to load a new kernel module
         * system("/sbin/rmmod kbp_driver.ko 2> /dev/null");
         */
        /** Load the KBP driver module */
        if (!system(cmd_str))
        {
            system(cmd_str);
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
