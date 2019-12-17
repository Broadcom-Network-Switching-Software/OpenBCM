/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/trill.c
 * Purpose:     BCMX Trill APIs
 */

#ifdef INCLUDE_L3

#include <sal/core/libc.h>

#include <bcm/types.h>

#include <bcmx/lport.h>
#include <bcmx/trill.h>

#include "bcmx_int.h"

#define BCMX_TRILL_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_TRILL_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_TRILL_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_TRILL_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_TRILL_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)

#define BCMX_TRILL_ERROR_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_TRILL_PORT_T_PTR_TO_BCM(_trill_port)    \
    ((bcm_trill_port_t *)(_trill_port))


/*
 * Function:
 *     bcmx_trill_port_t_init
 * Purpose:
 *     Trill port structure initializer.
 * Parameters:
 *     trill_port - (IN/OUT) TRILL port structure to initialize
 * Returns:
 *     BCM_E_XXX
 */
void
bcmx_trill_port_t_init(bcmx_trill_port_t *trill_port)
{
    if (trill_port != NULL) {
        sal_memset(trill_port, 0, sizeof(*trill_port));
    }
}

/*
 * Function:
 *     bcmx_trill_multicast_entry_t_init
 * Purpose:
 *     Trill Multicast entry structure initializer.
 * Parameters:
 *     trill_mc_entry - (IN/OUT) TRILL Multicast entry structure to initialize
 * Returns:
 *     BCM_E_XXX
 */
void
bcmx_trill_multicast_entry_t_init(bcmx_trill_multicast_entry_t *trill_mc_entry)
{
    if (trill_mc_entry != NULL) {
        sal_memset(trill_mc_entry, 0, sizeof(*trill_mc_entry));
    }
}

/*
 * Function:
 *     bcmx_trill_init
 * Purpose:
 *     Initialize the TRILL module.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_init(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_TRILL_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_cleanup
 * Purpose:
 *     Detach the TRILL module, clear all HW states.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_cleanup(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_cleanup(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_TRILL_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_port_add
 * Purpose:
 *     Add TRILL port to TRILL Cloud.
 * Parameters:
 *     trill_port - (IN/OUT) TRILL port information
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_port_add(bcmx_trill_port_t *trill_port)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    uint32  flags_orig;

    BCMX_TRILL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(trill_port);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = trill_port->flags & BCM_TRILL_PORT_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_port_add(bcm_unit,
                                    BCMX_TRILL_PORT_T_PTR_TO_BCM(trill_port));
        if (BCM_FAILURE(BCMX_TRILL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * Use the ID from first successful 'create' if TRILL port ID
         * is not specified.
         */
        if (!(trill_port->flags & BCM_TRILL_PORT_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                trill_port->flags |= BCM_TRILL_PORT_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    trill_port->flags &= ~BCM_TRILL_PORT_WITH_ID;
    trill_port->flags |= flags_orig;


    return rv;
}

/*
 * Function:
 *     bcmx_trill_port_delete
 * Purpose:
 *     Delete TRILL port from TRILL Cloud.
 * Parameters:
 *     trill_port_id - TRILL port ID
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_port_delete(bcm_gport_t trill_port_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_port_delete(bcm_unit, trill_port_id);
        BCM_IF_ERROR_RETURN
            (BCMX_TRILL_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_port_delete_all
 * Purpose:
 *     Delete all TRILL ports from TRILL Cloud.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_port_delete_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_port_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_TRILL_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_port_get
 * Purpose:
 *     Get TRILL port information for given ID.
 * Parameters:
 *     trill_port - (IN/OUT) TRILL port configuration info
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_port_get(bcmx_trill_port_t *trill_port)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(trill_port);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_trill_port_get(bcm_unit,
                                BCMX_TRILL_PORT_T_PTR_TO_BCM(trill_port));
        if (BCMX_TRILL_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_trill_port_get_all
 * Purpose:
 *     Get all TRILL ports from TRILL Cloud.
 * Parameters:
 *     port_max   - Maximum number of TRILL ports in array
 *     port_array - (OUT) Array of TRILL ports
 *     port_count - (OUT) Number of TRILL ports returned in array
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_port_get_all(int port_max,
                        bcmx_trill_port_t *port_array, int *port_count)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(port_max, port_array);
    BCMX_PARAM_NULL_CHECK(port_count);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_trill_port_get_all(bcm_unit, port_max,
                                    BCMX_TRILL_PORT_T_PTR_TO_BCM(port_array),
                                    port_count);
        if (BCMX_TRILL_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_trill_multicast_entry_add
 * Purpose:
 *     Add TRILL multicast entry.
 * Parameters:
 *     trill_mc_entry     - Trill Multicast Entry
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_multicast_entry_add(bcm_trill_multicast_entry_t  *trill_mc_entry)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_multicast_entry_add(bcm_unit, trill_mc_entry);
        BCM_IF_ERROR_RETURN(BCMX_TRILL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_multicast_delete
 * Purpose:
 *     Delete TRILL multicast entry.
 * Parameters:
 *     trill_mc_entry     - Trill Multicast Entry
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_multicast_entry_delete(bcm_trill_multicast_entry_t  *trill_mc_entry)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_multicast_entry_delete(bcm_unit, trill_mc_entry);
        BCM_IF_ERROR_RETURN
            (BCMX_TRILL_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_multicast_entry_get
 * Purpose:
 *     Get TRILL multicast entry.
 * Parameters:
 *     trill_mc_entry     - Trill Multicast Entry
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_multicast_entry_get(bcm_trill_multicast_entry_t  *trill_mc_entry)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_multicast_entry_get(bcm_unit, trill_mc_entry);
        BCM_IF_ERROR_RETURN
            (BCMX_TRILL_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_multicast_delete
 * Purpose:
 *     Delete all TRILL multicast entries with given root name.
 * Parameters:
 *     root_name - TRILL Root_RBridge
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_multicast_delete_all(bcm_trill_name_t root_name)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_multicast_delete_all(bcm_unit, root_name);
        BCM_IF_ERROR_RETURN
            (BCMX_TRILL_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_multicast_source_add
 * Purpose:
 *     Add TRILL multicast RPF entry.
 * Parameters:
 *     root_name   - Trill Root_RBridge
 *     source_name - Source Rbridge
 *     port        - Ingress port
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_multicast_source_add(bcm_trill_name_t root_name,
                                bcm_trill_name_t source_rbridge_name,
                                bcm_gport_t port,
                                bcm_if_t  encap_intf)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_multicast_source_add(bcm_unit, root_name,
                                                source_rbridge_name, port, encap_intf);
        BCM_IF_ERROR_RETURN(BCMX_TRILL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_multicast_source_delete
 * Purpose:
 *     Delete TRILL multicast RPF entry.
 * Parameters:
 *     root_name   - Trill Root_RBridge
 *     source_name - Source Rbridge
 *     port        - Ingress port
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_multicast_source_delete(bcm_trill_name_t root_name,
                                   bcm_trill_name_t source_rbridge_name,
                                bcm_gport_t port,
                                bcm_if_t  encap_intf)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_trill_multicast_source_delete(bcm_unit, root_name,
                                                   source_rbridge_name, port, encap_intf);
        BCM_IF_ERROR_RETURN
            (BCMX_TRILL_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_trill_multicast_source_get.
 * Purpose:
 *     Get TRILL multicast RPF entry.
 * Parameters:
 *     root_name   - Trill Root_RBridge
 *     source_name - Source Rbridge
 *     port        - (OUT) Ingress port
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_trill_multicast_source_get(bcm_trill_name_t root_name,
                                bcm_trill_name_t source_rbridge_name,
                                bcm_gport_t *port)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_TRILL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(port);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_trill_multicast_source_get(bcm_unit, root_name,
                                            source_rbridge_name, port);
        if (BCMX_TRILL_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_trill_stat_get/get32
 * Purpose:
 *     Get TRILL Stats.
 * Parameters:
 *     port - Port
 *     stat - Specify the Stat type
 *     val  - (OUT) 64-bit/32-bit Stats value
 * Returns:
 *     BCM_E_XXX
 */     
int
bcmx_trill_stat_get(bcm_gport_t port, bcm_trill_stat_t stat, uint64 *val)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TRILL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(val);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_trill_stat_get(bcm_unit, bcm_port, stat, val);
}

int
bcmx_trill_stat_get32(bcm_gport_t port, bcm_trill_stat_t stat, uint32 *val)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TRILL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(val);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_trill_stat_get32(bcm_unit, bcm_port, stat, val);
}

/*
 * Function:
 *     bcmx_trill_stat_clear
 * Purpose:
 *     Clear TRILL Stats.
 * Parameters:
 *     port - Port
 *     stat - Specify the Stat type
 * Returns:
 *     BCM_E_XXX
 */     
int
bcmx_trill_stat_clear(bcm_gport_t port, bcm_trill_stat_t stat)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TRILL_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_trill_stat_clear(bcm_unit, bcm_port, stat);
}

#endif /* INCLUDE_L3 */
