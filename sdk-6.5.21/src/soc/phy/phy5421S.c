/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy5421S.c
 *
 * Please see sdk/doc/combo-ports.txt for more information.
 *
 * The BCM5421S connects to virtually any MAC interface and dynamically
 * supports both fiber and copper line interfaces.
 *
 * This driver allows the StrataSwitch to talk to the 5421S using either
 * the GMII interface or TBI (Ten Bit Interface).  Upon initialization,
 * one interface or the other is statically configured based on the
 * if_tbi_portXX property.  The MDIO must be connected for either mode.
 * The default is GMII and is recommended for both fiber and copper.
 *
 * When used with BCM5690, this driver switches to SGMII mode.  There is
 * also a special check in phy5690.c to enable autonegotiation for
 * SGMII.  WARNING: this configuration is not represented in any
 * Broadcom SDK, so proper operation cannot be guaranteed.
 *
 * This driver allows the 5421S to use either the copper or fiber line
 * interface.  If the phy_automedium property is true, the mode is
 * selected dynamically on link up using the energy/signal detect.
 * If both copper and fiber signals are detected, the driver will
 * consult the configuration property phy_fiber_pref<_portXX>.  It will
 * select fiber if the property is 1, and copper if it is 0.  The
 * default value of the property is 1, since fiber is more likely to be
 * preferred.
 *
 * If the phy_automedium property is false, then the mode is fixed
 * according to the phy_fiber_pref<_portXX> property.
 *
 * This driver does not support autonegotiation with TBI+copper.
 *
 * If LINK_DOWN_RESET_WAR is 1, then the PHY will be reset each time
 * link down is detected on copper.  BCM5421SA2 has difficulty obtaining
 * link with certain other gigabit PHYs if not reset when link goes down
 * (see errata).
 *
 * Fiber de-glitch: some GBICs may cause a brief fiber energy detect
 * when inserted, even without a link.  This could cause the copper link
 * to be dropped, so an energy detect de-glitch is provided.  The
 * property phy_fiber_deglitch_usecs is the de-glitch time in usec.
 *
 * Autoneg defaults may be different for copper and fiber.  The routines
 * phy_5421S_medium_config_set change the default port modes for a
 * specific medium type.  The default port modes are installed as soon
 * as possible after signal detect indicates a change of medium.  The
 * standard PHY set/get routines operate only on the currently active
 * medium.
 */

#ifdef INCLUDE_PHY_5421S

#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phy.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>
#include "phyident.h"
#include "phyreg.h"
#include "phyfege.h"
#include "phy5421S.h"

#ifndef INCLUDE_PHY_SERDES
#error "SERDES support required."
#endif

#define LINK_DOWN_RESET_WAR        1    /* Needed for A0, A1, A2 */
#define REGULATOR_GAMMA_WAR        1    /* Needed if using external regulators */
#define SERDES_COPPER_AMP_WAR        0    /* Needed for A0 only, NOT A1/A2 */
#define ADC_PERF_WAR                1    /* Needed for A0, A1, A2 */

#define DPRINTF(_s)             LOG_INFO(BSL_LS_SOC_PHY, _s)

#define GMII_MODE(unit, port)        (!PHY_TBI_MODE(unit, port))

#define ADVERT_ALL_COPPER \
        (SOC_PM_PAUSE | SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB)
#define ADVERT_ALL_FIBER \
        (SOC_PM_PAUSE | SOC_PM_1000MB_FD)

/*
 * See comments above about fiber energy de-glitch.  De-glitch time is
 * specified in microseconds but precision is limited to the linkscan
 * period.  A de-glitch time of 0 disables de-glitching.
 */

STATIC int phy_5421S_interface_set(int unit, soc_port_t port,
                                   soc_port_if_t pif);

STATIC int _phy_5421S_medium_config_update(int unit, soc_port_t port,
                                           soc_phy_config_t *cfg);

STATIC int phy_5421S_duplex_set(int unit, soc_port_t port, int duplex);
STATIC int phy_5421S_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_5421S_master_set(int unit, soc_port_t port, int master);
STATIC int phy_5421S_adv_local_set(int unit, soc_port_t port,
                                   soc_port_mode_t mode);
STATIC int phy_5421S_autoneg_set(int unit, soc_port_t port, int an);

STATIC int phy_5421S_link_down(int unit, soc_port_t port);
STATIC int phy_5421S_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);

/*
 * Function:
 *        _phy_5421S_medium_check
 * Purpose:
 *        Returns preferred medium based on current signal detects
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        medium - (OUT) SOC_PORT_MEDIUM_XXX
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        Optionally implements fiber signal de-glitch.
 *        Uses the preferred medium property and mode enables,
 *        combined with the energy signals that are present.
 */

STATIC int
_phy_5421S_medium_check(int unit, soc_port_t port, int *medium)
{
    phy_ctrl_t     *pc = EXT_PHY_SW_STATE(unit, port);
    uint16          tmp;
    int             fiber_detect, copper_detect;
    int             elapsed_time;

    /*
     * If medium automedium is not disabled, ignore the energy signals
     * and return a fixed medium type according to phy_fiber_pref.
     */

    if (!pc->automedium) {
        if (pc->fiber.preferred) {
            *medium = SOC_PORT_MEDIUM_FIBER;
        } else {
            *medium = SOC_PORT_MEDIUM_COPPER;
        }

        return SOC_E_NONE;
    }

    *medium = SOC_PORT_MEDIUM_NONE;

    SOC_IF_ERROR_RETURN
        (READ_PHY5421_SGMII_1000X_STATr(unit, pc, &tmp));

    copper_detect = pc->copper.enable && ((tmp & 0x8000) != 0);
    fiber_detect  = pc->fiber.enable  && ((tmp & 0x2000) != 0);

    /* De-glitch fiber detect */

    if (!fiber_detect) {
        pc->fiber_plug_wait = 0;
    } else if (!pc->fiber_plug_wait) {
        /* Fiber just now detected; start timer how long to ignore it */
        if (pc->fiber_plug_dely > 0) {
            pc->fiber_plug_wait = 1;
            pc->fiber_plug_time = sal_time_usecs();
            pc->fiber_plug_done = 0;
            fiber_detect = FALSE;
        }
    } else if (!pc->fiber_plug_done) {
        /* Ignoring fiber until timer expiry */
        elapsed_time = SAL_USECS_SUB(sal_time_usecs(), pc->fiber_plug_time);
        if ((elapsed_time < 0) ||
            (elapsed_time >= pc->fiber_plug_dely)) {
            pc->fiber_plug_done = 1;
        } else {
            fiber_detect = FALSE;
        }
    }

    /* Detect which medium should be active, if any */

    if (fiber_detect && copper_detect) {
        *medium = (pc->fiber.preferred ?
                   SOC_PORT_MEDIUM_FIBER :
                   SOC_PORT_MEDIUM_COPPER);
    } else if (fiber_detect) {
        *medium = SOC_PORT_MEDIUM_FIBER;
    } else if (copper_detect) {
        *medium = SOC_PORT_MEDIUM_COPPER;
    } else {
        *medium = SOC_PORT_MEDIUM_NONE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *        _phy_5421S_medium_select
 * Purpose:
 *        Program the PHY to copper or fiber mode based on specified medium.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        medium - One of:
 *                SOC_PORT_MEDIUM_COPPER
 *                SOC_PORT_MEDIUM_FIBER
 *                SOC_PORT_MEDIUM_NONE
 * Returns:        
 *        SOC_E_XXX
 */

STATIC int
_phy_5421S_medium_select(int unit, soc_port_t port, int medium)
{
    phy_ctrl_t     *pc = EXT_PHY_SW_STATE(unit, port);
    uint16          tmp;
    int             allow_mode_switch;

    DPRINTF((BSL_META_U(unit,
                        "5421S_medium_select: port=%d medium=%d fp=%d\n"),
             port, medium, pc->fiber.preferred));

    /*
     * Inform outside world of the current mode, including phy5690.c for
     * notify routines and bcm/link.c for printing the link type.
     */

    if (medium == SOC_PORT_MEDIUM_FIBER) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
    } else {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
    }

    /*
     * Program medium.  If no neither medium is active, put the PHY in
     * fiber mode because it's lower power.
     */

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        /*
         * Clear SGMII enable when using GMII, as is the case for
         * StrataSwitches, and set it when using SGMII, which is the
         * case for BCM5690 only (shadow 010, power control).
         */
        tmp = 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY5421_POWER_CTRLr(unit, pc, tmp, 0x0800));

        /* Set full duplex */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY5421_MII_CTRLr(unit, pc, MII_CTRL_FD, MII_CTRL_FD));

        if (PHY_TBI_MODE(unit, port)) {
            /* Partial TBI+COPPER support - ANCTL.ANEN must be 0 */
            phy_ctrl_t  *int_pc;

            int_pc = INT_PHY_SW_STATE(unit, port);
            SOC_IF_ERROR_RETURN
                (PHYCTRL_AUTO_NEGOTIATE_SET(int_pc, unit, port, FALSE));
        }
    } else {        /* SOC_PORT_MEDIUM_FIBER/NONE */
        /* Set SGMII enable (shadow 010, power control) */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY5421_POWER_CTRLr(unit, pc, 0x0800, 0x0800));


        /*
         * Set Fiber mode (shadow 00100, spare control register 2).
         * This can remain even in copper mode.  However, it may be set
         * ONLY after setting SGMII enable or link will be messed up.
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY5421_SPARE_CTRL2r(unit, pc, 0x0020, 0x0020));
    }

    /*
     * Decide whether to enable the non-selected medium to send link
     * pulses, which could cause certain partners to assume link is up.
     * Link pulses should ONLY be sent on the non-selected medium if it
     * is the preferred medium, or if neither medium has signal.
     */

    switch (medium) {
    case SOC_PORT_MEDIUM_FIBER:
        allow_mode_switch = !pc->fiber.preferred;
        break;
    case SOC_PORT_MEDIUM_COPPER:
        allow_mode_switch = pc->fiber.preferred;
        break;
    default:
        allow_mode_switch = TRUE;
        break;
    }

    /* Spare Control Register 3 */
    if (allow_mode_switch) {
        tmp = 0;                /* Enable pulses on non-selected medium */
    } else {
        tmp = 0x08;                /* Disable pulses on non-selected medium */
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY5421_SPARE_CTRL3r(unit, pc, tmp, 0x0008));

    /*
     * Before link comes up, set the PHY parameters including autoneg
     * enable and advertisement, according to saved defaults for this
     * port type (copper/fiber).
     */

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        SOC_IF_ERROR_RETURN
            (_phy_5421S_medium_config_update(unit, port, &pc->copper));
    }

    if (medium == SOC_PORT_MEDIUM_FIBER) {
        SOC_IF_ERROR_RETURN
            (_phy_5421S_medium_config_update(unit, port, &pc->fiber));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *        phy_5421S_medium_status
 * Purpose:
 *        Indicate the current active medium
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        medium - (OUT) One of:
 *                SOC_PORT_MEDIUM_COPPER
 *                SOC_PORT_MEDIUM_FIBER
 *                SOC_PORT_MEDIUM_NONE
 * Returns:        
 *        SOC_E_NONE
 */

STATIC int
phy_5421S_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    *medium = EXT_PHY_SW_STATE(unit, port)->medium;
    return SOC_E_NONE;
}

/*
 * Function:
 *        _phy_5421S_medium_config_update
 * Purpose:
 *        Update the PHY with config parameters
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        cfg - Config structure.
 * Returns:        
 *        SOC_E_XXX
 */

STATIC int
_phy_5421S_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_5421S_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_5421S_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_5421S_master_set(unit, port, cfg->master));
    SOC_IF_ERROR_RETURN
        (phy_5421S_adv_local_set(unit, port, cfg->autoneg_advert));
    SOC_IF_ERROR_RETURN
        (phy_5421S_autoneg_set(unit, port, cfg->autoneg_enable));
    SOC_IF_ERROR_RETURN
        (phy_5421S_mdix_set(unit, port, cfg->mdix));

    return SOC_E_NONE;
}

/*
 * Function:
 *        phy_5421S_medium_config_set
 * Purpose:
 *        Set the operating parameters that are automatically selected
 *        when medium switches type.
 * Parameters:
 *        unit - StrataSwitch unit #
 *        port - Port number
 *        medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *        cfg - Operating parameters
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5421S_medium_config_set(int unit, soc_port_t port, 
                            soc_port_medium_t medium,
                            soc_phy_config_t *cfg)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Changes take effect immediately if the target medium is active.
     */

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        if (!pc->automedium && !PHY_COPPER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        sal_memcpy(&pc->copper, cfg, sizeof (pc->copper));

        pc->copper.autoneg_advert &= ADVERT_ALL_COPPER;

        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN
                (_phy_5421S_medium_config_update(unit, port, &pc->copper));
        }
    } else if (medium == SOC_PORT_MEDIUM_FIBER) {
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        sal_memcpy(&pc->fiber, cfg, sizeof (pc->fiber));

        pc->fiber.autoneg_advert &= ADVERT_ALL_FIBER;

        if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            SOC_IF_ERROR_RETURN
                (_phy_5421S_medium_config_update(unit, port, &pc->fiber));
        }
    } else {
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *        phy_5421S_medium_config_get
 * Purpose:
 *        Get the operating parameters that are automatically selected
 *        when medium switches type.
 * Parameters:
 *        unit - StrataSwitch unit #
 *        port - Port number
 *        medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *        cfg - (OUT) Operating parameters
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5421S_medium_config_get(int unit, soc_port_t port, 
                            soc_port_medium_t medium,
                            soc_phy_config_t *cfg)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    if (medium == SOC_PORT_MEDIUM_COPPER) {
        if (!pc->automedium && !PHY_COPPER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        sal_memcpy(cfg, &pc->copper, sizeof (*cfg));
    } else if (medium == SOC_PORT_MEDIUM_FIBER) {
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port)) {
            return SOC_E_UNAVAIL;
        }
        sal_memcpy(cfg, &pc->fiber, sizeof (*cfg));
    } else {
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        _phy_5421S_reset_setup
 * Purpose:        
 *        Function to reset the PHY and set up initial operating registers.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 * Returns:        
 *        SOC_E_XXX
 */

STATIC int
_phy_5421S_reset_setup(int unit, soc_port_t port)
{
    uint16          tmp;
    soc_port_if_t   pif;
    int             rv;
    phy_ctrl_t     *pc;
    phy_ctrl_t     *int_pc;

    pc     = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    DPRINTF((BSL_META_U(unit,
                        "_phy_5421S_reset_setup: u=%d p=%d\n"),
             unit, port));

    /* Reset PHY */
    SOC_IF_ERROR_RETURN(soc_phy_reset(unit, port));

    /* Enable SGMII registers */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY5421_SGMII_REG_ENBLEr(unit, pc, 0xfc01));

    /* Put chip in TBI or SGMII mode (retain current PHY_FLAG_10B setting) */
    if (PHY_TBI_MODE(unit, port)) {
        pif = SOC_PORT_IF_TBI;
    } else {
        pif = SOC_PORT_IF_GMII;
    }

    SOC_IF_ERROR_RETURN(phy_5421S_interface_set(unit, port, pif));

    /* Initialize the switch MAC PCS in case TBI mode is used now or later */
    rv = PHYCTRL_INIT(int_pc, unit, port);
    /* If the SOC chip does not support TBI, and the system is      */
    /* configured for TBI, return the error code. If the system     */
    /* is not configured for TBI, ignore the phy_serdes_init error  */
    /* and save the TBI capability status for use elsewhere.        */
    if (rv < 0) { 
        EXT_PHY_SW_STATE(unit, port)->tbi_capable = FALSE;
        if (PHY_TBI_MODE(unit, port)) {
            
#if 0 /* Should we take the PHY out of TBI mode? */ 
            SOC_IF_ERROR_RETURN(phy_5421S_interface_set(unit, port, 
                                                        SOC_PORT_IF_GMII)); 
#endif
            return (rv);
        }
    }

#if SERDES_COPPER_AMP_WAR
    /*
     * Supply Sensitivity Errata for BCM5421SA0; reduces amplitude out
     * of compliance if used on BCM5421SA1 or later parts.
     */
    SOC_IF_ERROR_RETURN
        (phy_reg_ge_write(unit, pc, 0x00, 0x0000, 0x18, 0x0c00));
    SOC_IF_ERROR_RETURN
        (phy_reg_ge_write(unit, pc, 0x00, 0x003e, 0x15, 0x0380));
#endif

#if ADC_PERF_WAR
    /*
     * Proprietary fix for unusual board/cabling arrangement.
     * Can compromise cable length performance if used unnecessarily.
     */
    SOC_IF_ERROR_RETURN
        (phy_reg_ge_write(unit, pc, 0x00, 0x0000, 0x18, 0x0c00));
    SOC_IF_ERROR_RETURN
        (phy_reg_ge_write(unit, pc, 0x00, 0x201f, 0x15, 0x2aaa));
#endif

#if REGULATOR_GAMMA_WAR
    /*
     * Gamma fix needed if not using internal regulators;
     * does not hurt if used either way.
     */
    SOC_IF_ERROR_RETURN
        (phy_reg_ge_write(unit, pc, 0x00, 0x0000, 0x18, 0x0c00));
    SOC_IF_ERROR_RETURN
        (phy_reg_ge_write(unit, pc, 0x00, 0x000a, 0x15, 0x0323));
#endif

    /* Clear RGMII enable (shadow 111, misc control) */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY5421_MISC_CTRLr(unit, pc, 0, 0x0080));

    /*
     * Configure Spare Control Register 3.
     * Enabling LEDs to work in fiber mode is important because this
     * signal is used to indicate link in _phy_5421S_link_gmii_fiber().
     */
    tmp  = 0x20;        /* Enable signal/energy detect interrupt */
    tmp |= 0x40;        /* Enable LEDs to work in fiber mode */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY5421_SPARE_CTRL3r(unit, pc, tmp, 0x0060));

    /* Configure Extended Control Register */
    tmp  = 0x0020;        /* Enable LEDs to indicate traffic status */
    tmp |= 0x0001;        /* Set high FIFO elasticity to support jumbo frames */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY5421_MII_ECRr(unit, pc, tmp, 0x0021));

    /* Enable extended packet length (4.5k through 25k) */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY5421_MISC_CTRLr(unit, pc, 0x4000, 0x4000));

    return(SOC_E_NONE);
}

/*
 * Function:         
 *        phy_5421S_init
 * Purpose:        
 *        Init function for 5421S PHY.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 * Returns:        
 *        SOC_E_XXX
 */

STATIC int
phy_5421S_init(int unit, soc_port_t port)
{
    phy_ctrl_t     *pc;
    int             fiber_preferred;

    DPRINTF((BSL_META_U(unit,
                        "phy_5421S_init: u=%d p=%d\n"),
             unit, port));

    pc = EXT_PHY_SW_STATE(unit, port);

    /* phyident.c has already set PHY_FLAGS_10B if required */

    PHY_FLAGS_SET(unit, port, PHY_FLAGS_5421S);

    /*
     * Set the default settings for copper and fiber.  The upper layer
     * application can change these using phy_5421S_medium_config_set.
     *
     * The settings are also updated by individual calls such as
     * phy_5421S_adv_local_set, phy_5421S_speed_set, etc.  In this case,
     * they operate on the currently-selected medium type.
     *
     * The property phy_fiber_pref_port<X> is used to set a preference
     * for fiber mode in the event both copper and fiber links are up.
     * Default is to prefer fiber.
     */

    pc->automedium =
        soc_property_port_get(unit, port,
                              spn_PHY_AUTOMEDIUM, 1);

    fiber_preferred =
        soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 1);

    pc->fiber_plug_dely =
        soc_property_port_get(unit, port,
                              spn_PHY_FIBER_DEGLITCH_USECS, 0);

    pc->copper.enable = TRUE;
    pc->copper.preferred = !fiber_preferred;
    pc->copper.autoneg_enable = TRUE;
    pc->copper.autoneg_advert = ADVERT_ALL_COPPER;
    pc->copper.force_speed = 1000;
    pc->copper.force_duplex = TRUE;
    pc->copper.master = SOC_PORT_MS_AUTO;
    pc->copper.mdix = SOC_PORT_MDIX_AUTO;

    pc->fiber.enable = TRUE;
    pc->fiber.preferred = fiber_preferred;
    pc->fiber.autoneg_enable = !PHY_TBI_MODE(unit, port);
    pc->fiber.autoneg_advert = ADVERT_ALL_FIBER;
    pc->fiber.force_speed = 1000;
    pc->fiber.force_duplex = TRUE;
    pc->fiber.master = SOC_PORT_MS_NONE;
    pc->fiber.mdix = SOC_PORT_MDIX_NORMAL;

    /* This PHY is TBI capable (although the SOC */
    /* chip may ultimately turn out not to be).  */
    pc->tbi_capable = TRUE; 

    /* Start with no medium */

    pc->medium = SOC_PORT_MEDIUM_NONE;

    /* Hardware initialization */

    SOC_IF_ERROR_RETURN(_phy_5421S_reset_setup(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_enable_set
 * Purpose:        
 *        Enable or disable the physical interface.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        enable - Boolean, true = enable PHY, false = disable.
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        This is a software enable/disable for the entire PHY.
 *        See phy_fe_ge_enable_set for an explanation.
 */

STATIC int
phy_5421S_enable_set(int unit, soc_port_t port, int enable)
{
    if (GMII_MODE(unit, port)) {
        if (enable) {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
        } else {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
        }
        return SOC_E_NONE;
    } else {
        phy_ctrl_t *int_pc;

        int_pc = INT_PHY_SW_STATE(unit, port);
        return PHYCTRL_ENABLE_SET(int_pc, unit, port, enable);
    }
}

/*
 * Function:         
 *        phy_5421S_enable_get
 * Purpose:        
 *        Enable or disable the physical interface for a 5421S device.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:        
 *        SOC_E_XXX
 */

STATIC int
phy_5421S_enable_get(int unit, soc_port_t port, int *enable)
{
    if (GMII_MODE(unit, port)) {
        *enable = !PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE);
        return SOC_E_NONE;
    } else {
        phy_ctrl_t *int_pc;

        int_pc = INT_PHY_SW_STATE(unit, port);
        return PHYCTRL_ENABLE_GET(int_pc, unit, port, enable);
    }
}

/*
 * Function:
 *        _phy_5421S_link_gmii_fiber (internal)
 * Purpose:
 *        Determine the current link up/down status for a 5421S device when
 *        operating in GMII+FIBER mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      link - (OUT) Boolean, true indicates link established.
 * Returns:
 *        SOC_E_XXX
 */
STATIC int
_phy_5421S_link_gmii_fiber(int unit, soc_port_t port, int *link)
{
    uint16          tmp;
    int             autoneg_enabled, autoneg_complete;
    int             autoneg_local_fault, autoneg_remote_fault;
    int             led_link;
    phy_ctrl_t    *pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Read LED link status (shadow 01000, LED status register).
     * This works because the init routine has configured the Fiber LED
     * mode.  It provides real time indication of true link, whether or
     * not autonegotiation is enabled.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY5421_LED_STATr(unit, pc, &tmp));

    led_link = ((tmp & 0x20) == 0);

    SOC_IF_ERROR_RETURN
        (READ_PHY5421_SGMII_1000X_CTRLr(unit, pc, &tmp));

    autoneg_enabled = ((tmp & 0x4) == 0);

    if (!autoneg_enabled) {
        /* If autonegotiation is off, go by LED signal */
        *link = led_link;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY5421_SGMII_1000X_STATr(unit, pc, &tmp));

    autoneg_complete = ((tmp & 0x2) != 0);

    if (!autoneg_complete) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    autoneg_local_fault = ((tmp & 1) != 0);        /* Autoneg Error */

    if (autoneg_local_fault) {
        autoneg_remote_fault = 0;
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY5421_1000X_ANPr(unit, pc, &tmp));
        autoneg_remote_fault = ((tmp & 0x60) != 0);
    }

    if (autoneg_local_fault || autoneg_remote_fault) {
        /* Restart autonegotiation */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY5421_SGMII_1000X_MISCr(unit, pc, 0x0001));
        *link = FALSE;
        return SOC_E_NONE;
    }

    *link = TRUE;

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_link_get
 * Purpose:        
 *        Determine the current link up/down status for a 5421S device.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        link - (OUT) Boolean, true indicates link established.
 * Returns:        
 *        SOC_E_XXX
 * Notes: 
 *        No synchronization performed at this level.
 */

STATIC int
phy_5421S_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);
    int                medium;

    SOC_IF_ERROR_RETURN(_phy_5421S_medium_check(unit, port, &medium));

    if (medium != pc->medium) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
        DPRINTF((BSL_META_U(unit,
                            "phy_5421S_link_get: u=%d p=%d: medium %d-->%d\n"),
                 unit, port, pc->medium, medium));

        pc->medium = medium;

        SOC_IF_ERROR_RETURN(_phy_5421S_medium_select(unit, port, medium));

        /* Ensure linkscan sees link down at least once */

        *link = FALSE;
        return SOC_E_NONE;
    } else {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
    }

    /*
     * Use the appropriate method to check for link up.
     */

    if (GMII_MODE(unit, port)) {
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            return phy_fe_ge_link_get(unit, port, link);
        } else if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            return _phy_5421S_link_gmii_fiber(unit, port, link);
        } else {
            *link = FALSE;
            return SOC_E_NONE;
        }
    } else {
        phy_ctrl_t *int_pc;

        int_pc = INT_PHY_SW_STATE(unit, port);
        return PHYCTRL_LINK_GET(int_pc, unit, port, link);
    }
}

/*
 * Function:         
 *        phy_5421S_duplex_set
 * Purpose:        
 *        Set the current duplex mode (forced).
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:        
 *        SOC_E_NONE
 *        SOC_E_UNAVAIL - Half duplex requested, and not supported.
 *        SOC_E_XXX
 * Notes: 
 *        The duplex is set only for the ACTIVE medium.
 *        No synchronization performed at this level.
 *        Autonegotiation is not manipulated.
 */

STATIC int
phy_5421S_duplex_set(int unit, soc_port_t port, int duplex)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    DPRINTF((BSL_META_U(unit,
                        "phy_5421S_duplex_set: u=%d p=%d d=%d m=%d\n"),
             unit, port, duplex, pc->medium));

    if (GMII_MODE(unit, port)) {
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_fe_ge_duplex_set(unit, port, duplex));
            pc->copper.force_duplex = duplex;
        }

        if (pc->medium == SOC_PORT_MEDIUM_FIBER && !duplex) {
            return SOC_E_UNAVAIL;
        }
    } else {
        phy_ctrl_t *int_pc;

        int_pc = INT_PHY_SW_STATE(unit, port);

        SOC_IF_ERROR_RETURN
            (PHYCTRL_DUPLEX_SET(int_pc, unit, port, duplex));

        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            pc->copper.force_duplex = duplex;
        } 

        if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            pc->fiber.force_duplex = duplex;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_duplex_get
 * Purpose:        
 *        Get the current operating duplex mode. If autoneg is enabled, 
 *        then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        duplex - (OUT) Boolean, true indicates full duplex, false 
 *                indicates half.
 * Returns:        
 *        SOC_E_NONE
 * Notes: 
 *        The duplex is retrieved for the ACTIVE medium.
 *        No synchronization performed at this level. Autonegotiation is 
 *        not manipulated. 
 */

STATIC int
phy_5421S_duplex_get(int unit, soc_port_t port, int *duplex)
{
    phy_ctrl_t        *pc;

    if (GMII_MODE(unit, port)) {
        pc = EXT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_fe_ge_duplex_get(unit, port, duplex));
        } else {
            *duplex = TRUE;
        }
        return SOC_E_NONE;
    } else {
        pc = INT_PHY_SW_STATE(unit, port);

        return PHYCTRL_DUPLEX_GET(pc, unit, port, duplex);
    }
}

/*
 * Function:         
 *        phy_5421S_speed_set
 * Purpose:        
 *        Set the current operating speed (forced).
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        speed - Requested speed, only 1000 supported.
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        The speed is set only for the ACTIVE medium.
 */

STATIC int
phy_5421S_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    DPRINTF((BSL_META_U(unit,
                        "phy_5421S_speed_set: u=%d p=%d s=%d m=%d\n"),
             unit, port, speed, pc->medium));

    if (GMII_MODE(unit, port)) {
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_fe_ge_speed_set(unit, port, speed));
            pc->copper.force_speed = speed;
        }

        if (pc->medium == SOC_PORT_MEDIUM_FIBER && speed != 1000) {
            return SOC_E_CONFIG;
        }
    } else {
        phy_ctrl_t  *int_pc;

        int_pc = INT_PHY_SW_STATE(unit, port);

        SOC_IF_ERROR_RETURN
            (PHYCTRL_SPEED_SET(int_pc, unit, port, speed));
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            pc->copper.force_speed = speed;
        }

        if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            pc->fiber.force_speed = speed;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_speed_get
 * Purpose:        
 *        Get the current operating speed for a 5421S device.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        speed - (OUT) Boolean, true indicates full duplex, false 
 *                indicates half.
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        The speed is retrieved for the ACTIVE medium, 0 if none.
 */

STATIC int
phy_5421S_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t        *pc;

    if (GMII_MODE(unit, port)) {
        pc = EXT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_fe_ge_speed_get(unit, port, speed));
        } else if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            *speed = 1000;
        } else {
            *speed = 0;
        }
        return SOC_E_NONE;
    } else {
        pc = INT_PHY_SW_STATE(unit, port);
        return PHYCTRL_SPEED_GET(pc, unit, port, speed);
    }
}

/*
 * Function:         
 *        phy_5421S_master_set
 * Purpose:        
 *        Set the current master mode
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        master - SOC_PORT_MS_*
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        The master mode is set only for the ACTIVE medium.
 */

STATIC int
phy_5421S_master_set(int unit, soc_port_t port, int master)
{
    phy_ctrl_t        *pc;

    if (GMII_MODE(unit, port)) {
        pc = EXT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_fe_ge_master_set(unit, port, master));
            pc->copper.master = master;
        }
    } else {
        pc = INT_PHY_SW_STATE(unit, port);

        return (PHYCTRL_MASTER_SET(pc, unit, port, master));
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_master_get
 * Purpose:        
 *        Get the current master mode for a 5421S device.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        master - (OUT) SOC_PORT_MS_*
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        The master mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_5421S_master_get(int unit, soc_port_t port, int *master)
{
    phy_ctrl_t        *pc;

    if (GMII_MODE(unit, port)) {
        pc = EXT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_fe_ge_master_get(unit, port, master));
        } else {
            *master = SOC_PORT_MS_NONE;
        }
    } else {
        pc = INT_PHY_SW_STATE(unit, port);

        return (PHYCTRL_MASTER_GET(pc, unit, port, master));
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_autoneg_set
 * Purpose:        
 *        Enable or disable auto-negotiation on the specified port.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        an   - Boolean, if true, auto-negotiation is enabled 
 *                (and/or restarted). If false, autonegotiation is disabled.
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        The autoneg mode is set only for the ACTIVE medium.
 */

STATIC int
phy_5421S_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    DPRINTF((BSL_META_U(unit,
                        "phy_5421S_autoneg_set: u=%d p=%d autoneg=%d m=%d\n"),
             unit, port, autoneg, pc->medium));

    if (GMII_MODE(unit, port)) {
        uint16                tmp;

        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_fe_ge_an_set(unit, port, autoneg));
            pc->copper.autoneg_enable = autoneg ? 1 : 0;
        }

        if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            if (autoneg) {
                tmp = 0;      /* Clear AUTONEG DISABLE */
            } else {
                tmp = 0x4;    /* Set AUTONEG DISABLE */
            }

            SOC_IF_ERROR_RETURN
                (MODIFY_PHY5421_SGMII_1000X_CTRLr(unit, pc, tmp, 0x0004));

            /* Restart autoneg */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY5421_SGMII_1000X_MISCr(unit, pc, 0x0001));

            pc->fiber.autoneg_enable = autoneg ? 1 : 0;
        }
    } else {
        phy_ctrl_t *int_pc;
        int_pc = INT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            /* Partial TBI+COPPER support - ANCTL.ANEN must be 0 */

            SOC_IF_ERROR_RETURN
                (PHYCTRL_AUTO_NEGOTIATE_SET(int_pc, unit, port, FALSE));

            pc->copper.autoneg_enable = FALSE;
        }

        if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            SOC_IF_ERROR_RETURN
                (PHYCTRL_AUTO_NEGOTIATE_SET(int_pc, unit, port, autoneg));

            pc->fiber.autoneg_enable = autoneg ? 1 : 0;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_autoneg_get
 * Purpose:        
 *        Get the current auto-negotiation status (enabled/busy).
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        autoneg - (OUT) if true, auto-negotiation is enabled.
 *        autoneg_done - (OUT) if true, auto-negotiation is complete. This
 *                     value is undefined if autoneg == FALSE. 
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        The autoneg mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_5421S_autoneg_get(int unit, soc_port_t port,
                      int *autoneg, int *autoneg_done)
{
    phy_ctrl_t        *pc;

    if (GMII_MODE(unit, port)) {
        pc = EXT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN
                (phy_fe_ge_an_get(unit, port, autoneg, autoneg_done));
        } else if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            uint16        tmp;

            SOC_IF_ERROR_RETURN
                (READ_PHY5421_SGMII_1000X_CTRLr(unit, pc, &tmp)); 
            *autoneg = ((tmp & 0x4) == 0);      /* AUTONEG DISABLE bit clear */

            SOC_IF_ERROR_RETURN
                (READ_PHY5421_SGMII_1000X_STATr(unit, pc, &tmp));

            *autoneg_done = ((tmp & 0x2) != 0);   /* AUTONEG DONE */
        } else {
            *autoneg = FALSE;
            *autoneg_done = FALSE;
        }
    } else {
        pc = INT_PHY_SW_STATE(unit, port);
        SOC_IF_ERROR_RETURN
            (PHYCTRL_AUTO_NEGOTIATE_GET(pc, unit, port, autoneg, autoneg_done));
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_adv_local_set
 * Purpose:        
 *        Set the current advertisement for auto-negotiation.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        mode - Port mode mask indicating supported options/speeds.
 * Returns:        
 *        SOC_E_XXX
 * Notes: 
 *        The advertisement is set only for the ACTIVE medium.
 *        No synchronization performed at this level.
 */

STATIC int
phy_5421S_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    DPRINTF((BSL_META_U(unit,
                        "phy_5421S_adv_local_set: u=%d p=%d mode=0x%x m=%d\n"),
             unit, port, mode, pc->medium));

    if (GMII_MODE(unit, port)) {
        uint16             tmp;
        int                autoneg;

        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_ge_adv_local_set(unit, port, mode));
            pc->copper.autoneg_advert = mode;
        }

        if (pc->medium == SOC_PORT_MEDIUM_FIBER) {

            SOC_IF_ERROR_RETURN
                (READ_PHY5421_SGMII_1000X_CTRLr(unit, pc, &tmp));

            autoneg = ((tmp & 0x4) == 0);

            tmp &= ~0x6000;            /* Transmit "No-Failure" to partner */

            switch (mode & SOC_PM_PAUSE) {
            default:
                tmp &= ~(0x1000 | 0x0800);     /* ASYMM=0 and PAUSE=0 */
                break;
            case SOC_PM_PAUSE_TX:
                tmp |= 0x1000;                 /* ASYMM=1 and PAUSE(dir)=0 */
                tmp &= ~0x0800;
                break;
            case SOC_PM_PAUSE_RX:
                tmp |= (0x1000 | 0x0800);      /* ASYMM=1 and PAUSE(dir)=1 */
                break;
            case SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX:
                tmp &= ~0x1000;                /* ASYMM=0 and PAUSE=1 */
                tmp |= 0x0800;
                break;
            }

            SOC_IF_ERROR_RETURN
                (WRITE_PHY5421_SGMII_1000X_CTRLr(unit, pc, tmp));

            if (autoneg) {
                /* Restart autonegotiation */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY5421_SGMII_1000X_MISCr(unit, pc, 0x0001));
            }

            pc->fiber.autoneg_advert = mode;
        }
    } else {
        phy_ctrl_t *int_pc;

        int_pc = INT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN
                (PHYCTRL_ADV_SET(int_pc, unit, port, mode));
            pc->copper.autoneg_advert = mode;
        }
        if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            SOC_IF_ERROR_RETURN
                (PHYCTRL_ADV_SET(int_pc, unit, port, mode));
            pc->fiber.autoneg_advert = mode;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_adv_local_get
 * Purpose:        
 *        Get the current advertisement for auto-negotiation.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:        
 *        SOC_E_XXX
 * Notes: 
 *        The advertisement is retrieved for the ACTIVE medium.
 *        No synchronization performed at this level.
 */

STATIC int
phy_5421S_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    phy_ctrl_t        *pc;

    if (GMII_MODE(unit, port)) {
        pc = EXT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_ge_adv_local_get(unit, port, mode));
        } else if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            uint16        tmp;

            SOC_IF_ERROR_RETURN
                (READ_PHY5421_SGMII_1000X_CTRLr(unit, pc, &tmp));

            *mode = SOC_PM_1000MB_FD;

            switch (tmp & 0x1800) {
            case 0x0000:
                break;
            case 0x0800:
                *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
                break;
            case 0x1000:
                *mode |= SOC_PM_PAUSE_TX;
                break;
            case 0x1800:
                *mode |= SOC_PM_PAUSE_RX;
                break;
            }
        } else {
            *mode = 0;
        }
    } else {
        pc = INT_PHY_SW_STATE(unit, port);
        SOC_IF_ERROR_RETURN
            (PHYCTRL_ADV_GET(pc, unit, port, mode));
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_adv_remote_get
 * Purpose:        
 *        Get partner's current advertisement for auto-negotiation.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:        
 *        SOC_E_XXX
 * Notes: 
 *        The remote advertisement is retrieved for the ACTIVE medium.
 *        No synchronization performed at this level. If Autonegotiation is
 *        disabled or in progress, this routine will return an error.
 */

STATIC int
phy_5421S_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    phy_ctrl_t        *pc;

    if (GMII_MODE(unit, port)) {
        pc = EXT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_ge_adv_remote_get(unit, port, mode));
        } else if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            uint16        tmp;
            int           link;

            *mode = 0;

            SOC_IF_ERROR_RETURN
                (READ_PHY5421_SGMII_1000X_CTRLr(unit, pc, &tmp));

            if (tmp & 0x4) {
                return SOC_E_DISABLED;
            }

            SOC_IF_ERROR_RETURN
                (_phy_5421S_link_gmii_fiber(unit, port, &link));

            if (!link) {
                return SOC_E_BUSY;
            }

            SOC_IF_ERROR_RETURN
                (READ_PHY5421_1000X_ANPr(unit, pc, &tmp)); 

            if (tmp & 0x10) {
                *mode |= SOC_PM_PAUSE_ASYMM;
            }

            if (tmp & 0x8) {
                *mode |= SOC_PM_PAUSE;
            }

            if (tmp & 0x4) {
                *mode |= SOC_PM_1000MB_HD;
            }

            if (tmp & 0x2) {
                *mode |= SOC_PM_1000MB_FD;
            }
        } else {
            *mode = 0;
        }
    } else {
        pc = INT_PHY_SW_STATE(unit, port);
        return (PHYCTRL_ADV_REMOTE_GET(pc, unit, port, mode));

    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_lb_set
 * Purpose:        
 *        Set the local PHY loopback mode.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        loopback - Boolean: true = enable loopback, false = disable.
 * Returns:        
 *        SOC_E_XXX
 * Notes: 
 *        The loopback mode is set only for the ACTIVE medium.
 *        No synchronization performed at this level.
 */

STATIC int
phy_5421S_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    DPRINTF((BSL_META_U(unit,
                        "phy_5421S_lb_set: u=%d p=%d en=%d m=%d\n"),
             unit, port, enable, pc->medium));

    if (GMII_MODE(unit, port) && pc->medium == SOC_PORT_MEDIUM_COPPER) {
        SOC_IF_ERROR_RETURN(phy_fe_ge_lb_set(unit, port, enable));
    } else {
        uint16                tmp;

        SOC_IF_ERROR_RETURN
            (READ_PHY5421_SGMII_1000X_CTRLr(unit, pc, &tmp));

        if (enable) {
            tmp |= 1;
        } else {
            tmp &= ~1;
        }

        SOC_IF_ERROR_RETURN
            (WRITE_PHY5421_SGMII_1000X_CTRLr(unit, pc, tmp));
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_lb_get
 * Purpose:        
 *        Get the local PHY loopback mode.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        The loopback mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_5421S_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    if (GMII_MODE(unit, port) && pc->medium == SOC_PORT_MEDIUM_COPPER) {
        SOC_IF_ERROR_RETURN(phy_fe_ge_lb_get(unit, port, enable));
    } else {
        uint16                tmp;

        SOC_IF_ERROR_RETURN
            (READ_PHY5421_SGMII_1000X_CTRLr(unit, pc, &tmp));

        *enable = ((tmp & 1)) != 0;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *        phy_5421S_interface_set
 * Purpose:
 *        Set the current operating mode of the PHY (TBI or MII/GMII)
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        pif - One of SOC_PORT_IF_*
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5421S_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
#if defined(BCM_ESW_SUPPORT)
    uint32              gpcsc;
#endif    
    uint16              mii_ecr;
    int                 tbi;
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    DPRINTF((BSL_META_U(unit,
                        "phy_5421S_interface_set: u=%d p=%d pif=%d\n"),
             unit, port, pif));

    /*
     * On SDKs with 5421S daughterboards, for even N, the port N EWRAP
     * line is used to select TBI mode on port N and N+1, and port N+1
     * EWRAP is ignored.  MII management registers are used to select
     * loopback.
     */

    tbi = (pif == SOC_PORT_IF_TBI);

    if (tbi) {
        if (EXT_PHY_SW_STATE(unit, port)->tbi_capable == TRUE) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_10B);
        } else {
            return SOC_E_UNAVAIL;
        }
    } else {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_10B);
    }

#if defined(BCM_ESW_SUPPORT)
    if (SOC_IS_ESW(unit)) {
       SOC_IF_ERROR_RETURN(READ_GPCSCr(unit, port, &gpcsc));
       soc_reg_field_set(unit, GPCSCr, &gpcsc, EWRAPf, tbi);
       SOC_IF_ERROR_RETURN(WRITE_GPCSCr(unit, port, gpcsc));
    }
#endif /* BCM_ESW_SUPPORT */
    /*
     * There is a strange "TBI Toggle" bit in this PHY that must be
     * written if the PHY comes up in GMII mode.  The initial mode is
     * latched from a pin and may vary depending if link is present.
     */

    SOC_IF_ERROR_RETURN
        (READ_PHY5421_MII_ECRr(unit, pc, &mii_ecr));

    DPRINTF((BSL_META_U(unit,
                        "5421S_if_set: port=%d tbi=%d reg 0x10=0x%x\n"),
             port, tbi, mii_ecr));

    if ((tbi && !(mii_ecr & MII_ECR_10B)) ||
        (!tbi && (mii_ecr & MII_ECR_10B))) {

        /* Update ECR */
        mii_ecr ^= MII_ECR_10B;

        DPRINTF((BSL_META_U(unit,
                            "5421S_if_set: port=%d set reg 0x10=0x%x "
                            "and toggle TBI\n"),
                 port, mii_ecr));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY5421_MII_ECRr(unit, pc, mii_ecr));

        /* Write 1 to TBI Toggle (shadow 00100, spare control register 2) */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY5421_SPARE_CTRL2r(unit, pc, 0x0040));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *        phy_5421S_interface_get
 * Purpose:
 *        Get the current operating mode of the PHY (TBI or GMII).
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        mii - (OUT) TRUE --> MII/GMII, FALSE --> TBI.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5421S_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_10B)) {
        *pif = SOC_PORT_IF_TBI;
    } else {
        *pif = SOC_PORT_IF_GMII;
    }

    return SOC_E_NONE;
}

/*
 * Function:         
 *        phy_5421S_ability_get
 * Purpose:        
 *        Get the abilities of the local gigabit PHY.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 *        mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:        
 *        SOC_E_NONE
 * Notes:
 *        The ability is retrieved only for the ACTIVE medium.
 */

STATIC int 
phy_5421S_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    phy_ctrl_t        *pc;

    /*
     * This should return both GMII and TBI abilities, but with the
     * current code that causes problems in _bcm_port_mode_setup().
     */

    if (GMII_MODE(unit, port)) {
        pc = EXT_PHY_SW_STATE(unit, port);
        if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
            SOC_IF_ERROR_RETURN(phy_ge_ability_get(unit, port, mode));
        } else if (pc->medium == SOC_PORT_MEDIUM_FIBER) {
            *mode = (SOC_PM_AN | SOC_PM_LB_PHY | 
                     SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM |
                     SOC_PM_GMII | SOC_PM_1000MB_FD);
        } else {
            *mode = SOC_PM_GMII;
        }
    } else {
        pc = INT_PHY_SW_STATE(unit, port);
        SOC_IF_ERROR_RETURN
            (PHYCTRL_ABILITY_GET(pc, unit, port, mode));

    }

    if (pc->automedium) {
        *mode |= SOC_PM_COMBO;
    }

    if (PHY_TBI_MODE(unit, port)) {
        *mode |= SOC_PM_TBI;
    }

    DPRINTF((BSL_META_U(unit,
                        "phy_5421S_ability_get: u=%d p=%d mode=0x%x\n"),
             unit, port, *mode));

    return(SOC_E_NONE);
}

/*
 * Function:
 *        phy_5421S_mdix_set
 * Description:
 *        Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *        unit - Device number
 *        port - Port number
 *        mode - One of:
 *                BCM_PORT_MDIX_AUTO
 *                        Enable auto-MDIX when autonegotiation is enabled
 *                BCM_PORT_MDIX_FORCE_AUTO
 *                        Enable auto-MDIX always
 *                BCM_PORT_MDIX_NORMAL
 *                        Disable auto-MDIX
 *                BCM_PORT_MDIX_XOVER
 *                        Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *        SOC_E_UNAVAIL - feature unsupported by hardware
 *        SOC_E_XXX - other error
 */
STATIC int
phy_5421S_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    phy_ctrl_t         *pc = EXT_PHY_SW_STATE(unit, port);
    int                 speed;
    int                 rv = SOC_E_NONE;

    if (pc == NULL) {
        return SOC_E_PARAM;
    }

    switch (pc->medium) {
    case SOC_PORT_MEDIUM_FIBER:
        if (mode != SOC_PORT_MDIX_NORMAL) {
            rv = SOC_E_UNAVAIL;
        }

        if (rv == SOC_E_NONE) {
            pc->fiber.mdix = mode;
        }
        break;

    case SOC_PORT_MEDIUM_COPPER:
        switch (mode) {
        case SOC_PORT_MDIX_AUTO:
            /* Clear bit 14 for automatic MDI crossover */
            SOC_IF_ERROR_RETURN  
                (MODIFY_PHY5421_MII_ECRr(unit, pc, 0, 0x4000));
            break;

        case SOC_PORT_MDIX_FORCE_AUTO:
            rv = SOC_E_UNAVAIL;
            break;

        case SOC_PORT_MDIX_NORMAL:
            SOC_IF_ERROR_RETURN(phy_5421S_speed_get(unit, port, &speed));
            if (speed == 10 || speed == 100) {
                /* Set bit 14 for manual MDI crossover */
                SOC_IF_ERROR_RETURN  
                    (MODIFY_PHY5421_MII_ECRr(unit, pc, 0x4000, 0x4000));

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY5421_TEST1r(unit, pc, 0x0000));
            } else {
                return SOC_E_UNAVAIL;
            }
            break;

        case SOC_PORT_MDIX_XOVER:
            SOC_IF_ERROR_RETURN(phy_5421S_speed_get(unit, port, &speed));
            if (speed == 10 || speed == 100) {
                /* Set bit 14 for manual MDI crossover */
                 SOC_IF_ERROR_RETURN  
                    (MODIFY_PHY5421_MII_ECRr(unit, pc, 0x4000, 0x4000));

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY5421_TEST1r(unit, pc, 0x0080));
            } else {
                return SOC_E_PARAM;
            }
            break;
        default:
            rv = SOC_E_UNAVAIL;
        }

        if (rv == SOC_E_NONE) {
            pc->copper.mdix = mode;
        }

        break;

    default:
        if (mode != SOC_PORT_MDIX_NORMAL) {
            rv = SOC_E_UNAVAIL;
        }
        break;
    }

    return (rv);
}        

/*
 * Function:
 *        phy_5421S_mdix_get
 * Description:
 *        Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *        unit - Device number
 *        port - Port number
 *        mode - (Out) One of:
 *                BCM_PORT_MDIX_AUTO
 *                        Enable auto-MDIX when autonegotiation is enabled
 *                BCM_PORT_MDIX_FORCE_AUTO
 *                        Enable auto-MDIX always
 *                BCM_PORT_MDIX_NORMAL
 *                        Disable auto-MDIX
 *                BCM_PORT_MDIX_XOVER
 *                        Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *        SOC_E_UNAVAIL - feature unsupported by hardware
 *        SOC_E_XXX - other error
 */
STATIC int
phy_5421S_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);

    if (mode == NULL || pc == NULL) {
        return SOC_E_PARAM;
    }

    if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
        *mode = pc->copper.mdix;
    } else {
        *mode = SOC_PORT_MDIX_NORMAL;
    }

    return SOC_E_NONE;
}    

/*
 * Function:
 *        phy_5421S_mdix_status_get
 * Description:
 *        Get the current MDIX status on a port/PHY
 * Parameters:
 *        unit    - Device number
 *        port    - Port number
 *        status  - (OUT) One of:
 *                BCM_PORT_MDIX_STATUS_NORMAL
 *                        Straight connection
 *                BCM_PORT_MDIX_STATUS_XOVER
 *                        Crossover has been performed
 * Return Value:
 *        SOC_E_UNAVAIL - feature unsupported by hardware
 *        SOC_E_XXX - other error
 */
STATIC int
phy_5421S_mdix_status_get(int unit, soc_port_t port, 
                         soc_port_mdix_status_t *status)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);
    uint16             tmp;

    if (status == NULL || pc == NULL) {
        return SOC_E_PARAM;
    }

    if (pc->medium == SOC_PORT_MEDIUM_COPPER) {
        SOC_IF_ERROR_RETURN
            (READ_PHY5421_MII_ESRr(unit, pc, &tmp));
        if (tmp & 0x2000) {
            *status = SOC_PORT_MDIX_STATUS_XOVER;
        } else {
            *status = SOC_PORT_MDIX_STATUS_NORMAL;
        }
    } else {
        *status = SOC_PORT_MDIX_STATUS_NORMAL;
    }

    return SOC_E_NONE;
}    

#if LINK_DOWN_RESET_WAR

/*
 * Function:         
 *        phy_5421S_link_down
 * Purpose:        
 *        Perform reset work-around on link down.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 * Returns:        
 *        SOC_E_XXX
 * Notes:
 *        This work-around is necessary to ensure interoperability with
 *        different PHYs.
 */

STATIC int 
phy_5421S_link_down(int unit, soc_port_t port)
{
    phy_ctrl_t        *pc = EXT_PHY_SW_STATE(unit, port);
    uint16             mii_ctrl, mii_gb_ctrl, mii_advert, mii_fiber;

    DPRINTF((BSL_META_U(unit,
                        "5421S_link_down: port=%d m=%d\n"),
             port, pc->medium));

    /* Only applies to copper link down */

    if (pc->medium != SOC_PORT_MEDIUM_COPPER) {
        return SOC_E_NONE;
    }

    /* Save settings managed by the driver. */
    SOC_IF_ERROR_RETURN
        (READ_PHY5421_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHY5421_MII_ANAr(unit, pc, &mii_advert));
    SOC_IF_ERROR_RETURN
        (READ_PHY5421_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHY5421_SGMII_1000X_CTRLr(unit, pc, &mii_fiber));

    /* Perform normal reset and initialization. */

    SOC_IF_ERROR_RETURN(_phy_5421S_reset_setup(unit, port));

    /* Restore settings managed by the driver. */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY5421_MII_CTRLr(unit, pc, mii_ctrl));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY5421_MII_ANAr(unit, pc, mii_advert));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY5421_MII_GB_CTRLr(unit, pc, mii_gb_ctrl));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY5421_SGMII_1000X_CTRLr(unit, pc, mii_fiber));

    return SOC_E_NONE;
}

#else
#define phy_5421S_link_down NULL
#endif /* LINK_DOWN_RESET_WAR */

#ifdef BROADCOM_DEBUG

/*
 * Function:
 *        phy_5421S_shadow_dump
 * Purpose:
 *        Debug routine to dump all shadow registers.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #. 
 */

void
phy_5421S_shadow_dump(int unit, soc_port_t port)
{
    uint16               tmp;
    phy_ctrl_t          *pc;

    pc        = EXT_PHY_SW_STATE(unit, port);
    /* Hideous Register 0x18 */

    WRITE_PHY_REG(unit, pc, 0x18, 0x0007);
    READ_PHY_REG(unit, pc, 0x18, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x18[000]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x18, 0x1007);
    READ_PHY_REG(unit, pc, 0x18, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x18[001]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x18, 0x2007);
    READ_PHY_REG(unit, pc, 0x18, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x18[010]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x18, 0x4007);
    READ_PHY_REG(unit, pc, 0x18, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x18[100]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x18, 0x7007);
    READ_PHY_REG(unit, pc, 0x18, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x18[111]=0x%04x\n"), tmp));

    /* Hideous Register 0x1c */

    WRITE_PHY_REG(unit, pc, 0x1c, 0x0800);
    READ_PHY_REG(unit, pc, 0x1c, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x1c[00010]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x1c, 0x1000);
    READ_PHY_REG(unit, pc, 0x1c, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x1c[00100]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x1c, 0x1400);
    READ_PHY_REG(unit, pc, 0x1c, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x1c[00101]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x1c, 0x2000);
    READ_PHY_REG(unit, pc, 0x1c, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x1c[01000]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x1c, 0x2400);
    READ_PHY_REG(unit, pc, 0x1c, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x1c[01001]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x1c, 0x2800);
    READ_PHY_REG(unit, pc, 0x1c, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x1c[01010]=0x%04x\n"), tmp));
    WRITE_PHY_REG(unit, pc, 0x1c, 0x7c00);
    READ_PHY_REG(unit, pc, 0x1c, &tmp);
    LOG_CLI((BSL_META_U(unit,
                        "0x1c[11111]=0x%04x\n"), tmp));
}

#endif /* BROADCOM_DEBUG */




/*
 * Variable:        phy_5421Sdrv_ge
 * Purpose:        PHY driver for 5421S
 */
phy_driver_t phy_5421Sdrv_ge = {
    "5421S Gigabit PHY Driver",
    phy_5421S_init,
    phy_fe_ge_reset,
    phy_5421S_link_get, 
    phy_5421S_enable_set, 
    phy_5421S_enable_get, 
    phy_5421S_duplex_set, 
    phy_5421S_duplex_get, 
    phy_5421S_speed_set, 
    phy_5421S_speed_get, 
    phy_5421S_master_set, 
    phy_5421S_master_get, 
    phy_5421S_autoneg_set, 
    phy_5421S_autoneg_get, 
    phy_5421S_adv_local_set,
    phy_5421S_adv_local_get, 
    phy_5421S_adv_remote_get,
    phy_5421S_lb_set,
    phy_5421S_lb_get,
    phy_5421S_interface_set, 
    phy_5421S_interface_get, 
    phy_5421S_ability_get,
    NULL,                       /* Link up event */
    phy_5421S_link_down,
    phy_5421S_mdix_set,
    phy_5421S_mdix_get,
    phy_5421S_mdix_status_get,
    phy_5421S_medium_config_set,
    phy_5421S_medium_config_get,
    phy_5421S_medium_status,
    NULL,                       /* phy_cable_diag  */
    NULL,                       /* phy_link_change */
    NULL,                       /* phy_control_set */
    NULL,                       /* phy_control_get */
    phy_ge_reg_read,
    phy_ge_reg_write, 
    phy_ge_reg_modify,
    NULL                        /* Phy event notify */
};

#endif /* INCLUDE_PHY_5421S */

typedef int _soc_phy_5421S_not_empty; /* Make ISO compilers happy. */
