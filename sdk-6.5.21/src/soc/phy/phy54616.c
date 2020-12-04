/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy54616.c
 * Purpose:     PHY driver for BCM54616 and BCM54616S
 */  
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_54616) || defined(INCLUDE_PHY_54618SE) || defined(INCLUDE_PHY_54618E)
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

#include "phyident.h"
#include "phyreg.h"
#include "phyfege.h"
#include "phy54616.h"

#define AUTO_MDIX_WHEN_AN_DIS   0       /* Non-standard-compliant option */
#define DISABLE_CLK125          0       /* Disable if unused to reduce */
                                        /*  EMI emissions */

#define PHY_IS_BCM54616S(_pc)  PHY_FLAGS_TST((_pc)->unit, (_pc)->port, \
                                              PHY_FLAGS_SECONDARY_SERDES)
#define PHY_IS_BCM54618E(_pc)  PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54618E_OUI, \
                                                PHY_BCM54618E_MODEL)
#define PHY_IS_BCM54618SE(_pc) PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54618SE_OUI, \
                                                PHY_BCM54618SE_MODEL)
#define PHY_IS_BCM54618SE_OR_BCM54618E(_pc) \
                (PHY_IS_BCM54618SE((_pc)) || PHY_IS_BCM54618E((_pc)))

#define SOC_IF_ERROR_BREAK(op, __rv__)           \
                if ( ((__rv__) = (op)) < 0 ) {   \
                    break;                       \
                }

STATIC int _phy_54616_no_reset_setup(int unit, soc_port_t port);
STATIC int _phy_54616_medium_change(int unit, soc_port_t port,int force_update);
STATIC int _phy_54616_copper_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int _phy_54616_fiber_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int _phy_54616_fiber_ability_advert_set(int unit, soc_port_t port,soc_port_ability_t *ability);
STATIC int phy_54616_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_54616_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_54616_master_set(int unit, soc_port_t port, int master);
STATIC int phy_54616_autoneg_set(int unit, soc_port_t port, int autoneg);
STATIC int phy_54616_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
STATIC int phy_54616_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_54616_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability);

/*
 * Function:
 *      _phy_54618e_cl45_reg_read
 * Purpose:
 *      Read Clause 45 Register using Clause 22 register access
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      pc        - Phy control
 *      flags     - Flags
 *      dev_addr  - Clause 45 Device
 *      reg_addr  - Register Address to read
 *      val       - Value Read
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_54618e_cl45_reg_read(int unit, phy_ctrl_t *pc, uint32 flags, 
                          uint8 dev_addr, uint16 reg_addr, uint16 *val)
{
    /* Write Device Address to register 0x0D (Set Function field to Address)*/
    SOC_IF_ERROR_RETURN
        (PHY54616_REG_WRITE(unit, pc, flags, 0x00, 0x0D, (dev_addr & 0x001f)));

    /* Select the register by writing to register address to register 0x0E */
    SOC_IF_ERROR_RETURN
        (PHY54616_REG_WRITE(unit, pc, flags, 0x00, 0x0E, reg_addr));

    /* Write Device Address to register 0x0D (Set Function field to Data)*/
    SOC_IF_ERROR_RETURN
        (PHY54616_REG_WRITE(unit, pc, flags, 0x00, 0x0D, 
                            ((0x4000) | (dev_addr & 0x001f))));

    /* Read register 0x0E to get the value */
    SOC_IF_ERROR_RETURN
        (PHY54616_REG_READ(unit, pc, flags, 0x00, 0x0E, val));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_54618e_cl45_reg_write
 * Purpose:
 *      Write Clause 45 Register content using Clause 22 register access
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      pc        - Phy control
 *      flags     - Flags
 *      dev_addr  - Clause 45 Device
 *      reg_addr  - Register Address to read
 *      val       - Value to be written
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_54618e_cl45_reg_write(int unit, phy_ctrl_t *pc, uint32 flags, 
                           uint8 dev_addr, uint16 reg_addr, uint16 val)
{
    /* Write Device Address to register 0x0D (Set Function field to Address)*/
    SOC_IF_ERROR_RETURN
        (PHY54616_REG_WRITE(unit, pc, flags, 0x00, 0x0D, (dev_addr & 0x001f)));

    /* Select the register by writing to register address to register 0x0E */
    SOC_IF_ERROR_RETURN
        (PHY54616_REG_WRITE(unit, pc, flags, 0x00, 0x0E, reg_addr));

    /* Write Device Address to register 0x0D (Set Function field to Data)*/
    SOC_IF_ERROR_RETURN
        (PHY54616_REG_WRITE(unit, pc, flags, 0x00, 0x0D, 
                            ((0x4000) | (dev_addr & 0x001f))));

    /* Write register 0x0E to write the value */
    SOC_IF_ERROR_RETURN
        (PHY54616_REG_WRITE(unit, pc, flags, 0x00, 0x0E, val));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_54618e_cl45_reg_modify
 * Purpose:
 *      Modify Clause 45 Register contents using Clause 22 register access
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      pc        - Phy control
 *      flags     - Flags
 *      reg_bank  - Register bank
 *      dev_addr  - Clause 45 Device
 *      reg_addr  - Register Address to read
 *      val       - Value 
 *      mask      - Mask for modifying the contents of the register
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_54618e_cl45_reg_modify(int unit, phy_ctrl_t *pc, uint32 flags, 
                            uint8 dev_addr, uint16 reg_addr, uint16 val,
                            uint16 mask)
{
    uint16 value = 0;

    SOC_IF_ERROR_RETURN
        (PHY54618E_CL45_REG_READ(unit, pc, flags, dev_addr, reg_addr, &value));

    value = (val & mask) | (value & ~mask);

    SOC_IF_ERROR_RETURN
        (PHY54618E_CL45_REG_WRITE(unit, pc, flags, dev_addr, reg_addr, value));

    return SOC_E_NONE;
}

/***********************************************************************
 *
 * HELPER FUNCTIONS
 */
/*
 * Function:
 *      _phy_54616_medium_check
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
_phy_54616_medium_check(int unit, soc_port_t port, int *medium)
{
    phy_ctrl_t    *pc = EXT_PHY_SW_STATE(unit, port);
    uint16         tmp = 0xffff;

    *medium = SOC_PORT_MEDIUM_COPPER;

    if (PHY_PRIMARY_SERDES_MODE(unit, port)) {
        /* Ie fiber capable */

        if (pc->automedium) {
            /* Read Mode Register (0x1c shadow 11111) */
            SOC_IF_ERROR_RETURN
                (READ_PHY54616_MODE_CTRLr(unit, pc, &tmp));

            if (pc->fiber.preferred) {
                /* 0x10 Fiber Signal Detect
                 * 0x20 Copper Energy Detect
                 */
                if ((tmp & 0x30) == 0x20) {
                    /* Only copper is link up */
                    *medium = SOC_PORT_MEDIUM_COPPER;
                } else {
                    *medium = SOC_PORT_MEDIUM_FIBER;
                }
            } else {
                if ((tmp & 0x30) == 0x10) {
                    /* Only fiber is link up */
                    *medium = SOC_PORT_MEDIUM_FIBER;
                } else {
                    *medium = SOC_PORT_MEDIUM_COPPER;
                }
            }
        } else {
            *medium = pc->fiber.preferred ? SOC_PORT_MEDIUM_FIBER:
                      SOC_PORT_MEDIUM_COPPER;
        } 
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "_phy_54616_medium_check: "
                            "u=%d p=%d fiber_pref=%d 0x1c(11111)=%04x fiber=%d\n"),
                 unit, port, pc->fiber.preferred, tmp, 
                 (*medium == SOC_PORT_MEDIUM_FIBER) ? 1 : 0));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_medium_status
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
phy_54616_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
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
 *      _phy_54616_medium_config_update
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
_phy_54616_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_54616_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_54616_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_54616_master_set(unit, port, cfg->master));
    SOC_IF_ERROR_RETURN
        (phy_54616_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_54616_autoneg_set(unit, port, cfg->autoneg_enable));
    SOC_IF_ERROR_RETURN
        (phy_54616_mdix_set(unit, port, cfg->mdix));

    return SOC_E_NONE;
}

/***********************************************************************
 *
 * PHY54616 DRIVER ROUTINES
 */

/*
 * Function:
 *      phy_54616_medium_config_set
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
phy_54616_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t    *pc;
    soc_phy_config_t *active_medium;  /* Currently active medium */
    soc_phy_config_t *change_medium;  /* Requested medium */
    soc_phy_config_t *other_medium;   /* The other medium */
    int               medium_update;

    if (NULL == cfg) {
        return SOC_E_PARAM;
    }

    pc            = EXT_PHY_SW_STATE(unit, port);
    medium_update = FALSE;

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        if (!pc->automedium) {
            if (!PHY_COPPER_MODE(unit, port)) {
                return SOC_E_UNAVAIL;
            }
            /* check if device is fiber capable before switching */
            if (cfg->preferred == 0) {

                if (!PHY_IS_BCM54616S(pc)) {
                    /* return if not fiber capable*/
                    return SOC_E_UNAVAIL;
                }
            }
        }
        
        change_medium  = &pc->copper;
        other_medium   = &pc->fiber;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->fiber;
        other_medium   = &pc->copper;
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

    if (medium_update) {
        /* The new configuration may cause medium change. Check
         * and update medium.
         */
        SOC_IF_ERROR_RETURN
            (_phy_54616_medium_change(unit, port,TRUE));
    } else {
        active_medium = (PHY_COPPER_MODE(unit, port)) ?  
                            &pc->copper : &pc->fiber;
        if (active_medium == change_medium) {
            /* If the medium to update is active, update the configuration */
            SOC_IF_ERROR_RETURN
                (_phy_54616_medium_config_update(unit, port, change_medium));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_medium_config_get
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
phy_54616_medium_config_get(int unit, soc_port_t port, 
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
 *      _phy_54616_reset_setup
 * Purpose:
 *      Function to reset the PHY and set up initial operating registers.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Configures for copper/fiber according to the current
 *      setting of PHY_FLAGS_FIBER.
 */

STATIC int
_phy_54616_reset_setup(int unit, soc_port_t port)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Reset the PHY with previously registered callback function. */
    SOC_IF_ERROR_RETURN(soc_phy_reset(unit, port));

    sal_usleep(10000);

    /* Disable super-isolate */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54616_MII_POWER_CTRLr(unit, pc, 0x0, 1U<<5));

    /* BCM54618E/SE is EEE capable */
    if ( PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
        /* disable AutogrEEEn (default EEE Native node) */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_AUTOEEECTRLr(unit, pc, 0x0000, 0x0001));

        PHY_FLAGS_SET(unit, port, PHY_FLAGS_EEE_CAPABLE);
    }

    SOC_IF_ERROR_RETURN
        (_phy_54616_no_reset_setup(unit, port));

    return SOC_E_NONE;
}

STATIC int
_phy_54616_no_reset_setup(int unit, soc_port_t port)
{
    phy_ctrl_t    *int_pc;
    phy_ctrl_t    *pc;
    uint16         data, mask;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_54616_reset_setup: u=%d p=%d medium=%s\n"),
                         unit, port,
              PHY_COPPER_MODE(unit, port) ? "COPPER" : "FIBER"));

    pc      = EXT_PHY_SW_STATE(unit, port);
    int_pc  = INT_PHY_SW_STATE(unit, port);


    /* copper regs */
    if (!pc->copper.enable || (!pc->automedium && pc->fiber.preferred)) {
        /* Copper interface is not used. Powered down. */
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "%s: u=%d p=%d Copper PowerDown=%d!\n"),
                             FUNCTION_NAME(), unit, port, 1));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54616_MII_CTRLr(unit, pc, MII_CTRL_PD, MII_CTRL_PD));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "%s: u=%d p=%d Copper PowerDown=%d!\n"),
                             FUNCTION_NAME(), unit, port, 0));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54616_MII_CTRLr(unit, pc, 0, MII_CTRL_PD));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY54616_MII_GB_CTRLr(unit, pc,
                            MII_GB_CTRL_ADV_1000FD | MII_GB_CTRL_PT));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY54616_MII_CTRLr(unit, pc,
                MII_CTRL_FD | MII_CTRL_SS_1000 | MII_CTRL_AE));
    }

    if (NULL != int_pc) {
        SOC_IF_ERROR_RETURN
            (PHY_INIT(int_pc->pd, unit, port));
    }

    /* Configure Auto-detect Medium (0x1c shadow 11110) */
    mask = 0x33f;               /* Auto-detect bit mask */

    /* In BCM54616S the SD pin is internally tied. For proper operation
       SD_INV and QUALIFY_SD_WITH_SYNC should be set.                  */
    data = (1 << 8) | (1 << 5);

    if (pc->automedium) {
        data |= 1 << 0;    /* Enable auto-detect medium */
    }
    /* When no link partner exists, fiber should be default medium */
    /* When both link partners exipc, fiber should be prefered medium
     * for the following reasons.
     * 1. Most fiber module generates signal detect when the fiber cable is
     *    plugged in regardless of any activity even if the link partner is
     *    powered down.
     * 2. Fiber mode consume much lesser power than copper mode.
     */
    if (pc->fiber.preferred) {
        data |= 1 << 1;    /* Fiber selected when both media are active */
        data |= 1 << 2;    /* Fiber selected when no medium is active */
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54616_AUTO_DETECT_MEDIUMr(unit, pc, data, mask));

#if DISABLE_CLK125
    /* Reduce EMI emissions by disabling the CLK125 pin if not used  */
    data = 0x001c;  /* When write, bit 9 and 7 must be zero and */ 
    data = 0x029c;  /* bit 2, 3, and 4 must be one. */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54616_SPARE_CTRL_3r(unit, pc, 0, 1));
#endif

#if AUTO_MDIX_WHEN_AN_DIS
    /* Enable Auto-MDIX When autoneg disabled */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54616_MII_MISC_CTRLr(unit, pc, 0x200, 0x200));
#endif
    /*
     * Configure Auxiliary 1000BASE-X control register to turn off
     * carrier extension.  The Intel 7131 NIC does not accept carrier
     * extension and gets CRC errors.
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54616_AUX_1000X_CTRLr(unit, pc, 0x0040, 0x0040));
    /* Enable LPI pass through on PHY for native EEE */ /*
    if ( PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
        SOC_IF_ERROR_RETURN   
            (phy_reg_aer_modify(unit, pc, 0x833e, 0xc000, 0xc000));
    }
    */

    /* Configure extended control register for led mode and jumbo frame support */
    mask  = (1 << 5) | (1 << 0);

    /* if using led link/activity mode, disable led traffic mode */
    if (pc->ledctrl & 0x10 || pc->ledselect == 0x01) {
        data = 0;
    } else {
        data  = 1 << 5;      /* Enable LEDs to indicate traffic status */
    }
    /* Configure Extended Control Register for Jumbo frames support */
    data |= 1 << 0;      /* Set high FIFO elasticity to support jumbo frames */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54616_MII_ECRr(unit, pc, data, mask));
    /* Enable extended packet length (4.5k through 25k) */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54616_MII_AUX_CTRLr(unit, pc, 0x4000, 0x4000));
    /* 1000BASE-T PCS Transmit FIFO Elasticity          *\
    \* enable jumbo frame for SGMII-Copper or GBIC mode */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54616_AUTO_DETECT_SGMII_MEDIAr(unit, pc, 0x4, 0x4));

    /* Configure LED selectors */
    data = ((pc->ledmode[1] & 0xf) << 4) | (pc->ledmode[0] & 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54616_LED_SELECTOR_1r(unit, pc, data));

    data = ((pc->ledmode[3] & 0xf) << 4) | (pc->ledmode[2] & 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54616_LED_SELECTOR_2r(unit, pc, data));

    data = (pc->ledctrl & 0x3ff);
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54616_LED_CTRLr(unit, pc, data));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54616_EXP_LED_SELECTORr(unit, pc, pc->ledselect));

    /*
    if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
        return SOC_E_NONE;
    }
    */

    /* 100BASE-TX initialization change for EEE and autogreen mode */
    /*
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54618_MII_AUX_CTRLr(unit, pc, 0x0c00, 0x0c00));
    SOC_IF_ERROR_RETURN(
        WRITE_PHY_REG(unit, pc, 0x17, 0x4022));
    SOC_IF_ERROR_RETURN(
        WRITE_PHY_REG(unit, pc, 0x15, 0x017b));
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54618_MII_AUX_CTRLr(unit, pc, 0x0400, 0x0c00));
    */

   return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_init
 * Purpose:
 *      Init function for 54616 PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_54616_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    int        fiber_capable;
    int        fiber_preferred;
    uint16     data;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54616_init: u=%d p=%d\n"),
                         unit, port));

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_IS_BCM54616S(pc) ) {
        /* check the for the precense of the SERDES block and and read
          the INTF_SEL[1:0] strap values */

        SOC_IF_ERROR_RETURN
            (READ_PHY54616_MODE_CTRLr(unit, pc, &data));

        if ((data & (0x2 << 1)) == (0x2 << 1)) {
            pc->interface = SOC_PORT_IF_SGMII;
            fiber_capable = FALSE;
        } else {
            /* pc->interface = SOC_PORT_IF_RGMII; */
            pc->interface = SOC_PORT_IF_GMII; /* MAC unable to handle SOC_PORT_IF_RGMII */
            PHY_FLAGS_SET(pc->unit, pc->port, PHY_FLAGS_PRIMARY_SERDES);
            fiber_capable = TRUE;
        }
    } else {
        pc->interface = SOC_PORT_IF_GMII;
        fiber_capable = FALSE;
    }

    /*
     * In automedium mode, the property phy_fiber_pref_port<X> is used
     * to set a preference for fiber mode in the event both copper and
     * fiber links are up.
     *
     * If NOT in automedium mode, phy_fiber_pref_port<X> is used to
     * select which of fiber or copper is forced.
     */
    if (!fiber_capable) {
        fiber_preferred = 0;
        pc->automedium      = 0;
    } else {
        fiber_preferred =
            soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 1);
        pc->automedium =
            soc_property_port_get(unit, port, spn_PHY_AUTOMEDIUM, 1);
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54616_init: "
                         "u=%d p=%d type=54616%s automedium=%d fiber_pref=%d\n"),
              unit, port, fiber_capable ? "S" : "",
              pc->automedium, fiber_preferred));

    pc->copper.enable = TRUE;
    pc->copper.preferred = !fiber_preferred;
    pc->copper.autoneg_enable = TRUE;
    pc->copper.force_speed = 1000;
    pc->copper.force_duplex = TRUE;
    pc->copper.master = SOC_PORT_MS_AUTO;
    pc->copper.mdix = SOC_PORT_MDIX_AUTO;

    pc->fiber.enable = fiber_capable;
    pc->fiber.preferred = fiber_preferred;
    pc->fiber.autoneg_enable = fiber_capable;
    pc->fiber.force_speed = 1000;
    pc->fiber.force_duplex = TRUE;
    pc->fiber.master = SOC_PORT_MS_NONE;
    pc->fiber.mdix = SOC_PORT_MDIX_NORMAL;

    SOC_IF_ERROR_RETURN
        (_phy_54616_copper_ability_local_get(unit, port, &pc->copper.advert_ability));
    if (fiber_capable) {
        (_phy_54616_fiber_ability_local_get(unit, port, &pc->fiber.advert_ability));
    }

    /* Initially configure for the preferred medium. */
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
    PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
    PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_MODE);

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

    SOC_IF_ERROR_RETURN
        (_phy_54616_reset_setup(unit, port));

    SOC_IF_ERROR_RETURN
        (_phy_54616_medium_config_update(unit, port,
                                        PHY_COPPER_MODE(unit, port) ?
                                        &pc->copper :
                                        &pc->fiber));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_enable_set
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
phy_54616_enable_set(int unit, soc_port_t port, int enable)
{
    uint16         power;
    phy_ctrl_t    *pc;

    pc     = EXT_PHY_SW_STATE(unit, port);
    power  = (enable) ? 0 : MII_CTRL_PD;

    /* Do not incorrectly Power up the interface if medium is disabled and 
     * global enable = true
     */
    if (pc->copper.enable && (pc->automedium || PHY_COPPER_MODE(unit, port))) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54616_MII_CTRLr(unit, pc, power, MII_CTRL_PD));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54616_enable_set: "
                             "Power %s copper medium\n"), (enable) ? "up" : "down"));
    }

    if (pc->fiber.enable && (pc->automedium || PHY_FIBER_MODE(unit, port))) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54616_1000X_MII_CTRLr(unit, pc, power, 
                                                   MII_CTRL_PD));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_54616_enable_set: "
                             "Power %s fiber medium\n"), (enable) ? "up" : "down"));
    }

    /* Update software state */
    SOC_IF_ERROR_RETURN(phy_fe_ge_enable_set(unit, port, enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_enable_get
 * Purpose:
 *      Enable or disable the physical interface for a 54616 device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_54616_enable_get(int unit, soc_port_t port, int *enable)
{
    return phy_fe_ge_enable_get(unit, port, enable);
}

/*
 * Function:
 *      _phy_54616_100fx_setup
 * Purpose:
 *      Switch from 1000X mode to 100FX.  
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_54616_fiber_100fx_setup(int unit, soc_port_t port) 
{
    phy_ctrl_t    *pc;
    uint16        data;

    pc          = EXT_PHY_SW_STATE(unit, port);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "_phy_54616_1000x_to_100fx: u=%d p=%d \n"),
                 unit, port));

    /* clear the power down bit of the SerDes */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "%s:p=%d,Pri-SerDes mode. Fiber PowerDown(%s)!\n"),
              FUNCTION_NAME(), port, "No"));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY54616_1000X_MII_CTRLr(unit, pc, 0, MII_CTRL_PD));

    /* set to fiber mode */ /* bit[2:1]=1 */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY54616_MODE_CTRLr(unit, pc, 0x2,0x6));

    /* set loopback */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY54616_1000X_MII_CTRLr(unit, pc, MII_CTRL_LE, MII_CTRL_LE));

    /* select 100FX mode */
        /* bit[0]=1 */
    data = 1;
    data |= pc->fiber.force_duplex? PHY_54616_PRIMARY_SERDES_100FX_FD:0;

    SOC_IF_ERROR_RETURN(
        MODIFY_PHY54616_100FX_CTRLr(unit, pc, data,
          1 | PHY_54616_PRIMARY_SERDES_100FX_FD));

    /* Power down SerDes RX path (Expention register)*/
    /* Power up SerDes RX path */ /* select Exp_reg 05 */
    SOC_IF_ERROR_RETURN(
        WRITE_PHY54616_EXP_LED_FLASH_CTRLr(unit, pc, 0x0C3B));
    SOC_IF_ERROR_RETURN(
        WRITE_PHY54616_EXP_LED_FLASH_CTRLr(unit, pc, 0x0C3A));

    /* reset loopback to switch clock back to SerDes RX clock */
        /* bit[14]=0 */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY54616_1000X_MII_CTRLr(unit, pc,
            PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE) ? MII_CTRL_PD : 0,
            MII_CTRL_LE | MII_CTRL_PD));

    PHY_FLAGS_SET(unit, port, PHY_FLAGS_100FX);
    pc->fiber.force_speed    = 100;
    pc->fiber.autoneg_enable = FALSE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_54616_1000x_setup
 * Purpose:
 *      Switch from 100FX mode to 1000X.  
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_54616_fiber_1000x_setup(int unit, soc_port_t port) 
{
    phy_ctrl_t    *pc;

    pc          = EXT_PHY_SW_STATE(unit, port);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "_phy_54616_fiber_1000x_setup: u=%d p=%d \n"),
                 unit, port));

    /* set to Fiber mode and enable primary SerDes register */
        /* bit[2:1]=01*/
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY54616_MODE_CTRLr(unit, pc, 0x2,0x6));
    /* enable 1000X, ie. disable 100FX */
        /* bit[0]=0 */
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY54616_100FX_CTRLr(unit,pc, 0, 1));

    /* clear the SerDes power down bit */
        /* bit[11]=0 */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "%s:p=%d,Pri-SerDes Mode. Fiber PowerDown(%s)!\n"),
              FUNCTION_NAME(), port, "No"));
    SOC_IF_ERROR_RETURN(
        MODIFY_PHY54616_1000X_MII_CTRLr(unit, pc,
            PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE) ? MII_CTRL_PD : 0,
            MII_CTRL_PD));
 
    pc->fiber.force_duplex = TRUE;
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);

    return SOC_E_NONE;
}

STATIC int
_phy_54616_medium_change(int unit, soc_port_t port, int force_update)
{
    phy_ctrl_t    *pc;
    int            medium;

    pc    = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_54616_medium_check(unit, port, &medium));

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        if ((!PHY_COPPER_MODE(unit, port)) || force_update) { /* Was fiber */ 
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
            /* BCM54618E/SE is EEE capable */
            if ( PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_EEE_CAPABLE);
            }
            SOC_IF_ERROR_RETURN
                (_phy_54616_no_reset_setup(unit, port));

            /* Do not power up the interface if medium is disabled. */
            if (pc->copper.enable) {
                SOC_IF_ERROR_RETURN
                    (_phy_54616_medium_config_update(unit, port, &pc->copper));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_54616_link_auto_detect: u=%d p=%d [F->C]\n"),
                      unit, port));
        }
    } else {        /* Fiber */
        if (PHY_COPPER_MODE(unit, port) || force_update) { /* Was copper */
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_EEE_CAPABLE);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
            SOC_IF_ERROR_RETURN
                (_phy_54616_no_reset_setup(unit, port));

            if (pc->fiber.enable) {
                SOC_IF_ERROR_RETURN
                    (_phy_54616_medium_config_update(unit, port, &pc->fiber));
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_54616_link_auto_detect: u=%d p=%d [C->F]\n"),
                      unit, port));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_link_get
 * Purpose:
 *      Determine the current link up/down status for a 54616 device.
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
phy_54616_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t    *pc;
    uint16         data; 
    uint16         mask;

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
            (_phy_54616_medium_change(unit, port,FALSE));
    }
    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(phy_fe_ge_link_get(unit, port, link));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY54616_1000X_MII_STATr(unit, pc, &data));
        *link = (data & MII_STAT_LA) ? TRUE : FALSE;
    }

    /* If preferred medium is up, disable the other medium 
     * to prevent false link. */
    if (pc->automedium) {
        mask = MII_CTRL_PD;
        if (pc->copper.preferred) {
            if (pc->fiber.enable) {
                /* power down secondary serdes */
                data = (*link && PHY_COPPER_MODE(unit, port)) ? MII_CTRL_PD : 0;
            } else {
                data = MII_CTRL_PD;
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54616_1000X_MII_CTRLr(unit, pc, data, mask));
        } else {  /* pc->fiber.preferred */
            if (pc->copper.enable) {
                data = (*link && PHY_FIBER_MODE(unit, port)) ? MII_CTRL_PD : 0;
            } else {
                data = MII_CTRL_PD;
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54616_MII_CTRLr(unit, pc, data, mask));
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_54616_link_get: u=%d p=%d mode=%s%s link=%d\n"),
                 unit, port,
                 PHY_COPPER_MODE(unit, port) ? "C" : "F",
                 PHY_FIBER_100FX_MODE(unit, port)? "(100FX)" : "",
                 *link));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_duplex_set
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
phy_54616_duplex_set(int unit, soc_port_t port, int duplex)
{
    int                  rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    if (PHY_COPPER_MODE(unit, port)) {
        rv = phy_fe_ge_duplex_set(unit, port, duplex);
        if (SOC_SUCCESS(rv)) {
            pc->copper.force_duplex = duplex;
        }
    } else {    /* Fiber */
        if (PHY_FIBER_100FX_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54616_100FX_CTRLr(unit, pc,
                       duplex? PHY_54616_PRIMARY_SERDES_100FX_FD:0, PHY_54616_PRIMARY_SERDES_100FX_FD));
            pc->fiber.force_duplex = duplex;
        } else { /* 1000X always full duplex */
            if (!duplex) {
                rv = SOC_E_UNAVAIL;
            }
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54616_duplex_set: u=%d p=%d d=%d rv=%d\n"),
              unit, port, duplex, rv));

    return rv;
}

/*
 * Function:
 *      phy_54616_duplex_get
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
phy_54616_duplex_get(int unit, soc_port_t port, int *duplex)
{
    phy_ctrl_t    *pc;
    uint16 data16;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        return phy_fe_ge_duplex_get(unit, port, duplex);    
    } else {
        if (PHY_FIBER_100FX_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (READ_PHY54616_100FX_CTRLr(unit, pc, &data16));
            *duplex = (data16 & PHY_54616_PRIMARY_SERDES_100FX_FD) ?
                      TRUE: FALSE;
        } else { /* 1000X */ 
            *duplex = TRUE;
        }
    }
        
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_speed_set
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
phy_54616_speed_set(int unit, soc_port_t port, int speed)
{
    int            rv; 
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    if (PHY_COPPER_MODE(unit, port)) {
        /* Note: mac.c uses phy_5690_notify_speed to update 5690 */
        rv = phy_fe_ge_speed_set(unit, port, speed);
        if (SOC_SUCCESS(rv)) {
            pc->copper.force_speed = speed;
        }
    } else {    /* Fiber */
        if (speed == 100) {
            rv = _phy_54616_fiber_100fx_setup(unit, port);
       } else  if (speed == 0 || speed == 1000) {
            rv = _phy_54616_fiber_1000x_setup(unit, port);
       } else {
            rv = SOC_E_CONFIG;
       }
       if (SOC_SUCCESS(rv)) {
           pc->fiber.force_speed = speed;
       }
    }
    if (SOC_SUCCESS(rv) && !PHY_SGMII_AUTONEG_MODE(unit, port)) {
        uint16 mii_ctrl;

        SOC_IF_ERROR_RETURN
            (READ_PHY54616_1000X_MII_CTRLr(unit, pc, &mii_ctrl));

        mii_ctrl &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
        switch(speed) {
        case 10:
            mii_ctrl |= MII_CTRL_SS_10;
            break;
        case 100:
            mii_ctrl |= MII_CTRL_SS_100;
            break;
        case 0: 
        case 1000:
            mii_ctrl |= MII_CTRL_SS_1000;
            break;
        default:
            return(SOC_E_CONFIG);
        }

        SOC_IF_ERROR_RETURN
            (WRITE_PHY54616_1000X_MII_CTRLr(unit, pc, mii_ctrl));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54616_speed_set: u=%d p=%d s=%d fiber=%d rv=%d\n"),
              unit, port, speed, PHY_FIBER_MODE(unit, port), rv));

    return rv;
}

/*
 * Function:
 *      phy_54616_speed_get
 * Purpose:
 *      Get the current operating speed for a 54616 device.
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
phy_54616_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t    *pc;
    uint16         opt_mode;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        return phy_fe_ge_speed_get(unit, port, speed);
    } else {
        *speed = 1000;
        SOC_IF_ERROR_RETURN
            (READ_PHY54616_EXP_OPT_MODE_STATr(unit, pc, &opt_mode));
        switch(opt_mode & (0x3 << 13)) {
        case (0 << 13):
            *speed = 10;
            break;
        case (1 << 13):
            *speed = 100;
            break;
        case (2 << 13):
            *speed = 1000;
            break;
        default:
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_54616_speed_get: u=%d p=%d invalid speed\n"),
                      unit, port));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_master_set
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
phy_54616_master_set(int unit, soc_port_t port, int master)
{
    int                  rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    if (PHY_COPPER_MODE(unit, port)) {
        rv = phy_fe_ge_master_set(unit, port, master);
        if (SOC_SUCCESS(rv)) {
            pc->copper.master = master;
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54616_master_set: u=%d p=%d master=%d fiber=%d rv=%d\n"),
              unit, port, master, PHY_FIBER_MODE(unit, port), rv));
    return rv;
}

/*
 * Function:
 *      phy_54616_master_get
 * Purpose:
 *      Get the current master mode for a 54616 device.
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
phy_54616_master_get(int unit, soc_port_t port, int *master)
{
    if (PHY_COPPER_MODE(unit, port)) {
        return phy_fe_ge_master_get(unit, port, master);
    } else {
        *master = SOC_PORT_MS_NONE;
        return SOC_E_NONE;
    }
}

/*
 * Function:
 *      phy_54616_autoneg_set
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
phy_54616_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    int                 rv;
    uint16              data, mask;
    phy_ctrl_t   *pc;

    /* don't allow AN if port is disabled  */

    if (autoneg) {
        if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
            return SOC_E_NONE;
        }
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    if (PHY_COPPER_MODE(unit, port)) {
        rv = phy_fe_ge_an_set(unit, port, autoneg);
        if (SOC_SUCCESS(rv)) {
            pc->copper.autoneg_enable = autoneg ? 1 : 0;
        }
    } else { 
        data = (autoneg) ? MII_CTRL_AE | MII_CTRL_RAN : 0;
        mask = MII_CTRL_AE | MII_CTRL_RAN | MII_CTRL_FD;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54616_1000X_MII_CTRLr(unit, pc, data, mask));

        pc->fiber.autoneg_enable = autoneg ? TRUE : FALSE;
    }
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54616_autoneg_set: u=%d p=%d autoneg=%d rv=%d\n"),
              unit, port, autoneg, rv));

    return rv;
}

/*
 * Function:
 *      phy_54616_autoneg_get
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
phy_54616_autoneg_get(int unit, soc_port_t port,
                     int *autoneg, int *autoneg_done)
{
    int           rv;
    uint16        data;
    phy_ctrl_t   *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    rv            = SOC_E_NONE;
    *autoneg      = FALSE;
    *autoneg_done = FALSE;
    if (PHY_COPPER_MODE(unit, port)) {
        rv = phy_fe_ge_an_get(unit, port, autoneg, autoneg_done);
    } else {
        /* autoneg is not enabled in 100FX mode */
        if (PHY_FIBER_100FX_MODE(unit, port)) {
            *autoneg = FALSE;
            *autoneg_done = TRUE;
            return rv;
        }
        SOC_IF_ERROR_RETURN
            (READ_PHY54616_1000X_MII_CTRLr(unit, pc, &data));
        if (data & MII_CTRL_AE) {
            *autoneg = TRUE;

            SOC_IF_ERROR_RETURN
                (READ_PHY54616_1000X_MII_STATr(unit, pc, &data));
            if (data & MII_STAT_AN_DONE) {
                *autoneg_done = TRUE;
            }
        }
        
    }

    return rv;
}

/*
 * Function:
 *      phy_54616_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
_phy_54616_fiber_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16      an_adv;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Support only full duplex */
    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? MII_ANP_C37_FD : 0;
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        an_adv |= MII_ANP_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        an_adv |= MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        an_adv |= MII_ANP_C37_PAUSE;
        break;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY54616_1000X_MII_ANAr(unit, pc, an_adv));

    return SOC_E_NONE;
}


STATIC int
phy_54616_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN
            (phy_fe_ge_ability_advert_set(unit, port, ability));
        pc->copper.advert_ability = *ability;
    } else { 
        SOC_IF_ERROR_RETURN
            (_phy_54616_fiber_ability_advert_set(unit, port, ability));
        pc->fiber.advert_ability = *ability;
    }

    /* EEE settings */
    if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
        uint16	eee_ability = 0;

        if ( ability->eee & SOC_PA_EEE_1GB_BASET ) {
            eee_ability |= 0x4;
        }
        if ( ability->eee & SOC_PA_EEE_100MB_BASETX ) {
            eee_ability |= 0x2;
        }
        if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_ENABLED) ) {
            SOC_IF_ERROR_RETURN
                    (MODIFY_PHY54618_EEE_ADVr(unit, pc, eee_ability, 0x0006));
        } else {
            SOC_IF_ERROR_RETURN
                    (MODIFY_PHY54618_EEE_ADVr(unit, pc, 0x0000, 0x0006));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
_phy_54616_fiber_ability_advert_get(int unit, soc_port_t port,soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16      an_adv;

    pc = EXT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_PHY54616_1000X_MII_ANAr(unit, pc, &an_adv));

    switch ( an_adv & (MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE) ) {
    case MII_ANP_C37_ASYM_PAUSE:
        ability->pause |= SOC_PA_PAUSE_TX;
        break;
    case (MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE):
        ability->pause |= SOC_PA_PAUSE_RX;
        break;
    case MII_ANP_C37_PAUSE:
        ability->pause |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        break;
    }

    ability->speed_full_duplex |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;

    return SOC_E_NONE;
}

STATIC int
phy_54616_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN
            (phy_fe_ge_ability_advert_get(unit, port, ability));

        /* EEE settings */
        if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
            uint16  eee_ability = 0;
            phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EEE_ADVr(unit, pc, &eee_ability));
            if ( eee_ability & 0x04 ) {
                ability->eee |= SOC_PA_EEE_1GB_BASET;
            }
            if ( eee_ability & 0x02 ) {
                ability->eee |= SOC_PA_EEE_100MB_BASETX;
            }
        }
    } else {    /* PHY_FIBER_MODE */
        SOC_IF_ERROR_RETURN
            (_phy_54616_fiber_ability_advert_get(unit, port, ability));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_ability_remote_get
 * Purpose:
 *      Get partners current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The remote advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. If Autonegotiation is
 *      disabled or in progress, this routine will return an error.
 */

STATIC int
_phy_54616_fiber_ability_remote_get(int unit, soc_port_t port,
                               soc_port_ability_t *ability)
{
    uint16            an_adv, data;
    phy_ctrl_t       *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_PHY54616_1000X_MII_CTRLr(unit, pc, &data));
    if (!(data & MII_CTRL_AE)) {
        return SOC_E_DISABLED;
    }
    SOC_IF_ERROR_RETURN
        (READ_PHY54616_1000X_MII_STATr(unit, pc, &data));

    if (data & MII_STAT_AN_DONE) {
        /* Decode remote advertisement only when link is up and autoneg is
         * completed.
         */

        SOC_IF_ERROR_RETURN
            (READ_PHY54616_1000X_MII_ANPr(unit, pc, &an_adv));

        ability->speed_full_duplex |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
        ability->speed_half_duplex |= (an_adv & MII_ANP_C37_HD) ? SOC_PA_SPEED_1000MB : 0;

        switch (an_adv & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
            case MII_ANP_C37_PAUSE:
                ability->pause |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANP_C37_ASYM_PAUSE:
                ability->pause |= SOC_PA_PAUSE_TX;
                break;
            case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
                ability->pause |= SOC_PA_PAUSE_RX;
                break;
        }

    } else {
        /* Simply return local abilities */
        SOC_IF_ERROR_RETURN
            (phy_54616_ability_advert_get(unit, port, ability));
    }

    return (SOC_E_NONE);
}

STATIC int
phy_54616_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    if ( PHY_COPPER_MODE(unit, port) ) {
        /* EEE settings */
        if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
            phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
            uint16       eee_resolution;

            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EEE_RESOLUTION_STATr(unit, pc, &eee_resolution));
            if ( eee_resolution & 0x04 ) {
                ability->eee |= SOC_PA_EEE_1GB_BASET;
            }
            if ( eee_resolution & 0x02 ) {
                ability->eee |= SOC_PA_EEE_100MB_BASETX;
            }
        }

        SOC_IF_ERROR_RETURN
            (phy_fe_ge_ability_remote_get(unit, port, ability));
    } else {    /* PHY_FIBER_MODE */
        SOC_IF_ERROR_RETURN
            (_phy_54616_fiber_ability_remote_get(unit, port, ability));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_lb_set
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
phy_54616_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

#if AUTO_MDIX_WHEN_AN_DIS
    {
        /* Disable Auto-MDIX When autoneg disabled */
        /* Enable Auto-MDIX When autoneg disabled */
        uint16 data = (enable) ? 0x0000 : 0x0200;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY_54616_MII_MISC_CTRLr(unit, pc, data, 0x0200)); 
    }
#endif

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_fe_ge_lb_set(unit, port, enable));
    } else { 
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54616_1000X_MII_CTRLr(unit, pc, (enable) ? MII_CTRL_LE : 0, MII_CTRL_LE));
                  
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_54616_lb_set: u=%d p=%d en=%d\n"), unit, port, enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_lb_get
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
phy_54616_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16         data;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_fe_ge_lb_get(unit, port, enable));
    } else {
       SOC_IF_ERROR_RETURN
           (READ_PHY54616_1000X_MII_CTRLr(unit, pc, &data));
        *enable = (data & MII_CTRL_LE) ? 1 : 0;
    }

    return SOC_E_NONE; 
}

/*
 * Function:
 *      phy_54616_interface_set
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
phy_54616_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(pif);

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_interface_get
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
phy_54616_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    *pif = pc->interface;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54616_ability_local_get
 * Purpose:
 *      Get the abilities of the local gigabit PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - return device's abilities.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The ability is retrieved only for the ACTIVE medium.
 */

STATIC int
_phy_54616_copper_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    ability->speed_half_duplex  = SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;
    ability->speed_full_duplex  = SOC_PA_SPEED_1000MB | SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_SGMII | SOC_PA_INTF_RGMII;
    ability->medium    = SOC_PA_MEDIUM_COPPER;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;

    return (SOC_E_NONE);
}

STATIC int
_phy_54616_fiber_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{

    ability->speed_half_duplex  = SOC_PA_SPEED_1000MB;
    ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;

    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_RGMII;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_ABILITY_NONE;

    return (SOC_E_NONE);
}

STATIC int
phy_54616_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(_phy_54616_copper_ability_local_get(unit, port, ability));
    } else {
        SOC_IF_ERROR_RETURN(_phy_54616_fiber_ability_local_get(unit, port, ability));
    }

    return (SOC_E_NONE);
}


/*
 * Function:
 *      phy_54616_mdix_set
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
phy_54616_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    phy_ctrl_t    *pc;
    int                  speed;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (PHY_COPPER_MODE(unit, port)) {
        switch (mode) {
        case SOC_PORT_MDIX_AUTO:
            /* Clear bit 14 for automatic MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54616_MII_ECRr(unit, pc, 0, 0x4000));

            /* Clear bit 9 to disable forced auto MDI xover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54616_MII_MISC_CTRLr(unit, pc, 0, 0x0200));
            break;

        case SOC_PORT_MDIX_FORCE_AUTO:
            /* Clear bit 14 for automatic MDI crossover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54616_MII_ECRr(unit, pc, 0, 0x4000));

            /* Set bit 9 to disable forced auto MDI xover */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54616_MII_MISC_CTRLr(unit, pc, 0x0200, 0x0200));
            break;

        case SOC_PORT_MDIX_NORMAL:
            SOC_IF_ERROR_RETURN(phy_54616_speed_get(unit, port, &speed));
            if (speed == 0 || speed == 10 || speed == 100) {
                /* Set bit 14 for automatic MDI crossover */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY54616_MII_ECRr(unit, pc, 0x4000, 0x4000));

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY54616_TEST1r(unit, pc, 0));
            } else {
                return SOC_E_UNAVAIL;
            }
            break;

        case SOC_PORT_MDIX_XOVER:
            SOC_IF_ERROR_RETURN(phy_54616_speed_get(unit, port, &speed));
            if (speed == 0 || speed == 10 || speed == 100) {
                /* Set bit 14 for automatic MDI crossover */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY54616_MII_ECRr(unit, pc, 0x4000, 0x4000));

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY54616_TEST1r(unit, pc, 0x0080));
            } else {
                return SOC_E_UNAVAIL;
            }
            break;

        default:
            return SOC_E_PARAM;
        }
        pc->copper.mdix = mode;
    } else {    /* fiber */
        if (mode != SOC_PORT_MDIX_NORMAL) {
            return SOC_E_UNAVAIL;
        }
    }
    return SOC_E_NONE;
}        

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *      phy_54616_mdix_wb_update
 * Description:
 *      Read the hw, derive the mode and update the mdix mode in sw structure
 *      during wb.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      mode - (OUT) :One of:
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
phy_54616_mdix_wb_update(int unit, soc_port_t port)
{
    phy_ctrl_t    *pc;
    int            speed;
    int            mode = 0;
    uint16         val = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        /* Check bit 14 for automatic MDI crossover */
        SOC_IF_ERROR_RETURN (READ_PHY54616_MII_ECRr(unit, pc, &val));
        if(val & 0x4000) {
            SOC_IF_ERROR_RETURN(phy_54616_speed_get(unit, port, &speed));
            if (speed == 0 || speed == 10 || speed == 100) {
                SOC_IF_ERROR_RETURN (READ_PHY54616_TEST1r(unit, pc, &val));
                if (0 == val) {
                    mode = SOC_PORT_MDIX_NORMAL;
                } else if (0x80 == val) {
                    mode = SOC_PORT_MDIX_XOVER;
                } else {
                    return SOC_E_UNAVAIL;
                }
            } else {
                return SOC_E_UNAVAIL;
            }
        } else {
            /* Check bit 9 forced auto MDI xover */
            SOC_IF_ERROR_RETURN (READ_PHY54616_MII_MISC_CTRLr(unit, pc,&val));
            if (val & 0x200) {
                /* bit 9 Set, forced auto MDI xover */
                mode = SOC_PORT_MDIX_FORCE_AUTO;
            } else {
                /* bit 9 Clear, forced auto MDI xover */
                mode = SOC_PORT_MDIX_AUTO;
            }
        }
        pc->copper.mdix = mode;
    } else {
        /* fiber */
        mode = SOC_PORT_MDIX_NORMAL;
    }
    return SOC_E_NONE;
}
#endif /* defined(BCM_WARM_BOOT_SUPPORT) */


/*
 * Function:
 *      phy_54616_mdix_get
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
phy_54616_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t    *pc;
    int            speed;

    if (mode == NULL) {
        return SOC_E_PARAM;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        return phy_54616_mdix_wb_update(unit, port);
    }
#endif /* defined(BCM_WARM_BOOT_SUPPORT) */

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN(phy_54616_speed_get(unit, port, &speed));
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
 *      phy_54616_mdix_status_get
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
phy_54616_mdix_status_get(int unit, soc_port_t port, 
                         soc_port_mdix_status_t *status)
{
    phy_ctrl_t    *pc;
    uint16          data;

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_COPPER_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY54616_MII_ESRr(unit, pc, &data));
        if (data & 0x2000) {
            *status = SOC_PORT_MDIX_STATUS_XOVER;
        } else {
            *status = SOC_PORT_MDIX_STATUS_NORMAL;
        }
    } else {
        *status = SOC_PORT_MDIX_STATUS_NORMAL;
    }

    return SOC_E_NONE;
}    

STATIC int
_phy_54616_power_mode_set (int unit, soc_port_t port, int mode)
{
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    if (pc->power_mode == mode) {
        return SOC_E_NONE;
    }

    /* Auto Power-Down   Reg.0x1C, shadow 0x0A, bit[5] Auto Power Down  Mode */
    /* Autodetect Medium Reg.0x1C, shadow 0x1E, bit[5] Auto Fiber Power Mode */
    if ( (mode == SOC_PHY_CONTROL_POWER_FULL) ||
         (mode == SOC_PHY_CONTROL_POWER_AUTO_DISABLE) ) {
        /* disable the auto power mode */
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY54616_AUTO_POWER_DOWNr(unit,pc,
                                    0 , PHY_54616_AUTO_PWRDWN_EN) );
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY54616_AUTO_DETECT_MEDIUMr(unit,pc,
                                    0 , PHY_54616_FiBER_AUTO_PWRDWN_EN) );
    } else if ( (mode == SOC_PHY_CONTROL_POWER_AUTO) ||
                (mode == SOC_PHY_CONTROL_POWER_AUTO_FULL) ) {
        /* enable the auto power mode */
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY54616_AUTO_POWER_DOWNr(unit,pc,
                                    PHY_54616_AUTO_PWRDWN_EN,
                                    PHY_54616_AUTO_PWRDWN_EN) );
        SOC_IF_ERROR_RETURN(
            MODIFY_PHY54616_AUTO_DETECT_MEDIUMr(unit,pc,
                                    PHY_54616_FiBER_AUTO_PWRDWN_EN,
                                    PHY_54616_FiBER_AUTO_PWRDWN_EN) );
    } /* support only FULL & AUTO, discard POWER_LOW & POWER_AUTO_LOW modes */

    pc->power_mode = mode;
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_54618_eee_enable
 * Purpose:
 *      Enable or disable EEE (Native)
 * Parameters:
 *      unit   - StrataSwitch unit #.
 *      port   - StrataSwitch port #.
 *      enable - Enable Native EEE
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
_phy_54618_eee_enable(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    
    if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
        return SOC_E_UNAVAIL;
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    if ( enable == 1 ) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_CTRLr(unit, pc, 0x8000, 0x8000)); /* 7.803d enable EEE */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_ADVr(unit, pc, 0x0006, 0x0006));
        pc->copper.advert_ability.eee |= (SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);

        SOC_IF_ERROR_RETURN            /* native mode, disable AutogrEEEn */
            (MODIFY_PHY54618_EEE_AUTOEEECTRLr(unit, pc, 0x0000, 0x0001));
        SOC_IF_ERROR_RETURN(PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 0x1, 0x1)); /* exp af */
    } else {  /* enable==0 */
        SOC_IF_ERROR_RETURN(PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 0x0, 0x1)); /* exp af */
        pc->copper.advert_ability.eee &= ~(SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_ADVr(unit, pc, 0x0000, 0x0006));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_CTRLr(unit, pc, 0x0000, 0x8000)); /* 7.803d disable EEE */
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_54618_eee_auto_enable
 * Purpose:
 *      Enable or disable EEE (Native)
 * Parameters:
 *      unit   - StrataSwitch unit #.
 *      port   - StrataSwitch port #.
 *      enable - Enable Native EEE
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
_phy_54618_eee_auto_enable(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    
    if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
        return SOC_E_UNAVAIL;
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    if ( enable == 1 ) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_CTRLr(unit, pc, 0x8000, 0x8000)); /* 7.803d enable EEE */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_ADVr(unit, pc, 0x0006, 0x0006));
        pc->copper.advert_ability.eee |= (SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);

        SOC_IF_ERROR_RETURN            /* enable AutogrEEEn */
            (MODIFY_PHY54618_EEE_AUTOEEECTRLr(unit, pc, 0x0001, 0x0001));
        SOC_IF_ERROR_RETURN(PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 0x1, 0x1)); /* exp af */
    } else {  /*enable == 0 */
        SOC_IF_ERROR_RETURN(PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 0x0, 0x3)); /* exp af */
        pc->copper.advert_ability.eee &= ~(SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_ADVr(unit, pc, 0x0000, 0x0006));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_CTRLr(unit, pc, 0x0000, 0x8000)); /* 7.803d disable EEE */
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54616_control_set
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
phy_54616_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t *pc;
    uint16 data16;
    int saved_autoneg;

    PHY_CONTROL_TYPE_CHECK(type);

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    switch(type) {
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        if ( PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
            /* RXC SyncE on/off. Expansion Register 0Eh, bit[4] */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54616_EXP_SYNCE_CTRLr(unit, pc,
                                                 value ? (1U << 4) : 0, 1U << 4));
        }
        else if ( pc->phy_rev == 0xB ) {    /* BCM54616 Rev.B only */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY54616_EXP_SGMII_REC_CTRLr(unit, pc,
                                                    value ? (1U << 4) : 0));
        }
        break;

    case SOC_PHY_CONTROL_CLOCK_SECONDARY_ENABLE:
        if ( PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
            /* CLK125 SyncE on/off. Top-Level Expansion Reg. 34h, bit[3] */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54616_TOPLVL_CLK125_ADCr(unit, pc,
                                                    value ? (1U << 3) : 0, 1U << 3));
        }
        break;

    case SOC_PHY_CONTROL_POWER:
        rv = _phy_54616_power_mode_set(unit,port,value);
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
        if (value <= PHY_54616_AUTO_PWRDWN_WAKEUP_MAX) {

            /* at least one unit */
            if (value < PHY_54616_AUTO_PWRDWN_WAKEUP_UNIT) {
                value = PHY_54616_AUTO_PWRDWN_WAKEUP_UNIT;
            }
        } else { /* anything more then max, set to the max */
            value = PHY_54616_AUTO_PWRDWN_WAKEUP_MAX;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54616_AUTO_POWER_DOWNr(unit,pc,
                      value/PHY_54616_AUTO_PWRDWN_WAKEUP_UNIT,
                      PHY_54616_AUTO_PWRDWN_WAKEUP_MASK));
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
        /* sleep time configuration is either 2.7s or 5.4 s, default is 2.7s */
        if (value < PHY_54616_AUTO_PWRDWN_SLEEP_MAX) {
            data16 = 0; /* anything less than 5.4s, default to 2.7s */
        } else {
            data16 = PHY_54616_AUTO_PWRDWN_SLEEP_MASK;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54616_AUTO_POWER_DOWNr(unit,pc,
                      data16,
                      PHY_54616_AUTO_PWRDWN_SLEEP_MASK));
        break;

    case SOC_PHY_CONTROL_EEE:
        if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
            rv = SOC_E_UNAVAIL;
        }

        if (value == 1) {
            /* Enable Native EEE */
            SOC_IF_ERROR_RETURN
                (_phy_54618_eee_enable(unit, port, 1));
            saved_autoneg = pc->copper.autoneg_enable;
            /* Initiate AN */
            SOC_IF_ERROR_RETURN
                (phy_54616_autoneg_set(unit, port, 1)); /* must AN=on for EEE */
            pc->copper.autoneg_enable = saved_autoneg;
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
            PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_MODE);    /* nativeEEE */
        } else {
            SOC_IF_ERROR_RETURN
                (_phy_54618_eee_enable(unit, port, 0));
            /* Initiate AN if administratively enabled */
            SOC_IF_ERROR_RETURN
                (phy_54616_autoneg_set(unit, port, pc->copper.autoneg_enable ? 1 : 0));
            PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }

        if (value == 1) {
            /* Disable Native EEE */
            SOC_IF_ERROR_RETURN
                (_phy_54618_eee_enable(unit, port, 0));
            /* Enable Auto EEE */
            SOC_IF_ERROR_RETURN
                (_phy_54618_eee_auto_enable(unit, port, 1));
            saved_autoneg = pc->copper.autoneg_enable;
            /* Initiate AN */
            SOC_IF_ERROR_RETURN
                (phy_54616_autoneg_set(unit, port, 1));
            pc->copper.autoneg_enable = saved_autoneg;
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
            PHY_FLAGS_SET(unit, pc->port, PHY_FLAGS_EEE_MODE);    /* autoEEE */
        } else {
            /* Disable Auto EEE */
            SOC_IF_ERROR_RETURN
                (_phy_54618_eee_auto_enable(unit, port, 0));
            /* Initiate AN if administratively enabled */
            SOC_IF_ERROR_RETURN
                (phy_54616_autoneg_set(unit, port, pc->copper.autoneg_enable ? 1 : 0));
            PHY_FLAGS_CLR(unit, pc->port, PHY_FLAGS_EEE_ENABLED);
        }
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }

        if (value > 7) { /* Values needs to between 0 to 7 (inclusive) */
            rv = SOC_E_CONFIG;
        }

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY54618_EEE_AUTOEEECTRLr(unit, pc, value<<8, 0x0700));
        /* Setting GPHY core, MII buffer register as well */
        SOC_IF_ERROR_RETURN
           (MODIFY_PHY54618_EXP_MII_BUF_CNTL_STAT1r(unit, pc, 
                                                              value<<8, 0x0700));
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
        if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
            rv = SOC_E_UNAVAIL;
        }
        /* Fixed latency can have only 0 or 1 value  */
        if ((value != FALSE) && (value != TRUE)) {
            return SOC_E_PARAM;
        }
        if ( value == FALSE ) {  
            /* Sets Variable Latency */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY54618_EEE_AUTOEEECTRLr(unit, pc, 0x0004, 0x0004));
            /* Setting GPHY core, MII buffer register as well */
            SOC_IF_ERROR_RETURN
               (MODIFY_PHY54618_EXP_MII_BUF_CNTL_STAT1r(unit, pc,
                                                                  0x0004, 0x0004));
        } else {
            /* Sets Fixed Latency */
            SOC_IF_ERROR_RETURN
	        (MODIFY_PHY54618_EEE_AUTOEEECTRLr(unit, pc, 0x0000, 0x0004));
            /* Setting GPHY core, MII buffer register as well */
            SOC_IF_ERROR_RETURN
               (MODIFY_PHY54618_EXP_MII_BUF_CNTL_STAT1r(unit, pc,
                                                                  0x0000, 0x0004));
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
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }

        /* Time modification not allowed, IEEE EEE spec constants */
        rv = SOC_E_UNAVAIL;
        break;

    case SOC_PHY_CONTROL_EEE_STATISTICS_CLEAR:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }
        
        if (value == 1) {
            /* Clearing the stats: Enable clear on Read  */
            SOC_IF_ERROR_RETURN
               (MODIFY_PHY54618_EXP_MII_BUF_CNTL_STAT1r(unit, pc, 
                                                        0x1000, 0x1000));
            /* Read all the stats to clear */
            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EXP_EEE_TX_EVENTSr(unit, pc, &data16));
            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EXP_EEE_TX_EVENTSr(unit, pc, &data16));
            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EXP_EEE_TX_DURATIONr(unit, pc, &data16));
            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EXP_EEE_TX_DURATIONr(unit, pc, &data16));
            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EXP_EEE_RX_EVENTSr(unit, pc, &data16));
            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EXP_EEE_RX_EVENTSr(unit, pc, &data16));
            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EXP_EEE_RX_DURATIONr(unit, pc, &data16));
            SOC_IF_ERROR_RETURN
                (READ_PHY54618_EXP_EEE_RX_DURATIONr(unit, pc, &data16));

            /* Disable Clear on Read  */
            SOC_IF_ERROR_RETURN
               (MODIFY_PHY54618_EXP_MII_BUF_CNTL_STAT1r(unit, pc, 
                                                        0x0000, 0x1000));
        }

        break;

    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}

/*
 * Function:
 *      phy_54616_control_get
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
phy_54616_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t *pc;
    uint16 data;
    uint32 temp;

    PHY_CONTROL_TYPE_CHECK(type);

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    switch(type) {
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        if ( PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
            /* RXC SyncE on/off. Expansion Register 0Eh, bit[4] */
            SOC_IF_ERROR_RETURN
                (READ_PHY54616_EXP_SYNCE_CTRLr(unit, pc, &data));
            *value = (data & (1U << 4)) ? TRUE : FALSE;
        }
        else if ( pc->phy_rev == 0xB ) {    /* BCM54616 Rev.B only */
            SOC_IF_ERROR_RETURN
                (READ_PHY54616_EXP_SGMII_REC_CTRLr(unit, pc, &data));
            *value = (data & (1U << 4)) ? TRUE : FALSE;
        }
        break;

    case SOC_PHY_CONTROL_CLOCK_SECONDARY_ENABLE:
        if ( PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
            /* CLK125 SyncE on/off. Top-Level Expansion Reg. 34h, bit[3] */
            SOC_IF_ERROR_RETURN
                (READ_PHY54616_TOPLVL_CLK125_ADCr(unit, pc, &data));
            *value = (*value || (data & (1U << 3))) ? TRUE : FALSE;
        }
        break;

    case SOC_PHY_CONTROL_POWER:
        *value = pc->power_mode;
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
        SOC_IF_ERROR_RETURN
            (READ_PHY54616_AUTO_POWER_DOWNr(unit,pc, &data));

        if (data & PHY_54616_AUTO_PWRDWN_SLEEP_MASK) {
            *value = PHY_54616_AUTO_PWRDWN_SLEEP_MAX;
        } else {
            *value = 2700;
        }
        break;

    case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
        SOC_IF_ERROR_RETURN
            (READ_PHY54616_AUTO_POWER_DOWNr(unit,pc, &data));

        data &= PHY_54616_AUTO_PWRDWN_WAKEUP_MASK;
        *value = data * PHY_54616_AUTO_PWRDWN_WAKEUP_UNIT;
        break;

    case SOC_PHY_CONTROL_EEE:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }
        *value = (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_ENABLED) &&
                 !PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_MODE)) ? 1 : 0;
        break;

    case SOC_PHY_CONTROL_EEE_AUTO:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }
        *value = (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_ENABLED) &&
                  PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_MODE)) ? 1 : 0;
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_MII_BUF_CNTL_STAT1r(unit, pc, &data));
        *value = ((data & 0x0004) != 0);
        break;

    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_MII_BUF_CNTL_STAT1r(unit, pc, &data));
        *value = ((data & 0x0700) >> 8);
        break;

    case SOC_PHY_CONTROL_EEE_TRANSMIT_EVENTS:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN
            (PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 0x0, 1U<<14)); /* exp af */
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_EEE_TX_EVENTSr(unit, pc, &data));
        temp = data;
        SOC_IF_ERROR_RETURN
            (PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 1U<<14, 1U<<14)); /* exp af */
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_EEE_TX_EVENTSr(unit, pc, &data));
        temp |= (data << 16);
        *value = temp;

        break;

    case SOC_PHY_CONTROL_EEE_TRANSMIT_DURATION:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN
            (PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 0x0, 1U<<14)); /* exp af */
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_EEE_TX_DURATIONr(unit, pc, &data));
        temp = data;
        SOC_IF_ERROR_RETURN
            (PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 1U<<14, 1U<<14)); /* exp af */
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_EEE_TX_DURATIONr(unit, pc, &data));
        temp |= (data << 16);
        *value = temp;

        break;

    case SOC_PHY_CONTROL_EEE_RECEIVE_EVENTS:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN
            (PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 0x0, 1U<<14)); /* exp af */
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_EEE_RX_EVENTSr(unit, pc, &data));
        temp = data;
        SOC_IF_ERROR_RETURN
            (PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 1U<<14, 1U<<14)); /* exp af */
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_EEE_RX_EVENTSr(unit, pc, &data));
        temp |= (data << 16);
        *value = temp;

        break;

    case SOC_PHY_CONTROL_EEE_RECEIVE_DURATION:
        if (!PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            rv = SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN
            (PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 0x0, 1U<<14)); /* exp af */
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_EEE_RX_DURATIONr(unit, pc, &data));
        temp = data;
        SOC_IF_ERROR_RETURN
            (PHY54616_REG_MODIFY(unit, pc, 0x00, 0x0FAF,0x15, 1U<<14, 1U<<14)); /* exp af */
        SOC_IF_ERROR_RETURN
            (READ_PHY54618_EXP_EEE_RX_DURATIONr(unit, pc, &data));
        temp |= (data << 16);
        *value = temp;
        break;


    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}

/*
 * Function:
 *      phy_54616_cable_diag
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
phy_54616_cable_diag(int unit, soc_port_t port,
                    soc_port_cable_diag_t *status)
{
    int                 rv=SOC_E_NONE, rv2, i;

    extern int phy_5464_cable_diag_sw(int, soc_port_t , 
                                      soc_port_cable_diag_t *);

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
        rv2 = _phy_54616_reset_setup(unit, port);
    }
    if (rv >= 0 && rv2 < 0) {
        return rv2;
    }
    return rv;
}

/*---------------- BCM54618SE TimeSync support ----------------------------------*/

void _phy_54618e_encode_egress_message_mode(soc_port_phy_timesync_event_message_egress_mode_t mode,
                                            int offset, uint16 *value)
{
    switch ( mode ) {
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE:
        *value |= (0x0 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD:
        *value |= (0x1 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN:
        *value |= (0x2 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP:
        *value |= (0x3 << offset);
        break;
    default:
        break;
    }
}

void _phy_54618e_encode_ingress_message_mode(soc_port_phy_timesync_event_message_ingress_mode_t mode,
                                             int offset, uint16 *value)
{
    switch ( mode ) {
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE:
        *value |= (0x0 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD:
        *value |= (0x1 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP:
        *value |= (0x2 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME:
        *value |= (0x3 << offset);
        break;
    default:
        break;
    }
}

void _phy_54618e_decode_egress_message_mode(uint16 value, int offset,
                                            soc_port_phy_timesync_event_message_egress_mode_t *mode)
{
    switch ( (value >> offset) & 0x3 ) {
    case 0x0:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE;
        break;
    case 0x1:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD;
        break;
    case 0x2:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN;
        break;
    case 0x3:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP;
        break;
    }
}

void _phy_54618e_decode_ingress_message_mode(uint16 value, int offset,
                                             soc_port_phy_timesync_event_message_ingress_mode_t *mode)
{
    switch ( (value >> offset) & 0x3 ) {
    case 0x0:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE;
        break;
    case 0x1:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD;
        break;
    case 0x2:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP;
        break;
    case 0x3:
        *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME;
        break;
    }
}

void _phy_54618e_encode_gmode(soc_port_phy_timesync_global_mode_t mode,
                              uint16 *value)
{
    switch ( mode ) {
    case SOC_PORT_PHY_TIMESYNC_MODE_FREE:
        *value = 0x1;
        break;
    case SOC_PORT_PHY_TIMESYNC_MODE_SYNCIN:
        *value = 0x2;
        break;
    case SOC_PORT_PHY_TIMESYNC_MODE_CPU:
        *value = 0x3;
        break;
    default:
        break;
    }
}

void _phy_54618e_decode_gmode(uint16 value,
                              soc_port_phy_timesync_global_mode_t *mode)
{
    switch ( value & 0x3 ) {
    case 0x1:
        *mode = SOC_PORT_PHY_TIMESYNC_MODE_FREE;
        break;
    case 0x2:
        *mode = SOC_PORT_PHY_TIMESYNC_MODE_SYNCIN;
        break;
    case 0x3:
        *mode = SOC_PORT_PHY_TIMESYNC_MODE_CPU;
        break;
    default:
        break;
    }
}

void _phy_54618e_encode_framesync_mode(soc_port_phy_timesync_framesync_mode_t mode,
                                       uint16 *value)
{
    switch ( mode ) {
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0:
        *value = 1U << 0;
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1:
        *value = 1U << 1;
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT:
        *value = 1U << 2;
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU:
        *value = 1U << 3;
        break;
    default:
        break;
    }
}

void _phy_54618e_decode_framesync_mode(uint16 value,
                                       soc_port_phy_timesync_framesync_mode_t *mode)
{
    switch ( value & 0xf ) {
    case 1U << 0:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0;
        break;
    case 1U << 1:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1;
        break;
    case 1U << 2:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT;
        break;
    case 1U << 3:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU;
        break;
    default:
        break;
    }
}

void _phy_54618e_encode_syncout_mode(soc_port_phy_timesync_syncout_mode_t mode,
                                     uint16 *value)
{
    switch ( mode ) {
    case SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE:
        *value = 0x0;
        break;
    case SOC_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME:
        *value = 0x1;
        break;
    case SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN:
        *value = 0x2;
        break;
    case SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC:
        *value = 0x3;
        break;
    default:
        break;
    }
}

void _phy_54618e_decode_syncout_mode(uint16 value,
                                     soc_port_phy_timesync_syncout_mode_t *mode)
{
    switch ( value & 0x3 ) {
    case 0x0:
        *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE;
        break;
    case 0x1:
        *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME;
        break;
    case 0x2:
        *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN;
        break;
    case 0x3:
        *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC;
        break;
    }
}

STATIC int
phy_54618_timesync_config_set(int unit, soc_port_t port,
                              soc_port_phy_timesync_config_t *conf)
{
    phy_ctrl_t *pc;
    int         rv = SOC_E_NONE;
    uint16      rx_control_reg = 0, tx_control_reg = 0, rx_crc_control_reg = 0,
                en_control_reg = 0, tx_capture_en_reg = 0, rx_capture_en_reg = 0,
                nse_nco_6 = 0, nse_nco_2c = 0, value, mask,
                gmode = 0, framesync_mode = 0, syncout_mode = 0,
                pulse_1_length, pulse_2_length, length_threshold, event_offset;
    uint32      interval;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( ! PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
        return SOC_E_UNAVAIL;
    }

    do {
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FLAGS) {

            if (conf->flags & SOC_PORT_PHY_TIMESYNC_ENABLE) {
                en_control_reg |= ((1U << 1) | (1U << 0));
                nse_nco_6 |= (1U << 12); /* NSE init */
            }

            /* Top-Level Expansion Reg. 00h[1:0] IEEE 1588 Slice Enable Control */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x00, en_control_reg,
                                                   0x3), rv);
            /* Top-Level Expansion Reg. 03h[1:0]  1588 TX/RX SOP Timestamp Capture Enable */
            if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE) {
                tx_capture_en_reg |= (1U <<  0);
                rx_capture_en_reg |= (1U <<  1);
            }
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x03,
                        (tx_capture_en_reg | rx_capture_en_reg), 0x3), rv);

            /* Top-Level Expansion Register 31h[13:12]  NSE NCO Register 6 */
            if (conf->flags & SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE) {
                nse_nco_6 |= (1U << 13);    /* TS_CAPTURE */
            }
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x31, nse_nco_6, 
                                                   (0x3U << 12)), rv);

            /* Top-Level Expansion Register 14h: IEEE 1588 Tx/Rx CRC control register */
            if (conf->flags & SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE) {
                rx_crc_control_reg |= (1U << 3);
            }
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x14, rx_crc_control_reg), rv);

            /* Top-Level Expansion Register 12h: IEEE 1588 Transmit/Receive Control Register */
            if (conf->flags & SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE) {
                rx_control_reg |= (1U << 3);    /* RX_AS_EN */
                tx_control_reg |= (1U << 3);    /* TX_AS_EN */
            }

            if (conf->flags & SOC_PORT_PHY_TIMESYNC_L2_ENABLE) {
                rx_control_reg |= (1U << 2);    /* RX_L2_EN */
                tx_control_reg |= (1U << 2);    /* TX_L2_EN */
            }

            if (conf->flags & SOC_PORT_PHY_TIMESYNC_IP4_ENABLE) {
                rx_control_reg |= (1U << 1);    /* RX_IPV4_UDP_EN */
                tx_control_reg |= (1U << 1);    /* TX_IPV4_UDP_EN */
            }

            if (conf->flags & SOC_PORT_PHY_TIMESYNC_IP6_ENABLE) {
                rx_control_reg |= (1U << 0);    /* RX_IPV6_UDP_EN */
                tx_control_reg |= (1U << 0);    /* TX_IPV6_UDP_EN */
            }

            /* Top-Level Expansion Register 12h: IEEE 1588 Tx/Rx Control Register */
            SOC_IF_ERROR_BREAK        /*  Rx[15:8] ,  Tx[7:0]  */ 
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x12,
                                ((rx_control_reg << 8) | tx_control_reg)), rv);

            /* Top-Level Expansion Register 26h[14]  NSE NCO Register 2-1 */
            if (conf->flags & SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT) {
                nse_nco_2c &= (uint16) ~(1U << 14);
            } else {
                nse_nco_2c |= (1U << 14);
            }
            SOC_IF_ERROR_BREAK       /* FREQ_mdio_sel[14] */
                (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x26, nse_nco_2c, 
                                               (1U << 14)), rv);
        }

        /* Top-Level Expansion Register 15h: VLAN1 Tags ITPID Register */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x15, conf->itpid), rv);
        }

        /* Top-Level Expansion Register 16h: VLAN2 Tags OTPID Register */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x16, conf->otpid), rv);
        }

        /* Top-Level Expansion Register 17h: VLAN2 Tags OTPID2 Register */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x17, conf->otpid2), rv);
        }

        /* Top-Level Expansion Register 2Dh[11:0] NCO Register 4, nse_reg_ts_divider */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x2d, conf->ts_divider & 0xfff), rv);
        }

        /* Top-Level Expansion Register 5Ch/5Dh: Linkdelay1/2 */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x5c, conf->rx_link_delay & 0xffff), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x5d, (conf->rx_link_delay >> 16) & 0xffff), rv);
        }

        /* Top-Level Expansion Register 07h~0Bh: IEEE 1588 Original Time Code */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x07, (uint16)(conf->original_timecode.nanoseconds & 0xffff)), rv);

            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x08, (uint16)((conf->original_timecode.nanoseconds >> 16) & 0xffff)), rv);

            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x09, (uint16)(COMPILER_64_LO(conf->original_timecode.seconds) & 0xffff)), rv);

            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x0a, (uint16)((COMPILER_64_LO(conf->original_timecode.seconds) >> 16) & 0xffff)), rv);

            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x0b, (uint16)(COMPILER_64_HI(conf->original_timecode.seconds) & 0xffff)), rv);

        }
        mask = 0;

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE) {
            _phy_54618e_encode_gmode(conf->gmode,&gmode);
            mask |= (0x3 << 14);
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE) {
            _phy_54618e_encode_framesync_mode(conf->framesync.mode, &framesync_mode);
            mask |= (0xf << 2) |  (0x1 << 11);

            /* Divide by 8 */
            length_threshold = conf->framesync.length_threshold >> 3;
            event_offset = conf->framesync.event_offset >> 3;

            /* Top-Level Expansion Register 32h, 1588 Length Threshold */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x32, length_threshold), rv);
            /* Top-Level Expansion Register 33h, 1588 Event Offset */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x33, event_offset), rv);
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE) {
            _phy_54618e_encode_syncout_mode(conf->syncout.mode, &syncout_mode);
            mask |= (0x3 << 0);

            /* Divide by 8 */
            interval = conf->syncout.interval >> 3;
            pulse_1_length = conf->syncout.pulse_1_length >> 3; /* framesync pulse length */
            pulse_2_length = conf->syncout.pulse_2_length >> 3; /* pulse train length */

            /* Top-Level Expansion Register 29h[15:0], 2Ah[13:0]. Interval_Length */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x29,
                                                   interval & 0xffff), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x2a,
                                                   (interval >> 16) & 0x3fff), rv);

            /* Top-Level Expansion Register 2bh[8:0], PULSE_TRAIN_length */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x2b,
                                                   (pulse_2_length & 0x1ff)), rv);
            /* Top-Level Expansion Register 2ch[8:0], FRMSYNC_PULSE_length */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x2c,
                                                   (pulse_1_length & 0x1ff)), rv);

            /* Top-Level Expansion Register 2Eh~30h. Synout_ts_reg */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x2e, 
                    (uint16)(COMPILER_64_LO(conf->syncout.syncout_ts) & 0xffff)), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x2f, 
                    (uint16)((COMPILER_64_LO(conf->syncout.syncout_ts) >> 16) & 0xffff)), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x30, 
                    (uint16)(COMPILER_64_HI(conf->syncout.syncout_ts) & 0xffff)), rv);
        }

        /* Top-Level Expansion Reg.31h,  NSE NCO Reg. 6 */
        /*           GMODE[15:14], FRAMESYNC_MODE[5:2], SYNCOUT_mode[1:0] */
        SOC_IF_ERROR_BREAK
            (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x31, (gmode << 14) |
                ((framesync_mode & 0x2) ? (1U << 11) : (1U << 6)) | /* if bits 1 or 2 are set, turn off syncout */
                (framesync_mode << 2) | (syncout_mode << 0), mask), rv);

        /* Top-Level Expansion Register 05h[11:0] IEEE 1588 Tx Timestamp Offset */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET) {
            value  = (uint16) (conf->tx_timestamp_offset & 0x0fff);       /* TS_OFFSET_TX[11:0] */
            value |= (uint16)((conf->rx_timestamp_offset >> 4) & 0xf000); /* TS_OFFSET_RX_MSB[15:12] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x05, value), rv);
        }
        /* Top-Level Expansion Register 05h[15:12]+06h[15:0] IEEE 1588 Rx Timestamp Offset */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write( unit, pc, 0, 0x06,
                                       (uint16)(conf->rx_timestamp_offset & 0xffff) ), rv);
        }

        /* Tx Event Message Mode Selection */
        value = 0;
        mask = 0;

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE) {
            _phy_54618e_encode_egress_message_mode(conf->tx_sync_mode, 0, &value);
            mask |= 0x3 << 0;
        }
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE) {
            _phy_54618e_encode_egress_message_mode(conf->tx_delay_request_mode, 2, &value);
            mask |= 0x3 << 2;
        }
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE) {
            _phy_54618e_encode_egress_message_mode(conf->tx_pdelay_request_mode, 4, &value);
            mask |= 0x3 << 4;
        }
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE) {
            _phy_54618e_encode_egress_message_mode(conf->tx_pdelay_response_mode, 6, &value);
            mask |= 0x3 << 6;
        }

        /* Top-Level Expansion Register 01h: IEEE 1588 TX Event Message Mode Selection */
        SOC_IF_ERROR_BREAK
            (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x01, value, mask), rv); 

        /* Rx Event Message Mode Selection */
        value = 0;
        mask = 0;

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE) {
            _phy_54618e_encode_ingress_message_mode(conf->rx_sync_mode, 0, &value);
            mask |= 0x3 << 0;
        }
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE) {
            _phy_54618e_encode_ingress_message_mode(conf->rx_delay_request_mode, 2, &value);
            mask |= 0x3 << 2;
        }
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE) {
            _phy_54618e_encode_ingress_message_mode(conf->rx_pdelay_request_mode, 4, &value);
            mask |= 0x3 << 4;
        }
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE) {
            _phy_54618e_encode_ingress_message_mode(conf->rx_pdelay_response_mode, 6, &value);
            mask |= 0x3 << 6;
        }

        /* Top-Level Expansion Register 02h: IEEE 1588 RX Event Message Mode Selection */
        SOC_IF_ERROR_BREAK
            (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x02, value, mask), rv); 

        /* Top-Level Expansion Register 19h~1Bh: NSE DPLL Register 2, REF_PHASE */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {
            /* Initial ref phase [15:0] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x19, (uint16)(COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) & 0xffff)), rv);

            /* Initial ref phase [31:16]  */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x1a, (uint16)((COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) >> 16) & 0xffff)), rv);

            /* Initial ref phase [47:32] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x1b, (uint16)(COMPILER_64_HI(conf->phy_1588_dpll_ref_phase) & 0xffff)), rv);
        }

        /* Top-Level Expansion Register 1Ch~1Dh: NSE DPLL Register 3, REF_PHASE_DELTA */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA) {
            /* Ref phase delta [15:0] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x1c, (uint16)(conf->phy_1588_dpll_ref_phase_delta & 0xffff)), rv);

            /* Ref phase delta [31:16]  */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x1d, (uint16)((conf->phy_1588_dpll_ref_phase_delta >> 16) & 0xffff)), rv);
        }

        /* DPLL K1 & K2 */
        mask = 0;
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
            mask |= 0xff;         /* bit[7:0]   */
        }
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
            mask |= 0xff << 8;    /* bit[15:8] */
        }
        if ( mask != 0 ) {
            /* Top-Level Expansion Register 1Eh: NSE DPLL K1/K2 */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x1e, (conf->phy_1588_dpll_k1 & 0xff)<<8 | (conf->phy_1588_dpll_k2 & 0xff), mask ), rv);
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
            /* Top-Level Expansion Register 1Fh: NSE DPLL K3 */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x1f, conf->phy_1588_dpll_k3 & 0xff), rv);
        }

        /* Top-Level Expansion Register 20h~23h: NSE DPLL Register 7, LOOP_FILTER */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER) {
            /* Initial loop filter[15:0] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x20, (uint16)(COMPILER_64_LO(conf->phy_1588_dpll_loop_filter) & 0xffff)), rv);

            /* Initial loop filter[31:16]  */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x21, (uint16)((COMPILER_64_LO(conf->phy_1588_dpll_loop_filter) >> 16) & 0xffff)), rv);

            /*  Initial loop filter[47:32] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x22, (uint16)(COMPILER_64_HI(conf->phy_1588_dpll_loop_filter) & 0xffff)), rv);

            /* Initial loop filter[63:48]  */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x23, (uint16)((COMPILER_64_HI(conf->phy_1588_dpll_loop_filter) >> 16) & 0xffff)), rv);
        }
    } while ( 0 );

    return SOC_E_NONE;
}

STATIC int
phy_54618_timesync_config_get(int unit, soc_port_t port,
                              soc_port_phy_timesync_config_t *conf)
{
    int    rv = SOC_E_NONE;
    uint16 tx_control_reg = 0, rx_crc_control_reg = 0,
           en_control_reg = 0, tx_capture_en_reg = 0,
           nse_nco_6 = 0, nse_nco_2c = 0, temp1, temp2, temp3, temp4, value;
    phy_ctrl_t *pc;
    soc_port_phy_timesync_global_mode_t gmode = SOC_PORT_PHY_TIMESYNC_MODE_FREE;
    soc_port_phy_timesync_framesync_mode_t framesync_mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_NONE;
    soc_port_phy_timesync_syncout_mode_t syncout_mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( ! PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
        return SOC_E_UNAVAIL;
    }
    conf->flags = 0;

    do {
        /* Top-Level Expansion Register 31h  NSE NCO Register 6 */
        SOC_IF_ERROR_BREAK
            (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x31, &nse_nco_6), rv);

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FLAGS) {
            /* Top-Level Expansion Reg. 00h[1:0] IEEE 1588 Slice Enable Control */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x00, &en_control_reg), rv); 

            if (en_control_reg & (1U << 0)) {
                conf->flags |= SOC_PORT_PHY_TIMESYNC_ENABLE;
            }

            /* Top-Level Expansion Reg. 03h[1:0]  1588 Rx/Tx SOP Timestamp Capture Enable */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x03, &tx_capture_en_reg), rv); 
            if (tx_capture_en_reg & (1U << 0)) {    /* bit[0] Tx TS Capture Enable */
                conf->flags |= SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;
            }

            if (nse_nco_6 & (1U << 13)) {    /* TopLvl Exp. 31h[13]: TS_CAPTURE */
                conf->flags |= SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE;
            }

            /* Top-Level Expansion Register 14h: IEEE 1588 Tx/Rx CRC control register */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x14, &rx_crc_control_reg), rv);

            if (rx_crc_control_reg & (1U << 3)) {
                conf->flags |= SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE;
            }

            /* Top-Level Expansion Register 12h: IEEE 1588 Tx/Rx Control Register */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x12, &tx_control_reg), rv);
                                              
            if (tx_control_reg & (1U << 3)) {
                conf->flags |= SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE;
            }

            if (tx_control_reg & (1U << 2)) {
                conf->flags |= SOC_PORT_PHY_TIMESYNC_L2_ENABLE;
            }

            if (tx_control_reg & (1U << 1)) {
                conf->flags |= SOC_PORT_PHY_TIMESYNC_IP4_ENABLE;
            }

            if (tx_control_reg & (1U << 0)) {
                conf->flags |= SOC_PORT_PHY_TIMESYNC_IP6_ENABLE;
            }

            /* Top-Level Expansion Register 26h[14]  NSE NCO Register 2-1 */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x26, &nse_nco_2c), rv);

            if (!(nse_nco_2c & (1U << 14))) {    /* FREQ_mdio_sel[14] */
                conf->flags |= SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;
            }
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE) {
            _phy_54618e_decode_gmode((nse_nco_6  >> 14),&gmode);
            conf->gmode = gmode;
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE) {
            _phy_54618e_decode_framesync_mode((nse_nco_6  >> 2), &framesync_mode);
            conf->framesync.mode = framesync_mode;
            /* Top-Level Expansion Register 32h, 1588 Length Threshold */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x32, &temp1), rv);
            /* Top-Level Expansion Register 33h, 1588 Event Offset */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x33, &temp2), rv);

            /* Multiply by 8 */
            conf->framesync.length_threshold = temp1 << 3;
            conf->framesync.event_offset     = temp2 << 3;
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE) {
            _phy_54618e_decode_syncout_mode((nse_nco_6  >> 0), &syncout_mode);
            conf->syncout.mode = syncout_mode;

            /* Top-Level Expansion Register 29h[15:0], 2Ah[13:0]. Interval_Length */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x29, &temp1), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x2a, &temp2), rv);
            conf->syncout.interval =  ((temp2 & 0x3ff)<< 16) | temp1;

            /* Top-Level Expansion Register 2bh[8:0], PULSE_TRAIN_length */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x2b, &temp2), rv);
            /* Top-Level Expansion Register 2ch[8:0], FRMSYNC_PULSE_length */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x2c, &temp1), rv);

            /* Multiply by 8  (unit = 8ns) */
            conf->syncout.pulse_1_length = (temp1 & 0x1ff) << 3;  /* FrmSync pulse length */
            conf->syncout.pulse_2_length = (temp2 & 0x1ff) << 3;    /* pulse train length */

            /* Top-Level Expansion Register 2Eh~30h. Synout_ts_reg (48 bits) */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x2e, &temp1), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x2f, &temp2), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x30, &temp3), rv);

            COMPILER_64_SET( conf->syncout.syncout_ts, (uint32) temp3,
                             ((uint32) temp2 << 16) | ((uint32) temp1) );
        }

        /* Top-Level Expansion Register 15h: VLAN1 Tags ITPID Register */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x15, &conf->itpid), rv);
        }

        /* Top-Level Expansion Register 16h: VLAN2 Tags OTPID Register */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x16, &conf->otpid), rv);
        }

        /* Top-Level Expansion Register 17h: VLAN2 Tags OTPID2 Register */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x17, &conf->otpid2), rv);
        }

        /* Top-Level Expansion Register 2Dh[11:0] NCO Register 4, nse_reg_ts_divider */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x2d, &conf->ts_divider), rv);
        }

        /* Top-Level Expansion Register 5Ch/5Dh: Linkdelay1/2 */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x5c, &temp1), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x5d, &temp2), rv);

            conf->rx_link_delay = ((uint32)temp2 << 16) | temp1; 
        }

        /* Top-Level Expansion Register 07h~0Bh: IEEE 1588 Original Time Code */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x07, &temp1), rv);

            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x08, &temp2), rv);

            conf->original_timecode.nanoseconds = ((uint32)temp2 << 16) | temp1; 

            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x09, &temp1), rv);

            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x0a, &temp2), rv);

            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x0b, &temp3), rv);

            COMPILER_64_SET(conf->original_timecode.seconds, ((uint32)temp3),  (((uint32)temp2<<16)|((uint32)temp1)));
        }

        /* Top-Level Expansion Register 05h[11:0] IEEE 1588 Tx Timestamp Offset */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x05, &temp2), rv);
            conf->tx_timestamp_offset = temp2 & 0x0fff;
        }

        /* Top-Level Expansion Register 05h[15:12]+06h[15:0] IEEE 1588 Rx Timestamp Offset */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x06, &temp1), rv);
            conf->rx_timestamp_offset  = (uint32) temp1;                    /* TS_OFFSET_RX[15:0] */
            conf->rx_timestamp_offset |= ((uint32) (temp2 & 0xf000)) << 4 ; /* TS_OFFSET_RX_MSB[15:12] */
        }

        /* Top-Level Expansion Register 01h: IEEE 1588 TX Event Message Mode Selection */
        SOC_IF_ERROR_BREAK
            (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x01, &value), rv); 

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE) {
            _phy_54618e_decode_egress_message_mode(value, 0, &conf->tx_sync_mode);
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE) {
            _phy_54618e_decode_egress_message_mode(value, 2, &conf->tx_delay_request_mode);
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE) {
            _phy_54618e_decode_egress_message_mode(value, 4, &conf->tx_pdelay_request_mode);
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE) {
            _phy_54618e_decode_egress_message_mode(value, 6, &conf->tx_pdelay_response_mode);
        }

        /* Top-Level Expansion Register 02h: IEEE 1588 RX Event Message Mode Selection */
        SOC_IF_ERROR_BREAK
            (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x02, &value), rv); 

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE) {
            _phy_54618e_decode_ingress_message_mode(value, 0, &conf->rx_sync_mode);
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE) {
            _phy_54618e_decode_ingress_message_mode(value, 2, &conf->rx_delay_request_mode);
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE) {
            _phy_54618e_decode_ingress_message_mode(value, 4, &conf->rx_pdelay_request_mode);
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE) {
            _phy_54618e_decode_ingress_message_mode(value, 6, &conf->rx_pdelay_response_mode);
        }

        /* Top-Level Expansion Register 19h~1Bh: NSE DPLL Register 2, REF_PHASE */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {
            /* Initial ref phase [15:0] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x19, &temp1), rv);

            /* Initial ref phase [31:16]  */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x1a, &temp2), rv);

            /*  Initial ref phase [47:32] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x1b, &temp3), rv);

            COMPILER_64_SET(conf->phy_1588_dpll_ref_phase, ((uint32)temp3), (((uint32)temp2<<16)|((uint32)temp1)));
        }

        /* Top-Level Expansion Register 1Ch~1Dh: NSE DPLL Register 3, REF_PHASE_DELTA */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA) {
            /* Ref phase delta [15:0] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x1c, &temp1), rv);

            /* Ref phase delta [31:16]  */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x1d, &temp2), rv);

            conf->phy_1588_dpll_ref_phase_delta = (temp2 << 16) | temp1;
        }

        /* Top-Level Expansion Register 1Eh: NSE DPLL K1/K2 */
        SOC_IF_ERROR_BREAK
            (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x1e, &temp1 ), rv);

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
            conf->phy_1588_dpll_k1 = (temp1 >> 8 ) & 0xff;
        }

        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
            conf->phy_1588_dpll_k2 = temp1 & 0xff;
        }

        /* Top-Level Expansion Register 1Fh: NSE DPLL K3 */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x1f, &temp1), rv);
            conf->phy_1588_dpll_k3 = temp1 & 0xff;
        }

        /* Top-Level Expansion Register 20h~23h: NSE DPLL Register 7, LOOP_FILTER */
        if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER) {
            /* Initial loop filter[15:0] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x20, &temp1), rv);

            /* Initial loop filter[31:16]  */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x21, &temp2), rv);

            /*  Initial loop filter[47:32] */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x22, &temp3), rv);

            /* Initial loop filter[63:48]  */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x23, &temp4), rv);

            COMPILER_64_SET(conf->phy_1588_dpll_loop_filter, (((uint32)temp4<<16)|((uint32)temp3)), (((uint32)temp2<<16)|((uint32)temp1)));
        }
    } while ( 0 );
                                              
    return SOC_E_NONE;
}

STATIC int
phy_54618_timesync_control_set(int unit, soc_port_t port,
                               soc_port_control_phy_timesync_t type, uint64 value)
{
    uint16       temp1, temp2;
    uint32       value0;
    int          rv = SOC_E_NONE;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( ! PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
        return SOC_E_UNAVAIL;
    }

    do {
        switch ( type ) {
        case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:
        case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:
            return SOC_E_UNAVAIL;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
            value0 = COMPILER_64_LO(value);    /* nse_reg_nco_freqcntrl[31:0] */
            /* Top-Level Expansion Register 24h~25h: NSE NCO Register 1, NSE_REQ_NCO_FREQCNTRL */
            SOC_IF_ERROR_BREAK                 /* nse_reg_nco_freqcntrl[15:0]  LSB */
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x24, (uint16) (value0 & 0xffff)), rv);
            SOC_IF_ERROR_BREAK                 /* nse_reg_nco_freqcntrl[31:16] MSB */
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x25, (uint16) ((value0 >> 16) & 0xffff)), rv);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC:
            /* Top-Level Expansion Register 31h  NSE NCO Register 6 */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x31, &temp1), rv);
            temp2 = ((temp1 | (0x3 << 14) | (0x1 << 12)) & ~(0xf << 2)) | (0x1 << 5);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x31, temp2), rv);
            sal_udelay(1);
            temp2 &= ~((0x1 << 5) | (0x1 << 12));
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x31, temp2), rv);
            sal_udelay(1);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x31, temp1), rv);

            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
            COMPILER_64_SHR(value, 4);
            /* Top-Level Expansion Register 26h~28h  NSE NCO Register 2, Local Time */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x28, (uint16)(COMPILER_64_LO(value) & 0xffff)), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x27, (uint16)((COMPILER_64_LO(value) >> 16) & 0xffff)), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x26, (uint16)(COMPILER_64_HI(value) & 0x0fff), 0x0fff), rv);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:
            temp1 = 0;
            temp2 = 0;
            value0 = COMPILER_64_LO(value);

            if (value0 &  SOC_PORT_PHY_TIMESYNC_TN_LOAD) {
                temp1 |= 1U << 11;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD) {
                temp2 |= 1U << 11;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD) {
                temp1 |= 1U << 10;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD) {
                temp2 |= 1U << 10;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD) {
                temp1 |= 1U << 9;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD) {
                temp2 |= 1U << 9;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD) {
                temp1 |= 1U << 8;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD) {
                temp2 |= 1U << 8;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD) {
                temp1 |= 1U << 7;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD) {
                temp2 |= 1U << 7;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD) {
                temp1 |= 1U << 6;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD) {
                temp2 |= 1U << 6;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD) {
                temp1 |= 1U << 5;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD) {
                temp2 |= 1U << 5;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD) {
                temp1 |= 1U << 4;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD) {
                temp2 |= 1U << 4;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD) {
                temp1 |= 1U << 3;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD) {
                temp2 |= 1U << 3;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD) {
                temp1 |= 1U << 2;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD) {
                temp2 |= 1U << 2;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD) {
                temp1 |= 1U << 1;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD) {
                temp2 |= 1U << 1;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD) {
                temp1 |= 1U << 0;
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD) {
                temp2 |= 1U << 0;
            }
            /* Top-Level Expansion Register 0Fh: IEEE 1588 Shadow Register Control */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x0f, temp1), rv);
            /* Top-Level Expansion Register 10h: IEEE 1588 Shadow Register Load */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x10, temp2), rv);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK:
            temp1 = 0;
            value0 = COMPILER_64_LO(value);

            if (value0 &  SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK) {
                temp1 |= 1U << 9;    /* bit[9] INTC_SOP_MASK */
            }
            if (value0 &  SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK) {
                temp1 |= 1U << 8;    /* bit[8] INTC_FSYNC_MASK */
            }
            /* Top-Level Expansion Register 11h: IEEE 1588 Interrupt/Interrupt Mask */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write(unit, pc, 0, 0x11, temp1), rv);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
            value0 = COMPILER_64_LO(value);

            /* Top-Level Expansion Register 05h[11:0] IEEE 1588 Tx Timestamp Offset */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x05, (uint16)(value0 & 0x0fff), 0x0fff), rv);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
            value0 = COMPILER_64_LO(value);

            /* Top-Level Expansion Register 05h[15:12]+06h[15:0] IEEE 1588 Rx Timestamp Offset */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_modify(unit, pc, 0, 0x05, (uint16)(value0 & 0xf000), 0xf000), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_write( unit, pc, 0, 0x06, (uint16)(value0 & 0xffff)), rv);

            break;

        default:
            return SOC_E_UNAVAIL;
        }
    } while ( 0 );

    return SOC_E_NONE;
}

STATIC int
phy_54618_timesync_control_get(int unit, soc_port_t port,
                               soc_port_control_phy_timesync_t type, uint64 *value)
{
    uint16       value0, value1, value2;
    int          rv = SOC_E_NONE;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    if ( ! PHY_IS_BCM54618SE_OR_BCM54618E(pc) ) {
        return SOC_E_UNAVAIL;
    }

    do {
        switch ( type ) {
        case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:
            /* Start a Heartbeat Read. Expansion Reg. F3h[0], Heartbeat_Start=1 */
            SOC_IF_ERROR_BREAK
                (WRITE_PHY54618_EXP_READ_START_END_CTRLr(unit, pc, 0x1), rv);

            /* Top-Level Expansion Register 57h~55h: Heartbeat Timestamp (48 bits) */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x55, &value0), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x56, &value1), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x57, &value2), rv);

            /* Stop a Heartbeat Read. Expansion Reg. F3h[1], Heartbeat_Stop=1 */
            SOC_IF_ERROR_BREAK
                (WRITE_PHY54618_EXP_READ_START_END_CTRLr(unit, pc, 0x2), rv);
            SOC_IF_ERROR_BREAK
                (WRITE_PHY54618_EXP_READ_START_END_CTRLr(unit, pc, 0x0), rv);

            COMPILER_64_SET((*value), ((uint32)value2), (((uint32)value1<<16) | ((uint32)value0)));
            /*  *value =  (((uint64)value2) << 32) | (((uint64)value1) << 16) | ((uint64)value0); */
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:
            /* Start a Timestamp Read. Expansion Reg. F3h[2], TS_START=1 */
            SOC_IF_ERROR_BREAK
                (WRITE_PHY54618_EXP_READ_START_END_CTRLr(unit, pc, 0x4), rv);

            /* Top-Level Expansion Register 52h~50h: SOP Timestamp (48 bits) */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x50, &value0), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x51, &value1), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x52, &value2), rv);

            /* Start a Timestamp Read. Expansion Reg. F3h[3], TS_END=1 */
            SOC_IF_ERROR_BREAK
                (WRITE_PHY54618_EXP_READ_START_END_CTRLr(unit, pc, 0x8), rv);
            SOC_IF_ERROR_BREAK
                (WRITE_PHY54618_EXP_READ_START_END_CTRLr(unit, pc, 0x0), rv);

            COMPILER_64_SET((*value), ((uint32)value2), (((uint32)value1<<16) | ((uint32)value0)));
            /*  *value =  (((uint64)value2) << 32) | (((uint64)value1) << 16) | ((uint64)value0); */
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
            /* Top-Level Expansion Register 24h~25h: NSE NCO Register 1, NSE_REQ_NCO_FREQCNTRL */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x24, &value0), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x25, &value1), rv);
            COMPILER_64_SET( (*value), 0,  ((uint32)value1 << 16) | (uint32)value0 );
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
            /* Top-Level Expansion Register 26h~28h  NSE NCO Register 2, Local Time (44 bits) */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x28, &value0), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x27, &value1), rv);
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x26, &value2), rv);
            /*  Local_Time_MSB             [43:32]               [31:16]          [15:0] */
            COMPILER_64_SET((*value), (value2 & 0x0fff), ((uint32)value1 << 16) | value0 );
            COMPILER_64_SHL((*value), 4);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:
            /* Top-Level Expansion Register 0Fh: IEEE 1588 Shadow Register Control */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x0f, &value1), rv);
            /* Top-Level Expansion Register 10h: IEEE 1588 Shadow Register Load */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x10, &value2), rv);

            value0 = 0;
            if (value1 & (1U << 11)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_TN_LOAD;
            }
            if (value2 & (1U << 11)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD;
            }
            if (value1 & (1U << 10)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD;
            }
            if (value2 & (1U << 10)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD;
            }
            if (value1 & (1U << 9)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD;
            }
            if (value2 & (1U << 9)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD;
            }
            if (value1 & (1U << 8)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD;
            }
            if (value1 & (1U << 8)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD;
            }
            if (value1 & (1U << 7)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD;
            }
            if (value1 & (1U << 7)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD;
            }
            if (value1 & (1U << 6)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD;
            }
            if (value2 & (1U << 6)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD;
            }
            if (value1 & (1U << 5)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD;
            }
            if (value2 & (1U << 5)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD;
            }
            if (value1 & (1U << 4)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD;
            }
            if (value2 & (1U << 4)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD;
            }
            if (value1 & (1U << 3)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD;
            }
            if (value2 & (1U << 3)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD;
            }
            if (value1 & (1U << 2)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD;
            }
            if (value2 & (1U << 2)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD;
            }
            if (value1 & (1U << 1)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD;
            }
            if (value2 & (1U << 1)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD;
            }
            if (value1 & (1U << 0)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD;
            }
            if (value2 & (1U << 0)) {
                value0 |= (uint16) SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD;
            }
            COMPILER_64_SET((*value), 0, (uint32)value0);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT:
            /* Top-Level Expansion Register 11h: IEEE 1588 Interrupt/Interrupt Mask */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x11, &value1), rv);
            value0 = 0;
            if (value1 & (1U << 1)) {        /* bit[1] INTC_SOP */
                value0 |= SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT;
            }
            if (value1 & (1U << 0)) {        /* bit[0] INTC_FSYNC */
                value0 |= SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT;
            }
            COMPILER_64_SET((*value), 0, (uint32)value0);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK:
            /* Top-Level Expansion Register 11h: IEEE 1588 Interrupt/Interrupt Mask */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x11, &value1), rv);
            value0 = 0;
            if (value1 & (1U << 9)) {        /* bit[9] INTC_SOP_MASK */
                value0 |= SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK;
            }
            if (value1 & (1U << 8)) {        /* bit[8] INTC_FSYNC_MASK */
                value0 |= SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK;
            }
            COMPILER_64_SET((*value), 0, (uint32)value0);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
            /* Top-Level Expansion Register 05h[11:0] IEEE 1588 Tx Timestamp Offset */
            SOC_IF_ERROR_BREAK
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x05, &value0), rv);
            COMPILER_64_SET((*value), 0, ((uint32)value0 & 0x0fff));
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
            /* Top-Level Expansion Register 05h[15:12]+06h[15:0] IEEE 1588 Rx Timestamp Offset */
            SOC_IF_ERROR_BREAK                          /* TS_OFFSET_RX_MSB[15:12] */
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x05, &value1), rv);
            SOC_IF_ERROR_BREAK                          /* TS_OFFSET_RX[15:0] */
                (_phy_54618e_toplvl_exp_reg_read(unit, pc, 0, 0x06, &value0), rv);
            COMPILER_64_SET((*value), 0, (((uint32)value1 & 0xf000) << 4) | value0);
            break;

        default:
            return SOC_E_UNAVAIL;
        }
    } while ( 0 );

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54616_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */
STATIC int
phy_54616_probe(int unit, phy_ctrl_t *pc)
{
    uint16 id0, id1, data;
    soc_phy_info_t *pi;

    if (READ_PHY54616_MII_PHY_ID0r(unit, pc, &id0) < 0) {
        return SOC_E_NOT_FOUND;
    }
    if (READ_PHY54616_MII_PHY_ID1r(unit, pc, &id1) < 0) {
        return SOC_E_NOT_FOUND;
    }

    pi = &SOC_PHY_INFO(unit, pc->port);

    switch (PHY_MODEL(id0, id1)) {
    case PHY_BCM54616_MODEL:
        SOC_IF_ERROR_RETURN
            (READ_PHY54616_MODE_CTRLr(unit, pc, &data));
        if (data & (1U << 3)) {
            /* Serdes present so it's a 54616S */
            pi->phy_name = "BCM54616S";
            PHY_FLAGS_SET(pc->unit, pc->port, PHY_FLAGS_SECONDARY_SERDES);
        }
        break;

    case PHY_BCM54618E_MODEL:
    case PHY_BCM54618SE_MODEL:
        return SOC_E_NONE;

    default:
        return SOC_E_NOT_FOUND;
    }

    return SOC_E_NONE;
}


#ifdef BROADCOM_DEBUG

/*
 * Function:
 *      phy_54616_shadow_dump
 * Purpose:
 *      Debug routine to dump all shadow registers.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 */

void
phy_54616_shadow_dump(int unit, soc_port_t port)
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
 * Variable:    phy_54616drv_ge
 * Purpose:     PHY driver for 54616
 */

phy_driver_t phy_54616drv_ge = {
    "54616/801x Gigabit PHY Driver",
    phy_54616_init,
    phy_fe_ge_reset,
    phy_54616_link_get,
    phy_54616_enable_set,
    phy_54616_enable_get,
    phy_54616_duplex_set,
    phy_54616_duplex_get,
    phy_54616_speed_set,
    phy_54616_speed_get,
    phy_54616_master_set,
    phy_54616_master_get,
    phy_54616_autoneg_set,
    phy_54616_autoneg_get,
    NULL, /* Local Advert set, deprecated */
    NULL, /* Local Advert get, deprecated */
    NULL, /* Remote Advert get, deprecated */
    phy_54616_lb_set,
    phy_54616_lb_get,
    phy_54616_interface_set,
    phy_54616_interface_get,
    NULL,
    NULL,                        /* Phy link up event */
    NULL,                        /* Phy link down event */
    phy_54616_mdix_set,
    phy_54616_mdix_get,
    phy_54616_mdix_status_get,
    phy_54616_medium_config_set,
    phy_54616_medium_config_get,
    phy_54616_medium_status,
    phy_54616_cable_diag,
    NULL,                       /* phy_link_change */
    phy_54616_control_set,       /* phy_control_set */ 
    phy_54616_control_get,       /* phy_control_get */
    phy_ge_reg_read,
    phy_ge_reg_write,
    phy_ge_reg_modify,
    NULL,                        /* Phy event notify */ 
    phy_54616_probe,          /* pd_probe  */
    phy_54616_ability_advert_set,  /* pd_ability_advert_set */
    phy_54616_ability_advert_get,  /* pd_ability_advert_get */
    phy_54616_ability_remote_get,  /* pd_ability_remote_get */
    phy_54616_ability_local_get,   /* pd_ability_local_get  */
    NULL, /* pd_firmware_set */
    phy_54618_timesync_config_set,
    phy_54618_timesync_config_get,
    phy_54618_timesync_control_set,
    phy_54618_timesync_control_get
};

#else /* INCLUDE_PHY_54616_ESW */
typedef int _soc_phy_54616_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_54616 || INCLUDE_PHY_54618SE || INCLUDE_PHY_54618E */

