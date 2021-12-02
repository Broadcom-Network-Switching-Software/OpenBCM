/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * DNXF LINK
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_LINK
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#endif
#include <sal/types.h>

#include <bcm_int/common/debug.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm_int/common/link.h>

#include <bcm_int/dnxf/auto_generated/dnxf_link_dispatch.h>
#include <bcm_int/dnxf/link.h>

#include <soc/drv.h>
#include <soc/linkctrl.h>
#include <soc/dnxc/error.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_linkscan.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

/*
 * Function:
 *     _bcm_dnxf_link_port_link_get
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
_bcm_dnxf_link_port_link_get(
    int unit,
    bcm_port_t port,
    int hw,
    int *up)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_link_status_get, (unit, port, up)));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     _bcm_dnxf_link_trunk_sw_ftrigger
 * Purpose:
 *     Remove specified ports with link down from trunks.
 * Parameters:
 *     unit        - Device unit number
 *     pbmp_active - Bitmap of ports
 *     pbmp_status - Bitmap of port status
 * Returns:
 *     _SHR_E_XXX
 */
STATIC int
_bcm_dnxf_link_trunk_sw_ftrigger(
    int unit,
    bcm_pbmp_t pbmp_active,
    bcm_pbmp_t pbmp_status)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * No action for DNXF device
     */
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static const _bcm_ls_driver_t _bcm_ls_driver_dnxf = {
    NULL,       /* ld_hw_interrupt */
    _bcm_dnxf_link_port_link_get,       /* ld_port_link_get */
    NULL,       /* ld_internal_select */
    NULL,       /* ld_update_asf */
    _bcm_dnxf_link_trunk_sw_ftrigger    /* ld_trunk_sw_failover_trigger */
};

int
bcm_dnxf_linkscan_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}
/*
 * Function:
 *     dnxf_linkscan_source_map_set
 * Purpose:
 *     Initialize link up indication sources to be used in the linkscan
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     _SHR_E_XXX
 */
static int
dnxf_linkscan_source_map_set(
    int unit)
{
    int link_up_sources = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set initial link up indication to include only FEC and SerDes as signal sources.
     * This is necessary because the signals in the MAC and RTP need control cells.
     */
    _SHR_FABRIC_LINK_UP_SOURCE_MAP_INDICATION_MAC_CLEAR(link_up_sources);
    _SHR_FABRIC_LINK_UP_SOURCE_MAP_INDICATION_FEC_SET(link_up_sources);
    _SHR_FABRIC_LINK_UP_SOURCE_MAP_INDICATION_SERDES_SET(link_up_sources);
    _SHR_FABRIC_LINK_UP_SOURCE_MAP_INDICATION_LOCAL_FAULT_CLEAR(link_up_sources);
    _SHR_FABRIC_LINK_UP_SOURCE_MAP_INDICATION_REMOTE_FAULT_CLEAR(link_up_sources);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                    (unit, mbcm_dnxf_fabric_links_source_map_indication_set, (unit, link_up_sources)));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     dnxf_linkscan_init
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
dnxf_linkscan_init(
    int unit)
{
    int rc = _SHR_E_NONE;
    bcm_pbmp_t pbmp_hw, pbmp_sw;
    char pbmp_buf[SOC_PBMP_FMT_LEN];
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        SHR_IF_ERR_EXIT(soc_iproc_m0_init(unit));
    }
#endif

    rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_linkctrl_init);
    SHR_IF_ERR_EXIT(rc);

    SHR_IF_ERR_EXIT(_bcm_linkscan_init(unit, &_bcm_ls_driver_dnxf));

    if (!SOC_WARM_BOOT(unit))
    {
        /*
         * Read Soc Properties for code boot
         */
        BCM_PBMP_CLEAR(pbmp_sw);
        pbmp_sw = dnxf_data_linkscan.general.pbmp_get(unit)->sw;
        BCM_PBMP_CLEAR(pbmp_hw);
        pbmp_hw = dnxf_data_linkscan.general.pbmp_get(unit)->hw;

        /*
         * HW linkscan is the default
         */
        BCM_PBMP_REMOVE(pbmp_sw, pbmp_hw);

        /*
         * Configure link up indication sources
         */
        if (dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_linkup_indication_map_supported))
        {
            SHR_IF_ERR_EXIT(dnxf_linkscan_source_map_set(unit));
        }
        /*
         * Print values
         */
        SOC_PBMP_FMT(pbmp_sw, pbmp_buf);
        LOG_VERBOSE(BSL_LS_BCM_LINK, (BSL_META_U(unit, "linkscan pbmp sw: %s\n"), pbmp_buf));
        SOC_PBMP_FMT(pbmp_hw, pbmp_buf);
        LOG_VERBOSE(BSL_LS_BCM_LINK, (BSL_META_U(unit, "linkscan pbmp hw: %s\n"), pbmp_buf));
        /*
         * Set values
         */
        SHR_IF_ERR_EXIT(bcm_dnxf_linkscan_mode_set_pbm(unit, pbmp_sw, BCM_LINKSCAN_MODE_SW));
        SHR_IF_ERR_EXIT(bcm_dnxf_linkscan_mode_set_pbm(unit, pbmp_hw, BCM_LINKSCAN_MODE_HW));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * Function:
 *     dnxf_linkscan_deinit
 * Purpose:
 *     Deinitialize the linkscan software module.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     _SHR_E_XXX
 * Notes:
 *
 */
int
dnxf_linkscan_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnxf_linkscan_detach(unit));

    if (SOC_FAILURE(soc_linkctrl_deinit(unit)))
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Failed in soc_linkctrl_deinit\n")));
    }
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
