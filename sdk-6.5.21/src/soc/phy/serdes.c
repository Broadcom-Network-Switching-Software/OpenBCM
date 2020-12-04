/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        serdes.c
 * Purpose:     Defines common PHY driver routines for Broadcom SerDes core.
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_SERDES)
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


#include "phyreg.h"
#include "serdes.h"

/*
 * Function:
 *      phy_serdes_link_get
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

int
phy_serdes_link_get(int unit, soc_port_t port, int *link)
{
    uint16       mii_stat;
    uint16       mii_ctrl;
    uint16       ctrl2;
    int          an_done;
    phy_ctrl_t  *pc;

    if (PHY_DISABLED_MODE(unit, port)) {
        *link = FALSE;
    } else {
        pc = INT_PHY_SW_STATE(unit, port);

        SOC_IF_ERROR_RETURN
            (READ_SERDES_MII_STATr(unit, pc, &mii_stat));

        *link = ((mii_stat & MII_STAT_LA) != 0);

        SOC_IF_ERROR_RETURN
            (READ_SERDES_MII_CTRLr(unit, pc, &mii_ctrl));

        SOC_IF_ERROR_RETURN
            (READ_SERDES_1000X_CTRL2r(unit, pc, &ctrl2));

        /* qualify the link with autoneg done only when autoneg is enabled
         * and paralell detect is disabled.
         */ 
        if ((mii_ctrl & MII_CTRL_AE) && 
            (!(ctrl2 & SERDES_1000X_CTRL2_PAR_DET_EN))) {
            an_done = ((mii_stat & MII_STAT_AN_DONE) != 0);

            *link = (*link) && (an_done);
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_serdes_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, TRUE indicates full duplex, FALSE indicates half.
 * Returns:
 *      SOC_E_XXX
 */

int
phy_serdes_duplex_set(int unit, soc_port_t port, int duplex)
{
    int rv;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    rv = duplex ? SOC_E_NONE : SOC_E_UNAVAIL;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes_duplex_set: u=%d p=%d duplex=%d rv=%d\n"),
              unit, port, duplex, rv));

    return rv;
}

/*
 * Function:
 *      phy_serdes_an_get
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

int
phy_serdes_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      ctrl, stat;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES_MII_CTRLr(unit, pc, &ctrl));

    *an = (ctrl & MII_CTRL_AE) != 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_MII_STATr(unit, pc, &stat));

    *an_done = (stat & MII_STAT_AN_DONE) != 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes_an_get: u=%d p=%d an=%d an_done=%d\n"),
              unit, port, *an, *an_done));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_serdes_adv_local_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 */
int
phy_serdes_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
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
        (MODIFY_SERDES_MII_ANAr(unit, pc, an_adv,
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
 *      phy_serdes_adv_local_get
 * Purpose:    
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:    
 *      SOC_E_XXX
 */

int
phy_serdes_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      an_adv;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_MII_ANAr(unit, pc, &an_adv));

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
_phy_serdes_sgmii_adv_remote_get(int unit, soc_port_t port,
                                soc_port_mode_t *mode)
{
    uint16              anlpa;
    soc_port_mode_t     duplex;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_MII_ANPr(unit, pc, &anlpa));

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
_phy_serdes_1000x_adv_remote_get(int unit, soc_port_t port,
                                soc_port_mode_t *mode)
{
    uint16              anlpa;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_SERDES_MII_ANPr(unit, pc, &anlpa));

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
 *      phy_serdes_adv_remote_get
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
phy_serdes_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16              fiber_status;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_SERDES_1000X_STAT1r(unit, pc, &fiber_status));

    if (fiber_status & SERDES_1000X_STAT1_SGMII_MODE) {
        SOC_IF_ERROR_RETURN
            (_phy_serdes_sgmii_adv_remote_get(unit, port, mode));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_serdes_1000x_adv_remote_get(unit, port, mode));
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
 *      phy_serdes_lb_set
 * Purpose:     
 *      Set the internal PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - Boolean: true = enable loopback, false = disable.
 * Returns:     
 *      SOC_E_XXX
 */ 
    
int
phy_serdes_lb_set(int unit, soc_port_t port, int enable)
{   
    uint16              ctrl;
    phy_ctrl_t  *pc;
    int                 rv;
    
    pc = INT_PHY_SW_STATE(unit, port);
    
    ctrl = (enable) ?MII_CTRL_LE : 0;

    rv = MODIFY_SERDES_MII_CTRLr(unit, pc,
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

int
phy_serdes_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_SERDES_MII_CTRLr(unit, pc, &ctrl));

    *enable = (ctrl & MII_CTRL_LE) != 0;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_serdes_interface_set
 * Purpose:
 *      Set the current operating mode of the internal PHY.
 *      (Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

int
phy_serdes_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    int                 rv;

    switch (pif) {
    case SOC_PORT_IF_MII:
    case SOC_PORT_IF_GMII:
    case SOC_PORT_IF_SGMII:
    case SOC_PORT_IF_NOCXN:
        rv = SOC_E_NONE;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_serdes_interface_set: u=%d p=%d pif=%d rv=%d\n"),
              unit, port, pif, rv));

    return rv;
}

/*
 * Function:
 *      phy_serdes_interface_get
 * Purpose:
 *      Get the current operating mode of the internal PHY.
 *      (Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_XXX
 */

int
phy_serdes_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    *pif = SOC_PORT_IF_GMII;

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_serdes_medium_config_get
 * Purpose:
 *      Get the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - (OUT) Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

int
phy_serdes_medium_config_get(int unit, soc_port_t port,
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    phy_ctrl_t    *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    switch (medium) {
    case SOC_PORT_MEDIUM_FIBER:
        sal_memcpy(cfg, &pc->fiber, sizeof (*cfg));
        return SOC_E_NONE;
    case SOC_PORT_MEDIUM_COPPER:
        return SOC_E_UNAVAIL;
    default:
        return SOC_E_PARAM;
    }
}

/*
 * Function:
 *      phy_serdes_medium_status
 * Purpose:
 *      Indicate the current active medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_FIBER
 * Returns:
 *      SOC_E_NONE
 */
int
phy_serdes_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *medium = SOC_PORT_MEDIUM_FIBER;

    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_serdes_master_set
 * Purpose:
 *      Configure PHY master mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - PHY master mode.
 * Returns:
 *      SOC_E_UNAVAIL
 */
int
phy_serdes_master_set(int unit, soc_port_t port, int master)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (master == SOC_PORT_MS_NONE) {
        return SOC_E_NONE;
    }
    return SOC_E_PARAM;
}

/*
 * Function:
 *      phy_serdes_master_get
 * Purpose:
 *      Get current master mode setting
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) BCM_PORT_MS_NONE
 * Returns:
 *      SOC_E_NONE
 */
int
phy_serdes_master_get(int unit, soc_port_t port, int *master)
{
    /* When in SGMII mode, the switch SerDes is always slave.  
     * However, we returns SOC_PORT_MS_NONE because SerDes driver
     * doesn't allow to change master mode.
     */

    *master = SOC_PORT_MS_NONE;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_serdes_reg_read
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
int
phy_serdes_reg_read(int unit, soc_port_t port, uint32 flags,
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
 *      phy_serdes_reg_write
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
int
phy_serdes_reg_write(int unit, soc_port_t port, uint32 flags,
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
 *      phy_serdes_reg_modify
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
int
phy_serdes_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data, uint32 phy_data_mask)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    SOC_IF_ERROR_RETURN
        (phy_reg_serdes_modify(unit, pc, reg_bank, reg_addr, data, mask));

    return SOC_E_NONE;
}


#else /* INCLUDE_PHY_SERDES */
typedef int _phy_serdes_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_SERDES */

