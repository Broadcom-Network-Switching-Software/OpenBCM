/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        serdes.c
 * Purpose:
 * Requires:    
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(_INCLUDE_SERDES_ASSUMED)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
#include <sal/types.h>
#include <sal/core/spl.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <shared/bsl.h>
#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phyident.h"
#include "phynull.h"
#include "serdesassumed.h"

/************************************************************************
 *									*
 *			SERDES Support					*
 */

#define	SERDES_STABLE_TIME	2	/* # seconds to stablize */

/*
 * Variable:
 *	serdes_state
 * Purpose:
 *	The serdes devices flap LINKOK/SYNCOK around in the wind
 *	for a long time as links are being negotiated etc. For this
 *	reason states for the GE ports that use the SERDES driver
 *	keeps state around. Those states marked as (timed) are required 
 *	to maintain their current state (or move forward) for 
 *	SERDES_STABLE_TIME. State transitions are:
 *
 *  	      | serReset|  serDOWN  | serSYNCOK |   serAN   | serUP
 *------------+---------+-----------+-----------+-----------+--------
 * SYNCOK=0   |		|	    |		|	    |
 * AN-Enable=0| serDOWN	|  serDOWN  |  serDOWN  |  serDOWN  | serRESET 
 * AN-Cmplt=0 |	(timed)	|	    |	        |  	    |
 *------------+---------+-----------+-----------+-----------+--------
 * SYNCOK=1   |		|	    |		|  	    |
 * AN-Enable=0| serDOWN | serSYNKOK |   serUP   |  serDOWN  | serUP
 * AN-Cmplt=0 |	(timed)	|  (timed)  |  (timed)  |  	    |
 *------------+---------+-----------+-----------+-----------+--------
 * SYNCOK=1   |		|	    |           |	    |
 * AN-Enable=1| serDOWN	| serSYNKOK |   serAN   |   serAN   |serRESET
 * AN-Cmplt=0 |	(timed)	|  (timed)  |  (timed)  |	    |
 *------------+---------+-----------+-----------+-----------+--------
 * SYNCOK=1   |		|	    |	      	|	    |
 * AN-Enable=1| serDOWN	| serSYNKOK |   serUP   |   serUP   | serUP
 * AN-Cmplt=1 |	(timed)	|  (timed)  |  (timed)  |	    |
 *------------+---------+-----------+----------+------------+--------
 */
typedef struct {
    enum {    
	serDISABLED,			/* Port is disabled */
	serRESET,
	serDOWN,
	serSYNCOK,			/* SYNOK detected */
	serAN,				/* Autonegotiation in progress */
	serUP				/* Running */
    }			ser_state;
    sal_time_t		ser_time;	/* First LINKOK time */
} serdes_state_t;

STATIC serdes_state_t	*serdes_state[SOC_MAX_NUM_DEVICES];

#define	SERDES_ALLOC(_u)	do { \
	serdes_state[_u] = sal_alloc(SOC_MAX_NUM_PORTS * \
					sizeof(serdes_state_t), \
					"serdes_state"); \
	sal_memset(serdes_state[_u], 0, SOC_MAX_NUM_PORTS * \
					sizeof(serdes_state_t)); \
	} while (0)
#define	SERDES_IS_ALLOC(_u)	(serdes_state[_u])

#define	SERDES(_u, _p)	(serdes_state[(_u)][(_p)])
#define	SERDES_STABLE(_u, _p)	\
    ((sal_time() - SERDES((_u), (_p)).ser_time) >= SERDES_STABLE_TIME)

#if defined(BROADCOM_DEBUG) 

STATIC char *serdes_state_desc[] = {
    "serDISABLED", "serRESET", "serDOWN", "serSYNCOK", "serAN", "serUP"};

#define	SERDES_TRANSITION(_u, _p, _s, _r) 			\
    LOG_VERBOSE(BSL_LS_SOC_LINK, \
                (BSL_META_U((_u), \
                            "Unit %d Port %s Serdes State %s --> %s (%s)\n"),	\
                 (_u), SOC_PORT_NAME(_u, _p),			\
                 serdes_state_desc[SERDES((_u), (_p)).ser_state], 	\
                 serdes_state_desc[(_s)], (_r)));			\
     SERDES((_u), (_p)).ser_state = (_s);			\
     SERDES((_u), (_p)).ser_time  = sal_time()

#define SERDES_RESET(_u, _p, _s)                                \
    LOG_VERBOSE(BSL_LS_SOC_LINK, \
                (BSL_META_U((_u), \
                            "Unit %d Port %s Serdes RESET (%s)\n"),	\
                 (_u), SOC_PORT_NAME(_u, _p), (_s)));		\
     _phy_serdesassumed_reset((_u), (_p), FALSE)

#else /* !defined(BROADCOM_DEBUG) */

#define	SERDES_TRANSITION(_u, _p, _s, _r) 			\
     SERDES((_u), (_p)).ser_state = (_s);			\
     SERDES((_u), (_p)).ser_time  = sal_time()

#define SERDES_RESET(_u, _p, _s) _phy_serdesassumed_reset((_u), (_p), FALSE)

#endif  /* BROADCOM_DEBUG */

/*
 * Function: 	
 *	_phy_serdesassumed_reset
 * Purpose:	
 *	Reset SERDES state machines.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	enable - TRUE, enable port, FALSE, disable port
 * Returns:	
 *	SOC_E_XXXX
 * Notes:
 *	If the port is in disabled state, nothing is done. 
 */

STATIC int
_phy_serdesassumed_reset(int unit, soc_port_t port, int enable)
{
    uint64	gpcsc;
    int		isdisabled;

    if (!SERDES_IS_ALLOC(unit)) {
	isdisabled = 1;
    } else {
	isdisabled = SERDES(unit, port).ser_state == serDISABLED;
    }
    if (enable || !isdisabled) {
	if (!SERDES_IS_ALLOC(unit)) {
	    SERDES_ALLOC(unit);
	}
	/*
	 * Attempt to make sure the other end notices that we have dropped
	 * the link.
	 */
	SOC_IF_ERROR_RETURN(READ_GPCSCr(unit, port, &gpcsc));
	soc_reg64_field32_set(unit, GPCSCr, &gpcsc, DTRDf, 1);
	soc_reg64_field32_set(unit, GPCSCr, &gpcsc, DRRDf, 1);
	SOC_IF_ERROR_RETURN(WRITE_GPCSCr(unit, port, gpcsc));
	SERDES_TRANSITION(unit, port, serRESET, "Reset");
    }

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_enable_set
 * Purpose:	
 *	Enable or disable the physical interface.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	enable - Boolean, true = enable PHY, false = disable.
 * Returns:	
 *	SOC_E_XXXX
 */

STATIC int
phy_serdesassumed_enable_set(int unit, soc_port_t port, int enable)
{
    uint64	gpcsc;

    if (enable) {
	SOC_IF_ERROR_RETURN(_phy_serdesassumed_reset(unit, port, TRUE));
    } else {
	/*
	 * Disable Disparity.
	 */
	SOC_IF_ERROR_RETURN(READ_GPCSCr(unit, port, &gpcsc));
	soc_reg64_field32_set(unit, GPCSCr, &gpcsc, DTRDf, 1);
	soc_reg64_field32_set(unit, GPCSCr, &gpcsc, DRRDf, 1);
	SOC_IF_ERROR_RETURN(WRITE_GPCSCr(unit, port, gpcsc));
	SERDES_TRANSITION(unit, port, serDISABLED, "Disabled");
    }

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_enable_get
 * Purpose:	
 *	Enable or disable the physical interface for a SERDES device.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:	
 *	SOC_E_XXXX
 */

STATIC int
phy_serdesassumed_enable_get(int unit, soc_port_t port, int *enable)
{
    if (SERDES_IS_ALLOC(unit)) {
	*enable = (SERDES(unit, port).ser_state != serDISABLED);
    } else {
	*enable = 0;
    }
    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	_phy_serdesassumed_link
 * Purpose:	
 *	Determine the current link up/down status for a SERDES device, 
 * 	using the timeds state machine.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	p_link - (OUT) Boolean, true indicates link established (may be NULL).
 *	p_an - (OUT) Boolean, true indicates AN is enabled (may be NULL).
 *	p_an_done - (OUT) Boolean, true indicates AN is complete (undefined
 *		if p_an is false, may be NULL).
 * Returns:	
 *	SOC_E_XXXX
 */

STATIC int
_phy_serdesassumed_link(int unit, soc_port_t port, 
		 int *p_link, int *p_an, int *p_an_done)
{
    uint64	anstt, anctl;
    int		sync_ok, an_enabled, an_done;

    SOC_IF_ERROR_RETURN(READ_ANSTTr(unit, port, &anstt));
    SOC_IF_ERROR_RETURN(READ_ANCTLr(unit, port, &anctl));

    sync_ok    = soc_reg64_field32_get(unit, ANSTTr, anstt, SYNCOKf);
    an_done    = soc_reg64_field32_get(unit, ANSTTr, anstt, ANCPLTf);
    an_enabled = soc_reg64_field32_get(unit, ANCTLr, anctl, ANENf);

    switch(SERDES(unit, port).ser_state) {
    case serDISABLED:
	break;
    case serRESET:
	if (SERDES_STABLE(unit, port)) {
	    uint64	gpcsc;

	    SOC_IF_ERROR_RETURN(READ_GPCSCr(unit, port, &gpcsc));
	    SERDES_TRANSITION(unit, port, serDOWN, "Reset Complete");
	    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, DTRDf, 0);
	    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, DRRDf, 0);
	    SOC_IF_ERROR_RETURN(WRITE_GPCSCr(unit, port, gpcsc));
	} else {
	    break;
	}
	/* *** NOBREAK *** */
    case serDOWN:
	if (!sync_ok) {
	    break;
	}
	SERDES_TRANSITION(unit, port, serSYNCOK, "Sync OK detected");
	/* *** NOBREAK *** */
    case serSYNCOK:
	if (!sync_ok) {
	    SERDES_TRANSITION(unit, port, serDOWN, "SYNCOK lost");
	    break;
	} 

	if (SERDES_STABLE(unit, port)) {
	    if (!an_enabled) {
		SERDES_TRANSITION(unit, port, serUP, "SYNCOK stable, no AN");
	    } else {
		SERDES_TRANSITION(unit, port, serAN, "SYNCOK stable, AN");
	    }
	}
	break;
    case serAN:
	if (!an_enabled) {
	    SERDES_TRANSITION(unit, port, serDOWN, "AN disabled");
	} else if (!sync_ok) {
	    SERDES_TRANSITION(unit, port, serDOWN, "SYNCOK lost");
	} else if (an_done) {
	    uint64	anlpa;

	    SOC_IF_ERROR_RETURN(READ_ANLPAr(unit, port, &anlpa));

	    if (soc_reg64_field32_get(unit, ANLPAr, anlpa, LPANERRf) != 0) {
		uint64	anctl;

		COMPILER_64_ZERO(anctl);

		soc_reg64_field32_set(unit, ANCTLr, &anctl, ANENf, 1);
		soc_reg64_field32_set(unit, ANCTLr, &anctl, RSTANf, 1);

		SOC_IF_ERROR_RETURN(WRITE_ANCTLr(unit, port, anctl));

		soc_reg64_field32_set(unit, ANCTLr, &anctl, RSTANf, 0);
		SOC_IF_ERROR_RETURN(WRITE_ANCTLr(unit, port, anctl));
		SERDES_TRANSITION(unit, port, serAN, "Remote Fault");
	    } else {
		SERDES_TRANSITION(unit, port, serUP, "AN Complete");
	    }
	}
	break;
    case serUP:
	if (!sync_ok) {
	    SERDES_RESET(unit, port, "SYNCOK lost");
	} else if (an_enabled && !an_done) {
	    SERDES_RESET(unit, port, "AN-DONE lost");
        }
	break;
    default:
	assert(0);
    }

    if (p_link)    *p_link    = SERDES(unit, port).ser_state == serUP;
    if (p_an_done) *p_an_done = SERDES(unit, port).ser_state == serUP;
    if (p_an)      *p_an      = an_enabled;

    return(SOC_E_NONE);
}

#undef SERDES				/* Used in above routine ONLY */
#undef SERDES_ALLOC			/* Used in above routine ONLY */
#undef SERDES_IS_ALLOC			/* Used in above routine ONLY */
#undef SERDES_STABLE			/* Used in above routine ONLY */
#undef SERDES_TRANSITION		/* Used in above routnes ONLY */
#undef SERDES_RESET			/* Used in above routnes ONLY */

/*
 * Function: 	
 *	phy_serdesassumed_init
 * Purpose:	
 *	Initialize the SERDES to a known good state.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 * Returns:	
 *	SOC_E_XXXX
 * Notes: 
 *	No synchronization performed at this level.
 */

STATIC int
phy_serdesassumed_init(int unit, soc_port_t port)
{
    soc_phy_info_t	*pi = &SOC_PHY_INFO(unit, port);
    uint64		gpcsc, anctl, annpg, anstt;

    SOC_IF_ERROR_RETURN(READ_GPCSCr(unit, port, &gpcsc));
    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, AUTOSf, 0); /* AN off */
    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, DRRDf, 0);
    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, DTRDf, 0);

    if (!(pi->phy_flags & PHY_FLAGS_5421S)) {
	soc_reg64_field32_set(unit, GPCSCr, &gpcsc, EWRAPf, 0);	/* loopback */
    }
    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, JTRPSf, 4);
    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, JTRPTf, 0);
    /* RCSELf set in mac.c */
    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, SCLTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_GPCSCr(unit, port, gpcsc));

    COMPILER_64_ZERO(anctl);
    soc_reg64_field32_set(unit, ANCTLr, &anctl, ANENf, 0);
    soc_reg64_field32_set(unit, ANCTLr, &anctl, RSTANf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ANCTLr(unit, port, anctl));

    COMPILER_64_ZERO(anstt);
    soc_reg64_field32_set(unit, ANSTTr, &anstt, SYNCOKf, 0);
    soc_reg64_field32_set(unit, ANSTTr, &anstt, LINKOKf, 0);
    soc_reg64_field32_set(unit, ANSTTr, &anstt, PGRXf, 0);
    soc_reg64_field32_set(unit, ANSTTr, &anstt, ANCPLTf, 0);
    soc_reg64_field32_set(unit, ANSTTr, &anstt, RFINDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ANSTTr(unit, port, anstt));

    COMPILER_64_ZERO(annpg);
    soc_reg64_field32_set(unit, ANNPGr, &annpg, FDf, 1); /* Default advert FD */
    SOC_IF_ERROR_RETURN(WRITE_ANNPGr(unit, port, annpg));

    _phy_serdesassumed_reset(unit, port, TRUE);

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_link_get
 * Purpose:	
 *	Determine the current link up/down status for a SERDES device.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	link - (OUT) Boolean, true indicates link established.
 * Returns:	
 *	SOC_E_XXXX
 * Notes: 
 *	No synchronization performed at this level. This routine will
 *	POLL the LINKOK/SYNCOK status if the link was not previously 
 *	detected as being up, but now appears so.
 */

STATIC int
phy_serdesassumed_link_get(int unit, soc_port_t port, int *link)
{
    return(_phy_serdesassumed_link(unit, port, link, NULL, NULL));
}

/*
 * Function: 	
 *	phy_serdesassumed_duplex_set
 * Purpose:	
 *	Set the current duplex mode (forced).
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex -Boolean, true indicates full duplex, false indicates half.
 * Returns:	
 *	SOC_E_NONE
 *	SOC_E_UNAVAIL - Half duplex requested, and not supported.
 * Notes: 
 *	No synchronization performed at this level. Autonegotiation is 
 *	not manipulated. 
 */

STATIC int
phy_serdesassumed_duplex_set(int unit, soc_port_t port, int duplex)
{
    if (duplex) {
	return (_phy_serdesassumed_reset(unit, port, FALSE));
    } else {
	return (SOC_E_UNAVAIL);
    }
}

/*
 * Function: 	
 *	phy_serdesassumed_duplex_get
 * Purpose:	
 *	Get the current operating duplex mode. If autoneg is enabled, 
 *	then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex - (OUT) Boolean, true indicates full duplex, false 
 *		indicates half.
 * Returns:	
 *	SOC_E_NONE
 * Notes: 
 *	No synchronization performed at this level. Autonegotiation is 
 *	not manipulated. 
 */

STATIC int
phy_serdesassumed_duplex_get(int unit, soc_port_t port, int *duplex)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *duplex = TRUE;

    return(SOC_E_NONE);
}


/*
 * Function: 	
 *	phy_serdesassumed_speed_set
 * Purpose:	
 *	Set the current operating speed (forced).
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	speed - Requested speed, only 1000 supported.
 * Returns:	
 *	SOC_E_XXXX
 */

STATIC int
phy_serdesassumed_speed_set(int unit, soc_port_t port, int speed)
{
    if (speed == 0 || speed == 1000) {
	return (_phy_serdesassumed_reset(unit, port, FALSE));
    } else {
	return (SOC_E_CONFIG);
    }
}

/*
 * Function: 	
 *	phy_serdesassumed_speed_get
 * Purpose:	
 *	Get the current operating speed for a SERDES device.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	duplex - (OUT) Boolean, true indicates full duplex, false 
 *		indicates half.
 * Returns:	
 *	SOC_E_NONE
 */

STATIC int
phy_serdesassumed_speed_get(int unit, soc_port_t port, int *speed)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *speed = 1000;

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_master_set
 * Purpose:	
 *	Set the current master mode
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	master - SOC_PORT_MS_*
 * Returns:	
 *	SOC_E_XXXX
 */

STATIC int
phy_serdesassumed_master_set(int unit, soc_port_t port, int master)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(master);

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_master_get
 * Purpose:	
 *	Get the current master mode for a SERDES device.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	master - (OUT) SOC_PORT_MS_*
 * Returns:	
 *	SOC_E_NONE
 */

STATIC int
phy_serdesassumed_master_get(int unit, soc_port_t port, int *master)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *master = SOC_PORT_MS_NONE;

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_an_set
 * Purpose:	
 *	Enable or disabled auto-negotiation on the specified port for a 
 *	SERDES device.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	an   - Boolean, if true, auto-negotiation is enabled 
 *		(and/or restarted). If false, autonegotiation is disabled.
 * Returns:	
 *	SOC_E_XXXX - success
 */

STATIC int
phy_serdesassumed_an_set(int unit, soc_port_t port, int an)
{
    uint64	anctl, gpcsc;
    int		autos;

    autos = soc_property_port_get(unit, port, spn_PHY_SERDES_AUTOS, 0);

    SOC_IF_ERROR_RETURN(READ_GPCSCr(unit, port, &gpcsc));
    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, AUTOSf,
                          (an && autos) ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_GPCSCr(unit, port, gpcsc));

    COMPILER_64_ZERO(anctl);
    soc_reg64_field32_set(unit, ANCTLr, &anctl, ANENf, an ? 1 : 0);
    soc_reg64_field32_set(unit, ANCTLr, &anctl, RSTANf, an ? 1 : 0);
    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, AUTOSf, 0);

    SOC_IF_ERROR_RETURN(WRITE_ANCTLr(unit, port, anctl));

    soc_reg64_field32_set(unit, ANCTLr, &anctl, RSTANf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ANCTLr(unit, port, anctl));

    return(_phy_serdesassumed_reset(unit, port, FALSE));
}

/*
 * Function: 	
 *	phy_serdesassumed_an_get
 * Purpose:	
 *	Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	an   - (OUT) if true, auto-negotiation is enabled.
 *	an_done - (OUT) if true, auto-negotioation is complete. This
 *	     	value is undefined if an == FALSE. 
 * Returns:	
 *	SOC_E_XXXX
 * Notes:
 *	This routine basis it's decision on the serdes S/W state, and not the 
 *	Hardware bit. This allows proper
 */

STATIC int
phy_serdesassumed_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    return(_phy_serdesassumed_link(unit, port, NULL, an, an_done));
}

/*
 * Function: 	
 *	phy_serdesassumed_adv_local_set
 * Purpose:	
 *	Set the current advertisement for auto-negotiation.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - Port mode mask indicating supported options/speeds.
 * Returns:	
 *	SOC_E_XXXX
 * Notes: 
 *	No synchronization performed at this level. The only options
 *	that are supported are pause and FD/HD. Although this routine
 *	supports FD/HD, in reality, the MAC only supports FULL so we
 *	should never really see it.
 */

STATIC int 
phy_serdesassumed_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    uint64	annpg, gmacc0;

    mode &= SOC_PM_1000MB_FD | SOC_PM_PAUSE;
    COMPILER_64_ZERO(annpg);

    if (mode & SOC_PM_1000MB_FD) {
	soc_reg64_field32_set(unit, ANNPGr, &annpg, FDf, 1);
    } else if (mode & SOC_PM_1000MB_HD) {
	soc_reg64_field32_set(unit, ANNPGr, &annpg, FDf, 0);
    } else {
	return(SOC_E_UNAVAIL);
    }

    switch (mode & SOC_PM_PAUSE) {
    case SOC_PM_PAUSE_TX:
	soc_reg64_field32_set(unit, ANNPGr, &annpg, PAUSEf, 0);
	soc_reg64_field32_set(unit, ANNPGr, &annpg, ASMDRf, 1);
	break;
    case SOC_PM_PAUSE_RX:
	soc_reg64_field32_set(unit, ANNPGr, &annpg, PAUSEf, 0);
	soc_reg64_field32_set(unit, ANNPGr, &annpg, ASMDRf, 1);
	break;
    case SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX:
	soc_reg64_field32_set(unit, ANNPGr, &annpg, PAUSEf, 1);
	soc_reg64_field32_set(unit, ANNPGr, &annpg, ASMDRf, 0);
	break;
    default:				/* No flow control */
	soc_reg64_field32_set(unit, ANNPGr, &annpg, PAUSEf, 0);
	soc_reg64_field32_set(unit, ANNPGr, &annpg, ASMDRf, 0);
	break;
    }

    SOC_IF_ERROR_RETURN(READ_GMACC0r(unit, port, &gmacc0)); /* into reset */
    soc_reg64_field32_set(unit, GMACC0r, &gmacc0, SRSTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_GMACC0r(unit, port, gmacc0));

    SOC_IF_ERROR_RETURN(WRITE_ANNPGr(unit, port, annpg));

    soc_reg64_field32_set(unit, GMACC0r, &gmacc0, SRSTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_GMACC0r(unit, port, gmacc0)); /* out of reset */

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_adv_local_get
 * Purpose:	
 *	Get the current advertisement for auto-negotiation.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:	
 *	SOC_E_XXXX
 * Notes: 
 *	No synchronization performed at this level.
 */

STATIC int
phy_serdesassumed_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint64	annpg;

    *mode = 0;

    SOC_IF_ERROR_RETURN(READ_ANNPGr(unit, port, &annpg));

    if (soc_reg64_field32_get(unit, ANNPGr, annpg, FDf)) {
	*mode = SOC_PM_1000MB_FD;
    } else {
	*mode = SOC_PM_1000MB_HD;
    }

    /* PAUSE */

    if (soc_reg64_field32_get(unit, ANNPGr, annpg, ASMDRf)) {
	if (soc_reg64_field32_get(unit, ANNPGr, annpg, PAUSEf)) {
	    *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
	} else {
	    *mode |= SOC_PM_PAUSE_TX;
	}
    } else {
	if (soc_reg64_field32_get(unit, ANNPGr, annpg, PAUSEf)) {
	    *mode |= SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX;
	} 
    }	
    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_adv_remote_get
 * Purpose:	
 *	Get partners current advertisement for auto-negotiation.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:	
 *	SOC_E_XXXX
 * Notes: 
 *	No synchronization performed at this level. If Autonegotiation is
 *	disabled or in progress, this routine will return an error.
 */

STATIC int
phy_serdesassumed_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint64	anlpa;
    int		link, an, an_done;

    *mode = 0;

    SOC_IF_ERROR_RETURN(_phy_serdesassumed_link(unit, port, &link, &an, &an_done));

    if (!an) {
	return(SOC_E_DISABLED);
    } else if (!an_done) {
	return(SOC_E_BUSY);
    }
	
    SOC_IF_ERROR_RETURN(READ_ANLPAr(unit, port, &anlpa));

    if (soc_reg64_field32_get(unit, ANLPAr, anlpa, LPANERRf)) {
	return(SOC_E_FAIL);
    }

    /* Remote duplex negotiation */
    if (soc_reg64_field32_get(unit, ANLPAr, anlpa, LPFDf)) {
	*mode |= SOC_PM_1000MB_FD;
    } else {
	*mode |= SOC_PM_1000MB_HD;
    }

    /* Remote pause negotiation */
 
    if (soc_reg64_field32_get(unit, ANLPAr, anlpa, LPPAUSEf)) {
	*mode |= SOC_PM_PAUSE;
    } else {
	if (soc_reg64_field32_get(unit, ANLPAr, anlpa, LPASMDRf)) {
	    *mode |= SOC_PM_PAUSE_TX;
	}
    }

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_lb_set
 * Purpose:	
 *	Set the local PHY loopback mode.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	loopback - Boolean: true = enable loopback, false = disable.
 * Returns:	
 *	SOC_E_XXXX
 * Notes: 
 *	No synchronization performed at this level.
 *
 *	Only the Even numbered EWRAP lines are visible outside
 *	the chip. Thus, the even numbered EWRAP line controls 
 *	both the specified port, and the specified port + 1.
 *	
 *	Although this is a matter of how the board is physically 
 *	wired, this is the only general mechanism that makes sense 
 *	for all the current chips. 
 */

STATIC int
phy_serdesassumed_lb_set(int unit, soc_port_t port, int enable)
{
    soc_port_t		gp;
    uint64		gpcsc;
    int			poff;

    /*
     * The EWRAP line for GE0 fans out to both of the GIGs.
     * This means they are both in loopback at the same time, 
     * and we make this routine work by setting the port to GE0 if
     * we see GE1.
     */

    poff = SOC_PORT_OFFSET(unit, port);		/* eg: 3 if ge3 */
    poff &= ~1;					/* now 2 if ge3 */
    gp = SOC_PORT(unit, ge, poff);		/* get ge2 */

    SOC_IF_ERROR_RETURN(READ_GPCSCr(unit, gp, &gpcsc));
    soc_reg64_field32_set(unit, GPCSCr, &gpcsc, EWRAPf, enable ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_GPCSCr(unit, gp, gpcsc));

    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_lb_get
 * Purpose:	
 *	Get the local SERDES loopback mode.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns:	
 *	SOC_E_XXXX
 */

STATIC int
phy_serdesassumed_lb_get(int unit, soc_port_t port, int *enable)
{
    soc_port_t	gp;
    uint64	gpcsc;
    int		poff;

    /*
     * See phy_serdesassumed_lb_set()
     */
    poff = SOC_PORT_OFFSET(unit, port);		/* eg: 3 if ge3 */
    poff &= ~1;					/* now 2 if ge3 */
    gp = SOC_PORT(unit, ge, poff);		/* get ge2 */

    SOC_IF_ERROR_RETURN(READ_GPCSCr(unit, gp, &gpcsc));
    *enable = soc_reg64_field32_get(unit, GPCSCr, gpcsc, EWRAPf) ? TRUE : FALSE;

    return(SOC_E_NONE);
}

/*
 * Function:
 *	phy_serdesassumed_interface_set
 * Purpose:
 *	Set the current operating mode of the PHY (TBI or MII/GMII)
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	pif - one of SOC_PORT_IF_*
 * Returns:
 *	SOC_E_XXXX
 */

STATIC int
phy_serdesassumed_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    switch (pif) {
    case SOC_PORT_IF_TBI:
	return SOC_E_NONE;
    default:
	return SOC_E_UNAVAIL;
    }
}

/*
 * Function:
 *	phy_serdesassumed_interface_get
 * Purpose:
 *	Get the current operating mode of the PHY (TBI or MII/GMII)
 *	for SERDES device.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	pif - (OUT) Interface
 * Returns:
 *	SOC_E_XXXX
 */

STATIC int
phy_serdesassumed_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_TBI;
    return(SOC_E_NONE);
}

/*
 * Function: 	
 *	phy_serdesassumed_ability_get
 * Purpose:	
 *	Get the abilities of the local gigabit phy.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	port - StrataSwitch port #. 
 *	mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:	
 *	SOC_E_NONE
 */

STATIC int 
phy_serdesassumed_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *mode = (SOC_PM_AN | SOC_PM_LB_PHY |
	     SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM |
	     SOC_PM_TBI | SOC_PM_1000MB_FD);

    return(SOC_E_NONE);
}

/*
 * Variable:	phy_serdesassumed_ge
 * Purpose:	Phy driver callouts for SERDES interface
 */
phy_driver_t phy_serdesassumed_ge = {
    "Assumed SERDES Driver",
    phy_serdesassumed_init, 
    phy_null_reset,
    phy_serdesassumed_link_get, 
    phy_serdesassumed_enable_set, 
    phy_serdesassumed_enable_get, 
    phy_serdesassumed_duplex_set, 
    phy_serdesassumed_duplex_get, 
    phy_serdesassumed_speed_set, 
    phy_serdesassumed_speed_get, 
    phy_serdesassumed_master_set,
    phy_serdesassumed_master_get,
    phy_serdesassumed_an_set, 
    phy_serdesassumed_an_get, 
    phy_serdesassumed_adv_local_set,
    phy_serdesassumed_adv_local_get, 
    phy_serdesassumed_adv_remote_get,
    phy_serdesassumed_lb_set,
    phy_serdesassumed_lb_get,
    phy_serdesassumed_interface_set, 
    phy_serdesassumed_interface_get, 
    phy_serdesassumed_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                       /* phy_cable_diag  */
    NULL,                       /* phy_link_change */
    NULL,                       /* phy_control_set */
    NULL,                       /* phy_control_get */
    NULL,                       /* phy_reg_read */
    NULL,                       /* phy_reg_write */
    NULL                        /* phy_reg_modify */
};

#else /* INCLUDE_SERDES_ASSUMED */
typedef int _phy_serdesassumed_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_SERDES_ASSUMED */

