/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgxs.c
 * Purpose:     Broadcom 10/100/1000/2500/10000/12000/13000/16000 SerDes 
 *              (SerDes-XGXS/Unicore)
 */

#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_XGXS1)
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
#include "xgxs.h"
#include "xgxs1.h"

STATIC int phy_xgxs1_lb_set(int unit, soc_port_t port, int enable);

/*
 * Function:
 *      phy_xgxs1_init
 * Purpose:     
 *      Initialize xgxs phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs1_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    /* NOTE: FusionCore/Unicore init is performed in BigMAC init */
    if (PHY_EXTERNAL_MODE(unit, port)) {
        if (IS_HG_PORT(unit, port)) {
            uint16 value;
            
            /* Enable XgxsNoCC if the HiGig port has external PHY.
             * XgxsNoCC enables in-band link signaling, per the 802.3ae
             * standard (|Q| ordered sets).  XgxsNoLssQNoCC is legacy 
             * HiG mode, which does not support in-band link signaling.
             * Current XAUI cores support both 802.3ae compliant 
             * signaling, as well as backwards compatibility with 
             * prior (legacy) protocols.
             */
            SOC_IF_ERROR_RETURN
                (READ_PHYXGXS_BLK0_CTRLr(unit, pc, &value));
            value &= ~(0x0f00);
            value |= (1 << 8);
            SOC_IF_ERROR_RETURN
                (WRITE_PHYXGXS_BLK0_CTRLr(unit, pc, value));
        }
    }

    /* clear loopback (just in case) */
    SOC_IF_ERROR_RETURN
        (phy_xgxs1_lb_set(unit, port, FALSE));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs1_init: u=%d p=%d\n"),
              unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs1_link_get
 * Purpose:
 *      Get layer2 connection status.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs1_link_get(int unit, soc_port_t port, int *link)
{
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    if (IS_LMD_ENABLED_PORT(unit, port)) {
        uint16      o_misctrl1;
        uint16      n_misctrl1;
        uint16      sync_stat;
        phy_ctrl_t *pc;

        pc = INT_PHY_SW_STATE(unit, port);

        SOC_IF_ERROR_RETURN         /* Get mdio reg mapping mode */
            (READ_PHYXGXS_BLK0_MISC_CTRL1r(unit, pc, &o_misctrl1));
        n_misctrl1 = 0x0000;
        if (n_misctrl1 != o_misctrl1) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHYXGXS_BLK0_MISC_CTRL1r(unit, pc, n_misctrl1));
        }
        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS_IEEE1_LANE_STATr(unit, pc, &sync_stat));
    
        if (n_misctrl1 != o_misctrl1) {  /* Restore PMA/PMD mdio reg mapping */
            SOC_IF_ERROR_RETURN
                (WRITE_PHYXGXS_BLK0_MISC_CTRL1r(unit, pc, o_misctrl1));
        }
     
        *link = (sync_stat & 1) ? TRUE : FALSE;
    } else {
        uint64  xgxs_stat;

        SOC_IF_ERROR_RETURN
            (READ_MAC_XGXS_STATr(unit, port, &xgxs_stat)); 
        *link = soc_reg64_field32_get
                        (unit, MAC_XGXS_STATr, xgxs_stat, 
                         LINKf) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs1_enable_set
 * Purpose:
 *      Enable/Disable phy 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs1_enable_set(int unit, soc_port_t port, int enable)
{
    uint16      disable_tx_xe;
    uint16      disable_tx_hg;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    /* PMA/PMD mapping are different between HG and XE ports.
     * Therefore, we need two different settings for HG and XE ports.
     */
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
        disable_tx_xe = 0; /* Clear Tx disable bit for XE*/
        disable_tx_hg = 0; /* Clear Tx disable bit for HG*/
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
        disable_tx_xe = (1 << 0);  /* Set disable Global PMD TX bit for XE */
        disable_tx_hg = (1 << 11); /* Set disable Global PMD TX bit for HG */
    }

    if (IS_XE_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHYXGXS_XAUI_TX_DISABLEr(unit, pc, disable_tx_xe, 1 << 0));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHYXGXS_BLK0_MISC_CTRL1r(unit, pc, disable_tx_hg, 
                                             (1 << 11)));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs1_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - current link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs1_speed_get(int unit, soc_port_t port, int *speed)
{
    /* Default speed */
    *speed = 10000;
    if (IS_LMD_ENABLED_PORT(unit, port)) {
        *speed >>= 2;   /* Div by 4 */
    }

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs1_lb_set
 * Purpose:
 *      Put XGXS/FusionCore in PHY loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs1_lb_set(int unit, soc_port_t port, int enable)
{
    int         rv;
    phy_ctrl_t *pc;
    uint16      lb_bit;

    rv = SOC_E_NONE;

    pc = INT_PHY_SW_STATE(unit, port);

    if (IS_LMD_ENABLED_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHYXGXS_BLK1_LANE_CTRL2r(unit, pc, 
                                             (enable) ? 0xFF0F : 0xFF00)); 
        return SOC_E_NONE;
    }

    /*
     * control RX signal detect, so that a cable is not needed for loopback
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS_RXALL_CTRLr(unit, pc, (enable) ? 0x0010 : 0, 0x0010));

    /*
     * control the actual loopback bit
     */
    lb_bit = XGXS_IEEE_CTRL_LB_ENA;
    if (IS_XE_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_xgxs_v5)) {
            lb_bit = 1;
        }
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS_XAUI_MII_CTRLr(unit, pc,
                                        (enable) ? lb_bit : 0, lb_bit)); 

    return rv;
}

/*
 * Function:
 *      phy_xgxs1_lb_get
 * Purpose:
 *      Get XGXS/FusionCore PHY loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs1_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      reg_val; 
    uint16      lb_bit;
    phy_ctrl_t *pc;
 
    pc = INT_PHY_SW_STATE(unit, port);

    if (IS_LMD_ENABLED_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN             /* Get Loopback status*/
            (READ_PHYXGXS_BLK1_LANE_CTRL2r(unit, pc, &reg_val));

        *enable = ((reg_val & 0x000F) == 0x000f) ? TRUE : FALSE;
        return SOC_E_NONE;
    }

    lb_bit = XGXS_IEEE_CTRL_LB_ENA;
    if (IS_XE_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_xgxs_v5) ||
            soc_feature(unit, soc_feature_xgxs_v6)) {
            lb_bit = 1;
        }
    }
 
    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS_XAUI_MII_CTRLr(unit, pc, &reg_val));

    *enable = (reg_val & lb_bit) ? TRUE : FALSE;

    return SOC_E_NONE;
}

STATIC int
phy_xgxs1_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    if (NULL == mode) {
        return SOC_E_PARAM;
    }

    *mode =  SOC_PM_MII | SOC_PM_XGMII | SOC_PM_LB_PHY |
             SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM;

    if (IS_LMD_ENABLED_PORT(unit, port)) {
        *mode |= SOC_PM_2500MB_FD;
    } else {
        *mode |= SOC_PM_10GB_FD;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs1_control_tx_driver_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
    uint16  data;             /* Temporary holder of reg value to be written */
    uint16  mask;             /* Bit mask of reg value to be updated */

    if (value > 15) {
        return SOC_E_PARAM;
    }

    data = mask = 0;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         data = _shr_bit_rev8((uint8)value) << 8;
         mask = 0xF000;
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         data = _shr_bit_rev8((uint8)value) << 4;
         mask = 0x0F00;
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         data = _shr_bit_rev8((uint8)value);
         mask = 0x00F0;
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    /* Update preemphasis/driver/pre-driver current */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS_TXALL_DRIVERr(unit, pc, data, mask));

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs1_control_tx_driver_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    uint16  data16;           /* Temporary holder of 16 bit reg value */
    uint8   data8;            /* Temporary holder of 8 bit reg value */

    /* Read preemphasis/driver/pre-driver current */
    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS_TXALL_DRIVERr(unit, pc, &data16));

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
         data8 = (uint8)((data16 & 0xF000) >> 8);
         *value = _shr_bit_rev8(data8);
         break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
         data8 = (uint8)((data16 & 0x0F00) >> 4);
         *value = _shr_bit_rev8(data8);
         break;
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
         data8 = (uint8)(data16 & 0x00F0);
         *value = _shr_bit_rev8(data8);
         break;
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs1_control_set
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
phy_xgxs1_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t *pc;
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }
   
    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_xgxs1_control_tx_driver_set(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        /* No support for force link tx */
        rv = SOC_E_UNAVAIL;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_xgxs1_control_get
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
phy_xgxs1_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t *pc;
 
    if (NULL == value) {
        return SOC_E_PARAM;
    }
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }
   
    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_xgxs1_control_tx_driver_get(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        /* No support for force link tx */
        rv = SOC_E_UNAVAIL;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }

    return rv;
}

STATIC int
phy_xgxs1_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
    case phyEventDuplex:
    case phyEventSpeed:
    case phyEventStop:
    case phyEventResume:
    case phyEventAutoneg:
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

#ifdef XGXS1_QUICK_LINK_FLAPPING_FIX
STATIC int
phy_xgxs1_linkdown(int unit, soc_port_t port)
{
    /* Workaround for Pass 0 silicon */
    return soc_fusioncore_reset(unit, port, -1);
}
#else /* no flapping */

#define phy_xgxs1_linkdown       NULL

#endif  /* XGXS1_QUICK_LINK_FLAPPING_FIX */

/*
 * Variable:
 *      phy_xgxs1_drv
 * Purpose:
 *      Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_xgxs1_hg = {
    "XGXS 10G PHY Driver",
    phy_xgxs1_init,           /* Init */
    phy_null_reset,           /* Reset (dummy) */
    phy_xgxs1_link_get,       /* Link get   */
    phy_xgxs1_enable_set,     /* Enable set */
    phy_null_enable_get,      /* Enable get */
    phy_null_set,             /* Duplex set */
    phy_null_one_get,         /* Duplex get */
    phy_null_speed_set,       /* Speed set  */
    phy_xgxs1_speed_get,      /* Speed get  */
    phy_null_set,             /* Master set */
    phy_null_zero_get,        /* Master get */
    phy_null_set,             /* ANA set */
    phy_null_an_get,          /* ANA get */
    phy_null_mode_set,        /* Local Advert set */
    phy_null_mode_get,        /* Local Advert get */
    phy_null_mode_get,        /* Remote Advert get */
    phy_xgxs1_lb_set,         /* PHY loopback set */
    phy_xgxs1_lb_get,         /* PHY loopback set */
    phy_null_interface_set,   /* IO Interface set */
    phy_null_interface_get,   /* IO Interface get */
    phy_xgxs1_ability_get,    /* PHY abilities mask */
    NULL,
    phy_xgxs1_linkdown,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                     /* phy_cable_diag  */
    NULL,                     /* phy_link_change */
    phy_xgxs1_control_set,
    phy_xgxs1_control_get,
    phy_xgxs_reg_read,
    phy_xgxs_reg_write, 
    phy_xgxs_reg_modify,
    phy_xgxs1_notify                      /* phy_notify */ 
};

#endif /* INCLUDE_PHY_XGXS1 */

typedef int _phy_xgxs1_not_empty; /* Make ISO compilers happy. */
