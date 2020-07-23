/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        safc.c
 * Purpose:     SAFC board programming implementation
 */

#include <shared/bsl.h>

#include <sal/core/alloc.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/stack.h>
#include <appl/stktask/topology.h>
#include <appl/stktask/topo_pkt.h>
#include <appl/stktask/topo_brd.h>
#include <appl/stktask/stktask.h>
#include <appl/cputrans/nh_tx.h>
#include <appl/stktask/safc.h>
#include "topo_int.h"

/*
  Generic topology matrix handling.
*/


#define TX_CXN(topo_info, src_idx, dest_idx) \
    (topo_info->tp_tx_cxns[db_ref->num_cpus * src_idx + dest_idx])

#define RX_CXN(topo_info, src_idx, dest_idx) \
    (topo_info->tp_rx_cxns[db_ref->num_cpus * src_idx + dest_idx])

/* modid, unit, stack port index set types */
typedef SHR_BITDCLNAME(modset_t, TOPO_MODIDS_MAX);
typedef SHR_BITDCLNAME(uset_t, BCM_MAX_NUM_UNITS);
typedef SHR_BITDCLNAME(stkset_t, CPUDB_CXN_MAX);

/* SAFC programming data

   'normal' means a port that handles non-SAFC traffic only
   'safc'   means a port than handles SAFC traffic and non-SAFC traffic

      This is because a SAFC ports are a subset of all stack ports.

   'all' is the union of normal and safc ports

*/
typedef struct safc_data_s {
    /* topology inputs */
    topo_cpu_t *tp_cpu;
    cpudb_ref_t db_ref;
    topo_info_t *safc_topo;

    /* BCM programming inputs */
    int group_priority;
    bcm_field_group_t group[BCM_MAX_NUM_UNITS];
    bcm_field_qset_t qset;
    uint32 safc_cos_map;
    uint32 safc_vlan_mask;
    bcm_port_config_t port_config[BCM_MAX_NUM_UNITS];

    /* derived */
    uset_t      units;                      /* set of units with stack ports */
    int         s_idx[TOPO_MODIDS_MAX];     /* modid to safc sp index */
    int         n_idx[TOPO_MODIDS_MAX];     /* modid to normal sp index */
    modset_t    modid[CPUDB_CXN_MAX];       /* sp index to modid set */
    modset_t    blocked[CPUDB_CXN_MAX];     /* sp index to blocked modid set */
    modset_t    all_modid;                  /* all modids in stack */
    bcm_pbmp_t  trunk[CPUDB_CXN_MAX];       /* sp index to trunk ports */
    int         t_idx[CPUDB_CXN_MAX];       /* sp/trunk mapping */
    bcm_pbmp_t  none_pbm_mask;              /* empty PBM */
    bcm_pbmp_t  all_pbm_mask;               /* full PBM */

    bcm_pbmp_t  safc_pbm[BCM_MAX_NUM_UNITS];   /* SAFC stack PBM */
    bcm_pbmp_t  normal_pbm[BCM_MAX_NUM_UNITS]; /* normal *only* stack PBM */
    stkset_t    safc;                       /* set of SAFC sp idxs */
    stkset_t    normal;                     /* set of normal sp idxs */
    stkset_t    trunked;                    /* set of trunked sp idxs */
    stkset_t    s_visit;                    /* visited flags for collect */
    /* true if this system has *only* SAFC ports*/
    int         safc_only;

} safc_data_t;

/* iterator function pointers */
typedef int (*_ufunc_t)(safc_data_t *info, int unit);
typedef int (*_pfunc_t)(safc_data_t *info, int spidx);
typedef int (*_cfunc_t)(safc_data_t *info, int unit, bcm_cos_t cos);

/* unit iterator */
STATIC int
_bcm_board_safc_unit_iterate(safc_data_t *info, _ufunc_t func)
{
    int unit;
    int rv;

    rv = BCM_E_NONE;

    for (unit = 0; unit < BCM_MAX_NUM_UNITS; unit++) {
        if (SHR_BITGET(info->units, unit)) {
            rv = func(info, unit);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    return rv;
}

/* stack port iterator */
STATIC int
_bcm_board_safc_stkport_iterate(safc_data_t *info, _pfunc_t func)
{
    int rv;
    int i;

    rv = BCM_E_NONE;

    for (i = 0; i < info->db_ref->local_entry->base.num_stk_ports; i++) {
        rv = func(info, i);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    return rv;
}

/* cos iterator */
STATIC int
_bcm_board_safc_cos_iterate(safc_data_t *info, int unit, _cfunc_t func)
{
    bcm_cos_t cos;
    int rv;

    for (cos = BCM_COS_MAX; cos >= BCM_COS_MIN; cos--) {
        if ((info->safc_cos_map & (1<<cos)) == 0) {
            continue;
        }
        rv = func(info, unit, cos);
        BCM_IF_ERROR_RETURN(rv);
    }
    return BCM_E_NONE;
}



/*
  Allocate a TX/RX topology matrix.
 */
topo_info_t *_topo_info_t_create(cpudb_ref_t db_ref)
{
    topo_info_t *topo;
    uint8 *rx, *tx;
    int topo_len, cpu_mtx_len;    /* CPUxCPU matrix */

    topo_len = sizeof(*topo);
    cpu_mtx_len = db_ref->num_cpus * db_ref->num_cpus;

    topo = sal_alloc(topo_len, "topo_info_t_create");
    rx = sal_alloc(cpu_mtx_len, "topo_info_t_create");
    tx = sal_alloc(cpu_mtx_len, "topo_info_t_create");
    if (topo != NULL && rx != NULL && tx != NULL) {
        sal_memset(topo, 0,  topo_len);
        sal_memset(rx, TOPO_CXN_UNKNOWN,  cpu_mtx_len);
        sal_memset(tx, TOPO_CXN_UNKNOWN,  cpu_mtx_len);
        topo->tp_rx_cxns = rx;
        topo->tp_tx_cxns = tx;
    } else {
        if (rx) {
            sal_free(rx);
        }
        if (tx) {
            sal_free(tx);
        }
        if (topo) {
            sal_free(topo);

        }
        topo = NULL;
    }

    return topo;

}

/* Mark all cut ports disabled */
STATIC void
_bcm_topo_disable_cut_ports(cpudb_ref_t db_ref)
{
    cpudb_entry_t *entry;
    int i;

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        for (i = 0; i < entry->base.num_stk_ports; i++) {
            if (entry->sp_info[i].flags & CPUDB_SPF_CUT_PORT) {
                entry->sp_info[i].flags |= CPUDB_SPF_TX_DISABLE_FORCE;
            }
        }
    }
}

/* copy out topo info from cpudb */
STATIC void
_bcm_topo_info_copy(topo_info_t *connection, cpudb_ref_t db_ref)
{
    topo_info_t *from;
    int cpu_mtx_len;

    from = (topo_info_t *)db_ref->topo_cookie;
    cpu_mtx_len = db_ref->num_cpus * db_ref->num_cpus;

    if (from) {
        sal_memcpy(connection->tp_rx_cxns, from->tp_rx_cxns, cpu_mtx_len);
        sal_memcpy(connection->tp_tx_cxns, from->tp_tx_cxns, cpu_mtx_len);
    }
}

/* Create a spanning tree topology from the existing CPUDB */
STATIC int
_bcm_topo_use_cut_port_spanning_tree(cpudb_ref_t db_ref,
                                     topo_info_t *connection)
{
    cpudb_ref_t newdb;
    topo_cpu_t *tp_cpu;
    int rv;

    rv = BCM_E_INTERNAL;

    /* topo_cpu_t is pretty big, so alloc from heap, not stack */
    tp_cpu = sal_alloc(sizeof(*tp_cpu), "_bcm_topo_spt");

    if (tp_cpu != NULL) {
        newdb = cpudb_copy(db_ref);
        if (newdb != NULL) {
            _bcm_topo_disable_cut_ports(newdb);
            rv = bcm_stack_topo_create(newdb, tp_cpu);
            if (BCM_SUCCESS(rv)) {
                _bcm_topo_info_copy(connection, newdb);
            }
            cpudb_destroy(newdb);
        } else {
            rv = BCM_E_MEMORY;
        }
        sal_free(tp_cpu);
    } else {
        rv = BCM_E_MEMORY;
    }

    return rv;
}

/* Generic spanning tree calculator - not implemented yet */
STATIC int
_bcm_topo_spanning_tree_create(cpudb_ref_t db_ref,
                               cpudb_entry_t *root,
                               topo_info_t *connection)
{
    COMPILER_REFERENCE(db_ref);
    COMPILER_REFERENCE(root);
    COMPILER_REFERENCE(connection);

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcm_stack_spanning_tree_create(cpudb_ref_t db_ref,
 *                                    cpudb_entry_t *root_entry,
 *                                    topo_info_t **connection)
 * Purpose:
 *     Calculate a local spanning tree topology from db_ref rooted at
 *     root_entry, returning a connection matrix 'connection'.
 * Parameters:
 *     db_ref     - (IN)  CPU database
 *     root_entry - (IN)  Root of spanning tree
 *     connection - (OUT) Calculated topology
 * Returns:
 *     BCM_E_NONE     - no errors
 *     BCM_E_PARAM    - db_ref NULL, db_ref not fully constructed
 *     BCM_E_MEMORY   - not enough memory or system resources
 *     BCM_E_FAIL     - topology calculation failed (CPUDB incomplete)
 * Notes:
 *     Does not perform any device programming.
 *
 *     If root_entry is NULL, then the spanning tree is recovered from
 *     the existing 'CUT Ports' calculated from the default topology.
 */

int
bcm_stack_spanning_tree_create(cpudb_ref_t db_ref,
                               cpudb_entry_t *root,
                               topo_info_t **connection)
{
    int rv;
    topo_info_t *conn = NULL;

    rv = BCM_E_PARAM;

    if (!cpudb_valid(db_ref)) {
        return BCM_E_PARAM;
    }

    if (db_ref->num_cpus > 1) {
        conn = _topo_info_t_create(db_ref);
        if (conn == NULL) {
            return BCM_E_MEMORY;
        }

        if (root == NULL) {
            /* As a shortcut, use existing cut port topology as the
               spanning tree */
            rv = _bcm_topo_use_cut_port_spanning_tree(db_ref, conn);
        } else {
            /* (re)calculate a spanning tree */
            rv = _bcm_topo_spanning_tree_create(db_ref, root, conn);
        }
    } else {
        /* More than one CPU is required for a useful topology */
        rv = BCM_E_NONE;
    }

    if (BCM_SUCCESS(rv)) {
        *connection = conn;
    } else {
        (void)bcm_stack_connection_destroy(conn);
    }

    return rv;
}


/*
 * Function:
 *     int bcm_stack_connection_destroy(topo_info_t *connection)
 * Purpose:
 *     Free a connection data structure
 * Parameters:
 *     connection - (IN) connection structure
 * Returns:
 *     BCM_E_NONE     - no errors
 *     BCM_E_FAIL     - error
 */

int
bcm_stack_connection_destroy(topo_info_t *connection)
{
    if (connection) {
        sal_free((void *)connection->tp_tx_cxns);
        sal_free((void *)connection->tp_rx_cxns);
        sal_free((void *)connection);
    }

    return BCM_E_NONE;
}



/*
  Problem - When SAFC is enabled in a ring configuration (or Stack)
  the flow control among XGS devices create a deadlock in the ring.

  Solution - Break the loop structure of the ring configuration among
  XGS devices "just" for the SAFC enabled traffic.  To break the loop
  structure of the ring, SAFC traffic is redirected to a new Higig
  distribution configuration using IFP.  Basically the current
  MODPORT_MAP Table would generate two HiGIG Output ports for an
  outgoing Higig packet (one for SAFC Traffic, and one for non-SAFC)
  and IFP, based on Traffic class, shall mask one of the outgoing
  Higig Ports (by an IFP rule to "and" the Port Bit Map).

  For SAFC-traffic: One pre-designated Higig link in the ring shall be
  disabled using IFP for SAFC traffic.  XGS devices that are connected
  to the disabled link will not allow SAFC-traffic to be forwarded to
  the this link using an IFP rule. All other XGS devices (which are
  not connected to the disabled link) will forward SAFC traffic
  without applying the regular "SRC MODID based block" .

  For Non-SAFC-Traffic: IFP will apply the SRC_MODID based mask
  (instead of the traditional table driven Mask) so that non-SAFC
  traffic is forwarded as today. Non-SAFC traffic is allowed to flow
  through the disabled link also.

  The following IFP rules are required - (assuming 16 nodes in a ring,
  and one SAFC traffic class and rest are non-SAFC)

  Packets from Higig Ports - (2 rules for every ingress Higig Port)

    o Either IFP higher precedence rule for "SAFC allow" for devices
    that don't have a SAFC-disabled link (Traffic with a specific TC
    value will be unconditionally allowed to go to the outgoing
    Higig-port), Or- IFP higher precedence rule for "SAFC block" for
    devices that have a SAFC-disabled link. (Traffic with a specific
    TC value will be unconditionally not allowed to go to the outgoing
    Higig-port).

    o IFP lower precedence rule for not allowing packet from a
    specific "SRC_MOD_ID" to go to the outgoing Higig port. This rule
    is to replace the current table driven SRC_MOD_ID block. The
    specific SRC_MOD_ID that each node is blocking is dependent on the
    relative position of the device in the ring.
    NOTE: this rule is not used in this implementation. Instead, egress
    masking is used.

  For Front Panel Packets - (16 DST_MOD_ID based rules SAFC Unicast,
  16 DST-MODID based rules for non-SAFC Unicast, 1 rule for SAFC
  Non-Unicast, 1 rule for non-SAFC Non-Unicast)

    o For SAFC- Unicast Traffic, block the local Higig Port that is
    used for the Non-SAFC Distribution Tree. If, for the current node,
    both SAFC and non-SAFC trees use the same local port, then this
    rule has no effect. (For a specific TC, destined to a specific
    MOD_ID, if the packet is Unicast, apply an "and-pbm" to block the
    non-SAFC local port)

    o For SAFC-Non-Unicast Traffic, allow the packet to be forwarded
    to only SAFC -local Higig ports.

    o For Non-SAFC- Unicast Traffic, block the local Higig Port that
    is used for the SAFC Distribution Tree. If, for the current node,
    both SAFC and non-SAFC trees use the same local port, then this
    rule has no effect. (For a specific TC, destined to a specific
    MOD_ID, if the packet is Unicast, apply an "and-pbm" to block the
    SAFC local port)

    o For Non-SAFC-Non-Unicast Traffic, allow the packet to be
    forwarded to only non SAFC- local Higig ports.

  For packets that are mirrored, SAFC distribution tree is selected by
  mapping the DST_MODID to a local Port using the IM/EM_MOD_PORT_MAP
  Table. Since IFP rules are not applied to mirror-copy the packet will
  be forwarded through the ring back to the originating device where
  it shall be dropped (as done today using the non-SAFC tree).

 */

typedef struct {
    int                 found;          /* true if group found */
    int                 priority;       /* priority to search for */
    bcm_field_group_t   group;          /* group which matches priority */
} group_info_t;

/* traverse function to find group matching a priority */
STATIC int
_bcm_board_safc_find_group(int unit, bcm_field_group_t group, void *user)
{
    group_info_t *gi;
    int priority;
    int rv;

    gi = (group_info_t *)user;
    rv = bcm_field_group_priority_get(unit, group, &priority);
    if (BCM_SUCCESS(rv) && priority == gi->priority) {
        gi->found = TRUE;
        gi->group = group;
    }

    return BCM_E_NONE;
}

#define SAFC_DEBUG 1

#ifdef SAFC_DEBUG

/* create a debug counter */
STATIC int
_bcm_board_safc_count_create(int unit,
                             bcm_field_group_t group,
                             bcm_field_entry_t entry,
                             const char *locus)
{
    bcm_field_stat_t stat;
    int id;
    int rv;

    stat = bcmFieldStatPackets;

    rv = bcm_field_stat_create(unit, group, 1, &stat, &id);
    BCM_IF_ERROR_RETURN(rv);

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("counter %d@%s\n"),
                 id, locus));
    return
        (bcm_field_entry_stat_attach(unit, entry, id));

}


/* destroy a debug counter */
STATIC int
_bcm_board_safc_count_destroy(int unit, bcm_field_entry_t entry)
{
    int id;
    int rv;

    rv = bcm_field_entry_stat_get(unit, entry, &id);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_entry_stat_detach(unit, entry, id);
    BCM_IF_ERROR_RETURN(rv);

    return
        (bcm_field_stat_destroy(unit, id));

}

#else
#define _bcm_board_safc_count_create(unit, group, entry, locus) (BCM_E_NONE)
#define _bcm_board_safc_count_destroy(unit, entry) (BCM_E_NONE)
#endif

/* Deallocate field group and entries */
STATIC int
_bcm_board_safc_fp_destroy(int unit, bcm_field_group_t group)
{
    int rv;
    int i;
    bcm_field_entry_t *entry;
    int entry_count;
    int entry_size;

    rv = bcm_field_entry_multi_get(unit, group, 0, NULL, &entry_size);
    if (BCM_SUCCESS(rv) && entry_size > 0) {
        entry = sal_alloc(entry_size * sizeof(*entry), "safc_fp_destroy");
        if (entry) {
            rv = bcm_field_entry_multi_get(unit, group,
                                           entry_size, entry, &entry_count);
            if (BCM_SUCCESS(rv)) {
                for (i = 0; i < entry_count; i++) {
                    rv = _bcm_board_safc_count_destroy(unit, entry[i]);
                    if (BCM_FAILURE(rv)) {
                        break;
                    }
                    rv = bcm_field_entry_remove(unit, entry[i]);
                    if (BCM_FAILURE(rv)) {
                        break;
                    }
                    rv = bcm_field_entry_destroy(unit, entry[i]);
                    if (BCM_FAILURE(rv)) {
                        break;
                    }
                }
            }
            sal_free(entry);
        } else {
            return BCM_E_MEMORY;
        }
    }
    if (BCM_SUCCESS(rv)) {
            rv = bcm_field_group_remove(unit, group);
    }
    if (BCM_SUCCESS(rv)) {
            rv = bcm_field_group_destroy(unit, group);
    }

    return rv;
}

/* unconfigure SAFC field programming */
STATIC int
_bcm_board_safc_unconfigure(int group_priority)
{
    int rv;
    int unit;
    group_info_t gi;

    gi.priority = group_priority;
    gi.found = FALSE;
    BCM_FOREACH_LOCAL_UNIT(unit) {
        (void)bcm_field_group_traverse(unit,
                                       _bcm_board_safc_find_group,
                                       &gi);
        if (gi.found) {
            rv = _bcm_board_safc_fp_destroy(unit, gi.group);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    return BCM_E_NONE;
}

/* per-unit SAFC data initializer function */
STATIC int
_bcm_board_safc_data_unit_init(safc_data_t *info, int unit)
{
    int rv;

    rv = bcm_port_config_get(unit, info->port_config + unit);

    return rv;
}

/*
  Collect the modids for SAFC topology. This is called recursively
  starting from the links of the root node.

  The 'visited flag' is used to prevent an infinite loop in case the
  topology was calculated incorrectly.

 */
STATIC int
_bcm_board_safc_collect_modid(safc_data_t *info,
                              int src_spidx,
                              cpudb_entry_t *dst_entry,
                              int dst_idx)
{
    int              i, rx_idx;
    cpudb_entry_t    *rx_entry;
    cpudb_key_t      rx_key;
    int              rv;

    /* already visited? */
    if (SHR_BITGET(info->s_visit, dst_entry->topo_idx)) {
        /* If the visit bit is set, then we've already been here,
           which should not happen because if this was really a
           spanning tree we are traversing, then there should be no
           loops. */
        return BCM_E_INTERNAL;
    }

    /* mark visited */
    SHR_BITSET(info->s_visit, dst_entry->topo_idx);

    /* descend on any remaining edges */
    for (i = 0; i < dst_entry->base.num_stk_ports; i++) {
        /* skip dst_idx because it points back to the source */
        if (i == dst_idx) {
            continue;
        }

        /* skip obvious non-stack ports, or stacking cut ports */
        if ((dst_entry->sp_info[i].flags &
             (CPUDB_SPF_ETHERNET|CPUDB_SPF_NO_LINK|
              CPUDB_SPF_INACTIVE|CPUDB_SPF_CUT_PORT))) {
            continue;
        }

        /* skip ports with no obvious partner */
        if (!(dst_entry->sp_info[i].flags & CPUDB_SPF_RX_RESOLVED)) {
            continue;
        }

        /* Get the remote RX CPUDB entry and and stack port index */
        CPUDB_KEY_COPY(rx_key, dst_entry->sp_info[i].rx_cpu_key);
        rx_entry = NULL;
        CPUDB_KEY_SEARCH(info->db_ref, rx_key, rx_entry);
        if (!rx_entry) {
            return BCM_E_NOT_FOUND;
        }
        rx_idx = dst_entry->sp_info[i].rx_stk_idx;
        if (!((rx_idx >= 0) && (rx_idx < CPUDB_CXN_MAX))) {
            return BCM_E_NOT_FOUND;
        }
        rv = _bcm_board_safc_collect_modid(info, src_spidx, rx_entry, rx_idx);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* mark modids found in this entry as being reachable via
       the base port related to src_spidx */

    for (i=0; i<dst_entry->base.num_units; i++) {
        int j;

        for (j=0; j<dst_entry->base.mod_ids_req[i]; j++) {
            info->s_idx[dst_entry->mod_ids[i] + j] = info->t_idx[src_spidx];
        }
    }
    return BCM_E_NONE;
}

/* return true if the stack ports a and b are sources and sunk to
   identical units
 */
STATIC int
_bcm_board_safc_same_link(cpudb_ref_t db_ref, int a, int b)
{
    cpudb_entry_t       *entry, *tx_entry;
    cpudb_stk_port_t    *sp_a, *sp_b;
    cpudb_unit_port_t   *txup;
    int                 tx_a, tx_b;

    entry = db_ref->local_entry;
    sp_a = entry->sp_info + a;
    sp_b = entry->sp_info + b;
    tx_a = sp_a->tx_stk_idx;
    tx_b = sp_b->tx_stk_idx;
    tx_entry = NULL;

    if (entry->base.stk_ports[a].unit != entry->base.stk_ports[b].unit) {
        /* not on the same local unit */
        return FALSE;
    }

    if ((tx_a < 0) || (tx_a >= CPUDB_CXN_MAX)) {
        /* index out of range */
        return FALSE;
    }

    if ((tx_b < 0) || (tx_b >= CPUDB_CXN_MAX)) {
        /* index out of range */
        return FALSE;
    }

    if (!CPUDB_KEY_EQUAL(sp_a->tx_cpu_key, sp_b->tx_cpu_key)) {
        /* not connected to the same remote system */
        return FALSE;
    }

    CPUDB_KEY_SEARCH(db_ref, sp_a->tx_cpu_key, tx_entry);
    if (tx_entry == NULL) {
        /* can't even find the key */
        return FALSE;
    }

    txup = tx_entry->base.stk_ports;

    if (txup[tx_a].unit != txup[tx_b].unit) {
        /* units on the far end are different */
        return FALSE;
    }

    return TRUE;
}

/* determin stack ports that may be a member of a trunk group */
STATIC int
_bcm_board_safc_trunk_idx_add(safc_data_t *info, int idx)
{
    int i, unit, port;
    cpudb_entry_t    *entry;

    if (SHR_BITGET(info->trunked, idx)) {
        /* already added */
        return BCM_E_NONE;
    }

    BCM_PBMP_CLEAR(info->trunk[idx]);

    /* check if this stack port idx is associated with other stack
       ports connected to the same remote system and same unit. If so,
       assume it is a member of a trunk and add it to the given stack
       set. Do not assume that trunks are formed at this point, so do
       not use the trunk API to make this determination. */

    entry = info->db_ref->local_entry;

    /*
      get TX key and remote unit of this port
    */

    for (i = 0; i < entry->base.num_stk_ports; i++) {

        if ((entry->sp_info[i].flags &
             (CPUDB_SPF_ETHERNET|CPUDB_SPF_NO_LINK|CPUDB_SPF_INACTIVE))) {
            continue;
        }

        if (idx == i) {
            /* looking at this port, so skip */
            continue;
        }

        /*
          get TX key and remote unit of this port
        */
        if (_bcm_board_safc_same_link(info->db_ref, idx, i)) {
            unit = entry->base.stk_ports[i].unit;
            port = entry->base.stk_ports[i].port;
            LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                        (BSL_META("Trunk related spidx %d (%d,%d)\n"),
                         i, unit, port));
            SHR_BITSET(info->trunked, i);
            BCM_PBMP_PORT_ADD(info->trunk[idx], port);
            if (info->t_idx[i] >= 0) {
                return BCM_E_INTERNAL;
            }
            info->t_idx[i] = idx;
        }
    }

    unit = entry->base.stk_ports[idx].unit;
    port = entry->base.stk_ports[idx].port;
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("Trunk base spidx %d (%d,%d)\n"),
                 idx, unit, port));
    BCM_PBMP_PORT_ADD(info->trunk[idx], port);
    SHR_BITSET(info->trunked, idx);
    if (info->t_idx[i] >= 0) {
        return BCM_E_INTERNAL;
    }
    info->t_idx[idx] = idx;

    return BCM_E_NONE;
}



/* add stack port idx and any related ports to the stack set  */
STATIC int
_bcm_board_safc_stack_idx_add(safc_data_t *info, stkset_t stkset, int idx)
{
    int i, related;

    if (SHR_BITGET(stkset, idx)) {
        /* already added */
        return BCM_E_NONE;
    }

    SHR_BITSET(stkset, idx);

    /* now see if there are any related ports that need to be added */

    /* stack port index of the 'base' stack port */
    related = info->t_idx[idx];
    for (i = 0; i < CPUDB_CXN_MAX; i++) {
        /* if this stack port index is related, then add it in */
        if (info->t_idx[i] == related) {
            SHR_BITSET(stkset, i);
        }
    }

    return BCM_E_NONE;
}

/* SAFC data initializer */
STATIC int
_bcm_board_safc_data_set(safc_data_t *info,
                topo_cpu_t *tp_cpu,
                cpudb_ref_t db_ref,
                topo_info_t *safc)
{
    int              num_cpu;
    topo_info_t      *normal;
    int              src;
    int              i, j, m, dst, idx, rx_idx;
    cpudb_entry_t    *entry;
    int              num_modids;
    bcm_port_t       port;
    int              unit;
    int              nhmod, nhport;
    cpudb_entry_t    *rx_entry;
    cpudb_key_t      rx_key;   /* RX key, unit on master port to .. */
    int              rx_unit;  /* .. compare against */
    cpudb_key_t      pkey;     /* RX key, unit on other ports being ..*/
    int              punit;    /* .. compare to master port */
    topo_stk_port_t  *tsp;
    int              rv;

    if(NULL == db_ref)
    {
        return BCM_E_PARAM;
    }

    num_cpu = db_ref->num_cpus;
    normal = (topo_info_t *)db_ref->topo_cookie;
    src = db_ref->local_entry->topo_idx;
    nhmod = -1;
    entry = db_ref->local_entry;
    info->tp_cpu = tp_cpu;
    info->db_ref = db_ref;
    info->safc_topo = safc;

    BCM_PBMP_CLEAR(info->none_pbm_mask);
    BCM_PBMP_NEGATE(info->all_pbm_mask, info->none_pbm_mask);

    /* init t_idx to catch errors */
    for (i = 0; i < CPUDB_CXN_MAX; i++) {
        info->t_idx[i] = -1;
    }

    /* collect information about stack ports that may be trunked */
    for (i = 0; i < entry->base.num_stk_ports; i++) {

        if ((entry->sp_info[i].flags &
             (CPUDB_SPF_ETHERNET|CPUDB_SPF_NO_LINK|CPUDB_SPF_INACTIVE))) {
            continue;
        }
        rv = _bcm_board_safc_trunk_idx_add(info, i);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set which stack ports are in use based on the topology data */
    for (dst = 0; dst < num_cpu; dst++ ) {
        if (src != dst) {
            idx = TX_CXN(normal, src, dst);
            if (idx != TOPO_CXN_UNKNOWN) {
                rv = _bcm_board_safc_stack_idx_add(info, info->normal, idx);
                BCM_IF_ERROR_RETURN(rv);
            }
            idx = TX_CXN(safc, src, dst);
            if (idx != TOPO_CXN_UNKNOWN) {
                rv = _bcm_board_safc_stack_idx_add(info, info->safc, idx);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    }

    /* mixed system or safc only? */
    if (shr_bitop_range_eq(info->normal, info->safc, 0, CPUDB_CXN_MAX)) {
        info->safc_only = TRUE;
    }

    /* init portidx */
    for (i = 0; i < TOPO_MODIDS_MAX; i++) {
        info->n_idx[i] = -1;
        info->s_idx[i] = -1;
    }

    /* logic from bcm_board_module_filter */
    num_modids = 0;
    for (i = 0; i < entry->base.num_units; i++) {
        num_modids += entry->base.mod_ids_req[i];
    }

    for (i = 0; i < entry->base.num_stk_ports; i++) {

        if ((entry->sp_info[i].flags &
             (CPUDB_SPF_ETHERNET|CPUDB_SPF_NO_LINK|CPUDB_SPF_INACTIVE))) {
            continue;
        }

        port = entry->base.stk_ports[i].port;
        unit = entry->base.stk_ports[i].unit;

        /* Every active port should be a member of the SAFC port set
           or the normal port set. If it is not, then there is some
           logic error. */
        if (SHR_BITGET(info->safc, i)) {
            BCM_PBMP_PORT_ADD(info->safc_pbm[unit], port);
        } else if (SHR_BITGET(info->normal, i)) {
            BCM_PBMP_PORT_ADD(info->normal_pbm[unit], port);
        } else {
            return BCM_E_INTERNAL;
        }

        SHR_BITSET(info->units, unit);
        /* If we're using a reserved NH modid, or there's no
           nhmod defined for this stack port, or there was a nhmod
           for this stack port but it was was less than zero, then get the
           default */

        if (bcm_st_reserved_modid_enable_get()) {
            nh_tx_src_mod_port_get(unit, port, &nhmod, &nhport);
        }

        if (nhmod >= 0 && nhmod < num_modids) {
            /* add nexthop module */
            SHR_BITSET(info->modid[i], nhmod);
            SHR_BITSET(info->all_modid, nhmod);
        }

        /*
         * Get the remote RX CPU key and unit for the master port
         */
        CPUDB_KEY_COPY(rx_key, entry->sp_info[i].rx_cpu_key);
        rx_entry = NULL;
        rx_unit = -1;
        rx_idx  = -1;
        CPUDB_KEY_SEARCH(db_ref, rx_key, rx_entry);
        if (rx_entry) {
            rx_idx = entry->sp_info[i].rx_stk_idx;
            if ((rx_idx >= 0) && (rx_idx < CPUDB_CXN_MAX)) {
                rx_unit = rx_entry->base.stk_ports[rx_idx].unit;
            } else {
                return BCM_E_NOT_FOUND;
            }
        } else {
            return BCM_E_NOT_FOUND;
        }

        /*
         * Allow only mod IDs of those stack ports with same RX CPU key
         * and same remote stack-port unit.
         *
         * Note: If stack-port information for remote CPU entries
         *       is not available, then check for CPU key only
         *       (old behavior).
         */
        for (j = 0; j < entry->base.num_stk_ports; j++) {
            if (entry->base.stk_ports[j].unit != unit) {
                continue;
            }
            if (!(entry->sp_info[j].flags & CPUDB_SPF_RX_RESOLVED)) {
                continue;
            }

            /*
             * Get other stack port RX CPU key and unit to
             * compare against master port
             */
            CPUDB_KEY_COPY(pkey, entry->sp_info[j].rx_cpu_key);
            punit = -1;
            if (rx_unit >= 0) {
                idx = entry->sp_info[j].rx_stk_idx;
                if ((idx >= 0) && (idx < CPUDB_CXN_MAX) && rx_entry) {
                    punit = rx_entry->base.stk_ports[idx].unit;
                }
            }

            /* Skip port if RX CPU key or unit are different */
            if (!CPUDB_KEY_EQUAL(rx_key, pkey) || (rx_unit != punit)) {
                continue;
            }

            /* Add modules that are rxable on this stack port.
               This is for normal topology.
             */
            tsp = &tp_cpu->tp_stk_port[j];
            for (m = 0; m < tsp->rx_mod_num; m++) {
                int modid = tsp->rx_mods[m];
                /* modid on this port */
                SHR_BITSET(info->modid[i], modid);
                /* all modids */
                SHR_BITSET(info->all_modid, modid);
                /* modid to base stack port index */
                info->n_idx[modid] = info->t_idx[i];
            }
        }
        /* Now if this is not a cut port, that means this port will be
           used for SAFC traffic. Gather all the modids reachable in
           the SAFC topology. */
        if (!(entry->sp_info[i].flags & (CPUDB_SPF_CUT_PORT))) {
            sal_memset(info->s_visit, 0, sizeof(stkset_t));
            SHR_BITSET(info->s_visit, entry->topo_idx);
            rv = _bcm_board_safc_collect_modid(info, i, rx_entry, rx_idx);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    /* set blocked modid map. This is a list of modids for each *normal*
       stack port that is not expected to be received on that port. */
    for (i = 0; i < entry->base.num_stk_ports; i++) {
        for (j = 0; j < TOPO_MODIDS_MAX; j++) {
            if (SHR_BITGET(info->all_modid, j) &&
                (!SHR_BITGET(info->modid[i], j))) {
                /* valid modid not on this port */
                SHR_BITSET(info->blocked[i], j);
            }
        }
    }

    rv = _bcm_board_safc_unit_iterate(info, _bcm_board_safc_data_unit_init);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/* SAFC data allocator and initializer */
STATIC int
_bcm_board_safc_data(safc_data_t **info_p,
                     topo_cpu_t *tp_cpu, cpudb_ref_t db_ref, topo_info_t *safc)
{
    safc_data_t *info;
    int rv;

    info = sal_alloc(sizeof(*info), "info");
    if (info) {
        sal_memset(info, 0, sizeof(*info));
        rv = _bcm_board_safc_data_set(info, tp_cpu, db_ref, safc);
        if (BCM_FAILURE(rv)) {
            sal_free(info);
            info = NULL;
        }
    } else {
        rv = BCM_E_MEMORY;
    }
    if (info) {
        *info_p = info;
    }
    return rv;
}

STATIC int
_bcm_board_safc_only_higig_u_cos(safc_data_t *info, int unit, bcm_cos_t cos)
{
    bcm_field_entry_t e1;
    int rv;

    rv = bcm_field_entry_create(unit, info->group[unit], &e1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_InPorts(unit, e1,
                                   info->safc_pbm[unit],
                                   info->all_pbm_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_OuterVlan(unit, e1,
                                     BCM_VLAN_CTRL(cos, 0, 0),
                                     info->safc_vlan_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_count_create(unit, info->group[unit], e1,
                                      FUNCTION_NAME());
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_entry_install(unit, e1);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/* SAFC-only Higig unit programming */
STATIC int
_bcm_board_safc_only_higig_u(safc_data_t *info, int unit)
{
    return _bcm_board_safc_cos_iterate(info, unit,
                                       _bcm_board_safc_only_higig_u_cos);
}

STATIC int
_bcm_board_safc_drop_ingress_u_cos(safc_data_t *info, int unit, bcm_cos_t cos)
{
    bcm_field_entry_t e1;
    int rv;

    /* Drop SAFC traffic ingressing on normal HG ports */
    rv = bcm_field_entry_create(unit, info->group[unit], &e1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_InPorts(unit, e1,
                                   info->normal_pbm[unit], info->all_pbm_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_OuterVlan(unit, e1,
                                     BCM_VLAN_CTRL(cos, 0, 0),
                                     info->safc_vlan_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_action_add(unit, e1, bcmFieldActionDrop, 0, 0);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_count_create(unit, info->group[unit], e1,
                                      FUNCTION_NAME());
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_entry_install(unit, e1);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/* Mixed unit programming */
STATIC int
_bcm_board_safc_drop_ingress_u(safc_data_t *info, int unit)
{
    return _bcm_board_safc_cos_iterate(info, unit,
                                       _bcm_board_safc_drop_ingress_u_cos);
}

STATIC int
_bcm_board_safc_block_egress_u_cos(safc_data_t *info, int unit, bcm_cos_t cos)
{
    bcm_field_entry_t e1;
    int rv;

    /* Block SAFC traffic from egressing normal HG ports */

    rv = bcm_field_entry_create(unit, info->group[unit], &e1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_InPorts(unit, e1,
                                   info->safc_pbm[unit], info->all_pbm_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_OuterVlan(unit, e1,
                                     BCM_VLAN_CTRL(cos, 0, 0),
                                     info->safc_vlan_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_action_ports_add(unit, e1,
                                    bcmFieldActionEgressMask,
                                    info->normal_pbm[unit]);
    BCM_IF_ERROR_RETURN(rv);

    rv =  _bcm_board_safc_count_create(unit, info->group[unit],
                                      e1, FUNCTION_NAME());
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_entry_install(unit, e1);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

STATIC int
_bcm_board_safc_block_egress_u(safc_data_t *info, int unit)
{
    return _bcm_board_safc_cos_iterate(info, unit,
                                       _bcm_board_safc_block_egress_u_cos);
}

STATIC int
_bcm_board_mixed_higig_u(safc_data_t *info, int unit)
{
    int rv;

    rv = _bcm_board_safc_drop_ingress_u(info, unit);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_block_egress_u(info, unit);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/* Per-port, per modid, UNDO src modid blocking */

STATIC int
_bcm_board_safc_undo_source_modid_block_p(safc_data_t *info, int spidx)
{
    int m, unit, port;
    int rv;

    unit = info->db_ref->local_entry->base.stk_ports[spidx].unit;
    port = info->db_ref->local_entry->base.stk_ports[spidx].port;

    /* No ports on this unit may be actively participating in
       any topology. If so, then skip. */
    if (!SHR_BITGET(info->units, unit)) {
        return BCM_E_NONE;
    }

    for (m=0; m < TOPO_MODIDS_MAX; m++) {
        if (SHR_BITGET(info->blocked[spidx], m)) {
            /* this modid should be blocked */

            /* undo device table block */
            rv = bcm_port_modid_enable_set(unit, port, m, TRUE);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}



/* SAFC/normal unicast forwarding */

STATIC int
_bcm_board_safc_unicast_forward_cos(safc_data_t *info,
                                    int unit, int port, int modid, int dest,
                                    bcm_pbmp_t egress_mask, bcm_cos_t cos)
{
    bcm_field_entry_t e1;
    int rv;

    rv = bcm_field_entry_create(unit, info->group[unit], &e1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_InPorts(unit, e1,
                                   info->port_config[unit].e,
                                   info->all_pbm_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_L2DestHit(unit, e1, 1, 1);
    BCM_IF_ERROR_RETURN(rv);

    if (dest) {
        rv = bcm_field_qualify_OuterVlan(unit, e1,
                                         BCM_VLAN_CTRL(cos, 0, 0),
                                         info->safc_vlan_mask);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = bcm_field_qualify_DstPort(unit, e1, modid,
                                    BCM_FIELD_EXACT_MATCH_MASK, 0, 0);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_action_ports_add(unit, e1,
                                    bcmFieldActionEgressMask, egress_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_count_create(unit, info->group[unit], e1,
                                      FUNCTION_NAME());
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_entry_install(unit, e1);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;

}

/* per-unit, per-dest-type unicast forwarding

   dest=1 SAFC
   dest=0 normal

   Standard board programming should already add the normal topology
   port to the modport map, so this function just adds the SAFC port.
*/

#define TRUNK(n) (((n)>1) ? "trunk " : "")

/* Check if a stack unit has multi stack ports mapped to the same modid. */
STATIC int
_bcm_board_safc_multi_modmap(safc_data_t *info, int unit, int *multi_map)
{
    int modid;
    int rv, i, j;
    int port_max = 16, port_count = 0;
    bcm_port_t port_array[16];

    for (modid = 0; modid < TOPO_MODIDS_MAX; modid++) {
        if (SHR_BITGET(info->all_modid, modid)) {
            rv = bcm_stk_modport_get_all(unit, modid, port_max,
                                         port_array, &port_count);
            BCM_IF_ERROR_RETURN(rv);
            if (port_count > 1) {
                for (i = 0; i < (port_count - 1); i++) {
                    for (j = (i + 1); j < port_count; j++) {
                        if (port_array[i] != port_array[j]) {
                            *multi_map = TRUE;
                            return BCM_E_NONE;
                        }
                    }
                }
            }
        }
    }

    *multi_map = FALSE;
    return BCM_E_NONE;
}

STATIC int
_bcm_board_safc_unicast_forward_port(safc_data_t *info,
                                     int unit, int modid, int dest)
{
    int spidx, otidx;
    int spunit, otunit;
    int s_port, num_port;
    bcm_pbmp_t s_pbmp, o_pbmp;
    bcm_pbmp_t egress_mask;
    int rv;
    bcm_cos_t cos;

    /* get stack port index appropriate for destination */
    spidx = dest ? info->s_idx[modid] : info->n_idx[modid];
    if (spidx < 0) {
        return BCM_E_NONE;
    }
    spunit = info->db_ref->local_entry->base.stk_ports[spidx].unit;

    BCM_PBMP_ASSIGN(s_pbmp, info->trunk[spidx]);
    if (BCM_PBMP_IS_NULL(s_pbmp)) {
        return BCM_E_INTERNAL;
    }

    /* get the other frame format stack port index */
    otidx = dest ? info->n_idx[modid] : info->s_idx[modid];
    if (otidx < 0) {
        return BCM_E_NONE;
    }
    otunit = info->db_ref->local_entry->base.stk_ports[otidx].unit;

    BCM_PBMP_ASSIGN(o_pbmp, info->trunk[otidx]);
    if (BCM_PBMP_IS_NULL(o_pbmp)) {
        return BCM_E_INTERNAL;
    }

    if (unit != spunit && unit != otunit) {
        /* modid is not forwarded off this unit, so skip */
        return BCM_E_NONE;
    }


    /* set s_port to first port */
    BCM_PBMP_ITER(s_pbmp, s_port) {
        break;
    }

    BCM_PBMP_COUNT(s_pbmp, num_port);

    /* mask out all ports for egress except for the ports appropriate
       for this packet type. */
    BCM_PBMP_CLEAR(egress_mask);
    if (BCM_PBMP_NEQ(o_pbmp, s_pbmp)) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META_U(unit,
                    "%s: masking for modid %d egress to %s%d\n"),
                     FUNCTION_NAME(), modid, TRUNK(num_port), s_port));
        BCM_PBMP_ASSIGN(egress_mask, o_pbmp);
    } else {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META_U(unit,
                    "%s: no mask for modid %d egress to %s%d\n"),
                     FUNCTION_NAME(), modid, TRUNK(num_port), s_port));
    }

    if (dest) {
        /* if SAFC, add the first port to modport */
        rv = bcm_stk_modport_add(unit, modid, s_port);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* forwarding rules for each port */
    BCM_PBMP_ITER(s_pbmp, s_port) {
        for (cos = BCM_COS_MAX; cos >= BCM_COS_MIN; cos--) {

            /* check each COS if SAFC dest */
            if ((dest != 0) && ((info->safc_cos_map & (1<<cos)) == 0)) {
                continue;
            }
            rv = _bcm_board_safc_unicast_forward_cos(info, spunit, s_port,
                                                     modid, dest,
                                                     egress_mask, cos);
            BCM_IF_ERROR_RETURN(rv);

            /* COS ignored if !SAFC, so quit */
            if (dest == 0) {
                break;
            }
        }
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_board_safc_unicast_ingress_lag(safc_data_t *info, int unit)
{
    bcm_field_entry_t e1;
    bcm_pbmp_t egress_mask;
    int s_port, num_port;
    int rv, multi_map;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META_U(unit,
                "%s(<>,%d)\n"),
                 FUNCTION_NAME(), unit));

    rv = bcm_field_entry_create(unit, info->group[unit], &e1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_L2DestHit(unit, e1, 1, 1);
    BCM_IF_ERROR_RETURN(rv);

    /* It just needs to be a trunk; it doesn't matter what the trunk ID is */
    rv = bcm_field_qualify_DstTrunk(unit, e1, 0, 0);
    BCM_IF_ERROR_RETURN(rv);

    BCM_PBMP_COUNT(info->normal_pbm[unit], num_port);
    if (num_port != 0) {
        rv = bcm_field_action_ports_add(unit, e1, bcmFieldActionEgressMask,
                                        info->normal_pbm[unit]);
    } else {
        BCM_PBMP_CLEAR(egress_mask);
        BCM_PBMP_COUNT(info->safc_pbm[unit], num_port);
        rv = _bcm_board_safc_multi_modmap(info, unit, &multi_map);
        BCM_IF_ERROR_RETURN(rv);

        if ((num_port > 1) && multi_map) {
            /* Simply pick a port from the safc_pbm and mask out egress */
            BCM_PBMP_ITER(info->safc_pbm[unit], s_port) {
                break;
            }
            /* coverity[overrun-local] */
            BCM_PBMP_PORT_ADD(egress_mask, s_port);
        }
        rv = bcm_field_action_ports_add(unit, e1, bcmFieldActionEgressMask,
                                        egress_mask);
    }
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_count_create(unit, info->group[unit], e1,
                                      FUNCTION_NAME());
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_entry_install(unit, e1);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

STATIC int
_bcm_board_safc_unicast_forwarding_u_d(safc_data_t *info, int unit, int dest)
{
    int modid;
    int rv;

    if (dest) {
        /* LAG handling */
        rv = _bcm_board_safc_unicast_ingress_lag(info, unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    for (modid = 0; modid < TOPO_MODIDS_MAX; modid++) {
        if (SHR_BITGET(info->all_modid, modid)) {
            rv = _bcm_board_safc_unicast_forward_port(info, unit,
                                                      modid, dest);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/* per-unit SAFC unicast forwarding */
STATIC int
_bcm_board_safc_unicast_forwarding_u(safc_data_t *info, int unit)
{
    return _bcm_board_safc_unicast_forwarding_u_d(info, unit, 1);
}

/* per-unit Normal unicast forwarding */
STATIC int
_bcm_board_normal_unicast_forwarding_u(safc_data_t *info, int unit)
{
    return _bcm_board_safc_unicast_forwarding_u_d(info, unit, 0);
}

/* per-unit non-unicast egress blocking from Ethernet ports */
STATIC int
_bcm_board_safc_nonunicast_egress_blocking_u_cos(safc_data_t *info,
                                                 int unit, bcm_cos_t cos)
{
    bcm_field_entry_t e1;
    int rv;

    rv = bcm_field_entry_create(unit, info->group[unit], &e1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_InPorts(unit, e1, info->port_config[unit].e,
                                   info->all_pbm_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_L2DestHit(unit, e1, 0, 1);
    BCM_IF_ERROR_RETURN(rv);

    /* Block all L2 misses instead of source modid blocking. */
    rv = bcm_field_action_ports_add(unit, e1, bcmFieldActionEgressMask,
                                    info->normal_pbm[unit]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_count_create(unit, info->group[unit], e1,
                                      FUNCTION_NAME());
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_entry_install(unit, e1);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/* per-unit MH header egress blocking from SAFC Higig ports */
STATIC int
_bcm_board_safc_hg_op_egress_blocking_u(safc_data_t *info, int unit, int op)
{
    bcm_field_entry_t e1;
    int rv;

    rv = bcm_field_entry_create(unit, info->group[unit], &e1);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_InPorts(unit, e1,
                                   info->safc_pbm[unit],
                                   info->all_pbm_mask);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_qualify_MHOpcode(unit, e1, op, 0x7);
    BCM_IF_ERROR_RETURN(rv);

    /* Block all L2 misses instead of source modid blocking. */
    rv = bcm_field_action_ports_add(unit, e1, bcmFieldActionEgressMask,
                                    info->normal_pbm[unit]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_count_create(unit, info->group[unit], e1,
                                      FUNCTION_NAME());
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_field_entry_install(unit, e1);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}


STATIC int
_bcm_board_safc_nonunicast_egress_blocking_u(safc_data_t *info, int unit)
{
    int rv;

    rv = _bcm_board_safc_cos_iterate(info, unit,
             _bcm_board_safc_nonunicast_egress_blocking_u_cos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_hg_op_egress_blocking_u(info, unit,
             BCM_FIELD_MHOPCODE_BROADCAST_DLF);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_hg_op_egress_blocking_u(info, unit,
             BCM_FIELD_MHOPCODE_MULTICAST);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_board_safc_hg_op_egress_blocking_u(info, unit,
             BCM_FIELD_MHOPCODE_IPMULTICAST);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
  System level programming using iterators and per-unit or per-port
  programming functions.
*/

/* SAFC only higig programming */
STATIC int
_bcm_board_safc_only_higig(safc_data_t *info)
{
    int rv;

    rv =_bcm_board_safc_unit_iterate(info, _bcm_board_safc_only_higig_u);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("%s()=%d\n"),
                 FUNCTION_NAME(), rv));

    return rv;
}

/* Mixed SAFC/normal higig programming */
STATIC int
_bcm_board_mixed_higig(safc_data_t *info)
{
    int rv;

    rv = _bcm_board_safc_unit_iterate(info, _bcm_board_mixed_higig_u);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("%s()=%d\n"),
                 FUNCTION_NAME(), rv));

    return rv;
}

/* UNDO Source modid blocking */
STATIC int
_bcm_board_safc_undo_source_modid_block(safc_data_t *info)
{
    int rv;

    rv = _bcm_board_safc_stkport_iterate
        (info, _bcm_board_safc_undo_source_modid_block_p);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("%s()=%d\n"),
                 FUNCTION_NAME(), rv));

    return rv;
}

/* SAFC unicast forwarding */
STATIC int
_bcm_board_safc_unicast_forwarding(safc_data_t *info)
{
    int rv;

    rv = _bcm_board_safc_unit_iterate
        (info, _bcm_board_safc_unicast_forwarding_u);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("%s()=%d\n"),
                 FUNCTION_NAME(), rv));

    return rv;
}

/* Normal unicast forwarding */
STATIC int
_bcm_board_normal_unicast_forwarding(safc_data_t *info)
{
    int rv;

    rv = _bcm_board_safc_unit_iterate
        (info, _bcm_board_normal_unicast_forwarding_u);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("%s()=%d\n"),
                 FUNCTION_NAME(), rv));

    return rv;
}

/* non-unicast egress blocking */
STATIC int
_bcm_board_safc_nonunicast_egress_blocking(safc_data_t *info)
{
    int rv;

    rv = _bcm_board_safc_unit_iterate
        (info, _bcm_board_safc_nonunicast_egress_blocking_u);
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("%s()=%d\n"),
                 FUNCTION_NAME(), rv));

    return rv;
}

/* field programming group initialization */
STATIC int
_bcm_board_safc_unit_init(safc_data_t *info, int unit)
{
    info->group[unit] = -1;

    return
        bcm_field_group_create(unit,
                               info->qset,
                               info->group_priority, info->group + unit);
}


/* field programming initialization */
STATIC int
_bcm_board_safc_init(safc_data_t *info,
                     uint32 safc_cos_map,
                     int group_priority)
{
    int rv;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("%s\n"),
                 FUNCTION_NAME()));

    info->safc_cos_map = safc_cos_map;
    info->safc_vlan_mask = BCM_VLAN_CTRL(BCM_COS_MAX,0,0);
    info->group_priority = group_priority;

    BCM_FIELD_QSET_INIT(info->qset);
    BCM_FIELD_QSET_ADD(info->qset,bcmFieldQualifyInPorts);
    BCM_FIELD_QSET_ADD(info->qset,bcmFieldQualifyDstTrunk);
    BCM_FIELD_QSET_ADD(info->qset,bcmFieldQualifyDstPort);
    BCM_FIELD_QSET_ADD(info->qset,bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(info->qset,bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(info->qset,bcmFieldQualifyL2DestHit);
    BCM_FIELD_QSET_ADD(info->qset,bcmFieldQualifyMHOpcode);

    rv = _bcm_board_safc_unit_iterate(info, _bcm_board_safc_unit_init);

    return rv;
}

/* SAFC configuration */
STATIC int
_bcm_board_safc_configure(topo_cpu_t *tp_cpu,
                          cpudb_ref_t db_ref,
                          uint32 safc_cos_map,
                          int group_priority,
                          topo_info_t *safc)
{
    int rv;
    safc_data_t *info;

    rv = BCM_E_PARAM;
    info = NULL;

    if ((safc_cos_map & ~((1<<BCM_COS_COUNT)-1)) != 0) {
        /* should be no bits set from bit BCM_COS_COUNT and higher */
        goto done;
    }

    rv = _bcm_board_safc_data(&info, tp_cpu, db_ref, safc);
    if (BCM_FAILURE(rv)) {
        goto done;
    }

    if (!info) {
        rv = BCM_E_MEMORY;
        goto done;
    }

    rv = _bcm_board_safc_init(info, safc_cos_map, group_priority);
    if (BCM_FAILURE(rv)) {
        goto done;
    }

    if (info->safc_only) {
        rv = _bcm_board_safc_only_higig(info);
        if (BCM_FAILURE(rv)) {
            goto done;
        }
    } else {
        rv = _bcm_board_mixed_higig(info);
        if (BCM_FAILURE(rv)) {
            goto done;
        }
    }

    rv = _bcm_board_safc_unicast_forwarding(info);
    if (BCM_FAILURE(rv)) {
        goto done;
    }

    rv = _bcm_board_normal_unicast_forwarding(info);
    if (BCM_FAILURE(rv)) {
        goto done;
    }

    if (!info->safc_only) {
        rv = _bcm_board_safc_nonunicast_egress_blocking(info);
        if (BCM_FAILURE(rv)) {
            goto done;
        }
    }

    rv = _bcm_board_safc_undo_source_modid_block(info);
    if (BCM_FAILURE(rv)) {
        goto done;
    }

 done:
    if (info) {
        sal_free(info);
    }

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("%s=%d\n"),
                 FUNCTION_NAME(), rv));
    return rv;
}

/*
 * Function:
 * int bcm_board_safc_configure(topo_cpu_t *tp_cpu,
 *                              cpudb_ref_t db_ref,
 *                              uint32 safc_cos_map,
 *                              int group_priority,
 *                              topo_info_t *connection)
 * Purpose:
 *     Program SAFC forwarding based on provided connection info
 * Parameters:
 *     tp_cpu         - (IN)  CPU topology information
 *     db_ref         - (IN)  CPU database
 *     safc_cos_map   - (IN)  SAFC COS bitmap
 *     group_priority - (IN)  field group priority
 *     connection     - (IN)  Connection matrix
 * Returns:
 *     BCM_E_NONE     - no errors
 *     BCM_E_FAIL     - topology calculation failed (CPUDB incomplete)
 *     BCM_E_RESOURCE - not enough programming resources
 * Notes:
 *
 *     Performs device programming. Assumes any necessary non-SAFC
 *     programming is complete.
 *
 *     Connection matrix is from bcm_stack_spanning_tree (for SAFC)
 *     *and* db_ref (for non SAFC).
 *
 *     This function will only use SDK APIs. Any register manipulation
 *     required by the application must be managed outside this
 *     function.
 *
 *     Group priority must be higher than any application or system
 *     field group priority, and must be unique for each different
 *     SAFC topology.
 *
 *     SAFC is unconfigured when connection is NULL. In this case,
 *     group_priority is the only parameter that must be valid; tp_cpu,
 *     db_ref, and safc_cos_map is ignored.
 */

int
bcm_board_safc_configure(topo_cpu_t *tp_cpu,
                         cpudb_ref_t db_ref,
                         uint32 safc_cos_map,
                         int group_priority,
                         topo_info_t *connection)
{
    int rv;

    rv = BCM_E_PARAM;

    if (cpudb_valid(db_ref) && tp_cpu && connection) {
        (void) _bcm_board_safc_unconfigure(group_priority);
        rv = _bcm_board_safc_configure(tp_cpu, db_ref,
                                       safc_cos_map, group_priority,
                                       connection);
        if (BCM_FAILURE(rv)) {
            (void) _bcm_board_safc_unconfigure(group_priority);
        }
    } else if (connection == NULL) {
        rv = _bcm_board_safc_unconfigure(group_priority);
    }

    return rv;
}


