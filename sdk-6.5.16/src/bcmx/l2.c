/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/l2.c
 * Purpose:     BCMX L2 APIs
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/l2.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

#define BCMX_L2_INIT_CHECK    BCMX_READY_CHECK
 
#define BCMX_L2_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_L2_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_L2_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


STATIC INLINE void
_bcmx_l2_addr_t_init(bcmx_l2_addr_t *l2addr,
                     bcm_mac_t mac_addr,
                     bcm_vlan_t vid)
{
    if (l2addr != NULL) {
        sal_memset(l2addr, 0, sizeof (*l2addr));
        sal_memcpy(l2addr->mac, mac_addr, sizeof(bcm_mac_t));
        l2addr->vid = vid;
    }
}

STATIC INLINE int
_bcmx_l2_addr_to_bcm(int bcm_unit,
                     bcm_l2_addr_t *dest,
                     bcmx_l2_addr_t *source,
                     bcmx_lplist_t *port_block)
{
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   to_bcm;
    _bcmx_dest_bcmx_t  from_bcmx;
    
    bcm_l2_addr_t_init(dest, source->mac, source->vid);
    dest->flags = source->flags;

    /*
     * Convert destination data
     */
    _bcmx_dest_bcmx_t_init(&from_bcmx);
    
    /* Set flags and data to convert */
    if (source->flags & BCM_L2_MCAST) {
        flags |= BCMX_DEST_MCAST;
    } else if (source->flags & BCM_L2_TRUNK_MEMBER) {
        flags |= BCMX_DEST_TRUNK;
    }
    from_bcmx.port  = source->lport;
    from_bcmx.trunk = source->tgid;
    from_bcmx.mcast = source->l2mc_group;

    /* Convert */
    BCM_IF_ERROR_RETURN(_bcmx_dest_to_bcm(&from_bcmx, &to_bcm, &flags));

    /* Set converted flags and data */
    if (flags & BCMX_DEST_MCAST) {
        dest->flags |= BCM_L2_MCAST;
    } else if (flags & BCMX_DEST_TRUNK) {
        dest->flags |= BCM_L2_TRUNK_MEMBER;
    } else if (flags & BCMX_DEST_DISCARD) {
        dest->flags |= BCM_L2_DISCARD_DST;
    }
    dest->modid      = (int)to_bcm.module_id;
    dest->port       = (int)to_bcm.module_port;
    dest->tgid       = to_bcm.trunk;
    dest->l2mc_group = to_bcm.mcast;

    /* Set remaining fields */
    dest->cos_dst = source->cos_dst;
    dest->cos_src = source->cos_src;
    dest->auth    = source->auth;
    dest->group   = source->group;

    BCM_PBMP_CLEAR(dest->block_bitmap);
    if (!BCMX_LPLIST_IS_EMPTY(port_block)) {
        BCMX_LPLIST_TO_PBMP(*port_block, bcm_unit,
                            dest->block_bitmap);
    }

    return BCM_E_NONE;
}

STATIC INLINE int
_bcmx_l2_addr_from_bcm(bcmx_l2_addr_t *dest,
                       bcmx_lplist_t *port_block,
                       bcm_l2_addr_t *source)
{
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   from_bcm;
    _bcmx_dest_bcmx_t  to_bcmx;

    _bcmx_l2_addr_t_init(dest, source->mac, source->vid);
    dest->flags = source->flags;

    /*
     * Convert destination data
     */
    _bcmx_dest_bcm_t_init(&from_bcm);

    /* Set flags and data to convert */
    if (source->flags & BCM_L2_MCAST) {
        flags |= BCMX_DEST_MCAST;
    } else if (source->flags & BCM_L2_TRUNK_MEMBER) {
        flags |= BCMX_DEST_TRUNK;
    }
    from_bcm.module_id   = (bcm_module_t)source->modid;
    from_bcm.module_port = (bcm_port_t)source->port;
    from_bcm.trunk       = source->tgid;
    from_bcm.mcast       = source->l2mc_group;

    /* Convert */
    BCM_IF_ERROR_RETURN(_bcmx_dest_from_bcm(&to_bcmx, &from_bcm, &flags));

    /* Set converted flags and data */
    if (flags & BCMX_DEST_MCAST) {
        dest->flags |= BCM_L2_MCAST;
    } else if (flags & BCMX_DEST_TRUNK) {
        dest->flags |= BCM_L2_TRUNK_MEMBER;
    } else if (flags & BCMX_DEST_DISCARD) {
        dest->flags |= BCM_L2_DISCARD_DST;
    }
    dest->lport      = to_bcmx.port;
    dest->tgid       = to_bcmx.trunk;
    dest->l2mc_group = to_bcmx.mcast;

    /* Set remaining fields */
    dest->cos_dst = source->cos_dst;
    dest->cos_src = source->cos_src;
    dest->auth = source->auth;
    dest->group = source->group;
    
    if (port_block != NULL && BCMX_LPORT_VALID(dest->lport)) {
        bcmx_lplist_init(port_block, -1, 0);
        BCMX_LPLIST_PBMP_ADD(port_block,
                             bcmx_lport_bcm_unit(dest->lport),
                             source->block_bitmap);
        bcmx_lplist_uniq(port_block);
    }

    return BCM_E_NONE;
}

STATIC INLINE int
_bcmx_l2_learn_limit_to_bcm(bcm_l2_learn_limit_t *dest,
                            int *dest_unit,
                            bcmx_l2_learn_limit_t *source)
{
    bcm_l2_learn_limit_t_init(dest);

    *dest_unit  = -1;
    dest->flags = source->flags;
    dest->vlan  = source->vlan;
    dest->port  = source->lport;
    dest->trunk = source->trunk;
    dest->limit = source->limit;

    if (source->flags & BCM_L2_LEARN_LIMIT_PORT) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(source->lport,
                                     dest_unit, &dest->port,
                                     BCMX_DEST_CONVERT_DEFAULT));
    }

    return BCM_E_NONE;
}

STATIC INLINE int
_bcmx_l2_learn_limit_from_bcm(bcmx_l2_learn_limit_t *dest,
                              bcm_l2_learn_limit_t *source,
                              int bcm_unit)
{
    bcmx_l2_learn_limit_t_init(dest);

    dest->flags = source->flags;
    dest->vlan  = source->vlan;
    dest->lport = source->port;
    dest->trunk = source->trunk;
    dest->limit = source->limit;

    if (source->flags & BCM_L2_LEARN_LIMIT_PORT) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_from_unit_port(&dest->lport,
                                       bcm_unit, source->port,
                                       BCMX_DEST_CONVERT_DEFAULT));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmx_l2_addr_t_init
 */

void
bcmx_l2_addr_t_init(bcmx_l2_addr_t *l2addr,
                    bcm_mac_t mac_addr,
                    bcm_vlan_t vid)
{
    _bcmx_l2_addr_t_init(l2addr, mac_addr, vid);
}


/*
 * Function:
 *      bcmx_l2_learn_limit_t_init
 */

void
bcmx_l2_learn_limit_t_init(bcmx_l2_learn_limit_t *limit)
{
    if (limit != NULL) {
        sal_memset(limit, 0, sizeof(*limit));
    }
    return;
}


/*
 * Function:
 *      bcmx_l2_addr_add
 */

int
bcmx_l2_addr_add(bcmx_l2_addr_t *l2addr, bcmx_lplist_t *port_block)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l2_addr_t bcm_l2;
    int map_local_cpu;  /* Is local CPU port mapping required? */
    bcmx_lport_t cpu_lport;
    bcm_module_t modid;
    bcm_port_t modport;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(l2addr);

    map_local_cpu = ((!(l2addr->flags & BCM_L2_TRUNK_MEMBER)) &&
                     (l2addr->lport == BCMX_LPORT_LOCAL_CPU));

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (_bcmx_l2_addr_to_bcm(bcm_unit, &bcm_l2, l2addr, port_block) < 0) {
            LOG_WARN(BSL_LS_BCMX_COMMON,
                     (BSL_META("BCMX L2 WARN:  Failed to convert "
                               "L2 address to BCM\n")));
        } else {
            if (map_local_cpu) {
                cpu_lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
                if (BCM_FAILURE
                    (_bcmx_dest_to_modid_port(cpu_lport,
                                              &modid,
                                              &modport,
                                              BCMX_DEST_CONVERT_DEFAULT))) {
                    /* Skip if can't find local CPU port */
                    continue;
                }
                bcm_l2.modid = (int)modid;
                bcm_l2.port  = (int)modport;
            }
            tmp_rv = bcm_l2_addr_add(bcm_unit, &bcm_l2);

            BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l2_addr_refresh
 * Purpose:
 *      The address is first looked up on the native unit.  If found,
 *      the address is updated on all other units.
 * Notes:
 *      This allows the hit-bit state not to be changed on the native unit.
 *
 *      No consistency checking is done to see if the addr shows up
 *      on multiple units as native.
 */

int
bcmx_l2_addr_refresh(bcmx_l2_addr_t *l2addr, bcmx_lplist_t *port_block)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l2_addr_t bcm_l2;
    bcmx_l2_addr_t bcmx_l2_native;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(l2addr);

    rv = bcmx_l2_addr_native_get(l2addr->mac, l2addr->vid, &bcmx_l2_native);

    if (BCM_SUCCESS(rv)) {/* Address exists on native device. Update */
        BCMX_UNIT_ITER(bcm_unit, i) {
            if (_bcmx_l2_addr_to_bcm(bcm_unit, &bcm_l2,
                                     l2addr, port_block) < 0) {
                LOG_WARN(BSL_LS_BCMX_COMMON,
                         (BSL_META("BCMX L2 WARN:  Failed to convert "
                                   "L2 address to BCM\n")));
            }
            tmp_rv = bcm_l2_addr_add(bcm_unit, &bcm_l2);
            BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}


int
bcmx_l2_addr_delete(bcm_mac_t mac, bcm_vlan_t vid)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_delete(bcm_unit, mac, vid);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_l2_addr_delete_by_port(bcmx_lport_t port, uint32 flags)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_module_t  modid;
    bcm_port_t    modport;

    BCMX_L2_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(port,
                                  &modid, &modport,
                                  BCMX_DEST_CONVERT_DEFAULT));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_delete_by_port(bcm_unit,
                                            modid,
                                            modport,
                                            flags);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_l2_addr_delete_by_mac(bcm_mac_t mac, uint32 flags)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_delete_by_mac(bcm_unit, mac, flags);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_l2_addr_delete_by_vlan(bcm_vlan_t vid, uint32 flags)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_delete_by_vlan(bcm_unit, vid, flags);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_l2_addr_delete_by_trunk(bcm_trunk_t tid, uint32 flags)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_delete_by_trunk(bcm_unit, tid, flags);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_l2_addr_delete_by_mac_port(bcm_mac_t mac,
                                bcmx_lport_t port,
                                uint32 flags)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_module_t  modid;
    bcm_port_t    modport;

    BCMX_L2_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(port,
                                  &modid, &modport,
                                  BCMX_DEST_CONVERT_DEFAULT));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_delete_by_mac_port(bcm_unit,
                                                mac,
                                                modid,
                                                modport,
                                                flags);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_l2_addr_delete_by_vlan_port(bcm_vlan_t vid,
                                 bcmx_lport_t port,
                                 uint32 flags)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
    bcm_module_t  modid;
    bcm_port_t    modport;

    BCMX_L2_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(port,
                                  &modid, &modport,
                                  BCMX_DEST_CONVERT_DEFAULT));

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_delete_by_vlan_port(bcm_unit,
                                                 vid,
                                                 modid,
                                                 modport,
                                                 flags);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

int
bcmx_l2_addr_delete_by_vlan_trunk(bcm_vlan_t vid,
                                  bcm_trunk_t tid,
                                  uint32 flags)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_delete_by_vlan_trunk(bcm_unit,
                                                  vid,
                                                  tid,
                                                  flags);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l2_addr_get
 */

int
bcmx_l2_addr_get(bcm_mac_t mac_addr,
                 bcm_vlan_t vid,
                 bcmx_l2_addr_t *l2addr,
                 bcmx_lplist_t *port_block)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l2_addr_t bcm_l2;
    int first_time = TRUE;
    uint32 merged_flags = 0;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(l2addr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        bcm_l2_addr_t_init (&bcm_l2, mac_addr, vid);
        tmp_rv = bcm_l2_addr_get(bcm_unit, mac_addr, vid, &bcm_l2);

        if (BCMX_L2_GET_IS_VALID(bcm_unit, tmp_rv)) {

            /* If not found in this unit, try the next one */
            if (tmp_rv == BCM_E_NOT_FOUND) {
                /*
                 * Set BCM_E_NOT_FOUND if entry has not been found yet,
                 * in case it is not found in any unit.
                 */
                if (rv != BCM_E_NONE) {
                    rv = tmp_rv;
                }
                continue;
            }

            rv = tmp_rv;
            if (BCM_FAILURE(tmp_rv)) {
                break;
            }

            /* Successfully found addr */
            merged_flags |= bcm_l2.flags;
            if (first_time) { /* Call address conversion */
                first_time = FALSE;
                tmp_rv = bcmx_l2_addr_from_bcm(l2addr, port_block, &bcm_l2);
                if (BCM_FAILURE(tmp_rv)) {
                    rv = tmp_rv;
                    break;
                }
            } else if (port_block != NULL) {
                /* Not first time, add to port block list for address */
                BCMX_LPLIST_PBMP_ADD(port_block,
                                     bcm_unit,
                                     bcm_l2.block_bitmap);
            }
        }
    }

    if (BCM_SUCCESS(rv)) {
        l2addr->flags |= merged_flags;
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l2_addr_native_get
 * Notes:
 *      Like l2_addr_get, but look for the (a) device which
 *      has the unit on a front panel port.
 *
 *      Does not provide any information about the blocked ports for
 *      the L2 entry.
 *
 *      Will return if L2 addr found on CPU port.
 */

int
bcmx_l2_addr_native_get(bcm_mac_t mac_addr,
                        bcm_vlan_t vid,
                        bcmx_l2_addr_t *l2addr)

{
    int rv;
    int i, bcm_unit;
    bcm_l2_addr_t bcm_l2;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(l2addr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        bcm_l2_addr_t_init (&bcm_l2, mac_addr, vid);
        rv = bcm_l2_addr_get(bcm_unit, mac_addr, vid, &bcm_l2);

        if (BCMX_L2_GET_IS_VALID(bcm_unit, rv)) {

            if (BCM_SUCCESS(rv)) { /* Device knows addr; front panel? */
                /* Get the port configuration for the device */
                rv = bcm_l2_port_native(bcm_unit, bcm_l2.modid, bcm_l2.port);

                if (BCM_SUCCESS(rv)) {
                    /* Okay, found a device with the port on the front panel */
                    rv = bcmx_l2_addr_from_bcm(l2addr, NULL, &bcm_l2);
                }
            }

            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_l2_addr_port_get
 * Notes:
 *      Like l2_addr_get, but gets the l2 entry from a unit specified
 *      by an lport.  Most often used to get unit specific hit bit status.
 */

int
bcmx_l2_addr_port_get(bcmx_lport_t port,
                      bcm_mac_t mac_addr,
                      bcm_vlan_t vid,
                      bcmx_l2_addr_t *l2addr)
{
    int rv, bcm_unit;
    bcm_l2_addr_t bcm_l2;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(l2addr);

    if (!BCMX_LPORT_VALID(port)) {
        return BCM_E_PORT;
    }

    bcm_unit = bcmx_lport_bcm_unit(port);

    bcm_l2_addr_t_init (&bcm_l2, mac_addr, vid);
    rv = bcm_l2_addr_get(bcm_unit, mac_addr, vid, &bcm_l2);
    if (BCM_SUCCESS(rv)) {
        rv = _bcmx_l2_addr_from_bcm(l2addr, NULL, &bcm_l2);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l2_age_timer_set
 */

int
bcmx_l2_age_timer_set(int age_seconds)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_age_timer_set(bcm_unit, age_seconds);
        BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_age_timer_get
 * Notes:
 *      Return results from first BCM unit that returns success
 */

int
bcmx_l2_age_timer_get(int *age_seconds)
{
    int rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(age_seconds);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l2_age_timer_get(bcm_unit, age_seconds);
        if (BCMX_L2_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_l2_addr_freeze
 */

int
bcmx_l2_addr_freeze(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_freeze(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_addr_thaw
 */

int
bcmx_l2_addr_thaw(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_addr_thaw(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_conflict_get
 * Notes:
 *      Returns data from first successful call.
 */

int
bcmx_l2_conflict_get(bcmx_l2_addr_t *addr,
                     bcmx_l2_addr_t *cf_array,
                     int cf_max,
                     int *cf_count)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l2_addr_t bcm_addr;
    bcm_l2_addr_t *bcm_cf_ptr, *bcm_cf_base;
    int tmp_size;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(addr);
    BCMX_PARAM_NULL_CHECK(cf_count);
    BCMX_PARAM_ARRAY_NULL_CHECK(cf_max, cf_array);

    tmp_size = sizeof(bcm_l2_addr_t) * cf_max;
    bcm_cf_ptr = bcm_cf_base = sal_alloc(tmp_size, "bcmx_l2_conflict_get");
    if (bcm_cf_ptr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(bcm_cf_ptr, 0, tmp_size);

    BCMX_UNIT_ITER(bcm_unit, i) {
        /* Convert L2 address to bcm type */
        tmp_rv = bcmx_l2_addr_to_bcm(bcm_unit, &bcm_addr, addr, NULL);
        if (BCM_FAILURE(tmp_rv)) {
            continue;
        }

        /*
         * On first successful call, convert data to bcmx type
         * and return information from the unit
         */
        tmp_rv = bcm_l2_conflict_get(bcm_unit, &bcm_addr, bcm_cf_ptr,
                                     cf_max, cf_count);
        if (BCMX_L2_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (i = 0; i < *cf_count; i++, cf_array++, bcm_cf_ptr++) {
                    bcmx_l2_addr_from_bcm(cf_array, NULL, bcm_cf_ptr);
                    cf_array->bcm_unit = bcm_unit;    /* Set unit */
                }
                break;
            }
        }
    }

    sal_free(bcm_cf_base);
    return rv;
}

/*
 * Function:
 *      bcmx_l2_tunnel_add
 * Purpose:
 *      Add a (MAC, VLAN) for tunnel/MPLS processing, frames
 *      destined to (MAC, VLAN) is subjected to TUNNEL/MPLS processing.
 * Parameters:
 *      mac  - MAC address
 *      vlan - VLAN ID
 */

int
bcmx_l2_tunnel_add(bcm_mac_t mac, bcm_vlan_t vlan)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_tunnel_add(bcm_unit, mac, vlan);
        BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l2_tunnel_delete
 * Purpose:
 *      Remove a tunnel processing indicator for an L2 address
 * Parameters:
 *      mac  - MAC address
 *      vlan - VLAN ID
 */

int
bcmx_l2_tunnel_delete(bcm_mac_t mac, bcm_vlan_t vlan)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_tunnel_delete(bcm_unit, mac, vlan);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l2_tunnel_delete_all
 * Purpose:
 *      Remove all tunnel processing indicating L2 addresses
 * Parameters:
 *      None
 */

int
bcmx_l2_tunnel_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_tunnel_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_L2_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/****************************************************************
 *
 * Convert L2 data types between BCM and BCMX
 *
 * Information is lost because the bcm_l2_addr only contains
 * the block bitmap on the local unit.
 *
 * The bcm_unit is used to determine which block ports to check
 */

int
bcmx_l2_addr_to_bcm(int bcm_unit,
                    bcm_l2_addr_t *dest,
                    bcmx_l2_addr_t *source,
                    bcmx_lplist_t *port_block)
{
    BCMX_L2_INIT_CHECK;

    return _bcmx_l2_addr_to_bcm(bcm_unit, dest, source, port_block);
}

int
bcmx_l2_addr_from_bcm(bcmx_l2_addr_t *dest,
                      bcmx_lplist_t *port_block,
                      bcm_l2_addr_t *source)
{
    BCMX_L2_INIT_CHECK;

    return _bcmx_l2_addr_from_bcm(dest, port_block, source);
}


/*
 * Function:
 *     bcmx_l2_learn_limit_set
 * Purpose:
 *     Set the L2 MAC learning limit
 * Parameters:
 *     limit        - BCMX learn limit control info structure
 * Return:
 *     BCM_E_NONE   Success
 *     BCM_E_XXX    Fail
 */
int
bcmx_l2_learn_limit_set(bcmx_l2_learn_limit_t *limit)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    bcm_l2_learn_limit_t  bcm_l2;

    
    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(limit);

    BCM_IF_ERROR_RETURN(_bcmx_l2_learn_limit_to_bcm(&bcm_l2, &bcm_unit,
                                                    limit));
    
    /* This is for a specific unit/port */
    if (bcm_unit >= 0) {
        tmp_rv = bcm_l2_learn_limit_set(bcm_unit, &bcm_l2);
        BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    } else {
        /* Do it for all units */
        BCMX_UNIT_ITER(bcm_unit, i) {
            tmp_rv = bcm_l2_learn_limit_set(bcm_unit, &bcm_l2);
            BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }
    
    return rv;
}


/*
 * Function:
 *     bcmx_l2_learn_limit_get
 * Purpose:
 *     Get the L2 MAC learning limit
 * Parameters:
 *     limit        - BCMX learn limit control info structure
 * Return:
 *     BCM_E_NONE   Success
 *     BCM_E_XXX    Fail
 * Notes:
 *     Returns on first successful find
 */
int
bcmx_l2_learn_limit_get(bcmx_l2_learn_limit_t *limit)
{
    int  rv = BCM_E_UNAVAIL;
    int  i, bcm_unit;
    bcm_l2_learn_limit_t  bcm_l2;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(limit);

    BCM_IF_ERROR_RETURN(_bcmx_l2_learn_limit_to_bcm(&bcm_l2, &bcm_unit,
                                                    limit));

    /* This is for a specific unit/port */
    if (bcm_unit >= 0) {
        rv = bcm_l2_learn_limit_get(bcm_unit, &bcm_l2);
    } else {
        bcm_l2_learn_limit_t bcm_l2_orig;

        bcm_l2_orig = bcm_l2;

        BCMX_UNIT_ITER(bcm_unit, i) {
            rv = bcm_l2_learn_limit_get(bcm_unit, &bcm_l2);

            if (BCMX_L2_GET_IS_VALID(bcm_unit, rv)) {
                break;
            }
            bcm_l2 = bcm_l2_orig;
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = _bcmx_l2_learn_limit_from_bcm(limit, &bcm_l2, bcm_unit);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_learn_class_set
 */
int
bcmx_l2_learn_class_set(int lclass, int lclass_prio, uint32 flags)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_learn_class_set(bcm_unit, lclass, lclass_prio, flags);
        BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l2_learn_class_get
 */
int
bcmx_l2_learn_class_get(int lclass, int *lclass_prio, uint32 *flags)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(lclass_prio);
    BCMX_PARAM_NULL_CHECK(flags);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l2_learn_class_get(bcm_unit, lclass, lclass_prio, flags);
        if (BCMX_L2_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_l2_learn_port_class_set
 * Notes:
 *      The port is globally significant,
 *      so this must be set for all units.
 */
int
bcmx_l2_learn_port_class_set(bcmx_lport_t port, int lclass)
{
    int           rv = BCM_E_UNAVAIL, tmp_rv;
    int           i, bcm_unit;
        
    BCMX_L2_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_learn_port_class_set(bcm_unit, port, lclass);
        BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_l2_learn_port_class_get
 */
int
bcmx_l2_learn_port_class_get(bcmx_lport_t port, int *lclass)
{
    int           rv;
    int           i, bcm_unit;

    BCMX_L2_INIT_CHECK;

    BCMX_LPORT_CHECK(port);
    BCMX_PARAM_NULL_CHECK(lclass);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l2_learn_port_class_get(bcm_unit, port, lclass);
        if (BCMX_L2_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_l2_replace
 */
int
bcmx_l2_replace(uint32 flags, bcmx_l2_addr_t *match_addr, 
                bcmx_lport_t new_port, bcm_trunk_t new_trunk)
{
    int             rv = BCM_E_UNAVAIL, tmp_rv;
    int             i, bcm_unit;
    bcm_l2_addr_t   bcm_l2;
    bcm_module_t    modid;
    bcm_port_t      modport;
    bcmx_lport_t    cpu_lport;
    int             match_local_cpu;
    int             new_local_cpu;
    bcm_module_t    new_bcm_module;
    bcm_port_t      new_bcm_port;

    BCMX_L2_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(match_addr);

    match_local_cpu = (match_addr->lport == BCMX_LPORT_LOCAL_CPU) ?
        TRUE : FALSE;

    if (BCM_FAILURE(_bcmx_l2_addr_to_bcm(0, &bcm_l2, match_addr, NULL))) {
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META("BCMX L2 WARN:  Failed to convert "
                           "L2 address to BCM\n")));
        return BCM_E_PARAM;
    }

    new_local_cpu = (new_port == BCMX_LPORT_LOCAL_CPU) ? TRUE : FALSE;

    if (!new_local_cpu) {
        if (flags & BCM_L2_REPLACE_NEW_TRUNK) {
            if (!BCM_GPORT_IS_TRUNK(new_port)) {
                BCM_GPORT_TRUNK_SET(new_bcm_port, new_trunk);
            }
        }
        else {
            BCM_IF_ERROR_RETURN
                (_bcmx_dest_to_modid_port(new_port,
                                          &new_bcm_module, &new_bcm_port,
                                          BCMX_DEST_CONVERT_DEFAULT));
        }
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (match_local_cpu) {
            cpu_lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);

            if (BCM_FAILURE
                (_bcmx_dest_to_modid_port(cpu_lport,
                                          &modid, &modport,
                                          BCMX_DEST_CONVERT_DEFAULT))) {
                continue;
            }
            bcm_l2.modid = (int)modid;
            bcm_l2.port  = (int)modport;
        }

        if (new_local_cpu) {
            cpu_lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
            if (BCM_FAILURE
                (_bcmx_dest_to_modid_port(cpu_lport,
                                          &new_bcm_module, &new_bcm_port,
                                          BCMX_DEST_CONVERT_DEFAULT))) {
                /* Skip if can't find local CPU port */
                continue;
            }
        }

        tmp_rv = bcm_l2_replace(bcm_unit, flags, &bcm_l2,
                                new_bcm_module, new_bcm_port, new_trunk);

        BCM_IF_ERROR_RETURN(BCMX_L2_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}
