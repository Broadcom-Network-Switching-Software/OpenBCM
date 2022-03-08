/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        link.c
 * Purpose:     BCM Linkscan module 
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LINK

#include <shared/shr_thread_manager.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/core/sync.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <bcm_int/common/debug.h>
#include <sal/types.h>
#include <soc/linkctrl.h>
#include <soc/link.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>
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
#include <include/bcm_int/dnx/auto_generated/dnx_link_dispatch.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/error.h>
#include <sal/core/boot.h>
#include <soc/dnx/dnx_linkctrl.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#define SW_FLR_LOCK(unit, _lock) do                         \
        {                                                   \
            if (_lock != NULL)                              \
            {                                               \
                sal_mutex_take(_lock, sal_mutex_FOREVER);   \
            }                                               \
        } while (0)

#define SW_FLR_UNLOCK(unit, _lock) do             \
        {                                       \
            if (_lock != NULL)                  \
            {                                   \
                sal_mutex_give(_lock);          \
            }                                   \
        } while (0)

static int dnx_flr_sw_link_port_init(
    int unit,
    int port,
    int enable,
    sal_usecs_t timeout_us);

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
    int rc, sw_flr_link;
    dnx_sw_flr_info_t *sw_flr = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    sw_flr_link = 0;
    sw_flr = (dnx_sw_flr_info_t *) dnx_drv_soc_info_flr_sw_linkscan_info_get(unit);
    if (sw_flr != NULL)
    {
        if (BCM_PBMP_MEMBER(sw_flr->pbmp_sw_flr, port))
        {
            SW_FLR_LOCK(unit, sw_flr->sw_flr_lock);
            sw_flr_link = 1;
            if (BCM_PBMP_MEMBER(sw_flr->pbmp_flr_fail, port))
            {
                /*
                 * Indicate there is link down out of FLR timeout window The link down event must to be triggered to
                 * linkscan thread
                 */
                *up = 0;
                BCM_PBMP_PORT_REMOVE(sw_flr->pbmp_flr_fail, port);
            }
            else if (BCM_PBMP_MEMBER(sw_flr->pbmp_link, port))
            {
                *up = 1;
            }
            else
            {
                *up = 0;
            }
            SW_FLR_UNLOCK(unit, sw_flr->sw_flr_lock);
        }
    }

    if (!sw_flr_link)
    {
        rc = dnx_port_link_get(unit, port, up);
        SHR_IF_ERR_EXIT(rc);
    }

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

    /*
     * Initial linkscan driver.
     */
    soc_linkscan_driver_init(unit);

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

static shr_error_e
dnx_linkscan_port_er_rollback(
    int unit,
    int er_thread_id,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    bcm_port_t *port = (bcm_port_t *) (metadata);
    int *is_add = (int *) (payload);
    SHR_FUNC_INIT_VARS(unit);

    if (*is_add)
    {
        SHR_IF_ERR_EXIT(bcm_common_linkscan_mode_set(unit, *port, BCM_LINKSCAN_MODE_NONE));
        SHR_IF_ERR_EXIT(_bcm_linkscan_port_deinit(unit, *port));
    }
    else
    {
        SHR_IF_ERR_EXIT(_bcm_linkscan_port_init(unit, *port));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_linkscan_port_er_journal_insert(
    int unit,
    bcm_port_t port,
    int is_add)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new(unit,
                                                        (uint8 *) (&port),
                                                        sizeof(bcm_port_t),
                                                        (uint8 *) (&is_add),
                                                        sizeof(int), &dnx_linkscan_port_er_rollback, FALSE));

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
    bcm_port_t port, master_port;
    dnx_algo_port_info_s port_info;
    int enable, loopback;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT)))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(_bcm_linkscan_port_init(unit, port));
    SHR_IF_ERR_EXIT(dnx_linkscan_port_er_journal_insert(unit, port, 1));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(MGMT)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
        if (master_port != port)
        {
            DNX_ERR_RECOVERY_SUPPRESS(unit);
            SHR_IF_ERR_CONT(imb_port_loopback_get(unit, master_port, &loopback));
            DNX_ERR_RECOVERY_UNSUPPRESS(unit);
            if (SHR_FUNC_ERR())
            {
                SHR_EXIT();
            }
            SHR_IF_ERR_EXIT(imb_port_enable_get(unit, port, &enable));
            if ((BCM_E_NONE == _bcm_linkscan_available(unit)) && (BCM_PORT_LOOPBACK_MAC == loopback))
            {
                SHR_IF_ERR_EXIT(_bcm_link_force(unit, port, TRUE, enable));
            }
        }
    }

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

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT)))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_flr_sw_link_port_init(unit, port, 0, 0));

    SHR_IF_ERR_EXIT(bcm_common_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_NONE));
    SHR_IF_ERR_EXIT(_bcm_linkscan_port_deinit(unit, port));
    SHR_IF_ERR_EXIT(dnx_linkscan_port_er_journal_insert(unit, port, 0));

exit:
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

        /*
         * Update SW FLR data while there is linkscan&FLR configure
         */
        SHR_IF_ERR_EXIT(dnx_port_flr_sw_linkscan_update(unit, port_ndx));

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

/*
 * Function:
 *     bcm_dnx_linkscan_flr_enable_set
 * Purpose:
 *     Set FLR mode/timeout for the specified port.
 *
 * Parameters:
 *     unit - Device unit number
 *     port - Port number.
 *     enable - If FLR is enabled.
 *     timeout - FLR timeout type (bcmPortFlrTimeoutxxms)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_dnx_linkscan_flr_enable_set(
    int unit,
    bcm_port_t port,
    int enable,
    bcm_port_flr_timeout_t timeout)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_common_linkscan_flr_enable_set(unit, port, enable, timeout));

    /*
     * Update SW FLR data while there is linkscan&FLR configure
     */
    SHR_IF_ERR_EXIT(dnx_port_flr_sw_linkscan_update(unit, port));
exit:
    SHR_FUNC_EXIT;
}

STATIC int
_bcm_dnx_port_sw_flr_timeout(
    sal_usecs_t cur_usecs,
    sal_usecs_t flr_down_usecs,
    sal_usecs_t timeout_usecs)
{
    int rc = FALSE;
    sal_usecs_t diff_usecs;

    if (cur_usecs < flr_down_usecs)
    {
        rc = TRUE;
    }
    else
    {
        diff_usecs = cur_usecs - flr_down_usecs;
        if (diff_usecs >= timeout_usecs)
        {
            rc = TRUE;
        }
        else
        {
            rc = FALSE;
        }
    }

    return rc;
}

/*
* Function:
*      dnx_flr_sw_link_polling_port
* Purpose:
*      polling port link status for SW linkscan FLR
* Parameters:
*      unit             - (IN)  Device number
*      port             - (IN)  polling port
*
* Returns:
*
*/
static int
dnx_flr_sw_link_polling_port(
    int unit,
    int port,
    dnx_sw_flr_info_t * sw_flr)
{
    int link, cur_link, flr_delay_check;
    sal_usecs_t cur_usecs;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_link_get(unit, port, &link));
    cur_usecs = sal_time_usecs();

    cur_link = BCM_PBMP_MEMBER(sw_flr->pbmp_link, port);
    if (!cur_link)
    {
        /*
         * the old link status is down
         */
        if (link)
        {
            BCM_PBMP_PORT_ADD(sw_flr->pbmp_link, port);
            sw_flr->down2up_counter[port]++;
            BCM_PBMP_PORT_REMOVE(sw_flr->pbmp_flr_up, port);
        }
        else
        {
            sw_flr->keep_down_counter[port]++;
        }
    }
    else
    {
        /*
         * the old link status is up
         */
        flr_delay_check = BCM_PBMP_MEMBER(sw_flr->pbm_flr_delay_check, port);
        if (link)
        {
            /*
             * the new link status is up
             */
            if (flr_delay_check)
            {
                /*
                 * It is successful FLR Remove flr_delay_check state and update FLR counter
                 */
                BCM_PBMP_PORT_REMOVE(sw_flr->pbm_flr_delay_check, port);
                sw_flr->flr_counter[port]++;
                BCM_PBMP_PORT_ADD(sw_flr->pbmp_flr_up, port);
                sw_flr->flr_up_delay[port] = cur_usecs - sw_flr->flr_down_usecs[port];
            }
            else
            {
                /*
                 * Link keep up, Nothing is required
                 */
                sw_flr->keep_up_counter[port]++;
            }
        }
        else
        {
            if (flr_delay_check)
            {
                if (_bcm_dnx_port_sw_flr_timeout(cur_usecs, sw_flr->flr_down_usecs[port], sw_flr->timeout_usecs[port]))
                {
                    /*
                     * FLR timeout, set cur link down and remove flr_delay_check state
                     */
                    BCM_PBMP_PORT_REMOVE(sw_flr->pbmp_link, port);
                    BCM_PBMP_PORT_REMOVE(sw_flr->pbm_flr_delay_check, port);
                    BCM_PBMP_PORT_ADD(sw_flr->pbmp_flr_fail, port);
                    sw_flr->flr_fail_counter[port]++;
                    sw_flr->flr_up_delay[port] = cur_usecs - sw_flr->flr_down_usecs[port];
                }
                else
                {
                    /*
                     * Still in FLR timeout window, do nothing
                     */
                    sw_flr->delay_check_counter[port]++;
                }
            }
            else
            {
                /*
                 * First time detect link down, Just set flr_delay_check state
                 */
                BCM_PBMP_PORT_ADD(sw_flr->pbm_flr_delay_check, port);
                sw_flr->flr_down_usecs[port] = cur_usecs;
                sw_flr->up2down_counter[port]++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* Function:
*      dnx_flr_sw_link_polling
* Purpose:
*      event handler entry
* Parameters:
*      unit                 - (IN)  Device number
*      userdata             - (IN)  User data
*
* Returns:
*
*/
static int
dnx_flr_sw_link_polling(
    int unit,
    void *userdata)
{
    int port, is_lock = 0;
    dnx_sw_flr_info_t *sw_flr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    sw_flr = (dnx_sw_flr_info_t *) dnx_drv_soc_info_flr_sw_linkscan_info_get(unit);
    if (sw_flr != NULL)
    {
        SW_FLR_LOCK(unit, sw_flr->sw_flr_lock);
        is_lock = 1;
        PBMP_ITER(sw_flr->pbmp_sw_flr, port)
        {
            SHR_IF_ERR_EXIT(dnx_flr_sw_link_polling_port(unit, port, sw_flr));
        }
    }

exit:
    if (is_lock)
    {
        SW_FLR_UNLOCK(unit, sw_flr->sw_flr_lock);
    }
    SHR_FUNC_EXIT;
}

static int
dnx_flr_sw_link_port_init(
    int unit,
    int port,
    int enable,
    sal_usecs_t timeout_us)
{
    int link, rv;
    dnx_sw_flr_info_t *sw_flr = NULL;
    SHR_FUNC_INIT_VARS(unit);

    sw_flr = (dnx_sw_flr_info_t *) dnx_drv_soc_info_flr_sw_linkscan_info_get(unit);
    if (sw_flr == NULL)
    {
        SHR_EXIT();
    }

    SW_FLR_LOCK(unit, sw_flr->sw_flr_lock);
    if (enable)
    {
        BCM_PBMP_PORT_ADD(sw_flr->pbmp_sw_flr, port);
        sw_flr->timeout_usecs[port] = timeout_us;

        /*
         * init link
         */
        rv = dnx_port_link_get(unit, port, &link);
        if ((BCM_SUCCESS(rv)) && (link))
        {
            BCM_PBMP_PORT_ADD(sw_flr->pbmp_link, port);
        }
        else
        {
            BCM_PBMP_PORT_REMOVE(sw_flr->pbmp_link, port);
        }
    }
    else
    {
        BCM_PBMP_PORT_REMOVE(sw_flr->pbmp_sw_flr, port);
        sw_flr->timeout_usecs[port] = 0;
        BCM_PBMP_PORT_REMOVE(sw_flr->pbmp_link, port);
    }

    sw_flr->flr_down_usecs[port] = 0;
    sw_flr->flr_up_delay[port] = 0;
    BCM_PBMP_PORT_REMOVE(sw_flr->pbm_flr_delay_check, port);
    BCM_PBMP_PORT_REMOVE(sw_flr->pbmp_flr_up, port);
    BCM_PBMP_PORT_REMOVE(sw_flr->pbmp_flr_fail, port);
    /*
     * clear SW FLR counter
     */
    sw_flr->flr_counter[port] = 0;
    sw_flr->flr_fail_counter[port] = 0;
    sw_flr->delay_check_counter[port] = 0;
    sw_flr->keep_up_counter[port] = 0;
    sw_flr->keep_down_counter[port] = 0;
    sw_flr->down2up_counter[port] = 0;
    sw_flr->up2down_counter[port] = 0;
    SW_FLR_UNLOCK(unit, sw_flr->sw_flr_lock);
exit:
    SHR_FUNC_EXIT;
}

/*
* Function:
*      dnx_port_flr_sw_linkscan_update
* Purpose:
*      Update port sw linkscan flr configure data
*      Only FlexE phy ports are supported in FLR SW mode
* Parameters:
*      unit             - (IN)  Device number
*      port             - (IN)  port
*
* Returns:
*
*/
int
dnx_port_flr_sw_linkscan_update(
    int unit,
    int port)
{
    int rv1, rv2, sw_flr_en = 0;
    int enable, mode;
    bcm_port_flr_timeout_t timeout;
    sal_usecs_t timeout_us = 0;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_drv_soc_port_valid(unit, port))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * Only FlexE phy ports are supported in FLR SW mode
     */
    if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        SHR_EXIT();
    }

    rv1 = bcm_common_linkscan_flr_enable_get(unit, port, &enable, &timeout);
    rv2 = bcm_common_linkscan_mode_get(unit, port, &mode);
    if ((BCM_SUCCESS(rv1)) && (BCM_SUCCESS(rv2)))
    {
        if ((enable) && (mode == BCM_LINKSCAN_MODE_SW))
        {
            sw_flr_en = 1;
            switch (timeout)
            {
                case bcmPortFlrTimeout50ms:
                {
                    timeout_us = 50000;
                    break;
                }
                case bcmPortFlrTimeout75ms:
                {
                    timeout_us = 75000;
                    break;
                }
                case bcmPortFlrTimeout100ms:
                {
                    timeout_us = 100000;
                    break;
                }
                case bcmPortFlrTimeout150ms:
                {
                    timeout_us = 150000;
                    break;
                }
                case bcmPortFlrTimeout250ms:
                {
                    timeout_us = 250000;
                    break;
                }
                case bcmPortFlrTimeout500ms:
                {
                    timeout_us = 500000;
                    break;
                }
                default:
                {
                    timeout_us = 0;
                    break;
                }
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_flr_sw_link_port_init(unit, port, sw_flr_en, timeout_us));
exit:
    SHR_FUNC_EXIT;
}

/*
* Function:
*      dnx_flr_sw_link_dump
* Purpose:
*      Dump SW linkscan FLR config&stat
* Parameters:
*      unit             - (IN)  Device number
*
* Returns:
*
*/
int
dnx_flr_sw_link_dump(
    int unit,
    int clear)
{
    int port;
    dnx_sw_flr_info_t *sw_flr = NULL;
    SHR_FUNC_INIT_VARS(unit);

    sw_flr = (dnx_sw_flr_info_t *) dnx_drv_soc_info_flr_sw_linkscan_info_get(unit);
    if (sw_flr == NULL)
    {
        cli_out("FLR SW linkscan feature was not supported!!\n");
        SHR_EXIT();
    }

    cli_out("%-5s%-8s%-8s%-8s%-14s%-6s %-6s %-6s %-10s %-10s %-6s %-6s\n",
            "Port", "Timeout", "link", "Delay", "Delay_check",
            "FLR", "Fail", "Check", "KeepUp", "KeepDown", "dwn2up", "up2dwn");
    SW_FLR_LOCK(unit, sw_flr->sw_flr_lock);
    PBMP_ITER(sw_flr->pbmp_sw_flr, port)
    {
        cli_out("%-5d", port);
        cli_out("%-8d", sw_flr->timeout_usecs[port]);
        if (BCM_PBMP_MEMBER(sw_flr->pbmp_link, port))
        {
            if (BCM_PBMP_MEMBER(sw_flr->pbmp_flr_up, port))
            {
                cli_out("%-8s", "up(FLR)");
            }
            else
            {
                cli_out("%-8s", "up");
            }
        }
        else
        {
            cli_out("%-8s", "down");
        }
        cli_out("%-8d", sw_flr->flr_up_delay[port]);

        if (BCM_PBMP_MEMBER(sw_flr->pbm_flr_delay_check, port))
        {
            cli_out("Yes(%08x) ", sw_flr->flr_down_usecs[port]);
        }
        else
        {
            cli_out("%-14s", "No");
        }

        cli_out("%-6d %-6d %-6d 0x%-8x 0x%-8x %-6d %-6d\n", sw_flr->flr_counter[port],
                sw_flr->flr_fail_counter[port], sw_flr->delay_check_counter[port],
                sw_flr->keep_up_counter[port], sw_flr->keep_down_counter[port],
                sw_flr->down2up_counter[port], sw_flr->up2down_counter[port]);

        if (clear)
        {
            sw_flr->flr_counter[port] = 0;
            sw_flr->flr_fail_counter[port] = 0;
            sw_flr->delay_check_counter[port] = 0;
            sw_flr->keep_up_counter[port] = 0;
            sw_flr->keep_down_counter[port] = 0;
            sw_flr->down2up_counter[port] = 0;
            sw_flr->up2down_counter[port] = 0;
        }
    }
    SW_FLR_UNLOCK(unit, sw_flr->sw_flr_lock);

exit:
    SHR_FUNC_EXIT;
}

/*
* Function:
*      dnx_flr_sw_linkscan_thread_init
* Purpose:
*      start an flexe link polling thread
* Parameters:
*      unit                    - (IN)  Device number
*
* Returns:
*        _SHR_E_XXX
*/
int
dnx_flr_sw_linkscan_thread_init(
    int unit)
{
    shr_thread_manager_config_t sw_linkscan_flr_config_info;
    shr_thread_manager_handler_t sw_linkscan_flr_handler = NULL;
    dnx_sw_flr_info_t *dnx_sw_flr_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(dnx_sw_flr_info, sizeof(dnx_sw_flr_info_t), "DNX FLR SW linkscan info",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(dnx_sw_flr_info, 0, sizeof(dnx_sw_flr_info_t));

    /** Configure SW FLR linkscan polling Thread */
    shr_thread_manager_config_t_init(&sw_linkscan_flr_config_info);
    sw_linkscan_flr_config_info.name = "SW_FLR";
    sw_linkscan_flr_config_info.bsl_module = BSL_LOG_MODULE;
    sw_linkscan_flr_config_info.interval = 5000;
    sw_linkscan_flr_config_info.callback = dnx_flr_sw_link_polling;
    sw_linkscan_flr_config_info.type = SHR_THREAD_MANAGER_TYPE_EVENT;
    sw_linkscan_flr_config_info.thread_priority = 50;

    /*
     * Create SW linkscan mode FLR polling thread
     */
    SHR_IF_ERR_EXIT(shr_thread_manager_create(unit, &sw_linkscan_flr_config_info, &sw_linkscan_flr_handler));
    dnx_sw_flr_info->handler = sw_linkscan_flr_handler;
    dnx_sw_flr_info->sw_flr_lock = sal_mutex_create("sw_flr_lock");
    if (dnx_sw_flr_info->sw_flr_lock == NULL)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INIT);
    }

exit:
    dnx_drv_soc_info_flr_sw_linkscan_info_set(unit, dnx_sw_flr_info);
    SHR_FUNC_EXIT;
}

/*
* Function:
*      dnx_flr_sw_linkscan_thread_deinit
* Purpose:
*      stop an flexe link polling thread
* Parameters:
*      unit                    - (IN)  Device number
*
* Returns:
*        _SHR_E_XXX
*/
int
dnx_flr_sw_linkscan_thread_deinit(
    int unit)
{
    shr_thread_manager_handler_t sw_linkscan_flr_handler = NULL;
    dnx_sw_flr_info_t *dnx_sw_flr_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    dnx_sw_flr_info = (dnx_sw_flr_info_t *) dnx_drv_soc_info_flr_sw_linkscan_info_get(unit);
    if (dnx_sw_flr_info == NULL)
    {
        SHR_EXIT();
    }

    if (dnx_sw_flr_info->sw_flr_lock != NULL)
    {
        sal_mutex_destroy(dnx_sw_flr_info->sw_flr_lock);
        dnx_sw_flr_info->sw_flr_lock = NULL;
    }

    sw_linkscan_flr_handler = dnx_sw_flr_info->handler;
    /** Destroy periodic event thread for PFC Deadlock recovery mechanism */
    if (sw_linkscan_flr_handler != NULL)
    {
        SHR_IF_ERR_CONT(shr_thread_manager_destroy(&sw_linkscan_flr_handler));
    }

    SHR_FREE(dnx_sw_flr_info);
    dnx_sw_flr_info = NULL;
    dnx_drv_soc_info_flr_sw_linkscan_info_set(unit, dnx_sw_flr_info);

exit:
    SHR_FUNC_EXIT;
}
