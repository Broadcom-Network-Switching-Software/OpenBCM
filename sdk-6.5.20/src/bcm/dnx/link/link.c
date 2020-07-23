/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        link.c
 * Purpose:     BCM Linkscan module 
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LINK

#include <sal/core/sync.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <shared/bsl.h>
#include <bcm_int/common/debug.h>
#include <sal/types.h>
#include <soc/linkctrl.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#else
#include <soc/cmicm.h>
#endif
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm_int/dnx/link/link.h>
#include <bcm_int/common/link.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/error.h>
#include <sal/core/boot.h>
#include <soc/dnx/dnx_linkctrl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

/*
 * Function:
 *     _bcm_dnx_link_port_link_get
 * Purpose:
 *     Return current PHY up/down status.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *     hw   - If TRUE, assume hardware linkscan is active and use it
 *              to reduce PHY reads.
 *            If FALSE, do not use information from hardware linkscan.
 *     up   - (OUT) TRUE for link up, FALSE for link down.
 * Returns:
 *     _SHR_E_NONE
 *     _SHR_E_XXX
 */
STATIC int
_bcm_dnx_link_port_link_get(
    int unit,
    bcm_port_t port,
    int hw,
    int *up)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    rc = dnx_port_link_get(unit, port, up);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

static const _bcm_ls_driver_t _bcm_ls_driver_dnx = {
    NULL,       /* ld_hw_interrupt */
    _bcm_dnx_link_port_link_get,        /* ld_port_link_get */
    NULL,       /* ld_internal_select */
    NULL,       /* ld_update_asf */
    NULL        /* ld_trunk_sw_failover_trigger */
};

/*
 * Function:
 *     dnx_linkscan_init
 * Purpose:
 *     Initialize the linkscan software module.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     _SHR_E_XXX
 * Notes:
 *     If specific HW linkscan initialization is required by device,
 *     driver should call that.
 */
int
dnx_linkscan_init(
    int unit)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_CMICX_SUPPORT
    if (dnx_drv_soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        SHR_IF_ERR_EXIT(soc_iproc_m0_init(unit));
    }
#endif
    /*
     * Initilaize SOC link control module
     */
    SHR_IF_ERR_EXIT(soc_dnx_linkctrl_init(unit));

    rv = _bcm_linkscan_init(unit, &_bcm_ls_driver_dnx);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     dnx_linkscan_deinit
 * Purpose:
 *     De-initialize the linkscan software module.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     _SHR_E_XXX
 * Notes:
 *     Called by init-deinit sequence.
 */
int
dnx_linkscan_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_common_linkscan_detach(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     dnx_linkscan_port_init
 * Purpose:
 *     Add the port to bitmaps in the link_control structure.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     _SHR_E_XXX
 * Notes:
 *     When adding ports dynamically, this API should be called.
 */
int
dnx_linkscan_port_init(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(_bcm_linkscan_port_init(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     dnx_linkscan_port_remove
 * Purpose:
 *     Remove the port from linkscan.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     _SHR_E_XXX
 * Notes:
 *     When removing ports dynamically, this API should be called.
 */
int
dnx_linkscan_port_remove(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcm_common_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_NONE));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_linkscan_mode_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_port_t port_ndx;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.local_port_bmp, port_ndx)
    {
        if (!dnx_drv_is_sfi_port(unit, port))
        {
            /*
             * Statistic ports are not supported
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port_ndx, &port_info));
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Stat port are not supported by linkscan");
            }
            /*
             * FlexE phy ports are not supported in HW mode
             */
            if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info) && mode == BCM_LINKSCAN_MODE_HW)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE phy port are not supported by linkscan HW mode");
            }
            /*
             * ILKN is not supported in HW mode
             */
            if (dnx_drv_is_il_port(unit, port_ndx) && mode == BCM_LINKSCAN_MODE_HW)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "ILKN port are not supported by linkscan HW mode");
            }
        }

        SHR_IF_ERR_EXIT(bcm_common_linkscan_mode_set(unit, port_ndx, mode));

    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
int
bcm_dnx_linkscan_trigger_event_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_linkscan_trigger_event_t trigger_event,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    switch (trigger_event)
    {
            /*
             * If trigger_event is BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT, also reach this case In the enum
             * definition, BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT = BCM_LINKSCAN_TRIGGER_EVENT_FAULT 
             */
        case BCM_LINKSCAN_TRIGGER_EVENT_FAULT:
        case BCM_LINKSCAN_TRIGGER_EVENT_LOCAL_FAULT:
            if (port != -1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Only port=-1 is supported for this Event type!");
            }
            if (enable == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "This Event type can only be enabled!");
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Event type is not supported");
    }
    SHR_IF_ERR_EXIT(bcm_common_linkscan_trigger_event_set(unit, port, flags, trigger_event, enable));
exit:
    SHR_FUNC_EXIT;
}
int
bcm_dnx_linkscan_trigger_event_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_linkscan_trigger_event_t trigger_event,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    switch (trigger_event)
    {
            /*
             * If trigger_event is BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT, also reach this case In the enum
             * definition, BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT = BCM_LINKSCAN_TRIGGER_EVENT_FAULT 
             */
        case BCM_LINKSCAN_TRIGGER_EVENT_FAULT:
        case BCM_LINKSCAN_TRIGGER_EVENT_LOCAL_FAULT:
            if (port != -1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Only port=-1 is supported for this Event type!");
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Event type is not supported");
    }
    SHR_IF_ERR_EXIT(bcm_common_linkscan_trigger_event_get(unit, port, flags, trigger_event, enable));
exit:
    SHR_FUNC_EXIT;
}
