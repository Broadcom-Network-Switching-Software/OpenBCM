 /*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *      physerdescombo_5601x.c
 * Purpose:
 *      Fiber driver for serdescombo_5601x using internal Serdes PHY with
 *      shadow registers.
 *
 * For BCM56218, the internal GMACC core of the GPORT should always be
 * configured to run in SGMII mode.  TBI mode is not supported between
 * the GMACC and the internal SERDES module.
 * 
 * When operating with an external PHY, this driver is not used.
 * However the speed/duplex of the internal PHY must be programmed to
 * match the MAC and external PHY settings so the data can pass through.
 * This file supplies some routines to allow mac.c to accomplish this
 * (think of the internal PHY as part of the MAC in this case):
 *
 *      _phy_serdescombo_5601x_notify_duplex
 *      _phy_serdescombo_5601x_notify_speed
 *      _phy_serdescombo_5601x_notify_stop
 *      _phy_serdescombo_5601x_notify_resume
 *
 * CMIC MIIM operations can be performed to the internal register set
 * using internal MDIO address (PORT_TO_PHY_ADDR_INT), and an external
 * PHY register set (such as BCM5424/34/64) can be programmed using the
 * external MDIO address (PORT_TO_PHY_ADDR).
 *
 * MDIO accesses to the internal PHY are not modeled on Quickturn.
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_SERDES_COMBO)
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
#include "serdescombo5601x.h"


#define ADVERT_ALL_FIBER \
            (SOC_PM_PAUSE | SOC_PM_1000MB_FD | SOC_PM_2500MB_FD)

/* Notify event forward declaration */
STATIC int
_phy_serdescombo_5601x_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int
_phy_serdescombo_5601x_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int
_phy_serdescombo_5601x_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int
_phy_serdescombo_5601x_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int
_phy_serdescombo_5601x_notify_interface(int unit, soc_port_t port, uint32 intf);
STATIC int 
phy_serdescombo_5601x_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode);



STATIC int
_phy_serdescombo_5601x_sregs_init(phy_ctrl_t *pc)
{
    serdescombo_5601x_sregs_t *sr;
    if (pc->driver_data == NULL) {
        return SOC_E_INTERNAL;
    }
    sr = pc->driver_data;

    sr->mii_ctrl       = 0x1140;
    sr->mii_ana        = 0x01a0;
    sr->r1000x_ctrl1   = 0x0181;
    sr->r1000x_ctrl2   = 0x0000;
    sr->analog_tx      = 0x0001;
    sr->digi3_ctrl     = 0x0000;
    sr->misc_misc2     = 0x0003;
    sr->misc_tx_actrl3 = 0x0e90;

    return SOC_E_NONE;
}

STATIC int 
phy_serdescombo_5601x_reg_read(int unit, phy_ctrl_t *pc,  uint16 reg_bank, 
                    uint8 reg_addr, uint16 *phy_rd_data) 
{ 
    serdescombo_5601x_sregs_t *sr;
    uint16 data = 0;
    int    rv = SOC_E_NONE;
    int    shadow_read = 1;

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

    /* Read shadow registers if shadow register */
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
    case 2:
        switch (reg_addr) {
        case 0x1b:
            data = sr->analog_tx;
            break;
        case 0x10:
            data = sr->digi3_ctrl;
            break;
        default:
            shadow_read = 0;
            break;
        }
        break;
    case 5:
        switch (reg_addr) {
        case 0x1e:
            data = sr->misc_misc2;
            break;
        case 0x17:
            data = sr->misc_tx_actrl3;
            break;
        default:
            shadow_read = 0;
            break;
        }
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
                             "phy_serdescombo_5601x_reg_read: %x*%x: %04x\n"),
                  reg_bank, reg_addr, data));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_serdescombo_5601x_reg_read: shadow %x*%x: %04x\n"),
                  reg_bank, reg_addr, data));
    } 

    *phy_rd_data = data;
 
    return rv; 
}


STATIC int
phy_serdescombo_5601x_reg_write(int unit, phy_ctrl_t *pc, uint16 reg_bank, 
                     uint8 reg_addr, uint16 phy_wr_data)
{
    serdescombo_5601x_sregs_t *sr;
    int rv     = SOC_E_NONE;
    int shadow_write = 1;

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

    /* Write to shadow if shadowed register */
    switch (reg_bank) {
    case 0:
        switch (reg_addr) {
        case 0:
           /*                                                     
             * Do not write the reset bit and restart autoneg bits 
             * to the shadow as these are self-cleared bits            
             */
            sr->mii_ctrl = phy_wr_data & 
                           ~(COMBO_MII_CTRL_RESET | COMBO_MII_CTRL_AN_RESTART);
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
        }
        break;
    case 2:
        switch (reg_addr) {
        case 0x1b:
            sr->analog_tx = phy_wr_data;
            break;
        case 0x10:
            sr->digi3_ctrl = phy_wr_data;
            break;
        default:
            shadow_write = 0;
        }
        break;
    case 5:
        switch (reg_addr) {
        case 0x1e:
            sr->misc_misc2 = phy_wr_data;
            break;
        case 0x17:
            sr->misc_tx_actrl3 = phy_wr_data;
            break;
        default:
            shadow_write = 0;
        }
        break;
    }

    if (shadow_write) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_serdescombo_5601x_reg_write: shadow %x*%x: %04x\n"),
                  reg_bank, reg_addr, phy_wr_data));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_serdescombo_5601x_reg_write: %x*%x: %04x\n"),
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
phy_serdescombo_5601x_reg_modify(int unit, phy_ctrl_t *pc,
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
        (phy_serdescombo_5601x_reg_read(unit, pc, reg_bank, reg_addr, &tmp));
    otmp = tmp;
    tmp &= ~(phy_mo_mask);
    tmp |= phy_mo_data;

    if (tmp != otmp) {
        SOC_IF_ERROR_RETURN
            (phy_serdescombo_5601x_reg_write(unit, pc, reg_bank, 
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
 *      phy_serdescombo_5601x_fiber_status_get
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
_phy_serdescombo_5601x_fiber_status_get(int unit, phy_ctrl_t *pc, 
                                        uint16 *fiber_status)
{
    uint16 fiber;

    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_1000X_CTRL1r(unit, pc, &fiber));
    if (fiber & COMBO_1000X_AUTO_DETECT) {
        /* Get status from 1000X_STAT1r */
        SOC_IF_ERROR_RETURN
            (READ_SERDESCOMBO_5601X_1000X_STAT1r(unit, pc, &fiber));
        *fiber_status = !(fiber & COMBO_1000X_STATUS1_SGMII_MODE);
    } else {
        /* Get from forced mode */
        *fiber_status = (fiber & COMBO_1000X_FIBER_MODE);
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_serdescombo_5601x_tx_control_set
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
_phy_serdescombo_5601x_tx_control_set(int unit, phy_ctrl_t *pc,
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
        /* If customer uses 1G fiber link, preemph should be set to
         * 0%. Currently, no SDK has 1G fiber link on combo SerDes
         * port. If customer need to use such configuration, change 
         * to preemph = 0.
         */  
        preemph = 0x9;
        idriver = 0x7;
        pdriver = 0x9;
    } else {
        preemph = 0x9;
        idriver = 0x7;
        pdriver = 0x9;
    }

    /* Read config override */
    preemph = soc_property_port_get(unit, port, 
                                    spn_SERDES_PREEMPHASIS, preemph);
    idriver = soc_property_port_get(unit, port,
                                    spn_SERDES_DRIVER_CURRENT, idriver);
    pdriver = soc_property_port_get(unit, port,
                                    spn_SERDES_PRE_DRIVER_CURRENT, pdriver);

    /* NB: preemphasis, driver, pre-driver currents are 
     * bit-reversed in HW */
    preemph = _shr_bit_rev8((uint8)preemph);
    idriver = _shr_bit_rev8((uint8)idriver);
    pdriver = _shr_bit_rev8((uint8)pdriver);

    /* Combo SerDes 
     * preemph[15:12], idriver[11:8], pdriver[7:4]
     */
    preemph = (preemph << 8) & 0xf000;
    idriver = (idriver << 4) & 0x0f00;
    pdriver = pdriver & 0x00f0;
    data16  = (uint16)(preemph | idriver | pdriver);

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_MISC_TX_ACTRL3r(unit, pc, data16, 0xfff0));

    return SOC_E_NONE;
}

/***********************************************************************
 *
 * FIBER DRIVER ROUTINES
 */
STATIC int
_phy_serdescombo_5601x_combo_serdes_init(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;

    if (!PHY_EXTERNAL_MODE(unit, port)) {

        /* PHY state is initialized in phy_serdescombo_5601x_init_no_reset */
        pc = INT_PHY_SW_STATE(unit, port);

        /* If no external PHY, advertise 2.5 Gpbs support */
        pc->fiber.autoneg_advert |= SOC_PM_2500MB_FD;
 
        /* Disable use_ieee to store over 1G capability advertisement
         * in IEEE block space.
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDESCOMBO_5601X_DIGI3_CTRL0r(unit, pc, 0, 1));

        /* Advertise 2.5 Gbps */
        SOC_IF_ERROR_RETURN
            (WRITE_SERDESCOMBO_5601X_DIGI3_CTRLBr(unit, pc, 1));
    }
    return SOC_E_NONE;
}

STATIC int
phy_serdescombo_5601x_init_no_reset(int unit, soc_port_t port)
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

    SOC_IF_ERROR_RETURN(_phy_serdescombo_5601x_sregs_init(pc));

    /* Since physerdescombo_5601x driver reset function vector does not reset
     * the SerDes, reset the SerDes in initialization first. 
     * The internal SERDES PHY's reset bit is self-clearing.
     */
    SOC_IF_ERROR_RETURN
        (WRITE_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, COMBO_MII_CTRL_RESET));

    sal_usleep(10000);
    SOC_IF_ERROR_RETURN(_phy_serdescombo_5601x_sregs_init(pc));

    mask16 = COMBO_MII_CTRL_AN_ENABLE | COMBO_MII_CTRL_AN_RESTART |
             COMBO_MII_CTRL_FD;
    if (PHY_COPPER_MODE(unit, port) && !PHY_SGMII_AUTONEG_MODE(unit, port)) {
        data16 = COMBO_MII_CTRL_FD;
    } else {
        data16 = COMBO_MII_CTRL_AN_ENABLE | COMBO_MII_CTRL_AN_RESTART |
                 COMBO_MII_CTRL_FD;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, data16, mask16));

    SOC_IF_ERROR_RETURN
        (phy_serdescombo_5601x_adv_local_set(unit, port, pc->fiber.autoneg_advert));

    /* Configure default preemphasis, predriver, idriver values */
    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_tx_control_set(unit, pc, port));

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_1000X_CTRL2r(unit, pc, 
                       (COMBO_1000X_FALSE_LNK_DIS | COMBO_1000X_FLT_FORCE_EN),
                       (COMBO_1000X_FALSE_LNK_DIS | COMBO_1000X_FLT_FORCE_EN)));

    data16 = 0;
    mask16 = COMBO_1000X_AUTO_DETECT | COMBO_1000X_FIBER_MODE;
    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_AUTOMEDIUM, FALSE)) {
        data16 |= COMBO_1000X_AUTO_DETECT;
    }
    /*
     * Put the Serdes in Fiber or SGMII mode
     */
    if (fiber) {
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_FIBER_PREF, TRUE)) {
            data16 |= COMBO_1000X_FIBER_MODE;
        }
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_1000X_CTRL1r(unit, pc, data16, mask16));

    if (!fiber) {
        /* Do not wait 10ms for sync status before a valid link is 
         * established when auto-negotiation is disabled.
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDESCOMBO_5601X_1000X_CTRL2r(unit, pc, 0x0000, 0x0004));
    }
   
    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_combo_serdes_init(unit, port));
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdescombo_5601x_init: u=%d p=%d %s\n"),
              unit, port, (fiber) ? "Fiber" : "Copper"));
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_serdescombo_5601x_init
 * Purpose:     
 *      Initialize serdescombo_5601x internal PHY driver
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 * Notes:
 *      This function implements the bulk part of phy_serdescombo_5601x_init and
 *      provides a method for initializing the SerDes when it is used
 *      as an intermediate PHY between the MAC and an external PHY. 
 *      In the latter case the PHY driver reset callback should be
 *      called only once, and this has already been done by the 
 *      external PHY driver init function.
 */

STATIC int
phy_serdescombo_5601x_init(int unit, soc_port_t port)
{


    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdescombo_5601x_init: u=%d p=%d\n"),
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
        (phy_serdescombo_5601x_init_no_reset(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdescombo_5601x_enable_set
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
phy_serdescombo_5601x_enable_set(int unit, soc_port_t port, int enable)
{
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdescombo_5601x_enable_set: u=%d p=%d en=%d\n"),
              unit, port, enable));
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_serdescombo_5601x_notify_resume(unit, port, PHY_STOP_PHY_DIS));
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);

        SOC_IF_ERROR_RETURN
            (_phy_serdescombo_5601x_notify_stop(unit, port, PHY_STOP_PHY_DIS));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_serdescombo_5601x_sgmii_speed_set(int unit, soc_port_t port, int speed)
{
    uint16 mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    switch (speed) {
        case 10:
            mii_ctrl = COMBO_MII_CTRL_SS_10;
            break;
        case 100:
            mii_ctrl = COMBO_MII_CTRL_SS_100;
            break;
        case 0:
        case 1000:  
            mii_ctrl = COMBO_MII_CTRL_SS_1000;
            break;
        default:
            return SOC_E_CONFIG;
    } 
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, mii_ctrl,
                             (COMBO_MII_CTRL_SS_LSB | COMBO_MII_CTRL_SS_MSB)));
    return SOC_E_NONE;
}

STATIC int
_phy_serdescombo_5601x_1000x_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    uint16 data;
    uint16 mask;

    pc = INT_PHY_SW_STATE(unit, port);

    if ((speed != 0) && (speed != 1000) && (speed != 2500)) {
        return SOC_E_CONFIG;
    }

    mask = COMBO_MISC_MISC2_PLL_MODE_DIGITAL | 
           COMBO_MISC_MISC2_PLL_MODE_ANALOG;
    data = (speed == 2500) ? mask : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_MISC_MISC2r(unit, pc, data, mask));
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdescombo_5601x_speed_set
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
phy_serdescombo_5601x_speed_set(int unit, soc_port_t port, int speed)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;
    int                 rv;

    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        /* SGMII mode */
        rv = _phy_serdescombo_5601x_sgmii_speed_set(unit, port, speed);
    } else {
        rv = _phy_serdescombo_5601x_1000x_speed_set(unit, port, speed);
    }

    if (SOC_SUCCESS(rv)) {
        pc->fiber.force_speed = speed;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdescombo_5601x_speed_set: u=%d p=%d speed=%d rv=%d\n"),
              unit, port, speed, rv));
    
    return rv;
}

STATIC int 
_phy_serdescombo_5601x_sgmii_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16 mii_ctrl, mii_stat, mii_anp;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_STATr(unit, pc, &mii_stat));
 
    if (mii_ctrl & COMBO_MII_CTRL_AN_ENABLE) { /*Auto-negotiation enabled */  
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *speed = 0;
            return SOC_E_NONE;
        } else { /* Read Advertise link partner */
            SOC_IF_ERROR_RETURN
                (READ_SERDESCOMBO_5601X_MII_ANPr(unit, pc, &mii_anp));
            if (mii_anp & COMBO_MII_ANP_SGMII) {
                switch(mii_anp & COMBO_MII_ANP_SGMII_SPEED) {
                case COMBO_MII_ANP_SGMII_SPEED_1000:
                    *speed = 1000;
                    break;
                case COMBO_MII_ANP_SGMII_SPEED_100:
                    *speed = 100;
                    break;
                case COMBO_MII_ANP_SGMII_SPEED_10:
                    *speed = 10;
                    break;
                default:
                    return SOC_E_UNAVAIL;
                }
            }
        }
    } else {  /* Forced speed */
        switch(mii_ctrl & (COMBO_MII_CTRL_SS_LSB | COMBO_MII_CTRL_SS_MSB)) {
        case COMBO_MII_CTRL_SS_10:
            *speed = 10; 
            break;
        case COMBO_MII_CTRL_SS_100:
            *speed = 100;
            break;
        case COMBO_MII_CTRL_SS_1000:
            *speed = 1000;
            break;
        default:
            return SOC_E_UNAVAIL;
        } 
    }
    return SOC_E_NONE;
}

STATIC int
_phy_serdescombo_5601x_1000x_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t  *pc;
    uint16       an_over1g;   /* Auto-neg advertise over 1 Gbps */
    uint16       lp_over1g;   /* Link partner advertise over 1 Gbps */
    uint16       mii_status;  /* MII status register */
    uint16       misc2;       /* Mis2 register */
    uint16       mii_ctrl;

    pc = INT_PHY_SW_STATE(unit, port);

    *speed = 1000;
    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_STATr(unit, pc, &mii_status));

    if ((mii_ctrl & COMBO_MII_CTRL_AN_ENABLE) &&
        (mii_status & COMBO_MII_STAT_AN_COMPLETE)) {
        /* Map digital block */
        SOC_IF_ERROR_RETURN
            (READ_SERDESCOMBO_5601X_DIGI3_CTRLBr(unit, pc, &an_over1g));
        SOC_IF_ERROR_RETURN
            (READ_SERDESCOMBO_5601X_DIGI3_CTRLDr(unit, pc, &lp_over1g));
        if ((an_over1g & COMBO_DIGI3_CTRLB_OVER_1G) &&
            (lp_over1g & COMBO_DIGI3_CTRLD_OVER_1G)) {
            /* Both local and link partner supports 2.5 Gbps */
            *speed = 2500;
        } else {
            *speed = 1000;
        }
    } else {
        /* Auto-neg is disabled or not completed */
        SOC_IF_ERROR_RETURN
            (READ_SERDESCOMBO_5601X_MISC_MISC2r(unit, pc, &misc2));

        /* Check whether digital and analog PLL are forced to 2.5 Gbps */ 
        misc2 &= (COMBO_MISC_MISC2_PLL_MODE_DIGITAL | 
                  COMBO_MISC_MISC2_PLL_MODE_ANALOG);
        *speed = (misc2) ? 2500 : 1000;
    }
  
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdescombo_5601x_speed_get
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
phy_serdescombo_5601x_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *speed = 1000;

    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        SOC_IF_ERROR_RETURN
            (_phy_serdescombo_5601x_sgmii_speed_get(unit, port, speed));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_serdescombo_5601x_1000x_speed_get(unit, port, speed));
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdescombo_5601x_an_set
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
phy_serdescombo_5601x_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16             par_det;
    uint16             an_enable;
    uint16             auto_det;

    pc = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdescombo_5601x_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));
    /*
     * Special case for BCM5421S being used in SGMII copper mode with
     * BCMserdescombo_5601x.  Always have autoneg on.
     */
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_5421S)) {
        an = 1;
    }

    par_det   = 0;
    an_enable = 0; 
    auto_det  = 0;
    if (an) {
        par_det = COMBO_1000X_PAR_DET_EN;
        an_enable = COMBO_MII_CTRL_AN_ENABLE | COMBO_MII_CTRL_AN_RESTART;
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_AUTOMEDIUM, TRUE)) {
            auto_det = COMBO_1000X_AUTO_DETECT; 
        }
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_1000X_CTRL2r(unit, pc,
                        par_det, COMBO_1000X_PAR_DET_EN));

    /* Enable/Disable and Restart autonegotiation (self-clearing bit) */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, an_enable,
                        COMBO_MII_CTRL_AN_ENABLE | COMBO_MII_CTRL_AN_RESTART));

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_1000X_CTRL1r(unit, pc, auto_det,
                        COMBO_1000X_AUTO_DETECT));

    pc->fiber.autoneg_enable = an;

    if (an) {
        /* If auto-neg is enabled, make sure not forcing 
         * digital and analog PLL to 2.5 Gbps. 
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDESCOMBO_5601X_MISC_MISC2r(unit, pc, 0,
                             COMBO_MISC_MISC2_PLL_MODE_DIGITAL | 
                             COMBO_MISC_MISC2_PLL_MODE_ANALOG)); 
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdescombo_5601x_adv_local_set
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
phy_serdescombo_5601x_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
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
        an_adv |= COMBO_MII_ANA_FD;
    }

    /*
     * Set advertised pause bits in link code word.
     */
    switch (mode & SOC_PM_PAUSE) {
    case SOC_PM_PAUSE_TX:
        an_adv |= COMBO_MII_ANA_PAUSE_ASYM;
        break;
    case SOC_PM_PAUSE_RX:
        an_adv |= COMBO_MII_ANA_PAUSE_SYM | COMBO_MII_ANA_PAUSE_ASYM;
        break;
    case SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX:
        an_adv |= COMBO_MII_ANA_PAUSE_SYM;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_MII_ANAr(unit, pc, an_adv,
                        COMBO_MII_ANA_PAUSE_ASYM | COMBO_MII_ANA_PAUSE_SYM |
                        COMBO_MII_ANA_FD | COMBO_MII_ANA_HD));

    an_adv = (mode & SOC_PM_2500MB_FD) ? COMBO_DIGI3_CTRLB_OVER_1G : 0;
    SOC_IF_ERROR_RETURN
        (WRITE_SERDESCOMBO_5601X_DIGI3_CTRLBr(unit, pc, an_adv));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdescombo_5601x_adv_local_set: u=%d p=%d adv=%s%s%s%s\n"),
              unit, port,
              (mode & SOC_PM_2500MB_FD) ? "2500MB " : "",
              (mode & SOC_PM_1000MB_FD) ? "1000MB " : "",
              (mode & SOC_PM_PAUSE_TX)  ? "PAUSE_TX " : "",    
              (mode & SOC_PM_PAUSE_RX)  ? "PAUSE_TX " : ""));    
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdescombo_5601x_adv_local_get
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
phy_serdescombo_5601x_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      an_adv;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_ANAr(unit, pc, &an_adv));

    if (an_adv & COMBO_MII_ANA_FD) {
        *mode |= SOC_PM_1000MB_FD;
    }

    switch (an_adv & (COMBO_MII_ANA_PAUSE_SYM | COMBO_MII_ANA_PAUSE_ASYM)) {
    case COMBO_MII_ANA_PAUSE_SYM:
        *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
        break;
    case COMBO_MII_ANA_PAUSE_ASYM:
        *mode |= SOC_PM_PAUSE_TX;
        break;
    case COMBO_MII_ANA_PAUSE_SYM | COMBO_MII_ANA_PAUSE_ASYM:
        *mode |= SOC_PM_PAUSE_RX;
        break;
    }

    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_DIGI3_CTRLBr(unit, pc, &an_adv));
    *mode |= (an_adv & COMBO_DIGI3_CTRLB_OVER_1G) ? 
                     SOC_PM_2500MB_FD : 0;

    return SOC_E_NONE;
}

STATIC int
_phy_serdescombo_5601x_sgmii_adv_remote_get(int unit, soc_port_t port, 
                                soc_port_mode_t *mode)
{
    uint16              anlpa;
    soc_port_mode_t     duplex;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_ANPr(unit, pc, &anlpa));

    switch (anlpa & COMBO_MII_ANP_SGMII_SPEED) {
    case COMBO_MII_ANP_SGMII_SPEED_10:
        *mode = SOC_PM_10MB;
        break;
    case COMBO_MII_ANP_SGMII_SPEED_100:
        *mode = SOC_PM_100MB;
        break;
    case COMBO_MII_ANP_SGMII_SPEED_1000:
        *mode = SOC_PM_1000MB;
        break;
    } 

    duplex = (anlpa & COMBO_MII_ANP_SGMII_DUPLEX) ? SOC_PM_FD : SOC_PM_HD;

    *mode = (*mode) & duplex; 

    return SOC_E_NONE;
}

STATIC int
_phy_serdescombo_5601x_1000x_adv_remote_get(int unit, soc_port_t port,
                                soc_port_mode_t *mode)
{
    uint16              anlpa;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_ANPr(unit, pc, &anlpa));

    if (anlpa & COMBO_MII_ANP_FIBER_FD) {
        *mode |= SOC_PM_1000MB_FD;
    }

    if (anlpa & COMBO_MII_ANP_FIBER_HD) {
        *mode |= SOC_PM_1000MB_HD;
    }

    switch (anlpa &
            (COMBO_MII_ANP_FIBER_PAUSE_SYM | COMBO_MII_ANP_FIBER_PAUSE_ASYM)) {
    case COMBO_MII_ANP_FIBER_PAUSE_SYM:
        *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
        break;
    case COMBO_MII_ANP_FIBER_PAUSE_ASYM:
        *mode |= SOC_PM_PAUSE_TX;
        break;
    case COMBO_MII_ANP_FIBER_PAUSE_SYM | COMBO_MII_ANP_FIBER_PAUSE_ASYM:
        *mode |= SOC_PM_PAUSE_RX;
        break;
    }
    
    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_DIGI3_CTRLDr(unit, pc, &anlpa));
    *mode |= (anlpa & COMBO_DIGI3_CTRLD_OVER_1G) ? 
             SOC_PM_2500MB_FD : 0;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdescombo_5601x_adv_remote_get
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
phy_serdescombo_5601x_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        SOC_IF_ERROR_RETURN
            (_phy_serdescombo_5601x_sgmii_adv_remote_get(unit, port, mode));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_serdescombo_5601x_1000x_adv_remote_get(unit, port, mode));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdescombo_5601x_adv_remote_get: u=%d p=%d adv=%s%s%s%s\n"),
              unit, port,
              (*mode & SOC_PM_2500MB_FD) ? "2500MB " : "",
              (*mode & SOC_PM_1000MB_FD) ? "1000MB " : "",
              (*mode & SOC_PM_PAUSE_TX)  ? "PAUSE_TX " : "",    
              (*mode & SOC_PM_PAUSE_RX)  ? "PAUSE_TX " : ""));    

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_serdescombo_5601x_ability_get
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
phy_serdescombo_5601x_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        *mode = SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB_FD |
                SOC_PM_LB_PHY | SOC_PM_GMII;
    } else {
        *mode = SOC_PM_1000MB_FD | SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM |
                SOC_PM_AN | SOC_PM_LB_PHY | SOC_PM_GMII | 
                SOC_PM_2500MB_FD | SOC_PM_SGMII;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_serdescombo_5601x_medium_config_update
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
_phy_serdescombo_5601x_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_serdescombo_5601x_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_serdes_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_serdescombo_5601x_adv_local_set(unit, port, cfg->autoneg_advert));
    SOC_IF_ERROR_RETURN
        (phy_serdescombo_5601x_an_set(unit, port, cfg->autoneg_enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_serdescombo_5601x_medium_config_set
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
phy_serdescombo_5601x_medium_config_set(int unit, soc_port_t port, 
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
                (_phy_serdescombo_5601x_medium_config_update(unit, port, &pc->fiber));
        }
        return SOC_E_NONE;
    case SOC_PORT_MEDIUM_COPPER:
        return SOC_E_UNAVAIL;
    default:
        return SOC_E_PARAM;
    }
}

STATIC int
_phy_serdescombo_5601x_control_tx_driver_set(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 value)
{
    uint16              data;  /* Temporary holder of reg value to be written */
    uint16              mask;  /* Bit mask of reg value to be updated */
    phy_ctrl_t  *pc;    /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    /* Combo SerDes */
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         data = (uint16)(value & 0xf);
         data = _shr_bit_rev16(value);
         mask = 0xF000;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         data = (uint16)(value & 0xf);
         data = _shr_bit_rev16(value);
         data = data >> 4;
         mask = 0x0F00;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         data = (uint16)(value & 0xf);
         data = _shr_bit_rev16(value);
         data = data >> 8;
         mask = 0x00F0;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }
    /* Combo SerDes */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_MISC_TX_ACTRL3r(unit, pc, data, mask));        

    return SOC_E_NONE;
}

STATIC int
_phy_serdescombo_5601x_control_tx_driver_get(int unit, soc_port_t port,
                                soc_phy_control_t type, uint32 *value)
{
    uint16                data16;   /* Temporary holder of a reg value */
    uint8                 data8;    /* Temporary holder of 8 bit value */
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data16 = 0;
    data8  = 0;
    /* Combo SerDes */
    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MISC_TX_ACTRL3r(unit, pc, &data16));
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         data8  = (uint8)((data16 & 0xF000) >> 8);
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         data8  = (uint8)((data16 & 0x0F00) >> 4);
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         data8 = (uint8)(data16 & 0x00F0); 
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    *value = _shr_bit_rev8(data8);
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_physerdescombo_5601x_control_set
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
phy_serdescombo_5601x_control_set(int unit, soc_port_t port,
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
        rv = _phy_serdescombo_5601x_control_tx_driver_set(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        /* enable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDESCOMBO_5601X_1000X_CTRL2r(unit, pc,
               value? COMBO_1000X_PAR_DET_EN:0,
               COMBO_1000X_PAR_DET_EN));
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
 *      phy_serdescombo_5601x_control_get
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
phy_serdescombo_5601x_control_get(int unit, soc_port_t port,
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
        rv = _phy_serdescombo_5601x_control_tx_driver_get(unit, port, type, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        SOC_IF_ERROR_RETURN
            (READ_SERDESCOMBO_5601X_1000X_CTRL2r(unit, pc,&data16));
        *value = data16 & COMBO_1000X_PAR_DET_EN?  TRUE: FALSE;
        rv = SOC_E_NONE;
        break;

    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

STATIC int
phy_serdescombo_5601x_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_serdescombo_5601x_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_serdescombo_5601x_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_serdescombo_5601x_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_serdescombo_5601x_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_serdescombo_5601x_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        rv = (phy_serdescombo_5601x_an_set(unit, port, value));
        break;
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}

STATIC int
phy_serdescombo_5601x_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    *duplex = TRUE;

    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber_status == 0) {
        /* retrieve the duplex setting in SGMII mode */
        SOC_IF_ERROR_RETURN
            (READ_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, &mii_ctrl));

        if (mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_SERDESCOMBO_5601X_MII_ANPr(unit,pc,&reg0_16));

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

STATIC int
phy_serdescombo_5601x_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      ctrl, stat;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, &ctrl));

    *an = (ctrl & MII_CTRL_AE) != 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_STATr(unit, pc, &stat));

    *an_done = (stat & MII_STAT_AN_DONE) != 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdescombo_5601x_an_get: u=%d p=%d an=%d an_done=%d\n"),
              unit, port, *an, *an_done));

    return SOC_E_NONE;
}

STATIC int
phy_serdescombo_5601x_lb_set(int unit, soc_port_t port, int enable)
{
    uint16              ctrl;
    phy_ctrl_t  *pc;
    int                 rv;

    pc = INT_PHY_SW_STATE(unit, port);

    ctrl = (enable) ?MII_CTRL_LE : 0;

    rv = MODIFY_SERDESCOMBO_5601X_MII_CTRLr(unit, pc,
                                 ctrl, MII_CTRL_LE);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdescombo_5601x_lb_set: u=%d p=%d lb=%d rv=%d\n"),
              unit, port, enable, rv));

    return rv;
}


STATIC int
phy_serdescombo_5601x_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, &ctrl));

    *enable = (ctrl & MII_CTRL_LE) != 0;

    return SOC_E_NONE;
}




STATIC int
phy_serdescombo_5601x_link_get(int unit, soc_port_t port, int *link)
{
    uint16       mii_stat;
    int          an_done;
    phy_ctrl_t  *pc;

    if (PHY_DISABLED_MODE(unit, port)) {
        *link = FALSE;
    } else {
        pc = INT_PHY_SW_STATE(unit, port);

        SOC_IF_ERROR_RETURN
            (READ_SERDESCOMBO_5601X_MII_STATr(unit, pc, &mii_stat));

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
 * Variable:    phy_serdescombo_5601x
 * Purpose:     PHY Driver for Dozen SerDes Serdes PHY internal to Draco, 
 */
phy_driver_t phy_serdescombo_5601x_ge = {
    "Internal 2.5G SERDES PHY Driver",
    phy_serdescombo_5601x_init,
    phy_null_reset,
    phy_serdescombo_5601x_link_get,     /* local */
    phy_serdescombo_5601x_enable_set, 
    phy_null_enable_get, 
    phy_serdes_duplex_set,           
    phy_serdescombo_5601x_duplex_get,               
    phy_serdescombo_5601x_speed_set, 
    phy_serdescombo_5601x_speed_get, 
    phy_serdes_master_set,          
    phy_serdes_master_get,          
    phy_serdescombo_5601x_an_set,        /* local */
    phy_serdescombo_5601x_an_get,        /* local */
    phy_serdescombo_5601x_adv_local_set,
    phy_serdescombo_5601x_adv_local_get,
    phy_serdescombo_5601x_adv_remote_get,
    phy_serdescombo_5601x_lb_set,        /* local */
    phy_serdescombo_5601x_lb_get,        /* local */
    phy_serdes_interface_set, 
    phy_serdes_interface_get, 
    phy_serdescombo_5601x_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    phy_serdescombo_5601x_medium_config_set,
    phy_serdes_medium_config_get,
    phy_serdes_medium_status,
    NULL,                          /* phy_cable_diag  */
    NULL,                          /* phy_link_change */
    phy_serdescombo_5601x_control_set,         /* phy_control_set */
    phy_serdescombo_5601x_control_get,         /* phy_control_get */
    phy_serdes_reg_read,
    phy_serdes_reg_write,
    phy_serdes_reg_modify,
    phy_serdescombo_5601x_notify
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _phy_serdescombo_5601x_notify_duplex
 * Purpose:     
 *      Program duplex if (and only if) serdescombo_5601x is an intermediate PHY.
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
_phy_serdescombo_5601x_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;

    fiber = PHY_FIBER_MODE(unit, port);
    pc    = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_serdescombo_5601x_notify_duplex: "
                         "u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, 
                                   COMBO_MII_CTRL_FD, COMBO_MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ? COMBO_MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, mii_ctrl, COMBO_MII_CTRL_FD)); 

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_serdescombo_5601x_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_serdescombo_5601x_notify_speed
 * Purpose:     
 *      Program duplex if (and only if) serdescombo_5601x is an intermediate PHY.
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
_phy_serdescombo_5601x_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int          fiber;
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    fiber = PHY_FIBER_MODE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_serdescombo_5601x_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }
 
    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_fiber_status_get(unit, pc, &fiber_status));
    if (fiber && fiber_status) {
        if ((speed != 0) && (speed != 1000)) {
            return SOC_E_CONFIG;
        }
    }

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_sgmii_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_serdescombo_5601x_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && !fiber) {
        SOC_IF_ERROR_RETURN
            (phy_serdescombo_5601x_an_set(unit, port, FALSE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_serdescombo_5601x_stop
 * Purpose:
 *      Put serdescombo_5601x SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_serdescombo_5601x_stop(int unit, soc_port_t port)
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
                         "phy_serdescombo_5601x_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    mii_ctrl = (stop) ? COMBO_MII_CTRL_PD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_MII_CTRLr(unit, pc, mii_ctrl, COMBO_MII_CTRL_PD)); 

    return SOC_E_NONE; 
}

/*
 * Function:
 *      _phy_serdescombo_5601x_notify_stop
 * Purpose:     
 *      Add a reason to put serdescombo_5601x PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_serdescombo_5601x_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;

    return _phy_serdescombo_5601x_stop(unit, port);
}

/*
 * Function:
 *      _phy_serdescombo_5601x_notify_resume
 * Purpose:     
 *      Remove a reason to put serdescombo_5601x PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_serdescombo_5601x_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;

    return _phy_serdescombo_5601x_stop(unit, port);
}

/*
 * Function:
 *      phy_serdescombo_5601x_media_setup
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
_phy_serdescombo_5601x_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;

    pc = INT_PHY_SW_STATE(unit, port);

    data16 = 0;
    if (intf != SOC_PORT_IF_SGMII) {
        data16 = COMBO_1000X_FIBER_MODE;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDESCOMBO_5601X_1000X_CTRL1r(unit, pc, data16, 
                                         COMBO_1000X_FIBER_MODE));

    return SOC_E_NONE;
}

#else /* INCLUDE_SERDES_COMBO */
typedef int _phy_serdescombo_5601x_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_SERDES_COMBO */



