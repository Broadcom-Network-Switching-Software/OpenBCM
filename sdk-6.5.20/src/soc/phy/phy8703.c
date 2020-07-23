/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8703.c
 * Purpose:    Phys driver support for Broadcom Serial 10Gig
 *             transceiver with XAUI interface.
 */
#include "phydefs.h"     /* Must include before other phy related includes */
#ifdef INCLUDE_PHY_8703
#include "phyconfig.h"   /* Must be the first phy include after phydefs.h */

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
#include "phyxehg.h"
#include "phy8703.h"


/*
 * Function:
 *    phy_8703_init
 * Purpose:    
 *    Initialize 8703 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

static int
phy_8703_init(int unit, soc_port_t port)
{
    int         is_8704;
    uint16      tmp;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45);

    /* Reset the device */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8703_PMA_PMD_CTRLr(unit, pc,
                                      MII_CTRL_RESET, MII_CTRL_RESET));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8703_PCS_CTRLr(unit, pc,
                                  MII_CTRL_RESET, MII_CTRL_RESET));

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8703_PHYXS_CTRLr(unit, pc,
                                    MII_CTRL_RESET, MII_CTRL_RESET));


    /* P_DOWN/OPTXRST1 */
    SOC_IF_ERROR_RETURN
        (READ_PHY8703_IDENTIFIERr(unit, pc, &tmp));

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

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8703_IDENTIFIERr(unit, pc, tmp));

    /*
     * Enable 8704 XFP clock output
     */
    is_8704 = pc->phy_rev == 3;
    if (is_8704 && 
        soc_property_port_get(unit, port, spn_PHY_XFP_CLOCK, TRUE)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY8703_PMD_TX_CTRLr(unit, pc, 0x0100, 0x0100));
    }
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "8703: u=%d port%d: init.\n"),
                         unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8703_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8703_link_get(int unit, soc_port_t port, int *link)
{
    uint16      pma_mii_stat, pcs_mii_stat, pxs_mii_stat, link_stat;
    phy_ctrl_t *pc;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8703_link_get: u=%d port%d: link:%s\n"),
              unit, port, ""));

    if (link == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8703_PMA_PMD_STATr(unit, pc, &pma_mii_stat));

    /* Receive Link status */
    SOC_IF_ERROR_RETURN
        (READ_PHY8703_PCS_STATr(unit, pc, &pcs_mii_stat));

    /* Transmit Link status */
    SOC_IF_ERROR_RETURN
        (READ_PHY8703_PHYXS_STATr(unit, pc, &pxs_mii_stat));

    link_stat = pma_mii_stat & pcs_mii_stat & pxs_mii_stat;

    *link = (link_stat & MII_STAT_LA) ? TRUE : FALSE;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8703_link_get: u=%d port%d: link:%s\n"),
              unit, port, *link ? "Up": "Down"));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8703_enable_set
 * Purpose:
 *    Enable/Disable phy 
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8703_enable_set(int unit, soc_port_t port, int enable)
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
        (MODIFY_PHY8703_PMA_PMD_TX_DISABLEr(unit, pc, data, mask));
    return (SOC_E_NONE);
}


/*
 * Function:
 *    phy_8703_lb_set
 * Purpose:
 *    Put 8703 in PHY PCS/PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8703_lb_set(int unit, soc_port_t port, int enable)
{
    uint16     tmp;
    phy_ctrl_t *pc; 

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8703_lb_set: u=%d port%d: %s PCS/PMA/PMD loopback.\n"),
              unit, port, enable ? "Enabling": "Disabling"));

    pc = EXT_PHY_SW_STATE(unit, port);

    tmp = enable ? MII_CTRL_PMA_LOOPBACK : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8703_PMA_PMD_CTRLr(unit, pc,
                                      tmp, MII_CTRL_PMA_LOOPBACK));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8703_lb_get
 * Purpose:
 *    Get 8703 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8703_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      tmp;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHY8703_PMA_PMD_CTRLr(unit, pc, &tmp));

    *enable = (tmp & MII_CTRL_PMA_LOOPBACK) ? TRUE : FALSE;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8703_lb_get: u=%d port%d: loopback:%s\n"),
              unit, port, *enable ? "Enabled": "Disabled"));
    
    return SOC_E_NONE;
}


/*
 * Variable:
 *    phy_8703_drv
 * Purpose:
 *    Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_8703drv_xe = {
    "8703 10-Gigabit PHY Driver",
    phy_8703_init,        /* Init */
    phy_null_reset,       /* Reset (dummy) */
    phy_8703_link_get,    /* Link get   */
    phy_8703_enable_set,  /* Enable set */
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
    phy_8703_lb_set,      /* PHY loopback set */
    phy_8703_lb_get,      /* PHY loopback set */
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
    NULL,                    /* phy_control_set */
    NULL,                    /* phy_control_get */
    NULL,                    /* phy_reg_read */
    NULL,                    /* phy_reg_write */
    NULL,                    /* phy_reg_modify */
    NULL,                    /* phy_notify */
};

#else /* INCLUDE_PHY_8703 */
typedef int _soc_phy_8703_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8703 */

