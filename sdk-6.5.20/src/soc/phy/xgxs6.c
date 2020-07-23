/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgxs6.c
 * Purpose:     Broadcom 10/100/1000/2500/10000/12000/13000/16000 SerDes 
 *              (SerDes-XGXS6/Unicore)
 */
#include "phydefs.h"      /* Must include before other phy related includes */ 
#if defined(INCLUDE_PHY_XGXS6)
#include "phyconfig.h"     /* Must include before other phy related includes */
#include <sal/types.h>

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
#include "xgxs6.h"

#define XGXS6_PLL_WAIT          250000

STATIC int phy_xgxs6_lb_set(int unit, soc_port_t port, int enable);
STATIC int phy_xgxs6_an_set(int unit, soc_port_t port, int an);
STATIC int phy_xgxs6_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int phy_xgxs6_speed_get(int unit, soc_port_t port, int *speed);

STATIC int
_phy_xgxs6_pll_lock_wait(int unit, soc_port_t port)
{
    uint16           data16;
    phy_ctrl_t      *pc;
    soc_timeout_t    to;
    int              rv;

    pc = INT_PHY_SW_STATE(unit, port);
    soc_timeout_init(&to, XGXS6_PLL_WAIT, 0);
    do {
        rv = READ_PHYXGXS6_BLK0_STATr(unit, pc, &data16);
        if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK) == 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "XGXS6 : TXPLL did not lock: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_TIMEOUT);
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs6_init
 * Purpose:     
 *      Initialize xgxs6 phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs6_init(int unit, soc_port_t port)
{
    int                 rv;               /* Return value */
    uint16              mii_ctrl;
    phy_ctrl_t         *pc;

    /* NOTE: FusionCore/Unicore init is performed in BigMAC init */

    /* clear loopback (just in case) */
    rv = phy_xgxs6_lb_set(unit, port, FALSE);

    /* Initialize the software state */
    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_SERDES_MII_CTRLr(unit, pc, &mii_ctrl));
    pc->fiber.autoneg_enable = ((mii_ctrl & MII_CTRL_AE) != 0);

    /* set elasticity fifo size to 13.5k to support 12k jumbo pkt size*/
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS6_SERDES_DIGITAL_1000X_CTRL3r(unit,pc, 0x4, 0x6));

     /* disable in-band MDIO. PHY-443 */ 
    SOC_IF_ERROR_RETURN 
        (PHYXGXS6_REG_MODIFY(unit, pc, 0x00, 0x8110, 0x11, 1 << 3, 1 << 3)); 

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs6_init: u=%d p=%d rv=%d\n"),
              unit, port, rv));

    return rv;
}

/*
 * Function:
 *      phy_xgxs6_link_get
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
phy_xgxs6_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc;

    if (PHY_DISABLED_MODE(unit, port)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    pc = INT_PHY_SW_STATE(unit, port);
    if (IS_LMD_ENABLED_PORT(unit, port)) {
        uint16      o_misctrl1;
        uint16      n_misctrl1;
        uint16      sync_stat;


        SOC_IF_ERROR_RETURN         /* Get mdio reg mapping mode */
            (READ_PHYXGXS6_BLK0_MISC_CTRL1r(unit, pc, &o_misctrl1));
        n_misctrl1 = 0x0000;
        if (n_misctrl1 != o_misctrl1) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHYXGXS6_BLK0_MISC_CTRL1r(unit, pc, n_misctrl1));
        }
        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS6_IEEE1_LANE_STATr(unit, pc, &sync_stat));
    
        if (n_misctrl1 != o_misctrl1) {  /* Restore PMA/PMD mdio reg mapping */
            SOC_IF_ERROR_RETURN
                (WRITE_PHYXGXS6_BLK0_MISC_CTRL1r(unit, pc, o_misctrl1));
        }
     
        *link = (sync_stat & 1) ? TRUE : FALSE;
    } else {
        uint16  mii_serdes_stat;
        uint16  mii_xaui_stat;
        uint16  xgxs_stat1;
        int     an_done;

        /* Link status on both SERDES_MII_STAT and XAUI_MII_STAT registers
         * are latched low.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS6_SERDES_MII_STATr(unit, pc, &mii_serdes_stat));
        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS6_XAUI_MII_STATr(unit, pc, &mii_xaui_stat));

        *link = ((mii_serdes_stat & MII_STAT_LA) != 0) ||
                ((mii_xaui_stat & MII_STAT_LA) != 0);

        if (pc->fiber.autoneg_enable) {
            SOC_IF_ERROR_RETURN
                (READ_PHYXGXS6_GPSTAT_XGXS_STAT1r(unit, pc, &xgxs_stat1));
            /* If autoneg is enabled, check for autoneg done. */
            an_done = ((xgxs_stat1 & XGXS_STAT1_AUTONEG_DONE) != 0);

            *link = (*link) && (an_done);
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs6_enable_set(int unit, soc_port_t port, int enable)
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
            (MODIFY_PHYXGXS6_XAUI_TX_DISABLEr(unit, pc, disable_tx_xe, 1 << 0));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHYXGXS6_BLK0_MISC_CTRL1r(unit, pc, disable_tx_hg, 
                                             (1 << 11)));
    }

    /* hold/release rxSeqStart to disable/enable the Rx */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS6_RXALL_CTRLr(unit, pc,
                 (enable) ? 0 : (1 << 15), (1 << 15)));


    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_xgxs6_enable_set
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
phy_xgxs6_enable_set(int unit, soc_port_t port, int enable)
{
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }
    return _phy_xgxs6_enable_set(unit,port,enable);
}

/*
 * Function:
 *     _phy_xgxs6_speed_set
 * Purpose:
 *     Get current Unicore speed.
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 *     speed - new XGXS speed.
 * Returns:
 *     SOC_E_XXX
 * Notes:
 *      The Start Sequencer bit in the xgxsControl register should
 *      be deasserted before calling this function to guarantee a
 *      glitch-free clock change.
 */
STATIC int
_phy_xgxs6_speed_set(int unit, soc_port_t port, int speed)
{
    int    speed_val;
    uint8  phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    uint16 mii_ctrl, misc1, misc_ctrl;
    uint16 block_select;

    if (IS_HG_PORT(unit, port)) {
        if ((speed != 0) && (speed < 10000)) {
            return SOC_E_PARAM;
        }
        if (SOC_INFO(unit).port_speed_max[port] > 0 &&
            speed > SOC_INFO(unit).port_speed_max[port]) {
            return SOC_E_PARAM;
        }
    } else if (IS_XE_PORT(unit, port)) {
        if ((speed != 0) && ((speed < 1000) || (speed > 10000))) {
            return SOC_E_PARAM;
        }
    } else {
        if ((speed < 0) || (speed > 2500)) {
            return SOC_E_PARAM;
        }
    }

    switch (speed) {
    case 13000:
        speed_val = 7;
        break;
    case 12500:
        speed_val = 6;
        break;
    case 12000:
        speed_val = 5;
        break;
    case 10000:
        speed_val = 4; /* 10G CX4 */
        if (IS_HG_PORT(unit, port) && 
            soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE) == FALSE) {
            speed_val = 3; /* 10G HiG */
        }
        break;
    case 2500:
        speed_val = 0;
        break;
    case 1000:
    case 100:
    case 10:
        speed_val = -1;
        break;
    case 0:
        return SOC_E_NONE;
    case 16000:
                                case 15000:
    case 6000:
    case 5000:
    default:
        return SOC_E_PARAM;
    }

    /* Save original block select mapping */
    SOC_IF_ERROR_RETURN
        (soc_miim_read(unit, phy_addr, 0x1f, &block_select));

    /* Select block DIGITAL (0x30) */
    SOC_IF_ERROR_RETURN
        (soc_miim_write(unit, phy_addr, 0x1f, 0x0300));

    /* Set speed in Misc1 register */
    SOC_IF_ERROR_RETURN
        (soc_miim_read(unit, phy_addr, 0x18, &misc1));
    misc1 &= ~0x001f;
    if (speed_val >= 0) {
        misc1 |= 0x0010 | speed_val;
    }
    SOC_IF_ERROR_RETURN
        (soc_miim_write(unit, phy_addr, 0x18, misc1));

    /* Select block BLOCK0 */
    SOC_IF_ERROR_RETURN
        (soc_miim_write(unit, phy_addr, 0x1f, 0x0000));

    /* Map XAUI or SerDes IEEE registers */
    SOC_IF_ERROR_RETURN
        (soc_miim_read(unit, phy_addr, 0x1e, &misc_ctrl));
    /* Disable dynamic IEEE address space mapping and
     * map Serdes IEEE address space.
     */
    misc_ctrl &= ~0x0003;
    if (speed > 2500) {
        misc_ctrl |= 0x0001; /* Select XAUI registers */
    }

    /* Write final IEEE register mapping */
    SOC_IF_ERROR_RETURN
        (soc_miim_write(unit, phy_addr, 0x1e, misc_ctrl));

    /*
     * Speeds of 2.5 Gbps and below must be set in IEEE registers.
     */
    if (speed <= 2500) {
        SOC_IF_ERROR_RETURN
            (soc_miim_read(unit, phy_addr, MII_CTRL_REG, &mii_ctrl));
        mii_ctrl &= ~(MII_CTRL_SS_MASK | MII_CTRL_FS_2500);
        if (speed == 2500) {
            /* Force 2.5 Gbps */
            mii_ctrl |= MII_CTRL_FS_2500;
        } else if (speed == 1000) {
            mii_ctrl |= MII_CTRL_SS_1000;
        } else if (speed == 100) {
            mii_ctrl |= MII_CTRL_SS_100;
        } else {
            mii_ctrl |= MII_CTRL_SS_10;
        }
        SOC_IF_ERROR_RETURN
            (soc_miim_write(unit, phy_addr, MII_CTRL_REG, mii_ctrl));
    }

    /* Restore Original Block select mapping */
    SOC_IF_ERROR_RETURN
        (soc_miim_write(unit, phy_addr, 0x1f, block_select));

    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_xgxs6_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs6_speed_set(int unit, soc_port_t port, int speed)
{
    int         rv;
    int         an;
    int         cur_speed;
    phy_ctrl_t *pc;

    /* If already forced to requested speed, don't change anything */
    SOC_IF_ERROR_RETURN
        (phy_xgxs6_an_get(unit, port, &an, NULL));
    if (!an) {
        SOC_IF_ERROR_RETURN
            (phy_xgxs6_speed_get(unit, port, &cur_speed));
        if (cur_speed == speed) {
            return SOC_E_NONE;
        }
    }

    rv = SOC_E_NONE;
    pc = INT_PHY_SW_STATE(unit, port);

    /* Stop sequencer */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS6_BLK0_CTRLr(unit, pc, 0, 0x2000));

    /* Disable autoneg if needed */
    if (an) {
        rv = (phy_xgxs6_an_set(unit, port, FALSE));
    }

    if (SOC_SUCCESS(rv)) {
        rv = _phy_xgxs6_speed_set(unit, port, speed);
    }

    /* Need to restart sequencer even if speed_set failed */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS6_BLK0_CTRLr(unit, pc, 0x2000, 0x2000));

    /* Wait for Tx PLL lock */
    if (SOC_SUCCESS(rv)) {
        rv =  _phy_xgxs6_pll_lock_wait(unit, port);
    }

    return rv;
}

/*
 * Function:
 *      phy_xgxs6_speed_get
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
phy_xgxs6_speed_get(int unit, soc_port_t port, int *speed)
{
    uint16      xgxs_stat;
    int         an;
    int         an_done;
    phy_ctrl_t *pc;

    /* Default speed */
    *speed = 10000;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_xgxs6_an_get(unit, port, &an, &an_done));
    if (an && !an_done) {
        *speed = 0;
        return (SOC_E_NONE);
    }

    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_GPSTAT_XGXS_STAT1r(unit, pc, &xgxs_stat));
    switch (xgxs_stat & XGXS_STAT1_ACTUAL_SPEED_MASK) {
    case XGXS_STAT1_ACTUAL_SPEED_10:       *speed = 10; break;
    case XGXS_STAT1_ACTUAL_SPEED_100:      *speed = 100; break;
    case XGXS_STAT1_ACTUAL_SPEED_1000:     *speed = 1000; break;
    case XGXS_STAT1_ACTUAL_SPEED_2500:     *speed = 2500; break;
    case XGXS_STAT1_ACTUAL_SPEED_5000:     *speed = 5000; break;
    case XGXS_STAT1_ACTUAL_SPEED_6000:     *speed = 6000; break;
    case XGXS_STAT1_ACTUAL_SPEED_10000:    /* fall through */
    case XGXS_STAT1_ACTUAL_SPEED_10000CX4: *speed = 10000; break;
    case XGXS_STAT1_ACTUAL_SPEED_12000:    *speed = 12000; break;
    case XGXS_STAT1_ACTUAL_SPEED_12500:    *speed = 12500; break;
    case XGXS_STAT1_ACTUAL_SPEED_13000:    *speed = 13000; break;
    case XGXS_STAT1_ACTUAL_SPEED_15000:    *speed = 15000; break;
    case XGXS_STAT1_ACTUAL_SPEED_16000:    *speed = 16000; break;
    default:
        return SOC_E_INTERNAL;
    } 

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_xgxs6_an_set
 * Purpose:     
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an   - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX
 */
int
phy_xgxs6_an_set(int unit, soc_port_t port, int an)
{
    int         rv; 
    uint16      mii_ctrl;
    uint16      xgxs_ctrl;
    uint16      mreg_val;
    phy_ctrl_t *pc;

    rv = SOC_E_NONE;
    pc = INT_PHY_SW_STATE(unit, port);

    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_SERDES_DIGITAL_1000X_CTRL1r(unit, pc, &mreg_val));

    mreg_val &= ~(1 << 4);  /* Clear auto-detect */
    if (an) {
        if (soc_property_port_get(unit, port, 
                                  spn_SERDES_AUTOMEDIUM, TRUE)) {
            mreg_val |= (1 << 4);   /* Set auto-detect */
        }
    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS6_SERDES_DIGITAL_1000X_CTRL1r(unit, pc, mreg_val));

    /* Stop sequencer */
    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_BLK0_CTRLr(unit, pc, &xgxs_ctrl));
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS6_BLK0_CTRLr(unit, pc, xgxs_ctrl & ~0x2000));

    /* Autoneg enable bit in SerDes block is for both SerDes 
     * and XAUI core */
    mii_ctrl = (an) ? MII_CTRL_AE | MII_CTRL_RAN : 0;
    rv = MODIFY_PHYXGXS6_SERDES_MII_CTRLr(unit, pc, mii_ctrl, 
                                          MII_CTRL_AE | MII_CTRL_RAN);

    /* Restore sequencer */
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS6_BLK0_CTRLr(unit, pc, xgxs_ctrl));

    if (rv < 0) {
        return rv;
    }

    /* Wait for Tx PLL lock if sequencer was restarted */
    if (xgxs_ctrl & 0x2000) {
        rv = _phy_xgxs6_pll_lock_wait(unit, port);
    }
 
    /* Save autoneg state in software */
    pc->fiber.autoneg_enable = an;

    return rv;
}

/*
 * Function:    
 *      phy_xgxs6_an_get
 * Purpose:     
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:     
 *      SOC_E_XXX
 */

int
phy_xgxs6_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      mii_ctrl;
    uint16      mii_stat;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_SERDES_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_SERDES_MII_STATr(unit, pc, &mii_stat));

    *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;

    if (NULL != an_done) {
        *an_done = (mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;
    }

    return SOC_E_NONE; 
}

/*
 * Function:
 *      phy_xgxs6_adv_local_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_xgxs6_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    uint16      an_adv;
    uint16      speed_10g;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    /*
     * Set advertised duplex (only FD supported).
     */
    an_adv = (mode & SOC_PM_1000MB_FD) ? XGXS6_ANA_FD : 0;

    /*
     * Set advertised pause bits in link code word.
     */
    switch (mode & SOC_PM_PAUSE) {
    case SOC_PM_PAUSE_TX:
        an_adv |= XGXS6_ANA_PAUSE_ASYM;
        break;
    case SOC_PM_PAUSE_RX:
        an_adv |= XGXS6_ANA_PAUSE_SYM | XGXS6_ANA_PAUSE_ASYM;
        break;
    case SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX:
        an_adv |= XGXS6_ANA_PAUSE_SYM;
        break;
    }

    /* Update less than 1G capability */ 
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS6_SERDES_MII_ANAr(unit, pc, an_adv));
  
    /* Over 1G advertising */
    if (IS_HG_PORT(unit, port)) {
        /* For Draco and Hercules, use pre-CX4 signalling */
        speed_10g = (1 << 3);
        if (soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE)) {
            /* Also include 10G CX4 signalling by default */
            speed_10g |= (1 << 4);
        }

        /* Advertise 10G */
        an_adv  = (mode & SOC_PM_10GB) ? speed_10g : 0;
        an_adv |= (mode & SOC_PM_12GB) ? (1 << 5) : 0;
        an_adv |= (mode & SOC_PM_13GB) ? (1 << 7) : 0;
    } else {
        /* Advertise 10G CX4 */
        an_adv  = (mode & SOC_PM_10GB) ? (1 << 4) : 0; 
        an_adv |= (mode & SOC_PM_2500MB) ? (1 << 0) : 0;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS6_OVER1G_UP1r(unit, pc, an_adv));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs6_adv_local_set:unit=%d p=%d mode=0x%08x\n"),
              unit, port, mode));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs6_adv_local_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_xgxs6_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      an_adv;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_OVER1G_UP1r(unit, pc, &an_adv));

    *mode = 0;
    if (IS_HG_PORT(unit, port)) {
        *mode  = (an_adv & ((1 << 3) | (1 << 4))) ? SOC_PM_10GB_FD : 0;
        *mode |= (an_adv & (1 << 5)) ? SOC_PM_12GB_FD : 0;
        *mode |= (an_adv & (1 << 7)) ? SOC_PM_13GB_FD : 0;
    } else {
        *mode  = (an_adv & (1 << 4)) ? SOC_PM_10GB_FD : 0;
        *mode |= (an_adv & (1 << 0)) ? SOC_PM_2500MB_FD : 0;

        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS6_SERDES_MII_ANAr(unit, pc, &an_adv));
      
        *mode |= (an_adv & XGXS6_ANA_FD) ? SOC_PM_1000MB_FD : 0;

        switch (an_adv & (XGXS6_ANA_PAUSE_SYM | XGXS6_ANA_PAUSE_ASYM)) {
            case XGXS6_ANA_PAUSE_SYM:
                *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
                break;
            case XGXS6_ANA_PAUSE_ASYM:
                *mode |= SOC_PM_PAUSE_TX;
                break;
            case XGXS6_ANA_PAUSE_SYM | XGXS6_ANA_PAUSE_ASYM:
                *mode |= SOC_PM_PAUSE_RX;
                break;
        }

    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs6_adv_local_get:unit=%d p=%d mode=0x%08x\n"),
              unit, port, *mode));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs6_adv_remote_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_xgxs6_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      an_adv;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_OVER1G_LP_UP1r(unit, pc, &an_adv));

    *mode = 0;
    if (IS_HG_PORT(unit, port)) {
        *mode = (an_adv & ((1 << 3) | (1 << 4))) ? SOC_PM_10GB_FD : 0;
        *mode |= (an_adv & (1 << 5)) ? SOC_PM_12GB_FD : 0;
        *mode |= (an_adv & (1 << 7)) ? SOC_PM_13GB_FD : 0;
    } else {
        *mode  = (an_adv & (1 << 4)) ? SOC_PM_10GB_FD : 0;
        *mode |= (an_adv & (1 << 0)) ? SOC_PM_2500MB_FD : 0;

        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS6_SERDES_MII_ANPr(unit, pc, &an_adv));
      
        *mode |= (an_adv & XGXS6_ANP_FD) ? SOC_PM_1000MB_FD : 0;

        switch (an_adv & (XGXS6_ANP_PAUSE_SYM | XGXS6_ANP_PAUSE_ASYM)) {
            case XGXS6_ANP_PAUSE_SYM:
                *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
                break;
            case XGXS6_ANP_PAUSE_ASYM:
                *mode |= SOC_PM_PAUSE_TX;
                break;
            case XGXS6_ANP_PAUSE_SYM | XGXS6_ANP_PAUSE_ASYM:
                *mode |= SOC_PM_PAUSE_RX;
                break;
        }
    }
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs6_adv_remote_get:unit=%d p=%d mode=0x%08x\n"),
              unit, port, *mode));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_xgxs6_lb_set
 * Purpose:
 *      Put XGXS6/FusionCore in PHY loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs6_lb_set(int unit, soc_port_t port, int enable)
{
    int         rv;
    phy_ctrl_t *pc;
    uint16      lb_bit;

    rv = SOC_E_NONE;

    pc = INT_PHY_SW_STATE(unit, port);

    if (IS_LMD_ENABLED_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHYXGXS6_BLK1_LANE_CTRL2r(unit, pc, 
                                             (enable) ? 0xFF0F : 0xFF00)); 
        return SOC_E_NONE;
    }

    /*
     * control RX signal detect, so that a cable is not needed for loopback
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS6_RXALL_CTRLr(unit, pc, (enable) ? 0x0010 : 0, 0x0010));

    /*
     * control the actual loopback bit
     */
    lb_bit = XGXS6_IEEE_CTRL_LB_ENA;
    if (IS_XE_PORT(unit, port)) {
        lb_bit = 1;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS6_XAUI_MII_CTRLr(unit, pc,
                                        (enable) ? lb_bit : 0, lb_bit)); 

    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS6_SERDES_MII_CTRLr(unit, pc,
                                         (enable) ? MII_CTRL_LE : 0,
                                         MII_CTRL_LE));
 
    return rv;
}

/*
 * Function:
 *      phy_xgxs6_lb_get
 * Purpose:
 *      Get XGXS6/FusionCore PHY loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs6_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      reg_val; 
    uint16      lb_bit;
    phy_ctrl_t *pc;
 
    pc = INT_PHY_SW_STATE(unit, port);

    if (IS_LMD_ENABLED_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN             /* Get Loopback status*/
            (READ_PHYXGXS6_BLK1_LANE_CTRL2r(unit, pc, &reg_val));

        *enable = ((reg_val & 0x000F) == 0x000f) ? TRUE : FALSE;
        return SOC_E_NONE;
    }

    lb_bit = XGXS6_IEEE_CTRL_LB_ENA;
    if (IS_XE_PORT(unit, port)) {
        lb_bit = 1;
    }
 
    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_XAUI_MII_CTRLr(unit, pc, &reg_val));

    *enable = (reg_val & lb_bit) ? TRUE : FALSE;

    return SOC_E_NONE;
}


STATIC int
phy_xgxs6_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    phy_ctrl_t *pc;
 
    pc = INT_PHY_SW_STATE(unit, port);

    *mode = (SOC_PM_1000MB_FD | SOC_PM_2500MB_FD | 
             SOC_PM_MII | SOC_PM_XGMII | SOC_PM_LB_PHY | SOC_PM_AN |
             SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM);
    switch(pc->speed_max) {
        case 13000:
            *mode |= SOC_PM_13GB_FD;
            /* fall through */
        case 12000:
            *mode |= SOC_PM_12GB_FD;
            /* fall through */
        default:
            *mode |= SOC_PM_10GB_FD;
    }
    
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs6_control_tx_driver_set(int unit, phy_ctrl_t *pc,
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
        (MODIFY_PHYXGXS6_TXALL_DRIVERr(unit, pc, data, mask));

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs6_control_tx_driver_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    uint16  data16;           /* Temporary holder of 16 bit reg value */
    uint8   data8;            /* Temporary holder of 8 bit reg value */

    /* Read preemphasis/driver/pre-driver current */
    SOC_IF_ERROR_RETURN
        (READ_PHYXGXS6_TXALL_DRIVERr(unit, pc, &data16));

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
 *      phy_xgxs6_control_set
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
phy_xgxs6_control_set(int unit, soc_port_t port,
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
        rv = _phy_xgxs6_control_tx_driver_set(unit, pc, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_xgxs6_control_get
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
phy_xgxs6_control_get(int unit, soc_port_t port,
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
        rv = _phy_xgxs6_control_tx_driver_get(unit, pc, type, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }

    return rv;
}

/*
 * Function:
 *      phy_xgxs6_reg_read
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
phy_xgxs6_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint32               reg_flags;
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);

    pc   = INT_PHY_SW_STATE(unit, port);

    reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    SOC_IF_ERROR_RETURN
        (phy_reg_xgxs6_read(unit, pc, reg_flags, reg_bank, reg_addr, &data));

   *phy_data = data;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs6_reg_write
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
phy_xgxs6_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint32               reg_flags;
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);


    pc   = INT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);

    reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_xgxs6_write(unit, pc, reg_flags, reg_bank, reg_addr, data);
}  

/*
 * Function:
 *      phy_xgxs6_reg_modify
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
phy_xgxs6_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    uint32               reg_flags;
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;     /* Temporary holder for phy_data_mask */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);

    pc   = INT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);
    mask = (uint16) (phy_data_mask & 0x0000FFFF);

    reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_xgxs6_modify(unit, pc, reg_flags, 
                             reg_bank, reg_addr, data, mask);
}

STATIC int
_phy_xgxs6_stop(int unit, soc_port_t port)
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
                         "phy_xgxs6_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));


    return _phy_xgxs6_enable_set(unit,port,stop?FALSE:TRUE);
}


/*
 * Function:
 *      _phy_xgxs6_notify_stop
 * Purpose:
 *      Add a reason to put serdesxgxs6 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_xgxs6_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;
    return _phy_xgxs6_stop(unit, port);
}

/*
 * Function:
 *      _phy_xgxs6_notify_resume
 * Purpose:
 *      Remove a reason to put serdesxgxs6 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_xgxs6_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;
    return _phy_xgxs6_stop(unit, port);
}

/*
 * Function:
 *      _phy_xgxs6_notify_speed
 * Purpose:
 *      Program speed if (and only if) serdesxgxs6 is an intermediate PHY.
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
_phy_xgxs6_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int          fiber;

    fiber = PHY_FIBER_MODE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs6_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (phy_xgxs6_speed_set(unit, port, speed));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && PHY_EXTERNAL_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_xgxs6_an_set(unit, port, FALSE));
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs6_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d rv=%d\n"),
              unit, port, speed, fiber,SOC_E_NONE));

    return SOC_E_NONE;
}

STATIC int
phy_xgxs6_notify(int unit, soc_port_t port,
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
        rv = (_phy_xgxs6_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_xgxs6_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_xgxs6_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        rv = (phy_xgxs6_an_set(unit, port, value));
        break;
    default:
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}


/*
 * Variable:
 *      phy_xgxs6_drv
 * Purpose:
 *      Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_xgxs6_hg = {
    "XGXS6 Unicore PHY Driver",
    phy_xgxs6_init,           /* Init */
    phy_null_reset,           /* Reset (dummy) */
    phy_xgxs6_link_get,       /* Link get   */
    phy_xgxs6_enable_set,     /* Enable set */
    phy_null_enable_get,      /* Enable get */
    phy_null_set,             /* Duplex set */
    phy_null_one_get,         /* Duplex get */
    phy_xgxs6_speed_set,      /* Speed set  */
    phy_xgxs6_speed_get,      /* Speed get  */
    phy_null_set,             /* Master set */
    phy_null_zero_get,        /* Master get */
    phy_xgxs6_an_set,         /* ANA set */
    phy_xgxs6_an_get,         /* ANA get */
    phy_xgxs6_adv_local_set,  /* Local Advert set */
    phy_xgxs6_adv_local_get,  /* Local Advert get */
    phy_xgxs6_adv_remote_get, /* Remote Advert get */
    phy_xgxs6_lb_set,         /* PHY loopback set */
    phy_xgxs6_lb_get,         /* PHY loopback set */
    phy_null_interface_set,   /* IO Interface set */
    phy_null_interface_get,   /* IO Interface get */
    phy_xgxs6_ability_get,    /* PHY abilities mask */
    NULL,                     /* phy_linkup_evnt */
    NULL,                     /* phy_linkdn_evnt */
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,                     /* Medium config set */
    NULL,                     /* Medium config get */
    phy_null_medium_get,
    NULL,                     /* phy_cable_diag  */
    NULL,                     /* phy_link_change */
    phy_xgxs6_control_set,
    phy_xgxs6_control_get,
    phy_xgxs6_reg_read,
    phy_xgxs6_reg_write, 
    phy_xgxs6_reg_modify,
    phy_xgxs6_notify                      /* phy_notify */ 
};

#endif /*  INCLUDE_PHY_XGXS6 */

typedef int _phy_xgxs6_not_empty; /* Make ISO compilers happy. */
