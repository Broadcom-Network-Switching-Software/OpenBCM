/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *      qsgmii65.c
 * Purpose:
 *      Fiber driver for serdes 65nm Low Power using internal Serdes PHY.
 *
 * When operating with an external PHY, this driver is not used.
 * However the speed/duplex of the internal PHY must be programmed to
 * match the MAC and external PHY settings so the data can pass through.
 * This file supplies some routines to allow mac.c to accomplish this
 * (think of the internal PHY as part of the MAC in this case):
 *
 *      phy_qsgmii65_notify_duplex
 *      phy_qsgmii65_notify_speed
 *      phy_qsgmii65_notify_stop
 *      phy_qsgmii65_notify_resume
 */
#include "phydefs.h"      /* Must include before other phy related includes */

#if defined(INCLUDE_XGXS_QSGMII65)
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


#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "serdes.h"
#include "qsgmii65.h"

#define WORKAROUND_FIX           1

#define ADVERT_ALL_FIBER \
            (SOC_PM_PAUSE | SOC_PM_1000MB_FD)

/* Notify event forward declaration */
STATIC int 
_phy_qsgmii65_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int 
_phy_qsgmii65_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int 
_phy_qsgmii65_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int 
_phy_qsgmii65_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int
_phy_qsgmii65_notify_interface(int unit, soc_port_t port, uint32 intf);

/***********************************************************************
 *
 * FIBER DRIVER ROUTINES
 */
STATIC int
_phy_qsgmii65_tx_control_set(int unit, phy_ctrl_t *pc,
                          soc_port_t port)
{
    uint32 preemph;
    uint32 idriver;
    uint32 pdriver;
    uint16 data;
    uint16 mask;

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_1000X_STAT1r(unit, pc, &data));

    if (data & LP_1000X_STATUS1_SGMII_MODE) {
        /* SGMII mode */
        idriver = 0x1;
        pdriver = 0x7;
    } else {
        /* 1000X mode */
        idriver = 0x1;
        pdriver = 0x7;
    }
    preemph = 0x0;

    /* Read config override */
    preemph = soc_property_port_get(unit, port,
                                    spn_SERDES_PREEMPHASIS, preemph);
    idriver = soc_property_port_get(unit, port,
                                    spn_SERDES_DRIVER_CURRENT, idriver);
    pdriver = soc_property_port_get(unit, port,
                                    spn_SERDES_PRE_DRIVER_CURRENT, pdriver);

    if (data & LP_1000X_STATUS1_SGMII_MODE) {
        /* SGMII mode */
        data = ((preemph & 0xf) << 12) | ((idriver & 0xf) << 8) | ((pdriver & 0xf) << 4);
        mask = 0xfff0;
        SOC_IF_ERROR_RETURN
            (MODIFY_QSGMII65_ANALOG_TX1r(unit, pc, data, mask));
    } else {
        /* 1000X mode */
        data = ((preemph & 0xf) << 12) | ((idriver & 0xf) << 8) | ((pdriver & 0xf) << 4);
        mask = 0xfff0;
        SOC_IF_ERROR_RETURN
            (MODIFY_QSGMII65_ANALOG_TX1r(unit, pc, data, mask));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_qsgmii65_init_no_reset(int unit, soc_port_t port)
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

    /* Since phyqsgmii65 driver reset function vector does not reset
     * the SerDes, reset the SerDes in initialization first. 
     * The internal SERDES PHY's reset bit is self-clearing.
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_QSGMII65_MII_CTRLr(unit, pc,
                               MII_CTRL_RESET, MII_CTRL_RESET));

    soc_timeout_init(&to, 10000, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_QSGMII65_MII_CTRLr(unit, pc, &data16);
        if (((data16 & MII_CTRL_RESET) == 0) || SOC_FAILURE(rv)) {
            break;
        }
    }
   if ((data16 & MII_CTRL_RESET) != 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Combo SerDes reset failed: u=%d p=%d\n"),
                  unit, port));
    }
   
    if (pc->lane_num == 0) {
        data16 = 0x0000;
        switch (soc_property_port_get(unit, port, 
                                  spn_SERDES_QSGMII_SGMII_OVERRIDE, 0)) {
            case 1:     /* QSGMII mode */
                data16 = 0x1100;
                /* Fall Thru */
            case 2:     /* SGMII mode */
                SOC_IF_ERROR_RETURN
                    (MODIFY_QSGMII65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x0000, 0x2000));

                mask16 = 0xff00;
                SOC_IF_ERROR_RETURN
                    (MODIFY_QSGMII65_LANE_CTRL0r(unit, pc, data16, mask16));

                SOC_IF_ERROR_RETURN
                    (MODIFY_QSGMII65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x2000, 0x2000));

                pc->lane_num = 4;

                SOC_IF_ERROR_RETURN
                    (MODIFY_QSGMII65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x0000, 0x2000));

                SOC_IF_ERROR_RETURN
                    (MODIFY_QSGMII65_LANE_CTRL0r(unit, pc, data16, mask16));

                SOC_IF_ERROR_RETURN
                    (MODIFY_QSGMII65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x2000, 0x2000));

                pc->lane_num = 0;
            default: /* H/W default */
                break;
        }
    }

    /* Set the default speed/duplex/autoneg */
    data16 = MII_CTRL_FD | MII_CTRL_SS_1000;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) ||
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        data16 |= MII_CTRL_AE | MII_CTRL_RAN;
    }

    /* If enabling autoneg, make sure that the default speed and duplex
     * are set correctly.
     */
    if (data16 & MII_CTRL_AE) {
        /* Advertise default speed and duplex */
        SOC_IF_ERROR_RETURN
            (WRITE_QSGMII65_MII_ANAr(unit, pc, MII_ANA_C37_FD | 
                                                 MII_ANA_C37_PAUSE |
                                                 MII_ANA_C37_ASYM_PAUSE));
    }
    SOC_IF_ERROR_RETURN
        (WRITE_QSGMII65_MII_CTRLr(unit, pc, data16));

    /* Configure auto-detect and fiber vs. sgmii mode */
    data16 = LP_1000X_PLL_PWRDWN;
    mask16 = LP_1000X_PLL_PWRDWN | 
             LP_1000X_AUTO_DETECT | LP_1000X_FIBER_MODE;
    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_AUTOMEDIUM, FALSE)) {
        data16 |= LP_1000X_AUTO_DETECT;
    }

    /*
     * Put the Serdes in Fiber or SGMII mode
     */
    mode_1000x = 0;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port)) {
        mode_1000x = LP_1000X_FIBER_MODE;
    }
    /* Allow property to override */
    if (soc_property_port_get(unit, port,
                              spn_SERDES_FIBER_PREF, mode_1000x)) {
        data16 |= LP_1000X_FIBER_MODE;
    } else {
        data16 &= ~LP_1000X_FIBER_MODE;
    }
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_SGMII_MASTER, FALSE)) {
        data16 |= LP_1000X_SGMII_MASTER;
        mask16 |= LP_1000X_SGMII_MASTER;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_QSGMII65_1000X_CTRL1r(unit, pc, data16, mask16));   

    data16 = LP_1000X_PAR_DET_EN; 
    mask16 = LP_1000X_FILTER_FORCE_EN | 
             LP_1000X_FORCE_XMIT_DATA_ON_TXSIDE |
             LP_1000X_PAR_DET_EN;
    if ((PHY_EXTERNAL_MODE(unit, port) && PHY_PASSTHRU_MODE(unit, port)) ||
        (!PHY_EXTERNAL_MODE(unit, port) && PHY_FIBER_MODE(unit, port))) {
        /* Wait 10ms for sync status before a valid link is 
         * established when auto-negotiation is disabled.
         */
        data16 |= LP_1000X_FILTER_FORCE_EN;
    } 
   
    SOC_IF_ERROR_RETURN
        (MODIFY_QSGMII65_1000X_CTRL2r(unit, pc, data16, mask16));

    if (soc_property_port_get(unit, port, "serdes_fifo_low_latency", 0)) {
        data16 = LP_1000X_FREQ_LOCK_ELASTICITY_RX |
                 LP_1000X_FREQ_LOCK_ELASTICITY_TX;
        mask16 = LP_1000X_FIFO_ELASTICITY_MASK |
                 LP_1000X_FREQ_LOCK_ELASTICITY_RX |
                 LP_1000X_FREQ_LOCK_ELASTICITY_TX;
        SOC_IF_ERROR_RETURN
            (MODIFY_QSGMII65_1000X_CTRL3r(unit, pc, data16, mask16));
    }

#if WORKAROUND_FIX
#ifdef BCM_HAWKEYE_SUPPORT  
    if (soc_feature(unit, soc_feature_hawkeye_a0_war)) {
        /* Workaroud :
         * change the TX driver register to value 170H 
         * for revision A0 of HAWKEYE and PHY 54684
         * (Addr : 0x8067)
         */
        if(pc->lane_num == 0){
            SOC_IF_ERROR_RETURN
                (_phy_qsgmii65_tx_control_set(unit, pc, port));
        }   

        /* Workaroud :
         * Make disparity error check configuration 
         */
        if((pc->lane_num == 0) || (pc->lane_num == 4)){
                SOC_IF_ERROR_RETURN   
                    (QSGMII65_REG_WRITE(unit, pc, 0x00, 0x000080ba, 0x00d0));   
                SOC_IF_ERROR_RETURN   
                    (QSGMII65_REG_WRITE(unit, pc, 0x00, 0x00008105, 0xf000));   
        }   
    }
#endif
#endif
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_qsgmii65_init: u=%d p=%d %s\n"),
              unit, port, 
              PHY_FIBER_MODE(unit, port)? "Fiber" : "Copper"));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_qsgmii65_init
 * Purpose:     
 *      Initialize qsgmii65 internal PHY driver
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 * Notes:
 *      This function implements the bulk part of phy_qsgmii65_init and
 *      provides a method for initializing the SerDes when it is used
 *      as an intermediate PHY between the MAC and an external PHY. 
 *      In the latter case the PHY driver reset callback should be
 *      called only once, and this has already been done by the 
 *      external PHY driver init function.
 */

STATIC int
phy_qsgmii65_init(int unit, soc_port_t port)
{

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_qsgmii65_init: u=%d p=%d\n"),
              unit, port));

    if (!PHY_EXTERNAL_MODE(unit, port)) {
         /* PHY reset callbacks */
        SOC_IF_ERROR_RETURN(soc_phy_reset(unit, port));

        /* No external PHY. SerDes is always in fiber mode. */
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_FIBER_PREF, 1)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        }

    }

    SOC_IF_ERROR_RETURN
        (_phy_qsgmii65_init_no_reset(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_qsgmii65_link_get
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
phy_qsgmii65_link_get(int unit, soc_port_t port, int *link)
{
    uint16       mii_stat;
    int          an_done;
    phy_ctrl_t  *pc;

    if (PHY_DISABLED_MODE(unit, port)) {
        *link = FALSE;
    } else {
        pc = INT_PHY_SW_STATE(unit, port);

        SOC_IF_ERROR_RETURN
            (READ_QSGMII65_MII_STATr(unit, pc, &mii_stat));

        *link = ((mii_stat & MII_STAT_LA) != 0);

        if (pc->fiber.autoneg_enable) {
            /* If autoneg is enabled, check for autoneg done. */
            an_done = ((mii_stat & MII_STAT_AN_DONE) != 0);

            *link = (*link) && (an_done);
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_qsgmii65_enable_set
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
phy_qsgmii65_enable_set(int unit, soc_port_t port, int enable)
{
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_qsgmii65_enable_set: u=%d p=%d en=%d\n"),
              unit, port, enable));
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_qsgmii65_notify_resume(unit, port, PHY_STOP_PHY_DIS));
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_qsgmii65_notify_stop(unit, port, PHY_STOP_PHY_DIS));
    }

    return SOC_E_NONE;
}


STATIC int
_phy_qsgmii65_sgmii_speed_set(int unit, soc_port_t port, int speed)
{
    uint16 mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

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
        default:
            return SOC_E_CONFIG;
    } 
    SOC_IF_ERROR_RETURN
        (MODIFY_QSGMII65_MII_CTRLr(unit, pc, mii_ctrl, MII_CTRL_SS_MASK));
    return SOC_E_NONE;
}

STATIC int
_phy_qsgmii65_1000x_speed_set(int unit, soc_port_t port, int speed)
{
    if (speed != 1000) { 
        return SOC_E_CONFIG;
    }
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_qsgmii65_speed_set
 * Purpose:     
 *      Set the current operating speed (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - Speed to set.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_qsgmii65_speed_set(int unit, soc_port_t port, int speed)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;
    int                 rv;

    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & LP_1000X_STATUS1_SGMII_MODE) {
        rv = _phy_qsgmii65_sgmii_speed_set(unit, port, speed);
    } else {
        rv = _phy_qsgmii65_1000x_speed_set(unit, port, speed);
    }

    if (SOC_SUCCESS(rv)) {
        pc->fiber.force_speed = speed;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_qsgmii65_speed_set: u=%d p=%d speed=%d rv=%d\n"),
              unit, port, speed, rv));
    
    return rv;
}

STATIC int 
_phy_qsgmii65_sgmii_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16 mii_ctrl, mii_stat, mii_anp;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_MII_STATr(unit, pc, &mii_stat));
 
    if (mii_ctrl & MII_CTRL_AE) { /*Auto-negotiation enabled */  
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *speed = 0;
            return SOC_E_NONE;
        } else { /* Read Advertise link partner */
            SOC_IF_ERROR_RETURN
                (READ_QSGMII65_MII_ANPr(unit, pc, &mii_anp));
            if (mii_anp & MII_ANP_SGMII_MODE) {
                switch(mii_anp & MII_ANP_SGMII_SS_MASK) {
                case MII_ANP_SGMII_SS_1000:
                    *speed = 1000;
                    break;
                case MII_ANP_SGMII_SS_100:
                    *speed = 100;
                    break;
                case MII_ANP_SGMII_SS_10:
                    *speed = 10;
                    break;
                default:
                    return SOC_E_INTERNAL;
                }
            }
        }
    } else {  /* Forced speed */
        switch(mii_ctrl & MII_CTRL_SS_MASK) {
        case MII_CTRL_SS_10:
            *speed = 10; 
            break;
        case MII_CTRL_SS_100:
            *speed = 100;
            break;
        case MII_CTRL_SS_1000:
            *speed = 1000;
            break;
        default:
            return SOC_E_INTERNAL;
        } 
    }
    return SOC_E_NONE;
}

STATIC int
_phy_qsgmii65_1000x_speed_get(int unit, soc_port_t port, int *speed)
{
    *speed = 1000;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_qsgmii65_speed_get
 * Purpose:     
 *      Get the current operating speed. If autoneg is enabled, 
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - Current port speed. 
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_qsgmii65_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *speed = 1000;

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & LP_1000X_STATUS1_SGMII_MODE) {
        SOC_IF_ERROR_RETURN
            (_phy_qsgmii65_sgmii_speed_get(unit, port, speed));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_qsgmii65_1000x_speed_get(unit, port, speed));
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_qsgmii65_an_set
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
phy_qsgmii65_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16             an_enable;
    uint16             auto_det;

    pc = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_qsgmii65_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));

    an_enable = 0; 
    auto_det  = 0;
    if (an) {
        an_enable = MII_CTRL_AE | MII_CTRL_RAN;
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_AUTOMEDIUM, FALSE)) {
            auto_det = LP_1000X_AUTO_DETECT; 
        }
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_QSGMII65_1000X_CTRL1r(unit, pc, 
                       auto_det, LP_1000X_AUTO_DETECT));

    /* Enable/Disable and Restart autonegotiation (self-clearing bit) */
    SOC_IF_ERROR_RETURN
        (MODIFY_QSGMII65_MII_CTRLr(unit, pc, an_enable,
                       MII_CTRL_AE | MII_CTRL_RAN));

    pc->fiber.autoneg_enable = an;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_qsgmii65_an_get
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
 */

STATIC int
phy_qsgmii65_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      ctrl, stat;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_MII_CTRLr(unit, pc, &ctrl));

    *an = (ctrl & MII_CTRL_AE) != 0;

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_MII_STATr(unit, pc, &stat));

    *an_done = (stat & MII_STAT_AN_DONE) != 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes_an_get: u=%d p=%d an=%d an_done=%d\n"),
              unit, port, *an, *an_done));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qsgmii65_adv_local_set
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
phy_qsgmii65_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    uint16              an_adv;
    phy_ctrl_t  *pc;

    /* Note:
     * We assume that switch SerDes is always used as slave when
     * the port is in SGMII mode. Therefore, the adv_local_set is applicable
     * only for 1000BASE-X mode and supported only clause 37 autoneg.
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
        (MODIFY_QSGMII65_MII_ANAr(unit, pc, an_adv,
                                MII_ANA_C37_ASYM_PAUSE | MII_ANA_C37_PAUSE |
                                MII_ANA_C37_FD | MII_ANA_C37_HD));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes_adv_local_set: u=%d p=%d adv=%s%s%s\n"),
              unit, port,
              (mode & SOC_PM_1000MB_FD) ? "1000MB " : "",
              (mode & SOC_PM_PAUSE_TX)  ? "PAUSE_TX " : "",
              (mode & SOC_PM_PAUSE_RX)  ? "PAUSE_TX " : ""));
    return SOC_E_NONE;
}

/*
 * Function:   
 *      phy_qsgmii65_adv_local_get
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
phy_qsgmii65_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      an_adv;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_MII_ANAr(unit, pc, &an_adv));

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
    }

    return SOC_E_NONE;
}

STATIC int
_phy_qsgmii65_sgmii_adv_remote_get(int unit, soc_port_t port,
                                soc_port_mode_t *mode)
{
    uint16              anlpa;
    soc_port_mode_t     duplex;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_MII_ANPr(unit, pc, &anlpa));

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

    return SOC_E_NONE;
}

STATIC int
_phy_qsgmii65_1000x_adv_remote_get(int unit, soc_port_t port,
                                soc_port_mode_t *mode)
{
    uint16              anlpa;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_MII_ANPr(unit, pc, &anlpa));

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

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qsgmii65_adv_remote_get
 * Purpose:
 *      Get partner's current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 */

int
phy_qsgmii65_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & SERDES_1000X_STAT1_SGMII_MODE) {
        SOC_IF_ERROR_RETURN
            (_phy_qsgmii65_sgmii_adv_remote_get(unit, port, mode));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_qsgmii65_1000x_adv_remote_get(unit, port, mode));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes_adv_remote_get: u=%d p=%d adv=%s%s%s\n"),
              unit, port,
              (*mode & SOC_PM_1000MB_FD) ? "1000MB " : "",
              (*mode & SOC_PM_PAUSE_TX)  ? "PAUSE_TX " : "",
              (*mode & SOC_PM_PAUSE_RX)  ? "PAUSE_TX " : ""));

    return SOC_E_NONE;
}
    
/*  
 * Function:    
 *      phy_qsgmii65_lb_set
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
phy_qsgmii65_lb_set(int unit, soc_port_t port, int enable)
{   
    uint16              ctrl;
    phy_ctrl_t  *pc;
    int                 rv;
    
    pc = INT_PHY_SW_STATE(unit, port);
    
    ctrl = (enable) ?MII_CTRL_LE : 0;

    rv = MODIFY_QSGMII65_MII_CTRLr(unit, pc,
                               ctrl, MII_CTRL_LE);
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes_lb_set: u=%d p=%d lb=%d rv=%d\n"),
              unit, port, enable, rv));

    return rv;
}

/*
 * Function:   
 *      phy_serdes_lb_get
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
phy_qsgmii65_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_MII_CTRLr(unit, pc, &ctrl));

    *enable = (ctrl & MII_CTRL_LE) != 0;

    return SOC_E_NONE;
}


/*
 * Function:    
 *      phy_qsgmii65_ability_get
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
phy_qsgmii65_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      fiber_status;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & LP_1000X_STATUS1_SGMII_MODE) {
        *mode = (SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB_FD |
                 SOC_PM_LB_PHY | SOC_PM_GMII);
    } else {
        *mode = (SOC_PM_1000MB_FD | SOC_PM_100MB_FD |
                 SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM | SOC_PM_AN | 
                 SOC_PM_LB_PHY | SOC_PM_GMII);
    }


    
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_qsgmii65_medium_config_update
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
_phy_qsgmii65_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_qsgmii65_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_serdes_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_serdes_adv_local_set(unit, port, cfg->autoneg_advert));
    SOC_IF_ERROR_RETURN
        (phy_qsgmii65_an_set(unit, port, cfg->autoneg_enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qsgmii65_medium_config_set
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
phy_qsgmii65_medium_config_set(int unit, soc_port_t port, 
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
                (_phy_qsgmii65_medium_config_update(unit, port, &pc->fiber));
        }
        return SOC_E_NONE;
    case SOC_PORT_MEDIUM_COPPER:
        return SOC_E_UNAVAIL;
    default:
        return SOC_E_PARAM;
    }
}

STATIC int
_phy_qsgmii65_control_tx_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    uint16       data;  /* Temporary holder of reg value to be written */
    uint16       mask;  /* Bit mask of reg value to be updated */
    phy_ctrl_t  *pc;    /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_1000X_STAT1r(unit, pc, &data));

    if (data & LP_1000X_STATUS1_SGMII_MODE) {
        /* SGMII mode */
        switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
             data = (uint16)(value & 0xF);
             data = data << 12;
             mask = 0xf000;
             SOC_IF_ERROR_RETURN
                 (MODIFY_QSGMII65_ANALOG_TX1r(unit, pc, data, mask));
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
             data = (uint16)(value & 0xF);
             data = data << 8;
             mask = 0x0f00;
             SOC_IF_ERROR_RETURN
                 (MODIFY_QSGMII65_ANALOG_TX1r(unit, pc, data, mask));
             break;
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
             data = (uint16)(value & 0xF);
             data = data << 4;
             mask = 0x00f0;
             SOC_IF_ERROR_RETURN
                 (MODIFY_QSGMII65_ANALOG_TX1r(unit, pc, data, mask));
             break;
        default:
             /* should never get here */
             return SOC_E_PARAM;
        }
    } else {
        /* 1000X mode */
        switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
             data = (uint16)(value & 0xF);
             data = data << 12;
             mask = 0xf000;
             SOC_IF_ERROR_RETURN
                 (MODIFY_QSGMII65_ANALOG_TX1r(unit, pc, data, mask));
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
             data = (uint16)(value & 0xF);
             data = data << 8;
             mask = 0x0f00;
             SOC_IF_ERROR_RETURN
                 (MODIFY_QSGMII65_ANALOG_TX1r(unit, pc, data, mask));
             break;
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
             data = (uint16)(value & 0xF);
             data = data << 4;
             mask = 0x00f0;
             SOC_IF_ERROR_RETURN
                 (MODIFY_QSGMII65_ANALOG_TX1r(unit, pc, data, mask));
             break;
        default:
             /* should never get here */
             return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_qsgmii65_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    uint16         data;   /* Temporary holder of a reg value */
    phy_ctrl_t    *pc;     /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_1000X_STAT1r(unit, pc, &data));

    if (data & LP_1000X_STATUS1_SGMII_MODE) {
        /* SGMII mode */
        switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
             SOC_IF_ERROR_RETURN
                 (READ_QSGMII65_ANALOG_TX1r(unit, pc, &data));
             *value = (data & 0xf000) >> 12;
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
             SOC_IF_ERROR_RETURN
                 (READ_QSGMII65_ANALOG_TX1r(unit, pc, &data));
             *value = (data & 0x0f00) >> 8;
             break;
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
             SOC_IF_ERROR_RETURN
                 (READ_QSGMII65_ANALOG_TX1r(unit, pc, &data));
             *value = (data & 0x00f0) >> 4;
             break;
        default:
             /* should never get here */
             return SOC_E_PARAM;
        }
    } else {
        /* 1000X mode */
        switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
             SOC_IF_ERROR_RETURN
                 (READ_QSGMII65_ANALOG_TX1r(unit, pc, &data));
             *value = (data & 0xf000) >> 12;
             break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
             SOC_IF_ERROR_RETURN
                 (READ_QSGMII65_ANALOG_TX1r(unit, pc, &data));
             *value = (data & 0x0f00) >> 8;
             break;
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
             SOC_IF_ERROR_RETURN
                 (READ_QSGMII65_ANALOG_TX1r(unit, pc, &data));
             *value = (data & 0x00f0) >> 4;
             break;
        default:
             /* should never get here */
             return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_phyqsgmii65_control_set
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
phy_qsgmii65_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
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
        rv = _phy_qsgmii65_control_tx_driver_set(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}
/*
 * Function:
 *      phy_qsgmii65_control_get
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
phy_qsgmii65_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;

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
        rv = _phy_qsgmii65_control_tx_driver_get(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_qsgmii65_reg_read
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
phy_qsgmii65_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_reg_qsgmii_aer_read(unit, pc, phy_reg_addr, &data));

   *phy_data = data;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_qsgmii65_reg_write
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
phy_qsgmii65_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_reg_qsgmii_aer_write(unit, pc, phy_reg_addr, data));

    return SOC_E_NONE;
}  

/*
 * Function:
 *      phy_qsgmii65_reg_modify
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
phy_qsgmii65_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_reg_qsgmii_aer_modify(unit, pc, phy_reg_addr, data, mask));

    return SOC_E_NONE;
}

STATIC int
phy_qsgmii65_notify(int unit, soc_port_t port,
                 soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_qsgmii65_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_qsgmii65_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_qsgmii65_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_qsgmii65_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_qsgmii65_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        rv = (phy_qsgmii65_an_set(unit, port, value));
        break;
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}

STATIC int
phy_qsgmii65_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    *duplex = TRUE;

    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_1000X_STAT1r(unit, pc, &reg0_16));

    if (reg0_16 & LP_1000X_STATUS1_SGMII_MODE) {
    /* retrieve the duplex setting in SGMII mode */

        SOC_IF_ERROR_RETURN
            (READ_QSGMII65_MII_CTRLr(unit, pc, &mii_ctrl));

        if (mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_QSGMII65_MII_ANPr(unit,pc,&reg0_16));

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
 * Variable:    phy_qsgmii65drv_ge
 * Purpose:     PHY Driver for Dozen SerDes Serdes PHY internal to Draco, 
 */
phy_driver_t phy_qsgmii65_ge = {
    "Internal QSGMII 65nm SERDES PHY Driver",
    phy_qsgmii65_init,
    phy_null_reset,
    phy_qsgmii65_link_get, 
    phy_qsgmii65_enable_set, 
    phy_null_enable_get, 
    phy_serdes_duplex_set, 
    phy_qsgmii65_duplex_get,          /* duplex_get */
    phy_qsgmii65_speed_set, 
    phy_qsgmii65_speed_get, 
    phy_serdes_master_set,              /* master_set */
    phy_serdes_master_get,              /* master_get */
    phy_qsgmii65_an_set,
    phy_qsgmii65_an_get,
    phy_qsgmii65_adv_local_set,
    phy_qsgmii65_adv_local_get,
    phy_qsgmii65_adv_remote_get,
    phy_qsgmii65_lb_set,
    phy_qsgmii65_lb_get,
    phy_serdes_interface_set, 
    phy_serdes_interface_get, 
    phy_qsgmii65_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    phy_qsgmii65_medium_config_set,
    phy_serdes_medium_config_get,
    phy_serdes_medium_status,
    NULL,                               /* phy_cable_diag  */
    NULL,                               /* phy_link_change */
    phy_qsgmii65_control_set,         /* phy_control_set */
    phy_qsgmii65_control_get,         /* phy_control_get */
    phy_qsgmii65_reg_read,
    phy_qsgmii65_reg_write,
    phy_qsgmii65_reg_modify,
    phy_qsgmii65_notify
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _phy_qsgmii65_notify_duplex
 * Purpose:     
 *      Program duplex if (and only if) qsgmii65 is an intermediate PHY.
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
_phy_qsgmii65_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;

    fiber = PHY_FIBER_MODE(unit, port);
    pc    = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_qsgmii65_notify_duplex: "
                         "u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_QSGMII65_MII_CTRLr(unit, pc, 
                                  MII_CTRL_FD, MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
        (_phy_qsgmii65_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ?MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_QSGMII65_MII_CTRLr(unit, pc, mii_ctrl, MII_CTRL_FD));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_qsgmii65_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_qsgmii65_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_qsgmii65_notify_speed
 * Purpose:     
 *      Program duplex if (and only if) qsgmii65 is an intermediate PHY.
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
_phy_qsgmii65_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int          fiber;
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    fiber = PHY_FIBER_MODE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_qsgmii65_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }
 
    SOC_IF_ERROR_RETURN
        (READ_QSGMII65_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber && !(fiber_status & LP_1000X_STATUS1_SGMII_MODE)) {
        if ((speed != 0) && (speed != 100) && (speed != 1000)) {
            return SOC_E_CONFIG;
        }
    }

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_qsgmii65_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (_phy_qsgmii65_sgmii_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_qsgmii65_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && !fiber) {
        SOC_IF_ERROR_RETURN
            (phy_qsgmii65_an_set(unit, port, FALSE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_qsgmii65_stop
 * Purpose:
 *      Put qsgmii65 SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_qsgmii65_stop(int unit, soc_port_t port)
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
                         "phy_qsgmii65_stop: u=%d p=%d copper=%d"
                         " stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    mii_ctrl = (stop) ?MII_CTRL_PD : 0;
#ifdef BCM_HAWKEYE_SUPPORT
    if (SOC_IS_HAWKEYE(unit)) {
        uint16 dev_id = 0;   
        uint8 rev_id = 0;   
    
        soc_cm_get_id(unit, &dev_id, &rev_id);
        if ((pc->lane_num == 0) && 
           ((dev_id == BCM53313_DEVICE_ID) || (dev_id == BCM53312_DEVICE_ID))) { 
             /* Issue : BCM53312 have four SGMII ports (GE8,9,10,11).
             *      If power down GE8 or GE10, 
             *      the GE9 or GE11 is powered down also.
             *
             * Analysis : When set the QSGMII in sgmii dual mode,
             *      if you power down lane 4, 
             *      only lane4 is powered down.
             *      But if you power down lane0, not only lane0
             *      is powered down but also pll get powered down. 
             *
             * Solution : We can set laneCtrl3 to 0xAFF for AER 0, not AER 4.
             *      The laneCtrl3 (addr:0X8018) is the reg in 
             *      XGXS Block1(default is 0x200). 
             *          pwrdwn_rx = laneCtrl3[3:0];
             *          pwrdwn_tx = laneCtrl3[7:4];
             *          pwrdwn_pll = laneCtrl3[8];
             *          pwrdwn10g_pll_dis = laneCtrl3[9];
             *          lock_ref_en = laneCtrl3[10];
             *          pwrdwn_force = laneCtrl3[11];
             */
            SOC_IF_ERROR_RETURN
                (WRITE_QSGMII65_LANE_CTRL3r(unit, pc, (stop ? 0xAFF : 0x200)));
        } else {
            if (soc_feature (unit, soc_feature_eee)) {
                /* If link on port1 (second port) of a QSGMII octet is  
                 *        down, Tx/Rx LPI_DURATION counters of all ports 
                 *        in that octet stop running. 
                 * So NOT to set the power-down bit for second port
                 *       of QSGMII serdes on HKEEE.
                 *       And MAC.SW_RESET in HKEEE essentially kill the traffic
                 *       in both Tx and Rx direction.
                 */
                if(pc->lane_num == 1) {
                    /* Do nothing */
                } else {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_QSGMII65_MII_CTRLr
                                (unit, pc, mii_ctrl, MII_CTRL_PD));
                }
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_QSGMII65_MII_CTRLr(unit, pc, mii_ctrl, MII_CTRL_PD));
            }
        }        
    } else
#endif /* BCM_HAWKEYE_SUPPORT */
#ifdef BCM_HURRICANE1_SUPPORT
        if (SOC_IS_HURRICANE(unit)) {
            uint16 lctrl;
            SOC_IF_ERROR_RETURN
                (READ_QSGMII65_LANE_CTRL0r(unit, pc,&lctrl));
            if ((pc->lane_num == 0) && ((lctrl & 0xff00)  == 0)) {
                /* SGMII Mode */
                SOC_IF_ERROR_RETURN
                    (WRITE_QSGMII65_LANE_CTRL3r(unit, pc, (stop ? 0xAFF : 0x200)));
            } else {
                if (soc_feature (unit, soc_feature_eee)) {
                    if(pc->lane_num == 1) {
                        /* Do nothing */
                    } else {
                        SOC_IF_ERROR_RETURN
                            (MODIFY_QSGMII65_MII_CTRLr
                                    (unit, pc, mii_ctrl, MII_CTRL_PD));
                    }
                } else {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_QSGMII65_MII_CTRLr(unit, pc, mii_ctrl, MII_CTRL_PD));
                }
            }        
        } else
#endif /* BCM_HURRICANE_SUPPORT */
    {
        SOC_IF_ERROR_RETURN
            (MODIFY_QSGMII65_MII_CTRLr(unit, pc, mii_ctrl, MII_CTRL_PD));
    }

    return SOC_E_NONE; 
}

/*
 * Function:
 *      _phy_qsgmii65_notify_stop
 * Purpose:     
 *      Add a reason to put qsgmii65 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_qsgmii65_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;

    return _phy_qsgmii65_stop(unit, port);
}

/*
 * Function:
 *      phy_qsgmii65_notify_resume
 * Purpose:     
 *      Remove a reason to put qsgmii65 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_qsgmii65_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;

    return _phy_qsgmii65_stop(unit, port);
}

/*
 * Function:
 *      phy_qsgmii65_media_setup
 * Purpose:     
 *      Configure 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      PHY_FIBER_MODE - Configure for fiber mode
 *      fiber_pref - Fiber preferrred (if fiber mode)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
_phy_qsgmii65_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;

    pc = INT_PHY_SW_STATE(unit, port);
  
    data16 = 0;
    if (intf != SOC_PORT_IF_SGMII) {
        data16 = LP_1000X_FIBER_MODE;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_QSGMII65_1000X_CTRL1r(unit, pc,
                                         data16, LP_1000X_FIBER_MODE));

    return SOC_E_NONE;
}
#else /* INCLUDE_XGXS_QSGMII65 */
typedef int _phy_qsgmii65_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_XGXS_QSGMII65 */
