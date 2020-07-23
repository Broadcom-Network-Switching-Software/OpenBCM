/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * File:        phy8729.c
 * Purpose:     SDK PHY driver for BCM8729 (MACSec)
 *
 * Supported BCM546X Family of PHY devices:
 *
 *      Device  Ports   Media                           MAC Interface
 *      8729    4       2 10G SFP+, 2 1G SFP            XFI/SGMII
 *
 * Workarounds:
 *
 * References:
 *     
 * Notes:
 */ 
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_8729)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */

#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#ifdef INCLUDE_MACSEC

#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyfege.h"
#include "phyxehg.h"
#include "phy8729.h"
#include <soc/macsecphy.h>
#include <bmacsec.h>
#include <bmacsec_io.h>

#define ADVERT_ALL_COPPER \
        (SOC_PA_PAUSE | SOC_PA_SPEED_10MB | \
         SOC_PA_SPEED_100MB | SOC_PA_SPEED_1000MB)

#define ADVERT_ALL_FIBER \
        (SOC_PA_PAUSE | SOC_PA_SPEED_1000MB) 

#define ADVERT_ALL_10GFIBER \
        (SOC_PA_PAUSE | SOC_PA_SPEED_10GB) 


#define PHY_ID1_8729     0x603c

static bmacsec_error_t error_tbl[] = {
                                SOC_E_NONE,
                                SOC_E_INTERNAL,
                                SOC_E_MEMORY,
                                SOC_E_PARAM,
                                SOC_E_EMPTY,
                                SOC_E_FULL,
                                SOC_E_NOT_FOUND,
                                SOC_E_EXISTS,
                                SOC_E_TIMEOUT,
                                SOC_E_FAIL,
                                SOC_E_DISABLED,
                                SOC_E_BADID,
                                SOC_E_RESOURCE,
                                SOC_E_CONFIG,
                                SOC_E_UNAVAIL,
                                SOC_E_INIT,
                                SOC_E_PORT,
                            };
/*
 * Convert BMACSEC return codes to SOC return code.
 */
#define SOC_ERROR(e)    error_tbl[(-e)]


STATIC int phy_8729_init(int unit, soc_port_t port);
STATIC int phy_8729_link_get(int unit, soc_port_t port, int *link);
STATIC int phy_8729_enable_set(int unit, soc_port_t port, int enable);
STATIC int phy_8729_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_8729_duplex_get(int unit, soc_port_t port, int *duplex);
STATIC int phy_8729_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_8729_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_8729_master_set(int unit, soc_port_t port, int master);
STATIC int phy_8729_master_get(int unit, soc_port_t port, int *master);
STATIC int phy_8729_an_set(int unit, soc_port_t port, int autoneg);
STATIC int phy_8729_an_get(int unit, soc_port_t port,
                                int *autoneg, int *autoneg_done);
STATIC int phy_8729_lb_set(int unit, soc_port_t port, int enable);
STATIC int phy_8729_lb_get(int unit, soc_port_t port, int *enable);
STATIC int phy_8729_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
STATIC int phy_8729_mdix_get(int unit, soc_port_t port, 
                             soc_port_mdix_t *mode);
STATIC int phy_8729_mdix_status_get(int unit, soc_port_t port, 
                                    soc_port_mdix_status_t *status);
STATIC int phy_8729_medium_config_set(int unit, soc_port_t port, 
                                      soc_port_medium_t  medium,
                                      soc_phy_config_t  *cfg);
STATIC int phy_8729_medium_config_get(int unit, soc_port_t port, 
                                      soc_port_medium_t medium,
                                      soc_phy_config_t *cfg);
STATIC int phy_8729_ability_advert_set(int unit, soc_port_t port, 
                                       soc_port_ability_t *ability);
STATIC int phy_8729_ability_advert_get(int unit, soc_port_t port,
                                       soc_port_ability_t *ability);
STATIC int phy_8729_ability_remote_get(int unit, soc_port_t port,
                                       soc_port_ability_t *ability);
STATIC int phy_8729_ability_local_get(int unit, soc_port_t port, 
                                soc_port_ability_t *ability);
STATIC int phy_8729_firmware_set(int unit, int port, int offset, 
                                 uint8 *data, int len);
STATIC int phy_8729_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value);
STATIC int phy_8729_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value);
STATIC int phy_8729_linkup(int unit, soc_port_t port);




/*
 * Function:
 *      _phy_8729_medium_config_update
 * Purpose:
 *      Update the PHY with config parameters
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      cfg - Config structure.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_8729_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{

    SOC_IF_ERROR_RETURN
        (phy_8729_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_8729_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_8729_master_set(unit, port, cfg->master));
    SOC_IF_ERROR_RETURN
        (phy_8729_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_8729_an_set(unit, port, cfg->autoneg_enable));
/*
    SOC_IF_ERROR_RETURN
        (phy_8729_mdix_set(unit, port, cfg->mdix));
*/

    return SOC_E_NONE;
}

/* Function:
 *    phy_8729_init
 * Purpose:    
 *    Initialize 8729 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8729_init(int unit, soc_port_t port)
{
    phy_ctrl_t     *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_dev_addr_t macsec_dev_addr;
    int rv = SOC_E_NONE;
    bmacsec_phy8729_intf_t line_mode;
    int fiber_preferred, fiber_enable = FALSE, copper_enable = FALSE;
    int macsec_enable = 0, mmi_mdio_addr;
    int     xaui_interface = 0;

    pc     = EXT_PHY_SW_STATE(unit, port);

    /* By default MACSEC is disable */
    /* Check if enable property is set */
    macsec_enable = soc_property_port_get(unit, port, spn_MACSEC_ENABLE, 0);

    pc->macsec_enable = macsec_enable;

    pc->macsec_dev_port = soc_property_port_get(unit, port, 
                                       spn_MACSEC_PORT_INDEX, -1);

    phy_dev_addr    = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 

    /* Base PHY ID is used to access MACSEC core */
    mmi_mdio_addr = soc_property_port_get(unit, port, 
                                        spn_MACSEC_DEV_ADDR, -1);
    if (mmi_mdio_addr < 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8729_init: "
                             "MACSEC_DEV_ADDR property "
                             "not configured for u=%d p=%d\n"), unit, port));
        return SOC_E_CONFIG;
    }
    macsec_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, mmi_mdio_addr, 1);
    pc->macsec_dev_addr = macsec_dev_addr;

    rv = bmacsec_phy8729_phy_mac_addr_set(phy_dev_addr, 
                                          macsec_dev_addr, 
                                          pc->macsec_dev_port,
                                          bmacsec_io_mmi1_cl45);
    if(rv != BMACSEC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8729_init: bmacsec_phy_mac_addr_set "
                             "returned error for u=%d p=%d\n"), unit, port));
        return SOC_E_FAIL;
    }


    /* 
     * Deafult :
     * Ports 0 and 1, line side will be in 10G SFI mode 
     * Ports 2 and 3, line side will be in SGMII mode
     */
    if ((pc->macsec_dev_port == 2) || (pc->macsec_dev_port == 3)) {
        fiber_preferred =
            soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 0);
    } else {
        fiber_preferred =
            soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 1);
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8729_init: "
                         "u=%d p=%d type=8729%s fiber_pref=%d\n"),
              unit, port, fiber_preferred ? "S" : "", fiber_preferred));

    if ((pc->macsec_dev_port == 2) || (pc->macsec_dev_port == 3)) {
        if (fiber_preferred == 0) {
            line_mode = BMACSEC_PHY8729_INTF_SGMII;
            copper_enable = TRUE;
            fiber_enable = FALSE;
        } else {
            line_mode = BMACSEC_PHY8729_INTF_1000X;
            copper_enable = FALSE;
            fiber_enable = TRUE;
        }
    } else {
        if (fiber_preferred == 0) {
            line_mode = BMACSEC_PHY8729_INTF_SGMII;
            copper_enable = TRUE;
            fiber_enable = FALSE;
        } else {
            if ((PHY_OUI(pc->phy_id0, pc->phy_id1) == PHY_BCM8729_OUI) && 
                (PHY_MODEL(pc->phy_id0, pc->phy_id1) == PHY_BCM8729_MODEL)) { 
                xaui_interface = 1;
            }
            line_mode = BMACSEC_PHY8729_INTF_SFI;
            copper_enable = FALSE;
            fiber_enable = TRUE;
        }
    }

    /* Init PHYS and MACs to defaults */
    rv = bmacsec_phy8729_init(phy_dev_addr, 1, line_mode, pc->macsec_enable);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    pc->copper.enable = copper_enable;
    pc->copper.preferred = !fiber_preferred;
    pc->copper.autoneg_enable = TRUE;
    pc->copper.autoneg_advert = ADVERT_ALL_COPPER;
    pc->copper.force_speed = 1000;
    pc->copper.force_duplex = TRUE;
    pc->copper.master = SOC_PORT_MS_AUTO;
    pc->copper.mdix = SOC_PORT_MDIX_AUTO;

    pc->fiber.enable = fiber_enable;
    pc->fiber.preferred = fiber_preferred;
    pc->fiber.autoneg_enable = FALSE;
    pc->fiber.autoneg_advert = ADVERT_ALL_FIBER;
    pc->fiber.force_speed = 10000;
    pc->fiber.force_duplex = TRUE;
    pc->fiber.master = SOC_PORT_MS_NONE;
    pc->fiber.mdix = SOC_PORT_MDIX_NORMAL;

    if ((pc->macsec_dev_port == 2) || (pc->macsec_dev_port == 3)) {
        pc->interface = SOC_PORT_IF_SGMII;
        pc->fiber.force_speed = 1000;
    } else {
        if (xaui_interface) {
            pc->interface = SOC_PORT_IF_XAUI;
        } else {
            pc->interface = (line_mode == BMACSEC_PHY8729_INTF_SGMII) ? 
                                    SOC_PORT_IF_SGMII : SOC_PORT_IF_XFI;
        }
    }

    /* Initially configure for the preferred medium. */
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
    if (pc->fiber.preferred) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
    }
    PHY_FLAGS_SET(unit, port, PHY_FLAGS_C45);

    /* Get Requested LED selectors (defaults are hardware defaults) */
    pc->ledmode[0] = soc_property_port_get(unit, port, spn_PHY_LED1_MODE, 0);
    pc->ledmode[1] = soc_property_port_get(unit, port, spn_PHY_LED2_MODE, 1);
    pc->ledmode[2] = soc_property_port_get(unit, port, spn_PHY_LED3_MODE, 3);
    pc->ledmode[3] = soc_property_port_get(unit, port, spn_PHY_LED4_MODE, 6);
    pc->ledctrl = soc_property_port_get(unit, port, spn_PHY_LED_CTRL, 0x8);
    pc->ledselect = soc_property_port_get(unit, port, spn_PHY_LED_SELECT, 0);

    /*
     * Some reasonable defaults
     */
    pc->macsec_switch_fixed = 0;
    pc->macsec_switch_fixed_speed = 10000;
    pc->macsec_switch_fixed_duplex = 1;
    pc->macsec_switch_fixed_pause = 1;
    pc->macsec_pause_rx_fwd = 0;
    pc->macsec_pause_tx_fwd = 0;
    pc->macsec_line_ipg = 0xc;
    pc->macsec_switch_ipg = 0xc;

    if (!PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_8729_medium_config_update(unit, port,
                                        PHY_COPPER_MODE(unit, port) ?
                                        &pc->copper :
                                        &pc->fiber));
    }

    rv = soc_macsecphy_init(unit, port, pc, 
                            BMACSEC_CORE_BCM8729, bmacsec_io_mmi1_cl45);
    if (!SOC_SUCCESS(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "soc_macsecphy_init: MACSEC init for"
                              " u=%d p=%d FAILED "), unit, port));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "soc_macsecphy_init: MACSEC init for"
                             " u=%d p=%d SUCCESS "), unit, port));
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *    phy_8729_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8729_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t    *pc;
    int rv = SOC_E_NONE;
    bmacsec_dev_addr_t phy_dev_addr;
    int bmacsec_link;

    pc    = EXT_PHY_SW_STATE(unit, port);
    *link = FALSE;      /* Default return */

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (!pc->fiber.enable && !pc->copper.enable) {
        return SOC_E_NONE;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_link_get(phy_dev_addr, &bmacsec_link);
    if (!BMACSEC_E_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }
    if (bmacsec_link == 1) {
        *link = TRUE;
    } else {
        *link = FALSE;
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_8729_link_get: u=%d p=%d mode=%s%s link=%d\n"),
                 unit, port,
                 PHY_COPPER_MODE(unit, port) ? "C" : "F",
                 PHY_FIBER_100FX_MODE(unit, port)? "(100FX)" : "",
                 *link));

    return SOC_E_NONE;
}



/*
 * Function:
 *      phy_8729_enable_set
 * Purpose:
 *      Enable or disable the physical interface.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8729_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;
    int rv = SOC_E_NONE;
    bmacsec_dev_addr_t phy_dev_addr;

    pc     = EXT_PHY_SW_STATE(unit, port);

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_enable_set(phy_dev_addr, enable);
    if(rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8729_enable_set: "
                         "Power %s fiber medium\n"), (enable) ? "up" : "down"));

    return SOC_E_NONE;
}





/*
 * Function:
 *      phy_8729_duplex_set
 * Purpose:
 *      Set the current duplex mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - Half duplex requested, and not supported.
 * Notes:
 */
STATIC int
phy_8729_duplex_set(int unit, soc_port_t port, int duplex)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t           phy_dev_addr;
    bmacsec_phy8729_duplex_t    bmacsec_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    if (duplex) {
        bmacsec_duplex = BMACSEC_PHY8729_FULL_DUPLEX;
    } else {
        bmacsec_duplex = BMACSEC_PHY8729_HALF_DUPLEX;
    }
    rv = bmacsec_phy8729_duplex_set(phy_dev_addr, bmacsec_duplex);

    if(BMACSEC_E_SUCCESS(rv)) {
        pc->copper.force_duplex = duplex;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8729_duplex_set: u=%d p=%d d=%d rv=%d\n"),
              unit, port, duplex, rv));
    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_8729_duplex_get
 * Purpose:
 *      Get the current operating duplex mode. If autoneg is enabled,
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The duplex is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_8729_duplex_get(int unit, soc_port_t port, int *duplex)
{

    int rv = SOC_E_NONE;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy8729_duplex_t bmacsec_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_duplex_get(phy_dev_addr, &bmacsec_duplex);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }
    if (bmacsec_duplex == BMACSEC_PHY8729_FULL_DUPLEX) {
        *duplex = TRUE;
    } else {
        *duplex = FALSE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8729_duplex_get: u=%d p=%d d=%d rv=%d\n"),
              unit, port, *duplex, rv));
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8729_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_8729_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc;
    phy_ctrl_t  *int_pc;
    int            rv; 
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_FIBER_MODE(unit, port)) {
        if (speed < 1000) {
            return SOC_E_UNAVAIL;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy8729_speed_set(phy_dev_addr, speed);

    if(BMACSEC_E_SUCCESS(rv)) {
        if (NULL != int_pc) {
            PHY_AUTO_NEGOTIATE_SET (int_pc->pd, unit, port, 0);
            PHY_SPEED_SET(int_pc->pd, unit, port, speed);
        }
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.force_speed = speed;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                pc->fiber.force_speed = speed;
            }
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8729_speed_set: u=%d p=%d s=%d fiber=%d rv=%d\n"),
              unit, port, speed, PHY_FIBER_MODE(unit, port), rv));

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_8729_speed_get
 * Purpose:
 *      Get the current operating speed for a 8729 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - (OUT) Speed of the phy
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The speed is retrieved for the ACTIVE medium.
 */

STATIC int
phy_8729_speed_get(int unit, soc_port_t port, int *speed)
{
    int rv = SOC_E_NONE;
    bmacsec_dev_addr_t phy_dev_addr;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy8729_speed_get(phy_dev_addr, speed);

    if(!BMACSEC_E_SUCCESS(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8729_speed_get: u=%d p=%d invalid speed\n"),
                  unit, port));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8729_speed_get: u=%d p=%d speed=%d"),
                  unit, port, *speed));
    }

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_8729_master_set
 * Purpose:
 *      Set the current master mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The master mode is set only for the ACTIVE medium.
 */
STATIC int
phy_8729_master_set(int unit, soc_port_t port, int master)
{
    int                  rv;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_master_set(phy_dev_addr, master);
    if(BMACSEC_E_SUCCESS(rv)) {
        pc->copper.master = master;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8729_master_set: u=%d p=%d master=%d fiber=%d rv=%d\n"),
              unit, port, master, PHY_FIBER_MODE(unit, port), rv));

    return SOC_ERROR(rv);
}


/*
 * Function:
 *      phy_8729_master_get
 * Purpose:
 *      Get the current master mode for a 8729 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The master mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_8729_master_get(int unit, soc_port_t port, int *master)
{
    int        rv;
    phy_ctrl_t *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_FIBER_MODE(unit, port)) {
        *master = SOC_PORT_MS_NONE;
        return SOC_E_NONE;
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_master_get(phy_dev_addr, master);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_8729_an_set
 * Purpose:
 *      Enable or disable auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is set only for the ACTIVE medium.
 */

STATIC int
phy_8729_an_set(int unit, soc_port_t port, int autoneg)
{
    int                 rv;
    phy_ctrl_t   *pc;
    int autoneg_advert = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        autoneg_advert = 0;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            autoneg_advert = pc->fiber.autoneg_advert;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_an_set(phy_dev_addr, autoneg, autoneg_advert);
    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.autoneg_enable = autoneg ? TRUE : FALSE;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                pc->fiber.autoneg_enable = autoneg ? TRUE : FALSE;
            }
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8729_an_set: u=%d p=%d autoneg=%d rv=%d\n"),
              unit, port, autoneg, rv));
    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_8729_an_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - (OUT) if true, auto-negotiation is enabled.
 *      autoneg_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if autoneg == FALSE.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_8729_an_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done)
{
    int           rv;
    phy_ctrl_t   *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    rv            = SOC_E_NONE;
    *autoneg      = FALSE;
    *autoneg_done = FALSE;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_an_get(phy_dev_addr, autoneg, autoneg_done);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_8729_lb_set
 * Purpose:
 *      Set the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_8729_lb_set(int unit, soc_port_t port, int enable)
{
    int           rv;
#if 0
    int           link = 0;
    soc_timeout_t  to;
#endif
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_loopback_set(phy_dev_addr, enable);

    if (rv == BMACSEC_E_NONE) {
        /* wait for link up when loopback is enabled */
        
        if (enable) {
            sal_usleep(2000000);

            SOC_IF_ERROR_RETURN
                (phy_8729_linkup(unit, port));
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8729_lb_set: u=%d p=%d en=%d rv=%d\n"), 
              unit, port, enable, rv));

    return SOC_ERROR(rv); 
}

/*
 * Function:
 *      phy_8729_lb_get
 * Purpose:
 *      Get the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_8729_lb_get(int unit, soc_port_t port, int *enable)
{
    int                  rv;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_loopback_get(phy_dev_addr, enable);

    return SOC_ERROR(rv); 
}


/*
 * Function:
 *      phy_8729_mdix_set
 * Description:
 *      Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_8729_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{

    int         rv;
    phy_ctrl_t  *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy8729_port_mdix_t bmacsec_mode;

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_FIBER_MODE(unit, port)) {
        if (mode != SOC_PORT_MDIX_NORMAL) {
            return SOC_E_UNAVAIL;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    switch(mode) {
    case SOC_PORT_MDIX_AUTO:
        bmacsec_mode = BMACSEC_PHY8729_PORT_MDIX_AUTO;
        break;
    case SOC_PORT_MDIX_FORCE_AUTO:
        bmacsec_mode = BMACSEC_PHY8729_PORT_MDIX_FORCE_AUTO;
        break;
    case SOC_PORT_MDIX_NORMAL:
        bmacsec_mode = BMACSEC_PHY8729_PORT_MDIX_NORMAL;
        break;
    case SOC_PORT_MDIX_XOVER:
        bmacsec_mode = BMACSEC_PHY8729_PORT_MDIX_XOVER;
        break;
    default :
        return SOC_E_CONFIG;
    }

    rv = bmacsec_phy8729_mdix_set(phy_dev_addr, bmacsec_mode);

    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.mdix = mode;
        }
    }
    return SOC_ERROR(rv);
}        

/*
 * Function:
 *      phy_8729_mdix_get
 * Description:
 *      Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - (Out) One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_8729_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    if (mode == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(phy_8729_speed_get(unit, port, &speed));
        if (speed == 1000) {
           *mode = SOC_PORT_MDIX_AUTO;
        } else {
            *mode = pc->copper.mdix;
        }
    } else {
        *mode = SOC_PORT_MDIX_NORMAL;
    }

    return SOC_E_NONE;
}    

/*
 * Function:
 *      phy_8729_mdix_status_get
 * Description:
 *      Get the current MDIX status on a port/PHY
 * Parameters:
 *      unit    - Device number
 *      port    - Port number
 *      status  - (OUT) One of:
 *              SOC_PORT_MDIX_STATUS_NORMAL
 *                      Straight connection
 *              SOC_PORT_MDIX_STATUS_XOVER
 *                      Crossover has been performed
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_8729_mdix_status_get(int unit, soc_port_t port, 
                         soc_port_mdix_status_t *status)
{
    int         rv;
    phy_ctrl_t    *pc;

    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy8729_port_mdix_status_t mdix_status;

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_mdix_status_get(phy_dev_addr, &mdix_status);
    if(BMACSEC_E_SUCCESS(rv)) {
        if(mdix_status == BMACSEC_PHY8729_PORT_MDIX_STATUS_NORMAL) {
            *status = SOC_PORT_MDIX_STATUS_NORMAL;
        }
        if(mdix_status == BMACSEC_PHY8729_PORT_MDIX_STATUS_XOVER) {
            *status = SOC_PORT_MDIX_STATUS_XOVER;
        }
        return SOC_E_NONE;
    }

    return SOC_ERROR(rv);
}


/*
 * Function:
 *      phy_8729_medium_config_set
 * Purpose:
 *      Set the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8729_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t    *pc;
#if 0
    soc_phy_config_t *active_medium;  /* Currently active medium */
    int               medium_update = FALSE;
#endif
    soc_phy_config_t *change_medium;  /* Requested medium */
    soc_phy_config_t *other_medium;   /* The other medium */

    soc_port_mode_t   advert_mask;

    if (NULL == cfg) {
        return SOC_E_PARAM;
    }

    pc            = EXT_PHY_SW_STATE(unit, port);

   switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (!pc->automedium && !PHY_COPPER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->copper;
        other_medium   = &pc->fiber;
        advert_mask    = ADVERT_ALL_COPPER;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->fiber;
        other_medium   = &pc->copper;
        advert_mask    = ADVERT_ALL_FIBER;
        break;
    default:
        return SOC_E_PARAM;
    }

    /*
     * Changes take effect immediately if the target medium is active or
     * the preferred medium changes.
     */
#if 0
    if (change_medium->enable != cfg->enable) {
        medium_update = TRUE;
    }
#endif
    if (change_medium->preferred != cfg->preferred) {
        /* Make sure that only one medium is preferred */
        other_medium->preferred = !cfg->preferred;
#if 0
        medium_update = TRUE;
#endif
    }

    sal_memcpy(change_medium, cfg, sizeof(*change_medium));
    change_medium->autoneg_advert &= advert_mask;

#if 0
    if (medium_update) {
        /* The new configuration may cause medium change. Check
         * and update medium.
         */
        SOC_IF_ERROR_RETURN
            (_phy_8729_medium_change(unit, port, TRUE, medium));
    } else {
        active_medium = (PHY_COPPER_MODE(unit, port)) ?  
                            &pc->copper : &pc->fiber;
        if (active_medium == change_medium) {
            /* If the medium to update is active, update the configuration */
            SOC_IF_ERROR_RETURN
                (_phy_8729_medium_config_update(unit, port, change_medium));
        }
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8729_medium_config_get
 * Purpose:
 *      Get the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - (OUT) Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_8729_medium_config_get(int unit, soc_port_t port, 
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    int            rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (pc->automedium || PHY_COPPER_MODE(unit, port)) {
            sal_memcpy(cfg, &pc->copper, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (pc->automedium || PHY_FIBER_MODE(unit, port)) {
            sal_memcpy(cfg, &pc->fiber, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_8729_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */
STATIC int
phy_8729_ability_advert_set(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    int        rv;
    phy_ctrl_t *pc;
    bmacsec_phy8729_port_ability_t local_ability;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    if (PHY_COPPER_MODE(unit, port)) {
        pc->copper.advert_ability = *ability;
    } else {
        pc->fiber.advert_ability = *ability;
    }

    local_ability = 0;
    if(ability->speed_half_duplex & SOC_PA_SPEED_10MB) {
        local_ability |= BMACSEC_PHY8729_PA_10MB_HD;
    }
    if(ability->speed_half_duplex & SOC_PA_SPEED_100MB) {
        local_ability |= BMACSEC_PHY8729_PA_100MB_HD;
    }
    if(ability->speed_full_duplex & SOC_PA_SPEED_10MB) {
        local_ability |= BMACSEC_PHY8729_PA_10MB_FD;
    }
    if(ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
        local_ability |= BMACSEC_PHY8729_PA_100MB_FD;
    }
    if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
        local_ability |= BMACSEC_PHY8729_PA_1000MB_FD;
    }
    if(ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
        local_ability |= BMACSEC_PHY8729_PA_10000MB_FD;
    }

    if(ability->pause & SOC_PA_PAUSE_TX) {
        local_ability |= BMACSEC_PHY8729_PA_PAUSE_TX;
    }
    if(ability->pause & SOC_PA_PAUSE_RX) {
        local_ability |= BMACSEC_PHY8729_PA_PAUSE_RX;
    }
    if(ability->pause & SOC_PA_PAUSE_ASYMM) {
        local_ability |= BMACSEC_PHY8729_PA_PAUSE_ASYMM;
    }


    if(ability->loopback & SOC_PA_LB_NONE) {
        local_ability |= BMACSEC_PHY8729_PA_LB_NONE;
    }
    if(ability->loopback & SOC_PA_LB_PHY) {
        local_ability |= BMACSEC_PHY8729_PA_LB_PHY;
    }
    if(ability->flags & SOC_PA_AUTONEG) {
        local_ability |= BMACSEC_PHY8729_PA_AN;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy8729_ability_advert_set(phy_dev_addr, local_ability);


    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_8729_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */
STATIC int
phy_8729_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    int        rv;
    bmacsec_phy8729_port_ability_t local_ability;
    bmacsec_dev_addr_t phy_dev_addr;

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    local_ability = 0;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy8729_ability_advert_get(phy_dev_addr, &local_ability);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(local_ability & BMACSEC_PHY8729_PA_10MB_HD) {
        ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_100MB_HD) {
        ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_10MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_100MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_1000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_10000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if(local_ability & BMACSEC_PHY8729_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(local_ability & BMACSEC_PHY8729_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(local_ability & BMACSEC_PHY8729_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }


    if(local_ability & BMACSEC_PHY8729_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(local_ability & BMACSEC_PHY8729_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(local_ability & BMACSEC_PHY8729_PA_AN) {
        ability->flags = SOC_PA_AUTONEG;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8729_ability_remote_get
 * Purpose:
 *      Get the current remote advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */
STATIC int
phy_8729_ability_remote_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    int        rv;
    bmacsec_phy8729_port_ability_t remote_ability;
    bmacsec_dev_addr_t phy_dev_addr;

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    remote_ability = 0;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy8729_remote_ability_advert_get(phy_dev_addr,
                                                   &remote_ability);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(remote_ability & BMACSEC_PHY8729_PA_10MB_HD) {
        ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
    }
    if(remote_ability & BMACSEC_PHY8729_PA_100MB_HD) {
        ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
    }
    if(remote_ability & BMACSEC_PHY8729_PA_10MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
    }
    if(remote_ability & BMACSEC_PHY8729_PA_100MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }
    if(remote_ability & BMACSEC_PHY8729_PA_1000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(remote_ability & BMACSEC_PHY8729_PA_10000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if(remote_ability & BMACSEC_PHY8729_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(remote_ability & BMACSEC_PHY8729_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(remote_ability & BMACSEC_PHY8729_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }


    if(remote_ability & BMACSEC_PHY8729_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(remote_ability & BMACSEC_PHY8729_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(remote_ability & BMACSEC_PHY8729_PA_AN) {
        ability->flags = SOC_PA_AUTONEG;
    }

    return SOC_E_NONE;
}



/*
 * Function:
 *      phy_8729_ability_local_get
 * Purpose:
 *      Get local abilities 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The ability is retrieved only for the ACTIVE medium.
 */

STATIC int
phy_8729_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int         rv = SOC_E_NONE;
    phy_ctrl_t *pc;
    bmacsec_phy8729_port_ability_t local_ability = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);
    rv = bmacsec_phy8729_ability_local_get(phy_dev_addr, &local_ability);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(local_ability & BMACSEC_PHY8729_PA_10MB_HD) {
        ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_100MB_HD) {
        ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_10MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_100MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_1000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(local_ability & BMACSEC_PHY8729_PA_10000MB_FD) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
    }

    if(local_ability & BMACSEC_PHY8729_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(local_ability & BMACSEC_PHY8729_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(local_ability & BMACSEC_PHY8729_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }


    if(local_ability & BMACSEC_PHY8729_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(local_ability & BMACSEC_PHY8729_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(local_ability & BMACSEC_PHY8729_PA_AN) {
        ability->flags = SOC_PA_AUTONEG;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8729_firmware_set
 * Purpose:
 *      program the given firmware into the SPI-ROM
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      offset - offset to the data stream
 *      array  - the given data
 *      datalen- the data length
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_8729_firmware_set(int unit, int port, int offset, uint8 *data,int len)
{
    int            rv;
    phy_ctrl_t *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);

    rv = bmacsec_phy8729_spi_firmware_update(phy_dev_addr, data, len);
    if (rv != BMACSEC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY8729 firmware upgrade possibly failed:"
                                 "u=%d p=%d\n"), unit, port));
        return (SOC_E_FAIL);
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY8729 firmware upgrade successful:"
                         "u=%d p=%d\n"), unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8729_medium_status
 * Purpose:
 *      Indicate the current active medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_XAUI
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_8729_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    if (PHY_COPPER_MODE(unit, port)) {
        *medium = SOC_PORT_MEDIUM_COPPER;
    } else {
        *medium = SOC_PORT_MEDIUM_FIBER;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_8729_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 * Returns:    
 *      SOC_E_NONE
 */
STATIC int
phy_8729_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv, pause_fwd = 0, line_speed, line_duplex, line_pause=1;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy8729_intf_t line_mode;

    rv = BMACSEC_E_NONE;    
    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc->macsec_enable != 1) {
        return SOC_E_UNAVAIL;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1); 
    rv = bmacsec_phy8729_line_intf_get(phy_dev_addr,
                                       pc->macsec_dev_port, &line_mode);
    if (rv != BMACSEC_E_NONE) {
        return SOC_ERROR(rv);
    }
    switch(type) {
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED:
            pc->macsec_switch_fixed = value;
            if(value) {
                rv = bmacsec_phy8729_switch_side_set_params(phy_dev_addr, 
                                BMACSEC_BCM8729_SWITCH_FIXED, 
                                pc->macsec_switch_fixed_speed,
                                pc->macsec_switch_fixed_duplex,
                                pc->macsec_switch_fixed_pause);
            } else { /* Switch side follows line side */
                SOC_IF_ERROR_RETURN
                    (phy_8729_speed_get(unit, port, &line_speed));
                SOC_IF_ERROR_RETURN
                    (phy_8729_duplex_get(unit, port, &line_duplex));
#if 0 
                SOC_IF_ERROR_RETURN
                    (phy_8729_pause_get(unit, port, &line_pause));
#endif
                rv = bmacsec_phy8729_switch_side_set_params(phy_dev_addr, 
                                BMACSEC_BCM8729_SWITCH_FOLLOWS_LINE, 
                                line_speed, line_duplex, line_pause);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED:
            /* Switch fixed speed is allowed only to be 1000Mbps */
            if (pc->macsec_dev_port > 2) {
                if (value != 1000) {
                    return SOC_E_CONFIG;
                }
            } else {
                if ((line_mode == BMACSEC_PHY8729_INTF_SGMII) || 
                    (line_mode == BMACSEC_PHY8729_INTF_1000X)) {
                    if (value != 1000) {
                        return SOC_E_CONFIG;
                    }
                } else {
                    if (value != 10000) {
                        return SOC_E_CONFIG;
                    }
                }
            } 

            if(pc->macsec_switch_fixed_speed != value) {
                pc->macsec_switch_fixed_speed = value;
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX:
            /* Switch fixed Duplex is allowed only to be Full Duplex mode */
            if (value != 1) {
                return SOC_E_CONFIG;
            }
            if(pc->macsec_switch_fixed_duplex != value) {
                pc->macsec_switch_fixed_duplex = value;
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE:
            if(pc->macsec_switch_fixed_pause != value) {
                pc->macsec_switch_fixed_pause = value;
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD:
            pc->macsec_pause_rx_fwd = value;
            pause_fwd = (value == 1) ? 1 : 0;

            rv = bmacsec_phy8729_line_side_pause_forward(phy_dev_addr,
                                                         pause_fwd);
            if (rv == BMACSEC_E_NONE) {
                rv = bmacsec_phy8729_switch_side_pause_forward(
                                                     phy_dev_addr, pause_fwd);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD:
            pc->macsec_pause_tx_fwd = value;
            pause_fwd = (value == 1) ? 1 : 0;

            rv = bmacsec_phy8729_line_side_pause_forward(phy_dev_addr,
                                                          pause_fwd);
            if (rv == BMACSEC_E_NONE) {
                rv = bmacsec_phy8729_switch_side_pause_forward(phy_dev_addr,
                                                                pause_fwd);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_LINE_IPG:
            pc->macsec_line_ipg = value;
            rv = bmacsec_phy8729_line_side_ipg_set(phy_dev_addr, value);
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_IPG:
            pc->macsec_switch_ipg = value;
            rv = bmacsec_phy8729_switch_side_ipg_set(phy_dev_addr, value);
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }

    return SOC_ERROR(rv);;
}

/*
 * Function:
 *      phy_8729_control_get
 * Purpose:
 *      Get current control settign of the PHY.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - (OUT)Current setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int 
phy_8729_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc->macsec_enable != 1) {
        return SOC_E_UNAVAIL;
    }

    if (NULL == value) {
        return SOC_E_PARAM;
    }

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(type) {
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED:
            *value = pc->macsec_switch_fixed;
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED:
            *value = pc->macsec_switch_fixed_speed;
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX:
            *value = pc->macsec_switch_fixed_duplex;
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE:
            *value = pc->macsec_switch_fixed_pause;
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD:
            *value = pc->macsec_pause_rx_fwd;
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD:
            *value = pc->macsec_pause_tx_fwd;
            break;
        case SOC_PHY_CONTROL_MACSEC_LINE_IPG:
            *value = pc->macsec_line_ipg;
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_IPG:
            *value = pc->macsec_switch_ipg;
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_8729_linkup
 * Purpose:
 *      Link up handler
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int 
phy_8729_linkup(int unit, soc_port_t port)
{
    int rv = SOC_E_NONE;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc->macsec_enable != 1) {
        return SOC_E_NONE;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 1);

    rv = bmacsec_8729_phy_linkup(phy_dev_addr, pc->macsec_enable);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_8729_interface_get
 * Purpose:
 *      Get interface on a given port.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      pif   - Interface.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_8729_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t     *pc;

    pc     = EXT_PHY_SW_STATE(unit, port);
    if ((pc->macsec_dev_port == 2) || (pc->macsec_dev_port == 3)) {
        *pif = SOC_PORT_IF_SGMII;
    } else {
        if (pc->phy_id1 == PHY_ID1_8729) {
            *pif = SOC_PORT_IF_XAUI;
        } else {
            *pif = PHY_COPPER_MODE(unit, port) ? SOC_PORT_IF_SGMII : SOC_PORT_IF_XFI;
        }
    }
    return SOC_E_NONE;
}




/*
 * Variable:
 *    phy_8729_drv
 * Purpose:
 *    Phy Driver for BCM8729 MACSEC PHY
 */

phy_driver_t phy_8729drv_gexe = {
    "8729 1G/10-Gigabit PHY Driver",
    phy_8729_init,        /* Init */
    phy_null_reset,       /* Reset */
    phy_8729_link_get,    /* Link get   */
    phy_8729_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_8729_duplex_set,  /* Duplex set */
    phy_8729_duplex_get,  /* Duplex get */
    phy_8729_speed_set,   /* Speed set  */
    phy_8729_speed_get,   /* Speed get  */
    phy_8729_master_set,  /* Master set */
    phy_8729_master_get,  /* Master get */
    phy_8729_an_set,      /* ANA set */
    phy_8729_an_get,      /* ANA get */
    NULL,                 /* Local Advert set, deprecated */
    NULL,                 /* Local Advert get, deprecated */
    NULL,                 /* Remote Advert get, deprecated */
    phy_8729_lb_set,      /* PHY loopback set */
    phy_8729_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_8729_interface_get, /* IO Interface get */
    NULL,                   /* pd_ability, deprecated */
    phy_8729_linkup,
    NULL,
    phy_8729_mdix_set,
    phy_8729_mdix_get,
    phy_8729_mdix_status_get,
    phy_8729_medium_config_set, /* medium config setting set */
    phy_8729_medium_config_get, /* medium config setting get */
    phy_8729_medium_status,        /* active medium */
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8729_control_set,    /* phy_control_set */
    phy_8729_control_get,    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
    NULL,          /* pd_probe  */
    phy_8729_ability_advert_set,  /* pd_ability_advert_set */
    phy_8729_ability_advert_get,  /* pd_ability_advert_get */
    phy_8729_ability_remote_get,  /* pd_ability_remote_get */
    phy_8729_ability_local_get,   /* pd_ability_local_get  */
    phy_8729_firmware_set
};


#endif /* INCLUDE_MACSEC */
#else /* INCLUDE_PHY_8729 */
typedef int _phy_8729_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8729 */
