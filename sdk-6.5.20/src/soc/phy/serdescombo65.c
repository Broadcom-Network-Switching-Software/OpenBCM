/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *      physerdescombo65.c
 * Purpose:
 *      Fiber driver for 65nm combo serdes using as internal Serdes PHY.
 *
 * When operating with an external PHY, this driver is not used.
 * However the speed/duplex of the internal PHY must be programmed to
 * match the MAC and external PHY settings so the data can pass through.
 * This file supplies some routines to allow mac.c to accomplish this
 * (think of the internal PHY as part of the MAC in this case):
 *
 *      _phy_combo65_notify_duplex
 *      _phy_combo65_notify_speed
 *      _phy_combo65_notify_stop
 *      _phy_combo65_notify_resume
 *
 * MDIO accesses to the internal PHY are not modeled on Quickturn.
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_SERDES_COMBO65)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
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


#include "phyreg.h"
#include "phynull.h"
#include "serdescombo65.h"

#define ADVERT_ALL_FIBER \
            (SOC_PM_PAUSE | SOC_PM_1000MB_FD | SOC_PM_2500MB_FD)

/* Notify event forward declaration */
STATIC int
_phy_combo65_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int
_phy_combo65_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int
_phy_combo65_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int
_phy_combo65_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int
_phy_combo65_notify_interface(int unit, soc_port_t port, uint32 intf);
STATIC int
phy_combo65_an_set(int unit, soc_port_t port, int an);
STATIC int
phy_combo65_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int
phy_combo65_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode);

/***********************************************************************
 *
 * HELPER FUNCTIONS
 */
/*
 * Function:
 *      phy_combo65_tx_control_set
 * Purpose:
 *      Set preemphasis, driver current, and predriver current values 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - PHY state object.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
_phy_combo65_tx_control_set(int unit, phy_ctrl_t *pc,
                          soc_port_t port)
{
    uint32 preemph;
    uint32 idriver;
    uint32 pdriver;
    uint16 data16;
    int    fiber;

    fiber = PHY_FIBER_MODE(unit, port);

    /* Default values for Combo SerDes */
    if (fiber) {
        preemph = 0x9;
        idriver = 0xA;
        pdriver = 0xA;
    } else {
        preemph = 0x0;
        idriver = 0x0;
        pdriver = 0x0;
    }

    /* Read config override */
    preemph = soc_property_port_get(unit, port,
                                    spn_SERDES_PREEMPHASIS, preemph);
    idriver = soc_property_port_get(unit, port,
                                    spn_SERDES_DRIVER_CURRENT, idriver);
    pdriver = soc_property_port_get(unit, port,
                                    spn_SERDES_PRE_DRIVER_CURRENT, pdriver);

    /* Combo SerDes
     * preemph[15:12], idriver[11:8], pdriver[7:4]
     */
    preemph = (preemph << 12) & TX_DRIVER_PREEMPHASIS;
    idriver = (idriver << 8) & TX_DRIVER_IDRIVER;
    pdriver = (pdriver << 4) & TX_DRIVER_IPREDRIVER;
    data16  = (uint16)(preemph | idriver | pdriver);

    SOC_IF_ERROR_RETURN
        (MODIFY_TX_ALL_DRIVERr(unit, pc, data16, 
                               TX_DRIVER_PREEMPHASIS | TX_DRIVER_IDRIVER | 
                               TX_DRIVER_IPREDRIVER));

    return SOC_E_NONE;
}

/***********************************************************************
 *
 * FIBER DRIVER ROUTINES
 */
STATIC int
phy_combo65_init_no_reset(int unit, soc_port_t port)
{
    uint16             data16, mask16;
    uint16             mode_1000x;
    soc_timeout_t      to;
    phy_ctrl_t        *pc;
    int                rv;

    pc                       = INT_PHY_SW_STATE(unit, port);
    pc->fiber.enable         = PHY_FIBER_MODE(unit, port);
    pc->fiber.preferred      = PHY_FIBER_MODE(unit, port);
    pc->fiber.autoneg_enable = 1;
    pc->fiber.autoneg_advert = ADVERT_ALL_FIBER;
    pc->fiber.force_speed    = 1000;
    pc->fiber.force_duplex   = TRUE;
    pc->fiber.master         = SOC_PORT_MS_NONE;
    pc->fiber.mdix           = SOC_PORT_MDIX_NORMAL;

    /* Since physerdescombo driver reset function vector does not reset
     * the SerDes, reset the SerDes in initialization first.
     * The internal SERDES PHY's reset bit is self-clearing.
     */
    SOC_IF_ERROR_RETURN
        (WRITE_COMBO_IEEE0_MII_CTRLr(unit, pc, MII_CTRL_RESET));
    soc_timeout_init(&to, 10000, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_COMBO_IEEE0_MII_CTRLr(unit, pc, &data16);
        if (((data16 & MII_CTRL_RESET) == 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((data16 & MII_CTRL_RESET) != 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Combo SerDes reset failed: u=%d p=%d\n"),
                  unit, port));
    }
 
    /* Stop PLL Sequencer */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS_BLK0_CTRLr(unit, pc, 0, XGXS_CTRL_START_SEQUENCER));

    SOC_IF_ERROR_RETURN
        (phy_combo65_adv_local_set(unit, port, pc->fiber.autoneg_advert));

    /* Initialialize autoneg and fullduplex */
    data16 = MII_CTRL_FD | MII_CTRL_SS_1000;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) || 
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        data16 |= MII_CTRL_AE | MII_CTRL_RAN;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_COMBO_IEEE0_MII_CTRLr(unit, pc, data16));

    /* Configure default preemphasis, predriver, idriver values */
    SOC_IF_ERROR_RETURN
        (_phy_combo65_tx_control_set(unit, pc, port));

    /* Configuring operating mode */
    data16 = 0;
    mask16 = CTRL1_AUTODET_EN | CTRL1_FIBER_MODE;
    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_AUTOMEDIUM, FALSE)) {
        data16 |= CTRL1_AUTODET_EN;
    }

    /*
     * Put the Serdes in Fiber or SGMII mode
     */
    mode_1000x = 0;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port)) {
        mode_1000x = CTRL1_FIBER_MODE;
    }
    /* Allow property to override */
    if (soc_property_port_get(unit, port,
                              spn_SERDES_FIBER_PREF, mode_1000x)) {
        data16 |= CTRL1_FIBER_MODE;
    } else {
        data16 &= ~CTRL1_FIBER_MODE;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_DIGITAL_CTRL1r(unit, pc, data16, mask16));

    /* Start PLL Sequencer */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS_BLK0_CTRLr(unit, pc, XGXS_CTRL_START_SEQUENCER, 
                                XGXS_CTRL_START_SEQUENCER));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_init: u=%d p=%d %s\n"),
              unit, port, 
              PHY_FIBER_MODE(unit, port) ? "Fiber" : "Copper"));
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_combo65_init
 * Purpose:     
 *      Initialize serdescombo65 internal PHY driver
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 * Notes:
 *      This function implements the bulk part of phy_combo65_init and
 *      provides a method for initializing the SerDes when it is used
 *      as an intermediate PHY between the MAC and an external PHY. 
 *      In the latter case the PHY driver reset callback should be
 *      called only once, and this has already been done by the 
 *      external PHY driver init function.
 */

STATIC int
phy_combo65_init(int unit, soc_port_t port)
{
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_init: u=%d p=%d\n"),
              unit, port));

    if (!PHY_EXTERNAL_MODE(unit, port)) {
         /* PHY reset callbacks */
        SOC_IF_ERROR_RETURN(soc_phy_reset(unit, port));

        if (soc_property_port_get(unit, port,
                                  spn_SERDES_FIBER_PREF, 1)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        }

    
        if (soc_property_port_get(unit, port,
                              spn_PHY_AN_C73, FALSE)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_C73);
        }
        if (PHY_SGMII_AUTONEG_MODE(unit, port)) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "WARNING: SGMII autoneg enabled but no PHY attached:"
                                 " u=%d p=%d \n"), unit, port));
        }
    }

    SOC_IF_ERROR_RETURN
        (phy_combo65_init_no_reset(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_combo65_link_get
 * Purpose:     
 *      Determine the current link up/down status
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      link - (OUT) Boolean, true indicates link established.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      MII_STATUS bit 2 reflects link state.
 */
STATIC int
phy_combo65_link_get(int unit, soc_port_t port, int *link)
{
    uint16       mii_stat;
    uint16       mii_ctrl;
    int          an_done;
    phy_ctrl_t  *pc;

    if (PHY_DISABLED_MODE(unit, port)) {
        *link = FALSE;
    } else {
        pc = INT_PHY_SW_STATE(unit, port);

        SOC_IF_ERROR_RETURN
            (READ_COMBO_IEEE0_MII_STATr(unit, pc, &mii_stat));

        *link = ((mii_stat & MII_STAT_LA) != 0);

        SOC_IF_ERROR_RETURN
            (READ_COMBO_IEEE0_MII_CTRLr(unit, pc, &mii_ctrl));

        if (mii_ctrl & MII_CTRL_AE) { 
            /* If autoneg is enabled, check for autoneg done. */
            an_done = ((mii_stat & MII_STAT_AN_DONE) != 0);

            *link = (*link) && (an_done);
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_combo65_enable_set
 * Purpose:     
 *      Enable or disable the physical interface.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - Boolean, true = enable PHY, false = disable.
 * Returns:     
 *      SOC_E_XXX
 * Notes:
 *      The PHY is held in reset when disabled.  Also, the
 *      MISC_CONTROL.IDDQ bit must be set to 1 so the remote partner
 *      sees link down.
 */

STATIC int
phy_combo65_enable_set(int unit, soc_port_t port, int enable)
{
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_enable_set: u=%d p=%d en=%d\n"),
              unit, port, enable));
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_combo65_notify_resume(unit, port, PHY_STOP_PHY_DIS));
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_combo65_notify_stop(unit, port, PHY_STOP_PHY_DIS));
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_combo65_duplex_set
 * Purpose:     
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      duplex - Boolean, TRUE indicates full duplex, FALSE indicates half.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_duplex_set(int unit, soc_port_t port, int duplex)
{
    int rv;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    rv = duplex ? SOC_E_NONE : SOC_E_UNAVAIL;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_duplex_set: u=%d p=%d duplex=%d rv=%d\n"),
              unit, port, duplex, rv));

    return rv; 
}

/*
 * Function:    
 *      phy_combo65_speed_set
 * Purpose:     
 *      Set the current operating speed (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      duplex - (OUT) Boolean, true indicates full duplex, false 
 *              indicates half.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_speed_set(int unit, soc_port_t port, int speed)
{
    uint16       status;
    uint16       mii_ctrl, misc1;
    phy_ctrl_t  *pc;
    int          rv;

    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_DIGITAL_STAT1r(unit, pc, &status));

    if (status & STAT1_SGMII_MODE) {
        if ((speed != 0) && (speed != 10) && 
            (speed != 100) && (speed != 1000)) {
            return SOC_E_PARAM;
        }
    } else {
        if ((speed != 0) && (speed != 1000) && (speed != 2500)) {
            return SOC_E_PARAM;
        }
    }

    /* Stop Sequencer */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS_BLK0_CTRLr(unit, pc, 0, XGXS_CTRL_START_SEQUENCER));

    mii_ctrl = misc1 = 0;
    switch (speed) {
        case 10:
            mii_ctrl = MII_CTRL_SS_10;
            break;
        case 100:
            mii_ctrl = MII_CTRL_SS_100;
            break;
        case 0:
        case 1000:  
            mii_ctrl = MII_CTRL_SS_1000;
            break;
        case 2500:
            misc1 = MISC1_FORCE_SPEED_SEL | MISC1_FORCE_SPEED_2P5GB;
            break;
    } 
    SOC_IF_ERROR_RETURN
        (MODIFY_COMBO_IEEE0_MII_CTRLr(unit, pc, mii_ctrl, MII_CTRL_SS_MASK));

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_DIGITAL_MISC1r(unit, pc, misc1, 
                                  MISC1_FORCE_SPEED_SEL | MISC1_FORCE_SPEED));

    /* Start Sequencer */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS_BLK0_CTRLr(unit, pc, XGXS_CTRL_START_SEQUENCER, 
                                          XGXS_CTRL_START_SEQUENCER));

    pc->fiber.force_speed = speed;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_speed_set: u=%d p=%d speed=%d rv=%d\n"),
              unit, port, speed, rv));
    
    return rv;
}

/*
 * Function:    
 *      phy_combo65_speed_get
 * Purpose:     
 *      Get the current operating speed. If autoneg is enabled, 
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      duplex - (OUT) Boolean, true indicates full duplex, false 
 *              indicates half.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16       mii_ctrl, mii_stat, digi_stat;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_COMBO_IEEE0_MII_CTRLr(unit, pc, &mii_ctrl));
 
    if (mii_ctrl & MII_CTRL_AE) { /* Auto-negotiation enabled */  
        SOC_IF_ERROR_RETURN
            (READ_COMBO_IEEE0_MII_STATr(unit, pc, &mii_stat));
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *speed = 0;
            return SOC_E_NONE;
        }
    }

    SOC_IF_ERROR_RETURN
        (READ_SERDES_DIGITAL_STAT1r(unit, pc, &digi_stat));
    switch (digi_stat & STAT1_SPEED_STATUS) {
    case STAT1_SPEED_10MB:
         *speed = 10;
         break;
    case STAT1_SPEED_100MB:
         *speed = 100;
         break;
    case STAT1_SPEED_1000MB:
         *speed = 1000;
         break;
    case STAT1_SPEED_2P5GB:
         *speed = 2500;
         break;
    default:              /* Will never happen */
         *speed = 1000;
         break;
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_combo65_an_set
 * Purpose:     
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16       an_enable;
    uint16       auto_det;

    pc = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));
    
    an_enable = 0; 
    auto_det  = 0;
    if (an) {
        an_enable = MII_CTRL_AE | MII_CTRL_RAN;
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_AUTOMEDIUM, FALSE)) {
            auto_det = CTRL1_AUTODET_EN; 
        }
        /* Stop Sequencer */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS_BLK0_CTRLr(unit, pc, 0, XGXS_CTRL_START_SEQUENCER));
    
        SOC_IF_ERROR_RETURN
            (MODIFY_COMBO_IEEE0_MII_CTRLr(unit, pc, MII_CTRL_SS_1000, 
                                              MII_CTRL_SS_MASK));
    
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES_DIGITAL_MISC1r(unit, pc, 0, 
                                   MISC1_FORCE_SPEED_SEL | MISC1_FORCE_SPEED));
    
        /* Start Sequencer */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS_BLK0_CTRLr(unit, pc, XGXS_CTRL_START_SEQUENCER, 
                                              XGXS_CTRL_START_SEQUENCER));
    }

    /* Enable/Disable auto detect */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_DIGITAL_CTRL1r(unit, pc, auto_det,
                                            CTRL1_AUTODET_EN));
    /* Enable/Disable parallel detect */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_DIGITAL_CTRL2r(unit, pc, 
                                      an? CTRL2_ENABLE_PARALLEL_DETECT: 0,
                                      CTRL2_ENABLE_PARALLEL_DETECT));

    /* Enable/Disable and Restart autonegotiation (self-clearing bit) */
    SOC_IF_ERROR_RETURN
        (MODIFY_COMBO_IEEE0_MII_CTRLr(unit, pc, an_enable,
                                      MII_CTRL_AE | MII_CTRL_RAN));

    if (PHY_CLAUSE73_MODE(unit, port)) {
        if (an) {
            /* Enable BAM Station Manager */
            SOC_IF_ERROR_RETURN
                (WRITE_CL73_USERB0_BAMCTRL1r(unit, pc,
                        (CL73_BAMCTRL1_BAMEN | CL73_BAMCTRL1_STATION_MNGR_EN | 
                         CL73_BAMCTRL1_BAMNP_AFTER_BP_EN)));

            /* Merge CL73 and CL37 autoneg resolution */
            SOC_IF_ERROR_RETURN
                (MODIFY_CL73_USERB0_BAMCTRL3r(unit, pc,
                                              CL73_BAMCTRL3_USE_CL73_HCD_MR,
                                              CL73_BAMCTRL3_USE_CL73_HCD_MR));

            /* Enable CL73 Autoneg and restart negotiation */
            SOC_IF_ERROR_RETURN
                (WRITE_CL73_IEEEB0_AN_CTRLr(unit, pc, 
                                            (CL73_AN_CTRL_AUTONEG_EN |
                                             CL73_AN_CTRL_RES_NEGOTIATION)));
        } else {
            /* Disable CL73 autoneg */
            SOC_IF_ERROR_RETURN
                (WRITE_CL73_IEEEB0_AN_CTRLr(unit, pc, 0));
        }
    }

    pc->fiber.autoneg_enable = an;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_combo65_an_get
 * Purpose:     
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete.
 *              This value is undefined if an == false.
 * Returns:     
 *      SOC_E_XXX
 * Note:
 *      Clause 37 autoneg is always in sync with clause 73. 
 *      Therefore, reading C37 config is sufficient.
 */

STATIC int
phy_combo65_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      ctrl, stat;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_COMBO_IEEE0_MII_CTRLr(unit, pc, &ctrl));

    *an = (ctrl & MII_CTRL_AE) != 0;

    SOC_IF_ERROR_RETURN
        (READ_COMBO_IEEE0_MII_STATr(unit, pc, &stat));

    *an_done = (stat & MII_STAT_AN_DONE) != 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_an_get: u=%d p=%d an=%d an_done=%d\n"),
              unit, port, *an, *an_done));

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_combo65_adv_local_set
 * Purpose:     
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int 
phy_combo65_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    uint16              an_adv;
    phy_ctrl_t  *pc;

    /* Note:
     * We assume that switch SerDes is always used as slave when
     * the port is in SGMII mode. Therefore, the adv_local_set is applicable
     * only for fiber mode.
     */ 
    pc = INT_PHY_SW_STATE(unit, port);

    an_adv  = 0;

    /*
     * Set advertised duplex (only FD supported).
     */
    if (mode & SOC_PM_1000MB_FD) {
        an_adv |= MII_ANA_C37_FD;
    }

    /*
     * Set advertised pause bits in link code word.
     */
    switch (mode & SOC_PM_PAUSE) {
    case SOC_PM_PAUSE_TX:
        an_adv |= MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PM_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE;
        break;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_COMBO_IEEE0_MII_ANAr(unit, pc, an_adv));

     /* Advertise 2.5 Gbps */
    an_adv = (mode & SOC_PM_2500MB_FD) ? UP1_ADV_2P5GB : 0;
    SOC_IF_ERROR_RETURN
        (WRITE_OVER_1G_UP1r(unit, pc, an_adv));

    if (PHY_CLAUSE73_MODE(unit, port)) {
        an_adv = (mode & SOC_PM_1000MB_FD) ? CL73_AN_ADV2_LP_TECH_1G_KX : 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_CL73_IEEEB1_AN_ADV2r(unit, pc, an_adv,
                                         CL73_AN_ADV2_LP_TECH_1G_KX));
      
        switch (mode & SOC_PM_PAUSE) {
        case SOC_PM_PAUSE_TX:
            an_adv = CL73_AN_ADV1_LP_ASYM_PAUSE;
            break;
        case SOC_PM_PAUSE_RX:
            an_adv = CL73_AN_ADV1_LP_PAUSE | CL73_AN_ADV1_LP_ASYM_PAUSE;
            break;
        case SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX:
            an_adv = CL73_AN_ADV1_LP_PAUSE;
            break;
        default:
            an_adv = 0;
            break;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_CL73_IEEEB1_AN_ADV1r(unit, pc, an_adv,
                                         (CL73_AN_ADV1_LP_PAUSE | 
                                          CL73_AN_ADV1_LP_ASYM_PAUSE)));
        
        /* When CL73 BAM is enabled, CL73 simply serves as conduit to
         * CL37 and transmits next pages and abilities including over 1G. 
         * Therefore, 2.5G advertising will take affect according to
         * over 1G up1 register.
         */

    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_adv_local_set: u=%d p=%d adv=%s%s%s%s\n"),
              unit, port,
              (mode & SOC_PM_2500MB_FD) ? "2500MB " : "",
              (mode & SOC_PM_1000MB_FD) ? "1000MB " : "",
              (mode & SOC_PM_PAUSE_TX)  ? "PAUSE_TX " : "",    
              (mode & SOC_PM_PAUSE_RX)  ? "PAUSE_TX " : ""));    
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_combo65_adv_local_get
 * Purpose:     
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      an_adv;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_COMBO_IEEE0_MII_ANAr(unit, pc, &an_adv));

    if (an_adv & MII_ANA_C37_FD) {
        *mode |= SOC_PM_1000MB_FD;
    }

    switch (an_adv & (MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE)) {
    case MII_ANA_C37_PAUSE:
        *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
        break;
    case MII_ANA_C37_ASYM_PAUSE:
        *mode |= SOC_PM_PAUSE_TX;
        break;
    case MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE:
        *mode |= SOC_PM_PAUSE_RX;
        break;
    default:
        break;
    }

    SOC_IF_ERROR_RETURN
        (READ_OVER_1G_UP1r(unit, pc, &an_adv));
    *mode |= (an_adv & UP1_ADV_2P5GB) ?  SOC_PM_2500MB_FD : 0;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_combo65_adv_remote_get
 * Purpose:     
 *      Get partner's current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16           anlpa;
    soc_port_mode_t  duplex;
    phy_ctrl_t      *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_COMBO_IEEE0_MII_ANPr(unit, pc, &anlpa));

    *mode = 0;
    if (anlpa & MII_ANP_SGMII_MODE) {
        /* Link partner is in SGMII mode */
        switch (anlpa & MII_ANP_SGMII_SS_MASK) {
        case MII_ANP_SGMII_SS_10:
            *mode = SOC_PM_10MB;
            break;
        case MII_ANP_SGMII_SS_100:
            *mode = SOC_PM_100MB;
            break;
        case MII_ANP_SGMII_SS_1000:
            *mode = SOC_PM_1000MB;
            break;
        } 

        duplex = (anlpa & MII_ANP_SGMII_FD) ? SOC_PM_FD : SOC_PM_HD;

        *mode = (*mode) & duplex; 
    } else {
        /* Link partner is in 1000X/2500X mode */
        if (anlpa & MII_ANP_C37_FD) {
            *mode |= SOC_PM_1000MB_FD;
        }
    
        if (anlpa & MII_ANP_C37_HD) {
            *mode |= SOC_PM_1000MB_HD;
        }
    
        switch (anlpa &
                (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
        case MII_ANP_C37_PAUSE:
            *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
            break;
        case MII_ANP_C37_ASYM_PAUSE:
            *mode |= SOC_PM_PAUSE_TX;
            break;
        case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
            *mode |= SOC_PM_PAUSE_RX;
            break;
        }
        
        SOC_IF_ERROR_RETURN
            (READ_OVER_1G_LP_UP1r(unit, pc, &anlpa));
        *mode |= (anlpa & LP_UP1_ADV_2P5GB) ? 
                 SOC_PM_2500MB_FD : 0;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_adv_remote_get:"
                         " u=%d p=%d adv=%s%s%s%s%s%s%s%s\n"),
              unit, port,
              (*mode & SOC_PM_2500MB) ? "2500MB " : "",
              (*mode & SOC_PM_1000MB) ? "1000MB " : "",
              (*mode & SOC_PM_100MB) ? "100MB " : "",
              (*mode & SOC_PM_10MB) ? "10MB " : "",
              (*mode & SOC_PM_FD) ? "FD " : "",
              (*mode & SOC_PM_HD) ? "HD " : "",
              (*mode & SOC_PM_PAUSE_TX)  ? "PAUSE_TX " : "",    
              (*mode & SOC_PM_PAUSE_RX)  ? "PAUSE_TX " : ""));    

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_combo65_lb_set
 * Purpose:     
 *      Set the internal PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      loopback - Boolean: true = enable loopback, false = disable.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_lb_set(int unit, soc_port_t port, int enable)
{
    uint16              ctrl;
    phy_ctrl_t  *pc;
    int                 rv;

    pc = INT_PHY_SW_STATE(unit, port);

    ctrl = (enable) ? MII_CTRL_LE : 0;

    rv = MODIFY_COMBO_IEEE0_MII_CTRLr(unit, pc, ctrl, MII_CTRL_LE);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_lb_set: u=%d p=%d lb=%d rv=%d\n"),
              unit, port, enable, rv));
 
    return rv;
}

/*
 * Function:    
 *      phy_combo65_lb_get
 * Purpose:     
 *      Get the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_COMBO_IEEE0_MII_CTRLr(unit, pc, &ctrl));

    *enable = (ctrl & MII_CTRL_LE) != 0;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_combo65_interface_set
 * Purpose:
 *      Set the current operating mode of the internal PHY.
 *      (Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    int                 rv;

    switch (pif) {
    case SOC_PORT_IF_MII:
    case SOC_PORT_IF_GMII:
    case SOC_PORT_IF_SGMII:
    case SOC_PORT_IF_NOCXN:
        rv = SOC_E_NONE;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_interface_set: "
                         "u=%d p=%d pif=%d rv=%d\n"),
              unit, port, pif, rv));

    return rv;
}

/*
 * Function:
 *      phy_combo65_interface_get
 * Purpose:
 *      Get the current operating mode of the internal PHY.
 *      (Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_SGMII;

    return(SOC_E_NONE);
}

/*
 * Function:    
 *      phy_combo65_ability_get
 * Purpose:     
 *      Get the abilities of the internal PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_combo65_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16         fiber_status;
    phy_ctrl_t    *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES_DIGITAL_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & STAT1_SGMII_MODE) {
        *mode = SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB_FD |
                SOC_PM_LB_PHY | SOC_PM_GMII;
    } else {
        *mode = SOC_PM_2500MB_FD | SOC_PM_1000MB_FD | SOC_PM_PAUSE |
                SOC_PM_PAUSE_ASYMM | SOC_PM_LB_PHY | SOC_PM_AN |
                SOC_PM_GMII | SOC_PM_SGMII;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_combo65_medium_config_update
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
_phy_combo65_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_combo65_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_combo65_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_combo65_adv_local_set(unit, port, cfg->autoneg_advert));
    SOC_IF_ERROR_RETURN
        (phy_combo65_an_set(unit, port, cfg->autoneg_enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_combo65_medium_config_set
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
phy_combo65_medium_config_set(int unit, soc_port_t port, 
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{

    phy_ctrl_t    *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    /*
     * Changes take effect immediately if the target medium is active.
     */
    switch (medium) {
    case SOC_PORT_MEDIUM_FIBER:
        /* Change the fiber modes */

        sal_memcpy(&pc->fiber, cfg, sizeof (pc->fiber));

        if (PHY_EXTERNAL_MODE(unit, port)) {
            pc->fiber.autoneg_advert &= ADVERT_ALL_FIBER;
        } else {
            pc->fiber.autoneg_advert &= ADVERT_ALL_FIBER | SOC_PM_2500MB_FD;
        }
        if (PHY_FIBER_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (_phy_combo65_medium_config_update(unit, port, &pc->fiber));
        }
        return SOC_E_NONE;
    case SOC_PORT_MEDIUM_COPPER:
        return SOC_E_UNAVAIL;
    default:
        return SOC_E_PARAM;
    }
}

/*
 * Function:
 *      phy_combo65_medium_config_get
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
phy_combo65_medium_config_get(int unit, soc_port_t port, 
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    phy_ctrl_t    *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    switch (medium) {
    case SOC_PORT_MEDIUM_FIBER:
        sal_memcpy(cfg, &pc->fiber, sizeof (*cfg));
        return SOC_E_NONE;
    default:
        return SOC_E_PARAM;
    }
}

/*
 * Function:
 *      phy_combo65_medium_status
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
phy_combo65_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *medium = SOC_PORT_MEDIUM_FIBER;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_combo65_master_set
 * Purpose:
 *      Configure PHY master mode 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - PHY master mode. 
 * Returns:
 *      SOC_E_UNAVAIL
 */
STATIC int
phy_combo65_master_set(int unit, soc_port_t port, int master)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (master == SOC_PORT_MS_NONE) {
        return SOC_E_NONE;
    } 
    return SOC_E_PARAM;
}

/*
 * Function:
 *      phy_combo65_master_get
 * Purpose:
 *      Get current master mode setting
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) BCM_PORT_MS_NONE
 * Returns:
 *      SOC_E_NONE
 */ 
STATIC int 
phy_combo65_master_get(int unit, soc_port_t port, int *master)
{
    uint16         fiber_status;
    phy_ctrl_t    *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES_DIGITAL_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & STAT1_SGMII_MODE) {
        /* When in SGMII mode, the switch SerDes is always slave.  */
        *master = SOC_PORT_MS_SLAVE;
    } else {
        *master = SOC_PORT_MS_NONE;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_combo65_control_tx_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    uint16              data;  /* Temporary holder of reg value to be written */
    uint16              mask;  /* Bit mask of reg value to be updated */
    phy_ctrl_t  *pc;    /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    /* Combo SerDes */
    data = mask = 0;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         data = (uint16)(value << 12) & TX_DRIVER_PREEMPHASIS;
         mask = TX_DRIVER_PREEMPHASIS;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         data = (uint16)(value << 8) & TX_DRIVER_IDRIVER;
         mask = TX_DRIVER_IDRIVER;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         data = (uint16)(value << 4) & TX_DRIVER_IPREDRIVER;
         mask = TX_DRIVER_IPREDRIVER;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }
    /* Combo SerDes */
    SOC_IF_ERROR_RETURN
        (MODIFY_TX_ALL_DRIVERr(unit, pc, data, mask));        

    return SOC_E_NONE;
}

STATIC int
_phy_combo65_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    uint16                data16;   /* Temporary holder of a reg value */
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data16 = 0;
    /* Combo SerDes */
    SOC_IF_ERROR_RETURN
        (READ_TX_ALL_DRIVERr(unit, pc, &data16));
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         *value  = (data16 & TX_DRIVER_PREEMPHASIS) >> 12;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         *value  = (data16 & TX_DRIVER_IDRIVER) >> 8;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         *value = (data16 & TX_DRIVER_IPREDRIVER) >> 4; 
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_combo65_control_linkdown_transmit_set(int unit, soc_port_t port, 
                                           uint32 value)
{
    uint16        data;
    phy_ctrl_t    *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    if (value) {
        data = CTRL2_FORCE_XMIT_DATA_ON_TXSIDE | CTRL2_FILTER_FORCE_LINK |
               CTRL2_DISABLE_FALSE_LINK;
        SOC_IF_ERROR_RETURN
            (WRITE_SERDES_DIGITAL_CTRL2r(unit, pc, data));
    } else {
        data = CTRL2_FILTER_FORCE_LINK | CTRL2_DISABLE_FALSE_LINK |
               CTRL2_ENABLE_PARALLEL_DETECT;
        SOC_IF_ERROR_RETURN
            (WRITE_SERDES_DIGITAL_CTRL2r(unit, pc, data));
    }

    return SOC_E_NONE;
}


STATIC int
_phy_combo65_control_linkdown_transmit_get(int unit, soc_port_t port, 
                                           uint32 *value)
{
    uint16        data;
    phy_ctrl_t    *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES_DIGITAL_CTRL2r(unit, pc, &data));

    *value = (data & CTRL2_FORCE_XMIT_DATA_ON_TXSIDE) ? 1 : 0;

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_physerdescombo65_control_set
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
phy_combo65_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t        *pc;
    
    pc                       = INT_PHY_SW_STATE(unit, port);
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }
  
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_combo65_control_tx_driver_set(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_combo65_control_linkdown_transmit_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        /* enable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES_DIGITAL_CTRL2r(unit, pc,
               value? CTRL2_ENABLE_PARALLEL_DETECT:0,
               CTRL2_ENABLE_PARALLEL_DETECT));
        rv = SOC_E_NONE;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}
/*
 * Function:
 *      phy_combo65_control_get
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
phy_combo65_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t        *pc;
    uint16 data16;
 
    pc                       = INT_PHY_SW_STATE(unit, port);

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }
  
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_combo65_control_tx_driver_get(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_combo65_control_linkdown_transmit_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        SOC_IF_ERROR_RETURN
            (READ_SERDES_DIGITAL_CTRL2r(unit, pc,&data16));
        *value = data16 & CTRL2_ENABLE_PARALLEL_DETECT?  TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_combo65_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_combo65_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_read(unit, pc, phy_reg_addr, &data));

   *phy_data = data;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_combo65_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_combo65_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_write(unit, pc, phy_reg_addr, data));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_combo65_reg_modify
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_mo_data  - New value for the bits specified in phy_mo_mask
 *      phy_mo_mask  - Bit mask to modify
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_combo65_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data, uint32 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_modify(unit, pc, phy_reg_addr, data, mask));

    return SOC_E_NONE;
}

STATIC int
phy_combo65_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_combo65_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_combo65_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_combo65_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_combo65_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_combo65_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        rv = (phy_combo65_an_set(unit, port, value));
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

STATIC int
phy_combo65_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    *duplex = TRUE;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_DIGITAL_STAT1r(unit, pc, &reg0_16));

    if (reg0_16 & STAT1_SGMII_MODE) {
    /* retrieve the duplex setting in SGMII mode */

        SOC_IF_ERROR_RETURN
            (READ_COMBO_IEEE0_MII_CTRLr(unit, pc, &mii_ctrl));

        if (mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_COMBO_IEEE0_MII_ANPr(unit,pc,&reg0_16));

            /* make sure link partner is also in SGMII mode
             * otherwise fall through to use the FD bit in MII_CTRL reg
             */
            if (reg0_16 & MII_ANP_SGMII_MODE) {
                if (reg0_16 & MII_ANP_SGMII_FD) {
                    *duplex = TRUE;
                } else {
                    *duplex = FALSE;
                }
                return SOC_E_NONE;
            }
        }
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    }
    return SOC_E_NONE;
}

/*
 * Variable:    phy_serdescombo65_ge
 * Purpose:     PHY Driver for Dozen SerDes Serdes PHY internal to Draco, 
 */
phy_driver_t phy_serdescombo65_ge = {
    "Internal 2.5G 65nm SERDES PHY Driver",
    phy_combo65_init,
    phy_null_reset,
    phy_combo65_link_get, 
    phy_combo65_enable_set, 
    phy_null_enable_get, 
    phy_combo65_duplex_set, 
    phy_combo65_duplex_get,                /* duplex_get */
    phy_combo65_speed_set, 
    phy_combo65_speed_get, 
    phy_combo65_master_set,          /* master_set */
    phy_combo65_master_get,          /* master_get */
    phy_combo65_an_set,
    phy_combo65_an_get,
    phy_combo65_adv_local_set,
    phy_combo65_adv_local_get,
    phy_combo65_adv_remote_get,
    phy_combo65_lb_set,
    phy_combo65_lb_get,
    phy_combo65_interface_set, 
    phy_combo65_interface_get, 
    phy_combo65_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    phy_combo65_medium_config_set,
    phy_combo65_medium_config_get,
    phy_combo65_medium_status,
    NULL,                          /* phy_cable_diag  */
    NULL,                          /* phy_link_change */
    phy_combo65_control_set,         /* phy_control_set */
    phy_combo65_control_get,         /* phy_control_get */
    phy_combo65_reg_read,
    phy_combo65_reg_write,
    phy_combo65_reg_modify,
    phy_combo65_notify
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _phy_combo65_notify_duplex
 * Purpose:     
 *      Program duplex if (and only if) serdescombo65 is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      duplex - Boolean, TRUE indicates full duplex, FALSE
 *              indicates half.
 * Returns:     
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_combo65_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;

    fiber = PHY_FIBER_MODE(unit, port);
    pc    = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_combo65_notify_duplex: "
                         "u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_COMBO_IEEE0_MII_CTRLr(unit, pc, 
                                          MII_CTRL_FD, MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
        (_phy_combo65_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ? MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_COMBO_IEEE0_MII_CTRLr(unit, pc, mii_ctrl, MII_CTRL_FD)); 

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_combo65_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_combo65_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_combo65_notify_speed
 * Purpose:     
 *      Program duplex if (and only if) serdescombo65 is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - Speed to program.
 * Returns:     
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_combo65_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int          fiber;
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    fiber = PHY_FIBER_MODE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_combo65_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }
 
    SOC_IF_ERROR_RETURN
        (READ_SERDES_DIGITAL_STAT1r(unit, pc, &fiber_status));

    if (fiber && !(fiber_status & STAT1_SGMII_MODE)) {
        if ((speed != 0) && (speed != 1000)) {
            return SOC_E_CONFIG;
        }
    }

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_combo65_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (phy_combo65_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_combo65_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && !fiber) {
        SOC_IF_ERROR_RETURN
            (phy_combo65_an_set(unit, port, FALSE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_combo65_stop
 * Purpose:
 *      Put serdescombo65 SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_combo65_stop(int unit, soc_port_t port)
{
    uint16              mii_ctrl;
    int                 stop, copper;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    copper = (pc->stop & 
              PHY_STOP_COPPER) != 0;

    stop = ((pc->stop &
             (PHY_STOP_PHY_DIS |
              PHY_STOP_DRAIN)) != 0 ||
            (copper &&
             (pc->stop &
              (PHY_STOP_MAC_DIS |
               PHY_STOP_DUPLEX_CHG |
               PHY_STOP_SPEED_CHG)) != 0));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_combo65_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    mii_ctrl = (stop) ? MII_CTRL_PD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_COMBO_IEEE0_MII_CTRLr(unit, pc, mii_ctrl, MII_CTRL_PD)); 

    return SOC_E_NONE; 
}

/*
 * Function:
 *      _phy_combo65_notify_stop
 * Purpose:     
 *      Add a reason to put serdescombo65 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_combo65_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;

    return _phy_combo65_stop(unit, port);
}

/*
 * Function:
 *      _phy_combo65_notify_resume
 * Purpose:     
 *      Remove a reason to put serdescombo65 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_combo65_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;

    return _phy_combo65_stop(unit, port);
}

/*
 * Function:
 *      phy_combo65_media_setup
 * Purpose:     
 *      Configure 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      fiber_mode - Configure for fiber mode
 *      fiber_pref - Fiber preferrred (if fiber mode)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
_phy_combo65_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;

    pc = INT_PHY_SW_STATE(unit, port);

    data16 = 0;
    if (intf != SOC_PORT_IF_SGMII) {
        data16 = CTRL1_FIBER_MODE;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_DIGITAL_CTRL1r(unit, pc, data16, 
                                      CTRL1_FIBER_MODE));

    return SOC_E_NONE;
}
#else /* INCLUDE_SERDES_COMBO65 */
typedef int _phy_serdescombo65_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_SERDES_COMBO65 */ 

