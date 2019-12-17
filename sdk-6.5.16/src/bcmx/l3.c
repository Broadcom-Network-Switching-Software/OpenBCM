/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    bcmx/l3.c
 * Purpose: BCMX L3 APIs
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/l3.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include <bcmx/switch.h>

#include "bcmx_int.h"

#ifdef  INCLUDE_L3

#define BCMX_L3_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_L3_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_L3_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_L3_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_L3_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_L3_SOURCE_BIND_T_PTR_TO_BCM(_info)    \
    ((bcm_l3_source_bind_t *)(_info))

#define BCMX_L3_INGRESS_T_PTR_TO_BCM(_ingress)    \
    ((bcm_l3_ingress_t *)(_ingress))

/*
 * Function:
 *     _bcmx_bcm_l3_dest
 * Purpose:
 *     Internal destination conversion routine to handle bcmx to/from bcm
 *     and check for egress mode to ignore invalid destinations (when indicated)
 * Parameters:
 *     dest_bcmx    - BCMX destination object to convert
 *     dest_bcm     - BCM destination object to convert
 *     flags        - Conversion flags
 *     to_bcm       - Indicates whether to convert destination
 *                    bcmx 'to' or 'from' bcm.
 *     egress_check - Indicates whether to ignore invalid destination
 *                    conversion when in L3 egress mode
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_xxx  - Failure
 */
STATIC INLINE int
_bcmx_bcm_l3_dest(_bcmx_dest_bcmx_t *dest_bcmx, _bcmx_dest_bcm_t *dest_bcm,
                  uint32 *flags, int to_bcm, int egress_check)
{
    int  rv;

    /* Convert */
    if (to_bcm) {
        rv = _bcmx_dest_to_bcm(dest_bcmx, dest_bcm, flags);
    } else {
        rv = _bcmx_dest_from_bcm(dest_bcmx, dest_bcm, flags);
    }

    /* Ignore invalid destination conversion when in L3 egress mode */
    if (BCM_FAILURE(rv) && egress_check) {
        int l3_egress;

        if (BCM_SUCCESS(bcmx_switch_control_get(bcmSwitchL3EgressMode,
                                                &l3_egress))) {
            if (l3_egress) {
                rv = BCM_E_NONE;
            }
        }
    }

    return rv;
}

STATIC INLINE int
_bcmx_l3ip_to_bcm(bcmx_l3_host_t *l3xip, bcm_l3_host_t *l3ip, int convert_port)
{
    int                rv = BCM_E_NONE;
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   to_bcm;
    _bcmx_dest_bcmx_t  from_bcmx;
    
    bcm_l3_host_t_init(l3ip);
    l3ip->l3a_flags = l3xip->l3a_flags;

    if (convert_port) {
        /*
         * Convert destination data
         */
        _bcmx_dest_bcmx_t_init(&from_bcmx);
    
        /* Set flags and data to convert */
        if (l3xip->l3a_flags & BCM_L3_TGID) {
            flags |= BCMX_DEST_TRUNK;
        }
        from_bcmx.port  = l3xip->l3a_lport;
        from_bcmx.trunk = l3xip->l3a_trunk;

        /* Convert */
        /* Indicate to check egress mode in case of invalid destination */
        rv = _bcmx_bcm_l3_dest(&from_bcmx, &to_bcm, &flags, 1, 1);

        /* Set converted flags and data */
        if (flags & BCMX_DEST_TRUNK) {
            l3ip->l3a_flags |= BCM_L3_TGID;
            l3ip->l3a_port_tgid = to_bcm.trunk;
        } else if (flags & BCMX_DEST_DISCARD) {
            l3ip->l3a_flags |= BCM_L3_DST_DISCARD;
        } else {
            l3ip->l3a_modid     = to_bcm.module_id;
            l3ip->l3a_port_tgid = to_bcm.module_port;
        }
    }


    /* Set remaining fields */
    l3ip->l3a_ip_addr = l3xip->l3a_ip_addr;
    l3ip->l3a_intf = l3xip->l3a_intf;
    l3ip->l3a_vrf = l3xip->l3a_vrf;
    sal_memcpy(l3ip->l3a_ip6_addr, l3xip->l3a_ip6_addr,
               sizeof(bcm_ip6_t));
    sal_memcpy(l3ip->l3a_nexthop_mac, l3xip->l3a_nexthop_mac,
               sizeof(bcm_mac_t));
    l3ip->l3a_ipmc_ptr = l3xip->l3a_ipmc_ptr;
    l3ip->l3a_pri = l3xip->l3a_pri;
    l3ip->l3a_lookup_class = l3xip->l3a_lookup_class;

    return rv;
}

STATIC INLINE int
_bcmx_l3ip_from_bcm(bcmx_l3_host_t *l3xip, bcm_l3_host_t *l3ip)
{
    int                rv = BCM_E_NONE;
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   from_bcm;
    _bcmx_dest_bcmx_t  to_bcmx;


    bcmx_l3_host_t_init(l3xip);
    l3xip->l3a_flags = l3ip->l3a_flags;

    /*
     * Convert destination data
     */
    _bcmx_dest_bcm_t_init(&from_bcm);

    /* Set flags and data to convert */
    if (l3ip->l3a_flags & BCM_L3_TGID) {
        flags |= BCMX_DEST_TRUNK;
    }
    from_bcm.module_id   = l3ip->l3a_modid;
    from_bcm.module_port = l3ip->l3a_port_tgid;
    from_bcm.trunk       = l3ip->l3a_port_tgid;

    /* Convert */
    /* Indicate to check egress mode in case of invalid destination */
    rv = _bcmx_bcm_l3_dest(&to_bcmx, &from_bcm, &flags, 0, 1);

    /* Set converted flags and data */
    if (flags & BCMX_DEST_TRUNK) {
        l3xip->l3a_flags |= BCM_L3_TGID;
    } else if (flags & BCMX_DEST_DISCARD) {
        l3xip->l3a_flags |= BCM_L3_DST_DISCARD;
    }
    l3xip->l3a_lport = to_bcmx.port;
    l3xip->l3a_trunk = to_bcmx.trunk;


    /* Set remaining fields */
    l3xip->l3a_ip_addr = l3ip->l3a_ip_addr;
    l3xip->l3a_intf = l3ip->l3a_intf;
    l3xip->l3a_vrf = l3ip->l3a_vrf;
    sal_memcpy(l3xip->l3a_ip6_addr, l3ip->l3a_ip6_addr,
               sizeof(bcm_ip6_t));
    sal_memcpy(l3xip->l3a_nexthop_mac, l3ip->l3a_nexthop_mac,
               sizeof(bcm_mac_t));
    l3xip->l3a_ipmc_ptr = l3ip->l3a_ipmc_ptr;
    l3xip->l3a_pri = l3ip->l3a_pri;
    l3xip->l3a_lookup_class = l3ip->l3a_lookup_class;

    return rv;
}

STATIC INLINE int
_bcmx_l3egress_to_bcm(bcmx_l3_egress_t *l3xegr, bcm_l3_egress_t *l3egr)
{
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   to_bcm;
    _bcmx_dest_bcmx_t  from_bcmx;

    bcm_l3_egress_t_init(l3egr);
    l3egr->flags = l3xegr->flags;

    /*
     * Convert destination data
     */
    _bcmx_dest_bcmx_t_init(&from_bcmx);
    
    /* Set flags and data to convert */
    if (l3xegr->flags & BCM_L3_TGID) {
        flags |= BCMX_DEST_TRUNK;
    }
    from_bcmx.port  = l3xegr->lport;
    from_bcmx.trunk = l3xegr->trunk;

    /* Convert */
    /* Return on invalid destination conversion since egress object
     * needs to always have a valid destination.
     */
    BCM_IF_ERROR_RETURN(_bcmx_bcm_l3_dest(&from_bcmx, &to_bcm, &flags, 1, 0));

    /* Set converted flags and data */
    if (flags & BCMX_DEST_TRUNK) {
        l3egr->flags |= BCM_L3_TGID;
    } else if (flags & BCMX_DEST_DISCARD) {
        l3egr->flags |= BCM_L3_DST_DISCARD;
    }
    l3egr->module = to_bcm.module_id;
    l3egr->port   = to_bcm.module_port;
    l3egr->trunk  = to_bcm.trunk;


    /* Set remaining fields */
    l3egr->intf  = l3xegr->intf; 
    sal_memcpy(l3egr->mac_addr, l3xegr->mac_addr, sizeof(bcm_mac_t));
    l3egr->vlan = l3xegr->vlan; 
    l3egr->mpls_flags = l3xegr->mpls_flags;
    l3egr->mpls_label = l3xegr->mpls_label;
    l3egr->mpls_qos_map_id = l3xegr->mpls_qos_map_id;
    l3egr->mpls_ttl = l3xegr->mpls_ttl;
    l3egr->mpls_pkt_pri = l3xegr->mpls_pkt_pri;
    l3egr->mpls_pkt_cfi = l3xegr->mpls_pkt_cfi;
    l3egr->mpls_exp = l3xegr->mpls_exp;
    l3egr->encap_id = l3xegr->encap_id;
    l3egr->failover_id = l3xegr->failover_id;
    l3egr->failover_if_id = l3xegr->failover_if_id;

    return BCM_E_NONE;
}

STATIC INLINE int
_bcmx_l3egress_from_bcm(bcmx_l3_egress_t *l3xegr, bcm_l3_egress_t *l3egr)
{
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   from_bcm;
    _bcmx_dest_bcmx_t  to_bcmx;

    bcmx_l3_egress_t_init(l3xegr);
    l3xegr->flags = l3egr->flags;

    /*
     * Convert destination data
     */
    _bcmx_dest_bcm_t_init(&from_bcm);

    /* Set flags and data to convert */
    if (l3egr->flags & BCM_L3_TGID) {
        flags |= BCMX_DEST_TRUNK;
    }
    from_bcm.module_id   = l3egr->module;
    from_bcm.module_port = l3egr->port;
    from_bcm.trunk       = l3egr->trunk;

    /* Convert */
    /* Return on invalid destination conversion since egress object
     * needs to always have a valid destination.
     */
    BCM_IF_ERROR_RETURN(_bcmx_bcm_l3_dest(&to_bcmx, &from_bcm, &flags, 0, 0));

    /* Set converted flags and data */
    if (flags & BCMX_DEST_TRUNK) {
        l3xegr->flags |= BCM_L3_TGID;
    } else if (flags & BCMX_DEST_DISCARD) {
        l3xegr->flags |= BCM_L3_DST_DISCARD;
    }
    l3xegr->lport = to_bcmx.port;
    l3xegr->trunk = to_bcmx.trunk;


    /* Set remaining fields */
    l3xegr->intf  = l3egr->intf;
    sal_memcpy(l3xegr->mac_addr, l3egr->mac_addr, sizeof(bcm_mac_t));
    l3xegr->vlan = l3egr->vlan;
    l3xegr->mpls_flags = l3egr->mpls_flags;
    l3xegr->mpls_label = l3egr->mpls_label;
    l3xegr->mpls_qos_map_id = l3egr->mpls_qos_map_id;
    l3xegr->mpls_ttl = l3egr->mpls_ttl;
    l3xegr->mpls_pkt_pri = l3egr->mpls_pkt_pri;
    l3xegr->mpls_pkt_cfi = l3egr->mpls_pkt_cfi;
    l3xegr->mpls_exp = l3egr->mpls_exp;
    l3xegr->encap_id = l3egr->encap_id;
    l3xegr->failover_id = l3egr->failover_id;
    l3xegr->failover_if_id = l3egr->failover_if_id;

    return BCM_E_NONE;
}


STATIC INLINE int
_bcmx_l3route_to_bcm(bcmx_l3_route_t *l3xrt, bcm_l3_route_t *l3rt,
                     int convert_port)
{
    int                rv = BCM_E_NONE;
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   to_bcm;
    _bcmx_dest_bcmx_t  from_bcmx;

    bcm_l3_route_t_init(l3rt);
    l3rt->l3a_flags = l3xrt->l3a_flags;

    if (convert_port) {
        /*
         * Convert destination data
         */
        _bcmx_dest_bcmx_t_init(&from_bcmx);
    
        /* Set flags and data to convert */
        if (l3xrt->l3a_flags & BCM_L3_TGID) {
            flags |= BCMX_DEST_TRUNK;
        }
        from_bcmx.port  = l3xrt->l3a_lport;
        from_bcmx.trunk = l3xrt->l3a_trunk;

        /* Convert */
        /* Indicate to check egress mode in case of invalid destination */
        rv = _bcmx_bcm_l3_dest(&from_bcmx, &to_bcm, &flags, 1, 1);

        /* Set converted flags and data */
        if (flags & BCMX_DEST_TRUNK) {
            l3rt->l3a_flags |= BCM_L3_TGID;
            l3rt->l3a_port_tgid = to_bcm.trunk;
        } else if (flags & BCMX_DEST_DISCARD) {
            l3rt->l3a_flags |= BCM_L3_DST_DISCARD;
        } else {
            l3rt->l3a_modid     = to_bcm.module_id;
            l3rt->l3a_port_tgid = to_bcm.module_port;
        }
    }

    /* Set remaining fields */
    l3rt->l3a_stack_port = 0;
    l3rt->l3a_subnet = l3xrt->l3a_subnet;
    l3rt->l3a_ip_mask = l3xrt->l3a_ip_mask;
    l3rt->l3a_intf = l3xrt->l3a_intf;
    l3rt->l3a_vrf = l3xrt->l3a_vrf;
    sal_memcpy(l3rt->l3a_ip6_net, l3xrt->l3a_ip6_net,
               sizeof(bcm_ip6_t));
    sal_memcpy(l3rt->l3a_ip6_mask, l3xrt->l3a_ip6_mask,
               sizeof(bcm_ip6_t));
    sal_memcpy(l3rt->l3a_nexthop_mac, l3xrt->l3a_nexthop_mac,
               sizeof(bcm_mac_t));
    l3rt->l3a_nexthop_ip = l3xrt->l3a_nexthop_ip;
    l3rt->l3a_vid = l3xrt->l3a_vid;
    l3rt->l3a_pri = l3xrt->l3a_pri;
    l3rt->l3a_tunnel_option = l3xrt->l3a_tunnel_option;
    l3rt->l3a_mpls_label = l3xrt->l3a_mpls_label;
    l3rt->l3a_lookup_class = l3xrt->l3a_lookup_class;

    return rv;
}

STATIC INLINE int
_bcmx_l3route_from_bcm(bcmx_l3_route_t *l3xrt, bcm_l3_route_t *l3rt)
{
    int                rv = BCM_E_NONE;
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   from_bcm;
    _bcmx_dest_bcmx_t  to_bcmx;

    bcmx_l3_route_t_init(l3xrt);
    l3xrt->l3a_flags = l3rt->l3a_flags;

    /*
     * Convert destination data
     */
    _bcmx_dest_bcm_t_init(&from_bcm);

    /* Set flags and data to convert */
    if (l3rt->l3a_flags & BCM_L3_TGID) {
        flags |= BCMX_DEST_TRUNK;
    }
    from_bcm.module_id   = l3rt->l3a_modid;
    from_bcm.module_port = l3rt->l3a_port_tgid;
    from_bcm.trunk       = l3rt->l3a_port_tgid;

    /* Convert */
    /* Indicate to check egress mode in case of invalid destination */
    rv = _bcmx_bcm_l3_dest(&to_bcmx, &from_bcm, &flags, 0, 1);

    /* Set converted flags and data */
    if (flags & BCMX_DEST_TRUNK) {
        l3xrt->l3a_flags |= BCM_L3_TGID;
    } else if (flags & BCMX_DEST_DISCARD) {
        l3xrt->l3a_flags |= BCM_L3_DST_DISCARD;
    }
    l3xrt->l3a_lport = to_bcmx.port;
    l3xrt->l3a_trunk = to_bcmx.trunk;


    /* Set remaining fields */
    l3xrt->l3a_subnet = l3rt->l3a_subnet;
    l3xrt->l3a_ip_mask = l3rt->l3a_ip_mask;
    l3xrt->l3a_intf = l3rt->l3a_intf;
    l3xrt->l3a_vrf = l3rt->l3a_vrf;
    sal_memcpy(l3xrt->l3a_ip6_net, l3rt->l3a_ip6_net,
               sizeof(bcm_ip6_t));
    sal_memcpy(l3xrt->l3a_ip6_mask, l3rt->l3a_ip6_mask,
               sizeof(bcm_ip6_t));
    sal_memcpy(l3xrt->l3a_nexthop_mac, l3rt->l3a_nexthop_mac,
               sizeof(bcm_mac_t));
    l3xrt->l3a_nexthop_ip = l3rt->l3a_nexthop_ip;
    l3xrt->l3a_vid = l3rt->l3a_vid;
    l3xrt->l3a_pri = l3rt->l3a_pri;
    l3xrt->l3a_tunnel_option = l3rt->l3a_tunnel_option;
    l3xrt->l3a_mpls_label = l3rt->l3a_mpls_label;
    l3xrt->l3a_lookup_class = l3rt->l3a_lookup_class;

    return rv;
}


/*
 * Initialize a L3 IP struct
 */
int
bcmx_l3_host_t_init(bcmx_l3_host_t *ip)
{
    if (ip != NULL) {
        sal_memset(ip, 0, sizeof(bcmx_l3_host_t));
        ip->l3a_vrf = BCM_L3_VRF_DEFAULT;
    }

    return BCM_E_NONE;
}

/*
 * Initialize a L3 route struct
 */
int
bcmx_l3_route_t_init(bcmx_l3_route_t *route)
{
    if (route != NULL) {
        sal_memset(route, 0, sizeof(bcmx_l3_route_t));
        route->l3a_vrf = BCM_L3_VRF_DEFAULT;
    }

    return BCM_E_NONE;
}

/*
 * Initialize a L3 interface struct
 */
int
bcmx_l3_intf_t_init(bcmx_l3_intf_t *intf)
{
    if (intf != NULL) {
        sal_memset(intf, 0, sizeof(bcmx_l3_intf_t));
        intf->l3a_vrf = BCM_L3_VRF_DEFAULT;
    }

    return BCM_E_NONE;
}

/*
 * Initialize a L3 egress object struct
 */
void
bcmx_l3_egress_t_init(bcmx_l3_egress_t *egr)
{
    if (egr != NULL) {
        sal_memset(egr, 0, sizeof(bcmx_l3_egress_t));
        egr->mpls_label = BCM_MPLS_LABEL_INVALID;
    }
}

/*
 * Initialize a L3 source bind object struct
 */
void bcmx_l3_source_bind_t_init(bcmx_l3_source_bind_t *info)
{
    if (info != NULL) {
        bcm_l3_source_bind_t_init
            (BCMX_L3_SOURCE_BIND_T_PTR_TO_BCM(info));
    }
}

/*
 * Initialize a L3 ingress object struct
 */
void
bcmx_l3_ingress_t_init(bcmx_l3_ingress_t *ing_intf)
{
    if (ing_intf != NULL) {
        bcm_l3_ingress_t_init(BCMX_L3_INGRESS_T_PTR_TO_BCM(ing_intf));
    }
}


/*
 * Function:
 *      bcmx_l3_init
 */

int
bcmx_l3_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_L3_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l3_cleanup
 */

int
bcmx_l3_cleanup(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_cleanup(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_L3_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l3_enable_set
 */

int
bcmx_l3_enable_set(int enable)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_enable_set(bcm_unit, enable);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l3_info
 */

int
bcmx_l3_info(bcm_l3_info_t *l3info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_info_t tmp_l3info;
    int first = TRUE;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(l3info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_info(bcm_unit, &tmp_l3info);

        if (BCMX_L3_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;

            BCM_IF_ERROR_RETURN(rv);

            if (first) {
                *l3info = tmp_l3info;
                first = FALSE;
                continue;
            }

            /* Set the minimum for 'max' members */
            if (tmp_l3info.l3info_max_vrf < l3info->l3info_max_vrf) {
                l3info->l3info_max_vrf = tmp_l3info.l3info_max_vrf;
            }
            if (tmp_l3info.l3info_max_intf < l3info->l3info_max_intf) {
                l3info->l3info_max_intf = tmp_l3info.l3info_max_intf;
            }
            if (tmp_l3info.l3info_max_intf_group < l3info->l3info_max_intf_group) {
                l3info->l3info_max_intf_group = tmp_l3info.l3info_max_intf_group;
            }
            if (tmp_l3info.l3info_max_host < l3info->l3info_max_host) {
                l3info->l3info_max_host = tmp_l3info.l3info_max_host;
            }
            if (tmp_l3info.l3info_max_route < l3info->l3info_max_route) {
                l3info->l3info_max_route = tmp_l3info.l3info_max_route;
            }
            if (tmp_l3info.l3info_max_ecmp < l3info->l3info_max_ecmp) {
                l3info->l3info_max_ecmp = tmp_l3info.l3info_max_ecmp;
            }
            
            if (tmp_l3info.l3info_max_lpm_block < l3info->l3info_max_lpm_block) {
                l3info->l3info_max_lpm_block = tmp_l3info.l3info_max_lpm_block;
            }
            if (tmp_l3info.l3info_max_l3 < l3info->l3info_max_l3) {
                l3info->l3info_max_l3 = tmp_l3info.l3info_max_l3;
            }
            if (tmp_l3info.l3info_max_defip < l3info->l3info_max_defip) {
                l3info->l3info_max_defip = tmp_l3info.l3info_max_defip;
            }

            if (tmp_l3info.l3info_max_nexthop < l3info->l3info_max_nexthop) {
                l3info->l3info_max_nexthop = tmp_l3info.l3info_max_nexthop;
            }
            if (tmp_l3info.l3info_max_tunnel_init < l3info->l3info_max_tunnel_init) {
                l3info->l3info_max_tunnel_init = tmp_l3info.l3info_max_tunnel_init;
            }
            if (tmp_l3info.l3info_max_tunnel_term < l3info->l3info_max_tunnel_term) {
                l3info->l3info_max_tunnel_term = tmp_l3info.l3info_max_tunnel_term;
            }
        }
    }

    return rv;
}


/**************************************************************
 * bcmx_l3_intf_xxx
 */

/*
 * Function:
 *     bcmx_l3_intf_create
 */

int
bcmx_l3_intf_create(bcmx_l3_intf_t *intf)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(intf);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_intf_create(bcm_unit, intf);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_intf_delete
 */

int
bcmx_l3_intf_delete(bcmx_l3_intf_t *intf)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(intf);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_intf_delete(bcm_unit, intf);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_intf_delete_all
 */

int
bcmx_l3_intf_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_intf_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_intf_find
 */

int
bcmx_l3_intf_find(bcmx_l3_intf_t *intf)
{
    int rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(intf);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_intf_find(bcm_unit, intf);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_l3_intf_find_vlan
 */

int
bcmx_l3_intf_find_vlan(bcm_l3_intf_t *intf)
{
    int rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(intf);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_intf_find_vlan(bcm_unit, intf);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_l3_intf_get
 */

int
bcmx_l3_intf_get(bcm_l3_intf_t *intf)
{
    int rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(intf);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_intf_get(bcm_unit, intf);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/**************************************************************
 * bcmx_l3_egress_xxx
 */
int 
bcmx_l3_egress_create(uint32 flags, bcmx_l3_egress_t *egr, bcm_if_t *intf) 
{
    int rv = BCM_E_UNAVAIL;    /* Overall operation result.           */
    int tmp_rv;                /* Unit operation result.              */
    int unit_cntr;             /* Number of devices counter.          */
    int bcm_unit;              /* bcm device iterator.                */
    bcm_l3_egress_t bcm_egr;   /* Egress object.                      */
    int map_local_cpu;         /* Is local CPU port mapping required? */
    bcmx_lport_t cpu_lport;    /* Cpu logical port.                   */

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(egr);
    BCMX_PARAM_NULL_CHECK(intf);

    map_local_cpu = ((!(egr->flags & BCM_L3_TGID)) &&
                     (egr->lport == BCMX_LPORT_LOCAL_CPU));

    BCM_IF_ERROR_RETURN(_bcmx_l3egress_to_bcm(egr, &bcm_egr));

    if ((!map_local_cpu) &&
        (!(egr->flags & BCM_L3_TGID) && (bcm_egr.module < 0)) &&
        (!BCM_GPORT_IS_SET(bcm_egr.port))) {
        return (BCM_E_PORT);
    }

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        if (map_local_cpu) {
            cpu_lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
            if (BCM_FAILURE
                (_bcmx_dest_to_modid_port(cpu_lport,
                                          &bcm_egr.module,
                                          &bcm_egr.port,
                                          BCMX_DEST_CONVERT_DEFAULT))) {  
                /* Skip if can't find local CPU port */
                continue;
            }
        }
        tmp_rv = bcm_l3_egress_create(bcm_unit, flags, &bcm_egr, intf);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int 
bcmx_l3_egress_destroy(bcm_if_t intf)
{
    int rv = BCM_E_UNAVAIL;    /* Overall operation result.      */
    int tmp_rv;                /* Unit operation result.         */
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        tmp_rv = bcm_l3_egress_destroy(bcm_unit, intf);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int 
bcmx_l3_egress_get(bcm_if_t intf, bcmx_l3_egress_t *egr)
{
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */
    bcm_l3_egress_t bcm_egr;   /* Egress object.                 */
    int rv;                    /* Overall operation result.      */

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(egr);

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        rv = bcm_l3_egress_get(bcm_unit, intf, &bcm_egr);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            if (BCM_SUCCESS(rv)) {
                rv = _bcmx_l3egress_from_bcm(egr, &bcm_egr);
            }
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int 
bcmx_l3_egress_find(bcmx_l3_egress_t *egr, bcm_if_t *intf)
{
    int unit_cntr;             /* Number of devices counter.          */
    int bcm_unit;              /* bcm device iterator.                */
    bcm_l3_egress_t bcm_egr;   /* Egress object.                      */
    int map_local_cpu;         /* Is local CPU port mapping required? */
    bcmx_lport_t cpu_lport;    /* Cpu logical port.                   */
    int rv;                    /* Overall operation result.           */

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(egr);
    BCMX_PARAM_NULL_CHECK(intf);

    map_local_cpu = ((!(egr->flags & BCM_L3_TGID)) &&
                     (egr->lport == BCMX_LPORT_LOCAL_CPU));

    BCM_IF_ERROR_RETURN(_bcmx_l3egress_to_bcm(egr, &bcm_egr));

    if ((!map_local_cpu) &&
        (!(egr->flags & BCM_L3_TGID) && (bcm_egr.module < 0))) {
        return (BCM_E_PORT);
    }

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        if (map_local_cpu) {
            cpu_lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
            if (BCM_FAILURE
                (_bcmx_dest_to_modid_port(cpu_lport,
                                          &bcm_egr.module,
                                          &bcm_egr.port,
                                          BCMX_DEST_CONVERT_DEFAULT))) {
                /* Skip if can't find local CPU port */
                continue;
            }
        }
        rv = bcm_l3_egress_find(bcm_unit, &bcm_egr, intf);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_l3_egress_traverse(bcm_l3_egress_traverse_cb trav_fn, void *user_data)
{
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */
    int rv;                    /* Overall operation result.      */

    BCMX_L3_INIT_CHECK;

    BCMX_LOCAL_UNIT_ITER(bcm_unit, unit_cntr) {
        rv = bcm_l3_egress_traverse(bcm_unit, trav_fn, user_data);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int 
bcmx_l3_egress_multipath_create(uint32 flags, int intf_count,
                                bcm_if_t *intf_array, bcm_if_t *mpintf)
{
    int rv = BCM_E_UNAVAIL;    /* Overall operation result.      */
    int tmp_rv;                /* Unit operation result.         */
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(intf_count, intf_array);
    BCMX_PARAM_NULL_CHECK(mpintf);

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        tmp_rv = bcm_l3_egress_multipath_create(bcm_unit, flags, intf_count,
                                                intf_array, mpintf);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_l3_egress_multipath_max_create(uint32 flags,
                                    int max_paths, int intf_count,
                                    bcm_if_t *intf_array, bcm_if_t *mpintf)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(intf_count, intf_array);
    BCMX_PARAM_NULL_CHECK(mpintf);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_egress_multipath_max_create(bcm_unit, flags,
                                                    max_paths, intf_count,
                                                    intf_array, mpintf);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int 
bcmx_l3_egress_multipath_destroy(bcm_if_t mpintf)
{
    int rv = BCM_E_UNAVAIL;    /* Overall operation result.      */
    int tmp_rv;                /* Unit operation result.         */
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        tmp_rv = bcm_l3_egress_multipath_destroy(bcm_unit, mpintf);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int 
bcmx_l3_egress_multipath_get(bcm_if_t mpintf, int intf_size,
                             bcm_if_t *intf_array, int *intf_count)
{
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */
    int rv;                    /* Overall operation result.      */

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(intf_size, intf_array);
    BCMX_PARAM_NULL_CHECK(intf_count);

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        rv = bcm_l3_egress_multipath_get(bcm_unit, mpintf, intf_size,
                                         intf_array, intf_count);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int 
bcmx_l3_egress_multipath_add(bcm_if_t mpintf, bcm_if_t intf)
{
    int rv = BCM_E_UNAVAIL;    /* Overall operation result.      */
    int tmp_rv;                /* Unit operation result.         */
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        tmp_rv = bcm_l3_egress_multipath_add(bcm_unit, mpintf, intf);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int 
bcmx_l3_egress_multipath_delete(bcm_if_t mpintf, bcm_if_t intf)
{
    int rv = BCM_E_UNAVAIL;    /* Overall operation result.      */
    int tmp_rv;                /* Unit operation result.         */
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        tmp_rv = bcm_l3_egress_multipath_delete(bcm_unit, mpintf, intf);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int 
bcmx_l3_egress_multipath_find(int intf_count, bcm_if_t *intf_array, 
                              bcm_if_t *mpintf)
{
    int unit_cntr;             /* Number of devices counter.          */
    int bcm_unit;              /* bcm device iterator.                */
    int rv;                    /* Overall operation result.      */

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(intf_count, intf_array);
    BCMX_PARAM_NULL_CHECK(mpintf);

    BCMX_UNIT_ITER(bcm_unit, unit_cntr) {
        rv = bcm_l3_egress_multipath_find(bcm_unit, intf_count,
                                          intf_array, mpintf);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_l3_egress_multipath_traverse(bcm_l3_egress_multipath_traverse_cb trav_fn,
                                  void *user_data)
{
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */
    int rv;                    /* Overall operation result.      */

    BCMX_L3_INIT_CHECK;

    BCMX_LOCAL_UNIT_ITER(bcm_unit, unit_cntr) {
        rv = bcm_l3_egress_multipath_traverse(bcm_unit, trav_fn, user_data);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/**************************************************************
 * bcmx_l3_ingress_xxx
 */
/*
 * Function:
 *     bcmx_l3_ingress_create
 * Purpose:
 *     Create an Ingress Interface object.
 * Parameters:
 *     ing_intf - Ingress Interface information.
 *     intf_id  - (IN/OUT) L3 Ingress interface id pointing to Ingress object.
 *                This is an IN argument if either BCM_L3_INGRESS_REPLACE
 *                or BCM_L3_INGRESS_WITH_ID are given in flags.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_l3_ingress_create(bcmx_l3_ingress_t *ing_intf, bcm_if_t *intf_id)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    bcmx_l3_ingress_t  tmp_ing_intf;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ing_intf);
    BCMX_PARAM_NULL_CHECK(intf_id);

    tmp_ing_intf = *ing_intf;
    BCMX_UNIT_ITER(bcm_unit, i) {

        tmp_rv = bcm_l3_ingress_create(bcm_unit,
                                       BCMX_L3_INGRESS_T_PTR_TO_BCM
                                       (&tmp_ing_intf),
                                       intf_id);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));

        /*
         * Use the ID from first successful 'create' if group ID
         * is not specified.
         */
        if (!(tmp_ing_intf.flags & BCM_L3_INGRESS_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                tmp_ing_intf.flags |= BCM_L3_INGRESS_WITH_ID;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_ingress_destroy
 * Purpose:
 *     Destroy an Ingress Interface object.
 * Parameters:
 *     intf_id  - L3 Ingress interface id pointing to Ingress object.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_l3_ingress_destroy(bcm_if_t intf_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_ingress_destroy(bcm_unit, intf_id);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_ingress_get
 * Purpose:
 *     Get an Ingress Interface object.
 * Parameters:
 *     intf_id  - L3 Ingress interface id pointing to Ingress object.
 *     ing_intf - (OUT) Ingress Interface object properties.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_l3_ingress_get(bcm_if_t intf, bcmx_l3_ingress_t *ing_intf)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ing_intf);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_ingress_get(bcm_unit, intf,
                                BCMX_L3_INGRESS_T_PTR_TO_BCM(ing_intf));
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_l3_ingress_find
 * Purpose:
 *     Find the Ingress interface id.
 * Parameters:
 *     ing_intf - Ingress Interface information.
 *     intf_id  - (OUT) L3 Ingress interface id pointing to Ingress object.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_l3_ingress_find(bcmx_l3_ingress_t *ing_intf, bcm_if_t *intf_id)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ing_intf);
    BCMX_PARAM_NULL_CHECK(intf_id);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_ingress_find(bcm_unit,
                                 BCMX_L3_INGRESS_T_PTR_TO_BCM(ing_intf),
                                 intf_id);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 *  IPv6 prefix map APIs.
 */
/**************************************************************
 * bcmx_l3_ip6_prefix_map APIs
 */
int bcmx_l3_ip6_prefix_map_get(int map_size, 
                               bcm_ip6_t *ip6_array, int *ip6_count) 
{
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */
    int rv;                    /* Overall operation result.      */

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(map_size, ip6_array);
    BCMX_PARAM_NULL_CHECK(ip6_count);

    BCMX_LOCAL_UNIT_ITER(bcm_unit, unit_cntr) {
        rv = bcm_l3_ip6_prefix_map_get(bcm_unit, map_size, 
                                       ip6_array, ip6_count);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int bcmx_l3_ip6_prefix_map_add(bcm_ip6_t ip6_addr) 
{
    int rv = BCM_E_UNAVAIL;    /* Overall operation result.      */
    int tmp_rv;                /* Unit operation result.         */
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */

    BCMX_L3_INIT_CHECK;

    BCMX_LOCAL_UNIT_ITER(bcm_unit, unit_cntr) {
        tmp_rv = bcm_l3_ip6_prefix_map_add(bcm_unit, ip6_addr);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_l3_ip6_prefix_map_delete(bcm_ip6_t ip6_addr) 
{
    int rv = BCM_E_UNAVAIL;    /* Overall operation result.      */
    int tmp_rv;                /* Unit operation result.         */
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */

    BCMX_L3_INIT_CHECK;

    BCMX_LOCAL_UNIT_ITER(bcm_unit, unit_cntr) {
        tmp_rv = bcm_l3_ip6_prefix_map_delete(bcm_unit, ip6_addr);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int bcmx_l3_ip6_prefix_map_delete_all(void) 
{
    int rv = BCM_E_UNAVAIL;    /* Overall operation result.      */
    int tmp_rv;                /* Unit operation result.         */
    int unit_cntr;             /* Number of devices counter.     */
    int bcm_unit;              /* bcm device iterator.           */

    BCMX_L3_INIT_CHECK;

    BCMX_LOCAL_UNIT_ITER(bcm_unit, unit_cntr) {
        tmp_rv = bcm_l3_ip6_prefix_map_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/**************************************************************
 * bcmx_l3_host_xxx
 */

/*
 * Function:
 *     bcmx_l3_host_find
 */

int
bcmx_l3_host_find(bcmx_l3_host_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_host_t bcm_l3;
    uint32 merged_flags = 0;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN(_bcmx_l3ip_to_bcm(info, &bcm_l3, FALSE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_host_find(bcm_unit, &bcm_l3);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(rv)) {
                merged_flags |= bcm_l3.l3a_flags;
            } else {
                break;
            }
        }
        /* bcm_l3_host_find() may overwrite input flags */
        bcm_l3.l3a_flags = info->l3a_flags;
    }
    
    if (BCM_SUCCESS(rv)) {
        bcm_l3.l3a_flags  |= merged_flags;
        BCM_IF_ERROR_RETURN(_bcmx_l3ip_from_bcm(info, &bcm_l3));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_host_add
 */

int
bcmx_l3_host_add(bcmx_l3_host_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_host_t bcm_l3;
    int map_local_cpu;  /* Is local CPU port mapping required? */
    bcmx_lport_t cpu_lport;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    map_local_cpu = ((!(info->l3a_flags & BCM_L3_TGID)) &&
                     (info->l3a_lport == BCMX_LPORT_LOCAL_CPU));
    BCM_IF_ERROR_RETURN(_bcmx_l3ip_to_bcm(info, &bcm_l3, TRUE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (map_local_cpu) {
            cpu_lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
            if (BCM_FAILURE
                (_bcmx_dest_to_modid_port(cpu_lport,
                                          &bcm_l3.l3a_modid,
                                          &bcm_l3.l3a_port_tgid,
                                          BCMX_DEST_CONVERT_DEFAULT))) {
                /* Skip if can't find local CPU port */
                continue;
            }
        }
        tmp_rv = bcm_l3_host_add(bcm_unit, &bcm_l3);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_host_delete
 */

int
bcmx_l3_host_delete(bcmx_l3_host_t *ip_addr)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_host_t bcm_l3;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ip_addr);

    BCM_IF_ERROR_RETURN(_bcmx_l3ip_to_bcm(ip_addr, &bcm_l3, FALSE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_host_delete(bcm_unit, &bcm_l3);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_host_delete_by_network
 */

int
bcmx_l3_host_delete_by_network(bcmx_l3_route_t *route)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_route_t l3_route;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(route);

    BCM_IF_ERROR_RETURN(_bcmx_l3route_to_bcm(route, &l3_route, FALSE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_host_delete_by_network(bcm_unit, &l3_route);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_host_delete_by_interface
 */

int
bcmx_l3_host_delete_by_interface(bcmx_l3_host_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_host_t bcm_l3;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN(_bcmx_l3ip_to_bcm(info, &bcm_l3, FALSE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_host_delete_by_interface(bcm_unit, &bcm_l3);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_host_delete_all
 */

int
bcmx_l3_host_delete_all(bcmx_l3_host_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_host_t bcm_l3;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN(_bcmx_l3ip_to_bcm(info, &bcm_l3, FALSE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_host_delete_all(bcm_unit, &bcm_l3);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_host_conflict_get
 */

int
bcmx_l3_host_conflict_get(bcm_l3_key_t *ipkey,
                          bcm_l3_key_t *cf_array,
                          int cf_max,
                          int *cf_count)
{
    int rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ipkey);
    BCMX_PARAM_NULL_CHECK(cf_count);
    BCMX_PARAM_ARRAY_NULL_CHECK(cf_max, cf_array);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_host_conflict_get(bcm_unit, ipkey,
                                          cf_array, cf_max, cf_count);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_l3_host_age
 */

int
bcmx_l3_host_age(uint32 flags,
                 bcm_l3_host_traverse_cb age_cb,
                 void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    bcmx_l3_host_traverse
 */

int
bcmx_l3_host_traverse(uint32 flags,
                      uint32 start,
                      uint32 end,
                      bcm_l3_host_traverse_cb cb,
                      void *user_data)
{
    int rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_LOCAL_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_host_traverse(bcm_unit,
                                  flags, start, end,
                                  cb, user_data);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_l3_host_invalidate_entry
 */

int
bcmx_l3_host_invalidate_entry(bcm_ip_t info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_host_invalidate_entry(bcm_unit, info);
        BCM_IF_ERROR_RETURN(BCMX_L3_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_host_validate_entry
 */

int
bcmx_l3_host_validate_entry(bcm_ip_t info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_host_validate_entry(bcm_unit, info);
        BCM_IF_ERROR_RETURN(BCMX_L3_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/**************************************************************
 * bcmx_l3_route_xxx
 */

/*
 * Function:
 *     bcmx_l3_route_add
 */

int
bcmx_l3_route_add(bcmx_l3_route_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_route_t l3_route;
    int map_local_cpu;  /* Is local CPU port mapping required? */
    bcmx_lport_t cpu_lport;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    map_local_cpu = ((!(info->l3a_flags & BCM_L3_TGID)) &&
                     (info->l3a_lport == BCMX_LPORT_LOCAL_CPU));

    BCM_IF_ERROR_RETURN(_bcmx_l3route_to_bcm(info, &l3_route, TRUE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (map_local_cpu) {
            cpu_lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
            if (BCM_FAILURE
                (_bcmx_dest_to_modid_port(cpu_lport,
                                          &l3_route.l3a_modid,
                                          &l3_route.l3a_port_tgid,
                                          BCMX_DEST_CONVERT_DEFAULT))) {
                /* Skip if can't find local CPU port */
                continue;
            }
        }
        tmp_rv = bcm_l3_route_add(bcm_unit, &l3_route);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_route_delete
 */

int
bcmx_l3_route_delete(bcmx_l3_route_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_route_t l3_route;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    /* See bcm_l3_route_delete()
       l3a_port_tgid is an input field if BCM_L3_MULTIPATH
       is set. */
    BCM_IF_ERROR_RETURN
        (_bcmx_l3route_to_bcm(info, &l3_route,
                              info->l3a_flags & BCM_L3_MULTIPATH));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_route_delete(bcm_unit, &l3_route);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_route_delete_by_interface
 */

int
bcmx_l3_route_delete_by_interface(bcmx_l3_route_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_route_t l3_route;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN(_bcmx_l3route_to_bcm(info, &l3_route, FALSE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_route_delete_by_interface(bcm_unit, &l3_route);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_route_delete_all
 */

int
bcmx_l3_route_delete_all(bcmx_l3_route_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_route_t l3_route;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN(_bcmx_l3route_to_bcm(info, &l3_route, FALSE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_route_delete_all(bcm_unit, &l3_route);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_route_get
 */

int
bcmx_l3_route_get(bcmx_l3_route_t *info)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_route_t l3_route;
    uint32 merged_flags = 0;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN(_bcmx_l3route_to_bcm(info, &l3_route, FALSE));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_route_get(bcm_unit, &l3_route);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                merged_flags |= l3_route.l3a_flags;
            } else {
                break;
            }
        }
        /* bcm_l3_route_get() may overwrite input flags */
        l3_route.l3a_flags = info->l3a_flags;
    }

    if (BCM_SUCCESS(rv)) {
        l3_route.l3a_flags  |= merged_flags;
        BCM_IF_ERROR_RETURN(_bcmx_l3route_from_bcm(info, &l3_route));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_route_multipath_get
 */

int
bcmx_l3_route_multipath_get(bcmx_l3_route_t *the_route,
                            bcmx_l3_route_t *path_array,
                            int max_path,
                            int *path_count)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l3_route_t l3_route, *l3_route_array, *l3p;
    int num_bcm_path = 0;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(the_route);
    BCMX_PARAM_NULL_CHECK(path_count);
    BCMX_PARAM_ARRAY_NULL_CHECK(max_path, path_array);

    BCM_IF_ERROR_RETURN(_bcmx_l3route_to_bcm(the_route, &l3_route, FALSE));

    l3_route_array = sal_alloc(sizeof(bcm_l3_route_t) * max_path,
                               "BCMX_L3_RGAP");
    if (l3_route_array == NULL) {
        return BCM_E_MEMORY;
    }

    /* Init route array structs */
    for (i=0; i<max_path; i++ ) {
        bcm_l3_route_t_init(l3_route_array + i);
    }

    l3p = l3_route_array;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_route_multipath_get(bcm_unit, &l3_route,
                                           l3p,
                                           max_path,
                                           &num_bcm_path);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            break;
        }
    }

    if (BCM_SUCCESS(rv)) {
        /* Convert bcm routes to bcmx */
        for (i=0, l3p=l3_route_array; i<num_bcm_path; i++) {
            rv = _bcmx_l3route_from_bcm(path_array+i, l3p+i);
        }
    } else {
        num_bcm_path = 0;
    }

    *path_count = num_bcm_path;
    sal_free(l3_route_array);

    return rv;
}

/*
 * Function:
 *     bcmx_l3_route_age
 */

int
bcmx_l3_route_age(uint32 flags,
                  bcm_l3_route_traverse_cb age_out,
                  void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_l3_route_traverse
 */

int
bcmx_l3_route_traverse(uint32 flags,
                       uint32 start,
                       uint32 end,
                       bcm_l3_route_traverse_cb trav_fn,
                       void *user_data)
{
    int rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_LOCAL_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_route_traverse(bcm_unit,
                                   flags, start, end,
                                   trav_fn, user_data);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_l3_route_max_ecmp_set
 */

int
bcmx_l3_route_max_ecmp_set(int max)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_route_max_ecmp_set(bcm_unit, max);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_l3_route_max_ecmp_get
 */

int
bcmx_l3_route_max_ecmp_get(int *max)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    int tmp_max;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(max);

    /* Get the minimum value of all units */
    *max = 0;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_route_max_ecmp_get(bcm_unit, &tmp_max);

        if (BCMX_L3_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            BCM_IF_ERROR_RETURN(rv);
            if ((*max == 0) || (tmp_max < *max)) {
                *max = tmp_max;
            }
        }
    }

    return rv;
}

/**************************************************************
 * bcmx_l3_defip_xxx (superseded by bcmx_l3_route_xxx)
 */


/*
 * Function:
 *      bcmx_l3_vrf_stat_enable_set
 * Purpose:
 *      Enable/disable packet and byte counters for the selected VRF.
 * Parameters:
 *      vrf    - Virtual router instance
 *      enable - Non-zero to enable counter collection, zero to disable
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_vrf_stat_enable_set(bcm_vrf_t vrf, int enable)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrf_stat_enable_set(bcm_unit, vrf, enable);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_vrf_stat_get
 * Purpose:
 *      Get 64-bit counter value for specified VRF statistic type.
 * Parameters:
 *      vrf  - Virtual router instance
 *      stat - Type of the counter to retrieve
 *      val  - (OUT) Pointer to a counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_vrf_stat_get(bcm_vrf_t vrf, bcm_l3_vrf_stat_t stat, uint64 *val)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    uint64  tmp_val;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(val);

    COMPILER_64_ZERO(*val);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrf_stat_get(bcm_unit, vrf, stat, &tmp_val);

        if (BCMX_L3_GET_IS_VALID(bcm_unit, tmp_rv)) {
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
     
/*
 * Function:
 *      bcmx_l3_vrf_stat_get32
 * Purpose:
 *      Get lower 32-bit counter value for specified VRF statistic
 *      type.
 * Parameters:
 *      vrf  - Virtual router instance
 *      stat - Type of the counter to retrieve
 *      val  - (OUT) Pointer to a counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_vrf_stat_get32(bcm_vrf_t vrf, bcm_l3_vrf_stat_t stat, uint32 *val)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    uint32  tmp_val;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(val);

    *val = 0;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrf_stat_get32(bcm_unit, vrf, stat, &tmp_val);

        if (BCMX_L3_GET_IS_VALID(bcm_unit, tmp_rv)) {
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
 *      bcmx_l3_vrf_stat_set
 * Purpose:
 *      Set 64-bit counter value for specified VRF statistic type.
 * Parameters:
 *      vrf  - Virtual router instance
 *      stat - Type of the counter to set
 *      val  - New counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_vrf_stat_set(bcm_vrf_t vrf, bcm_l3_vrf_stat_t stat, uint64 val)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrf_stat_set(bcm_unit, vrf, stat, val);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_vrf_stat_set32
 * Purpose:
 *      Set 32-bit counter value for specified VRF statistic type.
 * Parameters:
 *      vrf  - Virtual router instance
 *      stat - Type of the counter to set
 *      val  - New counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_vrf_stat_set32(bcm_vrf_t vrf, bcm_l3_vrf_stat_t stat, uint32 val)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrf_stat_set32(bcm_unit, vrf, stat, val);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_vrf_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple VRF statistic types.
 * Parameters:
 *      vrf       - Virtual router instance
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_vrf_stat_multi_get(bcm_vrf_t vrf, int nstat, 
                           bcm_l3_vrf_stat_t *stat_arr, uint64 *value_arr)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    uint64  *tmp_val;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    tmp_val = sal_alloc(sizeof(uint64) * nstat, "bcmx l3 vrf stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        COMPILER_64_ZERO(value_arr[i]);
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrf_stat_multi_get(bcm_unit, vrf, nstat,
                                           stat_arr, tmp_val);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, tmp_rv)) {
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
 *      bcmx_l3_vrf_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple VRF statistic types.
 * Parameters:
 *      vrf       - Virtual router instance
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_vrf_stat_multi_get32(bcm_vrf_t vrf, int nstat,
                             bcm_l3_vrf_stat_t *stat_arr, uint32 *value_arr)
{
    int     rv = BCM_E_UNAVAIL, tmp_rv;
    int     i, bcm_unit;
    uint32  *tmp_val;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    tmp_val = sal_alloc(sizeof(uint32) * nstat, "bcmx l3 vrf stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        value_arr[i] = 0;
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrf_stat_multi_get32(bcm_unit, vrf, nstat,
                                             stat_arr, tmp_val);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, tmp_rv)) {
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
 *      bcmx_l3_vrf_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple VRF statistic types.
 * Parameters:
 *      vrf       - Virtual router instance
 *      nstat     - Number of elements in stat array
 *      stat_arr  - New statistics descriptors array
 *      value_arr - New counters values to set
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_vrf_stat_multi_set(bcm_vrf_t vrf, int nstat,
                           bcm_l3_vrf_stat_t *stat_arr, uint64 *value_arr)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrf_stat_multi_set(bcm_unit, vrf, nstat,
                                           stat_arr, value_arr);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_vrf_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple VRF statistic types.
 * Parameters:
 *      vrf       - Virtual router instance
 *      nstat     - Number of elements in stat array
 *      stat_arr  - New statistics descriptors array
 *      value_arr - New counters values to set
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_vrf_stat_multi_set32(bcm_vrf_t vrf, int nstat,
                             bcm_l3_vrf_stat_t *stat_arr, uint32 *value_arr)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrf_stat_multi_set32(bcm_unit, vrf, nstat,
                                             stat_arr, value_arr);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l3_source_bind_enable_set
 * Purpose:
 *      Enable or disable l3 source binding checks on an ingress port.
 * Parameters:
 *      port   - Packet ingress logical port
 *      enable - Non-zero to enable l3 source binding checks,
 *               zero to disable.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_source_bind_enable_set(bcm_gport_t port, int enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_L3_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_l3_source_bind_enable_set(bcm_unit, port, enable);
}

/*
 * Function:
 *      bcmx_l3_source_bind_enable_get
 * Purpose:
 *      Retrieve whether l3 source binding checks are performed on an
 *      ingress port.
 * Parameters:
 *      port   - Packet ingress port.
 *      enable - (OUT) Non-zero if l3 source binding checks are enabled,
 *               zero if disabled.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_source_bind_enable_get(bcm_gport_t port, int *enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(enable);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_l3_source_bind_enable_get(bcm_unit, port, enable);
}

/*
 * Function:
 *      bcmx_l3_source_bind_add
 * Purpose:
 *      Add or replace an L3 source binding.
 * Parameters:
 *      info - L3 source binding information
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_source_bind_add(bcmx_l3_source_bind_t *info)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_source_bind_add(bcm_unit, info);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_source_bind_delete
 * Purpose:
 *      Remove an existing L3 source binding.
 * Parameters:
 *      info - L3 source binding information
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_source_bind_delete(bcmx_l3_source_bind_t *info)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_source_bind_delete(bcm_unit, info);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_source_bind_delete_all
 * Purpose:
 *      Remove all existing L3 source bindings.
 * Parameters:
 *      none
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_l3_source_bind_delete_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_source_bind_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_source_bind_get
 * Purpose:
 *      Retrieve the details of an existing L3 source binding.
 * Parameters:
 *      info - (IN/OUT) L3 source binding information
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Returns data from first successful call.
 */
int
bcmx_l3_source_bind_get(bcmx_l3_source_bind_t *info)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_source_bind_get(bcm_unit,
                                    BCMX_L3_SOURCE_BIND_T_PTR_TO_BCM(info));
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_l3_vrrp_add
 * Purpose:
 *      Add VRID for the given VSI. Adding a VRID using this API means
 *      the physical node has become the master for the virtual router.
 * Parameters:
 *      vlan - VLAN/VSI
 *      vrid - VRID Virtual router ID to be added
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_l3_vrrp_add(bcm_vlan_t vlan, uint32 vrid)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrrp_add(bcm_unit, vlan, vrid);
        BCM_IF_ERROR_RETURN(BCMX_L3_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_vrrp_delete
 * Purpose:
 *      Delete VRID for a particulat VLAN/VSI.
 * Parameters:
 *      vlan - VLAN/VSI
 *      vrid - VRID Virtual router ID to be deleted
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_l3_vrrp_delete(bcm_vlan_t vlan, uint32 vrid)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrrp_delete(bcm_unit, vlan, vrid);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_vrrp_delete_all
 * Purpose:
 *      Delete all the VRIDs for a particular VLAN/VSI.
 * Parameters:
 *      vlan - VLAN/VSI
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_l3_vrrp_delete_all(bcm_vlan_t vlan)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l3_vrrp_delete_all(bcm_unit, vlan);
        BCM_IF_ERROR_RETURN(BCMX_L3_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l3_vrrp_get
 * Purpose:
 *      Get all the VRIDs for which the physical node is master for
 *      the virtual routers on the given VLAN/VSI.
 * Parameters:
 *      vlan       - VLAN/VSI
 *      alloc_size - Size of vrid_array
 *      vrid_array - (OUT) Pointer to the array to which the VRIDs will be copied
 *      count      - (OUT) Number of VRIDs returned
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_l3_vrrp_get(bcm_vlan_t vlan, int alloc_size, int *vrid_array, int *count)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_L3_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(alloc_size, vrid_array);
    BCMX_PARAM_NULL_CHECK(count);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l3_vrrp_get(bcm_unit, vlan, alloc_size, vrid_array, count);
        if (BCMX_L3_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

#endif  /* INCLUDE_L3 */
