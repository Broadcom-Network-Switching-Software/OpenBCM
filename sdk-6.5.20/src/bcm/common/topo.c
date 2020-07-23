/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        topo.c
 * Purpose:     Topology mapping, when so programmed.
 * Requires:    
 */


#include <soc/drv.h>

#include <bcm/topo.h>
#include <bcm/error.h>
#include <bcm/stack.h>

static bcm_topo_map_f _topo_map;


/*
 * Function:
 *      bcm_topo_map_set/get
 * Purpose:
 *      Set/get the map used by topo_port_resolve
 * Parameters:
 *      _map        - (OUT for get) The function pointer to use
 * Returns:
 *      BCM_E_NONE
 * Notes:
 *      These functions are not dispatchable.  The board type
 *      and topology information determine the map to use.
 */

int
bcm_topo_map_set(bcm_topo_map_f _map)
{
    _topo_map = _map;

    return BCM_E_NONE;
}

int
bcm_topo_map_get(bcm_topo_map_f *_map)
{
    *_map = _topo_map;

    return BCM_E_NONE;
}


/*
 * Default topology mapping function.
 * This routine is used when no board specific mapping function
 * has been programmed.  It knows about common single board configurations.
 */

STATIC int
_bcm_topo_default_map(int unit, int dest_modid, bcm_port_t *exit_port)
{
    int		rv, local_mod, local_mod_count;
    bcm_port_t	port;
    bcm_pbmp_t	pbmp;

    /* is the destination local? */
    rv = bcm_stk_my_modid_get(unit, &local_mod);
    if (BCM_E_NONE != rv) {
        return rv;
    }
    rv = bcm_stk_modid_count(unit, &local_mod_count);
    if (BCM_E_NONE != rv) {
        return rv;
    }

    if (dest_modid >= local_mod && dest_modid < local_mod + local_mod_count) {
	*exit_port = -1;
	return BCM_E_NONE;
    }

    /* handles xgs switches */
    rv = bcm_stk_modport_get(unit, dest_modid, exit_port);
    if (rv != BCM_E_UNAVAIL) {
	return rv;
    }

    /* handles xgs fabrics */
    rv = bcm_stk_ucbitmap_get(unit, -1, dest_modid, &pbmp);
    if (rv != BCM_E_UNAVAIL) {
	if (rv >= 0) {
	    BCM_PBMP_ITER(pbmp, port) {
		*exit_port = port;
		break;
	    }
	}
	return rv;
    }

    /* handles strata back to back boards */
    
    SOC_PBMP_STACK_ACTIVE_GET(unit, pbmp);
    if (BCM_PBMP_NOT_NULL(pbmp)) {
	BCM_PBMP_ITER(pbmp, port) {
	    *exit_port = port;	/* just grab the first stack port */
	    break;
	}
	return BCM_E_NONE;
    }

    /* out of ideas */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_topo_port_get
 * Purpose:
 *      Helper function to bcm_topo_port_get API
 * Parameters:
 *      unit        - The source unit
 *      dest_modid  - Destination to reach
 *      exit_port   - (OUT) Port to exit device on
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The code for the local board is programmed by 
 */

int
_bcm_topo_port_get(int unit, int dest_modid, bcm_port_t *exit_port)
{
    if (_topo_map == NULL) {
	return _bcm_topo_default_map(unit, dest_modid, exit_port);
    }

    return _topo_map(unit, dest_modid, exit_port);
}

