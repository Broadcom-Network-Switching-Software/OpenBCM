/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    bcmx/l3.c
 * Purpose: BCMX APIs
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/l3.h>
#include <bcmx/tunnel.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

#ifdef  INCLUDE_L3

#define BCMX_TUNNEL_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_TUNNEL_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_TUNNEL_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_TUNNEL_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


STATIC INLINE void
_bcmx_tunnel_to_bcm(bcmx_tunnel_terminator_t *l3xtun,
                    bcm_tunnel_terminator_t *l3tun)
{
    bcm_tunnel_terminator_t_init(l3tun);
    l3tun->flags = l3xtun->flags;
    l3tun->vrf = l3xtun->vrf;
    l3tun->sip = l3xtun->sip;
    l3tun->dip = l3xtun->dip;
    l3tun->sip_mask = l3xtun->sip_mask;
    l3tun->dip_mask = l3xtun->dip_mask;
    sal_memcpy(l3tun->sip6, l3xtun->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(l3tun->dip6, l3xtun->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(l3tun->sip6_mask, l3xtun->sip6_mask, sizeof(bcm_ip6_t));
    sal_memcpy(l3tun->dip6_mask, l3xtun->dip6_mask, sizeof(bcm_ip6_t));
    l3tun->udp_dst_port = l3xtun->udp_dst_port;
    l3tun->udp_src_port = l3xtun->udp_src_port;
    l3tun->type = l3xtun->type;
    l3tun->vlan = l3xtun->vlan;
    l3tun->remote_port = l3xtun->remote_port;
    l3tun->tunnel_id = l3xtun->tunnel_id;
    l3tun->if_class = l3xtun->if_class;
}

STATIC INLINE void
_bcmx_tunnel_from_bcm(bcm_tunnel_terminator_t *l3tun,
                      bcmx_tunnel_terminator_t *l3xtun)
{
    bcmx_tunnel_terminator_t_init(l3xtun);
    l3xtun->flags = l3tun->flags;
    l3xtun->vrf = l3tun->vrf;
    l3xtun->sip = l3tun->sip;
    l3xtun->dip = l3tun->dip;
    l3xtun->sip_mask = l3tun->sip_mask;
    l3xtun->dip_mask = l3tun->dip_mask;
    sal_memcpy(l3xtun->sip6, l3tun->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(l3xtun->dip6, l3tun->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(l3xtun->sip6_mask, l3tun->sip6_mask, sizeof(bcm_ip6_t));
    sal_memcpy(l3xtun->dip6_mask, l3tun->dip6_mask, sizeof(bcm_ip6_t));
    l3xtun->udp_dst_port = l3tun->udp_dst_port;
    l3xtun->udp_src_port = l3tun->udp_src_port;
    l3xtun->type = l3tun->type;
    l3xtun->vlan = l3tun->vlan;
    l3xtun->remote_port = l3tun->remote_port;
    l3xtun->tunnel_id = l3tun->tunnel_id;
    l3xtun->if_class = l3tun->if_class;
}

/*
 * Initialize a tunneling terminator structure
 *
 * Notes:
 *     Memory is allocated for the port list member in this
 *     call.  The corresponding 'free' API needs to be called
 *     when structure is no longer needed.
*/
void
bcmx_tunnel_terminator_t_init(bcmx_tunnel_terminator_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(bcmx_tunnel_terminator_t));
        info->vrf = BCM_L3_VRF_DEFAULT;
        bcmx_lplist_t_init(&info->ports);
    }
}

/*
 * Free memory allocated by the tunneling terminator initialization routine
 */
void
bcmx_tunnel_terminator_t_free(bcmx_tunnel_terminator_t *info)
{
    if (info != NULL) {
        bcmx_lplist_free(&info->ports);
    }
}

/*
 * Initialize a tunneling header (initiator) structure
 */
void
bcmx_tunnel_initiator_t_init(bcmx_tunnel_initiator_t *tunnel)
{
    if (tunnel != NULL) {
        sal_memset(tunnel, 0, sizeof(bcmx_tunnel_initiator_t));
    }
}

/*
 * Initialize a tunnel configuration structure
 */
void
bcmx_tunnel_config_t_init(bcmx_tunnel_config_t *tconfig)
{
    if (tconfig != NULL) {
        sal_memset(tconfig, 0, sizeof(bcmx_tunnel_config_t));
    }
}

/**************************************************************
 * bcmx_tunnel_xxx
 */

/*
 * Function:
 *     bcmx_tunnel_terminator_add
 */

int
bcmx_tunnel_terminator_add(bcmx_tunnel_terminator_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_tunnel_terminator_t bcm_tunnel;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    _bcmx_tunnel_to_bcm(info, &bcm_tunnel);
    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(info->ports, bcm_unit, bcm_tunnel.pbmp);
        if(BCM_PBMP_NOT_NULL(bcm_tunnel.pbmp)) {
            tmp_rv = bcm_tunnel_terminator_add(bcm_unit, &bcm_tunnel);
            BCM_IF_ERROR_RETURN
                (BCMX_TUNNEL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }
    return rv;
}

/*
 * Function:
 *     bcmx_tunnel_terminator_delete
 */

int
bcmx_tunnel_terminator_delete(bcmx_tunnel_terminator_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_tunnel_terminator_t bcm_tunnel;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    _bcmx_tunnel_to_bcm(info, &bcm_tunnel);
    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(info->ports, bcm_unit, bcm_tunnel.pbmp);
        if(BCM_PBMP_NOT_NULL(bcm_tunnel.pbmp)) {
            tmp_rv = bcm_tunnel_terminator_delete(bcm_unit, &bcm_tunnel);
            BCM_IF_ERROR_RETURN
                (BCMX_TUNNEL_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_tunnel_terminator_update
 */

int
bcmx_tunnel_terminator_update(bcmx_tunnel_terminator_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_tunnel_terminator_t bcm_tunnel;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    _bcmx_tunnel_to_bcm(info, &bcm_tunnel);
    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(info->ports, bcm_unit, bcm_tunnel.pbmp);
        if(BCM_PBMP_NOT_NULL(bcm_tunnel.pbmp)) {
            tmp_rv = bcm_tunnel_terminator_update(bcm_unit, &bcm_tunnel);
            BCM_IF_ERROR_RETURN
                (BCMX_TUNNEL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_tunnel_terminator_get
 */

int
bcmx_tunnel_terminator_get(bcmx_tunnel_terminator_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_tunnel_terminator_t bcm_tunnel;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    _bcmx_tunnel_to_bcm(info, &bcm_tunnel);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_tunnel_terminator_get(bcm_unit, &bcm_tunnel);

        /* Ignore those units that don't have tunnel terminator ports */
        if (tmp_rv == BCM_E_NOT_FOUND) {
            if (rv == BCM_E_UNAVAIL) {
                rv = tmp_rv;
            }
            continue;
        }

        if (BCMX_TUNNEL_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                rv = BCMX_LPLIST_PBMP_ADD(&info->ports, bcm_unit,
                                          bcm_tunnel.pbmp);
            } else {
                break;
            }
        }
    }
    if (BCM_SUCCESS(rv)) {
        _bcmx_tunnel_from_bcm(&bcm_tunnel, info);
    }

    return rv;
}


/*
 * Function:
 *     bcmx_tunnel_initiator_set
 */

int
bcmx_tunnel_initiator_set(bcmx_l3_intf_t *intf,
                          bcmx_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(intf);
    BCMX_PARAM_NULL_CHECK(tunnel);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_tunnel_initiator_set(bcm_unit, intf, tunnel);
        BCM_IF_ERROR_RETURN(BCMX_TUNNEL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_tunnel_initiator_clear
 */

int
bcmx_tunnel_initiator_clear(bcmx_l3_intf_t *intf)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(intf);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_tunnel_initiator_clear(bcm_unit, intf);
        BCM_IF_ERROR_RETURN
            (BCMX_TUNNEL_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_tunnel_initiator_get
 */

int
bcmx_tunnel_initiator_get(bcmx_l3_intf_t *intf,
                             bcmx_tunnel_initiator_t *tunnel)
{
    int rv;
    int i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(intf);
    BCMX_PARAM_NULL_CHECK(tunnel);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_tunnel_initiator_get(bcm_unit, intf, tunnel);
        if (BCMX_TUNNEL_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function: 
 *      bcmx_tunnel_dscp_map_create
 * Purpose:
 *      Create a tunnel DSCP map instance.
 * Parameters:
 *      flags        - (IN)  BCM_L3_XXX
 *      dscp_map_id  - (OUT) Allocated DSCP map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_tunnel_dscp_map_create(uint32 flags, int *dscp_map_id)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(dscp_map_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_tunnel_dscp_map_create(bcm_unit, flags, dscp_map_id);
        BCM_IF_ERROR_RETURN(BCMX_TUNNEL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_tunnel_dscp_map_destroy
 * Purpose:
 *      Destroy an existing tunnel DSCP map instance.
 * Parameters:
 *      dscp_map_id - (IN) DSCP map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_tunnel_dscp_map_destroy(int dscp_map_id)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_tunnel_dscp_map_destroy(bcm_unit, dscp_map_id);
        BCM_IF_ERROR_RETURN
            (BCMX_TUNNEL_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_tunnel_dscp_map_set
 * Purpose:
 *      Set the mapping of { internal priority, color }
 *      to a DSCP value for outer tunnel headers
 *      in the specified DSCP map instance.
 * Parameters:
 *      dscp_map_id  - (IN) DSCP map ID
 *      dscp_map     - (IN) tunnel DSCP info
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_tunnel_dscp_map_set(int dscp_map_id,
                            bcm_tunnel_dscp_map_t *dscp_map)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(dscp_map);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_tunnel_dscp_map_set(bcm_unit, dscp_map_id, dscp_map);
        BCM_IF_ERROR_RETURN(BCMX_TUNNEL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_tunnel_dscp_map_get
 * Purpose:
 *      Get the tunnel DSCP map information for the specified
 *      DSCP map instance.
 * Parameters:
 *      dscp_map_id  - (IN)  DSCP map ID
 *      dscp_map     - (OUT) Returning DSCP map info
 * Returns:
 *      BCM_E_XXX
 */     
int
bcmx_tunnel_dscp_map_get(int dscp_map_id,
                            bcm_tunnel_dscp_map_t *dscp_map)
{
    int    rv;
    int    i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(dscp_map);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_tunnel_dscp_map_get(bcm_unit, dscp_map_id, dscp_map);
        if (BCMX_TUNNEL_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_tunnel_dscp_map_port_set
 * Purpose:
 *      Set the mapping of { internal priority, color }
 *      to a DSCP value for outer tunnel headers
 *      for a specific egress port.
 * Parameters:
 *      port         - (IN) Egress port number.
 *      dscp_map     - (IN) tunnel DSCP info
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_tunnel_dscp_map_port_set(bcmx_lport_t port, 
                              bcm_tunnel_dscp_map_t *dscp_map)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(dscp_map);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return  bcm_tunnel_dscp_map_set(bcm_unit, bcm_port,
                                    dscp_map);
}


/*
 * Function:
 *      bcmx_tunnel_dscp_map_port_get
 * Purpose:
 *      Get the tunnel DSCP map information for the specified
 *      egress port
 * Parameters:
 *      port         - (IN)  Egress port number
 *      dscp_map     - (OUT) Returning DSCP map info
 * Returns:
 *      BCM_E_XXX
 */     
int
bcmx_tunnel_dscp_map_port_get(bcmx_lport_t port,
                              bcm_tunnel_dscp_map_t *dscp_map)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(dscp_map);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return  bcm_tunnel_dscp_map_get(bcm_unit, bcm_port,
                                    dscp_map);
}


/*
 * Function:
 *     bcmx_tunnel_config_set
 */

int
bcmx_tunnel_config_set(bcmx_tunnel_config_t *tconfig)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(tconfig);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_tunnel_config_set(bcm_unit, tconfig);
        BCM_IF_ERROR_RETURN(BCMX_TUNNEL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_tunnel_config_get
 */

int
bcmx_tunnel_config_get(bcmx_tunnel_config_t *tconfig)
{
    int rv;
    int i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_tunnel_config_get(bcm_unit, tconfig);
        if (BCMX_TUNNEL_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_tunnel_terminator_vlan_set
 * Purpose:
 *     Set the allowed VLANs for a WLAN tunnel.
 * Parameters:
 *     tunnel   - Tunnel ID
 *     vlan_vec - VLAN vector
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_tunnel_terminator_vlan_set(bcm_gport_t tunnel,
                                bcm_vlan_vector_t vlan_vec)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_tunnel_terminator_vlan_set(bcm_unit, tunnel, vlan_vec);
        BCM_IF_ERROR_RETURN(BCMX_TUNNEL_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_tunnel_terminator_vlan_get
 * Purpose:
 *     Get the allowed VLANs for a WLAN tunnel.
 * Parameters:
 *     tunnel   - Tunnel ID
 *     vlan_vec - (OUT) VLAN vector
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_tunnel_terminator_vlan_get(bcm_gport_t tunnel, 
                                bcm_vlan_vector_t *vlan_vec)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_TUNNEL_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vlan_vec);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_tunnel_terminator_vlan_get(bcm_unit, tunnel, vlan_vec);
        if (BCMX_TUNNEL_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

#endif  /* INCLUDE_L3 */
