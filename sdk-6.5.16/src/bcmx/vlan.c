/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        src/bcmx/vlan.c
 * Purpose:     BCMX Virtual LAN (VLAN) APIs
 */

#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/vlan.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

#define BCMX_VLAN_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_VLAN_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_VLAN_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_VLAN_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)

#define BCMX_VLAN_ERROR_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_VLAN_PORT_T_PTR_TO_BCM(_vlan_port)   \
    ((bcm_vlan_port_t *)(_vlan_port))


/*
 * Function:
 *     bcmx_vlan_port_t_init
 */
void
bcmx_vlan_port_t_init(bcmx_vlan_port_t *vlan_port)
{
    if (vlan_port != NULL) {
        bcm_vlan_port_t_init
            (BCMX_VLAN_PORT_T_PTR_TO_BCM(vlan_port));
    }
}


int 
bcmx_vlan_create(bcm_vlan_t vid)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_create(bcm_unit, vid);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int 
bcmx_vlan_destroy(bcm_vlan_t vid)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_destroy(bcm_unit, vid);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_destroy_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_destroy_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_vlan_port_add
 * Notes:
 *      Currently assumes stack ports are added by BCM on vlan create.
 */

int
bcmx_vlan_port_add(bcm_vlan_t vid, 
                   bcmx_lplist_t lplist,
                   bcmx_lplist_t ut_lplist)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    bcm_pbmp_t pbmp, upbmp;
    int bcm_unit;
    int i;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        BCMX_LPLIST_TO_PBMP(ut_lplist, bcm_unit, upbmp);
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            tmp_rv = bcm_vlan_port_add(bcm_unit, vid, pbmp, upbmp);
            BCM_IF_ERROR_RETURN
                (BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}


int
bcmx_vlan_port_remove(bcm_vlan_t vid, bcmx_lplist_t lplist)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    bcm_pbmp_t pbmp;
    int bcm_unit;
    int i;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            tmp_rv = bcm_vlan_port_remove(bcm_unit, vid, pbmp);
            BCM_IF_ERROR_RETURN
                (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}


int
bcmx_vlan_port_get(bcm_vlan_t vid,
                   bcmx_lplist_t *lplist,
                   bcmx_lplist_t *ut_lplist)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    bcm_pbmp_t pbmp, upbmp;
    int bcm_unit;
    int i;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(lplist);
    BCMX_PARAM_NULL_CHECK(ut_lplist);
    
    bcmx_lplist_clear(lplist);
    bcmx_lplist_clear(ut_lplist);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_port_get(bcm_unit, vid, &pbmp, &upbmp);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_FAILURE(tmp_rv)) {
                break;
            }

            rv = BCMX_LPLIST_PBMP_ADD(lplist, bcm_unit, pbmp);
            rv = BCMX_LPLIST_PBMP_ADD(ut_lplist, bcm_unit, upbmp);
        }
    }

    return rv;
}


int
bcmx_vlan_default_get(bcm_vlan_t *vid_ptr)
{
    int rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vid_ptr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_default_get(bcm_unit, vid_ptr);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_vlan_default_set(bcm_vlan_t vid)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_default_set(bcm_unit, vid);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_stg_get(bcm_vlan_t vid, bcm_stg_t *stg_ptr)
{
    int rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(stg_ptr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_stg_get(bcm_unit, vid, stg_ptr);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_vlan_stg_set(bcm_vlan_t vid, bcm_stg_t stg)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stg_set(bcm_unit, vid, stg);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_port_default_action_set(bcmx_lport_t port,
                                  bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_default_action_set(bcm_unit, bcm_port,
                                            action);
}

int
bcmx_vlan_port_default_action_get(bcmx_lport_t port,
                                  bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_default_action_get(bcm_unit, bcm_port,
                                            action);
}

int
bcmx_vlan_port_default_action_delete(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_default_action_delete(bcm_unit, bcm_port);
}

int
bcmx_vlan_port_egress_default_action_set(bcmx_lport_t port,
                                         bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_egress_default_action_set(bcm_unit, bcm_port,
                                                   action);
}
 
int
bcmx_vlan_port_egress_default_action_get(bcmx_lport_t port,
                                         bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_egress_default_action_get(bcm_unit, bcm_port,
                                                   action);
}   

int
bcmx_vlan_port_egress_default_action_delete(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_egress_default_action_delete(bcm_unit, bcm_port);
}

int
bcmx_vlan_port_protocol_action_add(bcmx_lport_t port,
                                   bcm_port_frametype_t frame,
                                   bcm_port_ethertype_t ether,
                                   bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_protocol_action_add(bcm_unit, bcm_port,
                                             frame, ether, action);
}

int
bcmx_vlan_port_protocol_action_get(bcmx_lport_t port, 
                                   bcm_port_frametype_t frame, 
                                   bcm_port_ethertype_t ether, 
                                   bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_protocol_action_get(bcm_unit, bcm_port,
                                             frame, ether, action);
}

int
bcmx_vlan_port_protocol_action_delete(bcmx_lport_t port, 
                                      bcm_port_frametype_t frame, 
                                      bcm_port_ethertype_t ether)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_protocol_action_delete(bcm_unit, bcm_port,
                                                frame, ether);
}

int
bcmx_vlan_port_protocol_action_delete_all(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_protocol_action_delete_all(bcm_unit, bcm_port);
}


int
bcmx_vlan_mac_add(bcm_mac_t mac, bcm_vlan_t vid, int prio)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_add(bcm_unit, mac, vid, prio);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_mac_delete(bcm_mac_t mac)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_delete(bcm_unit, mac);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_mac_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int bcmx_vlan_mac_action_add(bcm_mac_t mac,
                             bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_action_add(bcm_unit, mac, action);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_mac_action_delete(bcm_mac_t mac)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_action_delete(bcm_unit, mac);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_mac_action_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_action_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_mac_action_get(bcm_mac_t mac, 
                         bcm_vlan_action_set_t *action)
{
    int rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_mac_action_get(bcm_unit, mac, action);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_vlan_translate_add(bcmx_lport_t port,
                        bcm_vlan_t old_vid,
                        bcm_vlan_t new_vid,
                        int prio)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_add(bcm_unit, bcm_port,
                                      old_vid, new_vid, prio);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_add(bcm_unit, port,
                                        old_vid, new_vid, prio);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_delete(bcmx_lport_t port, bcm_vlan_t old_vid)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_delete(bcm_unit, bcm_port, old_vid);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_delete(bcm_unit, port, old_vid);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_get(bcmx_lport_t port, bcm_vlan_t old_vid,
                        bcm_vlan_t *new_vid, int *prio)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(prio);
    
    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_get(bcm_unit, bcm_port,
                                      old_vid, new_vid, prio);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_translate_get(bcm_unit, port, old_vid, new_vid, prio);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_vlan_translate_action_add(bcm_gport_t port, 
                               bcm_vlan_translate_key_t key_type, 
                               bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                               bcm_vlan_action_set_t *action)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_action_add(bcm_unit, port,
                                             key_type, outer_vlan, inner_vlan,
                                             action);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_action_add(bcm_unit, port,
                                               key_type, outer_vlan,
                                               inner_vlan, action);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_action_delete(bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_action_delete(bcm_unit, port,
                                                key_type, outer_vlan,
                                                inner_vlan);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_action_delete(bcm_unit, port,
                                                  key_type, outer_vlan,
                                                  inner_vlan);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_action_get(bcm_gport_t port, 
                               bcm_vlan_translate_key_t key_type, 
                               bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                               bcm_vlan_action_set_t *action)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_action_get(bcm_unit, port, key_type,
                                             outer_vlan, inner_vlan,
                                             action);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_translate_action_get(bcm_unit, port, key_type,
                                           outer_vlan, inner_vlan,
                                           action);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_vlan_translate_egress_add(bcmx_lport_t port,
                               bcm_vlan_t old_vid,
                               bcm_vlan_t new_vid,
                               int prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_egress_add(bcm_unit, bcm_port,
                                         old_vid, new_vid, prio);
}

int
bcmx_vlan_translate_egress_delete(bcmx_lport_t port, bcm_vlan_t old_vid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_egress_delete(bcm_unit, bcm_port,
                                            old_vid);
}

int
bcmx_vlan_translate_egress_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_egress_get(bcmx_lport_t port, bcm_vlan_t old_vid, 
                               bcm_vlan_t *new_vid, int *prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(prio);
    
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_egress_get(bcm_unit, bcm_port, old_vid,
                                         new_vid, prio);
}


int
bcmx_vlan_translate_egress_action_add(int port_class,
                                      bcm_vlan_t outer_vlan,
                                      bcm_vlan_t inner_vlan,
                                      bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_action_add(bcm_unit, port_class,
                                                      outer_vlan, inner_vlan,
                                                      action);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_egress_action_delete(int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_action_delete(bcm_unit, port_class,
                                                         outer_vlan, inner_vlan);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_egress_action_get(int port_class, 
                                      bcm_vlan_t outer_vlan, 
                                      bcm_vlan_t inner_vlan, 
                                      bcm_vlan_action_set_t *action)
{
    int rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_translate_egress_action_get(bcm_unit, port_class,
                                                  outer_vlan, inner_vlan,
                                                  action);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_vlan_dtag_add(bcmx_lport_t port,
                   bcm_vlan_t old_vid,
                   bcm_vlan_t new_vid,
                   int prio)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_dtag_add(bcm_unit, bcm_port, old_vid, new_vid, prio);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_dtag_add(bcm_unit, port, old_vid, new_vid, prio);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_dtag_delete(bcmx_lport_t port, bcm_vlan_t old_vid)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_dtag_delete(bcm_unit, bcm_port, old_vid);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_dtag_delete(bcm_unit, port, old_vid);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_dtag_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_dtag_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_dtag_get(bcmx_lport_t port, bcm_vlan_t old_vid, 
                   bcm_vlan_t *new_vid, int *prio)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(prio);
    
    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_dtag_get(bcm_unit, bcm_port, old_vid, new_vid, prio);
    }
    
    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_dtag_get(bcm_unit, port, old_vid, new_vid, prio);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_vlan_translate_range_add(bcmx_lport_t port,
                              bcm_vlan_t old_vid_low, bcm_vlan_t old_vid_high,
                              bcm_vlan_t new_vid, int int_prio)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_range_add(bcm_unit, bcm_port,
                                            old_vid_low, old_vid_high,
                                            new_vid, int_prio);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_range_add(bcm_unit, port,
                                              old_vid_low, old_vid_high,
                                              new_vid, int_prio);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_range_delete(bcmx_lport_t port,
                                 bcm_vlan_t old_vid_low,
                                 bcm_vlan_t old_vid_high)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_range_delete(bcm_unit, bcm_port,
                                               old_vid_low, old_vid_high);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_range_delete(bcm_unit, port,
                                                 old_vid_low, old_vid_high);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_range_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_range_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_range_get(bcmx_lport_t port, 
                              bcm_vlan_t old_vid_low, bcm_vlan_t old_vid_high, 
                              bcm_vlan_t *new_vid, int *int_prio)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(int_prio);
    
    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_range_get(bcm_unit, bcm_port,
                                            old_vid_low, old_vid_high,
                                            new_vid, int_prio);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_translate_range_get(bcm_unit, port,
                                          old_vid_low, old_vid_high,
                                          new_vid, int_prio);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_vlan_dtag_range_add(bcmx_lport_t port,
                         bcm_vlan_t old_vid_low, bcm_vlan_t old_vid_high,
                         bcm_vlan_t new_vid, int int_prio)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_dtag_range_add(bcm_unit, bcm_port,
                                       old_vid_low, old_vid_high,
                                       new_vid, int_prio);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_dtag_range_add(bcm_unit, port,
                                         old_vid_low, old_vid_high,
                                         new_vid, int_prio);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_dtag_range_delete(bcmx_lport_t port, 
                            bcm_vlan_t old_vid_low,
                            bcm_vlan_t old_vid_high)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_dtag_range_delete(bcm_unit, bcm_port,
                                          old_vid_low, old_vid_high);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_dtag_range_delete(bcm_unit, port,
                                            old_vid_low, old_vid_high);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_dtag_range_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_dtag_range_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_dtag_range_get(bcmx_lport_t port,
                         bcm_vlan_t old_vid_low, bcm_vlan_t old_vid_high, 
                         bcm_vlan_t *new_vid, int *prio)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(prio);
    
    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_dtag_range_get(bcm_unit, bcm_port,
                                       old_vid_low, old_vid_high,
                                       new_vid, prio);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_dtag_range_get(bcm_unit, port,
                                     old_vid_low, old_vid_high,
                                     new_vid, prio);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_vlan_translate_action_range_add(bcm_gport_t port,
                                     bcm_vlan_t outer_vlan_low, 
                                     bcm_vlan_t outer_vlan_high, 
                                     bcm_vlan_t inner_vlan_low, 
                                     bcm_vlan_t inner_vlan_high, 
                                     bcm_vlan_action_set_t *action)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_action_range_add(bcm_unit, port,
                                                   outer_vlan_low,
                                                   outer_vlan_high,
                                                   inner_vlan_low, 
                                                   inner_vlan_high, 
                                                   action);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_action_range_add(bcm_unit, port,
                                                     outer_vlan_low,
                                                     outer_vlan_high,
                                                     inner_vlan_low, 
                                                     inner_vlan_high, 
                                                     action);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_action_range_delete(bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low, 
                                        bcm_vlan_t outer_vlan_high, 
                                        bcm_vlan_t inner_vlan_low, 
                                        bcm_vlan_t inner_vlan_high)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_action_range_delete(bcm_unit, port,
                                                      outer_vlan_low,
                                                      outer_vlan_high,
                                                      inner_vlan_low, 
                                                      inner_vlan_high);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_action_range_delete(bcm_unit, port,
                                                        outer_vlan_low,
                                                        outer_vlan_high,
                                                        inner_vlan_low, 
                                                        inner_vlan_high);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_action_range_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_action_range_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_action_range_get(bcm_gport_t port, 
                                     bcm_vlan_t outer_vlan_low, 
                                     bcm_vlan_t outer_vlan_high, 
                                     bcm_vlan_t inner_vlan_low, 
                                     bcm_vlan_t inner_vlan_high, 
                                     bcm_vlan_action_set_t *action)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_LPORT_CHECK(port);

    /* Physical port */
    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));
        return bcm_vlan_translate_action_range_get(bcm_unit, port,
                                                   outer_vlan_low,
                                                   outer_vlan_high,
                                                   inner_vlan_low,
                                                   inner_vlan_high,
                                                   action);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv =  bcm_vlan_translate_action_range_get(bcm_unit, port,
                                                  outer_vlan_low,
                                                  outer_vlan_high,
                                                  inner_vlan_low,
                                                  inner_vlan_high,
                                                  action);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_vlan_ip_add(bcm_vlan_ip_t *vlan_ip)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_ip);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_add(bcm_unit, vlan_ip);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_ip_delete(bcm_vlan_ip_t *vlan_ip)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_ip);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_delete(bcm_unit, vlan_ip);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_ip_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_ip_action_add(bcm_vlan_ip_t *vlan_ip,
                        bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_ip);
    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_action_add(bcm_unit, vlan_ip, action);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;

}

int
bcmx_vlan_ip_action_get(bcm_vlan_ip_t *vlan_ip,
                        bcm_vlan_action_set_t *action)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_ip);
    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_ip_action_get(bcm_unit, vlan_ip, action);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_vlan_ip_action_delete(bcm_vlan_ip_t *vlan_ip)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_ip);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_action_delete(bcm_unit, vlan_ip);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;

}

int
bcmx_vlan_ip_action_delete_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_action_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_control_set(bcm_vlan_control_t type, int arg)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_control_set(bcm_unit, type, arg);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_vlan_control_get(bcm_vlan_control_t type, int *arg)
{
    int rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(arg);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_control_get(bcm_unit, type, arg);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_vlan_control_port_set
 * Notes:
 *      Exposing the ability of the BCM API to set all Ethernet ports
 *      the BCMX layer provides an special value defined for this purpose:
 *          BCMX_LPORT_ETHER_ALL  --> All Ethernet ports (FE, GE, XE)
 */

int
bcmx_vlan_control_port_set(bcmx_lport_t port, 
                           bcm_vlan_control_port_t type,
                           int arg)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    int bcm_port_all = -1; /* This value indicates all ether ports in BCM */

    BCMX_VLAN_INIT_CHECK;

    if (port == BCMX_LPORT_ETHER_ALL) {
        BCMX_UNIT_ITER(bcm_unit, i) {
            tmp_rv = bcm_vlan_control_port_set(bcm_unit, bcm_port_all, 
                                               type, arg);
            BCM_IF_ERROR_RETURN
                (BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    } else {
        int         bcm_unit;
        bcm_port_t  bcm_port;

        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));

        return bcm_vlan_control_port_set(bcm_unit, bcm_port,
                                         type, arg);
    }

    return rv;
}


int
bcmx_vlan_control_port_get(bcmx_lport_t port,
                           bcm_vlan_control_port_t type,
                           int *arg)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(arg);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_control_port_get(bcm_unit, bcm_port,
                                     type, arg);
}


int
bcmx_vlan_mcast_flood_set(bcm_vlan_t vlan,
                          bcm_vlan_mcast_flood_t mode)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mcast_flood_set(bcm_unit, vlan, mode);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_mcast_flood_get(bcm_vlan_t vlan,
                          bcm_vlan_mcast_flood_t *mode)
{
    int rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_mcast_flood_get(bcm_unit, vlan, mode);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_vlan_control_vlan_set
 */
int 
bcmx_vlan_control_vlan_set(bcm_vlan_t vlan,
                           bcm_vlan_control_vlan_t control) 
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_control_vlan_set(bcm_unit, vlan, control);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_control_vlan_get
 */
int
bcmx_vlan_control_vlan_get(bcm_vlan_t vlan,
                           bcm_vlan_control_vlan_t *control) 
{
    int rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(control);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_control_vlan_get(bcm_unit, vlan, control);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_vlan_vector_flags_set
 */
int
bcmx_vlan_vector_flags_set(bcm_vlan_vector_t vlan_vector,
                           uint32 flags_mask, uint32 flags_value)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_vector_flags_set(bcm_unit, vlan_vector,
                                           flags_mask, flags_value);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_cross_connect_add
 */
int
bcmx_vlan_cross_connect_add(bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                            bcm_gport_t port_1, bcm_gport_t port_2)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_cross_connect_add(bcm_unit,
                                            outer_vlan, inner_vlan,
                                            port_1, port_2);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_cross_connect_delete
 */
int bcmx_vlan_cross_connect_delete(bcm_vlan_t outer_vlan, 
                                   bcm_vlan_t inner_vlan)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_cross_connect_delete(bcm_unit,
                                               outer_vlan, inner_vlan);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_cross_connect_delete_all
 */
int
bcmx_vlan_cross_connect_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_cross_connect_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

void
bcmx_vlan_block_t_init(bcmx_vlan_block_t *data)
{
    bcmx_lplist_t_init(&data->known_multicast);
    bcmx_lplist_t_init(&data->unknown_multicast);
    bcmx_lplist_t_init(&data->unknown_unicast);
    bcmx_lplist_t_init(&data->broadcast);
}

int
bcmx_vlan_block_get(bcm_vlan_t vlan, bcmx_vlan_block_t *vlan_block)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_vlan_block_t bcm_vlb;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_block);

    bcmx_vlan_block_t_init(vlan_block);
    BCMX_UNIT_ITER(bcm_unit, i) {
        bcm_vlan_block_t_init(&bcm_vlb);
        tmp_rv = bcm_vlan_block_get(bcm_unit, vlan, &bcm_vlb);

        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_FAILURE(tmp_rv)) {
                break;
            }

            BCM_IF_ERROR_RETURN
                (bcmx_lplist_pbmp_add(&vlan_block->known_multicast,
                                      bcm_unit, &bcm_vlb.known_multicast));
            BCM_IF_ERROR_RETURN
                (bcmx_lplist_pbmp_add(&vlan_block->unknown_multicast,
                                      bcm_unit, &bcm_vlb.unknown_multicast));
            BCM_IF_ERROR_RETURN
                (bcmx_lplist_pbmp_add(&vlan_block->unknown_unicast,
                                      bcm_unit, &bcm_vlb.unknown_unicast));
            BCM_IF_ERROR_RETURN
                (bcmx_lplist_pbmp_add(&vlan_block->broadcast,
                                      bcm_unit, &bcm_vlb.broadcast));
        }
    }

    return rv;
}

int
bcmx_vlan_block_set(bcm_vlan_t vlan, bcmx_vlan_block_t *vlan_block)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_vlan_block_t bcm_vlb;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_block);

    BCMX_UNIT_ITER(bcm_unit, i) {
        bcm_vlan_block_t_init(&bcm_vlb);
        BCMX_LPLIST_TO_PBMP(vlan_block->known_multicast,
                            bcm_unit, bcm_vlb.known_multicast);
        BCMX_LPLIST_TO_PBMP(vlan_block->unknown_multicast,
                            bcm_unit, bcm_vlb.unknown_multicast);
        BCMX_LPLIST_TO_PBMP(vlan_block->unknown_unicast,
                            bcm_unit, bcm_vlb.unknown_unicast);
        BCMX_LPLIST_TO_PBMP(vlan_block->broadcast,
                            bcm_unit, bcm_vlb.broadcast);
        tmp_rv = bcm_vlan_block_set(bcm_unit, vlan, &bcm_vlb);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }
    
    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated VLAN.
 * Parameters:
 *      vlan   - VLAN Id
 *      enable - Non-zero to enable counter collection, zero to disable
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_enable_set(bcm_vlan_t vlan, int enable)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_enable_set(bcm_unit, vlan, enable);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      vlan      - VLAN Id
 *      cos       - COS or priority
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_multi_get(bcm_vlan_t vlan, bcm_cos_t cos, int nstat, 
                         bcm_vlan_stat_t *stat_arr, uint64 *value_arr)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    uint64  *tmp_val;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    tmp_val = sal_alloc(sizeof(uint64) * nstat, "bcmx vlan stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        COMPILER_64_ZERO(value_arr[i]);
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_multi_get(bcm_unit, vlan, cos, nstat,
                                         stat_arr, tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (i = 0; i < nstat; i++) {
                    COMPILER_64_ADD_64(value_arr[i], tmp_val[i]);
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_val);

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      vlan      - VLAN Id
 *      cos       - COS or priority
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_multi_get32(bcm_vlan_t vlan, bcm_cos_t cos, int nstat,
                           bcm_vlan_stat_t *stat_arr, uint32 *value_arr)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    uint32  *tmp_val;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    tmp_val = sal_alloc(sizeof(uint32) * nstat, "bcmx vlan stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        value_arr[i] = 0;
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_multi_get32(bcm_unit, vlan, cos, nstat,
                                           stat_arr, tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (i = 0; i < nstat; i++) {
                    value_arr[i] += tmp_val[i];
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_val);

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      vlan      - VLAN Id
 *      cos       - COS or priority
 *      nstat     - Number of elements in stat array
 *      stat_arr  - New statistics descriptors array
 *      value_arr - New counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_multi_set(bcm_vlan_t vlan, bcm_cos_t cos, int nstat, 
                         bcm_vlan_stat_t *stat_arr, uint64 *value_arr)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_multi_set(bcm_unit, vlan, cos, nstat,
                                         stat_arr, value_arr);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      vlan      - VLAN Id
 *      cos       - COS or priority
 *      nstat     - Number of elements in stat array
 *      stat_arr  - New statistics descriptors array
 *      value_arr - New counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_multi_set32(bcm_vlan_t vlan, bcm_cos_t cos, int nstat, 
                           bcm_vlan_stat_t *stat_arr, uint32 *value_arr)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_multi_set32(bcm_unit, vlan, cos, nstat,
                                           stat_arr, value_arr);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated
 *      ingress VLAN translation.
 * Parameters:
 *      port       - Generic port, physical (lport) or virtual port
 *      key_type   - Key Type : bcmVlanTranslateKey*
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      enable     - Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_vlan_translate_stat_enable_set(bcm_gport_t port, 
                                    bcm_vlan_translate_key_t key_type, 
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan, 
                                    int enable)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_VLAN_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    /* Physical port, apply to unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_translate_stat_enable_set(bcm_unit, port, key_type,
                                                  outer_vlan, inner_vlan,
                                                  enable);
    }

    /* Virtual port, enable/disable stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_stat_enable_set(bcm_unit, port, key_type,
                                                    outer_vlan, inner_vlan,
                                                    enable);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_stat_get/get32
 * Purpose:
 *      Extract ingress VLAN translation statistics from the chip.
 * Parameters:
 *      port       - Generic port, physical (lport) or virtual port
 *      key_type   - Key Type : bcmVlanTranslateKey*
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      stat       - Type of the counter to retrieve.
 *      val        - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_vlan_translate_stat_get(bcm_gport_t port,
                             bcm_vlan_translate_key_t key_type, 
                             bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                             bcm_vlan_stat_t stat, uint64 *val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint64      tmp_val;
    int         i;

    BCMX_VLAN_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(val);

    /* Physical port, get stat from unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_translate_stat_get(bcm_unit, port, key_type,
                                           outer_vlan, inner_vlan, stat, val);
    }

    /* Virtual port, gather stats from all units */
    COMPILER_64_ZERO(*val);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_stat_get(bcm_unit, port, key_type,
                                             outer_vlan, inner_vlan, stat,
                                             &tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                COMPILER_64_ADD_64(*val, tmp_val);
            } else {
                break;
            }
        }
    }

    return rv;
}

int
bcmx_vlan_translate_stat_get32(bcm_gport_t port, 
                               bcm_vlan_translate_key_t key_type, 
                               bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                               bcm_vlan_stat_t stat, uint32 *val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint32      tmp_val;
    int         i;

    BCMX_VLAN_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(val);

    /* Physical port, get stat from unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_translate_stat_get32(bcm_unit, port, key_type,
                                             outer_vlan, inner_vlan, stat, val);
    }

    /* Virtual port, gather stats from all units */
    *val = 0;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_stat_get32(bcm_unit, port, key_type,
                                               outer_vlan, inner_vlan, stat,
                                               &tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                *val += tmp_val;
            } else {
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_stat_set/set32
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified ingress VLAN translation.
 * Parameters:
 *      port       - Generic port, physical (lport) or virtual port
 *      key_type   - Key Type : bcmVlanTranslateKey*
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      stat       - Type of the counter to retrieve.
 *      val        - New counter value
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_vlan_translate_stat_set(bcm_gport_t port, 
                             bcm_vlan_translate_key_t key_type, 
                             bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                             bcm_vlan_stat_t stat, uint64 val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_VLAN_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    /* Physical port, set stat in unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_translate_stat_set(bcm_unit, port, key_type,
                                           outer_vlan, inner_vlan, stat, val);
    }

    /* Virtual port, set stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_stat_set(bcm_unit, port, key_type,
                                             outer_vlan, inner_vlan, stat,
                                             val);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_stat_set32(bcm_gport_t port, 
                               bcm_vlan_translate_key_t key_type, 
                               bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                               bcm_vlan_stat_t stat, uint32 val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_VLAN_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    /* Physical port, set stat in unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_translate_stat_set32(bcm_unit, port, key_type,
                                             outer_vlan, inner_vlan, stat,
                                             val);
    }

    /* Virtual port, set stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv =  bcm_vlan_translate_stat_set32(bcm_unit, port, key_type,
                                                outer_vlan, inner_vlan, stat,
                                                val);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_stat_multi_get/get32
 * Purpose:
 *      Get 64-bit/32-bit counter values for multiple ingress VLAN translation
 *      statistic types.
 * Parameters:
 *      port       - Generic port, physical (lport) or virtual port
 *      key_type   - Key Type : bcmVlanTranslateKey*
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      nstat      - Number of elements in stat array
 *      stat_arr   - Collected statistics descriptors array
 *      value_arr  - (OUT) Collected counters values
 * Returns:
 *      BCM_E_xxx
 */
int
bcmx_vlan_translate_stat_multi_get(bcm_gport_t port, 
                                   bcm_vlan_translate_key_t key_type, 
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   int nstat, bcm_vlan_stat_t *stat_arr, 
                                   uint64 *value_arr)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint64      *tmp_val;
    int         i;

    BCMX_VLAN_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    /* Physical port, get stat from unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_translate_stat_multi_get(bcm_unit, port, key_type,
                                                 outer_vlan, inner_vlan,
                                                 nstat, stat_arr, value_arr);
    }

    /* Virtual port, gather stats from all units */
    tmp_val = sal_alloc(sizeof(uint64) * nstat, "bcmx vlan translate stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        COMPILER_64_ZERO(value_arr[i]);
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_stat_multi_get(bcm_unit, port, key_type,
                                                   outer_vlan, inner_vlan,
                                                   nstat, stat_arr, tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (i = 0; i < nstat; i++) {
                    COMPILER_64_ADD_64(value_arr[i], tmp_val[i]);
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_val);

    return rv;
}

int
bcmx_vlan_translate_stat_multi_get32(bcm_gport_t port, 
                                     bcm_vlan_translate_key_t key_type, 
                                     bcm_vlan_t outer_vlan,
                                     bcm_vlan_t inner_vlan,
                                     int nstat, bcm_vlan_stat_t *stat_arr, 
                                     uint32 *value_arr)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint32      *tmp_val;
    int         i;

    BCMX_VLAN_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    /* Physical port, get stat from unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_translate_stat_multi_get32(bcm_unit, port, key_type,
                                                   outer_vlan, inner_vlan,
                                                   nstat, stat_arr, value_arr);
    }

    /* Virtual port, gather stats from all units */
    tmp_val = sal_alloc(sizeof(uint32) * nstat, "bcmx vlan translate stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        value_arr[i] = 0;
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_stat_multi_get32(bcm_unit, port, key_type,
                                                     outer_vlan, inner_vlan,
                                                     nstat, stat_arr,
                                                     tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (i = 0; i < nstat; i++) {
                    value_arr[i] += tmp_val[i];
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_val);

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_stat_multi_set/set32
 * Purpose:
 *      Set 64-bit/32-bit counter value for multiple ingress VLAN translation
 *      statistic types.
 * Parameters:
 *      port       - Generic port, physical (lport) or virtual port
 *      key_type   - Key Type : bcmVlanTranslateKey*
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      nstat      - Number of elements in stat array
 *      stat_arr   - New statistics descriptors array
 *      value_arr  - New counters values
 * Returns:
 *      BCM_E_xxx
 */
int
bcmx_vlan_translate_stat_multi_set(bcm_gport_t port, 
                                   bcm_vlan_translate_key_t key_type, 
                                   bcm_vlan_t outer_vlan, 
                                   bcm_vlan_t inner_vlan, 
                                   int nstat, bcm_vlan_stat_t *stat_arr, 
                                   uint64 *value_arr)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_VLAN_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    /* Physical port, set stat in unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_translate_stat_multi_set(bcm_unit, port, key_type,
                                                 outer_vlan, inner_vlan,
                                                 nstat, stat_arr, value_arr);
    }

    /* Virtual port, set stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_stat_multi_set(bcm_unit, port, key_type,
                                                   outer_vlan, inner_vlan,
                                                   nstat, stat_arr, value_arr);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_stat_multi_set32(bcm_gport_t port, 
                                     bcm_vlan_translate_key_t key_type, 
                                     bcm_vlan_t outer_vlan, 
                                     bcm_vlan_t inner_vlan, 
                                     int nstat, bcm_vlan_stat_t *stat_arr, 
                                     uint32 *value_arr)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_VLAN_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    /* Physical port, set stat in unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_translate_stat_multi_set32(bcm_unit, port, key_type,
                                                   outer_vlan, inner_vlan,
                                                   nstat, stat_arr, value_arr);
    }

    /* Virtual port, set stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_stat_multi_set32(bcm_unit, port, key_type,
                                                     outer_vlan, inner_vlan,
                                                     nstat, stat_arr,
                                                     value_arr);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_egress_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated
 *      egress VLAN translation.
 * Parameters:
 *      port_class - Group ID of ingress port
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      enable     - Non-zero to enable counter collection, zero to disable
 * Returns:
 *      BCM_E_xxx
 */
int
bcmx_vlan_translate_egress_stat_enable_set(int port_class, 
                                           bcm_vlan_t outer_vlan, 
                                           bcm_vlan_t inner_vlan, 
                                           int enable)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  bcm_unit;
    int  i;

    BCMX_VLAN_INIT_CHECK;

    /* 'port_class' is a global ID */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_stat_enable_set(bcm_unit, port_class,
                                                           outer_vlan,
                                                           inner_vlan,
                                                           enable);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_egress_stat_get/get32
 * Purpose:
 *      Extract egress VLAN translation statistics from the chip.
 * Parameters:
 *      port_class - Group ID of ingress port
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      stat       - Type of the counter to retrieve
 *      val        - (OUT) Pointer to a counter value
 * Returns:
 *      BCM_E_xxx
 */
int
bcmx_vlan_translate_egress_stat_get(int port_class, 
                                    bcm_vlan_t outer_vlan, 
                                    bcm_vlan_t inner_vlan, 
                                    bcm_vlan_stat_t stat, uint64 *val)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     bcm_unit;
    uint64  tmp_val;
    int     i;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(val);

    /* 'port_class' is a global ID */
    COMPILER_64_ZERO(*val);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_stat_get(bcm_unit, port_class,
                                                    outer_vlan, inner_vlan,
                                                    stat, &tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                COMPILER_64_ADD_64(*val, tmp_val);
            } else {
                break;
            }
        }
    }

    return rv;
}

int
bcmx_vlan_translate_egress_stat_get32(int port_class, 
                                      bcm_vlan_t outer_vlan, 
                                      bcm_vlan_t inner_vlan, 
                                      bcm_vlan_stat_t stat, uint32 *val)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     bcm_unit;
    uint32  tmp_val;
    int     i;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(val);

    /* 'port_class' is a global ID */
    *val = 0;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_stat_get32(bcm_unit, port_class,
                                                      outer_vlan, inner_vlan,
                                                      stat, &tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                *val += tmp_val;
            } else {
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_egress_stat_set/set32
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified egress VLAN translation.
 * Parameters:
 *      port_class - Group ID of ingress port
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      stat       - Type of the counter to set
 *      val        - New counter value
 * Returns:
 *      BCM_E_xxx
 */
int
bcmx_vlan_translate_egress_stat_set(int port_class, 
                                    bcm_vlan_t outer_vlan, 
                                    bcm_vlan_t inner_vlan, 
                                    bcm_vlan_stat_t stat, uint64 val)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  bcm_unit;
    int  i;

    BCMX_VLAN_INIT_CHECK;

    /* 'port_class' is a global ID */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_stat_set(bcm_unit, port_class,
                                                    outer_vlan, inner_vlan,
                                                    stat, val);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_egress_stat_set32(int port_class, 
                                      bcm_vlan_t outer_vlan, 
                                      bcm_vlan_t inner_vlan, 
                                      bcm_vlan_stat_t stat, uint32 val)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  bcm_unit;
    int  i;

    BCMX_VLAN_INIT_CHECK;

    /* 'port_class' is a global ID */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv =  bcm_vlan_translate_egress_stat_set32(bcm_unit, port_class,
                                                       outer_vlan, inner_vlan,
                                                       stat, val);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_egress_stat_multi_get/get32
 * Purpose:
 *      Get 64-bit/32-bit counter value for multiple egress VLAN translation
 *      statistic types.
 * Parameters:
 *      port_class - Group ID of ingress port
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      nstat      - Number of elements in stat array
 *      stat_arr   - Collected statistics descriptors array
 *      value_arr  - (OUT) Collected counters values
 * Returns:
 *      BCM_E_xxx
 */
int
bcmx_vlan_translate_egress_stat_multi_get(int port_class, 
                                          bcm_vlan_t outer_vlan, 
                                          bcm_vlan_t inner_vlan, 
                                          int nstat, bcm_vlan_stat_t *stat_arr, 
                                          uint64 *value_arr)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     bcm_unit;
    uint64  *tmp_val;
    int     i;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    tmp_val = sal_alloc(sizeof(uint64) * nstat,
                        "bcmx vlan translate egress stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        COMPILER_64_ZERO(value_arr[i]);
    }

    /* 'port_class' is a global ID */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_stat_multi_get(bcm_unit, port_class,
                                                          outer_vlan,
                                                          inner_vlan,
                                                          nstat, stat_arr,
                                                          tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (i = 0; i < nstat; i++) {
                    COMPILER_64_ADD_64(value_arr[i], tmp_val[i]);
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_val);

    return rv;
}

int
bcmx_vlan_translate_egress_stat_multi_get32(int port_class, 
                                            bcm_vlan_t outer_vlan, 
                                            bcm_vlan_t inner_vlan, 
                                            int nstat, 
                                            bcm_vlan_stat_t *stat_arr, 
                                            uint32 *value_arr)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     bcm_unit;
    uint32  *tmp_val;
    int     i;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    tmp_val = sal_alloc(sizeof(uint32) * nstat,
                        "bcmx vlan translate egress stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        value_arr[i] = 0;
    }

    /* 'port_class' is a global ID */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_stat_multi_get32(bcm_unit,
                                                            port_class,
                                                            outer_vlan,
                                                            inner_vlan,
                                                            nstat, stat_arr,
                                                            tmp_val);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (i = 0; i < nstat; i++) {
                    value_arr[i] += tmp_val[i];
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_val);

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_translate_egress_stat_multi_set/set32
 * Purpose:
 *      Set 64-bit/32-bit counter value for multiple egress VLAN translation
 *      statistic types.
 * Parameters:
 *      port_class - Group ID of ingress port
 *      outer_vlan - Packet outer VLAN ID
 *      inner_vlan - Packet inner VLAN ID
 *      nstat      - Number of elements in stat array
 *      stat_arr   - New statistics descriptors array
 *      value_arr  - New counters values
 * Returns:
 *      BCM_E_xxx
 */
int
bcmx_vlan_translate_egress_stat_multi_set(int port_class, 
                                          bcm_vlan_t outer_vlan, 
                                          bcm_vlan_t inner_vlan, 
                                          int nstat, 
                                          bcm_vlan_stat_t *stat_arr, 
                                          uint64 *value_arr)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  bcm_unit;
    int  i;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    /* 'port_class' is a global ID */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_stat_multi_set(bcm_unit, port_class,
                                                          outer_vlan,
                                                          inner_vlan,
                                                          nstat, stat_arr,
                                                          value_arr);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_vlan_translate_egress_stat_multi_set32(int port_class, 
                                            bcm_vlan_t outer_vlan, 
                                            bcm_vlan_t inner_vlan, 
                                            int nstat, 
                                            bcm_vlan_stat_t *stat_arr, 
                                            uint32 *value_arr)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  bcm_unit;
    int  i;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    /* 'port_class' is a global ID */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_stat_multi_set32(bcm_unit,
                                                            port_class,
                                                            outer_vlan,
                                                            inner_vlan,
                                                            nstat, stat_arr,
                                                            value_arr);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_vlan_queue_map_create
 * Purpose:
 *      Create a VLAN queue map entry.
 * Parameters:
 *      flags - Control flags
 *      qmid  - (IN/OUT) Queue map identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_create(uint32 flags, int *qmid)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(qmid);

    BCMX_UNIT_ITER(bcm_unit, i) {

        tmp_rv = bcm_vlan_queue_map_create(bcm_unit, flags, qmid);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));

        /*
         * Use the ID from first successful 'create' if group ID
         * is not specified.
         */
        if (!(flags & BCM_VLAN_QUEUE_MAP_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                flags |= BCM_VLAN_QUEUE_MAP_WITH_ID;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_destroy
 * Purpose:
 *      Delete a VLAN queue map entry.
 * Parameters:
 *      qmid  - Queue map identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_destroy(int qmid)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_destroy(bcm_unit, qmid);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_destroy_all
 * Purpose:
 *      Delete all VLAN queue map entries.
 * Parameters:
 *      None
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_destroy_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_destroy_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_set
 * Purpose:
 *      Set packet queue and color for a VLAN queue map entry.
 * Parameters:
 *      qmid    - Queue map identifier
 *      pkt_pri - Packet priority
 *      cfi     - Packet CFI value
 *      queue   - Assigned COS queue number
 *      color   - Assigned color value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_set(int qmid, int pkt_pri, int cfi,
                        int queue, int color)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_set(bcm_unit, qmid, pkt_pri, cfi,
                                        queue, color);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_get
 * Purpose:
 *      Get packet queue and color for a VLAN queue map entry.
 * Parameters:
 *      qmid    - Queue map identifier
 *      pkt_pri - Packet priority
 *      cfi     - Packet CFI value
 *      queue   - (OUT) Assigned COS queue number
 *      color   - (OUT) Assigned color value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_get(int qmid, int pkt_pri, int cfi, 
                        int *queue, int *color)
{
    int  rv;
    int  i;
    int  bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(queue);
    BCMX_PARAM_NULL_CHECK(color);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_queue_map_get(bcm_unit, qmid, pkt_pri, cfi,
                                    queue, color);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_attach
 * Purpose:
 *      Attach a queue map object to a VLAN or VFI.
 * Parameters:
 *      vlan  - VLAN identifier
 *      qmid  - Queue map identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_attach(bcm_vlan_t vlan, int qmid)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_attach(bcm_unit, vlan, qmid);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_attach_get
 * Purpose:
 *      Get attached queue map object for given VLAN or VFI.
 * Parameters:
 *      vlan  - VLAN identifier
 *      qmid  - (OUT) Queue map identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_attach_get(bcm_vlan_t vlan, int *qmid)
{
    int  rv;
    int  i;
    int  bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(qmid);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_queue_map_attach_get(bcm_unit, vlan, qmid);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_detach
 * Purpose:
 *      Dettached queue map object for given VLAN or VFI.
 * Parameters:
 *      vlan  - VLAN identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_detach(bcm_vlan_t vlan)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_detach(bcm_unit, vlan);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}
    
/*
 * Function:
 *      bcmx_vlan_queue_map_detach_all
 * Purpose:
 *      Dettached queue map objects from all VLAN or VFI.
 * Parameters:
 *      None
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_detach_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_detach_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_vlan_gport_add
 * Purpose:
 *      Add a gport (virtual or physical port) to the specified VLAN.
 * Parameters:
 *      vlan        - VLAN ID to add port to as a member
 *      port        - Gport ID
 *      flags       - BCM_VLAN_PORT_XXX
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_add(bcm_vlan_t vlan, bcm_gport_t port, int flags)
{
    int          rv = BCM_E_UNAVAIL, tmp_rv;
    int          i, bcm_unit;
    bcm_port_t   bcm_port;

    BCMX_VLAN_INIT_CHECK;

    /*
     * If gport is:
     *    - physical port, apply to unit where port resides
     *    - trunk, apply to first unit
     *    - otherwise, apply to all units
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_gport_add(bcm_unit, vlan, port, flags);
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        BCMX_UNIT_ITER(bcm_unit, i) {
            tmp_rv = bcm_vlan_gport_add(bcm_unit, vlan, port, flags);
            if (!BCMX_VLAN_ERROR_IS_VALID(bcm_unit, tmp_rv)) {
                continue;
            }

            (void) BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv);
            break;
        }

        return rv;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_gport_add(bcm_unit, vlan, port, flags);
        BCM_IF_ERROR_RETURN(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_gport_delete
 * Purpose:
 *      Delete a gport (virtual or physical port) from the specified VLAN.
 * Parameters:
 *      vlan - VLAN ID
 *      port - Gport ID
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_delete(bcm_vlan_t vlan, bcm_gport_t port)
{
    int          rv = BCM_E_UNAVAIL, tmp_rv;
    int          i, bcm_unit;
    bcm_port_t   bcm_port;

    BCMX_VLAN_INIT_CHECK;

    /*
     * If gport is:
     *    - physical port, apply to unit where port resides
     *    - trunk, apply to first unit (skip fabric with unavail error code
     *    - otherwise, apply to all units
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_gport_delete(bcm_unit, vlan, port);
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        BCMX_UNIT_ITER(bcm_unit, i) {
            tmp_rv = bcm_vlan_gport_delete(bcm_unit, vlan, port);
            if (!BCMX_VLAN_ERROR_IS_VALID(bcm_unit, tmp_rv)) {
                continue;
            }

            (void) BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv);
            break;
        }

        return rv;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_gport_delete(bcm_unit, vlan, port);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_gport_delete_all
 * Purpose:
 *      Delete all gports (virtual and physical port) from the specified VLAN.
 * Parameters:
 *      vlan - VLAN ID
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_delete_all(bcm_vlan_t vlan)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_gport_delete_all(bcm_unit, vlan);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_gport_get
 * Purpose:
 *      Get control flags for given gport (virtual or physical port)
 *      on the specified VLAN.
 * Parameters:
 *      vlan        - VLAN ID
 *      port        - Gport ID
 *      flags       - (OUT) Control flag BCM_VLAN_PORT_XXX
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_get(bcm_vlan_t vlan, bcm_gport_t port, int *flags)
{
    int          rv;
    int          i, bcm_unit;
    bcm_port_t   bcm_port;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    /*
     * If gport is:
     *    - physical port, get from unit where port resides
     *    - otherwise (including trunk), return from first successful get
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_gport_get(bcm_unit, vlan, port, flags);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_gport_get(bcm_unit, vlan, port, flags);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_vlan_gport_get_all
 * Purpose:
 *      Get all gports (virtual and physical port) for the specified VLAN.
 * Parameters:
 *      vlan        - VLAN ID
 *      array_max   - Maximum number of elements in arrays
 *      gport_array - (OUT) Array of gports
 *      flags_array - (OUT) Array of control flags
 *      array_size  - (OUT) Actual number of returned values in array
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_get_all(bcm_vlan_t vlan,
                        int array_max, bcm_gport_t *gport_array, 
                        int *flags_array, int *array_size)
{
    int          rv = BCM_E_UNAVAIL, tmp_rv;
    int          i, bcm_unit;
    int          count, total_count, array_left;
    bcm_gport_t  *gport_tmp;
    int          *flags_array_tmp;
    int          j, k;    

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(array_max, gport_array);
    BCMX_PARAM_ARRAY_NULL_CHECK(array_max, flags_array);
    BCMX_PARAM_NULL_CHECK(array_size);

    if ((gport_tmp = sal_alloc(sizeof(bcm_gport_t) * array_max,
                               "bcmx vlan gport get")) == NULL) {
        return BCM_E_MEMORY;
    }
    if ((flags_array_tmp = sal_alloc(sizeof(int) * array_max,
                                     "bcmx vlan gport flags get")) == NULL) {
        sal_free(gport_tmp);
        return BCM_E_MEMORY;
    }

    /*
     * Collect gports from all units
     */
    *array_size = 0;
    total_count = 0;
    array_left  = array_max;

    BCMX_UNIT_ITER(bcm_unit, i) {
        count = 0;
        tmp_rv = bcm_vlan_gport_get_all(bcm_unit, vlan, array_left,
                                        gport_tmp, flags_array_tmp, &count);
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, tmp_rv)) {
            /*
             * Gports can be in some units but not others (see 'add'),
             * so ignore BCM_E_NOT_FOUND errors.
             */
            if (tmp_rv == BCM_E_NOT_FOUND) {
                continue;
            }

            rv = tmp_rv;
            if (BCM_FAILURE(tmp_rv)) {
                break;
            }
                                            
            /* Add ports that are not yet in the list */
            for (j = 0; j < count; j++) {
                for (k = 0; k < total_count; k++) {
                    if (gport_tmp[j] == gport_array[k]) {
                        break;
                    }
                }
                if (k == total_count) {
                    gport_array[total_count] = gport_tmp[j];
                    flags_array[total_count] = flags_array_tmp[j];
                    array_left--;
                    total_count++;
                }
            }
        }
    }

    *array_size = total_count;

    sal_free(gport_tmp);
    sal_free(flags_array_tmp);

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_port_create
 * Purpose:
 *      Create a VLAN virtual port.
 * Parameters:
 *      vlan_port - (IN/OUT) Pointer to a VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcmx_vlan_port_create(bcmx_vlan_port_t *vlan_port)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    uint32  flags_orig;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_port);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = vlan_port->flags & BCM_VLAN_PORT_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_port_create(bcm_unit,
                                      BCMX_VLAN_PORT_T_PTR_TO_BCM(vlan_port));

        if (BCM_FAILURE(BCMX_VLAN_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * Use the ID from first successful 'create' if VLAN port ID
         * is not specified.
         */
        if (!(vlan_port->flags & BCM_VLAN_PORT_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                vlan_port->flags |= BCM_VLAN_PORT_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    vlan_port->flags &= ~BCM_VLAN_PORT_WITH_ID;
    vlan_port->flags |= flags_orig;

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_port_destroy
 * Purpose:
 *      Destroy a VLAN virtual port.
 * Parameters:
 *      gport - (IN) VLAN virtual port GPORT ID. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcmx_vlan_port_destroy(bcm_gport_t gport)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_port_destroy(bcm_unit, gport);
        BCM_IF_ERROR_RETURN
            (BCMX_VLAN_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_port_find
 * Purpose:
 *      Get VLAN virtual port info.
 * Parameters:
 *      vlan_port - (IN/OUT) Pointer to VLAN virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcmx_vlan_port_find(bcmx_vlan_port_t *vlan_port)
{
    int rv;
    int i, bcm_unit;

    BCMX_VLAN_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_port);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_port_find(bcm_unit,
                                BCMX_VLAN_PORT_T_PTR_TO_BCM(vlan_port));
        if (BCMX_VLAN_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}
