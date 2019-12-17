/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * L2 Cache - Layer 2 BPDU and overflow address cache
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

#define BCMX_L2_CACHE_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_L2_CACHE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_L2_CACHE_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_L2_CACHE_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_L2_CACHE_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


STATIC INLINE void
_bcmx_l2_cache_addr_t_init(bcmx_l2_cache_addr_t *addr)
{
    sal_memset(addr, 0, sizeof (*addr));
}

STATIC INLINE int
_bcmx_l2_cache_addr_to_bcm(int bcm_unit,
                           bcm_l2_cache_addr_t *dest,
                           bcmx_l2_cache_addr_t *source)
{
    uint32             flags = BCMX_DEST_CONVERT_NON_GPORT;
    _bcmx_dest_bcm_t   to_bcm;
    _bcmx_dest_bcmx_t  from_bcmx;

    bcm_l2_cache_addr_t_init(dest);
    dest->flags = source->flags;
    sal_memcpy(dest->mac, source->mac, sizeof(bcm_mac_t));
    sal_memcpy(dest->mac_mask, source->mac_mask, sizeof(bcm_mac_t));
    dest->vlan = source->vlan;
    dest->vlan_mask = source->vlan_mask;
    
    dest->src_port = source->src_port;
    dest->src_port_mask = source->src_port_mask;
    dest->lookup_class = source->lookup_class;

    /*
     * Convert to destination port list to bitmap if destination
     * is not a trunk and DESTPORTS flag is set
     */
    if (!(source->flags & BCM_L2_CACHE_TRUNK) &&
        (source->flags & BCM_L2_CACHE_DESTPORTS)) {
        BCM_PBMP_CLEAR(dest->dest_ports);
        BCMX_LPLIST_TO_PBMP(source->dest_ports, bcm_unit, dest->dest_ports);

    } else {
        /*
         * Convert destination data
         */
        _bcmx_dest_bcmx_t_init(&from_bcmx);

        /* Set flags and data to convert */
        if (source->flags & BCM_L2_CACHE_TRUNK) {
            flags |= BCMX_DEST_TRUNK;
        }
        from_bcmx.port  = source->dest_lport;
        from_bcmx.trunk = source->dest_trunk;

        /* Convert */
        BCM_IF_ERROR_RETURN(_bcmx_dest_to_bcm(&from_bcmx, &to_bcm, &flags));

        /* Set converted flags and data */
        if (flags & BCMX_DEST_TRUNK) {
            dest->flags |= BCM_L2_CACHE_TRUNK;
        } else if (flags & BCMX_DEST_DISCARD) {
            dest->flags |= BCM_L2_CACHE_DISCARD;
        }

        dest->dest_modid = to_bcm.module_id;
        dest->dest_port  = to_bcm.module_port;
        dest->dest_trunk = to_bcm.trunk;
    }

    dest->prio = source->prio;

    return BCM_E_NONE;
}

STATIC INLINE int
_bcmx_l2_cache_addr_from_bcm(bcmx_l2_cache_addr_t *dest,
                             bcm_l2_cache_addr_t *source)
{
    uint32             flags = BCMX_DEST_CONVERT_DEFAULT;
    _bcmx_dest_bcm_t   from_bcm;
    _bcmx_dest_bcmx_t  to_bcmx;

    _bcmx_l2_cache_addr_t_init(dest);
    dest->flags = source->flags;
    sal_memcpy(dest->mac, source->mac, sizeof(bcm_mac_t));
    sal_memcpy(dest->mac_mask, source->mac_mask, sizeof(bcm_mac_t));
    dest->vlan = source->vlan;
    dest->vlan_mask = source->vlan_mask;
    
    dest->src_port = source->src_port;
    dest->src_port_mask = source->src_port_mask;
    dest->lookup_class = source->lookup_class;

    /*
     * Convert bitmap to destination port list if destination
     * is not a trunk and DESTPORTS flag is set
     */
    if (!(source->flags & BCM_L2_CACHE_TRUNK) &&
        (source->flags & BCM_L2_CACHE_DESTPORTS)) {
        bcm_port_t port;
        bcmx_lport_t lport;

        bcmx_lplist_clear(&dest->dest_ports);
        BCM_PBMP_ITER(source->dest_ports, port) {
            BCM_IF_ERROR_RETURN
                (_bcmx_dest_from_modid_port(&lport, source->dest_modid, port,
                                            BCMX_DEST_CONVERT_DEFAULT));
            BCMX_LPLIST_ADD(&dest->dest_ports, lport); 
        }

    } else {
        /*
         * Convert destination data
         */
        _bcmx_dest_bcm_t_init(&from_bcm);

        /* Set flags and data to convert */
        if (source->flags & BCM_L2_CACHE_TRUNK) {
            flags |= BCMX_DEST_TRUNK;
        }
        from_bcm.module_id   = source->dest_modid;
        from_bcm.module_port = source->dest_port;
        from_bcm.trunk       = source->dest_trunk;

        /* Convert */
        BCM_IF_ERROR_RETURN(_bcmx_dest_from_bcm(&to_bcmx, &from_bcm, &flags));

        /* Set converted flags and data */
        if (flags & BCMX_DEST_TRUNK) {
            dest->flags |= BCM_L2_CACHE_TRUNK;
        } else if (flags & BCMX_DEST_DISCARD) {
            dest->flags |= BCM_L2_CACHE_DISCARD;
        }

        dest->dest_lport = to_bcmx.port;
        dest->dest_trunk = to_bcmx.trunk;
    }

    dest->prio = source->prio;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmx_l2_cache_init
 */

int
bcmx_l2_cache_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_CACHE_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_cache_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_L2_CACHE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_cache_addr_t_init
 */

void
bcmx_l2_cache_addr_t_init(bcmx_l2_cache_addr_t *addr)
{
    if (addr != NULL) {
        _bcmx_l2_cache_addr_t_init(addr);
        bcmx_lplist_t_init(&addr->dest_ports);
    }
}

/*
 * Function:
 *      bcmx_l2_cache_addr_t_free
 * Purpose:
 *      Free memory allocated to bcmx_l2_cache_addr_t struct
 */
void
bcmx_l2_cache_addr_t_free(bcmx_l2_cache_addr_t *addr)
{
    if (addr != NULL) {
        bcmx_lplist_free(&addr->dest_ports);
    }
}
  

/*
 * Function:
 *      bcmx_l2_cache_size_get
 */

int
bcmx_l2_cache_size_get(int *size)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    int tmp_size = 0, current_size = 0;

    BCMX_L2_CACHE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(size);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_cache_size_get(bcm_unit, &tmp_size);
        if (BCMX_L2_CACHE_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_FAILURE(tmp_rv)) {
                break;
            }

            /* Set current_size to the first non-zero cache size
               or the minimum of all valid cache sizes found. */
            if ((tmp_size > 0) &&
                ((current_size == 0) || (tmp_size < current_size))) {
                current_size = tmp_size;
            }
        }
    }
    *size = current_size;

    return rv;
}


/*
 * Function:
 *      bcmx_l2_cache_set
 */

int
bcmx_l2_cache_set(int index, bcmx_l2_cache_addr_t *addr, int *index_used)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_l2_cache_addr_t bcm_l2;
    int map_local_cpu;  /* Is local CPU port mapping required? */
    bcmx_lport_t cpu_lport;

    BCMX_L2_CACHE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(addr);
    BCMX_PARAM_NULL_CHECK(index_used);

    /* Auto inser is not support across chips of different types */
    if (index < 0) {
        return BCM_E_PARAM;
    }

    map_local_cpu = ((!(addr->flags & BCM_L2_CACHE_TRUNK)) &&
                     (addr->dest_lport == BCMX_LPORT_LOCAL_CPU));

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (_bcmx_l2_cache_addr_to_bcm(bcm_unit, &bcm_l2, addr) < 0) {
            LOG_WARN(BSL_LS_BCMX_COMMON,
                     (BSL_META("BCMX L2 CACHE WARN:  "
                               "Failed to convert "
                               "L2 Cache address to BCM\n")));
        } else {
            if (map_local_cpu) {
                cpu_lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
                if (BCM_FAILURE
                    (_bcmx_dest_to_modid_port(cpu_lport,
                                              &bcm_l2.dest_modid,
                                              &bcm_l2.dest_port,
                                              BCMX_DEST_CONVERT_DEFAULT))) {
                    /* Skip if can't find local CPU port */
                    continue;
                }
            }
            tmp_rv = bcm_l2_cache_set(bcm_unit, index, &bcm_l2, index_used);

            BCM_IF_ERROR_RETURN
                (BCMX_L2_CACHE_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    *index_used = index;

    return rv;
}


/*
 * Function:
 *      bcmx_l2_cache_get
 */

int 
bcmx_l2_cache_get(int index, bcmx_l2_cache_addr_t *addr)
{
    int rv;
    int i, bcm_unit;
    bcm_l2_cache_addr_t bcm_l2;

    BCMX_L2_CACHE_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(addr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_l2_cache_get(bcm_unit, index, &bcm_l2);

        if (BCMX_L2_CACHE_GET_IS_VALID(bcm_unit, rv)) {
            if (BCM_SUCCESS(rv)) {
                rv = _bcmx_l2_cache_addr_from_bcm(addr, &bcm_l2);
            }
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_l2_cache_delete
 */

int
bcmx_l2_cache_delete(int index)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_CACHE_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_cache_delete(bcm_unit, index);
        BCM_IF_ERROR_RETURN
            (BCMX_L2_CACHE_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_l2_cache_delete_all
 */

int
bcmx_l2_cache_delete_all(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_L2_CACHE_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_l2_cache_delete_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_L2_CACHE_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}
