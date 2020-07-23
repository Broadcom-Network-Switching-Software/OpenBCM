/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/phy/drv.h>

/*
 * Function:
 *      soc_port_config_phy_oam_set
 * Purpose:
 *      Update PHY's phy_oam config
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      conf - Config
 * Returns:
 *      SOC_E_XXX
 */
int
soc_port_config_phy_oam_set(int unit, soc_port_t port, soc_port_config_phy_oam_t *conf)
{
    int         rv;
    phy_ctrl_t *pc;

    rv = SOC_E_UNAVAIL;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL != pc) {
        INT_MCU_LOCK(unit);
        rv = PHY_OAM_CONFIG_SET(pc->pd, unit, port, conf);
        INT_MCU_UNLOCK(unit);
    }

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "soc_port_config_phy_oam_set failed %d\n"), rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_port_config_phy_oam_get
 * Purpose:
 *      Retrive PHY's phy_oam config
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      conf - Config
 * Returns:
 *      SOC_E_XXX
 */
int
soc_port_config_phy_oam_get(int unit, soc_port_t port, soc_port_config_phy_oam_t *conf)
{
    int         rv;
    phy_ctrl_t *pc;

    rv = SOC_E_UNAVAIL;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL != pc) {
        INT_MCU_LOCK(unit);
        rv = PHY_OAM_CONFIG_GET(pc->pd, unit, port, conf);
        INT_MCU_UNLOCK(unit);
    }

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "soc_port_config_phy_oam_get failed %d\n"), rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_port_control_phy_oam_set
 * Purpose:
 *      Update PHY's phy_oam parameter
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      type - operation
 *      value - parameter
 * Returns:
 *      SOC_E_XXX
 */
int
soc_port_control_phy_oam_set(int unit, soc_port_t port, soc_port_control_phy_oam_t type, uint64 value)
{
    int         rv;
    phy_ctrl_t *pc;

    rv = SOC_E_UNAVAIL;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL != pc) {
        INT_MCU_LOCK(unit);
        rv = PHY_OAM_CONTROL_SET(pc->pd, unit, port, type, value);
        INT_MCU_UNLOCK(unit);
    }

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "soc_port_control_phy_oam_set failed %d\n"), rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_port_control_phy_oam_get
 * Purpose:
 *      Retrive PHY's phy_oam parameter
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      type - operation
 *      value - *parameter
 * Returns:
 *      SOC_E_XXX
 */
int
soc_port_control_phy_oam_get(int unit, soc_port_t port, soc_port_control_phy_oam_t type, uint64 *value)
{
    int         rv;
    phy_ctrl_t *pc;

    rv = SOC_E_UNAVAIL;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL != pc) {
        INT_MCU_LOCK(unit);
        rv = PHY_OAM_CONTROL_GET(pc->pd, unit, port, type, value);
        INT_MCU_UNLOCK(unit);
    }

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "soc_port_control_phy_oam_get failed %d\n"), rv));
    }
    return rv;
}
