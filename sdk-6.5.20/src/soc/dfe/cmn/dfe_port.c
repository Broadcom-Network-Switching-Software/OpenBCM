

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DFE PORT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC

#ifdef BCM_DFE_SUPPORT
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_port.h>

#ifdef BCM_88750_SUPPORT
#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/fe1600/fe1600_port.h>
#endif



soc_error_t 
soc_dfe_port_loopback_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback)
{
    int rc;
    soc_dcmn_loopback_mode_t lb_start; 
    uint32 cl72_start;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit) || !SOC_IS_DFE(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    DFE_LINK_INPUT_CHECK_SOCDNX(unit, port);

    if (loopback < soc_dcmn_loopback_mode_none || loopback > soc_dcmn_loopback_mode_phy_gloop) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid loopback")));
    }

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    SOCDNX_IF_ERR_EXIT(soc_dfe_port_loopback_get(unit, port, &lb_start));
    if (loopback != soc_dcmn_loopback_mode_none) {
        if (lb_start == soc_dcmn_loopback_mode_none) {
            SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_control_get,(unit, port, -1, -1, 0, SOC_PHY_CONTROL_CL72, &cl72_start)));
            rc = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, PORT_CL72_CONF, port, &cl72_start);
            SOCDNX_IF_ERR_EXIT(rc);
            if (cl72_start == 1) {
                SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_control_set,(unit, port, -1, -1, 0, SOC_PHY_CONTROL_CL72, 0)));
            }
        }
    }
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_loopback_set,(unit, port, loopback));
    SOCDNX_IF_ERR_EXIT(rc);

    if (loopback == soc_dcmn_loopback_mode_none) {
        if (lb_start != soc_dcmn_loopback_mode_none) {
            rc = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, PORT_CL72_CONF, port, &cl72_start);
            SOCDNX_IF_ERR_EXIT(rc);

            if (cl72_start == 1) {
                SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_control_set,(unit, port, -1, -1, 0, SOC_PHY_CONTROL_CL72, 1)));
            }
        }
    }

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_burst_control_set,(unit, port, loopback));
    SOCDNX_IF_ERR_EXIT(rc);

    
exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dfe_port_loopback_get(int unit, soc_port_t port, soc_dcmn_loopback_mode_t* loopback)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit) || !SOC_IS_DFE(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    DFE_LINK_INPUT_CHECK_SOCDNX(unit, port);
    SOCDNX_NULL_CHECK(loopback);

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_loopback_get,(unit, port, loopback));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;

}

#endif 

#undef _ERR_MSG_MODULE_NAME

