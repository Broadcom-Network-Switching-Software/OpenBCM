/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *      phy56xxx_5601x.c
 * Purpose:
 *      Fiber driver for 56xxx_5601x using internal Serdes PHY with shadow
 *      registers.
 *
 * For BCM56504, the internal GMACC core of the GPORT should always be
 * configured to run in SGMII mode.  TBI mode is not supported between
 * the GMACC and the internal SERDES module.
 * 
 * When operating with an external PHY, this driver is not used.
 * However the speed/duplex of the internal PHY must be programmed to
 * match the MAC and external PHY settings so the data can pass through.
 * This file supplies some routines to allow mac.c to accomplish this
 * (think of the internal PHY as part of the MAC in this case):
 *
 *      phy_56xxx_5601x_notify_duplex
 *      phy_56xxx_5601x_notify_speed
 *      phy_56xxx_5601x_notify_stop
 *      phy_56xxx_5601x_notify_resume
 *
 * CMIC MIIM operations can be performed to the internal register set
 * using internal MDIO address (PORT_TO_PHY_ADDR_INT), and an external
 * PHY register set (such as BCM5424/34/64) can be programmed using the
 * external MDIO address (PORT_TO_PHY_ADDR).
 *
 * MDIO accesses to the internal PHY are not modeled on Quickturn.
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_56XXX)
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
#include "physr.h"
#include "phy56xxx5601x.h"

#ifdef INCLUDE_PHY_XGXS6
#include "xgxs6.h"
#endif /*INCLUDE_PHY_XGXS6 */

#define ADVERT_ALL_FIBER \
            (SOC_PM_PAUSE | SOC_PM_1000MB_FD)

/* Notify event forward declaration */
STATIC int _phy_56xxx_5601x_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int _phy_56xxx_5601x_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int _phy_56xxx_5601x_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_56xxx_5601x_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_56xxx_5601x_notify_interface(int unit, soc_port_t port, uint32 intf);


STATIC int
_phy_56xxx_5601x_sregs_init(phy_ctrl_t *pc)
{
    serdes_5601x_sregs_t *sr;
    if (pc->driver_data == NULL) {
        return SOC_E_INTERNAL;
    }
    sr = pc->driver_data;

    sr->mii_ctrl       = 0x1140;
    sr->mii_ana        = 0x01a0;
    sr->r1000x_ctrl1   = 0x0181;
    sr->r1000x_ctrl2   = 0x0000;

    return SOC_E_NONE;
}

STATIC int
phy_serdes_5601x_reg_read(int unit, phy_ctrl_t *pc,  uint16 reg_bank,
                         uint8 reg_addr, uint16 *phy_rd_data)
{
    serdes_5601x_sregs_t  *sr;
    uint16              data = 0;
    int                 rv = SOC_E_NONE;
    int                 shadow_read = 1;

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        if ((reg_bank != 0) || (reg_addr >= 0x0010)) {
            reg_bank = 0x300 + (reg_bank << 4);
        }
    }
#endif /* INCLUDE_PHY_XGXS6 */

    /* Read shadow registers if shadowed register */
    sr = pc->driver_data;
    if (sr == NULL) {
        return SOC_E_INTERNAL;
    }
    switch (reg_bank) {
    case 0:
        switch (reg_addr) {
        case 0:
            data = sr->mii_ctrl;
            break;
        case 4:
            data = sr->mii_ana;
            break;
        case 0x10:
            data = sr->r1000x_ctrl1;
            break;
        case 0x11:
            data = sr->r1000x_ctrl2;
            break;
        default:
            shadow_read = 0;
            break;
        }
        break;
    default:
        /* Invalid block */
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_serdes_5601x_reg_read: unexpected read from %x*%x: %04x\n"),
                  reg_bank, reg_addr, data));
        break;
    }

    if (! shadow_read) { 
        /* Read real registers */
        rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank); 
        if (SOC_SUCCESS(rv)) { 
            rv = READ_PHY_REG(unit, pc, reg_addr, &data);  
        }
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_serdes_5601x_reg_read: %x*%x: %04x\n"),
                  reg_bank, reg_addr, data));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_serdes_5601x_reg_read: shadow %x*%x: %04x\n"),
                  reg_bank, reg_addr, data));
    }  


    *phy_rd_data = data;

    return rv;
}

STATIC int
phy_serdes_5601x_reg_write(int unit, phy_ctrl_t *pc, uint16 reg_bank,
                          uint8 reg_addr, uint16 phy_wr_data)
{
    serdes_5601x_sregs_t  *sr;
    int                 rv = SOC_E_NONE;
    int                 shadow_write = 1;

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        if ((reg_bank != 0) || (reg_addr >= 0x0010)) {
            reg_bank = 0x300 + (reg_bank << 4);
        }
    }
#endif /* INCLUDE_PHY_XGXS6 */

    sr = pc->driver_data;
    if (sr == NULL) {
        return SOC_E_INTERNAL;
    }

    /* Write to shadow if shadowed register*/
    switch (reg_bank) {
    case 0:
        switch (reg_addr) {
        case 0:
            /*                                           
             * Do not write the reset bit and restart autoneg bits
             * to the shadow as these are self-cleared bits
             */
            sr->mii_ctrl = phy_wr_data & ~(MII_CTRL_RESET | MII_CTRL_RAN);
            break;
        case 4:
            sr->mii_ana = phy_wr_data;
            break;
        case 0x10:
            sr->r1000x_ctrl1 = phy_wr_data;
            break;
        case 0x11:
            sr->r1000x_ctrl2 = phy_wr_data;
            break;
        default:
            shadow_write = 0;
            break;
        }
        break;
    default:
        /* Invalid block */
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_serdes_5601x_reg_write: unexpected write to %x*%x: %04x\n"),
                  reg_bank, reg_addr, phy_wr_data));
        break;
    }

    if (shadow_write) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_serdes_5601x_reg_write: shadow %x*%x: %04x\n"),
                  reg_bank, reg_addr, phy_wr_data));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_serdes_5601x_reg_write: %x*%x: %04x\n"),
                  reg_bank, reg_addr, phy_wr_data));
    }

    /* Write through to real device */
    rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank);
    if (SOC_SUCCESS(rv)) {
        rv = WRITE_PHY_REG(unit, pc, reg_addr, phy_wr_data);
    }

    return rv;
}

STATIC int
phy_serdes_5601x_reg_modify(int unit, phy_ctrl_t *pc,
                           uint16 reg_bank, uint8 reg_addr,
                           uint16 phy_mo_data, uint16 phy_mo_mask)
{
    uint16  tmp, otmp;

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        if ((reg_bank != 0) || (reg_addr >= 0x0010)) {
            reg_bank = 0x300 + (reg_bank << 4);
        }
    }
#endif /* INCLUDE_PHY_XGXS6 */

    phy_mo_data &=  phy_mo_mask;
    SOC_IF_ERROR_RETURN
        (phy_serdes_5601x_reg_read(unit, pc, reg_bank, reg_addr, &tmp));
    otmp = tmp;
    tmp &= ~(phy_mo_mask);
    tmp |= phy_mo_data;

    if (tmp != otmp) {
        SOC_IF_ERROR_RETURN
            (phy_serdes_5601x_reg_write(unit, pc, reg_bank,
                                       reg_addr, tmp));
    }

    return SOC_E_NONE;
}


/***********************************************************************
 *
 * HELPER FUNCTIONS
 */

/*
 * Function:
 *      phy_56xxx_5601x_fiber_status_get
 * Purpose:
 *      Retrieves whether port is in fiber or SGMII
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - PHY state object.
 *      fiber_status - TRUE if fiber, else SGMII
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_56xxx_5601x_fiber_status_get(int unit, phy_ctrl_t *pc, uint16 *fiber_status)
{
    uint16 fiber;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_5601X_1000X_CTRL1r(unit, pc, &fiber));
    if (fiber & DDS_1000X_AUTO_DETECT) {
        /* Get status from 1000X_STAT1r */
        SOC_IF_ERROR_RETURN
            (READ_SERDES_5601X_1000X_STAT1r(unit, pc, &fiber));
        *fiber_status = !(fiber & DDS_1000X_STATUS1_SGMII_MODE);
    } else {
        /* Get from forced mode */
        *fiber_status = (fiber & DDS_1000X_FIBER_MODE);
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_56xxx_5601x_tx_control_set
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
_phy_56xxx_5601x_tx_control_set(int unit, phy_ctrl_t *pc,
                          soc_port_t port)
{
    uint32 preemph;
    uint32 idriver;
    uint32 pdriver;
    uint16 data16;
    int    fiber;

    fiber = PHY_FIBER_MODE(unit, port);

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
         /* Unicore */
         /* Preemphasis overrided in gxmac.c */
        return SOC_E_NONE;
    } else 
#endif /* INCLUDE_PHY_XGXS6 */
    {
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
        (MODIFY_SERDES_5601X_1000X_CTRL1r(unit, pc,
              DDS_1000X_TX_AMPLITUDE_OVRD, DDS_1000X_TX_AMPLITUDE_OVRD)); 

    /* Set ANALOG_TX with recommended amplitude */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_ANALOG_TXr(unit, pc, data16, 0xffc0));

    return SOC_E_NONE;
}

/***********************************************************************
 *
 * FIBER DRIVER ROUTINES
 */
STATIC int
phy_56xxx_5601x_init_no_reset(int unit, soc_port_t port)
{
    uint16             data16, mask16;
    soc_timeout_t      to;
    phy_ctrl_t        *pc;
    int                rv;

    pc                       = INT_PHY_SW_STATE(unit, port);
    pc->fiber.enable         = PHY_FIBER_MODE(unit, port);
    pc->fiber.preferred      = PHY_FIBER_MODE(unit, port);
    pc->fiber.autoneg_enable = TRUE;
    pc->fiber.autoneg_advert = ADVERT_ALL_FIBER;
    pc->fiber.force_speed    = 1000;
    pc->fiber.force_duplex   = TRUE;
    pc->fiber.master         = SOC_PORT_MS_NONE;
    pc->fiber.mdix           = SOC_PORT_MDIX_NORMAL;

   SOC_IF_ERROR_RETURN(_phy_56xxx_5601x_sregs_init(pc));

    /* Since phy56xxx_5601x driver reset function vector does not reset
     * the SerDes, reset the SerDes in initialization first. 
     * The internal SERDES PHY's reset bit is self-clearing.
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_MII_CTRLr(unit, pc,
                               MII_CTRL_RESET, MII_CTRL_RESET));

    soc_timeout_init(&to, 10000, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_SERDES_5601X_MII_CTRLr(unit, pc, &data16);
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

   SOC_IF_ERROR_RETURN(_phy_56xxx_5601x_sregs_init(pc));

    data16 = 0;
    mask16 = DDS_MII_CTRL_AN_ENABLE | DDS_MII_CTRL_AN_RESTART;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) || 
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        data16 = DDS_MII_CTRL_AN_ENABLE | DDS_MII_CTRL_AN_RESTART;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_MII_CTRLr(unit, pc, data16, mask16));

    /* Configure default preemphasis, predriver, idriver values */
    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_tx_control_set(unit, pc, port));

    data16 = DDS_1000X_FALSE_LNK_DIS | DDS_1000X_FLT_FORCE_EN;
    mask16 = DDS_1000X_FALSE_LNK_DIS | DDS_1000X_FLT_FORCE_EN;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_1000X_CTRL2r(unit, pc, data16, mask16));

    data16 = 0;
    mask16 = DDS_1000X_AUTO_DETECT | DDS_1000X_FIBER_MODE;
    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_AUTOMEDIUM, FALSE)) {
        data16 |= DDS_1000X_AUTO_DETECT;
    }
    /*
     * Put the Serdes in Fiber or SGMII mode
     */
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port)) {
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_FIBER_PREF, TRUE)) {
            data16 |= DDS_1000X_FIBER_MODE;
        }
    }
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_SGMII_MASTER, FALSE)) {
        data16 |= DDS_1000X_SGMII_MASTER;
        mask16 |= DDS_1000X_SGMII_MASTER;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_1000X_CTRL1r(unit, pc, data16, mask16));

    if (PHY_COPPER_MODE(unit, port)) {
        /* Do not wait 10ms for sync status before a valid link is 
         * established when auto-negotiation is disabled.
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES_5601X_1000X_CTRL2r(unit, pc, 0x0000, 0x0004));
    }
   
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_56xxx_5601x_init: u=%d p=%d %s\n"),
              unit, port, 
              PHY_FIBER_MODE(unit, port) ? "Fiber" : "Copper"));
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_56xxx_5601x_init
 * Purpose:     
 *      Initialize 56xxx_5601x internal PHY driver
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 * Notes:
 *      This function implements the bulk part of phy_56xxx_5601x_init and
 *      provides a method for initializing the SerDes when it is used
 *      as an intermediate PHY between the MAC and an external PHY. 
 *      In the latter case the PHY driver reset callback should be
 *      called only once, and this has already been done by the 
 *      external PHY driver init function.
 */

STATIC int
phy_56xxx_5601x_init(int unit, soc_port_t port)
{

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_56xxx_5601x_init: u=%d p=%d\n"),
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
        (phy_56xxx_5601x_init_no_reset(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_56xxx_5601x_enable_set
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
phy_56xxx_5601x_enable_set(int unit, soc_port_t port, int enable)
{
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_56xxx_5601x_enable_set: u=%d p=%d en=%d\n"),
              unit, port, enable));
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_56xxx_5601x_notify_resume(unit, port, PHY_STOP_PHY_DIS));
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_56xxx_5601x_notify_stop(unit, port, PHY_STOP_PHY_DIS));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_56xxx_5601x_sgmii_speed_set(int unit, soc_port_t port, int speed)
{
    uint16 mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    switch (speed) {
        case 10:
            mii_ctrl = DDS_MII_CTRL_SS_10;
            break;
        case 100:
            mii_ctrl = DDS_MII_CTRL_SS_100;
            break;
        case 0:
        case 1000:  
            mii_ctrl = DDS_MII_CTRL_SS_1000;
            break;
        default:
            return SOC_E_CONFIG;
    } 
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_MII_CTRLr(unit, pc, mii_ctrl,
                               (DDS_MII_CTRL_SS_LSB | DDS_MII_CTRL_SS_MSB)));
    return SOC_E_NONE;
}

STATIC int
_phy_56xxx_5601x_1000x_speed_set(int unit, soc_port_t port, int speed)
{
    if ((speed !=0) && (speed != 1000)) {
        return SOC_E_CONFIG;
    } 

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_56xxx_5601x_speed_set
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
phy_56xxx_5601x_speed_set(int unit, soc_port_t port, int speed)
{
    uint16       fiber_status;
    int          rv;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        SOC_IF_ERROR_RETURN
            (phy_xgxs6_hg.pd_speed_set(unit, port, speed));
        pc->fiber.force_speed = speed;
        return SOC_E_NONE;
    }
#endif /* INCLUDE_PHY_XGXS6 */

    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        rv = _phy_56xxx_5601x_sgmii_speed_set(unit, port, speed);
    } else {
        rv = _phy_56xxx_5601x_1000x_speed_set(unit, port, speed);
    }

    if (SOC_SUCCESS(rv)) {
        pc->fiber.force_speed = speed;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_56xxx_5601x_speed_set: u=%d p=%d speed=%d rv=%d\n"),
              unit, port, speed, rv));
    
    return rv;
}

STATIC int 
_phy_56xxx_5601x_sgmii_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16 mii_ctrl, mii_stat, mii_anp;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES_5601X_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_SERDES_5601X_MII_STATr(unit, pc, &mii_stat)); 
 
    if (mii_ctrl & DDS_MII_CTRL_AN_ENABLE) { /*Auto-negotiation enabled */  
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *speed = 0;
            return SOC_E_NONE;
        } else { /* Read Advertise link partner */
            SOC_IF_ERROR_RETURN
                (READ_SERDES_5601X_MII_ANPr(unit, pc, &mii_anp));
            if (mii_anp & DDS_MII_ANP_SGMII) {
                switch(mii_anp & DDS_MII_ANP_SGMII_SPEED) {
                case DDS_MII_ANP_SGMII_SPEED_1000:
                    *speed = 1000;
                    break;
                case DDS_MII_ANP_SGMII_SPEED_100:
                    *speed = 100;
                    break;
                case DDS_MII_ANP_SGMII_SPEED_10:
                    *speed = 10;
                    break;
                default:
                    return SOC_E_UNAVAIL;
                }
            }
        }
    } else {  /* Forced speed */
        switch(mii_ctrl & (DDS_MII_CTRL_SS_LSB | DDS_MII_CTRL_SS_MSB)) {
        case DDS_MII_CTRL_SS_10:
            *speed = 10; 
            break;
        case DDS_MII_CTRL_SS_100:
            *speed = 100;
            break;
        case DDS_MII_CTRL_SS_1000:
            *speed = 1000;
            break;
        default:
            return SOC_E_UNAVAIL;
        } 
    }
    return SOC_E_NONE;
}

STATIC int
_phy_56xxx_5601x_1000x_speed_get(int unit, soc_port_t port, int *speed)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *speed = 1000;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_56xxx_5601x_speed_get
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
phy_56xxx_5601x_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *speed = 1000;

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        return  phy_xgxs6_hg.pd_speed_get(unit, port, speed);
    }
#endif /* INCLUDE_PHY_XGXS6 */
 
    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        SOC_IF_ERROR_RETURN
            (_phy_56xxx_5601x_sgmii_speed_get(unit, port, speed));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_56xxx_5601x_1000x_speed_get(unit, port, speed));
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_56xxx_5601x_an_set
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
phy_56xxx_5601x_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16       par_det;
    uint16       an_enable;
    uint16       auto_det;

    pc = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_56xxx_5601x_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));
    /*
     * Special case for BCM5421S being used in SGMII copper mode with
     * BCM56xxx_5601x.  Always have autoneg on.
     */
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_5421S)) {
        an = 1;
    }

    par_det   = 0;
    an_enable = 0; 
    auto_det  = 0;
    if (an) {
        par_det = DDS_1000X_PAR_DET_EN;
        an_enable = DDS_MII_CTRL_AN_ENABLE | DDS_MII_CTRL_AN_RESTART;
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_AUTOMEDIUM, FALSE)) {
            auto_det = DDS_1000X_AUTO_DETECT; 
        }
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_1000X_CTRL2r(unit, pc,
                        par_det, DDS_1000X_PAR_DET_EN));

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_1000X_CTRL1r(unit, pc, 
                        auto_det, DDS_1000X_AUTO_DETECT));


    /* Enable/Disable and Restart autonegotiation (self-clearing bit) */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_MII_CTRLr(unit, pc, an_enable,
                        DDS_MII_CTRL_AN_ENABLE | DDS_MII_CTRL_AN_RESTART));

    pc->fiber.autoneg_enable = an;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_56xxx_5601x_adv_local_set
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
phy_56xxx_5601x_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        mode &= SOC_PM_2500MB_FD | SOC_PM_1000MB_FD |
                SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
        return (phy_xgxs6_hg.pd_adv_local_set(unit, port, mode));
    }
#endif

    return phy_serdes_adv_local_set(unit, port, mode);
}

/*
 * Function:    
 *      phy_56xxx_5601x_adv_local_get
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
phy_56xxx_5601x_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        return  phy_xgxs6_hg.pd_adv_local_get(unit, port, mode);
    }
#endif /* INCLUDE_PHY_XGXS6 */
 
    return phy_serdes_adv_local_get(unit, port, mode);
}

STATIC int
_phy_56xxx5601x_sgmii_adv_remote_get(int unit, soc_port_t port, 
                                soc_port_mode_t *mode)
{
    uint16              anlpa;
    soc_port_mode_t     duplex;
    phy_ctrl_t         *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_5601X_MII_ANPr(unit, pc, &anlpa));

    switch (anlpa & DDS_MII_ANP_SGMII_SPEED) {
    case DDS_MII_ANP_SGMII_SPEED_10:
        *mode = SOC_PM_10MB;
        break;
    case DDS_MII_ANP_SGMII_SPEED_100:
        *mode = SOC_PM_100MB;
        break;
    case DDS_MII_ANP_SGMII_SPEED_1000:
        *mode = SOC_PM_1000MB;
        break;
    } 

    duplex = (anlpa & DDS_MII_ANP_SGMII_DUPLEX) ? SOC_PM_FD : SOC_PM_HD;

    *mode = (*mode) & duplex; 

    return SOC_E_NONE;
}

STATIC int
_phy_56xxx5601x_1000x_adv_remote_get(int unit, soc_port_t port,
                                soc_port_mode_t *mode)
{
    uint16              anlpa;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_5601X_MII_ANPr(unit, pc, &anlpa));

    if (anlpa & DDS_MII_ANP_FIBER_FD) {
        *mode |= SOC_PM_1000MB_FD;
    }

    if (anlpa & DDS_MII_ANP_FIBER_HD) {
        *mode |= SOC_PM_1000MB_HD;
    }

    switch (anlpa &
            (DDS_MII_ANP_FIBER_PAUSE_SYM | DDS_MII_ANP_FIBER_PAUSE_ASYM)) {
    case DDS_MII_ANP_FIBER_PAUSE_SYM:
        *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
        break;
    case DDS_MII_ANP_FIBER_PAUSE_ASYM:
        *mode |= SOC_PM_PAUSE_TX;
        break;
    case DDS_MII_ANP_FIBER_PAUSE_SYM | DDS_MII_ANP_FIBER_PAUSE_ASYM:
        *mode |= SOC_PM_PAUSE_RX;
        break;
    }
    
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_56xxx_5601x_adv_remote_get
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
_phy_56xxx_5601x_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        SOC_IF_ERROR_RETURN
            (_phy_56xxx5601x_sgmii_adv_remote_get(unit, port, mode));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_56xxx5601x_1000x_adv_remote_get(unit, port, mode));
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
 *      phy_56xxx_5601x_adv_remote_get
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
phy_56xxx_5601x_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        return  phy_xgxs6_hg.pd_adv_remote_get(unit, port, mode);
    }
#endif /* INCLUDE_PHY_XGXS6 */
 
    return _phy_56xxx_5601x_adv_remote_get(unit, port, mode);
}

/*
 * Function:    
 *      phy_56xxx_5601x_lb_set
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
phy_56xxx_5601x_lb_set(int unit, soc_port_t port, int enable)
{
    uint16       ctrl;
    phy_ctrl_t  *pc;
    int          rv;

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        return phy_xgxs6_hg.pd_lb_set(unit, port, enable);
    }
#endif /* INCLUDE_PHY_XGXS6 */

    pc = INT_PHY_SW_STATE(unit, port);

    ctrl = (enable) ? DDS_MII_CTRL_LOOPBACK : 0;

    rv = MODIFY_SERDES_5601X_MII_CTRLr(unit, pc, ctrl, DDS_MII_CTRL_LOOPBACK);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_56xxx_5601x_lb_set: u=%d p=%d lb=%d rv=%d\n"),
              unit, port, enable, rv));
    return rv;
}

/*
 * Function:    
 *      phy_56xxx_5601x_ability_get
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
phy_56xxx_5601x_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      fiber_status;
    phy_ctrl_t *pc;
    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        *mode = (SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB_FD | 
                 SOC_PM_LB_PHY | SOC_PM_GMII);
    } else {
        *mode = (SOC_PM_1000MB_FD | SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM |
                 SOC_PM_AN | SOC_PM_LB_PHY | SOC_PM_GMII);
    }

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        *mode |= SOC_PM_2500MB_FD;
    }
#endif /* INCLUDE_PHY_XGXS6 */
 

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_56xxx_5601x_medium_config_update
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
_phy_56xxx_5601x_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_56xxx_5601x_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_serdes_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_56xxx_5601x_adv_local_set(unit, port, cfg->autoneg_advert));
    SOC_IF_ERROR_RETURN
        (phy_56xxx_5601x_an_set(unit, port, cfg->autoneg_enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_56xxx_5601x_medium_config_set
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
phy_56xxx_5601x_medium_config_set(int unit, soc_port_t port, 
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
                (_phy_56xxx_5601x_medium_config_update(unit, port, &pc->fiber));
        }
        return SOC_E_NONE;
    case SOC_PORT_MEDIUM_COPPER:
        return SOC_E_UNAVAIL;
    default:
        return SOC_E_PARAM;
    }
}

STATIC int
_phy_56xxx_5601x_control_tx_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    uint16              data;  /* Temporary holder of reg value to be written */
    uint16              mask;  /* Bit mask of reg value to be updated */
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
        (MODIFY_SERDES_5601X_ANALOG_TXr(unit, pc, data, mask));

    return SOC_E_NONE;
}

STATIC int
_phy_56xxx_5601x_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    uint16                data16;   /* Temporary holder of a reg value */
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data16 = 0;
    /* Dozen SerDes */
    SOC_IF_ERROR_RETURN
        (READ_SERDES_5601X_ANALOG_TXr(unit, pc, &data16));
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
 *      phy_phy56xxx_5601x_control_set
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
phy_56xxx_5601x_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        return (phy_xgxs6_hg.pd_control_set(unit, port, type, value));
    }
#endif

    PHY_CONTROL_TYPE_CHECK(type);
  
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_56xxx_5601x_control_tx_driver_set(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}
/*
 * Function:
 *      phy_56xxx_5601x_control_get
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
phy_56xxx_5601x_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;

    if (NULL == value) {
        return SOC_E_PARAM;
    }

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        return (phy_xgxs6_hg.pd_control_get(unit, port, type, value));
    } 
#endif /* INCLUDE_PHY_XGXS6 */

    PHY_CONTROL_TYPE_CHECK(type);
  
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_56xxx_5601x_control_tx_driver_get(unit, port, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      phy_56xxx_5601x_reg_read
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
phy_56xxx_5601x_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        return (phy_xgxs6_hg.pd_reg_read(unit, port, flags, 
                                         phy_reg_addr, phy_data));
    } 
#endif /* INCLUDE_PHY_XGXS6 */

    pc = INT_PHY_SW_STATE(unit, port);

    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    SOC_IF_ERROR_RETURN
        (phy_reg_serdes_read(unit, pc, reg_bank, reg_addr, &data));

   *phy_data = data;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_56xxx_5601x_reg_write
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
phy_56xxx_5601x_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        return (phy_xgxs6_hg.pd_reg_write(unit, port, flags, 
                                         phy_reg_addr, phy_data));
    } 
#endif /* INCLUDE_PHY_XGXS6 */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    SOC_IF_ERROR_RETURN
        (phy_reg_serdes_write(unit, pc, reg_bank, reg_addr, data));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_56xxx_5601x_reg_modify
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
phy_56xxx_5601x_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data, uint32 phy_data_mask)
{
    uint16         reg_bank;
    uint8          reg_addr;
    uint16         data;     /* Temporary holder for phy_data */
    uint16         mask;
    phy_ctrl_t    *pc;       /* PHY software state */

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        return (phy_xgxs6_hg.pd_reg_modify(unit, port, flags, 
                                       phy_reg_addr, phy_data, phy_data_mask));
    } 
#endif /* INCLUDE_PHY_XGXS6 */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    SOC_IF_ERROR_RETURN
        (phy_reg_serdes_modify(unit, pc, reg_bank, reg_addr, data, mask));

    return SOC_E_NONE;
}

STATIC int
phy_56xxx_5601x_notify(int unit, soc_port_t port, 
                 soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_56xxx_5601x_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_56xxx_5601x_notify_duplex(unit, port, value));
        break; 
    case phyEventSpeed:
        rv = (_phy_56xxx_5601x_notify_speed(unit, port, value));
        break; 
    case phyEventStop:
        rv = (_phy_56xxx_5601x_notify_stop(unit, port, value));
        break; 
    case phyEventResume:
        rv = (_phy_56xxx_5601x_notify_resume(unit, port, value));
        break; 
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}

STATIC int 
phy_56xxx_5601x_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *duplex = TRUE;

    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        /* retrieve the duplex setting in SGMII mode */

        SOC_IF_ERROR_RETURN
            (READ_SERDES_5601X_MII_CTRLr(unit, pc, &mii_ctrl));

        if (mii_ctrl & DDS_MII_CTRL_AN_ENABLE) {
            SOC_IF_ERROR_RETURN
                (READ_SERDES_5601X_MII_ANPr(unit,pc,&reg0_16));

            /* make sure link partner is also in SGMII mode
             * otherwise fall through to use the FD bit in MII_CTRL reg
             */
            if (reg0_16 & DDS_MII_ANP_SGMII) {

                /* test shows the duplex setting of external phy is 
                 * propagated to the AN link partner register 
                 */                
                if (reg0_16 & DDS_MII_ANP_SGMII_DUPLEX) {
                    *duplex = TRUE;
                    return SOC_E_NONE;
                } else {
                    *duplex = FALSE;
                    return SOC_E_NONE;
                } 
            }
        }

        *duplex = (mii_ctrl & DDS_MII_CTRL_FD) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

STATIC int
phy_56xxx_5601x_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      ctrl, stat;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES_5601X_MII_CTRLr(unit, pc, &ctrl));

    *an = (ctrl & MII_CTRL_AE) != 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_5601X_MII_STATr(unit, pc, &stat));

    *an_done = (stat & MII_STAT_AN_DONE) != 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_phy56xxx_5601x_an_get: u=%d p=%d an=%d an_done=%d\n"),
              unit, port, *an, *an_done));

    return SOC_E_NONE;
}

STATIC int
phy_56xxx_5601x_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_SERDES_5601X_MII_CTRLr(unit, pc, &ctrl));

    *enable = (ctrl & MII_CTRL_LE) != 0;

    return SOC_E_NONE;
}


/*
 * Variable:    phy_56xxx_5601xdrv_ge
 * Purpose:     PHY Driver for Dozen SerDes Serdes PHY internal to Draco, 
 */
phy_driver_t phy_56xxx_5601x_drv_ge = {
    "56XXX_5601X Internal SERDES PHY Driver",
    phy_56xxx_5601x_init,
    phy_null_reset,
    phy_serdes_link_get, 
    phy_56xxx_5601x_enable_set, 
    phy_null_enable_get, 
    phy_serdes_duplex_set, 
    phy_56xxx_5601x_duplex_get,          /* duplex_get */
    phy_56xxx_5601x_speed_set, 
    phy_56xxx_5601x_speed_get, 
    phy_serdes_master_set,          
    phy_serdes_master_get,          
    phy_56xxx_5601x_an_set,              /* local */
    phy_56xxx_5601x_an_get,              /* local */
    phy_56xxx_5601x_adv_local_set,
    phy_56xxx_5601x_adv_local_get,
    phy_56xxx_5601x_adv_remote_get,
    phy_56xxx_5601x_lb_set,
    phy_56xxx_5601x_lb_get,
    phy_serdes_interface_set, 
    phy_serdes_interface_get, 
    phy_56xxx_5601x_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    phy_56xxx_5601x_medium_config_set,
    phy_serdes_medium_config_get,
    phy_serdes_medium_status,
    NULL,                          /* phy_cable_diag  */
    NULL,                          /* phy_link_change */
    phy_56xxx_5601x_control_set,         /* phy_control_set */
    phy_56xxx_5601x_control_get,         /* phy_control_get */
    phy_56xxx_5601x_reg_read,
    phy_56xxx_5601x_reg_write,
    phy_56xxx_5601x_reg_modify,
    phy_56xxx_5601x_notify
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      phy_56xxx_5601x_notify_duplex
 * Purpose:     
 *      Program duplex if (and only if) 56xxx_5601x is an intermediate PHY.
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
 *      When used in SGMII mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */
STATIC int
_phy_56xxx_5601x_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;

    fiber = PHY_FIBER_MODE(unit, port);
    pc    = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_56xxx_5601x_notify_duplex: u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES_5601X_MII_CTRLr(unit, pc, 
                                   DDS_MII_CTRL_FD, DDS_MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ? DDS_MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_MII_CTRLr(unit, pc, mii_ctrl, DDS_MII_CTRL_FD)); 

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_56xxx_5601x_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_56xxx_5601x_notify_speed
 * Purpose:     
 *      Program duplex if (and only if) 56xxx_5601x is an intermediate PHY.
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
_phy_56xxx_5601x_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int          fiber;
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    fiber = PHY_FIBER_MODE(unit, port);

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber && fiber_status) {
        if ((speed != 0) && (speed != 1000)) {
            return SOC_E_CONFIG;
        }
    }

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_sgmii_speed_set(unit, port, (int)speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_56xxx_5601x_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && !fiber) {
        SOC_IF_ERROR_RETURN
            (phy_56xxx_5601x_an_set(unit, port, FALSE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_56xxx_5601x_stop
 * Purpose:
 *      Put 56xxx_5601x SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_56xxx_5601x_stop(int unit, soc_port_t port)
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
                         "phy_56xxx_5601x_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    mii_ctrl = (stop) ? DDS_MII_CTRL_PD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_MII_CTRLr(unit, pc, mii_ctrl, DDS_MII_CTRL_PD));

    return SOC_E_NONE; 
}

/*
 * Function:
 *      phy_56xxx_5601x_notify_stop
 * Purpose:
 *      Add a reason to put 56xxx_5601x PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_56xxx_5601x_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    pc->stop |= flags;

    return _phy_56xxx_5601x_stop(unit, port);
}

/*
 * Function:
 *      phy_56xxx_5601x_notify_resume
 * Purpose:
 *      Remove a reason to put 56xxx_5601x PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_56xxx_5601x_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    pc->stop &= ~flags;

    return _phy_56xxx_5601x_stop(unit, port);
}


STATIC int 
_phy_56xxx_5601x_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;

    pc = INT_PHY_SW_STATE(unit, port);

    data16 = (intf == SOC_PORT_IF_SGMII) ? 0 : DDS_1000X_FIBER_MODE; 
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_5601X_1000X_CTRL1r(unit, pc, data16, DDS_1000X_FIBER_MODE));

    return SOC_E_NONE;
}
#else /* INCLUDE_PHY_56XXX_5601X */
typedef int _phy_56xxx_5601x_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_56XXX_5601X */

