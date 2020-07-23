/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * File:        phy54380.c
 * Purpose:     PHY driver for BCM54380 (MACSec)
 *
 * Supported BCM546X Family of PHY devices:
 *
 *      Device  Ports   Media                           MAC Interface
 *      54380   8       Copper(no Fiber)                SGMII
 *      54382   8       Copper(no Fiber)                2 QSGMII
 *      54340   4       Copper/Fiber                    4 SGMII
 *      54385   8       Copper/Fiber                    2 QSGMII

 * Workarounds:
 *
 *
 * Notes:
 */ 

#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_54380)
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
#include "phy543xx.h"
#include <soc/macsecphy.h>

#define DISABLE_CLK125          0       /* Disable if unused to reduce */
                                        /*  EMI emissions */


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

/*Copper*/
#define ADVERT_ALL_COPPER_FLOW_CTRL   (_SHR_PA_PAUSE_TX | _SHR_PA_PAUSE_RX)
#define ADVERT_ALL_COPPER_HD_SPEED    (_SHR_PA_SPEED_10MB |  _SHR_PA_SPEED_100MB | _SHR_PA_SPEED_1000MB) 
#define ADVERT_ALL_COPPER_FD_SPEED    (_SHR_PA_SPEED_10MB |  _SHR_PA_SPEED_100MB | _SHR_PA_SPEED_1000MB)

/*Fiber*/
#define ADVERT_ALL_FIBER_FLOW_CTRL    (_SHR_PA_PAUSE_TX | _SHR_PA_PAUSE_RX)
#define ADVERT_ALL_FIBER_FD_SPEED     (_SHR_PA_SPEED_100MB | _SHR_PA_SPEED_1000MB)


#define SET_FLOW_CTRL_ABILITY(ability, p) ((ability)->pause |= p)
#define SET_HD_SPEED_ABILITY(ability, s)  ((ability)->speed_half_duplex |= s)
#define SET_FD_SPEED_ABILITY(ability, s)  ((ability)->speed_full_duplex |= s)

#define ADVERT_ALL_COPPER(ability) {\
        SET_FLOW_CTRL_ABILITY(ability, ADVERT_ALL_COPPER_FLOW_CTRL);\
        SET_HD_SPEED_ABILITY(ability, ADVERT_ALL_COPPER_HD_SPEED);\
        SET_FD_SPEED_ABILITY(ability, ADVERT_ALL_COPPER_FD_SPEED);\
        }

#define ADVERT_ALL_FIBER(ability) {\
        SET_FLOW_CTRL_ABILITY(ability, ADVERT_ALL_FIBER_FLOW_CTRL);\
        SET_FD_SPEED_ABILITY(ability, ADVERT_ALL_FIBER_FD_SPEED);\
        }

#define PHY_IS_BCM54380(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54380_OUI, \
                                  PHY_BCM54380_MODEL))
  
#define PHY_IS_BCM54382(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54382_OUI, \
                                  PHY_BCM54382_MODEL) \
                                  && (((_pc)->phy_rev & 0x8)))
  
#define PHY_IS_BCM54340(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54340_OUI, \
                                  PHY_BCM54340_MODEL)) 
  
#define PHY_IS_BCM54385(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54385_OUI, \
                                  PHY_BCM54385_MODEL) \
                                  && (!((_pc)->phy_rev & 0x8)))
  
#define PHY_IS_BCM54385_B0(_pc) ((PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54385_OUI, \
                                  PHY_BCM54385_MODEL))\
                                  && ((_pc)->phy_rev  == 0x1))

#define PHY_BCM54385_B0_REV (0x1)
#define PHY_BCM54385_C0_REV (0x2)

/* 
   Flag(s) to specify config options to mac 
   It should eventually be moved to mac_ctrl.h.
 */
#define MAC_CTRL_FLAG_MACSEC_FIX_LATENCY_EN (1<<0)


STATIC int _phy_54380_medium_change(int unit, soc_port_t port, 
                          int force_update, soc_port_medium_t force_medium);
STATIC int phy_54380_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_54380_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_54380_master_set(int unit, soc_port_t port, int master);

#if 0

STATIC int phy_54380_adv_local_set(int unit, soc_port_t port,i
                                  /*soc_port_mode_t mode*/soc_port_ability_t ability);

#endif

STATIC int phy_54380_autoneg_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done);
STATIC int phy_54380_autoneg_set(int unit, soc_port_t port, int autoneg);
STATIC int phy_54380_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
STATIC int phy_54380_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_54380_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability);
extern int
phy_ecd_cable_diag_init_40nm(int unit, soc_port_t port);
extern int
phy_ecd_cable_diag_40nm(int unit, soc_port_t port,
            soc_port_cable_diag_t *status, uint16 ecd_ctrl);

extern int
bmacsec_phy54380_macsec_enable(int phy_id, bmacsec_dev_addr_t bmacsec_addr, int macsec_enable);

/***********************************************************************
 *
 * HELPER FUNCTIONS
 */
/*
 * Function:
 *      _phy_54380_medium_check
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
_phy_54380_medium_check(int unit, soc_port_t port, int *medium)
{

    phy_ctrl_t    *pc = EXT_PHY_SW_STATE(unit, port);
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy_port_medium_t bmacsec_port_medium;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0);
    
    *medium = SOC_PORT_MEDIUM_COPPER;

    if (pc->automedium) {
    
        bmacsec_phy54380_medium_get(phy_dev_addr, 
                                   pc->fiber.preferred, 
                                   &bmacsec_port_medium); 

        if (bmacsec_port_medium == BMACSEC_PHY_MEDIUM_COPPER) {
            *medium = SOC_PORT_MEDIUM_COPPER;
        } else {
            *medium = SOC_PORT_MEDIUM_FIBER;
        }
    } else {
       *medium = pc->fiber.preferred ? SOC_PORT_MEDIUM_FIBER:
                  SOC_PORT_MEDIUM_COPPER; 
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "_phy_54380_medium_check: "
                            "u=%d p=%d fiber_pref=%d fiber=%d\n"),
                 unit, port, pc->fiber.preferred,
                 (*medium == SOC_PORT_MEDIUM_FIBER) ? 1 : 0));

    return SOC_E_NONE;
}
 

/*
 * Function:
 *      phy_54380_medium_status
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
phy_54380_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
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
 *      _phy_54380_medium_config_update
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
_phy_54380_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_54380_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_54380_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_54380_master_set(unit, port, cfg->master));
    SOC_IF_ERROR_RETURN
        (phy_54380_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_54380_autoneg_set(unit, port, cfg->autoneg_enable));
    SOC_IF_ERROR_RETURN
        (phy_54380_mdix_set(unit, port, cfg->mdix));

    return SOC_E_NONE;
}


/*
 * Function:
 *     phy_54380_pa_2_bmacsec_pa 
 * Purpose:
 *     Translate soc_port_ability to 
 *     bmacsec_port_ability
 * Parameters:
 *      soc_port_ability_t
 *      bmacsec_port_ability_t
 * Returns:
 */
static void 
phy_54380_pa_2_bmacsec_pa( soc_port_ability_t *ability, 
                    bmacsec_port_ability_t *bmacsec_ability)
{
    if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
        bmacsec_ability->speed_full_duplex |= BMACSEC_PA_SPEED_1000MB;
    }
    if(ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
        bmacsec_ability->speed_full_duplex |= BMACSEC_PA_SPEED_100MB;
    }
    if(ability->speed_full_duplex & SOC_PA_SPEED_10MB) {
        bmacsec_ability->speed_full_duplex |= BMACSEC_PA_SPEED_10MB;
    }
    if(ability->speed_half_duplex & SOC_PA_SPEED_1000MB) {
        bmacsec_ability->speed_half_duplex |= BMACSEC_PA_SPEED_1000MB;
    }
    if(ability->speed_half_duplex & SOC_PA_SPEED_100MB) {
        bmacsec_ability->speed_half_duplex |= BMACSEC_PA_SPEED_100MB;
    }
    if(ability->speed_half_duplex & SOC_PA_SPEED_10MB) {
        bmacsec_ability->speed_half_duplex |= BMACSEC_PA_SPEED_10MB;
    }
    if(ability->pause & SOC_PA_PAUSE_TX) {
        bmacsec_ability->pause |= BMACSEC_PA_PAUSE_TX;
    }
    if(ability->pause & SOC_PA_PAUSE_RX) {
        bmacsec_ability->pause |= BMACSEC_PA_PAUSE_RX;
    }
    if(ability->pause & SOC_PA_PAUSE_ASYMM) {
        bmacsec_ability->pause |= BMACSEC_PA_PAUSE_ASYMM;
    }
    if(ability->loopback & SOC_PA_LB_NONE) {
        bmacsec_ability->loopback |= BMACSEC_PA_LB_NONE;
    }
    if(ability->loopback & SOC_PA_LB_PHY) {
        bmacsec_ability->loopback |= BMACSEC_PA_LB_PHY;
    }
    if(ability->flags & SOC_PA_AUTONEG) {
        bmacsec_ability->flags |= BMACSEC_PA_AUTONEG;
    }
    if(ability->eee & SOC_PA_EEE_1GB_BASET) {
        bmacsec_ability->eee |= BMACSEC_PA_EEE_1GB_BASET;
    }
    if(ability->eee & SOC_PA_EEE_100MB_BASETX) {
        bmacsec_ability->eee |= BMACSEC_PA_EEE_100MB_BASETX;
    }
    return;
}


/*
 * Function:
 *     phy_54380_bmacsec_pa_2_pa 
 * Purpose:
 *     Translate bmacsecc_port_ability to 
 *     soc_port_ability
 * Parameters:
 *      bmacsec_port_ability_t
 *      soc_port_ability_t
 * Returns:
 */
static void 
phy_54380_bmacsec_pa_2_pa( bmacsec_port_ability_t *bmacsec_ability, 
                    soc_port_ability_t *ability)
{
    if(bmacsec_ability->speed_full_duplex & BMACSEC_PA_SPEED_1000MB) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(bmacsec_ability->speed_full_duplex & BMACSEC_PA_SPEED_100MB) {
        ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }
    if(bmacsec_ability->speed_full_duplex & BMACSEC_PA_SPEED_10MB) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
    }
    if(bmacsec_ability->speed_half_duplex & BMACSEC_PA_SPEED_1000MB) {
        ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(bmacsec_ability->speed_half_duplex & BMACSEC_PA_SPEED_100MB) {
        ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
    }
    if(bmacsec_ability->speed_half_duplex & BMACSEC_PA_SPEED_10MB) {
        ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
    }
    if(bmacsec_ability->pause & BMACSEC_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(bmacsec_ability->pause & BMACSEC_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(bmacsec_ability->pause & BMACSEC_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }
    if(bmacsec_ability->loopback & BMACSEC_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(bmacsec_ability->loopback & BMACSEC_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(bmacsec_ability->flags & BMACSEC_PA_AUTONEG) {
        ability->flags |= SOC_PA_AUTONEG;
    }
     if(bmacsec_ability->eee & BMACSEC_PA_EEE_1GB_BASET) {
        ability->eee |= SOC_PA_EEE_1GB_BASET;
    }
    if(bmacsec_ability->eee & BMACSEC_PA_EEE_100MB_BASETX) {
        ability->eee |= SOC_PA_EEE_100MB_BASETX;
    }
    return;
}

/***********************************************************************
 *
 * PHY54380 DRIVER ROUTINES
 */





/*
 * Function:
 *      phy_54380_medium_config_set
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
phy_54380_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t    *pc;
    soc_phy_config_t *active_medium;  /* Currently active medium */
    soc_phy_config_t *change_medium;  /* Requested medium */
    soc_phy_config_t *other_medium;   /* The other medium */
    int               medium_update;
    soc_port_ability_t advert_ability;
    sal_memset(&advert_ability, 0, sizeof(soc_port_ability_t));

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
        ADVERT_ALL_COPPER(&advert_ability);
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->fiber;
        other_medium   = &pc->copper;
        ADVERT_ALL_FIBER(&advert_ability);
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
    change_medium->advert_ability.pause &= advert_ability.pause;
    change_medium->advert_ability.speed_half_duplex &= advert_ability.speed_half_duplex;
    change_medium->advert_ability.speed_full_duplex &= advert_ability.speed_full_duplex;
    change_medium->advert_ability.interface &= advert_ability.interface;
    change_medium->advert_ability.medium &= advert_ability.medium;
    change_medium->advert_ability.loopback &= advert_ability.loopback;
    change_medium->advert_ability.flags &= advert_ability.flags;

    if (medium_update) {
        /* The new configuration may cause medium change. Check
         * and update medium.
         */
        SOC_IF_ERROR_RETURN
            (_phy_54380_medium_change(unit, port, TRUE, medium));
    } else {
        active_medium = (PHY_COPPER_MODE(unit, port)) ?  
                            &pc->copper : &pc->fiber;
        if (active_medium == change_medium) {
            /* If the medium to update is active, update the configuration */
            SOC_IF_ERROR_RETURN
                (_phy_54380_medium_config_update(unit, port, change_medium));
        }
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54380_medium_config_get
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
phy_54380_medium_config_get(int unit, soc_port_t port, 
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
 *      _phy_54380_no_reset_setup
 * Purpose:
 *      Setup the operating parameters of the PHY without resetting PHY
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_54380_no_reset_setup(int unit, soc_port_t port)
{
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    int rv = SOC_E_NONE;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_54380_no_reset_setup: u=%d p=%d medium=%s\n"),
                         unit, port,
              PHY_COPPER_MODE(unit, port) ? "COPPER" : "FIBER"));

    pc      = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54380_init(phy_dev_addr, pc->macsec_dev_addr, 
                               0,
                               pc->automedium,
                               pc->fiber.preferred,
                               pc->fiber_detect,
                               pc->macsec_enable,
                               pc->fiber.enable,
                               pc->copper.enable);  

    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54380_init
 * Purpose:
 *      Init function for 54380 PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_54380_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    int        fiber_capable = 0, rv;
    int        fiber_preferred;
    int        qsgmii_mdio;
    bmacsec_dev_addr_t macsec_dev_addr, qsgmii_dev_addr, phy_dev_addr, phy_base_addr;
    int         mmi_mdio_addr, port_index, macsec_enable, macsec_fixed_latency_en;
    uint32      mac_config_flag = 0, primary_offset;

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

    mmi_mdio_addr = soc_property_port_get(unit, port, spn_MACSEC_DEV_ADDR, -1);

    if (mmi_mdio_addr == -1) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54380_init: "
                             "MACSEC_DEV_ADDR property "
                             "not configured for u=%d p=%d\n"), unit, port));
        return SOC_E_CONFIG;
    }
    
    port_index = soc_property_port_get(unit, port, spn_MACSEC_PORT_INDEX, -1);

    if (port_index == -1) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54380_init: "
                             "MACSEC_PORT_INDEX property not configured for "
                             "u=%d p=%d\n"), unit, port));
        return SOC_E_CONFIG;
    }

    /* EEE capable */ 
    if ( PHY_IS_BCM54380(pc) || PHY_IS_BCM54382(pc) ||
         PHY_IS_BCM54340(pc) || PHY_IS_BCM54385(pc) ) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_EEE_CAPABLE);
    }

    pc->macsec_dev_addr = mmi_mdio_addr;
    pc->macsec_dev_port = port_index;
    qsgmii_mdio = (pc->phy_id - pc->macsec_dev_port + 8);

    phy_dev_addr    = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    macsec_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->macsec_dev_addr, 0);
    qsgmii_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, qsgmii_mdio, 0); 
    phy_base_addr   = SOC_MACSECPHY_MDIO_ADDR(unit, ((pc->phy_id) - port_index), 0); 

    macsec_enable = soc_property_port_get(unit, port, spn_MACSEC_ENABLE, 0);
    pc->macsec_enable = macsec_enable;
    macsec_fixed_latency_en = soc_property_port_get(unit, port, spn_MACSEC_FIXED_LATENCY_ENABLE, -1);

    if(1 == macsec_fixed_latency_en) {
        mac_config_flag |= MAC_CTRL_FLAG_MACSEC_FIX_LATENCY_EN; 
    }
    rv = bmacsec_phy54380_phy_mac_addr_set(phy_dev_addr, macsec_dev_addr, 
                                   qsgmii_dev_addr, phy_base_addr,
                                   pc->macsec_dev_port, bmacsec_io_mmi1_quad,
                                   mac_config_flag); 
    if(rv != BMACSEC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54380_init: bmacsec_phy_mac_addr_set "
                             "returned error for u=%d p=%d\n"), unit, port));
        return SOC_E_FAIL;
    }

    /* fiber_capable? */
    if(PHY_IS_BCM54340(pc) || PHY_IS_BCM54385(pc)) {
        fiber_capable = 1;
    }

    /* Change it to copper prefer for default. */
    fiber_preferred =
        soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 0);
    pc->automedium =
        soc_property_port_get(unit, port, spn_PHY_AUTOMEDIUM, 0);
    pc->fiber_detect =
        soc_property_port_get(unit, port, spn_PHY_FIBER_DETECT, -4);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54380_init: "
                         "u=%d p=%d type=54380%s automedium=%d fiber_pref=%d detect=%d\n"),
              unit, port, fiber_capable ? "S" : "",
              pc->automedium, fiber_preferred, pc->fiber_detect));

    if (fiber_capable == 0) { /* not fiber capable */
        fiber_preferred  = 0;
        pc->automedium   = 0;
        pc->fiber_detect = 0;
    }

    pc->copper.enable = TRUE;
    pc->copper.preferred = !fiber_preferred;
    pc->copper.autoneg_enable = TRUE;
    ADVERT_ALL_COPPER(&pc->copper.advert_ability);
    pc->copper.force_speed = 1000;
    pc->copper.force_duplex = TRUE;
    pc->copper.master = SOC_PORT_MS_AUTO;
    pc->copper.mdix = SOC_PORT_MDIX_AUTO;

    pc->fiber.enable = fiber_capable;
    pc->fiber.preferred = fiber_preferred;
    pc->fiber.autoneg_enable = fiber_capable;
    ADVERT_ALL_FIBER(&pc->fiber.advert_ability);
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
    pc->ledctrl    = soc_property_port_get(unit, port, spn_PHY_LED_CTRL, 0x8);
    pc->ledselect  = soc_property_port_get(unit, port, spn_PHY_LED_SELECT, 0);

    /* default MACSEC MAC policy: MACSEC_SWITCH_FOLLOWS_LINE == 1 */
    pc->macsec_mac_policy = soc_property_port_get(unit, port,
                                          spn_MACSEC_SWITCH_SIDE_POLICY,
                                          BMACSEC_BCM54380_SWITCH_FOLLOWS_LINE);
    /* set Fixed policy if macsec_mac_policy == BMACSEC_SWITCH_FIXED == 0 */
    pc->macsec_switch_fixed = (pc->macsec_mac_policy ==
                               BMACSEC_BCM54380_SWITCH_FIXED) ? 1 : 0;
    /*
     * Some reasonable defaults
     */
    pc->macsec_switch_fixed_speed  = 1000;
    pc->macsec_switch_fixed_duplex = 1;
    pc->macsec_switch_fixed_pause  = 1;
    pc->macsec_pause_rx_fwd = 0;
    pc->macsec_pause_tx_fwd = 0;
    pc->macsec_line_ipg     = 0xc;
    pc->macsec_switch_ipg   = 0xc;

    /* Primary & Offset numbers from the config property */
    primary_offset = soc_property_port_get(unit, port, spn_PHY_PORT_PRIMARY_AND_OFFSET, 0x00);
    rv = soc_phyctrl_primary_set(unit, port, (primary_offset >>8) & 0xff);
    rv = soc_phyctrl_offset_set( unit, port,  primary_offset & 0xff);

    /* Init PHYS and MACs to defaults */
    rv = bmacsec_phy54380_init( phy_dev_addr, 
                                macsec_dev_addr, 
                                1,
                                pc->automedium,
                                fiber_preferred,
                                pc->fiber_detect,
                                pc->macsec_enable,
                                pc->fiber.enable,
                                pc->copper.enable); 
    
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    /* Set LED Modes and Control */
    rv = bmacsec_phy54380_set_led_selectors(phy_dev_addr, pc->ledmode[0],
                                            pc->ledmode[1], pc->ledmode[2],
                                            pc->ledmode[3], pc->ledselect,
                                            pc->ledctrl);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN
        (_phy_54380_medium_config_update(unit, port,
                        PHY_COPPER_MODE(unit, port) ? &pc->copper
                                                    : &pc->fiber));
    if (pc->macsec_enable) {
        rv = soc_macsecphy_init(unit, port, pc,
                            BMACSEC_CORE_BCM5438X, bmacsec_io_mmi1_quad);
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
    }
    return SOC_E_NONE;
}


/*
 * Function:    
 *  phy_54380_reset
 * Purpose: 
 *  Reset PHY and wait for it to come out of reset.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 * Returns: 
 *  SOC_E_XXX
     */

STATIC int
phy_54380_reset(int unit, soc_port_t port, void *user_arg)
{

    int rv;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54380_reset(phy_dev_addr);

    return (SOC_ERROR(rv));
}

#define  MACSEC_REG_PORT_SHIFT      12
#define  MACSEC_REG_PORT_MASK       (0xFU << MACSEC_REG_PORT_SHIFT)
#define  UINT16_MASK                0xFFFF

/*
 * Function:
 *      phy_54380_reg_read
 * Purpose:
 *      read a register.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - register type.
 *      phy_reg_addr - register address.
 *      phy_data - data read from the register.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_54380_reg_read(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 *phy_data)
{
    int  rv;

    if ( BMACSEC_PHY_REG_MACSEC & flags ) {    /* MACSEC registers */
        soc_port_t  offset;
        SOC_IF_ERROR_RETURN( soc_phyctrl_offset_get(unit, port, &offset) );

        phy_reg_addr &= ~(MACSEC_REG_PORT_MASK);
        phy_reg_addr |= (offset << MACSEC_REG_PORT_SHIFT);
        rv = bmacsec_phy54380_macsec_reg_read(port, flags, phy_reg_addr, phy_data);

    } else
    if ( BMACSEC_PHY_REG_DIRECT & flags ) {    /* RDB registers */
        uint16       data16 = 0;
        phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

        rv = bmacsec_phy54380_direct_reg_read(pc->phy_id,
                                (uint16) (phy_reg_addr & UINT16_MASK), &data16);
        *phy_data = (uint32) data16;

    } else {    /* other registers */
        rv = phy_ge_reg_read(unit, port, flags, phy_reg_addr, phy_data);
    }

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54380_reg_write
 * Purpose:
 *      write a register.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - register type.
 *      phy_reg_addr - register address.
 *      phy_data - data to be written.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_54380_reg_write(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data)
{
    int  rv;

    if ( BMACSEC_PHY_REG_MACSEC & flags ) {    /* MACSEC registers */
        soc_port_t  offset;
        SOC_IF_ERROR_RETURN( soc_phyctrl_offset_get(unit, port, &offset) );

        phy_reg_addr &= ~(MACSEC_REG_PORT_MASK);
        phy_reg_addr |= (offset << MACSEC_REG_PORT_SHIFT);
        rv = bmacsec_phy54380_macsec_reg_write(port, flags, phy_reg_addr, phy_data);

    } else
    if ( BMACSEC_PHY_REG_DIRECT & flags ) {    /* RDB registers */
        phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
        rv = bmacsec_phy54380_direct_reg_write(pc->phy_id,
                                      (uint16) (phy_reg_addr & UINT16_MASK),
                                      (uint16) (phy_data     & UINT16_MASK) );

    } else {    /* other registers */
        rv = phy_ge_reg_write(unit, port, flags, phy_reg_addr, phy_data);
    }

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54380_reg_modify
 * Purpose:
 *      modify a register.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - register type.
 *      phy_reg_addr - register address.
 *      phy_data - data to be modified.
 *      mask - register fields mask.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_54380_reg_modify(int unit, soc_port_t port, uint32 flags,
                     uint32 phy_reg_addr, uint32 phy_data, uint32 mask)
{
    int  rv;

    if ( BMACSEC_PHY_REG_MACSEC & flags ) {    /* MACSEC registers */
        soc_port_t  offset;
        SOC_IF_ERROR_RETURN( soc_phyctrl_offset_get(unit, port, &offset) );

        phy_reg_addr &= ~(MACSEC_REG_PORT_MASK);
        phy_reg_addr |= (offset << MACSEC_REG_PORT_SHIFT);
        rv = bmacsec_phy54380_macsec_reg_modify(port, flags, phy_reg_addr, phy_data, mask);

    } else
    if ( BMACSEC_PHY_REG_DIRECT & flags ) {    /* RDB registers */
        phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
        rv = bmacsec_phy54380_direct_reg_modify(pc->phy_id,
                                       (uint16) (phy_reg_addr & UINT16_MASK),
                                       (uint16) (phy_data     & UINT16_MASK),
                                       (uint16) mask );

    } else {    /* other registers */
        rv = phy_ge_reg_modify(unit, port, flags, phy_reg_addr, phy_data, mask);
    }

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54380_enable_set
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
phy_54380_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;
    int rv = SOC_E_NONE;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy_port_mode_t port_mode = 0;

    pc     = EXT_PHY_SW_STATE(unit, port);

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54380_enable_set(phy_dev_addr, pc->automedium,
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
                             "phy_54380_enable_set: "
                             "Power %s fiber medium\n"), (enable) ? "up" : "down"));
    }

    /* Update software state */
    SOC_IF_ERROR_RETURN(phy_fe_ge_enable_set(unit, port, enable));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54380_enable_get
 * Purpose:
 *      Enable or disable the physical interface for a 54380 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54380_enable_get(int unit, soc_port_t port, int *enable)
{
    return phy_fe_ge_enable_get(unit, port, enable);
}


/*
 * Function:
 *      _phy_54380_medium_change
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
_phy_54380_medium_change(int unit, soc_port_t port, int force_update,
                         soc_port_medium_t force_medium)
{
    phy_ctrl_t    *pc;
    int            medium;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_54380_medium_check(unit, port, &medium));

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        if (!PHY_COPPER_MODE(unit, port) || force_update) { /* Was fiber */ 
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
            if ( PHY_IS_BCM54380(pc) || PHY_IS_BCM54382(pc) ||
                 PHY_IS_BCM54340(pc) || PHY_IS_BCM54385(pc) ) {
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_EEE_CAPABLE);
            }
            SOC_IF_ERROR_RETURN
                (_phy_54380_no_reset_setup(unit, port));

            /* Do Not power up the interface if medium is disabled */
            if (pc->copper.enable) {
                SOC_IF_ERROR_RETURN
                    (_phy_54380_medium_config_update(unit, port, &pc->copper));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_54380_link_auto_detect: u=%d p=%d [F->C]\n"),
                      unit, port));
        }
    } else {        /* Fiber */
        if (PHY_COPPER_MODE(unit, port) || force_update) { /* Was copper */
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_EEE_CAPABLE); /* no EEE for fiber */
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);

            SOC_IF_ERROR_RETURN
                (_phy_54380_no_reset_setup(unit, port));

            if (pc->fiber.enable) {
                SOC_IF_ERROR_RETURN
                    (_phy_54380_medium_config_update(unit, port, &pc->fiber));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_54380_link_auto_detect: u=%d p=%d [C->F]\n"),
                      unit, port));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54380_link_get
 * Purpose:
 *      Determine the current link up/down status for a 54380 device.
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
phy_54380_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t    *pc;
    uint16         data; 
    int rv = SOC_E_NONE;
    bmacsec_phy_port_mode_t port_mode = 0;
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
            (_phy_54380_medium_change(unit, port, FALSE, 0));
    }

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy_54380_link_get(phy_dev_addr, port_mode, &bmacsec_link);
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
            (void)bmacsec_phy54380_enable_set(phy_dev_addr, pc->automedium,
                                                 port_mode, data);
        } else {  /* pc->fiber.preferred */
            if (pc->copper.enable) {
                data = (*link && PHY_FIBER_MODE(unit, port)) ? 0 : 1;
            } else {
                data = 0;
            }
            port_mode = BMACSEC_PHY_COPPER;
            (void)bmacsec_phy54380_enable_set(phy_dev_addr, pc->automedium,
                                             port_mode, data);
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_54380_link_get: u=%d p=%d mode=%s%s link=%d\n"),
                 unit, port,
                 PHY_COPPER_MODE(unit, port) ? "C" : "F",
                 PHY_FIBER_100FX_MODE(unit, port)? "(100FX)" : "",
                 *link));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54380_duplex_set
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
phy_54380_duplex_set(int unit, soc_port_t port, int duplex)
{
    int                          rv=SOC_E_NONE;
    phy_ctrl_t                   *pc;
    bmacsec_phy_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy_duplex_t bmacsec_duplex;

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
    rv = bmacsec_phy54380_duplex_set(phy_dev_addr, port_mode, 
                                     bmacsec_duplex);

    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_FIBER_MODE(unit, port)) {
            pc->fiber.force_duplex = duplex;
        } else {
            pc->copper.force_duplex = duplex;
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54380_duplex_set: u=%d p=%d d=%d rv=%d\n"),
              unit, port, duplex, rv));
    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_54380_duplex_get
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
phy_54380_duplex_get(int unit, soc_port_t port, int *duplex)
{

    int rv = SOC_E_NONE;
    bmacsec_phy_port_mode_t port_mode = 0;
    phy_ctrl_t             *pc;
    bmacsec_dev_addr_t      phy_dev_addr;
    bmacsec_phy_duplex_t    bmacsec_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        port_mode = BMACSEC_PHY_COPPER;
    } else
    if ( PHY_FIBER_MODE(unit, port) ) {
        port_mode = BMACSEC_PHY_FIBER;
        *duplex = TRUE;
        return SOC_E_NONE;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54380_duplex_get(phy_dev_addr, port_mode, 
                                     &bmacsec_duplex);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    *duplex = (bmacsec_duplex == BMACSEC_FULL_DUPLEX) ? TRUE : FALSE;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54380_speed_set
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
phy_54380_speed_set(int unit, soc_port_t port, int speed)
{
    int            rv = SOC_E_NONE; 
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_phy_port_mode_t port_mode = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

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
    rv = bmacsec_phy54380_speed_set(phy_dev_addr, port_mode, speed);

    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.force_speed = speed;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                if (speed == 100) {
                        PHY_FLAGS_SET(unit, port, PHY_FLAGS_100FX);
                        pc->fiber.autoneg_enable = FALSE;
                        phy_54380_autoneg_set(unit, port, 0);
                }

                if (speed == 1000) {
                        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
                        pc->fiber.force_duplex = TRUE;
                        phy_54380_autoneg_set(unit, port,
                                              pc->fiber.autoneg_enable);
                }
                pc->fiber.force_speed = speed;
            }
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54380_speed_set: u=%d p=%d s=%d fiber=%d rv=%d\n"),
              unit, port, speed, PHY_FIBER_MODE(unit, port), rv));
    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54380_speed_get
 * Purpose:
 *      Get the current operating speed for a 54380 device.
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
phy_54380_speed_get(int unit, soc_port_t port, int *speed)
{
    int rv = SOC_E_NONE;
    bmacsec_phy_port_mode_t port_mode = 0;
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
    rv = bmacsec_phy54380_speed_get(phy_dev_addr, port_mode, speed);

    if(!BMACSEC_E_SUCCESS(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54380_speed_get: u=%d p=%d invalid speed\n"),
                  unit, port));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54380_speed_get: u=%d p=%d speed=%d"),
                  unit, port, *speed));
    }

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54380_master_set
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
phy_54380_master_set(int unit, soc_port_t port, int master)
{
    int                  rv = SOC_E_NONE;
    phy_ctrl_t    *pc;
    bmacsec_phy_port_mode_t port_mode = 0;
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
    rv = bmacsec_phy54380_master_set(phy_dev_addr, port_mode, master);
    if(BMACSEC_E_SUCCESS(rv)) {
        pc->copper.master = master;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54380_master_set: u=%d p=%d master=%d fiber=%d rv=%d\n"),
              unit, port, master, PHY_FIBER_MODE(unit, port), rv));
    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54380_master_get
 * Purpose:
 *      Get the current master mode for a 54380 device.
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
phy_54380_master_get(int unit, soc_port_t port, int *master)
{
    int        rv;
    phy_ctrl_t *pc;
    bmacsec_phy_port_mode_t port_mode = 0;
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
    rv = bmacsec_phy54380_master_get(phy_dev_addr, port_mode, master);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54380_autoneg_set
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
phy_54380_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    int                 rv = SOC_E_NONE;
    phy_ctrl_t   *pc;
    bmacsec_port_ability_t *autoneg_advert = 0;
    bmacsec_phy_port_mode_t port_mode = 0;
    bmacsec_dev_addr_t phy_dev_addr;


    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
        autoneg_advert = (bmacsec_port_ability_t *)&pc->copper.advert_ability;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
            autoneg_advert = (bmacsec_port_ability_t *)&pc->fiber.advert_ability;
        }
    }
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54380_an_set(phy_dev_addr, port_mode,
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
                         "phy_54380_autoneg_set: u=%d p=%d autoneg=%d rv=%d\n"),
              unit, port, autoneg, rv));
    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54380_autoneg_get
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
phy_54380_autoneg_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done)
{
    int           rv;
    phy_ctrl_t   *pc;
    bmacsec_phy_port_mode_t port_mode = 0;
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
    rv = bmacsec_phy54380_an_get(phy_dev_addr, port_mode, autoneg,
                                 autoneg_done);

    return SOC_ERROR(rv);
}

/*
 * Function:
 *      phy_54380_ability_advert_set
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
phy_54380_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int rv = SOC_E_NONE;
    phy_ctrl_t    *pc;
    bmacsec_phy_port_mode_t port_mode = 0;
    bmacsec_port_ability_t bmacsec_ability ;
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
    
    
    sal_memset(&bmacsec_ability, 0,  sizeof(bmacsec_port_ability_t)); 
    phy_54380_pa_2_bmacsec_pa(ability, &bmacsec_ability);

    rv = bmacsec_phy54380_ability_advert_set(phy_dev_addr, port_mode, &bmacsec_ability);
    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.advert_ability = *ability;
        } else {
            if(PHY_FIBER_MODE(unit, port)) {
                pc->fiber.advert_ability = *ability;
        } else {
            }
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54380_adv_local_set: u=%d p=%d ability_hd_speed=0x%x, ability_fd_speed=0x%x, ability_pause=0x%x,  rv=%d\n"),
              unit, port, ability->speed_half_duplex, ability->speed_full_duplex, ability->pause, rv));
    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_54380_adv_local_get
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
phy_54380_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{

    int rv = SOC_E_NONE;
    bmacsec_port_ability_t bmacsec_ability;
    bmacsec_phy_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    phy_ctrl_t   *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }
    sal_memset(ability, 0,  sizeof(soc_port_ability_t)); /* zero initialize */
    
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
    
    rv = bmacsec_phy54380_ability_advert_get(phy_dev_addr, port_mode, 
                                        &bmacsec_ability);
    
    phy_54380_bmacsec_pa_2_pa( &bmacsec_ability, ability);
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54380_adv_local_get: u=%d p=%d ability_hd_speed=0x%x, ability_fd_speed=0x%x, ability_pause=0x%x,  rv=%d\n"),
              unit, port, ability->speed_half_duplex, ability->speed_full_duplex, ability->pause, rv));

    return  (SOC_ERROR(rv));

}

/*
 * Function:
 *      phy_54380_ability_remote_get
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
phy_54380_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int rv, eee_enable = FALSE;
    phy_ctrl_t    *pc;
    bmacsec_phy_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_port_ability_t bmacsec_ability;
    bmacsec_dev_addr_t phy_dev_addr;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(ability, 0, sizeof(soc_port_ability_t)); /*zero init*/

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
        }
    }
    
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
        eee_enable = TRUE;
    }

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
    rv = bmacsec_phy54380_ability_remote_get(phy_dev_addr, port_mode, 
                                         &bmacsec_ability, eee_enable);

    phy_54380_bmacsec_pa_2_pa( &bmacsec_ability, ability);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54380_adv_remote_get: u=%d p=%d ability_hd_speed=0x%x ability_fd_speed=0x%x ability_pause=0x%x rv=%d\n"), unit, port, ability->speed_half_duplex, ability->speed_full_duplex, ability->pause, rv));

    return  (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_54380_lb_set
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
phy_54380_lb_set(int unit, soc_port_t port, int enable)
{
    int           rv = SOC_E_NONE;
    phy_ctrl_t    *pc;
    bmacsec_phy_port_mode_t port_mode = BMACSEC_PHY_COPPER;
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
    rv = bmacsec_phy54380_loopback_set(phy_dev_addr, port_mode, enable);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54380_lb_set: u=%d p=%d en=%d rv=%d\n"), 
              unit, port, enable, rv));
    return SOC_ERROR(rv); 
}

/*
 * Function:
 *      phy_54380_lb_get
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
phy_54380_lb_get(int unit, soc_port_t port, int *enable)
{
    int                  rv;
    phy_ctrl_t    *pc;
    bmacsec_phy_port_mode_t port_mode = BMACSEC_PHY_COPPER;
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
    rv = bmacsec_phy54380_loopback_get(phy_dev_addr, port_mode, enable);

    return SOC_ERROR(rv); 
}

/*
 * Function:
 *      phy_54380_interface_set
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
phy_54380_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
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
 *      phy_54380_interface_get
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
phy_54380_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_SGMII;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54380_ability_local_get
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
phy_54380_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int         rv;
    phy_ctrl_t *pc;
    bmacsec_phy_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;
    bmacsec_port_ability_t bmacsec_ability;

    rv = SOC_E_NONE;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(ability, 0,  sizeof(soc_port_ability_t)); /* zero initialize */

    sal_memset(&bmacsec_ability, 0,  sizeof(bmacsec_port_ability_t)); /* zero initialize */
    
    if(PHY_COPPER_MODE(unit, port)) {
        port_mode = BMACSEC_PHY_COPPER;
        ability->medium = SOC_PA_MEDIUM_COPPER;
    } else {
        if(PHY_FIBER_MODE(unit, port)) {
            port_mode = BMACSEC_PHY_FIBER;
            ability->medium = SOC_PA_MEDIUM_FIBER;
        }
    }

    ability->interface = SOC_PA_INTF_SGMII;
    
    pc = EXT_PHY_SW_STATE(unit, port);

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0);
    
    rv = bmacsec_phy54380_ability_get(phy_dev_addr, port_mode, &bmacsec_ability);
    if (rv != BMACSEC_E_NONE) {
        return SOC_E_FAIL;
    }

    if(bmacsec_ability.speed_full_duplex & BMACSEC_PA_SPEED_1000MB) {
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(bmacsec_ability.speed_full_duplex & BMACSEC_PA_SPEED_100MB) {
        ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }
    if(bmacsec_ability.speed_full_duplex & BMACSEC_PA_SPEED_10MB) {
        ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
    }
    if(bmacsec_ability.speed_half_duplex & BMACSEC_PA_SPEED_1000MB) {
        ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
    }
    if(bmacsec_ability.speed_half_duplex & BMACSEC_PA_SPEED_100MB) {
        ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
    }
    if(bmacsec_ability.speed_half_duplex & BMACSEC_PA_SPEED_10MB) {
        ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
    }
    if(bmacsec_ability.pause & BMACSEC_PA_PAUSE_TX) {
        ability->pause |= SOC_PA_PAUSE_TX;
    }
    if(bmacsec_ability.pause & BMACSEC_PA_PAUSE_RX) {
        ability->pause |= SOC_PA_PAUSE_RX;
    }
    if(bmacsec_ability.pause & BMACSEC_PA_PAUSE_ASYMM) {
        ability->pause |= SOC_PA_PAUSE_ASYMM;
    }
    if(bmacsec_ability.loopback & BMACSEC_PA_LB_NONE) {
        ability->loopback |= SOC_PA_LB_NONE;
    }
    if(bmacsec_ability.loopback & BMACSEC_PA_LB_PHY) {
        ability->loopback |= SOC_PA_LB_PHY;
    }
    if(bmacsec_ability.flags & BMACSEC_PA_AUTONEG) {
        ability->flags |= SOC_PA_AUTONEG;
    }
    if(bmacsec_ability.eee & BMACSEC_PA_EEE_1GB_BASET) {
        ability->eee |= SOC_PA_EEE_1GB_BASET;
    }
    if(bmacsec_ability.eee & BMACSEC_PA_EEE_100MB_BASETX) {
        ability->eee |= SOC_PA_EEE_100MB_BASETX;
    }
 
    if (pc->automedium) {
        ability->flags     |= SOC_PA_COMBO;
    }


    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54380_ability_local_get: u=%d p=%d ability_hd_speed=0x%x, ability_fd_speed=0x%x, ability_pause=0x%x,  rv=%d\n"),
              unit, port, ability->speed_half_duplex, ability->speed_full_duplex, ability->pause, rv));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54380_mdix_set
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
phy_54380_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{

    int         rv = SOC_E_NONE;
    phy_ctrl_t  *pc;
    bmacsec_phy_port_mode_t port_mode = BMACSEC_PHY_COPPER;
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

    rv = bmacsec_phy54380_mdix_set(phy_dev_addr, port_mode, bmacsec_mode);

    if(BMACSEC_E_SUCCESS(rv)) {
        if(PHY_COPPER_MODE(unit, port)) {
            pc->copper.mdix = mode;
        }
    }
    return SOC_ERROR(rv);
}        

/*
 * Function:
 *      phy_54380_mdix_get
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
phy_54380_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    if (mode == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(phy_54380_speed_get(unit, port, &speed));
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
 *      phy_54380_mdix_status_get
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
phy_54380_mdix_status_get(int unit, soc_port_t port, 
                         soc_port_mdix_status_t *status)
{
    int         rv;
    phy_ctrl_t    *pc;

    bmacsec_phy_port_mode_t port_mode = BMACSEC_PHY_COPPER;
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
    rv = bmacsec_phy54380_mdix_status_get(phy_dev_addr, port_mode, 
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
 *      phy_54380_timesync_config_set
 * Description:
 *      
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */

int
phy_54380_timesync_config_set(int unit, soc_port_t port, soc_port_phy_timesync_config_t *timesync_config)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 

    rv = bmacsec_phy54380_timesync_config_set(phy_dev_addr, (bmacsec_port_phy_timesync_config_t *)timesync_config);
    return (SOC_ERROR(rv));

}


/*
 * Function:
 *     phy_54380_timesync_config_get 
 * Description:
 *     
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */

int
phy_54380_timesync_config_get(int unit, soc_port_t port, soc_port_phy_timesync_config_t *timesync_config)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
 
    rv = bmacsec_phy54380_timesync_config_get(phy_dev_addr, (bmacsec_port_phy_timesync_config_t *)timesync_config);
    return (SOC_ERROR(rv));

}


/*
 * Function:
 *      phy_54380_timesync_control_set
 * Description:
 *      
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */

int
phy_54380_timesync_control_set(int unit, soc_port_t port, soc_port_control_phy_timesync_t control_type, uint64 value)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    buint64_t btemp64;

    COMPILER_64_SET(btemp64, COMPILER_64_HI(value), COMPILER_64_LO(value)); 

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
 
    rv = bmacsec_phy54380_timesync_control_set(phy_dev_addr,
                (bmacsec_port_phy_control_timesync_t) control_type, btemp64);
    return (SOC_ERROR(rv));

}



/*
 * Function:
 *      phy_54380_timesync_control_get
 * Description:
 *      
 * Parameters:
 *
 * Return Value:
 *      SOC_E_XXX
 */
int
phy_54380_timesync_control_get(int unit, soc_port_t port, soc_port_control_phy_timesync_t control_type, uint64 *value)
{
    int                          rv;
    phy_ctrl_t                   *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    buint64_t btemp64;

    COMPILER_64_ZERO(btemp64);
    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0); 
 
    rv = bmacsec_phy54380_timesync_control_get(phy_dev_addr,
                (bmacsec_port_phy_control_timesync_t) control_type, &btemp64);
  
    COMPILER_64_SET(*value, COMPILER_64_HI(btemp64), COMPILER_64_LO(btemp64));

    return (SOC_ERROR(rv));

}

/*
 * Function:
 *      phy_54380_cable_diag_dispatch
 * Purpose:
 *      Run 54380 cable diagnostics
 * Parameters:
 *      unit - device number
 *      port - port number
 *      status - (OUT) cable diagnotic status structure
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_54380_cable_diag_dispatch(int unit, soc_port_t port,
            soc_port_cable_diag_t *status)
{
    /*ECD_CTRL :break link */
    uint16 ecd_ctrl = 0x1000;
    
    
    SOC_IF_ERROR_RETURN(
        phy_ecd_cable_diag_init_40nm(unit, port));

    SOC_IF_ERROR_RETURN(
        phy_ecd_cable_diag_40nm(unit, port, status, ecd_ctrl));
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54380_control_set
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
phy_54380_control_set(int unit, soc_port_t port,
                      soc_phy_control_t type, uint32 value)
{
    int rv, offset;
    phy_ctrl_t    *pc;
    bmacsec_phy_port_mode_t port_mode = BMACSEC_PHY_COPPER;
    bmacsec_dev_addr_t phy_dev_addr;
    soc_phy_config_t phy_config;

    pc = EXT_PHY_SW_STATE(unit, port);

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

    /* PHY Control types APIs*/
    switch ( type ) {
#if 0
    case SOC_PHY_CONTROL_MACSEC_SWITCH_FIXED:
        pc->macsec_switch_fixed = value;
        if ( value ) {
            rv = bmacsec_phy54380_switch_side_set_params(phy_dev_addr, 
                            BMACSEC_BCM54380_SWITCH_FIXED, 
                            pc->macsec_switch_fixed_speed,
                            pc->macsec_switch_fixed_duplex,
                            pc->macsec_switch_fixed_pause);
        } else { /* Switch side follows line side */
            rv = bmacsec_phy54380_switch_side_set_params(phy_dev_addr, 
                            BMACSEC_BCM54380_SWITCH_FOLLOWS_LINE, 
                            -1, -1, -1);
        }
        break;
#endif
    /* CLOCK CONTROLS  */
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        bmacsec_phy54380_clock_enable_set(phy_dev_addr, offset, value);
        break;

    case SOC_PHY_CONTROL_CLOCK_SECONDARY_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        bmacsec_phy54380_clock_secondary_enable_set(phy_dev_addr, offset, value);
        break;

    case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
        return SOC_E_UNAVAIL;
        break;

    case SOC_PHY_CONTROL_POWER:
        rv = bmacsec_phy54380_power_mode_set(phy_dev_addr, value);
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
        bmacsec_phy54380_power_auto_wake_time_set(phy_dev_addr, value);
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
        bmacsec_phy54380_power_auto_sleep_time_set(phy_dev_addr, value);
        break;

    case SOC_PHY_CONTROL_PORT_PRIMARY:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_primary_set(unit, port, (soc_port_t)value));
        break;

    case SOC_PHY_CONTROL_PORT_OFFSET:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_set(unit, port, (int)value));
        break;

    case SOC_PHY_CONTROL_EEE:
        phy_config = pc->copper;
        if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        if ( PHY_FIBER_MODE(unit, port) ) {
            phy_config = pc->fiber;
            /* PHY does not support EEE in fiber mode */
            rv = SOC_E_UNAVAIL;
            break;
        }
        /* enable/disable EEE */
        rv = bmacsec_phy_54380_eee_enable(phy_dev_addr, port_mode,
                          (bmacsec_port_ability_t*) &phy_config.advert_ability,
                          (value) ? 1 : 0 );
        break;

    case SOC_PHY_CONTROL_EEE_AUTO:
        if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        if ( PHY_FIBER_MODE(unit, port) ) {
            phy_config = pc->fiber;
            /* PHY does not support EEE in fiber mode */
            rv = SOC_E_UNAVAIL;
            break;
        }
        /* enable/disable AutoGrEEEn */
        rv = bmacsec_phy_54380_eee_auto_enable(phy_dev_addr,
                          (bmacsec_port_t) port, port_mode,
                          (value) ? 1 : 0 );
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        SOC_IF_ERROR_RETURN
                (bmacsec_phy_54380_eee_control_set(phy_dev_addr, 
                               BMACSEC_PHY_54380_CONTROL_EEE_AUTO_IDLE_THRESHOLD,
                               value));
        break;       

    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        SOC_IF_ERROR_RETURN
                (bmacsec_phy_54380_eee_control_set(phy_dev_addr, 
                               BMACSEC_PHY_54380_CONTROL_EEE_AUTO_FIXED_LATENCY, 
                               value));
        break;       
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:

        if(PHY_COPPER_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN(
               bmacsec_phy54380_remote_loopback_set(phy_dev_addr, port_mode, value));
        }else{
            return SOC_E_UNAVAIL;

        }
        break;    
    case SOC_PHY_CONTROL_EEE_AUTO_BUFFER_LIMIT:
        /* Buffer limit modification is not allowed */
    case SOC_PHY_CONTROL_EEE_TRANSMIT_WAKE_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_WAKE_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_SLEEP_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_SLEEP_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_QUIET_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_QUIET_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_REFRESH_TIME:
        return SOC_E_UNAVAIL;

    case SOC_PHY_CONTROL_EEE_STATISTICS_CLEAR:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        SOC_IF_ERROR_RETURN
                (bmacsec_phy_54380_eee_control_set(phy_dev_addr, 
                               BMACSEC_PHY_54380_CONTROL_EEE_STATISTICS_CLEAR, 
                               value));
        break;

    case SOC_PHY_CONTROL_SUPER_ISOLATE:
        rv = bmacsec_phy54380_super_isolate_set(phy_dev_addr, value);
        break;

    case SOC_PHY_CONTROL_MACSEC_ENABLE:
        rv = bmacsec_phy54380_macsec_enable(phy_dev_addr,
                                        pc->macsec_dev_addr, pc->macsec_enable);
        if ( rv != BMACSEC_E_NONE ) {
            return  SOC_E_FAIL;
        }
        if ( pc->macsec_enable ) {
            rv = soc_macsecphy_init(unit, port, pc,
                                    BMACSEC_CORE_BCM5438X, bmacsec_io_mmi1_quad);
            if ( SOC_SUCCESS(rv) ) {
                LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                         "MACSEC init for u=%d p=%d SUCCESS\n"), unit, port));
            } else {
                LOG_ERROR(BSL_ERROR    , (BSL_META_U(unit,
                         "MACSEC init for u=%d p=%d FAIL !!\n"), unit, port));
            }
        }
        break;

    default:
        return  SOC_E_UNAVAIL;
    }  /* switch ( type ) */

    return (SOC_ERROR(rv));
}

/*
 * Function:
 *      phy_54380_control_get
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
phy_54380_control_get(int unit, soc_port_t port,
                      soc_phy_control_t type, uint32 *value)
{
    int rv = SOC_E_NONE, offset;
    phy_ctrl_t    *pc;
    bmacsec_dev_addr_t phy_dev_addr;
    soc_port_t primary;
    bmacsec_phy_port_mode_t port_mode = BMACSEC_PHY_COPPER;

    pc = EXT_PHY_SW_STATE(unit, port);
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
   
    if (NULL == value) {
        return SOC_E_PARAM;
    }

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    /* PHY Control types APIs*/
    switch ( type ) {

    case SOC_PHY_CONTROL_POWER:
        *value = pc->power_mode;
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
        bmacsec_phy54380_power_auto_sleep_time_get(phy_dev_addr, value); 
        break;
    
    case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
        bmacsec_phy54380_power_auto_wake_time_get(phy_dev_addr, value); 
        break;

    case SOC_PHY_CONTROL_PORT_PRIMARY:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_primary_get(unit, port, &primary));
        *value = (uint32) primary;
        break;

    case SOC_PHY_CONTROL_PORT_OFFSET:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        *value = (uint32) offset;
        break;

    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        bmacsec_phy54380_clock_enable_get(phy_dev_addr, offset, value);
        break;

    case SOC_PHY_CONTROL_CLOCK_SECONDARY_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        bmacsec_phy54380_clock_secondary_enable_get(phy_dev_addr, offset, value); 
        break;

    case SOC_PHY_CONTROL_EEE:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        bmacsec_phy_54380_get_eee_control_status(phy_dev_addr, value);
        break;
    
    case SOC_PHY_CONTROL_EEE_AUTO:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        bmacsec_phy_54380_get_auto_eee_control_status(phy_dev_addr,
                               (bmacsec_port_t)port, value);
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        SOC_IF_ERROR_RETURN
                (bmacsec_phy_54380_eee_control_get(phy_dev_addr,
                               BMACSEC_PHY_54380_CONTROL_EEE_AUTO_FIXED_LATENCY,  
                               value));
            break;

    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        SOC_IF_ERROR_RETURN
                (bmacsec_phy_54380_eee_control_get(phy_dev_addr,
                               BMACSEC_PHY_54380_CONTROL_EEE_AUTO_IDLE_THRESHOLD,  
                               value));
        break;
    
    case SOC_PHY_CONTROL_EEE_TRANSMIT_EVENTS:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        SOC_IF_ERROR_RETURN
                (bmacsec_phy_54380_eee_control_get(phy_dev_addr,
                               BMACSEC_PHY_54380_CONTROL_EEE_TRANSMIT_EVENTS,  
                               value));
        break;
    
    case SOC_PHY_CONTROL_EEE_TRANSMIT_DURATION:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        SOC_IF_ERROR_RETURN
                (bmacsec_phy_54380_eee_control_get(phy_dev_addr,
                               BMACSEC_PHY_54380_CONTROL_EEE_TRANSMIT_DURATION,  
                               value));
        break;

    case SOC_PHY_CONTROL_EEE_RECEIVE_EVENTS:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        SOC_IF_ERROR_RETURN
                (bmacsec_phy_54380_eee_control_get(phy_dev_addr,
                               BMACSEC_PHY_54380_CONTROL_EEE_RECEIVE_EVENTS,  
                               value));
        break;

    case SOC_PHY_CONTROL_EEE_RECEIVE_DURATION:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        SOC_IF_ERROR_RETURN
                (bmacsec_phy_54380_eee_control_get(phy_dev_addr,
                               BMACSEC_PHY_54380_CONTROL_EEE_RECEIVE_DURATION,  
                               value));
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:

        if(PHY_COPPER_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN(
               bmacsec_phy54380_remote_loopback_get(phy_dev_addr, port_mode, value));
        }else{
            return SOC_E_UNAVAIL;

        }
        break;    

    default:
        return SOC_E_UNAVAIL;    
    }  /* switch ( type ) */
    
    return rv;
}

STATIC int 
phy_54380_probe(int unit, phy_ctrl_t *pc) {
    int rv = SOC_E_NONE, oui = 0, model = 0, rev = 0;
    soc_phy_info_t *pi;
    bmacsec_dev_addr_t phy_dev_addr;

    phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0);

    phy54380_get_model_rev(phy_dev_addr, &oui, &model, &rev);

    pi = &SOC_PHY_INFO(unit, pc->port);

    switch (model) {
    
        case PHY_BCM54380_MODEL:
        case PHY_BCM54340_MODEL:
        break;

        case PHY_BCM54382_MODEL:
        /*case PHY_BCM54385_MODEL  (same as PHY_BCM54382_MODEL)
         */
            if (rev & 0x8)  {
                /* 54382 detected  - A0/B0/.. */
                pi->phy_name = "BCM54382";
            } else {
                /* 54385 detected */
                pi->phy_name = "BCM54385";
            }
            break;
        default:
            return SOC_E_NOT_FOUND;
    }
    return rv;
}


STATIC int
phy_54380_link_up(int unit, soc_port_t port)
{
    int speed;
    phy_ctrl_t    *pc;
    pc = EXT_PHY_SW_STATE(unit, port);
    
    /* get the speed status from line side */
    SOC_IF_ERROR_RETURN
        (phy_54380_speed_get(unit, port, &speed));
    
    /* Notify interface to internal PHY */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventInterface, SOC_PORT_IF_SGMII));

    /* Situation observed with Saber devices XGXS16g1l+BCM54640E
       Linkscan is not updating speed for internal phy xgxs16g1l. 
       It was observed that when speed is set to 100M the int phy 
       is still set to 1G and traffic does not flow through untill 
       int phy speed is also update to 100M. This applies to other 
       speeds as well. 
       Explicitly send speed notify to int phy from ext phy.
    */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventSpeed, speed));

    /* Check if MACSEC_SWITCH_SIDE_POLICY is DUPLEX_GATEWAY mode */
    if ( (pc->macsec_enable) &&
         (pc->macsec_mac_policy == BMACSEC_BCM54380_SWITCH_DUPLEX_GATEWAY) ) {
        int duplex;
        bmacsec_dev_addr_t phy_dev_addr;
        phy_dev_addr = SOC_MACSECPHY_MDIO_ADDR(unit, pc->phy_id, 0);

        /* Duplex Gateway mode for supporting half-duplex-free switches:   *\
        |* When line-side PHY lilnks up at half-duplex,                    *|
        \* set half-duplex to line-side MAC, full-duplex to switch-side MAC*/

        /* get the duplex status from line side */
        SOC_IF_ERROR_RETURN(
            phy_54380_duplex_get(unit, port, &duplex) );
        if ( pc->macsec_switch_fixed_speed  == speed  &&
             pc->macsec_switch_fixed_duplex == duplex) {
            return SOC_E_NONE;    /* speed/duplex unchanged */
        }

        pc->macsec_switch_fixed_speed  = speed;
        pc->macsec_switch_fixed_duplex = duplex;
        /* Config duplex Gateway parameters accordingly */
        SOC_IF_ERROR_RETURN(
                bmacsec_phy54380_switch_side_set_params(phy_dev_addr,
                              (bmacsec_phy_mac_policy_t) pc->macsec_mac_policy,
                              speed, duplex, pc->macsec_switch_fixed_pause) );
        /* under DUPLEX_GATEWAY mode, always Full-Duplex on system side */
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventDuplex, TRUE));
    }

    return SOC_E_NONE;
}

/*
 * Variable:    phy_54380drv_ge
 * Purpose:     PHY driver for 54380
 */
phy_driver_t phy_54380drv_ge = {
    "54380 (MACSec PHY) Gigabit PHY Driver",
    phy_54380_init,
    phy_54380_reset,
    phy_54380_link_get,
    phy_54380_enable_set,
    phy_54380_enable_get,
    phy_54380_duplex_set,
    phy_54380_duplex_get,
    phy_54380_speed_set,
    phy_54380_speed_get,
    phy_54380_master_set,
    phy_54380_master_get,
    phy_54380_autoneg_set,
    phy_54380_autoneg_get,
    NULL,                       /* pd_adv_local_set */
    NULL,                       /* pd_adv_local_get */
    NULL,                       /* pd_adv_remote_get */
    phy_54380_lb_set,
    phy_54380_lb_get,
    phy_54380_interface_set,
    phy_54380_interface_get,
    NULL,                       /* pd_ability */
    phy_54380_link_up,          /* pd_linkup */
    NULL,                       /* pd_linkdn_evt */
    phy_54380_mdix_set,
    phy_54380_mdix_get,
    phy_54380_mdix_status_get,
    phy_54380_medium_config_set,
    phy_54380_medium_config_get,
    phy_54380_medium_status,
    phy_54380_cable_diag_dispatch,
    NULL,                       /* pd_link_change */
    phy_54380_control_set,
    phy_54380_control_get,
    phy_54380_reg_read,
    phy_54380_reg_write,
    phy_54380_reg_modify,
    NULL,                       /* pd_notify */
    phy_54380_probe,
    phy_54380_ability_advert_set,
    phy_54380_ability_advert_get,
    phy_54380_ability_remote_get,
    phy_54380_ability_local_get,
    NULL,                        /* pd_firmware_set */
    phy_54380_timesync_config_set,
    phy_54380_timesync_config_get,
    phy_54380_timesync_control_set,
    phy_54380_timesync_control_get
};

#endif /* INCLUDE_MACSEC */
#else /* INCLUDE_PHY_54380 */
typedef int _soc_phy_54380_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_54380 */


