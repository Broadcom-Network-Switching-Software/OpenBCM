/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        physim.c
 * Purpose:     Dummy phy driver for simulation
 * Requires:    
 */

#if defined(INCLUDE_PHY_SIMUL)

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "physim.h"

/*
 * Variable:	phy_simul
 * Purpose:	Phy driver callouts for SIMUL interface
 */
phy_driver_t phy_simul = {
    "Simulation PHY Driver",
    phy_simul_init, 
    phy_simul_reset,
    phy_simul_link_get, 
    phy_simul_enable_set, 
    phy_simul_enable_get, 
    phy_simul_duplex_set, 
    phy_simul_duplex_get, 
    phy_simul_speed_set, 
    phy_simul_speed_get, 
    phy_simul_master_set,
    phy_simul_master_get,
    phy_simul_an_set, 
    phy_simul_an_get, 
    phy_simul_adv_local_set,
    phy_simul_adv_local_get, 
    phy_simul_adv_remote_get,
    phy_simul_lb_set,
    phy_simul_lb_get,
    phy_simul_interface_set, 
    phy_simul_interface_get, 
    phy_simul_ability_get,
    NULL,
    NULL,
    phy_simul_mdix_set,          /* phy_mdix_set */
    phy_simul_mdix_get,          /* phy_mdix_get */
    phy_simul_mdix_status_get,   /* phy_mdix_status_get */
    NULL,
    NULL,
    phy_simul_medium_get,
    NULL,                       /* phy_cable_diag  */
    NULL,                       /* phy_link_change */
    NULL,                       /* phy_control_set */
    NULL,                       /* phy_control_get */
    NULL,                       /* phy_reg_read */
    NULL,                       /* phy_reg_write */
    NULL                        /* phy_reg_modify */
};

struct _bcm_sim_data_s {
    int enable;
    int link;  /* When CMIC_LINK_STAT not used */
    int duplex;
    int speed;
    int master;
    int an;
    soc_port_mode_t adv_local;
    soc_port_mode_t adv_remote;
    int lb; /* loopback */
    soc_port_if_t pif;
    soc_port_mdix_t mdix;
} *_bcm_sim_data[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];


#define SIMUL_PARAM_CHECK(unit, port) \
    if ((unit < 0) || (port < 0) || (unit >= SOC_MAX_NUM_DEVICES) || \
            (port >= SOC_MAX_NUM_PORTS) || (_bcm_sim_data[unit][port] == NULL)) \
        return SOC_E_PARAM


/*
 * Function: 	
 *	phy_simul_init
 *
 * Purpose:	
 *	Initialize the SIMUL to a known good state.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *
 * Returns:	
 *	SOC_E_XXXX
 *
 * Notes: 
 *	No synchronization performed at this level.
 */

int
phy_simul_init(int unit, soc_port_t port)
{
    soc_port_mode_t mode;

    if ((unit < 0) || (port < 0) || (unit >= SOC_MAX_NUM_DEVICES) || \
            (port >= SOC_MAX_NUM_PORTS)) {
        return SOC_E_PARAM;
    }

    /* Allocate and init simulation structure here */
    if (_bcm_sim_data[unit][port] == NULL) {
        _bcm_sim_data[unit][port] =
            sal_alloc(sizeof(*_bcm_sim_data[unit][port]), "phy_simul");
    }
    if (_bcm_sim_data[unit][port] == NULL) {
        return SOC_E_MEMORY;
    }

    _bcm_sim_data[unit][port]->link = TRUE;
    _bcm_sim_data[unit][port]->enable = FALSE;
    _bcm_sim_data[unit][port]->pif = SOC_PORT_IF_NOCXN;
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit) &&
        (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) && IS_EGPHY_PORT(unit, port)) {
        _bcm_sim_data[unit][port]->pif = SOC_PORT_IF_GMII;
    }
#endif
    _bcm_sim_data[unit][port]->duplex = TRUE;
    _bcm_sim_data[unit][port]->speed = 0; /* max */
    _bcm_sim_data[unit][port]->master = SOC_PORT_MS_NONE;
    _bcm_sim_data[unit][port]->an = 0;
    _bcm_sim_data[unit][port]->adv_local = 0;
    _bcm_sim_data[unit][port]->lb = 0;
    _bcm_sim_data[unit][port]->mdix = SOC_PORT_MDIX_NORMAL;

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        mode = SOC_PM_10GB_FD | SOC_PM_XGMII;
    } else if (IS_GE_PORT(unit, port)) {
        mode = SOC_PM_1000MB_FD | SOC_PM_GMII;
    } else {
        mode = SOC_PM_100MB_FD | SOC_PM_MII;
    }
    _bcm_sim_data[unit][port]->adv_remote = mode;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_reset
 *
 * Purpose:	
 *	Initialize the SIMUL hardware to a known good state.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *
 * Returns:	
 *	SOC_E_XXXX
 *
 * Notes: 
 *	No synchronization performed at this level.
 */

int
phy_simul_reset(int unit, soc_port_t port, void *user_arg)
{
    COMPILER_REFERENCE(user_arg);

    return phy_simul_init(unit, port);
}

/*
 * Function: 	
 *	phy_simul_enable_set
 *
 * Purpose:	
 *	Enable or disable the physical interface.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	enable - Boolean, true = enable PHY, false = disable.
 *
 * Returns:	
 *	SOC_E_XXXX
 */
int
phy_simul_enable_set(int unit, soc_port_t port, int enable)
{
    SIMUL_PARAM_CHECK(unit, port);

    _bcm_sim_data[unit][port]->enable = enable;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_enable_get
 *
 * Purpose:	
 *	Enable or disable the physical interface for a SIMUL device.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	enable - (OUT) Boolean, true = enable PHY, false = disable.
 *
 * Returns:	
 *	SOC_E_XXXX
 */
int
phy_simul_enable_get(int unit, soc_port_t port, int *enable)
{
    SIMUL_PARAM_CHECK(unit, port);

    *enable = _bcm_sim_data[unit][port]->enable;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_link_get
 *
 * Purpose:	
 *	Always sets link to true
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	link - (OUT) Boolean, true indicates link established.
 *
 * Returns:	
 *	SOC_E_XXXX
 */
int
phy_simul_link_get(int unit, soc_port_t port, int *link)
{
    SIMUL_PARAM_CHECK(unit, port);

#if !defined(SIM_CMIC_LINK_STAT)
    *link = _bcm_sim_data[unit][port]->link;
#else
    {
        uint32 link_bmp;

        link_bmp = soc_pci_read(unit, CMIC_LINK_STAT);
        *link = ((link_bmp & (1 << port)) != 0);
    }
    
#endif

    return SOC_E_NONE;
}


/*
 * Function: 	
 *	phy_simul_duplex_set
 *
 * Purpose:	
 *	Set the current duplex mode (forced).
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex -Boolean, true indicates full duplex, false indicates half.
 *
 * Returns:	
 *	SOC_E_NONE
 *	SOC_E_UNAVAIL - Half duplex requested, and not supported.
 *
 * Notes: 
 *	Only full duplex supported (?)
 */
int
phy_simul_duplex_set(int unit, soc_port_t port, int duplex)
{
    SIMUL_PARAM_CHECK(unit, port);

    _bcm_sim_data[unit][port]->duplex = duplex;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_duplex_get
 *
 * Purpose:	
 *	Only full duplex supported
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex - (OUT) Boolean, true indicates full duplex, false 
 *		indicates half.
 *
 * Returns:	
 *	SOC_E_NONE
 *
 * Notes: 
 *	No synchronization performed at this level. Autonegotiation is 
 *	not manipulated. 
 */
int
phy_simul_duplex_get(int unit, soc_port_t port, int *duplex)
{
    SIMUL_PARAM_CHECK(unit, port);

    *duplex = _bcm_sim_data[unit][port]->duplex;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_speed_set
 *
 * Purpose:	
 *	Set the current operating speed (forced).
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	speed - Requested speed, only 1000 supported.
 *
 * Returns:	
 *	SOC_E_XXXX
 */
int
phy_simul_speed_set(int unit, soc_port_t port, int speed)
{
    SIMUL_PARAM_CHECK(unit, port);

    /* Should check port type here */
    _bcm_sim_data[unit][port]->speed = speed;
#if 0
    if (IS_FE_PORT(unit, port)) {
        if ((speed != 10) && (speed = 100)) {
            /* What needs to happen here? */
            return  SOC_E_NONE;
        }
    } else {
        if ((speed == 1000)||(speed == 10000)) {
            /* What needs to happen here? */
            return  SOC_E_NONE;
        }
    }
    return SOC_E_CONFIG;
#endif

    return  SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_speed_get
 *
 * Purpose:	
 *	Get the current operating speed for a SIMUL device.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex - (OUT) Boolean, true indicates full duplex, false 
 *		indicates half.
 *
 * Returns:	
 *	SOC_E_NONE
 */
int
phy_simul_speed_get(int unit, soc_port_t port, int *speed)
{
    SIMUL_PARAM_CHECK(unit, port);

    *speed = _bcm_sim_data[unit][port]->speed;
    if (*speed == 0) { /* Max speed for the port */
        if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
            *speed = 10000;
        } else if (IS_GE_PORT(unit, port)) {
            *speed = 1000;
        } else {
            *speed = 100;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_master_set
 *
 * Purpose:	
 *	Set the current master mode
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	master - SOC_PORT_MS_*
 *
 * Returns:	
 *	SOC_E_XXXX
 */
int
phy_simul_master_set(int unit, soc_port_t port, int master)
{
    SIMUL_PARAM_CHECK(unit, port);

    _bcm_sim_data[unit][port]->master = master;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_master_get
 *
 * Purpose:	
 *	Get the current master mode for a SIMUL device.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	master - (OUT) SOC_PORT_MS_*
 *
 * Returns:	
 *	SOC_E_NONE
 */
int
phy_simul_master_get(int unit, soc_port_t port, int *master)
{
    SIMUL_PARAM_CHECK(unit, port);

    *master = _bcm_sim_data[unit][port]->master;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_an_set
 *
 * Purpose:	
 *	Enable or disabled auto-negotiation on the specified port for a 
 *	SIMUL device.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	an   - Boolean, if true, auto-negotiation is enabled 
 *		(and/or restarted). If false, autonegotiation is disabled.
 *
 * Returns:	
 *	SOC_E_XXXX0 - success
 */
int
phy_simul_an_set(int unit, soc_port_t port, int an)
{
    SIMUL_PARAM_CHECK(unit, port);

    _bcm_sim_data[unit][port]->an = an;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_an_get
 *
 * Purpose:	
 *	Get the current auto-negotiation status (enabled/busy)
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	an   - (OUT) if true, auto-negotiation is enabled.
 *	an_done - (OUT) if true, auto-negotioation is complete. This
 *	     	value is undefined if an == false.
 *
 * Returns:	
 *	SOC_E_XXXX
 */
int
phy_simul_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    SIMUL_PARAM_CHECK(unit, port);

    *an = _bcm_sim_data[unit][port]->an;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_adv_local_set
 *
 * Purpose:	
 *	Set the current advertisement for auto-negotiation.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - Port mode mask indicating supported options/speeds.
 *
 * Returns:	
 *	SOC_E_XXXX
 *
 * Notes: 
 *	No synchronization performed at this level. The only options
 *	that are supported are pause and FD/HD. Although this routine
 *	supports FD/HD, in reality, the MAC only supports FULL so we
 *	should never really see it.
 */
int 
phy_simul_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    SIMUL_PARAM_CHECK(unit, port);

    _bcm_sim_data[unit][port]->adv_local = mode;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_adv_local_get
 *
 * Purpose:	
 *	Get the current advertisement for auto-negotiation.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - (OUT) Port mode mask indicating supported options/speeds.
 *
 * Returns:	
 *	SOC_E_XXXX
 *
 * Notes: 
 *	No synchronization performed at this level.
 */
int
phy_simul_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    SIMUL_PARAM_CHECK(unit, port);

    *mode = _bcm_sim_data[unit][port]->adv_local;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_adv_remote_get
 *
 * Purpose:	
 *	Get partners current advertisement for auto-negotiation.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - (OUT) Port mode mask indicating supported options/speeds.
 *
 * Returns:	
 *	SOC_E_XXXX
 *
 * Notes: 
 *	No synchronization performed at this level. If Autonegotiation is
 *	disabled or in progress, this routine will return an error.
 */
int
phy_simul_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    SIMUL_PARAM_CHECK(unit, port);

    /* Need to figure this out based on port type and ?config? */
    /* For now, return static value from simul data */

    *mode = _bcm_sim_data[unit][port]->adv_remote;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_lb_set
 *
 * Purpose:	
 *	Set the local PHY loopback mode.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	loopback - Boolean: true = enable loopback, false = disable.
 *
 * Returns:	
 *	SOC_E_XXXX
 *
 * Notes: 
 *	No synchronization performed at this level.
 */
int
phy_simul_lb_set(int unit, soc_port_t port, int enable)
{
    SIMUL_PARAM_CHECK(unit, port);

    _bcm_sim_data[unit][port]->lb = enable;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_lb_get
 *
 * Purpose:	
 *	Get the local SIMUL loopback mode.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	loopback - (OUT) Boolean: true = enable loopback, false = disable.
 *
 * Returns:	
 *	SOC_E_XXXX
 */
int
phy_simul_lb_get(int unit, soc_port_t port, int *enable)
{
    SIMUL_PARAM_CHECK(unit, port);

    *enable = _bcm_sim_data[unit][port]->lb;

    return SOC_E_NONE;
}

/*
 * Function:
 *	phy_simul_interface_set
 *
 * Purpose:
 *	Set the current operating mode of the PHY (TBI or MII/GMII) for
 *	the SIMUL device.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	pif - one of SOC_PORT_IF_*
 *
 * Returns:
 *	SOC_E_XXXX
 */
int
phy_simul_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    SIMUL_PARAM_CHECK(unit, port);

    /* This should check port type is compatible w/ IF. */
    _bcm_sim_data[unit][port]->pif = pif;

    return SOC_E_NONE;
}

/*
 * Function:
 *	phy_simul_interface_get
 *
 * Purpose:
 *	Get the current operating mode of the PHY (TBI or MII/GMII)
 *	for SIMUL device.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	pif - (OUT) one of SOC_PORT_IF_*
 *
 * Returns:
 *	SOC_E_XXXX
 */
int
phy_simul_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    SIMUL_PARAM_CHECK(unit, port);

    /* Should check set has been called */
    *pif = _bcm_sim_data[unit][port]->pif;

    return SOC_E_NONE;
}

/*
 * Function: 	
 *	phy_simul_ability_get
 *
 * Purpose:	
 *	Get the abilities of the local gigabit phy.
 *
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - (OUT) Port mode mask indicating supported options/speeds.
 *
 * Returns:	
 *	SOC_E_NONE
 */
int 
phy_simul_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    COMPILER_REFERENCE(unit);

    if (IS_FE_PORT(unit, port)) {
        *mode = SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX |
          SOC_PM_10MB_FD | SOC_PM_100MB_FD;
    } else if (IS_HG_PORT(unit, port)) {
	*mode = (SOC_PM_10GB_FD | SOC_PM_MII | SOC_PM_XGMII |SOC_PM_LB_MAC |
		 SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM);
    }  else {
        *mode = SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX | SOC_PM_1000MB_FD;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	phy_simul_mdix_set
 * Description:
 *	Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *	unit - Device number
 *	port - Port number
 *	mode - One of:
 *	        BCM_PORT_MDIX_AUTO
 *			Enable auto-MDIX when autonegotiation is enabled
 *	        BCM_PORT_MDIX_FORCE_AUTO
 *			Enable auto-MDIX always
 *		BCM_PORT_MDIX_NORMAL
 *			Disable auto-MDIX
 *		BCM_PORT_MDIX_XOVER
 *			Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *	SOC_E_UNAVAIL - feature unsupported by hardware
 *	SOC_E_XXX - other error
 */
int
phy_simul_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    SIMUL_PARAM_CHECK(unit, port);

    /* Should check set has been called */
    _bcm_sim_data[unit][port]->mdix = mode;

    return SOC_E_NONE;
}    

/*
 * Function:
 *	phy_simul_mdix_get
 * Description:
 *	Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *	unit - Device number
 *	port - Port number
 *	mode - (Out) One of:
 *	        BCM_PORT_MDIX_AUTO
 *			Enable auto-MDIX when autonegotiation is enabled
 *	        BCM_PORT_MDIX_FORCE_AUTO
 *			Enable auto-MDIX always
 *		BCM_PORT_MDIX_NORMAL
 *			Disable auto-MDIX
 *		BCM_PORT_MDIX_XOVER
 *			Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *	SOC_E_UNAVAIL - feature unsupported by hardware
 *	SOC_E_XXX - other error
 */
int
phy_simul_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    SIMUL_PARAM_CHECK(unit, port);

    *mode = _bcm_sim_data[unit][port]->mdix;

    return SOC_E_NONE;
}    

/*
 * Function:
 *	phy_simul_mdix_status_get
 * Description:
 *	Get the current MDIX status on a port/PHY
 * Parameters:
 *	unit    - Device number
 *	port    - Port number
 *	status  - (OUT) One of:
 *	        BCM_PORT_MDIX_STATUS_NORMAL
 *			Straight connection
 *	        BCM_PORT_MDIX_STATUS_XOVER
 *			Crossover has been performed
 * Return Value:
 *	SOC_E_UNAVAIL - feature unsupported by hardware
 *	SOC_E_XXX - other error
 */
int
phy_simul_mdix_status_get(int unit, soc_port_t port, 
                          soc_port_mdix_status_t *status)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    *status = SOC_PORT_MDIX_STATUS_NORMAL;

    return SOC_E_NONE;
}    

/*
 * Function:
 *      phy_simul_medium_get
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
phy_simul_medium_get(int unit, soc_port_t port,
                     soc_port_medium_t *medium)
{
    int rv;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (medium != NULL) {
        *medium = SOC_PORT_MEDIUM_NONE;
        rv = SOC_E_NONE;
    } else {    
        rv = SOC_E_PARAM;
    }
    
    return (rv);
}

#endif /* INCLUDE_PHY_SIMUL */

typedef int _soc_phy_simul_not_empty; /* Make ISO compilers happy. */
