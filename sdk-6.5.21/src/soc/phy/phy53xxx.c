/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        serdes.c
 * Purpose:
 *	Fiber driver for 53xxx using internal QuadSerdes PHY.
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_53XXX)
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
#include "phy53xxx.h"
#define ADVERT_ALL_FIBER \
            (SOC_PM_PAUSE | SOC_PM_1000MB_FD)


/* Notify event forward declaration */
STATIC int _phy_53xxx_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int _phy_53xxx_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int _phy_53xxx_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_53xxx_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_53xxx_notify_interface(int unit, soc_port_t port, uint32 intf);

/***********************************************************************
 *
 * FIBER DRIVER ROUTINES
 */
STATIC int
phy_53xxx_init_no_reset(int unit, soc_port_t port)
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

    /* Since phy53xxx driver reset function vector does not reset
     * the SerDes, reset the SerDes in initialization first. 
     * The internal SERDES PHY's reset bit is self-clearing.
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_MII_CTRLr(unit, pc,
                               MII_CTRL_RESET, MII_CTRL_RESET));

    soc_timeout_init(&to, 10000, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_SERDES_MII_CTRLr(unit, pc, &data16);
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

    data16 = 0;
    mask16 = QS_MII_CTRL_AN_ENABLE | QS_MII_CTRL_AN_RESTART;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) || 
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        data16 = QS_MII_CTRL_AN_ENABLE | QS_MII_CTRL_AN_RESTART;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_MII_CTRLr(unit, pc, data16, mask16));

    data16 = QS_1000X_FALSE_LNK_DIS | QS_1000X_FLT_FORCE_EN;
    mask16 = QS_1000X_FALSE_LNK_DIS | QS_1000X_FLT_FORCE_EN;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_1000X_CTRL2r(unit, pc, data16, mask16));

    data16 = 0;
    mask16 = QS_1000X_AUTO_DETECT | QS_1000X_FIBER_MODE;
    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_AUTOMEDIUM, FALSE)) {
        data16 |= QS_1000X_AUTO_DETECT;
    }
    /*
     * Put the Serdes in Fiber or SGMII mode
     */
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port)) {
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_FIBER_PREF, TRUE)) {
            data16 |= QS_1000X_FIBER_MODE;
        }
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_1000X_CTRL1r(unit, pc, data16, mask16));

    if (PHY_COPPER_MODE(unit, port)) {
        /* Do not wait 10ms for sync status before a valid link is 
         * established when auto-negotiation is disabled.
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES_1000X_CTRL2r(unit, pc, 0x0000, 0x0004));
    }
   
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_53xxx_init: u=%d p=%d %s\n"),
              unit, port, 
              PHY_FIBER_MODE(unit, port) ? "Fiber" : "Copper"));
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_53xxx_init
 * Purpose:     
 *      Initialize 53xxx internal PHY driver
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 * Returns:     
 *      SOC_E_NONE
 * Notes:
 *      This function implements the bulk part of phy_53xxx_init and
 *      provides a method for initializing the SerDes when it is used
 *      as an intermediate PHY between the MAC and an external PHY. 
 *      In the latter case the PHY driver reset callback should be
 *      called only once, and this has already been done by the 
 *      external PHY driver init function.
 */

STATIC int
phy_53xxx_init(int unit, soc_port_t port)
{

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_53xxx_init: u=%d p=%d\n"),
              unit, port));

    if (!PHY_EXTERNAL_MODE(unit, port)) {
        /* PHY reset callbacks */
        SOC_IF_ERROR_RETURN(soc_phy_reset(unit, port));

        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
    }
    SOC_IF_ERROR_RETURN
        (phy_53xxx_init_no_reset(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_53xxx_enable_set
 * Purpose:     
 *      Enable or disable the physical interface.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 *      enable - Boolean, true = enable PHY, false = disable.
 * Returns:     
 *      SOC_E_XXX
 * Notes:
 *      The PHY is held in reset when disabled.  Also, the
 *      MISC_CONTROL.IDDQ bit must be set to 1 so the remote partner
 *      sees link down.
 */

STATIC int
phy_53xxx_enable_set(int unit, soc_port_t port, int enable)
{
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_53xxx_enable_set: u=%d p=%d en=%d\n"),
              unit, port, enable));
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    return SOC_E_NONE;
}

STATIC int
_phy_53xxx_sgmii_speed_set(int unit, soc_port_t port, int speed)
{
    uint16 mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    switch (speed) {
        case 10:
            mii_ctrl = QS_MII_CTRL_SS_10;
            break;
        case 100:
            mii_ctrl = QS_MII_CTRL_SS_100;
            break;
        case 0:
        case 1000:  
            mii_ctrl = QS_MII_CTRL_SS_1000;
            break;
        default:
            return SOC_E_CONFIG;
    } 
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_MII_CTRLr(unit, pc, mii_ctrl,
                               (QS_MII_CTRL_SS_LSB | QS_MII_CTRL_SS_MSB)));
    return SOC_E_NONE;
}

STATIC int
_phy_53xxx_1000x_speed_set(int unit, soc_port_t port, int speed)
{
    if ((speed !=0) && (speed != 1000)) {
        return SOC_E_CONFIG;
    } 

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_53xxx_speed_set
 * Purpose:     
 *      Set the current operating speed (forced).
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 *      duplex - (OUT) Boolean, true indicates full duplex, false 
 *              indicates half.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_53xxx_speed_set(int unit, soc_port_t port, int speed)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;
    int                 rv;

    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;
    
    SOC_IF_ERROR_RETURN
        (READ_SERDES_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & QS_1000X_STATUS1_SGMII_MODE) {
        rv = _phy_53xxx_sgmii_speed_set(unit, port, speed);
    } else {
        rv = _phy_53xxx_1000x_speed_set(unit, port, speed);
    }

    if (SOC_SUCCESS(rv)) {
        pc->fiber.force_speed = speed;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_53xxx_speed_set: u=%d p=%d speed=%d rv=%d\n"),
              unit, port, speed, rv));
    
    return rv;
}

STATIC int 
_phy_53xxx_sgmii_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16 mii_ctrl, mii_stat, mii_anp;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_SERDES_MII_STATr(unit, pc, &mii_stat)); 
 
    if (mii_ctrl & QS_MII_CTRL_AN_ENABLE) { /*Auto-negotiation enabled */  
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *speed = 0;
            return SOC_E_NONE;
        } else { /* Read Advertise link partner */
            SOC_IF_ERROR_RETURN
                (READ_SERDES_MII_ANPr(unit, pc, &mii_anp));
            if (mii_anp & QS_MII_ANP_SGMII) {
                switch(mii_anp & QS_MII_ANP_SGMII_SPEED) {
                case QS_MII_ANP_SGMII_SPEED_1000:
                    *speed = 1000;
                    break;
                case QS_MII_ANP_SGMII_SPEED_100:
                    *speed = 100;
                    break;
                case QS_MII_ANP_SGMII_SPEED_10:
                    *speed = 10;
                    break;
                default:
                    return SOC_E_UNAVAIL;
                }
            }
        }
    } else {  /* Forced speed */
        switch(mii_ctrl & (QS_MII_CTRL_SS_LSB | QS_MII_CTRL_SS_MSB)) {
        case QS_MII_CTRL_SS_10:
            *speed = 10; 
            break;
        case QS_MII_CTRL_SS_100:
            *speed = 100;
            break;
        case QS_MII_CTRL_SS_1000:
            *speed = 1000;
            break;
        default:
            return SOC_E_UNAVAIL;
        } 
    }
    return SOC_E_NONE;
}

STATIC int
_phy_53xxx_1000x_speed_get(int unit, soc_port_t port, int *speed)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *speed = 1000;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_53xxx_speed_get
 * Purpose:     
 *      Get the current operating speed. If autoneg is enabled, 
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 *      duplex - (OUT) Boolean, true indicates full duplex, false 
 *              indicates half.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_53xxx_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *speed = 1000;
 
    SOC_IF_ERROR_RETURN
        (READ_SERDES_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & QS_1000X_STATUS1_SGMII_MODE) {
        SOC_IF_ERROR_RETURN
            (_phy_53xxx_sgmii_speed_get(unit, port, speed));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_53xxx_1000x_speed_get(unit, port, speed));
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_53xxx_an_set
 * Purpose:     
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 *      an - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_53xxx_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16       par_det;
    uint16       an_enable;
    uint16       auto_det;

    pc = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_53xxx_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));
    /*
     * Special case for BCM5421S being used in SGMII copper mode with
     * BCM53xxx.  Always have autoneg on.
     */
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_5421S)) {
        an = 1;
    }

    par_det   = 0;
    an_enable = 0; 
    auto_det  = 0;
    if (an) {
        par_det = QS_1000X_PAR_DET_EN;
        an_enable = QS_MII_CTRL_AN_ENABLE | QS_MII_CTRL_AN_RESTART;
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_AUTOMEDIUM, FALSE)) {
            auto_det = QS_1000X_AUTO_DETECT; 
        }
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_1000X_CTRL2r(unit, pc,
                        par_det, QS_1000X_PAR_DET_EN));

    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_1000X_CTRL1r(unit, pc, 
                        auto_det, QS_1000X_AUTO_DETECT));


    /* Enable/Disable and Restart autonegotiation (self-clearing bit) */
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_MII_CTRLr(unit, pc, an_enable,
                        QS_MII_CTRL_AN_ENABLE | QS_MII_CTRL_AN_RESTART));

    pc->fiber.autoneg_enable = an;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_53xxx_adv_local_set
 * Purpose:     
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int 
phy_53xxx_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    return phy_serdes_adv_local_set(unit, port, mode);
}

/*
 * Function:    
 *      phy_53xxx_adv_local_get
 * Purpose:     
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_53xxx_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    return phy_serdes_adv_local_get(unit, port, mode);
}

/*
 * Function:    
 *      phy_53xxx_adv_remote_get
 * Purpose:     
 *      Get partner's current advertisement for auto-negotiation.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_53xxx_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{ 
    return phy_serdes_adv_remote_get(unit, port, mode);
}

/*
 * Function:    
 *      phy_53xxx_lb_set
 * Purpose:     
 *      Set the internal PHY loopback mode.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 *      loopback - Boolean: true = enable loopback, false = disable.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_53xxx_lb_set(int unit, soc_port_t port, int enable)
{
    uint16       ctrl;
    phy_ctrl_t  *pc;
    int          rv;

    pc = INT_PHY_SW_STATE(unit, port);

    ctrl = (enable) ? QS_MII_CTRL_LOOPBACK : 0;

    rv = MODIFY_SERDES_MII_CTRLr(unit, pc, ctrl, QS_MII_CTRL_LOOPBACK);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_53xxx_lb_set: u=%d p=%d lb=%d rv=%d\n"),
              unit, port, enable, rv));
    return rv;
}

/*
 * Function:    
 *      phy_53xxx_ability_get
 * Purpose:     
 *      Get the abilities of the internal PHY.
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #. 
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_53xxx_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      fiber_status;
    phy_ctrl_t *pc;
    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & QS_1000X_STATUS1_SGMII_MODE) {
        *mode = (SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB_FD | 
                 SOC_PM_LB_PHY | SOC_PM_GMII);
    } else {
        *mode = (SOC_PM_1000MB_FD | SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM |
                 SOC_PM_AN | SOC_PM_LB_PHY | SOC_PM_GMII);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_53xxx_medium_config_update
 * Purpose:
 *      Update the PHY with config parameters
 * Parameters:
 *      unit - Switch unit #.
 *      port - Switch port #.
 *      cfg - Config structure.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_53xxx_medium_config_update(int unit, soc_port_t port,
                                soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_53xxx_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_serdes_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_53xxx_adv_local_set(unit, port, cfg->autoneg_advert));
    SOC_IF_ERROR_RETURN
        (phy_53xxx_an_set(unit, port, cfg->autoneg_enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_53xxx_medium_config_set
 * Purpose:
 *      Set the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - Switch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_53xxx_medium_config_set(int unit, soc_port_t port, 
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
                (_phy_53xxx_medium_config_update(unit, port, &pc->fiber));
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
 *      phy_53xxx_reg_read
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
phy_53xxx_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */


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
 *      phy_53xxx_reg_write
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
phy_53xxx_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

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
 *      phy_53xxx_reg_modify
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
phy_53xxx_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data, uint32 phy_data_mask)
{
    uint16         reg_bank;
    uint8          reg_addr;
    uint16         data;     /* Temporary holder for phy_data */
    uint16         mask;
    phy_ctrl_t    *pc;       /* PHY software state */

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
phy_53xxx_notify(int unit, soc_port_t port, 
                 soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_53xxx_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_53xxx_notify_duplex(unit, port, value));
        break; 
    case phyEventSpeed:
        rv = (_phy_53xxx_notify_speed(unit, port, value));
        break; 
    case phyEventStop:
        rv = (_phy_53xxx_notify_stop(unit, port, value));
        break; 
    case phyEventResume:
        rv = (_phy_53xxx_notify_resume(unit, port, value));
        break; 
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}


/*
 * Variable:    phy_53xxxdrv_ge
 * Purpose:     PHY Driver for Dozen SerDes Serdes PHY
 */
phy_driver_t phy_53xxxdrv_ge = {
    "53XXX Internal SERDES PHY Driver",
    phy_53xxx_init,
    phy_null_reset,
    phy_serdes_link_get, 
    phy_53xxx_enable_set, 
    phy_null_enable_get, 
    phy_serdes_duplex_set, 
    phy_null_one_get,              /* duplex_get */
    phy_53xxx_speed_set, 
    phy_53xxx_speed_get, 
    phy_serdes_master_set,          /* master_set */
    phy_serdes_master_get,          /* master_get */
    phy_53xxx_an_set,
    phy_serdes_an_get,
    phy_53xxx_adv_local_set,
    phy_53xxx_adv_local_get,
    phy_53xxx_adv_remote_get,
    phy_53xxx_lb_set,
    phy_serdes_lb_get,
    phy_serdes_interface_set, 
    phy_serdes_interface_get, 
    phy_53xxx_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    phy_53xxx_medium_config_set,
    phy_serdes_medium_config_get,
    phy_serdes_medium_status,
    NULL,                          /* phy_cable_diag  */
    NULL,                          /* phy_link_change */
    NULL,         /* phy_control_set */
    NULL,         /* phy_control_get */
    phy_53xxx_reg_read,
    phy_53xxx_reg_write,
    phy_53xxx_reg_modify,
    phy_53xxx_notify
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      phy_53xxx_notify_duplex
 * Purpose:     
 *      Program duplex if (and only if) 53xxx is an intermediate PHY.
 * Parameters:
 *      unit - unit #.
 *      port - port #. 
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
_phy_53xxx_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;

    fiber = PHY_FIBER_MODE(unit, port);
    pc    = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_53xxx_notify_duplex: u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_SERDES_MII_CTRLr(unit, pc, 
                                   QS_MII_CTRL_FD, QS_MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
        (_phy_53xxx_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ? QS_MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_MII_CTRLr(unit, pc, mii_ctrl, QS_MII_CTRL_FD)); 

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_53xxx_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_53xxx_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_53xxx_notify_speed
 * Purpose:     
 *      Program duplex if (and only if) 53xxx is an intermediate PHY.
 * Parameters:
 *      unit - unit #.
 *      port - port #. 
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
_phy_53xxx_notify_speed(int unit, soc_port_t port, uint32 speed)
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
        (READ_SERDES_1000X_STAT1r(unit, pc,
                         &fiber_status));

    if (fiber && !(fiber_status & QS_1000X_STATUS1_SGMII_MODE)) {
        if ((speed != 0) && (speed != 1000)) {
            return SOC_E_CONFIG;
        }
    }

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_53xxx_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (_phy_53xxx_sgmii_speed_set(unit, port, (int)speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_53xxx_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && !fiber) {
        SOC_IF_ERROR_RETURN
            (phy_53xxx_an_set(unit, port, FALSE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_53xxx_stop
 * Purpose:
 *      Put 53xxx SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_53xxx_stop(int unit, soc_port_t port)
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
                         "phy_53xxx_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    mii_ctrl = (stop) ? QS_MII_CTRL_PD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_MII_CTRLr(unit, pc, mii_ctrl, QS_MII_CTRL_PD));

    return SOC_E_NONE; 
}

/*
 * Function:
 *      phy_53xxx_notify_stop
 * Purpose:
 *      Add a reason to put 53xxx PHY in reset.
 * Parameters:
 *      unit - unit #.
 *      port - port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_53xxx_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    pc->stop |= flags;

    return _phy_53xxx_stop(unit, port);
}

/*
 * Function:
 *      phy_53xxx_notify_resume
 * Purpose:
 *      Remove a reason to put 53xxx PHY in reset.
 * Parameters:
 *      unit - unit #.
 *      port - port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_53xxx_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    pc->stop &= ~flags;

    return _phy_53xxx_stop(unit, port);
}


STATIC int 
_phy_53xxx_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;

    pc = INT_PHY_SW_STATE(unit, port);

    data16 = (intf == SOC_PORT_IF_SGMII) ? 0 : QS_1000X_FIBER_MODE; 
    SOC_IF_ERROR_RETURN
        (MODIFY_SERDES_1000X_CTRL1r(unit, pc, data16, QS_1000X_FIBER_MODE));

    return SOC_E_NONE;
}


#else /* INCLUDE_PHY_53XXX */
typedef int _phy_53xxx_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_53XXX */

