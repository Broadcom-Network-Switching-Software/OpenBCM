/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgxs.c
 * Purpose:     Broadcom 10/100/1000/2500/10000/12000/13000/16000 SerDes 
 *              (SerDes-XGXS5/Unicore)
 */

#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_XGXS5)
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
#include "xgxs.h"
#include "xgxs5.h"

STATIC int phy_xgxs5_lb_set(int unit, soc_port_t port, int enable);

/*
 *      Configure specified port for Independent channel mode operation
 */
STATIC int
_xgxs5_independent_channel_mode_set(int unit, phy_ctrl_t *pc)
{
    SOC_IF_ERROR_RETURN         /* Independent Channdel Mode */
        (WRITE_PHYXGXS5_BLK0_CTRLr(unit, pc, 0x261f));

    /* Set all lanes to full rate mode */
    SOC_IF_ERROR_RETURN         /* Div  /1 mode select */
        (WRITE_PHYXGXS5_BLK1_LANE_CTRL1r(unit, pc, 0xffff));

    SOC_IF_ERROR_RETURN         /* Set Comma detect and 8/B/10B ON */
        (WRITE_PHYXGXS5_BLK1_LANE_CTRL2r(unit, pc, 0xff00));

    SOC_IF_ERROR_RETURN         /* Set 1G link enable & comma_adj_sync_sel */
        (WRITE_PHYXGXS5_RXALL_CTRL_PCIEr(unit, pc, 0x9130));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs5_init
 * Purpose:     
 *      Initialize xgxs phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs5_init(int unit, soc_port_t port)
{
    int            preemph, idriver, pdriver;
    int            locked;
    uint16         pll_stat;
    phy_ctrl_t    *pc;
    soc_timeout_t  to; 

    pc = INT_PHY_SW_STATE(unit, port);

    if (PHY_SINGLE_LANE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_xgxs5_independent_channel_mode_set(unit, pc));
    }

    /* Power on the Core */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS5_BLK1_LANE_TESTr(unit, pc, (1 << 10), (1 << 10)));

    /* Disable PLL State Machine */
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS5_PLL_CTRLr(unit, pc, 0x5006));

    /* Turn off slowdn_xor */
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS5_PLL_CAP_CTRLr(unit, pc, 0x0000));

    if (IS_HG_PORT(unit,port)) {
        if (!PHY_SINGLE_LANE_MODE(unit, port)) {
            /*
             * Do this only for regular 10G/12G mode.
             * Independent Lane mode is configured above.
             */
            SOC_IF_ERROR_RETURN         /* Disable LssQ */
                (WRITE_PHYXGXS5_BLK0_CTRLr(unit, pc, 0x292f));
        }
        SOC_IF_ERROR_RETURN         /* Enable DTE mdio reg mapping */
            (WRITE_PHYXGXS5_BLK0_MISC_CTRL1r(unit, pc, 0x0000));
        preemph = 0x8;
        idriver = 0x7;
        pdriver = 0x9;
    } else {
        SOC_IF_ERROR_RETURN         /* Enable PMA/PMD mdio reg mapping */
            (WRITE_PHYXGXS5_BLK0_MISC_CTRL1r(unit, pc, 0x0200)); 
        preemph = 0xa;
        idriver = 0xf;
        pdriver = 0xf;
    }

    /* Enable PLL state machine */
    SOC_IF_ERROR_RETURN             
        (WRITE_PHYXGXS5_PLL_CTRLr(unit, pc, 0xf01e));
 
    /*
     * Transform CX4 pin out on the board to HG pinout
     */
    if (soc_property_port_get(unit, port, spn_CX4_TO_HIGIG, FALSE)) {
        /* Swap TX lanes */
        SOC_IF_ERROR_RETURN         /* Enable TX Lane swap */
            (WRITE_PHYXGXS5_BLK2_TX_LANE_SWAPr(unit, pc, 0x80e4));
        /* Flip TX Lane polarity */
        SOC_IF_ERROR_RETURN         /* Flip TX Lane polarity */
            (WRITE_PHYXGXS5_TXALL_ACTRL0r(unit, pc, 0x1020));
    } else {
        /* If CX4 to HG conversion is enabled, do not allow individual lane
         * swapping.
         */
        uint16 lane_map, i;
        uint16 lane, hw_map, chk_map;

        /* Update RX lane map */
        lane_map = soc_property_port_get(unit, port,
                                   spn_XGXS_RX_LANE_MAP, 0x0123) & 0xffff;

        if (lane_map != 0x0123) {
            hw_map  = 0;
            chk_map = 0;
            for (i = 0; i < 4; i++) {
                lane     = (lane_map >> (i * 4)) & 0xf;
                hw_map  |= lane << (i * 2);
                chk_map |= 1 << lane;
            }
            if (chk_map == 0xf) {
                SOC_IF_ERROR_RETURN     /* Enable RX Lane swap */
                   (MODIFY_PHYXGXS5_BLK2_RX_LANE_SWAPr(unit, pc,
                                                    0x8000 | hw_map, 0x80ff));
            } else {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "unit %d port %s: Invalid RX lane map 0x%04x.\n"),
                           unit, SOC_PORT_NAME(unit, port), lane_map));
            }
        }

        /* Update TX lane map */
        lane_map = soc_property_port_get(unit, port,
                                  spn_XGXS_TX_LANE_MAP, 0x0123) & 0xffff;

        if (lane_map != 0x0123) {
            hw_map  = 0;
            chk_map = 0;
            for (i = 0; i < 4; i++) {
                lane     = (lane_map >> (i * 4)) & 0xf;
                hw_map  |= lane << (i * 2);
                chk_map |= 1 << lane;
            }
            if (chk_map == 0xf) {
                SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
                   (MODIFY_PHYXGXS5_BLK2_TX_LANE_SWAPr(unit, pc,
                                                    0x8000 | hw_map, 0x80ff));
            } else {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "unit %d port %s: Invalid TX lane map 0x%04x.\n"),
                           unit, SOC_PORT_NAME(unit, port), lane_map));
            }
        }
    }

    /*
     * Flip TX lane polarity
     */
    if (soc_property_port_get(unit, port, "polarity_swap", FALSE)) {
        /* Flip TX Lane polarity */
        SOC_IF_ERROR_RETURN         /* Flip TX Lane polarity */
            (WRITE_PHYXGXS5_TXALL_ACTRL0r(unit, pc, 0x1020));
    }

    /* Allow config setting to override preemphasis, driver current, 
     * and pre-driver current.
     */
    preemph = soc_property_port_get(unit, port,
                                    spn_XGXS_PREEMPHASIS, preemph);
    preemph = _shr_bit_rev8(preemph) >> 4;
    idriver = soc_property_port_get(unit, port,
                                    spn_XGXS_DRIVER_CURRENT, idriver);
    idriver = _shr_bit_rev8(idriver) >> 4;
    pdriver = soc_property_port_get(unit, port,
                                    spn_XGXS_PRE_DRIVER_CURRENT, pdriver);
    pdriver = _shr_bit_rev8(pdriver) >> 4;

    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS5_TXALL_DRIVERr(unit, pc, ((preemph & 0xf) << 12) |
                                                ((idriver & 0xf) << 8) |
                                                    ((pdriver & 0xf) << 4)));

    /* Wait up to 0.1 sec for TX PLL lock. */
    soc_timeout_init(&to, 100000, 0);

    locked = 0;
    while (!soc_timeout_check(&to)) {
        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS5_PLL_STATr(unit, pc, &pll_stat));
        /* Check pllSeqDone, freqDone, capDone, and ampDone */
        locked = ((pll_stat & (0xf000)) == 0xf000);
        if (locked) {
            break;
        }
    }

    if (!locked) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "unit %d port %s: XGXS PLL did not lock PLL_STAT %04x\n"),
                   unit, SOC_PORT_NAME(unit, port), pll_stat));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "unit %d port %s: XGXS PLL locked in %d usec\n"),
                     unit, SOC_PORT_NAME(unit, port),
                     soc_timeout_elapsed(&to)));
    }

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
                (READ_PHYXGXS5_BLK0_CTRLr(unit, pc, &value));
            value &= ~(0x0f00);
            value |= (1 << 8);
            SOC_IF_ERROR_RETURN
                (WRITE_PHYXGXS5_BLK0_CTRLr(unit, pc, value));
        }
    }

    /* clear loopback (just in case) */
    SOC_IF_ERROR_RETURN
        (phy_xgxs5_lb_set(unit, port, FALSE));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs5_init: u=%d p=%d\n"),
              unit, port));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs5_link_get
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
phy_xgxs5_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc;

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    pc = INT_PHY_SW_STATE(unit, port);

    if (PHY_SINGLE_LANE_MODE(unit, port)) {
        uint16      o_misctrl1;
        uint16      n_misctrl1;
        uint16      sync_stat;

        SOC_IF_ERROR_RETURN         /* Get mdio reg mapping mode */
            (READ_PHYXGXS5_BLK0_MISC_CTRL1r(unit, pc, &o_misctrl1));
        n_misctrl1 = 0x0000;
        if (n_misctrl1 != o_misctrl1) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHYXGXS5_BLK0_MISC_CTRL1r(unit, pc, n_misctrl1));
        }
        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS5_IEEE1_LANE_STATr(unit, pc, &sync_stat));
    
        if (n_misctrl1 != o_misctrl1) {  /* Restore PMA/PMD mdio reg mapping */
            SOC_IF_ERROR_RETURN
                (WRITE_PHYXGXS5_BLK0_MISC_CTRL1r(unit, pc, o_misctrl1));
        }
     
        *link = (sync_stat & 1) ? TRUE : FALSE;
    } else {
        uint16  mii_stat;

        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS5_XAUI_MII_STATr(unit, pc, &mii_stat));
        *link = (mii_stat & MII_STAT_LA) ? TRUE : FALSE; 
    }

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs5_enable_set(int unit, soc_port_t port, int enable)
{
    uint16      disable_tx_xe;
    uint16      disable_tx_hg;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    /* PMA/PMD mapping are different between HG and XE ports.
     * Therefore, we need two different settings for HG and XE ports.
     */
    if (enable) {
        disable_tx_xe = 0; /* Clear Tx disable bit for XE*/
        disable_tx_hg = 0; /* Clear Tx disable bit for HG*/
    } else {
        disable_tx_xe = (1 << 0);  /* Set disable Global PMD TX bit for XE */
        disable_tx_hg = (1 << 11); /* Set disable Global PMD TX bit for HG */
    }

    if (IS_XE_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHYXGXS5_XAUI_TX_DISABLEr(unit, pc, disable_tx_xe, 1 << 0));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHYXGXS5_BLK0_MISC_CTRL1r(unit, pc, disable_tx_hg, 
                                             (1 << 11)));
    }

    /* hold/release rxSeqStart to disable/enable the Rx */ 
    SOC_IF_ERROR_RETURN 
        (MODIFY_PHYXGXS5_RXALL_CTRLr(unit, pc, 
                 (enable) ? 0 : (1 << 15), (1 << 15))); 

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs5_enable_set
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
phy_xgxs5_enable_set(int unit, soc_port_t port, int enable)
{
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }
    return _phy_xgxs5_enable_set(unit,port,enable);
}
/*
 * Function:
 *      phy_xgxs5_speed_get
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
phy_xgxs5_speed_get(int unit, soc_port_t port, int *speed)
{
    /* Default speed */
    *speed = 10000;

    if (PHY_SINGLE_LANE_MODE(unit, port)) {
        *speed >>= 2;   /* Div by 4 */
    }

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs5_lb_set
 * Purpose:
 *      Put XGXS5/FusionCore in PHY loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs5_lb_set(int unit, soc_port_t port, int enable)
{
    int         rv;
    phy_ctrl_t *pc;
    uint16      lb_bit;

    rv = SOC_E_NONE;

    pc = INT_PHY_SW_STATE(unit, port);

    if (PHY_SINGLE_LANE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHYXGXS5_BLK1_LANE_CTRL2r(unit, pc, 
                                             (enable) ? 0xFF0F : 0xFF00)); 
        return SOC_E_NONE;
    }

    /*
     * control RX signal detect, so that a cable is not needed for loopback
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS5_RXALL_CTRLr(unit, pc, (enable) ? 0x0010 : 0, 0x0010));

    /*
     * control the actual loopback bit
     */
    lb_bit = XGXS5_IEEE_CTRL_LB_ENA;
    if (IS_XE_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_xgxs_v5)) {
            lb_bit = 1;
        }
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS5_XAUI_MII_CTRLr(unit, pc,
                                        (enable) ? lb_bit : 0, lb_bit)); 

    return rv;
}

/*
 * Function:
 *      phy_xgxs5_lb_get
 * Purpose:
 *      Get XGXS5/FusionCore PHY loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs5_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      reg_val; 
    uint16      lb_bit;
    phy_ctrl_t *pc;
 
    pc = INT_PHY_SW_STATE(unit, port);

    if (PHY_SINGLE_LANE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN             /* Get Loopback status*/
            (READ_PHYXGXS5_BLK1_LANE_CTRL2r(unit, pc, &reg_val));

        *enable = ((reg_val & 0x000F) == 0x000f) ? TRUE : FALSE;
        return SOC_E_NONE;
    }

    lb_bit = XGXS5_IEEE_CTRL_LB_ENA;
    if (IS_XE_PORT(unit, port)) {
         lb_bit = 1;
    }
 
    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS5_XAUI_MII_CTRLr(unit, pc, &reg_val));

    *enable = (reg_val & lb_bit) ? TRUE : FALSE;

    return SOC_E_NONE;
}

STATIC int
phy_xgxs5_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    *mode =  SOC_PM_MII | SOC_PM_XGMII | SOC_PM_LB_PHY |
             SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM;

    if (PHY_SINGLE_LANE_MODE(unit, port)) {
        *mode |= SOC_PM_2500MB_FD | SOC_PM_3000MB_FD;
    } else {
        *mode |= SOC_PM_10GB_FD | SOC_PM_12GB_FD;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs5_control_tx_driver_set(int unit, phy_ctrl_t *pc,
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
        (MODIFY_PHYXGXS5_TXALL_DRIVERr(unit, pc, data, mask));

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs5_control_tx_driver_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    uint16  data16;           /* Temporary holder of 16 bit reg value */
    uint8   data8;            /* Temporary holder of 8 bit reg value */

    /* Read preemphasis/driver/pre-driver current */
    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS5_TXALL_DRIVERr(unit, pc, &data16));

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
 *      phy_xgxs5_control_set
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
phy_xgxs5_control_set(int unit, soc_port_t port,
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
        rv = _phy_xgxs5_control_tx_driver_set(unit, pc, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_xgxs5_control_get
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
phy_xgxs5_control_get(int unit, soc_port_t port,
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
        rv = _phy_xgxs5_control_tx_driver_get(unit, pc, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }

    return rv;
}

STATIC int
_phy_xgxs5_stop(int unit, soc_port_t port)
{
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
                         "phy_xgxs5_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));


    return _phy_xgxs5_enable_set(unit,port,stop?FALSE:TRUE);
}

/*
 * Function:
 *      _phy_xgxs5_notify_stop
 * Purpose:
 *      Add a reason to put serdesxgxs5 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_xgxs5_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;
    return _phy_xgxs5_stop(unit, port);
}

/*
 * Function:
 *      _phy_xgxs5_notify_resume
 * Purpose:
 *      Remove a reason to put serdesxgxs5 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_xgxs5_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;
    return _phy_xgxs5_stop(unit, port);
}

STATIC int
phy_xgxs5_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        break;
    case phyEventDuplex:
        break;
    case phyEventSpeed:
        break;
    case phyEventStop:
        rv = (_phy_xgxs5_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_xgxs5_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Variable:
 *      phy_xgxs5_drv
 * Purpose:
 *      Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_xgxs5_hg = {
    "XGXS5 10G PHY Driver",
    phy_xgxs5_init,           /* Init */
    phy_null_reset,           /* Reset (dummy) */
    phy_xgxs5_link_get,       /* Link get   */
    phy_xgxs5_enable_set,     /* Enable set */
    phy_null_enable_get,      /* Enable get */
    phy_null_set,             /* Duplex set */
    phy_null_one_get,         /* Duplex get */
    phy_null_speed_set,       /* Speed set  */
    phy_xgxs5_speed_get,      /* Speed get  */
    phy_null_set,             /* Master set */
    phy_null_zero_get,        /* Master get */
    phy_null_set,             /* ANA set */
    phy_null_an_get,          /* ANA get */
    phy_null_mode_set,        /* Local Advert set */
    phy_null_mode_get,        /* Local Advert get */
    phy_null_mode_get,        /* Remote Advert get */
    phy_xgxs5_lb_set,         /* PHY loopback set */
    phy_xgxs5_lb_get,         /* PHY loopback set */
    phy_null_interface_set,   /* IO Interface set */
    phy_null_interface_get,   /* IO Interface get */
    phy_xgxs5_ability_get,    /* PHY abilities mask */
    NULL,                     /* Link up event */
    NULL,                     /* Link down event */
    phy_null_mdix_set,       
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,                     /* Medium config set */
    NULL,                     /* Medium config get */
    phy_null_medium_get,
    NULL,                     /* phy_cable_diag  */
    NULL,                     /* phy_link_change */
    phy_xgxs5_control_set,
    phy_xgxs5_control_get,
    phy_xgxs_reg_read,
    phy_xgxs_reg_write, 
    phy_xgxs_reg_modify, 
    phy_xgxs5_notify          /* Notify event */
};

#else /* BCM_XGS5_SUPPORT */
typedef int _phy_xgxs5_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_XGS5_SUPPORT */

