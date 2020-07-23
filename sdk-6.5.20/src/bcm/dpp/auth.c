/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * 802.1X
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_AUTH
#include <shared/bsl.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/auth.h>
#include <bcm/debug.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/switch.h>

#include <soc/dpp/mbcm.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <bcm_int/petra_dispatch.h>

#include <soc/dpp/port_sw_db.h>


/*
 * Function:
 *	bcm_petra_auth_mode_set
 * Purpose:
 *	Set the 802.1X operating mode
 * Parameters:
 *	unit - Device number
 *	port - Port number, -1 to set all non-stack-ports
 *	mode - One of BCM_AUTH_MODE_XXX and other flags (see below)
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 *     BCM_E_PORT     - Invalid port
 * Notes:
 *	While in the uncontrolled state, any packets may flow in or out
 *	of the port and normal L2 learning takes place.
 */

int
bcm_petra_auth_mode_set(int unit, int port, uint32 mode)
{
    int            rv = BCM_E_NONE;
    uint32         soc_sand_rc;
    int            local_port, core;
    SOC_PPC_PORT       pp_port;              
    SOC_PPC_PORT_INFO  port_info;
    _bcm_dpp_gport_info_t gport_info;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);


    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    /* Configuration per port */
    PBMP_ITER(gport_info.pbmp_local_ports, local_port) {   
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &pp_port, &core)));
    
        soc_sand_rc = soc_ppd_port_info_get(unit, core, pp_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

        switch (mode) {
            case BCM_AUTH_MODE_AUTH:
                port_info.not_authorized_802_1x = 0;
                break;
            case BCM_AUTH_MODE_UNAUTH:
                port_info.not_authorized_802_1x = 1;
                break;
            default:
                return BCM_E_PARAM;
        }

        port_info.flags = SOC_PPC_PORT_IHP_PINFO_LLR_TBL;
        soc_sand_rc = soc_ppd_port_info_set(unit, core, pp_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *	bcm_petra_auth_mode_get
 * Purpose:
 *	Get the 802.1X operating mode
 * Parameters:
 *	unit - Device number
 *	port - Port number
 *	mode - (OUT) One of BCM_AUTH_MODE_XXX and other flags
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 *     BCM_E_PORT     - Invalid port
 * Notes:
 */

int
bcm_petra_auth_mode_get(int unit, int port, uint32 *modep)
{
    int                     rv = BCM_E_NONE, core;
    uint32                  soc_sand_rc;
    SOC_PPC_PORT            pp_port;              
    SOC_PPC_PORT_INFO       port_info;
    _bcm_dpp_gport_info_t   gport_info;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* Retrive local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &pp_port, &core)));
    
    soc_sand_rc = soc_ppd_port_info_get(unit, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    switch (port_info.not_authorized_802_1x) {
        case 0:
            *modep = BCM_AUTH_MODE_AUTH;
            break;
        case 1:
        default:
            *modep = BCM_AUTH_MODE_UNAUTH;
            break;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

