/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        topology.c
 * Purpose:     Sample topology analysis implementation
 * Requires:
 * Notes:
 *    Although this is set up to generate data on a db_ref object,
 *    this code is not re-entrant and care should be taken to ensure
 *    that topology create is not called multiple times simultaneously.
 */

#include <shared/bsl.h>

#include <bcm/error.h>

#include <appl/stktask/topology.h>
#include <appl/stktask/topo_brd.h>
#include <appl/stktask/stktask.h>
#include <appl/cpudb/cpudb.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>

#include "topo_int.h"

typedef uint32 weight_t;

/* Synchronize access to topo structures */
#define TOPO_LOCK_INIT if (topo_lock == NULL) topo_lock_init()
#define TOPO_LOCK_CHECK if (topo_lock == NULL) return BCM_E_MEMORY
#define TOPO_LOCK sal_mutex_take(topo_lock, sal_sem_FOREVER)
#define TOPO_UNLOCK sal_mutex_give(topo_lock)

STATIC void _topology_destroy(cpudb_ref_t db_ref);

static sal_mutex_t topo_lock;
static uint32 topo_rsvd_modid = 0;
static topology_mod_id_assign_f topology_mod_id_assign_func = NULL;

STATIC void
topo_lock_init(void)
{
    if (topo_lock == NULL) {
        topo_lock = sal_mutex_create("topo_lock");
    }
    if (topo_lock == NULL) {
        LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                  (BSL_META("TOPO: Could not create topology mutex\n")));
    }
}


/* Boolean:  Is connection matrix complete? */

STATIC int
tp_all_destinations_reachable(cpudb_ref_t db_ref)
{
    int i, j;

    for (i = 0; i < db_ref->num_cpus; i++) {
        for (j = 0; j < db_ref->num_cpus; j++) {
            if (i == j) {
                continue;
            }
            if (!TP_REACHABLE(db_ref, i, j)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/* Set up internal indexes for CPUs */

STATIC void
tp_index_init(cpudb_ref_t db_ref)
{
    cpudb_entry_t *entry;
    int entry_count = 0;

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        entry->topo_idx = entry_count++;
    }

#if defined(BROADCOM_DEBUG)
    if (entry_count != db_ref->num_cpus) {
        LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META("TOPO WARNING: Bad cpu count: db %d. local %d\n"),
                  db_ref->num_cpus, entry_count));
    }
#endif /* BROADCOM_DEBUG */
}

/*
 * Originally, did depth first search.  This has been replaced,
 * but the new code may be overridden by defining TOPO_NO_SHORTEST_PATH.
 * This will likely go away soon.
 */
#ifndef TOPO_NO_SHORTEST_PATH

/*
 * SHORTEST HOP TOPOLOGY
 *
 * 1.  Initialize weights for processing.  Assume the DB has TX/RX connections
 *     completed.  Set first found connection to immediate neighbors.  Fabric
 *     trunking takes care of redundant links.
 *
 * 2.  Iteratively process next possible hops until no more edges are added.
 */
#define TP_WEIGHT(_w, _s, _d)    (_w)[((_s)*CPUDB_CPU_MAX)+(_d)]

STATIC void
tp_weights_init(cpudb_ref_t db_ref, weight_t *weights)
{
    int s_idx, d_idx;
    cpudb_entry_t *src_entry, *dest_entry;
    int i;


    CPUDB_FOREACH_ENTRY(db_ref, src_entry) {
        s_idx = src_entry->topo_idx;
        for (i = 0; i < src_entry->base.num_stk_ports; i++) {
            cpudb_unit_port_t *sp_base = &src_entry->base.stk_ports[i];
            cpudb_stk_port_t *sp = &src_entry->sp_info[i];
            if (sp->flags == 0) {    /* no info on port */
                continue;
            }
            if (sp->flags & (
                             CPUDB_SPF_TX_DISABLE_FORCE | /* disabled */
                             CPUDB_SPF_NO_LINK |          /* no link on port */
                             CPUDB_SPF_INACTIVE |         /* no pkts on port */
                             CPUDB_SPF_ETHERNET )) {      /* Ethernet port */
                continue;
            }
            /* If cut ports are disabled, skip */
            if ((sp->flags & CPUDB_SPF_CUT_PORT) &&
                (sp_base->bflags & CPUDB_UPF_DISABLE_IF_CUT)) {
                /* This pops up with redundant SL stack links */
                LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                            (BSL_META("skipping cut ports with flag\n")));
                continue;
            }
            CPUDB_KEY_SEARCH(db_ref, sp->tx_cpu_key, dest_entry);
            if (dest_entry == NULL) {
                LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                         (BSL_META("TOPO WARNING: Could not find tx key "
                                   CPUDB_KEY_FMT_EOLN),
                          CPUDB_KEY_DISP(sp->tx_cpu_key)));
                continue;
            }
            d_idx = dest_entry->topo_idx;
            if (TP_REACHABLE(db_ref, s_idx, d_idx)) {
                /*
                 * Already have a connection indicated;
                 * fabric trunking handles this case
                 */
                continue;
            }

            TP_WEIGHT(weights, s_idx, d_idx) = TOPO_DEFAULT_WEIGHT;
            /* Below indicates is reachable and by what stk port */
            TP_TX_CXN(db_ref, s_idx, d_idx) = i;
            TP_RX_CXN(db_ref, d_idx, s_idx) = sp->tx_stk_idx;
            /* Check for duplex connection and set up if so. */
            if (sp->flags & CPUDB_SPF_DUPLEX) {
                if (!TP_REACHABLE(db_ref, d_idx, s_idx)) {
                    TP_WEIGHT(weights, d_idx, s_idx) = TOPO_DEFAULT_WEIGHT;
                    /* Below indicates is reachable and by what stk port */
                    TP_TX_CXN(db_ref, d_idx, s_idx) = sp->tx_stk_idx;
                    TP_RX_CXN(db_ref, s_idx, d_idx) = i;
                } else {
                    LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                             (BSL_META("TOPO WARNING: Duplex port already marked, "
                              "src (%d,%d), dest (%d,%d)\n"), s_idx, i,
                              d_idx, sp->tx_stk_idx));
                }
            }
        }
    }
}

/* Return TRUE if the cpudb is all full duplex */
STATIC int
tp_full_duplex(cpudb_ref_t db_ref)
{
    cpudb_entry_t *entry;
    int i;

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        for (i = 0; i < entry->base.num_stk_ports; i++) {
            cpudb_stk_port_t *sp = &entry->sp_info[i];
            if (sp->flags == 0) {    /* no info on port */
                continue;
            }
            if (sp->flags & (CPUDB_SPF_TX_DISABLE_FORCE | /* disabled */
                             CPUDB_SPF_NO_LINK |          /* no link on port */
                             CPUDB_SPF_INACTIVE |         /* no pkts on port */
                             CPUDB_SPF_ETHERNET )) {      /* Ethernet port */
                continue;
            }
            if (!(sp->flags & CPUDB_SPF_DUPLEX)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*
 * Run through src->mid connections; then mid->dest connections.  If
 * src->dest is not known, add it with weighted sum via src->mid->dest.
 *
 * Note that we don't actually need to look at weights since we're
 * doing a breadth first search and all initial weights are 1.  But
 * I'll leave it here in case there's a need for assigning different
 * weights to different (types of) links.
 *
 * If all the connections are full duplex, make the reverse path the
 * same as the forward path.
 */



STATIC void
tp_weights_process(cpudb_ref_t db_ref, weight_t *weights)
{
    int count;
    int src_idx, mid_idx, dest_idx;
    int full_duplex;

    full_duplex = tp_full_duplex(db_ref);

    do {
        count = 0;
        for (src_idx = 0; src_idx < db_ref->num_cpus; src_idx++) {
            for (mid_idx = 0; mid_idx < db_ref->num_cpus; mid_idx++) {
                if (mid_idx == src_idx ||
                        !TP_REACHABLE(db_ref, src_idx, mid_idx)) {
                    continue;
                }
                for (dest_idx = 0; dest_idx < db_ref->num_cpus; dest_idx++) {
                    if (dest_idx == src_idx || dest_idx == mid_idx) {
                        continue;
                    }
                    if (!TP_REACHABLE(db_ref, mid_idx, dest_idx)) {
                        continue;
                    }
                    if (!TP_REACHABLE(db_ref, src_idx, dest_idx) ||
                        (TP_WEIGHT(weights, src_idx, mid_idx) +
                         TP_WEIGHT(weights, mid_idx, dest_idx) <
                         TP_WEIGHT(weights, src_idx, dest_idx))) {
                        int tx,rx,w;
                        ++count;
                        TP_TX_CXN(db_ref, src_idx, dest_idx) =
                            (tx=TP_TX_CXN(db_ref, src_idx, mid_idx));
                        TP_RX_CXN(db_ref, dest_idx, src_idx) =
                            (rx=TP_RX_CXN(db_ref, dest_idx, mid_idx));
                        TP_WEIGHT(weights, src_idx, dest_idx) =
                            (w=(TP_WEIGHT(weights, src_idx, mid_idx) +
                                TP_WEIGHT(weights, mid_idx, dest_idx)));
                        if (full_duplex) {
                            TP_TX_CXN(db_ref, dest_idx, src_idx) = rx;
                            TP_RX_CXN(db_ref, src_idx, dest_idx) = tx;
                            TP_WEIGHT(weights, dest_idx, src_idx) = w;
                        }
                    }
                }
            }
        }
    } while (count > 0);
}


/*
 * find the paths from each cpu to every other cpu
 * (shortest path version)
 */
STATIC int
tp_find_paths(cpudb_ref_t db_ref)
{
    weight_t *weights;
    int      size;

    size = CPUDB_CPU_MAX * CPUDB_CPU_MAX * sizeof(*weights);
    weights = sal_alloc(size, "topo_weights");
    if (weights == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(weights, 0, size);
    tp_weights_init(db_ref, weights);
    tp_weights_process(db_ref, weights);
    sal_free(weights);
    return BCM_E_NONE;
}

#else  /* TOPO_NO_SHORTEST_PATH */

/*
 * Old, depth first search path processing.
 */

/*
 * The connection src->dest has just been added.  Look for
 * connections src->dest->new_dest where src->new_dest doesn't exist yet.
 *
 * Returns number of cxns added.
 *
 * The RX information is updated as well.  The RX port at new for (src->new)
 * is the same as the RX port at new for (dest->new).
 */

STATIC int
tp_one_cxn_update(cpudb_ref_t db_ref, int src_idx, int dest_idx)
{
    int new_dest;
    int cxns_added = 0;

    for (new_dest = 0; new_dest < db_ref->num_cpus; new_dest++) {
        if (new_dest == dest_idx || new_dest == src_idx) {
            continue;
        }
        if (TP_REACHABLE(db_ref, dest_idx, new_dest) &&
                !TP_REACHABLE(db_ref, src_idx, new_dest)) {
            TP_TX_CXN(db_ref, src_idx, new_dest) =
                TP_TX_CXN(db_ref, src_idx, dest_idx);
            TP_RX_CXN(db_ref, new_dest, src_idx) =
                TP_RX_CXN(db_ref, new_dest, dest_idx);
            cxns_added++;
        }
    }

    return cxns_added;
}


/*
 * Update the connection matrix after an edge has been added.
 *
 * Given a set of CPUs that have new destinations.  Look for new
 * CPUs that can reach anything in this set, and add any new edges
 * found.
 *
 * When a connection src->dest is added, we need to keep track of
 * src (gets placed on new_cpus).  Then, do a recursive call to
 * look for deeper connections.
 *
 * The worst case depth of this is CPUDB_CPU_MAX - 1.
 */

STATIC void
tp_cxns_update(cpudb_ref_t db_ref,
               uint8 *cpu_list,
               int cpu_count)
{
    uint8 new_cpus[CPUDB_CPU_MAX];
    int new_count = 0;
    uint8 src_idx;
    uint8 mid_cpu;
    int i;

#if defined(BROADCOM_DEBUG)
    static volatile int depth = 0;

    if (depth++ > CPUDB_CPU_MAX) {
        TOPO_ERR(("TOPO ERROR: cxn update depth too great\n"));
        return;
    }
#endif /* BROADCOM_DEBUG */

    /*
     * Look for new connections:  new_idx->mid_cpu->dest_idx where
     * new->dest_idx is NOT yet defined; mid_cpu comes from cpu_list.
     */

    for (i = 0; i < cpu_count; i++) {
        mid_cpu = cpu_list[i];
        for (src_idx = 0; src_idx < db_ref->num_cpus; src_idx++) {
            if (src_idx == mid_cpu) {
                continue;
            }
            if (TP_REACHABLE(db_ref, src_idx, mid_cpu)) {
                if (tp_one_cxn_update(db_ref, src_idx, mid_cpu) > 0) {
                    /* Added connections; need to go deeper */
                    new_cpus[new_count++] = src_idx;
                }
            }
        }
    }

    if (new_count > 0) {  /* Recursive call to handle new connections */
        tp_cxns_update(db_ref, new_cpus, new_count);
    }

#if defined(BROADCOM_DEBUG)
    depth--;
#endif /* BROADCOM_DEBUG */
}

/*
 * Add the edge from src_idx to dest_idx using cxn stk_idx and update
 * all connections
 */

STATIC void
tp_edge_add(cpudb_ref_t db_ref, int src_idx, int dest_idx,
            int src_stk_idx, int dest_stk_idx)
{
    uint8 src_idx8;

    TP_TX_CXN(db_ref, src_idx, dest_idx) = src_stk_idx;
    TP_RX_CXN(db_ref, dest_idx, src_idx) = dest_stk_idx;
    tp_one_cxn_update(db_ref, src_idx, dest_idx);

    src_idx8 = src_idx;
    tp_cxns_update(db_ref, &src_idx8, 1);
}

/* Process all edges that are forced enabled */
STATIC int
tp_forced_up(cpudb_ref_t db_ref)
{
    int i;
    int src_idx, dest_idx;      /* CPUs */
    cpudb_entry_t *src_entry, *dest_entry;
    cpudb_stk_port_t *sp;

    CPUDB_FOREACH_ENTRY(db_ref, src_entry) {
        src_idx = src_entry->topo_idx;
        if (src_idx < 0) {
            LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                      (BSL_META("TOPO ERROR: Could not find src key "
                                CPUDB_KEY_FMT_EOLN),
                       CPUDB_KEY_DISP(src_entry->base.key)));
            return BCM_E_FAIL;
        }

        for (i = 0; i < src_entry->base.num_stk_ports; i++) {
            sp = &src_entry->sp_info[i];
            if (sp->flags & CPUDB_SPF_INACTIVE) {
                continue;
            }
            if (sp->flags & CPUDB_SPF_TX_ENABLED) { /* Forced enabled */
                CPUDB_KEY_SEARCH(db_ref, sp->tx_cpu_key, dest_entry);
                if (dest_entry == NULL) {
                    LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                              (BSL_META("TOPO ERROR: Could not find dest key "
                                        CPUDB_KEY_FMT_EOLN),
                               CPUDB_KEY_DISP(sp->tx_cpu_key)));
                    return BCM_E_FAIL;
                }

                dest_idx = dest_entry->topo_idx;
                if (TP_REACHABLE(db_ref, src_idx, dest_idx)) {
                    LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                             (BSL_META("TOPO WARNING: "
                                       "Multiple paths in pre-enabled links: "
                                       CPUDB_KEY_FMT " to " CPUDB_KEY_FMT_EOLN),
                              CPUDB_KEY_DISP(src_entry->base.key),
                              CPUDB_KEY_DISP(sp->tx_cpu_key)));
                }
                tp_edge_add(db_ref, src_idx, dest_idx, i, sp->tx_stk_idx);
            }
        }
    }

    return BCM_E_NONE;
}

/* Process all edges that are neither already enabled, nor forced disabled. */

STATIC void
tp_process_edges(cpudb_ref_t db_ref)
{
    int i;
    int src_idx, dest_idx;      /* CPUs */
    cpudb_entry_t *src_entry, *dest_entry;
    cpudb_stk_port_t *sp;

    /* Now process all remaining edges */
    CPUDB_FOREACH_ENTRY(db_ref, src_entry) {
        src_idx = src_entry->topo_idx;
        for (i = 0; i < src_entry->base.num_stk_ports; i++) {
            sp = &src_entry->sp_info[i];
            if (sp->flags == 0) {    /* no info on port */
                continue;
            }
            if (sp->flags & (
                             CPUDB_SPF_TX_ENABLED |       /* Already enabled */
                             CPUDB_SPF_TX_DISABLE_FORCE | /* disabled */
                             CPUDB_SPF_NO_LINK |          /* no link on port */
                             CPUDB_SPF_INACTIVE |         /* no pkts on port */
                             CPUDB_SPF_ETHERNET) ) {      /* Ethernet port */
                continue;
            }
            CPUDB_KEY_SEARCH(db_ref, sp->tx_cpu_key, dest_entry);
            if (dest_entry == NULL) {
                LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                         (BSL_META("TOPO WARNING: Could not find tx key "
                                   CPUDB_KEY_FMT_EOLN),
                          CPUDB_KEY_DISP(sp->tx_cpu_key)));
                continue;
            }
            dest_idx = dest_entry->topo_idx;
            if (!TP_REACHABLE(db_ref, src_idx, dest_idx)) { /* Add the edge */
                sp->flags |= CPUDB_SPF_TX_ENABLED;
                tp_edge_add(db_ref, src_idx, dest_idx, i, sp->tx_stk_idx);
            }
        }
    }
}

/*
 * find the paths from each cpu to every other cpu
 * (first path version)
 */
STATIC int
tp_find_paths(cpudb_ref_t db_ref)
{
    int rv;

    rv = tp_forced_up(db_ref);
    if (rv < 0) {
        return rv;
    }
    tp_process_edges(db_ref);
    return BCM_E_NONE;
}

#endif  /* !TOPO_NO_SHORTEST_PATH */

/*
 * CUT PORTS:  Identify edges not in a spanning tree.
 *
 * In SL stacked systems, we have the ability to block broadcast,
 * multicast and DLF traffic (collectively BC/MC) on a per-port
 * basis.  This is used to handle the case of BC/MC traffic in
 * duplex rings where infinite loops will occur if all ports are
 * enabled.  In such rings, a "break point" is determined,
 * and at that CPU, a port in the ring is specified as a "cut"
 * port.  All BC/MC traffic from a cut port is discarded and 
 * no BC/MC traffic should egress on the cut port.
 *
 * In fact, both ends of the connection can be marked as cut
 * ports saving some bandwidth.
 *
 * This approach supports rings, stars, interconnected rings and
 * even rings of rings.  Note that unicast traffic can still go
 * on a shortest path route (so long as the HW supports blocking
 * BC/MC separately from unicast).
 *
 * Determining cut ports is actually very easy:  Generate a spanning
 * tree of the graph (by doing a depth first traversal).  The
 * stack ports on edges not in the spanning tree are cut ports.
 *
 * This routine assumes that basic connectivity is okay (all boxes
 * reachable).  It only supports all links being duplex, ignoring
 * simplex links.  It will work for some configurations that
 * mix simplex and duplex links.  (Identify simplex rings to
 * a vertex leaving a graph with duplex edges; that graph
 * must be connected.)
 */

/* Use the reserved TOPO1 flags in the CPU DB */
#define CPU_VISITED(entry) ((entry)->flags & CPUDB_F_TOPO1)
#define CPU_VISITED_SET(entry) ((entry)->flags |= CPUDB_F_TOPO1)
#define CPU_NOT_VISITED_SET(entry) ((entry)->flags &= ~CPUDB_F_TOPO1)

/*
 * Start by assuming all edges are out of the tree (CUT flag is
 * set).  Clear cut flag (at both ends) when an edge is added.
 * Once spanning tree is complete, it's all done (the cut ports
 * are appropriately marked).
 *
 * Given this logic, it's a little easier to talk about UNUSED edges.
 */

#define EDGE_NOT_USED(sp) ((sp)->flags & CPUDB_SPF_CUT_PORT)
#define EDGE_NOT_USED_SET(sp) ((sp)->flags |= CPUDB_SPF_CUT_PORT)
#define EDGE_USED_SET(sp) ((sp)->flags &= ~CPUDB_SPF_CUT_PORT)

/* Depth first traversal */

STATIC int
depth_first(cpudb_ref_t db_ref, cpudb_entry_t *entry)
{
    cpudb_entry_t *other_end;
    cpudb_stk_port_t *sp, *sp_other_end;
    int i;

    CPU_VISITED_SET(entry);

    for (i = 0; i < entry->base.num_stk_ports; i++) {
        sp = &entry->sp_info[i];

        /* Only pay attention to full duplex ports */
        if (!(sp->flags & CPUDB_SPF_DUPLEX)) {
            continue;
        }

        if (EDGE_NOT_USED(sp)) {
            CPUDB_KEY_SEARCH(db_ref, sp->tx_cpu_key, other_end);
            if (other_end == NULL) {
                LOG_ERROR(BSL_LS_TKS_TOPOLOGY,
                          (BSL_META("TOPO ERROR: Could not find TX key "
                                    CPUDB_KEY_FMT_EOLN),
                           CPUDB_KEY_DISP(sp->tx_cpu_key)));
                return -1;
            }
            if (!(CPU_VISITED(other_end))) {
                /* Mark both ends of edge as used */
                sp_other_end = &(other_end->sp_info[sp->tx_stk_idx]);
                EDGE_USED_SET(sp);
                EDGE_USED_SET(sp_other_end);

                /* Recursive search */
                depth_first(db_ref, other_end);
            }
        }
    }

    return 0;
}

STATIC int
tp_find_cut_ports(cpudb_ref_t db_ref)
{
    cpudb_entry_t *entry;
    int i;
    cpudb_stk_port_t *sp;

    /* Clear visited and edge use flags */
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        CPU_NOT_VISITED_SET(entry);
        for (i = 0; i < entry->base.num_stk_ports; i++) {
            sp = &(entry->sp_info[i]);
            if (sp->flags & CPUDB_SPF_DUPLEX) {
                EDGE_NOT_USED_SET(sp);
            } else { /* All simplex edges should be marked used (not cut) */
                EDGE_USED_SET(sp);
            }
        }
    }

    /* Start with master entry */
    return depth_first(db_ref, db_ref->master_entry);
}


/*
 * Function:
 *      topology_create
 * Purpose:
 *      Generate the system topology for the given DB
 * Parameters:
 *      db_ref           - DB to update
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Default behavior is to use shortest path algorithm.  Alternative
 *      allows forcing some links up.
 */

int
topology_create(cpudb_ref_t db_ref)
{
    int bytes, rv;

    if (db_ref == CPUDB_REF_NULL || db_ref->local_entry == NULL) {
        return BCM_E_PARAM;
    }

    TOPO_LOCK_INIT;
    TOPO_LOCK_CHECK;

    TOPO_LOCK;
    if (db_ref->topo_cookie != NULL) {
        LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META("TOPO WARNING: structure already active\n")));
        _topology_destroy(db_ref);
    }

    db_ref->topo_cookie = sal_alloc(sizeof(topo_info_t), "topo_create");
    if (db_ref->topo_cookie == NULL) {
        TOPO_UNLOCK;
        return BCM_E_MEMORY;
    }
    sal_memset(db_ref->topo_cookie, 0, sizeof(topo_info_t));

    tp_index_init(db_ref);   /* Set up internal CPU indexes */

    if (db_ref->num_cpus < 2) {  /* Only 1 CPU; standalone */
        TOPO_UNLOCK;
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META("TOPO: Single CPU mode\n")));
        return BCM_E_NONE;
    }

    /* Set up topo connection matrices */
    bytes = db_ref->num_cpus * db_ref->num_cpus;
    TP_TX_CXN_MATRIX(db_ref) = sal_alloc(bytes, "topo_tx_matrix");
    if (TP_TX_CXN_MATRIX(db_ref) == NULL) {
        _topology_destroy(db_ref);
        TOPO_UNLOCK;
        return BCM_E_MEMORY;
    }
    TP_TX_CXN_INIT(db_ref, bytes);

    TP_RX_CXN_MATRIX(db_ref) = sal_alloc(bytes, "topo_rx_matrix");
    if (TP_RX_CXN_MATRIX(db_ref) == NULL) {
        _topology_destroy(db_ref);
        TOPO_UNLOCK;
        return BCM_E_MEMORY;
    }
    TP_RX_CXN_INIT(db_ref, bytes);
    
    if (tp_find_cut_ports(db_ref) < 0) {
        LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META("TOPO WARNING: Spanning-tree/cut-port failed\n")));
    }
    
    rv = tp_find_paths(db_ref);
    if (rv < 0) {
        _topology_destroy(db_ref);
        TOPO_UNLOCK;
        return rv;
    }

    if (!tp_all_destinations_reachable(db_ref)) {
        _topology_destroy(db_ref);
        TOPO_UNLOCK;
        LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META("TOPO WARNING: Cannot reach all CPUs.\n")));
        return BCM_E_FAIL;
    }

    TOPO_UNLOCK;
    return BCM_E_NONE;
}

/*
 * Function:
 *      topology_mod_id_assign_function
 * Purpose:
 *      Sets the mod id assign funtion to the parameter. If null,
 *      generic_topology_mod_ids_assign() will be used.
 * Parameters:
 *      func -- (IN)
 * Returns:
 *      BCM_E_XXX
 */
int
topology_mod_id_assign_function(topology_mod_id_assign_f func)
{
    topology_mod_id_assign_func = func;

    return BCM_E_NONE;
}

/*
 * Function:
 *      topo_reserved_modid_set
 * Purpose:
 *      Reserves modid from the available pool, that must not
 *      be used for assigning to devices during stack topology
 * Parameters:
 *      modid_in_use -- (IN)
 * Returns:
 *      BCM_E_XXX
 */
int
topo_reserved_modid_set(uint32 modid_in_use, int enable)
{
                                                                                       
    if (enable) {
        topo_rsvd_modid |= modid_in_use;
    } else {
        topo_rsvd_modid &= ~modid_in_use;
    }
    return BCM_E_NONE;
}
                                                                                       
/*
 * Function:
 *      topo_reserved_modid_get
 * Purpose:
 *      Get the current bitmap of reserved modid's
 * Parameters:
 *      modid_in_use -- (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int
topo_reserved_modid_get(uint32 *modid_in_use)
{
  *modid_in_use = topo_rsvd_modid;
                                                                                       
  return BCM_E_NONE;
}

/*
 * Function:
 *      generic_topology_mod_ids_assign
 * Purpose:
 *      Internal function to assign MODIDs
 * Notes:
 *      First, try to put all mod ids in by old settings or preference.
 *      Priority given to the value the entry's unit had in old CPU DB;
 *      then to the value in entry->base.pref_mod_id[unit]. After assigning
 *      these, look for any mod id where it fits.
 */



/* This is restricted to 32 module ID systems */
#define MOD_FREE(_inuse, _mod, _mask) \
    (((_inuse) & ((_mask) << (_mod))) == 0)

STATIC int
generic_topology_mod_ids_assign(cpudb_ref_t db_ref)
{
    uint32 modinuse, trymask;
    int oldmod, prefmod, mod;
    cpudb_entry_t *entry, *oent;
    char keybuf[CPUDB_KEY_STRING_LEN];
    int unit;
    int rv = BCM_E_NONE;

    modinuse = topo_rsvd_modid;

    /* Assign as per old CPUDB if present */
    if (cpudb_valid(db_ref->old_db)) {
        CPUDB_FOREACH_ENTRY(db_ref, entry) {
            CPUDB_KEY_SEARCH(db_ref->old_db, entry->base.key, oent);
            if (oent == NULL) {   /* No entry in old db */
                continue;
            }
            for (unit = 0; unit < entry->base.num_units; unit++) {
                if (entry->base.mod_ids_req[unit] <= 0) {
                    continue;
                }
                trymask = (1 << entry->base.mod_ids_req[unit]) - 1;
                oldmod = oent->mod_ids[unit];
                if (oldmod >= 0 && MOD_FREE(modinuse, oldmod, trymask)) {
                    modinuse |= (trymask << oldmod);
                    entry->mod_ids[unit] = oldmod;
                }
            }
        }
    }

    /* Assign as per preference in current DB */
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        for (unit = 0; unit < entry->base.num_units; unit++) {
            if (entry->mod_ids[unit] >= 0 ||    /* Already assigned */
                    entry->base.mod_ids_req[unit] <= 0) {  /* None needed */
                continue;
            }
            trymask = (1 << entry->base.mod_ids_req[unit]) - 1;
            prefmod = entry->base.pref_mod_id[unit];
            if (prefmod >= 0 && MOD_FREE(modinuse, prefmod, trymask)) {
                modinuse |= (trymask << prefmod);
                entry->mod_ids[unit] = prefmod;
            }
        }
    }

    /* Now, fit in anywhere */
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        for (unit = 0; unit < entry->base.num_units; unit++) {
            if (entry->mod_ids[unit] >= 0 ||    /* Already assigned */
                    entry->base.mod_ids_req[unit] <= 0) {  /* None needed */
                continue;
            }
            for (mod = 0; mod <= (32 - entry->base.mod_ids_req[unit]); mod++) {
                
                if ((mod & 1) != 0 &&
                    entry->base.pref_mod_id[unit] == CPUDB_TOPO_MODID_EVEN) {
                    /* If the unit's modid needs to be even, and 'mod'
                       is odd, then skip this modid. */
                    continue;
                }
                
                trymask = (1 << entry->base.mod_ids_req[unit]) - 1;
                if (MOD_FREE(modinuse, mod, trymask)) {
                    modinuse |= (trymask << mod);
                    entry->mod_ids[unit] = mod;
                    break;
                }
            }

            if (entry->mod_ids[unit] < 0) {
                cpudb_key_format(entry->base.key, keybuf, sizeof(keybuf));
                LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                         (BSL_META_U(unit,
                         "TOPO WARN: no available modids for cpu %s, "
                          "unit %d (needs %d)\n"), keybuf, unit,
                          entry->base.mod_ids_req[unit]));
                rv = BCM_E_RESOURCE;
            }
        }
    }

    return rv;
}

#undef MOD_FREE

/*
 * Function:
 *      topology_mod_ids_assign
 * Purpose:
 *      Assign module IDs for each unit in the system
 * Parameters:
 *      db_ref     -- (IN/OUT) DB reference to update
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *
 *      This routine does not program any hardware; it only updates the
 *      DB.
 */
int
topology_mod_ids_assign(cpudb_ref_t db_ref)
{
    int unit;
    cpudb_entry_t *entry;

    /* Clear all mod ids */
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        for (unit = 0; unit < entry->base.num_units; unit++) {
            entry->mod_ids[unit] = -1;
        }
    }

    return topology_mod_id_assign_func ?
        topology_mod_id_assign_func(db_ref) :
        generic_topology_mod_ids_assign(db_ref);
}

/*
 * Function:
 *      _topology_destroy
 * Purpose:
 *      Internal function to de-allocate topology info from a CPU DB.
 */

STATIC void
_topology_destroy(cpudb_ref_t db_ref)
{
    if (db_ref->topo_cookie != NULL) {
        if (TP_TX_CXN_MATRIX(db_ref) != NULL) {
            sal_free(TP_TX_CXN_MATRIX(db_ref));
        }
        if (TP_RX_CXN_MATRIX(db_ref) != NULL) {
            sal_free(TP_RX_CXN_MATRIX(db_ref));
        }
        sal_free(db_ref->topo_cookie);
        db_ref->topo_cookie = NULL;
    }
}

/*
 * Function:
 *      topology_destroy
 * Purpose:
 *      De-allocate the topology associated with a DB reference
 * Parameters:
 *      db_ref      - The DB to update
 * Returns:
 *      BCM_E_XXX
 */

int
topology_destroy(cpudb_ref_t db_ref)
{
    TOPO_LOCK_INIT;
    TOPO_LOCK_CHECK;

    TOPO_LOCK;
    _topology_destroy(db_ref);
    TOPO_UNLOCK;

    return BCM_E_NONE;
}

/*
 * Function:
 *      topo_tx_port_get
 * Purpose:
 *      Get the transmit port to send a pkt from src to dest
 * Parameters:
 *      db_ref              - DB reference to use
 *      src_entry           - Source CPU entry
 *      dest_entry          - Destination CPU entry
 *      stk_idx             - (OUT) Stack port index in src_entry to use
 * Returns:
 *      BCM_E_INIT          - Topology DB not initialized
 *      BCM_E_NOT_FOUND     - Could not find source or dest key in db
 *      BCM_E_FAIL          - Connection not found; either dest key is not
 *                            found or connection not possible.
 *      BCM_E_NONE          - Success
 */

int
topo_tx_port_get(cpudb_ref_t db_ref,
                 cpudb_entry_t *src_entry,
                 cpudb_entry_t *dest_entry,
                 int *stk_idx)
{
    uint8 cxn;

    if (db_ref->topo_cookie == NULL) {
        /* Doesn't look like analysis is done */
        return BCM_E_INIT;
    }

    cxn = TP_TX_CXN(db_ref, src_entry->topo_idx, dest_entry->topo_idx);
    if (cxn == TOPO_CXN_UNKNOWN) {
        return BCM_E_FAIL;
    }

    *stk_idx = cxn;
    return BCM_E_NONE;
}
