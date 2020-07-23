/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    phy.c
 * Purpose: Defines PHY driver routines, and standard 802.3 10/100/1000
 *      PHY interface routines.
 */


#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/phy.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyfege.h"

#define BCM5400_LOOPBACK_WAR

/* Variable:
 *  phy_drv_fe
 * Purpose:
 *  Phy driver callouts for a standard phy (IEEE 802.3)
 */

phy_driver_t phy_drv_fe = {
    "Fast Ethernet PHY Driver",
    phy_fe_init, 
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
    phy_fe_adv_local_set,
    phy_fe_adv_local_get, 
    phy_fe_adv_remote_get,
    phy_fe_ge_lb_set,
    phy_fe_ge_lb_get,
    phy_fe_interface_set, 
    phy_fe_interface_get, 
    phy_fe_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_fe_ge_medium_get,
    NULL,
    NULL
};

/*
 * Variable:
 *  phy_drv_ge
 * Purpose:
 *  Phy driver callouts for a Gigabit phy (IEEE 802.3)
 */

phy_driver_t phy_drv_ge = {
    "Gigabit PHY Driver",
    phy_ge_init, 
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
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_fe_ge_medium_get,
    NULL,                       /* phy_cable_diag */
    NULL                       /* phy_link_change */
};

/*
 * Function:    
 *  phy_fe_ge_reset
 * Purpose: 
 *  Reset PHY and wait for it to come out of reset.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_fe_ge_reset(int unit, soc_port_t port, void *user_arg)
{
    phy_ctrl_t          *pc;
    uint16      ctrl, tmp;
    soc_timeout_t   to;
    int         timeout = 0;

    COMPILER_REFERENCE(user_arg);

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &ctrl));

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_CTRLr(unit, pc, (ctrl | MII_CTRL_RESET)));

    soc_timeout_init(&to,
             soc_property_get(unit,
                      spn_PHY_RESET_TIMEOUT,
                      PHY_RESET_TIMEOUT_USEC), 1);

    do {
    SOC_IF_ERROR_RETURN
            (READ_PHYFEGE_MII_CTRLr(unit, pc, &tmp));
    if (soc_timeout_check(&to)) {
        timeout = 1;
        break;
    }
    } while ((tmp & MII_CTRL_RESET) != 0);

    if (timeout) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_fe_ge_reset: timeout on u=%d p=%d\n"),
                  unit, port));
    SOC_IF_ERROR_RETURN
            (WRITE_PHYFEGE_MII_CTRLr(unit, pc, ctrl));
    }

    return (SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_enable_set
 * Purpose: 
 *  Enable or disable the physical interface.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  enable - Boolean, true = enable PHY, false = disable.
 * Returns: 
 *  SOC_E_XXX
 * Notes:
 *  There does not seem to be a way to truly disable the external
 *  link on standard PHYs.  Isolate mode disables the MII side but
 *  not the wire side.  Super-isolate mode disables the MII and
 *  link pulses; however disabling MII causes duplex-switch hangs.
 *
 *  This code uses a PHY flag to keep track of enable state.
 *  This is important because linkscan copies the PHY enable state
 *  to the MAC enable state.
 */

int
phy_fe_ge_enable_set(int unit, soc_port_t port, int enable)
{
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    return (SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_enable_get
 * Purpose: 
 *  Return physical interface enable/disable state.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_fe_ge_enable_get(int unit, soc_port_t port, int *enable)
{
    *enable = !PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE);
    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_init
 * Purpose: 
 *  Initialize the PHY to a known good state.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 * Returns: 
 *  SOC_E_XXX
 * Notes: 
 *  No synchronization performed at this level. By default, phy set
 *  in autonegotiation mode, with maximum abilities advertised.
 */

int
phy_fe_init(int unit, soc_port_t port)
{
    uint16  mii_ana, mii_ctrl;
    phy_ctrl_t  *pc;

    /* Reset PHY */
    SOC_IF_ERROR_RETURN(soc_phy_reset(unit, port));

    pc = EXT_PHY_SW_STATE(unit, port);

    mii_ana = MII_ANA_HD_10 | MII_ANA_FD_10 | MII_ANA_HD_100 | 
          MII_ANA_FD_100 | MII_ANA_ASF_802_3;
    mii_ctrl = MII_CTRL_FD | MII_CTRL_SS_100 | MII_CTRL_AE | MII_CTRL_RAN;

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_CTRLr(unit, pc, mii_ctrl));
    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_ANAr(unit, pc, mii_ana));


    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_ge_init
 * Purpose: 
 *  Initialize the PHY (MII mode) to a known good state.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 * Returns: 
 *  SOC_E_XXX

 * Notes: 
 *  No synchronization performed at this level.
 */

int
phy_ge_init(int unit, soc_port_t port)
{
    uint16      mii_ctrl, mii_gb_ctrl;
    soc_port_if_t   pif;
    phy_ctrl_t          *pc;

    /* Reset PHY */
    SOC_IF_ERROR_RETURN(soc_phy_reset(unit, port));

    pc = EXT_PHY_SW_STATE(unit, port);

    mii_ctrl = MII_CTRL_FD | MII_CTRL_SS_1000 | MII_CTRL_AE | MII_CTRL_RAN;
    mii_gb_ctrl = MII_GB_CTRL_ADV_1000FD | MII_GB_CTRL_PT;

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_10B)) {
        pif = SOC_PORT_IF_TBI;
    } else {
        pif = SOC_PORT_IF_GMII;
    }

    SOC_IF_ERROR_RETURN(phy_ge_interface_set(unit, port, pif));

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_GB_CTRLr(unit, pc, mii_gb_ctrl));
    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_CTRLr(unit, pc, mii_ctrl));

#if defined(INCLUDE_PHY_54XX) && defined(BCM5400_LOOPBACK_WAR)
    /* Workaround for loop back w/5400; use a pseudo-random value */
    if (PHY_IS_BCM5400(pc)) {
        uint16 seed = (uint16) (sal_time_usecs() + port) & MII_MSSEED_SEED;

        /* Set up test reg/seed reg for external loopback */
        SOC_IF_ERROR_RETURN
            (WRITE_PHYFEGE_MII_TEST2r(unit, pc, MII_TEST2_MS_SEL));
        SOC_IF_ERROR_RETURN
            (WRITE_PHYFEGE_MII_MSSEEDr(unit, pc, seed));
    }
#endif

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_link_get
 * Purpose: 
 *  Determine the current link up/down status
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  link - (OUT) Boolean, true indicates link established.
 * Returns:
 *  SOC_E_XXX
 * Notes: 
 *  No synchronization performed at this level.
 */

int
phy_fe_ge_link_get(int unit, soc_port_t port, int *link)
{
    uint16      mii_ctrl, mii_stat;
    soc_timeout_t   to;
    phy_ctrl_t          *pc;

    *link = FALSE;      /* Default */
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }
   
    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_STATr(unit, pc, &mii_stat));

    if (!(mii_stat & MII_STAT_LA) || (mii_stat == 0xffff)) {
    /* mii_stat == 0xffff check is to handle removable PHY daughter cards */
        return SOC_E_NONE;
    }

    /* Link appears to be up; we are done if autoneg is off. */

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));

    if (!(mii_ctrl & MII_CTRL_AE)) {
    *link = TRUE;
    return SOC_E_NONE;
    }

    /*
     * If link appears to be up but autonegotiation is still in
     * progress, wait for it to complete.  For BCM5228, autoneg can
     * still be busy up to about 200 usec after link is indicated.  Also
     * continue to check link state in case it goes back down.
     */
    soc_timeout_init(&to, SOC_PHY_INFO(unit, port).an_timeout, 0);
    for (;;) {
    SOC_IF_ERROR_RETURN
            (READ_PHYFEGE_MII_STATr(unit, pc, &mii_stat));

    if (!(mii_stat & MII_STAT_LA)) {
        return SOC_E_NONE;
    }

    if (mii_stat & MII_STAT_AN_DONE) {
        break;
    }

    if (soc_timeout_check(&to)) {
        return SOC_E_BUSY;
    }
    }

    /* Return link state at end of polling */

    *link = ((mii_stat & MII_STAT_LA) != 0);

    return SOC_E_NONE;
}

/*
 * Function:     
 *    _phy_auto_negotiate_gcd (greatest common denominator).
 * Purpose:    
 *    Determine the current greatest common denominator between 
 *    two ends of a link
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *    speed - (OUT) greatest common speed.
 *    duplex - (OUT) greatest common duplex.
 *    link - (OUT) Boolean, true indicates link established.
 * Returns:    
 *    SOC_E_XXX
 * Notes: 
 *    No synchronization performed at this level.
 */

static int
_phy_auto_negotiate_gcd(int unit, soc_port_t port, int *speed, int *duplex)
{
    int        t_speed, t_duplex;
    uint16     mii_ana, mii_anp, mii_stat;
    uint16     mii_gb_stat, mii_esr, mii_gb_ctrl;
    phy_ctrl_t *pc;

    mii_gb_stat = 0;            /* Start off 0 */
    mii_gb_ctrl = 0;            /* Start off 0 */

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ANAr(unit, pc, &mii_ana));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ANPr(unit, pc, &mii_anp));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_STATr(unit, pc, &mii_stat));

    if (mii_stat & MII_STAT_ES) {    /* Supports extended status */
        /*
         * If the PHY supports extended status, check if it is 1000MB
         * capable.  If it is, check the 1000Base status register to see
         * if 1000MB negotiated.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHYFEGE_MII_ESRr(unit, pc, &mii_esr));

        if (mii_esr & (MII_ESR_1000_X_FD | MII_ESR_1000_X_HD | 
                       MII_ESR_1000_T_FD | MII_ESR_1000_T_HD)) {
            SOC_IF_ERROR_RETURN
                (READ_PHYFEGE_MII_GB_STATr(unit, pc, &mii_gb_stat));
            SOC_IF_ERROR_RETURN
                (READ_PHYFEGE_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));
        }
    }

    /*
     * At this point, if we did not see Gig status, one of mii_gb_stat or 
     * mii_gb_ctrl will be 0. This will cause the first 2 cases below to 
     * fail and fall into the default 10/100 cases.
     */

    mii_ana &= mii_anp;

    if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000FD) &&
        (mii_gb_stat & MII_GB_STAT_LP_1000FD)) {
        t_speed  = 1000;
        t_duplex = 1;
    } else if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000HD) &&
               (mii_gb_stat & MII_GB_STAT_LP_1000HD)) {
        t_speed  = 1000;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_100) {         /* [a] */
        t_speed = 100;
        t_duplex = 1;
    } else if (mii_ana & MII_ANA_T4) {            /* [b] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_HD_100) {        /* [c] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_10) {        /* [d] */
        t_speed = 10;
        t_duplex = 1 ;
    } else if (mii_ana & MII_ANA_HD_10) {        /* [e] */
        t_speed = 10;
        t_duplex = 0;
    } else {
        return(SOC_E_FAIL);
    }

    if (speed)  *speed  = t_speed;
    if (duplex)    *duplex = t_duplex;

    return(SOC_E_NONE);
}

/*
 * Function:     
 *    _phy_auto_negotiate_ew (Autoneg-ed mode with E@W on).
 * Purpose:    
 *    Determine autoneg-ed mode between
 *    two ends of a link
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *    speed - (OUT) greatest common speed.
 *    duplex - (OUT) greatest common duplex.
 *    link - (OUT) Boolean, true indicates link established.
 * Returns:    
 *    SOC_E_XXX
 * Notes: 
 *    No synchronization performed at this level.
 */

static int
_phy_auto_negotiate_ew(int unit, soc_port_t port, int *speed, int *duplex)
{
    int        t_speed, t_duplex;
    uint16     mii_assr;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ASSRr(unit, pc, &mii_assr));

    switch ((mii_assr >> 8) & 0x7) {
    case 0x7:
        t_speed = 1000;
        t_duplex = TRUE;
        break;
    case 0x6:
        t_speed = 1000;
        t_duplex = FALSE;
        break;
    case 0x5:
        t_speed = 100;
        t_duplex = TRUE;
        break;
    case 0x3:
        t_speed = 100;
        t_duplex = FALSE;
        break;
    case 0x2:
        t_speed = 10;
        t_duplex = TRUE;
        break;
    case 0x1:
        t_speed = 10;
        t_duplex = FALSE;
        break;
    default:
        t_speed = 0; /* 0x4 is 100BASE-T4 which is not supported */
        t_duplex = FALSE;
        break;
    }

    if (speed)  *speed  = t_speed;
    if (duplex)    *duplex = t_duplex;

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_duplex_set
 * Purpose: 
 *  Set the current duplex mode (forced).
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns: 
 *  SOC_E_XXX
 * Notes: 
 *  No synchronization performed at this level. Autonegotiation is 
 *  not manipulated. 
 */

int
phy_fe_ge_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16  mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));

    if (duplex) {
    mii_ctrl |= MII_CTRL_FD;
    } else {
    mii_ctrl &= ~MII_CTRL_FD;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_CTRLr(unit, pc, mii_ctrl));

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_duplex_get
 * Purpose: 
 *  Get the current operating duplex mode. If autoneg is enabled, 
 *  then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  duplex - (OUT) Boolean, TRUE indicates full duplex, FALSE 
 *      indicates half.
 * Returns: 
 *  SOC_E_XXX
 * Notes: 
 *  Returns a duplex of FALSE if autonegotiation is not complete.
 *  No synchronization performed at this level. Autonegotiation is 
 *  not manipulated. 
 */

int
phy_fe_ge_duplex_get(int unit, soc_port_t port, int *duplex)
{
    int     rv;
    uint16  mii_ctrl, mii_stat;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_STATr(unit, pc, &mii_stat));

    rv = SOC_E_NONE;
    if (mii_ctrl & MII_CTRL_AE) {     /* Auto-negotiation enabled */
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *duplex = FALSE;
        } else 
        if (IS_GE_PORT(unit, port)) {
            uint16 misc_ctrl;
            SOC_IF_ERROR_RETURN
                (phy_reg_ge_read(unit, pc, 0x00, 0x0007, 0x18, &misc_ctrl));
            /* First check for Ethernet@Wirespeed */
            if (misc_ctrl & (1U << 4)) {   /* Ethernet@Wirespeed enabled */
                rv = _phy_auto_negotiate_ew(unit, port, NULL, duplex);
            } else {
                rv = _phy_auto_negotiate_gcd(unit, port, NULL, duplex);
            }
        } else {
            rv = _phy_auto_negotiate_gcd(unit, port, NULL, duplex);
        }
    } else {                /* Auto-negotiation disabled */
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    }

    return(rv);
}

/*
 * Function:    
 *  phy_fe_ge_speed_set
 * Purpose: 
 *  Set the current operating speed (forced).
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  duplex - (OUT) Boolean, true indicates full duplex, false 
 *      indicates half.
 * Returns: 
 *  SOC_E_XXX
 * Notes: 
 *  No synchronization performed at this level. Autonegotiation is 
 *  not manipulated. 
 */

int
phy_fe_ge_speed_set(int unit, soc_port_t port, int speed)
{
    uint16  mii_ctrl;
    phy_ctrl_t  *pc;
    soc_pbmp_t pbmp_100fx;

    if (speed == 0) {
        return SOC_E_NONE;
    }

    /*
      * Get 100-FX ports from config.bcm.
      */
    pbmp_100fx = soc_property_get_pbmp(unit, spn_PBMP_FE_100FX, 0);
    if (SOC_PBMP_MEMBER(pbmp_100fx, port) && (speed != 100)) {
        return SOC_E_CONFIG;
    }
    
    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));

    mii_ctrl &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
    switch(speed) {
    case 10:
    mii_ctrl |= MII_CTRL_SS_10;
    break;
    case 100:
    mii_ctrl |= MII_CTRL_SS_100;
    break;
    case 1000:  
    mii_ctrl |= MII_CTRL_SS_1000;
    break;
    default:
    return(SOC_E_CONFIG);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_CTRLr(unit, pc, mii_ctrl));

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_speed_get
 * Purpose: 
 *  Get the current operating speed. If autoneg is enabled, 
 *  then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  duplex - (OUT) Boolean, true indicates full duplex, false 
 *      indicates half.
 * Returns: 
 *  SOC_E_XXX
 * Notes: 
 *  Returns a speed of 0 if autonegotiation is not complete.
 *  No synchronization performed at this level. Autonegotiation is 
 *  not manipulated. 
 */

int
phy_fe_ge_speed_get(int unit, soc_port_t port, int *speed)
{
    int     rv;
    uint16  mii_ctrl, mii_stat;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_STATr(unit, pc, &mii_stat));

    if (mii_ctrl & MII_CTRL_AE) {   /* Auto-negotiation enabled */
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            *speed = 0;
            rv = SOC_E_NONE;
        } else
        if (IS_GE_PORT(unit, port)) {
            uint16 misc_ctrl;
            SOC_IF_ERROR_RETURN
                (phy_reg_ge_read(unit, pc, 0x00, 0x0007, 0x18, &misc_ctrl));
            /* First check for Ethernet@Wirespeed */
            if (misc_ctrl & (1U << 4)) {   /* Ethernet@Wirespeed enabled */
                rv = _phy_auto_negotiate_ew(unit, port, speed, NULL);
            } else {
                rv = _phy_auto_negotiate_gcd(unit, port, speed, NULL);
            }
        } else {
            rv = _phy_auto_negotiate_gcd(unit, port, speed, NULL);
        }
    } else {                /* Auto-negotiation disabled */
    /*
     * Simply pick up the values we force in CTRL register.
     */
    switch(MII_CTRL_SS(mii_ctrl)) {
    case MII_CTRL_SS_10:
        *speed = 10;
        break;
    case MII_CTRL_SS_100:
        *speed = 100;
        break;
    case MII_CTRL_SS_1000:
        *speed = 1000;
        break;
    default:            /* Just pass error back */
        return(SOC_E_UNAVAIL);
    }
    rv = SOC_E_NONE;
    }

    return(rv);
}

/*
 * Function:    
 *  phy_fe_ge_master_get
 * Purpose: 
 *  Get the master mode for the PHY.  If mode is forced, then
 *  forced mode is returned; otherwise operating mode is returned.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  master - (OUT) SOC_PORT_MS_*
 * Returns: 
 *  SOC_E_XXX
 * Notes: 
 *  No synchronization performed at this level. Autonegotiation is 
 *  not manipulated. 
 */

int
phy_fe_ge_master_get(int unit, soc_port_t port, int *master)
{
    int an = 0, an_done = 0, speed = 0;
    uint16  mii_gb_ctrl, mii_gb_stat;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (IS_FE_PORT(unit, port)) {
        *master = SOC_PORT_MS_NONE;
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHYFEGE_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

        if (!(mii_gb_ctrl & MII_GB_CTRL_MS_MAN)) {
             *master = SOC_PORT_MS_AUTO;
            return SOC_E_NONE;
        }


        SOC_IF_ERROR_RETURN
            (phy_fe_ge_an_get(unit, port, &an, &an_done));

        SOC_IF_ERROR_RETURN
            (phy_fe_ge_speed_get(unit, port, &speed));

        /* 
         * Master/Slave Status check can be skipped if :
         *     - autoneg is off
         * or  - speed not 1000Mbps
         *
         * Here we just return the Manually configured result
         */
        if (!an || speed != 1000) {
            if (mii_gb_ctrl & MII_GB_CTRL_MS) {
                *master = SOC_PORT_MS_MASTER;
            } else {
                *master = SOC_PORT_MS_SLAVE;
            }
            return SOC_E_NONE;
        }

        /* Status check needed for 1000Mbps with autoneg */
        SOC_IF_ERROR_RETURN
            (READ_PHYFEGE_MII_GB_STATr(unit, pc, &mii_gb_stat));

        if (mii_gb_stat & MII_GB_STAT_MS_FAULT) {
            *master = SOC_PORT_MS_NONE;
        } else if (mii_gb_stat & MII_GB_STAT_MS) {
            *master = SOC_PORT_MS_MASTER;
        } else {
            *master = SOC_PORT_MS_SLAVE;
        }
    }

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_master_set
 * Purpose: 
 *  Set the master mode for the PHY.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  master - SOC_PORT_MS_*
 * Returns: 
 *  SOC_E_XXX
 * Notes: 
 *  No synchronization performed at this level. Autonegotiation is 
 *  not manipulated. 
 */

int
phy_fe_ge_master_set(int unit, soc_port_t port, int master)
{
    uint16  mii_gb_ctrl;
    phy_ctrl_t  *pc;

    if (IS_FE_PORT(unit, port)) {
    return(SOC_E_NONE);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

    switch (master) {
    case SOC_PORT_MS_SLAVE:
    mii_gb_ctrl |= MII_GB_CTRL_MS_MAN;
    mii_gb_ctrl &= ~MII_GB_CTRL_MS;
    break;
    case SOC_PORT_MS_MASTER:
    mii_gb_ctrl |= MII_GB_CTRL_MS_MAN;
    mii_gb_ctrl |= MII_GB_CTRL_MS;
    break;
    case SOC_PORT_MS_AUTO:
    mii_gb_ctrl &= ~MII_GB_CTRL_MS_MAN;
    break;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_GB_CTRLr(unit, pc, mii_gb_ctrl));

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_an_set
 * Purpose: 
 *  Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  an   - Boolean, if true, auto-negotiation is enabled 
 *      (and/or restarted). If false, autonegotiation is disabled.
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_fe_ge_an_set(int unit, soc_port_t port, int an)
{
    uint16  mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));

    if (an) {
    mii_ctrl |= MII_CTRL_AE | MII_CTRL_RAN;
    } else {
    mii_ctrl &= ~(MII_CTRL_AE | MII_CTRL_RAN);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_CTRLr(unit, pc, mii_ctrl));

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_an_get
 * Purpose: 
 *  Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  an   - (OUT) if true, auto-negotiation is enabled.
 *  an_done - (OUT) if true, auto-negotiation is complete. This
 *          value is undefined if an == false.
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_fe_ge_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16  mii_ctrl, mii_stat;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_STATr(unit, pc, &mii_stat));

    *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;
    *an_done = (mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_adv_local_set
 * Purpose: 
 *  Set the current advertisement for auto-negotiation.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  mode - Port mode mask indicating supported options/speeds.
 * Returns: 
 *  SOC_E_XXX
 */

int 
phy_fe_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    uint16  mii_adv, mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    mii_adv = MII_ANA_ASF_802_3;

    if (mode & SOC_PM_10MB_HD)  mii_adv |= MII_ANA_HD_10;
    if (mode & SOC_PM_10MB_FD)  mii_adv |= MII_ANA_FD_10;
    if (mode & SOC_PM_100MB_HD) mii_adv |= MII_ANA_HD_100;
    if (mode & SOC_PM_100MB_FD) mii_adv |= MII_ANA_FD_100;

    switch(mode & SOC_PM_PAUSE) {
    case 0:
    break;
    case SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX:
    mii_adv |= MII_ANA_PAUSE;
    break;
    default:
    return(SOC_E_UNAVAIL);          /* Parameter error */
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_ANAr(unit, pc, mii_adv));

    /* Restart auto-neg if enabled */

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));

    if (mii_ctrl & MII_CTRL_AE) {
    SOC_IF_ERROR_RETURN
            (WRITE_PHYFEGE_MII_CTRLr(unit, pc, mii_ctrl | MII_CTRL_RAN));
    }

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_adv_local_get
 * Purpose: 
 *  Get the current advertisement for auto-negotiation.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_fe_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16  mii_ana;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ANAr(unit, pc, &mii_ana));

    if (mii_ana & MII_ANA_HD_10)    *mode |= SOC_PM_10MB_HD;
    if (mii_ana & MII_ANA_FD_10)    *mode |= SOC_PM_10MB_FD;
    if (mii_ana & MII_ANA_HD_100)   *mode |= SOC_PM_100MB_HD;
    if (mii_ana & MII_ANA_FD_100)   *mode |= SOC_PM_100MB_FD;
    if (mii_ana & MII_ANA_PAUSE)    *mode |= SOC_PM_PAUSE;

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_adv_remote_get
 * Purpose: 
 *  Get link partner's current advertisement for auto-negotiation.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *  Returns an empty mask if autonegotiation is not complete.
 */

int
phy_fe_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16  mii_anp, mii_ctrl, mii_stat;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    *mode = 0;

    /* ORDER: must read control, then stat, then anp */

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_STATr(unit, pc, &mii_stat));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ANPr(unit, pc, &mii_anp));

    if (!(mii_ctrl & MII_CTRL_AE)) {
    return(SOC_E_DISABLED);
    } else if (!(mii_stat & MII_STAT_AN_DONE)) {
    return(SOC_E_NONE);
    }

    /* Figure out what it means */

    if (mii_anp & MII_ANA_HD_10)    *mode |= SOC_PM_10MB_HD;
    if (mii_anp & MII_ANA_FD_10)    *mode |= SOC_PM_10MB_FD;
    if (mii_anp & MII_ANA_HD_100)   *mode |= SOC_PM_100MB_HD;
    if (mii_anp & MII_ANA_FD_100)   *mode |= SOC_PM_100MB_FD;

    if (mii_anp & MII_ANA_PAUSE) {
    *mode |= SOC_PM_PAUSE;
    }

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_ge_adv_local_set
 * Purpose: 
 *  Set the current advertisement for auto-negotiation.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  mode - Port mode mask indicating supported options/speeds.
 * Returns: 
 *  SOC_E_XXX
 */

int 
phy_ge_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    uint16  mii_ctrl, mii_adv, mii_gb_ctrl;
    phy_ctrl_t  *pc;

    pc          = EXT_PHY_SW_STATE(unit, port);
    mii_adv = MII_ANA_ASF_802_3;

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_GB_CTRLr(unit, pc,  &mii_gb_ctrl));

    mii_gb_ctrl &= ~(MII_GB_CTRL_ADV_1000HD | MII_GB_CTRL_ADV_1000FD);

    if (mode & SOC_PM_10MB_HD)  mii_adv |= MII_ANA_HD_10;
    if (mode & SOC_PM_10MB_FD)  mii_adv |= MII_ANA_FD_10;
    if (mode & SOC_PM_100MB_HD) mii_adv |= MII_ANA_HD_100;
    if (mode & SOC_PM_100MB_FD) mii_adv |= MII_ANA_FD_100;
    if (mode & SOC_PM_1000MB_HD) mii_gb_ctrl |= MII_GB_CTRL_ADV_1000HD;
    if (mode & SOC_PM_1000MB_FD) mii_gb_ctrl |= MII_GB_CTRL_ADV_1000FD;

    if ((mode & SOC_PM_PAUSE) == SOC_PM_PAUSE) {
        /* Advertise symmetric pause */
        mii_adv |= MII_ANA_PAUSE;
    } else {
        /*
         * For Asymmetric pause, 
         *   if (Bit 10)
         *       then pause frames flow toward the Transceiver
         *       else pause frames flow toward link partner.
         */
        if (mode & SOC_PM_PAUSE_TX) {
            mii_adv |= MII_ANA_ASYM_PAUSE;
        } else if (mode & SOC_PM_PAUSE_RX) {
            mii_adv |= MII_ANA_ASYM_PAUSE;
            mii_adv |= MII_ANA_PAUSE;
        }
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_ANAr(unit, pc, mii_adv));
    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_GB_CTRLr(unit, pc, mii_gb_ctrl));

    /* Restart auto-neg if enabled */

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));

    if (mii_ctrl & MII_CTRL_AE) {
    SOC_IF_ERROR_RETURN
            (WRITE_PHYFEGE_MII_CTRLr(unit, pc, mii_ctrl | MII_CTRL_RAN));
    }

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_ge_adv_local_get
 * Purpose: 
 *  Get the current advertisement for auto-negotiation.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_ge_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16  mii_ana, mii_gb_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ANAr(unit, pc, &mii_ana));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

    *mode = 0;

    if (mii_ana & MII_ANA_HD_10)    *mode |= SOC_PM_10MB_HD;
    if (mii_ana & MII_ANA_FD_10)    *mode |= SOC_PM_10MB_FD;
    if (mii_ana & MII_ANA_HD_100)   *mode |= SOC_PM_100MB_HD;
    if (mii_ana & MII_ANA_FD_100)   *mode |= SOC_PM_100MB_FD;

    if (mii_ana & MII_ANA_ASYM_PAUSE) {
    if (mii_ana & MII_ANA_PAUSE) {
        *mode |= SOC_PM_PAUSE_RX;
    } else {
        *mode |= SOC_PM_PAUSE_TX;
    }
    } else if (mii_ana & MII_ANA_PAUSE) {
    *mode |= SOC_PM_PAUSE;
    }

    /* GE Specific values */

    if (mii_gb_ctrl & MII_GB_CTRL_ADV_1000FD)   *mode |= SOC_PM_1000MB_FD;
    if (mii_gb_ctrl & MII_GB_CTRL_ADV_1000HD)   *mode |= SOC_PM_1000MB_HD;

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_ge_adv_remote_get
 * Purpose: 
 *  Get partner's current advertisement for auto-negotiation.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *  Returns an empty mask if autonegotiation is not complete.
 */

int
phy_ge_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16  mii_ctrl, mii_stat, mii_gb_stat, mii_anp;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    *mode = 0;

    /* ORDER: must read control, then stat, then anp */

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_STATr(unit, pc, &mii_stat));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ANPr(unit, pc, &mii_anp));
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_GB_STATr(unit, pc, &mii_gb_stat));

    if (!(mii_ctrl & MII_CTRL_AE)) {
    return(SOC_E_DISABLED);
    } else if (!(mii_stat & MII_STAT_AN_DONE)) {
    return(SOC_E_NONE);
    }

    /* Figure out what it means */

    if (mii_anp & MII_ANA_HD_10)    *mode |= SOC_PM_10MB_HD;
    if (mii_anp & MII_ANA_FD_10)    *mode |= SOC_PM_10MB_FD;
    if (mii_anp & MII_ANA_HD_100)   *mode |= SOC_PM_100MB_HD;
    if (mii_anp & MII_ANA_FD_100)   *mode |= SOC_PM_100MB_FD;

    if (mii_gb_stat & MII_GB_STAT_LP_1000FD)    *mode |= SOC_PM_1000MB_FD;
    if (mii_gb_stat & MII_GB_STAT_LP_1000HD)    *mode |= SOC_PM_1000MB_HD;

    if (mii_anp & MII_ANA_ASYM_PAUSE) {
    if (mii_anp & MII_ANA_PAUSE) {
        *mode |= SOC_PM_PAUSE_RX;
    } else {
        *mode |= SOC_PM_PAUSE_TX;
    }
    } else if (mii_anp & MII_ANA_PAUSE) {
    *mode |= SOC_PM_PAUSE;
    }

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_lb_set
 * Purpose: 
 *  Set the local PHY loopback mode.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  loopback - Boolean: true = enable loopback, false = disable.
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_fe_ge_lb_set(int unit, soc_port_t port, int enable)
{
    uint16  mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));

    mii_ctrl &= ~MII_CTRL_LE;
    mii_ctrl |= enable ? MII_CTRL_LE : 0;

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_CTRLr(unit, pc, mii_ctrl));

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ge_lb_get
 * Purpose: 
 *  Get the local PHY loopback mode.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_fe_ge_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16  mii_ctrl;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_CTRLr(unit, pc, &mii_ctrl));

    *enable = (mii_ctrl & MII_CTRL_LE) ? 1 : 0;

    return(SOC_E_NONE);
}

/*
 * Function:
 *  phy_fe_interface_set
 * Purpose:
 *  Set the current operating mode of the PHY (SOC_PORT_IF_*)
 *  For example: TBI or MII/GMII.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  pif - one of SOC_PORT_IF_*
 * Returns:
 *  SOC_E_XXX
 */

int
phy_fe_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    switch (pif) {
    case SOC_PORT_IF_MII:
    case SOC_PORT_IF_NOCXN:
    break;
    default:
    return SOC_E_UNAVAIL;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *  phy_fe_interface_get
 * Purpose:
 *  Get the current operating mode of the PHY.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  pif - (OUT) one of SORT_PORT_IF_*
 * Returns:
 *  SOC_E_XXX
 */

int
phy_fe_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_MII;

    return(SOC_E_NONE);
}

/*
 * Function:
 *  phy_ge_interface_set
 * Purpose:
 *  Set the current operating mode of the PHY.
 *  (Pertaining to the MAC/PHY interface, not the line interface).
 *      For example: TBI or MII/GMII.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  pif - one of SOC_PORT_IF_*
 * Returns:
 *  SOC_E_XXX
 */

int
phy_ge_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    uint16      mii_ecr;
    int             mii;            /* MII if true, TBI otherwise */
    phy_ctrl_t         *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch (pif) {
    case SOC_PORT_IF_MII:
    case SOC_PORT_IF_GMII:
    case SOC_PORT_IF_SGMII:
        mii = TRUE;
        break;
    case SOC_PORT_IF_NOCXN:
    return (SOC_E_NONE);
    case SOC_PORT_IF_TBI:
        mii = FALSE;
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ECRr(unit, pc, &mii_ecr));

    if (mii) {
        mii_ecr &= ~MII_ECR_10B;
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_10B);
    } else {
        mii_ecr |= MII_ECR_10B;
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_10B);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHYFEGE_MII_ECRr(unit, pc, mii_ecr));

    return(SOC_E_NONE);
}

/*
 * Function:
 *  phy_ge_interface_get
 * Purpose:
 *  Get the current operating mode of the PHY.
 *  (Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *  SOC_E_XXX
 */

int
phy_ge_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_10B)) {
        *pif = SOC_PORT_IF_TBI;
    } else {
        *pif = SOC_PORT_IF_GMII;
    }

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_fe_ability_get
 * Purpose: 
 *  Get the abilities of the local phy.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *  SOC_E_XXX
 */

int
phy_fe_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16  mii_status;

    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    *mode = SOC_PM_LB_PHY | SOC_PM_MII | SOC_PM_PAUSE;  /* No PAUSE_ASYMM */
    *mode |= SOC_PM_AN;
    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_STATr(unit, pc, &mii_status));

    if (mii_status & MII_STAT_100_T4)       *mode |= SOC_PM_100MB_FD;
    if (mii_status & MII_STAT_FD_100_T2)    *mode |= SOC_PM_100MB_FD;
    if (mii_status & MII_STAT_HD_100_T2)    *mode |= SOC_PM_100MB_HD;
    if (mii_status & MII_STAT_FD_10)        *mode |= SOC_PM_10MB_FD;
    if (mii_status & MII_STAT_HD_10)        *mode |= SOC_PM_10MB_HD;
    if (mii_status & MII_STAT_FD_100)       *mode |= SOC_PM_100MB_FD;
    if (mii_status & MII_STAT_HD_100)       *mode |= SOC_PM_100MB_HD;

    return(SOC_E_NONE);
}

/*
 * Function:    
 *  phy_ge_ability_get
 * Purpose: 
 *  Get the abilities of the local gigabit phy.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_ge_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16  mii_esr;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
    (phy_fe_ability_get(unit, port, mode));

    *mode |= (SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM | SOC_PM_GMII | SOC_PM_SGMII);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ESRr(unit, pc, &mii_esr));

    

    if (mii_esr & MII_ESR_1000_T_FD)    *mode |= SOC_PM_1000MB_FD;
    if (mii_esr & MII_ESR_1000_T_HD)    *mode |= SOC_PM_1000MB_HD;
    if (mii_esr & MII_ESR_1000_X_FD)    *mode |= SOC_PM_1000MB_FD;
    if (mii_esr & MII_ESR_1000_X_HD)    *mode |= SOC_PM_1000MB_HD;

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_fe_ge_medium_get
 * Description:
 *      Get the currently chosen medium for the dual-medium PHY
 * Parameters:
 *      unit    -- Device number
 *      port    -- Port number
 *      medium  -- (Out) One of
 *                 SOC_PORT_MEDIUM_NONE  
 *                 SOC_PORT_MEDIUM_COPPER
 *                 SOC_PORT_MEDIUM_FIBER
 *
 * Return value:
 *      SOC_E_NONE
 *      SOC_E_PARAM
 *      SOC_E_UNAVAIL
 */
int
phy_fe_ge_medium_get(int unit, soc_port_t port,
                    soc_port_medium_t *medium)
{
    int rv;
    soc_pbmp_t pbmp_100fx;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (medium != NULL) {
        /*
          * Get 100-FX ports from config.bcm.
          */
        pbmp_100fx = soc_property_get_pbmp(unit, spn_PBMP_FE_100FX, 0);
        if (SOC_PBMP_MEMBER(pbmp_100fx, port)) {
            *medium = SOC_PORT_MEDIUM_FIBER;
            return SOC_E_NONE;
        }

        *medium = SOC_PORT_MEDIUM_COPPER;
        rv = SOC_E_NONE;
    } else {    
        rv = SOC_E_PARAM;
    }
    
    return (rv);
}

/*
 * Function:
 *      phy_fe_ge_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      port    - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 */

int
phy_fe_ge_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    uint16       mii_adv, mii_gb_ctrl;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    mii_adv     = MII_ANA_ASF_802_3;

    if (ability->speed_half_duplex & SOC_PA_SPEED_10MB)  {
        mii_adv |= MII_ANA_HD_10;
    }
    if (ability->speed_half_duplex & SOC_PA_SPEED_100MB) {
        mii_adv |= MII_ANA_HD_100;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_10MB)  {
        mii_adv |= MII_ANA_FD_10;
    }
    if (ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
        mii_adv |= MII_ANA_FD_100;
    }

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        mii_adv |= MII_ANA_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        mii_adv |= MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        mii_adv |= MII_ANA_PAUSE;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHYFEGE_MII_ANAr(unit, pc, mii_adv,
        MII_ANA_PAUSE|MII_ANA_ASYM_PAUSE|MII_ANA_FD_100|MII_ANA_HD_100|
        MII_ANA_FD_10|MII_ANA_HD_10|MII_ANA_ASF_802_3));

    if (IS_GE_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN
            (READ_PHYFEGE_MII_GB_CTRLr(unit, pc,  &mii_gb_ctrl));

        mii_gb_ctrl &= ~(MII_GB_CTRL_ADV_1000HD | MII_GB_CTRL_ADV_1000FD);
        mii_gb_ctrl |= MII_GB_CTRL_PT; /* repeater / switch port */

        if (ability->speed_half_duplex & SOC_PA_SPEED_1000MB) {
            mii_gb_ctrl |= MII_GB_CTRL_ADV_1000HD;
        }
        if (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
            mii_gb_ctrl |= MII_GB_CTRL_ADV_1000FD;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHYFEGE_MII_GB_CTRLr(unit, pc, mii_gb_ctrl,
            MII_GB_CTRL_PT|MII_GB_CTRL_ADV_1000FD|MII_GB_CTRL_ADV_1000HD));
    }

    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_fe_ge_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      port    - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

int
phy_fe_ge_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    uint16      mii_ana;
    uint16      mii_gb_ctrl;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ANAr(unit, pc, &mii_ana));

    sal_memset(ability, 0, sizeof(*ability));

    if (mii_ana & MII_ANA_HD_10) {
       ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
    }
    if (mii_ana & MII_ANA_HD_100) {
      ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
    }
    if (mii_ana & MII_ANA_FD_10) {
      ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
    }
    if (mii_ana & MII_ANA_FD_100) {
      ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
    }

    switch (mii_ana & (MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE)) {
        case MII_ANA_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANA_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    /* GE Specific values */
    if (IS_GE_PORT(unit, port)) {
         SOC_IF_ERROR_RETURN
             (READ_PHYFEGE_MII_GB_CTRLr(unit, pc, &mii_gb_ctrl));

         if (mii_gb_ctrl & MII_GB_CTRL_ADV_1000HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
         }
         if (mii_gb_ctrl & MII_GB_CTRL_ADV_1000FD) {
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_fe_ge_ability_remote_get
 * Purpose:
 *      Get partners current advertisement for auto-negotiation.
 * Parameters:
 *      unit    - StrataSwitch unit #.
 *      port    - StrataSwitch port #.
 *      ability - Port ability indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The remote advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. If Autonegotiation is
 *      disabled or in progress, this routine will return an error.
 */

int
phy_fe_ge_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t       *pc;
    uint16            mii_stat;
    uint16            mii_anp;
    uint16            mii_gb_stat;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_STATr(unit, pc, &mii_stat));

    sal_memset(ability, 0, sizeof(*ability));

    if ((mii_stat & (MII_STAT_AN_DONE | MII_STAT_LA))
                   == (MII_STAT_AN_DONE | MII_STAT_LA)) {
        /* Decode remote advertisement only when link is up and autoneg is
         * completed.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHYFEGE_MII_ANPr(unit, pc, &mii_anp));

        if (mii_anp & MII_ANA_HD_10) {
            ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
        }
        if (mii_anp & MII_ANA_HD_100) {
            ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
        }
        if (mii_anp & MII_ANA_FD_10) {
           ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        }
        if (mii_anp & MII_ANA_FD_100) {
           ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        }

        switch (mii_anp & (MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE)) {
            case MII_ANA_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANA_ASYM_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX;
                break;
            case MII_ANA_PAUSE | MII_ANA_ASYM_PAUSE:
                ability->pause = SOC_PA_PAUSE_RX;
                break;
       }

        /* GE Specific values */
       if (IS_GE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (READ_PHYFEGE_MII_GB_STATr(unit, pc, &mii_gb_stat));

            if (mii_gb_stat & MII_GB_STAT_LP_1000HD) {
            ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
            }
            if (mii_gb_stat & MII_GB_STAT_LP_1000FD) {
                ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
            }
        }

    } else {
        /* Simply return local abilities */
        SOC_IF_ERROR_RETURN
                (phy_fe_ge_ability_advert_get(unit, port, ability));
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_fe_ge_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return SOC_E_NONE;
}

/*
 * Function:
 *  phy_fe_cable_diag
 * Purpose:
 *  Run 5248 cable diagnostics
 * Parameters:
 *  unit - device number
 *  port - port number
 *  status - (OUT) cable diagnotic status structure
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_fe_cable_diag(int unit, soc_port_t port,
                soc_port_cable_diag_t *status)
{

    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *      phy_ge_reg_read
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
phy_ge_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint32               reg_flags;
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);

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
 *      phy_ge_reg_write
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
phy_ge_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint32               reg_flags;
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);


    pc   = EXT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);

    reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_ge_write(unit, pc, reg_flags, reg_bank, reg_addr, data);
}

/*
 * Function:
 *      phy_ge_reg_modify
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
phy_ge_reg_modify(int unit, soc_port_t port, uint32 flags,
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

    pc   = EXT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);
    mask = (uint16) (phy_data_mask & 0x0000FFFF);

    reg_flags = SOC_PHY_REG_FLAGS(phy_reg_addr);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_ge_modify(unit, pc, reg_flags, reg_bank,
                             reg_addr, data, mask);
}

