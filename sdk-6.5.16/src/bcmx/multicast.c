/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/multicast.c
 * Purpose:     BCMX Multicast APIs
 */

#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/lport.h>
#include <bcmx/multicast.h>

#include "bcmx_int.h"

#define BCMX_MULTICAST_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_MULTICAST_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MULTICAST_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MULTICAST_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MULTICAST_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


/*
 * Function:
 *     bcmx_multicast_init
 */
int
bcmx_multicast_init(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_multicast_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_MULTICAST_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_detach
 */
int
bcmx_multicast_detach(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_multicast_detach(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_MULTICAST_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_create
 * Notes:
 *     If group ID is not specified, the ID returned
 *     from the first successful 'create' is used for the remaining
 *     units.
 */
int
bcmx_multicast_create(uint32 flags, bcm_multicast_t *group)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(group);

    BCMX_UNIT_ITER(bcm_unit, i) {

        tmp_rv = bcm_multicast_create(bcm_unit, flags, group);
        BCM_IF_ERROR_RETURN
            (BCMX_MULTICAST_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));

        /*
         * Use the ID from first successful 'create' if policer ID
         * is not specified.
         */
        if (!(flags & BCM_MULTICAST_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                flags |= BCM_MULTICAST_WITH_ID;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmx_multicast_destroy
 */
int
bcmx_multicast_destroy(bcm_multicast_t group)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_multicast_destroy(bcm_unit, group);
        BCM_IF_ERROR_RETURN
            (BCMX_MULTICAST_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_group_get
 * Purpose:
 *     Retrieve the flags associated with a multicast group.
 * Parameters:
 *     group - (IN) Multicast group ID
 *     flags - (OUT) BCM_MULTICAST_*
 * Returns:
 *     BCM_E_xxx
 */
int
bcmx_multicast_group_get(bcm_multicast_t group, uint32 *flags)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_multicast_group_get(bcm_unit, group, flags);
        if (BCMX_MULTICAST_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}
     
/*
 * Function:
 *     bcmx_multicast_l3_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_l3_encap_get(bcm_multicast_t group,
                            bcm_gport_t port, bcm_if_t intf, 
                            bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_l3_encap_get(bcm_unit, group, port, intf, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_l2_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_l2_encap_get(bcm_multicast_t group, 
                            bcm_gport_t port, bcm_vlan_t vlan, 
                            bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_l2_encap_get(bcm_unit, group, port, vlan, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_vpls_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_vpls_encap_get(bcm_multicast_t group, 
                              bcm_gport_t port, bcm_gport_t mpls_port_id, 
                              bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_vpls_encap_get(bcm_unit, group,
                                        port, mpls_port_id, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_trill_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_trill_encap_get(bcm_multicast_t group, 
                               bcm_gport_t port, bcm_if_t intf, 
                               bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_trill_encap_get(bcm_unit, group,
                                         port, intf, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_subport_encap_get
 * Notes:
 *     Gport 'port' is a physical local port.
 */
int
bcmx_multicast_subport_encap_get(bcm_multicast_t group, 
                                 bcm_gport_t port, bcm_gport_t subport, 
                                 bcm_if_t *encap_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_subport_encap_get(bcm_unit, group,
                                           port, subport, encap_id);
}

/*
 * Function:
 *     bcmx_multicast_mim_encap_get
 * Notes:
 *     Gport 'port' can be a physical local port or a virtual port.
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_multicast_mim_encap_get(bcm_multicast_t group,
                             bcm_gport_t port, bcm_gport_t mim_port_id,
                             bcm_if_t *encap_id)
{
    int         rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_multicast_mim_encap_get(bcm_unit, group,
                                           port, mim_port_id, encap_id);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_multicast_mim_encap_get(bcm_unit, group,
                                         port, mim_port_id, encap_id);
        if (BCMX_MULTICAST_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_multicast_wlan_encap_get
 * Notes:
 *     Gport 'port' can be a physical local port or a virtual port.
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_multicast_wlan_encap_get(bcm_multicast_t group,
                              bcm_gport_t port, bcm_gport_t wlan_port_id,
                              bcm_if_t *encap_id)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_multicast_wlan_encap_get(bcm_unit, group,
                                            port, wlan_port_id, encap_id);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_multicast_wlan_encap_get(bcm_unit, group,
                                          port, wlan_port_id, encap_id);
        if (BCMX_MULTICAST_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_multicast_vlan_encap_get
 * Notes:
 *     Gport 'port' can be a physical local port or a virtual port.
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_multicast_vlan_encap_get(bcm_multicast_t group,
                              bcm_gport_t port, bcm_gport_t vlan_port_id,
                              bcm_if_t *encap_id)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_multicast_vlan_encap_get(bcm_unit, group,
                                            port, vlan_port_id, encap_id);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_multicast_vlan_encap_get(bcm_unit, group,
                                          port, vlan_port_id, encap_id);
        if (BCMX_MULTICAST_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_multicast_niv_encap_get
 * Notes:
 *     Gport 'port' can be a physical local port or a virtual port.
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_multicast_niv_encap_get(bcm_multicast_t group, bcm_gport_t port, 
                             bcm_gport_t niv_port_id, bcm_if_t *encap_id)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_id);

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_multicast_niv_encap_get(bcm_unit, group,
                                           port, niv_port_id, encap_id);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_multicast_niv_encap_get(bcm_unit, group,
                                         port, niv_port_id, encap_id);
        if (BCMX_MULTICAST_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_multicast_egress_add
 * Notes:
 *     Gport 'port' can be a physical local port or a virtual port.
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_multicast_egress_add(bcm_multicast_t group,
                          bcm_gport_t port, bcm_if_t encap_id)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_multicast_egress_add(bcm_unit, group, port, encap_id);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_multicast_egress_add(bcm_unit, group, port, encap_id);
        BCM_IF_ERROR_RETURN
            (BCMX_MULTICAST_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_egress_delete
 * Notes:
 *     Gport 'port' can be a physical local port or a virtual port.
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_multicast_egress_delete(bcm_multicast_t group, 
                             bcm_gport_t port, bcm_if_t encap_id)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));

        return bcm_multicast_egress_delete(bcm_unit, group, port, encap_id);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_multicast_egress_delete(bcm_unit, group, port, encap_id);
        BCM_IF_ERROR_RETURN
            (BCMX_MULTICAST_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *    bcmx_multicast_egress_delete_all
 */
int
bcmx_multicast_egress_delete_all(bcm_multicast_t group)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_multicast_egress_delete_all(bcm_unit, group);
        BCM_IF_ERROR_RETURN
            (BCMX_MULTICAST_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_egress_set
 * Notes:
 *     Each gport 'port' in array is a physical local port.
 */
int
bcmx_multicast_egress_set(bcm_multicast_t group, int port_count, 
                          bcm_gport_t *port_array, bcm_if_t *encap_id_array)
{
    int          rv = BCM_E_UNAVAIL;
    int          tmp_rv;
    int          i, bcm_unit;
    int          j, unit;
    bcm_port_t   unit_port;
    int          unit_port_count;
    bcm_gport_t  *unit_port_array;
    bcm_if_t     *unit_encap_id_array;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(port_count, port_array);
    BCMX_PARAM_ARRAY_NULL_CHECK(port_count, encap_id_array);

    if ((unit_port_array = sal_alloc(port_count * sizeof(*port_array),
                                     "bcmx_multicast_egress_set")) == NULL) {
        return BCM_E_MEMORY;
    }

    if ((unit_encap_id_array = sal_alloc(port_count * sizeof(*encap_id_array),
                                         "bcmx_multicast_egress_set")) == NULL) {
        sal_free(unit_port_array);
        return BCM_E_MEMORY;
    }        

    /* Need to gather all ports in a unit, then do a 'set' */
    BCMX_UNIT_ITER(bcm_unit, i) {
        unit_port_count = 0;

        /* Get all ports that belong to a unit */
        for (j = 0; j < port_count; j++) {
            if (BCM_FAILURE
                (_bcmx_dest_to_unit_port(port_array[j],
                                         &unit, &unit_port,
                                         BCMX_DEST_CONVERT_DEFAULT))) {
                continue;
            }
            if (unit != bcm_unit) {
                continue;
            }

            unit_port_array[unit_port_count] = port_array[j];
            unit_encap_id_array[unit_port_count] = encap_id_array[j];
            unit_port_count++;
        }

        if (unit_port_count == 0) {
            continue;
        }

        tmp_rv = bcm_multicast_egress_set(bcm_unit, group, unit_port_count,
                                          unit_port_array, unit_encap_id_array);

        if (BCM_FAILURE
            (BCMX_MULTICAST_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }
    }

    sal_free(unit_port_array);
    sal_free(unit_encap_id_array);

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_egress_get
 * Notes:
 *     Each gport 'port' is a physical local port.
 */
int
bcmx_multicast_egress_get(bcm_multicast_t group, int port_max, 
                          bcm_gport_t *port_array, bcm_if_t *encap_id_array, 
                          int *port_count)
{
    int          rv = BCM_E_UNAVAIL;
    int          tmp_rv;
    int          i, bcm_unit;
    int          port_max_left;
    int          unit_port_count;
    bcm_gport_t  *unit_port_array;
    bcm_if_t     *unit_encap_id_array;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(port_max, port_array);
    BCMX_PARAM_ARRAY_NULL_CHECK(port_max, encap_id_array);
    BCMX_PARAM_NULL_CHECK(port_count);

    *port_count   = 0;
    port_max_left = port_max;

    /* Need to gather egress ports from all units */
    BCMX_UNIT_ITER(bcm_unit, i) {

        if (port_max_left <= 0) {
            break;
        }

        unit_port_count = 0;
        unit_port_array = &port_array[*port_count];
        unit_encap_id_array = &encap_id_array[*port_count];

        tmp_rv = bcm_multicast_egress_get(bcm_unit, group, port_max_left,
                                          unit_port_array, unit_encap_id_array,
                                          &unit_port_count);
        
        if (BCMX_MULTICAST_GET_IS_VALID(bcm_unit, tmp_rv)) {
            /*
             * Ignore BCM_E_NOT_FOUND since objects are physical ports
             * and are added for specific unit.  Exit on other failure.
             */
            if (tmp_rv == BCM_E_NOT_FOUND) {
                if (rv == BCM_E_UNAVAIL) {
                    rv = tmp_rv;
                }
                continue;
            }
            rv = tmp_rv;
            if (BCM_FAILURE(tmp_rv)) {
                break;
            }
            *port_count   += unit_port_count;
            port_max_left -= unit_port_count;
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_multicast_repl_set
 * Notes:
 *     Each gport 'port' is a physical local port.
 */
int
bcmx_multicast_repl_set(int index,
		   bcmx_lport_t port,
		   bcm_vlan_vector_t vlan_vec)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_repl_set(bcm_unit, index, bcm_port, vlan_vec);
}

/*
 * Function:
 *     bcmx_multicast_repl_get
 * Notes:
 *     Each gport 'port' is a physical local port.
 */
int
bcmx_multicast_repl_get(int index,
		   bcmx_lport_t port,
		   bcm_vlan_vector_t vlan_vec)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MULTICAST_INIT_CHECK;

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT));

    return bcm_multicast_repl_get(bcm_unit, index, bcm_port, vlan_vec);
}

/*
 * Function:
 *      bcmx_multicast_control_set
 * Purpose:
 *      Set multicast group control.
 * Parameters:
 *      group - (IN) Multicast group ID.
 *      type  - (IN) Multicast group control type.
 *      arg   - (IN) Multicast group control argument.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_multicast_control_set(bcm_multicast_t group,
                           bcm_multicast_control_t type, int arg)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_multicast_control_set(bcm_unit, group, type, arg);
        BCM_IF_ERROR_RETURN
            (BCMX_MULTICAST_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_multicast_control_get
 * Purpose:
 *      Get multicast group control.
 * Parameters:
 *      group - (IN) Multicast group ID.
 *      type  - (IN) Multicast group control type.
 *      arg   - (OUT) Multicast group control argument.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_multicast_control_get(bcm_multicast_t group,
                           bcm_multicast_control_t type, int *arg)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_MULTICAST_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(arg);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_multicast_control_get(bcm_unit, group, type, arg);
        if (BCMX_MULTICAST_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}
