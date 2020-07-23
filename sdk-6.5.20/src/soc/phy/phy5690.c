/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *	phy5690.c
 * Purpose:
 *	Fiber driver for 5690 using internal QuadSerdes PHY.
 *
 * For BCM5690, the internal GMACC core of the GPIC should always be
 * configured to run in SGMII mode.  TBI mode is not supported between
 * the GMACC and the internal SERDES module.
 * 
 * When operating with an external PHY, this driver is not used.
 * However the speed/duplex of the internal PHY must be programmed to
 * match the MAC and external PHY settings so the data can pass through.
 * This file supplies some routines to allow mac.c to accomplish this
 * (think of the internal PHY as part of the MAC in this case):
 *
 *	_phy_5690_notify_duplex
 *	_phy_5690_notify_speed
 *	_phy_5690_notify_stop
 *	_phy_5690_notify_resume
 *
 * CMIC MIIM operations can be performed to the internal register set
 * using internal MDIO address (PORT_TO_PHY_ADDR_INT), and an external
 * PHY register set (such as BCM5424/34/64) can be programmed using the
 * external MDIO address (PORT_TO_PHY_ADDR).
 *
 * MDIO accesses to the internal PHY are not modeled on Quickturn.
 */
#if defined(BCM_5690) && defined(BCM_ESW_SUPPORT)

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
#include "phy5690.h"

#define PHY_5690_AN_STATE_ACCESS_INTERVAL 60000 /* 60000 Usec */

#define PHY_5690_AN_TIME(_pc) \
	((_pc)->an_state_time)
#define PHY_5690_AN_VALID(_pc) \
	((_pc)->an_state_valid)
#define PHY_5690_SGMII_STATE(_pc) \
	((_pc)->sgmii_an_state)

#define PHY_5690_AN_RECORD_TIME(_pc)  \
	((_pc)->an_state_time) = sal_time_usecs()
#define PHY_5690_AN_RECORD_STATE(_pc, _state) \
	((_pc)->sgmii_an_state) = (_state)
#define PHY_5690_AN_VALID_SET(_pc) \
	((_pc)->an_state_valid) = TRUE
#define PHY_5690_AN_VALID_RESET(_pc) \
	((_pc)->an_state_valid) = FALSE

#define	PHY_5690_AN_ACCESS_TIMEOUT(_pc) \
	((uint32)SAL_USECS_SUB(sal_time_usecs(), PHY_5690_AN_TIME((_pc))) \
		>= PHY_5690_AN_STATE_ACCESS_INTERVAL)

STATIC int
_phy_5690_notify_resume(int unit, soc_port_t port, uint32 data);
STATIC int
_phy_5690_notify_stop(int unit, soc_port_t port, uint32 data);
STATIC int
_phy_5690_notify_interface(int unit, soc_port_t port, uint32 data);
STATIC int
_phy_5690_notify_duplex(int unit, soc_port_t port, uint32 data);
STATIC int
_phy_5690_notify_speed(int unit, soc_port_t port, uint32 data); 

/***********************************************************************
 *
 * FIBER DRIVER ROUTINES
 */

/*
 * Function:
 *	phy_5690_init
 * Purpose:	
 *	Initialize 5690 internal PHY driver
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 * Returns:	
 *	SOC_E_NONE
 */

STATIC int
phy_5690_init(int unit, soc_port_t port)
{
    uint16		ctrl;
    phy_ctrl_t         *pc;

    pc       = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_5690_init: u=%d p=%d\n"),
              unit, port));

    if (soc_feature(unit, soc_feature_phy_5690_link_war)) {
	PHY_5690_AN_VALID_RESET(pc);
    }

    /*
     * The internal SERDES PHY's reset bit must be held high for at
     * least 1 usec and is not self-clearing.
     */

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc,
		       QS_MII_CTRL_REG, &ctrl));

    ctrl |= QS_MII_CTRL_RESET;

    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_MII_CTRL_REG, ctrl));

    sal_usleep(1);

    ctrl = QS_MII_CTRL_SS_1000 | QS_MII_CTRL_FD;

    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_MII_CTRL_REG, ctrl));

    sal_usleep(10000);

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, &ctrl));
    ctrl &= ~QS_SGMII_TBI_LOOP;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, ctrl));
    
    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_SGMII_CTRL2_REG, 
                      (PHY_COPPER_MODE(unit, port) ? 0 : QS_SGMII_FIBER_MODE)));

    return SOC_E_NONE;
}


/*
 * Function:
 *	_phy_5690_war_link_get
 * Purpose:
 *	Perform Link get work-around
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	link - (OUT) Boolean, true indicates link established.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	This is the workaround for GNATS 3711, QS_SGMII_STAT_REG does
 *	not properly reflect the link up/down state.  However, it is
 *	possible to divine the true link state using the autonegotiation
 *	state debug register, in combination with its latching behavior
 *	and certain timing requirements.
 */

STATIC int
_phy_5690_war_link_get(int unit, soc_port_t port, int *link)
{
    uint16		sgmii_an_state;
    uint16		sgmii_misc_ctrl;
    uint16		sgmii_stat;
    phy_ctrl_t         *pc;

    pc       = INT_PHY_SW_STATE(unit, port);

    if ((PHY_5690_AN_ACCESS_TIMEOUT(pc)) || 
	(!PHY_5690_AN_VALID(pc))) {
        SOC_IF_ERROR_RETURN
	    (READ_PHY_REG(unit, pc, QS_SGMII_STAT_REG, &sgmii_stat));

        sgmii_an_state = sgmii_stat & 0x0ff0;

        *link = ((sgmii_an_state == 0x0770 || sgmii_an_state == 0x0880) &&
		 PHY_5690_AN_VALID(pc));

        /* Get a snapshot of AN state into QS_SGMII_STAT_REG */

        if (!(*link)) {
            SOC_IF_ERROR_RETURN
		(READ_PHY_REG(unit, pc,
			       QS_SGMII_MISC_CTRL_REG, &sgmii_misc_ctrl));
            sgmii_misc_ctrl &= ~0x0010;
            SOC_IF_ERROR_RETURN
		(WRITE_PHY_REG(unit, pc,
				QS_SGMII_MISC_CTRL_REG, sgmii_misc_ctrl));
            sgmii_misc_ctrl |= 0x0010;
            SOC_IF_ERROR_RETURN
		(WRITE_PHY_REG(unit, pc,
				QS_SGMII_MISC_CTRL_REG, sgmii_misc_ctrl));
            sgmii_an_state = 0;
        }

        PHY_5690_AN_RECORD_TIME(pc);
        PHY_5690_AN_RECORD_STATE(pc, sgmii_an_state);
        PHY_5690_AN_VALID_SET(pc);
    } else {
        sgmii_an_state = PHY_5690_SGMII_STATE(pc);

        *link = (sgmii_an_state == 0x0770 || sgmii_an_state == 0x0880);

        /* Check if link went down */

        if (*link) {
            SOC_IF_ERROR_RETURN
		(READ_PHY_REG(unit, pc,
			       QS_SGMII_STAT_REG, &sgmii_stat));

            sgmii_an_state = sgmii_stat & 0x0ff0;

            *link = (sgmii_an_state == 0x0770 || sgmii_an_state == 0x0880);

            PHY_5690_AN_RECORD_STATE(pc, sgmii_an_state);
        }
    }

    return SOC_E_NONE;
}


/*
 * Function: 	
 *	phy_5690_link_get
 * Purpose:	
 *	Determine the current link up/down status
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	link - (OUT) Boolean, true indicates link established.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	MII_STATUS bit 2 reflects link state.
 *	This bit gets set immediately when the link is plugged in,
 *	before autoneg completes.
 *	Energy control (MISC_CONTROL bit 3) is not used.
 */

STATIC int
phy_5690_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t         *pc;
    uint16		mii_stat;

    pc       = INT_PHY_SW_STATE(unit, port);
 
    if (soc_feature(unit, soc_feature_phy_5690_link_war)) {
       return (_phy_5690_war_link_get(unit, port, link));
    }

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_MII_STAT_REG, &mii_stat));

    *link = ((mii_stat & QS_MII_STAT_LA) != 0);


    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_enable_set
 * Purpose:	
 *	Enable or disable the physical interface.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	enable - Boolean, true = enable PHY, false = disable.
 * Returns:	
 *	SOC_E_XXX
 * Notes:
 *	The PHY is held in reset when disabled.  Also, the
 *	MISC_CONTROL.IDDQ bit must be set to 1 so the remote partner
 *	sees link down.
 */

STATIC int
phy_5690_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    uint16	misc_ctrl;

    pc       = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_5690_enable_set: u=%d p=%d en=%d\n"),
              unit, port, enable));

    if (enable) {
	PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
	SOC_IF_ERROR_RETURN
	    (READ_PHY_REG(unit, pc,
			   QS_SGMII_MISC_CTRL_REG, &misc_ctrl));
	misc_ctrl &= ~QS_SGMII_IDDQ_MODE;
	SOC_IF_ERROR_RETURN
	    (WRITE_PHY_REG(unit, pc,
			    QS_SGMII_MISC_CTRL_REG, misc_ctrl));
	SOC_IF_ERROR_RETURN
	    (_phy_5690_notify_resume(unit, port, PHY_STOP_PHY_DIS));
    } else {
	PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
	SOC_IF_ERROR_RETURN
	    (_phy_5690_notify_stop(unit, port, PHY_STOP_PHY_DIS));
	SOC_IF_ERROR_RETURN
	    (READ_PHY_REG(unit, pc,
			   QS_SGMII_MISC_CTRL_REG, &misc_ctrl));
	misc_ctrl |= QS_SGMII_IDDQ_MODE;
	SOC_IF_ERROR_RETURN
	    (WRITE_PHY_REG(unit, pc,
			    QS_SGMII_MISC_CTRL_REG, misc_ctrl));
    }

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_enable_get
 * Purpose:	
 *	Return physical interface enable/disable state.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:
 *	SOC_E_XXX
 */

STATIC int
phy_5690_enable_get(int unit, soc_port_t port, int *enable)
{
    *enable = !PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE);

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_5690_duplex_set
 * Purpose:	
 *	Set the current duplex mode (forced).
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex - Boolean, TRUE indicates full duplex, FALSE indicates half.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_duplex_set(int unit, soc_port_t port, int duplex)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    return (duplex ? SOC_E_NONE : SOC_E_UNAVAIL);
}

/*
 * Function: 	
 *	phy_5690_duplex_get
 * Purpose:	
 *	Get the current operating duplex mode. If autoneg is enabled, 
 *	then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex - (OUT) Boolean, true indicates full duplex, false 
 *		indicates half.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_duplex_get(int unit, soc_port_t port, int *duplex)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *duplex = TRUE;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_speed_set
 * Purpose:	
 *	Set the current operating speed (forced).
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex - (OUT) Boolean, true indicates full duplex, false 
 *		indicates half.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_speed_set(int unit, soc_port_t port, int speed)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (speed == 0 || speed == 1000) {
	return SOC_E_NONE;
    }

    return SOC_E_CONFIG;
}

/*
 * Function: 	
 *	phy_5690_speed_get
 * Purpose:	
 *	Get the current operating speed. If autoneg is enabled, 
 *	then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex - (OUT) Boolean, true indicates full duplex, false 
 *		indicates half.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_speed_get(int unit, soc_port_t port, int *speed)
{
    *speed = 1000;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_an_set
 * Purpose:	
 *	Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	an - Boolean, if true, auto-negotiation is enabled 
 *		(and/or restarted). If false, autonegotiation is disabled.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t *pc; 
    uint16	sgmii_ctrl, misc_ctrl;

    pc       = INT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, &sgmii_ctrl));
    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_MISC_CTRL_REG, &misc_ctrl));

    /*
     * Special case for BCM5421S being used in SGMII copper mode with
     * BCM5690.  Always have autoneg on.
     */

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_5421S)) {
	an = 1;
    }

    if (an) {
	sgmii_ctrl |= QS_SGMII_AN_SEL;		/* IEEE clause 37 compliant */
	sgmii_ctrl &= ~QS_SGMII_AN_DISABLE;
	misc_ctrl |= QS_SGMII_MAC_MODE;
    } else {
	sgmii_ctrl &= ~QS_SGMII_AN_SEL;
	sgmii_ctrl |= QS_SGMII_AN_DISABLE;
	misc_ctrl &= ~QS_SGMII_MAC_MODE;
    }

    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, sgmii_ctrl));
    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_SGMII_MISC_CTRL_REG, misc_ctrl));

    if (an) {
	/* Restart autonegotiation (not a self-clearing bit) */

	SOC_IF_ERROR_RETURN
	    (WRITE_PHY_REG(unit, pc,
			    QS_SGMII_MISC_REG, QS_SGMII_ANA_RESTART));
	SOC_IF_ERROR_RETURN
	    (WRITE_PHY_REG(unit, pc,
			    QS_SGMII_MISC_REG, 0));
    }

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_an_get
 * Purpose:	
 *	Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	an - (OUT) if true, auto-negotiation is enabled.
 *	an_done - (OUT) if true, auto-negotiation is complete.
 *	        This value is undefined if an == false.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    uint16	sgmii_ctrl, sgmii_stat;

    pc       = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, &sgmii_ctrl));

    *an = (sgmii_ctrl & QS_SGMII_AN_DISABLE) == 0;

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_STAT_REG, &sgmii_stat));

    *an_done = (sgmii_stat & QS_SGMII_ANA_COMPLETE) != 0;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_adv_local_set
 * Purpose:	
 *	Set the current advertisement for auto-negotiation.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - Port mode mask indicating supported options/speeds.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int 
phy_5690_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    uint16	an_adv, sgmii_ctrl;
    phy_ctrl_t *pc;

    pc       = INT_PHY_SW_STATE(unit, port);

    mode &= SOC_PM_1000MB_FD | SOC_PM_PAUSE;

    /*
     * Set advertised duplex (only FD supported).
     */

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_ANA_REG, &an_adv));

    an_adv &= ~(QS_MII_ANA_FD | QS_MII_ANA_HD);

    if (mode & SOC_PM_1000MB_FD) {
	an_adv |= QS_MII_ANA_FD;
    }

    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_ANA_REG, an_adv));

    /*
     * Set advertised pause bits in link code word.
     */

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, &sgmii_ctrl));

    sgmii_ctrl &= ~(QS_SGMII_PAUSE_SYM | QS_SGMII_PAUSE_ASYM);

    switch (mode & SOC_PM_PAUSE) {
    case SOC_PM_PAUSE_TX:
	sgmii_ctrl |= QS_SGMII_PAUSE_ASYM;
	break;
    case SOC_PM_PAUSE_RX:
	sgmii_ctrl |= QS_SGMII_PAUSE_SYM | QS_SGMII_PAUSE_ASYM;
	break;
    case SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX:
	sgmii_ctrl |= QS_SGMII_PAUSE_SYM;
	break;
    }

    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, sgmii_ctrl));

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_adv_local_get
 * Purpose:	
 *	Get the current advertisement for auto-negotiation.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16	an_adv, sgmii_ctrl;
    phy_ctrl_t *pc;

    pc       = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_ANA_REG, &an_adv));

    if (an_adv & QS_MII_ANA_FD) {
	*mode |= SOC_PM_1000MB_FD;
    }

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, &sgmii_ctrl));

    switch (sgmii_ctrl & (QS_SGMII_PAUSE_SYM | QS_SGMII_PAUSE_ASYM)) {
    case QS_SGMII_PAUSE_SYM:
	*mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
	break;
    case QS_SGMII_PAUSE_ASYM:
	*mode |= SOC_PM_PAUSE_TX;
	break;
    case QS_SGMII_PAUSE_SYM | QS_SGMII_PAUSE_ASYM:
	*mode |= SOC_PM_PAUSE_RX;
	break;
    }

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_adv_remote_get
 * Purpose:	
 *	Get partner's current advertisement for auto-negotiation.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *	SOC_E_XXX
 */

STATIC int
phy_5690_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    phy_ctrl_t *pc;
    uint16	anlpa;

    pc       = INT_PHY_SW_STATE(unit, port);

    *mode = 0;

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_ANP_REG, &anlpa));

    if (anlpa & QS_MII_ANP_FIBER_FD) {
	*mode |= SOC_PM_1000MB_FD;
    }

    if (anlpa & QS_MII_ANP_FIBER_HD) {
	*mode |= SOC_PM_1000MB_HD;
    }

    switch (anlpa &
	    (QS_MII_ANP_FIBER_PAUSE_SYM | QS_MII_ANP_FIBER_PAUSE_ASYM)) {
    case QS_MII_ANP_FIBER_PAUSE_SYM:
	*mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
	break;
    case QS_MII_ANP_FIBER_PAUSE_ASYM:
	*mode |= SOC_PM_PAUSE_TX;
	break;
    case QS_MII_ANP_FIBER_PAUSE_SYM | QS_MII_ANP_FIBER_PAUSE_ASYM:
	*mode |= SOC_PM_PAUSE_RX;
	break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_5690_master_set
 * Purpose:
 *      Configure PHY master mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - PHY master mode.
 * Returns:
 *      SOC_E_UNAVAIL
 */
STATIC int
phy_5690_master_set(int unit, soc_port_t port, int master)
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
 *      phy_5690_master_get
 * Purpose:
 *      Get current master mode setting
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) BCM_PORT_MS_NONE
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_5690_master_get(int unit, soc_port_t port, int *master)
{
    *master = SOC_PORT_MS_NONE;
    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_lb_set
 * Purpose:	
 *	Set the internal PHY loopback mode.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	loopback - Boolean: true = enable loopback, false = disable.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_lb_set(int unit, soc_port_t port, int enable)
{
    uint16	sgmii_ctrl;
    phy_ctrl_t *pc;

    pc       = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, &sgmii_ctrl));

    if (enable) {
	sgmii_ctrl |= QS_SGMII_TBI_LOOP | QS_SGMII_REV_PHASE;
    } else {
	sgmii_ctrl &= ~(QS_SGMII_TBI_LOOP | QS_SGMII_REV_PHASE);
    }

    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, sgmii_ctrl));

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_5690_lb_get
 * Purpose:	
 *	Get the local PHY loopback mode.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16	sgmii_ctrl;
    phy_ctrl_t *pc;

    pc       = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_CTRL_REG, &sgmii_ctrl));

    *enable = (sgmii_ctrl & QS_SGMII_TBI_LOOP) != 0;

    return SOC_E_NONE;
}

/*
 * Function:
 *	phy_5690_interface_set
 * Purpose:
 *	Set the current operating mode of the internal PHY.
 *	(Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	pif - one of SOC_PORT_IF_*
 * Returns:
 *	SOC_E_XXX
 */

STATIC int
phy_5690_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    int			rv;

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
                         "phy_5690_interface_set: u=%d p=%d pif=%d rv=%d\n"),
              unit, port, pif, rv));

    return rv;
}

/*
 * Function:
 *	phy_5690_interface_get
 * Purpose:
 *	Get the current operating mode of the internal PHY.
 *	(Pertaining to the MAC/PHY interface, not the line interface).
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *	SOC_E_XXX
 */

STATIC int
phy_5690_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    *pif = SOC_PORT_IF_GMII;

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_5690_ability_get
 * Purpose:	
 *	Get the abilities of the internal PHY.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
phy_5690_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16      serdes_mode;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_SGMII_CTRL2_REG, &serdes_mode));

    if (serdes_mode & QS_SGMII_FIBER_MODE) {
        *mode = (SOC_PM_1000MB_FD | SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM |
                 SOC_PM_AN | SOC_PM_LB_PHY | SOC_PM_GMII);
    } else {
        *mode = (SOC_PM_10MB | SOC_PM_100MB | SOC_PM_1000MB_FD | 
                 SOC_PM_LB_PHY | SOC_PM_GMII);
    }

    return SOC_E_NONE;
}

STATIC int
phy_5690_notify(int unit, soc_port_t port, 
                 soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_5690_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_5690_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_5690_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_5690_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_5690_notify_resume(unit, port, value));
        break;
    default:
        rv = SOC_E_PARAM;
    }

    return rv;
}

/*
 * Variable:	phy_5690drv_ge
 * Purpose:	PHY Driver for Quad Serdes PHY internal to Draco, 
 */

phy_driver_t phy_5690drv_ge = {
    "BCM5690 Internal SERDES PHY Driver",
    phy_5690_init,
    phy_null_reset,
    phy_5690_link_get, 
    phy_5690_enable_set, 
    phy_5690_enable_get, 
    phy_5690_duplex_set, 
    phy_5690_duplex_get, 
    phy_5690_speed_set, 
    phy_5690_speed_get, 
    phy_5690_master_set,
    phy_5690_master_get,
    phy_5690_an_set,
    phy_5690_an_get,
    phy_5690_adv_local_set,
    phy_5690_adv_local_get,
    phy_5690_adv_remote_get,
    phy_5690_lb_set,
    phy_5690_lb_get,
    phy_5690_interface_set, 
    phy_5690_interface_get, 
    phy_5690_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                       /* phy_cable_diag */
    NULL,                       /* phy_link_change */
    NULL,                       /* phy_control_set */ 
    NULL,                       /* phy_control_get */
    NULL,                       /* phy_reg_read */
    NULL,                       /* phy_reg_write */
    NULL,                       /* phy_reg_modify */
    phy_5690_notify
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *	_phy_5690_notify_duplex
 * Purpose:	
 *	Program duplex if (and only if) 5690 is an intermediate PHY.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex - Boolean, TRUE indicates full duplex, FALSE
 *		indicates half.
 * Returns:	
 *	SOC_E_XXX
 * Notes:
 *	If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *	talk directly to an external fiber module.
 *
 *	If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *	pass-through mode to talk to an external SGMII PHY.
 *
 *	When used in pass-through mode, autoneg must be turned off and
 *	the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_5690_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int		fiber = PHY_FLAGS_TST(unit, port, PHY_FLAGS_FIBER);
    uint16	mii_ctrl;
    phy_ctrl_t *pc;

    pc       = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_5690_notify_duplex: u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
	    (READ_PHY_REG(unit, pc, QS_MII_CTRL_REG, &mii_ctrl));

        if (!(mii_ctrl & QS_MII_CTRL_FD)) {
            mii_ctrl |= QS_MII_CTRL_FD;
            SOC_IF_ERROR_RETURN
		(WRITE_PHY_REG(unit, pc, QS_MII_CTRL_REG, mii_ctrl));
        }

        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
	(_phy_5690_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_MII_CTRL_REG, &mii_ctrl));

    if (duplex) {
	mii_ctrl |= QS_MII_CTRL_FD;
    } else {
	mii_ctrl &= ~QS_MII_CTRL_FD;
    }

    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_MII_CTRL_REG, mii_ctrl));

    /* Take SERDES PHY out of reset */

    SOC_IF_ERROR_RETURN
	(_phy_5690_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */

    SOC_IF_ERROR_RETURN
	(phy_5690_an_set(unit, port, FALSE));

    return SOC_E_NONE;
}

/*
 * Function:
 *	_phy_5690_notify_speed
 * Purpose:	
 *	Program duplex if (and only if) 5690 is an intermediate PHY.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	speed - Speed to program.
 * Returns:	
 *	SOC_E_XXX
 * Notes:
 *	If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *	talk directly to an external fiber module.
 *
 *	If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *	pass-through mode to talk to an external SGMII PHY.
 *
 *	When used in pass-through mode, autoneg must be turned off and
 *	the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_5690_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int		fiber = PHY_FLAGS_TST(unit, port, PHY_FLAGS_FIBER);
    uint16	mii_ctrl;
    int		rv = SOC_E_NONE;
    phy_ctrl_t *pc;

    pc       = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_5690_notify_speed: u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
	    (READ_PHY_REG(unit, pc, QS_MII_CTRL_REG, &mii_ctrl));

        if ((mii_ctrl & (QS_MII_CTRL_SS_LSB | QS_MII_CTRL_SS_MSB))
                != QS_MII_CTRL_SS_1000) {
            mii_ctrl &= ~(QS_MII_CTRL_SS_LSB | QS_MII_CTRL_SS_MSB);
            mii_ctrl |= QS_MII_CTRL_SS_1000;
            SOC_IF_ERROR_RETURN
		(WRITE_PHY_REG(unit, pc, MII_CTRL_REG, mii_ctrl));
        }
        return rv;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
	(_phy_5690_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_MII_CTRL_REG, &mii_ctrl));

    mii_ctrl &= ~(QS_MII_CTRL_SS_LSB | QS_MII_CTRL_SS_MSB);

    switch (speed) {
    case 10:
	mii_ctrl |= QS_MII_CTRL_SS_10;
	break;
    case 100:
	mii_ctrl |= QS_MII_CTRL_SS_100;
	break;
    case 0:
    case 1000:	
	mii_ctrl |= QS_MII_CTRL_SS_1000;
	break;
    default:
	rv = SOC_E_CONFIG;
	break;
    }

    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_MII_CTRL_REG, mii_ctrl));

    /* Take SERDES PHY out of reset */

    SOC_IF_ERROR_RETURN
	(_phy_5690_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */

    SOC_IF_ERROR_RETURN
	(phy_5690_an_set(unit, port, FALSE));

    return rv;
}

/*
 * Function:
 *	_phy_5690_stop
 * Purpose:
 *	Put 5690 SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_5690_stop(int unit, soc_port_t port)
{
    uint16		mii_ctrl;
    int			stop, copper;
    phy_ctrl_t         *pc;

    pc       = INT_PHY_SW_STATE(unit, port);

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
                         "phy_5690_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    SOC_IF_ERROR_RETURN
	(READ_PHY_REG(unit, pc, QS_MII_CTRL_REG, &mii_ctrl));

    if (stop) {
	mii_ctrl |= MII_CTRL_RESET;
    } else {
	mii_ctrl &= ~MII_CTRL_RESET;
    }

    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_MII_CTRL_REG, mii_ctrl));

    return SOC_E_NONE;
}

/*
 * Function:
 *	_phy_5690_notify_stop
 * Purpose:	
 *	Add a reason to put 5690 PHY in reset.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	flags - Reason to stop
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
_phy_5690_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pc->stop |= flags;

    return _phy_5690_stop(unit, port);
}

/*
 * Function:
 *	_phy_5690_notify_resume
 * Purpose:	
 *	Remove a reason to put 5690 PHY in reset.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	flags - Reason to stop
 * Returns:	
 *	SOC_E_XXX
 */

STATIC int
_phy_5690_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pc->stop &= ~flags;

    return _phy_5690_stop(unit, port);
}

STATIC int
_phy_5690_notify_interface(int unit, soc_port_t port, uint32 flags)
{
    uint16      mode;
    phy_ctrl_t *pc;

    pc       = INT_PHY_SW_STATE(unit, port);

    mode = (flags == SOC_PORT_IF_SGMII) ? 0 : QS_SGMII_FIBER_MODE;
    
    SOC_IF_ERROR_RETURN
	(WRITE_PHY_REG(unit, pc, QS_SGMII_CTRL2_REG, mode));
    
    return SOC_E_NONE;
}

#endif /* BCM_5690 */

typedef int _soc_phy_5690_not_empty; /* Make ISO compilers happy. */
