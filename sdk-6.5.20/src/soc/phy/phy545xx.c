/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * File:        phy54580.c
 * Purpose:     PHY driver for BCM54580 (MACSec)
 *
 * Supported BCM546X Family of PHY devices:
 *
 *      Device  Ports   Media                           MAC Interface
 *      54584   8       6 Copper + 2 Fiber/Copper       QSGMII
 *      54580   8       Copper(no Fiber)                SGMII
 *      54540   4       Copper/Fiber Combo              SGMII
 *      54585   8       Copper                          QSGMII
 *
 * Workarounds:
 *
 * References:
 *     
 *    +----------+   +---------------------------+
 *    |          |   |            +->Copper      |<--> Magnetic
 *    |  QSGMII  |<->| QSGMII   <-+              |
 *    | (SerDes) |   |            |              |
 *    |          |   |            +->SGMII/      |<--> (PHY)SFP (10/100/1000)
 *    +----------+   |               1000BASE-X/ |<--> 1000BASE-X SFP
 *        Switch     |               100BASE-FX  |<--> 100BASE-FX SFP
 *                   |                           |
 *                   |                           |
 *                   +---------------------------+
 *                               54580
 *
 * Notes:
 */ 

#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_54580)
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
#include "phyfege.h"
#include "phy545xx.h"
#include <soc/macsecphy.h>

#define AUTO_MDIX_WHEN_AN_DIS   0       /* Non-standard-compliant option */
#define DISABLE_CLK125          0       /* Disable if unused to reduce */
                                        /*  EMI emissions */
#define WORKAROUND_FIX       1

#define ADVERT_ALL_COPPER \
        (SOC_PM_PAUSE | SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB)

#define ADVERT_ALL_FIBER \
        (SOC_PM_PAUSE | SOC_PM_100MB_FD | SOC_PM_1000MB_FD) 

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

#define MACSEC_READ_PHY_REG(_unit, _pc, _mdio, _reg, _value)     \
            ((_pc->read)((_unit), (_mdio), (_reg), (_value)))
#define MACSEC_WRITE_PHY_REG(_unit, _pc, _mdio, _reg, _value)     \
            ((_pc->write)((_unit), (_mdio), (_reg), (_value)))


STATIC int _phy_54580_medium_change(int unit, soc_port_t port, 
                          int force_update, soc_port_medium_t force_medium);
STATIC int phy_54580_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_54580_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_54580_master_set(int unit, soc_port_t port, int master);
STATIC int phy_54580_adv_local_set(int unit, soc_port_t port,
                                  soc_port_mode_t mode);
STATIC int phy_54580_autoneg_set(int unit, soc_port_t port, int autoneg);
STATIC int phy_54580_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
STATIC int phy_54580_speed_get(int unit, soc_port_t port, int *speed);
STATIC int _macsec_54580_rev0_init(phy_ctrl_t *pc, int dev_port, int dev_addr,
                   int macsec_enable);
STATIC int _macsec_54580_rev1_init(phy_ctrl_t *pc, int dev_port, int dev_addr,
                   int macsec_enable);

/***********************************************************************
 *
 * HELPER FUNCTIONS
 */
/*
 * Function:
 *      _phy_54580_medium_check
 * Purpose:
 *      Determine if chip should operate in copper or fiber mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) SOC_PORT_MEDIUM_XXX
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_54580_medium_check(int unit, soc_port_t port, int *medium)
{
    phy_ctrl_t    *pc = EXT_PHY_SW_STATE(unit, port);
    int rv;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy54580_port_medium_t bmacsec_port_medium;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0);
    rv = bmacsec_phy54580_medium_get(phy_dev_addr, &bmacsec_port_medium); 
    if(!BMACSEC_E_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }

    *medium = SOC_PORT_MEDIUM_COPPER;

    if (pc->automedium) {
        if (pc->fiber.preferred) {
            if (bmacsec_port_medium == BMACSEC_PHY_MEDIUM_COPPER) {
                /* Only Copper is lined up */
                *medium = SOC_PORT_MEDIUM_COPPER;
            } else {
                *medium = SOC_PORT_MEDIUM_FIBER;
            }
        } else {
            if (bmacsec_port_medium == BMACSEC_PHY_MEDIUM_FIBER) {
                /* Only Fiber is linked up */
                *medium = SOC_PORT_MEDIUM_FIBER;
            } else {
                *medium = SOC_PORT_MEDIUM_COPPER;
            }
        }
    } else {
       *medium = pc->fiber.preferred ? SOC_PORT_MEDIUM_FIBER:
                  SOC_PORT_MEDIUM_COPPER; 
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "_phy_54580_medium_check: "
                            "u=%d p=%d fiber_pref=%d fiber=%d\n"),
                 unit, port, pc->fiber.preferred,
                 (*medium == SOC_PORT_MEDIUM_FIBER) ? 1 : 0));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54580_medium_status
 * Purpose:
 *      Indicate the current active medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_FIBER
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_54580_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
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
 *      _phy_54580_medium_config_update
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
_phy_54580_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_54580_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_54580_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_54580_master_set(unit, port, cfg->master));
    SOC_IF_ERROR_RETURN
        (phy_54580_adv_local_set(unit, port, cfg->autoneg_advert));
    SOC_IF_ERROR_RETURN
        (phy_54580_autoneg_set(unit, port, cfg->autoneg_enable));
    SOC_IF_ERROR_RETURN
        (phy_54580_mdix_set(unit, port, cfg->mdix));

    return SOC_E_NONE;
}

/***********************************************************************
 *
 * PHY54580 DRIVER ROUTINES
 */

/*
 * Function:
 *      phy_54580_medium_config_set
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
phy_54580_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t    *pc;
    soc_phy_config_t *active_medium;  /* Currently active medium */
    soc_phy_config_t *change_medium;  /* Requested medium */
    soc_phy_config_t *other_medium;   /* The other medium */
    int               medium_update;
    soc_port_mode_t   advert_mask;

    if (NULL == cfg) {
        return SOC_E_PARAM;
    }

    pc            = EXT_PHY_SW_STATE(unit, port);
    medium_update = FALSE;

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
    if (change_medium->enable != cfg->enable) {
        medium_update = TRUE;
    }
    if (change_medium->preferred != cfg->preferred) {
        /* Make sure that only one medium is preferred */
        other_medium->preferred = !cfg->preferred;
        medium_update = TRUE;
    }

    sal_memcpy(change_medium, cfg, sizeof(*change_medium));
    change_medium->autoneg_advert &= advert_mask;

    if (medium_update) {
        /* The new configuration may cause medium change. Check
         * and update medium.
         */
        SOC_IF_ERROR_RETURN
            (_phy_54580_medium_change(unit, port, TRUE, medium));
    } else {
        active_medium = (PHY_COPPER_MODE(unit, port)) ?  
                            &pc->copper : &pc->fiber;
        if (active_medium == change_medium) {
            /* If the medium to update is active, update the configuration */
            SOC_IF_ERROR_RETURN
                (_phy_54580_medium_config_update(unit, port, change_medium));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54580_medium_config_get
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
phy_54580_medium_config_get(int unit, soc_port_t port, 
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
 *      _phy_54580_no_reset_setup
 * Purpose:
 *      Setup the operating parameters of the PHY without resetting PHY
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_54580_no_reset_setup(int unit, soc_port_t port, int fiber)
{
    phy_ctrl_t    *int_pc;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    int rv = SOC_E_NONE;
    int fiber_capable = 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_54580_reset_setup: u=%d p=%d medium=%s\n"),
                         unit, port,
              PHY_COPPER_MODE(unit, port) ? "COPPER" : "FIBER"));

    pc      = EXT_PHY_SW_STATE(unit, port);
    int_pc  = INT_PHY_SW_STATE(unit, port);

    if (NULL != int_pc) {
        SOC_IF_ERROR_RETURN
            (PHY_INIT(int_pc->pd, unit, port));

        if (PHY_FIBER_MODE(unit, port)) {
           /* Interface to MAC or internal SerDes is always SGMII mode.
            * Typical internal SerDes driver set the interface to 1000BASE-X,
            * if the media is fiber. Therefore, we need to force the interface 
            * of internal SerDes Driver to SGMII. */
           SOC_IF_ERROR_RETURN
               (PHY_NOTIFY(int_pc->pd, unit, port, phyEventInterface,
                           SOC_PORT_IF_SGMII));
        } 

        if (PHY_COPPER_MODE(unit, port) && 
            !PHY_SGMII_AUTONEG_MODE(unit, port)) {
            /* Make sure internal SerDes is in power down state 
             * until the MAC is ready. */
            SOC_IF_ERROR_RETURN
                (PHY_NOTIFY(int_pc->pd, unit, port, 
                            phyEventStop, PHY_STOP_COPPER));
        }
    }
 
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_init(phy_dev_addr, 0,
                               pc->automedium,
                               fiber,
                               pc->fiber_detect,
                               pc->macsec_enable,
                               &fiber_capable);
   if (rv != BMACSEC_E_NONE) {
       return SOC_E_FAIL;
   }
   return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54580_init
 * Purpose:
 *      Init function for 54580 PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_54580_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    int        fiber_capable, rv;
    int        fiber_preferred;
    int        qsgmii_mdio;
    bmacsec_dev_addr_t macsec_dev_addr, qsgmii_dev_addr, phy_dev_addr;
    int         mmi_mdio_addr, port_index, macsec_enable;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54xxx_init: u=%d p=%d\n"),
                         unit, port));

    /* Only SGMII interface to switch MAC is supported */ 
    if (IS_GMII_PORT(unit, port)) {
        return SOC_E_CONFIG; 
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    pc->interface = SOC_PORT_IF_SGMII;

    qsgmii_mdio = (pc->phy_id - pc->macsec_dev_port + 8); 

    /* 
     * For 54540 the qsgmii_mdio will contain MDIO address
     * of SGMII
     */
    if (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL) {
        qsgmii_mdio = (pc->phy_id + 4); 
    }

    mmi_mdio_addr = soc_property_port_get(unit, port, spn_MACSEC_DEV_ADDR, -1);
    if (mmi_mdio_addr == -1) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54580_init: "
                             "MACSEC_DEV_ADDR property "
                             "not configured for u=%d p=%d\n"), unit, port));
        return SOC_E_CONFIG;
    }

    port_index = soc_property_port_get(unit, port, spn_MACSEC_PORT_INDEX, -1);
    if (port_index == -1) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54580_init: "
                             "MACSEC_PORT_INDEX property not configured for "
                             "u=%d p=%d\n"), unit, port));
        return SOC_E_CONFIG;
    }

    pc->macsec_dev_addr = mmi_mdio_addr;
    pc->macsec_dev_port = port_index;

    phy_dev_addr    = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    macsec_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->macsec_dev_addr, 0);
    qsgmii_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, qsgmii_mdio, 0); 

    macsec_enable = soc_property_port_get(unit, port, spn_MACSEC_ENABLE, 0);
    pc->macsec_enable = macsec_enable;

    if (pc->phy_rev == 0) {
        rv = _macsec_54580_rev0_init(pc, pc->macsec_dev_port, pc->macsec_dev_addr,
                                  macsec_enable);
    } else if (pc->phy_rev == 1) {
        rv = _macsec_54580_rev1_init(pc, pc->macsec_dev_port, pc->macsec_dev_addr,
                                  macsec_enable);
    } else {
        rv = SOC_E_UNAVAIL;
    }
    if (!SOC_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }

    if (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL) {
        pc->macsec_dev_port = port_index + 4;
    }

    /* call common soc function to create port and setyp defaults */
    if (pc->phy_rev == 0) {
        rv = soc_macsecphy_init(unit, port, pc, 
                            BMACSEC_CORE_BCM5458X, bmacsec_io_mmi);
    } else {
        rv = soc_macsecphy_init(unit, port, pc, 
                            BMACSEC_CORE_BCM5458X_B0, bmacsec_io_mmi1);
    }
    if (!SOC_SUCCESS(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "soc_macsecphy_init: MACSEC init for"
                              " u=%d p=%d FAILED "), unit, port));
        return SOC_E_FAIL;
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "soc_macsecphy_init: MACSEC init for"
                             " u=%d p=%d SUCCESS "), unit, port));
    }

    rv = bmacsec_phy54580_phy_mac_addr_set(phy_dev_addr, macsec_dev_addr, 
                                   qsgmii_dev_addr, 
                                   qsgmii_mdio, pc->macsec_dev_port,
                                   (pc->phy_rev) ? bmacsec_io_mmi1 : bmacsec_io_mmi); 
    if(rv != BMACSEC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54580_init: bmacsec_phy_mac_addr_set "
                             "returned error for u=%d p=%d\n"), unit, port));
        return SOC_E_FAIL;
    }

    fiber_capable = 1;

    /* Change it to copper prefer for default.
     * Because the SD of fiber doesn't work in BCM54580
     */
    fiber_preferred =
        soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 0);
    pc->automedium =
        soc_property_port_get(unit, port, spn_PHY_AUTOMEDIUM, 1);
    pc->fiber_detect =
        soc_property_port_get(unit, port, spn_PHY_FIBER_DETECT, 0);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54580_init: "
                         "u=%d p=%d type=54580%s automedium=%d fiber_pref=%d detect=%d\n"),
              unit, port, fiber_capable ? "S" : "",
              pc->automedium, fiber_preferred, pc->fiber_detect));

    /* Init PHYS and MACs to defaults */
    rv = bmacsec_phy54580_init(phy_dev_addr, 1,
                               pc->automedium,
                               fiber_preferred,
                               pc->fiber_detect,
                               pc->macsec_enable,
                               &fiber_capable);

    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    /* Based on automedium, fiber_preferred and the PHY model, fiber enable is 
     * set to 1 or 0.
     * For 54584 devices only port 7 and port 8 are fiber capable hence
     * fiber properties apply to port 7 and port 8. 
     * 54580 is copper only PHY, hence only copper properties are set
     */
    if (fiber_capable == 0) { /* not fiber capable */
        fiber_preferred  = 0;
        pc->automedium   = 0;
        pc->fiber_detect = 0;
    }

    if (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL) {
        if (pc->automedium == 0) {
            /* Power down non-preferred media */
            if (fiber_preferred == 0) {  /* Power Down Fiber */
                rv = bmacsec_phy54580_enable_set(phy_dev_addr, 0, 
                                                 BMACSEC_PHY_FIBER, 0);
            } else { /* Power down copper */
                rv = bmacsec_phy54580_enable_set(phy_dev_addr, 0, 
                                                 BMACSEC_PHY_COPPER, 0);
            }
        }
    }
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    pc->copper.enable = TRUE;
    pc->copper.preferred = !fiber_preferred;
    pc->copper.autoneg_enable = TRUE;
    pc->copper.autoneg_advert = ADVERT_ALL_COPPER;
    pc->copper.force_speed = 1000;
    pc->copper.force_duplex = TRUE;
    pc->copper.master = SOC_PORT_MS_AUTO;
    pc->copper.mdix = SOC_PORT_MDIX_AUTO;

    pc->fiber.enable = fiber_capable;
    pc->fiber.preferred = fiber_preferred;
    pc->fiber.autoneg_enable = fiber_capable;
    pc->fiber.autoneg_advert = ADVERT_ALL_FIBER;
    pc->fiber.force_speed = 1000;
    pc->fiber.force_duplex = TRUE;
    pc->fiber.master = SOC_PORT_MS_NONE;
    pc->fiber.mdix = SOC_PORT_MDIX_NORMAL;

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
    pc->macsec_switch_fixed_speed = 1000;
    pc->macsec_switch_fixed_duplex = 1;
    pc->macsec_switch_fixed_pause = 1;
    pc->macsec_pause_rx_fwd = 0;
    pc->macsec_pause_tx_fwd = 0;
    pc->macsec_line_ipg = 0xc;
    pc->macsec_switch_ipg = 0xc;


    /* Set LED Modes and Control */
    rv = bmacsec_phy54580_set_led_selectors(phy_dev_addr, pc->ledmode[0],
                                            pc->ledmode[1], pc->ledmode[2],
                                            pc->ledmode[3], pc->ledselect,
                                            pc->ledctrl);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if (0) {
    SOC_IF_ERROR_RETURN
        (_phy_54580_medium_config_update(unit, port,
                                        PHY_COPPER_MODE(unit, port) ?
                                        &pc->copper :
                                        &pc->fiber));
    }
    return SOC_E_NONE;
}


/*
 * Function: 	
 *	phy_54580_reset
 * Purpose:	
 *	Reset PHY and wait for it to come out of reset.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 * Returns:	
 *	SOC_E_XXX
 */

int
phy_54580_reset(int unit, soc_port_t port, void *user_arg)
{

    int rv;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_reset(phy_dev_addr);

    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_54580_enable_set
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
phy_54580_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;
    int rv = SOC_E_NONE;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy54580_port_mode_t port_mode = 0;

    pc     = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_enable_set(phy_dev_addr, pc->automedium,
                                     port_mode, enable);
    if(rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if (pc->automedium || PHY_FIBER_MODE(unit, port)) {
        phy_ctrl_t  *int_pc;

        int_pc = INT_PHY_SW_STATE(unit, port);
        if (NULL != int_pc) {
            SOC_IF_ERROR_RETURN
                (PHY_ENABLE_SET(int_pc->pd, unit, port, enable));
        }
    
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54580_enable_set: "
                             "Power %s fiber medium\n"), (enable) ? "up" : "down"));
    }

    /* Update software state */
    SOC_IF_ERROR_RETURN(phy_fe_ge_enable_set(unit, port, enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54580_enable_get
 * Purpose:
 *      Enable or disable the physical interface for a 54580 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54580_enable_get(int unit, soc_port_t port, int *enable)
{
    return phy_fe_ge_enable_get(unit, port, enable);
}

/*
 * Function:
 *      _phy_54580_medium_change
 * Purpose:
 *      Change medium when media change detected or forced
 * Parameters:
 *      unit         - StrataSwitch unit #.
 *      port         - StrataSwitch port #.
 *      force_update - Force update
 *      force_medium - Force update medium
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_54580_medium_change(int unit, soc_port_t port, int force_update,
                         soc_port_medium_t force_medium)
{
    phy_ctrl_t    *pc;
    int            medium;

    pc    = EXT_PHY_SW_STATE(unit, port);

    if (force_update == TRUE) {
        medium = force_medium;
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_54580_medium_check(unit, port, &medium));
    }

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        if (!PHY_COPPER_MODE(unit, port) || force_update) { /* Was fiber */ 
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
            SOC_IF_ERROR_RETURN
                (_phy_54580_no_reset_setup(unit, port, 0));

            /* Do Not power up the interface if medium is disabled */
            if (pc->copper.enable) {
                SOC_IF_ERROR_RETURN
                    (_phy_54580_medium_config_update(unit, port, &pc->copper));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_54580_link_auto_detect: u=%d p=%d [F->C]\n"),
                      unit, port));
        }
    } else {        /* Fiber */
        if (PHY_COPPER_MODE(unit, port) || force_update) { /* Was copper */
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
            SOC_IF_ERROR_RETURN
                (_phy_54580_no_reset_setup(unit, port, 1));

            SOC_IF_ERROR_RETURN
                (_phy_54580_medium_config_update(unit, port, &pc->fiber));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_54580_link_auto_detect: u=%d p=%d [C->F]\n"),
                      unit, port));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54580_link_get
 * Purpose:
 *      Determine the current link up/down status for a 54580 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      link - (OUT) Boolean, true indicates link established.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If using automedium, also switches the mode.
 */
STATIC int
phy_54580_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t    *pc;
    uint16         data; 
    int rv = SOC_E_NONE;
    bmacsec_phy54580_port_mode_t port_mode = 0;
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

    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
    if (pc->automedium) {
        /* Check for medium change and update HW/SW accordingly. */
        SOC_IF_ERROR_RETURN
            (_phy_54580_medium_change(unit, port, FALSE, 0));
    }

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy_54580_link_get(phy_dev_addr, port_mode, &bmacsec_link);
    if (!BMACSEC_E_SUCCESS(rv)) {
        return SOC_E_FAIL;
    }
    if (bmacsec_link == 1) {
        *link = TRUE;
    } else {
        *link = FALSE;
    }

    /* If preferred medium is up, disable the other medium 
     * to prevent false link. */
    if (pc->automedium) {
        if (pc->copper.preferred) {
            if (pc->fiber.enable) {
                /* Power down Serdes if Copper mode is up */
                data = (*link && PHY_COPPER_MODE(unit, port)) ? 0 : 1;
            } else {
                data = 0;

            }
            port_mode = BMACSEC_PHY_FIBER;
            (void)bmacsec_phy54580_enable_set(phy_dev_addr, pc->automedium,
                                                 port_mode, data);
        } else {  /* pc->fiber.preferred */
            if (pc->copper.enable) {
                data = (*link && PHY_FIBER_MODE(unit, port)) ? 0 : 1;
            } else {
                data = 0;
            }
            port_mode = BMACSEC_PHY_COPPER;
            (void)bmacsec_phy54580_enable_set(phy_dev_addr, pc->automedium,
                                             port_mode, data);
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_54580_link_get: u=%d p=%d mode=%s%s link=%d\n"),
                 unit, port,
                 PHY_COPPER_MODE(unit, port) ? "C" : "F",
                 PHY_FIBER_100FX_MODE(unit, port)? "(100FX)" : "",
                 *link));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54580_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - Half duplex requested, and not supported.
 * Notes:
 *      The duplex is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 *      Autonegotiation is not manipulated.
 */

STATIC int
phy_54580_duplex_set(int unit, soc_port_t port, int duplex)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_phy54580_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy54580_duplex_t bmacsec_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
            if (duplex == 0) {
                return SOC_E_UNAVAIL;
            }
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    if (duplex) {
        bmacsec_duplex = BMACSEC_FULL_DUPLEX;
    } else {
        bmacsec_duplex = BMACSEC_HALF_DUPLEX;
    }
    rv = bmacsec_phy54580_duplex_set(phy_dev_addr, port_mode, 
                                     bmacsec_duplex);

    if(BMACSEC_E_SUCCESS(rv)) {
        pc->copper.force_duplex = duplex;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54580_duplex_set: u=%d p=%d d=%d rv=%d\n"),
              unit, port, duplex, rv));
    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_54580_duplex_get
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
phy_54580_duplex_get(int unit, soc_port_t port, int *duplex)
{

    int rv = SOC_E_NONE;
    bmacsec_phy54580_port_mode_t port_mode = 0;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy54580_duplex_t bmacsec_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
            *duplex = TRUE;
            return SOC_E_NONE;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_duplex_get(phy_dev_addr, port_mode, 
                                     &bmacsec_duplex);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }
    if (bmacsec_duplex == BMACSEC_FULL_DUPLEX) {
        *duplex = TRUE;
    } else {
        *duplex = FALSE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54580_speed_set
 * Purpose:
 *      Set the current operating speed (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Requested speed, only 1000 supported.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The speed is set only for the ACTIVE medium.
 */

STATIC int
phy_54580_speed_set(int unit, soc_port_t port, int speed)
{
    int            rv; 
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy54580_port_mode_t port_mode = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
            if (speed == 10) {
                return SOC_E_UNAVAIL;
            }
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0);
    rv = bmacsec_phy54580_speed_set(phy_dev_addr, port_mode, speed);

    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.force_speed = speed;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                pc->fiber.force_speed = speed;

                if (speed == 100) {
                        PHY_FLAGS_SET(unit, port, PHY_FLAGS_100FX);
                        pc->fiber.autoneg_enable = FALSE;
                        phy_54580_autoneg_set(unit, port, 0);
                }

                if (speed == 1000) {
                        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
                        phy_54580_autoneg_set(unit, port,
                                              pc->fiber.autoneg_enable);
                }
            }
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54580_speed_set: u=%d p=%d s=%d fiber=%d rv=%d\n"),
              unit, port, speed, PHY_FIBER_MODE(unit, port), rv));

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54580_speed_get
 * Purpose:
 *      Get the current operating speed for a 54580 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The speed is retrieved for the ACTIVE medium.
 */

STATIC int
phy_54580_speed_get(int unit, soc_port_t port, int *speed)
{
    int rv = SOC_E_NONE;
    bmacsec_phy54580_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0);
    rv = bmacsec_phy54580_speed_get(phy_dev_addr, port_mode, speed);

    if(!BMACSEC_E_SUCCESS(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54580_speed_get: u=%d p=%d invalid speed\n"),
                  unit, port));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54580_speed_get: u=%d p=%d speed=%d"),
                  unit, port, *speed));
    }

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54580_master_set
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
phy_54580_master_set(int unit, soc_port_t port, int master)
{
    int                  rv;
    phy_ctrl_t    *pc;
    bmacsec_phy54580_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_master_set(phy_dev_addr, port_mode, master);
    if(BMACSEC_E_SUCCESS(rv)) {
        pc->copper.master = master;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54580_master_set: u=%d p=%d master=%d fiber=%d rv=%d\n"),
              unit, port, master, PHY_FIBER_MODE(unit, port), rv));

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54580_master_get
 * Purpose:
 *      Get the current master mode for a 54580 device.
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
phy_54580_master_get(int unit, soc_port_t port, int *master)
{
    int        rv;
    phy_ctrl_t *pc;
    bmacsec_phy54580_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
            *master = SOC_PORT_MS_NONE;
            return SOC_E_NONE;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_master_get(phy_dev_addr, port_mode, master);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54580_autoneg_set
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
phy_54580_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    int                 rv;
    phy_ctrl_t   *pc;
    int autoneg_advert = 0;
    bmacsec_phy54580_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;


    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
        autoneg_advert = 0;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
            autoneg_advert = pc->fiber.autoneg_advert;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_an_set(phy_dev_addr, port_mode,
                                 autoneg, autoneg_advert);
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
                         "phy_54580_autoneg_set: u=%d p=%d autoneg=%d rv=%d\n"),
              unit, port, autoneg, rv));
    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54580_autoneg_get
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
phy_54580_autoneg_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done)
{
    int           rv;
    phy_ctrl_t   *pc;
    bmacsec_phy54580_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    rv            = SOC_E_NONE;
    *autoneg      = FALSE;
    *autoneg_done = FALSE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_an_get(phy_dev_addr, port_mode, autoneg,
                                 autoneg_done);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54580_adv_local_set
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
phy_54580_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    int           rv;
    phy_ctrl_t    *pc;
    bmacsec_phy54580_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_port_mode_t bmacsec_mode = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    if(mode & SOC_PM_10MB_HD) {
        bmacsec_mode |= BMACSEC_PM_10MB_HD;
    }
    if(mode & SOC_PM_10MB_FD) {
        bmacsec_mode |= BMACSEC_PM_10MB_FD;
    }
    if(mode & SOC_PM_100MB_HD) {
        bmacsec_mode |= BMACSEC_PM_100MB_HD;
    }
    if(mode & SOC_PM_100MB_FD) {
        bmacsec_mode |= BMACSEC_PM_100MB_FD;
    }
    if(mode & SOC_PM_1000MB_HD) {
        bmacsec_mode |= BMACSEC_PM_1000MB_HD;
    }
    if(mode & SOC_PM_1000MB_FD) {
        bmacsec_mode |= BMACSEC_PM_1000MB_FD;
    }
    if((mode & SOC_PM_PAUSE) == SOC_PM_PAUSE) {
        bmacsec_mode |= BMACSEC_PM_PAUSE;
    }
    if(mode & SOC_PM_PAUSE_TX) {
        bmacsec_mode |= BMACSEC_PM_PAUSE_TX;
    }
    if(mode & SOC_PM_PAUSE_RX) {
        bmacsec_mode |= BMACSEC_PM_PAUSE_RX;
    }


    rv = bmacsec_phy54580_adv_local_set(phy_dev_addr, port_mode, bmacsec_mode);

    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.autoneg_advert = mode & ADVERT_ALL_COPPER;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                pc->fiber.autoneg_advert = mode & ADVERT_ALL_FIBER;
            }
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54580_adv_local_set: u=%d p=%d mode=0x%x, rv=%d\n"),
              unit, port, mode, rv));
    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_54580_adv_local_get
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
phy_54580_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{

    int          rv;
    bmacsec_phy54580_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    phy_ctrl_t   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_port_mode_t bmacsec_mode = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_adv_local_get(phy_dev_addr, port_mode, 
                                        &bmacsec_mode);
    *mode = 0;
    if (rv == BMACSEC_E_NONE) {
        if(bmacsec_mode & BMACSEC_PM_10MB_HD) {
            *mode |= SOC_PM_10MB_HD;
        }
        if(bmacsec_mode & BMACSEC_PM_10MB_FD) {
            *mode |= SOC_PM_10MB_FD;
        }
        if(bmacsec_mode & BMACSEC_PM_100MB_HD) {
            *mode |= SOC_PM_100MB_HD;
        }
        if(bmacsec_mode & BMACSEC_PM_100MB_FD) {
            *mode |= SOC_PM_100MB_FD;
        }
        if(bmacsec_mode & BMACSEC_PM_1000MB_HD) {
            *mode |= SOC_PM_1000MB_HD;
        }
        if(bmacsec_mode & BMACSEC_PM_1000MB_FD) {
            *mode |= SOC_PM_1000MB_FD;
        }
        if((bmacsec_mode & BMACSEC_PM_PAUSE) == BMACSEC_PM_PAUSE) {
            *mode |= SOC_PM_PAUSE;
        }
        if(bmacsec_mode & BMACSEC_PM_PAUSE_TX) {
            *mode |= SOC_PM_PAUSE_TX;
        }
        if(bmacsec_mode & BMACSEC_PM_PAUSE_RX) {
            *mode |= SOC_PM_PAUSE_RX;
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54580_adv_local_get: u=%d p=%d mode=0x%x, rv=%d\n"),
              unit, port, *mode, rv));
    return  (SOC_ERROR(rv));

}

/*
 * Function:
 *      phy_54580_adv_remote_get
 * Purpose:
 *      Get partners current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The remote advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. If Autonegotiation is
 *      disabled or in progress, this routine will return an error.
 */

STATIC int
phy_54580_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    int                  rv;
    phy_ctrl_t    *pc;
    bmacsec_phy54580_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_port_mode_t bmacsec_mode = 0;


    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_adv_remote_get(phy_dev_addr, port_mode, 
                                         &bmacsec_mode);
    *mode = 0;
    if (rv == BMACSEC_E_NONE) {
        if(bmacsec_mode & BMACSEC_PM_10MB_HD) {
            *mode |= SOC_PM_10MB_HD;
        }
        if(bmacsec_mode & BMACSEC_PM_10MB_FD) {
            *mode |= SOC_PM_10MB_FD;
        }
        if(bmacsec_mode & BMACSEC_PM_100MB_HD) {
            *mode |= SOC_PM_100MB_HD;
        }
        if(bmacsec_mode & BMACSEC_PM_100MB_FD) {
            *mode |= SOC_PM_100MB_FD;
        }
        if(bmacsec_mode & BMACSEC_PM_1000MB_HD) {
            *mode |= SOC_PM_1000MB_HD;
        }
        if(bmacsec_mode & BMACSEC_PM_1000MB_FD) {
            *mode |= SOC_PM_1000MB_FD;
        }
        if((bmacsec_mode & BMACSEC_PM_PAUSE) == BMACSEC_PM_PAUSE) {
            *mode |= SOC_PM_PAUSE;
        }
        if(bmacsec_mode & BMACSEC_PM_PAUSE_TX) {
            *mode |= SOC_PM_PAUSE_TX;
        }
        if(bmacsec_mode & BMACSEC_PM_PAUSE_RX) {
            *mode |= SOC_PM_PAUSE_RX;
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54580_adv_remote_get: u=%d p=%d mode=0x%x, rv=%d\n"),
              unit, port, *mode, rv));
    return  (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_54580_lb_set
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
phy_54580_lb_set(int unit, soc_port_t port, int enable)
{
    int           rv;
    phy_ctrl_t    *pc;
    bmacsec_phy54580_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

#if AUTO_MDIX_WHEN_AN_DIS
    {
        /* Disable Auto-MDIX When autoneg disabled */
        /* Enable Auto-MDIX When autoneg disabled */
        data = (enable) ? 0x0000 : 0x0200;
        rv = BMACSEC_MODIFY_PHY_54580_MII_MISC_CTRLr(pc->phy_id, data, 0x0200);
        if(!BMACSEC_E_SUCCESS(rv)) {
            return SOC_ERROR(rv);
        }

    }
#endif

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_loopback_set(phy_dev_addr, port_mode, enable);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54580_lb_set: u=%d p=%d en=%d rv=%d\n"), 
              unit, port, enable, rv));

    return SOC_ERROR(rv); 
}

/*
 * Function:
 *      phy_54580_lb_get
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
phy_54580_lb_get(int unit, soc_port_t port, int *enable)
{
    int                  rv;
    phy_ctrl_t    *pc;
    bmacsec_phy54580_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_loopback_get(phy_dev_addr, port_mode, enable);

    return SOC_ERROR(rv); 
}

/*
 * Function:
 *      phy_54580_interface_set
 * Purpose:
 *      Set the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_UNAVAIL - unsupported interface
 */

STATIC int
phy_54580_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    switch (pif) {
    case SOC_PORT_IF_SGMII:
    case SOC_PORT_IF_GMII:
        return SOC_E_NONE;
    default:
        return SOC_E_UNAVAIL;
    }
}

/*
 * Function:
 *      phy_54580_interface_get
 * Purpose:
 *      Get the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_54580_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_SGMII;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54580_ability_get
 * Purpose:
 *      Get the abilities of the local gigabit PHY.
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
phy_54580_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    int         rv;
    phy_ctrl_t *pc;
    bmacsec_phy54580_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_port_mode_t ability = 0;
    bmacsec_dev_addr_t phy_dev_addr;

    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0);
    rv = bmacsec_phy54580_ability_get(phy_dev_addr, port_mode, &ability);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    *mode = 0;
    if(ability & BMACSEC_PM_10MB_HD) {
        *mode |= SOC_PM_10MB_HD;
    }
    if(ability & BMACSEC_PM_10MB_FD) {
        *mode |= SOC_PM_10MB_FD;
    }
    if(ability & BMACSEC_PM_100MB_HD) {
        *mode |= SOC_PM_100MB_HD;
    }
    if(ability & BMACSEC_PM_100MB_FD) {
        *mode |= SOC_PM_100MB_FD ;
    }
    if(ability & BMACSEC_PM_1000MB_HD) {
        *mode |= SOC_PM_1000MB_HD;
    }
    if(ability & BMACSEC_PM_1000MB_FD) {
        *mode |= SOC_PM_1000MB_FD;
    }
    if(ability & BMACSEC_PM_PAUSE_TX) {
        *mode |= SOC_PM_PAUSE_TX;
    }
    if(ability & BMACSEC_PM_PAUSE_RX) {
        *mode |= SOC_PM_PAUSE_RX;
    }
    if(ability & BMACSEC_PM_PAUSE_ASYMM) {
        *mode |= SOC_PM_PAUSE_ASYMM;
    }
    if(ability & BMACSEC_PM_TBI) {
        *mode |= SOC_PM_TBI;
    }
    if(ability & BMACSEC_PM_MII) {
        *mode |= SOC_PM_MII;
    }
    if(ability & BMACSEC_PM_GMII) {
        *mode |= SOC_PM_GMII;
    }
    if(ability & BMACSEC_PM_SGMII) {
        *mode |= SOC_PM_SGMII;
    }
    if(ability & BMACSEC_PM_LB_NONE) {
        *mode |= SOC_PM_LB_NONE;
    }
    if(ability & BMACSEC_PM_LB_PHY) {
        *mode |= SOC_PM_LB_PHY;
    }
    if(ability & BMACSEC_PM_AN) {
        *mode |= SOC_PM_AN;
    }

    if (pc->automedium) {
        *mode |= SOC_PM_COMBO;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54580_mdix_set
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
phy_54580_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{

    int         rv;
    phy_ctrl_t  *pc;
    bmacsec_phy54580_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_port_mdix_t bmacsec_mode;

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
            if (mode != SOC_PORT_MDIX_NORMAL) {
                return SOC_E_UNAVAIL;
            }
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    switch(mode) {
    case SOC_PORT_MDIX_AUTO:
        bmacsec_mode = BMACSEC_PORT_MDIX_AUTO;
        break;
    case SOC_PORT_MDIX_FORCE_AUTO:
        bmacsec_mode = BMACSEC_PORT_MDIX_FORCE_AUTO;
        break;
    case SOC_PORT_MDIX_NORMAL:
        bmacsec_mode = BMACSEC_PORT_MDIX_NORMAL;
        break;
    case SOC_PORT_MDIX_XOVER:
        bmacsec_mode = BMACSEC_PORT_MDIX_XOVER;
        break;
    default :
        return SOC_E_CONFIG;
    }

    rv = bmacsec_phy54580_mdix_set(phy_dev_addr, port_mode, bmacsec_mode);

    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.mdix = mode;
        }
    }
    return SOC_ERROR(rv);
}        

/*
 * Function:
 *      phy_54580_mdix_get
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
phy_54580_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    if (mode == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(phy_54580_speed_get(unit, port, &speed));
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
 *      phy_54580_mdix_status_get
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
phy_54580_mdix_status_get(int unit, soc_port_t port, 
                         soc_port_mdix_status_t *status)
{
    int         rv;
    phy_ctrl_t    *pc;

    bmacsec_phy54580_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_port_mdix_status_t mdix_status;

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54580_mdix_status_get(phy_dev_addr, port_mode, 
                                          &mdix_status);
    if(BMACSEC_E_SUCCESS(rv)) {
        if(mdix_status == BMACSEC_PORT_MDIX_STATUS_NORMAL) {
            *status = SOC_PORT_MDIX_STATUS_NORMAL;
        }
        if(mdix_status == BMACSEC_PORT_MDIX_STATUS_XOVER) {
            *status = SOC_PORT_MDIX_STATUS_XOVER;
        }
        return SOC_E_NONE;
    }

    return SOC_ERROR(rv);
}    

/*
 * Function:
 *      phy_54580_cable_diag
 * Purpose:
 *      Run 546x cable diagnostics
 * Parameters:
 *      unit - device number
 *      port - port number
 *      status - (OUT) cable diagnotic status structure
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_54580_cable_diag(int unit, soc_port_t port,
                    soc_port_cable_diag_t *status)
{
    int                 rv, rv2, i;
    bmacsec_dev_addr_t phy_dev_addr;
    phy_ctrl_t    *pc;
    int fiber_capable = 0;

    extern int phy_5464_cable_diag_sw(int, soc_port_t , 
                                      soc_port_cable_diag_t *);
    pc = EXT_PHY_SW_STATE(unit, port);

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    status->state = SOC_PORT_CABLE_STATE_OK;
    status->npairs = 4;
    status->fuzz_len = 0;
    for (i = 0; i < 4; i++) {
        status->pair_state[i] = SOC_PORT_CABLE_STATE_OK;
    }

    MIIM_LOCK(unit);    /* this locks out linkscan, essentially */
    rv = phy_5464_cable_diag_sw(unit,port, status);
    MIIM_UNLOCK(unit);
    rv2 = 0;
    if (rv <= 0) {      /* don't reset if > 0 -- link was up */
        phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
        rv2 = bmacsec_phy54580_init(phy_dev_addr, 1,
                               pc->automedium,
                               pc->fiber.preferred,
                               pc->fiber_detect,
                               pc->macsec_enable,
                               &fiber_capable);
    }
    if (rv >= 0 && rv2 < 0) {
        return rv2;
    }
    return rv;
}

/*
 * Function:
 *      phy_54580_control_set
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
phy_54580_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv, pause_fwd = 0;
    phy_ctrl_t    *pc;
    bmacsec_phy54580_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc->macsec_enable != 1) {
        return SOC_E_UNAVAIL;
    }

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        } else {
            return SOC_E_NONE;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 

    rv = BMACSEC_E_NONE;    
    switch(type) {
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED:
            pc->macsec_switch_fixed = value;
            if(value) {
                rv = bmacsec_phy54580_switch_side_set_params(phy_dev_addr, 
                                port_mode,
                                BMACSEC_SWITCH_FIXED, 
                                pc->macsec_switch_fixed_speed,
                                pc->macsec_switch_fixed_duplex,
                                pc->macsec_switch_fixed_pause);
            } else { /* Switch side follows line side */
                rv = bmacsec_phy54580_switch_side_set_params(phy_dev_addr, 
                                port_mode,
                                BMACSEC_SWITCH_FOLLOWS_LINE, 
                                -1, -1, -1);
            }

            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED:
            /* Switch fixed speed is allowed only to be 1000Mbps */
            if (value != 1000) {
                return SOC_E_CONFIG;
            }
            if(pc->macsec_switch_fixed_speed != value) {
                int line_speed;

                pc->macsec_switch_fixed_speed = value;

                /* 
                 * If PHY is 54584 then switch side speed need to line side 
                 * speed.
                 */
                if ((pc->phy_oui == BMACSEC_PHY_BRCM_OUI2) &&
                    (pc->phy_model == BMACSEC_PHY_BCM54584_MODEL)) {

                    SOC_IF_ERROR_RETURN
                        (phy_54580_speed_get(unit, port, &line_speed));
                    pc->macsec_switch_fixed_speed = line_speed;

                }


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

            rv = bmacsec_phy54580_line_side_pause_forward(phy_dev_addr,
                                                          pause_fwd);
            if (rv == BMACSEC_E_NONE) {
                rv = bmacsec_phy54580_switch_side_pause_forward(
                                                     phy_dev_addr, pause_fwd);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD:
            pc->macsec_pause_tx_fwd = value;
            pause_fwd = (value == 1) ? 1 : 0;

            rv = bmacsec_phy54580_line_side_pause_forward(phy_dev_addr,
                                                          pause_fwd);
            if (rv == BMACSEC_E_NONE) {
                rv = bmacsec_phy54580_switch_side_pause_forward(phy_dev_addr,
                                                                pause_fwd);
            }
            break;
        case SOC_PHY_CONTROL_MACSEC_LINE_IPG:
            pc->macsec_line_ipg = value;
            rv = bmacsec_phy54580_line_side_ipg_set(phy_dev_addr, value);
            break;
        case SOC_PHY_CONTROL_MACSEC_SWITCH_IPG:
            pc->macsec_switch_ipg = value;
            rv = bmacsec_phy54580_switch_side_ipg_set(phy_dev_addr, value);
            break;
        default:
            return SOC_E_UNAVAIL;
    }

    if (rv != BMACSEC_E_NONE) {
        return SOC_ERROR(rv);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54580_control_get
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
phy_54580_control_get(int unit, soc_port_t port,
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
 *      phy_54580_linkup
 * Purpose:
 *      Link up handler
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int 
phy_54580_linkup(int unit, soc_port_t port)
{
    int rv;
    int line_speed;
    phy_ctrl_t    *pc;
    bmacsec_phy54580_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;


    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc->macsec_enable != 1) {
        return SOC_E_NONE;
    }

    /* 
     * For 54584 PHYs when in switch fixed mode, the line speed 
     * has to follow the switch side otherwise the switch fixed will
     * not work 
     */
    if ((pc->phy_oui == BMACSEC_PHY_BRCM_OUI2) &&
        (pc->phy_model == BMACSEC_PHY_BCM54584_MODEL)) {

        SOC_IF_ERROR_RETURN
            (phy_54580_speed_get(unit, port, &line_speed));
        pc->macsec_switch_fixed_speed = line_speed;

        if (pc->macsec_switch_fixed == 1) {

            if(PHY_COPPER_MODE(unit, port)) {
                port_mode = BMACSEC_PHY_COPPER;
            } else {
                port_mode = BMACSEC_PHY_FIBER;
            }

            phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
            rv = bmacsec_phy54580_switch_side_set_params(phy_dev_addr, 
                                              port_mode,
                                              BMACSEC_SWITCH_FIXED,
                                              pc->macsec_switch_fixed_speed,
                                              pc->macsec_switch_fixed_duplex,
                                              pc->macsec_switch_fixed_pause);
            if (BMACSEC_E_FAILURE(rv)) {
                return SOC_E_FAIL;
            }
        }

    }
    return SOC_E_NONE;
}

/*
 * 54580 Top level register Read
 */
STATIC int
macsecphy_54580_toplvl_reg_read(phy_ctrl_t *pc, uint8 phy_base_addr, 
                                uint8 reg_offset, uint16 *data)

{
    int    unit;
    uint8  port3_phy_id, port5_phy_id;
    uint16 reg_data, status;

    if (!pc) {
        return SOC_E_FAIL;
    }
    unit = pc->unit;
    port3_phy_id = phy_base_addr + 2;
    port5_phy_id = phy_base_addr + 4;
    /* port6_phy_id = phy_base_addr + 6; Used for writes only */

    /* Write Reg address to Port 5's register 0x1C, shadow 0x0B */
    /* Status READ from Port 3's register 0x15 */


    /* Write Reg offset to Port 5's register 0x1C, shadow 0x0B */
    reg_data = (0xAC00 | reg_offset);
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port5_phy_id, 0x1c, reg_data));

    /* Read data from Top level MII Status register(0x15h) */
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port3_phy_id, 0x17, 0x8F0B));
    SOC_IF_ERROR_RETURN
        (MACSEC_READ_PHY_REG(unit, pc, port3_phy_id, 0x15, &status));

    *data = (status & 0xff);

    return SOC_E_NONE;
}


/*
 * 54580 Top level register Write
 */
STATIC int
macsecphy_54580_toplvl_reg_write(phy_ctrl_t *pc, uint8 phy_base_addr, 
                                 uint8 reg_offset, uint16 data)

{
    int    unit;
    uint8  port3_phy_id, port6_phy_id, port5_phy_id;
    uint16 status, reg_data;

    if (!pc) {
        return SOC_E_FAIL;
    }
    unit = pc->unit;

    port3_phy_id = phy_base_addr + 2;
    port5_phy_id = phy_base_addr + 4;
    port6_phy_id = phy_base_addr + 5;

    /* Write Reg address to Port 5's register 0x1C, shadow 0x0B */
    /* Write data to Port 6's register 0x1C, shadow 0x0c */
    /* Status READ from Port 3's register 0x15 */

    /* Write Data to port6, register 0x1C, shadow 0x0c */
    reg_data = (0xB000 | (data & 0xff));
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port6_phy_id, 0x1c, reg_data));

    /* Write Reg address to Port 5's register 0x1C, shadow 0x0B */
    /* Enable Write ( Port 5's register 0x1C, shadow 0x0B) Bit 7 = 1 */
    reg_data = (0xAC80 | reg_offset);
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port5_phy_id, 0x1c, reg_data));

    /* Disable Write ( Port 5's register 0x1C, shadow 0x0B) Bit 7 = 0 */
    reg_data = (0xAC00 | reg_offset);
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port5_phy_id, 0x1c, reg_data));

    status = 0;
#ifdef BROADCOM_DEBUG
    /* Read data from Top level MII Status register(0x15h) */
    SOC_IF_ERROR_RETURN
        (MACSEC_WRITE_PHY_REG(unit, pc, port3_phy_id, 0x17, 0x8F0B));
    SOC_IF_ERROR_RETURN
        (MACSEC_READ_PHY_REG(unit, pc, port3_phy_id, 0x15, &status));

#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      _macsec_54580_rev0_init
 * Purpose:
 *      Initialize MACSEC Top Level configuration
 *
 * Parameters:
 *      pc       - Phy control structure
 *      dev_port - Port in the MACSEC device
 *      dev_addr - MMI address
 *
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_macsec_54580_rev0_init(phy_ctrl_t *pc, int dev_port, int dev_addr,
                   int macsec_enable)
{
    int   unit, port, rev;
    uint8 phy_base_add, reg_offset;
    uint16 data;
 
#define MACSEC_MDIO_ENABLE                                (1U << 5)
#define MACSEC_MDIO_ADDR_MASK                             0x1F
#define MACSEC_INTERRUPT_ENABLE                           (1U << 3)
#define MACSEC_TOP_LVL_PORT_PASS_THRU_REG_OFFSET          0x2C
#define MACSEC_TOP_LVL_PORT_GLOBAL_PASS_THRU_REG_OFFSET   0x2D
#define MACSEC_TOP_LVL_MDIO_ENABLE_REG_OFFSET             0x2E
#define MACSEC_TOP_LVL_QUAD_MEDIA_DETECT_REG_OFFSET       0x0A
#define MACSEC_TOP_LVL_DUAL_SERDES_ENABLE                 0x2F

    if (!pc) {
        return SOC_E_FAIL;
    }
    unit = pc->unit;
    port = pc->port;
    rev  = pc->phy_rev;

    phy_base_add = pc->phy_id - dev_port;

    if (macsec_enable) { 

        /* Enable MACSEC in Top level register 0x2D */
        reg_offset = MACSEC_TOP_LVL_PORT_GLOBAL_PASS_THRU_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        /* Write to register 0x2D to Enable MACSEC  GLOBALLY */
        if ((data & 0x1) == 0) {
            int         ii;
            uint16      aer_addr;

            data |= 1;  /* Enable MACSEC GLOBALLY */
            if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                                 reg_offset, data) < 0) {
                return SOC_E_FAIL;
            }
            if ((pc->phy_model == BMACSEC_PHY_BCM54584_MODEL) ||
                (pc->phy_model == BMACSEC_PHY_BCM54585_MODEL)) {
                /*
                 * Reset QSGMII
                 */
                for(ii = 0; ii < 8; ii++) {
                    aer_addr = phy_base_add + 8 + ii;
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + 8,
                                              0x1f, 0xffd0));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + 8,
                                              0x1e, aer_addr));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add+8,
                                              0x1f, 0x0000));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add+8,
                                              0x00, 0x9140));
                }
            }
            if ((pc->phy_model == BMACSEC_PHY_BCM54580_MODEL) ||
                (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL)) {
                /*
                 * Reset SGMII
                 */
                for(ii = 0; ii < 8; ii++) {
                    uint16 blk_sel;

                    /* Map 1000X page */ 
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                              0x1c, 0x7c00));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_READ_PHY_REG(unit, pc, phy_base_add + ii,
                                             0x1c, &blk_sel));
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                              0x1c, blk_sel | 0x8001));

                    /* Write 1000X IEEE register */
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                              0x0, 0x8000));

                   /* Restore IEEE mapping */
                    SOC_IF_ERROR_RETURN
                        (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                              0x1c, 
                                              (blk_sel & 0xfffe) | 0x8000));
                }
            }
            if (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL) {
                reg_offset = MACSEC_TOP_LVL_QUAD_MEDIA_DETECT_REG_OFFSET;
                if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
                    return SOC_E_FAIL;
                }
                data &= ~(0x1f);
                data |= 0x10;  /* Enable MACSEC GLOBALLY */
                if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                                 reg_offset, data) < 0) {
                    return SOC_E_FAIL;
                }
            }
        }

#if 0 
        /* Enable MACSEC in Top level register 0x2c */
        reg_offset = MACSEC_TOP_LVL_PORT_PASS_THRU_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        /* Write to register 0x2C to Enable MACSEC */
        data |= (1 << dev_port);  /* Enable MACSEC on given port */
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                            reg_offset, data) < 0) {
            return SOC_E_FAIL;
        }
#endif

        /* Enable MACSEC MDIO and program MACSEC core MDIO address */
        reg_offset = MACSEC_TOP_LVL_MDIO_ENABLE_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        data &= ~(0x3f);
        data |= (MACSEC_MDIO_ENABLE | (dev_addr & MACSEC_MDIO_ADDR_MASK));
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                        reg_offset, data) < 0) {
            return SOC_E_FAIL;
        }

        /* Enable MACSEC interrupts */
        reg_offset = MACSEC_TOP_LVL_PORT_GLOBAL_PASS_THRU_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        data &= ~(0x08);
        data |= MACSEC_INTERRUPT_ENABLE;
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                        reg_offset, data) < 0) {
            return SOC_E_FAIL;
        }

    } else {
        /* Disable MACSEC in Top level register 0x2D */
        reg_offset = MACSEC_TOP_LVL_PORT_GLOBAL_PASS_THRU_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        /* Write to register 0x2D to Disable MACSEC GLOBALLY */
        data &= 0xfffe;  /* Disable MACSEC GLOBALLY (bit 0 = 0) */
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                                 reg_offset, data) < 0) {
           return SOC_E_FAIL;
        }
    }
    if (pc->phy_model == BMACSEC_PHY_BCM54584_MODEL) {
        /* Enable Serders in 0x2F reg */
        reg_offset = MACSEC_TOP_LVL_DUAL_SERDES_ENABLE;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        data |= 0x00c0; /* set Bits 6 and 7 */
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                             reg_offset, data) < 0) {
           return SOC_E_FAIL;
        }

   }
 

    return SOC_E_NONE;
}

/*
 * 54580 rev 1 Top level register Read
 */
STATIC int
macsecphy_54580_rev1_toplvl_reg_read(phy_ctrl_t *pc, uint8 mdio_addr, 
                                uint8 reg_offset, uint16 *data)

{
    int    unit = pc->unit;

    SOC_IF_ERROR_RETURN(
            MACSEC_WRITE_PHY_REG(unit, pc, mdio_addr,
                                 0x17, (0x0D00|(reg_offset & 0xff))));
    SOC_IF_ERROR_RETURN(
            MACSEC_READ_PHY_REG(unit, pc, mdio_addr, 0x15, data));

    return SOC_E_NONE;
}


/*
 * 54580 rev 1Top level register Write
 */
STATIC int
macsecphy_54580_rev1_toplvl_reg_write(phy_ctrl_t *pc, uint8 mdio_addr, 
                                 uint8 reg_offset, uint16 val)

{
    int    unit = pc->unit;

    SOC_IF_ERROR_RETURN(
            MACSEC_WRITE_PHY_REG(unit, pc, mdio_addr,
                                 0x17, (0x0D00|(reg_offset & 0xff))));
    SOC_IF_ERROR_RETURN(
            MACSEC_WRITE_PHY_REG(unit, pc, mdio_addr, 0x15, val));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _macsec_54580_rev1_init
 * Purpose:
 *      Initialize MACSEC Top Level configuration
 *
 * Parameters:
 *      pc       - Phy control structure
 *      dev_port - Port in the MACSEC device
 *      dev_addr - MMI address
 *
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_macsec_54580_rev1_init(phy_ctrl_t *pc, int dev_port, int dev_addr,
                   int macsec_enable)
{
    int   unit, port, rev;
    uint8 phy_base_add;
    uint16 data;

#define MACSEC_R1_MDIO_ENABLE           (1U << 13)
#define MACSEC_R1_INTERRUPT_ENABLE      (1U << 0)
#define MACSEC_R1_BYPASS_ENABLE         (1U << 13)

    unit = pc->unit;
    port = pc->port;
    rev  = pc->phy_rev;

    phy_base_add = pc->phy_id - dev_port;

#if 0
            SOC_IF_ERROR_RETURN(
                MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add, 0x17, 0xf7f));
            SOC_IF_ERROR_RETURN(
                MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add, 0x15, 0x3));
#endif
    /* Enable MACSEC in Top level register 0xC8  */
    if (macsecphy_54580_rev1_toplvl_reg_read(pc, phy_base_add, 
                                             0xC8, &data) < 0) {
        return SOC_E_FAIL;
    }
    /* Write to register 0x2D to Enable MACSEC  GLOBALLY */
    if ((data & MACSEC_R1_MDIO_ENABLE) == 0) {
        /* Enable MACSEC GLOBALLY and program dev_addr */
        data = MACSEC_R1_MDIO_ENABLE | MACSEC_R1_INTERRUPT_ENABLE |
                ((dev_addr & 0x1f) << 8); 
        if (macsecphy_54580_rev1_toplvl_reg_write(pc, phy_base_add, 
                                                  0xC8, data) < 0) {
            return SOC_E_FAIL;
        }
    }

    /* Enable/Disable MACSEC bypass */
    if (macsecphy_54580_rev1_toplvl_reg_read(pc, pc->phy_id, 
                                             0x07, &data) < 0) {
        return SOC_E_FAIL;
    }
    data &= ~MACSEC_R1_BYPASS_ENABLE;
    if (!macsec_enable) {
        data |= MACSEC_R1_BYPASS_ENABLE;
    }

    if (macsecphy_54580_rev1_toplvl_reg_write(pc, pc->phy_id, 
                                              0x07, data)< 0) {
        return SOC_E_FAIL;
    }

    #if 0
    if ((pc->phy_model == BMACSEC_PHY_BCM54584_MODEL) ||
        (pc->phy_model == BMACSEC_PHY_BCM54585_MODEL)) {
        /*
         * Reset QSGMII
         */
        for(ii = 0; ii < 8; ii++) {
            aer_addr = phy_base_add + 8 + ii;
            SOC_IF_ERROR_RETURN
                (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + 8,
                                      0x1f, 0xffd0));
            SOC_IF_ERROR_RETURN
                (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + 8,
                                      0x1e, aer_addr));
            SOC_IF_ERROR_RETURN
                (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add+8,
                                      0x1f, 0x0000));
            SOC_IF_ERROR_RETURN
                (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add+8,
                                      0x00, 0x9140));
        }
    }
    if ((pc->phy_model == BMACSEC_PHY_BCM54580_MODEL) ||
        (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL)) {
        /*
         * Reset SGMII
         */
        for(ii = 0; ii < 8; ii++) {
            uint16 blk_sel;

            /* Map 1000X page */ 
            SOC_IF_ERROR_RETURN
                (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                      0x1c, 0x7c00));
            SOC_IF_ERROR_RETURN
                (MACSEC_READ_PHY_REG(unit, pc, phy_base_add + ii,
                                     0x1c, &blk_sel));
            SOC_IF_ERROR_RETURN
                (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                      0x1c, blk_sel | 0x8001));

            /* Write 1000X IEEE register */
            SOC_IF_ERROR_RETURN
                (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                      0x0, 0x8000));

           /* Restore IEEE mapping */
            SOC_IF_ERROR_RETURN
                (MACSEC_WRITE_PHY_REG(unit, pc, phy_base_add + ii,
                                      0x1c, 
                                      (blk_sel & 0xfffe) | 0x8000));
        }
    }
    if (pc->phy_model == BMACSEC_PHY_BCM54540_MODEL) {
        reg_offset = MACSEC_TOP_LVL_QUAD_MEDIA_DETECT_REG_OFFSET;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                               reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        data &= ~(0x1f);
        data |= 0x10;  /* Enable MACSEC GLOBALLY */
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                         reg_offset, data) < 0) {
            return SOC_E_FAIL;
        }
    }
    #endif

    #if 0
    if (pc->phy_model == BMACSEC_PHY_BCM54584_MODEL) {
        /* Enable Serders in 0x2F reg */
        reg_offset = MACSEC_TOP_LVL_DUAL_SERDES_ENABLE;
        if (macsecphy_54580_toplvl_reg_read(pc, phy_base_add, 
                                       reg_offset, &data) < 0) {
            return SOC_E_FAIL;
        }
        data |= 0x00c0; /* set Bits 6 and 7 */
        if (macsecphy_54580_toplvl_reg_write(pc, phy_base_add, 
                                             reg_offset, data) < 0) {
           return SOC_E_FAIL;
        }

   }
   #endif
 

    return SOC_E_NONE;
}


#ifdef BROADCOM_DEBUG

/*
 * Function:
 *      phy_54580_shadow_dump
 * Purpose:
 *      Debug routine to dump all shadow registers.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 */

void
phy_54580_shadow_dump(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    uint16      tmp;
    int         i;

    pc       = EXT_PHY_SW_STATE(unit, port);

    /* Register 0x18 Shadows */
    for (i = 0; i <= 7; i++) {
        WRITE_PHY_REG(unit, pc, 0x18, (i << 12) | 0x7);
        READ_PHY_REG(unit, pc, 0x18, &tmp);
        if ((tmp & ~7) == 0x0000) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "0x18[0x%x]=0x%04x\n"), i, tmp));
    }

    /* Register 0x1c Shadows */
    for (i = 0; i <= 0x1f; i++) {
        WRITE_PHY_REG(unit, pc, 0x1c, i << 10);
        READ_PHY_REG(unit, pc, 0x1c, &tmp);
        if ((tmp & ~0x7c00) == 0x0000) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "0x1c[0x%x]=0x%04x\n"), i, tmp));
    }

    /* Register 0x17/0x15 Shadows */
    for (i = 0; i <= 0x13; i++) {
        WRITE_PHY_REG(unit, pc, 0x17, 0xf00 | i);
        READ_PHY_REG(unit, pc, 0x15, &tmp);
        if (tmp  == 0x0000) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "0x17[0x%x]=0x%04x\n"), i, tmp));
    }
}

#endif /* BROADCOM_DEBUG */

/*
 * Variable:    phy_54580drv_ge
 * Purpose:     PHY driver for 54580
 */
phy_driver_t phy_54580drv_ge = {
    "54580 (MACSec PHY) Gigabit PHY Driver",
    phy_54580_init,
    phy_54580_reset,
    phy_54580_link_get,
    phy_54580_enable_set,
    phy_54580_enable_get,
    phy_54580_duplex_set,
    phy_54580_duplex_get,
    phy_54580_speed_set,
    phy_54580_speed_get,
    phy_54580_master_set,
    phy_54580_master_get,
    phy_54580_autoneg_set,
    phy_54580_autoneg_get,
    phy_54580_adv_local_set,
    phy_54580_adv_local_get,
    phy_54580_adv_remote_get,
    phy_54580_lb_set,
    phy_54580_lb_get,
    phy_54580_interface_set,
    phy_54580_interface_get,
    phy_54580_ability_get,
    phy_54580_linkup,            /* Phy link up event */
    NULL,                        /* Phy link down event */
    phy_54580_mdix_set,
    phy_54580_mdix_get,
    phy_54580_mdix_status_get,
    phy_54580_medium_config_set,
    phy_54580_medium_config_get,
    phy_54580_medium_status,
    phy_54580_cable_diag,
    NULL,                       /* phy_link_change */
    phy_54580_control_set,      /* phy_control_set */ 
    phy_54580_control_get,      /* phy_control_get */
    phy_ge_reg_read,
    phy_ge_reg_write,
    phy_ge_reg_modify,
    NULL                        /* Phy event notify */
};
#endif /* INCLUDE_MACSEC */
#else /* INCLUDE_PHY_54580 */
typedef int _soc_phy_54580_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_54580 */
