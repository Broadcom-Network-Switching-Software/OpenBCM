/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        link.c
 * Purpose:     BCM Linkscan module 
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_LINK

#include <shared/bsl.h>

#include <bcm_int/common/debug.h>
#include <sal/types.h>
#include <soc/linkctrl.h>
#include <soc/cmicm.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm_int/dpp/link.h>
#include <bcm_int/common/link.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dcmn/dcmn_wb.h>

static const _bcm_ls_driver_t  _bcm_ls_driver_dpp = {
    NULL,                                             /* ld_hw_interrupt */
    _bcm_petra_port_link_get_and_hook_placeholder,    /* ld_port_link_get */
    NULL,                                             /* ld_internal_select */
    NULL,                                             /* ld_update_asf */
    NULL                                              /* ld_trunk_sw_failover_trigger */
};


/*
 * Function:
 *     bcm_petra_linkscan_init
 * Purpose:
 *     Initialize the linkscan software module.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If specific HW linkscan initialization is required by device,
 *     driver should call that.
 */


int
_bcm_arad_linkscan_init(int unit)
{
   int  rv;
   BCMDNX_INIT_FUNC_DEFS;
   
   rv = _bcm_linkscan_init(unit, &_bcm_ls_driver_dpp);
   BCMDNX_IF_ERR_EXIT(rv);
   
#ifdef BCM_CMICM_SUPPORT
   if(!SOC_WARM_BOOT(unit)){
       soc_cmicm_intr1_enable(unit, IRQ_CMCx_LINK_STAT_MOD);
   }
#endif /* BCM_CMICM_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN; 

}


int
bcm_petra_linkscan_init(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_arad_linkscan_init(unit));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_linkscan_mode_set(
    int unit, 
    bcm_port_t port, 
    int mode)
{
    uint32 flags;
    _bcm_dpp_gport_info_t   gport_info;
    bcm_port_t port_ndx;
    BCMDNX_INIT_FUNC_DEFS;
  
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        if (!IS_SFI_PORT(unit, port)) {
            /*Statistic ports are not supported*/
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
            if(SOC_PORT_IS_STAT_INTERFACE(flags)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Stat ports are not supported by linkscan"))); 
            }

            /*ILKN is not supported in HW mode*/
            if (IS_IL_PORT(unit, port) && mode == BCM_LINKSCAN_MODE_HW) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ILKN ports are not supported by linkscan HW mode"))); 
            }
        }

        /* disable WB since the following function call calls for a common function which dispatches an API */
        _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit);
         
        BCMDNX_IF_ERR_EXIT(bcm_common_linkscan_mode_set(unit, port, mode));

        /* re enable WB */
        _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit);
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}
int bcm_petra_linkscan_trigger_event_set(
      int unit,
      bcm_port_t port,
      uint32 flags,
      bcm_linkscan_trigger_event_t trigger_event,
      int enable)
{
    BCMDNX_INIT_FUNC_DEFS;

    switch (trigger_event) {
            /*
             * If trigger_event is BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT, also reach this case In the enum
             * definition, BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT = BCM_LINKSCAN_TRIGGER_EVENT_FAULT 
             */
        case BCM_LINKSCAN_TRIGGER_EVENT_FAULT:
        case BCM_LINKSCAN_TRIGGER_EVENT_LOCAL_FAULT:
            if (port != -1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only port=-1 is supported for this Event type!")));
            }
            if (enable == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("This Event type can only be enabled!")));
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Event type is not supported")));
    }
    BCMDNX_IF_ERR_EXIT(bcm_common_linkscan_trigger_event_set(unit, port, flags, trigger_event, enable));
exit:
    BCMDNX_FUNC_RETURN;
}
int bcm_petra_linkscan_trigger_event_get(
      int unit,
      bcm_port_t port,
      uint32 flags,
      bcm_linkscan_trigger_event_t trigger_event,
      int *enable)
{
    BCMDNX_INIT_FUNC_DEFS;

    switch (trigger_event) {
            /*
             * If trigger_event is BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT, also reach this case In the enum
             * definition, BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT = BCM_LINKSCAN_TRIGGER_EVENT_FAULT 
             */
        case BCM_LINKSCAN_TRIGGER_EVENT_FAULT:
        case BCM_LINKSCAN_TRIGGER_EVENT_LOCAL_FAULT:
            if (port != -1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only port=-1 is supported for this Event type!")));
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Event type is not supported")));
    }
    BCMDNX_IF_ERR_EXIT(bcm_common_linkscan_trigger_event_get(unit, port, flags, trigger_event, enable));
exit:
    BCMDNX_FUNC_RETURN;
}


