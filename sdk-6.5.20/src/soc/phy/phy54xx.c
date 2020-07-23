/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy54xx.c
 *
 * The PHY drivers in this file mostly use the standard register set and
 * are implemented using the phy_fe/ge driver routines, with the
 * addition of a few chip-dependent register settings.
 *
 * If LDR is 1, then the PHY will be reset each time link down is
 * detected on copper.  BCM54xx has difficulty obtaining link with
 * certain other gigabit PHYs if not reset when link goes down.
 */

#ifdef INCLUDE_PHY_54XX

#include <sal/types.h>
#include <sal/core/spl.h>

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
#include "phyfege.h"
#include "phy54xx.h"

#define LDR        1                /* Link Down Reset Workaround */

#if LDR

/*
 * Function:
 *        phy_54xx_reset_war
 * Purpose:
 *        Helper function for PHY reset workaround
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 *        config_fn - PHY-specific post-reset initialization callback
 * Notes:
 *        PHYs in the 54xx family have an errata where after link goes
 *        down and then back up, autonegotiation may cycle forever and
 *        never finish.  The condition is infrequent but does happen.  The
 *        effective workaround is to reset the PHY any time the link goes
 *        down.  This routine performs a PHY reset while saving and
 *        restoring all the relevent register settings.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_54xx_reset_war(int unit, soc_port_t port,
                   int (*config_fn)(int unit, soc_port_t port))
{
    phy_ctrl_t   *pc;
    uint16        mii_ctrl, mii_ana, mii_gb_ctrl;

    pc = EXT_PHY_SW_STATE(unit, port);
    /* Save some of the registers */

    SOC_IF_ERROR_RETURN
        (READ_PHY54XX_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHY54XX_MII_ANAr(unit, pc, &mii_ana));
    SOC_IF_ERROR_RETURN
        (READ_PHY54XX_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

    /* Reset PHY */

    SOC_IF_ERROR_RETURN(soc_phy_reset(unit, port));

    /* Perform PHY-specific initial configuration */

    if (config_fn != NULL) {
        SOC_IF_ERROR_RETURN(config_fn(unit, port));
    }

    /* Restore some of the registers */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54XX_MII_CTRLr(unit, pc, mii_ctrl));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54XX_MII_ANAr(unit, pc, mii_ana));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54XX_MII_GB_CTRLr(unit, pc, mii_gb_ctrl));

    return SOC_E_NONE;
}

#endif /* LDR */

STATIC int
_phy_54xx_init(int unit, soc_port_t port)
{
    uint16             id0, id1;
    phy_ctrl_t       *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);

    SOC_IF_ERROR_RETURN
        (READ_PHY54XX_MII_PHY_ID0r(unit, pc, &id0));
    SOC_IF_ERROR_RETURN
        (READ_PHY54XX_MII_PHY_ID1r(unit, pc, &id1));

    pc->phy_oui   = PHY_OUI(id0, id1);
    pc->phy_model = PHY_MODEL(id0, id1);
    pc->phy_rev   = PHY_REV(id0, id1);

    return SOC_E_NONE;
}

/*
 * Function:
 *        phy_5401_setup
 * Purpose:
 *        Initialize 5401 registers
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5401_setup(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Disable tap power-mgmt.
     * Will need to check revision number for later revisions.
     */

    SOC_IF_ERROR_RETURN(phy_reg_ge_write(unit, pc, 0x00, 0x0000, 0x18, 0x0c20));
    SOC_IF_ERROR_RETURN(phy_reg_ge_write(unit, pc, 0x00, 0x0012, 0x15, 0x1804));
    SOC_IF_ERROR_RETURN(phy_reg_ge_write(unit, pc, 0x00, 0x0013, 0x15, 0x1204));
    SOC_IF_ERROR_RETURN(phy_reg_ge_write(unit, pc, 0x00, 0x8006, 0x15, 0x0132));
    SOC_IF_ERROR_RETURN(phy_reg_ge_write(unit, pc, 0x00, 0x8006, 0x15, 0x0232));
    SOC_IF_ERROR_RETURN(phy_reg_ge_write(unit, pc, 0x00, 0x201f, 0x15, 0x0a20));

    return(SOC_E_NONE);
}

#if LDR

/*
 * Function:
 *        phy_5401_linkdown
 * Purpose:
 *        Implement PHY reset workaround.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5401_linkdown(int unit, soc_port_t port)
{
    return phy_54xx_reset_war(unit, port, phy_5401_setup);
}

#endif /* LDR */

/*
 * Function:
 *        phy_bcm5401_init
 * Purpose:
 *        Init function for 5401 phy.  Write default registers.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_bcm5401_init(int unit, soc_port_t port)
{
    SOC_IF_ERROR_RETURN(_phy_54xx_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_ge_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_5401_setup(unit, port));

    return(SOC_E_NONE);
}

/*
 * Function:
 *        phy_5402_setup
 * Purpose:
 *        Initialize 5402 registers
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5402_setup(int unit, soc_port_t port)
{
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (port % 2) { /* set power for high port of pair. */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY54XX_POWER_CTRLr(unit, pc, 0x0212));
    }

    /* Magic numbers for 100 speed initialization. */
    SOC_IF_ERROR_RETURN(phy_reg_ge_write(unit, pc, 0x00, 0x0000, 0x18, 0x0c20));
    SOC_IF_ERROR_RETURN(phy_reg_ge_write(unit, pc, 0x00, 0x0007, 0x15, 0x8107));
    SOC_IF_ERROR_RETURN(phy_reg_ge_write(unit, pc, 0x00, 0x0000, 0x18, 0x0420));

    return SOC_E_NONE;
}

#if LDR

/*
 * Function:
 *        phy_5402_linkdown
 * Purpose:
 *        Implement PHY reset workaround.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5402_linkdown(int unit, soc_port_t port)
{
    return phy_54xx_reset_war(unit, port, phy_5402_setup);
}

#endif /* LDR */

/*
 * Function:
 *        phy_5402_init
 * Purpose:
 *        Init function for 5402 phy.  Write default registers.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5402_init(int unit, soc_port_t port)
{

    SOC_IF_ERROR_RETURN(_phy_54xx_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_ge_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_5402_setup(unit, port));

    return(SOC_E_NONE);
}

/*
 * Function:
 *        phy_5404_setup
 * Purpose:
 *        Initialize 5404 registers
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5404_setup(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (!((port + 2) % 4)) { /* set power for first port of quad. */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY54XX_POWER_CTRLr(unit, pc, 0x0592));
    }

    /* led settings */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54XX_SPARE_CTRL1r(unit, pc, 0x8820));

    /* Bang autoneg register */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54XX_MII_ANAr(unit, pc, 0x05de));

    return SOC_E_NONE;
}

#if LDR

/*
 * Function:
 *        phy_5404_linkdown
 * Purpose:
 *        Implement PHY reset workaround.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5404_linkdown(int unit, soc_port_t port)
{
    return phy_54xx_reset_war(unit, port, phy_5404_setup);
}

#endif /* LDR */

/*
 * Function:
 *        phy_5404_init
 * Purpose:
 *        Init function for 5404 phy.  Write default registers.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5404_init(int unit, soc_port_t port)
{

    SOC_IF_ERROR_RETURN(_phy_54xx_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_ge_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_5404_setup(unit, port));

    return(SOC_E_NONE);
}

/*
 * Function:
 *        phy_5411_setup
 * Purpose:
 *        Initialize 5411 registers
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5411_setup(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    /*
     * NOTE: the following are provided only because earlier 48-port
     * boards left the external loopback input floating by accident.
     * Otherwise it is not required.
     */

    /* Clear ext loopback */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54XX_AUX_CTRLr(unit, pc, 0x0420));

    /* Clear Swap Rx MDIX and TXHalfout */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54XX_MISC_TESTr(unit, pc, 0x0004));

    return SOC_E_NONE;
}

#if LDR

/*
 * Function:
 *        phy_5411_linkdown
 * Purpose:
 *        Implement PHY reset workaround.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5411_linkdown(int unit, soc_port_t port)
{
    return phy_54xx_reset_war(unit, port, phy_5411_setup);
}

#endif /* LDR */

/*
 * Function:
 *        phy_5411_init
 * Purpose:
 *        Init function for 5411 phy.  Write default registers.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5411_init(int unit, soc_port_t port)
{

    SOC_IF_ERROR_RETURN(_phy_54xx_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_ge_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_5411_setup(unit, port));

    return(SOC_E_NONE);
}

/*
 * Function:
 *        phy_5424_setup
 * Purpose:
 *        Initialize 5424 registers
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5424_setup(int unit, soc_port_t port)
{
    uint16        tmp;
    phy_ctrl_t   *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Configure Extended Control Register */
    SOC_IF_ERROR_RETURN
        (READ_PHY54XX_MII_ECRr(unit, pc, &tmp));
    tmp |= 0x0020;        /* Enable LEDs to indicate traffic status */
    tmp |= 0x0001;        /* Set high FIFO elasticity to support jumbo frames */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY54XX_MII_ECRr(unit, pc, tmp));

    /* Enable extended packet length (4.5k through 25k) */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54XX_AUX_CTRLr(unit, pc, 0x4000, 0x4000));

    return SOC_E_NONE;
}

#if LDR

/*
 * Function:
 *        phy_5424_linkdown
 * Purpose:
 *        Implement PHY reset workaround.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5424_linkdown(int unit, soc_port_t port)
{
    return phy_54xx_reset_war(unit, port, phy_5424_setup);
}

#endif /* LDR */

/*
 * Function:
 *        phy_5424_init
 * Purpose:
 *        Init function for 5424/5434 phy.  Write default registers.
 * Parameters:
 *        unit - StrataSwitch unit #.
 *        port - StrataSwitch port #.
 * Returns:
 *        SOC_E_XXX
 */

STATIC int
phy_5424_init(int unit, soc_port_t port)
{

    SOC_IF_ERROR_RETURN(_phy_54xx_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_ge_init(unit, port));
    SOC_IF_ERROR_RETURN(phy_5424_setup(unit, port));

    return(SOC_E_NONE);
}

/*
 * Function:
 *     phy_54xx_enable_set
 * Description:
 *     Enable or disable the physical interface
 * Parameters:
 *     unit   - Device number
 *     port   - Port number
 *     enable - Boolean, true = enable, false = enable.
 * Returns:
 *     SOC_E_XXX
 */
STATIC int
phy_54xx_enable_set(int unit, soc_port_t port, int enable)
{
    int          rv;         /* Return value */
    uint16       data;       /* New value to write to PHY register */
    phy_ctrl_t *pc;

    pc       = EXT_PHY_SW_STATE(unit, port);
    data     = enable ? 0 : MII_ECR_TD; /* Transmitt enable/disable */

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY54XX_MII_ECRr(unit, pc, data, MII_ECR_TD));

    rv = phy_fe_ge_enable_set(unit, port, enable);

    return rv;
}

/*
 * Function:
 *      phy_54xx_reg_read
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
phy_54xx_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint32               reg_flags;
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);
    EXT_PHY_INIT_CHECK(unit, port);

    pc   = EXT_PHY_SW_STATE(unit, port);

    reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    SOC_IF_ERROR_RETURN
        (phy_reg_ge_read(unit, pc, reg_flags, reg_bank, reg_addr, &data));

   *phy_data = data;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_54xx_reg_write
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
phy_54xx_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint32               reg_flags;
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);

    EXT_PHY_INIT_CHECK(unit, port);

    pc   = EXT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);

    reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_ge_write(unit, pc, reg_flags, reg_bank, reg_addr, data);
}   

/*
 * Function:
 *      phy_54xx_reg_modify
 * Purpose:
 *      Routine to write PHY register 
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      why_mo_data  - New value for the bits specified in phy_mo_mask 
 *      phy_mo_mask  - Bit mask to modify 
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads. 
 */
STATIC int
phy_54xx_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data, uint32 phy_data_mask)
{
    uint32               reg_flags;
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;     /* Temporary holder for phy_data_mask */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);

    EXT_PHY_INIT_CHECK(unit, port);

    pc   = EXT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);
    mask = (uint16) (phy_data_mask & 0x0000FFFF);

    reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_ge_modify(unit, pc, reg_flags, reg_bank, reg_addr, data, mask);
} 

/*
 * Variable:        phy_5401drv_ge
 * Purpose:        Phy driver for 5401.
 */

phy_driver_t phy_5401drv_ge = {
    "5401 Gigabit PHY Driver",
    phy_bcm5401_init,
    phy_fe_ge_reset,
    phy_fe_ge_link_get,
    phy_fe_ge_enable_set,
    phy_fe_ge_enable_get,
    phy_fe_ge_duplex_set,
    phy_fe_ge_duplex_get,
    phy_fe_ge_speed_set,
    phy_fe_ge_speed_get,
    phy_fe_ge_master_set,
    phy_fe_ge_master_get,
    phy_fe_ge_an_set,
    phy_fe_ge_an_get,
    phy_ge_adv_local_set,
    phy_ge_adv_local_get,
    phy_ge_adv_remote_get,
    phy_fe_ge_lb_set,
    phy_fe_ge_lb_get,
    phy_ge_interface_set,
    phy_ge_interface_get,
    phy_ge_ability_get,
    NULL,                       /* phy_linkup_evt */
#if LDR
    phy_5401_linkdown,
#else
    NULL,                       /* phy_linkdn_evt */
#endif /* LDR */
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,                       /* phy_medium_config_set */
    NULL,                       /* phy_medium_config_get */ 
    phy_fe_ge_medium_get,
    NULL,                       /* phy_cable_diag */
    NULL,                       /* phy_link_change */
    NULL,                       /* phy_control_set */
    NULL,                       /* phy_control_get */
    phy_54xx_reg_read,
    phy_54xx_reg_write, 
    phy_54xx_reg_modify,
    NULL                        /* phy_notify */
};


/*
 * Variable:        phy_5402drv_ge
 * Purpose:        Phy driver for 5402.
 */

phy_driver_t phy_5402drv_ge = {
    "5402 Gigabit PHY Driver",
    phy_5402_init,
    phy_fe_ge_reset,
    phy_fe_ge_link_get,
    phy_54xx_enable_set,
    phy_fe_ge_enable_get,
    phy_fe_ge_duplex_set,
    phy_fe_ge_duplex_get,
    phy_fe_ge_speed_set,
    phy_fe_ge_speed_get,
    phy_fe_ge_master_set,
    phy_fe_ge_master_get,
    phy_fe_ge_an_set,
    phy_fe_ge_an_get,
    phy_ge_adv_local_set,
    phy_ge_adv_local_get,
    phy_ge_adv_remote_get,
    phy_fe_ge_lb_set,
    phy_fe_ge_lb_get,
    phy_ge_interface_set,
    phy_ge_interface_get,
    phy_ge_ability_get,
    NULL,                       /* phy_linkup_evt */ 
#if LDR
    phy_5402_linkdown,
#else
    NULL,                       /* phy_linkdn_evt */
#endif /* LDR */
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,                       /* phy_medium_config_set */
    NULL,                       /* phy_medium_config_get */
    phy_fe_ge_medium_get,
    NULL,
    NULL,                       /* phy_link_change */
    NULL,                       /* phy_control_set */
    NULL,                       /* phy_control_get */
    phy_54xx_reg_read,
    phy_54xx_reg_write, 
    phy_54xx_reg_modify,
    NULL                        /* phy_notify */
};

/*
 * Variable:        phy_5404drv_ge
 * Purpose:        Phy driver for 5404.
 */

phy_driver_t phy_5404drv_ge = {
    "5404 Gigabit PHY Driver",
    phy_5404_init,
    phy_fe_ge_reset,
    phy_fe_ge_link_get,
    phy_54xx_enable_set,
    phy_fe_ge_enable_get,
    phy_fe_ge_duplex_set,
    phy_fe_ge_duplex_get,
    phy_fe_ge_speed_set,
    phy_fe_ge_speed_get,
    phy_fe_ge_master_set,
    phy_fe_ge_master_get,
    phy_fe_ge_an_set,
    phy_fe_ge_an_get,
    phy_ge_adv_local_set,
    phy_ge_adv_local_get,
    phy_ge_adv_remote_get,
    phy_fe_ge_lb_set,
    phy_fe_ge_lb_get,
    phy_ge_interface_set,
    phy_ge_interface_get,
    phy_ge_ability_get,
    NULL,                       /* phy_linkup_evt */
#if LDR
    phy_5404_linkdown,
#else
    NULL,                       /* phy_linkdn_evt */
#endif /* LDR */
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,                       /* phy_medium_set */
    NULL,                       /* phy_medium_get */
    phy_fe_ge_medium_get,
    NULL,
    NULL,                       /* phy_link_change */
    NULL,                       /* phy_control_set */
    NULL,                       /* phy_control_get */
    phy_54xx_reg_read,
    phy_54xx_reg_write, 
    phy_54xx_reg_modify,
    NULL                        /* phy_notify */
};

/*
 * Variable:        phy_5411drv_ge
 * Purpose:        Phy driver for 5411.
 */

phy_driver_t phy_5411drv_ge = {
    "5411 Gigabit PHY Driver",
    phy_5411_init,
    phy_fe_ge_reset,
    phy_fe_ge_link_get,
    phy_54xx_enable_set,
    phy_fe_ge_enable_get,
    phy_fe_ge_duplex_set,
    phy_fe_ge_duplex_get,
    phy_fe_ge_speed_set,
    phy_fe_ge_speed_get,
    phy_fe_ge_master_set,
    phy_fe_ge_master_get,
    phy_fe_ge_an_set,
    phy_fe_ge_an_get,
    phy_ge_adv_local_set,
    phy_ge_adv_local_get,
    phy_ge_adv_remote_get,
    phy_fe_ge_lb_set,
    phy_fe_ge_lb_get,
    phy_ge_interface_set,
    phy_ge_interface_get,
    phy_ge_ability_get,
    NULL,                        /* phy_linkup_event */
#if LDR
    phy_5411_linkdown,
#else
    NULL,                        /* phy_linkdn_event */
#endif /* LDR */
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,                        /* phy_medium_config_set */
    NULL,                        /* phy_medium_config_get */
    phy_fe_ge_medium_get,
    NULL,
    NULL,                       /* phy_link_change */
    NULL,                       /* phy_control_set */
    NULL,                       /* phy_control_get */
    phy_54xx_reg_read,
    phy_54xx_reg_write, 
    phy_54xx_reg_modify,
    NULL                        /* phy_notify */
};

/*
 * Variable:        phy_5424drv_ge
 * Purpose:        Phy driver for 5424/5434.
 */
phy_driver_t phy_5424drv_ge = {
    "5424/34 Gigabit PHY Driver",
    phy_5424_init,
    phy_fe_ge_reset,
    phy_fe_ge_link_get,
    phy_54xx_enable_set,
    phy_fe_ge_enable_get,
    phy_fe_ge_duplex_set,
    phy_fe_ge_duplex_get,
    phy_fe_ge_speed_set,
    phy_fe_ge_speed_get,
    phy_fe_ge_master_set,
    phy_fe_ge_master_get,
    phy_fe_ge_an_set,
    phy_fe_ge_an_get,
    phy_ge_adv_local_set,
    phy_ge_adv_local_get,
    phy_ge_adv_remote_get,
    phy_fe_ge_lb_set,
    phy_fe_ge_lb_get,
    phy_ge_interface_set,
    phy_ge_interface_get,
    phy_ge_ability_get,
    NULL,                       /* Phy link up event */
#if LDR
    phy_5424_linkdown,
#else
    NULL,                       /* Phy link down event */
#endif /* LDR */
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,                       /* Phy config set */
    NULL,                       /* Phy config get */
    phy_fe_ge_medium_get,
    NULL,                       /* Phy medium set */
    NULL,                       /* phy_link_change */
    NULL,                       /* phy_control_set */
    NULL,                       /* phy_control_get */
    phy_54xx_reg_read,
    phy_54xx_reg_write, 
    phy_54xx_reg_modify,
    NULL                        /* Phy event notify */
};

#endif /* INCLUDE_PHY_54XX */

typedef int _soc_phy_54xx_not_empty; /* Make ISO compilers happy. */
