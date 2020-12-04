/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *      physerdes100fx.c
 * Purpose:
 *      Fiber driver for serdes100fx using internal Serdes PHY.
 *
 * For BCM56514, the internal GMACC core of the GPORT should always be
 * configured to run in SGMII mode.  TBI mode is not supported between
 * the GMACC and the internal SERDES module.
 * 
 * When operating with an external PHY, this driver is not used.
 * However the speed/duplex of the internal PHY must be programmed to
 * match the MAC and external PHY settings so the data can pass through.
 * This file supplies some routines to allow mac.c to accomplish this
 * (think of the internal PHY as part of the MAC in this case):
 *
 *      phy_serdes100fx_notify_duplex
 *      phy_serdes100fx_notify_speed
 *      phy_serdes100fx_notify_stop
 *      phy_serdes100fx_notify_resume
 *
 * CMIC MIIM operations can be performed to the internal register set
 * using internal MDIO address (PORT_TO_PHY_ADDR_INT), and an external
 * PHY register set (such as BCM5424/34/64) can be programmed using the
 * external MDIO address (PORT_TO_PHY_ADDR).
 *
 * MDIO accesses to the internal PHY are not modeled on Quickturn.
 */

#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_SERDES_100FX)
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
#include "phynull.h"
#include "serdes.h"
#include "serdes100fx.h"

#define ADVERT_ALL_FIBER \
            (SOC_PM_PAUSE | SOC_PM_1000MB_FD)

/* Notify event forward declaration */
STATIC int 
_phy_serdes100fx_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int 
_phy_serdes100fx_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int 
_phy_serdes100fx_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int 
_phy_serdes100fx_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int
_phy_serdes100fx_notify_interface(int unit, soc_port_t port, uint32 intf);
STATIC int
phy_serdes100fx_duplex_set(int unit, soc_port_t port, int duplex);

/***********************************************************************
 *
 * HELPER FUNCTIONS
 */
/*
 * Function:
 *      phy_serdes100fx_tx_control_set
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
_phy_serdes100fx_tx_control_set(int unit, phy_ctrl_t *pc,
                          soc_port_t port)
{
    uint32 preemph;
    uint32 idriver;
    uint32 pdriver;
    uint16 data16;
    int    fiber;

    fiber = PHY_FIBER_MODE(unit, port);

    /* Default values for Dozen SerDes */
    if (fiber) {
        idriver = 0xf;
        pdriver = 0x7;
        preemph = 0x0;
    } else {
        idriver = 0xc;
        pdriver = 0x7;
        preemph = 0x0;
    }

    /* Read config override */
    preemph = soc_property_port_get(unit, port, 
                                    spn_SERDES_PREEMPHASIS, preemph);
    idriver = soc_property_port_get(unit, port,
                                    spn_SERDES_DRIVER_CURRENT, idriver);
    pdriver = soc_property_port_get(unit, port,
                                    spn_SERDES_PRE_DRIVER_CURRENT, pdriver);

    /* Dozen SerDes 
     * preemph[8:6], idriver[15:12], pdriver[11:9]
     */
    preemph = (preemph << 6) & 0x01c0;
    idriver = (idriver << 12) & 0xf000;
    pdriver = (pdriver << 9) & 0x0e00;
    data16  = (uint16)(preemph | idriver | pdriver);

    /* If amplitude override is not enabled, the amplitude value is 
     * automatically adjusted when interface changes between SGMII and 
     * fiber(1000X).
     */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_1000X_CTRL1r(unit, pc,
              FX_1000X_TX_AMPLITUDE_OVRD, FX_1000X_TX_AMPLITUDE_OVRD)); 

    /* Set ANALOG_TX with recommended amplitude */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_ANALOG_TXr(unit, pc, data16, 0xffc0));
    return SOC_E_NONE;
}

/***********************************************************************
 *
 * FIBER DRIVER ROUTINES
 */
STATIC int
_phy_serdes100fx_init_no_reset(int unit, soc_port_t port)
{
    int                fiber;
    uint16             data16, mask16;
    phy_ctrl_t        *pc;

    pc                       = INT_PHY_SW_STATE(unit, port);
    fiber                    = PHY_FIBER_MODE(unit, port);
    pc->fiber.enable         = fiber;
    pc->fiber.preferred      = fiber;
    pc->fiber.autoneg_enable = 1;
    pc->fiber.autoneg_advert = ADVERT_ALL_FIBER;
    pc->fiber.force_speed    = 1000;
    pc->fiber.force_duplex   = TRUE;
    pc->fiber.master         = SOC_PORT_MS_NONE;
    pc->fiber.mdix           = SOC_PORT_MDIX_NORMAL;

    /* Since physerdes100fx driver reset function vector does not reset
     * the SerDes, reset the SerDes in initialization first. 
     * The internal SERDES PHY's reset bit is self-clearing.
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_MII_CTRLr(unit, pc,
                               FX_MII_CTRL_RESET, FX_MII_CTRL_RESET));

    sal_usleep(10000);

    data16 = FX_MII_CTRL_AN_ENABLE | FX_MII_CTRL_AN_RESTART;
    mask16 = FX_MII_CTRL_AN_ENABLE | FX_MII_CTRL_AN_RESTART;
    if (PHY_COPPER_MODE(unit, port) && !PHY_SGMII_AUTONEG_MODE(unit, port)) {
        data16 = 0;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_MII_CTRLr(unit, pc, data16, mask16));

    /* Configure default preemphasis, predriver, idriver values */
    SOC_IF_ERROR_RETURN
        (_phy_serdes100fx_tx_control_set(unit, pc, port));

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_1000X_CTRL2r(unit, pc, 
                       (FX_1000X_FALSE_LNK_DIS | FX_1000X_FLT_FORCE_EN),
                       (FX_1000X_FALSE_LNK_DIS | FX_1000X_FLT_FORCE_EN)));

    data16 = 0;
    mask16 = FX_1000X_AUTO_DETECT | FX_1000X_FIBER_MODE;
    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_AUTOMEDIUM, FALSE)) {
        data16 |= FX_1000X_AUTO_DETECT;
    }
    /*
     * Put the Serdes in Fiber or SGMII mode
     */
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port)) {
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_FIBER_PREF, TRUE)) {
            data16 |= FX_1000X_FIBER_MODE;
        }
    }
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_SGMII_MASTER, FALSE)) {
        data16 |= FX_1000X_SGMII_MASTER;
        mask16 |= FX_1000X_SGMII_MASTER;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_1000X_CTRL1r(unit, pc, data16, mask16));   

    data16 = FX_1000X_CLRAR_BER_CNTR | FX_1000X_FLT_FORCE_EN |
             FX_1000X_FALSE_LNK_DIS;
    mask16 = FX_1000X_CLRAR_BER_CNTR | FX_1000X_FLT_FORCE_EN |
             FX_1000X_FALSE_LNK_DIS | FX_1000X_PAR_DET_EN;

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_1000X_CTRL2r(unit, pc, data16, mask16));

    if (PHY_COPPER_MODE(unit, port)) {
        /* Do not wait 10ms for sync status before a valid link is 
         * established when auto-negotiation is disabled.
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES100FX_1000X_CTRL2r(unit, pc, 0x0000, 0x0004));
    }
   
    /* Enable fast timer */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_FX100_CTRL3r(unit, pc, 
                                         FX_FX100_CONTROL3_FAST_TIMER, 
                                         FX_FX100_CONTROL3_FAST_TIMER));

    /* Allow extended packet length ( > 9 kilo bytes) */
    /* NB: The link takes longer to link up if extended packet length
     *     is enabled.
     */  
    SOC_IF_ERROR_RETURN
        (WRITE_SERDES100FX_FX100_CTRL2r(unit, pc, 
                                        FX_FX100_CONTROL2_EXT_PKT_SZ));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes100fx_init: u=%d p=%d %s\n"),
              unit, port, (fiber) ? "Fiber" : "Copper"));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_serdes100fx_init
 * Purpose:     
 *      Initialize serdes100fx internal PHY driver
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 * Notes:
 *      This function implements the bulk part of phy_serdes100fx_init and
 *      provides a method for initializing the SerDes when it is used
 *      as an intermediate PHY between the MAC and an external PHY. 
 *      In the latter case the PHY driver reset callback should be
 *      called only once, and this has already been done by the 
 *      external PHY driver init function.
 */

STATIC int
phy_serdes100fx_init(int unit, soc_port_t port)
{

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes100fx_init: u=%d p=%d\n"),
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

    }
    SOC_IF_ERROR_RETURN
        (_phy_serdes100fx_init_no_reset(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdes100fx_enable_set
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
phy_serdes100fx_enable_set(int unit, soc_port_t port, int enable)
{
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes100fx_enable_set: u=%d p=%d en=%d\n"),
              unit, port, enable));
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_serdes100fx_notify_resume(unit, port, PHY_STOP_PHY_DIS));
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_serdes100fx_notify_stop(unit, port, PHY_STOP_PHY_DIS));
    }

    return SOC_E_NONE;
}


STATIC int
_phy_serdes100fx_sgmii_speed_set(int unit, soc_port_t port, int speed)
{
    uint16 mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    switch (speed) {
        case 10:
            mii_ctrl = FX_MII_CTRL_SS_10;
            break;
        case 100:
            mii_ctrl = FX_MII_CTRL_SS_100;
            break;
        case 0:
        case 1000:  
            mii_ctrl = FX_MII_CTRL_SS_1000;
            break;
        default:
            return SOC_E_CONFIG;
    } 
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_MII_CTRLr(unit, pc, mii_ctrl,
                               (FX_MII_CTRL_SS_LSB | FX_MII_CTRL_SS_MSB)));
    return SOC_E_NONE;
}

STATIC int
_phy_serdes100fx_1000x_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    uint16       data, mask;

    pc = INT_PHY_SW_STATE(unit, port);


    if ((speed != 0) && (speed != 100) && (speed != 1000)) { 
        return SOC_E_CONFIG;
    }

    /* Stop MAC clock  */
    data = mask = 0x0060;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_FX100_CTRL2r(unit, pc, data, mask));

    mask = FX_FX100_CONTROL1_EN; 
    data = (speed == 100) ? FX_FX100_CONTROL1_EN : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_FX100_CTRL1r(unit, pc, data, mask));

    /* Reset analog clock */
    SOC_IF_ERROR_RETURN
        (WRITE_SERDES100FX_ANALOG_RX1r(unit, pc,
                    FX_ANALOG_RX1_RESET));

    SOC_IF_ERROR_RETURN
         (WRITE_SERDES100FX_ANALOG_RX1r(unit, pc,
                    FX_ANALOG_RX1_SIG_DET | FX_ANALOG_RX1_EDGE_SEL));
    sal_usleep(500);

    /* Allow extended packet length ( > 9 kilo bytes) */
    /* NB: The link takes longer to link up if extended packet length
     *     is enabled. Need to set this everytime speed changes.
     */  
    SOC_IF_ERROR_RETURN
        (WRITE_SERDES100FX_FX100_CTRL2r(unit, pc, 
                                        FX_FX100_CONTROL2_EXT_PKT_SZ));
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdes100fx_speed_set
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
phy_serdes100fx_speed_set(int unit, soc_port_t port, int speed)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;
    int                 rv;

    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & FX_1000X_STATUS1_SGMII_MODE) {
        rv = _phy_serdes100fx_sgmii_speed_set(unit, port, speed);
    } else {
        rv = _phy_serdes100fx_1000x_speed_set(unit, port, speed);
    }

    if (SOC_SUCCESS(rv)) {
        pc->fiber.force_speed = speed;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes100fx_speed_set: u=%d p=%d speed=%d rv=%d\n"),
              unit, port, speed, rv));
    
    return rv;
}

STATIC int 
_phy_serdes100fx_sgmii_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16 mii_ctrl, mii_stat, mii_anp;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_MII_STATr(unit, pc, &mii_stat));
 
    if (mii_ctrl & FX_MII_CTRL_AN_ENABLE) { /*Auto-negotiation enabled */  
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *speed = 0;
            return SOC_E_NONE;
        } else { /* Read Advertise link partner */
            SOC_IF_ERROR_RETURN
                (READ_SERDES100FX_MII_ANPr(unit, pc, &mii_anp));
            if (mii_anp & FX_MII_ANP_SGMII) {
                switch(mii_anp & FX_MII_ANP_SGMII_SPEED) {
                case FX_MII_ANP_SGMII_SPEED_1000:
                    *speed = 1000;
                    break;
                case FX_MII_ANP_SGMII_SPEED_100:
                    *speed = 100;
                    break;
                case FX_MII_ANP_SGMII_SPEED_10:
                    *speed = 10;
                    break;
                default:
                    return SOC_E_UNAVAIL;
                }
            }
        }
    } else {  /* Forced speed */
        switch(mii_ctrl & (FX_MII_CTRL_SS_LSB | FX_MII_CTRL_SS_MSB)) {
        case FX_MII_CTRL_SS_10:
            *speed = 10; 
            break;
        case FX_MII_CTRL_SS_100:
            *speed = 100;
            break;
        case FX_MII_CTRL_SS_1000:
            *speed = 1000;
            break;
        default:
            return SOC_E_UNAVAIL;
        } 
    }
    return SOC_E_NONE;
}

STATIC int
_phy_serdes100fx_1000x_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t  *pc;
    uint16       fx100_control;

    pc = INT_PHY_SW_STATE(unit, port);

    *speed = 1000;

    SOC_IF_ERROR_RETURN 
        (READ_SERDES100FX_FX100_CTRL1r(unit, pc, &fx100_control));
    *speed = (fx100_control & FX_FX100_CONTROL1_EN) ? 100 : 1000; 

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdes100fx_speed_get
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
phy_serdes100fx_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *speed = 1000;

    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & FX_1000X_STATUS1_SGMII_MODE) {
        SOC_IF_ERROR_RETURN
            (_phy_serdes100fx_sgmii_speed_get(unit, port, speed));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_serdes100fx_1000x_speed_get(unit, port, speed));
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdes100fx_an_set
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
phy_serdes100fx_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16             par_det;
    uint16             an_enable;
    uint16             auto_det;

    pc = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes100fx_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));
    /*
     * Special case for BCM5421S being used in SGMII copper mode with
     * BCMserdes100fx.  Always have autoneg on.
     */
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_5421S)) {
        an = 1;
    }

    par_det   = 0;
    an_enable = 0; 
    auto_det  = 0;
    if (an) {
        an_enable = FX_MII_CTRL_AN_ENABLE | FX_MII_CTRL_AN_RESTART;
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_AUTOMEDIUM, FALSE)) {
            auto_det = FX_1000X_AUTO_DETECT; 
        }
    } else {
        /* Enable parallel detect if autoneg is disabled so that
         * parallel detect can assist in determining port speed */
        par_det = FX_1000X_PAR_DET_EN;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_1000X_CTRL2r(unit, pc,
                        par_det, FX_1000X_PAR_DET_EN));

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_1000X_CTRL1r(unit, pc, 
                       auto_det, FX_1000X_AUTO_DETECT));

    /* Enable/Disable and Restart autonegotiation (self-clearing bit) */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_MII_CTRLr(unit, pc, an_enable,
                       FX_MII_CTRL_AN_ENABLE | FX_MII_CTRL_AN_RESTART));

    pc->fiber.autoneg_enable = an;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdes100fx_lb_set
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
phy_serdes100fx_lb_set(int unit, soc_port_t port, int enable)
{
    uint16              ctrl;
    phy_ctrl_t  *pc;
    int                 rv;

    pc = INT_PHY_SW_STATE(unit, port);

    ctrl = (enable) ?FX_MII_CTRL_LOOPBACK : 0;

    rv = MODIFY_SERDES100FX_MII_CTRLr(unit, pc,
                               ctrl, FX_MII_CTRL_LOOPBACK);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes100fx_lb_set: u=%d p=%d lb=%d rv=%d\n"),
              unit, port, enable, rv));
 
    return rv;
}

/*
 * Function:    
 *      phy_serdes100fx_lb_get
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
phy_serdes100fx_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_MII_CTRLr(unit, pc, &ctrl));

    *enable = (ctrl & FX_MII_CTRL_LOOPBACK) != 0;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_serdes100fx_interface_set
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
phy_serdes100fx_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
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
                         "phy_serdes100fx_interface_set: u=%d p=%d pif=%d rv=%d\n"),
              unit, port, pif, rv));

    return rv;
}

/*
 * Function:
 *      phy_serdes100fx_interface_get
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
phy_serdes100fx_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    *pif = SOC_PORT_IF_GMII;

    return(SOC_E_NONE);
}

/*
 * Function:    
 *      phy_serdes100fx_ability_get
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
phy_serdes100fx_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    if (PHY_FIBER_MODE(unit, port)) {
        *mode = SOC_PM_1000MB_FD | SOC_PM_PAUSE | SOC_PM_AN | 
                SOC_PM_PAUSE_ASYMM |
                SOC_PM_LB_PHY | SOC_PM_GMII | SOC_PM_100MB_FD;
    } else { 
        *mode = SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB_FD |
                SOC_PM_PAUSE_ASYMM |
                SOC_PM_PAUSE | SOC_PM_AN | SOC_PM_LB_PHY | SOC_PM_GMII;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_serdes100fx_medium_config_update
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
_phy_serdes100fx_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_serdes100fx_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_serdes100fx_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_serdes_adv_local_set(unit, port, cfg->autoneg_advert));
    SOC_IF_ERROR_RETURN
        (phy_serdes100fx_an_set(unit, port, cfg->autoneg_enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_serdes100fx_medium_config_set
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
phy_serdes100fx_medium_config_set(int unit, soc_port_t port, 
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

        pc->fiber.autoneg_advert &= ADVERT_ALL_FIBER;
        if (PHY_FIBER_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (_phy_serdes100fx_medium_config_update(unit, port, &pc->fiber));
        }
        return SOC_E_NONE;
    case SOC_PORT_MEDIUM_COPPER:
        return SOC_E_UNAVAIL;
    default:
        return SOC_E_PARAM;
    }
}


STATIC int
_phy_serdes100fx_control_linkdown_transmit_set(int unit, soc_port_t port, 
                                               uint32 value)
{
    uint16        data;
    phy_ctrl_t    *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    if (value) {
        data = FX_1000X_FORCE_XMIT_TX | FX_1000X_FLT_FORCE_EN |
               FX_1000X_FALSE_LNK_DIS;
        SOC_IF_ERROR_RETURN
            (WRITE_SERDES100FX_1000X_CTRL2r(unit, pc, data));
    } else {
        data = FX_1000X_FLT_FORCE_EN | FX_1000X_FALSE_LNK_DIS |
               FX_1000X_PAR_DET_EN;
        SOC_IF_ERROR_RETURN
            (WRITE_SERDES100FX_1000X_CTRL2r(unit, pc, data));
    }

    return SOC_E_NONE;
}


STATIC int
_phy_serdes100fx_control_linkdown_transmit_get(int unit, soc_port_t port, 
                                               uint32 *value)
{
    uint16        data;
    phy_ctrl_t    *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_1000X_CTRL2r(unit, pc, &data));

    *value = (data & FX_1000X_FORCE_XMIT_TX) ? 1 : 0;

    return SOC_E_NONE;
}

STATIC int
_phy_serdes100fx_control_tx_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    uint16       data;  /* Temporary holder of reg value to be written */
    uint16       mask;  /* Bit mask of reg value to be updated */
    phy_ctrl_t  *pc;    /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    /* Dozen SerDes */
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         data = (uint16)(value & 0x7);
         data = data << 6;
         mask = 0x01C0;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         data = (uint16)(value & 0xF);
         data = data << 12;
         mask = 0xF000;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         data = (uint16)(value & 0x7);
         data = data << 9;
         mask = 0x0E00;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_ANALOG_TXr(unit, pc, data, mask));

    return SOC_E_NONE;
}

STATIC int
_phy_serdes100fx_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    uint16         data16;   /* Temporary holder of a reg value */
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data16 = 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_ANALOG_TXr(unit, pc, &data16));
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         *value = (data16 & 0x01C0) >> 6;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         *value = (data16 & 0xF000) >> 12;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         *value = (data16 & 0x0E00) >> 9;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_physerdes100fx_control_set
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
phy_serdes100fx_control_set(int unit, soc_port_t port,
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
        rv = _phy_serdes100fx_control_tx_driver_set(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_serdes100fx_control_linkdown_transmit_set(unit, port, value);
        break;

    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        /* enable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES100FX_1000X_CTRL2r(unit, pc,
                value? FX_1000X_PAR_DET_EN:0,
               FX_1000X_PAR_DET_EN));
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
 *      phy_serdes100fx_control_get
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
phy_serdes100fx_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t        *pc;
    uint16 data16 = 0;

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
        rv = _phy_serdes100fx_control_tx_driver_get(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_serdes100fx_control_linkdown_transmit_get(unit, port, value);
        break;

    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        SOC_IF_ERROR_RETURN
            (READ_SERDES100FX_1000X_CTRL2r(unit, pc,&data16));
        *value = data16 & FX_1000X_PAR_DET_EN?  TRUE: FALSE;
        rv = SOC_E_NONE;
        break;

    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

STATIC int
_phy_serdes100fx_tx_fifo_reset(int unit, soc_port_t port,uint32 speed)
{
    uint16 data16;
    phy_ctrl_t  *pc;
    pc = INT_PHY_SW_STATE(unit, port);

    if (speed == 100) {
        /* check if it is in 100fx mode */
        SOC_IF_ERROR_RETURN
            (READ_SERDES100FX_FX100_CTRL1r(unit,pc,&data16));

        if (data16 & FX_FX100_CONTROL1_EN) {
            /* reset TX FIFO  */
            SOC_IF_ERROR_RETURN
                (MODIFY_SERDES100FX_1000X_CTRL4r(unit,pc,
                                 FX_1000X_DIG_RESET,
                                 FX_1000X_DIG_RESET));
            SOC_IF_ERROR_RETURN
                (MODIFY_SERDES100FX_1000X_CTRL4r(unit,pc,
                                 0,
                                 FX_1000X_DIG_RESET));
        }
    }
    return SOC_E_NONE;
}

STATIC int
phy_serdes100fx_notify(int unit, soc_port_t port,
                 soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_serdes100fx_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_serdes100fx_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_serdes100fx_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_serdes100fx_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_serdes100fx_notify_resume(unit, port, value));
        break;
    case phyEventTxFifoReset:
        rv = (_phy_serdes100fx_tx_fifo_reset(unit, port, value));
        break;
    default:
        rv = SOC_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      phy_serdes100fx_duplex_get
 * Purpose:
 *      Get the current operating duplex mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_serdes100fx_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       data16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;
                                                                                
    pc = INT_PHY_SW_STATE(unit, port);
                                                                                
    *duplex = TRUE;
                                                                                
    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_1000X_STAT1r(unit, pc, &data16));

    if (!(data16 & FX_1000X_STATUS1_SGMII_MODE)) { /* fiber mode */
        *duplex = (data16 & FX_1000X_STATUS1_FULL_DUPLEX)?
                   TRUE:FALSE; 
    } else {
                                                                                
        /* retrieve the duplex setting in SGMII mode */
                                                                                
        SOC_IF_ERROR_RETURN
            (READ_SERDES100FX_MII_CTRLr(unit, pc, &mii_ctrl));
                                                                                
        if (mii_ctrl & FX_MII_CTRL_AN_ENABLE) {
            SOC_IF_ERROR_RETURN
                (READ_SERDES100FX_MII_ANPr(unit,pc,&data16));
                                                                                
            /* make sure link partner is also in SGMII mode
             * otherwise fall through to use the FD bit in MII_CTRL reg
             */
            if (data16 & FX_MII_ANP_SGMII) {
                                                                                
                /* test shows the duplex setting of external phy is
                 * propagated to the AN link partner register
                 */
                if (data16 & FX_MII_ANP_SGMII_DUPLEX) {
                    *duplex = TRUE;
                    return SOC_E_NONE;
                } else {
                    *duplex = FALSE;
                    return SOC_E_NONE;
                }
            }
        }
        *duplex = (mii_ctrl & FX_MII_CTRL_FD) ? TRUE : FALSE;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_serdes100fx_duplex_set  
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:  
 *      unit - StrataSwitch unit #.  
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_serdes100fx_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16       data16;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_1000X_STAT1r(unit, pc, &data16));

    if (!(data16 & FX_1000X_STATUS1_SGMII_MODE)) {  /* fiber mode */

        /* set only for 100FX. 1000X always fd */
        SOC_IF_ERROR_RETURN
            (READ_SERDES100FX_FX100_CTRL1r(unit, pc,&data16));

        if (data16 & FX_FX100_CONTROL1_EN) {
            SOC_IF_ERROR_RETURN
                (MODIFY_SERDES100FX_FX100_CTRL1r(unit, pc,
                   duplex? FX_FX100_CONTROL1_FD:0,
                   FX_FX100_CONTROL1_FD));
        }
    } else {  /* SGMII mode */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES100FX_MII_CTRLr(unit, pc,
                            duplex? MII_CTRL_FD: 0,
                            MII_CTRL_FD));
    }
    return SOC_E_NONE;
}


/*
 * Variable:    phy_serdes100fxdrv_ge
 * Purpose:     PHY Driver for Dozen SerDes Serdes PHY internal to Draco, 
 */
phy_driver_t phy_serdes100fx_ge = {
    "Internal SERDES(100FX) PHY Driver",
    phy_serdes100fx_init,
    phy_null_reset,
    phy_serdes_link_get, 
    phy_serdes100fx_enable_set, 
    phy_null_enable_get, 
    phy_serdes100fx_duplex_set, 
    phy_serdes100fx_duplex_get,                    /* duplex_get */
    phy_serdes100fx_speed_set, 
    phy_serdes100fx_speed_get, 
    phy_serdes_master_set,          /* master_set */
    phy_serdes_master_get,          /* master_get */
    phy_serdes100fx_an_set,
    phy_serdes_an_get,
    phy_serdes_adv_local_set,
    phy_serdes_adv_local_get,
    phy_serdes_adv_remote_get,
    phy_serdes100fx_lb_set,
    phy_serdes100fx_lb_get,
    phy_serdes100fx_interface_set, 
    phy_serdes100fx_interface_get, 
    phy_serdes100fx_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    phy_serdes100fx_medium_config_set,
    phy_serdes_medium_config_get,
    phy_serdes_medium_status,
    NULL,                          /* phy_cable_diag  */
    NULL,                          /* phy_link_change */
    phy_serdes100fx_control_set,         /* phy_control_set */
    phy_serdes100fx_control_get,         /* phy_control_get */
    phy_serdes_reg_read,
    phy_serdes_reg_write,
    phy_serdes_reg_modify,
    phy_serdes100fx_notify
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _phy_serdes100fx_notify_duplex
 * Purpose:     
 *      Program duplex if (and only if) serdes100fx is an intermediate PHY.
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
_phy_serdes100fx_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;

    fiber = PHY_FIBER_MODE(unit, port);
    pc    = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes100fx_notify_duplex: "
                         "u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES100FX_MII_CTRLr(unit, pc, 
                                  FX_MII_CTRL_FD, FX_MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
        (_phy_serdes100fx_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ?FX_MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_MII_CTRLr(unit, pc, mii_ctrl, FX_MII_CTRL_FD));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_serdes100fx_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_serdes100fx_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_serdes100fx_notify_speed
 * Purpose:     
 *      Program duplex if (and only if) serdes100fx is an intermediate PHY.
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
_phy_serdes100fx_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int          fiber;
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    fiber = PHY_FIBER_MODE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_serdes100fx_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }
 
    SOC_IF_ERROR_RETURN
        (READ_SERDES100FX_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber && !(fiber_status & FX_1000X_STATUS1_SGMII_MODE)) {
        if ((speed != 0) && (speed != 100) && (speed != 1000)) {
            return SOC_E_CONFIG;
        }
    }

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_serdes100fx_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (_phy_serdes100fx_sgmii_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_serdes100fx_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && !fiber) {
        SOC_IF_ERROR_RETURN
            (phy_serdes100fx_an_set(unit, port, FALSE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_serdes100fx_stop
 * Purpose:
 *      Put serdes100fx SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_serdes100fx_stop(int unit, soc_port_t port)
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
                         "phy_serdes100fx_stop: u=%d p=%d copper=%d"
                         " stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    mii_ctrl = (stop) ?FX_MII_CTRL_PD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_MII_CTRLr(unit, pc, mii_ctrl, FX_MII_CTRL_PD));

    return SOC_E_NONE; 
}

/*
 * Function:
 *      _phy_serdes100fx_notify_stop
 * Purpose:     
 *      Add a reason to put serdes100fx PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_serdes100fx_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;

    return _phy_serdes100fx_stop(unit, port);
}

/*
 * Function:
 *      phy_serdes100fx_notify_resume
 * Purpose:     
 *      Remove a reason to put serdes100fx PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_serdes100fx_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;

    return _phy_serdes100fx_stop(unit, port);
}

/*
 * Function:
 *      phy_serdes100fx_media_setup
 * Purpose:     
 *      Configure 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      intf - Preferred interface (SGMII or 1000X) 
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
_phy_serdes100fx_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;

    pc = INT_PHY_SW_STATE(unit, port);
  
    data16 = 0;
    if (intf != SOC_PORT_IF_SGMII) {
        data16 = FX_1000X_FIBER_MODE;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES100FX_1000X_CTRL1r(unit, pc,
                                         data16, FX_1000X_FIBER_MODE));

    return SOC_E_NONE;
}

#else /* INCLUDE_SERDES_100FX */
typedef int _phy_serdes100fx_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_SERDES_100FX */

