/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>
#if defined(BCM_HERCULES_SUPPORT)
#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/port.h>
#include <bcm/stg.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stg.h>
#include <bcm_int/esw/hercules.h>

/*
 * Function:
 *	bcm_hercules_stg_stp_init
 * Purpose:
 *	Write an entry with the spanning tree state DISABLE for all ports.
 */

int
bcm_hercules_stg_stp_init(int unit, bcm_stg_t stg)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_hercules_stg_stp_set
 * Purpose:
 *	Set the spanning tree state for a port in specified STG.
 * Parameters:
 *	unit - StrataSwitch unit number.
 *      stg - STG ID.
 *	port - StrataSwitch port number.
 *	stp_state - State to place port in.
 */

int
bcm_hercules_stg_stp_set(int unit, bcm_stg_t stg, bcm_port_t port,
			 int stp_state)
{    
    return (BCM_E_NONE);
}

/*
 * Function:
 *	bcm_hercules_stg_stp_get
 * Purpose:
 *	Retrieve the spanning tree state for a port in specified STG.
 * Parameters:
 *	unit - StrataSwitch unit number.
 *      stg - STG ID.
 *	port - StrataSwitch port number.
 *	stp_state - Pointer where state stored.
 */

int
bcm_hercules_stg_stp_get(int unit, bcm_stg_t stg, bcm_port_t port,
			 int *stp_state)
{
    /* Hercules ports are always in forwarding, really. */
    *stp_state = BCM_STG_STP_FORWARD;
    return (BCM_E_NONE);
}
#else /* BCM_HERCULES_SUPPORT */
typedef int _hercules_stg_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_HERCULES_SUPPORT */
