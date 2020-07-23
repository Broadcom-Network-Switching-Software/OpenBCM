/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8705.c
 * Purpose:    Phys driver support for Broadcom Serial 10Gig
 *             transceiver with XAUI interface.
 */
#include "phydefs.h"    /* Must include before other phy related includes */
#ifdef INCLUDE_PHY_8705
#include "phyconfig.h"  /* Must be the first phy include after phydefs.h */

#include <sal/types.h>
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
#include "phyxehg.h"
#include "phy8705.h"

#define LAN_MODE 0 
#define WAN_MODE 1

STATIC int
_phy_8705_mode_set(int unit, soc_port_t port, uint32 mode);
 
/*
 * Function:
 *    phy_8705_init
 * Purpose:    
 *    Initialize 8705 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

static int
phy_8705_init(int unit, soc_port_t port)
{
    int         wan_mode;
    uint16      tmp;
    uint16      invert_txrx;
    phy_ctrl_t *pc;

    PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45);

    wan_mode = soc_property_port_get(unit, port, spn_PHY_WAN_MODE, FALSE);

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Reset the device */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8705_PMA_PMD_CTRLr(unit, pc, 
                                      MII_CTRL_RESET, MII_CTRL_RESET));
    if (wan_mode) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8705_WIS_CTRLr(unit, pc,
                                      MII_CTRL_RESET, MII_CTRL_RESET));
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8705_PCS_CTRLr(unit, pc,
                                  MII_CTRL_RESET, MII_CTRL_RESET));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8705_PHYXS_CTRLr(unit, pc,
                                    MII_CTRL_RESET, MII_CTRL_RESET));

    /* P_DOWN/OPTXRST1 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8705_IDENTIFIERr(unit, pc, &tmp));

    if (soc_property_port_get(unit, port, spn_FORCE_OPTTXENBLVL, FALSE)) {
        tmp |= 0x8000; /* Set OPTXENB_LVL */
    } else {
        tmp &= ~0x8000;
    }
 
    if (soc_property_port_get(unit, port, spn_FORCE_OPTTXRSTLVL, TRUE)) {
        tmp |= 0x4000; /* Set OPTXRST_LVL */
    } else {
        tmp &= ~0x4000;
    }

    if (soc_property_port_get(unit, port, spn_FORCE_OPTBIASFLTLVL, TRUE)) {
        tmp |= 0x2000; /* Set OPBIASFLT_LVL */
    } else {
        tmp &= ~0x2000;
    }

    if (soc_property_port_get(unit, port, spn_FORCE_OPTTEMPFLTLVL, TRUE)) {
        tmp |= 0x1000; /* Set OPMPFLT_LVL */
    } else {
        tmp &= ~0x1000;
    }

    if (soc_property_port_get(unit, port, spn_FORCE_OPTPRFLTLVL, TRUE)) {
        tmp |= 0x0800; /* Set OPPRFLT_LVL */
    } else {
        tmp &= ~0x0800;
    }

    if (soc_property_port_get(unit, port, spn_FORCE_OPTTXFLLVL, TRUE)) {
        tmp |= 0x0400; /* Set OPTXFLT_LVL */
    } else {
        tmp &= ~0x0400;
    }

    if (soc_property_port_get(unit, port, spn_FORCE_OPTRXLOSLVL, TRUE)) {
        tmp |= 0x0200; /* Set OPRXLOS_LVL */
    } else {
        tmp &= ~0x0200;
    }

    if (soc_property_port_get(unit, port, spn_FORCE_OPTRXFLTLVL, TRUE)) {
        tmp |= 0x0100; /* Set OPRXFLT_LVL */
    } else {
        tmp &= ~0x0100;
    }

    if (soc_property_port_get(unit, port, spn_FORCE_OPTTXONLVL, TRUE)) {
        tmp |= 0x0080; /* Set OPTXON_LVL */
    } else {
        tmp &= ~0x0080;
    }

    /* P_DOWN/OPTXRST1 */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8705_IDENTIFIERr(unit, pc, tmp));

    if (soc_property_port_get(unit, port, spn_PHY_XCLKSEL_OVRD, FALSE)) {
        /*  set clock override bit to 1 and clear bit6,7*/
        tmp = (1 << 7) | (1 << 6) | (1 << 4);
        SOC_IF_ERROR_RETURN
           (MODIFY_PHY8705_PMD_MISC_CTRLr(unit, pc, (1 << 4), tmp));
    }

    tmp = 0;

    if (soc_property_port_get(unit, port, spn_PHY_CLOCK_ENABLE, FALSE)) {
        tmp = (0x1 << 15);
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8705_PMA_PMD_REG(unit, pc, 0xCA08, tmp, (0x1 << 15)));

    if (wan_mode) {
        SOC_IF_ERROR_RETURN
            (_phy_8705_mode_set(unit, port, WAN_MODE));
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "8705: u=%d port=%d mode=%s : init.\n"), 
                         unit, port, wan_mode ? "WAN" : "LAN"));

    /*
     * Enable 8705 XFP clock output
     */
    if (soc_property_port_get(unit, port, spn_PHY_XFP_CLOCK, TRUE)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8705_PMD_MISC_CTRLr(unit, pc, 0x0008, 0x0008));
    } 

    invert_txrx = 0;
    if (soc_property_port_get(unit, port, spn_PHY_TX_INVERT, FALSE)) {
        invert_txrx |= (1 << 10);
    }
    if (soc_property_port_get(unit, port, spn_PHY_RX_INVERT, FALSE)) {
        invert_txrx |= (1 << 9);
    }
    if (invert_txrx) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8705_PMD_DIGI_CTRLr(unit, pc,
                            invert_txrx, (1 << 10) | (1 << 9))); 
    }

    /* Override the lock detect signal in the PMD PLL. */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8705_OVERRIDE_CTRLr(unit, pc, (1 << 8), (1 << 8)));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8705_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:    
 *    SOC_E_NONE
 */

int
phy_8705_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc;
    uint16      pma_mii_stat, pcs_mii_stat, pxs_mii_stat, wis_mii_stat;
    uint16      link_stat;

    if (link == NULL) {
        return SOC_E_PARAM;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8705_PMA_PMD_STATr(unit, pc, &pma_mii_stat));

    /* Receive Link status */
    SOC_IF_ERROR_RETURN
        (READ_PHY8705_PCS_STATr(unit, pc, &pcs_mii_stat));

    /* Transmit Link status */
    SOC_IF_ERROR_RETURN
        (READ_PHY8705_PHYXS_STATr(unit, pc, &pxs_mii_stat));
 
    link_stat = pma_mii_stat & pcs_mii_stat & pxs_mii_stat;
 
    if (PHY_WAN_MODE(unit, port)) {
         /* WIS status */
        SOC_IF_ERROR_RETURN
            (READ_PHY8705_WIS_STATr(unit, pc, &wis_mii_stat));
        link_stat &= wis_mii_stat;
    }

    *link = (link_stat & MII_STAT_LA) ? TRUE : FALSE;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8705_link_get: u=%d port%d: link:%s\n"),
              unit, port, *link ? "Up": "Down"));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8705_enable_set
 * Purpose:
 *    Enable/Disable phy 
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:    
 *    SOC_E_NONE
 */

int
phy_8705_enable_set(int unit, soc_port_t port, int enable)
{
    uint16      data;       /* Holder for new value to write to PHY reg */
    uint16      mask;       /* Holder for bit mask to update in PHY reg */
    phy_ctrl_t *pc;
 
    pc = EXT_PHY_SW_STATE(unit, port);

    data = 0;
    mask = 1 << 0; /* Global PMD transmit disable */ 
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        data = 1 << 0;  /* Global PMD transmit disable */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8705_PMA_PMD_TX_DISABLEr(unit, pc, data, mask));
    return (SOC_E_NONE);
}

    
/*
 * Function:
 *    phy_8705_lb_set
 * Purpose:
 *    Put 8705 in PHY PCS/PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

int
phy_8705_lb_set(int unit, soc_port_t port, int enable)
{
    uint16      tmp;
    phy_ctrl_t *pc;
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8705_lb_set: u=%d port%d: %s %s loopback.\n"),
              unit, port, enable ? "Enabling": "Disabling",
              PHY_WAN_MODE(unit, port) ? "WAN" : "PMA/PMD"));

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_WAN_MODE(unit, port)) {
        /* Remove PMA/PMD loopback first */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8705_PMA_PMD_CTRLr(unit, pc,
                                          0, MII_CTRL_PMA_LOOPBACK));
        tmp = enable ? MII_CTRL_WIS_LOOP_BACK : 0;

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8705_WIS_CTRLr(unit, pc,
                                      tmp, MII_CTRL_WIS_LOOP_BACK));
    } else {
        /* Remove WAN loopback first */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8705_WIS_CTRLr(unit, pc,
                                      0, MII_CTRL_WIS_LOOP_BACK));
        tmp = enable ? MII_CTRL_PMA_LOOPBACK : 0;

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8705_PMA_PMD_CTRLr(unit, pc,
                                          tmp, MII_CTRL_PMA_LOOPBACK));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8705_lb_get
 * Purpose:
 *    Get 8705 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

int
phy_8705_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      tmp;
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_WAN_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY8705_WIS_CTRLr(unit, pc, &tmp));

        *enable = (tmp & MII_CTRL_WIS_LOOP_BACK) ? TRUE : FALSE;
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY8705_PMA_PMD_CTRLr(unit, pc, &tmp));

        *enable = (tmp & MII_CTRL_PMA_LOOPBACK) ? TRUE : FALSE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8705_lb_get: u=%d port%d: loopback:%s\n"),
              unit, port, *enable ? "Enabled": "Disabled"));
    
    return SOC_E_NONE;
}

STATIC int
_phy_8705_mode_set(int unit, soc_port_t port, uint32 mode) {
    uint16      dev_reg, mask,data16, tmp;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch(mode) {
        case WAN_MODE:
            if (PHY_WAN_MODE(unit, port)) {
                /* Already in WAN mode */ 
                return SOC_E_NONE;
            }
            SOC_IF_ERROR_RETURN
                (READ_PHY8705_WIS_DEV_IN_PKGr(unit, pc,  &dev_reg));
            /* Check WIS present */ 
            if (!(dev_reg & 0x0004)) {
                return SOC_E_UNAVAIL;
            }
            /* To configure WAN mode clock,
             * CKMODE_SEL     3.3V
             * MODE_SEL       3.3V
             * XCLKMODE_OVRD    1
             * P_IN_MUXSEL      0
             * X_IN_MUXSEL      0
             */
            mask = (1 << 7) | (1 << 6) | (1 << 4);
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8705_PMD_MISC_CTRLr(unit, pc, (1 << 4), mask));

            /* put device in WAN mode */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8705_IDENTIFIERr(unit, pc, PMAD_IDr_WAN_MODE, 
                        PMAD_IDr_WAN_MODE));

            tmp = 0;
            if (soc_property_port_get(unit, port, spn_PHY_CLOCK_ENABLE, FALSE)) {
                tmp = (0x1 << 15);
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8705_PMA_PMD_REG(unit, pc, 0xCA08, tmp, (0x1 << 15)));

            PHY_FLAGS_SET(unit, port, PHY_FLAGS_WAN);
            break;

        case LAN_MODE:
            if (!PHY_WAN_MODE(unit, port)) {
                /* Already in LAN mode */
                return SOC_E_NONE;
            } 
            /* To configure LAN mode clock,
             * CKMODE_SEL     3.3V
             * MODE_SEL       GND
             * XCLKMODE_OVRD  0  ( Bit 4: Address 0xCA0A )
             * P_IN_MXSEL     1  ( Bit 7: Address 0xCA0A ) 
             * X_IN_MXSEL     0  ( Bit 6: Address 0xCA0A )
             */ 
            mask = (1 << 7) | (1 << 6) | (1 << 4);
            if (soc_property_port_get(unit, port, spn_PHY_XCLKSEL_OVRD,FALSE)) {
                data16 = (1 << 4);
            } else {
                data16 = (1 << 7);
            }
            SOC_IF_ERROR_RETURN
                 (MODIFY_PHY8705_PMD_MISC_CTRLr(unit, pc, data16, mask));
           
            /* put device in LAN mode */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8705_IDENTIFIERr(unit, pc, 0, 
                        PMAD_IDr_WAN_MODE));
            tmp = 0;
            if (soc_property_port_get(unit, port, spn_PHY_CLOCK_ENABLE, FALSE)) {
                tmp = (0x1 << 15);
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8705_PMA_PMD_REG(unit, pc, 0xCA08, tmp, (0x1 << 15)));

            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_WAN);
            break;
        default:
            return SOC_E_UNAVAIL;
    }
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8705_mode_set: u=%d port=%d mode=%d\n"),
              unit, port, mode));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8705_control_set
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
phy_8705_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t *pc;

    PHY_CONTROL_TYPE_CHECK(type);
    
    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;    

    switch(type) {
        case SOC_PHY_CONTROL_WAN:
            rv = _phy_8705_mode_set(unit, port, 
                                       value ? WAN_MODE : LAN_MODE);
            break;

        case SOC_PHY_CONTROL_CLOCK_ENABLE:
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY8705_PMA_PMD_REG(unit, pc, 0xCA08, value ? (0x1 << 15) : 0, (0x1 << 15)));
            rv = SOC_E_NONE;
            break;

        default:
            break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_8705_control_get
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
phy_8705_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int    rv;
    uint16 temp16;
    phy_ctrl_t *pc;

    if (NULL == value) {
        return SOC_E_PARAM;
    }

    PHY_CONTROL_TYPE_CHECK(type);

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;

    switch(type) {
        case SOC_PHY_CONTROL_WAN:
            *value = PHY_WAN_MODE(unit, port) ? 1 : 0;   
            rv = SOC_E_NONE;
            break;

        case SOC_PHY_CONTROL_CLOCK_ENABLE:
            SOC_IF_ERROR_RETURN
                (READ_PHY8705_PMA_PMD_REG(unit, pc, 0xCA08, &temp16));
            *value = temp16 & (0x1<<15) ? TRUE : FALSE;
            rv = SOC_E_NONE;
            break;

        default:
            break;
    }
    return rv;
}

/*
 * Variable:
 *    phy_8705_drv
 * Purpose:
 *    Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_8705drv_xe = {
    "8705/8724/8725 10-Gigabit PHY Driver",
    phy_8705_init,        /* Init */
    phy_null_reset,       /* Reset (dummy) */
    phy_8705_link_get,    /* Link get   */
    phy_8705_enable_set,  /* Enable set */
    phy_null_enable_get,  /* Enable get */
    phy_null_set,         /* Duplex set */
    phy_null_one_get,     /* Duplex get */
    phy_xehg_speed_set,   /* Speed set  */
    phy_xehg_speed_get,   /* Speed get  */
    phy_null_set,         /* Master set */
    phy_null_zero_get,    /* Master get */
    phy_null_set,         /* ANA set */
    phy_null_an_get,      /* ANA get */
    phy_null_mode_set,    /* Local Advert set */
    phy_null_mode_get,    /* Local Advert get */
    phy_null_mode_get,    /* Remote Advert get */
    phy_8705_lb_set,      /* PHY loopback set */
    phy_8705_lb_get,      /* PHY loopback set */
    phy_null_interface_set, /* IO Interface set */
    phy_xehg_interface_get, /* IO Interface get */
    phy_xehg_ability_get,   /* PHY abilities mask */
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                    /* phy_cable_diag  */
    NULL,                    /* phy_link_change */
    phy_8705_control_set,    /* phy_control_set */
    phy_8705_control_get,    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL                     /* phy_notify */
};

#else /* INCLUDE_PHY_8705 */
typedef int _phy_8705_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8705 */

